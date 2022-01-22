/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <ctime>

#include "config.h"
#include "cpus.h"
#include "intr.h"
#include "log.h"
#include "p6vm.h"
#include "schedule.h"


// イベントID
#define	EID_INTCHK		(1)
#define	EID_VECTOR		(2)
#define	EID_DATA		(3)

// ウェイト(us)
#define	WAIT_INTCHK		(300)
//#define	WAIT_INTCHK		(30)
// ウェイト(Z80ステート数)
#define	WAIT_DATA		(15)
#define	WAIT_JOY		(9225)

// 割込み要求フラグ
#define	IR_KEY1			(0b000000000001)
#define	IR_KEY12		(0b000000000010)
#define	IR_CMTR			(0b000000000100)
#define	IR_CMTE			(0b000000001000)
#define	IR_KEY2			(0b000000010000)
#define	IR_KEY3			(0b000000100000)
#define	IR_SIO			(0b000001000000)
#define	IR_JOY			(0b000010000000)
#define	IR_TVR			(0b000100000000)
#define	IR_DATE			(0b001000000000)

// 割込みベクタ
#define	IVEC_NOINTR		(0xff)
#define	IVEC_KEY3		(0x02)
#define	IVEC_SIO		(0x04)
#define	IVEC_TIMER		(0x06)
#define	IVEC_CMT_R		(0x08)
#define	IVEC_KEY1		(0x0e)
#define	IVEC_KEY12		(0x10)
#define	IVEC_CMT_E		(0x12)
#define	IVEC_KEY2		(0x14)
#define	IVEC_JOY		(0x16)
#define	IVEC_TVR		(0x18)
#define	IVEC_DATE		(0x1a)


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
SUB6::SUB6( VM6* vm, const ID& id ) : Device( vm, id ),
	CpuStatus( SS_IDLE ), CmtStatus( CMTCLOSE ),
	IntrFlag( 0 ), KeyCode( 0 ), JoyCode( 0 ), CmtData( 0 ), SioData( 0 ),
	TVRCnt( 0 ), DateCnt( 0 )
{
	TVRData.fill( 0xff );
	TVRData[0] = 0xa6;
	DateData.fill( 0 );
}

SUB60::SUB60( VM6* vm, const ID& id ) : SUB6( vm, id )
{
}

SUB62::SUB62( VM6* vm, const ID& id ) : SUB6( vm, id )
{
}

SUB68::SUB68( VM6* vm, const ID& id ) : SUB6( vm, id )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
SUB6::~SUB6( void )
{
}

SUB60::~SUB60( void )
{
}

SUB62::~SUB62( void )
{
}

