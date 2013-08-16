#include "pc6001v.h"
#include "graph.h"
#include "log.h"
#include "common.h"
#include "debug.h"
#include "osd.h"
#include "status.h"
#include "vdg.h"

#include "p6el.h"

// 通常ウィンドウサイズ
#define	P6WINW		(vm->vdg->Width())
#define	P6WINH		(vm->vdg->Height())

#define	HBBUS		(282-12)	/* 4:3表示時画面高さ */

// フルスクリーンウィンドウサイズ
#define	P6WIFW		(640)
#define	P6WIFH		(480)

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
// デバッグモードウィンドウサイズ
#define	P6DEBUGW	(P6WINW+vm->el->regw->Width())
#define	P6DEBUGH	(max(P6WINH,vm->el->regw->Height()+vm->el->memw->Height())+vm->el->monw->Height())
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //


// モニタモード時はスキャンライン，4:3表示禁止
// フルスクリーンモード時はステータスバー表示禁止
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
#define	DISPMON		(vm->el->IsMonitor())
#define	DISPFULL	(vm->el->cfg->GetFullScreen())
#define	DISPSCAN	(!DISPMON  && vm->el->cfg->GetScanLine())
#define	DISPNTSC	(!DISPMON  && vm->el->cfg->GetDispNTSC())
#define	DISPSTAT	(!DISPFULL && vm->el->cfg->GetDispStat())

#else

#define	DISPFULL	(vm->el->cfg->GetFullScreen())
#define	DISPSCAN	(vm->el->cfg->GetScanLine())
#define	DISPNTSC	(vm->el->cfg->GetDispNTSC())
#define	DISPSTAT	(vm->el->cfg->GetDispStat())

#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //





////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
DSP6::DSP6( VM6 *pvm ) : vm(pvm), Wh(NULL), SBuf(NULL), Pal(NULL),
							Bpp(DEFAULT_COLOR_MODE), SLBr(DEFAULT_SCANLINEBR) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSP6::~DSP6( void )
{
	if( SBuf ) delete SBuf;
	if( Wh ) OSD_DestroyWindow( Wh );
}


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	bpp		1ピクセルのbit数
//			br		スキャンライン輝度
//			pal		パレットへのポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool DSP6::Init( int bpp, int br, VPalette *pal )
{
	PRINTD( GRP_LOG, "[GRP][Init]\n" );
	
	Bpp  = bpp;
	SLBr = br;
	Pal  = pal;
	
	// スクリーンサーフェス作成
	if( !SetScreenSurface() ) return false;
	
	return true;
}


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	model	機種 60,62,66
// 返値:	なし
////////////////////////////////////////////////////////////////
void DSP6::SetIcon( int model )
{
	OSD_SetIcon( Wh, model );
}


