#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman -S --needed --noconfirm \
$MINGW_PACKAGE_PREFIX-libvorbis \
$MINGW_PACKAGE_PREFIX-libvpx \
$MINGW_PACKAGE_PREFIX-yasm
}

function build(){
if [ -e $PREFIX/lib/libavcodec.a -a $((FORCE_INSTALL)) == 0 ]; then
echo "FFMpeg is already installed."
exit 0
fi

FFMPEG_VERSION=3.3.5
FFMPEG_SRC_DIR=ffmpeg-$FFMPEG_VERSION
FFMPEG_BUILD_DIR=$FFMPEG_SRC_DIR-$BIT
wget -c https://www.ffmpeg.org/releases/$FFMPEG_SRC_DIR.tar.xz

rm -rf $FFMPEG_SRC_DIR $FFMPEG_BUILD_DIR 
tar xf $FFMPEG_SRC_DIR.tar.xz
mv $FFMPEG_SRC_DIR $FFMPEG_BUILD_DIR
pushd $FFMPEG_BUILD_DIR

./configure --target-os=mingw32 --prefix=$PREFIX --enable-small --disable-programs --disable-doc --disable-everything --disable-sdl2 --disable-iconv --enable-libvpx --enable-encoder=libvpx_vp8 --enable-libvorbis --enable-encoder=libvorbis --enable-muxer=webm --enable-protocol=file
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
