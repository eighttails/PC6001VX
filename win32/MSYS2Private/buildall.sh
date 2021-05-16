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

) 200>$LOCKFILE
