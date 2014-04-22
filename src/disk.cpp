#include <stdlib.h>
#include <string.h>
#include <new>

#include "p6el.h"
#include "log.h"
#include "disk.h"
#include "error.h"
#include "osd.h"
#include "schedule.h"

#include "p6vm.h"
// イベントID
// --- mini FDD ---
#define EID_INIT1	(1)		// 00h イニシャライズ(ドライブ1)
#define EID_INIT2	(2)		// 00h イニシャライズ(ドライブ2)
#define EID_WRDATEX	(21)	// 01h ライト データ実行
#define EID_RDDATEX	(22)	// 02h リード データ実行
#define EID_GETPAR	(30)	// パラメータ受信


// --- FDC ---
#define	EID_SEEK1	(101)
#define	EID_SEEK2	(102)
#define	EID_SEEK3	(103)
#define	EID_SEEK4	(104)
#define	EID_EXWAIT	(105)


//************* Wait (us) *************
// --- mini FDD ---
// この辺 よく分からなので超てけとー
#define WFDD_INIT			(500000)	// 00h イニシャライズ
#define WFDD_WRDAT			(100)		// 01h ライト データ
#define WFDD_RDDAT			(100)		// 02h リード データ
#define WFDD_SDDAT			(100)		// 03h センド データ
#define WFDD_FORMAT			(100)		// 05h フォーマット
#define WFDD_SDRES			(100)		// 06h センド リザルト ステータス
#define WFDD_SDDRV			(100)		// 07h センド ドライブ ステータス
#define WFDD_GETPAR			(100)		// パラメータ受信
#define WFDD_SEEK			(13000)		// とりあえずSRT=13


// --- FDC ---
#define WAIT_SEEK			(1000*2)	// クロック4MHzなので2倍する
#define WAIT_BYTE			(32)
#define WAIT_TRACK			(WAIT_BYTE*6250)
#define WAIT_2MS			(2000)

static const int Gap3size[] = {  26,  54,  84, 116,  150,  186,  224,  264 };
static const int Gap4size[] = { 488, 152, 182,  94, 1584, 1760, 2242, 4144 };

#define WAIT_GAP0			(WAIT_BYTE*( 80+12+(3+1)+50 ))

#define WAIT_ID				(WAIT_BYTE*( 12+(3+1)+4+2+22 ))
#define WAIT_DATA(n)		(WAIT_BYTE*( 12+(3+1)+(128<<(n))+2+Gap3size[(n)&7] ))
#define WAIT_SECTOR(n)		(WAIT_ID + WAIT_DATA(n))

#define WAIT_RID_ID			(WAIT_BYTE*( 12+(3+1)+4+2 ))
#define WAIT_RID_DATA(n)	(WAIT_BYTE*( 22 + 12+(3+1)+(128<<(n))+2+Gap3size[(n)&7] ))

#define WAIT_RDT_ID			(WAIT_BYTE*( 12+(3+1)+4+2+22 + 12+(3+1) ))
#define WAIT_RDT_DATA(n)	(WAIT_BYTE*( (128<<(n))+2+Gap3size[(n)&7] ))

#define WAIT_GAP3(n)		(WAIT_BYTE*( Gap3size[(n)&7] ))
#define WAIT_GAP4(n)		(WAIT_BYTE*( Gap4size[(n)&7] ))




////////////////////////////////////////////////////////////////
// ディスク 基底クラス
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
DSK6::DSK6( VM6 *vm, const ID& id ) : Device(vm,id), UType(PC6031), DrvNum(0)
{
	for( int i=0; i<MAXDRV; i++ ){
		ZeroMemory( FilePath[i], PATH_MAX );
		Dimg[i]  = NULL;
		Sys[i]   = false;
		DDDrv[i] = false;
	}
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSK6::~DSK6( void )
{
	for( int i=0; i<DrvNum; i++ )
		if( Dimg[i] ) Unmount( i );
}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
void DSK6::EventCallback( int id, int clock ){}


////////////////////////////////////////////////////////////////
// ウェイトカウンタリセット
////////////////////////////////////////////////////////////////
void DSK6::ResetWait( void )
{
	waitcnt = 0;
}


////////////////////////////////////////////////////////////////
// ウェイトカウンタ加算
////////////////////////////////////////////////////////////////
void DSK6::AddWait( int w )
{
	waitcnt += w;
}


////////////////////////////////////////////////////////////////
// ウェイト設定
////////////////////////////////////////////////////////////////
bool DSK6::SetWait( int eid )
{
	PRINTD( DISK_LOG, "[DISK][SetWait] %dus ->", waitcnt );
	
	if( waitcnt && vm->EventAdd( this, eid, waitcnt, EV_US ) ){
		waitcnt = 0;
		PRINTD( DISK_LOG, "OK\n" );
		return true;
	}else{
		waitcnt = 0;
		PRINTD( DISK_LOG, "FALSE\n" );
		return false;
	}
}


////////////////////////////////////////////////////////////////
// DISK マウント
////////////////////////////////////////////////////////////////
bool DSK6::Mount( int drvno, const char *filename )
{
	PRINTD( DISK_LOG, "[DISK][Mount] Drive : %d\n", drvno );
	
	if( drvno >= DrvNum ) return false;
	
	// もしマウント済みであればアンマウントする
	if( Dimg[drvno] ) Unmount( drvno );
	
	// ディスクイメージオブジェクトを確保
	try{
		Dimg[drvno] = new cD88( DDDrv[drvno] );
		if( !Dimg[drvno]->Init( filename ) ) throw Error::DiskMountFailed;
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		
		Unmount( drvno );
		return false;
	}
	
	// ファイルパス保存
	strncpy( FilePath[drvno], filename, PATH_MAX );
	
	// システムディスクチェック
	Dimg[drvno]->Seek( 0 );
	
	Sys[drvno] = false;
	const char IDS[][4] = { "SYS", "RXR", "IPL" };
	char idstr[4];
	for( int i=0; i<3; i++ )
		idstr[i] = Dimg[drvno]->Get8();
	for( int i=0; i<3; i++ ){
		if( !strncmp( idstr, IDS[i], 3 ) ){
			Sys[drvno] = true;
			break;
		}
	}
	Dimg[drvno]->Seek( 0 );	// 念のため
	
	return true;
}


////////////////////////////////////////////////////////////////
// DISK アンマウント
////////////////////////////////////////////////////////////////
void DSK6::Unmount( int drvno )
{
	PRINTD( DISK_LOG, "[DISK][Unmount] Drive : %d\n", drvno );
	
	if( drvno >= DrvNum ) return;
	
	if( Dimg[drvno] ){
		// ディスクイメージオブジェクトを開放
		delete Dimg[drvno];
		Dimg[drvno] = NULL;
		*FilePath[drvno] = '\0';
		Sys[drvno] = false;
	}
}


////////////////////////////////////////////////////////////////
// ドライブ数取得
////////////////////////////////////////////////////////////////
int DSK6::GetDrives( void )
{
	return DrvNum;
}


////////////////////////////////////////////////////////////////
// マウント済み?
////////////////////////////////////////////////////////////////
bool DSK6::IsMount( int drvno ) const
{
	if( drvno < DrvNum ) return Dimg[drvno] ? true : false;
	else                 return false;
}


////////////////////////////////////////////////////////////////
// システムディスク?
////////////////////////////////////////////////////////////////
bool DSK6::IsSystem( int drvno ) const
{
	return Sys[drvno];
}


////////////////////////////////////////////////////////////////
// プロテクト?
////////////////////////////////////////////////////////////////
bool DSK6::IsProtect( int drvno ) const
{
	if( !IsMount( drvno ) ) return false;
	
	return Dimg[drvno]->IsProtect();
}


////////////////////////////////////////////////////////////////
// ファイルパス取得
////////////////////////////////////////////////////////////////
const char *DSK6::GetFile( int drvno ) const
{
	return FilePath[drvno];
}


////////////////////////////////////////////////////////////////
// DISK名取得
////////////////////////////////////////////////////////////////
const char *DSK6::GetName( int drvno ) const
{
	if( !IsMount( drvno ) ) return "";
	
	return Dimg[drvno]->GetDiskImgName();
}












////////////////////////////////////////////////////////////////
// ミニフロッピーディスククラス
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
DSK60::DSK60( VM6 *vm, const ID& id ) :
	DSK6(vm,id), io_D1H(0), io_D2H(0x08)
{
	INITARRAY( RBuf, 0 );
	INITARRAY( WBuf, 0 );
	UType = PC6031;
}

DSK64::DSK64( VM6 *vm, const ID& id ) :	DSK60(vm,id)
{
	UType = PC6031SR;
	for( int i=0; i<MAXDRV; i++ )
		DDDrv[i] = true;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSK60::~DSK60( void ){}
DSK64::~DSK64( void ){}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
void DSK60::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_INIT1:		// 00h INITIALIZE(Drive1)
		PRINTD( DISK_LOG, "<< [DISK][EventCallback] EID_INIT1 >>\n" );
		if( DrvNum > 1 ){
			mdisk.busy = 2;
			// ウェイト加算
			DSK6::ResetWait();
			DSK6::AddWait( WFDD_INIT );
			DSK6::SetWait( EID_INIT2 );
			break;
		}
	case EID_INIT2:		// 00h INITIALIZE(Drive2)
		PRINTD( DISK_LOG, "<< [DISK][EventCallback] EID_INIT2 >>\n" );
		if( !(io_D2H&0x10) ){
			// DAVが立っていなければ待ち
			DSK6::ResetWait();
			DSK6::AddWait( WFDD_INIT );
			DSK6::SetWait( EID_INIT2 );
		}else{
			mdisk.busy = 0;
			mdisk.RFD  = true;
			mdisk.DAC  = true;
		}
		break;
		
	case EID_WRDATEX:	// 01h WRITE DATA 実行
		PRINTD( DISK_LOG, "<< [DISK][EventCallback] EID_WRDATEX >>\n" );
		mdisk.busy = 0;
		mdisk.RFD  = true;
		mdisk.DAC  = true;
		break;
		
	case EID_RDDATEX:	// 02h READ DATA 実行
		PRINTD( DISK_LOG, "<< [DISK][EventCallback] EID_RDDATEX >>\n" );
		mdisk.busy = 0;
		mdisk.RFD  = true;
		mdisk.DAC  = true;
		break;
		
	case EID_GETPAR:	// パラメータ受信
		PRINTD( DISK_LOG, "<< [DISK][EventCallback] EID_GETPAR >>\n" );
		mdisk.RFD = true;
		mdisk.DAC = true;
		break;
		
	default:;
	}
}


