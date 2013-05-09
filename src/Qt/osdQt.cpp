// OS依存の汎用ルーチン(主にUI用)
#include "../typedef.h"

#include <QtCore>
#include <QtWidgets>
#include <QtMultimedia>

#ifndef NOJOYSTICK
#include <SDL2/SDL.h>
#endif

#include "../log.h"
#include "../osd.h"
#include "../common.h"
#include "../pc6001v.h"
#include "../joystick.h"

#include "renderview.h"

//エミュレータ内部用イベントキュー
QMutex eventMutex;
QQueue<Event> eventQueue;
QWaitCondition eventEmitted;

//経過時間タイマ
QElapsedTimer elapsedTimer;

std::map<int, PCKEYsym> VKTable;			// Qtキーコード  -> 仮想キーコード 変換テーブル
QVector<QRgb> PaletteTable;              //パレットテーブル

//サウンド関連
QPointer<QIODevice> audioBuffer = NULL;
QPointer<QAudioOutput> audioOutput = NULL;

static const struct {	// SDLキーコード -> 仮想キーコード定義
    int InKey;			// SDLのキーコード
    PCKEYsym VKey;		// 仮想キーコード
} VKeyDef[] = {
    { Qt::Key_unknown,          KVC_UNKNOWN },

    { Qt::Key_1,                KVC_1 },			// 1	!
    { Qt::Key_Exclam,           KVC_1 },			// 1	!
    { Qt::Key_2,                KVC_2 },			// 2	"
    { Qt::Key_QuoteDbl,         KVC_2 },			// 2	"
    { Qt::Key_3,				KVC_3 },			// 3	#
    { Qt::Key_NumberSign,       KVC_3 },			// 3	#
    { Qt::Key_4,				KVC_4 },			// 4	$
    { Qt::Key_Dollar,           KVC_4 },			// 4	$
    { Qt::Key_5,				KVC_5 },			// 5	%
    { Qt::Key_Percent,          KVC_5 },			// 5	%
    { Qt::Key_6,				KVC_6 },			// 6	&
    { Qt::Key_Ampersand,        KVC_6 },			// 6	&
    { Qt::Key_7,				KVC_7 },			// 7	'
    { Qt::Key_Apostrophe,   	KVC_7 },			// 7	'
    { Qt::Key_8,				KVC_8 },			// 8	(
    { Qt::Key_ParenLeft,    	KVC_8 },			// 8	(
    { Qt::Key_9,				KVC_9 },			// 9	)
    { Qt::Key_ParenRight,       KVC_9 },			// 9	)
    { Qt::Key_0,				KVC_0 },			// 0

    { Qt::Key_A,				KVC_A },			// a	A
    { Qt::Key_B,				KVC_B },			// b	B
    { Qt::Key_C,				KVC_C },			// c	C
    { Qt::Key_D,				KVC_D },			// d	D
    { Qt::Key_E,				KVC_E },			// e	E
    { Qt::Key_F,				KVC_F },			// f	F
    { Qt::Key_G,				KVC_G },			// g	G
    { Qt::Key_H,				KVC_H },			// h	H
    { Qt::Key_I,				KVC_I },			// i	I
    { Qt::Key_J,				KVC_J },			// j	J
    { Qt::Key_K,				KVC_K },			// k	K
    { Qt::Key_L,				KVC_L },			// l	L
    { Qt::Key_M,				KVC_M },			// m	M
    { Qt::Key_N,				KVC_N },			// n	N
    { Qt::Key_O,				KVC_O },			// o	O
    { Qt::Key_P,				KVC_P },			// p	P
    { Qt::Key_Q,				KVC_Q },			// q	Q
    { Qt::Key_R,				KVC_R },			// r	R
    { Qt::Key_S,				KVC_S },			// s	S
    { Qt::Key_T,				KVC_T },			// t	T
    { Qt::Key_U,				KVC_U },			// u	U
    { Qt::Key_V,				KVC_V },			// v	V
    { Qt::Key_W,				KVC_W },			// w	W
    { Qt::Key_X,				KVC_X },			// x	X
    { Qt::Key_Y,				KVC_Y },			// y	Y
    { Qt::Key_Z,				KVC_Z },			// z	Z

    { Qt::Key_F1,				KVC_F1 },			// F1
    { Qt::Key_F2,				KVC_F2 },			// F2
    { Qt::Key_F3,				KVC_F3 },			// F3
    { Qt::Key_F4,				KVC_F4 },			// F4
    { Qt::Key_F5,				KVC_F5 },			// F5
    { Qt::Key_F6,				KVC_F6 },			// F6
    { Qt::Key_F7,				KVC_F7 },			// F7
    { Qt::Key_F8,				KVC_F8 },			// F8
    { Qt::Key_F9,				KVC_F9 },			// F9
    { Qt::Key_F10,				KVC_F10 },			// F10
    { Qt::Key_F11,				KVC_F11 },			// F11
    { Qt::Key_F12,				KVC_F12 },			// F12

    { Qt::Key_Minus,			KVC_MINUS },		// -	=
    { Qt::Key_AsciiCircum,		KVC_CARET },		// ^	~
    { Qt::Key_Backspace,		KVC_BACKSPACE },	// BackSpace
    { Qt::Key_At,				KVC_AT },			// @	`
    { Qt::Key_QuoteLeft,		KVC_AT },			// @	`
    { Qt::Key_BracketLeft,		KVC_LBRACKET },		// [	{
    { Qt::Key_BraceLeft,		KVC_LBRACKET },		// [	{
    { Qt::Key_Semicolon,		KVC_SEMICOLON },	// ;	+
    { Qt::Key_Plus,             KVC_SEMICOLON },	// ;	+
    { Qt::Key_Colon,			KVC_COLON },		// :	*
    { Qt::Key_Asterisk,			KVC_COLON },		// :	*
    { Qt::Key_Comma,			KVC_COMMA },		// ,	<
    { Qt::Key_Less, 			KVC_COMMA },		// ,	<
    { Qt::Key_Period,			KVC_PERIOD },		// .	>
    { Qt::Key_Greater, 			KVC_PERIOD },		// .	>
    { Qt::Key_Slash,			KVC_SLASH },		// /	?
    { Qt::Key_Question,			KVC_SLASH },		// /	?
    { Qt::Key_Space,			KVC_SPACE },		// Space

    { Qt::Key_Escape,			KVC_ESC },			// ESC
    { Qt::Key_Zenkaku_Hankaku,	KVC_HANZEN },		// 半角/全角
    { Qt::Key_Tab,				KVC_TAB },			// Tab
    { Qt::Key_CapsLock,		KVC_CAPSLOCK },		// CapsLock
    { Qt::Key_Return,			KVC_ENTER },		// Enter
    { Qt::Key_Control,			KVC_LCTRL },		// L-Ctrl
    //Qtでは右コントロールキーコードは定義されていない
    //{ SDLK_RCTRL,			KVC_RCTRL },		// R-Ctrl
    { Qt::Key_Shift,			KVC_LSHIFT },		// L-Shift
    //Qtでは右シフトキーコードは定義されていない
    //{ SDLK_RSHIFT,			KVC_RSHIFT },		// R-Shift
    { Qt::Key_Alt,			KVC_LALT },			// L-Alt
    //Qtでは右ALTキーコードは定義されていない
    //{ SDLK_RALT,			KVC_RALT },			// R-Alt
    { Qt::Key_Print,			KVC_PRINT },		// PrintScreen
    { Qt::Key_ScrollLock,		KVC_SCROLLLOCK },	// ScrollLock
    { Qt::Key_Pause,			KVC_PAUSE },		// Pause
    { Qt::Key_Insert,			KVC_INSERT },		// Insert
    { Qt::Key_Delete,			KVC_DELETE },		// Delete
    { Qt::Key_End,				KVC_END },			// End
    { Qt::Key_Home,             KVC_HOME },			// Home
    { Qt::Key_PageUp,			KVC_PAGEUP },		// PageUp
    { Qt::Key_PageDown,         KVC_PAGEDOWN },		// PageDown

    { Qt::Key_Up,				KVC_UP },			// ↑
    { Qt::Key_Down,         	KVC_DOWN },			// ↓
    { Qt::Key_Left,         	KVC_LEFT },			// ←
    { Qt::Key_Right,			KVC_RIGHT },		// →

    //Qtではテンキーを識別しないため、除外する
//  { SDLK_KP0,				KVC_P0 },			// [0]
//  { SDLK_KP1,				KVC_P1 },			// [1]
//  { SDLK_KP2,				KVC_P2 },			// [2]
//  { SDLK_KP3,				KVC_P3 },			// [3]
//  { SDLK_KP4,				KVC_P4 },			// [4]
//  { SDLK_KP5,				KVC_P5 },			// [5]
//  { SDLK_KP6,				KVC_P6 },			// [6]
//  { SDLK_KP7,				KVC_P7 },			// [7]
//  { SDLK_KP8,				KVC_P8 },			// [8]
//  { SDLK_KP9,				KVC_P9 },			// [9]
//  { SDLK_NUMLOCK,			KVC_NUMLOCK },		// NumLock
//  { SDLK_KP_PLUS,			KVC_P_PLUS },		// [+]
//  { SDLK_KP_MINUS,		KVC_P_MINUS },		// [-]
//  { SDLK_KP_MULTIPLY,		KVC_P_MULTIPLY },	// [*]
//  { SDLK_KP_DIVIDE,		KVC_P_DIVIDE },		// [/]
//  { SDLK_KP_PERIOD,		KVC_P_PERIOD },		// [.]
//  { SDLK_KP_ENTER,		KVC_P_ENTER },		// [Enter]

    // 日本語キーボードのみ
    { Qt::Key_Backslash,    KVC_YEN },			// ¥	|
    { Qt::Key_BracketRight,	KVC_RBRACKET },		// ]	}
    { Qt::Key_BraceRight,	KVC_RBRACKET },		// ]	}
    { Qt::Key_Underscore,	KVC_UNDERSCORE },	// ¥	_
//	{ 				,		KVC_MUHENKAN },		// 無変換
//	{ 				,		KVC_HENKAN },		// 変換
//	{ 				,		KVC_HIRAGANA },		// ひらがな

    // 追加キー
    { Qt::Key_Meta,			KVX_LMETA },		// L-Meta
    //Qtでは右Metaキーコードは定義されていない
//    { SDLK_RMETA,			KVX_RMETA },		// R-Meta
    { Qt::Key_Menu,			KVX_MENU }			// Menu
};


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
#ifndef NOJOYSTICK
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) )
        return false;
