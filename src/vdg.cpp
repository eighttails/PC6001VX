#include "pc6001v.h"
#include "log.h"
#include "intr.h"
#include "vdg.h"
#include "p6el.h"
#include "p6vm.h"


// イベントID
#define	EID_VSYNCS	1			/* 垂直同期開始 */
#define	EID_VSYNCE	2			/* 垂直同期終了 */
#define	EID_HDISPS	3			/* 表示区間開始 */
#define	EID_HDISPE	4			/* 表示区間終了 */

#define	VSLINE		262			/* 垂直トータルライン   */
#define	VLINES60	192			/* 垂直表示ライン(N60)  */
#define	VLINES62	200			/* 垂直表示ライン(N60m) */

#define	HSDCLK60	455			/* 水平トータル期間(ドットクロック) 60    		*/
#define	HSDCLK62	456			/* 水平トータル期間(ドットクロック) 62,66,64,68 */
#define	HCLK6060	(256+40)	/* 水平表示期間(N60)  60    */
#define	HCLK6062	(256+48)	/* 水平表示期間(N60)  62,66 */
#define	HCLK6064	(256+ 8)	/* 水平表示期間(N60)  64,68 */
#define	HCLOCK62	(320+48)	/* 水平表示期間(N60m) 62,66	*/
#define	HCLOCK64	(320+16)	/* 水平表示期間(N60m) 64,68	*/



// カラーコード
// --------- 60 ---------
const BYTE VDG6::COL60_AN[] =			// mode 1 -----
{ 17,18,19,20,16 };

const BYTE VDG6::COL60_SG[] =			// mode 2 -----
{ 21,22,23,24,25,26,27,28,16 };

const BYTE VDG6::COL60_CG[][8] = {		// mode 3 -----
										{ 29,30,31,32, 0, 0, 0, 0 },
										{ 33,34,35,36, 0, 0, 0, 0 },
										{ 37,41,42,38,45,46,47,48 },	// mode4(Set1) Jにじみ
										{ 39,53,54,40,57,58,59,60 },	// mode4(Set2) Jにじみ
										{ 37,42,41,38,47,48,45,46 },	// mode4(Set1) Jにじみ
										{ 39,54,53,40,59,60,57,58 },	// mode4(Set2) Jにじみ
										{ 37,43,44,38,49,50,51,52 },	// mode4(Set1) Jにじみ
										{ 39,55,56,40,61,62,63,64 },	// mode4(Set2) Jにじみ
										{ 37,44,43,38,51,52,49,50 },	// mode4(Set1) Jにじみ
										{ 39,56,55,40,63,64,61,62 }		// mode4(Set2) Jにじみ
								 };

const BYTE VDG6::COL60_RG[][2] = {		// mode 4 -----
										{ 37,38 },
										{ 39,40 }
								 };

// --------- mk2 ---------
const BYTE VDG6::COL62_AN[] =			// mode 1-1 -----
{ 80,73,75,73,73 };

const BYTE VDG6::COL62_SG[] =			// mode 1-2 -----
{ 75,76,77,74,80,79,78,66,73 };

const BYTE VDG6::COL62_CG[][8] = {		// mode 1-3 -----
										{ 75,76,77,74, 0, 0, 0, 0 },	// mode1-4(Set1) Jにじみ
										{ 80,79,78,66, 0, 0, 0, 0 },	// mode1-4(Set1) Jにじみ
										{ 73,41,42,75,45,46,47,48 },	// mode1-4(Set1) Jにじみ
										{ 73,53,54,80,57,58,59,60 },	// mode1-4(Set1) Jにじみ
										{ 73,42,41,75,47,48,45,46 },	// mode1-4(Set1) Jにじみ
										{ 73,54,53,80,59,60,57,58 },	// mode1-4(Set1) Jにじみ
										{ 73,43,44,75,49,50,51,52 },	// mode1-4(Set1) Jにじみ
										{ 73,55,56,80,61,62,63,64 },	// mode1-4(Set1) Jにじみ
										{ 73,44,43,75,51,52,49,50 },	// mode1-4(Set1) Jにじみ
										{ 73,56,55,80,63,64,61,62 }		// mode1-4(Set1) Jにじみ
								 };

