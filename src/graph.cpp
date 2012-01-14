#include "pc6001v.h"
#include "log.h"
#include "graph.h"
#include "common.h"
#include "debug.h"
#include "osd.h"
#include "status.h"
#include "vdg.h"

// SDLサーフェス用オプション
#define SDLOP_SCREEN	(SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_HWACCEL)
#define SDLOP_SURFACE	(SDL_SWSURFACE | SDL_HWACCEL)

// 通常ウィンドウサイズ
#define	P6WINW		(vm->vdg->Width())
#define	P6WINH		(vm->vdg->Height())

#define	HBBUS		(282-12)	/* 4:3表示時画面高さ */

// フルスクリーンウィンドウサイズ
#define	P6WIFW		(640)
#define	P6WIFH		(480)

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
// デバッグモードウィンドウサイズ
#define	P6DEBUGW	(P6WINW+vm->regw->Width())
#define	P6DEBUGH	(max(P6WINH,vm->regw->Height()+vm->memw->Height())+vm->monw->Height())
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //


// モニタモード時はフルスクリーン，スキャンライン，4:3表示禁止
// フルスクリーンモード時はステータスバー表示禁止
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
#define	DISPMON		(vm->cfg->GetMonDisp())
#define	DISPFULL	(!DISPMON  && vm->cfg->GetFullScreen())
#define	DISPSCAN	(!DISPMON  && vm->cfg->GetScanLine())
#define	DISPNTSC	(!DISPMON  && vm->cfg->GetDispNTSC())
#define	DISPSTAT	(!DISPFULL && vm->cfg->GetStatDisp())

#else

#define	DISPFULL	(vm->cfg->GetFullScreen())
#define	DISPSCAN	(vm->cfg->GetScanLine())
#define	DISPNTSC	(vm->cfg->GetDispNTSC())
#define	DISPSTAT	(vm->cfg->GetStatDisp())

#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //



void DSP6::BlitToSDL( VSurface *src, int x, int y )
{
	VRect src1,drc1;
	
	if( !src ) return;
	SDL_Surface *dst  = SDL_GetVideoSurface();	// スクリーンサーフェスへのポインタ取得
	
	// 転送元範囲設定
	src1.x = max( 0, -x );
	src1.y = max( 0, -y );
	src1.w = min( src->Width()  - src1.x, dst->w );
	src1.h = min( src->Height() - src1.y, dst->h );
	
	if( src1.w <= 0 || src1.h <= 0 ) return;
	
	// 転送先範囲設定
	drc1.x = max( 0, x );
	drc1.y = max( 0, y );
	
	
	if( SDL_MUSTLOCK( dst ) ) SDL_LockSurface( dst );
	
	BYTE *psrc = (BYTE *)src->GetPixels() + src->Pitch() * src1.y + src1.x * src->Bpp() / 8;
	BYTE *pdst = (BYTE *)dst->pixels      + dst->pitch   * drc1.y + drc1.x * dst->format->BytesPerPixel;
	
	if( src->Bpp() == dst->format->BitsPerPixel ){
		for( int i=0; i < src1.h; i++ ){
			memcpy( pdst, psrc, src1.w * dst->format->BytesPerPixel );
			psrc += src->Pitch();
			pdst += dst->pitch;
		}
	}else if( src->Bpp() == 8 && dst->format->BitsPerPixel == 16 ){	// 8 -> 16 bpp (5:6:5)
		for( int i=0; i < src1.h; i++ ){
			for( int j=0; j < src1.w; j++ ){
				WORD wdat = (WORD)(Pal[*psrc].r & 0xf8) << 8
				          | (WORD)(Pal[*psrc].g & 0xfc) << 3
				          | (WORD)(Pal[*psrc].b & 0xf8) >> 3;
				*pdst++ = (BYTE)( wdat       & 0xff);
				*pdst++ = (BYTE)((wdat >> 8) & 0xff);
				psrc++;
			}
			psrc += src->Pitch() - src1.w;
			pdst += dst->pitch   - src1.w * 2;
		}
	}else if( src->Bpp() == 8 && dst->format->BitsPerPixel == 24 ){	// 8 -> 24 bpp
		for( int i=0; i < src1.h; i++ ){
			for( int j=0; j < src1.w; j++ ){
				*pdst++ = Pal[*psrc].b;
				*pdst++ = Pal[*psrc].g;
				*pdst++ = Pal[*psrc].r;
				psrc++;
			}
			psrc += src->Pitch() - src1.w;
			pdst += dst->pitch   - src1.w * 3;
		}
	}
	
	if( SDL_MUSTLOCK( dst ) ) SDL_UnlockSurface( dst );

}


