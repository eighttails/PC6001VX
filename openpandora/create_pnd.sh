#バイナリが生成されたディレクトリにカレントを移して実行すること

SHDIR=`dirname $0`
echo $SHDIR

rm -rf pnd
mkdir pnd

cp PC6001VX pnd/

cp $SHDIR/PXML.xml pnd
cp $SHDIR/PC-6001.png pnd
cp $SHDIR/PC6001VX.sh pnd
cp $SHDIR/../README.txt pnd

cp $PNDSDK/usr/lib/libGL.so.1 pnd
cp $PNDSDK/usr/lib/libGLU.so.1 pnd
cp $PNDSDK/usr/lib/libXxf86vm.so.1 pnd
cp $PNDSDK/usr/lib/libxcb.so.1 pnd
cp $PNDSDK/usr/lib/libX11-xcb.so.1 pnd
cp $PNDSDK/usr/lib/libX11.so.6 pnd

chmod +x pnd/PC6001VX
chmod +x pnd/PC6001VX.sh

~/pandora-dev/sdk_utils/pnd_make.sh -d pnd -p PC6001VX_.pnd -c
cat PC6001VX_.pnd $SHDIR/PXML.xml $SHDIR/PC-6001.png > PC6001VX.pnd
rm PC6001VX_.pnd

