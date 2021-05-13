#include "pc6001v.h"

#include "config.h"
#include "cpum.h"

#include "p6vm.h"

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
CPU6::CPU6( VM6 *vm, const ID& id ) : Device(vm,id) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
CPU6::~CPU6( void ){}


////////////////////////////////////////////////////////////////
// フェッチ(M1)
////////////////////////////////////////////////////////////////
BYTE CPU6::Fetch( WORD addr, int *m1wait )
{
	return vm->MemFetch( addr, m1wait );
}


////////////////////////////////////////////////////////////////
// メモリアクセス(ウェイトなし)
////////////////////////////////////////////////////////////////
BYTE CPU6::ReadMemNW( WORD addr )
{
	return vm->MemRead( addr );
}


////////////////////////////////////////////////////////////////
// メモリアクセス(ウェイトあり)
////////////////////////////////////////////////////////////////
BYTE CPU6::ReadMem( WORD addr )
{
	return vm->MemRead( addr, &mstate );
}

void CPU6::WriteMem( WORD addr, BYTE data)
{
	vm->MemWrite( addr, data, &mstate );
}


////////////////////////////////////////////////////////////////
// I/Oポートアクセス
////////////////////////////////////////////////////////////////
BYTE CPU6::ReadIO( int addr )
{
	return vm->IomIn( addr, &mstate );
}

void CPU6::WriteIO( int addr, BYTE data )
{
	vm->IomOut( addr, data, &mstate );
}


////////////////////////////////////////////////////////////////
// 割込みベクタ取得
////////////////////////////////////////////////////////////////
int CPU6::GetIntrVector( void )
{
	return vm->IntIntrCheck();
}


////////////////////////////////////////////////////////////////
// バスリクエスト区間停止フラグ取得
////////////////////////////////////////////////////////////////
bool CPU6::IsBUSREQ( void )
{
	return
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			vm->ElIsMonitor() ? false :
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			vm->VdgIsBusReqStop();
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
bool CPU6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "Z80", NULL, "AF",		"0x%04X",	AF.W );
	Ini->PutEntry( "Z80", NULL, "BC",		"0x%04X",	BC.W );
	Ini->PutEntry( "Z80", NULL, "DE",		"0x%04X",	DE.W );
	Ini->PutEntry( "Z80", NULL, "HL",		"0x%04X",	HL.W );
	Ini->PutEntry( "Z80", NULL, "IX",		"0x%04X",	IX.W );
	Ini->PutEntry( "Z80", NULL, "IY",		"0x%04X",	IY.W );
	Ini->PutEntry( "Z80", NULL, "PC",		"0x%04X",	PC.W );
	Ini->PutEntry( "Z80", NULL, "SP",		"0x%04X",	SP.W );
	Ini->PutEntry( "Z80", NULL, "AF1",		"0x%04X",	AF1.W );
	Ini->PutEntry( "Z80", NULL, "BC1",		"0x%04X",	BC1.W );
	Ini->PutEntry( "Z80", NULL, "DE1",		"0x%04X",	DE1.W );
	Ini->PutEntry( "Z80", NULL, "HL1",		"0x%04X",	HL1.W );
	Ini->PutEntry( "Z80", NULL, "I",		"0x%02X",	I );
	Ini->PutEntry( "Z80", NULL, "R",		"0x%02X",	R );
	Ini->PutEntry( "Z80", NULL, "R_saved",	"0x%02X",	R_saved );
	Ini->PutEntry( "Z80", NULL, "IFF",		"0x%02X",	IFF );
	Ini->PutEntry( "Z80", NULL, "IFF2",		"0x%02X",	IFF2 );
	Ini->PutEntry( "Z80", NULL, "IM",		"0x%02X",	IM );
	Ini->PutEntry( "Z80", NULL, "Halt",		"0x%02X",	Halt );
	
	Ini->PutEntry( "Z80", NULL, "mstate",	"%d",	mstate );
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
bool CPU6::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt( "Z80", "AF",		&st,	AF.W );		AF.W = st;
	Ini->GetInt( "Z80", "BC",		&st,	BC.W );		BC.W = st;
	Ini->GetInt( "Z80", "DE",		&st,	DE.W );		DE.W = st;
	Ini->GetInt( "Z80", "HL",		&st,	HL.W );		HL.W = st;
	Ini->GetInt( "Z80", "IX",		&st,	IX.W );		IX.W = st;
	Ini->GetInt( "Z80", "IY",		&st,	IY.W );		IY.W = st;
	Ini->GetInt( "Z80", "PC",		&st,	PC.W );		PC.W = st;
	Ini->GetInt( "Z80", "SP",		&st,	SP.W );		SP.W = st;
	Ini->GetInt( "Z80", "AF1",		&st,	AF1.W );	AF1.W = st;
	Ini->GetInt( "Z80", "BC1",		&st,	BC1.W );	BC1.W = st;
	Ini->GetInt( "Z80", "DE1",		&st,	DE1.W );	DE1.W = st;
	Ini->GetInt( "Z80", "HL1",		&st,	HL1.W );	HL1.W = st;
	Ini->GetInt( "Z80", "I",		&st,	I );		I = st;
	Ini->GetInt( "Z80", "R",		&st,	R );		R = st;
	Ini->GetInt( "Z80", "R_saved",	&st,	R_saved );	R_saved = st;
	Ini->GetInt( "Z80", "IFF",		&st,	IFF );		IFF = st;
	Ini->GetInt( "Z80", "IFF2",		&st,	IFF2 );		IFF2 = st;
	Ini->GetInt( "Z80", "IM",		&st,	IM );		IM = st;
	Ini->GetInt( "Z80", "Halt",		&st,	Halt );		Halt = st;
	
	Ini->GetInt( "Z80", "mstate",	&mstate,	mstate );
	
	return true;
}
