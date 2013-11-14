#include "pc6001v.h"
#include "cpus.h"
#include "intr.h"
#include "log.h"
#include "schedule.h"

#include "p6el.h"
#include "p6vm.h"

// イベントID
#define	EID_TIMER	(1)


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
IRQ6::IRQ6( VM6 *vm, const ID& id ) : Device(vm,id),
	IntrFlag(0), TimerIntrEnable(false), TimerCntUp(0), Timer1st(50)
{
	INITARRAY( IntEnable, false );
	INITARRAY( VecOutput, false );
	INITARRAY( IntVector, 0 );
}

IRQ60::IRQ60( VM6 *vm, const ID& id ) : IRQ6(vm,id)
{
	TimerCntUp   = 3;
	IntEnable[0] = true;
	IntEnable[2] = true;
}

IRQ62::IRQ62( VM6 *vm, const ID& id ) : IRQ6(vm,id)
{
	Timer1st = 88;
}

IRQ64::IRQ64( VM6 *vm, const ID& id ) : IRQ6(vm,id)
{
	Timer1st = 88;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
IRQ6::~IRQ6( void ){}

IRQ60::~IRQ60( void ){}

IRQ62::~IRQ62( void ){}

IRQ64::~IRQ64( void ){}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
////////////////////////////////////////////////////////////////
void IRQ6::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_TIMER:		// タイマ割込み発生
		PRINTD( INTR_LOG, "[INTR][EventCallback] TimerIntr\n" );
		ReqIntr( IREQ_TIMER );
		break;
	}
}


////////////////////////////////////////////////////////////////
// タイマ割込みスイッチ
////////////////////////////////////////////////////////////////
void IRQ6::SetTimerIntr( bool en )
{
	PRINTD( INTR_LOG, "[INTR][SetTimerIntr] %s->%s\n", TimerIntrEnable ? "ON" : "OFF", en ? "ON" : "OFF" );
	
	// 割込みOff->Onのタイミングでカウンタはリセットされるらしい
	if( en && !TimerIntrEnable ){
		// 初代機は割込み有効後，最初の周期は指定の半分(1ms)になるらしい
		// (推定) 割込みパルスは1/4CLOCK(約1MHz)をカウンタで2048分周したもの。
		//        パルスの立ち上がり(L->H)で割込みが発生するがカウンタの初期値がLのため
		//        初回は半周期で立ち上がってしまう。本来はインバータが必要。
		// mk2以降はちゃんと指定の周期になる...はずだけど若干短くなるようだ。
		// フロッガーで実機と比較しながら調整したら88%くらいだった
		SetTimerIntrHz(	TimerCntUp, Timer1st );
	}
	TimerIntrEnable = en;
}


