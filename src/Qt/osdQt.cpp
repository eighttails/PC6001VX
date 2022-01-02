// OS依存の汎用ルーチン(主にUI用)
#include "../typedef.h"

#include <cstdio>

#include <QtCore>
#include <QtWidgets>
#ifndef NOSOUND
#include <QtMultimedia>
#endif

#ifndef NOJOYSTICK
#ifdef SDLJOYSTICK
#include <SDL2/SDL.h>
#else
#include <QtGamepad>
#endif // SDLJOYSTICK
#endif // NOJOYSTICK

#include "../log.h"
#include "../osd.h"
#include "../common.h"
#include "../pc6001v.h"
#include "../joystick.h"
#include "../config.h"
#include "../error.h"
#include "configdialog.h"

#include "renderview.h"
#include "p6vxapp.h"
#include "p6vxcommon.h"

#ifndef NOSOUND
#include "wavfile.h"
#endif


//エミュレータ内部用イベントキュー
QMutex eventMutex;
QQueue<Event> eventQueue;
QWaitCondition eventEmitted;

//経過時間タイマ
QElapsedTimer elapsedTimer;

std::map<int, PCKEYsym> VKTable;		// Qtキーコード  -> 仮想キーコード 変換テーブル

#ifndef NOSOUND
//サウンド関連
#include "audiooutputwrapper.h"
QPointer<QIODevice> audioBuffer = nullptr;
QPointer<AudioOutputWrapper> audioOutput = nullptr;
#endif

//ジョイスティック関連
#ifndef NOJOYSTICK
QMutex joystickMutex;
#endif // NOJOYSTICK

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
{ Qt::Key_CapsLock,			KVC_CAPSLOCK },		// CapsLock
{ Qt::Key_Return,			KVC_ENTER },		// Enter
{ Qt::Key_Enter,			KVC_ENTER },		// Enter(テンキー側)
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

#if 0 //#TODO 廃止？

// INIファイル用メッセージ ------
const char *MsgIni[] = {
	// [CONFIG]
	QT_TRANSLATE_NOOP("PC6001VX", "; === PC6001V 初期設定ファイル ===\n\n"),
	QT_TRANSLATE_NOOP("PC6001VX", "機種 60:PC-6001 61:PC-6001A 62:PC-6001mk2 66:PC-6601 64:PC-6001mk2SR 68:PC-6601SR"),
	// QT_TRANSLATE_NOOP("PC6001VX", "機種 60:PC-6001 61:PC-6001A 62:PC-6001mk2 66:PC-6601"),
	QT_TRANSLATE_NOOP("PC6001VX", "FDD接続台数 (0-2)"),
	QT_TRANSLATE_NOOP("PC6001VX", "拡張RAM使用"),
	QT_TRANSLATE_NOOP("PC6001VX", "Turbo TAPE Yes:有効 No:無効"),
	QT_TRANSLATE_NOOP("PC6001VX", "BoostUp Yes:有効 No:無効"),
	QT_TRANSLATE_NOOP("PC6001VX", "BoostUp 最大倍率(N60モード)"),
	QT_TRANSLATE_NOOP("PC6001VX", "BoostUp 最大倍率(N60m/N66モード)"),
	QT_TRANSLATE_NOOP("PC6001VX", "オーバークロック率 (1-1000)%"),
	QT_TRANSLATE_NOOP("PC6001VX", "CRCチェック Yes:有効 No:無効"),
	QT_TRANSLATE_NOOP("PC6001VX", "ROMパッチ Yes:あてる No:あてない"),
	QT_TRANSLATE_NOOP("PC6001VX", "FDDアクセスウェイト Yes:有効 No:無効"),
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

	if( num < 0 || num >= COUNTOF( JColorName ) ) return nullptr;
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

	const char *str = nullptr;
	for( int i=0; i<(int)(sizeof(kname)/sizeof(PCKeyName)); i++ ){
		if( kname[i].PCKey == sym ){
			str = kname[i].Name;
			break;
		}
	}
	return str;
}
#endif

////////////////////////////////////////////////////////////////
// 小文字による文字列比較
//
// 引数:	s1			比較する文字列へのポインタ1
//		s2			比較する文字列へのポインタ2
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

/////////////////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_Init( void )
{
	// 経過時間タイマーをスタート
	elapsedTimer.start();

	// Qtキーコード  -> 仮想キーコード 変換テーブル初期化
	for( int i=0; i < COUNTOF(VKeyDef); i++ )
		VKTable[VKeyDef[i].InKey] = VKeyDef[i].VKey;

	return OSD_Init_Sub();
}

/////////////////////////////////////////////////////////////////////////////
// 初期化Sub(ライブラリ依存処理等)
//
// 引数:	なし
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_Init_Sub( void )
{
#ifndef NOJOYSTICK
#ifdef SDLJOYSTICK
	if( SDL_Init( SDL_INIT_JOYSTICK ) )
		return false;
	SDL_JoystickEventState(SDL_DISABLE);
#else
	// 何もしない
#endif // SDLJOYSTICK
#endif // NOJOYSTICK
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

	OSD_Quit_Sub();
}

/////////////////////////////////////////////////////////////////////////////
// 終了処理Sub(ライブラリ依存処理等)
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_Quit_Sub( void )
{
#ifndef NOJOYSTICK
#ifdef SDLJOYSTICK
	SDL_Quit();
#else
	// 何もしない
#endif // SDLJOYSTICK
#endif // NOJOYSTICK
}

/////////////////////////////////////////////////////////////////////////////
// 多重起動チェック
//
// 引数:	なし
// 返値:	bool			true:起動済み false:未起動
/////////////////////////////////////////////////////////////////////////////
bool OSD_IsWorking( void )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	return app->isRunning();
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
// キャプション設定
//
// 引数:	hwnd			ウィンドウハンドル
//		str			キャプション文字列へのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_SetWindowCaption( HWINDOW hwnd, const char *str )
{
	QGraphicsView* view = static_cast<QGraphicsView*>(hwnd);
	if(view == nullptr) return;
	auto window = view->parentWidget();
	QMetaObject::invokeMethod(window, "setWindowTitle",
							  Q_ARG(QString, str));
}

