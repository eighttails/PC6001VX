#include "pc6001v.h"
#include "p6el.h"
#include "log.h"
#include "vdg.h"
#include "memory.h"
#include "schedule.h"


// イベントID
#define	EID_VSYNC	(1)			/* 垂直同期 */
#define	EID_DISPS	(2)			/* 表示区間開始 */
#define	EID_DISPE	(3)			/* 表示区間終了 */

#define	VSLINE		(262)		/* 垂直トータルライン   */
#define	VLINES60	(192)		/* 垂直表示ライン(N60)  */
#define	VLINES62	(200)		/* 垂直表示ライン(N60m) */

#define	HSDCLK60	(455)		/* 水平トータル期間(ドットクロック) 60    */
#define	HSDCLK62	(456)		/* 水平トータル期間(ドットクロック) 62,66 */
#define	HCLK6060	(256+40)	/* 水平表示期間(N60)  60    */
#define	HCLK6062	(256+48)	/* 水平表示期間(N60)  62,66 */
#define	HCLOCK62	(320+48)	/* 水平表示期間(N60m)   */



// カラーコード
// --------- 60 ---------
const BYTE VDG60::COL60_AN[] =			// mode 1 -----
				{ 17,18,19,20,16 };

const BYTE VDG60::COL60_SG[] =			// mode 2 -----
				{ 21,22,23,24,25,26,27,28,16 };

