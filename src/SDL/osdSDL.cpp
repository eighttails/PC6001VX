// SDL依存の汎用ルーチン
#include <string.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_endian.h>

#include "../log.h"
#include "../osd.h"
#include "../common.h"
#include "../vsurface.h"


///////////////////////////////////////////////////////////
// SDL関連
///////////////////////////////////////////////////////////
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define	AUDIOFORMAT	AUDIO_S16MSB	// 16ビット符号あり
#else
#define	AUDIOFORMAT	AUDIO_S16		// 16ビット符号あり
#endif

// SDLサーフェス用オプション
#define SDLOP_SCREEN	(SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_HWACCEL)

// ユーザー定義イベント
#define UEV_FPSUPDATE	(SDL_USEREVENT+0)
#define UEV_RESTART		(SDL_USEREVENT+1)
#define UEV_DOKOLOAD	(SDL_USEREVENT+2)
#define UEV_REPLAY		(SDL_USEREVENT+3)
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
#define UEV_DEBUGMODEBP	(SDL_USEREVENT+4)
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //

#define USESDLTIMER					// SDLのタイマ使用


///////////////////////////////////////////////////////////
// ローカル関数定義
///////////////////////////////////////////////////////////
// OSD_FolderDiaog()で使用するコールバックプロシージャ
//static int CALLBACK OsdBrowseCallbackProc( HWND, UINT, LPARAM, LPARAM );
#ifndef USESDLTIMER
// OSD_AddTimer()で使用するコールバックプロシージャ
static void CALLBACK OsdTimerCallbackProc( UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR );
#endif

///////////////////////////////////////////////////////////
// スタティック変数
///////////////////////////////////////////////////////////
//static HANDLE hMutex;						// 多重起動チェック用のミューテックス
//static BROWSEINFO OBI;						// OSD_FolderDiaog()で使用する情報を格納する構造体

#if SDL_VERSION_ATLEAST(2,0,0)
static PCKEYsym VKTable[SDL_NUM_SCANCODES];	// SDLスキャンコード  -> 仮想キーコード 変換テーブル
#else
static PCKEYsym VKTable[SDLK_LAST];			// SDLキーコード  -> 仮想キーコード 変換テーブル
#endif



