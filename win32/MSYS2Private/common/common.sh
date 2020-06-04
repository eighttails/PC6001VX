#!/bin/bash
#共通関数、設定
#このスクリプトはsourceコマンドで実行すること。

function exitOnError(){
if [ $? -ne 0 ]; then
    echo "ERROR."
    exit 1
else
    echo "SUCCESS."
fi
}

function checkJDK(){
if [ "$JAVA_HOME" = "" ]; then
    echo 'Please specify $JAVA_HOME.'
    exit 1
else
    export PATH=$(cygpath -up "$JAVA_HOME/bin"):$PATH
    export JAVA_TOOL_OPTIONS=-Duser.language=en
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

function makeParallel(){
#並列ビルドの場合依存関係でビルドに失敗することがあるので3回までmakeする。
for (( i=0; i<3; i++))
do
    make -j$(($(nproc)*2)) "$@"
    if [ $? -eq 0 ]; then
        return 0
    fi
done
return 1
}

function mingw32MakeParallel(){
#並列ビルドの場合依存関係でビルドに失敗することがあるので3回までmakeする。
for (( i=0; i<3; i++))
do
    mingw32-make -j$(($(nproc)*2)) "$@"
    if [ $? -eq 0 ]; then
        return 0
    fi
done
return 1
}

function waitEnter(){
echo "Hit Enter"
read Wait
}

function toolchain(){
#基本ツールチェーン
#ディレクトリが存在しない場合があるので作っておく
mkdir $MINGW_PREFIX 2> /dev/null

#ツール類
pacman "${PACMAN_INSTALL_OPTS[@]}" \
base-devel \
VCS \
unzip \
wget \
tar \
zip \
perl \
python \
ruby \
autoconf-archive \
$MINGW_PACKAGE_PREFIX-toolchain \
$MINGW_PACKAGE_PREFIX-cmake \
$MINGW_PACKAGE_PREFIX-curl \
2>/dev/null

exitOnError
}

function commonSetup(){
#共通の環境変数、パスの設定
#環境チェック
if [ -z "$MINGW_PREFIX" ]; then
  echo "Please run this script in MinGW 32bit or 64bit shell. (not in MSYS2 shell)"
  exit 1
fi

#pacmanのパッケージ取得オプション
PACMAN_INSTALL_OPTS=()
PACMAN_INSTALL_OPTS+=('-S')
PACMAN_INSTALL_OPTS+=('--needed')
PACMAN_INSTALL_OPTS+=('--noconfirm')
PACMAN_INSTALL_OPTS+=('--disable-download-timeout')
export PACMAN_INSTALL_OPTS

#基本ツールチェーンのセットアップ
toolchain

#外部依存ライブラリのソース展開先
mkdir ~/extlib 2> /dev/null
export EXTLIB=~/extlib

#インストール先(/mingw32/localまたは/mingw64/local)
export PREFIX=$MINGW_PREFIX/local
mkdir -p $PREFIX/bin 2> /dev/null
mkdir -p $PREFIX/include 2> /dev/null
mkdir -p $PREFIX/lib 2> /dev/null

export PATH=$PREFIX/bin:$PATH
export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH
export CMAKE_PREFIX_PATH=$PREFIX:$CMAKE_PREFIX_PATH
export CMAKE_INCLUDE_PATH=$PREFIX/include

#最低限必要なDLLをコピー
pushd $MINGW_PREFIX/bin
if [ "$MINGW_CHOST" = "i686-w64-mingw32" ]; then
    #32bit
        export BIT='32bit'
    NEEDED_DLLS='libgcc_s_dw2-1.dll libstdc++-6.dll libwinpthread-1.dll zlib1.dll'
else
    #64bit
        export BIT='64bit'
    NEEDED_DLLS='libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll zlib1.dll'
fi
cp -f $NEEDED_DLLS $PREFIX/bin
popd
}
