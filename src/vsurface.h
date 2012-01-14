#ifndef VSURFACE_H_INCLUDED
#define VSURFACE_H_INCLUDED

#include "typedef.h"


// サーフェスの仕様
//  色深度は8,16,24bit
//  8bitの時のみパレットを持つ
//  1ラインのメモリサイズは4byte(DWORD)の倍数とする
//  最大サイズは65536x65536


// 矩形領域構造体
typedef struct{
	int x, y;		// 描画領域座標
	int w, h;		// 描画領域サイズ
} VRect;

typedef struct{
  int ncols;
  COLOR24 *colors;
} VPalette;


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class VSurface {
protected:
	int w, h;					// 幅,高さ
	int bpp;					// 1ピクセルのbyte数
	int pitch;					// 1ラインのbyte数
	void *pixels;				// ピクセルデータへのポインタ
	
	VRect rect;					// 描画領域
	VPalette *palette;			// パレット(8bppのみ)
	
	DWORD rmask, gmask, bmask;	// マスク
	int rshift, gshift, bshift;	// 右シフト数
	int rloss, gloss, bloss;	// 左シフト数
	
public:
	VSurface();									// コンストラクタ
	virtual ~VSurface();						// デストラクタ
	
	virtual BOOL InitSurface( int, int, int );	// 初期化(サーフェスサイズ指定)
	void SetRect( int, int, int, int );			// 描画領域設定
	VRect *GetRect();							// 描画領域取得
	int SetPalette( COLOR24 *, int = 256 );		// パレット設定
	VPalette *GetPalette();						// パレット取得
	
	void SetMask( DWORD, DWORD, DWORD );		// カラーマスク設定
	DWORD Rmask();								// Rマスク取得
	DWORD Gmask();								// Gマスク取得
	DWORD Bmask();								// Bマスク取得
	int Rshift();								// R右シフト数取得
	int Gshift();								// G右シフト数取得
	int Bshift();								// B右シフト数取得
	int Rloss();								// R左シフト数取得
	int Gloss();								// G左シフト数取得
	int Bloss();								// B左シフト数取得
	
	int Width();								// 幅取得
	int Height();								// 高さ取得
	int Bpp();									// 1ピクセルのbyte数取得
	int Pitch();								// 1ラインのbyte数取得
	void *GetPixels();							// ピクセルデータへのポインタ取得
	
	void PSet( int, int, DWORD );				// 点を描く
	DWORD PGet( int, int );						// 色を取得
	void Fill( DWORD, VRect * = NULL );			// 矩形領域を塗りつぶす
	void Blit( VRect *, VSurface *, VRect * );	// 転送する
};


#endif	// VSURFACE_H_INCLUDED
