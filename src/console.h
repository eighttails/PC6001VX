#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include "typedef.h"
#include "vsurface.h"

#define	FSIZE		(6)	// 半角フォントの幅

// 色定義
#define	FC_BLACK	0x00000000
#define	FC_DBLUE	(0x8f8f8f8f &  BMASK32)
#define	FC_DGREEN	(0x8f8f8f8f &  GMASK32)
#define	FC_DCYAN	(0x8f8f8f8f & (GMASK32|BMASK32))
#define	FC_DRED		(0x8f8f8f8f &  RMASK32)
#define	FC_DMAGENTA	(0x8f8f8f8f & (RMASK32|BMASK32))
#define	FC_DYELLOW	(0x8f8f8f8f & (RMASK32|GMASK32))
#define	FC_GRAY		(0x8f8f8f8f & (RMASK32|GMASK32|BMASK32))
#define	FC_BLUE		(0xffffffff &  BMASK32)
#define	FC_GREEN	(0xffffffff &  GMASK32)
#define	FC_CYAN		(0xffffffff & (GMASK32|BMASK32))
#define	FC_RED		(0xffffffff &  RMASK32)
#define	FC_MAGENTA	(0xffffffff & (RMASK32|BMASK32))
#define	FC_YELLOW	(0xffffffff & (RMASK32|GMASK32))
#define	FC_WHITE	(0xffffffff & (RMASK32|GMASK32|BMASK32))



////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class JFont {
protected:
	static VSurface *ZFont;		// 全角フォントデータサーフェスへのポインタ
	static VSurface *HFont;		// 半角フォントデータサーフェスへのポインタ
	
	static int zWidth, zHeight;	// 全角文字の幅,高さ
	static int hWidth, hHeight;	// 半角文字の幅,高さ
	
public:
	JFont();									// コンストラクタ
	~JFont();									// デストラクタ
	
	static bool OpenFont( char *, char * );		// ファイルファイルを開く
	static void CloseFont();					// フォントを破棄する
	
	static int FontWidth(){ return hWidth; }	// フォントの幅取得(半角)
	static int FontHeight(){ return hHeight; }	// フォントの高さ取得
	
	void PutCharh( VSurface *, int, int, BYTE, DWORD, DWORD );	// 半角文字描画
	void PutCharz( VSurface *, int, int, WORD, DWORD, DWORD );	// 全角文字描画

};


class ZCons : public JFont, public VSurface {
protected:
	VRect con;					// 描画範囲
	int Xmax,Ymax;				// 縦横最大文字数(半角)
	int x,y;					// カーソル位置
	DWORD fgc,bgc;				// 描画色と背景色
	char Caption[129];			// キャプション
	
	void DrawFrame();							// 枠描画
	void ScrollUp();							// スクロールアップ
	
public:
	ZCons();									// コンストラクタ
	virtual ~ZCons();							// デストラクタ
	
	bool Init   ( int, int, const char *, DWORD = FC_WHITE, DWORD = FC_BLACK );	// 初期化(文字数でサイズ指定)
	bool InitRes( int, int, const char *, DWORD = FC_WHITE, DWORD = FC_BLACK );	// 初期化(解像度でサイズ指定)
	
	void Locate( int, int );					// カーソル位置設定
	void LocateR( int, int );					// カーソル位置設定(相対座標)
	void SetColor( DWORD, DWORD );				// 描画色設定
	void SetColor( DWORD );
	void Cls();									// 画面消去
	
	void PutCharH( BYTE );						// 半角1文字描画
	void PutCharZ( WORD );						// 全角1文字描画
	
	void Print( const char *, ... );			// 書式付文字列描画(制御文字非対応)
	void Printf( const char *, ... );			// 書式付文字列描画(制御文字対応)
	void Printfr( const char *, ... );			// 書式付文字列描画(右詰め)
	
	int GetXline();								// 横最大文字数取得
	int GetYline();								// 縦最大文字数取得
};


#endif	// CONSOLE_H_INCLUDED
