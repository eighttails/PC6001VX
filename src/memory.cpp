/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <cstring>
#include <map>

#include "pc6001v.h"

#include "error.h"
#include "log.h"
#include "memory.h"
#include "p6vm.h"




// メモリブロック割り当て
// 内部ROM
enum {
	EMPTYROM = 0,	// EmptyRom
	EMPTYRAM,		// EmptyRam
	MAINROM,		// SysRom1
	MAINROM1,		// SysRom1+0x2000
	MAINROM2,		// SysRom1+0x4000
	MAINROM3,		// SysRom1+0x6000
	MAINROM4,		// SysRom1+0x8000
	MAINROM5,		// SysRom1+0xa000
	MAINROM6,		// SysRom1+0xc000
	MAINROM7,		// SysRom1+0xe000
	CGROM1,			// CGRom1
	CGROM2,			// CGRom2			CGRom1+0x2000
	SRMENROM,		// MenuRom			SysRom2
	SRMENROM1,		// MenuRom+0x2000	SysRom2+0x2000
	VOICEROM,		// VoiceRom			SysRom2+0x4000
	VOICEROM1,		// VoiceRom+0x2000	SysRom2+0x6000
	KANJIROM,		// KanjiRom			SysRom2+0x8000
	KANJIROM1,		// KanjiRom+0x2000	SysRom2+0xa000
	KANJIROM2,		// KanjiRom+0x4000	SysRom2+0xc000
	KANJIROM3,		// KanjiRom+0x6000	SysRom2+0xe000
	
	RWCOMMON,		// 汎用読込み/書込み
	RWAREAA60,		// AreaA,B読込み/書込み(60)
	RWAREAC60,		// AreaC読込み/書込み(60)
	RWAREAD60,		// AreaD読込み/書込み(60)
	KNJROM,			// 漢字ROM選択用
	
	EndofIROM
};

// 内部RAM
enum {
	INTRAM = 0,		// IntRam
	INTRAM1,		// IntRam+0x2000
	INTRAM2,		// IntRam+0x4000
	INTRAM3,		// IntRam+0x6000
	INTRAM4,		// IntRam+0x8000
	INTRAM5,		// IntRam+0xa000
	INTRAM6,		// IntRam+0xc000
	INTRAM7,		// IntRam+0xe000
	
	IEXRAM,			// IntRam+ExtRam
	IEXRAM1,		// IntRam+ExtRam+0x2000
	IEXRAM2,		// IntRam+ExtRam+0x4000
	IEXRAM3,		// IntRam+ExtRam+0x6000
	IEXRAM4,		// IntRam+ExtRam+0x8000
	IEXRAM5,		// IntRam+ExtRam+0xa000
	IEXRAM6,		// IntRam+ExtRam+0xc000
	IEXRAM7,		// IntRam+ExtRam+0xe000
	
	EndofIRAM
};

// 外部ROM/RAM
enum {
	EXTRAM = 0,		// ExtRam
	EXTRAM1,		// ExtRam+0x2000
	EXTRAM2,		// ExtRam+0x4000
	EXTRAM3,		// ExtRam+0x6000
	EXTRAM4,		// ExtRam+0x8000
	EXTRAM5,		// ExtRam+0xa000
	EXTRAM6,		// ExtRam+0xc000
	EXTRAM7,		// ExtRam+0xe000
	
	EXTROM,			// ExtRom
	EXTROM1,		// ExtRom+0x2000
	
	EndofEMEM
};


// メモリコントローラ内部レジスタ初期値
#define	INIT_RF0	(0x71)
#define INIT_RF1	(0xdd)
#define INIT_RF2	(0x50)




/////////////////////////////////////////////////////////////////////////////
// ROM情報
/////////////////////////////////////////////////////////////////////////////

// Dummy                                   ROMファイル名	サイズ		CRC32
const std::vector<ROMINFO> NOROM     = {};

// PC-6001                                 ROMファイル名	サイズ		CRC32
const std::vector<ROMINFO> SYSROM160 = { { "BASICROM.60",	0x04000,	0x54c03109 },
										 { "BASICROM.60",	0x04000,	0          } };	// 互換BASIC
const std::vector<ROMINFO> CGROM160  = { { "CGROM60.60",	0x01000,	0xb0142d32 },
										 { "CGROM60.60",	0x01000,	0          } };	// 互換BASIC

// PC-6001A                                ROMファイル名	サイズ		CRC32
const std::vector<ROMINFO> SYSROM161 = { { "BASICROM.61",	0x04000,	0xfa8e88d9 } };
const std::vector<ROMINFO> CGROM161  = { { "CGROM60.61",	0x01000,	0x49c21d08 } };

// PC-6001mk2                              ROMファイル名	サイズ		CRC32
const std::vector<ROMINFO> SYSROM162 = { { "BASICROM.62",	0x08000,	0x950ac401 },	// 前期
										 { "BASICROM.62",	0x08000,	0xd7e61957 },	// 後期
										 { "BASICROM.62",	0x08000,	0          } };	// 互換BASIC
const std::vector<ROMINFO> CGROM162  = { { "CGROM60.62",	0x02000,	0x81eb5d95 },
										 { "CGROM60.62",	0x02000,	0          } };	// 互換BASIC
const std::vector<ROMINFO> CGROM262  = { { "CGROM60m.62",	0x02000,	0x3ce48c33 },
										 { "CGROM60m.62",	0x02000,	0          } };	// 互換BASIC
const std::vector<ROMINFO> KANJI62   = { { "KANJIROM.62",	0x08000,	0x20c8f3eb } };
const std::vector<ROMINFO> VOICE62   = { { "VOICEROM.62",	0x04000,	0x49b4f917 },
										 { "VOICEROM.62",	0x04000,	0          } };	// 互換BASIC

// PC-6601                                 ROMファイル名	サイズ		CRC32
const std::vector<ROMINFO> SYSROM166 = { { "BASICROM.66",	0x08000,	0xc0b01772 },
										 { "BASICROM.66",	0x08000,	0          } };	// 互換BASIC
const std::vector<ROMINFO> CGROM166  = { { "CGROM60.66",	0x02000,	0xd2434f29 },
										 { "CGROM60.66",	0x02000,	0          } };	// 互換BASIC
const std::vector<ROMINFO> CGROM266  = { { "CGROM66.66",	0x02000,	0x3ce48c33 },
										 { "CGROM66.66",	0x02000,	0          } };	// 互換BASIC
const std::vector<ROMINFO> KANJI66   = { { "KANJIROM.66",	0x08000,	0x20c8f3eb } };
const std::vector<ROMINFO> VOICE66   = { { "VOICEROM.66",	0x04000,	0x91d078c1 },
										 { "VOICEROM.66",	0x04000,	0          } };	// 互換BASIC

// PC-6001mk2SR / PC-6601SR                ROMファイル名	サイズ		CRC32
const std::vector<ROMINFO> SYSROM164 = { { "SYSTEMROM1.64",	0x10000,	0xb6fc2db2 },
										 { "SYSTEMROM1.68",	0x10000,	0xb6fc2db2 } };
const std::vector<ROMINFO> SYSROM264 = { { "SYSTEMROM2.64",	0x10000,	0x55a62a1d },
										 { "SYSTEMROM2.68",	0x10000,	0x55a62a1d } };
const std::vector<ROMINFO> CGROM164  = { { "CGROM68.64",	0x04000,	0x73bc3256 },
										 { "CGROM68.68",	0x04000,	0x73bc3256 } };

// 拡張カートリッジ
const std::vector<ROMINFO> EXBASIC00 = { { "EXBASIC.ROM",	0x02000,	0          } };	// CRCは後で調べる
const std::vector<ROMINFO> EXKANJI00 = { { "EXKANJI.ROM",	0x020000,	0          } };	// 自作フォントを想定してCRCなし？ただし存在チェックで0x2c9(16bit)のデータを0x4141に固定する必要あり
const std::vector<ROMINFO> EXVOICE00 = { { "EXVOICE.ROM",	0x04000,	0          } };	// CRCは後で調べる



/////////////////////////////////////////////////////////////////////////////
// ROMセット情報
/////////////////////////////////////////////////////////////////////////////
const std::map<int, const std::vector<std::vector<ROMINFO>>> ROMSET {
										{ 0,  {}													},
										{ 60, { SYSROM160, CGROM160 }								},
										{ 61, { SYSROM161, CGROM161 }								},
										{ 62, { SYSROM162, CGROM162,  CGROM262, KANJI62, VOICE62 }	},
										{ 66, { SYSROM166, CGROM166,  CGROM266, KANJI66, VOICE66 }	},
										{ 64, { SYSROM164, SYSROM264, CGROM164 }					},
										{ 68, { SYSROM164, SYSROM264, CGROM164 }					} };



/////////////////////////////////////////////////////////////////////////////
// メモリ情報
/////////////////////////////////////////////////////////////////////////////
// 共通									 ROM情報	サイズ		初期値	Wait
const MEMB::MEMINFO MEMB::IEMPTROM   = { NOROM,		0x002000,	0xff,	1 };
const MEMB::MEMINFO MEMB::IEMPTRAM   = { NOROM,		0x002000,	0xff,	0 };

const MEMB::MEMINFO MEM6::IEXTROM8   = { NOROM,		0x002000,	0xff,	1 };
const MEMB::MEMINFO MEM6::IEXTROM16  = { NOROM,		0x004000,	0xff,	1 };
const MEMB::MEMINFO MEM6::IEXTROM128 = { NOROM,		0x020000,	0xff,	1 };
const MEMB::MEMINFO MEM6::IEXTROM512 = { NOROM,		0x080000,	0xff,	1 };
const MEMB::MEMINFO MEM6::IEXTROM8M  = { NOROM,		0x800000,	0xff,	1 };

const MEMB::MEMINFO MEM6::IEXTRAM16  = { NOROM,		0x004000,	0x00,	0 };
const MEMB::MEMINFO MEM6::IEXTRAM32  = { NOROM,		0x008000,	0x00,	0 };
const MEMB::MEMINFO MEM6::IEXTRAM64  = { NOROM,		0x010000,	0x00,	0 };
const MEMB::MEMINFO MEM6::IEXTRAM128 = { NOROM,		0x020000,	0x00,	0 };
const MEMB::MEMINFO MEM6::IEXTRAM512 = { NOROM,		0x080000,	0x00,	0 };

// PC-6001								 ROM情報	サイズ		初期値	Wait
const MEMB::MEMINFO MEM60::ISYSROM1  = { SYSROM160,	0x004000,	0xff,	1 };
const MEMB::MEMINFO MEM60::ICGROM1   = { CGROM160,	0x001000,	0xff,	1 };
const MEMB::MEMINFO MEM60::IINTRAM   = { NOROM,		0x004000,	0x00,	0 };

// PC-6001A								 ROM情報	サイズ		初期値	Wait
const MEMB::MEMINFO MEM61::ISYSROM1  = { SYSROM161,	0x004000,	0xff,	1 };
const MEMB::MEMINFO MEM61::ICGROM1   = { CGROM161,	0x001000,	0xff,	1 };
const MEMB::MEMINFO MEM61::IINTRAM   = { NOROM,		0x004000,	0x00,	0 };

// PC-6001mk2							 ROM情報	サイズ		初期値	Wait
const MEMB::MEMINFO MEM62::ISYSROM1  = { SYSROM162,	0x008000,	0xff,	1 };
const MEMB::MEMINFO MEM62::ICGROM1   = { CGROM162,	0x002000,	0xff,	1 };
const MEMB::MEMINFO MEM62::ICGROM2   = { CGROM262,	0x002000,	0xff,	1 };
const MEMB::MEMINFO MEM62::IKANJI    = { KANJI62,	0x008000,	0xff,	1 };
const MEMB::MEMINFO MEM62::IVOICE    = { VOICE62,	0x004000,	0xff,	1 };
const MEMB::MEMINFO MEM62::IINTRAM   = { NOROM,		0x010000,	0x00,	0 };

// PC-6601								 ROM情報	サイズ		初期値	Wait
const MEMB::MEMINFO MEM66::ISYSROM1  = { SYSROM166,	0x008000,	0xff,	1 };
const MEMB::MEMINFO MEM66::ICGROM1   = { CGROM166,	0x002000,	0xff,	1 };
const MEMB::MEMINFO MEM66::ICGROM2   = { CGROM266,	0x002000,	0xff,	1 };
const MEMB::MEMINFO MEM66::IKANJI    = { KANJI66,	0x008000,	0xff,	1 };
const MEMB::MEMINFO MEM66::IVOICE    = { VOICE66,	0x004000,	0xff,	1 };
const MEMB::MEMINFO MEM66::IINTRAM   = { NOROM,		0x010000,	0x00,	0 };

// PC-6001mk2SR / PC-6601SR				 ROM情報	サイズ		初期値	Wait
const MEMB::MEMINFO MEM64::ISYSROM1  = { SYSROM164,	0x010000,	0xff,	1 };
const MEMB::MEMINFO MEM64::ISYSROM2  = { SYSROM264,	0x010000,	0xff,	1 };
const MEMB::MEMINFO MEM64::ICGROM1   = { CGROM164,	0x004000,	0xff,	1 };
const MEMB::MEMINFO MEM64::IINTRAM   = { NOROM,		0x010000,	0x00,	0 };

// 拡張カートリッジ
const MEMB::MEMINFO MEM6::IEXBASIC   = { EXBASIC00,	0x002000,	0xff,	1 };
const MEMB::MEMINFO MEM6::IEXKANJI   = { EXKANJI00,	0x020000,	0xff,	0 };
const MEMB::MEMINFO MEM6::IEXVOICE   = { EXVOICE00,	0x004000,	0xff,	1 };




/////////////////////////////////////////////////////////////////////////////
// ROMセット情報取得
/////////////////////////////////////////////////////////////////////////////
const std::vector<std::vector<ROMINFO>>& GetRomSetList( const int model )
{
	try{
		return ROMSET.at( model );
	}
	catch( std::out_of_range& ){
		return ROMSET.at( 0 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CRC32計算
//
// 引数:	buf				データバッファへのポインタ
//			num				データ数(バイト)
// 返値:	DWORD			CRC32値
/////////////////////////////////////////////////////////////////////////////
static DWORD CalcCrc32( MemCells& buf, int num )
{
	DWORD crc = 0xffffffff;
	
	for( int i = 0; i < num; i++ ){
		crc ^= buf.Read( i );
		for( int j = 0; j < 8; j++ )
			if( crc & 1 ) crc   = (crc >> 1) ^ 0xedb88320;
			else		  crc >>= 1;
	}
	return crc ^ 0xffffffff;
}



/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
MEMB::MEMB( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
MEMB::~MEMB( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// メモリ確保とROMファイル読込み
//
// 引数:	buf			対象メモリセルへの参照
//			info		メモリ情報ポインタ
//			path		ROMフォルダパス名への参照
//			crc			CRCチェック true:する false:しない
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEMB::AllocMemory( MemCells& buf, const MEMINFO* info, const P6VPATH& path, bool crc )
{
	PRINTD( MEM_LOG, "[MEMB][AllocMemory] " );
	
	int i = 0;
	bool ErrSize = false;
	bool ErrCrc  = false;
	std::string fpath = "";
	
	try{
		// メモリリサイズ
		buf.Resize( info->Size, info->Init );
		
		// ROM情報なし ならばRAMまたはnullptr
		if( info->Rinfo.empty() ) return true;
		
		// ファイル候補の数だけ繰り返し
		do{
			fpath = info->Rinfo[i].FileName;
			
			PRINTD( MEM_LOG, "-> %s ", fpath.c_str() );
			
			// ファイルの存在チェック
			std::vector<P6VPATH> ffiles;
			ffiles.clear();
			OSD_FindFile( path, STR2P6VPATH( fpath ), ffiles, info->Size );
			
			// 見つからなければ次の候補を探す
			if( ffiles.empty() ) continue;
			
			// ROMデータをファイルから読込み
			for( auto& ff : ffiles ){
				if( buf.SetData( ff ) ){
					// CRC32が一致するものを探す
					// crc=false または CRC=0の時はチェックしない
					if( !crc || (info->Rinfo[i].Crc == 0) || (CalcCrc32( buf, info->Size ) == info->Rinfo[i].Crc) ){
						PRINTD( MEM_LOG, "-> OK\n" );
						return true;
					}
				}
			}
			
			// どれも一致しなければCRCエラー
			ErrCrc = true;
			
		}while( ++i < (int)info->Rinfo.size() );
		
		if     ( ErrCrc )  throw Error::RomCrcNG;
		else if( ErrSize ) throw Error::RomSizeNG;
		else               throw Error::NoRom;
	}
	catch( Error::Errno i ){	// 例外発生
		PRINTD( MEM_LOG, "-> Failed\n" );
		
		switch( i ){
		case Error::NoRom:		// ファイルが見つからない場合
			Error::SetError( i, info->Rinfo[0].FileName );
			break;
			
		case Error::RomSizeNG:	// サイズが合わない場合
		case Error::RomCrcNG:	// CRCが合わない場合
		default:				// メモリを開放
			Error::SetError( i, fpath );
			buf.Resize( 0 );
		}
		return false;
	}
	
	return false;
}




/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
MEM6::MEM6( VM6* vm, const ID& id ) : Device( vm, id ),
	CGBank( false ), M1Wait( 1 ),
	// 拡張カートリッジ ======================================================================
	ExCart( 0 ), FilePath( "" ),
	Kaddr( 0 ), Kenable( false ),
	Sol60Mode( false ), CS01R( false ), CS01W( false ), CS02W( false ), SolBankSet( 0 )
	// =======================================================================================
{
	IRom.assign( EndofIROM, MemBlock() );
	IRam.assign( EndofIRAM, MemBlock() );
	
	RBLK1.fill( &IRom[EMPTYROM] );
	WBLK1.fill( &IRom[EMPTYROM] );
	RBLK2.fill( &IRom[EMPTYROM] );
	WBLK2.fill( &IRom[EMPTYROM] );
	
	Rf = { INIT_RF0, INIT_RF1, INIT_RF2 };
	
	descs.outdef.clear();
	descs.indef.clear();
	
	
	// 拡張カートリッジ ======================================================================
	EMem.assign( EndofEMEM, MemBlock() );
	// =======================================================================================
}

MEM60::MEM60( VM6* vm, const ID& id ) : MEM6( vm, id )
{
	MemTable.System1 = &MEM60::ISYSROM1;
	MemTable.CGRom1  = &MEM60::ICGROM1;
	MemTable.IntRam  = &MEM60::IINTRAM;
}

MEM61::MEM61( VM6* vm, const ID& id ) : MEM60( vm, id )
{
	MemTable.System1 = &MEM61::ISYSROM1;
	MemTable.CGRom1  = &MEM61::ICGROM1;
	MemTable.IntRam  = &MEM61::IINTRAM;
}

MEM62::MEM62( VM6* vm, const ID& id ) : MEM6( vm, id ),
	kj_rom( true ), kj_LR( true ), cgrom( true ), cgenable( true ), cgaden( 0 ), cgaddr( 3 ), c2acc( 0xff )
{
	MemTable.System1 = &MEM62::ISYSROM1;
	MemTable.CGRom1  = &MEM62::ICGROM1;
	MemTable.CGRom2  = &MEM62::ICGROM2;
	MemTable.Kanji   = &MEM62::IKANJI;
	MemTable.Voice   = &MEM62::IVOICE;
	MemTable.IntRam  = &MEM62::IINTRAM;
	
	// Device Description (Out)
	descs.outdef.emplace( outC1H, STATIC_CAST( Device::OutFuncPtr, &MEM62::OutC1H ) );
	descs.outdef.emplace( outC2H, STATIC_CAST( Device::OutFuncPtr, &MEM62::OutC2H ) );
	descs.outdef.emplace( outC3H, STATIC_CAST( Device::OutFuncPtr, &MEM62::OutC3H ) );
	descs.outdef.emplace( outF0H, STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF0H ) );
	descs.outdef.emplace( outF1H, STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF1H ) );
	descs.outdef.emplace( outF2H, STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF2H ) );
	descs.outdef.emplace( outF3H, STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF3H ) );
	descs.outdef.emplace( outF8H, STATIC_CAST( Device::OutFuncPtr, &MEM62::OutF8H ) );
	
	// Device Description (In)
	descs.indef.emplace ( inC2H,  STATIC_CAST( Device::InFuncPtr,  &MEM62::InC2H  ) );
	descs.indef.emplace ( inF0H,  STATIC_CAST( Device::InFuncPtr,  &MEM62::InF0H  ) );
	descs.indef.emplace ( inF1H,  STATIC_CAST( Device::InFuncPtr,  &MEM62::InF1H  ) );
	descs.indef.emplace ( inF2H,  STATIC_CAST( Device::InFuncPtr,  &MEM62::InF2H  ) );
	descs.indef.emplace ( inF3H,  STATIC_CAST( Device::InFuncPtr,  &MEM62::InF3H  ) );
}

MEM66::MEM66( VM6* vm, const ID& id ) : MEM62( vm, id )
{
	MemTable.System1 = &MEM66::ISYSROM1;
	MemTable.CGRom1  = &MEM66::ICGROM1;
	MemTable.CGRom2  = &MEM66::ICGROM2;
	MemTable.Kanji   = &MEM66::IKANJI;
	MemTable.Voice   = &MEM66::IVOICE;
	MemTable.IntRam  = &MEM66::IINTRAM;
}

MEM64::MEM64( VM6* vm, const ID& id ) : MEM62( vm, id )
{
	RBLKSR.fill( &IRom[EMPTYROM] );
	WBLKSR.fill( &IRom[EMPTYROM] );
	RfSR.fill( 0 );
	
	MemTable.System1 = &MEM64::ISYSROM1;
	MemTable.System2 = &MEM64::ISYSROM2;
	MemTable.CGRom1  = &MEM64::ICGROM1;
	MemTable.CGRom2  = nullptr;
	MemTable.Kanji   = nullptr;
	MemTable.Voice   = nullptr;
	MemTable.IntRam  = &MEM64::IINTRAM;
	
	// Device Description (Out)
	descs.outdef.emplace( out6xH, STATIC_CAST( Device::OutFuncPtr, &MEM64::Out6xH ) );
	
	// Device Description (In)
	descs.indef.emplace ( in6xH,  STATIC_CAST( Device::InFuncPtr,  &MEM64::In6xH  ) );
	descs.indef.emplace ( inB2H,  STATIC_CAST( Device::InFuncPtr,  &MEM64::InB2H  ) );
}

MEM68::MEM68( VM6* vm, const ID& id ) : MEM64( vm, id )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
MEM6::~MEM6( void )
{
}

MEM60::~MEM60( void )
{
}

MEM61::~MEM61( void )
{
}

MEM62::~MEM62( void )
{
}

MEM66::~MEM66( void )
{
}

MEM64::~MEM64( void )
{
}

MEM68::~MEM68( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// メモリブロック用関数
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 汎用 メモリブロック名取得
/////////////////////////////////////////////////////////////////////////////
const std::string& MEM6::GetNameCommon( WORD addr, bool rw )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	return rw ? WBLK2[idx]->GetName( addr, rw )
			  : RBLK2[idx]->GetName( addr, rw );
}

const std::string& MEM62::GetNameCommon( WORD addr, bool rw )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	// Port92HとF8H 両方とも設定されていたらCG ROM有効
	if( CGBank && cgenable && ((idx | cgaden) == cgaddr) ){
		return IRom[cgrom ? CGROM1 : CGROM2].GetName( addr, rw );
	}
	return rw ? WBLK2[idx]->GetName( addr, rw )
			  : RBLK2[idx]->GetName( addr, rw );
}


