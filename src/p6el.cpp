#include <new>

#include "pc6001v.h"
#include "p6el.h"
#include "error.h"

#include "breakpoint.h"
#include "common.h"
#include "cpum.h"
#include "cpus.h"
#include "debug.h"
#include "disk.h"
#include "graph.h"
#include "intr.h"
#include "keyboard.h"
#include "memory.h"
#include "osd.h"
#include "pio.h"
#include "psg.h"
#include "schedule.h"
#include "sound.h"
#include "status.h"
#include "tape.h"
#include "voice.h"
#include "vdg.h"
#include "vsurface.h"




////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
EL6::EL6( void ) : vm(NULL), cfg(NULL), sche(NULL), graph(NULL), snd(NULL), joy(NULL), staw(NULL),
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	regw(NULL), memw(NULL), monw(NULL),
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	UpDateFPSID(NULL), UDFPSCount(0), FSkipCount(0)
{}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
EL6::~EL6( void )
{
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( monw )   delete monw;
	if( memw )   delete memw;
	if( regw )   delete regw;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	if( staw )   delete staw;
	if( joy )    delete joy;
	if( snd )    delete snd;
	if( graph )  delete graph;
	if( sche )   delete sche;
	if( vm )     delete vm;
	
	// 自動キー入力バッファ破棄
	if( ak.Buffer ) delete [] ak.Buffer;
	
	if( GPal.colors ) delete [] GPal.colors;
}



