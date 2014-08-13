#ifndef YM2203_H_INCLUDED
#define YM2203_H_INCLUDED

#include "../typedef.h"
#include "../ini.h"
#include "psgbase.h"
#include "fmgen/opna.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cYM2203 : public cPSG, public virtual IDoko {
protected:
	FM::OPN	opn;
	
	BYTE RegTimerA1, RegTimerA2;
	BYTE RegTimerB;
	BYTE RegTimerCtr;
	
	void _WriteReg( BYTE, BYTE );		// レジスタ書込みサブ
	void WriteReg( BYTE, BYTE );		// レジスタ書込み
	BYTE ReadReg();						// レジスタ読込み
	BYTE ReadStatus();					// ステータスレジスタ読込み
	
	void SetClock( int, int );			// クロック設定
	void SetVolumeTable( int );			// 音量設定(ボリュームテーブル設定)
	
	bool Init( int, int );				// 初期化
	void Reset();						// リセット
	
	virtual void SetTimerA( int ){}		// TimerA設定
	virtual void SetTimerB( int ){}		// TimerB設定
	
	bool TimerIntr();					// タイマ割込み発生
	
public:
	cYM2203();							// コンストラクタ
	virtual ~cYM2203();					// デストラクタ
	
	int Update1Sample();				// ストリーム1Sample更新
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


#endif	// YM2203_H_INCLUDED
