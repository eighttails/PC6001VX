/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef PSGBASE_H_INCLUDED
#define PSGBASE_H_INCLUDED

#include "typedef.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class cPSG {
protected:
	BYTE RegisterLatch;
	BYTE LastEnable;
	
	virtual void WriteReg( BYTE, BYTE ) = 0;	// レジスタ書込み
	virtual BYTE ReadReg() = 0;					// レジスタ読込み
	virtual BYTE ReadStatus(){ return 0xff; }	// ステータスレジスタ読込み
	
	virtual void SetClock( int, int ) = 0;		// クロック設定
	virtual void SetVolumeTable( int ) = 0;		// 音量設定(ボリュームテーブル設定)
	
	// ポートアクセス関数
	virtual BYTE PortAread() = 0;
	virtual BYTE PortBread(){ return 0xff; }
	virtual void PortAwrite( BYTE ){}
	virtual void PortBwrite( BYTE ) = 0;
	
	virtual void PreWriteReg() = 0;				// レジスタ変更前のストリーム更新
	
	virtual bool InitMod( int, int ) = 0;		// 初期化
	virtual void Reset() = 0;					// リセット
	
public:
	cPSG() : RegisterLatch(0), LastEnable(0xff) {};		// Constructor
	virtual ~cPSG(){};							// Destructor
	
	virtual int Update1Sample() = 0;			// ストリーム1Sample更新
};


#endif	// PSGBASE_H_INCLUDED
