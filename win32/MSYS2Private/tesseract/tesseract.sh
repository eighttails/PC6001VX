#!/bin/bash

function prerequisite(){
#他スクリプト依存関係
if [ $((NO_DEPENDENCY)) == 0 ]; then
#依存スクリプトなし
exitOnError
fi

#必要ライブラリ
pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-leptonica \
$MINGW_PACKAGE_PREFIX-libpng \
$MINGW_PACKAGE_PREFIX-libjpeg \
$MINGW_PACKAGE_PREFIX-libtiff \
$MINGW_PACKAGE_PREFIX-zlib \
$MINGW_PACKAGE_PREFIX-libarchive \
$MINGW_PACKAGE_PREFIX-cairo \
$MINGW_PACKAGE_PREFIX-pango \
$MINGW_PACKAGE_PREFIX-icu \
$MINGW_PACKAGE_PREFIX-docbook-xsl \
2>/dev/null

exitOnError
}

function build(){
if [ -e $PREFIX/bin/tesseract.exe -a $((FORCE_INSTALL)) == 0 ]; then
echo "tesseract is already installed."
exit 0
fi


if [ "$TESSERACT_GIT" != "" ]; then
    TESSERACT_SRC_DIR=tesseract-git
    git clone https://github.com/tesseract-ocr/tesseract.git $TESSERACT_SRC_DIR 2> /dev/null
    pushd $TESSERACT_SRC_DIR
    git pull
else
    TESSERACT_VERSION=master
    TESSERACT_TAG=$TESSERACT_VERSION
    TESSERACT_ARCHIVE=tesseract-$TESSERACT_TAG.tar.gz
    TESSERACT_SRC_DIR=tesseract-$TESSERACT_VERSION
    TESSERACT_BUILD_DIR=$TESSERACT_SRC_DIR-$BIT

    if [ "$TESSERACT_VERSION" == "master" ]; then
    rm $TESSERACT_ARCHIVE 2> /dev/null
    fi

    if [ ! -e $TESSERACT_ARCHIVE ]; then
    wget -c https://github.com/tesseract-ocr/tesseract/archive/$TESSERACT_TAG/$TESSERACT_ARCHIVE
    fi
    rm -rf $TESSERACT_SRC_DIR $TESSERACT_BUILD_DIR 
    tar xf $TESSERACT_ARCHIVE
    mv $TESSERACT_SRC_DIR $TESSERACT_BUILD_DIR
    pushd $TESSERACT_BUILD_DIR
fi

#asciidocが動かない問題への暫定対応
sed -i -e 's/AM_CONDITIONAL(\[ASCIIDOC\], true)/AM_CONDITIONAL([ASCIIDOC], false)/' configure.ac

if [ -e Makefile ]; then
make clean
fi

./autogen.sh
exitOnError

if [ "$TESSERACT_DEBUG" != "" ]; then
    echo Building Tessract with DEBUG flags.
    DEBUG_FLAGS="--enable-debug "
fi
./configure \
$DEBUG_FLAGS \
--build=$MINGW_CHOST \
--host=$MINGW_CHOST \
--target=$MINGW_CHOST \
--prefix=$PREFIX \
--with-extra-includes=$PREFIX/include \
--with-extra-libraries=$PREFIX/lib

exitOnError

if [ "$TESSERACT_DEBUG" != "" ]; then
    #O0以外でビルドされるとデバッグが困難な局面があるため修正
    sed -i -e 's/Og/O0/g' Makefile
    sed -i -e 's/O2/O0/g' Makefile
fi

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
