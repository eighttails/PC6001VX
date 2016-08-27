#include <stdlib.h>
#include <string.h>

#include "pc6001v.h"

#include "breakpoint.h"
#include "debug.h"
#include "osd.h"
#include "schedule.h"

#include "p6el.h"
#include "p6vm.h"

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define	SCRWINW		(380)
#define	SCRWINH		(250)

#define	REGWINW		(40+32)
#define	REGWINH		( 8+10)
#define	MEMWINW		(72)
#define	MEMWINH		(31)
#define	MONWINW		(60)
#define	MONWINH		(30)

#define	PROMPT		"P6V>"

//------------------------------------------------------
//  モニタモードウィンドウ インターフェース(?)クラス
//------------------------------------------------------
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
iMon::iMon( VM6 *vm, const ID& id ) : Device(vm,id)
{
	x = y = 0;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
iMon::~iMon( void ){}


////////////////////////////////////////////////////////////////
// X座標取得
////////////////////////////////////////////////////////////////
int iMon::X( void )
{
	return x;
}


////////////////////////////////////////////////////////////////
// Y座標取得
////////////////////////////////////////////////////////////////
int iMon::Y( void )
{
	return y;
}


////////////////////////////////////////////////////////////////
// X座標設定
////////////////////////////////////////////////////////////////
void iMon::SetX( int xx )
{
	x = xx;
}


////////////////////////////////////////////////////////////////
// Y座標設定
////////////////////////////////////////////////////////////////
void iMon::SetY( int yy )
{
	x = yy;
}



//------------------------------------------------------
//  メモリダンプウィンドウクラス
//------------------------------------------------------
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cWndMem::cWndMem( VM6 *vm, const ID& id ) : iMon(vm,id), Addr(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cWndMem::~cWndMem( void ){}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool cWndMem::Init( void )
{
	// 表示アドレス初期化
	Addr = 0;
	
	return ZCons::Init( MEMWINW, MEMWINH, "MEMORY" );
}


////////////////////////////////////////////////////////////////
// ウィンドウ更新
////////////////////////////////////////////////////////////////
void cWndMem::Update( void )
{
	WORD addr = Addr;
	static DWORD TexCol[] = { FC_GRAY, FC_BLUE, FC_GREEN, FC_CYAN, FC_RED, FC_MAGENTA, FC_YELLOW, FC_WHITE };
	int i,j;
	
	ZCons::Locate( 0, 0 );
	ZCons::SetColor( FC_YELLOW, FC_DYELLOW );
	ZCons::Printf( "MAP " );
	ZCons::SetColor( FC_DCYAN, FC_BLACK );
	ZCons::Printf( "  0000   2000   4000   6000   8000   A000   C000   E000\n" );
	ZCons::SetColor( FC_WHITE );
	
	// Read メモリブロック名表示
	ZCons::Printf( "Read  " );
	for( i=0; i<8; i++ ){
		ZCons::SetColor( TexCol[i] );
		ZCons::Printf( "%-7s", vm->MemGetReadMemBlk( i ) );
	}
	ZCons::Printf( "\nWrire " );
	
	// Write メモリブロック名表示
	for( i=0; i<8; i++ ){
		ZCons::SetColor( TexCol[i] );
		ZCons::Printf( "%-7s", vm->MemGetWriteMemBlk( i ) );
	}
	ZCons::Printf( "\n" );
	
	// メモリダンプ表示
	ZCons::SetColor( FC_YELLOW, FC_DYELLOW );
	ZCons::Printf( "ADDR" );
	ZCons::SetColor( FC_DCYAN, FC_BLACK );
	ZCons::Printf( " 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" );
	for( i=0; i<(ZCons::GetYline()-5)/2; i++ ){
		ZCons::Printf( "\n" );
		ZCons::SetColor( TexCol[ addr>>13 ] );
		ZCons::Printf( "%04X ", addr );
		ZCons::SetColor( FC_WHITE );
		for( j=0; j<16; j++)
			ZCons::Printf( "%02X ", vm->MemRead(addr+j) );
		for( j=0; j<16; j++)
			ZCons::PutCharH( vm->MemRead(addr+j) );
		addr += 16;
	}
	
	addr = Addr;
	ZCons::Printf( "\n" );
	ZCons::SetColor( FC_YELLOW, FC_DYELLOW );
	ZCons::Printf( "RAM " );
	ZCons::SetColor( FC_DCYAN, FC_BLACK );
	ZCons::Printf( " 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" );
	for( i=0; i<(ZCons::GetYline()-5)/2; i++ ){
		ZCons::Printf( "\n" );
		ZCons::SetColor( TexCol[ addr>>13 ] );
		ZCons::Printf( "%04X ", addr );
		ZCons::SetColor( FC_WHITE );
		for( j=0; j<16; j++)
			ZCons::Printf( "%02X ", vm->MemReadIntRam(addr+j) );
		for( j=0; j<16; j++)
			ZCons::PutCharH( vm->MemReadIntRam(addr+j) );
		addr += 16;
	}
}


////////////////////////////////////////////////////////////////
// 表示アドレス設定
////////////////////////////////////////////////////////////////
void cWndMem::SetAddress( WORD addr )
{
	Addr = addr & 0xfff8;
}


////////////////////////////////////////////////////////////////
// 表示アドレス取得
////////////////////////////////////////////////////////////////
WORD cWndMem::GetAddress( void )
{
	return Addr;
}



//------------------------------------------------------
//  レジスタウィンドウクラス
//------------------------------------------------------
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cWndReg::cWndReg( VM6 *vm, const ID& id ) : iMon(vm,id) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cWndReg::~cWndReg( void ){}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool cWndReg::Init( void )
{
	return ZCons::Init( REGWINW, REGWINH, "REGISTER" );
}


////////////////////////////////////////////////////////////////
// ウィンドウ更新
////////////////////////////////////////////////////////////////
void cWndReg::Update( void )
{
	const char flags[9] = "SZ.H.PNC";
	cZ80::Register reg;
	char DisCode[128];
	int i,j;
	
	// レジスタ値取得
	vm->CpumGetRegister( &reg );
	
	// 1ライン逆アセンブル
	vm->CpumDisasm( DisCode, reg.PC.W );
	
	ZCons::Locate( 0, 0 ); ZCons::Print( "AF :%04X BC :%04X DE :%04X HL :%04X", reg.AF.W, reg.BC.W, reg.DE.W, reg.HL.W );
	ZCons::Locate( 0, 1 ); ZCons::Print( "AF':%04X BC':%04X DE':%04X HL':%04X", reg.AF1.W, reg.BC1.W, reg.DE1.W, reg.HL1.W );
	ZCons::Locate( 0, 2 ); ZCons::Print( "IX :%04X IY :%04X PC :%04X SP :%04X", reg.IX.W, reg.IY.W, reg.PC.W, reg.SP.W );
	
	ZCons::Locate( 0, 3 ); ZCons::Print( "FLAG:");
	for( j=0,i=reg.AF.B.l; j<8; j++,i<<=1 ){
		ZCons::SetColor( i&0x80 ? FC_WHITE : FC_GRAY );
		ZCons::Print( "%c", flags[j] );
	}
	ZCons::SetColor( FC_WHITE );
	ZCons::Print( " I:%02X IFF:%d IM:%1d HALT:%1d", reg.I, reg.IFF, reg.IM, reg.Halt );
	
	ZCons::SetColor( FC_WHITE, FC_DCYAN );
	ZCons::Locate( 0, 4 ); ZCons::Print( "%-36s", DisCode );
	ZCons::SetColor( FC_WHITE, FC_BLACK );

	ZCons::Locate( 0, 5 );

// PRINTER STROBE 0/1
	ZCons::Printf( "CRT  :%s\n", vm->VdgGetCrtDisp() ? "DISP" : "KILL" );
// CGROM ON/OFF
	ZCons::Printf( "TIMER:%s\n", vm->IntGetTimerIntr() ? "ON" : "OFF" );
	ZCons::Printf( "VRAM:%04X ATTR:%04X\n", vm->VdgGetVramAddr(), vm->VdgGerAttrAddr() );
// RELAY ON/OFF





}


//------------------------------------------------------
//  モニタウィンドウクラス
//------------------------------------------------------

////////////////////////////////////////////////////////////////
// 命令の種類判定テーブル
////////////////////////////////////////////////////////////////
enum MonitorJob	// ジョブ
{
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
	MONITOR_DISASM,
	
	EndofMONITOR
};

const struct{
	MonitorJob Step;
	const char *cmd;
	const char *HelpMes;
}MonitorCmd[]=
{
{ MONITOR_HELP,		"help",		QT_TRANSLATE_NOOP("PC6001VX", "ヘルプを表示") },
{ MONITOR_HELP,		"?",		QT_TRANSLATE_NOOP("PC6001VX", "    〃") },
{ MONITOR_GO,		"go",		QT_TRANSLATE_NOOP("PC6001VX", "実行") },
{ MONITOR_GO,		"g",		QT_TRANSLATE_NOOP("PC6001VX", "    〃") },
{ MONITOR_TRACE,	"trace",	QT_TRANSLATE_NOOP("PC6001VX", "トレース実行") },
{ MONITOR_TRACE,	"t",		QT_TRANSLATE_NOOP("PC6001VX", "    〃") },
{ MONITOR_STEP,		"step",		QT_TRANSLATE_NOOP("PC6001VX", "ステップ実行") },
{ MONITOR_STEP,		"s",		QT_TRANSLATE_NOOP("PC6001VX", "    〃") },
{ MONITOR_STEPALL,	"S",		QT_TRANSLATE_NOOP("PC6001VX", "    〃") },
{ MONITOR_BREAK,	"break",	QT_TRANSLATE_NOOP("PC6001VX", "ブレークポイント設定") },
{ MONITOR_BREAK,	"b",		QT_TRANSLATE_NOOP("PC6001VX", "    〃") },
{ MONITOR_READ,		"read",		QT_TRANSLATE_NOOP("PC6001VX", "メモリを読込む") },
{ MONITOR_WRITE,	"write",	QT_TRANSLATE_NOOP("PC6001VX", "メモリに書込む") },
{ MONITOR_FILL,		"fill",		QT_TRANSLATE_NOOP("PC6001VX", "メモリを埋める") },
{ MONITOR_MOVE,		"move",		QT_TRANSLATE_NOOP("PC6001VX", "メモリを移動") },
{ MONITOR_SEARCH,	"search",	QT_TRANSLATE_NOOP("PC6001VX", "メモリを検索") },
{ MONITOR_OUT,		"out",		QT_TRANSLATE_NOOP("PC6001VX", "ポート出力") },
{ MONITOR_LOADMEM,	"loadmem",	QT_TRANSLATE_NOOP("PC6001VX", "ファイルからメモリに読込む") },
{ MONITOR_SAVEMEM,	"savemem",	QT_TRANSLATE_NOOP("PC6001VX", "メモリからファイルに書込む") },
{ MONITOR_RESET,	"reset",	QT_TRANSLATE_NOOP("PC6001VX", "PC6001Vをリセット") },
{ MONITOR_REG,		"reg",		QT_TRANSLATE_NOOP("PC6001VX", "CPUレジスタを参照/設定") },
{ MONITOR_DISASM,	"disasm",	QT_TRANSLATE_NOOP("PC6001VX", "逆アセンブル") },

};


////////////////////////////////////////////////////////////////
// 引数の種類判定テーブル
////////////////////////////////////////////////////////////////
enum ArgvType{
	ARGV_END	= 0x00000,
	ARGV_STR	= 0x00001,	// strings
	ARGV_PORT	= 0x00002,	// 0〜0xff
	ARGV_ADDR	= 0x00004,	// 0〜0xffff
	ARGV_NUM	= 0x00008,	// 0〜0x7fffffff
	ARGV_INT	= 0x00010,	// -0x7fffffff〜0x7fffffff
	ARGV_SIZE	= 0x00080,	// #1〜#0x7fffffff
	ARGV_REG	= 0x00400,	// RegisterName
	ARGV_BREAK	= 0x00800,	// BreakAction
	ARGV_STEP	= 0x04000,	// StepCommand
	
	EndofArgvType
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
	ARG_READ,	ARG_WRITE,	ARG_IN,
	ARG_OUT,	ARG_CLEAR,
	
	// step <cmd>
	//ARG_ALL
	ARG_CALL,	ARG_JP,		ARG_REP,
	
	// reg all
	ARG_ALL,
	
	EndofArgName
};


const struct{
	const char *StrL;
	const char *StrU;
	int Type;
	int Val;
}MonitorArgv[]=
{
	// <reg>
{ "af",		"AF",		ARGV_REG,	ARG_AF,		},
{ "bc",		"BC",		ARGV_REG,	ARG_BC,		},
{ "de",		"DE",		ARGV_REG,	ARG_DE,		},
{ "hl",		"HL",		ARGV_REG,	ARG_HL,		},
{ "ix",		"IX",		ARGV_REG,	ARG_IX,		},
{ "iy",		"IY",		ARGV_REG,	ARG_IY,		},
{ "sp",		"SP",		ARGV_REG,	ARG_SP,		},
{ "pc",		"PC",		ARGV_REG,	ARG_PC,		},
{ "af'",	"AF'",		ARGV_REG,	ARG_AF1,	},
{ "bc'",	"BC'",		ARGV_REG,	ARG_BC1,	},
{ "de'",	"DE'",		ARGV_REG,	ARG_DE1,	},
{ "hl'",	"HL'",		ARGV_REG,	ARG_HL1,	},
{ "i",		"I",		ARGV_REG,	ARG_I,		},
{ "r",		"R",		ARGV_REG,	ARG_R,		},
{ "iff",	"IFF",		ARGV_REG,	ARG_IFF,	},
{ "im",		"IM",		ARGV_REG,	ARG_IM,		},
{ "halt",	"HALT",		ARGV_REG,	ARG_HALT,	},

//<action>
{ "pc",		"PC",		ARGV_BREAK,	ARG_PC,		},
{ "read",	"READ",		ARGV_BREAK,	ARG_READ,	},
{ "write",	"WRITE",	ARGV_BREAK,	ARG_WRITE,	},
{ "in",		"IN",		ARGV_BREAK,	ARG_IN,		},
{ "out",	"OUT",		ARGV_BREAK,	ARG_OUT,	},
{ "clear",	"CLEAR",	ARGV_BREAK,	ARG_CLEAR,	},

// step
{ "call",	"CALL",		ARGV_STEP,	ARG_CALL,	},
{ "jp",		"JP",		ARGV_STEP,	ARG_JP,		},
{ "rep",	"REP",		ARGV_STEP,	ARG_REP,	},
{ "all",	"ALL",		ARGV_STEP,	ARG_ALL,	},
};


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cWndMon::cWndMon( VM6 *vm, const ID& id ) : iMon(vm,id), Argc(0), ArgvCounter(0)
{
	INITARRAY( KeyBuf, '\0' );
	for( int i=0; i<MAX_HIS; i++ ) ZeroMemory( HisBuf[i], MAX_CHRS );
	INITARRAY( Argv, NULL );
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cWndMon::~cWndMon( void ){}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool cWndMon::Init( void )
{
	if( ZCons::Init( MONWINW, MONWINH, "" ) ){
		// 最初だけメッセージ表示
		ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX", "***********************************************\n") );
		ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX", "* PC6001V  - monitor mode -                   *\n") );
		ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX", "*  help 又は ? と入力するとヘルプを表示します *\n") );
		ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX", "***********************************************\n\n") );
		ZCons::Printf( PROMPT );
		return true;
	}
	
	return false;
}


////////////////////////////////////////////////////////////////
// ウィンドウ更新
////////////////////////////////////////////////////////////////
void cWndMon::Update( void ){}


////////////////////////////////////////////////////////////////
// キー入力処理
////////////////////////////////////////////////////////////////
void cWndMon::KeyIn( int kcode, bool shift, int ccode )
{
	static int LastKey  = KVC_ENTER;	// 前回のキー
	static int HisLevel = 1;			// ヒストリレベル
	
	switch( kcode ){		// キーコード
	case KVC_F6:			// モニタモード変更
		vm->el->ToggleMonitor();
		break;
		
	case KVC_ENTER:			// Enter
	case KVC_P_ENTER:		// Enter(テンキー)
		ZCons::Printf( "\n" );
		
		if( strlen( KeyBuf ) > 0 ){						// キーバッファが有効で
			if( stricmp( KeyBuf, HisBuf[1] ) != 0 ){	// 直前のヒストリと異なるなら
				for( int i=MAX_HIS-1; i>1; i-- ) strcpy( HisBuf[i], HisBuf[i-1] );
				strcpy( HisBuf[1], KeyBuf );			// キーバッファをヒストリバッファにコピー
			}
		}
		
		// ここで引数解析処理
		Exec( GetArg() );
		
		*KeyBuf = '\0';						// キーバッファクリア
		ZCons::Printf( PROMPT );
		break;
		
	case KVC_UP:			// 上矢印
	case KVC_DOWN:			// 下矢印
		if( LastKey == KVC_UP || LastKey == KVC_DOWN ){
			if( kcode == KVC_UP   && HisLevel < MAX_HIS-1 && strlen( HisBuf[HisLevel+1] ) > 0 ) HisLevel++;
			if( kcode == KVC_DOWN && HisLevel > 0 )                                             HisLevel--;
		}else{
			if( kcode == KVC_UP ) HisLevel = 1;
			else                  HisLevel = 0;
		}
		
		// 今のコマンドラインを消去
		while( strlen(KeyBuf) > 0 ){
			ZCons::LocateR( -1, 0 );
			ZCons::PutCharH( ' ' );
			ZCons::LocateR( -1, 0 );
			KeyBuf[strlen(KeyBuf)-1] = (BYTE)'\0';
		}
		// ヒストリバッファからキーバッファにコピーして表示
		strcpy( KeyBuf, HisBuf[HisLevel] );
		ZCons::Printf( KeyBuf );
		break;
		
	case KVC_BACKSPACE:		// BackSpace
		if( strlen(KeyBuf) > 0 ){
			ZCons::LocateR( -1, 0 );
			ZCons::PutCharH( ' ' );
			ZCons::LocateR( -1, 0 );
			KeyBuf[strlen(KeyBuf)-1] = (BYTE)'\0';
		}
		break;
		
	// メモリウィンドウ
	case KVC_PAGEDOWN:		// PageDown
		vm->el->memw->SetAddress( vm->el->memw->GetAddress() + ( shift ? 2048 : 16 ) );
		break;
		
	case KVC_PAGEUP:		// PageUp
		vm->el->memw->SetAddress( vm->el->memw->GetAddress() - ( shift ? 2048 : 16 ) );
		break;
		
	default:
		// 有効な文字コードかつバッファがあふれていなければ
		if( ( ccode > 0x1f ) && ( ccode < 0x80 ) ){
			if( strlen(KeyBuf) < (MAX_CHRS-1) ){
				char *p = &KeyBuf[strlen(KeyBuf)];
				*p++ = (BYTE)ccode;
				*p   = (BYTE)'\0';
//				ZCons::PutCharH( ccode );
				ZCons::Printf( "%c", ccode );
			}
		}
	}
	
	LastKey = kcode;
	
}


////////////////////////////////////////////////////////////////
// ブレークポイント到達
////////////////////////////////////////////////////////////////
void cWndMon::BreakIn( WORD addr )
{
	ZCons::SetColor( FC_YELLOW );
	ZCons::Printf( "\n << Break in %04XH >>", addr );
	switch( vm->BpGetType( vm->BpGetReqBPNum() ) ){
	case BPoint::BP_READ:	ZCons::Printf( " Read Memory %04XH",    vm->BpGetAddr( vm->BpGetReqBPNum() ) );	break;
	case BPoint::BP_WRITE:	ZCons::Printf( " Write Memory %04XH",   vm->BpGetAddr( vm->BpGetReqBPNum() ) );	break;
	case BPoint::BP_IN:		ZCons::Printf( " Read I/O Port %02XH",  vm->BpGetAddr( vm->BpGetReqBPNum() ) );	break;
	case BPoint::BP_OUT:	ZCons::Printf( " Write I/O Port %02XH", vm->BpGetAddr( vm->BpGetReqBPNum() ) );	break;
	default:				break;
	}
	ZCons::SetColor( FC_WHITE );
	ZCons::Printf( "\n" PROMPT );
}


////////////////////////////////////////////////////////////////
// 引数処理
////////////////////////////////////////////////////////////////
int cWndMon::GetArg( void )
{
	char *p = KeyBuf;
	
	ArgvCounter = 1;	// Shift()用カウンタ初期化
	Argc = 0;			// 引数の個数初期化
	
	// 引数分解
	while( (*p!='\0') && (Argc<=MAX_ARGS) ){
		if( *p==' ' )
			p++;
		else{
			Argv[Argc++] = p;
			while( (*p!='\0') && (*p!=' ') )
				p++;
			if( *p==' ' ) *p++ = '\0';
		}
	}
	
	int JobNo;
	if( !Argc )	// 空行?
		JobNo = MONITOR_NONE;
	else{		// 有効命令?
		int i;
		for( i=0; i < COUNTOF(MonitorCmd); i++ )
			if( !strcmp( Argv[0], MonitorCmd[i].cmd ) ) break;
		
		if( i == COUNTOF( MonitorCmd ) ){	// 無効命令の場合
			ZCons::SetColor( FC_RED );
			ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX", "無効なコマンドです : %s\n"), Argv[0] );
			ZCons::SetColor( FC_WHITE );
			JobNo = MONITOR_NONE;
		}else{								// 引数が ? の場合
			if( Argc==2 && !strcmp( Argv[1], "?" ) ){
				Help( MonitorCmd[i].Step );
				JobNo = MONITOR_NONE;
			}else{							// 通常の命令の場合
				JobNo = MonitorCmd[i].Step;
				Shift();
			}
		}
	}
	
	return JobNo;
}


////////////////////////////////////////////////////////////////
// 引数配列シフト
////////////////////////////////////////////////////////////////
void cWndMon::Shift( void )
{
	bool size = false;
	char *p, *chk;
	
	// これ以上引数が無い
	if( ArgvCounter > MAX_ARGS || ArgvCounter >= Argc )
		argv.Type = ARGV_END;
	// まだ引数があるので解析
	else{
		p = Argv[ ArgvCounter ];
		if( *p == '#' ){ size = true; p++; }
		
		argv.Type = ARGV_END;
		argv.Val  = strtol( p, &chk, 0 );
		argv.Str  = Argv[ ArgvCounter ];
		
		// 数値の場合
		if( p!=chk && *chk=='\0' ){
			if( size ){		// #で始まる
				if( argv.Val <= 0 ) argv.Type = ARGV_STR;
				else                argv.Type = ARGV_SIZE;
			}else{			// 数で始まる
				argv.Type |= ARGV_INT;
				if( argv.Val >= 0 )      argv.Type |= ARGV_NUM;
				if( argv.Val <= 0xff )   argv.Type |= ARGV_PORT;
				if( argv.Val <= 0xffff ) argv.Type |= ARGV_ADDR;
			}
			// 文字列の場合
		}else{
			if( size ){		// #で始まる
				argv.Type = ARGV_STR;
			}else{			// 字で始まる
				for( int i=0; i<COUNTOF( MonitorArgv ); i++ ){
					if( !strcmp( p, MonitorArgv[i].StrL ) || !strcmp( p, MonitorArgv[i].StrU ) ){
						argv.Type |= MonitorArgv[i].Type;
						argv.Val   = MonitorArgv[i].Val;
					}
				}
				if( argv.Type == ARGV_END ) argv.Type = ARGV_STR;
			}
		}
		ArgvCounter++;
	}
}


////////////////////////////////////////////////////////////////
// 引数エラーメッセージ処理
////////////////////////////////////////////////////////////////
#define ErrorMes()													\
	do{																\
	ZCons::SetColor( FC_RED );									\
	ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX", "引数が無効です (arg %d)\n"), ArgvCounter );	\
	ZCons::SetColor( FC_WHITE );								\
	return;														\
	}while(0)


////////////////////////////////////////////////////////////////
// コマンド実行
////////////////////////////////////////////////////////////////
void cWndMon::Exec( int cmd )
{
	// 処理された引数の種類をチェック
#define	ArgvIs( x )	(argv.Type & (x))

	switch( cmd ){
	case MONITOR_HELP:
		//--------------------------------------------------------------
		// help [<cmd>]
		//	ヘルプを表示する
		//--------------------------------------------------------------
	{
		int i;
		char *cmd = NULL;
		
		if( argv.Type != ARGV_END ){				// [cmd]
			cmd = argv.Str;
			Shift();
		}
		if( argv.Type != ARGV_END ) ErrorMes();		// 余計な引数があればエラー
		
		if( !cmd ){	// 引数なし。全ヘルプ表示
			ZCons::Printf( "help\n" );
			for( i=0; i < COUNTOF(MonitorCmd); i++ )
				ZCons::Printf( "  %-7s %s\n", TRANS(MonitorCmd[i].cmd), TRANS(MonitorCmd[i].HelpMes) );
			ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX","     注: \"help <コマンド名>\" と入力すると 更に詳細なヘルプを表示します。\n") );
		}else{		// 引数のコマンドのヘルプ表示
			for( i=0; i < COUNTOF(MonitorCmd); i++ )
				if( !strcmp( cmd, MonitorCmd[i].cmd ) ) break;
			if( i==COUNTOF(MonitorCmd) ) ErrorMes();
			Help( MonitorCmd[i].Step );
		}
		
		break;
	}
	case MONITOR_GO:
		//--------------------------------------------------------------
		//  go
		//	 実行
		//--------------------------------------------------------------
	{
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->el->ToggleMonitor();
		
		break;
	}

	case MONITOR_TRACE:
		//--------------------------------------------------------------
		//  trace <step>
		//  trace #<step>
		//  指定したステップ分処理が変わるまで実行
		//--------------------------------------------------------------
	{
		int step = 1;
		
		if( argv.Type != ARGV_END ){
			if     ( ArgvIs( ARGV_SIZE ) ) step = argv.Val;	// [<step>]
			else if( ArgvIs( ARGV_NUM )  ) step = argv.Val;	// [#<step>]
			else                            ErrorMes();
			Shift();
		}
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		
		while( step-- ){
			int st = 0;
			while( st <= 0 ){	// バスリクエスト期間をスキップ
				st = vm->Emu();
				vm->EventUpdate( st <= 0 ? 1 : st );	// 1命令実行とイベント更新
			}
		}
		
		//		if( CheckBreakPointPC() ) set_emumode( TRACE_BP );
		//		else                      set_emumode( M_TRACE );
		
		break;
	}

	case MONITOR_STEP:
		//--------------------------------------------------------------
		//  step
		//  step [call] [jp] [rep] [all]
		//  1ステップ,実行
		//  CALL,DJNZ,LDIR etc のスキップも可
		//--------------------------------------------------------------
	{
		bool call = false, jp = false, rep = false;
		BYTE code;
		WORD addr;
		char DisCode[128];
		cZ80::Register reg;
		int st = 0;
		
		while( argv.Type != ARGV_END ){
			if( ArgvIs( ARGV_STEP ) ){
				if( argv.Val == ARG_CALL )	call = true;
				if( argv.Val == ARG_JP )	jp   = true;
				if( argv.Val == ARG_REP )	rep  = true;
				if( argv.Val == ARG_ALL )	call = jp = rep = true;
				Shift();
			}else
				ErrorMes();
		}
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->CpumGetRegister( &reg );
		
		addr = reg.PC.W;
		code = vm->MemRead( addr );
		
		if( call ){
			if( code		== 0xcd ||	// CALL nn    = 11001101B
					( code&0xc7 ) == 0xc4 ){	// CALL cc,nn = 11ccc100B
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
				code = vm->MemRead( addr+1 );
				if( (code&0xf4) == 0xb0 ){
					addr += 2;
				}
			}
		}
		
		vm->CpumDisasm( DisCode, addr );
		ZCons::Printf( "%s\n", DisCode );
		
		while( st <= 0 ){	// バスリクエスト期間をスキップ
			st = vm->Emu();
			vm->EventUpdate( st <= 0 ? 1 : st );	// 1命令実行とイベント更新
		}
		
		break;
	}
	case MONITOR_STEPALL:
		//--------------------------------------------------------------
		//  S
		//  step all に同じ
		//--------------------------------------------------------------
	{
		BYTE code;
		WORD addr;
		char DisCode[128];
		cZ80::Register reg;
		int st = -1;
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->CpumGetRegister( &reg );
		
		addr = reg.PC.W;
		code = vm->MemRead( addr );
		
		if( code		== 0xcd ||	// CALL nn    = 11001101B
				( code&0xc7 ) == 0xc4 ){	// CALL cc,nn = 11ccc100B
			addr += 3;
		}
		
		if( code == 0x10 ){			// DJNZ e     = 00010000B
			addr += 2;
		}
		
		if( code == 0xed ){			// LDIR/LDDR/CPIR/CPDR etc
			code = vm->MemRead( addr+1 );
			if( (code&0xf4) == 0xb0 ){
				addr += 2;
			}
		}
		
		vm->CpumDisasm( DisCode, addr );
		ZCons::Printf( "%s\n", DisCode );
		
		while( st <= 0 ){	// バスリクエスト期間をスキップ
			st = vm->Emu();
			vm->EventUpdate( st <= 0 ? 1 : st );	// 1命令実行とイベント更新
		}
		
		break;
	}
	case MONITOR_BREAK:
		//--------------------------------------------------------------
		//  break [PC|READ|WRITE|IN|OUT] <addr|port> [#<No>]
		//  break CLEAR [#<No>]
		//  break
		//  ブレークポイントの設定／解除／表示
		//--------------------------------------------------------------
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
				if( !ArgvIs( ARGV_PORT ) ) ErrorMes();
				addr = argv.Val;
				Shift();
				break;
			case ARG_PC:
			case ARG_READ:
			case ARG_WRITE:
				if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
				addr = argv.Val;
				Shift();
				break;
			}
			
			// [#<No>]
			if( argv.Type != ARGV_END ){
				if( !ArgvIs( ARGV_SIZE ) ) ErrorMes();
				if( argv.Val < 1 || argv.Val > NR_BP ) ErrorMes();
				number = argv.Val - 1;
				Shift();
			}
		}else{
			show = true;
		}
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		
		if( show ){
			for( int i=0; i<NR_BP; i++ ){
				ZCons::Printf( "    #%02d  ", i+1 );
				addr = vm->BpGetAddr( i );
				switch( vm->BpGetType( i ) ){
				case BPoint::BP_NONE:
					ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX", "-- なし --\n") );
					break;
				case BPoint::BP_PC:
					ZCons::Printf( "PC   reach %04XH\n", addr&0xffff );
					break;
				case BPoint::BP_READ:
					ZCons::Printf( "READ  from %04XH\n", addr&0xffff );
					break;
				case BPoint::BP_WRITE:
					ZCons::Printf( "WRITE   to %04XH\n", addr&0xffff );
					break;
				case BPoint::BP_IN:
					ZCons::Printf( "INPUT from %02XH\n", addr&0xff );
					break;
				case BPoint::BP_OUT:
					ZCons::Printf( "OUTPUT  to %04XH\n", addr&0xff) ;
					break;
				default:
					break;
				}
			}
		}else{
			if( action == ARG_CLEAR ){
				vm->BpSetType( number, BPoint::BP_NONE );
				ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX", "ブレークポイント #%02d を消去します。\n"), number+1 );
			}else{
				const char *s = NULL;
				
				switch( action ){
				case ARG_PC:
					vm->BpSetType( number, BPoint::BP_PC );
					s = "PC : %04XH";
					break;
				case ARG_READ:
					vm->BpSetType( number, BPoint::BP_READ );
					s = "READ : %04XH";
					break;
				case ARG_WRITE:
					vm->BpSetType( number, BPoint::BP_WRITE );
					s = "WRITE : %04XH";
					break;
				case ARG_IN:
					vm->BpSetType( number, BPoint::BP_IN );
					s = "IN : %02XH";
					break;
				case ARG_OUT:
					vm->BpSetType( number, BPoint::BP_OUT );
					s = "OUT : %02XH";
					break;
				}
				vm->BpSetAddr( number, addr );
				ZCons::Printf( QT_TRANSLATE_NOOP("PC6001VX", "ブレークポイント #%02d を設定します。[ "),number+1 );
				ZCons::Printf( s, addr );
				ZCons::Printf( " ]\n" );
			}
		}
		break;
	}
	case MONITOR_READ:
	//--------------------------------------------------------------
	//  read <addr>
	//  特定のアドレスをリード
	//--------------------------------------------------------------
		break;
		
	case MONITOR_WRITE:
	//--------------------------------------------------------------
	//  write <addr> <data>
	//  特定のアドレスにライト
	//--------------------------------------------------------------
	{
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR )) ErrorMes();
		WORD addr = argv.Val;
		Shift();
		
		// <data>
		if( !ArgvIs( ARGV_INT )) ErrorMes();
		BYTE data = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->MemWrite( addr, data );
		
		ZCons::Printf( "WRITE memory [ %04XH ] <- %02X  (= %d | %+d | ", addr, (BYTE)data, (BYTE)data, (int8_t)data );
		int i,j;
		for( i=0, j=0x80; i<8; i++, j>>=1 ) ZCons::Printf( "%d", (data & j) ? 1 : 0 );
		ZCons::Printf( "B )\n");
		
		break;
	}
	
	case MONITOR_FILL:
	//--------------------------------------------------------------
	//  fill <start-addr> <end-addr> <value>
	//  fill <start-addr> #<size> <value>
	//  メモリを埋める
	//--------------------------------------------------------------
	{
		int start, size, value;
		
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ) size = argv.Val;
		else if( ArgvIs( ARGV_ADDR ) ) size = argv.Val - start +1;
		else                           ErrorMes();
		Shift();
		
		// <data>
		if( !ArgvIs( ARGV_INT )) ErrorMes();
		value = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		for( int i=0; i<size; i++ ) vm->MemWrite( start+i, value );
		
		break;
	}
	
	case MONITOR_MOVE:
	//--------------------------------------------------------------
	//  move <src-addr> <end-addr> <dist-addr>
	//  move <src-addr> #size      <dist-addr>
	//  メモリ転送
	//--------------------------------------------------------------
	{
		int start, size, dist;
		
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ) size = argv.Val;
		else if( ArgvIs( ARGV_ADDR ) ) size = argv.Val - start +1;
		else                           ErrorMes();
		Shift();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		dist = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		// 転送元-転送先が 重ならない
		if( start+size <= dist ) for( int i=0; i<size; i++ )    vm->MemWrite( dist+i, vm->MemRead( start+i ) );
		// 転送元-転送先が 重なる
		else                     for( int i=size-1; i>=0; i-- ) vm->MemWrite( dist+i, vm->MemRead( start+i ) );
		
		break;
	}
	
	case MONITOR_SEARCH:
	//--------------------------------------------------------------
	//  search [<value> [<start-addr> <end-addr>]]
	//  search [<value> [<start-addr> #<size>]]
	//  特定の定数 (1バイト) をサーチ
	//--------------------------------------------------------------
		break;
		
	case MONITOR_OUT:
	//--------------------------------------------------------------
	//  out <port> <data>
	//  特定のポートに出力
	//--------------------------------------------------------------
	{
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <port>
		if( !ArgvIs( ARGV_PORT )) ErrorMes();
		WORD port = argv.Val;
		Shift();
		
		// <data>
		if( !ArgvIs( ARGV_INT )) ErrorMes();
		BYTE data = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->IomOut( port, data );
		
		ZCons::Printf( "OUT port [ %02XH ] <- %02X  (= %d | %+d | ", port, (BYTE)data, (BYTE)data, (int8_t)data );
		int i,j;
		for( i=0, j=0x80; i<8; i++, j>>=1 ) ZCons::Printf( "%d", (data & j) ? 1 : 0 );
		ZCons::Printf( "B )\n");
		
		break;
	}
		
	case MONITOR_LOADMEM:
	//--------------------------------------------------------------
	//  loadmem <filename> <start-addr> <end-addr>
	//  loadmem <filename> <start-addr> #<size>
	// ファイルからメモリにロード
	//--------------------------------------------------------------
	{
		char *fname;
		int start,size;
		FILE *fp;
		
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <filename>
		if( !ArgvIs( ARGV_STR ) ) ErrorMes();
		fname = argv.Str;
		Shift();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ) size = (argv.Val > 0xffff) ? 0xffff : argv.Val;
		else if( ArgvIs( ARGV_ADDR ) ) size = (argv.Val < start ) ? (0x10000 | argv.Val) - start + 1 : argv.Val - start + 1;
		else                           ErrorMes();
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		if( (fp=fopen( fname, "rb" )) != NULL ){
			int addr = start;
			for( int i=0; i<size; i++ ){
				BYTE dat = fgetc( fp );
				vm->MemWrite( (addr++)&0xffff, dat );
			}
			fclose( fp );
			ZCons::Printf( " Loaded [%s] -> %d bytes\n", fname, addr-start );
		}else{
			ZCons::SetColor( FC_RED );
			ZCons::Printf( "Failed : File open error\n" );
			ZCons::SetColor( FC_WHITE );
		}
		break;
	}
		
	case MONITOR_SAVEMEM:
	//--------------------------------------------------------------
	//  savemem <filename> <start-addr> <end-addr>
	//  savemem <filename> <start-addr> #<size>
	//  メモリをファイルにセーブ
	//--------------------------------------------------------------
	{
		char *fname;
		int start,size;
		FILE *fp;
		
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <filename>
		if( !ArgvIs( ARGV_STR ) ) ErrorMes();
		fname = argv.Str;
		Shift();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ) size = (argv.Val > 0xffff) ? 0xffff : argv.Val;
		else if( ArgvIs( ARGV_ADDR ) ) size = (argv.Val < start ) ? (0x10000 | argv.Val) - start + 1 : argv.Val - start + 1;
		else                           ErrorMes();
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		if( (fp=fopen( fname, "wb" )) != NULL ){
			int addr = start;
			for( int i=0; i<size; i++ ){
				if( fputc( vm->MemRead( (addr++)&0xffff ), fp ) == EOF ){
					ZCons::SetColor( FC_RED );
					ZCons::Printf( "Failed : Data write error\n" );
					ZCons::SetColor( FC_WHITE );
					break;
				}
			}
			fclose( fp );
			ZCons::Printf( " Saved [%s] -> %d bytes\n", fname, addr-start );
		}else{
			ZCons::SetColor( FC_RED );
			ZCons::Printf( "Failed : File open error\n" );
			ZCons::SetColor( FC_WHITE );
		}
		break;
	}
		
	case MONITOR_RESET:
	//--------------------------------------------------------------
	//  reset
	//	リセット
	//--------------------------------------------------------------
		if( argv.Type != ARGV_END ) ErrorMes();
		vm->Reset();
		
		break;
		
	case MONITOR_REG:
	//--------------------------------------------------------------
	//  reg <name> <value>
	//  レジスタの内容を変更
	//--------------------------------------------------------------
	{	int re = -1, val=0, i;
		const char *str;
		cZ80::Register reg;
		
		if( argv.Type != ARGV_END ){
			if( !ArgvIs( ARGV_REG )) ErrorMes();		// <name>
			re = argv.Val;
			Shift();
			if( !ArgvIs( ARGV_INT )) ErrorMes();		// <value>
			val = argv.Val;
			Shift();
		}
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->CpumGetRegister( &reg );
		
		switch( re ){
		case ARG_AF:	reg.AF.W = val;		break;
		case ARG_BC:	reg.BC.W = val;		break;
		case ARG_DE:	reg.DE.W = val;		break;
		case ARG_HL:	reg.HL.W = val;		break;
		case ARG_IX:	reg.IX.W = val;		break;
		case ARG_IY:	reg.IY.W = val;		break;
		case ARG_SP:	reg.SP.W = val;		break;
		case ARG_PC:	reg.PC.W = val;		break;
		case ARG_AF1:	reg.AF1.W = val;	break;
		case ARG_BC1:	reg.BC1.W = val;	break;
		case ARG_DE1:	reg.DE1.W = val;	break;
		case ARG_HL1:	reg.HL1.W = val;	break;
		case ARG_I:		val &= 0xff; reg.I = val;		break;
		case ARG_R:		val &= 0xff; reg.R = val;		break;
		case ARG_IFF:	if(val)   val=1; reg.IFF  = val;	break;
		case ARG_IM:	if(val>3) val=2; reg.IM   = val;	break;
		case ARG_HALT:	if(val)   val=1; reg.Halt = val;	break;
		}
		
		vm->CpumSetRegister( &reg );
		
		for( i=0; i<COUNTOF( MonitorArgv ); i++ )
			if( re == MonitorArgv[i].Val ) break;
		if( i == COUNTOF( MonitorArgv ) ) str = "";
		else                              str = MonitorArgv[i].StrU;
		ZCons::Printf( "reg %s <- %04X\n", str, val );
		
		break;
	}
		
	case MONITOR_DISASM:
	//--------------------------------------------------------------
	//  disasm [[<start-addr>][#<steps>]]
	//  逆アセンブル
	//--------------------------------------------------------------
	{	static int SaveDisasmAddr = -1;
		int i, pc;
		int addr = SaveDisasmAddr;
		int step = 16;
		char DisCode[128];
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
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->CpumGetRegister( &reg );
		if( addr == -1 ) addr = reg.PC.W;	// ADDR 未指定時
		
		pc = 0;
		for( i=0; i<step; i++ ){
			pc += vm->CpumDisasm( DisCode, (WORD)(addr+pc) );
			ZCons::Printf( "%s\n", DisCode );
		}
		SaveDisasmAddr = ( addr + pc ) & 0xffff;
		break;
	}
	
	}
}


