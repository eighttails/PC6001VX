#ifndef DISK_H_INCLUDED
#define DISK_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "d88.h"
#include "ini.h"

#define	MAXDRV	4	// 最大ドライブ接続数


// 装置タイプ
enum UnitType
{
	PC6031 = 0,
	PC6031SR,
	PC8031,
	PC80S31,
	PC6601,
	PC6601SR,
	EndofUnitType
};

// ドライブタイプ
enum FddType
{
	FDD1D = 0,
	FDD1DD,
	FDD2D,
	FDD2DD,
	
	EndofFddType
};

// コマンド
// PC-6031準拠(えすびさん調査ベース)
enum FddCommand
{
	INITIALIZE			= 0x00,
	WRITE_DATA			= 0x01,
	READ_DATA			= 0x02,
	SEND_DATA			= 0x03,
	FORMAT				= 0x05,
	SEND_RESULT_STATUS	= 0x06,
	SEND_DRIVE_STATUS	= 0x07,
	
	COPY				= 0x04,	// PC-6031SR
	GET_MEMORY			= 0x0b,	// PC-6031SR
	FAST_WRITE_DISK		= 0x11,	// PC-6031SR
	FAST_SEND_DATA		= 0x12,	// PC-6031SR
	SET_MODE			= 0x17,	// PC-6031SR
	
	IDLE				= 0xff,	// 処理待ちの状態
	EndofFddCmd
};

// ミニフロッピーディスク 各種情報
struct DISK60 {
	FddType Type;		// ドライブタイプ
	
	bool DAC;			// Data Accepted	:データ受信完
	bool RFD;			// Ready For Data	:データ受信準備完
	bool DAV;			// Data Valid		:データ送信完
	
	int command;		// 受け取ったコマンド
	int step;			// パラメータ入力待ちステータス
	
	int blk;			// 転送ブロック数
	int drv;			// ドライブ番号-1
	int trk;			// トラック番号
	int sct;			// セクタ番号
	
	int rsize;			// 読込みバッファのデータ数
	int wsize;			// 書込みバッファのデータ数
	int ridx;
	
	int size;			// 処理するバイト数
	
	bool Fast;			// 高速アクセスフラグ true:有効 false:無効
	bool FastStat;		// 高速アクセス時アクセスデータフラグ true:2バイト目 false:1バイト目
	
	BYTE retdat;		// port D0H から返す値
	
	BYTE busy;			// ドライブBUSY 1:ドライブ1 2:ドライブ2
	
	bool error;			// エラーフラグ true:エラーあり false:エラーなし
	
	DISK60() :
		Type(FDD1D), DAC(0), RFD(0), DAV(0),
		command(IDLE), step(0),
		blk(0), drv(0), trk(0), sct(0),
		size(0), Fast(false), FastStat(false),
		retdat(0xff), busy(0), error(false) {}
};


enum FdcPhase {
	IDLEP = 0,
	C_PHASE,
	E_PHASE,
	R_PHASE
};

enum FdcSeek{
	SK_STOP = 0,	// シークなし
	SK_SEEK,		// シーク中
	SK_END			// シーク完了
};

struct PD765 {
	BYTE command;		// コマンド
	
//	FdcPhase phase;		// Phase (C/E/R)
//	int step;			// Phase内の処理手順
	
	BYTE SRT;			// Step Rate Time
	BYTE HUT;			// Head Unloaded Time
	BYTE HLT;			// Head Load Time
	bool ND;			// Non DMA Mode  true:Non DMA false:DMA
	
	FdcSeek SeekSta[4];	// シーク状態
	BYTE NCN[4];		// New Cylinder Number
	BYTE PCN[4];		// Present Cylinder Number
	
	
	BYTE MT;			// Multi-track
	BYTE MF;			// MFM/FM Mode
	BYTE SK;			// Skip
	BYTE HD;			// Head
	BYTE US;			// Unit Select
	
