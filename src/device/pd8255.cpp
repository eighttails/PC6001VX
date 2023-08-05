/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "log.h"
#include "pd8255.h"


// ハンドシェイク用制御ビット
#define	HS_INT	(0x08)	// bit3
#define	HS_STB	(0x10)	// bit4
#define	HS_IBF	(0x20)	// bit5
#define	HS_DAK	(0x40)	// bit6
#define	HS_OBF	(0x80)	// bit7

#define	HS_RIE	(0x10)	// bit4
#define	HS_WIE	(0x40)	// bit6


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cD8255::cD8255( void )
{
	Reset();
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cD8255::~cD8255(){}


/////////////////////////////////////////////////////////////////////////////
// リセット
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cD8255::Reset( void )
{
	PortA    = PortB    = PortC     = PortAbuf  = 0;
	ModeA    = ModeB    = 0;
	PortAdir = PortBdir = PortC1dir = PortC2dir = true;
	
	HSINT0 = HSWINT0 = HSRINT0 = HSIBF0 = false;
	HSSTB0 = HSDAK0  = HSOBF0  = true;
	RIE0   = WIE0    = false;
}


/////////////////////////////////////////////////////////////////////////////
// モード設定
//
// 引数:	data	データ転送方向
//				bit6,5: グループAのモード	1x:モード2 01:モード1 00:モード0
//				bit4:   PortA				1:入力(読み) 0:出力(書き)
//				bit3:   PortC(bit4-7)		1:入力(読み) 0:出力(書き)
//				bit2:   グループBのモード	1:モード1 0:モード0
//				bit1:   PortB				1:入力(読み) 0:出力(書き)
//				bit0:   PortC(bit0-3)		1:入力(読み) 0:出力(書き)
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cD8255::SetMode( BYTE data )
{
	ModeA     = (data >> 6) & 1 ? 2    : (data >> 5) & 1;
	ModeB     = (data >> 2) & 1;
	PortAdir  = (data >> 4) & 1 ? true : false;
	PortC2dir = (data >> 3) & 1 ? true : false;
	PortBdir  = (data >> 1) & 1 ? true : false;
	PortC1dir = (data     ) & 1 ? true : false;
	
	PortA = PortB = PortC = 0;
	if( ModeA == 2 ) PortC |= HS_OBF;
	
	PRINTD( PPI_LOG, "[8255][SetMode] GrA:%d GrB:%d\n", ModeA, ModeB );
}


/////////////////////////////////////////////////////////////////////////////
// PartA ライト(周辺側)
//
// 引数:	data	書き込むデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cD8255::WriteAE( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteAE] %02X", data );
	
	// とりあえずモード2の場合だけ考える
	if( ModeA == 2 ){
		// バッファにデータをラッチ
		PortAbuf = data;
		// 立下り
		if( HSSTB0 ){
			// IBF=H
			HSIBF0   = true;
			
			PRINTD( PPI_LOG, " DOWN IBF:%d RINT:%d DATA:%02X", HSIBF0 ? 1 : 0, HSRINT0 ? 1 : 0, data );
		}
		HSSTB0 = false;
		// 立上り
		if( !HSSTB0 ){
			// IBF=H ならRINT=H
			if( HSIBF0 ) HSRINT0 = true;
			
			PRINTD( PPI_LOG, " UP IBF:%d RINT:%d", HSIBF0 ? 1 : 0, HSRINT0 ? 1 : 0 );
		}
		HSSTB0 = true;
	}
	
	PRINTD( PPI_LOG, "\n" );
}


