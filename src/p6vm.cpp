#include <new>

#include "log.h"
#include "config.h"
#include "pc6001v.h"
#include "p6vm.h"
#include "p6el.h"
#include "breakpoint.h"
#include "cpum.h"
#include "cpus.h"
#include "debug.h"
#include "disk.h"
#include "graph.h"
#include "intr.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "pio.h"
#include "psg.h"
#include "schedule.h"
#include "sound.h"
#include "tape.h"
#include "voice.h"
#include "vdg.h"




////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
VM6::VM6( EL6 *emuobj ) : clock(0), el(emuobj), evsc(NULL), iom(NULL), ios(NULL),
	intr(NULL), cpum(NULL), cpus(NULL), mem(NULL),
	vdg(NULL), psg(NULL), voice(NULL), pio(NULL), key(NULL),
	cmtl(NULL), cmts(NULL), disk(NULL)
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	, bp(NULL)
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	{}

VM60::VM60( EL6 *emuobj ) : VM6(emuobj){ clock = CPUM_CLOCK60; }
VM62::VM62( EL6 *emuobj ) : VM6(emuobj){ clock = CPUM_CLOCK62; }
VM66::VM66( EL6 *emuobj ) : VM6(emuobj){ clock = CPUM_CLOCK62; }



////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
VM6::~VM6( void )
{
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	if( bp )     delete bp;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	if( voice )  delete voice;
	if( key )    delete key;
	if( pio )    delete pio;
	if( cpus )   delete cpus;
	if( cmts )   delete cmts;
	if( cmtl )   delete cmtl;
	if( disk )   delete disk;
	if( cpum )   delete cpum;
	if( intr )   delete intr;
	if( vdg )    delete vdg;
	if( mem )    delete mem;
	if( psg )    delete psg;
	if( ios )    delete ios;
	if( iom )    delete iom;
	if( evsc )   delete evsc;
}

VM60::~VM60( void ){}
VM62::~VM62( void ){}
VM66::~VM66( void ){}



