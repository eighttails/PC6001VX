#include "pc6001v.h"
#include "common.h"
#include "config.h"
#include "debug.h"
#include "graph.h"
#include "log.h"
#include "osd.h"
#include "status.h"
#include "vdg.h"

#include "p6el.h"

// 通常ウィンドウサイズ
#define	P6WINW		(vm->vdg->Width()*2/vm->vdg->XScale())
#define	P6WINH		(vm->vdg->Height()*2)

#define	HBBUS		((282-12)*2)	/* 4:3表示時画面高さ */

// フルスクリーンウィンドウサイズ
#define	P6WIFW		640
#define	P6WIFH		480

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// デバッグモードウィンドウサイズ
#define	P6DEBUGW	(P6WINW/2+vm->el->regw->Width())
#define	P6DEBUGH	(max(P6WINH/2,vm->el->regw->Height()+vm->el->memw->Height())+vm->el->monw->Height())
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// モニタモード時はスキャンライン，4:3表示禁止
// フルスクリーンモード時はステータスバー表示禁止
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define	DISPMON		vm->el->IsMonitor()
#define	DISPFULL	(vm->el->cfg->GetFullScreen())
#define	DISPSCAN	(!DISPMON  && vm->el->cfg->GetScanLine())
#define	DISPNTSC	(!DISPMON  && vm->el->cfg->GetDispNTSC())
#else
#define	DISPFULL	vm->el->cfg->GetFullScreen()
#define	DISPSCAN	vm->el->cfg->GetScanLine()
#define	DISPNTSC	vm->el->cfg->GetDispNTSC()
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define	DISPSTAT	(!DISPFULL && vm->el->cfg->GetDispStat())




