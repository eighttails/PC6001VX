#ifndef VDG_H_INCLUDED
#define VDG_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "p6device.h"
#include "vsurface.h"
#include "device/mc6847.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class VDG6 : public P6DEVICE, public VSurface, public virtual cMC6847core, public IDoko {
protected:
	BYTE AddrOff;								// VRAM,ATTRオフセット(Bit1,2)
	
	// 同期信号ワーク
	int VLcnt;									// 表示ラインカウンタ
	bool OnDisp;								// 表示区間フラグ
	int HSdclk;									// 水平トータル期間(ドットクロック)
	int Hclk60;									// 水平表示期間(N60)
	
	BYTE *GetBufAddr();							// バッファアドレス取得
	int GetBufPitch();							// バッファピッチ(1Lineバイト数)取得
	int GetBufHeight();							// バッファ高さ取得
	
	void LatchAttr();							// アトリビュートデータラッチ
	void LatchGMODE();							// アトリビュートデータラッチ(グラフィックモードのみ)
	BYTE GetAttr();								// アトリビュートデータ取得
	BYTE GetVram();								// VRAMデータ取得
	BYTE GetFont1( WORD );						// Font1データ取得
	
	virtual WORD GerVramAddr() = 0;				// VRAMアドレス取得
	virtual WORD GerAttrAddr() = 0;				// ATTRアドレス取得
	
public:
	VDG6( VM6 *, const P6ID& );					// コンストラクタ
	virtual ~VDG6();							// デストラクタ
	
	void EventCallback( int, int );				// イベントコールバック関数
	
	virtual bool Init();						// 初期化
	bool IsDisp();								// 表示区間フラグ取得
};


class VDG60 : public VDG6, public cMC6847_1, public Device {
protected:
	// カラーコード
	static const BYTE COL60_AN[];
	static const BYTE COL60_SG[];
	static const BYTE COL60_CG[][4];
	static const BYTE COL60_RG[][2];
	
	BYTE GetFont0( WORD );						// Font0(VDG Font)データ取得
	
	WORD GerVramAddr();							// VRAMアドレス取得
	WORD GerAttrAddr();							// ATTRアドレス取得
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutB0H( int, BYTE );
	
public:
	VDG60( VM6 *, const ID& );					// コンストラクタ
	virtual  ~VDG60();							// デストラクタ
	
	// デバイスID
	enum IDOut{ outB0H=0 };
	enum IDIn {};
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};



class VDG62 : public VDG6, public cMC6847_2, public Device {
protected:
	// カラーコード
	static const BYTE COL62_AN[];
	static const BYTE COL62_SG[];
	static const BYTE COL62_CG[][4];
	static const BYTE COL62_RG[][2];
	static const BYTE COL62_AN2[];
	static const BYTE COL62_CG3[][16];
	static const BYTE COL62_CG4[][16];
	
	BYTE GetFont2( WORD );						// Font2データ取得
	
	WORD GerVramAddr();							// VRAMアドレス取得
	WORD GerAttrAddr();							// ATTRアドレス取得
	
	void SetCss( BYTE );						// 色の組合せ指定
	void SetCrtControler( BYTE );				// CRTコントローラモード設定
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutB0H( int, BYTE );
	void OutC0H( int, BYTE );
	void OutC1H( int, BYTE );
	BYTE InC1H( int );
	
public:
	VDG62( VM6 *, const P6ID& );				// コンストラクタ
	virtual  ~VDG62();							// デストラクタ
	
	// デバイスID
	enum IDOut{ outB0H=0, outC0H, outC1H };
	enum IDIn {  inC1H=0 };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


#endif	// VDG_H_INCLUDED
