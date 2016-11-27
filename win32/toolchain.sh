#ディレクトリが存在しない場合があるので作っておく
mkdir /mingw32 > /dev/null
mkdir /mingw64 > /dev/null

#ツール類
pacman -S --needed --noconfirm base base-devel mingw-w64-i686-toolchain VCS unzip wget tar zip perl python ruby mingw-w64-i686-icu  mingw-w64-i686-SDL2 mingw-w64-i686-libvorbis mingw-w64-i686-libvpx mingw-w64-i686-yasm
