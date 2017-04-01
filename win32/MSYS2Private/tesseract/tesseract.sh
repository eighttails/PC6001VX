#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman -S --needed --noconfirm \
$MINGW_PACKAGE_PREFIX-libpng \
$MINGW_PACKAGE_PREFIX-libjpeg \
$MINGW_PACKAGE_PREFIX-libtiff \
$MINGW_PACKAGE_PREFIX-zlib \
$MINGW_PACKAGE_PREFIX-cairo \
$MINGW_PACKAGE_PREFIX-pango \
$MINGW_PACKAGE_PREFIX-icu 
}

function buildTesseract(){
if [ -e $PREFIX/bin/tesseract.exe ]; then
echo "tesseract is already installed."
exit 0
fi

TESSERACT_VERSION=4.00.00alpha
TESSERACT_SRC_DIR=tesseract-$TESSERACT_VERSION
TESSERACT_BUILD_DIR=$TESSERACT_SRC_DIR-$MINGW_CHOST

if [ ! -e  $TESSERACT_SRC_DIR.tar.gz ]; then
wget -c https://github.com/tesseract-ocr/tesseract/archive/$TESSERACT_VERSION.tar.gz 
mv $TESSERACT_VERSION.tar.gz $TESSERACT_SRC_DIR.tar.gz
fi

rm -rf $TESSERACT_SRC_DIR $TESSERACT_BUILD_DIR 
tar xf $TESSERACT_SRC_DIR.tar.gz
mv $TESSERACT_SRC_DIR $TESSERACT_BUILD_DIR
pushd $TESSERACT_BUILD_DIR

./autogen.sh

export LIBLEPT_HEADERSDIR=$PREFIX/include/leptonica

./configure \
--build=$MINGW_CHOST \
--host=$MINGW_CHOST \
--target=$MINGW_CHOST \
--prefix=$PREFIX \
--with-extra-includes=$PREFIX/include \
--with-extra-libraries=$PREFIX/lib

makeParallel && makeParallel install
exitOnError
makeParallel training && makeParallel training-install
exitOnError
popd
}

#-----------------------------
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
source $SCRIPT_DIR/../common/common.sh
commonSetup
prerequisite

cd $EXTLIB

buildTesseract
