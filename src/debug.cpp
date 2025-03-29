/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <cctype>
#include <fstream>

#include "pc6001v.h"

#include "breakpoint.h"
#include "common.h"
#include "config.h"
#include "debug.h"
#include "intr.h"
#include "memory.h"
#include "osd.h"
#include "p6el.h"
#include "p6vm.h"
#include "schedule.h"


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define	SCRWINW		(380)
#define	SCRWINH		(250)

#define	REGWINW		(40+32)
#define	REGWINH		( 8+24)
#define	MEMWINW		(72)
#define	MEMWINH		(22)
#define	MONWINW		(60)
#define	MONWINH		(30)

#define	PROMPT		"P6V>"

#define	MAX_CHRS	(256)	// キーバッファ最大値
#define	MAX_HIS		(256)	// ヒストリバッファ最大値


#define	IOPORTX		0		// I/Oポート 原点X
#define	IOPORTY		9		// I/Oポート 原点Y
#define	IOPORTH		20		// I/Oポート 行数

#define	IVECX		37		// 割込みベクタ 原点X
#define	IVECY		0		// 割込みベクタ 原点Y
#define	IVECH		5		// 割込みベクタ 行数


// I/Oポート表示用
#define	IO_PIO			(-1)
#define	IO_PSG			(-2)
#define	IO_OPN			(-3)
#define	IO_SYSLATCH		(-4)
#define	IO_PRINTER		(-5)
#define	IO_SYS_VDG		(-6)
#define	IO_FDD			(-7)
#define	IO_VOICE		(-8)
#define	IO_MEMORY		(-9)
#define	IO_PALETTE		(-10)
#define	IO_SRMEM		(-11)
#define	IO_INTADDR		(-12)
#define	IO_VDG			(-13)
#define	IO_INTCTRL		(-14)

#define	IO_SIO			(-30)
#define	IO_EXVOICE		(-31)
#define	IO_EXKANJI		(-32)
#define	IO_EXSOL		(-33)

#define	IO_SP			(-100)
#define	IO_CR			(-101)
#define	IO_GENERAL		(-102)
#define	IO_SR			(-103)
#define	IO_EXT			(-104)


static const std::map<int,std::string> PGroup = {
	{ IO_PIO,		"PIO"      },
	{ IO_PSG,		"PSG"      },
	{ IO_OPN,		"OPN"      },
	{ IO_SYSLATCH,	"SYSLATCH" },
	{ IO_PRINTER,	"PRINTER"  },
	{ IO_SYS_VDG,	"SYS/VDG"  },
	{ IO_FDD,		"FDD"      },
	{ IO_VOICE,		"VOICE"    },
	{ IO_MEMORY,	"MEMORY"   },
	{ IO_PALETTE,	"PALETTE"  },
	{ IO_SRMEM,		"SRMEMORY" },
	{ IO_INTADDR,	"INT ADDR" },
	{ IO_VDG,		"VDG"      },
	{ IO_INTCTRL,	"INT CTRL" },
	{ IO_SIO,		"SIO"      },
	{ IO_EXVOICE,	"VOICE/FM" },
	{ IO_EXKANJI,	"EXTKANJI" },
	{ IO_EXSOL,		"SOLDIER"  },
};

// PC-6001,PC-6001A
static const std::vector<int> Ports60 = {
	IO_PIO,			0x90, 0x91, 0x92, 0x93,
	IO_PSG,			0xa0, 0xa1, 0xa2, 0xa3,
	IO_SYSLATCH,	0xb0,
	IO_PRINTER,		0xc0,
	IO_CR,
	IO_FDD,			0xd0, 0xd1, 0xd2, 0xd3,
};

// PC-6001mkII
static const std::vector<int> Ports62 = {
	IO_PIO,			0x90, 0x91, 0x92, 0x93,
	IO_PSG,			0xa0, 0xa1, 0xa2, 0xa3,
	IO_SYSLATCH,	0xb0,
	IO_SYS_VDG,		0xc0, 0xc1, 0xc2, 0xc3,
	IO_CR,
	IO_VOICE,		0xe0, 0xe1, 0xe2, 0xe3,
	IO_MEMORY,		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	IO_CR,
	IO_FDD,			0xd0, 0xd1, 0xd2, 0xd3,
};

// PC-6601
static const std::vector<int> Ports66 = {
	IO_PIO,			0x90, 0x91, 0x92, 0x93,
	IO_PSG,			0xa0, 0xa1, 0xa2, 0xa3,
	IO_SYSLATCH,	0xb0, 0xb1, 0xb2, 0xb3,
	IO_SYS_VDG,		0xc0, 0xc1, 0xc2, 0xc3,
	IO_CR,
	IO_VOICE,		0xe0, 0xe1, 0xe2, 0xe3,
	IO_MEMORY,		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	IO_CR,
	IO_FDD,			0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xdd, 0xde,
};

// PC-6001mkIISR
static const std::vector<int> Ports64 = {
	IO_PIO,			0x90, 0x91, 0x92, 0x93,
	IO_OPN,			0xa0, 0xa1, 0xa2, 0xa3,
	IO_SYSLATCH,	0xb0, 0xb2,
	IO_SYS_VDG,		0xc0, 0xc1, 0xc2, 0xc3,
	IO_CR,
	IO_VOICE,		0xe0, 0xe1, 0xe2, 0xe3,
	IO_MEMORY,		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	IO_CR,
	IO_FDD,			0xd0, 0xd1, 0xd2, 0xd3,
	
	IO_SR, IO_SP, IO_SP, IO_SP, IO_SP, IO_SP, IO_SP, IO_SP, IO_SP, IO_SP, IO_SP,
	IO_PALETTE,		0x40, 0x41, 0x42, 0x43,
	IO_CR,
	IO_SRMEM,		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	IO_INTCTRL,		0xfa, 0xfb,
	IO_CR,
	IO_INTADDR,		0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	IO_VDG,			0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
};

// PC-6601SR
static const std::vector<int> Ports68 = {
	IO_PIO,			0x90, 0x91, 0x92, 0x93,
	IO_OPN,			0xa0, 0xa1, 0xa2, 0xa3,
	IO_SYSLATCH,	0xb0, 0xb1, 0xb2, 0xb3,
	IO_SYS_VDG,		0xc0, 0xc1, 0xc2, 0xc3,
	IO_CR,
	IO_VOICE,		0xe0, 0xe1, 0xe2, 0xe3,
	IO_MEMORY,		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	IO_CR,
	IO_FDD,			0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xdd, 0xde,
	
	IO_SR, IO_SP, IO_SP, IO_SP,
	IO_PALETTE,		0x40, 0x41, 0x42, 0x43,
	IO_CR,
	IO_SRMEM,		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	IO_INTCTRL,		0xfa, 0xfb,
	IO_CR,
	IO_INTADDR,		0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	IO_VDG,			0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
};

// OPT,拡張カートリッジ
static const std::vector<int> PortsEx = {
	IO_EXT,
	IO_CR,
	
	// SIO
	IO_SIO,			0x80, 0x81,
	// ボイスシンセサイザ/FM音源カートリッジ
	IO_EXVOICE,		0x70, 0x71, 0x72, 0x73, 0x74,
	// 拡張漢字ROM
	IO_EXKANJI,		0xfc, 0xfd, 0xfe, 0xff,
	
	IO_CR,
	// 戦士のカートリッジ
	IO_EXSOL,		0x06, 0x07, 0x10, 0x11, 0x12, 0x13, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x7f,
};

static const std::map<int,const std::vector<int>&> PortsList = {
	{ 60, Ports60 },
	{ 61, Ports60 },
	{ 62, Ports62 },
	{ 66, Ports66 },
	{ 64, Ports64 },
	{ 68, Ports68 }
};

static const std::vector<std::string> IntName = {
	"",
	"KEY3",
	"SIO",
	"TIMER",
	"CMTR",
	"",
	"",
	"KEY1A",
	"KEY1B",
	"CMTE",
	"KEY2",
	"STICK",
	"TVR",
	"DATE",
	"",
	"",
	"VOICE",
	"VRTC",
	"",
	""
};




