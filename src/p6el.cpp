#include <new>

#include "pc6001v.h"
#include "id_menu.h"

#include "breakpoint.h"
#include "common.h"
#include "config.h"
#include "cpum.h"
#include "cpus.h"
#include "debug.h"
#include "disk.h"
#include "error.h"
#include "graph.h"
#include "intr.h"
#include "joystick.h"
#include "keyboard.h"
#include "memory.h"
#include "osd.h"
#include "pio.h"
#include "psg.h"
#include "schedule.h"
#include "sound.h"
#include "status.h"
#include "tape.h"
#include "vdg.h"
#include "voice.h"
#include "vsurface.h"

#include "p6el.h"


#define	FRAMERATE	(VSYNC_HZ/(cfg->GetFrameSkip()+1))



////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
EL6::EL6( void ) : vm(NULL), cfg(NULL), sche(NULL), graph(NULL), snd(NULL), joy(NULL), staw(NULL),
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	regw(NULL), memw(NULL), monw(NULL),
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	UpDateFPSID(NULL), UDFPSCount(0), FSkipCount(0)
{}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
EL6::~EL6( void )
{
	DeleteAllObject();
}



////////////////////////////////////////////////////////////////
// スレッド関数(オーバーライド)
////////////////////////////////////////////////////////////////
void EL6::OnThread( void *inst )
{
	EL6 *p6;
	
	p6 = STATIC_CAST( EL6 *, inst );	// 自分自身のオブジェクトポインタ取得
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( p6->cfg->GetMonDisp() ){
	// モニタモード
		while( !this->cThread::IsCancel() ){
			// 画面更新
			if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
			p6->Wait();		// ウェイト
		}
	}else
	// 通常モード
		if( p6->vm->BpExistBreakPoint() ){
			// ブレークポイントあり
			while( !this->cThread::IsCancel() ){
				// ポーズ中なら何もしない
				if( p6->sche->GetPauseEnable() )
					// ウェイト
					p6->Wait();
				else{
					p6->Emu();		// 1命令実行
					
					if( p6->vm->evsc->IsVSYNC() ){
						p6->vm->key->ScanMatrix();	// キーマトリクススキャン
						
						// サウンド更新
						p6->SoundUpdate( 0 );
						// 画面更新
						if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
						
						// 自動キー入力
						if( IsAutoKey() ){
							BYTE key = GetAutoKey();
							if( key ){
								if( key == 0x14 ) p6->vm->cpus->ReqKeyIntr( 6, GetAutoKey() );
								else			  p6->vm->cpus->ReqKeyIntr( 0, key );
							}
						}
						
						// ウェイト
						p6->Wait();
					}
				}
				
				// ブレークポイントチェック
				if( p6->vm->BpCheckBreakPoint( BPoint::BP_PC, p6->vm->cpum->GetPC(), 0, NULL ) || p6->vm->BpIsReqBreak() ){
					this->cThread::Cancel();	// スレッド終了フラグ立てる
					p6->vm->BpResetBreak();
                    Event ev;
                    ev.type = EV_DEBUGMODEBP;
                    ev.bp.addr = p6->vm->cpum->GetPC();
                    OSD_PushEvent( ev );
				}
			}
		}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{	// 通常実行
		while( !this->cThread::IsCancel() ){
			// ポーズ中なら画面更新のみ
			if( p6->sche->GetPauseEnable() ){
				// 画面更新時期を迎えていたら画面更新
				// ノーウェイトの時にFPSが変わらないようにする
				if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
			}else{
				// キーマトリクススキャン
				bool matchg = p6->vm->key->ScanMatrix();
				
				// リプレイ記録中
				if( REPLAY::GetStatus() == REP_RECORD )
					REPLAY::ReplayWriteFrame( p6->vm->key->GetMatrix2(), matchg );
				
				// リプレイ再生中
				if( REPLAY::GetStatus() == REP_REPLAY ){
					REPLAY::ReplayReadFrame( p6->vm->key->GetMatrix() );
				}
				
				p6->EmuVSYNC();			// 1画面分実行
				
				// ビデオキャプチャ中?
				if( AVI6::IsAVI() ){
					// ビデオキャプチャ中ならここでAVI保存処理
					// サウンド更新
					p6->SoundUpdate( 0, AVI6::GetAudioBuffer() );
					// 画面更新されたら AVI1画面保存
					if( p6->ScreenUpdate() ) AVI6::AVIWriteFrame( p6->graph->GetSubBuffer() );
				}else{
					// ビデオキャプチャ中でないなら通常の更新
					// サウンド更新
					p6->SoundUpdate( 0 );
					// 画面更新時期を迎えていたら画面更新
					// ノーウェイトの時にFPSが変わらないようにする
					if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
				}
				
				// 自動キー入力
				if( IsAutoKey() ){
					BYTE key = GetAutoKey();
					if( key ){
						if( key == 0x14 ) p6->vm->cpus->ReqKeyIntr( 6, GetAutoKey() );
						else			  p6->vm->cpus->ReqKeyIntr( 0, key );
					}
				}
			}
			
			// ウェイト
			p6->Wait();
		}
	}
}


////////////////////////////////////////////////////////////////
// Wait
////////////////////////////////////////////////////////////////
void EL6::Wait( void )
{
	if( sche->GetWaitEnable() && (!cfg->GetTurboTAPE() || (vm->cpus->GetCmtStatus() == CMTCLOSE)) )
		sche->VWait();
	vm->evsc->ReVSYNC();
}


////////////////////////////////////////////////////////////////
// 1命令実行
////////////////////////////////////////////////////////////////
int EL6::Emu( void )
{
	int st = vm->Emu();			// VM 1命令実行
	vm->evsc->Update( st );		// イベント更新
	sche->Update( st );
	
	return st;
}


