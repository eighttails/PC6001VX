#!/bin/bash
export LOCKFILE='/tmp/msys2private.lck'

(
# 排他制御(32ビット版と64ビット版のビルドが同時に走らないように)
# 200という番号は慣例
echo "Waiting for other builds to finish..."
flock -x 200

SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
cd $SCRIPT_DIR
source $SCRIPT_DIR/common/common.sh
commonSetup

./lhasa/lhasa.sh
exitOnError

./ffmpeg/ffmpeg.sh
exitOnError

./gimagereader/gimagereader.sh
exitOnError

./qt5-static-angle/qt.sh
exitOnError

pacman "${PACMAN_INSTALL_OPTS[@]}" \
$MINGW_PACKAGE_PREFIX-clang \
$MINGW_PACKAGE_PREFIX-qt-creator \
2> /dev/null

) 200>$LOCKFILE