#endif

    //経過時間タイマーをスタート
    elapsedTimer.start();

    // Qtキーコード  -> 仮想キーコード 変換テーブル初期化
    for( int i=0; i < COUNTOF(VKeyDef); i++ )
        VKTable[VKeyDef[i].InKey] = VKeyDef[i].VKey;

    return true;
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

#ifndef NOJOYSTICK
    SDL_Quit();
#endif
}

////////////////////////////////////////////////////////////////
// キーリピート設定
//
// 引数:	repeat	キーリピートの間隔(ms) 0で無効
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_SetKeyRepeat( int repeat )
{
    QApplication::setKeyboardInputInterval(repeat);
}

////////////////////////////////////////////////////////////////
// OSDキーコード -> 仮想キーコード変換
//
// 引数:	scode		環境依存のキーコード
// 返値:	PCKEYsym	仮想キーコード
////////////////////////////////////////////////////////////////
PCKEYsym OSD_ConvertKeyCode( int scode )
{
    if(VKTable.count(scode) == 0){
        qDebug("keycode %x unknown\n", scode);
        return KVC_UNKNOWN;
    }
    return VKTable[scode];
}

////////////////////////////////////////////////////////////////
// 指定時間待機
//
// 引数:	tms			待機時間(ms)
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_Delay( DWORD tms )
{
    QThread::msleep(tms);
}