/////////////////////////////////////////////////////////////////////////////
// 設定ファイルパス取得
//
// 引数:	なし
// 返値:	P6VPATH&		取得した文字列への参照(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const P6VPATH& OSD_GetConfigPath()
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetConfigPath]\n" );

	// Windowsの場合はexe本体と同じ場所。
	// それ以外(UNIX系を想定)は ~/.pc6001vx を返す
#if defined Q_OS_WIN
	QString confPath = qApp->applicationDirPath();
#elif defined Q_OS_IOS
	QString confPath = QDir::homePath() + QDir::separator() + QString("Documents");
#else
	QString confPath = QDir::homePath() + QDir::separator() + QString(".pc6001vx4");
#endif
	static P6VPATH cpath = QSTR2P6VPATH(confPath);
	OSD_AddDelimiter( cpath );	// 念のため
	return cpath;
}

/////////////////////////////////////////////////////////////////////////////
// パスの末尾にデリミタを追加
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_AddDelimiter( P6VPATH& path )
{
	QString qPath = P6VPATH2QSTR(path);
	if (!qPath.endsWith('/') && !qPath.endsWith(QDir::separator())){
		qPath += QDir::separator();
	}
	path = QSTR2P6VPATH(qPath);
}


////////////////////////////////////////////////////////////////
// パスの末尾のデリミタを削除
//
// 引数:	path		パス格納バッファポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_DelDelimiter( P6VPATH& path )
{
	QString qPath = P6VPATH2QSTR(path);
	if (qPath.endsWith('/') || qPath.endsWith(QDir::separator())){
		qPath.truncate(qPath.length() - 1);
	}
	path = QSTR2P6VPATH(qPath);
}

/////////////////////////////////////////////////////////////////////////////
// 相対パス化
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_RelativePath( P6VPATH& path )
{
#ifdef Q_OS_WIN
	if( QDir( P6VPATH2QSTR(path) ).isRelative()
			|| QDir( P6VPATH2QSTR(path) ).path().startsWith(":")
			|| ! path.length() ) return;
	QDir dir(P6VPATH2QSTR(OSD_GetConfigPath()));
	QString relPath = dir.relativeFilePath(P6VPATH2QSTR(path));
	path = QSTR2P6VPATH(relPath);
#else
	// Windows以外では相対パスを使った運用はしない
	OSD_AbsolutePath(path);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// 絶対パス化
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_AbsolutePath( P6VPATH& path )
{
	QString qPath = P6VPATH2QSTR(path);
	if( !QDir( qPath ).isRelative() || qPath.isEmpty() ) return;
	QDir dir(P6VPATH2QSTR(OSD_GetConfigPath()));
	path = QSTR2P6VPATH(QDir::cleanPath(dir.absoluteFilePath(qPath)));
}


/////////////////////////////////////////////////////////////////////////////
// パス結合
//
// 引数:	cpath			結合後パス
//			path1			パス1
//			path2			パス2
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_AddPath( P6VPATH& cpath, const P6VPATH& path1, const P6VPATH& path2 )
{
	QDir dir(P6VPATH2QSTR(path1));
	QString path = QDir::cleanPath(dir.filePath(P6VPATH2QSTR(path2)));
	if (!path.startsWith(":")) {
		path = QDir::toNativeSeparators(path);
	}
	cpath = QSTR2P6VPATH(path);
}


/////////////////////////////////////////////////////////////////////////////
// パスからフォルダ名を取得
//
// 引数:	path			パス
// 返値:	std::string		取得した文字列
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetFolderNamePart( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFolderNamePart]\n" );

	QFileInfo info(P6VPATH2QSTR(path));
	return QDir::cleanPath(info.dir().absolutePath()).toStdString();
}


/////////////////////////////////////////////////////////////////////////////
// パスからファイル名を取得
//
// 引数:	path			パス
// 返値:	std::string		取得した文字列(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetFileNamePart( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFileNamePart]\n" );

	static QByteArray filename;
	QFileInfo info(P6VPATH2QSTR(path));
	filename = info.fileName().toUtf8();
	return filename.toStdString();
}


/////////////////////////////////////////////////////////////////////////////
// パスから拡張子名を取得
//
// 引数:	path			パス
// 返値:	std::string		取得した文字列(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetFileNameExt( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFileNameExt]\n" );

	static QByteArray ext;
	QFileInfo info(P6VPATH2QSTR(path));
	ext = info.suffix().toUtf8();
	return ext.toStdString();
}

/////////////////////////////////////////////////////////////////////////////
// 拡張子名を変更
//
// 引数:	path			パス
//		ext				新しい拡張子への参照
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_ChangeFileNameExt( P6VPATH& path, const std::string& ext )
{
	PRINTD( OSD_LOG, "[OSD][OSD_ChangeFileNameExt] %s -> %s\n", OSD_GetFileNameExt( path ).c_str(), ext.c_str() );

	QString qPath = P6VPATH2QSTR(path);
	QFileInfo info(qPath);
	if (!info.exists()) {
		return false;
	}
	QString newPath = info.path() + info.completeBaseName() + "." + QString::fromStdString(ext);
	QFile::rename(qPath, newPath);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// ファイルを開く
//
// 引数:	path			パス
//			mode			モード文字列への参照
// 返値:	FILE*			ファイルポインタ
/////////////////////////////////////////////////////////////////////////////
FILE* OSD_Fopen( const P6VPATH& path, const std::string& mode )
{
	PRINTD( OSD_LOG, "[OSD][OSD_Fopen] %s(%s) ", P6VPATH2STR( path ).c_str(), mode.c_str() );
	QString strFileName = P6VPATH2QSTR(path);

	if (strFileName.startsWith(":")){
		// リソース内のファイルはテンポラリファイルとして作成
		QTemporaryFile* tempFile = QTemporaryFile::createNativeFile(strFileName);
		tempFile->setAutoRemove(true);
		// アプリ終了時に削除されるように設定
		tempFile->setParent(qApp);
		return fopen(tempFile->fileName().toLocal8Bit(), mode.c_str());
	} else {
		return fopen(strFileName.toLocal8Bit(), mode.c_str());
	}
}


/////////////////////////////////////////////////////////////////////////////
// ファイルストリームを開く
//
// 引数:	fs				ファイルストリームへの参照
//			path			パス
//			mode			モード
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FSopen( std::fstream& fs, const P6VPATH& path, const std::ios_base::openmode mode )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FSopen] %s\n", P6VPATH2STR( path ).c_str() );
	QString strFileName = P6VPATH2QSTR(path);

	if (strFileName.startsWith(":")){
		// リソース内のファイルはテンポラリファイルとして作成
		QTemporaryFile* tempFile = QTemporaryFile::createNativeFile(strFileName);
		tempFile->setAutoRemove(true);
		// アプリ終了時に削除されるように設定
		tempFile->setParent(qApp);
		fs.open( tempFile->fileName().toStdString(), mode );
	} else {
		fs.open( strFileName.toStdString(), mode );
	}
	return fs.is_open() && fs.good();
}


