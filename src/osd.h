/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef OSD_H_INCLUDED
#define OSD_H_INCLUDED

// OS依存の汎用ルーチン(主にUI用)

#include <fstream>
#include <iostream>
#include <string>

#include "event.h"
#include "keydef.h"
#include "typedef.h"
#include "vsurface.h"


// ピクセルフォーマット
enum PixelFMT{ PX32ARGB, PX24RGB, PX24BGR, PX16RGB };

// ファイル選択ダイアログ用
enum FileMode{ FM_Load, FM_Save };
enum FileDlg{ FD_TapeLoad, FD_TapeSave, FD_Disk, FD_ExtRom, FD_Printer, FD_FontZ, FD_FontH,
			  FD_DokoLoad, FD_DokoSave, FD_RepLoad, FD_RepSave, FD_AVISave, FD_LoadAll };

// イベントState
enum EventState{ EVS_QUERY, EVS_DISABLE, EVS_ENABLE };


/////////////////////////////////////////////////////////////////////////////
// プロセス管理関数
/////////////////////////////////////////////////////////////////////////////
// 初期化
bool OSD_Init();
// 初期化Sub(ライブラリ依存処理等)
bool OSD_Init_Sub();
// 終了処理
void OSD_Quit();
// 終了処理Sub(ライブラリ依存処理等)
void OSD_Quit_Sub();
// 多重起動チェック
bool OSD_IsWorking();


/////////////////////////////////////////////////////////////////////////////
// パス名処理関数
/////////////////////////////////////////////////////////////////////////////
// 設定ファイルパス取得
const P6VPATH& OSD_GetConfigPath();
// パスの末尾にデリミタを追加
void OSD_AddDelimiter( P6VPATH& );
// パスの末尾のデリミタを削除
void OSD_DelDelimiter( P6VPATH& );
// 相対パス化
void OSD_RelativePath( P6VPATH& );
// 絶対パス化
void OSD_AbsolutePath( P6VPATH& );
// パス結合
void OSD_AddPath( P6VPATH&, const P6VPATH&, const P6VPATH& );
// パスからフォルダ名を取得
const std::string OSD_GetFolderNamePart( const P6VPATH& );
// パスからファイル名を取得
const std::string OSD_GetFileNamePart( const P6VPATH& );
// パスから拡張子名を取得
const std::string OSD_GetFileNameExt( const P6VPATH& );
// 拡張子名を変更
bool OSD_ChangeFileNameExt( P6VPATH&, const std::string& );




/////////////////////////////////////////////////////////////////////////////
// ファイル操作関数
/////////////////////////////////////////////////////////////////////////////
// ファイルを開く
FILE* OSD_Fopen( const P6VPATH&, const std::string& );
// ファイルストリームを開く
bool OSD_FSopen( std::fstream&, const P6VPATH&, const std::ios_base::openmode );
// フォルダを作成
bool OSD_CreateFolder( const P6VPATH& );
// ファイルの存在チェック
bool OSD_FileExist( const P6VPATH& );
// ファイルサイズ取得
DWORD OSD_GetFileSize( const P6VPATH& );
// ファイルの読取り専用チェック
bool OSD_FileReadOnly( const P6VPATH& );
// ファイル名を変更
bool OSD_FileRename( const P6VPATH&, const P6VPATH& );
// ファイルを削除
bool OSD_FileDelete( const P6VPATH& );
// ファイルを探す
bool OSD_FindFile( const P6VPATH&, const P6VPATH&, std::vector<P6VPATH>&, size_t = 0 );
// フォルダの参照
bool OSD_FolderDiaog( HWINDOW, P6VPATH& );
// 各種ファイル選択
bool OSD_FileSelect( HWINDOW, FileDlg, P6VPATH&, P6VPATH& );




/////////////////////////////////////////////////////////////////////////////
// メッセージ表示関数
/////////////////////////////////////////////////////////////////////////////
// メッセージ表示
int OSD_Message( HWINDOW, const std::string&, const std::string&, int );


/////////////////////////////////////////////////////////////////////////////
// キー入力処理関数
/////////////////////////////////////////////////////////////////////////////
// キーリピート設定
void OSD_SetKeyRepeat( int );


/////////////////////////////////////////////////////////////////////////////
// ジョイスティック処理関数
/////////////////////////////////////////////////////////////////////////////
// 利用可能なジョイスティック数取得
int OSD_GetJoyNum();
// ジョイスティック名取得
const std::string OSD_GetJoyName( int );
// ジョイスティックオープンされてる？
bool OSD_OpenedJoy( HJOYINFO );
// ジョイスティックオープン
HJOYINFO OSD_OpenJoy( int );
// ジョイスティッククローズ
void OSD_CloseJoy( HJOYINFO );
// ジョイスティックの軸の数取得
int OSD_GetJoyNumAxes( HJOYINFO );
// ジョイスティックのボタンの数取得
int OSD_GetJoyNumButtons( HJOYINFO );
// ジョイスティックの軸の状態取得
int OSD_GetJoyAxis( HJOYINFO, int );
// ジョイスティックのボタンの状態取得
bool OSD_GetJoyButton( HJOYINFO, int );


