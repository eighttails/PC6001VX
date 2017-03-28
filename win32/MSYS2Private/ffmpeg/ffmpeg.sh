#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman -S --needed --noconfirm \
$MINGW_PACKAGE_PREFIX-libvorbis \
$MINGW_PACKAGE_PREFIX-libvpx \
$MINGW_PACKAGE_PREFIX-yasm
}

function buildFFmpeg(){
if [ -e $PREFIX/lib/libavcodec.a ]; then
echo "FFMpeg is already installed."
exit 0
fi

FFMPEG_VERSION=3.1.1
FFMPEG_SRC_DIR=ffmpeg-$FFMPEG_VERSION
wget -c https://www.ffmpeg.org/releases/$FFMPEG_SRC_DIR.tar.xz

rm -rf $FFMPEG_SRC_DIR
tar xf $FFMPEG_SRC_DIR.tar.xz
pushd $FFMPEG_SRC_DIR

./configure --target-os=mingw32 --prefix=$PREFIX --enable-small --disable-programs --disable-doc --disable-everything --disable-sdl --disable-iconv --enable-libvpx --enable-encoder=libvpx_vp8 --enable-libvorbis --enable-encoder=libvorbis --enable-muxer=webm --enable-protocol=file

makeParallel && makeParallel install

exitOnError
popd
}

#-----------------------------
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
source $SCRIPT_DIR/../common/common.sh
commonSetup
prerequisite

cd $EXTLIB

buildFFmpeg
