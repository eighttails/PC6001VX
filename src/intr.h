#ifndef INTR_H_INCLUDED
#define INTR_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"


// 割込み要求フラグ
//  0 RS232S 受信 割込み
//  1 キー割込み3
//  2 予約
//  3 タイマ割込み
//  4 CMT READ 割込み
//  5 予約
//  6 予約
//  7 キー割込み1 その1
//  8 キー割込み1 その2
//  9 CMT ERROR 割込み
// 10 キー割込み2
// 11 ゲーム用キー割込み
// 12 8049割込み(上記1,2,4-11)
#define	IREQ_SIO	(0x0001)
#define	IREQ_KEY3	(0x0002)
#define	IREQ_TIMER	(0x0008)
#define	IREQ_CMT_R	(0x0010)
#define	IREQ_KEY1	(0x0080)
#define	IREQ_KEY1_2	(0x0100)
#define	IREQ_CMT_E	(0x0200)
#define	IREQ_KEY2	(0x0400)
#define	IREQ_JOY	(0x0800)
#define	IREQ_8049	(0x1000)


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class INT6 : public Device, public IDoko {
protected:
	DWORD IntrFlag;			// 割込み要求フラグ
	
	bool TimerIntrEnable;	// タイマ割込み許可フラグ
	BYTE TimerAddr;			// タイマ割込みアドレス
	BYTE TimerCntUp;		// タイマ割込みカウントアップ値
	
	void SetTimerIntrHz( BYTE, BYTE=0 );	// タイマ割込み周波数設定
	virtual bool IsTimerIntrEnable();		// タイマ割込み許可チェック
	
public:
	INT6( VM6 *, const ID& );				// コンストラクタ
	virtual ~INT6();						// デストラクタ
	
	void EventCallback( int, int );			// イベントコールバック関数
	
	virtual void Reset() = 0;				// リセット
	
	int	IntrCheck();						// 割込みチェック
	
	void ReqIntr( DWORD );					// 割込み要求
	void CancelIntr( DWORD );				// 割込み撤回
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class INT60 : public INT6 {
private:
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutB0H( int, BYTE );
	
public:
	INT60( VM6 *, const ID& );				// コンストラクタ
	~INT60();								// デストラクタ
	
	void Reset();							// リセット
	
	// デバイスID
	enum IDOut{ outB0H=0 };
	enum IDIn {};
};


class INT62 : public INT6 {
private:
	bool TimerIntrEnable2;	// タイマ割込み許可フラグ(PortF3H mk2以降)
	bool Int1IntrEnable;	// INT1割込み許可フラグ
	bool Int2IntrEnable;	// INT2割込み許可フラグ
	bool Int1AddrOutput;	// INT1割込みアドレス出力フラグ
	bool Int2AddrOutput;	// INT2割込みアドレス出力フラグ
	
	BYTE Int1Addr;			// INT1割込みアドレス
	BYTE Int2Addr;			// INT2割込みアドレス
	
	bool IsTimerIntrEnable();				// タイマ割込み許可チェック
	void SetIntrEnable( BYTE );				// 割込み許可フラグ設定
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutB0H( int, BYTE );
	void OutF3H( int, BYTE );
	void OutF4H( int, BYTE );
	void OutF5H( int, BYTE );
	void OutF6H( int, BYTE );
	void OutF7H( int, BYTE );
	BYTE InF3H( int );
	BYTE InF4H( int );
	BYTE InF5H( int );
	BYTE InF6H( int );
	BYTE InF7H( int );
	
public:
	INT62( VM6 *, const ID& );				// コンストラクタ
	~INT62();								// デストラクタ
	
	void Reset();							// リセット
	
	// デバイスID
	enum IDOut{ outB0H=0, outF3H, outF4H, outF5H, outF6H, outF7H };
	enum IDIn {  inF3H=0,  inF4H,  inF5H,  inF6H,  inF7H };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};

#endif	// INTR_H_INCLUDED