////////////////////////////////////////////////////////////////
// スクリーンサーフェス作成
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool DSP6::SetScreenSurface( void )
{
	PRINTD( GRP_LOG, "[GRP][SetScreenSurface]\n" );
	
	int x = 0, y = 0;
	bool fsflag = false;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( DISPMON ){	// モニタモード?
		x      = P6DEBUGW;
		y      = P6DEBUGH;
		
		PRINTD( GRP_LOG, " -> Monitor Mode ( X:%d Y:%d )\n", x, y );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	{
        x      = P6WINW * ( DISPSCAN ? 2 : 1 );
        y      = ( DISPNTSC ? HBBUS : P6WINH ) * ( DISPSCAN ? 2 : 1 ) + ( DISPSTAT ? vm->el->staw->Height() : 0 );
	}

    if( DISPFULL ){	// フルスクリーン?
        fsflag = true;
        PRINTD( GRP_LOG, " -> FullScreen ( X:%d Y:%d %dbpp)\n", x, y, Bpp );
    }else{
        fsflag = false;
        PRINTD( GRP_LOG, " -> Window ( X:%d Y:%d %dbpp )\n", x, y, Bpp );
    }

	// スクリーンサーフェス作成
	OSD_CreateWindow( &Wh, x, y, Bpp, fsflag );
	if( !Wh ){
		PRINTD( GRP_LOG, " -> Failed\n" );
		return false;
	}else
		PRINTD( GRP_LOG, " -> OK ( %d x %d x %d )\n", OSD_GetWindowWidth( Wh ), OSD_GetWindowHeight( Wh ), OSD_GetWindowBPP( Wh ) );
	
	if( DISPFULL ){	// フルスクリーンの時
		// マウスカーソルを消す
		OSD_ShowCursor( false );
	}else{			// フルスクリーンでない時
		// マウスカーソルを表示する
		OSD_ShowCursor( true );
		// ウィンドウキャプション設定
		OSD_SetWindowCaption( Wh, vm->el->cfg->GetCaption() );
	}
	
	// スクリーンサーフェスにパレットを選択する
	if( OSD_GetWindowBPP( Wh ) == 8 )
		if( !OSD_SetPalette( Wh, Pal ) ) return false;
	
	return true;
}


////////////////////////////////////////////////////////////////
// スクリーンサイズ変更
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool DSP6::ResizeScreen( void )
{
	PRINTD( GRP_LOG, "[GRP][ResizeScreen]\n" );
	
	int x, y;
	
	// ウィンドウサイズチェック
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( DISPMON ){	// モニタモード?
		x = P6DEBUGW;
		y = P6DEBUGH;
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	{
        x = ( P6WINW * ( DISPSCAN ? 2 : 1 ) );
        y = ( ( DISPNTSC ? HBBUS : P6WINH ) * ( DISPSCAN ? 2 : 1 ) );
		
		// ステータスバー表示?
		if( !DISPFULL && DISPSTAT ) y += vm->el->staw->Height();
	}
	
	// ウィンドウサイズが不適切なら作り直す
	if( !Wh || (x != OSD_GetWindowWidth( Wh )) || (y != OSD_GetWindowHeight( Wh )) ){
		if( !SetScreenSurface() ) return false;
		vm->el->staw->Init( OSD_GetWindowWidth( Wh ) );	// ステータスバーも
	}else
		// 作り直さない場合は現在のスクリーンサーフェスをクリア
		OSD_ClearWindow( Wh );
	
	return true;
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
	
	VSurface *BBuf = vm->vdg;		// バックバッファへのポインタ取得
	
	if( !Wh || !BBuf ) return;
	
	// スクリーンサーフェスにblit
	PRINTD( GRP_LOG, " -> Blit" );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( DISPMON ){	// モニタモード?
		PRINTD( GRP_LOG, " -> Monitor" );
		
		// バックバッファ
		OSD_BlitToWindow( Wh, BBuf, 0, 0, Pal );
		
		// モニタウィンドウ
		OSD_BlitToWindow( Wh, vm->el->monw, 0, max( P6WINH, vm->el->regw->Height() + vm->el->memw->Height() ), Pal );
		
		// レジスタウィンドウ
		OSD_BlitToWindow( Wh, vm->el->regw, P6WINW, 0, Pal );
		
		// メモリウィンドウ
		OSD_BlitToWindow( Wh, vm->el->memw, P6WINW, vm->el->regw->Height(), Pal );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	{
		UpdateSubBuf();	// サブバッファ更新
		
		if( DISPFULL ){	// フルスクリーン
			PRINTD( GRP_LOG, " -> FullScreen" );
            PRINTD( GRP_LOG, "(Scan line)" );
            OSD_BlitToWindow( Wh, SBuf, ( OSD_GetWindowWidth( Wh ) - SBuf->Width() ) / 2, ( OSD_GetWindowHeight( Wh ) - SBuf->Height() * SBuf->GetAspectRatio() ) / 2, Pal );
        }else{			// フルスクリーンでない
			PRINTD( GRP_LOG, " -> Window" );
            OSD_BlitToWindow( Wh, SBuf, 0, 0, Pal );
		}
		
		// ステータスバー
		if( DISPSTAT ){
			PRINTD( GRP_LOG, " -> Statusbar" );
			
			// ステータスバー更新
			// スクリーンサーフェス下端に位置を合わせてblit
			vm->el->staw->Update();
			
			OSD_BlitToWindow( Wh, STATIC_CAST( VSurface *, vm->el->staw ), 0, OSD_GetWindowHeight( Wh ) - vm->el->staw->Height(), Pal );
		}
	}
	
	PRINTD( GRP_LOG, " -> OK\n" );
	
	// フリップ
	OSD_RenderWindow( Wh );
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
    // P6VXでは4:3モードでもスクリーンサイズは変化しない
    return ( /*DISPNTSC ? HBBUS :*/ P6WINH ) * ( DISPSCAN ? 2 : 1 );
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
// ウィンドウハンドル取得
//
// 引数:	なし
// 返値:	HWINDOW *	ウィンドウハンドル
////////////////////////////////////////////////////////////////
HWINDOW DSP6::GetWindowHandle( void )
{
	return (HWINDOW)Wh;
}


////////////////////////////////////////////////////////////////
// サブバッファ更新
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
#define	RESO43		(8)		/* 中間色計算用分解能(1ラインをRESO43分割する) */
bool DSP6::UpdateSubBuf( void )
{
	PRINTD( GRP_LOG, "[GRP][UpdateSubBuf]\n" );
	
	VSurface *BBuf = vm->vdg;
	bool dscan = DISPSCAN;
	bool dntsc = DISPNTSC;
	
	if( !BBuf ) return false;
	if( !RefreshSubBuf() ) return false;
	
	if( !dscan && !dntsc ){	// スキャンラインなし,4:3表示有効の場合
		// そのままコピー
		BBuf->Blit( NULL, SBuf, NULL );
	}else{
		switch( SBuf->Bpp() ){
		case 8:		// 8bitモード
			PRINTD( GRP_LOG, " -> 8bit" );
			for( int y=0; y<SBuf->Height(); y += dscan ? 2 : 1 ){
				int y0 = ( y * BBuf->Height() ) / SBuf->Height();
				
				DWORD *soff = (DWORD *)BBuf->GetPixels() + BBuf->Pitch()/sizeof(DWORD) * y0;
				DWORD *doff = (DWORD *)SBuf->GetPixels() + SBuf->Pitch()/sizeof(DWORD) * y;
				DWORD *eoff = doff + SBuf->Pitch()/sizeof(DWORD);
				
				// 8bitの時は中間色処理なし
				for( int x=0; x<BBuf->Width()/(int)sizeof(DWORD); x++ ){
					if( dscan ){
						BYTE b1,b2,b3,b4;
						
						DWORD s = *soff++;
						DWTOB( s, b1, b2, b3, b4 );
						DWORD d = BTODW( b4, b4, b3, b3 );
						*doff++ = d;
						*eoff++ = d|0x80808080;
						d = BTODW( b2, b2, b1, b1 );
						*doff++ = d;
						*eoff++ = d|0x80808080;
					}else{
						*doff++ = *soff++;
					}
				}
			}
			break;
			
		case 16:	// 16bitモード
			PRINTD( GRP_LOG, " -> 16bit" );
			for( int y=0; y<SBuf->Height(); y += dscan ? 2 : 1 ){
				int y0 = ( y * BBuf->Height() ) / SBuf->Height();
				int a1 = ( y * BBuf->Height() * RESO43 ) / SBuf->Height() - y0 * RESO43;
				int a2 = RESO43 - a1;
				
				BYTE *soff = (BYTE *)BBuf->GetPixels() + BBuf->Pitch() * y0;
				DWORD *doff = (DWORD *)SBuf->GetPixels() + ( SBuf->Pitch() * y ) / sizeof(DWORD);
				DWORD *eoff = doff + SBuf->Pitch() / sizeof(DWORD);
				
				for( int x=0; x<(int)BBuf->Width(); x += dscan ? 1 : 2 ){
					BYTE d1 = *(soff);
					BYTE d2 = *((soff++)+BBuf->Pitch());
					
					BYTE r = ( Pal->colors[d1].r * a2 + Pal->colors[d2].r * a1 ) / RESO43;
					BYTE g = ( Pal->colors[d1].g * a2 + Pal->colors[d2].g * a1 ) / RESO43;
					BYTE b = ( Pal->colors[d1].b * a2 + Pal->colors[d2].b * a1 ) / RESO43;
					
					DWORD dd = ( (DWORD)( r & 0xf8 ) << 8 ) | ( (DWORD)( g & 0xfc ) << 3 ) | ( (DWORD)( b & 0xf8 ) >> 3 );
					
					if( dscan ){
						dd |= dd << 16;
						*(doff++) = dd;
						
						r = ( r * SLBr ) / 100;
						g = ( g * SLBr ) / 100;
						b = ( b * SLBr ) / 100;
						
						DWORD ed = ( (DWORD)( r & 0xf8 ) << 8 ) | ( (DWORD)( g & 0xfc ) << 3 ) | ( (DWORD)( b & 0xf8 ) >> 3 );
						
						ed |= ed << 16;
						*(eoff++) = ed;
					}else{
						d1 = *(soff);
						d2 = *((soff++)+BBuf->Pitch());
						r = ( Pal->colors[d1].r * a2 + Pal->colors[d2].r * a1 ) / RESO43;
						g = ( Pal->colors[d1].g * a2 + Pal->colors[d2].g * a1 ) / RESO43;
						b = ( Pal->colors[d1].b * a2 + Pal->colors[d2].b * a1 ) / RESO43;
						
						dd |= ( (DWORD)( r & 0xf8 ) << 24 ) | ( (DWORD)( g & 0xfc ) << 19 ) | ( (DWORD)( b & 0xf8 ) << 13 );
						
						*(doff++) = dd;
					}
				}
			}
			break;
			
		case 24:	// 24bitモード
			PRINTD( GRP_LOG, " -> 24bit" );
			for( int y=0; y<SBuf->Height(); y += dscan ? 2 : 1 ){
				int y0 = ( y * BBuf->Height() ) / SBuf->Height();
				int a1 = ( y * BBuf->Height() * RESO43 ) / SBuf->Height() - y0 * RESO43;
				int a2 = RESO43 - a1;
				
				BYTE *soff = (BYTE *)BBuf->GetPixels() + BBuf->Pitch() * y0;
				BYTE *doff = (BYTE *)SBuf->GetPixels() + SBuf->Pitch() * y;
				BYTE *eoff = doff + SBuf->Pitch();
				
				for( int x=0; x<(int)BBuf->Width(); x++ ){
					BYTE d1 = *(soff);
					BYTE d2 = *((soff++)+BBuf->Pitch());
					
					BYTE r = ( Pal->colors[d1].r * a2 + Pal->colors[d2].r * a1 ) / RESO43;
					BYTE g = ( Pal->colors[d1].g * a2 + Pal->colors[d2].g * a1 ) / RESO43;
					BYTE b = ( Pal->colors[d1].b * a2 + Pal->colors[d2].b * a1 ) / RESO43;
					
					PUT3BYTE( b, g, r, doff );
					if( dscan ){
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
	}
	
	PRINTD( GRP_LOG, " -> %sScanLine -> OK\n", dscan ? "" : "No " );
	
    // 4:3表示有効の場合、表示比率を設定する
    if(DISPNTSC){
        SBuf->SetAspectRatio(double(HBBUS) / P6WINH);
    } else {
        SBuf->SetAspectRatio(1.0);
    }

	return true;
}


////////////////////////////////////////////////////////////////
// サブバッファリフレッシュ
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool DSP6::RefreshSubBuf( void )
{
	PRINTD( GRP_LOG, "[GRP][RefreshSubBuf]\n" );
	
	if( SBuf ){
		if( SBuf->Width() == ScreenX() && SBuf->Height() == ScreenY() ){
			// サイズが変わっていなければOK
			PRINTD( GRP_LOG, " X:%d Y:%d -> OK\n", SBuf->Width(), SBuf->Height() );
			return true;
		}else{
			// サブバッファ一旦削除
			delete SBuf;
			SBuf = NULL;
		}
	}
	
	// サブバッファ作成
	PRINTD( GRP_LOG, " Create SubBuffer X:%d Y:%d BPP:%d\n", ScreenX(), ScreenY(), Bpp );
	SBuf = new VSurface;
	if( !SBuf ) return false;
	
	if( !SBuf->InitSurface( ScreenX(), ScreenY(), Bpp ) ){
		delete SBuf;
		SBuf = NULL;
		return false;
	}
	
	// パレット設定
	if( Bpp == 8 ) SBuf->SetPalette( Pal->colors );
	
	return true;
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
	
	// スナップショット格納フォルダがなければエラー
	if( !OSD_FileExist( path ) ) return;
	
	// スナップショットファイル名を決める
	do{
		sprintf( img_file, "%s/%s%03d.%s", path, img_name, ++Index, IMG_EXT );
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
