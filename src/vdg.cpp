/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "pc6001v.h"

#include "common.h"
#include "log.h"
#include "intr.h"
#include "vdg.h"

#include "p6el.h"
#include "p6vm.h"


// イベントID
#define	EID_VSYNCS	1			// 垂直同期開始
#define	EID_VSYNCE	2			// 垂直同期終了
#define	EID_HDISPS	3			// 表示区間開始
#define	EID_HDISPE	4			// 表示区間終了

#define	VSLINE		262			// 垂直トータルライン
#define	VLINES60	192			// 垂直表示ライン(N60)
#define	VLINES62	200			// 垂直表示ライン(N60m)

#define	HSDCLK60	455			// 水平トータル期間(ドットクロック) 60
#define	HSDCLK62	456			// 水平トータル期間(ドットクロック) 62,66,64,68
#define	HCLK6060	(256+40)	// 水平表示期間(N60)  60
#define	HCLK6062	(256+48)	// 水平表示期間(N60)  62,66
#define	HCLK6064	(256+ 8)	// 水平表示期間(N60)  64,68
#define	HCLOCK62	(320+48)	// 水平表示期間(N60m) 62,66
#define	HCLOCK64	(320+16)	// 水平表示期間(N60m) 64,68



// カラーコード
#define	CM1			(0)			// mode 1   カラーセット 5
#define	CM2			(CM1+5)		// mode 2,3 カラーセット 8
#define	CM4			(CM2+8)		// mode 4   カラーセット 28
#define	CMD			(CM4+28)	// RGB      カラーセット 16

// --------- 60 ---------
const BYTE VDG6::COL60_AN[] =			// mode 1 -----
				{ CM1+1, CM1+2, CM1+3, CM1+4, CM1 };

const BYTE VDG6::COL60_SG[] =			// mode 2 -----
				{ CM2,   CM2+1, CM2+2, CM2+3, CM2+4, CM2+5, CM2+6, CM2+7, CM1 };

const BYTE VDG6::COL60_CG[][8] = {		// mode 3 -----
				{ CM2,   CM2+ 1, CM2+ 2, CM2+3, 0,      0,      0,      0      },
				{ CM2+4, CM2+ 5, CM2+ 6, CM2+7, 0,      0,      0,      0      },
				{ CM4,   CM4+ 4, CM4+ 5, CM4+1, CM4+ 8, CM4+ 9, CM4+10, CM4+11 },	// mode4(Set1) Jにじみ
				{ CM4+2, CM4+16, CM4+17, CM4+3, CM4+20, CM4+21, CM4+22, CM4+23 },	// mode4(Set2) Jにじみ
				{ CM4,   CM4+ 5, CM4+ 4, CM4+1, CM4+10, CM4+11, CM4+ 8, CM4+ 9 },	// mode4(Set1) Jにじみ
				{ CM4+2, CM4+17, CM4+16, CM4+3, CM4+22, CM4+23, CM4+20, CM4+21 },	// mode4(Set2) Jにじみ
				{ CM4,   CM4+ 6, CM4+ 7, CM4+1, CM4+12, CM4+13, CM4+14, CM4+15 },	// mode4(Set1) Jにじみ
				{ CM4+2, CM4+18, CM4+19, CM4+3, CM4+24, CM4+25, CM4+26, CM4+27 },	// mode4(Set2) Jにじみ
				{ CM4,   CM4+ 7, CM4+ 6, CM4+1, CM4+14, CM4+15, CM4+12, CM4+13 },	// mode4(Set1) Jにじみ
				{ CM4+2, CM4+19, CM4+18, CM4+3, CM4+26, CM4+27, CM4+24, CM4+25 }	// mode4(Set2) Jにじみ
			};

const BYTE VDG6::COL60_RG[][2] = {		// mode 4 -----
				{ CM4,   CM4+1 },
				{ CM4+2, CM4+3 }
			};

// --------- mk2 ---------
const BYTE VDG6::COL62_AN[] =			// mode 1-1 -----
				{ CMD+15, CMD+8, CMD+10, CMD+8, CMD+8 };

const BYTE VDG6::COL62_SG[] =			// mode 1-2 -----
				{ CMD+10, CMD+11, CMD+12, CMD+9, CMD+15, CMD+14, CMD+13, CMD+1, CMD+8 };

