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

    #プリコンパイル済みヘッダーが巨大すぎでビルドが通らない問題へのパッチ
    #sed -i -e "s| precompile_header||g" qtbase/mkspecs/win32-g++/qmake.conf    
    
    #Qt5.8.0でMultimediaのヘッダーがおかしい問題へのパッチ
    rm qtmultimedia/include/QtMultimedia/qtmultimediadefs.h
    touch qtmultimedia/include/QtMultimedia/qtmultimediadefs.h
    
    popd
fi

#共通ビルドオプション
export QT_COMMON_CONFIGURE_OPTION='-opensource -confirm-license -silent -platform win32-g++ -no-direct2d -nomake tests'
}

function buildQtShared(){
#shared版(QtCreator用)
rm -rf qt5-shared
mkdir qt5-shared
pushd qt5-shared

../$QT_SOURCE_DIR/configure -prefix "`cygpath -am ~/qt-creator`" -shared -release $QT_COMMON_CONFIGURE_OPTION

makeParallel && makeParallel install && makeParallel docs && makeParallel install_qch_docs
exitOnError
popd
rm -rf qt5-shared
}

function buildQtCreator(){
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
fi

pushd $QTC_SOURCE_DIR
#MSYSでビルドが通らない問題へのパッチ
patch -p1 < $SCRIPT_DIR/qt-creator-3.5.0-shellquote-declspec-dllexport-for-unix-shell.patch
popd

rm -rf qt-creator
mkdir qt-creator
pushd qt-creator

~/qt-creator/bin/qmake CONFIG-=precompile_header QTC_PREFIX="`cygpath -am ~/qt-creator`" ../$QTC_SOURCE_DIR/qtcreator.pro
makeParallel && makeParallel install 
exitOnError
popd
rm -rf qt-creator
}

function buildQtStatic(){
#static版(P6VX用)
rm -rf qt5-static
mkdir qt5-static
pushd qt5-static

../$QT_SOURCE_DIR/configure -prefix "`cygpath -am /mingw32/local`" -static -nomake examples $QT_COMMON_CONFIGURE_OPTION

makeParallel && makeParallel install
exitOnError

#MSYS2のlibtiffはliblzmaに依存しているためリンクを追加する
sed -i -e "s|-ltiff|-ltiff -llzma|g" /mingw32/local/plugins/imageformats/qtiff.prl
sed -i -e "s|-ltiff|-ltiff -llzma|g" /mingw32/local/plugins/imageformats/qtiffd.prl

popd
rm -rf qt5-static
}

function buildFFmpeg(){
FFMPEG_VERSION=3.1.1
FFMPEG_SRC_DIR=ffmpeg-$FFMPEG_VERSION
wget -c https://www.ffmpeg.org/releases/$FFMPEG_SRC_DIR.tar.xz 
	
rm -rf $FFMPEG_SRC_DIR
tar xf $FFMPEG_SRC_DIR.tar.xz
pushd $FFMPEG_SRC_DIR

./configure --target-os=mingw32 --prefix=/mingw32/local --enable-small --disable-programs --disable-doc --disable-everything --disable-sdl --disable-iconv --enable-libvpx --enable-encoder=libvpx_vp8 --enable-libvorbis --enable-encoder=libvorbis --enable-muxer=webm --enable-protocol=file

makeParallel && makeParallel install

exitOnError
popd
}



#---------------------------------
#MSYSルート
export MSYS_ROOT=`cygpath -m /`

#このスクリプトの置き場所をカレントとして実行すること。
#カレントディレクトリ
export SCRIPT_DIR=$PWD

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


