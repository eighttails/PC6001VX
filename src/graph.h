/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <memory>
#include <string>

#include "p6vm.h"
#include "typedef.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class DSP6 {
protected:
	EL6* el;
	HWINDOW Wh;
	bool rsize;					// リサイズ通知
	
	bool SetScreenSurface();				// スクリーンサーフェス作成
	
public:
	DSP6( EL6* );
	~DSP6();
	
	bool Init();							// 初期化
	void SetIcon( const int );				// アイコン設定
	
	bool ResizeScreen();					// スクリーンサイズ変更
	bool CheckResize();						// ResizeScreen()でリサイズしたかチェック
	
	void DrawScreen();						// 画面更新
	void SnapShot( const P6VPATH& );		// スナップショット
	
	int ScreenX() const;					// 有効スクリーン幅取得
	int ScreenY() const;					// 有効スクリーン高さ取得
	
	HWINDOW GetWindowHandle();				// ウィンドウハンドル取得
};


#endif	// GRAPH_H_INCLUDED
