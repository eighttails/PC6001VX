#include "p6el.h"
#include "log.h"
#include "status.h"
#include "cpus.h"
#include "disk.h"
#include "keyboard.h"
#include "osd.h"
#include "replay.h"


//------------------------------------------------------
//  ステータスバークラス
//------------------------------------------------------
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cWndStat::cWndStat( VM6 *pvm ) : vm(pvm), DrvNum(0), ReplayStatus(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cWndStat::~cWndStat( void ){}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool cWndStat::Init( int w, int drv )
{
	PRINTD( WIN_LOG, "[WndStat][Init]\n" );
	
	if( drv >= 0 ) DrvNum = drv;
	ZCons::SetColor( FC_WHITE, FC_GRAY );
	
	return ZCons::InitRes( w < 0 ? VSurface::Width() : w , JFont::FontHeight()*(DrvNum+1) + 4, NULL, FC_WHITE, FC_GRAY );
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
	if( vm->CmtlIsMount() ){
		ZCons::SetColor( vm->CmtlIsAutoStart() ? FC_YELLOW : FC_WHITE );
		ZCons::Printf( " %-16s", *vm->CmtlGetName() ? vm->CmtlGetName() : OSD_GetFileNamePart( vm->CmtlGetFile() ) );
		ZCons::SetColor( FC_WHITE );
		ZCons::Locate( ZCons::GetXline()-19, 0 );
		if( vm->CpusIsCmtIntrReady() == LOADOPEN ) ZCons::SetColor( FC_WHITE, FC_MAGENTA );
		ZCons::Printf( "[%05d/%05d]", vm->CmtlGetCount(), vm->CmtlGetSize() );
		ZCons::SetColor( FC_WHITE, FC_GRAY );
	}
	
	// DISK
	if( DrvNum > 0 ){
		if( vm->DskInAccess( 0 ) ) ZCons::SetColor( FC_WHITE, FC_RED );
		else					   ZCons::SetColor( FC_WHITE, FC_GRAY );
		ZCons::Locate( 0, 1 );
		ZCons::Printf( "[DRV1]" );
		if( vm->DskIsMount( 0 ) ){
			ZCons::SetColor( vm->DskIsSystem( 0 ) ? FC_YELLOW : FC_WHITE, vm->DskIsProtect( 0 ) ? FC_DRED : FC_GRAY );
			ZCons::Printf( " %-16s", *vm->DskGetName( 0 ) ? vm->DskGetName( 0 ) : OSD_GetFileNamePart( vm->DskGetFile( 0 ) ) );
		}
	}
	if( DrvNum > 1 ){
		if( vm->DskInAccess( 1 ) ) ZCons::SetColor( FC_WHITE, FC_RED );
		else					   ZCons::SetColor( FC_WHITE, FC_GRAY );
		ZCons::Locate( 0, 2 );
		ZCons::Printf( "[DRV2]" );
		if( vm->DskIsMount( 1 ) ){
			ZCons::SetColor( vm->DskIsSystem( 1 ) ? FC_YELLOW : FC_WHITE, vm->DskIsProtect( 1 ) ? FC_DRED : FC_GRAY );
			ZCons::Printf( " %-16s", *vm->DskGetName( 1 ) ? vm->DskGetName( 1 ) : OSD_GetFileNamePart( vm->DskGetFile( 1 ) ) );
		}
		// アクセスランプ
	}
	ZCons::SetColor( FC_WHITE, FC_GRAY );
	
	// かなキー
	ZCons::Locate( -5, 0 );
	switch( vm->KeyGetKeyIndicator() & 3 ){
	case KI_KANA:	// かな
		ZCons::PutCharH( Kana[0] );
		ZCons::PutCharH( Kana[1] );
		break;
	case KI_KKANA:	// カナ
		ZCons::PutCharH( KKana[0] );
		ZCons::PutCharH( KKana[1] );
	}
	
	// CAPSキー
	if( vm->KeyGetKeyIndicator() & 4 ) ZCons::Printfr( "ABC" );	// ABC
	else                               ZCons::Printfr( "abc" );	// abc
	
	// リプレイステータス
	ZCons::Locate( -2, 0 );
	switch( ReplayStatus ){
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


////////////////////////////////////////////////////////////////
// リプレイステータスセット
//
// 引数:	stat		リプレイステータス
// 返値:	なし
////////////////////////////////////////////////////////////////
void cWndStat::SetReplayStatus( int stat )
{
	ReplayStatus = stat;
}
