// OS依存の汎用ルーチン(主にUI用)
#include "../typedef.h"

#include <QtCore>
#include <QtWidgets>
#ifndef NOSOUND
#include <QtMultimedia>
#endif

#ifndef NOJOYSTICK
#include <SDL2/SDL.h>
#endif

#include "../log.h"
#include "../osd.h"
#include "../common.h"
#include "../pc6001v.h"
#include "../joystick.h"
#include "../config.h"
#include "../error.h"
#include "configdialog.h"
#include "aboutdialog.h"

#include "renderview.h"

///////////////////////////////////////////////////////////
// 仕方なしにスタティック変数
///////////////////////////////////////////////////////////
static CFG6 *ccfg = NULL;				// 環境設定オブジェクトポインタ
static CFG6 *ecfg = NULL;				// 環境設定オブジェクトポインタ(編集用)

//エミュレータ内部用イベントキュー
QMutex eventMutex;
QQueue<Event> eventQueue;
QWaitCondition eventEmitted;

//経過時間タイマ
QElapsedTimer elapsedTimer;

std::map<int, PCKEYsym> VKTable;			// Qtキーコード  -> 仮想キーコード 変換テーブル
QVector<QRgb> PaletteTable;              //パレットテーブル

#ifndef NOSOUND
//サウンド関連
QPointer<QIODevice> audioBuffer = NULL;
QPointer<QAudioOutput> audioOutput = NULL;
#endif

//ジョイスティック関連
#ifndef NOJOYSTICK
QMutex joystickMutex;
#endif

static const struct {	// Qtキーコード -> 仮想キーコード定義
    int InKey;			// Qtのキーコード
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
    { Qt::Key_Equal,			KVC_MINUS },		// -	=
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

    #ifdef PANDORA //PANDORAではAボタン(HOME)をSPACEに、Xボタン(PAGEDOWN)をSHIFTに割り当てる
    { Qt::Key_Home,             KVC_SPACE },			// Home
    { Qt::Key_PageUp,			KVC_PAGEUP },		// PageUp
    { Qt::Key_PageDown,         KVC_LSHIFT },		// PageDown
    #else
    { Qt::Key_Home,             KVC_HOME },			// Home
    { Qt::Key_PageUp,			KVC_PAGEUP },		// PageUp
    { Qt::Key_PageDown,         KVC_PAGEDOWN },		// PageDown
    #endif

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
    { Qt::Key_Muhenkan,     KVC_MUHENKAN },		// 無変換
    { Qt::Key_Henkan,       KVC_HENKAN },		// 変換
    { Qt::Key_Hiragana_Katakana,	KVC_HIRAGANA },		// ひらがな

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
    QT_TRANSLATE_NOOP("PC6001VX", "終了してよろしいですか?"),
    QT_TRANSLATE_NOOP("PC6001VX", "終了確認"),
    QT_TRANSLATE_NOOP("PC6001VX", "再起動してよろしいですか?"),
    QT_TRANSLATE_NOOP("PC6001VX", "変更は再起動後に有効となります。\n今すぐ再起動しますか?"),
    QT_TRANSLATE_NOOP("PC6001VX", "再起動確認"),
    QT_TRANSLATE_NOOP("PC6001VX", "拡張ROMを挿入してリセットします。"),
    QT_TRANSLATE_NOOP("PC6001VX", "拡張ROMを排出してリセットします。"),
    QT_TRANSLATE_NOOP("PC6001VX", "リセット確認")
};

// メニュー用メッセージ ------
const char *MsgMen[] = {
    QT_TRANSLATE_NOOP("PC6001VX", "ビデオキャプチャ..."),
    QT_TRANSLATE_NOOP("PC6001VX", "ビデオキャプチャ停止"),
    QT_TRANSLATE_NOOP("PC6001VX", "記録..."),	// (リプレイ)
    QT_TRANSLATE_NOOP("PC6001VX", "記録停止"),	// (リプレイ)
    QT_TRANSLATE_NOOP("PC6001VX", "再生..."),	// (リプレイ)
    QT_TRANSLATE_NOOP("PC6001VX", "再生停止")	// (リプレイ)
};

