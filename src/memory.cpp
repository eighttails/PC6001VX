#include <stdlib.h>
#include <new>

#include "log.h"
#include "memory.h"
#include "common.h"
#include "error.h"
#include "osd.h"

#include "p6vm.h"

// メモリブロック割り当て
#define EMPTYROM	(RomB[0])	// EmptyRom
#define EMPTYRAM	(RomB[1])	// EmptyRam
#define EXTROM0		(RomB[2])	// ExtRom
#define EXTROM1		(RomB[3])	// ExtRom+0x2000
#define MAINROM0	(RomB[4])	// MainRom
#define MAINROM1	(RomB[5])	// MainRom+0x2000
#define MAINROM2	(RomB[6])	// MainRom+0x4000
#define MAINROM3	(RomB[7])	// MainRom+0x6000
#define MAINROM4	(RomB[8])	// MainRom+0x8000
#define MAINROM5	(RomB[9])	// MainRom+0xa000
#define MAINROM6	(RomB[10])	// MainRom+0xc000
#define MAINROM7	(RomB[11])	// MainRom+0xe000
#define CGROM1		(RomB[12])	// CGRom1
#define CGROM2		(RomB[13])	// CGRom2			CGRom1+0x2000
#define KANJIROM0	(RomB[14])	// KanjiRom			SysRom2+0x8000
#define KANJIROM1	(RomB[15])	// KanjiRom+0x2000	SysRom2+0xa000
#define KANJIROM2	(RomB[16])	// KanjiRom+0x4000	SysRom2+0xc000
#define KANJIROM3	(RomB[17])	// KanjiRom+0x6000	SysRom2+0xe000
#define VOICEROM0	(RomB[18])	// VoiceRom			SysRom2+0x4000
#define VOICEROM1	(RomB[19])	// VoiceRom+0x2000	ysRom2+0x6000
#define SRMENROM0	(RomB[20])	//					SysRom2
#define SRMENROM1	(RomB[21])	//					SysRom2+0x2000

#define MAINRAM0	(RamB[0])	// IntRam
#define MAINRAM1	(RamB[1])	// IntRam+0x2000
#define MAINRAM2	(RamB[2])	// IntRam+0x4000
#define MAINRAM3	(RamB[3])	// IntRam+0x6000
#define MAINRAM4	(RamB[4])	// IntRam+0x8000
#define MAINRAM5	(RamB[5])	// IntRam+0xa000
#define MAINRAM6	(RamB[6])	// IntRam+0xc000
#define MAINRAM7	(RamB[7])	// IntRam+0xe000
#define EXTRAM0		(RamB[8])	// ExtRam
#define EXTRAM1		(RamB[9])	// ExtRam+0x2000
#define EXTRAM2		(RamB[10])	// ExtRam+0x4000
#define EXTRAM3		(RamB[11])	// ExtRam+0x6000
#define EXTRAM4		(RamB[12])	// ExtRam+0x8000
#define EXTRAM5		(RamB[13])	// ExtRam+0xa000
#define EXTRAM6		(RamB[14])	// ExtRam+0xc000
#define EXTRAM7		(RamB[15])	// ExtRam+0xe000
#define SOLRAM		(RamB[11])	// EXTRAM3と共用


#define pEXTROM0	(UseExtRom ? &EXTROM0 : &EMPTYROM)
#define pEXTROM1	(UseExtRom ? &EXTROM1 : &EMPTYROM)


#define pKNJROM0	(kj_rom ? ( kj_LR ? &KANJIROM2 : &KANJIROM0 ) : &VOICEROM0 )
#define pKNJROM1	(kj_rom ? ( kj_LR ? &KANJIROM3 : &KANJIROM1 ) : &VOICEROM1 )


// メモリコントローラ内部レジスタ初期値
#define	INIT_RF0	(0x71)
#define INIT_RF1	(0xdd)
#define INIT_RF2	(0x50)


////////////////////////////////////////////////////////////////
// ROM情報
////////////////////////////////////////////////////////////////
// PC-6001                                  ROMファイル名		CRC32
const MEM6::ROMINFO MEM60::AMAINROM[] = { { "BASICROM.60",		0x54c03109 } };
const MEM6::ROMINFO MEM60::ACGROM1[]  = { { "CGROM60.60",		0xb0142d32 } };

// PC-6001A                                 ROMファイル名		CRC32
const MEM6::ROMINFO MEM61::AMAINROM[] = { { "BASICROM.61",		0xfa8e88d9 } };
const MEM6::ROMINFO MEM61::ACGROM1[]  = { { "CGROM60.61",		0x49c21d08 } };
// PC-6001mk2                               ROMファイル名		CRC32
const MEM6::ROMINFO MEM62::AMAINROM[] = { { "BASICROM.62",		0x950ac401 },	// 前期
										  { "BASICROM.62",		0xd7e61957 } };	// 後期
const MEM6::ROMINFO MEM62::ACGROM1[]  = { { "CGROM60.62",		0x81eb5d95 } };
const MEM6::ROMINFO MEM62::ACGROM2[]  = { { "CGROM60m.62",		0x3ce48c33 } };
const MEM6::ROMINFO MEM62::AKANJI[]   = { { "KANJIROM.62",		0x20c8f3eb } };
const MEM6::ROMINFO MEM62::AVOICE[]   = { { "VOICEROM.62",		0x49b4f917 } };

// PC-6601                                  ROMファイル名		CRC32
const MEM6::ROMINFO MEM66::AMAINROM[] = { { "BASICROM.66",		0xc0b01772 } };
const MEM6::ROMINFO MEM66::ACGROM1[]  = { { "CGROM60.66",		0xd2434f29 } };
const MEM6::ROMINFO MEM66::ACGROM2[]  = { { "CGROM66.66",		0x3ce48c33 } };
const MEM6::ROMINFO MEM66::AKANJI[]   = { { "KANJIROM.66",		0x20c8f3eb } };
const MEM6::ROMINFO MEM66::AVOICE[]   = { { "VOICEROM.66",		0x91d078c1 } };

// PC-6001mk2SR / PC-6601SR                 ROMファイル名		CRC32
const MEM6::ROMINFO MEM64::ASYSROM1[] = { { "SYSTEMROM1.64",	0xb6fc2db2 },
										  { "SYSTEMROM1.68",	0xb6fc2db2 } };
const MEM6::ROMINFO MEM64::ASYSROM2[] = { { "SYSTEMROM2.64",	0x55a62a1d },
										  { "SYSTEMROM2.68",	0x55a62a1d } };
const MEM6::ROMINFO MEM64::ACGROM[]   = { { "CGROM68.64",		0x73bc3256 },
										  { "CGROM68.68",		0x73bc3256 } };