#ifndef USESDLTIMER
static TINFO *tif;
#endif
///////////////////////////////////////////////////////////
// 定数
///////////////////////////////////////////////////////////
#if SDL_VERSION_ATLEAST(2,0,0)
static const struct {	// SDLスキャンコード -> 仮想キーコード定義
	SDL_Scancode InKey;	// SDLのスキャンコード
	PCKEYsym VKey;		// 仮想キーコード
} VKeyDef[] = {
	{ SDL_SCANCODE_UNKNOWN,			KVC_UNKNOWN },
	
	{ SDL_SCANCODE_1,				KVC_1 },			// 1	!
	{ SDL_SCANCODE_2,				KVC_2 },			// 2	"
	{ SDL_SCANCODE_3,				KVC_3 },			// 3	#
	{ SDL_SCANCODE_4,				KVC_4 },			// 4	$
	{ SDL_SCANCODE_5,				KVC_5 },			// 5	%
	{ SDL_SCANCODE_6,				KVC_6 },			// 6	&
	{ SDL_SCANCODE_7,				KVC_7 },			// 7	'
	{ SDL_SCANCODE_8,				KVC_8 },			// 8	(
	{ SDL_SCANCODE_9,				KVC_9 },			// 9	)
	{ SDL_SCANCODE_0,				KVC_0 },			// 0
	
	{ SDL_SCANCODE_A,				KVC_A },			// a	A
	{ SDL_SCANCODE_B,				KVC_B },			// b	B
	{ SDL_SCANCODE_C,				KVC_C },			// c	C
	{ SDL_SCANCODE_D,				KVC_D },			// d	D
	{ SDL_SCANCODE_E,				KVC_E },			// e	E
	{ SDL_SCANCODE_F,				KVC_F },			// f	F
	{ SDL_SCANCODE_G,				KVC_G },			// g	G
	{ SDL_SCANCODE_H,				KVC_H },			// h	H
	{ SDL_SCANCODE_I,				KVC_I },			// i	I
	{ SDL_SCANCODE_J,				KVC_J },			// j	J
	{ SDL_SCANCODE_K,				KVC_K },			// k	K
	{ SDL_SCANCODE_L,				KVC_L },			// l	L
	{ SDL_SCANCODE_M,				KVC_M },			// m	M
	{ SDL_SCANCODE_N,				KVC_N },			// n	N
	{ SDL_SCANCODE_O,				KVC_O },			// o	O
	{ SDL_SCANCODE_P,				KVC_P },			// p	P
	{ SDL_SCANCODE_Q,				KVC_Q },			// q	Q
	{ SDL_SCANCODE_R,				KVC_R },			// r	R
	{ SDL_SCANCODE_S,				KVC_S },			// s	S
	{ SDL_SCANCODE_T,				KVC_T },			// t	T
	{ SDL_SCANCODE_U,				KVC_U },			// u	U
	{ SDL_SCANCODE_V,				KVC_V },			// v	V
	{ SDL_SCANCODE_W,				KVC_W },			// w	W
	{ SDL_SCANCODE_X,				KVC_X },			// x	X
	{ SDL_SCANCODE_Y,				KVC_Y },			// y	Y
	{ SDL_SCANCODE_Z,				KVC_Z },			// z	Z
	
	{ SDL_SCANCODE_F1,				KVC_F1 },			// F1
	{ SDL_SCANCODE_F2,				KVC_F2 },			// F2
	{ SDL_SCANCODE_F3,				KVC_F3 },			// F3
	{ SDL_SCANCODE_F4,				KVC_F4 },			// F4
	{ SDL_SCANCODE_F5,				KVC_F5 },			// F5
	{ SDL_SCANCODE_F6,				KVC_F6 },			// F6
	{ SDL_SCANCODE_F7,				KVC_F7 },			// F7
	{ SDL_SCANCODE_F8,				KVC_F8 },			// F8
	{ SDL_SCANCODE_F9,				KVC_F9 },			// F9
	{ SDL_SCANCODE_F10,				KVC_F10 },			// F10
	{ SDL_SCANCODE_F11,				KVC_F11 },			// F11
	{ SDL_SCANCODE_F12,				KVC_F12 },			// F12
	
	{ SDL_SCANCODE_MINUS,			KVC_MINUS },		// -	=
	{ SDL_SCANCODE_APOSTROPHE,		KVC_CARET },		// ^	~
	{ SDL_SCANCODE_BACKSPACE,		KVC_BACKSPACE },	// BackSpace
	{ SDL_SCANCODE_GRAVE,			KVC_AT },			// @	`
	{ SDL_SCANCODE_LEFTBRACKET,		KVC_LBRACKET },		// [	{
	{ SDL_SCANCODE_EQUALS,			KVC_SEMICOLON },	// ;	+
	{ SDL_SCANCODE_SEMICOLON,		KVC_COLON },		// :	*
	{ SDL_SCANCODE_COMMA,			KVC_COMMA },		// ,	<
	{ SDL_SCANCODE_PERIOD,			KVC_PERIOD },		// .	>
	{ SDL_SCANCODE_SLASH,			KVC_SLASH },		// /	?
	{ SDL_SCANCODE_SPACE,			KVC_SPACE },		// Space
	
	{ SDL_SCANCODE_ESCAPE,			KVC_ESC },			// ESC
	{ SDL_SCANCODE_TAB,				KVC_TAB },			// Tab
	{ SDL_SCANCODE_CAPSLOCK,		KVC_CAPSLOCK },		// CapsLock
	{ SDL_SCANCODE_RETURN,			KVC_ENTER },		// Enter
	{ SDL_SCANCODE_LCTRL,			KVC_LCTRL },		// L-Ctrl
	{ SDL_SCANCODE_RCTRL,			KVC_RCTRL },		// R-Ctrl
	{ SDL_SCANCODE_LSHIFT,			KVC_LSHIFT },		// L-Shift
	{ SDL_SCANCODE_RSHIFT,			KVC_RSHIFT },		// R-Shift
	{ SDL_SCANCODE_LALT,			KVC_LALT },			// L-Alt
	{ SDL_SCANCODE_RALT,			KVC_RALT },			// R-Alt
	{ SDL_SCANCODE_PRINTSCREEN,		KVC_PRINT },		// PrintScreen
	{ SDL_SCANCODE_PRINTSCREEN,		KVC_SCROLLLOCK },	// ScrollLock
	{ SDL_SCANCODE_PAUSE,			KVC_PAUSE },		// Pause
	{ SDL_SCANCODE_INSERT,			KVC_INSERT },		// Insert
	{ SDL_SCANCODE_DELETE,			KVC_DELETE },		// Delete
	{ SDL_SCANCODE_END,				KVC_END },			// End
	{ SDL_SCANCODE_HOME,			KVC_HOME },			// Home
	{ SDL_SCANCODE_PAGEUP,			KVC_PAGEUP },		// PageUp
	{ SDL_SCANCODE_PAGEDOWN,		KVC_PAGEDOWN },		// PageDown
	
	{ SDL_SCANCODE_UP,				KVC_UP },			// ↑
	{ SDL_SCANCODE_DOWN,			KVC_DOWN },			// ↓
	{ SDL_SCANCODE_LEFT,			KVC_LEFT },			// ←
	{ SDL_SCANCODE_RIGHT,			KVC_RIGHT },		// →
	
	{ SDL_SCANCODE_KP_0,			KVC_P0 },			// [0]
	{ SDL_SCANCODE_KP_1,			KVC_P1 },			// [1]
	{ SDL_SCANCODE_KP_2,			KVC_P2 },			// [2]
	{ SDL_SCANCODE_KP_3,			KVC_P3 },			// [3]
	{ SDL_SCANCODE_KP_4,			KVC_P4 },			// [4]
	{ SDL_SCANCODE_KP_5,			KVC_P5 },			// [5]
	{ SDL_SCANCODE_KP_6,			KVC_P6 },			// [6]
	{ SDL_SCANCODE_KP_7,			KVC_P7 },			// [7]
	{ SDL_SCANCODE_KP_8,			KVC_P8 },			// [8]
	{ SDL_SCANCODE_KP_9,			KVC_P9 },			// [9]
	{ SDL_SCANCODE_NUMLOCKCLEAR,	KVC_NUMLOCK },		// NumLock
	{ SDL_SCANCODE_KP_PLUS,			KVC_P_PLUS },		// [+]
	{ SDL_SCANCODE_KP_MINUS,		KVC_P_MINUS },		// [-]
	{ SDL_SCANCODE_KP_MULTIPLY,		KVC_P_MULTIPLY },	// [*]
	{ SDL_SCANCODE_KP_DIVIDE,		KVC_P_DIVIDE },		// [/]
	{ SDL_SCANCODE_KP_PERIOD,		KVC_P_PERIOD },		// [.]
	{ SDL_SCANCODE_KP_ENTER,		KVC_P_ENTER },		// [Enter]
	
	// 日本語キーボードのみ
	{ SDL_SCANCODE_BACKSLASH,		KVC_YEN },			// ¥	|
	{ SDL_SCANCODE_RIGHTBRACKET,	KVC_RBRACKET },		// ]	}
	{ SDL_SCANCODE_NONUSBACKSLASH,	KVC_UNDERSCORE },	// ¥	_
//	{ 				,				KVC_HANZEN },		// 半角/全角
//	{ 				,				KVC_MUHENKAN },		// 無変換
//	{ 				,				KVC_HENKAN },		// 変換
//	{ 				,				KVC_HIRAGANA },		// ひらがな
	
	// 追加キー
	{ SDL_SCANCODE_LGUI,			KVX_LMETA },		// L-Meta
	{ SDL_SCANCODE_RGUI,			KVX_RMETA },		// R-Meta
	{ SDL_SCANCODE_MENU,			KVX_MENU }			// Menu
};
#else
static const struct {	// SDLキーコード -> 仮想キーコード定義
	int InKey;			// SDLのキーコード
	PCKEYsym VKey;		// 仮想キーコード
} VKeyDef[] = {
	{ SDLK_UNKNOWN,			KVC_UNKNOWN },
	
	{ SDLK_1,				KVC_1 },			// 1	!
	{ SDLK_2,				KVC_2 },			// 2	"
	{ SDLK_3,				KVC_3 },			// 3	#
	{ SDLK_4,				KVC_4 },			// 4	$
	{ SDLK_5,				KVC_5 },			// 5	%
	{ SDLK_6,				KVC_6 },			// 6	&
	{ SDLK_7,				KVC_7 },			// 7	'
	{ SDLK_8,				KVC_8 },			// 8	(
	{ SDLK_9,				KVC_9 },			// 9	)
	{ SDLK_0,				KVC_0 },			// 0
	
	{ SDLK_a,				KVC_A },			// a	A
	{ SDLK_b,				KVC_B },			// b	B
	{ SDLK_c,				KVC_C },			// c	C
	{ SDLK_d,				KVC_D },			// d	D
	{ SDLK_e,				KVC_E },			// e	E
	{ SDLK_f,				KVC_F },			// f	F
	{ SDLK_g,				KVC_G },			// g	G
	{ SDLK_h,				KVC_H },			// h	H
	{ SDLK_i,				KVC_I },			// i	I
	{ SDLK_j,				KVC_J },			// j	J
	{ SDLK_k,				KVC_K },			// k	K
	{ SDLK_l,				KVC_L },			// l	L
	{ SDLK_m,				KVC_M },			// m	M
	{ SDLK_n,				KVC_N },			// n	N
	{ SDLK_o,				KVC_O },			// o	O
	{ SDLK_p,				KVC_P },			// p	P
	{ SDLK_q,				KVC_Q },			// q	Q
	{ SDLK_r,				KVC_R },			// r	R
	{ SDLK_s,				KVC_S },			// s	S
	{ SDLK_t,				KVC_T },			// t	T
	{ SDLK_u,				KVC_U },			// u	U
	{ SDLK_v,				KVC_V },			// v	V
	{ SDLK_w,				KVC_W },			// w	W
	{ SDLK_x,				KVC_X },			// x	X
	{ SDLK_y,				KVC_Y },			// y	Y
	{ SDLK_z,				KVC_Z },			// z	Z
	
	{ SDLK_F1,				KVC_F1 },			// F1
	{ SDLK_F2,				KVC_F2 },			// F2
	{ SDLK_F3,				KVC_F3 },			// F3
	{ SDLK_F4,				KVC_F4 },			// F4
	{ SDLK_F5,				KVC_F5 },			// F5
	{ SDLK_F6,				KVC_F6 },			// F6
	{ SDLK_F7,				KVC_F7 },			// F7
	{ SDLK_F8,				KVC_F8 },			// F8
	{ SDLK_F9,				KVC_F9 },			// F9
	{ SDLK_F10,				KVC_F10 },			// F10
	{ SDLK_F11,				KVC_F11 },			// F11
	{ SDLK_F12,				KVC_F12 },			// F12
	
	{ SDLK_MINUS,			KVC_MINUS },		// -	=
	{ SDLK_CARET,			KVC_CARET },		// ^	~
	{ SDLK_BACKSPACE,		KVC_BACKSPACE },	// BackSpace
	{ SDLK_AT,				KVC_AT },			// @	`
	{ SDLK_LEFTBRACKET,		KVC_LBRACKET },		// [	{
	{ SDLK_SEMICOLON,		KVC_SEMICOLON },	// ;	+
	{ SDLK_COLON,			KVC_COLON },		// :	*
	{ SDLK_COMMA,			KVC_COMMA },		// ,	<
	{ SDLK_PERIOD,			KVC_PERIOD },		// .	>
	{ SDLK_SLASH,			KVC_SLASH },		// /	?
	{ SDLK_SPACE,			KVC_SPACE },		// Space
	
	{ SDLK_ESCAPE,			KVC_ESC },			// ESC
	{ SDLK_BACKQUOTE,		KVC_HANZEN },		// 半角/全角
	{ SDLK_TAB,				KVC_TAB },			// Tab
	{ SDLK_CAPSLOCK,		KVC_CAPSLOCK },		// CapsLock
	{ SDLK_RETURN,			KVC_ENTER },		// Enter
	{ SDLK_LCTRL,			KVC_LCTRL },		// L-Ctrl
	{ SDLK_RCTRL,			KVC_RCTRL },		// R-Ctrl
	{ SDLK_LSHIFT,			KVC_LSHIFT },		// L-Shift
	{ SDLK_RSHIFT,			KVC_RSHIFT },		// R-Shift
	{ SDLK_LALT,			KVC_LALT },			// L-Alt
	{ SDLK_RALT,			KVC_RALT },			// R-Alt
	{ SDLK_PRINT,			KVC_PRINT },		// PrintScreen
	{ SDLK_SCROLLOCK,		KVC_SCROLLLOCK },	// ScrollLock
	{ SDLK_PAUSE,			KVC_PAUSE },		// Pause
	{ SDLK_INSERT,			KVC_INSERT },		// Insert
	{ SDLK_DELETE,			KVC_DELETE },		// Delete
	{ SDLK_END,				KVC_END },			// End
	{ SDLK_HOME,			KVC_HOME },			// Home
	{ SDLK_PAGEUP,			KVC_PAGEUP },		// PageUp
	{ SDLK_PAGEDOWN,		KVC_PAGEDOWN },		// PageDown
	
	{ SDLK_UP,				KVC_UP },			// ↑
	{ SDLK_DOWN,			KVC_DOWN },			// ↓
	{ SDLK_LEFT,			KVC_LEFT },			// ←
	{ SDLK_RIGHT,			KVC_RIGHT },		// →
	
	{ SDLK_KP0,				KVC_P0 },			// [0]
	{ SDLK_KP1,				KVC_P1 },			// [1]
	{ SDLK_KP2,				KVC_P2 },			// [2]
	{ SDLK_KP3,				KVC_P3 },			// [3]
	{ SDLK_KP4,				KVC_P4 },			// [4]
	{ SDLK_KP5,				KVC_P5 },			// [5]
	{ SDLK_KP6,				KVC_P6 },			// [6]
	{ SDLK_KP7,				KVC_P7 },			// [7]
	{ SDLK_KP8,				KVC_P8 },			// [8]
	{ SDLK_KP9,				KVC_P9 },			// [9]
	{ SDLK_NUMLOCK,			KVC_NUMLOCK },		// NumLock
	{ SDLK_KP_PLUS,			KVC_P_PLUS },		// [+]
	{ SDLK_KP_MINUS,		KVC_P_MINUS },		// [-]
	{ SDLK_KP_MULTIPLY,		KVC_P_MULTIPLY },	// [*]
	{ SDLK_KP_DIVIDE,		KVC_P_DIVIDE },		// [/]
	{ SDLK_KP_PERIOD,		KVC_P_PERIOD },		// [.]
	{ SDLK_KP_ENTER,		KVC_P_ENTER },		// [Enter]
	
	// 日本語キーボードのみ
	{ SDLK_BACKSLASH,		KVC_YEN },			// ¥	|
	{ SDLK_RIGHTBRACKET,	KVC_RBRACKET },		// ]	}
	{ SDLK_LESS,			KVC_UNDERSCORE },	// ¥	_
//	{ 				,		KVC_MUHENKAN },		// 無変換
//	{ 				,		KVC_HENKAN },		// 変換
//	{ 				,		KVC_HIRAGANA },		// ひらがな
	
	// 追加キー
	{ SDLK_LMETA,			KVX_LMETA },		// L-Meta
	{ SDLK_RMETA,			KVX_RMETA },		// R-Meta
	{ SDLK_MENU,			KVX_MENU }			// Menu
};
#endif


