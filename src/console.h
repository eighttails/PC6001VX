/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include <string>

#include "typedef.h"
#include "vsurface.h"

#define	FSIZE		(6)	// 半角フォントの幅

// 色定義
enum SystemColor {
	FC_BLACK = 128,
	
	FC_BLUE1,
	FC_GREEN1,
	FC_CYAN1,
	FC_RED1,
	FC_MAGENTA1,
	FC_YELLOW1,
	FC_WHITE1,
	
	FC_BLUE2,
	FC_GREEN2,
	FC_CYAN2,
	FC_RED2,
	FC_MAGENTA2,
	FC_YELLOW2,
	FC_WHITE2,
	
	FC_BLUE3,
	FC_GREEN3,
	FC_CYAN3,
	FC_RED3,
	FC_MAGENTA3,
	FC_YELLOW3,
	FC_WHITE3,
	
	FC_BLUE4,
	FC_GREEN4,
	FC_CYAN4,
	FC_RED4,
	FC_MAGENTA4,
	FC_YELLOW4,
	FC_WHITE4
};




/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class JFont {
protected:
	static VSurface ZFont;		// 全角フォントデータサーフェス
	static VSurface HFont;		// 半角フォントデータサーフェス
	
	static int zWidth, zHeight;	// 全角文字の幅,高さ
	static int hWidth, hHeight;	// 半角文字の幅,高さ

public:
	JFont();
	~JFont();
	
	static bool OpenFont( const P6VPATH&, const P6VPATH& );	// フォントファイルを開く
	
	static int FontWidth(){ return hWidth; }	// フォントの幅取得(半角)
	static int FontHeight(){ return hHeight; }	// フォントの高さ取得(半角)
};


class ZCons : public JFont, public VSurface {
protected:
	VRect con;					// 描画範囲
	int Xmax, Ymax;				// 縦横最大文字数(半角)
	int x, y;					// カーソル位置
	BYTE fgc, bgc;				// 描画色と背景色
	std::string Caption;		// キャプション
	
	void DrawFrame();							// 枠描画
	void ScrollUp();							// スクロールアップ
	
	void PutCharh( int, int, BYTE, BYTE,  BYTE );	// 半角文字描画
	void PutCharz( int, int, WORD, BYTE,  BYTE );	// 全角文字描画
	
	void sprintc( const std::string& );			// 文字列描画(制御文字対応)
	void sprintr( const std::string& );			// 文字列描画(右詰め)

public:
	ZCons();
	virtual ~ZCons();
	
	bool Init   ( int, int, const std::string&, BYTE = FC_WHITE4, BYTE = FC_BLACK );	// 初期化(文字数でサイズ指定)
	bool InitRes( int, int, const std::string&, BYTE = FC_WHITE4, BYTE = FC_BLACK );	// 初期化(解像度でサイズ指定)
	void SetColor( BYTE, BYTE );				// 描画色設定
	void SetColor( BYTE );
	
	void Locate( int, int );					// カーソル位置設定
	void LocateR( int, int );					// カーソル位置設定(相対座標)
	void Cls();									// 画面消去
	
	void PutCharH( BYTE );						// 半角1文字描画
	void PutCharZ( WORD );						// 全角1文字描画(SJIS)
	
	template <typename ... Args> void Printf( const std::string& fmt, Args ... args )	// 書式付文字列描画
	{
		std::vector<char> buf( std::snprintf( nullptr, 0, fmt.c_str(), args ... ) + 1 );
		std::snprintf( &buf[0], buf.size(), fmt.c_str(), args ... );
		sprintc( std::string( buf.data(), buf.data() + buf.size() - 1 ) );
	}
	
	template <typename ... Args> void PrintfR( const std::string& fmt, Args ... args )	// 書式付文字列描画(右詰め)
	{
		std::vector<char> buf( std::snprintf( nullptr, 0, fmt.c_str(), args ... ) + 1 );
		std::snprintf( &buf[0], buf.size(), fmt.c_str(), args ... );
		sprintr( std::string( buf.data(), buf.data() + buf.size() - 1 ) );
	}
	
	int GetXline();								// 横最大文字数取得
	int GetYline();								// 縦最大文字数取得
};


#endif	// CONSOLE_H_INCLUDED