// INIファイル用メッセージ ------
const char *MsgIni[] = {
    // [CONFIG]
    QT_TRANSLATE_NOOP("PC6001VX", "; === PC6001V 初期設定ファイル ===\n\n"),
    QT_TRANSLATE_NOOP("PC6001VX", "機種 60:PC-6001 61:PC-6001A 62:PC-6001mk2 66:PC-6601 64:PC-6001mk2SR 68:PC-6601SR"),
    //QT_TRANSLATE_NOOP("PC6001VX", "機種 60:PC-6001 61:PC-6001A 62:PC-6001mk2 66:PC-6601"),
    QT_TRANSLATE_NOOP("PC6001VX", "FDD接続台数 (0-2)"),
    QT_TRANSLATE_NOOP("PC6001VX", "拡張RAM使用"),
    QT_TRANSLATE_NOOP("PC6001VX", "Turbo TAPE Yes:有効 No:無効"),
    QT_TRANSLATE_NOOP("PC6001VX", "BoostUp Yes:有効 No:無効"),
    QT_TRANSLATE_NOOP("PC6001VX", "BoostUp 最大倍率(N60モード)"),
    QT_TRANSLATE_NOOP("PC6001VX", "BoostUp 最大倍率(N60m/N66モード)"),
    QT_TRANSLATE_NOOP("PC6001VX", "オーバークロック率 (1-1000)%"),
    QT_TRANSLATE_NOOP("PC6001VX", "CRCチェック Yes:有効 No:無効"),
    QT_TRANSLATE_NOOP("PC6001VX", "ROMパッチ Yes:あてる No:あてない"),
    // [DISPLAY]
    QT_TRANSLATE_NOOP("PC6001VX", "MODE4カラーモード 0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク"),
    QT_TRANSLATE_NOOP("PC6001VX", "スキャンライン Yes:あり No:なし"),
    QT_TRANSLATE_NOOP("PC6001VX", "スキャンライン輝度 (0-100)%"),
    QT_TRANSLATE_NOOP("PC6001VX", "4:3表示 Yes:有効 No:無効"),
    QT_TRANSLATE_NOOP("PC6001VX", "フルスクリーンモード Yes:有効 No:無効"),
    QT_TRANSLATE_NOOP("PC6001VX", "ステータスバー Yes:表示 No:非表示"),
    QT_TRANSLATE_NOOP("PC6001VX", "フレームスキップ"),
    // [SOUND]
    QT_TRANSLATE_NOOP("PC6001VX", "サンプリングレート (44100/22050/11025)Hz"),
    QT_TRANSLATE_NOOP("PC6001VX", "サウンドバッファサイズ"),
    QT_TRANSLATE_NOOP("PC6001VX", "マスター音量 (0-100)"),
    QT_TRANSLATE_NOOP("PC6001VX", "PSG音量 (0-100)"),
    QT_TRANSLATE_NOOP("PC6001VX", "音声合成音量 (0-100)"),
    QT_TRANSLATE_NOOP("PC6001VX", "TAPEモニタ音量 (0-100)"),
    QT_TRANSLATE_NOOP("PC6001VX", "PSG LPFカットオフ周波数(0で無効)"),
    // [MOVIE]
    QT_TRANSLATE_NOOP("PC6001VX", "ビデオキャプチャ色深度 (16,24,32)"),
    // [FILES]
    QT_TRANSLATE_NOOP("PC6001VX", "拡張ROMファイル名"),
    QT_TRANSLATE_NOOP("PC6001VX", "TAPE(LODE)ファイル名(起動時に自動マウント)"),
    QT_TRANSLATE_NOOP("PC6001VX", "TAPE(SAVE)ファイル名(SAVE時に自動マウント)"),
    QT_TRANSLATE_NOOP("PC6001VX", "DISK1ファイル名(起動時に自動マウント)"),
    QT_TRANSLATE_NOOP("PC6001VX", "DISK2ファイル名(起動時に自動マウント)"),
    QT_TRANSLATE_NOOP("PC6001VX", "プリンタ出力ファイル名"),
    QT_TRANSLATE_NOOP("PC6001VX", "全角フォントファイル名"),
    QT_TRANSLATE_NOOP("PC6001VX", "半角フォントファイル名"),
    // [PATH]
    QT_TRANSLATE_NOOP("PC6001VX", "ROMイメージ格納パス"),
    QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージ格納パス"),
    QT_TRANSLATE_NOOP("PC6001VX", "DISKイメージ格納パス"),
    QT_TRANSLATE_NOOP("PC6001VX", "拡張ROMイメージ格納パス"),
    QT_TRANSLATE_NOOP("PC6001VX", "スナップショット格納パス"),
    QT_TRANSLATE_NOOP("PC6001VX", "WAVEファイル格納パス"),
    QT_TRANSLATE_NOOP("PC6001VX", "FONT格納パス"),
    QT_TRANSLATE_NOOP("PC6001VX", "どこでもSAVE格納パス"),
    // [CHECK]
    QT_TRANSLATE_NOOP("PC6001VX", "終了時確認 Yes:する No:しない"),
    QT_TRANSLATE_NOOP("PC6001VX", "終了時INIファイルを保存 Yes:する No:しない"),
    // [KEY]
    QT_TRANSLATE_NOOP("PC6001VX", "キーリピートの間隔(単位:ms 0で無効)"),
    QT_TRANSLATE_NOOP("PC6001VX", "PS/2キーボード使う? Yes:PS/2 No:USB"),
    // [OPTION]
    QT_TRANSLATE_NOOP("PC6001VX", "戦士のカートリッジ Yes:有効 No:無効")

};

// どこでもSAVE用メッセージ ------
const char *MsgDok[] = {
    QT_TRANSLATE_NOOP("PC6001VX", "; === PC6001V どこでもSAVEファイル ===\n\n")
};