////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool VM60::Init( CFG6 *cnfg )
{
	PRINTD( VM_LOG, "[VM][Init]\n" );
	
	if( !cnfg ) return false;
	
	// 各種オブジェクト確保
	cpus   = new SUB60( this, DEV_ID("8049") );						// SUB CPU
	intr   = new INT60( this, DEV_ID("INTR") );						// 割込み
	mem    = new MEM60( DEV_ID("MEM1"), cnfg->GetUseExtRam() );		// メモリ
	vdg    = new VDG60( this, DEV_ID("VDG1") );						// VDG
	key    = new KEY60( this, DEV_ID("KEYB") );						// キー
	disk   = new DSK60( this, DEV_ID("DISK") );						// DISK
	
	if( !cpus || !intr || !mem || !vdg || !key || !disk )
		return false;
	
	// 共通部分初期化
	if( !VM6::InitCommon( cnfg ) ) return false;
	
	// 各種オブジェクト初期化とデバイス接続
	
	// SUB CPU -----
	cpus->Reset();
	
	// 割込み -----
	const static IOBus::Connector c_intr[] = {
		{ 0xb0, IOBus::portout, INT60::outB0H },
		{ 0xb1, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb2, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb3, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb4, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb5, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb6, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb7, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb8, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xb9, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xba, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbb, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbc, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbd, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbe, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0xbf, IOBus::portout, INT60::outB0H },	// イメージ
		{ 0, 0, 0 }	};
	if( !iom->Connect( STATIC_CAST( INT60*, intr ), c_intr ) ) return false;
	
	// VDG -----
	const static IOBus::Connector c_vdg[] = {
		{ 0xb0, IOBus::portout, VDG60::outB0H },
		{ 0xb1, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb2, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb3, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb4, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb5, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb6, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb7, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb8, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xb9, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xba, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbb, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbc, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbd, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbe, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0xbf, IOBus::portout, VDG60::outB0H },	// イメージ
		{ 0, 0, 0 } };
	if( !iom->Connect( STATIC_CAST( VDG60*, vdg ), c_vdg ) ) return false;
	
	// PSG -----
	const static IOBus::Connector c_psg[] = {
		{ 0xa0, IOBus::portout, PSG6::outA0H },
		{ 0xa1, IOBus::portout, PSG6::outA1H },
		{ 0xa3, IOBus::portout, PSG6::outA3H },
		{ 0xa4, IOBus::portout, PSG6::outA0H },	// イメージ
		{ 0xa5, IOBus::portout, PSG6::outA1H },	// イメージ
		{ 0xa7, IOBus::portout, PSG6::outA3H },	// イメージ
		{ 0xa8, IOBus::portout, PSG6::outA0H },	// イメージ
		{ 0xa9, IOBus::portout, PSG6::outA1H },	// イメージ
		{ 0xab, IOBus::portout, PSG6::outA3H },	// イメージ
		{ 0xac, IOBus::portout, PSG6::outA0H },	// イメージ
		{ 0xad, IOBus::portout, PSG6::outA1H },	// イメージ
		{ 0xaf, IOBus::portout, PSG6::outA3H },	// イメージ
		{ 0xa2, IOBus::portin,  PSG6::inA2H },
		{ 0xa6, IOBus::portin,  PSG6::inA2H },	// イメージ
		{ 0xaa, IOBus::portin,  PSG6::inA2H },	// イメージ
		{ 0xae, IOBus::portin,  PSG6::inA2H },	// イメージ
		{ 0, 0, 0 }	};
	if( !iom->Connect( psg, c_psg ) ) return false;
	int i=0;
	while( c_psg[i].bank ){						// ウェイト設定
		if( c_psg[i].rule == IOBus::portout ) iom->SetOutWait( c_psg[i].bank, 1 );
		else								  iom->SetInWait( c_psg[i].bank, 1 );
		i++;
	}
	if( !psg->Init( PSG_CLOCK60, cnfg->GetSampleRate() ) ) return false;
	
	// 8255(Z80側) -----
	const static IOBus::Connector c_8255[] = {
		{ 0x90, IOBus::portout, PIO6::out90H },
		{ 0x91, IOBus::portout, PIO6::out91H },
		{ 0x92, IOBus::portout, PIO6::out92H },
		{ 0x93, IOBus::portout, PIO6::out93H },
		{ 0x94, IOBus::portout, PIO6::out90H },	// イメージ
		{ 0x95, IOBus::portout, PIO6::out91H },	// イメージ
		{ 0x96, IOBus::portout, PIO6::out92H },	// イメージ
		{ 0x97, IOBus::portout, PIO6::out93H },	// イメージ
		{ 0x98, IOBus::portout, PIO6::out90H },	// イメージ
		{ 0x99, IOBus::portout, PIO6::out91H },	// イメージ
		{ 0x9a, IOBus::portout, PIO6::out92H },	// イメージ
		{ 0x9b, IOBus::portout, PIO6::out93H },	// イメージ
		{ 0x9c, IOBus::portout, PIO6::out90H },	// イメージ
		{ 0x9d, IOBus::portout, PIO6::out91H },	// イメージ
		{ 0x9e, IOBus::portout, PIO6::out92H },	// イメージ
		{ 0x9f, IOBus::portout, PIO6::out93H },	// イメージ
		{ 0x90, IOBus::portin,  PIO6::in90H },
		{ 0x92, IOBus::portin,  PIO6::in92H },
		{ 0x93, IOBus::portin,  PIO6::in93H },
		{ 0x94, IOBus::portin,  PIO6::in90H },	// イメージ
		{ 0x96, IOBus::portin,  PIO6::in92H },	// イメージ
		{ 0x97, IOBus::portin,  PIO6::in93H },	// イメージ
		{ 0x98, IOBus::portin,  PIO6::in90H },	// イメージ
		{ 0x9a, IOBus::portin,  PIO6::in92H },	// イメージ
		{ 0x9b, IOBus::portin,  PIO6::in93H },	// イメージ
		{ 0x9c, IOBus::portin,  PIO6::in90H },	// イメージ
		{ 0x9e, IOBus::portin,  PIO6::in92H },	// イメージ
		{ 0x9f, IOBus::portin,  PIO6::in93H },	// イメージ
		{ 0, 0, 0 }	};
	if( !iom->Connect( pio, c_8255 ) ) return false;
	
	// 8255(SUB CPU側) -----
	const static IOBus::Connector c_8255s[] = {
		{ IO8049_BUS, IOBus::portout, PIO6::outPBH },
		{ IO8049_BUS, IOBus::portin,  PIO6::inPBH  },
		{ IO8049_T0,  IOBus::portin,  PIO6::inIBF  },
		{ IO8049_INT, IOBus::portin,  PIO6::inOBF  },
		{ 0, 0, 0 }	};
	if( !ios->Connect( pio, c_8255s ) ) return false;
	
	// DISK -----
	if( cnfg->GetFddNum() ){
		const static IOBus::Connector c_disk[] = {
			{ 0xd1, IOBus::portout, DSK60::outD1H },
			{ 0xd2, IOBus::portout, DSK60::outD2H },
			{ 0xd3, IOBus::portout, DSK60::outD3H },
			{ 0xd5, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xd6, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xd7, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xd9, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xda, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xdb, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xdd, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xde, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xdf, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xd0, IOBus::portin,  DSK60::inD0H },
			{ 0xd1, IOBus::portin,  DSK60::inD1H },
			{ 0xd2, IOBus::portin,  DSK60::inD2H },
			{ 0xd3, IOBus::portin,  DSK60::inD2H },		// イメージ?
			{ 0xd4, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xd5, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xd6, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xd7, IOBus::portin,  DSK60::inD2H },		// イメージ?
			{ 0xd8, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xd9, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xda, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xdb, IOBus::portin,  DSK60::inD2H },		// イメージ?
			{ 0xdc, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xdd, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xde, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xdf, IOBus::portin,  DSK60::inD2H },		// イメージ?
			{ 0, 0, 0 }	};
		if( !iom->Connect( STATIC_CAST( DSK60*, disk ), c_disk ) ) return false;
		if( !disk->Init( cnfg->GetFddNum() ) ) return false;
	}
	
	// CMT(LOAD) -----
	const static IOBus::Connector c_cmt[] = {
		{ 0xb0, IOBus::portout, CMTL::outB0H },
		{ 0xb1, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb2, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb3, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb4, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb5, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb6, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb7, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb8, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xb9, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xba, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbb, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbc, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbd, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbe, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0xbf, IOBus::portout, CMTL::outB0H },	// イメージ
		{ 0, 0, 0 }	};
	if( !iom->Connect( cmtl, c_cmt ) ) return false;
	
	// オプション機能 -----
	// 戦士のカートリッジ対応
	if( cnfg->GetUseSoldier() ){
		const static IOBus::Connector c_soldier[] = {
			{ 0x7f, IOBus::portout, MEM60::out7FH },
			{ 0, 0, 0 }	};
		if( !iom->Connect( STATIC_CAST( MEM60*, mem ), c_soldier ) ) return false;
	}
	
	return true;
}