//---------------------------------------------------------------------------
//  モニタモードウィンドウ インターフェース(?)クラス
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
iMon::iMon( EL6* el ) : el( el ), x( 0 ), y( 0 )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
iMon::~iMon( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// X座標取得
/////////////////////////////////////////////////////////////////////////////
int iMon::X( void )
{
	return x;
}


/////////////////////////////////////////////////////////////////////////////
// Y座標取得
/////////////////////////////////////////////////////////////////////////////
int iMon::Y( void )
{
	return y;
}


/////////////////////////////////////////////////////////////////////////////
// X座標設定
/////////////////////////////////////////////////////////////////////////////
void iMon::SetX( int xx )
{
	x = xx;
}


/////////////////////////////////////////////////////////////////////////////
// Y座標設定
/////////////////////////////////////////////////////////////////////////////
void iMon::SetY( int yy )
{
	y = yy;
}



//---------------------------------------------------------------------------
//  メモリダンプウィンドウクラス
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cWndMem::cWndMem( EL6* el ) : iMon( el ), Addr( 0 )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cWndMem::~cWndMem( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool cWndMem::Init( void )
{
	// 表示アドレス初期化
	Addr = 0;
	
	return ZCons::Init( MEMWINW, MEMWINH, "MEMORY" );
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウ更新
/////////////////////////////////////////////////////////////////////////////
void cWndMem::Update( void )
{
	WORD addr = Addr;
	const DWORD TextCol[] = { FC_WHITE2, FC_CYAN2, FC_GREEN4, FC_CYAN4, FC_RED3, FC_MAGENTA4, FC_YELLOW4, FC_WHITE4 };
	int i,j;
	
	ZCons::Locate( 0, 0 );
	ZCons::SetColor( FC_YELLOW4, FC_YELLOW2 );
	ZCons::Printf( "MAP " );
	ZCons::SetColor( FC_WHITE4, FC_CYAN2 );
	ZCons::Printf( "  0000   2000   4000   6000   8000   A000   C000   E000          \n" );
	ZCons::SetColor( FC_WHITE4, FC_BLACK );
	
	// Read メモリブロック名表示
	ZCons::Printf( "READ  " );
	for( i = 0; i < 8; i++ ){
		ZCons::SetColor( TextCol[i] );
		ZCons::Printf( "%-7s", el->vm->mem->GetReadMemBlk( i ).c_str() );
	}
	ZCons::Printf( "\nWRITE " );
	
	// Write メモリブロック名表示
	for( i = 0; i < 8; i++ ){
		ZCons::SetColor( TextCol[i] );
		ZCons::Printf( "%-7s", el->vm->mem->GetWriteMemBlk( i ).c_str() );
	}
	ZCons::Printf( "\n" );
	
	// メモリダンプ表示
	ZCons::SetColor( FC_YELLOW4, FC_YELLOW2 );
	ZCons::Printf( "ADDR" );
	ZCons::SetColor( FC_WHITE4, FC_CYAN2 );
	ZCons::Printf( " 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 0123456789ABCDEF" );
	ZCons::SetColor( FC_WHITE4, FC_BLACK );
	
	for( i = 0; i < GetYline() - 4; i++ ){
		ZCons::Printf( "\n" );
		ZCons::SetColor( TextCol[ addr >> 13 ] );
		ZCons::Printf( "%04X", addr );
		ZCons::SetColor( FC_WHITE4, FC_BLACK );
		ZCons::Printf( " " );
		for( j = 0; j < 16; j++){
			ZCons::Printf( "%02X ", el->vm->mem->Read(addr+j) );
		}
		for( j = 0; j < 16; j++){
			ZCons::PutCharH( el->vm->mem->Read(addr+j) );
		}
		addr += 16;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 表示アドレス設定
/////////////////////////////////////////////////////////////////////////////
void cWndMem::SetAddress( WORD addr )
{
	Addr = addr & 0xfff8;
}


/////////////////////////////////////////////////////////////////////////////
// 表示アドレス取得
/////////////////////////////////////////////////////////////////////////////
WORD cWndMem::GetAddress( void )
{
	return Addr;
}



//---------------------------------------------------------------------------
//  レジスタウィンドウクラス
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cWndReg::cWndReg( EL6* el ) : iMon( el )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cWndReg::~cWndReg( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool cWndReg::Init( void )
{
	return ZCons::Init( REGWINW, REGWINH, "REGISTER" );
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウ更新
/////////////////////////////////////////////////////////////////////////////
void cWndReg::Update( void )
{
	const std::string flags = "SZ.H.PNC";
	cZ80::Register reg;
	std::string DisCode;
	int i,j;
	
	// レジスタ値取得
	el->vm->cpum->GetRegister( reg );
	
	// 1ライン逆アセンブル
	el->vm->cpum->Disasm( DisCode, reg.PC.W );
	
	ZCons::SetColor( FC_WHITE4, FC_BLACK );
	ZCons::Locate( 0, 0 );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "AF :" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X ",  reg.AF.W );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "BC :" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X ",  reg.BC.W );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "DE :" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X ",  reg.DE.W );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "HL :" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X\n", reg.HL.W );
	
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "AF':" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X ",  reg.AF1.W );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "BC':" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X ",  reg.BC1.W );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "DE':" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X ",  reg.DE1.W );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "HL':" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X\n", reg.HL1.W );
	
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "IX :" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X ",  reg.IX.W );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "IY :" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X ",  reg.IY.W );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "PC :" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X ",  reg.PC.W );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "SP :" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%04X\n", reg.SP.W );
	
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "FLAG[" );
	ZCons::SetColor( FC_WHITE4 );
	for( j = 0, i = reg.AF.B.l; j < 8; j++, i <<= 1 ){
		if( flags.data()[j] != '.' ){
			ZCons::SetColor( i & 0x80 ? FC_WHITE4 : FC_WHITE2 );
			ZCons::Printf( "%c", flags.data()[j] );
		}
	}
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "] " );
	
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "I:"    );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%02X ", reg.I    );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "IFF:"  );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%d ",   reg.IFF  );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "IM:"   );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%1d ",  reg.IM   );
	ZCons::SetColor( FC_CYAN4 );	ZCons::Printf( "HALT:" );	ZCons::SetColor( FC_WHITE4 );	ZCons::Printf( "%1d\n", reg.Halt );
	
	ZCons::SetColor( FC_WHITE4, FC_GREEN2 );
	ZCons::Locate( 0, 4 );
	ZCons::Printf( "%-35s", DisCode.c_str() );
	ZCons::SetColor( FC_WHITE4, FC_BLACK );
	


	ZCons::Locate( 0, 5 );
	
	// PRINTER STROBE 0/1
	ZCons::Printf( "CRT  :%s\n", el->vm->vdg->GetCrtDisp() ? "DISP" : "KILL" );
	// CGROM ON/OFF
	ZCons::Printf( "TIMER:%s\n", el->vm->intr->GetTimerIntr() ? "ON " : "OFF" );
	ZCons::Printf( "ATTR :%04X VRAM:%04X\n", el->vm->vdg->GerAttrAddr(), el->vm->vdg->GetVramAddr() );
	// RELAY ON/OFF
	
	
	// I/O port
	ZCons::Locate( IOPORTX, IOPORTY );
	ZCons::SetColor( FC_YELLOW4, FC_CYAN2 );
	ZCons::Printf( "%-62s", "[I/O PORT HISTORY] PORT:IN:OUT ??:NO ACCESS" );
	
	// 登録済I/Oポートリストを取得
	std::vector<int> PIn, POut;
	el->vm->iom->GetPortList( PIn, POut );
	
	// 機種別I/Oポートリスト取得
	std::vector<int> Ports;
	try{
		Ports = PortsList.at( el->cfg->GetValue( CV_Model ) );
	}
	catch( std::out_of_range& ){
		return;	// ポートリストが見つからなかったら何もしない
	}
	
	// OPT,拡張カートリッジのポートリストを結合する
	std::copy( PortsEx.begin(), PortsEx.end(), std::back_inserter( Ports ) );
	
	
	int CPortE = FC_CYAN4;	// 文字色 有効ポート番号
	int CPortD = FC_CYAN2;	// 文字色 無効ポート番号
	int xx = 0, yy = 0;
	
	for( auto &p : Ports ){
		switch( p ){
		case IO_GENERAL:	// 一般ポート
			CPortE = FC_CYAN4;
			CPortD = FC_CYAN2;
			continue;
			
		case IO_SR:			// SR専用ポート
			CPortE = FC_MAGENTA4;
			CPortD = FC_MAGENTA2;
			continue;
			
		case IO_EXT:		// 拡張カートリッジ
			CPortE = FC_GREEN4;
			CPortD = FC_GREEN2;
			continue;
			
		case IO_SP:			// 空欄
			if( ++yy < IOPORTH ){
				continue;
			}
			[[fallthrough]];
			
		case IO_CR:			// 改行
			if( yy != 0 ){	// 改行直後なら無視
				xx += 9;
				yy  = 0;
			}
			continue;
		}
		
		ZCons::Locate( xx + IOPORTX, yy + IOPORTY+1 );
		
		if( p < 0 ){	// 負数ならグループ名表示
			std::string str;
			
			try{
				str = PGroup.at( p );
			}
			catch( std::out_of_range& ){
				str = "???";
			}
			ZCons::SetColor( FC_YELLOW4, FC_BLUE4 );
			ZCons::Printf( "%-8s", str.c_str() );
			ZCons::SetColor( FC_WHITE4, FC_BLACK );
			
		}else{			// 正数ならポート情報表示
			bool min  = (std::count( PIn.begin(),  PIn.end(),  p ));
			bool mout = (std::count( POut.begin(), POut.end(), p ));
			
			ZCons::SetColor( min | mout ? CPortE : CPortD );
			ZCons::Printf( "%02X:", p );
			
			if( min ){
				ZCons::SetColor( FC_WHITE4 );
				if( el->vm->iom->PeepIn ( p ) < 0 ){
					ZCons::Printf( "??" );
				}else{
					ZCons::Printf( "%02X", el->vm->iom->PeepIn ( p ) );
				}
			}else{
				ZCons::SetColor( FC_WHITE1 );
				ZCons::Printf( "--" );
			}
			
			ZCons::SetColor( FC_WHITE2 );
			ZCons::Printf( ":" );
			
			if( mout ){
				ZCons::SetColor( FC_WHITE4 );
				if( el->vm->iom->PeepOut( p ) < 0 ){
					ZCons::Printf( "??" );
				}else{
					ZCons::Printf( "%02X", el->vm->iom->PeepOut( p ) );
				}
			}else{
				ZCons::SetColor( FC_WHITE1 );
				ZCons::Printf( "--" );
			}
		}
		
		// 改行?
		if( ++yy < IOPORTH ){
			continue;
		}
		
		// 改行
		xx += 9;
		yy  = 0;
	}
	
	
	// 割込みベクタ
	ZCons::Locate( IVECX, IVECY );
	ZCons::SetColor( FC_YELLOW4, FC_CYAN2 );
	ZCons::Printf( "%-31s", "[INTERRUPT VECTOR]" );
	ZCons::SetColor( FC_WHITE4, FC_BLACK );
	
	int imax = el->cfg->GetValue( CV_Model );
	imax = (imax == 64 || imax == 68) ? 18 : 12;
		
	for( int y = 0, i = 0; y < (imax + 3) / 4; y++ ){
		ZCons::Locate( IVECX, IVECY + y + 1 );
		for( int x = 0; x < 4; x++, i++ ){
			if( i >= imax ){
				break;
			}
			
			DWORD flg = el->vm->intr->GetIntrFlag();
			if( ((flg & IREQ_TIMER)  && i ==  3) ||	// INT3:タイマ割込み
				((flg & IREQ_8049)   && (i == 1 || i == 4 || i == 7 || i == 8 || i == 9 || i == 10 || i == 11)) ||	// INT1:8049割込み
				((flg & IREQ_VOICE)  && i == 16) ||	// INT4:音声合成割込み
				((flg & IREQ_VRTC)   && i == 17) ||	// INT5:VRTC割込み(SRモードの時だけ発生)
				((flg & IREQ_SIO)    && i ==  2) ||	// IRQ6:RS-232C割込み
				((flg & IREQ_JOYSTK) && i ==  8) ||	// INT2:ジョイスティック割込み(7pin)
				((flg & IREQ_EXTINT) && i ==  8) ||	// INT8:外部割込み
				((flg & IREQ_PRINT)  && i ==  8) ){	// INT7:プリンタ割込み
					ZCons::SetColor( FC_RED4, FC_BLACK );
			}else{
				ZCons::SetColor( IntName[ i ].size() ? FC_WHITE4 : FC_WHITE2, FC_BLACK );
			}
			ZCons::Printf( "%02X:%02X%02X ", i * 2, el->vm->mem->Read( (reg.I << 8) + i * 2 + 1 ), el->vm->mem->Read( (reg.I << 8) + i * 2 ) );
		}
	}
	
	
	// SUB CPU
	ZCons::Locate( IVECX, IVECY + (imax + 3) / 4 );
	ZCons::SetColor( FC_YELLOW4, FC_CYAN2 );
	ZCons::Printf( "%-31s", "[SUB CPU]" );
	ZCons::SetColor( FC_WHITE4, FC_BLACK );
	ZCons::Locate( IVECX, IVECY + (imax + 3) / 4 + 1 );
	
	ZCons::Printf( "CPU STATUS : " );
	switch( el->vm->cpus->GetStatus() ){
	case SUB6::SS_IDLE:		ZCons::Printf( "IDLE     " );	break;	// 何もしていない
	case SUB6::SS_KEY1:		ZCons::Printf( "KEY1     " );	break;	// キー割込み1その1
	case SUB6::SS_KEY12:	ZCons::Printf( "KEY1-2   " );	break;	// キー割込み1その2
	case SUB6::SS_KEY2:		ZCons::Printf( "KEY2     " );	break;	// キー割込み2
	case SUB6::SS_KEY3:		ZCons::Printf( "KEY3     " );	break;	// キー割込み3
	case SUB6::SS_CMTR:		ZCons::Printf( "CMT READ " );	break;	// CMT READ割込み
	case SUB6::SS_CMTE:		ZCons::Printf( "CMT ERROR" );	break;	// CMT ERROR割込み
	case SUB6::SS_SIO:		ZCons::Printf( "SIO      " );	break;	// RS232C受信割込み
	case SUB6::SS_JOY:		ZCons::Printf( "STICK    " );	break;	// ゲーム用キー割込み
	case SUB6::SS_TVRR:		ZCons::Printf( "TVR READ " );	break;	// TV予約読込み割込み
	case SUB6::SS_DATE:		ZCons::Printf( "DATE     " );	break;	// DATE割込み
	case SUB6::SS_CMTO:		ZCons::Printf( "CMT WRITE" );	break;	// CMT 1文字出力 データ待ち
	case SUB6::SS_TVRW:		ZCons::Printf( "TVR WRITE" );	break;	// TV予約書込み データ待ち
	default:				ZCons::Printf( "(UNKNOWN)" );			// 何か
	}

}


