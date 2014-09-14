#include <new>

#include "pc6001v.h"

#include "breakpoint.h"
#include "config.h"
#include "cpum.h"
#include "cpus.h"
#include "debug.h"
#include "disk.h"
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




////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
VM6::VM6( EL6 *emuobj ) : cclock(0), pclock(0), el(emuobj), evsc(NULL), iom(NULL), ios(NULL),
	intr(NULL), cpum(NULL), cpus(NULL), mem(NULL),
	vdg(NULL), psg(NULL), voice(NULL), pio(NULL), key(NULL),
	cmtl(NULL), cmts(NULL), disk(NULL)
  #ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  , bp(NULL)
  #endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
{}

VM60::VM60( EL6 *emuobj ) : VM6(emuobj)
{
	cclock = CPUM_CLOCK60;
	pclock = PSG_CLOCK60;
	
	DevTable.Intr    = VM60::c_intr;	// 割込み
	DevTable.Vdg     = VM60::c_vdg;		// VDG
	DevTable.Psg     = VM60::c_psg;		// PSG
	DevTable.M8255   = VM60::c_8255m;	// I/O(Z80側)
	DevTable.S8255   = VM60::c_8255s;	// I/O(SUB CPU側)
	DevTable.Disk    = VM60::c_disk;	// DISK
	DevTable.CmtL    = VM60::c_cmtl;	// CMT(LOAD)
	DevTable.Soldier = VM6::c_soldier;	// 戦士のカートリッジ
}

VM61::VM61( EL6 *emuobj ) : VM6(emuobj)
{
	cclock = CPUM_CLOCK60;
	pclock = PSG_CLOCK60;
	
	DevTable.Intr    = VM60::c_intr;	// 割込み
	DevTable.Vdg     = VM60::c_vdg;		// VDG
	DevTable.Psg     = VM60::c_psg;		// PSG
	DevTable.M8255   = VM60::c_8255m;	// I/O(Z80側)
	DevTable.S8255   = VM60::c_8255s;	// I/O(SUB CPU側)
	DevTable.Disk    = VM60::c_disk;	// DISK
	DevTable.CmtL    = VM60::c_cmtl;	// CMT(LOAD)
	DevTable.Soldier = VM6::c_soldier;	// 戦士のカートリッジ
}

VM62::VM62( EL6 *emuobj ) : VM6(emuobj)
{
	cclock = CPUM_CLOCK62;
	pclock = PSG_CLOCK62;
	
	DevTable.Intr    = VM62::c_intr;	// 割込み
	DevTable.Memory  = VM62::c_memory;	// メモリ
	DevTable.Vdg     = VM62::c_vdg;		// VDG
	DevTable.Psg     = VM62::c_psg;		// PSG
	DevTable.M8255   = VM62::c_8255m;	// I/O(Z80側)
	DevTable.S8255   = VM62::c_8255s;	// I/O(SUB CPU側)
	DevTable.Voice   = VM62::c_voice;	// 音声合成
	DevTable.Disk    = VM62::c_disk;	// DISK
	DevTable.CmtL    = VM62::c_cmtl;	// CMT(LOAD)
	DevTable.Soldier = VM6::c_soldier;	// 戦士のカートリッジ
}

VM66::VM66( EL6 *emuobj ) : VM6(emuobj)
{
	cclock = CPUM_CLOCK62;
	pclock = PSG_CLOCK62;
	
	DevTable.Intr    = VM62::c_intr;	// 割込み
	DevTable.Memory  = VM62::c_memory;	// メモリ
	DevTable.Vdg     = VM62::c_vdg;		// VDG
	DevTable.Psg     = VM62::c_psg;		// PSG
	DevTable.M8255   = VM62::c_8255m;	// I/O(Z80側)
	DevTable.S8255   = VM62::c_8255s;	// I/O(SUB CPU側)
	DevTable.Voice   = VM62::c_voice;	// 音声合成
	DevTable.Disk    = VM66::c_disk;	// DISK
	DevTable.CmtL    = VM62::c_cmtl;	// CMT(LOAD)
	DevTable.Soldier = VM6::c_soldier;	// 戦士のカートリッジ
}

VM64::VM64( EL6 *emuobj ) : VM6(emuobj)
{
	cclock = CPUM_CLOCK64;
	pclock = PSG_CLOCK64;
	
	DevTable.Intr    = VM64::c_intr;	// 割込み
	DevTable.Memory  = VM64::c_memory;	// メモリ
	DevTable.Vdg     = VM64::c_vdg;		// VDG
	DevTable.Psg     = VM64::c_psg;		// PSG
	DevTable.M8255   = VM64::c_8255m;	// I/O(Z80側)
	DevTable.S8255   = VM64::c_8255s;	// I/O(SUB CPU側)
	DevTable.Voice   = VM64::c_voice;	// 音声合成
	DevTable.Disk    = VM64::c_disk;	// DISK
	DevTable.CmtL    = VM64::c_cmtl;	// CMT(LOAD)
	DevTable.Soldier = VM6::c_soldier;	// 戦士のカートリッジ
}

VM68::VM68( EL6 *emuobj ) : VM6(emuobj)
{
	cclock = CPUM_CLOCK64;
	pclock = PSG_CLOCK64;
	
	DevTable.Intr    = VM64::c_intr;	// 割込み
	DevTable.Memory  = VM64::c_memory;	// メモリ
	DevTable.Vdg     = VM64::c_vdg;		// VDG
	DevTable.Psg     = VM64::c_psg;		// PSG
	DevTable.M8255   = VM64::c_8255m;	// I/O(Z80側)
	DevTable.S8255   = VM64::c_8255s;	// I/O(SUB CPU側)
	DevTable.Voice   = VM64::c_voice;	// 音声合成
	DevTable.Disk    = VM68::c_disk;	// DISK
	DevTable.CmtL    = VM64::c_cmtl;	// CMT(LOAD)
	DevTable.Soldier = VM6::c_soldier;	// 戦士のカートリッジ
}



////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
VM6::~VM6( void )
{
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( bp )     delete bp;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	if( voice )  delete voice;
	if( key )    delete key;
	if( pio )    delete pio;
	if( cpus )   delete cpus;
	if( cmts )   delete cmts;
	if( cmtl )   delete cmtl;
	if( disk )   delete disk;
	if( cpum )   delete cpum;
	if( intr )   delete intr;
	if( vdg )    delete vdg;
	if( mem )    delete mem;
	if( psg )    delete psg;
	if( ios )    delete ios;
	if( iom )    delete iom;
	if( evsc )   delete evsc;
}

VM60::~VM60( void ){}
VM61::~VM61( void ){}
VM62::~VM62( void ){}
VM66::~VM66( void ){}
VM64::~VM64( void ){}
VM68::~VM68( void ){}




