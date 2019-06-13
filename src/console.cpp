#include <stdarg.h>
#include <ctype.h>

#include "log.h"
#include "console.h"
#include "common.h"
#include "p6vxcommon.h"


#define	BLNKW	(2)	/* 横方向の余白 */
#define	BLNKH	(2)	/* 縦方向の余白 */


VSurface *JFont::ZFont = NULL;	// 全角フォントデータサーフェスへのポインタ
VSurface *JFont::HFont = NULL;	// 半角フォントデータサーフェスへのポインタ
int JFont::zWidth  = 0;			// 全角文字の幅
int JFont::zHeight = 0;			//           高さ
int JFont::hWidth  = 0;			// 半角文字の幅
int JFont::hHeight = 0;			//           高さ


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
JFont::JFont( void ){}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
JFont::~JFont( void ){}


////////////////////////////////////////////////////////////////
// フォントファイルを開く
////////////////////////////////////////////////////////////////
bool JFont::OpenFont( char *zfilename, char *hfilename )
{
	VRect ff;
	
	// 既に読込まれていたら破棄する
	CloseFont();
	
	// とりあえずサイズ指定
	hWidth  = FSIZE;
	hHeight = FSIZE * 2;
	zWidth  = FSIZE * 2;
	zHeight = FSIZE * 2;
	
	// フォントファイル読み込み
	HFont = LoadImg( hfilename );
	ZFont = LoadImg( zfilename );
	
	// フォントファイルが無ければダミー作成
	if( !HFont ){
		HFont = new VSurface;
		HFont->InitSurface( hWidth*96*2, hHeight* 2 );
	}
	if( !ZFont ){
		ZFont = new VSurface;
		ZFont->InitSurface( zWidth*96*2, zHeight*96 );
	}
	
	// 半角と全角でサイズが異なった場合は小さいほうに合わせる(当然表示がズレる)
	hWidth  = min( HFont->Width(), ZFont->Width() ) / 96 / 2;
	hHeight = min( HFont->Height() / 2, ZFont->Height() / 96 );
	zWidth  = hWidth * 2;
	zHeight = hHeight;
	
	return true;
}


////////////////////////////////////////////////////////////////
// フォントを破棄する
////////////////////////////////////////////////////////////////
void JFont::CloseFont( void )
{
	if( HFont ){
		delete HFont;
		HFont = NULL;
	}
	if( ZFont ){
		delete ZFont;
		ZFont = NULL;
	}
}


////////////////////////////////////////////////////////////////
// 半角文字描画
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
void JFont::PutCharh( VSurface *dst, int dx, int dy, BYTE txt, BYTE  fg, BYTE  bg )
#else			// 32bit
void JFont::PutCharh( VSurface *dst, int dx, int dy, BYTE txt, DWORD fg, DWORD bg )
#endif

{
	PRINTD( GRP_LOG, "[JFont][PutCharh]\n" );
	
	int index = txt;
	
	// クリッピング
	VRect sr,dr;
	sr.x = ( index % 128 ) * hWidth;
	sr.y = ( index / 128 ) * hHeight;
	dr.x = dx;
	dr.y = dy;
	sr.w = dr.w = hWidth;
	sr.h = dr.h = hHeight;
	
	// 転送
	for( int y=0; y<sr.h; y++ )
		for( int x=0; x<sr.w; x++ )
#if INBPP == 8	// 8bit
			dst->PSet( dr.x + x, dr.y + y, HFont && HFont->PGet( sr.x + x, sr.y + y )                           ? fg : bg );
#else			// 32bit
			dst->PSet( dr.x + x, dr.y + y, HFont && HFont->PGet( sr.x + x, sr.y + y )&(RMASK32|GMASK32|BMASK32) ? fg : bg );
#endif
}


////////////////////////////////////////////////////////////////
// 全角文字描画
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
void JFont::PutCharz( VSurface *dst, int dx, int dy, WORD txt, BYTE  fg, BYTE  bg )
#else			// 32bit
void JFont::PutCharz( VSurface *dst, int dx, int dy, WORD txt, DWORD fg, DWORD bg )
#endif
{
	PRINTD( GRP_LOG, "[JFont][PutCharz]\n" );
	
	BYTE high = (txt>>8) & 0xff;
	BYTE low  =  txt     & 0xff;
	
	Convert2Jis( &high, &low );
	int index = ( high - 0x20 ) * 96 + low - 0x20;
	
	// クリッピング
	VRect sr,dr;
	sr.x = ( index % 96 ) * zWidth;
	sr.y = ( index / 96 ) * zHeight;
	dr.x = dx;
	dr.y = dy;
	sr.w = dr.w = zWidth;
	sr.h = dr.h = zHeight;
	
	// 転送
	for( int y=0; y<sr.h; y++ )
		for( int x=0; x<sr.w; x++ )
#if INBPP == 8	// 8bit
			dst->PSet( dr.x + x, dr.y + y, ZFont && ZFont->PGet( sr.x + x, sr.y + y )                           ? fg : bg );
#else			// 32bit
			dst->PSet( dr.x + x, dr.y + y, ZFont && ZFont->PGet( sr.x + x, sr.y + y )&(RMASK32|GMASK32|BMASK32) ? fg : bg );
#endif
}