SUB68::~SUB68( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SUB6::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_INTCHK:	// 割込みチェック
		if( (CpuStatus == SS_IDLE) && IntrFlag ){
			int wt = WAIT_DATA;
			
			PRINTD( SUB_LOG, "[8049][EventCallback] EID_INTCHK %02X->", CpuStatus );
			
			// 割込み要求を調べる
			if     ( IntrFlag & IR_JOY )  { CpuStatus = SS_JOY;   wt += WAIT_JOY; }
			else if( IntrFlag & IR_KEY1 ) { CpuStatus = SS_KEY1;  }
			else if( IntrFlag & IR_KEY12 ){ CpuStatus = SS_KEY12; }
			else if( IntrFlag & IR_CMTR ) { CpuStatus = SS_CMTR;  }
			else if( IntrFlag & IR_CMTE ) { CpuStatus = SS_CMTE;  }
			else if( IntrFlag & IR_KEY2 ) { CpuStatus = SS_KEY2;  }
			else if( IntrFlag & IR_KEY3 ) { CpuStatus = SS_KEY3;  }
			else if( IntrFlag & IR_SIO )  { CpuStatus = SS_SIO;   }
			else if( IntrFlag & IR_TVR )  { CpuStatus = SS_TVRR;  }
			else if( IntrFlag & IR_DATE ) { CpuStatus = SS_DATE;  }
			
			if( CpuStatus != SS_IDLE ){
				vm->EventAdd( Device::GetID(), EID_VECTOR, wt, EV_LOOP|EV_STATE );
			}
			
			PRINTD( SUB_LOG, "%02X\n", CpuStatus );
		}
		break;
		
	case EID_VECTOR:	// 割込みベクタ出力
		// T0=IBF=H つまり8255のバッファが空いていなければリトライ
		if( GetT0() ){ break; }
		
		vm->EventDel( Device::GetID(), EID_VECTOR );
		
		OutVector();	// 割込みベクタ出力
		
		// 続いてデータ出力
		if( CpuStatus != SS_IDLE ){
			vm->EventAdd( Device::GetID(), EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
		}
		break;
		
	case EID_DATA:		// 割込みデータ出力
		// T0=IBF=H つまり8255のバッファが空いていなければリトライ
		if( GetT0() ){ break; }
		
		vm->EventDel( Device::GetID(), EID_DATA );
		
		OutData();		// 割込みデータ出力
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// リセット
/////////////////////////////////////////////////////////////////////////////
void SUB6::Reset( void )
{
	IntrFlag  = 0;
	CpuStatus = SS_IDLE;
	CmtStatus = CMTCLOSE;
	
	KeyCode   = 0;
	JoyCode   = 0;
	CmtData   = 0;
	SioData   = 0;
	
	vm->EventAdd( Device::GetID(), EID_INTCHK, WAIT_INTCHK, EV_LOOP|EV_US );
}


/////////////////////////////////////////////////////////////////////////////
// I/Oポートアクセス
/////////////////////////////////////////////////////////////////////////////
BYTE SUB6::ReadIO( int addr )
{
	return vm->IosIn( addr );
}

void SUB6::WriteIO( int addr, BYTE data )
{
	vm->IosOut( addr, data );
}


/////////////////////////////////////////////////////////////////////////////
// 外部メモリアクセス
/////////////////////////////////////////////////////////////////////////////
void SUB6::WriteExt( BYTE data )
{
	vm->IosOut( IO_BUS, data );
}

BYTE SUB6::ReadExt( void )
{
	return vm->IosIn( IO_BUS );
}


/////////////////////////////////////////////////////////////////////////////
// テスト0ステータス取得
/////////////////////////////////////////////////////////////////////////////
bool SUB6::GetT0( void )
{
	return vm->IosIn( IO_T0 ) ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// テスト1ステータス取得
/////////////////////////////////////////////////////////////////////////////
bool SUB6::GetT1( void )
{
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// 外部割込みステータス取得
/////////////////////////////////////////////////////////////////////////////
bool SUB6::GetINT( void )
{
	return vm->IosIn( IO_INT ) ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// 外部割込み要求
/////////////////////////////////////////////////////////////////////////////
void SUB6::ExtIntr( void )
{
	// 外部メモリ入力(8255のPortA)
	BYTE comm = ReadExt();
	
	PRINTD( SUB_LOG, "[8049][ExtIntr] -> " );
	
	// 外部割込み処理中?
	switch( CpuStatus ){
	case SS_CMTO:	// CMT 1文字出力
		PRINTD( SUB_LOG, "CmtPut %02X\n", comm );
		
		vm->CmtsWriteOne( comm );
		CpuStatus = SS_IDLE;
		break;
		
	case SS_TVRW:	// TVR 書込み
		PRINTD( SUB_LOG, "TV reserve write %02X\n", comm );
		
		try{
			TVRData.at( TVRCnt++ ) = comm;
		}
		catch( std::out_of_range& ){}
		
		if( comm == 0xff || TVRCnt >= (int)TVRData.size() ){
			CpuStatus = SS_IDLE;
		}
		break;
		
	case SS_IDLE:	// ヒマなら新たな外部割込み受付
		PRINTD( SUB_LOG, "Command %02X\n", comm );
		
		ExtIntrExec( comm );
		break;
		
	default:		// 何らか処理中
		PRINTD( SUB_LOG, " %02X << Cancel >> (%d)\n", comm, CpuStatus );
	}
}


/////////////////////////////////////////////////////////////////////////////
// 外部割込み処理
/////////////////////////////////////////////////////////////////////////////
void SUB6::ExtIntrExec( BYTE comm )
{
	// コマンド
	switch( comm ){
	case 0x0c:	// ------ RS-232C ----------
	case 0x2c:
		break;
		
	case 0x06:	// ------ STICK,STRIG ----------
		// ゲーム用キー割込み要求
		ReqJoyIntr();
		break;
		
	case 0x19:	// ------ CMT(LOAD) OPEN ----------
		CmtStatus = LOADOPEN;
		break;
		
	case 0x1a:	// ------ CMT(LOAD) CLOSE ----------
		CmtStatus = CMTCLOSE;
		break;
		
	case 0x1d:	// ------ CMT(LOAD) 600ボー ----------
//		vm->CmtlSetBaud( 600 );
		break;
		
	case 0x1e:	// ------ CMT(LOAD) 1200ボー ----------
//		vm->CmtlSetBaud( 1200 );
		break;
		
	case 0x38:	// ------ CMT 1文字出力 ----------
		CpuStatus = SS_CMTO;
		break;
		
	case 0x39:	// ------ CMT(SAVE) OPEN ----------
		vm->CmtsMount();
		CmtStatus = SAVEOPEN;
		break;
		
	case 0x3a:	// ------ CMT(SAVE) CLOSE ----------
		CmtStatus = CMTCLOSE;
		vm->CmtsUnmount();
		break;
		
	case 0x3d:	// ------ CMT(SAVE) 600ボー ----------
		vm->CmtsSetBaud( 600 );
		break;
		
	case 0x3e:	// ------ CMT(SAVE) 1200ボー ----------
		vm->CmtsSetBaud( 1200 );
		break;
		
	default:
		PRINTD( SUB_LOG, " %02X << Unknown >>\n", comm );
	}
}

void SUB62::ExtIntrExec( BYTE comm )
{
	// コマンド
	switch( comm ){
	case 0x04:	// ------ 英字<->かな切換 ----------
		vm->KeyChangeKana();
		break;
		
	case 0x05:	// ------ かな<->カナ切換 ----------
		vm->KeyChangeKKana();
		break;
		
	default:
		SUB6::ExtIntrExec( comm );
	}
}

void SUB68::ExtIntrExec( BYTE comm )
{
	// コマンド
	switch( comm ){
	case 0x04:	// ------ 英字<->かな切換 ----------
		vm->KeyChangeKana();
		break;
		
	case 0x05:	// ------ かな<->カナ切換 ----------
		vm->KeyChangeKKana();
		break;
		
	case 0x30:	// ------ TV予約書込み ----------
		CpuStatus = SS_TVRW;
		TVRCnt = 0;
		break;
		
	case 0x31:	// ------ TV予約読込み ----------
		ReqTVRReadIntr();
		break;
		
	case 0x32:	// ------ DATE ----------
		ReqDateIntr();
		break;
		
	default:
		SUB6::ExtIntrExec( comm );
	}
}


/////////////////////////////////////////////////////////////////////////////
// キー割込み要求
/////////////////////////////////////////////////////////////////////////////
void SUB6::ReqKeyIntr( int flag, BYTE data )
{
	// flag
	// bit 0 : ON_STOP
	// bit 1 : ON_GRAPH
	// bit 2 : ON_FUNC
	
	// 前回のキー割込みが未処理だったら無視
	if( IntrFlag & ( IR_KEY1 | IR_KEY12 | IR_KEY2 | IR_KEY3 ) ){ return; }
	
	if( flag & 1 ){	// STOP ?
		// CMT OPEN ?
		if( CmtStatus ){
			PRINTD( SUB_LOG, "[8049][ReqKeyIntr] KEY1 03\n" );
			IntrFlag |= IR_KEY1;
			KeyCode   = 0x03;
		}else{
			PRINTD( SUB_LOG, "[8049][ReqKeyIntr] KEY2(STOP) FA\n" );
			IntrFlag |= IR_KEY2;
			KeyCode   = 0xfa;
		}
	}else{			// STOP以外
		// GRAPH or FUNCTION KEY ?
		if( flag & 6 ){
			PRINTD( SUB_LOG, "[8049][ReqKeyIntr] KEY2 %02X\n", data );
			IntrFlag |= IR_KEY2;
			KeyCode   = data;
		}else{
			PRINTD( SUB_LOG, "[8049][ReqKeyIntr] KEY3 %02X\n", data );
			IntrFlag |= IR_KEY3;
			KeyCode   = data;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMT READ割込み要求
/////////////////////////////////////////////////////////////////////////////
void SUB6::ReqCmtIntr( BYTE data )
{
	PRINTD( SUB_LOG, "[8049][ReqCmtIntr]\n" );
	
	// LOADOPENならCMT割込み発生
	if( CmtStatus == LOADOPEN ){
		// 前回のCMT READ割込みが未処理だったら無視
		if( IntrFlag & IR_CMTR ){ return; }
		
		IntrFlag |= IR_CMTR;
		CmtData   = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// ゲーム用キー割込み要求
/////////////////////////////////////////////////////////////////////////////
void SUB6::ReqJoyIntr( void )
{
	PRINTD( SUB_LOG, "[8049][ReqJoyIntr]\n" );
	
	// 前回のゲーム用キー割込みが未処理だったら無視
	if( IntrFlag & IR_JOY ){ return; }
	
	IntrFlag |= IR_JOY;
	JoyCode   = vm->KeyGetKeyJoy();
}


/////////////////////////////////////////////////////////////////////////////
// TV予約読込み割込み要求
/////////////////////////////////////////////////////////////////////////////
void SUB68::ReqTVRReadIntr( void )
{
	PRINTD( SUB_LOG, "[8049][ReqTVRReadIntr]\n" );
	
	// 前回のTV予約読込み割込みが未処理だったら無視
	if( IntrFlag & IR_TVR ){ return; }
	
	IntrFlag |= IR_TVR;
	TVRCnt    = 0;
}


/////////////////////////////////////////////////////////////////////////////
// DATE割込み要求
/////////////////////////////////////////////////////////////////////////////
void SUB68::ReqDateIntr( void )
{
	PRINTD( SUB_LOG, "[8049][ReqDateIntr]\n" );
	
	// 前回のDATE割込みが未処理だったら無視
	if( IntrFlag & IR_DATE ){ return; }
	
	IntrFlag |= IR_DATE;
	DateCnt   = 0;
	
	time_t timer;
	tm* t_st;
	
	time( &timer );
	t_st = localtime( &timer );
	if( !t_st->tm_wday ){	// 曜日補正
		t_st->tm_wday = 7;
	}
	PRINTD( SUB_LOG, "Localtime : %d/%d/%d(%d) %d:%d:%d\n",t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_wday-1,t_st->tm_hour,t_st->tm_min,t_st->tm_sec );
	
	try{
		DateData.at( 0 ) = ((t_st->tm_mon  +  1)<<4) | (t_st->tm_wday -  1);
		DateData.at( 1 ) = ((t_st->tm_mday / 10)<<4) | (t_st->tm_mday % 10);
		DateData.at( 2 ) = ((t_st->tm_hour / 10)<<4) | (t_st->tm_hour % 10);
		DateData.at( 3 ) = ((t_st->tm_min  / 10)<<4) | (t_st->tm_min  % 10);
		DateData.at( 4 ) = ((t_st->tm_sec  / 10)<<4) | (t_st->tm_sec  % 10);
	}
	catch( std::out_of_range& ){}
}


/////////////////////////////////////////////////////////////////////////////
// 割込みベクタ出力
/////////////////////////////////////////////////////////////////////////////
void SUB6::OutVector( void )
{
	BYTE IntrVector = IVEC_NOINTR;
	
	switch( CpuStatus ){
	case SS_JOY:	// ゲーム用キー割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector JOY]\n" );
		IntrVector = IVEC_JOY;
		break;
		
	case SS_KEY1:	// キー割込み1 その1 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector KEY1]\n" );
		IntrVector = IVEC_KEY1;
		IntrFlag  &= ~IR_KEY1;
		CpuStatus = SS_IDLE;
		break;
		
	case SS_KEY12:	// キー割込み1 その2 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector KEY12]\n" );
		IntrVector = IVEC_KEY12;
		IntrFlag  &= ~IR_KEY12;
		CpuStatus = SS_IDLE;
		break;
		
	case SS_CMTR:	// CMT READ割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector CMTR]\n" );
		IntrVector = IVEC_CMT_R;
		break;
		
	case SS_CMTE:	// CMT ERROR割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector CMTE]\n" );
		IntrVector = IVEC_CMT_E;
		IntrFlag  &= ~IR_CMTE;
		CpuStatus = SS_IDLE;
		break;
		
	case SS_KEY2:	// キー割込み2 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector KEY2]\n" );
		IntrVector = IVEC_KEY2;
		break;
		
	case SS_KEY3:	// キー割込み3 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector KEY3]\n" );
		IntrVector = IVEC_KEY3;
		break;
		
	case SS_SIO:	// RS232C受信割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector SIO]\n" );
		IntrVector = IVEC_SIO;
		break;
		
	case SS_TVRR:	// TV予約割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector TVR]\n" );
		IntrVector = IVEC_TVR;
		break;
		
	case SS_DATE:	// DATE割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector DATE]\n" );
		IntrVector = IVEC_DATE;
		break;
		
	default:		// どれでもなければベクタ出力なし
		return;
	}
	
	WriteExt( IntrVector );
	vm->IntReqIntr( IREQ_8049 );
}


/////////////////////////////////////////////////////////////////////////////
// 割込みデータ出力
/////////////////////////////////////////////////////////////////////////////
void SUB6::OutData( void )
{
	BYTE dat;
	
	switch( CpuStatus ){
	case SS_JOY:	// ゲーム用キー割込み データ出力
		PRINTD( SUB_LOG, "[OutData JOY] Data: %02X\n", JoyCode );
		WriteExt( JoyCode );
		IntrFlag &= ~IR_JOY;
		break;
		
	case SS_CMTR:	// CMT READ割込み データ出力
		PRINTD( SUB_LOG, "[OutData CMTR] Data: %02X\n", CmtData );
		WriteExt( CmtData );
		IntrFlag &= ~IR_CMTR;
		break;
		
	case SS_KEY2:	// キー割込み2 データ出力
		PRINTD( SUB_LOG, "[OutData KEY2] Data: %02X\n", KeyCode );
		WriteExt( KeyCode );
		IntrFlag &= ~IR_KEY2;
		break;
		
	case SS_KEY3:	// キー割込み3 データ出力
		PRINTD( SUB_LOG, "[OutData KEY3] Data: %02X\n", KeyCode );
		WriteExt( KeyCode );
		IntrFlag &= ~IR_KEY3;
		break;
		
	case SS_SIO:	// RS232C受信割込み データ出力
		PRINTD( SUB_LOG, "[OutData SIO] Data: %02X\n", SioData );
		WriteExt( SioData );
		IntrFlag &= ~IR_SIO;
		break;
		
	case SS_TVRR:	// TV予約読込み割込み データ出力
		try{
			dat = TVRData.at( TVRCnt++ );
		}
		catch( std::out_of_range& ){
			dat = 0xff;
		}
		
		PRINTD( SUB_LOG, "[OutData TVR] Data%d: %02X\n", TVRCnt - 1, dat );
		WriteExt( dat );
		if( dat == 0xff ){	// FFHなら終了
			IntrFlag &= ~IR_TVR;
		}else{				// FFH以外なら残りのデータ出力
			vm->EventAdd( Device::GetID(), EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
			return;
		}
		break;
		
	case SS_DATE:	// DATE割込み データ出力
		try{
			dat = DateData.at( DateCnt++ );
		}
		catch( std::out_of_range& ){
			dat = 0;
		}
		
		PRINTD( SUB_LOG, "[OutData DATE] Data%d: %02X\n", DateCnt - 1, dat );
		WriteExt( dat );
		if( DateCnt > 4 ){	// 5回出力したら終了
			IntrFlag &= ~IR_DATE;
		}else{				// 5回未満なら残りのデータ出力
			vm->EventAdd( Device::GetID(), EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
			return;
		}
		break;
	}
	
	// 割込み処理終了
	CpuStatus = SS_IDLE;
	// CPUに対する割込み要求をキャンセル(割込み禁止対策)
	vm->IntCancelIntr( IREQ_8049 );
}


/////////////////////////////////////////////////////////////////////////////
// CMTステータス取得
/////////////////////////////////////////////////////////////////////////////
int SUB6::GetCmtStatus( void ) const
{
	return CmtStatus;
}


/////////////////////////////////////////////////////////////////////////////
// CMT割込み発生可?
/////////////////////////////////////////////////////////////////////////////
bool SUB6::IsCmtIntrReady( void )
{
	// 何もしていなくてLOADOPENで前回のCMT READ割込みが処理済みでIBF=Lだったら発生可
	
	return ( CpuStatus == SS_IDLE ) && ( CmtStatus == LOADOPEN ) &&
			!( IntrFlag & IR_CMTR ) && !GetT0();
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/////////////////////////////////////////////////////////////////////////////
// 動作ステータス取得
/////////////////////////////////////////////////////////////////////////////
int SUB6::GetStatus( void ) const
{
	return CpuStatus;
}

#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
/////////////////////////////////////////////////////////////////////////////
bool SUB6::DokoSave( cIni* Ini )
{
	if( !Ini ){ return false; }
	
	Ini->SetVal( "8049", "CmtStatus",	"", CmtStatus  );
	Ini->SetVal( "8049", "CpuStatus",	"", CpuStatus );
	Ini->SetVal( "8049", "IntrFlag",	"", "0x%08X",	IntrFlag );
	Ini->SetVal( "8049", "KeyCode",		"", "0x%02X",	KeyCode  );
	Ini->SetVal( "8049", "JoyCode",		"", "0x%02X",	JoyCode  );
	Ini->SetVal( "8049", "CmtData",		"", "0x%02X",	CmtData  );
	Ini->SetVal( "8049", "SioData",		"", "0x%02X",	SioData  );
	Ini->SetVal( "8049", "TVRCnt",		"", "0x%02X",	TVRCnt   );
	Ini->SetVal( "8049", "DateCnt",		"", "0x%02X",	DateCnt  );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
/////////////////////////////////////////////////////////////////////////////
bool SUB6::DokoLoad( cIni* Ini )
{
	if( !Ini ){ return false; }
	
	Ini->GetVal( "8049", "CmtStatus",	CmtStatus  );
	Ini->GetVal( "8049", "CpuStatus",	CpuStatus );
	Ini->GetVal( "8049", "IntrFlag",	IntrFlag   );
	Ini->GetVal( "8049", "KeyCode",		KeyCode    );
	Ini->GetVal( "8049", "JoyCode",		JoyCode    );
	Ini->GetVal( "8049", "CmtData",		CmtData    );
	Ini->GetVal( "8049", "SioData",		SioData    );
	Ini->GetVal( "8049", "TVRCnt",		TVRCnt     );
	Ini->GetVal( "8049", "DateCnt",		DateCnt    );
	
	return true;
}