///////////////////////////////////////////////////////////
// メッセージ配列
///////////////////////////////////////////////////////////
// 一般メッセージ
const char *MsgStr[] = {
	"終了してよろしいですか?",
	"終了確認",
	"再起動してよろしいですか?",
	"変更は再起動後に有効となります。\n今すぐ再起動しますか?",
	"再起動確認",
	"拡張ROMを挿入してリセットします。",
	"拡張ROMを排出してリセットします。",
	"リセット確認"
};

// メニュー用メッセージ ------
const char *MsgMen[] = {
	"ビデオキャプチャ...",
	"ビデオキャプチャ停止",
	"記録...",	// (リプレイ)
	"記録停止",	// (リプレイ)
	"再生...",	// (リプレイ)
	"再生停止"	// (リプレイ)
};

// INIファイル用メッセージ ------
const char *MsgIni[] = {
	// [CONFIG]
	"; === PC6001V 初期設定ファイル ===\n\n",
//	"機種 60:PC-6001 62:PC-6001mk2 66:PC-6601 64:PC-6001mk2SR 68:PC-6601SR",
	"機種 60:PC-6001 62:PC-6001mk2 66:PC-6601",
	"FDD接続台数 (0-2)",
	"拡張RAM使用",
	"Turbo TAPE Yes:有効 No:無効",
	"BoostUp Yes:有効 No:無効",
	"BoostUp 最大倍率(N60モード)",
	"BoostUp 最大倍率(N60m/N66モード)",
	"オーバークロック率 (1-1000)%",
	"CRCチェック Yes:有効 No:無効",
	"ROMパッチ Yes:あてる No:あてない",
	// [DISPLAY]
	"カラーモード 8:8bit 16:16bit 24:24bit",
	"MODE4カラーモード 0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク",
	"スキャンライン Yes:あり No:なし",
	"スキャンライン輝度 (0-100)%",
	"4:3表示 Yes:有効 No:無効",
	"フルスクリーンモード Yes:有効 No:無効",
	"ステータスバー Yes:表示 No:非表示",
	"フレームスキップ",
	// [SOUND]
	"サンプリングレート (44100/22050/11025)Hz",
	"サウンドバッファサイズ",
	"マスター音量 (0-100)",
	"PSG音量 (0-100)",
	"音声合成音量 (0-100)",
	"TAPEモニタ音量 (0-100)",
	"PSG LPFカットオフ周波数(0で無効)",
	// [MOVIE]
	"RLEフラグ Yes:RLE有効 No:ベタ",
	// [FILES]
	"拡張ROMファイル名",
	"TAPE(LODE)ファイル名(起動時に自動マウント)",
	"TAPE(SAVE)ファイル名(SAVE時に自動マウント)",
	"DISK1ファイル名(起動時に自動マウント)",
	"DISK2ファイル名(起動時に自動マウント)",
	"プリンタ出力ファイル名",
	"全角フォントファイル名",
	"半角フォントファイル名",
	// [PATH]
	"ROMイメージ格納パス",
	"TAPEイメージ格納パス",
	"DISKイメージ格納パス",
	"拡張ROMイメージ格納パス",
	"スナップショット格納パス",
	"WAVEファイル格納パス",
	"FONT格納パス",
	// [CHECK]
	"終了時確認 Yes:する No:しない",
	"終了時INIファイルを保存 Yes:する No:しない",
	// [KEY]
	"キーリピートの間隔(単位:ms 0で無効)",
	"PS/2キーボード使う? Yes:PS/2 No:USB",
	// [OPTION]
	"戦士のカートリッジ Yes:有効 No:無効"
	
};

// どこでもSAVE用メッセージ ------
const char *MsgDok[] = {
	"; === PC6001V どこでもSAVEファイル ===\n\n"
};

// Error用メッセージ ------
const char *MsgErr[] = {
	"Error",
	"エラーはありません",
	"原因不明のエラーが発生しました",
	"メモリの確保に失敗しました",
	"指定された機種のROMイメージが見つからないため機種を変更しました\n設定を確認してください",
    "ROMイメージが見つかりません\n設定を確認してください",
	"ROMイメージのサイズが不正です",
	"ROMイメージのCRCが不正です",
	"ライブラリの初期化に失敗しました",
	"初期化に失敗しました\n設定を確認してください",
	"フォントの読込みに失敗しました",
	"フォントファイルの作成に失敗しました",
	"INIファイルの読込みに失敗しました\nデフォルト設定で起動します",
	"INIファイルの読込みに失敗しました",
	"INIファイルの保存に失敗しました",
	"TAPEイメージのマウントに失敗しました",
	"DISKイメージのマウントに失敗しました",
	"拡張ROMイメージのマウントに失敗しました",
	"どこでもLOADに失敗しました",
	"どこでもSAVEに失敗しました",
	"どこでもLOADに失敗しました\n保存時とPC6001Vのバージョンが異なります",
	"リプレイ再生に失敗しました",
	"リプレイ記録に失敗しました",
	"リプレイデータがありません"
};


