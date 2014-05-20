#include <time.h>

#include "config.h"
#include "log.h"
#include "cpus.h"
#include "intr.h"
#include "schedule.h"
#include "p6vm.h"


// イベントID
#define	EID_INTCHK	(1)
#define	EID_VECTOR	(2)
#define	EID_DATA	(3)

// ウェイト(us)
#define	WAIT_INTCHK	(300)
//#define	WAIT_INTCHK	(30)
// ウェイト(Z80ステート数)
#define	WAIT_DATA	(15)
#define	WAIT_JOY	(9225)

// 8049動作ステータス
#define	D8049_IDLE		(0)				/* 何もしていない */
#define	D8049_IMASK		(0xff00)		/* 外部割込みステータスマスク */
#define	D8049_INTR		(0x0100)		/* 外部割込み要求 */

#define	D8049_CMTO		(0x0200)		/* CMT 1文字出力 データ待ち */
#define	D8049_TVRW		(0x0300)		/* TV予約書込み データ待ち */

#define	D8049_KEY1		(0x0001)		/* キー割込み1その1 */
#define	D8049_KEY12		(0x0002)		/* キー割込み1その2 */
#define	D8049_KEY2		(0x0003)		/* キー割込み2 */
#define	D8049_KEY3		(0x0004)		/* キー割込み3 */
#define	D8049_CMTR		(0x0005)		/* CMT READ割込み */
#define	D8049_CMTE		(0x0006)		/* CMT ERROR割込み */
#define	D8049_SIO		(0x0007)		/* RS232C受信割込み */
#define	D8049_JOY		(0x0008)		/* ゲーム用キー割込み */
#define	D8049_TVRR		(0x0009)		/* TV予約読込み割込み */
#define	D8049_DATE		(0x000a)		/* DATE割込み */

#define	D8049_NODATA	(0x000f)		/* データ出力なし */

// 割込み要求フラグ
#define	IR_KEY1			(0x0001)
#define	IR_KEY12		(0x0002)
#define	IR_CMTR			(0x0004)
#define	IR_CMTE			(0x0008)
#define	IR_KEY2			(0x0010)
#define	IR_KEY3			(0x0020)
#define	IR_SIO			(0x0040)
#define	IR_JOY			(0x0080)
#define	IR_TVR			(0x0100)
#define	IR_DATE			(0x0200)

// 割込みベクタ
#define	IVEC_NOINTR		(0xff)
#define	IVEC_KEY3		(2)
#define	IVEC_SIO		(4)
#define	IVEC_TIMER		(6)
#define	IVEC_CMT_R		(8)
#define	IVEC_KEY1		(14)
#define	IVEC_KEY12		(16)
#define	IVEC_CMT_E		(18)
#define	IVEC_KEY2		(20)
#define	IVEC_JOY		(22)
#define	IVEC_TVR		(24)
#define	IVEC_DATE		(26)


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
SUB6::SUB6( VM6 *vm, const ID& id ) : Device(vm,id),
	CmtStatus(CMTCLOSE), Status8049(D8049_IDLE),
	IntrFlag(0), KeyCode(0), JoyCode(0), CmtData(0), SioData(0),
	TVRCnt(0), DateCnt(0)
{
	INITARRAY( TVRData,  0xff );
	TVRData[0] = 0xa6;
	INITARRAY( DateData, 0 );
}