//---------------------------------------------------------------------------
//  モニタウィンドウクラス
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// 命令の種類判定テーブル
/////////////////////////////////////////////////////////////////////////////
enum MonitorJob{	// ジョブ
	MONITOR_NONE = 0,
	
	MONITOR_HELP,
	
	MONITOR_GO,
	MONITOR_TRACE,
	MONITOR_STEP,
	MONITOR_STEPALL,
	MONITOR_BREAK,
	
	MONITOR_READ,
	MONITOR_WRITE,
	MONITOR_FILL,
	MONITOR_MOVE,
	MONITOR_SEARCH,
	MONITOR_OUT,
	MONITOR_LOADMEM,
	MONITOR_SAVEMEM,
	
	MONITOR_RESET,
	MONITOR_REG,
	MONITOR_DISASM
};

struct MonCmd{
	MonitorJob Step;
	const std::string cmd;
	const std::string HelpMes;
};

static const std::vector<MonCmd> MonitorCmd = {
	{ MONITOR_HELP,		"help",		"ヘルプを表示" },
	{ MONITOR_HELP,		"?",		"    〃" },
	{ MONITOR_GO,		"go",		"実行" },
	{ MONITOR_GO,		"g",		"    〃" },
	{ MONITOR_TRACE,	"trace",	"トレース実行" },
	{ MONITOR_TRACE,	"t",		"    〃" },
	{ MONITOR_STEP,		"step",		"ステップ実行" },
	{ MONITOR_STEP,		"s",		"    〃" },
	{ MONITOR_STEPALL,	"S",		"    〃" },
	{ MONITOR_BREAK,	"break",	"ブレークポイント設定" },
	{ MONITOR_BREAK,	"b",		"    〃" },
	{ MONITOR_READ,		"read",		"メモリを読込む" },
	{ MONITOR_WRITE,	"write",	"メモリに書込む" },
	{ MONITOR_FILL,		"fill",		"メモリを埋める" },
	{ MONITOR_MOVE,		"move",		"メモリを移動" },
	{ MONITOR_SEARCH,	"search",	"メモリを検索" },
	{ MONITOR_OUT,		"out",		"ポート出力" },
	{ MONITOR_LOADMEM,	"loadmem",	"ファイルからメモリに読込む" },
	{ MONITOR_SAVEMEM,	"savemem",	"メモリからファイルに書込む" },
	{ MONITOR_RESET,	"reset",	"PC6001Vをリセット" },
	{ MONITOR_REG,		"reg",		"CPUレジスタを参照/設定" },
	{ MONITOR_DISASM,	"disasm",	"逆アセンブル" }
};