/////////////////////////////////////////////////////////////////////////////
// フォルダを作成
//
// 引数:	path			パス
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_CreateFolder( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_CreateFolder] %s\n", P6VPATH2STR( path ).c_str() );

	std::error_code ec;

	P6VPATH tpath = path;
	OSD_DelDelimiter( tpath );	// デリミタ付きだと作成されない?
	OSD_AbsolutePath( tpath );
	PRINTD( OSD_LOG, "-> %s\n", P6VPATH2STR( tpath ).c_str() );

	// 設定ファイルパスより外側には作成しない
	if( P6VPATH2STR( tpath ).compare( 0, P6VPATH2STR( OSD_GetConfigPath() ).length(), P6VPATH2STR( OSD_GetConfigPath() ) ) ) return false;

	QDir dir;
	return dir.mkpath(P6VPATH2QSTR(tpath));
}

/////////////////////////////////////////////////////////////////////////////
// ファイルの存在チェック
//
// 引数:	fullpath		パス
// 返値:	bool			true:存在する false:存在しない
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileExist( const P6VPATH& fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileExist]\n" );

	QString pathString = P6VPATH2QSTR(fullpath);

	// ワイルドカードを含む場合
	if (pathString.contains("*")){
		QFileInfo info(pathString);

		QDir dir = info.absoluteDir();
		QFile file(pathString);
		QString wildcard = info.fileName();

		QFileInfoList list = dir.entryInfoList(QStringList(wildcard), QDir::Files);
		if (list.empty()){
			qDebug() << pathString << " does not exist.";
		}
		return !list.empty();
	} else {
		QFile file(pathString);
		if (!file.exists()){
			qDebug() << pathString << " does not exist.";
		}
		return file.exists();
	}
}

/////////////////////////////////////////////////////////////////////////////
// ファイルサイズ取得
//
// 引数:	fullpath		パス
// 返値:	DWORD			ファイルサイズ
/////////////////////////////////////////////////////////////////////////////
DWORD OSD_GetFileSize( const P6VPATH& fullpath )
{
	QFileInfo info(P6VPATH2QSTR(fullpath));
	return info.size(); // 存在しない場合は0が返る
}

/////////////////////////////////////////////////////////////////////////////
// ファイルの読取り専用チェック
//
// 引数:	fullpath		パス
// 返値:	bool			true:読取り専用 false:読み書き
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileReadOnly( const P6VPATH& fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileReadOnly]\n" );

	QFileInfo info(P6VPATH2QSTR(fullpath));
	return !info.isWritable();
}



/////////////////////////////////////////////////////////////////////////////
// ファイルを削除
//
// 引数:	fullpath		パス
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileDelete( const P6VPATH& fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileDelete] %s\n", P6VPATH2STR( fullpath ).c_str() );

	QFile file(P6VPATH2QSTR(fullpath));
	return file.remove();
}


