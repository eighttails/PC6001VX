#include <new>
#include <limits.h>

#include "common.h"
#include "vsurface.h"



////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
VSurface::VSurface( void ) : w(0), h(0), pitch(0), aspect(1.0), pixels(NULL) {}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
VSurface::~VSurface( void )
{
	if( pixels ) delete [] (BYTE *)pixels;
}



////////////////////////////////////////////////////////////////
// 初期化(サーフェスサイズ指定)
//
// 引数:	ww		幅
//			hh		高さ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool VSurface::InitSurface( int ww, int hh )
{
	// サイズチェック
	if( ww <= 0 || ww > 0xffff || hh <= 0 || hh > 0xffff ) return false;
	
	// 作成済みならいったん開放
	if( pixels ) delete [] (BYTE *)pixels;
	
	// メモリ確保
	pitch = (ww*sizeof(DWORD))&0xfffffffc;
	pixels = new BYTE[pitch*hh];
	if( !pixels ){
		w = h = pitch = rect.x = rect.y = rect.w = rect.h = 0;
		return false;
	}
	
	w = ww;
	h = hh;
	rect.x = rect.y = 0;
	rect.w = w;
	rect.h = h;
	
	return true;
}


////////////////////////////////////////////////////////////////
// 描画領域設定
// 引数:	xx,yy	座標
//			ww,hh	幅,高さ
// 返値:	なし
////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////
// アスペクト比取得
// 引数:	なし
// 返値:	double アスペクト比(幅を1とした場合の高さの比率)
////////////////////////////////////////////////////////////////
double VSurface::GetAspectRatio()
{
    return aspect;
}

////////////////////////////////////////////////////////////////
// アスペクト比設定
// 引数:	aspectRatio アスペクト比(幅を1とした場合の高さの比率)
// 返値:	なし
////////////////////////////////////////////////////////////////
void VSurface::SetAspectRatio(double aspectRatio)
{
    aspect = aspectRatio;
}


////////////////////////////////////////////////////////////////
// 描画領域取得
// 引数:	なし
// 返値:	VRect *	描画領域へのポインタ
////////////////////////////////////////////////////////////////
VRect *VSurface::GetRect( void )
{
	return &rect;
}


////////////////////////////////////////////////////////////////
// 幅取得
// 引数:	なし
// 返値:	int		幅
////////////////////////////////////////////////////////////////
int VSurface::Width( void )
{
	return w;
}


////////////////////////////////////////////////////////////////
// 高さ取得
// 引数:	なし
// 返値:	int		高さ
////////////////////////////////////////////////////////////////
int VSurface::Height( void )
{
	return h;
}


////////////////////////////////////////////////////////////////
// 1ラインのbyte数取得
// 引数:	なし
// 返値:	int		1ラインのbyte数
////////////////////////////////////////////////////////////////
int VSurface::Pitch( void )
{
	return pitch;
}


////////////////////////////////////////////////////////////////
// ピクセルデータへのポインタ取得
// 引数:	なし
// 返値:	BYTE *	ピクセルデータへのポインタ
////////////////////////////////////////////////////////////////
void *VSurface::GetPixels( void )
{
	return pixels;
}


////////////////////////////////////////////////////////////////
// 点を描く
//
// 引数:	x,y				座標
//			col				カラーコード
// 返値:	なし
////////////////////////////////////////////////////////////////
void VSurface::PSet( int x, int y, DWORD col )
{
	if( pixels && x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h ){
		DWORD *p = (DWORD *)pixels + y * pitch/sizeof(DWORD) + x;
		*p = col;
	}
}


////////////////////////////////////////////////////////////////
// 色を取得
//
// 引数:	x,y				座標
// 返値:	BYTE			カラーコード
////////////////////////////////////////////////////////////////
DWORD VSurface::PGet( int x, int y )
{
	DWORD res = 0;
	
	if( pixels && x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h ){
		DWORD *p = (DWORD *)pixels + y * pitch/sizeof(DWORD)+ x;
		res  = *p;
	}
	return res;
}


////////////////////////////////////////////////////////////////
// 矩形領域を塗りつぶす
// 引数:	col		色
//			rc		塗りつぶし範囲(NULLなら既定値)
// 返値:	なし
////////////////////////////////////////////////////////////////
void VSurface::Fill( DWORD col, VRect *rc )
{
	VRect rr;
	
	if( !pixels ) return;
	
	if( rc ){
		RectAdd( &rr, &rect, rc );
	}else{
		rr.x = rect.x;
		rr.y = rect.y;
		rr.w = rect.w;
		rr.h = rect.h;
	}
	
	if( rr.w && rr.h ){
		for( int i=0; i < rr.h; i++ ){
			DWORD *p = (DWORD *)pixels + (rr.y + i) * pitch/sizeof(DWORD) + rr.x;
			for( int j=0; j < rr.w; j++ )
				*p++ = col;
		}
	}
}


////////////////////////////////////////////////////////////////
// 転送する
//
// 引数:	srect			転送元範囲
//			dst				転送先サーフェスへのポインタ
//			drect			転送先範囲(x,yのみ有効)
// 返値:	なし
////////////////////////////////////////////////////////////////
void VSurface::Blit( VRect *srect, VSurface *dst, VRect *drect )
{
	VRect src1,drc1,src2,drc2;
	
	if( !dst ) return;
	
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
	
	if( src2.w > drc2.w ) src2.w = drc2.w;
	if( src2.h > drc2.h ) src2.h = drc2.h;
	
	if( !src2.w || !src2.h || !drc2.w || !drc2.h ) return;
	
	DWORD *psrc = (DWORD *)pixels           + src2.y * pitch/sizeof(DWORD)        + src2.x;
	DWORD *pdst = (DWORD *)dst->GetPixels() + drc2.y * dst->Pitch()/sizeof(DWORD) + drc2.x;
	
	for( int i=0; i < src2.h; i++ ){
		memcpy( pdst, psrc, src2.w * sizeof(DWORD) );
		psrc += pitch/sizeof(DWORD);
		pdst += dst->Pitch()/sizeof(DWORD);
	}
}

