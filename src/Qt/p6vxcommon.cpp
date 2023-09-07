#include "../log.h"
#include "../common.h"
#include "./p6vxcommon.h"
#include "./p6vxapp.h"

#include <QVector>
#include <QImage>
#include <QRgb>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QUrl>

extern QVector<QRgb> PaletteTable;              // パレットテーブル

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
bool SaveImgData( const P6VPATH& filename, BYTE *pixels, const int bpp, const int ww, const int hh, VRect *pos )
{
	PRINTD( GRP_LOG, "[COMMON][SaveImg] -> %s\n", P6VPATH2STR(filename).c_str() );
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	VRect rec;
	// 領域設定
	if( pos ){
		rec.x = pos->x;	rec.y = pos->y;
		rec.w = pos->w;	rec.h = pos->h;
	}else{
		rec.x =     rec.y = 0;
		rec.w = ww; rec.h = hh;
	}

	QImage image(rec.w, rec.h, QImage::Format_RGB888);
	if(bpp == 8){
		image.setColorTable(app->getPaletteTable());
	}

	memcpy( image.bits(), pixels, image.sizeInBytes());

	auto saveFilePath = QDir::cleanPath(P6VPATH2QSTR(filename));
	auto saveDir = QFileInfo(saveFilePath).absoluteDir();
	// P6VXでは独自にファイル名をつける(filenameは無視する)
	auto dt = QDateTime::currentDateTime();
	auto d = dt.date();
	auto t = dt.time();
	auto saveFileName = QString("P6VX%1%2%3%4%5%6.png")
			.arg(d.year(), 4)
			.arg(d.month(), 2, 10, QLatin1Char('0'))
			.arg(d.day(), 2, 10, QLatin1Char('0'))
			.arg(t.hour(), 2, 10, QLatin1Char('0'))
			.arg(t.minute(), 2, 10, QLatin1Char('0'))
			.arg(t.second(), 2, 10, QLatin1Char('0'));

	auto saveFileFullPath = QDir(saveDir).filePath(saveFileName);
	image.save(saveFileFullPath);

#ifdef Q_OS_ANDROID
#if 0 //#TODO
	// Androidの場合はインテントで他のアプリに送る
	PlatformShareUtils util;
	int req = 0;
	bool altImpl = false;
	util.sendFile(saveFileFullPath, "Snapshot", "image/png", req, altImpl);
#endif
#endif

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
bool SaveImg( const P6VPATH& filename, VSurface *sur, VRect *pos )
{
	return SaveImgData( filename, (BYTE *)sur->GetPixels().data(), INBPP, sur->Width(), sur->Height(), pos );
}

////////////////////////////////////////////////////////////////
// Img LOAD
//			 「プログラミング・ライブラリ」 http://dencha.ojaru.jp/
//
// 引数:	filename		読込むファイル名
// 返値:	VSurface *		読込まれたサーフェスへのポインタ
////////////////////////////////////////////////////////////////
VSurface *LoadImg( const P6VPATH& filepath )
{
	PRINTD( GRP_LOG, "[COMMON][LoadImg] <- %s\n", P6VPATH2STR(filepath).c_str() );

	// 画像を読み込む
	QImage loadImage(P6VPATH2QSTR(filepath));
	QImage image = loadImage.convertToFormat(QImage::Format_Indexed8);

	// サーフェスを作成
	VSurface* sur = new VSurface;
	sur->InitSurface( image.width(), image.height() );

	// 画像データを取得
	BYTE *doff = (BYTE *)sur->GetPixels().data();
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
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDateTime>
#include <QDateTime>

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
	case LEFT: // 左
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

QString EncodeContentURI(const QString &uri)
{
	if (uri.startsWith("content:")){
		// AndroidのContentURIの日本語部分をURLエンコードする
		qDebug() << "       URI" << uri;
		auto parts = uri.split("%3A");
		QStringList encodedParts;
		for (auto s : parts){
			auto subParts = s.split("%");
			QStringList encodedSubParts;
			for ( auto ss : subParts){
				encodedSubParts.append(QUrl::toPercentEncoding(ss, ":/"));
			}
			encodedParts.append(encodedSubParts.join("%"));
		}
		auto encodedURI = encodedParts.join("%3A");
		qDebug() << "encodedURI" << encodedURI;
		return encodedURI;
	} else {
		// ContentURIでないものはそのまま返す
		return uri;
	}
}
