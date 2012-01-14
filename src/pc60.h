#ifndef PC60_H_INCLUDED
#define PC60_H_INCLUDED

#include <SDL.h>

#include "typedef.h"

#include "config.h"
#include "joystick.h"
#include "movie.h"
#include "replay.h"
#include "thread.h"
#include "vsurface.h"


#define	FRAMERATE	(VSYNC_HZ/(cfg->GetFrameSkip()+1))


class CPU6;
class SUB6;
class IO6;
class MEM6;
class VDG6;
class DSP6;
class INT6;
class KEY6;
class DSK6;
class CMTL;
class CMTS;
class SND6;
class PSG6;
class PIO6;
class SCH6;
class VCE6;
class cWndStat;

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
class BPoint;
class cWndReg;
class cWndMem;
class cWndMon;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
// 基本仮想マシンクラス
class VM6 : public cThread, public AVI6, public REPLAY {
	
	// vm->xxx を使うにはfriendでなきゃならん。ちょっとイヤ。
	friend class VDG6;
	friend class VDG60;
	friend class VDG62;
	friend class DSP6;
	friend class SUB6;
	friend class PIO6;
	friend class CPU6;
	friend class PSG6;
	friend class INT6;
	friend class VCE6;
	friend class CMTL;
	friend class DSK6;
	friend class DSK60;
	friend class DSK66;
	friend class SCH6;
	friend class KEY6;
	friend class cWndStat;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	friend class BPoint;
	friend class cWndReg;
	friend class cWndMem;
	friend class cWndMon;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
public:
	enum ReturnCode
	{
		Quit = 0,
		Restart,
		Doko,
		Replay,
		Error,
		
		EndofReturnCode
	};
	
protected:
	// オブジェクトポインタ
	cConfig *cfg;				// 環境設定オブジェクト
	IO6 *io;					// I/O(Z80)
	IO6 *ios;					// I/O(SUB CPU)
	INT6 *intr;					// 割込み
	SCH6 *sche;					// スケジューラ
	SND6 *snd;					// サウンド
	CPU6 *cpum;					// CPU
	SUB6 *cpus;					// SUB CPU
	MEM6 *mem;					// メモリ
	VDG6 *vdg;					// VDG
	DSP6 *graph;				// 画面描画
	PSG6 *psg;					// PSG
	VCE6 *voice;				// 音声合成
	PIO6 *pio;					// 8255
	KEY6 *key;					// キー
	CMTL *cmt;					// CMT
	CMTS *cmts;					// CMT(SAVE)
	DSK6 *disk;					// DISK
	cJoy *joy;					// ジョイスティック
	
	cWndStat *staw;				// ステータスバー
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	BPoint *bp;					// ブレークポイント
	cWndReg *regw;				// レジスタウィンドウ
	cWndMem *memw;				// メモリウィンドウ
	cWndMon *monw;				// モニタウィンドウ
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	SDL_TimerID UpDateFPSID;	// FPS表示タイマID
	
	COLOR24 GPal[256];			// パレット
	
	BOOL ScreenUpdate();								// 画面更新
	int SoundUpdate( int, cRing * = NULL );				// サウンド更新
	static void StreamUpdate( void *, BYTE *, int);		// ストリーム更新 コールバック関数
	static Uint32 UpDateFPS( Uint32, void * );			// FPS表示タイマ コールバック関数
	BOOL SetFPSTimer( int );							// FPS表示タイマ設定
	
	int UDFPSCount;				// FPS表示タイマカウンタ
	int FSkipCount;				// フレームスキップカウンタ
	
	void OnThread( void * );				// スレッド関数
	BOOL IsWaitEnable();					// Wait有効?
	
	BOOL CheckFuncKey( SDL_Event * );		// 各種機能キーチェック
	
	
	// 自動キー入力情報構造体
	typedef struct{
		char *Buffer;			// キーバッファポインタ
		int Num;				// 残り文字数
		int Wait;				// 待ち回数カウンタ
		BOOL Relay;				// リレースイッチOFF待ちフラグ
		int Seek;
	} AKEY;
	AKEY ak;					// 自動キー入力情報
	
	char GetAutoKey();			// 自動キー入力1文字取得
	
	
	
public:
	VM6();									// コンストラクタ
	virtual ~VM6();							// デストラクタ
	
	virtual BOOL Init( const cConfig * ) = 0;	// 初期化
	BOOL InitCommon();						// 共通部分初期化
	
	BOOL Start();							// 動作開始
	void Stop();							// 動作停止
	void Reset();							// リセット
	
	int Emu();								// 1命令実行
	int EmuVSYNC();							// 1画面分実行
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	void Exec( int );						// 指定ステート数実行
	BOOL ToggleMonitor();					// モニタモード切替え
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	ReturnCode EventLoop();					// イベントループ
	
	void ShowPopupMenu( int, int );			// ポップアップメニュー表示
	
	
	BOOL IsAutoKey();						// 自動キー入力実行中?
	BOOL SetAutoKey( char *, int );			// 自動キー入力文字列設定
	BOOL SetAutoKeyFile( char * );			// 自動キー入力文字列設定(ファイルから)
	
	
	// ------------------------------------------
	BOOL DokoDemoSave( char * );	// どこでもSAVE
	BOOL DokoDemoLoad( char * );	// どこでもLOAD
	// ------------------------------------------
};

// PC-6001 仮想マシンクラス
class VM60 : public VM6 {
public:
	VM60();			// コンストラクタ
	~VM60();		// デストラクタ
	
	BOOL Init( const cConfig * );	// 初期化
};

// PC-6001mk2 仮想マシンクラス
class VM62 : public VM6 {
public:
	VM62();			// コンストラクタ
	~VM62();		// デストラクタ
	
	BOOL Init( const cConfig * );	// 初期化
};

// PC-6601 仮想マシンクラス
class VM66 : public VM6 {
public:
	VM66();			// コンストラクタ
	~VM66();		// デストラクタ
	
	BOOL Init( const cConfig * );	// 初期化
};

#endif		// PC60_H_INCLUDED