////////////////////////////////////////////////////////////////
// DISK処理 初期化
////////////////////////////////////////////////////////////////
bool DSK60::Init( int num )
{
	PRINTD( DISK_LOG, "[DISK][Init]\n" );
	
	DrvNum = max( min( num, MAXDRV ) , 0 );
	Reset();
	
	return true;
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void DSK60::Reset( void )
{
	PRINTD( DISK_LOG, "[DISK][Reset]\n" );
	
	ZeroMemory( &mdisk, sizeof( DISK60 ) );
	mdisk.Type     = FDD1D;	// ドライブタイプ
	mdisk.command  = IDLE;	// 受け取ったコマンド
	mdisk.retdat   = 0xff;	// port D0H から返す値
	mdisk.error    = false;	// エラーフラグ降ろす
	mdisk.Fast     = false;
	mdisk.FastStat = false;
	
	io_D1H = 0;
	io_D2H = 0x08;
	
	INITARRAY( RBuf, 0 );
	INITARRAY( WBuf, 0 );
}


////////////////////////////////////////////////////////////////
// アクセス中?
////////////////////////////////////////////////////////////////
bool DSK60::InAccess( int drvno ) const
{
	return ( mdisk.busy == ( drvno + 1 ) ) ? true : false;
}


////////////////////////////////////////////////////////////////
// DISKユニットからのデータ入力 (port D0H)
////////////////////////////////////////////////////////////////
BYTE DSK60::FddIn( void )
{
	PRINTD( DISK_LOG, "[DISK][FddIn]  <- " );
	
	mdisk.retdat = 0xff;
	
//	if( mdisk.DAV && mdisk.step != 0 ){		// コマンド処理中でデータが有効な場合

	// コマンド処理中でデータが有効な場合
	// または高速アクセス2バイト目の場合，DAVが立っていなくても読む
	// ホントちゃんとDAVの処理が必要だと思う
	if( (mdisk.DAV || ( mdisk.Fast && mdisk.FastStat )) && mdisk.step != 0 ){
		switch( mdisk.command ){
		case FAST_SEND_DATA:		// 12h FAST SEND DATA
			PRINTD( DISK_LOG, "FAST_" );
			mdisk.FastStat = mdisk.FastStat ? false : true;
			
		case SEND_DATA:				// 03h SEND DATA
			PRINTD( DISK_LOG, "SEND_DATA" );
			// バッファから読む
			mdisk.retdat = RBuf[mdisk.ridx++];
			if( mdisk.ridx >= mdisk.rsize ){
				mdisk.rsize    = 0;
				mdisk.ridx     = 0;
				mdisk.command  = IDLE;
				mdisk.step     = 0;
				mdisk.Fast     = false;
				mdisk.FastStat = false;
			}
			break;
			
		case SEND_RESULT_STATUS:	// 06h SEND RESULT STATUS
			PRINTD( DISK_LOG, "SEND_RESULT_STATUS" );
			//	Bit7:I/O動作終了フラグ
			//	Bit6:読込みバッファにデータ 有:1 無:0
			//	Bit5-1:-
			//	Bit0:エラー有:1 無:0
			
			// Bit7はよくわからないので後回し
			mdisk.retdat = (mdisk.rsize ? 0x40 : 0) | (mdisk.error ? 1 : 0);
			break;
			
		case SEND_DRIVE_STATUS:		// 07h SEND DRIVE STATUS
			PRINTD( DISK_LOG, "SEND_DRIVE_STATUS" );
			mdisk.retdat = 0xf0;
			for( int i=DrvNum; i>0; i-- )
				mdisk.retdat |= 1<<(4+i);
			break;
			
			
		case GET_MEMORY:			// 0Bh GET MEMORY
			// TO DO
			PRINTD( DISK_LOG, "GET_MEMORY" );
			// バッファから読む
			mdisk.retdat = RBuf[mdisk.ridx++];
			if( --mdisk.rsize == 0 ){
				mdisk.rsize    = 0;
				mdisk.ridx     = 0;
				mdisk.command  = IDLE;
				mdisk.step     = 0;
			}
			break;
			
		}
		PRINTD( DISK_LOG, ":%02X\n", mdisk.retdat );
		
		return mdisk.retdat;
	}
	else{			// データが無効な場合
		
		PRINTD( DISK_LOG, "FF\n" );
		
		return 0xff;
	}
}


////////////////////////////////////////////////////////////////
// DISKユニットへのコマンド，データ出力 (port D1H)
////////////////////////////////////////////////////////////////
void DSK60::FddOut( BYTE dat )
{
	PRINTD( DISK_LOG, "[DISK][FddOut]    -> %02X ", dat );
	
	int eid = EID_GETPAR;
	
	io_D1H = dat;
	mdisk.RFD = false;
	
	DSK6::ResetWait();
	DSK6::AddWait( WFDD_GETPAR );
	
	if( mdisk.command == IDLE ){	// コマンドの場合
		mdisk.command = dat;
		mdisk.error   = false;
		switch( mdisk.command ){
		case INITIALIZE:			// 00h INITIALIZE
			PRINTD( DISK_LOG, "[INITIALIZE]" );
			eid  = EID_INIT1;
			DSK6::AddWait( WFDD_INIT );
			
			mdisk.busy = 1;
			break;
			
		case WRITE_DATA:			// 01h WRITE DATA
			PRINTD( DISK_LOG, "[WRITE_DATA]" );
			mdisk.step  = 1;
			mdisk.wsize = 0;
			break;
			
		case READ_DATA:				// 02h READ DATA
			PRINTD( DISK_LOG, "[READ_DATA]" );
			mdisk.step  = 1;
			mdisk.rsize = 0;
			mdisk.ridx  = 0;
			break;
			
		case SEND_DATA:				// 03h SEND DATA
			PRINTD( DISK_LOG, "[SEND_DATA]" );
			mdisk.step = 1;
			break;
			
		case FORMAT:				// 05h FORMAT
			PRINTD( DISK_LOG, "[FORMAT]" );
			break;
			
		case SEND_RESULT_STATUS:	// 06h SEND RESULT STATUS
			PRINTD( DISK_LOG, "[SEND_RESULT_STATUS]" );
			mdisk.step = 1;
			break;
			
		case SEND_DRIVE_STATUS:		// 07h SEND DRIVE STATUS
			PRINTD( DISK_LOG, "[SEND_DRIVE_STATUS]" );
			mdisk.step = 1;
			break;
			
			
		case GET_MEMORY:			// 0Bh GET MEMORY
			if( UType == PC6031SR ){
				PRINTD( DISK_LOG, "[GET_MEMORY]" );
				mdisk.step = 1;
			}else{
				PRINTD( DISK_LOG, "[UNKNOWN]" );
				eid = 0;
			}
			break;
			
		case FAST_SEND_DATA:		// 12h FAST SEND DATA
			if( UType == PC6031SR ){
				PRINTD( DISK_LOG, "[FAST_SEND_DATA]" );
				mdisk.step     = 1;
				mdisk.Fast     = true;
				mdisk.FastStat = false;
			}else{
				PRINTD( DISK_LOG, "[UNKNOWN]" );
				eid = 0;
			}
			break;
			
		case SET_MODE:				// 17h SET MODE
			if( UType == PC6031SR ){
				PRINTD( DISK_LOG, "[SET_MODE]" );
				mdisk.step = 1;
			}else{
				PRINTD( DISK_LOG, "[UNKNOWN]" );
				eid = 0;
			}
			break;
			
		default:					// 無効コマンド
			PRINTD( DISK_LOG, "[UNKNOWN]" );
			eid = 0;
		}
	}else{					// データの場合
		switch( mdisk.command ){
		case WRITE_DATA:			// 01h WRITE DATA
			switch( mdisk.step ){
			case 1:	// 01h:転送ブロック数
				PRINTD( DISK_LOG, "<WRITE_DATA P1>" );
				mdisk.blk   = max( dat, 16 );
				mdisk.size  = mdisk.blk*256;
				mdisk.step++;
				break;
				
			case 2:	// 02h:ドライブ番号-1
				PRINTD( DISK_LOG, "<WRITE_DATA P2>" );
				mdisk.drv = dat;
				mdisk.step++;
				break;
				
			case 3:	// 03h:トラック番号
				PRINTD( DISK_LOG, "<WRITE_DATA P3>" );
				mdisk.trk = dat;
				mdisk.step++;
				break;
				
			case 4:	// 04h:セクタ番号
				PRINTD( DISK_LOG, "<WRITE_DATA P4>" );
				mdisk.sct = dat;
				mdisk.step++;
				break;
				
			case 5:	// 05h:データ書き込み
				PRINTD( DISK_LOG, "<WRITE_DATA P5>" );
				eid  = EID_WRDATEX;
				mdisk.busy = mdisk.drv + 1;
				
				WBuf[mdisk.wsize++] = dat;
				if( mdisk.wsize >= mdisk.size ){
					if( Dimg[mdisk.drv] ){
						// トラックNoを2倍(1D->2D)
						DSK6::AddWait( abs( Dimg[mdisk.drv]->Track() - mdisk.trk * 2 ) / 2 );
						// 目的のセクタへ移動
						if( Dimg[mdisk.drv]->Seek( mdisk.trk * 2 * ((DDDrv[mdisk.drv] && (mdisk.Type == FDD1D)) ? 2 : 1) ) ){
							if( Dimg[mdisk.drv]->SearchSector( mdisk.trk, 0, mdisk.sct, 1 ) ){
								switch( Dimg[mdisk.drv]->GetSecStatus() ){
								case BIOS_ID_CRC_ERROR:
								case BIOS_MISSING_DAM:
									DSK6::AddWait( WAIT_RID_ID );
									
								case BIOS_WRITE_PROTECT:
									mdisk.error = true;
									break;
									
								case BIOS_DATA_CRC_ERROR:
									// (ここでステータスを書換えてCRCエラーを解除する)
									
								case BIOS_READY:
								default:
									// バッファから書込む
									for( int i=0; i < mdisk.wsize; i++ )
										Dimg[mdisk.drv]->Put8( WBuf[i] );
									DSK6::AddWait( mdisk.blk * WAIT_SECTOR(1) );
								};
								
							}else{
								// 失敗したらエラーフラグ立てる
								mdisk.error = true;
								DSK6::AddWait( WAIT_TRACK*2 );
							}
						}else{
							// 失敗したらエラーフラグ立てる
							mdisk.error = true;
							DSK6::AddWait( WAIT_TRACK*2 );
						}
					}
					mdisk.step = 0;
				}
				break;
			}
			break;
			
		case READ_DATA:				// 02h READ DATA
			switch( mdisk.step ){
			case 1:	// 01h:転送ブロック数
				PRINTD( DISK_LOG, "<REAR_DATA P1>" );
				mdisk.blk  = max( dat, 16 );
				mdisk.size = mdisk.blk*256;
				mdisk.step++;
				break;
				
			case 2:	// 02h:ドライブ番号-1
				PRINTD( DISK_LOG, "<REAR_DATA P2>" );
				mdisk.drv = dat;
				mdisk.step++;
				break;
				
			case 3:	// 03h:トラック番号
				PRINTD( DISK_LOG, "<REAR_DATA P3>" );
				mdisk.trk = dat;
				mdisk.step++;
				break;
				
			case 4:	// 04h:セクタ番号
				PRINTD( DISK_LOG, "<REAR_DATA P4>" );
				mdisk.sct = dat;
				
				eid  = EID_RDDATEX;
				mdisk.busy = mdisk.drv + 1;
				
				if( Dimg[mdisk.drv] ){
					// トラックNoを2倍(1D->2D)
					DSK6::AddWait( abs( Dimg[mdisk.drv]->Track() - mdisk.trk * 2 ) / 2 );
					if( Dimg[mdisk.drv]->Seek( mdisk.trk * 2 * ((DDDrv[mdisk.drv] && (mdisk.Type == FDD1D)) ? 2 : 1) ) ){
						if( Dimg[mdisk.drv]->SearchSector( mdisk.trk, 0, mdisk.sct, 1 ) ){
							switch( Dimg[mdisk.drv]->GetSecStatus() ){
							case BIOS_ID_CRC_ERROR:
							case BIOS_MISSING_DAM:
								mdisk.error = true;
								DSK6::AddWait( WAIT_RID_ID );
								break;
								
							case BIOS_DATA_CRC_ERROR:
								mdisk.error = true;
							case BIOS_READY:
							default:
								// バッファに読込む
								for( mdisk.rsize = 0; mdisk.rsize < mdisk.size; mdisk.rsize++ )
									RBuf[mdisk.rsize] = Dimg[mdisk.drv]->Get8();
								DSK6::AddWait( mdisk.blk * WAIT_SECTOR(1) );
								mdisk.ridx = 0;
							};
						}else{
							// 失敗したらエラーフラグ立てる
							mdisk.error = true;
							DSK6::AddWait( WAIT_TRACK*2 );
						}
					}else
						// 失敗したらエラーフラグ立てる
						mdisk.error = true;
						DSK6::AddWait( WAIT_TRACK*2 );
				}
				mdisk.step = 0;
				break;
			}
			break;
			
		case GET_MEMORY:			// 0Bh GET MEMORY
			switch( mdisk.step ){
			case 1:	// 01h:転送アドレス上位
				PRINTD( DISK_LOG, "<GET_MEMORY P1>" );
				mdisk.ridx = dat<<8;
				mdisk.step++;
				break;
				
			case 2:	// 02h:転送アドレス下位
				PRINTD( DISK_LOG, "<GET_MEMORY P2>" );
				mdisk.ridx += dat;
				mdisk.step++;
				break;
				
			case 3:	// 03h:転送バイト数上位
				PRINTD( DISK_LOG, "<GET_MEMORY P3>" );
				mdisk.rsize = dat<<8;
				mdisk.step++;
				break;
				
			case 4:	// 04h:転送バイト数下位
				PRINTD( DISK_LOG, "<GET_MEMORY P4>" );
				// TO DO
				mdisk.rsize += dat;
				break;
			}
			break;
			
		case SET_MODE:				// 17h SET MODE
			switch( mdisk.step ){
			case 1:	// 01h:モード
				PRINTD( DISK_LOG, "<SET_MODE P1> %s", dat&0x0f ? "1DD" : "1D" );
				// TO DO
				if( dat&0x0f ) mdisk.Type = FDD1DD;
				else		   mdisk.Type = FDD1D;
				mdisk.step = 0;
				break;
			}
			break;
			
		}
	}
	
	PRINTD( DISK_LOG, "\n" );
	
	// ウェイト設定
	if( eid ) DSK6::SetWait( eid );
}


////////////////////////////////////////////////////////////////
// DISKユニットからの制御信号入力 (port D2H)
////////////////////////////////////////////////////////////////
BYTE DSK60::FddCntIn( void )
{
	PRINTD( DISK_LOG, "[DISK][FddCntIn]  <- %02X %s %s %s\n",
						(io_D2H&0xf0) | (mdisk.DAC ? 4 : 0) | (mdisk.RFD ? 2 : 0) | (mdisk.DAV ? 1 : 0),
						mdisk.DAC ? "DAC" : "", mdisk.RFD ? "RFD" : "", mdisk.DAV ? "DAV" : "" );
	
	io_D2H = (io_D2H&0xf0) | (mdisk.DAC ? 4 : 0) | (mdisk.RFD ? 2 : 0) | (mdisk.DAV ? 1 : 0);
	return io_D2H;
}


////////////////////////////////////////////////////////////////
// DISKユニットへの制御信号出力 (port D3H)
////////////////////////////////////////////////////////////////
void DSK60::FddCntOut( BYTE dat )
{
	PRINTD( DISK_LOG, "[DISK][FddCntOut] -> %02X ", dat );
	
	if( dat&0x80 ){		// 最上位bitチェック
						// 1の場合は8255のモード設定なので無視(必ずモード0と仮定する)
		PRINTD( DISK_LOG, "8255 mode set\n" );
		return;
	}
	
	switch( (dat>>1)&0x07 ){
	case 7:	// bit7 ATN
		PRINTD( DISK_LOG, "ATN:%d", dat&1 );
		if( (dat&1) && !(io_D2H&0x80) ){
			mdisk.RFD = true;
			mdisk.command = IDLE;
			PRINTD( DISK_LOG, " -> RFD:%d", mdisk.RFD ? 1 : 0 );
		}
		break;
		
	case 6:	// bit6 DAC
		PRINTD( DISK_LOG, "DAC:%d", dat&1 );
		if( (dat&1) && !(io_D2H&0x40) ){
			mdisk.DAV = false;
			PRINTD( DISK_LOG, " -> DAV:%d", mdisk.DAV ? 1 : 0 );
		}
		break;
		
	case 5:	// bit5 RFD
		PRINTD( DISK_LOG, "RFD:%d", dat&1 );
		if( (dat&1) && !(io_D2H&0x20) ){
			mdisk.DAV = true;
			PRINTD( DISK_LOG, " -> DAV:%d", mdisk.DAV ? 1 : 0 );
		}
		break;
		
	case 4:	// bit4 DAV
		PRINTD( DISK_LOG, "DAV:%d", dat&1 );
		if( !(dat&1) ){
			mdisk.DAC = false;
			PRINTD( DISK_LOG, " -> DAC:%d", mdisk.DAC ? 1 : 0 );
		}
		break;
	}
	
	if( dat&1 ) io_D2H |=   1<<((dat>>1)&0x07);
	else		io_D2H &= ~(1<<((dat>>1)&0x07));
	
	PRINTD( DISK_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
void DSK60::OutD1H( int, BYTE data ){ FddOut( data ); }
void DSK60::OutD2H( int, BYTE data ){ io_D2H = (data&0xf0) | (io_D2H&0x0f); }
void DSK60::OutD3H( int, BYTE data ){ FddCntOut( data ); }

BYTE DSK60::InD0H( int ){ return FddIn(); }
BYTE DSK60::InD1H( int ){ return io_D1H; }
BYTE DSK60::InD2H( int ){ return FddCntIn(); }










////////////////////////////////////////////////////////////////
// 66ディスククラス
////////////////////////////////////////////////////////////////

//************ FDC Status *******************
#define FDC_BUSY_D0					(0x01)
#define FDC_BUSY_D1					(0x02)
#define FDC_BUSY_D2					(0x04)
#define FDC_BUSY_D3					(0x08)
#define FDC_BUSY					(0x10)
#define FDC_NON_DMA					(0x20)
#define FDC_FD2PC					(0x40)
#define FDC_DATA_READY				(0x80)

//************* Result Status 0 *************
#define ST0_NOT_READY				(0x08)
#define ST0_EQUIP_CHK				(0x10)
#define ST0_SEEK_END				(0x20)
#define ST0_IC_NT					(0x00)
#define ST0_IC_AT					(0x40)
#define ST0_IC_IC					(0x80)
#define ST0_IC_AI					(0xc0)

//************* Result Status 1 *************
#define ST1_MISSING_AM				(0x01)
#define ST1_NOT_WRITABLE			(0x02)
#define ST1_NO_DATA					(0x04)
#define ST1_DATA_ERROR				(0x20)

//************* Result Status 2 *************
#define ST2_MA_IN_DATA_FIELD		(0x01)
#define ST2_BAD_CYLINDER			(0x02)
#define ST2_NO_CYLINDER				(0x10)
#define ST2_DE_IN_DATA_FIELD		(0x20)
#define ST2_CONTROL_MARK			(0x40)

//************* Result Status 3 *************
#define ST3_TRACK0					(0x10)
#define ST3_READY					(0x20)
#define ST3_WRITE_PROTECT			(0x40)
#define ST3_FAULT					(0x80)







////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
DSK66::DSK66( VM6 *vm, const ID& id ) : DSK6(vm,id),
	SendBytes(0), ExtDrv(false), B2Dir(false)
{
	INITARRAY( FDDBuf, 0 );
	UType = PC6601;
}

DSK68::DSK68( VM6 *vm, const ID& id ) :	DSK66(vm,id)
{
	UType = PC6601SR;
	for( int i=0; i<MAXDRV; i++ )
		DDDrv[i] = true;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSK66::~DSK66( void ){}
DSK68::~DSK68( void ){}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
void DSK66::EventCallback( int id, int clock )
{
	int Drv = 0xff;
	
	switch( id ){
	case EID_SEEK1: Drv = 0; PRINTD( FDC_LOG, "[DSK66][EventCallback] EID_SEEK Drv:%d\n", Drv ); break;
	case EID_SEEK2: Drv = 1; PRINTD( FDC_LOG, "[DSK66][EventCallback] EID_SEEK Drv:%d\n", Drv ); break;
	case EID_SEEK3: Drv = 2; PRINTD( FDC_LOG, "[DSK66][EventCallback] EID_SEEK Drv:%d\n", Drv ); break;
	case EID_SEEK4: Drv = 3; PRINTD( FDC_LOG, "[DSK66][EventCallback] EID_SEEK Drv:%d\n", Drv ); break;
	case EID_EXWAIT:
		fdc.Intr    = true;
		fdc.Status |= FDC_DATA_READY;
		break;
		
	default:;
	}
	
	if( Drv < 4 ){
		fdc.SeekSta[Drv] = SK_END;
		fdc.PCN[Drv]     = fdc.NCN[Drv];
		fdc.Intr         = true;
		fdc.Status      |= 1<<Drv;
	}
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool DSK66::Init( int num )
{
	DrvNum = max( min( num, MAXDRV ) , 0 );
	
	fdc.SRT = 5;		// Step Rate Time
	fdc.HUT = 240;		// Head Unloaded Time
	fdc.HLT = 38;		// Head Load Time
	fdc.ND  = false;	// true:Non DMAモード false:DMAモード
	
	Reset();
	
	return true;
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void DSK66::Reset( void )
{
	ZeroMemory( &CmdIn,  sizeof( CmdBuffer ) );
	ZeroMemory( &CmdOut, sizeof( CmdBuffer ) );
	for( int i=0; i < 4; i++ ){
		fdc.NCN[i]     = 0;
		fdc.PCN[i]     = 0;
		fdc.SeekSta[i] = SK_STOP;
	}
	fdc.US     = 0;
	fdc.Status = FDC_DATA_READY;
	fdc.Intr   = false;
	
	SendBytes  = 0;
	ExtDrv     = false;
	B2Dir      = false;
}


////////////////////////////////////////////////////////////////
// アクセス中?
////////////////////////////////////////////////////////////////
bool DSK66::InAccess( int drvno ) const
{
	return ( (fdc.SeekSta[drvno] == SK_SEEK) ||
			((fdc.Status & FDC_BUSY) && (fdc.US == drvno)) ) ? true : false;
}


////////////////////////////////////////////////////////////////
// FDDバッファ書込み
////////////////////////////////////////////////////////////////
void DSK66::BufWrite( int addr, BYTE data )
{
	FDDBuf[addr&0x3ff] = data;
}


////////////////////////////////////////////////////////////////
// FDDバッファ読込み
////////////////////////////////////////////////////////////////
BYTE DSK66::BufRead( int addr )
{
	return FDDBuf[addr&0x3ff];
}


////////////////////////////////////////////////////////////////
// ステータスバッファにデータを入れる
////////////////////////////////////////////////////////////////
void DSK66::PushStatus( BYTE data )
{
	PRINTD( FDC_LOG, "[DSK66][PushStatus] Index:%d ->%02X\n", CmdOut.Index, data );
	
	CmdOut.Data[CmdOut.Index++] = data;
}

////////////////////////////////////////////////////////////////
// ステータスバッファからデータを取り出す
////////////////////////////////////////////////////////////////
BYTE DSK66::PopStatus( void )
{
	PRINTD( FDC_LOG, "%02X\n", (BYTE)CmdOut.Data[CmdOut.Index-1] );
	
	if( CmdOut.Index ) --CmdOut.Index;
	return CmdOut.Data[CmdOut.Index];
}


////////////////////////////////////////////////////////////////
// FDC に書込む
////////////////////////////////////////////////////////////////
void DSK66::OutFDC( BYTE data )
{
	PRINTD( FDC_LOG, "[DSK66][OutFDC] Index:%d ->%02X\n", CmdIn.Index, data );
	
	const int CmdLength[] = { 0,0,9,3,2,9,9,2,1,0,0,0,0,6,0,3 };
	
	fdc.Intr = false;	// 読み書きするとキャンセルされるようだ?
	
	if( !(fdc.Status & FDC_FD2PC) ){
		CmdIn.Data[CmdIn.Index++] = data;
		
		// コマンド長のコマンドが来たら コマンド実行
		if( CmdLength[CmdIn.Data[0]&0xf] == CmdIn.Index ){
			fdc.Status = FDC_FD2PC | FDC_BUSY | ( fdc.Status & 0x0f );
			Exec();
		}else{
			fdc.Status = FDC_DATA_READY | FDC_BUSY | ( fdc.Status & 0x0f );
		}
	}
}


////////////////////////////////////////////////////////////////
// FDC から読込む
////////////////////////////////////////////////////////////////
BYTE DSK66::InFDC( void )
{
	PRINTD( FDC_LOG, "[DSK66][InFDC] Index:%d ->", CmdOut.Index-1 );
	
	fdc.Intr = false;	// 読み書きするとキャンセルされるようだ?
	
	if( fdc.Status & FDC_FD2PC ){
		if( CmdOut.Index == 1 ) fdc.Status = FDC_DATA_READY | ( fdc.Status & 0x0f );
		else					fdc.Status = FDC_DATA_READY | FDC_FD2PC | FDC_BUSY | ( fdc.Status & 0x0f );
		return PopStatus();
	}else{
		return 0xff;
	}
}


////////////////////////////////////////////////////////////////
// FDC コマンド実行
////////////////////////////////////////////////////////////////
void DSK66::Exec( void )
{
	PRINTD( FDC_LOG, "[DSK66][Exec] Command:%02X", CmdIn.Data[0]&0x1f );
	
	CmdOut.Index = 0;
	fdc.command = CmdIn.Data[0] & 0x1f;
	
	switch( fdc.command ){
	case 0x02:	// Read Diagnostic
		PRINTD( FDC_LOG, " Read Diagnostic\n" );
		ReadDiagnostic();
		break;
	case 0x03:	// Specify
		PRINTD( FDC_LOG, " Specify\n" );
		Specify();
		break;
		
	case 0x04:	// Sence Device Status
		PRINTD( FDC_LOG, " Sence Device Status\n" );
		break;
		
	case 0x05:	// Write Data
		PRINTD( FDC_LOG, " Write Data\n" );
		WriteData();
		break;
		
	case 0x06:	// Read Data
		PRINTD( FDC_LOG, " Read Data\n" );
		ReadData();
		break;
		
	case 0x07:	// Recalibrate
		PRINTD( FDC_LOG, " Recalibrate\n" );
		Recalibrate();
		break;
		
	case 0x08:	// Sense Interrupt Status
		PRINTD( FDC_LOG, " Sense Interrupt Status\n" );
		SenseInterruptStatus();
		break;
		
	case 0x09:	// Write Deleted Data
		PRINTD( FDC_LOG, " Write Deleted Data\n" );
		break;
		
	case 0x0a:	// Read ID
		PRINTD( FDC_LOG, " Read ID\n" );
		break;
		
	case 0x0c:	// Read Deleted Data
		PRINTD( FDC_LOG, " Read Deleted Data\n" );
		break;
		
	case 0x0d:	// Write ID
		PRINTD( FDC_LOG, " Write ID\n" );
		// Format is Not Implimented
		break;
		
	case 0x0f:	// Seek
		PRINTD( FDC_LOG, " Seek\n" );
		Seek();
		break;
		
	case 0x11:	// Scan Equal
		PRINTD( FDC_LOG, " Scan Equal\n" );
		break;
		
	case 0x19:	// Scan Low or Equal
		PRINTD( FDC_LOG, " Scan Low or Equal\n" );
		break;
		
	case 0x1d:	// Scan High or Equal
		PRINTD( FDC_LOG, " Scan High or Equal\n" );
		break;
		
	default:	// Invalid
		PRINTD( FDC_LOG, " Invalid!!\n" );
	}
	CmdIn.Index = 0;
}
////////////////////////////////////////////////////////////////
// セクタを探す
////////////////////////////////////////////////////////////////
bool DSK66::SearchSector( BYTE *sta )
{
	int idxcnt = 0;	// インデックスホール検出回数カウンタ
	
	PRINTD( FDC_LOG, "[DSK66][SearchSector] C:%d H:%d R:%d N:%d ", fdc.C, fdc.H, fdc.R, fdc.N );
	
	if( Dimg[fdc.US]->SecNum() ){	// 無効トラック(アンフォーマット)?
		while( 1 ){
			BYTE c0, h0, r0, n0;
			
			// 現在最終セクタ?
			if( Dimg[fdc.US]->Sector() == Dimg[fdc.US]->SecNum() ){
				PRINTD( FDC_LOG, "[Final]\n" );
				Dimg[fdc.US]->GetID( &c0, &h0, &r0, &n0 );
				DSK6::AddWait( WAIT_GAP4( n0 ) );
				if( ++idxcnt == 2 ) break;
				DSK6::AddWait( WAIT_GAP0 );
			}
			
			Dimg[fdc.US]->NextSector();
			DSK6::AddWait( WAIT_ID );
			
			Dimg[fdc.US]->GetID( &c0, &h0, &r0, &n0 );
			PRINTD( FDC_LOG, "Next C:%d H:%d R:%d N:%d ", c0, h0, r0, n0 );
			if( ( c0 == fdc.C ) && ( h0 == fdc.H ) && ( r0 == fdc.R ) && ( n0 == fdc.N ) ){
				*sta = Dimg[fdc.US]->GetSecStatus();
				
				// セクタのステータスをチェック
				switch( *sta ){
				case BIOS_NO_DATA:
					fdc.ST0 = ST0_IC_AT;
					fdc.ST1 = ST1_NO_DATA;
					fdc.ST2 = 0;
					if( fdc.C != fdc.PCN[fdc.US] ){
						fdc.ST2 |= ST2_NO_CYLINDER;
						if( fdc.C == 0xff ) fdc.ST2 |= ST2_BAD_CYLINDER;
					}
					break;
					
				case BIOS_MISSING_IAM:
					fdc.ST0 = ST0_IC_AT;
					fdc.ST1 = ST1_MISSING_AM;
					fdc.ST2 = 0;
					break;
					
				case BIOS_ID_CRC_ERROR:
					fdc.ST0 = ST0_IC_AT;
					fdc.ST1 = ST1_DATA_ERROR;
					fdc.ST2 = 0;
					break;
					
				case BIOS_MISSING_DAM:
					// Read Data or Read Deleted Data
					if( (fdc.command == 0x06) || (fdc.command == 0x0c) ){
						fdc.ST0 = ST0_IC_AT;
						fdc.ST1 = ST1_MISSING_AM;
						fdc.ST2 = ST2_MA_IN_DATA_FIELD;
					}else{
						fdc.ST0 = ST0_IC_NT;
						fdc.ST1 = 0;
						fdc.ST2 = 0;
					}
					break;
					
				case BIOS_DATA_CRC_ERROR:
					// Read Data or Read Deleted Data
					if( (fdc.command == 0x06) || (fdc.command == 0x0c) ){
						fdc.ST0 = ST0_IC_AT;
						fdc.ST1 = ST1_DATA_ERROR;
						fdc.ST2 = ST2_DE_IN_DATA_FIELD;
					}else{
						fdc.ST0 = ST0_IC_NT;
						fdc.ST1 = 0;
						fdc.ST2 = 0;
					}
					break;
					
				default:
					fdc.ST0 = ST0_IC_NT;
					fdc.ST1 = 0;
					fdc.ST2 = 0;
				}
				PRINTD( FDC_LOG, "-> Found:%02X\n", *sta );
				return true;
			}
			PRINTD( FDC_LOG, "-> Not Found\n" );
		
		}
		
	}else{
		PRINTD( FDC_LOG, "-> Unformat\n" );
		DSK6::AddWait( WAIT_TRACK * 2 );
	}
	
	PRINTD( FDC_LOG, "-> false\n" );
	*sta    = BIOS_MISSING_IAM;
	fdc.ST0 = ST0_IC_AT;
	fdc.ST1 = ST1_MISSING_AM;
	fdc.ST2 = 0;
	return false;
}


////////////////////////////////////////////////////////////////
// Read Diagnostic
//  とりあえず暫定仕様
////////////////////////////////////////////////////////////////
void DSK66::ReadDiagnostic( void )
{
	PRINTD( FDC_LOG, "[DSK66][ReadDiag] Drv:%d C:%d H:%d R:%d N:%d\n", CmdIn.Data[1]&3, CmdIn.Data[2], CmdIn.Data[3], CmdIn.Data[4], CmdIn.Data[5] );
	
	WORD secsize = 0, gap3size, bufad;
	
	DSK6::ResetWait();
	
	// Command phase
	fdc.MT  = (CmdIn.Data[0]&0x80)>>7;	// Multi-track
	fdc.MF  = (CmdIn.Data[0]&0x40)>>6;	// MFM/FM Mode
	fdc.SK  = (CmdIn.Data[0]&0x20)>>5;	// Skip
	fdc.HD  = (CmdIn.Data[1]&0x04)>>2;	// Head (0-1)
	fdc.US  =  CmdIn.Data[1]&0x03;		// Unit Select (0-3)
	fdc.C   =  CmdIn.Data[2];			// Cylinder Number
	fdc.H   =  CmdIn.Data[3];			// Head Address
	fdc.R   =  CmdIn.Data[4];			// Record
	fdc.N   =  CmdIn.Data[5];			// Number
	fdc.EOT =  CmdIn.Data[6];			// End of Track
	fdc.GPL =  CmdIn.Data[7];			// Gap Length
	fdc.DTL =  CmdIn.Data[8];			// Data length
	fdc.ST0 = fdc.ST1 = fdc.ST2 = 0;
	
	// Execution phase
	if( IsMount( fdc.US ) ){
		// インデックス信号の直後のセクタから読む
		Dimg[fdc.US]->Seek( Dimg[fdc.US]->Track() );
		DSK6::AddWait( WAIT_GAP0 + WAIT_ID );
		
		secsize = Dimg[fdc.US]->GetSecSize();
		
		// gap3: 標準的な物理フォーマット
		gap3size = Gap3size[(secsize>>8)&7];
		
		bufad = 0;
		// データ読込み
		for( int i=0; i<secsize; i++ ){
			BufWrite( bufad, Dimg[fdc.US]->Get8() );
			bufad++;
		}
		
		bufad += 2;		//CRC2バイト飛ばす
		
		// gap3読込み(の振り)
		for( int i=0; i<gap3size; i++ ){
			BufWrite( bufad, 0x4e);
			bufad++;
		}
		DSK6::AddWait( WAIT_DATA((secsize>>8)&7) );
		
		// sync読込み(の振り)
		for( int i=0; i<12; i++ ){
			BufWrite( bufad, 0x00);
			bufad++;
		}
		DSK6::AddWait( WAIT_BYTE*12 );
		
		Dimg[fdc.US]->GetID( &fdc.C, &fdc.H, &fdc.R, &fdc.N );
		
	}else{
		fdc.ST0  = ST0_NOT_READY;	// ST0  bit3 : media not ready
		DSK6::AddWait( WAIT_TRACK * 2 );
	}
	
	// Result phase
	fdc.ST0 |= fdc.US;
	
	PushStatus( fdc.N );	// N
	PushStatus( fdc.R );	// R
	PushStatus( fdc.H );	// H
	PushStatus( fdc.C );	// C
	PushStatus( fdc.ST2 );	// ST2
	PushStatus( fdc.ST1 );	// ST1
	PushStatus( fdc.ST0 );	// ST0
	
	fdc.Status = FDC_FD2PC | FDC_BUSY | ( fdc.Status & 0x0f );
	fdc.Intr   = false;
	
	// ウェイト設定
	DSK6::SetWait( EID_EXWAIT );
}


////////////////////////////////////////////////////////////////
// Specify
////////////////////////////////////////////////////////////////
void DSK66::Specify( void )
{
	PRINTD( FDC_LOG, "[DSK66][Specify] SRT:%dms HUT:%dms HLT:%dms %s\n", 16-((CmdIn.Data[1]>>4)&0x0f), (CmdIn.Data[1]&0x0f)*16, (CmdIn.Data[2]>>1)*2, CmdIn.Data[2]&1 ? "NonDMA" : "DMA" );
	
	// Command phase
	fdc.SRT = 16-((CmdIn.Data[1]>>4)&0x0f);		// Step Rate Time
	fdc.HUT = (CmdIn.Data[1]&0x0f)*16;			// Head Unloaded Time
	fdc.HLT = (CmdIn.Data[2]>>1)*2;				// Head Load Time
	fdc.ND  =  CmdIn.Data[2]&1 ? true : false;	// Non DMA Mode  true:Non DMA false:DMA
	
	fdc.Status = FDC_DATA_READY | ( fdc.Status & 0x0f );
}


////////////////////////////////////////////////////////////////
// Read Data
////////////////////////////////////////////////////////////////
void DSK66::ReadData( void )
{
	PRINTD( FDC_LOG, "[DSK66][Read] Drv:%d C:%d H:%d R:%d N:%d size:%d\n", CmdIn.Data[1]&3, CmdIn.Data[2], CmdIn.Data[3], CmdIn.Data[4], CmdIn.Data[5], SendBytes );
	
	BYTE imgsta = BIOS_READY;
	
	DSK6::ResetWait();
	
	// Command phase
	fdc.MT  = (CmdIn.Data[0]&0x80)>>7;	// Multi-track
	fdc.MF  = (CmdIn.Data[0]&0x40)>>6;	// MFM/FM Mode
	fdc.SK  = (CmdIn.Data[0]&0x20)>>5;	// Skip
	fdc.HD  = (CmdIn.Data[1]&0x04)>>2;	// Head (0-1)
	fdc.US  =  CmdIn.Data[1]&0x03;		// Unit Select (0-3)
	fdc.C   =  CmdIn.Data[2];			// Cylinder Number
	fdc.H   =  CmdIn.Data[3];			// Head Address
	fdc.R   =  CmdIn.Data[4];			// Record
	fdc.N   =  CmdIn.Data[5];			// Number
	fdc.EOT =  CmdIn.Data[6];			// End of Track
	fdc.GPL =  CmdIn.Data[7];			// Gap Length
	fdc.DTL =  CmdIn.Data[8];			// Data length
	fdc.ST0 = fdc.ST1 = fdc.ST2 = 0;
	
	// Execution phase
	if( IsMount( fdc.US ) ){
		int i       = 0;				// 転送済みデータ数
		int secbyte = 0;				// セクタ内の残りデータ数
		
		
		do{
			secbyte = 0;
			
			// セクタをサーチ
			SearchSector( &imgsta );
			
			// セクタのステータスをチェック
			// 状態によりデータ読込みをスキップする
			switch( imgsta ){
			case BIOS_NO_DATA:
			case BIOS_MISSING_IAM:
			case BIOS_ID_CRC_ERROR:
			case BIOS_MISSING_DAM:
				break;
				
			case BIOS_DATA_CRC_ERROR:
			default:
				secbyte = Dimg[fdc.US]->GetSecSize();
				DSK6::AddWait( WAIT_DATA( fdc.N ) );
			}
			
			// データ読込み
			for( ; secbyte; secbyte-- )
				BufWrite( i++, Dimg[fdc.US]->Get8() );
			
			// 最終セクタ?
			if( fdc.R == fdc.EOT ) i = SendBytes;	// 読込み終了
			else				   fdc.R++;			// 次のセクタ
			
			
		}while( (SendBytes > i) && (fdc.ST0 == ST0_IC_NT) );
		
	}else{
		fdc.ST0 = ST0_NOT_READY;	// ST0  bit3 : media not ready
	}
	
	// Result phase
	fdc.ST0 |= fdc.US;
	
	PushStatus( fdc.N );	// N
	PushStatus( fdc.R );	// R
	PushStatus( fdc.H );	// H
	PushStatus( fdc.C );	// C
	PushStatus( fdc.ST2 );	// ST2
	PushStatus( fdc.ST1 );	// ST1
	PushStatus( fdc.ST0 );	// ST0
	
	fdc.Status = FDC_FD2PC | FDC_BUSY | ( fdc.Status & 0x0f );
	fdc.Intr   = false;
	
	// ウェイト設定
	DSK6::SetWait( EID_EXWAIT );
}


////////////////////////////////////////////////////////////////
// Write Data
////////////////////////////////////////////////////////////////
void DSK66::WriteData( void )
{
	PRINTD( FDC_LOG, "[DSK66][Write] Drv:%d C:%d H:%d R:%d N:%d size:%d\n", CmdIn.Data[1]&3, CmdIn.Data[2], CmdIn.Data[3], CmdIn.Data[4], CmdIn.Data[5], SendBytes );
	
	BYTE imgsta = BIOS_READY;
	
	DSK6::ResetWait();
	
	// Command phase
	fdc.MT  = (CmdIn.Data[0]&0x80)>>7;	// Multi-track
	fdc.MF  = (CmdIn.Data[0]&0x40)>>6;	// MFM/FM Mode
	fdc.SK  = (CmdIn.Data[0]&0x20)>>5;	// Skip
	fdc.HD  = (CmdIn.Data[1]&0x04)>>2;	// Head (0-1)
	fdc.US  =  CmdIn.Data[1]&0x03;		// Unit Select (0-3)
	fdc.C   =  CmdIn.Data[2];			// Cylinder Number
	fdc.H   =  CmdIn.Data[3];			// Head Address
	fdc.R   =  CmdIn.Data[4];			// Record
	fdc.N   =  CmdIn.Data[5];			// Number
	fdc.EOT =  CmdIn.Data[6];			// End of Track
	fdc.GPL =  CmdIn.Data[7];			// Gap Length
	fdc.DTL =  CmdIn.Data[8];			// Data length
	fdc.ST0 = fdc.ST1 = fdc.ST2 = 0;
	
	// Execution phase
	if( IsMount( fdc.US ) ){
		int i       = 0;				// 転送済みデータ数
		int secbyte = 0;				// セクタ内の残りデータ数
		
		do{
			secbyte = 0;
			
			// セクタをサーチ
			SearchSector( &imgsta );
			
			// セクタのステータスをチェック
			// 状態によりデータ書込みをスキップする
			switch( imgsta ){
			case BIOS_NO_DATA:
			case BIOS_MISSING_IAM:
			case BIOS_ID_CRC_ERROR:
				break;
				
			case BIOS_MISSING_DAM:
			case BIOS_DATA_CRC_ERROR:
			default:
				// 書込みサイズ 本当はNで決まるが次セクタの上書き処理とか面倒なので
				secbyte = min( Dimg[fdc.US]->GetSecSize(), SendBytes - i );
				DSK6::AddWait( WAIT_DATA( fdc.N ) );
			}
			
			// データ書込み
			for( ; secbyte; secbyte-- )
				Dimg[fdc.US]->Put8( BufRead( i++ ) );
			
			// 最終セクタ?
			if( fdc.R == fdc.EOT ) i = SendBytes;	// 読込み終了
			else				   fdc.R++;			// 次のセクタ
			
			
		}while( (SendBytes > i) && (fdc.ST0 == ST0_IC_NT) );
		
	}else{
		fdc.ST0 = ST0_NOT_READY;	// ST0  bit3 : media not ready
	}
	
	// Result phase
	fdc.ST0 |= fdc.US;
	
	PushStatus( fdc.N );	// N
	PushStatus( fdc.R );	// R
	PushStatus( fdc.H );	// H
	PushStatus( fdc.C );	// C
	PushStatus( fdc.ST2 );	// ST2
	PushStatus( fdc.ST1 );	// ST1
	PushStatus( fdc.ST0 );	// ST0
	
	fdc.Status = FDC_FD2PC | FDC_BUSY | ( fdc.Status & 0x0f );
	fdc.Intr   = false;
	
	// ウェイト設定
	DSK6::SetWait( EID_EXWAIT );
}


////////////////////////////////////////////////////////////////
// Recalibrate
////////////////////////////////////////////////////////////////
void DSK66::Recalibrate( void )
{
	PRINTD( FDC_LOG, "[DSK66][Recalibrate]\n" );
	
	CmdIn.Data[2] = 0;	// トラック0をシーク
	Seek();
}


////////////////////////////////////////////////////////////////
// Seek
////////////////////////////////////////////////////////////////
void DSK66::Seek( void )
{
	PRINTD( FDC_LOG, "[DSK66][Seek] Drv:%d C:%d\n", CmdIn.Data[1]&3, CmdIn.Data[2] );
	
	// Command phase
	fdc.US          = CmdIn.Data[1]&3;		// Unit Select (0-3)
	fdc.NCN[fdc.US] = CmdIn.Data[2];		// New Cylinder Number
	
	// Execution phase
	// 無効ドライブ or シーク不要 or ディスクがマウントされてない?
	if( ( fdc.US >= DrvNum ) || fdc.NCN[fdc.US] == fdc.PCN[fdc.US] || !IsMount( fdc.US ) ){
		fdc.SeekSta[fdc.US] = SK_END;
		fdc.Intr            = true;
	}else{										// シーク実行
		// ウェイト設定
		int eid = EID_SEEK1;
		switch( fdc.US ){
		case 0: eid = EID_SEEK1; break;
		case 1: eid = EID_SEEK2; break;
		case 2: eid = EID_SEEK3; break;
		case 3: eid = EID_SEEK4; break;
		}
		DSK6::ResetWait();
		DSK6::AddWait( (abs((int)(fdc.NCN[fdc.US]-fdc.PCN[fdc.US]))*fdc.SRT+fdc.HUT+fdc.HLT)*WAIT_SEEK );	// 適当
		DSK6::SetWait( eid );
		
		fdc.SeekSta[fdc.US] = SK_SEEK;
		fdc.Intr            = false;
		fdc.Status         &= ~(1<<fdc.US);
	}
	fdc.Status = FDC_DATA_READY | ( fdc.Status & 0x0f );
}


////////////////////////////////////////////////////////////////
// Sense Interrupt Status
////////////////////////////////////////////////////////////////
void DSK66::SenseInterruptStatus( void )
{
	PRINTD( FDC_LOG, "[DSK66][SenseInterruptStatus] " );
	
	int Drv;
	
	// Result phase
	// シーク完了ドライブを探す
	for( Drv=0; Drv<4; Drv++ )
		if( fdc.SeekSta[Drv] == SK_END ) break;
	
	if( Drv >= 4 ){	// シーク完了ドライブなし
		PRINTD( FDC_LOG, "None\n" );
		PushStatus( ST0_IC_IC );
	}else{			// シーク完了ドライブあり
		if( Drv < DrvNum && IsMount( Drv ) ){
			fdc.ST0 = ST0_IC_NT | ST0_SEEK_END | Drv;
			// トラックNoを2倍(1D->2D)
			Dimg[fdc.US]->Seek( fdc.PCN[Drv] * 2 );
			PRINTD( FDC_LOG, "Drv:%d ST0:%02X C:%d\n", Drv, fdc.ST0, fdc.PCN[Drv] );
		}else{
			PRINTD( FDC_LOG, "Drv:%d NotReady\n", Drv );
			fdc.ST0 = ST0_IC_AT | ST0_SEEK_END | ST0_NOT_READY | Drv;
		}
		fdc.SeekSta[Drv] = SK_STOP;
		fdc.Status      &= ~(1<<Drv);
		
		PushStatus( fdc.PCN[Drv] );
		PushStatus( fdc.ST0 );
	}
	
	fdc.Status = FDC_DATA_READY | FDC_FD2PC | ( fdc.Status & 0x0f );
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
void DSK66::OutB1H( int, BYTE data ){ ExtDrv = data&4 ? true : false; }	// FDCIモード設定
void DSK66::OutB3H( int, BYTE data ){ B2Dir  = data&1 ? true : false; }	// PortB2hの入出力制御


void DSK66::OutD0H( int port, BYTE data ){ if( !ExtDrv ) BufWrite( ((port&0xff)<<8)|((port>>8)&0xff), data ); }	// Buffer
void DSK66::OutD1H( int port, BYTE data ){ if( !ExtDrv ) BufWrite( ((port&0xff)<<8)|((port>>8)&0xff), data ); }	// Buffer
void DSK66::OutD2H( int port, BYTE data ){ if( !ExtDrv ) BufWrite( ((port&0xff)<<8)|((port>>8)&0xff), data ); }	// Buffer
void DSK66::OutD3H( int port, BYTE data ){ if( !ExtDrv ) BufWrite( ((port&0xff)<<8)|((port>>8)&0xff), data ); }	// Buffer

void DSK66::OutD6H( int, BYTE data ){}									// FDDモータ制御
void DSK66::OutD8H( int, BYTE data ){}									// 書き込み補償制御 ???
void DSK66::OutDAH( int, BYTE data ){ SendBytes = ~( data - 0x10 ) * 256; }	// 転送量指定
void DSK66::OutDDH( int, BYTE data ){ OutFDC( data ); }					// FDC データレジスタ
void DSK66::OutDEH( int, BYTE data ){}									// ?


BYTE DSK66::InB2H( int ){ return !B2Dir && fdc.Intr ? 0xff : 0x0e; }			// FDC INT

BYTE DSK66::InD0H( int port ){ return ExtDrv ? 0xff : BufRead( ((port&0xff)<<8)|((port>>8)&0xff) ); }			// Buffer
BYTE DSK66::InD1H( int port ){ return ExtDrv ? 0xff : BufRead( ((port&0xff)<<8)|((port>>8)&0xff) ); }			// Buffer
BYTE DSK66::InD2H( int port ){ return ExtDrv ? 0xff : BufRead( ((port&0xff)<<8)|((port>>8)&0xff) ); }			// Buffer
BYTE DSK66::InD3H( int port ){ return ExtDrv ? 0xff : BufRead( ((port&0xff)<<8)|((port>>8)&0xff) ); }			// Buffer


BYTE DSK66::InD4H( int ){ return 0; }									// FDDモータの状態
BYTE DSK66::InDCH( int ){ return fdc.Status; }							// FDC ステータスレジスタ
BYTE DSK66::InDDH( int ){ return InFDC(); }								// FDC データレジスタ


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
bool DSK6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "DISK", NULL, "DrvNum",  "%d", DrvNum );
	Ini->PutEntry( "DISK", NULL, "WaitCnt", "%d", waitcnt );
	
	// ディスクイメージオブジェクト
	for( int i=0; i<DrvNum; i++ ){
		if( Dimg[i] ){
			char stren[16], pathstr[PATH_MAX+1];
			strncpy( pathstr, Dimg[i]->GetFileName(), PATH_MAX );
			OSD_RelativePath( pathstr );
			sprintf( stren, "DISK_%d_FileName", i );
			Ini->PutEntry( "DISK", NULL, stren,	"%s", pathstr );
			sprintf( stren, "DISK_%d_trkno", i );
			Ini->PutEntry( "DISK", NULL, stren,	"%d", Dimg[i]->Track() );
			sprintf( stren, "DISK_%d_secno", i );
			Ini->PutEntry( "DISK", NULL, stren,	"%d", Dimg[i]->Sector() );
		}
	}
	
	return true;
}
bool DSK60::DokoSave( cIni *Ini )
{
	char stren[16],strva[256];
	int i,j;
	
	if( !Ini || !DSK6::DokoSave( Ini ) ) return false;
	
	// DSK60
	Ini->PutEntry( "P66DISK", NULL, "mdisk_DAC",		"%s",		mdisk.DAC ? "Yes" : "No" );
	Ini->PutEntry( "P66DISK", NULL, "mdisk_RFD",		"%s",		mdisk.RFD ? "Yes" : "No" );
	Ini->PutEntry( "P66DISK", NULL, "mdisk_DAV",		"%s",		mdisk.DAV ? "Yes" : "No" );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_command",	"%d",		mdisk.command );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_step",		"%d",		mdisk.step );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_blk",		"%d",		mdisk.blk );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_drv",		"%d",		mdisk.drv );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_trk",		"%d",		mdisk.trk );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_sct",		"%d",		mdisk.sct );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_rsize",		"%d",		mdisk.rsize );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_wsize",		"%d",		mdisk.wsize );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_ridx",		"%d",		mdisk.ridx );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_size",		"%d",		mdisk.size );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_retdat",		"0x%02X",	mdisk.retdat );
	Ini->PutEntry( "P60DISK", NULL, "mdisk_busy",		"%d",		mdisk.busy );
	Ini->PutEntry( "P66DISK", NULL, "mdisk_error",		"%s",		mdisk.error ? "Yes" : "No" );
	
	Ini->PutEntry( "P60DISK", NULL, "io_D1H",			"0x%02X",	io_D1H );
	Ini->PutEntry( "P60DISK", NULL, "io_D2H",			"0x%02X",	io_D2H );
	
	for( i=0; i<4096; i+=64 ){
		sprintf( stren, "RBuf_%04X", i );
		for( j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", RBuf[i+j] );
		Ini->PutEntry( "P60DISK", NULL, stren, "%s", strva );
	}
	for( i=0; i<4096; i+=64 ){
		sprintf( stren, "WBuf_%04X", i );
		for( j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", WBuf[i+j] );
		Ini->PutEntry( "P60DISK", NULL, stren, "%s", strva );
	}
	
	return true;
}

bool DSK66::DokoSave( cIni *Ini )
{
	char stren[16],strva[256];
	int i,j,k;
	
	if( !Ini || !DSK6::DokoSave( Ini ) ) return false;
	
	// DSK66
	for( i=0; i<10; i++ ){
		sprintf( stren, "CmdIn_Data_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "0x%02X", CmdIn.Data[i] );
		sprintf( stren, "CmdOut_Data_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "0x%02X", CmdOut.Data[i] );
	}
	Ini->PutEntry( "P66DISK", NULL, "CmdIn_Index", "%d", CmdIn.Index );
	Ini->PutEntry( "P66DISK", NULL, "CmdOut_Index", "%d", CmdOut.Index );
	
	// FDC
	for( i=0; i<4; i++ ){
		sprintf( stren, "fdc_SeekSta_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "%d",		(int)fdc.SeekSta[i] );
		sprintf( stren, "fdc_NCN_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "0x%02X",	fdc.NCN[i] );
		sprintf( stren, "fdc_PCN_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "0x%02X",	fdc.PCN[i] );
	}
	Ini->PutEntry( "P66DISK", NULL, "fdc_SRT",		"0x%02X",	fdc.SRT );
	Ini->PutEntry( "P66DISK", NULL, "fdc_HUT",		"0x%02X",	fdc.HUT );
	Ini->PutEntry( "P66DISK", NULL, "fdc_HLT",		"0x%02X",	fdc.HLT );
	Ini->PutEntry( "P66DISK", NULL, "fdc_ND",		"%s",		fdc.ND ? "Yes" : "No" );
	Ini->PutEntry( "P66DISK", NULL, "fdc_MT",		"0x%02X",	fdc.MT );
	Ini->PutEntry( "P66DISK", NULL, "fdc_MF",		"0x%02X",	fdc.MF );
	Ini->PutEntry( "P66DISK", NULL, "fdc_SK",		"0x%02X",	fdc.SK );
	Ini->PutEntry( "P66DISK", NULL, "fdc_HD",		"0x%02X",	fdc.HD );
	Ini->PutEntry( "P66DISK", NULL, "fdc_US",		"0x%02X",	fdc.US );
	Ini->PutEntry( "P66DISK", NULL, "fdc_C",		"0x%02X",	fdc.C );
	Ini->PutEntry( "P66DISK", NULL, "fdc_H",		"0x%02X",	fdc.H );
	Ini->PutEntry( "P66DISK", NULL, "fdc_R",		"0x%02X",	fdc.R );
	Ini->PutEntry( "P66DISK", NULL, "fdc_N",		"0x%02X",	fdc.N );
	Ini->PutEntry( "P66DISK", NULL, "fdc_EOT",		"0x%02X",	fdc.EOT );
	Ini->PutEntry( "P66DISK", NULL, "fdc_GPL",		"0x%02X",	fdc.GPL );
	Ini->PutEntry( "P66DISK", NULL, "fdc_DTL",		"0x%02X",	fdc.DTL );
	Ini->PutEntry( "P66DISK", NULL, "fdc_D",		"0x%02X",	fdc.D );
	Ini->PutEntry( "P66DISK", NULL, "fdc_SC",		"0x%02X",	fdc.SC );
	Ini->PutEntry( "P66DISK", NULL, "fdc_ST0",		"0x%02X",	fdc.ST0 );
	Ini->PutEntry( "P66DISK", NULL, "fdc_ST1",		"0x%02X",	fdc.ST1 );
	Ini->PutEntry( "P66DISK", NULL, "fdc_ST2",		"0x%02X",	fdc.ST2 );
	Ini->PutEntry( "P66DISK", NULL, "fdc_ST3",		"0x%02X",	fdc.ST3 );
	Ini->PutEntry( "P66DISK", NULL, "fdc_Status",	"0x%02X",	fdc.Status );
	Ini->PutEntry( "P66DISK", NULL, "fdc_Intr",		"%s",		fdc.Intr ? "Yes" : "No" );
	
	// FDCI
	Ini->PutEntry( "P66DISK", NULL, "SendBytes",	"%d",		SendBytes );
	Ini->PutEntry( "P66DISK", NULL, "ExtDrv",		"%s", 		ExtDrv ? "Yes" : "No" );
	Ini->PutEntry( "P66DISK", NULL, "B2Dir",		"%s",		B2Dir  ? "Yes" : "No" );
	
	for( i=0; i<4; i++ ){
		for( j=0; j<256; j+=64 ){
			sprintf( stren, "FDDBuf_%d_%02X", i, j );
			for( k=0; k<64; k++ ) sprintf( &strva[k*2], "%02X", FDDBuf[i*256+j+k] );
			Ini->PutEntry( "P66DISK", NULL, stren, "%s", strva );
		}
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
bool DSK6::DokoLoad( cIni *Ini )
{
	if( !Ini ) return false;
	
	// とりあえず全部アンマウント
	for( int i=0; i<DrvNum; i++ ) if( Dimg[i] ) Unmount( i) ;
	
	Ini->GetInt("DISK", "DrvNum",	&DrvNum,	DrvNum );
	Ini->GetInt("DISK", "WaitCnt",	&waitcnt,	waitcnt );
	
	// ディスクイメージオブジェクト
	for( int i=0; i<DrvNum; i++ ){
		char stren[16], strva[256];
		sprintf( stren, "DISK_%d_FileName", i );
		if( Ini->GetString( "DISK", stren, strva, "" ) && Mount( i, strva ) ){
			int tr,sc;
			sprintf( stren, "DISK_%d_trkno", i );
			Ini->GetInt( "DISK", stren,	&tr,	0 );
			sprintf( stren, "DISK_%d_secno", i );
			Ini->GetInt( "DISK", stren,	&sc,	0 );
			Dimg[i]->Seek( tr, sc );
		}
	}
	
	return true;
}


bool DSK60::DokoLoad( cIni *Ini )
{
	int st,i,j;
	char stren[16], strva[256];
	
	if( !Ini || !DSK6::DokoLoad( Ini ) ) return false;
	
	// DSK60
	Ini->GetTruth( "P66DISK", "mdisk_DAC",		&mdisk.DAC,		mdisk.DAC );
	Ini->GetTruth( "P66DISK", "mdisk_RFD",		&mdisk.RFD,		mdisk.RFD );
	Ini->GetTruth( "P66DISK", "mdisk_DAV",		&mdisk.DAV,		mdisk.DAV );
	Ini->GetInt(   "P60DISK", "mdisk_command",	&mdisk.command,	mdisk.command );
	Ini->GetInt(   "P60DISK", "mdisk_step",		&mdisk.step,	mdisk.step );
	Ini->GetInt(   "P60DISK", "mdisk_blk",		&mdisk.blk,		mdisk.blk );
	Ini->GetInt(   "P60DISK", "mdisk_drv",		&mdisk.drv,		mdisk.drv );
	Ini->GetInt(   "P60DISK", "mdisk_trk",		&mdisk.trk,		mdisk.trk );
	Ini->GetInt(   "P60DISK", "mdisk_sct",		&mdisk.sct,		mdisk.sct );
	Ini->GetInt(   "P60DISK", "mdisk_rsize",	&mdisk.rsize,	mdisk.rsize );
	Ini->GetInt(   "P60DISK", "mdisk_wsize",	&mdisk.wsize,	mdisk.wsize );
	Ini->GetInt(   "P60DISK", "mdisk_ridx",		&mdisk.ridx,	mdisk.ridx );
	Ini->GetInt(   "P60DISK", "mdisk_size",		&mdisk.size,	mdisk.size );
	Ini->GetInt(   "P60DISK", "mdisk_retdat",	&st,			mdisk.retdat );	mdisk.retdat = st;
	Ini->GetInt(   "P60DISK", "mdisk_busy",		&st,			mdisk.busy );	mdisk.busy = st;
	Ini->GetTruth( "P66DISK", "fdc_Intr",		&mdisk.error,	mdisk.error );
	Ini->GetInt(   "P60DISK", "io_D1H",			&st,			io_D1H );		io_D1H = st;
	Ini->GetInt(   "P60DISK", "io_D2H",			&st,			io_D2H );		io_D2H = st;
	
	for( i=0; i<4096; i+=64 ){
		sprintf( stren, "RBuf_%04X", i );
		memset( strva, '0', 64*2 );
		if( Ini->GetString( "P60DISK", stren, strva, strva ) ){
			for( j=0; j<64; j++ ){
				char dt[5] = "0x";
				strncpy( &dt[2], &strva[j*2], 2 );
				RBuf[i+j] = strtol( dt, NULL, 16 );
			}
		}
	}
	for( i=0; i<4096; i+=64 ){
		sprintf( stren, "WBuf_%04X", i );
		memset( strva, '0', 64*2 );
		if( Ini->GetString( "P60DISK", stren, strva, strva ) ){
			for( j=0; j<64; j++ ){
				char dt[5] = "0x";
				strncpy( &dt[2], &strva[j*2], 2 );
				WBuf[i+j] = strtol( dt, NULL, 16 );
			}
		}
	}
	
	return true;
}

bool DSK66::DokoLoad( cIni *Ini )
{
	int st,i,j,k;
	char stren[16], strva[256];
	
	if( !Ini || !DSK6::DokoLoad( Ini ) ) return false;
	
	// DSK66
	for( i=0; i<10; i++ ){
		sprintf( stren, "CmdIn_Data_%d", i );
		Ini->GetInt( "P66DISK", stren,	&st,	CmdIn.Data[i] );	CmdIn.Data[i] = st;
		sprintf( stren, "CmdOut_Data_%d", i );
		Ini->GetInt( "P66DISK", stren,	&st,	CmdOut.Data[i] );	CmdOut.Data[i] = st;
	}
	Ini->GetInt( "P66DISK", "CmdIn_Index",	&CmdIn.Index,	CmdIn.Index );
	Ini->GetInt( "P66DISK", "CmdOut_Index",	&CmdOut.Index,	CmdOut.Index );
	
	// FDC
	for( i=0; i<4; i++ ){
		sprintf( stren, "fdc_NCN_%d", i );
		Ini->GetInt( "P66DISK", stren,	&st,	fdc.NCN[i] );		fdc.NCN[i] = st;
		sprintf( stren, "fdc_PCN_%d", i );
		Ini->GetInt( "P66DISK", stren,	&st,	fdc.PCN[i] );		fdc.PCN[i] = st;
		sprintf( stren, "fdc_SeekSta%d", i );
		Ini->GetInt( "P66DISK", stren,	&st,	fdc.SeekSta[i] );	fdc.SeekSta[i] = (FdcSeek)st;
	}
	Ini->GetInt(   "P66DISK", "fdc_SRT",	&st,		fdc.SRT );		fdc.SRT = st;
	Ini->GetInt(   "P66DISK", "fdc_HUT",	&st,		fdc.HUT );		fdc.HUT = st;
	Ini->GetInt(   "P66DISK", "fdc_HLT",	&st,		fdc.HLT );		fdc.HLT = st;
	Ini->GetTruth( "P66DISK", "fdc_ND",		&fdc.ND,	fdc.ND );
	Ini->GetInt(   "P66DISK", "fdc_MT",		&st,		fdc.MT );		fdc.MT = st;
	Ini->GetInt(   "P66DISK", "fdc_MF",		&st,		fdc.MF );		fdc.MF = st;
	Ini->GetInt(   "P66DISK", "fdc_SK",		&st,		fdc.SK );		fdc.SK = st;
	Ini->GetInt(   "P66DISK", "fdc_HD",		&st,		fdc.HD );		fdc.HD = st;
	Ini->GetInt(   "P66DISK", "fdc_US",		&st,		fdc.US );		fdc.US = st;
	Ini->GetInt(   "P66DISK", "fdc_C",		&st,		fdc.C );		fdc.C = st;
	Ini->GetInt(   "P66DISK", "fdc_H",		&st,		fdc.H );		fdc.H = st;
	Ini->GetInt(   "P66DISK", "fdc_R",		&st,		fdc.R );		fdc.R = st;
	Ini->GetInt(   "P66DISK", "fdc_N",		&st,		fdc.N );		fdc.N = st;
	Ini->GetInt(   "P66DISK", "fdc_EOT",	&st,		fdc.EOT );		fdc.EOT = st;
	Ini->GetInt(   "P66DISK", "fdc_GPL",	&st,		fdc.GPL );		fdc.GPL = st;
	Ini->GetInt(   "P66DISK", "fdc_DTL",	&st,		fdc.DTL );		fdc.DTL = st;
	Ini->GetInt(   "P66DISK", "fdc_D",		&st,		fdc.D );		fdc.D = st;
	Ini->GetInt(   "P66DISK", "fdc_SC",		&st,		fdc.SC );		fdc.SC = st;
	Ini->GetInt(   "P66DISK", "fdc_ST0",	&st,		fdc.ST0 );		fdc.ST0 = st;
	Ini->GetInt(   "P66DISK", "fdc_ST1",	&st,		fdc.ST1 );		fdc.ST1 = st;
	Ini->GetInt(   "P66DISK", "fdc_ST2",	&st,		fdc.ST2 );		fdc.ST2 = st;
	Ini->GetInt(   "P66DISK", "fdc_ST3",	&st,		fdc.ST3 );		fdc.ST3 = st;
	Ini->GetInt(   "P66DISK", "fdc_Status",	&st,		fdc.Status );	fdc.Status = st;
	Ini->GetTruth( "P66DISK", "fdc_Intr",	&fdc.Intr,	fdc.Intr );
	
	// FDCI
	Ini->GetInt(   "P66DISK", "SendBytes",	&SendBytes,	SendBytes );
	Ini->GetTruth( "P66DISK", "ExtDrv",		&ExtDrv,	ExtDrv );
	Ini->GetTruth( "P66DISK", "B2Dir",		&B2Dir,		B2Dir );
	
	for( i=0; i<4; i++ ){
		for( j=0; j<256; j+=64 ){
			sprintf( stren, "FDDBuf_%d_%02X", i, j );
			memset( strva, '0', 64*2 );
			Ini->GetString( "P66DISK", stren, strva, strva );
			for( k=0; k<64; k++ ){
				char dt[5] = "0x";
				strncpy( dt, &strva[k*2], 2 );
				FDDBuf[i*256+j+k] = strtol( dt, NULL, 16 );
			}
		}
	}
	
	return true;
}





////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor DSK60::descriptor = {
	DSK60::indef, DSK60::outdef
};

const Device::OutFuncPtr DSK60::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &DSK60::OutD1H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK60::OutD2H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK60::OutD3H )
};

const Device::InFuncPtr DSK60::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &DSK60::InD0H ),
	STATIC_CAST( Device::InFuncPtr, &DSK60::InD1H ),
	STATIC_CAST( Device::InFuncPtr, &DSK60::InD2H )
};


const Device::Descriptor DSK66::descriptor = {
	DSK66::indef, DSK66::outdef
};

const Device::OutFuncPtr DSK66::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutB1H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutB3H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutD0H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutD1H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutD2H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutD3H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutD6H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutD8H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutDAH ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutDDH ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutDEH )
};

const Device::InFuncPtr DSK66::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &DSK66::InB2H ),
	STATIC_CAST( Device::InFuncPtr, &DSK66::InD0H ),
	STATIC_CAST( Device::InFuncPtr, &DSK66::InD1H ),
	STATIC_CAST( Device::InFuncPtr, &DSK66::InD2H ),
	STATIC_CAST( Device::InFuncPtr, &DSK66::InD3H ),
	STATIC_CAST( Device::InFuncPtr, &DSK66::InD4H ),
	STATIC_CAST( Device::InFuncPtr, &DSK66::InDCH ),
	STATIC_CAST( Device::InFuncPtr, &DSK66::InDDH )
};
