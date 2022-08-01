/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef YM2203_H_INCLUDED
#define YM2203_H_INCLUDED

#include "ini.h"
#include "psgbase.h"
#include "typedef.h"
#include "fmgen/opna.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class cYM2203 : public cPSG {
protected:
	FM::OPN	opn;
	
	BYTE RegTimerA1, RegTimerA2;
	BYTE RegTimerB;
	BYTE RegTimerCtr;
	
	void _WriteReg( BYTE, BYTE );			// レジスタ書込みサブ
	void WriteReg( BYTE, BYTE ) override;	// レジスタ書込み
	BYTE ReadReg() override;				// レジスタ読込み
	BYTE ReadStatus() override;				// ステータスレジスタ読込み
	
	void SetClock( int, int ) override;		// クロック設定
	void SetVolumeTable( int ) override;	// 音量設定(ボリュームテーブル設定)
	
	bool InitMod( int, int ) override;		// 初期化
	void Reset() override;					// リセット
	
	virtual void SetTimerA( int ){}			// TimerA設定
	virtual void SetTimerB( int ){}			// TimerB設定
	
	bool TimerIntr();						// タイマ割込み発生
	
public:
	cYM2203();								// Constructor
	virtual ~cYM2203();						// Destructor
	
	int Update1Sample() override;			// ストリーム1Sample更新
};


#endif	// YM2203_H_INCLUDED
