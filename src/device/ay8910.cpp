////////////////////////////////////////////////////////////////////
//  ay8910.cpp
////////////////////////////////////////////////////////////////////
#include "../log.h"
#include "ay8910.h"


#define MAX_OUTPUT	(0x7fff)
#define STEP 		(0x8000)

#define AY_AFINE	(0)
#define AY_ACOARSE	(1)
#define AY_BFINE	(2)
#define AY_BCOARSE	(3)
#define AY_CFINE	(4)
#define AY_CCOARSE	(5)
#define AY_NOISEPER	(6)
#define AY_ENABLE	(7)
#define AY_AVOL		(8)
#define AY_BVOL		(9)
#define AY_CVOL		(10)
#define AY_EFINE	(11)
#define AY_ECOARSE	(12)
#define AY_ESHAPE	(13)

#define AY_PORTA	(14)
#define AY_PORTB	(15)


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cAY8910::cAY8910( void )
#ifdef USEFMGEN
{}
#else
	: UpdateStep(0),
	PeriodA(0), PeriodB(0), PeriodC(0), PeriodN(0), PeriodE(0),
	CountA(0), CountB(0), CountC(0), CountN(0), CountE(0),
	VolA(0), VolB(0), VolC(0), VolE(0),
	EnvelopeA(0), EnvelopeB(0), EnvelopeC(0),
	OutputA(0), OutputB(0), OutputC(0), OutputN(0xff),
	CountEnv(0), Hold(0), Alternate(0), Attack(0), Holding(0),
	RNG(1)
{
	INITARRAY( Regs, 0 );
	INITARRAY( VolTable, 0 );
}
#endif


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cAY8910::~cAY8910( void ){}


////////////////////////////////////////////////////////////////
// クロック設定
////////////////////////////////////////////////////////////////
void cAY8910::SetClock( int clock, int rate )
{
	PRINTD( PSG_LOG, "[PSG][SetClock] clock:%d SampleRate:%d\n", clock, rate );
	
#ifdef USEFMGEN
	psg.SetClock( clock/2, rate );
#else
	// the step clock for the tone and noise generators is the chip clock
	// divided by 8; for the envelope generator of the AY-3-8910, it is half
	// that much (clock/16), but the envelope of the YM2149 goes twice as
	// fast, therefore again clock/8.
	// Here we calculate the number of steps which happen during one sample
	// at the given sample rate. No. of events = sample rate / (clock/8).
	// STEP is a multiplier used to turn the fraction into a fixed point
	// number.
	UpdateStep = (int)( ((double)STEP * (double)rate) / ((double)clock / 8.0) );
#endif
}