SUB60::SUB60( VM6 *vm, const ID& id ) : SUB6(vm,id){}
SUB62::SUB62( VM6 *vm, const ID& id ) : SUB60(vm,id){}
SUB68::SUB68( VM6 *vm, const ID& id ) : SUB62(vm,id){}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
SUB6::~SUB6( void ){}
SUB60::~SUB60( void ){}
SUB62::~SUB62( void ){}
SUB68::~SUB68( void ){}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
void SUB6::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_INTCHK:	// 割込みチェック
		
		if( (Status8049 == D8049_IDLE) && IntrFlag ){
			int wt = WAIT_DATA;
			
			PRINTD( SUB_LOG, "[8049][EventCallback] EID_INTCHK %02X->", Status8049 );
			
			// 割込み要求を調べる
			if     ( IntrFlag & IR_JOY )  { Status8049 = D8049_JOY;   wt += WAIT_JOY; }
			else if( IntrFlag & IR_KEY1 ) { Status8049 = D8049_KEY1;  }
			else if( IntrFlag & IR_KEY12 ){ Status8049 = D8049_KEY12; }
			else if( IntrFlag & IR_CMTR ) { Status8049 = D8049_CMTR;  }
			else if( IntrFlag & IR_CMTE ) { Status8049 = D8049_CMTE;  }
			else if( IntrFlag & IR_KEY2 ) { Status8049 = D8049_KEY2;  }
			else if( IntrFlag & IR_KEY3 ) { Status8049 = D8049_KEY3;  }
			else if( IntrFlag & IR_SIO )  { Status8049 = D8049_SIO;   }
			else if( IntrFlag & IR_TVR )  { Status8049 = D8049_TVRR;  }
			else if( IntrFlag & IR_DATE ) { Status8049 = D8049_DATE;  }
			
			if( Status8049 != D8049_IDLE )
				vm->EventAdd( this, EID_VECTOR, wt, EV_LOOP|EV_STATE );
			
			PRINTD( SUB_LOG, "%02X\n", Status8049 );
		}
		break;
		
	case EID_VECTOR:	// 割込みベクタ出力
		// T0=IBF=H つまり8255のバッファが空いていなければリトライ
		if( GetT0() ) break;
		vm->EventDel( this, EID_VECTOR );
		
		OutVector();	// 割込みベクタ出力
		
		// 続いてデータ出力
		vm->EventAdd( this, EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
		break;
		
	case EID_DATA:		// 割込みデータ出力
		// T0=IBF=H つまり8255のバッファが空いていなければリトライ
		if( GetT0() ) break;
		vm->EventDel( this, EID_DATA );
		
		OutData();		// 割込みデータ出力
		break;
	}
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void SUB6::Reset( void )
{
	IntrFlag   = 0;
	Status8049 = D8049_IDLE;
	CmtStatus  = CMTCLOSE;
	
	KeyCode    = 0;
	JoyCode    = 0;
	CmtData    = 0;
	SioData    = 0;
	
	vm->EventAdd( this, EID_INTCHK, WAIT_INTCHK, EV_LOOP|EV_US );
}


////////////////////////////////////////////////////////////////
// I/Oポートアクセス
////////////////////////////////////////////////////////////////
BYTE SUB6::ReadIO( int addr )
{
	return vm->IosIn( addr );
}

void SUB6::WriteIO( int addr, BYTE data )
{
	vm->IosOut( addr, data );
}


////////////////////////////////////////////////////////////////
// 外部メモリアクセス
////////////////////////////////////////////////////////////////
void SUB6::WriteExt( BYTE data )
{
	vm->IosOut( IO8049_BUS, data );
}

BYTE SUB6::ReadExt( void )
{
	return vm->IosIn( IO8049_BUS );
}


////////////////////////////////////////////////////////////////
// テスト0ステータス取得
////////////////////////////////////////////////////////////////
bool SUB6::GetT0( void )
{
	return vm->IosIn( IO8049_T0 ) ? true : false;
}


////////////////////////////////////////////////////////////////
// テスト1ステータス取得
////////////////////////////////////////////////////////////////
bool SUB6::GetT1( void )
{
	return 0;
}


////////////////////////////////////////////////////////////////
// 外部割込みステータス取得
////////////////////////////////////////////////////////////////
bool SUB6::GetINT( void )
{
	return vm->IosIn( IO8049_INT ) ? true : false;
}


////////////////////////////////////////////////////////////////
// 外部割込み要求
////////////////////////////////////////////////////////////////
void SUB6::ExtIntr( void )
{
	// 外部メモリ入力(8255のPortA)
	BYTE comm = ReadExt();
	
	PRINTD( SUB_LOG, "[8049][ExtIntr] -> " );
	
	// 外部割込み処理中?
	switch( Status8049 ){
	case D8049_CMTO:	// CMT 1文字出力
		PRINTD( SUB_LOG, "CmtPut %02X\n", comm );
		
		vm->CmtsCmtWrite( comm );
		Status8049 = D8049_IDLE;
		break;
		
	case D8049_TVRW:	// TVR 書込み
		PRINTD( SUB_LOG, "TV reserve write %02X\n", comm );
		
		TVRData[TVRCnt++] = comm;
		if( comm == 0xff || TVRCnt >= (int)sizeof(TVRData) )
			Status8049 = D8049_IDLE;
		break;
		
	default:			// ヒマなら新たな外部割込み受付
		PRINTD( SUB_LOG, "Command %02X\n", comm );
		ExtIntrExec( comm );
		// 本当はベクタ(+データ)出力中は外部割込み禁止
		// 後で考える，かも

//	case D8049_IDLE:	// ヒマなら新たな外部割込み受付
//		PRINTD( SUB_LOG, "Command %02X\n", comm );
		
//		ExtIntrExec( comm );
//		break;
		
//	default:			// 何らか処理中
//		PRINTD( SUB_LOG, " %02X pass\n", comm );
	}
}


