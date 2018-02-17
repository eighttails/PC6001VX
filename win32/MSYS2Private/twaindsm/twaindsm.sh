#!/bin/bash

#function prerequisite(){
#}

function build(){
if [ -e $PREFIX/bin/twaindsm.dll -a $((FORCE_INSTALL)) == 0 ]; then
echo "TWAIN DSM is already installed."
exit 0
fi

TWAINDSM_VERSION=2.3.1
TWAINDSM_TAG=$TWAINDSM_VERSION
TWAINDSM_ARCHIVE=twaindsm-$TWAINDSM_TAG.source.zip
TWAINDSM_SRC_DIR=twaindsm-$TWAINDSM_TAG.orig
TWAINDSM_BUILD_DIR=$TWAINDSM_SRC_DIR-$BIT

if [ ! -e $TWAINDSM_ARCHIVE ]; then
wget https://sourceforge.net/projects/twain-dsm/files/TWAIN%20DSM%202%20Source/$TWAINDSM_ARCHIVE
fi
rm -rf $TWAINDSM_SRC_DIR $TWAINDSM_BUILD_DIR 
unzip $TWAINDSM_ARCHIVE
mv $TWAINDSM_SRC_DIR $TWAINDSM_BUILD_DIR
pushd $TWAINDSM_BUILD_DIR


patch -p1 --binary < $SCRIPT_DIR/twaindsm_mingw.patch
pushd TWAIN_DSM/src
mkdir build
pushd build
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_INSTALL_PREFIX=$PREFIX 
exitOnError

makeParallel && make install
exitOnError
popd
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
