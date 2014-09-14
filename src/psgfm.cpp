#include "pc6001v.h"
#include "log.h"
#include "osd.h"
#include "psgfm.h"
#include "schedule.h"

#include "p6el.h"
#include "p6vm.h"
// イベントID
#define	EID_PSG		(1)
#define	EID_TIMERA	(2)
#define	EID_TIMERB	(3)

// ポートアクセスウェイトクロック数
#define PAWAIT		(1)


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
PSGb::PSGb( VM6 *vm, const ID& id ) : Device(vm,id), JoyNo(0), Clock(0) {}
PSG6::PSG6( VM6 *vm, const ID& id ) : PSGb(vm,id) {}
OPN6::OPN6( VM6 *vm, const ID& id ) : PSGb(vm,id) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
PSGb::~PSGb( void ){}
PSG6::~PSG6( void ){}
OPN6::~OPN6( void ){}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
void PSGb::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_PSG:
		break;
	}
}


void OPN6::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_PSG:
		break;
	
	case EID_TIMERA:
	case EID_TIMERB:
		cYM2203::TimerIntr();
		break;
	}
}


////////////////////////////////////////////////////////////////
// ポートアクセス関数
////////////////////////////////////////////////////////////////
BYTE PSGb::PortAread( void ){ return vm->KeyGetJoy( JoyNo ); }
void PSGb::PortBwrite( BYTE data ){ JoyNo = (~data>>6)&1; }


////////////////////////////////////////////////////////////////
// 更新サンプル数取得
//   若干の誤差を含むが実用上(多分)問題なし
////////////////////////////////////////////////////////////////
int PSGb::GetUpdateSamples( void )
{
	int samples = (int)( (double)SndDev::SampleRate * vm->EventGetProgress( this->Device::GetID(), EID_PSG ) + 0.5 );
	vm->EventReset( this->Device::GetID(), EID_PSG );
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


void OPN6::PreWriteReg( void )
{
	SoundUpdate( 0 );
}


////////////////////////////////////////////////////////////////
// TimerA設定
////////////////////////////////////////////////////////////////
void OPN6::SetTimerA( int cnt )
{
	double ct = 72. * (1024. - (double)cnt) / (double)Clock * 1000000.;
	
	if( cnt ) vm->EventAdd( this, EID_TIMERA, ct, EV_LOOP|EV_US );
	else	  vm->EventDel( this, EID_TIMERA );
}


////////////////////////////////////////////////////////////////
// TimerB設定
////////////////////////////////////////////////////////////////
void OPN6::SetTimerB( int cnt )
{
	double ct = 1152. * (256. - (double)cnt) / (double)Clock * 1000000.;
	
	if( cnt ) vm->EventAdd( this, EID_TIMERB, ct, EV_LOOP|EV_US );
	else	  vm->EventDel( this, EID_TIMERB );
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool PSG6::Init( int clock, int srate )
{
	PRINTD( PSG_LOG, "[PSG][Init]\n" );
	
	Clock = clock;
	
	// PSGクロック設定
	cAY8910::SetClock( Clock, srate );
	
	// PSG ボリュームテーブル設定
	cAY8910::SetVolumeTable( DEFAULT_PSGVOL );
	
	// リセット
	cAY8910::Reset();
	
	// 少なくとも1秒に1回くらいは更新するだろうという前提
	if( !vm->EventAdd( this, EID_PSG, 1000, EV_LOOP|EV_MS ) ) return false;
	
	return SndDev::Init( srate );
}


bool OPN6::Init( int clock, int srate )
{
	PRINTD( PSG_LOG, "[OPN][Init]\n" );
	
	// 何故かクロック2倍
	Clock = clock * 2;
	
	// OPNクロック設定
	cYM2203::Init( Clock, srate );
	
	// OPN ボリュームテーブル設定
	cYM2203::SetVolumeTable( Volume );
	
	// リセット
	cYM2203::Reset();
	
	// 少なくとも1秒に1回くらいは更新するだろうという前提
	if( !vm->EventAdd( this, EID_PSG, 1000, EV_LOOP|EV_MS ) ) return false;
	
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


bool OPN6::SetSampleRate( int rate, int size )
{
	cYM2203::SetClock( Clock, rate );
	
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
	else					length = SndDev::cRing::FreeSize();
	
	PRINTD( PSG_LOG, " -> %d\n", length );
	
	if( length <= 0 ) return 0;
	
	
	// buffering loop
	for( int i=0; i<length; i++ ){
		// バッファに書込み
		SndDev::cRing::Put( ( this->Update1Sample() * SndDev::Volume ) / 100 );
	}
	
	return length;
}


int OPN6::SoundUpdate( int samples )
{
//	PRINTD( PSG_LOG, "[OPN][SoundUpdate] Samples: %d(%d)", samples, SndDev::cRing::FreeSize() );
	
	int length = 0;
	
	if( samples == 0 ){
		int sam = GetUpdateSamples();
		length = min( sam, SndDev::cRing::FreeSize() );
	}else if( samples > 0 ) length = min( samples, SndDev::cRing::FreeSize() );
	else					length = SndDev::cRing::FreeSize();
	
//	PRINTD( PSG_LOG, " -> %d\n", length );
	
	if( length <= 0 ) return 0;
	
	
	// buffering loop
	for( int i=0; i<length; i++ ){
		// バッファに書込み
		SndDev::cRing::Put( ( this->Update1Sample() * SndDev::Volume ) / 100 );
	}
	
	return length;
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
// PSGレジスタアドレスラッチ
void PSG6::OutA0H( int, BYTE data )
{
	this->WriteReg( 0, data );
}

// PSGライトデータ
void PSG6::OutA1H( int, BYTE data )
{
	this->WriteReg( 1, data );
}

// PSGインアクティブ
void PSG6::OutA3H( int, BYTE data ){}

// PSGリードデータ
BYTE PSG6::InA2H( int )
{
	return this->ReadReg();
}


// OPNレジスタアドレスラッチ
void OPN6::OutA0H( int, BYTE data )
{
	this->WriteReg( 0, data );
}

// OPNライトデータ
void OPN6::OutA1H( int, BYTE data )
{
	this->WriteReg( 1, data );
}

// OPNインアクティブ
void OPN6::OutA3H( int, BYTE data ){}

// OPNリードデータ
BYTE OPN6::InA2H( int )
{
	return this->ReadReg();
}

// OPNステータスリード
BYTE OPN6::InA3H( int )
{
	return this->ReadStatus();
}



////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool PSG6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	// cAY8910
	if( !cAY8910::DokoSave( Ini ) ) return false;
	
	Ini->PutEntry( "PSG", NULL, "BufSize",		"%d",		SndDev::FreeSize() );
	
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
	int st;
	
	if( !Ini ) return false;
	
	// cAY8910
	if( !cAY8910::DokoLoad( Ini ) ) return false;
	
	Ini->GetInt( "PSG", "BufSize",		&st,            SndDev::FreeSize() );
	while( SndDev::FreeSize() > st )
		SndDev::Put(0);
	
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

const Device::Descriptor OPN6::descriptor = {
	OPN6::indef, OPN6::outdef
};

const Device::OutFuncPtr OPN6::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &OPN6::OutA0H ),
	STATIC_CAST( Device::OutFuncPtr, &OPN6::OutA1H ),
	STATIC_CAST( Device::OutFuncPtr, &OPN6::OutA3H )
};

const Device::InFuncPtr OPN6::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &OPN6::InA2H ),
	STATIC_CAST( Device::InFuncPtr, &OPN6::InA3H )
};

