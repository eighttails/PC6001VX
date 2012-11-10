#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"


// メモリブロック数
#define MAXRMB	(16)
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
	
	void SetMemory( const char *, BYTE *, int, int );	// メモリブロック設定
	void SetWait( int, int );					// アクセスウェイト設定
	int GetWait();								// アクセスウェイト取得
	void SetProtect( bool );					// ライトプロテクト設定
	bool GetProtect();							// ライトプロテクト取得
	
	char *GetName(){ return Name; }				// メモリブロック名取得
	
	BYTE Read( WORD, int * = NULL );			// メモリリード
	void Write( WORD, BYTE, int * = NULL );		// メモリライト
};


class MEM6 : public IDoko {
protected:
	// メモリ情報構造体
	typedef struct{
		const char *FileName;	// ファイル名
		DWORD Size;				// サイズ
		DWORD Crc;				// CRC32
		BYTE Init;				// 初期化データ
		int WaitR;				// アクセスウェイト(Read)
		int WaitW;				// アクセスウェイト(Write)
	} MEMINFO;
	
	// メモリ情報
	static const MEMINFO IEXTROM;
	static const MEMINFO IEMPTROM;
	static const MEMINFO IEMPTRAM;
	static const MEMINFO IEXTRAM;
	
	bool CGBank;				// CG ROM BANK true:有効 false:無効
	bool UseExtRom;				// 拡張ROM     true:有効 false:無効
	bool UseExtRam;				// 拡張RAM     true:有効 false:無効
	
	BYTE *MainRom;				// BASIC ROM	60:16KB 62,66:32KB
	BYTE *ExtRom;				// 拡張 ROM		60:16KB 62,66:16KB
	BYTE *CGRom1;				// CG ROM1		60: 4KB 62,66: 8KB
	BYTE *IntRam;				// 内部 RAM		60:16KB 62,66:64KB
	BYTE *ExtRam;				// 外部 RAM		60:16KB 62,66:16KB
	BYTE *EmptyRom;				// 未実装ROM	60: 8KB 62,66: 8KB
	BYTE *EmptyRam;				// 未実装RAM	60: 8KB 62,66: 8KB
	
	MemBlock RomB[MAXRMB];		// ROMブロック
	MemBlock RamB[MAXWMB];		// RAMブロック
	
	MemBlock *Rm_blk[8];		// リード時メモリブロックポインタ(8KB*8)
	MemBlock *Wm_blk[8];		// ライト時メモリブロックポインタ(8KB*8)
	
	BYTE ram_sel[4];			// 内部RAM,外部RAM 選択用(16KB*4)
	
	char FilePath[PATH_MAX];	// 拡張ROMファイルフルパス
	int M1Wait;					// M1ウェイト
	bool EnableChkCRC;			// CRCチェック有効?
	
	
	// 戦士のカートリッジ ------------------------------------------
	static const MEMINFO ISOLRAM;	// RAM情報
	bool UseSol;					// true:有効 false:無効
	int SolBank;					// ROMバンク(0-15)
	void SetSolBank( BYTE );		// ROMバンク設定
	// -------------------------------------------------------------
	
	
	bool AllocMemory( BYTE **, const MEMINFO *, char * );	// メモリ確保とROMファイル読込み
	bool InitCommon( bool, bool );	// 共通部分初期化
	
public:
	MEM6( bool );							// コンストラクタ
	virtual ~MEM6();						// デストラクタ
	
	virtual bool Init( char *, bool, bool ) = 0;	// 初期化
	virtual void Patch();					// パッチ
	virtual void Reset();					// リセット
	
	BYTE Fetch( WORD, int * = NULL );		// フェッチ(M1)
	BYTE Read( WORD, int * = NULL );		// メモリリード
	void Write( WORD, BYTE, int * = NULL );	// メモリライト
	
	bool MountExtRom( char * );				// 拡張ROM マウント
	void UnmountExtRom();					// 拡張ROM アンマウント
	char *GetFile();						// 拡張ROMファイルパス取得
	
	// 8255入出力関連関数
	virtual void SetCGBank( bool );			// CG ROM BANK を切り替える
	
	// 直接アクセス関数
	virtual BYTE ReadMainRom( WORD );
	virtual BYTE ReadMainRam( WORD );
	virtual BYTE ReadExtRom ( WORD );
	virtual BYTE ReadCGrom0 ( WORD );
	virtual BYTE ReadCGrom1 ( WORD );
	virtual BYTE ReadCGrom2 ( WORD );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	virtual char *GetReadMemBlk( int );
	virtual char *GetWriteMemBlk( int );
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
};