////////////////////////////////////////////////////////////////
// 1画面分実行
////////////////////////////////////////////////////////////////
int EL6::EmuVSYNC( void )
{
	int state = 0;
	
	// VSYNCが発生するまで繰返し
	while( !vm->evsc->IsVSYNC() ){
		int st = vm->Emu();		// VM 1命令実行
		vm->evsc->Update( st );	// イベント更新
		sche->Update( st );
		state += st;
	}
	
	return state;
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////////////////////////////////////////
// 指定ステート数実行
////////////////////////////////////////////////////////////////
void EL6::Exec( int st )
{
	int State = st;
	
	while( State > 0 ) State -= Emu();
}


////////////////////////////////////////////////////////////////
// モニタモード切替え
////////////////////////////////////////////////////////////////
bool EL6::ToggleMonitor( void )
{
	// VM停止
	Stop();
	
	// モニタウィンドウ表示状態切換え
	cfg->SetMonDisp( cfg->GetMonDisp() ? false : true );
	
	// ブレークポイントの情報をクリア
	vm->BpClearStatus();
	
	// スクリーンサイズ変更
	graph->ResizeScreen();
	
	// VM再開
	Start();
	
	return cfg->GetMonDisp();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool EL6::Init( const CFG6 *config )
{
	// エラーメッセージ初期値
	Error::SetError( Error::InitFailed );
	
	if( !config ) return false;
	cfg = (CFG6 *)config;
	
	// 全オブジェクト削除
	DeleteAllObject();
	
	// パレット
	GPal.colors = new COLOR24[256];
	if( !GPal.colors ) return false;
	GPal.ncols = 256;
	
	for( int i=0; i<128; i++ ){
		cfg->GetColor( i, &GPal.colors[i] );
		VSurface::SetColor( i, COL2DW( GPal.colors[i] ) );

        // スキャンライン用カラー設定
        GPal.colors[i+128].r = ( GPal.colors[i].r * cfg->GetScanLineBr() ) / 100;
        GPal.colors[i+128].g = ( GPal.colors[i].g * cfg->GetScanLineBr() ) / 100;
        GPal.colors[i+128].b = ( GPal.colors[i].b * cfg->GetScanLineBr() ) / 100;
        VSurface::SetColor( i+128, COL2DW( GPal.colors[i+128] ) );
	}

    // パレット設定
    OSD_SetPalette(NULL, &GPal);

	// 機種別 VM確保
	switch( cfg->GetModel() ){
	case 61: vm = new VM61( this ); break;
	case 62: vm = new VM62( this ); break;
	case 66: vm = new VM66( this ); break;
	case 64: vm = new VM64( this ); break;
	case 68: vm = new VM68( this ); break;
	default: vm = new VM60( this );
	}
	// VM初期化
	if( !vm || !vm->AllocObject( cfg ) || !vm->Init( cfg ) ) return false;
	
	// 各種オブジェクト確保
	try{
		sche   = new SCH6( vm->GetCPUClock() * cfg->GetOverClock() / 100 );	// スケジューラ
		snd    = new SND6;											// サウンド
		graph  = new DSP6( vm );									// 画面描画
		joy    = new JOY6;											// ジョイスティック
		staw   = new cWndStat( vm );								// ステータスバー
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		regw   = new cWndReg( vm, DEV_ID("REGW") );					// レジスタウィンドウ
		memw   = new cWndMem( vm, DEV_ID("MEMW") );					// メモリウィンドウ
		monw   = new cWndMon( vm, DEV_ID("MONW") );					// モニタウィンドウ
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	}
	// new に失敗した場合
	catch( std::bad_alloc ){
		if( sche ) { delete sche;  sche  = NULL; }
		if( snd )  { delete snd;   snd   = NULL; }
		if( graph ){ delete graph; graph = NULL; }
		if( staw ) { delete staw;  staw  = NULL; }
		if( joy )  { delete joy;   joy   = NULL; }
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        if( regw ) { delete regw;  regw  = NULL; }
		if( memw ) { delete memw;  memw  = NULL; }
		if( monw ) { delete monw;  monw  = NULL; }
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

		return false;
	}
	
	// サウンド -----
    if( !snd->Init( this, NULL, cfg->GetSampleRate(), cfg->GetSoundBuffer() ) ) return false;
	snd->SetVolume( cfg->GetMasterVol() );
	
    // ステータスバー -----
    if( !staw->Init( graph->ScreenX(), cfg->GetFddNum() ) ) return false;

    // 画面描画 -----
    graph->SetIcon( cfg->GetModel() );	// アイコン設定
	if( !graph->Init() ) return false;
	
	// ジョイスティック -----
	if( !joy->Init() ) return false;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// レジスタウィンドウ　-----
	if( !regw->Init() ) return false;
	
	// メモリウィンドウ　-----
	if( !memw->Init() ) return false;
	
	// モニタウィンドウ　-----
	if( !monw->Init() ) return false;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// FPS表示タイマ設定
    //FPS表示タイマはQtで実装
    //	if( !SetFPSTimer( 1 ) ){
    //		Stop();
    //		return false;
    //	}
	
	// ビデオキャプチャ -----
	if( !AVI6::Init() ) return false;
	
	// リプレイ -----
	if( !REPLAY::Init( vm->key->GetMatrixSize() ) ) return false;
	
	// スクリーンサイズ変更
	graph->ResizeScreen();
	
	// ストリーム接続
	snd->ConnectStream( vm->psg );						// PSG
	snd->ConnectStream( vm->cmtl );						// CMT(LOAD)
	if( vm->voice ) snd->ConnectStream( vm->voice );	// 音声合成
	
	
	// TAPE挿入
	if( *(cfg->GetTapeFile()) ) TapeMount( cfg->GetTapeFile() );
	// ドライブ1,2にDISK挿入
	if( *(cfg->GetDiskFile( 1 )) ) DiskMount( 0, cfg->GetDiskFile( 1 ) );
	if( *(cfg->GetDiskFile( 2 )) ) DiskMount( 1, cfg->GetDiskFile( 2 ) );
	
	
	UI_Reset();
	
	
	// エラーなし
	Error::Reset();
	
	return true;
}


////////////////////////////////////////////////////////////////
// 動作開始
////////////////////////////////////////////////////////////////
bool EL6::Start( void )
{
	FSkipCount = 0;
	
	// スレッド生成
	if( !this->cThread::BeginThread( this ) ) return false;
	
	sche->Start();
	snd->Play();
	
	return true;
}


////////////////////////////////////////////////////////////////
// 動作停止
////////////////////////////////////////////////////////////////
void EL6::Stop( void )
{
	this->cThread::Cancel();	// スレッド終了フラグ立てる
	this->cThread::Waiting();	// スレッド終了まで待つ
	
	snd->Pause();
	sche->Stop();

}


////////////////////////////////////////////////////////////////
// イベントループ
////////////////////////////////////////////////////////////////
EL6::ReturnCode EL6::EventLoop( void )
{
	Event event;
	
	while( OSD_GetEvent( &event ) ){
		switch( event.type ){
		case EV_FPSUPDATE:		// FPS表示
			{
			char str[256];
			if( sche->GetPauseEnable() )
				sprintf( str, "%s === PAUSE ===", cfg->GetCaption() );
			else
				sprintf( str, "%s (%3d%%  %2d/%2d fps)", cfg->GetCaption(), sche->GetRatio(), event.fps.fps, FRAMERATE );
				if( sche->GetSpeedRatio() != 100 )
					sprintf( str+strlen(str), " [x%3.1f]", (double)sche->GetSpeedRatio()/100 );
					
			OSD_SetWindowCaption( graph->GetWindowHandle(), str );
			}
			break;
			
		case EV_KEYDOWN:		// キー入力
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			// モニタモード?
			if( cfg->GetMonDisp() ){
				monw->KeyIn( event.key.sym, event.key.mod & KVM_SHIFT, event.key.unicode );
				break;
			}else
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			// 各種機能キーチェック
			if( CheckFuncKey( event.key.sym,
							  event.key.mod & KVM_ALT  ? true : false,
							  event.key.mod & KVM_META ? true : false ) )
				break;
			// リプレイ再生中 or 自動キー入力実行中でなければ
			if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() )
				// キーマトリクス更新(キー)
				vm->key->UpdateMatrixKey( event.key.sym, true );
			break;
			
		case EV_KEYUP:
			// リプレイ再生中 or 自動キー入力実行中でなければ
			if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() )
				// キーマトリクス更新(キー)
				vm->key->UpdateMatrixKey( event.key.sym, false );
			break;
			
		case EV_JOYAXISMOTION:
		case EV_JOYBUTTONDOWN:
		case EV_JOYBUTTONUP:
			// リプレイ再生中 or 自動キー入力実行中でなければ
            if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() ){
                OSD_UpdateJoy();
				// キーマトリクス更新(ジョイスティック)
				vm->key->UpdateMatrixJoy( joy->GetJoyState( 0 ), joy->GetJoyState( 1 ) );
            }
            break;

		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		case EV_DEBUGMODEBP:	// モニタモード変更(ブレークポイント到達時)
			monw->BreakIn( event.bp.addr );		// ブレークポイントの情報を表示
			ToggleMonitor();					// モニタモード変更
			
			break;
			
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        case EV_CONTEXTMENU:	// コンテキストメニュー
                // ポップアップメニュー表示
                ShowPopupMenu( event.mousebt.x, event.mousebt.y );
            break;
		case EV_QUIT:			// 終了
			if( cfg->GetCkQuit() )
				if( OSD_Message( MSG_QUIT, MSG_QUITC, OSDM_YESNO | OSDM_ICONQUESTION ) != OSDR_YES )
					break;
			return Quit;
			
		case EV_RESTART:		// 再起動
			return Restart;
			
		case EV_DOKOLOAD:		// どこでもLOAD
			return Dokoload;
			
		case EV_REPLAY:			// リプレイ再生
			return Replay;

		case EV_DROPFILE:		// Drag & Drop
			if( !stricmp( "p6",  OSD_GetFileNameExt( event.drop.file ) ) ||
				!stricmp( "cas", OSD_GetFileNameExt( event.drop.file ) ) ||
				!stricmp( "p6t", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_TapeInsert( event.drop.file );
			}else if( !stricmp( "d88", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_DiskInsert( 0, event.drop.file );
			}else if( !stricmp( "rom",  OSD_GetFileNameExt( event.drop.file ) ) ||
				!stricmp( "bin", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_RomInsert( event.drop.file );
			}else if( !stricmp( "dds", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_DokoLoad( event.drop.file );
			}else if( !stricmp( "ddr", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_ReplayLoad( event.drop.file );
			}else if( !stricmp( "bas",  OSD_GetFileNameExt( event.drop.file ) ) ||
				!stricmp( "txt", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_AutoType( event.drop.file );
			}

			// ファイル名を開放
			delete [] event.drop.file;
			break;
			
		default:
			break;
		}
		
		// エラー処理
		switch( Error::GetError() ){
		case Error::NoError:
			break;
			
		default:
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			Error::Reset();
		}
	}
	return Quit;
}


////////////////////////////////////////////////////////////////
// 各種機能キーチェック
////////////////////////////////////////////////////////////////
bool EL6::CheckFuncKey( int kcode, bool OnALT, bool OnMETA )
{
	switch( kcode ){	// キーコード
	case KVC_F6:		// モニタモード変更 or スクリーンモード変更
		// ビデオキャプチャ中は無効
		if( AVI6::IsAVI() ) return false;
		
		if( OnALT ){
			Stop();
			cfg->SetFullScreen( cfg->GetFullScreen() ? false : true );
			graph->ResizeScreen();	// スクリーンサイズ変更
			Start();
		}else{
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			ToggleMonitor();
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		}
		break;
		
	case KVC_F7:			// スキャンラインモード変更
		// ビデオキャプチャ中は無効
		if( AVI6::IsAVI() ) return false;
		
		Stop();
		if( OnALT ){
			cfg->SetDispNTSC( cfg->GetDispNTSC() ? false : true );
		}else{
			cfg->SetScanLine( cfg->GetScanLine() ? false : true );
		}
		graph->ResizeScreen();	// スクリーンサイズ変更
		Start();
		break;
		
	case KVC_F8:			// モード4カラー変更 or ステータスバー表示状態変更
		if( OnALT ){
			Stop();
			cfg->SetDispStat( cfg->GetDispStat() ? false : true );
			graph->ResizeScreen();	// スクリーンサイズ変更
			Start();
		}else{
			int c = vm->vdg->GetMode4Color();
			if( ++c > 4 ) c = 0;
			vm->vdg->SetMode4Color( c );
		}
		break;

		
	case KVC_F9:			// ポーズ有効無効変更
		if( OnALT ){
		}else{
			sche->SetPauseEnable( sche->GetPauseEnable() ? false : true );
		}
		break;
		

	case KVC_F10:			// Wait有効無効変更
		if( OnALT ){
		}else{
			sche->SetWaitEnable( sche->GetWaitEnable() ? false : true );
		}
		break;
		
	case KVC_F11:			// リセット or 再起動
		if( OnALT ){
			OSD_PushEvent( EV_RESTART );
		}else{
			UI_Reset();
		}
		break;
		
	case KVC_F12:			// スナップショット
		if( OnALT ){
		}else{
			graph->SnapShot( cfg->GetImgPath() );
		}
		break;
		
	case KVX_MENU:			// ポップアップメニュー表示
		Stop();
		ShowPopupMenu( 0, 0 );
		Start();
		break;
	default:				// どれでもない
		return false;
	}
	return true;
}




////////////////////////////////////////////////////////////////
// 全オブジェクト削除
////////////////////////////////////////////////////////////////
void EL6::DeleteAllObject( void )
{
    //if( UpDateFPSID ) OSD_DelTimer( UpDateFPSID );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( monw ) { delete monw;	monw = NULL; }
	if( memw ) { delete memw;	memw = NULL; }
	if( regw ) { delete regw;	regw = NULL; }
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	if( staw ) { delete staw;	staw  = NULL; }
	if( joy )  { delete joy;	joy   = NULL; }
	if( snd )  { delete snd;	snd   = NULL; }
	if( graph ){ delete graph;	graph = NULL; }
	if( sche ) { delete sche;	sche  = NULL; }
	if( vm )   { delete vm;		vm    = NULL; }
	
	// 自動キー入力バッファ
	if( ak.Buffer ){   delete [] ak.Buffer;		ak.Buffer = NULL; }
	
	// パレット
	if( GPal.colors ){ delete [] GPal.colors;	GPal.colors = NULL; }
}


////////////////////////////////////////////////////////////////
// 画面更新
////////////////////////////////////////////////////////////////
bool EL6::ScreenUpdate( void )
{
	// フレームスキップチェック
	if( ++FSkipCount > cfg->GetFrameSkip() ){
		// ステータスバー更新
		staw->SetReplayStatus( REPLAY::GetStatus() );	// リプレイステータス
		vm->vdg->UpdateBackBuf();	// バックバッファ更新
		graph->DrawScreen();		// 画面描画
		
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		if( cfg->GetMonDisp() ){
			regw->Update();
			memw->Update();
			monw->Update();
		}
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		
		UDFPSCount++;
		FSkipCount = 0;
		
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// サウンド更新
//
// 引数:	samples		更新するサンプル数(-1:残りバッファ全て 0:処理クロック分)
//			exbuf		外部バッファ
// 返値:	int			更新したサンプル数
////////////////////////////////////////////////////////////////
int EL6::SoundUpdate( int samples, cRing *exbuf )
{
	// PSG更新
	vm->psg->SoundUpdate( samples );
	int size = vm->psg->SndDev::cRing::ReadySize();
	
	// CMT(LOAD)更新
	vm->cmtl->SoundUpdate( size );
	
	// 音声合成更新
	if( vm->voice ) vm->voice->SoundUpdate( size );
	
	// サウンドバッファ更新
	int ret = snd->PreUpdate( size, exbuf );
	
    // サウンド更新
    snd->Update();
	return ret;
}


////////////////////////////////////////////////////////////////
// ストリーム更新 コールバック関数
//
// 引数:	userdata	コールバック関数に渡すデータ(自分自身へのオブジェクトポインタ)
//			stream		ストリーム書込みバッファへのポインタ
//			len			バッファサイズ(バイト単位)
// 返値:	なし
////////////////////////////////////////////////////////////////
// P6VXでは使わない
//void EL6::StreamUpdate( void *userdata, BYTE *stream, int len )
//{
//	EL6 *p6 = STATIC_CAST( EL6 *, userdata );	// 自分自身のオブジェクトポインタ取得
	
//	// サウンドバッファ更新
//	//  もしサンプル数が足りなければここで追加
//	//  ただしビデオキャプチャ中は無視
//	int addsam = len/sizeof(int16_t) - p6->snd->cRing::ReadySize();
	
//	p6->snd->Update( stream, min( (int)(len/sizeof(int16_t)), p6->snd->cRing::ReadySize() ) );
	
//	if( addsam > 0 && !p6->AVI6::IsAVI() ){
//		p6->SoundUpdate( addsam );
//		p6->snd->Update( stream, addsam );
//	}
	
//}


////////////////////////////////////////////////////////////////
// FPS表示タイマ コールバック関数
////////////////////////////////////////////////////////////////
// FPSタイマはQtで実装
//DWORD EL6::UpDateFPS( DWORD interval, void *obj )
//{
//	EL6 *p6 = STATIC_CAST( EL6 *, obj );	// 自分自身のオブジェクトポインタ取得
	
//	OSD_PushEvent( EV_FPSUPDATE, p6->UDFPSCount );
//	p6->UDFPSCount = 0;
	
//	return interval;
//}


////////////////////////////////////////////////////////////////
// FPS表示タイマ設定
////////////////////////////////////////////////////////////////
// FPSタイマはQtで実装
//bool EL6::SetFPSTimer( int fps )
//{
//	// タイマ稼動中なら停止
//	if( UpDateFPSID ){
//		OSD_DelTimer( UpDateFPSID );
//		UpDateFPSID = NULL;
//	}
	
//	// タイマ設定
//	if( fps > 0 )
//		UpDateFPSID = OSD_AddTimer( 1000/fps, EL6::UpDateFPS, this );
	
//	return UpDateFPSID ? true : false;
//}


////////////////////////////////////////////////////////////////
// 自動キー入力実行中?
//
// 引数:	なし
// 返値:	bool	true:実行中 false:実行中でない
////////////////////////////////////////////////////////////////
bool EL6::IsAutoKey( void )
{
	return ak.Num ? true : false;
}


////////////////////////////////////////////////////////////////
// 自動キー入力1文字取得
//   (VSYNC=1/60sec毎に呼ばれる)
//
// 引数:	なし
// 返値:	BYTE	P6のキーコード
////////////////////////////////////////////////////////////////
char EL6::GetAutoKey( void )
{
	// リレーON待ち
	if( ak.RelayOn ){
		if( vm->cmtl->IsRelay() ) ak.RelayOn = false;
		else                      return 0;
	}
	
	// リレーOFF待ち
	if( ak.Relay ){
		if( !vm->cmtl->IsRelay() ) ak.Relay = false;
		else                       return 0;
	}
	
	// 待ち?
	if( ak.Wait > 0 ){
		ak.Wait--;
		return 0;
	}
	
	
	// 次の文字を取得
	if( ak.Num-- > 0 ){
		BYTE dat = ak.Buffer[ak.Seek++];
		switch( dat ){
		case 0x17:	// '\w' ウェイト設定
			if( ak.Num-- > 0 ) ak.Wait += (int)((BYTE)ak.Buffer[ak.Seek++]);	// 待ち回数設定
			dat = 0;
			break;
			
		case 0x0a:	// '\r' リレーOFF待ち
			ak.Relay   = true;
			ak.RelayOn = true;
			dat = 0x0d;
		case 0x0d:	// '\n' 改行?
			ak.Wait = 9;	// 待ち9回(=150msec)
			break;
			
		default:	// 一般の文字
			ak.Wait = 0;	// 待ちなし
		}
		return dat;
	}else{
		// バッファが空なら終了
		delete [] ak.Buffer;
		ak.Buffer = NULL;
	}
	return 0;
}


////////////////////////////////////////////////////////////////
// 自動キー入力文字列設定
//
// 引数:	str		文字列
//			num		文字列の長さ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::SetAutoKey( const char *str, int num )
{
	// キーバッファを一旦削除
	
	// キーバッファ確保
	if( ak.Buffer ) delete [] ak.Buffer;
	ak.Buffer = new char[num+1];
	if( !ak.Buffer ){
		ak.Num = 0;
		return false;
	}
	
	// 文字列をコピー
	strncpy( ak.Buffer, str, num );
	
	ak.Num     = num;	// 残り文字数
	ak.Wait    = 60;	// 待ち回数カウンタ(初回は1sec)
	ak.Relay   = false;	// リレースイッチOFF待ちフラグ
	ak.RelayOn = false;	// リレースイッチON待ちフラグ
	ak.Seek    = 0;		// 読込みポインタ
	
	return true;
}


////////////////////////////////////////////////////////////////
// 自動キー入力文字列設定(ファイルから)
//
// 引数:	filepath	入力ファイルへのパス
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::SetAutoKeyFile( const char *filepath )
{
	FILE *fp;
	char lbuf[1024];
	
	fp = FOPENEN( filepath, "rb" );
	if( !fp ) return false;
	
	fseek( fp, 0, SEEK_END );
	int tsize = ftell( fp ) + 1;
	fseek( fp, 0, SEEK_SET );
	
	// キーバッファ確保
	if( ak.Buffer ) delete [] ak.Buffer;
	ak.Buffer = new char[tsize+1];
	if( !ak.Buffer ){
		fclose( fp );
		ak.Num = 0;
		return false;
	}
	ZeroMemory( ak.Buffer, sizeof(ak.Buffer) );
	
	// 文字列を読込み
	// データが無くなるまで繰り返し
	while( fgets( lbuf, 1024, fp ) ){
		Sjis2P6( lbuf, lbuf );	// SJIS -> P6
		strcat( ak.Buffer, lbuf );
	}
	fclose( fp );
	tsize = strlen( ak.Buffer );
	
	ak.Num     = tsize;	// 残り文字数
	ak.Wait    = 60;	// 待ち回数カウンタ(初回は1sec)
	ak.Relay   = false;	// リレースイッチOFF待ちフラグ
	ak.RelayOn = false;	// リレースイッチON待ちフラグ
	ak.Seek    = 0;		// 読込みポインタ
	
	return true;
}


////////////////////////////////////////////////////////////////
// オートスタート文字列設定
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::SetAutoStart( void )
{
	char kbuf[256] = "";
	char *buf = kbuf;
	
	if( !(vm->cmtl->IsMount() && vm->cmtl->IsAutoStart()) ) return;
	
	const P6TAUTOINFO *ainf = vm->cmtl->GetAutoStartInfo();
	
	// キーバッファに書込み
	switch( cfg->GetModel() ){
	case 60:	// PC-6001
	case 61:	// PC-6001A
		sprintf( buf, "%c%c", ainf->Page+'0', 0x0d );
		break;
		
	case 64:	// PC-6001mk2SR
		if( ainf->BASIC == 6 ){
			if( vm->disk->GetDrives() )	// ??? 実際は?
				sprintf( buf, "%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 20, 0x0d, 0x17, 10 );
			else
				sprintf( buf, "%c%c%c%c%c%c%c",   0x17, 10, ainf->BASIC+'0', 0x17, 20,       0x17, 10 );
			break;
		}
		
	case 62:	// PC-6001mk2
		switch( ainf->BASIC ){
		case 3:
		case 4:
		case 5:
			if( vm->disk->GetDrives() )	// ??? 実際は?
				sprintf( buf, "%c%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 30, 0x0d, ainf->Page+'0', 0x0d, 0x17, 120 );
			else
				sprintf( buf, "%c%c%c%c%c%c%c%c%c",   0x17, 50, ainf->BASIC+'0', 0x17, 30,       ainf->Page+'0', 0x0d, 0x17, 120 );
			break;
		default:
			sprintf( buf, "%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 30, ainf->Page+'0', 0x0d, 0x17, 20 );
		}
		break;
		
	case 68:	// PC-6601SR
		if( ainf->BASIC == 6 ){
			if( vm->disk->IsMount( 0 ) )
				sprintf( buf, "%c%c%c%c%c%c%c%c%c", 0x17, 260, 0x14, 0xf4, 0x17, 30, 0x0d, 0x17, 10 );
			else
				sprintf( buf, "%c%c%c%c%c%c%c%c",   0x17, 200, 0x14, 0xf4, 0x17, 30,       0x17, 10 );
			break;
		}else{
			sprintf( buf, "%c%c%c%c%c%c%c%c", 0x17, 200, 0x17, vm->disk->IsMount( 0 ) ? 60 : 1, 0x17, vm->disk->IsMount( 1 ) ? 60 : 1, 0x14, 0xf3 );
		}
		buf += strlen(kbuf);
		
	case 66:	// PC-6601
		switch( ainf->BASIC ){
		case 3:
		case 4:
		case 5:
			if( vm->disk->IsMount( 0 ) )
				sprintf( buf, "%c%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 30, 0x0d, ainf->Page+'0', 0x0d, 0x17, 110 );
			else
				sprintf( buf, "%c%c%c%c%c%c%c%c%c",   0x17, 50, ainf->BASIC+'0', 0x17, 30,       ainf->Page+'0', 0x0d, 0x17, 110 );
			break;
		default:
			sprintf( buf, "%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 30, ainf->Page+'0', 0x0d, 0x17, 10 );
		}
		break;
		
	}
	strcat( kbuf, ainf->ask );
		
	// 自動キー入力設定
	if( SetAutoKey( kbuf, strlen(kbuf) ) );
}




////////////////////////////////////////////////////////////////
// モニタモード?
//
// 引数:	なし
// 返値:	bool		true:モニタモード false:実行中
////////////////////////////////////////////////////////////////
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
bool EL6::IsMonitor( void ) const
{
	return cfg->GetMonDisp();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	filename	ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::DokoDemoSave( const char *filename )
{
	cIni *Ini = NULL;
	
	// とりあえずエラー設定
	Error::SetError( Error::DokoWriteFailed );
	try{
		FILE *fp = FOPENEN( filename, "wt" );
		if( !fp ) throw Error::DokoWriteFailed;
		// タイトル行を出力して一旦閉じる
		fprintf( fp, MSDOKO_TITLE );
		fclose( fp );
		
		// どこでもSAVEファイルを開く
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::DokoWriteFailed;
		
		// 各オブジェクトのパラメータ書込み
		if( !cfg->DokoSave( Ini )      ||
			!vm->evsc->DokoSave( Ini ) ||
			!vm->intr->DokoSave( Ini ) ||
			!vm->cpum->DokoSave( Ini ) ||
			!vm->cpus->DokoSave( Ini ) ||
			!vm->mem->DokoSave( Ini )  ||
			!vm->vdg->DokoSave( Ini )  ||
			!vm->psg->DokoSave( Ini )  ||
			!vm->pio->DokoSave( Ini )  ||
			!vm->key->DokoSave( Ini )  ||
			!vm->cmtl->DokoSave( Ini ) ||
			!vm->disk->DokoSave( Ini )
		) throw Error::GetError();
		if( vm->voice && !vm->voice->DokoSave( Ini ) ) throw Error::GetError();
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		if( Ini ) delete Ini;
		return false;
	}
	
	Ini->Write();
	
	delete Ini;
	
	// 無事だったのでエラーなし
	Error::Reset();
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	filename	ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::DokoDemoLoad( const char *filename )
{
	cIni *Ini = NULL;
	
	// とりあえずエラー設定
	Error::SetError( Error::DokoReadFailed );
	try{
		// どこでもLOADファイルを開く
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::DokoReadFailed;
//		cfg->SetDokoFile( filename );
		
		// PC6001Vのバージョン確認と主要構成情報を読込み
		// (機種,FDD台数,拡張RAM,ROMパッチ,戦士のカートリッジ)
		if( !cfg->DokoLoad( Ini ) ) throw Error::GetError();

        // VM停止
        Stop();

		// VM再初期化
		Init( cfg );
		
		// 各オブジェクトのパラメータ読込み
		if(	!vm->evsc->DokoLoad( Ini ) ||
			!vm->intr->DokoLoad( Ini ) ||
			!vm->cpum->DokoLoad( Ini ) ||
			!vm->cpus->DokoLoad( Ini ) ||
			!vm->mem->DokoLoad( Ini )  ||
			!vm->vdg->DokoLoad( Ini )  ||
			!vm->psg->DokoLoad( Ini )  ||
			!vm->pio->DokoLoad( Ini )  ||
			!vm->key->DokoLoad( Ini )  ||
			!vm->cmtl->DokoLoad( Ini ) ||
			!vm->disk->DokoLoad( Ini )
		) throw Error::GetError();
		if( vm->voice && !vm->voice->DokoLoad( Ini ) ) throw Error::GetError();
		
		// ディスクドライブ数によってスクリーンサイズ変更
		if( !staw->Init( -1, vm->disk->GetDrives() ) ) throw Error::GetError();
		if( !graph->ResizeScreen() ) throw Error::GetError();
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		if( Ini ) delete Ini;
		return false;
	}
	
	delete Ini;
//	cfg->SetDokoFile( "" );
	
	// 無事だったのでエラーなし
	Error::Reset();
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOADファイルから機種名読込
//
// 引数:	filename	ファイル名
// 返値:	int			機種名(60,61,62,66)
////////////////////////////////////////////////////////////////
int EL6::GetDokoModel( const char *filename )
{
	cIni *Ini = NULL;
	int st;
	
	try{
		// どこでもLOADファイルを開く
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::DokoReadFailed;
		
		// 機種取得
		Ini->GetInt( "GLOBAL", "Model",	&st, 0 );
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return 0;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		if( Ini ) delete Ini;
		return 0;
	}
	
	delete Ini;
	
	return st;
}







////////////////////////////////////////////////////////////////
// TAPE マウント
//
// 引数:	filename	ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::TapeMount( const char *filename )
{
	if( !vm->cmtl->Mount( filename ) ) return false;
	return true;
}


////////////////////////////////////////////////////////////////
// TAPE アンマウント
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::TapeUnmount( void )
{
	vm->cmtl->Unmount();
}


////////////////////////////////////////////////////////////////
// DISK マウント
//
// 引数:	drv			ドライブ番号
//			filename	ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::DiskMount( int drv, const char *filename )
{
	if( !vm->disk->Mount( drv, filename ) ) return false;
	return true;
}


////////////////////////////////////////////////////////////////
// DISK アンマウント
//
// 引数:	drv			ドライブ番号
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::DiskUnmount( int drv )
{
	vm->disk->Unmount( drv );
}







////////////////////////////////////////////////////////////////
// リプレイ保存開始
//
// 引数:	filename	ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::ReplayRecStart( const char *filename )
{
	return REPLAY::StartRecord( filename );
}


////////////////////////////////////////////////////////////////
// リプレイ保存停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::ReplayRecStop( void )
{
	REPLAY::StopRecord();
}


////////////////////////////////////////////////////////////////
// リプレイ再生開始
//
// 引数:	filename	ファイル名
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::ReplayPlayStart( const char *filename )
{
	cfg->SetModel( GetDokoModel( filename ) );
	cfg->SetDokoFile( filename );
	OSD_PushEvent( EV_REPLAY );
}


////////////////////////////////////////////////////////////////
// リプレイ再生停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::ReplayPlayStop( void )
{
	REPLAY::StopReplay();
}

////////////////////////////////////////////////////////////////
// UI:TAPE 挿入
//
// 引数:	path		ファイル名
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_TapeInsert( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path ){
		if( !OSD_FileExist( TapePathUI ) )
			strncpy( TapePathUI, cfg->GetTapePath(), PATH_MAX );
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_TapeLoad, str, TapePathUI ) )
			fpath = str;
	}
	if( !fpath ) return;
	
	if( !TapeMount( fpath ) ) Error::SetError( Error::TapeMountFailed );
}



////////////////////////////////////////////////////////////////
// UI:DISK 挿入
//
// 引数:	drv			ドライブ番号
//			path		ファイル名
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_DiskInsert( int drv, const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path ){
		if( !OSD_FileExist( DiskPathUI ) )
			strncpy( DiskPathUI, cfg->GetDiskPath(), PATH_MAX );
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_Disk, str, DiskPathUI ) )
			fpath = str;
	}
	if( !fpath ) return;
	
	if( !DiskMount( drv, fpath ) ) Error::SetError( Error::DiskMountFailed );
}


////////////////////////////////////////////////////////////////
// UI:拡張ROM 挿入
//
// 引数:	path		ファイル名
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_RomInsert( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path ){
		if( !OSD_FileExist( ExRomPathUI ) )
			strncpy( ExRomPathUI, cfg->GetExtRomPath(), PATH_MAX );
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_ExtRom, str, ExRomPathUI ) )
			fpath = str;
	}
	if( !fpath ) return;
	
	// リセットを伴うのでメッセージ表示
	OSD_Message( MSG_RESETI, MSG_RESETC, OSDM_OK | OSDM_ICONINFO );
	if( !vm->mem->MountExtRom( fpath ) )
		Error::SetError( Error::ExtRomMountFailed );
	else
		UI_Reset();
}


////////////////////////////////////////////////////////////////
// UI:拡張ROM 排出
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_RomEject( void )
{
	// リセットを伴うのでメッセージ表示
	OSD_Message( MSG_RESETE, MSG_RESETC, OSDM_OK | OSDM_ICONINFO );
	vm->mem->UnmountExtRom();
	UI_Reset();
}


////////////////////////////////////////////////////////////////
// UI:どこでもSAVE
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_DokoSave( void )
{
	char str[PATH_MAX];
	
	if( OSD_FileSelect( graph->GetWindowHandle(), FD_DokoSave, str, (char *)OSD_GetModulePath() ) )
	DokoDemoSave( str );
}


////////////////////////////////////////////////////////////////
// UI:どこでもLOAD
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_DokoLoad( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path && OSD_FileSelect( graph->GetWindowHandle(), FD_DokoLoad, str, (char *)OSD_GetModulePath() ) )
		fpath = str;
	
	if( !fpath ) return;
	
	cfg->SetModel( GetDokoModel( fpath ) );
	cfg->SetDokoFile( fpath );
	OSD_PushEvent( EV_DOKOLOAD );
}


////////////////////////////////////////////////////////////////
// UI:リプレイ保存
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_ReplaySave( void )
{
	char str[PATH_MAX];
	
	if( REPLAY::GetStatus() == REP_IDLE ){
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_RepSave, str, (char *)OSD_GetModulePath() ) ){
			if( DokoDemoSave( str ) ) ReplayRecStart( str );
		}
	}else if( REPLAY::GetStatus() == REP_RECORD ){
		ReplayRecStop();
	}
}


////////////////////////////////////////////////////////////////
// UI:リプレイ再生
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_ReplayLoad( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( REPLAY::GetStatus() == REP_IDLE ){
		if( !path && OSD_FileSelect( graph->GetWindowHandle(), FD_RepLoad, str, (char *)OSD_GetModulePath() ) )
			fpath = str;
	}else if( REPLAY::GetStatus() == REP_REPLAY ){
		ReplayPlayStop();
	}
	
	if( !fpath ) return;
	
	ReplayPlayStart( fpath );
}


////////////////////////////////////////////////////////////////
// UI:ビデオキャプチャ
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_AVISave( void )
{
	char str[PATH_MAX];
	
	if( !AVI6::IsAVI() ){
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_AVISave, str, (char *)OSD_GetModulePath() ) ){
			AVI6::StartAVI( str, graph->GetSubBuffer(), FRAMERATE, cfg->GetSampleRate(), cfg->GetAviBpp() );
		}
	}else{
		AVI6::StopAVI();
	}
}


////////////////////////////////////////////////////////////////
// UI:打込み代行
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_AutoType( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path && OSD_FileSelect( graph->GetWindowHandle(), FD_LoadAll, str, (char *)OSD_GetModulePath() ) )
		fpath = str;
	
	if( !fpath ) return;
	
	if( !SetAutoKeyFile( fpath ) ) Error::SetError( Error::Unknown );
}


