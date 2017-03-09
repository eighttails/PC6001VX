#!/bin/bash
function exitOnError(){
if [ $? -ne 0 ]; then 
    echo "ERROR."
    exit
else
    echo "SUCCESS."
fi
}

function patchOnce(){
#適用済みでない場合のみパッチを充てる
patch -p$1 -N --dry-run --silent < $2 2>/dev/null
if [ $? -eq 0 ];
then
    #apply the patch
    patch -p$1 -N < $2
fi
}

#環境チェック
if [ -z "$MINGW_PREFIX" ]; then
	echo "Please run this script in MinGW 32bit or 64bit shell. (not in MSYS2 shell)"
	exit 1
fi

#このスクリプトの置き場所
export SCRIPT_DIR=$(cd $(dirname $(readlink $0 || echo $0));pwd)

#ディレクトリが存在しない場合があるので作っておく
mkdir $MINGW_PREFIX 2> /dev/null

#ツール類
pacman -S --needed --noconfirm \
base \
base-devel \
VCS \
unzip \
wget \
tar \
zip \
perl \
python \
ruby \
$MINGW_PACKAGE_PREFIX-toolchain \
$MINGW_PACKAGE_PREFIX-icu \
$MINGW_PACKAGE_PREFIX-SDL2 \
$MINGW_PACKAGE_PREFIX-libvorbis \
$MINGW_PACKAGE_PREFIX-libvpx \
$MINGW_PACKAGE_PREFIX-yasm

exitOnError

#DirectShowのヘッダー問題対策
pushd $MINGW_PREFIX/$MINGW_CHOST
#https://github.com/Alexpux/MINGW-packages/issues/1689
patchOnce 2 $SCRIPT_DIR/0001-Revert-Avoid-declaring-something-extern-AND-initiali.patch
#https://sourceforge.net/p/mingw-w64/mailman/message/35527066/
patchOnce 2 $SCRIPT_DIR/wrl.patch
popd

