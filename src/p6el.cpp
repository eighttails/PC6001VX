/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <cctype>
#include <new>
#include <string>
#include <utility>

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
#include "log.h"
#include "memory.h"
#include "osd.h"
#include "p6el.h"
#include "pio.h"
#include "psgfm.h"
#include "schedule.h"
#include "sound.h"
#include "status.h"
#include "tape.h"
#include "vdg.h"
#include "voice.h"
#include "vsurface.h"


#define	FRAMERATE					((double)VSYNC_HZ/(double)(cfg->GetValue( CV_FrameSkip )+1.0))

// リプレイ中どこでもSAVEの履歴数
#define	REPLAY_DOKOSAVE_HISTORY		5


int EL6::Speed = 100;



/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
EL6::EL6( void ) : cfg( nullptr ), vm( nullptr ), sche( nullptr ), snd( nullptr ),
	joy( nullptr ), graph( nullptr ), staw( nullptr ),
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	regw( nullptr ), memw( nullptr ), monw( nullptr ),
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	UpDateFPSID( 0 ), FSkipCount( 0 ), MMotion( true ),
	TapePathUI( "" ), DiskPathUI( "" ), ExRomPathUI( "" ), DokoPathUI( "" )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
EL6::~EL6( void )
{
}



/////////////////////////////////////////////////////////////////////////////
// スレッド関数(オーバーライド)
/////////////////////////////////////////////////////////////////////////////
void EL6::OnThread( void* inst )
{
	EL6* p6 = STATIC_CAST( EL6*, inst );	// 自分自身のオブジェクトポインタ取得
	
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( p6->vm->IsMonitor() ){
	// モニタモード
		while( !this->cThread::IsCancel() ){
			// 画面更新
			if( p6->ScreenUpdate() ){
				OSD_PushEvent( EV_RENDER );
			}
			
			// ウェイト
			p6->Wait();
		}
	}else
	// 通常モード
		if( p6->vm->bp->GetNum() ){
			// ブレークポイントあり
			while( !this->cThread::IsCancel() ){
				// ポーズ中なら画面更新のみ
				if( p6->sche->GetPauseEnable() ){
					// 画面更新
					if( p6->ScreenUpdate() ){
						OSD_PushEvent( EV_RENDER );
					}
					
					// ウェイト
					p6->Wait();
				}else{
					int st = p6->Emu();		// 1命令実行
					
					// ブレークポイントチェック(バスリクエスト期間中はチェックしない)
					if( st > 0 && ( p6->vm->bp->Check( BPoint::BP_PC,   p6->vm->cpum->GetPC() ) ||
									p6->vm->bp->Check( BPoint::BP_INTR, (WORD)p6->vm->cpum->GetIntVec() ) ||
									p6->vm->bp->GetReqNum() ) ){
						OSD_PushEvent( EV_DEBUGMODEBP, p6->vm->cpum->GetPC() );
						break;	// ブレーク条件にヒットしたらスレッド抜ける
					}
					
					if( p6->vm->evsc->IsVSYNC() ){
						p6->vm->key->ScanMatrix();	// キーマトリクススキャン
						
						// サウンド更新
						p6->SoundUpdate( 0 );
						// 画面更新
						if( p6->ScreenUpdate() ){
							OSD_PushEvent( EV_RENDER );
						}
						
						// 自動キー入力
						if( IsAutoKey() ){
							BYTE key = GetAutoKey();
							if( key ){
								if( key == 0x14 ){ p6->vm->cpus->ReqKeyIntr( 6, GetAutoKey() ); }
								else			 { p6->vm->cpus->ReqKeyIntr( 0, key ); }
							}
						}
						
						// ウェイト
						p6->Wait();
					}
				}
			}
		}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{	// 通常実行
		while( !this->cThread::IsCancel() ){
			// ポーズ中なら画面更新のみ
			if( p6->sche->GetPauseEnable() ){
				// 画面更新
				if( p6->ScreenUpdate() ){
					OSD_PushEvent( EV_RENDER );
				}
			// ビデオキャプチャ フレーム出力が終わるまで空回り
			}else if( !AVI6::GetRequest() ){
				// キーマトリクススキャン
				bool matchg = p6->vm->key->ScanMatrix();
				
				// リプレイ記録中
				if( REPLAY::GetStatus() == ST_REPLAYREC ){
					REPLAY::ReplayWriteFrame( p6->vm->key->GetMatrix2(), matchg );
				}
				
				// リプレイ再生中
				if( REPLAY::GetStatus() == ST_REPLAYPLAY ){
					REPLAY::ReplayReadFrame( p6->vm->key->GetMatrix() );
					// リプレイ終了時にビデオキャプチャ中だったらキャプチャを停止する
					if( REPLAY::GetStatus() == ST_IDLE && AVI6::IsAVI() ){
						UI_AVISaveStop();
					}
				}
				
				p6->EmuVSYNC();			// 1画面分実行
				
				// ビデオキャプチャ中?
				if( AVI6::IsAVI() ){
					// ビデオキャプチャ中ならここでAVI保存処理
					// サウンド更新
					p6->SoundUpdate( 0, AVI6::GetAudioBuffer() );
					// 画面更新されたら AVI1画面保存
					if( p6->ScreenUpdate() ){
						// サンプル数が足りなければ更にサウンド更新
						DWORD sam = AVI6::GetUpdateSample();
						if( sam ){
							p6->SoundUpdate( sam, AVI6::GetAudioBuffer() );
						}
						// 画面キャプチャ処理はSDLと同じメインスレッドで実施
						AVI6::Request();
						OSD_PushEvent( EV_CAPTURE );
					}
				}else{
					// ビデオキャプチャ中でないなら通常の更新
					// サウンド更新
					p6->SoundUpdate( 0 );
					// 画面更新
					if( p6->ScreenUpdate() ){
						// 画面更新処理はSDLと同じメインスレッドで実施
						OSD_PushEvent( EV_RENDER );
					}
				}
				
				// 自動キー入力
				if( IsAutoKey() ){
					BYTE key = GetAutoKey();
					if( key ){
						if( key == 0x14 ){ p6->vm->cpus->ReqKeyIntr( 6, GetAutoKey() ); }
						else			 { p6->vm->cpus->ReqKeyIntr( 0, key ); }
					}
				}
			}
			
			// ウェイト
			p6->Wait();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// Wait
/////////////////////////////////////////////////////////////////////////////
void EL6::Wait( void )
{
	if( sche->GetWaitEnable() && (!cfg->GetValue( CB_TurboTAPE ) || (vm->cpus->GetCmtStatus() == SUB6::CMTCLOSE)) )
		sche->VWait();
	vm->evsc->ReVSYNC();
}


/////////////////////////////////////////////////////////////////////////////
// 1命令実行
/////////////////////////////////////////////////////////////////////////////
int EL6::Emu( void )
{
	int st = vm->Emu();				// VM 1命令実行
	int ste = st <= 0 ? 1 : st;
	vm->evsc->Update( ste );		// イベント更新
	sche->Update( ste );
	
	return st;
}


/////////////////////////////////////////////////////////////////////////////
// 1画面分実行
/////////////////////////////////////////////////////////////////////////////
int EL6::EmuVSYNC( void )
{
	int state = 0;
	
	// VSYNCが発生するまで繰返し
	while( !vm->evsc->IsVSYNC() ){
		int st = vm->Emu();		// VM 1命令実行
		if( st <= 0 ) st = 1;
		vm->evsc->Update( st );	// イベント更新
		sche->Update( st );
		state += st;
	}
	
	return state;
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// 指定ステート数実行
/////////////////////////////////////////////////////////////////////////////
void EL6::Exec( int st )
{
	int State = st;
	
	while( State > 0 ) State -= Emu();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@




/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool EL6::Init( const std::shared_ptr<CFG6>& config )
{
	// エラーなし
	Error::Clear();
	
	// 念の為
	StopFPSTimer();
	ak.Buffer.clear();
	
	if( !config ) return false;
	cfg = config;
	
	// パレット設定
	SetPalette();
	
	try{
		// 機種別 VM確保
		switch( cfg->GetValue( CV_Model ) ){
		case 61: vm = std::make_unique<VM61>();	break;
		case 62: vm = std::make_unique<VM62>();	break;
		case 66: vm = std::make_unique<VM66>();	break;
		case 64: vm = std::make_unique<VM64>();	break;
		case 68: vm = std::make_unique<VM68>();	break;
		default: vm = std::make_unique<VM60>();
		}
		
		// 各種オブジェクト確保
		sche  = std::make_unique<SCH6>();			// スケジューラ
		snd   = std::make_unique<SND6>();			// サウンド
		joy   = std::make_unique<JOY6>();			// ジョイスティック
		graph = std::make_unique<DSP6>( this );		// 画面描画
		staw  = std::make_unique<cWndStat>();		// ステータスバー
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		regw  = std::make_unique<cWndReg>( this );	// レジスタウィンドウ
		memw  = std::make_unique<cWndMem>( this );	// メモリウィンドウ
		monw  = std::make_unique<cWndMon>( this );	// モニタウィンドウ
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		
		
		// VM初期化
		if( !vm->Init( cfg ) ) throw Error::GetError();
		
		// スケジューラ -----
		sche->SetMasterClock( vm->evsc->GetMasterClock() );
		
		// サウンド -----
		if( !snd->Init( this, EL6::StreamUpdate, cfg->GetValue( CV_SampleRate ), cfg->GetValue( CV_SoundBuffer ) ) ) throw Error::GetError();
		snd->SetVolume( cfg->GetValue( CV_MasterVol ) );
		
		// 画面描画 -----
		if( !graph->Init() ) throw Error::GetError();
		graph->SetIcon( cfg->GetValue( CV_Model ) );	// アイコン設定
		
		// ジョイスティック -----
		if( !joy->Init() ) throw Error::GetError();
		
		// ステータスバー -----
		if( !staw->Init( graph->ScreenX(), cfg->GetValue( CV_FDDrive ), cfg->GetValue( CV_ExCartridge ) ) ) throw Error::GetError();
		
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		// レジスタウィンドウ　-----
		if( !regw->Init() ) throw Error::GetError();
		
		// メモリウィンドウ　-----
		if( !memw->Init() ) throw Error::GetError();
		
		// モニタウィンドウ　-----
		if( !monw->Init() ) throw Error::GetError();
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		
		// リプレイ -----
		if( !REPLAY::Init() ) throw Error::GetError();
		
		// スクリーンサイズ変更
		graph->ResizeScreen();
		
		// ストリーム接続
		snd->ConnectStream( vm->psg );		// PSG/OPN
		snd->ConnectStream( vm->cmtl );		// CMT(LOAD)
		snd->ConnectStream( vm->voice );	// 音声合成
		
		
		// TAPE挿入
		if( !cfg->GetValue( CF_Tape ).empty() ) TapeMount( cfg->GetValue( CF_Tape ) );
		
		// ドライブ1,2にDISK挿入
		if( !cfg->GetValue( CF_Disk1 ).empty() ) DiskMount( 0, cfg->GetValue( CF_Disk1 ) );
		if( !cfg->GetValue( CF_Disk2 ).empty() ) DiskMount( 1, cfg->GetValue( CF_Disk2 ) );
		
		// リセット
		UI_Reset();
		
	}
	catch( std::bad_alloc& ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// 例外発生
		return false;
	}
	
	// エラーなし
//	Error::Clear();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 動作開始
/////////////////////////////////////////////////////////////////////////////
bool EL6::Start( void )
{
	// 実行速度を復元
	while( sche->GetSpeedRatio() != Speed ){
		sche->SetSpeedRatio( Speed > 100 ? 1 : -1 );
	}
	
	FSkipCount = 0;
	
	// スレッド生成
	if( !this->cThread::BeginThread( this ) ) return false;
	
	sche->Start();
	snd->Play();
	
	// FPS表示タイマ開始
	StartFPSTimer();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 動作停止
/////////////////////////////////////////////////////////////////////////////
void EL6::Stop( void )
{
	// FPS表示タイマ停止
	StopFPSTimer();
	
	// 実行速度を退避
	Speed = sche->GetSpeedRatio();
	
	if( !this->cThread::IsCancel() ){
		this->cThread::Cancel();	// スレッド終了フラグ立てる
		this->cThread::Waiting();	// スレッド終了まで待つ
	}
	snd->Pause();
	sche->Stop();
}


/////////////////////////////////////////////////////////////////////////////
// イベントループ
/////////////////////////////////////////////////////////////////////////////
EL6::ReturnCode EL6::EventLoop( ReturnCode rc )
{
	Event event;
	std::string str;
	
//	Event lastkey;
//	lastkey.key.sym   = KVC_UNKNOWN;
//	lastkey.key.state = false;
	
	
	switch( rc ){
	case ReplayPlay:	// リプレイ再生
		REPLAY::StartReplay( cfg->GetDokoFile() );
		break;
		
	case ReplayResume:	// リプレイ保存再開
		{
			P6VPATH fpath = cfg->GetDokoFile();
			cIni save;
			int frame = 0;
			
			save.Read( fpath );
			save.GetVal( "REPLAY", "frame", frame );
			OSD_ChangeFileNameExt( fpath, EXT_REPLAY );	// 拡張子を差替え
			REPLAY::ResumeRecord( fpath, frame );
		}
		break;
		
	case ReplayMovie:	// リプレイを動画に変換
		UI_AVISaveStart();
		REPLAY::StartReplay( cfg->GetDokoFile() );
		break;
		
	default:
		;
	}
	cfg->SetDokoFile( "" );
	
	
	// イベントキュークリア
	OSD_FlushEvents();
	
	// タイトルバーをすぐに表示するためにイベントを投げる
	OSD_PushEvent( EV_FPSUPDATE );
	
	// ResizeScreen()でリサイズしたかチェック 空読みしてリセット
	graph->CheckResize();
	
	Start();	// VM開始
	
	while( OSD_GetEvent( &event ) ){
		switch( event.type ){
		case EV_FPSUPDATE:		// FPS表示
			str = cfg->GetCaption();
			if( sche->GetPauseEnable() )
				str += " === PAUSE ===";
			else{
				str += Stringf( " (%4d%%  %5.2f/%5.2f fps)", sche->GetRatio(), sche->GetFPS(), FRAMERATE );
				if( sche->GetSpeedRatio() != 100 )
					str += Stringf( " [x%3.1f]", (double)sche->GetSpeedRatio()/100 );
			}
			OSD_SetWindowCaption( GetWindowHandle(), str );
			
			// フルスクリーン時にマウスを一定時間動かさなかったらカーソルを消す
			if( cfg->GetValue( CB_FullScreen ) ){
				if( MMotion ){ MMotion = false; }
				else         { OSD_ShowCursor( false ); }
			}
			break;
			
		case EV_KEYDOWN:		// キー入力
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			// モニタモード?
			if( vm->IsMonitor() ){
				CheckMonKey( event.key.sym, event.key.unicode, event.key.mod & KVM_SHIFT ? true : false );
				break;
			}
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			// 各種機能キーチェック
			if( CheckFuncKey( event.key.sym, event.key.mod & KVM_ALT ? true : false ) )
				break;
			
			// リプレイ再生中 or 自動キー入力実行中でなければ
			if( REPLAY::GetStatus() != ST_REPLAYPLAY && !IsAutoKey() ){
				// キーリピート無効化実験
//				if( event.key.sym != lastkey.key.sym || lastkey.key.state == false ){
					// キーマトリクス更新(キー)
					vm->key->UpdateMatrixKey( event.key.sym, event.key.state );
//				}
			}
//			lastkey = event;
			break;
			
		case EV_KEYUP:
			// リプレイ再生中 or 自動キー入力実行中でなければ
			if( REPLAY::GetStatus() != ST_REPLAYPLAY && !IsAutoKey() ){
				// キーマトリクス更新(キー)
				vm->key->UpdateMatrixKey( event.key.sym, event.key.state );
			}
			
			// キーリピート無効化実験
//			if( event.key.sym == lastkey.key.sym ){
//				lastkey = event;
//			}
			break;
			
		case EV_JOYDEVICEADDED:
		case EV_JOYDEVICEREMOVED:
			break;
			
		case EV_JOYAXISMOTION:
		case EV_JOYBUTTONDOWN:
		case EV_JOYBUTTONUP:
			// リプレイ再生中 or 自動キー入力実行中でなければ
			if( REPLAY::GetStatus() != ST_REPLAYPLAY && !IsAutoKey() )
				// キーマトリクス更新(ジョイスティック)
				vm->key->UpdateMatrixJoy( joy->GetJoyState( 0 ), joy->GetJoyState( 1 ) );
			break;
			
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		case EV_DEBUGMODEBP:		// モニタモード変更(ブレークポイント到達時)
			monw->BreakIn( event.bp.addr );		// ブレークポイントの情報を表示
			[[fallthrough]];
			
		case EV_DEBUGMODETOGGLE:	// モニタモード変更(モニタモードから通常モードへの復帰)
			Stop();
			UI_Monitor();
			Start();
			break;
			
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		case EV_MOUSEMOTION:	// マウスカーソル動いた
			if( cfg->GetValue( CB_FullScreen ) ){
				OSD_ShowCursor( true );
				MMotion = true;
			}
			break;
			
		case EV_MOUSEBUTTONUP:	// マウスボタンクリック(離した時)
			switch( event.mousebt.button ){
			case MBT_LEFT:		// 等速
				sche->SetSpeedRatio( 0 );
				break;
				
			case MBT_MIDDLE:
				break;
				
			case MBT_RIGHT:		// ポップアップメニュー表示
				Stop();
				ShowPopupMenu( event.mousebt.x, event.mousebt.y );
				Start();
				break;
				
			default:
				break;
			}
			break;
			
		case EV_MOUSEWHEEL:		// マウスホイール
			if( event.mousewh.y > 0 ){	// スピードアップ
				sche->SetSpeedRatio( 1 );
			}
			if( event.mousewh.y < 0 ){	// スピードダウン
				sche->SetSpeedRatio( -1 );
			}
			break;
			
		case EV_RENDER:				// 画面描画
			graph->DrawScreen();
			break;
			
		case EV_CAPTURE:			// ビデオキャプチャ
			graph->DrawScreen();
			AVI6::AVIWriteFrame( GetWindowHandle() );
			break;
			
//		case EV_WINDOWRESIZED:		// ウィンドウサイズ変更
		case EV_WINDOWSIZECHANGED:	// ウィンドウサイズ変更
			{
				// ResizeScreen()でリサイズしたなら何もしないで戻る
				if( graph->CheckResize() ) break;
				
				// 変化率が大きい方の軸のサイズを優先
				double rx = (double) event.window.w                   / (double)graph->ScreenX();
				double ry = (double)(event.window.h - staw->Height()) / (double)graph->ScreenY();
				
				// 倍率を逆算
				int zoom = cfg->GetValue( CV_WindowZoom ) * (((rx < 1 ? 1/rx : rx) > (ry < 1 ? 1/ry : ry)) ? rx : ry);
				
				Stop();
				cfg->SetValue( CV_WindowZoom, zoom );	// ウィンドウサイズに合わせて倍率再設定
				graph->ResizeScreen();		// スクリーンサイズ変更
				Start();
			}
			break;
			
		case EV_WINDOWEVENT_RESTORED:
			Stop();
			graph->ResizeScreen();		// スクリーンサイズ変更
			Start();
			break;
			
		case EV_DROPFILE:		// Drag & Drop
			{
				P6VPATH fpath = STR2P6VPATH( event.drop.file );
				// ファイル名を開放
				delete [] event.drop.file;
				
				// 拡張子取得(小文字)
				std::string ext = OSD_GetFileNameExt( fpath );
				std::transform( ext.begin(), ext.end(), ext.begin(), ::tolower );
				
				if( ext == EXT_P6RAW || ext == EXT_CAS || ext == EXT_P6T ){
					UI_TapeInsert( fpath );
				}else if( ext == EXT_DISK ){
					UI_DiskInsert( 0, fpath );
				}else if( ext == EXT_ROM1 || ext == EXT_ROM2 ){
					UI_CartInsert( EXC6005, fpath );
				}else if( ext == EXT_DOKO ){
					UI_DokoLoad( fpath );
				}else if( ext == EXT_REPLAY ){
					UI_ReplayPlay( fpath );
				}else if( ext == EXT_BASIC || ext == EXT_TEXT ){
					UI_AutoType( fpath );
				}
			}
			break;
			
		case EV_QUIT:			// 終了
			// もし未処理のEV_CAPTUREが残っていたら1周待ってからEV_QUIT
			if( OSD_HasEvent( EV_CAPTURE ) ){
				Stop();			// 新たなイベントを発生させないためにVM停止
				OSD_PushEvent( EV_QUIT );
				break;
			}
			
			if( cfg->GetValue( CB_CkQuit ) && OSD_Message( GetWindowHandle(), GetText( T_QUIT ), GetText( T_QUITC ), OSDM_YESNO | OSDM_ICONQUESTION ) != OSDR_YES ){
				break;
			}
			
			// ビデオキャプチャ中なら停止
			UI_AVISaveStop();
			
			Stop();	// VMを停止してから抜ける
			return Quit;
			
		case EV_RESTART:		// 再起動
			Stop();	// VMを停止してから抜ける
			return Restart;
			
		case EV_DOKOLOAD:		// どこでもLOAD
			Stop();	// VMを停止してから抜ける
			return Dokoload;
			
		case EV_REPLAYPLAY:		// リプレイ再生
			Stop();	// VMを停止してから抜ける
			return ReplayPlay;
			
		case EV_REPLAYRESUME:	// リプレイ保存再開
			Stop();	// VMを停止してから抜ける
			return ReplayResume;
			
		case EV_REPLAYMOVIE:	// リプレイを動画に変換
			Stop();	// VMを停止してから抜ける
			return ReplayMovie;
			
		default:
			break;
		}
		
		// エラー処理
		switch( Error::GetError() ){
		case Error::NoError:
			break;
			
		default:
			OSD_Message( GetWindowHandle(), Error::GetErrorText(), GetText( TERR_ERROR ), OSDR_OK | OSDM_ICONERROR );
			Error::Clear();
		}
	}
	Stop();	// VMを停止してから抜ける
	return Quit;
}


/////////////////////////////////////////////////////////////////////////////
// 各種機能キーチェック
/////////////////////////////////////////////////////////////////////////////
bool EL6::CheckFuncKey( int kcode, bool OnALT )
{
	switch( kcode ){	// キーコード
	case KVC_F6:		// モニタモード変更 or スクリーンモード変更
		if( OnALT ){
			Stop();
			UI_FullScreen();
			Start();
		}else{
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			Stop();
			UI_Monitor();
			Start();
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		}
		break;
		
	case KVC_F7:			// スキャンラインモード変更
		if( OnALT ){
			Stop();
			UI_Disp43();
			Start();
		}else{
			UI_ScanLine();
		}
		break;
		
	case KVC_F8:			// モード4カラー変更 or ステータスバー表示状態変更
		if( OnALT ){
			Stop();
			UI_StatusBar();
			Start();
		}else{
			UI_Mode4Color( cfg->GetValue( CV_Mode4Color ) + 1 );
		}
		break;
		
	case KVC_F9:			// ポーズ変更 or どこでもSAVE(スロット使用)
		if( OnALT ){
			Stop();
			UI_DokoSave( 1 );
			Start();
		}else{
			UI_Pause();
		}
		break;
		
	case KVC_F10:			// Wait変更 or どこでもLOAD(スロット使用)
		if( OnALT ){
			Stop();
			UI_DokoLoad( 1, true );
			Start();
		}else{
			UI_NoWait();
		}
		break;
		
	case KVC_F11:			// リセット or 再起動
		if( OnALT ){
			UI_Restart();
		}else{
			UI_Reset();
		}
		break;
		
	case KVC_F12:			// スナップショット
		if( OnALT ){
		}else{
			Stop();
			UI_SnapShot();
			Start();
		}
		break;
		
	case KVX_MENU:			// ポップアップメニュー表示
		Stop();
		ShowPopupMenu( 0, 0 );
		Start();
		break;
		
	case KVC_MUHENKAN:		// どこでもSAVE(スロット使用)
		Stop();
		UI_DokoSave( 1 );
		Start();
		break;
		
	case KVC_HENKAN:		// どこでもLOAD(スロット使用)
		Stop();
		UI_DokoLoad( 1, true );
		Start();
		break;
		
	default:				// どれでもない
		return false;
	}
	return true;
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// モニタモードキーチェック
/////////////////////////////////////////////////////////////////////////////
void EL6::CheckMonKey( int kcode, int ccode, bool OnSHIFT )
{
	switch( kcode ){		// キーコード
	case KVC_F6:			// モニタモード変更(モニタモードから通常モードへの復帰)
		Stop();
		UI_Monitor();
		Start();
		break;
		
	// メモリウィンドウ
	case KVC_PAGEDOWN:		// PageDown
		memw->SetAddress( memw->GetAddress() + ( OnSHIFT ? 2048 : 16 ) );
		break;
		
	case KVC_PAGEUP:		// PageUp
		memw->SetAddress( memw->GetAddress() - ( OnSHIFT ? 2048 : 16 ) );
		break;
		
	default:
		monw->KeyIn( kcode, ccode );
	}
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


/////////////////////////////////////////////////////////////////////////////
// 画面更新
/////////////////////////////////////////////////////////////////////////////
bool EL6::ScreenUpdate( void )
{
	// 画面更新時期を迎えた?(ビデオキャプチャ中は無視)
	if( !AVI6::IsAVI() && !sche->IsScreenUpdate() ){ return false; }
	
	// フレームスキップチェック
	if( FSkipCount++ < cfg->GetValue( CV_FrameSkip ) ){ return false; }
	
	
	// ここではバックバッファの更新のみ
	// 実際に画面に反映するには「メインスレッドから」graph->DrawScreen()を呼ぶ
	// (SDLの制約による。この手のフレームワークでは大体同じ制約があるらしい。)
	
	// バックバッファ更新
	vm->vdg->UpdateBackBuf();
	
	// ステータスバー更新
	staw->Update( this );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// モニタモード画面更新
	if( vm->IsMonitor() ){
		regw->Update();
		memw->Update();
		monw->Update();
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// FPSカウントアップ
	sche->FPSUpdate();
	
	FSkipCount = 0;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// サウンド更新
//
// 引数:	samples		更新するサンプル数(0:処理クロック分)
//			exbuf		外部バッファ
// 返値:	int			更新したサンプル数
/////////////////////////////////////////////////////////////////////////////
int EL6::SoundUpdate( int samples, cRing* exbuf )
{
	// PSG更新
	// PSGは常に発音されているためこれをサンプル数の基準とする)
	int size = vm->psg->SoundUpdate( samples );
	
	// CMT(LOAD)更新
	vm->cmtl->SoundUpdate( size );
	
	// 音声合成更新
	vm->voice->SoundUpdate( size );
	
	// サウンドバッファ更新
	return snd->PreUpdate( size, exbuf );
}


/////////////////////////////////////////////////////////////////////////////
// ストリーム更新 コールバック関数
//
// 引数:	userdata	コールバック関数に渡すデータ(自分自身へのオブジェクトポインタ)
//			stream		ストリーム書込みバッファへのポインタ
//			len			バッファサイズ(バイト単位)
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::StreamUpdate( void* userdata, BYTE* stream, int len )
{
	EL6* p6 = STATIC_CAST( EL6*, userdata );	// 自分自身のオブジェクトポインタ取得
	
	// サウンドバッファ更新
	//  もしサンプル数が足りなければここで追加
	//  ただしビデオキャプチャ中,ポーズ中,モニタモードの場合は無視
	int addsam = len/sizeof(int16_t) - p6->snd->cRing::ReadySize();
	
	if( addsam > 0 && !p6->AVI6::IsAVI() && !p6->sche->GetPauseEnable()
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		&& !p6->vm->IsMonitor()
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
																		){
		p6->SoundUpdate( addsam );
	}
	p6->snd->Update( stream, len/sizeof(int16_t) );
}


/////////////////////////////////////////////////////////////////////////////
// FPS表示タイマ コールバック関数
/////////////////////////////////////////////////////////////////////////////
DWORD EL6::UpDateFPS( DWORD interval, void* obj )
{
	// obj未使用
	
	OSD_PushEvent( EV_FPSUPDATE );
	
	return interval;
}


/////////////////////////////////////////////////////////////////////////////
// FPS表示タイマ開始
/////////////////////////////////////////////////////////////////////////////
bool EL6::StartFPSTimer( void )
{
	// タイマ稼動中なら一旦停止
	StopFPSTimer();
	
	// タイマ設定
	UpDateFPSID = OSD_AddTimer( 1000, EL6::UpDateFPS, nullptr );
	
	return UpDateFPSID ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// FPS表示タイマ停止
/////////////////////////////////////////////////////////////////////////////
void EL6::StopFPSTimer( void )
{
	if( UpDateFPSID ){
		OSD_DelTimer( UpDateFPSID );
		UpDateFPSID = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 自動キー入力実行中?
//
// 引数:	なし
// 返値:	bool	true:実行中 false:実行中でない
/////////////////////////////////////////////////////////////////////////////
bool EL6::IsAutoKey( void )
{
	return !ak.Buffer.empty();
}


/////////////////////////////////////////////////////////////////////////////
// 自動キー入力1文字取得
//   (VSYNC=1/60sec毎に呼ばれる)
//
// 引数:	なし
// 返値:	BYTE	P6のキーコード
/////////////////////////////////////////////////////////////////////////////
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
	
	
	// バッファが空なら終了
	if( ak.Buffer.empty() )
		return 0;
	
	// 次の文字を取得
	BYTE dat = ak.Buffer.front();
	ak.Buffer.erase( ak.Buffer.begin() );
	
	switch( dat ){
	case 0x17:	// '\w' ウェイト設定
		if( !ak.Buffer.empty() ){
			ak.Wait += (BYTE)ak.Buffer.front();	// 待ち回数設定
			ak.Buffer.erase( ak.Buffer.begin() );
		}
		return 0;
		
	case 0x0a:	// '\r' リレーOFF待ち
		ak.Relay   = true;
		ak.RelayOn = true;
		dat = 0x0d;
		[[fallthrough]];
		
	case 0x0d:	// '\n' 改行?
		ak.Wait = 9;	// 待ち9回(=150msec)
		break;
		
	default:	// 一般の文字
		ak.Wait = 0;	// 待ちなし
	}
	return dat;
}


/////////////////////////////////////////////////////////////////////////////
// 自動キー入力文字列設定
//
// 引数:	str		文字列への参照
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EL6::SetAutoKey( const std::string& str )
{
	ak.Buffer.clear();
	
	ak.Buffer  = str;
	ak.Wait    = 60;	// 待ち回数カウンタ(初回は1sec)
	ak.Relay   = false;	// リレースイッチOFF待ちフラグ
	ak.RelayOn = false;	// リレースイッチON待ちフラグ
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 自動キー入力文字列設定(ファイルから)
//
// 引数:	filepath	入力ファイルパス
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EL6::SetAutoKeyFile( const P6VPATH& filepath )
{
	std::fstream fs;
	char lbuf[1024];
	
	if( !OSD_FSopen( fs, filepath, std::ios_base::in ) ) return false;
	
	ak.Buffer.clear();
	
	// 文字列を読込み
	// データが無くなるまで繰り返し
	// 最初の1行読込む
	fs.getline( lbuf, sizeof(lbuf) );
	while( !fs.eof() ){
		Sjis2P6( ak.Buffer, lbuf );	// SJIS -> P6
		ak.Buffer += 0x0d;			// '\n'追加
		// 次の1行読込む
		fs.getline( lbuf, sizeof(lbuf) );
	}
	fs.close();
	
	ak.Wait    = 60;	// 待ち回数カウンタ(初回は1sec)
	ak.Relay   = false;	// リレースイッチOFF待ちフラグ
	ak.RelayOn = false;	// リレースイッチON待ちフラグ
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// オートスタート文字列設定
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::SetAutoStart( void )
{
	std::string kbuf;
	
	if( !(vm->cmtl->IsMount() && vm->cmtl->IsAutoStart()) ) return;
	
	const P6TAUTOINFO& ainf = vm->cmtl->GetAutoStartInfo();
	
	// キーバッファに書込み
	switch( cfg->GetValue( CV_Model ) ){
	case 60:	// PC-6001
	case 61:	// PC-6001A
		kbuf = Stringf( "%c%c", ainf.Page+'0', 0x0d );
		break;
		
	case 64:	// PC-6001mk2SR
		if( ainf.BASIC == 6 ){
			if( vm->disk->GetDrives() )	// ??? 実際は?
				kbuf = Stringf( "%c%c%c%c%c%c%c%c", 0x17, 50, ainf.BASIC+'0', 0x17, 20, 0x0d, 0x17, 10 );
			else
				kbuf = Stringf( "%c%c%c%c%c%c%c",   0x17, 10, ainf.BASIC+'0', 0x17, 20,       0x17, 10 );
			break;
		}
		[[fallthrough]];
		
	case 62:	// PC-6001mk2
		switch( ainf.BASIC ){
		case 3:
		case 4:
		case 5:
			if( vm->disk->GetDrives() )	// ??? 実際は?
				kbuf = Stringf( "%c%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf.BASIC+'0', 0x17, 30, 0x0d, ainf.Page+'0', 0x0d, 0x17, 120 );
			else
				kbuf = Stringf( "%c%c%c%c%c%c%c%c%c",   0x17, 50, ainf.BASIC+'0', 0x17, 30,       ainf.Page+'0', 0x0d, 0x17, 120 );
			break;
		default:
			kbuf = Stringf( "%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf.BASIC+'0', 0x17, 30, ainf.Page+'0', 0x0d, 0x17, 20 );
		}
		break;
		
	case 68:	// PC-6601SR
		if( ainf.BASIC == 6 ){
			if( vm->disk->IsMount( 0 ) )
				kbuf = Stringf( "%c%c%c%c%c%c%c%c%c%c%c", 0x17, 240, 0x17, 60, 0x14, 0xf4, 0x17, 30, 0x0d, 0x17, 10 );
			else
				kbuf = Stringf( "%c%c%c%c%c%c%c%c",   0x17, 240, 0x14, 0xf4, 0x17, 30,       0x17, 10 );
			break;
		}else{
			kbuf = Stringf( "%c%c%c%c%c%c%c%c", 0x17, 240, 0x17, vm->disk->IsMount( 0 ) ? 60 : 1, 0x17, vm->disk->IsMount( 1 ) ? 60 : 1, 0x14, 0xf3 );
		}
		[[fallthrough]];
		
	case 66:	// PC-6601
		switch( ainf.BASIC ){
		case 3:
		case 4:
		case 5:
			if( vm->disk->IsMount( 0 ) )
				kbuf += Stringf( "%c%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf.BASIC+'0', 0x17, 30, 0x0d, ainf.Page+'0', 0x0d, 0x17, 110 );
			else
				kbuf += Stringf( "%c%c%c%c%c%c%c%c%c",   0x17, 50, ainf.BASIC+'0', 0x17, 30,       ainf.Page+'0', 0x0d, 0x17, 110 );
			break;
		default:
			kbuf += Stringf( "%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf.BASIC+'0', 0x17, 30, ainf.Page+'0', 0x0d, 0x17, 10 );
		}
		break;
		
	}
	kbuf += ainf.ask.data();
	
	// 自動キー入力設定
	if( !kbuf.empty() ) SetAutoKey( kbuf );
}


/////////////////////////////////////////////////////////////////////////////
// パレット設定
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::SetPalette( void )
{
	if( !cfg ) return;
	
	for( int i=0; i<256; i++ )
		VSurface::SetColor( i, COL2DW( cfg->GetColor( i ) ) );
}


/////////////////////////////////////////////////////////////////////////////
// バックバッファ取得
//
// 引数:	なし
// 返値:	std::shared_ptr<VSurface>	バックバッファ
/////////////////////////////////////////////////////////////////////////////
std::shared_ptr<VSurface> EL6::GetBackBuffer( void )
{
	return vm->vdg;
}


/////////////////////////////////////////////////////////////////////////////
// 画面情報取得
//
// 引数:	なし
// 返値:	VDGInfo&	画面情報
/////////////////////////////////////////////////////////////////////////////
const VDGInfo& EL6::GetVideoInfo( void ) const
{
	return vm->vdg->GetVideoInfo();
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウハンドル取得
//
// 引数:	なし
// 返値:	HWINDOW		ウィンドウハンドル
/////////////////////////////////////////////////////////////////////////////
HWINDOW EL6::GetWindowHandle( void )
{
	return graph ? (HWINDOW)graph->GetWindowHandle() : nullptr;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	path		ファイルパスへの参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EL6::DokoDemoSave( const P6VPATH& path )
{
	PRINTD( VM_LOG, "[EL6][DokoDemoSave]\n" );
	
	cIni ini;
	
	// エラーをリセット
	Error::Clear();
	try{
		std::fstream fs;
		
		if( !OSD_FSopen( fs, path, std::ios_base::out ) ) throw Error::DokoWriteFailed;
		
		// タイトル行を出力して一旦閉じる
		fs << GetText( TDOK_TITLE ) << std::endl;
		fs.close();
		
		// どこでもSAVEファイルを開く
		if( !ini.Read( path ) ) throw Error::DokoWriteFailed;
		
		// 各オブジェクトのパラメータ書込み
		if( !cfg->DokoSave( &ini )      ||
			!vm->evsc->DokoSave( &ini ) ||
			!vm->intr->DokoSave( &ini ) ||
			!vm->cpum->DokoSave( &ini ) ||
			!vm->cpus->DokoSave( &ini ) ||
			!vm->mem->DokoSave( &ini )  ||
			!vm->vdg->DokoSave( &ini )  ||
			!vm->psg->DokoSave( &ini )  ||
			!vm->pio->DokoSave( &ini )  ||
			!vm->key->DokoSave( &ini )  ||
			!vm->cmtl->DokoSave( &ini ) ||
			!vm->disk->DokoSave( &ini ) ||
			!vm->voice->DokoSave( &ini )
		) throw Error::GetError();
		
		ini.SetVal( "KEY", "AK_Wait",		"", ak.Wait    );
		ini.SetVal( "KEY", "AK_Relay",		"", ak.Relay   );
		ini.SetVal( "KEY", "AK_RelayOn",	"", ak.RelayOn );
		
		std::string strva;
		int nn=0;
		
		for( size_t i=0; i<ak.Buffer.length(); i++ ){
			strva += Stringf( "%02X", ak.Buffer[i] );
			if( !((i+1)&63) ){
				ini.SetEntry( "KEY", Stringf( "AKBuf_%02X", nn++ ), "", strva.c_str() );
				strva.clear();
			}
		};
		if( !ak.Buffer.empty() )
			ini.SetEntry( "KEY", Stringf( "AKBuf_%02X", nn ), "", strva.c_str() );
		
		ini.Write();
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		return false;
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	path		ファイルパスへの参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EL6::DokoDemoLoad( const P6VPATH& path )
{
	PRINTD( VM_LOG, "[EL6][DokoDemoLoad]\n" );
	
	cIni ini;
	
	// エラーをリセット
	Error::Clear();
	try{
		// どこでもLOADファイルを開く
		if( !ini.Read( path ) ) throw Error::DokoReadFailed;
		
		// PC6001Vのバージョン確認と主要構成情報を読込み
		// (機種,FDドライブ数,拡張カートリッジ)
		if( !cfg->DokoLoad( &ini ) ) throw Error::GetError();
		
		// VM再初期化
		Init( cfg );
		
		// 各オブジェクトのパラメータ読込み
		if(	!vm->evsc->DokoLoad( &ini ) ||
			!vm->intr->DokoLoad( &ini ) ||
			!vm->cpum->DokoLoad( &ini ) ||
			!vm->cpus->DokoLoad( &ini ) ||
			!vm->mem->DokoLoad( &ini )  ||
			!vm->vdg->DokoLoad( &ini )  ||
			!vm->psg->DokoLoad( &ini )  ||
			!vm->pio->DokoLoad( &ini )  ||
			!vm->key->DokoLoad( &ini )  ||
			!vm->cmtl->DokoLoad( &ini ) ||
			!vm->disk->DokoLoad( &ini ) ||
			!vm->voice->DokoLoad( &ini )
		) throw Error::GetError();
		
		ini.GetVal( "KEY", "AK_Wait",		ak.Wait    );
		ini.GetVal( "KEY", "AK_Relay",		ak.Relay   );
		ini.GetVal( "KEY", "AK_RelayOn",	ak.RelayOn );
		
		std::string strva;
		int nn=0;
		
		ak.Buffer.clear();
		while( ini.GetEntry( "KEY", Stringf( "AKBuf_%02X", nn++ ), strva ) ){
			while( strva.length() >= 2 ){
				ak.Buffer += std::stoul( strva.substr( 0, 2 ), nullptr, 16 );
				strva.erase( strva.begin() );
				strva.erase( strva.begin() );
			}
		}
		
		// ディスクドライブ数によってステータスバーサイズ変更
		if( !staw->Init( -1, vm->disk->GetDrives() ) ) throw Error::GetError();
		if( !graph->ResizeScreen() ) throw Error::GetError();
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		return false;
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVEファイルのバージョンチェック
//
// 引数:	path		ファイルパスへの参照
// 返値:	bool		true:一致 false:不一致
/////////////////////////////////////////////////////////////////////////////
bool EL6::CheckDokoVer( const P6VPATH& path )
{
	cIni ini;
	std::string str;
	
	try{
		// どこでもLOADファイルを開く
		if( !ini.Read( path ) ) throw Error::DokoReadFailed;
		
		// バージョン取得
		ini.GetEntry( "GLOBAL", "Version", str );
		if( str != VERSION ) throw Error::DokoDiffVersion;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		return false;
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// TAPE マウント
//
// 引数:	path		ファイルパス
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EL6::TapeMount( const P6VPATH& path )
{
	if( !vm->cmtl->Mount( path ) ) return false;
	
	vm->cmtl->SetStopBit( cfg->GetValue( CV_StopBit ) );		// ストップビット数
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// TAPE アンマウント
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::TapeUnmount( void )
{
	vm->cmtl->Unmount();
}


/////////////////////////////////////////////////////////////////////////////
// DISK マウント
//
// 引数:	drv			ドライブ番号
//			path		ファイルパスへの参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EL6::DiskMount( int drv, const P6VPATH& path )
{
	if( !vm->disk->Mount( drv, path ) ) return false;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// DISK アンマウント
//
// 引数:	drv			ドライブ番号
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::DiskUnmount( int drv )
{
	vm->disk->Unmount( drv );
}






/////////////////////////////////////////////////////////////////////////////
// リプレイ保存開始
//
// 引数:	path		ファイルパス
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EL6::ReplayRecStart( const P6VPATH& path )
{
	// 途中セーブファイルを探して削除
	P6VPATH fpath = path;
	for( int i = 0; i < REPLAY_DOKOSAVE_HISTORY; i++ ){
		OSD_ChangeFileNameExt( fpath, EXT_RES + ( i == 0 ? "" : std::to_string( i ) ) );	// 拡張子を差替え
		// ファイルがあれば削除
		if( OSD_FileExist( fpath ) ){ OSD_FileDelete( fpath ); }
	}
	
	return REPLAY::StartRecord( path );
}


/////////////////////////////////////////////////////////////////////////////
// リプレイ保存再開
//
// 引数:	path		ファイルパスへの参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EL6::ReplayRecResume( const P6VPATH& path )
{
	// 途中セーブファイルを探す
	P6VPATH fpath = path;
	OSD_ChangeFileNameExt( fpath, EXT_RES );	// 拡張子を差替え
	
	if( !OSD_FileExist( fpath ) ){ return false; }
	
	cIni save;
	int frame = 0;
	
	save.Read( fpath );
	save.GetVal( "REPLAY", "frame", frame );
	if( frame == 0 ){ return false; }
	
	if( !UI_CheckDokoVer( fpath ) ){ return false; }
	
	cfg->SetDokoFile( fpath );
	OSD_PushEvent( EV_REPLAYRESUME );
	return true;
}




/////////////////////////////////////////////////////////////////////////////
// UI:TAPE 挿入
//
// 引数:	path		ファイルパスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_TapeInsert( const P6VPATH& path )
{
	P6VPATH fpath = path;
	
	if( fpath.empty() ){
		if( !OSD_FileExist( TapePathUI ) ){
			TapePathUI = cfg->GetValue( CF_TapePath );
		}
		if( !OSD_FileSelect( GetWindowHandle(), FD_TapeLoad, fpath, TapePathUI ) ){
			return;
		}
	}
	
	if( !TapeMount( fpath ) ){
		Error::SetError( Error::TapeMountFailed, P6VPATH2STR( fpath ) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// UI:DISK 挿入
//
// 引数:	drv			ドライブ番号
//			path		ファイルパスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_DiskInsert( int drv, const P6VPATH& path )
{
	P6VPATH fpath = path;
	
	if( fpath.empty() ){
		if( !OSD_FileExist( DiskPathUI ) ){
			DiskPathUI = cfg->GetValue( CF_DiskPath );
		}
		if( !OSD_FileSelect( GetWindowHandle(), FD_Disk, fpath, DiskPathUI ) ){
			return;
		}
	}
	
	if( !DiskMount( drv, fpath ) ){
		Error::SetError( Error::DiskMountFailed, P6VPATH2STR( fpath ) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// UI:拡張カートリッジ 挿入
//
// 引数:	cart		カートリッジタイプ
//			path		ファイルパスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_CartInsert( WORD cart, const P6VPATH& path )
{
	P6VPATH fpath = path;
	
	if( (cart & EXCROM) && fpath.empty() ){
		// ファイル固定でなければファイル選択
		if( !(cart & EXCFIX) ){
			if( !OSD_FileExist( ExRomPathUI ) ){
				ExRomPathUI = cfg->GetValue( CF_ExtRomPath );
			}
			if( !OSD_FileSelect( GetWindowHandle(), FD_ExtRom, fpath, ExRomPathUI ) ){
				// キャンセルしたらROMなしで起動する
				return;
			}
		}
	}
	
	cfg->SetValue( CV_ExCartridge, (int)cart );
	cfg->SetValue( CF_ExtRom, fpath );
	cfg->Write();
	
	// 再起動を伴うのでメッセージ表示
	OSD_Message( GetWindowHandle(), GetText( T_RESTARTI ), GetText( T_RESTARTC ), OSDM_OK | OSDM_ICONINFO );
	UI_Restart();
}


/////////////////////////////////////////////////////////////////////////////
// UI:拡張カートリッジ 挿入(ROMなし)
//
// 引数:	cart		カートリッジタイプ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_CartInsertNoRom( WORD cart )
{
	P6VPATH fpath = "";
	
	cfg->SetValue( CV_ExCartridge, (int)cart );
	cfg->SetValue( CF_ExtRom, fpath );
	cfg->Write();
	
	// 再起動を伴うのでメッセージ表示
	OSD_Message( GetWindowHandle(), GetText( T_RESTARTI ), GetText( T_RESTARTC ), OSDM_OK | OSDM_ICONINFO );
	UI_Restart();
}


/////////////////////////////////////////////////////////////////////////////
// UI:拡張カートリッジ 排出
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_CartEject( void )
{
	P6VPATH fpath = "";
	
	cfg->SetValue( CV_ExCartridge, 0 );
	cfg->SetValue( CF_ExtRom, fpath );
	cfg->Write();
	
	// 再起動を伴うのでメッセージ表示
	OSD_Message( GetWindowHandle(), GetText( T_RESTARTE ), GetText( T_RESTARTC ), OSDM_OK | OSDM_ICONINFO );
	UI_Restart();
}


/////////////////////////////////////////////////////////////////////////////
// UI:どこでもSAVEファイルのバージョンチェック
//
// 引数:	path		ファイルパスへの参照
// 返値:	bool		true:一致 false:不一致
/////////////////////////////////////////////////////////////////////////////
bool EL6::UI_CheckDokoVer( const P6VPATH& path )
{
	if( CheckDokoVer( path ) ){ return true; }
	
	if( Error::GetError() == Error::DokoDiffVersion ){
		int ret = OSD_Message( nullptr, Error::GetErrorText(), GetText( TERR_WARNING ), OSDM_YESNO | OSDM_ICONWARNING );
		Error::Clear();
		if( ret == OSDR_YES ){ return true; }
	}
	
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// UI:どこでもSAVE
//
// 引数:	path		ファイルパスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_DokoSave( const P6VPATH& path )
{
	P6VPATH fpath = path;
	
	if( fpath.empty() ){
		if( !OSD_FileExist( DokoPathUI ) ){
			DokoPathUI = cfg->GetValue( CF_DokoPath );
		}
		if( !OSD_FileSelect( GetWindowHandle(), FD_DokoSave, fpath, DokoPathUI ) ){
			return;
		}
	}
	
	if( !DokoDemoSave( fpath ) ){
		Error::SetError( Error::DokoWriteFailed );
	}
}


/////////////////////////////////////////////////////////////////////////////
// UI:どこでもLOAD
//
// 引数:	path		ファイルパスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_DokoLoad( const P6VPATH& path )
{
	P6VPATH fpath = path;
	
	if( fpath.empty() ){
		if( !OSD_FileExist( DokoPathUI ) ){
			DokoPathUI = cfg->GetValue( CF_DokoPath );
		}
		if( !OSD_FileSelect( GetWindowHandle(), FD_DokoLoad, fpath, DokoPathUI ) ){
			return;
		}
	}
	
	if( !UI_CheckDokoVer( fpath ) ){ return; }
	
	cfg->SetDokoFile( fpath );
	OSD_PushEvent( EV_DOKOLOAD );
}


/////////////////////////////////////////////////////////////////////////////
// UI:どこでもSAVE(スロット使用)
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_DokoSave( int slot )
{
	if( REPLAY::GetStatus() == ST_REPLAYREC ){
		UI_ReplayDokoSave();
	}else{
		P6VPATH fpath;
		OSD_AddPath( fpath, cfg->GetValue( CF_DokoPath ), Stringf( ".%d." EXT_DOKO, slot ) );
		DokoDemoSave( fpath );
		
		cIni save;
		if( save.Read( fpath ) ){
			// 一旦キー入力を無効化する(LOAD時にキーが押しっぱなしになるのを防ぐため)
			save.SetEntry( "KEY", "P6Matrix", "", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
			save.SetEntry( "KEY", "P6Mtrx",   "", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
			
			save.Write();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// UI:どこでもLOAD(スロット使用)
//
// 引数:	slot		スロット番号
//			ask			true:確認する false:確認しない
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_DokoLoad( int slot, bool ask )
{
	if( REPLAY::GetStatus() == ST_REPLAYREC ){
		P6VPATH fpath = REPLAY::cIni::GetFilePath();
		OSD_ChangeFileNameExt( fpath, EXT_RES );	// 拡張子を差替え
		if( OSD_FileExist( fpath ) ){
			if( !ask || OSD_Message( GetWindowHandle(), GetText( T_REPLAYRES ), GetText( T_DOKOC ), OSDM_YESNO | OSDM_ICONQUESTION ) == OSDR_YES ){
				UI_ReplayDokoLoad();
			}
		}
	}else{
		P6VPATH fpath;
		OSD_AddPath( fpath, cfg->GetValue( CF_DokoPath ), Stringf( ".%d." EXT_DOKO, slot ) );
		
		if( OSD_FileExist( fpath ) ){
			if( !ask || OSD_Message( GetWindowHandle(), GetText( T_DOKOSLOT ), GetText( T_DOKOC ), OSDM_YESNO | OSDM_ICONQUESTION ) == OSDR_YES ){
				cfg->SetDokoFile( fpath );
				OSD_PushEvent( EV_DOKOLOAD );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// UI:リプレイ保存/停止
//
// 引数:	path		ファイルパスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_ReplaySave( const P6VPATH& path )
{
	P6VPATH fpath = path;
	
	if( REPLAY::GetStatus() == ST_IDLE ){
		if( fpath.empty() ){
			if( !OSD_FileExist( DokoPathUI ) ){
				DokoPathUI = cfg->GetValue( CF_DokoPath );
			}
			if( !OSD_FileSelect( GetWindowHandle(), FD_RepSave, fpath, DokoPathUI ) ){
				return;
			}
		}
		
		if( !DokoDemoSave( fpath ) || !ReplayRecStart( fpath ) ){
			Error::SetError( Error::ReplayRecError );
		}
		
	}else if( REPLAY::GetStatus() == ST_REPLAYREC ){
		UI_ReplayDokoSave();
		REPLAY::StopRecord();
	}
}


/////////////////////////////////////////////////////////////////////////////
// UI:リプレイ再開
//
// 引数:	path		ファイルパスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_ReplayResumeSave( const P6VPATH& path )
{
	P6VPATH fpath = path;
	
	if( REPLAY::GetStatus() != ST_IDLE ){ return; }
	
	if( fpath.empty() ){
		if( !OSD_FileExist( DokoPathUI ) ){
			DokoPathUI = cfg->GetValue( CF_DokoPath );
		}
		if( !OSD_FileSelect( GetWindowHandle(), FD_RepSave, fpath, DokoPathUI ) ){
			return;
		}
	}
	
	if( !ReplayRecResume( fpath ) ){
		Error::SetError( Error::ReplayRecError );
	}
}


/////////////////////////////////////////////////////////////////////////////
// UI:リプレイ中どこでもSAVE
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_ReplayDokoSave( void )
{
	if( REPLAY::GetStatus() != ST_REPLAYREC ){ return; }
	
	// 途中セーブファイルを保存
	P6VPATH fpath = REPLAY::cIni::GetFilePath();
	P6VPATH bpath = fpath;
	P6VPATH apath = fpath;
	OSD_ChangeFileNameExt( fpath, EXT_RES );	// 拡張子を差替え
	
	// リプレイの途中保存ファイルの履歴を1つ進める
	for( int i = REPLAY_DOKOSAVE_HISTORY - 2; i >= 0; i-- ){
		OSD_ChangeFileNameExt( bpath, EXT_RES + ( i == 0 ? "" : std::to_string( i ) ) );
		OSD_ChangeFileNameExt( apath, EXT_RES + std::to_string( i + 1 ) );
		
		if( !OSD_FileExist( bpath ) ){ continue; }
		
		OSD_FileDelete( apath );
		OSD_FileRename( bpath, apath );
	}
	
	// 途中セーブ情報を追記
	cIni save;
	
	if( !DokoDemoSave( fpath ) || !save.Read( fpath ) ){
		Error::SetError( Error::ReplayRecError );
		return;
	}
	
	save.SetVal( "REPLAY", "frame", "", REPLAY::RepFrm );
	// 一旦キー入力を無効化する(LOAD時にキーが押しっぱなしになるのを防ぐため)
	save.SetEntry( "KEY", "P6Matrix", "", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
	save.SetEntry( "KEY", "P6Mtrx",   "", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
	
	if( !save.Write() ){
		Error::SetError( Error::ReplayRecError );
	}
}


/////////////////////////////////////////////////////////////////////////////
// UI:リプレイ中どこでもLOAD
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_ReplayDokoLoad( void )
{
	if( REPLAY::GetStatus() != ST_REPLAYREC ){ return; }
	
	P6VPATH fpath = REPLAY::cIni::GetFilePath();
	
	REPLAY::StopRecord();
	ReplayRecResume( fpath );
}


/////////////////////////////////////////////////////////////////////////////
// UI:リプレイ中どこでもLOADを巻き戻す
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_ReplayRollback( void )
{
	if( REPLAY::GetStatus() != ST_REPLAYREC ){ return; }
	
	P6VPATH fpath = REPLAY::cIni::GetFilePath();
	P6VPATH bpath = fpath;
	P6VPATH apath = fpath;
	OSD_ChangeFileNameExt( fpath, EXT_RES );	// 拡張子を差替え
	
	// リプレイの途中保存ファイルの履歴を1つ戻す
	for( int i = 0; i < REPLAY_DOKOSAVE_HISTORY; i++ ){
		OSD_ChangeFileNameExt( bpath, EXT_RES + std::to_string( i + 1 ) );
		OSD_ChangeFileNameExt( apath, EXT_RES + ( i == 0 ? "" : std::to_string( i ) ) );
		
		// これ以上履歴がなければ抜ける
		if( !OSD_FileExist( bpath ) ){ break; }
		
		OSD_FileDelete( apath );
		OSD_FileRename( bpath, apath );
	}
	
	REPLAY::StopRecord();
	ReplayRecResume( fpath );
}


/////////////////////////////////////////////////////////////////////////////
// UI:リプレイ再生/停止
//
// 引数:	path		ファイルパスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_ReplayPlay( const P6VPATH& path )
{
	P6VPATH fpath = path;
	
	if( REPLAY::GetStatus() == ST_IDLE ){
		if( fpath.empty() ){
			if( !OSD_FileExist( DokoPathUI ) ){
				DokoPathUI = cfg->GetValue( CF_DokoPath );
			}
			if( !OSD_FileSelect( GetWindowHandle(), FD_RepLoad, fpath, DokoPathUI ) ){
				return;
			}
		}
	}else if( REPLAY::GetStatus() == ST_REPLAYPLAY ){
		REPLAY::StopReplay();
		if( fpath.empty() ){ return; }
	}
	
	if( !UI_CheckDokoVer( fpath ) ){ return; }
	
	cfg->SetDokoFile( fpath );
	OSD_PushEvent( EV_REPLAYPLAY );
}


/////////////////////////////////////////////////////////////////////////////
// UI:リプレイを動画に変換
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_ReplayMovie( void )
{
	P6VPATH fpath;
	
	if( REPLAY::GetStatus() != ST_IDLE ){ return; }
	
	if( !OSD_FileExist( DokoPathUI ) ){
		DokoPathUI = cfg->GetValue( CF_DokoPath );
	}
	if( !OSD_FileSelect( GetWindowHandle(), FD_RepLoad, fpath, DokoPathUI ) ){
		return;
	}
	
	if( !UI_CheckDokoVer( fpath ) ){ return; }
	
	cfg->SetDokoFile( fpath );
	OSD_PushEvent( EV_REPLAYMOVIE );
}


/////////////////////////////////////////////////////////////////////////////
// UI:ビデオキャプチャ開始
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_AVISaveStart( void )
{
	P6VPATH fpath;
	P6VPATH mpath = OSD_GetConfigPath();
	
	if( !OSD_FileSelect( GetWindowHandle(), FD_AVISave, fpath, mpath ) ){
		return;
	}
	// ビデオキャプチャ用の画面設定に変更
	cfg->PushAviPara();		// 退避
	
	cfg->SetValue( CV_WindowZoom, cfg->GetValue( CV_AviZoom      ) );
	cfg->SetValue( CV_FrameSkip,  cfg->GetValue( CV_AviFrameSkip ) );
	cfg->SetValue( CB_ScanLine,   cfg->GetValue( CB_AviScanLine  ) );
	graph->ResizeScreen();	// スクリーンサイズ変更
	graph->DrawScreen();
	
	if( AVI6::StartAVI( fpath, graph->ScreenX(), graph->ScreenY(), FRAMERATE, cfg->GetValue( CV_SampleRate ), cfg->GetValue( CV_AviBpp ) ) ){
		return;
	}
	
	Error::SetError( Error::CaptureFailed );
	UI_AVISaveStop();
}


/////////////////////////////////////////////////////////////////////////////
// UI:ビデオキャプチャ停止
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_AVISaveStop( void )
{
	if( !AVI6::IsAVI() ){ return; }
	
	AVI6::StopAVI();
	
	// 通常画面設定に戻す
	cfg->PopAviPara();		// 復帰
	graph->ResizeScreen();	// スクリーンサイズ変更
	graph->DrawScreen();
}


/////////////////////////////////////////////////////////////////////////////
// UI:スナップショット
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_SnapShot( void )
{
	graph->SnapShot( cfg->GetValue( CF_ImgPath ) );
}


/////////////////////////////////////////////////////////////////////////////
// UI:打込み代行
//
// 引数:	path		ファイルパスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_AutoType( const P6VPATH& path )
{
	P6VPATH fpath = path;
	P6VPATH mpath = OSD_GetConfigPath();
	
	if( fpath.empty() ){
		if( !OSD_FileSelect( GetWindowHandle(), FD_LoadAll, fpath, mpath ) ){
			return;
		}
	}
	
	if( !SetAutoKeyFile( fpath ) ){
		Error::SetError( Error::Unknown );
	}
}


/////////////////////////////////////////////////////////////////////////////
// UI:リセット
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_Reset( void )
{
	bool can = this->cThread::IsCancel();	// スレッド停止済み?
	
	if( !can ){ Stop(); }	// スレッド動いてたら一旦止める
	
	// システムディスクが入っていたらTAPEのオートスタート無効
	if( !vm->disk->IsSystem(0) && !vm->disk->IsSystem(1) ){
		SetAutoStart();
	}
	
	vm->Reset();
	
	if( !can ){ Start(); }	// 元々スレッドが動いていたら再始動
}


/////////////////////////////////////////////////////////////////////////////
// UI:再起動
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_Restart( void )
{
	// ビデオキャプチャ中は無効
	if( AVI6::IsAVI() ){ return; }
	
	OSD_PushEvent( EV_RESTART );
}


/////////////////////////////////////////////////////////////////////////////
// UI:終了
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_Quit( void )
{
	OSD_PushEvent( EV_QUIT );
}


/////////////////////////////////////////////////////////////////////////////
// UI:Pause変更
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_Pause( void )
{
	sche->SetPauseEnable( !sche->GetPauseEnable() );
}


/////////////////////////////////////////////////////////////////////////////
// UI:Wait変更
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_NoWait( void )
{
	sche->SetWaitEnable( !sche->GetWaitEnable() );
}


/////////////////////////////////////////////////////////////////////////////
// UI:Turbo TAPE変更
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_TurboTape( void )
{
	cfg->SetValue( CB_TurboTAPE, !cfg->GetValue( CB_TurboTAPE ) );
}


/////////////////////////////////////////////////////////////////////////////
// UI:Boost Up変更
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_BoostUp( void )
{
	cfg->SetValue( CB_BoostUp, !cfg->GetValue( CB_BoostUp ) );
	vm->cmtl->SetBoost( !vm->cmtl->IsBoostUp() );
}


/////////////////////////////////////////////////////////////////////////////
// UI:フルスクリーン変更
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_FullScreen( void )
{
	// ビデオキャプチャ中は無効
	if( AVI6::IsAVI() ){ return; }
	
	cfg->SetValue( CB_FullScreen, !cfg->GetValue( CB_FullScreen ) );
	graph->ResizeScreen();	// スクリーンサイズ変更
	graph->DrawScreen();
}


/////////////////////////////////////////////////////////////////////////////
// UI:ウィンドウ表示倍率変更
//
// 引数:	zoom		表示倍率(%)
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_WindowZoom( int zoom )
{
	// ビデオキャプチャ中は無効
	if( AVI6::IsAVI() ){ return; }
	
	cfg->SetValue( CV_WindowZoom, zoom );
	graph->ResizeScreen();	// スクリーンサイズ変更
	graph->DrawScreen();
}


/////////////////////////////////////////////////////////////////////////////
// UI:ステータスバー表示状態変更
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_StatusBar( void )
{
	// ビデオキャプチャ中は無効
	if( AVI6::IsAVI() ){ return; }
	
	cfg->SetValue( CB_DispStatus, !cfg->GetValue( CB_DispStatus ) );
	graph->ResizeScreen();	// スクリーンサイズ変更
	graph->DrawScreen();
}


/////////////////////////////////////////////////////////////////////////////
// UI:4:3表示変更
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_Disp43( void )
{
	// ビデオキャプチャ中は無効
	if( AVI6::IsAVI() ){ return; }
	
	cfg->SetValue( CB_DispNTSC, !cfg->GetValue( CB_DispNTSC ) );
	graph->ResizeScreen();	// スクリーンサイズ変更
	graph->DrawScreen();
}


/////////////////////////////////////////////////////////////////////////////
// UI:スキャンラインモード変更
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_ScanLine( void )
{
	cfg->SetValue( CB_ScanLine, !cfg->GetValue( CB_ScanLine ) );
}


/////////////////////////////////////////////////////////////////////////////
// UI:フィルタリング変更
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_Filtering( void )
{
	// ビデオキャプチャ中は無効
	if( AVI6::IsAVI() ){ return; }
	
	cfg->SetValue( CB_Filtering, !cfg->GetValue( CB_Filtering ) );
	graph->ResizeScreen();	// スクリーンサイズ変更
	graph->DrawScreen();
}


/////////////////////////////////////////////////////////////////////////////
// UI:MODE4カラー変更
//
// 引数:	col			0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_Mode4Color( int col )
{
	col = col % 5;
	cfg->SetValue( CV_Mode4Color, col );
	vm->vdg->SetMode4Color( col );
}


/////////////////////////////////////////////////////////////////////////////
// UI:フレームスキップ変更
//
// 引数:	sk			フレームスキップ数
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_FrameSkip( int sk )
{
	// ビデオキャプチャ中は無効
	if( AVI6::IsAVI() ){ return; }
	
	cfg->SetValue( CV_FrameSkip, sk );
}


/////////////////////////////////////////////////////////////////////////////
// UI:サンプリングレート変更
//
// 引数:	rate		サンプリングレート(Hz)
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_SampleRate( int rate )
{
	// ビデオキャプチャ中は無効
	if( AVI6::IsAVI() ){ return; }
	
	cfg->SetValue( CV_SampleRate, rate );
	snd->SetSampleRate( rate );
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// UI:モニタモード切替え
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_Monitor( void )
{
	// ビデオキャプチャ中は無効
	if( AVI6::IsAVI() ){ return; }
	
	vm->SetMonitor( !vm->IsMonitor() );
	graph->ResizeScreen();
	graph->DrawScreen();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



/////////////////////////////////////////////////////////////////////////////
// UI:環境設定
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EL6::UI_Config( void )
{
	cfg->Write();
	if( OSD_ConfigDialog( GetWindowHandle() ) > 0 ){
		CFG6 ccfg;
		
		ccfg.Init();	// 変更したINIを読込み(比較用)
		bool reb =  cfg->GetValue( CV_Model )       != ccfg.GetValue( CV_Model )       ||	// 機種取得
					cfg->GetValue( CV_FDDrive )     != ccfg.GetValue( CV_FDDrive )     ||	// FDドライブ数取得
					cfg->GetValue( CV_OverClock )   != ccfg.GetValue( CV_OverClock )   ||	// オーバークロック率
					cfg->GetValue( CV_ExCartridge ) != ccfg.GetValue( CV_ExCartridge ) ||	// 拡張カートリッジ
					cfg->GetValue( CF_ExtRom )      != ccfg.GetValue( CF_ExtRom )      ||	// 拡張ROMファイル名取得
					cfg->GetValue( CF_Tape )        != ccfg.GetValue( CF_Tape )        ||	// TAPE(LOAD)ファイル名
					cfg->GetValue( CF_Disk1 )       != ccfg.GetValue( CF_Disk1 )       ||	// DISK1ファイル名
					cfg->GetValue( CF_Disk2 )       != ccfg.GetValue( CF_Disk2 );			// DISK2ファイル名
		
		cfg->Init();	// 変更したINIを読込み(オリジナル)
		
		// 再起動?
		if( reb && OSD_Message( GetWindowHandle(), GetText( T_RESTART ), GetText( T_RESTARTC ), OSDM_YESNO | OSDM_ICONQUESTION ) == OSDR_YES ){
			OSD_PushEvent( EV_RESTART );
			return;
		}
		
		// 設定反映
		// [CONFIG] ----------------------------------------------------
		vm->cmtl->SetBoost( cfg->GetValue( CB_BoostUp ) );			// BoostUp 有効フラグ
		vm->cmtl->SetMaxBoost( cfg->GetValue( CV_MaxBoost60 ), cfg->GetValue( CV_MaxBoost62 ) );	// BoostUp 最大倍率
		vm->disk->WaitEnable( cfg->GetValue( CB_FDDWait ) );		// FDアクセスウェイト有効フラグ
		vm->cmtl->SetStopBit( cfg->GetValue( CV_StopBit ) );		// ストップビット数
		
		// [DISPLAY] ---------------------------------------------------
		vm->vdg->SetMode4Color( cfg->GetValue( CV_Mode4Color ) );	// モード4カラーモード
		
		// [SOUND] -----------------------------------------------------
																	// サンプリングレート, サウンドバッファ長倍率
		snd->SetSampleRate( cfg->GetValue( CV_SampleRate ), cfg->GetValue( CV_SoundBuffer ) );
		snd->SetVolume( cfg->GetValue( CV_MasterVol ) );			// マスター音量
		vm->psg->SetVolume( cfg->GetValue( CV_PsgVolume ) );		// PSG/OPN音量
		vm->psg->SetLPF( cfg->GetValue( CV_PsgLPF ) );				// PSG/OPN LPFカットオフ周波数
		vm->voice->SetVolume( cfg->GetValue( CV_VoiceVolume ) );	// 音声合成音量
		vm->cmtl->SetVolume( cfg->GetValue( CV_TapeVolume ) );		// TAPEモニタ音量取得
		vm->cmtl->SetLPF( cfg->GetValue( CV_TapeLPF ) );			// TAPE LPFカットオフ周波数取得
		
		// [FILES] -----------------------------------------------------
		vm->pio->cPRT::SetFile( cfg->GetValue( CF_Printer ) );		// プリンタファイル名取得
		
		// [PATH] ------------------------------------------------------
		vm->voice->SetPath( cfg->GetValue( CF_WavePath ) );			// WAVEパス取得
		
		// [COLOR] -----------------------------------------------------
		SetPalette();												// パレット設定
		
		graph->ResizeScreen();	// スクリーンサイズ変更
	}
}


/////////////////////////////////////////////////////////////////////////////
// メニュー選択項目実行
//
// 引数:	id		選択したメニューID
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
#if 0 // P6VXでは独自実装
void EL6::ExecMenu( int id )
{
	// 項目ごとの処理
	switch( id ){
	case ID_TAPEINSERT:		UI_TapeInsert();						break;	// TAPE 挿入
	case ID_TAPEEJECT:		TapeUnmount();							break;	// TAPE 排出
	
	case ID_DISKINSERT1:													// DISK 挿入
	case ID_DISKINSERT2:	UI_DiskInsert( id - ID_DISKINSERT1 );	break;
	case ID_DISKEJECT1:														// DISK 排出
	case ID_DISKEJECT2:		DiskUnmount( id - ID_DISKEJECT1 );		break;
	
	case ID_C6005:			UI_CartInsert     ( EXC6005   );		break;	// 拡張カートリッジ 挿入			(PC-6005	ROMカートリッジ)
	case ID_C6006:			UI_CartInsert     ( EXC6006   );		break;	// 拡張カートリッジ 挿入			(PC-6006	拡張ROM/RAMカートリッジ)
	case ID_C6006NR:		UI_CartInsertNoRom( EXC6006   );		break;	// 拡張カートリッジ 挿入(ROMなし)	(PC-6006	拡張ROM/RAMカートリッジ)
	case ID_C6001:			UI_CartInsert     ( EXC6001   );		break;	// 拡張カートリッジ 挿入			(PCS-6001R	拡張BASIC)
	case ID_C660101:		UI_CartInsert     ( EXC660101 );		break;	// 拡張カートリッジ 挿入			(PC-6601-01	拡張漢字ROMカートリッジ)
	case ID_C6006SR:		UI_CartInsert     ( EXC6006SR );		break;	// 拡張カートリッジ 挿入			(PC-6006SR	拡張64KRAMカートリッジ)
	case ID_C6007SR:		UI_CartInsert     ( EXC6007SR );		break;	// 拡張カートリッジ 挿入			(PC-6007SR	拡張漢字ROM&RAMカートリッジ)
	case ID_C6053:			UI_CartInsert     ( EXC6053   );		break;	// 拡張カートリッジ 挿入			(PC-6053    ボイスシンセサイザー)
	case ID_C60M55:			UI_CartInsert     ( EXC60M55  );		break;	// 拡張カートリッジ 挿入			(PC-60m55   FM音源カートリッジ)
	case ID_CSOL1:			UI_CartInsert     ( EXCSOL1   );		break;	// 拡張カートリッジ 挿入			(戦士のカートリッジ)
	case ID_CSOL1NR:		UI_CartInsertNoRom( EXCSOL1   );		break;	// 拡張カートリッジ 挿入(ROMなし)	(戦士のカートリッジ)
	case ID_CSOL2:			UI_CartInsert     ( EXCSOL2   );		break;	// 拡張カートリッジ 挿入			(戦士のカートリッジmkⅡ)
	case ID_CSOL2NR:		UI_CartInsertNoRom( EXCSOL2   );		break;	// 拡張カートリッジ 挿入(ROMなし)	(戦士のカートリッジmkⅡ)
	case ID_CSOL3:			UI_CartInsert     ( EXCSOL3   );		break;	// 拡張カートリッジ 挿入			(戦士のカートリッジmkⅢ)
	case ID_CSOL3NR:		UI_CartInsertNoRom( EXCSOL3   );		break;	// 拡張カートリッジ 挿入(ROMなし)	(戦士のカートリッジmkⅢ)
	case ID_CARTEJECT:		UI_CartEject();							break;	// 拡張カートリッジ 排出
	
	case ID_JOY100:															// ジョイスティック1
	case ID_JOY101:
	case ID_JOY102:
	case ID_JOY103:
	case ID_JOY104:
	case ID_JOY105:			joy->Connect( 0, id - ID_JOY101 );		break;
	case ID_JOY200:															// ジョイスティック2
	case ID_JOY201:
	case ID_JOY202:
	case ID_JOY203:
	case ID_JOY204:
	case ID_JOY205:			joy->Connect( 1, id - ID_JOY201 );		break;
	
	case ID_CONFIG:			UI_Config();							break;	// 環境設定
	case ID_RESET:			UI_Reset();								break;	// リセット
	case ID_RESTART:		UI_Restart();							break;	// 再起動
	case ID_PAUSE:			UI_Pause();								break;	// ポーズ
	case ID_NOWAIT:			UI_NoWait();							break;	// Wait変更
	
	case ID_SNAPSHOT:		UI_SnapShot();							break;	// スナップショット
	
	case ID_DOKOSAVE:		UI_DokoSave();							break;	// どこでもSAVE
	case ID_DOKOSAVE1:														// どこでもSAVE1
	case ID_DOKOSAVE2:														// どこでもSAVE2
	case ID_DOKOSAVE3:		UI_DokoSave( id - ID_DOKOSAVE );		break;	// どこでもSAVE3
	
	case ID_DOKOLOAD:		UI_DokoLoad();							break;	// どこでもLOAD
	case ID_DOKOLOAD1:														// どこでもLOAD1
	case ID_DOKOLOAD2:														// どこでもLOAD2
	case ID_DOKOLOAD3:		UI_DokoLoad( id - ID_DOKOLOAD );		break;	// どこでもLOAD3
	
	case ID_REPLAYSAVE:		UI_ReplaySave();						break;	// リプレイ保存/停止
	case ID_REPLAYRESUME:	UI_ReplayResumeSave();					break;	// リプレイ保存再開
	case ID_REPLAYDOKOSAVE:	UI_ReplayDokoSave();					break;	// リプレイ中どこでもSAVE
	case ID_REPLAYDOKOLOAD:	UI_ReplayDokoLoad();					break;	// リプレイ中どこでもLOAD
	case ID_REPLAYROLLBACK:	UI_ReplayRollback();					break;	// リプレイ中どこでもLOADを巻き戻す
	case ID_REPLAYPLAY:		UI_ReplayPlay();						break;	// リプレイ再生/停止
	case ID_REPLAYMOVIE:	UI_ReplayMovie();						break;	// リプレイを動画に変換
	case ID_AVISAVE:		AVI6::IsAVI() ? UI_AVISaveStop()
										  : UI_AVISaveStart();		break;	// ビデオキャプチャ
										  
	case ID_AUTOTYPE:		UI_AutoType();							break;	// 打込み代行
	case ID_QUIT:			UI_Quit();								break;	// 終了
	case ID_TURBO:			UI_TurboTape();							break;	// Turbo TAPE
	case ID_BOOST:			UI_BoostUp();							break;	// Boost Up
	case ID_FULLSCRN:		UI_FullScreen();						break;	// フルスクリーン変更
	
	case ID_ZOOM100:														// ウィンドウ表示倍率100%
	case ID_ZOOM200:														// ウィンドウ表示倍率200%
	case ID_ZOOM300:		UI_WindowZoom( (id-ID_ZOOM100+1)*100 );	break;	// ウィンドウ表示倍率300%
	
	case ID_STATUS:			UI_StatusBar();							break;	// ステータスバー表示状態変更
	case ID_DISP43:			UI_Disp43();							break;	// 4:3表示変更
	case ID_SCANLINE:		UI_ScanLine();							break;	// スキャンラインモード変更
	case ID_FILTERING:		UI_Filtering();							break;	// フィルタリング変更
	
	case ID_M4MONO:															// MODE4カラー モノクロ
	case ID_M4RDBL:															// MODE4カラー 赤/青
	case ID_M4BLRD:															// MODE4カラー 青/赤
	case ID_M4PKGR:															// MODE4カラー ピンク/緑
	case ID_M4GRPK:			UI_Mode4Color( id - ID_M4MONO );		break;	// MODE4カラー 緑/ピンク
	
	case ID_FSKP0:															// フレームスキップ なし
	case ID_FSKP1:															// フレームスキップ 1
	case ID_FSKP2:															// フレームスキップ 2
	case ID_FSKP3:															// フレームスキップ 3
	case ID_FSKP4:															// フレームスキップ 4
	case ID_FSKP5:			UI_FrameSkip( id - ID_FSKP0 );			break;	// フレームスキップ 5
	
	case ID_SPR44:															// サンプリングレート 44100Hz
	case ID_SPR22:															// サンプリングレート 22050Hz
	case ID_SPR11:			UI_SampleRate( 44100 >> (id - ID_SPR44 ) );	break;	// サンプリングレート 11025Hz
	
	case ID_VERSION:		OSD_VersionDialog( GetWindowHandle(), cfg->GetValue( CV_Model ) );	break;	// バージョン情報
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	case ID_MONITOR:		UI_Monitor();							break;	// モニターモード
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	}
}
#endif
