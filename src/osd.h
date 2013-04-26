#ifndef OSD_H_INCLUDED
#define OSD_H_INCLUDED

// OS依存の汎用ルーチン(主にUI用)

#include "event.h"
#include "typedef.h"
#include "keydef.h"
#include "vsurface.h"



// ファイル選択ダイアログ用
enum FileMode{ FM_Load, FM_Save };
enum FileDlg{ FD_TapeLoad, FD_TapeSave, FD_Disk, FD_ExtRom, FD_Printer, FD_FontZ, FD_FontH,
			  FD_DokoLoad, FD_DokoSave, FD_RepLoad, FD_RepSave, FD_AVISave, FD_LoadAll, EndofFileDlg };

// --- 文字列操作関数 ---
// 小文字による文字列比較
//   stricmp 関数は not ANSI,not POSIXでした。MSの独自拡張かな?
#ifndef stricmp
int stricmp ( const char *, const char * );
#endif

// --- パス名処理関数 ---
// パスのデリミタを'/'に変換
void Delimiter( char * );
// パスのデリミタをOSデフォルトに変換(Windowsなら'\')
void UnDelimiter( char * );
// パスの末尾にデリミタを追加
void AddDelimiter( char * );
// パスの末尾のデリミタを削除
void DelDelimiter( char * );

// --- プロセス管理関数 ---
// 初期化
bool OSD_Init();
// 終了処理
void OSD_Quit();

// --- ファイル操作関数 ---
// 設定ファイルパス作成
bool OSD_CreateConfigPath();
// 設定ファイルパス取得
const char *OSD_GetConfigPath();
// ファイルの存在チェック
bool OSD_FileExist( const char * );
// ファイルの読取り専用チェック
bool OSD_FileReadOnly( const char * );
// フルパスからファイル名を取得
const char *OSD_GetFileNamePart( const char * );
// フォルダの参照
const char *OSD_FolderDiaog( void *, char * );
// ファイルの参照
const char *OSD_FileDiaog( void *, FileMode, const char *, const char *, char *, char *, const char * );
// 各種ファイル選択
const char *OSD_FileSelect( void *, FileDlg, char *, char * );

// --- メッセージ表示関数 ---
// メッセージ表示
int OSD_Message( const char *, const char *, int );

// --- キー入力処理関数 ---
// キーリピート設定
void OSD_SetKeyRepeat( int );
// OSDキーコード -> 仮想キーコード変換
PCKEYsym OSD_ConvertKeyCode( int );

// --- ジョイスティック処理関数 ---
// 利用可能なジョイスティック数取得
int OSD_GetJoyNum();
// ジョイスティック名取得
const char *OSD_GetJoyName( int );
// ジョイスティックオープンされてる？
bool OSD_OpenedJoy( int );
// ジョイスティックオープン
HJOYINFO OSD_OpenJoy( int );
// ジョイスティッククローズ
void OSD_CloseJoy( HJOYINFO );
// ジョイスティックの軸の数取得
int OSD_GetJoyNumAxes( HJOYINFO );
// ジョイスティックのボタンの数取得
int OSD_GetJoyNumButtons( HJOYINFO );
// ジョイスティックの状態を更新
void OSD_UpdateJoy();
// ジョイスティックの軸の状態取得
int OSD_GetJoyAxis( HJOYINFO, int );
// ジョイスティックのボタンの状態取得
bool OSD_GetJoyButton( HJOYINFO, int );

// --- サウンド関連関数 ---
// オーディオデバイスオープン
bool OSD_OpenAudio( void *, CBF_SND, int, int );
// オーディオデバイスクローズ
void OSD_CloseAudio();
// 再生開始
void OSD_StartAudio();
// 再生停止
void OSD_StopAudio();
// オーディオストリーム書き込み
void OSD_WriteAudioStream(BYTE *stream, int samples);
// 再生状態取得
bool OSD_AudioPlaying();
// Waveファイル読込み
bool OSD_LoadWAV( const char *, BYTE **, DWORD *, int * );
// Waveファイル開放
void OSD_FreeWAV( BYTE * );
// オーディオをロックする
void OSD_LockAudio();
// オーディオをアンロックする
void OSD_UnlockAudio();

