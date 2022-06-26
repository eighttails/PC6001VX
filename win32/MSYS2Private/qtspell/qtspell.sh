#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-qt6-base \
$MINGW_PACKAGE_PREFIX-qt6-tools \
$MINGW_PACKAGE_PREFIX-enchant \
2>/dev/null

exitOnError
}

function build(){
if [ -e $PREFIX/lib/libqtspell-qt6.a -a $((FORCE_INSTALL)) == 0 ]; then
echo "QtSpell is already installed."
exit 0
fi

QTSPELL_VERSION=1.0.1
QTSPELL_TAG=$QTSPELL_VERSION
QTSPELL_ARCHIVE=qtspell-$QTSPELL_TAG.tar.gz
QTSPELL_SRC_DIR=qtspell-$QTSPELL_VERSION
QTSPELL_BUILD_DIR=$QTSPELL_SRC_DIR-$BIT

if [ ! -e $QTSPELL_ARCHIVE ]; then
wget -c https://github.com/manisandro/qtspell/archive/$QTSPELL_TAG/$QTSPELL_ARCHIVE
fi
rm -rf $QTSPELL_SRC_DIR $QTSPELL_BUILD_DIR 
tar xf $QTSPELL_ARCHIVE
mv $QTSPELL_SRC_DIR $QTSPELL_BUILD_DIR
pushd $QTSPELL_BUILD_DIR

mkdir build
pushd build
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_INSTALL_PREFIX=$PREFIX \
-DBUILD_STATIC_LIBS=1 \
-DQT_VER=6 
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
