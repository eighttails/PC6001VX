#!/bin/bash

function prerequisite(){
#他スクリプト依存関係
if [ $((NO_DEPENDENCY)) == 0 ]; then
$SCRIPT_DIR/../qt/qt.sh
exitOnError
fi

#必要ライブラリ
pacman -S --needed --noconfirm \
$MINGW_PACKAGE_PREFIX-enchant

}

function build(){
if [ -e $PREFIX/bin/gImageReader-qt5 -a $((FORCE_INSTALL)) == 0 ]; then
echo "QtScript is already installed."
exit 0
fi

QTSPELL_VERSION=0.8.2
QTSPELL_TAG=$QTSPELL_VERSION
QTSPELL_ARCHIVE=qtspell-$QTSPELL_TAG.tar.gz
QTSPELL_SRC_DIR=qtspell-$QTSPELL_VERSION
QTSPELL_BUILD_DIR=$QTSPELL_SRC_DIR-$MINGW_CHOST

wget -c https://github.com/manisandro/qtspell/archive/$QTSPELL_TAG/$QTSPELL_ARCHIVE
rm -rf $QTSPELL_SRC_DIR $QTSPELL_BUILD_DIR 
tar xf $QTSPELL_ARCHIVE
mv $QTSPELL_SRC_DIR $QTSPELL_BUILD_DIR
pushd $QTSPELL_BUILD_DIR

mkdir build
pushd build
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_INSTALL_PREFIX=$PREFIX \
-DUSE_QT5=1

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

cd $EXTLIB

build
