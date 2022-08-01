/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <cctype>
//#include <cstdarg>
//#include <cstdio>

#include "common.h"
#include "console.h"
#include "error.h"
#include "log.h"
#include "osd.h"


#define	BLNKW	(2)			// 横方向の余白
#define	BLNKH	(2)			// 縦方向の余白


VSurface JFont::ZFont;		// 全角フォントデータサーフェス
VSurface JFont::HFont;		// 半角フォントデータサーフェス
int JFont::zWidth  = 0;		// 全角文字の幅
int JFont::zHeight = 0;		//           高さ
int JFont::hWidth  = 0;		// 半角文字の幅
int JFont::hHeight = 0;		//           高さ


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
JFont::JFont( void )
{
	VRect ff;
	
	// ダミーフォント(半角)
	if( HFont.GetPixels().empty() ){
		hWidth  = FSIZE;
		hHeight = FSIZE * 2;
		
		HFont.InitSurface( hWidth * 192, hHeight * 2 );
		HFont.Fill( FC_BLACK );
		ff.w = hWidth  - 1;
		ff.h = hHeight - 1;
		for( int y = 0; y < 2; y++ ){
			ff.y = hHeight * y + 1;
			for( int x = 0; x < 192; x++ ){
				ff.x = hWidth * x + 1;
				HFont.Fill( FC_WHITE4, &ff );
			}
		}
	}
	
	// ダミーフォント(全角)
	if( ZFont.GetPixels().empty() ){
		zWidth  = FSIZE * 2;
		zHeight = FSIZE * 2;
		
		ZFont.InitSurface( zWidth * 192, zHeight * 48 );
		ZFont.Fill( FC_BLACK );
		ff.w = zWidth  - 1;
		ff.h = zHeight - 1;
		for( int y = 0; y < 48; y++ ){
			ff.y = zHeight * y + 1;
			for( int x = 0; x < 192; x++ ){
				ff.x = zWidth * x + 1;
				ZFont.Fill( FC_WHITE4, &ff );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
JFont::~JFont( void ){}


/////////////////////////////////////////////////////////////////////////////
// フォントファイルを開く
/////////////////////////////////////////////////////////////////////////////
bool JFont::OpenFont( const P6VPATH& zfilename, const P6VPATH& hfilename )
{
	VSurface* fnt;
	
	// フォントファイル読み込み
	if( (fnt = LoadImg( hfilename )) ){ HFont = *fnt; }
	if( (fnt = LoadImg( zfilename )) ){ ZFont = *fnt; }
	
	// 半角と全角でサイズが異なった場合は小さいほうに合わせる(当然表示がズレる)
	hWidth  = min( HFont.Width()  / 192, ZFont.Width()  / 192 / 2 );
	hHeight = min( HFont.Height() / 2,   ZFont.Height() / 48 );
	zWidth  = hWidth * 2;
	zHeight = hHeight;
	
	return ( Error::GetError() == Error::NoError );
}




/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
ZCons::ZCons( void ) : Xmax( 0 ), Ymax( 0 ), x( 0 ), y( 0 ), fgc( FC_WHITE4 ), bgc( FC_BLACK ), Caption( "" )
{
}

/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
ZCons::~ZCons( void ){}



/////////////////////////////////////////////////////////////////////////////
// コンソール作成(文字数でサイズ指定)
/////////////////////////////////////////////////////////////////////////////
bool ZCons::Init( int winx, int winy, const std::string& caption, BYTE fcol, BYTE bcol )
{
	int winxr = winx * hWidth  + BLNKW * 2;
	int winyr = winy * hHeight + BLNKH * 2;
	
	return InitRes( winxr, winyr, caption, fcol, bcol );
}


/////////////////////////////////////////////////////////////////////////////
// コンソール作成(解像度でサイズ指定)
/////////////////////////////////////////////////////////////////////////////
bool ZCons::InitRes( int winx, int winy, const std::string& caption, BYTE fcol, BYTE bcol )
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
	
	if( !caption.empty() ){	// キャプションあり(フレームあり)の場合
		// キャプション保存
		Caption = caption;
		
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


/////////////////////////////////////////////////////////////////////////////
// カーソル位置設定
/////////////////////////////////////////////////////////////////////////////
void ZCons::Locate( int xx, int yy )
{
	// 右端，下端チェック
	// 負だったら右端，下端から
	if( ( xx >= 0 ) && ( xx < Xmax ) )           { x = xx; }
	else if( ( xx < 0 ) && ( (Xmax - xx) >= 0 ) ){ x = Xmax + xx; }
	
	if( ( yy >= 0 ) && ( yy < Ymax ) )           { y = yy; }
	else if( ( yy < 0 ) && ( (Ymax - yy) >= 0 ) ){ y = Ymax + yy; }
}


/////////////////////////////////////////////////////////////////////////////
// カーソル位置設定(間接座標)
/////////////////////////////////////////////////////////////////////////////
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
	if( y < 0 ){
		y = 0;
	}
	if( y > Ymax ){
		y = Ymax;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 描画色設定
/////////////////////////////////////////////////////////////////////////////
void ZCons::SetColor( BYTE fg, BYTE bg )
{
	fgc = fg;
	bgc = bg;
}

void ZCons::SetColor( BYTE fg )
{
	fgc = fg;
}


/////////////////////////////////////////////////////////////////////////////
// 画面消去
/////////////////////////////////////////////////////////////////////////////
void ZCons::Cls( void )
{
	// 描画範囲を背景色で塗りつぶす
	VSurface::Fill( bgc, &con );
	// カーソルをホームに戻す
	x = y = 0;
}


/////////////////////////////////////////////////////////////////////////////
// 半角1文字描画
/////////////////////////////////////////////////////////////////////////////
void ZCons::PutCharH( BYTE c )
{
	PutCharh( x * hWidth + con.x, y * hHeight + con.y, c, fgc, bgc );
	
	// 次のカーソルを設定
	x++;
}


/////////////////////////////////////////////////////////////////////////////
// 全角1文字描画(SJIS)
/////////////////////////////////////////////////////////////////////////////
void ZCons::PutCharZ( WORD c )
{
	PutCharz( x * hWidth + con.x, y * hHeight + con.y, c, fgc, bgc );
	
	// 次のカーソルを設定
	x += 2;
}


/////////////////////////////////////////////////////////////////////////////
// 文字列描画(制御文字対応)
/////////////////////////////////////////////////////////////////////////////
void ZCons::sprintc( const std::string& text )
{
	std::string str = text;
	OSD_UTF8toSJIS( str );
	
	for( size_t i = 0; i < str.length(); i++ ){
		switch( str[i] ){
		case '\n':	// 改行
			x = 0;
			y++;
			break;
			
		case '\b':	// Back space
			LocateR( -1, 0 );
			PutCharH( ' ' );
			LocateR( -1, 0 );
			break;
			
		default:	// 普通の文字
			if( std::isprint( str[i] ) ){
				PutCharH( str[i] );
			}else{
				PutCharZ( ((BYTE)str[i] << 8) | (BYTE)str[i+1] );
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


/////////////////////////////////////////////////////////////////////////////
// 文字列描画(右詰め)
// (現在のカーソル位置に対して右詰め))
/////////////////////////////////////////////////////////////////////////////
void ZCons::sprintr( const std::string& text )
{
	std::string str = text;
	OSD_UTF8toSJIS( str );
	
	size_t num = min( str.length(), x + 1 );
	Locate( x - num + 1, y );
	
	for( size_t i = max( str.length() - num, 0 ); i < str.length(); i++ ){
		if( std::isprint( str[i] ) ){
			PutCharH( str[i] );
		}else{
			PutCharZ( ((BYTE)str[i] << 8) | (BYTE)str[i+1] );
			i++;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// 横最大文字数取得
/////////////////////////////////////////////////////////////////////////////
int ZCons::GetXline( void )
{
	return Xmax;
}


/////////////////////////////////////////////////////////////////////////////
// 縦最大文字数取得
/////////////////////////////////////////////////////////////////////////////
int ZCons::GetYline( void )
{
	return Ymax;
}


/////////////////////////////////////////////////////////////////////////////
// 枠描画
/////////////////////////////////////////////////////////////////////////////
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
	if( !Caption.empty() ){
		Locate( 1, 0 );
		sprintc( ' ' + Caption + ' ' );
	}
}


/////////////////////////////////////////////////////////////////////////////
// スクロールアップ
/////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////
// 半角文字描画
/////////////////////////////////////////////////////////////////////////////
void ZCons::PutCharh( int dx, int dy, BYTE txt, BYTE fg, BYTE bg )
{
	PRINTD( GRP_LOG, "[ZCons][PutCharh]\n" );
	
	int index = txt;
	
	// クリッピング
	VRect sr,dr;
	sr.x = ( index % 128 ) * JFont::FontWidth();
	sr.y = ( index / 128 ) * JFont::FontHeight();
	dr.x = dx;
	dr.y = dy;
	sr.w = dr.w = JFont::FontWidth();
	sr.h = dr.h = JFont::FontHeight();
	
	// 転送
	for( int y = 0; y < sr.h; y++ ){
		for( int x = 0; x < sr.w; x++ ){
			VSurface::PSet( dr.x + x, dr.y + y, JFont::HFont.PGet( sr.x + x, sr.y + y ) ? fg : bg );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// 全角文字描画
/////////////////////////////////////////////////////////////////////////////
void ZCons::PutCharz( int dx, int dy, WORD txt, BYTE fg, BYTE bg )
{
	PRINTD( GRP_LOG, "[ZCons][PutCharz]\n" );
	
	BYTE th = (txt >> 8) & 0xff;
	BYTE tl =  txt       & 0xff;
	
	// 上位 0x81 - 0x9f, 0xe0 - 0xef
	// 下位 0x40 - 0x7e, 0x80 - 0xfc
	int index = ( th - (th < 0xe0 ? 0x80 : 0xc0) ) * 192 + tl - 0x40;
	
	// クリッピング
	VRect sr,dr;
	sr.x = ( index % 192 ) * JFont::FontWidth() * 2;
	sr.y = ( index / 192 ) * JFont::FontHeight();
	dr.x = dx;
	dr.y = dy;
	sr.w = dr.w = JFont::FontWidth() * 2;
	sr.h = dr.h = JFont::FontHeight();
	
	// 転送
	for( int y = 0; y < sr.h; y++ ){
		for( int x = 0; x < sr.w; x++ ){
			VSurface::PSet( dr.x + x, dr.y + y, JFont::ZFont.PGet( sr.x + x, sr.y + y ) ? fg : bg );
		}
	}
}

