#include "p6el.h"
#include "log.h"
#include "status.h"
#include "cpus.h"
#include "disk.h"
#include "keyboard.h"
#include "replay.h"
#include "tape.h"


//------------------------------------------------------
//  ステータスバークラス
//------------------------------------------------------
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cWndStat::cWndStat( VM6 *pvm ) : vm(pvm),TapeIsAutoStart(false),
	TapeIsOpen(false), TapeSize(0), TapeCount(0), DrvNum(0),
	KeyIndicator(0), ReplayStatus(0)
{
	INITARRAY( TapeName, '\0' );
	INITARRAY( DiskName[0], '\0' );
	INITARRAY( DiskName[1], '\0' );
	INITARRAY( DiskIsSystem, false );
	INITARRAY( DiskIsProtect, false );
	INITARRAY( DiskIsAccess, false );
}


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
	
	
	TapeIsOpen   = ( vm->cpus->GetCmtStatus() == LOADOPEN );
	TapeCount    = vm->cmtl->GetCount();
	KeyIndicator = vm->key->GetKeyIndicator();
	
	ZCons::Cls();
	ZCons::SetColor( FC_WHITE );
	
	// TAPE
	ZCons::Locate( 0, 0 );
	ZCons::Printf( "[TAPE]" );
	if( strlen( TapeName ) ){
		ZCons::SetColor( TapeIsAutoStart ? FC_YELLOW : FC_WHITE );
		ZCons::Printf( " %-16s", TapeName );
		ZCons::SetColor( FC_WHITE );
		ZCons::Locate( ZCons::GetXline()-19, 0 );
		if( TapeIsOpen ) ZCons::SetColor( FC_WHITE, FC_MAGENTA );
		ZCons::Printf( "[%05d/%05d]", TapeCount, TapeSize );
		ZCons::SetColor( FC_WHITE, FC_GRAY );
	}
	
	// DISK
	if( DrvNum > 0 ){
		if( vm->disk->InAccess( 0 ) ) ZCons::SetColor( FC_WHITE, FC_RED );
		else						  ZCons::SetColor( FC_WHITE, FC_GRAY );
		ZCons::Locate( 0, 1 );
		ZCons::Printf( "[DRV1]" );
		if( strlen( DiskName[0] ) ){
			ZCons::SetColor( DiskIsSystem[0] ? FC_YELLOW : FC_WHITE, DiskIsProtect[0] ? FC_DRED : FC_GRAY );
			ZCons::Printf( " %-16s", DiskName[0] );
		}
	}
	if( DrvNum > 1 ){
		if( vm->disk->InAccess( 1 ) ) ZCons::SetColor( FC_WHITE, FC_RED );
		else						  ZCons::SetColor( FC_WHITE, FC_GRAY );
		ZCons::Locate( 0, 2 );
		ZCons::Printf( "[DRV2]" );
		if( strlen( DiskName[1] ) ){
			ZCons::SetColor( DiskIsSystem[1] ? FC_YELLOW : FC_WHITE, DiskIsProtect[1] ? FC_DRED : FC_GRAY );
			ZCons::Printf( " %-16s", DiskName[1] );
		}
		// アクセスランプ
	}
	ZCons::SetColor( FC_WHITE, FC_GRAY );
	
	// かなキー
	ZCons::Locate( -5, 0 );
	switch( KeyIndicator&3 ){
	case KI_KANA:	// かな
		ZCons::PutCharH( Kana[0] );
		ZCons::PutCharH( Kana[1] );
		break;
	case KI_KKANA:	// カナ
		ZCons::PutCharH( KKana[0] );
		ZCons::PutCharH( KKana[1] );
	}
	
	// CAPSキー
	if( KeyIndicator&4 ) ZCons::Printfr( "ABC" );	// ABC
	else                 ZCons::Printfr( "abc" );	// abc
	
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
// TAPE マウント
//
// 引数:	filename	ファイル名
//			astart		オートスタート?
//			size		サイズ
// 返値:	なし
////////////////////////////////////////////////////////////////
void cWndStat::TapeMount( char *filename, bool astart, int size )
{
	strcpy( TapeName, filename );
	TapeIsAutoStart = astart;
	TapeSize        = size;
}


////////////////////////////////////////////////////////////////
// TAPE アンマウント
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void cWndStat::TapeUnmount( void )
{
	*TapeName = '\0';
	TapeIsAutoStart = false;
	TapeIsOpen      = false;
	TapeSize        = 0;
	TapeCount       = 0;
}


////////////////////////////////////////////////////////////////
// DISK マウント
//
// 引数:	drv			ドライブ番号
//			filename	ファイル名
//			sys			システムディスク?
//			prot		プロテクト?
// 返値:	なし
////////////////////////////////////////////////////////////////
void cWndStat::DiskMount( int drv, char *filename, bool sys, bool prot )
{
	strcpy( DiskName[drv], filename );
	DiskIsSystem[drv]  = sys;
	DiskIsProtect[drv] = prot;
}


////////////////////////////////////////////////////////////////
// DISK アンマウント
//
// 引数:	drv			ドライブ番号
// 返値:	なし
////////////////////////////////////////////////////////////////
void cWndStat::DiskUnmount( int drv )
{
	*DiskName[drv] = '\0';
	DiskIsSystem[drv]  = false;
	DiskIsProtect[drv] = false;
	DiskIsAccess[drv]  = false;
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