////////////////////////////////////////////////////////////////
// 小文字による文字列比較
//
// 引数:	s1			比較する文字列へのポインタ1
//			s2			比較する文字列へのポインタ2
// 返値:	int			0:等しい 正数:s1>s2 負数:s1<s2
////////////////////////////////////////////////////////////////
int stricmp ( const char *s1, const char *s2 )
{
	const char *ps1 = s1, *ps2 = s2;
	
	if( !s1 ) return -1;
	if( !s2 ) return  1;
	
	// 先頭の文字
	char c1 = *ps1++;
	char c2 = *ps2++;
	
	// どちらかの文字列終端まで繰り返し
	while( c1 && c2 ){
		// 小文字に変換
		if( c1 >= 'A' && c1 <= 'Z' ) c1 += 'a' - 'A';
		if( c2 >= 'A' && c2 <= 'Z' ) c2 += 'a' - 'A';
		
		// 等しくなかったら比較終了
		if( c1 != c2 ) break;
		
		// 次の文字取得
		c1 = *ps1++;
		c2 = *ps2++;
	}
	
	return (int)c1 - (int)c2;
}




////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_Init( void )
{
	if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) )
		return false;
	
	// SDLキーコード  -> 仮想キーコード 変換テーブル初期化
	ZeroMemory( &VKTable, sizeof(VKTable) );
	for( int i=0; i < COUNTOF(VKeyDef); i++ ) VKTable[VKeyDef[i].InKey] = VKeyDef[i].VKey;
	
	#if SDL_VERSION_ATLEAST(2,0,0)
	// SDL2.0はキーシンボルがUnicodeに対応しているらしい
	#else
	// Unicode有効 オーバーヘッドは気にしないことにする
	SDL_EnableUNICODE( 1 );
	#endif
	
	#ifndef USESDLTIMER
	tif = NULL;
	#endif
	
	return true;
}


////////////////////////////////////////////////////////////////
// キーリピート設定
//
// 引数:	repeat	キーリピートの間隔(ms) 0で無効
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_SetKeyRepeat( int repeat )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	// SDL2.0にはキーリピートの設定がないらしい
	#else
	SDL_EnableKeyRepeat( 500, repeat );
	#endif
}


////////////////////////////////////////////////////////////////
// OSDキーコード -> 仮想キーコード変換
//
// 引数:	scode		環境依存のキーコード
// 返値:	PCKEYsym	仮想キーコード
////////////////////////////////////////////////////////////////
PCKEYsym OSD_ConvertKeyCode( int scode )
{
	return VKTable[scode];
}


////////////////////////////////////////////////////////////////
// 利用可能なジョイスティック数取得
//
// 引数:	なし
// 返値:	int			利用可能なジョイスティック数
////////////////////////////////////////////////////////////////
int OSD_GetJoyNum( void )
{
	return SDL_NumJoysticks();
}


////////////////////////////////////////////////////////////////
// ジョイスティック名取得
//
// 引数:	int			インデックス
// 返値:	char *		ジョイスティック名文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetJoyName( int index )
{
	return SDL_JoystickName( index );
}


////////////////////////////////////////////////////////////////
// ジョイスティックオープンされてる？
//
// 引数:	int			インデックス
// 返値:	bool		true:OPEN false:CLOSE
////////////////////////////////////////////////////////////////
bool OSD_OpenedJoy( int index )
{
	return SDL_JoystickOpened( index ) ? true : false;
}


////////////////////////////////////////////////////////////////
// ジョイスティックオープン
//
// 引数:	int			インデックス
// 返値:	HJOYINFO	ジョイスティック情報へのポインタ
////////////////////////////////////////////////////////////////
HJOYINFO OSD_OpenJoy( int index )
{
	return (HJOYINFO)SDL_JoystickOpen( index );
}


////////////////////////////////////////////////////////////////
// ジョイスティッククローズ
//
// 引数:	int			インデックス
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_CloseJoy( HJOYINFO jinfo )
{
	SDL_JoystickClose( (SDL_Joystick *)jinfo );
}


////////////////////////////////////////////////////////////////
// ジョイスティックの軸の数取得
//
// 引数:	HJOYINFO	ジョイスティック情報へのポインタ
// 返値:	int			軸の数
////////////////////////////////////////////////////////////////
int OSD_GetJoyNumAxes( HJOYINFO jinfo )
{
	return SDL_JoystickNumAxes( (SDL_Joystick *)jinfo );
}


////////////////////////////////////////////////////////////////
// ジョイスティックのボタンの数取得
//
// 引数:	HJOYINFO	ジョイスティック情報へのポインタ
// 返値:	int			ボタンの数
////////////////////////////////////////////////////////////////
int OSD_GetJoyNumButtons( HJOYINFO jinfo )
{
	return SDL_JoystickNumButtons( (SDL_Joystick *)jinfo );
}


////////////////////////////////////////////////////////////////
// ジョイスティックの軸の状態取得
//
// 引数:	HJOYINFO	ジョイスティック情報へのポインタ
//			int			軸の番号
// 返値:	int			軸の状態(-32768〜32767)
////////////////////////////////////////////////////////////////
int OSD_GetJoyAxis( HJOYINFO jinfo, int num )
{
	return SDL_JoystickGetAxis( (SDL_Joystick *)jinfo, num );
}


////////////////////////////////////////////////////////////////
// ジョイスティックのボタンの状態取得
//
// 引数:	HJOYINFO	ジョイスティック情報へのポインタ
//			int			ボタンの番号
// 返値:	bool		ボタンの状態 true:ON false:OFF
////////////////////////////////////////////////////////////////
bool OSD_GetJoyButton( HJOYINFO jinfo, int num )
{
	return SDL_JoystickGetButton( (SDL_Joystick *)jinfo, num ) ? true : false;
}


////////////////////////////////////////////////////////////////
// オーディオデバイスオープン
//
// 引数:	obj			自分自身へのオブジェクトポインタ
//			callback	コールバック関数へのポインタ
//			rate		サンプリングレート
//			sample		バッファサイズ(サンプル数)
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_OpenAudio( void *obj, CBF_SND callback, int rate, int samples )
{
	SDL_AudioSpec ASpec;				// オーディオスペック
	
	ASpec.freq     = rate;				// サンプリングレート
	ASpec.format   = AUDIOFORMAT;		// フォーマット
	ASpec.channels = 1;					// モノラル
	ASpec.samples  = samples;			// バッファサイズ(サンプル数)
	ASpec.callback = callback;			// コールバック関数の指定
	ASpec.userdata = obj;				// コールバック関数に自分自身のオブジェクトポインタを渡す
	
	// オーディオデバイスを開く
	if( SDL_OpenAudio( &ASpec, NULL ) < 0 ) return false;
	
	return true;
}


////////////////////////////////////////////////////////////////
// オーディオデバイスクローズ
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_CloseAudio( void )
{
	SDL_CloseAudio();
}


////////////////////////////////////////////////////////////////
// 再生開始
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_StartAudio( void )
{
	SDL_PauseAudio( 0 );
}


////////////////////////////////////////////////////////////////
// 再生停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_StopAudio( void )
{
	SDL_PauseAudio( 1 );
}


////////////////////////////////////////////////////////////////
// 再生状態取得
//
// 引数:	なし
// 返値:	bool		true:再生中 false:停止中
////////////////////////////////////////////////////////////////
bool OSD_AudioPlaying( void )
{
	return SDL_GetAudioStatus() == SDL_AUDIO_PLAYING ? true : false;
}


////////////////////////////////////////////////////////////////
// Waveファイル読込み
// 　対応形式は 22050Hz以上,符号付き16bit,1ch
//
// 引数:	filepath	ファイルパス
//			buf			バッファポインタ格納ポインタ
//			len			ファイル長さ格納ポインタ
//			freq		サンプリングレート格納ポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_LoadWAV( const char *filepath, BYTE **buf, DWORD *len, int *freq )
{
	SDL_AudioSpec ws;
	
	if( !SDL_LoadWAV( filepath, &ws, buf, (Uint32 *)len ) ) return false;
	
	if( ws.freq < 22050 || ws.format != AUDIO_S16 || ws.channels != 1 ){
		SDL_FreeWAV( *buf );
		return false;
	}
	
	*freq    = ws.freq;
	
	return true;
}


