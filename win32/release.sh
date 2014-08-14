#ビルドしたバイナリのフォルダで実行
SCRIPTDIR=`dirname $0`
#引数としてバージョンを指定すること
VERSION=$1
RELEASEDIR=$PWD/../../PC6001VX_release
rm -rf $RELEASEDIR

#Windows版バイナリ
WINBINNAME=PC6001VX_$VERSION\_win
WINBINDIR=$RELEASEDIR/$WINBINNAME

mkdir -p $WINBINDIR
cp -f PC6001VX.exe $WINBINDIR
cp -f $SCRIPTDIR/../README.txt $WINBINDIR
cp -f $SCRIPTDIR/safemode.bat $WINBINDIR

pushd $RELEASEDIR
zip -r $WINBINNAME.zip $WINBINNAME
popd

#ソースtarball
pushd $SCRIPTDIR/..
SOURCENAME=PC6001VX_$VERSION\_src
git archive --format=tar --prefix=$SOURCENAME/ HEAD | gzip > $RELEASEDIR/$SOURCENAME.tar.gz
popd
