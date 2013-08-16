#include "config.h"
#include "log.h"
#include "cpum.h"
#include "breakpoint.h"
#include "intr.h"
#include "io.h"
#include "memory.h"
#include "vdg.h"

#include "p6el.h"

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
CPU6::CPU6( VM6 *vm, const P6ID& id ) : P6DEVICE(vm,id) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
CPU6::~CPU6( void ){}


////////////////////////////////////////////////////////////////
// フェッチ(M1)
////////////////////////////////////////////////////////////////
inline BYTE CPU6::Fetch( WORD addr, int *m1wait )
{
	BYTE data = vm->mem->Fetch( addr, m1wait );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	// ブレークポイントチェック
	if( vm->bp->CheckBreakPoint( BPoint::BP_READ, addr, data, NULL ) ){
		PRINTD( MEM_LOG, " -> Break!" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	PRINTD( MEM_LOG, "\n" );
	
	return data;
}


////////////////////////////////////////////////////////////////
// メモリアクセス(ウェイトなし)
////////////////////////////////////////////////////////////////
inline BYTE CPU6::ReadMemNW( WORD addr )
{
	BYTE data = vm->mem->Read( addr );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	// ブレークポイントチェック
	if( vm->bp->CheckBreakPoint( BPoint::BP_READ, addr, data, NULL ) ){
		PRINTD( MEM_LOG, " -> Break!" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	PRINTD( MEM_LOG, "\n" );
	
	return data;
}


////////////////////////////////////////////////////////////////
// メモリアクセス(ウェイトあり)
////////////////////////////////////////////////////////////////
inline BYTE CPU6::ReadMem( WORD addr )
{
	BYTE data = vm->mem->Read( addr, &mstate );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	// ブレークポイントチェック
	if( vm->bp->CheckBreakPoint( BPoint::BP_READ, addr, data, NULL ) ){
		PRINTD( MEM_LOG, " -> Break!" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	PRINTD( MEM_LOG, "\n" );
	
	return data;
}

inline void CPU6::WriteMem( WORD addr, BYTE data)
{
	vm->mem->Write( addr, data, &mstate );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	// ブレークポイントチェック
	if( vm->bp->CheckBreakPoint( BPoint::BP_WRITE, addr, data, NULL ) ){
		PRINTD( MEM_LOG, " -> Break!" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	PRINTD( MEM_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// I/Oポートアクセス
////////////////////////////////////////////////////////////////
inline BYTE CPU6::ReadIO( int addr )
{
	BYTE data = vm->iom->In( addr, &mstate );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	// ブレークポイントチェック
	if( vm->bp->CheckBreakPoint( BPoint::BP_IN, addr&0xff, data, NULL ) ){
		PRINTD( IO_LOG, " -> Break!" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	PRINTD( IO_LOG, "\n" );
	
	return data;
}

inline void CPU6::WriteIO( int addr, BYTE data )
{
	vm->iom->Out( addr, data, &mstate );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	// ブレークポイントチェック
	if( vm->bp->CheckBreakPoint( BPoint::BP_OUT, addr&0xff, data, NULL ) ){
		PRINTD( IO_LOG, " -> Break!" );
	}
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	PRINTD( IO_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// 割込みベクタ取得
////////////////////////////////////////////////////////////////
inline int CPU6::GetIntrVector( void )
{
	return vm->intr->IntrCheck();
}


////////////////////////////////////////////////////////////////
// BUSREQ取得
////////////////////////////////////////////////////////////////
inline bool CPU6::IsBUSREQ( void )
{
	return vm->el->IsMonitor() ? false : vm->vdg->IsDisp();
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
