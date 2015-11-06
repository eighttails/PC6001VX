#このスクリプトの置き場所をカレントとして実行すること。
#カレントディレクトリ
export SCRIPT_DIR=$PWD

#並列ビルド
MINGW32MAKE="mingw32-make -j$NUMBER_OF_PROCESSORS"

P6VX_DBUILD_DIR=PC6001VX-build

pushd $SCRIPT_DIR/../../
if [ -e $P6VX_DBUILD_DIR ]; then 
	rm -rf $P6VX_DBUILD_DIR
fi
mkdir $P6VX_DBUILD_DIR

cd $P6VX_DBUILD_DIR
/mingw32/local/bin/qmake ../PC6001VX/PC6001VX.pro 
$MINGW32MAKE debug
$MINGW32MAKE release

