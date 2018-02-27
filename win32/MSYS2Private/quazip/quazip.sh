#!/bin/bash

function prerequisite(){
#他スクリプト依存関係
if [ $((NO_DEPENDENCY)) == 0 ]; then
$SCRIPT_DIR/../qt/qt.sh
exitOnError
fi
}

function build(){
if [ -e $PREFIX/bin/libquazip5.dll -a $((FORCE_INSTALL)) == 0 ]; then
echo "QuaZip is already installed."
exit 0
fi

QUAZIP_VERSION=0.7.3
QUAZIP_TAG=$QUAZIP_VERSION
QUAZIP_ARCHIVE=quazip-$QUAZIP_TAG.tar.gz
QUAZIP_SRC_DIR=quazip-$QUAZIP_VERSION
QUAZIP_BUILD_DIR=$QUAZIP_SRC_DIR-$BIT

if [ ! -e $QUAZIP_ARCHIVE ]; then
wget -c https://sourceforge.net/projects/quazip/files/quazip/$QUAZIP_TAG/$QUAZIP_ARCHIVE
fi
rm -rf $QUAZIP_SRC_DIR $QUAZIP_BUILD_DIR 
tar xf $QUAZIP_ARCHIVE
mv $QUAZIP_SRC_DIR $QUAZIP_BUILD_DIR
pushd $QUAZIP_BUILD_DIR

mkdir build
pushd build
CMAKE_PREFIX_PATH=$PREFIX/qt5-shared:$CMAKE_PREFIX_PATH \
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_INSTALL_PREFIX=$PREFIX

exitOnError

makeParallel && make install
exitOnError

#dllがlibフォルダにインストールされてしまうので移動
mv $PREFIX/lib/libquazip5.dll $PREFIX/bin/

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