////////////////////////////////////////////////////////////////
// タイマ割込み周波数設定
////////////////////////////////////////////////////////////////
void IRQ6::SetTimerIntrHz( BYTE data, BYTE first )
{
	TimerCntUp = data;
	
	// イベント追加
	vm->EventAdd( this, EID_TIMER, (double)(2048/(vm->VdgIsSRmode() ? 32 : 1))*(TimerCntUp+1), EV_LOOP|EV_STATE );
	
	// 初回周期の指定がある場合の処理
	if( first ){
		EVSC::evinfo e;
		
		e.devid = this->Device::GetID();
		e.id    = EID_TIMER;
		
		vm->EventGetInfo( &e );
		e.Clock = (e.Clock*first)/100;
		vm->EventSetInfo( &e );
	}
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void IRQ60::Reset( void )
{
	PRINTD( INTR_LOG, "[INTR][Reset]\n" );
	
	// 各種変数初期化
	TimerIntrEnable = false;	// タイマ割込み許可フラグ降ろす
	IntrFlag        = 0;		// 割込み要求フラグクリア
	
	// タイマ割込み周波数初期化
	SetTimerIntrHz(	TimerCntUp );

}

void IRQ62::Reset( void )
{
	PRINTD( INTR_LOG, "[INTR][Reset]\n" );
	
	// 各種変数初期化
	TimerCntUp      = 0;		// タイマカウンタ初期化
	TimerIntrEnable = false;	// タイマ割込み許可フラグ降ろす
	IntrFlag        = 0;		// 割込み要求フラグクリア
	
	for( int i=0; i<2; i++ ){
		IntEnable[i] = false;	// 割込み許可フラグ降ろす
		VecOutput[i] = false;	// 割込みアドレス出力フラグ降ろす
		IntVector[i] = 0;		// 割込みアドレス初期化
	}
	
	// タイマ割込み周波数初期化
	SetTimerIntrHz(	TimerCntUp );
}

void IRQ64::Reset( void )
{
	PRINTD( INTR_LOG, "[INTR][Reset]\n" );
	
	// 各種変数初期化
	TimerCntUp      = 0;		// タイマカウンタ初期化
	TimerIntrEnable = false;	// タイマ割込み許可フラグ降ろす
	IntrFlag        = 0;		// 割込み要求フラグクリア
	
	for( int i=0; i<8; i++ ){
		IntEnable[i] = false;	// 割込み許可フラグ降ろす
		VecOutput[i] = false;	// 割込みアドレス出力フラグ降ろす
		IntVector[i] = 0;		// 割込みアドレス初期化
	}
	
	// タイマ割込み周波数初期化
	SetTimerIntrHz(	TimerCntUp );
}

////////////////////////////////////////////////////////////////
// 割込みチェック
////////////////////////////////////////////////////////////////
//   割込み発生源はタイマか8049で,タイマ優先か?
//   と思ったけどSRの調査をしていたら違うような気がしてきた。
//   mk2のINT1,INT2がずっと謎だったのだがこう考えればよいのか?
//    INT1 = 8049割込み
//    INT2 = ジョイスティック割込み
//    INT3 = タイマ割込み
//   とすると8049割込みを再優先にするのが正しいことになる
//   ジョイスティック割込みはジョイスティックポートの7pinをトリガにした外部割込み
//   8049を介したゲーム用キー割込みとは別物

// INT1: SUB CPU
// INT2: JOYSTICK
// INT3: TIMER
// INT4: VOICE
// INT5: VRTC
// IRQ6: RS-232C
// INT7: PRINTER
// INT8: EXT INT

int IRQ6::IntrCheck( void )
{
	int	IntrNo = -1;
	
	PRINTD( INTR_LOG, "[INTR][IntrCheck]" );
	
	// INT1:8049割込み
	if( ( IntrFlag & IREQ_8049 ) && IntEnable[0] ){
		PRINTD( INTR_LOG, "(8049)" );
		
		CancelIntr( IREQ_8049 );
		IntrNo = ( VecOutput[0] ? IntVector[0] : vm->PioReadA() )>>1;
	}
	// INT1:ジョイスティック割込み(7pin)
	else if( ( IntrFlag & IREQ_JOYSTK ) && IntEnable[1] ){
		PRINTD( INTR_LOG, "(JoyStick)" );
		
		CancelIntr( IREQ_JOYSTK );
		IntrNo = ( VecOutput[1] ? IntVector[1] : 0x00 )>>1;	// アドレス不明(とりあえず0x00)
	}
	// INT3:タイマ割込み
	else if( ( IntrFlag & IREQ_TIMER ) && IntEnable[2] && TimerIntrEnable ){
		PRINTD( INTR_LOG, "(Timer)" );
		
		CancelIntr( IREQ_TIMER );
		IntrNo = ( VecOutput[2] ? IntVector[2] : 0x06 )>>1;	// アドレス0x06固定?
	}
	// INT4:音声合成割込み
	else if( ( IntrFlag & IREQ_VOICE ) && IntEnable[3] ){
		PRINTD( INTR_LOG, "(Voice)" );
		
		CancelIntr( IREQ_VOICE );
		IntrNo = ( VecOutput[3] ? IntVector[3] : 0x20 )>>1;	// アドレス不明(とりあえず0x20)
	}
	// INT5:VRTC割込み(SRモードの時だけ発生)
	else if( ( IntrFlag & IREQ_VRTC ) && IntEnable[4] && vm->VdgIsSRmode() ){
		PRINTD( INTR_LOG, "(VRTC)" );
		
		CancelIntr( IREQ_VRTC );
		IntrNo = ( VecOutput[4] ? IntVector[4] : 0x22 )>>1;	// アドレス不明(とりあえず0x22)
	}
	// IRQ6:RS-232C割込み
	else if( ( IntrFlag & IREQ_SIO ) && IntEnable[5] ){
		PRINTD( INTR_LOG, "(RS-232C)" );
		
		CancelIntr( IREQ_SIO );
		IntrNo = ( VecOutput[5] ? IntVector[5] : 0x04 )>>1;	// アドレス0x04固定？
	}
	// INT7:プリンタ割込み
	else if( ( IntrFlag & IREQ_PRINT ) && IntEnable[6] ){
		PRINTD( INTR_LOG, "(Printer)" );
		
		CancelIntr( IREQ_PRINT );
		IntrNo = ( VecOutput[6] ? IntVector[6] : 0x00 )>>1;	// アドレス不明(とりあえず0x00)
	}
	// INT8:外部割込み
	else if( ( IntrFlag & IREQ_EXTINT ) && IntEnable[7] ){
		PRINTD( INTR_LOG, "(Ext)" );
		
		CancelIntr( IREQ_EXTINT );
		IntrNo = ( VecOutput[7] ? IntVector[7] : 0x00 )>>1;	// アドレス不明(とりあえず0x00)
	}
	
	
	if( IntrNo >= 0 ){
		PRINTD( INTR_LOG, " IntrNo = %d", IntrNo  );
		IntrNo <<= 1;
	}
	PRINTD( INTR_LOG, "\n" );
	
	return IntrNo;
}


////////////////////////////////////////////////////////////////
// 割込み要求
////////////////////////////////////////////////////////////////
void IRQ6::ReqIntr( DWORD vec )
{
	IntrFlag |= vec;
	
	PRINTD( INTR_LOG, "[INTR][ReqIntr] %04X -> %04X\n", (unsigned int)vec, (unsigned int)IntrFlag );
}


////////////////////////////////////////////////////////////////
// 割込み撤回
////////////////////////////////////////////////////////////////
void IRQ6::CancelIntr( DWORD vec )
{
	IntrFlag &= ~vec;
	
	PRINTD( INTR_LOG, "[INTR][CancelIntr] %04X -> %04X\n", (unsigned int)vec, (unsigned int)IntrFlag );
}


////////////////////////////////////////////////////////////////
// 割込み許可フラグ設定
////////////////////////////////////////////////////////////////
void IRQ6::SetIntrEnable( BYTE data ){}

void IRQ62::SetIntrEnable( BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][SetIntrEnable]\n" );
	
	IntEnable[0] = (data&0x01 ? false : true);
	IntEnable[1] = (data&0x02 ? false : true);
	IntEnable[2] = (data&0x04 ? false : true);
	VecOutput[0] = (data&0x08 ? true : false);
	VecOutput[1] = (data&0x10 ? true : false);
	
	PRINTD( INTR_LOG, "\tINT1:SUB CPU  %s\n", IntEnable[0] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT2:JOYSTICK %s\n", IntEnable[1] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT3:TIMER    %s\n", IntEnable[2] ? "Enable" : "Disable" );
}

