#ifndef VDG_H_INCLUDED
#define VDG_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "vsurface.h"
#include "device/mc6847.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class VDG6 : public Device, public VSurface, public virtual MC6847core, public IDoko {
protected:
	// カラーコード
	// for 60,61 -----------------------------------------------------------------------------
	static const BYTE COL60_AN[];
	static const BYTE COL60_SG[];
	static const BYTE COL60_CG[][8];
	static const BYTE COL60_RG[][2];
	// ---------------------------------------------------------------------------------------
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	static const BYTE COL62_AN[];
	static const BYTE COL62_SG[];
	static const BYTE COL62_CG[][8];
	static const BYTE COL62_RG[][2];
	static const BYTE COL62_AN2[];
	static const BYTE COL62_CG2[][16];
	// ---------------------------------------------------------------------------------------
	
	WORD AddrOff;								// VRAM,ATTRオフセット
	
	// 同期信号ワーク
	bool VSYNC;									// 垂直同期フラグ
	bool HSYNC;									// 水平同期フラグ
	int VLcnt;									// 表示ラインカウンタ
	int HSdclk;									// 水平トータル期間(ドットクロック)
	int Hclk60;									// 水平表示期間(N60)
	
	bool CreateBuffer( int=1 );					// バックバッファ作成
	BYTE *GetBufAddr() const;					// バッファアドレス取得
	int GetBufPitch() const;					// バッファピッチ(1Lineバイト数)取得
	
	void LatchAttr();							// アトリビュートデータラッチ
	void LatchGMODE();							// アトリビュートデータラッチ(グラフィックモードのみ)
	BYTE GetAttr() const;						// アトリビュートデータ取得
	virtual BYTE GetVram() const;				// VRAMデータ取得
	BYTE GetFont1( WORD ) const;				// Font1データ取得
	BYTE GetFont2( WORD ) const;				// Font2データ取得
	BYTE GetFont3( WORD ) const;				// Font3データ取得
	
	virtual WORD GetVramAddr() const = 0;		// VRAMアドレス取得
	virtual WORD GerAttrAddr() const = 0;		// ATTRアドレス取得
	virtual void SetAttrAddr( BYTE );			// ATTRアドレス設定
	
	// I/Oアクセス関数
	void OutB0H( int, BYTE );
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	void OutC0H( int, BYTE );
	void OutC1H( int, BYTE );
	BYTE InA2H( int );
	// ---------------------------------------------------------------------------------------
	
public:
	VDG6( VM6 *, const ID& );					// コンストラクタ
	virtual ~VDG6();							// デストラクタ
	
	virtual void EventCallback( int, int );		// イベントコールバック関数
	
	virtual bool Init();						// 初期化
	virtual void Reset();						// リセット
	
	bool IsBusReqStop() const;					// バスリクエスト区間停止フラグ取得
	bool IsBusReqExec() const;					// バスリクエスト区間実行フラグ取得
	bool IsSRmode() const;						// SRモード取得
	bool IsSRGVramAccess( WORD ) const;			// SRのG-VRAMアクセス?
	WORD SRGVramAddr( WORD ) const;				// SRのG-VRAMアドレス取得
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class VDG60 : public VDG6, public MC6847 {
protected:
	WORD GetVramAddr() const;					// VRAMアドレス取得
	WORD GerAttrAddr() const;					// ATTRアドレス取得
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	VDG60( VM6 *, const ID& );					// コンストラクタ
	virtual  ~VDG60();							// デストラクタ
	
	// デバイスID
	enum IDOut{ outB0H=0 };
	enum IDIn {};
};


class VDG62 : public VDG6, public virtual PCZ80_07 {
protected:
	WORD GetVramAddr() const;					// VRAMアドレス取得
	WORD GerAttrAddr() const;					// ATTRアドレス取得
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	VDG62( VM6 *, const ID& );					// コンストラクタ
	virtual ~VDG62();							// デストラクタ
	
	// デバイスID
	enum IDOut{ outB0H=0, outC0H, outC1H };
	enum IDIn {  inA2H=0 };
};


class VDG64 : public VDG6, public PCZ80_12 {
protected:
	BYTE GetVram() const;						// VRAMデータ取得
	WORD GetVramAddr() const;					// VRAMアドレス取得
	WORD GerAttrAddr() const;					// ATTRアドレス取得
	void SetAttrAddr( BYTE );					// ATTRアドレス設定
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void Out4xH( int, BYTE );
	void OutC8H( int, BYTE );
	void OutC9H( int, BYTE );
	void OutCAH( int, BYTE );
	void OutCBH( int, BYTE );
	void OutCCH( int, BYTE );
	void OutCEH( int, BYTE );
	void OutCFH( int, BYTE );
	
public:
	VDG64( VM6 *, const ID& );					// コンストラクタ
	virtual ~VDG64();							// デストラクタ
	
	void EventCallback( int, int );				// イベントコールバック関数
	
	void Reset();								// リセット
	
	// デバイスID
	enum IDOut{ out4xH=0, outB0H, outC0H, outC1H, outC8H, outC9H,
				outCAH,   outCBH, outCCH, outCEH, outCFH };
	enum IDIn {  inA2H=0 };
};


#endif	// VDG_H_INCLUDED