////////////////////////////////////////////////////////////////
// プロセス開始からの経過時間取得
//
// 引数:	なし
// 返値:	DWORD		経過時間(ms)
////////////////////////////////////////////////////////////////
DWORD OSD_GetTicks( void )
{
   return elapsedTimer.elapsed();
}


////////////////////////////////////////////////////////////////
// イベント取得(イベントが発生するまで待つ)
//
// 引数:	ev			イベント情報共用体へのポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_GetEvent( Event *ev )
{
    eventMutex.lock();
    //イベントキューが空の場合、イベントが発行されるまで待つ
    if(eventQueue.empty()){
        eventEmitted.wait(&eventMutex);
    }
    *ev = eventQueue.front();
    eventQueue.pop_front();
    eventMutex.unlock();

    return true;
}
////////////////////////////////////////////////////////////////
// イベントをキューにプッシュする
//
// 引数:	ev			イベントタイプ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_PushEvent(EventType ev)
{
    Event event;
    event.type = ev;
    return OSD_PushEvent(event);
}

////////////////////////////////////////////////////////////////
// イベントをキューにプッシュする
//
// 引数:	ev			イベント
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_PushEvent(const Event& ev)
{
    //イベントキューにプッシュ
    eventMutex.lock();
    eventQueue.push_back(ev);
    eventEmitted.wakeAll();
    eventMutex.unlock();
    return true;
}



////////////////////////////////////////////////////////////////
// マウスカーソル表示/非表示
//
// 引数:	disp		true:表示 false:非表示
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_ShowCursor( bool disp )
{
    qApp->setOverrideCursor(disp ? Qt::ArrowCursor : Qt::BlankCursor);
}


////////////////////////////////////////////////////////////////
// キャプション設定
//
// 引数:	Wh			ウィンドウハンドル
//			str			キャプション文字列へのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_SetWindowCaption( HWINDOW Wh, const char *str )
{
    QGraphicsView* view = static_cast<QGraphicsView*>(Wh);
    Q_ASSERT(view);
    QMetaObject::invokeMethod(view, "setWindowTitle",
                              Q_ARG(QString, str));
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
    static QGraphicsScene* scene = new QGraphicsScene();
    static RenderView* view = new RenderView(scene);
    scene->setSceneRect(0, 0, w, h);

    //アプリケーション終了前にインスタンスを削除(単なる親子関係にすると終了時にクラッシュする)
    QObject::connect(qApp, SIGNAL(aboutToQuit()), scene, SLOT(deleteLater()));

    view->moveToThread(qApp->thread());
    *pwh = view;

    QMetaObject::invokeMethod(qApp, "createWindow",
                              Q_ARG(HWINDOW, *pwh),
                              Q_ARG(int, bpp),
                              Q_ARG(bool, fsflag));

    return true;
}