////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
ZCons::ZCons( void ) : Xmax(0), Ymax(0), x(0), y(0), fgc(FC_WHITE), bgc(FC_BLACK)
{
	INITARRAY( Caption, '\0' );
}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
ZCons::~ZCons( void ){}



////////////////////////////////////////////////////////////////
// コンソール作成(文字数でサイズ指定)
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
bool ZCons::Init( int winx, int winy, const char *caption, BYTE  fcol, BYTE  bcol )
#else			// 32bit
bool ZCons::Init( int winx, int winy, const char *caption, DWORD fcol, DWORD bcol )
#endif
{
	int winxr = winx * hWidth  + BLNKW * 2;
	int winyr = winy * hHeight + BLNKH * 2;
	
	return InitRes( winxr, winyr, caption, fcol, bcol );
}


////////////////////////////////////////////////////////////////
// コンソール作成(解像度でサイズ指定)
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
bool ZCons::InitRes( int winx, int winy, const char *caption, BYTE  fcol, BYTE  bcol )
#else			// 32bit
bool ZCons::InitRes( int winx, int winy, const char *caption, DWORD fcol, DWORD bcol )
#endif
{
	// サーフェス作成
	if( !VSurface::InitSurface( winx, winy ) ) return false;
	
	fgc = fcol;
	bgc = bcol;
	
	// サーフェス全体を背景色で塗りつぶす
	VSurface::Fill( bgc );
	
	// 縦横最大文字数設定
	Xmax = ( winx - BLNKW * 2 ) / hWidth;
	Ymax = ( winy - BLNKH * 2 ) / hHeight;
	
	// 描画範囲設定
	con.x = BLNKW;
	con.y = BLNKH;
	con.w = Xmax * hWidth;
	con.h = Ymax * hHeight;
	
	if( caption ){	// キャプションあり(フレームあり)の場合
		// キャプション保存
		strncpy( Caption, caption, min( Xmax-2, (int)sizeof(Caption)-1 ) );
		
		// フレーム描画
		DrawFrame();
		
		// 縦横最大文字数設定
		Xmax -= 2;
		Ymax -= 2;
		
		// 描画範囲設定
		con.x += hWidth;
		con.y += hHeight;
		con.w  = Xmax * hWidth;
		con.h  = Ymax * hHeight;
	}
	
	x = y = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// カーソル位置設定
////////////////////////////////////////////////////////////////
void ZCons::Locate( int xx, int yy )
{
	// 右端，下端チェック
	// 負だったら右端，下端から
	if( ( xx >= 0 )&&( xx < Xmax ) ) x = xx;
	else if( ( xx < 0 )&&( (Xmax-xx)>=0 ) ) x = Xmax + xx;
	
	if( ( yy >= 0 )&&( yy < Ymax ) ) y = yy;
	else if( ( yy < 0 )&&( (Ymax-yy)>=0 ) ) y = Ymax + yy;
}


////////////////////////////////////////////////////////////////
// カーソル位置設定(間接座標)
////////////////////////////////////////////////////////////////
void ZCons::LocateR( int xx, int yy )
{
	x += xx;
	if( x < 0 ){
		x += Xmax;
		y--;
	}
	if( x > Xmax ){
		x -= Xmax;
		y++;
	}
	
	y += yy;
	if( y < 0 )    y = 0;
	if( y > Ymax ) y = Ymax;
}


////////////////////////////////////////////////////////////////
// 描画色設定
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
void ZCons::SetColor( BYTE  fg, BYTE  bg )
#else			// 32bit
void ZCons::SetColor( DWORD fg, DWORD bg )
#endif
{
	fgc = fg;
	bgc = bg;
}

#if INBPP == 8	// 8bit
void ZCons::SetColor( BYTE  fg )
#else			// 32bit
void ZCons::SetColor( DWORD fg )
#endif
{
	fgc = fg;
}


////////////////////////////////////////////////////////////////
// 画面消去
////////////////////////////////////////////////////////////////
void ZCons::Cls( void )
{
	// 描画範囲を背景色で塗りつぶす
	if( VSurface::pixels ) VSurface::Fill( bgc, &con );
	// カーソルをホームに戻す
	x = y = 0;
}


////////////////////////////////////////////////////////////////
// 半角1文字描画
////////////////////////////////////////////////////////////////
void ZCons::PutCharH( BYTE c )
{
	JFont::PutCharh( this, x * hWidth + con.x, y * hHeight + con.y, c, fgc, bgc );
	
	// 次のカーソルを設定
	x++;
}


////////////////////////////////////////////////////////////////
// 全角1文字描画
////////////////////////////////////////////////////////////////
void ZCons::PutCharZ( WORD c )
{
	JFont::PutCharz( this, x * hWidth + con.x, y * hHeight + con.y, c, fgc, bgc );
	
	// 次のカーソルを設定
	x += 2;
}

#include <QString>
#include <QTextCodec>
////////////////////////////////////////////////////////////////
// 書式付文字列描画(制御文字非対応)
////////////////////////////////////////////////////////////////
void ZCons::Print( const char *text, ... )
{
	char buf[1024];
	int num = 0;
	va_list ap;

	// 可変長引数展開（文字列に変換）
	va_start( ap, text );
	vsprintf(buf, TRANS(text), ap);
	QString str = buf;
	const QByteArray array = QTextCodec::codecForName("Shift-JIS")->fromUnicode(str);

	for( int i=0; i<array.size(); i++ ){
		if( isprint( array[i] ) )
			PutCharH( array[i] );
		else{
			const unsigned char c1 = array[i];
			const unsigned char c2 = array[i+1];
			PutCharZ( c1<<8 | c2 );
			i++;
		}
	}
}


////////////////////////////////////////////////////////////////
// 書式付文字列描画(制御文字対応)
////////////////////////////////////////////////////////////////
void ZCons::Printf( const char *text, ... )
{
	char buf[1024];
	int num = 0;
	va_list ap;

	// 可変長引数展開（文字列に変換）
	va_start( ap, text );
	vsprintf(buf, TRANS(text), ap);
	QString str = buf;
	const QByteArray array = QTextCodec::codecForName("Shift-JIS")->fromUnicode(str);
	for( int i=0; i<array.size(); i++ ){
		switch( array[i] ){
		case '\n':	// 改行
			x = 0;
			y++;
			break;

		default:	// 普通の文字
			if( isprint( array[i] ) )
				PutCharH( array[i] );
			else{
				const unsigned char c1 = array[i];
				const unsigned char c2 = array[i+1];
				PutCharZ( c1<<8 | c2 );
				i++;
			}

			// 次のカーソルを設定
			if( x >= Xmax ){
				x = 0;
				y++;
			}
		}

		// スクロール?
		if( y >= Ymax){
			y = Ymax - 1;
			ScrollUp();
		}
	}
}


////////////////////////////////////////////////////////////////
// 書式付文字列描画(右詰め)
////////////////////////////////////////////////////////////////
void ZCons::Printfr( const char *text, ... )
{
	char buf[1024];
	int num = 0;
	va_list ap;

	// 可変長引数展開（文字列に変換）
	va_start( ap, text );
	vsprintf(buf, TRANS(text), ap);
	QString str = buf;
	const QByteArray array = QTextCodec::codecForName("Shift-JIS")->fromUnicode(str);

	if( array.size() > Xmax ) num = Xmax;
	Locate( -array.size(), y );

	for( int i=0; i<array.size(); i++ ){
		if( isprint( array[i] ) )
			PutCharH( array[i] );
		else{
			const unsigned char c1 = array[i];
			const unsigned char c2 = array[i+1];
			PutCharZ( c1<<8 | c2 );
			i++;
		}
	}
}


////////////////////////////////////////////////////////////////
// 横最大文字数取得
////////////////////////////////////////////////////////////////
int ZCons::GetXline( void )
{
	return Xmax;
}


////////////////////////////////////////////////////////////////
// 縦最大文字数取得
////////////////////////////////////////////////////////////////
int ZCons::GetYline( void )
{
	return Ymax;
}


////////////////////////////////////////////////////////////////
// 枠描画
////////////////////////////////////////////////////////////////
void ZCons::DrawFrame( void )
{
	VRect frm;
	
	frm.x = con.x;
	frm.y = con.y + 4;
	frm.w = con.w;
	frm.h = con.h - 8;
	VSurface::Fill( fgc, &frm );
	
	frm.x += 1;
	frm.y += 1;
	frm.w -= 2;
	frm.h -= 2;
	VSurface::Fill( bgc, &frm );
	
	// キャプション
	if( strlen( Caption ) > 0 ){
		Locate( 1, 0 );
		Print( " %s ", Caption );
	}
}


////////////////////////////////////////////////////////////////
// スクロールアップ
////////////////////////////////////////////////////////////////
void ZCons::ScrollUp( void )
{
	VRect SPos,DPos;
	
	// 転送元
	SPos.x = con.x;
	SPos.y = con.y + hHeight;
	SPos.w = con.w;
	SPos.h = con.h - hHeight;
	
	// 転送先
	DPos.x = con.x;
	DPos.y = con.y;
	
	// スクロール
	VSurface::Blit( &SPos, this, &DPos );
	
	
	// 描画範囲を背景色で塗りつぶす
	DPos.x = con.x;
	DPos.y = con.y + con.h - hHeight;
	DPos.w = con.w;
	DPos.h = hHeight;
	VSurface::Fill( bgc, &DPos );
}
