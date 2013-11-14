#ifndef AY8910_H_INCLUDED
#define AY8910_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cAY8910 {
protected:
	int RegisterLatch;
	BYTE Regs[16];
	int LastEnable;
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
	
	void _AYWriteReg( BYTE, BYTE );		// レジスタ書込みサブ
	void AYWriteReg( BYTE, BYTE );		// レジスタ書込み
	BYTE AYReadReg();					// レジスタ読込み
	
	void SetClock( int, int );			// クロック設定
	void SetVolumeTable( int );			// 音量設定(ボリュームテーブル設定)
	
	// ポートアクセス関数
	virtual BYTE PortAread();
	virtual BYTE PortBread();
	virtual void PortAwrite( BYTE );
	virtual void PortBwrite( BYTE );
	
	virtual void PreWriteReg() = 0;		// レジスタ変更前のストリーム更新
	
public:
	cAY8910();							// コンストラクタ
	virtual ~cAY8910();					// デストラクタ
	
	void Reset();						// リセット
	
	int Update1Sample();				// ストリーム1Sample更新
};


#endif	// PSG_H_INCLUDED
