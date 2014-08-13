////////////////////////////////////////////////////////////////////
//  ym2203.cpp
////////////////////////////////////////////////////////////////////
#include "../log.h"
#include "ym2203.h"


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

#define YM_TIMERA1	(36)
#define YM_TIMERA2	(37)
#define YM_TIMERB	(38)
#define YM_TIMERCTR	(39)


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cYM2203::cYM2203( void ) : RegTimerA1(0), RegTimerA2(0), RegTimerB(0), RegTimerCtr(0)
{}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cYM2203::~cYM2203( void ){}


////////////////////////////////////////////////////////////////
// クロック設定
////////////////////////////////////////////////////////////////
void cYM2203::SetClock( int clock, int rate )
{
	PRINTD( PSG_LOG, "[OPN][SetClock] clock:%d SampleRate:%d\n", clock, rate );
	
	opn.SetRate( clock, rate );
}


////////////////////////////////////////////////////////////////
// 音量設定(ボリュームテーブル設定)
////////////////////////////////////////////////////////////////
void cYM2203::SetVolumeTable( int vol )
{
	PRINTD( PSG_LOG, "[OPN][SetVolumeTable] %d\n", vol );
	
	// とりあえず
	opn.SetVolumeFM( -15 );
	opn.SetVolumePSG( 0 );
//	opn.SetVolume( vol );
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool cYM2203::Init( int clock, int rate )
{
	PRINTD( PSG_LOG, "[OPN][Init] clock:%d SampleRate:%d\n", clock, rate );
	
	return opn.Init( clock, rate );
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void cYM2203::Reset( void )
{
	PRINTD( PSG_LOG, "[OPN][Reset]\n" );
	
	RegisterLatch = 0;
	RegTimerA1    = 0;
	RegTimerA2    = 0;
	RegTimerB     = 0;
	RegTimerCtr   = 0;
	
	opn.Reset();
}


////////////////////////////////////////////////////////////////
// タイマ割込み発生
////////////////////////////////////////////////////////////////
bool cYM2203::TimerIntr( void )
{
	// CSMモード対応
	// タイマオーバフロー時にストリームを更新する
	PreWriteReg();
	
	DWORD ct = opn.GetNextEvent();
	if( ct == ULONG_MAX ) return false;
	
	return opn.Count( ct );
}


////////////////////////////////////////////////////////////////
// レジスタ書込み
////////////////////////////////////////////////////////////////
// レジスタ書込みサブ
void cYM2203::_WriteReg( BYTE r, BYTE v )
{
	opn.SetReg( r, v );
	
	switch( r ){
	case AY_ENABLE:
		if( (LastEnable == 0xff) || ((LastEnable & 0x40) != (v & 0x40)) ){
			// write out 0xff if port set to input
			PortAwrite( (v & 0x40) ? opn.GetReg(AY_PORTA) : 0xff );
		}
		if( (LastEnable == 0xff) || ((LastEnable & 0x80) != (v & 0x80)) ){
			// write out 0xff if port set to input
			PortBwrite( (v & 0x80) ? opn.GetReg(AY_PORTB) : 0xff );
		}
		LastEnable = v;
		break;
		
	case AY_PORTA:
		// 暫定措置
		// 実機ではポートがinput設定でも書込みできるようだ??
		if( opn.GetReg(AY_ENABLE) & 0x40 )
			PortAwrite( v );
		break;
		
	case AY_PORTB:
		// 暫定措置
		// 実機ではポートがinput設定でも書込みできるようだ??
		if( opn.GetReg(AY_ENABLE) & 0x80 )
			PortBwrite( v );
		break;
		
		
	case YM_TIMERA1:
		RegTimerA1 = v;
		if( RegTimerCtr & 0x01 )
			// タイマ稼働中なら再設定
			SetTimerA( (RegTimerA1<<2) | (RegTimerA2&0x03) );
		break;
		
	case YM_TIMERA2:
		RegTimerA2 = v;
		if( RegTimerCtr & 0x01 )
			// タイマ稼働中なら再設定
			SetTimerA( (RegTimerA1<<2) | (RegTimerA2&0x03) );
		break;
		
	case YM_TIMERB:
		RegTimerB = v;
		if( RegTimerCtr & 0x02 )
			// タイマ稼働中なら再設定
			SetTimerB( RegTimerB );
		break;
		
	case YM_TIMERCTR:
		RegTimerCtr = v;
		SetTimerA( RegTimerCtr & 0x01 ? ((RegTimerA1<<2)|(RegTimerA2&0x03)) : 0 );
		SetTimerB( RegTimerCtr & 0x02 ? RegTimerB                           : 0 );
		break;
	}
}


// レジスタ書込みメイン
void cYM2203::WriteReg( BYTE addr, BYTE v )
{
	if( addr & 1 ){	// Data port
		PRINTD( PSG_LOG, "[OPN][WriteReg] Data -> %02X, %02X\n", RegisterLatch, v );
		
		if( RegisterLatch == AY_ESHAPE || opn.GetReg( RegisterLatch ) != v ){
// (ToDo)FM音源の音が変わるレジスタを特定
			// レジスタを変更する前にストリームを更新する
			PreWriteReg();
		}
		_WriteReg( RegisterLatch, v );
	}else{			// Register port
		PRINTD( PSG_LOG, "[OPN][WriteReg] Latch -> Reg:%02X\n", v );
		
		RegisterLatch = v;
	}
}


////////////////////////////////////////////////////////////////
// レジスタ読込み
////////////////////////////////////////////////////////////////
BYTE cYM2203::ReadReg( void )
{
	PRINTD( PSG_LOG, "[OPN][ReadReg] -> %02X ", RegisterLatch );
	
	switch( RegisterLatch ){
	case AY_PORTA:
//		if( !(opn.GetReg(AY_ENABLE) & 0x40) )
			opn.SetReg( AY_PORTA, PortAread() );
		break;
		
	case AY_PORTB:
//		if( !(opn.GetReg(AY_ENABLE) & 0x80) )
			opn.SetReg( AY_PORTB, PortBread() );
		break;
	}
	PRINTD( PSG_LOG, "%02X\n", opn.GetReg( RegisterLatch ) );
	return opn.GetReg( RegisterLatch );
}


////////////////////////////////////////////////////////////////
// ステータスレジスタ読込み
////////////////////////////////////////////////////////////////
BYTE cYM2203::ReadStatus( void )
{
	PRINTD( PSG_LOG, "[OPN][ReadStatus] -> %02X\n", opn.ReadStatus() );
	
	return opn.ReadStatus();
}


////////////////////////////////////////////////////////////////
// ストリーム1Sample更新
//
// 引数:	なし
// 返値:	int		更新サンプル
////////////////////////////////////////////////////////////////
int cYM2203::Update1Sample( void )
{
//	PRINTD( PSG_LOG, "[OPN][Update1Sample]\n" );
	
	FM::Sample sbuf[2] = { 0, 0 };
	
	opn.Mix( sbuf, 1 );
	
	return sbuf[0];
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cYM2203::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "OPN", NULL, "RegisterLatch",	"0x%02X",	RegisterLatch );
	Ini->PutEntry( "OPN", NULL, "LastEnable",		"0x%02X",	LastEnable );
	
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cYM2203::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt( "OPN", "RegisterLatch",	&st,	RegisterLatch );	RegisterLatch = st;
	Ini->GetInt( "OPN", "LastEnable",		&st,	LastEnable );		LastEnable = st;
	
	return true;
}