////////////////////////////////////////////////////////////////
// ヘルプ表示
////////////////////////////////////////////////////////////////
void cWndMon::Help( int cmd )
{
	switch( cmd ){
	case MONITOR_HELP:
		ZCons::Printf(
			QT_TRANSLATE_NOOP("PC6001VX",
			"  help [<cmd>]\n"
			"    ヘルプを表示します\n"
			"    <cmd> ... ヘルプを表示したいコマンド\n"
			"              [omit]... 全コマンドの簡易ヘルプを表示\n"
			)
		);
		break;
		
	case MONITOR_GO:
		ZCons::Printf(
			QT_TRANSLATE_NOOP("PC6001VX",
			"  go\n"
			"    プログラムを実行します\n"
			)
		);
		break;
		
	case MONITOR_TRACE:
		ZCons::Printf(
			QT_TRANSLATE_NOOP("PC6001VX",
			"  trace [#<steps>|<steps>]\n"
			"    execute program specityes times\n"
			"    [all omit]        ... trace some steps (previous steps)\n"
			"    #<steps>, <steps> ... step counts of trace  ( you can omit '#' )\n"
			)
		);
		break;
		
	case MONITOR_STEP:
		ZCons::Printf(
			QT_TRANSLATE_NOOP("PC6001VX",
			"  step [call][jp][rep]\n"
			"    execute program 1 time\n"
			"    [all omit] ... execute 1 step\n"
			"    call       ... not trace CALL instruction\n"
			"    jp         ... not trace DJNZ instruction\n"
			"    rep        ... not trace LD*R/CP*R/IN*R/OT*R instruction\n"
			"    CAUTION)\n"
			"         call/jp/rep are use break-point #10.\n"
			)
		);
		break;
		
	case MONITOR_STEPALL:
		ZCons::Printf(
			QT_TRANSLATE_NOOP("PC6001VX",
			"  S\n"
			"    'step all' と同じ   (stepを参照)\n"
			)
		);
		break;
		
	case MONITOR_BREAK:
		ZCons::Printf(
			QT_TRANSLATE_NOOP("PC6001VX",
			"  break [<action>] <addr|port> [#<No>]\n"
			"  break CLEAR [#<No>]\n"
			"  break\n"
			"    ブレークポイントを設定します\n"
			"    [all omit]  ... 全てのブレークポイントを表示\n"
			"    <action>    ... set action of conditon PC|READ|WRITE|IN|OUT or CLEAR\n"
			"                    PC    ... break if PC reach addr\n"
			"                    READ  ... break if data is read\n"
			"                    WRITE ... break if data is written\n"
			"                    IN    ... break if data is input\n"
			"                    OUT   ... break if data is output\n"
			"                    CLEAR ... clear all break point\n"
			"                    [omit]... select PC\n"
			"    <addr|port> ... specify address or port\n"
			"                    if <action> is CLEAR, this argument is invalid\n"
			"    #<No>       ... number of break point. (#1..#10)\n"
			"                    [omit]... select #1\n"
			"                    CAUTION).. #10 is used by system\n"
			)
		);
		break;
		
	case MONITOR_READ:
		ZCons::Printf(
			QT_TRANSLATE_NOOP("PC6001VX",
			"  read <addr>\n"
			"    メモリを読込みます\n"
			"    <addr> ... 指定アドレス\n"
			)
		);
		
	case MONITOR_WRITE:
		ZCons::Printf(
			QT_TRANSLATE_NOOP("PC6001VX",
			"  write <addr> <data>\n"
			"    メモリに書込みます\n"
			"    <addr> ... 指定アドレス\n"
			"    <data> ... 書込むデータ\n"
			)
		);
		break;
		
	case MONITOR_FILL:
		ZCons::Printf(
			QT_TRANSLATE_NOOP("PC6001VX",
			"  fill <start-addr> <end-addr> <value>\n"
			"  fill <start-addr> #<size>    <value>\n"
			"    メモリを指定値で埋めます\n"
			"    <start-addr> ... 開始アドレス\n"
			"    <end-addr>   ... 終了アドレス\n"
			"    #<size>      ... サイズ\n"
			"    <value>      ... 書込む値\n"
			)
		);
		break;
		
	case MONITOR_MOVE:
		ZCons::Printf(
					QT_TRANSLATE_NOOP("PC6001VX",
									  "  move <src-addr> <end-addr> <dist-addr>\n"
									  "  move <src-addr> #<size>    <dist-addr>\n"
									  "    メモリを転送します\n"
									  "    <src-addr>  ... 転送元開始アドレス\n"
									  "    <end-addr>  ... 転送元終了アドレス\n"
									  "    #<size>     ... 転送サイズ\n"
									  "    <dist-addr> ... 転送先アドレス\n"
									  )
					);
		break;
		
	case MONITOR_SEARCH:
		ZCons::Printf(
					QT_TRANSLATE_NOOP("PC6001VX",
									  "  search [<value> [<start-addr> <end-addr>]]\n"
									  "  search [<value> [<start-addr> #<size>]]\n"
									  "    メモリを検索します\n"
									  "    <value>      ... 検索値\n"
									  "    <start-addr> ... 検索開始アドレス\n"
									  "    <end-addr>   ... 検索終了アドレス\n"
									  "    #<size>      ... 検索サイズ\n"
									  "    [omit-all]   ... 前回の値または文字列を検索\n"
									  )
					);
		break;
		
	case MONITOR_OUT:
		ZCons::Printf(
					QT_TRANSLATE_NOOP("PC6001VX",
									  "  out <port> <data>\n"
									  "    I/Oポートに出力します\n"
									  "    <port> ... I/Oポートアドレス\n"
									  "    <data> ... 出力データ\n"
									  )
					);
		break;
		
	case MONITOR_LOADMEM:
		ZCons::Printf(
					QT_TRANSLATE_NOOP("PC6001VX",
									  "  loadmem <filename> <start-addr> <end-addr>\n"
									  "  loadmem <filename> <start-addr> #<size>\n"
									  "    ファイルからメモリにロードします\n"
									  "    <filename>   ... ファイル名\n"
									  "    <start-addr> ... ロード開始アドレス\n"
									  "    <end-addr>   ... ロード終了アドレス\n"
									  "    #<size>      ... ロードサイズ\n"
									  )
					);
		break;
		
	case MONITOR_SAVEMEM:
		ZCons::Printf(
					QT_TRANSLATE_NOOP("PC6001VX",
									  "  savemem <filename> <start-addr> <end-addr>\n"
									  "  savemem <filename> <start-addr> #<size>\n"
									  "    メモリイメージをファイルにセーブします\n"
									  "    <filename>   ... ファイル名\n"
									  "    <start-addr> ... セーブ開始アドレス\n"
									  "    <end-addr>   ... セーブ終了アドレス\n"
									  "    #<size>      ... セーブサイズ\n"
									  )
					);
		break;
		
	case MONITOR_RESET:
		ZCons::Printf(
					QT_TRANSLATE_NOOP("PC6001VX",
									  "  reset\n"
									  "    PC6001Vをリセットし，アドレス 0000H から実行します\n"
									  )
					);
		break;
		
	case MONITOR_REG:
		ZCons::Printf(
					QT_TRANSLATE_NOOP("PC6001VX",
									  "  reg <name> <value>\n"
									  "    レジスタの値を参照，設定します\n"
									  "    <name>     ... specity register name.\n"
									  "                   AF|BC|DE|HL|AF'|BC'|DE'|HL'|IX|IY|SP|PC|I|R|IFF|IM\n"
									  "    <value>    ... set value\n"
									  )
					);
		break;
		
	case MONITOR_DISASM:
		ZCons::Printf(
					QT_TRANSLATE_NOOP("PC6001VX",
									  "  disasm [[<start-addr>][#<steps>]]\n"
									  "    逆アセンブルします\n"
									  "    [all omit]   ... PCレジスタアドレスから16ステップ分を逆アセンブル\n"
									  "    <start-addr> ... start-addr から逆アセンブルします\n"
									  "                     [omit]... PCレジスタアドレス\n"
									  "    #<steps>     ... 逆アセンブルするステップ数\n"
									  "                     [omit]... 16ステップ\n"
									  )
					);
		break;
		
	}
}