////////////////////////////////////////////////////////////////
// Waveファイル開放
//
// 引数:	buf			バッファへのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_FreeWAV( BYTE *buf )
{
	SDL_FreeWAV( buf );
}


////////////////////////////////////////////////////////////////
// 指定時間待機
//
// 引数:	tms			待機時間(ms)
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_Delay( DWORD tms )
{
	#ifdef USESDLTIMER
	SDL_Delay( tms );
	#else
	Sleep( tms );
	#endif
}


////////////////////////////////////////////////////////////////
// プロセス開始からの経過時間取得
//
// 引数:	なし
// 返値:	DWORD		経過時間(ms)
////////////////////////////////////////////////////////////////
DWORD OSD_GetTicks( void )
{
	#ifdef USESDLTIMER
	return SDL_GetTicks();
	#else
	return GetTickCount();
	#endif
}


////////////////////////////////////////////////////////////////
// タイマ追加
//
// 引数:	interval	割込み間隔(ms)
//			callback	コールバック関数
//			param		コールバック関数に渡す引数
// 返値:	HTIMERID	タイマID(失敗したらNULL)
////////////////////////////////////////////////////////////////
HTIMERID OSD_AddTimer( DWORD interval, CBF_TMR callback, void *param )
{
	#ifdef USESDLTIMER
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_TimerID *pid = NULL;
	SDL_TimerID tid = SDL_AddTimer( interval, (SDL_TimerCallback)callback, param );
	if( tid ){
		pid = new SDL_TimerID;
		if( pid ) *pid = tid;
	}
	return (HTIMERID)pid;
	#else
	return (HTIMERID)SDL_AddTimer( interval, (SDL_NewTimerCallback)callback, param );
	#endif
	#else
	TINFO *tinfo = new TINFO;
	if( !tinfo ) return NULL;
	
	timeBeginPeriod( TIMERRES );
	tinfo->interval = interval;
	tinfo->callback = callback;
	tinfo->param    = param;
	tinfo->id       = timeSetEvent( interval, 1, OsdTimerCallbackProc, 0, TIME_PERIODIC );
	if( !tinfo->id ){
		delete tinfo;
		timeEndPeriod( TIMERRES );
		return NULL;
	}
	
	TINFO *ti = tif;
	if( !ti ) tif = tinfo;
	else{
		while( ti->next ) ti = ti->next;
		ti->next = tinfo;
	}
	
	return (HTIMERID)tinfo;
	#endif
}

#ifndef USESDLTIMER
// OSD_AddTimer()で使用するコールバックプロシージャ
static void CALLBACK OsdTimerCallbackProc( UINT id,  UINT, DWORD_PTR data, DWORD_PTR, DWORD_PTR )
{
	TINFO *ti = tif;
	
	while( ti && (ti->id != id) ) ti = ti->next;
	if( ti ) ti->callback( ti->interval, ti->param );
}
#endif


////////////////////////////////////////////////////////////////
// タイマ削除
//
// 引数:	id			タイマID
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_DelTimer( HTIMERID id )
{
	#ifdef USESDLTIMER
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( SDL_RemoveTimer( *((SDL_TimerID *)id) ) ){
		delete (SDL_TimerID *)id;
		return true;
	}else
		return false;
	#else
	return (bool)SDL_RemoveTimer( (SDL_TimerID)id );
	#endif
	#else
	if( !id ) return false;
	
	TINFO *tinfo = (TINFO *)id;
	TINFO *ti    = tif;
	TINFO *tip   = NULL;
	
	while( ti && (ti->id != tinfo->id) ){
		tip = ti;
		ti  = ti->next;
	}
	if( !ti ) return false;
	
	if( timeKillEvent( ti->id ) != TIMERR_NOERROR )
		return false;
	timeEndPeriod( TIMERRES );
	
	delete ti;
	if( tip ) tip->next = NULL;
	else      tif = NULL;
	
	return true;
	#endif
}


////////////////////////////////////////////////////////////////
// キャプション設定
//
// 引数:	wh			ウィンドウハンドル
//			str			キャプション文字列へのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_SetWindowCaption( HWINDOW wh, const char *str )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( wh && str ) SDL_SetWindowTitle( (SDL_Window*)wh, str );
	#else
	if( str ) SDL_WM_SetCaption( str, "" );
	#endif
}


////////////////////////////////////////////////////////////////
// マウスカーソル表示/非表示
//
// 引数:	disp		true:表示 false:非表示
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_ShowCursor( bool disp )
{
	SDL_ShowCursor( disp ? SDL_ENABLE : SDL_DISABLE );
}


////////////////////////////////////////////////////////////////
// ウィンドウ作成
//
// 引数:	pwh			ウィンドウハンドルへのポインタ
//			w			幅
//			h			高さ
//			bpp			色深度
//			fsflag		true:フルスクリーン false:ウィンドウ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_CreateWindow( HWINDOW *pwh, int w, int h, int bpp, bool fsflag )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( !*pwh ){
		*pwh = (HWINDOW)SDL_CreateWindow( "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h,
											SDL_WINDOW_SHOWN );
		if( !*pwh ) return false;
	}
	
	SDL_Renderer *rend = SDL_GetRenderer( (SDL_Window *)*pwh );
	if( rend ) SDL_DestroyRenderer( rend );
	
	if( fsflag ){
		SDL_DisplayMode mode;
		
		mode.format = bpp ==  8 ? SDL_PIXELFORMAT_INDEX8 :
					  bpp == 16 ? SDL_PIXELFORMAT_RGB555 :
								  SDL_PIXELFORMAT_RGB24;
		mode.w = w;
		mode.h = h;
		mode.refresh_rate = 0;
		mode.driverdata   = 0;
		SDL_SetWindowDisplayMode( (SDL_Window *)*pwh, &mode );
		SDL_SetWindowFullscreen( (SDL_Window *)*pwh, SDL_TRUE );
	}else{
		SDL_SetWindowFullscreen( (SDL_Window *)*pwh, SDL_FALSE );
		SDL_SetWindowSize( (SDL_Window *)*pwh, w, h );
	}
	
	rend = SDL_CreateRenderer( (SDL_Window *)*pwh, -1, SDL_RENDERER_ACCELERATED );
	#else
	*pwh = (HWINDOW)SDL_SetVideoMode( w, h, bpp, SDLOP_SCREEN | (fsflag ? SDL_FULLSCREEN : 0) );
	#endif
	return *pwh ? true : false;
}


////////////////////////////////////////////////////////////////
// ウィンドウ破棄
//
// 引数:	wh			ウィンドウハンドル
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_DestroyWindow( HWINDOW wh )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( wh ){
		SDL_Renderer *rend = SDL_GetRenderer( (SDL_Window *)wh );
		if( rend ) SDL_DestroyRenderer( rend );
		SDL_DestroyWindow( (SDL_Window *)wh );
	}
	#else
	// SDL1.2は無視
	#endif
}


////////////////////////////////////////////////////////////////
// ウィンドウの幅を取得
//
// 引数:	wh			ウィンドウハンドル
// 返値:	int			幅
////////////////////////////////////////////////////////////////
int OSD_GetWindowWidth( HWINDOW wh )
{
	int res = 0;
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( wh ) SDL_GetWindowSize( (SDL_Window *)wh, &res, NULL );
	#else
	if( wh ) res = ((SDL_Surface *)wh)->w;
	#endif
	return res;
}


////////////////////////////////////////////////////////////////
// ウィンドウの高さを取得
//
// 引数:	wh			ウィンドウハンドル
// 返値:	int			高さ
////////////////////////////////////////////////////////////////
int OSD_GetWindowHeight( HWINDOW wh )
{
	int res = 0;
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( wh ) SDL_GetWindowSize( (SDL_Window *)wh, NULL, &res );
	#else
	if( wh ) res = ((SDL_Surface *)wh)->h;
	#endif
	return res;
}


