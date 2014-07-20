#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"


// メモリブロック数
#define MAXRMB	(22)
#define MAXWMB	(16)

////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
// メモリブロッククラス
class MemBlock {
protected:
	char Name[33];				// メモリブロック名
	BYTE *MB;					// メモリブロックへのポインタ
	int RWait;					// アクセスウェイト(読込み)
	int WWait;					// アクセスウェイト(書込み)
	bool WPt;					// ライトプロテクトフラグ
	
public:
	MemBlock();									// コンストラクタ
	~MemBlock();								// デストラクタ
	
	void SetMemory( const char *, BYTE *, int, int, bool );	// メモリブロック設定
	void SetWait( int, int );					// アクセスウェイト設定
	int GetWait() const;						// アクセスウェイト取得
	void SetProtect( bool );					// ライトプロテクト設定
	bool GetProtect() const;					// ライトプロテクト取得
	
	const char *GetName() const;				// メモリブロック名取得
	
	BYTE Read( WORD, int * = NULL ) const;		// メモリリード
	void Write( WORD, BYTE, int * = NULL ) const;	// メモリライト
};


class MEM6 : public Device, public IDoko {
protected:
	// ROM情報構造体
	typedef struct{
		const char *FileName;	// ファイル名
		DWORD Crc;				// CRC32
	} ROMINFO;
	
	// メモリ情報構造体
	typedef struct{
		const ROMINFO *Rinf;	// ROM情報へのポインタ
		const int Rnum;			// ROM情報の要素数
		DWORD Size;				// サイズ
		BYTE Init;				// 初期化データ
		int WaitR;				// アクセスウェイト(Read)
		int WaitW;				// アクセスウェイト(Write)
	} MEMINFO;
	
	// ROM情報テーブル構造体
	struct ROMINFOTABLE {
		const ROMINFO *System1;
		const ROMINFO *System2;
		const ROMINFO *CGRom1;
		const ROMINFO *CGRom2;
		const ROMINFO *Kanji;
		const ROMINFO *Voice;
		
		ROMINFOTABLE() : System1(NULL), System2(NULL), CGRom1(NULL), CGRom2(NULL),
						 Kanji(NULL), Voice(NULL) {}
	};
	
	// メモリ情報テーブル構造体
	struct MEMINFOTABLE {
		const MEMINFO *EmptRom;
		const MEMINFO *EmptRam;
		const MEMINFO *ExtRom;
		const MEMINFO *IntRam;
		const MEMINFO *ExtRam;
		const MEMINFO *SolRam;
		
		const MEMINFO *System1;
		const MEMINFO *System2;
		const MEMINFO *CGRom1;
		const MEMINFO *CGRom2;
		const MEMINFO *Kanji;
		const MEMINFO *Voice;
		
		MEMINFOTABLE() : EmptRom(NULL), EmptRam(NULL), ExtRom(NULL), IntRam(NULL), ExtRam(NULL),
						 SolRam(NULL), System1(NULL), System2(NULL), CGRom1(NULL), CGRom2(NULL),
						 Kanji(NULL), Voice(NULL) {}
	};
	
	// メモリ情報
	static const MEMINFO IEMPTROM;
	static const MEMINFO IEMPTRAM;
	static const MEMINFO IEXTROM;
	static const MEMINFO IEXTRAM;
	
	ROMINFOTABLE RomTable;		// ROM情報テーブル
	MEMINFOTABLE MemTable;		// メモリ情報テーブル
	
	bool CGBank;				// CG ROM BANK	true:有効 false:無効
	bool UseExtRom;				// 拡張ROM		true:有効 false:無効
	bool UseExtRam;				// 拡張RAM		true:有効 false:無効
	
	BYTE *MainRom;				// BASIC ROM	ALL (64,68の時はSystem ROM1)
	BYTE *SysRom2;				// System ROM2	64,68
	BYTE *ExtRom;				// 拡張 ROM		ALL
	BYTE *CGRom1;				// CG ROM1		ALL
	BYTE *CGRom2;				// CG ROM2		62,66
	BYTE *KanjiRom;				// 漢字 ROM		62,66
	BYTE *VoiceRom;				// 音声合成 ROM	62,66
	
	BYTE *IntRam;				// 内部 RAM		ALL
	BYTE *ExtRam;				// 外部 RAM		ALL
	BYTE *EmptyRom;				// 未実装ROM	ALL
	BYTE *EmptyRam;				// 未実装RAM	ALL
	
	MemBlock RomB[MAXRMB];		// ROMブロック
	MemBlock RamB[MAXWMB];		// RAMブロック
	
	MemBlock *Rm_blk[8];		// リード時メモリブロックポインタ(8KB*8)
	MemBlock *Wm_blk[8];		// ライト時メモリブロックポインタ(8KB*8)
	