////////////////////////////////////////////////////////////////
// UI:リセット
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_Reset( void )
{
	bool can = this->cThread::IsCancel();	// スレッド停止済み?
	
	if( !can ) Stop();
	
	// システムディスクが入っていたらTAPEのオートスタート無効
	if( !vm->disk->IsSystem(0) && !vm->disk->IsSystem(1) )
		SetAutoStart();
	
	vm->Reset();
	
	if( !can ) Start();
}


////////////////////////////////////////////////////////////////
// UI:再起動
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_Restart( void )
{
	OSD_PushEvent( EV_RESTART );
}


////////////////////////////////////////////////////////////////
// UI:終了
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_Quit( void )
{
	OSD_PushEvent( EV_QUIT );
}


////////////////////////////////////////////////////////////////
// UI:Wait変更
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_NoWait( void )
{
	sche->SetWaitEnable( sche->GetWaitEnable() ? false : true );
}


////////////////////////////////////////////////////////////////
// UI:Turbo TAPE変更
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_TurboTape( void )
{
	cfg->SetTurboTAPE( cfg->GetTurboTAPE() ? false : true );
}


////////////////////////////////////////////////////////////////
// UI:Boost Up変更
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_BoostUp( void )
{
	cfg->SetBoostUp( cfg->GetBoostUp() ? false : true );
	vm->cmtl->SetBoost( vm->cmtl->IsBoostUp() ? false : true );
}


