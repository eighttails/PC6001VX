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

#このスクリプトの置き場所をカレントとして実行すること。
#カレントディレクトリ
export SCRIPT_DIR=$PWD

cd
mkdir extlib

#Qt
cd ~/extlib
export QT_VERSION=5.2.0
export QT_SOURCE_DIR=qt-everywhere-opensource-src-$QT_VERSION
wget -c  http://download.qt-project.org/official_releases/qt/5.2/$QT_VERSION/single/$QT_SOURCE_DIR.zip

if [ -e $QT_SOURCE_DIR ]; then
    # 存在する場合
    echo "$QT_SOURCE_DIR already exists."
else
    # 存在しない場合
    unzip -q -n $QT_SOURCE_DIR.zip 
    #MSYSでビルドが通らない問題への対策パッチ
    patch -p0 --binary < $SCRIPT_DIR/libGLESv2.patch
fi

export PATH=$PWD/$QT_SOURCE_DIR/gnuwin32/bin:$PATH

#if[ 1 ]; then #ここからコメントアウト

#shared版(QtCreator用)
rm -rf qt5-shared
mkdir qt5-shared
pushd qt5-shared

cmd.exe "/c %CD%/../$QT_SOURCE_DIR/configure.bat -I %CD%/../../../../mingw32/include -L %CD%/../../../../mingw32/lib -L %CD%/../../../../mingw32/i686-w64-mingw32/lib -opensource -confirm-license -platform win32-g++ -prefix %CD%/../../../../local -shared -release -nomake examples -nomake tests -skip qtwebkit-examples"

PATH=$PWD/qtbase/lib:$PATH mingw32-make -j$NUMBER_OF_PROCESSORS && mingw32-make install && mingw32-make docs && mingw32-make install_qch_docs
exitOnError
popd


#Qt Creator
cd ~/extlib
export QTC_VER=3.0.0
export QTC_SOURCE_DIR=qt-creator-opensource-src-$QTC_VER
wget -c  http://download.qt-project.org/official_releases/qtcreator/3.0/$QTC_VER/$QTC_SOURCE_DIR.zip
if [ -e $QTC_SOURCE_DIR ]; then
    # 存在する場合
    echo "$QTC_SOURCE_DIR already exists."
else
    # 存在しない場合
    unzip -q -n $QTC_SOURCE_DIR.zip
fi

rm -rf qtcreator
mkdir qtcreator
pushd qtcreator

qmake ../$QTC_SOURCE_DIR/qtcreator.pro
mingw32-make -j$NUMBER_OF_PROCESSORS && INSTALL_ROOT=/usr/local mingw32-make install 
exitOnError
popd

#fi #if[ 1 ]; then

#static版(P6VX用)
rm -rf qt5-static
mkdir qt5-static
pushd qt5-static

mkdir qtbase
pushd qtbase
cmd.exe "/c %CD%/../../$QT_SOURCE_DIR/qtbase/configure.bat -opensource -confirm-license -platform win32-g++ -prefix %CD%/../../../../../local/qt5-static -static -no-icu -no-openssl -qt-pcre -qt-zlib -qt-libpng -qt-libjpeg -nomake examples -nomake tests"

mingw32-make -j$NUMBER_OF_PROCESSORS && mingw32-make install 
exitOnError

cp lib/libpreprocessor*.a /usr/local/qt5-static/lib
cp lib/libtranslator_*.a /usr/local/qt5-static/lib
popd

mkdir qttools
pushd qttools
/usr/local/qt5-static/bin/qmake ../../$QT_SOURCE_DIR/qttools/qttools.pro
mingw32-make -j$NUMBER_OF_PROCESSORS && mingw32-make install 
exitOnError
popd

mkdir qtmultimedia
pushd qtmultimedia
/usr/local/qt5-static/bin/qmake ../../$QT_SOURCE_DIR/qtmultimedia/qtmultimedia.pro
mingw32-make -j$NUMBER_OF_PROCESSORS && mingw32-make install 
exitOnError
popd

mkdir qttranslations
pushd qttranslations
/usr/local/qt5-static/bin/qmake ../../$QT_SOURCE_DIR/qttranslations/qttranslations.pro
mingw32-make -j$NUMBER_OF_PROCESSORS && mingw32-make install 
exitOnError
popd

popd

#SDL2

