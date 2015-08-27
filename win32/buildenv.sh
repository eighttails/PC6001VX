function exitOnError(){
if [ $? -ne 0 ]; then 
    echo "ERROR."
    exit
else
    echo "SUCCESS."
fi
}

function waitEnter(){
echo "Hit Enter"
read Wait
}

function buildQtShared(){
#shared版(QtCreator用)
rm -rf qt5-shared
mkdir qt5-shared
pushd qt5-shared

cmd.exe /c "%CD%/../$QT_SOURCE_DIR/configure.bat -opensource -confirm-license -platform win32-g++ -prefix %CD%/../../../../usr/local -shared -release -nomake tests -skip qtwebkit-examples -skip qtactiveqt"

#並列ビルドの場合依存関係でビルドに失敗することがあるので2回までmakeする。
PATH=$PWD/qtbase/lib:$PATH $MINGW32MAKE || $MINGW32MAKE && $MINGW32MAKE install
exitOnError
popd
rm -rf qt5-shared
}

function buildQtCreator(){
#Qt Creator
cd ~/extlib
export QTC_MAJOR_VER=3.5
export QTC_MINOR_VER=.0
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
    unzip -q -b -n $QTC_SOURCE_DIR.zip
    pushd $QTC_SOURCE_DIR
    
    #MinGWでコンパイルが通らない問題の修正
    patch -p1 < $SCRIPT_DIR/qt-creator-3.3.0-MinGW-w64-MIB_TCP_STATE-not-defined-until-Vista.patch
    patch -p1 < $SCRIPT_DIR/qt-creator-3.5.0-shellquote-declspec-dllexport-for-unix-shell.patch
    patch -p1 < $SCRIPT_DIR/qt-creator-3.5.0-Hacky-fix-for-__GNUC_PREREQ-usage.patch
    popd
fi

rm -rf qtcreator
mkdir qtcreator
pushd qtcreator

qmake ../$QTC_SOURCE_DIR/qtcreator.pro
#並列ビルドの場合依存関係でビルドに失敗することがあるので2回までmakeする。
$MINGW32MAKE || $MINGW32MAKE && INSTALL_ROOT=/usr/local $MINGW32MAKE install 
exitOnError
popd
rm -rf qtcreator
}

function buildQtStatic(){
#static版(P6VX用)
rm -rf qt5-static
mkdir qt5-static
pushd qt5-static

cmd.exe /c "%CD%/../$QT_SOURCE_DIR/configure.bat -opensource -confirm-license -platform win32-g++ -prefix %CD%/../../../../usr/local/qt5-static -static -no-icu -no-openssl -qt-pcre -nomake examples -nomake tests -skip qtwebkit-examples -skip qtactiveqt"

#並列ビルドの場合依存関係でビルドに失敗することがあるので2回までmakeする。
$MINGW32MAKE || $MINGW32MAKE && $MINGW32MAKE install && $MINGW32MAKE docs && $MINGW32MAKE install_qch_docs
exitOnError

#MSYS2のlibtiffはliblzmaに依存しているためリンクを追加する
sed -i -e "s|-ltiff|-ltiff -llzma|g" /usr/local/qt5-static/plugins/imageformats/qtiff.prl
sed -i -e "s|-ltiff|-ltiff -llzma|g" /usr/local/qt5-static/plugins/imageformats/qtiffd.prl

popd
rm -rf qt5-static
}

function buildFFmpeg(){
FFMPEG_VERSION=2.7.2
FFMPEG_SRC_DIR=ffmpeg-$FFMPEG_VERSION
wget -c https://www.ffmpeg.org/releases/$FFMPEG_SRC_DIR.tar.xz 
	
rm -rf $FFMPEG_SRC_DIR
tar xf $FFMPEG_SRC_DIR.tar.xz
pushd $FFMPEG_SRC_DIR

./configure --target-os=mingw32 --prefix=/mingw32 --enable-small --disable-programs --disable-doc --disable-everything --disable-sdl --disable-iconv --enable-libvpx --enable-encoder=libvpx_vp8 --enable-libvorbis --enable-encoder=libvorbis --enable-muxer=webm --enable-protocol=file

#並列ビルドの場合依存関係でビルドに失敗することがあるので2回までmakeする。
$MINGW32MAKE || $MINGW32MAKE && $MINGW32MAKE install

exitOnError
popd
}

#並列ビルド
MINGW32MAKE="mingw32-make -j$NUMBER_OF_PROCESSORS"

#このスクリプトの置き場所をカレントとして実行すること。
#カレントディレクトリ
export SCRIPT_DIR=$PWD

cd
mkdir extlib

#Qt
cd ~/extlib
export QT_MAJOR_VERSION=5.5
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
    unzip -q -b -n $QT_SOURCE_DIR.zip 
    #MSYSでビルドが通らない問題への対策パッチ
    pushd $QT_SOURCE_DIR
    sed -i -e "s|/nologo |//nologo |g" qtbase/src/angle/src/libGLESv2/libGLESv2.pro
    sed -i -e "s|/E |//E |g" qtbase/src/angle/src/libGLESv2/libGLESv2.pro
    sed -i -e "s|/T |//T |g" qtbase/src/angle/src/libGLESv2/libGLESv2.pro
    sed -i -e "s|/Fh |//Fh |g" qtbase/src/angle/src/libGLESv2/libGLESv2.pro

    #MinGWでANGLEがコンパイルできない問題への対策パッチ
    patch -p0 < $SCRIPT_DIR/angle.patch

    #Osで最適化するためのパッチ(サイズ削減のため)
    patch -p0 --binary < $SCRIPT_DIR/optimize.patch
    popd
fi

export PATH=$PWD/$QT_SOURCE_DIR/gnuwin32/bin:$PATH
#ANGLEのコンパイルを通すための対策
#export DXSDK_DIR="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)"

#shared版Qtをビルド(QtCreator用)
buildQtShared

#QtCreatorをビルド
buildQtCreator

#static版Qtをビルド(P6VX用)
buildQtStatic

#FFmpegをビルド
buildFFmpeg