const BYTE VDG6::COL62_CG[][8] = {		// mode 1-3 -----
				{ CMD+10, CMD+11, CMD+12, CMD+ 9, 0,      0,      0,      0      },	// mode1-4(Set1) Jにじみ
				{ CMD+15, CMD+14, CMD+13, CMD+ 1, 0,      0,      0,      0      },	// mode1-4(Set1) Jにじみ
				{ CMD+ 8, CM4+ 4, CM4+ 5, CMD+10, CM4+ 8, CM4+ 9, CM4+10, CM4+11 },	// mode1-4(Set1) Jにじみ
				{ CMD+ 8, CM4+16, CM4+17, CMD+15, CM4+20, CM4+21, CM4+22, CM4+23 },	// mode1-4(Set1) Jにじみ
				{ CMD+ 8, CM4+ 5, CM4+ 4, CMD+10, CM4+10, CM4+11, CM4+ 8, CM4+ 9 },	// mode1-4(Set1) Jにじみ
				{ CMD+ 8, CM4+17, CM4+16, CMD+15, CM4+22, CM4+23, CM4+20, CM4+21 },	// mode1-4(Set1) Jにじみ
				{ CMD+ 8, CM4+ 6, CM4+ 7, CMD+10, CM4+12, CM4+13, CM4+14, CM4+15 },	// mode1-4(Set1) Jにじみ
				{ CMD+ 8, CM4+18, CM4+19, CMD+15, CM4+24, CM4+25, CM4+26, CM4+27 },	// mode1-4(Set1) Jにじみ
				{ CMD+ 8, CM4+ 7, CM4+ 6, CMD+10, CM4+14, CM4+15, CM4+12, CM4+13 },	// mode1-4(Set1) Jにじみ
				{ CMD+ 8, CM4+19, CM4+18, CMD+15, CM4+26, CM4+27, CM4+24, CM4+25 }	// mode1-4(Set1) Jにじみ
			};

const BYTE VDG6::COL62_RG[][2] = {		// mode 1-4 -----
				{ CMD+8, CMD+10 },
				{ CMD+8, CMD+15 }
			};

const BYTE VDG6::COL62_AN2[] =			// mode 2-1,2 -----
				{ CMD,   CMD+1, CMD+ 2, CMD+ 3, CMD+ 4, CMD+ 5, CMD+ 6, CMD+ 7,
				  CMD+8, CMD+9, CMD+10, CMD+11, CMD+12, CMD+13, CMD+14, CMD+15 };

const BYTE VDG6::COL62_CG2[][16] = {	// mode 2-3,4 -----
				{ CMD,    CMD+ 4, CMD+ 1, CMD+ 5, CMD+ 2, CMD+ 6, CMD+ 3, CMD+ 7,
				  CMD+ 8, CMD+12, CMD+ 9, CMD+13, CMD+10, CMD+14, CMD+11, CMD+15 },
				{ CMD+10, CMD+11, CMD+12, CMD+ 9, CMD+15, CMD+14, CMD+13, CMD+ 1,
				  CMD+10, CMD+11, CMD+12, CMD+ 9, CMD+15, CMD+14, CMD+13, CMD+ 1 }
			};


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
VDG6::VDG6( VM6* vm, const ID& id ) : Device( vm, id ),
		AddrOff( 0 ), VSYNC( false ), HSYNC( false ), VLcnt( VLINES60 )
{
}

VDG60::VDG60( VM6* vm, const ID& id ) : VDG6( vm, id )
{
	HSdclk = HSDCLK60;
	Hclk60 = HCLK6060;
	
	// カラーテーブル設定
	for( int i=0; i<COUNTOF(COL_AN); i++ )
		COL_AN[i] = COL60_AN[i];
	
	for( int i=0; i<COUNTOF(COL_SG); i++ )
		COL_SG[i] = COL60_SG[i];
	
	for( int i=0; i<COUNTOF(COL_CG); i++ )
		for( int j=0; j<COUNTOF(COL_CG[0]); j++ )
			COL_CG[i][j] = COL60_CG[i][j];
	
	for( int i=0; i<COUNTOF(COL_RG); i++ )
		for( int j=0; j<COUNTOF(COL_RG[0]); j++ )
			COL_RG[i][j] = COL60_RG[i][j];
	
	// Device Description (Out)
	descs.outdef.emplace( outB0H, STATIC_CAST( Device::OutFuncPtr, &VDG60::OutB0H ) );
}