////////////////////////////////////////////////////////////////
// ウィンドウの色深度を取得
//
// 引数:	wh			ウィンドウハンドル
// 返値:	int			色深度
////////////////////////////////////////////////////////////////
int OSD_GetWindowBPP( HWINDOW wh )
{
	int res = 0;
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( wh ) res = (SDL_GetWindowSurface( (SDL_Window *)wh ))->format->BitsPerPixel;
	#else
	if( wh ) res = ((SDL_Surface *)wh)->format->BitsPerPixel;
	#endif
	return res;
}


////////////////////////////////////////////////////////////////
// パレット設定
//
// 引数:	wh			ウィンドウハンドル
//			pal			パレットへのポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_SetPalette( HWINDOW wh, VPalette *pal )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	return SDL_SetSurfacePalette( SDL_GetWindowSurface( (SDL_Window *)wh ), (SDL_Palette *)pal ) ? false : true;
	#else
	return SDL_SetPalette( (SDL_Surface *)wh, SDL_LOGPAL|SDL_PHYSPAL, (SDL_Color *)pal->colors, 0, pal->ncols ) ? false : true;
	#endif
}


////////////////////////////////////////////////////////////////
// ウィンドウクリア
//  色は0(黒)で決め打ち
//
// 引数:	wh			ウィンドウハンドル
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_ClearWindow( HWINDOW wh )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_Renderer *rend = SDL_GetRenderer( (SDL_Window *)wh );
	SDL_SetRenderDrawColor( rend, 0, 0, 0, 0xff );
	SDL_RenderClear( rend );

//	SDL_Surface *sur = SDL_GetWindowSurface( (SDL_Window *)wh );
//	SDL_FillRect( sur, NULL, SDL_MapRGB( sur->format, 0, 0, 0 ) );

	#else
	SDL_FillRect( (SDL_Surface *)wh, NULL, 0 );
	#endif
}


////////////////////////////////////////////////////////////////
// ウィンドウ反映
//
// 引数:	wh			ウィンドウハンドル
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_RenderWindow( HWINDOW wh )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_Renderer *rend = SDL_GetRenderer( (SDL_Window *)wh );
	SDL_RenderPresent( rend );

//	SDL_UpdateWindowSurface( (SDL_Window *)wh );

	#else
 	SDL_Flip( (SDL_Surface *)wh );
	#endif

}


////////////////////////////////////////////////////////////////
// ウィンドウに転送
//
// 引数:	wh			ウィンドウハンドル
//			src			転送元サーフェス
//			pal			パレットへのポインタ
//			x,y			転送先座標
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_BlitToWindow( HWINDOW wh, VSurface *src, int x, int y, VPalette *pal )
{
	VRect src1,drc1;
	
	if( !src || !wh ) return;
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_Surface *dst  = SDL_GetWindowSurface( (SDL_Window *)wh );
	#else
	SDL_Surface *dst  = SDL_GetVideoSurface();
	#endif
	
	// 転送元範囲設定
	src1.x = max( 0, -x );
	src1.y = max( 0, -y );
	src1.w = min( src->Width()  - src1.x, dst->w );
	src1.h = min( src->Height() - src1.y, dst->h );
	
	if( src1.w <= 0 || src1.h <= 0 ) return;
	
	// 転送先範囲設定
	drc1.x = max( 0, x );
	drc1.y = max( 0, y );
	
	
	if( SDL_MUSTLOCK( dst ) ) SDL_LockSurface( dst );
	
	BYTE *psrc = (BYTE *)src->GetPixels() + src->Pitch() * src1.y + src1.x * src->Bpp() / 8;
	BYTE *pdst = (BYTE *)dst->pixels      + dst->pitch   * drc1.y + drc1.x * dst->format->BytesPerPixel;
	
	if( src->Bpp() == dst->format->BitsPerPixel ){
		for( int i=0; i < src1.h; i++ ){
			memcpy( pdst, psrc, src1.w * dst->format->BytesPerPixel );
			psrc += src->Pitch();
			pdst += dst->pitch;
		}
	}else if( src->Bpp() == 8 && dst->format->BitsPerPixel == 16 ){	// 8 -> 16 bpp (5:6:5)
		for( int i=0; i < src1.h; i++ ){
			for( int j=0; j < src1.w; j++ ){
				WORD wdat = (WORD)(pal->colors[*psrc].r & 0xf8) << 8
				          | (WORD)(pal->colors[*psrc].g & 0xfc) << 3
				          | (WORD)(pal->colors[*psrc].b & 0xf8) >> 3;
				*pdst++ = (BYTE)( wdat       & 0xff);
				*pdst++ = (BYTE)((wdat >> 8) & 0xff);
				psrc++;
			}
			psrc += src->Pitch() - src1.w;
			pdst += dst->pitch   - src1.w * 2;
		}
	}else if( src->Bpp() == 8 && dst->format->BitsPerPixel == 24 ){	// 8 -> 24 bpp
		for( int i=0; i < src1.h; i++ ){
			for( int j=0; j < src1.w; j++ ){
				*pdst++ = pal->colors[*psrc].b;
				*pdst++ = pal->colors[*psrc].g;
				*pdst++ = pal->colors[*psrc].r;
				psrc++;
			}
			psrc += src->Pitch() - src1.w;
			pdst += dst->pitch   - src1.w * 3;
		}
	}
	
	if( SDL_MUSTLOCK( dst ) ) SDL_UnlockSurface( dst );
}


////////////////////////////////////////////////////////////////
// ウィンドウに転送(2倍)
//
// 引数:	wh			ウィンドウハンドル
//			src			転送元サーフェス
//			x,y			転送先座標
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_BlitToWindow2( HWINDOW wh, VSurface *src, int x, int y )
{
	VRect src1,drc1;
	
	if( !src || !wh ) return;
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_Surface *dst  = SDL_GetWindowSurface( (SDL_Window *)wh );
	#else
	SDL_Surface *dst  = SDL_GetVideoSurface();
	#endif
	
	// 転送元範囲設定
	src1.x = max( 0, -x/2 );
	src1.y = max( 0, -y/2 );
	src1.w = min( src->Width()  - src1.x, dst->w / 2 );
	src1.h = min( src->Height() - src1.y, dst->h / 2 );
	
	if( src1.w <= 0 || src1.h <= 0 ) return;
	
	// 転送先範囲設定
	drc1.x = max( 0, x );
	drc1.y = max( 0, y );
	
	
	if( SDL_MUSTLOCK( dst ) ) SDL_LockSurface( dst );
	
	BYTE *psrc = (BYTE *)src->GetPixels() + src->Pitch() * src1.y + src1.x * src->Bpp() / 8;
	BYTE *pdst = (BYTE *)dst->pixels      + dst->pitch   * drc1.y + drc1.x * dst->format->BytesPerPixel;
	
	if( src->Bpp() == dst->format->BitsPerPixel ){
		switch( src->Bpp() ){
		case 8:
			for( int i=0; i < src1.h; i++ ){
				for( int j=0; j < src1.w; j++ ){
					*pdst++ = *psrc;
					*pdst++ = *psrc++;
				}
				psrc += src->Pitch()   - src1.w;
				pdst += dst->pitch * 2 - src1.w * 2;
			}
			break;
			
		case 16:
			for( int i=0; i < src1.h; i++ ){
				for( int j=0; j < src1.w; j++ ){
					WORD d1 = *(WORD *)psrc++;
					psrc++;
					*((WORD *)pdst++) = d1;
					pdst++;
					*((WORD *)pdst++) = d1;
					pdst++;
				}
				psrc += src->Pitch()   - src1.w * 2;
				pdst += dst->pitch * 2 - src1.w * 4;
			}
			break;
			
		case 24:
			for( int i=0; i < src1.h; i++ ){
				for( int j=0; j < src1.w; j++ ){
					BYTE b = *psrc++;
					BYTE g = *psrc++;
					BYTE r = *psrc++;
					*pdst++ = b;
					*pdst++ = g;
					*pdst++ = r;
					*pdst++ = b;
					*pdst++ = g;
					*pdst++ = r;
				}
				psrc += src->Pitch()   - src1.w * 3;
				pdst += dst->pitch * 2 - src1.w * 6;
			}
		}
		
		pdst = (BYTE *)dst->pixels + dst->pitch * drc1.y + drc1.x * dst->format->BytesPerPixel;
		for( int i=0; i < src1.h; i++ ){
			memcpy( pdst + dst->pitch, pdst, src1.w * 2 * dst->format->BytesPerPixel );
			pdst += dst->pitch * 2;
		}
	}
	
	if( SDL_MUSTLOCK( dst ) ) SDL_UnlockSurface( dst );
}

