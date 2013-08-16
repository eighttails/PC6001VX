#include <stdlib.h>
#include <new>

#include "log.h"
#include "memory.h"
#include "common.h"
#include "error.h"
#include "osd.h"


// メモリブロック割り当て

// RomB[0]  : EmptyRom
// RomB[1]  : EmptyRam
// RomB[2]  : ExtRom
// RomB[3]  : ExtRom+0x2000
// RomB[4]  : MainRom
// RomB[5]  : MainRom+0x2000
// RomB[6]  : MainRom+0x4000
// RomB[7]  : MainRom+0x6000
// RomB[8]  : MainRom+0x8000
// RomB[9]  : MainRom+0xa000
// RomB[10] : MainRom+0xc000
// RomB[11] : MainRom+0xe000
// RomB[12] : CGRom1
// RomB[13] : CGRom2
// RomB[14] : KanjiRom
// RomB[15] : KanjiRom+0x2000
// RomB[16] : KanjiRom+0x4000
// RomB[17] : KanjiRom+0x6000
// RomB[18] : VoiceRom
// RomB[19] : VoiceRom+0x2000

// RamB[0]  : IntRam
// RamB[1]  : IntRam+0x2000
// RamB[2]  : IntRam+0x4000
// RamB[3]  : IntRam+0x6000
// RamB[4]  : IntRam+0x8000
// RamB[5]  : IntRam+0xa000
// RamB[6]  : IntRam+0xc000
// RamB[7]  : IntRam+0xe000
// RamB[8]  : ExtRam
// RamB[9]  : ExtRam+0x2000
// RamB[10] : ExtRam+0x4000
// RamB[11] : ExtRam+0x6000
// RamB[12] : ExtRam+0x8000
// RamB[13] : ExtRam+0xa000
// RamB[14] : ExtRam+0xc000
// RamB[15] : ExtRam+0xe000

#define EMPTYROM	(RomB[0])
#define EMPTYRAM	(RomB[1])
#define EXTROM0		(RomB[2])
#define EXTROM1		(RomB[3])
#define MAINROM0	(RomB[4])
#define MAINROM1	(RomB[5])
#define MAINROM2	(RomB[6])
#define MAINROM3	(RomB[7])
#define MAINROM4	(RomB[8])
#define MAINROM5	(RomB[9])
#define MAINROM6	(RomB[10])
#define MAINROM7	(RomB[11])
#define CGROM1		(RomB[12])
#define CGROM2		(RomB[13])
#define KANJIROM0	(RomB[14])
#define KANJIROM1	(RomB[15])
#define KANJIROM2	(RomB[16])
#define KANJIROM3	(RomB[17])
#define VOICEROM0	(RomB[18])
#define VOICEROM1	(RomB[19])
#define MAINRAM0	(RamB[0])
#define MAINRAM1	(RamB[1])
#define MAINRAM2	(RamB[2])
#define MAINRAM3	(RamB[3])
#define MAINRAM4	(RamB[4])
#define MAINRAM5	(RamB[5])
#define MAINRAM6	(RamB[6])
#define MAINRAM7	(RamB[7])
#define EXTRAM0		(RamB[8])
#define EXTRAM1		(RamB[9])
#define EXTRAM2		(RamB[10])
#define EXTRAM3		(RamB[11])
#define EXTRAM4		(RamB[12])
#define EXTRAM5		(RamB[13])
#define EXTRAM6		(RamB[14])
#define EXTRAM7		(RamB[15])
#define SOLRAM		(RamB[11])


#define pEMPTYROM	((MemBlock *)&EMPTYROM)
#define pEMPTYRAM	((MemBlock *)&EMPTYRAM)
#define pEXTROM0	(UseExtRom ? (MemBlock *)&EXTROM0 : pEMPTYROM)
#define pEXTROM1	(UseExtRom ? (MemBlock *)&EXTROM1 : pEMPTYROM)
#define pMAINROM0	((MemBlock *)&MAINROM0)
#define pMAINROM1	((MemBlock *)&MAINROM1)
#define pMAINROM2	((MemBlock *)&MAINROM2)
#define pMAINROM3	((MemBlock *)&MAINROM3)
#define pMAINROM4	((MemBlock *)&MAINROM4)
#define pMAINROM5	((MemBlock *)&MAINROM5)
#define pMAINROM6	((MemBlock *)&MAINROM6)
#define pMAINROM7	((MemBlock *)&MAINROM7)
#define pCGROM1		((MemBlock *)&CGROM1)
#define pCGROM2		((MemBlock *)&CGROM2)
#define pKANJIROM0	((MemBlock *)&KANJIROM0)
#define pKANJIROM1	((MemBlock *)&KANJIROM1)
#define pKANJIROM2	((MemBlock *)&KANJIROM2)
#define pKANJIROM3	((MemBlock *)&KANJIROM3)
#define pVOICEROM0	((MemBlock *)&VOICEROM0)
#define pVOICEROM1	((MemBlock *)&VOICEROM1)
#define pMAINRAM0	((MemBlock *)&MAINRAM0)
#define pMAINRAM1	((MemBlock *)&MAINRAM1)
#define pMAINRAM2	((MemBlock *)&MAINRAM2)
#define pMAINRAM3	((MemBlock *)&MAINRAM3)
#define pMAINRAM4	((MemBlock *)&MAINRAM4)
#define pMAINRAM5	((MemBlock *)&MAINRAM5)
#define pMAINRAM6	((MemBlock *)&MAINRAM6)
#define pMAINRAM7	((MemBlock *)&MAINRAM7)
#define pEXTRAM0	(UseExtRam ? (MemBlock *)&EXTRAM0 : pEMPTYRAM)
#define pEXTRAM1	(UseExtRam ? (MemBlock *)&EXTRAM1 : pEMPTYRAM)
#define pEXTRAM2	(UseExtRam ? (MemBlock *)&EXTRAM2 : pEMPTYRAM)
#define pEXTRAM3	(UseExtRam ? (MemBlock *)&EXTRAM3 : pEMPTYRAM)
#define pEXTRAM4	(UseExtRam ? (MemBlock *)&EXTRAM4 : pEMPTYRAM)
#define pEXTRAM5	(UseExtRam ? (MemBlock *)&EXTRAM5 : pEMPTYRAM)
#define pEXTRAM6	(UseExtRam ? (MemBlock *)&EXTRAM6 : pEMPTYRAM)
#define pEXTRAM7	(UseExtRam ? (MemBlock *)&EXTRAM7 : pEMPTYRAM)
#define pKNJROM0	(kj_rom ? ( kj_LR ? pKANJIROM2 : pKANJIROM0 ) : pVOICEROM0 )
#define pKNJROM1	(kj_rom ? ( kj_LR ? pKANJIROM3 : pKANJIROM1 ) : pVOICEROM1 )
#define pSOLRAM		((MemBlock *)&SOLRAM)


// メモリコントローラ内部レジスタ初期値
#define	INIT_RF0	(0x71)
#define INIT_RF1	(0xdd)
#define INIT_RF2	(0x50)


////////////////////////////////////////////////////////////////
// ROM情報
////////////////////////////////////////////////////////////////
// PC-6001                                  ROMファイル名  CRC32
const MEM6::ROMINFO MEM60::AMAINROM[] = { { "BASICROM.60", 0x54c03109 } };
const MEM6::ROMINFO MEM60::ACGROM0[]  = { { "CGROM60S.60", 0,         } };
const MEM6::ROMINFO MEM60::ACGROM1[]  = { { "CGROM60.60",  0xb0142d32 } };

// PC-6001mk2                               ROMファイル名  CRC32
const MEM6::ROMINFO MEM62::AMAINROM[] = { { "BASICROM.62", 0x950ac401 },	// 前期
										  { "BASICROM.62", 0xd7e61957 } };	// 後期
const MEM6::ROMINFO MEM62::ACGROM1[]  = { { "CGROM60.62",  0x81eb5d95 } };
const MEM6::ROMINFO MEM62::ACGROM2[]  = { { "CGROM60m.62", 0x3ce48c33 } };
const MEM6::ROMINFO MEM62::AKANJI[]   = { { "KANJIROM.62", 0x20c8f3eb } };
const MEM6::ROMINFO MEM62::AVOICE[]   = { { "VOICEROM.62", 0x49b4f917 } };

// PC-6601                                  ROMファイル名  CRC32
const MEM6::ROMINFO MEM66::AMAINROM[] = { { "BASICROM.66", 0xc0b01772 } };
const MEM6::ROMINFO MEM66::ACGROM1[]  = { { "CGROM60.66",  0xd2434f29 } };
const MEM6::ROMINFO MEM66::ACGROM2[]  = { { "CGROM66.66",  0x3ce48c33 } };
const MEM6::ROMINFO MEM66::AKANJI[]   = { { "KANJIROM.66", 0x20c8f3eb } };
const MEM6::ROMINFO MEM66::AVOICE[]   = { { "VOICEROM.66", 0x91d078c1 } };

