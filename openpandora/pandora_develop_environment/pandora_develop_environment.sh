#必要ツール
sudo dpkg --add-architecture i386
sudo apt-get install libssl-dev libcurl4-openssl-dev libgpgme11-dev libtool build-essential libglib2.0-dev xsltproc squashfs-tools libncurses5:i386
sudo apt-get build-dep gdb

#コマンドのエイリアス
NUM_OF_PROCESSORS=`getconf _NPROCESSORS_ONLN`
PARALLEL_JOBS=`expr $NUM_OF_PROCESSORS \* 2`
makeparallel="nice -n 19 make -j$PARALLEL_JOBS"
makeinstall='make install'

#カレントディレクトリ
INSTALLER_DIR=$PWD                                                  

#SDKディレクトリ
export PNDSDK=$HOME/pandora-dev/arm-2010q1

#SDKフォルダ内のホームフォルダ
export SDKHOME=$PNDSDK/home/$USER


#初回起動時に環境全体を構築する
if [ ! -d $PNDSDK ]; then
FIRSTRUN=1
else
FIRSTRUN=0
fi

#sdk_installer_openpandora_toolchain.sh中の
#pandora_configure.sh
#CPPFLAGSとCXXFLAGSを交換
#pkgconfigのパスを置換している箇所に$PNDSDK/usr/share/pkgconfig -name '*pc';
#を追加しておく
#さらにpkgconfigファイル内のディレクトリを置換している箇所にgstreamer用の設定
# s#pluginsdir=\/.*#pluginsdir='$PNDSDK/usr/libgstreamer-0.10'#;
#を追加しておく
#(予め編集しておく)
if [ $FIRSTRUN -eq 1 ]; then 
./sdk_installer_openpandora_toolchain.sh
ln -snf $PNDSDK $HOME/pandora-dev/arm
fi

#pkgconfigを設定（プロジェクト設定にも追加？）
export PKG_CONFIG_LIBDIR=$PNDSDK/usr/lib/pkgconfig:$PNDSDK/usr/share/pkgconfig
export PKG_CONFIG_PATH=$PKG_CONFIG_LIBDIR
export PKG_CONFIG_SYSROOT_DIR=/

if [ $FIRSTRUN -eq 1 ]; then 
#cmathの不具合対策
find $PNDSDK/arm-none-linux-gnueabi/include/c++ -name cmath | xargs sed -i -e "s|  using ::\([a-z0-9]\+\)l;|  //using ::\1l;|"
find $PNDSDK/arm-none-linux-gnueabi/include/c++ -name cmath | xargs sed -i -e "s|  //using ::ceil;|  using ::ceil;|"

#EGLにおける、X11とQtのシンボル衝突対策
pushd $PNDSDK/usr/include/EGL
patch < $INSTALLER_DIR/eglplatform.patch
popd

#libtoolはクロスコンパイルで問題が出るので関連ファイルを消しておく
rm $PNDSDK/usr/lib/*.la

#gdb(pythonサポート付き)
cd $SDKHOME
wget -c http://ftp.gnu.org/gnu/gdb/gdb-7.12.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/gdb-7.12.tar.gz
cd gdb-7.12
./configure --prefix=$PNDSDK/usr/local/gdb-with-python --target=arm-linux-gnueabi --with-python
$makeparallel && $makeinstall

#libpthread-stubs
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/libpthread-stubs-0.3.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/libpthread-stubs-0.3.tar.gz
cd libpthread-stubs-0.3
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
$makeparallel && $makeinstall

#xcb-proto 
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/xcb-proto-1.11.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/xcb-proto-1.11.tar.gz
cd xcb-proto-1.11
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
$makeparallel && $makeinstall

#libxcb
cd $SDKHOME
wget -c http://xcb.freedesktop.org/dist/libxcb-1.11.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/libxcb-1.11.tar.gz
cd libxcb-1.11
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr
$makeparallel && $makeinstall

#libX11
cd $SDKHOME
wget -c http://ftp.x.org/pub/individual/lib/libX11-1.3.6.tar.gz -P $INSTALLER_DIR/tmp
tar xf $INSTALLER_DIR/tmp/libX11-1.3.6.tar.gz
cd libX11-1.3.6
../../../../sdk_utils/pandora_configure.sh --prefix=$PNDSDK/usr --disable-malloc0returnsnull
$makeparallel && $makeinstall

fi #if [ $FIRSTRUN -eq 1 ] 


#Qt
#インストールに使用するフォルダの名前。「qt5」という名前にしてはならない。
QT_INSTALLNAME=qt562-release

QT_MAJOR_VER=5.6
QT_VER=$QT_MAJOR_VER.2
QT_FULL_VER=$QT_VER
#QT_RELEASE=development_releases
QT_RELEASE=official_releases
QT_SOURCE_NAME=qt-everywhere-opensource-src-$QT_FULL_VER

cd $SDKHOME
rm -rf $QT_INSTALLNAME
mkdir $QT_INSTALLNAME

wget -c http://download.qt.io/$QT_RELEASE/qt/$QT_MAJOR_VER/$QT_FULL_VER/single/$QT_SOURCE_NAME.tar.xz -P $INSTALLER_DIR/tmp

if [ ! -d $QT_SOURCE_NAME ]; then
tar Jxf $INSTALLER_DIR/tmp/$QT_SOURCE_NAME.tar.xz
pushd $QT_SOURCE_NAME

#ALSAで音が出ない問題に対処するパッチを当てる
#patch -p1  < $INSTALLER_DIR/qtmultimedia53.patch
#EGLFS関連でコンパイルが通らない問題に対処するパッチを当てる
#patch -p1  < $INSTALLER_DIR/eglfs.patch
#カメラ関係でビルドが通らないのを回避
rm -rf qtmultimedia/config.tests/gstreamer_encodingprofiles

popd
fi #if [ ! -d $QT_SOURCE_NAME ]; then

#mkspecをコピー
cd $QT_SOURCE_NAME
cp -rf $INSTALLER_DIR/linux-pandora-g++ qtbase/mkspecs/
sed -i -e "s|\$\$PNDSDK|$HOME/pandora-dev/arm|" qtbase/mkspecs/linux-pandora-g++/qmake.conf

cd $SDKHOME/$QT_INSTALLNAME
#make confclean -j3
../$QT_SOURCE_NAME/configure -opensource -confirm-license -prefix $PNDSDK/usr/local/$QT_INSTALLNAME -xplatform linux-pandora-g++ -static -opengl es2 -c++11 -qpa xcb -qt-xcb -no-xinput2 -no-icu -no-pulseaudio -no-sql-sqlite -nomake examples -skip qtlocation -continue -silent


#echo "Hit Enter.";read Wait
#並列ビルドの場合依存関係でビルドに失敗することがあるので2回までmakeする。
$makeparallel || $makeparallel && rm -rf $PNDSDK/usr/local/$QT_INSTALLNAME || $makeinstall
ln -snf $PNDSDK/usr/local/$QT_INSTALLNAME $PNDSDK/usr/local/qt5

