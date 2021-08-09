/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef VDG_H_INCLUDED
#define VDG_H_INCLUDED

#include <memory>
#include <vector>

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "vsurface.h"
#include "device/mc6847.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class VDG6 : public Device, public VSurface, public virtual MC6847core, public IDoko {
protected:
	// カラーコード
	// for 60,61 ------------------------------------------------------------
	static const BYTE COL60_AN[];
	static const BYTE COL60_SG[];
	static const BYTE COL60_CG[][8];
	static const BYTE COL60_RG[][2];
	// ----------------------------------------------------------------------
	
	// for 62,66,64,68 ------------------------------------------------------
	static const BYTE COL62_AN[];
	static const BYTE COL62_SG[];
	static const BYTE COL62_CG[][8];
	static const BYTE COL62_RG[][2];
	static const BYTE COL62_AN2[];
	static const BYTE COL62_CG2[][16];
	// ----------------------------------------------------------------------
	
	WORD AddrOff;								// VRAM,ATTRオフセット
	
	// 同期信号ワーク
	bool VSYNC;									// 垂直同期フラグ
	bool HSYNC;									// 水平同期フラグ
	int VLcnt;									// 表示ラインカウンタ
	int HSdclk;									// 水平トータル期間(ドットクロック)
	int Hclk60;									// 水平表示期間(N60)
	
	bool CreateBuffer();						// バックバッファ作成
	std::vector<BYTE>& GetBufAddr() override;	// バッファ取得
	int GetBufPitch() const override;			// バッファピッチ(1Lineバイト数)取得
	
	void LatchAttr() override;					// アトリビュートデータラッチ
	void LatchGMODE() override;					// アトリビュートデータラッチ(グラフィックモードのみ)
	BYTE GetFont1( WORD ) const override;		// Font1データ取得
	BYTE GetFont2( WORD ) const override;		// Font2データ取得
	BYTE GetFont3( WORD ) const override;		// Font3データ取得
	
	virtual void SetAttrAddr( BYTE );			// ATTRアドレス設定
	
	// I/Oアクセス関数
	void OutB0H( int, BYTE );
	
	// for 62,66,64,68 ------------------------------------------------------
	void OutC0H( int, BYTE );
	void OutC1H( int, BYTE );
	BYTE InA2H( int );
	// ----------------------------------------------------------------------
	
public:
	VDG6( VM6*, const ID& );
	virtual ~VDG6();
	
	virtual void EventCallback( int, int ) override;	// イベントコールバック関数
	
	virtual bool Init();						// 初期化
	virtual void Reset();						// リセット
	
	virtual WORD GetVramAddr() const = 0;		// VRAMアドレス取得
	virtual WORD GerAttrAddr() const = 0;		// ATTRアドレス取得
	
	bool IsBusReqStop() const;					// バスリクエスト区間停止フラグ取得
	bool IsBusReqExec() const;					// バスリクエスト区間実行フラグ取得
	WORD SRGVramAddr( WORD ) const;				// SRのG-VRAMアドレス取得(ビットマップモード)
	
	// デバイスID
	enum IDOut{ out4xH=0, outB0H, outC0H, outC1H, outC8H, outC9H, outCAH, outCBH, outCCH, outCDH, outCEH, outCFH };
	enum IDIn {  inA2H=0 };
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};


class VDG60 : public VDG6, public MC6847 {
protected:
	BYTE GetAttr() const override;				// アトリビュートデータ取得
	BYTE GetVram() const override;				// VRAMデータ取得
	
public:
	VDG60( VM6*, const ID& );
	virtual ~VDG60();
	
	WORD GetVramAddr() const override;			// VRAMアドレス取得
	WORD GerAttrAddr() const override;			// ATTRアドレス取得
};


class VDG62 : public VDG6, public virtual PCZ80_07 {
protected:
	BYTE GetAttr() const override;				// アトリビュートデータ取得
	BYTE GetVram() const override;				// VRAMデータ取得
	
public:
	VDG62( VM6*, const ID& );
	virtual ~VDG62();
	
	WORD GetVramAddr() const override;			// VRAMアドレス取得
	WORD GerAttrAddr() const override;			// ATTRアドレス取得
};


class VDG64 : public VDG6, public PCZ80_12 {
protected:
	BYTE GetAttr() const override;				// アトリビュートデータ取得
	BYTE GetVram() const override;				// VRAMデータ取得
	void SetAttrAddr( BYTE ) override;			// ATTRアドレス設定
	
	// I/Oアクセス関数
	void Out4xH( int, BYTE );
	void OutC8H( int, BYTE );
	void OutC9H( int, BYTE );
	void OutCAH( int, BYTE );
	void OutCBH( int, BYTE );
	void OutCCH( int, BYTE );
	void OutCDH( int, BYTE );
	void OutCEH( int, BYTE );
	void OutCFH( int, BYTE );
	
public:
	VDG64( VM6*, const ID& );
	virtual ~VDG64();
	
	void EventCallback( int, int ) override;	// イベントコールバック関数
	
	void Reset() override;						// リセット
	
	WORD GetVramAddr() const override;			// VRAMアドレス取得
	WORD GerAttrAddr() const override;			// ATTRアドレス取得
};


#endif	// VDG_H_INCLUDED
