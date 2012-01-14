#include <new>

#include "pc6001v.h"
#include "pc60.h"
#include "error.h"

#include "ini.h"

#include "breakpoint.h"
#include "common.h"
#include "cpum.h"
#include "cpus.h"
#include "debug.h"
#include "disk.h"
#include "graph.h"
#include "intr.h"
#include "io.h"
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
#include "usrevent.h"
#include "vdg.h"

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
VM6::VM6( void )
{
	UDFPSCount = 0;		// FPS表示タイマカウンタ初期化
	FSkipCount = 0;		// フレームスキップカウンタ
	
	cfg		= NULL;
	io		= NULL;
	ios		= NULL;
	sche	= NULL;
	snd		= NULL;
	psg		= NULL;
	mem		= NULL;
	vdg		= NULL;
	graph	= NULL;
	intr	= NULL;
	cpum	= NULL;
	disk	= NULL;
	cmt		= NULL;
	cmts	= NULL;
	cpus	= NULL;
	pio		= NULL;
	key		= NULL;
	joy     = NULL;
	voice   = NULL;
	
	staw    = NULL;
	
	UpDateFPSID	= NULL;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	bp      = NULL;
	regw	= NULL;
	memw	= NULL;
	monw	= NULL;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	// パレット初期化
	ZeroMemory( GPal, sizeof(COLOR24)*256 );
	
	// 自動キー入力無効
	ZeroMemory( &ak, sizeof( AKEY ) );

}

VM60::VM60( void ){}
VM62::VM62( void ){}
VM66::VM66( void ){}



////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
VM6::~VM6( void )
{
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( monw )   delete monw;
	if( memw )   delete memw;
	if( regw )   delete regw;
	if( bp )     delete bp;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	if( UpDateFPSID ) SDL_RemoveTimer( UpDateFPSID );
	
	if( staw )   delete staw;
	
	if( voice )  delete voice;
	if( joy )    delete joy;
	if( key )    delete key;
	if( pio )    delete pio;
	if( cpus )   delete cpus;
	if( cmts )   delete cmts;
	if( cmt )    delete cmt;
	if( disk )   delete disk;
	if( cpum )   delete cpum;
	if( intr )   delete intr;
	if( graph )  delete graph;
	if( vdg )    delete vdg;
	if( mem )    delete mem;
	if( psg )    delete psg;
	if( snd )    delete snd;
	if( sche )   delete sche;
	if( ios )    delete ios;
	if( io )     delete io;
	
	// コンソール用フォント破棄
	JFont::CloseFont();
	
	// 自動キー入力バッファ破棄
	if( ak.Buffer ) delete [] ak.Buffer;
}

VM60::~VM60( void ){}
VM62::~VM62( void ){}
VM66::~VM66( void ){}



////////////////////////////////////////////////////////////////
// スレッド関数(オーバーライド)
////////////////////////////////////////////////////////////////
void VM6::OnThread( void *inst )
{
	VM6 *p6;
	
	p6 = (VM6 *)inst;	// 自分自身のオブジェクトポインタ取得
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( p6->cfg->GetMonDisp() ){
	// モニタモード
		while( !this->cThread::IsCancel() ){
			// 画面更新
			if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
			p6->sche->VWait();		// ウェイト
		}
	}else
	// 通常モード
		if( bp->ExistBreakPoint() ){
			// ブレークポイントあり
			while( !this->cThread::IsCancel() ){
				p6->Emu();		// 1命令実行
				
				if( p6->sche->IsVSYNC() ){
					// ノーウェイト時のキーレスポンスをよくするため
					// 強制的にイベントを汲み上げる
					SDL_PumpEvents();
					p6->key->ScanMatrix();	// キーマトリクススキャン
					
					// サウンド更新
					p6->SoundUpdate( 0 );
					// 画面更新
					if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
					p6->sche->VWait();		// ウェイト
				}
				
				// ブレークポイントチェック
				if( bp->CheckBreakPoint( BPoint::BP_PC, cpum->GetPC(), 0, NULL ) || bp->IsReqBreak() ){
					this->cThread::Cancel();	// スレッド終了フラグ立てる
					bp->ResetBreak();
					SENDEVENT1( SDL_DEBUGMODEBP, cpum->GetPC() )
				}
			}
		}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	{	// 通常実行
		while( !this->cThread::IsCancel() ){
			// ノーウェイト時のキーレスポンスをよくするため
			// 強制的にイベントを汲み上げる
			SDL_PumpEvents();
			
			
			// リプレイ再生中
			if( REPLAY::GetStatus() == REP_REPLAY )
				REPLAY::ReplayReadFrame( p6->key->GetMatrix() );
			
			// キーマトリクススキャン
			BOOL matchg = p6->key->ScanMatrix();
			
			// リプレイ記録中
			if( REPLAY::GetStatus() == REP_RECORD )
				REPLAY::ReplayWriteFrame( p6->key->GetMatrix2(), matchg );
			
			
			p6->EmuVSYNC();			// 1画面分実行
			
			// ビデオキャプチャ中?
			if( AVI6::IsAVI() ){
				// ビデオキャプチャ中ならここでAVI保存処理
				// サウンド更新
				p6->SoundUpdate( 0, AVI6::GetAudioBuffer() );
				// 画面更新
				BOOL update = p6->ScreenUpdate();
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
				if( key ) p6->cpus->ReqKeyIntr( 0, key );
			}
			
			p6->sche->VWait();		// ウェイト
		}
	}
}


////////////////////////////////////////////////////////////////
// Wait有効?
////////////////////////////////////////////////////////////////
BOOL VM6::IsWaitEnable( void )
{
	return sche->GetWaitEnable() && (!cfg->GetTurboTAPE() || (cpus->GetCmtStatus() == CMTCLOSE));
}


////////////////////////////////////////////////////////////////
// 1命令実行
////////////////////////////////////////////////////////////////
int VM6::Emu( void )
{
	int state = cpum->Exec();	// CPU 1命令実行
	sche->Update( state );		// イベント更新
	
	return state;
}


