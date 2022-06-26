#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-qt6-base \
$MINGW_PACKAGE_PREFIX-qt6-5compat \
2>/dev/null

exitOnError
}

function build(){
if [ -e $PREFIX/bin/libquazip1-qt6.dll -a $((FORCE_INSTALL)) == 0 ]; then
echo "QuaZip is already installed."
exit 0
fi

QUAZIP_VERSION=1.2
QUAZIP_TAG=v$QUAZIP_VERSION
QUAZIP_ARCHIVE=quazip-$QUAZIP_TAG.tar.gz
QUAZIP_SRC_DIR=quazip-$QUAZIP_VERSION
QUAZIP_BUILD_DIR=$QUAZIP_SRC_DIR-$BIT

if [ ! -e $QUAZIP_ARCHIVE ]; then
wget -c https://github.com/stachenov/quazip/archive/$QUAZIP_TAG/$QUAZIP_ARCHIVE
fi
rm -rf $QUAZIP_SRC_DIR $QUAZIP_BUILD_DIR 
tar xf $QUAZIP_ARCHIVE
mv $QUAZIP_SRC_DIR $QUAZIP_BUILD_DIR
pushd $QUAZIP_BUILD_DIR

mkdir build
pushd build
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_INSTALL_PREFIX=$PREFIX \
-DQUAZIP_QT_MAJOR_VERSION=6

exitOnError

makeParallel && make install
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
