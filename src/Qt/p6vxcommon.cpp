#include "../log.h"
#include "../common.h"
#include "../p6vxcommon.h"
#include "../p6vxapp.h"

#include <QVector>
#include <QImage>
#include <QRgb>

extern QVector<QRgb> PaletteTable;              //パレットテーブル

////////////////////////////////////////////////////////////////
// Img SAVE from Data
//   情報参照「PNG利用術」 http://gmoon.jp/png/
//			 「プログラミング・ライブラリ」 http://dencha.ojaru.jp/
//
// 引数:	filename		保存ファイル名
//			pixels			保存するデータ領域へのポインタ
//			bpp				色深度
//			ww				幅
//			hh				高さ
//			pos				保存する領域情報へのポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool SaveImgData( const char *filename, BYTE *pixels, const int bpp, const int ww, const int hh, VRect *pos )
{
	PRINTD( GRP_LOG, "[COMMON][SaveImg] -> %s\n", filename );

	VRect rec;
	int pitch = ww * bpp / 8;

	// 領域設定
	if( pos ){
		rec.x = pos->x;	rec.y = pos->y;
		rec.w = pos->w;	rec.h = pos->h;
	}else{
		rec.x =     rec.y = 0;
		rec.w = ww; rec.h = hh;
	}

	QImage::Format format = (bpp == 8) ? QImage::Format_Indexed8 : QImage::Format_RGB32;

	QImage image(rec.w, rec.h, format);
	if(bpp == 8){
		image.setColorTable(PaletteTable);
	}

	BYTE *doff = pixels + rec.x + rec.y * pitch;
	for( int i=0; i<rec.h; i++ ){
		memcpy( image.scanLine(i), doff, rec.w * bpp / 8);
		doff += pitch;
	}

	image.save(filename);

	return true;
}


////////////////////////////////////////////////////////////////
// Img SAVE
//			 「プログラミング・ライブラリ」 http://dencha.ojaru.jp/
//
// 引数:	filename		保存ファイル名
//			sur				保存するサーフェスへのポインタ
//			pos				保存する領域情報へのポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool SaveImg( const char *filename, VSurface *sur, VRect *pos )
{
	return SaveImgData( filename, (BYTE *)sur->GetPixels(), INBPP, sur->Width(), sur->Height(), pos );
}

////////////////////////////////////////////////////////////////
// Img LOAD
//			 「プログラミング・ライブラリ」 http://dencha.ojaru.jp/
//
// 引数:	filename		読込むファイル名
// 返値:	VSurface *		読込まれたサーフェスへのポインタ
////////////////////////////////////////////////////////////////
VSurface *LoadImg( const char *filename )
{
	PRINTD( GRP_LOG, "[COMMON][LoadImg] <- %s\n", filename );

	// 画像を読み込む
	QImage loadImage(filename);
	QImage image = loadImage.convertToFormat(QImage::Format_Indexed8);

	// サーフェスを作成
	VSurface* sur = new VSurface;
	sur->InitSurface( image.width(), image.height() );

	// 画像データを取得
	BYTE *doff = (BYTE *)sur->GetPixels();
	for( int i=0; i<(int)sur->Height(); i++ ){
		memcpy( doff, image.scanLine(i), sur->Width() * image.depth() / 8 );
		doff += sur->Pitch();
	}

	return sur;
}


////////////////////////////////////////////////////////////////
// 矩形領域合成
//
// 引数:	rr		合成結果を出力する矩形領域へのポインタ
//			r1,r2	合成する矩形領域へのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void RectAdd( VRect *rr, VRect *r1, VRect *r2 )
{
	if( !rr || !r1 || !r2 ) return;

	int x1 = qMax( r1->x, r2->x );
	int y1 = qMax( r1->y, r2->y );
	int x2 = qMin( r1->x + r1->w - 1, r2->x + r2->w - 1);
	int y2 = qMin( r1->y + r1->h - 1, r2->y + r2->h - 1);

	rr->w = x1 > x2 ? 0 : x2 - x1 + 1;
	rr->h = y1 > y2 ? 0 : y2 - y1 + 1;

	rr->x = rr->w ? x1 : 0;
	rr->y = rr->h ? y1 : 0;
}

#include <QString>
#include <QByteArray>
#include <QMutex>

////////////////////////////////////////////////////////////////
// UTF8->Local(Windowsの場合SJIS,Linuxの場合UTF8)
//
// 引数:	str				SJIS文字列へのポインタ
// 返値:	char *			Local文字列へのポインタ
////////////////////////////////////////////////////////////////
char *UTF8toLocal( const char *str )
{
	//文字コード変換用バッファ
	static QByteArray array;
	QMutex mutex;
	QMutexLocker lock(&mutex);

	QString qStr = QString::fromUtf8(str);
	array = qStr.toLocal8Bit();
	return array.data();
}

void TiltScreen(TiltDirection dir)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	app->setTiltDirection(dir);
}

void UpdateTilt()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	if(!app->isTiltEnabled()) return;

	const int maxStep = 30;
	const int t = app->getTiltStep();
	switch (app->getTiltDirection()){
	case LEFT: //左
		app->setTiltStep(qMax(-maxStep, t - 1));
		break;
	case RIGHT: // 右
		app->setTiltStep(qMin(maxStep, t + 1));
		break;
	case NEWTRAL:
		app->setTiltStep(t == 0 ? 0 : t > 0 ? t - 1 : t + 1);
	default:;
	}
}
