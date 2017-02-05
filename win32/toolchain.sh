#このスクリプトの置き場所をカレントとして実行すること。
#カレントディレクトリ
export SCRIPT_DIR=$PWD

#ディレクトリが存在しない場合があるので作っておく
mkdir /mingw32 2> /dev/null
mkdir /mingw64 2> /dev/null

#ツール類
pacman -S --needed --noconfirm base base-devel mingw-w64-i686-toolchain VCS unzip wget tar zip perl python ruby mingw-w64-i686-icu  mingw-w64-i686-SDL2 mingw-w64-i686-libvorbis mingw-w64-i686-libvpx mingw-w64-i686-yasm

#DirectShowのヘッダー問題対策
pushd /mingw32/i686-w64-mingw32
#https://github.com/Alexpux/MINGW-packages/issues/1689
patch -p2 < $SCRIPT_DIR/0001-Revert-Avoid-declaring-something-extern-AND-initiali.patch
#https://sourceforge.net/p/mingw-w64/mailman/message/35527066/
patch -p2 < $SCRIPT_DIR/wrl.patch
popd