////////////////////////////////////////////////////////////////
// 外部割込み処理
////////////////////////////////////////////////////////////////
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
//		vm->cmtl->SetBaud( 600 );
		break;
		
	case 0x1e:	// ------ CMT(LOAD) 1200ボー ----------
//		vm->cmtl->SetBaud( 1200 );
		break;
		
	case 0x38:	// ------ CMT 1文字出力 ----------
		Status8049 |= D8049_CMTO;
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
	case 0x30:	// ------ TV予約書込み ----------
		Status8049 |= D8049_TVRW;
		TVRCnt = 0;
		break;
		
	case 0x31:	// ------ TV予約読込み ----------
		ReqTVRReadIntr();
		break;
		
	case 0x32:	// ------ DATE ----------
		ReqDateIntr();
		break;
		
	default:
		SUB62::ExtIntrExec( comm );
	}
}


////////////////////////////////////////////////////////////////
// キー割込み要求
////////////////////////////////////////////////////////////////
void SUB6::ReqKeyIntr( int flag, BYTE data )
{
	// flag
	// bit 0 : ON_STOP
	// bit 1 : ON_GRAPH
	// bit 2 : ON_FUNC
	
	// 前回のキー割込みが未処理だったら無視
	if( IntrFlag & ( IR_KEY1 | IR_KEY12 | IR_KEY2 | IR_KEY3 ) ) return;
	
	if( flag&1 ){	// STOP ?
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
		if( flag&6 ){
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


////////////////////////////////////////////////////////////////
// CMT READ割込み要求
////////////////////////////////////////////////////////////////
void SUB6::ReqCmtIntr( BYTE data )
{
	PRINTD( SUB_LOG, "[8049][ReqCmtIntr]\n" );
	
	// LOADOPENならCMT割込み発生
	if( CmtStatus == LOADOPEN ){
		// 前回のCMT READ割込みが未処理だったら無視
		if( IntrFlag & IR_CMTR ) return;
		
		IntrFlag |= IR_CMTR;
		CmtData   = data;
	}
}


////////////////////////////////////////////////////////////////
// ゲーム用キー割込み要求
////////////////////////////////////////////////////////////////
void SUB6::ReqJoyIntr( void )
{
	PRINTD( SUB_LOG, "[8049][ReqJoyIntr]\n" );
	
	// 前回のゲーム用キー割込みが未処理だったら無視
	if( IntrFlag & IR_JOY ) return;
	
	IntrFlag |= IR_JOY;
	JoyCode   = vm->KeyGetKeyJoy();
}


////////////////////////////////////////////////////////////////
// TV予約読込み割込み要求
////////////////////////////////////////////////////////////////
void SUB68::ReqTVRReadIntr( void )
{
	PRINTD( SUB_LOG, "[8049][ReqTVRReadIntr]\n" );
	
	// 前回のTV予約読込み割込みが未処理だったら無視
	if( IntrFlag & IR_TVR ) return;
	
	IntrFlag |= IR_TVR;
	TVRCnt    = 0;
}


////////////////////////////////////////////////////////////////
// DATE割込み要求
////////////////////////////////////////////////////////////////
void SUB68::ReqDateIntr( void )
{
	PRINTD( SUB_LOG, "[8049][ReqDateIntr]\n" );
	
	// 前回のDATE割込みが未処理だったら無視
	if( IntrFlag & IR_DATE ) return;
	
	IntrFlag |= IR_DATE;
	DateCnt   = 0;
	
	time_t timer;
	tm *t_st;
	
	time( &timer );
	t_st = localtime( &timer );
	if( !t_st->tm_wday ) t_st->tm_wday = 7;	// 曜日補正
	PRINTD( SUB_LOG, "Localtime : %d/%d/%d(%d) %d:%d:%d\n",t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_wday-1,t_st->tm_hour,t_st->tm_min,t_st->tm_sec );
	
	DateData[0] = ((t_st->tm_mon  +  1)<<4) | (t_st->tm_wday -  1);
	DateData[1] = ((t_st->tm_mday / 10)<<4) | (t_st->tm_mday % 10);
	DateData[2] = ((t_st->tm_hour / 10)<<4) | (t_st->tm_hour % 10);
	DateData[3] = ((t_st->tm_min  / 10)<<4) | (t_st->tm_min  % 10);
	DateData[4] = ((t_st->tm_sec  / 10)<<4) | (t_st->tm_sec  % 10);
}


////////////////////////////////////////////////////////////////
// 割込みベクタ出力
////////////////////////////////////////////////////////////////
void SUB6::OutVector( void )
{
	BYTE IntrVector = IVEC_NOINTR;
	
	switch( Status8049 ){
	case D8049_JOY:		// ゲーム用キー割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector JOY]\n" );
		IntrVector = IVEC_JOY;
		break;
		
	case D8049_KEY1:	// キー割込み1 その1 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector KEY1]\n" );
		IntrVector = IVEC_KEY1;
		IntrFlag  &= ~IR_KEY1;
		Status8049 = D8049_NODATA;
		break;
		
	case D8049_KEY12:	// キー割込み1 その2 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector KEY12]\n" );
		IntrVector = IVEC_KEY12;
		IntrFlag  &= ~IR_KEY12;
		Status8049 = D8049_NODATA;
		break;
		
	case D8049_CMTR:	// CMT READ割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector CMTR]\n" );
		IntrVector = IVEC_CMT_R;
		break;
		
	case D8049_CMTE:	// CMT ERROR割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector CMTE]\n" );
		IntrVector = IVEC_CMT_E;
		IntrFlag  &= ~IR_CMTE;
		Status8049 = D8049_NODATA;
		break;
		
	case D8049_KEY2:	// キー割込み2 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector KEY2]\n" );
		IntrVector = IVEC_KEY2;
		break;
		
	case D8049_KEY3:	// キー割込み3 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector KEY3]\n" );
		IntrVector = IVEC_KEY3;
		break;
		
	case D8049_SIO:		// RS232C受信割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector SIO]\n" );
		IntrVector = IVEC_SIO;
		break;
		
	case D8049_TVRR:	// TV予約割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector TVR]\n" );
		IntrVector = IVEC_TVR;
		break;
		
	case D8049_DATE:	// DATE割込み 割込みベクタ出力
		PRINTD( SUB_LOG, "[OutVector DATE]\n" );
		IntrVector = IVEC_DATE;
		break;
		
	default:			// どれでもなければベクタ出力なし
		return;
	}
	
	WriteExt( IntrVector );
	vm->IntReqIntr( IREQ_8049 );
}


////////////////////////////////////////////////////////////////
// 割込みデータ出力
////////////////////////////////////////////////////////////////
void SUB6::OutData( void )
{
	switch( Status8049 ){
	case D8049_JOY:		// ゲーム用キー割込み データ出力
		PRINTD( SUB_LOG, "[OutData JOY] Data: %02X\n", JoyCode );
		WriteExt( JoyCode );
		IntrFlag &= ~IR_JOY;
		break;
		
	case D8049_KEY1:	// キー割込み1 その1 割込みベクタ出力
		IntrFlag &= ~IR_KEY1;
		break;
		
	case D8049_KEY12:	// キー割込み1 その2 割込みベクタ出力
		IntrFlag &= ~IR_KEY12;
		break;
		
	case D8049_CMTR:	// CMT READ割込み データ出力
		PRINTD( SUB_LOG, "[OutData CMTR] Data: %02X\n", CmtData );
		WriteExt( CmtData );
		IntrFlag &= ~IR_CMTR;
		break;
		
	case D8049_CMTE:	// CMT ERROR割込み 割込みベクタ出力
		IntrFlag &= ~IR_CMTE;
		break;
		
	case D8049_KEY2:	// キー割込み2 データ出力
		PRINTD( SUB_LOG, "[OutData KEY2] Data: %02X\n", KeyCode );
		WriteExt( KeyCode );
		IntrFlag &= ~IR_KEY2;
		break;
		
	case D8049_KEY3:	// キー割込み3 データ出力
		PRINTD( SUB_LOG, "[OutData KEY3] Data: %02X\n", KeyCode );
		WriteExt( KeyCode );
		IntrFlag &= ~IR_KEY3;
		break;
		
	case D8049_SIO:		// RS232C受信割込み データ出力
		PRINTD( SUB_LOG, "[OutData SIO] Data: %02X\n", SioData );
		WriteExt( SioData );
		IntrFlag &= ~IR_SIO;
		break;
		
	case D8049_TVRR:{	// TV予約読込み割込み データ出力
		BYTE tvd = TVRCnt >= (int)sizeof(TVRData) ? 0xff : TVRData[TVRCnt++];
		PRINTD( SUB_LOG, "[OutData TVR] Data: %02X\n", tvd );
		WriteExt( tvd );
		if( tvd == 0xff ){	// FFHなら終了
			IntrFlag &= ~IR_TVR;
		}else{				// FFH以外なら残りのデータ出力
			vm->EventAdd( this, EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
			return;
		}
		break;
	}
	case D8049_DATE:	// DATE割込み データ出力
		PRINTD( SUB_LOG, "[OutData DATE] Data%d: %02X\n", DateCnt, DateData[DateCnt] );
		WriteExt( DateData[DateCnt++] );
		if( DateCnt > 4 ){	// 5回出力したら終了
			IntrFlag &= ~IR_DATE;
		}else{				// 5回未満なら残りのデータ出力
			vm->EventAdd( this, EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
			return;
		}
		break;
		
	}
	
	// 割込み処理終了
	Status8049 = D8049_IDLE;
	// CPUに対する割込み要求をキャンセル(割込み禁止対策)
	vm->IntCancelIntr( IREQ_8049 );
}


////////////////////////////////////////////////////////////////
// CMTステータス取得
////////////////////////////////////////////////////////////////
int SUB6::GetCmtStatus( void ) const
{
	return CmtStatus;
}


////////////////////////////////////////////////////////////////
// CMT割込み発生可?
////////////////////////////////////////////////////////////////
bool SUB6::IsCmtIntrReady( void )
{
	// 何もしていなくてLOADOPENで前回のCMT READ割込みが処理済みでIBF=Lだったら発生可
	
	return ( Status8049 == D8049_IDLE ) && ( CmtStatus == LOADOPEN ) &&
			!( IntrFlag & IR_CMTR ) && !GetT0();
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
bool SUB6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "8049", NULL, "CmtStatus",	"%d",		CmtStatus );
	Ini->PutEntry( "8049", NULL, "Status8049",	"%d",		Status8049 );
	Ini->PutEntry( "8049", NULL, "IntrFlag",	"0x%08X",	IntrFlag );
	Ini->PutEntry( "8049", NULL, "KeyCode",		"0x%02X",	KeyCode );
	Ini->PutEntry( "8049", NULL, "JoyCode",		"0x%02X",	JoyCode );
	Ini->PutEntry( "8049", NULL, "CmtData",		"0x%02X",	CmtData );
	Ini->PutEntry( "8049", NULL, "SioData",		"0x%02X",	SioData );
	Ini->PutEntry( "8049", NULL, "TVRCnt",		"0x%02X",	TVRCnt );
	Ini->PutEntry( "8049", NULL, "DateCnt",		"0x%02X",	DateCnt );
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
bool SUB6::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt(   "8049", "CmtStatus",		&CmtStatus,		CmtStatus );
	Ini->GetInt(   "8049", "Status8049",	&Status8049,	Status8049 );
	Ini->GetInt(   "8049", "IntrFlag",		&st,			IntrFlag );	IntrFlag = st;
	Ini->GetInt(   "8049", "KeyCode",		&st,			KeyCode );	KeyCode  = st;
	Ini->GetInt(   "8049", "JoyCode",		&st,			JoyCode );	JoyCode  = st;
	Ini->GetInt(   "8049", "CmtData",		&st,			CmtData );	CmtData  = st;
	Ini->GetInt(   "8049", "SioData",		&st,			SioData );	SioData  = st;
	Ini->GetInt(   "8049", "TVRCnt",		&st,			TVRCnt );	TVRCnt   = st;
	Ini->GetInt(   "8049", "DateCnt",		&st,			DateCnt );	DateCnt  = st;
	
	return true;
}
