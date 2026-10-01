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
export QML_SOURCES_PATHS="$PROJECT_DIR/src/Qt/qml"
if ! QMAKE=$(command -v qmake6); then
    echo "qmake6 not found; cannot locate Qt QML runtime modules" >&2
    exit 1
fi
export QMAKE
QT_INSTALL_QML=$("$QMAKE" -query QT_INSTALL_QML)
if [[ ! -d "$QT_INSTALL_QML/QtQuick" || ! -d "$QT_INSTALL_QML/QtQml" ]]; then
    echo "Qt QML runtime modules not found under $QT_INSTALL_QML" >&2
    exit 1
fi
QT_QML_PLUGIN_LIBRARIES=(
    "$QT_INSTALL_QML/QtQuick/libqtquick2plugin.so"
    "$QT_INSTALL_QML/QtQml/libqmlplugin.so"
    "$QT_INSTALL_QML/QtQml/Models/libmodelsplugin.so"
    "$QT_INSTALL_QML/QtQml/WorkerScript/libworkerscriptplugin.so"
)
for plugin in "${QT_QML_PLUGIN_LIBRARIES[@]}"; do
    if [[ ! -f "$plugin" ]]; then
        echo "Qt QML runtime plugin not found: $plugin" >&2
        exit 1
    fi
done
mkdir -p "$WORKDIR/AppDir/usr/qml"
cp -a "$QT_INSTALL_QML/QtQuick" "$QT_INSTALL_QML/QtQml" "$WORKDIR/AppDir/usr/qml/"

# Qt 6.2のQt MultimediaはLinuxでGStreamerを使って音声を出力する。
# ホストのGStreamerプラグインは同梱のglibと互換性がなく読み込めないため、
# 音声出力に必要なプラグインだけを同梱する。
GST_SYSTEM_PLUGINS_DIR=$(pkg-config --variable=pluginsdir gstreamer-1.0 2>/dev/null || echo "/usr/lib/$(uname -m)-linux-gnu/gstreamer-1.0")
GST_REQUIRED_PLUGINS=(
    coreelements
    app
    audioconvert
    audioresample
    volume
    autodetect
    pulseaudio
    alsa
)
export GSTREAMER_PLUGINS_DIR="$WORKDIR/gstreamer-plugins"
mkdir -p "$GSTREAMER_PLUGINS_DIR"
for plugin in "${GST_REQUIRED_PLUGINS[@]}"; do
    if [[ ! -f "$GST_SYSTEM_PLUGINS_DIR/libgst$plugin.so" ]]; then
        echo "GStreamer plugin not found: $GST_SYSTEM_PLUGINS_DIR/libgst$plugin.so" >&2
        exit 1
    fi
    cp "$GST_SYSTEM_PLUGINS_DIR/libgst$plugin.so" "$GSTREAMER_PLUGINS_DIR/"
done

cd "$WORKDIR"
linuxdeploy \
    --plugin qt \
    --plugin gstreamer \
    --appdir="$WORKDIR/AppDir" \
    --executable="$WORKDIR/build/PC6001VX" \
    --library="${QT_QML_PLUGIN_LIBRARIES[0]}" \
    --library="${QT_QML_PLUGIN_LIBRARIES[1]}" \
    --library="${QT_QML_PLUGIN_LIBRARIES[2]}" \
    --library="${QT_QML_PLUGIN_LIBRARIES[3]}" \
    --icon-file="$PROJECT_DIR/data/PC-6001_256.png" \
    --desktop-file="$PROJECT_DIR/appimage/PC6001VX.desktop" \
    --output appimage
cp *.AppImage PC6001VX.AppImage
mv PC6001VX*.AppImage "$PROJECT_DIR/build/appimage"
echo "$WORKDIR"