////////////////////////////////////////////////////////////////
// UI:スキャンラインモード変更
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_ScanLine( void )
{
	// ビデオキャプチャ中は無効
	if( !AVI6::IsAVI() ){
		cfg->SetScanLine( cfg->GetScanLine() ? false : true );
		graph->ResizeScreen();	// スクリーンサイズ変更
	}
}


////////////////////////////////////////////////////////////////
// UI:4:3表示変更
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_Disp43( void )
{
	// ビデオキャプチャ中は無効
	if( !AVI6::IsAVI() ){
		cfg->SetDispNTSC( cfg->GetDispNTSC() ? false : true );
		graph->ResizeScreen();	// スクリーンサイズ変更
	}
}


////////////////////////////////////////////////////////////////
// UI:ステータスバー表示状態変更
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_StatusBar( void )
{
	cfg->SetDispStat( cfg->GetDispStat() ? false : true );
	graph->ResizeScreen();	// スクリーンサイズ変更
}


////////////////////////////////////////////////////////////////
// UI:MODE4カラー変更
//
// 引数:	col			0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_Mode4Color( int col )
{
	cfg->SetMode4Color( col );
	vm->vdg->SetMode4Color( col );
}


////////////////////////////////////////////////////////////////
// UI:フレームスキップ変更
//
// 引数:	sk			フレームスキップ数
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_FrameSkip( int sk )
{
	if( !AVI6::IsAVI() ) cfg->SetFrameSkip( sk );
}


////////////////////////////////////////////////////////////////
// UI:サンプリングレート変更
//
// 引数:	rate		サンプリングレート(Hz)
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_SampleRate( int rate )
{
	cfg->SetSampleRate( rate );
	snd->SetSampleRate( rate );
}


////////////////////////////////////////////////////////////////
// UI:環境設定
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::UI_Config( void )
{
	cfg->Write();
	if( OSD_ConfigDialog( graph->GetWindowHandle() ) > 0 )
		// 再起動?
		if( OSD_Message( MSG_RESTART, MSG_RESTARTC, OSDM_YESNO | OSDM_ICONQUESTION ) == OSDR_YES )
			OSD_PushEvent( EV_RESTART );
}