const BYTE VDG6::COL62_RG[][2] = {		// mode 1-4 -----
										{ 73,75 },
										{ 73,80 }
								 };

const BYTE VDG6::COL62_AN2[] =			// mode 2-1,2 -----
{ 65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80 };

const BYTE VDG6::COL62_CG2[][16] = {	// mode 2-3,4 -----
										{ 65,69,66,70,67,71,68,72,73,77,74,78,75,79,76,80 },
										{ 75,76,77,74,80,79,78,66,75,76,77,74,80,79,78,66 }
								   };



////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
VDG6::VDG6( VM6 *vm, const ID& id ) : Device(vm,id),
	AddrOff(0), VSYNC(false), HSYNC(false), VLcnt(VLINES60) {}

VDG60::VDG60( VM6 *vm, const ID& id ) : VDG6(vm,id)
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
}

VDG62::VDG62( VM6 *vm, const ID& id ) : VDG6(vm,id)
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
}

VDG64::VDG64( VM6 *vm, const ID& id ) : VDG6(vm,id)
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
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
VDG6::~VDG6( void ){}

VDG60::~VDG60( void ){}

VDG62::~VDG62( void ){}

VDG64::~VDG64( void ){}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////
// バックバッファ作成
////////////////////////////////////////////////////////////////
bool VDG6::CreateBuffer( int xsc )
{
	return VSurface::InitSurface( MC6847core::GetW(), MC6847core::GetH(), xsc );
}


////////////////////////////////////////////////////////////////
// バッファアドレス取得
////////////////////////////////////////////////////////////////
BYTE *VDG6::GetBufAddr( void ) const
{
	return (BYTE *)VSurface::pixels;
}


////////////////////////////////////////////////////////////////
// バッファピッチ(1Lineバイト数)取得
////////////////////////////////////////////////////////////////
int VDG6::GetBufPitch( void ) const
{
	return VSurface::Pitch();
}


////////////////////////////////////////////////////////////////
// アトリビュートデータラッチ
////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////
// アトリビュートデータラッチ(グラフィックモードのみ)
////////////////////////////////////////////////////////////////
void VDG6::LatchGMODE( void )
{
	BYTE attr = GetAttr();
	
	AT_AG  = ( attr >> 7 ) & 1;
	AT_GM  = ( ( attr >> 2 ) & 4 ) | ( ( attr >> 1 ) & 2 ) | ( ( attr >> 3 ) & 1 );
}


////////////////////////////////////////////////////////////////
// アトリビュートデータ取得
////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////
// VRAMデータ取得 (表示)
////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////
// Font1データ取得
////////////////////////////////////////////////////////////////
BYTE VDG6::GetFont1( WORD addr ) const
{
	return vm->MemReadCGrom1( addr );
}


////////////////////////////////////////////////////////////////
// Font2データ取得
////////////////////////////////////////////////////////////////
BYTE VDG6::GetFont2( WORD addr ) const
{
	return vm->MemReadCGrom2( addr );
}


