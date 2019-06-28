#!/bin/bash

function prerequisite(){
#他スクリプト依存関係
if [ $((NO_DEPENDENCY)) == 0 ]; then
$SCRIPT_DIR/../qt/qt.sh
exitOnError
fi
    
#必要ライブラリ
pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-cairo \
$MINGW_PACKAGE_PREFIX-curl \
$MINGW_PACKAGE_PREFIX-freetype \
$MINGW_PACKAGE_PREFIX-icu \
$MINGW_PACKAGE_PREFIX-libjpeg \
$MINGW_PACKAGE_PREFIX-libpng \
$MINGW_PACKAGE_PREFIX-libtiff \
$MINGW_PACKAGE_PREFIX-nss \
$MINGW_PACKAGE_PREFIX-openjpeg2 \
$MINGW_PACKAGE_PREFIX-poppler-data \
$MINGW_PACKAGE_PREFIX-zlib

exitOnError
}

function build(){
if [ -e $PREFIX/lib/libpoppler-qt5.dll.a -a $((FORCE_INSTALL)) == 0 ]; then
echo "poppler is already installed."
exit 0
fi

POPPLER_VERSION=0.76.0
POPPLER_TAG=$POPPLER_VERSION
POPPLER_ARCHIVE=poppler-$POPPLER_TAG.tar.xz
POPPLER_SRC_DIR=poppler-$POPPLER_VERSION
POPPLER_BUILD_DIR=$POPPLER_SRC_DIR-$BIT
if [ ! -e $POPPLER_ARCHIVE ]; then
wget -c https://poppler.freedesktop.org/$POPPLER_ARCHIVE
fi

rm -rf $POPPLER_SRC_DIR $POPPLER_BUILD_DIR 
tar xf $POPPLER_ARCHIVE
mv $POPPLER_SRC_DIR $POPPLER_BUILD_DIR
pushd $POPPLER_BUILD_DIR

export PKG_CONFIG_PATH=$PREFIX/qt5-shared/lib/pkgconfig:$PKG_CONFIG_PATH

mkdir build
pushd build
CMAKE_PREFIX_PATH=$PREFIX/qt5-shared:$CMAKE_PREFIX_PATH \
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_INSTALL_PREFIX=$PREFIX \
-DENABLE_XPDF_HEADERS=ON \
-DENABLE_GTK_DOC=OFF 

exitOnError

makeParallel && make install
exitOnError
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
