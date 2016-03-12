#リポジトリをGCC4.9リポジトリに差し替え(GCC5が安定するまでの暫定策)
cp -f mirrorlist.mingw32 /etc/pacman.d

#リポジトリを更新
pacman -Syy

#ツール類
pacman -Suu --needed --noconfirm base base-devel mingw-w64-i686-toolchain VCS unzip wget tar zip perl python ruby mingw-w64-i686-icu  mingw-w64-i686-SDL2 mingw-w64-i686-libvorbis mingw-w64-i686-libvpx mingw-w64-i686-yasm


