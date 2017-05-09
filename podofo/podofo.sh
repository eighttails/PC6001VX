#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman -S --needed --noconfirm \
$MINGW_PACKAGE_PREFIX-fontconfig \
$MINGW_PACKAGE_PREFIX-libtiff \
$MINGW_PACKAGE_PREFIX-libidn \
$MINGW_PACKAGE_PREFIX-libjpeg-turbo \
$MINGW_PACKAGE_PREFIX-lua \
$MINGW_PACKAGE_PREFIX-openssl 
}

function build(){
if [ -e $PREFIX/lib/libpodofo.a -a $((FORCE_INSTALL)) == 0 ]; then
echo "podofo is already installed."
exit 0
fi

#PODOFO_VERSION=4.00.00alpha
PODOFO_VERSION=0.9.5
PODOFO_TAG=$PODOFO_VERSION
PODOFO_ARCHIVE=podofo-$PODOFO_TAG.tar.gz
PODOFO_SRC_DIR=podofo-$PODOFO_TAG
PODOFO_BUILD_DIR=$PODOFO_SRC_DIR-$MINGW_CHOST


wget -c https://sourceforge.net/projects/podofo/files/podofo/$PODOFO_VERSION/$PODOFO_ARCHIVE
rm -rf $PODOFO_SRC_DIR $PODOFO_BUILD_DIR 
tar xf $PODOFO_ARCHIVE
mv $PODOFO_SRC_DIR $PODOFO_BUILD_DIR
pushd $PODOFO_BUILD_DIR

patch -p1 < $SCRIPT_DIR/win.patch
patch -p1 < $SCRIPT_DIR/test.patch

mkdir build
pushd build
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_INSTALL_PREFIX=$PREFIX \
-DFREETYPE_INCLUDE_DIR=${MINGW_PREFIX}/include/freetype2 \
-DCMAKE_BUILD_TYPE=Release \
-DPODOFO_BUILD_STATIC=1 \
-DPODOFO_HAVE_JPEG_LIB=1 \
-DPODOFO_HAVE_PNG_LIB=1 \
-DPODOFO_HAVE_TIFF_LIB=1 


exitOnError

makeParallel && makeParallel install
exitOnError
popd
popd
}

#-----------------------------
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
source $SCRIPT_DIR/../common/common.sh
commonSetup
#prerequisite

cd $EXTLIB

build
exitOnError
