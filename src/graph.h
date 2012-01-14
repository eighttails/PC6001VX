#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "typedef.h"
#include "p6device.h"
#include "vsurface.h"



////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class DSP6 : public P6DEVICE {
protected:
	VSurface *SBuf;			// サブバッファポインタ
	COLOR24 *Pal;			// パレットへのポインタ
	int Bpp;				// カラーモード
	int SLBr;				// スキャンライン輝度
	
	BOOL SetScreenSurface();				// スクリーンサーフェス作成
	BOOL UpdateSubBuf();					// サブバッファ更新
	BOOL RefreshSubBuf();					// サブバッファリフレッシュ
	
	void BlitToSDL( VSurface *, int, int );	// SDLサーフェスにVSurfaceをBlit
	void BlitToSDL2( VSurface *, int, int );	// SDLサーフェスにVSurfaceをBlit 2倍

public:
	DSP6( VM6 *, const P6ID& );				// コンストラクタ
	~DSP6();								// デストラクタ
	
	BOOL Init( int, int, COLOR24 * );		// 初期化
	
	BOOL ResizeScreen();					// スクリーンサイズ変更
	
	void DrawScreen();						// 画面更新
	void SnapShot( char * );				// スナップショット
	
	int ScreenX();							// 有効スクリーン幅取得
	int ScreenY();							// 有効スクリーン高さ取得
	
	VSurface *GetSubBuffer();				// サブバッファ取得
};


#endif	// GRAPH_H_INCLUDED
