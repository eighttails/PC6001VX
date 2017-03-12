#!/bin/bash
#基本設定
source MSYS2Private/common/common.sh
commonSetup

#このスクリプトの置き場所
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))

#Qtをビルド
#OpenGLにAngleを使用、バイナリサイズ抑制最適化
bash $SCRIPT_DIR/MSYS2Private/qt/qt.sh

#FFmpegをビルド
#P6VX用に必要最低限のCODECのみ有効化しサイズを削減したビルド
bash $SCRIPT_DIR/MSYS2Private/ffmpeg/ffmpeg.sh