VDG62::VDG62( VM6* vm, const ID& id ) : VDG6( vm, id )
{
	HSdclk = HSDCLK62;
	Hclk60 = HCLK6062;
	
	// カラーテーブル設定
	for( int i=0; i<COUNTOF(COL_AN); i++ )
		COL_AN[i] = COL62_AN[i];
	
	for( int i=0; i<COUNTOF(COL_SG); i++ )
		COL_SG[i] = COL62_SG[i];
	
	for( int i=0; i<COUNTOF(COL_CG); i++ )
		for( int j=0; j<COUNTOF(COL_CG[0]); j++ )
			COL_CG[i][j] = COL62_CG[i][j];
	
	for( int i=0; i<COUNTOF(COL_RG); i++ )
		for( int j=0; j<COUNTOF(COL_RG[0]); j++ )
			COL_RG[i][j] = COL62_RG[i][j];
	
	for( int i=0; i<COUNTOF(COL_AN2); i++ )
		COL_AN2[i] = COL62_AN2[i];
	
	for( int i=0; i<COUNTOF(COL_CG2); i++ )
		for( int j=0; j<COUNTOF(COL_CG2[0]); j++ )
			COL_CG2[i][j] = COL62_CG2[i][j];
	
	// Device Description (Out)
	descs.outdef.emplace( outB0H, STATIC_CAST( Device::OutFuncPtr, &VDG62::OutB0H ) );
	descs.outdef.emplace( outC0H, STATIC_CAST( Device::OutFuncPtr, &VDG62::OutC0H ) );
	descs.outdef.emplace( outC1H, STATIC_CAST( Device::OutFuncPtr, &VDG62::OutC1H ) );
	
	// Device Description (In)
	descs.indef.emplace ( inA2H,  STATIC_CAST( Device::InFuncPtr,  &VDG62::InA2H  ) );
}

VDG64::VDG64( VM6* vm, const ID& id ) : VDG6( vm, id )
{
	HSdclk = HSDCLK62;
	Hclk60 = HCLK6064;
	
	// カラーテーブル設定
	for( int i=0; i<COUNTOF(COL_AN); i++ )
		COL_AN[i] = COL62_AN[i];
	
	for( int i=0; i<COUNTOF(COL_SG); i++ )
		COL_SG[i] = COL62_SG[i];
	
	for( int i=0; i<COUNTOF(COL_CG); i++ )
		for( int j=0; j<COUNTOF(COL_CG[0]); j++ )
			COL_CG[i][j] = COL62_CG[i][j];
	
	for( int i=0; i<COUNTOF(COL_RG); i++ )
		for( int j=0; j<COUNTOF(COL_RG[0]); j++ )
			COL_RG[i][j] = COL62_RG[i][j];
	
	for( int i=0; i<COUNTOF(COL_AN2); i++ )
		COL_AN2[i] = COL62_AN2[i];
	
	for( int i=0; i<COUNTOF(COL_CG2); i++ )
		for( int j=0; j<COUNTOF(COL_CG2[0]); j++ )
			COL_CG2[i][j] = COL62_CG2[i][j];
	
	// Device Description (Out)
	descs.outdef.emplace( out4xH, STATIC_CAST( Device::OutFuncPtr, &VDG64::Out4xH ) );
	descs.outdef.emplace( outB0H, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutB0H ) );
	descs.outdef.emplace( outC0H, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutC0H ) );
	descs.outdef.emplace( outC1H, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutC1H ) );
	descs.outdef.emplace( outC8H, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutC8H ) );
	descs.outdef.emplace( outC9H, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutC9H ) );
	descs.outdef.emplace( outCAH, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCAH ) );
	descs.outdef.emplace( outCBH, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCBH ) );
	descs.outdef.emplace( outCCH, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCCH ) );
	descs.outdef.emplace( outCDH, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCDH ) );
	descs.outdef.emplace( outCEH, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCEH ) );
	descs.outdef.emplace( outCFH, STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCFH ) );
	
	// Device Description (In)
	descs.indef.emplace ( inA2H,  STATIC_CAST( Device::InFuncPtr,  &VDG64::InA2H  ) );
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
VDG6::~VDG6( void )
{
}

VDG60::~VDG60( void )
{
}

VDG62::~VDG62( void )
{
}

VDG64::~VDG64( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VDG6::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_VSYNCS:	// VSYNC開始
		VSYNC = true;
		vm->EventOnVSYNC();				// VSYNCを通知する
		VLcnt = N60Win ? VLINES60 : VLINES62;	// 表示ラインカウント初期化
		vm->EventReset( this->Device::GetID(), EID_HDISPS, (double)( N60Win ? Hclk60 : HCLOCK62 ) / (double)HSdclk );
		vm->EventReset( this->Device::GetID(), EID_HDISPE );
		break;
		
	case EID_VSYNCE:	// VSYNC終了
		VSYNC = false;
		break;
		
	case EID_HDISPS:	// 表示区間開始
		if( VLcnt ){
			BusReq = CrtDisp ? true : false;
			VLcnt--;
		}
		break;
		
	case EID_HDISPE:	// 表示区間終了
		BusReq = false;
		break;
	}
}