void IRQ64::SetIntrEnable( BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][SetIntrEnable]\n" );
	
	for( int i=0; i<8; i++ )
		IntEnable[i] = (data>>i)&1 ? false : true;
	PRINTD( INTR_LOG, "\tINT1:SUB CPU  %s\n", IntEnable[0] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT2:JOYSTICK %s\n", IntEnable[1] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT3:TIMER    %s\n", IntEnable[2] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT4:VOICE    %s\n", IntEnable[3] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT5:VRTC     %s\n", IntEnable[4] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tIRQ6:RS-232C  %s\n", IntEnable[5] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT7:PRINTER  %s\n", IntEnable[6] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT8:EXT INT  %s\n", IntEnable[7] ? "Enable" : "Disable" );
}


////////////////////////////////////////////////////////////////
// 割込みベクタアドレス出力フラグ設定
////////////////////////////////////////////////////////////////
void IRQ64::SetIntrVectorEnable( BYTE data )
{
	PRINTD( INTR_LOG, "[INTR][SetIntrVectorEnable]\n" );
	
	for( int i=0; i<8; i++ )
		VecOutput[i] = (data>>i)&1 ? true : false;
	
	PRINTD( INTR_LOG, "\tINT1:SUB CPU  %s\n", VecOutput[0] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT2:JOYSTICK %s\n", VecOutput[1] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT3:TIMER    %s\n", VecOutput[2] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT4:VOICE    %s\n", VecOutput[3] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT5:VRTC     %s\n", VecOutput[4] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tIRQ6:RS-232C  %s\n", VecOutput[5] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT7:PRINTER  %s\n", VecOutput[6] ? "Enable" : "Disable" );
	PRINTD( INTR_LOG, "\tINT8:EXT INT  %s\n", VecOutput[7] ? "Enable" : "Disable" );
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
void IRQ6::OutB0H( int, BYTE data ){ SetTimerIntr( data&1 ? false : true ); }
void IRQ6::OutF3H( int, BYTE data ){ SetIntrEnable( data ); }
void IRQ6::OutF4H( int, BYTE data ){ IntVector[0] = data; }
void IRQ6::OutF5H( int, BYTE data ){ IntVector[1] = data; }
void IRQ6::OutF6H( int, BYTE data ){ SetTimerIntrHz( data ); }
void IRQ6::OutF7H( int, BYTE data ){ IntVector[2] = data; }

BYTE IRQ6::InF3H( int )
{
	return (IntEnable[0] ? 0 : 0x01) | (IntEnable[1] ? 0 : 0x02) |
	       (IntEnable[2] ? 0 : 0x04) | (VecOutput[0] ? 0 : 0x08) |
		   (VecOutput[1] ? 0 : 0x10) | 0xe0;
}
BYTE IRQ6::InF4H( int ){ return IntVector[0]; }
BYTE IRQ6::InF5H( int ){ return IntVector[1]; }
BYTE IRQ6::InF6H( int ){ return TimerCntUp; }
BYTE IRQ6::InF7H( int ){ return IntVector[2]; }

void IRQ64::OutBxH( int port, BYTE data ){ IntVector[port&7] = data; }
void IRQ64::OutFAH( int, BYTE data ){ SetIntrEnable( data ); }
void IRQ64::OutFBH( int, BYTE data ){ SetIntrVectorEnable( data ); }

BYTE IRQ64::InFAH( int )
{
	return (IntEnable[0] ? 0 : 0x01) | (IntEnable[1] ? 0 : 0x02) |
		   (IntEnable[2] ? 0 : 0x04) | (IntEnable[3] ? 0 : 0x08) |
		   (IntEnable[4] ? 0 : 0x10) | (IntEnable[5] ? 0 : 0x20) |
		   (IntEnable[6] ? 0 : 0x40) | (IntEnable[7] ? 0 : 0x80);
}
BYTE IRQ64::InFBH( int )
{
	return (VecOutput[0] ? 0x01 : 0) | (VecOutput[1] ? 0x02 : 0) |
		   (VecOutput[2] ? 0x04 : 0) | (VecOutput[3] ? 0x08 : 0) |
		   (VecOutput[4] ? 0x10 : 0) | (VecOutput[5] ? 0x20 : 0) |
		   (VecOutput[6] ? 0x40 : 0) | (VecOutput[7] ? 0x80 : 0);
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
bool IRQ6::DokoSave( cIni *Ini )
{
	char stren[16];
	
	if( !Ini ) return false;
	
	Ini->PutEntry( "INTR", NULL, "IntrFlag",			"0x%08X",	IntrFlag );
	Ini->PutEntry( "INTR", NULL, "TimerIntrEnable",		"%s",		TimerIntrEnable  ? "Yes" : "No" );
	Ini->PutEntry( "INTR", NULL, "TimerCntUp",			"%d",		TimerCntUp );
	
	for( int i=0; i<8; i++ ){
		sprintf( stren, "IntEnable%d", i );
		Ini->PutEntry( "INTR", NULL, stren,		"%s",		IntEnable[i]  ? "Yes" : "No" );
	}
	for( int i=0; i<8; i++ ){
		sprintf( stren, "VecOutput%d", i );
		Ini->PutEntry( "INTR", NULL, stren,		"%s",		VecOutput[i]  ? "Yes" : "No" );
	}
	for( int i=0; i<8; i++ ){
		sprintf( stren, "IntVector%d", i );
		Ini->PutEntry( "INTR", NULL, stren,		"0x%02X",	IntVector[i] );
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
bool IRQ6::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt(   "INTR", "IntrFlag",			&st,				IntrFlag );			IntrFlag = st;
	Ini->GetTruth( "INTR", "TimerIntrEnable",	&TimerIntrEnable,	TimerIntrEnable );
	Ini->GetInt(   "INTR", "TimerCntUp",		&st,				TimerCntUp );		TimerCntUp   = st;
	
	char stren[16];
	
	for( int i=0; i<8; i++ ){
		sprintf( stren, "IntEnable%d", i );
		Ini->GetTruth( "INTR", stren,		&IntEnable[i],	IntEnable[i] );
	}
	for( int i=0; i<8; i++ ){
		sprintf( stren, "VecOutput%d", i );
		Ini->GetTruth( "INTR", stren,		&VecOutput[i],	VecOutput[i] );
	}
	for( int i=0; i<8; i++ ){
		sprintf( stren, "IntVector%d", i );
		Ini->GetInt(   "INTR", stren,		&st,			IntVector[i] );		IntVector[i] = st;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor IRQ60::descriptor = {
	IRQ60::indef, IRQ60::outdef
};

const Device::OutFuncPtr IRQ60::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &IRQ60::OutB0H )
};

const Device::InFuncPtr IRQ60::indef[] = { NULL };

const Device::Descriptor IRQ62::descriptor = {
	IRQ62::indef, IRQ62::outdef
};

const Device::OutFuncPtr IRQ62::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &IRQ62::OutB0H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ62::OutF3H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ62::OutF4H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ62::OutF5H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ62::OutF6H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ62::OutF7H )
};

const Device::InFuncPtr IRQ62::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &IRQ62::InF3H ),
	STATIC_CAST( Device::InFuncPtr, &IRQ62::InF4H ),
	STATIC_CAST( Device::InFuncPtr, &IRQ62::InF5H ),
	STATIC_CAST( Device::InFuncPtr, &IRQ62::InF6H ),
	STATIC_CAST( Device::InFuncPtr, &IRQ62::InF7H )
};

const Device::Descriptor IRQ64::descriptor = {
	IRQ64::indef, IRQ64::outdef
};

const Device::OutFuncPtr IRQ64::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &IRQ64::OutB0H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ64::OutBxH ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ64::OutF3H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ64::OutF4H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ64::OutF5H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ64::OutF6H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ64::OutF7H ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ64::OutFAH ),
	STATIC_CAST( Device::OutFuncPtr, &IRQ64::OutFBH )
};

const Device::InFuncPtr IRQ64::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &IRQ64::InF3H ),
	STATIC_CAST( Device::InFuncPtr, &IRQ64::InF4H ),
	STATIC_CAST( Device::InFuncPtr, &IRQ64::InF5H ),
	STATIC_CAST( Device::InFuncPtr, &IRQ64::InF6H ),
	STATIC_CAST( Device::InFuncPtr, &IRQ64::InF7H ),
	STATIC_CAST( Device::InFuncPtr, &IRQ64::InFAH ),
	STATIC_CAST( Device::InFuncPtr, &IRQ64::InFBH )
};
