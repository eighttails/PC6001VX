/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <new>

#include "pc6001v.h"

#include "breakpoint.h"
#include "config.h"
#include "cpum.h"
#include "cpus.h"
#include "debug.h"
#include "disk.h"
#include "error.h"
#include "graph.h"
#include "intr.h"
#include "io.h"
#include "keyboard.h"
#include "log.h"
#include "memory.h"
#include "pio.h"
#include "psgfm.h"
#include "schedule.h"
#include "sound.h"
#include "tape.h"
#include "voice.h"
#include "vdg.h"

#include "p6el.h"
#include "p6vm.h"


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
VM6::VM6( void ) : cclock( 0 ), pclock( 0 ), evsc( nullptr ),
	iom( nullptr ), ios( nullptr ), intr( nullptr ), cpum( nullptr ), cpus( nullptr ),
	mem( nullptr ), vdg( nullptr ), psg( nullptr ), voice( nullptr ), pio( nullptr ),
	key( nullptr ), cmtl( nullptr ), cmts( nullptr ), disk( nullptr )
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	, bp( nullptr ), MonDisp( false )
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
{
	PRINTD( CONST_LOG, "[[VM6-" );
}

VM60::VM60( void )
{
	PRINTD( CONST_LOG, "VM60]]\n" );
	
	cclock = CPUM_CLOCK60;
	pclock = PSG_CLOCK60;
	
	DevTable.Intr    = &VM60::c_intr;		// 割込み
	DevTable.Vdg     = &VM60::c_vdg;		// VDG
	DevTable.Psg     = &VM60::c_psg;		// PSG
	DevTable.M8255   = &VM60::c_8255m;		// I/O(Z80側)
	DevTable.S8255   = &VM60::c_8255s;		// I/O(SUB CPU側)
	DevTable.Disk    = &VM60::c_disk;		// DISK
	DevTable.CmtL    = &VM60::c_cmtl;		// CMT(LOAD)
}

VM61::VM61( void )
{
	PRINTD( CONST_LOG, "VM61]]\n" );
	
	cclock = CPUM_CLOCK60;
	pclock = PSG_CLOCK60;
	
	DevTable.Intr    = &VM60::c_intr;		// 割込み
	DevTable.Vdg     = &VM60::c_vdg;		// VDG
	DevTable.Psg     = &VM60::c_psg;		// PSG
	DevTable.M8255   = &VM60::c_8255m;		// I/O(Z80側)
	DevTable.S8255   = &VM60::c_8255s;		// I/O(SUB CPU側)
	DevTable.Disk    = &VM60::c_disk;		// DISK
	DevTable.CmtL    = &VM60::c_cmtl;		// CMT(LOAD)
}

VM62::VM62( void )
{
	PRINTD( CONST_LOG, "VM62]]\n" );
	
	cclock = CPUM_CLOCK60;
	pclock = PSG_CLOCK60;
	
	DevTable.Intr    = &VM62::c_intr;		// 割込み
	DevTable.Memory  = &VM62::c_memory;		// メモリ
	DevTable.Vdg     = &VM62::c_vdg;		// VDG
	DevTable.Psg     = &VM62::c_psg;		// PSG
	DevTable.M8255   = &VM62::c_8255m;		// I/O(Z80側)
	DevTable.S8255   = &VM62::c_8255s;		// I/O(SUB CPU側)
	DevTable.Voice   = &VM62::c_voice;		// 音声合成
	DevTable.Disk    = &VM62::c_disk;		// DISK
	DevTable.CmtL    = &VM62::c_cmtl;		// CMT(LOAD)
}

VM66::VM66( void )
{
	PRINTD( CONST_LOG, "VM66]]\n" );
	
	cclock = CPUM_CLOCK66;
	pclock = PSG_CLOCK66;
	
	DevTable.Intr    = &VM62::c_intr;		// 割込み
	DevTable.Memory  = &VM62::c_memory;		// メモリ
	DevTable.Vdg     = &VM62::c_vdg;		// VDG
	DevTable.Psg     = &VM62::c_psg;		// PSG
	DevTable.M8255   = &VM62::c_8255m;		// I/O(Z80側)
	DevTable.S8255   = &VM62::c_8255s;		// I/O(SUB CPU側)
	DevTable.Voice   = &VM62::c_voice;		// 音声合成
	DevTable.Disk    = &VM66::c_disk;		// DISK
	DevTable.CmtL    = &VM62::c_cmtl;		// CMT(LOAD)
}

VM64::VM64( void )
{
	PRINTD( CONST_LOG, "VM64]]\n" );
	
	cclock = CPUM_CLOCK64;
	pclock = PSG_CLOCK64;
	
	DevTable.Intr    = &VM64::c_intr;		// 割込み
	DevTable.Memory  = &VM64::c_memory;		// メモリ
	DevTable.Vdg     = &VM64::c_vdg;		// VDG
	DevTable.Psg     = &VM64::c_psg;		// PSG
	DevTable.M8255   = &VM64::c_8255m;		// I/O(Z80側)
	DevTable.S8255   = &VM64::c_8255s;		// I/O(SUB CPU側)
	DevTable.Voice   = &VM64::c_voice;		// 音声合成
	DevTable.Disk    = &VM64::c_disk;		// DISK
	DevTable.CmtL    = &VM64::c_cmtl;		// CMT(LOAD)
}