	char FilePath[PATH_MAX];	// 拡張ROMファイルフルパス
	int M1Wait;					// M1ウェイト
	bool EnableChkCRC;			// CRCチェック  true:有効 false:無効
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	bool cgrom;					// CG ROM 選択用 true:N60モード false:N60mモード
	bool kj_rom;				// 漢字ROM,音声合成ROM 選択用 true:漢字ROM false:音声合成ROM
	bool kj_LR;					// 漢字ROM 左右選択用 true:右 false:左
	bool cgenable;				// CG ROMアクセスフラグ true:アクセス可 false:アクセス不可
	BYTE cgaddr;				// CG ROMアドレス A13,14,15
	BYTE Rf[3];					// メモリコントローラ内部レジスタ
	// ---------------------------------------------------------------------------------------
	
	// for 64,68 -----------------------------------------------------------------------------
	MemBlock *Rm_blkSR[8];		// リード時メモリブロックポインタ(8KB*8)	SRモード用
	MemBlock *Wm_blkSR[8];		// ライト時メモリブロックポインタ(8KB*8)	SRモード用
	BYTE RfSR[16];				// メモリコントローラ内部レジスタ			SRモード用
	// ---------------------------------------------------------------------------------------
	
	
	bool AllocMemory( BYTE **, const MEMINFO *, const char * );	// メモリ確保とROMファイル読込み
	virtual void SetRamValue() = 0;			// RAMの初期値を設定
	virtual bool InitSpecific() = 0;		// 機種別初期化
	virtual void SetMemBlockR( BYTE, BYTE ) = 0;	// メモリリード時のメモリブロック指定(62,66)
	virtual void SetMemBlockW( BYTE ) = 0;	// メモリライト時のメモリブロック指定(62,66)
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	void SetWait( BYTE );					// メモリアクセスウェイト設定
	BYTE GetWait() const;					// メモリアクセスウェイト取得
	void SetCGrom( BYTE );					// CG ROM アドレス等設定(62,66)
	void SelectCGrom( int );				// CG ROM 選択(62,66)
	void SelectKanjiRom( BYTE );			// 漢字ROM および 音声合成ROM 切り替え(62,66)
	// ---------------------------------------------------------------------------------------
	
	// 戦士のカートリッジ --------------------------------------------------------------------
	static const MEMINFO ISOLRAM;	// メモリ情報
	bool UseSol;					// true:有効 false:無効
	int SolBank;					// ROMバンク(0-15)
	void SetSolBank( BYTE );				// ROMバンク設定
	// ---------------------------------------------------------------------------------------
	
	// I/Oアクセス関数
	void Out7FH( int, BYTE );
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	void OutC1H( int, BYTE );
	void OutC2H( int, BYTE );
	void OutC3H( int, BYTE );
	void OutF0H( int, BYTE );
	void OutF1H( int, BYTE );
	void OutF2H( int, BYTE );
	void OutF3H( int, BYTE );
	void OutF8H( int, BYTE );
	BYTE InF0H( int );
	BYTE InF1H( int );
	BYTE InF2H( int );
	BYTE InF3H( int );
	// ---------------------------------------------------------------------------------------
	
public:
	MEM6( VM6 *, const ID& );				// コンストラクタ
	virtual ~MEM6();						// デストラクタ
	
	bool AllocAllMemory( const char *, bool );	// 全メモリ確保とROMファイル読込み
	bool Init( bool, bool );				// 初期化
	virtual void Patch();					// パッチ
	virtual void Reset();					// リセット
	
	BYTE Fetch( WORD, int * = NULL ) const;	// フェッチ(M1)
	BYTE Read( WORD, int * = NULL ) const;	// メモリリード
	void Write( WORD, BYTE, int * = NULL ) const;	// メモリライト
	
	bool MountExtRom( const char * );		// 拡張ROM マウント
	void UnmountExtRom();					// 拡張ROM アンマウント
	const char *GetFile() const;			// 拡張ROMファイルパス取得
	
	// 8255入出力関連関数
	virtual void SetCGBank( bool );			// CG ROM BANK を切り替える
	
