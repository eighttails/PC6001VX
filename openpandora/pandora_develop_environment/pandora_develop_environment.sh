#必要ツール
sudo dpkg --add-architecture i386
sudo apt-get install ia32-libs libssl-dev libcurl4-openssl-dev libgpgme11-dev libtool build-essential libglib2.0-dev xsltproc squashfs-tools libncurses5:i386
sudo apt-get build-dep gdb

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
export PNDSDK=$HOME/pandora-dev/arm-2010q1

#cmathの不具合対策
find $PNDSDK/arm-none-linux-gnueabi/include/c++ -name cmath | xargs sed -i -e "s|  using ::\([a-z0-9]\+\)l;|  //using ::\1l;|"
find $PNDSDK/arm-none-linux-gnueabi/include/c++ -name cmath | xargs sed -i -e "s|  //using ::ceil;|  using ::ceil;|"


#SDKフォルダ内のホームフォルダ
export SDKHOME=$PNDSDK/home/$USER

#pkgconfigを設定（プロジェクト設定にも追加？）
export PKG_CONFIG_LIBDIR=$PNDSDK/usr/lib/pkgconfig:$PNDSDK/usr/share/pkgconfig
export PKG_CONFIG_PATH=$PKG_CONFIG_LIBDIR
export PKG_CONFIG_SYSROOT_DIR=/

#libtoolはクロスコンパイルで問題が出るので関連ファイルを消しておく
rm $PNDSDK/usr/lib/*.la

#gdb(pythonサポート付き)
cd $SDKHOME
wget -c http://ftp.gnu.org/gnu/gdb/gdb-7.7.1.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/gdb-7.7.1.tar.gz
cd gdb-7.7.1
./configure --prefix=$PNDSDK/usr/local/gdb-with-python --target=arm-linux-gnueabi --with-python
make -j3 && make install

#libpthread-stubs
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/libpthread-stubs-0.3.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/libpthread-stubs-0.3.tar.gz
cd libpthread-stubs-0.3
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
make -j3 && make install

#xcb-proto 
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/xcb-proto-1.10.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/xcb-proto-1.10.tar.gz
cd xcb-proto-1.10
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
make -j3 && make install

#libxcb
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/libxcb-1.10.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/libxcb-1.10.tar.gz
cd libxcb-1.10
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
make -j3 && make install

#libX11
cd $SDKHOME
wget -c http://ftp.x.org/pub/individual/lib/libX11-1.3.6.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/libX11-1.3.6.tar.gz
cd libX11-1.3.6
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr --disable-malloc0returnsnull
make -j3 && make install

#Qt
cd $SDKHOME
QT_MAJOR_VER=5.3
QT_VER=$QT_MAJOR_VER.2
wget -c http://download.qt-project.org/official_releases/qt/$QT_MAJOR_VER/$QT_VER/single/qt-everywhere-opensource-src-$QT_VER.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/qt-everywhere-opensource-src-$QT_VER.tar.gz
cd qt-everywhere-opensource-src-$QT_VER

#mkspecをコピー
cp -rf $INSTALLER_DIR/linux-pandora-g++ qtbase/mkspecs
sed -i -e "s|\$\$PNDSDK|$PNDSDK|" qtbase/mkspecs/linux-pandora-g++/qmake.conf

#ALSAで音が出ない問題に対処するパッチを当てる
patch -p1  < $INSTALLER_DIR/qtmultimedia53.patch
#カメラ関係でビルドが通らないのを回避
rm -rf qtmultimedia/config.tests/gstreamer_encodingprofiles

cd qtbase
#make confclean -j3
cd ..
./configure -opensource -confirm-license -prefix $PNDSDK/usr -headerdir $PNDSDK/usr/include/qt5 -xplatform linux-pandora-g++ -static -c++11 -qt-xcb -no-icu -no-pulseaudio -no-sql-sqlite -nomake examples -skip qtwebkit-examples -skip qtlocation -silent
#echo "Hit Enter.";read Wait
make -j3 && make install

