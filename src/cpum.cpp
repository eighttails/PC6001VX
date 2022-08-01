/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "pc6001v.h"

#include "config.h"
#include "cpum.h"
#include "p6vm.h"


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CPU6::CPU6( VM6* vm, const ID& id ) : Device( vm, id )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
CPU6::~CPU6( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// フェッチ(M1)
/////////////////////////////////////////////////////////////////////////////
BYTE CPU6::Fetch( WORD addr, int* m1wait )
{
	return vm->MemFetch( addr, m1wait );
}


/////////////////////////////////////////////////////////////////////////////
// メモリアクセス(ウェイトなし)
/////////////////////////////////////////////////////////////////////////////
BYTE CPU6::ReadMemNW( WORD addr )
{
	return vm->MemRead( addr );
}


/////////////////////////////////////////////////////////////////////////////
// メモリアクセス(ウェイトあり)
/////////////////////////////////////////////////////////////////////////////
BYTE CPU6::ReadMem( WORD addr )
{
	return vm->MemRead( addr, &mstate );
}

void CPU6::WriteMem( WORD addr, BYTE data)
{
	vm->MemWrite( addr, data, &mstate );
}


/////////////////////////////////////////////////////////////////////////////
// I/Oポートアクセス
/////////////////////////////////////////////////////////////////////////////
BYTE CPU6::ReadIO( int addr )
{
	return vm->IomIn( addr, &mstate );
}

void CPU6::WriteIO( int addr, BYTE data )
{
	vm->IomOut( addr, data, &mstate );
}


/////////////////////////////////////////////////////////////////////////////
// 割込みチェック＆ベクタ取得
/////////////////////////////////////////////////////////////////////////////
int CPU6::GetIntrVector( void )
{
	return vm->IntIntrCheck();
}


/////////////////////////////////////////////////////////////////////////////
// バスリクエスト区間停止フラグ取得
/////////////////////////////////////////////////////////////////////////////
bool CPU6::IsBUSREQ( void )
{
	return
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		vm->IsMonitor() ? false :
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		vm->VdgIsBusReqStop();
}


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
/////////////////////////////////////////////////////////////////////////////
bool CPU6::DokoSave( cIni* Ini )
{
	if( !Ini ) return false;
	
	Ini->SetVal( "Z80", "AF",		"", "0x%04X",	AF.W    );
	Ini->SetVal( "Z80", "BC",		"", "0x%04X",	BC.W    );
	Ini->SetVal( "Z80", "DE",		"", "0x%04X",	DE.W    );
	Ini->SetVal( "Z80", "HL",		"", "0x%04X",	HL.W    );
	Ini->SetVal( "Z80", "IX",		"", "0x%04X",	IX.W    );
	Ini->SetVal( "Z80", "IY",		"", "0x%04X",	IY.W    );
	Ini->SetVal( "Z80", "PC",		"", "0x%04X",	PC.W    );
	Ini->SetVal( "Z80", "SP",		"", "0x%04X",	SP.W    );
	Ini->SetVal( "Z80", "AF1",		"", "0x%04X",	AF1.W   );
	Ini->SetVal( "Z80", "BC1",		"", "0x%04X",	BC1.W   );
	Ini->SetVal( "Z80", "DE1",		"", "0x%04X",	DE1.W   );
	Ini->SetVal( "Z80", "HL1",		"", "0x%04X",	HL1.W   );
	Ini->SetVal( "Z80", "I",		"", "0x%02X",	I       );
	Ini->SetVal( "Z80", "R",		"", "0x%02X",	R       );
	Ini->SetVal( "Z80", "R_saved",	"", "0x%02X",	R_saved );
	Ini->SetVal( "Z80", "IFF",		"",				IFF     );
	Ini->SetVal( "Z80", "IFF2",		"",				IFF2    );
	Ini->SetVal( "Z80", "IM",		"", "0x%02X",	IM      );
	Ini->SetVal( "Z80", "Halt",		"",				Halt    );
	
	Ini->SetVal( "Z80", "mstate",	"", mstate );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
/////////////////////////////////////////////////////////////////////////////
bool CPU6::DokoLoad( cIni* Ini )
{
	if( !Ini ) return false;
	
	Ini->GetVal( "Z80", "AF",		AF.W    );
	Ini->GetVal( "Z80", "BC",		BC.W    );
	Ini->GetVal( "Z80", "DE",		DE.W    );
	Ini->GetVal( "Z80", "HL",		HL.W    );
	Ini->GetVal( "Z80", "IX",		IX.W    );
	Ini->GetVal( "Z80", "IY",		IY.W    );
	Ini->GetVal( "Z80", "PC",		PC.W    );
	Ini->GetVal( "Z80", "SP",		SP.W    );
	Ini->GetVal( "Z80", "AF1",		AF1.W   );
	Ini->GetVal( "Z80", "BC1",		BC1.W   );
	Ini->GetVal( "Z80", "DE1",		DE1.W   );
	Ini->GetVal( "Z80", "HL1",		HL1.W   );
	Ini->GetVal( "Z80", "I",		I       );
	Ini->GetVal( "Z80", "R",		R       );
	Ini->GetVal( "Z80", "R_saved",	R_saved );
	Ini->GetVal( "Z80", "IFF",		IFF     );
	Ini->GetVal( "Z80", "IFF2",		IFF2    );
	Ini->GetVal( "Z80", "IM",		IM      );
	Ini->GetVal( "Z80", "Halt",		Halt    );
	
	Ini->GetVal( "Z80", "mstate",	mstate  );
	
	return true;
}