	// 直接アクセス関数
	virtual BYTE ReadMainRom( WORD ) const;
	virtual BYTE ReadMainRam( WORD ) const;
	virtual BYTE ReadExtRom ( WORD ) const;
	virtual BYTE ReadCGrom1 ( WORD ) const;
	virtual BYTE ReadCGrom2 ( WORD ) const;
	virtual BYTE ReadCGrom3 ( WORD ) const;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	const char *GetReadMemBlk( int ) const ;
	const char *GetWriteMemBlk( int ) const ;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// デバイスID
	enum IDOut{ out7FH=0 };
	enum IDIn {};
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class MEM60 : public MEM6 {
private:
	// ROM情報
	static const ROMINFO AMAINROM[];
	static const ROMINFO ACGROM1[];
	
	// メモリ情報
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM0;
	static const MEMINFO ICGROM1;
	static const MEMINFO IINTRAM;
	
	void SetRamValue();						// RAMの初期値を設定
	bool InitSpecific();					// 機種別初期化
	void SetMemBlockR( BYTE, BYTE );		// メモリリード時のメモリブロック指定
	void SetMemBlockW( BYTE );				// メモリライト時のメモリブロック指定
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	MEM60( VM6 *, const ID& );				// コンストラクタ
	~MEM60();								// デストラクタ
	
	void Reset();							// リセット
	
	// 直接アクセス関数
	BYTE ReadMainRam( WORD ) const ;
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class MEM61 : public MEM60 {
private:
	// ROM情報
	static const ROMINFO AMAINROM[];
	static const ROMINFO ACGROM1[];
	
	// メモリ情報
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM0;
	static const MEMINFO ICGROM1;
	static const MEMINFO IINTRAM;
	
public:
	MEM61( VM6 *, const ID& );				// コンストラクタ
	~MEM61();								// デストラクタ
};


class MEM62 : public MEM6 {
protected:
	// ROM情報
	static const ROMINFO AMAINROM[];
	static const ROMINFO ACGROM1[];
	static const ROMINFO ACGROM2[];
	static const ROMINFO AKANJI[];
	static const ROMINFO AVOICE[];
	
	// メモリ情報
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM1;
	static const MEMINFO ICGROM2;
	static const MEMINFO IKANJI;
	static const MEMINFO IVOICE;
	static const MEMINFO IINTRAM;
	
	virtual void SetRamValue();				// RAMの初期値を設定
	virtual bool InitSpecific();			// 機種別初期化
	virtual void SetMemBlockR( BYTE, BYTE );// メモリリード時のメモリブロック指定
	void SetMemBlockW( BYTE );				// メモリライト時のメモリブロック指定
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	MEM62( VM6 *, const ID& );				// コンストラクタ
	virtual ~MEM62();						// デストラクタ
	
	virtual void Reset();					// リセット
	
	// 8255入出力関連関数
	void SetCGBank( bool );					// CG ROM BANK を切り替える
	
	// 直接アクセス関数
	virtual BYTE ReadMainRom( WORD ) const;
	BYTE ReadMainRam( WORD ) const;
	virtual BYTE ReadCGrom2( WORD ) const;
	virtual BYTE ReadKanjiRom( WORD ) const;
	virtual BYTE ReadVoiceRom( WORD ) const;
	
	// デバイスID
	enum IDOut{ out7FH=0, outC1H, outC2H, outC3H, outF0H,  outF1H, outF2H, outF3H, outF8H };
	enum IDIn {                                    inF0H=0, inF1H,  inF2H,  inF3H         };
	
	// ------------------------------------------
	bool DokoSave( cIni * );			// どこでもSAVE
	virtual bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class MEM66 : public MEM62 {
private:
	// ROM情報
	static const ROMINFO AMAINROM[];
	static const ROMINFO ACGROM1[];
	static const ROMINFO ACGROM2[];
	static const ROMINFO AKANJI[];
	static const ROMINFO AVOICE[];
	
	// メモリ情報
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM1;
	static const MEMINFO ICGROM2;
	static const MEMINFO IKANJI;
	static const MEMINFO IVOICE;
	static const MEMINFO IINTRAM;
	
public:
	MEM66( VM6 *, const ID& );				// コンストラクタ
	~MEM66();								// デストラクタ
};


class MEM64 : public MEM62 {
protected:
	// ROM情報
	static const ROMINFO ASYSROM1[];
	static const ROMINFO ASYSROM2[];
	static const ROMINFO ACGROM[];
	
	// メモリ情報
	static const MEMINFO ISYSROM1;
	static const MEMINFO ISYSROM2;
	static const MEMINFO ICGROM;
	static const MEMINFO IINTRAM;
	
	virtual void SetRamValue();				// RAMの初期値を設定
	bool InitSpecific();					// 機種別初期化
	void SetMemBlockR( BYTE, BYTE );		// メモリリード時のメモリブロック指定
	void SetMemBlockSR( BYTE, BYTE );		// メモリリード/ライト時のメモリブロック指定(64,68)
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	void Out6xH( int, BYTE );
	void OutC8H( int, BYTE );
	
	BYTE In6xH( int );
	virtual BYTE InB2H( int );
	
public:
	MEM64( VM6 *, const ID& );				// コンストラクタ
	virtual ~MEM64();						// デストラクタ
	
	void Patch();							// ** パッチ
	void Reset();							// ** リセット
	
	// 直接アクセス関数
	BYTE ReadMainRom( WORD ) const;
	BYTE ReadCGrom1( WORD ) const;
	BYTE ReadCGrom2( WORD ) const;
	BYTE ReadCGrom3( WORD ) const;
	BYTE ReadKanjiRom( WORD ) const;
	BYTE ReadVoiceRom( WORD ) const;
	
	// デバイスID
	enum IDOut{ out6xH=0, out7FH, outC1H, outC2H, outC3H, outF0H, outF1H, outF2H, outF3H, outF8H };
	enum IDIn {  in6xH=0,                                  inF0H,  inF1H,  inF2H,  inF3H,
				 inB2H };
	
	// ------------------------------------------
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class MEM68 : public MEM64 {
protected:
	void SetRamValue();						// RAMの初期値を設定
	
	BYTE InB2H( int );
	
public:
	MEM68( VM6 *, const ID& );				// コンストラクタ
	virtual ~MEM68();						// デストラクタ
};

#endif	// MEMORY_H_INCLUDED
