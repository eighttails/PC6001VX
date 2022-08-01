/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef VSURFACE_H_INCLUDED
#define VSURFACE_H_INCLUDED


#include <algorithm>
#include <array>
#include <vector>

#include "typedef.h"

// サーフェスの仕様
//  色深度は8bitのみ
//  最大サイズは65536x65536

#define	AMASK32		0xff000000
#define	RMASK32		0x00ff0000
#define	GMASK32		0x0000ff00
#define	BMASK32		0x000000ff

#define	ASHIFT32	24
#define	RSHIFT32	16
#define	GSHIFT32	8
#define	BSHIFT32	0

struct COLOR24 {
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
};

// 矩形領域構造体
struct VRect{
	int x, y;		// 描画領域座標
	int w, h;		// 描画領域サイズ
	
	VRect() : x(0), y(0), w(0), h(0) {}
};

#define COL2DW(c)	(DWORD)(((DWORD)c.a<<ASHIFT32)|((DWORD)c.r<<RSHIFT32)|((DWORD)c.g<<GSHIFT32)|((DWORD)c.b<<BSHIFT32))


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class VSurface {
protected:
	int w, h;					// 幅,高さ
	int pitch;					// 1ラインのbyte数
	std::vector<BYTE> pixels;	// ピクセルデータへのポインタ
	VRect rect;					// 描画領域
	
	static std::array<DWORD,256> col32;	// 32bitカラーテーブル
	
	void RectAdd( VRect*, VRect*, VRect* );		// 矩形領域合成
	
public:
	VSurface();
	virtual ~VSurface();
	
	virtual bool InitSurface( int, int );		// 初期化(サーフェスサイズ指定)
	void SetRect( int, int, int, int );			// 描画領域設定
	VRect* GetRect();							// 描画領域取得
	
	int Width() const;							// 幅取得
	int Height() const;							// 高さ取得
	int Pitch() const;							// 1ラインのbyte数取得
	std::vector<BYTE>& GetPixels();				// ピクセルデータ取得
	
	void PSet( int, int, BYTE );				// 点を描く
	BYTE PGet( int, int );						// 色を取得
	void Fill( BYTE, VRect* = nullptr );		// 矩形領域を塗りつぶす
	
	void Blit( VRect*, VSurface*, VRect* );		// 転送する
	
	static void SetColor( int, DWORD );			// 32bitカラー設定
	static DWORD GetColor( int );				// 32bitカラー取得
};


#endif	// VSURFACE_H_INCLUDED
