/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef CPUS_H_INCLUDED
#define CPUS_H_INCLUDED

#include <memory>

#include "device.h"
#include "ini.h"


// CMTステータス
#define	CMTCLOSE	(0)
#define	LOADOPEN	(1)
#define	SAVEOPEN	(2)

// I/Oポート番号
#define	IO8049_BUS	(0x00)	// バスポート
#define	IO8049_P1	(0x01)	// Port1
#define	IO8049_P2	(0x02)	// Port2
#define	IO8049_T0	(0x03)	// T0(OUT)
#define	IO8049_INT	(0x04)	// ~INT(IN)


/////////////////////////////////////////////////////////////////////////////
// インターフェース定義
/////////////////////////////////////////////////////////////////////////////
struct ISUB {
   virtual ~ISUB(){};							// Destructor
   
	virtual void Reset() = 0;					// リセット
	virtual void ExtIntr() = 0;					// 外部割込み要求
	
	// キーボード関連
	virtual void ReqKeyIntr( int, BYTE ) = 0;	// キー割込み要求
	
	// CMT関連
	virtual void ReqCmtIntr( BYTE ) = 0;		// CMT READ割込み要求
	virtual int GetCmtStatus() const = 0;		// CMTステータス取得
	virtual bool IsCmtIntrReady() = 0;			// CMT割込み発生可?
};


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class SUB6 : public Device, public IDoko {
protected:
	int CmtStatus;						// CMTステータス
	int Status8049;						// 8049動作ステータス
	
	DWORD IntrFlag;						// 割込み要求フラグ
	
	BYTE KeyCode;						// キーコード(割込み用)
	BYTE JoyCode;						// ゲーム用キーコード(割込み用)
	BYTE CmtData;						// CMTリードデータ(割込み用)
	BYTE SioData;						// RS232C受信データ(割込み用)
	
	BYTE TVRData[32];					// TV予約データ
	BYTE DateData[5];					// DATEデータ
	
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