// --- タイマ関連関数 ---
// 指定時間待機
void OSD_Delay( DWORD );
// プロセス開始からの経過時間取得
DWORD OSD_GetTicks();

// --- ウィンドウ関連関数 ---
// キャプション設定
void OSD_SetWindowCaption( HWINDOW, const char * );
// マウスカーソル表示/非表示
void OSD_ShowCursor( bool );
// ウィンドウ作成
bool OSD_CreateWindow( HWINDOW *, int, int, int, bool );
// ウィンドウ破棄
void OSD_DestroyWindow( HWINDOW );
// ウィンドウの幅を取得
int OSD_GetWindowWidth( HWINDOW );
// ウィンドウの高さを取得
int OSD_GetWindowHeight( HWINDOW );
// ウィンドウの色深度を取得
int OSD_GetWindowBPP( HWINDOW );
// パレット設定
bool OSD_SetPalette( HWINDOW, VPalette * );
// ウィンドウクリア
void OSD_ClearWindow( HWINDOW );
// ウィンドウ反映
void OSD_RenderWindow( HWINDOW );
// ウィンドウに転送
void OSD_BlitToWindow( HWINDOW, VSurface *, int, int, VPalette * );
// ウィンドウに転送(2倍)
void OSD_BlitToWindow2( HWINDOW, VSurface *, int, int );
// アイコン設定
void OSD_SetIcon( HWINDOW, int );

// --- イベント処理関連関数 ---
// イベント取得(イベントが発生するまで待つ)
bool OSD_GetEvent( Event * );
// イベントをキューにプッシュする
bool OSD_PushEvent( EventType );
bool OSD_PushEvent( const Event& ev );


// --- その他の雑関数 ---
// 色の名前取得
const char *OSD_ColorName( int );
// キーの名前取得
const char *OSD_KeyName( PCKEYsym );
// フォントファイル作成
bool OSD_CreateFont( char *, char *, int );


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


// --- メッセージ配列 ---
// 一般メッセージ
extern const char *MsgStr[];
#define	MSG_QUIT			MsgStr[0]	// "終了してよろしいですか?"
#define	MSG_QUITC			MsgStr[1]	// "終了確認"
#define	MSG_RESTART0		MsgStr[2]	// "再起動してよろしいですか?"
#define	MSG_RESTART			MsgStr[3]	// "変更は再起動後に有効となります。\n今すぐ再起動しますか?"
#define	MSG_RESTARTC		MsgStr[4]	// "再起動確認"
#define	MSG_RESETI			MsgStr[5]	// "拡張ROMを挿入してリセットします。"
#define	MSG_RESETE			MsgStr[6]	// "拡張ROMを排出してリセットします。"
#define	MSG_RESETC			MsgStr[7]	// "リセット確認"


// メニュー用メッセージ ------
extern const char *MsgMen[];
// [システム]
#define	MSMEN_AVI0			MsgMen[0]	// "ビデオキャプチャ..."
#define	MSMEN_AVI1			MsgMen[1]	// "ビデオキャプチャ停止"
#define	MSMEN_REP0			MsgMen[2]	// "記録..."  (リプレイ)
#define	MSMEN_REP1			MsgMen[3]	// "記録停止" (リプレイ)
#define	MSMEN_REP2			MsgMen[4]	// "再生..."  (リプレイ)
#define	MSMEN_REP3			MsgMen[5]	// "再生停止" (リプレイ)


