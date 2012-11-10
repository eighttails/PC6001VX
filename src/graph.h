#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "typedef.h"
#include "p6vm.h"
#include "vsurface.h"



////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class DSP6 {
protected:
	VM6* vm;
	
	HWINDOW Wh;				// ウィンドウハンドル
	VSurface *SBuf;			// サブバッファポインタ
	VPalette *Pal;			// パレットへのポインタ
	int Bpp;				// カラーモード
	int SLBr;				// スキャンライン輝度
	
	bool SetScreenSurface();				// スクリーンサーフェス作成
	bool UpdateSubBuf();					// サブバッファ更新
	bool RefreshSubBuf();					// サブバッファリフレッシュ
	

public:
	DSP6( VM6 * );							// コンストラクタ
	~DSP6();								// デストラクタ
	
	bool Init( int, int, VPalette * );		// 初期化
	void SetIcon( int );					// アイコン設定
	
	bool ResizeScreen();					// スクリーンサイズ変更
	
	void DrawScreen();						// 画面更新
	void SnapShot( char * );				// スナップショット
	
	int ScreenX();							// 有効スクリーン幅取得
	int ScreenY();							// 有効スクリーン高さ取得
	
	VSurface *GetSubBuffer();				// サブバッファ取得
	HWINDOW GetWindowHandle();				// ウィンドウハンドル取得
};


#endif	// GRAPH_H_INCLUDED
