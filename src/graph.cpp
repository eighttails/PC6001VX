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
// Mac のフルスクリーンは 640x480, 800x600, 1024x768 に限られるらしい。by Windyさん
#define	P6WIFW		640
#define	P6WIFH		480

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// モニタモードウィンドウサイズ
#define	P6DEBUGW	(max(P6WINW/2,vm->el->monw->Width())+max(vm->el->regw->Width(),vm->el->memw->Width()))
#define	P6DEBUGH	(max(P6WINH/2+vm->el->monw->Height(),vm->el->regw->Height()+vm->el->memw->Height()))
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
DSP6::DSP6( VM6 *pvm ) : vm(pvm), Wh(NULL) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSP6::~DSP6( void )
{
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
		x      = ScreenX();
		y      = ScreenY();

		if( DISPFULL ){	// フルスクリーン?
			fsflag = true;
			PRINTD( GRP_LOG, " -> FullScreen ( X:%d Y:%d )\n", x, y );
		}else{
			fsflag = false;
			y     += DISPSTAT ? vm->el->staw->Height() : 0;
			PRINTD( GRP_LOG, " -> Window ( X:%d Y:%d )\n", x, y );
		}
	}

	// スクリーンサーフェス作成
	OSD_CreateWindow( &Wh, x, y, fsflag );
	if( !Wh ){
		PRINTD( GRP_LOG, " -> Failed\n" );
		return false;
	}else{
		PRINTD( GRP_LOG, " -> OK ( %d x %d )\n", OSD_GetWindowWidth( Wh ), OSD_GetWindowHeight( Wh ) );
	}
	
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
		x = ScreenX();
		y = ScreenY();
		
		// ステータスバー表示?
		if( !DISPFULL && DISPSTAT ) y += vm->el->staw->Height();
	}
	
	// ウィンドウサイズが不適切なら作り直す
	//if( !Wh || (x != OSD_GetWindowWidth( Wh )) || (y != OSD_GetWindowHeight( Wh )) ){
	if(1){ //VXでは常に作りなおすことにする。(サーフェスを使いまわすとフルスクリーンに遷移できないため)
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
		OSD_BlitToWindow( Wh, vm->el->monw, 0, P6WINH/2 );
		
		// レジスタウィンドウ
		OSD_BlitToWindow( Wh, vm->el->regw, max( P6WINW/2, vm->el->monw->Width() ), 0 );
		
		// メモリウィンドウ
		OSD_BlitToWindow( Wh, vm->el->memw, max( P6WINW/2, vm->el->monw->Width() ), vm->el->regw->Height() );
	}else
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
		if( 0/*DISPFULL P6VXではフルスクリーンでもサイズを変えない*/ ){	// フルスクリーン表示
			PRINTD( GRP_LOG, " -> FullScreen" );
			OSD_BlitToWindowEx( Wh, BBuf, ( P6WIFW - P6WINW ) / 2, ( P6WIFH - (DISPNTSC ? HBBUS : P6WINH) ) / 2, DISPNTSC ? HBBUS : P6WINH, DISPNTSC, DISPSCAN, vm->el->cfg->GetScanLineBr() );
		}else{			// ウィンドウ表示
			PRINTD( GRP_LOG, " -> Window" );
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
	// P6VXではスクリーンサイズは変化しない
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
	// P6VXではフルスクリーンモードでもスクリーンサイズは変化しない
	return DISPNTSC ? HBBUS : P6WINH;
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
		scr.w = ScreenX();
		scr.h = ScreenY();
		
		BYTE *pixels = new BYTE[scr.w * scr.h * sizeof(DWORD)];
		if( !pixels ) return;
		
		if( !OSD_GetWindowImage( Wh, (void **)(&pixels), &scr ) ) return;
		SaveImgData( img_file, pixels, 32, scr.w, scr.h, NULL );
		
		if( pixels ) delete [] pixels;
	}
}