// Error用メッセージ ------
const char *MsgErr[] = {
    QT_TRANSLATE_NOOP("PC6001VX", "Error"),
    QT_TRANSLATE_NOOP("PC6001VX", "エラーはありません"),
    QT_TRANSLATE_NOOP("PC6001VX", "原因不明のエラーが発生しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "メモリの確保に失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "指定された機種のROMイメージが見つからないため機種を変更しました\n設定を確認してください"),
    QT_TRANSLATE_NOOP("PC6001VX", "ROMイメージが見つかりません\n設定を確認してください"),
    QT_TRANSLATE_NOOP("PC6001VX", "ROMイメージのサイズが不正です"),
    QT_TRANSLATE_NOOP("PC6001VX", "ROMイメージのCRCが不正です"),
    QT_TRANSLATE_NOOP("PC6001VX", "ライブラリの初期化に失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "初期化に失敗しました\n設定を確認してください"),
    QT_TRANSLATE_NOOP("PC6001VX", "フォントの読込みに失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "フォントファイルの作成に失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "INIファイルの読込みに失敗しました\nデフォルト設定で起動します"),
    QT_TRANSLATE_NOOP("PC6001VX", "INIファイルの読込みに失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "INIファイルの保存に失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージのマウントに失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "DISKイメージのマウントに失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "拡張ROMイメージのマウントに失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "どこでもLOADに失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "どこでもSAVEに失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "どこでもLOADに失敗しました\n保存時とPC6001Vのバージョンが異なります"),
    QT_TRANSLATE_NOOP("PC6001VX", "リプレイ再生に失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "リプレイ記録に失敗しました"),
    QT_TRANSLATE_NOOP("PC6001VX", "リプレイデータがありません")
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
        QT_TRANSLATE_NOOP("PC6001VX", "MODE1,2 黒(ボーダー)"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE1 Set1 緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE1 Set1 深緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE1 Set2 橙"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE1 Set2 深橙"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE2 緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE2 黄"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE2 青"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE2 赤"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE2 白"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE2 シアン"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE2 マゼンタ"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE2 橙"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE3 Set1 緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE3 Set1 黄"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE3 Set1 青"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE3 Set1 赤"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE3 Set2 白"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE3 Set2 シアン"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE3 Set2 マゼンタ"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE3 Set2 橙"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 深緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 黒"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 白"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 赤"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 青"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 桃"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 明赤"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 暗赤"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 明青"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 暗青"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 明桃"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 暗桃"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 明緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set1 にじみ 暗緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 赤"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 青"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 桃"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 明赤"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 暗赤"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 明青"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 暗青"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 明桃"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 暗桃"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 明緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "MODE4 Set2 にじみ 暗緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 透明(黒)"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 橙"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 青緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 黄緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 青紫"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 赤紫"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 空色"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 灰色"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 黒"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 赤"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 緑"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 黄"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 青"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 マゼンタ"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 シアン"),
        QT_TRANSLATE_NOOP("PC6001VX", "mk2〜 白")
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
        { KVC_1,			QT_TRANSLATE_NOOP("PC6001VX", "1") },
        { KVC_2,			QT_TRANSLATE_NOOP("PC6001VX", "2") },
        { KVC_3,			QT_TRANSLATE_NOOP("PC6001VX", "3") },
        { KVC_4,			QT_TRANSLATE_NOOP("PC6001VX", "4") },
        { KVC_5,			QT_TRANSLATE_NOOP("PC6001VX", "5") },
        { KVC_6,			QT_TRANSLATE_NOOP("PC6001VX", "6") },
        { KVC_7,			QT_TRANSLATE_NOOP("PC6001VX", "7") },
        { KVC_8,			QT_TRANSLATE_NOOP("PC6001VX", "8") },
        { KVC_9,			QT_TRANSLATE_NOOP("PC6001VX", "9") },
        { KVC_0,			QT_TRANSLATE_NOOP("PC6001VX", "0") },

        { KVC_A,			QT_TRANSLATE_NOOP("PC6001VX", "A") },
        { KVC_B,			QT_TRANSLATE_NOOP("PC6001VX", "B") },
        { KVC_C,			QT_TRANSLATE_NOOP("PC6001VX", "C") },
        { KVC_D,			QT_TRANSLATE_NOOP("PC6001VX", "D") },
        { KVC_E,			QT_TRANSLATE_NOOP("PC6001VX", "E") },
        { KVC_F,			QT_TRANSLATE_NOOP("PC6001VX", "F") },
        { KVC_G,			QT_TRANSLATE_NOOP("PC6001VX", "G") },
        { KVC_H,			QT_TRANSLATE_NOOP("PC6001VX", "H") },
        { KVC_I,			QT_TRANSLATE_NOOP("PC6001VX", "I") },
        { KVC_J,			QT_TRANSLATE_NOOP("PC6001VX", "J") },
        { KVC_K,			QT_TRANSLATE_NOOP("PC6001VX", "K") },
        { KVC_L,			QT_TRANSLATE_NOOP("PC6001VX", "L") },
        { KVC_M,			QT_TRANSLATE_NOOP("PC6001VX", "M") },
        { KVC_N,			QT_TRANSLATE_NOOP("PC6001VX", "N") },
        { KVC_O,			QT_TRANSLATE_NOOP("PC6001VX", "O") },
        { KVC_P,			QT_TRANSLATE_NOOP("PC6001VX", "P") },
        { KVC_Q,			QT_TRANSLATE_NOOP("PC6001VX", "Q") },
        { KVC_R,			QT_TRANSLATE_NOOP("PC6001VX", "R") },
        { KVC_S,			QT_TRANSLATE_NOOP("PC6001VX", "S") },
        { KVC_T,			QT_TRANSLATE_NOOP("PC6001VX", "T") },
        { KVC_U,			QT_TRANSLATE_NOOP("PC6001VX", "U") },
        { KVC_V,			QT_TRANSLATE_NOOP("PC6001VX", "V") },
        { KVC_W,			QT_TRANSLATE_NOOP("PC6001VX", "W") },
        { KVC_X,			QT_TRANSLATE_NOOP("PC6001VX", "X") },
        { KVC_Y,			QT_TRANSLATE_NOOP("PC6001VX", "Y") },
        { KVC_Z,			QT_TRANSLATE_NOOP("PC6001VX", "Z") },

        { KVC_F1,			QT_TRANSLATE_NOOP("PC6001VX", "F1") },
        { KVC_F2,			QT_TRANSLATE_NOOP("PC6001VX", "F2") },
        { KVC_F3,			QT_TRANSLATE_NOOP("PC6001VX", "F3") },
        { KVC_F4,			QT_TRANSLATE_NOOP("PC6001VX", "F4") },
        { KVC_F5,			QT_TRANSLATE_NOOP("PC6001VX", "F5") },
        { KVC_F6,			QT_TRANSLATE_NOOP("PC6001VX", "F6") },
        { KVC_F7,			QT_TRANSLATE_NOOP("PC6001VX", "F7") },
        { KVC_F8,			QT_TRANSLATE_NOOP("PC6001VX", "F8") },
        { KVC_F9,			QT_TRANSLATE_NOOP("PC6001VX", "F9") },
        { KVC_F10,			QT_TRANSLATE_NOOP("PC6001VX", "F10") },
        { KVC_F11,			QT_TRANSLATE_NOOP("PC6001VX", "F11") },
        { KVC_F12,			QT_TRANSLATE_NOOP("PC6001VX", "F12") },

        { KVC_MINUS,		QT_TRANSLATE_NOOP("PC6001VX", "-") },
        { KVC_CARET,		QT_TRANSLATE_NOOP("PC6001VX", "^") },
        { KVC_BACKSPACE,	QT_TRANSLATE_NOOP("PC6001VX", "BackSpace") },
        { KVC_AT,			QT_TRANSLATE_NOOP("PC6001VX", "@") },
        { KVC_LBRACKET,		QT_TRANSLATE_NOOP("PC6001VX", "[") },
        { KVC_SEMICOLON,	QT_TRANSLATE_NOOP("PC6001VX", ";") },
        { KVC_COLON,		QT_TRANSLATE_NOOP("PC6001VX", ":") },
        { KVC_COMMA,		QT_TRANSLATE_NOOP("PC6001VX", ",") },
        { KVC_PERIOD,		QT_TRANSLATE_NOOP("PC6001VX", ".") },
        { KVC_SLASH,		QT_TRANSLATE_NOOP("PC6001VX", "/") },
        { KVC_SPACE,		QT_TRANSLATE_NOOP("PC6001VX", "Space") },

        { KVC_ESC,			QT_TRANSLATE_NOOP("PC6001VX", "ESC") },
        { KVC_HANZEN,		QT_TRANSLATE_NOOP("PC6001VX", "半角/全角") },
        { KVC_TAB,			QT_TRANSLATE_NOOP("PC6001VX", "Tab") },
        { KVC_CAPSLOCK,		QT_TRANSLATE_NOOP("PC6001VX", "CapsLock") },
        { KVC_ENTER,		QT_TRANSLATE_NOOP("PC6001VX", "Enter") },
        { KVC_LCTRL,		QT_TRANSLATE_NOOP("PC6001VX", "L-Ctrl") },
        { KVC_RCTRL,		QT_TRANSLATE_NOOP("PC6001VX", "R-Ctrl") },
        { KVC_LSHIFT,		QT_TRANSLATE_NOOP("PC6001VX", "L-Shift") },
        { KVC_RSHIFT,		QT_TRANSLATE_NOOP("PC6001VX", "R-Shift") },
        { KVC_LALT,			QT_TRANSLATE_NOOP("PC6001VX", "L-Alt") },
        { KVC_RALT,			QT_TRANSLATE_NOOP("PC6001VX", "R-Alt") },
        { KVC_PRINT,		QT_TRANSLATE_NOOP("PC6001VX", "PrintScreen") },
        { KVC_SCROLLLOCK,	QT_TRANSLATE_NOOP("PC6001VX", "ScrollLock") },
        { KVC_PAUSE,		QT_TRANSLATE_NOOP("PC6001VX", "Pause") },
        { KVC_INSERT,		QT_TRANSLATE_NOOP("PC6001VX", "Insert") },
        { KVC_DELETE,		QT_TRANSLATE_NOOP("PC6001VX", "Delete") },
        { KVC_END,			QT_TRANSLATE_NOOP("PC6001VX", "End") },
        { KVC_HOME,			QT_TRANSLATE_NOOP("PC6001VX", "Home") },
        { KVC_PAGEUP,		QT_TRANSLATE_NOOP("PC6001VX", "PageUp") },
        { KVC_PAGEDOWN,		QT_TRANSLATE_NOOP("PC6001VX", "PageDown") },

        { KVC_UP,			QT_TRANSLATE_NOOP("PC6001VX", "↑") },
        { KVC_DOWN,			QT_TRANSLATE_NOOP("PC6001VX", "↓") },
        { KVC_LEFT,			QT_TRANSLATE_NOOP("PC6001VX", "←") },
        { KVC_RIGHT,		QT_TRANSLATE_NOOP("PC6001VX", "→") },

        { KVC_P0,			QT_TRANSLATE_NOOP("PC6001VX", "0(テンキー)") },
        { KVC_P1,			QT_TRANSLATE_NOOP("PC6001VX", "1(テンキー)") },
        { KVC_P2,			QT_TRANSLATE_NOOP("PC6001VX", "2(テンキー)") },
        { KVC_P3,			QT_TRANSLATE_NOOP("PC6001VX", "3(テンキー)") },
        { KVC_P4,			QT_TRANSLATE_NOOP("PC6001VX", "4(テンキー)") },
        { KVC_P5,			QT_TRANSLATE_NOOP("PC6001VX", "5(テンキー)") },
        { KVC_P6,			QT_TRANSLATE_NOOP("PC6001VX", "6(テンキー)") },
        { KVC_P7,			QT_TRANSLATE_NOOP("PC6001VX", "7(テンキー)") },
        { KVC_P8,			QT_TRANSLATE_NOOP("PC6001VX", "8(テンキー)") },
        { KVC_P9,			QT_TRANSLATE_NOOP("PC6001VX", "9(テンキー)") },
        { KVC_NUMLOCK,		QT_TRANSLATE_NOOP("PC6001VX", "NumLock") },
        { KVC_P_PLUS,		QT_TRANSLATE_NOOP("PC6001VX", "+(テンキー)") },
        { KVC_P_MINUS,		QT_TRANSLATE_NOOP("PC6001VX", "-(テンキー)") },
        { KVC_P_MULTIPLY,	QT_TRANSLATE_NOOP("PC6001VX", "*(テンキー)") },
        { KVC_P_DIVIDE,		QT_TRANSLATE_NOOP("PC6001VX", "/(テンキー)") },
        { KVC_P_PERIOD,		QT_TRANSLATE_NOOP("PC6001VX", ".(テンキー)") },
        { KVC_P_ENTER,		QT_TRANSLATE_NOOP("PC6001VX", "Enter(テンキー)") },

        // 日本語キーボードのみ
        { KVC_YEN,			QT_TRANSLATE_NOOP("PC6001VX", "\\") },
        { KVC_RBRACKET,		QT_TRANSLATE_NOOP("PC6001VX", "]") },
        { KVC_UNDERSCORE,	QT_TRANSLATE_NOOP("PC6001VX", "_") },
        { KVC_MUHENKAN,		QT_TRANSLATE_NOOP("PC6001VX", "無変換") },
        { KVC_HENKAN,		QT_TRANSLATE_NOOP("PC6001VX", "変換") },
        { KVC_HIRAGANA,		QT_TRANSLATE_NOOP("PC6001VX", "ひらがな") },

        // 英語キーボードのみ
        { KVE_BACKSLASH,	QT_TRANSLATE_NOOP("PC6001VX", "BackSlash") },

        // 追加キー
        { KVX_RMETA,		QT_TRANSLATE_NOOP("PC6001VX", "L-Windows") },
        { KVX_LMETA,		QT_TRANSLATE_NOOP("PC6001VX", "R-Windows") },
        { KVX_MENU,			QT_TRANSLATE_NOOP("PC6001VX", "Menu") }
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
    if( SDL_Init( SDL_INIT_JOYSTICK ) )
        return false;
    SDL_JoystickEventState(SDL_DISABLE);
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
    if(view == NULL) return;
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
bool OSD_CreateWindow( HWINDOW *pwh, int w, int h, bool fsflag )
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
void OSD_BlitToWindow( HWINDOW Wh, VSurface *src, int x, int y )
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

    BYTE *psrc = (BYTE *)src->GetPixels() + src->Pitch() * src1.y + src1.x;

    const int length = image.bytesPerLine();
    for( int i=0; i < src1.h; i++ ){
        BYTE *pdst = (BYTE *)image.scanLine(i);
        memcpy( pdst, psrc, length );
        psrc += src->Pitch();
    }
    // 表示用のQPixmapItemへの変換はメインスレッドでないとできないため、
    // スロットを呼び出してメインスレッドでSceneを更新する
    // (直接呼び出すと呼び出し側スレッドで実行されてしまう)
    QMetaObject::invokeMethod(qApp, "layoutBitmap",
                              Q_ARG(HWINDOW, Wh),
                              Q_ARG(int, x),
                              Q_ARG(int, y),
                              Q_ARG(double, 1.0),
                              Q_ARG(double, 1.0),
                              Q_ARG(QImage, image));
}

////////////////////////////////////////////////////////////////
// ウィンドウに転送(拡大等)
//
// 引数:	wh			ウィンドウハンドル
//			src			転送元サーフェス
//			dx			転送先x座標
//			dy			転送先y座標
//			dh			転送先高さ
//			ntsc		4:3表示フラグ
//			scan		スキャンラインフラグ
//			brscan		スキャンライン輝度
// 返値:	なし
////////////////////////////////////////////////////////////////
#define	RESO		256		/* 中間色計算用分解能(1ラインをRESO分割する) */
void OSD_BlitToWindowEx( HWINDOW wh, VSurface *src, const int dx, const int dy, const int dh,
                        const bool ntsc, const bool scan, const int brscan )
{
    VRect src1,drc1;
    if( !src || !wh ) return;

    const int s = (scan ? 2 : 1);   //スキャンライン時には2行ずつ処理する

    const BYTE *spt  = (BYTE *)src->GetPixels();
    const int pp     = src->Pitch();

    const int xsc    = src->XScale();
    const int ww     = src->Width();
    const int hh     = src->Height();

    QImage image(src->Width(), src->Height() * s, QImage::Format_Indexed8);
    image.setColorTable(PaletteTable);

    const int dpp    = image.bytesPerLine();

    // 転送元範囲設定
    src1.x = qMax( 0, -dx / 2 * xsc );
    src1.y = qMax( 0, -dy );
    src1.w = qMin( ww * xsc - src1.x, image.width() );
    src1.h = qMin( hh       - src1.y, image.height() );

    if( src1.w <= 0 || src1.h <= 0 ) return;

    // 転送先範囲設定
    drc1.x = qMax( 0, dx );
    drc1.y = qMax( 0, dy );
    drc1.w = qMin( ww, (image.width() - drc1.x)*xsc );
    drc1.h = qMin( dh, image.height() - drc1.y );

    for( int y=0; y<drc1.h; y+=s ){
        BYTE *sof  = (BYTE *)spt  + pp * y / s + src1.x;

        memcpy( (BYTE *)image.scanLine(y), sof, dpp );

        if(scan){
            BYTE *sdoff = (BYTE *)image.scanLine(y+1);
            memcpy( sdoff, sof, dpp );
            for( int x=0; x<drc1.w; x++ ){
                //スキャンライン用の暗い色は128ずらしたインデックスに登録されている
                (*sdoff++) += 128;
            }
        }
    }

    double imgXScale = 720.0 / image.width();
    double imgYScale = (ntsc ? 540.0 : 460.0) / image.height();
    // 表示用のQPixmapItemへの変換はメインスレッドでないとできないため、
    // スロットを呼び出してメインスレッドでSceneを更新する
    // (直接呼び出すと呼び出し側スレッドで実行されてしまう)
    QMetaObject::invokeMethod(qApp, "layoutBitmap",
                              Q_ARG(HWINDOW, wh),
                              Q_ARG(int, dx),
                              Q_ARG(int, dy),
                              Q_ARG(double, imgXScale),
                              Q_ARG(double, imgYScale),
                              Q_ARG(QImage, image));
}


////////////////////////////////////////////////////////////////
// ウィンドウのイメージデータ取得
//
// 引数:	wh			ウィンドウハンドル
//			pixels		転送先配列ポインタへのポインタ
//			pos			保存する領域情報へのポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_GetWindowImage( HWINDOW wh, void **pixels, VRect *pos )
{
    if( !wh || !pixels ) return false;

    // Sceneのレンダリングはメインスレッドでないとできないため、
    // スロットを呼び出してメインスレッドでSceneの画像を取得する
    // (直接呼び出すと呼び出し側スレッドで実行されてしまう)
    // 呼び出し元に結果を返す必要があるため、Qt::BlockingQueuedConnectionで実行する。
    QMetaObject::invokeMethod(qApp, "getWindowImage",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(HWINDOW, wh),
                              Q_ARG(QRect, QRect(pos->x, pos->y, pos->w, pos->h)),
                              Q_ARG(void**, pixels));

    return true;
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
    strcpy(path, strPath.toUtf8().constData());
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
    strcpy(path, strPath.toUtf8().constData());
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
bool OSD_CreateModulePath()
{
    QString basePath = QString::fromUtf8(OSD_GetModulePath());

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

    // どこでもSAVEファイル
    if(!QDir(basePath).mkpath(QString::fromUtf8(DOKOSAVE_DIR)))
        return false;

    return true;
}

////////////////////////////////////////////////////////////////
// 設定及び諸ファイル(ROMなどの初期位置)パス取得
//
// 引数:	なし
// 返値:	char *		取得した文字列へのポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetModulePath( void )
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
    sprintf( mpath, "%s", confPath.toUtf8().constData() );
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

    static QByteArray fileName;
    QFileInfo info(QString::fromUtf8(path));
    fileName = info.fileName().toUtf8();
    return fileName.constData();
}

////////////////////////////////////////////////////////////////
// フルパスからフォルダ名を取得
//
// 引数:	path		フルパス格納バッファポインタ
// 返値:	char *		フォルダ名の開始ポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetFolderNamePart( const char *path )
{
    PRINTD( OSD_LOG, "[OSD][OSD_GetFolderNamePart]\n" );

    static QByteArray filePath;
    QFileInfo info(QString::fromUtf8(path));
    filePath = info.dir().absolutePath().toUtf8();
    return filePath.constData();
}

////////////////////////////////////////////////////////////////
// フルパスから拡張子名を取得
//
// 引数:	path		フルパス格納バッファポインタ
// 返値:	char *		拡張子名の開始ポインタ
////////////////////////////////////////////////////////////////
const char *OSD_GetFileNameExt( const char *path )
{
    PRINTD( OSD_LOG, "[OSD][OSD_GetFileNameExt]\n" );

    static QByteArray ext;
    QFileInfo info(QString::fromUtf8(path));
    ext = info.suffix().toUtf8();
    return ext.constData();
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
    QByteArray result = QFileDialog::getExistingDirectory(NULL, TRANS("フォルダを選択してください。"), QDir::homePath()).toUtf8();
    strcpy(Result, result);
    return result.constData();
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
        // 入力されたファイル名に拡張子がついていない場合は付与する
        QFileInfo info(result);
        if(info.suffix() != ext){
            result += QString(".") + ext;
        }
    } else {
        result = QFileDialog::getOpenFileName(NULL, title, pathStr, filter);
    }
    if(result.isEmpty())    return NULL;

    QDir dir(result);

    if( path ) strcpy( path, dir.path().toUtf8().constData() );
    if( fullpath ) strcpy( fullpath, result.toUtf8().constData() );

    QFile file(result);
    return file.fileName().toUtf8().constData();
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
        title  = QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージ選択(LOAD)");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージ (*.p6t *.cas *.p6);;"
                "P6T形式 (*.p6t);;"
                "ベタ形式 (*.cas *.p6);;"
                "全てのファイル (*.*)");
        ext    = "p6t";
        break;

    case FD_TapeSave:	// TAPE(SAVE)選択
        mode   = FM_Save;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージ選択(SAVE)");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "TAPEイメージ (*.p6t *.cas *.p6);;"
                "P6T形式 (*.p6t);;"
                "ベタ形式 (*.cas *.p6);;"
                "全てのファイル (*.*)");
        ext    = "p6t";
        break;

    case FD_Disk:		// DISK選択
        mode   = FM_Load;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "DISKイメージ選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "DISKイメージ(*.d88);;"
                "D88形式 (*.d88);;"
                "全てのファイル (*.*)");
        ext    = "d88";
        break;

    case FD_ExtRom:		// 拡張ROM選択
        mode   = FM_Load;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "拡張ROMイメージ選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "全てのファイル (*.*)");
        break;

    case FD_Printer:	// プリンター出力ファイル選択
        mode   = FM_Save;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "プリンター出力ファイル選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "全てのファイル (*.*)");
        ext    = "txt";
        break;

    case FD_FontZ:		// 全角フォントファイル選択
        mode   = FM_Load;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "全角フォントファイル選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "PNGファイル (*.png);;"
                "全てのファイル (*.*)");
        ext    = "png";
        break;

    case FD_FontH:		// 半角フォントファイル選択
        mode   = FM_Load;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "半角フォントファイル選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "PNGファイル (*.png);;"
                "全てのファイル (*.*)");
        ext    = "png";
        break;

    case FD_DokoLoad:	// どこでもLOADファイル選択
        mode   = FM_Load;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "どこでもLOADファイル選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "どこでもSAVEファイル (*.dds *.ddr);;"
                "どこでもSAVE形式 (*.dds);;"
                "リプレイファイル (*.ddr);;"
                "全てのファイル (*.*)");
        ext    = "dds";
        break;

    case FD_DokoSave:	// どこでもSAVEファイル選択
        mode   = FM_Save;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "どこでもSAVEファイル選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "どこでもSAVE形式 (*.dds);;"
                "全てのファイル (*.*)");
        ext    = "dds";
        break;

    case FD_RepLoad:	// リプレイ再生ファイル選択
        mode   = FM_Load;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "リプレイ再生ファイル選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "リプレイファイル (*.ddr);;"
                "全てのファイル (*.*)");
        ext    = "ddr";
        break;

    case FD_RepSave:	// リプレイ保存ファイル選択
        mode   = FM_Save;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "リプレイ保存ファイル選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "リプレイファイル (*.ddr);;"
                "全てのファイル (*.*)");
        ext    = "ddr";
        break;

    case FD_AVISave:	// ビデオキャプチャ出力ファイル選択
        mode   = FM_Save;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "ビデオキャプチャ出力ファイル選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "AVIファイル (*.avi);;"
                "全てのファイル (*.*)");
        ext    = "avi";
        break;

    case FD_LoadAll:	// 汎用LOAD
    default:
        mode   = FM_Load;
        title  = QT_TRANSLATE_NOOP("PC6001VX", "ファイル選択");
        filter = QT_TRANSLATE_NOOP("PC6001VX", "全てのファイル (*.*)");
        break;
    }

    return OSD_FileDiaog( NULL, mode, TRANS(title), TRANS(filter), fullpath, path, ext );
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

    QMessageBox mb(IconType, TRANS(cap), TRANS(mes), Type);
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
#ifndef NOSOUND
    //実行時に出る警告の抑止
    qRegisterMetaType<QAudio::State>();

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
        format = info.nearestFormat(format);
    }

    audioOutput = new QAudioOutput(info, format);
    //#PENDING これではグローバルボリュームを変えてしまう？
    audioOutput->setVolume(0.5);

    audioOutput->moveToThread(qApp->thread());
    audioOutput->setParent(qApp);
