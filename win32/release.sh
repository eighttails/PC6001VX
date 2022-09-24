#!/bin/bash

export LOCKFILE='/tmp/pc6001vx.lck'

(
# 排他制御(32ビット版と64ビット版のビルドが同時に走らないように)
# 200という番号は慣例
echo "Waiting for other builds to finish..."
flock -x 200

#環境チェック
if [ -z "$MINGW_PREFIX" ]; then
	echo "Please run this script in MinGW 32bit or 64bit shell. (not in MSYS2 shell)"
	exit 1
fi

if [ "$MINGW_CHOST" = "i686-w64-mingw32" ]; then
	BIT=32bit
else
	BIT=64bit
fi

export SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
cd $SCRIPT_DIR

#引数としてバージョンを指定すること
if [ -z $1 ]; then
    VERSION=$(cat $SCRIPT_DIR/../VERSION.PC6001VX)
else
    VERSION=$1
fi

RELEASE_DIR=$PWD/../../PC6001VX_release
#Windows版バイナリ
WIN_BIN_NAME=PC6001VX_$VERSION\_win_$BIT
WIN_BIN_DIR=$RELEASE_DIR/$WIN_BIN_NAME
rm -rf $WIN_BIN_DIR
mkdir -p $WIN_BIN_DIR
pushd $RELEASE_DIR

#プログラム本体をビルド
$SCRIPT_DIR/buildrelease.sh "$PWD"

pushd PC6001VX-build-$MINGW_CHOST/release
cp -f PC6001VX.exe $WIN_BIN_DIR
cp -f $SCRIPT_DIR/safemode.bat $WIN_BIN_DIR
asciidoctor $SCRIPT_DIR/../README.adoc -o $WIN_BIN_DIR/README.html
popd

zip -r $WIN_BIN_NAME.zip $WIN_BIN_NAME
popd

#ソースtarball
pushd $SCRIPT_DIR/..
SOURCENAME=PC6001VX_$VERSION\_src
git archive --format=tar --prefix=$SOURCENAME/ HEAD | gzip > $RELEASE_DIR/$SOURCENAME.tar.gz
popd

) 200>$LOCKFILE

