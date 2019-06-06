#!/bin/bash

function prerequisite(){
#他スクリプト依存関係
if [ $((NO_DEPENDENCY)) == 0 ]; then
#依存スクリプトなし
exitOnError
fi
}

function build(){
if [ -e $PREFIX/bin/lha.exe -a $((FORCE_INSTALL)) == 0 ]; then
echo "lhasa is already installed."
exit 0
fi

LHASA_VERSION=0.3.1
LHASA_TAG=v$LHASA_VERSION
LHASA_ARCHIVE=lhasa-$LHASA_VERSION.tar.gz
LHASA_SRC_DIR=lhasa-$LHASA_VERSION
LHASA_BUILD_DIR=$LHASA_SRC_DIR-$BIT

if [ ! -e $LHASA_ARCHIVE ]; then
wget -c https://github.com/fragglet/lhasa/archive/$LHASA_TAG/$LHASA_ARCHIVE
fi
rm -rf $LHASA_SRC_DIR $LHASA_BUILD_DIR 
tar xf $LHASA_ARCHIVE
mv $LHASA_SRC_DIR $LHASA_BUILD_DIR
pushd $LHASA_BUILD_DIR

if [ -e Makefile ]; then
make clean
fi

./autogen.sh
exitOnError

./configure \
--build=$MINGW_CHOST \
--host=$MINGW_CHOST \
--target=$MINGW_CHOST \
--prefix=$PREFIX 

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
