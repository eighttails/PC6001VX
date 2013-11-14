#ifndef VSURFACE_H_INCLUDED
#define VSURFACE_H_INCLUDED

#include "typedef.h"

// サーフェスの仕様
//  色深度は32bitのみ
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
	BYTE reserved;
};

// 矩形領域構造体
struct VRect{
	int x, y;		// 描画領域座標
	int w, h;		// 描画領域サイズ
	
	VRect() : x(0), y(0), w(0), h(0) {}
};

struct VPalette{
	int ncols;
	COLOR24 *colors;
	
	VPalette() : ncols(0), colors(NULL) {}
};

#define COL2DW(c)	(DWORD)(((DWORD)c.reserved<<ASHIFT32)|((DWORD)c.r<<RSHIFT32)|((DWORD)c.g<<GSHIFT32)|((DWORD)c.b<<BSHIFT32))

////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class VSurface {
protected:
	int w, h;					// 幅,高さ
	int pitch;					// 1ラインのbyte数
    double aspect;              // アスペクト比(幅を1とした場合の高さの比率)
	void *pixels;				// ピクセルデータへのポインタ
	int xscale;					// 幅倍率(1:等倍 2:2倍)
	VRect rect;					// 描画領域
	
	static DWORD col32[256];	// 32bitカラーテーブル
	
public:
	VSurface();										// コンストラクタ
	virtual ~VSurface();							// デストラクタ
	
	virtual bool InitSurface( int, int, int=1 );	// 初期化(サーフェスサイズ指定)
	void SetRect( int, int, int, int );				// 描画領域設定
	VRect *GetRect();								// 描画領域取得
	
	int Width() const;								// 幅取得
	int Height() const;								// 高さ取得
	int Pitch() const;								// 1ラインのbyte数取得
	int XScale() const;								// 幅倍率取得
	void *GetPixels() const;						// ピクセルデータへのポインタ取得
	
	#if INBPP == 8	// 8bit
	void PSet( int, int, BYTE );					// 点を描く
	BYTE PGet( int, int );							// 色を取得
	void Fill( BYTE, VRect * = NULL );				// 矩形領域を塗りつぶす
	#else			// 32bit
	void PSet( int, int, DWORD );					// 点を描く
	DWORD PGet( int, int );							// 色を取得
	void Fill( DWORD, VRect * = NULL );				// 矩形領域を塗りつぶす
	#endif
	
	void Blit( VRect *, VSurface *, VRect * );		// 転送する
	
	static void SetColor( int, DWORD );				// 32bitカラーテーブル設定
	static DWORD GetColor( int );					// 32bitカラー取得
};


#endif	// VSURFACE_H_INCLUDED
