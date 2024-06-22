/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <array>
#include <string>
#include <vector>

#include "device.h"
#include "ini.h"
#include "io.h"
#include "memblk.h"
#include "typedef.h"




/////////////////////////////////////////////////////////////////////////////

// ROM情報構造体
struct ROMINFO {
	const std::string FileName;		// ファイル名
	const DWORD Size;				// サイズ
	const DWORD Crc;				// CRC32
};


/////////////////////////////////////////////////////////////////////////////

// ROMセット情報取得
const std::vector<std::vector<ROMINFO>>& GetRomSetList( const int );

/////////////////////////////////////////////////////////////////////////////


class MEMB {
protected:
	// メモリ情報構造体
	struct MEMINFO {
		const std::vector<ROMINFO>& Rinfo;	// ROM情報へのポインタ
		const DWORD Size;					// サイズ
		const BYTE Init;					// 初期化データ
		const int Wait;						// アクセスウェイト
	};
	
	// メモリ情報
	static const MEMINFO IEMPTROM;
	static const MEMINFO IEMPTRAM;
	
	bool AllocMemory( MemCells&, const MEMINFO*, const P6VPATH&, bool );	// メモリ確保とROMファイル読込み


public:
	MEMB();
	virtual ~MEMB();
	
};


class MEM6 : public MEMB, public Device, public IDoko {
protected:
	// メモリ情報テーブル構造体
	struct MEMINFOTABLE {
		const MEMINFO* System1;
		const MEMINFO* System2;
		const MEMINFO* CGRom1;
		const MEMINFO* CGRom2;
		const MEMINFO* Kanji;
		const MEMINFO* Voice;
		const MEMINFO* IntRam;
		
		const MEMINFO* ExtRom;			// 拡張カートリッジ　切り分けたい
		const MEMINFO* ExtRam;			// 拡張カートリッジ　切り分けたい
		
		MEMINFOTABLE() :
			System1( &IEMPTROM ), System2( &IEMPTROM ), CGRom1( &IEMPTROM ), CGRom2( &IEMPTROM ),
			Kanji( &IEMPTROM ), Voice( &IEMPTROM ), IntRam( &IEMPTRAM ),
			ExtRom( &IEMPTROM ), ExtRam( &IEMPTROM ) {}
	};
	
	MEMINFOTABLE MemTable;				// 内部メモリ情報テーブル
	
	MemCells SysRom1;					// BASIC ROM	ALL (64,68の時はSystem ROM1)
	MemCells SysRom2;					// System ROM2	64,68
	MemCells CGRom1;					// CG ROM1		ALL
	MemCells CGRom2;					// CG ROM2		62,66
	MemCells KanjiRom;					// 漢字 ROM		62,66
	MemCells VoiceRom;					// 音声合成 ROM	62,66
	MemCells IntRam;					// 内部RAM		ALL
	
	std::vector<MemBlock> IRom;			// 内部ROMブロック
	std::vector<MemBlock> IRam;			// 内部RAMブロック
	
	std::array<MemBlock*,8> RBLK1;		// リード時メモリブロックポインタ(8KB*8) 1段目
	std::array<MemBlock*,8> WBLK1;		// ライト時メモリブロックポインタ(8KB*8) 1段目
	std::array<MemBlock*,8> RBLK2;		// リード時メモリブロックポインタ(8KB*8) 2段目
	std::array<MemBlock*,8> WBLK2;		// ライト時メモリブロックポインタ(8KB*8) 2段目
	
	bool CGBank;						// CG ROM BANK	true:有効 false:無効
	int M1Wait;							// M1ウェイト
	
	// for 62,66,64,68 ------------------------------------------------------
	std::array<BYTE,3> Rf;				// メモリコントローラ内部レジスタ
	// ----------------------------------------------------------------------
	
	virtual bool AllocMemorySpec( const P6VPATH&, bool ) = 0;	// 内部メモリ確保とROMファイル読込み(機種別)
	virtual void SetRamValue() = 0;								// 内部RAMの初期値を設定
	virtual bool InitIntSpec() = 0;								// 内部メモリ初期化(機種別)
	virtual void SetMemBlockR( BYTE, BYTE ) = 0;				// メモリリード時のメモリブロック指定(62,66)
	virtual void SetMemBlockW( BYTE ) = 0;						// メモリライト時のメモリブロック指定(62,66)
	