/////////////////////////////////////////////////////////////////////////////
// 引数の種類判定テーブル
/////////////////////////////////////////////////////////////////////////////
enum ArgvType{
	ARGV_END	= 0x00000,
	ARGV_STR	= 0x00001,	// strings
	ARGV_PORT	= 0x00002,	// 0～0xff
	ARGV_ADDR	= 0x00004,	// 0～0xffff
	ARGV_NUM	= 0x00008,	// 0～0x7fffffff
	ARGV_INT	= 0x00010,	// -0x7fffffff～0x7fffffff
	ARGV_SIZE	= 0x00080,	// #1～#0x7fffffff
	ARGV_REG	= 0x00400,	// RegisterName
	ARGV_BREAK	= 0x00800,	// BreakAction
	ARGV_STEP	= 0x04000	// StepCommand
};


enum ArgvName{
	// <reg>
	ARG_AF,		ARG_BC,		ARG_DE,		ARG_HL,
	ARG_IX,		ARG_IY,		ARG_SP,		ARG_PC,
	ARG_AF1,	ARG_BC1,	ARG_DE1,	ARG_HL1,
	ARG_I,		ARG_R,
	ARG_IFF,	ARG_IM,		ARG_HALT,
	
	// <action>
	//ARG_PC,
	ARG_READ,	ARG_WRITE,
	ARG_IN,		ARG_OUT,
	ARG_INTR,
	ARG_CLEAR,
	
	// step <cmd>
	//ARG_ALL
	ARG_CALL,	ARG_JP,		ARG_REP,
	
	// reg all
	ARG_ALL
};


struct MonArgv{
	const std::string Str;
	int Type;
	int Val;
};

