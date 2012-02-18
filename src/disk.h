#ifndef DISK_H_INCLUDED
#define DISK_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "d88.h"
#include "ini.h"

#include "p6device.h"

// 最大ドライブ接続数
#define	MAXDRV	2

// ミニフロッピーディスク 各種情報
struct DISK60 {
	int ATN;			// Attention		:コマンド送信要求
	int DAC;			// Data Accepted	:データ受信完
	int RFD;			// Ready For Data	:データ受信準備完
	int DAV;			// Data Valid		:データ送信完
	
	int command;		// 受け取ったコマンド
	int step;			// パラメータ入力待ちステータス
	
	int blk;			// 転送ブロック数
	int drv;			// ドライブ番号-1
	int trk;			// トラック番号
	int sct;			// セクタ番号
	
	int size;			// 処理するバイト数
	
	BYTE retdat;		// port D0H から返す値
        DISK60() :
            ATN(0), DAC(0), RFD(0), DAV(0),
            command(0), step(0),
            blk(0), drv(0), trk(0), sct(0),
            size(0),
            retdat(0) {}
};

// コマンド
// 実際には PC-80S31 のコマンドだけど大体同じ？
enum FddCommand
{
	INIT				= 0x00,
	WRITE_DATA			= 0x01,
	READ_DATA			= 0x02,
	SEND_DATA			= 0x03,
	COPY				= 0x04,
	FORMAT				= 0x05,
	SEND_RESULT_STATUS	= 0x06,
	SEND_DRIVE_STATUS	= 0x07,
	TRANSMIT			= 0x11,
	RECEIVE				= 0x12,
	LOAD				= 0x14,
	SAVE				= 0x15,
	
	WAIT				= 0xff,	// 処理待ちの状態
	EndofFdcCmd
};

////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class DSK6 : public P6DEVICE, public IDoko {
protected:
	int DrvNum;							// ドライブ数
	char FilePath[MAXDRV][PATH_MAX];	// ファイルパス
	cD88 *d88[MAXDRV];					// D88オブジェクトへのポインタ
	BOOL Sys[MAXDRV];					// システムディスクフラグ
	
	BOOL SetWait( int, int );			// ウェイト設定
	
public:
	DSK6( VM6 *, const P6ID&, int );	// コンストラクタ
	virtual ~DSK6();					// デストラクタ
	
	virtual void EventCallback( int, int );	// イベントコールバック関数
	
	virtual BOOL Init() = 0;			// 初期化
	
	BOOL Mount( int, char * );			// DISK マウント
	void Unmount( int );				// DISK アンマウント
	
	int GetDrives();					// ドライブ数取得
	
	BOOL IsMount( int );				// マウント済み?
	BOOL IsSystem( int );				// システムディスク?
	BOOL IsProtect( int );				// プロテクト?
	
	const char *GetFile( int );			// ファイルパス取得
	const char *GetName( int );			// DISK名取得
};


class DSK60 : public DSK6, public Device {
private:
	DISK60 mdisk;			// ミニフロッピーディスク各種情報
	
	BYTE io_D1H;
	BYTE io_D2H;
	BYTE io_D3H;
	
	BYTE FddIn();			// DISKユニットからのデータ入力 (port D0H)
	void FddOut( BYTE );	// DISKユニットへのコマンド，データ出力 (port D1H)
	BYTE FddCntIn();		// DISKユニットからの制御信号入力 (port D2H)
	void FddCntOut( BYTE );	// DISKユニットへの制御信号出力 (port D3H)
	
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
	BYTE InD3H( int );
	
public:
	DSK60( VM6 *, const ID&, int );		// コンストラクタ
	~DSK60();							// デストラクタ
	
	BOOL Init();						// 初期化
	
	// デバイスID
	enum IDOut{ outD1H=0, outD2H, outD3H };
	enum IDIn {  inD0H=0,  inD1H,  inD2H, inD3H };
	
