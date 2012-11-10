#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include "typedef.h"
#include "vsurface.h"

#define	FSIZE		(6)	// 半角フォントの幅

// 色定義
#define	FC_BLACK	(0)
#define	FC_DBLUE	(1)
#define	FC_DGREEN	(2)
#define	FC_DCYAN	(3)
#define	FC_DRED		(4)
#define	FC_DMAGENTA	(5)
#define	FC_DYELLOW	(6)
#define	FC_GRAY		(7)
#define	FC_BLUE		(9)
#define	FC_GREEN	(10)
#define	FC_CYAN		(11)
#define	FC_RED		(12)
#define	FC_MAGENTA	(13)
#define	FC_YELLOW	(14)
#define	FC_WHITE	(15)



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
	
	void PutCharh( VSurface *, int, int, BYTE, BYTE, BYTE );	// 半角文字描画
	void PutCharz( VSurface *, int, int, WORD, BYTE, BYTE );	// 全角文字描画

};


class ZCons : public JFont, public VSurface {
protected:
	VRect con;					// 描画範囲
	int Xmax,Ymax;				// 縦横最大文字数(半角)
	int x,y;					// カーソル位置
	BYTE fgc,bgc;				// 描画色と背景色
	char Caption[129];			// キャプション
	
	void DrawFrame();							// 枠描画
	void ScrollUp();							// スクロールアップ
	
public:
	ZCons();									// コンストラクタ
	virtual ~ZCons();							// デストラクタ
	
	bool Init   ( int, int, const char *, int = FC_WHITE, int = FC_BLACK );	// 初期化(文字数でサイズ指定)
	bool InitRes( int, int, const char *, int = FC_WHITE, int = FC_BLACK );	// 初期化(解像度でサイズ指定)
	
	void Locate( int, int );					// カーソル位置設定
	void LocateR( int, int );					// カーソル位置設定(相対座標)
	void SetColor( BYTE, BYTE );				// 描画色設定
	void SetColor( BYTE );
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
