#include "log.h"
#include "status.h"
#include "cpus.h"
#include "disk.h"
#include "keyboard.h"
#include "osd.h"
#include "replay.h"
#include "tape.h"


//------------------------------------------------------
//  ステータスバークラス
//------------------------------------------------------
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cWndStat::cWndStat( VM6 *vm, const P6ID& id ) : P6DEVICE(vm,id){}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cWndStat::~cWndStat( void ){}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
BOOL cWndStat::Init( int w )
{
	PRINTD( WIN_LOG, "[WndStat][Init]\n" );
	
	ZCons::SetColor( FC_WHITE, FC_GRAY );
	
	return ZCons::InitRes( w , JFont::FontHeight()*(vm->disk->GetDrives() ? 2 : 1) + 4, NULL, FC_WHITE, FC_GRAY );
}


////////////////////////////////////////////////////////////////
// ウィンドウ更新
////////////////////////////////////////////////////////////////
void cWndStat::Update( void )
{
	PRINTD( WIN_LOG, "[WndStat][Update]\n" );
	
	const BYTE Kana[]  = { 0x96, 0xe5, 0 };	// かな
	const BYTE KKana[] = { 0xb6, 0xc5, 0 };	// カナ
	
	ZCons::Cls();
	ZCons::SetColor( FC_WHITE );
	
	// TAPE
	ZCons::Locate( 0, 0 );
	ZCons::Printf( "[TAPE]" );
	if( vm->cmt->IsMount() ){
		ZCons::SetColor( vm->cmt->IsAutoStart() ? FC_YELLOW : FC_WHITE );
		if( *vm->cmt->GetName() )
			ZCons::Printf( "%-16s", vm->cmt->GetName() );
		else
			ZCons::Printf( "%-16s", OSD_GetFileNamePart( vm->cmt->GetFile() ) );
		ZCons::SetColor( FC_WHITE );
		ZCons::Locate( ZCons::GetXline()-19, 0 );
		if( vm->cpus->GetCmtStatus() == LOADOPEN ) ZCons::SetColor( FC_WHITE, FC_MAGENTA );
		ZCons::Printf( "[%05d/%05d]", vm->cmt->GetCount(), vm->cmt->GetSize() );
		ZCons::SetColor( FC_WHITE, FC_GRAY );
	}
	
	// DISK
	if( vm->disk->GetDrives() > 0 ){
		ZCons::Locate( 0, 1 );
		ZCons::Printf( "[DRV1]" );
		if( vm->disk->IsMount(0) ){
			ZCons::SetColor( vm->disk->IsSystem(0) ? FC_YELLOW : FC_WHITE, vm->disk->IsProtect(0) ? FC_DRED : FC_GRAY );
			if( *vm->disk->GetName(0) )
				ZCons::Printf( "%-13s", vm->disk->GetName(0) );
			else
				ZCons::Printf( "%-13s", OSD_GetFileNamePart( vm->disk->GetFile(0) ) );
		}
	}
	if( vm->disk->GetDrives() > 1 ){
		ZCons::Locate( 20, 1 );
		ZCons::Printf( "[DRV2]" );
		if( vm->disk->IsMount(1) ){
			ZCons::SetColor( vm->disk->IsSystem(1) ? FC_YELLOW : FC_WHITE, vm->disk->IsProtect(1) ? FC_DRED : FC_GRAY );
			if( *vm->disk->GetName(1) )
				ZCons::Printf( "%-13s", vm->disk->GetName(1) );
			else
				ZCons::Printf( "%-13s", OSD_GetFileNamePart( vm->disk->GetFile(1) ) );
		}
		// アクセスランプ
	}
	ZCons::SetColor( FC_WHITE, FC_GRAY );
	
	// かなキー
	ZCons::Locate( -5, 0 );
	switch( vm->key->GetKeyIndicator()&3 ){
	case KI_KANA:	// かな
		ZCons::PutCharH( Kana[0] );
		ZCons::PutCharH( Kana[1] );
		break;
	case KI_KKANA:	// カナ
		ZCons::PutCharH( KKana[0] );
		ZCons::PutCharH( KKana[1] );
	}
	
	// CAPSキー
	if( vm->key->GetKeyIndicator()&4 ) ZCons::Printfr( "ABC" );	// ABC
	else                               ZCons::Printfr( "abc" );	// abc
	
	// リプレイインジケータ
	ZCons::Locate( -2, 0 );
	switch( vm->REPLAY::GetStatus() ){
	case REP_RECORD:	// 記録中
		ZCons::SetColor( FC_RED );
		ZCons::Print( "●" );
		break;
	case REP_REPLAY:	// 再生中
		ZCons::SetColor( FC_GREEN );
		ZCons::Print( "■" );
		break;
	}
}