////////////////////////////////////////////////////////////////
// スレッド関数(オーバーライド)
////////////////////////////////////////////////////////////////
void EL6::OnThread( void *inst )
{
	EL6 *p6;
	
	p6 = STATIC_CAST( EL6 *, inst );	// 自分自身のオブジェクトポインタ取得
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( p6->cfg->GetMonDisp() ){
	// モニタモード
		while( !this->cThread::IsCancel() ){
			// 画面更新
			if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
			p6->Wait();		// ウェイト
		}
	}else
	// 通常モード
		if( p6->vm->bp->ExistBreakPoint() ){
			// ブレークポイントあり
			while( !this->cThread::IsCancel() ){
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
						if( key ) p6->vm->cpus->ReqKeyIntr( 0, key );
					}
					
					// ウェイト
					p6->Wait();
				}
				
				// ブレークポイントチェック
				if( p6->vm->bp->CheckBreakPoint( BPoint::BP_PC, p6->vm->cpum->GetPC(), 0, NULL ) || p6->vm->bp->IsReqBreak() ){
					this->cThread::Cancel();	// スレッド終了フラグ立てる
					p6->vm->bp->ResetBreak();
                    Event ev;
                    ev.type = EV_DEBUGMODEBP;
                    ev.bp.addr = p6->vm->cpum->GetPC();
                    OSD_PushEvent( ev );
				}
			}
		}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	{	// 通常実行
		while( !this->cThread::IsCancel() ){
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
				// 画面更新
				bool update = p6->ScreenUpdate();
				// 画面更新されていたら AVI1画面保存
				if( update ) AVI6::AVIWriteFrame( p6->graph->GetSubBuffer() );
			}else{
				// ビデオキャプチャ中でないなら通常の更新
				// サウンド更新
				p6->SoundUpdate( 0 );
				// 画面更新
				// 画面更新時期を迎えていたら画面更新
				// ノーウェイトの時にFPSが変わらないようにする
				if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
			}
			
			// 自動キー入力
			if( IsAutoKey() ){
				BYTE key = GetAutoKey();
				if( key ) p6->vm->cpus->ReqKeyIntr( 0, key );
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


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
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
	vm->bp->ClearStatus();
	
	// スクリーンサイズ変更
	graph->ResizeScreen();
	
	// VM再開
	Start();
	
	return cfg->GetMonDisp();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool EL6::Init( const CFG6 *config )
{
	// エラーメッセージ初期値
	Error::SetError( Error::InitFailed );
	
	if( !config ) return false;
	cfg = CONST_CAST( CFG6 *, config );
	
	// パレット
	GPal.colors = new COLOR24[256];
	if( !GPal.colors ) return false;
	GPal.ncols = 256;
	
	for( int i=0; i<128; i++ ){
		cfg->GetColor( i, &GPal.colors[i] );
		
		// スキャンライン用カラー設定
		GPal.colors[i+128].r = ( GPal.colors[i].r * cfg->GetScanLineBr() ) / 100;
		GPal.colors[i+128].g = ( GPal.colors[i].g * cfg->GetScanLineBr() ) / 100;
		GPal.colors[i+128].b = ( GPal.colors[i].b * cfg->GetScanLineBr() ) / 100;
	}
	
	// 機種別 VM確保
	switch( cfg->GetModel() ){
	case 62: vm = new VM62( this ); break;
	case 66: vm = new VM66( this ); break;
	default: vm = new VM60( this );
	}
	// VM初期化
	if( !vm || !vm->Init( cfg ) ) return false;
	
	// 各種オブジェクト確保
	sche   = new SCH6( vm->GetCPUClock() * cfg->GetOverClock() / 100 );	// スケジューラ
	snd    = new SND6;											// サウンド
	graph  = new DSP6( vm );									// 画面描画
	joy    = new JOY6;											// ジョイスティック
	staw   = new cWndStat( vm );								// ステータスバー
	
	if( !sche || !snd || !graph || !staw || !joy ) return false;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	regw   = new cWndReg( vm, DEV_ID("REGW") );					// レジスタウィンドウ
	memw   = new cWndMem( vm, DEV_ID("MEMW") );					// メモリウィンドウ
	monw   = new cWndMon( vm, DEV_ID("MONW") );					// モニタウィンドウ
	
	if( !regw || !memw || !monw ) return false;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	// サウンド -----
	if( !snd->Init( this, EL6::StreamUpdate, cfg->GetSampleRate(), cfg->GetSoundBuffer() ) ) return false;
	snd->SetVolume( cfg->GetMasterVol() );
	
	// 画面描画 -----
    graph->SetIcon( cfg->GetModel() );	// アイコン設定
    if( !graph->Init( cfg->GetScrBpp(), cfg->GetScanLineBr(), &GPal ) ) return false;
	
	// ジョイスティック -----
	if( !joy->Init() ) return false;
	
	// ステータスバー -----
	if( !staw->Init( graph->ScreenX(), cfg->GetFddNum() ) ) return false;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	// レジスタウィンドウ　-----
	if( !regw->Init() ) return false;
	
	// メモリウィンドウ　-----
	if( !memw->Init() ) return false;
	
	// モニタウィンドウ　-----
	if( !monw->Init() ) return false;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	// FPS表示タイマ設定
    //#PENDING FPS表示タイマはQtで実装
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
	
	
	// TAPE挿入
	if( *(cfg->GetTapeFile()) ) TapeMount( cfg->GetTapeFile() );
	// ドライブ1,2にDISK挿入
	if( *(cfg->GetDiskFile( 1 )) ) DiskMount( 0, cfg->GetDiskFile( 1 ) );
	if( *(cfg->GetDiskFile( 2 )) ) DiskMount( 1, cfg->GetDiskFile( 2 ) );
	
	
	vm->Reset();
	
	
	// ストリーム接続
	snd->ConnectStream( STATIC_CAST( SndDev *, vm->psg ) );						// PSG
	snd->ConnectStream( STATIC_CAST( SndDev *, vm->cmtl ) );					// CMT(LOAD)
	if( vm->voice ) snd->ConnectStream( STATIC_CAST( SndDev *, vm->voice ) );	// 音声合成
	
	// エラーなし
	Error::SetError( Error::NoError );
	
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
			sprintf( str, "%s (%3d%%  %2d/%2d fps)", cfg->GetCaption(), sche->GetRatio(), event.fps.fps, FRAMERATE );
			OSD_SetWindowCaption( graph->GetWindowHandle(), str );
			}
			break;
			
		case EV_KEYDOWN:		// キー入力
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
			// モニタモード?
			if( cfg->GetMonDisp() ){
				monw->KeyIn( event.key.sym, event.key.mod & KVM_SHIFT, event.key.unicode );
				break;
			}else
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
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
			if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() )
				// キーマトリクス更新(ジョイスティック)
				vm->key->UpdateMatrixJoy( joy->GetJoyState( 0 ), joy->GetJoyState( 1 ) );
			break;
			
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
		case EV_DEBUGMODEBP:	// モニタモード変更(ブレークポイント到達時)
			monw->BreakIn( event.bp.addr );		// ブレークポイントの情報を表示
			ToggleMonitor();					// モニタモード変更
			
			break;
			
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
            //#PENDING 消す
//        case EV_MOUSEBUTTONUP:	// マウスボタンクリック
//            if( ( event.mousebt.button == MBT_RIGHT ) && ( event.mousebt.state == false ) ){
//                Stop();

//                // ポップアップメニュー表示
//                ShowPopupMenu( event.mousebt.x, event.mousebt.y );

//                Start();
//            }
//            break;
        case EV_CONTEXTMENU:	// コンテキストメニュー
                Stop();
                // ポップアップメニュー表示
                ShowPopupMenu( event.mousebt.x, event.mousebt.y );
                Start();
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
			
		default:
			break;
		}
		
		// エラー処理
		switch( Error::GetError() ){
		case Error::NoError:
			break;
			
		default:
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			Error::SetError( Error::NoError );
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
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
			ToggleMonitor();
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
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
			cfg->SetStatDisp( cfg->GetStatDisp() ? false : true );
			graph->ResizeScreen();	// スクリーンサイズ変更
			Start();
		}else{
			int c = vm->vdg->GetMode4Color();
			if( ++c > 4 ) c = 0;
			vm->vdg->SetMode4Color( c );
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
			vm->Reset();
		}
		break;
		
	case KVC_F12:			// スナップショット
		if( OnALT ){
		}else{
			graph->SnapShot( cfg->GetImgPath() );
		}
		break;
		
	default:				// どれでもない
		return false;
	}
	return true;
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
		
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
		if( cfg->GetMonDisp() ){
			regw->Update();
			memw->Update();
			monw->Update();
		}
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
		
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
void EL6::StreamUpdate( void *userdata, BYTE *stream, int len )
{
	EL6 *p6 = STATIC_CAST( EL6 *, userdata );	// 自分自身のオブジェクトポインタ取得
	
	// サウンドバッファ更新
	//  もしサンプル数が足りなければここで追加
	//  ただしビデオキャプチャ中は無視
	int addsam = len/sizeof(int16_t) - p6->snd->cRing::ReadySize();
	
	p6->snd->Update( stream, min( (int)(len/sizeof(int16_t)), p6->snd->cRing::ReadySize() ) );
	
	if( addsam > 0 && !p6->AVI6::IsAVI() ){
		p6->SoundUpdate( addsam );
		p6->snd->Update( stream, addsam );
	}
	
}


