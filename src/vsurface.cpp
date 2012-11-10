#include <new>
#include <limits.h>

#include "common.h"
#include "vsurface.h"

#define	RMASK32	0x00ff0000
#define	GMASK32	0x0000ff00
#define	BMASK32	0x000000ff

#define	RMASK16	0x0000f800
#define	GMASK16	0x000007e0
#define	BMASK16	0x0000001f


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
VSurface::VSurface( void ) : w(0), h(0), bpp(0), pitch(0), pixels(NULL), palette(NULL),
	rmask(0), gmask(0), bmask(0), rshift(0), gshift(0),bshift(0),
	rloss(0), gloss(0), bloss(0) {}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
VSurface::~VSurface( void )
{
	if( pixels ) delete [] (BYTE *)pixels;
	
	if( palette ){
		if( palette->colors ) delete [] palette->colors;
		delete palette;
	}
}



////////////////////////////////////////////////////////////////
// 初期化(サーフェスサイズ指定)
//
// 引数:	ww,hh	幅,高さ
//			bpp		1ピクセルのbit数
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool VSurface::InitSurface( int ww, int hh, int bp )
{
	// サイズチェック
	if( ww <= 0 || ww > 0xffff || hh <= 0 || hh > 0xffff ) return false;
	
	if( bp != 8 && bp != 16 && bp != 24 ) return false;
	
	// 作成済みならいったん開放
	if( pixels ) delete [] (BYTE *)pixels;
	if( palette ){
		if( palette->colors ) delete [] palette->colors;
		delete palette;
		palette = NULL;
	}
	
	// メモリ確保
	pitch = (ww*(bp/8)+3)&0xfffffffc;
	pixels = new BYTE[pitch*hh];
	if( !pixels ){
		w = h = bpp = pitch = rect.x = rect.y = rect.w = rect.h = 0;
		return false;
	}
	
	// パレット作成(8bppのみ)
	if( bp == 8 ){
		palette = new VPalette;
		palette->ncols  = 256;
		palette->colors = new COLOR24[256];
		
		for( int i=0; i<palette->ncols; i++ ){
			palette->colors[i].r        = 0;
			palette->colors[i].g        = 0;
			palette->colors[i].b        = 0;
			palette->colors[i].reserved = 0;
		}
	}
	
	w = ww;
	h = hh;
	bpp = bp;
	rect.x = rect.y = 0;
	rect.w = w;
	rect.h = h;
	
	switch( bpp ){
	case 16: SetMask( RMASK16, GMASK16, BMASK16 ); break;
	case 24: SetMask( RMASK32, GMASK32, BMASK32 ); break;
	}
	
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
// 描画領域取得
// 引数:	なし
// 返値:	VRect *	描画領域へのポインタ
////////////////////////////////////////////////////////////////
VRect *VSurface::GetRect( void )
{
	return &rect;
}


////////////////////////////////////////////////////////////////
// パレット設定
// 引数:	colors	パレットデータへのポインタ
//			ncols	設定する色数
// 返値:	int		設定された色数
////////////////////////////////////////////////////////////////
int VSurface::SetPalette( COLOR24 *cols, int ncols )
{
	if( !cols || (ncols < 1) || (bpp != 8) ) return 0;
	
	palette->ncols  = ncols;
	for( int i=0; i<palette->ncols; i++ ){
		palette->colors[i].r = cols[i].r;
		palette->colors[i].g = cols[i].g;
		palette->colors[i].b = cols[i].b;
	}
	
	return palette->ncols;
}


////////////////////////////////////////////////////////////////
// パレット取得
// 引数:	なし
// 返値:	VPalette *	パレットへのポインタ
////////////////////////////////////////////////////////////////
VPalette *VSurface::GetPalette( void )
{
	return palette;
}


////////////////////////////////////////////////////////////////
// カラーマスク設定
// 引数:	rm			赤マスク
//			gm			緑マスク
//			bm			青マスク
// 返値:	VPalette *	パレットへのポインタ
////////////////////////////////////////////////////////////////
void VSurface::SetMask( DWORD rm, DWORD gm, DWORD bm )
{
	rmask = rm;
	gmask = gm;
	bmask = bm;
	rshift = gshift = bshift = 0;
	rloss  = gloss  = bloss  = 0;
	
	switch( bpp ){
	case 16:
		while( !(rmask&1) && rshift<16 ){ rshift++; rmask >>= 1; }
		while( !(gmask&1) && gshift<16 ){ gshift++; gmask >>= 1; }
		while( !(bmask&1) && bshift<16 ){ bshift++; bmask >>= 1; }
		while( !(rmask&0x80) && rloss<8 ){ rloss++; rmask <<= 1; }
		while( !(gmask&0x80) && gloss<8 ){ gloss++; gmask <<= 1; }
		while( !(bmask&0x80) && bloss<8 ){ bloss++; bmask <<= 1; }
		break;
		
	case 24:
		while( !(rmask&1) && rshift<24 ){ rshift++; rmask >>= 1; }
		while( !(gmask&1) && gshift<24 ){ gshift++; gmask >>= 1; }
		while( !(bmask&1) && bshift<24 ){ bshift++; bmask >>= 1; }
		while( !(rmask&0x80) && rloss<8 ){ rloss++; rmask <<= 1; }
		while( !(gmask&0x80) && gloss<8 ){ gloss++; gmask <<= 1; }
		while( !(bmask&0x80) && bloss<8 ){ bloss++; bmask <<= 1; }
	}
	
	rmask = rm;
	gmask = gm;
	bmask = bm;
}


////////////////////////////////////////////////////////////////
// RGBマスク取得
// 引数:	なし
// 返値:	DWORD		マスク
////////////////////////////////////////////////////////////////
DWORD VSurface::Rmask( void ){ return rmask; }
DWORD VSurface::Gmask( void ){ return gmask; }
DWORD VSurface::Bmask( void ){ return bmask; }


////////////////////////////////////////////////////////////////
// RGB右シフト取得
// 引数:	なし
// 返値:	int		右シフト数
////////////////////////////////////////////////////////////////
int VSurface::Rshift( void ){ return rshift; }
int VSurface::Gshift( void ){ return gshift; }
int VSurface::Bshift( void ){ return bshift; }


////////////////////////////////////////////////////////////////
// RGB左シフト取得
// 引数:	なし
// 返値:	int		左シフト数
////////////////////////////////////////////////////////////////
int VSurface::Rloss( void ){ return rloss; }
int VSurface::Gloss( void ){ return gloss; }
int VSurface::Bloss( void ){ return bloss; }


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
// 1ピクセルのbyte数取得
// 引数:	なし
// 返値:	int		1ピクセルのbyte数
////////////////////////////////////////////////////////////////
int VSurface::Bpp( void )
{
	return bpp;
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
	BYTE *p;
	
	if( pixels && x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h ){
		p = (BYTE *)pixels + y * pitch + x * bpp / 8;
		switch( bpp ){
		case 8:
			*p   =  col     &0xff;
			break;
			
		case 16:
			*p++ =  col     &0xff;
			*p   = (col>> 8)&0xff;
			break;
			
		case 24:
			*p++ =  col     &0xff;
			*p++ = (col>> 8)&0xff;
			*p   = (col>>16)&0xff;
			break;
		default:
			break;
		}
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
	BYTE *p;
	DWORD res = 0;
	
	if( pixels && x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h ){
		p = (BYTE *)pixels + y * pitch + x * bpp / 8;
		switch( bpp ){
		case 8:
			res  = (DWORD)(*p);
			break;
			
		case 16:
			res  = (DWORD)(*p++);
			res |= (DWORD)(*p  )<< 8;
			break;
			
		case 24:
			res  = (DWORD)(*p++);
			res |= (DWORD)(*p++)<< 8;
			res |= (DWORD)(*p  )<<16;
			break;
		default:
			break;
		}
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
		BYTE *p = (BYTE *)pixels + rr.y * pitch + rr.x * bpp / 8;
		for( int i=0; i < rr.h; i++ ){
			switch( bpp ){
			case 8:
				memset( p, col, rr.w );
				break;
				
			case 16:
				for( int j=0; j < rr.w; j++ ){
					*p     =  col     &0xff;
					*(p+1) = (col>> 8)&0xff;
				}
				break;
				
			case 24:
				for( int j=0; j < rr.w; j++ ){
					*p     =  col     &0xff;
					*(p+1) = (col>> 8)&0xff;
					*(p+2) = (col>>16)&0xff;
				}
				break;
			default:
				break;
			}
			p += pitch;
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
	
	BYTE *psrc = (BYTE *)pixels           + src2.y * pitch        + src2.x * bpp        / 8;
	BYTE *pdst = (BYTE *)dst->GetPixels() + drc2.y * dst->Pitch() + drc2.x * dst->Bpp() / 8;
	
	if( dst->Bpp() == bpp ){	// 転送先と転送元のbppが同じ場合
		for( int i=0; i < src2.h; i++ ){
			memcpy( pdst, psrc, src2.w * bpp / 8 );
			psrc += pitch;
			pdst += dst->Pitch();
		}
	}else if( dst->Bpp() == 16 && bpp == 8 ){	// 8 -> 16 bpp (5:6:5)
		for( int i=0; i < src2.h; i++ ){
			for( int j=0; j < src2.w; j++ ){
				WORD wdat = (WORD)(palette->colors[*psrc].r & 0xf8) << 8
				          | (WORD)(palette->colors[*psrc].g & 0xfc) << 3
				          | (WORD)(palette->colors[*psrc].b & 0xf8) >> 3;
				*pdst++ = (BYTE)( wdat       & 0xff);
				*pdst++ = (BYTE)((wdat >> 8) & 0xff);
				psrc++;
			}
			psrc += pitch - src2.w;
			pdst += dst->Pitch() - src2.w * 2;
		}
	}else if( dst->Bpp() == 24 && bpp == 8 ){	// 8 -> 24 bpp
		for( int i=0; i < src2.h; i++ ){
			for( int j=0; j < src2.w; j++ ){
				*pdst++ = palette->colors[*psrc].b;
				*pdst++ = palette->colors[*psrc].g;
				*pdst++ = palette->colors[*psrc].r;
				psrc++;
			}
			psrc += pitch - src2.w;
			pdst += dst->Pitch() - src2.w * 3;
		}
	}
}