#endif
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
#ifndef NOSOUND
    if(audioOutput){
        audioOutput->stop();
    }
#endif
}


////////////////////////////////////////////////////////////////
// 再生開始
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_StartAudio( void )
{
#ifndef NOSOUND
    if(audioOutput){
        if(audioOutput->state() == QAudio::SuspendedState){
            audioOutput->resume();
        } else {
            audioBuffer = audioOutput->start();
        }
    }
#endif
}


////////////////////////////////////////////////////////////////
// 再生停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_StopAudio( void )
{
#ifndef NOSOUND
    if(audioOutput){
        audioOutput->suspend();
    }
#endif
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
#ifndef NOSOUND
    if(audioBuffer){
        audioBuffer->write((const char*)stream, samples);
    }
#endif
}


////////////////////////////////////////////////////////////////
// 再生状態取得
//
// 引数:	なし
// 返値:	bool		true:再生中 false:停止中
////////////////////////////////////////////////////////////////
bool OSD_AudioPlaying( void )
{
#ifndef NOSOUND
    if(audioOutput){
        return audioOutput->state() == QAudio::ActiveState;
    } else {
        return false;
    }
#else
    return false;
#endif
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
    // 何もしない
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
bool OSD_CreateFont( const char *hfile, const char *zfile, int size )
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
    QMutexLocker lock(&joystickMutex);
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
    QMutexLocker lock(&joystickMutex);
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
    QMutexLocker lock(&joystickMutex);
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
    QMutexLocker lock(&joystickMutex);
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
    QMutexLocker lock(&joystickMutex);
    SDL_JoystickClose( (SDL_Joystick *)jinfo );
    for (std::map<int, HJOYINFO>::iterator p = joyMap.begin(); p != joyMap.end(); ++p){
        if((*p).second == jinfo){
            joyMap.erase(p);
        }
    }
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
    QMutexLocker lock(&joystickMutex);
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
    QMutexLocker lock(&joystickMutex);
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
    QMutexLocker lock(&joystickMutex);
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
    QMutexLocker lock(&joystickMutex);
    static TiltDirection prevDir = NEWTRAL;
    if(num == 0){
        // ジョイスティックの左右に対応して画面を傾ける
        int Xmove = SDL_JoystickGetAxis( (SDL_Joystick *)jinfo, num );
        if( Xmove < INT16_MIN / 2 ){  // 左
            TiltScreen(LEFT);
            prevDir = LEFT;
        } else if( Xmove > INT16_MAX / 2 ){  // 右
            TiltScreen(RIGHT);
            prevDir = RIGHT;
        } else if( prevDir != NEWTRAL ){
            TiltScreen(NEWTRAL);
            prevDir = NEWTRAL;
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
    QMutexLocker lock(&joystickMutex);
    return SDL_JoystickGetButton( (SDL_Joystick *)jinfo, num ) ? true : false;
#else
    return false;
#endif
}

///////////////////////////////////////////////////////////
// 環境設定ダイアログ表示
//
// 引数:	hwnd		ウィンドウハンドル
// 返値:	int			1:OK 0:CANCEL -1:ERROR
///////////////////////////////////////////////////////////
int OSD_ConfigDialog( HWINDOW hwnd )
{
    // INIファイルを開く
    try{
        ecfg = new CFG6();
        if( !ecfg->Init() ) throw Error::IniReadFailed;
    }
    // new に失敗した場合
    catch( std::bad_alloc ){
        return -1;
    }
    // 例外発生
    catch( Error::Errno i ){
        delete ecfg;
        ecfg = NULL;
        return -1;
    }

    ConfigDialog dialog(ecfg);
    dialog.exec();
    int ret = dialog.result();
    // OKボタンが押されたならINIファイル書込み
    if( ret == QDialog::Accepted) ecfg->Write();

    delete ecfg;
    ecfg = NULL;

    return ret;
}

///////////////////////////////////////////////////////////
// バージョン情報表示
//
// 引数:	hwnd		ウィンドウハンドル
//			mdl			機種
// 返値:	なし
///////////////////////////////////////////////////////////
void OSD_VersionDialog( HWINDOW hwnd, int mdl )
{
    AboutDialog dialog(mdl);
    dialog.exec();
}