////////////////////////////////////////////////////////////////
// 音量設定(ボリュームテーブル設定)
////////////////////////////////////////////////////////////////
void cAY8910::SetVolumeTable( int vol )
{
	PRINTD( PSG_LOG, "[PSG][SetVolumeTable] %d\n", vol );
	
#ifdef USEFMGEN
	// とりあえず
	psg.SetVolume( 0 );
//	psg.SetVolume( vol );
#else
	double out = (MAX_OUTPUT * min( max( vol, 0 ), 100 ) ) / 100;
	
	for( int i=31; i>0; i-- ){
		VolTable[i] = (int)out;
		out /= 1.188502227;	// = 10 ^ (1.5/20) = 1.5dB
	}
	VolTable[0] = 0;
#endif
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void cAY8910::Reset( void )
{
	PRINTD( PSG_LOG, "[PSG][Reset]\n" );
	
	RegisterLatch = 0;
	LastEnable    = 0xff;
#ifdef USEFMGEN
	psg.Reset();
#else
	RNG = 1;
	OutputA = 0;
	OutputB = 0;
	OutputC = 0;
	OutputN = 0xff;
	for( int i=0; i<AY_PORTA; i++ ) _WriteReg( i, 0 );
#endif
}


////////////////////////////////////////////////////////////////
// レジスタ書込み
////////////////////////////////////////////////////////////////
// レジスタ書込みサブ
void cAY8910::_WriteReg( BYTE r, BYTE v )
{
#ifdef USEFMGEN
	psg.SetReg( r, v );
	
	switch( r ){
	case AY_ENABLE:
		if( (LastEnable == 0xff) || ((LastEnable & 0x40) != (v & 0x40)) ){
			// write out 0xff if port set to input
			PortAwrite( (v & 0x40) ? psg.GetReg(AY_PORTA) : 0xff );
		}
		if( (LastEnable == 0xff) || ((LastEnable & 0x80) != (v & 0x80)) ){
			// write out 0xff if port set to input
			PortBwrite( (v & 0x80) ? psg.GetReg(AY_PORTB) : 0xff );
		}
		LastEnable = v;
		break;
		
	case AY_PORTA:
		// 暫定措置
		// 実機ではポートがinput設定でも書込みできるようだ??
		if( psg.GetReg(AY_ENABLE) & 0x40 )
			PortAwrite( v );
		break;
		
	case AY_PORTB:
		// 暫定措置
		// 実機ではポートがinput設定でも書込みできるようだ??
		if( psg.GetReg(AY_ENABLE) & 0x80 )
			PortBwrite( v );
		break;
	}
#else
	int old;
	
	Regs[r] = v;
	
	switch( r ){
	case AY_AFINE:
	case AY_ACOARSE:
		Regs[AY_ACOARSE] &= 0x0f;
		old = PeriodA;
		PeriodA = ( Regs[AY_AFINE] + 256 * Regs[AY_ACOARSE] ) * UpdateStep;
		if( PeriodA == 0 ) PeriodA = UpdateStep;
		CountA += PeriodA - old;
		if( CountA <= 0 ) CountA = 1;
		break;
	case AY_BFINE:
	case AY_BCOARSE:
		Regs[AY_BCOARSE] &= 0x0f;
		old = PeriodB;
		PeriodB = ( Regs[AY_BFINE] + 256 * Regs[AY_BCOARSE] ) * UpdateStep;
		if( PeriodB == 0 ) PeriodB = UpdateStep;
		CountB += PeriodB - old;
		if( CountB <= 0 ) CountB = 1;
		break;
	case AY_CFINE:
	case AY_CCOARSE:
		Regs[AY_CCOARSE] &= 0x0f;
		old = PeriodC;
		PeriodC = ( Regs[AY_CFINE] + 256 * Regs[AY_CCOARSE] ) * UpdateStep;
		if( PeriodC == 0 ) PeriodC = UpdateStep;
		CountC += PeriodC - old;
		if( CountC <= 0 ) CountC = 1;
		break;
	case AY_NOISEPER:
		Regs[AY_NOISEPER] &= 0x1f;
		old = PeriodN;
		PeriodN = Regs[AY_NOISEPER] * UpdateStep;
		if( PeriodN == 0 ) PeriodN = UpdateStep;
		CountN += PeriodN - old;
		if( CountN <= 0 ) CountN = 1;
		break;
	case AY_ENABLE:
		if( (LastEnable == 0xff) || ((LastEnable & 0x40) != (Regs[AY_ENABLE] & 0x40)) ){
			// write out 0xff if port set to input
			PortAwrite( (Regs[AY_ENABLE] & 0x40) ? Regs[AY_PORTA] : 0xff );
		}
		if( (LastEnable == 0xff) || ((LastEnable & 0x80) != (Regs[AY_ENABLE] & 0x80)) ){
			// write out 0xff if port set to input
			PortBwrite( (Regs[AY_ENABLE] & 0x80) ? Regs[AY_PORTB] : 0xff );
		}
		LastEnable = Regs[AY_ENABLE];
		break;
	case AY_AVOL:
		Regs[AY_AVOL] &= 0x1f;
		EnvelopeA = Regs[AY_AVOL] & 0x10;
		VolA = EnvelopeA ? VolE : VolTable[Regs[AY_AVOL] ? Regs[AY_AVOL]*2+1 : 0];
		break;
	case AY_BVOL:
		Regs[AY_BVOL] &= 0x1f;
		EnvelopeB = Regs[AY_BVOL] & 0x10;
		VolB = EnvelopeB ? VolE : VolTable[Regs[AY_BVOL] ? Regs[AY_BVOL]*2+1 : 0];
		break;
	case AY_CVOL:
		Regs[AY_CVOL] &= 0x1f;
		EnvelopeC = Regs[AY_CVOL] & 0x10;
		VolC = EnvelopeC ? VolE : VolTable[Regs[AY_CVOL] ? Regs[AY_CVOL]*2+1 : 0];
		break;
	case AY_EFINE:
	case AY_ECOARSE:
		old = PeriodE;
		PeriodE = ( (Regs[AY_EFINE] + 256 * Regs[AY_ECOARSE]) ) * UpdateStep;
		if( PeriodE == 0 ) PeriodE = UpdateStep / 2;
		CountE += PeriodE - old;
		if( CountE <= 0 ) CountE = 1;
		break;
	case AY_ESHAPE:
		Regs[AY_ESHAPE] &= 0x0f;
		Attack = (Regs[AY_ESHAPE] & 0x04) ? 0x1f : 0x00;
		if( (Regs[AY_ESHAPE] & 0x08) == 0 ){
			Hold = 1;
			Alternate = Attack;
		}else{
			Hold = Regs[AY_ESHAPE] & 0x01;
			Alternate = Regs[AY_ESHAPE] & 0x02;
		}
		CountE = PeriodE;
		CountEnv = 0x1f;
		Holding = 0;
		VolE = VolTable[CountEnv ^ Attack];
		if( EnvelopeA ) VolA = VolE;
		if( EnvelopeB ) VolB = VolE;
		if( EnvelopeC ) VolC = VolE;
		break;
	case AY_PORTA:
		// 暫定措置
		// 実機ではポートがinput設定でも書込みできるようだ??
		if( Regs[AY_ENABLE] & 0x40 )
			PortAwrite( Regs[AY_PORTA] );
		break;
	case AY_PORTB:
		// 暫定措置
		// 実機ではポートがinput設定でも書込みできるようだ??
		if( Regs[AY_ENABLE] & 0x80 )
			PortBwrite( Regs[AY_PORTB] );
		break;
	}
#endif
}


// レジスタ書込みメイン
void cAY8910::WriteReg( BYTE addr, BYTE v )
{
	if( addr & 1 ){	// Data port
		PRINTD( PSG_LOG, "[PSG][WriteReg] Data -> %02X, %02X\n", RegisterLatch, v );
		
		if( RegisterLatch > 15 ) return;
#ifdef USEFMGEN
		if( RegisterLatch == AY_ESHAPE || psg.GetReg( RegisterLatch ) != v ){
#else
		if( RegisterLatch == AY_ESHAPE || Regs[RegisterLatch]         != v ){
#endif
			// レジスタを変更する前にストリームを更新する
			PreWriteReg();
		}
		_WriteReg( RegisterLatch, v );
	}else{			// Register port
		PRINTD( PSG_LOG, "[PSG][WriteReg] Latch -> Reg:%02X\n", v );
		
		RegisterLatch = v & 0x0f;
	}
}


////////////////////////////////////////////////////////////////
// レジスタ読込み
////////////////////////////////////////////////////////////////
BYTE cAY8910::ReadReg( void )
{
	PRINTD( PSG_LOG, "[PSG][ReadReg] -> %02X ", RegisterLatch );
	
	if( RegisterLatch > 15 ){
		PRINTD( PSG_LOG, "false\n" );
		return 0;
	}
	
#ifdef USEFMGEN
	switch( RegisterLatch ){
	case AY_PORTA:
//		if( !(psg.GetReg(AY_ENABLE) & 0x40) )
			psg.SetReg( AY_PORTA, PortAread() );
		break;
		
	case AY_PORTB:
//		if( !(psg.GetReg(AY_ENABLE) & 0x80) )
			psg.SetReg( AY_PORTB, PortBread() );
		break;
	}
	PRINTD( PSG_LOG, "%02X\n", psg.GetReg( RegisterLatch ) );
	return psg.GetReg( RegisterLatch );
#else
	switch( RegisterLatch ){
	case AY_PORTA:
//		if( !(Regs[AY_ENABLE] & 0x40) )
			Regs[AY_PORTA] = PortAread();
		break;
		
	case AY_PORTB:
//		if( !(Regs[AY_ENABLE] & 0x80) )
			Regs[AY_PORTB] = PortBread();
		break;
	}
	PRINTD( PSG_LOG, "%02X\n", Regs[RegisterLatch] );
	return Regs[RegisterLatch];
#endif
}


////////////////////////////////////////////////////////////////
// ストリーム1Sample更新
//
// 引数:	なし
// 返値:	int		更新サンプル
////////////////////////////////////////////////////////////////
int cAY8910::Update1Sample( void )
{
	PRINTD( PSG_LOG, "[PSG][Update1Sample]\n" );
	
#ifdef USEFMGEN
	PSG::Sample sbuf[2] = { 0, 0 };
	
	psg.Mix( sbuf, 1 );
	
	return sbuf[0];
#else
	int vola,volb,volc;
	int left;
	
	vola = volb = volc = 0;
	left = STEP;
	
	
	if( Regs[AY_ENABLE] & 0x01 ){
		if( CountA <= STEP ) CountA += STEP;
		OutputA = 1;
	}else if( Regs[AY_AVOL] == 0 ){
		if( CountA <= STEP ) CountA += STEP;
	}
	if( Regs[AY_ENABLE] & 0x02 ){
		if( CountB <= STEP ) CountB += STEP;
		OutputB = 1;
	}else if( Regs[AY_BVOL] == 0 ){
		if( CountB <= STEP ) CountB += STEP;
	}
	if( Regs[AY_ENABLE] & 0x04 ){
		if( CountC <= STEP ) CountC += STEP;
		OutputC = 1;
	}else if( Regs[AY_CVOL] == 0 ){
		if( CountC <= STEP ) CountC += STEP;
	}
	if( (Regs[AY_ENABLE] & 0x38) == 0x38 )	// all off
		if( CountN <= STEP ) CountN += STEP;
	
	int outn = (OutputN | Regs[AY_ENABLE]);
	
	
	do{
		int nextevent;
		
		if( CountN < left ) nextevent = CountN;
		else                nextevent = left;
		
		if( outn & 0x08 ){
			if( OutputA ) vola += CountA;
			CountA -= nextevent;
			while( CountA <= 0 ){
				CountA += PeriodA;
				if( CountA > 0 ){
					OutputA ^= 1;
					if( OutputA ) vola += PeriodA;
					break;
				}
				CountA += PeriodA;
				vola += PeriodA;
			}
			if( OutputA ) vola -= CountA;
		}else{
			CountA -= nextevent;
			while( CountA <= 0 ){
				CountA += PeriodA;
				if( CountA > 0 ){
					OutputA ^= 1;
					break;
				}
				CountA += PeriodA;
			}
		}
		
		if( outn & 0x10 ){
			if( OutputB ) volb += CountB;
			CountB -= nextevent;
			while( CountB <= 0 ){
				CountB += PeriodB;
				if( CountB > 0 ){
					OutputB ^= 1;
					if( OutputB ) volb += PeriodB;
					break;
				}
				CountB += PeriodB;
				volb += PeriodB;
			}
			if( OutputB ) volb -= CountB;
		}else{
			CountB -= nextevent;
			while( CountB <= 0 ){
				CountB += PeriodB;
				if( CountB > 0 ){
					OutputB ^= 1;
					break;
				}
				CountB += PeriodB;
			}
		}
		
		if( outn & 0x20 ){
			if( OutputC ) volc += CountC;
			CountC -= nextevent;
			while( CountC <= 0 ){
				CountC += PeriodC;
				if( CountC > 0 ){
					OutputC ^= 1;
					if( OutputC ) volc += PeriodC;
					break;
				}
				CountC += PeriodC;
				volc += PeriodC;
			}
			if( OutputC ) volc -= CountC;
		}else{
			CountC -= nextevent;
			while( CountC <= 0 ){
				CountC += PeriodC;
				if( CountC > 0 ){
					OutputC ^= 1;
					break;
				}
				CountC += PeriodC;
			}
		}
		
		CountN -= nextevent;
		if( CountN <= 0 ){
			// Is noise output going to change?
			if( (RNG + 1) & 2 ){	// (bit0^bit1)?
				OutputN = ~OutputN;
				outn = ( OutputN | Regs[AY_ENABLE] );
			}
			if( RNG & 1 ) RNG ^= 0x24000;	// This version is called the "Galois configuration".
			RNG >>= 1;
			CountN += PeriodN;
		}
		
		left -= nextevent;
	}while( left > 0 );
	
	// update envelope
	if( Holding == 0 ){
		CountE -= STEP;
		if( CountE <= 0 ){
			do{
				CountEnv--;
				CountE += PeriodE;
			}while( CountE <= 0 );
			
			// check envelope current position
			if( CountEnv < 0){
				if( Hold ){
					if( Alternate ) Attack ^= 0x1f;
					Holding = 1;
					CountEnv = 0;
				}else{
					if( Alternate && (CountEnv & 0x20) ) Attack ^= 0x1f;
					CountEnv &= 0x1f;
				}
			}
			
			VolE = VolTable[CountEnv ^ Attack];
			// reload volume
			if( EnvelopeA ) VolA = VolE;
			if( EnvelopeB ) VolB = VolE;
			if( EnvelopeC ) VolC = VolE;
		}
	}
	
	return (vola * VolA) / STEP + (volb * VolB) / STEP + (volc * VolC) / STEP;
#endif
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cAY8910::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "PSG", NULL, "RegisterLatch",	"0x%02X",	RegisterLatch );
	Ini->PutEntry( "PSG", NULL, "LastEnable",		"0x%02X",	LastEnable );

#ifdef USEFMGEN
/*
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "reg_%02d", i );
		Ini->PutEntry( "PSG", NULL, stren, "0x%02X", reg[i] );
	}
	
	Ini->PutEntry( "PSG", NULL, "envelop",			"%d",		(envelop - enveloptable)/64 );
	
	for( int i=0; i<3; i++ ){
		char stren[16];
		sprintf( stren, "olevel_%02d", i );
		Ini->PutEntry( "PSG", NULL, stren, "%d", olevel[i] );
		sprintf( stren, "scount_%02d", i );
		Ini->PutEntry( "PSG", NULL, stren, "0x%08X", scount[i] );
		sprintf( stren, "speriod_%02d", i );
		Ini->PutEntry( "PSG", NULL, stren, "0x%08X", speriod[i] );
	}
	Ini->PutEntry( "PSG", NULL, "ecount",			"0x%08X",	ecount );
	Ini->PutEntry( "PSG", NULL, "eperiod",			"0x%08X",	eperiod );
	Ini->PutEntry( "PSG", NULL, "ncount",			"0x%08X",	ncount );
	Ini->PutEntry( "PSG", NULL, "nperiod",			"0x%08X",	nperiod );
	
	Ini->PutEntry( "PSG", NULL, "tperiodbase",		"0x%08X",	tperiodbase );
	Ini->PutEntry( "PSG", NULL, "eperiodbase",		"0x%08X",	eperiodbase );
	Ini->PutEntry( "PSG", NULL, "nperiodbase",		"0x%08X",	nperiodbase );
	
	Ini->PutEntry( "PSG", NULL, "mask",				"%d",		mask );
*/
#else
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "Regs_%02d", i );
		Ini->PutEntry( "PSG", NULL, stren, "0x%02X", Regs[i] );
	}
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
#endif
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cAY8910::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt( "PSG", "RegisterLatch",	&st,	RegisterLatch );	RegisterLatch = st;
	Ini->GetInt( "PSG", "LastEnable",		&st,	LastEnable );		LastEnable = st;

