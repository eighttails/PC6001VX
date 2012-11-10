#include "config.h"
#include "log.h"
#include "cpus.h"
#include "intr.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "schedule.h"
#include "tape.h"


// イベントID
#define	EID_INTCHK	(1)
#define	EID_VECTOR	(2)
#define	EID_DATA	(3)

// ウェイト(us)
#define	WAIT_INTCHK	(300)
// ウェイト(Z80ステート数)
#define	WAIT_DATA	(15)

// 8049動作ステータス
#define	D8049_IDLE		(0)				/* 何もしていない */
#define	D8049_IMASK		(0xff00)		/* 外部割込みステータスマスク */
#define	D8049_INTR		(0x0100)		/* 外部割込み要求 */

#define	D8049_CMTO		(0x0200)		/* CMT 1文字出力 データ待ち */

#define	D8049_KEY1		(0x0001)		/* キー割込み1その1 */
#define	D8049_KEY12		(0x0002)		/* キー割込み1その2 */
#define	D8049_KEY2		(0x0003)		/* キー割込み2 */
#define	D8049_KEY3		(0x0004)		/* キー割込み3 */
#define	D8049_CMTR		(0x0005)		/* CMT READ割込み */
#define	D8049_CMTE		(0x0006)		/* CMT ERROR割込み */
#define	D8049_SIO		(0x0007)		/* RS232C受信割込み */
#define	D8049_JOY		(0x0008)		/* ゲーム用キー割込み */

#define	D8049_NODATA	(0x0009)		/* データ出力なし */

// 割込み要求フラグ
#define	IR_KEY1			(0x0001)
#define	IR_KEY12		(0x0002)
#define	IR_CMTR			(0x0004)
#define	IR_CMTE			(0x0008)
#define	IR_KEY2			(0x0010)
#define	IR_KEY3			(0x0020)
#define	IR_SIO			(0x0040)
#define	IR_JOY			(0x0080)

// 割込みベクタ
#define	IVEC_NOINTR		(0xff)
#define	IVEC_SIO		(0)
#define	IVEC_KEY3		(2)
#define	IVEC_TIMER		(6)
#define	IVEC_CMT_R		(8)
#define	IVEC_KEY1		(14)
#define	IVEC_KEY12		(16)
#define	IVEC_CMT_E		(18)
#define	IVEC_KEY2		(20)
#define	IVEC_JOY		(22)


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
SUB6::SUB6( VM6 *vm, const P6ID& id ) : P6DEVICE(vm,id),
	CmtStatus(CMTCLOSE), Status8049(D8049_IDLE),
	IntrFlag(0), KeyCode(0), JoyCode(0), CmtData(0), SioData(0) {}

SUB60::SUB60( VM6 *vm, const P6ID& id ) : SUB6(vm,id){}
SUB62::SUB62( VM6 *vm, const P6ID& id ) : SUB6(vm,id){}

////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
SUB6::~SUB6( void ){}
SUB60::~SUB60( void ){}
SUB62::~SUB62( void ){}


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
			// 割込み要求を調べる
			if     ( IntrFlag & IR_JOY )  { Status8049 = D8049_JOY;   }
			else if( IntrFlag & IR_KEY1 ) { Status8049 = D8049_KEY1;  }
			else if( IntrFlag & IR_KEY12 ){ Status8049 = D8049_KEY12; }
			else if( IntrFlag & IR_CMTR ) { Status8049 = D8049_CMTR;  }
			else if( IntrFlag & IR_CMTE ) { Status8049 = D8049_CMTE;  }
			else if( IntrFlag & IR_KEY2 ) { Status8049 = D8049_KEY2;  }
			else if( IntrFlag & IR_KEY3 ) { Status8049 = D8049_KEY3;  }
			else if( IntrFlag & IR_SIO )  { Status8049 = D8049_SIO;   }
			
			if( Status8049 != D8049_IDLE )
				vm->evsc->Add( this, EID_VECTOR, WAIT_DATA, EV_LOOP|EV_STATE );
		}
		break;
		
	case EID_VECTOR:	// 割込みベクタ出力
		// T0=IBF=H つまり8255のバッファが空いていなければリトライ
		if( GetT0() ) break;
		vm->evsc->Del( this, EID_VECTOR );
		
		OutVector();	// 割込みベクタ出力
		
		// 続いてデータ出力
		vm->evsc->Add( this, EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
		break;
		
	case EID_DATA:		// 割込みデータ出力
		// T0=IBF=H つまり8255のバッファが空いていなければリトライ
		if( GetT0() ) break;
		vm->evsc->Del( this, EID_DATA );
		
		// CPUに対する割込み要求をキャンセル(割込み禁止対策)
		vm->intr->CancelIntr( IREQ_8049 );
		
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
	
	vm->evsc->Add( this, EID_INTCHK, WAIT_INTCHK, EV_LOOP|EV_US );
}


