/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <stdexcept>

#include "common.h"
#include "vsurface.h"


std::array<DWORD,256> VSurface::col32;	// 32bitカラーテーブル


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
VSurface::VSurface( void ) : w( 0 ), h( 0 ), pitch( 0 )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
VSurface::~VSurface( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// 矩形領域合成
//
// 引数:	rr		合成結果を出力する矩形領域へのポインタ
//			r1,r2	合成する矩形領域へのポインタ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VSurface::RectAdd( VRect* rr, VRect* r1, VRect* r2 )
{
	if( !rr || !r1 || !r2 ){
		return;
	}
	
	int x1 = max( r1->x, r2->x );
	int y1 = max( r1->y, r2->y );
	int x2 = min( r1->x + r1->w - 1, r2->x + r2->w - 1);
	int y2 = min( r1->y + r1->h - 1, r2->y + r2->h - 1);
	
	rr->w = x1 > x2 ? 0 : x2 - x1 + 1;
	rr->h = y1 > y2 ? 0 : y2 - y1 + 1;
	
	rr->x = rr->w ? x1 : 0;
	rr->y = rr->h ? y1 : 0;
}


/////////////////////////////////////////////////////////////////////////////
// 初期化(サーフェスサイズ指定)
//
// 引数:	ww		幅
//			hh		高さ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool VSurface::InitSurface( int ww, int hh )
{
	// 作成済み かつ サイズ同じならそのまま戻る
	if( w == ww && h == hh ){
		return true;
	}
	
	// 作成済みならいったん開放
	w = h = pitch = rect.x = rect.y = rect.w = rect.h = 0;
	
	// サイズチェック
	if( ww <= 0 || ww > 0xffff || hh <= 0 || hh > 0xffff ){
		pixels.clear();
		return false;
	}
	
	// メモリ確保
	pitch  = ((ww + 3) >> 2) << 2;	// 4の倍数
	pixels.resize( pitch * hh );
	
	w = ww;
	h = hh;
	rect.x = rect.y = 0;
	rect.w = w;
	rect.h = h;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 描画領域設定
// 引数:	xx,yy	座標
//			ww,hh	幅,高さ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VSurface::SetRect( int xx, int yy, int ww, int hh )
{
	VRect r1,r2;
	
	r1.x = 0;
	r1.y = 0;
	r1.w = w;
	r1.h = h;
	
	r2.x = xx;
	r2.y = xx;
	r2.w = ww > 0 ? ww : 0;
	r2.h = hh > 0 ? hh : 0;
	
	RectAdd( &rect, &r1, &r2 );
}


/////////////////////////////////////////////////////////////////////////////
// 描画領域取得
// 引数:	なし
// 返値:	VRect*	描画領域へのポインタ
/////////////////////////////////////////////////////////////////////////////
VRect* VSurface::GetRect( void )
{
	return &rect;
}


/////////////////////////////////////////////////////////////////////////////
// 幅取得
// 引数:	なし
// 返値:	int		幅
/////////////////////////////////////////////////////////////////////////////
int VSurface::Width( void ) const
{
	return w;
}


/////////////////////////////////////////////////////////////////////////////
// 高さ取得
// 引数:	なし
// 返値:	int		高さ
/////////////////////////////////////////////////////////////////////////////
int VSurface::Height( void ) const
{
	return h;
}


/////////////////////////////////////////////////////////////////////////////
// 1ラインのbyte数取得
// 引数:	なし
// 返値:	int		1ラインのbyte数
/////////////////////////////////////////////////////////////////////////////
int VSurface::Pitch( void ) const
{
	return pitch;
}


/////////////////////////////////////////////////////////////////////////////
// ピクセルデータ取得
// 引数:	なし
// 返値:	std::vector<BYTE>&	ピクセルデータ
/////////////////////////////////////////////////////////////////////////////
std::vector<BYTE>& VSurface::GetPixels( void )
{
	return pixels;
}


/////////////////////////////////////////////////////////////////////////////
// 点を描く
//
// 引数:	x,y				座標
//			col				カラーコード
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VSurface::PSet( int x, int y, BYTE col )
{
	if( !pixels.empty() && x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h ){
		try{
			pixels.at( y * pitch + x ) = col;
		}
		catch( std::out_of_range& ){}
	}
}


/////////////////////////////////////////////////////////////////////////////
// 色を取得
//
// 引数:	x,y				座標
// 返値:	BYTE			カラーコード
/////////////////////////////////////////////////////////////////////////////
BYTE VSurface::PGet( int x, int y )
{
	BYTE res = 0;
	
	if( !pixels.empty() && x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h ){
		try{
			res = pixels.at( y * pitch + x );
		}
		catch( std::out_of_range& ){
			res = 0;
		}
	}
	return res;
}


/////////////////////////////////////////////////////////////////////////////
// 矩形領域を塗りつぶす
// 引数:	col		色
//			rc		塗りつぶし範囲(nullptrなら既定値)
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VSurface::Fill( BYTE col, VRect* rc )
{
	VRect rr;
	
	if( pixels.empty() ){
		return;
	}
	
	if( rc ){
		RectAdd( &rr, &rect, rc );
	}else{
		rr.x = rect.x;
		rr.y = rect.y;
		rr.w = rect.w;
		rr.h = rect.h;
	}
	
	if( rr.w && rr.h ){
		for( int i = 0; i < rr.h; i++ ){
			for( int j = 0; j < rr.w; j++ ){
				try{
					pixels.at( (rr.y + i) * pitch + rr.x + j ) = col;
				}
				catch( std::out_of_range& ){}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// 転送する
//
// 引数:	srect			転送元範囲
//			dst				転送先サーフェスへのポインタ
//			drect			転送先範囲(x,yのみ有効)
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VSurface::Blit( VRect* srect, VSurface* dst, VRect* drect )
{
	VRect src1,drc1,src2,drc2;
	
	if( pixels.empty() || !dst ){
		return;
	}
	
	// 転送元範囲設定
	src1.x = srect ? srect->x : 0;
	src1.y = srect ? srect->y : 0;
	src1.w = srect ? srect->w : w;
	src1.h = srect ? srect->h : h;
	RectAdd( &src2, &src1, &rect );
	
	// 転送先設定
	drc1.x = drect ? drect->x : 0;
	drc1.y = drect ? drect->y : 0;
	drc1.w = src2.w;
	drc1.h = src2.h;
	RectAdd( &drc2, &drc1, dst->GetRect() );
	
	if( src2.w > drc2.w ){ src2.w = drc2.w; }
	if( src2.h > drc2.h ){ src2.h = drc2.h; }
	
	if( !src2.w || !src2.h || !drc2.w || !drc2.h ){
		return;
	}
	
	auto psrc = pixels.begin()           + src2.y * pitch        + src2.x;
	auto pdst = dst->GetPixels().begin() + drc2.y * dst->Pitch() + drc2.x;
	for( int i = 0; i < src2.h; i++ ){
		std::copy( psrc, psrc + src2.w, pdst );
		psrc += pitch;
		pdst += dst->Pitch();
	}
}


/////////////////////////////////////////////////////////////////////////////
// 32bitカラー設定
//
// 引数:	num				インデックス(0-255)
//			col				32bitカラーデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VSurface::SetColor( int num, DWORD col )
{
	try{
		col32.at( num & 0xff ) = col;
	}
	catch( std::out_of_range& ){}
}


/////////////////////////////////////////////////////////////////////////////
// 32bitカラー取得
//
// 引数:	num				インデックス(0-255)
// 返値:	DWORD			32bitカラーデータ
/////////////////////////////////////////////////////////////////////////////
DWORD VSurface::GetColor( int num )
{
	try{
		return col32.at( num & 0xff );
	}
	catch( std::out_of_range& ){
		return 0;
	}
}