//------------------------------------------------------
//  モニタモードクラス
//------------------------------------------------------

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
Monitor::Monitor( VM6 *vm ) : vm(vm)
{
	for( int i=0; i<COUNTOF(dcn); i++ )
		dcn[i] = NULL;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
Monitor::~Monitor( void )
{
	for( int i=0; i<COUNTOF(dcn); i++ )
		if( dcn[i] ) delete dcn[i];
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool Monitor::Init( void )
{
	dcn[0] = new cWndMon( vm, DEV_ID("MONW") );	// モニタウィンドウ
	dcn[1] = new cWndReg( vm, DEV_ID("REGW") );	// レジスタウィンドウ
	dcn[2] = new cWndMem( vm, DEV_ID("MEMW") );	// メモリウィンドウ
	
	for( int i=0; i<COUNTOF(dcn); i++ )
		if( !(dcn[i] && dcn[i]->Init()) ) return false;
	
	// 位置合わせ
	dcn[0]->SetX( 0 );								dcn[0]->SetY( SCRWINH/12 );
	dcn[1]->SetX( dcn[0]->X() + dcn[0]->Width());	dcn[1]->SetY( 0 );
	dcn[2]->SetX( dcn[1]->X() );					dcn[2]->SetY( dcn[1]->Y() + dcn[1]->Height() );
	return true;
}


////////////////////////////////////////////////////////////////
// ウィンドウ更新
////////////////////////////////////////////////////////////////
void Monitor::Update( void )
{
	for( int i=0; i<COUNTOF(dcn); i++ )
		dcn[i]->Update();
}


////////////////////////////////////////////////////////////////
// モニタモード ウィンドウ幅取得
////////////////////////////////////////////////////////////////
int Monitor::Width( void )
{
	int ww = 0;
	for( int i=0; i<COUNTOF(dcn); i++ )
		ww = max( ww, dcn[i]->X() + dcn[i]->Width() );
	
	return ww;
}


////////////////////////////////////////////////////////////////
// モニタモード ウィンドウ高さ取得
////////////////////////////////////////////////////////////////
int Monitor::Height( void )
{
	int hh = 0;
	for( int i=0; i<COUNTOF(dcn); i++ )
		hh = max( hh, dcn[i]->Y() + dcn[i]->Height() );
	
	return hh;
}




#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
