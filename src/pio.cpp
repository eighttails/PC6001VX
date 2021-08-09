/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <fstream>

#include "config.h"
#include "intr.h"
#include "osd.h"
#include "p6vm.h"
#include "pio.h"


/*
BASIC ROM内ルーチン
								// 8049からAにデータを受け取る
Z0323:	LD	A,0CH		;E78	// WIE(6)をLにする つまり書込み要求割込み禁止 INT=RINTとするため?
		OUT	(093H),A	;E7A
								// たぶんこの辺のタイミングで8049がデータを出力してSTB(4)=Lにする。
								// するとPortAはラッチされ,バッファが満たされるのでIBF(5)=Hとなる。
								// それにより8049はデータ出力を停止し,STB(4)=Hにする。
								// STB(4)の立ち上がりでINT(3)=Hとなる。
								// だから IBF(5)=H -> INT(3)=H の順に待つのが正しい?
Z0319:	IN	A,(092H)	;E7C
		AND	08H			;E7E	// INT(3)がHになるまで待つ
		JR	Z,0E7CH		;E80	// つまり8049からのデータがPortAにラッチされるまで待つ
		IN	A,(092H)	;E82
		AND	020H		;E84	// IBF(5)がHになるまで待つ ←多分INT=Hより先にHになってる
		JR	Z,0E7CH		;E86	// つまりPortAの入力バッファが満たされるまで待つ
		LD	A,0DH		;E88	// WIE(6)をHにする つまり書込み要求割込み許可
		OUT	(093H),A	;E8A
		IN	A,(090H)	;E8C	// PortAからデータを受け取る
		RET				;E8E
		
								// Aのデータを8049へ送る
Z0044:	PUSH	AF		;E8F
		LD	A,08H		;E90	// RIE(4)をLにする つまり読込み要求割込み禁止 INT=WINTとするため?
		OUT	(093H),A	;E92
Z0320:	IN	A,(092H)	;E94
		AND	08H			;E96	// INT(3)がHになるまで待つ
		JR	Z,0E94H		;E98	// つまり8049がデータを受け取ってバッファが空になるまで待つ 前回送ったデータ?
		IN	A,(092H)	;E9A
		AND	080H		;E9C	// OBF(7)がHになるまで待つ ←多分INT=Hより先にHになってる
		JR	Z,0E94H		;E9E	// つまり8049がデータを受け取ってDAK=Hになるまで待つ 前回送ったデータ?
		LD	A,09H		;EA0	// RIE(4)をHにする つまり読込み要求割込み許可
		OUT	(093H),A	;EA2
		POP	AF			;EA4
		OUT	(090H),A	;EA5	// PortAにデータを送る
		RET				;EA7


【8255 モード2の挙動】
bit3 INT(出力)	[入力の場合]この出力は8049からのデータがPortA(入力)にラッチされたときにHとなり
				CPUに対するデータ読込み要求割り込み信号として働く。
				[出力の場合]この出力は8049がデータを受け取った時にHとなり,CPUに対する
				次のデータの書込み要求割り込み信号として働く。

bit4 STB(入力)	この入力をLにすると8049からPortAに送られている信号をPortAがラッチする
				[注意]IBFがLになる前にSTBがLになるとポート内ラッチデータが変化してしまう。
				　　　STBはIBFがLになるまではHに保たなければならない。
				[補足]8049の~WRに繋がっているらしい。つまり8049がPortAに出力したら即ラッチされる。

bit5 IBF(出力)	この出力がHの時はPortAの入力バッファが満たされていることを意味し,8049に対してデータの転送の禁止を知らせる。
				この信号はSTBの立ち下がりでHとなりSTB=1の時のデータ読込み終了時(~RD立ち上がり)でLになる。
				モード設定直後の初期値はLである。

bit6 DAK(入力)	この入力は8049がPortA(出力)のデータを受け取ったということを知らせる信号である。
				データを受け取った時にLの信号を出すように8049を設計する。
				[補足]8049の~RDに繋がっているらしい。つまり8049がPortAから入力したら即DAK=Lになる。

bit7 OBF(出力)	この信号はCPUからのデータを受けPortA(出力)にラッチされた時にLとなり,8049に対してのデータ受け取り要求として働く。
				OBFはDAK=1のときのデータ書込み終了時(~WR立ち上がり)でLになり8049がデータを受け取った時のDAKの立ち下がりでHになる。
				モード設定直後の初期値はHである。
				[注意]OBFがHになる前にデータの書込みを行うとPortA(出力)が変化してしまうので,OBFがLの間は書込みは避ける。
				[補足]8049のINTに繋がっているらしい。つまり8049は割込みでデータを受け取る。


bit4 RIE		CPUに対するデータ読込み要求割り込みの許可フラグで,このビットをビット操作命令で1にすることによって割り込み許可,
				0にすることによって割り込み禁止にする。
				RIEを書き換えても同じビットに割り当てられているSTBの機能には影響を与えない。

bit6 WIE		CPUに対するデータ書込み要求割り込みの許可フラグで,このビットをビット操作命令で1にすることによって割り込み許可,
				0にすることによって割り込み禁止にする。
				WIEを書き換えても同じビットに割り当てられているDAKの機能には影響を与えない。
*/