/////////////////////////////////////////////////////////////////////////////
// ファイルを探す
//
// 引数:	path			パス
//		file			探すファイル名
//		folders			見つかったパスを格納するvectorへの参照
//		size			ファイルサイズ (0:チェックしない)
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FindFile( const P6VPATH& path, const P6VPATH& file, std::vector<P6VPATH>& files, size_t size )
{
	std::string sfile = OSD_GetFileNamePart( file );
	std::transform( sfile.begin(), sfile.end(), sfile.begin(), ::tolower );	// 小文字

	QDirIterator it(P6VPATH2QSTR(path),  QDir::Files, QDirIterator::FollowSymlinks);
	while (it.hasNext()) {
		it.next();
		// パスからファイル名を抽出し、小文字に変換
		std::string tfile = it.fileName().toLower().toStdString();

		// ファイル名比較。
		if (sfile == tfile){
			// #TODO ファイル名一致＆サイズ一致してたら結果に登録
			QFileInfo info(it.filePath());
			if (size_t(info.size()) == size){
				files.push_back(QSTR2P6VPATH(it.filePath()));
			}
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ファイル名を変更
//
// 引数:	fullpath1		変更元のパス
//		fullpath2		変更するパス
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileRename( const P6VPATH& fullpath1, const P6VPATH& fullpath2 )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileRename] %s -> %s\n", P6VPATH2STR( fullpath1 ).c_str(), P6VPATH2STR( fullpath2 ).c_str() );

	QFile file(P6VPATH2QSTR(fullpath1));
	return file.rename(P6VPATH2QSTR(fullpath2));
}


/////////////////////////////////////////////////////////////////////////////
// フォルダの参照
//
// 引数:	hwnd		親のウィンドウハンドル
//		path		パス
// 返値:	bool		true:選択成功 false:エラーorキャンセル
/////////////////////////////////////////////////////////////////////////////
bool OSD_FolderDiaog( HWINDOW hwnd, P6VPATH& path )
{
	const char* ret = nullptr;
	char cpath[PATH_MAX];
	strcpy(cpath, P6VPATH2STR(path).c_str());
	// 呼び元スレッドによってコネクションタイプを変える(戻り値を取得できるようにするために必要)
	Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
				Qt::DirectConnection : Qt::BlockingQueuedConnection;
	QMetaObject::invokeMethod(qApp, "folderDialog",
							  cType,
							  Q_RETURN_ARG(const char*, ret),
							  Q_ARG(void*, hwnd),
							  Q_ARG(char*, cpath));
	path = STR2P6VPATH(ret);
	return strlen(ret) ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// 各種ファイル選択
//
// 引数:	hwnd		親のウィンドウハンドル
//		type		ダイアログの種類(FileDlg参照)
//		fullpath	フルパス
//		path		ファイル検索パス
// 返値:	bool		true:選択成功 false:エラーorキャンセル
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileSelect( HWINDOW hwnd, FileDlg type, P6VPATH& fullpath, P6VPATH& path )
{
	FileMode mode = FM_Load;
	const char *title   = nullptr;
	const char *filter  = nullptr;
	const char *ext     = nullptr;

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
		filter = QT_TRANSLATE_NOOP("PC6001VX", "WebMファイル (*.webm);;"
											   "全てのファイル (*.*)");
		ext    = "webm";
		break;

	case FD_LoadAll:	// 汎用LOAD
	default:
		mode   = FM_Load;
		title  = QT_TRANSLATE_NOOP("PC6001VX", "ファイル選択");
		filter = QT_TRANSLATE_NOOP("PC6001VX", "全てのファイル (*.*)");
		break;
	}

	const char* ret = nullptr;
	char cpath[PATH_MAX];
	strcpy(cpath, P6VPATH2STR(path).c_str());
	char cfullpath[PATH_MAX];
	strcpy(cfullpath, P6VPATH2STR(fullpath).c_str());

	// 呼び元スレッドによってコネクションタイプを変える(戻り値を取得できるようにするために必要)
	Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
				Qt::DirectConnection : Qt::BlockingQueuedConnection;
	QMetaObject::invokeMethod(qApp, "fileDialog",
							  cType,
							  Q_RETURN_ARG(const char*, ret),
							  Q_ARG(void*, hwnd),
							  Q_ARG(FileMode, mode),
							  Q_ARG(const char*, TRANS(title)),
							  Q_ARG(const char*, TRANS(filter)),
							  Q_ARG(char*, cfullpath),
							  Q_ARG(char*, cpath),
							  Q_ARG(const char*, ext));
	path = STR2P6VPATH(ret);
	fullpath = STR2P6VPATH(ret);
	return strlen(ret) ? true : false;
}



/////////////////////////////////////////////////////////////////////////////
// メッセージ表示
//
// 引数:	hwnd		親のウィンドウハンドル
//		mes			メッセージ文字列への参照(UTF-8)
//		cap			ウィンドウキャプション文字列への参照(UTF-8)
//		type		表示形式指示のフラグ
// 返値:	int			押されたボタンの種類
//						OSDR_OK:     OKボタン
//						OSDR_CANCEL: CANCELボタン
//						OSDR_YES:    YESボタン
//						OSDR_NO:     NOボタン
/////////////////////////////////////////////////////////////////////////////
int OSD_Message( HWINDOW hwnd, const std::string& mes, const std::string& cap, int type )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	int ret = OSDR_OK;
	// 呼び元スレッドによってコネクションタイプを変える(戻り値を取得できるようにするために必要)
	Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
				Qt::DirectConnection : Qt::BlockingQueuedConnection;
	QMetaObject::invokeMethod(qApp, "showMessageBox",
							  cType,
							  Q_RETURN_ARG(int, ret),
							  Q_ARG(void*, app->getView()),
							  Q_ARG(const char*, mes.c_str()),
							  Q_ARG(const char*, cap.c_str()),
							  Q_ARG(int, type));
	return ret;
}


/////////////////////////////////////////////////////////////////////////////
// キーリピート設定
//
// 引数:	repeat			キーリピートの間隔(ms) 0で無効
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_SetKeyRepeat( int repeat )
{
	QApplication::setKeyboardInputInterval(repeat);
}


#ifndef NOJOYSTICK
std::map<int, HJOYINFO> joyMap;
#endif // NOJOYSTICK

/////////////////////////////////////////////////////////////////////////////
// 利用可能なジョイスティック数取得
//
// 引数:	なし
// 返値:	int				利用可能なジョイスティック数
/////////////////////////////////////////////////////////////////////////////
int OSD_GetJoyNum( void )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
#ifdef SDLJOYSTICK
	return SDL_NumJoysticks();
#else
	auto mgr = QGamepadManager::instance();
	auto num = mgr->connectedGamepads().length();
	return num;
#endif // SDLJOYSTICK
#else
	return 0;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティック名取得
//
// 引数:	int				インデックス
// 返値:	std::string		ジョイスティック名文字列(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetJoyName( int index )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
#ifdef SDLJOYSTICK
	return SDL_JoystickNameForIndex( index );
#else
	auto mgr = QGamepadManager::instance();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
	auto devIndex = mgr->connectedGamepads()[index];
	auto name = mgr->gamepadName(devIndex);
#else
	QString name = "";
#endif
	return name.length() > 0
			? name.toStdString()
			: QString("Joystick%1").arg(index).toStdString();
#endif // SDLJOYSTICK
#else
	return "";
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティックオープンされてる？
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
// 返値:	bool			true:OPEN false:CLOSE
/////////////////////////////////////////////////////////////////////////////
bool OSD_OpenedJoy( HJOYINFO joy )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
#ifdef SDLJOYSTICK
	return SDL_JoystickGetAttached( (SDL_Joystick*)joy ) ? true : false;
#else
	auto mgr = QGamepadManager::instance();
	for(auto index : mgr->connectedGamepads()){
		if((static_cast<QGamepad*>(joy))->deviceId() == index){
			return true;
		}
	}
	return false;
#endif // SDLJOYSTICK
#else
	return false;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティックオープン
//
// 引数:	int				インデックス
// 返値:	HJOYINFO		ジョイスティック情報へのポインタ
/////////////////////////////////////////////////////////////////////////////
HJOYINFO OSD_OpenJoy( int index )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
#ifdef SDLJOYSTICK
	joyMap[index] = (HJOYINFO)SDL_JoystickOpen( index );
	return joyMap[index];
#else
	auto mgr = QGamepadManager::instance();
	auto devIndex = mgr->connectedGamepads()[index];
	if (joyMap.count(devIndex) == 0){
		joyMap[devIndex] = new QGamepad(devIndex, qApp);
	}
	return (HJOYINFO)joyMap[devIndex];
