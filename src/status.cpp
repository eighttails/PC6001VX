/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "cpus.h"
#include "disk.h"
#include "keyboard.h"
#include "log.h"
#include "memory.h"
#include "osd.h"
#include "p6el.h"
#include "p6vm.h"
#include "replay.h"
#include "status.h"
#include "pc6001v.h"


//---------------------------------------------------------------------------
//  ステータスバークラス
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cWndStat::cWndStat( void ) : DrvNum( 0 ), ExCart( 0 )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cWndStat::~cWndStat( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool cWndStat::Init( int w, int drv, WORD cart )
{
	PRINTD( WIN_LOG, "[WndStat][Init]\n" );
	
	if( drv  >= 0      ){ DrvNum = drv;  }
	if( cart != 0xffff ){ ExCart = cart; }
	ZCons::SetColor( FC_WHITE4, FC_WHITE2 );
	
	return ZCons::InitRes( w < 0 ? VSurface::Width() : w, JFont::FontHeight() * (DrvNum + (ExCart ? 1 : 0) + 1) + 4, "", FC_WHITE4, FC_WHITE2 );
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウ更新
/////////////////////////////////////////////////////////////////////////////
void cWndStat::Update( EL6* el )
{
	PRINTD( WIN_LOG, "[WndStat][Update]\n" );
	
	const BYTE Kana[]  = { 0x96, 0xe5, 0 };	// かな
	const BYTE KKana[] = { 0xb6, 0xc5, 0 };	// カナ
	int yyy = 0;
	
	ZCons::Cls();
	
	// 拡張カートリッジ
	if( ExCart ){
		std::string str = el->vm->mem->GetExtCartName();
		if( (el->vm->mem->GetCartridge() & (EXCFIX | EXCROM)) == EXCROM && P6VPATH2STR( el->vm->mem->GetFile() ).length() ){
			str += " (" + OSD_GetFileNamePart( el->vm->mem->GetFile() ) + ")";
		}
		
		ZCons::Locate( 0, yyy++ );
		ZCons::SetColor( FC_WHITE4, FC_BLUE1 );
		ZCons::Printf( " CART  " );
		ZCons::SetColor( FC_WHITE4, FC_WHITE2 );
		ZCons::Printf( " %-16s", str.c_str() );
	}
	
	// DISK
	for( int i = 0; i < DrvNum; i++ ){
		ZCons::Locate( 0, yyy++ );
		if( el->vm->disk->InAccess( i ) ){ ZCons::SetColor( FC_WHITE4, FC_RED4 ); }
		else							 { ZCons::SetColor( FC_WHITE4, FC_RED1 ); }
		ZCons::Printf( " DISK%d ", i+1 );
		if( el->vm->disk->IsMount( i ) ){
			ZCons::SetColor( el->vm->disk->IsSystem( i ) ? FC_YELLOW4 : FC_WHITE4, el->vm->disk->IsProtect( i ) ? FC_RED2 : FC_WHITE2 );
			ZCons::Printf( " %-16s", el->vm->disk->GetName( i ).empty() ? OSD_GetFileNamePart( el->vm->disk->GetFile( i ) ).c_str() : el->vm->disk->GetName( i ).c_str() );
		}
	}
	
	// TAPE
	ZCons::Locate( 0, yyy );
	if( el->vm->cmtl->IsRelay() ){ ZCons::SetColor( FC_GREEN1, FC_GREEN4 ); }
	else						 { ZCons::SetColor( FC_WHITE4, FC_GREEN1 ); }
	ZCons::Printf( " TAPE  " );
	ZCons::SetColor( FC_WHITE3, FC_WHITE2 );
	if( el->vm->cmtl->IsMount() ){
		ZCons::SetColor( el->vm->cmtl->IsAutoStart() ? FC_YELLOW4 : FC_WHITE4, FC_WHITE2 );
		ZCons::Printf( " %-16s", el->vm->cmtl->GetName().empty() ? OSD_GetFileNamePart( el->vm->cmtl->GetFile() ).c_str() : el->vm->cmtl->GetName().c_str() );
		if( el->vm->cpus->IsCmtIntrReady() == LOADOPEN ){ ZCons::SetColor( FC_WHITE4, FC_MAGENTA4 ); }
		else											{ ZCons::SetColor( FC_WHITE4, FC_WHITE2   ); }
	}
	ZCons::Locate( yyy ? -1 : -7, yyy );
	ZCons::PrintfR( "%06d/%06d", el->vm->cmtl->GetCount(), el->vm->cmtl->GetBetaSize() );
	yyy++;
	
	
	// かなキー
	ZCons::Locate( -5, 0 );
	ZCons::SetColor( (el->vm->key->GetKeyIndicator() & KI_KANA) ? FC_WHITE4 : FC_WHITE3, FC_WHITE2 );
	if( el->vm->key->GetKeyIndicator() & KI_KKANA ){	// カナ
		ZCons::PutCharH( KKana[0] );
		ZCons::PutCharH( KKana[1] );
	}else{												// かな
		ZCons::PutCharH( Kana[0] );
		ZCons::PutCharH( Kana[1] );
	}
	
	// CAPSキー
	ZCons::Locate( -1, 0 );
	ZCons::SetColor( FC_WHITE4, FC_WHITE2 );
	if( el->vm->key->GetKeyIndicator() & KI_CAPS ){ ZCons::PrintfR( "ABC" ); }	// ABC
	else										  { ZCons::PrintfR( "abc" ); }	// abc
	
	// リプレイ，ビデオキャプチャ インジケータ
	ZCons::Locate( -1, 0 );
	switch( el->REPLAY::GetStatus() | (el->AVI6::IsAVI() ? ST_CAPTUREREC : ST_IDLE) ){
	case ST_REPLAYREC:					// リプレイ記録中
		ZCons::SetColor( FC_RED4 );
		ZCons::PrintfR( "●" );
		break;
		
	case ST_REPLAYPLAY:					// リプレイ再生中
		ZCons::SetColor( FC_GREEN4 );
		ZCons::PrintfR( "■" );
		break;
		
	case ST_CAPTUREREC:					// ビデオキャプチャ中
		ZCons::SetColor( FC_RED4 );
		ZCons::PrintfR( "◎" );
		break;
		
	case ST_REPLAYPLAY|ST_CAPTUREREC:	// リプレイ再生中＆ビデオキャプチャ中
		ZCons::SetColor( FC_RED4 );
		ZCons::PrintfR( "◆" );
		break;
	}
}