////////////////////////////////////////////////////////////////
// I/Oポートアクセス
////////////////////////////////////////////////////////////////
inline BYTE SUB6::ReadIO( int addr )
{
	return vm->ios->In( addr );
}

inline void SUB6::WriteIO( int addr, BYTE data )
{
	vm->ios->Out( addr, data );
}


////////////////////////////////////////////////////////////////
// 外部メモリアクセス
////////////////////////////////////////////////////////////////
inline void SUB6::WriteExt( BYTE data )
{
	vm->ios->Out( IO8049_BUS, data );
}

inline BYTE SUB6::ReadExt( void )
{
	return vm->ios->In( IO8049_BUS );
}


////////////////////////////////////////////////////////////////
// テスト0ステータス取得
////////////////////////////////////////////////////////////////
inline bool SUB6::GetT0( void )
{
	return vm->ios->In( IO8049_T0 ) ? true : false;
}


////////////////////////////////////////////////////////////////
// テスト1ステータス取得
////////////////////////////////////////////////////////////////
inline bool SUB6::GetT1( void )
{
	return 0;
}


////////////////////////////////////////////////////////////////
// 外部割込みステータス取得
////////////////////////////////////////////////////////////////
inline bool SUB6::GetINT( void )
{
	return vm->ios->In( IO8049_INT ) ? true : false;
}


////////////////////////////////////////////////////////////////
// 外部割込み要求
////////////////////////////////////////////////////////////////
void SUB6::ExtIntr( void )
{
//	static bool CmtPut = false;
	
	// 外部メモリ入力(8255のPortA)
	BYTE comm = ReadExt();
	
	// 外部割込み処理中?
	if( Status8049 & D8049_IMASK ){
		// CMT 1文字出力
		if( ( Status8049 & D8049_IMASK ) == D8049_CMTO ){
			PRINTD( SUB_LOG, "[8049][ExtIntr] CmtPut %02X\n", comm );
			
			vm->cmts->CmtWrite( comm );
			Status8049 &= ~D8049_IMASK;
		}
		
	}else{
		// 外部割込み処理
		ExtIntrExec( comm );
	}
}


////////////////////////////////////////////////////////////////
// 外部割込み処理
////////////////////////////////////////////////////////////////
void SUB60::ExtIntrExec( BYTE comm )
{
	PRINTD( SUB_LOG, "[8049][ExtIntrExec] Command %02X\n", comm );
	
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
		vm->cmts->Mount();
		CmtStatus = SAVEOPEN;
		break;
		
	case 0x3a:	// ------ CMT(SAVE) CLOSE ----------
		CmtStatus = CMTCLOSE;
		vm->cmts->Unmount();
		break;
		
	case 0x3d:	// ------ CMT(SAVE) 600ボー ----------
		vm->cmts->SetBaud( 600 );
		break;
		
	case 0x3e:	// ------ CMT(SAVE) 1200ボー ----------
		vm->cmts->SetBaud( 1200 );
		break;
	}
}

