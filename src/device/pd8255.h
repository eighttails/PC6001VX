#ifndef PD8255_H_INCLUDED
#define PD8255_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cD8255 {
protected:
	BYTE PortA;						// ポートA
	BYTE PortB;						// ポートB
	BYTE PortC;						// ポートC
	
	BYTE PortAbuf;					// ポートAバッファ(ポートAの入力を一時的にラッチ)
	
	int ModeA;						// グループAモード
	int ModeB;						// グループBモード
	
	BOOL PortAdir;					// ポートA         入出力制御 TRUE:入力(読み) FALSE:出力(書き)
	BOOL PortBdir;					// ポートB         入出力制御 TRUE:入力(読み) FALSE:出力(書き)
	BOOL PortC1dir;					// ポートC(bit0-3) 入出力制御 TRUE:入力(読み) FALSE:出力(書き)
	BOOL PortC2dir;					// ポートC(bit4-7) 入出力制御 TRUE:入力(読み) FALSE:出力(書き)
	
	BOOL HSINT0;					// INT0
	BOOL HSWINT0;					// WINT0
	BOOL HSRINT0;					// RINT0
	BOOL HSSTB0;					// STB0
	BOOL HSIBF0;					// IBF0
	BOOL HSDAK0;					// DAK0
	BOOL HSOBF0;					// OBF0
	
	BOOL RIE0;						// RIE0
	BOOL WIE0;						// WIE0
	
	// 入出力処理関数
	virtual void JobWriteA( BYTE ){}
	virtual void JobWriteB( BYTE ){}
	virtual void JobWriteC1( BYTE ){}	// 下位
	virtual void JobWriteC2( BYTE ){}	// 上位
	virtual void JobWriteD( BYTE ){}	// ビットセット/リセットのみ
	virtual void JobReadA(){}
	virtual void JobReadB(){}
	virtual void JobReadC(){}
	
	void SetMode( BYTE );			// モード設定
	
public:
	cD8255();						// コンストラクタ
	virtual ~cD8255();				// デストラクタ
	
	void Reset();					// リセット
	
	// 入出力関数
	void WriteA( BYTE );
	void WriteB( BYTE );
	void WriteC( BYTE );
	void WriteD( BYTE );			// コントロールポート出力
	BYTE ReadA();
	BYTE ReadB();
	BYTE ReadC();
	
	// モード2ハンドシェイク用
	void WriteAE( BYTE );			// PartA ライト(周辺側)
	BYTE ReadAE();					// PartA リード(周辺側)
	BOOL GetIBF();					// IBF取得
	BOOL GetOBF();					// OBF取得
};


#endif	// PD8255_H_INCLUDED