////////////////////////////////////////////////////////////////
// イベント取得(イベントが発生するまで待つ)
//
// 引数:	ev			イベント情報共用体へのポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_GetEvent( Event *ev )
{
	SDL_Event event;
	
	if( !SDL_WaitEvent( &event ) ) return false;
	
	switch( event.type ){
	case SDL_KEYDOWN:
		ev->type			= EV_KEYDOWN;
		ev->key.state		= true;
		#if SDL_VERSION_ATLEAST(2,0,0)
		ev->key.sym			= OSD_ConvertKeyCode( event.key.keysym.scancode );
		#else
		ev->key.sym			= OSD_ConvertKeyCode( event.key.keysym.sym );
		#endif
		ev->key.mod			= (PCKEYmod)(
							  ( event.key.keysym.mod & KMOD_LSHIFT ? KVM_LSHIFT : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RSHIFT ? KVM_RSHIFT : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_LCTRL  ? KVM_LCTRL  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RCTRL  ? KVM_RCTRL  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_LALT   ? KVM_LALT   : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RALT   ? KVM_RALT   : KVM_NONE )
							#if SDL_VERSION_ATLEAST(2,0,0)
							| ( event.key.keysym.mod & KMOD_LGUI   ? KVM_LMETA  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RGUI   ? KVM_RMETA  : KVM_NONE )
							#else
							| ( event.key.keysym.mod & KMOD_LMETA  ? KVM_LMETA  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RMETA  ? KVM_RMETA  : KVM_NONE )
							#endif
							| ( event.key.keysym.mod & KMOD_NUM    ? KVM_NUM    : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_CAPS   ? KVM_CAPS   : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_MODE   ? KVM_MODE   : KVM_NONE ) );
		ev->key.unicode		= event.key.keysym.unicode;
		break;
		
	case SDL_KEYUP:
		ev->type			= EV_KEYUP;
		ev->key.state		= false;
		#if SDL_VERSION_ATLEAST(2,0,0)
		ev->key.sym			= OSD_ConvertKeyCode( event.key.keysym.scancode );
		#else
		ev->key.sym			= OSD_ConvertKeyCode( event.key.keysym.sym );
		#endif
		ev->key.mod			= (PCKEYmod)(
							  ( event.key.keysym.mod & KMOD_LSHIFT ? KVM_LSHIFT : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RSHIFT ? KVM_RSHIFT : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_LCTRL  ? KVM_LCTRL  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RCTRL  ? KVM_RCTRL  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_LALT   ? KVM_LALT   : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RALT   ? KVM_RALT   : KVM_NONE )
							#if SDL_VERSION_ATLEAST(2,0,0)
							| ( event.key.keysym.mod & KMOD_LGUI   ? KVM_LMETA  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RGUI   ? KVM_RMETA  : KVM_NONE )
							#else
							| ( event.key.keysym.mod & KMOD_LMETA  ? KVM_LMETA  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RMETA  ? KVM_RMETA  : KVM_NONE )
							#endif
							| ( event.key.keysym.mod & KMOD_NUM    ? KVM_NUM    : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_CAPS   ? KVM_CAPS   : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_MODE   ? KVM_MODE   : KVM_NONE ) );
		ev->key.unicode		= event.key.keysym.unicode;
		break;
		
	case SDL_MOUSEBUTTONDOWN:
		ev->type			= EV_MOUSEBUTTONDOWN;
		ev->mousebt.button	= event.button.button == SDL_BUTTON_LEFT      ? MBT_LEFT      :
							  event.button.button == SDL_BUTTON_MIDDLE    ? MBT_MIDDLE    :
							  event.button.button == SDL_BUTTON_RIGHT     ? MBT_RIGHT     :
//							  event.button.button == SDL_BUTTON_WHEELUP   ? MBT_WHEELUP   :
//							  event.button.button == SDL_BUTTON_WHEELDOWN ? MBT_WHEELDOWN :
							  MBT_NONE;
		ev->mousebt.state	= true;
		ev->mousebt.x		= event.button.x;
		ev->mousebt.y		= event.button.y;
		break;
		
	case SDL_MOUSEBUTTONUP:
		ev->type			= EV_MOUSEBUTTONUP;
		ev->mousebt.button	= event.button.button == SDL_BUTTON_LEFT      ? MBT_LEFT      :
							  event.button.button == SDL_BUTTON_MIDDLE    ? MBT_MIDDLE    :
							  event.button.button == SDL_BUTTON_RIGHT     ? MBT_RIGHT     :
//							  event.button.button == SDL_BUTTON_WHEELUP   ? MBT_WHEELUP   :
//							  event.button.button == SDL_BUTTON_WHEELDOWN ? MBT_WHEELDOWN :
							  MBT_NONE;
		ev->mousebt.state	= false;
		ev->mousebt.x		= event.button.x;
		ev->mousebt.y		= event.button.y;
		break;
		
	case SDL_JOYAXISMOTION:
		ev->type			= EV_JOYAXISMOTION;
		ev->joyax.idx		= event.jaxis.which;
		ev->joyax.axis		= event.jaxis.axis;
		ev->joyax.value		= event.jaxis.value;
		break;
		
	case SDL_JOYBUTTONDOWN:
		ev->type			= EV_JOYBUTTONDOWN;
		ev->joybt.idx		= event.jbutton.which;
		ev->joybt.button	= event.jbutton.button;
		ev->joybt.state		= true;
		break;
		
	case SDL_JOYBUTTONUP:
		ev->type			= EV_JOYBUTTONUP;
		ev->joybt.idx		= event.jbutton.which;
		ev->joybt.button	= event.jbutton.button;
		ev->joybt.state		= false;
		break;
		
	case SDL_QUIT:
		ev->type			= EV_QUIT;
		break;
		
	case UEV_RESTART:
		ev->type			= EV_RESTART;
		break;
		
	case UEV_DOKOLOAD:
		ev->type			= EV_DOKOLOAD;
		break;
		
	case UEV_REPLAY:
		ev->type			= EV_REPLAY;
		break;
		
	case UEV_FPSUPDATE:
		ev->type			= EV_FPSUPDATE;
		ev->fps.fps			= event.user.code;
		break;
		
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	case UEV_DEBUGMODEBP:
		ev->type			= EV_DEBUGMODEBP;
		ev->bp.addr			= event.user.code;
		break;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
		
	default:
		ev->type			= EV_NOEVENT;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// イベントをキューにプッシュする
//
// 引数:	ev			イベントタイプ
//			...			イベントタイプに応じた引数
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_PushEvent( EventType ev, ... )
{
	SDL_Event event;
	
	va_list args;
	va_start( args, ev );
	
	#if SDL_VERSION_ATLEAST(2,0,0)
	event.type = SDL_FIRSTEVENT;
	#else
	event.type = SDL_NOEVENT;
	#endif
	
	switch( ev ){
	case EV_QUIT:
		event.type		= SDL_QUIT;
		break;
		
	case EV_RESTART:
		event.type		= UEV_RESTART;
		break;
		
	case EV_DOKOLOAD:
		event.type		= UEV_DOKOLOAD;
		break;
		
	case EV_REPLAY:
		event.type		= UEV_REPLAY;
		break;
		
	case EV_FPSUPDATE:
		event.type		= UEV_FPSUPDATE;
		event.user.code	= va_arg( args, int );
		break;
		
	case EV_DEBUGMODEBP:
		event.type		= UEV_DEBUGMODEBP;
		event.user.code	= va_arg( args, int );
		break;
		
	default:
		return false;
	}
	
	va_end( args );
	
	return SDL_PushEvent( &event ) ? false : true;
}



////////////////////////////////////////////////////////////////
// 色の名前取得
//
// 引数:	num			色コード
// 返値:	char *		色の名前文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_ColorName( int num )
{
	static const char *JColorName[] = {
		"MODE1,2 黒(ボーダー)",
		"MODE1 Set1 緑",		"MODE1 Set1 深緑",		"MODE1 Set2 橙",		"MODE1 Set2 深橙",
		"MODE2 緑",				"MODE2 黄",				"MODE2 青",				"MODE2 赤",
		"MODE2 白",				"MODE2 シアン",			"MODE2 マゼンタ",		"MODE2 橙",
		"MODE3 Set1 緑",		"MODE3 Set1 黄",		"MODE3 Set1 青",		"MODE3 Set1 赤",
		"MODE3 Set2 白",		"MODE3 Set2 シアン",	"MODE3 Set2 マゼンタ",	"MODE3 Set2 橙",
		"MODE4 Set1 深緑",		"MODE4 Set1 緑",		"MODE4 Set2 黒",		"MODE4 Set2 白",
		"MODE4 Set1 にじみ 赤",	"MODE4 Set1 にじみ 黄",	"MODE4 Set1 にじみ -",	"MODE4 Set1 にじみ -",
		"MODE4 Set2 にじみ 赤",	"MODE4 Set2 にじみ 青",	"MODE4 Set2 にじみ 桃",	"MODE4 Set2 にじみ 緑",
		"mk2〜 透明(黒)",		"mk2〜 橙",				"mk2〜 青緑",			"mk2〜 黄緑",
		"mk2〜 青紫",			"mk2〜 赤紫",			"mk2〜 空色",			"mk2〜 灰色",
		"mk2〜 黒",				"mk2〜 赤",				"mk2〜 緑",				"mk2〜 黄",
        "mk2〜 青",				"mk2〜 マゼンタ",		"mk2〜 シアン",			"mk2〜 白",
        "MODE4 Set2 にじみ 明るい赤",
        "MODE4 Set2 にじみ 暗い赤",
        "MODE4 Set2 にじみ 明るい青",
        "MODE4 Set2 にじみ 暗い青",
        "MODE4 Set2 にじみ 明るい桃",
        "MODE4 Set2 にじみ 暗い桃",
        "MODE4 Set2 にじみ 明るい緑",
        "MODE4 Set2 にじみ 暗い緑",
    };
	
	if( num < 0 || num >= COUNTOF( JColorName ) ) return NULL;
	else                                          return JColorName[num];

}


////////////////////////////////////////////////////////////////
// キーの名前取得
//
// 引数:	sym			仮想キーコード
// 返値:	char *		キー名前文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_KeyName( PCKEYsym sym )
{
	static const PCKeyName kname[] = {
		{ KVC_1,			"1" },
		{ KVC_2,			"2" },
		{ KVC_3,			"3" },
		{ KVC_4,			"4" },
		{ KVC_5,			"5" },
		{ KVC_6,			"6" },
		{ KVC_7,			"7" },
		{ KVC_8,			"8" },
		{ KVC_9,			"9" },
		{ KVC_0,			"0" },
		
		{ KVC_A,			"A" },
		{ KVC_B,			"B" },
		{ KVC_C,			"C" },
		{ KVC_D,			"D" },
		{ KVC_E,			"E" },
		{ KVC_F,			"F" },
		{ KVC_G,			"G" },
		{ KVC_H,			"H" },
		{ KVC_I,			"I" },
		{ KVC_J,			"J" },
		{ KVC_K,			"K" },
		{ KVC_L,			"L" },
		{ KVC_M,			"M" },
		{ KVC_N,			"N" },
		{ KVC_O,			"O" },
		{ KVC_P,			"P" },
		{ KVC_Q,			"Q" },
		{ KVC_R,			"R" },
		{ KVC_S,			"S" },
		{ KVC_T,			"T" },
		{ KVC_U,			"U" },
		{ KVC_V,			"V" },
		{ KVC_W,			"W" },
		{ KVC_X,			"X" },
		{ KVC_Y,			"Y" },
		{ KVC_Z,			"Z" },
		
		{ KVC_F1,			"F1" },
		{ KVC_F2,			"F2" },
		{ KVC_F3,			"F3" },
		{ KVC_F4,			"F4" },
		{ KVC_F5,			"F5" },
		{ KVC_F6,			"F6" },
		{ KVC_F7,			"F7" },
		{ KVC_F8,			"F8" },
		{ KVC_F9,			"F9" },
		{ KVC_F10,			"F10" },
		{ KVC_F11,			"F11" },
		{ KVC_F12,			"F12" },
		
		{ KVC_MINUS,		"-" },
		{ KVC_CARET,		"^" },
		{ KVC_BACKSPACE,	"BackSpace" },
		{ KVC_AT,			"@" },
		{ KVC_LBRACKET,		"[" },
		{ KVC_SEMICOLON,	";" },
		{ KVC_COLON,		":" },
		{ KVC_COMMA,		"," },
		{ KVC_PERIOD,		"." },
		{ KVC_SLASH,		"/" },
		{ KVC_SPACE,		"Space" },
		
		{ KVC_ESC,			"ESC" },
		{ KVC_HANZEN,		"半角/全角" },
		{ KVC_TAB,			"Tab" },
		{ KVC_CAPSLOCK,		"CapsLock" },
		{ KVC_ENTER,		"Enter" },
		{ KVC_LCTRL,		"L-Ctrl" },
		{ KVC_RCTRL,		"R-Ctrl" },
		{ KVC_LSHIFT,		"L-Shift" },
		{ KVC_RSHIFT,		"R-Shift" },
		{ KVC_LALT,			"L-Alt" },
		{ KVC_RALT,			"R-Alt" },
		{ KVC_PRINT,		"PrintScreen" },
		{ KVC_SCROLLLOCK,	"ScrollLock" },
		{ KVC_PAUSE,		"Pause" },
		{ KVC_INSERT,		"Insert" },
		{ KVC_DELETE,		"Delete" },
		{ KVC_END,			"End" },
		{ KVC_HOME,			"Home" },
		{ KVC_PAGEUP,		"PageUp" },
		{ KVC_PAGEDOWN,		"PageDown" },
		
		{ KVC_UP,			"↑" },
		{ KVC_DOWN,			"↓" },
		{ KVC_LEFT,			"←" },
		{ KVC_RIGHT,		"→" },
		
		{ KVC_P0,			"0(テンキー)" },
		{ KVC_P1,			"1(テンキー)" },
		{ KVC_P2,			"2(テンキー)" },
		{ KVC_P3,			"3(テンキー)" },
		{ KVC_P4,			"4(テンキー)" },
		{ KVC_P5,			"5(テンキー)" },
		{ KVC_P6,			"6(テンキー)" },
		{ KVC_P7,			"7(テンキー)" },
		{ KVC_P8,			"8(テンキー)" },
		{ KVC_P9,			"9(テンキー)" },
		{ KVC_NUMLOCK,		"NumLock" },
		{ KVC_P_PLUS,		"+(テンキー)" },
		{ KVC_P_MINUS,		"-(テンキー)" },
		{ KVC_P_MULTIPLY,	"*(テンキー)" },
		{ KVC_P_DIVIDE,		"/(テンキー)" },
		{ KVC_P_PERIOD,		".(テンキー)" },
		{ KVC_P_ENTER,		"Enter(テンキー)" },
		
		// 日本語キーボードのみ
		{ KVC_YEN,			"\\" },
		{ KVC_RBRACKET,		"]" },
		{ KVC_UNDERSCORE,	"_" },
		{ KVC_MUHENKAN,		"無変換" },
		{ KVC_HENKAN,		"変換" },
		{ KVC_HIRAGANA,		"ひらがな" },
		
		// 英語キーボードのみ
		{ KVE_BACKSLASH,	"BackSlash" },
		
		// 追加キー
		{ KVX_RMETA,		"L-Windows" },
		{ KVX_LMETA,		"R-Windows" },
		{ KVX_MENU,			"Menu" }
	};
	
	const char *str = NULL;
	for( int i=0; i<(int)(sizeof(kname)/sizeof(PCKeyName)); i++ ){
		if( kname[i].PCKey == sym ){
			str = kname[i].Name;
			break;
		}
	}
	return str;
}

