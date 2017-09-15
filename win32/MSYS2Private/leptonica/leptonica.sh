#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman -S --needed --noconfirm \
$MINGW_PACKAGE_PREFIX-giflib \
$MINGW_PACKAGE_PREFIX-libpng \
$MINGW_PACKAGE_PREFIX-libjpeg \
$MINGW_PACKAGE_PREFIX-openjpeg2 \
$MINGW_PACKAGE_PREFIX-libtiff \
$MINGW_PACKAGE_PREFIX-libwebp \
$MINGW_PACKAGE_PREFIX-zlib
}

function build(){
if [ -e $PREFIX/lib/liblept.a -a $((FORCE_INSTALL)) == 0 ]; then
echo "Leptonica is already installed."
exit 0
fi

LEPTONICA_VERSION=1.74.4
LEPTONICA_SRC_DIR=leptonica-$LEPTONICA_VERSION
LEPTONICA_SRC_ARCHIVE=$LEPTONICA_SRC_DIR.tar.gz
LEPTONICA_BUILD_DIR=$LEPTONICA_SRC_DIR-$MINGW_CHOST

if [ ! -e $LEPTONICA_SRC_ARCHIVE ]; then
wget -c https://github.com/DanBloomberg/leptonica/releases/download/$LEPTONICA_VERSION/$LEPTONICA_SRC_ARCHIVE
fi

rm -rf $LEPTONICA_SRC_DIR $LEPTONICA_BUILD_DIR 
tar xf $LEPTONICA_SRC_ARCHIVE
mv $LEPTONICA_SRC_DIR $LEPTONICA_BUILD_DIR
pushd $LEPTONICA_BUILD_DIR

mkdir build
pushd build
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_INSTALL_PREFIX=$PREFIX 

exitOnError

makeParallel && makeParallel install

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