////////////////////////////////////////////////////////////////
// Font3データ取得
////////////////////////////////////////////////////////////////
BYTE VDG6::GetFont3( WORD addr ) const
{
	return vm->MemReadCGrom3( addr );
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool VDG6::Init( void )
{
	EVSC::evinfo e;
	
	PRINTD( VDG_LOG, "[VDG][Init]\n" );
	
	// イベント追加
	if( !vm->EventAdd( this, EID_VSYNCS, VSYNC_HZ,        EV_LOOP|EV_HZ ) ) return false;
	if( !vm->EventAdd( this, EID_VSYNCE, VSYNC_HZ,        EV_LOOP|EV_HZ ) ) return false;
	if( !vm->EventAdd( this, EID_HDISPS, VSYNC_HZ*VSLINE, EV_LOOP|EV_HZ ) ) return false;
	if( !vm->EventAdd( this, EID_HDISPE, VSYNC_HZ*VSLINE, EV_LOOP|EV_HZ ) ) return false;
	
	// VSYNC終了タイミングを合わせる
	e.devid = this->Device::GetID();
	e.id    = EID_VSYNCE;
	vm->EventGetInfo( &e );
	e.Clock = (e.Clock*3)/VSLINE;
	vm->EventSetInfo( &e );
	
	// バックバッファ作成
	return CreateBuffer();

}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////
// バスリクエスト区間停止フラグ取得
////////////////////////////////////////////////////////////////
bool VDG6::IsBusReqStop( void ) const
{
	// SRの場合，BusReq,SRBusReq 両方のフラグが立っていればバスリクエスト発生
	return BusReq && SRBusReq;
}


////////////////////////////////////////////////////////////////
// バスリクエスト区間実行フラグ取得
////////////////////////////////////////////////////////////////
bool VDG6::IsBusReqExec( void ) const
{
	return BusReq && (!SRBusReq);
}


////////////////////////////////////////////////////////////////
// SRモード取得
////////////////////////////////////////////////////////////////
bool VDG6::IsSRmode( void ) const
{
	return SRmode;
}


////////////////////////////////////////////////////////////////
// SRのG-VRAMアクセス?
////////////////////////////////////////////////////////////////
bool VDG6::IsSRGVramAccess( WORD addr ) const
{
//	return SRmode && SRBitmap && (SRBMPage == (addr&0x8000));
	return SRmode && SRBitmap;
}


////////////////////////////////////////////////////////////////
// SRのG-VRAMアドレス取得 (メモリアクセス)
////////////////////////////////////////////////////////////////
WORD VDG6::SRGVramAddr( WORD addr ) const
{
	//	WORD ad = GetVramAddr();
	WORD ad = SRmode && SRBMPage ? 0x8000 : 0x0000;

	WORD hh = addr & 0x1ff;
	WORD vv = SRVramAddrY;
	
	while( vv >= 204 ) vv -= 204;
	
	if( hh < 256 ){
		ad += hh       + (vv>>1) * 256 + 0x1a00;
	}else{
		vv  = (vv&0xff1) | ((vv&2)<<2) | ((vv&0xc)>>1);	// bit1,2,3を入替える
		ad += hh - 256 + (vv>>1) *  64;
	}
	ad = (ad&0xfffc) + ((vv&1)<<1) + ((hh&2)>>1);
	
	return ad;
}


////////////////////////////////////////////////////////////////
// VRAMアドレス取得 (メモリアクセス,表示)
////////////////////////////////////////////////////////////////
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
		if( CharMode ) return (WORD)SRTextAddr<<12;			// テキストモード
		else		   return (SRBMPage ? 0x8000 : 0x0000);	// ビットマップモード
	}else{			// 旧モード
		if( N60Win )			// N60  [00]C200H  [01]E200H  [10]8200H  [11]A200H
			return AddrOff + 0x0200;
		else{					// N60m
			if( CharMode ) return AddrOff + 0x0400;	// キャラクタモード   [00]8400H  [01]C400H  [10]0400H  [11]4400H
			else		   return AddrOff + 0x2000;	// グラフィックモード [00]A000H  [01]E000H  [10]2000H  [11]6000H
		}
	}
}


////////////////////////////////////////////////////////////////
// ATTRアドレス取得 (表示)
////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////
// ATTRアドレス設定
////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
void VDG6::OutB0H( int, BYTE data ){ SetAttrAddr( data ); }
void VDG6::OutC0H( int, BYTE data ){ SetCss( data ); }
void VDG6::OutC1H( int, BYTE data )
{
	SetCrtControler( data );
	CreateBuffer( (SRmode && ((CharMode && !SRCharWidth) || (!CharMode && !GraphMode))) ? 2 : 1 );
}