void VDG64::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_VSYNCS:	// VSYNC
		VSYNC = true;
		vm->EventOnVSYNC();				// VSYNCを通知する
		VLcnt = N60Win ? VLINES60 : VLINES62;	// 表示ラインカウント初期化
		vm->EventReset( this->Device::GetID(), EID_HDISPS, (double)( N60Win ? Hclk60 : HCLOCK64 ) / (double)HSdclk );
		vm->EventReset( this->Device::GetID(), EID_HDISPE );
		break;
		
	case EID_VSYNCE:	// VSYNC終了
		VSYNC = false;
		vm->IntReqIntr(IREQ_VRTC);		// VRTC割込み(立上りエッジで割込発生)
		break;
		
	case EID_HDISPS:	// 表示区間開始
		if( VLcnt ){
			BusReq = CrtDisp ? true : false;
			VLcnt--;
		}
		break;
		
	case EID_HDISPE:	// 表示区間終了
		BusReq = false;
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// バックバッファ作成
/////////////////////////////////////////////////////////////////////////////
bool VDG6::CreateBuffer( void )
{
	return VSurface::InitSurface( GetVideoInfo().w * (IsSRHires() ? 2 : 1), GetVideoInfo().h );
}


/////////////////////////////////////////////////////////////////////////////
// バッファ取得
/////////////////////////////////////////////////////////////////////////////
std::vector<BYTE>& VDG6::GetBufAddr( void )
{
	return VSurface::GetPixels();
}


/////////////////////////////////////////////////////////////////////////////
// バッファピッチ(1Lineバイト数)取得
/////////////////////////////////////////////////////////////////////////////
int VDG6::GetBufPitch( void ) const
{
	return VSurface::Pitch();
}


/////////////////////////////////////////////////////////////////////////////
// アトリビュートデータラッチ
/////////////////////////////////////////////////////////////////////////////
void VDG6::LatchAttr( void )
{
	BYTE attr = GetAttr();
	
	AT_AG  = ( attr >> 7 ) & 1;
	AT_AS  = ( attr >> 6 ) & 1;
	AT_IE  = ( attr >> 5 ) & 1;
	AT_GM  = ( ( attr >> 2 ) & 4 ) | ( ( attr >> 1 ) & 2 ) | ( ( attr >> 3 ) & 1 );
	AT_CSS = ( attr >> 1 ) & 1;
	AT_INV =   attr        & 1;
}


/////////////////////////////////////////////////////////////////////////////
// アトリビュートデータラッチ(グラフィックモードのみ)
/////////////////////////////////////////////////////////////////////////////
void VDG6::LatchGMODE( void )
{
	BYTE attr = GetAttr();
	
	AT_AG  = ( attr >> 7 ) & 1;
	AT_GM  = ( ( attr >> 2 ) & 4 ) | ( ( attr >> 1 ) & 2 ) | ( ( attr >> 3 ) & 1 );
}


/////////////////////////////////////////////////////////////////////////////
// アトリビュートデータ取得
/////////////////////////////////////////////////////////////////////////////
BYTE VDG60::GetAttr( void ) const
{
	WORD addr = GerAttrAddr() + (( VAddr * 32 + HAddr ) & 0x01ff);
	return addr < 0xc000 ? vm->MemReadExtRam( addr ) : vm->MemReadIntRam( addr );
}

BYTE VDG62::GetAttr( void ) const
{
	WORD addr = ( VAddr * ( N60Win ? 32 : 40 ) + HAddr ) & ( N60Win ? 0x01ff : 0x1fff );
	return vm->MemReadIntRam( GerAttrAddr() +  addr );
}

BYTE VDG64::GetAttr( void ) const
{
	WORD addr = ( VAddr * ( N60Win ? 32 : 40 ) + HAddr ) & ( N60Win ? 0x01ff : 0x1fff );
	return vm->MemReadIntRam( GerAttrAddr() +  addr );
}


/////////////////////////////////////////////////////////////////////////////
// VRAMデータ取得 (表示)
/////////////////////////////////////////////////////////////////////////////
BYTE VDG60::GetVram( void ) const
{
	WORD addr = GetVramAddr() + VAddr * 32 + HAddr;
	return addr < 0xc000 ? vm->MemReadExtRam( addr ) : vm->MemReadIntRam( addr );
}

BYTE VDG62::GetVram( void ) const
{
	WORD addr = VAddr * ( N60Win ? 32 : 40 ) + HAddr;
	return vm->MemReadIntRam( GetVramAddr() + addr );
}

