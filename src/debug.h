#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "console.h"


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define MAX_ARGS	(8)
#define	MAX_CHRS	(256)
#define	MAX_HIS		(11)

//------------------------------------------------------
//  モニタモードウィンドウ インターフェース(?)クラス
//------------------------------------------------------
class iMon : public Device, public ZCons {
private:
	int x;								// X座標(モニタモード内での表示位置)
	int y;								// Y座標(モニタモード内での表示位置)
	
public:
	iMon( VM6 *, const ID& );			// コンストラクタ
	~iMon();							// デストラクタ
	
	int X();							// X座標取得
	int Y();							// Y座標取得
	void SetX( int );					// X座標設定
	void SetY( int );					// Y座標設定
	
	virtual bool Init() = 0;			// 初期化
	virtual void Update() = 0;			// ウィンドウ更新
};


//------------------------------------------------------
//  メモリダンプウィンドウクラス
//------------------------------------------------------
class cWndMem : public iMon {
private:
	WORD Addr;							// 表示アドレス
	
public:
	cWndMem( VM6 *, const ID& );		// コンストラクタ
	~cWndMem();							// デストラクタ
	
	bool Init();						// 初期化
	void Update();						// ウィンドウ更新
	
	void SetAddress( WORD );			// 表示アドレス設定
	WORD GetAddress();					// 表示アドレス取得
};


//------------------------------------------------------
//  レジスタウィンドウクラス
//------------------------------------------------------
class cWndReg : public iMon {
private:
	
public:
	cWndReg( VM6 *, const ID& );		// コンストラクタ
	~cWndReg();							// デストラクタ
	
	bool Init();						// 初期化
	void Update();						// ウィンドウ更新
};


//------------------------------------------------------
//  モニタウィンドウクラス
//------------------------------------------------------
class cWndMon : public iMon {
private:
	char KeyBuf[MAX_CHRS];				// キー入力バッファ
	char HisBuf[MAX_HIS][MAX_CHRS];		// キー入力ヒストリバッファ
	char *Argv[MAX_ARGS];				// 引数へのポインタ配列
	int Argc;							// 引数の個数
	int ArgvCounter;					// Shift()用カウンタ
	
	struct argv{						// 引数処理用
		int Type;	// 引数の種類		ARGV_xxx
		int Val;	// 引数の値			ARG_xxx または、数
		char *Str;	// 引数の文字列		Argv[xxx]と同じ
		argv() : Type(0), Val(0), Str(NULL) {}
	} argv;
	
	int GetArg();						// 引数処理
	void Shift();						// 引数配列シフト
	void Exec( int );					// コマンド実行
	void Help( int );					// ヘルプ表示
	
public:
	cWndMon( VM6 *, const ID& );		// コンストラクタ
	~cWndMon();							// デストラクタ
	
	bool Init();						// 初期化
	void Update();						// ウィンドウ更新
	
	void KeyIn( int, bool, int );		// キー入力処理
	
	void BreakIn( WORD );				// ブレークポイント到達
};







//------------------------------------------------------
//  モニタモードクラス
//------------------------------------------------------
class Monitor {
private:
	VM6 *vm;
	iMon *dcn[3];
	
public:
	Monitor( VM6 * );					// コンストラクタ
	~Monitor();							// デストラクタ
	
	bool Init();						// 初期化
	void Update();						// ウィンドウ更新
	
	int Width();						// モニタモード ウィンドウ幅取得
	int Height();						// モニタモード ウィンドウ高さ取得
	
	// メモリウィンドウ
	void SetAddress( WORD );			// 表示アドレス設定
	WORD GetAddress();					// 表示アドレス取得
	
	// モニタウィンドウ
	void KeyIn( int, bool, int );		// キー入力処理
	void BreakIn( WORD );				// ブレークポイント到達
};




#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#endif	// DEBUG_H_INCLUDED