/////////////////////////////////////////////////////////////////////////////
// 汎用 読込み
/////////////////////////////////////////////////////////////////////////////
BYTE MEM6::CommonRead( MemCell* ptr, WORD addr, int* wcnt )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	return RBLK2[idx]->Read( addr, wcnt );
}

BYTE MEM62::CommonRead( MemCell* ptr, WORD addr, int* wcnt )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	// Port92HとF8H 両方とも設定されていたらCG ROM有効
	if( CGBank && cgenable && ((idx | cgaden) == cgaddr) ){
		return IRom[cgrom ? CGROM1 : CGROM2].Read( addr, wcnt );
	}
	return RBLK2[idx]->Read( addr, wcnt );
}


/////////////////////////////////////////////////////////////////////////////
// 汎用 書込み
/////////////////////////////////////////////////////////////////////////////
void MEM6::CommonWrite( MemCell* ptr, WORD addr, BYTE data, int* wcnt )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	WBLK2[idx]->Write( addr, data, wcnt );
	
	// 拡張カートリッジに直接書き込みする場合
	// RAS/CASではなくWRで判定しているようなケース　たぶん
	if( GetWriteEnableExt( addr ) ){ EMem[idx].Write( addr, data ); }
}


/////////////////////////////////////////////////////////////////////////////
// AreaA,B メモリブロック名取得(60)
/////////////////////////////////////////////////////////////////////////////
const std::string& MEM60::GetNameAreaA60Read( WORD addr, bool rw )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	return rw ? GetWriteEnableExt( addr ) ? EMem[idx].GetName( addr, rw )
										  : WBLK2[idx]->GetName( addr, rw )
			  : GetReadEnableExt ( addr ) ? EMem[idx].GetName( addr, rw )
										  : RBLK2[idx]->GetName( addr, rw );
}


/////////////////////////////////////////////////////////////////////////////
// AreaA,B 読込み(60)
/////////////////////////////////////////////////////////////////////////////
BYTE MEM60::AreaA60Read( MemCell* ptr, WORD addr, int* wcnt )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	// ウェイトは常にSystem1相当
	if( wcnt ){ *wcnt += MemTable.System1->Wait; }
	
	return GetReadEnableExt( addr ) ? EMem[idx].Read( addr )
									: RBLK2[idx]->Read( addr );
}


/////////////////////////////////////////////////////////////////////////////
// AreaA,B,C,D 書込み(60)
/////////////////////////////////////////////////////////////////////////////
void MEM60::AreaABCD60Write( MemCell* ptr, WORD addr, BYTE data, int* wcnt )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	// ウェイトは常にSystem1相当
	if( wcnt ){ *wcnt += MemTable.System1->Wait; }
	
	if( GetWriteEnableExt( addr ) ){ EMem[idx].Write( addr, data ); }
}


/////////////////////////////////////////////////////////////////////////////
// AreaC メモリブロック名取得(60)
/////////////////////////////////////////////////////////////////////////////
const std::string& MEM60::GetNameAreaC60Read( WORD addr, bool rw )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	return rw ? GetWriteEnableExt( addr ) ? EMem[idx].GetName( addr, rw )
										  : WBLK2[idx]->GetName( addr, rw )
			  : RBLK2[idx]->GetName( addr, rw );
}


/////////////////////////////////////////////////////////////////////////////
// AreaC 読込み(60)
/////////////////////////////////////////////////////////////////////////////
BYTE MEM60::AreaC60Read( MemCell* ptr, WORD addr, int* wcnt )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	// ウェイトは常にSystem1相当
	if( wcnt ){ *wcnt += MemTable.System1->Wait; }
	
	return RBLK2[idx]->Read( addr );
}


/////////////////////////////////////////////////////////////////////////////
// AreaD メモリブロック名取得(60)
/////////////////////////////////////////////////////////////////////////////
const std::string& MEM60::GetNameAreaD60Read( WORD addr, bool rw )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	if( CGBank && (!(addr & 0x1000) || (RBLK2[idx] == &IRom[EMPTYROM])) ){
		return IRom[CGROM1].GetName( addr, rw );
	}
	return rw ? GetWriteEnableExt( addr ) ? EMem[idx].GetName( addr, rw )
										  : WBLK2[idx]->GetName( addr, rw )
			  : RBLK2[idx]->GetName( addr, rw );
}


/////////////////////////////////////////////////////////////////////////////
// AreaD 読込み(60)
/////////////////////////////////////////////////////////////////////////////
BYTE MEM60::AreaD60Read( MemCell* ptr, WORD addr, int* wcnt )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	// ウェイトは常にSystem1相当
	if( wcnt ){ *wcnt += MemTable.System1->Wait; }
	
	if( CGBank && (!(addr & 0x1000) || (RBLK2[idx] == &IRom[EMPTYROM])) ){
		// 前半4KBはそのままCGROMデータを返す
		// 後半4KBは外部ROMがなければCGROMのイメージを返す ホント？
		// 外部ROMがあったら外部ROMを読む ホント？
		return IRom[CGROM1].Read( addr & 0x0fff );
	}
	return RBLK2[idx]->Read( addr );
}


/////////////////////////////////////////////////////////////////////////////
// 内部/外部RAM書込み
/////////////////////////////////////////////////////////////////////////////
void MEM62::IERamWrite( MemCell* ptr, WORD addr, BYTE data, int* wcnt )
{
	IRam[addr >> MemBlock::PAGEBITS].Write( addr, data, wcnt );
	EMem[addr >> MemBlock::PAGEBITS].Write( addr, data );		// ウェイト2重カウントしない
}


/////////////////////////////////////////////////////////////////////////////
// 漢字ROM メモリブロック名取得
/////////////////////////////////////////////////////////////////////////////
const std::string& MEM62::KanjiGetName( WORD addr, bool rw )
{
	int idx = (addr >> MemBlock::PAGEBITS) & 1;
	
	MemBlock& mb = kj_rom ? IRom[KANJIROM + (kj_LR ? 2 : 0) + idx] : IRom[VOICEROM + 0 + idx];
	return mb.GetName( addr );
}

const std::string& MEM64::KanjiGetName( WORD addr, bool rw )
{
	int idx =  (addr >> MemBlock::PAGEBITS) & 1;
	int mbi = ((addr >> MemBlock::PAGEBITS) & 6) ? VOICEROM + 0 : SRMENROM + 0;
	
	MemBlock& mb = kj_rom ? IRom[KANJIROM + (kj_LR ? 2 : 0) + idx] : IRom[mbi + idx];
	return mb.GetName( addr );
}


/////////////////////////////////////////////////////////////////////////////
// 漢字ROM読込み
/////////////////////////////////////////////////////////////////////////////
BYTE MEM62::ReadKanji( MemCell* ptr, WORD addr, int* wcnt )
{
	int idx = (addr >> MemBlock::PAGEBITS) & 1;
	
	MemBlock& mb = kj_rom ? IRom[KANJIROM + (kj_LR ? 2 : 0) + idx] : IRom[VOICEROM + 0 + idx];
	return mb.Read( addr, wcnt );
}

BYTE MEM64::ReadKanji( MemCell* ptr, WORD addr, int* wcnt )
{
	int idx =  (addr >> MemBlock::PAGEBITS) & 1;
	int mbi = ((addr >> MemBlock::PAGEBITS) & 6) ? VOICEROM + 0 : SRMENROM + 0;
	
	MemBlock& mb = kj_rom ? IRom[KANJIROM + (kj_LR ? 2 : 0) + idx] : IRom[mbi + idx];
	return mb.Read( addr, wcnt );
}


/////////////////////////////////////////////////////////////////////////////
// メモリブロック用関数 ここまで
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// 内部メモリ確保とROMファイル読込み
//
// 引数:	path		ROMフォルダパス名への参照
//			crc			CRCチェック true:する false:しない
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM6::AllocMemoryInt( const P6VPATH& path, bool crc )
{
	PRINTD( MEM_LOG, "[MEM][AllocMemoryInt]\n" );
	
	// 共通
	if( !AllocMemory( SysRom1, MemTable.System1, path, crc ) ) return false;
	if( !AllocMemory( CGRom1,  MemTable.CGRom1,  path, crc ) ) return false;
	if( !AllocMemory( IntRam,  MemTable.IntRam,  "",   crc ) ) return false;
	
	 // 内部メモリ確保とROMファイル読込み(機種別)
	if( !AllocMemorySpec( path, crc ) ) return false;
	
	// 内部RAMの初期値を設定
	SetRamValue();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 内部メモリ確保とROMファイル読込み(機種別)
//
// 引数:	path		ROMフォルダパス名への参照
//			crc			CRCチェック true:する false:しない
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM60::AllocMemorySpec( const P6VPATH& path, bool crc )
{
	return true;
}

bool MEM62::AllocMemorySpec( const P6VPATH& path, bool crc )
{
	PRINTD( MEM_LOG, "[MEM][AllocMemorySpec]\n" );
	
	if( !AllocMemory( CGRom2,   MemTable.CGRom2, path, crc ) ) return false;
	if( !AllocMemory( KanjiRom, MemTable.Kanji,  path, crc ) ) return false;
	if( !AllocMemory( VoiceRom, MemTable.Voice,  path, crc ) ) return false;
	
	return true;
}

bool MEM64::AllocMemorySpec( const P6VPATH& path, bool crc )
{
	PRINTD( MEM_LOG, "[MEM][AllocMemorySpec]\n" );
	
	if( !AllocMemory( SysRom2,  MemTable.System2, path, crc ) ) return false;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 内部RAMの初期値を設定
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void MEM60::SetRamValue( void )
{
	for( int i = 0; i < (int)MemTable.IntRam->Size; i++ ){
		IntRam.Write( i, i & 0x40 ? 0x00 : 0xff );
	}
}

void MEM62::SetRamValue( void )
{
	for( int i = 0; i < (int)MemTable.IntRam->Size; i++ ){
		IntRam.Write( i, ((i >> 7) ^ i) & 1 ? 0xff : 0x00 );
	}
}

void MEM64::SetRamValue( void )
{
	// 0000-FF9FH
	for( int i = 0; i < 0xffa0; i++ ){
		IntRam.Write( i, ((i >> 9) ^ (i >> 1)) & 1 ? 0xff : 0x00 );
	}
	// FFA0-FFFFH
	for( int i = 0xffa0; i < 0x10000; i++ ){
		IntRam.Write( i, 0x00 );
	}
}

void MEM68::SetRamValue( void )
{
	for( int i = 0; i < (int)MemTable.IntRam->Size; i++ ){
		IntRam.Write( i, i & 0x100 ? 0xff : 0x00 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// 内部メモリ初期化
//
// 引数:	なし
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM6::InitInt( void )
{
	PRINTD( MEM_LOG, "[MEM][InitInt]\n" );
	
	// とりあえず全メモリブロックをEmptyに設定(ROMはウェイトあり)
	for( auto &mb : IRom ){ mb.SetFunc( "EMPTY", nullptr, nullptr, 1 ); }
	for( auto &mb : IRam ){ mb.SetFunc( "EMPTY", nullptr, nullptr, 0 ); }
	
	// 外部メモリもここで初期化しておく(ROMはウェイトあり)
	for( auto &mb : EMem ){ mb.SetFunc( "EMPTY", nullptr, nullptr, 0 ); }
	EMem[EXTROM+0].SetWait( 1 );
	EMem[EXTROM+1].SetWait( 1 );
	
	// 汎用アクセス用
	IRom[RWCOMMON].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM6::GetNameCommon ) ), FR( STATIC_CAST( RFuncPtr, &MEM6::CommonRead ) ), FW( STATIC_CAST( WFuncPtr, &MEM6::CommonWrite ) ), 0 );
	
	// CGROMアクセス用に一段噛ませる
	RBLK1[0] = WBLK1[0] = &IRom[RWCOMMON];
	RBLK1[1] = WBLK1[1] = &IRom[RWCOMMON];
	RBLK1[2] = WBLK1[2] = &IRom[RWCOMMON];
	RBLK1[3] = WBLK1[3] = &IRom[RWCOMMON];
	RBLK1[4] = WBLK1[4] = &IRom[RWCOMMON];
	RBLK1[5] = WBLK1[5] = &IRom[RWCOMMON];
	RBLK1[6] = WBLK1[6] = &IRom[RWCOMMON];
	RBLK1[7] = WBLK1[7] = &IRom[RWCOMMON];
	
	// 内部メモリ初期化(機種別)
	if( !InitIntSpec() ) return false;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 内部メモリ初期化(機種別)
//
// 引数:	なし
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM60::InitIntSpec( void )
{
	PRINTD( MEM_LOG, "[MEM][InitIntSpec]\n" );
	
	// 60専用アクセス用
	IRom[RWAREAA60].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM60::GetNameAreaA60Read ) ), FR( STATIC_CAST( RFuncPtr, &MEM60::AreaA60Read ) ), FW( STATIC_CAST( WFuncPtr, &MEM60::AreaABCD60Write ) ), MemTable.System1->Wait );
	IRom[RWAREAC60].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM60::GetNameAreaC60Read ) ), FR( STATIC_CAST( RFuncPtr, &MEM60::AreaC60Read ) ), FW( STATIC_CAST( WFuncPtr, &MEM60::AreaABCD60Write ) ), MemTable.System1->Wait );
	IRom[RWAREAD60].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM60::GetNameAreaD60Read ) ), FR( STATIC_CAST( RFuncPtr, &MEM60::AreaD60Read ) ), FW( STATIC_CAST( WFuncPtr, &MEM60::AreaABCD60Write ) ), MemTable.CGRom1->Wait );
	
	// CGROMアクセス用に一段噛ませる
	RBLK1[0] = WBLK1[0] = &IRom[RWAREAA60];
	RBLK1[1] = WBLK1[1] = &IRom[RWAREAA60];
	RBLK1[2] = WBLK1[2] = &IRom[RWAREAC60];
	RBLK1[3] = WBLK1[3] = &IRom[RWAREAD60];
	
	
	// BASIC ROM
	IRom[MAINROM+0].SetMemory( "BASIC0", SysRom1( 0 ), MemTable.System1->Wait );
	IRom[MAINROM+1].SetMemory( "BASIC1", SysRom1( 1 ), MemTable.System1->Wait );
	
	// CG ROM
	IRom[CGROM1].SetMemory   ( "CGROM1", CGRom1( 0 ),  MemTable.CGRom1->Wait );
	
	// 内部RAM
	IRam[INTRAM+0].SetMemory ( "IRAM00", IntRam( 0 ),  MemTable.IntRam->Wait );
	IRam[INTRAM+1].SetMemory ( "IRAM01", IntRam( 1 ),  MemTable.IntRam->Wait );
	
	return true;
}