#ifdef USEFMGEN
/*
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "reg_%02d", i );
		Ini->GetInt( "PSG", stren, &st, reg[i] );	reg[i] = st;
	}
	
	Ini->GetInt( "PSG", "envelop",		&st,	(envelop - enveloptable)/64 );	envelop = enveloptable[st];
	
	for( int i=0; i<3; i++ ){
		char stren[16];
		sprintf( stren, "olevel_%02d", i );
		Ini->GetInt( "PSG", stren, &st, olevel[i] );	olevel[i] = st;
		sprintf( stren, "scount_%02d", i );
		Ini->GetInt( "PSG", stren, &st, scount[i] );	scount[i] = st;
		sprintf( stren, "speriod_%02d", i );
		Ini->GetInt( "PSG", stren, &st, speriod[i] );	speriod[i] = st;
	}
	
	Ini->GetInt( "PSG", "ecount",		&st,	ecount );		ecount  = st;
	Ini->GetInt( "PSG", "eperiod",		&st,	eperiod );		eperiod = st;
	Ini->GetInt( "PSG", "ncount",		&st,	ncount );		ncount  = st;
	Ini->GetInt( "PSG", "nperiod",		&st,	nperiod );		nperiod = st;
	Ini->GetInt( "PSG", "tperiodbase",	&st,	tperiodbase );	tperiodbase = st;
	Ini->GetInt( "PSG", "eperiodbase",	&st,	eperiodbase );	eperiodbase = st;
	Ini->GetInt( "PSG", "nperiodbase",	&st,	nperiodbase );	nperiodbase = st;
	
	Ini->GetInt( "PSG", "mask",			&mask,	mask );
*/
#else
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "Regs_%02d", i );
		Ini->GetInt( "PSG", stren, &st, Regs[i] );	Regs[i] = st;
	}
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
#endif
	
	return true;
}