#endif // SDLJOYSTICK
#else
	return (HJOYINFO)nullptr;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティッククローズ
//
// 引数:	int				インデックス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_CloseJoy( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
#ifdef SDLJOYSTICK
	SDL_JoystickClose( static_cast<SDL_Joystick*>(jinfo) );
#else
	(static_cast<QGamepad*>(jinfo))->deleteLater();
#endif // SDLJOYSTICK
	for (auto p = joyMap.begin(); p != joyMap.end();){
		if(p->second == jinfo){
			p = joyMap.erase(p);
		} else {
			++p;
		}
	}
#else
#endif // NOJOYSTICK
}

/////////////////////////////////////////////////////////////////////////////
// ジョイスティックの軸の数取得
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
// 返値:	int				軸の数
/////////////////////////////////////////////////////////////////////////////
int OSD_GetJoyNumAxes( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
#ifdef SDLJOYSTICK
	return SDL_JoystickNumAxes( static_cast<SDL_Joystick*>(jinfo) );
#else
	// QtGamepadでは固定(スティックがLR2本、軸がXYの2軸でトータルで4つ)
	return 4;
#endif // SDLJOYSTICK
#else
	return 1;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティックのボタンの数取得
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
// 返値:	int				ボタンの数
/////////////////////////////////////////////////////////////////////////////
int OSD_GetJoyNumButtons( HJOYINFO jinfo )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
#ifdef SDLJOYSTICK
	return SDL_JoystickNumButtons( static_cast<SDL_Joystick*>(jinfo) );
#else
	// QtGamepadでは固定(エミュレーターに渡すのはABXYの4つ)
	return 4;
#endif // SDLJOYSTICK
#else
	return 0;
#endif // NOJOYSTICK
}

#if 0 //#TODO 廃止？
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
#ifdef SDLJOYSTICK
	SDL_JoystickUpdate();
#else
	// 何もしない
#endif // SDLJOYSTICK
#else
#endif // NOJOYSTICK
}
#endif

/////////////////////////////////////////////////////////////////////////////
// ジョイスティックの軸の状態取得
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
//			int				軸の番号
// 返値:	int				軸の状態(-32768～32767)
/////////////////////////////////////////////////////////////////////////////
int OSD_GetJoyAxis( HJOYINFO jinfo, int num )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
#ifdef SDLJOYSTICK
	// HAT(デジタルスティック)から値を取得
	auto hat = SDL_JoystickGetHat( static_cast<SDL_Joystick*>(jinfo), 0 );
	int hatVal = 0;
	switch (num){
	case 0:
		if (hat & SDL_HAT_RIGHT)	hatVal = 32767;
		if (hat & SDL_HAT_LEFT)		hatVal = -32767;
		break;
	case 1:;
		if (hat & SDL_HAT_UP)		hatVal = -32767;
		if (hat & SDL_HAT_DOWN)		hatVal = 32767;
		break;
	default:;
	}

	// アナログスティックから値を取得
	int axisVal = SDL_JoystickGetAxis( static_cast<SDL_Joystick*>(jinfo), num );

	// 出力値はHAT優先
	return hatVal ? hatVal : axisVal;
#else
	QGamepad* joy = static_cast<QGamepad*>(jinfo);
	if (!joy) return 0;
	int val = 0;
	// 出力値はHAT優先
	switch (num){
	case 0:
		if (joy->buttonRight()) val = 32767;
		else if (joy->buttonLeft()) val = -32767;
		else val = int16_t(joy->axisLeftX() * 32767);
		break;
	case 1:
		if (joy->buttonUp()) val = -32767;
		else if (joy->buttonDown()) val = 32767;
		else val = int16_t(joy->axisLeftY() * 32767);
		break;
	case 2:
		val = int16_t(joy->axisRightX() * 32767);	break;
	case 3:
		val = int16_t(joy->axisRightY() * 32767);	break;
	default:;
	}
	return val;
#endif // SDLJOYSTICK
#else
	return 0;
#endif // NOJOYSTICK
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティックのボタンの状態取得
//
// 引数:	HJOYINFO		ジョイスティック情報へのポインタ
//			int				ボタンの番号
// 返値:	bool			ボタンの状態 true:ON false:OFF
/////////////////////////////////////////////////////////////////////////////
bool OSD_GetJoyButton( HJOYINFO jinfo, int num )
{
#ifndef NOJOYSTICK
	QMutexLocker lock(&joystickMutex);
#ifdef SDLJOYSTICK
	return SDL_JoystickGetButton( static_cast<SDL_Joystick*>(jinfo), num ) ? true : false;
#else
	QGamepad* joy = static_cast<QGamepad*>(jinfo);
	if (!joy) return false;
	bool val = false;
	switch (num) {
	case 0:
		val = joy->buttonA();	break;
	case 1:
		val = joy->buttonB();	break;
	case 2:
		val = joy->buttonX();	break;
	case 3:
		val = joy->buttonY();	break;
	default:;
	}
	return val;
#endif // SDLJOYSTICK
#else
	return false;
#endif // NOJOYSTICK
}

