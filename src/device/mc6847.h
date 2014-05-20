#ifndef MC6847_H_INCLUDED
#define MC6847_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class MC6847core {
protected:
	BYTE COL_AN[5];			// カラーコード(アルファニューメリック)
	BYTE COL_SG[9];			// カラーコード(セミグラフィック)
	BYTE COL_CG[10][8];		// カラーコード(カラーグラフィック)
	BYTE COL_RG[2][2];		// カラーコード(モノクログラフィック)
	// for 62,66,64,68 -----------------------------------------------------------------------
	BYTE COL_AN2[16];		// カラーコード(アルファニューメリック 60m)
	BYTE COL_CG2[2][16];	// カラーコード(カラーグラフィック 60m)
	// ---------------------------------------------------------------------------------------
	
	static const BYTE NJM_TBL[][2];	// 色にじみカラーコードテーブル
	
	bool CrtDisp;			// CRT表示状態			true:表示	false:非表示
	bool BusReq;			// バスリクエスト		true:ON		false:OFF
	bool N60Win;			// ウィンドウサイズ		true:N60	false:N60m
	int Mode4Col;			// モード4カラーモード	0:モノ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク
	
	WORD VAddr;				// 垂直アドレスカウンタ
	WORD HAddr;				// 水平アドレスカウンタ
	int RowCntA;			// 表示ラインカウンタ(アルファニューメリック,セミグラ)
	int RowCntG;			// 表示ラインカウンタ(グラフィック)
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	bool CharMode;			// テキスト表示モード	true:キャラクタ				false:グラフィック
	bool GraphMode;			// グラフィック解像度	true:160*200(SR:320*200)	false:320*200(SR:640*200)
	int Css1;				// 色の組み合わせ
	int Css2;
	int Css3;
	// ---------------------------------------------------------------------------------------
	
	// for 64,68 -----------------------------------------------------------------------------
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
	// ---------------------------------------------------------------------------------------
	
	// アトリビュートデータ
	BYTE AT_AG;
	BYTE AT_AS;
	BYTE AT_IE;
	BYTE AT_GM;
	BYTE AT_CSS;
	BYTE AT_INV;
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	void SetCss( BYTE );						// 色の組合せ指定
	virtual void SetCrtControler( BYTE );		// CRTコントローラモード設定
	// ---------------------------------------------------------------------------------------
	
	int GetW() const;							// バックバッファ幅取得(規定値)
	int GetH() const;							// バックバッファ高さ取得(規定値)
	
	virtual BYTE *GetBufAddr() const = 0;		// バッファアドレス取得
	virtual int GetBufPitch() const = 0;		// バッファピッチ(1Lineバイト数)取得
	
	virtual void LatchGMODE() = 0;				// アトリビュートデータラッチ(グラフィックモードのみ)
	virtual void LatchAttr() = 0;				// アトリビュートデータラッチ
	virtual BYTE GetAttr() const = 0;			// アトリビュートデータ取得
	virtual BYTE GetVram() const = 0;			// VRAMデータ取得
	virtual BYTE GetFont1( WORD ) const = 0;	// Font1データ取得
	virtual BYTE GetFont2( WORD ) const = 0;	// Font2データ取得
	virtual BYTE GetFont3( WORD ) const = 0;	// Font3データ取得
	BYTE GetSemi4( BYTE ) const;				// セミグラ4データ取得
	virtual BYTE GetSemi6( BYTE ) const;		// セミグラ6データ取得
	
public:
	MC6847core();								// コンストラクタ
	virtual ~MC6847core();						// デストラクタ
	
	virtual void UpdateBackBuf() = 0;			// バックバッファ更新
	
	bool GetCrtDisp() const;					// CRT表示状態取得
	void SetCrtDisp( bool );					// CRT表示状態設定
	
	bool GetWinSize() const;					// ウィンドウサイズ取得
	
	int GetMode4Color() const;					// モード4カラーモード取得
	void SetMode4Color( int );					// モード4カラーモード設定
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
	MC6847();									// コンストラクタ
	virtual ~MC6847();							// デストラクタ
	
	void UpdateBackBuf();						// バックバッファ更新
};


class PCZ80_07 : public virtual MC6847core {
protected:
	BYTE GetSemi6( BYTE ) const;				// セミグラ6データ取得
	
	void Draw1line1( int );						// 1ライン描画(N60)
	void Draw1line2( int );						// 1ライン描画(N60m)
	
	#if INBPP == 8	// 8bit
	BYTE GetBcol() const;						// ボーダーカラー取得
	#else			// 32bit
	DWORD GetBcol() const;						// ボーダーカラー取得
	#endif
	
public:
	PCZ80_07();									// コンストラクタ
	virtual ~PCZ80_07();						// デストラクタ
	
	virtual void UpdateBackBuf();				// バックバッファ更新
};


class PCZ80_12 : public virtual PCZ80_07 {
protected:
	BYTE GetSemi6( BYTE ) const;				// セミグラ6データ取得
	BYTE GetSemi8( BYTE ) const;				// セミグラ8データ取得
	
	void SetCrtControler( BYTE );				// CRTコントローラモード設定
	void SetCrtCtrlType( BYTE );				// CRTコントローラタイプ設定
	
	void Draw1line3( int );						// 1ライン描画(SR)
	
public:
	PCZ80_12();									// コンストラクタ
	virtual ~PCZ80_12();						// デストラクタ
	
	void UpdateBackBuf();						// バックバッファ更新
	
	void SetPalette( int, BYTE );				// パレット設定
};


#endif	// MC6847_H_INCLUDED