const BYTE VDG60::COL60_CG[][8] = {		// mode 3 -----
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

const BYTE VDG60::COL60_RG[][2] = {		// mode 4 -----
				{ 37,38 },
				{ 39,40 }
			};

// --------- mk2 ---------
const BYTE VDG62::COL62_AN[] =			// mode 1-1 -----
				{ 80,73,75,73,73 };

const BYTE VDG62::COL62_SG[] =			// mode 1-2 -----
				{ 75,76,77,74,80,79,78,66,73 };

const BYTE VDG62::COL62_CG[][8] = {		// mode 1-3 -----
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

const BYTE VDG62::COL62_RG[][2] = {		// mode 1-4 -----
				{ 73,75 },
				{ 73,80 }
			};

const BYTE VDG62::COL62_AN2[] =			// mode 2-1,2 -----
				{ 65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80 };

const BYTE VDG62::COL62_CG3[][16] = {	// mode 2-3 -----
				{ 65,69,66,70,67,71,68,72,73,77,74,78,75,79,76,80 },
				{ 75,76,77,74,80,79,78,66,75,76,77,74,80,79,78,66 }
			};

const BYTE VDG62::COL62_CG4[][16] = {	// mode 2-4 -----
				{ 65,69,66,70,67,71,68,72,73,77,74,78,75,79,76,80 },
				{ 75,76,77,74,80,79,78,66,75,76,77,74,80,79,78,66 }
			};
// 色にじみカラーコードテーブル
// 数値はCOL_CG上のインデックス
const BYTE VDG6::NJM6_TBL[][2] = {
				{ 0,0 }, { 0,0 }, { 0,7 }, { 0,7 },
				{ 5,4 }, { 5,4 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 6,2 }, { 6,7 },
				{ 6,4 }, { 6,4 }, { 6,3 }, { 6,3 },
				{ 5,0 }, { 5,0 }, { 5,7 }, { 5,7 },
				{ 1,4 }, { 1,1 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 6,7 }, { 6,7 },
				{ 3,4 }, { 3,4 }, { 3,3 }, { 3,3 },
				{ 0,0 }, { 0,0 }, { 0,7 }, { 0,7 },
				{ 5,4 }, { 5,4 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 2,2 }, { 2,7 },
				{ 6,4 }, { 6,4 }, { 6,3 }, { 6,3 },
				{ 5,0 }, { 5,0 }, { 5,7 }, { 5,7 },
				{ 5,4 }, { 5,1 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 6,7 }, { 6,7 },
				{ 3,4 }, { 3,4 }, { 3,3 }, { 3,3 }
			};
//色にじみは2ドット単位で描画する。
//描画する対象の2ドットとその前後の各2ドット、計6ビットをVRAMから取得し
//その値をインデックスとして、2ドット分の色情報を取得する。
//テーブルに格納された値はさらにCOL_CG上のインデックスとなる。
//黒　0
//白　3
//赤/桃　明←6 2 7→暗
//青/緑　明←4 1 5→暗



////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
VDG6::VDG6( VM6 *vm, const P6ID& id ) : P6DEVICE(vm,id),
		AddrOff(0), VLcnt(VLINES60), OnDisp(false) {}

VDG60::VDG60( VM6 *vm, const ID& id ) : VDG6(vm,id), Device(id)
{
	HSdclk = HSDCLK60;
	Hclk60 = HCLK6060;
	
	// カラーコード設定
	for( int i=0; i<COUNTOF(COL_AN); i++ ) COL_AN[i] = COL60_AN[i];
	for( int i=0; i<COUNTOF(COL_SG); i++ ) COL_SG[i] = COL60_SG[i];
	for( int i=0; i<COUNTOF(COL_CG); i++ )
		for( int j=0; j<COUNTOF(COL_CG[0]); j++ )
			COL_CG[i][j] = COL60_CG[i][j];
	for( int i=0; i<COUNTOF(COL_RG); i++ )
		for( int j=0; j<COUNTOF(COL_RG[0]); j++ )
			COL_RG[i][j] = COL60_RG[i][j];
	
	for( int i=0; i<COUNTOF(NIJIMI_TBL); i++ )
		for( int j=0; j<COUNTOF(NIJIMI_TBL[0]); j++ )
			NIJIMI_TBL[i][j] = NJM6_TBL[i][j];
}

VDG62::VDG62( VM6 *vm, const ID& id ) : VDG6(vm,id), Device(id)
{
	HSdclk = HSDCLK62;
	Hclk60 = HCLK6062;
	
	// カラーコード設定
	for( int i=0; i<COUNTOF(COL_AN); i++ ) COL_AN[i] = COL62_AN[i];
	for( int i=0; i<COUNTOF(COL_SG); i++ ) COL_SG[i] = COL62_SG[i];
	for( int i=0; i<COUNTOF(COL_CG); i++ )
		for( int j=0; j<COUNTOF(COL_CG[0]); j++ )
			COL_CG[i][j] = COL62_CG[i][j];
	for( int i=0; i<COUNTOF(COL_RG); i++ )
		for( int j=0; j<COUNTOF(COL_RG[0]); j++ )
			COL_RG[i][j] = COL62_RG[i][j];
	
	for( int i=0; i<COUNTOF(COL_AN2); i++ ) COL_AN2[i] = COL62_AN2[i];
	for( int i=0; i<COUNTOF(COL_CG3); i++ )
		for( int j=0; j<COUNTOF(COL_CG3[0]); j++ )
			COL_CG3[i][j] = COL62_CG3[i][j];
	for( int i=0; i<COUNTOF(COL_CG4); i++ )
		for( int j=0; j<COUNTOF(COL_CG4[0]); j++ )
			COL_CG4[i][j] = COL62_CG4[i][j];
	
	for( int i=0; i<COUNTOF(NIJIMI_TBL); i++ )
		for( int j=0; j<COUNTOF(NIJIMI_TBL[0]); j++ )
			NIJIMI_TBL[i][j] = NJM6_TBL[i][j];
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
VDG6::~VDG6( void ){}

VDG60::~VDG60( void ){}

VDG62::~VDG62( void ){}


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
	case EID_VSYNC:	// VSYNC
		vm->evsc->OnVSYNC();				// VSYNCを通知する
		VLcnt = N60Win ? VLINES60 : VLINES62;	// 表示ラインカウント初期化
		vm->evsc->Reset( this, EID_DISPS, (double)( N60Win ? Hclk60 : HCLOCK62 ) / (double)HSdclk );
		vm->evsc->Reset( this, EID_DISPE );
		break;
		
	case EID_DISPS:	// 表示区間開始
		if( VLcnt ){
			OnDisp = CrtDisp ? true : false;
			VLcnt--;
		}
		break;
		
	case EID_DISPE:	// 表示区間終了
		OnDisp = false;
		break;
	}
}


////////////////////////////////////////////////////////////////
// バッファアドレス取得
////////////////////////////////////////////////////////////////
BYTE *VDG6::GetBufAddr( void )
{
	return (BYTE *)VSurface::pixels;
}


////////////////////////////////////////////////////////////////
// バッファピッチ(1Lineバイト数)取得
////////////////////////////////////////////////////////////////
int VDG6::GetBufPitch( void )
{
	return VSurface::Pitch();
}


////////////////////////////////////////////////////////////////
// バッファ高さ取得
////////////////////////////////////////////////////////////////
int VDG6::GetBufHeight( void )
{
	return VSurface::Height();
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
BYTE VDG6::GetAttr( void )
{
	WORD addr = ( VAddr * ( N60Win ? 32 : 40 ) + HAddr ) & ( N60Win ? 0x01ff : 0x1fff );
	return vm->mem->ReadMainRam( GerAttrAddr() +  addr );
}


////////////////////////////////////////////////////////////////
// VRAMデータ取得
////////////////////////////////////////////////////////////////
BYTE VDG6::GetVram( void )
{
	WORD addr = VAddr * ( N60Win ? 32 : 40 ) + HAddr;
	return vm->mem->ReadMainRam( GerVramAddr() + addr );
}


////////////////////////////////////////////////////////////////
// Font0(VDG Font)データ取得
////////////////////////////////////////////////////////////////
BYTE VDG60::GetFont0( WORD addr )
{
	return vm->mem->ReadCGrom0( addr );
}


////////////////////////////////////////////////////////////////
// Font1データ取得
////////////////////////////////////////////////////////////////
BYTE VDG6::GetFont1( WORD addr )
{
	return vm->mem->ReadCGrom1( addr );
}


////////////////////////////////////////////////////////////////
// Font2データ取得
////////////////////////////////////////////////////////////////
BYTE VDG62::GetFont2( WORD addr )
{
	return vm->mem->ReadCGrom2( addr );
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool VDG6::Init( void )
{
	// イベント追加
	if( !vm->evsc->Add( this, EID_VSYNC, VSYNC_HZ,        EV_LOOP|EV_HZ ) ) return false;
	if( !vm->evsc->Add( this, EID_DISPS, VSYNC_HZ*VSLINE, EV_LOOP|EV_HZ ) ) return false;
	if( !vm->evsc->Add( this, EID_DISPE, VSYNC_HZ*VSLINE, EV_LOOP|EV_HZ ) ) return false;
	
	// サーフェス作成
	if( VSurface::InitSurface( cMC6847core::GetW(), cMC6847core::GetH(), 8 ) ) return true;
	else																	   return false;
}


////////////////////////////////////////////////////////////////
// 表示区間フラグ取得
////////////////////////////////////////////////////////////////
bool VDG6::IsDisp( void )
{
	return OnDisp;
}


////////////////////////////////////////////////////////////////
// VRAMアドレス取得
//   注! アドレスはMainRamブロック先頭からのオフセットとする
////////////////////////////////////////////////////////////////
WORD VDG60::GerVramAddr( void )
{
	//	[00] V:C200H
	//	[01] V:E200H
	//	[10] V:8200H
	//	[11] V:A200H
	// 実アドレスは +0x8000
	return (((AddrOff+4)&6)<<12) + 0x0200;
}

WORD VDG62::GerVramAddr( void )
{
	if( N60Win ){			// 60
		//	[00] V:C200H
		//	[01] V:E200H
		//	[10] V:8200H
		//	[11] V:A200H
		// mk2以降の場合は実アドレスに一致
		return ( 0x8000 | (((AddrOff+4)&6)<<12) ) + 0x0200;
	}else{					// mk2 or 66
		//	[00] V:8400H or A000H
		//	[01] V:C400H or E000H
		//	[10] V:0400H or 2000H
		//	[11] V:4400H or 6000H
		if( Mk2CharMode )	// キャラクタモード
			return (((AddrOff+4)&6)<<13) + 0x0400;
		else				// グラフィックモード
			return (((AddrOff+4)&6)<<13) + 0x2000;
	}
}


////////////////////////////////////////////////////////////////
// ATTRアドレス取得
//   注! アドレスはMainRamブロック先頭からのオフセットとする
////////////////////////////////////////////////////////////////
WORD VDG60::GerAttrAddr( void )
{
	//	[00] A:C000H
	//	[01] A:E000H
	//	[10] A:8000H
	//	[11] A:A000H
	// 実アドレスは +0x8000
	return (((AddrOff+4)&6)<<12);
}

WORD VDG62::GerAttrAddr( void )
{
	if( N60Win ){			// mk2 or 66
		//	[00] A:C000H
		//	[01] A:E000H
		//	[10] A:8000H
		//	[11] A:A000H
		// mk2以降の場合は実アドレスに一致
		return 0x8000 | (((AddrOff+4)&6)<<12);
	}else{					// 60
		//	[00] A:8000H
		//	[01] A:C000H
		//	[10] A:0000H
		//	[11] A:4000H
		return ((AddrOff+4)&6)<<13;
	}
}


////////////////////////////////////////////////////////////////
// 色の組合せ指定
////////////////////////////////////////////////////////////////
void VDG62::SetCss( BYTE data )
{
	PRINTD( GRP_LOG, "[VDG][62][SetCss] -> %02X : ", data );
	
	Css1 =  data    &1;
	Css2 = (data>>1)&1;
	Css3 = (data>>2)&1;
	
	PRINTD( GRP_LOG, "1:%d 2:%d 3:%d\n", Css1, Css2, Css3 );
}


////////////////////////////////////////////////////////////////
// CRTコントローラモード設定
////////////////////////////////////////////////////////////////
void VDG62::SetCrtControler( BYTE data )
{
	PRINTD( GRP_LOG, "[VDG][62][SetCrtControler] -> %02X\n", data );
	
	N60Win       = data&2 ? true : false;	// ウィンドウサイズ       true:N60        false:N60m
	Mk2CharMode  = data&4 ? true : false;	// mk2 表示モード         true:キャラクタ false:グラフィック
	Mk2GraphMode = data&8 ? true : false;	// mk2 グラフィック解像度 true:160*200    false:320*200
	
	PRINTD( GRP_LOG, " DATA        :%02X\n", data );
	PRINTD( GRP_LOG, " N60Win      :%s\n", N60Win       ? "N60"  : "N60m" );
	PRINTD( GRP_LOG, " Mk2CharMode :%s\n", Mk2CharMode  ? "Char" : "Graph" );
	PRINTD( GRP_LOG, " Mk2GraphMode:%d\n", Mk2GraphMode ? 160    : 320 );
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
inline void VDG60::OutB0H( int, BYTE data ){ AddrOff = data; }

inline void VDG62::OutB0H( int, BYTE data ){ AddrOff = data; }
inline void VDG62::OutC0H( int, BYTE data ){ SetCss( data ); }
inline void VDG62::OutC1H( int, BYTE data ){ SetCrtControler( data ); }
inline BYTE VDG62::InC1H( int ){ return (N60Win ? 2 : 0) | (Mk2CharMode ? 4 : 0) | (Mk2GraphMode ? 8 : 0); }


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
bool VDG60::DokoSave( cIni *Ini )
{
	int i;
	cSche::evinfo e;
	char stren[16];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	
	Ini->PutEntry( "VDG", NULL, "CrtDisp",	"%s",			CrtDisp ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "N60Win",	"%s",			N60Win  ? "Yes" : "No" );
	
	Ini->PutEntry( "VDG", NULL, "VAddr",	"0x%04X",		VAddr );
	Ini->PutEntry( "VDG", NULL, "HAddr",	"0x%04X",		HAddr );
	Ini->PutEntry( "VDG", NULL, "RowCntA",	"%d",			RowCntA );
	Ini->PutEntry( "VDG", NULL, "RowCntG",	"%d",			RowCntG );
	Ini->PutEntry( "VDG", NULL, "AT_AG",	"0x%02X",		AT_AG );
	Ini->PutEntry( "VDG", NULL, "AT_AS",	"0x%02X",		AT_AS );
	Ini->PutEntry( "VDG", NULL, "AT_IE",	"0x%02X",		AT_IE );
	Ini->PutEntry( "VDG", NULL, "AT_GM",	"0x%02X",		AT_GM );
	Ini->PutEntry( "VDG", NULL, "AT_CSS",	"0x%02X",		AT_CSS );
	Ini->PutEntry( "VDG", NULL, "AT_INV",	"0x%02X",		AT_INV );
	
	Ini->PutEntry( "VDG", NULL, "AddrOff",	"0x%02X",		AddrOff );
	Ini->PutEntry( "VDG", NULL, "VLcnt",	"%d",			VLcnt );
	Ini->PutEntry( "VDG", NULL, "OnDisp",	"%s",			OnDisp ? "Yes" : "No" );
	
	
	// イベント
	int eid[] = { EID_VSYNC, EID_DISPS, EID_DISPE, 0 };
	i = 0;
	
	while( eid[i] ){
		e.id = eid[i++];
		if( vm->evsc->GetEvinfo( &e ) ){
			sprintf( stren, "Event%08X", e.id );
			Ini->PutEntry( "VDG", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
		}
	}
	
	return true;
}

bool VDG62::DokoSave( cIni *Ini )
{
	int i;
	cSche::evinfo e;
	char stren[16];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	
	Ini->PutEntry( "VDG", NULL, "CrtDisp",		"%s",		CrtDisp ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "N60Win",		"%s",		N60Win  ? "Yes" : "No" );
	
	Ini->PutEntry( "VDG", NULL, "VAddr",		"0x%04X",	VAddr );
	Ini->PutEntry( "VDG", NULL, "HAddr",		"0x%04X",	HAddr );
	Ini->PutEntry( "VDG", NULL, "RowCntA",		"%d",		RowCntA );
	Ini->PutEntry( "VDG", NULL, "AT_AG",		"0x%02X",	AT_AG );
	Ini->PutEntry( "VDG", NULL, "AT_AS",		"0x%02X",	AT_AS );
	Ini->PutEntry( "VDG", NULL, "AT_IE",		"0x%02X",	AT_IE );
	Ini->PutEntry( "VDG", NULL, "AT_GM",		"0x%02X",	AT_GM );
	Ini->PutEntry( "VDG", NULL, "AT_CSS",		"0x%02X",	AT_CSS );
	Ini->PutEntry( "VDG", NULL, "AT_INV",		"0x%02X",	AT_INV );
	
	Ini->PutEntry( "VDG", NULL, "AddrOff",		"0x%02X",	AddrOff );
	Ini->PutEntry( "VDG", NULL, "VLcnt",		"%d",		VLcnt );
	Ini->PutEntry( "VDG", NULL, "OnDisp",		"%s",		OnDisp ? "Yes" : "No" );
	
	Ini->PutEntry( "VDG", NULL, "Mk2CharMode",	"%s",		Mk2CharMode  ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "Mk2GraphMode",	"%s",		Mk2GraphMode ? "Yes" : "No" );
	Ini->PutEntry( "VDG", NULL, "Css1",			"%d",		Css1 );
	Ini->PutEntry( "VDG", NULL, "Css2",			"%d",		Css2 );
	Ini->PutEntry( "VDG", NULL, "Css3",			"%d",		Css3 );
	
	// イベント
	int eid[] = { EID_VSYNC, EID_DISPS, EID_DISPE, 0 };
	i = 0;
	
	while( eid[i] ){
		e.id = eid[i++];
		if( vm->evsc->GetEvinfo( &e ) ){
			sprintf( stren, "Event%08X", e.id );
			Ini->PutEntry( "VDG", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
		}
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
bool VDG60::DokoLoad( cIni *Ini )
{
	int st,yn,i;
	cSche::evinfo e;
	char stren[16];
	char strrs[64];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	
	Ini->GetTruth( "VDG", "CrtDisp",		&CrtDisp,	CrtDisp );
	Ini->GetTruth( "VDG", "N60Win",			&N60Win,	N60Win );
	
	Ini->GetInt(   "VDG", "VAddr",			&st,		VAddr );	VAddr = st;
	Ini->GetInt(   "VDG", "HAddr",			&st,		HAddr );	HAddr = st;
	Ini->GetInt(   "VDG", "RowCntA",		&RowCntA,	RowCntA );
	Ini->GetInt(   "VDG", "RowCntG",		&RowCntG,	RowCntG );
	Ini->GetInt(   "VDG", "AT_AG",			&st,		AT_AG );	AT_AG = st;
	Ini->GetInt(   "VDG", "AT_AS",			&st,		AT_AS );	AT_AS = st;
	Ini->GetInt(   "VDG", "AT_IE",			&st,		AT_IE );	AT_IE = st;
	Ini->GetInt(   "VDG", "AT_GM",			&st,		AT_GM );	AT_GM = st;
	Ini->GetInt(   "VDG", "AT_CSS",			&st,		AT_CSS );	AT_CSS = st;
	Ini->GetInt(   "VDG", "AT_INV",			&st,		AT_INV );	AT_INV = st;
	
	Ini->GetInt(   "VDG", "AddrOff",		&st,		AddrOff );	AddrOff = st;
	Ini->GetInt(   "VDG", "VLcnt",			&VLcnt,		VLcnt );
	Ini->GetTruth( "VDG", "OnDisp",			&OnDisp,	OnDisp );
	
	// イベント
	int eid[] = { EID_VSYNC, EID_DISPS, EID_DISPE, 0 };
	i = 0;
	
	while( eid[i] ){
		e.id = eid[i++];
		sprintf( stren, "Event%08X", e.id );
		if( Ini->GetString( "VDG", stren, strrs, "" ) ){
			sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
			e.Active = yn ? true : false;
			if( !vm->evsc->SetEvinfo( &e ) ) return false;
		}
	}
	
	return true;
}

bool VDG62::DokoLoad( cIni *Ini )
{
	int st,yn,i;
	cSche::evinfo e;
	char stren[16];
	char strrs[64];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	
	Ini->GetTruth( "VDG", "CrtDisp",		&CrtDisp,		CrtDisp );
	Ini->GetTruth( "VDG", "N60Win",			&N60Win,		N60Win );
	
	Ini->GetInt(   "VDG", "VAddr",			&st,			VAddr );	VAddr = st;
	Ini->GetInt(   "VDG", "HAddr",			&st,			HAddr );	HAddr = st;
	Ini->GetInt(   "VDG", "RowCntA",		&RowCntA,		RowCntA );
	Ini->GetInt(   "VDG", "AT_AG",			&st,			AT_AG );	AT_AG = st;
	Ini->GetInt(   "VDG", "AT_AS",			&st,			AT_AS );	AT_AS = st;
	Ini->GetInt(   "VDG", "AT_IE",			&st,			AT_IE );	AT_IE = st;
	Ini->GetInt(   "VDG", "AT_GM",			&st,			AT_GM );	AT_GM = st;
	Ini->GetInt(   "VDG", "AT_CSS",			&st,			AT_CSS );	AT_CSS = st;
	Ini->GetInt(   "VDG", "AT_INV",			&st,			AT_INV );	AT_INV = st;
	
	Ini->GetInt(   "VDG", "AddrOff",		&st,			AddrOff );	AddrOff = st;
	Ini->GetInt(   "VDG", "VLcnt",			&VLcnt,			VLcnt );
	Ini->GetTruth( "VDG", "OnDisp",			&OnDisp,		OnDisp );
	
	Ini->GetTruth( "VDG", "Mk2CharMode",	&Mk2CharMode,	Mk2CharMode );
	Ini->GetTruth( "VDG", "Mk2GraphMode",	&Mk2GraphMode,	Mk2GraphMode );
	Ini->GetInt(   "VDG", "Css1",			&Css1,			Css1 );
	Ini->GetInt(   "VDG", "Css2",			&Css2,			Css2 );
	Ini->GetInt(   "VDG", "Css3",			&Css3,			Css3 );
	
	// イベント
	int eid[] = { EID_VSYNC, EID_DISPS, EID_DISPE, 0 };
	i = 0;
	
	while( eid[i] ){
		e.id = eid[i++];
		sprintf( stren, "Event%08X", e.id );
		if( Ini->GetString( "VDG", stren, strrs, "" ) ){
			sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
			e.Active = yn ? true : false;
			if( !vm->evsc->SetEvinfo( &e ) ) return false;
		}
	}
	
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

const Device::InFuncPtr VDG60::indef[] = { NULL };

const Device::Descriptor VDG62::descriptor = {
	VDG62::indef, VDG62::outdef
};

const Device::OutFuncPtr VDG62::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &VDG62::OutB0H ),
	STATIC_CAST( Device::OutFuncPtr, &VDG62::OutC0H ),
	STATIC_CAST( Device::OutFuncPtr, &VDG62::OutC1H )
};

const Device::InFuncPtr VDG62::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &VDG62::InC1H )
};