void DSP6::BlitToSDL2( VSurface *src, int x, int y )
{
	VRect src1,drc1;
	
	if( !src ) return;
	SDL_Surface *dst  = SDL_GetVideoSurface();	// スクリーンサーフェスへのポインタ取得
	
	// 転送元範囲設定
	src1.x = max( 0, -x/2 );
	src1.y = max( 0, -y/2 );
	src1.w = min( src->Width()  - src1.x, dst->w / 2 );
	src1.h = min( src->Height() - src1.y, dst->h / 2 );
	
	if( src1.w <= 0 || src1.h <= 0 ) return;
	
	// 転送先範囲設定
	drc1.x = max( 0, x );
	drc1.y = max( 0, y );
	
	
	if( SDL_MUSTLOCK( dst ) ) SDL_LockSurface( dst );
	
	BYTE *psrc = (BYTE *)src->GetPixels() + src->Pitch() * src1.y + src1.x * src->Bpp() / 8;
	BYTE *pdst = (BYTE *)dst->pixels      + dst->pitch   * drc1.y + drc1.x * dst->format->BytesPerPixel;
	
	if( src->Bpp() == dst->format->BitsPerPixel ){
		switch( src->Bpp() ){
		case 8:
			for( int i=0; i < src1.h; i++ ){
				for( int j=0; j < src1.w; j++ ){
					*pdst++ = *psrc;
					*pdst++ = *psrc++;
				}
				psrc += src->Pitch()   - src1.w;
				pdst += dst->pitch * 2 - src1.w * 2;
			}
			break;
			
		case 16:
			for( int i=0; i < src1.h; i++ ){
				for( int j=0; j < src1.w; j++ ){
					WORD d1 = *(WORD *)psrc++;
					psrc++;
					*((WORD *)pdst++) = d1;
					pdst++;
					*((WORD *)pdst++) = d1;
					pdst++;
				}
				psrc += src->Pitch()   - src1.w * 2;
				pdst += dst->pitch * 2 - src1.w * 4;
			}
			break;
			
		case 24:
			for( int i=0; i < src1.h; i++ ){
				for( int j=0; j < src1.w; j++ ){
					BYTE b = *psrc++;
					BYTE g = *psrc++;
					BYTE r = *psrc++;
					*pdst++ = b;
					*pdst++ = g;
					*pdst++ = r;
					*pdst++ = b;
					*pdst++ = g;
					*pdst++ = r;
				}
				psrc += src->Pitch()   - src1.w * 3;
				pdst += dst->pitch * 2 - src1.w * 6;
			}
		}
		
		pdst = (BYTE *)dst->pixels + dst->pitch * drc1.y + drc1.x * dst->format->BytesPerPixel;
		for( int i=0; i < src1.h; i++ ){
			memcpy( pdst + dst->pitch, pdst, src1.w * 2 * dst->format->BytesPerPixel );
			pdst += dst->pitch * 2;
		}
	}
	
	if( SDL_MUSTLOCK( dst ) ) SDL_UnlockSurface( dst );
}


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
DSP6::DSP6( VM6 *vm, const P6ID& id ) : P6DEVICE(vm,id)
{
	SBuf = NULL;
	Pal  = NULL;
	Bpp  = DEFAULT_COLOR_MODE;
	SLBr = DEFAULT_SCANLINEBR;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSP6::~DSP6( void )
{
	if( SBuf ) delete SBuf;
}


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	bpp		1ピクセルのbit数
//			br		スキャンライン輝度
//			pal		パレットへのポインタ
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL DSP6::Init( int bpp, int br, COLOR24 *pal )
{
	PRINTD( GRP_LOG, "[GRP][Init]\n" );
	
	Bpp  = bpp;
	SLBr = br;
	Pal  = pal;
	
	// スクリーンサーフェス作成
	if( !SetScreenSurface() ) return FALSE;
	
	// ステータスバー初期化
	if( !vm->staw->Init( SDL_GetVideoSurface()->w ) ) return FALSE;
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// スクリーンサーフェス作成
//
// 引数:	なし
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL DSP6::SetScreenSurface( void )
{
	PRINTD( GRP_LOG, "[GRP][SetScreenSurface]\n" );
	
	int x = 0, y = 0, opt = 0;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( DISPMON ){	// モニタモード?
		x   = P6DEBUGW;
		y   = P6DEBUGH;
		opt = SDLOP_SCREEN;
		
		PRINTD2( GRP_LOG, " -> Monitor Mode ( X:%d Y:%d )\n", x, y );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	{
		if( DISPFULL ){	// フルスクリーン?
			// Mac のフルスクリーンは 640x480, 800x600, 1024x768 に限られるらしい。by Windyさん
			// とりあえず問答無用で640x480に固定してみる
			x   = P6WIFW;
			y   = P6WIFH;
			opt = SDLOP_SCREEN | SDL_FULLSCREEN;
			
			PRINTD3( GRP_LOG, " -> FullScreen ( X:%d Y:%d %dbpp)\n", x, y, Bpp );
		}else{
			x   = P6WINW * ( DISPSCAN ? 2 : 1 );
			y   = ( DISPNTSC ? HBBUS : P6WINH ) * ( DISPSCAN ? 2 : 1 ) + ( DISPSTAT ? vm->staw->Height() : 0 );
			opt = SDLOP_SCREEN;
			
			PRINTD3( GRP_LOG, " -> Window ( X:%d Y:%d %dbpp )\n", x, y, Bpp );
		}
	}
	
	// スクリーンサーフェス作成
	if( !SDL_SetVideoMode( x, y, Bpp, opt ) ) return FALSE;
	
	PRINTD3( GRP_LOG, " -> OK ( %d x %d x %d )\n", SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h, SDL_GetVideoSurface()->format->BitsPerPixel );
	
	if( DISPFULL ){	// フルスクリーンの時
		// マウスカーソルを消す
		SDL_ShowCursor( SDL_DISABLE );
	}else{			// フルスクリーンでない時
		// マウスカーソルを表示する
		SDL_ShowCursor( SDL_ENABLE );
		// ウィンドウキャプション設定
		SDL_WM_SetCaption( vm->cfg->GetCaption(), "" );
	}
	
	// スクリーンサーフェスにパレットを選択する
	if( SDL_GetVideoSurface()->format->BitsPerPixel == 8 )
		if( !SDL_SetPalette( SDL_GetVideoSurface(), SDL_LOGPAL|SDL_PHYSPAL, (SDL_Color *)Pal, 0, 256 ) ) return FALSE;
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// スクリーンサイズ変更
//
// 引数:	なし
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL DSP6::ResizeScreen( void )
{
	PRINTD( GRP_LOG, "[GRP][ResizeScreen]\n" );
	
	int x,y;
	SDL_Surface *scr  = SDL_GetVideoSurface();	// スクリーンサーフェスへのポインタ取得
	
	// ウィンドウサイズチェック
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( DISPMON ){	// モニタモード?
		x = P6DEBUGW;
		y = P6DEBUGH;
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	{
		x = ( DISPFULL ? P6WIFW : P6WINW * ( DISPSCAN ? 2 : 1 ) );
		y = ( DISPFULL ? P6WIFH : ( DISPNTSC ? HBBUS : P6WINH ) * ( DISPSCAN ? 2 : 1 ) );
		
		// ステータスバー表示?
		if( !DISPFULL && DISPSTAT ) y += vm->staw->Height();
	}
	
	// ウィンドウサイズが不適切なら作り直す
	if( !scr || (x != scr->w) || (y != scr->h) ){
		if( !SetScreenSurface() ) return FALSE;
		vm->staw->Init( SDL_GetVideoSurface()->w );	// ステータスバーも
	}else
		// 作り直さない場合は現在のスクリーンサーフェスをクリア
		SDL_FillRect( scr, NULL, 0 );
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 画面更新
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void DSP6::DrawScreen( void )
{
	PRINTD( GRP_LOG, "[GRP][DrawScreen]\n" );
	
	SDL_Surface *scr = SDL_GetVideoSurface();	// スクリーンサーフェスへのポインタ取得
	VSurface *BBuf = vm->vdg;		// バックバッファへのポインタ取得
	
	if( !scr || !BBuf ) return;
	
	// スクリーンサーフェスにblit
	PRINTD( GRP_LOG, " -> Blit" );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( DISPMON ){	// モニタモード?
		PRINTD( GRP_LOG, " -> Monitor" );
		
		// バックバッファ
		BlitToSDL( BBuf, 0, 0 );
		
		// モニタウィンドウ
		BlitToSDL( vm->monw, 0, max( P6WINH, vm->regw->Height() + vm->memw->Height() ) );
		
		// レジスタウィンドウ
		BlitToSDL( vm->regw, P6WINW, 0 );
		
		// メモリウィンドウ
		BlitToSDL( vm->memw, P6WINW, vm->regw->Height() );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	{
		UpdateSubBuf();	// サブバッファ更新
		
		if( DISPFULL ){	// フルスクリーン
			PRINTD( GRP_LOG, " -> FullScreen" );
			if( DISPSCAN ){	// スキャンラインありの場合
				PRINTD( GRP_LOG, "(Scan line)" );
				BlitToSDL( SBuf, ( scr->w - SBuf->Width() ) / 2, ( scr->h - SBuf->Height() ) / 2 );
			}else{
				BlitToSDL2( SBuf, ( scr->w - SBuf->Width() * 2 ) / 2, ( scr->h - SBuf->Height() * 2 ) / 2 );
			}
		}else{			// フルスクリーンでない
			PRINTD( GRP_LOG, " -> Window" );
			BlitToSDL( SBuf, 0, 0 );
		}
		
		// ステータスバー
		if( DISPSTAT ){
			PRINTD( GRP_LOG, " -> Statusbar" );
			
			// ステータスバー更新
			// スクリーンサーフェス下端に位置を合わせてblit
			vm->staw->Update();
			
			BlitToSDL( (VSurface *)vm->staw, 0, SDL_GetVideoSurface()->h - vm->staw->Height() );
		}
	}
	
	PRINTD( GRP_LOG, " -> OK\n" );
	
	// フリップ
 	SDL_Flip( scr );
}


////////////////////////////////////////////////////////////////
// 有効スクリーン幅取得
//
// 引数:	なし
// 返値:	int		有効スクリーン幅ピクセル数
////////////////////////////////////////////////////////////////
int DSP6::ScreenX( void )
{
	return P6WINW * ( DISPSCAN ? 2 : 1 );
}


////////////////////////////////////////////////////////////////
// 有効スクリーン高さ取得
//
// 引数:	なし
// 返値:	int		有効スクリーン高さピクセル数
////////////////////////////////////////////////////////////////
int DSP6::ScreenY( void )
{
	return ( DISPNTSC ? HBBUS : P6WINH ) * ( DISPSCAN ? 2 : 1 );
}


////////////////////////////////////////////////////////////////
// サブバッファ取得
//
// 引数:	なし
// 返値:	VSurface *	サブバッファへのポインタ
////////////////////////////////////////////////////////////////
VSurface *DSP6::GetSubBuffer( void )
{
	return SBuf;
}


////////////////////////////////////////////////////////////////
// サブバッファ更新
//
// 引数:	なし
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
#define	RESO43		(8)		/* 中間色計算用分解能(1ラインをRESO43分割する) */
#define	THRE43		(0)		/* 中間色処理のしきい値(これより大きければ中間色処理) */
BOOL DSP6::UpdateSubBuf( void )
{
	PRINTD( GRP_LOG, "[GRP][UpdateSubBuf]\n" );
	
	VSurface *BBuf = vm->vdg;
	
	if( !BBuf ) return FALSE;
	
	if( !RefreshSubBuf() ) return FALSE;
	
	if( DISPNTSC ){	// 4:3表示有効の場合
		PRINTD( GRP_LOG, " -> 4:3" );
		if( DISPSCAN ){	// スキャンラインありの場合
			PRINTD( GRP_LOG, " -> ScanLine" );
			switch( SBuf->Bpp() ){
			case 8:		// 8bitモード
				PRINTD( GRP_LOG, " -> 8bit" );
				for( int y=0; y<SBuf->Height(); y+=2 ){
					int y0 = ( y * BBuf->Height() ) / SBuf->Height();
					
					DWORD *soff = (DWORD *)BBuf->GetPixels() + BBuf->Pitch()/sizeof(DWORD) * y0;
					DWORD *doff = (DWORD *)SBuf->GetPixels() + SBuf->Pitch()/sizeof(DWORD) * y;
					DWORD *eoff = doff + SBuf->Pitch()/sizeof(DWORD);
					DWORD s, d;
					
					// 8bitの時は中間色処理なし
					for( int x=0; x<(int)(BBuf->Width()/sizeof(DWORD)); x++ ){
						BYTE b1,b2,b3,b4;
						s = *soff++;
						DWTOB( s, b1, b2, b3, b4 );
						d = BTODW( b4, b4, b3, b3 );
						*doff++ = d;
						*eoff++ = d|0x80808080;
						d = BTODW( b2, b2, b1, b1 );
						*doff++ = d;
						*eoff++ = d|0x80808080;
					}
				}
				break;
				
			case 16:	// 16bitモード
				PRINTD( GRP_LOG, " -> 16bit" );
				for( int y=0; y<SBuf->Height(); y+=2 ){
					int y0 = ( y * BBuf->Height() ) / SBuf->Height();
					int a1 = ( y * BBuf->Height() * RESO43 ) / SBuf->Height() - y0 * RESO43;
					int a2 = RESO43 - a1;
					
					BYTE *soff = (BYTE *)BBuf->GetPixels() + BBuf->Pitch() * y0;
					DWORD *doff = (DWORD *)SBuf->GetPixels() + ( SBuf->Pitch() * y ) / sizeof(DWORD);
					DWORD *eoff = doff + SBuf->Pitch() / sizeof(DWORD);
					
					
					// しきい値より上?
					if( a1 > THRE43 && a2 > THRE43 ){
						for( int x=0; x<(int)BBuf->Width(); x++ ){
							BYTE d1 = *(soff);
							BYTE d2 = *((soff++)+BBuf->Pitch());
							
							BYTE r = ( Pal[d1].r * a2 + Pal[d2].r * a1 ) / RESO43;
							BYTE g = ( Pal[d1].g * a2 + Pal[d2].g * a1 ) / RESO43;
							BYTE b = ( Pal[d1].b * a2 + Pal[d2].b * a1 ) / RESO43;
							
							DWORD dd = ( (DWORD)( r & 0xf8 ) << 8 ) | ( (DWORD)( g & 0xfc ) << 3 ) | ( (DWORD)( b & 0xf8 ) >> 3 );
							dd |= dd << 16;
							*(doff++) = dd;
							
							r = ( r * SLBr ) / 100;
							g = ( g * SLBr ) / 100;
							b = ( b * SLBr ) / 100;
							
							DWORD ed = ( (DWORD)( r & 0xf8 ) << 8 ) | ( (DWORD)( g & 0xfc ) << 3 ) | ( (DWORD)( b & 0xf8 ) >> 3 );
							ed |= ed << 16;
							*(eoff++) = ed;
						}
					}else{
						for( int x=0; x<(int)BBuf->Width(); x++ ){
							BYTE d1 = *(soff++);
							
							BYTE r = Pal[d1].r;
							BYTE g = Pal[d1].g;
							BYTE b = Pal[d1].b;
							
							DWORD dd = ( (DWORD)( r & 0xf8 ) << 8 ) | ( (DWORD)( g & 0xfc ) << 3 ) | ( (DWORD)( b & 0xf8 ) >> 3 );
							dd |= dd << 16;
							*(doff++) = dd;
							
							r = ( r * SLBr ) / 100;
							g = ( g * SLBr ) / 100;
							b = ( b * SLBr ) / 100;
							
							DWORD ed = ( (DWORD)( r & 0xf8 ) << 8 ) | ( (DWORD)( g & 0xfc ) << 3 ) | ( (DWORD)( b & 0xf8 ) >> 3 );
							ed |= ed << 16;
							*(eoff++) = ed;
						}
					}
				}
				break;
				
			case 24:	// 24bitモード
				PRINTD( GRP_LOG, " -> 24bit" );
				for( int y=0; y<SBuf->Height(); y+=2 ){
					int y0 = ( y * BBuf->Height() ) / SBuf->Height();
					int a1 = ( y * BBuf->Height() * RESO43 ) / SBuf->Height() - y0 * RESO43;
					int a2 = RESO43 - a1;
					
					BYTE *soff = (BYTE *)BBuf->GetPixels() + BBuf->Pitch() * y0;
					BYTE *doff = (BYTE *)SBuf->GetPixels() + SBuf->Pitch() * y;
					BYTE *eoff = doff + SBuf->Pitch();
					
					
					// しきい値より上?
					if( a1 > THRE43 && a2 > THRE43 ){
						for( int x=0; x<(int)BBuf->Width(); x++ ){
							BYTE d1 = *(soff);
							BYTE d2 = *((soff++)+BBuf->Pitch());
							
							BYTE r = ( Pal[d1].r * a2 + Pal[d2].r * a1 ) / RESO43;
							BYTE g = ( Pal[d1].g * a2 + Pal[d2].g * a1 ) / RESO43;
							BYTE b = ( Pal[d1].b * a2 + Pal[d2].b * a1 ) / RESO43;
							
							PUT3BYTE( b, g, r, doff );
							PUT3BYTE( b, g, r, doff );
							
							r = ( r * SLBr ) / 100;
							g = ( g * SLBr ) / 100;
							b = ( b * SLBr ) / 100;
							
							PUT3BYTE( b, g, r, eoff );
							PUT3BYTE( b, g, r, eoff );
						}
					}else{
						for( int x=0; x<(int)BBuf->Width(); x++ ){
							BYTE d1 = *(soff++);
							
							BYTE r = Pal[d1].r;
							BYTE g = Pal[d1].g;
							BYTE b = Pal[d1].b;
							
							PUT3BYTE( b, g, r, doff );
							PUT3BYTE( b, g, r, doff );
							
							r = ( r * SLBr ) / 100;
							g = ( g * SLBr ) / 100;
							b = ( b * SLBr ) / 100;
							
							PUT3BYTE( b, g, r, eoff );
							PUT3BYTE( b, g, r, eoff );
						}
					}
				}
			}
		}else{			// スキャンラインなしの場合
			PRINTD( GRP_LOG, " -> No ScanLine" );
			switch( SBuf->Bpp() ){
			case 8:		// 8bitモード
				PRINTD( GRP_LOG, " -> 8bit" );
				for( int y=0; y<SBuf->Height(); y++ ){
					int y0 = ( y * BBuf->Height() ) / SBuf->Height();
					
					BYTE *soff = (BYTE *)BBuf->GetPixels() + BBuf->Pitch() * y0;
					BYTE *doff = (BYTE *)SBuf->GetPixels() + SBuf->Pitch() * y;
					
					// 8bitの時は中間色処理なし
					memcpy( doff, soff, SBuf->Pitch() );
				}
				break;
				
			case 16:	// 16bitモード
				PRINTD( GRP_LOG, " -> 16bit" );
				for( int y=0; y<SBuf->Height(); y++ ){
					int y0 = ( y * BBuf->Height() ) / SBuf->Height();
					int a1 = ( y * BBuf->Height() * RESO43 ) / SBuf->Height() - y0 * RESO43;
					int a2 = RESO43 - a1;
					
					BYTE *soff = (BYTE *)BBuf->GetPixels() +   BBuf->Pitch() * y0;
					WORD *doff = (WORD *)SBuf->GetPixels() + ( SBuf->Pitch() * y ) / sizeof(WORD);
					
					// しきい値より上?
					if( a1 > THRE43 && a2 > THRE43 ){
						for( int x=0; x<BBuf->Pitch(); x++ ){
							BYTE d1 = *(soff);
							BYTE d2 = *((soff++)+BBuf->Pitch());
							WORD dd = ( (WORD)( ( ( Pal[d1].r * a2 + Pal[d2].r * a1 ) / RESO43 ) & 0xf8 ) << 8 ) |
									  ( (WORD)( ( ( Pal[d1].g * a2 + Pal[d2].g * a1 ) / RESO43 ) & 0xfc ) << 3 ) |
									  ( (WORD)( ( ( Pal[d1].b * a2 + Pal[d2].b * a1 ) / RESO43 ) & 0xf8 ) >> 3 );
							*(doff++) = dd;
						}
					}else{
						for( int x=0; x<BBuf->Pitch(); x++ ){
							BYTE d1 = *(soff++);
							WORD dd = ( (WORD)( Pal[d1].r & 0xf8 ) << 8 ) |
									  ( (WORD)( Pal[d1].g & 0xfc ) << 3 ) |
									  ( (WORD)( Pal[d1].b & 0xf8 ) >> 3 );
							*(doff++) = dd;
						}
					}
				}
				break;
				
			case 24:	// 24bitモード
				PRINTD( GRP_LOG, " -> 24bit" );
				for( int y=0; y<SBuf->Height(); y++ ){
					int y0 = ( y * BBuf->Height() ) / SBuf->Height();
					int a1 = ( y * BBuf->Height() * RESO43 ) / SBuf->Height() - y0 * RESO43;
					int a2 = RESO43 - a1;
					
					BYTE *soff = (BYTE *)BBuf->GetPixels() + BBuf->Pitch() * y0;
					BYTE *doff = (BYTE *)SBuf->GetPixels() + SBuf->Pitch() * y;
					
					// しきい値より上?
					if( a1 > THRE43 && a2 > THRE43 ){
						for( int x=0; x<BBuf->Pitch(); x++ ){
							BYTE d1 = *(soff);
							BYTE d2 = *((soff++)+BBuf->Pitch());
							
							BYTE r = ( Pal[d1].r * a2 + Pal[d2].r * a1 ) / RESO43;
							BYTE g = ( Pal[d1].g * a2 + Pal[d2].g * a1 ) / RESO43;
							BYTE b = ( Pal[d1].b * a2 + Pal[d2].b * a1 ) / RESO43;
							
							PUT3BYTE( b, g, r, doff );
						}
					}else{
						for( int x=0; x<BBuf->Pitch(); x++ ){
							BYTE d1 = *(soff++);
							
							BYTE r = Pal[d1].r;
							BYTE g = Pal[d1].g;
							BYTE b = Pal[d1].b;
							
							PUT3BYTE( b, g, r, doff );
						}
					}
				}
			}
		}
	}else{			// 4:3表示無効の場合
		PRINTD( GRP_LOG, " -> No 4:3(8bit)" );
		if( DISPSCAN ){	// スキャンラインありの場合
			PRINTD( GRP_LOG, " -> ScanLine" );
			switch( SBuf->Bpp() ){
			case 8:		// 8bitモード
				PRINTD( GRP_LOG, " -> 8bit" );
				for( int y=0; y<BBuf->Height(); y++ ){
					DWORD *soff = (DWORD *)BBuf->GetPixels() + BBuf->Pitch()/sizeof(DWORD) * y;
					DWORD *doff = (DWORD *)SBuf->GetPixels() + SBuf->Pitch()/sizeof(DWORD) * y*2;
					DWORD *eoff = doff + SBuf->Pitch()/sizeof(DWORD);
					DWORD s, d;
					
					for( int x=0; x<(int)(BBuf->Width()/sizeof(DWORD)); x++ ){
						BYTE b1,b2,b3,b4;
						s = *soff++;
						DWTOB( s, b1, b2, b3, b4 );
						d = BTODW( b4, b4, b3, b3 );
						*doff++ = d;
						*eoff++ = d|0x80808080;
						d = BTODW( b2, b2, b1, b1 );
						*doff++ = d;
						*eoff++ = d|0x80808080;
					}
				}
				break;
				
			case 16:	// 16bitモード
				PRINTD( GRP_LOG, " -> 16bit" );
				for( int y=0; y<BBuf->Height(); y++ ){
					BYTE *soff = (BYTE *)BBuf->GetPixels() + BBuf->Pitch() * y;
					WORD *doff = (WORD *)SBuf->GetPixels() + SBuf->Pitch()/sizeof(WORD) * y*2;
					WORD *eoff = doff + SBuf->Pitch()/sizeof(WORD);
					
					for( int x=0; x<BBuf->Width(); x++ ){
						BYTE r = Pal[*(soff)].r;
						BYTE g = Pal[*(soff)].g;
						BYTE b = Pal[*(soff)].b;
						
						WORD dd = ( (WORD)( r & 0xf8 ) << 8 ) |
								  ( (WORD)( g & 0xfc ) << 3 ) |
								  ( (WORD)( b & 0xf8 ) >> 3 );
						*(doff++) = dd;
						*(doff++) = dd;
						
						r = ( r * SLBr ) / 100;
						g = ( g * SLBr ) / 100;
						b = ( b * SLBr ) / 100;
						
						WORD ed = ( (WORD)( r & 0xf8 ) << 8 ) |
								  ( (WORD)( g & 0xfc ) << 3 ) |
								  ( (WORD)( b & 0xf8 ) >> 3 );
						*(eoff++) = ed;
						*(eoff++) = ed;
						
						soff++;
					}
				}
				break;
				
			case 24:	// 24bitモード
				PRINTD( GRP_LOG, " -> 24bit" );
				for( int y=0; y<BBuf->Height(); y++ ){
					BYTE *soff = (BYTE *)BBuf->GetPixels() + BBuf->Pitch() * y;
					BYTE *doff = (BYTE *)SBuf->GetPixels() + SBuf->Pitch() * y*2;
					BYTE *eoff = doff + SBuf->Pitch();
					
					for( int x=0; x<BBuf->Width(); x++ ){
						BYTE r = Pal[*(soff)].r;
						BYTE g = Pal[*(soff)].g;
						BYTE b = Pal[*(soff)].b;
						
						PUT3BYTE( b, g, r, doff );
						PUT3BYTE( b, g, r, doff );
						
						r = ( r * SLBr ) / 100;
						g = ( g * SLBr ) / 100;
						b = ( b * SLBr ) / 100;
						
						PUT3BYTE( b, g, r, eoff );
						PUT3BYTE( b, g, r, eoff );
						
						soff++;
					}
				}
			}
		}else{			// スキャンラインなしの場合
			PRINTD( GRP_LOG, " -> No ScanLine" );
			// そのままコピー
			BBuf->Blit( NULL, SBuf, NULL );
		}
	}
	
	PRINTD( GRP_LOG, " -> OK\n" );
	
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// サブバッファリフレッシュ
//
// 引数:	なし
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL DSP6::RefreshSubBuf( void )
{
	PRINTD( GRP_LOG, "[GRP][RefreshSubBuf]\n" );
	
	if( SBuf ){
		if( SBuf->Width() == ScreenX() && SBuf->Height() == ScreenY() ){
			// サイズが変わっていなければOK
			PRINTD2( GRP_LOG, " X:%d Y:%d -> OK\n", SBuf->Width(), SBuf->Height() );
			return TRUE;
		}else{
			// サブバッファ一旦削除
			delete SBuf;
			SBuf = NULL;
		}
	}
	
	// サブバッファ作成
	PRINTD3( GRP_LOG, " Create SubBuffer X:%d Y:%d BPP:%d\n", ScreenX(), ScreenY(), Bpp );
	SBuf = new VSurface;
	if( !SBuf ) return FALSE;
	
	if( !SBuf->InitSurface( ScreenX(), ScreenY(), Bpp ) ){
		delete SBuf;
		SBuf = NULL;
		return FALSE;
	}
	
	// パレット設定
	if( Bpp == 8 ) SBuf->SetPalette( Pal );
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// スナップショット
//
// 引数:	path	スクリーンショット格納パス
// 返値:	なし
////////////////////////////////////////////////////////////////
void DSP6::SnapShot( char *path )
{
	PRINTD( GRP_LOG, "[GRP][SnapShot]\n" );
	
	char img_file[PATH_MAX];
	char img_name[PATH_MAX] = "P6V";
	int Index = 0;
	
	// スクリーンショット格納フォルダがなければエラー
	if( !OSD_FileExist( path ) ) return;
	
	// スクリーンショットファイル名を決める
	do{
		sprintf( img_file, "%s%s%03d.%s", path, img_name, ++Index, IMG_EXT );
	}while( OSD_FileExist( img_file ) || (Index > 999) );
	
	// 連番が有効なら画像ファイル保存
	if( !(Index > 999) ) SaveImg( img_file, SBuf, NULL );
}


// [メモ] http://www.joochan.net/rgb-convert.html
// RGB <-> YIQ

// Y =  0.2989 * R + 0.5866 * G + 0.1145 * B
// I =  0.5959 * R - 0.2750 * G - 0.3210 * B
// Q =  0.2065 * R - 0.4969 * G + 0.2904 * B

// R = Y + 0.9489 * I + 0.6561 * Q
// G = Y - 0.2645 * I - 0.6847 * Q
// B = Y - 1.1270 * I + 1.8050 * Q
