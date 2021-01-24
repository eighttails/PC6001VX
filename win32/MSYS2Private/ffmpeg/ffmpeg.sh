#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-libvorbis \
$MINGW_PACKAGE_PREFIX-libvpx \
$MINGW_PACKAGE_PREFIX-yasm \
2>/dev/null

exitOnError
}

function build(){
if [ -e $PREFIX/lib/libavcodec-private.a -a $((FORCE_INSTALL)) == 0 ]; then
echo "FFMpeg is already installed."
exit 0
fi

FFMPEG_VERSION=4.3.1
FFMPEG_SRC_DIR=ffmpeg-$FFMPEG_VERSION
FFMPEG_BUILD_DIR=$FFMPEG_SRC_DIR-$BIT
wget -c https://www.ffmpeg.org/releases/$FFMPEG_SRC_DIR.tar.xz

rm -rf $FFMPEG_SRC_DIR $FFMPEG_BUILD_DIR 
tar xf $FFMPEG_SRC_DIR.tar.xz
mv $FFMPEG_SRC_DIR $FFMPEG_BUILD_DIR
pushd $FFMPEG_BUILD_DIR

./configure \
--target-os=mingw32 \
--prefix=$PREFIX \
--build-suffix=-private \
--disable-shared \
--enable-static \
--pkg-config-flags=--static \
--extra-libs=-static \
--extra-cflags=--static \
--enable-small \
--disable-programs \
--disable-doc \
--disable-everything \
--disable-sdl2 \
--disable-iconv \
--enable-libvpx \
--enable-encoder=libvpx_vp9 \
--enable-libvorbis \
--enable-encoder=libvorbis \
--enable-bsf=vp9_superframe \
--enable-hwaccel=vp9_d3d11va \
--enable-muxer=webm \
--enable-protocol=file
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