	// メモリブロック用関数 -------------------------------------------------
	virtual const std::string& GetNameCommon( WORD, bool );		// 汎用 メモリブロック名取得
	virtual BYTE CommonRead( MemCell*, WORD, int* );			// 汎用 読込み
	virtual void CommonWrite( MemCell*, WORD, BYTE, int* );		// 汎用 書込み
	// ----------------------------------------------------------------------
	
	// 拡張カートリッジ =====================================================
	// メモリ情報
	static const MEMINFO IEXTROM8;
	static const MEMINFO IEXTROM16;
	static const MEMINFO IEXTROM128;
	static const MEMINFO IEXTROM512;
	static const MEMINFO IEXTROM8M;
	static const MEMINFO IEXTRAM16;
	static const MEMINFO IEXTRAM32;
	static const MEMINFO IEXTRAM64;
	static const MEMINFO IEXTRAM128;
	static const MEMINFO IEXTRAM512;
	
	static const MEMINFO IEXBASIC;		// 拡張BASIC
	static const MEMINFO IEXKANJI;		// 拡張漢字
	static const MEMINFO IEXVOICE;		// ボイスシンセサイザー
	
	// デバイスコネクタ
	static const std::vector<IOBus::Connector> c_dummy;		// Dummy
	static const std::vector<IOBus::Connector> c_exvoice;	// ボイスシンセサイザー
	static const std::vector<IOBus::Connector> c_exfm;		// FM音源カートリッジ
	static const std::vector<IOBus::Connector> c_exkanji;	// 拡張漢字ROMカートリッジ
	static const std::vector<IOBus::Connector> c_soldier1;	// 戦士のカートリッジ
	static const std::vector<IOBus::Connector> c_soldier2;	// 戦士のカートリッジmkⅡ
	static const std::vector<IOBus::Connector> c_soldier3;	// 戦士のカートリッジmkⅢ
	
	MemCells ExtRom;					// 外部ROM
	MemCells ExtRam;					// 外部RAM
	
	std::vector<MemBlock> EMem;			// 外部ROM/RAMブロック
	
	WORD ExCart;						// 0:なし
	P6VPATH FilePath;					// 拡張ROMファイルフルパス
	
	bool AllocMemoryExt( const P6VPATH&, bool );	// 外部メモリ確保とSystemROMファイル読込み
	void AddDeviceDescriptorExt();		// 拡張カートリッジ デバイスディスクリプタ追加
	
	// 拡張漢字ROMカートリッジ ----------------------------------------------
	WORD Kaddr;							// 漢字ROMアドレス
	bool Kenable;						// 漢字ROMチップイネーブル
	
	// 戦士のカートリッジ ---------------------------------------------------
	bool Sol60Mode;						// 初代機モード　true:有効 false:無効
	bool CS01R;							// 初代機 0000-3FFFH 読込みフラグ
	bool CS01W;							// 初代機 0000-3FFFH 書込みフラグ
	bool CS02W;							// 初代機 4000-7FFFH 書込みフラグ
	std::array<BYTE,8> SolBank;			// メモリバンクレジスタ
	int SolBankSet;						// ROMバンクセット
	
	void SetSol2Bank( BYTE, BYTE );								// 戦士のカートリッジmkⅡ/Ⅲ メモリバンクレジスタ設定
	const std::string& Sol2GetName( WORD, bool );				// 戦士のカートリッジmkⅡ/Ⅲ メモリブロック名取得
	BYTE Sol2Read( MemCell*, WORD, int* = nullptr );			// 戦士のカートリッジmkⅡ/Ⅲ 読込み
	void Sol2Write( MemCell*, WORD, BYTE, int* = nullptr );		// 戦士のカートリッジmkⅡ/Ⅲ 書込み
	BYTE Sol2SccRead( MemCell*, WORD, int* = nullptr );			// 戦士のカートリッジmkⅡ SCC読込み
	void Sol2SccWrite( MemCell*, WORD, BYTE, int* = nullptr );	// 戦士のカートリッジmkⅡ SCC書込み
	