bool MEM62::InitIntSpec( void )
{
	PRINTD( MEM_LOG, "[MEM][InitIntSpec]\n" );
	
	// BASIC ROM
	IRom[MAINROM+0].SetMemory ( "BASIC0", SysRom1( 0 ),  MemTable.System1->Wait );
	IRom[MAINROM+1].SetMemory ( "BASIC1", SysRom1( 1 ),  MemTable.System1->Wait );
	IRom[MAINROM+2].SetMemory ( "BASIC2", SysRom1( 2 ),  MemTable.System1->Wait );
	IRom[MAINROM+3].SetMemory ( "BASIC3", SysRom1( 3 ),  MemTable.System1->Wait );
	
	// CG ROM
	IRom[CGROM1].SetMemory    ( "CGROM1", CGRom1( 0 ),   MemTable.CGRom1->Wait );
	IRom[CGROM2].SetMemory    ( "CGROM2", CGRom2( 0 ),   MemTable.CGRom2->Wait );
	
	// 漢字ROM
	IRom[KANJIROM+0].SetMemory( "KJROM0", KanjiRom( 0 ), MemTable.Kanji->Wait );
	IRom[KANJIROM+1].SetMemory( "KJROM1", KanjiRom( 1 ), MemTable.Kanji->Wait );
	IRom[KANJIROM+2].SetMemory( "KJROM2", KanjiRom( 2 ), MemTable.Kanji->Wait );
	IRom[KANJIROM+3].SetMemory( "KJROM3", KanjiRom( 3 ), MemTable.Kanji->Wait );
	IRom[KNJROM].SetFunc      ( FN( STATIC_CAST( NFuncPtr, &MEM62::KanjiGetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM62::ReadKanji ) ), nullptr, MemTable.Kanji->Wait );
	
	// 音声合成ROM
	IRom[VOICEROM+0].SetMemory( "VOROM0", VoiceRom( 0 ), MemTable.Voice->Wait );
	IRom[VOICEROM+1].SetMemory( "VOROM1", VoiceRom( 1 ), MemTable.Voice->Wait );
	
	// 内部RAM
	for( int i = 0; i < 8; i++ ){
		IRam[INTRAM+i].SetMemory( Stringf( "IRAM%02d", i ), IntRam( i ), MemTable.IntRam->Wait );
	}
	
	// 内部/外部RAM書込み
	for( int i = 0; i < 8; i++ ){
		IRam[IEXRAM+i].SetFunc( Stringf( "XRAM%02d", i ), nullptr, FW( STATIC_CAST( WFuncPtr, &MEM62::IERamWrite ) ), MemTable.IntRam->Wait );
	}
	
	return true;
}

