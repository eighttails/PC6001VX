#ifndef INTR_H_INCLUDED
#define INTR_H_INCLUDED

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



////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class IRQ6 : public Device, public IDoko {
protected:
	DWORD IntrFlag;			// 割込み要求フラグ
	
	bool TimerIntrEnable;	// タイマ割込み許可フラグ
	BYTE TimerCntUp;		// タイマ割込みカウントアップ値
	
	bool IntEnable[8];		// 割込み許可フラグ
	bool VecOutput[8];		// 割込みアドレス出力フラグ
	BYTE IntVector[8];		// 割込みアドレス
	
	BYTE Timer1st;			// タイマ割込み初回周期比率
	
	virtual void SetIntrEnable( BYTE );		// 割込み許可フラグ設定
	void SetTimerIntr( bool );				// タイマ割込みスイッチ設定
	void SetTimerIntrHz( BYTE, BYTE=0 );	// タイマ割込み周波数設定
	
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
	// ---------------------------------------------------------------------------------------
	
public:
	IRQ6( VM6 *, const ID& );				// コンストラクタ
	virtual ~IRQ6();						// デストラクタ
	
	void EventCallback( int, int );			// イベントコールバック関数
	
	virtual void Reset() = 0;				// リセット
	
	int	IntrCheck();						// 割込みチェック
	
	void ReqIntr( DWORD );					// 割込み要求
	void CancelIntr( DWORD );				// 割込み撤回
	
	bool GetTimerIntr();					// タイマ割込みスイッチ取得
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class IRQ60 : public IRQ6 {
private:
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	IRQ60( VM6 *, const ID& );				// コンストラクタ
	~IRQ60();								// デストラクタ
	
	void Reset();							// リセット
	
	// デバイスID
	enum IDOut{ outB0H=0 };
	enum IDIn {};
};


class IRQ62 : public IRQ6 {
private:
	void SetIntrEnable( BYTE );				// 割込み許可フラグ設定
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	IRQ62( VM6 *, const ID& );				// コンストラクタ
	~IRQ62();								// デストラクタ
	
	void Reset();							// リセット
	
	// デバイスID
	enum IDOut{ outB0H=0, outF3H,  outF4H, outF5H, outF6H, outF7H };
	enum IDIn {            inF3H=0, inF4H,  inF5H,  inF6H,  inF7H };
};



class IRQ64 : public IRQ6 {
private:
	void SetIntrEnable( BYTE );				// 割込み許可フラグ設定
	void SetIntrVectorEnable( BYTE );		// 割込みベクタアドレス出力フラグ設定
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutBxH( int, BYTE );
	void OutFAH( int, BYTE );
	void OutFBH( int, BYTE );
	BYTE InFAH( int );
	BYTE InFBH( int );
	
public:
	IRQ64( VM6 *, const ID& );				// コンストラクタ
	~IRQ64();								// デストラクタ
	
	void Reset();							// リセット
	
	// デバイスID
	enum IDOut{ outB0H=0, outBxH,
				outF3H,   outF4H, outF5H, outF6H, outF7H, outFAH, outFBH };
	enum IDIn {  inF3H=0,  inF4H,  inF5H,  inF6H,  inF7H,  inFAH,  inFBH };
};


#endif	// INTR_H_INCLUDED
