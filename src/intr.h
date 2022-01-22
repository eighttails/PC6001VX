/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef INTR_H_INCLUDED
#define INTR_H_INCLUDED

#include <memory>

#include "typedef.h"
#include "device.h"
#include "ini.h"

// 割込み要求フラグ
//  0 予約
//  1 キー割込み3
//  2 RS232S 受信 割込み
//  3 タイマ割込み
//  4 CMT READ 割込み
//  5 予約
//  6 予約
//  7 キー割込み1 その1
//  8 キー割込み1 その2
//  9 CMT ERROR 割込み
// 10 キー割込み2
// 11 ゲーム用キー割込み
// xx 8049割込み(上記1,2,4-11)

#define	IREQ_KEY3	(0x0001)
#define	IREQ_SIO	(0x0002)
#define	IREQ_TIMER	(0x0004)
#define	IREQ_CMT_R	(0x0008)
#define	IREQ_KEY1	(0x0010)
#define	IREQ_KEY12	(0x0020)
#define	IREQ_CMT_E	(0x0040)
#define	IREQ_KEY2	(0x0080)
#define	IREQ_JOYKEY	(0x0100)
#define	IREQ_8049	(IREQ_KEY3|IREQ_SIO|IREQ_CMT_R|IREQ_KEY1|IREQ_KEY12|IREQ_CMT_E|IREQ_KEY2|IREQ_JOYKEY)

// 12 TVR
// 13 DATE
// 14 予約
// 15 予約
// 16 音声合成
// 17 VRTC
// 18 予約
// 19 予約
#define	IREQ_JOYSTK	(0x0200)

#define	IREQ_VOICE	(0x0400)
#define	IREQ_VRTC	(0x0800)
#define	IREQ_PRINT	(0x1000)
#define	IREQ_EXTINT	(0x2000)



/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class IRQ6 : public Device, public IDoko {
protected:
	DWORD IntrFlag;			// 割込み要求フラグ
	
	bool TimerIntrEnable;	// タイマ割込み許可フラグ
	BYTE TimerCntUp;		// タイマ割込みカウントアップ値
	
	bool IntEnable[8];		// 割込み許可フラグ
	bool VecOutput[8];		// 割込みアドレス出力フラグ
	BYTE IntVector[8];		// 割込みアドレス
	
	BYTE Timer1st;			// タイマ割込み初回周期比率
	
	virtual void SetIntrEnable( BYTE );				// 割込み許可フラグ設定
	void SetTimerIntr( bool );						// タイマ割込みスイッチ設定
	virtual void SetTimerIntrHz( BYTE, BYTE=0 );	// タイマ割込み周波数設定
	
	// I/Oアクセス関数
	void OutB0H( int, BYTE );
	
	// for 62,66,64,68 -----------------------------------------------------------------------
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
	// ----------------------------------------------------------------------
	
public:
	IRQ6( VM6*, const ID& );
	virtual ~IRQ6();
	
	void EventCallback( int, int ) override;	// イベントコールバック関数
	
	virtual void Reset();						// リセット
	
	int	IntrCheck();							// 割込みチェック＆ベクタ取得
	
	void ReqIntr( DWORD );						// 割込み要求
	void CancelIntr( DWORD );					// 割込み撤回
	
	bool GetTimerIntr();						// タイマ割込みスイッチ取得
	DWORD GetIntrFlag();						// 割込み要求フラグ取得
	
	// デバイスID
	enum IDOut{ outB0H=0, outBxH, outF3H, outF4H, outF5H, outF6H, outF7H, outFAH, outFBH };
	enum IDIn {  inF3H=0,  inF4H,  inF5H,  inF6H,  inF7H,  inFAH,  inFBH };
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};


class IRQ60 : public IRQ6 {
private:
	
public:
	IRQ60( VM6*, const ID& );
	~IRQ60();
	
	void Reset();							// リセット
};


class IRQ62 : public IRQ6 {
private:
	void SetIntrEnable( BYTE ) override;	// 割込み許可フラグ設定
	
public:
	IRQ62( VM6*, const ID& );
	~IRQ62();
	
	void Reset();							// リセット
};


class IRQ64 : public IRQ6 {
private:
	void SetIntrEnable( BYTE ) override;			// 割込み許可フラグ設定
	void SetIntrEnableSR( BYTE );					// 割込み許可フラグ設定(SR)
	void SetTimerIntrHz( BYTE, BYTE=0 ) override;	// タイマ割込み周波数設定
	void SetIntrVectorEnableSR( BYTE );				// 割込みベクタアドレス出力フラグ設定(SR)
	
	// I/Oアクセス関数
	void OutBxH( int, BYTE );
	void OutFAH( int, BYTE );
	void OutFBH( int, BYTE );
	BYTE InFAH( int );
	BYTE InFBH( int );
	
public:
	IRQ64( VM6*, const ID& );
	~IRQ64();
	
	void Reset();							// リセット
};


#endif	// INTR_H_INCLUDED
