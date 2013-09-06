#ifndef P6EL_H_INCLUDED
#define P6EL_H_INCLUDED

#include "typedef.h"
#include "config.h"
#include "joystick.h"
#include "movie.h"
#include "replay.h"
#include "thread.h"
#include "vsurface.h"


#define	FRAMERATE	(VSYNC_HZ/(cfg->GetFrameSkip()+1))


class VM6;
class DSP6;
class SCH6;
class cWndStat;

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
class cWndReg;
class cWndMem;
class cWndMon;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
// エミュレータレイヤークラス
class EL6 : public cThread, public AVI6, public REPLAY {
	
	friend class VM6;
	friend class DSP6;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	friend class cWndMon;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
public:
	enum ReturnCode
	{
		Quit = 0,
		Restart,
		Dokoload,
		Replay,
		Error,
		
		EndofReturnCode
	};
	
protected:
	// オブジェクトポインタ
	VM6 *vm;					// VM
	CFG6 *cfg;					// 環境設定オブジェクト
	SCH6 *sche;					// スケジューラ
	DSP6 *graph;				// 画面描画
	SND6 *snd;					// サウンド
	JOY6 *joy;					// ジョイスティック
	
	cWndStat *staw;				// ステータスバー
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	cWndReg *regw;				// レジスタウィンドウ
	cWndMem *memw;				// メモリウィンドウ
	cWndMon *monw;				// モニタウィンドウ
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //

	
	HTIMERID UpDateFPSID;		// FPS表示タイマID
	
	VPalette GPal;				// パレット
	
    bool isPause;               // ポーズ中フラグ

	bool ScreenUpdate();								// 画面更新
	int SoundUpdate( int, cRing * = NULL );				// サウンド更新
    //#PENDING static void StreamUpdate( void *, BYTE *, int);		// ストリーム更新 コールバック関数

    //#PENDING FPS表示はQtで独自実装
    //    static DWORD UpDateFPS( DWORD, void * );			// FPS表示タイマ コールバック関数
//    bool SetFPSTimer( int );							// FPS表示タイマ設定

	int UDFPSCount;				// FPS表示タイマカウンタ
	int FSkipCount;				// フレームスキップカウンタ
	
	void OnThread( void * );				// スレッド関数
	
	int Emu();								// 1命令実行
	int EmuVSYNC();							// 1画面分実行
	void Wait();							// Wait

	
	bool CheckFuncKey( int, bool, bool );	// 各種機能キーチェック
	
	// 自動キー入力情報構造体
	struct AKEY{
		char *Buffer;			// キーバッファポインタ
		int Num;				// 残り文字数
		int Wait;				// 待ち回数カウンタ
		bool Relay;				// リレースイッチOFF待ちフラグ
		int Seek;
		
		AKEY() : Buffer(NULL), Num(0), Wait(0), Relay(false), Seek(0) {}
	};
	AKEY ak;					// 自動キー入力情報
	
	char GetAutoKey();			// 自動キー入力1文字取得
	
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	void Exec( int );						// 指定ステート数実行
	bool ToggleMonitor();					// モニタモード切替え
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	
public:
	EL6();									// コンストラクタ
    virtual ~EL6();							// デストラクタ
	
	bool Init( const CFG6 * );				// 初期化
	
	bool Start();							// 動作開始
	void Stop();							// 動作停止
	
	
	ReturnCode EventLoop();					// イベントループ
	
	void ShowPopupMenu( int, int );			// ポップアップメニュー表示
	
	bool TapeMount( char * );				// TAPE マウント
	void TapeUnmount();						// TAPE アンマウント
	bool DiskMount( int, char * );			// DISK マウント
	void DiskUnmount( int );				// DISK アンマウント
	
	bool ReplayStartRec( char * );			// リプレイ保存開始
	void ReplayStopRec();					// リプレイ保存停止
	void ReplayStartPlay( char * );			// リプレイ再生開始
	void ReplayStopPlay();					// リプレイ再生停止
	
	bool IsAutoKey();						// 自動キー入力実行中?
	bool SetAutoKey( char *, int );			// 自動キー入力文字列設定
	bool SetAutoKeyFile( char * );			// 自動キー入力文字列設定(ファイルから)
	
	void Notify( int, int );				// 変化を通知する(int)
	void Notify( int, char * );				// 変化を通知する(char *)
	
	bool IsMonitor();						// モニタモード?
	
	// ------------------------------------------
	bool DokoDemoSave( char * );	// どこでもSAVE
	bool DokoDemoLoad( char * );	// どこでもLOAD
	int GetDokoModel( char * );		// どこでもLOADファイルから機種名読込
	// ------------------------------------------
};


#endif		// P6EL_H_INCLUDED