bool MEM64::InitIntSpec( void )
{
	PRINTD( MEM_LOG, "[MEM][InitIntSpec]\n" );
	
	// N66-BASIC ROM
	IRom[MAINROM+0].SetMemory ( "SYS1-0", SysRom1( 0 ), MemTable.System1->Wait );
	IRom[MAINROM+1].SetMemory ( "SYS1-1", SysRom1( 1 ), MemTable.System1->Wait );
	IRom[MAINROM+2].SetMemory ( "SYS1-2", SysRom1( 2 ), MemTable.System1->Wait );
	IRom[MAINROM+3].SetMemory ( "SYS1-3", SysRom1( 3 ), MemTable.System1->Wait );
	
	// N66SR-BASIC ROM
	IRom[MAINROM+4].SetMemory ( "SYS1-4", SysRom1( 4 ), MemTable.System1->Wait );
	IRom[MAINROM+5].SetMemory ( "SYS1-5", SysRom1( 5 ), MemTable.System1->Wait );
	IRom[MAINROM+6].SetMemory ( "SYS1-6", SysRom1( 6 ), MemTable.System1->Wait );
	IRom[MAINROM+7].SetMemory ( "SYS1-7", SysRom1( 7 ), MemTable.System1->Wait );
	
	// SR メニューROM
	IRom[SRMENROM+0].SetMemory( "SYS2-0", SysRom2( 0 ), MemTable.System1->Wait );
	IRom[SRMENROM+1].SetMemory( "SYS2-1", SysRom2( 1 ), MemTable.System1->Wait );
	
	// 音声合成ROM
	IRom[VOICEROM+0].SetMemory( "SYS2-2", SysRom2( 2 ), MemTable.System1->Wait );
	IRom[VOICEROM+1].SetMemory( "SYS2-3", SysRom2( 3 ), MemTable.System1->Wait );
	
	// 漢字ROM
	IRom[KANJIROM+0].SetMemory( "SYS2-4", SysRom2( 4 ), MemTable.System1->Wait );
	IRom[KANJIROM+1].SetMemory( "SYS2-5", SysRom2( 5 ), MemTable.System1->Wait );
	IRom[KANJIROM+2].SetMemory( "SYS2-6", SysRom2( 6 ), MemTable.System1->Wait );
	IRom[KANJIROM+3].SetMemory( "SYS2-7", SysRom2( 7 ), MemTable.System1->Wait );
	IRom[KNJROM].SetFunc      ( FN( STATIC_CAST( NFuncPtr, &MEM64::KanjiGetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM64::ReadKanji ) ), nullptr, MemTable.System1->Wait );
	
	// CG ROM
	IRom[CGROM1].SetMemory    ( "CGROM1", CGRom1( 0 ),  MemTable.CGRom1->Wait );
	IRom[CGROM2].SetMemory    ( "CGROM2", CGRom1( 1 ),  MemTable.CGRom1->Wait );
	
	// 内部RAM
	for( int i = 0; i < 8; i++ ){
		IRam[INTRAM+i].SetMemory( Stringf( "IRAM%02d", i ), IntRam( i ), MemTable.IntRam->Wait );
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// リセット
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void MEM6::Reset()
{
	PRINTD( MEM_LOG, "[MEM][Reset]\n" );
	
	CGBank = false;	// CG ROM BANK 無効
	
	// 外部メモリリセット
	ResetExt();
	
	// メモリコントローラ内部レジスタ初期値設定
	Rf = { INIT_RF0, INIT_RF1, INIT_RF2 };
	
	SetMemBlockR( Rf[0], Rf[1] );
	SetMemBlockW( Rf[2] );
}

void MEM62::Reset( void )
{
	MEM6::Reset();
	
	kj_rom   = true;
	kj_LR    = true;
	cgrom    = true;
	cgenable = true;
	cgaden   = 0;
	cgaddr   = 3;
	c2acc    = 0xff;
}

void MEM64::Reset( void )
{
	MEM62::Reset();
	
	// TO DO
	const BYTE initmb[] = { 0xf8, 0xfa, 0xfc, 0xfe, 0x08, 0x0a, 0x0c, 0x0e,
							0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e };
	for( int i = 0; i < 16; i++ ){
		SetMemBlockSR( i, initmb[i] );
	}
}


/////////////////////////////////////////////////////////////////////////////
// フェッチ(M1)
/////////////////////////////////////////////////////////////////////////////
BYTE MEM6::Fetch( WORD addr, int* m1wait ) const
{
	BYTE data = RBLK1[addr >> MemBlock::PAGEBITS]->Read( addr );
	
	PRINTD( MEM_LOG, "[MEM][Fetch] -> %04X:%02X\n", addr, data );
	
	// M1ウェイト追加
	if( m1wait ){ (*m1wait) += M1Wait; }
	
	// バスリクエスト区間実行時ウェイト追加
	if( vm->VdgIsBusReqExec() ){ (*m1wait)++; }
	
	return data;
}

BYTE MEM64::Fetch( WORD addr, int* m1wait ) const
{
	BYTE data = vm->VdgIsSRmode() ? RBLKSR[addr >> MemBlock::PAGEBITS]->Read( addr )
								  : RBLK1 [addr >> MemBlock::PAGEBITS]->Read( addr );
	
	PRINTD( MEM_LOG, "[MEM][Fetch] -> %04X:%02X\n", addr, data );
	
	// M1ウェイト追加
	if( m1wait ){ (*m1wait) += M1Wait; }
	
	// バスリクエスト区間実行時ウェイト追加
	if( vm->VdgIsBusReqExec() ){ (*m1wait)++; }
	
	return data;
}


/////////////////////////////////////////////////////////////////////////////
// メモリリード
/////////////////////////////////////////////////////////////////////////////
BYTE MEM6::Read( WORD addr, int* wcnt ) const
{
	BYTE data = RBLK1[addr >> MemBlock::PAGEBITS]->Read( addr, wcnt );
	
	// バスリクエスト区間実行時ウェイト追加
	if( wcnt && vm->VdgIsBusReqExec() ){ (*wcnt)++; }
	
	PRINTD( MEM_LOG, "[MEM][Read]  -> %04X:%02X\n", addr, data );
	
	return data;
}

BYTE MEM64::Read( WORD addr, int* wcnt ) const
{
	BYTE data = 0xff;
	
	if( vm->VdgIsSRmode() ){
		if( vm->VdgIsSRBitmap( addr ) && (RfSR[addr >> MemBlock::PAGEBITS] == 0) ){	// ビットマップモード(内部RAMアクセス)
			WORD ad = vm->VdgSRGVramAddr( addr );
			data = ((addr & 1) ? (IntRam.Read( ad ) >> 4) : IntRam.Read( ad )) & 0x0f;
		}else{														// 直接アクセスモード
			data = RBLKSR[addr >> MemBlock::PAGEBITS]->Read( addr, wcnt );
		}
	}else{
		data = RBLK1[addr >> MemBlock::PAGEBITS]->Read( addr, wcnt );
	}
	
	// バスリクエスト区間実行時ウェイト追加
	if( wcnt && vm->VdgIsBusReqExec() ){ (*wcnt)++; }
	
	PRINTD( MEM_LOG, "[MEM][Read]  -> %04X:%02X\n", addr, data );
	
	return data;
}


/////////////////////////////////////////////////////////////////////////////
// メモリライト
/////////////////////////////////////////////////////////////////////////////
void MEM6::Write( WORD addr, BYTE data, int* wcnt )
{
	PRINTD( MEM_LOG, "[MEM][Write] %04X:%02X -> %s[%04X]'%c'\n", addr, data, WBLK1[addr >> MemBlock::PAGEBITS]->GetName( addr, true ).c_str(), addr & 0x1fff, data );
	
	WBLK1[addr >> MemBlock::PAGEBITS]->Write( addr, data, wcnt );
	
	// バスリクエスト区間実行時ウェイト追加
	if( wcnt && vm->VdgIsBusReqExec() ){ (*wcnt)++; }
	
	// 内部/外部RAMとも書込みの場合はひとまず内部だけ
}

void MEM64::Write( WORD addr, BYTE data, int* wcnt )
{
	PRINTD( MEM_LOG, "[MEM][Write] %04X:%02X -> %s[%04X]'%c'\n", addr, data, vm->VdgIsSRmode() ? WBLKSR[addr >> MemBlock::PAGEBITS]->GetName( addr, true ).c_str() : WBLK2[addr >> MemBlock::PAGEBITS]->GetName( addr, true ).c_str(), addr & 0x1fff, data );
	
	if( vm->VdgIsSRmode() ){
		if( vm->VdgIsSRBitmap( addr ) && (RfSR[(addr >> MemBlock::PAGEBITS)+8] == 0) ){	// ビットマップモード(内部RAMアクセス)
			WORD ad = vm->VdgSRGVramAddr( addr );
			IntRam.Write( ad, (addr & 1) ? ((IntRam.Read( ad ) & 0x0f) | ((data << 4) & 0xf0)) : ((IntRam.Read( ad ) & 0xf0) | ( data & 0x0f)) );
		}else{															// 直接アクセスモード
			WBLKSR[addr >> MemBlock::PAGEBITS]->Write( addr, data, wcnt );
		}
	}else{
		WBLK1[addr >> MemBlock::PAGEBITS]->Write( addr, data, wcnt );
	}
	
	// バスリクエスト区間実行時ウェイト追加
	if( wcnt && vm->VdgIsBusReqExec() ){ (*wcnt)++; }
	
	// 内部/外部RAMとも書込みの場合はひとまず内部だけ
}


/////////////////////////////////////////////////////////////////////////////
// メモリアクセスウェイト設定
/////////////////////////////////////////////////////////////////////////////
void MEM62::SetWait( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetWait] -> M1:%d ROM:%d RAM:%d\n", (data >> 7) & 1, (data >> 6) & 1, (data >> 5) & 1 );
	
	// M1
	M1Wait = data & 0x80 ? 1 : 0;
	
	// ROM
	std::vector<MemBlock> mbrom = {
		IRom[EMPTYROM],
		IRom[KANJIROM+0], IRom[KANJIROM+1], IRom[KANJIROM+2], IRom[KANJIROM+3], IRom[VOICEROM+0], IRom[VOICEROM+1],
		IRom[MAINROM+4],  IRom[MAINROM+5],  IRom[MAINROM+6],  IRom[MAINROM+7],	IRom[SRMENROM+0], IRom[SRMENROM+1],	// SR
		EMem[EXTROM+0],   EMem[EXTROM+1],   IRom[MAINROM+0],  IRom[MAINROM+1],  IRom[MAINROM+2],  IRom[MAINROM+3]
	};
	for( auto &mb : mbrom ){
		mb.SetWait( data & 0x40 ? 1 : 0 );
	}
	
	// RAM
	std::vector<MemBlock> mbram = {
		IRom[EMPTYRAM],
		IRam[INTRAM+0], IRam[INTRAM+1], IRam[INTRAM+2], IRam[INTRAM+3], IRam[INTRAM+4], IRam[INTRAM+5], IRam[INTRAM+6], IRam[INTRAM+7],
		EMem[EXTRAM+0], EMem[EXTRAM+1], EMem[EXTRAM+2], EMem[EXTRAM+3], EMem[EXTRAM+4], EMem[EXTRAM+5], EMem[EXTRAM+6], EMem[EXTRAM+7]
	};
	for( auto &mb : mbram ){
		mb.SetWait( data & 0x20 ? 1 : 0 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// メモリアクセスウェイト取得
/////////////////////////////////////////////////////////////////////////////
BYTE MEM62::GetWait( void ) const
{
	return ( M1Wait ? 0x80 : 0 ) | ( IRom[EMPTYROM].GetWait() ? 0x40 : 0 ) | ( IRom[EMPTYRAM].GetWait() ? 0x20 : 0 );
}


/////////////////////////////////////////////////////////////////////////////
// メモリリード時のメモリブロック指定
/////////////////////////////////////////////////////////////////////////////
void MEM60::SetMemBlockR( BYTE mem1, BYTE mem2 )
{
	PRINTD( MEM_LOG, "[MEM][SetMemBlockR]\n" );
	
	RBLK2[0] = &IRom[MAINROM+0];	RBLK2[1] = &IRom[MAINROM+1];
	RBLK2[2] = &EMem[EXTROM+0];		RBLK2[3] = &EMem[EXTROM+1];
	RBLK2[4] = &EMem[EXTRAM+4];		RBLK2[5] = &EMem[EXTRAM+5];
	RBLK2[6] = &IRam[INTRAM+0];		RBLK2[7] = &IRam[INTRAM+1];
	
	#if (MEM_LOG)
	for( int i = 0; i < 8; i += 2 ){
		PRINTD( MEM_LOG, "\t%d:%8s\t%d:%8s\n", i, RBLK2[i]->GetName( i << MemBlock::PAGEBITS ).c_str(), i + 1, RBLK2[i + 1]->GetName( (i + 1) << MemBlock::PAGEBITS ).c_str() );
	}
	#endif
}

void MEM62::SetMemBlockR( BYTE mem1, BYTE mem2 )
{
	PRINTD( MEM_LOG, "[MEM][SetMemBlockR] -> %02X %02X\n", mem1, mem2 );
	
	// Port F0H
	switch( mem1 & 0x0f ){	// RF0下位 (0000 - 3FFF)
		case 0x00:	RBLK2[0] = &IRom[EMPTYROM];		RBLK2[1] = &IRom[EMPTYROM];		break;
		case 0x01:	RBLK2[0] = &IRom[MAINROM+0];	RBLK2[1] = &IRom[MAINROM+1];	break;
		case 0x02:	RBLK2[0] = &IRom[KNJROM];		RBLK2[1] = &IRom[KNJROM];		break;
		case 0x03:	RBLK2[0] = &EMem[EXTROM+1];		RBLK2[1] = &EMem[EXTROM+1];		break;
		case 0x04:	RBLK2[0] = &EMem[EXTROM+0];		RBLK2[1] = &EMem[EXTROM+0];		break;
		case 0x05:	RBLK2[0] = &IRom[KNJROM];		RBLK2[1] = &IRom[MAINROM+1];	break;
		case 0x06:	RBLK2[0] = &IRom[MAINROM+0];	RBLK2[1] = &IRom[KNJROM];		break;
		case 0x07:	RBLK2[0] = &EMem[EXTROM+0];		RBLK2[1] = &EMem[EXTROM+1];		break;
		case 0x08:	RBLK2[0] = &EMem[EXTROM+1];		RBLK2[1] = &EMem[EXTROM+0];		break;
		case 0x09:	RBLK2[0] = &EMem[EXTROM+1];		RBLK2[1] = &IRom[MAINROM+1];	break;
		case 0x0a:	RBLK2[0] = &IRom[MAINROM+0];	RBLK2[1] = &EMem[EXTROM+1];		break;
		case 0x0b:	RBLK2[0] = &EMem[EXTROM+0];		RBLK2[1] = &IRom[KNJROM];		break;
		case 0x0c:	RBLK2[0] = &IRom[KNJROM];		RBLK2[1] = &EMem[EXTROM+0];		break;
		case 0x0d:	RBLK2[0] = &IRam[INTRAM+0];		RBLK2[1] = &IRam[INTRAM+1];		break;
		case 0x0e:	RBLK2[0] = &EMem[EXTRAM+0];		RBLK2[1] = &EMem[EXTRAM+1];		break;
		case 0x0f:	RBLK2[0] = &IRom[EMPTYROM];		RBLK2[1] = &IRom[EMPTYROM];		break;
	}
	switch( mem1 & 0xf0 ){	// RF0上位 (4000 - 7FFF)
		case 0x00:	RBLK2[2] = &IRom[EMPTYROM];		RBLK2[3] = &IRom[EMPTYROM];		break;
		case 0x10:	RBLK2[2] = &IRom[MAINROM+2];	RBLK2[3] = &IRom[MAINROM+3];	break;
		case 0x20:	RBLK2[2] = &IRom[KNJROM];		RBLK2[3] = &IRom[KNJROM];		break;
		case 0x30:	RBLK2[2] = &EMem[EXTROM+1];		RBLK2[3] = &EMem[EXTROM+1];		break;
		case 0x40:	RBLK2[2] = &EMem[EXTROM+0];		RBLK2[3] = &EMem[EXTROM+0];		break;
		case 0x50:	RBLK2[2] = &IRom[KNJROM];		RBLK2[3] = &IRom[MAINROM+3];	break;
		case 0x60:	RBLK2[2] = &IRom[MAINROM+2];	RBLK2[3] = &IRom[KNJROM];		break;
		case 0x70:	RBLK2[2] = &EMem[EXTROM+0];		RBLK2[3] = &EMem[EXTROM+1];		break;
		case 0x80:	RBLK2[2] = &EMem[EXTROM+1];		RBLK2[3] = &EMem[EXTROM+0];		break;
		case 0x90:	RBLK2[2] = &EMem[EXTROM+1];		RBLK2[3] = &IRom[MAINROM+3];	break;
		case 0xa0:	RBLK2[2] = &IRom[MAINROM+2];	RBLK2[3] = &EMem[EXTROM+1];		break;
		case 0xb0:	RBLK2[2] = &EMem[EXTROM+0];		RBLK2[3] = &IRom[KNJROM];		break;
		case 0xc0:	RBLK2[2] = &IRom[KNJROM];		RBLK2[3] = &EMem[EXTROM+0];		break;
		case 0xd0:	RBLK2[2] = &IRam[INTRAM+2];		RBLK2[3] = &IRam[INTRAM+3];		break;
		case 0xe0:	RBLK2[2] = &EMem[EXTRAM+2];		RBLK2[3] = &EMem[EXTRAM+3];		break;
		case 0xf0:	RBLK2[2] = &IRom[EMPTYROM];		RBLK2[3] = &IRom[EMPTYROM];		break;
	}
	
	// Port F1H
	switch( mem2 & 0x0f ){	// RF1下位 (8000 - BFFF)
		case 0x00:	RBLK2[4] = &IRom[EMPTYROM];		RBLK2[5] = &IRom[EMPTYROM];		break;
		case 0x01:	RBLK2[4] = &IRom[MAINROM+0];	RBLK2[5] = &IRom[MAINROM+1];	break;
		case 0x02:	RBLK2[4] = &IRom[KNJROM];		RBLK2[5] = &IRom[KNJROM];		break;
		case 0x03:	RBLK2[4] = &EMem[EXTROM+1];		RBLK2[5] = &EMem[EXTROM+1];		break;
		case 0x04:	RBLK2[4] = &EMem[EXTROM+0];		RBLK2[5] = &EMem[EXTROM+0];		break;
		case 0x05:	RBLK2[4] = &IRom[KNJROM];		RBLK2[5] = &IRom[MAINROM+1];	break;
		case 0x06:	RBLK2[4] = &IRom[MAINROM+0];	RBLK2[5] = &IRom[KNJROM];		break;
		case 0x07:	RBLK2[4] = &EMem[EXTROM+0];		RBLK2[5] = &EMem[EXTROM+1];		break;
		case 0x08:	RBLK2[4] = &EMem[EXTROM+1];		RBLK2[5] = &EMem[EXTROM+0];		break;
		case 0x09:	RBLK2[4] = &EMem[EXTROM+1];		RBLK2[5] = &IRom[MAINROM+1];	break;
		case 0x0a:	RBLK2[4] = &IRom[MAINROM+0];	RBLK2[5] = &EMem[EXTROM+1];		break;
		case 0x0b:	RBLK2[4] = &EMem[EXTROM+0];		RBLK2[5] = &IRom[KNJROM];		break;
		case 0x0c:	RBLK2[4] = &IRom[KNJROM];		RBLK2[5] = &EMem[EXTROM+0];		break;
		case 0x0d:	RBLK2[4] = &IRam[INTRAM+4];		RBLK2[5] = &IRam[INTRAM+5];		break;
		case 0x0e:	RBLK2[4] = &EMem[EXTRAM+4];		RBLK2[5] = &EMem[EXTRAM+5];		break;
		case 0x0f:	RBLK2[4] = &IRom[EMPTYROM];		RBLK2[5] = &IRom[EMPTYROM];		break;
	}
	switch( mem2 & 0xf0 ){	// RF1上位 (C000 - FFFF)
		case 0x00:	RBLK2[6] = &IRom[EMPTYROM];		RBLK2[7] = &IRom[EMPTYROM];		break;
		case 0x10:	RBLK2[6] = &IRom[MAINROM+2];	RBLK2[7] = &IRom[MAINROM+3];	break;
		case 0x20:	RBLK2[6] = &IRom[KNJROM];		RBLK2[7] = &IRom[KNJROM];		break;
		case 0x30:	RBLK2[6] = &EMem[EXTROM+1];		RBLK2[7] = &EMem[EXTROM+1];		break;
		case 0x40:	RBLK2[6] = &EMem[EXTROM+0];		RBLK2[7] = &EMem[EXTROM+0];		break;
		case 0x50:	RBLK2[6] = &IRom[KNJROM];		RBLK2[7] = &IRom[MAINROM+3];	break;
		case 0x60:	RBLK2[6] = &IRom[MAINROM+2];	RBLK2[7] = &IRom[KNJROM];		break;
		case 0x70:	RBLK2[6] = &EMem[EXTROM+0];		RBLK2[7] = &EMem[EXTROM+1];		break;
		case 0x80:	RBLK2[6] = &EMem[EXTROM+1];		RBLK2[7] = &EMem[EXTROM+0];		break;
		case 0x90:	RBLK2[6] = &EMem[EXTROM+1];		RBLK2[7] = &IRom[MAINROM+3];	break;
		case 0xa0:	RBLK2[6] = &IRom[MAINROM+2];	RBLK2[7] = &EMem[EXTROM+1];		break;
		case 0xb0:	RBLK2[6] = &EMem[EXTROM+0];		RBLK2[7] = &IRom[KNJROM];		break;
		case 0xc0:	RBLK2[6] = &IRom[KNJROM];		RBLK2[7] = &EMem[EXTROM+0];		break;
		case 0xd0:	RBLK2[6] = &IRam[INTRAM+6];		RBLK2[7] = &IRam[INTRAM+7];		break;
		case 0xe0:	RBLK2[6] = &EMem[EXTRAM+6];		RBLK2[7] = &EMem[EXTRAM+7];		break;
		case 0xf0:	RBLK2[6] = &IRom[EMPTYROM];		RBLK2[7] = &IRom[EMPTYROM];		break;
	}
	
	// 内部レジスタ保存
	Rf[0] = mem1;
	Rf[1] = mem2;
	
	#if (MEM_LOG)
	for( int i = 0; i < 8; i += 2 ){
		PRINTD( MEM_LOG, "\t%d:%8s\t%d:%8s\n", i, RBLK2[i]->GetName( i << MemBlock::PAGEBITS ).c_str(), i + 1, RBLK2[i + 1]->GetName( (i + 1) << MemBlock::PAGEBITS ).c_str() );
	}
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
		case 0x00:	RBLK2[0] = &IRom[EMPTYROM];		RBLK2[1] = &IRom[EMPTYROM];		break;
		case 0x01:	RBLK2[0] = &IRom[MAINROM+0];	RBLK2[1] = &IRom[MAINROM+1];	break;
		case 0x02:	RBLK2[0] = &IRom[KNJROM];		RBLK2[1] = &IRom[KNJROM];		break;	// ココ
		case 0x03:	RBLK2[0] = &EMem[EXTROM+1];		RBLK2[1] = &EMem[EXTROM+1];		break;
		case 0x04:	RBLK2[0] = &EMem[EXTROM+0];		RBLK2[1] = &EMem[EXTROM+0];		break;
		case 0x05:	RBLK2[0] = &IRom[KNJROM];		RBLK2[1] = &IRom[MAINROM+1];	break;	// ココ
		case 0x06:	RBLK2[0] = &IRom[MAINROM+0];	RBLK2[1] = &IRom[KNJROM];		break;	// ココ
		case 0x07:	RBLK2[0] = &EMem[EXTROM+0];		RBLK2[1] = &EMem[EXTROM+1];		break;
		case 0x08:	RBLK2[0] = &EMem[EXTROM+1];		RBLK2[1] = &EMem[EXTROM+0];		break;
		case 0x09:	RBLK2[0] = &EMem[EXTROM+1];		RBLK2[1] = &IRom[MAINROM+1];	break;
		case 0x0a:	RBLK2[0] = &IRom[MAINROM+0];	RBLK2[1] = &EMem[EXTROM+1];		break;
		case 0x0b:	RBLK2[0] = &EMem[EXTROM+0];		RBLK2[1] = &IRom[KNJROM];		break;	// ココ
		case 0x0c:	RBLK2[0] = &IRom[KNJROM];		RBLK2[1] = &EMem[EXTROM+0];		break;	// ココ
		case 0x0d:	RBLK2[0] = &IRam[INTRAM+0];		RBLK2[1] = &IRam[INTRAM+1];		break;
		case 0x0e:	RBLK2[0] = &EMem[EXTRAM+0];		RBLK2[1] = &EMem[EXTRAM+1];		break;
		case 0x0f:	RBLK2[0] = &IRom[EMPTYROM];		RBLK2[1] = &IRom[EMPTYROM];		break;
	}
	switch( mem1 & 0xf0 ){	// RF0上位 (4000 - 7FFF)
		case 0x00:	RBLK2[2] = &IRom[EMPTYROM];		RBLK2[3] = &IRom[EMPTYROM];		break;
		case 0x10:	RBLK2[2] = &IRom[MAINROM+2];	RBLK2[3] = &IRom[MAINROM+3];	break;
		case 0x20:	RBLK2[2] = &IRom[KNJROM];		RBLK2[3] = &IRom[KNJROM];		break;
		case 0x30:	RBLK2[2] = &EMem[EXTROM+1];		RBLK2[3] = &EMem[EXTROM+1];		break;
		case 0x40:	RBLK2[2] = &EMem[EXTROM+0];		RBLK2[3] = &EMem[EXTROM+0];		break;
		case 0x50:	RBLK2[2] = &IRom[KNJROM];		RBLK2[3] = &IRom[MAINROM+3];	break;
		case 0x60:	RBLK2[2] = &IRom[MAINROM+2];	RBLK2[3] = &IRom[KNJROM];		break;
		case 0x70:	RBLK2[2] = &EMem[EXTROM+0];		RBLK2[3] = &EMem[EXTROM+1];		break;
		case 0x80:	RBLK2[2] = &EMem[EXTROM+1];		RBLK2[3] = &EMem[EXTROM+0];		break;
		case 0x90:	RBLK2[2] = &EMem[EXTROM+1];		RBLK2[3] = &IRom[MAINROM+3];	break;
		case 0xa0:	RBLK2[2] = &IRom[MAINROM+2];	RBLK2[3] = &EMem[EXTROM+1];		break;
		case 0xb0:	RBLK2[2] = &EMem[EXTROM+0];		RBLK2[3] = &IRom[KNJROM];		break;
		case 0xc0:	RBLK2[2] = &IRom[KNJROM];		RBLK2[3] = &EMem[EXTROM+0];		break;
		case 0xd0:	RBLK2[2] = &IRam[INTRAM+2];		RBLK2[3] = &IRam[INTRAM+3];		break;
		case 0xe0:	RBLK2[2] = &EMem[EXTRAM+2];		RBLK2[3] = &EMem[EXTRAM+3];		break;
		case 0xf0:	RBLK2[2] = &IRom[EMPTYROM];		RBLK2[3] = &IRom[EMPTYROM];		break;
	}
	
	// Port F1H
	switch( mem2 & 0x0f ){	// RF1下位 (8000 - BFFF)
		case 0x00:	RBLK2[4] = &IRom[EMPTYROM];		RBLK2[5] = &IRom[EMPTYROM];		break;
		case 0x01:	RBLK2[4] = &IRom[MAINROM+0];	RBLK2[5] = &IRom[MAINROM+1];	break;
		case 0x02:	RBLK2[4] = &IRom[KNJROM];		RBLK2[5] = &IRom[KNJROM];		break;
		case 0x03:	RBLK2[4] = &EMem[EXTROM+1];		RBLK2[5] = &EMem[EXTROM+1];		break;
		case 0x04:	RBLK2[4] = &EMem[EXTROM+0];		RBLK2[5] = &EMem[EXTROM+0];		break;
		case 0x05:	RBLK2[4] = &IRom[KNJROM];		RBLK2[5] = &IRom[MAINROM+1];	break;
		case 0x06:	RBLK2[4] = &IRom[MAINROM+0];	RBLK2[5] = &IRom[KNJROM];		break;
		case 0x07:	RBLK2[4] = &EMem[EXTROM+0];		RBLK2[5] = &EMem[EXTROM+1];		break;
		case 0x08:	RBLK2[4] = &EMem[EXTROM+1];		RBLK2[5] = &EMem[EXTROM+0];		break;
		case 0x09:	RBLK2[4] = &EMem[EXTROM+1];		RBLK2[5] = &IRom[MAINROM+1];	break;
		case 0x0a:	RBLK2[4] = &IRom[MAINROM+0];	RBLK2[5] = &EMem[EXTROM+1];		break;
		case 0x0b:	RBLK2[4] = &EMem[EXTROM+0];		RBLK2[5] = &IRom[KNJROM];		break;
		case 0x0c:	RBLK2[4] = &IRom[KNJROM];		RBLK2[5] = &EMem[EXTROM+0];		break;
		case 0x0d:	RBLK2[4] = &IRam[INTRAM+4];		RBLK2[5] = &IRam[INTRAM+5];		break;
		case 0x0e:	RBLK2[4] = &EMem[EXTRAM+4];		RBLK2[5] = &EMem[EXTRAM+5];		break;
		case 0x0f:	RBLK2[4] = &IRom[EMPTYROM];		RBLK2[5] = &IRom[EMPTYROM];		break;
	}
	switch( mem2 & 0xf0 ){	// RF1上位 (C000 - FFFF)
		case 0x00:	RBLK2[6] = &IRom[EMPTYROM];		RBLK2[7] = &IRom[EMPTYROM];		break;
		case 0x10:	RBLK2[6] = &IRom[MAINROM+2];	RBLK2[7] = &IRom[MAINROM+3];	break;
		case 0x20:	RBLK2[6] = &IRom[KNJROM];		RBLK2[7] = &IRom[KNJROM];		break;
		case 0x30:	RBLK2[6] = &EMem[EXTROM+1];		RBLK2[7] = &EMem[EXTROM+1];		break;
		case 0x40:	RBLK2[6] = &EMem[EXTROM+0];		RBLK2[7] = &EMem[EXTROM+0];		break;
		case 0x50:	RBLK2[6] = &IRom[KNJROM];		RBLK2[7] = &IRom[MAINROM+3];	break;
		case 0x60:	RBLK2[6] = &IRom[MAINROM+2];	RBLK2[7] = &IRom[KNJROM];		break;
		case 0x70:	RBLK2[6] = &EMem[EXTROM+0];		RBLK2[7] = &EMem[EXTROM+1];		break;
		case 0x80:	RBLK2[6] = &EMem[EXTROM+1];		RBLK2[7] = &EMem[EXTROM+0];		break;
		case 0x90:	RBLK2[6] = &EMem[EXTROM+1];		RBLK2[7] = &IRom[MAINROM+3];	break;
		case 0xa0:	RBLK2[6] = &IRom[MAINROM+2];	RBLK2[7] = &EMem[EXTROM+1];		break;
		case 0xb0:	RBLK2[6] = &EMem[EXTROM+0];		RBLK2[7] = &IRom[KNJROM];		break;
		case 0xc0:	RBLK2[6] = &IRom[KNJROM];		RBLK2[7] = &EMem[EXTROM+0];		break;
		case 0xd0:	RBLK2[6] = &IRam[INTRAM+6];		RBLK2[7] = &IRam[INTRAM+7];		break;
		case 0xe0:	RBLK2[6] = &EMem[EXTRAM+6];		RBLK2[7] = &EMem[EXTRAM+7];		break;
		case 0xf0:	RBLK2[6] = &IRom[EMPTYROM];		RBLK2[7] = &IRom[EMPTYROM];		break;
	}
	
	// 内部レジスタ保存
	Rf[0] = mem1;
	Rf[1] = mem2;
	
	#if (MEM_LOG)
	for( int i = 0; i < 8; i += 2 ){
		PRINTD( MEM_LOG, "\t%d:%8s\t%d:%8s\n", i, RBLK2[i]->GetName( i << MemBlock::PAGEBITS ).c_str(), i + 1, RBLK2[i + 1]->GetName( (i + 1) << MemBlock::PAGEBITS ).c_str() );
	}
	#endif
}


/////////////////////////////////////////////////////////////////////////////
// メモリライト時のメモリブロック指定
/////////////////////////////////////////////////////////////////////////////
void MEM60::SetMemBlockW( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetMemBlockW]\n" );
	
	WBLK2[0] = &IRom[EMPTYROM];	WBLK2[1] = &IRom[EMPTYROM];
	WBLK2[2] = &IRom[EMPTYROM];	WBLK2[3] = &IRom[EMPTYROM];
	WBLK2[4] = &EMem[EXTRAM+4];	WBLK2[5] = &EMem[EXTRAM+5];
	WBLK2[6] = &IRam[INTRAM+0];	WBLK2[7] = &IRam[INTRAM+1];
	
	#if (MEM_LOG)
	for( int i = 0; i < 8; i += 2 ){
		PRINTD( MEM_LOG, "\t%d:%8s\t%d:%8s\n", i, WBLK2[i]->GetName( i << MemBlock::PAGEBITS, true ).c_str(), i + 1, WBLK2[i + 1]->GetName( (i + 1) << MemBlock::PAGEBITS, true ).c_str() );
	}
	#endif
}

void MEM62::SetMemBlockW( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetMemBlockW] -> %02X\n", data );
	
	switch( data & 3 ){			// 0000 - 3FFF
		case 0: WBLK2[0] = &IRom[EMPTYRAM];	WBLK2[1] = &IRom[EMPTYRAM];	break;
		case 1: WBLK2[0] = &IRam[INTRAM+0];	WBLK2[1] = &IRam[INTRAM+1];	break;
		case 2: WBLK2[0] = &EMem[EXTRAM+0];	WBLK2[1] = &EMem[EXTRAM+1];	break;
		case 3: WBLK2[0] = &IRam[IEXRAM+0];	WBLK2[1] = &IRam[IEXRAM+1];	break;
	}
	switch( (data >> 2) & 3 ){	// 4000 - 7FFF
		case 0: WBLK2[2] = &IRom[EMPTYRAM];	WBLK2[3] = &IRom[EMPTYRAM];	break;
		case 1: WBLK2[2] = &IRam[INTRAM+2];	WBLK2[3] = &IRam[INTRAM+3];	break;
		case 2: WBLK2[2] = &EMem[EXTRAM+2];	WBLK2[3] = &EMem[EXTRAM+3];	break;
		case 3: WBLK2[2] = &IRam[IEXRAM+2];	WBLK2[3] = &IRam[IEXRAM+3];	break;
	}
	switch( (data >> 4) & 3 ){	// 8000 - BFFF
		case 0: WBLK2[4] = &IRom[EMPTYRAM];	WBLK2[5] = &IRom[EMPTYRAM];	break;
		case 1: WBLK2[4] = &IRam[INTRAM+4];	WBLK2[5] = &IRam[INTRAM+5];	break;
		case 2: WBLK2[4] = &EMem[EXTRAM+4];	WBLK2[5] = &EMem[EXTRAM+5];	break;
		case 3: WBLK2[4] = &IRam[IEXRAM+4];	WBLK2[5] = &IRam[IEXRAM+5];	break;
	}
	switch( (data >> 6) & 3 ){	// C000 - FFFF
		case 0: WBLK2[6] = &IRom[EMPTYRAM];	WBLK2[7] = &IRom[EMPTYRAM];	break;
		case 1: WBLK2[6] = &IRam[INTRAM+6];	WBLK2[7] = &IRam[INTRAM+7];	break;
		case 2: WBLK2[6] = &EMem[EXTRAM+6];	WBLK2[7] = &EMem[EXTRAM+7];	break;
		case 3: WBLK2[6] = &IRam[IEXRAM+6];	WBLK2[7] = &IRam[IEXRAM+7];	break;
	}
	
	// 内部レジスタ保存
	Rf[2] = data;
	
	#if (MEM_LOG)
	for( int i = 0; i < 8; i += 2 ){
		PRINTD( MEM_LOG, "\t%d:%8s\t%d:%8s\n", i, WBLK2[i]->GetName( i << MemBlock::PAGEBITS, true ).c_str(), i + 1, WBLK2[i + 1]->GetName( (i + 1) << MemBlock::PAGEBITS, true ).c_str() );
	}
	#endif
}


/////////////////////////////////////////////////////////////////////////////
// メモリリード/ライト時のメモリブロック指定(64,68)
/////////////////////////////////////////////////////////////////////////////
void MEM64::SetMemBlockSR( BYTE port, BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetMemBlockSR] -> Port:%02X Data:%02X\n", port, data );
	
	BYTE cs   = data >> 4;
	BYTE addr = (data >> 1) & 0x07;
	MemBlock** mb;
	
	RfSR[port & 0x0f] = data;
	if( port & 0x08 ) mb = &WBLKSR[port & 0x07];	// 8-F : Write
	else			  mb = &RBLKSR[port & 0x07];	// 0-7 : Read
	
	switch( cs ){
	case 0x00:	// System RAM (16KB単位でしか設定できない)
		*mb = &IRam[INTRAM + (addr & 0x06) + (port & 1)];
		break;
		
	case 0x02:	// Ext RAM (こっちも16KB単位とすべき? 検証必要)
		*mb = &EMem[EXTRAM + addr]; break;
		break;
		
	case 0x0b:	// Ext ROM1
		*mb = &EMem[EXTROM+1];
		break;
		
	case 0x0c:	// Ext ROM2
		*mb = &EMem[EXTROM+0];
		break;
		
	case 0x0d:	// CGROM
		switch( addr & 0x01 ){
		case 0x00: *mb = &IRom[CGROM1]; break;
		case 0x01: *mb = &IRom[CGROM2]; break;
		}
		break;
		
	case 0x0e:	// System Rom2
		switch( addr ){
		case 0x00: *mb = &IRom[SRMENROM+0]; break;
		case 0x01: *mb = &IRom[SRMENROM+1]; break;
		case 0x02: *mb = &IRom[VOICEROM+0]; break;
		case 0x03: *mb = &IRom[VOICEROM+1]; break;
		case 0x04: *mb = &IRom[KANJIROM+0]; break;
		case 0x05: *mb = &IRom[KANJIROM+1]; break;
		case 0x06: *mb = &IRom[KANJIROM+2]; break;
		case 0x07: *mb = &IRom[KANJIROM+3]; break;
		}
		break;
		
	case 0x0f:	// System Rom1
		*mb = &IRom[MAINROM + addr];
		break;
		
	default:
		*mb = &IRom[EMPTYROM];
	}
	
	#if (MEM_LOG)
	PRINTD( MEM_LOG, "              [Read]\t\t[Write]\n" );
	{	int i = 0;
		for( auto &rb : RBLKSR ){
			PRINTD( MEM_LOG, "               %d:%8s\t%8s\n", i, rb->GetName( i << MemBlock::PAGEBITS, true ).c_str(), rb->GetName( (i + 1) << MemBlock::PAGEBITS, true ).c_str() );
			i++;
		}
	}
	#endif
}