/////////////////////////////////////////////////////////////////////////////
// サウンド関連関数
/////////////////////////////////////////////////////////////////////////////
// オーディオデバイスオープン
bool OSD_OpenAudio( void*, CBF_SND, int, int );
// オーディオデバイスクローズ
void OSD_CloseAudio();
// 再生開始
void OSD_StartAudio();
// 再生停止
void OSD_StopAudio();
// 再生状態取得
bool OSD_AudioPlaying();
// Waveファイル読込み
bool OSD_LoadWAV( const P6VPATH&, BYTE**, DWORD*, int* );
// Waveファイル開放
void OSD_FreeWAV( BYTE* );
// オーディオをロックする
void OSD_LockAudio();
// オーディオをアンロックする
void OSD_UnlockAudio();


/////////////////////////////////////////////////////////////////////////////
// タイマ関連関数
/////////////////////////////////////////////////////////////////////////////
// 指定時間待機
void OSD_Delay( DWORD );
// プロセス開始からの経過時間取得
DWORD OSD_GetTicks();
// タイマ追加
TIMERID OSD_AddTimer( DWORD, CBF_TMR, void * );
// タイマ削除
bool OSD_DelTimer( TIMERID );


/////////////////////////////////////////////////////////////////////////////
// ウィンドウ関連関数
/////////////////////////////////////////////////////////////////////////////
// ウィンドウ作成
bool OSD_CreateWindow( HWINDOW*, const int, const int, const int, const int, const bool, const bool, const int );
// ウィンドウ破棄
void OSD_DestroyWindow( HWINDOW );
// ウィンドウの幅を取得
int OSD_GetWindowWidth( HWINDOW );
// ウィンドウの高さを取得
int OSD_GetWindowHeight( HWINDOW );
// フルスクリーン?
bool OSD_IsFullScreen( HWINDOW );
// フィルタリング有効?
bool OSD_IsFiltering( HWINDOW );
// ウィンドウのサイズ変更可否設定
void OSD_SetWindowResizable( HWINDOW, bool );
// ウィンドウクリア
void OSD_ClearWindow( HWINDOW );
// ウィンドウ反映
void OSD_RenderWindow( HWINDOW );
// ウィンドウに転送(等倍)
void OSD_BlitToWindow( HWINDOW, VSurface*, const int, const int );
// ウィンドウに転送(拡大等)
void OSD_BlitToWindowEx( HWINDOW, VSurface*,  const VRect*, const bool );
// ウィンドウのイメージデータ取得
bool OSD_GetWindowImage( HWINDOW, std::vector<BYTE>&, VRect*, PixelFMT );
// アイコン設定
void OSD_SetIcon( HWINDOW, int );
// キャプション設定
void OSD_SetWindowCaption( HWINDOW, const std::string& );
// マウスカーソル表示/非表示
void OSD_ShowCursor( bool );
// OS依存のウィンドウハンドルを取得
void* OSD_GetWindowHandle( HWINDOW );
// 環境設定ダイアログ表示
int OSD_ConfigDialog( HWINDOW hwnd );
// バージョン情報表示
void OSD_VersionDialog( HWINDOW, int );


/////////////////////////////////////////////////////////////////////////////
// イベント処理関連関数
/////////////////////////////////////////////////////////////////////////////
// イベントキュークリア
void OSD_FlushEvents();
// イベント取得(イベントが発生するまで待つ)
bool OSD_GetEvent( Event* );
// イベントをキューにプッシュする
bool OSD_PushEvent( EventType, ... );
bool OSD_PushEvent(const Event& ev);
// キューに指定のイベントが存在するか調査する
bool OSD_HasEvent( EventType );
// イベント処理の状態を種類ごとに設定する
bool OSD_EventState( EventType, EventState );


/////////////////////////////////////////////////////////////////////////////
// その他の雑関数
/////////////////////////////////////////////////////////////////////////////
// フォントファイル作成
bool OSD_CreateFont( const P6VPATH&, const P6VPATH&, int );
// ShiftJIS -> UTF-8
bool OSD_SJIStoUTF8( std::string& );
// UTF-8 -> ShiftJIS
bool OSD_UTF8toSJIS( std::string& );


// メッセージボックスのタイプ
#define	OSDM_OK				0x000
#define	OSDM_OKCANCEL		0x001
#define	OSDM_YESNO			0x002
#define	OSDM_YESNOCANCEL	0x003

// メッセージボックスのアイコンタイプ
#define	OSDM_ICONERROR		0x010
#define	OSDM_ICONQUESTION	0x020
#define	OSDM_ICONWARNING	0x030
#define	OSDM_ICONINFO		0x040

// メッセージボックスの戻り値
#define	OSDR_OK				0x00
#define	OSDR_CANCEL			0x01
#define	OSDR_YES			0x02
#define	OSDR_NO				0x03


#endif	// OSD_H_INCLUDED
