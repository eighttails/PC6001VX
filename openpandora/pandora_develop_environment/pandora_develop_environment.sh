#カレントディレクトリ
INSTALLER_DIR=$PWD

#sdk_installer_openpandora_toolchain.sh中の
#pandora_configure.sh
#CPPFLAGSとCXXFLAGSを交換
#pkgconfigのパスを置換している箇所に$PNDSDK/usr/share/pkgconfig -name '*pc';
#を追加しておく
#さらにpkgconfigファイル内のディレクトリを置換している箇所にgstreamer用の設定
# s#pluginsdir=\/.*#pluginsdir='$PNDSDK/usr/libgstreamer-0.10'#;
#を追加しておく
#(予め編集しておく)
./sdk_installer_openpandora_toolchain.sh
source ~/.bashrc


#SDKフォルダ内のホームフォルダ
SDKHOME=$PNDSDK/home/$USERNAME

#pkgconfigを設定（プロジェクト設定にも追加？）
export PKG_CONFIG_LIBDIR=$PNDSDK/usr/lib/pkgconfig:$PNDSDK/usr/share/pkgconfig
export PKG_CONFIG_PATH=$PKG_CONFIG_LIBDIR
export PKG_CONFIG_SYSROOT_DIR=/

#libpthread-stubs
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/libpthread-stubs-0.3.tar.gz
tar xf libpthread-stubs-0.3.tar.gz
cd libpthread-stubs-0.3
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
make && make install

#xcb-proto
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/xcb-proto-1.8.tar.gz
tar xf xcb-proto-1.8.tar.gz
cd xcb-proto-1.8
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
make && make install

#libxcb
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/libxcb-1.9.1.tar.gz
tar xf libxcb-1.9.1.tar.gz
cd libxcb-1.9.1
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
make && make install

#Qt
cd $SDKHOME
wget -c  http://download.qt-project.org/official_releases/qt/5.1/5.1.1/single/qt-everywhere-opensource-src-5.1.1.tar.gz
tar xf qt-everywhere-opensource-src-5.1.1.tar.gz
cd qt-everywhere-opensource-src-5.1.1

#mkspecをコピー
cp -rf $INSTALLER_DIR/linux-pandora-g++ qtbase/mkspecs

#ALSAで音が出ない問題に対処するパッチを当てる
patch -p1  < $INSTALLER_DIR/qtmultimedia.patch

cd qtbase
make confclean -j2
cd ..
./configure -opensource -confirm-license -prefix $PNDSDK/usr -headerdir $PNDSDK/usr/include/qt5 -xplatform linux-pandora-g++ -static -qt-xcb -system-sqlite -no-c++11 -nomake examples -skip qtwebkit-examples -v
#echo "Hit Enter.";read Wait
make -j2 && make install

