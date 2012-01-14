#ifndef PSG_H_INCLUDED
#define PSG_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "sound.h"
#include "p6device.h"
#include "device/ay8910.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class PSG6 : public P6DEVICE, public Device, public cAY8910, public SndDev, public IDoko {
private:
	BYTE JoyNo;							// 読取るジョイスティックの番号(0-1)
	
	// ポートアクセス関数
	BYTE PortAread();
	void PortBwrite( BYTE );
	
	int GetUpdateSamples();				// 更新サンプル数取得
	void PreWriteReg();					// レジスタ変更前のストリーム更新
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutA0H( int, BYTE );			// PSGレジスタアドレスラッチ
	void OutA1H( int, BYTE );			// PSGライトデータ
	void OutA3H( int, BYTE );			// PSGインアクティブ
	BYTE InA2H( int );					// PSGリードデータ
	
public:
	PSG6( VM6 *, const ID& );			// コンストラクタ
	~PSG6();							// デストラクタ
	
	void EventCallback( int, int );		// イベントコールバック関数
	
	BOOL Init( int, int );				// 初期化
	
	int SoundUpdate( int );				// ストリーム更新
	
	// デバイスID
	enum IDOut{ outA0H=0, outA1H, outA3H };
	enum IDIn {  inA2H=0 };
	
	// ------------------------------------------
	BOOL DokoSave( cIni * );	// どこでもSAVE
	BOOL DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


#endif	// AY8910_H_INCLUDED