////////////////////////////////////////////////////////////////
// ウィンドウ破棄
//
// 引数:	Wh			ウィンドウハンドル
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_DestroyWindow( HWINDOW Wh )
{
    // 何もしない
}


////////////////////////////////////////////////////////////////
// ウィンドウの幅を取得
//
// 引数:	Wh			ウィンドウハンドル
// 返値:	int			幅
////////////////////////////////////////////////////////////////
int OSD_GetWindowWidth( HWINDOW Wh )
{
    //QtではSceneRectの幅を返す
    QGraphicsView* view = static_cast<QGraphicsView*>(Wh);
    Q_ASSERT(view);

    return view->scene()->width();
}


////////////////////////////////////////////////////////////////
// ウィンドウの高さを取得
//
// 引数:	Wh			ウィンドウハンドル
// 返値:	int			高さ
////////////////////////////////////////////////////////////////
int OSD_GetWindowHeight( HWINDOW Wh )
{
    //QtではSceneRectの幅を返す
    QGraphicsView* view = static_cast<QGraphicsView*>(Wh);
    Q_ASSERT(view);

    return view->scene()->height();
}


////////////////////////////////////////////////////////////////
// ウィンドウの色深度を取得
//
// 引数:	Wh			ウィンドウハンドル
// 返値:	int			色深度
////////////////////////////////////////////////////////////////
int OSD_GetWindowBPP( HWINDOW Wh )
{
    //内部カラーは8ビット固定
    return 8;
}


////////////////////////////////////////////////////////////////
// パレット設定
//
// 引数:	Wh			ウィンドウハンドル
//			pal			パレットへのポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_SetPalette( HWINDOW Wh, VPalette *pal )
{
    PaletteTable.clear();
    for (int i=0; i < pal->ncols; i++){
        COLOR24& col = pal->colors[i];
        PaletteTable.push_back(qRgb(col.r, col.g, col.b));
    }
    return true;
}

////////////////////////////////////////////////////////////////
// ウィンドウ反映
//
// 引数:	Wh			ウィンドウハンドル
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_RenderWindow( HWINDOW Wh )
{
    //何もしない
}


////////////////////////////////////////////////////////////////
// ウィンドウクリア
//  色は0(黒)で決め打ち
//
// 引数:	Wh			ウィンドウハンドル
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_ClearWindow( HWINDOW Wh )
{
    QMetaObject::invokeMethod(qApp, "clearLayout",
                                  Q_ARG(HWINDOW, Wh));
}


////////////////////////////////////////////////////////////////
// ウィンドウに転送
//
// 引数:	Wh			ウィンドウハンドル
//			src			転送元サーフェス
//			pal			パレットへのポインタ
//			x,y			転送先座標
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_BlitToWindow( HWINDOW Wh, VSurface *src, int x, int y, VPalette *pal )
{
    VRect src1,drc1;

    QImage image(src->Width(), src->Height(), QImage::Format_Indexed8);
    image.setColorTable(PaletteTable);

    // 転送元範囲設定
    src1.x = 0;
    src1.y = 0;
    src1.w = src->Width();
    src1.h = src->Height();

    if( src1.w <= 0 || src1.h <= 0 ) return;

    // 転送先範囲設定
    drc1.x = 0;
    drc1.y = 0;

    BYTE *psrc = (BYTE *)src->GetPixels() + src->Pitch() * src1.y + src1.x * src->Bpp() / 8;

    for( int i=0; i < src1.h; i++ ){
        BYTE *pdst = (BYTE *)image.scanLine(i);
        memcpy( pdst, psrc, image.bytesPerLine() );
        psrc += src->Pitch();
    }

    // 表示用のQPixmapItemへの変換はメインスレッドでないとできないため、
    // スロットを呼び出してメインスレッドでSceneを更新する
    // (直接呼び出すと呼び出し側スレッドで実行されてしまう)
    QMetaObject::invokeMethod(qApp, "layoutBitmap",
                              Q_ARG(HWINDOW, Wh),
                              Q_ARG(int, x),
                              Q_ARG(int, y),
                              Q_ARG(double, src->GetAspectRatio()),
                              Q_ARG(QImage, image));
}

////////////////////////////////////////////////////////////////
// パスのデリミタを'/'に変換
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void Delimiter( char *path )
{
    QString strPath = QDir::fromNativeSeparators(QString::fromUtf8(path));
    strcpy(path, strPath.toUtf8().data());
}


////////////////////////////////////////////////////////////////
// パスのデリミタを'\'に変換
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void UnDelimiter( char *path )
{
    QString strPath = QDir::toNativeSeparators(QString::fromUtf8(path));
    strcpy(path, strPath.toUtf8().data());
}


////////////////////////////////////////////////////////////////
// パスの末尾にデリミタを追加
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void AddDelimiter( char *path )
{
    if( !strlen( path ) )
        strncpy( path, "/", PATH_MAX );
    else if( (path[strlen(path)-1] != '/' ) && ( path[strlen(path)-1] != '\\') )
        strncat( path, "/", PATH_MAX );
}