// ハンドシェイク用制御ビット
//#define	HS_INT	(0x08)	// bit3
//#define	HS_STB	(0x10)	// bit4
//#define	HS_IBF	(0x20)	// bit5
//#define	HS_DAK	(0x40)	// bit6
//#define	HS_OBF	(0x80)	// bit7

//#define	HS_RIE	(0x10)	// bit4
//#define	HS_WIE	(0x40)	// bit6

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cPRT::cPRT() : FilePath( "" ), pdata( 0 ), strb( false )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cPRT::~cPRT()
{
}


/////////////////////////////////////////////////////////////////////////////
// プリンタ出力ファイル名設定
//
// 引数:	filepath	出力ファイルパス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cPRT::SetFile( const P6VPATH& filepath )
{
	if( !filepath.empty() ){
		// ファイルパス保存
		FilePath = filepath;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 印刷するデータを受付
//
// 引数:	data	印刷するデータデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cPRT::SetData( BYTE data )
{
	pdata = ~data;
}


/////////////////////////////////////////////////////////////////////////////
// ストローブ受付
//
// 引数:	st		ストローブ信号 true:有効 false:無効
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cPRT::Strobe( bool st )
{
	std::fstream fs;
	
	if( !strb && st && OSD_FSopen( fs, FilePath, std::ios_base::out|std::ios_base::binary|std::ios_base::app ) ){
		FSPUTBYTE( pdata, fs );
		fs.close();
	}
	
	strb = st;
}


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
PIO6::PIO6( VM6* vm, const ID& id ) : Device( vm, id )
{
	// Device Description (Out)
	descs.outdef.emplace( out90H, STATIC_CAST( Device::OutFuncPtr, &PIO6::Out90H ) );
	descs.outdef.emplace( out91H, STATIC_CAST( Device::OutFuncPtr, &PIO6::Out91H ) );
	descs.outdef.emplace( out92H, STATIC_CAST( Device::OutFuncPtr, &PIO6::Out92H ) );
	descs.outdef.emplace( out93H, STATIC_CAST( Device::OutFuncPtr, &PIO6::Out93H ) );
	descs.outdef.emplace( outPBH, STATIC_CAST( Device::OutFuncPtr, &PIO6::OutPBH ) );
	
	// Device Description (In)
	descs.indef.emplace ( in90H,  STATIC_CAST( Device::InFuncPtr,  &PIO6::In90H  ) );
	descs.indef.emplace ( in92H,  STATIC_CAST( Device::InFuncPtr,  &PIO6::In92H  ) );
	descs.indef.emplace ( in93H,  STATIC_CAST( Device::InFuncPtr,  &PIO6::In93H  ) );
	descs.indef.emplace ( inPBH,  STATIC_CAST( Device::InFuncPtr,  &PIO6::InPBH  ) );
	descs.indef.emplace ( inIBF,  STATIC_CAST( Device::InFuncPtr,  &PIO6::InIBF  ) );
	descs.indef.emplace ( inOBF,  STATIC_CAST( Device::InFuncPtr,  &PIO6::InOBF  ) );
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
PIO6::~PIO6()
{
}


/////////////////////////////////////////////////////////////////////////////
// PartA ライト
//
// 引数:	data	書き込むデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void PIO6::JobWriteA( BYTE data )
{
	// モード2で DAK=H,OBF=L(つまりWR0立上り) なら8049に対して割込み要求
	if( ModeA == 2 && HSDAK0 && !HSOBF0 )
		vm->CpusExtIntr();
}


/////////////////////////////////////////////////////////////////////////////
// PartB ライト
//
// 引数:	data	書き込むデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void PIO6::JobWriteB( BYTE data )
{
	// プリンタにデータ出力
	cPRT::SetData( data );
}


/////////////////////////////////////////////////////////////////////////////
// PartC ライト
//
// 引数:	data	書き込むデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void PIO6::JobWriteC1( BYTE data )
{
	// プリンタストローブ
	cPRT::Strobe( data&1 ? false : true );
	
	// CRT表示状態設定
	vm->VdgSetCrtDisp( data&2 ? true : false );
	
	// CG ROM BANK 選択
	vm->MemSetCGBank( data&4 ? false : true );
}


/////////////////////////////////////////////////////////////////////////////
// PartD ライト(コントロールポート)
//
// 引数:	data	書き込むデータ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void PIO6::JobWriteD( BYTE data )
{
	// bit毎の対応
	switch( (data>>1)&0x07 ){
	case 0: // プリンタストローブ
		cPRT::Strobe( data&1 ? false : true );
		break;
		
	case 1: // CRT表示状態設定
		vm->VdgSetCrtDisp( data&1 ? true : false );
		break;
		
	case 2: // CG ROM BANK 選択
		vm->MemSetCGBank( data&1 ? false : true );
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// I/Oアクセス関数
/////////////////////////////////////////////////////////////////////////////
void PIO6::Out90H( int, BYTE data ){ WriteA( data ); }
void PIO6::Out91H( int, BYTE data ){ WriteB( data ); }
void PIO6::Out92H( int, BYTE data ){ WriteC( data ); }
void PIO6::Out93H( int, BYTE data ){ WriteD( data ); }
BYTE PIO6::In90H( int ){ return ReadA(); }
BYTE PIO6::In92H( int ){ return ReadC(); }
BYTE PIO6::In93H( int ){ return 0xff; }

void PIO6::OutPBH( int, BYTE data ){ WriteAE( data ); }
BYTE PIO6::InPBH( int ){ return ReadAE(); }
BYTE PIO6::InIBF( int ){ return GetIBF() ? 1 : 0; }
BYTE PIO6::InOBF( int ){ return GetOBF() ? 1 : 0; }


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool PIO6::DokoSave( cIni* Ini )
{
	if( !Ini ) return false;
	
	Ini->SetVal( "8255", "PortA",		"", "0x%02X", PortA    );
	Ini->SetVal( "8255", "PortB",		"", "0x%02X", PortB    );
	Ini->SetVal( "8255", "PortC",		"", "0x%02X", PortC    );
	Ini->SetVal( "8255", "PortAbuf",	"", "0x%02X", PortAbuf );
	Ini->SetVal( "8255", "ModeA",		"", ModeA     );
	Ini->SetVal( "8255", "ModeB",		"", ModeB     );
	Ini->SetVal( "8255", "PortAdir",	"", PortAdir  );
	Ini->SetVal( "8255", "PortBdir",	"", PortBdir  );
	Ini->SetVal( "8255", "PortC1dir",	"", PortC1dir );
	Ini->SetVal( "8255", "PortC2dir",	"", PortC2dir );
	Ini->SetVal( "8255", "HSINT0",		"", HSINT0    );
	Ini->SetVal( "8255", "HSWINT0",		"", HSWINT0   );
	Ini->SetVal( "8255", "HSRINT0",		"", HSRINT0   );
	Ini->SetVal( "8255", "HSSTB0",		"", HSSTB0    );
	Ini->SetVal( "8255", "HSIBF0",		"", HSIBF0    );
	Ini->SetVal( "8255", "HSDAK0",		"", HSDAK0    );
	Ini->SetVal( "8255", "HSOBF0",		"", HSOBF0    );
	Ini->SetVal( "8255", "RIE0",		"", RIE0      );
	Ini->SetVal( "8255", "WIE0",		"", WIE0      );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool PIO6::DokoLoad( cIni* Ini )
{
	if( !Ini ) return false;
	
	Ini->GetVal( "8255", "PortA",		PortA     );
	Ini->GetVal( "8255", "PortB",		PortB     );
	Ini->GetVal( "8255", "PortC",		PortC     );
	Ini->GetVal( "8255", "PortAbuf",	PortAbuf  );
	Ini->GetVal( "8255", "ModeA",		ModeA     );
	Ini->GetVal( "8255", "ModeB",		ModeB     );
	Ini->GetVal( "8255", "PortAdir",	PortAdir  );
	Ini->GetVal( "8255", "PortBdir",	PortBdir  );
	Ini->GetVal( "8255", "PortC1dir",	PortC1dir );
	Ini->GetVal( "8255", "PortC2dir",	PortC2dir );
	Ini->GetVal( "8255", "HSINT0",		HSINT0    );
	Ini->GetVal( "8255", "HSWINT0",		HSWINT0   );
	Ini->GetVal( "8255", "HSRINT0",		HSRINT0   );
	Ini->GetVal( "8255", "HSSTB0",		HSSTB0    );
	Ini->GetVal( "8255", "HSIBF0",		HSIBF0    );
	Ini->GetVal( "8255", "HSDAK0",		HSDAK0    );
	Ini->GetVal( "8255", "HSOBF0",		HSOBF0    );
	Ini->GetVal( "8255", "RIE0",		RIE0      );
	Ini->GetVal( "8255", "WIE0",		WIE0      );
	
	return true;
}

