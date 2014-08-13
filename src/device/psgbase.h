#ifndef PSGBASE_H_INCLUDED
#define PSGBASE_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cPSG {
protected:
	BYTE RegisterLatch;
	BYTE LastEnable;
	
	virtual void WriteReg( BYTE, BYTE ){}	// レジスタ書込み
	virtual BYTE ReadReg(){ return 0xff; }	// レジスタ読込み
	virtual BYTE ReadStatus(){ return 0xff; }	// ステータスレジスタ読込み
	
	virtual void SetClock( int, int ){}		// クロック設定
	virtual void SetVolumeTable( int ){}	// 音量設定(ボリュームテーブル設定)
	
	// ポートアクセス関数
	virtual BYTE PortAread(){ return 0xff; }
	virtual BYTE PortBread(){ return 0xff; }
	virtual void PortAwrite( BYTE ){}
	virtual void PortBwrite( BYTE ){}
	
	virtual void PreWriteReg(){}		// レジスタ変更前のストリーム更新
	
	virtual bool Init( int, int ){}		// 初期化
	virtual void Reset(){}				// リセット
	
public:
	cPSG() : RegisterLatch(0), LastEnable(0xff) {};		// コンストラクタ
	virtual ~cPSG(){};					// デストラクタ
	
	int Update1Sample(){ return 0; }	// ストリーム1Sample更新
};


#endif	// PSGBASE_H_INCLUDED