/////////////////////////////////////////////////////////////////////////////
// CG ROM アドレス設定
/////////////////////////////////////////////////////////////////////////////
void MEM62::SetCGrom( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetCGrom] -> %02x\n", data );
	
	// bit 7
	int	cgwait = data & 0x80 ? 0 : 1;
	IRom[CGROM1].SetWait( cgwait );
	IRom[CGROM2].SetWait( cgwait );
	
	// bit 6 CG ROMアクセスフラグ true:アクセス可 false:アクセス不可
	cgenable = data & 0x40 ? true : false;
	
	// bit 5,4,3 CG ROMアドレスイネーブル 0:ON 1:OFF
	// 無効ビットは1固定??
	cgaden   = (data >> 3) & 7;
	
	// bit 2,1,0 CG ROMアドレス A13,14,15
	// 予めcgadenでマスク(OR)しておく
	cgaddr   = (data & 7) | cgaden;
}

void MEM64::SetCGrom( BYTE data )
{
	MEM62::SetCGrom( data );
	
	// CGROMは16KB単位でしか割当てられないらしい(内部RAMと同じ)
	// SR 20行モードのフォントはCGROMの0x2000H-0x3FFFH
	// C2H(0x4000H)を指示してもC3H(0x6000H)と同じ挙動になる
	// 日本語ワードプロセッサの挙動だけで判断
	cgaden |= 1;
	cgaddr |= 1;
}


/*
	http://sbeach.seesaa.net/article/459909631.html

注）通常は8K毎に割り当てが出来ますが、内部RAM（VRAM）とCGROMには、制限があります。
　　（基板接続間違いのようです）

　　内部RAM（VRAM）、CGROMの	0x0000～0x1FFFは、									0x0000～0x1FFF、0x4000～0x5FFF、0x8000～0x9FFF、0xC000～0xDFFFにしか割り当てられない。
　　内部RAM（VRAM）、CGROMの	0x2000～0x3FFFは、									0x2000～0x3FFF、0x6000～0x7FFF、0xA000～0xBFFF、0xE000～0xEFFFにしか割り当てられない。
　　内部RAM（VRAM）の			0x4000～0x5FFF、0x8000～0x9FFF、0xC000～0xDFFFは、	0x0000～0x1FFF、0x4000～0x5FFF、0x8000～0x9FFF、0xC000～0xDFFFにしか割り当てられない。
　　内部RAM（VRAM）の			0x6000～0x7FFF、0xA000～0xBFFF、0xE000～0xEFFFは、	0x2000～0x3FFF、0x6000～0x7FFF、0xA000～0xBFFF、0xE000～0xEFFFにしか割り当てられない。
*/


/////////////////////////////////////////////////////////////////////////////
// CG ROM 選択
/////////////////////////////////////////////////////////////////////////////
void MEM62::SelectCGrom( bool mode )
{
	PRINTD( MEM_LOG, "[MEM][SelectCGrom] -> %s\n", mode ? "1:32*16(N60)" : "0:40*20(N60m)" );
	
	// mode 1:32*16(N60モード) 0:40*20(N60mモード)
	cgrom = mode;
}


/////////////////////////////////////////////////////////////////////////////
// CG ROM BANK 選択
/////////////////////////////////////////////////////////////////////////////
void MEM6::SetCGBank( bool data )
{
	PRINTD( MEM_LOG, "[MEM][SetCGBank] -> %s\n", data ? "Enable" : "Disable" );
	
	CGBank = data;
}


/////////////////////////////////////////////////////////////////////////////
// 漢字ROMおよび音声合成ROM設定
/////////////////////////////////////////////////////////////////////////////
void MEM62::SetKanjiRom( BYTE mode )
{
	PRINTD( MEM_LOG, "[MEM][SetKanjiRom] -> %02X\n", mode );
	
	// mode bit0 0:音声合成ROM選択 1:漢字ROM選択
	//      bit1 0:漢字ROM左側     1:漢字ROM右側
	if( c2acc & 2 ){ kj_LR  = mode & 2 ? true : false; }	// 漢字 左？右？
	if( c2acc & 1 ){ kj_rom = mode & 1 ? true : false; }	// 漢字ROM？音声合成ROM？
}


/////////////////////////////////////////////////////////////////////////////
// 漢字ROMおよび音声合成ROM取得
/////////////////////////////////////////////////////////////////////////////
BYTE MEM62::GetKanjiRom( void ) const
{
	return 0xfc | kj_LR ? 2 : 0 | kj_rom ? 1 : 0;
}


/////////////////////////////////////////////////////////////////////////////
// PortC2Hアクセス設定
/////////////////////////////////////////////////////////////////////////////
void MEM62::SetPortC2HAccess( BYTE data )
{
	PRINTD( MEM_LOG, "[MEM][SetPortC2HAccess] -> %02X\n", data );
	
	// 0:入力 1:出力
	c2acc = data;
}


/////////////////////////////////////////////////////////////////////////////
// 直接アクセス関数
/////////////////////////////////////////////////////////////////////////////
BYTE MEM6::ReadSysRom   ( WORD addr ) const { return SysRom1.Read ( addr ); }
BYTE MEM6::ReadIntRam   ( WORD addr ) const { return IntRam.Read  ( addr ); }

BYTE MEM6::ReadCGrom1   ( WORD addr ) const { return CGRom1.Read  ( addr & 0x1fff ); }
BYTE MEM6::ReadCGrom2   ( WORD )      const { return 0xff; }
BYTE MEM6::ReadCGrom3   ( WORD )      const { return 0xff; }

BYTE MEM62::ReadCGrom2  ( WORD addr ) const { return CGRom2.Read  ( addr ); }
BYTE MEM62::ReadKanjiRom( WORD addr ) const { return KanjiRom.Read( addr ); }
BYTE MEM62::ReadVoiceRom( WORD addr ) const { return VoiceRom.Read( addr ); }

BYTE MEM64::ReadCGrom1  ( WORD addr ) const { return CGRom1.Read  (  addr & 0x0fff           ); }
BYTE MEM64::ReadCGrom2  ( WORD addr ) const { return CGRom1.Read  ( (addr & 0x1fff) + 0x2000 ); }
BYTE MEM64::ReadCGrom3  ( WORD addr ) const { return CGRom1.Read  ( (addr & 0x0fff) + 0x1000 ); }
BYTE MEM64::ReadKanjiRom( WORD addr ) const { return SysRom2.Read ( (addr & 0x7fff) + 0x8000 ); }
BYTE MEM64::ReadVoiceRom( WORD addr ) const { return SysRom2.Read ( (addr & 0x3fff) + 0x4000 ); }


/////////////////////////////////////////////////////////////////////////////
// I/Oアクセス関数
/////////////////////////////////////////////////////////////////////////////
void MEM62::OutC1H( int, BYTE data ){ SelectCGrom( (data & 2) ); }
void MEM62::OutC2H( int, BYTE data ){ SetKanjiRom( data ); }
void MEM62::OutC3H( int, BYTE data ){ SetPortC2HAccess( data ); }
void MEM62::OutF0H( int, BYTE data ){ SetMemBlockR( data, Rf[1] ); }
void MEM62::OutF1H( int, BYTE data ){ SetMemBlockR( Rf[0], data ); }
void MEM62::OutF2H( int, BYTE data ){ SetMemBlockW( data ); }
void MEM62::OutF3H( int, BYTE data ){ SetWait( data ); }
void MEM62::OutF8H( int, BYTE data ){ SetCGrom( data ); }

void MEM64::Out6xH( int port, BYTE data ){ SetMemBlockSR( port, data ); }

BYTE MEM62::InC2H( int ){ return GetKanjiRom(); }
BYTE MEM62::InF0H( int ){ return Rf[0]; }
BYTE MEM62::InF1H( int ){ return Rf[1]; }
BYTE MEM62::InF2H( int ){ return Rf[2]; }
BYTE MEM62::InF3H( int ){ return GetWait() | 0x1f; }

BYTE MEM64::In6xH( int port ){ return RfSR[port & 0x0f]; }
BYTE MEM64::InB2H( int port ){ return 0xfd; }	// bit1 0:mk2SR 1:66SR
BYTE MEM68::InB2H( int port ){ return 0xff; }	// bit1 0:mk2SR 1:66SR














// 拡張カートリッジ ============================================

// 戦士のカートリッジ バンク分類
#define ROMBANK		(0x00)
#define RAMBANK		(0x40)
#define SCCBANK		(0x80)
#define NONBANK		(0xc0)

#define ROMBANKH	(0x80)	// mkⅢ