BYTE VDG64::GetVram( void ) const
{
	WORD addr = 0;
	
	if( SRmode ){
		if( CharMode ){		// テキストモード
			// HAddrは8dot/320毎
			addr = VAddr * ( SRCharWidth ? 40 : 80 ) * 2 + HAddr;
		}else{				// ビットマップモード
			WORD Had = HAddr + (SRRollX & (GraphMode ? 0xffff : 0xfffc));
			WORD Vad = VAddr +  SRRollY;
			
			while( Had >= 320 ) Had -= 320;
			while( Vad >= 204 ) Vad -= 204;
			
			// HAddrは1dot毎@320 or 4dot/2byte毎@640
			if( Had < 256 ){
				addr =  Had      + ((Vad>>1) * 256);
			}else{
				Vad = (Vad&0xfff1)|((Vad&2)<<2)|((Vad&0xc)>>1);	// bit1,2,3を入替える
				addr = (Had-256) + ((Vad>>1) *  64);
			}
			addr = ((addr&0xfffc) | ((Vad&1)<<1) | ((Had&2)>>1)) + ( Had < 256 ? 0x1a00 : 0 );
		}
	}else{
		// HAddrは8dot毎
		addr = VAddr * ( N60Win ? 32 : 40 ) + HAddr;
	}
	return vm->MemReadIntRam( GetVramAddr() + addr );
}


/////////////////////////////////////////////////////////////////////////////
// Font1データ取得
/////////////////////////////////////////////////////////////////////////////
BYTE VDG6::GetFont1( WORD addr ) const
{
	return vm->MemReadCGrom1( addr );
}


/////////////////////////////////////////////////////////////////////////////
// Font2データ取得
/////////////////////////////////////////////////////////////////////////////
BYTE VDG6::GetFont2( WORD addr ) const
{
	return vm->MemReadCGrom2( addr );
}


