/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef P6EL_H_INCLUDED
#define P6EL_H_INCLUDED

#include <memory>
#include <string>

#include "typedef.h"
#include "movie.h"
#include "replay.h"
#include "thread.h"
#include "vdg.h"
#include "vsurface.h"


class VM6;
class CFG6;
class DSP6;
class SND6;
class SCH6;
class JOY6;
class cWndStat;

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class cWndReg;
class cWndMem;
class cWndMon;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
// エミュレータレイヤークラス
class EL6 : public cThread, public AVI6, public REPLAY {
	
	friend class DSP6;
	friend class cWndStat;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	friend class cWndReg;
	friend class cWndMem;
	friend class cWndMon;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
public:
	enum ReturnCode{
		Quit = 0,
		Restart,
		Dokoload,
		ReplayResume,
		ReplayPlay,
		ReplayMovie,
		Error
	};
	
protected:
	// オブジェクトポインタ
	std::shared_ptr<CFG6> cfg;		// 環境設定オブジェクト
	std::shared_ptr<VM6>  vm;		// VM
	
	std::unique_ptr<SCH6> sche;		// スケジューラ
	std::unique_ptr<SND6> snd;		// サウンド
	std::unique_ptr<JOY6> joy;		// ジョイスティック
	std::unique_ptr<DSP6> graph;	// 画面描画
	
	std::unique_ptr<cWndStat> staw;	// ステータスバー
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	std::unique_ptr<cWndReg> regw;	// レジスタウィンドウ
	std::unique_ptr<cWndMem> memw;	// メモリウィンドウ
	std::unique_ptr<cWndMon> monw;	// モニタウィンドウ
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	TIMERID UpDateFPSID;			// FPS表示タイマID
	int FSkipCount;					// フレームスキップカウンタ
	bool MMotion;					// マウス動いたフラグ
	
	static int Speed;				// 停止時の速度退避用
	
	bool ScreenUpdate();								// 画面更新
	int SoundUpdate( int, cRing* = nullptr );			// サウンド更新
	#ifndef NOCALLBACK	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	static void StreamUpdate( void*, BYTE*, int );		// ストリーム更新 コールバック関数
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	static DWORD UpDateFPS( DWORD, void* );				// FPS表示タイマ コールバック関数
	bool StartFPSTimer();								// FPS表示タイマ開始
	void StopFPSTimer();								// FPS表示タイマ停止
	
	void OnThread( void* ) override;					// スレッド関数
	
	int Emu();											// 1命令実行
	int EmuVSYNC();										// 1画面分実行
	void Wait();										// Wait
	
	bool CheckFuncKey( int, bool );						// 各種機能キーチェック
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void CheckMonKey( int, int, bool );					// モニタモードキーチェック
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// 自動キー入力情報構造体
	struct AKEY{
		std::string Buffer;		// キーバッファ
		int Wait;				// 待ち回数カウンタ
		bool Relay;				// リレースイッチOFF待ちフラグ
		bool RelayOn;			// リレースイッチON待ちフラグ
		
		AKEY() : Buffer(""), Wait(0), Relay(false), RelayOn(false) {}
	};
	AKEY ak;					// 自動キー入力情報
	
	WORD GetAutoKey();									// 自動キー入力1文字取得
	
	
	// UI関連
	P6VPATH TapePathUI;		// TAPEパス
	P6VPATH DiskPathUI;		// DISKパス
	P6VPATH ExRomPathUI;	// 拡張ROMパス
	P6VPATH DokoPathUI;		// どこでもSAVEパス
	