/////////////////////////////////////////////////////////////////////////////
// PartA リード(周辺側)
//
// 引数:	なし
// 返値:	BYTE	PortAの値
/////////////////////////////////////////////////////////////////////////////
BYTE cD8255::ReadAE( void )
{
	PRINTD( PPI_LOG, "[8255][ReadAE]" );
	
	// とりあえずモード2の場合だけ考える
	if( ModeA == 2 ){
		// 立下りならOBF=H
		if( HSDAK0 ){
			HSOBF0 = true;
			
			PRINTD( PPI_LOG, " DN OBF:%d WINT %d", HSOBF0 ? 1 : 0, HSWINT0 ? 1 : 0 );
		}
		HSDAK0 = false;
		// 立上り
		if( !HSDAK0 ){
			// OBF=H ならWRINT=H
			if( HSOBF0 ) HSWINT0 = true;
			
			PRINTD( PPI_LOG, " UP OBF:%d WINT %d", HSOBF0 ? 1 : 0, HSWINT0 ? 1 : 0 );
		}
		HSDAK0 = true;
	}
	
	PRINTD( PPI_LOG, "\n" );
	
	return PortA;
}


/////////////////////////////////////////////////////////////////////////////
// PartA ライト
//
// 引数:	data	書き込むデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cD8255::WriteA( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteA] %02X", data );
	
	if( !PortAdir ){
		PortA = data;
		
		// とりあえずモード2の場合だけ考える
		if( ModeA == 2 ){
			// WR0立下りでWINT=L
			HSWINT0 = false;
			// WR0立上りで DAK=H なら OBF=L
			if( HSDAK0 ) HSOBF0 = false;
			
			PRINTD( PPI_LOG, " WINT:%d OBF %d\n", HSWINT0 ? 1 : 0, HSOBF0 ? 1 : 0 );
		}
		JobWriteA( PortA );
	}else{
		PRINTD( PPI_LOG, "\n" );
	}
}


/////////////////////////////////////////////////////////////////////////////
// PartB ライト
//
// 引数:	data	書き込むデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cD8255::WriteB( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteB] %02X", data );
	
	if( !PortBdir ){
		PortB = data;
		JobWriteB( PortB );
	}
	
	PRINTD( PPI_LOG, "\n" );
}


/////////////////////////////////////////////////////////////////////////////
// PartC ライト
//
// 引数:	data	書き込むデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cD8255::WriteC( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteC] %02X", data );
	
	// 直接書込みはモード0の時にのみ可能
	
	// グループAがモード0  だったら 下位はbit0-3 上位はbit4-7
	//            モード1,2だったら 下位はbit0-2 上位はbit3-7
	
	
	if( ModeA == 0 ){
		// グループAがモード0,グループBがモード1だったら下位(bit0-3)をマスク
		if( ModeB == 1 ) data = ( PortC & 0x0f ) | ( data & 0xf0 );
	}else{
		// グループAがモード1,2だったら上位(bit3-7)をマスク
		PortC = ( PortC & 0xf8 ) | ( data & 0x07 );
		// グループAがモード1,2,グループBがモード1だったら全てマスク
		if( ModeB == 1 ) data = PortC;
	}
	
	// 下位
	if( !PortC1dir && ModeB == 0 ){
		if( ModeA == 0 ) PortC = ( PortC & 0xf0 ) | ( data & 0x0f );
		else             PortC = ( PortC & 0xf8 ) | ( data & 0x07 );
		JobWriteC1( PortC );
	}
	// 上位
	if( !PortC2dir && ModeA == 0 ){
		PortC = ( PortC & 0x07 ) | ( data & 0xf8 );
		JobWriteC2( PortC );
	}
	
	PRINTD( PPI_LOG, "\n" );
}


/////////////////////////////////////////////////////////////////////////////
// PartD ライト(コントロールポート)
//
// 引数:	data	書き込むデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cD8255::WriteD( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteD] %02X", data );
	
	if( data & 0x80 ){	// 最上位ビットが1なら
		// モード選択
		SetMode( data );
	}else{				// 最上位ビットが0なら
		// ビットセット/リセット
		// モード2で対象bitがINT(bit3),IBF(bit5),OBF(bit7)の場合は(入力ポートなので)マスクする
		// 他のモードはノーケア
		if( ModeA == 2 ){
			// bit毎の対応
			switch( (data >> 1) & 0x07 ){
			case 4: // RIE0
				RIE0 = ( data & 1 ) ? true : false;
				break;
			
			case 6: // WIE0
				WIE0 = ( data & 1 ) ? true : false;
				break;
			
			case 3: // INT0
			case 5: // IBF0
			case 7: // OBF0
				break;
			
			default:	// つまりbit0-2
				if( data & 1 ) PortC |=   1 << ((data >> 1) & 0x07);
				else           PortC &= ~(1 << ((data >> 1) & 0x07));
			}
		}else{
			if( data & 1 ) PortC |=   1 << ((data >> 1) & 0x07);
			else           PortC &= ~(1 << ((data >> 1) & 0x07));
		}
		JobWriteD( data );
	}
	
	PRINTD( PPI_LOG, "\n" );
}


