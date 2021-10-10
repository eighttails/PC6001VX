#!/bin/bash

function prerequisite(){
#他スクリプト依存関係
if [ $((NO_DEPENDENCY)) == 0 ]; then
$SCRIPT_DIR/../qtspell/qtspell.sh
exitOnError
$SCRIPT_DIR/../twaindsm/twaindsm.sh
exitOnError
$SCRIPT_DIR/../tesseract/tesseract.sh
exitOnError
fi

#必要ライブラリ
pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-qt5 \
$MINGW_PACKAGE_PREFIX-poppler \
$MINGW_PACKAGE_PREFIX-quazip \
$MINGW_PACKAGE_PREFIX-djvulibre \
$MINGW_PACKAGE_PREFIX-podofo \
$MINGW_PACKAGE_PREFIX-dlfcn \
2>/dev/null

exitOnError
}

function build(){
if [ "$GIMAGEREADER_PREFIX" == "" ]; then
GIMAGEREADER_PREFIX=$PREFIX
fi

if [ -e $GIMAGEREADER_PREFIX/bin/gImageReader-qt5 -a $((FORCE_INSTALL)) == 0 ]; then
echo "gImageReader is already installed."
exit 0
fi

if [ "$GIMAGEREADER_GIT" != "" ]; then
    GIMAGEREADER_SRC_DIR=gImageReader-git
    git clone https://github.com/manisandro/gImageReader.git $GIMAGEREADER_SRC_DIR 2> /dev/null
    pushd $GIMAGEREADER_SRC_DIR
    # git pull
    GIMAGEREADER_BUILD_DIR=$GIMAGEREADER_SRC_DIR
else
    GIMAGEREADER_VERSION=master
    if [ "$GIMAGEREADER_VERSION" == "master" ]; then
        GIMAGEREADER_TAG=$GIMAGEREADER_VERSION
        GIMAGEREADER_ARCHIVE=gImageReader-$GIMAGEREADER_TAG.tar.gz
        rm $GIMAGEREADER_ARCHIVE 2> /dev/null
    else
        GIMAGEREADER_TAG=v$GIMAGEREADER_VERSION
        GIMAGEREADER_ARCHIVE=gImageReader-$GIMAGEREADER_TAG.tar.gz
    fi
    GIMAGEREADER_SRC_DIR=gImageReader-$GIMAGEREADER_VERSION
    GIMAGEREADER_BUILD_DIR=$GIMAGEREADER_SRC_DIR-$BIT

    if [ ! -e $GIMAGEREADER_ARCHIVE ]; then
    wget -c https://github.com/manisandro/gImageReader/archive/$GIMAGEREADER_TAG/$GIMAGEREADER_ARCHIVE
    fi
    rm -rf $GIMAGEREADER_SRC_DIR $GIMAGEREADER_BUILD_DIR 
    tar xf $GIMAGEREADER_ARCHIVE
    mv $GIMAGEREADER_SRC_DIR $GIMAGEREADER_BUILD_DIR
    pushd $GIMAGEREADER_BUILD_DIR
fi

#gImageReaderのデフォルト設定を変更
patchOnce 1 $SCRIPT_DIR/customsettings.patch

rm -rf build
mkdir build
pushd build
if [ "$GIMAGEREADER_DEBUG" != "" ]; then
    echo Building gImageReader with DEBUG flags.
    BUILD_TYPE=Debug
else
    BUILD_TYPE=Release
fi
CMAKE_PREFIX_PATH=$PREFIX/qt5-shared:$CMAKE_PREFIX_PATH \
cmake .. \
-G"MSYS Makefiles" \
-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
-DCMAKE_INSTALL_PREFIX=$GIMAGEREADER_PREFIX \
-DCMAKE_CXX_FLAGS="-I$PREFIX/include" \
-DCMAKE_VERBOSE_MAKEFILE:BOOL=FALSE \
-DINTERFACE_TYPE=qt5

makeParallel && make install
exitOnError
cp -r  ../packaging/win32/skel/share/icons $GIMAGEREADER_PREFIX/share/
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
