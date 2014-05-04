#ifndef P6EL_H_INCLUDED
#define P6EL_H_INCLUDED

#include "typedef.h"
#include "movie.h"
#include "replay.h"
#include "thread.h"
#include "vsurface.h"




class VM6;
class CFG6;
class DSP6;
class SND6;
class SCH6;
class JOY6;
class cWndStat;

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class cWndReg;
class cWndMem;
class cWndMon;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
// エミュレータレイヤークラス
class EL6 : public cThread, public AVI6, public REPLAY {
	
	friend class VM6;
	friend class DSP6;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	friend class cWndMon;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
public:
	enum ReturnCode {
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
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	cWndReg *regw;				// レジスタウィンドウ
	cWndMem *memw;				// メモリウィンドウ
	cWndMon *monw;				// モニタウィンドウ
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	
	HTIMERID UpDateFPSID;		// FPS表示タイマID
	VPalette GPal;				// パレット

	int UDFPSCount;				// FPS表示タイマカウンタ
	int FSkipCount;				// フレームスキップカウンタ
	
	
	void DeleteAllObject();					// 全オブジェクト削除
	
	bool ScreenUpdate();					// 画面更新
	int SoundUpdate( int, cRing * = NULL );	// サウンド更新
	//Qtでは使わない
	//static void StreamUpdate( void *, BYTE *, int);		// ストリーム更新 コールバック関数
	//static DWORD UpDateFPS( DWORD, void * );	// FPS表示タイマ コールバック関数
	//bool SetFPSTimer( int );				// FPS表示タイマ設定
	
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
		bool RelayOn;			// リレースイッチON待ちフラグ
		int Seek;
		
        AKEY() : Buffer(NULL), Num(0), Wait(0), Relay(false), RelayOn(false), Seek(0) {}
	};
	AKEY ak;								// 自動キー入力情報
	
	char GetAutoKey();						// 自動キー入力1文字取得
	
	
	// UI関連
	char TapePathUI[PATH_MAX];	// TAPEパス
	char DiskPathUI[PATH_MAX];	// DISKパス
	char ExRomPathUI[PATH_MAX];	// 拡張ROMパス
	
	void UI_TapeInsert( const char * = NULL );		// TAPE 挿入
	void UI_DiskInsert( int, const char * = NULL );	// DISK 挿入
	void UI_RomInsert( const char * = NULL );		// 拡張ROM 挿入
	void UI_RomEject();								// 拡張ROM 排出
	void UI_DokoSave();								// どこでもSAVE
	void UI_DokoLoad( const char * = NULL );		// どこでもLOAD
	void UI_ReplaySave();							// リプレイ保存
    void UI_ReplayResumeSave();						// リプレイ保存再開
    void UI_ReplayDokoLoad();						// リプレイ中どこでもLOAD
    void UI_ReplayDokoSave();						// リプレイ中どこでもSAVE
    void UI_ReplayLoad( const char * = NULL );		// リプレイ再生
	void UI_AVISave();								// ビデオキャプチャ
	void UI_AutoType( const char * = NULL );		// 打込み代行
	void UI_Reset();						// リセット
	void UI_Restart();						// 再起動
	void UI_Quit();							// 終了
	void UI_NoWait();						// Wait変更
	void UI_TurboTape();					// Turbo TAPE変更
	void UI_BoostUp();						// Boost Up変更
	void UI_ScanLine();						// スキャンラインモード変更
	void UI_Disp43();						// 4:3表示変更
	void UI_StatusBar();					// ステータスバー表示状態変更
	void UI_Mode4Color( int );				// MODE4カラー変更
	void UI_FrameSkip( int );				// フレームスキップ変更
	void UI_SampleRate( int );				// サンプリングレート変更
	void UI_Config();						// 環境設定
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void Exec( int );						// 指定ステート数実行
	bool ToggleMonitor();					// モニタモード切替え
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	void ShowPopupMenu( int, int );			// ポップアップメニュー表示
	void ExecMenu( int );					// メニュー選択項目実行
	
	bool TapeMount( const char * );			// TAPE マウント
	void TapeUnmount();						// TAPE アンマウント
	bool DiskMount( int, const char * );	// DISK マウント
	void DiskUnmount( int );				// DISK アンマウント
	
	bool ReplayRecStart( const char * );	// リプレイ保存開始
    bool ReplayRecResume(const char *);     // リプレイ保存再開
    bool ReplayRecDokoLoad();               // リプレイ中どこでもLOAD
    bool ReplayRecDokoSave();               // リプレイ中どこでもSAVE
    void ReplayRecStop();					// リプレイ保存停止
	void ReplayPlayStart( const char * );	// リプレイ再生開始
	void ReplayPlayStop();					// リプレイ再生停止
	
	bool IsAutoKey();						// 自動キー入力実行中?
	bool SetAutoKeyFile( const char * );	// 自動キー入力文字列設定(ファイルから)
	bool SetAutoKey( const char *, int );	// 自動キー入力文字列設定
	void SetAutoStart();					// オートスタート文字列設定
	
public:
	EL6();									// コンストラクタ
    virtual ~EL6();							// デストラクタ
	
	bool Init( const CFG6 * );				// 初期化
	
	bool Start();							// 動作開始
	void Stop();							// 動作停止
	
	ReturnCode EventLoop();					// イベントループ
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	bool IsMonitor() const;					// モニタモード?
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	

	// ------------------------------------------
	bool DokoDemoSave( const char * );	// どこでもSAVE
	bool DokoDemoLoad( const char * );	// どこでもLOAD
	int GetDokoModel( const char * );	// どこでもLOADファイルから機種名読込
	// ------------------------------------------
};


#endif		// P6EL_H_INCLUDED