/////////////////////////////////////////////////////////////////////////////
// Font3データ取得
/////////////////////////////////////////////////////////////////////////////
BYTE VDG6::GetFont3( WORD addr ) const
{
	return vm->MemReadCGrom3( addr );
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool VDG6::Init( void )
{
	EVSC::evinfo e;
	
	PRINTD( VDG_LOG, "[VDG][Init]\n" );
	
	// イベント追加
	if( !vm->EventAdd( Device::GetID(), EID_VSYNCS, VSYNC_HZ,          EV_LOOP|EV_HZ ) ) return false;
	if( !vm->EventAdd( Device::GetID(), EID_VSYNCE, VSYNC_HZ,          EV_LOOP|EV_HZ ) ) return false;
	if( !vm->EventAdd( Device::GetID(), EID_HDISPS, VSYNC_HZ * VSLINE, EV_LOOP|EV_HZ ) ) return false;
	if( !vm->EventAdd( Device::GetID(), EID_HDISPE, VSYNC_HZ * VSLINE, EV_LOOP|EV_HZ ) ) return false;
	
	// VSYNC終了タイミングを合わせる
	e.devid = this->Device::GetID();
	e.id    = EID_VSYNCE;
	vm->EventGetInfo( &e );
	e.Clock = (e.Clock * 3) / VSLINE;
	vm->EventSetInfo( &e );
	
	// バックバッファ作成
	return CreateBuffer();

}


/////////////////////////////////////////////////////////////////////////////
// リセット
/////////////////////////////////////////////////////////////////////////////
void VDG6::Reset()
{
	PRINTD( VDG_LOG, "[VDG][Reset]\n" );
	SRmode = false;
}

void VDG64::Reset()
{
	PRINTD( VDG_LOG, "[VDG][Reset]\n" );
	SRmode = true;
}


/////////////////////////////////////////////////////////////////////////////
// バスリクエスト区間停止フラグ取得
/////////////////////////////////////////////////////////////////////////////
bool VDG6::IsBusReqStop( void ) const
{
	// SRの場合，BusReq,SRBusReq 両方のフラグが立っていればバスリクエスト発生
	return BusReq && SRBusReq;
}


/////////////////////////////////////////////////////////////////////////////
// バスリクエスト区間実行フラグ取得
/////////////////////////////////////////////////////////////////////////////
bool VDG6::IsBusReqExec( void ) const
{
	return BusReq && (!SRBusReq);
}


/////////////////////////////////////////////////////////////////////////////
// SRのG-VRAMアドレス取得 (ビットマップモード)
/////////////////////////////////////////////////////////////////////////////
WORD VDG6::SRGVramAddr( WORD addr ) const
{
//	WORD ad = SRmode && SRBMPage ? 0x8000 : 0x0000;
	WORD ad = SRBMPage ? 0x8000 : 0x0000;
	WORD hh = addr        & 0x03ff;	// 10bit有効
	WORD vv = SRVramAddrY & 0x01ff;	//  9bit有効
	
	while( hh >= 320 ) hh -= 320;
	while( vv >= 204 ) vv -= 204;
	
	if( hh < 256 ){	// X = 0-255
		ad += hh       + (vv>>1) * 256 + 0x1a00;
	}else{			// X = 256-319
		vv  = (vv&0xff1) | ((vv&2)<<2) | ((vv&0xc)>>1);	// bit1,2,3を入替える
		ad += hh - 256 + (vv>>1) *  64;
	}
	ad = (ad&0xfffc) + ((vv&1)<<1) + ((hh&2)>>1);
	
	return ad;
}


/////////////////////////////////////////////////////////////////////////////
// VRAMアドレス取得 (メモリアクセス,表示)
/////////////////////////////////////////////////////////////////////////////
WORD VDG60::GetVramAddr( void ) const
{
	return ( 0x8000 | AddrOff ) + 0x0200;	//	[00]C200H  [01]E200H  [10]8200H  [11]A200H
}

WORD VDG62::GetVramAddr( void ) const
{
	if( N60Win )			// N60  [00]C200H  [01]E200H  [10]8200H  [11]A200H
		return ( 0x8000 | AddrOff ) + 0x0200;
	else{					// N60m
		if( CharMode ) return (AddrOff<<1) + 0x0400;	// キャラクタモード   [00]8400H  [01]C400H  [10]0400H  [11]4400H
		else		   return (AddrOff<<1) + 0x2000;	// グラフィックモード [00]A000H  [01]E000H  [10]2000H  [11]6000H
	}
}

WORD VDG64::GetVramAddr( void ) const
{
	if( SRmode ){	// SRモード
		return	(WORD)(SRTextAddr & (CharMode ? 0x0f : 0x08))<<12;
		
	}else{			// 旧モード
		if( N60Win )			// N60  [00]C200H  [01]E200H  [10]8200H  [11]A200H
			return AddrOff + 0x0200;
		else{					// N60m
			if( CharMode ) return AddrOff + 0x0400;	// キャラクタモード   [00]8400H  [01]C400H  [10]0400H  [11]4400H
			else		   return AddrOff + 0x2000;	// グラフィックモード [00]A000H  [01]E000H  [10]2000H  [11]6000H
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// ATTRアドレス取得 (表示)
/////////////////////////////////////////////////////////////////////////////
WORD VDG60::GerAttrAddr( void ) const
{
	return 0x8000 | AddrOff;				// [00]C000H  [01]E000H  [10]8000H  [11]A000H
}

WORD VDG62::GerAttrAddr( void ) const
{
	if( N60Win ) return 0x8000 | AddrOff;	// N60  [00]C000H  [01]E000H  [10]8000H  [11]A000H
	else		 return AddrOff<<1;			// N60m [00]8000H  [01]C000H  [10]0000H  [11]4000H
}

WORD VDG64::GerAttrAddr( void ) const
{
	if( SRmode ) return GetVramAddr() + 1;	// SRモード(テキストモードアクセスのみ)
	else		 return AddrOff;			// 旧モード
}


/////////////////////////////////////////////////////////////////////////////
// ATTRアドレス設定
/////////////////////////////////////////////////////////////////////////////
void VDG6::SetAttrAddr( BYTE data )
{
	PRINTD( VDG_LOG, "[VDG][SetAttrAddr]" );
	AddrOff = ((~data&4)|(data&2))<<12;
	PRINTD( VDG_LOG, " %d%d -> %04X\n", data&4 ? 1 : 0, data&2 ? 1 : 0, AddrOff );
}

void VDG64::SetAttrAddr( BYTE data )
{
	// SRの場合はポート書込み時の画面モードでアドレスが決まるようだ
	// N60  [00]C000H  [01]E000H  [10]8000H  [11]A000H
	// N60m [00]8000H  [01]C000H  [10]0000H  [11]4000H
	PRINTD( VDG_LOG, "[VDG][SetAttrAddr]" );
//	if( !SRmode ){		// SRモードの時は無効?わからんのでとりあえず有効にしておく
		AddrOff = ((~data&4)|(data&2))<<12;
		if( N60Win ) AddrOff |= 0x8000;	// N60
		else		 AddrOff <<= 1;		// N60m
		
		PRINTD( VDG_LOG, " %d%d -> %04X", data&4 ? 1 : 0, data&2 ? 1 : 0, AddrOff );
//	}
	PRINTD( VDG_LOG, "\n" );
}


/////////////////////////////////////////////////////////////////////////////
// I/Oアクセス関数
/////////////////////////////////////////////////////////////////////////////
void VDG6::OutB0H( int, BYTE data ){ SetAttrAddr( data ); }
void VDG6::OutC0H( int, BYTE data ){ SetCss( data ); }
void VDG6::OutC1H( int, BYTE data )
{
	SetCrtControler( data );
	CreateBuffer();
}

void VDG64::Out4xH( int port, BYTE data ){ SetPalette( 15-(port & 3), 15-(data & 0xf) ); }
void VDG64::OutC8H( int, BYTE data ){ SetCrtCtrlType( data ); }
void VDG64::OutC9H( int, BYTE data ){ SRTextAddr = data & 0x0f; }
void VDG64::OutCAH( int, BYTE data ){ SRRollX = (SRRollX & 0xff00) |   (WORD)data; }
void VDG64::OutCBH( int, BYTE data ){ SRRollX = (SRRollX & 0x00ff) | (((WORD)data & 3) << 8); }
void VDG64::OutCCH( int, BYTE data ){ SRRollY = (SRRollY & 0xff00) |   (WORD)data; }
void VDG64::OutCDH( int, BYTE data ){ SRRollY = (SRRollY & 0x00ff) | (((WORD)data & 0) << 8); }
void VDG64::OutCEH( int, BYTE data ){ SRVramAddrY = (SRVramAddrY & 0xff00) |   (WORD)data; }
void VDG64::OutCFH( int, BYTE data ){ SRVramAddrY = (SRVramAddrY & 0x00ff) | (((WORD)data & 1) << 8); }

BYTE VDG6::InA2H( int ){ return (VSYNC ? 0 : 0x80) | (HSYNC ? 0 : 0x40) | 0x3f; }


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
/////////////////////////////////////////////////////////////////////////////
bool VDG6::DokoSave( cIni* Ini )
{
	if( !Ini ) return false;
	
	// Core
	Ini->SetVal( "VDG", "CrtDisp",		"", CrtDisp );
	Ini->SetVal( "VDG", "BusReq",		"", BusReq  );
	Ini->SetVal( "VDG", "N60Win",		"", N60Win  );
	Ini->SetVal( "VDG", "VAddr",		"", "0x%04X", VAddr );
	Ini->SetVal( "VDG", "HAddr",		"", "0x%04X", HAddr );
	Ini->SetVal( "VDG", "RowCntA",		"", RowCntA );
	Ini->SetVal( "VDG", "RowCntG",		"", RowCntG );
	Ini->SetVal( "VDG", "AT_AG",		"", "0x%02X", AT_AG );
	Ini->SetVal( "VDG", "AT_AS",		"", "0x%02X", AT_AS );
	Ini->SetVal( "VDG", "AT_IE",		"", "0x%02X", AT_IE );
	Ini->SetVal( "VDG", "AT_GM",		"", "0x%02X", AT_GM );
	Ini->SetVal( "VDG", "AT_CSS",		"", "0x%02X", AT_CSS );
	Ini->SetVal( "VDG", "AT_INV",		"", "0x%02X", AT_INV );
	
	// 62,66,64,68
	Ini->SetVal( "VDG", "CharMode",		"",	CharMode  );
	Ini->SetVal( "VDG", "GraphMode",	"",	GraphMode );
	Ini->SetVal( "VDG", "Css1",			"",	Css1 );
	Ini->SetVal( "VDG", "Css2",			"",	Css2 );
	Ini->SetVal( "VDG", "Css3",			"",	Css3 );
	
	// 64,68
	Ini->SetVal( "VDG", "SRmode",		"",	SRmode      );
	Ini->SetVal( "VDG", "SRBusReq",		"",	SRBusReq    );
	Ini->SetVal( "VDG", "SRBitmap",		"",	SRBitmap    );
	Ini->SetVal( "VDG", "SRBMPage",		"",	SRBMPage    );
	Ini->SetVal( "VDG", "SRLine204",	"",	SRLine204   );
	Ini->SetVal( "VDG", "SRCharLine",	"",	SRCharLine  );
	Ini->SetVal( "VDG", "SRCharWidth",	"",	SRCharWidth );
	Ini->SetVal( "VDG", "SRTextAddr",	"", "0x%02X", SRTextAddr );
	Ini->SetVal( "VDG", "SRRollX",		"",	SRRollX );
	Ini->SetVal( "VDG", "SRRollY",		"",	SRRollY );
	Ini->SetVal( "VDG", "SRVramAddrY",	"", "0x%04X", SRVramAddrY );
	
	// VDG6
	Ini->SetVal( "VDG", "AddrOff",		"", "0x%04X", AddrOff );
	Ini->SetVal( "VDG", "VSYNC",		"",	VSYNC );
	Ini->SetVal( "VDG", "HSYNC",		"",	HSYNC );
	Ini->SetVal( "VDG", "VLcnt",		"",	VLcnt );
	
	for( int i=0; i<16; i++ ){
		Ini->SetVal( "VDG", Stringf( "COL_AN2_%02d", i ),	"",	COL_AN2[i] );
		Ini->SetVal( "VDG", Stringf( "COL_CG2_0_%02d", i ),	"",	COL_CG2[0][i] );
		Ini->SetVal( "VDG", Stringf( "COL_CG2_1_%02d", i ),	"",	COL_CG2[1][i] );
	}
	
	// VSurface
	Ini->SetVal( "VDG", "VSurface_w",		"",	VSurface::w      );
	Ini->SetVal( "VDG", "VSurface_h",		"",	VSurface::h      );
	Ini->SetVal( "VDG", "VSurface_pitch",	"",	VSurface::pitch  );
	Ini->SetVal( "VDG", "VRect_x",			"",	VSurface::rect.x );
	Ini->SetVal( "VDG", "VRect_y",			"",	VSurface::rect.y );
	Ini->SetVal( "VDG", "VRect_w",			"",	VSurface::rect.w );
	Ini->SetVal( "VDG", "VRect_h",			"",	VSurface::rect.h );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
/////////////////////////////////////////////////////////////////////////////
bool VDG6::DokoLoad( cIni* Ini )
{
	if( !Ini ) return false;
	
	// Core
	Ini->GetVal( "VDG", "CrtDisp",		CrtDisp );
	Ini->GetVal( "VDG", "BusReq",		BusReq  );
	Ini->GetVal( "VDG", "N60Win",		N60Win  );
	Ini->GetVal( "VDG", "VAddr",		VAddr   );
	Ini->GetVal( "VDG", "HAddr",		HAddr   );
	Ini->GetVal( "VDG", "RowCntA",		RowCntA );
	Ini->GetVal( "VDG", "RowCntG",		RowCntG );
	Ini->GetVal( "VDG", "AT_AG",		AT_AG   );
	Ini->GetVal( "VDG", "AT_AS",		AT_AS   );
	Ini->GetVal( "VDG", "AT_IE",		AT_IE   );
	Ini->GetVal( "VDG", "AT_GM",		AT_GM   );
	Ini->GetVal( "VDG", "AT_CSS",		AT_CSS  );
	Ini->GetVal( "VDG", "AT_INV",		AT_INV  );
	
	// 62,66,64,68
	Ini->GetVal( "VDG", "CharMode",		CharMode  );
	Ini->GetVal( "VDG", "GraphMode",	GraphMode );
	Ini->GetVal( "VDG", "Css1",			Css1      );
	Ini->GetVal( "VDG", "Css2",			Css2      );
	Ini->GetVal( "VDG", "Css3",			Css3      );
	
	// 64,68
	Ini->GetVal( "VDG", "SRmode",		SRmode      );
	Ini->GetVal( "VDG", "SRBusReq",		SRBusReq    );
	Ini->GetVal( "VDG", "SRBitmap",		SRBitmap    );
	Ini->GetVal( "VDG", "SRBMPage",		SRBMPage    );
	Ini->GetVal( "VDG", "SRLine204",	SRLine204   );
	Ini->GetVal( "VDG", "SRCharLine",	SRCharLine  );
	Ini->GetVal( "VDG", "SRCharWidth",	SRCharWidth );
	Ini->GetVal( "VDG", "SRTextAddr",	SRTextAddr  );
	Ini->GetVal( "VDG", "SRRollX",		SRRollX     );
	Ini->GetVal( "VDG", "SRRollY",		SRRollY     );
	Ini->GetVal( "VDG", "SRVramAddrY",	SRVramAddrY );
	
	// VDG6
	Ini->GetVal( "VDG", "AddrOff",		AddrOff );
	Ini->GetVal( "VDG", "VSYNC",		VSYNC   );
	Ini->GetVal( "VDG", "HSYNC",		HSYNC   );
	Ini->GetVal( "VDG", "VLcnt",		VLcnt   );
	
	for( int i=0; i<16; i++ ){
		Ini->GetVal( "VDG", Stringf( "COL_AN2_%02d",   i ),	COL_AN2[i]    );
		Ini->GetVal( "VDG", Stringf( "COL_CG2_0_%02d", i ),	COL_CG2[0][i] );
		Ini->GetVal( "VDG", Stringf( "COL_CG2_1_%02d", i ),	COL_CG2[1][i] );
	}
	
	// VSurface
	Ini->GetVal( "VDG", "VSurface_w",		VSurface::w      );
	Ini->GetVal( "VDG", "VSurface_h",		VSurface::h      );
	Ini->GetVal( "VDG", "VSurface_pitch",	VSurface::pitch  );
	Ini->GetVal( "VDG", "VRect_x",			VSurface::rect.x );
	Ini->GetVal( "VDG", "VRect_y",			VSurface::rect.y );
	Ini->GetVal( "VDG", "VRect_w",			VSurface::rect.w );
	Ini->GetVal( "VDG", "VRect_h",			VSurface::rect.h );
	VSurface::pixels.resize( VSurface::pitch * VSurface::h );
	
	return true;
}

