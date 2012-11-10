#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "typedef.h"
#include "console.h"
#include "p6device.h"


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //

#define	REGWINW	(40)
#define	REGWINH	( 8)
#define	MEMWINW	(40)
#define	MEMWINH	(15)
#define	MONWINW	(100)
#define	MONWINH	(30)

#define MAX_ARGS	(8)
#define	MAX_CHRS	(256)
#define	MAX_HIS		(11)
#define	PROMPT		"P6V>"

//------------------------------------------------------
//  メモリダンプウィンドウクラス
//------------------------------------------------------
class cWndMem : public P6DEVICE, public ZCons {
private:
	WORD Addr;							// 表示アドレス
	
public:
	cWndMem( VM6 *, const P6ID& );		// コンストラクタ
	~cWndMem();							// デストラクタ
	
	bool Init();						// 初期化
	void Update();						// ウィンドウ更新
	
	void SetAddress( WORD addr ){ Addr = addr & 0xfff8; }	// 表示アドレス設定
	WORD GetAddress(){ return Addr; }						// 表示アドレス取得
};


//------------------------------------------------------
//  レジスタウィンドウクラス
//------------------------------------------------------
class cWndReg : public P6DEVICE, public ZCons {
private:
	
public:
	cWndReg( VM6 *, const P6ID& );		// コンストラクタ
	~cWndReg();							// デストラクタ
	
	bool Init();						// 初期化
	void Update();						// ウィンドウ更新
};


//------------------------------------------------------
//  モニタウィンドウクラス
//------------------------------------------------------
class cWndMon : public P6DEVICE, public ZCons {
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
	cWndMon( VM6 *, const P6ID& );		// コンストラクタ
	~cWndMon();							// デストラクタ
	
	bool Init();						// 初期化
	void Update();						// ウィンドウ更新
	
	void KeyIn( int, bool, int );		// キー入力処理
	
	void BreakIn( WORD );				// ブレークポイント到達
};
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //

#endif	// DEBUG_H_INCLUDED