// PC-6001mk2SR                             ROMファイル名  CRC32
// PC-6601SR                                ROMファイル名  CRC32


////////////////////////////////////////////////////////////////
// メモリ情報
////////////////////////////////////////////////////////////////
// 共通                                 ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM6::IEMPTROM  = { NULL,     0,                 0x02000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM6::IEMPTRAM  = { NULL,     0,                 0x02000, 0xff,  0, 0 };
const MEM6::MEMINFO MEM6::IEXTROM   = { NULL,     0,                 0x20000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM6::IEXTRAM   = { NULL,     0,                 0x20000, 0x00,  0, 0 };
										// MEGA-ROM/RAMカートリッジ対応のため
										// 拡張ROM,RAM領域は128KB確保しておく

// ----------------------------------------------------------------------------------------
// 戦士のカートリッジ                   ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM6::ISOLRAM   = { NULL,     0,                 0x02000, 0xff,  1, 1 };
// ----------------------------------------------------------------------------------------

// PC-6001                              ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM60::IMAINROM = { AMAINROM, COUNTOF(AMAINROM), 0x04000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM60::ICGROM0  = { ACGROM0,  COUNTOF(ACGROM0),  0x00400, 0xff,  0, 0 };
const MEM6::MEMINFO MEM60::ICGROM1  = { ACGROM1,  COUNTOF(ACGROM1),  0x01000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM60::IINTRAM  = { NULL,     0,                 0x04000, 0x00,  0, 0 };

// PC-6001mk2                           ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM62::IMAINROM = { AMAINROM, COUNTOF(AMAINROM), 0x08000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM62::ICGROM1  = { ACGROM1,  COUNTOF(ACGROM1),  0x02000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM62::ICGROM2  = { ACGROM2,  COUNTOF(ACGROM2),  0x02000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM62::IKANJI   = { AKANJI,   COUNTOF(AKANJI),   0x08000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM62::IVOICE   = { AVOICE,   COUNTOF(AVOICE),   0x04000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM62::IINTRAM  = { NULL,     0,                 0x10000, 0x00,  0, 0 };

// PC-6601                              ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM66::IMAINROM = { AMAINROM, COUNTOF(AMAINROM), 0x08000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM66::ICGROM1  = { ACGROM1,  COUNTOF(ACGROM1),  0x02000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM66::ICGROM2  = { ACGROM2,  COUNTOF(ACGROM2),  0x02000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM66::IKANJI   = { AKANJI,   COUNTOF(AKANJI),   0x08000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM66::IVOICE   = { AVOICE,   COUNTOF(AVOICE),   0x04000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM66::IINTRAM  = { NULL,     0,                 0x10000, 0x00,  0, 0 };

// PC-6001mk2SR                         ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
// PC-6601SR                            ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)



//--------------------------------------------------------------
// メモリブロック
//--------------------------------------------------------------

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
MemBlock::MemBlock( void ) : MB(NULL), RWait(0), WWait(0), WPt(false)
{
	INITARRAY( Name, '\0' );
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
MemBlock::~MemBlock( void ){}


////////////////////////////////////////////////////////////////
// メモリブロック設定
//
// 引数:	name	メモリブロック名
//			mem		メモリブロックへのポインタ
//			rw		アクセスウェイト(読込み)
//			ww		アクセスウェイト(書込み)
// 返値:	なし
////////////////////////////////////////////////////////////////
void MemBlock::SetMemory( const char *name, BYTE *mem, int rw, int ww )
{
	strncpy( Name, name, 32 );
	MB    = mem;
	RWait = rw;
	WWait = ww;
}


////////////////////////////////////////////////////////////////
// アクセスウェイト設定
//
// 引数:	rw		アクセスウェイト(読込み)
//			ww		アクセスウェイト(書込み)
// 返値:	なし
////////////////////////////////////////////////////////////////
void MemBlock::SetWait( int rw, int ww )
{
	RWait = rw;
	WWait = ww;
}


////////////////////////////////////////////////////////////////
// アクセスウェイト取得
//
// 引数:	なし
// 返値:	bit0-7	読込み時ウェイト
//			bit8-15	書込み時ウェイト
////////////////////////////////////////////////////////////////
int MemBlock::GetWait( void )
{
	return ((WWait&0xff)<<8) | (RWait&0xff);
}


////////////////////////////////////////////////////////////////
// ライトプロテクト設定
//
// 引数:	prt		ライトプロテクトフラグ true:セット false：解除
// 返値:	なし
////////////////////////////////////////////////////////////////
void MemBlock::SetProtect( bool prt )
{
	WPt = prt;
}


////////////////////////////////////////////////////////////////
// ライトプロテクト取得
//
// 引数:	なし
// 返値:	bool	true:セット false：解除
////////////////////////////////////////////////////////////////
bool MemBlock::GetProtect( void )
{
	return WPt;
}


////////////////////////////////////////////////////////////////
// メモリリード
////////////////////////////////////////////////////////////////
BYTE MemBlock::Read( WORD addr, int *wcnt )
{
	if( wcnt ) (*wcnt) += RWait;
	return MB[addr&0x1fff];
}


////////////////////////////////////////////////////////////////
// メモリライト
////////////////////////////////////////////////////////////////
void MemBlock::Write( WORD addr, BYTE data, int *wcnt )
{
	if( wcnt ) (*wcnt) += WWait;
	if( !WPt ) MB[addr&0x1fff] = data;
}




////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
MEM6::MEM6( bool extram ) : 
	CGBank(false), UseExtRom(false), UseExtRam(extram),
	MainRom(NULL), ExtRom(NULL), CGRom1(NULL), IntRam(NULL),
	ExtRam(NULL), EmptyRom(NULL), EmptyRam(NULL),
	M1Wait(1), EnableChkCRC(true),
	UseSol(false), SolBank(0)
{
	INITARRAY( Rm_blk, NULL );
	INITARRAY( Wm_blk, NULL );
	INITARRAY( ram_sel, 0xff );
	INITARRAY( FilePath, '\0' );
}

MEM60::MEM60( const ID& id, bool extram ) : MEM6(extram), Device(id), CGRom0(NULL) {}

MEM62::MEM62( const ID& id, bool extram ) : MEM6(extram), Device(id),
	CGRom2(NULL), KanjiRom(NULL), VoiceRom(NULL),
	cgrom(true), kj_rom(true), kj_LR(true), cgenable(true), cgaddr(3),
	Rf0(INIT_RF0), Rf1(INIT_RF1), Rf2(INIT_RF2) {}

MEM66::MEM66( const ID& id, bool extram ) : MEM62(id,extram) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
MEM6::~MEM6( void )
{
	if( MainRom )	delete[] MainRom;
	if( IntRam )	delete[] IntRam;
	if( CGRom1 )	delete[] CGRom1;
	if( ExtRom )	delete[] ExtRom;
	if( EmptyRom )	delete[] EmptyRom;
	if( EmptyRam )	delete[] EmptyRam;
}

MEM60::~MEM60( void ){}

MEM62::~MEM62( void )
{
	if( CGRom2 )	delete[] CGRom2;
	if( KanjiRom )	delete[] KanjiRom;
	if( VoiceRom )	delete[] VoiceRom;
}

MEM66::~MEM66( void ){}


////////////////////////////////////////////////////////////////
// 拡張ROM マウント
////////////////////////////////////////////////////////////////
bool MEM6::MountExtRom( char *filename )
{
	PRINTD( MEM_LOG, "[MEM][MountExtRom] -> %s\n", filename );
	
	UnmountExtRom();	// 一旦開放
	
	try{
		FILE *fp = FOPENEN( filename, "rb" );
		if( !fp ) throw Error::ExtRomMountFailed;
		fread( ExtRom, sizeof(BYTE), IEXTROM.Size, fp );
		fclose( fp );
		
		// ファイルパス保存
		strncpy( FilePath, filename, PATH_MAX );
	}
	// 例外発生
	catch( Error::Errno i ){
		Error::SetError( i );
		
		UnmountExtRom();
		return false;
	}
	
	UseExtRom = true;
	
	return true;
}


////////////////////////////////////////////////////////////////
// 拡張ROM アンマウント
////////////////////////////////////////////////////////////////
void MEM6::UnmountExtRom( void )
{
	PRINTD( MEM_LOG, "[MEM][UnmountExtRom]\n" );
	
	memset( ExtRom, IEXTROM.Init, IEXTROM.Size );
	*FilePath = '\0';
	
	UseExtRom = false;
}


////////////////////////////////////////////////////////////////
// 拡張ROMファイルパス取得
////////////////////////////////////////////////////////////////
char *MEM6::GetFile( void )
{
	return FilePath;
}


////////////////////////////////////////////////////////////////
// メモリ確保とROMファイル読込み
////////////////////////////////////////////////////////////////
bool MEM6::AllocMemory( BYTE **buf, const MEMINFO *info, char *path )
{
	PRINTD( MEM_LOG, "[MEM][AllocMemory] " );
	
	DWORD LoadSize = 0;
	int i = 0;
	bool ErrSize = false;
	bool ErrCrc = false;
	
	try{
		// メモリ確保(最小サイズ8KB)
		if( *buf) delete [] *buf;
		*buf = new BYTE[max( info->Size, 0x2000 )];
		memset( *buf, info->Init, info->Size );
		
		// ROM情報なし ならばRAMまたはNULL
		if( !info->Rinf ) return true;
		
		do{
			PRINTD( MEM_LOG, "-> %s ", info->Rinf[i].FileName );
			
			// ファイルから読込み
			char fpath[PATH_MAX] = "";
			if( path ){
				strncpy( fpath, path, PATH_MAX );
				AddDelimiter( fpath );
			}
			strncat( fpath, info->Rinf[i].FileName, PATH_MAX );
			
			FILE *fp = FOPENEN( fpath, "rb" );
			if( fp ){
				LoadSize = fread( *buf, sizeof(BYTE), info->Size, fp );
				fclose( fp );
				
				// ファイルサイズチェック
				if( LoadSize != info->Size )
					ErrSize = true;
				// CRCチェック
				// EnableChkCRC=false または CRC=0の時はチェックしない
				else if( EnableChkCRC && (info->Rinf[i].Crc != 0) &&
					( CalcCrc32( *buf, info->Size ) != info->Rinf[i].Crc ) )
					ErrCrc = true;
				else{
					PRINTD( MEM_LOG, "-> OK\n" );
					return true;
				}
			}
		}while( ++i < info->Rnum );
		
		if     ( ErrCrc )  throw Error::RomCrcNG;
		else if( ErrSize ) throw Error::RomSizeNG;
		else               throw Error::NoRom;
	}
	// new に失敗した場合
	catch( std::bad_alloc ){
		PRINTD( MEM_LOG, "-> MemAlloc Failed\n" );
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	// 例外発生
	catch( Error::Errno i ){
		PRINTD( MEM_LOG, "-> Failed\n" );
		
		Error::SetError( i );
		
		switch(i){
		case Error::NoRom:		// ファイルのオープンに失敗した場合
		case Error::RomSizeNG:	// サイズが合わない場合
		case Error::RomCrcNG:	// CRCが合わない場合
		default:				// メモリを開放
			if( *buf) delete [] *buf;
			*buf = NULL;
		}
		return false;
	}
	
	return false;
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool MEM6::InitCommon( bool chkcrc, bool usesol )
{
	PRINTD( MEM_LOG, "[MEM][InitCommon]\n" );
	
	EnableChkCRC = chkcrc;
	UseSol       = usesol;	// 戦士のカートリッジ有効
	
	// 戦士のカートリッジを使う場合は必ず外部RAM有効
	if( UseSol ) UseExtRam = true;
	
	// メモリ確保
	if( !AllocMemory( &EmptyRom, &IEMPTROM, NULL ) ) return false;
	if( !AllocMemory( &EmptyRam, &IEMPTRAM, NULL ) ) return false;
	
	if( !AllocMemory( &ExtRom,   &IEXTROM,  NULL ) ) return false;
	if( !AllocMemory( &ExtRam,   &IEXTRAM,  NULL ) ) return false;
	
	
	// メモリブロック設定
	// とりあえず全てEmptyに設定
	for( int i=0; i<MAXRMB; i++ ){
		RomB[i].SetMemory( "EmptyRom", EmptyRom, IEMPTROM.WaitR, IEMPTROM.WaitW );
		RomB[i].SetProtect( true );		// ライトプロテクト有効
	}
	for( int i=0; i<MAXWMB; i++ ){
		RamB[i].SetMemory( "EmptyRam", EmptyRam, IEMPTRAM.WaitR, IEMPTRAM.WaitW );
		RamB[i].SetProtect( false );	// ライトプロテクト無効
	}
	
	// EmptyRom,EmptyRam
	//   EmptyRamは(書込めないので)実質EmptyRomと同じ
	//   アクセスウェイトのみ異なる
	EMPTYROM.SetMemory( "EmptyRom", EmptyRom, IEMPTROM.WaitR, IEMPTROM.WaitW );
	EMPTYRAM.SetMemory( "EmptyRam", EmptyRom, IEMPTRAM.WaitR, IEMPTRAM.WaitW );
	EMPTYROM.SetProtect( true );		// ライトプロテクト有効
	EMPTYRAM.SetProtect( true );		// ライトプロテクト有効
	
	// 拡張ROM領域
	EXTROM0.SetMemory( "ExtRom0", ExtRom,        IEXTROM.WaitR, IEXTROM.WaitW );
	EXTROM1.SetMemory( "ExtRom1", ExtRom+0x2000, IEXTROM.WaitR, IEXTROM.WaitW );
	
	// 外部RAM
	if( UseExtRam ){
		EXTRAM0.SetMemory( "ExtRam0", ExtRam,        IEXTRAM.WaitR, IEXTRAM.WaitW );
		EXTRAM1.SetMemory( "ExtRam1", ExtRam+0x2000, IEXTRAM.WaitR, IEXTRAM.WaitW );
		EXTRAM2.SetMemory( "ExtRam2", ExtRam+0x4000, IEXTRAM.WaitR, IEXTRAM.WaitW );
		EXTRAM3.SetMemory( "ExtRam3", ExtRam+0x6000, IEXTRAM.WaitR, IEXTRAM.WaitW );
		EXTRAM4.SetMemory( "ExtRam4", ExtRam+0x8000, IEXTRAM.WaitR, IEXTRAM.WaitW );
		EXTRAM5.SetMemory( "ExtRam5", ExtRam+0xa000, IEXTRAM.WaitR, IEXTRAM.WaitW );
		EXTRAM6.SetMemory( "ExtRam6", ExtRam+0xc000, IEXTRAM.WaitR, IEXTRAM.WaitW );
		EXTRAM7.SetMemory( "ExtRam7", ExtRam+0xe000, IEXTRAM.WaitR, IEXTRAM.WaitW );
	}
	
	// 戦士のカートリッジRAM領域
	if( UseSol )	// 外部RAMの一部を利用
		SOLRAM.SetMemory( "SolRam", ExtRam+0x6000, ISOLRAM.WaitR, ISOLRAM.WaitW );
	
	PRINTD( MEM_LOG, "[MEM][Alloc] ExtRom   :%p\n", ExtRom );
	PRINTD( MEM_LOG, "[MEM][Alloc] ExtRam   :%p\n", ExtRam );
	PRINTD( MEM_LOG, "[MEM][Alloc] EmptyRom :%p\n", EmptyRom );
	PRINTD( MEM_LOG, "[MEM][Alloc] EmptyRam :%p\n", EmptyRam );
	
	return true;
}

bool MEM60::Init( char *path, bool chkcrc, bool usesol )
{
	PRINTD( MEM_LOG, "[MEM][Init]\n" );
	
	if( !InitCommon( chkcrc, usesol ) ) return false;	// 共通部分初期化
	
	// メモリ確保
	if( !AllocMemory( &MainRom, &IMAINROM, path ) ) return false;
	if( !AllocMemory( &CGRom0,  &ICGROM0,  path ) ) return false;
	if( !AllocMemory( &CGRom1,  &ICGROM1,  path ) ) return false;
	if( !AllocMemory( &IntRam,  &IINTRAM,  path ) ) return false;
	
	
	// メモリブロック設定
	// BASIC ROM
	MAINROM0.SetMemory( "MainRom0", MainRom,        IMAINROM.WaitR, IMAINROM.WaitW );
	MAINROM1.SetMemory( "MainRom1", MainRom+0x2000, IMAINROM.WaitR, IMAINROM.WaitW );
	
	// CG ROM
	CGROM1.SetMemory( "CGRom0", CGRom0, ICGROM0.WaitR, ICGROM0.WaitW );
	CGROM1.SetMemory( "CGRom1", CGRom1, ICGROM1.WaitR, ICGROM1.WaitW );
	
	// 内部RAM
	MAINRAM0.SetMemory( "IntRam0", IntRam,        IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM1.SetMemory( "IntRam1", IntRam+0x2000, IINTRAM.WaitR, IINTRAM.WaitW );
	
	// RAM初期値設定
	int addr = 0;
	for( int i=0; i < (int)IINTRAM.Size/128; i++ ){
		for( int j=0; j < 64; j++ ) IntRam[addr++] = 0;
		for( int j=0; j < 64; j++ ) IntRam[addr++] = 0xff;
	}
	
	Reset();
	
	PRINTD( MEM_LOG, "[MEM][Alloc] IntRam  :%p\n", IntRam );
	PRINTD( MEM_LOG, "[MEM][Alloc] MainRom :%p\n", MainRom );
	PRINTD( MEM_LOG, "[MEM][Alloc] CGRom1  :%p\n", CGRom1 );
	
	return true;
}

bool MEM62::Init( char *path, bool chkcrc, bool usesol )
{
	PRINTD( MEM_LOG, "[MEM][Init]\n" );
	
	if( !InitCommon( chkcrc, usesol ) ) return false;	// 共通部分初期化
	
	// メモリ確保
	if( !AllocMemory( &MainRom,  &IMAINROM, path ) ) return false;
	if( !AllocMemory( &CGRom1,   &ICGROM1,  path ) ) return false;
	if( !AllocMemory( &CGRom2,   &ICGROM2,  path ) ) return false;
	if( !AllocMemory( &KanjiRom, &IKANJI,   path ) ) return false;
	if( !AllocMemory( &VoiceRom, &IVOICE,   path ) ) return false;
	if( !AllocMemory( &IntRam,   &IINTRAM,  path ) ) return false;
	
	
	// メモリブロック設定
	// BASIC ROM
	MAINROM0.SetMemory( "MainRom0", MainRom,        IMAINROM.WaitR, IMAINROM.WaitW );
	MAINROM1.SetMemory( "MainRom1", MainRom+0x2000, IMAINROM.WaitR, IMAINROM.WaitW );
	MAINROM2.SetMemory( "MainRom2", MainRom+0x4000, IMAINROM.WaitR, IMAINROM.WaitW );
	MAINROM3.SetMemory( "MainRom3", MainRom+0x6000, IMAINROM.WaitR, IMAINROM.WaitW );
	
	// CG ROM
	CGROM1.SetMemory( "CGRom1", CGRom1, ICGROM1.WaitR, ICGROM1.WaitW );
	CGROM2.SetMemory( "CGRom2", CGRom2, ICGROM2.WaitR, ICGROM2.WaitW );
	
	// 漢字ROM
	KANJIROM0.SetMemory( "KanjRom0", KanjiRom,        IKANJI.WaitR, IKANJI.WaitW );
	KANJIROM1.SetMemory( "KanjRom1", KanjiRom+0x2000, IKANJI.WaitR, IKANJI.WaitW );
	KANJIROM2.SetMemory( "KanjRom2", KanjiRom+0x4000, IKANJI.WaitR, IKANJI.WaitW );
	KANJIROM3.SetMemory( "KanjRom3", KanjiRom+0x6000, IKANJI.WaitR, IKANJI.WaitW );
	
	// 音声合成ROM
	VOICEROM0.SetMemory( "VoicRom0", VoiceRom,        IVOICE.WaitR, IVOICE.WaitW );
	VOICEROM1.SetMemory( "VoicRom1", VoiceRom+0x2000, IVOICE.WaitR, IVOICE.WaitW );
	
	// 内部RAM
	MAINRAM0.SetMemory( "IntRam0", IntRam,        IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM1.SetMemory( "IntRam1", IntRam+0x2000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM2.SetMemory( "IntRam2", IntRam+0x4000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM3.SetMemory( "IntRam3", IntRam+0x6000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM4.SetMemory( "IntRam4", IntRam+0x8000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM5.SetMemory( "IntRam5", IntRam+0xa000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM6.SetMemory( "IntRam6", IntRam+0xc000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM7.SetMemory( "IntRam7", IntRam+0xe000, IINTRAM.WaitR, IINTRAM.WaitW );
	
	// RAM初期値設定
	int addr = 0;
	for( int i=0; i < (int)IINTRAM.Size/256; i++ ){
		for( int j=0; j < 64; j++ ){
			IntRam[addr++] = 0;
			IntRam[addr++] = 0xff;
		}
		for( int j=0; j < 64; j++ ){
			IntRam[addr++] = 0xff;
			IntRam[addr++] = 0;
		}
	}
	
	Reset();
	
	PRINTD( MEM_LOG, "[MEM][Alloc] IntRam   :%p\n", IntRam );
	PRINTD( MEM_LOG, "[MEM][Alloc] MainRom  :%p\n", MainRom );
	PRINTD( MEM_LOG, "[MEM][Alloc] CGRom1   :%p\n", CGRom1 );
	PRINTD( MEM_LOG, "[MEM][Alloc] CGRom2   :%p\n", CGRom2 );
	PRINTD( MEM_LOG, "[MEM][Alloc] KanjiRom :%p\n", KanjiRom );
	PRINTD( MEM_LOG, "[MEM][Alloc] VoiceRom :%p\n", VoiceRom );
	
	return true;
}

bool MEM66::Init( char *path, bool chkcrc, bool usesol )
{
	PRINTD( MEM_LOG, "[MEM][Init]\n" );
	
	if( !InitCommon( chkcrc, usesol ) ) return false;	// 共通部分初期化
	
	// メモリ確保
	if( !AllocMemory( &MainRom,  &IMAINROM, path ) ) return false;
	if( !AllocMemory( &CGRom1,   &ICGROM1,  path ) ) return false;
	if( !AllocMemory( &CGRom2,   &ICGROM2,  path ) ) return false;
	if( !AllocMemory( &KanjiRom, &IKANJI,   path ) ) return false;
	if( !AllocMemory( &VoiceRom, &IVOICE,   path ) ) return false;
	if( !AllocMemory( &IntRam,  &IINTRAM,  path ) ) return false;
	
	
	// メモリブロック設定
	// BASIC ROM
	MAINROM0.SetMemory( "MainRom0", MainRom,        IMAINROM.WaitR, IMAINROM.WaitW );
	MAINROM1.SetMemory( "MainRom1", MainRom+0x2000, IMAINROM.WaitR, IMAINROM.WaitW );
	MAINROM2.SetMemory( "MainRom2", MainRom+0x4000, IMAINROM.WaitR, IMAINROM.WaitW );
	MAINROM3.SetMemory( "MainRom3", MainRom+0x6000, IMAINROM.WaitR, IMAINROM.WaitW );
	
	// CG ROM
	CGROM1.SetMemory( "CGRom1", CGRom1, ICGROM1.WaitR, ICGROM1.WaitW );
	CGROM2.SetMemory( "CGRom2", CGRom2, ICGROM2.WaitR, ICGROM2.WaitW );
	
	// 漢字ROM
	KANJIROM0.SetMemory( "KanjRom0", KanjiRom,        IKANJI.WaitR, IKANJI.WaitW );
	KANJIROM1.SetMemory( "KanjRom1", KanjiRom+0x2000, IKANJI.WaitR, IKANJI.WaitW );
	KANJIROM2.SetMemory( "KanjRom2", KanjiRom+0x4000, IKANJI.WaitR, IKANJI.WaitW );
	KANJIROM3.SetMemory( "KanjRom3", KanjiRom+0x6000, IKANJI.WaitR, IKANJI.WaitW );
	
	// 音声合成ROM
	VOICEROM0.SetMemory( "VoicRom0", VoiceRom,        IVOICE.WaitR, IVOICE.WaitW );
	VOICEROM1.SetMemory( "VoicRom1", VoiceRom+0x2000, IVOICE.WaitR, IVOICE.WaitW );
	
	// 内部RAM
	MAINRAM0.SetMemory( "IntRam0", IntRam,        IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM1.SetMemory( "IntRam1", IntRam+0x2000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM2.SetMemory( "IntRam2", IntRam+0x4000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM3.SetMemory( "IntRam3", IntRam+0x6000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM4.SetMemory( "IntRam4", IntRam+0x8000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM5.SetMemory( "IntRam5", IntRam+0xa000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM6.SetMemory( "IntRam6", IntRam+0xc000, IINTRAM.WaitR, IINTRAM.WaitW );
	MAINRAM7.SetMemory( "IntRam7", IntRam+0xe000, IINTRAM.WaitR, IINTRAM.WaitW );
	
	// RAM初期値設定
	int addr = 0;
	for( int i=0; i < (int)IINTRAM.Size/256; i++ ){
		for( int j=0; j < 64; j++ ){
			IntRam[addr++] = 0;
			IntRam[addr++] = 0xff;
		}
		for( int j=0; j < 64; j++ ){
			IntRam[addr++] = 0xff;
			IntRam[addr++] = 0;
		}
	}
	
	Reset();
	
	PRINTD( MEM_LOG, "[MEM][Alloc] IntRam   :%p\n", IntRam );
	PRINTD( MEM_LOG, "[MEM][Alloc] MainRom  :%p\n", MainRom );
	PRINTD( MEM_LOG, "[MEM][Alloc] CGRom1   :%p\n", CGRom1 );
	PRINTD( MEM_LOG, "[MEM][Alloc] CGRom2   :%p\n", CGRom2 );
	PRINTD( MEM_LOG, "[MEM][Alloc] KanjiRom :%p\n", KanjiRom );
	PRINTD( MEM_LOG, "[MEM][Alloc] VoiceRom :%p\n", VoiceRom );
	
	return true;
}


////////////////////////////////////////////////////////////////
// パッチ
////////////////////////////////////////////////////////////////
void MEM6::Patch(){}

void MEM60::Patch( void )
{
	// ジョイスティック入力ルーチン バグ修正(やや不完全)
	MainRom[0x1cb4] = 0xe6;	// AND A,x
	MainRom[0x1cb5] = 0xbf;
}

void MEM62::Patch( void )
{
	// ジョイスティック入力ルーチン バグ修正(やや不完全)
	MainRom[0x1cb4] = 0xe6;	// AND A,x
	MainRom[0x1cb5] = 0xbf;
}

void MEM66::Patch( void )
{
	// ジョイスティック入力ルーチン バグ修正(やや不完全)
	MainRom[0x1cb4] = 0xe6;	// AND A,x
	MainRom[0x1cb5] = 0xbf;
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void MEM6::Reset(){}

void MEM60::Reset( void )
{
	// 戦士のカートリッジ ------------------------------------------
	if( UseSol ) SetSolBank( 0 );	// メモリバンク初期化
	// -------------------------------------------------------------
	
	CGBank = false;	// CG ROM BANK 無効
	
	// CGROMの後半4KBに拡張ROMの内容をコピー
	// (バンク切り替えを8KB単位で行うため)
	// 拡張ROMが無い場合はCGROMのイメージとなるらしい
	memcpy( &CGRom1[0x1000], UseExtRom ? &ExtRom[0x3000] : &CGRom1[0x0000], 0x1000 );
	
	SetMB_60();
}

void MEM62::Reset( void )
{
	// 戦士のカートリッジ ------------------------------------------
	if( UseSol ) SetSolBank( 0 );	// メモリバンク初期化
	// -------------------------------------------------------------
	
	CGBank = false;	// CG ROM BANK 無効
	Rf0 = INIT_RF0;	// メモリコントローラ内部レジスタ初期値設定
	Rf1 = INIT_RF1;	// メモリコントローラ内部レジスタ初期値設定
	Rf2 = INIT_RF2;	// メモリコントローラ内部レジスタ初期値設定
	
	SetMB_R( Rf0, Rf1 );
	SetMB_W( Rf2 );
}


////////////////////////////////////////////////////////////////
// フェッチ(M1)
////////////////////////////////////////////////////////////////
BYTE MEM6::Fetch( WORD addr, int *m1wait )
{
	PRINTD( MEM_LOG, "[MEM][Fetch] -> %04X", addr );
	
	// M1ウェイト追加
	if( m1wait ) (*m1wait) += M1Wait;
	
	return Rm_blk[addr>>13]->Read( addr );
}


////////////////////////////////////////////////////////////////
// メモリリード
////////////////////////////////////////////////////////////////
BYTE MEM6::Read( WORD addr, int *wcnt )
{
	PRINTD( MEM_LOG, "[MEM][Read] -> %04X", addr );
	
	return Rm_blk[addr>>13]->Read( addr, wcnt );
}


////////////////////////////////////////////////////////////////
// メモリライト
////////////////////////////////////////////////////////////////
void MEM6::Write( WORD addr, BYTE data, int *wcnt )
{
	PRINTD( MEM_LOG, "[MEM][Write] %04X:%02X -> %p[%04X]'%c'", addr, data, Wm_blk[addr>>13], addr&0x1fff, data );
	
	Wm_blk[addr>>13]->Write( addr, data, wcnt );
	
	// 内部/外部RAMとも書込みの場合はひとまず内部だけ
//	if( ram_sel[addr>>14]&3 == 3 ){
//	}
}


////////////////////////////////////////////////////////////////
// メモリリード時のメモリブロック指定(62,66のみ)
////////////////////////////////////////////////////////////////
void MEM62::SetMB_R( BYTE mem1, BYTE mem2 )
{
	PRINTD( MEM_LOG, "[MEM][SetMB_R]\n" );
	
	// Port F0H
	switch( mem1 & 0x0f ){	// RF0下位 (0x0000 - 0x3fff)
		case 0x00:	Rm_blk[0] = pEMPTYROM;	Rm_blk[1] = pEMPTYROM;	break;
		case 0x01:	Rm_blk[0] = pMAINROM0;	Rm_blk[1] = pMAINROM1;	break;
		case 0x02:	Rm_blk[0] = pKNJROM0;	Rm_blk[1] = pKNJROM1;	break;
		case 0x03:	Rm_blk[0] = pEXTROM1;	Rm_blk[1] = pEXTROM1;	break;
		case 0x04:	Rm_blk[0] = pEXTROM0;	Rm_blk[1] = pEXTROM0;	break;
		case 0x05:	Rm_blk[0] = pKNJROM0;	Rm_blk[1] = pMAINROM1;	break;
		case 0x06:	Rm_blk[0] = pMAINROM0;	Rm_blk[1] = pKNJROM1;	break;
		case 0x07:	Rm_blk[0] = pEXTROM0;	Rm_blk[1] = pEXTROM1;	break;
		case 0x08:	Rm_blk[0] = pEXTROM1;	Rm_blk[1] = pEXTROM0;	break;
		case 0x09:	Rm_blk[0] = pEXTROM1;	Rm_blk[1] = pMAINROM1;	break;
		case 0x0a:	Rm_blk[0] = pMAINROM0;	Rm_blk[1] = pEXTROM1;	break;
		case 0x0b:	Rm_blk[0] = pEXTROM0;	Rm_blk[1] = pKNJROM1;	break;
		case 0x0c:	Rm_blk[0] = pKNJROM0;	Rm_blk[1] = pEXTROM0;	break;
		case 0x0d:	Rm_blk[0] = pMAINRAM0;	Rm_blk[1] = pMAINRAM1;	break;
		case 0x0e:	Rm_blk[0] = pEXTRAM0;	Rm_blk[1] = pEXTRAM1;	break;
		case 0x0f:	Rm_blk[0] = pEMPTYROM;	Rm_blk[1] = pEMPTYROM;	break;
	}
	switch( mem1 & 0xf0 ){	// RF0上位 (0x4000 - 0x7fff)
		case 0x00:	Rm_blk[2] = pEMPTYROM;	Rm_blk[3] = pEMPTYROM;	break;
		case 0x10:	Rm_blk[2] = pMAINROM2;	Rm_blk[3] = pMAINROM3;	break;
		case 0x20:	Rm_blk[2] = pKNJROM0;	Rm_blk[3] = pKNJROM1;	break;
		case 0x30:	Rm_blk[2] = pEXTROM1;	Rm_blk[3] = UseSol ? pSOLRAM : pEXTROM1;	break;	// 戦士のカートリッジ対応
		case 0x40:	Rm_blk[2] = pEXTROM0;	Rm_blk[3] = pEXTROM0;	break;
		case 0x50:	Rm_blk[2] = pKNJROM0;	Rm_blk[3] = pMAINROM3;	break;
		case 0x60:	Rm_blk[2] = pMAINROM2;	Rm_blk[3] = pKNJROM1;	break;
		case 0x70:	Rm_blk[2] = pEXTROM0;	Rm_blk[3] = UseSol ? pSOLRAM : pEXTROM1;	break;	// 戦士のカートリッジ対応
		case 0x80:	Rm_blk[2] = pEXTROM1;	Rm_blk[3] = pEXTROM0;	break;
		case 0x90:	Rm_blk[2] = pEXTROM1;	Rm_blk[3] = pMAINROM3;	break;
		case 0xa0:	Rm_blk[2] = pMAINROM2;	Rm_blk[3] = UseSol ? pSOLRAM : pEXTROM1;	break;	// 戦士のカートリッジ対応
		case 0xb0:	Rm_blk[2] = pEXTROM0;	Rm_blk[3] = pKNJROM1;	break;
		case 0xc0:	Rm_blk[2] = pKNJROM0;	Rm_blk[3] = pEXTROM0;	break;
		case 0xd0:	Rm_blk[2] = pMAINRAM2;	Rm_blk[3] = pMAINRAM3;	break;
		case 0xe0:	Rm_blk[2] = pEXTRAM2;	Rm_blk[3] = pEXTRAM3;	break;
		case 0xf0:	Rm_blk[2] = pEMPTYROM;	Rm_blk[3] = pEMPTYROM;	break;
	}
//	if( CGBank ) Rm_blk[3] = cgrom ? pCGROM1 : pCGROM2;
	
	// Port F1H
	switch( mem2 & 0x0f ){	// RF1下位 (0x8000 - 0xbfff)
		case 0x00:	Rm_blk[4] = pEMPTYROM;	Rm_blk[5] = pEMPTYROM;	break;
		case 0x01:	Rm_blk[4] = pMAINROM0;	Rm_blk[5] = pMAINROM1;	break;
		case 0x02:	Rm_blk[4] = pKNJROM0;	Rm_blk[5] = pKNJROM1;	break;
		case 0x03:	Rm_blk[4] = pEXTROM1;	Rm_blk[5] = pEXTROM1;	break;
		case 0x04:	Rm_blk[4] = pEXTROM0;	Rm_blk[5] = pEXTROM0;	break;
		case 0x05:	Rm_blk[4] = pKNJROM0;	Rm_blk[5] = pMAINROM1;	break;
		case 0x06:	Rm_blk[4] = pMAINROM0;	Rm_blk[5] = pKNJROM1;	break;
		case 0x07:	Rm_blk[4] = pEXTROM0;	Rm_blk[5] = pEXTROM1;	break;
		case 0x08:	Rm_blk[4] = pEXTROM1;	Rm_blk[5] = pEXTROM0;	break;
		case 0x09:	Rm_blk[4] = pEXTROM1;	Rm_blk[5] = pMAINROM1;	break;
		case 0x0a:	Rm_blk[4] = pMAINROM0;	Rm_blk[5] = pEXTROM1;	break;
		case 0x0b:	Rm_blk[4] = pEXTROM0;	Rm_blk[5] = pKNJROM1;	break;
		case 0x0c:	Rm_blk[4] = pKNJROM0;	Rm_blk[5] = pEXTROM0;	break;
		case 0x0d:	Rm_blk[4] = pMAINRAM4;	Rm_blk[5] = pMAINRAM5;	break;
		case 0x0e:	Rm_blk[4] = pEXTRAM4;	Rm_blk[5] = pEXTRAM5;	break;
		case 0x0f:	Rm_blk[4] = pEMPTYROM;	Rm_blk[5] = pEMPTYROM;	break;
	}
	switch( mem2 & 0xf0 ){	// RF1上位 (0xc000 - 0xffff)
		case 0x00:	Rm_blk[6] = pEMPTYROM;	Rm_blk[7] = pEMPTYROM;	break;
		case 0x10:	Rm_blk[6] = pMAINROM2;	Rm_blk[7] = pMAINROM3;	break;
		case 0x20:	Rm_blk[6] = pKNJROM0;	Rm_blk[7] = pKNJROM1;	break;
		case 0x30:	Rm_blk[6] = pEXTROM1;	Rm_blk[7] = pEXTROM1;	break;
		case 0x40:	Rm_blk[6] = pEXTROM0;	Rm_blk[7] = pEXTROM0;	break;
		case 0x50:	Rm_blk[6] = pKNJROM0;	Rm_blk[7] = pMAINROM3;	break;
		case 0x60:	Rm_blk[6] = pMAINROM2;	Rm_blk[7] = pKNJROM1;	break;
		case 0x70:	Rm_blk[6] = pEXTROM0;	Rm_blk[7] = pEXTROM1;	break;
		case 0x80:	Rm_blk[6] = pEXTROM1;	Rm_blk[7] = pEXTROM0;	break;
		case 0x90:	Rm_blk[6] = pEXTROM1;	Rm_blk[7] = pMAINROM3;	break;
		case 0xa0:	Rm_blk[6] = pMAINROM2;	Rm_blk[7] = pEXTROM1;	break;
		case 0xb0:	Rm_blk[6] = pEXTROM0;	Rm_blk[7] = pKNJROM1;	break;
		case 0xc0:	Rm_blk[6] = pKNJROM0;	Rm_blk[7] = pEXTROM0;	break;
		case 0xd0:	Rm_blk[6] = pMAINRAM6;	Rm_blk[7] = pMAINRAM7;	break;
		case 0xe0:	Rm_blk[6] = pEXTRAM6;	Rm_blk[7] = pEXTRAM7;	break;
		case 0xf0:	Rm_blk[6] = pEMPTYROM;	Rm_blk[7] = pEMPTYROM;	break;
	}
	
	if( CGBank ) Rm_blk[cgaddr] = cgrom ? pCGROM1 : pCGROM2;
	
	// 内部レジスタ保存
	Rf0 = mem1;
	Rf1 = mem2;
	
	#if (MEM_LOG)
	fprintf( stdout, "[MEM][SetMB_R] %02X %02X\n", mem1, mem2 );
	for( int i=0; i<8; i++ ) fprintf( stdout, "               %d:%8s\n", i, GetReadMemBlk(i) );
	#endif
}



////////////////////////////////////////////////////////////////
// メモリライト時のメモリブロック指定(62,66のみ)
////////////////////////////////////////////////////////////////
void MEM62::SetMB_W( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetMB_W] -> %02X\n", data );
	
	switch( ram_sel[0] = data & 3 ){
	case 0: Wm_blk[0] = pEMPTYRAM;	Wm_blk[1] = pEMPTYRAM;	break;
	case 1: Wm_blk[0] = pMAINRAM0;	Wm_blk[1] = pMAINRAM1;	break;
	case 2: Wm_blk[0] = pEXTRAM0;	Wm_blk[1] = pEXTRAM1;	break;
	case 3: Wm_blk[0] = pMAINRAM0;	Wm_blk[1] = pMAINRAM1;	break;	// とりあえず内部にしておく(本当は内部外部とも)
	}
	switch( ram_sel[1] = (data>>2) & 3 ){
	case 0: Wm_blk[2] = pEMPTYRAM;	Wm_blk[3] = pEMPTYRAM;	break;
	case 1: Wm_blk[2] = pMAINRAM2;	Wm_blk[3] = UseSol ? pSOLRAM : pMAINRAM3;	break;	// 戦士のカートリッジ対応
	case 2: Wm_blk[2] = pEXTRAM2;	Wm_blk[3] = UseSol ? pSOLRAM : pEXTRAM3;	break;	// 戦士のカートリッジ対応
	case 3: Wm_blk[2] = pMAINRAM2;	Wm_blk[3] = UseSol ? pSOLRAM : pMAINRAM3;	break;	// 戦士のカートリッジ対応
																						// とりあえず内部にしておく(本当は内部外部とも)
	}
	switch( ram_sel[2] = (data>>4) & 3 ){
	case 0: Wm_blk[4] = pEMPTYRAM;	Wm_blk[5] = pEMPTYRAM;	break;
	case 1: Wm_blk[4] = pMAINRAM4;	Wm_blk[5] = pMAINRAM5;	break;
	case 2: Wm_blk[4] = pEXTRAM4;	Wm_blk[5] = pEXTRAM5;	break;
	case 3: Wm_blk[4] = pMAINRAM4;	Wm_blk[5] = pMAINRAM5;	break;	// とりあえず内部にしておく(本当は内部外部とも)
	}
	switch( ram_sel[3] = (data>>6) & 3 ){
	case 0: Wm_blk[6] = pEMPTYRAM;	Wm_blk[7] = pEMPTYRAM;	break;
	case 1: Wm_blk[6] = pMAINRAM6;	Wm_blk[7] = pMAINRAM7;	break;
	case 2: Wm_blk[6] = pEXTRAM6;	Wm_blk[7] = pEXTRAM7;	break;
	case 3: Wm_blk[6] = pMAINRAM6;	Wm_blk[7] = pMAINRAM7;	break;	// とりあえず内部にしておく(本当は内部外部とも)
	}
	
	// 内部レジスタ保存
	Rf2 = data;
	
	#if (MEM_LOG)
	for( int i=0; i<8; i++ ) fprintf( stdout, "               %d:%8s\n", i, GetWriteMemBlk(i) );
	#endif
}


////////////////////////////////////////////////////////////////
// メモリアクセスウェイト設定(62,66のみ)
////////////////////////////////////////////////////////////////
void MEM62::SetWait( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetWait] -> M1:%d ROM:%d RAM:%d\n", (data>>7)&1, (data>>6)&1, (data>>5)&1 );
	
	// M1
	M1Wait = data&0x80 ? 1 : 0;
	
	// ROM
	int romwait = data&0x40 ? 1 : 0;
	EMPTYROM.SetWait( romwait, romwait );
	EXTROM0.SetWait( romwait, romwait );
	EXTROM1.SetWait( romwait, romwait );
	MAINROM0.SetWait( romwait, romwait );
	MAINROM1.SetWait( romwait, romwait );
	MAINROM2.SetWait( romwait, romwait );
	MAINROM3.SetWait( romwait, romwait );
	KANJIROM0.SetWait( romwait, romwait );
	KANJIROM1.SetWait( romwait, romwait );
	KANJIROM2.SetWait( romwait, romwait );
	KANJIROM3.SetWait( romwait, romwait );
	VOICEROM0.SetWait( romwait, romwait );
	VOICEROM1.SetWait( romwait, romwait );
	
	// RAM
	int ramwait = data&0x20 ? 1 : 0;
	EMPTYRAM.SetWait( ramwait, ramwait );
	MAINRAM0.SetWait( ramwait, ramwait );
	MAINRAM1.SetWait( ramwait, ramwait );
	MAINRAM2.SetWait( ramwait, ramwait );
	MAINRAM3.SetWait( ramwait, ramwait );
	MAINRAM4.SetWait( ramwait, ramwait );
	MAINRAM5.SetWait( ramwait, ramwait );
	MAINRAM6.SetWait( ramwait, ramwait );
	MAINRAM7.SetWait( ramwait, ramwait );
	EXTRAM0.SetWait( ramwait, ramwait );
	EXTRAM1.SetWait( ramwait, ramwait );
	EXTRAM2.SetWait( ramwait, ramwait );
	EXTRAM3.SetWait( ramwait, ramwait );
	EXTRAM4.SetWait( ramwait, ramwait );
	EXTRAM5.SetWait( ramwait, ramwait );
	EXTRAM6.SetWait( ramwait, ramwait );
	EXTRAM7.SetWait( ramwait, ramwait );
}


////////////////////////////////////////////////////////////////
// メモリアクセスウェイト取得(62,66のみ)
////////////////////////////////////////////////////////////////
BYTE MEM62::GetWait( void )
{
	return ( M1Wait ? 0x80 : 0 ) | ( EMPTYROM.GetWait() ? 0x40 : 0 ) | ( EMPTYRAM.GetWait() ? 0x20 : 0 );
}


////////////////////////////////////////////////////////////////
// メモリブロック設定(60のみ)
////////////////////////////////////////////////////////////////
void MEM60::SetMB_60( void )
{
	PRINTD( MEM_LOG, "[MEM][SetMB_60]\n" );
	
	// Read
	Rm_blk[0] = pMAINROM0; Rm_blk[1] = pMAINROM1;
	Rm_blk[2] = pEXTROM0;  Rm_blk[3] = UseSol ? pSOLRAM : pEXTROM1;	// 戦士のカートリッジ対応
	Rm_blk[4] = pEXTRAM0;  Rm_blk[5] = pEXTRAM1;
	Rm_blk[6] = pMAINRAM0; Rm_blk[7] = pMAINRAM1;
	
	// Write
	Wm_blk[0] = pMAINROM0; Wm_blk[1] = pMAINROM1;
	Wm_blk[2] = pEXTROM0;  Wm_blk[3] = UseSol ? pSOLRAM : pEXTROM1; // 戦士のカートリッジ対応
	Wm_blk[4] = pEXTRAM0;  Wm_blk[5] = pEXTRAM1;
	Wm_blk[6] = pMAINRAM0; Wm_blk[7] = pMAINRAM1;
	
	// ram_sel の設定はたぶん不要
}


////////////////////////////////////////////////////////////////
// CG ROM アドレス等設定(62,66のみ)
////////////////////////////////////////////////////////////////
void MEM62::SetCGrom( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetCGrom] -> %02x\n", data );
	
	// bit 7
	int	cgwait = data&0x80 ? 0 : 1;
	CGROM1.SetWait( cgwait, cgwait );
	CGROM2.SetWait( cgwait, cgwait );
	
	// bit 6
	cgenable = data&0x40 ? true : false;
	
	// bit 5,4,3
	// とりあえず無視
	
	// bit 2,1,0
	cgaddr   = data&3;
	
	SetMB_R( Rf0, Rf1 );
}


////////////////////////////////////////////////////////////////
// CG ROM 選択(62,66のみ)
////////////////////////////////////////////////////////////////
void MEM62::SelectCGrom( int mode )
{
	PRINTD( MEM_LOG, "[MEM][SelectCGrom] -> %d\n", mode );
	
	// mode 1:32*16(N60モード) 0:40*20(N60mモード)
	cgrom = mode ? true : false;
}


////////////////////////////////////////////////////////////////
// CG ROM BANK を切り替える
////////////////////////////////////////////////////////////////
void MEM6::SetCGBank( bool data )
{
	PRINTD( MEM_LOG, "[MEM][SetCGBank] -> %d\n", data );
	
	CGBank = data;
	Rm_blk[3] = CGBank ? pCGROM1 : UseSol ? (MemBlock*)pSOLRAM : pEXTROM1;
}

void MEM62::SetCGBank( bool data )
{
	PRINTD( MEM_LOG, "[MEM][SetCGBank] -> %d\n", data );
	
	CGBank = data;
	SetMB_R( Rf0, Rf1 );
}


////////////////////////////////////////////////////////////////
// 漢字ROM および 音声合成ROM 切り替え(62,66のみ)
////////////////////////////////////////////////////////////////
void MEM62::SelectKanjiRom( BYTE mode )
{
	PRINTD( MEM_LOG, "[MEM][SelectKanjiRom] -> %02X\n", mode );
	
// mode bit0 0:音声合成ROM選択 1:漢字ROM選択
//      bit1 0:漢字ROM左側     1:漢字ROM右側

	// 漢字 左？右？
	kj_LR = mode&0x02 ? true : false;
	
	// 漢字ROM？音声合成ROM？
	kj_rom = mode&0x01 ? true : false;
	
	SetMB_R( Rf0, Rf1 );
}

void MEM66::SelectKanjiRom( BYTE mode )
{
	MEM62::SelectKanjiRom( mode );
	
	// 66の場合,ちょっと違うらしい
	if( !(mode&0x01) ){
		if( Rm_blk[0] != pMAINROM0 ) Rm_blk[0] = pVOICEROM0;
		if( Rm_blk[1] != pMAINROM1 ) Rm_blk[1] = pVOICEROM1;
	}
}


////////////////////////////////////////////////////////////////
// 戦士のカートリッジ ROMバンク設定
////////////////////////////////////////////////////////////////
void MEM6::SetSolBank( BYTE data )
{
	SolBank = data & 0xf;
	EXTROM0.SetMemory( EXTROM0.GetName(), ExtRom + 0x2000 * SolBank, IEXTROM.WaitR, IEXTROM.WaitW );
	
	// RAMのライトプロテクト
	SOLRAM.SetProtect( data & 0x10 ? true : false );
}


////////////////////////////////////////////////////////////////
// 直接アクセス関数
////////////////////////////////////////////////////////////////
inline BYTE MEM6::ReadMainRom( WORD addr ){ return MainRom[addr&0x3fff]; }
inline BYTE MEM6::ReadMainRam( WORD addr ){ return IntRam[addr&0x3fff]; }
inline BYTE MEM6::ReadExtRom ( WORD addr ){ return ExtRom ? ExtRom[addr&0x3fff] : 0xff; }
inline BYTE MEM6::ReadCGrom0 ( WORD )     { return 0xff; }
inline BYTE MEM6::ReadCGrom1 ( WORD addr ){ return CGRom1[addr&0x1fff]; }
inline BYTE MEM6::ReadCGrom2 ( WORD )     { return 0xff; }

inline BYTE MEM60::ReadMainRam( WORD addr ){ return addr<0x4000 ? ExtRam[addr&0x3fff] : IntRam[addr&0x3fff]; }
inline BYTE MEM60::ReadCGrom0( WORD addr ) { return CGRom0[addr&0x3ff]; }

inline BYTE MEM62::ReadMainRom( WORD addr ) { return MainRom[addr&0x7fff]; }
inline BYTE MEM62::ReadMainRam( WORD addr ) { return IntRam[addr&0xffff]; }
inline BYTE MEM62::ReadCGrom2( WORD addr )  { return CGRom2[addr&0x1fff]; }
inline BYTE MEM62::ReadKanjiRom( WORD addr ){ return KanjiRom[addr&0x7fff]; }
inline BYTE MEM62::ReadVoiceRom( WORD addr ){ return VoiceRom[addr&0x3fff]; }


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
inline void MEM60::Out7FH( int, BYTE data ){ SetSolBank( data ); }

inline void MEM62::Out7FH( int, BYTE data ){ SetSolBank( data ); }
inline void MEM62::OutC1H( int, BYTE data ){ SelectCGrom( (data>>1)&1 ); }
inline void MEM62::OutC2H( int, BYTE data ){ SelectKanjiRom( data ); }
inline void MEM62::OutC3H( int, BYTE data ){}
inline void MEM62::OutF0H( int, BYTE data ){ SetMB_R( data, Rf1 ); }
inline void MEM62::OutF1H( int, BYTE data ){ SetMB_R( Rf0, data ); }
inline void MEM62::OutF2H( int, BYTE data ){ SetMB_W( data ); }
inline void MEM62::OutF3H( int, BYTE data ){ SetWait( data ); }
inline void MEM62::OutF8H( int, BYTE data ){ SetCGrom( data ); }
inline BYTE MEM62::InF0H( int ){ return Rf0; }
inline BYTE MEM62::InF1H( int ){ return Rf1; }
inline BYTE MEM62::InF2H( int ){ return Rf2; }
inline BYTE MEM62::InF3H( int ){ return GetWait() | 0x1f; }


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool MEM60::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "MEMORY", NULL, "CGBank",	 "%s",	CGBank ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "UseExtRam",	 "%s",	UseExtRam ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "M1Wait",	 "%d",	M1Wait );
	Ini->PutEntry( "MEMORY", NULL, "UseSoldier", "%s",	UseSol ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "SolBank",	 "%d",	SolBank );
	
	// 拡張ROMがマウントされている場合
	if( UseExtRom )
		Ini->PutEntry( "MEMORY", NULL, "FilePath",	"%s",	FilePath );
	
	// 内部RAM
	for( int i=0; i<(int)IINTRAM.Size; i+=64 ){
		char stren[16],strva[256];
		sprintf( stren, "IntRam_%04X", i );
		for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", IntRam[i+j] );
		Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
	}
	
	// 外部RAM
	if( UseExtRam ){
		for( int i=0; i<0x4000; i+=64 ){
			char stren[16],strva[256];
			sprintf( stren, "ExtRam_%04X", i );
			for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", ExtRam[i+j] );
			Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
		}
		if( UseSol ){
			for( int i=0x6000; i<0x8000; i+=64 ){
				char stren[16],strva[256];
				sprintf( stren, "ExtRam_%04X", i );
				for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", ExtRam[i+j] );
				Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
			}
		}
	}
	
	return true;
}

bool MEM62::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "MEMORY", NULL, "CGBank",	 "%s",	CGBank ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "UseExtRam",	 "%s",	UseExtRam ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "cgrom",		 "%s",	cgrom ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "kj_rom",	 "%s",	kj_rom ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "kj_LR",		 "%s",	kj_LR ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "Rf0",	 "0x%02X",	Rf0 );
	Ini->PutEntry( "MEMORY", NULL, "Rf1",	 "0x%02X",	Rf1 );
	Ini->PutEntry( "MEMORY", NULL, "Rf2",	 "0x%02X",	Rf2 );
	Ini->PutEntry( "MEMORY", NULL, "M1Wait",	 "%d",	M1Wait );
	Ini->PutEntry( "MEMORY", NULL, "UseSoldier", "%s",	UseSol ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "SolBank",	 "%d",	SolBank );
	
	// 拡張ROMがマウントされている場合
	if( UseExtRom )
		Ini->PutEntry( "MEMORY", NULL, "FilePath",	"%s",	FilePath );
	
	// 内部RAM
	for( int i=0; i<(int)IINTRAM.Size; i+=64 ){
		char stren[16],strva[256];
		sprintf( stren, "IntRam_%04X", i );
		for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", IntRam[i+j] );
		Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
	}
	
	// 外部RAM
	if( UseExtRam ){
		for( int i=0; i<(int)IEXTRAM.Size; i+=64 ){
			char stren[16],strva[256];
			sprintf( stren, "ExtRam_%04X", i );
			for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", ExtRam[i+j] );
			Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
		}
	}
	
	return true;
}



////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool MEM60::DokoLoad( cIni *Ini )
{
	char strfp[PATH_MAX];
	
	if( !Ini ) return false;
	
	Ini->GetTruth( "MEMORY", "CGBank",		&CGBank,	CGBank );
	Ini->GetTruth( "MEMORY", "UseExtRam",	&UseExtRam,	UseExtRam );
	Ini->GetInt(   "MEMORY", "M1Wait",		&M1Wait,	M1Wait );
	Ini->GetTruth( "MEMORY", "UseSoldier",	&UseSol,	UseSol );
	Ini->GetInt(   "MEMORY", "SolBank",		&SolBank,	SolBank );
	
	// 内部RAM
	for( int i=0; i<(int)IINTRAM.Size; i+=64 ){
		char stren[16],strva[256];
		sprintf( stren, "IntRam_%04X", i );
		memset( strva, '0', 64*2 );
		if( Ini->GetString( "MEMORY", stren, strva, strva ) ){
			for( int j=0; j<64; j++ ){
				char dt[5] = "0x";
				strncpy( &dt[2], &strva[j*2], 2 );
				IntRam[i+j] = strtol( dt, NULL, 16 );
			}
		}
	}
	
	// 外部RAM
	for( int i=0; i<(int)IEXTRAM.Size; i+=64 ){
		char stren[16],strva[256];
		sprintf( stren, "ExtRam_%04X", i );
		memset( strva, '0', 64*2 );
		if( Ini->GetString( "MEMORY", stren, strva, strva ) ){
			for( int j=0; j<64; j++ ){
				char dt[5] = "0x";
				strncpy( &dt[2], &strva[j*2], 2 );
				ExtRam[i+j] = strtol( dt, NULL, 16 );
			}
		}
	}
	
	// 拡張ROM
	Ini->GetString( "MEMORY", "FilePath", strfp, "" );
	if( *strfp ) MountExtRom( strfp );
	
	SetMB_60();
	SetCGBank( CGBank );
	
	// 戦士のカートリッジ ------------------------------------------
	if( UseSol ) SetSolBank( SolBank );	// メモリバンク初期化
	// -------------------------------------------------------------
	
	return true;
}

bool MEM62::DokoLoad( cIni *Ini )
{
	int st;
	char strfp[PATH_MAX];
	
	if( !Ini ) return false;
	
	Ini->GetTruth( "MEMORY", "CGBank",		&CGBank,	CGBank );
	Ini->GetTruth( "MEMORY", "UseExtRam",	&UseExtRam,	UseExtRam );
	Ini->GetTruth( "MEMORY", "cgrom",		&cgrom,		cgrom );
	Ini->GetTruth( "MEMORY", "kj_rom",		&kj_rom,	kj_rom );
	Ini->GetTruth( "MEMORY", "kj_LR",		&kj_LR,		kj_LR );
	Ini->GetInt(   "MEMORY", "Rf0",			&st,		Rf0 );	Rf0 = st;
	Ini->GetInt(   "MEMORY", "Rf1",			&st,		Rf1 );	Rf1 = st;
	Ini->GetInt(   "MEMORY", "Rf2",			&st,		Rf2 );	Rf2 = st;
	Ini->GetInt(   "MEMORY", "M1Wait",		&M1Wait,	M1Wait );
	Ini->GetTruth( "MEMORY", "UseSoldier",	&UseSol,	UseSol );
	Ini->GetInt(   "MEMORY", "SolBank",		&SolBank,	SolBank );
	
	// 内部RAM
	for( int i=0; i<(int)IINTRAM.Size; i+=64 ){
		char stren[16],strva[256];
		sprintf( stren, "IntRam_%04X", i );
		memset( strva, '0', 64*2 );
		if( Ini->GetString( "MEMORY", stren, strva, strva ) ){
			for( int j=0; j<64; j++ ){
				char dt[5] = "0x";
				strncpy( dt, &strva[j*2], 2 );
				IntRam[i+j] = strtol( dt, NULL, 16 );
			}
		}
	}
	
	// 外部RAM
	for( int i=0; i<(int)IEXTRAM.Size; i+=64 ){
		char stren[16],strva[256];
		sprintf( stren, "ExtRam_%04X", i );
		memset( strva, '0', 64*2 );
		if( Ini->GetString( "MEMORY", stren, strva, strva ) ){
			for( int j=0; j<64; j++ ){
				char dt[5] = "0x";
				strncpy( dt, &strva[j*2], 2 );
				ExtRam[i+j] = strtol( dt, NULL, 16 );
			}
		}
	}
	
	// 拡張ROM
	Ini->GetString( "MEMORY", "FilePath", strfp, "" );
	if( *strfp ) MountExtRom( strfp );
	
	SetMB_R( Rf0, Rf1 );
	SetMB_W( Rf2 );
	SetCGBank( CGBank );
	
	// 戦士のカートリッジ ------------------------------------------
	if( UseSol ) SetSolBank( SolBank );	// メモリバンク初期化
	// -------------------------------------------------------------
	
	return true;
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
////////////////////////////////////////////////////////////////
// デバッグ用関数
////////////////////////////////////////////////////////////////
char *MEM6::GetReadMemBlk( int blk )
{
	return Rm_blk[blk]->GetName();
}

char *MEM6::GetWriteMemBlk( int blk )
{
	return Wm_blk[blk]->GetName();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //



////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor MEM60::descriptor = {
	MEM60::indef, MEM60::outdef
};

const Device::OutFuncPtr MEM60::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &MEM60::Out7FH )
};

const Device::InFuncPtr MEM60::indef[] = { NULL };

const Device::Descriptor MEM62::descriptor = {
	MEM62::indef, MEM62::outdef
};

const Device::OutFuncPtr MEM62::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &MEM62::Out7FH ),
	STATIC_CAST( Device::OutFuncPtr, &MEM62::OutC1H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM62::OutC2H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM62::OutC3H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF0H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF1H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF2H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF3H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF8H )
};

const Device::InFuncPtr MEM62::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &MEM62::InF0H ),
	STATIC_CAST( Device::InFuncPtr, &MEM62::InF1H ),
	STATIC_CAST( Device::InFuncPtr, &MEM62::InF2H ),
	STATIC_CAST( Device::InFuncPtr, &MEM62::InF3H )
};
