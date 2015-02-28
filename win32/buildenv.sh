function exitOnError(){
if [ $? -ne 0 ]; then 
    exit
else
    echo "success"
fi
}

function waitEnter(){
echo "Hit Enter"
read Wait
}

function buildQtCreator(){
#shared版(QtCreator用)
rm -rf qt5-shared
mkdir qt5-shared
pushd qt5-shared

cmd.exe /c "%CD%/../$QT_SOURCE_DIR/configure.bat -opensource -confirm-license -platform win32-g++ -prefix %CD%/../../../../usr/local -shared -release -opengl es2 -angle -nomake tests -skip qtwebkit-examples"

#並列ビルドの場合依存関係でビルドに失敗することがあるので2回までmakeする。
PATH=$PWD/qtbase/lib:$PATH $MINGW32MAKE || $MINGW32MAKE && $MINGW32MAKE install && $MINGW32MAKE docs && $MINGW32MAKE install_qch_docs
exitOnError
popd

#qbs
cd ~/extlib
export QBS_VER=1.3.3
export QBS_SOURCE_DIR=qbs-$QBS_VER
wget -c  http://download.qt-project.org/official_releases/qbs/$QBS_VER/$QBS_SOURCE_DIR.src.zip
if [ -e $QBS_SOURCE_DIR ]; then
    # 存在する場合
    echo "$QBS_SOURCE_DIR already exists."
else
    # 存在しない場合
    unzip -q -b -n $QBS_SOURCE_DIR.src.zip
fi
rm -rf qbs
mkdir qbs
pushd qbs

qmake ../$QBS_SOURCE_DIR/qbs.pro
$MINGW32MAKE && INSTALL_ROOT=/usr/local $MINGW32MAKE install 
exitOnError
popd

#Qt Creator
cd ~/extlib
export QTC_MAJOR_VER=3.3
export QTC_MINOR_VER=.1
export QTC_VER=$QTC_MAJOR_VER$QTC_MINOR_VER
export QTC_SOURCE_DIR=qt-creator-opensource-src-$QTC_VER
wget -c  http://download.qt-project.org/official_releases/qtcreator/$QTC_MAJOR_VER/$QTC_VER/$QTC_SOURCE_DIR.zip
if [ -e $QTC_SOURCE_DIR ]; then
    # 存在する場合
    echo "$QTC_SOURCE_DIR already exists."
else
    # 存在しない場合
    unzip -q -b -n $QTC_SOURCE_DIR.zip
    pushd $QTC_SOURCE_DIR
    
    #MinGWでコンパイルが通らない問題の修正
    patch -p1 < $SCRIPT_DIR/qt-creator-3.3.0-MinGW-w64-MIB_TCP_STATE-not-defined-until-Vista.patch
    popd
fi

rm -rf qtcreator
mkdir qtcreator
pushd qtcreator

qmake ../$QTC_SOURCE_DIR/qtcreator.pro
$MINGW32MAKE && INSTALL_ROOT=/usr/local $MINGW32MAKE install 
exitOnError
popd
}

function buildQtStatic(){
#static版(P6VX用)
rm -rf qt5-static
mkdir qt5-static
pushd qt5-static

cmd.exe /c "%CD%/../$QT_SOURCE_DIR/configure.bat -opensource -confirm-license -platform win32-g++ -prefix %CD%/../../../../usr/local/qt5-static -static -opengl es2 -angle  -no-icu -no-openssl -qt-pcre -qt-zlib -qt-libpng -qt-libjpeg -nomake examples -nomake tests -skip qtwebkit-examples"

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
export QT_MAJOR_VERSION=5.4
export QT_MINOR_VERSION=.1
export QT_VERSION=$QT_MAJOR_VERSION$QT_MINOR_VERSION
export QT_SOURCE_DIR=qt-everywhere-opensource-src-$QT_VERSION
wget -c  http://download.qt-project.org/official_releases/qt/$QT_MAJOR_VERSION/$QT_VERSION/single/$QT_SOURCE_DIR.zip

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
export DX_REL_PATH=../../../mingw32/i686-w64-mingw32
export DXSDK_DIR=`pwd -W`/$DX_REL_PATH/
#export DXSDK_DIR="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)"
cp $DX_REL_PATH/lib/libd3dx11.a $DX_REL_PATH/lib/d3d11.lib
mkdir -p $DX_REL_PATH/Utilities/bin/x86
cp "C:\Program Files (x86)\Windows Kits\8.1\bin\x86\fxc.exe" $DX_REL_PATH/Utilities/bin/x86/

#QtCreatorをビルド
buildQtCreator

#static版Qtをビルド(P6VX用)
buildQtStatic