////////////////////////////////////////////////////////////////
// 1画面分実行
////////////////////////////////////////////////////////////////
int VM6::EmuVSYNC( void )
{
	int state = 0;
	
	// VSYNCが発生するまで繰返し
	while( !sche->IsVSYNC() ){
		int st = cpum->Exec();	// CPU 1命令実行
		sche->Update( st );		// イベント更新
		state += st;
	}
	
	return state;
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
////////////////////////////////////////////////////////////////
// 指定ステート数実行
////////////////////////////////////////////////////////////////
void VM6::Exec( int st )
{
	int State = st;
	
	while( State > 0 ) State -= Emu();
}


////////////////////////////////////////////////////////////////
// モニタモード切替え
////////////////////////////////////////////////////////////////
BOOL VM6::ToggleMonitor( void )
{
	// VM停止
	Stop();
	
	// モニタウィンドウ表示状態切換え
	cfg->SetMonDisp( cfg->GetMonDisp() ? FALSE : TRUE );
	
	// モニタモードならUnicode有効
	SDL_EnableUNICODE( cfg->GetMonDisp() ? 1 : 0 );
	
	// ブレークポイントの情報をクリア
	bp->ClearStatus();
	
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
BOOL VM60::Init( const cConfig *config )
{
	// エラーメッセージ初期値
	Error::SetError( Error::InitFailed );
	
	if( !config ) return FALSE;
	cfg = (cConfig *)config;
	
	// 各種オブジェクト確保
	intr   = new INT60( this, DEV_ID('I', 'N', 'T', 'R') );		// 割込み
	sche   = new SCH6 ( this, DEV_ID('S', 'C', 'H', 'E'), CPUM_CLOCK60 * cfg->GetOverClock() / 100 );		// スケジューラ
	mem    = new MEM60( DEV_ID('M', 'E', 'M', '1'), cfg->GetUseExtRam() );		// メモリ
	vdg    = new VDG60( this, DEV_ID('V', 'D', 'G', '1') );		// VDG
	key    = new KEY60( this, DEV_ID('K', 'E', 'Y', 'B') );		// キー
	disk   = new DSK60( this, DEV_ID('D', 'I', 'S', 'K'), cfg->GetFddNum() );	// DISK
	
	if( !intr || !sche || !mem || !vdg || !key || !disk )
		return FALSE;
	
	// 共通部分初期化
	if( !VM6::InitCommon() ) return FALSE;
	
	// 各種オブジェクト初期化とデバイス接続
	
	// 割込み -----
	const static IOBus::Connector c_intr[] = {
		{ 0xb0, IOBus::portout, INT60::outB0H },
		{ 0xb1, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb2, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb3, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb4, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb5, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb6, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb7, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb8, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb9, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xba, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbb, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbc, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbd, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbe, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbf, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0, 0, 0 }	};
	if( !io->Connect( (INT60*)intr, c_intr ) ) return FALSE;
	
	// VDG -----
	const static IOBus::Connector c_vdg[] = {
		{ 0xb0, IOBus::portout, VDG60::outB0H },
		{ 0xb1, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb2, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb3, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb4, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb5, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb6, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb7, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb8, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb9, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xba, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbb, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbc, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbd, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbe, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbf, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0, 0, 0 } };
	if( !io->Connect( (VDG60*)vdg, c_vdg ) ) return FALSE;
	
	// PSG -----
	const static IOBus::Connector c_psg[] = {
		{ 0xa0, IOBus::portout, PSG6::outA0H },
		{ 0xa1, IOBus::portout, PSG6::outA1H },
		{ 0xa3, IOBus::portout, PSG6::outA3H },
		{ 0xa4, IOBus::portout, PSG6::outA0H },	// イメージ
		{ 0xa5, IOBus::portout, PSG6::outA1H },	// イメージ
		{ 0xa7, IOBus::portout, PSG6::outA3H },	// イメージ
		{ 0xa8, IOBus::portout, PSG6::outA0H },	// イメージ
		{ 0xa9, IOBus::portout, PSG6::outA1H },	// イメージ
		{ 0xab, IOBus::portout, PSG6::outA3H },	// イメージ
		{ 0xac, IOBus::portout, PSG6::outA0H },	// イメージ
		{ 0xad, IOBus::portout, PSG6::outA1H },	// イメージ
		{ 0xaf, IOBus::portout, PSG6::outA3H },	// イメージ
		{ 0xa2, IOBus::portin,  PSG6::inA2H },
		{ 0xa6, IOBus::portin,  PSG6::inA2H },	// イメージ
		{ 0xaa, IOBus::portin,  PSG6::inA2H },	// イメージ
		{ 0xae, IOBus::portin,  PSG6::inA2H },	// イメージ
		{ 0, 0, 0 }	};
	if( !io->Connect( psg, c_psg ) ) return FALSE;
	int i=0;
	while( c_psg[i].bank ){						// ウェイト設定
		if( c_psg[i].rule == IOBus::portout ) io->SetOutWait( c_psg[i].bank, 1 );
		else								  io->SetInWait( c_psg[i].bank, 1 );
		i++;
	}
	if( !psg->Init( PSG_CLOCK60, snd->GetSampleRate() ) ) return FALSE;
	
	// 8255(Z80側) -----
	const static IOBus::Connector c_8255[] = {
		{ 0x90, IOBus::portout, PIO6::out90H },
		{ 0x91, IOBus::portout, PIO6::out91H },
		{ 0x92, IOBus::portout, PIO6::out92H },
		{ 0x93, IOBus::portout, PIO6::out93H },
		{ 0x94, IOBus::portout, PIO6::out90H },	// イメージ
		{ 0x95, IOBus::portout, PIO6::out91H },	// イメージ
		{ 0x96, IOBus::portout, PIO6::out92H },	// イメージ
		{ 0x97, IOBus::portout, PIO6::out93H },	// イメージ
		{ 0x98, IOBus::portout, PIO6::out90H },	// イメージ
		{ 0x99, IOBus::portout, PIO6::out91H },	// イメージ
		{ 0x9a, IOBus::portout, PIO6::out92H },	// イメージ
		{ 0x9b, IOBus::portout, PIO6::out93H },	// イメージ
		{ 0x9c, IOBus::portout, PIO6::out90H },	// イメージ
		{ 0x9d, IOBus::portout, PIO6::out91H },	// イメージ
		{ 0x9e, IOBus::portout, PIO6::out92H },	// イメージ
		{ 0x9f, IOBus::portout, PIO6::out93H },	// イメージ
		{ 0x90, IOBus::portin,  PIO6::in90H },
		{ 0x92, IOBus::portin,  PIO6::in92H },
		{ 0x93, IOBus::portin,  PIO6::in93H },
		{ 0x94, IOBus::portin,  PIO6::in90H },	// イメージ
		{ 0x96, IOBus::portin,  PIO6::in92H },	// イメージ
		{ 0x97, IOBus::portin,  PIO6::in93H },	// イメージ
		{ 0x98, IOBus::portin,  PIO6::in90H },	// イメージ
		{ 0x9a, IOBus::portin,  PIO6::in92H },	// イメージ
		{ 0x9b, IOBus::portin,  PIO6::in93H },	// イメージ
		{ 0x9c, IOBus::portin,  PIO6::in90H },	// イメージ
		{ 0x9e, IOBus::portin,  PIO6::in92H },	// イメージ
		{ 0x9f, IOBus::portin,  PIO6::in93H },	// イメージ
		{ 0, 0, 0 }	};
	if( !io->Connect( pio, c_8255 ) ) return FALSE;
	
	// 8255(SUB CPU側) -----
	const static IOBus::Connector c_8255s[] = {
		{ IO8049_BUS, IOBus::portout, PIO6::outPBH },
		{ IO8049_BUS, IOBus::portin,  PIO6::inPBH  },
		{ IO8049_T0,  IOBus::portin,  PIO6::inIBF  },
		{ IO8049_INT, IOBus::portin,  PIO6::inOBF  },
		{ 0, 0, 0 }	};
	if( !ios->Connect( pio, c_8255s ) ) return FALSE;
	
	// DISK -----
	if( disk->GetDrives() ){
		const static IOBus::Connector c_disk[] = {
			{ 0xd1, IOBus::portout, DSK60::outD1H },
			{ 0xd2, IOBus::portout, DSK60::outD2H },
			{ 0xd3, IOBus::portout, DSK60::outD3H },
			{ 0xd5, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xd6, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xd7, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xd9, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xda, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xdb, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xdd, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xde, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xdf, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xd0, IOBus::portin,  DSK60::inD0H },
			{ 0xd1, IOBus::portin,  DSK60::inD1H },
			{ 0xd2, IOBus::portin,  DSK60::inD2H },
			{ 0xd3, IOBus::portin,  DSK60::inD3H },
			{ 0xd4, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xd5, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xd6, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xd7, IOBus::portin,  DSK60::inD3H },		// イメージ
			{ 0xd8, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xd9, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xda, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xdb, IOBus::portin,  DSK60::inD3H },		// イメージ
			{ 0xdc, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xdd, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xde, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xdf, IOBus::portin,  DSK60::inD3H },		// イメージ
			{ 0, 0, 0 }	};
		if( !io->Connect( (DSK60*)disk, c_disk ) ) return FALSE;
		if( !disk->Init() ) return FALSE;
		// ドライブ1にDISK挿入
		if( *(cfg->GetDiskFile()) ) disk->Mount( 0, cfg->GetDiskFile() );
	}
	
	// CMT(LOAD) -----
	const static IOBus::Connector c_cmt[] = {
		{ 0xb0, IOBus::portout, CMTL::outB0H },
		{ 0xb1, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb2, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb3, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb4, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb5, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb6, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb7, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb8, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb9, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xba, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbb, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbc, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbd, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbe, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbf, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0, 0, 0 }	};
	if( !io->Connect( cmt, c_cmt ) ) return FALSE;
	
	// オプション機能 -----
	// 戦士のカートリッジ対応
	if( cfg->GetUseSoldier() ){
		const static IOBus::Connector c_soldier[] = {
			{ 0x7f, IOBus::portout, MEM60::out7FH },
			{ 0, 0, 0 }	};
		if( !io->Connect( (MEM60*)mem, c_soldier ) ) return FALSE;
	}
	
	
	Reset();
	
	// スクリーンサイズ変更
	graph->ResizeScreen();
	
	// エラーなし
	Error::SetError( Error::NoError );
	
	return TRUE;
}

BOOL VM62::Init( const cConfig *config )
{
	// エラーメッセージ初期値
	Error::SetError( Error::InitFailed );
	
	if( !config ) return FALSE;
	cfg = (cConfig *)config;
	
	// 各種オブジェクト確保
	intr   = new INT62( this, DEV_ID('I', 'N', 'T', 'R') );		// 割込み
	sche   = new SCH6 ( this, DEV_ID('S', 'C', 'H', 'E'), CPUM_CLOCK62 * cfg->GetOverClock() / 100 );		// スケジューラ
	mem    = new MEM62( DEV_ID('M', 'E', 'M', '1'), cfg->GetUseExtRam() );	// メモリ
	vdg    = new VDG62( this, DEV_ID('V', 'D', 'G', '2') );		// VDG
	key    = new KEY6 ( this, DEV_ID('K', 'E', 'Y', 'B') );		// キー
	disk   = new DSK60( this, DEV_ID('D', 'I', 'S', 'K'), cfg->GetFddNum() );	// DISK
	voice  = new VCE6 ( this, DEV_ID('V', 'O', 'I', 'C') );		// 音声合成
	
	if( !intr || !sche || !mem || !vdg || !key || !disk || !voice )
		return FALSE;
	
	// 共通部分初期化
	if( !VM6::InitCommon() ) return FALSE;
	
	// 各種オブジェクト初期化とデバイス接続
	
	// 割込み -----
	const static IOBus::Connector c_intr[] = {
		{ 0xb0, IOBus::portout, INT62::outB0H },
		{ 0xf3, IOBus::portout, INT62::outF3H },
		{ 0xf4, IOBus::portout, INT62::outF4H },
		{ 0xf5, IOBus::portout, INT62::outF5H },
		{ 0xf6, IOBus::portout, INT62::outF6H },
		{ 0xf7, IOBus::portout, INT62::outF7H },
		{ 0xf3, IOBus::portin,  INT62::inF3H },
		{ 0xf4, IOBus::portin,  INT62::inF4H },
		{ 0xf5, IOBus::portin,  INT62::inF5H },
		{ 0xf6, IOBus::portin,  INT62::inF6H },
		{ 0xf7, IOBus::portin,  INT62::inF7H },
		{ 0, 0, 0 }	};
	if( !io->Connect( (INT62*)intr, c_intr ) ) return FALSE;
	
	// メモリ -----
	const static IOBus::Connector c_memory[] = {
		{ 0xc1, IOBus::portout, MEM62::outC1H },
		{ 0xc2, IOBus::portout, MEM62::outC2H },
		{ 0xc3, IOBus::portout, MEM62::outC3H },
		{ 0xf0, IOBus::portout, MEM62::outF0H },
		{ 0xf1, IOBus::portout, MEM62::outF1H },
		{ 0xf2, IOBus::portout, MEM62::outF2H },
		{ 0xf3, IOBus::portout, MEM62::outF3H },
		{ 0xf0, IOBus::portin,  MEM62::inF0H },
		{ 0xf1, IOBus::portin,  MEM62::inF1H },
		{ 0xf2, IOBus::portin,  MEM62::inF2H },
		{ 0xf3, IOBus::portin,  MEM62::inF3H },
		{ 0, 0, 0 }	};
	if( !io->Connect( (MEM62*)mem, c_memory ) ) return FALSE;
	
	// VDG -----
	const static IOBus::Connector c_vdg[] = {
		{ 0xb0, IOBus::portout, VDG62::outB0H },
		{ 0xc0, IOBus::portout, VDG62::outC0H },
		{ 0xc1, IOBus::portout, VDG62::outC1H },
		{ 0xc1, IOBus::portin,  VDG62::inC1H },
		{ 0, 0, 0 } };
	if( !io->Connect( (VDG62*)vdg, c_vdg ) ) return FALSE;
	
	// PSG -----
	const static IOBus::Connector c_psg[] = {
		{ 0xa0, IOBus::portout, PSG6::outA0H },
		{ 0xa1, IOBus::portout, PSG6::outA1H },
		{ 0xa3, IOBus::portout, PSG6::outA3H },
		{ 0xa2, IOBus::portin,  PSG6::inA2H },
		{ 0, 0, 0 }	};
	if( !io->Connect( psg, c_psg ) ) return FALSE;
	int i=0;
	while( c_psg[i].bank ){						// ウェイト設定
		if( c_psg[i].rule == IOBus::portout ) io->SetOutWait( c_psg[i].bank, 1 );
		else								  io->SetInWait( c_psg[i].bank, 1 );
		i++;
	}
	if( !psg->Init( PSG_CLOCK62, snd->GetSampleRate() ) ) return FALSE;
	
	// 8255(Z80側) -----
	const static IOBus::Connector c_8255[] = {
		{ 0x90, IOBus::portout, PIO6::out90H },
		{ 0x91, IOBus::portout, PIO6::out91H },
		{ 0x92, IOBus::portout, PIO6::out92H },
		{ 0x93, IOBus::portout, PIO6::out93H },
		{ 0x90, IOBus::portin,  PIO6::in90H },
		{ 0x92, IOBus::portin,  PIO6::in92H },
		{ 0x93, IOBus::portin,  PIO6::in93H },
		{ 0, 0, 0 }	};
	if( !io->Connect( pio, c_8255 ) ) return FALSE;
	
	// 8255(SUB CPU側) -----
	const static IOBus::Connector c_8255s[] = {
		{ IO8049_BUS, IOBus::portout, PIO6::outPBH },
		{ IO8049_BUS, IOBus::portin,  PIO6::inPBH  },
		{ IO8049_T0,  IOBus::portin,  PIO6::inIBF  },
		{ IO8049_INT, IOBus::portin,  PIO6::inOBF  },
		{ 0, 0, 0 }	};
	if( !ios->Connect( pio, c_8255s ) ) return FALSE;
	
	// 音声合成 -----
	const static IOBus::Connector c_voice[] = {
		{ 0xe0, IOBus::portout, VCE6::outE0H },
		{ 0xe2, IOBus::portout, VCE6::outE2H },
		{ 0xe3, IOBus::portout, VCE6::outE3H },
		{ 0xe4, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xe6, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xe7, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xe8, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xea, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xeb, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xec, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xee, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xef, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xe0, IOBus::portin,  VCE6::inE0H },
		{ 0xe2, IOBus::portin,  VCE6::inE2H },
		{ 0xe3, IOBus::portin,  VCE6::inE3H },
		{ 0xe4, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xe6, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xe7, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0xe8, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xea, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xeb, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0xec, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xee, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xef, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0, 0, 0 }	};
	if( !io->Connect( voice, c_voice ) ) return FALSE;
	if( !voice->Init( snd->GetSampleRate(), cfg->GetWavePath() ) ) return FALSE;
	snd->ConnectStream( (SndDev *)voice );	// ストリーム接続
	voice->SetVolume( cfg->GetVoiceVol() );	// 音量設定
	
	// DISK -----
	if( disk->GetDrives() ){
		const static IOBus::Connector c_disk[] = {
			{ 0xd1, IOBus::portout, DSK60::outD1H },
			{ 0xd2, IOBus::portout, DSK60::outD2H },
			{ 0xd3, IOBus::portout, DSK60::outD3H },
			{ 0xd5, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xd6, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xd7, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xd9, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xda, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xdb, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xdd, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xde, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xdf, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xd0, IOBus::portin,  DSK60::inD0H },
			{ 0xd1, IOBus::portin,  DSK60::inD1H },
			{ 0xd2, IOBus::portin,  DSK60::inD2H },
			{ 0xd3, IOBus::portin,  DSK60::inD3H },
			{ 0xd4, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xd5, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xd6, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xd7, IOBus::portin,  DSK60::inD3H },		// イメージ
			{ 0xd8, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xd9, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xda, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xdb, IOBus::portin,  DSK60::inD3H },		// イメージ
			{ 0xdc, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xdd, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xde, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xdf, IOBus::portin,  DSK60::inD3H },		// イメージ
			{ 0, 0, 0 }	};
		if( !io->Connect( (DSK60*)disk, c_disk ) ) return FALSE;
		if( !disk->Init() ) return FALSE;
		// ドライブ1にDISK挿入
		if( *(cfg->GetDiskFile()) ) disk->Mount( 0, cfg->GetDiskFile() );
	}
	
	// CMT(LOAD) -----
	const static IOBus::Connector c_cmt[] = {
		{ 0xb0, IOBus::portout, CMTL::outB0H },
		{ 0, 0, 0 }	};
	if( !io->Connect( cmt, c_cmt ) ) return FALSE;
	
	// オプション機能 -----
	// 戦士のカートリッジ対応
	if( cfg->GetUseSoldier() ){
		const static IOBus::Connector c_soldier[] = {
			{ 0x7f, IOBus::portout, MEM62::out7FH },
			{ 0, 0, 0 }	};
		if( !io->Connect( (MEM62*)mem, c_soldier ) ) return FALSE;
	}
	
	
	Reset();
	
	// スクリーンサイズ変更
	graph->ResizeScreen();
	
	// エラーなし
	Error::SetError( Error::NoError );
	
	return TRUE;
}

BOOL VM66::Init( const cConfig *config )
{
	// エラーメッセージ初期値
	Error::SetError( Error::InitFailed );
	
	if( !config ) return FALSE;
	cfg = (cConfig *)config;
	
	// 各種オブジェクト確保
	intr   = new INT62( this, DEV_ID('I', 'N', 'T', 'R') );		// 割込み
	sche   = new SCH6 ( this, DEV_ID('S', 'C', 'H', 'E'), CPUM_CLOCK62 * cfg->GetOverClock() / 100 );		// スケジューラ
	mem    = new MEM66( DEV_ID('M', 'E', 'M', '1'), cfg->GetUseExtRam() );	// メモリ
	vdg    = new VDG62( this, DEV_ID('V', 'D', 'G', '2') );		// VDG
	key    = new KEY6 ( this, DEV_ID('K', 'E', 'Y', 'B') );		// キー
	disk   = new DSK66( this, DEV_ID('D', 'I', 'S', 'K'), cfg->GetFddNum() );	// DISK
	voice  = new VCE6 ( this, DEV_ID('V', 'O', 'I', 'C') );		// 音声合成
	
	if( !intr || !sche || !mem || !vdg || !key || !disk || !voice )
		return FALSE;
	
	// 共通部分初期化
	if( !VM6::InitCommon() ) return FALSE;
	
	// 各種オブジェクト初期化とデバイス接続
	
	// 割込み -----
	const static IOBus::Connector c_intr[] = {
		{ 0xb0, IOBus::portout, INT62::outB0H },
		{ 0xf3, IOBus::portout, INT62::outF3H },
		{ 0xf4, IOBus::portout, INT62::outF4H },
		{ 0xf5, IOBus::portout, INT62::outF5H },
		{ 0xf6, IOBus::portout, INT62::outF6H },
		{ 0xf7, IOBus::portout, INT62::outF7H },
		{ 0xf3, IOBus::portin,  INT62::inF3H },
		{ 0xf4, IOBus::portin,  INT62::inF4H },
		{ 0xf5, IOBus::portin,  INT62::inF5H },
		{ 0xf6, IOBus::portin,  INT62::inF6H },
		{ 0xf7, IOBus::portin,  INT62::inF7H },
		{ 0, 0, 0 }	};
	if( !io->Connect( (INT62*)intr, c_intr ) ) return FALSE;
	
	// メモリ -----
	const static IOBus::Connector c_memory[] = {
		{ 0xc1, IOBus::portout, MEM62::outC1H },
		{ 0xc2, IOBus::portout, MEM62::outC2H },
		{ 0xc3, IOBus::portout, MEM62::outC3H },
		{ 0xf0, IOBus::portout, MEM62::outF0H },
		{ 0xf1, IOBus::portout, MEM62::outF1H },
		{ 0xf2, IOBus::portout, MEM62::outF2H },
		{ 0xf3, IOBus::portout, MEM62::outF3H },
		{ 0xf0, IOBus::portin,  MEM62::inF0H },
		{ 0xf1, IOBus::portin,  MEM62::inF1H },
		{ 0xf2, IOBus::portin,  MEM62::inF2H },
		{ 0xf3, IOBus::portin,  MEM62::inF3H },
		{ 0, 0, 0 }	};
	if( !io->Connect( (MEM66*)mem, c_memory ) ) return FALSE;
	
	// VDG -----
	const static IOBus::Connector c_vdg[] = {
		{ 0xb0, IOBus::portout, VDG62::outB0H },
		{ 0xc0, IOBus::portout, VDG62::outC0H },
		{ 0xc1, IOBus::portout, VDG62::outC1H },
		{ 0xc1, IOBus::portin,  VDG62::inC1H },
		{ 0, 0, 0 } };
	if( !io->Connect( (VDG62*)vdg, c_vdg ) ) return FALSE;
	
	// PSG -----
	const static IOBus::Connector c_psg[] = {
		{ 0xa0, IOBus::portout, PSG6::outA0H },
		{ 0xa1, IOBus::portout, PSG6::outA1H },
		{ 0xa3, IOBus::portout, PSG6::outA3H },
		{ 0xa2, IOBus::portin,  PSG6::inA2H },
		{ 0, 0, 0 }	};
	if( !io->Connect( psg, c_psg ) ) return FALSE;
	int i=0;
	while( c_psg[i].bank ){						// ウェイト設定
		if( c_psg[i].rule == IOBus::portout ) io->SetOutWait( c_psg[i].bank, 1 );
		else								  io->SetInWait( c_psg[i].bank, 1 );
		i++;
	}
	if( !psg->Init( PSG_CLOCK62, snd->GetSampleRate() ) ) return FALSE;
	
	// 8255(Z80側) -----
	const static IOBus::Connector c_8255[] = {
		{ 0x90, IOBus::portout, PIO6::out90H },
		{ 0x91, IOBus::portout, PIO6::out91H },
		{ 0x92, IOBus::portout, PIO6::out92H },
		{ 0x93, IOBus::portout, PIO6::out93H },
		{ 0x90, IOBus::portin,  PIO6::in90H },
		{ 0x92, IOBus::portin,  PIO6::in92H },
		{ 0x93, IOBus::portin,  PIO6::in93H },
		{ 0, 0, 0 }	};
	if( !io->Connect( pio, c_8255 ) ) return FALSE;
	
	// 8255(SUB CPU側) -----
	const static IOBus::Connector c_8255s[] = {
		{ IO8049_BUS, IOBus::portout, PIO6::outPBH },
		{ IO8049_BUS, IOBus::portin,  PIO6::inPBH  },
		{ IO8049_T0,  IOBus::portin,  PIO6::inIBF  },
		{ IO8049_INT, IOBus::portin,  PIO6::inOBF  },
		{ 0, 0, 0 }	};
	if( !ios->Connect( pio, c_8255s ) ) return FALSE;
	
	// 音声合成 -----
	const static IOBus::Connector c_voice[] = {
		{ 0xe0, IOBus::portout, VCE6::outE0H },
		{ 0xe2, IOBus::portout, VCE6::outE2H },
		{ 0xe3, IOBus::portout, VCE6::outE3H },
		{ 0xe4, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xe6, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xe7, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xe8, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xea, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xeb, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xec, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xee, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xef, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xe0, IOBus::portin,  VCE6::inE0H },
		{ 0xe2, IOBus::portin,  VCE6::inE2H },
		{ 0xe3, IOBus::portin,  VCE6::inE3H },
		{ 0xe4, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xe6, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xe7, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0xe8, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xea, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xeb, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0xec, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xee, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xef, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0, 0, 0 }	};
	if( !io->Connect( voice, c_voice ) ) return FALSE;
	if( !voice->Init( snd->GetSampleRate(), cfg->GetWavePath() ) ) return FALSE;
	snd->ConnectStream( (SndDev *)voice );	// ストリーム接続
	voice->SetVolume( cfg->GetVoiceVol() );	// 音量設定
	
	// DISK -----
	const static IOBus::Connector c_disk[] = {
		{ 0xb1, IOBus::portout, DSK66::outB1H },
		{ 0xb2, IOBus::portout, DSK66::outB2H },
		{ 0xb3, IOBus::portout, DSK66::outB3H },
		{ 0xd0, IOBus::portout, DSK66::outD0H },
		{ 0xd1, IOBus::portout, DSK66::outD1H },
		{ 0xd2, IOBus::portout, DSK66::outD2H },
		{ 0xd3, IOBus::portout, DSK66::outD3H },
		{ 0xd6, IOBus::portout, DSK66::outD6H },
		{ 0xd8, IOBus::portout, DSK66::outD8H },
		{ 0xda, IOBus::portout, DSK66::outDAH },
		{ 0xdd, IOBus::portout, DSK66::outDDH },
		{ 0xde, IOBus::portout, DSK66::outDEH },
		{ 0xb2, IOBus::portin,  DSK66::inB2H },
		{ 0xd0, IOBus::portin,  DSK66::inD0H },
		{ 0xd1, IOBus::portin,  DSK66::inD1H },
		{ 0xd2, IOBus::portin,  DSK66::inD2H },
		{ 0xd3, IOBus::portin,  DSK66::inD3H },
		{ 0xd4, IOBus::portin,  DSK66::inD4H },
		{ 0xdc, IOBus::portin,  DSK66::inDCH },
		{ 0xdd, IOBus::portin,  DSK66::inDDH },
		{ 0, 0, 0 }	};
	if( !io->Connect( (DSK66*)disk, c_disk ) ) return FALSE;
	if( !disk->Init() ) return FALSE;
	// ドライブ1にDISK挿入
	if( *(cfg->GetDiskFile()) ) disk->Mount( 0, cfg->GetDiskFile() );
	
	// CMT(LOAD) -----
	const static IOBus::Connector c_cmt[] = {
		{ 0xb0, IOBus::portout, CMTL::outB0H },
		{ 0, 0, 0 }	};
	if( !io->Connect( cmt, c_cmt ) ) return FALSE;
	
	// オプション機能 -----
	// 戦士のカートリッジ対応
	if( cfg->GetUseSoldier() ){
		const static IOBus::Connector c_soldier[] = {
			{ 0x7f, IOBus::portout, MEM62::out7FH },
			{ 0, 0, 0 }	};
		if( !io->Connect( (MEM66*)mem, c_soldier ) ) return FALSE;
	}
	
	
	Reset();
	
	// スクリーンサイズ変更
	graph->ResizeScreen();
	
	// エラーなし
	Error::SetError( Error::NoError );
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 共通部分初期化
////////////////////////////////////////////////////////////////
BOOL VM6::InitCommon( void )
{
	// 各種オブジェクト確保
	io     = new IO6;											// I/O(Z80)
	ios    = new IO6;											// I/O(SUB CPU)
	snd    = new SND6;											// サウンド
	cpum   = new CPU6 ( this, DEV_ID('C', 'P', 'U', '1') );		// CPU
	cpus   = new SUB6 ( this, DEV_ID('8', '0', '4', '9') );		// SUB CPU
	graph  = new DSP6 ( this, DEV_ID('D', 'I', 'S', 'P') );		// 画面描画
	psg    = new PSG6 ( this, DEV_ID('P', 'S', 'G', '1') );		// PSG
	pio    = new PIO6 ( this, DEV_ID('8', '2', '5', '5') );		// 8255
	cmt    = new CMTL ( this, DEV_ID('T', 'A', 'P', 'E') );		// CMT(LOAD)
	cmts   = new CMTS ( this, DEV_ID('S', 'A', 'V', 'E') );		// CMT(SAVE)
	staw   = new cWndStat( this, DEV_ID('S', 'T', 'A', 'W') );	// ステータスバー
	joy    = new cJoy;
	
	if( !io  || !ios || !snd || !cpum || !cpus || !graph ||
		!psg || !pio || !cmt || !cmts || !staw || !joy )
			return FALSE;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	bp     = new BPoint();										// ブレークポイント
	regw   = new cWndReg( this, DEV_ID('R', 'E', 'G', 'W') );	// レジスタウィンドウ
	memw   = new cWndMem( this, DEV_ID('M', 'E', 'M', 'W') );	// メモリウィンドウ
	monw   = new cWndMon( this, DEV_ID('M', 'O', 'N', 'W') );	// モニタウィンドウ
	
	if( !bp || !regw || !memw || !monw ) return FALSE;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	// コンソール用フォント読込み
	if( !JFont::OpenFont( cfg->GetFontFileZ(), cfg->GetFontFileH() ) ){
		Error::SetError( Error::FontFailed );
		return FALSE;
	}
	
	// パレット
	for( int i=0; i<128; i++ ){
		cfg->GetColor( i, &GPal[i] );
		
		// スキャンライン用カラー設定
		GPal[i+128].r = ( GPal[i].r * cfg->GetScanLineBr() ) / 100;
		GPal[i+128].g = ( GPal[i].g * cfg->GetScanLineBr() ) / 100;
		GPal[i+128].b = ( GPal[i].b * cfg->GetScanLineBr() ) / 100;
	}
	
	// OSD関連初期化
	if( !OSD_Init() ) return FALSE;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	// ブレークポイント　-----
	bp->ClearStatus();
	
	// レジスタウィンドウ　-----
	if( !regw->Init() ) return FALSE;
	
	// メモリウィンドウ　-----
	if( !memw->Init() ) return FALSE;
	
	// モニタウィンドウ　-----
	if( !monw->Init() ) return FALSE;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	// I/O(Z80)　-----
	if( !io->Init( 256 ) ) return FALSE;
	
	// I/O(SUB CPU)　-----
	if( !ios->Init( 10 ) ) return FALSE;
	
	// 割込み -----
	intr->Reset();
	
	// スケジューラ -----
	if( !sche->Init() ) return FALSE;
	
	// サウンド -----
	if( !snd->Init( this, VM6::StreamUpdate, cfg->GetSampleRate(), cfg->GetSoundBuffer() ) ) return FALSE;
	snd->SetVolume( cfg->GetMasterVol() );
	
	// CPU -----
	cpum->Reset();
	
	// SUB CPU -----
	cpus->Reset();
	
	// メモリ -----
	if( !mem->Init( cfg->GetRomPath(), cfg->GetCheckCRC(), cfg->GetUseSoldier() ) ) return FALSE;
	if( cfg->GetRomPatch() ) mem->Patch();
	if( *(cfg->GetExtRomFile()) ) if( !mem->MountExtRom( cfg->GetExtRomFile() ) ) return FALSE;
	
	// VDG -----
	if( !vdg->Init() ) return FALSE;
	vdg->SetPalette( GPal );				// 念のためパレット設定
	vdg->SetMode4Color( cfg->GetMode4Color() );
	
	// 画面描画 -----
	if( !graph->Init( cfg->GetScrBpp(), cfg->GetScanLineBr(), GPal ) ) return FALSE;
	
	// PSG -----
	snd->ConnectStream( (SndDev *)psg );	// ストリーム接続
	psg->SetVolume( cfg->GetPsgVol() );		// 音量設定
	psg->SetLPF( cfg->GetPsgLPF() );		// ローパスフィルタ カットオフ周波数設定
	
	// 8255 -----
	pio->Reset();
	
	// キー -----
	if( !key->Init( cfg->GetKeyRepeat() ) ) return FALSE;
	VKeyConv *vk;
	if( cfg->GetVKeyDef( &vk ) )	// キー定義配列取得
		key->SetVKeySymbols( vk );	// 仮想キーコード -> P6キーコード 設定
	
	// CMT(LOAD) -----
	if( !cmt->Init( snd->GetSampleRate() ) ) return FALSE;
	snd->ConnectStream( (SndDev *)cmt );	// ストリーム接続
	cmt->SetVolume( cfg->GetCmtVol() );		// 音量設定
	cmt->SetLPF( DEFAULT_TAPELPF );			// ローパスフィルタ カットオフ周波数設定
	cmt->SetBoost( cfg->GetBoostUp() );
	cmt->SetMaxBoost( cfg->GetMaxBoost1(), cfg->GetMaxBoost2() );
	
	// TAPE挿入
	if( *(cfg->GetTapeFile()) ) cmt->Mount( cfg->GetTapeFile() );
	
	// CMT(SAVE) -----
	if( !cmts->Init( cfg->GetSaveFile() ) ) return FALSE;
	
	// ステータスバー -----
	if( !staw->Init( SDL_GetVideoSurface()->w ) ) return FALSE;
	
	// ジョイスティック -----
	joy->Init();
	
	// FPS表示タイマ設定
	if( !SetFPSTimer( 1 ) ){
		Stop();
		return FALSE;
	}
	
	// ビデオキャプチャ -----
	if( !AVI6::Init() ) return FALSE;
	
	// リプレイ -----
	if( !REPLAY::Init( key->GetMatrixSize() ) ) return FALSE;
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 動作開始
////////////////////////////////////////////////////////////////
BOOL VM6::Start( void )
{
	FSkipCount = 0;
	
	// スレッド生成
	if( !this->cThread::BeginThread( this ) ) return FALSE;
	
	sche->Start();
	snd->Play();
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 動作停止
////////////////////////////////////////////////////////////////
void VM6::Stop( void )
{
	this->cThread::Cancel();	// スレッド終了フラグ立てる
	this->cThread::Waiting();	// スレッド終了まで待つ
	
	snd->Pause();
	sche->Stop();
	
	FSkipCount = 0;
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void VM6::Reset( void )
{
	cpum->Reset();
	intr->Reset();
	mem->Reset();
	cpus->Reset();
	pio->Reset();
	psg->Reset();
	disk->Init();
	cmt->Reset();
	// システムディスクが入っていたらTAPEのオートスタート無効
	if( !disk->IsSystem(0) && !disk->IsSystem(1) ) cmt->SetAutoStart( cfg->GetModel() );
	if( voice ) voice->Reset();
}


////////////////////////////////////////////////////////////////
// イベントループ
////////////////////////////////////////////////////////////////
VM6::ReturnCode VM6::EventLoop( void )
{
	SDL_Event event;

        while( SDL_WaitEvent( &event ) ){
                switch( event.type ){
		case SDL_FPSUPDATE:		// FPS表示
			{
			char str[256];
			sprintf( str, "%s (%d%%  %d/%d fps)", cfg->GetCaption(), sche->GetRatio(), event.user.code, FRAMERATE );
			SDL_WM_SetCaption( str, "" );
			}
			break;
			
		case SDL_KEYDOWN:		// キー入力
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
			// モニタモード?
			if( cfg->GetMonDisp() ){
				monw->KeyIn( OSD_ConvertKeyCode( event.key.keysym.sym ), event.key.keysym.mod & KMOD_SHIFT, event.key.keysym.unicode );
				break;
			}else
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
			// 各種機能キーチェック
			if( CheckFuncKey( &event ) ) break;
			// リプレイ再生中 or 自動キー入力実行中でなければ
			if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() )
				// キーマトリクス更新(キー)
				key->UpdateMatrixKey( OSD_ConvertKeyCode( event.key.keysym.sym ), TRUE );
			break;
			
		case SDL_KEYUP:
			// リプレイ再生中 or 自動キー入力実行中でなければ
			if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() )
				// キーマトリクス更新(キー)
				key->UpdateMatrixKey( OSD_ConvertKeyCode( event.key.keysym.sym ), FALSE );
			break;
			
		case SDL_JOYAXISMOTION:
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			// リプレイ再生中 or 自動キー入力実行中でなければ
			if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() )
				// キーマトリクス更新(ジョイスティック)
				key->UpdateMatrixJoy();
			break;
			
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
		case SDL_DEBUGMODEBP:	// モニタモード変更(ブレークポイント到達時)
			monw->BreakIn( event.user.code );	// ブレークポイントの情報を表示
			ToggleMonitor();					// モニタモード変更
			
			break;
			
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
		case SDL_MOUSEBUTTONUP:	// マウスボタンクリック
			if( ( event.button.button == SDL_BUTTON_RIGHT ) && ( event.button.state == SDL_RELEASED ) ){
                                Stop();
				
				// ポップアップメニュー表示
				ShowPopupMenu( event.button.x, event.button.y );
				
                                Start();
			}
			break;
			
		case SDL_QUIT:			// 終了
			if( cfg->GetCkQuit() )
				if( OSD_Message( MSG_QUIT, MSG_QUITC, OSDM_YESNO | OSDM_ICONQUESTION ) != OSDR_YES )
					break;
			return Quit;
			
		case SDL_RESTART:		// 再起動
			return Restart;
		}
		
		// エラー処理
//		if( Error::GetError() != Error::NoError ){
//			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
//			Error::SetError( Error::NoError );
//		}
		switch( Error::GetError() ){
		case Error::NoError:
			break;
			
		case Error::DokoDiffModel:
			Error::SetError( Error::NoError );
			return Doko;
			
		case Error::ReplayDiffModel:
			Error::SetError( Error::NoError );
			return Replay;
			
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
BOOL VM6::CheckFuncKey( SDL_Event *event )
{
	// ALTキーの状態を取得
	BOOL OnALT  = event->key.keysym.mod & KMOD_ALT  ? TRUE : FALSE;
#ifdef __APPLE__
	// METAキーの状態を取得
	BOOL OnMETA = event->key.keysym.mod & KMOD_META ? TRUE : FALSE;
#endif
	
	switch( event->key.keysym.sym ){	// キーコード
	case SDLK_F6:		// モニタモード変更 or スクリーンモード変更
		// ビデオキャプチャ中は無効
		if( AVI6::IsAVI() ) return FALSE;
		
		if( OnALT ){
			Stop();
			cfg->SetFullScreen( cfg->GetFullScreen() ? FALSE : TRUE );
			graph->ResizeScreen();	// スクリーンサイズ変更
			Start();
		}else{
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
			ToggleMonitor();
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
		}
		break;
		
	case SDLK_F7:			// スキャンラインモード変更
		// ビデオキャプチャ中は無効
		if( AVI6::IsAVI() ) return FALSE;
		
		Stop();
		if( OnALT ){
			cfg->SetDispNTSC( cfg->GetDispNTSC() ? FALSE : TRUE );
		}else{
			cfg->SetScanLine( cfg->GetScanLine() ? FALSE : TRUE );
		}
		graph->ResizeScreen();	// スクリーンサイズ変更
		Start();
		break;
		
	case SDLK_F8:			// モード4カラー変更 or ステータスバー表示状態変更
		if( OnALT ){
			Stop();
			cfg->SetStatDisp( cfg->GetStatDisp() ? FALSE : TRUE );
			graph->ResizeScreen();	// スクリーンサイズ変更
			Start();
		}else{
			int c = vdg->GetMode4Color();
			if( ++c > 4 ) c = 0;
			vdg->SetMode4Color( c );
		}
		break;
		
	case SDLK_F10:			// Wait有効無効変更
		if( OnALT ){
		}else{
			sche->SetWaitEnable( sche->GetWaitEnable() ? FALSE : TRUE );
		}
		break;
		
	case SDLK_F11:			// リセット or 再起動
		if( OnALT ){
			SENDEVENT( SDL_RESTART )
		}else{
			Reset();
		}
		break;
		
	case SDLK_F12:			// スナップショット
		if( OnALT ){
		}else{
			graph->SnapShot( cfg->GetImgPath() );
		}
		break;
		
#ifdef __APPLE__
	// Mac では Cmd(Meta)+Q がアプリケーションの終了だそうな (by Windyさん)
	case SDLK_q:
		if( OnMETA ){
			SENDEVENT( SDL_QUIT );
			break;	// break がないと、Cmd+Q を押したときに、Q が入力されてしまう。   add Windy  2007/4/24
		}
		
	// Cmd(Meta)＋Spaceでも、かなキーとして動作させる。(by Windy）
	case SDLK_SPACE:
		if( OnMETA ){
			event->key.keysym.sym = SDLK_PAUSE;
		}
#endif
		
	default:				// どれでもない
		return FALSE;
	}
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 画面更新
////////////////////////////////////////////////////////////////
BOOL VM6::ScreenUpdate( void )
{
	// フレームスキップチェック
	if( ++FSkipCount > cfg->GetFrameSkip() ){
		vdg->UpdateBackBuf();	// バックバッファ更新
		graph->DrawScreen();	// 画面描画
		
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
		if( cfg->GetMonDisp() ){
			regw->Update();
			memw->Update();
			monw->Update();
		}
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
		
		UDFPSCount++;
		FSkipCount = 0;
		
		return TRUE;
	}
	return FALSE;
}


////////////////////////////////////////////////////////////////
// サウンド更新
//
// 引数:	samples		更新するサンプル数(-1:残りバッファ全て 0:処理クロック分)
//			exbuf		外部バッファ
// 返値:	int			更新したサンプル数
////////////////////////////////////////////////////////////////
int VM6::SoundUpdate( int samples, cRing *exbuf )
{
	// PSG更新
	psg->SoundUpdate( samples );
	
	// 音声合成更新
//	if( voice ) voice->SoundUpdate( samples );
	
	// サウンドバッファ更新
	return snd->PreUpdate( psg->SndDev::cRing::ReadySize(), exbuf );
}


////////////////////////////////////////////////////////////////
// ストリーム更新 コールバック関数
//
// 引数:	userdata	自分自身へのオブジェクトポインタ
//			stream		ストリーム書込みバッファへのポインタ
//			len			バッファサイズ(バイト単位)
// 返値:	なし
////////////////////////////////////////////////////////////////
void VM6::StreamUpdate( void *userdata, BYTE *stream, int len )
{
	VM6 *p6 = (VM6 *)userdata;			// 自分自身のオブジェクトポインタ取得
	
	// サウンドバッファ更新
	//  もしサンプル数が足りなければここで追加
	//  ただしビデオキャプチャ中は無視
	int addsam = (int)(len/sizeof(Sint16)) - (int)(p6->snd->cRing::ReadySize());
	if( addsam > 0 && !p6->AVI6::IsAVI() ) p6->SoundUpdate( addsam );
	
	p6->snd->Update( stream, len/sizeof(Sint16) );
}


////////////////////////////////////////////////////////////////
// FPS表示タイマ コールバック関数
////////////////////////////////////////////////////////////////
Uint32 VM6::UpDateFPS( Uint32 interval, void *obj )
{
	VM6 *p6 = (VM6 *)obj;	// 自分自身のオブジェクトポインタ取得
	
	SENDEVENT1( SDL_FPSUPDATE, p6->UDFPSCount )
	p6->UDFPSCount = 0;
	
	return interval;
}


////////////////////////////////////////////////////////////////
// FPS表示タイマ設定
////////////////////////////////////////////////////////////////
BOOL VM6::SetFPSTimer( int fps )
{
	// タイマ稼動中なら停止
	if( UpDateFPSID ){
		SDL_RemoveTimer( UpDateFPSID );
		UpDateFPSID = NULL;
		SDL_EventState( SDL_FPSUPDATE, SDL_IGNORE );
	}
	
	// タイマ設定
	if( fps > 0 ){
		UpDateFPSID = SDL_AddTimer( 1000/fps, VM6::UpDateFPS, this );
		if( !UpDateFPSID ) return FALSE;
		
		SDL_EventState( SDL_FPSUPDATE, SDL_ENABLE );
	}
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 自動キー入力実行中?
//
// 引数:	なし
// 返値:	BOOL	TRUE:実行中 FALSE:実行中でない
////////////////////////////////////////////////////////////////
BOOL VM6::IsAutoKey( void )
{
	return ak.Num ? TRUE : FALSE;
}


////////////////////////////////////////////////////////////////
// 自動キー入力1文字取得
//   (VSYNC=1/60sec毎に呼ばれる)
//
// 引数:	なし
// 返値:	BYTE	P6のキーコード
////////////////////////////////////////////////////////////////
char VM6::GetAutoKey( void )
{
	// リレーOFF待ち
	if( ak.Relay ){
		if( cmt->IsRelay() ) return 0;
		else                 ak.Relay = FALSE;
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
			ak.Relay = TRUE;
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
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL VM6::SetAutoKey( char *str, int num )
{
	// キーバッファを一旦削除
	
	// キーバッファ確保
	if( ak.Buffer ) delete [] ak.Buffer;
	ak.Buffer = new char[num+1];
	if( !ak.Buffer ){
		ak.Num = 0;
		return FALSE;
	}
	
	// 文字列をコピー
	strncpy( ak.Buffer, str, num );
	
	ak.Num    = num;	// 残り文字数
	ak.Wait   = 60;		// 待ち回数カウンタ(初回は1sec)
	ak.Relay  = FALSE;	// リレースイッチOFF待ちフラグ
	ak.Seek   = 0;		// 読込みポインタ
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 自動キー入力文字列設定(ファイルから)
//
// 引数:	filepath	入力ファイルへのパス
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL VM6::SetAutoKeyFile( char *filepath )
{
	FILE *fp;
	char lbuf[1024];
	
	fp = FOPENEN( filepath, "rb" );
	if( !fp ) return FALSE;
	
	fseek( fp, 0, SEEK_END );
	int tsize = ftell( fp ) + 1;
	fseek( fp, 0, SEEK_SET );
	
	// キーバッファ確保
	if( ak.Buffer ) delete [] ak.Buffer;
	ak.Buffer = new char[tsize+1];
	if( !ak.Buffer ){
		fclose( fp );
		ak.Num = 0;
		return FALSE;
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
	ak.Relay  = FALSE;	// リレースイッチOFF待ちフラグ
	ak.Seek   = 0;		// 読込みポインタ
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	filename	ファイル名
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL VM6::DokoDemoSave( char *filename )
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
			!sche->DokoSave( Ini ) ||
			!psg->DokoSave( Ini ) ||
			!disk->DokoSave( Ini ) ||
			!vdg->DokoSave( Ini ) ||
			!intr->DokoSave( Ini ) ||
			!key->DokoSave( Ini ) ||
			!cpus->DokoSave( Ini ) ||
			!pio->DokoSave( Ini ) ||
			!cpum->DokoSave( Ini ) ||
			!mem->DokoSave( Ini ) ||
			!cmt->DokoSave( Ini )
		) throw Error::GetError();
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return FALSE;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		if( Ini ) delete Ini;
		return FALSE;
	}
	
	Ini->Write();
	
	delete Ini;
	
	// 無事だったのでエラーなし
	Error::SetError( Error::NoError );
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	filename	ファイル名
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL VM6::DokoDemoLoad( char *filename )
{
	cIni *Ini = NULL;
	
	// とりあえずエラー設定
	Error::SetError( Error::DokoReadFailed );
	try{
		// どこでもLOADファイルを開く
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::DokoReadFailed;
		cfg->SetDokoFile( filename );
		
		// まずはPC6001Vのバージョンと機種を確認
		if( !cfg->DokoLoad( Ini ) ) throw Error::GetError();
		
		Reset();
		
		if(	!sche->DokoLoad( Ini ) ||
			!psg->DokoLoad( Ini ) ||
			!disk->DokoLoad( Ini ) ||
			!vdg->DokoLoad( Ini ) ||
			!intr->DokoLoad( Ini ) ||
			!key->DokoLoad( Ini ) ||
			!cpus->DokoLoad( Ini ) ||
			!pio->DokoLoad( Ini ) ||
			!cpum->DokoLoad( Ini ) ||
			!mem->DokoLoad( Ini ) ||
			!cmt->DokoLoad( Ini )
		) throw Error::GetError();
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return FALSE;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		if( Ini ) delete Ini;
		return FALSE;
	}
	
	delete Ini;
	cfg->SetDokoFile( "" );
	
	// 無事だったのでエラーなし
	Error::SetError( Error::NoError );
	
	return TRUE;
}