bool VM62::Init( CFG6 *cnfg )
{
	PRINTD( VM_LOG, "[VM][Init]\n" );
	
	if( !cnfg ) return false;
	
	// 各種オブジェクト確保
	cpus   = new SUB62( this, DEV_ID("8049") );						// SUB CPU
	intr   = new INT62( this, DEV_ID("INTR") );						// 割込み
	mem    = new MEM62( DEV_ID("MEM1"), cnfg->GetUseExtRam() );		// メモリ
	vdg    = new VDG62( this, DEV_ID("VDG2") );						// VDG
	key    = new KEY6 ( this, DEV_ID("KEYB") );						// キー
	voice  = new VCE6 ( this, DEV_ID("VOIC") );						// 音声合成
	disk   = new DSK60( this, DEV_ID("DISK") );						// DISK
	
	if( !cpus || !intr || !mem || !vdg || !key || !voice || !disk )
		return false;
	
	// 共通部分初期化
	if( !VM6::InitCommon( cnfg ) ) return false;
	
	// 各種オブジェクト初期化とデバイス接続
	
	// SUB CPU -----
	cpus->Reset();
	
	// 割込み -----
	const static IOBus::Connector c_intr[] = {
		{ 0xb0, IOBus::portout, INT62::outB0H },
		{ 0xf3, IOBus::portout, INT62::outF3H },
		{ 0xf4, IOBus::portout, INT62::outF4H },
		{ 0xf5, IOBus::portout, INT62::outF5H },
		{ 0xf6, IOBus::portout, INT62::outF6H },
		{ 0xf7, IOBus::portout, INT62::outF7H },
		{ 0xf3, IOBus::portin,  INT62::inF3H },
		{ 0xf4, IOBus::portin,  INT62::inF4H },
		{ 0xf5, IOBus::portin,  INT62::inF5H },
		{ 0xf6, IOBus::portin,  INT62::inF6H },
		{ 0xf7, IOBus::portin,  INT62::inF7H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( STATIC_CAST( INT62*, intr ), c_intr ) ) return false;
	
	// メモリ -----
	const static IOBus::Connector c_memory[] = {
		{ 0xc1, IOBus::portout, MEM62::outC1H },
		{ 0xc2, IOBus::portout, MEM62::outC2H },
		{ 0xc3, IOBus::portout, MEM62::outC3H },
		{ 0xf0, IOBus::portout, MEM62::outF0H },
		{ 0xf1, IOBus::portout, MEM62::outF1H },
		{ 0xf2, IOBus::portout, MEM62::outF2H },
		{ 0xf3, IOBus::portout, MEM62::outF3H },
		{ 0xf8, IOBus::portout, MEM62::outF8H },
		{ 0xf0, IOBus::portin,  MEM62::inF0H },
		{ 0xf1, IOBus::portin,  MEM62::inF1H },
		{ 0xf2, IOBus::portin,  MEM62::inF2H },
		{ 0xf3, IOBus::portin,  MEM62::inF3H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( STATIC_CAST( MEM62*, mem ), c_memory ) ) return false;
	
	// VDG -----
	const static IOBus::Connector c_vdg[] = {
		{ 0xb0, IOBus::portout, VDG62::outB0H },
		{ 0xc0, IOBus::portout, VDG62::outC0H },
		{ 0xc1, IOBus::portout, VDG62::outC1H },
		{ 0xc1, IOBus::portin,  VDG62::inC1H },
		{ 0, 0, 0 } };
	if( !iom->Connect( STATIC_CAST( VDG62*, vdg ), c_vdg ) ) return false;
	
	// PSG -----
	const static IOBus::Connector c_psg[] = {
		{ 0xa0, IOBus::portout, PSG6::outA0H },
		{ 0xa1, IOBus::portout, PSG6::outA1H },
		{ 0xa3, IOBus::portout, PSG6::outA3H },
		{ 0xa2, IOBus::portin,  PSG6::inA2H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( psg, c_psg ) ) return false;
	int i=0;
	while( c_psg[i].bank ){						// ウェイト設定
		if( c_psg[i].rule == IOBus::portout ) iom->SetOutWait( c_psg[i].bank, 1 );
		else								  iom->SetInWait( c_psg[i].bank, 1 );
		i++;
	}
	if( !psg->Init( PSG_CLOCK62, cnfg->GetSampleRate() ) ) return false;
	
	// 8255(Z80側) -----
	const static IOBus::Connector c_8255[] = {
		{ 0x90, IOBus::portout, PIO6::out90H },
		{ 0x91, IOBus::portout, PIO6::out91H },
		{ 0x92, IOBus::portout, PIO6::out92H },
		{ 0x93, IOBus::portout, PIO6::out93H },
		{ 0x90, IOBus::portin,  PIO6::in90H },
		{ 0x92, IOBus::portin,  PIO6::in92H },
		{ 0x93, IOBus::portin,  PIO6::in93H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( pio, c_8255 ) ) return false;
	
	// 8255(SUB CPU側) -----
	const static IOBus::Connector c_8255s[] = {
		{ IO8049_BUS, IOBus::portout, PIO6::outPBH },
		{ IO8049_BUS, IOBus::portin,  PIO6::inPBH  },
		{ IO8049_T0,  IOBus::portin,  PIO6::inIBF  },
		{ IO8049_INT, IOBus::portin,  PIO6::inOBF  },
		{ 0, 0, 0 }	};
	if( !ios->Connect( pio, c_8255s ) ) return false;
	
	// 音声合成 -----
	const static IOBus::Connector c_voice[] = {
		{ 0xe0, IOBus::portout, VCE6::outE0H },
		{ 0xe2, IOBus::portout, VCE6::outE2H },
		{ 0xe3, IOBus::portout, VCE6::outE3H },
		{ 0xe4, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xe6, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xe7, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xe8, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xea, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xeb, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xec, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xee, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xef, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xe0, IOBus::portin,  VCE6::inE0H },
		{ 0xe2, IOBus::portin,  VCE6::inE2H },
		{ 0xe3, IOBus::portin,  VCE6::inE3H },
		{ 0xe4, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xe6, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xe7, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0xe8, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xea, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xeb, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0xec, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xee, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xef, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0, 0, 0 }	};
	if( !iom->Connect( voice, c_voice ) ) return false;
	if( !voice->Init( cnfg->GetSampleRate(), cnfg->GetWavePath() ) ) return false;
	voice->SetVolume( cnfg->GetVoiceVol() );	// 音量設定
	
	// DISK -----
	if( cnfg->GetFddNum() ){
		const static IOBus::Connector c_disk[] = {
			{ 0xd1, IOBus::portout, DSK60::outD1H },
			{ 0xd2, IOBus::portout, DSK60::outD2H },
			{ 0xd3, IOBus::portout, DSK60::outD3H },
			{ 0xd5, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xd6, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xd7, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xd9, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xda, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xdb, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xdd, IOBus::portout, DSK60::outD1H },	// イメージ
			{ 0xde, IOBus::portout, DSK60::outD2H },	// イメージ
			{ 0xdf, IOBus::portout, DSK60::outD3H },	// イメージ
			{ 0xd0, IOBus::portin,  DSK60::inD0H },
			{ 0xd1, IOBus::portin,  DSK60::inD1H },
			{ 0xd2, IOBus::portin,  DSK60::inD2H },
			{ 0xd3, IOBus::portin,  DSK60::inD2H },		// イメージ?
			{ 0xd4, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xd5, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xd6, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xd7, IOBus::portin,  DSK60::inD2H },		// イメージ?
			{ 0xd8, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xd9, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xda, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xdb, IOBus::portin,  DSK60::inD2H },		// イメージ?
			{ 0xdc, IOBus::portin,  DSK60::inD0H },		// イメージ
			{ 0xdd, IOBus::portin,  DSK60::inD1H },		// イメージ
			{ 0xde, IOBus::portin,  DSK60::inD2H },		// イメージ
			{ 0xdf, IOBus::portin,  DSK60::inD2H },		// イメージ?
			{ 0, 0, 0 }	};
		if( !iom->Connect( STATIC_CAST( DSK60*, disk ), c_disk ) ) return false;
		if( !disk->Init( cnfg->GetFddNum() ) ) return false;
	}
	
	// CMT(LOAD) -----
	const static IOBus::Connector c_cmt[] = {
		{ 0xb0, IOBus::portout, CMTL::outB0H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( cmtl, c_cmt ) ) return false;
	
	// オプション機能 -----
	// 戦士のカートリッジ対応
	if( cnfg->GetUseSoldier() ){
		const static IOBus::Connector c_soldier[] = {
			{ 0x7f, IOBus::portout, MEM62::out7FH },
			{ 0, 0, 0 }	};
		if( !iom->Connect( STATIC_CAST( MEM62*, mem ), c_soldier ) ) return false;
	}
	
	return true;
}

bool VM66::Init( CFG6 *cnfg )
{
	PRINTD( VM_LOG, "[VM][Init]\n" );
	
	if( !cnfg ) return false;
	
	// 各種オブジェクト確保
	cpus   = new SUB62( this, DEV_ID("8049") );						// SUB CPU
	intr   = new INT62( this, DEV_ID("INTR") );						// 割込み
	mem    = new MEM66( DEV_ID("MEM1"), cnfg->GetUseExtRam() );		// メモリ
	vdg    = new VDG62( this, DEV_ID("VDG2") );						// VDG
	key    = new KEY6 ( this, DEV_ID("KEYB") );						// キー
	voice  = new VCE6 ( this, DEV_ID("VOIC") );						// 音声合成
	disk   = new DSK66( this, DEV_ID("DISK") );						// DISK
	
	if( !cpus || !intr || !mem || !vdg || !key  || !voice || !disk )
		return false;
	
	// 共通部分初期化
	if( !VM6::InitCommon( cnfg ) ) return false;
	
	// 各種オブジェクト初期化とデバイス接続
	
	// SUB CPU -----
	cpus->Reset();
	
	// 割込み -----
	const static IOBus::Connector c_intr[] = {
		{ 0xb0, IOBus::portout, INT62::outB0H },
		{ 0xf3, IOBus::portout, INT62::outF3H },
		{ 0xf4, IOBus::portout, INT62::outF4H },
		{ 0xf5, IOBus::portout, INT62::outF5H },
		{ 0xf6, IOBus::portout, INT62::outF6H },
		{ 0xf7, IOBus::portout, INT62::outF7H },
		{ 0xf3, IOBus::portin,  INT62::inF3H },
		{ 0xf4, IOBus::portin,  INT62::inF4H },
		{ 0xf5, IOBus::portin,  INT62::inF5H },
		{ 0xf6, IOBus::portin,  INT62::inF6H },
		{ 0xf7, IOBus::portin,  INT62::inF7H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( STATIC_CAST( INT62*, intr ), c_intr ) ) return false;
	
	// メモリ -----
	const static IOBus::Connector c_memory[] = {
		{ 0xc1, IOBus::portout, MEM62::outC1H },
		{ 0xc2, IOBus::portout, MEM62::outC2H },
		{ 0xc3, IOBus::portout, MEM62::outC3H },
		{ 0xf0, IOBus::portout, MEM62::outF0H },
		{ 0xf1, IOBus::portout, MEM62::outF1H },
		{ 0xf2, IOBus::portout, MEM62::outF2H },
		{ 0xf3, IOBus::portout, MEM62::outF3H },
		{ 0xf8, IOBus::portout, MEM62::outF8H },
		{ 0xf0, IOBus::portin,  MEM62::inF0H },
		{ 0xf1, IOBus::portin,  MEM62::inF1H },
		{ 0xf2, IOBus::portin,  MEM62::inF2H },
		{ 0xf3, IOBus::portin,  MEM62::inF3H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( STATIC_CAST( MEM66*, mem ), c_memory ) ) return false;
	
	// VDG -----
	const static IOBus::Connector c_vdg[] = {
		{ 0xb0, IOBus::portout, VDG62::outB0H },
		{ 0xc0, IOBus::portout, VDG62::outC0H },
		{ 0xc1, IOBus::portout, VDG62::outC1H },
		{ 0xc1, IOBus::portin,  VDG62::inC1H },
		{ 0, 0, 0 } };
	if( !iom->Connect( STATIC_CAST( VDG62*, vdg ), c_vdg ) ) return false;
	
	// PSG -----
	const static IOBus::Connector c_psg[] = {
		{ 0xa0, IOBus::portout, PSG6::outA0H },
		{ 0xa1, IOBus::portout, PSG6::outA1H },
		{ 0xa3, IOBus::portout, PSG6::outA3H },
		{ 0xa2, IOBus::portin,  PSG6::inA2H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( psg, c_psg ) ) return false;
	int i=0;
	while( c_psg[i].bank ){						// ウェイト設定
		if( c_psg[i].rule == IOBus::portout ) iom->SetOutWait( c_psg[i].bank, 1 );
		else								  iom->SetInWait( c_psg[i].bank, 1 );
		i++;
	}
	if( !psg->Init( PSG_CLOCK62, cnfg->GetSampleRate() ) ) return false;
	
	// 8255(Z80側) -----
	const static IOBus::Connector c_8255[] = {
		{ 0x90, IOBus::portout, PIO6::out90H },
		{ 0x91, IOBus::portout, PIO6::out91H },
		{ 0x92, IOBus::portout, PIO6::out92H },
		{ 0x93, IOBus::portout, PIO6::out93H },
		{ 0x90, IOBus::portin,  PIO6::in90H },
		{ 0x92, IOBus::portin,  PIO6::in92H },
		{ 0x93, IOBus::portin,  PIO6::in93H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( pio, c_8255 ) ) return false;
	
	// 8255(SUB CPU側) -----
	const static IOBus::Connector c_8255s[] = {
		{ IO8049_BUS, IOBus::portout, PIO6::outPBH },
		{ IO8049_BUS, IOBus::portin,  PIO6::inPBH  },
		{ IO8049_T0,  IOBus::portin,  PIO6::inIBF  },
		{ IO8049_INT, IOBus::portin,  PIO6::inOBF  },
		{ 0, 0, 0 }	};
	if( !ios->Connect( pio, c_8255s ) ) return false;
	
	// 音声合成 -----
	const static IOBus::Connector c_voice[] = {
		{ 0xe0, IOBus::portout, VCE6::outE0H },
		{ 0xe2, IOBus::portout, VCE6::outE2H },
		{ 0xe3, IOBus::portout, VCE6::outE3H },
		{ 0xe4, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xe6, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xe7, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xe8, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xea, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xeb, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xec, IOBus::portout, VCE6::outE0H },	// イメージ
		{ 0xee, IOBus::portout, VCE6::outE2H },	// イメージ
		{ 0xef, IOBus::portout, VCE6::outE3H },	// イメージ
		{ 0xe0, IOBus::portin,  VCE6::inE0H },
		{ 0xe2, IOBus::portin,  VCE6::inE2H },
		{ 0xe3, IOBus::portin,  VCE6::inE3H },
		{ 0xe4, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xe6, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xe7, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0xe8, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xea, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xeb, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0xec, IOBus::portin,  VCE6::inE0H },	// イメージ
		{ 0xee, IOBus::portin,  VCE6::inE2H },	// イメージ
		{ 0xef, IOBus::portin,  VCE6::inE3H },	// イメージ
		{ 0, 0, 0 }	};
	if( !iom->Connect( voice, c_voice ) ) return false;
	if( !voice->Init( cnfg->GetSampleRate(), cnfg->GetWavePath() ) ) return false;
	voice->SetVolume( cnfg->GetVoiceVol() );	// 音量設定
	
	// DISK -----
	const static IOBus::Connector c_disk[] = {
		{ 0xb1, IOBus::portout, DSK66::outB1H },
		{ 0xb3, IOBus::portout, DSK66::outB3H },
		{ 0xd0, IOBus::portout, DSK66::outD0H },
		{ 0xd1, IOBus::portout, DSK66::outD1H },
		{ 0xd2, IOBus::portout, DSK66::outD2H },
		{ 0xd3, IOBus::portout, DSK66::outD3H },
		{ 0xd6, IOBus::portout, DSK66::outD6H },
		{ 0xd8, IOBus::portout, DSK66::outD8H },
		{ 0xda, IOBus::portout, DSK66::outDAH },
		{ 0xdd, IOBus::portout, DSK66::outDDH },
		{ 0xde, IOBus::portout, DSK66::outDEH },
		{ 0xb2, IOBus::portin,  DSK66::inB2H },
		{ 0xd0, IOBus::portin,  DSK66::inD0H },
		{ 0xd1, IOBus::portin,  DSK66::inD1H },
		{ 0xd2, IOBus::portin,  DSK66::inD2H },
		{ 0xd3, IOBus::portin,  DSK66::inD3H },
		{ 0xd4, IOBus::portin,  DSK66::inD4H },
		{ 0xdc, IOBus::portin,  DSK66::inDCH },
		{ 0xdd, IOBus::portin,  DSK66::inDDH },
		{ 0, 0, 0 }	};
	if( !iom->Connect( STATIC_CAST( DSK66*, disk ), c_disk ) ) return false;
	if( !disk->Init( cnfg->GetFddNum() ) ) return false;
	
	// CMT(LOAD) -----
	const static IOBus::Connector c_cmt[] = {
		{ 0xb0, IOBus::portout, CMTL::outB0H },
		{ 0, 0, 0 }	};
	if( !iom->Connect( cmtl, c_cmt ) ) return false;
	
	// オプション機能 -----
	// 戦士のカートリッジ対応
	if( cnfg->GetUseSoldier() ){
		const static IOBus::Connector c_soldier[] = {
			{ 0x7f, IOBus::portout, MEM62::out7FH },
			{ 0, 0, 0 }	};
		if( !iom->Connect( STATIC_CAST( MEM66*, mem ), c_soldier ) ) return false;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// 共通部分初期化
////////////////////////////////////////////////////////////////
bool VM6::InitCommon( CFG6 *cnfg  )
{
	PRINTD( VM_LOG, "[VM][InitCommon]\n" );
	
	// 各種オブジェクト確保
	evsc   = new cSche( clock * cnfg->GetOverClock() / 100) ;	// イベントスケジューラ
	iom    = new IO6;											// I/O(Z80)
	ios    = new IO6;											// I/O(SUB CPU)
	cpum   = new CPU6 ( this, DEV_ID("CPU1") );					// CPU
	psg    = new PSG6 ( this, DEV_ID("PSG1") );					// PSG
	pio    = new PIO6 ( this, DEV_ID("8255") );					// 8255
	cmtl   = new CMTL ( this, DEV_ID("TAPE") );					// CMT(LOAD)
	cmts   = new CMTS ( this, DEV_ID("SAVE") );					// CMT(SAVE)
	
	if( !evsc || !iom || !ios || !cpum || !psg || !pio || !cmtl || !cmts )
			return false;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	bp     = new BPoint();										// ブレークポイント
	if( !bp ) return false;
	bp->ClearStatus();
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	// I/O(Z80)　-----
	if( !iom->Init( 256 ) ) return false;
	
	// I/O(SUB CPU)　-----
	if( !ios->Init( 10 ) ) return false;
	
	// 割込み -----
	intr->Reset();
	
	// CPU -----
	cpum->Reset();
	
	// メモリ -----
	if( !mem->Init( cnfg->GetRomPath(), cnfg->GetCheckCRC(), cnfg->GetUseSoldier() ) ) return false;
	if( cnfg->GetRomPatch() ) mem->Patch();
	if( *(cnfg->GetExtRomFile()) ) if( !mem->MountExtRom( cnfg->GetExtRomFile() ) ) return false;
	
	// VDG -----
	if( !vdg->Init() ) return false;
	vdg->SetPalette( el->GPal.colors );			// 念のためパレット設定
	vdg->SetMode4Color( cnfg->GetMode4Color() );
	
	// PSG -----
	psg->SetVolume( cnfg->GetPsgVol() );		// 音量設定
	psg->SetLPF( cnfg->GetPsgLPF() );			// ローパスフィルタ カットオフ周波数設定
	
	// 8255 -----
	pio->Reset();
	pio->cPRT::Init( cnfg->GetPrinterFile() );
	
	// キー -----
	if( !key->Init( cnfg->GetKeyRepeat() ) ) return false;
	VKeyConv *vk;
	if( cnfg->GetVKeyDef( &vk ) )				// キー定義配列取得
		key->SetVKeySymbols( vk );				// 仮想キーコード -> P6キーコード 設定
	
	// CMT(LOAD) -----
	if( !cmtl->Init( cnfg->GetSampleRate() ) ) return false;
	cmtl->SetVolume( cnfg->GetCmtVol() );		// 音量設定
	cmtl->SetLPF( DEFAULT_TAPELPF );			// ローパスフィルタ カットオフ周波数設定
	cmtl->SetBoost( cnfg->GetBoostUp() );
	cmtl->SetMaxBoost( cnfg->GetMaxBoost1(), cnfg->GetMaxBoost2() );
	
	// CMT(SAVE) -----
	if( !cmts->Init( cnfg->GetSaveFile() ) ) return false;
	
	return true;
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void VM6::Reset( void )
{
	PRINTD( VM_LOG, "[VM][Reset]\n" );
	
	cpum->Reset();
	intr->Reset();
	mem->Reset();
	cpus->Reset();
	pio->Reset();
	psg->Reset();
	disk->Reset();
	cmtl->Reset();
	// システムディスクが入っていたらTAPEのオートスタート無効
	if( !disk->IsSystem(0) && !disk->IsSystem(1) ) cmtl->SetAutoStart( el->cfg->GetModel() );
	if( voice ) voice->Reset();
}


////////////////////////////////////////////////////////////////
// 1命令実行
////////////////////////////////////////////////////////////////
int VM6::Emu( void )
{
	PRINTD( VM_LOG, "[VM][Emu]\n" );
	
	return cpum->Exec();	// CPU 1命令実行
}


////////////////////////////////////////////////////////////////
// CPUクロック取得
////////////////////////////////////////////////////////////////
int VM6::GetCPUClock( void )
{
	PRINTD( VM_LOG, "[VM][GetCPUClock]\n" );
	
	return clock;
}