////////////////////////////////////////////////////////////////
// パスの末尾のデリミタを削除
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void DelDelimiter( char *path )
{
    if( strlen( path ) > 1 )
        while( ( path[strlen(path)-1] == '/' ) || ( path[strlen(path)-1] == '\\' ) )
            path[strlen(path)-1] = '\0';
}

////////////////////////////////////////////////////////////////
// 設定及び諸ファイル(ROMなどの初期位置)フォルダ作成
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
bool OSD_CreateConfigPath()
{
    QString basePath = QString::fromUtf8(OSD_GetConfigPath());

    // ROMイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(ROM_DIR)))
        return false;

    // TAPEイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(TAPE_DIR)))
        return false;

    // DISKイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(DISK_DIR)))
        return false;

    // 拡張ROMイメージ
    if(!QDir(basePath).mkpath(QString::fromUtf8(EXTROM_DIR)))
        return false;

    // スナップショット
    if(!QDir(basePath).mkpath(QString::fromUtf8(IMAGE_DIR)))
        return false;

    // WAVEファイル
    if(!QDir(basePath).mkpath(QString::fromUtf8(WAVE_DIR)))
        return false;

    return true;
}

////////////////////////////////////////////////////////////////
// 設定及び諸ファイル(ROMなどの初期位置)パス取得
//
// 引数:	なし
// 返値:	char *		取得した文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetConfigPath( void )
{
    PRINTD( OSD_LOG, "[OSD][OSD_GetModulePath]\n" );
    static char mpath[PATH_MAX] = "";	// モジュールパス取得用

    //Windowsの場合はexe本体と同じ場所。
    //それ以外(UNIX系を想定)は ~/.pc6001vx を返す
#ifdef WIN32
    QString confPath = qApp->applicationDirPath() + QDir::separator();
#else
    QString confPath = QDir::homePath() + QDir::separator() + QString(".pc6001vx");
#endif
    sprintf( mpath, "%s", confPath.toUtf8().data() );
    AddDelimiter( mpath );	// 念のため
    UnDelimiter( mpath );
    return mpath;
}


////////////////////////////////////////////////////////////////
// フルパスからファイル名を取得
//
// 引数:	path		フルパス格納バッファポインタ
// 返値:	char *		ファイル名の開始ポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetFileNamePart( const char *path )
{
    PRINTD( OSD_LOG, "[OSD][OSD_GetFileNamePart]\n" );

    QFileInfo info(QString::fromUtf8(path));
    return info.fileName().toUtf8().data();
}


////////////////////////////////////////////////////////////////
// ファイルの存在チェック
//
// 引数:	path		フルパス格納バッファポインタ
// 返値:	bool		true:存在する false:存在しない
////////////////////////////////////////////////////////////////
bool OSD_FileExist( const char *fullpath )
{
    PRINTD( OSD_LOG, "[OSD][OSD_FileExist]\n" );

    QString pathString = QString::fromUtf8(fullpath);

    //ワイルドカードを含む場合
    if (pathString.contains("*")){
        QFileInfo info(pathString);

        QDir dir = info.absoluteDir();
        QFile file(pathString);
        QString wildcard = info.fileName();

        QFileInfoList list = dir.entryInfoList(QStringList(wildcard), QDir::Files);
        return !list.empty();
    } else {
        QFile file(pathString);
        return file.exists();
    }
}


////////////////////////////////////////////////////////////////
// ファイルの読取り専用チェック
//
// 引数:	path		フルパス格納バッファポインタ
// 返値:	bool		true:読取り専用 false:読み書き
///////////////////////////////////////////////////////////////
bool OSD_FileReadOnly( const char *fullpath )
{
    PRINTD( OSD_LOG, "[OSD][OSD_FileReadOnly]\n" );

    QFileInfo info(QString::fromUtf8(fullpath));
    return !info.isWritable();
}


///////////////////////////////////////////////////////////
// フォルダの参照
//
// 引数:	hwnd		親のウィンドウハンドル
//			Result		フォルダへのパス格納バッファポインタ
// 返値:	char *		取得した文字列へのポインタ
///////////////////////////////////////////////////////////
const char *OSD_FolderDiaog( void *hwnd, char *Result )
{
    QByteArray result = QFileDialog::getExistingDirectory(NULL, "フォルダを選択してください。", QDir::homePath()).toUtf8();
    strcpy(Result, result);
    return result.data();
}


