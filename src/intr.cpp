#include "pc6001v.h"
#include "p6el.h"
#include "log.h"
#include "intr.h"
#include "cpus.h"
#include "schedule.h"
#include "pio.h"


// イベントID
#define	EID_TIMER	(1)


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
INT6::INT6( VM6 *vm, const P6ID& id ) : P6DEVICE(vm,id),
	IntrFlag(0), TimerIntrEnable(false), TimerAddr(0), TimerCntUp(0) {}
INT60::INT60( VM6 *vm, const ID& id ) : INT6(vm,id), Device(id) {}
INT62::INT62( VM6 *vm, const ID& id ) : INT6(vm,id), Device(id),
	TimerIntrEnable2(false), Int1IntrEnable(false), Int2IntrEnable(false),
	Int1AddrOutput(false), Int2AddrOutput(false), Int1Addr(0), Int2Addr(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
INT6::~INT6( void ){}
INT60::~INT60( void ){}
INT62::~INT62( void ){}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
////////////////////////////////////////////////////////////////
void INT6::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_TIMER:		// タイマ割込み発生
		PRINTD( INTR_LOG, "[INTR][EventCallback] TimerIntr\n" );
		ReqIntr( IREQ_TIMER );
		break;
	}
}


////////////////////////////////////////////////////////////////
// タイマ割込み周波数設定
////////////////////////////////////////////////////////////////
void INT6::SetTimerIntrHz( BYTE data, BYTE first )
{
	TimerCntUp = data;
	
	// イベント追加
	vm->evsc->Add( this, EID_TIMER, (double)(2048)*(TimerCntUp+1), EV_LOOP|EV_STATE );
	
	// 初回周期の指定がある場合の処理
	if( first ){
		cSche::evinfo e;
		
		e.device = this;
		e.id     = EID_TIMER;
		
		vm->evsc->GetEvinfo( &e );
		e.Clock = (e.Clock*first)/100;
		vm->evsc->SetEvinfo( &e );
	}
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void INT60::Reset( void )
{
	PRINTD( INTR_LOG, "[INTR][Reset]\n" );
	
	// 割込みアドレス初期値設定
	TimerAddr = 6;
	
	// 各種変数初期化
	TimerCntUp       = 3;
	TimerIntrEnable  = false;	// タイマ割込み許可フラグ降ろす
	IntrFlag         = 0;		// 割込み要求フラグクリア
	
	// タイマ割込み周波数初期化
	SetTimerIntrHz(	TimerCntUp );

}

void INT62::Reset( void )
{
	PRINTD( INTR_LOG, "[INTR][Reset]\n" );
	
	// 割込みアドレス初期値設定
	TimerAddr  = 6;
	Int1Addr   = 0;
	Int2Addr   = 0;
	
	// 各種変数初期化
	TimerCntUp       = 3;
	TimerIntrEnable  = false;	// タイマ割込み許可フラグ降ろす
	TimerIntrEnable2 = false;	// タイマ割込み許可フラグ降ろす(mk2以降)
	Int1IntrEnable   = true;	// INT1割込み許可フラグ立てる
	Int2IntrEnable   = false;	// INT2割込み許可フラグ降ろす
	Int1AddrOutput   = false;	// INT1割込みアドレス出力フラグ降ろす
	Int2AddrOutput   = false;	// INT2割込みアドレス出力フラグ降ろす
	IntrFlag         = 0;		// 割込み要求フラグクリア
	
	// タイマ割込み周波数初期化
	SetTimerIntrHz(	TimerCntUp );
}


////////////////////////////////////////////////////////////////
// 割込みチェック
//   割込み発生源はタイマか8049で,タイマ優先か?
////////////////////////////////////////////////////////////////
int INT6::IntrCheck( void )
{
	int	IntrNo = -1;
	
	// タイマ割込み
	if( ( IntrFlag & IREQ_TIMER ) && IsTimerIntrEnable() ){
		PRINTD( INTR_LOG, "[INTR][IntrCheck](Timer)\n" );
		
		CancelIntr( IREQ_TIMER );
		IntrNo    = TimerAddr>>1;
	}
	// 8049割込み
	else if( IntrFlag & IREQ_8049 ){
		PRINTD( INTR_LOG, "[INTR][IntrCheck](8049)\n" );
		
		CancelIntr( IREQ_8049 );
		IntrNo = vm->pio->ReadA() / 2;
	}
	
	if( IntrNo >= 0 ){
		PRINTD( INTR_LOG, " IntrNo = %d\n", IntrNo  );
		IntrNo <<= 1;
	}
	
	return IntrNo;
}


////////////////////////////////////////////////////////////////
// 割込み要求
////////////////////////////////////////////////////////////////
void INT6::ReqIntr( DWORD vec )
{
	IntrFlag |= vec;
	
	PRINTD( INTR_LOG, "[INTR][ReqIntr] %04X -> %04X\n", (unsigned int)vec, (unsigned int)IntrFlag );
}


////////////////////////////////////////////////////////////////
// 割込み撤回
////////////////////////////////////////////////////////////////
void INT6::CancelIntr( DWORD vec )
{
	IntrFlag &= ~vec;
	
	PRINTD( INTR_LOG, "[INTR][CancelIntr] %04X -> %04X\n", (unsigned int)vec, (unsigned int)IntrFlag );
}


////////////////////////////////////////////////////////////////
// タイマ割込み許可チェック
////////////////////////////////////////////////////////////////
bool INT6::IsTimerIntrEnable( void )
{
	return TimerIntrEnable;
}
bool INT62::IsTimerIntrEnable( void )
{
	return TimerIntrEnable && TimerIntrEnable2;
}


////////////////////////////////////////////////////////////////
// 割込み許可フラグ設定
////////////////////////////////////////////////////////////////
void INT62::SetIntrEnable( BYTE data )
{
	Int1IntrEnable   = (data&0x01 ? false : true);
	Int2IntrEnable   = (data&0x02 ? false : true);
	TimerIntrEnable2 = (data&0x04 ? false : true);
	Int1AddrOutput   = (data&0x08 ? false : true);
	Int2AddrOutput   = (data&0x10 ? false : true);
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
inline void INT60::OutB0H( int, BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][OutB0H] %02X\n", data );
	
	// 割込みOff->Onのタイミングでカウンタはリセットされるらしい
	if( !(data & 1) && !TimerIntrEnable ){
		// 初代機は割込み有効後，最初の周期は指定の半分(1ms)になるらしい
		// (推定) 割込みパルスは1/4CLOCK(約1MHz)をカウンタで2048分周したもの。
		//        パルスの立ち上がり(L->H)で割込みが発生するがカウンタの初期値がLのため
		//        初回は半周期で立ち上がってしまう。本来はインバータが必要。
		SetTimerIntrHz(	TimerCntUp, 50 );
	}
	TimerIntrEnable = (data&1 ? false : true);
}


inline void INT62::OutB0H( int, BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][OutB0H] %02X\n", data );
	
	// 割込みOff->Onのタイミングでカウンタはリセットされるらしい
	if( !(data & 1) && !TimerIntrEnable ){
		// mk2以降はちゃんと指定の周期になる...はずだけど若干短くなるようだ。
		// フロッガーで実機と比較しながら調整したら88%くらいだった
		SetTimerIntrHz(	TimerCntUp, 88 );
	}
	TimerIntrEnable  = (data&1 ? false : true);
}

