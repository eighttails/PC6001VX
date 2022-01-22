/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "pc6001v.h"

#include "common.h"
#include "config.h"
#include "debug.h"
#include "graph.h"
#include "log.h"
#include "osd.h"
#include "p6el.h"
#include "status.h"
#include "vdg.h"


// スクリーン表示倍率(%)
#define	WSCALE		el->cfg->GetValue( CV_WindowZoom )

// スクリーンサイズ(標準)
#define	P6WINW		el->GetVideoInfo().w
#define	P6WINH		el->GetVideoInfo().h

// アスペクトレシオ(幅に対する高さの比)
#define	P6RATIO		el->GetVideoInfo().ratio


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// モニタモード スクリーン表示マージン
#define	P6WINMGN	4
// モニタモードウィンドウサイズ
#define	P6DEBUGW	(max(P6WINW+P6WINMGN*2,el->monw->Width())+max(el->regw->Width(),el->memw->Width()))
#define	P6DEBUGH	(max(P6WINH+P6WINMGN*2+el->monw->Height(),el->regw->Height()+el->memw->Height()))
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// モニタモード時はフルスクリーン，スキャンライン，4:3表示禁止
// フルスクリーンモード時はステータスバー表示禁止
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define	DISPMON		el->vm->IsMonitor()
#define	DISPFULL	(!DISPMON && el->cfg->GetValue( CB_FullScreen ))
#define	DISPSCAN	(!DISPMON && el->cfg->GetValue( CB_ScanLine ))
#define	DISPNTSC	(!DISPMON && el->cfg->GetValue( CB_DispNTSC ))
#else
#define	DISPFULL	el->cfg->GetValue( CB_FullScreen )
#define	DISPSCAN	el->cfg->GetValue( CB_ScanLine )
#define	DISPNTSC	el->cfg->GetValue( CB_DispNTSC )
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define	DISPSTAT	(!DISPFULL && el->cfg->GetValue( CB_DispStatus ))


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
DSP6::DSP6( EL6* el ) : el( el ), Wh( nullptr ), rsize( false )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
DSP6::~DSP6( void )
{
	if( Wh ) OSD_DestroyWindow( Wh );
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool DSP6::Init( void )
{
	PRINTD( GRP_LOG, "[GRP][Init]\n" );
	
	// スクリーンサーフェス作成
	if( !SetScreenSurface() ) return false;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	model	機種 60,62,66
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void DSP6::SetIcon( const int model )
{
	OSD_SetIcon( Wh, model );
}


/////////////////////////////////////////////////////////////////////////////
// スクリーンサーフェス作成
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool DSP6::SetScreenSurface( void )
{
	PRINTD( GRP_LOG, "[GRP][SetScreenSurface]\n" );
	
	int x, y;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( DISPMON ){	// モニタモード?
		x = P6DEBUGW;
		y = P6DEBUGH;
		
		PRINTD( GRP_LOG, " -> Monitor Mode ( X:%d Y:%d )\n", x, y );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
		x = ScreenX();
		y = ScreenY() + (DISPSTAT ? el->staw->Height() : 0);
		
		PRINTD( GRP_LOG, " -> %s ( X:%d Y:%d )\n", DISPFULL ? "FullScreen" : "Window", x, y );
	}
	
	// ウィンドウ作成
	OSD_CreateWindow( &Wh, x, y, P6WINW, P6WINH, DISPFULL, el->cfg->GetValue( CB_Filtering ), el->cfg->GetValue( CV_ScanLineBr ) );
	
	// フルスクリーン，モニタモード時はウィンドウサイズ変更不可
	OSD_SetWindowResizable( Wh, !(DISPFULL | DISPMON) );
	
	PRINTD( GRP_LOG, " -> %s ( %d x %d )\n", Wh ? "OK" : "Failed", Wh ? OSD_GetWindowWidth( Wh ) : 0, Wh ? OSD_GetWindowHeight( Wh ) : 0 );
	
	if( !Wh ) return false;
	
	// マウスカーソル表示
	OSD_ShowCursor( true );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ResizeScreen()でリサイズしたかチェック
//
// 引数:	なし
// 返値:	bool	true:ResizeScreen()使用 false:その他
/////////////////////////////////////////////////////////////////////////////
bool DSP6::CheckResize( void )
{
	bool ret = rsize;
	rsize = false;
	
	return ret;
}


/////////////////////////////////////////////////////////////////////////////
// スクリーンサイズ変更
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool DSP6::ResizeScreen( void )
{
	PRINTD( GRP_LOG, "[GRP][ResizeScreen]\n" );
	
	int x, y;
	
	// ウィンドウサイズチェック
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( DISPMON ){	// モニタモード?
		x = P6DEBUGW;
		y = P6DEBUGH;
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
		x = ScreenX();
		y = ScreenY() + (DISPSTAT ? el->staw->Height() : 0);
	}
	
	// ウィンドウサイズ変更 or フィルタリング変更なら作り直す
	if( !Wh || (x != OSD_GetWindowWidth( Wh )) || (y != OSD_GetWindowHeight( Wh )) || (OSD_IsFullScreen( Wh ) != DISPFULL) || OSD_IsFiltering( Wh ) != el->cfg->GetValue( CB_Filtering ) ){
		if( !SetScreenSurface() ) return false;
		el->staw->Init( OSD_GetWindowWidth( Wh ) );	// ステータスバーも
		
		// ResizeScreen()を使ったリサイズであることを通知
		rsize = true;
	}else
		// 作り直さない場合は現在のスクリーンサーフェスをクリア
		OSD_ClearWindow( Wh );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 画面更新
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void DSP6::DrawScreen( void )
{
	PRINTD( GRP_LOG, "[GRP][DrawScreen]\n" );
	
	VRect pos;
	
	if( !Wh ) return;
	
	// スクリーンサーフェスにblit
	PRINTD( GRP_LOG, " -> Blit" );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( DISPMON ){	// モニタモード?
		PRINTD( GRP_LOG, " -> Monitor" );
		
		pos.x = P6WINMGN;
		pos.y = P6WINMGN;
		pos.w = P6WINW;
		pos.h = P6WINH;
		
		// モニタウィンドウ描画
		OSD_BlitToWindow( Wh, el->monw.get(), 0, el->regw->Height()+el->memw->Height()-el->monw->Height() );
		
		// レジスタウィンドウ描画
		OSD_BlitToWindow( Wh, el->regw.get(), max( P6WINW+P6WINMGN * 2, el->monw->Width() ), 0 );
		
		// メモリウィンドウ描画
		OSD_BlitToWindow( Wh, el->memw.get(), max( P6WINW+P6WINMGN * 2, el->monw->Width() ), el->regw->Height() );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
		pos.x = pos.y = 0;
		pos.w = ScreenX();
		pos.h = ScreenY();
		
		// ステータスバー
		if( DISPSTAT ){
			PRINTD( GRP_LOG, " -> Statusbar" );
			
			// ステータスバー描画
			// スクリーンサーフェス下端に位置を合わせてblit
			OSD_BlitToWindow( Wh, el->staw.get(), 0, OSD_GetWindowHeight( Wh ) - el->staw->Height() );
		}
	}
	
	// バックバッファ描画
	OSD_BlitToWindowEx( Wh, el->GetBackBuffer().get(), &pos, DISPSCAN );
	
	PRINTD( GRP_LOG, " -> OK\n" );
	
	// 描画反映
	OSD_RenderWindow( Wh );
}


/////////////////////////////////////////////////////////////////////////////
// 有効スクリーン幅取得
//
// 引数:	なし
// 返値:	int		有効スクリーン幅ピクセル数
/////////////////////////////////////////////////////////////////////////////
int DSP6::ScreenX( void ) const
{
	return (int)((double)(P6WINW * WSCALE) / (DISPNTSC ? P6RATIO : 100.0) + 0.5);
}


/////////////////////////////////////////////////////////////////////////////
// 有効スクリーン高さ取得
//
// 引数:	なし
// 返値:	int		有効スクリーン高さピクセル数
/////////////////////////////////////////////////////////////////////////////
int DSP6::ScreenY( void ) const
{
	return P6WINH * WSCALE / 100;
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウハンドル取得
//
// 引数:	なし
// 返値:	HWINDOW*	ウィンドウハンドル
/////////////////////////////////////////////////////////////////////////////
HWINDOW DSP6::GetWindowHandle( void )
{
	return (HWINDOW)Wh;
}


/////////////////////////////////////////////////////////////////////////////
// スナップショット
//
// 引数:	path	スクリーンショット格納パスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void DSP6::SnapShot( const P6VPATH& path )
{
	PRINTD( GRP_LOG, "[GRP][SnapShot]\n" );
	
	P6VPATH tpath;
	int Index = 0;
	
	// スナップショット格納フォルダがなければフォルダを作成
	if( !OSD_FileExist( path ) ){
		if( !OSD_CreateFolder( path ) ) return;
	}
	
	// スナップショットファイル名を決める
	do{
		OSD_AddPath( tpath, path, STR2P6VPATH( Stringf( "%s%03d.%s", FILE_SNAP, ++Index, EXT_IMG ) ) );
	}while( OSD_FileExist( tpath ) || (Index > 999) );
	
	// 連番が有効なら画像ファイル保存
	if( !(Index > 999) ){
		VRect scr;
		std::vector<BYTE> pixels;
		
		scr.x = scr.y = 0;
		
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		if( DISPMON ){	// モニタモード?
			scr.w = P6DEBUGW;
			scr.h = P6DEBUGH;
		}else
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		{
			scr.w = ScreenX();
			scr.h = ScreenY();
		}
		
		pixels.resize( ((scr.w * 24 + 31) / 32) * sizeof(DWORD) * scr.h );
		
		if( !OSD_GetWindowImage( Wh, pixels, &scr, PX24RGB ) ){
			return;
		}
		SaveImgData( tpath, pixels.data(), 24, scr.w, scr.h, nullptr );
	}
}