// =============================================================
// P6デバイス用関数群
// =============================================================


// EL ----------------------------------------------------------

////////////////////////////////////////////////////////////////
// モニタモード?
////////////////////////////////////////////////////////////////
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
bool VM6::ElIsMonitor( void ) const
{
	return el->IsMonitor();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// EVSC --------------------------------------------------------

////////////////////////////////////////////////////////////////
// イベント追加
////////////////////////////////////////////////////////////////
bool VM6::EventAdd( Device *dev, int id, double hz, int flag )
{
	return evsc->Add( dev, id, hz, flag );
}


////////////////////////////////////////////////////////////////
// イベント削除
////////////////////////////////////////////////////////////////
bool VM6::EventDel( Device *dev, int id )
{
	return evsc->Del( dev, id );
}


////////////////////////////////////////////////////////////////
// イベント更新
////////////////////////////////////////////////////////////////
void VM6::EventUpdate( int clk )
{
	evsc->Update( clk );
}


////////////////////////////////////////////////////////////////
// 指定イベントをリセットする
////////////////////////////////////////////////////////////////
void VM6::EventReset( Device::ID devid, int id, double ini )
{
	evsc->Reset( devid, id, ini );
}


////////////////////////////////////////////////////////////////
// イベントの進行率を求める
////////////////////////////////////////////////////////////////
double VM6::EventGetProgress( Device::ID devid, int id )
{
	return evsc->GetProgress( devid, id );
}


////////////////////////////////////////////////////////////////
// イベント情報取得
////////////////////////////////////////////////////////////////
bool VM6::EventGetInfo( EVSC::evinfo *info )
{
	return evsc->GetEvinfo( info );
}


////////////////////////////////////////////////////////////////
// イベント情報設定
////////////////////////////////////////////////////////////////
bool VM6::EventSetInfo( EVSC::evinfo *info )
{
	return evsc->SetEvinfo( info );
}


////////////////////////////////////////////////////////////////
// VSYNCを通知する
////////////////////////////////////////////////////////////////
void VM6::EventOnVSYNC( void )
{
	evsc->OnVSYNC();
}


// IO6 ---------------------------------------------------------

////////////////////////////////////////////////////////////////
// IN関数
////////////////////////////////////////////////////////////////
BYTE VM6::IomIn( int port, int *wcnt )
{
	BYTE data = iom->In( port, wcnt );
	
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( BpCheckBreakPoint( BPoint::BP_IN, port&0xff, data, NULL ) ){
		PRINTD( IO_LOG, " -> Break!\n" );
	}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	return data;
}

BYTE VM6::IosIn( int port, int *wcnt )
{
	return ios->In( port, wcnt );
}


////////////////////////////////////////////////////////////////
// OUT関数
////////////////////////////////////////////////////////////////
void VM6::IomOut( int port, BYTE data, int *wcnt )
{
	iom->Out( port, data, wcnt );
	
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( BpCheckBreakPoint( BPoint::BP_OUT, port&0xff, data, NULL ) ){
		PRINTD( IO_LOG, " -> Break!\n" );
	}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
}

void VM6::IosOut( int port, BYTE data, int *wcnt )
{
	ios->Out( port, data, wcnt );
}


// IRQ6 --------------------------------------------------------

////////////////////////////////////////////////////////////////
// 割込みチェック
////////////////////////////////////////////////////////////////
int VM6::IntIntrCheck( void )
{
	return intr->IntrCheck();
}


////////////////////////////////////////////////////////////////
// 割込み要求
////////////////////////////////////////////////////////////////
void VM6::IntReqIntr( DWORD vec )
{
	intr->ReqIntr( vec );
}


////////////////////////////////////////////////////////////////
// 割込み撤回
////////////////////////////////////////////////////////////////
void VM6::IntCancelIntr( DWORD vec )
{
	intr->CancelIntr( vec );
}


// CPU6 --------------------------------------------------------
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////////////////////////////////////////
// 1ライン逆アセンブル
////////////////////////////////////////////////////////////////
int VM6::CpumDisasm( char *str, WORD pc )
{
	return cpum->Disasm( str, pc );
}


////////////////////////////////////////////////////////////////
// レジスタ値取得
////////////////////////////////////////////////////////////////
void VM6::CpumGetRegister( cZ80::Register * reg )
{
	cpum->GetRegister( reg );
}


////////////////////////////////////////////////////////////////
// レジスタ値設定
////////////////////////////////////////////////////////////////
void VM6::CpumSetRegister( cZ80::Register * reg )
{
	cpum->SetRegister( reg );
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// SUB6 --------------------------------------------------------

////////////////////////////////////////////////////////////////
// キー割込み要求
////////////////////////////////////////////////////////////////
void VM6::CpusReqKeyIntr( int flag, BYTE data )
{
	cpus->ReqKeyIntr( flag, data );
}


////////////////////////////////////////////////////////////////
// 外部割込み要求
////////////////////////////////////////////////////////////////
void VM6::CpusExtIntr( void )
{
	cpus->ExtIntr();
}


////////////////////////////////////////////////////////////////
// CMT READ割込み要求
////////////////////////////////////////////////////////////////
void VM6::CpusReqCmtIntr( BYTE data )
{
	cpus->ReqCmtIntr( data );
}


////////////////////////////////////////////////////////////////
// CMT割込み発生可?
////////////////////////////////////////////////////////////////
bool VM6::CpusIsCmtIntrReady( void )
{
	// BoostUp有効の場合はワークエリアもチェック
	return cpus->IsCmtIntrReady() &&
			!( cmtl->IsBoostUp() && ( mem->Read( vdg->IsSRmode() ? 0xe6b8 : 0xfa19 ) & 2 ) );
}


// MEM6 --------------------------------------------------------

////////////////////////////////////////////////////////////////
// フェッチ(M1)
////////////////////////////////////////////////////////////////
BYTE VM6::MemFetch( WORD addr, int *m1wait )
{
	BYTE data = mem->Fetch( addr, m1wait );
	
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( BpCheckBreakPoint( BPoint::BP_READ, addr, data, NULL ) ){
		PRINTD( MEM_LOG, " -> Break!\n" );
	}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	return data;
}


////////////////////////////////////////////////////////////////
// メモリリード
////////////////////////////////////////////////////////////////
BYTE VM6::MemRead( WORD addr, int *wcnt )
{
	BYTE data = mem->Read( addr, wcnt );
	
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( BpCheckBreakPoint( BPoint::BP_READ, addr, data, NULL ) ){
		PRINTD( MEM_LOG, " -> Break!\n" );
	}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	return data;
}


////////////////////////////////////////////////////////////////
// メモリライト
////////////////////////////////////////////////////////////////
void VM6::MemWrite( WORD addr, BYTE data, int *wcnt )
{
	mem->Write( addr, data, wcnt );
	
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイントチェック
	if( BpCheckBreakPoint( BPoint::BP_WRITE, addr, data, NULL ) ){
		PRINTD( MEM_LOG, " -> Break!\n" );
	}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
}


////////////////////////////////////////////////////////////////
// CG ROM BANK を切り替える
////////////////////////////////////////////////////////////////
void VM6::MemSetCGBank( bool data )
{
	mem->SetCGBank( data );
}


////////////////////////////////////////////////////////////////
// 直接読込み
////////////////////////////////////////////////////////////////
BYTE VM6::MemReadMainRom( WORD addr ) const { return mem->ReadMainRom( addr ); }
BYTE VM6::MemReadMainRam( WORD addr ) const { return mem->ReadMainRam( addr ); }
BYTE VM6::MemReadExtRom ( WORD addr ) const { return mem->ReadExtRom( addr ); }
BYTE VM6::MemReadCGrom1 ( WORD addr ) const { return mem->ReadCGrom1( addr ); }
BYTE VM6::MemReadCGrom2 ( WORD addr ) const { return mem->ReadCGrom2( addr ); }
BYTE VM6::MemReadCGrom3 ( WORD addr ) const { return mem->ReadCGrom3( addr ); }
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
const char *VM6::MemGetReadMemBlk( int blk )  const { return mem->GetReadMemBlk( blk );  }
const char *VM6::MemGetWriteMemBlk( int blk ) const { return mem->GetWriteMemBlk( blk ); }
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// VDG ---------------------------------------------------------

////////////////////////////////////////////////////////////////
// CRT表示状態取得
////////////////////////////////////////////////////////////////
bool VM6::VdgGetCrtDisp( void ) const
{
	return vdg->GetCrtDisp();
}


////////////////////////////////////////////////////////////////
// CRT表示状態設定
////////////////////////////////////////////////////////////////
void VM6::VdgSetCrtDisp( bool st )
{
	vdg->SetCrtDisp( st );
}


////////////////////////////////////////////////////////////////
// ウィンドウサイズ取得
////////////////////////////////////////////////////////////////
bool VM6::VdgGetWinSize( void ) const
{
	return vdg->GetWinSize();
}


////////////////////////////////////////////////////////////////
// バスリクエスト区間停止フラグ取得
////////////////////////////////////////////////////////////////
bool VM6::VdgIsBusReqStop( void ) const
{
	return vdg->IsBusReqStop();
}


////////////////////////////////////////////////////////////////
// バスリクエスト区間実行フラグ取得
////////////////////////////////////////////////////////////////
bool VM6::VdgIsBusReqExec( void ) const
{
	return vdg->IsBusReqExec();
}


////////////////////////////////////////////////////////////////
// SRモード取得
////////////////////////////////////////////////////////////////
bool VM6::VdgIsSRmode( void ) const
{
	return vdg->IsSRmode();
}


////////////////////////////////////////////////////////////////
// SRのG-VRAMアクセス?
////////////////////////////////////////////////////////////////
bool VM6::VdgIsSRGVramAccess( WORD addr, bool rflag ) const
{
	return vdg->IsSRGVramAccess( addr ) && (iom->In( 0x60 + (addr>>13) + (rflag ? 0 : 8) ) == 0);
}


////////////////////////////////////////////////////////////////
// SRのG-VRAMアドレス取得 (メモリアクセス)
////////////////////////////////////////////////////////////////
WORD VM6::VdgSRGVramAddr( WORD addr ) const
{
	return vdg->SRGVramAddr( addr );
}


// PIO6 --------------------------------------------------------

////////////////////////////////////////////////////////////////
// PartA リード
////////////////////////////////////////////////////////////////
BYTE VM6::PioReadA( void )
{
	return pio->ReadA();
}


// KEY6 --------------------------------------------------------

////////////////////////////////////////////////////////////////
// カーソルキー状態取得
////////////////////////////////////////////////////////////////
BYTE VM6::KeyGetKeyJoy( void ) const
{
	return key->GetKeyJoy();
}


////////////////////////////////////////////////////////////////
// ジョイスティック状態取得
////////////////////////////////////////////////////////////////
BYTE VM6::KeyGetJoy( int JoyNo ) const
{
	return key->GetJoy( JoyNo );
}


////////////////////////////////////////////////////////////////
// キーボードインジケータ状態取得
////////////////////////////////////////////////////////////////
BYTE VM6::KeyGetKeyIndicator( void ) const
{
	return key->GetKeyIndicator();
}


////////////////////////////////////////////////////////////////
// 英字<->かな切換
////////////////////////////////////////////////////////////////
void VM6::KeyChangeKana( void )
{
	key->ChangeKana();
}


////////////////////////////////////////////////////////////////
// かな<->カナ切換
////////////////////////////////////////////////////////////////
void VM6::KeyChangeKKana( void )
{
	key->ChangeKKana();
}


// CMTL --------------------------------------------------------

////////////////////////////////////////////////////////////////
// マウント済み?
////////////////////////////////////////////////////////////////
bool VM6::CmtlIsMount( void ) const
{
	return cmtl->IsMount();
}


////////////////////////////////////////////////////////////////
// オートスタート?
////////////////////////////////////////////////////////////////
bool VM6::CmtlIsAutoStart( void ) const
{
	return cmtl->IsAutoStart();
}


////////////////////////////////////////////////////////////////
// ファイルパス取得
////////////////////////////////////////////////////////////////
const char *VM6::CmtlGetFile() const
{
	return cmtl->GetFile();
}


////////////////////////////////////////////////////////////////
// TAPE名取得
////////////////////////////////////////////////////////////////
const char *VM6::CmtlGetName( void ) const
{
	return cmtl->GetName();
}


////////////////////////////////////////////////////////////////
// ベタイメージサイズ取得
////////////////////////////////////////////////////////////////
DWORD VM6::CmtlGetSize( void ) const
{
	return cmtl->GetSize();
}


////////////////////////////////////////////////////////////////
// カウンタ取得
////////////////////////////////////////////////////////////////
int VM6::CmtlGetCount( void ) const
{
	return cmtl->GetCount();
}


// CMTS --------------------------------------------------------

////////////////////////////////////////////////////////////////
// TAPEマウント
////////////////////////////////////////////////////////////////
bool VM6::CmtsMount( void )
{
	return cmts->Mount();
}


////////////////////////////////////////////////////////////////
// TAPEアンマウント
////////////////////////////////////////////////////////////////
void VM6::CmtsUnmount( void )
{
	cmts->Unmount();
}


////////////////////////////////////////////////////////////////
// ボーレート設定
////////////////////////////////////////////////////////////////
void VM6::CmtsSetBaud( int b )
{
	cmts->SetBaud( b );
}


////////////////////////////////////////////////////////////////
// CMT 1文字書込み
////////////////////////////////////////////////////////////////
void VM6::CmtsCmtWrite( BYTE data )
{
	cmts->CmtWrite( data );
}



// DSK ---------------------------------------------------------

////////////////////////////////////////////////////////////////
// マウント済み?
////////////////////////////////////////////////////////////////
bool VM6::DskIsMount( int drvno ) const
{
	return disk->IsMount( drvno );
}


////////////////////////////////////////////////////////////////
// システムディスク?
////////////////////////////////////////////////////////////////
bool VM6::DskIsSystem( int drvno ) const
{
	return disk->IsSystem( drvno );
}


////////////////////////////////////////////////////////////////
// プロテクト?
////////////////////////////////////////////////////////////////
bool VM6::DskIsProtect( int drvno ) const
{
	return disk->IsProtect( drvno );
}


////////////////////////////////////////////////////////////////
// アクセス中?
////////////////////////////////////////////////////////////////
bool VM6::DskInAccess( int drvno ) const
{
	return disk->InAccess( drvno );
}


////////////////////////////////////////////////////////////////
// ファイルパス取得
////////////////////////////////////////////////////////////////
const char *VM6::DskGetFile( int drvno ) const
{
	return disk->GetFile( drvno );
}


////////////////////////////////////////////////////////////////
// DISK名取得
////////////////////////////////////////////////////////////////
const char *VM6::DskGetName( int drvno ) const
{
	return disk->GetName( drvno );
}


// BPoint ------------------------------------------------------
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////////////////////////////////////////
// ブレークポイントの有無をチェック
////////////////////////////////////////////////////////////////
bool VM6::BpExistBreakPoint( void ) const
{
	return bp->ExistBreakPoint();
}


////////////////////////////////////////////////////////////////
// ブレークポイントをチェック
////////////////////////////////////////////////////////////////
bool VM6::BpCheckBreakPoint( BPoint::BPtype type, WORD addr, BYTE data, const char *str )
{
	return bp->CheckBreakPoint( type, addr, data, str );
}


////////////////////////////////////////////////////////////////
// ブレークポイントの情報をクリア
////////////////////////////////////////////////////////////////
void VM6::BpClearStatus( void )
{
	bp->ClearStatus();
}


////////////////////////////////////////////////////////////////
// ブレークポイントのタイプを取得
////////////////////////////////////////////////////////////////
BPoint::BPtype VM6::BpGetType( int num ) const
{
	return bp->GetType( num );
}


////////////////////////////////////////////////////////////////
// ブレークポイントのタイプを設定
////////////////////////////////////////////////////////////////
void VM6::BpSetType( int num, BPoint::BPtype type )
{
	bp->SetType( num, type );
}


////////////////////////////////////////////////////////////////
// ブレークポイントのアドレスを取得
////////////////////////////////////////////////////////////////
WORD VM6::BpGetAddr( int num ) const
{
	return bp->GetAddr( num );
}


////////////////////////////////////////////////////////////////
// ブレークポイントのアドレスを設定
////////////////////////////////////////////////////////////////
void VM6::BpSetAddr( int num, WORD addr )
{
	bp->SetAddr( num, addr );
}


////////////////////////////////////////////////////////////////
// ブレーク要求あり?
////////////////////////////////////////////////////////////////
bool VM6::BpIsReqBreak( void ) const
{
	return bp->IsReqBreak();
}


////////////////////////////////////////////////////////////////
// ブレーク要求のあったブレークポイントNo.を取得
////////////////////////////////////////////////////////////////
int VM6::BpGetReqBPNum( void ) const
{
	return bp->GetReqBPNum();
}


////////////////////////////////////////////////////////////////
// ブレーク要求キャンセル
////////////////////////////////////////////////////////////////
void VM6::BpResetBreak( void )
{
	bp->ResetBreak();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// =============================================================

////////////////////////////////////////////////////////////////
// 機種別オブジェクト確保
////////////////////////////////////////////////////////////////
bool VM60::AllocObjSpecific( void )
{
	cpus   = new SUB60( this, DEV_ID("8049") );		// SUB CPU
	intr   = new IRQ60( this, DEV_ID("INTR") );		// 割込み
	mem    = new MEM60( this, DEV_ID("MEM1") );		// メモリ
	vdg    = new VDG60( this, DEV_ID("VDG1") );		// VDG
	key    = new KEY60( this, DEV_ID("KEYB") );		// キー
	psg    = new PSG6 ( this, DEV_ID("PSG1") );		// PSG
	disk   = new DSK60( this, DEV_ID("DSK1") );		// DISK
	
	return true;
}

bool VM61::AllocObjSpecific( void )
{
	cpus   = new SUB60( this, DEV_ID("8049") );		// SUB CPU
	intr   = new IRQ60( this, DEV_ID("INTR") );		// 割込み
	mem    = new MEM61( this, DEV_ID("MEM1") );		// メモリ
	vdg    = new VDG60( this, DEV_ID("VDG1") );		// VDG
	key    = new KEY60( this, DEV_ID("KEYB") );		// キー
	psg    = new PSG6 ( this, DEV_ID("PSG1") );		// PSG
	disk   = new DSK60( this, DEV_ID("DSK1") );		// DISK
	
	return true;
}

bool VM62::AllocObjSpecific( void )
{
	cpus   = new SUB62( this, DEV_ID("8049") );		// SUB CPU
	intr   = new IRQ62( this, DEV_ID("INTR") );		// 割込み
	mem    = new MEM62( this, DEV_ID("MEM1") );		// メモリ
	vdg    = new VDG62( this, DEV_ID("VDG2") );		// VDG
	key    = new KEY6 ( this, DEV_ID("KEYB") );		// キー
	psg    = new PSG6 ( this, DEV_ID("PSG1") );		// PSG
	voice  = new VCE6 ( this, DEV_ID("VCE1") );		// 音声合成
	disk   = new DSK60( this, DEV_ID("DSK1") );		// DISK
	
	return true;
}

bool VM66::AllocObjSpecific( void )
{
	cpus   = new SUB62( this, DEV_ID("8049") );		// SUB CPU
	intr   = new IRQ62( this, DEV_ID("INTR") );		// 割込み
	mem    = new MEM66( this, DEV_ID("MEM1") );		// メモリ
	vdg    = new VDG62( this, DEV_ID("VDG2") );		// VDG
	key    = new KEY6 ( this, DEV_ID("KEYB") );		// キー
	psg    = new PSG6 ( this, DEV_ID("PSG1") );		// PSG
	voice  = new VCE6 ( this, DEV_ID("VCE1") );		// 音声合成
	disk   = new DSK66( this, DEV_ID("DSK3") );		// DISK
	
	return true;
}

bool VM64::AllocObjSpecific( void )
{
	cpus   = new SUB62( this, DEV_ID("8049") );		// SUB CPU
	intr   = new IRQ64( this, DEV_ID("INTR") );		// 割込み
	mem    = new MEM64( this, DEV_ID("MEM1") );		// メモリ
	vdg    = new VDG64( this, DEV_ID("VDG3") );		// VDG
	key    = new KEY6 ( this, DEV_ID("KEYB") );		// キー
	psg    = new OPN6 ( this, DEV_ID("OPN1") );		// OPN
	voice  = new VCE64( this, DEV_ID("VCE2") );		// 音声合成
	disk   = new DSK64( this, DEV_ID("DSK2") );		// DISK
	
	return true;
}

bool VM68::AllocObjSpecific( void )
{
	cpus   = new SUB68( this, DEV_ID("8049") );		// SUB CPU
	intr   = new IRQ64( this, DEV_ID("INTR") );		// 割込み
	mem    = new MEM68( this, DEV_ID("MEM1") );		// メモリ
	vdg    = new VDG64( this, DEV_ID("VDG3") );		// VDG
	key    = new KEY6 ( this, DEV_ID("KEYB") );		// キー
	psg    = new OPN6 ( this, DEV_ID("OPN1") );		// OPN
	voice  = new VCE64( this, DEV_ID("VCE2") );		// 音声合成
	disk   = new DSK68( this, DEV_ID("DSK4") );		// DISK
	
	return true;
}

////////////////////////////////////////////////////////////////
// 全オブジェクト確保
////////////////////////////////////////////////////////////////
bool VM6::AllocObject( CFG6 *cnfg )
{
	PRINTD( VM_LOG, "[VM][AllocObject]\n" );
	
	// 各種オブジェクト確保
	try{
		evsc   = new EVSC( cclock ) ;				// イベントスケジューラ
		iom    = new IO6;							// I/O(Z80)
		ios    = new IO6;							// I/O(SUB CPU)
		cpum   = new CPU6 ( this, DEV_ID("CPU1") );	// CPU
		pio    = new PIO6 ( this, DEV_ID("8255") );	// 8255
		cmtl   = new CMTL ( this, DEV_ID("TAPE") );	// CMT(LOAD)
		cmts   = new CMTS ( this, DEV_ID("SAVE") );	// CMT(SAVE)
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		bp     = new BPoint();						// ブレークポイント
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	}
	// new に失敗した場合
	catch( std::bad_alloc ){
		if( evsc ){ delete evsc; evsc = NULL; }
		if( iom ) { delete iom;  iom  = NULL; }
		if( ios ) { delete ios;  ios  = NULL; }
		if( cpum ){ delete cpum; cpum = NULL; }
		if( pio ) { delete pio;  pio  = NULL; }
		if( cmtl ){ delete cmtl; cmtl = NULL; }
		if( cmts ){ delete cmts; cmts = NULL; }
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		if( bp )  { delete bp;   bp   = NULL; }
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		return false;
	}
	
	// 機種別オブジェクト確保
	if( !AllocObjSpecific() ){
		if( cpus ) { delete cpus;  cpus  = NULL; }
		if( intr ) { delete intr;  intr  = NULL; }
		if( mem )  { delete mem;   mem   = NULL; }
		if( vdg )  { delete vdg;   vdg   = NULL; }
		if( key )  { delete key;   key   = NULL; }
		if( psg )  { delete psg;   psg   = NULL; }
		if( voice ){ delete voice; voice = NULL; }
		if( disk ) { delete disk;  disk  = NULL; }
		return false;
	}
	
	// 全メモリ確保とROMファイル読込み
	if( !mem->AllocAllMemory( cnfg->GetRomPath(), cnfg->GetCheckCRC() ) ) return false;
	if( cnfg->GetRomPatch() ) mem->Patch();
	
	return true;
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool VM6::Init( CFG6 *cnfg  )
{
	PRINTD( VM_LOG, "[VM][Init]\n" );
	
	// イベントスケジューラ
	evsc->SetMasterClock( cclock * cnfg->GetOverClock() / 100 );
	evsc->Entry( intr );
	evsc->Entry( cpum );
	evsc->Entry( cpus );
	evsc->Entry( vdg );
	evsc->Entry( psg );
	evsc->Entry( voice );
	evsc->Entry( pio );
	evsc->Entry( key );
	evsc->Entry( cmtl );
	evsc->Entry( cmts );
	evsc->Entry( disk );
	
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
	if( !mem->Init( cnfg->GetUseExtRam(), cnfg->GetUseSoldier() ) ) return false;
	mem->Reset();
	if( *(cnfg->GetExtRomFile()) ) if( !mem->MountExtRom( cnfg->GetExtRomFile() ) ) return false;
	
	// VDG -----
	if( !vdg->Init() ) return false;
	vdg->SetMode4Color( cnfg->GetMode4Color() );
	
	// PSG/OPN -----
	psg->SetVolume( cnfg->GetPsgVol() );		// 音量設定
	psg->SetLPF( cnfg->GetPsgLPF() );			// ローパスフィルタ カットオフ周波数設定
	int i=0;
	while( DevTable.Psg[i].bank ){						// ウェイト設定
		if( DevTable.Psg[i].rule == IOBus::portout ) iom->SetOutWait( DevTable.Psg[i].bank, 1 );
		else										 iom->SetInWait ( DevTable.Psg[i].bank, 1 );
		i++;
	}
	if( !psg->Init( pclock, cnfg->GetSampleRate() ) ) return false;
	
	
	// 8255 -----
	pio->Reset();
	pio->cPRT::Init( cnfg->GetPrinterFile() );
	
	// キー -----
	if( !key->Init( cnfg->GetKeyRepeat() ) ) return false;
	VKeyConv *vk;
	if( cnfg->GetVKeyDef( &vk ) )				// キー定義配列取得
		key->SetVKeySymbols( vk );				// 仮想キーコード -> P6キーコード 設定
	
	// CMT(LOAD) -----
	if( !cmtl->Init( cnfg->GetSampleRate() ) ) return false;
	cmtl->SetVolume( cnfg->GetCmtVol() );		// 音量設定
	cmtl->SetLPF( DEFAULT_TAPELPF );			// ローパスフィルタ カットオフ周波数設定
	cmtl->SetBoost( cnfg->GetBoostUp() );
	cmtl->SetMaxBoost( cnfg->GetMaxBoost1(), cnfg->GetMaxBoost2() );
	
	// CMT(SAVE) -----
	if( !cmts->Init( cnfg->GetSaveFile() ) ) return false;
	
	// DISK -----
	//	if( cnfg->GetFddNum() )
	if( !disk->Init( cnfg->GetFddNum() ) ) return false;
	
	// 音声合成 -----
	if( DevTable.Voice ){
		if( !voice->Init( cnfg->GetSampleRate(), cnfg->GetWavePath() ) ) return false;
		voice->SetVolume( cnfg->GetVoiceVol() );	// 音量設定
	}
	
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ブレークポイント
	bp->ClearStatus();
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	
	
	
	// I/Oポートにデバイスを接続
	if( !iom->Connect( intr, DevTable.Intr  ) ) return false;	// 割込み
	if( !iom->Connect( vdg,  DevTable.Vdg   ) ) return false;	// VDG
	if( !iom->Connect( psg,  DevTable.Psg   ) ) return false;	// PSG/OPN
	if( !iom->Connect( pio,  DevTable.M8255 ) ) return false;	// 8255(Z80側)
	if( !ios->Connect( pio,  DevTable.S8255 ) ) return false;	// 8255(SUB CPU側)
	if( !iom->Connect( cmtl, DevTable.CmtL  ) ) return false;	// CMT(LOAD)
	if( cnfg->GetFddNum() || (cnfg->GetModel() == 66) || (cnfg->GetModel() == 68) )	// DISK
		if( !iom->Connect( disk,  DevTable.Disk   ) ) return false;
	if( DevTable.Memory )										// メモリ
		if( !iom->Connect( mem,   DevTable.Memory ) ) return false;
	if( DevTable.Voice )										// 音声合成
		if( !iom->Connect( voice, DevTable.Voice  ) ) return false;
	
	// オプション機能 -----
	if( cnfg->GetUseSoldier() )									// 戦士のカートリッジ
		if( !iom->Connect( mem, DevTable.Soldier ) ) return false;
	
	
	return true;
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
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
	cmtl->Reset();
	disk->Reset();
	if( voice ) voice->Reset();
}


////////////////////////////////////////////////////////////////
// 1命令実行
////////////////////////////////////////////////////////////////
int VM6::Emu( void )
{
	PRINTD( VM_LOG, "[VM][Emu]\n" );
	
	return cpum->Exec();	// CPU 1命令実行
}


////////////////////////////////////////////////////////////////
// CPUクロック取得
////////////////////////////////////////////////////////////////
int VM6::GetCPUClock( void ) const
{
	PRINTD( VM_LOG, "[VM][GetCPUClock]\n" );
	
	return cclock;
}
////////////////////////////////////////////////////////////////
// デバイスコネクタ
////////////////////////////////////////////////////////////////

// 戦士のカートリッジ
const IOBus::Connector VM6::c_soldier[] = {
	{ 0x7f, IOBus::portout, MEM6::out7FH },
	{ 0, 0, 0 }	};



// PC-6001,PC-6001A --------------------------------------------

// 割込み -----
const IOBus::Connector VM60::c_intr[] = {
	{ 0xb0, IOBus::portout, IRQ60::outB0H },
	{ 0xb1, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xb2, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xb3, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xb4, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xb5, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xb6, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xb7, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xb8, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xb9, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xba, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xbb, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xbc, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xbd, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xbe, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0xbf, IOBus::portout, IRQ60::outB0H },	// イメージ
	{ 0, 0, 0 }	};

// VDG -----
const IOBus::Connector VM60::c_vdg[] = {
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

// PSG -----
const IOBus::Connector VM60::c_psg[] = {
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

// 8255(SUB CPU側) -----
const IOBus::Connector VM60::c_8255s[] = {
	{ IO8049_BUS, IOBus::portout, PIO6::outPBH },
	{ IO8049_BUS, IOBus::portin,  PIO6::inPBH  },
	{ IO8049_T0,  IOBus::portin,  PIO6::inIBF  },
	{ IO8049_INT, IOBus::portin,  PIO6::inOBF  },
	{ 0, 0, 0 }	};

// 8255(Z80側) -----
const IOBus::Connector VM60::c_8255m[] = {
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

// DISK -----
const IOBus::Connector VM60::c_disk[] = {
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
	{ 0xd3, IOBus::portin,  DSK60::inD2H },		// イメージ?
	{ 0xd4, IOBus::portin,  DSK60::inD0H },		// イメージ
	{ 0xd5, IOBus::portin,  DSK60::inD1H },		// イメージ
	{ 0xd6, IOBus::portin,  DSK60::inD2H },		// イメージ
	{ 0xd7, IOBus::portin,  DSK60::inD2H },		// イメージ?
	{ 0xd8, IOBus::portin,  DSK60::inD0H },		// イメージ
	{ 0xd9, IOBus::portin,  DSK60::inD1H },		// イメージ
	{ 0xda, IOBus::portin,  DSK60::inD2H },		// イメージ
	{ 0xdb, IOBus::portin,  DSK60::inD2H },		// イメージ?
	{ 0xdc, IOBus::portin,  DSK60::inD0H },		// イメージ
	{ 0xdd, IOBus::portin,  DSK60::inD1H },		// イメージ
	{ 0xde, IOBus::portin,  DSK60::inD2H },		// イメージ
	{ 0xdf, IOBus::portin,  DSK60::inD2H },		// イメージ?
	{ 0, 0, 0 }	};

// CMT(LOAD) -----
const IOBus::Connector VM60::c_cmtl[] = {
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




// PC-6001mk2 --------------------------------------------------

// 割込み -----
const IOBus::Connector VM62::c_intr[] = {
	{ 0xb0, IOBus::portout, IRQ62::outB0H },
	{ 0xf3, IOBus::portout, IRQ62::outF3H },
	{ 0xf4, IOBus::portout, IRQ62::outF4H },
	{ 0xf5, IOBus::portout, IRQ62::outF5H },
	{ 0xf6, IOBus::portout, IRQ62::outF6H },
	{ 0xf7, IOBus::portout, IRQ62::outF7H },
	{ 0xf3, IOBus::portin,  IRQ62::inF3H },
	{ 0xf4, IOBus::portin,  IRQ62::inF4H },
	{ 0xf5, IOBus::portin,  IRQ62::inF5H },
	{ 0xf6, IOBus::portin,  IRQ62::inF6H },
	{ 0xf7, IOBus::portin,  IRQ62::inF7H },
	{ 0, 0, 0 }	};

// メモリ -----
const IOBus::Connector VM62::c_memory[] = {
	{ 0xc1, IOBus::portout, MEM62::outC1H },
	{ 0xc2, IOBus::portout, MEM62::outC2H },
	{ 0xc3, IOBus::portout, MEM62::outC3H },
	{ 0xf0, IOBus::portout, MEM62::outF0H },
	{ 0xf1, IOBus::portout, MEM62::outF1H },
	{ 0xf2, IOBus::portout, MEM62::outF2H },
	{ 0xf3, IOBus::portout, MEM62::outF3H },
	{ 0xf8, IOBus::portout, MEM62::outF8H },
	{ 0xf0, IOBus::portin,  MEM62::inF0H },
	{ 0xf1, IOBus::portin,  MEM62::inF1H },
	{ 0xf2, IOBus::portin,  MEM62::inF2H },
	{ 0xf3, IOBus::portin,  MEM62::inF3H },
	{ 0, 0, 0 }	};

// VDG -----
const IOBus::Connector VM62::c_vdg[] = {
	{ 0xb0, IOBus::portout, VDG62::outB0H },
	{ 0xc0, IOBus::portout, VDG62::outC0H },
	{ 0xc1, IOBus::portout, VDG62::outC1H },
	{ 0xa2, IOBus::portin,  VDG62::inA2H },
	{ 0, 0, 0 } };

// PSG -----
const IOBus::Connector VM62::c_psg[] = {
	{ 0xa0, IOBus::portout, PSG6::outA0H },
	{ 0xa1, IOBus::portout, PSG6::outA1H },
	{ 0xa3, IOBus::portout, PSG6::outA3H },
	{ 0xa2, IOBus::portin,  PSG6::inA2H },
	{ 0, 0, 0 }	};

// 8255(Z80側) -----
const IOBus::Connector VM62::c_8255m[] = {
	{ 0x90, IOBus::portout, PIO6::out90H },
	{ 0x91, IOBus::portout, PIO6::out91H },
	{ 0x92, IOBus::portout, PIO6::out92H },
	{ 0x93, IOBus::portout, PIO6::out93H },
	{ 0x90, IOBus::portin,  PIO6::in90H },
	{ 0x92, IOBus::portin,  PIO6::in92H },
	{ 0x93, IOBus::portin,  PIO6::in93H },
	{ 0, 0, 0 }	};

// 8255(SUB CPU側) -----
const IOBus::Connector VM62::c_8255s[] = {
	{ IO8049_BUS, IOBus::portout, PIO6::outPBH },
	{ IO8049_BUS, IOBus::portin,  PIO6::inPBH  },
	{ IO8049_T0,  IOBus::portin,  PIO6::inIBF  },
	{ IO8049_INT, IOBus::portin,  PIO6::inOBF  },
	{ 0, 0, 0 }	};

// 音声合成 -----
const IOBus::Connector VM62::c_voice[] = {
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

// DISK -----
const IOBus::Connector VM62::c_disk[] = {
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
	{ 0xd3, IOBus::portin,  DSK60::inD2H },		// イメージ?
	{ 0xd4, IOBus::portin,  DSK60::inD0H },		// イメージ
	{ 0xd5, IOBus::portin,  DSK60::inD1H },		// イメージ
	{ 0xd6, IOBus::portin,  DSK60::inD2H },		// イメージ
	{ 0xd7, IOBus::portin,  DSK60::inD2H },		// イメージ?
	{ 0xd8, IOBus::portin,  DSK60::inD0H },		// イメージ
	{ 0xd9, IOBus::portin,  DSK60::inD1H },		// イメージ
	{ 0xda, IOBus::portin,  DSK60::inD2H },		// イメージ
	{ 0xdb, IOBus::portin,  DSK60::inD2H },		// イメージ?
	{ 0xdc, IOBus::portin,  DSK60::inD0H },		// イメージ
	{ 0xdd, IOBus::portin,  DSK60::inD1H },		// イメージ
	{ 0xde, IOBus::portin,  DSK60::inD2H },		// イメージ
	{ 0xdf, IOBus::portin,  DSK60::inD2H },		// イメージ?
	{ 0, 0, 0 }	};

// CMT(LOAD) -----
const IOBus::Connector VM62::c_cmtl[] = {
	{ 0xb0, IOBus::portout, CMTL::outB0H },
	{ 0, 0, 0 }	};



// PC-6601 -----------------------------------------------------

// DISK -----
const IOBus::Connector VM66::c_disk[] = {
	{ 0xb1, IOBus::portout, DSK66::outB1H },
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


// PC-6001mk2SR ------------------------------------------------

// 割込み -----
const IOBus::Connector VM64::c_intr[] = {
	{ 0xb0, IOBus::portout, IRQ64::outB0H },
	{ 0xb8, IOBus::portout, IRQ64::outBxH },
	{ 0xb9, IOBus::portout, IRQ64::outBxH },
	{ 0xba, IOBus::portout, IRQ64::outBxH },
	{ 0xbb, IOBus::portout, IRQ64::outBxH },
	{ 0xbc, IOBus::portout, IRQ64::outBxH },
	{ 0xbd, IOBus::portout, IRQ64::outBxH },
	{ 0xbe, IOBus::portout, IRQ64::outBxH },
	{ 0xbf, IOBus::portout, IRQ64::outBxH },
	{ 0xf3, IOBus::portout, IRQ64::outF3H },
	{ 0xf4, IOBus::portout, IRQ64::outF4H },
	{ 0xf5, IOBus::portout, IRQ64::outF5H },
	{ 0xf6, IOBus::portout, IRQ64::outF6H },
	{ 0xf7, IOBus::portout, IRQ64::outF7H },
	{ 0xfa, IOBus::portout, IRQ64::outFAH },
	{ 0xfb, IOBus::portout, IRQ64::outFBH },
	{ 0xf3, IOBus::portin,  IRQ64::inF3H },
	{ 0xf4, IOBus::portin,  IRQ64::inF4H },
	{ 0xf5, IOBus::portin,  IRQ64::inF5H },
	{ 0xf6, IOBus::portin,  IRQ64::inF6H },
	{ 0xf7, IOBus::portin,  IRQ64::inF7H },
	{ 0xfa, IOBus::portin,  IRQ64::inFAH },
	{ 0xfb, IOBus::portin,  IRQ64::inFBH },
	{ 0, 0, 0 }	};

// メモリ -----
const IOBus::Connector VM64::c_memory[] = {
	{ 0x60, IOBus::portout, MEM64::out6xH },
	{ 0x61, IOBus::portout, MEM64::out6xH },
	{ 0x62, IOBus::portout, MEM64::out6xH },
	{ 0x63, IOBus::portout, MEM64::out6xH },
	{ 0x64, IOBus::portout, MEM64::out6xH },
	{ 0x65, IOBus::portout, MEM64::out6xH },
	{ 0x66, IOBus::portout, MEM64::out6xH },
	{ 0x67, IOBus::portout, MEM64::out6xH },
	{ 0x68, IOBus::portout, MEM64::out6xH },
	{ 0x69, IOBus::portout, MEM64::out6xH },
	{ 0x6a, IOBus::portout, MEM64::out6xH },
	{ 0x6b, IOBus::portout, MEM64::out6xH },
	{ 0x6c, IOBus::portout, MEM64::out6xH },
	{ 0x6d, IOBus::portout, MEM64::out6xH },
	{ 0x6e, IOBus::portout, MEM64::out6xH },
	{ 0x6f, IOBus::portout, MEM64::out6xH },
	
	{ 0xc1, IOBus::portout, MEM64::outC1H },
	{ 0xc2, IOBus::portout, MEM64::outC2H },
	{ 0xc3, IOBus::portout, MEM64::outC3H },
	
	{ 0xf0, IOBus::portout, MEM64::outF0H },
	{ 0xf1, IOBus::portout, MEM64::outF1H },
	{ 0xf2, IOBus::portout, MEM64::outF2H },
	{ 0xf3, IOBus::portout, MEM64::outF3H },
	{ 0xf8, IOBus::portout, MEM64::outF8H },
	
	{ 0x60, IOBus::portin,  MEM64::in6xH },
	{ 0x61, IOBus::portin,  MEM64::in6xH },
	{ 0x62, IOBus::portin,  MEM64::in6xH },
	{ 0x63, IOBus::portin,  MEM64::in6xH },
	{ 0x64, IOBus::portin,  MEM64::in6xH },
	{ 0x65, IOBus::portin,  MEM64::in6xH },
	{ 0x66, IOBus::portin,  MEM64::in6xH },
	{ 0x67, IOBus::portin,  MEM64::in6xH },
	{ 0x68, IOBus::portin,  MEM64::in6xH },
	{ 0x69, IOBus::portin,  MEM64::in6xH },
	{ 0x6a, IOBus::portin,  MEM64::in6xH },
	{ 0x6b, IOBus::portin,  MEM64::in6xH },
	{ 0x6c, IOBus::portin,  MEM64::in6xH },
	{ 0x6d, IOBus::portin,  MEM64::in6xH },
	{ 0x6e, IOBus::portin,  MEM64::in6xH },
	{ 0x6f, IOBus::portin,  MEM64::in6xH },
	
	{ 0xf0, IOBus::portin,  MEM64::inF0H },
	{ 0xf1, IOBus::portin,  MEM64::inF1H },
	{ 0xf2, IOBus::portin,  MEM64::inF2H },
	{ 0xf3, IOBus::portin,  MEM64::inF3H },

	{ 0xb2, IOBus::portin,  MEM64::inB2H },

	{ 0, 0, 0 }	};

// VDG -----
const IOBus::Connector VM64::c_vdg[] = {
	{ 0x40, IOBus::portout, VDG64::out4xH },
	{ 0x41, IOBus::portout, VDG64::out4xH },
	{ 0x42, IOBus::portout, VDG64::out4xH },
	{ 0x43, IOBus::portout, VDG64::out4xH },
	{ 0xb0, IOBus::portout, VDG64::outB0H },
	{ 0xc0, IOBus::portout, VDG64::outC0H },
	{ 0xc1, IOBus::portout, VDG64::outC1H },
	{ 0xc8, IOBus::portout, VDG64::outC8H },
	{ 0xc9, IOBus::portout, VDG64::outC9H },
	{ 0xca, IOBus::portout, VDG64::outCAH },
	{ 0xcb, IOBus::portout, VDG64::outCBH },
	{ 0xcc, IOBus::portout, VDG64::outCCH },
	{ 0xce, IOBus::portout, VDG64::outCEH },
	{ 0xcf, IOBus::portout, VDG64::outCFH },
	{ 0xa2, IOBus::portin,  VDG64::inA2H },
	{ 0, 0, 0 } };

// PSG -----
const IOBus::Connector VM64::c_psg[] = {
	{ 0xa0, IOBus::portout, OPN6::outA0H },
	{ 0xa1, IOBus::portout, OPN6::outA1H },
	{ 0xa3, IOBus::portout, OPN6::outA3H },
	{ 0xa2, IOBus::portin,  OPN6::inA2H },
	{ 0xa3, IOBus::portin,  OPN6::inA3H },
	{ 0, 0, 0 }	};

// 8255(Z80側) -----
const IOBus::Connector VM64::c_8255m[] = {
	{ 0x90, IOBus::portout, PIO6::out90H },
	{ 0x91, IOBus::portout, PIO6::out91H },
	{ 0x92, IOBus::portout, PIO6::out92H },
	{ 0x93, IOBus::portout, PIO6::out93H },
	{ 0x90, IOBus::portin,  PIO6::in90H },
	{ 0x92, IOBus::portin,  PIO6::in92H },
	{ 0x93, IOBus::portin,  PIO6::in93H },
	{ 0, 0, 0 }	};

// 8255(SUB CPU側) -----
const IOBus::Connector VM64::c_8255s[] = {
	{ IO8049_BUS, IOBus::portout, PIO6::outPBH },
	{ IO8049_BUS, IOBus::portin,  PIO6::inPBH  },
	{ IO8049_T0,  IOBus::portin,  PIO6::inIBF  },
	{ IO8049_INT, IOBus::portin,  PIO6::inOBF  },
	{ 0, 0, 0 }	};

// 音声合成 -----
const IOBus::Connector VM64::c_voice[] = {
	{ 0xe0, IOBus::portout, VCE6::outE0H },
	{ 0xe2, IOBus::portout, VCE6::outE2H },
	{ 0xe3, IOBus::portout, VCE6::outE3H },
	{ 0xe0, IOBus::portin,  VCE6::inE0H },
	{ 0xe2, IOBus::portin,  VCE6::inE2H },
	{ 0xe3, IOBus::portin,  VCE6::inE3H },
	{ 0, 0, 0 }	};

// DISK -----
const IOBus::Connector VM64::c_disk[] = {
	{ 0xd1, IOBus::portout, DSK60::outD1H },
	{ 0xd2, IOBus::portout, DSK60::outD2H },
	{ 0xd3, IOBus::portout, DSK60::outD3H },
	{ 0xd0, IOBus::portin,  DSK60::inD0H },
	{ 0xd1, IOBus::portin,  DSK60::inD1H },
	{ 0xd2, IOBus::portin,  DSK60::inD2H },
	{ 0, 0, 0 }	};

// CMT(LOAD) -----
const IOBus::Connector VM64::c_cmtl[] = {
	{ 0xb0, IOBus::portout, CMTL::outB0H },
	{ 0, 0, 0 }	};



// PC-6601SR ---------------------------------------------------

// DISK -----
const IOBus::Connector VM68::c_disk[] = {
	{ 0xb1, IOBus::portout, DSK66::outB1H },
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