	// ------------------------------------------
	BOOL DokoSave( cIni * );	// どこでもSAVE
	BOOL DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


// データバッファ(256BytesX4)
class cFDCbuf {
protected:
	BYTE Data[4][256];
	int Index[4];
	
public:
	cFDCbuf();							// コンストラクタ
	~cFDCbuf();							// デストラクタ
	
	void Push( int, BYTE );				// データバッファにデータを入れる
	BYTE Pop( int );					// データバッファからデータを取り出す
	
	void Clear( int );					// バッファをクリアする
};


class DSK66 : public DSK6, public Device, public cFDCbuf {
private:
        struct CmdBuffer {
		BYTE Data[10];
		int Index;
                CmdBuffer() : Index(0) { *Data = 0; }
	};
	
	CmdBuffer CmdIn;					// コマンドバッファ
	CmdBuffer CmdOut;					// ステータスバッファ
	
	BYTE SRT;							// Step Rate Time シーク動作時にドライブに送るSTEP信号の間隔時間(ms)
	BYTE HUT;							// Head Unloaded Time
	BYTE HLT;							// Head Load Time
	BOOL NonDMA;						// TRUE:Non DMAモード FALSE:DMAモード
	BYTE SeekST0[4];					// SEEK時のST0
	BYTE LastCylinder[4];				// 最後にシークしたシリンダ
	BOOL SeekEnd[4];					// シーク完了フラグ
	BYTE SendSectors;					// 転送量(100H単位)
        BOOL DIO;							// データ転送方向 TRUE: Buffer->CPU FALSE: CPU->Buffer

        BYTE FDCStatus;						// FDC ステータスレジスタ

	void PushStatus( int );				// ステータスバッファにデータを入れる
	BYTE PopStatus();					// ステータスバッファからデータを取り出す
	void OutFDC( BYTE );				// FDC に書込む
	BYTE InFDC();						// FDC から読込む
	void Exec();						// FDC コマンド実行
	void Specify();						// Specify
	void Read();						// Read
	void Write();						// Write
	void Recalibrate();					// Recalibrate
	void Seek();						// Seek
	void SenseInterruptStatus();		// Sense Interrupt Status
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutB1H( int, BYTE );	// FDモード設定
	void OutB2H( int, BYTE );	// FDC INT?
	void OutB3H( int, BYTE );	// PortB2hの入出力制御
	void OutD0H( int, BYTE );	// Buffer
	void OutD1H( int, BYTE );	// Buffer
	void OutD2H( int, BYTE );	// Buffer
	void OutD3H( int, BYTE );	// Buffer
	void OutD6H( int, BYTE );	// ドライブセレクト
	void OutD8H( int, BYTE );	// 書き込み補償制御 ???
	void OutDAH( int, BYTE );	// 転送量指定
	void OutDDH( int, BYTE );	// FDC データレジスタ
	void OutDEH( int, BYTE );	// ?
	
	BYTE InB2H( int );			// FDC INT
	BYTE InD0H( int );			// Buffer
	BYTE InD1H( int );			// Buffer
	BYTE InD2H( int );			// Buffer
	BYTE InD3H( int );			// Buffer
	BYTE InD4H( int );			// ?
	BYTE InDCH( int );			// FDC ステータスレジスタ
	BYTE InDDH( int );			// FDC データレジスタ

public:
	DSK66( VM6 *, const ID&, int );		// コンストラクタ
	~DSK66();							// デストラクタ
	
	void EventCallback( int, int );		// イベントコールバック関数
	
	BOOL Init();						// 初期化
	
	// デバイスID
	enum IDOut{ outB1H=0, outB2H, outB3H, outD0H, outD1H, outD2H,
				outD3H,   outD6H, outD8H, outDAH, outDDH, outDEH };
	enum IDIn {  inB2H=0,  inD0H,  inD1H,  inD2H,  inD3H,  inD4H,  inDCH,  inDDH };
	
	// ------------------------------------------
	BOOL DokoSave( cIni * );	// どこでもSAVE
	BOOL DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};

#endif	// DISK_H_INCLUDED