///////////////////////////////////////////////////////////
// ファイルの参照
//
// 引数:	hwnd		親のウィンドウハンドル
//			mode		モード FM_Load:ファイルを開く FM_Save:名前を付けて保存
//			title		ウィンドウキャプション文字列へのポインタ
//			filter		ファイルフィルタ文字列へのポインタ
//			fullpath	フルパス格納バッファポインタ
//			path		ファイル検索パス格納バッファポインタ
//			ext			拡張子文字列へのポインタ
// 返値:	char *		取得したファイル名へのポインタ(フルパスではない)
///////////////////////////////////////////////////////////
const char *OSD_FileDiaog( void *hwnd, FileMode mode, const char *title, const char *filter, char *fullpath, char *path, const char *ext )
{
    QString result;
    //検索パスが指定されていない場合はホームフォルダとする
    QString pathStr = strlen(path) ? path : QDir::homePath();
    if(mode == FM_Save){
        result = QFileDialog::getSaveFileName(NULL, title, pathStr, filter);
    } else {
        result = QFileDialog::getOpenFileName(NULL, title, pathStr, filter);
    }
    if(result.isEmpty())    return NULL;

    QDir dir(result);
    if( path ) strcpy( path, dir.path().toUtf8().data() );
    if( fullpath ) strcpy( fullpath, result.toUtf8().data() );

    QFile file(result);
    return file.fileName().toUtf8().data();
}


////////////////////////////////////////////////////////////////
// 各種ファイル選択
//
// 引数:	hwnd		親のウィンドウハンドル(NULLの場合はスクリーンサーフェスのハンドル取得を試みる)
//			type		ダイアログの種類(FileDlg参照)
//			fullpath	フルパス格納バッファポインタ
//			path		ファイル検索パス格納バッファポインタ
// 返値:	char *		取得したファイル名へのポインタ(フルパスではない)
////////////////////////////////////////////////////////////////
const char *OSD_FileSelect( void *hwnd, FileDlg type, char *fullpath, char *path )
{
    FileMode mode = FM_Load;
    const char *title   = NULL;
    const char *filter  = NULL;
    const char *ext     = NULL;

    switch( type ){
    case FD_TapeLoad:	// TAPE(LOAD)選択
        mode   = FM_Load;
        title  = "TAPEイメージ選択(LOAD)";
        filter = "TAPEイメージ (*.p6t *.cas *.p6);;"
                "P6T形式 (*.p6t);;"
                "ベタ形式 (*.cas *.p6);;"
                "全てのファイル (*.*)";
        ext    = "p6t";
        break;

    case FD_TapeSave:	// TAPE(SAVE)選択
        mode   = FM_Save;
        title  = "TAPEイメージ選択(SAVE)";
        filter = "TAPEイメージ (*.p6t *.cas *.p6);;"
                "P6T形式 (*.p6t);;"
                "ベタ形式 (*.cas *.p6);;"
                "全てのファイル (*.*)";
        ext    = "p6t";
        break;

    case FD_Disk:		// DISK選択
        mode   = FM_Load;
        title  = "DISKイメージ選択";
        filter = "DISKイメージ(*.d88);;"
                "D88形式 (*.d88);;"
                "全てのファイル (*.*)";
        ext    = "d88";
        break;

    case FD_ExtRom:		// 拡張ROM選択
        mode   = FM_Load;
        title  = "拡張ROMイメージ選択";
        filter = "全てのファイル (*.*)";
        break;

    case FD_Printer:	// プリンター出力ファイル選択
        mode   = FM_Save;
        title  = "プリンター出力ファイル選択";
        filter = "全てのファイル (*.*)";
        ext    = "txt";
        break;

    case FD_FontZ:		// 全角フォントファイル選択
        mode   = FM_Load;
        title  = "全角フォントファイル選択";
        filter = "PNGファイル (*.png);;"
                "全てのファイル (*.*)";
        ext    = "png";
        break;

    case FD_FontH:		// 半角フォントファイル選択
        mode   = FM_Load;
        title  = "半角フォントファイル選択";
        filter = "PNGファイル (*.png);;"
                "全てのファイル (*.*)";
        ext    = "png";
        break;

    case FD_DokoLoad:	// どこでもLOADファイル選択
        mode   = FM_Load;
        title  = "どこでもLOADファイル選択";
        filter = "どこでもSAVEファイル(*.dds *.ddr);;"
                "どこでもSAVE形式 (*.dds);;"
                "リプレイファイル (*.ddr);;"
                "全てのファイル (*.*)";
        ext    = "dds";
        break;

    case FD_DokoSave:	// どこでもSAVEファイル選択
        mode   = FM_Save;
        title  = "どこでもSAVEファイル選択";
        filter = "どこでもSAVE形式 (*.dds);;"
                "全てのファイル (*.*)";
        ext    = "dds";
        break;

    case FD_RepLoad:	// リプレイ再生ファイル選択
        mode   = FM_Load;
        title  = "リプレイ再生ファイル選択";
        filter = "リプレイファイル (*.ddr);;"
                "全てのファイル (*.*)";
        ext    = "ddr";
        break;

    case FD_RepSave:	// リプレイ保存ファイル選択
        mode   = FM_Save;
        title  = "リプレイ保存ファイル選択";
        filter = "リプレイファイル (*.ddr);;"
                "全てのファイル (*.*)";
        ext    = "ddr";
        break;

    case FD_AVISave:	// ビデオキャプチャ出力ファイル選択
        mode   = FM_Save;
        title  = "ビデオキャプチャ出力ファイル選択";
        filter = "AVIファイル (*.avi);;"
                "全てのファイル (*.*)";
        ext    = "avi";
        break;

    case FD_LoadAll:	// 汎用LOAD
    default:
        mode   = FM_Load;
        title  = "ファイル選択";
        filter = "全てのファイル (*.*)";
        break;
    }

    return OSD_FileDiaog( NULL, mode, title, filter, fullpath, path, ext );
}


