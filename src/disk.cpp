#include <stdlib.h>
#include <string.h>
#include <new>

#include "log.h"
#include "disk.h"
#include "error.h"
#include "schedule.h"

// イベントID
#define	EID_SEEK1	(1)
#define	EID_SEEK2	(2)
#define	EID_SEEK3	(3)
#define	EID_SEEK4	(4)

#define	EID_OUTFDC	(5)
#define	EID_EXECFDC	(6)


////////////////////////////////////////////////////////////////
// ディスク 基底クラス
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
DSK6::DSK6( VM6 *vm, const P6ID& id, int num ) : P6DEVICE(vm,id)
{
	DrvNum = max( min( num, MAXDRV ) , 0 );
	
	for( int i=0; i<MAXDRV; i++ ){
		*FilePath[i] = '\0';
		d88[i]       = NULL;
		Sys[i]       = FALSE;
	}
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSK6::~DSK6( void )
{
	for( int i=0; i<DrvNum; i++ )
		if( d88[i] ) Unmount( i );
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
// ウェイト設定
////////////////////////////////////////////////////////////////
BOOL DSK6::SetWait( int eid, int wait )
{
	PRINTD1( DISK_LOG, "[DISK][SetWait] %dus\n", wait );
	
	// 既に設定されていたら問答無用でキャンセル
	vm->sche->Del( this, eid );
	if( !vm->sche->Add( this, eid, wait, EV_US ) ) return TRUE;
	else                                           return FALSE;
}


////////////////////////////////////////////////////////////////
// DISK マウント
////////////////////////////////////////////////////////////////
BOOL DSK6::Mount( int drvno, char *filename )
{
	PRINTD1( DISK_LOG, "[DISK][Mount] Drive : %d\n", drvno );
	
	if( drvno >= DrvNum ) return FALSE;
	
	// もしマウント済みであればアンマウントする
	if( d88[drvno] ) Unmount( drvno );
	
	// D88オブジェクトを確保
	try{
		d88[drvno] = new cD88;
		if( !d88[drvno]->Init( filename ) ) throw Error::DiskMountFailed;
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return FALSE;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		
		Unmount( drvno );
		return FALSE;
	}
	
	// ファイルパス保存
	strncpy( FilePath[drvno], filename, PATH_MAX );
	
	// システムディスクチェック
	d88[drvno]->Seek88( 0, 1 );
	if( d88[drvno]->Getc88() == 'S' &&
		d88[drvno]->Getc88() == 'Y' &&
		d88[drvno]->Getc88() == 'S' )
			Sys[drvno] = TRUE;
	else
			Sys[drvno] = FALSE;
	d88[drvno]->Seek88( 0, 1 );	// 念のため
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// DISK アンマウント
////////////////////////////////////////////////////////////////
void DSK6::Unmount( int drvno )
{
	PRINTD1( DISK_LOG, "[DISK][Unmount] Drive : %d\n", drvno );
	
	if( drvno >= DrvNum ) return;
	
	if( d88[drvno] ){
		// D88オブジェクトを開放
		delete d88[drvno];
		d88[drvno] = NULL;
		*FilePath[drvno] = '\0';
		Sys[drvno] = FALSE;
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
BOOL DSK6::IsMount( int drvno )
{
	if( drvno < DrvNum ) return d88[drvno] ? TRUE : FALSE;
	else                 return FALSE;
}


////////////////////////////////////////////////////////////////
// システムディスク?
////////////////////////////////////////////////////////////////
BOOL DSK6::IsSystem( int drvno )
{
	return Sys[drvno];
}


////////////////////////////////////////////////////////////////
// プロテクト?
////////////////////////////////////////////////////////////////
BOOL DSK6::IsProtect( int drvno )
{
	if( !IsMount( drvno ) ) return FALSE;
	
	return d88[drvno]->IsProtect();
}


////////////////////////////////////////////////////////////////
// ファイルパス取得
////////////////////////////////////////////////////////////////
const char *DSK6::GetFile( int drvno )
{
	return FilePath[drvno];
}


////////////////////////////////////////////////////////////////
// DISK名取得
////////////////////////////////////////////////////////////////
const char *DSK6::GetName( int drvno )
{
	if( !IsMount( drvno ) ) return "";
	
	return d88[drvno]->GetDiskImgName();
}




////////////////////////////////////////////////////////////////
// ミニフロッピーディスククラス
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
DSK60::DSK60( VM6 *vm, const ID& id, int num ) : DSK6(vm,id,num), Device(id),
    io_D1H(0), io_D2H(0), io_D3H(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSK60::~DSK60( void ){}


////////////////////////////////////////////////////////////////
// DISK処理 初期化
////////////////////////////////////////////////////////////////
BOOL DSK60::Init( void )
{
	PRINTD( DISK_LOG, "[DISK][Init]\n" );
	
	ZeroMemory( &mdisk, sizeof( DISK60 ) );
	
	mdisk.command = WAIT;		// 受け取ったコマンド
	mdisk.retdat  = 0xff;		// port D0H から返す値
	
	io_D1H = 0;
	io_D2H = 0xf0 | 0x08 | (mdisk.DAC<<2) | (mdisk.RFD<<1) | mdisk.DAV;
	io_D3H = 0;
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// DISKユニットからのデータ入力 (port D0H)
////////////////////////////////////////////////////////////////
BYTE DSK60::FddIn( void )
{
	PRINTD( DISK_LOG, "[DISK][FddIn]  <- " );
	
	if( mdisk.DAV ){		// データが有効な場合
		if( mdisk.step == 6 ){
		if( d88[mdisk.drv] ) 
			mdisk.retdat = d88[mdisk.drv]->Getc88();
		else
			mdisk.retdat = 0xff;
		if( --mdisk.size == 0 ) mdisk.step = 0;
		}
		
		mdisk.DAC = 1;
		
		PRINTD1( DISK_LOG, "%02X\n", mdisk.retdat );
		
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
	PRINTD1( DISK_LOG, "[DISK][FddOut]    -> %02X", dat );
	
	if( mdisk.command == WAIT ){	// コマンドの場合
		mdisk.command = dat;
		switch( mdisk.command ){
		case INIT:					// 00h イニシャライズ
			PRINTD( DISK_LOG, " INIT" );
			break;
		case WRITE_DATA:			// 01h ライト データ
			PRINTD( DISK_LOG, " WRITE_DATA" );
			mdisk.step = 1;
			break;
		case READ_DATA:				// 02h リード データ
			PRINTD( DISK_LOG, " READ_DATA" );
			mdisk.step = 1;
			break;
		case SEND_DATA:				// 03h センド データ
			PRINTD( DISK_LOG, " SEND_DATA" );
			mdisk.step = 6;
			break;
		case COPY:					// 04h コピー
			PRINTD( DISK_LOG, " COPY" );
			break;
		case FORMAT:				// 05h フォーマット
			PRINTD( DISK_LOG, " FORMAT" );
			break;
		case SEND_RESULT_STATUS:	// 06h センド リザルト ステータス
			PRINTD( DISK_LOG, " SEND_RESULT_STATUS" );
			mdisk.retdat = 0x40;
			break;
		case SEND_DRIVE_STATUS:		// 07h センド ドライブ ステータス
			PRINTD( DISK_LOG, " SEND_DRIVE_STATUS" );
			mdisk.retdat = 0;
			for( int i=DrvNum; i>0; i-- )
				mdisk.retdat |= 1<<(4+i);
			break;
		case TRANSMIT:				// 11h トランスミット
			PRINTD( DISK_LOG, " TRANSMIT" );
			break;
		case RECEIVE:				// 12h レシーブ
			PRINTD( DISK_LOG, " RECEIVE" );
			break;
		case LOAD:					// 14h ロード
			PRINTD( DISK_LOG, " LOAD" );
			break;
		case SAVE:					// 15h セーブ
			PRINTD( DISK_LOG, " SAVE" );
			break;
		}
	}else{					// データの場合
		switch( mdisk.command ){
		case WRITE_DATA:			// 01h ライト データ
			switch( mdisk.step ){
			case 1:	// 01h:転送ブロック数
				mdisk.blk = dat;
				mdisk.size = mdisk.blk*256;
				mdisk.step++;
				break;
			case 2:	// 02h:ドライブ番号-1
				mdisk.drv = dat;
				mdisk.step++;
				break;
			case 3:	// 03h:トラック番号
				mdisk.trk = dat;
				mdisk.step++;
				break;
			case 4:	// 04h:セクタ番号
				mdisk.sct = dat;
				// トラックNoを2倍(1D->2D)
				if( d88[mdisk.drv] ) d88[mdisk.drv]->Seek88( mdisk.trk*2, mdisk.sct );
				mdisk.step++;
				break;
			case 5:	// 05h:データ書き込み
				if( d88[mdisk.drv] ) d88[mdisk.drv]->Putc88( dat );
				if( --mdisk.size == 0 ){
					mdisk.step = 0;
				}
				break;
			}
			break;
		case READ_DATA:				// 02h リード データ
			switch( mdisk.step ){
			case 1:	// 01h:転送ブロック数
				mdisk.blk = dat;
				mdisk.size = mdisk.blk*256;
				mdisk.step++;
				break;
			case 2:	// 02h:ドライブ番号-1
				mdisk.drv = dat;
				mdisk.step++;
				break;
			case 3:	// 03h:トラック番号
				mdisk.trk = dat;
				mdisk.step++;
				break;
			case 4:	// 04h:セクタ番号
				mdisk.sct = dat;
				// トラックNoを2倍(1D->2D)
				if( d88[mdisk.drv] ) d88[mdisk.drv]->Seek88( mdisk.trk*2, mdisk.sct );
				mdisk.step = 0;
				break;
			}
		}
	}
	PRINTD( DISK_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// DISKユニットからの制御信号入力 (port D2H)
////////////////////////////////////////////////////////////////
BYTE DSK60::FddCntIn( void )
{
	PRINTD1( DISK_LOG, "[DISK][FddCntIn]  <- %02X\n", (io_D2H&0xf0) | 0x08 | (mdisk.DAC<<2) | (mdisk.RFD<<1) | mdisk.DAV );
	
	return( (io_D2H&0xf0) | 0x08 | (mdisk.DAC<<2) | (mdisk.RFD<<1) | mdisk.DAV );
}


////////////////////////////////////////////////////////////////
// DISKユニットへの制御信号出力 (port D3H)
////////////////////////////////////////////////////////////////
void DSK60::FddCntOut( BYTE dat )
{
	PRINTD1( DISK_LOG, "[DISK][FddCntOut] -> %02X", dat );
	
	if( !(dat&0x80) ){		// 最上位bitチェック
							// 1の場合は8255のモード設定なので無視(必ずモード0と仮定する)
		switch( (dat>>1)&0x07 ){
		case 7:	// bit7 ATN
			PRINTD1( DISK_LOG, " ATN:%d", dat&1 );
			mdisk.ATN = dat&1;
			if( mdisk.ATN ){
				mdisk.RFD = 1;
				mdisk.command = WAIT;
			}
			break;
		case 6:	// bit6 DAC
			PRINTD1( DISK_LOG, " DAC:%d", dat&1 );
			mdisk.DAC = dat&1;
			if( mdisk.DAC ) mdisk.DAV = 0;
			break;
		case 5:	// bit5 RFD
			PRINTD1( DISK_LOG, " RFD:%d", dat&1 );
			mdisk.RFD = dat&1;
			if( mdisk.RFD ) mdisk.DAV = 1;
			break;
		case 4:	// bit4 DAV
			PRINTD1( DISK_LOG, " DAV:%d", dat&1 );
			mdisk.DAV = dat&1;
			mdisk.DAC = mdisk.DAV;
			break;
		}
		io_D2H = (io_D2H&0xf0) | 0x08 | (mdisk.DAC<<2) | (mdisk.RFD<<1) | mdisk.DAV;
	}
	PRINTD( DISK_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
void DSK60::OutD1H( int, BYTE data )
{
	io_D1H = data; FddOut( io_D1H );
}

void DSK60::OutD2H( int, BYTE data )
{
	io_D2H = (io_D2H&0x0f) | (data&0xf0);
}

void DSK60::OutD3H( int, BYTE data )
{
	io_D3H = data; FddCntOut( io_D3H );
}

BYTE DSK60::InD0H( int )
{
	return FddIn();
}

BYTE DSK60::InD1H( int )
{
	return io_D1H;
}

BYTE DSK60::InD2H( int )
{
	io_D2H = FddCntIn();
	return io_D2H;
}

BYTE DSK60::InD3H( int )
{
	return io_D3H;
}




////////////////////////////////////////////////////////////////
// データバッファクラス
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cFDCbuf::cFDCbuf( void )
{
	for( int i=0; i<4; i++ ) Index[i] = 0;
        for( int i=0; i<4; i++ ) *Data[i] = 0;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cFDCbuf::~cFDCbuf( void ){}


////////////////////////////////////////////////////////////////
// データバッファにデータを入れる
////////////////////////////////////////////////////////////////
void cFDCbuf::Push( int part, BYTE data )
{
	if( part > 3 ) return;
	
	if( Index[part] < 256 ) Data[part][Index[part]++] = data;
}


////////////////////////////////////////////////////////////////
// データバッファからデータを取り出す
////////////////////////////////////////////////////////////////
BYTE cFDCbuf::Pop( int part )
{
	if( part > 3 ) return 0xff;
	
	if( Index[part] > 0 ) return Data[part][--Index[part]];
	else                  return 0xff;
}


////////////////////////////////////////////////////////////////
// バッファをクリアする
////////////////////////////////////////////////////////////////
void cFDCbuf::Clear( int i )
{
	Index[i] = 0;
}




////////////////////////////////////////////////////////////////
// 66ディスククラス
////////////////////////////////////////////////////////////////

//************ FDC Status *******************
#define FDC_BUSY_D0			(0x01)
#define FDC_BUSY_D1			(0x02)
#define FDC_BUSY_D2			(0x04)
#define FDC_BUSY_D3			(0x08)
#define FDC_BUSY			(0x10)
#define FDC_READY			(0x00)
#define FDC_NON_DMA			(0x20)
#define FDC_FD2PC			(0x40)
#define FDC_PC2FD			(0x00)
#define FDC_DATA_READY		(0x80)

//************* Result Status 0 *************
#define ST0_NOT_READY		(0x08)
#define ST0_EQUIP_CHK		(0x10)
#define ST0_SEEK_END		(0x20)
#define ST0_IC_NT			(0x00)
#define ST0_IC_AT			(0x40)
#define ST0_IC_IC			(0x80)
#define ST0_IC_AI			(0xc0)

//************* Result Status 1 *************
#define ST1_NOT_WRITABLE	(0x02)

//************* Result Status 2 *************

//************* Result Status 3 *************
#define ST3_TRACK0			(0x10)
#define ST3_READY			(0x20)
#define ST3_WRITE_PROTECT	(0x40)
#define ST3_FAULT			(0x80)


//************* Wait (us) *************
#define	WAIT_RD				(1)
#define	WAIT_WR				(1)
#define WAIT_SEEK			(1000)


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
DSK66::DSK66( VM6 *vm, const ID& id, int num ) : DSK6(vm,id,num), Device(id),
    SRT(0), HUT(0), HLT(0), NonDMA(FALSE), SendSectors(0), DIO(0),
    FDCStatus(0)
{
    INITARRAY(SeekST0, 0);
    INITARRAY(LastCylinder, 0);
    INITARRAY(SeekEnd, 0);
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
DSK66::~DSK66( void ){}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
void DSK66::EventCallback( int id, int clock )
{
	int Drv = 0;
	
	switch( id ){
	case EID_SEEK1: Drv = 0; break;
	case EID_SEEK2: Drv = 1; break;
	case EID_SEEK3: Drv = 2; break;
	case EID_SEEK4: Drv = 3; break;
	}
        FDCStatus &= ~(1<<Drv);
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
BOOL DSK66::Init( void )
{
	ZeroMemory( &CmdIn,  sizeof( CmdBuffer ) );
	ZeroMemory( &CmdOut, sizeof( CmdBuffer ) );
	for( int i=0; i < 4; i++ ){
		SeekST0[i] = 0;
		LastCylinder[i] = 0;
		SeekEnd[i] = FALSE;
	}
	SRT    = 32;	// Step Rate Time
	HUT    = 1;		// Head Unloaded Time
	HLT    = 1;		// Head Load Time
	NonDMA = FALSE;	// TRUE:Non DMAモード FALSE:DMAモード
	
	SendSectors  = 0;
        FDCStatus = FDC_DATA_READY | FDC_READY | FDC_PC2FD;
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// ステータスバッファにデータを入れる
////////////////////////////////////////////////////////////////
void DSK66::PushStatus( int data )
{
	PRINTD2( DISK_LOG, "[DSK66][PushStatus] Index:%d ->%02X\n", CmdOut.Index, data );
	
	CmdOut.Data[CmdOut.Index++] = data;
}

////////////////////////////////////////////////////////////////
// ステータスバッファからデータを取り出す
////////////////////////////////////////////////////////////////
BYTE DSK66::PopStatus( void )
{
	PRINTD1( DISK_LOG, "%02X\n", (BYTE)CmdOut.Data[CmdOut.Index-1] );
	
	return CmdOut.Data[--CmdOut.Index];
}


////////////////////////////////////////////////////////////////
// FDC に書込む
////////////////////////////////////////////////////////////////
void DSK66::OutFDC( BYTE data )
{
	PRINTD2( DISK_LOG, "[DSK66][OutFDC] Index:%d ->%02X\n", CmdIn.Index, data );
	
	const int CmdLength[] = { 0,0,0,3,2,9,9,2,1,0,0,0,0,6,0,3 };
	
	CmdIn.Data[CmdIn.Index++] = data;
	
	// コマンド長のコマンドが来たら コマンド実行
	if( CmdLength[CmdIn.Data[0]&0xf] == CmdIn.Index ){
                FDCStatus = FDC_BUSY | FDC_FD2PC | ( FDCStatus & 0x0f );
		Exec();
	}else{
                FDCStatus = FDC_BUSY | FDC_DATA_READY | ( FDCStatus & 0x0f );
		DSK6::SetWait( EID_OUTFDC, WAIT_RD );
	}
}

////////////////////////////////////////////////////////////////
// FDC から読込む
////////////////////////////////////////////////////////////////
BYTE DSK66::InFDC( void )
{
	PRINTD1( DISK_LOG, "[DSK66][InFDC] Index:%d ->", CmdOut.Index-1 );
	
	// PC側が、全て受信したら、PC2FD にする。
        if( CmdOut.Index == 1 ) FDCStatus = FDC_DATA_READY | FDC_PC2FD | ( FDCStatus & 0x0f );
	return PopStatus();
}

////////////////////////////////////////////////////////////////
// FDC コマンド実行
////////////////////////////////////////////////////////////////
void DSK66::Exec( void )
{
	PRINTD1( DISK_LOG, "[DSK66][Exec] Command:%02X", CmdIn.Data[0]&0xf );
	
	CmdOut.Index = 0;
	
	switch( CmdIn.Data[0] & 0xf ){
	case 0x03:	// Specify
		PRINTD( DISK_LOG, " Specify\n" );
		Specify();
		break;
		
	case 0x05:	// Write Data
		PRINTD( DISK_LOG, " Write Data\n" );
		Write();
		break;
		
	case 0x06:	// Read Data
		PRINTD( DISK_LOG, " Read Data\n" );
		Read();
		break;
		
	case 0x08:	// Sense Interrupt Status
		PRINTD( DISK_LOG, " Sense Interrupt Status\n" );
		SenseInterruptStatus();
		break;
		
	case 0x0d:	// Write ID
		PRINTD( DISK_LOG, " Write ID\n" );
		// Format is Not Implimented
		break;
		
	case 0x07:	// Recalibrate
		PRINTD( DISK_LOG, " Recalibrate\n" );
		Recalibrate();
		break;
		
	case 0x0f:	// Seek
		PRINTD( DISK_LOG, " Seek\n" );
		Seek();
		break;
		
	default:	// Invalid
		PRINTD( DISK_LOG, " Invalid!!\n" );
	}
	CmdIn.Index = 0;
}


////////////////////////////////////////////////////////////////
// Specify
////////////////////////////////////////////////////////////////
void DSK66::Specify( void )
{
	SRT    = 32 - ((CmdIn.Data[1]>>3)&0x1e);	// Step Rate Time
	HUT    = CmdIn.Data[1]&0x0f;				// Head Unloaded Time
	HLT    = CmdIn.Data[2]>>1;					// Head Load Time
	NonDMA = CmdIn.Data[2]&1 ? TRUE : FALSE;	// TRUE:Non DMAモード FALSE:DMAモード
	
        FDCStatus = FDC_DATA_READY | ( FDCStatus & 0x0f );
}


////////////////////////////////////////////////////////////////
// Read
////////////////////////////////////////////////////////////////
void DSK66::Read( void )
{
	PRINTD5( DISK_LOG, "[DSK66][Read] Drv:%d C:%d H:%d R:%d N:%d\n", CmdIn.Data[1]&3, CmdIn.Data[2], CmdIn.Data[3], CmdIn.Data[4], CmdIn.Data[5] );
	
	int Drv,C,H,R,N;
	int i,j;
	
	Drv = CmdIn.Data[1]&3;		// ドライブNo.(0-3) 0-1に制限しておいた方がいい?
	C   = CmdIn.Data[2];		// シリンダ
//	C   = LastCylinder[Drv];	// シリンダ
	H   = CmdIn.Data[3];		// ヘッドアドレス
	R   = CmdIn.Data[4];		// セクタNo.
	N   = CmdIn.Data[5] ? CmdIn.Data[5]*256 : 256;	// セクタサイズ
	
//	C = ( !is1DD() && isSYS() && P6Version==4 ) ? c / 2: c;   // if 1D and sys-disk and PC-6601SR then c= c/2
	
	if( IsMount( Drv ) ){
		// シーク
		// トラックNoを2倍(1D->2D)
		d88[Drv]->Seek88( C*2, R );
		for( i=0; i<SendSectors; i++ ){
			cFDCbuf::Clear( i );
			for( j=0; j<N; j++ )
//			for( j=0; j<0x100; j++ )
				cFDCbuf::Push( i, d88[Drv]->Getc88() );
		}
	}
	
	PushStatus( N );	// N
	PushStatus( R );	// R
	PushStatus( H );	// H
	PushStatus( C );	// C
	PushStatus( 0 );	// st2
	PushStatus( 0 );	// st1
	
	PushStatus( IsMount( Drv ) ? 0 : ST0_NOT_READY );	// st0  bit3 : media not ready
	
        FDCStatus = FDC_DATA_READY | FDC_FD2PC | ( FDCStatus & 0x0f );
}


////////////////////////////////////////////////////////////////
// Write
////////////////////////////////////////////////////////////////
void DSK66::Write( void )
{
	PRINTD5( DISK_LOG, "[DSK66][Write] Drv:%d C:%d H:%d R:%d N:%d\n", CmdIn.Data[1]&3, CmdIn.Data[2], CmdIn.Data[3], CmdIn.Data[4], CmdIn.Data[5] );
	
	int Drv,C,H,R,N;
	int i,j;
	
	Drv = CmdIn.Data[1]&3;		// ドライブNo.(0-3) 0-1に制限しておいた方がいい?
	C   = CmdIn.Data[2];		// シリンダ
//	C   = LastCylinder[Drv];	// シリンダ
	H   = CmdIn.Data[3];		// ヘッドアドレス
	R   = CmdIn.Data[4];		// セクタNo.
	N   = CmdIn.Data[5] ? CmdIn.Data[5]*256 : 256;	// セクタサイズ
	
	if( IsMount( Drv ) ){
		// シーク
		// トラックNoを2倍(1D->2D)
		d88[Drv]->Seek88( C*2, R );
		for( i=0; i<SendSectors; i++ ){
//			for( j=0; j<N; j++ )
			for( j=0; j<0x100; j++ )
				d88[Drv]->Putc88( cFDCbuf::Pop( i ) );	// write data
		}
	}
	
	PushStatus( N );	// N
	PushStatus( R );	// R
	PushStatus( H );	// H
	PushStatus( C );	// C
	PushStatus( 0 );	// st2
	PushStatus( 0 );	// st1
	
	PushStatus( IsMount( Drv ) ? 0 : ST0_NOT_READY );	// st0  bit3 : media not ready
	
        FDCStatus = FDC_DATA_READY | FDC_FD2PC | ( FDCStatus & 0x0f );
}


////////////////////////////////////////////////////////////////
// Recalibrate
////////////////////////////////////////////////////////////////
void DSK66::Recalibrate( void )
{
	PRINTD( DISK_LOG, "[DSK66][Recalibrate]\n" );
	
	CmdIn.Data[2] = 0;	// トラック0をシーク
	Seek();
}


////////////////////////////////////////////////////////////////
// Seek
////////////////////////////////////////////////////////////////
void DSK66::Seek( void )
{
	PRINTD2( DISK_LOG, "[DSK66][Seek] Drv:%d C:%d\n", CmdIn.Data[1]&3, CmdIn.Data[2] );
	
	int Drv,C;
	
	Drv = CmdIn.Data[1]&3;		// ドライブNo.(0-3) 0-1に制限しておいた方がいい?
	C   = CmdIn.Data[2];		// シリンダ
	
	if( ( Drv > DrvNum ) || !IsMount( Drv ) ){	// 無効ドライブ or ディスクがマウントされてない?
		SeekST0[Drv]      = ST0_IC_AT | ST0_SEEK_END | ST0_NOT_READY | Drv;
		SeekEnd[Drv]      = FALSE;
		LastCylinder[Drv] = 0;
	}else{										// シーク実行
		// トラックNoを2倍(1D->2D)
		d88[Drv]->Seek88( C*2, 1 );
		SeekST0[Drv]      = ST0_IC_NT | ST0_SEEK_END | Drv;
		SeekEnd[Drv]      = TRUE;
		LastCylinder[Drv] = C;
		
		// ウェイト設定
		int eid = EID_SEEK1;
		switch( Drv ){
		case 0: eid = EID_SEEK1; break;
		case 1: eid = EID_SEEK2; break;
		case 2: eid = EID_SEEK3; break;
		case 3: eid = EID_SEEK4; break;
		}
		if( !DSK6::SetWait( eid, SRT*WAIT_SEEK ) ){
                        FDCStatus |= 1<<Drv;
		}
	}
        FDCStatus = FDC_DATA_READY | ( FDCStatus & 0x0f );
}


////////////////////////////////////////////////////////////////
// Sense Interrupt Status
////////////////////////////////////////////////////////////////
void DSK66::SenseInterruptStatus( void )
{
	PRINTD( DISK_LOG, "[DSK66][SenseInterruptStatus]\n" );
	
	int Drv;
	
	// シーク完了ドライブを探す
	for( Drv=0; Drv<DrvNum; Drv++ )
		if( SeekEnd[Drv] ) break;
	
	if( Drv < DrvNum ){	// シーク完了ドライブあり
		SeekEnd[Drv] = FALSE;
		PushStatus( LastCylinder[Drv] );
		PushStatus( SeekST0[Drv] );
	}else{				// シーク完了ドライブなし
		PushStatus( 0 );
		PushStatus( ST0_IC_IC );
	}
	
        FDCStatus = FDC_DATA_READY | FDC_FD2PC | ( FDCStatus & 0x0f );
//	Status = FDC_DATA_READY | ( Status & 0x0f );
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
void DSK66::OutB1H( int, BYTE data ){ DIO = data&2 ? TRUE : FALSE; }	// FDモード設定
void DSK66::OutB2H( int, BYTE data ){}									// FDC INT?
void DSK66::OutB3H( int, BYTE data ){}									// PortB2hの入出力制御
//void DSK66::OutD0H( int, BYTE data ){ if( !DIO ) cFDCbuf::Push( 0, data ); }	// Buffer
//void DSK66::OutD1H( int, BYTE data ){ if( !DIO ) cFDCbuf::Push( 1, data ); }	// Buffer
//void DSK66::OutD2H( int, BYTE data ){ if( !DIO ) cFDCbuf::Push( 2, data ); }	// Buffer
//void DSK66::OutD3H( int, BYTE data ){ if( !DIO ) cFDCbuf::Push( 3, data ); }	// Buffer
void DSK66::OutD0H( int, BYTE data ){ cFDCbuf::Push( 0, data ); }		// Buffer
void DSK66::OutD1H( int, BYTE data ){ cFDCbuf::Push( 1, data ); }		// Buffer
void DSK66::OutD2H( int, BYTE data ){ cFDCbuf::Push( 2, data ); }		// Buffer
void DSK66::OutD3H( int, BYTE data ){ cFDCbuf::Push( 3, data ); }		// Buffer
void DSK66::OutD6H( int, BYTE data ){}									// ドライブセレクト
void DSK66::OutD8H( int, BYTE data ){}									// 書き込み補償制御 ???
void DSK66::OutDAH( int, BYTE data ){ SendSectors = ~( data - 0x10 ); }	// 転送量指定
void DSK66::OutDDH( int, BYTE data ){ OutFDC( data ); }					// FDC データレジスタ
void DSK66::OutDEH( int, BYTE data ){}									// ?

BYTE DSK66::InB2H( int ){ return 1; }									// FDC INT
//BYTE DSK66::InD0H( int ){ return DIO ? cFDCbuf::Pop( 0 ) : 0; }		// Buffer
//BYTE DSK66::InD1H( int ){ return DIO ? cFDCbuf::Pop( 1 ) : 0; }		// Buffer
//BYTE DSK66::InD2H( int ){ return DIO ? cFDCbuf::Pop( 2 ) : 0; }		// Buffer
//BYTE DSK66::InD3H( int ){ return DIO ? cFDCbuf::Pop( 3 ) : 0; }		// Buffer
BYTE DSK66::InD0H( int ){ return cFDCbuf::Pop( 0 ); }					// Buffer
BYTE DSK66::InD1H( int ){ return cFDCbuf::Pop( 1 ); }					// Buffer
BYTE DSK66::InD2H( int ){ return cFDCbuf::Pop( 2 ); }					// Buffer
BYTE DSK66::InD3H( int ){ return cFDCbuf::Pop( 3 ); }					// Buffer
BYTE DSK66::InD4H( int ){ return 0; }									// ?
BYTE DSK66::InDCH( int ){ return FDCStatus; }								// FDC ステータスレジスタ
BYTE DSK66::InDDH( int ){ return InFDC(); }								// FDC データレジスタ


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
BOOL DSK60::DokoSave( cIni *Ini )
{
	cSche::evinfo e;
	char stren[16];
	
	e.device = this;
	
	if( !Ini ) return FALSE;
	
	// DSK6
	Ini->PutEntry( "P6DISK", NULL, "DrvNum", "%d", DrvNum );
	
	// d88
	for( int i=0; i<DrvNum; i++ ){
		if( d88[i] ){
			char stren[16];
			sprintf( stren, "D88_%d", i );
			Ini->PutEntry( stren, NULL, "FileName", "%s", d88[i]->GetFileName() );
		}
	}
	
	// DSK60
	Ini->PutEntry( "P6DISK", NULL, "mdisk_ATN",		"%d",		mdisk.ATN );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_DAC",		"%d",		mdisk.DAC );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_RFD",		"%d",		mdisk.RFD );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_DAV",		"%d",		mdisk.DAV );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_command",	"%d",		mdisk.command );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_step",	"%d",		mdisk.step );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_blk",		"%d",		mdisk.blk );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_drv",		"%d",		mdisk.drv );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_trk",		"%d",		mdisk.trk );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_sct",		"%d",		mdisk.sct );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_size",	"%d",		mdisk.size );
	Ini->PutEntry( "P6DISK", NULL, "mdisk_retdat",	"0x%02X",	mdisk.retdat );
	Ini->PutEntry( "P6DISK", NULL, "io_D1H",		"0x%02X",	io_D1H );
	Ini->PutEntry( "P6DISK", NULL, "io_D2H",		"0x%02X",	io_D1H );
	Ini->PutEntry( "P6DISK", NULL, "io_D3H",		"0x%02X",	io_D1H );
	
	// イベント
	e.id = EID_SEEK1;
	if( vm->sche->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "P6DISK", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	e.id = EID_SEEK2;
	if( vm->sche->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "P6DISK", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	return TRUE;
}

BOOL DSK66::DokoSave( cIni *Ini )
{
	cSche::evinfo e;
	char stren[16],strva[256];
	
	e.device = this;
	
	if( !Ini ) return FALSE;
	
	// DSK6
	Ini->PutEntry( "P66DISK", NULL, "DrvNum", "%d", DrvNum );
	
	// d88
	for( int i=0; i<DrvNum; i++ ){
		if( d88[i] ){
			char stren[16];
			sprintf( stren, "D88_%d", i );
			Ini->PutEntry( stren, NULL, "FileName", "%s", d88[i]->GetFileName() );
		}
	}
	
	// cFDCbuf
	for( int i=0; i<4; i++ ){
		// Data
		for( int j=0; j<256; j+=64 ){
			sprintf( stren, "Data_%d_%02X", i, j );
			for( int k=0; k<64; k++ ) sprintf( &strva[k*2], "%02X", Data[i][j+k] );
			Ini->PutEntry( "P66DISK", NULL, stren, "%s", strva );
		}
		// Index
		sprintf( stren, "Index_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "%d", Index[i] );
	}
	
	// DSK66
	for( int i=0; i<10; i++ ){
		sprintf( stren, "CmdIn_Data_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "0x%02X", CmdIn.Data[i] );
		sprintf( stren, "CmdOut_Data_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "0x%02X", CmdOut.Data[i] );
	}
	Ini->PutEntry( "P66DISK", NULL, "CmdIn_Index", "%d", CmdIn.Index );
	Ini->PutEntry( "P66DISK", NULL, "CmdOut_Index", "%d", CmdOut.Index );
	
	for( int i=0; i<DrvNum; i++ ){
		sprintf( stren, "SeekST0_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "0x%02X",	SeekST0[i] );
		sprintf( stren, "LastCylinder_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "0x%02X",	LastCylinder[i] );
		sprintf( stren, "SeekEnd_%d", i );
		Ini->PutEntry( "P66DISK", NULL, stren, "%s",		SeekEnd[i] ? "Yes" : "No" );
	}
	
	Ini->PutEntry( "P66DISK", NULL, "SendSectors",	"0x%02X",	SendSectors );
	Ini->PutEntry( "P66DISK", NULL, "DIO",			"%s",		DIO ? "Yes" : "No" );
        Ini->PutEntry( "P66DISK", NULL, "Status",		"0x%02X",	FDCStatus );
	
	// イベント
	e.id = EID_SEEK1;
	if( vm->sche->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "P66DISK", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	e.id = EID_SEEK2;
	if( vm->sche->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "P66DISK", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	e.id = EID_OUTFDC;
	if( vm->sche->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "P66DISK", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	e.id = EID_EXECFDC;
	if( vm->sche->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "P66DISK", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
BOOL DSK60::DokoLoad( cIni *Ini )
{
	int st,yn;
	cSche::evinfo e;
	char stren[16];
	char strrs[64];
	
	e.device = this;
	
	if( !Ini ) return FALSE;
	
	// とりあえず全部アンマウント
	for( int i=0; i<DrvNum; i++ ) if( d88[i] ) Unmount( i) ;
	
	// DSK6
	Ini->GetInt( "P6DISK", "DrvNum",	&DrvNum,	DrvNum );
	
	// d88
	for( int i=0; i<DrvNum; i++ ){
		char stren[16], strva[256];
		sprintf( stren, "D88_%d", i );
		if( Ini->GetString( stren, "FileName", strva, "" ) ) Mount( i, strva );
	}
	
	// DSK60
	Ini->GetInt( "P6DISK", "mdisk_ATN",		&mdisk.ATN,		mdisk.ATN );
	Ini->GetInt( "P6DISK", "mdisk_DAC",		&mdisk.DAC,		mdisk.DAC );
	Ini->GetInt( "P6DISK", "mdisk_RFD",		&mdisk.RFD,		mdisk.RFD );
	Ini->GetInt( "P6DISK", "mdisk_DAV",		&mdisk.DAV,		mdisk.DAV );
	Ini->GetInt( "P6DISK", "mdisk_command",	&mdisk.command,	mdisk.command );
	Ini->GetInt( "P6DISK", "mdisk_step",	&mdisk.step,	mdisk.step );
	Ini->GetInt( "P6DISK", "mdisk_blk",		&mdisk.blk,		mdisk.blk );
	Ini->GetInt( "P6DISK", "mdisk_drv",		&mdisk.drv,		mdisk.drv );
	Ini->GetInt( "P6DISK", "mdisk_trk",		&mdisk.trk,		mdisk.trk );
	Ini->GetInt( "P6DISK", "mdisk_sct",		&mdisk.sct,		mdisk.sct );
	Ini->GetInt( "P6DISK", "mdisk_size",	&mdisk.size,	mdisk.size );
	Ini->GetInt( "P6DISK", "mdisk_retdat",	&st,			mdisk.retdat );	mdisk.retdat = st;
	Ini->GetInt( "P6DISK", "io_D1H",		&st,			io_D1H );		io_D1H = st;
	Ini->GetInt( "P6DISK", "io_D2H",		&st,			io_D2H );		io_D2H = st;
	Ini->GetInt( "P6DISK", "io_D3H",		&st,			io_D3H );		io_D3H = st;
	
	// イベント
	e.id = EID_SEEK1;
	sprintf( stren, "Event%08X", e.id );
	if( Ini->GetString( "P6DISK", stren, strrs, "" ) ){
		sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
		e.Active = yn ? TRUE : FALSE;
		if( !vm->sche->SetEvinfo( &e ) ) return FALSE;
	}
	
	e.id = EID_SEEK2;
	sprintf( stren, "Event%08X", e.id );
	if( Ini->GetString( "P6DISK", stren, strrs, "" ) ){
		sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
		e.Active = yn ? TRUE : FALSE;
		if( !vm->sche->SetEvinfo( &e ) ) return FALSE;
	}
	
	return TRUE;
}

BOOL DSK66::DokoLoad( cIni *Ini )
{
	int st,yn;
	cSche::evinfo e;
	char stren[16], strva[256];
	char strrs[64];
	
	e.device = this;
	
	if( !Ini ) return FALSE;
	
	// とりあえず全部アンマウント
	for( int i=0; i<DrvNum; i++ ) if( d88[i] ) Unmount( i) ;
	
	// DSK6
	Ini->GetInt( "P66DISK", "DrvNum",	&DrvNum,	DrvNum );
	
	// d88
	for( int i=0; i<DrvNum; i++ ){
		sprintf( stren, "D88_%d", i );
		if( Ini->GetString( stren, "FileName", strva, "" ) ) Mount( i, strva );
	}
	
	// cFDCbuf
	for( int i=0; i<4; i++ ){
		// Data
		for( int j=0; j<256; j+=64 ){
			sprintf( stren, "Data_%d_%02X", i, j );
			memset( strva, '0', 64*2 );
			Ini->GetString( "P66DISK", stren, strva, strva );
			
			for( int k=0; k<64; k++ ){
				char dt[5] = "0x";
				strncpy( dt, &strva[k*2], 2 );
				Data[i][j+k] = strtol( dt, NULL, 16 );
			}
		}
		// Index
		sprintf( stren, "Index_%d", i );
		Ini->GetInt( "P66DISK", stren,	&Index[i],	Index[i] );
	}
	
	// DSK66
	for( int i=0; i<10; i++ ){
		sprintf( stren, "CmdIn_Data_%d", i );
		Ini->GetInt( "P66DISK", stren,	&st,	CmdIn.Data[i] );	CmdIn.Data[i] = st;
		sprintf( stren, "CmdOut_Data_%d", i );
		Ini->GetInt( "P66DISK", stren,	&st,	CmdOut.Data[i] );	CmdOut.Data[i] = st;
	}
	Ini->GetInt( "P66DISK", "CmdIn_Index",	&CmdIn.Index,	CmdIn.Index );
	Ini->GetInt( "P66DISK", "CmdOut_Index",	&CmdOut.Index,	CmdOut.Index );
	
	for( int i=0; i<DrvNum; i++ ){
		sprintf( stren, "SeekST0_%d", i );
		Ini->GetInt(   "P66DISK", stren,	&st,			SeekST0[i] ); 		SeekST0[i] = st;
		sprintf( stren, "LastCylinder_%d", i );
		Ini->GetInt(   "P66DISK", stren,	&st,			LastCylinder[i] );	LastCylinder[i] = st;
		sprintf( stren, "SeekEnd_%d", i );
		Ini->GetTruth( "P66DISK", stren,	&SeekEnd[i],	SeekEnd[i] );
	}
	Ini->GetInt(   "P66DISK", "SendSectors",	&st,	SendSectors );	SendSectors = st;
	Ini->GetTruth( "P66DISK", "DIO",			&DIO,	DIO );
        Ini->GetInt(   "P66DISK", "Status",			&st,	FDCStatus );		FDCStatus = st;
	
	// イベント
	e.id = EID_SEEK1;
	sprintf( stren, "Event%08X", e.id );
	if( Ini->GetString( "P66DISK", stren, strrs, "" ) ){
		sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
		e.Active = yn ? TRUE : FALSE;
		if( !vm->sche->SetEvinfo( &e ) ) return FALSE;
	}
	
	e.id = EID_SEEK2;
	sprintf( stren, "Event%08X", e.id );
	if( Ini->GetString( "P66DISK", stren, strrs, "" ) ){
		sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
		e.Active = yn ? TRUE : FALSE;
		if( !vm->sche->SetEvinfo( &e ) ) return FALSE;
	}
	
	e.id = EID_OUTFDC;
	sprintf( stren, "Event%08X", e.id );
	if( Ini->GetString( "P66DISK", stren, strrs, "" ) ){
		sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
		e.Active = yn ? TRUE : FALSE;
		if( !vm->sche->SetEvinfo( &e ) ) return FALSE;
	}
	
	e.id = EID_EXECFDC;
	sprintf( stren, "Event%08X", e.id );
	if( Ini->GetString( "P66DISK", stren, strrs, "" ) ){
		sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
		e.Active = yn ? TRUE : FALSE;
		if( !vm->sche->SetEvinfo( &e ) ) return FALSE;
	}
	
	return TRUE;
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
	STATIC_CAST( Device::InFuncPtr, &DSK60::InD2H ),
	STATIC_CAST( Device::InFuncPtr, &DSK60::InD3H )
};


const Device::Descriptor DSK66::descriptor = {
	DSK66::indef, DSK66::outdef
};

const Device::OutFuncPtr DSK66::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutB1H ),
	STATIC_CAST( Device::OutFuncPtr, &DSK66::OutB2H ),
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
