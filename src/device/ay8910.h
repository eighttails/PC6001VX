/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef AY8910_H_INCLUDED
#define AY8910_H_INCLUDED

#include "ini.h"
#include "psgbase.h"
#include "typedef.h"

#ifdef USEFMGEN
#include "fmgen/types.h"
#include "fmgen/psg.h"
#endif


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
#ifdef USEFMGEN
class cAY8910 : public cPSG, public PSG {
protected:
#else
class cAY8910 : public cPSG {
protected:
	BYTE Regs[16];
	int UpdateStep;
	int PeriodA, PeriodB, PeriodC, PeriodN, PeriodE;
	int CountA, CountB, CountC, CountN, CountE;
	int VolA, VolB, VolC, VolE;
	BYTE EnvelopeA, EnvelopeB, EnvelopeC;
	BYTE OutputA, OutputB, OutputC, OutputN;
	int8_t CountEnv;
	BYTE Hold,Alternate,Attack,Holding;
	int RNG;
	int VolTable[32];
#endif
	
	void _WriteReg( BYTE, BYTE );			// レジスタ書込みサブ
	void WriteReg( BYTE, BYTE ) override;	// レジスタ書込み
	BYTE ReadReg() override;				// レジスタ読込み
	
	void SetClock( int, int ) override;		// クロック設定
	void SetVolumeTable( int ) override;	// 音量設定(ボリュームテーブル設定)
	
	bool InitMod( int, int ) override;		// 初期化
	void Reset() override;					// リセット
	
public:
	cAY8910();								// Constructor
	virtual ~cAY8910();						// Destructor
	
	int Update1Sample() override;			// ストリーム1Sample更新
};


#endif	// AY8910_H_INCLUDED