class MEM60 : public MEM6, public Device {
private:
	// メモリ情報
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM0;
	static const MEMINFO ICGROM1;
	static const MEMINFO IINTRAM;
	
	BYTE *CGRom0;		// CG ROM0		60: 1KB 62,66: -KB
	
	void SetMB_60();					// メモリブロック設定(60のみ)
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void Out7FH( int, BYTE );
	
public:
	MEM60( const ID&, bool );			// コンストラクタ
	~MEM60();							// デストラクタ
	
	bool Init( char *, bool, bool );	// 初期化
	void Patch();						// パッチ
	void Reset();						// リセット
	
	// 直接アクセス関数
	BYTE ReadMainRam( WORD );
	BYTE ReadCGrom0( WORD );
	
	// デバイスID
	enum IDOut{ out7FH=0 };
	enum IDIn {};
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class MEM62 : public MEM6, public Device {
protected:
	// メモリ情報
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM1;
	static const MEMINFO ICGROM2;
	static const MEMINFO IKANJI;
	static const MEMINFO IVOICE;
	static const MEMINFO IINTRAM;
	
	BYTE *CGRom2;		// CG ROM2		60: -KB 62,66: 8KB
	BYTE *KanjiRom;		// 漢字 ROM		60: -KB 62,66:32KB
	BYTE *VoiceRom;		// 音声合成 ROM	60: -KB 62,66:16KB
	
	bool cgrom;			// CG ROM 選択用 true:N60モード false:N60mモード
	bool kj_rom;		// 漢字ROM,音声合成ROM 選択用 true:漢字ROM false:音声合成ROM
	bool kj_LR;			// 漢字ROM 左右選択用 true:右 false:左
	
	bool cgenable;		// CG ROMアクセスフラグ true:アクセス可 false:アクセス不可
	BYTE cgaddr;		// CG ROMアドレス A13,14,15
	
	BYTE Rf0;			// メモリコントローラ内部レジスタ
	BYTE Rf1;			// メモリコントローラ内部レジスタ
	BYTE Rf2;			// メモリコントローラ内部レジスタ
	
	void SetMB_R( BYTE, BYTE );			// メモリリード時のメモリブロック指定(62,66のみ)
	void SetMB_W( BYTE );				// メモリライト時のメモリブロック指定(62,66のみ)
	void SetWait( BYTE );				// メモリアクセスウェイト設定
	BYTE GetWait();						// メモリアクセスウェイト取得
	
	void SetCGrom( BYTE );				// CG ROM アドレス等設定(62,66のみ)
	void SelectCGrom( int );			// CG ROM 選択(62,66のみ)
	void SelectKanjiRom( BYTE );		// 漢字ROM および 音声合成ROM 切り替え(62,66のみ)
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void Out7FH( int, BYTE );
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
	
public:
	MEM62( const ID&, bool );			// コンストラクタ
	virtual ~MEM62();					// デストラクタ
	
	virtual bool Init( char *, bool, bool );	// 初期化
	void Patch();						// パッチ
	void Reset();						// リセット
	
	// 8255入出力関連関数
	void SetCGBank( bool );				// CG ROM BANK を切り替える
	
	// 直接アクセス関数
	BYTE ReadMainRom( WORD );
	BYTE ReadMainRam( WORD );
	BYTE ReadCGrom2( WORD );
	BYTE ReadKanjiRom( WORD );
	BYTE ReadVoiceRom( WORD );
	
	// デバイスID
	enum IDOut{ out7FH=0, outC1H, outC2H, outC3H, outF0H,  outF1H, outF2H, outF3H, outF8H };
	enum IDIn {                                    inF0H=0, inF1H,  inF2H,  inF3H         };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class MEM66 : public MEM62 {
private:
	// メモリ情報
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM1;
	static const MEMINFO ICGROM2;
	static const MEMINFO IKANJI;
	static const MEMINFO IVOICE;
	static const MEMINFO IINTRAM;
	
	void SelectKanjiRom( BYTE );		// 漢字ROM および 音声合成ROM 切り替え(62,66のみ)
	
public:
	MEM66( const ID&, bool );			// コンストラクタ
	~MEM66();							// デストラクタ
	
	bool Init( char *, bool, bool );	// 初期化
	void Patch();						// パッチ
};


#endif	// MEMORY_H_INCLUDED