	// I/Oアクセス関数 ------------------------------------------------------
	// 拡張漢字ROMカートリッジ ----------------------------------------------
	void OutFCH( int, BYTE );
	void OutFFH( int, BYTE );
	BYTE InFDH( int );
	BYTE InFEH( int );
	// ボイスシンセサイザー -------------------------------------------------
	void Out70H( int, BYTE );
	void Out72H( int, BYTE );
	void Out73H( int, BYTE );
	void Out74H( int, BYTE );
	BYTE In70H( int );
	BYTE In72H( int );
	BYTE In73H( int );
	// FM音源カートリッジ ---------------------------------------------------
	void Out70Hf( int, BYTE );
	void Out71Hf( int, BYTE );
	BYTE In72Hf( int );
	BYTE In73Hf( int );
	// 戦士のカートリッジ ---------------------------------------------------
	void Out06H( int, BYTE );
	void Out07H( int, BYTE );	// mkⅢ
	void Out3xH( int, BYTE );
	void Out7FH( int, BYTE );
	void OutF0Hs( int, BYTE );
	void OutF2Hs( int, BYTE );
	// ======================================================================

public:
	MEM6( VM6*, const ID& );
	virtual ~MEM6();
	
	bool AllocMemoryInt( const P6VPATH&, bool );		// 内部メモリ確保とROMファイル読込み
	bool InitInt();										// 内部メモリ初期化
	virtual void Reset();								// リセット
	
	virtual BYTE Fetch( WORD, int* = nullptr ) const;	// フェッチ(M1)
	virtual BYTE Read( WORD, int* = nullptr ) const;	// メモリリード
	virtual void Write( WORD, BYTE, int* = nullptr );	// メモリライト
	
	// 8255入出力関連関数
	void SetCGBank( bool );								// CG ROM BANK 選択
	
	// 直接アクセス関数
	BYTE ReadSysRom( WORD ) const;
	BYTE ReadIntRam( WORD ) const;
	virtual BYTE ReadCGrom1( WORD ) const;
	virtual BYTE ReadCGrom2( WORD ) const;
	virtual BYTE ReadCGrom3( WORD ) const;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	virtual const std::string& GetReadMemBlk( int ) const;
	virtual const std::string& GetWriteMemBlk( int ) const;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// 拡張カートリッジ =====================================================
	bool InitExt();										// 外部メモリ初期化
	void ResetExt();									// 外部メモリリセット
	
	bool MountExtRom( const P6VPATH& );					// 拡張ROM マウント
	void UnmountExtRom();								// 拡張ROM アンマウント
	const P6VPATH& GetFile() const;						// 拡張ROM ファイルパス取得
	WORD GetCartridge() const;							// 拡張カートリッジの種類取得
	bool MountExtCart( WORD, const P6VPATH&, bool );	// 拡張カートリッジマウント
	const std::vector<IOBus::Connector>& GetDeviceConnector();	// デバイスコネクタリスト取得
	
	// 直接アクセス関数
	BYTE ReadExtRom( DWORD ) const;
	BYTE ReadExtRam( DWORD ) const;
	
	bool GetReadEnableExt( WORD );		// Read Enable取得 (ROM KILL)
	bool GetWriteEnableExt( WORD );		// Write Enable取得
	// ======================================================================
	
	// デバイスID
	enum IDOut{ out06H=0, out07H, out3xH, out70H, out72H, out73H, out74H, out70Hf, out71Hf, out7FH, outF0Hs, outF2Hs, outFCH, outFFH, out6xH, outC1H, outC2H, outC3H, outF0H, outF1H, outF2H, outF3H, outF8H };
	enum IDIn {  in70H=0,  in72H,  in73H, in72Hf, in73Hf,  inFDH,  inFEH,   in6xH,   inB2H,  inC2H,   inF0H,   inF1H,  inF2H,  inF3H };
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};


class MEM60 : public MEM6 {
private:
	// メモリ情報
	static const MEMINFO ISYSROM1;
	static const MEMINFO ICGROM0;
	static const MEMINFO ICGROM1;
	static const MEMINFO IINTRAM;
	
	bool AllocMemorySpec( const P6VPATH&, bool ) override;		// 内部メモリ確保とROMファイル読込み(機種別)
	void SetRamValue() override;								// 内部RAMの初期値を設定
	bool InitIntSpec() override;								// 内部メモリ初期化(機種別)
	void SetMemBlockR( BYTE, BYTE ) override;					// メモリリード時のメモリブロック指定
	void SetMemBlockW( BYTE ) override;							// メモリライト時のメモリブロック指定
	
	// メモリブロック用関数 -------------------------------------------------
	const std::string& GetNameAreaA60Read( WORD, bool );		// AreaA,B メモリブロック名取得(60)
	BYTE AreaA60Read( MemCell*, WORD, int* );					// AreaA,B 読込み(60)
	void AreaABCD60Write( MemCell*, WORD, BYTE, int* );			// AreaA,B,C,D 書込み(60)
	const std::string& GetNameAreaC60Read( WORD, bool );		// AreaC メモリブロック名取得(60)
	BYTE AreaC60Read( MemCell*, WORD, int* );					// AreaC 読込み(60)
	const std::string& GetNameAreaD60Read( WORD, bool );		// AreaD メモリブロック名取得(60)
	BYTE AreaD60Read( MemCell*, WORD, int* );					// AreaD 読込み(60)
	// ----------------------------------------------------------------------

public:
	MEM60( VM6*, const ID& );
	~MEM60();
};


