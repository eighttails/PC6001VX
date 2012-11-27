#ifndef MC6847_H_INCLUDED
#define MC6847_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cMC6847core {
protected:
	BYTE COL_AN[5];			// カラーコード(アルファニューメリック)
	BYTE COL_SG[9];			// カラーコード(セミグラフィック)
    BYTE COL_CG[10][8];		// カラーコード(カラーグラフィック)
	BYTE COL_RG[2][2];		// カラーコード(モノクログラフィック)

	bool CrtDisp;			// CRT表示状態 true:表示 false:非表示
	bool N60Win;			// ウィンドウサイズ true:N60 false:N60m
	int Mode4Col;			// モード4カラーモード 0:モノ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク
	
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
	
	int GetW();									// バックバッファ幅取得(規定値)
	int GetH();									// バックバッファ高さ取得(規定値)
	
	virtual BYTE *GetBufAddr() = 0;				// バッファアドレス取得
	virtual int GetBufPitch() = 0;				// バッファピッチ(1Lineバイト数)取得
	virtual int GetBufHeight() = 0;				// バッファ高さ取得
	
	virtual void LatchGMODE() = 0;				// アトリビュートデータラッチ(グラフィックモードのみ)
	virtual void LatchAttr() = 0;				// アトリビュートデータラッチ
	virtual BYTE GetAttr() = 0;					// アトリビュートデータ取得
	virtual BYTE GetVram() = 0;					// VRAMデータ取得
	virtual BYTE GetFont1( WORD ) = 0;			// Font1データ取得
	
public:
	cMC6847core();								// コンストラクタ
	virtual ~cMC6847core();						// デストラクタ
	
	virtual void UpdateBackBuf() = 0;			// バックバッファ更新
	
	void SetCrtDisp( bool );					// CRT表示状態設定
	
	bool GetWinSize();							// ウィンドウサイズ取得
	
	int GetMode4Color();						// モード4カラーモード取得
	void SetMode4Color( int );					// モード4カラーモード設定
};


class cMC6847_1 : public virtual cMC6847core {
protected:
	virtual BYTE GetFont0( WORD ) = 0;			// Font0(VDG Font)データ取得
	BYTE GetBcol();								// ボーダーカラー取得
	
	void Draw1line1( int );						// 1ライン描画(N60)
	
public:
	cMC6847_1();								// コンストラクタ
	virtual ~cMC6847_1();						// デストラクタ
	
	void UpdateBackBuf();						// バックバッファ更新
};


class cMC6847_2 : public virtual cMC6847core {
protected:
	BYTE COL_AN2[16];		// カラーコード(アルファニューメリック 60m)
	BYTE COL_CG3[2][16];	// カラーコード(カラーグラフィック 60m モード3)
	BYTE COL_CG4[2][16];	// カラーコード(カラーグラフィック 60m モード4)
	
	bool Mk2CharMode;		// mk2 表示モード true:キャラクタ false:グラフィック
	bool Mk2GraphMode;		// mk2 グラフィック解像度 true:160*200 false:320*200
	
	int Css1;				// 色の組み合わせ
	int Css2;
	int Css3;
	
	virtual BYTE GetFont2( WORD ) = 0;			// Font2データ取得
	BYTE GetBcol();								// ボーダーカラー取得
	
	void Draw1line1( int );						// 1ライン描画(N60)
	void Draw1line2( int );						// 1ライン描画(N60m)
	
public:
	cMC6847_2();								// コンストラクタ
	virtual ~cMC6847_2();						// デストラクタ
	
	void UpdateBackBuf();						// バックバッファ更新
};


#endif	// MC6847_H_INCLUDED
