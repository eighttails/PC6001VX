/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef CPUS_H_INCLUDED
#define CPUS_H_INCLUDED

#include <array>

#include "device.h"
#include "ini.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class SUB6 : public Device, public IDoko {
public:
	// I/Oポート番号
	enum {
		IO_BUS,	// バスポート
		IO_P1,	// Port1
		IO_P2,	// Port2
		IO_T0,	// T0(OUT)
		IO_INT	// ~INT(IN)
	};
	
	// CMTステータス
	enum { CMTCLOSE, LOADOPEN, SAVEOPEN };
	
#ifdef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
protected:
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// 動作ステータス
	enum {
		SS_IDLE,	// 何もしていない
		SS_KEY1,	// キー割込み1その1
		SS_KEY12,	// キー割込み1その2
		SS_KEY2,	// キー割込み2
		SS_KEY3,	// キー割込み3
		SS_CMTR,	// CMT READ割込み
		SS_CMTE,	// CMT ERROR割込み
		SS_SIO,		// RS232C受信割込み
		SS_JOY,		// ゲーム用キー割込み
		SS_TVRR,	// TV予約読込み割込み
		SS_DATE,	// DATE割込み
		SS_CMTO,	// CMT 1文字出力 データ待ち
		SS_TVRW		// TV予約書込み データ待ち
	};
	
protected:
	int CpuStatus;						// 動作ステータス
	int CmtStatus;						// CMTステータス
	
	DWORD IntrFlag;						// 割込み要求フラグ
	
	BYTE KeyCode;						// キーコード(割込み用)
	BYTE JoyCode;						// ゲーム用キーコード(割込み用)
	BYTE CmtData;						// CMTリードデータ(割込み用)
	BYTE SioData;						// RS232C受信データ(割込み用)
	
	std::array<BYTE,32> TVRData;		// TV予約データ
	std::array<BYTE, 5> DateData;		// DATEデータ
	
	int TVRCnt;							// TV予約割込み用カウンタ
	int DateCnt;						// DATE割込み用カウンタ;
	
	BYTE ReadIO( int );					// I/Oポートアクセス Read
	void WriteIO( int, BYTE );			// I/Oポートアクセス Write
	
	void WriteExt( BYTE );				// 外部メモリ出力(8255のPortA)
	BYTE ReadExt();						// 外部メモリ入力(8255のPortA)
	
	bool GetT0();						// テスト0ステータス取得
	bool GetT1();						// テスト1ステータス取得
	bool GetINT();						// 外部割込みステータス取得
	
	void ReqJoyIntr();					// ゲーム用キー割込み要求
	
	void OutVector();					// 割込みベクタ出力
	void OutData()	;					// 割込みデータ出力
	
	virtual void ExtIntrExec( BYTE );	// 外部割込み処理
	
public:
	SUB6( VM6*, const ID& );
	virtual ~SUB6();
	
	void EventCallback( int, int );		// イベントコールバック関数
	
	void Reset();						// リセット
	void ExtIntr();						// 外部割込み要求
	
	// キーボード関連
	void ReqKeyIntr( int, BYTE );		// キー割込み要求
	
	// CMT関連
	void ReqCmtIntr( BYTE );			// CMT READ割込み要求
	int GetCmtStatus() const;			// CMTステータス取得
	bool IsCmtIntrReady();				// CMT割込み発生可?
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int GetStatus() const;				// 動作ステータス取得
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* );		// どこでもSAVE
	bool DokoLoad( cIni* );		// どこでもLOAD
	// ----------------------------------------------------------------------
};


class SUB60 : virtual public SUB6 {
protected:
	
public:
	SUB60( VM6*, const ID& );
	virtual ~SUB60();
};


class SUB62 : virtual public SUB6 {
protected:
	void ExtIntrExec( BYTE ) override;	// 外部割込み処理
	
public:
	SUB62( VM6*, const ID& );
	virtual ~SUB62();
};


class SUB68 : virtual public SUB6 {
protected:
	void ReqTVRReadIntr();				// TV予約読込み割込み要求
	void ReqDateIntr();					// DATE割込み要求
	
	void ExtIntrExec( BYTE ) override;	// 外部割込み処理
	
public:
	SUB68( VM6*, const ID& );
	~SUB68();
};



#endif	// CPUS_H_INCLUDED
