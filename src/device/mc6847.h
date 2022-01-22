/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef MC6847_H_INCLUDED
#define MC6847_H_INCLUDED

#include <vector>

#include "typedef.h"


// 画面情報構造体
struct VDGInfo{
	int w;			// 画面幅  (標準)
	int h;			// 画面高さ(標準)
	double ratio;	// アスペクトレシオ(幅に対する高さの比)
};


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class MC6847core {
protected:
	BYTE COL_AN[5];			// カラーコード(アルファニューメリック)
	BYTE COL_SG[9];			// カラーコード(セミグラフィック)
	BYTE COL_CG[10][8];		// カラーコード(カラーグラフィック)
	BYTE COL_RG[2][2];		// カラーコード(モノクログラフィック)
	// for 62,66,64,68 ------------------------------------------------------
	BYTE COL_AN2[16];		// カラーコード(アルファニューメリック 60m)
	BYTE COL_CG2[2][16];	// カラーコード(カラーグラフィック 60m)
	// ----------------------------------------------------------------------
	
	static const BYTE NJM_TBL[][2];	// 色にじみカラーコードテーブル
	
	bool CrtDisp;			// CRT表示状態			true:表示	false:非表示
	bool BusReq;			// バスリクエスト		true:ON		false:OFF
	bool N60Win;			// ウィンドウサイズ		true:N60	false:N60m
	int Mode4Col;			// モード4カラーモード	0:モノ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク
	
	WORD VAddr;				// 垂直アドレスカウンタ
	WORD HAddr;				// 水平アドレスカウンタ
	int RowCntA;			// 表示ラインカウンタ(アルファニューメリック,セミグラ)
	int RowCntG;			// 表示ラインカウンタ(グラフィック)
	
	// アトリビュートデータ
	BYTE AT_AG;
	BYTE AT_AS;
	BYTE AT_IE;
	BYTE AT_GM;
	BYTE AT_CSS;
	BYTE AT_INV;
	
	// for 62,66,64,68 ------------------------------------------------------
	bool CharMode;			// テキスト表示モード	true:キャラクタ				false:グラフィック
	bool GraphMode;			// グラフィック解像度	true:160*200(SR:320*200)	false:320*200(SR:640*200)
	int Css1;				// 色の組み合わせ
	int Css2;
	int Css3;
	// ----------------------------------------------------------------------
	
	// for 64,68 ------------------------------------------------------------
	bool SRmode;			// SRモードフラグ 			true:SR-BASIC	false:旧BASIC
	bool SRBusReq;			// SRバスリクエストフラグ	true:有効		false:無効
	bool SRBitmap;			// SRビットマップフラグ		true:有効		false:無効
	bool SRBMPage;			// SRビットマップページ		true:上位32KB	false:下位32KB
	bool SRLine204;			// SRグラフィックライン数	true:204ライン	false:200ライン
	bool SRCharLine;		// SRテキスト行数			true:20行		false:25行
	bool SRCharWidth;		// SRテキスト文字数			true:40文字		false:80文字
	
	BYTE SRTextAddr;		// SRテキストVRAMアドレス(0H-FH)
	WORD SRRollX;			// ドットスクロール X座標
	WORD SRRollY;			// ドットスクロール Y座標
	WORD SRVramAddrY;		// グラフィックVRAM Y座標
	// ----------------------------------------------------------------------
	
	// for 62,66,64,68 ------------------------------------------------------
	void SetCss( BYTE );						// 色の組合せ指定
	virtual void SetCrtControler( BYTE );		// CRTコントローラモード設定
	// ----------------------------------------------------------------------
	
	virtual std::vector<BYTE>& GetBufAddr() = 0;	// バッファ取得
	virtual int GetBufPitch() const = 0;		// バッファピッチ(1Lineバイト数)取得
	
	virtual void LatchAttr() = 0;				// アトリビュートデータラッチ
	virtual void LatchGMODE() = 0;				// アトリビュートデータラッチ(グラフィックモードのみ)
	virtual BYTE GetAttr() const = 0;			// アトリビュートデータ取得
	virtual BYTE GetVram() const = 0;			// VRAMデータ取得
	virtual BYTE GetFont1( WORD ) const = 0;	// Font1データ取得
	virtual BYTE GetFont2( WORD ) const = 0;	// Font2データ取得
	virtual BYTE GetFont3( WORD ) const = 0;	// Font3データ取得
	BYTE GetSemi4( BYTE ) const;				// セミグラ4データ取得
	virtual BYTE GetSemi6( BYTE ) const;		// セミグラ6データ取得
	
public:
	MC6847core();								// Constructor
	virtual ~MC6847core();						// Destructor
	
	virtual void UpdateBackBuf() = 0;			// バックバッファ更新
	
	bool GetCrtDisp() const;					// CRT表示状態取得
	void SetCrtDisp( bool );					// CRT表示状態設定
	
	bool GetWinSize() const;					// ウィンドウサイズ取得(N60/N60m)
	
	int GetMode4Color() const;					// モード4カラーモード取得
	void SetMode4Color( int );					// モード4カラーモード設定
	
	const VDGInfo& GetVideoInfo() const;		// 画面情報取得
	
	bool IsSRmode() const;						// SRモード取得
	bool IsSRBitmap( WORD ) const;				// SRビットマップモード取得
	bool IsSRHires() const;						// SRの高解像度?
};


class MC6847 : public virtual MC6847core {
protected:
	static const BYTE VDGfont[];				// VDG Font data
	
	void Draw1line1( int );						// 1ライン描画(N60)
	
	#if INBPP == 8	// 8bit
	BYTE GetBcol() const;						// ボーダーカラー取得
	#else			// 32bit
	DWORD GetBcol() const;						// ボーダーカラー取得
	#endif
	
public:
	MC6847();									// Constructor
	virtual ~MC6847();							// Destructor
	
	void UpdateBackBuf() override;				// バックバッファ更新
};


class PCZ80_07 : public virtual MC6847core {
protected:
	BYTE GetSemi6( BYTE ) const override;		// セミグラ6データ取得
	
	void Draw1line1( int );						// 1ライン描画(N60)
	void Draw1line2( int );						// 1ライン描画(N60m)
	
	#if INBPP == 8	// 8bit
	BYTE GetBcol() const;						// ボーダーカラー取得
	#else			// 32bit
	DWORD GetBcol() const;						// ボーダーカラー取得
	#endif
	
public:
	PCZ80_07();									// Constructor
	virtual ~PCZ80_07();						// Destructor
	
	virtual void UpdateBackBuf() override;		// バックバッファ更新
};


class PCZ80_12 : public virtual PCZ80_07 {
protected:
	BYTE GetSemi6( BYTE ) const override;		// セミグラ6データ取得
	BYTE GetSemi8( BYTE ) const;				// セミグラ8データ取得
	
	void SetCrtControler( BYTE );				// CRTコントローラモード設定
	void SetCrtCtrlType( BYTE );				// CRTコントローラタイプ設定
	
	void Draw1line3( int );						// 1ライン描画(SR)
	
public:
	PCZ80_12();									// Constructor
	virtual ~PCZ80_12();						// Destructor
	
	void UpdateBackBuf() override;				// バックバッファ更新
	
	void SetPalette( int, BYTE );				// パレット設定
};


#endif	// MC6847_H_INCLUDED