class MEM61 : public MEM60 {
private:
	// メモリ情報
	static const MEMINFO ISYSROM1;
	static const MEMINFO ICGROM0;
	static const MEMINFO ICGROM1;
	static const MEMINFO IINTRAM;

public:
	MEM61( VM6*, const ID& );
	~MEM61();
};


class MEM62 : public MEM6 {
protected:
	// メモリ情報
	static const MEMINFO ISYSROM1;
	static const MEMINFO ICGROM1;
	static const MEMINFO ICGROM2;
	static const MEMINFO IKANJI;
	static const MEMINFO IVOICE;
	static const MEMINFO IINTRAM;
	
	bool kj_rom;						// 漢字ROM,音声合成ROM 選択用 true:漢字ROM false:音声合成ROM
	bool kj_LR;							// 漢字ROM 左右選択用 true:右 false:左
	bool cgrom;							// CG ROM選択用 true:N60モード false:N60mモード
	bool cgenable;						// CG ROMアクセスフラグ true:アクセス可 false:アクセス不可
	BYTE cgaden;						// CG ROMアドレスイネーブル
	BYTE cgaddr;						// CG ROMアドレス A13,14,15
	BYTE c2acc;							// PortC2Hアクセスフラグ
	
	virtual bool AllocMemorySpec( const P6VPATH&, bool ) override;	// 内部メモリ確保とROMファイル読込み(機種別)
	virtual void SetRamValue() override;							// 内部RAMの初期値を設定
	virtual bool InitIntSpec() override;							// 内部メモリ初期化(機種別)
	virtual void SetMemBlockR( BYTE, BYTE ) override;				// メモリリード時のメモリブロック指定
	void SetMemBlockW( BYTE ) override;								// メモリライト時のメモリブロック指定
	
	void SetWait( BYTE );											// メモリアクセスウェイト設定
	BYTE GetWait() const;											// メモリアクセスウェイト取得
	virtual void SetCGrom( BYTE );									// CG ROM アドレス設定
	void SelectCGrom( bool );										// CG ROM 選択
	void SetKanjiRom( BYTE );										// 漢字ROMおよび音声合成ROM設定
	BYTE GetKanjiRom() const;										// 漢字ROMおよび音声合成ROM取得
	void SetPortC2HAccess( BYTE );									// PortC2Hアクセス設定
	
	// メモリブロック用関数 -------------------------------------------------
	const std::string& GetNameCommon( WORD, bool ) override;		// 汎用 メモリブロック名取得
	BYTE CommonRead( MemCell*, WORD, int* ) override;				// 汎用 読込み
	void IERamWrite( MemCell*, WORD, BYTE, int* = nullptr );		// 内部/外部RAM書込み
	virtual const std::string& KanjiGetName( WORD, bool = false );	// 漢字ROM メモリブロック名取得
	virtual BYTE ReadKanji( MemCell*, WORD, int* = nullptr );		// 漢字ROM読込み
	// ----------------------------------------------------------------------
	
	// I/Oアクセス関数 ------------------------------------------------------
	void OutC1H( int, BYTE );
	void OutC2H( int, BYTE );
	void OutC3H( int, BYTE );
	void OutF0H( int, BYTE );
	void OutF1H( int, BYTE );
	void OutF2H( int, BYTE );
	void OutF3H( int, BYTE );
	void OutF8H( int, BYTE );
	BYTE InC2H( int );
	BYTE InF0H( int );
	BYTE InF1H( int );
	BYTE InF2H( int );
	virtual BYTE InF3H( int );

public:
	MEM62( VM6*, const ID& );
	virtual ~MEM62();
	
	virtual void Reset() override;						// リセット
	
	// 直接アクセス関数
	virtual BYTE ReadCGrom2( WORD ) const override;
	virtual BYTE ReadKanjiRom( WORD ) const;
	virtual BYTE ReadVoiceRom( WORD ) const;
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};


