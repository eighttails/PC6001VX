#!/bin/bash
rm -rf $MINGW_PREFIX/local
./lhasa/lhasa.sh
./ffmpeg/ffmpeg.sh
./gimagereader/gimagereader.sh
