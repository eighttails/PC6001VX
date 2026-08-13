#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "${BASH_SOURCE:-$0}")")
PROJECT_DIR=$(readlink -f "$SCRIPT_DIR/..")
cd "$PROJECT_DIR"
rm -rf build/appimage 2>/dev/null
mkdir -p build/appimage

WORKDIR=$(mktemp -d)
mkdir -p "$WORKDIR/AppDir"

cmake -S "$PROJECT_DIR" -B "$WORKDIR/build" -DCMAKE_BUILD_TYPE=Release
cmake --build "$WORKDIR/build" --parallel "$(nproc)"

export LINUXDEPLOY_OUTPUT_VERSION=$(cat "$PROJECT_DIR/VERSION.PC6001VX")
cd "$WORKDIR"
linuxdeploy \
    --plugin qt \
    --appdir="$WORKDIR/AppDir" \
    --executable="$WORKDIR/build/PC6001VX" \
    --icon-file="$PROJECT_DIR/data/PC-6001_256.png" \
    --desktop-file="$PROJECT_DIR/appimage/PC6001VX.desktop" \
    --output appimage
cp *.AppImage PC6001VX.AppImage
mv PC6001VX*.AppImage "$PROJECT_DIR/build/appimage"
echo "$WORKDIR"