void SUB62::ExtIntrExec( BYTE comm )
{
	PRINTD( SUB_LOG, "[8049][ExtIntrExec] Command %02X\n", comm );
	
	// コマンド
	switch( comm ){
	case 0x0c:	// ------ RS-232C ----------
	case 0x2c:
		break;
		
	case 0x04:	// ------ 英字<->かな切換 ----------
		vm->key->ChangeKana();
		break;
		
	case 0x05:	// ------ かな<->カナ切換 ----------
		vm->key->ChangeKKana();
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
		vm->cmts->Mount();
		CmtStatus = SAVEOPEN;
		break;
		
	case 0x3a:	// ------ CMT(SAVE) CLOSE ----------
		CmtStatus = CMTCLOSE;
		vm->cmts->Unmount();
		break;
		
	case 0x3d:	// ------ CMT(SAVE) 600ボー ----------
		vm->cmts->SetBaud( 600 );
		break;
		
	case 0x3e:	// ------ CMT(SAVE) 1200ボー ----------
		vm->cmts->SetBaud( 1200 );
		break;
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
	JoyCode   = vm->key->GetKeyJoy();
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
		
	default:			// どれでもなければベクタ出力なし
		return;
	}
	
	WriteExt( IntrVector );
	vm->intr->ReqIntr( IREQ_8049 );
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
	}
	Status8049 = D8049_IDLE;
}


////////////////////////////////////////////////////////////////
// CMTステータス取得
////////////////////////////////////////////////////////////////
int SUB6::GetCmtStatus( void )
{
	return CmtStatus;
}


////////////////////////////////////////////////////////////////
// CMT割込み発生可?
////////////////////////////////////////////////////////////////
bool SUB6::IsCmtIntrReady( void )
{
	// 何もしていなくてLOADOPENで前回のCMT READ割込みが処理済みでIBF=Lだったら発生可
	// BoostUp有効の場合はワークエリアもチェック
	
	return( ( Status8049 == D8049_IDLE ) &&
			( CmtStatus == LOADOPEN ) &&
			!( IntrFlag & IR_CMTR ) &&
			!GetT0() &&
			!( vm->cmtl->IsBoostUp() && ( vm->mem->Read( 0xfa19 ) & 2 ) )	// ワークエリアのフラグ参照
			);
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
bool SUB6::DokoSave( cIni *Ini )
{
	int i;
	cSche::evinfo e;
	char stren[16];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	Ini->PutEntry( "8049", NULL, "CmtStatus",	"%d",		CmtStatus );
	Ini->PutEntry( "8049", NULL, "Status8049",	"%d",		Status8049 );
	Ini->PutEntry( "8049", NULL, "IntrFlag",	"0x%08X",	IntrFlag );
	Ini->PutEntry( "8049", NULL, "KeyCode",		"0x%02X",	KeyCode );
	Ini->PutEntry( "8049", NULL, "JoyCode",		"0x%02X",	JoyCode );
	Ini->PutEntry( "8049", NULL, "CmtData",		"0x%02X",	CmtData );
	Ini->PutEntry( "8049", NULL, "SioData",		"0x%02X",	SioData );
	
	// イベント
	int eid[] = { EID_INTCHK, EID_VECTOR, EID_DATA, 0 };
	i = 0;
	
	while( eid[i] ){
		e.id = eid[i++];
		if( vm->evsc->GetEvinfo( &e ) ){
			sprintf( stren, "Event%08X", e.id );
			Ini->PutEntry( "8049", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
		}
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
bool SUB6::DokoLoad( cIni *Ini )
{
	int st,yn,i;
	cSche::evinfo e;
	char stren[16];
	char strrs[64];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	Ini->GetInt(   "8049", "CmtStatus",		&CmtStatus,		CmtStatus );
	Ini->GetInt(   "8049", "Status8049",	&Status8049,	Status8049 );
	Ini->GetInt(   "8049", "IntrFlag",		&st,		IntrFlag );	IntrFlag = st;
	Ini->GetInt(   "8049", "KeyCode",		&st,		KeyCode );	KeyCode  = st;
	Ini->GetInt(   "8049", "JoyCode",		&st,		JoyCode );	JoyCode  = st;
	Ini->GetInt(   "8049", "CmtData",		&st,		CmtData );	CmtData  = st;
	Ini->GetInt(   "8049", "SioData",		&st,		SioData );	SioData  = st;
	
	// イベント
	int eid[] = { EID_INTCHK, EID_VECTOR, EID_DATA, 0 };
	i = 0;
	
	while( eid[i] ){
		e.id = eid[i++];
		sprintf( stren, "Event%08X", e.id );
		if( Ini->GetString( "8049", stren, strrs, "" ) ){
			sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
			e.Active = yn ? true : false;
			if( !vm->evsc->SetEvinfo( &e ) ) return false;
		}
	}
	
	return true;
}
