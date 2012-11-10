#include "pc6001v.h"
#include "p6el.h"
#include "log.h"
#include "psg.h"
#include "keyboard.h"
#include "schedule.h"

// イベントID
#define	EID_PSG		(1)

// ポートアクセスウェイトクロック数
#define PAWAIT		(1)


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
PSG6::PSG6( VM6 *vm, const ID& id ) : P6DEVICE(vm,id), Device(id), JoyNo(0), Clock(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
PSG6::~PSG6( void ){}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
void PSG6::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_PSG:
		break;
	}
}


////////////////////////////////////////////////////////////////
// ポートアクセス関数
////////////////////////////////////////////////////////////////
BYTE PSG6::PortAread( void ){ return vm->key->GetJoy( JoyNo ); }
void PSG6::PortBwrite( BYTE data ){ JoyNo = (~data>>6)&1; }


////////////////////////////////////////////////////////////////
// 更新サンプル数取得
//   若干の誤差を含むが実用上(多分)問題なし
////////////////////////////////////////////////////////////////
int PSG6::GetUpdateSamples( void )
{
	int samples = (int)( (double)SndDev::SampleRate * vm->evsc->Scale( this, EID_PSG ) + 0.5 );
	vm->evsc->Reset( this, EID_PSG );
	return samples;
}