	BYTE C;				// Cylinder Number
	BYTE H;				// Head Address
	BYTE R;				// Record
	BYTE N;				// Number
	BYTE EOT;			// End of Track
	BYTE GPL;			// Gap Length
	BYTE DTL;			// Data length
	
	BYTE D;				// Format Data
	BYTE SC;			// Sector
	
	BYTE ST0;			// ST0
	BYTE ST1;			// ST1
	BYTE ST2;			// ST2
	BYTE ST3;			// ST3
	
	BYTE Status;		// Status
	bool Intr;			// FDC割込み発生フラグ
	
		PD765() :
		command(0), // phase(R_PHASE), step(0),
		SRT(32), HUT(0), HLT(0), ND(false),
		MT(0), MF(0), SK(0), HD(0), US(0), C(0), H(0), R(0), N(0),
		EOT(0), GPL(0), DTL(0),
		ST0(0), ST1(0), ST2(0), ST3(0), Status(0), Intr(false)
		{
			INITARRAY( SeekSta, SK_STOP );
			INITARRAY( NCN, 0 );
			INITARRAY( PCN, 0 );
		}
};


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class DSK6 : public Device, public IDoko {
protected:
	UnitType UType;						// 装置タイプ
	int DrvNum;							// ドライブ数
	char FilePath[MAXDRV][PATH_MAX];	// ファイルパス
	cD88 *Dimg[MAXDRV];					// ディスクイメージオブジェクトへのポインタ
	bool Sys[MAXDRV];					// システムディスクフラグ
	bool DDDrv[MAXDRV];					// 1DDドライブフラグ
	int waitcnt;						// ウェイトカウンタ
	
	void ResetWait();					// ウェイトカウンタリセット
	void AddWait( int );				// ウェイトカウンタ加算
	bool SetWait( int );				// ウェイト設定
	
public:
	DSK6( VM6 *, const ID& );			// コンストラクタ
	virtual ~DSK6();					// デストラクタ
	
	virtual void EventCallback( int, int );	// イベントコールバック関数
	
	virtual bool Init( int ) = 0;		// 初期化
	virtual void Reset() = 0;			// リセット
	
	bool Mount( int, const char * );	// DISK マウント
	void Unmount( int );				// DISK アンマウント
	
	int GetDrives();					// ドライブ数取得
	
	bool IsMount( int ) const;			// マウント済み?
	bool IsSystem( int ) const;			// システムディスク?
	bool IsProtect( int ) const;		// プロテクト?
	virtual bool InAccess( int ) const = 0;	// アクセス中?
	
	const char *GetFile( int ) const;	// ファイルパス取得
	const char *GetName( int ) const;	// DISK名取得
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class DSK60 : public DSK6 {
protected:
	DISK60 mdisk;			// ミニフロッピーディスク各種情報
	
	BYTE RBuf[4096];		// 読込みバッファ
	BYTE WBuf[4096];		// 書込みバッファ
	
	BYTE io_D1H;
	BYTE io_D2H;
	
	BYTE FddIn();			// DISKユニットからのデータ入力 		(port D0H)
	void FddOut( BYTE );	// DISKユニットへのコマンド，データ出力 (port D1H)
	BYTE FddCntIn();		// DISKユニットからの制御信号入力 		(port D2H)
	void FddCntOut( BYTE );	// DISKユニットへの制御信号出力 		(port D3H)
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutD1H( int, BYTE );
	void OutD2H( int, BYTE );
	void OutD3H( int, BYTE );
	BYTE InD0H( int );
	BYTE InD1H( int );
	BYTE InD2H( int );
	
public:
	DSK60( VM6 *, const ID& );			// コンストラクタ
	virtual ~DSK60();					// デストラクタ
	
	void EventCallback( int, int );		// イベントコールバック関数
	
	bool Init( int );					// 初期化
	void Reset();						// リセット
	bool InAccess( int ) const;			// アクセス中?
	
	// デバイスID
	enum IDOut{ outD1H=0, outD2H, outD3H };
	enum IDIn {  inD0H=0,  inD1H,  inD2H };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class DSK64 : public DSK60 {
public:
	DSK64( VM6 *, const ID& );			// コンストラクタ
	virtual ~DSK64();					// デストラクタ
};


class DSK66 : public DSK6 {
private:
	struct CmdBuffer {
		BYTE Data[10];
		int Index;
		
		CmdBuffer() : Index(0) { *Data = 0; }
	};
	
	BYTE FDDBuf[1024];					// FDDバッファ
	void BufWrite( int, BYTE );			// FDDバッファ書込み
	BYTE BufRead( int );				// FDDバッファ読込み
	
	CmdBuffer CmdIn;					// コマンドバッファ
	CmdBuffer CmdOut;					// ステータスバッファ
	
	// FDC
	PD765 fdc;
	
	// FDCI
	int SendBytes;						// 転送量(256Bytes単位)
	bool ExtDrv;						// FDDタイプ true: 外付 false: 内蔵
	bool B2Dir;							// PortB2H アクセスレジスタ true: 出力 false: 入力
	
	void PushStatus( BYTE );			// ステータスバッファにデータを入れる
	BYTE PopStatus();					// ステータスバッファからデータを取り出す
	
	void OutFDC( BYTE );				// FDC に書込む
	BYTE InFDC();						// FDC から読込む
	void Exec();						// FDC コマンド実行
	
	bool SearchSector( BYTE * );		// セクタを探す
	
	void ReadDiagnostic();				// Read Diagnostic
	void Specify();						// Specify
	void ReadData();					// Read Data
	void WriteData();					// Write Data
	void Recalibrate();					// Recalibrate
	void Seek();						// Seek
	void SenseInterruptStatus();		// Sense Interrupt Status
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutB1H( int, BYTE );	// FDCIモード設定
	void OutB3H( int, BYTE );	// PortB2hの入出力制御
	void OutD0H( int, BYTE );	// Buffer
	void OutD1H( int, BYTE );	// Buffer
	void OutD2H( int, BYTE );	// Buffer
	void OutD3H( int, BYTE );	// Buffer
	void OutD6H( int, BYTE );	// FDDモータ制御
	void OutD8H( int, BYTE );	// 書き込み補償制御 ???
	void OutDAH( int, BYTE );	// 転送量指定
	void OutDDH( int, BYTE );	// FDC データレジスタ
	void OutDEH( int, BYTE );	// ?
	
	BYTE InB2H( int );			// FDC INT
	BYTE InD0H( int );			// Buffer
	BYTE InD1H( int );			// Buffer
	BYTE InD2H( int );			// Buffer
	BYTE InD3H( int );			// Buffer
	BYTE InD4H( int );			// FDDモータの状態
	BYTE InDCH( int );			// FDC ステータスレジスタ
	BYTE InDDH( int );			// FDC データレジスタ

public:
	DSK66( VM6 *, const ID& );			// コンストラクタ
	~DSK66();							// デストラクタ
	
	void EventCallback( int, int );		// イベントコールバック関数
	
	bool Init( int );					// 初期化
	void Reset();						// リセット
	bool InAccess( int ) const;			// アクセス中?
	
	// デバイスID
	enum IDOut{ outB1H=0, outB3H, outD0H, outD1H, outD2H, outD3H, outD6H, outD8H,
				outDAH,   outDDH, outDEH };
	enum IDIn {  inB2H=0,  inD0H,  inD1H,  inD2H,  inD3H,  inD4H,  inDCH,  inDDH };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class DSK68 : public DSK66 {
public:
	DSK68( VM6 *, const ID& );			// コンストラクタ
	virtual ~DSK68();					// デストラクタ
};

#endif	// DISK_H_INCLUDED
