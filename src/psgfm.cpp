/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "pc6001v.h"

#include "common.h"
#include "log.h"
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


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
PSGb::PSGb( VM6* vm, const ID& id ) : Device( vm, id ), JoyNo( 0 ), Clock( 0 ), Smpls( 0 )
{
}

PSG60::PSG60( VM6* vm, const ID& id ) : PSGb( vm, id )
{
	// Device Description (Out)
	descs.outdef.emplace( outA0H, STATIC_CAST( Device::OutFuncPtr, &PSG60::OutA0H ) );
	descs.outdef.emplace( outA1H, STATIC_CAST( Device::OutFuncPtr, &PSG60::OutA1H ) );
	descs.outdef.emplace( outA3H, STATIC_CAST( Device::OutFuncPtr, &PSG60::OutA3H ) );
	
	// Device Description (In)
	descs.indef.emplace ( inA2H,  STATIC_CAST( Device::InFuncPtr,  &PSG60::InA2H  ) );
}

OPN64::OPN64( VM6* vm, const ID& id ) : PSGb( vm, id )
{
	// Device Description (Out)
	descs.outdef.emplace( outA0H, STATIC_CAST( Device::OutFuncPtr, &OPN64::OutA0H ) );
	descs.outdef.emplace( outA1H, STATIC_CAST( Device::OutFuncPtr, &OPN64::OutA1H ) );
	descs.outdef.emplace( outA3H, STATIC_CAST( Device::OutFuncPtr, &OPN64::OutA3H ) );
	
	// Device Description (In)
	descs.indef.emplace ( inA2H,  STATIC_CAST( Device::InFuncPtr,  &OPN64::InA2H  ) );
	descs.indef.emplace ( inA3H,  STATIC_CAST( Device::InFuncPtr,  &OPN64::InA3H  ) );
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
PSGb::~PSGb( void )
{
}

PSG60::~PSG60( void )
{
}

OPN64::~OPN64( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void PSG60::EventCallback( int id, int clock )
{
	std::lock_guard<cMutex> lock( Mutex );
	
	switch( id ){
	case EID_PSG:
		Smpls -= SndDev::SampleRate;
		break;
	}
}

void OPN64::EventCallback( int id, int clock )
{
	std::lock_guard<cMutex> lock( Mutex );
	
	switch( id ){
	case EID_PSG:
		Smpls -= SndDev::SampleRate;
		break;
	
	case EID_TIMERA:
	case EID_TIMERB:
		cYM2203::TimerIntr();
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 更新サンプル数取得
//   若干の誤差を含むが実用上(多分)問題なし
/////////////////////////////////////////////////////////////////////////////
int PSGb::GetUpdateSamples( void )
{
	std::lock_guard<cMutex> lock( Mutex );
	
	int prgsample = (int)( (double)SndDev::SampleRate * vm->EventGetProgress( this->Device::GetID(), EID_PSG ) + 0.5 );
	int samples = prgsample - Smpls;
	Smpls = prgsample;
	
	return samples;
}


/////////////////////////////////////////////////////////////////////////////
// レジスタ変更前のストリーム更新
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void PSG60::PreWriteReg( void )
{
	SoundUpdate( 0 );
}


void OPN64::PreWriteReg( void )
{
	SoundUpdate( 0 );
}


/////////////////////////////////////////////////////////////////////////////
// TimerA設定
/////////////////////////////////////////////////////////////////////////////
void OPN64::SetTimerA( int cnt )
{
	double ct = 72. * (1024. - (double)cnt) / (double)Clock * 1000000.;
	
	if( cnt ){ vm->EventAdd( Device::GetID(), EID_TIMERA, ct, EV_LOOP | EV_US ); }
	else	 { vm->EventDel( Device::GetID(), EID_TIMERA ); }
}


/////////////////////////////////////////////////////////////////////////////
// TimerB設定
/////////////////////////////////////////////////////////////////////////////
void OPN64::SetTimerB( int cnt )
{
	double ct = 1152. * (256. - (double)cnt) / (double)Clock * 1000000.;
	
	if( cnt ){ vm->EventAdd( Device::GetID(), EID_TIMERB, ct, EV_LOOP | EV_US ); }
	else	 { vm->EventDel( Device::GetID(), EID_TIMERB ); }
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool PSG60::Init( int clock, int srate )
{
	PRINTD( PSG_LOG, "[PSG][Init]\n" );
	
	Clock = clock;
	
	// PSG/OPN クロック設定＆リセット
	InitMod( Clock, srate );
	
	// PSG/OPN ボリュームテーブル設定
	SetVolumeTable( DEFAULT_PSGVOL );
	
	// 1サイクル=サンプリングレート相当のサンプル数
	if( !vm->EventAdd( Device::GetID(), EID_PSG, 1000, EV_LOOP | EV_MS ) ){
		return false;
	}
	
	return true;
}


bool OPN64::Init( int clock, int srate )
{
	PRINTD( PSG_LOG, "[OPN][Init]\n" );
	
	Clock = clock;
	
	// PSG/OPN クロック設定＆リセット
	InitMod( Clock, srate );
	
	// PSG/OPN ボリュームテーブル設定
	SetVolumeTable( DEFAULT_PSGVOL );
	
	// 1サイクル=サンプリングレート相当のサンプル数
	if( !vm->EventAdd( Device::GetID(), EID_PSG, 1000, EV_LOOP | EV_MS ) ){
		return false;
	}
	
	return true;
}



/////////////////////////////////////////////////////////////////////////////
// サンプリングレート設定
//
// 引数:	rate	サンプリングレート
//			size	バッファサイズ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool PSG60::SetSampleRate( int rate, int size )
{
	cAY8910::SetClock( Clock, rate );
	
	return SndDev::SetSampleRate( rate, size );
}


bool OPN64::SetSampleRate( int rate, int size )
{
	cYM2203::SetClock( Clock, rate );
	
	return SndDev::SetSampleRate( rate, size );
}


/////////////////////////////////////////////////////////////////////////////
// ストリーム更新
//
// 引数:	samples	更新するサンプル数(0:処理クロック分)
// 返値:	int		残りサンプル数
/////////////////////////////////////////////////////////////////////////////
int PSG60::SoundUpdate( int samples )
{
	int length = max( 0, samples ? samples : GetUpdateSamples() );
	
	PRINTD( PSG_LOG, "[PSG][SoundUpdate] Samples: %d -> %d\n", samples, length );
	
	// バッファに書込み
	for( int i = 0; i < length; i++ ){
		SndDev::Put( this->Update1Sample() );
	}
	
	return SndDev::cRing::ReadySize();
}

int OPN64::SoundUpdate( int samples )
{
	int length = max( 0, samples ? samples : GetUpdateSamples() );
	
	PRINTD( PSG_LOG, "[OPN][SoundUpdate] Samples: %d -> %d\n", samples, length );
	
	// バッファに書込み
	for( int i = 0; i < length; i++ ){
		SndDev::Put( this->Update1Sample() );
	}
	
	return SndDev::cRing::ReadySize();
}


/////////////////////////////////////////////////////////////////////////////
// ポートアクセス関数
/////////////////////////////////////////////////////////////////////////////
BYTE PSG60::PortAread( void ){ return vm->KeyGetJoy( JoyNo ); }
void PSG60::PortBwrite( BYTE data ){ JoyNo = (~data >> 6) & 1; }

BYTE OPN64::PortAread( void ){ return vm->KeyGetJoy( JoyNo ); }
void OPN64::PortBwrite( BYTE data ){ JoyNo = (~data >> 6) & 1; }


/////////////////////////////////////////////////////////////////////////////
// I/Oアクセス関数
/////////////////////////////////////////////////////////////////////////////
// PSGレジスタアドレスラッチ
void PSG60::OutA0H( int, BYTE data )
{
	this->WriteReg( 0, data );
}

// PSGライトデータ
void PSG60::OutA1H( int, BYTE data )
{
	this->WriteReg( 1, data );
}

// PSGインアクティブ
void PSG60::OutA3H( int, BYTE data ){}

// PSGリードデータ
BYTE PSG60::InA2H( int )
{
	return this->ReadReg();
}


// OPNレジスタアドレスラッチ
void OPN64::OutA0H( int, BYTE data )
{
	this->WriteReg( 0, data );
}

// OPNライトデータ
void OPN64::OutA1H( int, BYTE data )
{
	this->WriteReg( 1, data );
}

// OPNインアクティブ
void OPN64::OutA3H( int, BYTE data ){}

// OPNリードデータ
BYTE OPN64::InA2H( int )
{
	return this->ReadReg();
}

// OPNステータスリード
BYTE OPN64::InA3H( int )
{
	return this->ReadStatus();
}



/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool PSG60::DokoSave( cIni* Ini )
{
	if( !Ini ){
		return false;
	}
	
	Ini->SetVal( "PSG", "RegisterLatch",	"", "0x%02X", RegisterLatch );
	Ini->SetVal( "PSG", "LastEnable",		"", "0x%02X", LastEnable    );

#ifdef USEFMGEN
	for( int i = 0; i < 16; i++ ){
		Ini->SetVal( "PSG", Stringf( "Regs_%02d", i ), "", "0x%02X", reg[i] );
	}
	
	Ini->SetVal( "PSG", "envelop", "", (BYTE)((envelop - (const uint*)enveloptable) / 64) );
	
	for( int i = 0; i < 3; i++ ){
		Ini->SetVal( "PSG", Stringf( "olevel_%02d",  i ), "", "0x%02X", olevel[i]  );
		Ini->SetVal( "PSG", Stringf( "scount_%02d",  i ), "", "0x%08X", scount[i]  );
		Ini->SetVal( "PSG", Stringf( "speriod_%02d", i ), "", "0x%08X", speriod[i] );
	}
	
	Ini->SetVal( "PSG", "ecount",		"", "0x%08X", ecount      );
	Ini->SetVal( "PSG", "eperiod",		"", "0x%08X", eperiod     );
	Ini->SetVal( "PSG", "ncount",		"", "0x%08X", ncount      );
	Ini->SetVal( "PSG", "nperiod",		"", "0x%08X", nperiod     );
	Ini->SetVal( "PSG", "tperiodbase",	"", "0x%08X", tperiodbase );
	Ini->SetVal( "PSG", "eperiodbase",	"", "0x%08X", eperiodbase );
	Ini->SetVal( "PSG", "nperiodbase",	"", "0x%08X", nperiodbase );
	Ini->SetVal( "PSG", "mask",			"",           mask        );
#else
	for( int i = 0; i < 16; i++ ){
		Ini->SetVal( "PSG", Stringf( "Regs_%02d", i ), "", "0x%02X", Regs[i] );
	}
	
	Ini->SetVal( "PSG", "PeriodA",		"",           PeriodA   );
	Ini->SetVal( "PSG", "PeriodB",		"",           PeriodB   );
	Ini->SetVal( "PSG", "PeriodC",		"",           PeriodC   );
	Ini->SetVal( "PSG", "PeriodN",		"",           PeriodN   );
	Ini->SetVal( "PSG", "PeriodE",		"",           PeriodE   );
	Ini->SetVal( "PSG", "CountA",		"",           CountA    );
	Ini->SetVal( "PSG", "CountB",		"",           CountB    );
	Ini->SetVal( "PSG", "CountC",		"",           CountC    );
	Ini->SetVal( "PSG", "CountN",		"",           CountN    );
	Ini->SetVal( "PSG", "CountE",		"",           CountE    );
	Ini->SetVal( "PSG", "VolA",			"",           VolA      );
	Ini->SetVal( "PSG", "VolB",			"",           VolB      );
	Ini->SetVal( "PSG", "VolC",			"",           VolC      );
	Ini->SetVal( "PSG", "VolE",			"",           VolE      );
	Ini->SetVal( "PSG", "EnvelopeA",	"", "0x%02X", EnvelopeA );
	Ini->SetVal( "PSG", "EnvelopeB",	"", "0x%02X", EnvelopeB );
	Ini->SetVal( "PSG", "EnvelopeC",	"", "0x%02X", EnvelopeC );
	Ini->SetVal( "PSG", "OutputA",		"", "0x%02X", OutputA   );
	Ini->SetVal( "PSG", "OutputB",		"", "0x%02X", OutputB   );
	Ini->SetVal( "PSG", "OutputC",		"", "0x%02X", OutputC   );
	Ini->SetVal( "PSG", "OutputN",		"", "0x%02X", OutputN   );
	Ini->SetVal( "PSG", "CountEnv",		"",           CountEnv  );
	Ini->SetVal( "PSG", "Hold",			"", "0x%02X", Hold      );
	Ini->SetVal( "PSG", "Alternate",	"", "0x%02X", Alternate );
	Ini->SetVal( "PSG", "Attack",		"", "0x%02X", Attack    );
	Ini->SetVal( "PSG", "Holding",		"", "0x%02X", Holding   );
	Ini->SetVal( "PSG", "RNG",			"",           RNG       );
#endif
	
	return true;
}


bool OPN64::DokoSave( cIni* Ini )
{
	if( !Ini ){
		return false;
	}
	
	Ini->SetVal( "OPN", "RegTimerA1",		"", "0x%02X", RegTimerA1    );
	Ini->SetVal( "OPN", "RegTimerA2",		"", "0x%02X", RegTimerA2    );
	Ini->SetVal( "OPN", "RegTimerB",		"", "0x%02X", RegTimerB     );
	Ini->SetVal( "OPN", "RegTimerCtr",		"", "0x%02X", RegTimerCtr   );
	Ini->SetVal( "OPN", "RegisterLatch",	"", "0x%02X", RegisterLatch );
	Ini->SetVal( "OPN", "LastEnable",		"", "0x%02X", LastEnable    );


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// fmgen
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class OPN : public OPNBase
//	private:
//		uint	fnum[3];
//		uint	fnum3[3];
//		uint8	fnum2[6];
//xx	Channel4 ch[3];				// 別途処理
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for( int i = 0; i < 3; i++ ){
		Ini->SetVal( "FMGEN_OPN", Stringf( "fnum%02d",  i ), "", "0x%08X", opn.fnum[i]  );
		Ini->SetVal( "FMGEN_OPN", Stringf( "fnum3%02d", i ), "", "0x%08X", opn.fnum3[i] );
	}
	for( int i = 0; i < 6; i++ ){
		Ini->SetVal( "FMGEN_OPN", Stringf( "fnum2%02d", i ), "", "0x%02X", opn.fnum2[i] );
	}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class OPNBase : public Timer
//	protected:
//		int		fmvolume;
//		uint	clock;
//		uint	rate;
//		uint	psgrate;
//		uint	status;
//xx	Channel4* csmch;			// コンストラクタで確定
//		static  uint32 lfotable[8];
//	private:
//		uint8	prescale;
//	protected:
//		Chip	chip;				// 別途処理
//xx	PSG		psg;				// 別途処理
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Ini->SetVal( "FMGEN_OPNBase", "fmvolume",	"",           opn.OPNBase::fmvolume );
	Ini->SetVal( "FMGEN_OPNBase", "clock",		"", "0x%08X", opn.OPNBase::clock    );
	Ini->SetVal( "FMGEN_OPNBase", "rate",		"", "0x%08X", opn.OPNBase::rate     );
	Ini->SetVal( "FMGEN_OPNBase", "psgrate",	"", "0x%08X", opn.OPNBase::psgrate  );
	Ini->SetVal( "FMGEN_OPNBase", "status",		"", "0x%08X", opn.OPNBase::status   );
	for( int i = 0; i < 8; i++ ){
		Ini->SetVal( "FMGEN_OPNBase", Stringf( "lfotable%02d", i ), "", "0x%08X", opn.lfotable[i] );
	}
	Ini->SetVal( "FMGEN_OPNBase", "prescale",	"", "0x%02X", opn.OPNBase::prescale );

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class Timer
//	protected:
//		uint8	status;
//		uint8	regtc;
//	private:
//		uint8	regta[2];
//		int32	timera, timera_count;
//		int32	timerb, timerb_count;
//		int32	timer_step;
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Ini->SetVal( "FMGEN_Timer", "status",		"", "0x%02X", opn.OPNBase::Timer::status );
	Ini->SetVal( "FMGEN_Timer", "regtc",		"", "0x%02X", opn.OPNBase::Timer::regtc );
	for( int i = 0; i < 2; i++ ){
		Ini->SetVal( "FMGEN_Timer", Stringf( "regta%02d", i ), "", "0x%02X", opn.OPNBase::Timer::regta[i] );
	}
	Ini->SetVal( "FMGEN_Timer", "timera",		"", "0x%08X", opn.OPNBase::Timer::timera       );
	Ini->SetVal( "FMGEN_Timer", "timerb",		"", "0x%08X", opn.OPNBase::Timer::timerb       );
	Ini->SetVal( "FMGEN_Timer", "timera_count",	"", "0x%08X", opn.OPNBase::Timer::timera_count );
	Ini->SetVal( "FMGEN_Timer", "timerb_count",	"", "0x%08X", opn.OPNBase::Timer::timerb_count );
	Ini->SetVal( "FMGEN_Timer", "timer_step",	"", "0x%08X", opn.OPNBase::Timer::timer_step   );

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class Channel4
//	private:
//xx	static const uint8 fbtable[8];	// const
//		uint	fb;
//		int		buf[4];
//		int*	in[3];
//		int*	out[3];
//??	int*	pms;				// 非メンバテーブル参照 常に更新されてるから無視する
//		int		algo_;
//		Chip*	chip_;				// コンストラクタで確定
//xx	static bool tablehasmade;	// コンストラクタで確定
//xx	static int 	kftable[64];	// コンストラクタで確定
//	public:
//xx	Operator op[4];				// 別途処理
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for( int i = 0; i < 3; i++ ){
		Ini->SetVal( "FMGEN_Channel4", Stringf( "fb%02d", i ),		"", "0x%08X", opn.ch[i].fb );
		for( int j = 0; j < 4; j++ ){
			Ini->SetVal( "FMGEN_Channel4", Stringf( "buf%02d_%02d", i, j ),	"", opn.ch[i].buf[j] );
		}
		for( int j = 0; j < 3; j++ ){
			Ini->SetVal( "FMGEN_Channel4", Stringf( "in%02d_%02d",  i, j ), "", (BYTE)((opn.ch[i].in[j]  - (const int*)opn.ch[i].buf) / sizeof(int*)) );
			Ini->SetVal( "FMGEN_Channel4", Stringf( "out%02d_%02d", i, j ), "", (BYTE)((opn.ch[i].out[j] - (const int*)opn.ch[i].buf) / sizeof(int*)) );
		}
		Ini->SetVal( "FMGEN_Channel4", Stringf( "algo_%02d", i ),	"",           opn.ch[i].algo_ );
		
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		//	class Operator
		//	private:
		//		typedef uint32 Counter;
		//		typedef int32  ISample;
		//		enum OpType { typeN=0, typeM=1 };
		//		enum EGPhase { next, attack, decay, sustain, release, off };
		//
		//??	Chip*	chip_;			// たぶんコンストラクタで確定
		//		ISample	out_, out2_;
		//		ISample in2_;
		//	Phase Generator -------------------------------------------------
		//		uint	dp_;			// ΔP
		//		uint	detune_;		// Detune
		//		uint	detune2_;		// DT2
		//		uint	multiple_;		// Multiple
		//		uint32	pg_count_;		// Phase 現在値
		//		uint32	pg_diff_;		// Phase 差分値
		//		int32	pg_diff_lfo_;	// Phase 差分値 >> x
		//	Envelop Generator -----------------------------------------------
		//		OpType	type_;			// OP の種類 (M, N...)
		//		uint	bn_;			// Block/Note
		//		int		eg_level_;		// EG の出力値
		//		int		eg_level_on_next_phase_;	// 次の eg_phase_ に移る値
		//		int		eg_count_;		// EG の次の変移までの時間
		//		int		eg_count_diff_;	// eg_count_ の差分
		//		int		eg_out_;		// EG+TL を合わせた出力値
		//		int		tl_out_;		// TL 分の出力値
		//		int		eg_rate_;
		//		int		eg_curve_count_;
		//		int		ssg_offset_;
		//		int		ssg_vector_;
		//		int		ssg_phase_;
		//
		//		uint	key_scale_rate_;	// key scale rate
		//		EGPhase	eg_phase_;
		//??	uint*	ams_;			// 非メンバテーブル参照 常に更新されてるから無視する
		//		uint	ms_;
		//		uint	tl_;			// Total Level	 (0-127)
		//		uint	tl_latch_;		// Total Level Latch (for CSM mode)
		//		uint	ar_;			// Attack Rate   (0-63)
		//		uint	dr_;			// Decay Rate    (0-63)
		//		uint	sr_;			// Sustain Rate  (0-63)
		//		uint	sl_;			// Sustain Level (0-127)
		//		uint	rr_;			// Release Rate  (0-63)
		//		uint	ks_;			// Keyscale      (0-3)
		//		uint	ssg_type_;		// SSG-Type Envelop Control
		//		bool	keyon_;
		//		bool	amon_;			// enable Amplitude Modulation
		//		bool	param_changed_;	// パラメータが更新された
		//		bool	mute_;
		//	Tables ----------------------------------------------------------
		//xx	static Counter rate_table[16];				// 未使用
		//xx	static uint32 multable[4][16];				// 未使用
		//xx	static const uint8 notetable[128];			// const
		//xx	static const int8 dttable[256];				// const
		//xx	static const int8 decaytable1[64][8];		// const
		//xx	static const int decaytable2[16];			// const
		//xx	static const int8 attacktable[64][8];		// const
		//xx	static const int ssgenvtable[8][2][3][2];	// const
		//xx	static uint	sinetable[1024];				// コンストラクタで確定
		//xx	static int32 cltable[FM_CLENTS];			// コンストラクタで確定
		//xx	static bool tablehasmade;					// コンストラクタで確定
		//	public:
		//		int		dbgopout_;
		//		int		dbgpgout_;
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		for( int j = 0; j < 4; j++ ){
			Ini->SetVal( "FMGEN_Operator", Stringf( "out_%02d_%02d",                    i, j ),	"",           opn.ch[i].op[j].out_                    );
			Ini->SetVal( "FMGEN_Operator", Stringf( "out2_%02d_%02d",                   i, j ),	"",           opn.ch[i].op[j].out2_                   );
			Ini->SetVal( "FMGEN_Operator", Stringf( "in2_%02d_%02d",                    i, j ),	"",           opn.ch[i].op[j].in2_                    );
			Ini->SetVal( "FMGEN_Operator", Stringf( "dp_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].dp_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "detune_%02d_%02d",                 i, j ),	"", "0x%08X", opn.ch[i].op[j].dp_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "detune2_%02d_%02d",                i, j ),	"", "0x%08X", opn.ch[i].op[j].detune2_                );
			Ini->SetVal( "FMGEN_Operator", Stringf( "multiple_%02d_%02d",               i, j ),	"", "0x%08X", opn.ch[i].op[j].multiple_               );
			Ini->SetVal( "FMGEN_Operator", Stringf( "pg_count_%02d_%02d",               i, j ),	"", "0x%08X", opn.ch[i].op[j].pg_count_               );
			Ini->SetVal( "FMGEN_Operator", Stringf( "pg_diff_%02d_%02d",                i, j ),	"", "0x%08X", opn.ch[i].op[j].pg_diff_                );
			Ini->SetVal( "FMGEN_Operator", Stringf( "pg_diff_lfo_%02d_%02d",            i, j ),	"",           opn.ch[i].op[j].pg_diff_lfo_            );
			Ini->SetVal( "FMGEN_Operator", Stringf( "type_%02d_%02d",                   i, j ),	"",           (int)opn.ch[i].op[j].type_              );
			Ini->SetVal( "FMGEN_Operator", Stringf( "bn_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].bn_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "eg_level_%02d_%02d",               i, j ),	"",           opn.ch[i].op[j].eg_level_               );
			Ini->SetVal( "FMGEN_Operator", Stringf( "eg_level_on_next_phase_%02d_%02d", i, j ),	"",           opn.ch[i].op[j].eg_level_on_next_phase_ );
			Ini->SetVal( "FMGEN_Operator", Stringf( "eg_count_%02d_%02d",               i, j ),	"",           opn.ch[i].op[j].eg_count_               );
			Ini->SetVal( "FMGEN_Operator", Stringf( "eg_count_diff_%02d_%02d",          i, j ),	"",           opn.ch[i].op[j].eg_count_diff_          );
			Ini->SetVal( "FMGEN_Operator", Stringf( "eg_out_%02d_%02d",                 i, j ),	"",           opn.ch[i].op[j].eg_out_                 );
			Ini->SetVal( "FMGEN_Operator", Stringf( "tl_out_%02d_%02d",                 i, j ),	"",           opn.ch[i].op[j].tl_out_                 );
			Ini->SetVal( "FMGEN_Operator", Stringf( "eg_rate_%02d_%02d",                i, j ),	"",           opn.ch[i].op[j].eg_rate_                );
			Ini->SetVal( "FMGEN_Operator", Stringf( "eg_curve_count_%02d_%02d",         i, j ),	"",           opn.ch[i].op[j].eg_curve_count_         );
			Ini->SetVal( "FMGEN_Operator", Stringf( "ssg_offset_%02d_%02d",             i, j ),	"",           opn.ch[i].op[j].ssg_offset_             );
			Ini->SetVal( "FMGEN_Operator", Stringf( "ssg_vector_%02d_%02d",             i, j ),	"",           opn.ch[i].op[j].ssg_vector_             );
			Ini->SetVal( "FMGEN_Operator", Stringf( "ssg_phase_%02d_%02d",              i, j ),	"",           opn.ch[i].op[j].ssg_phase_              );
			Ini->SetVal( "FMGEN_Operator", Stringf( "key_scale_rate_%02d_%02d",         i, j ),	"", "0x%08X", opn.ch[i].op[j].key_scale_rate_         );
			Ini->SetVal( "FMGEN_Operator", Stringf( "eg_phase_%02d_%02d",               i, j ),	"",           (int)opn.ch[i].op[j].eg_phase_          );
			Ini->SetVal( "FMGEN_Operator", Stringf( "ms_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].ms_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "tl_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].tl_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "tl_latch_%02d_%02d",               i, j ),	"", "0x%08X", opn.ch[i].op[j].tl_latch_               );
			Ini->SetVal( "FMGEN_Operator", Stringf( "ar_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].ar_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "dr_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].dr_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "sr_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].sr_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "sl_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].sl_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "rr_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].rr_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "ks_%02d_%02d",                     i, j ),	"", "0x%08X", opn.ch[i].op[j].ks_                     );
			Ini->SetVal( "FMGEN_Operator", Stringf( "ssg_type_%02d_%02d",               i, j ),	"", "0x%08X", opn.ch[i].op[j].ssg_type_               );
			Ini->SetVal( "FMGEN_Operator", Stringf( "keyon_%02d_%02d",                  i, j ),	"",           opn.ch[i].op[j].keyon_                  );
			Ini->SetVal( "FMGEN_Operator", Stringf( "amon_%02d_%02d",                   i, j ),	"",           opn.ch[i].op[j].amon_                   );
			Ini->SetVal( "FMGEN_Operator", Stringf( "param_changed_%02d_%02d",          i, j ),	"",           opn.ch[i].op[j].param_changed_          );
			Ini->SetVal( "FMGEN_Operator", Stringf( "mute_%02d_%02d",                   i, j ),	"",           opn.ch[i].op[j].mute_                   );
			Ini->SetVal( "FMGEN_Operator", Stringf( "dbgopout_%02d_%02d",               i, j ),	"",           opn.ch[i].op[j].dbgopout_               );
			Ini->SetVal( "FMGEN_Operator", Stringf( "dbgpgout_%02d_%02d",               i, j ),	"",           opn.ch[i].op[j].dbgpgout_               );
		}
	}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class Chip
//	private:
//		uint	ratio_;
//		uint	aml_;
//		uint	pml_;
//		int		pmv_;
//xx	OpType	optype_;			// 未使用
//		uint32	multable_[4][16];
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Ini->SetVal( "FMGEN_Chip", "ratio_",	"", "0x%08X", opn.chip.ratio_ );
	Ini->SetVal( "FMGEN_Chip", "aml_",		"", "0x%08X", opn.chip.aml_   );
	Ini->SetVal( "FMGEN_Chip", "pml_",		"", "0x%08X", opn.chip.pml_   );
	Ini->SetVal( "FMGEN_Chip", "pmv_",		"",           opn.chip.pmv_   );
	for( int i = 0; i < 4; i++ ){
		std::string strva;
		for( int j = 0; j < 16; j++ ){
			strva += Stringf( "%08X", opn.chip.multable_[i][j] );
		}
		Ini->SetEntry( "FMGEN_Chip", Stringf( "multable_%02X", i ), "", strva.c_str() );
	}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class PSG
//	protected:
//		uint8 reg[16];
//		const uint* envelop;
//		uint olevel[3];
//		uint32 scount[3], speriod[3];
//		uint32 ecount, eperiod;
//		uint32 ncount, nperiod;
//		uint32 tperiodbase;
//		uint32 eperiodbase;
//		uint32 nperiodbase;
//		int mask;
//xx	static uint enveloptable[16][64];		// コンストラクタで確定
//xx	static uint noisetable[noisetablesize];	// コンストラクタで確定
//xx	static int EmitTable[32];				// コンストラクタで確定
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for( int i = 0; i < 16; i++ ){
		Ini->SetVal( "FMGEN_PSG", Stringf( "Regs_%02d", i ), "", "0x%02X", opn.psg.reg[i] );
	}
	
	Ini->SetVal( "FMGEN_PSG", "envelop", "", (BYTE)((opn.psg.envelop - (const uint*)opn.psg.enveloptable) / 64) );
	
	for( int i = 0; i < 3; i++ ){
		Ini->SetVal( "FMGEN_PSG", Stringf( "olevel_%02d",  i ), "", "0x%02X", opn.psg.olevel[i]  );
		Ini->SetVal( "FMGEN_PSG", Stringf( "scount_%02d",  i ), "", "0x%08X", opn.psg.scount[i]  );
		Ini->SetVal( "FMGEN_PSG", Stringf( "speriod_%02d", i ), "", "0x%08X", opn.psg.speriod[i] );
	}
	
	Ini->SetVal( "FMGEN_PSG", "ecount",			"", "0x%08X", opn.psg.ecount      );
	Ini->SetVal( "FMGEN_PSG", "eperiod",		"", "0x%08X", opn.psg.eperiod     );
	Ini->SetVal( "FMGEN_PSG", "ncount",			"", "0x%08X", opn.psg.ncount      );
	Ini->SetVal( "FMGEN_PSG", "nperiod",		"", "0x%08X", opn.psg.nperiod     );
	Ini->SetVal( "FMGEN_PSG", "tperiodbase",	"", "0x%08X", opn.psg.tperiodbase );
	Ini->SetVal( "FMGEN_PSG", "eperiodbase",	"", "0x%08X", opn.psg.eperiodbase );
	Ini->SetVal( "FMGEN_PSG", "nperiodbase",	"", "0x%08X", opn.psg.nperiodbase );
	Ini->SetVal( "FMGEN_PSG", "mask",			"",           opn.psg.mask        );

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool PSG60::DokoLoad( cIni* Ini )
{
	if( !Ini ){
		return false;
	}
	
	Ini->GetVal( "PSG", "RegisterLatch",	RegisterLatch );
	Ini->GetVal( "PSG", "LastEnable",		LastEnable    );

#ifdef USEFMGEN
	for( int i = 0; i < 16; i++ ){
		Ini->GetVal( "PSG", Stringf( "Regs_%02d", i ), reg[i] );
	}
	
	Ini->GetVal( "PSG", "envelop", st );
	envelop = enveloptable[st];
	
	for( int i = 0; i < 3; i++ ){
		Ini->GetVal( "PSG", Stringf( "olevel_%02d",  i ), olevel[i]  );
		Ini->GetVal( "PSG", Stringf( "scount_%02d",  i ), scount[i]  );
		Ini->GetVal( "PSG", Stringf( "speriod_%02d", i ), speriod[i] );
	}
	
	Ini->GetVal( "PSG", "ecount",		ecount      );
	Ini->GetVal( "PSG", "eperiod",		eperiod     );
	Ini->GetVal( "PSG", "ncount",		ncount      );
	Ini->GetVal( "PSG", "nperiod",		nperiod     );
	Ini->GetVal( "PSG", "tperiodbase",	tperiodbase );
	Ini->GetVal( "PSG", "eperiodbase",	eperiodbase );
	Ini->GetVal( "PSG", "nperiodbase",	nperiodbase );
	Ini->GetVal( "PSG", "mask",			mask        );
#else
	for( int i = 0; i < 16; i++ ){
		Ini->GetVal( "PSG", Stringf( "Regs_%02d", i ), Regs[i] );
	}
	Ini->GetVal( "PSG", "PeriodA",		PeriodA   );
	Ini->GetVal( "PSG", "PeriodB",		PeriodB   );
	Ini->GetVal( "PSG", "PeriodC",		PeriodC   );
	Ini->GetVal( "PSG", "PeriodN",		PeriodN   );
	Ini->GetVal( "PSG", "PeriodE",		PeriodE   );
	Ini->GetVal( "PSG", "CountA",		CountA    );
	Ini->GetVal( "PSG", "CountB",		CountB    );
	Ini->GetVal( "PSG", "CountC",		CountC    );
	Ini->GetVal( "PSG", "CountN",		CountN    );
	Ini->GetVal( "PSG", "CountE",		CountE    );
	Ini->GetVal( "PSG", "VolA",			VolA      );
	Ini->GetVal( "PSG", "VolB",			VolB      );
	Ini->GetVal( "PSG", "VolC",			VolC      );
	Ini->GetVal( "PSG", "VolE",			VolE      );
	Ini->GetVal( "PSG", "EnvelopeA",	EnvelopeA );
	Ini->GetVal( "PSG", "EnvelopeB",	EnvelopeB );
	Ini->GetVal( "PSG", "EnvelopeC",	EnvelopeC );
	Ini->GetVal( "PSG", "OutputA",		OutputA   );
	Ini->GetVal( "PSG", "OutputB",		OutputB   );
	Ini->GetVal( "PSG", "OutputC",		OutputC   );
	Ini->GetVal( "PSG", "OutputN",		OutputN   );
	Ini->GetVal( "PSG", "CountEnv",		CountEnv  );
	Ini->GetVal( "PSG", "Hold",			Hold      );
	Ini->GetVal( "PSG", "Alternate",	Alternate );
	Ini->GetVal( "PSG", "Attack",		Attack    );
	Ini->GetVal( "PSG", "Holding",		Holding   );
	Ini->GetVal( "PSG", "RNG",			RNG       );
#endif
	
	return true;
}


bool OPN64::DokoLoad( cIni* Ini )
{
	int st;
	
	if( !Ini ){
		return false;
	}
	
	Ini->GetVal( "OPN", "RegTimerA1",		RegTimerA1    );
	Ini->GetVal( "OPN", "RegTimerA2",		RegTimerA2    );
	Ini->GetVal( "OPN", "RegTimerB",		RegTimerB     );
	Ini->GetVal( "OPN", "RegTimerCtr",		RegTimerCtr   );
	Ini->GetVal( "OPN", "RegisterLatch",	RegisterLatch );
	Ini->GetVal( "OPN", "LastEnable",		LastEnable    );


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// fmgen
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class OPN : public OPNBase
//	private:
//		uint	fnum[3];
//		uint	fnum3[3];
//		uint8	fnum2[6];
//xx	Channel4 ch[3];				// 別途処理
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for( int i = 0; i < 3; i++ ){
		Ini->GetVal( "FMGEN_OPN", Stringf( "fnum%02d",  i ), opn.fnum[i]  );
		Ini->GetVal( "FMGEN_OPN", Stringf( "fnum3%02d", i ), opn.fnum3[i] );
	}
	for( int i = 0; i < 6; i++ ){
		Ini->GetVal( "FMGEN_OPN", Stringf( "fnum2%02d", i ), opn.fnum2[i] );
	}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class OPNBase : public Timer
//	protected:
//		int		fmvolume;
//		uint	clock;
//		uint	rate;
//		uint	psgrate;
//		uint	status;
//xx	Channel4* csmch;			// コンストラクタで確定
//		static  uint32 lfotable[8];
//	private:
//		uint8	prescale;
//	protected:
//xx	Chip	chip;				// 別途処理
//xx	PSG		psg;				// 別途処理
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Ini->GetVal( "FMGEN_OPNBase", "fmvolume",	opn.OPNBase::fmvolume );
	Ini->GetVal( "FMGEN_OPNBase", "clock",		opn.OPNBase::clock    );
	Ini->GetVal( "FMGEN_OPNBase", "rate",		opn.OPNBase::rate     );
	Ini->GetVal( "FMGEN_OPNBase", "psgrate",	opn.OPNBase::psgrate  );
	Ini->GetVal( "FMGEN_OPNBase", "status",		opn.OPNBase::status   );
	for( int i = 0; i < 8; i++ ){
		Ini->GetVal( "FMGEN_OPNBase", Stringf( "lfotable%02d", i ), opn.lfotable[i] );
	}
	Ini->GetVal( "FMGEN_OPNBase", "prescale",	opn.OPNBase::prescale );

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class Timer
//	protected:
//		uint8	status;
//		uint8	regtc;
//	private:
//		uint8	regta[2];
//		int32	timera, timera_count;
//		int32	timerb, timerb_count;
//		int32	timer_step;
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Ini->GetVal( "FMGEN_Timer", "status",		opn.OPNBase::Timer::status       );
	Ini->GetVal( "FMGEN_Timer", "regtc",		opn.OPNBase::Timer::regtc        );
	for( int i = 0; i < 2; i++ ){
		Ini->GetVal( "FMGEN_Timer", Stringf( "regta%02d", i ), opn.OPNBase::Timer::regta[i] );
	}
	Ini->GetVal( "FMGEN_Timer", "timera",		opn.OPNBase::Timer::timera       );
	Ini->GetVal( "FMGEN_Timer", "timerb",		opn.OPNBase::Timer::timerb       );
	Ini->GetVal( "FMGEN_Timer", "timera_count",	opn.OPNBase::Timer::timera_count );
	Ini->GetVal( "FMGEN_Timer", "timerb_count",	opn.OPNBase::Timer::timerb_count );
	Ini->GetVal( "FMGEN_Timer", "timer_step",	opn.OPNBase::Timer::timer_step   );

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class Channel4
//	private:
//xx	static const uint8 fbtable[8];	// const
//		uint	fb;
//		int		buf[4];
//		int*	in[3];
//		int*	out[3];
//??	int*	pms;				// 非メンバテーブル参照 常に更新されてるから無視する
//		int		algo_;
//		Chip*	chip_;				// コンストラクタで確定
//xx	static bool tablehasmade;	// コンストラクタで確定
//xx	static int 	kftable[64];	// コンストラクタで確定
//	public:
//xx	Operator op[4];				// 別途処理
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for( int i = 0; i < 3; i++ ){
		Ini->GetVal( "FMGEN_Channel4", Stringf( "fb%02d", i ), opn.ch[i].fb );
		for( int j = 0; j < 4; j++ ){
			Ini->GetVal( "FMGEN_Channel4", Stringf( "buf%02d_%02d", i, j ), opn.ch[i].buf[j] );
		}
		for( int j = 0; j < 3; j++ ){
			Ini->GetVal( "FMGEN_Channel4", Stringf( "in%02d_%02d",  i, j ), st );
			opn.ch[i].in[j]  = &opn.ch[i].buf[st];
			Ini->GetVal( "FMGEN_Channel4", Stringf( "out%02d_%02d", i, j ), st );
			opn.ch[i].out[j] = &opn.ch[i].buf[st];
		}
		Ini->GetVal( "FMGEN_Channel4", Stringf( "algo_%02d", i ), opn.ch[i].algo_ );
		
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		//	class Operator
		//	private:
		//		typedef uint32 Counter;
		//		typedef int32  ISample;
		//		enum OpType { typeN=0, typeM=1 };
		//		enum EGPhase { next, attack, decay, sustain, release, off };
		//
		//??	Chip*	chip_;			// たぶんコンストラクタで確定
		//		ISample	out_, out2_;
		//		ISample in2_;
		//	Phase Generator -------------------------------------------------
		//		uint	dp_;			// ΔP
		//		uint	detune_;		// Detune
		//		uint	detune2_;		// DT2
		//		uint	multiple_;		// Multiple
		//		uint32	pg_count_;		// Phase 現在値
		//		uint32	pg_diff_;		// Phase 差分値
		//		int32	pg_diff_lfo_;	// Phase 差分値 >> x
		//	Envelop Generator -----------------------------------------------
		//		OpType	type_;			// OP の種類 (M, N...)
		//		uint	bn_;			// Block/Note
		//		int		eg_level_;		// EG の出力値
		//		int		eg_level_on_next_phase_;	// 次の eg_phase_ に移る値
		//		int		eg_count_;		// EG の次の変移までの時間
		//		int		eg_count_diff_;	// eg_count_ の差分
		//		int		eg_out_;		// EG+TL を合わせた出力値
		//		int		tl_out_;		// TL 分の出力値
		//		int		eg_rate_;
		//		int		eg_curve_count_;
		//		int		ssg_offset_;
		//		int		ssg_vector_;
		//		int		ssg_phase_;
		//
		//		uint	key_scale_rate_;	// key scale rate
		//		EGPhase	eg_phase_;
		//??	uint*	ams_;			// 非メンバテーブル参照 常に更新されてるから無視する
		//		uint	ms_;
		//		uint	tl_;			// Total Level	 (0-127)
		//		uint	tl_latch_;		// Total Level Latch (for CSM mode)
		//		uint	ar_;			// Attack Rate   (0-63)
		//		uint	dr_;			// Decay Rate    (0-63)
		//		uint	sr_;			// Sustain Rate  (0-63)
		//		uint	sl_;			// Sustain Level (0-127)
		//		uint	rr_;			// Release Rate  (0-63)
		//		uint	ks_;			// Keyscale      (0-3)
		//		uint	ssg_type_;		// SSG-Type Envelop Control
		//		bool	keyon_;
		//		bool	amon_;			// enable Amplitude Modulation
		//		bool	param_changed_;	// パラメータが更新された
		//		bool	mute_;
		//	Tables ----------------------------------------------------------
		//xx	static Counter rate_table[16];				// 未使用
		//xx	static uint32 multable[4][16];				// 未使用
		//xx	static const uint8 notetable[128];			// const
		//xx	static const int8 dttable[256];				// const
		//xx	static const int8 decaytable1[64][8];		// const
		//xx	static const int decaytable2[16];			// const
		//xx	static const int8 attacktable[64][8];		// const
		//xx	static const int ssgenvtable[8][2][3][2];	// const
		//xx	static uint	sinetable[1024];				// コンストラクタで確定
		//xx	static int32 cltable[FM_CLENTS];			// コンストラクタで確定
		//xx	static bool tablehasmade;					// コンストラクタで確定
		//	public:
		//		int		dbgopout_;
		//		int		dbgpgout_;
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		for( int j = 0; j < 4; j++ ){
			Ini->GetVal( "FMGEN_Operator", Stringf( "out_%02d_%02d",                    i, j ), opn.ch[i].op[j].out_                    );
			Ini->GetVal( "FMGEN_Operator", Stringf( "out2_%02d_%02d",                   i, j ), opn.ch[i].op[j].out2_                   );
			Ini->GetVal( "FMGEN_Operator", Stringf( "in2_%02d_%02d",                    i, j ), opn.ch[i].op[j].in2_                    );
			Ini->GetVal( "FMGEN_Operator", Stringf( "dp_%02d_%02d",                     i, j ), opn.ch[i].op[j].dp_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "detune_%02d_%02d",                 i, j ), opn.ch[i].op[j].dp_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "detune2_%02d_%02d",                i, j ), opn.ch[i].op[j].detune2_                );
			Ini->GetVal( "FMGEN_Operator", Stringf( "multiple_%02d_%02d",               i, j ), opn.ch[i].op[j].multiple_               );
			Ini->GetVal( "FMGEN_Operator", Stringf( "pg_count_%02d_%02d",               i, j ), opn.ch[i].op[j].pg_count_               );
			Ini->GetVal( "FMGEN_Operator", Stringf( "pg_diff_%02d_%02d",                i, j ), opn.ch[i].op[j].pg_diff_                );
			Ini->GetVal( "FMGEN_Operator", Stringf( "pg_diff_lfo_%02d_%02d",            i, j ), opn.ch[i].op[j].pg_diff_lfo_            );
			Ini->GetVal( "FMGEN_Operator", Stringf( "type_%02d_%02d",                   i, j ), (int&)opn.ch[i].op[j].type_             );
			Ini->GetVal( "FMGEN_Operator", Stringf( "bn_%02d_%02d",                     i, j ), opn.ch[i].op[j].bn_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "eg_level_%02d_%02d",               i, j ), opn.ch[i].op[j].eg_level_               );
			Ini->GetVal( "FMGEN_Operator", Stringf( "eg_level_on_next_phase_%02d_%02d", i, j ), opn.ch[i].op[j].eg_level_on_next_phase_ );
			Ini->GetVal( "FMGEN_Operator", Stringf( "eg_count_%02d_%02d",               i, j ), opn.ch[i].op[j].eg_count_               );
			Ini->GetVal( "FMGEN_Operator", Stringf( "eg_count_diff_%02d_%02d",          i, j ), opn.ch[i].op[j].eg_count_diff_          );
			Ini->GetVal( "FMGEN_Operator", Stringf( "eg_out_%02d_%02d",                 i, j ), opn.ch[i].op[j].eg_out_                 );
			Ini->GetVal( "FMGEN_Operator", Stringf( "tl_out_%02d_%02d",                 i, j ), opn.ch[i].op[j].tl_out_                 );
			Ini->GetVal( "FMGEN_Operator", Stringf( "eg_rate_%02d_%02d",                i, j ), opn.ch[i].op[j].eg_rate_                );
			Ini->GetVal( "FMGEN_Operator", Stringf( "eg_curve_count_%02d_%02d",         i, j ), opn.ch[i].op[j].eg_curve_count_         );
			Ini->GetVal( "FMGEN_Operator", Stringf( "ssg_offset_%02d_%02d",             i, j ), opn.ch[i].op[j].ssg_offset_             );
			Ini->GetVal( "FMGEN_Operator", Stringf( "ssg_vector_%02d_%02d",             i, j ), opn.ch[i].op[j].ssg_vector_             );
			Ini->GetVal( "FMGEN_Operator", Stringf( "ssg_phase_%02d_%02d",              i, j ), opn.ch[i].op[j].ssg_phase_              );
			Ini->GetVal( "FMGEN_Operator", Stringf( "key_scale_rate_%02d_%02d",         i, j ), opn.ch[i].op[j].key_scale_rate_         );
			Ini->GetVal( "FMGEN_Operator", Stringf( "eg_phase_%02d_%02d",               i, j ), (int&)opn.ch[i].op[j].eg_phase_         );
			Ini->GetVal( "FMGEN_Operator", Stringf( "ms_%02d_%02d",                     i, j ), opn.ch[i].op[j].ms_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "tl_%02d_%02d",                     i, j ), opn.ch[i].op[j].tl_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "tl_latch_%02d_%02d",               i, j ), opn.ch[i].op[j].tl_latch_               );
			Ini->GetVal( "FMGEN_Operator", Stringf( "ar_%02d_%02d",                     i, j ), opn.ch[i].op[j].ar_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "dr_%02d_%02d",                     i, j ), opn.ch[i].op[j].dr_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "sr_%02d_%02d",                     i, j ), opn.ch[i].op[j].sr_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "sl_%02d_%02d",                     i, j ), opn.ch[i].op[j].sl_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "rr_%02d_%02d",                     i, j ), opn.ch[i].op[j].rr_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "ks_%02d_%02d",                     i, j ), opn.ch[i].op[j].ks_                     );
			Ini->GetVal( "FMGEN_Operator", Stringf( "ssg_type_%02d_%02d",               i, j ), opn.ch[i].op[j].ssg_type_               );
			Ini->GetVal( "FMGEN_Operator", Stringf( "keyon_%02d_%02d",                  i, j ), opn.ch[i].op[j].keyon_                  );
			Ini->GetVal( "FMGEN_Operator", Stringf( "amon_%02d_%02d",                   i, j ), opn.ch[i].op[j].amon_                   );
			Ini->GetVal( "FMGEN_Operator", Stringf( "param_changed_%02d_%02d",          i, j ), opn.ch[i].op[j].param_changed_          );
			Ini->GetVal( "FMGEN_Operator", Stringf( "mute_%02d_%02d",                   i, j ), opn.ch[i].op[j].mute_                   );
			Ini->GetVal( "FMGEN_Operator", Stringf( "dbgopout_%02d_%02d",               i, j ), opn.ch[i].op[j].dbgopout_               );
			Ini->GetVal( "FMGEN_Operator", Stringf( "dbgpgout_%02d_%02d",               i, j ), opn.ch[i].op[j].dbgpgout_               );
		}
	}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class Chip
