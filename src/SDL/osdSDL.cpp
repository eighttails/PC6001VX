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
        "MODE4 Set1 にじみ 赤",		"MODE4 Set1 にじみ 青",		"MODE4 Set1 にじみ 桃",		"MODE4 Set1 にじみ 緑",
        "MODE4 Set1 にじみ 明赤",	"MODE4 Set1 にじみ 暗赤",	"MODE4 Set1 にじみ 明青",	"MODE4 Set1 にじみ 暗青",
        "MODE4 Set1 にじみ 明桃",	"MODE4 Set1 にじみ 暗桃",	"MODE4 Set1 にじみ 明緑",	"MODE4 Set1 にじみ 暗緑",
        "MODE4 Set2 にじみ 赤",		"MODE4 Set2 にじみ 青",		"MODE4 Set2 にじみ 桃",		"MODE4 Set2 にじみ 緑",
        "MODE4 Set2 にじみ 明赤",	"MODE4 Set2 にじみ 暗赤",	"MODE4 Set2 にじみ 明青",	"MODE4 Set2 にじみ 暗青",
        "MODE4 Set2 にじみ 明桃",	"MODE4 Set2 にじみ 暗桃",	"MODE4 Set2 にじみ 明緑",	"MODE4 Set2 にじみ 暗緑",
        "mk2〜 透明(黒)",		"mk2〜 橙",				"mk2〜 青緑",			"mk2〜 黄緑",
        "mk2〜 青紫",			"mk2〜 赤紫",			"mk2〜 空色",			"mk2〜 灰色",
        "mk2〜 黒",				"mk2〜 赤",				"mk2〜 緑",				"mk2〜 黄",
        "mk2〜 青",				"mk2〜 マゼンタ",		"mk2〜 シアン",			"mk2〜 白"
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

////////////////////////////////////////////////////////////////
// オーディオをロックする
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_LockAudio( void )
{
    SDL_LockAudio();
}


////////////////////////////////////////////////////////////////
// オーディオをアンロックする
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_UnlockAudio( void )
{
    SDL_UnlockAudio();
}

////////////////////////////////////////////////////////////////
// 終了処理
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_Quit( void )
{
    PRINTD( OSD_LOG, "[OSD][OSD_Quit]\n" );

    SDL_Quit();
}