/////////////////////////////////////////////////////////////////////////////
// 拡張カートリッジマウント
//
// 引数:	cart		カートリッジの種類
//			filepath	SystemROMフォルダパス名への参照
//			crc			CRCチェック true:する false:しない
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM6::MountExtCart( WORD cart, const P6VPATH& path, bool crc  )
{
	PRINTD( MEM_LOG, "[MEM][MountExtCart]\n" );
	
	ExCart = cart;			// 拡張カートリッジ
	
	// 外部メモリ確保とSystemROMファイル読込み
	// ファイルが見つからなくても継続
	if( !AllocMemoryExt( path, crc ) && Error::GetError() != Error::NoRom ){
		return false;
	}
	
	// 外部メモリ初期化
	if( !InitExt() ){
		return false;
	}
	
	// 拡張カートリッジ デバイスディスクリプタ追加
	AddDeviceDescriptorExt();
	
	// リセット
	Reset();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// メモリブロック用関数
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ/Ⅲ メモリブロック名取得
/////////////////////////////////////////////////////////////////////////////
const std::string& MEM6::Sol2GetName( WORD addr, bool rw )
{
	int bank  = SolBank[addr >> MemBlock::PAGEBITS];
	int bank3 = (bank & 0x3f) | ((bank & 0x80) >> 1) | ((bank & 0x40) << 1);
	static std::string name = "";
	
	switch( ExCart ){
	case EXCSOL1:	// 戦士のカートリッジ
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		switch( bank & 0xc0 ){
		case ROMBANK:	// ROM
			name = Stringf( "EROM%02d", (SolBankSet | (bank & 0x0f)) & (ExtRom.Size() - 1) );
			break;
			
		case RAMBANK:	// RAM
			name = Stringf( "ERAM%02d", (bank & 0x0f) & (ExtRam.Size() - 1) );
			break;
			
		case SCCBANK:	// SCC
			name = "SCC";
			break;
			
		case NONBANK:	// 無効
		default:
			name = "EMPTY";
		}
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		switch( bank & 0xc0 ){
		case ROMBANK:	// ROM L
		case ROMBANKH:	// ROM H
			name = Stringf( "EROM%03d", ((SolBankSet << 7) | (bank3 & 0x7f)) & (ExtRom.Size() - 1) );
			break;
			
		case RAMBANK:	// RAM
			name = Stringf( "ERAM%03d", (bank3 & 0x3f) & (ExtRam.Size() - 1) );
			break;
			
		case NONBANK:	// 無効
		default:
			name = "EMPTY";
		}
		break;
	}
	
	return name;
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ/Ⅲ 読込み
/////////////////////////////////////////////////////////////////////////////
BYTE MEM6::Sol2Read( MemCell* ptr, WORD addr, int* wcnt )
{
	int bank  = SolBank[addr >> MemBlock::PAGEBITS];
	int bank3 = (bank & 0x3f) | ((bank & 0x80) >> 1) | ((bank & 0x40) << 1);
	
	switch( ExCart ){
	case EXCSOL1:	// 戦士のカートリッジ
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		switch( bank & 0xc0 ){
		case ROMBANK:	// ROM
			return ExtRom( (SolBankSet | (bank & 0x0f)) & (ExtRom.Size() - 1) ).Read( addr );
			
		case RAMBANK:	// RAM
			return ExtRam( (bank & 0x0f) & (ExtRam.Size() - 1) ).Read( addr );
			
		case SCCBANK:	// SCC
			return Sol2SccRead( ptr, addr, wcnt );
		}
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		switch( bank & 0xc0 ){
		case ROMBANK:	// ROM L
		case ROMBANKH:	// ROM H
			return ExtRom( ((SolBankSet << 7) | (bank3 & 0x7f)) & (ExtRom.Size() - 1) ).Read( addr );
			
		case RAMBANK:	// RAM
			return ExtRam( (bank3 & 0x3f) & (ExtRam.Size() - 1) ).Read( addr );
		}
		break;
	}
	
	return 0xff;
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ/Ⅲ 書込み
/////////////////////////////////////////////////////////////////////////////
void MEM6::Sol2Write( MemCell* ptr, WORD addr, BYTE data, int* wcnt )
{
	int bank  = SolBank[addr >> MemBlock::PAGEBITS];
	int bank3 = (bank & 0x3f) | ((bank & 0x80) >> 1) | ((bank & 0x40) << 1);
	
	switch( ExCart ){
	case EXCSOL1:	// 戦士のカートリッジ
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		switch( bank & 0xc0 ){
		case RAMBANK:	// RAM
			ExtRam( (bank & 0x0f) & (ExtRam.Size() - 1) ).Write( addr, data );
			break;
			
		case SCCBANK:	// SCC
			Sol2SccWrite( ptr, addr, data, wcnt );
			break;
		}
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		switch( bank & 0xc0 ){
		case RAMBANK:	// RAM
			ExtRam( (bank3 & 0x3f) & (ExtRam.Size() - 1) ).Write( addr, data );
			break;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ SCC読込み
/////////////////////////////////////////////////////////////////////////////
BYTE MEM6::Sol2SccRead( MemCell* ptr, WORD addr, int* wcnt )
{
	// 後で書く
	return 0xff;
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ SCC書込み
/////////////////////////////////////////////////////////////////////////////
void MEM6::Sol2SccWrite( MemCell* ptr, WORD addr, BYTE data, int* wcnt )
{
	// 後で書く
}

/////////////////////////////////////////////////////////////////////////////
// メモリブロック用関数 ここまで
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// 拡張カートリッジ デバイスディスクリプタ追加
/////////////////////////////////////////////////////////////////////////////
void MEM6::AddDeviceDescriptorExt( void )
{
	switch( ExCart ){
	case EXC660101:	// 拡張漢字ROMカートリッジ
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		// Device Description (Out)
		descs.outdef.emplace( outFCH,  STATIC_CAST( Device::OutFuncPtr, &MEM6::OutFCH  ) );
		descs.outdef.emplace( outFFH,  STATIC_CAST( Device::OutFuncPtr, &MEM6::OutFFH  ) );
		
		// Device Description (In)
		descs.indef.emplace ( inFDH,   STATIC_CAST( Device::InFuncPtr,  &MEM6::InFDH   ) );
		descs.indef.emplace ( inFEH,   STATIC_CAST( Device::InFuncPtr,  &MEM6::InFEH   ) );
		break;
		
	case EXC6053:	// ボイスシンセサイザー
		// Device Description (Out)
		descs.outdef.emplace( out70H,  STATIC_CAST( Device::OutFuncPtr, &MEM6::Out70H  ) );
		descs.outdef.emplace( out72H,  STATIC_CAST( Device::OutFuncPtr, &MEM6::Out72H  ) );
		descs.outdef.emplace( out73H,  STATIC_CAST( Device::OutFuncPtr, &MEM6::Out73H  ) );
		descs.outdef.emplace( out74H,  STATIC_CAST( Device::OutFuncPtr, &MEM6::Out74H  ) );
		
		// Device Description (In)
		descs.indef.emplace ( in70H,   STATIC_CAST( Device::InFuncPtr,  &MEM6::In70H   ) );
		descs.indef.emplace ( in72H,   STATIC_CAST( Device::InFuncPtr,  &MEM6::In72H   ) );
		descs.indef.emplace ( in73H,   STATIC_CAST( Device::InFuncPtr,  &MEM6::In73H   ) );
		break;
		
	case EXC60M55:	// FM音源カートリッジ
		// Device Description (Out)
		descs.outdef.emplace( out70Hf, STATIC_CAST( Device::OutFuncPtr, &MEM6::Out70Hf ) );
		descs.outdef.emplace( out71Hf, STATIC_CAST( Device::OutFuncPtr, &MEM6::Out71Hf ) );
		
		// Device Description (In)
		descs.indef.emplace ( in72Hf,  STATIC_CAST( Device::InFuncPtr,  &MEM6::In72Hf  ) );
		descs.indef.emplace ( in73Hf,  STATIC_CAST( Device::InFuncPtr,  &MEM6::In73Hf  ) );
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		// Device Description (Out)
		descs.outdef.emplace( out07H,  STATIC_CAST( Device::OutFuncPtr, &MEM6::Out07H  ) );
		[[fallthrough]];
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		// Device Description (Out)
		descs.outdef.emplace( out06H,  STATIC_CAST( Device::OutFuncPtr, &MEM6::Out06H  ) );
		descs.outdef.emplace( out3xH,  STATIC_CAST( Device::OutFuncPtr, &MEM6::Out3xH  ) );
		descs.outdef.emplace( outF0Hs, STATIC_CAST( Device::OutFuncPtr, &MEM6::OutF0Hs ) );	// 戦士のカートリッジ 60対応
		descs.outdef.emplace( outF2Hs, STATIC_CAST( Device::OutFuncPtr, &MEM6::OutF2Hs ) );	// 戦士のカートリッジ 60対応
		[[fallthrough]];
		
	case EXCSOL1:	// 戦士のカートリッジ
		// Device Description (Out)
		descs.outdef.emplace( out7FH,  STATIC_CAST( Device::OutFuncPtr, &MEM6::Out7FH  ) );
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ メモリバンクレジスタ設定
//
//  [ROMの場合]
//    bit7   : 0
//    bit6   : 0
//    bit5-4 : Bank Set No. (0-3)
//    bit3-0 : Bank No. (0-15)
//  [RAMの場合]
//    bit7   : 0
//    bit6   : 1
//    bit5-4 : 未使用
//    bit3-0 : Bank No. (0-15)
//  [SCCの場合]
//    bit7   : 1
//    bit6   : 0
//    bit5-3 : 未使用
//    bit2   : 必ず1にする(AA15に出力される)
//    bit1   : 必ず0にする(AA14に出力される)
//    bit0   : 0:互換モード 1:SCC-Iモード
//  [無効の場合]
//    bit7   : 1
//    bit6   : 1
//    bit5-0 : 未使用
/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅢ メモリバンクレジスタ設定
//
//  [ROMの場合]
//    bit7   : 0
//    bit6   : 0
//    bit5-0 : Bank No. (0-63)
//  [RAMの場合]
//    bit7   : 0
//    bit6   : 1
//    bit5-0 : Bank No. (0-63)
//  [ROMの場合]
//    bit7   : 1
//    bit6   : 0
//    bit5-0 : Bank No. (64-127)
//  [無効の場合]
//    bit7   : 1
//    bit6   : 1
//    bit5-0 : 未使用
/////////////////////////////////////////////////////////////////////////////
void MEM6::SetSol2Bank( BYTE port, BYTE data )
{
	SolBank[port & 7] = data;
	
	switch( ExCart ){
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		if( (data & 0xc0) == ROMBANK ){
			SolBankSet = data & 0x30;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 外部メモリ確保とSystemROMファイル読込み
//
// 引数:	path		SystemROMフォルダパス名への参照
//			crc			CRCチェック true:する false:しない
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM6::AllocMemoryExt( const P6VPATH& path, bool crc )
{
	PRINTD( MEM_LOG, "[MEM][AllocMemoryExt]\n" );
	
	switch( ExCart ){
	case EXC6001:	// 拡張BASIC
		// ROM:8KB RAM:-
		MemTable.ExtRom = &MEM6::IEXTROM8;
		MemTable.ExtRam = &MEM6::IEMPTRAM;
		break;
		
	case EXC6005:	// ROMカートリッジ
		// ROM:16KB RAM:-
		MemTable.ExtRom = &MEM6::IEXTROM16;
		MemTable.ExtRam = &MEM6::IEMPTRAM;
		break;
		
	case EXC6006:	// 拡張ROM/RAMカートリッジ
		// ROM:16KB RAM:16KB
		MemTable.ExtRom = &MEM6::IEXTROM16;
		MemTable.ExtRam = &MEM6::IEXTRAM16;
		break;
		
	case EXC660101:	// 拡張漢字ROMカートリッジ
		// ROM:128KB RAM:-
		MemTable.ExtRom = &MEM6::IEXTROM128;
		MemTable.ExtRam = &MEM6::IEMPTRAM;
		break;
		
	case EXC6006SR:	// 拡張64KRAMカートリッジ
		// ROM:- RAM:64KB
		MemTable.ExtRom = &MEM6::IEMPTROM;
		MemTable.ExtRam = &MEM6::IEXTRAM64;
		break;
		
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		// ROM:128KB RAM:64KB
		MemTable.ExtRom = &MEM6::IEXTROM128;
		MemTable.ExtRam = &MEM6::IEXTRAM64;
		break;
		
	case EXC6053:	// ボイスシンセサイザー
		// ROM:16KB RAM:-
		MemTable.ExtRom = &MEM6::IEXTROM16;
		MemTable.ExtRam = &MEM6::IEMPTRAM;
		break;
		
	case EXCSOL1:	// 戦士のカートリッジ
		// ROM:128KB RAM:32KB
		MemTable.ExtRom = &MEM6::IEXTROM128;
		MemTable.ExtRam = &MEM6::IEXTRAM32;
		break;
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		// ROM:512KB RAM:128KB
		MemTable.ExtRom = &MEM6::IEXTROM512;
		MemTable.ExtRam = &MEM6::IEXTRAM128;
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		// ROM:8MB RAM:512KB
		MemTable.ExtRom = &MEM6::IEXTROM8M;
		MemTable.ExtRam = &MEM6::IEXTRAM512;
		break;
	}
	
	// ROM確保&ファイル読込み
	switch( ExCart ){
	case EXC6001:	// 拡張BASIC
		if( !AllocMemory( ExtRom, &IEXBASIC, path, crc ) ) return false;
		break;
		
	case EXC660101:	// 拡張漢字ROMカートリッジ
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		if( !AllocMemory( ExtRom, &IEXKANJI, path, crc ) ) return false;
		break;
		
	case EXC6053:	// ボイスシンセサイザー
		if( !AllocMemory( ExtRom, &IEXVOICE, path, crc ) ) return false;
		break;
		
	default:		// その他
		if( !AllocMemory( ExtRom, MemTable.ExtRom, "", crc ) ) return false;
	}
	
	// RAM確保
	if( !AllocMemory( ExtRam, MemTable.ExtRam, "", crc ) ) return false;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Read Enable取得 (ROM KILL)
/////////////////////////////////////////////////////////////////////////////
bool MEM6::GetReadEnableExt( WORD addr )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	switch( ExCart ){
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		return Sol60Mode && (CS01R && (idx == 0 || idx == 1));
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// Write Enable取得
/////////////////////////////////////////////////////////////////////////////
bool MEM6::GetWriteEnableExt( WORD addr )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	switch( ExCart ){
	case EXCSOL1:	// 戦士のカートリッジ
		return (idx == 3);
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		return Sol60Mode && ((CS01W && (idx == 0 || idx == 1)) || (CS02W && (idx == 2 || idx == 3)));
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// 外部メモリ初期化
//
// 引数:	なし
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM6::InitExt( void )
{
	PRINTD( MEM_LOG, "[MEM][InitExt]\n" );
	
	// ROM
	switch( ExCart ){
	case EXC6001:	// 拡張BASIC
		EMem[EXTROM+0].SetMemory( "EROM00", ExtRom( 0 ), MemTable.ExtRom->Wait );
		break;
		
	case EXC6005:	// ROMカートリッジ
	case EXC6006:	// 拡張ROM/RAMカートリッジ
		EMem[EXTROM+0].SetMemory( "EROM00", ExtRom( 0 ), MemTable.ExtRom->Wait );
		EMem[EXTROM+1].SetMemory( "EROM01", ExtRom( 1 ), MemTable.ExtRom->Wait );
		break;
		
	case EXC6053:	// ボイスシンセサイザー
		EMem[EXTROM+1].SetMemory( "EROM01", ExtRom( 0 ), MemTable.ExtRom->Wait );
		break;
		
	case EXCSOL1:	// 戦士のカートリッジ
		EMem[EXTROM+0].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM6::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM6::Sol2Read ) ), nullptr, MemTable.ExtRom->Wait );
		EMem[EXTROM+1].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM6::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM6::Sol2Read ) ), nullptr, MemTable.ExtRom->Wait );
		break;
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		EMem[EXTROM+0].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM6::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM6::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &MEM6::Sol2Write ) ), MemTable.ExtRom->Wait );
		EMem[EXTROM+1].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM6::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM6::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &MEM6::Sol2Write ) ), MemTable.ExtRom->Wait );
		break;
	}
	
	// RAM
	switch( ExCart ){
	case EXC6006:	// 拡張ROM/RAMカートリッジ
	case EXC6006SR:	// 拡張64KRAMカートリッジ
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		for( int i = 0; i < 8; i++ ){
			EMem[EXTRAM + i].SetMemory( Stringf( "ERAM%02d", i & (ExtRam.Size() - 1) ), ExtRam( i & (ExtRam.Size() - 1) ), MemTable.ExtRam->Wait );
		}
		break;
		
	case EXCSOL1:	// 戦士のカートリッジ
		SolBank.fill( NONBANK );
		SolBankSet = 0;
		EMem[EXTRAM+3].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM6::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM6::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &MEM6::Sol2Write ) ), MemTable.ExtRam->Wait );
		EMem[EXTRAM+4].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM6::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM6::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &MEM6::Sol2Write ) ), MemTable.ExtRam->Wait );
		EMem[EXTRAM+5].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM6::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM6::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &MEM6::Sol2Write ) ), MemTable.ExtRam->Wait );
		break;
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		SolBank.fill( NONBANK );
		SolBankSet = 0;
		for( int i = 0; i < 8; i++ ){
			EMem[EXTRAM + i].SetFunc( FN( STATIC_CAST( NFuncPtr, &MEM6::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &MEM6::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &MEM6::Sol2Write ) ), MemTable.ExtRam->Wait );
		}
		break;
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 外部メモリリセット
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void MEM6::ResetExt( void )
{
	PRINTD( MEM_LOG, "[MEM][ResetExt]\n" );
	
	switch( ExCart ){
	case EXC660101:		// 拡張漢字ROMカートリッジ
	case EXC6007SR:		// 拡張漢字ROM&RAMカートリッジ
		Kenable = false;
		break;
		
	case EXCSOL1:		// 戦士のカートリッジ
	case EXCSOL2:		// 戦士のカートリッジmkⅡ
	case EXCSOL3:		// 戦士のカートリッジmkⅢ
		Sol60Mode  = false;
		CS01R      = false;
		CS01W      = false;
		CS02W      = false;
		SolBankSet = 0;
		
		// メモリバンク初期化
		{	int i = 0;
			const std::vector<BYTE> bk2 = { NONBANK,     NONBANK,     ROMBANK | 0, RAMBANK | 3,
											RAMBANK | 4, RAMBANK | 5, NONBANK,     NONBANK      };
			for( auto bk : bk2 ){
				SetSol2Bank( i, bk );
				i++;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// 拡張ROM マウント
//
// 引数:	filepath	ファイルパス名への参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM6::MountExtRom( const P6VPATH& filepath )
{
	PRINTD( MEM_LOG, "[MEM][MountExtRom] -> %s -> ", P6VPATH2STR( filepath ).c_str() );
	
	// ファイル名が空またはROM固定カートリッジならエラー無しで戻る
	if( P6VPATH2STR( filepath ).empty() || (ExCart & EXCFIX) ){
		return true;
	}
	
	// マウント済みなら一旦開放
	UnmountExtRom();
	
	try{
		if( !ExtRom.SetData( filepath ) ){
			throw Error::GetError();
		}
		
		// ファイルパス保存
		FilePath = filepath;
	}
	// 例外発生
	catch( Error::Errno i ){
		Error::SetError( i );
		
		PRINTD( MEM_LOG, "Error\n" );
		
		UnmountExtRom();
		return false;
	}
	
	PRINTD( MEM_LOG, "OK %s\n", P6VPATH2STR( FilePath ).c_str() );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 拡張ROM アンマウント
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void MEM6::UnmountExtRom( void )
{
	PRINTD( MEM_LOG, "[MEM][UnmountExtRom]\n" );
	
	ExtRom.Resize( MemTable.ExtRom->Size, MemTable.ExtRom->Init );
	FilePath.clear();
}


/////////////////////////////////////////////////////////////////////////////
// 拡張ROM ファイルパス取得
//
// 引数:	なし
// 返値:	P6VPATH&	ファイルパス名への参照
/////////////////////////////////////////////////////////////////////////////
const P6VPATH& MEM6::GetFile( void ) const
{
	return FilePath;
}


/////////////////////////////////////////////////////////////////////////////
// 拡張カートリッジの種類取得
//
// 引数:	なし
// 返値:	WORD		カートリッジの種類
/////////////////////////////////////////////////////////////////////////////
WORD MEM6::GetCartridge( void ) const
{
	return ExCart;
}


/////////////////////////////////////////////////////////////////////////////
// 直接アクセス関数
/////////////////////////////////////////////////////////////////////////////
BYTE MEM6::ReadExtRom( DWORD addr ) const { return ExtRom.Read( addr ); }
BYTE MEM6::ReadExtRam( DWORD addr ) const { return ExtRam.Read( addr ); }


/////////////////////////////////////////////////////////////////////////////
// I/Oアクセス関数
/////////////////////////////////////////////////////////////////////////////
// 拡張漢字ROMカートリッジ -------------------------------------
void MEM6::OutFCH( int port, BYTE data )
{
	Kaddr   = (data << 8) | ((port >> 8) & 0xff);
	Kenable = false;
}

void MEM6::OutFFH( int, BYTE ){ Kenable = !Kenable; }
BYTE MEM6::InFDH( int ){ return Kenable ? ExtRom.Read( (Kaddr << 1)     ) : 0xff; }
BYTE MEM6::InFEH( int ){ return Kenable ? ExtRom.Read( (Kaddr << 1) | 1 ) : 0xff; }

// ボイスシンセサイザー ----------------------------------------
void MEM6::Out70H( int, BYTE ){}
void MEM6::Out72H( int, BYTE ){}
void MEM6::Out73H( int, BYTE ){}
void MEM6::Out74H( int, BYTE ){}
BYTE MEM6::In70H( int ){ return 0xff; }
BYTE MEM6::In72H( int ){ return 0xff; }
BYTE MEM6::In73H( int ){ return 0xff; }

// FM音源カートリッジ ------------------------------------------
void MEM6::Out70Hf( int, BYTE ){}
void MEM6::Out71Hf( int, BYTE ){}
BYTE MEM6::In72Hf( int ){ return 0xff; }
BYTE MEM6::In73Hf( int ){ return 0xff; }

// 戦士のカートリッジ(全共通) ----------------------------------
void MEM6::Out7FH( int, BYTE data ){ SetSol2Bank( 2, ROMBANK | (data & 0x0f) ); }

// 戦士のカートリッジ mkⅡ -------------------------------------
// 戦士のカートリッジ mkⅢ -------------------------------------
void MEM6::Out06H( int, BYTE data ){ Sol60Mode = (data == 0x66) ? true : false; }
void MEM6::Out3xH( int port, BYTE data ){ SetSol2Bank( port, data ); }

void MEM6::OutF0Hs( int, BYTE data )
{
	switch( data & 0x0f ){			// RF0下位 (0000 - 3FFF)
	case 0x01:
	case 0x02:
	case 0x05:
	case 0x06:
		CS01R = false;
		break;
		
	default:
		CS01R = true;
	}
}

void MEM6::OutF2Hs( int, BYTE data )
{
	switch( data & 0x03 ){			// 0000 - 3FFF
	case 0:
		CS01W = false;
		break;
		
	default:
		CS01W = true;
	}
	
	switch( (data >> 2) & 0x03 ){	// 4000 - 7FFF
	case 0:
		CS02W = false;
		break;
		
	default:
		CS02W = true;
	}
}

// 戦士のカートリッジ mkⅢ -------------------------------------
void MEM6::Out07H( int, BYTE data ){ SolBankSet = data & 0x07; }


/////////////////////////////////////////////////////////////////////////////
// デバイスコネクタリスト取得
/////////////////////////////////////////////////////////////////////////////
const std::vector<IOBus::Connector>& MEM6::GetDeviceConnector( void )
{
	switch( ExCart ){
	case EXC6053:	return c_exvoice;	// ボイスシンセサイザー
	case EXC60M55:	return c_exfm;		// FM音源カートリッジ
	case EXC660101:						// 拡張漢字ROMカートリッジ
	case EXC6007SR:	return c_exkanji;	// 拡張漢字ROM&RAMカートリッジ
	case EXCSOL1:	return c_soldier1;	// 戦士のカートリッジ
	case EXCSOL2:	return c_soldier2;	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	return c_soldier3;	// 戦士のカートリッジmkⅢ
	default:		return c_dummy;
	}
}


/////////////////////////////////////////////////////////////////////////////
// デバイスコネクタ
/////////////////////////////////////////////////////////////////////////////

// Dummy
const std::vector<IOBus::Connector> MEM6::c_dummy = {};

// ボイスシンセサイザー ----------------------------------------
const std::vector<IOBus::Connector> MEM6::c_exvoice = {
	{ 0x70, IOBus::portout, out70H },
	{ 0x72, IOBus::portout, out72H },
	{ 0x73, IOBus::portout, out73H },
	{ 0x74, IOBus::portout, out74H },
	{ 0x70, IOBus::portin,  in70H },
	{ 0x72, IOBus::portin,  in72H },
	{ 0x73, IOBus::portin,  in73H }
};

// FM音源カートリッジ ------------------------------------------
const std::vector<IOBus::Connector> MEM6::c_exfm = {
	{ 0x70, IOBus::portout, out70Hf },
	{ 0x71, IOBus::portout, out71Hf },
	{ 0x72, IOBus::portin,  in72Hf },
	{ 0x73, IOBus::portin,  in73Hf }
};

// 拡張漢字ROMカートリッジ -------------------------------------
const std::vector<IOBus::Connector> MEM6::c_exkanji = {
	{ 0xfc, IOBus::portout, outFCH },
	{ 0xff, IOBus::portout, outFFH },
	{ 0xfd, IOBus::portin,  inFDH },
	{ 0xfe, IOBus::portin,  inFEH }
};

// 戦士のカートリッジ ------------------------------------------
const std::vector<IOBus::Connector> MEM6::c_soldier1 = {
	{ 0x70, IOBus::portout, out7FH  },
	{ 0x71, IOBus::portout, out7FH  },	// イメージ
	{ 0x72, IOBus::portout, out7FH  },	// イメージ
	{ 0x73, IOBus::portout, out7FH  },	// イメージ
	{ 0x74, IOBus::portout, out7FH  },	// イメージ
	{ 0x75, IOBus::portout, out7FH  },	// イメージ
	{ 0x76, IOBus::portout, out7FH  },	// イメージ
	{ 0x77, IOBus::portout, out7FH  },	// イメージ
	{ 0x78, IOBus::portout, out7FH  },	// イメージ
	{ 0x79, IOBus::portout, out7FH  },	// イメージ
	{ 0x7a, IOBus::portout, out7FH  },	// イメージ
	{ 0x7b, IOBus::portout, out7FH  },	// イメージ
	{ 0x7c, IOBus::portout, out7FH  },	// イメージ
	{ 0x7d, IOBus::portout, out7FH  },	// イメージ
	{ 0x7e, IOBus::portout, out7FH  },	// イメージ
	{ 0x7f, IOBus::portout, out7FH  }	// イメージ
};

// 戦士のカートリッジmkⅡ --------------------------------------
const std::vector<IOBus::Connector> MEM6::c_soldier2 = {
	{ 0x06, IOBus::portout, out06H  },
	{ 0x7f, IOBus::portout, out7FH  },
	{ 0x30, IOBus::portout, out3xH  },
	{ 0x31, IOBus::portout, out3xH  },
	{ 0x32, IOBus::portout, out3xH  },
	{ 0x33, IOBus::portout, out3xH  },
	{ 0x34, IOBus::portout, out3xH  },
	{ 0x35, IOBus::portout, out3xH  },
	{ 0x36, IOBus::portout, out3xH  },
	{ 0x37, IOBus::portout, out3xH  },
	{ 0x38, IOBus::portout, out3xH  },	// イメージ
	{ 0x39, IOBus::portout, out3xH  },	// イメージ
	{ 0x3a, IOBus::portout, out3xH  },	// イメージ
	{ 0x3b, IOBus::portout, out3xH  },	// イメージ
	{ 0x3c, IOBus::portout, out3xH  },	// イメージ
	{ 0x3d, IOBus::portout, out3xH  },	// イメージ
	{ 0x3e, IOBus::portout, out3xH  },	// イメージ
	{ 0x3f, IOBus::portout, out3xH  },	// イメージ
	{ 0xf0, IOBus::portout, outF0Hs },
	{ 0xf2, IOBus::portout, outF2Hs }
};

// 戦士のカートリッジmkⅢ --------------------------------------
const std::vector<IOBus::Connector> MEM6::c_soldier3 = {
	{ 0x06, IOBus::portout, out06H  },
	{ 0x07, IOBus::portout, out07H  },
	{ 0x7f, IOBus::portout, out7FH  },
	{ 0x30, IOBus::portout, out3xH  },
	{ 0x31, IOBus::portout, out3xH  },
	{ 0x32, IOBus::portout, out3xH  },
	{ 0x33, IOBus::portout, out3xH  },
	{ 0x34, IOBus::portout, out3xH  },
	{ 0x35, IOBus::portout, out3xH  },
	{ 0x36, IOBus::portout, out3xH  },
	{ 0x37, IOBus::portout, out3xH  },
	{ 0x38, IOBus::portout, out3xH  },	// イメージ
	{ 0x39, IOBus::portout, out3xH  },	// イメージ
	{ 0x3a, IOBus::portout, out3xH  },	// イメージ
	{ 0x3b, IOBus::portout, out3xH  },	// イメージ
	{ 0x3c, IOBus::portout, out3xH  },	// イメージ
	{ 0x3d, IOBus::portout, out3xH  },	// イメージ
	{ 0x3e, IOBus::portout, out3xH  },	// イメージ
	{ 0x3f, IOBus::portout, out3xH  },	// イメージ
	{ 0xf0, IOBus::portout, outF0Hs },
	{ 0xf2, IOBus::portout, outF2Hs }
};
// =============================================================












/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM6::DokoSave( cIni* Ini )
{
	PRINTD( MEM_LOG, "[MEM][DokoSave]\n" );
	
	if( !Ini ) return false;
	
	Ini->SetVal( "MEMORY", "CGBank",		"", CGBank );
	Ini->SetVal( "MEMORY", "M1Wait",		"", M1Wait );
	
	// 62,66,64,68
	for( int i = 0; i < 3; i++ ){
		Ini->SetVal( "MEMORY", Stringf( "Rf%d", i ), "", "0x%02X", Rf[i] );
	}
	
	// CGRomウェイト
	Ini->SetVal( "MEMORY", "CgRomWait",	"", IRom[CGROM1].GetWait() );
	
	// 内部RAM
	for( int i = 0; i < (int)MemTable.IntRam->Size; i += 64 ){
		std::string strva;
		for( int j = 0; j < 64; j++ ){
			strva += Stringf( "%02X", IntRam.Read( i+j ) );
		}
		Ini->SetEntry( "MEMORY", Stringf( "IntRam_%04X", i ), "", strva.c_str() );
	}
	
	
	// 拡張カートリッジ ========================================
	Ini->SetVal( "MEMORY", "ExCart",	"", ExCart );
	
	// 外部ROM
	if( !FilePath.empty() ){
		P6VPATH tpath = FilePath;
		OSD_RelativePath( tpath );
		Ini->SetVal( "MEMORY", "FilePath",	"", tpath );
	}
	
	// 外部RAM
	if( ExCart & EXCRAM ){
		for( int i = 0; i < (int)MemTable.ExtRam->Size; i += 64 ){
			std::string strva;
			for( int j = 0; j < 64; j++ ){
				strva += Stringf( "%02X", ExtRam.Read( i + j ) );
			}
			Ini->SetEntry( "MEMORY", Stringf( "ExtRam_%06X", i ), "", strva.c_str() );
		}
	}
	
	switch( ExCart ){
	case EXC6006:	// 拡張ROM/RAMカートリッジ
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		Ini->SetVal( "MEMORY", "Kaddr",			"", Kaddr      );
		Ini->SetVal( "MEMORY", "Kenable",		"", Kenable    );
		break;
		
	case EXCSOL1:	// 戦士のカートリッジ
		Ini->SetVal( "MEMORY", "SolBank2",		"", "0x%02X", SolBank[2] );
		break;
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		Ini->SetVal( "MEMORY", "Soldier60",		"", Sol60Mode  );
		Ini->SetVal( "MEMORY", "CS01R",			"", CS01R      );
		Ini->SetVal( "MEMORY", "CS01W",			"", CS01W      );
		Ini->SetVal( "MEMORY", "CS02W",			"", CS02W      );
		Ini->SetVal( "MEMORY", "SoldierBank",	"", SolBankSet );
		// メモリバンクレジスタ
		{	int i = 0;
			for( auto &sb : SolBank ){
				Ini->SetVal( "MEMORY", Stringf( "SolBank%d", i ), "", "0x%02X", sb );
				i++;
			}
		}
		break;
	}
	// =========================================================
	
	return true;
}

bool MEM62::DokoSave( cIni* Ini )
{
	if( !MEM6::DokoSave( Ini ) ) return false;
	
	Ini->SetVal( "MEMORY", "cgrom",			"", cgrom    );
	Ini->SetVal( "MEMORY", "kj_rom",		"", kj_rom   );
	Ini->SetVal( "MEMORY", "kj_LR",			"", kj_LR    );
	Ini->SetVal( "MEMORY", "cgenable",		"", cgenable );
	Ini->SetVal( "MEMORY", "cgaden",		"", cgaden   );
	Ini->SetVal( "MEMORY", "cgaddr",		"", cgaddr   );
	Ini->SetVal( "MEMORY", "c2acc",			"", c2acc    );
	
	// メモリウェイト
	Ini->SetVal( "MEMORY", "Wait",		"", GetWait() );
	return true;
}

bool MEM64::DokoSave( cIni* Ini )
{
	if( !MEM62::DokoSave( Ini ) ) return false;
	
	for( int i = 0; i < 16; i++ ){
		Ini->SetVal( "MEMORY", Stringf( "RfSR_%02d", i ), "", "0x%02X", RfSR[i] );
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool MEM6::DokoLoad( cIni* Ini )
{
	P6VPATH tpath = "";
	int st;
	
	PRINTD( MEM_LOG, "[MEM][DokoLoad]\n" );
	
	if( !Ini ) return false;
	
	Ini->GetVal( "MEMORY", "CGBank",		CGBank   );
	Ini->GetVal( "MEMORY", "M1Wait",		M1Wait   );
	
	// 62,66,64,68
	for( int i = 0; i < 3; i++ ){
		Ini->GetVal( "MEMORY", Stringf( "Rf%d", i ), Rf[i] );
	}
	
	// メモリブロック設定
	InitInt();
	SetMemBlockR( Rf[0], Rf[1] );
	SetMemBlockW( Rf[2] );
	SetCGBank( CGBank );
	
	// CGRomウェイト
	st = IRom[CGROM1].GetWait();
	Ini->GetVal( "MEMORY", "CgRomWait", st );
	st &= 0xff;
	IRom[CGROM1].SetWait( st );
	IRom[CGROM2].SetWait( st );
	
	// 内部RAM
	for( int i = 0; i < (int)MemTable.IntRam->Size; i += 64 ){
		std::string strva;
		if( Ini->GetEntry( "MEMORY", Stringf( "IntRam_%04X", i ), strva ) ){
			strva.resize( 64 * 2, '0' );
			for( int j = 0; j < 64; j++ ){
				IntRam.Write( i+j, std::stoul( strva.substr( j * 2, 2 ), nullptr, 16 ) );
			}
		}
	}
	
	
	// 拡張カートリッジ ========================================
	Ini->GetVal( "MEMORY", "ExCart",	ExCart   );
	
	// 外部メモリ初期化
	InitExt();
	
	// 拡張ROM
	if( Ini->GetVal( "MEMORY", "FilePath", tpath ) )
		MountExtRom( tpath );
	
	// 外部RAM
	if( ExCart & EXCRAM ){
		for( int i = 0; i < (int)MemTable.ExtRam->Size; i += 64 ){
			std::string strva;
			if( Ini->GetEntry( "MEMORY", Stringf( "ExtRam_%06X", i ), strva ) ){
				strva.resize( 64 * 2, '0' );
				for( int j = 0; j < 64; j++ ){
					ExtRam.Write( i+j, std::stoul( strva.substr( j * 2, 2 ), nullptr, 16 ) );
				}
			}
		}
	}
	
	switch( ExCart ){
	case EXC6006:	// 拡張ROM/RAMカートリッジ
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		Ini->GetVal( "MEMORY", "Kaddr",			Kaddr      );
		Ini->GetVal( "MEMORY", "Kenable",		Kenable    );
		break;
		
	case EXCSOL1:	// 戦士のカートリッジ
		Ini->GetVal( "MEMORY", "SolBank2",		SolBank[2] );
		break;
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		Ini->GetVal( "MEMORY", "Soldier60",		Sol60Mode  );
		Ini->GetVal( "MEMORY", "CS01R",			CS01R      );
		Ini->GetVal( "MEMORY", "CS01W",			CS01W      );
		Ini->GetVal( "MEMORY", "CS02W",			CS02W      );
		Ini->GetVal( "MEMORY", "SoldierBank",	SolBankSet );
		// メモリバンクレジスタ
		{	int i = 0;
			for( auto &sb : SolBank ){
				Ini->GetVal( "MEMORY", Stringf( "SolBank%d", i ), sb );
				SetSol2Bank( i, sb );	// メモリバンク設定
				i++;
			}
		}
		break;
	}
	// =========================================================
	
	return true;
}

bool MEM62::DokoLoad( cIni* Ini )
{
	int st;
	
	if( !MEM6::DokoLoad( Ini ) ) return false;
	
	Ini->GetVal( "MEMORY", "cgrom",			cgrom    );
	Ini->GetVal( "MEMORY", "kj_rom",		kj_rom   );
	Ini->GetVal( "MEMORY", "kj_LR",			kj_LR    );
	Ini->GetVal( "MEMORY", "cgenable",		cgenable );
	Ini->GetVal( "MEMORY", "cgaden",		cgaden   );
	Ini->GetVal( "MEMORY", "cgaddr",		cgaddr   );
	Ini->GetVal( "MEMORY", "c2acc",			c2acc    );
	
	// メモリウェイト
	st = GetWait();
	Ini->GetVal( "MEMORY", "Wait",      st );
	SetWait( st );
	
	return true;
}

bool MEM64::DokoLoad( cIni* Ini )
{
	if( !MEM62::DokoLoad( Ini ) ) return false;
	
	for( int i = 0; i < 16; i++ ){
		Ini->GetVal( "MEMORY", Stringf( "RfSR_%02d", i ), RfSR[i] );
		SetMemBlockSR( i, RfSR[i] );
	}
	
	return true;
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/////////////////////////////////////////////////////////////////////////////
// モニタモード用関数
/////////////////////////////////////////////////////////////////////////////
const std::string& MEM6::GetReadMemBlk( int blk ) const
{
	 return RBLK1[blk]->GetName( blk << MemBlock::PAGEBITS );
}

const std::string& MEM64::GetReadMemBlk( int blk ) const
{
	return vm->VdgIsSRmode() ? RBLKSR[blk]->GetName( blk << MemBlock::PAGEBITS )
							 : RBLK1 [blk]->GetName( blk << MemBlock::PAGEBITS );
}


const std::string& MEM6::GetWriteMemBlk( int blk ) const
{
	return WBLK1[blk]->GetName( blk << MemBlock::PAGEBITS, true );
}

const std::string& MEM64::GetWriteMemBlk( int blk ) const
{
	return vm->VdgIsSRmode() ? WBLKSR[blk]->GetName( blk << MemBlock::PAGEBITS, true )
							 : WBLK1 [blk]->GetName( blk << MemBlock::PAGEBITS, true );
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@






























/////////////////////////////////////////////////////////////////////////////
// メモリ情報
/////////////////////////////////////////////////////////////////////////////
// 共通										ROM情報		サイズ		初期値	Wait
const MEMB::MEMINFO EXTCART::IEXTROM8   = { NOROM,		0x002000,	0xff,	1 };
const MEMB::MEMINFO EXTCART::IEXTROM16  = { NOROM,		0x004000,	0xff,	1 };
const MEMB::MEMINFO EXTCART::IEXTROM128 = { NOROM,		0x020000,	0xff,	1 };
const MEMB::MEMINFO EXTCART::IEXTROM512 = { NOROM,		0x080000,	0xff,	1 };
const MEMB::MEMINFO EXTCART::IEXTROM8M  = { NOROM,		0x800000,	0xff,	1 };
const MEMB::MEMINFO EXTCART::IEXTRAM16  = { NOROM,		0x004000,	0x00,	0 };
const MEMB::MEMINFO EXTCART::IEXTRAM32  = { NOROM,		0x008000,	0x00,	0 };
const MEMB::MEMINFO EXTCART::IEXTRAM64  = { NOROM,		0x010000,	0x00,	0 };
const MEMB::MEMINFO EXTCART::IEXTRAM128 = { NOROM,		0x020000,	0x00,	0 };
const MEMB::MEMINFO EXTCART::IEXTRAM512 = { NOROM,		0x080000,	0x00,	0 };


// 拡張カートリッジ
const MEMB::MEMINFO EXTCART::IEXBASIC   = { EXBASIC00,	0x002000,	0xff,	1 };
const MEMB::MEMINFO EXTCART::IEXKANJI   = { EXKANJI00,	0x020000,	0xff,	0 };
const MEMB::MEMINFO EXTCART::IEXVOICE   = { EXVOICE00,	0x004000,	0xff,	1 };




/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
EXTCART::EXTCART( VM6* vm, const ID& id, DWORD cart ) : Device( vm, id ),
	ExCart( cart ), FilePath( "" ),
	Kaddr( 0 ), Kenable( false ),
	Sol60Mode( false ), CS01R( false ), CS01W( false ), CS02W( false ), SolBankSet( 0 )
{
	descs.outdef.clear();
	descs.indef.clear();
	
	AddDeviceDescriptorExt();
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
EXTCART::~EXTCART( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// メモリブロック用関数
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ/Ⅲ メモリブロック名取得
/////////////////////////////////////////////////////////////////////////////
const std::string& EXTCART::Sol2GetName( WORD addr, bool rw )
{
	int bank  = SolBank[addr >> MemBlock::PAGEBITS];
	int bank3 = (bank & 0x3f) | ((bank & 0x80) >> 1) | ((bank & 0x40) << 1);
	static std::string name = "";
	
	switch( ExCart ){
	case EXCSOL1:	// 戦士のカートリッジ
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		switch( bank & 0xc0 ){
		case ROMBANK:	// ROM
			name = Stringf( "EROM%02d", (SolBankSet | (bank & 0x0f)) & (ExtRom.Size() - 1) );
			break;
			
		case RAMBANK:	// RAM
			name = Stringf( "ERAM%02d", (bank & 0x0f) & (ExtRam.Size() - 1) );
			break;
			
		case SCCBANK:	// SCC
			name = "SCC";
			break;
			
		case NONBANK:	// 無効
		default:
			name = "EMPTY";
		}
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		switch( bank & 0xc0 ){
		case ROMBANK:	// ROM L
		case ROMBANKH:	// ROM H
			name = Stringf( "EROM%03d", ((SolBankSet << 7) | (bank3 & 0x7f)) & (ExtRom.Size() - 1) );
			break;
			
		case RAMBANK:	// RAM
			name = Stringf( "ERAM%03d", (bank3 & 0x3f) & (ExtRam.Size() - 1) );
			break;
			
		case NONBANK:	// 無効
		default:
			name = "EMPTY";
		}
		break;
	}
	
	return name;
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ/Ⅲ 読込み
/////////////////////////////////////////////////////////////////////////////
BYTE EXTCART::Sol2Read( MemCell* ptr, WORD addr, int* wcnt )
{
	int bank  = SolBank[addr >> MemBlock::PAGEBITS];
	int bank3 = (bank & 0x3f) | ((bank & 0x80) >> 1) | ((bank & 0x40) << 1);
	
	switch( ExCart ){
	case EXCSOL1:	// 戦士のカートリッジ
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		switch( bank & 0xc0 ){
		case ROMBANK:	// ROM
			return ExtRom( (SolBankSet | (bank & 0x0f)) & (ExtRom.Size() - 1) ).Read( addr );
			
		case RAMBANK:	// RAM
			return ExtRam( (bank & 0x0f) & (ExtRam.Size() - 1) ).Read( addr );
			
		case SCCBANK:	// SCC
			return Sol2SccRead( ptr, addr, wcnt );
		}
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		switch( bank & 0xc0 ){
		case ROMBANK:	// ROM L
		case ROMBANKH:	// ROM H
			return ExtRom( ((SolBankSet << 7) | (bank3 & 0x7f)) & (ExtRom.Size() - 1) ).Read( addr );
			
		case RAMBANK:	// RAM
			return ExtRam( (bank3 & 0x3f) & (ExtRam.Size() - 1) ).Read( addr );
		}
		break;
	}
	
	return 0xff;
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ/Ⅲ 書込み
/////////////////////////////////////////////////////////////////////////////
void EXTCART::Sol2Write( MemCell* ptr, WORD addr, BYTE data, int* wcnt )
{
	int bank  = SolBank[addr >> MemBlock::PAGEBITS];
	int bank3 = (bank & 0x3f) | ((bank & 0x80) >> 1) | ((bank & 0x40) << 1);
	
	switch( ExCart ){
	case EXCSOL1:	// 戦士のカートリッジ
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		switch( bank & 0xc0 ){
		case RAMBANK:	// RAM
			ExtRam( (bank & 0x0f) & (ExtRam.Size() - 1) ).Write( addr, data );
			break;
			
		case SCCBANK:	// SCC
			Sol2SccWrite( ptr, addr, data, wcnt );
			break;
		}
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		switch( bank & 0xc0 ){
		case RAMBANK:	// RAM
			ExtRam( (bank3 & 0x3f) & (ExtRam.Size() - 1) ).Write( addr, data );
			break;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ SCC読込み
/////////////////////////////////////////////////////////////////////////////
BYTE EXTCART::Sol2SccRead( MemCell* ptr, WORD addr, int* wcnt )
{
	// 後で書く
	return 0xff;
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ SCC書込み
/////////////////////////////////////////////////////////////////////////////
void EXTCART::Sol2SccWrite( MemCell* ptr, WORD addr, BYTE data, int* wcnt )
{
	// 後で書く
}

/////////////////////////////////////////////////////////////////////////////
// メモリブロック用関数 ここまで
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
// 拡張カートリッジ デバイスディスクリプタ追加
/////////////////////////////////////////////////////////////////////////////
void EXTCART::AddDeviceDescriptorExt( void )
{
	switch( ExCart ){
	case EXC660101:	// 拡張漢字ROMカートリッジ
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		// Device Description (Out)
		descs.outdef.emplace( outFCH,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::OutFCH  ) );
		descs.outdef.emplace( outFFH,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::OutFFH  ) );
		
		// Device Description (In)
		descs.indef.emplace ( inFDH,   STATIC_CAST( Device::InFuncPtr,  &EXTCART::InFDH   ) );
		descs.indef.emplace ( inFEH,   STATIC_CAST( Device::InFuncPtr,  &EXTCART::InFEH   ) );
		break;
		
	case EXC6053:	// ボイスシンセサイザー
		// Device Description (Out)
		descs.outdef.emplace( out70H,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out70H  ) );
		descs.outdef.emplace( out72H,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out72H  ) );
		descs.outdef.emplace( out73H,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out73H  ) );
		descs.outdef.emplace( out74H,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out74H  ) );
		
		// Device Description (In)
		descs.indef.emplace ( in70H,   STATIC_CAST( Device::InFuncPtr,  &EXTCART::In70H   ) );
		descs.indef.emplace ( in72H,   STATIC_CAST( Device::InFuncPtr,  &EXTCART::In72H   ) );
		descs.indef.emplace ( in73H,   STATIC_CAST( Device::InFuncPtr,  &EXTCART::In73H   ) );
		break;
		
	case EXC60M55:	// FM音源カートリッジ
		// Device Description (Out)
		descs.outdef.emplace( out70Hf, STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out70Hf ) );
		descs.outdef.emplace( out71Hf, STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out71Hf ) );
		
		// Device Description (In)
		descs.indef.emplace ( in72Hf,  STATIC_CAST( Device::InFuncPtr,  &EXTCART::In72Hf  ) );
		descs.indef.emplace ( in73Hf,  STATIC_CAST( Device::InFuncPtr,  &EXTCART::In73Hf  ) );
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		// Device Description (Out)
		descs.outdef.emplace( out07H,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out07H  ) );
		[[fallthrough]];
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		// Device Description (Out)
		descs.outdef.emplace( out06H,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out06H  ) );
		descs.outdef.emplace( out3xH,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out3xH  ) );
		descs.outdef.emplace( outF0Hs, STATIC_CAST( Device::OutFuncPtr, &EXTCART::OutF0Hs ) );	// 戦士のカートリッジ 60対応
		descs.outdef.emplace( outF2Hs, STATIC_CAST( Device::OutFuncPtr, &EXTCART::OutF2Hs ) );	// 戦士のカートリッジ 60対応
		[[fallthrough]];
		
	case EXCSOL1:	// 戦士のカートリッジ
		// Device Description (Out)
		descs.outdef.emplace( out7FH,  STATIC_CAST( Device::OutFuncPtr, &EXTCART::Out7FH  ) );
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅡ メモリバンクレジスタ設定
//
//  [ROMの場合]
//    bit7   : 0
//    bit6   : 0
//    bit5-4 : Bank Set No. (0-3)
//    bit3-0 : Bank No. (0-15)
//  [RAMの場合]
//    bit7   : 0
//    bit6   : 1
//    bit5-4 : 未使用
//    bit3-0 : Bank No. (0-15)
//  [SCCの場合]
//    bit7   : 1
//    bit6   : 0
//    bit5-3 : 未使用
//    bit2   : 必ず1にする(AA15に出力される)
//    bit1   : 必ず0にする(AA14に出力される)
//    bit0   : 0:互換モード 1:SCC-Iモード
//  [無効の場合]
//    bit7   : 1
//    bit6   : 1
//    bit5-0 : 未使用
/////////////////////////////////////////////////////////////////////////////
// 戦士のカートリッジmkⅢ メモリバンクレジスタ設定
//
//  [ROMの場合]
//    bit7   : 0
//    bit6   : 0
//    bit5-0 : Bank No. (0-63)
//  [RAMの場合]
//    bit7   : 0
//    bit6   : 1
//    bit5-0 : Bank No. (0-63)
//  [ROMの場合]
//    bit7   : 1
//    bit6   : 0
//    bit5-0 : Bank No. (64-127)
//  [無効の場合]
//    bit7   : 1
//    bit6   : 1
//    bit5-0 : 未使用
/////////////////////////////////////////////////////////////////////////////
void EXTCART::SetSol2Bank( BYTE port, BYTE data )
{
	SolBank[port & 7] = data;
	
	switch( ExCart ){
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		if( (data & 0xc0) == ROMBANK ){
			SolBankSet = data & 0x30;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 外部メモリ確保とSystemROMファイル読込み
//
// 引数:	path		SystemROMフォルダパス名への参照
//			crc			CRCチェック true:する false:しない
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EXTCART::AllocMemoryExt( const P6VPATH& path, bool crc )
{
	PRINTD( MEM_LOG, "[EXTCART][AllocMemoryExt]\n" );
	
	switch( ExCart ){
	case EXC6001:	// 拡張BASIC
		// ROM:8KB RAM:-
		MemTable.ExtRom = &IEXTROM8;
		MemTable.ExtRam = &IEMPTRAM;
		break;
		
	case EXC6005:	// ROMカートリッジ
		// ROM:16KB RAM:-
		MemTable.ExtRom = &IEXTROM16;
		MemTable.ExtRam = &IEMPTRAM;
		break;
		
	case EXC6006:	// 拡張ROM/RAMカートリッジ
		// ROM:16KB RAM:16KB
		MemTable.ExtRom = &IEXTROM16;
		MemTable.ExtRam = &IEXTRAM16;
		break;
		
	case EXC660101:	// 拡張漢字ROMカートリッジ
		// ROM:128KB RAM:-
		MemTable.ExtRom = &IEXTROM128;
		MemTable.ExtRam = &IEMPTRAM;
		break;
		
	case EXC6006SR:	// 拡張64KRAMカートリッジ
		// ROM:- RAM:64KB
		MemTable.ExtRom = &IEMPTROM;
		MemTable.ExtRam = &IEXTRAM64;
		break;
		
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		// ROM:128KB RAM:64KB
		MemTable.ExtRom = &IEXTROM128;
		MemTable.ExtRam = &IEXTRAM64;
		break;
		
	case EXC6053:	// ボイスシンセサイザー
		// ROM:16KB RAM:-
		MemTable.ExtRom = &IEXTROM16;
		MemTable.ExtRam = &IEMPTRAM;
		break;
		
	case EXCSOL1:	// 戦士のカートリッジ
		// ROM:128KB RAM:32KB
		MemTable.ExtRom = &IEXTROM128;
		MemTable.ExtRam = &IEXTRAM32;
		break;
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
		// ROM:512KB RAM:128KB
		MemTable.ExtRom = &IEXTROM512;
		MemTable.ExtRam = &IEXTRAM128;
		break;
		
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		// ROM:8MB RAM:512KB
		MemTable.ExtRom = &IEXTROM8M;
		MemTable.ExtRam = &IEXTRAM512;
		break;
	}
	
	// ROM確保&ファイル読込み
	switch( ExCart ){
	case EXC6001:	// 拡張BASIC
		if( !AllocMemory( ExtRom, &IEXBASIC, path, crc ) ) return false;
		break;
		
	case EXC660101:	// 拡張漢字ROMカートリッジ
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		if( !AllocMemory( ExtRom, &IEXKANJI, path, crc ) ) return false;
		break;
		
	case EXC6053:	// ボイスシンセサイザー
		if( !AllocMemory( ExtRom, &IEXVOICE, path, crc ) ) return false;
		break;
		
	default:		// その他
		if( !AllocMemory( ExtRom, MemTable.ExtRom, "", crc ) ) return false;
	}
	
	// RAM確保
	if( !AllocMemory( ExtRam, MemTable.ExtRam, "", crc ) ) return false;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Read Enable取得 (ROM KILL)
/////////////////////////////////////////////////////////////////////////////
bool EXTCART::GetReadEnableExt( WORD addr )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	switch( ExCart ){
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		return Sol60Mode && (CS01R && (idx == 0 || idx == 1));
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// Write Enable取得
/////////////////////////////////////////////////////////////////////////////
bool EXTCART::GetWriteEnableExt( WORD addr )
{
	int idx = addr >> MemBlock::PAGEBITS;
	
	switch( ExCart ){
	case EXCSOL1:	// 戦士のカートリッジ
		return (idx == 3);
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		return Sol60Mode && ((CS01W && (idx == 0 || idx == 1)) || (CS02W && (idx == 2 || idx == 3)));
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// 外部メモリ初期化
//
// 引数:	memb		外部ROM/RAMブロック
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EXTCART::InitExt( std::vector<MemBlock>& memb )
{
	PRINTD( MEM_LOG, "[EXTCART][InitExt]\n" );
	
	// ROM
	switch( ExCart ){
	case EXC6001:	// 拡張BASIC
		memb[EXTROM+0].SetMemory( "EROM00", ExtRom( 0 ), MemTable.ExtRom->Wait );
		break;
		
	case EXC6005:	// ROMカートリッジ
	case EXC6006:	// 拡張ROM/RAMカートリッジ
		memb[EXTROM+0].SetMemory( "EROM00", ExtRom( 0 ), MemTable.ExtRom->Wait );
		memb[EXTROM+1].SetMemory( "EROM01", ExtRom( 1 ), MemTable.ExtRom->Wait );
		break;
		
	case EXC6053:	// ボイスシンセサイザー
		memb[EXTROM+1].SetMemory( "EROM01", ExtRom( 0 ), MemTable.ExtRom->Wait );
		break;
		
	case EXCSOL1:	// 戦士のカートリッジ
		memb[EXTROM+0].SetFunc( FN( STATIC_CAST( NFuncPtr, &EXTCART::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &EXTCART::Sol2Read ) ), nullptr, MemTable.ExtRom->Wait );
		memb[EXTROM+1].SetFunc( FN( STATIC_CAST( NFuncPtr, &EXTCART::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &EXTCART::Sol2Read ) ), nullptr, MemTable.ExtRom->Wait );
		break;
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		memb[EXTROM+0].SetFunc( FN( STATIC_CAST( NFuncPtr, &EXTCART::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &EXTCART::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &EXTCART::Sol2Write ) ), MemTable.ExtRom->Wait );
		memb[EXTROM+1].SetFunc( FN( STATIC_CAST( NFuncPtr, &EXTCART::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &EXTCART::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &EXTCART::Sol2Write ) ), MemTable.ExtRom->Wait );
		break;
	}
	
	// RAM
	switch( ExCart ){
	case EXC6006:	// 拡張ROM/RAMカートリッジ
	case EXC6006SR:	// 拡張64KRAMカートリッジ
	case EXC6007SR:	// 拡張漢字ROM&RAMカートリッジ
		for( int i = 0; i < 8; i++ ){
			memb[EXTRAM + i].SetMemory( Stringf( "ERAM%02d", i & (ExtRam.Size() - 1) ), ExtRam( i & (ExtRam.Size() - 1) ), MemTable.ExtRam->Wait );
		}
		break;
		
	case EXCSOL1:	// 戦士のカートリッジ
		SolBank.fill( NONBANK );
		SolBankSet = 0;
		memb[EXTRAM+3].SetFunc( FN( STATIC_CAST( NFuncPtr, &EXTCART::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &EXTCART::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &EXTCART::Sol2Write ) ), MemTable.ExtRam->Wait );
		memb[EXTRAM+4].SetFunc( FN( STATIC_CAST( NFuncPtr, &EXTCART::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &EXTCART::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &EXTCART::Sol2Write ) ), MemTable.ExtRam->Wait );
		memb[EXTRAM+5].SetFunc( FN( STATIC_CAST( NFuncPtr, &EXTCART::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &EXTCART::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &EXTCART::Sol2Write ) ), MemTable.ExtRam->Wait );
		break;
		
	case EXCSOL2:	// 戦士のカートリッジmkⅡ
	case EXCSOL3:	// 戦士のカートリッジmkⅢ
		SolBank.fill( NONBANK );
		SolBankSet = 0;
		for( int i = 0; i < 8; i++ ){
			memb[EXTRAM + i].SetFunc( FN( STATIC_CAST( NFuncPtr, &EXTCART::Sol2GetName ) ), FR( STATIC_CAST( RFuncPtr, &EXTCART::Sol2Read ) ), FW( STATIC_CAST( WFuncPtr, &EXTCART::Sol2Write ) ), MemTable.ExtRam->Wait );
		}
		break;
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 外部メモリリセット
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EXTCART::ResetExt( void )
{
	PRINTD( MEM_LOG, "[EXTCART][ResetExt]\n" );
	
	switch( ExCart ){
	case EXC660101:		// 拡張漢字ROMカートリッジ
	case EXC6007SR:		// 拡張漢字ROM&RAMカートリッジ
		Kenable = false;
		break;
		
	case EXCSOL1:		// 戦士のカートリッジ
	case EXCSOL2:		// 戦士のカートリッジmkⅡ
	case EXCSOL3:		// 戦士のカートリッジmkⅢ
		Sol60Mode  = false;
		CS01R      = false;
		CS01W      = false;
		CS02W      = false;
		SolBankSet = 0;
		
		// メモリバンク初期化
		{	int i = 0;
			const std::vector<BYTE> bk2 = { NONBANK,     NONBANK,     ROMBANK | 0, RAMBANK | 3,
											RAMBANK | 4, RAMBANK | 5, NONBANK,     NONBANK      };
			for( auto bk : bk2 ){
				SetSol2Bank( i, bk );
				i++;
			}
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 拡張ROM マウント
//
// 引数:	filepath	ファイルパス名への参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EXTCART::MountExtRom( const P6VPATH& filepath )
{
	PRINTD( MEM_LOG, "[EXTCART][MountExtRom] -> %s -> ", P6VPATH2STR( filepath ).c_str() );
	
	// ファイル名が空またはROM固定カートリッジならエラー無しで戻る
	if( P6VPATH2STR( filepath ).empty() || (ExCart & EXCFIX) ){
		return true;
	}
	
	// マウント済みなら一旦開放
	UnmountExtRom();
	
	try{
		if( !ExtRom.SetData( filepath ) ){
			throw Error::GetError();
		}
		
		// ファイルパス保存
		FilePath = filepath;
	}
	// 例外発生
	catch( Error::Errno i ){
		Error::SetError( i );
		
		PRINTD( MEM_LOG, "Error\n" );
		
		UnmountExtRom();
		return false;
	}
	
	PRINTD( MEM_LOG, "OK %s\n", P6VPATH2STR( FilePath ).c_str() );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 拡張ROM アンマウント
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EXTCART::UnmountExtRom( void )
{
	PRINTD( MEM_LOG, "[EXTCART][UnmountExtRom]\n" );
	
	ExtRom.Resize( MemTable.ExtRom->Size, MemTable.ExtRom->Init );
	FilePath.clear();
}


/////////////////////////////////////////////////////////////////////////////
// 拡張ROM ファイルパス取得
//
// 引数:	なし
// 返値:	P6VPATH&	ファイルパス名への参照
/////////////////////////////////////////////////////////////////////////////
const P6VPATH& EXTCART::GetFile( void ) const
{
	return FilePath;
}


/////////////////////////////////////////////////////////////////////////////
// 拡張カートリッジの種類取得
//
// 引数:	なし
// 返値:	WORD		カートリッジの種類
/////////////////////////////////////////////////////////////////////////////
WORD EXTCART::GetCartridge( void ) const
{
	return ExCart;
}


/////////////////////////////////////////////////////////////////////////////
// 直接アクセス関数
/////////////////////////////////////////////////////////////////////////////
BYTE EXTCART::ReadExtRom( DWORD addr ) const { return ExtRom.Read( addr ); }
BYTE EXTCART::ReadExtRam( DWORD addr ) const { return ExtRam.Read( addr ); }


/////////////////////////////////////////////////////////////////////////////
// I/Oアクセス関数
/////////////////////////////////////////////////////////////////////////////

// 拡張漢字ROMカートリッジ -------------------------------------
void EXTCART::OutFCH( int port, BYTE data )
{
	Kaddr   = (data << 8) | ((port >> 8) & 0xff);
	Kenable = false;
}

void EXTCART::OutFFH( int, BYTE ){ Kenable = !Kenable; }
BYTE EXTCART::InFDH( int ){ return Kenable ? ExtRom.Read( (Kaddr << 1)     ) : 0xff; }
BYTE EXTCART::InFEH( int ){ return Kenable ? ExtRom.Read( (Kaddr << 1) | 1 ) : 0xff; }

// ボイスシンセサイザー ----------------------------------------
void EXTCART::Out70H( int, BYTE ){}
void EXTCART::Out72H( int, BYTE ){}
void EXTCART::Out73H( int, BYTE ){}
void EXTCART::Out74H( int, BYTE ){}
BYTE EXTCART::In70H( int ){ return 0xff; }
BYTE EXTCART::In72H( int ){ return 0xff; }
BYTE EXTCART::In73H( int ){ return 0xff; }

// FM音源カートリッジ ------------------------------------------
void EXTCART::Out70Hf( int, BYTE ){}
void EXTCART::Out71Hf( int, BYTE ){}
BYTE EXTCART::In72Hf( int ){ return 0xff; }
BYTE EXTCART::In73Hf( int ){ return 0xff; }

// 戦士のカートリッジ(全共通) ----------------------------------
void EXTCART::Out7FH( int, BYTE data ){ SetSol2Bank( 2, ROMBANK | (data & 0xf) ); }

// 戦士のカートリッジ mkⅡ -------------------------------------
// 戦士のカートリッジ mkⅢ -------------------------------------
void EXTCART::Out06H( int, BYTE data ){ Sol60Mode = (data == 0x66) ? true : false; }
void EXTCART::Out3xH( int port, BYTE data ){ SetSol2Bank( port, data ); }

void EXTCART::OutF0Hs( int, BYTE data )
{
	switch( data & 0x0f ){			// RF0下位 (0000 - 3FFF)
	case 0x01:
	case 0x02:
	case 0x05:
	case 0x06:
		CS01R = false;
		break;
		
	default:
		CS01R = true;
	}
}

void EXTCART::OutF2Hs( int, BYTE data )
{
	switch( data & 0x03 ){			// 0000 - 3FFF
	case 0:
		CS01W = false;
		break;
		
	default:
		CS01W = true;
	}
	
	switch( (data >> 2) & 0x03 ){	// 4000 - 7FFF
	case 0:
		CS02W = false;
		break;
		
	default:
		CS02W = true;
	}
}

// 戦士のカートリッジ mkⅢ -------------------------------------
void EXTCART::Out07H( int, BYTE data ){ SolBankSet = data & 0x07; }