/////////////////////////////////////////////////////////////////////////////
// PartA リード
//
// 引数:	なし
// 返値:	BYTE	PortAの値
/////////////////////////////////////////////////////////////////////////////
BYTE cD8255::ReadA( void )
{
	// 出力ポートでもそのまま読込める
	
	JobReadA();
	// とりあえずモード2の場合だけ考える
	if( ModeA == 2 ){
		// RD0立下りでRINT=L
		HSRINT0 = false;
		
		// IBF=Hならバッファからデータを読込む
		if( HSIBF0 ){
			PortA = PortAbuf;
		}
		
		// RD0立上りで STB=H なら IBF=L
		if( HSSTB0 ) HSIBF0 = false;
	}
	
	PRINTD( PPI_LOG, "[8255][Read][PortA] %02X STB:%d IBF:%d\n", PortA, HSSTB0, HSIBF0 );
	return PortA;
}


/////////////////////////////////////////////////////////////////////////////
// PartB リード
//
// 引数:	なし
// 返値:	BYTE	PortBの値
/////////////////////////////////////////////////////////////////////////////
BYTE cD8255::ReadB( void )
{
	// 出力ポートでもそのまま読込める
	
	JobReadB();
	return PortB;
}


/////////////////////////////////////////////////////////////////////////////
// PartC リード
//
// 引数:	なし
// 返値:	BYTE	PortCの値
/////////////////////////////////////////////////////////////////////////////
BYTE cD8255::ReadC( void )
{
	// 出力ポートでもそのまま読込める
	
	JobReadC();
	// とりあえずモード2の場合だけ考える
	if( ModeA == 2 ){
		HSWINT0 = ( HSOBF0 && WIE0 && HSDAK0 ) ? true : false;
		HSRINT0 = ( HSIBF0 && RIE0 && HSSTB0 ) ? true : false;
		HSINT0  = ( HSWINT0 || HSRINT0 ) ? true : false;
		
		PortC = ( HSOBF0 ? HS_OBF : 0 ) | ( WIE0   ? HS_WIE : 0 ) | ( HSIBF0 ? HS_IBF : 0 ) |
				( RIE0   ? HS_RIE : 0 ) | ( HSINT0 ? HS_INT : 0 ) | ( PortC & 0x07 );
	}
	PRINTD( PPI_LOG, "[8255][Read][PortC] OBF:%d WIE:%d IBF:%d RIE:%d INT:%d ", HSOBF0, WIE0, HSIBF0, RIE0, HSINT0  );
	PRINTD( PPI_LOG, "WINT:%d RINT:%d\n", HSWINT0, HSRINT0 );
	return PortC;
}


/////////////////////////////////////////////////////////////////////////////
// IBF取得
//
// 引数:	なし
// 返値:	bool	ステータス
/////////////////////////////////////////////////////////////////////////////
bool cD8255::GetIBF( void )
{
	// とりあえずモード2の場合だけ考える
	if( ModeA == 2 ) return HSIBF0;
	
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// OBF取得
//
// 引数:	なし
// 返値:	bool	ステータス
/////////////////////////////////////////////////////////////////////////////
bool cD8255::GetOBF( void )
{
	// とりあえずモード2の場合だけ考える
	if( ModeA == 2 ) return HSOBF0;
	
	return false;
}


