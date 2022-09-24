#!/bin/bash

#このスクリプトの置き場所
if [ -z "$MINGW_PREFIX" ]; then
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))
PERL=perl
else
SCRIPT_DIR=$(cygpath -m $(dirname $(readlink -f ${BASH_SOURCE:-$0})))
PERL=/bin/perl
fi
echo $SCRIPT_DIR

#バージョン番号
MAJOR=$1
MINOR=$2
PATCH=$3
PRE=$4

if [ -z "$1" -o -z "$2" -o -z "$3" ]; then
    echo usage $0 MAJOR MINOR PATCH [PRE]
    exit 1
fi 

#数字のみのバージョン
NUMVER=$MAJOR.$MINOR.$PATCH

#プレリリース(alpha,beta,rc)を含んだバージョン
if [ -n "$PRE" ];then
    FULLVER=$NUMVER-$PRE
else
    FULLVER=$NUMVER
fi

#バージョン番号ファイル(ビルドスクリプト用)
echo $FULLVER > $SCRIPT_DIR/VERSION.PC6001VX

#ヘッダーファイル
$PERL -pi -e "s/(#define\tVERSION)(.*)/\$1\t\t\t\t\t\"$FULLVER\"/" "$SCRIPT_DIR/src/pc6001v.h" 	

#Androidマニフェストファイル
$PERL -pi -e "s/(android:versionName=)\"([^\"]+)\"/\$1\"$FULLVER\"/" "$SCRIPT_DIR/android/AndroidManifest.xml"

#proファイル
$PERL -pi -e "s/(VERSION =)(.*)/\$1 $NUMVER/" "$SCRIPT_DIR/PC6001VX.pro"