////////////////////////////////////////////////////////////////
// FPS表示タイマ コールバック関数
////////////////////////////////////////////////////////////////
//#PENDING FPSタイマはQtで実装
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
//#PENDING FPSタイマはQtで実装
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
	// リレーOFF待ち
	if( ak.Relay ){
		if( vm->cmtl->IsRelay() ) return 0;
		else                 ak.Relay = false;
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
			ak.Relay = true;
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
bool EL6::SetAutoKey( char *str, int num )
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
	
	ak.Num    = num;	// 残り文字数
	ak.Wait   = 60;		// 待ち回数カウンタ(初回は1sec)
	ak.Relay  = false;	// リレースイッチOFF待ちフラグ
	ak.Seek   = 0;		// 読込みポインタ
	
	return true;
}


////////////////////////////////////////////////////////////////
// 自動キー入力文字列設定(ファイルから)
//
// 引数:	filepath	入力ファイルへのパス
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::SetAutoKeyFile( char *filepath )
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
	
	ak.Num    = tsize;	// 残り文字数
	ak.Wait   = 60;		// 待ち回数カウンタ(初回は1sec)
	ak.Relay  = false;	// リレースイッチOFF待ちフラグ
	ak.Seek   = 0;		// 読込みポインタ
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	filename	ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::DokoDemoSave( char *filename )
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
		
		if( !cfg->DokoSave( Ini ) ||
			!vm->evsc->DokoSave( Ini ) ||
			!vm->psg->DokoSave( Ini ) ||
			!vm->disk->DokoSave( Ini ) ||
			!vm->vdg->DokoSave( Ini ) ||
			!vm->intr->DokoSave( Ini ) ||
			!vm->key->DokoSave( Ini ) ||
			!vm->cpus->DokoSave( Ini ) ||
			!vm->pio->DokoSave( Ini ) ||
			!vm->cpum->DokoSave( Ini ) ||
			!vm->mem->DokoSave( Ini ) ||
			!vm->cmtl->DokoSave( Ini )
		) throw Error::GetError();
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
	Error::SetError( Error::NoError );
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	filename	ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::DokoDemoLoad( char *filename )
{
	cIni *Ini = NULL;
	
	// とりあえずエラー設定
	Error::SetError( Error::DokoReadFailed );
	try{
		// どこでもLOADファイルを開く
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::DokoReadFailed;
		cfg->SetDokoFile( filename );
		
		// まずはPC6001Vのバージョンを確認
		if( !cfg->DokoLoad( Ini ) ) throw Error::GetError();
		
		vm->Reset();
		
		if(	!vm->evsc->DokoLoad( Ini ) ||
			!vm->psg->DokoLoad( Ini ) ||
			!vm->disk->DokoLoad( Ini ) ||
			!vm->vdg->DokoLoad( Ini ) ||
			!vm->intr->DokoLoad( Ini ) ||
			!vm->key->DokoLoad( Ini ) ||
			!vm->cpus->DokoLoad( Ini ) ||
			!vm->pio->DokoLoad( Ini ) ||
			!vm->cpum->DokoLoad( Ini ) ||
			!vm->mem->DokoLoad( Ini ) ||
			!vm->cmtl->DokoLoad( Ini )
		) throw Error::GetError();
		
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
	cfg->SetDokoFile( "" );
	
	// 無事だったのでエラーなし
	Error::SetError( Error::NoError );
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOADファイルから機種名読込
//
// 引数:	filename	ファイル名
// 返値:	int			機種名(60,62,66)
////////////////////////////////////////////////////////////////
int EL6::GetDokoModel( char *filename )
{
	cIni *Ini = NULL;
	int st;
	
	try{
		// どこでもLOADファイルを開く
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::DokoReadFailed;
		
		// 機種取得
		Ini->GetInt( "GLOBAL", "P6Model",	&st, 0 );
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
bool EL6::TapeMount( char *filename )
{
	if( !vm->cmtl->Mount( filename ) ){
		staw->TapeUnmount();
		return false;
	}
	
	staw->TapeMount( *vm->cmtl->GetName() ? (char *)vm->cmtl->GetName() : (char *)OSD_GetFileNamePart( filename ),
					  vm->cmtl->IsAutoStart(), vm->cmtl->GetSize() );
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
	staw->TapeUnmount();
}


////////////////////////////////////////////////////////////////
// DISK マウント
//
// 引数:	drv			ドライブ番号
//			filename	ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::DiskMount( int drv, char *filename )
{
	if( !vm->disk->Mount( drv, filename ) ){
		staw->DiskUnmount( drv );
		return false;
	}
	
	staw->DiskMount( drv, *vm->disk->GetName( drv ) ? (char *)vm->disk->GetName( drv ) : (char *)OSD_GetFileNamePart( filename ),
					  vm->disk->IsSystem( drv ), vm->disk->IsProtect( drv ) );
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
	staw->DiskUnmount( drv );
}







////////////////////////////////////////////////////////////////
// リプレイ保存開始
//
// 引数:	filename	ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EL6::ReplayStartRec( char *filename )
{
	return REPLAY::StartRecord( filename );
}


////////////////////////////////////////////////////////////////
// リプレイ保存停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::ReplayStopRec( void )
{
	REPLAY::StopRecord();
}


////////////////////////////////////////////////////////////////
// リプレイ再生開始
//
// 引数:	filename	ファイル名
// 返値:	なし
////////////////////////////////////////////////////////////////
void EL6::ReplayStartPlay( char *filename )
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
void EL6::ReplayStopPlay( void )
{
	REPLAY::StopReplay();
}

