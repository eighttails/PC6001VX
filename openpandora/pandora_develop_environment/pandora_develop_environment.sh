#必要ツール
sudo dpkg --add-architecture i386
sudo apt-get install ia32-libs libcurl4-openssl-dev libgpgme11-dev libtool build-essential libglib2.0-dev xsltproc libncurses5:i386

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
export PNDSDK=$HOME/pandora-dev/arm-2011.09


#SDKフォルダ内のホームフォルダ
export SDKHOME=$PNDSDK/home/$USER

#pkgconfigを設定（プロジェクト設定にも追加？）
export PKG_CONFIG_LIBDIR=$PNDSDK/usr/lib/pkgconfig:$PNDSDK/usr/share/pkgconfig
export PKG_CONFIG_PATH=$PKG_CONFIG_LIBDIR
export PKG_CONFIG_SYSROOT_DIR=/

#libtoolはクロスコンパイルで問題が出るので関連ファイルを消しておく
rm $PNDSDK/usr/lib/*.la

#libpthread-stubs
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/libpthread-stubs-0.3.tar.gz
tar xf libpthread-stubs-0.3.tar.gz
cd libpthread-stubs-0.3
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
make -j3 && make install

#xcb-proto
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/xcb-proto-1.8.tar.gz
tar xf xcb-proto-1.8.tar.gz
cd xcb-proto-1.8
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
make -j3 && make install

#libxcb
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/libxcb-1.9.1.tar.gz
tar xf libxcb-1.9.1.tar.gz
cd libxcb-1.9.1
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
make -j3 && make install

#libX11
cd $SDKHOME
wget http://ftp.x.org/pub/individual/lib/libX11-1.3.6.tar.gz
tar xf libX11-1.3.6.tar.gz
cd libX11-1.3.6
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr --disable-malloc0returnsnull
make -j3 && make install

#Qt
cd $SDKHOME
wget -c http://download.qt-project.org/official_releases/qt/5.2/5.2.1/single/qt-everywhere-opensource-src-5.2.1.tar.gz
tar xf qt-everywhere-opensource-src-5.2.1.tar.gz
cd qt-everywhere-opensource-src-5.2.1

#mkspecをコピー
cp -rf $INSTALLER_DIR/linux-pandora-g++ qtbase/mkspecs

#ALSAで音が出ない問題に対処するパッチを当てる
patch -p1  < $INSTALLER_DIR/qtmultimedia.patch

cd qtbase
make confclean -j2
cd ..
./configure -opensource -confirm-license -prefix $PNDSDK/usr -headerdir $PNDSDK/usr/include/qt5 -xplatform linux-pandora-g++ -static -no-c++11 -opengl es2 -qreal float -qt-xcb -no-icu -no-sql-sqlite -nomake examples -skip qtwebkit-examples -silent
#echo "Hit Enter.";read Wait
make -j3 && make install

