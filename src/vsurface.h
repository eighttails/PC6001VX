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
	
	VRect rect;					// 描画領域
	
public:
	VSurface();									// コンストラクタ
	virtual ~VSurface();						// デストラクタ
	
	virtual bool InitSurface( int, int );		// 初期化(サーフェスサイズ指定)
	void SetRect( int, int, int, int );			// 描画領域設定
    double GetAspectRatio();
    void SetAspectRatio(double);                // アスペクト比を設定
	VRect *GetRect();							// 描画領域取得
	
	int Width();								// 幅取得
	int Height();								// 高さ取得
	int Pitch();								// 1ラインのbyte数取得
	void *GetPixels();							// ピクセルデータへのポインタ取得
	
	void PSet( int, int, DWORD );				// 点を描く
	DWORD PGet( int, int );						// 色を取得
	void Fill( DWORD, VRect * = NULL );			// 矩形領域を塗りつぶす
	void Blit( VRect *, VSurface *, VRect * );	// 転送する
};


#endif	// VSURFACE_H_INCLUDED