// INIファイル用メッセージ ------
extern const char *MsgIni[];
// [CONFIG]
#define	MSINI_TITLE			MsgIni[0]	// "; === PC6001V 初期設定ファイル ===\n\n"
#define	MSINI_Model			MsgIni[1]	// " 機種 60:PC-6001 62:PC-6001mk2 66:PC-6601 64:PC-6001mk2SR 68:PC-6601SR"
#define	MSINI_FDD			MsgIni[2]	// " FDD接続台数 (0-2)"
#define	MSINI_ExtRam		MsgIni[3]	// " 拡張RAM使用"
#define	MSINI_TurboTAPE		MsgIni[4]	// " Turbo TAPE Yes:有効 No:無効"
#define	MSINI_BoostUp		MsgIni[5]	// " BoostUp Yes:有効 No:無効"
#define	MSINI_MaxBoost60	MsgIni[6]	// " BoostUp 最大倍率(N60モード)
#define	MSINI_MaxBoost62	MsgIni[7]	// " BoostUp 最大倍率(N60m/N66モード)
#define	MSINI_OverClock		MsgIni[8]	// " オーバークロック率 (1-1000)%"
#define	MSINI_CheckCRC		MsgIni[9]	// " CRCチェック Yes:有効 No:無効"
#define	MSINI_RomPatch		MsgIni[10]	// " ROMパッチ Yes:あてる No:あてない"
// [DISPLAY]
#define	MSINI_ScrBpp		MsgIni[11]	// " カラーモード 8:8bit 16:16bit 24:24bit"
#define	MSINI_Mode4Color	MsgIni[12]	// " MODE4カラーモード 0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク"
#define	MSINI_ScanLine		MsgIni[13]	// " スキャンライン Yes:あり No:なし"
#define	MSINI_ScanLineBr	MsgIni[14]	// " スキャンライン輝度 (0-100)%"
#define	MSINI_DispNTSC		MsgIni[15]	// " 4:3表示 Yes:有効 No:無効"
#define	MSINI_FullScreen	MsgIni[16]	// " フルスクリーンモード Yes:有効 No:無効"
#define	MSINI_DispStatus	MsgIni[17]	// " ステータスバー Yes:表示 No:非表示"
#define	MSINI_FrameSkip		MsgIni[18]	// " フレームスキップ"
// [SOUND]
#define	MSINI_SampleRate	MsgIni[19]	// " サンプリングレート (44100/22050/11025)Hz"
#define	MSINI_SoundBuffer	MsgIni[20]	// " サウンドバッファサイズ"
#define	MSINI_MasterVolume	MsgIni[21]	// " マスター音量 (0-100)"
#define	MSINI_PsgVolume		MsgIni[22]	// " PSG音量 (0-100)"
#define	MSINI_VoiceVolume	MsgIni[23]	// " 音声合成音量 (0-100)"
#define	MSINI_TapeVolume	MsgIni[24]	// " TAPEモニタ音量 (0-100)"
#define	MSINI_PsgLPF		MsgIni[25]	// " PSG LPFカットオフ周波数(0で無効)"
// [MOVIE]
#define	MSINI_AviRle		MsgIni[26]	// " RLEフラグ Yes:RLE有効 No:ベタ"
// [FILES]
#define	MSINI_ExtRom		MsgIni[27]	// " 拡張ROMファイル名"
#define	MSINI_tape			MsgIni[28]	// " TAPE(LODE)ファイル名(起動時に自動マウント)"
#define	MSINI_save			MsgIni[29]	// " TAPE(SAVE)ファイル名(SAVE時に自動マウント)"
#define	MSINI_disk1			MsgIni[30]	// " DISK1ファイル名(起動時に自動マウント)"
#define	MSINI_disk2			MsgIni[31]	// " DISK2ファイル名(起動時に自動マウント)"
#define	MSINI_printer		MsgIni[32]	// " プリンタ出力ファイル名"
#define	MSINI_fontz			MsgIni[33]	// " 全角フォントファイル名"
#define	MSINI_fonth			MsgIni[34]	// " 半角フォントファイル名"
// [PATH]
#define	MSINI_RomPath		MsgIni[35]	// " ROMイメージ格納パス"
#define	MSINI_TapePath		MsgIni[36]	// " TAPEイメージ格納パス"
#define	MSINI_DiskPath		MsgIni[37]	// " DISKイメージ格納パス"
#define	MSINI_ExtRomPath	MsgIni[38]	// " 拡張ROMイメージ格納パス"
#define	MSINI_ImgPath		MsgIni[39]	// " スナップショット格納パス"
#define	MSINI_WavePath		MsgIni[40]	// " WAVEファイル格納パス"
#define	MSINI_FontPath		MsgIni[41]	// " FONT格納パス"
// [CHECK]
#define	MSINI_CkQuit		MsgIni[42]	// " 終了時確認 Yes:する No:しない"
#define	MSINI_SaveQuit		MsgIni[43]	// " 終了時INIファイルを保存 Yes:する No:しない"
// [KEY]
#define	MSINI_KeyRepeat		MsgIni[44]	// " キーリピートの間隔(単位:ms 0で無効)"
#define	MSINI_UsePS2KBD		MsgIni[45]	// " PS/2キーボード使う? Yes:PS/2 No:USB"
// [OPTION]
#define	MSINI_UseSoldier	MsgIni[46]	// " 戦士のカートリッジ Yes:有効 No:無効"