VM68::VM68( void )
{
	PRINTD( CONST_LOG, "VM68]]\n" );
	
	cclock = CPUM_CLOCK64;
	pclock = PSG_CLOCK64;
	
	DevTable.Intr    = &VM64::c_intr;		// 割込み
	DevTable.Memory  = &VM64::c_memory;		// メモリ
	DevTable.Vdg     = &VM64::c_vdg;		// VDG
	DevTable.Psg     = &VM64::c_psg;		// PSG
	DevTable.M8255   = &VM64::c_8255m;		// I/O(Z80側)
	DevTable.S8255   = &VM64::c_8255s;		// I/O(SUB CPU側)
	DevTable.Voice   = &VM64::c_voice;		// 音声合成
	DevTable.Disk    = &VM68::c_disk;		// DISK
	DevTable.CmtL    = &VM64::c_cmtl;		// CMT(LOAD)
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
VM6::~VM6( void )
{
	PRINTD( CONST_LOG, "-~VM6]]\n" );
}

VM60::~VM60( void )
{
	PRINTD( CONST_LOG, "[[~VM60" );
}

VM61::~VM61( void )
{
	PRINTD( CONST_LOG, "[[~VM61" );
}

VM62::~VM62( void )
{
	PRINTD( CONST_LOG, "[[~VM62" );
}

VM66::~VM66( void )
{
	PRINTD( CONST_LOG, "[[~VM66" );
}

VM64::~VM64( void )
{
	PRINTD( CONST_LOG, "[[~VM64" );
}

VM68::~VM68( void )
{
	PRINTD( CONST_LOG, "[[~VM68" );
}


/////////////////////////////////////////////////////////////////////////////
// 機種別オブジェクト確保
/////////////////////////////////////////////////////////////////////////////
void VM60::AllocObjSpec( void )
{
	cpus  = std::make_shared<SUB60>( this, DEV_ID("8049") );	// SUB CPU
	intr  = std::make_shared<IRQ60>( this, DEV_ID("INTR") );	// 割込み
	mem   = std::make_shared<MEM60>( this, DEV_ID("MEM1") );	// メモリ
	vdg   = std::make_shared<VDG60>( this, DEV_ID("VDG1") );	// VDG
	key   = std::make_shared<KEY60>( this, DEV_ID("KEYB") );	// キー
	psg   = std::make_shared<PSG60>( this, DEV_ID("PSG1") );	// PSG
	voice = std::make_shared<VCE60>( this, DEV_ID("VCE1") );	// 音声合成
	disk  = std::make_shared<DSK60>( this, DEV_ID("DSK1") );	// DISK
}

void VM61::AllocObjSpec( void )
{
	cpus  = std::make_shared<SUB60>( this, DEV_ID("8049") );	// SUB CPU
	intr  = std::make_shared<IRQ60>( this, DEV_ID("INTR") );	// 割込み
	mem   = std::make_shared<MEM61>( this, DEV_ID("MEM1") );	// メモリ
	vdg   = std::make_shared<VDG60>( this, DEV_ID("VDG1") );	// VDG
	key   = std::make_shared<KEY60>( this, DEV_ID("KEYB") );	// キー
	psg   = std::make_shared<PSG60>( this, DEV_ID("PSG1") );	// PSG
	voice = std::make_shared<VCE60>( this, DEV_ID("VCE1") );	// 音声合成
	disk  = std::make_shared<DSK60>( this, DEV_ID("DSK1") );	// DISK
}

void VM62::AllocObjSpec( void )
{
	cpus  = std::make_shared<SUB62>( this, DEV_ID("8049") );	// SUB CPU
	intr  = std::make_shared<IRQ62>( this, DEV_ID("INTR") );	// 割込み
	mem   = std::make_shared<MEM62>( this, DEV_ID("MEM1") );	// メモリ
	vdg   = std::make_shared<VDG62>( this, DEV_ID("VDG2") );	// VDG
	key   = std::make_shared<KEY62>( this, DEV_ID("KEYB") );	// キー
	psg   = std::make_shared<PSG60>( this, DEV_ID("PSG1") );	// PSG
	voice = std::make_shared<VCE62>( this, DEV_ID("VCE2") );	// 音声合成
	disk  = std::make_shared<DSK60>( this, DEV_ID("DSK1") );	// DISK
}

void VM66::AllocObjSpec( void )
{
	cpus  = std::make_shared<SUB62>( this, DEV_ID("8049") );	// SUB CPU
	intr  = std::make_shared<IRQ62>( this, DEV_ID("INTR") );	// 割込み
	mem   = std::make_shared<MEM66>( this, DEV_ID("MEM1") );	// メモリ
	vdg   = std::make_shared<VDG62>( this, DEV_ID("VDG2") );	// VDG
	key   = std::make_shared<KEY62>( this, DEV_ID("KEYB") );	// キー
	psg   = std::make_shared<PSG60>( this, DEV_ID("PSG1") );	// PSG
	voice = std::make_shared<VCE62>( this, DEV_ID("VCE2") );	// 音声合成
	disk  = std::make_shared<DSK66>( this, DEV_ID("DSK3") );	// DISK
}

void VM64::AllocObjSpec( void )
{
	cpus  = std::make_shared<SUB62>( this, DEV_ID("8049") );	// SUB CPU
	intr  = std::make_shared<IRQ64>( this, DEV_ID("INTR") );	// 割込み
	mem   = std::make_shared<MEM64>( this, DEV_ID("MEM1") );	// メモリ
	vdg   = std::make_shared<VDG64>( this, DEV_ID("VDG3") );	// VDG
	key   = std::make_shared<KEY62>( this, DEV_ID("KEYB") );	// キー
	psg   = std::make_shared<OPN64>( this, DEV_ID("OPN1") );	// OPN
	voice = std::make_shared<VCE64>( this, DEV_ID("VCE3") );	// 音声合成
	disk  = std::make_shared<DSK64>( this, DEV_ID("DSK2") );	// DISK
}

void VM68::AllocObjSpec( void )
{
	cpus  = std::make_shared<SUB68>( this, DEV_ID("8049") );	// SUB CPU
	intr  = std::make_shared<IRQ64>( this, DEV_ID("INTR") );	// 割込み
	mem   = std::make_shared<MEM68>( this, DEV_ID("MEM1") );	// メモリ
	vdg   = std::make_shared<VDG64>( this, DEV_ID("VDG3") );	// VDG
	key   = std::make_shared<KEY62>( this, DEV_ID("KEYB") );	// キー
	psg   = std::make_shared<OPN64>( this, DEV_ID("OPN1") );	// OPN
	voice = std::make_shared<VCE64>( this, DEV_ID("VCE3") );	// 音声合成
	disk  = std::make_shared<DSK68>( this, DEV_ID("DSK4") );	// DISK
}


/////////////////////////////////////////////////////////////////////////////
// 全オブジェクト確保
/////////////////////////////////////////////////////////////////////////////
bool VM6::AllocObject( const std::shared_ptr<CFG6>& cnfg )
{
	PRINTD( VM_LOG, "[VM][AllocObject]\n" );
	
	try{
		// 各種オブジェクト確保
		evsc = std::make_unique<EVSC>();						// イベントスケジューラ
		iom  = std::make_unique<IO6>();  						// I/O(Z80)
		ios  = std::make_unique<IO6>();  						// I/O(SUB CPU)
		
		cpum = std::make_shared<CPU6>( this, DEV_ID("CPU1") );	// CPU
		pio  = std::make_shared<PIO6>( this, DEV_ID("8255") );	// 8255
		cmtl = std::make_shared<CMTL>( this, DEV_ID("TAPE") );	// CMT(LOAD)
		cmts = std::make_shared<CMTS>( this, DEV_ID("SAVE") );	// CMT(SAVE)
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		bp   = std::make_unique<BPoint>();			// ブレークポイント
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		
		// 機種別オブジェクト確保
		AllocObjSpec();
		
		
		// 内部メモリ確保とROMファイル読込み
		if( !mem->AllocMemoryInt( cnfg->GetValue( CF_RomPath ), cnfg->GetValue( CB_CheckCRC ) ) ) throw Error::GetError();
	}
	catch( std::bad_alloc& ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// 例外発生
		return false;
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool VM6::Init( const std::shared_ptr<CFG6>& cnfg  )
{
	// 全オブジェクト確保
	if( !AllocObject( cnfg ) ) return false;
	
	// イベントスケジューラ
	evsc->SetMasterClock( cclock * cnfg->GetValue( CV_OverClock ) / 100 );
	const std::vector<std::shared_ptr<IDevice>> devs = { intr, cpum, cpus, vdg, psg, voice, pio, key, cmtl, cmts, disk };
	evsc->Entry( devs );
	
	// I/O(Z80)　-----
	if( !iom->Init( 256 ) ) return false;
	
	// I/O(SUB CPU)　-----
	if( !ios->Init( 10 ) ) return false;
	
	// 割込み -----
	intr->Reset();
	
	// CPU -----
	cpum->Reset();
	
	// SUB CPU -----
	cpus->Reset();
	
	// メモリ -----
	if( !mem->InitInt() ) return false;
	mem->Reset();
	
	// VDG -----
	if( !vdg->Init() ) return false;
	vdg->SetMode4Color( cnfg->GetValue( CV_Mode4Color ) );
	
	// PSG/OPN -----
	psg->SetVolume( cnfg->GetValue( CV_PsgVolume ) );	// 音量設定
	psg->SetLPF( cnfg->GetValue( CV_PsgLPF ) );			// ローパスフィルタ カットオフ周波数設定
	for( auto &i : *DevTable.Psg ){						// ウェイト設定
		if( i.rule == IOBus::portout ) iom->SetOutWait( i.bank, 1 );
		else						   iom->SetInWait ( i.bank, 1 );
	}
	if( !psg->Init( pclock, cnfg->GetValue( CV_SampleRate ) ) ) return false;
	
	// 8255 -----
	pio->Reset();
	pio->cPRT::SetFile( cnfg->GetValue( CF_Printer ) );
	
	// キー -----
	if( !key->Init() ) return false;
	std::vector<VKeyConv> vk;
	if( cnfg->GetVKeyDef( vk ) )				// キー定義配列取得
		key->SetVKeySymbols( vk );				// 仮想キーコード -> P6キーコード 設定
	
	// CMT(LOAD) -----
	if( !cmtl->Init( cnfg->GetValue( CV_SampleRate ) ) ) return false;
	cmtl->SetVolume( cnfg->GetValue( CV_TapeVolume ) );	// 音量設定
	cmtl->SetLPF( cnfg->GetValue( CV_TapeLPF ) );		// ローパスフィルタ カットオフ周波数設定
	cmtl->SetBoost( cnfg->GetValue( CB_BoostUp ) );
	cmtl->SetMaxBoost( cnfg->GetValue( CV_MaxBoost60 ), cnfg->GetValue( CV_MaxBoost62 ) );
	cmtl->SetStopBit( cnfg->GetValue( CV_StopBit ) );	// ストップビット数
	
	// CMT(SAVE) -----
	if( !cmts->Init( cnfg->GetValue( CF_Save ) ) ) return false;
	
	// DISK -----
	if( !disk->Init( cnfg->GetValue( CV_FDDrive ) ) ) return false;
	disk->WaitEnable( cnfg->GetValue( CB_FDDWait ) );
	
	// 音声合成 -----
	if( DevTable.Voice ){
		if( !voice->Init( cnfg->GetValue( CV_SampleRate ) ) ) return false;
		voice->SetVolume( cnfg->GetValue( CV_VoiceVolume ) );	// 音量設定
		voice->SetPath( cnfg->GetValue( CF_WavePath ) );
	}
	
	
	// I/Oポートにデバイスを接続
	if( !iom->Connect( pio,  *DevTable.M8255 ) ) return false;	// 8255(Z80側)
	if( !ios->Connect( pio,  *DevTable.S8255 ) ) return false;	// 8255(SUB CPU側)
	if( !iom->Connect( cmtl, *DevTable.CmtL  ) ) return false;	// CMT(LOAD)
	if( !iom->Connect( intr, *DevTable.Intr  ) ) return false;	// 割込み
	if( !iom->Connect( vdg,  *DevTable.Vdg   ) ) return false;	// VDG
	if( !iom->Connect( psg,  *DevTable.Psg   ) ) return false;	// PSG/OPN
	if( cnfg->GetValue( CV_FDDrive ) || (cnfg->GetValue( CV_Model ) == 66) || (cnfg->GetValue( CV_Model ) == 68) ){	// DISK
		if( !iom->Connect( disk,  *DevTable.Disk   ) ){
			return false;
		}
	}
	if( DevTable.Memory ){										// メモリ
		if( !iom->Connect( mem,   *DevTable.Memory ) ){
			return false;
		}
	}
	if( DevTable.Voice ){										// 音声合成
		if( !iom->Connect( voice, *DevTable.Voice  ) ){
			return false;
		}
	}
	
	
	// 拡張カートリッジ ========================================
	// 拡張カートリッジマウント
	if( cnfg->GetValue( CV_ExCartridge ) ){
		if( !mem->MountExtCart( cnfg->GetValue( CV_ExCartridge ), cnfg->GetValue( CF_RomPath ), cnfg->GetValue( CB_CheckCRC ) ) ){
			return false;
		}
		
		// ROMファイル読込み
		if( !mem->MountExtRom( cnfg->GetValue( CF_ExtRom ) ) ){
			// 失敗したらエラーをセットして継続
			Error::SetError( Error::ExtRomMountFailed, P6VPATH2STR( cnfg->GetValue( CF_ExtRom ) ) );
		}
		
		// I/Oポートにデバイスを接続
		if( !iom->Connect( mem, mem->GetDeviceConnector() ) ){
			return false;
		}
	}
	// =========================================================
	
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// リセット
/////////////////////////////////////////////////////////////////////////////
void VM6::Reset( void )
{
	PRINTD( VM_LOG, "[VM][Reset]\n" );
	
	intr->Reset();
	cpum->Reset();
	cpus->Reset();
	mem->Reset();
	vdg->Reset();
	psg->Reset();
	pio->Reset();
	key->Reset();
	cmtl->Rewind();
	disk->Reset();
	voice->Reset();
}


/////////////////////////////////////////////////////////////////////////////
// 1命令実行
/////////////////////////////////////////////////////////////////////////////
int VM6::Emu( void )
{
	PRINTD( VM_LOG, "[VM][Emu]\n" );
	
	return cpum->Exec();	// CPU 1命令実行
}


/////////////////////////////////////////////////////////////////////////////
// CPUクロック取得
/////////////////////////////////////////////////////////////////////////////
int VM6::GetCPUClock( void ) const
{
	PRINTD( VM_LOG, "[VM][GetCPUClock]\n" );
	
	return cclock;
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// モニタモード?
//
// 引数:	なし
// 返値:	bool		true:モニタモード false:実行中
/////////////////////////////////////////////////////////////////////////////
bool VM6::IsMonitor( void ) const
{
	return MonDisp;
}


/////////////////////////////////////////////////////////////////////////////
// モニタモード設定
//
// 引数:	mon			true:モニタモード false:通常モード
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VM6::SetMonitor( bool mon )
{
	MonDisp = mon;
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@




// =============================================================
// P6デバイス用関数群
// =============================================================


// EVSC --------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// イベント追加
/////////////////////////////////////////////////////////////////////////////
bool VM6::EventAdd( const Device::ID did, int id, double hz, int flag )
{
	return evsc->Add( did, id, hz, flag );
}


/////////////////////////////////////////////////////////////////////////////
// イベント削除
/////////////////////////////////////////////////////////////////////////////
bool VM6::EventDel( const Device::ID did, int id )
{
	return evsc->Del( did, id );
}


/////////////////////////////////////////////////////////////////////////////
// イベント更新
/////////////////////////////////////////////////////////////////////////////
void VM6::EventUpdate( int clk )
{
	evsc->Update( clk );
}


/////////////////////////////////////////////////////////////////////////////
// 指定イベントをリセットする
/////////////////////////////////////////////////////////////////////////////
void VM6::EventReset( Device::ID devid, int id, double ini )
{
	evsc->Reset( devid, id, ini );
}


/////////////////////////////////////////////////////////////////////////////
// イベントの進行率を求める
/////////////////////////////////////////////////////////////////////////////
double VM6::EventGetProgress( Device::ID devid, int id )
{
	return evsc->GetProgress( devid, id );
}


/////////////////////////////////////////////////////////////////////////////
// イベント情報取得
/////////////////////////////////////////////////////////////////////////////
bool VM6::EventGetInfo( EVSC::evinfo* info )
{
	return evsc->GetEvinfo( info );
}


/////////////////////////////////////////////////////////////////////////////
// イベント情報設定
/////////////////////////////////////////////////////////////////////////////
bool VM6::EventSetInfo( EVSC::evinfo* info )
{
	return evsc->SetEvinfo( info );
}


/////////////////////////////////////////////////////////////////////////////
// VSYNCを通知する
/////////////////////////////////////////////////////////////////////////////
void VM6::EventOnVSYNC( void )
{
	evsc->OnVSYNC();
}


// IO6 ---------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// IN関数
/////////////////////////////////////////////////////////////////////////////
BYTE VM6::IomIn( int port, int* wcnt )
{
	BYTE data = iom->In( port, wcnt );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( bp->Check( BPoint::BP_IN, port & 0xff ) ){
		PRINTD( IO_LOG, " -> Break!\n" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	return data;
}

BYTE VM6::IosIn( int port, int* wcnt )
{
	return ios->In( port, wcnt );
}


/////////////////////////////////////////////////////////////////////////////
// OUT関数
/////////////////////////////////////////////////////////////////////////////
void VM6::IomOut( int port, BYTE data, int* wcnt )
{
	iom->Out( port, data, wcnt );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( bp->Check( BPoint::BP_OUT, port & 0xff ) ){
		PRINTD( IO_LOG, " -> Break!\n" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
}

void VM6::IosOut( int port, BYTE data, int* wcnt )
{
	ios->Out( port, data, wcnt );
}


// IRQ6 --------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// 割込みチェック＆ベクタ取得
/////////////////////////////////////////////////////////////////////////////
int VM6::IntIntrCheck( void )
{
	return intr->IntrCheck();
}


/////////////////////////////////////////////////////////////////////////////
// 割込み要求
/////////////////////////////////////////////////////////////////////////////
void VM6::IntReqIntr( DWORD vec )
{
	intr->ReqIntr( vec );
}


/////////////////////////////////////////////////////////////////////////////
// 割込み撤回
/////////////////////////////////////////////////////////////////////////////
void VM6::IntCancelIntr( DWORD vec )
{
	intr->CancelIntr( vec );
}


/////////////////////////////////////////////////////////////////////////////
// タイマ割込みスイッチ取得
/////////////////////////////////////////////////////////////////////////////
bool VM6::IntGetTimerIntr( void )
{
	return intr->GetTimerIntr();
}


// SUB6 --------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// キー割込み要求
/////////////////////////////////////////////////////////////////////////////
void VM6::CpusReqKeyIntr( int flag, BYTE data )
{
	cpus->ReqKeyIntr( flag, data );
}


/////////////////////////////////////////////////////////////////////////////
// 外部割込み要求
/////////////////////////////////////////////////////////////////////////////
void VM6::CpusExtIntr( void )
{
	cpus->ExtIntr();
}


/////////////////////////////////////////////////////////////////////////////
// CMT READ割込み要求
/////////////////////////////////////////////////////////////////////////////
void VM6::CpusReqCmtIntr( BYTE data )
{
	cpus->ReqCmtIntr( data );
}


/////////////////////////////////////////////////////////////////////////////
// CMT割込み発生可?
/////////////////////////////////////////////////////////////////////////////
bool VM6::CpusIsCmtIntrReady( void )
{
	// BoostUp有効の場合はワークエリアもチェック
	return cpus->IsCmtIntrReady() &&
			!( cmtl->IsBoostUp() && ( mem->Read( vdg->IsSRmode() ? 0xe6b8 : 0xfa19 ) & 2 ) );
}


// MEM6 --------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// フェッチ(M1)
/////////////////////////////////////////////////////////////////////////////
BYTE VM6::MemFetch( WORD addr, int* m1wait )
{
	BYTE data = mem->Fetch( addr, m1wait );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( bp->Check( BPoint::BP_READ, addr ) ){
		PRINTD( MEM_LOG, " -> Break!\n" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	return data;
}


/////////////////////////////////////////////////////////////////////////////
// メモリリード
/////////////////////////////////////////////////////////////////////////////
BYTE VM6::MemRead( WORD addr, int* wcnt )
{
	BYTE data = mem->Read( addr, wcnt );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( bp->Check( BPoint::BP_READ, addr ) ){
		PRINTD( MEM_LOG, " -> Break!\n" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	return data;
}


/////////////////////////////////////////////////////////////////////////////
// メモリライト
/////////////////////////////////////////////////////////////////////////////
void VM6::MemWrite( WORD addr, BYTE data, int* wcnt )
{
	mem->Write( addr, data, wcnt );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( bp->Check( BPoint::BP_WRITE, addr ) ){
		PRINTD( MEM_LOG, " -> Break!\n" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
}


/////////////////////////////////////////////////////////////////////////////
// CG ROM BANK 選択
/////////////////////////////////////////////////////////////////////////////
void VM6::MemSetCGBank( bool data )
{
	mem->SetCGBank( data );
}


/////////////////////////////////////////////////////////////////////////////
// 直接読込み
/////////////////////////////////////////////////////////////////////////////
BYTE VM6::MemReadIntRam( WORD addr ) const { return mem->ReadIntRam( addr ); }
BYTE VM6::MemReadExtRam( WORD addr ) const { return mem->ReadExtRam( addr ); }
BYTE VM6::MemReadCGrom1( WORD addr ) const { return mem->ReadCGrom1( addr ); }
BYTE VM6::MemReadCGrom2( WORD addr ) const { return mem->ReadCGrom2( addr ); }
BYTE VM6::MemReadCGrom3( WORD addr ) const { return mem->ReadCGrom3( addr ); }


/*
// VDG ---------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CRT表示状態取得
/////////////////////////////////////////////////////////////////////////////
bool VM6::VdgGetCrtDisp( void ) const
{
	return vdg->GetCrtDisp();
}
*/


/////////////////////////////////////////////////////////////////////////////
// CRT表示状態設定
/////////////////////////////////////////////////////////////////////////////
void VM6::VdgSetCrtDisp( bool st )
{
	vdg->SetCrtDisp( st );
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウサイズ取得
/////////////////////////////////////////////////////////////////////////////
bool VM6::VdgGetWinSize( void ) const
{
	return vdg->GetWinSize();
}


/////////////////////////////////////////////////////////////////////////////
// バスリクエスト区間停止フラグ取得
/////////////////////////////////////////////////////////////////////////////
bool VM6::VdgIsBusReqStop( void ) const
{
	return vdg->IsBusReqStop();
}


/////////////////////////////////////////////////////////////////////////////
// バスリクエスト区間実行フラグ取得
/////////////////////////////////////////////////////////////////////////////
bool VM6::VdgIsBusReqExec( void ) const
{
	return vdg->IsBusReqExec();
}


/////////////////////////////////////////////////////////////////////////////
// SRモード取得
/////////////////////////////////////////////////////////////////////////////
bool VM6::VdgIsSRmode( void ) const
{
	return vdg->IsSRmode();
}


/////////////////////////////////////////////////////////////////////////////
// SRビットマップモード取得
/////////////////////////////////////////////////////////////////////////////
bool VM6::VdgIsSRBitmap( WORD addr ) const
{
	return vdg->IsSRBitmap( addr );
}


/////////////////////////////////////////////////////////////////////////////
// SRのG-VRAMアドレス取得 (ビットマップモード)
/////////////////////////////////////////////////////////////////////////////
WORD VM6::VdgSRGVramAddr( WORD addr ) const
{
	return vdg->SRGVramAddr( addr );
}


// PIO6 --------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// PartA リード
/////////////////////////////////////////////////////////////////////////////
BYTE VM6::PioReadA( void )
{
	return pio->ReadA();
}


// KEY6 --------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// カーソルキー状態取得
/////////////////////////////////////////////////////////////////////////////
BYTE VM6::KeyGetKeyJoy( void ) const
{
	return key->GetKeyJoy();
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティック状態取得
/////////////////////////////////////////////////////////////////////////////
BYTE VM6::KeyGetJoy( int JoyNo ) const
{
	return key->GetJoy( JoyNo );
}


/////////////////////////////////////////////////////////////////////////////
// 英字<->かな切換
/////////////////////////////////////////////////////////////////////////////
void VM6::KeyChangeKana( void )
{
	key->ChangeKana();
}


/////////////////////////////////////////////////////////////////////////////
// かな<->カナ切換
/////////////////////////////////////////////////////////////////////////////
void VM6::KeyChangeKKana( void )
{
	key->ChangeKKana();
}


// CMTS --------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// TAPEマウント
/////////////////////////////////////////////////////////////////////////////
bool VM6::CmtsMount( void )
{
	return cmts->Mount();
}


/////////////////////////////////////////////////////////////////////////////
// TAPEアンマウント
/////////////////////////////////////////////////////////////////////////////
void VM6::CmtsUnmount( void )
{
	cmts->Unmount();
}


/////////////////////////////////////////////////////////////////////////////
// ボーレート設定
/////////////////////////////////////////////////////////////////////////////
void VM6::CmtsSetBaud( int b )
{
	cmts->SetBaud( b );
}


/////////////////////////////////////////////////////////////////////////////
// CMT 1文字書込み
/////////////////////////////////////////////////////////////////////////////
void VM6::CmtsWriteOne( BYTE data )
{
	cmts->WriteOne( data );
}

// =============================================================




/////////////////////////////////////////////////////////////////////////////
// デバイスコネクタ
/////////////////////////////////////////////////////////////////////////////

// PC-6001,PC-6001A --------------------------------------------

// 割込み -----
const std::vector<IOBus::Connector> VM60::c_intr = {
	{ 0xb0, IOBus::portout, IRQ6::outB0H },
	{ 0xb1, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xb2, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xb3, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xb4, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xb5, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xb6, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xb7, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xb8, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xb9, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xba, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xbb, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xbc, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xbd, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xbe, IOBus::portout, IRQ6::outB0H },		// イメージ
	{ 0xbf, IOBus::portout, IRQ6::outB0H }		// イメージ
};

// VDG -----
const std::vector<IOBus::Connector> VM60::c_vdg = {
	{ 0xb0, IOBus::portout, VDG6::outB0H },
	{ 0xb1, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xb2, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xb3, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xb4, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xb5, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xb6, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xb7, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xb8, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xb9, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xba, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xbb, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xbc, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xbd, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xbe, IOBus::portout, VDG6::outB0H },		// イメージ
	{ 0xbf, IOBus::portout, VDG6::outB0H }		// イメージ
};

// PSG -----
const std::vector<IOBus::Connector> VM60::c_psg = {
	{ 0xa0, IOBus::portout, PSGb::outA0H },
	{ 0xa1, IOBus::portout, PSGb::outA1H },
	{ 0xa3, IOBus::portout, PSGb::outA3H },
	{ 0xa4, IOBus::portout, PSGb::outA0H },		// イメージ
	{ 0xa5, IOBus::portout, PSGb::outA1H },		// イメージ
	{ 0xa7, IOBus::portout, PSGb::outA3H },		// イメージ
	{ 0xa8, IOBus::portout, PSGb::outA0H },		// イメージ
	{ 0xa9, IOBus::portout, PSGb::outA1H },		// イメージ
	{ 0xab, IOBus::portout, PSGb::outA3H },		// イメージ
	{ 0xac, IOBus::portout, PSGb::outA0H },		// イメージ
	{ 0xad, IOBus::portout, PSGb::outA1H },		// イメージ
	{ 0xaf, IOBus::portout, PSGb::outA3H },		// イメージ
	{ 0xa2, IOBus::portin,  PSGb::inA2H },
	{ 0xa6, IOBus::portin,  PSGb::inA2H },		// イメージ
	{ 0xaa, IOBus::portin,  PSGb::inA2H },		// イメージ
	{ 0xae, IOBus::portin,  PSGb::inA2H }		// イメージ
};

// 8255(SUB CPU側) -----
const std::vector<IOBus::Connector> VM60::c_8255s = {
	{ SUB6::IO_BUS, IOBus::portout, PIO6::outPBH },
	{ SUB6::IO_BUS, IOBus::portin,  PIO6::inPBH  },
	{ SUB6::IO_T0,  IOBus::portin,  PIO6::inIBF  },
	{ SUB6::IO_INT, IOBus::portin,  PIO6::inOBF  }
};

// 8255(Z80側) -----
const std::vector<IOBus::Connector> VM60::c_8255m = {
	{ 0x90, IOBus::portout, PIO6::out90H },
	{ 0x91, IOBus::portout, PIO6::out91H },
	{ 0x92, IOBus::portout, PIO6::out92H },
	{ 0x93, IOBus::portout, PIO6::out93H },
	{ 0x94, IOBus::portout, PIO6::out90H },		// イメージ
	{ 0x95, IOBus::portout, PIO6::out91H },		// イメージ
	{ 0x96, IOBus::portout, PIO6::out92H },		// イメージ
	{ 0x97, IOBus::portout, PIO6::out93H },		// イメージ
	{ 0x98, IOBus::portout, PIO6::out90H },		// イメージ
	{ 0x99, IOBus::portout, PIO6::out91H },		// イメージ
	{ 0x9a, IOBus::portout, PIO6::out92H },		// イメージ
	{ 0x9b, IOBus::portout, PIO6::out93H },		// イメージ
	{ 0x9c, IOBus::portout, PIO6::out90H },		// イメージ
	{ 0x9d, IOBus::portout, PIO6::out91H },		// イメージ
	{ 0x9e, IOBus::portout, PIO6::out92H },		// イメージ
	{ 0x9f, IOBus::portout, PIO6::out93H },		// イメージ
	{ 0x90, IOBus::portin,  PIO6::in90H },
	{ 0x92, IOBus::portin,  PIO6::in92H },
	{ 0x93, IOBus::portin,  PIO6::in93H },
	{ 0x94, IOBus::portin,  PIO6::in90H },		// イメージ
	{ 0x96, IOBus::portin,  PIO6::in92H },		// イメージ
	{ 0x97, IOBus::portin,  PIO6::in93H },		// イメージ
	{ 0x98, IOBus::portin,  PIO6::in90H },		// イメージ
	{ 0x9a, IOBus::portin,  PIO6::in92H },		// イメージ
	{ 0x9b, IOBus::portin,  PIO6::in93H },		// イメージ
	{ 0x9c, IOBus::portin,  PIO6::in90H },		// イメージ
	{ 0x9e, IOBus::portin,  PIO6::in92H },		// イメージ
	{ 0x9f, IOBus::portin,  PIO6::in93H }		// イメージ
};

// DISK -----
const std::vector<IOBus::Connector> VM60::c_disk = {
	{ 0xd1, IOBus::portout, DSK6::outD1H },
	{ 0xd2, IOBus::portout, DSK6::outD2H },
	{ 0xd3, IOBus::portout, DSK6::outD3H },
	{ 0xd5, IOBus::portout, DSK6::outD1H },		// イメージ
	{ 0xd6, IOBus::portout, DSK6::outD2H },		// イメージ
	{ 0xd7, IOBus::portout, DSK6::outD3H },		// イメージ
	{ 0xd9, IOBus::portout, DSK6::outD1H },		// イメージ
	{ 0xda, IOBus::portout, DSK6::outD2H },		// イメージ
	{ 0xdb, IOBus::portout, DSK6::outD3H },		// イメージ
	{ 0xdd, IOBus::portout, DSK6::outD1H },		// イメージ
	{ 0xde, IOBus::portout, DSK6::outD2H },		// イメージ
	{ 0xdf, IOBus::portout, DSK6::outD3H },		// イメージ
	{ 0xd0, IOBus::portin,  DSK6::inD0H },
	{ 0xd1, IOBus::portin,  DSK6::inD1H },
	{ 0xd2, IOBus::portin,  DSK6::inD2H },
	{ 0xd3, IOBus::portin,  DSK6::inD2H },		// イメージ?
	{ 0xd4, IOBus::portin,  DSK6::inD0H },		// イメージ
	{ 0xd5, IOBus::portin,  DSK6::inD1H },		// イメージ
	{ 0xd6, IOBus::portin,  DSK6::inD2H },		// イメージ
	{ 0xd7, IOBus::portin,  DSK6::inD2H },		// イメージ?
	{ 0xd8, IOBus::portin,  DSK6::inD0H },		// イメージ
	{ 0xd9, IOBus::portin,  DSK6::inD1H },		// イメージ
	{ 0xda, IOBus::portin,  DSK6::inD2H },		// イメージ
	{ 0xdb, IOBus::portin,  DSK6::inD2H },		// イメージ?
	{ 0xdc, IOBus::portin,  DSK6::inD0H },		// イメージ
	{ 0xdd, IOBus::portin,  DSK6::inD1H },		// イメージ
	{ 0xde, IOBus::portin,  DSK6::inD2H },		// イメージ
	{ 0xdf, IOBus::portin,  DSK6::inD2H }		// イメージ?
};

// CMT(LOAD) -----
const std::vector<IOBus::Connector> VM60::c_cmtl = {
	{ 0xb0, IOBus::portout, CMTL::outB0H },
	{ 0xb1, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xb2, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xb3, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xb4, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xb5, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xb6, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xb7, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xb8, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xb9, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xba, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xbb, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xbc, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xbd, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xbe, IOBus::portout, CMTL::outB0H },		// イメージ
	{ 0xbf, IOBus::portout, CMTL::outB0H }		// イメージ
};




// PC-6001mk2 --------------------------------------------------

// 割込み -----
const std::vector<IOBus::Connector> VM62::c_intr = {
	{ 0xb0, IOBus::portout, IRQ6::outB0H },
	{ 0xf3, IOBus::portout, IRQ6::outF3H },
	{ 0xf4, IOBus::portout, IRQ6::outF4H },
	{ 0xf5, IOBus::portout, IRQ6::outF5H },
	{ 0xf6, IOBus::portout, IRQ6::outF6H },
	{ 0xf7, IOBus::portout, IRQ6::outF7H },
	{ 0xf3, IOBus::portin,  IRQ6::inF3H },
	{ 0xf4, IOBus::portin,  IRQ6::inF4H },
	{ 0xf5, IOBus::portin,  IRQ6::inF5H },
	{ 0xf6, IOBus::portin,  IRQ6::inF6H },
	{ 0xf7, IOBus::portin,  IRQ6::inF7H }
};

// メモリ -----
const std::vector<IOBus::Connector> VM62::c_memory = {
	{ 0xc1, IOBus::portout, MEM6::outC1H },
	{ 0xc2, IOBus::portout, MEM6::outC2H },
	{ 0xc3, IOBus::portout, MEM6::outC3H },
	{ 0xf0, IOBus::portout, MEM6::outF0H },
	{ 0xf1, IOBus::portout, MEM6::outF1H },
	{ 0xf2, IOBus::portout, MEM6::outF2H },
	{ 0xf3, IOBus::portout, MEM6::outF3H },
	{ 0xf8, IOBus::portout, MEM6::outF8H },
	{ 0xc2, IOBus::portin,  MEM6::inC2H },
	{ 0xf0, IOBus::portin,  MEM6::inF0H },
	{ 0xf1, IOBus::portin,  MEM6::inF1H },
	{ 0xf2, IOBus::portin,  MEM6::inF2H },
	{ 0xf3, IOBus::portin,  MEM6::inF3H }
};

// VDG -----
const std::vector<IOBus::Connector> VM62::c_vdg = {
	{ 0xb0, IOBus::portout, VDG6::outB0H },
	{ 0xc0, IOBus::portout, VDG6::outC0H },
	{ 0xc1, IOBus::portout, VDG6::outC1H },
	{ 0xa2, IOBus::portin,  VDG6::inA2H }
};

// PSG -----
const std::vector<IOBus::Connector> VM62::c_psg = {
	{ 0xa0, IOBus::portout, PSGb::outA0H },
	{ 0xa1, IOBus::portout, PSGb::outA1H },
	{ 0xa3, IOBus::portout, PSGb::outA3H },
	{ 0xa2, IOBus::portin,  PSGb::inA2H }
};

// 8255(Z80側) -----
const std::vector<IOBus::Connector> VM62::c_8255m = {
	{ 0x90, IOBus::portout, PIO6::out90H },
	{ 0x91, IOBus::portout, PIO6::out91H },
	{ 0x92, IOBus::portout, PIO6::out92H },
	{ 0x93, IOBus::portout, PIO6::out93H },
	{ 0x90, IOBus::portin,  PIO6::in90H },
	{ 0x92, IOBus::portin,  PIO6::in92H },
	{ 0x93, IOBus::portin,  PIO6::in93H }
};

// 8255(SUB CPU側) -----
const std::vector<IOBus::Connector> VM62::c_8255s = {
	{ SUB6::IO_BUS, IOBus::portout, PIO6::outPBH },
	{ SUB6::IO_BUS, IOBus::portin,  PIO6::inPBH  },
	{ SUB6::IO_T0,  IOBus::portin,  PIO6::inIBF  },
	{ SUB6::IO_INT, IOBus::portin,  PIO6::inOBF  }
};

// 音声合成 -----
const std::vector<IOBus::Connector> VM62::c_voice = {
	{ 0xe0, IOBus::portout, VCE6::outE0H },
	{ 0xe2, IOBus::portout, VCE6::outE2H },
	{ 0xe3, IOBus::portout, VCE6::outE3H },
	{ 0xe4, IOBus::portout, VCE6::outE0H },		// イメージ
	{ 0xe6, IOBus::portout, VCE6::outE2H },		// イメージ
	{ 0xe7, IOBus::portout, VCE6::outE3H },		// イメージ
	{ 0xe8, IOBus::portout, VCE6::outE0H },		// イメージ
	{ 0xea, IOBus::portout, VCE6::outE2H },		// イメージ
	{ 0xeb, IOBus::portout, VCE6::outE3H },		// イメージ
	{ 0xec, IOBus::portout, VCE6::outE0H },		// イメージ
	{ 0xee, IOBus::portout, VCE6::outE2H },		// イメージ
	{ 0xef, IOBus::portout, VCE6::outE3H },		// イメージ
	{ 0xe0, IOBus::portin,  VCE6::inE0H },
	{ 0xe2, IOBus::portin,  VCE6::inE2H },
	{ 0xe3, IOBus::portin,  VCE6::inE3H },
	{ 0xe4, IOBus::portin,  VCE6::inE0H },		// イメージ
	{ 0xe6, IOBus::portin,  VCE6::inE2H },		// イメージ
	{ 0xe7, IOBus::portin,  VCE6::inE3H },		// イメージ
	{ 0xe8, IOBus::portin,  VCE6::inE0H },		// イメージ
	{ 0xea, IOBus::portin,  VCE6::inE2H },		// イメージ
	{ 0xeb, IOBus::portin,  VCE6::inE3H },		// イメージ
	{ 0xec, IOBus::portin,  VCE6::inE0H },		// イメージ
	{ 0xee, IOBus::portin,  VCE6::inE2H },		// イメージ
	{ 0xef, IOBus::portin,  VCE6::inE3H }		// イメージ
};

// DISK -----
const std::vector<IOBus::Connector> VM62::c_disk = {
	{ 0xd1, IOBus::portout, DSK6::outD1H },
	{ 0xd2, IOBus::portout, DSK6::outD2H },
	{ 0xd3, IOBus::portout, DSK6::outD3H },
	{ 0xd5, IOBus::portout, DSK6::outD1H },		// イメージ
	{ 0xd6, IOBus::portout, DSK6::outD2H },		// イメージ
	{ 0xd7, IOBus::portout, DSK6::outD3H },		// イメージ
	{ 0xd9, IOBus::portout, DSK6::outD1H },		// イメージ
	{ 0xda, IOBus::portout, DSK6::outD2H },		// イメージ
	{ 0xdb, IOBus::portout, DSK6::outD3H },		// イメージ
	{ 0xdd, IOBus::portout, DSK6::outD1H },		// イメージ
	{ 0xde, IOBus::portout, DSK6::outD2H },		// イメージ
	{ 0xdf, IOBus::portout, DSK6::outD3H },		// イメージ
	{ 0xd0, IOBus::portin,  DSK6::inD0H },
	{ 0xd1, IOBus::portin,  DSK6::inD1H },
	{ 0xd2, IOBus::portin,  DSK6::inD2H },
	{ 0xd3, IOBus::portin,  DSK6::inD2H },		// イメージ?
	{ 0xd4, IOBus::portin,  DSK6::inD0H },		// イメージ
	{ 0xd5, IOBus::portin,  DSK6::inD1H },		// イメージ
	{ 0xd6, IOBus::portin,  DSK6::inD2H },		// イメージ
	{ 0xd7, IOBus::portin,  DSK6::inD2H },		// イメージ?
	{ 0xd8, IOBus::portin,  DSK6::inD0H },		// イメージ
	{ 0xd9, IOBus::portin,  DSK6::inD1H },		// イメージ
	{ 0xda, IOBus::portin,  DSK6::inD2H },		// イメージ
	{ 0xdb, IOBus::portin,  DSK6::inD2H },		// イメージ?
	{ 0xdc, IOBus::portin,  DSK6::inD0H },		// イメージ
	{ 0xdd, IOBus::portin,  DSK6::inD1H },		// イメージ
	{ 0xde, IOBus::portin,  DSK6::inD2H },		// イメージ
	{ 0xdf, IOBus::portin,  DSK6::inD2H }		// イメージ?
};

// CMT(LOAD) -----
const std::vector<IOBus::Connector> VM62::c_cmtl = {
	{ 0xb0, IOBus::portout, CMTL::outB0H }
};



// PC-6601 -----------------------------------------------------

// DISK -----
const std::vector<IOBus::Connector> VM66::c_disk = {
	{ 0xb1, IOBus::portout, DSK6::outB1H },
	{ 0xb3, IOBus::portout, DSK6::outB3H },
	{ 0xd0, IOBus::portout, DSK6::outD0H },
	{ 0xd1, IOBus::portout, DSK6::outD1H },
	{ 0xd2, IOBus::portout, DSK6::outD2H },
	{ 0xd3, IOBus::portout, DSK6::outD3H },
	{ 0xd6, IOBus::portout, DSK6::outD6H },
	{ 0xd8, IOBus::portout, DSK6::outD8H },
	{ 0xda, IOBus::portout, DSK6::outDAH },
	{ 0xdd, IOBus::portout, DSK6::outDDH },
	{ 0xde, IOBus::portout, DSK6::outDEH },
	{ 0xb2, IOBus::portin,  DSK6::inB2H },
	{ 0xd0, IOBus::portin,  DSK6::inD0H },
	{ 0xd1, IOBus::portin,  DSK6::inD1H },
	{ 0xd2, IOBus::portin,  DSK6::inD2H },
	{ 0xd3, IOBus::portin,  DSK6::inD3H },
	{ 0xd4, IOBus::portin,  DSK6::inD4H },
	{ 0xdc, IOBus::portin,  DSK6::inDCH },
	{ 0xdd, IOBus::portin,  DSK6::inDDH }
};



// PC-6001mk2SR ------------------------------------------------

// 割込み -----
const std::vector<IOBus::Connector> VM64::c_intr = {
	{ 0xb0, IOBus::portout, IRQ6::outB0H },
	{ 0xb8, IOBus::portout, IRQ6::outBxH },
	{ 0xb9, IOBus::portout, IRQ6::outBxH },
	{ 0xba, IOBus::portout, IRQ6::outBxH },
	{ 0xbb, IOBus::portout, IRQ6::outBxH },
	{ 0xbc, IOBus::portout, IRQ6::outBxH },
	{ 0xbd, IOBus::portout, IRQ6::outBxH },
	{ 0xbe, IOBus::portout, IRQ6::outBxH },
	{ 0xbf, IOBus::portout, IRQ6::outBxH },
	{ 0xf3, IOBus::portout, IRQ6::outF3H },
	{ 0xf4, IOBus::portout, IRQ6::outF4H },
	{ 0xf5, IOBus::portout, IRQ6::outF5H },
	{ 0xf6, IOBus::portout, IRQ6::outF6H },
	{ 0xf7, IOBus::portout, IRQ6::outF7H },
	{ 0xfa, IOBus::portout, IRQ6::outFAH },
	{ 0xfb, IOBus::portout, IRQ6::outFBH },
	{ 0xf3, IOBus::portin,  IRQ6::inF3H },
	{ 0xf4, IOBus::portin,  IRQ6::inF4H },
	{ 0xf5, IOBus::portin,  IRQ6::inF5H },
	{ 0xf6, IOBus::portin,  IRQ6::inF6H },
	{ 0xf7, IOBus::portin,  IRQ6::inF7H },
	{ 0xfa, IOBus::portin,  IRQ6::inFAH },
	{ 0xfb, IOBus::portin,  IRQ6::inFBH }
};

// メモリ -----
const std::vector<IOBus::Connector> VM64::c_memory = {
	{ 0x60, IOBus::portout, MEM6::out6xH },
	{ 0x61, IOBus::portout, MEM6::out6xH },
	{ 0x62, IOBus::portout, MEM6::out6xH },
	{ 0x63, IOBus::portout, MEM6::out6xH },
	{ 0x64, IOBus::portout, MEM6::out6xH },
	{ 0x65, IOBus::portout, MEM6::out6xH },
	{ 0x66, IOBus::portout, MEM6::out6xH },
	{ 0x67, IOBus::portout, MEM6::out6xH },
	{ 0x68, IOBus::portout, MEM6::out6xH },
	{ 0x69, IOBus::portout, MEM6::out6xH },
	{ 0x6a, IOBus::portout, MEM6::out6xH },
	{ 0x6b, IOBus::portout, MEM6::out6xH },
	{ 0x6c, IOBus::portout, MEM6::out6xH },
	{ 0x6d, IOBus::portout, MEM6::out6xH },
	{ 0x6e, IOBus::portout, MEM6::out6xH },
	{ 0x6f, IOBus::portout, MEM6::out6xH },
	
	{ 0xc1, IOBus::portout, MEM6::outC1H },
	{ 0xc2, IOBus::portout, MEM6::outC2H },
	{ 0xc3, IOBus::portout, MEM6::outC3H },
	
	{ 0xf0, IOBus::portout, MEM6::outF0H },
	{ 0xf1, IOBus::portout, MEM6::outF1H },
	{ 0xf2, IOBus::portout, MEM6::outF2H },
	{ 0xf3, IOBus::portout, MEM6::outF3H },
	{ 0xf8, IOBus::portout, MEM6::outF8H },
	
	{ 0x60, IOBus::portin,  MEM6::in6xH },
	{ 0x61, IOBus::portin,  MEM6::in6xH },
	{ 0x62, IOBus::portin,  MEM6::in6xH },
	{ 0x63, IOBus::portin,  MEM6::in6xH },
	{ 0x64, IOBus::portin,  MEM6::in6xH },
	{ 0x65, IOBus::portin,  MEM6::in6xH },
	{ 0x66, IOBus::portin,  MEM6::in6xH },
	{ 0x67, IOBus::portin,  MEM6::in6xH },
	{ 0x68, IOBus::portin,  MEM6::in6xH },
	{ 0x69, IOBus::portin,  MEM6::in6xH },
	{ 0x6a, IOBus::portin,  MEM6::in6xH },
	{ 0x6b, IOBus::portin,  MEM6::in6xH },
	{ 0x6c, IOBus::portin,  MEM6::in6xH },
	{ 0x6d, IOBus::portin,  MEM6::in6xH },
	{ 0x6e, IOBus::portin,  MEM6::in6xH },
	{ 0x6f, IOBus::portin,  MEM6::in6xH },
	
	{ 0xc2, IOBus::portin,  MEM6::inC2H },
	
	{ 0xf0, IOBus::portin,  MEM6::inF0H },
	{ 0xf1, IOBus::portin,  MEM6::inF1H },
	{ 0xf2, IOBus::portin,  MEM6::inF2H },
	{ 0xf3, IOBus::portin,  MEM6::inF3H },
	
	{ 0xb2, IOBus::portin,  MEM6::inB2H }
};

// VDG -----
const std::vector<IOBus::Connector> VM64::c_vdg = {
	{ 0x40, IOBus::portout, VDG6::out4xH },
	{ 0x41, IOBus::portout, VDG6::out4xH },
	{ 0x42, IOBus::portout, VDG6::out4xH },
	{ 0x43, IOBus::portout, VDG6::out4xH },
	{ 0xb0, IOBus::portout, VDG6::outB0H },
	{ 0xc0, IOBus::portout, VDG6::outC0H },
	{ 0xc1, IOBus::portout, VDG6::outC1H },
	{ 0xc8, IOBus::portout, VDG6::outC8H },
	{ 0xc9, IOBus::portout, VDG6::outC9H },
	{ 0xca, IOBus::portout, VDG6::outCAH },
	{ 0xcb, IOBus::portout, VDG6::outCBH },
	{ 0xcc, IOBus::portout, VDG6::outCCH },
	{ 0xcd, IOBus::portout, VDG6::outCDH },
	{ 0xce, IOBus::portout, VDG6::outCEH },
	{ 0xcf, IOBus::portout, VDG6::outCFH },
	{ 0xa2, IOBus::portin,  VDG6::inA2H }
};

// PSG -----
const std::vector<IOBus::Connector> VM64::c_psg = {
	{ 0xa0, IOBus::portout, PSGb::outA0H },
	{ 0xa1, IOBus::portout, PSGb::outA1H },
	{ 0xa3, IOBus::portout, PSGb::outA3H },
	{ 0xa2, IOBus::portin,  PSGb::inA2H },
	{ 0xa3, IOBus::portin,  PSGb::inA3H }
};

// 8255(Z80側) -----
const std::vector<IOBus::Connector> VM64::c_8255m = {
	{ 0x90, IOBus::portout, PIO6::out90H },
	{ 0x91, IOBus::portout, PIO6::out91H },
	{ 0x92, IOBus::portout, PIO6::out92H },
	{ 0x93, IOBus::portout, PIO6::out93H },
	{ 0x90, IOBus::portin,  PIO6::in90H },
	{ 0x92, IOBus::portin,  PIO6::in92H },
	{ 0x93, IOBus::portin,  PIO6::in93H }
};

// 8255(SUB CPU側) -----
const std::vector<IOBus::Connector> VM64::c_8255s = {
	{ SUB6::IO_BUS, IOBus::portout, PIO6::outPBH },
	{ SUB6::IO_BUS, IOBus::portin,  PIO6::inPBH  },
	{ SUB6::IO_T0,  IOBus::portin,  PIO6::inIBF  },
	{ SUB6::IO_INT, IOBus::portin,  PIO6::inOBF  }
};

// 音声合成 -----
const std::vector<IOBus::Connector> VM64::c_voice = {
	{ 0xe0, IOBus::portout, VCE6::outE0H },
	{ 0xe2, IOBus::portout, VCE6::outE2H },
	{ 0xe3, IOBus::portout, VCE6::outE3H },
	{ 0xe0, IOBus::portin,  VCE6::inE0H },
	{ 0xe2, IOBus::portin,  VCE6::inE2H },
	{ 0xe3, IOBus::portin,  VCE6::inE3H }
};

// DISK -----
const std::vector<IOBus::Connector> VM64::c_disk = {
	{ 0xd1, IOBus::portout, DSK6::outD1H },
	{ 0xd2, IOBus::portout, DSK6::outD2H },
	{ 0xd3, IOBus::portout, DSK6::outD3H },
	{ 0xd0, IOBus::portin,  DSK6::inD0H },
	{ 0xd1, IOBus::portin,  DSK6::inD1H },
	{ 0xd2, IOBus::portin,  DSK6::inD2H }
};

// CMT(LOAD) -----
const std::vector<IOBus::Connector> VM64::c_cmtl = {
	{ 0xb0, IOBus::portout, CMTL::outB0H }
};



// PC-6601SR ---------------------------------------------------

// DISK -----
const std::vector<IOBus::Connector> VM68::c_disk = {
	{ 0xb1, IOBus::portout, DSK6::outB1H },
	{ 0xb3, IOBus::portout, DSK6::outB3H },
	{ 0xd0, IOBus::portout, DSK6::outD0H },
	{ 0xd1, IOBus::portout, DSK6::outD1H },
	{ 0xd2, IOBus::portout, DSK6::outD2H },
	{ 0xd3, IOBus::portout, DSK6::outD3H },
	{ 0xd6, IOBus::portout, DSK6::outD6H },
	{ 0xd8, IOBus::portout, DSK6::outD8H },
	{ 0xda, IOBus::portout, DSK6::outDAH },
	{ 0xdd, IOBus::portout, DSK6::outDDH },
	{ 0xde, IOBus::portout, DSK6::outDEH },
	{ 0xb2, IOBus::portin,  DSK6::inB2H },
	{ 0xd0, IOBus::portin,  DSK6::inD0H },
	{ 0xd1, IOBus::portin,  DSK6::inD1H },
	{ 0xd2, IOBus::portin,  DSK6::inD2H },
	{ 0xd3, IOBus::portin,  DSK6::inD3H },
	{ 0xd4, IOBus::portin,  DSK6::inD4H },
	{ 0xdc, IOBus::portin,  DSK6::inDCH },
	{ 0xdd, IOBus::portin,  DSK6::inDDH }
};