class MEM66 : public MEM62 {
private:
	// メモリ情報
	static const MEMINFO ISYSROM1;
	static const MEMINFO ICGROM1;
	static const MEMINFO ICGROM2;
	static const MEMINFO IKANJI;
	static const MEMINFO IVOICE;
	static const MEMINFO IINTRAM;

public:
	MEM66( VM6*, const ID& );
	~MEM66();
};


class MEM64 : public MEM62 {
protected:
	// メモリ情報
	static const MEMINFO ISYSROM1;
	static const MEMINFO ISYSROM2;
	static const MEMINFO ICGROM1;
	static const MEMINFO IINTRAM;
	
	std::array<MemBlock*,8> RBLKSR;		// リード時メモリブロックポインタ(8KB*8) SRモード用
	std::array<MemBlock*,8> WBLKSR;		// ライト時メモリブロックポインタ(8KB*8) SRモード用
	std::array<BYTE,16> RfSR;			// メモリコントローラ内部レジスタ		 SRモード用
	
	bool AllocMemorySpec( const P6VPATH&, bool ) override;			// 内部メモリ確保とROMファイル読込み(機種別)
	virtual void SetRamValue() override;							// 内部RAMの初期値を設定
	bool InitIntSpec() override;									// 内部メモリ初期化(機種別)
	void SetMemBlockR( BYTE, BYTE ) override;						// メモリリード時のメモリブロック指定
	void SetMemBlockSR( BYTE, BYTE );								// メモリリード/ライト時のメモリブロック指定(64,68)
	
	void SetCGrom( BYTE ) override;									// CG ROM アドレス設定
	
	// メモリブロック用関数 -------------------------------------------------
	const std::string& KanjiGetName( WORD, bool = false ) override;	// 漢字ROM メモリブロック名取得
	BYTE ReadKanji( MemCell*, WORD, int* = nullptr ) override;		// 漢字ROM読込み
	// ----------------------------------------------------------------------
	
	// I/Oアクセス関数 ------------------------------------------------------
	void Out6xH( int, BYTE );
	BYTE In6xH( int );
	virtual BYTE InB2H( int );
	BYTE InF3H( int ) override;

public:
	MEM64( VM6*, const ID& );
	virtual ~MEM64();
	
	void Reset() override;								// リセット
	
	BYTE Fetch( WORD, int* = nullptr ) const override;	// フェッチ(M1)
	BYTE Read( WORD, int* = nullptr ) const override;	// メモリリード
	void Write( WORD, BYTE, int* = nullptr ) override;	// メモリライト
	
	// 直接アクセス関数
	BYTE ReadCGrom1( WORD ) const override;
	BYTE ReadCGrom2( WORD ) const override;
	BYTE ReadCGrom3( WORD ) const override;
	BYTE ReadKanjiRom( WORD ) const override;
	BYTE ReadVoiceRom( WORD ) const override;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	const std::string& GetReadMemBlk( int ) const override;
	const std::string& GetWriteMemBlk( int ) const override;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};


class MEM68 : public MEM64 {
protected:
	void SetRamValue() override;			// 内部RAMの初期値を設定
	
	// I/Oアクセス関数 ------------------------------------------------------
	BYTE InB2H( int ) override;

public:
	MEM68( VM6*, const ID& );
	virtual ~MEM68();
};












class EXTCART : public MEMB, public Device {
protected:
	// メモリ情報テーブル構造体
	struct MEMINFOTABLE {
		const MEMINFO* ExtRom;
		const MEMINFO* ExtRam;
		
		MEMINFOTABLE() :
			ExtRom( &MEM6::IEMPTROM ), ExtRam( &MEM6::IEMPTROM ) {}
	};
	
	// メモリ情報
	static const MEMINFO IEXTROM8;
	static const MEMINFO IEXTROM16;
	static const MEMINFO IEXTROM128;
	static const MEMINFO IEXTROM512;
	static const MEMINFO IEXTROM8M;
	
	static const MEMINFO IEXTRAM16;
	static const MEMINFO IEXTRAM32;
	static const MEMINFO IEXTRAM64;
	static const MEMINFO IEXTRAM128;
	static const MEMINFO IEXTRAM512;
	
	static const MEMINFO IEXBASIC;		// 拡張BASIC
	static const MEMINFO IEXKANJI;		// 拡張漢字
	static const MEMINFO IEXVOICE;		// ボイスシンセサイザー
	
	MEMINFOTABLE MemTable;				// 外部メモリ情報テーブル
	
	MemCells ExtRom;					// 外部ROM		ALL
	MemCells ExtRam;					// 外部RAM		ALL
	