// どこでもSAVE用メッセージ ------
extern const char *MsgDok[];
#define	MSDOKO_TITLE		MsgDok[0]	// "; === PC6001V どこでもSAVEファイル ===\n\n"


// Error用メッセージ ------
extern const char *MsgErr[];
#define	MSERR_ERROR				MsgErr[0]	// "Error"
#define	MSERR_NoError			MsgErr[1]	// "エラーはありません"
#define	MSERR_Unknown			MsgErr[2]	// "原因不明のエラーが発生しました"
#define	MSERR_MemAllocFailed	MsgErr[3]	// "メモリの確保に失敗しました"
#define	MSERR_RomChange			MsgErr[4]	// "指定された機種のROMイメージが見つからないため機種を変更しました\n設定を確認してください"
#define	MSERR_NoRom				MsgErr[5]	// "ROMイメージが見つかりません\n設定とファイル名を確認してください"
#define	MSERR_RomSizeNG			MsgErr[6]	// "ROMイメージのサイズが不正です"
#define	MSERR_RomCrcNG			MsgErr[7]	// "ROMイメージのCRCが不正です"
#define	MSERR_LibInitFailed		MsgErr[8]	// "ライブラリの初期化に失敗しました"
#define	MSERR_InitFailed		MsgErr[9]	// "初期化に失敗しました\n設定を確認してください"
#define	MSERR_FontLoadFailed	MsgErr[10]	// "フォントの読込みに失敗しました"
#define	MSERR_FontCreateFailed	MsgErr[11]	// "フォントファイルの作成に失敗しました"
#define	MSERR_IniDefault		MsgErr[12]	// "INIファイルの読込みに失敗しました\nデフォルト設定で起動します"
#define	MSERR_IniReadFailed		MsgErr[13]	// "INIファイルの読込みに失敗しました"
#define	MSERR_IniWriteFailed	MsgErr[14]	// "INIファイルの保存に失敗しました"
#define	MSERR_TapeMountFailed	MsgErr[15]	// "TAPEイメージのマウントに失敗しました"
#define	MSERR_DiskMountFailed	MsgErr[16]	// "DISKイメージのマウントに失敗しました"
#define	MSERR_ExtRomMountFailed	MsgErr[17]	// "拡張ROMイメージのマウントに失敗しました"
#define	MSERR_DokoReadFailed	MsgErr[18]	// "どこでもLOADに失敗しました"
#define	MSERR_DokoWriteFailed	MsgErr[19]	// "どこでもSAVEに失敗しました"
#define	MSERR_DokoDiffVersion	MsgErr[20]	// "どこでもLOADに失敗しました\n保存時とPC6001Vのバージョンが異なります"
#define	MSERR_ReplayPlayError	MsgErr[21]	// "リプレイ再生に失敗しました"
#define	MSERR_ReplayRecError	MsgErr[22]	// "リプレイ記録に失敗しました"
#define	MSERR_NoReplayData		MsgErr[23]	// "リプレイデータがありません"





#endif	// OSD_H_INCLUDED