////////////////////////////////////////////////////////////////
// メモリ情報
////////////////////////////////////////////////////////////////
// 共通                                 ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM6::IEMPTROM  = { NULL,     0,                 0x02000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM6::IEMPTRAM  = { NULL,     0,                 0x02000, 0xff,  0, 0 };
const MEM6::MEMINFO MEM6::IEXTROM   = { NULL,     0,                 0x20000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM6::IEXTRAM   = { NULL,     0,                 0x20000, 0x00,  0, 0 };
// MEGA-ROM/RAMカートリッジ対応のため
// 拡張ROM/RAM領域は128KB確保しておく

// ----------------------------------------------------------------------------------------
// 戦士のカートリッジ                   ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM6::ISOLRAM   = { NULL,     0,                 0x02000, 0xff,  1, 1 };
// ----------------------------------------------------------------------------------------

// PC-6001                              ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM60::IMAINROM = { AMAINROM, COUNTOF(AMAINROM), 0x04000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM60::ICGROM1  = { ACGROM1,  COUNTOF(ACGROM1),  0x01000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM60::IINTRAM  = { NULL,     0,                 0x04000, 0x00,  0, 0 };

// PC-6001A                             ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM61::IMAINROM = { AMAINROM, COUNTOF(AMAINROM), 0x04000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM61::ICGROM1  = { ACGROM1,  COUNTOF(ACGROM1),  0x01000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM61::IINTRAM  = { NULL,     0,                 0x04000, 0x00,  0, 0 };
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

// PC-6001mk2SR / PC-6601SR             ROM情報   ROM情報要素数      サイズ   初期値 Wait(Read/Write)
const MEM6::MEMINFO MEM64::ISYSROM1 = { ASYSROM1, COUNTOF(ASYSROM1), 0x10000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM64::ISYSROM2 = { ASYSROM2, COUNTOF(ASYSROM2), 0x10000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM64::ICGROM   = { ACGROM,   COUNTOF(ACGROM),   0x04000, 0xff,  1, 1 };
const MEM6::MEMINFO MEM64::IINTRAM  = { NULL,     0,                 0x10000, 0x00,  0, 0 };



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
//			prt		ライトプロテクトフラグ true:セット false：解除
// 返値:	なし
////////////////////////////////////////////////////////////////
void MemBlock::SetMemory( const char *name, BYTE *mem, int rw, int ww, bool prt )
{
	strncpy( Name, name, 32 );
	MB    = mem;
	RWait = rw;
	WWait = ww;
	WPt = prt;
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
int MemBlock::GetWait( void ) const
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
bool MemBlock::GetProtect( void ) const
{
	return WPt;
}


////////////////////////////////////////////////////////////////
// メモリブロック名取得
//
// 引数:	なし
// 返値:	char *	メモリブロック名へのポインタ
////////////////////////////////////////////////////////////////
const char *MemBlock::GetName( void ) const
{
	return Name;
}


////////////////////////////////////////////////////////////////
// メモリリード
////////////////////////////////////////////////////////////////
BYTE MemBlock::Read( WORD addr, int *wcnt ) const
{
	if( wcnt ) (*wcnt) += RWait;
	return MB[addr&0x1fff];
}


////////////////////////////////////////////////////////////////
// メモリライト
////////////////////////////////////////////////////////////////
void MemBlock::Write( WORD addr, BYTE data, int *wcnt ) const
{
	if( wcnt ) (*wcnt) += WWait;
	if( !WPt ) MB[addr&0x1fff] = data;
}




////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
MEM6::MEM6( VM6 *vm, const ID& id ) : Device(vm,id),
	CGBank(false), UseExtRom(false), UseExtRam(false),
	MainRom(NULL), SysRom2(NULL), ExtRom(NULL), CGRom1(NULL), CGRom2(NULL),
	KanjiRom(NULL), VoiceRom(NULL), IntRam(NULL), ExtRam(NULL),
	EmptyRom(NULL), EmptyRam(NULL),	M1Wait(1), EnableChkCRC(true),
	
	cgrom(true), kj_rom(true), kj_LR(true), cgenable(true), cgaddr(3),
	
	UseSol(false), SolBank(0)
{
	Rf[0] = INIT_RF0;
	Rf[1] = INIT_RF1;
	Rf[2] = INIT_RF2;
	
	INITARRAY( Rm_blk, NULL );
	INITARRAY( Wm_blk, NULL );
	INITARRAY( FilePath, '\0' );
	
	INITARRAY( Rm_blkSR, NULL );
	INITARRAY( Wm_blkSR, NULL );
	INITARRAY( RfSR, 0 );
	
	MemTable.EmptRom = &MEM6::IEMPTROM;
	MemTable.EmptRam = &MEM6::IEMPTRAM;
	MemTable.ExtRom  = &MEM6::IEXTROM;
	MemTable.ExtRam  = &MEM6::IEXTRAM;
	MemTable.SolRam  = &MEM6::ISOLRAM;
}

MEM60::MEM60( VM6 *vm, const ID& id ) : MEM6(vm,id)
{
	RomTable.System1 = MEM60::AMAINROM;
	RomTable.CGRom1  = MEM60::ACGROM1;
	
	MemTable.IntRam  = &MEM60::IINTRAM;
	MemTable.System1 = &MEM60::IMAINROM;
	MemTable.CGRom1  = &MEM60::ICGROM1;
}

MEM61::MEM61( VM6 *vm, const ID& id ) : MEM60(vm,id)
{
	RomTable.System1 = MEM61::AMAINROM;
	RomTable.CGRom1  = MEM61::ACGROM1;
	
	MemTable.IntRam  = &MEM61::IINTRAM;
	MemTable.System1 = &MEM61::IMAINROM;
	MemTable.CGRom1  = &MEM61::ICGROM1;
}

MEM62::MEM62( VM6 *vm, const ID& id ) : MEM6(vm,id)
{
	RomTable.System1 = MEM62::AMAINROM;
	RomTable.CGRom1  = MEM62::ACGROM1;
	RomTable.CGRom2  = MEM62::ACGROM2;
	RomTable.Kanji   = MEM62::AKANJI;
	RomTable.Voice   = MEM62::AVOICE;
	
	MemTable.IntRam  = &MEM62::IINTRAM;
	MemTable.System1 = &MEM62::IMAINROM;
	MemTable.CGRom1  = &MEM62::ICGROM1;
	MemTable.CGRom2  = &MEM62::ICGROM2;
	MemTable.Kanji   = &MEM62::IKANJI;
	MemTable.Voice   = &MEM62::IVOICE;
}

MEM66::MEM66( VM6 *vm, const ID& id ) : MEM62(vm,id)
{
	RomTable.System1 = MEM66::AMAINROM;
	RomTable.CGRom1  = MEM66::ACGROM1;
	RomTable.CGRom2  = MEM66::ACGROM2;
	RomTable.Kanji   = MEM66::AKANJI;
	RomTable.Voice   = MEM66::AVOICE;
	
	MemTable.IntRam  = &MEM66::IINTRAM;
	MemTable.System1 = &MEM66::IMAINROM;
	MemTable.CGRom1  = &MEM66::ICGROM1;
	MemTable.CGRom2  = &MEM66::ICGROM2;
	MemTable.Kanji   = &MEM66::IKANJI;
	MemTable.Voice   = &MEM66::IVOICE;
}

MEM64::MEM64( VM6 *vm, const ID& id ) : MEM62(vm,id)
{
	RomTable.System1 = MEM64::ASYSROM1;
	RomTable.System2 = MEM64::ASYSROM2;
	RomTable.CGRom1  = MEM64::ACGROM;
	RomTable.CGRom2  = NULL;
	RomTable.Kanji   = NULL;
	RomTable.Voice   = NULL;
	
	MemTable.IntRam  = &MEM64::IINTRAM;
	MemTable.System1 = &MEM64::ISYSROM1;
	MemTable.System2 = &MEM64::ISYSROM2;
	MemTable.CGRom1  = &MEM64::ICGROM;
	MemTable.CGRom2  = NULL;
	MemTable.Kanji   = NULL;
	MemTable.Voice   = NULL;
}

MEM68::MEM68( VM6 *vm, const ID& id ) : MEM64(vm,id){}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
MEM6::~MEM6( void )
{
	if( MainRom )	delete[] MainRom;
	if( SysRom2 )	delete[] SysRom2;
	if( CGRom1 )	delete[] CGRom1;
	if( CGRom2 )	delete[] CGRom2;
	if( KanjiRom )	delete[] KanjiRom;
	if( VoiceRom )	delete[] VoiceRom;
	if( IntRam )	delete[] IntRam;
	if( ExtRom )	delete[] ExtRom;
	if( EmptyRom )	delete[] EmptyRom;
	if( EmptyRam )	delete[] EmptyRam;
}

MEM60::~MEM60( void ){}

MEM61::~MEM61( void ){}
MEM62::~MEM62( void ){}

MEM66::~MEM66( void ){}

MEM64::~MEM64( void ){}

MEM68::~MEM68( void ){}


////////////////////////////////////////////////////////////////
// 拡張ROM マウント
////////////////////////////////////////////////////////////////
bool MEM6::MountExtRom( const char *filename )
{
	PRINTD( MEM_LOG, "[MEM][MountExtRom] -> %s -> ", filename );
	
	// マウント済みなら一旦開放
	if( UseExtRom ) UnmountExtRom();
	
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
		
		PRINTD( MEM_LOG, "Error\n" );
		
		UnmountExtRom();
		return false;
	}
	
	UseExtRom = true;
	
	PRINTD( MEM_LOG, "OK %s\n", FilePath );
	
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
const char *MEM6::GetFile( void ) const
{
	return FilePath;
}


////////////////////////////////////////////////////////////////
// メモリ確保とROMファイル読込み
////////////////////////////////////////////////////////////////
bool MEM6::AllocMemory( BYTE **buf, const MEMINFO *info, const char *path )
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
				OSD_AddDelimiter( fpath );
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
// 全メモリ確保とROMファイル読込み
////////////////////////////////////////////////////////////////
bool MEM6::AllocAllMemory( const char *path, bool chkcrc )
{
	PRINTD( MEM_LOG, "[MEM][AllocAllMemory]\n" );
	
	EnableChkCRC = chkcrc;		// CRCチェック有効
	
	// 共通
	if( !AllocMemory( &EmptyRom, MemTable.EmptRom, NULL ) ) return false;
	if( !AllocMemory( &EmptyRam, MemTable.EmptRam, NULL ) ) return false;
	
	if( !AllocMemory( &ExtRom,   MemTable.ExtRom,  NULL ) ) return false;
	if( !AllocMemory( &ExtRam,   MemTable.ExtRam,  NULL ) ) return false;
	if( !AllocMemory( &IntRam,   MemTable.IntRam,  NULL ) ) return false;
	
	if( !AllocMemory( &MainRom,  MemTable.System1, path ) ) return false;
	if( !AllocMemory( &CGRom1,   MemTable.CGRom1,  path ) ) return false;
	
	// 62,66
	if( MemTable.CGRom2 )  if( !AllocMemory( &CGRom2,   MemTable.CGRom2, path ) ) return false;
	if( MemTable.Kanji )   if( !AllocMemory( &KanjiRom, MemTable.Kanji,  path ) ) return false;
	if( MemTable.Voice )   if( !AllocMemory( &VoiceRom, MemTable.Voice,  path ) ) return false;
	
	// 64,68
	if( MemTable.System2 ) if( !AllocMemory( &SysRom2,  MemTable.System2, path ) ) return false;
	
	SetRamValue();
	
	return true;
}

////////////////////////////////////////////////////////////////
// RAMの初期値を設定
////////////////////////////////////////////////////////////////
void MEM60::SetRamValue( void )
{
	BYTE *addr = IntRam;
	for( int i=0; i<(int)IINTRAM.Size/128; i++ ){
		for( int j=0; j<64; j++ ) *addr++ = 0x00;
		for( int j=0; j<64; j++ ) *addr++ = 0xff;
	}
}

void MEM62::SetRamValue( void )
{
	BYTE *addr = IntRam;
	for( int i=0; i<(int)IINTRAM.Size/256; i++ ){
		for( int j=0; j<64; j++ ){
			*addr++ = 0x00;
			*addr++ = 0xff;
		}
		for( int j=0; j<64; j++ ){
			*addr++ = 0xff;
			*addr++ = 0x00;
		}
	}
}

void MEM64::SetRamValue( void )
{
	WORD *addr = (WORD *)IntRam;
	// 0000-01FFH
	for( int i=0; i<512/4; i++ ){
		*addr++ = 0x0000;
		*addr++ = 0xffff;
	}
	// 0200-FDFFH
	for( int i=0; i<63; i++ ){
		for( int j=0; j<512/4; j++ ){
			*addr++ = 0xffff;
			*addr++ = 0x0000;
		}
		for( int j=0; j<512/4; j++ ){
			*addr++ = 0x0000;
			*addr++ = 0xffff;
		}
	}
	// FE00-FF9FH
	for( int i=0; i<416/4; i++ ){
		*addr++ = 0xffff;
		*addr++ = 0x0000;
	}
	// FE00-FF9FH
	for( int i=0; i<96/2; i++ ){
		*addr++ = 0x0000;
	}
}

void MEM68::SetRamValue( void )
{
	BYTE *addr = IntRam;
	for( int i=0; i<128; i++ ){
		memset( addr, 0,    256 );
		addr += 256;
		memset( addr, 0xff, 256 );
		addr += 256;
	}
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool MEM6::Init( bool useexram, bool usesol )
{
	PRINTD( MEM_LOG, "[MEM][Init]\n" );
	
	UseExtRam = useexram;	// 外部RAM有効
	UseSol    = usesol;		// 戦士のカートリッジ有効
	
	// メモリブロック設定
	// とりあえず全てEmptyに設定
	for( int i=0; i<MAXRMB; i++ )
		RomB[i].SetMemory( "EmpRom", EmptyRom, MemTable.EmptRom->WaitR, MemTable.EmptRom->WaitW, true );
	for( int i=0; i<MAXWMB; i++ )
		RamB[i].SetMemory( "EmpRam", EmptyRam, MemTable.EmptRam->WaitR, MemTable.EmptRam->WaitW, true );
	
	// 拡張ROM領域
	EXTROM0.SetMemory( "ExRom0", ExtRom,        MemTable.ExtRom->WaitR, MemTable.ExtRom->WaitW, true );
	EXTROM1.SetMemory( "ExRom1", ExtRom+0x2000, MemTable.ExtRom->WaitR, MemTable.ExtRom->WaitW, true );
	
	// 外部RAM設定(排他)
	if( UseSol ){			// 戦士のカートリッジ
		EXTRAM0.SetMemory( "ExRam0", ExtRam,        MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
		EXTRAM1.SetMemory( "ExRam1", ExtRam+0x2000, MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
		SOLRAM.SetMemory ( "SolRam", ExtRam+0x6000, MemTable.SolRam->WaitR, MemTable.SolRam->WaitW, false );
	}else if( UseExtRam ){	// RAMカートリッジ
		EXTRAM0.SetMemory( "ExRam0", ExtRam,        MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
		EXTRAM1.SetMemory( "ExRam1", ExtRam+0x2000, MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
		EXTRAM2.SetMemory( "ExRam2", ExtRam+0x4000, MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
		EXTRAM3.SetMemory( "ExRam3", ExtRam+0x6000, MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
		EXTRAM4.SetMemory( "ExRam4", ExtRam+0x8000, MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
		EXTRAM5.SetMemory( "ExRam5", ExtRam+0xa000, MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
		EXTRAM6.SetMemory( "ExRam6", ExtRam+0xc000, MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
		EXTRAM7.SetMemory( "ExRam7", ExtRam+0xe000, MemTable.ExtRam->WaitR, MemTable.ExtRam->WaitW, false );
	}
	
	// 機種別初期化
	if( !InitSpecific() ) return false;
	
	return true;
}

////////////////////////////////////////////////////////////////
// 機種別初期化
////////////////////////////////////////////////////////////////
bool MEM60::InitSpecific( void )
{
	PRINTD( MEM_LOG, "[MEM][InitSpecific]\n" );
	
	// メモリブロック設定
	// BASIC ROM
	MAINROM0.SetMemory( "Basic0", MainRom,        MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM1.SetMemory( "Basic1", MainRom+0x2000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	
	// CG ROM
	CGROM1.SetMemory  ( "CGRom1", CGRom1,         MemTable.CGRom1->WaitR,  MemTable.CGRom1->WaitW,  true );
	
	// 内部RAM
	MAINRAM0.SetMemory( "InRam0", IntRam,         MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM1.SetMemory( "InRam1", IntRam+0x2000,  MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	
	return true;
}

bool MEM62::InitSpecific( void )
{
	PRINTD( MEM_LOG, "[MEM][InitSpecific]\n" );
	
	// メモリブロック設定
	// BASIC ROM
	MAINROM0.SetMemory ( "Basic0", MainRom,         MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM1.SetMemory ( "Basic1", MainRom+0x2000,  MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM2.SetMemory ( "Basic2", MainRom+0x4000,  MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM3.SetMemory ( "Basic3", MainRom+0x6000,  MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	
	// CG ROM
	CGROM1.SetMemory   ( "CGRom1", CGRom1,          MemTable.CGRom1->WaitR,  MemTable.CGRom1->WaitW,  true );
	CGROM2.SetMemory   ( "CGRom2", CGRom2,          MemTable.CGRom2->WaitR,  MemTable.CGRom2->WaitW,  true );
	
	// 漢字ROM
	KANJIROM0.SetMemory( "KjRom0", KanjiRom,        MemTable.Kanji->WaitR,   MemTable.Kanji->WaitW,   true );
	KANJIROM1.SetMemory( "KjRom1", KanjiRom+0x2000, MemTable.Kanji->WaitR,   MemTable.Kanji->WaitW,   true );
	KANJIROM2.SetMemory( "KjRom2", KanjiRom+0x4000, MemTable.Kanji->WaitR,   MemTable.Kanji->WaitW,   true );
	KANJIROM3.SetMemory( "KjRom3", KanjiRom+0x6000, MemTable.Kanji->WaitR,   MemTable.Kanji->WaitW,   true );
	
	// 音声合成ROM
	VOICEROM0.SetMemory( "VoRom0", VoiceRom,        MemTable.Voice->WaitR,   MemTable.Voice->WaitW,   true );
	VOICEROM1.SetMemory( "VoRom1", VoiceRom+0x2000, MemTable.Voice->WaitR,   MemTable.Voice->WaitW,   true );
	
	// 内部RAM
	MAINRAM0.SetMemory ( "InRam0", IntRam,          MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM1.SetMemory ( "InRam1", IntRam+0x2000,   MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM2.SetMemory ( "InRam2", IntRam+0x4000,   MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM3.SetMemory ( "InRam3", IntRam+0x6000,   MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM4.SetMemory ( "InRam4", IntRam+0x8000,   MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM5.SetMemory ( "InRam5", IntRam+0xa000,   MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM6.SetMemory ( "InRam6", IntRam+0xc000,   MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM7.SetMemory ( "InRam7", IntRam+0xe000,   MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	
	return true;
}

bool MEM64::InitSpecific( void )
{
	PRINTD( MEM_LOG, "[MEM][InitSpecific]\n" );
	
	// メモリブロック設定
	// N66-BASIC ROM
	MAINROM0.SetMemory ( "SYS1-0", MainRom,        MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM1.SetMemory ( "SYS1-1", MainRom+0x2000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM2.SetMemory ( "SYS1-2", MainRom+0x4000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM3.SetMemory ( "SYS1-3", MainRom+0x6000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	
	// N66SR-BASIC ROM
	MAINROM4.SetMemory ( "SYS1-4", MainRom+0x8000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM5.SetMemory ( "SYS1-5", MainRom+0xa000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM6.SetMemory ( "SYS1-6", MainRom+0xc000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	MAINROM7.SetMemory ( "SYS1-7", MainRom+0xe000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	
	// SR メニューROM
	SRMENROM0.SetMemory( "SYS2-0", SysRom2,        MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	SRMENROM1.SetMemory( "SYS2-1", SysRom2+0x2000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	
	// 音声合成ROM
	VOICEROM0.SetMemory( "SYS2-2", SysRom2+0x4000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	VOICEROM1.SetMemory( "SYS2-3", SysRom2+0x6000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	
	// 漢字ROM
	KANJIROM0.SetMemory( "SYS2-4", SysRom2+0x8000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	KANJIROM1.SetMemory( "SYS2-5", SysRom2+0xa000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	KANJIROM2.SetMemory( "SYS2-6", SysRom2+0xc000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	KANJIROM3.SetMemory( "SYS2-7", SysRom2+0xe000, MemTable.System1->WaitR, MemTable.System1->WaitW, true );
	
	// CG ROM
	CGROM1.SetMemory   ( "CGRom1", CGRom1,         MemTable.CGRom1->WaitR,  MemTable.CGRom1->WaitW,  true );
	CGROM2.SetMemory   ( "CGRom2", CGRom1+0x2000,  MemTable.CGRom1->WaitR,  MemTable.CGRom1->WaitW,  true );
	
	// 内部RAM
	MAINRAM0.SetMemory ( "InRam0", IntRam,         MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM1.SetMemory ( "InRam1", IntRam+0x2000,  MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM2.SetMemory ( "InRam2", IntRam+0x4000,  MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM3.SetMemory ( "InRam3", IntRam+0x6000,  MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM4.SetMemory ( "InRam4", IntRam+0x8000,  MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM5.SetMemory ( "InRam5", IntRam+0xa000,  MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM6.SetMemory ( "InRam6", IntRam+0xc000,  MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	MAINRAM7.SetMemory ( "InRam7", IntRam+0xe000,  MemTable.IntRam->WaitR,  MemTable.IntRam->WaitW,  false );
	
	return true;
}


////////////////////////////////////////////////////////////////
// パッチ
////////////////////////////////////////////////////////////////
void MEM6::Patch( void )
{
	// 60,61,62,66 共通
	
	// ジョイスティック入力ルーチン バグ修正(やや不完全)
	MainRom[0x1cb4] = 0xe6;	// AND A,x
	MainRom[0x1cb5] = 0xbf;
}

void MEM64::Patch( void )
{
	// 64,68 共通
	
	// TO DO
}



////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void MEM6::Reset(){}

void MEM60::Reset( void )
{
	PRINTD( MEM_LOG, "[MEM][Reset]\n" );
	
	CGBank = false;	// CG ROM BANK 無効
	
	// CGROMの後半4KBに拡張ROMの内容をコピー
	// (バンク切り替えを8KB単位で行うため)
	// 拡張ROMが無い場合はCGROMのイメージとなるらしい
	memcpy( &CGRom1[0x1000], UseExtRom ? &ExtRom[0x3000] : &CGRom1[0x0000], 0x1000 );
	
	SetMemBlockR( 0, 0 );
	SetMemBlockW( 0 );
	
	
	// 戦士のカートリッジ ------------------------------------------
	if( UseSol ) SetSolBank( 0 );	// メモリバンク初期化
	// -------------------------------------------------------------
}

void MEM62::Reset( void )
{
	PRINTD( MEM_LOG, "[MEM][Reset]\n" );
	
	CGBank = false;	// CG ROM BANK 無効
	Rf[0] = INIT_RF0;	// メモリコントローラ内部レジスタ初期値設定
	Rf[1] = INIT_RF1;	// メモリコントローラ内部レジスタ初期値設定
	Rf[2] = INIT_RF2;	// メモリコントローラ内部レジスタ初期値設定
	
	SetMemBlockR( Rf[0], Rf[1] );
	SetMemBlockW( Rf[2] );
	
	// 戦士のカートリッジ ------------------------------------------
	if( UseSol ) SetSolBank( 0 );	// メモリバンク初期化
	// -------------------------------------------------------------
}

void MEM64::Reset( void )
{
	PRINTD( MEM_LOG, "[MEM][Reset]\n" );
	
	CGBank = false;	// CG ROM BANK 無効
	Rf[0] = INIT_RF0;	// メモリコントローラ内部レジスタ初期値設定
	Rf[1] = INIT_RF1;	// メモリコントローラ内部レジスタ初期値設定
	Rf[2] = INIT_RF2;	// メモリコントローラ内部レジスタ初期値設定
	
	SetMemBlockR( Rf[0], Rf[1] );
	SetMemBlockW( Rf[2] );
	
	// TO DO
	const BYTE initmb[] = { 0xf8, 0xfa, 0xfc, 0xfe, 0x08, 0x0a, 0x0c, 0x0e,
							0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e };
	for( int i=0; i<16; i++ )
		SetMemBlockSR( i, initmb[i] );
	
	// 戦士のカートリッジ ------------------------------------------
	if( UseSol ) SetSolBank( 0 );	// メモリバンク初期化
	// -------------------------------------------------------------
}


////////////////////////////////////////////////////////////////
// フェッチ(M1)
////////////////////////////////////////////////////////////////
BYTE MEM6::Fetch( WORD addr, int *m1wait ) const
{
	BYTE data = vm->VdgIsSRmode() ? Rm_blkSR[addr>>13]->Read( addr )
												   : Rm_blk  [addr>>13]->Read( addr );
	
	PRINTD( MEM_LOG, "[MEM][Fetch] -> %04X:%02X\n", addr, data );
	
	// M1ウェイト追加
	if( m1wait ) (*m1wait) += M1Wait;
	
	// バスリクエスト区間実行時ウェイト追加
	if( vm->VdgIsBusReqExec() ) (*m1wait)++;
	
	return data;
}


////////////////////////////////////////////////////////////////
// メモリリード
////////////////////////////////////////////////////////////////
BYTE MEM6::Read( WORD addr, int *wcnt ) const
{
	BYTE data = 0xff;

	if( vm->VdgIsSRmode() ){
		if( vm->VdgIsSRGVramAccess( addr, true ) ){
			WORD ad = vm->VdgSRGVramAddr( addr );
			data = addr&1 ? (IntRam[ad]>>4)&0x0f : IntRam[ad]&0x0f;
		}else
			data = Rm_blkSR[addr>>13]->Read( addr, wcnt );
	}else
		data = Rm_blk[addr>>13]->Read( addr, wcnt );

	// バスリクエスト区間実行時ウェイト追加
	if( wcnt && vm->VdgIsBusReqExec() ) (*wcnt)++;

	PRINTD( MEM_LOG, "[MEM][Read]  -> %04X:%02X\n", addr, data );
	
	return data;
}


////////////////////////////////////////////////////////////////
// メモリライト
////////////////////////////////////////////////////////////////
void MEM6::Write( WORD addr, BYTE data, int *wcnt ) const
{
	PRINTD( MEM_LOG, "[MEM][Write] %04X:%02X -> %s[%04X]'%c'\n", addr, data, vm->VdgIsSRmode() ? Wm_blkSR[addr>>13]->GetName() : Wm_blk[addr>>13]->GetName(), addr&0x1fff, data );
	
	if( vm->VdgIsSRmode() ){
		if( vm->VdgIsSRGVramAccess( addr, false ) ){
			WORD ad = vm->VdgSRGVramAddr( addr );
			IntRam[ad] = addr&1 ? ((IntRam[ad]&0x0f)|((data<<4)&0xf0)) :
								  ((IntRam[ad]&0xf0)|( data    &0x0f));
		}else
			Wm_blkSR[addr>>13]->Write( addr, data, wcnt );
	}else
		Wm_blk[addr>>13]->Write( addr, data, wcnt );
	
	// バスリクエスト区間実行時ウェイト追加
	if( wcnt && vm->VdgIsBusReqExec() ) (*wcnt)++;
	
	// 内部/外部RAMとも書込みの場合はひとまず内部だけ
}


////////////////////////////////////////////////////////////////
// メモリアクセスウェイト設定
////////////////////////////////////////////////////////////////
void MEM6::SetWait( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetWait] -> M1:%d ROM:%d RAM:%d\n", (data>>7)&1, (data>>6)&1, (data>>5)&1 );
	
	// M1
	M1Wait = data&0x80 ? 1 : 0;
	
	// ROM
	int romwait = data&0x40 ? 1 : 0;
	EMPTYROM.SetWait ( romwait, romwait );
	EXTROM0.SetWait  ( romwait, romwait );
	EXTROM1.SetWait  ( romwait, romwait );
	MAINROM0.SetWait ( romwait, romwait );
	MAINROM1.SetWait ( romwait, romwait );
	MAINROM2.SetWait ( romwait, romwait );
	MAINROM3.SetWait ( romwait, romwait );
	MAINROM4.SetWait ( romwait, romwait );	// SR
	MAINROM5.SetWait ( romwait, romwait );	// SR
	MAINROM6.SetWait ( romwait, romwait );	// SR
	MAINROM7.SetWait ( romwait, romwait );	// SR
	KANJIROM0.SetWait( romwait, romwait );
	KANJIROM1.SetWait( romwait, romwait );
	KANJIROM2.SetWait( romwait, romwait );
	KANJIROM3.SetWait( romwait, romwait );
	VOICEROM0.SetWait( romwait, romwait );
	VOICEROM1.SetWait( romwait, romwait );
	SRMENROM0.SetWait( romwait, romwait );	// SR
	SRMENROM1.SetWait( romwait, romwait );	// SR
	
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
	EXTRAM0.SetWait ( ramwait, ramwait );
	EXTRAM1.SetWait ( ramwait, ramwait );
	EXTRAM2.SetWait ( ramwait, ramwait );
	EXTRAM3.SetWait ( ramwait, ramwait );
	EXTRAM4.SetWait ( ramwait, ramwait );
	EXTRAM5.SetWait ( ramwait, ramwait );
	EXTRAM6.SetWait ( ramwait, ramwait );
	EXTRAM7.SetWait ( ramwait, ramwait );
}


////////////////////////////////////////////////////////////////
// メモリアクセスウェイト取得
////////////////////////////////////////////////////////////////
BYTE MEM6::GetWait( void ) const
{
	return ( M1Wait ? 0x80 : 0 ) | ( EMPTYROM.GetWait() ? 0x40 : 0 ) | ( EMPTYRAM.GetWait() ? 0x20 : 0 );
}


////////////////////////////////////////////////////////////////
// メモリリード時のメモリブロック指定
////////////////////////////////////////////////////////////////
void MEM60::SetMemBlockR( BYTE mem1, BYTE mem2 )
{
	// 引数はダミー
	PRINTD( MEM_LOG, "[MEM][SetMemBlockR]\n" );
	
	// Read
	Rm_blk[0] = &MAINROM0; Rm_blk[1] = &MAINROM1;
	Rm_blk[2] = pEXTROM0;  Rm_blk[3] = UseSol ? &SOLRAM : pEXTROM1;	// 戦士のカートリッジ対応
	Rm_blk[4] = &EXTRAM0;  Rm_blk[5] = &EXTRAM1;
	Rm_blk[6] = &MAINRAM0; Rm_blk[7] = &MAINRAM1;
	
#if (MEM_LOG)
	for( int i=0; i<8; i+=2 )
		fprintf( stdout, "\t%d:%8s\t%d:%8s\n", i, Rm_blk[i]->GetName(), i+1, Rm_blk[i+1]->GetName() );
#endif
}

void MEM62::SetMemBlockR( BYTE mem1, BYTE mem2 )
{
	PRINTD( MEM_LOG, "[MEM][SetMemBlockR] -> %02X %02X\n", mem1, mem2 );
	
	// Port F0H
	switch( mem1 & 0x0f ){	// RF0下位 (0000 - 3FFF)
	case 0x00:	Rm_blk[0] = &EMPTYROM;	Rm_blk[1] = &EMPTYROM;	break;
	case 0x01:	Rm_blk[0] = &MAINROM0;	Rm_blk[1] = &MAINROM1;	break;
	case 0x02:	Rm_blk[0] = pKNJROM0;	Rm_blk[1] = pKNJROM1;	break;
	case 0x03:	Rm_blk[0] = pEXTROM1;	Rm_blk[1] = pEXTROM1;	break;
	case 0x04:	Rm_blk[0] = pEXTROM0;	Rm_blk[1] = pEXTROM0;	break;
	case 0x05:	Rm_blk[0] = pKNJROM0;	Rm_blk[1] = &MAINROM1;	break;
	case 0x06:	Rm_blk[0] = &MAINROM0;	Rm_blk[1] = pKNJROM1;	break;
	case 0x07:	Rm_blk[0] = pEXTROM0;	Rm_blk[1] = pEXTROM1;	break;
	case 0x08:	Rm_blk[0] = pEXTROM1;	Rm_blk[1] = pEXTROM0;	break;
	case 0x09:	Rm_blk[0] = pEXTROM1;	Rm_blk[1] = &MAINROM1;	break;
	case 0x0a:	Rm_blk[0] = &MAINROM0;	Rm_blk[1] = pEXTROM1;	break;
	case 0x0b:	Rm_blk[0] = pEXTROM0;	Rm_blk[1] = pKNJROM1;	break;
	case 0x0c:	Rm_blk[0] = pKNJROM0;	Rm_blk[1] = pEXTROM0;	break;
	case 0x0d:	Rm_blk[0] = &MAINRAM0;	Rm_blk[1] = &MAINRAM1;	break;
	case 0x0e:	Rm_blk[0] = &EXTRAM0;	Rm_blk[1] = &EXTRAM1;	break;
	case 0x0f:	Rm_blk[0] = &EMPTYROM;	Rm_blk[1] = &EMPTYROM;	break;
	}
	switch( mem1 & 0xf0 ){	// RF0上位 (4000 - 7FFF)
	case 0x00:	Rm_blk[2] = &EMPTYROM;	Rm_blk[3] = &EMPTYROM;	break;
	case 0x10:	Rm_blk[2] = &MAINROM2;	Rm_blk[3] = &MAINROM3;	break;
	case 0x20:	Rm_blk[2] = pKNJROM0;	Rm_blk[3] = pKNJROM1;	break;
	case 0x30:	Rm_blk[2] = pEXTROM1;	Rm_blk[3] = UseSol ? &SOLRAM : pEXTROM1;	break;	// 戦士のカートリッジ対応
	case 0x40:	Rm_blk[2] = pEXTROM0;	Rm_blk[3] = pEXTROM0;	break;
	case 0x50:	Rm_blk[2] = pKNJROM0;	Rm_blk[3] = &MAINROM3;	break;
	case 0x60:	Rm_blk[2] = &MAINROM2;	Rm_blk[3] = pKNJROM1;	break;
	case 0x70:	Rm_blk[2] = pEXTROM0;	Rm_blk[3] = UseSol ? &SOLRAM : pEXTROM1;	break;	// 戦士のカートリッジ対応
	case 0x80:	Rm_blk[2] = pEXTROM1;	Rm_blk[3] = pEXTROM0;	break;
	case 0x90:	Rm_blk[2] = pEXTROM1;	Rm_blk[3] = &MAINROM3;	break;
	case 0xa0:	Rm_blk[2] = &MAINROM2;	Rm_blk[3] = UseSol ? &SOLRAM : pEXTROM1;	break;	// 戦士のカートリッジ対応
	case 0xb0:	Rm_blk[2] = pEXTROM0;	Rm_blk[3] = pKNJROM1;	break;
	case 0xc0:	Rm_blk[2] = pKNJROM0;	Rm_blk[3] = pEXTROM0;	break;
	case 0xd0:	Rm_blk[2] = &MAINRAM2;	Rm_blk[3] = &MAINRAM3;	break;
	case 0xe0:	Rm_blk[2] = &EXTRAM2;	Rm_blk[3] = &EXTRAM3;	break;
	case 0xf0:	Rm_blk[2] = &EMPTYROM;	Rm_blk[3] = &EMPTYROM;	break;
	}
	
	// Port F1H
	switch( mem2 & 0x0f ){	// RF1下位 (8000 - BFFF)
	case 0x00:	Rm_blk[4] = &EMPTYROM;	Rm_blk[5] = &EMPTYROM;	break;
	case 0x01:	Rm_blk[4] = &MAINROM0;	Rm_blk[5] = &MAINROM1;	break;
	case 0x02:	Rm_blk[4] = pKNJROM0;	Rm_blk[5] = pKNJROM1;	break;
	case 0x03:	Rm_blk[4] = pEXTROM1;	Rm_blk[5] = pEXTROM1;	break;
	case 0x04:	Rm_blk[4] = pEXTROM0;	Rm_blk[5] = pEXTROM0;	break;
	case 0x05:	Rm_blk[4] = pKNJROM0;	Rm_blk[5] = &MAINROM1;	break;
	case 0x06:	Rm_blk[4] = &MAINROM0;	Rm_blk[5] = pKNJROM1;	break;
	case 0x07:	Rm_blk[4] = pEXTROM0;	Rm_blk[5] = pEXTROM1;	break;
	case 0x08:	Rm_blk[4] = pEXTROM1;	Rm_blk[5] = pEXTROM0;	break;
	case 0x09:	Rm_blk[4] = pEXTROM1;	Rm_blk[5] = &MAINROM1;	break;
	case 0x0a:	Rm_blk[4] = &MAINROM0;	Rm_blk[5] = pEXTROM1;	break;
	case 0x0b:	Rm_blk[4] = pEXTROM0;	Rm_blk[5] = pKNJROM1;	break;
	case 0x0c:	Rm_blk[4] = pKNJROM0;	Rm_blk[5] = pEXTROM0;	break;
	case 0x0d:	Rm_blk[4] = &MAINRAM4;	Rm_blk[5] = &MAINRAM5;	break;
	case 0x0e:	Rm_blk[4] = &EXTRAM4;	Rm_blk[5] = &EXTRAM5;	break;
	case 0x0f:	Rm_blk[4] = &EMPTYROM;	Rm_blk[5] = &EMPTYROM;	break;
	}
	switch( mem2 & 0xf0 ){	// RF1上位 (C000 - FFFF)
	case 0x00:	Rm_blk[6] = &EMPTYROM;	Rm_blk[7] = &EMPTYROM;	break;
	case 0x10:	Rm_blk[6] = &MAINROM2;	Rm_blk[7] = &MAINROM3;	break;
	case 0x20:	Rm_blk[6] = pKNJROM0;	Rm_blk[7] = pKNJROM1;	break;
	case 0x30:	Rm_blk[6] = pEXTROM1;	Rm_blk[7] = pEXTROM1;	break;
	case 0x40:	Rm_blk[6] = pEXTROM0;	Rm_blk[7] = pEXTROM0;	break;
	case 0x50:	Rm_blk[6] = pKNJROM0;	Rm_blk[7] = &MAINROM3;	break;
	case 0x60:	Rm_blk[6] = &MAINROM2;	Rm_blk[7] = pKNJROM1;	break;
	case 0x70:	Rm_blk[6] = pEXTROM0;	Rm_blk[7] = pEXTROM1;	break;
	case 0x80:	Rm_blk[6] = pEXTROM1;	Rm_blk[7] = pEXTROM0;	break;
	case 0x90:	Rm_blk[6] = pEXTROM1;	Rm_blk[7] = &MAINROM3;	break;
	case 0xa0:	Rm_blk[6] = &MAINROM2;	Rm_blk[7] = pEXTROM1;	break;
	case 0xb0:	Rm_blk[6] = pEXTROM0;	Rm_blk[7] = pKNJROM1;	break;
	case 0xc0:	Rm_blk[6] = pKNJROM0;	Rm_blk[7] = pEXTROM0;	break;
	case 0xd0:	Rm_blk[6] = &MAINRAM6;	Rm_blk[7] = &MAINRAM7;	break;
	case 0xe0:	Rm_blk[6] = &EXTRAM6;	Rm_blk[7] = &EXTRAM7;	break;
	case 0xf0:	Rm_blk[6] = &EMPTYROM;	Rm_blk[7] = &EMPTYROM;	break;
	}
	
	if( CGBank ) Rm_blk[cgaddr] = cgrom ? &CGROM1 : &CGROM2;
	
	// 内部レジスタ保存
	Rf[0] = mem1;
	Rf[1] = mem2;
	
#if (MEM_LOG)
	for( int i=0; i<8; i+=2 )
		fprintf( stdout, "\t%d:%8s\t%d:%8s\n", i, Rm_blk[i]->GetName(), i+1, Rm_blk[i+1]->GetName() );
#endif
}

void MEM64::SetMemBlockR( BYTE mem1, BYTE mem2 )
{
	PRINTD( MEM_LOG, "[MEM][SetMemBlockR] -> %02X %02X\n", mem1, mem2 );
	
	// Port F0H
	switch( mem1 & 0x0f ){	// RF0下位 (0000 - 3FFF)
	// SRの場合，この領域に音声合成ROMを選択するとSYSROM2の先頭16KBが割当てられる
	// mk2,66の場合は音声合成ROMが16KBしかないので
	// 4000-7FFFHは0000-3FFFHのイメージと考えればよいようだ
	case 0x00:	Rm_blk[0] = &EMPTYROM;	Rm_blk[1] = &EMPTYROM;	break;
	case 0x01:	Rm_blk[0] = &MAINROM0;	Rm_blk[1] = &MAINROM1;	break;
	case 0x02:	Rm_blk[0] = &SRMENROM0;	Rm_blk[1] = &SRMENROM1;	break;	// ココ
	case 0x03:	Rm_blk[0] = pEXTROM1;	Rm_blk[1] = pEXTROM1;	break;
	case 0x04:	Rm_blk[0] = pEXTROM0;	Rm_blk[1] = pEXTROM0;	break;
	case 0x05:	Rm_blk[0] = &SRMENROM0;	Rm_blk[1] = &MAINROM1;	break;	// ココ
	case 0x06:	Rm_blk[0] = &MAINROM0;	Rm_blk[1] = &SRMENROM1;	break;	// ココ
	case 0x07:	Rm_blk[0] = pEXTROM0;	Rm_blk[1] = pEXTROM1;	break;
	case 0x08:	Rm_blk[0] = pEXTROM1;	Rm_blk[1] = pEXTROM0;	break;
	case 0x09:	Rm_blk[0] = pEXTROM1;	Rm_blk[1] = &MAINROM1;	break;
	case 0x0a:	Rm_blk[0] = &MAINROM0;	Rm_blk[1] = pEXTROM1;	break;
	case 0x0b:	Rm_blk[0] = pEXTROM0;	Rm_blk[1] = &SRMENROM1;	break;	// ココ
	case 0x0c:	Rm_blk[0] = &SRMENROM0;	Rm_blk[1] = pEXTROM0;	break;	// ココ
	case 0x0d:	Rm_blk[0] = &MAINRAM0;	Rm_blk[1] = &MAINRAM1;	break;
	case 0x0e:	Rm_blk[0] = &EXTRAM0;	Rm_blk[1] = &EXTRAM1;	break;
	case 0x0f:	Rm_blk[0] = &EMPTYROM;	Rm_blk[1] = &EMPTYROM;	break;
	}
	switch( mem1 & 0xf0 ){	// RF0上位 (4000 - 7FFF)
	case 0x00:	Rm_blk[2] = &EMPTYROM;	Rm_blk[3] = &EMPTYROM;	break;
	case 0x10:	Rm_blk[2] = &MAINROM2;	Rm_blk[3] = &MAINROM3;	break;
	case 0x20:	Rm_blk[2] = pKNJROM0;	Rm_blk[3] = pKNJROM1;	break;
	case 0x30:	Rm_blk[2] = pEXTROM1;	Rm_blk[3] = UseSol ? &SOLRAM : pEXTROM1;	break;	// 戦士のカートリッジ対応
	case 0x40:	Rm_blk[2] = pEXTROM0;	Rm_blk[3] = pEXTROM0;	break;
	case 0x50:	Rm_blk[2] = pKNJROM0;	Rm_blk[3] = &MAINROM3;	break;
	case 0x60:	Rm_blk[2] = &MAINROM2;	Rm_blk[3] = pKNJROM1;	break;
	case 0x70:	Rm_blk[2] = pEXTROM0;	Rm_blk[3] = UseSol ? &SOLRAM : pEXTROM1;	break;	// 戦士のカートリッジ対応
	case 0x80:	Rm_blk[2] = pEXTROM1;	Rm_blk[3] = pEXTROM0;	break;
	case 0x90:	Rm_blk[2] = pEXTROM1;	Rm_blk[3] = &MAINROM3;	break;
	case 0xa0:	Rm_blk[2] = &MAINROM2;	Rm_blk[3] = UseSol ? &SOLRAM : pEXTROM1;	break;	// 戦士のカートリッジ対応
	case 0xb0:	Rm_blk[2] = pEXTROM0;	Rm_blk[3] = pKNJROM1;	break;
	case 0xc0:	Rm_blk[2] = pKNJROM0;	Rm_blk[3] = pEXTROM0;	break;
	case 0xd0:	Rm_blk[2] = &MAINRAM2;	Rm_blk[3] = &MAINRAM3;	break;
	case 0xe0:	Rm_blk[2] = &EXTRAM2;	Rm_blk[3] = &EXTRAM3;	break;
	case 0xf0:	Rm_blk[2] = &EMPTYROM;	Rm_blk[3] = &EMPTYROM;	break;
	}
	
	// Port F1H
	switch( mem2 & 0x0f ){	// RF1下位 (8000 - BFFF)
	case 0x00:	Rm_blk[4] = &EMPTYROM;	Rm_blk[5] = &EMPTYROM;	break;
	case 0x01:	Rm_blk[4] = &MAINROM0;	Rm_blk[5] = &MAINROM1;	break;
	case 0x02:	Rm_blk[4] = pKNJROM0;	Rm_blk[5] = pKNJROM1;	break;
	case 0x03:	Rm_blk[4] = pEXTROM1;	Rm_blk[5] = pEXTROM1;	break;
	case 0x04:	Rm_blk[4] = pEXTROM0;	Rm_blk[5] = pEXTROM0;	break;
	case 0x05:	Rm_blk[4] = pKNJROM0;	Rm_blk[5] = &MAINROM1;	break;
	case 0x06:	Rm_blk[4] = &MAINROM0;	Rm_blk[5] = pKNJROM1;	break;
	case 0x07:	Rm_blk[4] = pEXTROM0;	Rm_blk[5] = pEXTROM1;	break;
	case 0x08:	Rm_blk[4] = pEXTROM1;	Rm_blk[5] = pEXTROM0;	break;
	case 0x09:	Rm_blk[4] = pEXTROM1;	Rm_blk[5] = &MAINROM1;	break;
	case 0x0a:	Rm_blk[4] = &MAINROM0;	Rm_blk[5] = pEXTROM1;	break;
	case 0x0b:	Rm_blk[4] = pEXTROM0;	Rm_blk[5] = pKNJROM1;	break;
	case 0x0c:	Rm_blk[4] = pKNJROM0;	Rm_blk[5] = pEXTROM0;	break;
	case 0x0d:	Rm_blk[4] = &MAINRAM4;	Rm_blk[5] = &MAINRAM5;	break;
	case 0x0e:	Rm_blk[4] = &EXTRAM4;	Rm_blk[5] = &EXTRAM5;	break;
	case 0x0f:	Rm_blk[4] = &EMPTYROM;	Rm_blk[5] = &EMPTYROM;	break;
	}
	switch( mem2 & 0xf0 ){	// RF1上位 (C000 - FFFF)
	case 0x00:	Rm_blk[6] = &EMPTYROM;	Rm_blk[7] = &EMPTYROM;	break;
	case 0x10:	Rm_blk[6] = &MAINROM2;	Rm_blk[7] = &MAINROM3;	break;
	case 0x20:	Rm_blk[6] = pKNJROM0;	Rm_blk[7] = pKNJROM1;	break;
	case 0x30:	Rm_blk[6] = pEXTROM1;	Rm_blk[7] = pEXTROM1;	break;
	case 0x40:	Rm_blk[6] = pEXTROM0;	Rm_blk[7] = pEXTROM0;	break;
	case 0x50:	Rm_blk[6] = pKNJROM0;	Rm_blk[7] = &MAINROM3;	break;
	case 0x60:	Rm_blk[6] = &MAINROM2;	Rm_blk[7] = pKNJROM1;	break;
	case 0x70:	Rm_blk[6] = pEXTROM0;	Rm_blk[7] = pEXTROM1;	break;
	case 0x80:	Rm_blk[6] = pEXTROM1;	Rm_blk[7] = pEXTROM0;	break;
	case 0x90:	Rm_blk[6] = pEXTROM1;	Rm_blk[7] = &MAINROM3;	break;
	case 0xa0:	Rm_blk[6] = &MAINROM2;	Rm_blk[7] = pEXTROM1;	break;
	case 0xb0:	Rm_blk[6] = pEXTROM0;	Rm_blk[7] = pKNJROM1;	break;
	case 0xc0:	Rm_blk[6] = pKNJROM0;	Rm_blk[7] = pEXTROM0;	break;
	case 0xd0:	Rm_blk[6] = &MAINRAM6;	Rm_blk[7] = &MAINRAM7;	break;
	case 0xe0:	Rm_blk[6] = &EXTRAM6;	Rm_blk[7] = &EXTRAM7;	break;
	case 0xf0:	Rm_blk[6] = &EMPTYROM;	Rm_blk[7] = &EMPTYROM;	break;
	}
	
	if( CGBank ) Rm_blk[cgaddr] = cgrom ? &CGROM1 : &CGROM2;
	
	// 内部レジスタ保存
	Rf[0] = mem1;
	Rf[1] = mem2;
	
#if (MEM_LOG)
	for( int i=0; i<8; i+=2 )
		fprintf( stdout, "\t%d:%8s\t%d:%8s\n", i, Rm_blk[i]->GetName(), i+1, Rm_blk[i+1]->GetName() );
#endif
}


////////////////////////////////////////////////////////////////
// メモリライト時のメモリブロック指定
////////////////////////////////////////////////////////////////
void MEM60::SetMemBlockW( BYTE data )
{
	// 引数はダミー
	PRINTD( MEM_LOG, "[MEM][SetMemBlockW]\n" );
	
	// Write
	Wm_blk[0] = &MAINROM0; Wm_blk[1] = &MAINROM1;
	Wm_blk[2] = pEXTROM0;  Wm_blk[3] = UseSol ? &SOLRAM : pEXTROM1; // 戦士のカートリッジ対応
	Wm_blk[4] = &EXTRAM0;  Wm_blk[5] = &EXTRAM1;
	Wm_blk[6] = &MAINRAM0; Wm_blk[7] = &MAINRAM1;
	
#if (MEM_LOG)
	for( int i=0; i<8; i+=2 )
		fprintf( stdout, "\t%d:%8s\t%d:%8s\n", i, Wm_blk[i]->GetName(), i+1, Wm_blk[i+1]->GetName() );
#endif
}

void MEM62::SetMemBlockW( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetMemBlockW] -> %02X\n", data );
	
	switch( data & 3 ){
	case 0: Wm_blk[0] = &EMPTYRAM;	Wm_blk[1] = &EMPTYRAM;	break;
	case 1: Wm_blk[0] = &MAINRAM0;	Wm_blk[1] = &MAINRAM1;	break;
	case 2: Wm_blk[0] = &EXTRAM0;	Wm_blk[1] = &EXTRAM1;	break;
	case 3: Wm_blk[0] = &MAINRAM0;	Wm_blk[1] = &MAINRAM1;	break;	// とりあえず内部にしておく(本当は内部外部とも)
	}
	switch( (data>>2) & 3 ){
	case 0: Wm_blk[2] = &EMPTYRAM;	Wm_blk[3] = &EMPTYRAM;	break;
	case 1: Wm_blk[2] = &MAINRAM2;	Wm_blk[3] = UseSol ? &SOLRAM : &MAINRAM3;	break;	// 戦士のカートリッジ対応
	case 2: Wm_blk[2] = &EXTRAM2;	Wm_blk[3] = UseSol ? &SOLRAM : &EXTRAM3;	break;	// 戦士のカートリッジ対応
	case 3: Wm_blk[2] = &MAINRAM2;	Wm_blk[3] = UseSol ? &SOLRAM : &MAINRAM3;	break;	// 戦士のカートリッジ対応
		// とりあえず内部にしておく(本当は内部外部とも)
	}
	switch( (data>>4) & 3 ){
	case 0: Wm_blk[4] = &EMPTYRAM;	Wm_blk[5] = &EMPTYRAM;	break;
	case 1: Wm_blk[4] = &MAINRAM4;	Wm_blk[5] = &MAINRAM5;	break;
	case 2: Wm_blk[4] = &EXTRAM4;	Wm_blk[5] = &EXTRAM5;	break;
	case 3: Wm_blk[4] = &MAINRAM4;	Wm_blk[5] = &MAINRAM5;	break;	// とりあえず内部にしておく(本当は内部外部とも)
	}
	switch( (data>>6) & 3 ){
	case 0: Wm_blk[6] = &EMPTYRAM;	Wm_blk[7] = &EMPTYRAM;	break;
	case 1: Wm_blk[6] = &MAINRAM6;	Wm_blk[7] = &MAINRAM7;	break;
	case 2: Wm_blk[6] = &EXTRAM6;	Wm_blk[7] = &EXTRAM7;	break;
	case 3: Wm_blk[6] = &MAINRAM6;	Wm_blk[7] = &MAINRAM7;	break;	// とりあえず内部にしておく(本当は内部外部とも)
	}
	
	// 内部レジスタ保存
	Rf[2] = data;
	
#if (MEM_LOG)
	for( int i=0; i<8; i+=2 )
		fprintf( stdout, "\t%d:%8s\t%d:%8s\n", i, Wm_blk[i]->GetName(), i+1, Wm_blk[i+1]->GetName() );
#endif
}


////////////////////////////////////////////////////////////////
// メモリリード/ライト時のメモリブロック指定(64,68)
////////////////////////////////////////////////////////////////
void MEM64::SetMemBlockSR( BYTE port, BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetMemBlockSR] -> Port:%02X Data:%02X\n", port, data );
	
	BYTE cs   = data>>4;
	BYTE addr = (data>>1)&0x07;
	MemBlock **mb;
	
	RfSR[port&0x0f] = data;
	if( port&0x08 ) mb = &Wm_blkSR[port&0x07];	// 8-F : Write
	else			mb = &Rm_blkSR[port&0x07];	// 0-7 : Read
	
	switch( cs ){
	case 0x00:	// System RAM (16KB毎)
		switch( addr ){
		case 0x00: *mb = port&1 ? &MAINRAM1 : &MAINRAM0; break;
		case 0x01: *mb = port&1 ? &MAINRAM1 : &MAINRAM0; break;
		case 0x02: *mb = port&1 ? &MAINRAM3 : &MAINRAM2; break;
		case 0x03: *mb = port&1 ? &MAINRAM3 : &MAINRAM2; break;
		case 0x04: *mb = port&1 ? &MAINRAM5 : &MAINRAM4; break;
		case 0x05: *mb = port&1 ? &MAINRAM5 : &MAINRAM4; break;
		case 0x06: *mb = port&1 ? &MAINRAM7 : &MAINRAM6; break;
		case 0x07: *mb = port&1 ? &MAINRAM7 : &MAINRAM6; break;
		}
		break;
		
	case 0x02:	// Ext RAM (こっちも16KB毎とすべき?)
		switch( addr ){
		case 0x00: *mb = &EXTRAM0; break;
		case 0x01: *mb = &EXTRAM1; break;
		case 0x02: *mb = &EXTRAM2; break;
		case 0x03: *mb = &EXTRAM3; break;
		case 0x04: *mb = &EXTRAM4; break;
		case 0x05: *mb = &EXTRAM5; break;
		case 0x06: *mb = &EXTRAM6; break;
		case 0x07: *mb = &EXTRAM7; break;
		}
		break;
		
	case 0x0b:	// Ext ROM1
		*mb = pEXTROM1;
		break;
		
	case 0x0c:	// Ext ROM2
		*mb = pEXTROM0;
		break;
		
	case 0x0d:	// CGROM
		switch( addr&0x01 ){
		case 0x00: *mb = &CGROM1; break;
		case 0x01: *mb = &CGROM2; break;
		}
		break;
		
	case 0x0e:	// System Rom2
		switch( addr ){
		case 0x00: *mb = &SRMENROM0; break;
		case 0x01: *mb = &SRMENROM1; break;
		case 0x02: *mb = &VOICEROM0; break;
		case 0x03: *mb = &VOICEROM1; break;
		case 0x04: *mb = &KANJIROM0; break;
		case 0x05: *mb = &KANJIROM1; break;
		case 0x06: *mb = &KANJIROM2; break;
		case 0x07: *mb = &KANJIROM3; break;
		}
		break;
		
	case 0x0f:	// System Rom1
		switch( addr ){
		case 0x00: *mb = &MAINROM0; break;
		case 0x01: *mb = &MAINROM1; break;
		case 0x02: *mb = &MAINROM2; break;
		case 0x03: *mb = &MAINROM3; break;
		case 0x04: *mb = &MAINROM4; break;
		case 0x05: *mb = &MAINROM5; break;
		case 0x06: *mb = &MAINROM6; break;
		case 0x07: *mb = &MAINROM7; break;
		}
		break;
		
	default:
		*mb = &EMPTYROM;
	}
	
#if (MEM_LOG)
	fprintf( stdout, "              [Read]\t\t[Write]\n" );
	for( int i=0; i<8; i++ ) fprintf( stdout, "               %d:%8s\t%8s\n", i, Rm_blkSR[i]->GetName(), Wm_blkSR[i]->GetName() );
#endif
}


////////////////////////////////////////////////////////////////
// CG ROM アドレス等設定(62,66)
////////////////////////////////////////////////////////////////
void MEM6::SetCGrom( BYTE data )
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
	
	SetMemBlockR( Rf[0], Rf[1] );
}


////////////////////////////////////////////////////////////////
// CG ROM 選択(62,66)
////////////////////////////////////////////////////////////////
void MEM6::SelectCGrom( int mode )
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
	Rm_blk[3] = CGBank ? &CGROM1 : UseSol ? &SOLRAM : pEXTROM1;
}

void MEM62::SetCGBank( bool data )
{
	PRINTD( MEM_LOG, "[MEM][SetCGBank] -> %d\n", data );
	
	CGBank = data;
	SetMemBlockR( Rf[0], Rf[1] );
}


////////////////////////////////////////////////////////////////
// 漢字ROM および 音声合成ROM 切り替え(62,66)
////////////////////////////////////////////////////////////////
void MEM6::SelectKanjiRom( BYTE mode )
{
	PRINTD( MEM_LOG, "[MEM][SelectKanjiRom] -> %02X\n", mode );
	
	// mode bit0 0:音声合成ROM選択 1:漢字ROM選択
	//      bit1 0:漢字ROM左側     1:漢字ROM右側
	kj_LR  = mode&0x02 ? true : false;	// 漢字 左？右？
	kj_rom = mode&0x01 ? true : false;	// 漢字ROM？音声合成ROM？
	SetMemBlockR( Rf[0], Rf[1] );
}

/*
void MEM66::SelectKanjiRom( BYTE mode )
{
	PRINTD( MEM_LOG, "[MEM][SelectKanjiRom] -> %02X\n", mode );
	
	// mode bit0 0:音声合成ROM選択 1:漢字ROM選択
	//      bit1 0:漢字ROM左側     1:漢字ROM右側
	kj_LR  = mode&0x02 ? true : false;	// 漢字 左？右？
	kj_rom = mode&0x01 ? true : false;	// 漢字ROM？音声合成ROM？
	SetMemBlockR( Rf[0], Rf[1] );
	
	// 66の場合,ちょっと違うらしい
	if( !kj_rom ){
		if( Rm_blk[0] != &MAINROM0 ) Rm_blk[0] = &VOICEROM0;
		if( Rm_blk[1] != &MAINROM1 ) Rm_blk[1] = &VOICEROM1;
	}
}
*/


////////////////////////////////////////////////////////////////
// 戦士のカートリッジ ROMバンク設定
////////////////////////////////////////////////////////////////
void MEM6::SetSolBank( BYTE data )
{
	SolBank = data & 0xf;
	EXTROM0.SetMemory( EXTROM0.GetName(), ExtRom + 0x2000 * SolBank, MemTable.ExtRom->WaitR, MemTable.ExtRom->WaitW, true );
	
	// RAMのライトプロテクト
	SOLRAM.SetProtect( data & 0x10 ? true : false );
}


////////////////////////////////////////////////////////////////
// 直接アクセス関数
////////////////////////////////////////////////////////////////
BYTE MEM6::ReadMainRom( WORD addr ) const { return MainRom[addr&0x3fff]; }
BYTE MEM6::ReadMainRam( WORD addr ) const { return IntRam[addr&0x3fff]; }
BYTE MEM6::ReadExtRom ( WORD addr ) const { return ExtRom ? ExtRom[addr&0x3fff] : 0xff; }
	BYTE MEM6::ReadCGrom1 ( WORD addr ) const { return CGRom1[addr&0x1fff]; }
BYTE MEM6::ReadCGrom2 ( WORD )      const { return 0xff; }
BYTE MEM6::ReadCGrom3 ( WORD )      const { return 0xff; }

BYTE MEM60::ReadMainRam( WORD addr ) const { return addr<0x4000 ? ExtRam[addr&0x3fff] : IntRam[addr&0x3fff]; }

	BYTE MEM62::ReadMainRom( WORD addr )  const { return MainRom[addr&0x7fff]; }
BYTE MEM62::ReadMainRam( WORD addr )  const { return IntRam[addr&0xffff]; }
BYTE MEM62::ReadCGrom2( WORD addr )   const { return CGRom2[addr&0x1fff]; }
BYTE MEM62::ReadKanjiRom( WORD addr ) const { return KanjiRom[addr&0x7fff]; }
BYTE MEM62::ReadVoiceRom( WORD addr ) const { return VoiceRom[addr&0x3fff]; }

BYTE MEM64::ReadMainRom( WORD addr )  const { return MainRom[addr&0xffff]; }
BYTE MEM64::ReadCGrom1 ( WORD addr )  const { return CGRom1[addr&0x0fff]; }
BYTE MEM64::ReadCGrom2 ( WORD addr )  const { return CGRom1[(addr&0x1fff)+0x2000]; }
BYTE MEM64::ReadCGrom3 ( WORD addr )  const { return CGRom1[(addr&0x0fff)+0x1000]; }
BYTE MEM64::ReadKanjiRom( WORD addr ) const { return SysRom2[(addr&0x7fff)+0x8000]; }
BYTE MEM64::ReadVoiceRom( WORD addr ) const { return SysRom2[(addr&0x3fff)+0x4000]; }




////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
void MEM6::Out7FH( int, BYTE data ){ SetSolBank( data ); }
void MEM6::OutC1H( int, BYTE data ){ SelectCGrom( (data>>1)&1 ); }
void MEM6::OutC2H( int, BYTE data ){ SelectKanjiRom( data ); }
void MEM6::OutC3H( int, BYTE data ){}
void MEM6::OutF0H( int, BYTE data ){ SetMemBlockR( data, Rf[1] ); }
void MEM6::OutF1H( int, BYTE data ){ SetMemBlockR( Rf[0], data ); }
void MEM6::OutF2H( int, BYTE data ){ SetMemBlockW( data ); }
void MEM6::OutF3H( int, BYTE data ){ SetWait( data ); }
void MEM6::OutF8H( int, BYTE data ){ SetCGrom( data ); }

void MEM64::Out6xH( int port, BYTE data ){ SetMemBlockSR( port, data ); }

BYTE MEM6::InF0H( int ){ return Rf[0]; }
BYTE MEM6::InF1H( int ){ return Rf[1]; }
BYTE MEM6::InF2H( int ){ return Rf[2]; }
BYTE MEM6::InF3H( int ){ return GetWait() | 0x1f; }

BYTE MEM64::In6xH( int port ){ return RfSR[port&0x0f]; }
BYTE MEM64::InB2H( int port ){ return 0xfd; }	// bit1 0:mk2SR 1:66SR
BYTE MEM68::InB2H( int port ){ return 0xff; }	// bit1 0:mk2SR 1:66SR


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool MEM6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "MEMORY", NULL, "CGBank",		"%s",		CGBank    ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "UseExtRam",		"%s",		UseExtRam ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "M1Wait",		"%d",		M1Wait );
	Ini->PutEntry( "MEMORY", NULL, "UseSoldier",	"%s",		UseSol    ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "SolBank",		"%d",		SolBank );
	
	// 62,66,64,68
	Ini->PutEntry( "MEMORY", NULL, "cgrom",			"%s",		cgrom    ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "kj_rom",		"%s",		kj_rom   ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "kj_LR",			"%s",		kj_LR    ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "cgenable",		"%s",		cgenable ? "Yes" : "No" );
	Ini->PutEntry( "MEMORY", NULL, "cgaddr",		"%d",		cgaddr );
	Ini->PutEntry( "MEMORY", NULL, "Rf0",	 		"0x%02X",	Rf[0] );
	Ini->PutEntry( "MEMORY", NULL, "Rf1",	 		"0x%02X",	Rf[1] );
	Ini->PutEntry( "MEMORY", NULL, "Rf2",	 		"0x%02X",	Rf[2] );
	
	// 64,68
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "RfSR_%02d", i );
		Ini->PutEntry( "MEMORY", NULL, stren, "0x%02X", RfSR[i] );
	}
	
	// 拡張ROMがマウントされている場合
	if( UseExtRom ){
		char pathstr[PATH_MAX+1];
		strncpy( pathstr, FilePath, PATH_MAX );
		OSD_RelativePath( pathstr );
		Ini->PutEntry( "MEMORY", NULL, "FilePath",	"%s",	pathstr );
	}

	// メモリウェイト
	Ini->PutEntry( "MEMORY", NULL, "Wait",          "%d",		GetWait() );
	// CGRomウェイト
	Ini->PutEntry( "MEMORY", NULL, "CgRomWait",     "%d",		CGROM1.GetWait() );

	return true;
}

bool MEM60::DokoSave( cIni *Ini )
{
	PRINTD( MEM_LOG, "[MEM][DokoSave]\n" );
	
	if( !Ini || !MEM6::DokoSave( Ini ) ) return false;
	
	// 内部RAM
	for( int i=0; i<(int)IINTRAM.Size; i+=64 ){
		char stren[16],strva[256];
		sprintf( stren, "IntRam_%04X", i );
		for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", IntRam[i+j] );
		Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
	}
	
	// 外部RAM
	if( UseExtRam || UseSol ){
		for( int i=0; i<0x4000; i+=64 ){		// 16KB
			char stren[16],strva[256];
			sprintf( stren, "ExtRam_%04X", i );
			for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", ExtRam[i+j] );
			Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
		}
	}
	
	if( UseSol ){
		// 戦士のカートリッジ用外部RAMは0x6000-0x7FFFを使用
		for( int i=0x6000; i<0x8000; i+=64 ){	// 2KB
			char stren[16],strva[256];
			sprintf( stren, "ExtRam_%04X", i );
			for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", ExtRam[i+j] );
			Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
		}
	}
	
	return true;
}

bool MEM62::DokoSave( cIni *Ini )
{
	PRINTD( MEM_LOG, "[MEM][DokoSave]\n" );
	
	if( !Ini || !MEM6::DokoSave( Ini ) ) return false;
	
	// 内部RAM
	for( int i=0; i<(int)IINTRAM.Size; i+=64 ){
		char stren[16],strva[256];
		sprintf( stren, "IntRam_%04X", i );
		for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", IntRam[i+j] );
		Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
	}
	
	// 外部RAM
	if( UseExtRam ){
		for( int i=0; i<0x10000; i+=64 ){		// 64KB
			char stren[16],strva[256];
			sprintf( stren, "ExtRam_%04X", i );
			for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", ExtRam[i+j] );
			Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
		}
	}
	
	if( UseSol ){
		// 現在の実装では外部RAMを使っていないので無視
		// 戦士のカートリッジ用外部RAMは0x6000-0x7FFFを使用
		//		for( int i=0x6000; i<0x8000; i+=64 ){	// 2KB
		//			char stren[16],strva[256];
		//			sprintf( stren, "ExtRam_%04X", i );
		//			for( int j=0; j<64; j++ ) sprintf( &strva[j*2], "%02X", ExtRam[i+j] );
		//			Ini->PutEntry( "MEMORY", NULL, stren, "%s", strva );
		//		}
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool MEM6::DokoLoad( cIni *Ini )
{
	char strfp[PATH_MAX];
	int st;
	
	PRINTD( MEM_LOG, "[MEM][DokoLoad]\n" );
	
	if( !Ini ) return false;
	
	Ini->GetTruth( "MEMORY", "CGBank",		&CGBank,	CGBank );
	Ini->GetTruth( "MEMORY", "UseExtRam",	&UseExtRam,	UseExtRam );
	Ini->GetInt(   "MEMORY", "M1Wait",		&M1Wait,	M1Wait );
	Ini->GetTruth( "MEMORY", "UseSoldier",	&UseSol,	UseSol );
	Ini->GetInt(   "MEMORY", "SolBank",		&SolBank,	SolBank );
	
	// 62,66,64,68
	Ini->GetTruth( "MEMORY", "cgrom",		&cgrom,		cgrom );
	Ini->GetTruth( "MEMORY", "kj_rom",		&kj_rom,	kj_rom );
	Ini->GetTruth( "MEMORY", "kj_LR",		&kj_LR,		kj_LR );
	Ini->GetTruth( "MEMORY", "cgenable",	&cgenable,	cgenable );
	Ini->GetInt(   "MEMORY", "cgaddr",		&st,		cgaddr );	cgaddr = st;
	Ini->GetInt(   "MEMORY", "Rf0",			&st,		Rf[0] );	Rf[0]  = st;
	Ini->GetInt(   "MEMORY", "Rf1",			&st,		Rf[1] );	Rf[1]  = st;
	Ini->GetInt(   "MEMORY", "Rf2",			&st,		Rf[2] );	Rf[2]  = st;
	
	// 64,68
	for( int i=0; i<16; i++ ){
		char stren[16];
		sprintf( stren, "RfSR_%02d", i );
		Ini->GetInt( "MEMORY", stren,	&st,	RfSR[i] );	RfSR[i] = st;
	}
	
	// 拡張ROM
	if( Ini->GetString( "MEMORY", "FilePath", strfp, "" ) )
		MountExtRom( strfp );
	
	return true;
}

bool MEM60::DokoLoad( cIni *Ini )
{
	if( !Ini || !MEM6::DokoLoad( Ini ) ) return false;
	
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
	if( UseExtRam || UseSol ){
		for( int i=0; i<0x4000; i+=64 ){		// 16KB
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
	}
	
	if( UseSol ){
		// 戦士のカートリッジ用外部RAMは0x6000-0x7FFFを使用
		for( int i=0x6000; i<0x8000; i+=64 ){	// 2KB
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
	}
	
	Init( UseExtRam, UseSol );
	SetMemBlockR( 0, 0 );
	SetMemBlockW( 0 );
	SetCGBank( CGBank );
	
	// 戦士のカートリッジ ------------------------------------------
	if( UseSol ) SetSolBank( SolBank );	// メモリバンク初期化
	// -------------------------------------------------------------

	// メモリウェイト
	int st;
	Ini->GetInt(    "MEMORY", "Wait",       &st,        GetWait() );    SetWait(st);
	// CGRomウェイト
	Ini->GetInt(    "MEMORY", "CgRomWait",  &st,        CGROM1.GetWait() );
	int	cgwait = st&0xff;
	CGROM1.SetWait( cgwait, cgwait );
	CGROM2.SetWait( cgwait, cgwait );

	return true;
}

bool MEM62::DokoLoad( cIni *Ini )
{
	if( !Ini || !MEM6::DokoLoad( Ini ) ) return false;
	
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
	if( UseExtRam ){
		for( int i=0; i<0x10000; i+=64 ){
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
	}
	
	if( UseSol ){
		// 現在の実装では外部RAMを使っていないので無視
		// 戦士のカートリッジ用外部RAMは0x6000-0x7FFFを使用
		//		for( int i=0x6000; i<0x8000; i+=64 ){	// 2KB
		//			char stren[16],strva[256];
		//			sprintf( stren, "ExtRam_%04X", i );
		//			memset( strva, '0', 64*2 );
		//			if( Ini->GetString( "MEMORY", stren, strva, strva ) ){
		//				for( int j=0; j<64; j++ ){
		//					char dt[5] = "0x";
		//					strncpy( &dt[2], &strva[j*2], 2 );
		//					ExtRam[i+j] = strtol( dt, NULL, 16 );
		//				}
		//			}
		//		}
	}
	
	Init( UseExtRam, UseSol );
	SetMemBlockR( Rf[0], Rf[1] );
	SetMemBlockW( Rf[2] );
	SetCGBank( CGBank );
	
	// 戦士のカートリッジ ------------------------------------------
	if( UseSol ) SetSolBank( SolBank );	// メモリバンク初期化
	// -------------------------------------------------------------

	// メモリウェイト
	int st;
	Ini->GetInt(    "MEMORY", "Wait",       &st,        GetWait() );    SetWait(st);
	// CGRomウェイト
	Ini->GetInt(    "MEMORY", "CgRomWait",  &st,        CGROM1.GetWait() );
	int	cgwait = st&0xff;
	CGROM1.SetWait( cgwait, cgwait );
	CGROM2.SetWait( cgwait, cgwait );

	return true;
}

bool MEM64::DokoLoad( cIni *Ini )
{
	if( !Ini || !MEM62::DokoLoad( Ini ) ) return false;
	
	for( int i=0; i<16; i++ )
		SetMemBlockSR( i, RfSR[i] );
	
	return true;
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////////////////////////////////////////
// モニタモード用関数
////////////////////////////////////////////////////////////////
const char *MEM6::GetReadMemBlk( int blk ) const 
{
	return vm->VdgIsSRmode() ? Rm_blkSR[blk]->GetName()
							 : Rm_blk  [blk]->GetName();
}

const char *MEM6::GetWriteMemBlk( int blk ) const 
{
	return vm->VdgIsSRmode() ? Wm_blkSR[blk]->GetName()
							 : Wm_blk  [blk]->GetName();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



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

const Device::Descriptor MEM64::descriptor = {
	MEM64::indef, MEM64::outdef
};

const Device::OutFuncPtr MEM64::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &MEM64::Out6xH ),
	STATIC_CAST( Device::OutFuncPtr, &MEM64::Out7FH ),
	STATIC_CAST( Device::OutFuncPtr, &MEM64::OutC1H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM64::OutC2H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM64::OutC3H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM64::OutF0H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM64::OutF1H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM64::OutF2H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM64::OutF3H ),
	STATIC_CAST( Device::OutFuncPtr, &MEM64::OutF8H )
};

const Device::InFuncPtr MEM64::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &MEM64::In6xH ),
	STATIC_CAST( Device::InFuncPtr, &MEM64::InF0H ),
	STATIC_CAST( Device::InFuncPtr, &MEM64::InF1H ),
	STATIC_CAST( Device::InFuncPtr, &MEM64::InF2H ),
	STATIC_CAST( Device::InFuncPtr, &MEM64::InF3H ),
	STATIC_CAST( Device::InFuncPtr, &MEM64::InB2H )
};
