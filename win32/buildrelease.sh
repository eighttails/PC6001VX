#!/bin/bash

SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
. $SCRIPT_DIR/buildenv.sh
QTDIR=$MINGW_PREFIX/local/qt6-static-private
export PKG_CONFIG_PATH=$MINGW_PREFIX/local/lib/pkgconfig

#並列ビルド
MAKE="make -j$NUMBER_OF_PROCESSORS"

if [ -z $1 ]; then
    cd $SCRIPT_DIR/../../
else
    cd $1 
fi

P6VX_DBUILD_DIR=PC6001VX-build-$MINGW_CHOST

if [ -e $P6VX_DBUILD_DIR ]; then 
	rm -rf $P6VX_DBUILD_DIR
fi
mkdir $P6VX_DBUILD_DIR

cd $P6VX_DBUILD_DIR
$QTDIR/bin/qmake $SCRIPT_DIR/../PC6001VX.pro 
$MAKE release