////////////////////////////////////////////////////////////////
// メッセージ表示
//
// 引数:	mes			メッセージ文字列へのポインタ
//			cap			ウィンドウキャプション文字列へのポインタ
//			type		表示形式指示のフラグ
// 返値:	int			押されたボタンの種類
//							OSDR_OK:     OKボタン
//							OSDR_CANCEL: CANCELボタン
//							OSDR_YES:    YESボタン
//							OSDR_NO:     NOボタン
////////////////////////////////////////////////////////////////
int OSD_Message( const char *mes, const char *cap, int type )
{
    QMessageBox::StandardButtons Type = QMessageBox::Ok;
    QMessageBox::Icon IconType = QMessageBox::Information;

    // メッセージボックスのタイプ
    switch( type&0x000f ){
    case OSDM_OK:		Type = QMessageBox::Ok;                         break;
    case OSDM_OKCANCEL:		Type = QMessageBox::Ok | QMessageBox::Cancel;	break;
    case OSDM_YESNO:		Type = QMessageBox::Yes | QMessageBox::No;	break;
    case OSDM_YESNOCANCEL:	Type = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;    break;
    }

    // メッセージボックスのアイコンタイプ
    switch( type&0x00f0 ){
    case OSDM_ICONERROR:	IconType = QMessageBox::Critical;	break;
    case OSDM_ICONQUESTION:	IconType = QMessageBox::Question;	break;
    case OSDM_ICONWARNING:	IconType = QMessageBox::Warning;	break;
    case OSDM_ICONINFO:		IconType = QMessageBox::Information;	break;
    }

    QMessageBox mb(IconType, cap, mes, Type);
    int res = mb.exec();

    switch( res ){
    case QMessageBox::Ok:	return OSDR_OK;
    case QMessageBox::Yes:	return OSDR_YES;
    case QMessageBox::No:	return OSDR_NO;
    default:	return OSDR_CANCEL;
    }
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
    QAudioFormat format;
    format.setCodec("audio/pcm");
    format.setChannelCount(1);
    format.setSampleRate(rate);
    format.setSampleSize(16);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    if(audioOutput){
        audioOutput->deleteLater();
    }

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning()<<"raw audio format not supported by backend, cannot play audio.";
        return false;
    }

    audioOutput = new QAudioOutput(info, format, qApp);
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
    if(audioOutput){
        audioOutput->stop();
    }
}


////////////////////////////////////////////////////////////////
// 再生開始
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_StartAudio( void )
{
    if(audioOutput){
        audioBuffer = audioOutput->start();
        //#PENDING これではグローバルボリュームを変えてしまう？
        audioOutput->setVolume(0.3);
    }
}


////////////////////////////////////////////////////////////////
// 再生停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_StopAudio( void )
{
    if(audioOutput){
        audioOutput->suspend();
    }
}

////////////////////////////////////////////////////////////////
// オーディオストリーム書き込み
//
// 引数:	stream  書き込むデータへのポインタ
//      samples 書き込むバイト数
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_WriteAudioStream(BYTE *stream, int samples)
{
    if(audioBuffer){
        audioBuffer->write((const char*)stream, samples);
    }
}


////////////////////////////////////////////////////////////////
// 再生状態取得
//
// 引数:	なし
// 返値:	bool		true:再生中 false:停止中
////////////////////////////////////////////////////////////////
bool OSD_AudioPlaying( void )
{
    return audioOutput->state() == QAudio::ActiveState;
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
    //#PENDING
//    SDL_AudioSpec ws;

//    if( !SDL_LoadWAV( filepath, &ws, buf, (Uint32 *)len ) ) return false;

//    if( ws.freq < 22050 || ws.format != AUDIO_S16 || ws.channels != 1 ){
//        SDL_FreeWAV( *buf );
//        return false;
//    }

//    *freq    = ws.freq;

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
    //#PENDING SDL_FreeWAV( buf );
}



////////////////////////////////////////////////////////////////
// オーディオをロックする
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_LockAudio( void )
{
    // 何もしない
}


////////////////////////////////////////////////////////////////
// オーディオをアンロックする
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_UnlockAudio( void )
{
    // 何もしない
}


