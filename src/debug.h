/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <memory>
#include <string>
#include <vector>

#include "console.h"
#include "device.h"
#include "typedef.h"


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class EL6;

//---------------------------------------------------------------------------
//  モニタモードウィンドウ インターフェース(?)クラス
//---------------------------------------------------------------------------
class iMon : public ZCons {
protected:
	EL6* el;
	
private:
	int x;								// X座標(モニタモード内での表示位置)
	int y;								// Y座標(モニタモード内での表示位置)
	
public:
	iMon( EL6* );
	~iMon();
	
	int X();							// X座標取得
	int Y();							// Y座標取得
	void SetX( int );					// X座標設定
	void SetY( int );					// Y座標設定
	
	virtual bool Init() = 0;			// 初期化
	virtual void Update() = 0;			// ウィンドウ更新
};


//---------------------------------------------------------------------------
//  メモリダンプウィンドウクラス
//---------------------------------------------------------------------------
class cWndMem : public iMon {
private:
	WORD Addr;							// 表示アドレス
	
public:
	cWndMem( EL6* );
	~cWndMem();
	
	bool Init() override;				// 初期化
	void Update() override;				// ウィンドウ更新
	
	void SetAddress( WORD );			// 表示アドレス設定
	WORD GetAddress();					// 表示アドレス取得
};


//---------------------------------------------------------------------------
//  レジスタウィンドウクラス
//---------------------------------------------------------------------------
class cWndReg : public iMon {
private:
	
public:
	cWndReg( EL6* );
	~cWndReg();
	
	bool Init() override;				// 初期化
	void Update() override;				// ウィンドウ更新
};


//---------------------------------------------------------------------------
//  モニタウィンドウクラス
//---------------------------------------------------------------------------
class cWndMon : public iMon {
private:
	std::string KeyBuf;					// キー入力バッファ
	std::vector<std::string> HisBuf;	// キー入力ヒストリバッファ
	std::vector<std::string> Argv;		// 引数へのポインタ配列
	int ArgvCounter;					// Shift()用カウンタ
	
	struct argv{						// 引数処理用
		int Type;			// 引数の種類		ARGV_xxx
		int Val;			// 引数の値			ARG_xxx または、数
		std::string Str;	// 引数の文字列		Argv[xxx]と同じ
		argv() : Type(0), Val(0), Str("") {}
	} argv;
	
	int GetArg();						// 引数処理
	void Shift();						// 引数配列シフト
	void Exec( int );					// コマンド実行
	void Help( int );					// ヘルプ表示
	
public:
	cWndMon( EL6* );
	~cWndMon();
	
	bool Init() override;				// 初期化
	void Update() override;				// ウィンドウ更新
	
	void KeyIn( int, int );				// キー入力処理
	
	void BreakIn( WORD );				// ブレークポイント到達
};


#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#endif	// DEBUG_H_INCLUDED