static const std::vector<MonArgv> MonitorArgv = {
	// <reg>
	{ "AF",		ARGV_REG,	ARG_AF,		},
	{ "BC",		ARGV_REG,	ARG_BC,		},
	{ "DE",		ARGV_REG,	ARG_DE,		},
	{ "HL",		ARGV_REG,	ARG_HL,		},
	{ "IX",		ARGV_REG,	ARG_IX,		},
	{ "IY",		ARGV_REG,	ARG_IY,		},
	{ "SP",		ARGV_REG,	ARG_SP,		},
	{ "PC",		ARGV_REG,	ARG_PC,		},
	{ "AF'",	ARGV_REG,	ARG_AF1,	},
	{ "BC'",	ARGV_REG,	ARG_BC1,	},
	{ "DE'",	ARGV_REG,	ARG_DE1,	},
	{ "HL'",	ARGV_REG,	ARG_HL1,	},
	{ "I",		ARGV_REG,	ARG_I,		},
	{ "R",		ARGV_REG,	ARG_R,		},
	{ "IFF",	ARGV_REG,	ARG_IFF,	},
	{ "IM",		ARGV_REG,	ARG_IM,		},
	{ "HALT",	ARGV_REG,	ARG_HALT,	},
	
	//<action>
	{ "PC",		ARGV_BREAK,	ARG_PC,		},
	{ "READ",	ARGV_BREAK,	ARG_READ,	},
	{ "WRITE",	ARGV_BREAK,	ARG_WRITE,	},
	{ "IN",		ARGV_BREAK,	ARG_IN,		},
	{ "OUT",	ARGV_BREAK,	ARG_OUT,	},
	{ "INTR",	ARGV_BREAK,	ARG_INTR,	},
	{ "CLEAR",	ARGV_BREAK,	ARG_CLEAR,	},
	
	// step
	{ "CALL",	ARGV_STEP,	ARG_CALL,	},
	{ "JP",		ARGV_STEP,	ARG_JP,		},
	{ "REP",	ARGV_STEP,	ARG_REP,	},
	{ "ALL",	ARGV_STEP,	ARG_ALL,	}
};


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cWndMon::cWndMon( EL6* el ) : iMon( el ), ArgvCounter( 0 )
{
	KeyBuf.clear();
	HisBuf.clear();
	Argv.clear();
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cWndMon::~cWndMon( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool cWndMon::Init( void )
{
	if( ZCons::Init( MONWINW, MONWINH, "" ) ){
		// 最初だけメッセージ表示
		ZCons::Printf( "***********************************************\n" );
		ZCons::Printf( "* PC6001V  - monitor mode -                   *\n" );
		ZCons::Printf( "*  help 又は ? と入力するとヘルプを表示します *\n" );
		ZCons::Printf( "***********************************************\n\n" );
		ZCons::Printf( PROMPT );
		return true;
	}
	
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウ更新
/////////////////////////////////////////////////////////////////////////////
void cWndMon::Update( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// キー入力処理
/////////////////////////////////////////////////////////////////////////////
void cWndMon::KeyIn( int kcode, int ccode )
{
	static int LastKey  = KVC_ENTER;	// 前回のキー
	static int HisLevel = 0;			// ヒストリレベル
	
	switch( kcode ){		// キーコード
	case KVC_ENTER:			// Enter
	case KVC_P_ENTER:		// Enter(テンキー)
		ZCons::Printf( "\n" );
		
		if( !KeyBuf.empty() ){	// キーバッファに文字列あり
			// 末尾のスペースを削除
			while( KeyBuf.substr( KeyBuf.length() - 1, 1) == " " ){
				KeyBuf.pop_back();
			}
			
			auto result = std::find( HisBuf.begin(), HisBuf.end(), KeyBuf );
			
			// ヒストリに含まれるなら一旦削除
			if( result != HisBuf.end() ){
				HisBuf.erase( HisBuf.begin() + std::distance( HisBuf.begin(), result ) );
			}
			
			// ヒストリバッファがいっぱいなら削除
			while( HisBuf.size() >= MAX_HIS ){
				HisBuf.erase( HisBuf.begin() );
			}
			
			// キーバッファをヒストリバッファにコピー
			if( HisBuf.size() < MAX_HIS ){
				HisBuf.emplace_back( KeyBuf );
			}
		}
		
		// ここで引数解析処理
		Exec( GetArg() );
		
		KeyBuf.clear();						// キーバッファクリア
		ZCons::Printf( PROMPT );
		break;
		
	case KVC_UP:			// 上矢印
	case KVC_DOWN:			// 下矢印
		if( !HisBuf.size() ){
			break;
		}
		
		if( LastKey == KVC_UP || LastKey == KVC_DOWN ){
			if( kcode == KVC_UP   && HisLevel < (int)HisBuf.size() ){ HisLevel++; }
			if( kcode == KVC_DOWN && HisLevel > 0 )                 { HisLevel--; }
		}else{
			if( kcode == KVC_UP ){ HisLevel = 1; }
			else                 { HisLevel = 0; }
		}
		
		// 今のコマンドラインを消去
		while( !KeyBuf.empty() ){
			ZCons::Printf( "\b" );
			KeyBuf.pop_back();
		}
		
		// ヒストリバッファからキーバッファにコピーして表示
		if( HisLevel > 0 ){
			KeyBuf = HisBuf[HisBuf.size() - HisLevel];
			ZCons::Printf( KeyBuf );
		}
		break;
		
	case KVC_BACKSPACE:		// BackSpace
		if( !KeyBuf.empty() ){
			ZCons::Printf( "\b" );
			KeyBuf.pop_back();
		}
		break;
		
	default:
		// 有効な文字コードかつバッファがあふれていなければ
		if( ( ccode > 0x1f ) && ( ccode < 0x80 ) ){
			if( KeyBuf.length() < (MAX_CHRS - 1) ){
				KeyBuf.push_back( ccode );
				ZCons::Printf( "%c", ccode );
			}
		}
	}
	
	LastKey = kcode;
}


/////////////////////////////////////////////////////////////////////////////
// ブレークポイント到達
/////////////////////////////////////////////////////////////////////////////
void cWndMon::BreakIn( WORD addr )
{
	ZCons::SetColor( FC_YELLOW4 );
	ZCons::Printf( "\n << Break in %04XH >>", addr );
	switch( el->vm->bp->GetType( el->vm->bp->GetReqNum() ) ){
	case BPoint::BP_READ:	ZCons::Printf( " Read Memory %04XH",      el->vm->bp->GetAddr( el->vm->bp->GetReqNum() ) );	break;
	case BPoint::BP_WRITE:	ZCons::Printf( " Write Memory %04XH",     el->vm->bp->GetAddr( el->vm->bp->GetReqNum() ) );	break;
	case BPoint::BP_IN:		ZCons::Printf( " Read I/O Port %02XH",    el->vm->bp->GetAddr( el->vm->bp->GetReqNum() ) );	break;
	case BPoint::BP_OUT:	ZCons::Printf( " Write I/O Port %02XH",   el->vm->bp->GetAddr( el->vm->bp->GetReqNum() ) );	break;
	case BPoint::BP_INTR:	ZCons::Printf( " Interrupt Vector %02XH", el->vm->bp->GetAddr( el->vm->bp->GetReqNum() ) );	break;
	default:																											break;
	}
	ZCons::SetColor( FC_WHITE4 );
	ZCons::Printf( "\n" PROMPT );
	
	el->vm->bp->Reset();
}


/////////////////////////////////////////////////////////////////////////////
// 引数処理
/////////////////////////////////////////////////////////////////////////////
int cWndMon::GetArg( void )
{
	std::string::size_type ps = 0, pe = 0;
	
	ArgvCounter = 0;	// Shift()用カウンタ初期化
	
	// 引数分解
	Argv.clear();
	while( ps != std::string::npos ){
		pe = KeyBuf.find( ' ', ps );
		
		if( pe == std::string::npos ){
			if( KeyBuf.substr( ps ).length() ){
				Argv.push_back( KeyBuf.substr( ps ) );
			}
			break;
		}else{
			if( pe > ps ){
				Argv.push_back( KeyBuf.substr( ps, pe - ps ) );
			}
			ps = pe + 1;
		}
	}
	
	if( !Argv.size() ){	// 空行?
		return MONITOR_NONE;
	}
	
	// 有効命令探す
	for( auto &m : MonitorCmd ){
		if( !StriCmp( Argv.front(), m.cmd ) ){
			if( Argv.size() == 2 && (Argv[1] == "?") ){	// 引数が ? の場合
				Help( m.Step );
				return MONITOR_NONE;
			}else{										// 通常の命令の場合
				Shift();
				return m.Step;
			}
		}
	}
	
	// 無効命令の場合
	ZCons::SetColor( FC_RED4 );
	ZCons::Printf( "無効なコマンドです : %s\n", Argv.front().c_str() );
	ZCons::SetColor( FC_WHITE4 );
	
	return MONITOR_NONE;
}


/////////////////////////////////////////////////////////////////////////////
// 引数配列シフト
/////////////////////////////////////////////////////////////////////////////
void cWndMon::Shift( void )
{
	bool size = false;
	size_t chk;
	
	// これ以上引数が無い
	if( Argv.size() <= 1 ){
		argv.Type = ARGV_END;
		Argv.clear();
	// まだ引数があるので解析
	}else{
		// 先頭要素(処理済み)を削除
		Argv.erase( Argv.begin() );
		
		argv.Type = ARGV_END;
		argv.Str  = Argv.front();
		
		if( argv.Str.front() == '#' ){
			size = true;
			argv.Str.erase( argv.Str.begin() );
		}
		
		try{
			argv.Val = std::stoi( argv.Str, &chk, 0 );
		}
		catch( std::logic_error& ){	// 変換失敗
			chk = 0;
		}
		
		// 数値の場合
		if( chk == argv.Str.length() ){
			if( size ){		// #で始まる
				if( argv.Val <= 0 ){ argv.Type = ARGV_STR;  }
				else               { argv.Type = ARGV_SIZE; }
			}else{			// 数で始まる
				argv.Type |= ARGV_INT;
				if( argv.Val >= 0      ){ argv.Type |= ARGV_NUM;  }
				if( argv.Val <= 0xff   ){ argv.Type |= ARGV_PORT; }
				if( argv.Val <= 0xffff ){ argv.Type |= ARGV_ADDR; }
			}
		// 文字列の場合
		}else{
			if( size ){		// #で始まる
				argv.Type = ARGV_STR;
			}else{			// 字で始まる
				for( auto &m : MonitorArgv ){
					if( !StriCmp( argv.Str, m.Str ) ){
						argv.Type |= m.Type;
						argv.Val   = m.Val;
					}
				}
				if( argv.Type == ARGV_END ){
					argv.Type = ARGV_STR;
				}
			}
		}
	}
	ArgvCounter++;
}


/////////////////////////////////////////////////////////////////////////////
// 引数エラーメッセージ処理
/////////////////////////////////////////////////////////////////////////////
#define ErrorMes()													\
	do{																\
		ZCons::SetColor( FC_RED4 );									\
		ZCons::Printf( "引数が無効です (arg %d)\n", ArgvCounter );	\
		ZCons::SetColor( FC_WHITE4 );								\
		return;														\
	}while(0)


/////////////////////////////////////////////////////////////////////////////
// コマンド実行
/////////////////////////////////////////////////////////////////////////////
void cWndMon::Exec( int cmd )
{
// 処理された引数の種類をチェック
#define	ArgvIs( x )	(argv.Type & (x))

	switch( cmd ){
	case MONITOR_HELP:
	//-----------------------------------------------------------------------
	// help [<cmd>]
	//	ヘルプを表示する
	//-----------------------------------------------------------------------
	{
		std::string cmds = "";
		
		if( argv.Type != ARGV_END ){				// [cmd]
			cmds = argv.Str;
			Shift();
		}
		if( argv.Type != ARGV_END ) ErrorMes();		// 余計な引数があればエラー
		
		if( cmds.empty() ){	// 引数なし。全ヘルプ表示
			ZCons::Printf( "help\n" );
			for( auto &m : MonitorCmd ){
				ZCons::Printf( "  %-7s %s\n", m.cmd.c_str(), m.HelpMes.c_str() );
			}
			ZCons::Printf( "     注: \"help <コマンド名>\" と入力すると\n         更に詳細なヘルプを表示します。\n" );
		}else{		// 引数のコマンドのヘルプ表示
			size_t i = 0;
			for( auto &m : MonitorCmd ){
				if( !StriCmp( cmds, m.cmd ) ){
					Help( m.Step );
					break;
				}
				i++;
			}
			if( i == MonitorCmd.size() ){ ErrorMes(); }
		}
		
		break;
	}
	case MONITOR_GO:
	//-----------------------------------------------------------------------
	//  go
	//	 実行
	//-----------------------------------------------------------------------
	{
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		OSD_PushEvent( EV_DEBUGMODETOGGLE );
		
		break;
	}
	
	case MONITOR_TRACE:
	//-----------------------------------------------------------------------
	//  trace <step>
	//  trace #<step>
	//  指定したステップ分処理が変わるまで実行
	//-----------------------------------------------------------------------
	{
		int step = 1;
		
		if( argv.Type != ARGV_END ){
			if     ( ArgvIs( ARGV_SIZE ) ){ step = argv.Val; }	// [<step>]
			else if( ArgvIs( ARGV_NUM  ) ){ step = argv.Val; }	// [#<step>]
			else                          { ErrorMes(); }
			Shift();
		}
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		
		while( step-- ){
			int st = 0;
			while( st <= 0 ){	// バスリクエスト期間をスキップ
				st = el->vm->Emu();
				el->vm->EventUpdate( st <= 0 ? 1 : st );	// 1命令実行とイベント更新
			}
		}
		
//		if( CheckBreakPointPC() ){ set_emumode( TRACE_BP ); }
//		else                     { set_emumode( M_TRACE );  }
		
		break;
	}
	
	case MONITOR_STEP:
	//-----------------------------------------------------------------------
	//  step
	//  step [call] [jp] [rep] [all]
	//  1ステップ,実行
	//  CALL,DJNZ,LDIR etc のスキップも可
	//-----------------------------------------------------------------------
	{
		bool call = false, jp = false, rep = false;
		BYTE code;
		WORD addr;
		std::string DisCode;
		cZ80::Register reg;
		int st = 0;
		
		while( argv.Type != ARGV_END ){
			if( ArgvIs( ARGV_STEP ) ){
				if( argv.Val == ARG_CALL ){ call = true; }
				if( argv.Val == ARG_JP   ){ jp   = true; }
				if( argv.Val == ARG_REP  ){ rep  = true; }
				if( argv.Val == ARG_ALL  ){ call = jp = rep = true; }
				Shift();
			}else{
				ErrorMes();
			}
		}
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		el->vm->cpum->GetRegister( reg );
		
		addr = reg.PC.W;
		code = el->vm->mem->Read( addr );
		
		if( call ){
			if( code		  == 0xcd ||	// CALL nn    = 11001101B
			  ( code & 0xc7 ) == 0xc4 ){	// CALL cc,nn = 11ccc100B
				addr += 3;
			}
		}
		
		if( jp ){
			if( code == 0x10 ){			// DJNZ e     = 00010000B
				addr += 2;
			}
		}
		
	    if( rep ){
			if( code == 0xed ){			// LDIR/LDDR/CPIR/CPDR etc
				code = el->vm->mem->Read( addr+1 );
				if( (code & 0xf4) == 0xb0 ){
					addr += 2;
				}
			}
		}
		
		el->vm->cpum->Disasm( DisCode, addr );
		ZCons::Printf( "%s\n", DisCode.c_str() );
		
		while( st <= 0 ){	// バスリクエスト期間をスキップ
			st = el->vm->Emu();
			el->vm->EventUpdate( st <= 0 ? 1 : st );	// 1命令実行とイベント更新
		}
		
		break;
	}	
	case MONITOR_STEPALL:
	//-----------------------------------------------------------------------
	//  S
	//  step all に同じ
	//-----------------------------------------------------------------------
	{
		BYTE code;
		WORD addr;
		std::string DisCode;
		cZ80::Register reg;
		int st = -1;
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		el->vm->cpum->GetRegister( reg );
		
		addr = reg.PC.W;
		code = el->vm->mem->Read( addr );
		
		if( code		  == 0xcd ||	// CALL nn    = 11001101B
		  ( code & 0xc7 ) == 0xc4 ){	// CALL cc,nn = 11ccc100B
			addr += 3;
		}
		
		if( code == 0x10 ){			// DJNZ e     = 00010000B
			addr += 2;
		}
		
		if( code == 0xed ){			// LDIR/LDDR/CPIR/CPDR etc
			code = el->vm->mem->Read( addr+1 );
			if( (code & 0xf4) == 0xb0 ){
				addr += 2;
			}
		}
		
		el->vm->cpum->Disasm( DisCode, addr );
		ZCons::Printf( "%s\n", DisCode.c_str() );
		
		while( st <= 0 ){	// バスリクエスト期間をスキップ
			st = el->vm->Emu();
			el->vm->EventUpdate( st <= 0 ? 1 : st );	// 1命令実行とイベント更新
		}
		
		break;
	}	
	case MONITOR_BREAK:
	//-----------------------------------------------------------------------
	//  break [PC|READ|WRITE|IN|OUT] <addr|port>
	//  break CLEAR [#<No>]
	//  break
	//  ブレークポイントの設定／解除／表示
	//-----------------------------------------------------------------------
	{
		bool show = false;
		int action = ARG_PC;
		WORD addr = 0;
		int number = 0;
		
		if( argv.Type != ARGV_END ){
			// <action>
			if( ArgvIs( ARGV_BREAK ) ){
				action = argv.Val;
				Shift();
			}
			
			// <addr|port>
			switch( action ){
			case ARG_IN:
			case ARG_OUT:
				if( !ArgvIs( ARGV_PORT ) ){ ErrorMes(); }
				addr = argv.Val;
				Shift();
				break;
			case ARG_PC:
			case ARG_READ:
			case ARG_WRITE:
			case ARG_INTR:
				if( !ArgvIs( ARGV_ADDR ) ){ ErrorMes(); }
				addr = argv.Val;
				Shift();
				break;
			}
			
			// [#<No>]
			if( argv.Type != ARGV_END ){
				if( !ArgvIs( ARGV_SIZE ) ){ ErrorMes(); }
				if( argv.Val < 1 || argv.Val > el->vm->bp->GetNum() ){ ErrorMes(); }
				number = argv.Val;
				Shift();
			}
		}else{
			show = true;
		}
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		
		if( show ){
			if( el->vm->bp->GetNum() ){
				for( int i = 1; i <= el->vm->bp->GetNum(); i++ ){
					ZCons::Printf( "    #%02d  ", i );
					addr = el->vm->bp->GetAddr( i );
					switch( el->vm->bp->GetType( i ) ){
					case BPoint::BP_NONE:	ZCons::Printf( "-- なし --\n" );						break;
					case BPoint::BP_PC:		ZCons::Printf( "PC    reach %04XH\n", addr & 0xffff );	break;
					case BPoint::BP_READ:	ZCons::Printf( "READ   from %04XH\n", addr & 0xffff );	break;
					case BPoint::BP_WRITE:	ZCons::Printf( "WRITE    to %04XH\n", addr & 0xffff );	break;
					case BPoint::BP_IN:		ZCons::Printf( "INPUT  from %02XH\n", addr & 0xff   );	break;
					case BPoint::BP_OUT:	ZCons::Printf( "OUTPUT   to %02XH\n", addr & 0xff   );	break;
					case BPoint::BP_INTR:	ZCons::Printf( "INTR vector %02XH\n", addr & 0xff   );	break;
					default:																		break;
					}
				}
			}else
				ZCons::Printf( "ブレークポイントは設定されていません。\n" );
		}else{
			if( action == ARG_CLEAR ){
				el->vm->bp->Delete( number );
				if( number ){
					ZCons::Printf( "ブレークポイント #%02d を消去します。\n", number );
				}else{
					ZCons::Printf( "ブレークポイントを全て消去します。\n" );
				}
			}else{
				std::string s = "";
				
				switch( action ){
				case ARG_PC:	el->vm->bp->Set( BPoint::BP_PC,    addr );	s = "PC : %04XH";		break;
				case ARG_READ:	el->vm->bp->Set( BPoint::BP_READ,  addr );	s = "READ : %04XH";		break;
				case ARG_WRITE:	el->vm->bp->Set( BPoint::BP_WRITE, addr );	s = "WRITE : %04XH";	break;
				case ARG_IN:	el->vm->bp->Set( BPoint::BP_IN,    addr );	s = "IN : %02XH";		break;
				case ARG_OUT:	el->vm->bp->Set( BPoint::BP_OUT,   addr );	s = "OUT : %02XH";		break;
				case ARG_INTR:	el->vm->bp->Set( BPoint::BP_INTR,  addr );	s = "INTR : %02XH";		break;
				default:																			break;
				}
				ZCons::Printf( "ブレークポイント #%02d を設定します。[ ", el->vm->bp->GetNum() );
				ZCons::Printf( s, addr );
				ZCons::Printf( " ]\n" );
			}
		}
		break;
	}
	case MONITOR_READ:
	//-----------------------------------------------------------------------
	//  read <addr>
	//  特定のアドレスをリード
	//-----------------------------------------------------------------------
		break;
		
	case MONITOR_WRITE:
	//-----------------------------------------------------------------------
	//  write <addr> <data>
	//  特定のアドレスにライト
	//-----------------------------------------------------------------------
	{
		if( argv.Type == ARGV_END ){ ErrorMes(); }
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR )){ ErrorMes(); }
		WORD addr = argv.Val;
		Shift();
		
		// <data>
		if( !ArgvIs( ARGV_INT )){ ErrorMes(); }
		BYTE data = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		el->vm->mem->Write( addr, data );
		
		ZCons::Printf( "WRITE memory [ %04XH ] <- %02X  (= %d | %+d | ", addr, (BYTE)data, (BYTE)data, (int8_t)data );
		int i,j;
		for( i = 0, j = 0x80; i < 8; i++, j >>= 1 ){
			ZCons::Printf( "%d", (data & j) ? 1 : 0 );
		}
		ZCons::Printf( "B )\n");
		
		break;
	}
	
	case MONITOR_FILL:
	//-----------------------------------------------------------------------
	//  fill <start-addr> <end-addr> <value>
	//  fill <start-addr> #<size> <value>
	//  メモリを埋める
	//-----------------------------------------------------------------------
	{
		int start, size, value;
		
		if( argv.Type == ARGV_END ){ ErrorMes(); }
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ){ ErrorMes(); }
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ){ size = argv.Val; }
		else if( ArgvIs( ARGV_ADDR ) ){ size = argv.Val - start +1; }
		else                          { ErrorMes(); }
		Shift();
		
		// <data>
		if( !ArgvIs( ARGV_INT )){ ErrorMes(); }
		value = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		for( int i = 0; i < size; i++ ){
			el->vm->mem->Write( start+i, value );
		}
		
		break;
	}
	
	case MONITOR_MOVE:
	//-----------------------------------------------------------------------
	//  move <src-addr> <end-addr> <dist-addr>
	//  move <src-addr> #size      <dist-addr>
	//  メモリ転送
	//-----------------------------------------------------------------------
	{
		int start, size, dist;
		
		if( argv.Type == ARGV_END ){ ErrorMes(); }
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ){ ErrorMes(); }
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ){ size = argv.Val; }
		else if( ArgvIs( ARGV_ADDR ) ){ size = argv.Val - start + 1; }
		else                          { ErrorMes(); }
		Shift();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ){ ErrorMes(); }
		dist = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		// 転送元-転送先が 重ならない
		if( start + size <= dist ){
			for( int i = 0; i < size; i++ ){
				el->vm->mem->Write( dist+i, el->vm->mem->Read( start + i ) );
			}
		// 転送元-転送先が 重なる
		}else{
			for( int i = size - 1; i >= 0; i-- ){
				el->vm->mem->Write( dist+i, el->vm->mem->Read( start + i ) );
			}
		}
		break;
	}
	
	case MONITOR_SEARCH:
	//-----------------------------------------------------------------------
	//  search [<value> [<start-addr> <end-addr>]]
	//  search [<value> [<start-addr> #<size>]]
	//  特定の定数 (1バイト) をサーチ
	//-----------------------------------------------------------------------
		break;
		
	case MONITOR_OUT:
	//-----------------------------------------------------------------------
	//  out <port> <data>
	//  特定のポートに出力
	//-----------------------------------------------------------------------
	{
		if( argv.Type == ARGV_END ){ ErrorMes(); }
		
		// <port>
		if( !ArgvIs( ARGV_PORT )){ ErrorMes(); }
		WORD port = argv.Val;
		Shift();
		
		// <data>
		if( !ArgvIs( ARGV_INT )){ ErrorMes(); }
		BYTE data = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		el->vm->iom->Out( port, data );
		
		ZCons::Printf( "OUT port [ %02XH ] <- %02X  (= %d | %+d | ", port, (BYTE)data, (BYTE)data, (int8_t)data );
		int i,j;
		for( i = 0, j = 0x80; i < 8; i++, j >>= 1 ){
			ZCons::Printf( "%d", (data & j) ? 1 : 0 );
		}
		ZCons::Printf( "B )\n");
		
		break;
	}
		
	case MONITOR_LOADMEM:
	//-----------------------------------------------------------------------
	//  loadmem <filename> <start-addr> <end-addr>
	//  loadmem <filename> <start-addr> #<size>
	// ファイルからメモリにロード
	//-----------------------------------------------------------------------
	{
		std::fstream fs;
		int start,size;
		
		if( argv.Type == ARGV_END ){ ErrorMes(); }
		
		// <filename>
		if( !ArgvIs( ARGV_STR ) ){ ErrorMes(); }
		P6VPATH fname = STR2P6VPATH( argv.Str );
		Shift();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ){ ErrorMes(); }
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ){ size = (argv.Val > 0xffff) ? 0xffff : argv.Val; }
		else if( ArgvIs( ARGV_ADDR ) ){ size = (argv.Val < start ) ? (0x10000 | argv.Val) - start + 1 : argv.Val - start + 1; }
		else                          { ErrorMes(); }
		Shift();
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		if( !OSD_FSopen( fs, fname, std::ios_base::in | std::ios_base::binary ) ){
			ZCons::SetColor( FC_RED4 );
			ZCons::Printf( "Failed : File open error\n" );
			ZCons::SetColor( FC_WHITE4 );
			break;
		}
		
		int addr = start;
		for( int i = 0; i < size; i++ ){
			el->vm->mem->Write( (addr++) & 0xffff, FSGETBYTE( fs ) );
		}
		fs.close();
		
		ZCons::Printf( " Loaded [%s] -> %d bytes\n", P6VPATH2STR( fname ).c_str(), addr - start );
		break;
	}
		
	case MONITOR_SAVEMEM:
	//-----------------------------------------------------------------------
	//  savemem <filename> <start-addr> <end-addr>
	//  savemem <filename> <start-addr> #<size>
	//  メモリをファイルにセーブ
	//-----------------------------------------------------------------------
	{
		std::fstream fs;
		int start,size;
		
		if( argv.Type == ARGV_END ){ ErrorMes(); }
		
		// <filename>
		if( !ArgvIs( ARGV_STR ) ){ ErrorMes(); }
		P6VPATH fname = STR2P6VPATH( argv.Str );
		Shift();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ){ ErrorMes(); }
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ){ size = (argv.Val > 0xffff) ? 0xffff : argv.Val; }
		else if( ArgvIs( ARGV_ADDR ) ){ size = (argv.Val < start ) ? (0x10000 | argv.Val) - start + 1 : argv.Val - start + 1; }
		else                          { ErrorMes(); }
		Shift();
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		if( !OSD_FSopen( fs, fname, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc ) ){
			ZCons::SetColor( FC_RED4 );
			ZCons::Printf( "Failed : File open error\n" );
			ZCons::SetColor( FC_WHITE4 );
			break;
		}
		
		int addr = start;
		for( int i = 0; i < size; i++ ){
			FSPUTBYTE( el->vm->mem->Read( (addr++) & 0xffff ), fs );
		}
		fs.close();
		
		ZCons::Printf( " Saved [%s] -> %d bytes\n", P6VPATH2STR( fname ).c_str(), addr - start );
		break;
	}
		
	case MONITOR_RESET:
	//-----------------------------------------------------------------------
	//  reset
	//	リセット
	//-----------------------------------------------------------------------
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		el->vm->Reset();
		
		break;
		
	case MONITOR_REG:
	//-----------------------------------------------------------------------
	//  reg <name> <value>
	//  レジスタの内容を変更
	//-----------------------------------------------------------------------
	{	int re = -1, val = 0;
		std::string str = "XX";
		cZ80::Register reg;
		
		if( argv.Type == ARGV_END ){ ErrorMes(); }
		
		if( !ArgvIs( ARGV_REG ) ){ ErrorMes(); }	// <name>
		re = argv.Val;
		Shift();
		if( !ArgvIs( ARGV_INT ) ){ ErrorMes(); }	// <value>
		val = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		el->vm->cpum->GetRegister( reg );
		
		switch( re ){
		case ARG_AF:	reg.AF.W  = val;		break;
		case ARG_BC:	reg.BC.W  = val;		break;
		case ARG_DE:	reg.DE.W  = val;		break;
		case ARG_HL:	reg.HL.W  = val;		break;
		case ARG_IX:	reg.IX.W  = val;		break;
		case ARG_IY:	reg.IY.W  = val;		break;
		case ARG_SP:	reg.SP.W  = val;		break;
		case ARG_PC:	reg.PC.W  = val;		break;
		case ARG_AF1:	reg.AF1.W = val;		break;
		case ARG_BC1:	reg.BC1.W = val;		break;
		case ARG_DE1:	reg.DE1.W = val;		break;
		case ARG_HL1:	reg.HL1.W = val;		break;
		case ARG_I:		val &= 0xff; reg.I = val;			break;
		case ARG_R:		val &= 0xff; reg.R = val;			break;
		case ARG_IFF:	if( val )    { val = 1; reg.IFF  = val; }	break;
		case ARG_IM:	if( val > 3 ){ val = 2; reg.IM   = val; }	break;
		case ARG_HALT:	if( val )    { val = 1; reg.Halt = val; }	break;
		}
		
		el->vm->cpum->SetRegister( reg );
		
		for( auto &m : MonitorArgv ){
			if( re == m.Val ){
				str = m.Str;
				break;
			}
		}
		ZCons::Printf( "reg %s <- %04X\n", str.c_str(), val );
		
		break;
	}
		
	case MONITOR_DISASM:
	//-----------------------------------------------------------------------
	//  disasm [[<start-addr>][#<steps>]]
	//  逆アセンブル
	//-----------------------------------------------------------------------
	{	static int SaveDisasmAddr = -1;
		int i, pc;
		int addr = SaveDisasmAddr;
		int step = 16;
		std::string DisCode;
		cZ80::Register reg;
		
		if( argv.Type != ARGV_END ){
			if( ArgvIs( ARGV_ADDR )){		// [<addr>]
				addr = argv.Val;
				Shift();
			}
			if( ArgvIs( ARGV_SIZE )){		// [#<step>]
				step = argv.Val;
				Shift();
			}
		}
		if( argv.Type != ARGV_END ){ ErrorMes(); }
		
		el->vm->cpum->GetRegister( reg );
		if( addr == -1 ){
			addr = reg.PC.W;	// ADDR 未指定時
		}
		
		pc = 0;
		for( i = 0; i < step; i++ ){
			pc += el->vm->cpum->Disasm( DisCode, (WORD)(addr+pc) );
			ZCons::Printf( "%s\n", DisCode.c_str() );
		}
		SaveDisasmAddr = ( addr + pc ) & 0xffff;
		break;
	}
	
	}
}