inline void INT62::OutF3H( int, BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][OutF3H] %02X\n", data );
	SetIntrEnable( data );
}

inline void INT62::OutF4H( int, BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][OutF4H] %02X\n", data );
	Int1Addr = data;
}

inline void INT62::OutF5H( int, BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][OutF5H] %02X\n", data );
	Int2Addr = data;
}

inline void INT62::OutF6H( int, BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][OutF6H] %02X\n", data );
	SetTimerIntrHz( data );
}

inline void INT62::OutF7H( int, BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][OutF7H] %02X\n", data );
	TimerAddr = data;
}

inline BYTE INT62::InF3H( int )
{
	return (Int1IntrEnable   ? 0 : 0x01) | (Int2IntrEnable ? 0 : 0x02) |
	       (TimerIntrEnable2 ? 0 : 0x04) | (Int1AddrOutput ? 0 : 0x08) |
		   (Int2AddrOutput   ? 0 : 0x10) | 0xe0;
}

inline BYTE INT62::InF4H( int )
{
	return Int1Addr;
}

inline BYTE INT62::InF5H( int )
{
	return Int2Addr;
}

inline BYTE INT62::InF6H( int )
{
	return TimerCntUp;
}

inline BYTE INT62::InF7H( int )
{
	PRINTD( INTR_LOG, "[INTR][InF7H] %02X\n", TimerAddr );
	return TimerAddr;
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
bool INT6::DokoSave( cIni *Ini )
{
	cSche::evinfo e;
	char stren[16];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	Ini->PutEntry( "INTR", NULL, "IntrFlag",			"0x%08X",	IntrFlag );
	Ini->PutEntry( "INTR", NULL, "TimerIntrEnable",		"%s",		TimerIntrEnable ? "Yes" : "No" );
	Ini->PutEntry( "INTR", NULL, "TimerAddr",			"0x%02X",	TimerAddr );
	Ini->PutEntry( "INTR", NULL, "TimerCntUp",			"%d",		TimerCntUp );
	
	// イベント
	e.id = EID_TIMER;
	if( vm->evsc->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "INTR", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	return true;
}

bool INT62::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "INTR", NULL, "TimerIntrEnable2",	"%s",		TimerIntrEnable2 ? "Yes" : "No" );
	Ini->PutEntry( "INTR", NULL, "Int1IntrEnable",		"%s",		Int1IntrEnable ? "Yes" : "No" );
	Ini->PutEntry( "INTR", NULL, "Int2IntrEnable",		"%s",		Int2IntrEnable ? "Yes" : "No" );
	Ini->PutEntry( "INTR", NULL, "Int1AddrOutput",		"%s",		Int1AddrOutput ? "Yes" : "No" );
	Ini->PutEntry( "INTR", NULL, "Int2AddrOutput",		"%s",		Int2AddrOutput ? "Yes" : "No" );
	Ini->PutEntry( "INTR", NULL, "Int1Addr",			"0x%02X",	Int1Addr );
	Ini->PutEntry( "INTR", NULL, "Int2Addr",			"0x%02X",	Int2Addr );
	
	return INT6::DokoSave( Ini );
}



