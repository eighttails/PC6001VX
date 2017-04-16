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

function buildLeptonica(){
if [ -e $PREFIX/lib/liblept.a -a $((FORCE_INSTALL)) == 0 ]; then
echo "Leptonica is already installed."
exit 0
fi

LEPTONICA_VERSION=1.74.1
LEPTONICA_SRC_DIR=leptonica-$LEPTONICA_VERSION
LEPTONICA_BUILD_DIR=$LEPTONICA_SRC_DIR-$MINGW_CHOST

wget -c http://www.leptonica.com/source/$LEPTONICA_SRC_DIR.tar.gz
	
rm -rf $LEPTONICA_SRC_DIR $LEPTONICA_BUILD_DIR 
tar xf $LEPTONICA_SRC_DIR.tar.gz
mv $LEPTONICA_SRC_DIR $LEPTONICA_BUILD_DIR
pushd $LEPTONICA_BUILD_DIR

./configure \
--build=$MINGW_CHOST \
--host=$MINGW_CHOST \
--target=$MINGW_CHOST \
--prefix=$PREFIX \
--disable-programs \
CPPFLAGS=-DMINIMUM_SEVERITY=4

makeParallel && makeParallel install

exitOnError
popd
}

#-----------------------------
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
source $SCRIPT_DIR/../common/common.sh
commonSetup
prerequisite

cd $EXTLIB

buildLeptonica
