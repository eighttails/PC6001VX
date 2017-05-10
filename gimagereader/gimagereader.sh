#!/bin/bash

function prerequisite(){
#他スクリプト依存関係
if [ $((NO_DEPENDENCY)) == 0 ]; then
$SCRIPT_DIR/../qt/qt.sh
exitOnError
$SCRIPT_DIR/../qtspell/qtspell.sh
exitOnError
$SCRIPT_DIR/../podofo/podofo.sh
exitOnError
$SCRIPT_DIR/../poppler/poppler.sh
exitOnError
$SCRIPT_DIR/../twaindsm/twaindsm.sh
exitOnError
$SCRIPT_DIR/../tesseract/tesseract.sh
exitOnError
fi

#必要ライブラリ
pacman -S --needed --noconfirm \
$MINGW_PACKAGE_PREFIX-dlfcn

}

function build(){
if [ -e $PREFIX/bin/gImageReader-qt5 -a $((FORCE_INSTALL)) == 0 ]; then
echo "gImageReader is already installed."
exit 0
fi

GIMAGEREADER_VERSION=3.2.1
GIMAGEREADER_TAG=v$GIMAGEREADER_VERSION
GIMAGEREADER_ARCHIVE=gImageReader-$GIMAGEREADER_TAG.tar.gz
GIMAGEREADER_SRC_DIR=gImageReader-$GIMAGEREADER_VERSION
GIMAGEREADER_BUILD_DIR=$GIMAGEREADER_SRC_DIR-$MINGW_CHOST

if [ ! -e $GIMAGEREADER_ARCHIVE ]; then
wget -c https://github.com/manisandro/gImageReader/archive/$GIMAGEREADER_TAG/$GIMAGEREADER_ARCHIVE
fi
rm -rf $GIMAGEREADER_SRC_DIR $GIMAGEREADER_BUILD_DIR 
tar xf $GIMAGEREADER_ARCHIVE
mv $GIMAGEREADER_SRC_DIR $GIMAGEREADER_BUILD_DIR
pushd $GIMAGEREADER_BUILD_DIR

sed -i -e 's/SET(gimagereader_LIBS \(.*\))/SET(gimagereader_LIBS \1 -llept -lpng -ljpeg -lgif -ltiff -lwebp -lopenjp2 -lcrypto -lidn -llzma -lz)/' CMakeLists.txt
sed -i -e 's/-lintl/-lintl -liconv/' CMakeLists.txt
mkdir build
pushd build
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_INSTALL_PREFIX=$PREFIX \
-DCMAKE_VERBOSE_MAKEFILE:BOOL=TRUE \
-DINTERFACE_TYPE=qt5 \
-DCMAKE_EXE_LINKER_FLAGS="-static" 

makeParallel && makeParallel install
exitOnError
popd
popd
}

#-----------------------------
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
source $SCRIPT_DIR/../common/common.sh
commonSetup
prerequisite
exitOnError

cd $EXTLIB

build
exitOnError
