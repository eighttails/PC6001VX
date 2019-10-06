#!/bin/bash

#このスクリプトの置き場所
SCRIPT_DIR=$(dirname $(readlink -f ${BASH_SOURCE:-$0}))

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
echo $FULLVER > $SCRIPT_DIR/VERSION 

#ヘッダーファイル
perl -pi -e "s/(#define\tVERSION)(.*)/\$1\t\t\"$FULLVER\"/" $SCRIPT_DIR/src/pc6001v.h 	

#Androidマニフェストファイル
perl -pi -e "s/(android:versionName=)\"([^\"]+)\"/\$1\"$FULLVER\"/" $SCRIPT_DIR/android/AndroidManifest.xml

#proファイル
perl -pi -e "s/(VERSION =)(.*)/\$1 $NUMVER/" $SCRIPT_DIR/PC6001VX.pro 	
