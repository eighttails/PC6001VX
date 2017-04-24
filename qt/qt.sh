#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman -S --needed --noconfirm \
$MINGW_PACKAGE_PREFIX-SDL2

mkdir -p $QTCREATOR_PREFIX/bin 2> /dev/null
pushd $MINGW_PREFIX/bin
cp -f $NEEDED_DLLS $QTCREATOR_PREFIX/bin
popd
}

function makeQtSourceTree(){
#Qt
QT_MAJOR_VERSION=5.8
QT_MINOR_VERSION=.0
QT_VERSION=$QT_MAJOR_VERSION$QT_MINOR_VERSION
QT_SOURCE_DIR=qt-everywhere-opensource-src-$QT_VERSION
#QT_RELEASE=development_releases
QT_RELEASE=official_releases

if [ -e $QT_SOURCE_DIR ]; then
    # 存在する場合
    echo "$QT_SOURCE_DIR already exists."
else
    # 存在しない場合
    wget -c  http://download.qt.io/$QT_RELEASE/qt/$QT_MAJOR_VERSION/$QT_VERSION/single/$QT_SOURCE_DIR.zip
    unzip -q $QT_SOURCE_DIR.zip
    #MSYSでビルドが通らない問題への対策パッチ
    pushd $QT_SOURCE_DIR
    sed -i -e "s|/nologo |//nologo |g" qtbase/src/angle/src/libGLESv2/libGLESv2.pro
    sed -i -e "s|/E |//E |g" qtbase/src/angle/src/libGLESv2/libGLESv2.pro
    sed -i -e "s|/T |//T |g" qtbase/src/angle/src/libGLESv2/libGLESv2.pro
    sed -i -e "s|/Fh |//Fh |g" qtbase/src/angle/src/libGLESv2/libGLESv2.pro

    sed -i -e "s|#ifdef __MINGW32__|#if 0|g"  qtbase/src/3rdparty/angle/src/libANGLE/renderer/d3d/d3d11/Query11.cpp

    #Osで最適化するためのパッチ
    sed -i -e "s|= -O2|= -Os|g" qtbase/mkspecs/win32-g++/qmake.conf

    #64bit環境で生成されるオブジェクトファイルが巨大すぎでビルドが通らない問題へのパッチ
    sed -i -e "s|QMAKE_CFLAGS            = |QMAKE_CFLAGS            = -Wa,-mbig-obj |g" qtbase/mkspecs/win32-g++/qmake.conf

    #プリコンパイル済みヘッダーが巨大すぎでビルドが通らない問題へのパッチ
    sed -i -e "s| precompile_header||g" qtbase/mkspecs/win32-g++/qmake.conf

    #Qt5.8.0でMultimediaのヘッダーがおかしい問題へのパッチ
    rm qtmultimedia/include/QtMultimedia/qtmultimediadefs.h
    touch qtmultimedia/include/QtMultimedia/qtmultimediadefs.h

    popd
fi

#共通ビルドオプション
QT_COMMON_CONFIGURE_OPTION='-opensource -confirm-license -silent -platform win32-g++ -no-pch -no-ssse3 -no-direct2d -qt-zlib -qt-libjpeg -qt-libpng -qt-freetype -qt-pcre -qt-harfbuzz -nomake tests QMAKE_CXXFLAGS+=-Wno-deprecated-declarations'
}

function buildQtShared(){
if [ -e $QTCREATOR_PREFIX/bin/qmake.exe -a $((FORCE_INSTALL)) == 0 ]; then
	echo "Qt5 Shared Libs are already installed."
	return 0
fi

#shared版(QtCreator用)
QT5_SHARED_BUILD=qt5-shared-$MINGW_CHOST
rm -rf $QT5_SHARED_BUILD
mkdir $QT5_SHARED_BUILD
pushd $QT5_SHARED_BUILD

../$QT_SOURCE_DIR/configure -prefix "`cygpath -am $QTCREATOR_PREFIX`" -shared -release $QT_COMMON_CONFIGURE_OPTION

./config.status &> ../qt5-shared-$MINGW_CHOST-config.status

makeParallel && makeParallel install && makeParallel docs && makeParallel install_qch_docs
exitOnError
popd
rm -rf $QT5_SHARED_BUILD
}