	void UI_TapeInsert( const P6VPATH& = "" );						// UI:TAPE 挿入
	void UI_DiskInsert( int, const P6VPATH& = "" );					// UI:DISK 挿入
	void UI_CartInsert( WORD, const P6VPATH& = "" );				// UI:拡張カートリッジ 挿入
	void UI_CartInsertNoRom( WORD );								// UI:拡張カートリッジ 挿入(ROMなし)
	void UI_CartEject();											// UI:拡張カートリッジ 排出
	bool UI_CheckDokoVer( const P6VPATH& );							// UI:どこでもSAVEファイルのバージョンチェック
	void UI_DokoSave( const P6VPATH& = "" );						// UI:どこでもSAVE
	void UI_DokoLoad( const P6VPATH& = "" );						// UI:どこでもLOAD
	void UI_DokoSave( int );										// UI:どこでもSAVE(スロット使用)
	void UI_DokoLoad( int, bool = false );							// UI:どこでもLOAD(スロット使用)
	void UI_ReplaySave( const P6VPATH& = "" );						// UI:リプレイ保存/停止
	void UI_ReplayResumeSave( const P6VPATH& = "" );				// UI:リプレイ保存再開
	void UI_ReplayDokoSave();										// UI:リプレイ中どこでもSAVE
	void UI_ReplayDokoLoad();										// UI:リプレイ中どこでもLOAD
	void UI_ReplayRollback();										// UI:リプレイ中どこでもLOADを巻き戻す
	void UI_ReplayPlay( const P6VPATH& = "" );						// UI:リプレイ再生/停止
	void UI_ReplayMovie();											// UI:リプレイを動画に変換
	void UI_AVISaveStart();											// UI:ビデオキャプチャ開始
	void UI_AVISaveStop();											// UI:ビデオキャプチャ停止
	void UI_SnapShot();												// UI:スナップショット
	void UI_AutoType( const P6VPATH& = "" );						// UI:打込み代行
	void UI_Reset();												// UI:リセット
	void UI_Restart();												// UI:再起動
	void UI_Quit();													// UI:終了
	void UI_Pause();												// UI:Pause変更
	void UI_NoWait();												// UI:Wait変更
	void UI_TurboTape();											// UI:Turbo TAPE変更
	void UI_BoostUp();												// UI:Boost Up変更
	void UI_FullScreen();											// UI:フルスクリーン変更
	void UI_WindowZoom( int );										// UI:ウィンドウ表示倍率変更
	void UI_StatusBar();											// UI:ステータスバー表示状態変更
	void UI_Disp43();												// UI:4:3表示変更
	void UI_ScanLine();												// UI:スキャンラインモード変更
	void UI_Filtering();											// UI:フィルタリング変更
	void UI_Mode4Color( int );										// UI:MODE4カラー変更
	void UI_FrameSkip( int );										// UI:フレームスキップ変更
	void UI_SampleRate( int );										// UI:サンプリングレート変更
	void UI_Romaji();												// UI:ローマ字入力切換
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void UI_Monitor();												// UI:モニタモード切替え
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void UI_Config();												// UI:環境設定
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void Exec( int );							// 指定ステート数実行
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	
	void ShowPopupMenu( int, int );							// ポップアップメニュー表示
	void ExecMenu( int );									// メニュー選択項目実行
	
	bool TapeMount( const P6VPATH& );						// TAPE マウント
	void TapeUnmount();										// TAPE アンマウント
	bool DiskMount( int, const P6VPATH& );					// DISK マウント
	void DiskUnmount( int );								// DISK アンマウント
	
	bool ReplayRecStart( const P6VPATH& );					// リプレイ保存開始
	bool ReplayRecResume( const P6VPATH& );					// リプレイ保存再開
	
	bool IsAutoKey();										// 自動キー入力実行中?
	bool SetAutoKeyFile( const P6VPATH& );					// 自動キー入力文字列設定(ファイルから)
	bool SetAutoKey( const std::string&, int = 60 );		// 自動キー入力文字列設定
	void SetAutoStart();									// オートスタート文字列設定
	
	void SetPalette();										// パレット設定
	
	std::shared_ptr<VSurface> GetBackBuffer();				// バックバッファ取得
	const VDGInfo& GetVideoInfo() const;					// 画面情報取得
	
public:
	EL6();
	~EL6();
	
	bool Init( const std::shared_ptr<CFG6>& );	// 初期化
	
	bool Start();								// 動作開始
	void Stop();								// 動作停止
	
	ReturnCode EventLoop( ReturnCode );			// イベントループ
	
	HWINDOW GetWindowHandle();					// ウィンドウハンドル取得
	
	// ----------------------------------------------------------------------
	bool DokoDemoSave( const P6VPATH& );		// どこでもSAVE
	bool DokoDemoLoad( const P6VPATH& );		// どこでもLOAD
	bool CheckDokoVer( const P6VPATH& );		// どこでもSAVEファイルのバージョンチェック
	// ----------------------------------------------------------------------
};


#endif		// P6EL_H_INCLUDED