void VDG64::Out4xH( int port, BYTE data ){ SetPalette( 15-(port&3), 15-(data&0xf) ); }
void VDG64::OutC8H( int, BYTE data ){ SetCrtCtrlType( data ); }
void VDG64::OutC9H( int, BYTE data ){ SRTextAddr = data&0x0f; }
void VDG64::OutCAH( int, BYTE data ){ SRRollX = (SRRollX&0xff00) | (WORD)data; }
void VDG64::OutCBH( int, BYTE data ){ SRRollX = (SRRollX&0x00ff) | (((WORD)data&3)<<8); }
void VDG64::OutCCH( int, BYTE data ){ SRRollY = (WORD)data; }
void VDG64::OutCEH( int, BYTE data ){ SRVramAddrY = (SRVramAddrY&0xff00) | (WORD)data; }
void VDG64::OutCFH( int, BYTE data ){ SRVramAddrY = (SRVramAddrY&0x00ff) | (((WORD)data&1)<<8); }

BYTE VDG6::InA2H( int ){ return (VSYNC ? 0 : 0x80) | (HSYNC ? 0 : 0x40) | 0x3f; }


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
bool VDG6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	// Core
	Ini->PutEntry( "VDG", NULL, "CrtDisp",		"%s",		CrtDisp ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "BusReq",		"%s",		BusReq  ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "N60Win",		"%s",		N60Win  ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "VAddr",		"0x%04X",	VAddr );
	Ini->PutEntry( "VDG", NULL, "HAddr",		"0x%04X",	HAddr );
	Ini->PutEntry( "VDG", NULL, "RowCntA",		"%d",		RowCntA );
	Ini->PutEntry( "VDG", NULL, "RowCntG",		"%d",		RowCntG );
	Ini->PutEntry( "VDG", NULL, "AT_AG",		"0x%02X",	AT_AG );
	Ini->PutEntry( "VDG", NULL, "AT_AS",		"0x%02X",	AT_AS );
	Ini->PutEntry( "VDG", NULL, "AT_IE",		"0x%02X",	AT_IE );
	Ini->PutEntry( "VDG", NULL, "AT_GM",		"0x%02X",	AT_GM );
	Ini->PutEntry( "VDG", NULL, "AT_CSS",		"0x%02X",	AT_CSS );
	Ini->PutEntry( "VDG", NULL, "AT_INV",		"0x%02X",	AT_INV );
	
	// 62,66,64,68
	Ini->PutEntry( "VDG", NULL, "CharMode",		"%s",		CharMode  ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "GraphMode",	"%s",		GraphMode ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "Css1",			"%d",		Css1 );
	Ini->PutEntry( "VDG", NULL, "Css2",			"%d",		Css2 );
	Ini->PutEntry( "VDG", NULL, "Css3",			"%d",		Css3 );
	
	// 64,68
	Ini->PutEntry( "VDG", NULL, "SRmode",		"%s",		SRmode      ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "SRBusReq",		"%s",		SRBusReq    ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "SRBitmap",		"%s",		SRBitmap    ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "SRBMPage",		"%s",		SRBMPage    ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "SRLine204",	"%s",		SRLine204   ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "SRCharLine",	"%s",		SRCharLine  ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "SRCharWidth",	"%s",		SRCharWidth ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "SRTextAddr",	"0x%02X",	SRTextAddr );
	Ini->PutEntry( "VDG", NULL, "SRRollX",		"%d",		SRRollX );
	Ini->PutEntry( "VDG", NULL, "SRRollY",		"%d",		SRRollY );
	Ini->PutEntry( "VDG", NULL, "SRVramAddrY",	"0x%04X",	SRVramAddrY );
	
	// VDG6
	Ini->PutEntry( "VDG", NULL, "AddrOff",		"0x%04X",	AddrOff );
	Ini->PutEntry( "VDG", NULL, "VSYNC",		"%s",		VSYNC  ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "HSYNC",		"%s",		HSYNC  ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "VLcnt",		"%d",		VLcnt );
	
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "COL_AN2_%02d", i );
		Ini->PutEntry( "VDG", NULL, stren,		"%d",		COL_AN2[i] );
		sprintf( stren, "COL_CG2_0_%02d", i );
		Ini->PutEntry( "VDG", NULL, stren,		"%d",		COL_CG2[0][i] );
		sprintf( stren, "COL_CG2_1_%02d", i );
		Ini->PutEntry( "VDG", NULL, stren,		"%d",		COL_CG2[1][i] );
	}
	
	// VSurface
	Ini->PutEntry( "VDG", NULL, "VSurface_w",		"%d",	VSurface::w			);
	Ini->PutEntry( "VDG", NULL, "VSurface_h",		"%d",	VSurface::h			);
	Ini->PutEntry( "VDG", NULL, "VSurface_pitch",	"%d",	VSurface::pitch		);
	Ini->PutEntry( "VDG", NULL, "VSurface_xscale",	"%d",	VSurface::xscale	);
	Ini->PutEntry( "VDG", NULL, "VRect_x",			"%d",	VSurface::rect.x	);
	Ini->PutEntry( "VDG", NULL, "VRect_y",			"%d",	VSurface::rect.y	);
	Ini->PutEntry( "VDG", NULL, "VRect_w",			"%d",	VSurface::rect.w	);
	Ini->PutEntry( "VDG", NULL, "VRect_h",			"%d",	VSurface::rect.h	);

	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