function buildQtCreator(){
if [ -e $QTCREATOR_PREFIX/bin/qtcreator.exe -a $((FORCE_INSTALL)) == 0 ]; then
	echo "Qt Creator is already installed."
	return 0
fi

#Qt Creator
cd ~/extlib
QTC_MAJOR_VER=4.2
QTC_MINOR_VER=.1
QTC_VER=$QTC_MAJOR_VER$QTC_MINOR_VER
QTC_SOURCE_DIR=qt-creator-opensource-src-$QTC_VER
#QTC_RELEASE=development_releases
QTC_RELEASE=official_releases
wget -c  http://download.qt.io/$QTC_RELEASE/qtcreator/$QTC_MAJOR_VER/$QTC_VER/$QTC_SOURCE_DIR.zip
if [ -e $QTC_SOURCE_DIR ]; then
	# 存在する場合
	echo "$QTC_SOURCE_DIR already exists."
else
	# 存在しない場合
	unzip -q $QTC_SOURCE_DIR.zip

	pushd $QTC_SOURCE_DIR
	#MSYSでビルドが通らない問題へのパッチ
	patch -p1 < $SCRIPT_DIR/qt-creator-3.5.0-shellquote-declspec-dllexport-for-unix-shell.patch
	popd
fi

QTCREATOR_BUILD=qt-creator-$MINGW_CHOST
rm -rf $QTCREATOR_BUILD
mkdir $QTCREATOR_BUILD
pushd $QTCREATOR_BUILD

$QTCREATOR_PREFIX/bin/qmake CONFIG-=precompile_header CONFIG+=silent QTC_PREFIX="`cygpath -am $QTCREATOR_PREFIX`" ../$QTC_SOURCE_DIR/qtcreator.pro
makeParallel && makeParallel install
exitOnError
popd
rm -rf $QTCREATOR_BUILD
}

function buildQtStatic(){
if [ -e $PREFIX/bin/qmake.exe -a $((FORCE_INSTALL)) == 0 ]; then
	echo "Qt5 Static Libs are already installed."
	return 0
fi

#static版(P6VX用)
QT5_STATIC_BUILD=qt5-static-$MINGW_CHOST
rm -rf $QT5_STATIC_BUILD
mkdir $QT5_STATIC_BUILD
pushd $QT5_STATIC_BUILD

../$QT_SOURCE_DIR/configure -prefix "`cygpath -am $PREFIX`" -static -static-runtime -nomake examples $QT_COMMON_CONFIGURE_OPTION

./config.status &> ../qt5-static-$MINGW_CHOST-config.status

makeParallel && makeParallel install
exitOnError

#MSYS2のlibtiffはliblzmaに依存しているためリンクを追加する
sed -i -e "s|-ltiff|-ltiff -llzma|g" $PREFIX/plugins/imageformats/qtiff.prl
sed -i -e "s|-ltiff|-ltiff -llzma|g" $PREFIX/plugins/imageformats/qtiffd.prl

popd
rm -rf $QT5_STATIC_BUILD
}

#----------------------------------------------------
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
source $SCRIPT_DIR/../common/common.sh
commonSetup

#Qt Creatorのインストール場所
QTCREATOR_PREFIX=$MINGW_PREFIX/local/qt-creator

#必要ライブラリ
prerequisite

cd $EXTLIB

#Qtのソースコードを展開
makeQtSourceTree

#shared版Qtをビルド(QtCreator用)
buildQtShared

#QtCreatorをビルド
buildQtCreator

#static版Qtをビルド(P6VX用)
buildQtStatic