/////////////////////////////////////////////////////////////////////////////
// オーディオデバイスオープン
//
// 引数:	obj				自分自身へのオブジェクトポインタ
//			callback		コールバック関数へのポインタ
//			rate			サンプリングレート
//			sample			バッファサイズ(サンプル数)
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_OpenAudio( void* obj, CBF_SND callback, int rate, int samples )
{
#ifndef NOSOUND
	// 実行時に出る警告の抑止
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

	audioOutput = new AudioOutputWrapper(info, format);
	// #TODO これではグローバルボリュームを変えてしまう？
	// audioOutput->setVolume(0.5);

	audioOutput->moveToThread(qApp->thread());
#endif
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// オーディオデバイスクローズ
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_CloseAudio( void )
{
#ifndef NOSOUND
	if(audioOutput){
		QMetaObject::invokeMethod(audioOutput, "stop");
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// 再生開始
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_StartAudio( void )
{
#ifndef NOSOUND
	if(audioOutput){
		if(audioOutput->state() == QAudio::SuspendedState){
			QMetaObject::invokeMethod(audioOutput, "resume");
		} else {
			// 呼び元スレッドによってコネクションタイプを変える(戻り値を取得できるようにするために必要)
			Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
						Qt::DirectConnection : Qt::BlockingQueuedConnection;
			QMetaObject::invokeMethod(audioOutput, "start", cType,
									  Q_RETURN_ARG(QPointer<QIODevice>, audioBuffer));
		}
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// 再生停止
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_StopAudio( void )
{
#ifndef NOSOUND
	if(audioOutput){
		QMetaObject::invokeMethod(audioOutput, "suspend");
	}
#endif
}

////////////////////////////////////////////////////////////////
// オーディオストリーム書き込み
//
// 引数:	stream  書き込むデータへのポインタ
//      size 書き込むバイト数
// 返値:	なし
////////////////////////////////////////////////////////////////
void OSD_WriteAudioStream(BYTE *stream, int size)
{
#ifndef NOSOUND
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	if(!app->isAVI()){
		if(audioBuffer){
			audioBuffer->write((const char*)stream, size);
		}
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// 再生状態取得
//
// 引数:	なし
// 返値:	bool			true:再生中 false:停止中
/////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////
// Waveファイル読込み
// 　対応形式は 22050Hz以上,符号付き16bit,1ch
//
// 引数:	filepath		入力ファイルパス
//			buf				バッファポインタ格納ポインタ
//			len				ファイル長さ格納ポインタ
//			freq			サンプリングレート格納ポインタ
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_LoadWAV( const P6VPATH& filepath, BYTE** buf, DWORD* len, int* freq )
{
#ifndef NOSOUND
	WavFile w;
	if(!w.open(P6VPATH2STR(filepath).c_str())) return false;

	const QAudioFormat& format = w.fileFormat();
	size_t bodySize = w.size() - w.headerLength();
	*len = bodySize;
	*freq = format.sampleRate();
	BYTE* buffer = new BYTE[bodySize];
	memcpy(buffer, &w.readAll().data()[w.headerLength()], bodySize);
	*buf = buffer;
	return true;
#else
	return false;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// Waveファイル開放
//
// 引数:	buf				バッファへのポインタ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_FreeWAV( BYTE* buf )
{
	if(buf) delete[] buf;
}


/////////////////////////////////////////////////////////////////////////////
// オーディオをロックする
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_LockAudio( void )
{
	// 何もしない
}


/////////////////////////////////////////////////////////////////////////////
// オーディオをアンロックする
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_UnlockAudio( void )
{
	// 何もしない
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

/////////////////////////////////////////////////////////////////////////////
// タイマ追加
//
// 引数:	interval		割込み間隔(ms)
//			callback		コールバック関数
//			param			コールバック関数に渡す引数
// 返値:	TIMERID			タイマID(失敗したら0)
/////////////////////////////////////////////////////////////////////////////
TIMERID OSD_AddTimer( DWORD interval, CBF_TMR callback, void* param )
{
	// 何もしない(失敗を返す)
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// タイマ削除
//
// 引数:	id				タイマID
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_DelTimer( TIMERID id )
{
	// 何もしない(失敗を返す)
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウ作成
//
// 引数:	hwnd			ウィンドウハンドルへのポインタ
//			w				ウィンドウの幅
//			h				ウィンドウの高さ
//			sw				スクリーンの幅
//			sh				スクリーンの高さ
//			fsflag			true:フルスクリーン false:ウィンドウ
//			filter			true:拡縮時にフィルタリングする false:しない
//			scanbr			スキャンライン輝度
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_CreateWindow( HWINDOW* hwnd, const int w, const int h, const int sw, const int sh, const bool fsflag, const bool filter, const int scanbr )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);

	RenderView* view = app->getView();
	QGraphicsScene* scene = view->scene();

	scene->setSceneRect(0, 0, w, h);
	*hwnd = view;

	QMetaObject::invokeMethod(qApp, "createWindow",
							  Q_ARG(HWINDOW, *hwnd),
							  Q_ARG(bool, fsflag));

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウ破棄
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_DestroyWindow( HWINDOW hwnd )
{
	// 何もしない
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウの幅を取得
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	int				幅
/////////////////////////////////////////////////////////////////////////////
int OSD_GetWindowWidth( HWINDOW hwnd )
{
	// QtではSceneRectの幅を返す
	QGraphicsView* view = static_cast<QGraphicsView*>(hwnd);
	Q_ASSERT(view);

	return view->scene()->width();
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウの高さを取得
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	int				高さ
/////////////////////////////////////////////////////////////////////////////
int OSD_GetWindowHeight( HWINDOW hwnd )
{
	// QtではSceneRectの高さを返す
	QGraphicsView* view = static_cast<QGraphicsView*>(hwnd);
	Q_ASSERT(view);

	return view->scene()->height();
}


////////////////////////////////////////////////////////////////
// パレット設定
//
// 引数:	hwnd			ウィンドウハンドル
//		pal			パレットへのポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool OSD_SetPalette( VSurface* sur )
{

}

/////////////////////////////////////////////////////////////////////////////
// フルスクリーン?
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	bool			true:フルスクリーン表示 false:ウィンドウ表示
/////////////////////////////////////////////////////////////////////////////
bool OSD_IsFullScreen( HWINDOW hwnd )
{
	//#TODO 後で実装
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// フィルタリング有効?
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	bool			true:有効 false:無効
/////////////////////////////////////////////////////////////////////////////
bool OSD_IsFiltering( HWINDOW hwnd )
{
	//#TODO 後で実装
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウのサイズ変更可否設定
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	bool			true:変更可 false:変更不可
/////////////////////////////////////////////////////////////////////////////
void OSD_SetWindowResizable( HWINDOW hwnd, bool resize )
{
	// 何もしない(常にリサイズ可能)
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウクリア
//  色は0(黒)で決め打ち
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_ClearWindow( HWINDOW hwnd )
{
	QMetaObject::invokeMethod(qApp, "clearLayout",
							  Q_ARG(HWINDOW, hwnd));
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウ反映
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_RenderWindow( HWINDOW hwnd )
{
	// 何もしない
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウに転送(等倍)
//
// 引数:	hwnd			ウィンドウハンドル
//			src				転送元サーフェス
//			x				転送先x座標
//			y				転送先y座標
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_BlitToWindow( HWINDOW hwnd, VSurface* src, const int x, const int y )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	VRect src1,drc1;

	QImage image(src->Width(), src->Height(), QImage::Format_Indexed8);
	image.setColorTable(app->getPaletteTable());

	// 転送元範囲設定
	src1.x = 0;
	src1.y = 0;
	src1.w = src->Width();
	src1.h = src->Height();

	if( src1.w <= 0 || src1.h <= 0 ) return;

	// 転送先範囲設定
	drc1.x = 0;
	drc1.y = 0;

	BYTE *psrc = src->GetPixels().data() + src->Pitch() * src1.y + src1.x;

	const int length = image.bytesPerLine();
	for( int i=0; i < src1.h; i++ ){
		BYTE *pdst = (BYTE *)image.scanLine(i);
		memcpy( pdst, psrc, length );
		psrc += src->Pitch();
	}
	image = image.convertToFormat(QImage::Format_RGBX8888);
	// 表示用のQPixmapItemへの変換はメインスレッドでないとできないため、
	// スロットを呼び出してメインスレッドでSceneを更新する
	// (直接呼び出すと呼び出し側スレッドで実行されてしまう)
	QMetaObject::invokeMethod(qApp, "layoutBitmap",
							  Q_ARG(HWINDOW, hwnd),
							  Q_ARG(int, x),
							  Q_ARG(int, y),
							  Q_ARG(double, 1.0),
							  Q_ARG(double, 1.0),
							  Q_ARG(QImage, image));
}

/////////////////////////////////////////////////////////////////////////////
// ウィンドウに転送(拡大等)
//
// 引数:	hwnd			ウィンドウハンドル
//		src				転送元サーフェス
//		pos				転送先 座標/サイズ
//		scanen			スキャンラインフラグ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_BlitToWindowEx( HWINDOW hwnd, VSurface* src, const VRect* pos, const bool scanen )
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	VRect src1,drc1;
	if( !src || !hwnd ) return;

	const int s = (scanen ? 2 : 1);   // スキャンライン時には2行ずつ処理する

	const BYTE *spt  = src->GetPixels().data();
	const int pp     = src->Pitch();

	const int ww     = src->Width();
	const int hh     = src->Height();


	QImage image(src->Width(), src->Height() * s, QImage::Format_Indexed8);
	image.setColorTable(app->getPaletteTable());

	const int dpp    = image.bytesPerLine();

	// 転送元範囲設定
	src1.x = qMax( 0, -pos->x );
	src1.y = qMax( 0, -pos->y );
	src1.w = qMin( ww - src1.x, image.width() );
	src1.h = qMin( hh - src1.y, image.height() );

	if( src1.w <= 0 || src1.h <= 0 ) return;

	// 転送先範囲設定
	drc1.x = qMax( 0, pos->x );
	drc1.y = qMax( 0, pos->y );
	drc1.w = qMin( ww, (image.width() - drc1.x) );
	drc1.h = qMin( hh * s, image.height() - drc1.y );

	for( int y=0; y<drc1.h; y+=s ){
		BYTE *sof  = (BYTE *)spt  + pp * y / s + src1.x;

		memcpy( (BYTE *)image.scanLine(y), sof, dpp );

		if(scanen){
			BYTE *sdoff = (BYTE *)image.scanLine(y+1);
			memcpy( sdoff, sof, dpp );
			for( int x=0; x<drc1.w; x++ ){
				// スキャンライン用の暗い色は128ずらしたインデックスに登録されている
				(*sdoff++) += 128;
			}
		}
	}

	double imgXScale = double(pos->w) / image.width();
	double imgYScale = double(pos->h) / image.height();
	// 表示用のQPixmapItemへの変換はメインスレッドでないとできないため、
	// スロットを呼び出してメインスレッドでSceneを更新する
	// (直接呼び出すと呼び出し側スレッドで実行されてしまう)

	QMetaObject::invokeMethod(qApp, "layoutBitmap",
							  Q_ARG(HWINDOW, hwnd),
							  Q_ARG(int, pos->x),
							  Q_ARG(int, pos->y),
							  Q_ARG(double, imgXScale),
							  Q_ARG(double, imgYScale),
							  Q_ARG(QImage, image));
}


/////////////////////////////////////////////////////////////////////////////
// ウィンドウのイメージデータ取得
//
// 引数:	hwnd			ウィンドウハンドル
//			pixels			転送先配列への参照
//			pos				保存する領域情報へのポインタ
//			pxfmt			ピクセルフォーマット
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_GetWindowImage( HWINDOW hwnd, std::vector<BYTE>& pixels, VRect* pos, PixelFMT pxfmt )
{
	if( !hwnd ) return false;
	// 呼び元スレッドによってコネクションタイプを変える(戻り値を取得できるようにするために必要)
	Qt::ConnectionType cType = QThread::currentThread() == qApp->thread() ?
				Qt::DirectConnection : Qt::BlockingQueuedConnection;
	QMetaObject::invokeMethod(qApp, "getWindowImage",
							  cType,
							  Q_ARG(HWINDOW, hwnd),
							  Q_ARG(QRect, QRect(pos->x, pos->y, pos->w, pos->h)),
							  Q_ARG(void**, (void**)pixels.data()));

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// アイコン設定
//
// 引数:	hwnd			ウィンドウハンドル
//			model			機種 60,61,62,66,64,68
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_SetIcon( HWINDOW hwnd, int model )
{
	// 機種別P6オブジェクト確保
	const char* iconRes = nullptr;
	switch( model ){
	case 68: iconRes = ":/res/PC-6601SR.ico"; break;
	case 66: iconRes = ":/res/PC-6601.ico"; break;
	case 64: iconRes = ":/res/PC-6001mk2SR.ico"; break;
	case 62: iconRes = ":/res/PC-6001mk2.ico"; break;
	default: iconRes = ":/res/PC-6001.ico";
	}

	// アイコン設定
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	QMetaObject::invokeMethod(app, "setWindowIcon",
							  Q_ARG(QIcon, QIcon(iconRes)));
}


/////////////////////////////////////////////////////////////////////////////
// キャプション設定
//
// 引数:	hwnd			ウィンドウハンドル
//			str				キャプション文字列への参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_SetWindowCaption( HWINDOW hwnd, const std::string& str )
{
	QGraphicsView* view = static_cast<QGraphicsView*>(hwnd);
	if(view == nullptr) return;
	auto window = view->parentWidget();
	QMetaObject::invokeMethod(window, "setWindowTitle",
							  Q_ARG(QString, QString::fromStdString(str)));
}

/////////////////////////////////////////////////////////////////////////////
// マウスカーソル表示/非表示
//
// 引数:	disp			true:表示 false:非表示
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_ShowCursor( bool disp )
{
	QMetaObject::invokeMethod(qApp, "deactivateMouseCursorTimer");
	qApp->setOverrideCursor(disp ? Qt::ArrowCursor : Qt::BlankCursor);
}

/////////////////////////////////////////////////////////////////////////////
// OS依存のウィンドウハンドルを取得
//
// 引数:	hwnd			ウィンドウハンドル
// 返値:	void*			OS依存のウィンドウハンドル
/////////////////////////////////////////////////////////////////////////////
void* OSD_GetWindowHandle( HWINDOW hwnd )
{
	// 使わない
	assert(false);
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////
// 環境設定ダイアログ表示
//
// 引数:	hwnd		ウィンドウハンドル
// 返値:	int			1:OK 0:CANCEL -1:ERROR
/////////////////////////////////////////////////////////////////////////////
int OSD_ConfigDialog( HWINDOW hwnd )
{
	// INIファイルを開く
	try{
		CFG6 ecfg;
		if( !ecfg.Init() ) throw Error::IniReadFailed;

		ConfigDialog dialog(&ecfg);
#ifdef ALWAYSFULLSCREEN
		dialog.setWindowState(dialog.windowState() | Qt::WindowFullScreen);
#endif
		dialog.exec();
		int ret = dialog.result();
		// OKボタンが押されたならINIファイル書込み
		if( ret == QDialog::Accepted) {
			ecfg.Write();
		}
		return ret;
	}
	// new に失敗した場合
	catch( std::bad_alloc ){
		return -1;
	}
	// 例外発生
	catch( Error::Errno i ){
		return -1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// バージョン情報表示
//
// 引数:	hwnd		ウィンドウハンドル
//			mdl			機種
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_VersionDialog( HWINDOW hwnd, int mdl )
{
	QWidget* parent = static_cast<QWidget*>(hwnd);
	QMessageBox::about(parent, "About PC6001VX",
					   APPNAME " Version " VERSION "\n"
					  "https://eighttails.seesaa.net\n"
					  "©2012-2021, Tadahito Yao (@eighttails)\n"
					  "Based on PC6001V by Yumitaro.");
}


/////////////////////////////////////////////////////////////////////////////
// イベントキュークリア
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_FlushEvents( void )
{
	QMutexLocker lock(&eventMutex);
	eventQueue.clear();
}


/////////////////////////////////////////////////////////////////////////////
// イベント取得(イベントが発生するまで待つ)
//
// 引数:	ev				イベント情報共用体へのポインタ
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_GetEvent( Event *ev )
{
	QMutexLocker lock(&eventMutex);
	// イベントキューが空の場合、イベントが発行されるまで待つ
	if(eventQueue.empty()){
		eventEmitted.wait(&eventMutex);
	}
	*ev = eventQueue.front();
	eventQueue.pop_front();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// イベントをキューにプッシュする
//
// 引数:	ev				イベントタイプ
//			...				イベントタイプに応じた引数
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_PushEvent(const Event& ev)
{
	// イベントキューにプッシュ
	QMutexLocker lock(&eventMutex);
	eventQueue.push_back(ev);
	eventEmitted.wakeAll();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// イベントをキューにプッシュする
//
// 引数:	ev				イベントタイプ
//			...				イベントタイプに応じた引数
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_PushEvent( EventType ev, ... )
{
	Event event;
	event.type = ev;
	va_list args;
	va_start( args, ev );

	switch( ev ){
	case EV_DEBUGMODEBP:
		event.bp.addr = va_arg( args, int );
		break;
	default:;
	}

	va_end( args );

	return OSD_PushEvent( event );
}


/////////////////////////////////////////////////////////////////////////////
// キューに指定のイベントが存在するか調査する
//
// 引数:	ev				イベントタイプ
// 返値:	bool			true:ある false:ない
/////////////////////////////////////////////////////////////////////////////
bool OSD_HasEvent( EventType ev )
{
	QMutexLocker lock(&eventMutex);
	foreach(auto e, eventQueue) {
		if (e.type == ev) return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// イベント処理の状態を種類ごとに設定する
//
// 引数:	ev				イベントタイプ
//			st				ステータス
// 返値:	bool			変更前の状態 true:有効 false:無効
/////////////////////////////////////////////////////////////////////////////
bool OSD_EventState( EventType ev, EventState st )
{
	// 何もしない(呼ばれない)
	assert(false);
	return false;
}

////////////////////////////////////////////////////////////////
// フォントファイル作成
//
// 引数:	hfile		半角フォントファイルパス
//		zfile		全角フォントファイルパス
//		size		文字サイズ(半角文字幅ピクセル数)
// 返値:	bool		true:作成成功 false:作成失敗
////////////////////////////////////////////////////////////////
bool OSD_CreateFont( const char *hfile, const char *zfile, int size )
{
	// 何もしない
	// フォントファイルはQtのリソースから読み込む
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// ShiftJIS -> UTF-8
//
// 引数:	str			文字列バッファへの参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_SJIStoUTF8( std::string& str )
{
	QByteArray encodedString = str.c_str();
	QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");
	if (!codec) return false;
	str = codec->toUnicode(encodedString).toUtf8().data();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// UTF-8 -> ShiftJIS
//
// 引数:	str			文字列バッファへの参照
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_UTF8toSJIS( std::string& str )
{
	QString string = str.c_str();
	QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");
	if (!codec) return false;
	str = codec->fromUnicode(string).data();
	return true;
}