////////////////////////////////////////////////////////////////
// フォントファイル作成
//
// 引数:	hfile		半角フォントファイルパス
//			zfile		全角フォントファイルパス
//			size		文字サイズ(半角文字幅ピクセル数)
// 返値:	bool		true:作成成功 false:作成失敗
////////////////////////////////////////////////////////////////
bool OSD_CreateFont( char *hfile, char *zfile, int size )
{
    // 何もしない
    // フォントファイルはQtのリソースから読み込む
    return true;
}



///////////////////////////////////////////////////////////
// アイコン設定
//
// 引数:	Wh			ウィンドウハンドル
//			model		機種 60,62,66
// 返値:	なし
///////////////////////////////////////////////////////////
void OSD_SetIcon( HWINDOW Wh, int model )
{
    // 機種別P6オブジェクト確保
    const char* iconRes = NULL;
    switch( model ){
    case 62: iconRes = ":/res/PC-6001mk2.ico"; break;
    case 66: iconRes = ":/res/PC-6601.ico"; break;
    default: iconRes = ":/res/PC-6001.ico";
    }

    // アイコン設定
    QImage icon = QImage(iconRes).convertToFormat(QImage::Format_RGB16);
    QApplication::setWindowIcon(QIcon(iconRes));
}


#ifndef NOJOYSTICK
std::map<int, HJOYINFO> joyMap;
#endif

////////////////////////////////////////////////////////////////
// 利用可能なジョイスティック数取得
//
// 引数:	なし
// 返値:	int			利用可能なジョイスティック数
////////////////////////////////////////////////////////////////
int OSD_GetJoyNum( void )
{
#ifndef NOJOYSTICK
    return SDL_NumJoysticks();
#else
    return 0;
#endif
}


////////////////////////////////////////////////////////////////
// ジョイスティック名取得
//
// 引数:	int			インデックス
// 返値:	char *		ジョイスティック名文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetJoyName( int index )
{
#ifndef NOJOYSTICK
    return SDL_JoystickNameForIndex( index );
#else
    return "";
#endif
}


////////////////////////////////////////////////////////////////
// ジョイスティックオープンされてる？
//
// 引数:	int			インデックス
// 返値:	bool		true:OPEN false:CLOSE
////////////////////////////////////////////////////////////////
bool OSD_OpenedJoy( int index )
{
#ifndef NOJOYSTICK
    return SDL_JoystickGetAttached( (SDL_Joystick*)joyMap[index] ) ? true : false;
#else
    return false;
#endif
}


////////////////////////////////////////////////////////////////
// ジョイスティックオープン
//
// 引数:	int			インデックス
// 返値:	HJOYINFO	ジョイスティック情報へのポインタ
////////////////////////////////////////////////////////////////
HJOYINFO OSD_OpenJoy( int index )
{
#ifndef NOJOYSTICK
    joyMap[index] = (HJOYINFO)SDL_JoystickOpen( index );
    return joyMap[index];
#else
    return (HJOYINFO)NULL;
#endif
}


////////////////////////////////////////////////////////////////
// ジョイスティッククローズ
//
// 引数:	int			インデックス
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_CloseJoy( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
    SDL_JoystickClose( (SDL_Joystick *)jinfo );
#else
#endif
}

////////////////////////////////////////////////////////////////
// ジョイスティックの軸の数取得
//
// 引数:	HJOYINFO	ジョイスティック情報へのポインタ
// 返値:	int			軸の数
////////////////////////////////////////////////////////////////
int OSD_GetJoyNumAxes( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
    return SDL_JoystickNumAxes( (SDL_Joystick *)jinfo );
#else
    return 1;
#endif
}


////////////////////////////////////////////////////////////////
// ジョイスティックのボタンの数取得
//
// 引数:	HJOYINFO	ジョイスティック情報へのポインタ
// 返値:	int			ボタンの数
////////////////////////////////////////////////////////////////
int OSD_GetJoyNumButtons( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
    return SDL_JoystickNumButtons( (SDL_Joystick *)jinfo );
#else
    return 0;
#endif
}

////////////////////////////////////////////////////////////////
// ジョイスティックの状態を更新
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_UpdateJoy()
{
#ifndef NOJOYSTICK
    SDL_JoystickUpdate();
#else
#endif
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
#ifndef NOJOYSTICK
    if(num == 0){
        // ジョイスティックの左右に対応して画面を傾ける
        int Xmove = SDL_JoystickGetAxis( (SDL_Joystick *)jinfo, num );
        if( Xmove < INT16_MIN / 2 ){  // 左
            TiltScreen(LEFT);
        } else if( Xmove > INT16_MAX / 2 ){  // 右
            TiltScreen(RIGHT);
        } else {
            TiltScreen(NEWTRAL);
        }
    }
    return SDL_JoystickGetAxis( (SDL_Joystick *)jinfo, num );
#else
    return 0;
#endif
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
#ifndef NOJOYSTICK
    return SDL_JoystickGetButton( (SDL_Joystick *)jinfo, num ) ? true : false;
#else
    return false;
#endif
}