//	private:
//		uint	ratio_;
//		uint	aml_;
//		uint	pml_;
//		int		pmv_;
//xx	OpType	optype_;			// 未使用
//		uint32	multable_[4][16];
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Ini->GetVal( "FMGEN_Chip", "ratio_",	opn.chip.ratio_ );
	Ini->GetVal( "FMGEN_Chip", "aml_",		opn.chip.aml_   );
	Ini->GetVal( "FMGEN_Chip", "pml_",		opn.chip.pml_   );
	Ini->GetVal( "FMGEN_Chip", "pmv_",		opn.chip.pmv_   );
	for( int i = 0; i < 4; i++ ){
		std::string strva;
		if( Ini->GetEntry( "FMGEN_Chip", Stringf( "multable_%02X", i ), strva ) ){
			strva.resize( 16 * sizeof(DWORD) * 2, '0' );
			for( int j = 0; j < 16; j++ ){
				opn.chip.multable_[i][j] = std::stoul( strva.substr( j * sizeof(DWORD) * 2, sizeof(DWORD) * 2 ), nullptr, 16 );
			}
		}
	}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	class PSG
//	protected:
//		uint8 reg[16];
//		const uint* envelop;
//		uint olevel[3];
//		uint32 scount[3], speriod[3];
//		uint32 ecount, eperiod;
//		uint32 ncount, nperiod;
//		uint32 tperiodbase;
//		uint32 eperiodbase;
//		uint32 nperiodbase;
//		int mask;
//xx	static uint enveloptable[16][64];		// コンストラクタで確定
//xx	static uint noisetable[noisetablesize];	// コンストラクタで確定
//xx	static int EmitTable[32];				// コンストラクタで確定
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for( int i = 0; i < 16; i++ ){
		Ini->GetVal( "FMGEN_PSG", Stringf( "Regs_%02d", i ), opn.psg.reg[i] );
	}
	
	Ini->GetVal( "FMGEN_PSG", "envelop", st );
	opn.psg.envelop = opn.psg.enveloptable[st];
	
	for( int i = 0; i < 3; i++ ){
		Ini->GetVal( "FMGEN_PSG", Stringf( "olevel_%02d",  i ), opn.psg.olevel[i]  );
		Ini->GetVal( "FMGEN_PSG", Stringf( "scount_%02d",  i ), opn.psg.scount[i]  );
		Ini->GetVal( "FMGEN_PSG", Stringf( "speriod_%02d", i ), opn.psg.speriod[i] );
	}
	
	Ini->GetVal( "FMGEN_PSG", "ecount",			opn.psg.ecount      );
	Ini->GetVal( "FMGEN_PSG", "eperiod",		opn.psg.eperiod     );
	Ini->GetVal( "FMGEN_PSG", "ncount",			opn.psg.ncount      );
	Ini->GetVal( "FMGEN_PSG", "nperiod",		opn.psg.nperiod     );
	Ini->GetVal( "FMGEN_PSG", "tperiodbase",	opn.psg.tperiodbase );
	Ini->GetVal( "FMGEN_PSG", "eperiodbase",	opn.psg.eperiodbase );
	Ini->GetVal( "FMGEN_PSG", "nperiodbase",	opn.psg.nperiodbase );
	Ini->GetVal( "FMGEN_PSG", "mask",			opn.psg.mask        );

	return true;
}

