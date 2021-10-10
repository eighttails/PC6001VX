#!/bin/bash

#このスクリプトの置き場所
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))

#基本設定
source $SCRIPT_DIR/MSYS2Private/common/common.sh
commonSetup

#ツール類のインストール
pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-SDL2 \
$MINGW_PACKAGE_PREFIX-asciidoctor \
2>/dev/null

#FFmpegをビルド
#P6VX用に必要最低限のCODECのみ有効化しサイズを削減したビルド
bash $SCRIPT_DIR/MSYS2Private/ffmpeg/ffmpeg.sh
exitOnError