	WORD ExCart;						// 0:なし
	P6VPATH FilePath;					// 拡張ROMファイルフルパス
	
	// 拡張漢字ROMカートリッジ ----------------------------------------------
	WORD Kaddr;							// 漢字ROMアドレス
	bool Kenable;						// 漢字ROMチップイネーブル
	
	// 戦士のカートリッジ ---------------------------------------------------
	bool Sol60Mode;						// 初代機モード　true:有効 false:無効
	bool CS01R;							// 初代機 0000-3FFFH 読込みフラグ
	bool CS01W;							// 初代機 0000-3FFFH 書込みフラグ
	bool CS02W;							// 初代機 4000-7FFFH 書込みフラグ
	std::array<BYTE,8> SolBank;			// メモリバンクレジスタ
	int SolBankSet;						// ROMバンクセット
	
	void SetSol2Bank( BYTE, BYTE );								// 戦士のカートリッジmkⅡ/Ⅲ メモリバンクレジスタ設定
	const std::string& Sol2GetName( WORD, bool );				// 戦士のカートリッジmkⅡ/Ⅲ メモリブロック名取得
	BYTE Sol2Read( MemCell*, WORD, int* = nullptr );			// 戦士のカートリッジmkⅡ/Ⅲ 読込み
	void Sol2Write( MemCell*, WORD, BYTE, int* = nullptr );		// 戦士のカートリッジmkⅡ/Ⅲ 書込み
	BYTE Sol2SccRead( MemCell*, WORD, int* = nullptr );			// 戦士のカートリッジmkⅡ SCC読込み
	void Sol2SccWrite( MemCell*, WORD, BYTE, int* = nullptr );	// 戦士のカートリッジmkⅡ SCC書込み
	
	// I/Oアクセス関数 ------------------------------------------------------
	// 拡張漢字ROMカートリッジ ----------------------------------------------
	void OutFCH( int, BYTE );
	void OutFFH( int, BYTE );
	BYTE InFDH( int );
	BYTE InFEH( int );
	// ボイスシンセサイザー -------------------------------------------------
	void Out70H( int, BYTE );
	void Out72H( int, BYTE );
	void Out73H( int, BYTE );
	void Out74H( int, BYTE );
	BYTE In70H( int );
	BYTE In72H( int );
	BYTE In73H( int );
	// FM音源カートリッジ ---------------------------------------------------
	void Out70Hf( int, BYTE );
	void Out71Hf( int, BYTE );
	BYTE In72Hf( int );
	BYTE In73Hf( int );
	// 戦士のカートリッジ ---------------------------------------------------
	void Out06H( int, BYTE );
	void Out07H( int, BYTE );	// mkⅢ
	void Out3xH( int, BYTE );
	void Out7FH( int, BYTE );
	void OutF0Hs( int, BYTE );
	void OutF2Hs( int, BYTE );
	// ======================================================================
	
	void AddDeviceDescriptorExt();						// 拡張カートリッジ デバイスディスクリプタ追加
	bool AllocMemoryExt( const P6VPATH&, bool );		// 外部メモリ確保とSystemROMファイル読込み

public:
	EXTCART( VM6*, const ID&, DWORD );
	virtual ~EXTCART();
	
	bool InitExt( std::vector<MemBlock>& );				// 外部メモリ初期化
	void ResetExt();									// 外部メモリリセット
	
	bool MountExtRom( const P6VPATH& );					// 拡張ROM マウント
	void UnmountExtRom();								// 拡張ROM アンマウント
	const P6VPATH& GetFile() const;						// 拡張ROM ファイルパス取得
	WORD GetCartridge() const;							// 拡張カートリッジの種類取得
//	bool MountExtCart( WORD, const P6VPATH&, bool );	// 拡張カートリッジマウント
	
	// 直接アクセス関数
	BYTE ReadExtRom( DWORD ) const;
	BYTE ReadExtRam( DWORD ) const;
	
	virtual bool GetReadEnableExt( WORD );		// Read  Enable取得 (ROM KILL)
	virtual bool GetWriteEnableExt( WORD );		// Write Enable取得
	
	// デバイスID
	enum IDOut{ out06H=0, out07H, out3xH, out70H, out72H, out73H, out74H, out70Hf, out71Hf, out7FH, outF0Hs, outF2Hs, outFCH, outFFH };
	enum IDIn {  in70H=0,  in72H,  in73H, in72Hf, in73Hf,  inFDH,  inFEH };
};


#endif	// MEMORY_H_INCLUDED
