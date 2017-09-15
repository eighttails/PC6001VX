#!/bin/bash
rm -rf $MINGW_PREFIX/local

./ffmpeg/ffmpeg.sh

./gimagereader/gimagereader.sh