////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
bool INT6::DokoLoad( cIni *Ini )
{
	int st,yn;
	cSche::evinfo e;
	char stren[16];
	char strrs[64];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	Ini->GetInt(   "INTR", "IntrFlag",			&st,				IntrFlag );			IntrFlag = st;
	Ini->GetTruth( "INTR", "TimerIntrEnable",	&TimerIntrEnable,	TimerIntrEnable );
	Ini->GetInt(   "INTR", "TimerAddr",			&st,				TimerAddr );		TimerAddr = st;
	Ini->GetInt(   "INTR", "TimerCntUp",		&st,				TimerCntUp );		TimerCntUp = st;
	
	// イベント
	e.id = EID_TIMER;
	sprintf( stren, "Event%08X", e.id );
	if( Ini->GetString( "INTR", stren, strrs, "" ) ){
		sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
		e.Active = yn ? true : false;
		if( !vm->evsc->SetEvinfo( &e ) ) return false;
	}
	
	return true;
}

bool INT62::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetTruth( "INTR", "TimerIntrEnable2",	&TimerIntrEnable2,	TimerIntrEnable2 );
	Ini->GetTruth( "INTR", "Int1IntrEnable",	&Int1IntrEnable,	Int1IntrEnable );
	Ini->GetTruth( "INTR", "Int2IntrEnable",	&Int2IntrEnable,	Int2IntrEnable );
	Ini->GetTruth( "INTR", "Int1AddrOutput",	&Int1AddrOutput,	Int1AddrOutput );
	Ini->GetTruth( "INTR", "Int2AddrOutput",	&Int2AddrOutput,	Int2AddrOutput );
	Ini->GetInt(   "INTR", "Int1Addr",			&st,				Int1Addr );		Int1Addr = st;
	Ini->GetInt(   "INTR", "Int2Addr",			&st,				Int2Addr );		Int2Addr = st;
	
	return INT6::DokoLoad( Ini );
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor INT60::descriptor = {
	INT60::indef, INT60::outdef
};

const Device::OutFuncPtr INT60::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &INT60::OutB0H )
};

const Device::InFuncPtr INT60::indef[] = { NULL };

const Device::Descriptor INT62::descriptor = {
	INT62::indef, INT62::outdef
};

const Device::OutFuncPtr INT62::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &INT62::OutB0H ),
	STATIC_CAST( Device::OutFuncPtr, &INT62::OutF3H ),
	STATIC_CAST( Device::OutFuncPtr, &INT62::OutF4H ),
	STATIC_CAST( Device::OutFuncPtr, &INT62::OutF5H ),
	STATIC_CAST( Device::OutFuncPtr, &INT62::OutF6H ),
	STATIC_CAST( Device::OutFuncPtr, &INT62::OutF7H )
};

const Device::InFuncPtr INT62::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &INT62::InF3H ),
	STATIC_CAST( Device::InFuncPtr, &INT62::InF4H ),
	STATIC_CAST( Device::InFuncPtr, &INT62::InF5H ),
	STATIC_CAST( Device::InFuncPtr, &INT62::InF6H ),
	STATIC_CAST( Device::InFuncPtr, &INT62::InF7H )
};
