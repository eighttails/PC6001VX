#!/bin/bash

function exitOnError(){
if [ $? -ne 0 ]; then 
    echo "ERROR."
    exit
else
    echo "SUCCESS."
fi
}

function makeParallel(){
#並列ビルドの場合依存関係でビルドに失敗することがあるので3回までmakeする。
for (( i=0; i<3; i++))
do
    mingw32-make -j$NUMBER_OF_PROCESSORS "$@"
    if [ $? -eq 0 ]; then
        return 0
    fi
done
return 1
}

function waitEnter(){
echo "Hit Enter"
read Wait
}

function makeQtSourceTree(){
#Qt
export QT_MAJOR_VERSION=5.8
export QT_MINOR_VERSION=.0
export QT_VERSION=$QT_MAJOR_VERSION$QT_MINOR_VERSION
export QT_SOURCE_DIR=qt-everywhere-opensource-src-$QT_VERSION
#QT_RELEASE=development_releases
QT_RELEASE=official_releases
wget -c  http://download.qt.io/$QT_RELEASE/qt/$QT_MAJOR_VERSION/$QT_VERSION/single/$QT_SOURCE_DIR.zip

if [ -e $QT_SOURCE_DIR ]; then
    # 存在する場合
    echo "$QT_SOURCE_DIR already exists."
else
    # 存在しない場合
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
export QT_COMMON_CONFIGURE_OPTION='-opensource -confirm-license -silent -platform win32-g++ -no-direct2d -nomake tests'
}

function buildQtShared(){
if [ -e $QTCREATOR_PREFIX/bin/qmake.exe ]; then
	echo "Qt5 Shared Libs are already installed."
	return 0
fi

#shared版(QtCreator用)
QT5_SHARED_BUILD=qt5-shared-$MINGW_CHOST
rm -rf $QT5_SHARED_BUILD
mkdir $QT5_SHARED_BUILD
pushd $QT5_SHARED_BUILD

../$QT_SOURCE_DIR/configure -prefix "`cygpath -am $QTCREATOR_PREFIX`" -shared -release $QT_COMMON_CONFIGURE_OPTION

makeParallel && makeParallel install && makeParallel docs && makeParallel install_qch_docs
exitOnError
popd
rm -rf $QT5_SHARED_BUILD
}

function buildQtCreator(){
if [ -e $QTCREATOR_PREFIX/bin/qtcreator.exe ]; then
	echo "Qt Creator is already installed."
	return 0
fi

#Qt Creator
cd ~/extlib
export QTC_MAJOR_VER=4.2
export QTC_MINOR_VER=.1
export QTC_VER=$QTC_MAJOR_VER$QTC_MINOR_VER
export QTC_SOURCE_DIR=qt-creator-opensource-src-$QTC_VER
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

$QTCREATOR_PREFIX/bin/qmake CONFIG-=precompile_header QTC_PREFIX="`cygpath -am $QTCREATOR_PREFIX`" ../$QTC_SOURCE_DIR/qtcreator.pro
makeParallel && makeParallel install 
exitOnError
popd
rm -rf $QTCREATOR_BUILD
}

function buildQtStatic(){
if [ -e $PREFIX/bin/qmake.exe ]; then
	echo "Qt5 Static Libs are already installed."
	return 0
fi

#static版(P6VX用)
QT5_STATIC_BUILD=qt5-static-$MINGW_CHOST
rm -rf $QT5_STATIC_BUILD
mkdir $QT5_STATIC_BUILD
pushd $QT5_STATIC_BUILD

../$QT_SOURCE_DIR/configure -prefix "`cygpath -am $PREFIX`" -static -static-runtime -nomake examples $QT_COMMON_CONFIGURE_OPTION

makeParallel && makeParallel install
exitOnError

#MSYS2のlibtiffはliblzmaに依存しているためリンクを追加する
sed -i -e "s|-ltiff|-ltiff -llzma|g" $PREFIX/plugins/imageformats/qtiff.prl
sed -i -e "s|-ltiff|-ltiff -llzma|g" $PREFIX/plugins/imageformats/qtiffd.prl

popd
rm -rf $QT5_STATIC_BUILD
}

function buildFFmpeg(){
if [ -e $PREFIX/lib/libavcodec.a ]; then
echo "FFMpeg is already installed."
exit 0
fi

FFMPEG_VERSION=3.1.1
FFMPEG_SRC_DIR=ffmpeg-$FFMPEG_VERSION
wget -c https://www.ffmpeg.org/releases/$FFMPEG_SRC_DIR.tar.xz 

rm -rf $FFMPEG_SRC_DIR
tar xf $FFMPEG_SRC_DIR.tar.xz
pushd $FFMPEG_SRC_DIR

./configure --target-os=mingw32 --prefix=$PREFIX --enable-small --disable-programs --disable-doc --disable-everything --disable-sdl --disable-iconv --enable-libvpx --enable-encoder=libvpx_vp8 --enable-libvorbis --enable-encoder=libvorbis --enable-muxer=webm --enable-protocol=file

makeParallel && makeParallel install

exitOnError
popd
}



#---------------------------------
#環境チェック
if [ -z "$MINGW_PREFIX" ]; then
	echo "Please run this script in MinGW 32bit or 64bit shell. (not in MSYS2 shell)"
	exit 1
fi

#このスクリプトの置き場所
export SCRIPT_DIR=$(cd $(dirname $(readlink $0 || echo $0));pwd)

#インストール先(/mingw32/localまたは/mingw64/local)
export PREFIX=$MINGW_PREFIX/local
mkdir -p $PREFIX/bin 2> /dev/null
export QTCREATOR_PREFIX=$MINGW_PREFIX/local/qt-creator
mkdir -p $QTCREATOR_PREFIX/bin 2> /dev/null

#最低限必要なDLLをコピー
pushd $MINGW_PREFIX/bin
if [ "$MINGW_CHOST" = "i686-w64-mingw32" ]; then
	#32bit
	NEEDED_DLLS='libgcc_s_dw2-1.dll libstdc++-6.dll libwinpthread-1.dll zlib1.dll'
else
	#64bit
	NEEDED_DLLS='libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll zlib1.dll'
fi
cp -f $NEEDED_DLLS $PREFIX/bin
cp -f $NEEDED_DLLS $QTCREATOR_PREFIX/bin
popd

#外部依存ライブラリのソース展開先
cd
mkdir extlib 2> /dev/null
cd ~/extlib

#Qtのソースコードを展開
makeQtSourceTree

#shared版Qtをビルド(QtCreator用)
buildQtShared

#QtCreatorをビルド
buildQtCreator

#static版Qtをビルド(P6VX用)
buildQtStatic

#FFmpegをビルド
buildFFmpeg


