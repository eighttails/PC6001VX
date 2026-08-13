#!/bin/bash

SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
QTDIR=${MINGW_PREFIX}/local/qt6-static-private
FFMPEG_DIR=${FFMPEG_DIR:-${MINGW_PREFIX}/local/ffmpeg-private7.1.4}

. $SCRIPT_DIR/buildenv.sh
. $SCRIPT_DIR/MSYS2Private/qt6-static-private/qt_ffmpeg_config.sh

if [ -z "$1" ]; then
    cd "$SCRIPT_DIR/../../"
else
    cd "$1"
fi

P6VX_DBUILD_DIR=PC6001VX-build-$MINGW_CHOST

if [ -e "$P6VX_DBUILD_DIR" ]; then
    rm -rf "$P6VX_DBUILD_DIR"
fi
mkdir "$P6VX_DBUILD_DIR"

cd "$P6VX_DBUILD_DIR"

cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$QTDIR" \
    -DQt6_DIR="$QTDIR/lib/cmake/Qt6" \
    -DFFMPEG_DIR="$FFMPEG_DIR" \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$PWD/release" \
    -S "$SCRIPT_DIR/.." \
    -B .

cmake --build . --config Release --parallel