////////////////////////////////////////////////////////////////
// レジスタ変更前のストリーム更新
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void PSG6::PreWriteReg( void )
{
	SoundUpdate( 0 );
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool PSG6::Init( int clock, int srate )
{
	PRINTD( PSG_LOG, "[PSG][Init]\n" );
	
	// PSGクロック設定
	cAY8910::SetClock( clock, srate );
	
	// PSG ボリュームテーブル設定
	cAY8910::SetVolumeTable( DEFAULT_PSGVOL );
	
	// リセット
	cAY8910::Reset();
	
	// 少なくとも1秒に1回くらいは更新するだろうという前提
	if( !vm->evsc->Add( this, EID_PSG, 1000, EV_LOOP|EV_MS ) ) return false;
	
	Clock = clock;
	
	return SndDev::Init( srate );
}


////////////////////////////////////////////////////////////////
// サンプリングレート設定
//
// 引数:	rate	サンプリングレート
//			size	バッファサイズ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool PSG6::SetSampleRate( int rate, int size )
{
	cAY8910::SetClock( Clock, rate );
	
	return SndDev::SetSampleRate( rate, size );
}


////////////////////////////////////////////////////////////////
// ストリーム更新
//
// 引数:	samples	更新サンプル数(-1:残りバッファ全て 0:処理クロック分)
// 返値:	int		更新サンプル数
////////////////////////////////////////////////////////////////
int PSG6::SoundUpdate( int samples )
{
	PRINTD( PSG_LOG, "[PSG][SoundUpdate] Samples: %d(%d)", samples, SndDev::cRing::FreeSize() );
	
	int length = 0;
	
	if( samples == 0 ){
		int sam = GetUpdateSamples();
		length = min( sam, SndDev::cRing::FreeSize() );
	}else if( samples > 0 ) length = min( samples, SndDev::cRing::FreeSize() );
	else                   length = SndDev::cRing::FreeSize();
	
	PRINTD( PSG_LOG, " -> %d\n", length );
	
	if( !length ) return 0;
	
	
	// buffering loop
	for( int i=0; i<length; i++ ){
		// バッファに書込み
		SndDev::cRing::Put( cAY8910::Update1Sample() );
	}
	
	return length;
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
// PSGレジスタアドレスラッチ
inline void PSG6::OutA0H( int, BYTE data )
{
	PRINTD( PSG_LOG, "[PSG][RegisterLatch] -> %d\n", data );
	
	cAY8910::RegisterLatch = data & 0x0f;
}

// PSGライトデータ
inline void PSG6::OutA1H( int, BYTE data )
{
	cAY8910::AYWriteReg( cAY8910::RegisterLatch, data );
}

// PSGインアクティブ
inline void PSG6::OutA3H( int, BYTE data ){}

// PSGリードデータ
inline BYTE PSG6::InA2H( int )
{
	return cAY8910::AYReadReg( cAY8910::RegisterLatch );
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool PSG6::DokoSave( cIni *Ini )
{
	cSche::evinfo e;
	char stren[16];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	// cAY8910
	Ini->PutEntry( "PSG", NULL, "RegisterLatch",	"%d",	RegisterLatch );
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "Regs_%02d", i );
		Ini->PutEntry( "PSG", NULL, stren, "0x%02X", Regs[i] );
	}
	Ini->PutEntry( "PSG", NULL, "LastEnable",	"%d",		LastEnable );
	Ini->PutEntry( "PSG", NULL, "PeriodA",		"%d",		PeriodA );
	Ini->PutEntry( "PSG", NULL, "PeriodB",		"%d",		PeriodB );
	Ini->PutEntry( "PSG", NULL, "PeriodC",		"%d",		PeriodC );
	Ini->PutEntry( "PSG", NULL, "PeriodN",		"%d",		PeriodN );
	Ini->PutEntry( "PSG", NULL, "PeriodE",		"%d",		PeriodE );
	Ini->PutEntry( "PSG", NULL, "CountA",		"%d",		CountA );
	Ini->PutEntry( "PSG", NULL, "CountB",		"%d",		CountB );
	Ini->PutEntry( "PSG", NULL, "CountC",		"%d",		CountC );
	Ini->PutEntry( "PSG", NULL, "CountN",		"%d",		CountN );
	Ini->PutEntry( "PSG", NULL, "CountE",		"%d",		CountE );
	Ini->PutEntry( "PSG", NULL, "VolA",			"%d",		VolA );
	Ini->PutEntry( "PSG", NULL, "VolB",			"%d",		VolB );
	Ini->PutEntry( "PSG", NULL, "VolC",			"%d",		VolC );
	Ini->PutEntry( "PSG", NULL, "VolE",			"%d",		VolE );
	Ini->PutEntry( "PSG", NULL, "EnvelopeA",	"0x%02X",	EnvelopeA );
	Ini->PutEntry( "PSG", NULL, "EnvelopeB",	"0x%02X",	EnvelopeB );
	Ini->PutEntry( "PSG", NULL, "EnvelopeC",	"0x%02X",	EnvelopeC );
	Ini->PutEntry( "PSG", NULL, "OutputA",		"0x%02X",	OutputA );
	Ini->PutEntry( "PSG", NULL, "OutputB",		"0x%02X",	OutputB );
	Ini->PutEntry( "PSG", NULL, "OutputC",		"0x%02X",	OutputC );
	Ini->PutEntry( "PSG", NULL, "OutputN",		"0x%02X",	OutputN );
	Ini->PutEntry( "PSG", NULL, "CountEnv",		"%d",		CountEnv );
	Ini->PutEntry( "PSG", NULL, "Hold",			"0x%02X",	Hold );
	Ini->PutEntry( "PSG", NULL, "Alternate",	"0x%02X",	Alternate );
	Ini->PutEntry( "PSG", NULL, "Attack",		"0x%02X",	Attack );
	Ini->PutEntry( "PSG", NULL, "Holding",		"0x%02X",	Holding );
	Ini->PutEntry( "PSG", NULL, "RNG",			"%d",		RNG );
	
	// イベント
	e.id = EID_PSG;
	if( vm->evsc->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "PSG", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool PSG6::DokoLoad( cIni *Ini )
{
	int st,yn;
	cSche::evinfo e;
	char stren[16];
	char strrs[64];
	
	e.device = this;
	
	if( !Ini ) return false;
	
	// cAY8910
	Ini->GetInt( "PSG", "RegisterLatch", &RegisterLatch, RegisterLatch );
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "Regs_%02d", i );
		Ini->GetInt( "PSG", stren, &st, Regs[i] );	Regs[i] = st;
	}
	Ini->GetInt( "PSG", "LastEnable",	&LastEnable,	LastEnable );
	Ini->GetInt( "PSG", "PeriodA",		&PeriodA,		PeriodA );
	Ini->GetInt( "PSG", "PeriodB",		&PeriodB,		PeriodB );
	Ini->GetInt( "PSG", "PeriodC",		&PeriodC,		PeriodC );
	Ini->GetInt( "PSG", "PeriodN",		&PeriodN,		PeriodN );
	Ini->GetInt( "PSG", "PeriodE",		&PeriodE,		PeriodE );
	Ini->GetInt( "PSG", "CountA",		&CountA,		CountA );
	Ini->GetInt( "PSG", "CountB",		&CountB,		CountB );
	Ini->GetInt( "PSG", "CountC",		&CountC,		CountC );
	Ini->GetInt( "PSG", "CountN",		&CountN,		CountN );
	Ini->GetInt( "PSG", "CountE",		&CountE,		CountE );
	Ini->GetInt( "PSG", "VolA",			&VolA,			VolA );
	Ini->GetInt( "PSG", "VolB",			&VolB,			VolB );
	Ini->GetInt( "PSG", "VolC",			&VolC,			VolC );
	Ini->GetInt( "PSG", "VolE",			&VolE,			VolE );
	Ini->GetInt( "PSG", "EnvelopeA",	&st,			EnvelopeA );	EnvelopeA = st;
	Ini->GetInt( "PSG", "EnvelopeB",	&st,			EnvelopeB );	EnvelopeB = st;
	Ini->GetInt( "PSG", "EnvelopeC",	&st,			EnvelopeC );	EnvelopeC = st;
	Ini->GetInt( "PSG", "OutputA",		&st,			OutputA );		OutputA = st;
	Ini->GetInt( "PSG", "OutputB",		&st,			OutputB );		OutputB = st;
	Ini->GetInt( "PSG", "OutputC",		&st,			OutputC );		OutputC = st;
	Ini->GetInt( "PSG", "OutputN",		&st,			OutputN );		OutputN = st;
	Ini->GetInt( "PSG", "CountEnv",		&st,			CountEnv );		CountEnv = st;
	Ini->GetInt( "PSG", "Hold",			&st,			Hold );			Hold = st;
	Ini->GetInt( "PSG", "Alternate",	&st,			Alternate );	Alternate = st;
	Ini->GetInt( "PSG", "Attack",		&st,			Attack );		Attack = st;
	Ini->GetInt( "PSG", "Holding",		&st,			Holding );		Holding = st;
	Ini->GetInt( "PSG", "RNG",			&RNG,			RNG );
	
	// イベント
	e.id = EID_PSG;
	sprintf( stren, "Event%08X", e.id );
	if( Ini->GetString( "PSG", stren, strrs, "" ) ){
		sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
		e.Active = yn ? true : false;
		if( !vm->evsc->SetEvinfo( &e ) ) return false;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor PSG6::descriptor = {
	PSG6::indef, PSG6::outdef
};

const Device::OutFuncPtr PSG6::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &PSG6::OutA0H ),
	STATIC_CAST( Device::OutFuncPtr, &PSG6::OutA1H ),
	STATIC_CAST( Device::OutFuncPtr, &PSG6::OutA3H )
};

const Device::InFuncPtr PSG6::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &PSG6::InA2H )
};

