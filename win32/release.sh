#このスクリプトの置き場所をカレントとして実行すること。
#カレントディレクトリ
export SCRIPT_DIR=$PWD

#引数としてバージョンを指定すること
if [ -z $1 ]; then
    echo "Please specify build number."
    exit
fi

VERSION=$1
RELEASE_DIR=$PWD/../../PC6001VX_release
rm -rf $RELEASE_DIR


bash $SCRIPT_DIR/buildrelease.sh
cd $SCRIPT_DIR/../../PC6001VX-build/release

#Windows版バイナリ
WIN_BIN_NAME=PC6001VX_$VERSION\_win
WIN_BIN_DIR=$RELEASE_DIR/$WIN_BIN_NAME

mkdir -p $WIN_BIN_DIR
cp -f PC6001VX.exe $WIN_BIN_DIR
cp -f $SCRIPT_DIR/../README.html $WIN_BIN_DIR
cp -rf $SCRIPT_DIR/../doc $WIN_BIN_DIR
cp -f $SCRIPT_DIR/safemode.bat $WIN_BIN_DIR

pushd $RELEASE_DIR
zip -r $WIN_BIN_NAME.zip $WIN_BIN_NAME
popd

#ソースtarball
pushd $SCRIPT_DIR/..
SOURCENAME=PC6001VX_$VERSION\_src
git archive --format=tar --prefix=$SOURCENAME/ HEAD | gzip > $RELEASE_DIR/$SOURCENAME.tar.gz
popd
