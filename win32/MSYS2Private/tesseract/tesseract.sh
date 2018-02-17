#!/bin/bash

function prerequisite(){
#他スクリプト依存関係
if [ $((NO_DEPENDENCY)) == 0 ]; then
#依存スクリプトなし
exitOnError
fi

#必要ライブラリ
pacman -S --needed --noconfirm \
$MINGW_PACKAGE_PREFIX-leptonica \
$MINGW_PACKAGE_PREFIX-libpng \
$MINGW_PACKAGE_PREFIX-libjpeg \
$MINGW_PACKAGE_PREFIX-libtiff \
$MINGW_PACKAGE_PREFIX-zlib \
$MINGW_PACKAGE_PREFIX-cairo \
$MINGW_PACKAGE_PREFIX-pango \
$MINGW_PACKAGE_PREFIX-icu 
}

function build(){
if [ -e $PREFIX/bin/tesseract.exe -a $((FORCE_INSTALL)) == 0 ]; then
echo "tesseract is already installed."
exit 0
fi

#TESSERACT_VERSION=4.00.00alpha
TESSERACT_SRC_DIR=tesseract-$BIT

if [ ! -e  $TESSERACT_SRC_DIR ]; then
git clone https://github.com/tesseract-ocr/tesseract.git $TESSERACT_SRC_DIR
fi

pushd $TESSERACT_SRC_DIR
git pull

if [ -e Makefile ]; then
make clean
fi

./autogen.sh
exitOnError

export LIBLEPT_HEADERSDIR=$PREFIX/include/leptonica

./configure \
--build=$MINGW_CHOST \
--host=$MINGW_CHOST \
--target=$MINGW_CHOST \
--prefix=$PREFIX \
--with-extra-includes=$PREFIX/include \
--with-extra-libraries=$PREFIX/lib

exitOnError

#WindowsでScrollView.jarのビルドが通らないのを修正
#sed -i -e "s|piccolo2d-core-3.0.jar:piccolo2d-extras-3.0.jar|'piccolo2d-core-3.0.jar;piccolo2d-extras-3.0.jar'|" java/Makefile
#makeParallel ScrollView.jar
#exitOnError
makeParallel training && make training-install
exitOnError
makeParallel && make install
exitOnError
popd
}

#-----------------------------
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
source $SCRIPT_DIR/../common/common.sh
commonSetup
#checkJDK
prerequisite
exitOnError

cd $EXTLIB

build
exitOnError
