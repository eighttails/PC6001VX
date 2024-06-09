#!/bin/bash

function prerequisite(){
#必要ライブラリ
pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-libvorbis \
$MINGW_PACKAGE_PREFIX-libvpx \
$MINGW_PACKAGE_PREFIX-yasm \
$MINGW_PACKAGE_PREFIX-vulkan-devel \
2>/dev/null

exitOnError
}

function build(){
MAJOR_VER=$1
MINOR_VER=$2
if [ -n "$3" ];then
PATCH_VER=$3
FFMPEG_VERSION=$MAJOR_VER.$MINOR_VER.$PATCH_VER
else
FFMPEG_VERSION=$MAJOR_VER.$MINOR_VER
fi

if [ -e $PREFIX/lib/libavcodec-private$FFMPEG_VERSION.a -a $((FORCE_INSTALL)) == 0 ]; then
echo "FFMpeg $FFMPEG_VERSION is already installed."
return
fi

FFMPEG_SRC_DIR=ffmpeg-$FFMPEG_VERSION
FFMPEG_BUILD_DIR=$FFMPEG_SRC_DIR-$MSYSTEM
wget -c https://www.ffmpeg.org/releases/$FFMPEG_SRC_DIR.tar.xz

rm -rf $FFMPEG_SRC_DIR $FFMPEG_BUILD_DIR 
tar xf $FFMPEG_SRC_DIR.tar.xz
mv $FFMPEG_SRC_DIR $FFMPEG_BUILD_DIR
pushd $FFMPEG_BUILD_DIR

if [ -n "$FFMPEG_DEBUG" ];then
    DEBUG_FLAGS="--enable-debug=3 --disable-optimizations"
else
    DEBUG_FLAGS="--disable-debug"
fi

./configure \
--target-os=mingw32 \
--prefix=$PREFIX \
--incdir=$PREFIX/include/ffmpeg-private$FFMPEG_VERSION \
--build-suffix=-private$FFMPEG_VERSION \
$DEBUG_FLAGS \
--disable-shared \
--enable-static \
--pkg-config-flags=--static \
--extra-libs=-static \
--extra-cflags=--static \
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

build 5 1 4; exitOnError
build 6 1 1; exitOnError
build 7 0 1; exitOnError