bool VDG6::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;

	// Core
	Ini->GetTruth( "VDG", "CrtDisp",		&CrtDisp,		CrtDisp );
	Ini->GetTruth( "VDG", "BusReq",			&BusReq,		BusReq );
	Ini->GetTruth( "VDG", "N60Win",			&N60Win,		N60Win );
	Ini->GetInt(   "VDG", "VAddr",			&st,			VAddr );	VAddr   = st;
	Ini->GetInt(   "VDG", "HAddr",			&st,			HAddr );	HAddr   = st;
	Ini->GetInt(   "VDG", "RowCntA",		&RowCntA,		RowCntA );
	Ini->GetInt(   "VDG", "RowCntG",		&RowCntG,		RowCntG );
	Ini->GetInt(   "VDG", "AT_AG",			&st,			AT_AG );	AT_AG   = st;
	Ini->GetInt(   "VDG", "AT_AS",			&st,			AT_AS );	AT_AS   = st;
	Ini->GetInt(   "VDG", "AT_IE",			&st,			AT_IE );	AT_IE   = st;
	Ini->GetInt(   "VDG", "AT_GM",			&st,			AT_GM );	AT_GM   = st;
	Ini->GetInt(   "VDG", "AT_CSS",			&st,			AT_CSS );	AT_CSS  = st;
	Ini->GetInt(   "VDG", "AT_INV",			&st,			AT_INV );	AT_INV  = st;
	
	// 62,66,64,68
	Ini->GetTruth( "VDG", "CharMode",		&CharMode,		CharMode );
	Ini->GetTruth( "VDG", "GraphMode",		&GraphMode,		GraphMode );
	Ini->GetInt(   "VDG", "Css1",			&Css1,			Css1 );
	Ini->GetInt(   "VDG", "Css2",			&Css2,			Css2 );
	Ini->GetInt(   "VDG", "Css3",			&Css3,			Css3 );

	// 64,68
	Ini->GetTruth( "VDG", "SRmode",			&SRmode,		SRmode );
	Ini->GetTruth( "VDG", "SRBusReq",		&SRBusReq,		SRBusReq );
	Ini->GetTruth( "VDG", "SRBitmap",		&SRBitmap,		SRBitmap );
	Ini->GetTruth( "VDG", "SRBMPage",		&SRBMPage,		SRBMPage );
	Ini->GetTruth( "VDG", "SRLine204",		&SRLine204,		SRLine204 );
	Ini->GetTruth( "VDG", "SRCharLine",		&SRCharLine,	SRCharLine );
	Ini->GetTruth( "VDG", "SRCharWidth",	&SRCharWidth,	SRCharWidth );
	Ini->GetInt(   "VDG", "SRTextAddr",		&st,			SRTextAddr );	SRTextAddr  = st;
	Ini->GetInt(   "VDG", "SRRollX",		&st,			SRRollX );		SRRollX     = st;
	Ini->GetInt(   "VDG", "SRRollY",		&st,			SRRollY );		SRRollY     = st;
	Ini->GetInt(   "VDG", "SRVramAddrY",	&st,			SRVramAddrY );	SRVramAddrY = st;
	
	// VDG6
	Ini->GetInt(   "VDG", "AddrOff",		&st,			AddrOff );	AddrOff = st;
	Ini->GetTruth( "VDG", "VSYNC",			&VSYNC,			VSYNC );
	Ini->GetTruth( "VDG", "HSYNC",			&HSYNC,			HSYNC );
	Ini->GetInt(   "VDG", "VLcnt",			&VLcnt,			VLcnt );
	
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "COL_AN2_%02d", i );
		Ini->GetInt( "VDG", stren,	&st,	COL_AN2[i] );		COL_AN2[i]    = st;
		sprintf( stren, "COL_CG2_0_%02d", i );
		Ini->GetInt( "VDG", stren,	&st,	COL_CG2[0][i] );	COL_CG2[0][i] = st;
		sprintf( stren, "COL_CG2_1_%02d", i );
		Ini->GetInt( "VDG", stren,	&st,	COL_CG2[1][i] );	COL_CG2[1][i] = st;
	}

	// VSurface
	Ini->GetInt( "VDG", "VSurface_w",		&st,	VSurface::w);		VSurface::w			= st;
	Ini->GetInt( "VDG", "VSurface_h",		&st,	VSurface::h);		VSurface::h			= st;
	Ini->GetInt( "VDG", "VSurface_pitch",	&st,	VSurface::pitch);	VSurface::pitch		= st;
	Ini->GetInt( "VDG", "VSurface_xscale",	&st,	VSurface::xscale);	VSurface::xscale	= st;
	Ini->GetInt( "VDG", "VRect_x",			&st,	VSurface::rect.x);	VSurface::rect.x	= st;
	Ini->GetInt( "VDG", "VRect_y",			&st,	VSurface::rect.y);	VSurface::rect.y	= st;
	Ini->GetInt( "VDG", "VRect_w",			&st,	VSurface::rect.w);	VSurface::rect.w	= st;
	Ini->GetInt( "VDG", "VRect_h",			&st,	VSurface::rect.h);	VSurface::rect.h	= st;
	delete[] (BYTE *)VSurface::pixels;
	VSurface::pixels = new BYTE[VSurface::pitch * VSurface::h];

	return true;
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor VDG60::descriptor = {
	VDG60::indef, VDG60::outdef
};

const Device::OutFuncPtr VDG60::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &VDG60::OutB0H )
};

const Device::InFuncPtr VDG60::indef[] = {
	NULL
};

const Device::Descriptor VDG62::descriptor = {
	VDG62::indef, VDG62::outdef
};

const Device::OutFuncPtr VDG62::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &VDG62::OutB0H ),
	STATIC_CAST( Device::OutFuncPtr, &VDG62::OutC0H ),
	STATIC_CAST( Device::OutFuncPtr, &VDG62::OutC1H )
};

const Device::InFuncPtr VDG62::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &VDG62::InA2H )
};



const Device::Descriptor VDG64::descriptor = {
	VDG64::indef, VDG64::outdef
};

const Device::OutFuncPtr VDG64::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &VDG64::Out4xH ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutB0H ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutC0H ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutC1H ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutC8H ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutC9H ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCAH ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCBH ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCCH ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCEH ),
	STATIC_CAST( Device::OutFuncPtr, &VDG64::OutCFH )
};

const Device::InFuncPtr VDG64::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &VDG64::InA2H )
};