////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
//DSP6::DSP6( VM6 *pvm ) : vm(pvm), Wh(NULL), SBuf(NULL) {}
DSP6::DSP6( VM6 *pvm ) : vm(pvm), Wh(NULL) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSP6::~DSP6( void )
{
//	if( SBuf ) delete SBuf;
	if( Wh ) OSD_DestroyWindow( Wh );
}


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool DSP6::Init( void )
{
	PRINTD( GRP_LOG, "[GRP][Init]\n" );
	
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
void DSP6::SetIcon( const int model )
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
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( DISPMON ){	// モニタモード?
		x      = P6DEBUGW;
		y      = P6DEBUGH;
		fsflag = false;
		
		PRINTD( GRP_LOG, " -> Monitor Mode ( X:%d Y:%d )\n", x, y );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
			x      = P6WINW;
			y      = (DISPNTSC ? HBBUS : P6WINH) + ( DISPSTAT ? vm->el->staw->Height() : 0 );
	}

    if( DISPFULL ){	// フルスクリーン?
        fsflag = true;
        PRINTD( GRP_LOG, " -> FullScreen ( X:%d Y:%d %dbpp)\n", x, y, Bpp );
    }else{
        fsflag = false;
        PRINTD( GRP_LOG, " -> Window ( X:%d Y:%d %dbpp )\n", x, y, Bpp );
    }

	// スクリーンサーフェス作成
	OSD_CreateWindow( &Wh, x, y, fsflag );
	if( !Wh ){
		PRINTD( GRP_LOG, " -> Failed\n" );
		return false;
	}else
		PRINTD( GRP_LOG, " -> OK ( %d x %d )\n", OSD_GetWindowWidth( Wh ), OSD_GetWindowHeight( Wh ) );
	
	if( DISPFULL ){	// フルスクリーンの時
		// マウスカーソルを消す
		OSD_ShowCursor( false );
	}else{			// フルスクリーンでない時
		// マウスカーソルを表示する
		OSD_ShowCursor( true );
		// ウィンドウキャプション設定
		OSD_SetWindowCaption( Wh, vm->el->cfg->GetCaption() );
	}
	
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
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( DISPMON ){	// モニタモード?
		x = P6DEBUGW;
		y = P6DEBUGH;
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
		x      = P6WINW;
		y      = (DISPNTSC ? HBBUS : P6WINH) + ( DISPSTAT ? vm->el->staw->Height() : 0 );
		
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
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( DISPMON ){	// モニタモード?
		PRINTD( GRP_LOG, " -> Monitor" );
		
		// バックバッファ
		OSD_BlitToWindow( Wh, BBuf, 0, 0 );
		
		// モニタウィンドウ
		OSD_BlitToWindow( Wh, vm->el->monw, 0, max( P6WINH/2, vm->el->regw->Height() + vm->el->memw->Height() ) );
		
		// レジスタウィンドウ
		OSD_BlitToWindow( Wh, vm->el->regw, P6WINW/2, 0 );
		
		// メモリウィンドウ
		OSD_BlitToWindow( Wh, vm->el->memw, P6WINW/2, vm->el->regw->Height() );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
//		UpdateSubBuf();	// サブバッファ更新
		
        if( 0/*DISPFULL P6VXではフルスクリーンでもサイズを変えない*/ ){	// フルスクリーン表示
			PRINTD( GRP_LOG, " -> FullScreen" );
//			OSD_BlitToWindow( Wh, SBuf, ( OSD_GetWindowWidth( Wh ) - SBuf->Width() ) / 2, ( OSD_GetWindowHeight( Wh ) - SBuf->Height() ) / 2 );
			OSD_BlitToWindowEx( Wh, BBuf, ( OSD_GetWindowWidth( Wh ) - ScreenX() ) / 2, ( OSD_GetWindowHeight( Wh ) - ScreenY() ) / 2, ScreenY(), DISPNTSC, DISPSCAN, vm->el->cfg->GetScanLineBr() );
		}else{			// ウィンドウ表示
			PRINTD( GRP_LOG, " -> Window" );
//			OSD_BlitToWindow( Wh, SBuf, 0, 0 );
			OSD_BlitToWindowEx( Wh, BBuf, 0, 0, ScreenY(), DISPNTSC, DISPSCAN, vm->el->cfg->GetScanLineBr() );
		}
		
		// ステータスバー
		if( DISPSTAT ){
			PRINTD( GRP_LOG, " -> Statusbar" );
			
			// ステータスバー更新
			// スクリーンサーフェス下端に位置を合わせてblit
			vm->el->staw->Update();
			OSD_BlitToWindow( Wh, vm->el->staw, 0, OSD_GetWindowHeight( Wh ) - vm->el->staw->Height() );
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
int DSP6::ScreenX( void ) const
{
	return P6WINW;
}


////////////////////////////////////////////////////////////////
// 有効スクリーン高さ取得
//
// 引数:	なし
// 返値:	int		有効スクリーン高さピクセル数
////////////////////////////////////////////////////////////////
int DSP6::ScreenY( void ) const
{
    // P6VXでは4:3モードでもスクリーンサイズは変化しない
    return P6WINH;
}


////////////////////////////////////////////////////////////////
// サブバッファ取得
//
// 引数:	なし
// 返値:	VSurface *	サブバッファへのポインタ
////////////////////////////////////////////////////////////////
VSurface *DSP6::GetSubBuffer( void )
{
	return NULL;
//	return SBuf;
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
#define	RESO43		256		/* 中間色計算用分解能(1ラインをRESO43分割する) */
/*
bool DSP6::UpdateSubBuf( void )
{
	PRINTD( GRP_LOG, "[GRP][UpdateSubBuf]\n" );
	
	VSurface *BBuf = vm->vdg;
	
	if( !BBuf ) return false;
	if( !RefreshSubBuf() ) return false;
	
	const bool dntsc = DISPNTSC;
	const bool dscan = DISPSCAN;
	const int brscan = vm->el->cfg->GetScanLineBr();
	const int xsc    = BBuf->XScale();
	const int yy     = SBuf->Height();
	const int xx     = BBuf->Width();
	
	for( int y=0; y<yy; y++ ){
		DWORD y0 = ( y * BBuf->Height() ) / yy;
		DWORD a1 = ( y * BBuf->Height() * RESO43 ) / yy - y0 * RESO43;
		DWORD a2 = RESO43 - a1;
		
		DWORD *sof1 = (DWORD *)BBuf->GetPixels() + BBuf->Pitch()/sizeof(DWORD) * y0;
		DWORD *sof2 = sof1 + ( y0 < (DWORD)BBuf->Height()-1 ? BBuf->Pitch()/sizeof(DWORD) : 0 );
		DWORD *doff = (DWORD *)SBuf->GetPixels() + SBuf->Pitch()/sizeof(DWORD) * y ;
		
		for( int x=0; x<xx; x++ ){
			DWORD r,g,b;
			DWORD d1 = *sof1++;
			DWORD d2 = *sof2++;
			
			if( dntsc ){
				r = ( ( ( (d1>>RSHIFT32)&0xff ) * a2 + ( (d2>>RSHIFT32)&0xff ) * a1 ) / RESO43 ) & 0xff;
				g = ( ( ( (d1>>GSHIFT32)&0xff ) * a2 + ( (d2>>GSHIFT32)&0xff ) * a1 ) / RESO43 ) & 0xff;
				b = ( ( ( (d1>>BSHIFT32)&0xff ) * a2 + ( (d2>>BSHIFT32)&0xff ) * a1 ) / RESO43 ) & 0xff;
			}else{
				r = (d1>>RSHIFT32)&0xff;
				g = (d1>>GSHIFT32)&0xff;
				b = (d1>>BSHIFT32)&0xff;
			}
			
			if( dscan && y&1 ){
				r = ( ( r * brscan ) / 100 ) & 0xff;
				g = ( ( g * brscan ) / 100 ) & 0xff;
				b = ( ( b * brscan ) / 100 ) & 0xff;
			}
			*doff++ = (r<<RSHIFT32) | (g<<GSHIFT32) | (b<<BSHIFT32);
			if( xsc == 1 ) *doff++ = (r<<RSHIFT32) | (g<<GSHIFT32) | (b<<BSHIFT32);
		}
	}
	
	PRINTD( GRP_LOG, " -> %sScanLine -> OK\n", DISPSCAN ? "" : "No " );
	
    // 4:3表示有効の場合、表示比率を設定する
    if(DISPNTSC){
        SBuf->SetAspectRatio(double(HBBUS) / (P6WINH));
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
	PRINTD( GRP_LOG, " Create SubBuffer X:%d Y:%d\n", ScreenX(), ScreenY() );
	SBuf = new VSurface;
	if( !SBuf ) return false;
	
	if( !SBuf->InitSurface( ScreenX(), ScreenY() ) ){
		delete SBuf;
		SBuf = NULL;
		return false;
	}
	
	return true;
}
*/


////////////////////////////////////////////////////////////////
// スナップショット
//
// 引数:	path	スクリーンショット格納パス
// 返値:	なし
////////////////////////////////////////////////////////////////
void DSP6::SnapShot( const char *path )
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
	if( !(Index > 999) ){
		VRect scr;
		
		scr.x = scr.y = 0;
		scr.w = DISPFULL ? P6WIFW : P6WINW;
		scr.h = DISPFULL ? P6WIFH : DISPNTSC ? HBBUS : P6WINH;
		
		BYTE *pixels = new BYTE[scr.w * scr.h * sizeof(DWORD)];
		if( !pixels ) return;
		
		if( !OSD_GetWindowImage( Wh, (void **)(&pixels), &scr ) ) return;
		SaveImgData( img_file, pixels, 32, scr.w, scr.h, NULL );
		
		if( pixels ) delete [] pixels;
	}
}