/////////////////////////////////////////////////////////////////////////////
// ヘルプ表示
/////////////////////////////////////////////////////////////////////////////
void cWndMon::Help( int cmd )
{
	switch( cmd ){
	case MONITOR_HELP:
		ZCons::Printf(
			"  help [<cmd>]\n"
			"    ヘルプを表示します\n"
			"    <cmd> ... ヘルプを表示したいコマンド\n"
			"              [omit]... 全コマンドの簡易ヘルプを表示\n"
		);
		break;
		
	case MONITOR_GO:
		ZCons::Printf(
			"  go\n"
			"    プログラムを実行します\n"
		);
		break;
		
	case MONITOR_TRACE:
		ZCons::Printf(
			"  trace [#<steps>|<steps>]\n"
			"    execute program specityes times\n"
			"    [all omit]        ... trace some steps (previous steps)\n"
			"    #<steps>, <steps> ... step counts of trace  ( you can omit '#' )\n"
		);
		break;
		
	case MONITOR_STEP:
		ZCons::Printf(
			"  step [call][jp][rep]\n"
			"    execute program 1 time\n"
			"    [all omit] ... execute 1 step\n"
			"    call       ... not trace CALL instruction\n"
			"    jp         ... not trace DJNZ instruction\n"
			"    rep        ... not trace LD*R/CP*R/IN*R/OT*R instruction\n"
			"    CAUTION)\n"
			"         call/jp/rep are use break-point #10.\n"
		);
		break;
		
	case MONITOR_STEPALL:
		ZCons::Printf(
			"  S\n"
			"    'step all' と同じ   (stepを参照)\n"
		);
		break;
		
	case MONITOR_BREAK:
		ZCons::Printf(
			"  break [<action>] <addr|port|vec>\n"
			"  break CLEAR [#<No>]\n"
			"  break\n"
			"    ブレークポイントを設定します\n"
			"    [all omit]      ... 全てのブレークポイントを表示\n"
			"    <action>        ... set action of conditon PC|READ|WRITE|IN|OUT|INTR or CLEAR\n"
			"                        PC    ... break if PC reach addr\n"
			"                        READ  ... break if data is read\n"
			"                        WRITE ... break if data is written\n"
			"                        IN    ... break if data is input\n"
			"                        OUT   ... break if data is output\n"
			"                        INTR  ... break if interrupt\n"
			"                        CLEAR ... clear break point\n"
			"                        [omit]... select PC\n"
			"    <addr|port|vec> ... specify address or port\n"
			"                        if <action> is CLEAR, this argument is invalid\n"
			"    #<No>           ... number of break point.\n"
			"                        [omit]... select all\n"
		);
		break;
		
	case MONITOR_READ:
		ZCons::Printf(
			"  read <addr>\n"
			"    メモリを読込みます\n"
			"    <addr> ... 指定アドレス\n"
		);
		break;
		
	case MONITOR_WRITE:
		ZCons::Printf(
			"  write <addr> <data>\n"
			"    メモリに書込みます\n"
			"    <addr> ... 指定アドレス\n"
			"    <data> ... 書込むデータ\n"
		);
		break;
		
	case MONITOR_FILL:
		ZCons::Printf(
			"  fill <start-addr> <end-addr> <value>\n"
			"  fill <start-addr> #<size>    <value>\n"
			"    メモリを指定値で埋めます\n"
			"    <start-addr> ... 開始アドレス\n"
			"    <end-addr>   ... 終了アドレス\n"
			"    #<size>      ... サイズ\n"
			"    <value>      ... 書込む値\n"
		);
		break;
		
	case MONITOR_MOVE:
		ZCons::Printf(
			"  move <src-addr> <end-addr> <dist-addr>\n"
			"  move <src-addr> #<size>    <dist-addr>\n"
			"    メモリを転送します\n"
			"    <src-addr>  ... 転送元開始アドレス\n"
			"    <end-addr>  ... 転送元終了アドレス\n"
			"    #<size>     ... 転送サイズ\n"
			"    <dist-addr> ... 転送先アドレス\n"
		);
		break;
		
	case MONITOR_SEARCH:
		ZCons::Printf(
			"  search [<value> [<start-addr> <end-addr>]]\n"
			"  search [<value> [<start-addr> #<size>]]\n"
			"    メモリを検索します\n"
			"    <value>      ... 検索値\n"
			"    <start-addr> ... 検索開始アドレス\n"
			"    <end-addr>   ... 検索終了アドレス\n"
			"    #<size>      ... 検索サイズ\n"
			"    [omit-all]   ... 前回の値または文字列を検索\n"
		);
		break;
		
	case MONITOR_OUT:
		ZCons::Printf(
			"  out <port> <data>\n"
			"    I/Oポートに出力します\n"
			"    <port> ... I/Oポートアドレス\n"
			"    <data> ... 出力データ\n"
		);
		break;
		
	case MONITOR_LOADMEM:
		ZCons::Printf(
			"  loadmem <filename> <start-addr> <end-addr>\n"
			"  loadmem <filename> <start-addr> #<size>\n"
			"    ファイルからメモリにロードします\n"
			"    <filename>   ... ファイル名\n"
			"    <start-addr> ... ロード開始アドレス\n"
			"    <end-addr>   ... ロード終了アドレス\n"
			"    #<size>      ... ロードサイズ\n"
		);
		break;
		
	case MONITOR_SAVEMEM:
		ZCons::Printf(
			"  savemem <filename> <start-addr> <end-addr>\n"
			"  savemem <filename> <start-addr> #<size>\n"
			"    メモリイメージをファイルにセーブします\n"
			"    <filename>   ... ファイル名\n"
			"    <start-addr> ... セーブ開始アドレス\n"
			"    <end-addr>   ... セーブ終了アドレス\n"
			"    #<size>      ... セーブサイズ\n"
		);
		break;
		
	case MONITOR_RESET:
		ZCons::Printf(
			"  reset\n"
			"    PC6001Vをリセットし，アドレス 0000H から実行します\n"
		);
		break;
		
	case MONITOR_REG:
		ZCons::Printf(
			"  reg <name> <value>\n"
			"    レジスタの値を参照，設定します\n"
			"    <name>     ... specity register name.\n"
			"                   AF|BC|DE|HL|AF'|BC'|DE'|HL'|IX|IY|SP|PC|I|R|IFF|IM\n"
			"    <value>    ... set value\n"
		);
		break;
		
	case MONITOR_DISASM:
		ZCons::Printf(
			"  disasm [[<start-addr>][#<steps>]]\n"
			"    逆アセンブルします\n"
			"    [all omit]   ... PCレジスタアドレスから16ステップ分を逆アセンブル\n"
			"    <start-addr> ... start-addr から逆アセンブルします\n"
			"                     [omit]... PCレジスタアドレス\n"
			"    #<steps>     ... 逆アセンブルするステップ数\n"
			"                     [omit]... 16ステップ\n"
		);
		break;
		
	}
}

#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
