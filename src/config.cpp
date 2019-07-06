#include <stdlib.h>
#include <new>

#include "pc6001v.h"
#include "config.h"
#include "common.h"
#include "p6vxcommon.h"
#include "error.h"
#include "osd.h"

const P6KeyName CFG6::P6KeyNameDef[] = {	// P6キーコード 名称定義
											{ KP6_UNKNOWN,		"K6_UNKNOWN" },

											{ KP6_1,			"K6_1" },
											{ KP6_2,			"K6_2" },
											{ KP6_3,			"K6_3" },
											{ KP6_4,			"K6_4" },
											{ KP6_5,			"K6_5" },
											{ KP6_6,			"K6_6" },
											{ KP6_7,			"K6_7" },
											{ KP6_8,			"K6_8" },
											{ KP6_9,			"K6_9" },
											{ KP6_0,			"K6_0" },

											{ KP6_A,			"K6_A" },
											{ KP6_B,			"K6_B" },
											{ KP6_C,			"K6_C" },
											{ KP6_D,			"K6_D" },
											{ KP6_E,			"K6_E" },
											{ KP6_F,			"K6_F" },
											{ KP6_G,			"K6_G" },
											{ KP6_H,			"K6_H" },
											{ KP6_I,			"K6_I" },
											{ KP6_J,			"K6_J" },
											{ KP6_K,			"K6_K" },
											{ KP6_L,			"K6_L" },
											{ KP6_M,			"K6_M" },
											{ KP6_N,			"K6_N" },
											{ KP6_O,			"K6_O" },
											{ KP6_P,			"K6_P" },
											{ KP6_Q,			"K6_Q" },
											{ KP6_R,			"K6_R" },
											{ KP6_S,			"K6_S" },
											{ KP6_T,			"K6_T" },
											{ KP6_U,			"K6_U" },
											{ KP6_V,			"K6_V" },
											{ KP6_W,			"K6_W" },
											{ KP6_X,			"K6_X" },
											{ KP6_Y,			"K6_Y" },
											{ KP6_Z,			"K6_Z" },

											{ KP6_F1,			"K6_F1" },
											{ KP6_F2,			"K6_F2" },
											{ KP6_F3,			"K6_F3" },
											{ KP6_F4,			"K6_F4" },
											{ KP6_F5,			"K6_F5" },

											{ KP6_MINUS,		"K6_MINUS" },
											{ KP6_CARET,		"K6_CARET" },
											{ KP6_YEN,			"K6_YEN" },
											{ KP6_AT,			"K6_AT" },
											{ KP6_LBRACKET,		"K6_LBRACKET" },
											{ KP6_RBRACKET,		"K6_RBRACKET" },
											{ KP6_SEMICOLON,	"K6_SEMICOLON" },
											{ KP6_COLON,		"K6_COLON" },
											{ KP6_COMMA,		"K6_COMMA" },
											{ KP6_PERIOD,		"K6_PERIOD" },
											{ KP6_SLASH,		"K6_SLASH" },
											{ KP6_UNDERSCORE,	"K6_UNDERSCORE" },
											{ KP6_SPACE,		"K6_SPACE" },

											{ KP6_ESC,			"K6_ESC" },
											{ KP6_TAB,			"K6_TAB" },
											{ KP6_CTRL,			"K6_CTRL" },
											{ KP6_SHIFT,		"K6_SHIFT" },
											{ KP6_GRAPH,		"K6_GRAPH" },
											{ KP6_HOME,			"K6_HOME" },
											{ KP6_STOP,			"K6_STOP" },
											{ KP6_PAGE,			"K6_PAGE" },
											{ KP6_RETURN,		"K6_RETURN" },
											{ KP6_KANA,			"K6_KANA" },
											{ KP6_INS,			"K6_INS" },
											{ KP6_DEL,			"K6_DEL" },

											{ KP6_UP,			"K6_UP" },
											{ KP6_DOWN,			"K6_DOWN" },
											{ KP6_LEFT,			"K6_LEFT" },
											{ KP6_RIGHT,		"K6_RIGHT" },

											{ KP6_MODE,			"K6_MODE" },
											{ KP6_CAPS,			"K6_CAPS" },


											// テンキー部拡張
											{ KP6_P0,			"K6_P0" },
											{ KP6_P1,			"K6_P1" },
											{ KP6_P2,			"K6_P2" },
											{ KP6_P3,			"K6_P3" },
											{ KP6_P4,			"K6_P4" },
											{ KP6_P5,			"K6_P5" },
											{ KP6_P6,			"K6_P6" },
											{ KP6_P7,			"K6_P7" },
											{ KP6_P8,			"K6_P8" },
											{ KP6_P9,			"K6_P9" },
											{ KP6_PPLUS,		"K6_PPLUS" },
											{ KP6_PMINUS,		"K6_PMINUS" },
											{ KP6_PMULTIPLY,	"K6_PMULTIPLY" },
											{ KP6_PDIVIDE,		"K6_PDIVIDE" },
											{ KP6_PPERIOD,		"K6_PPERIOD" },
											{ KP6_PRETURN,		"K6_PRETURN" },


											// 各種機能キー
											{ KFN_1,			"K6_FN1" },
											{ KFN_2,			"K6_FN2" },
											{ KFN_3,			"K6_FN3" },
											{ KFN_4,			"K6_FN4" },
											{ KFN_5,			"K6_FN5" },
											{ KFN_6,			"K6_FN6" },
											{ KFN_7,			"K6_FN7" },
											{ KFN_8,			"K6_FN8" },
											{ KFN_9,			"K6_FN9" }
									   };


const PCKeyName CFG6::PCKeyNameDef[] = {	// 仮想キーコード 名称定義
											{ KVC_UNKNOWN,		"K_UNKNOWN" },

											{ KVC_1,			"K_1" },
											{ KVC_2,			"K_2" },
											{ KVC_3,			"K_3" },
											{ KVC_4,			"K_4" },
											{ KVC_5,			"K_5" },
											{ KVC_6,			"K_6" },
											{ KVC_7,			"K_7" },
											{ KVC_8,			"K_8" },
											{ KVC_9,			"K_9" },
											{ KVC_0,			"K_0" },

											{ KVC_A,			"K_A" },
											{ KVC_B,			"K_B" },
											{ KVC_C,			"K_C" },
											{ KVC_D,			"K_D" },
											{ KVC_E,			"K_E" },
											{ KVC_F,			"K_F" },
											{ KVC_G,			"K_G" },
											{ KVC_H,			"K_H" },
											{ KVC_I,			"K_I" },
											{ KVC_J,			"K_J" },
											{ KVC_K,			"K_K" },
											{ KVC_L,			"K_L" },
											{ KVC_M,			"K_M" },
											{ KVC_N,			"K_N" },
											{ KVC_O,			"K_O" },
											{ KVC_P,			"K_P" },
											{ KVC_Q,			"K_Q" },
											{ KVC_R,			"K_R" },
											{ KVC_S,			"K_S" },
											{ KVC_T,			"K_T" },
											{ KVC_U,			"K_U" },
											{ KVC_V,			"K_V" },
											{ KVC_W,			"K_W" },
											{ KVC_X,			"K_X" },
											{ KVC_Y,			"K_Y" },
											{ KVC_Z,			"K_Z" },

											{ KVC_F1,			"K_F1" },
											{ KVC_F2,			"K_F2" },
											{ KVC_F3,			"K_F3" },
											{ KVC_F4,			"K_F4" },
											{ KVC_F5,			"K_F5" },
											{ KVC_F6,			"K_F6" },
											{ KVC_F7,			"K_F7" },
											{ KVC_F8,			"K_F8" },
											{ KVC_F9,			"K_F9" },
											{ KVC_F10,			"K_F10" },
											{ KVC_F11,			"K_F11" },
											{ KVC_F12,			"K_F12" },

											{ KVC_MINUS,		"K_MINUS" },
											{ KVC_CARET,		"K_CARET" },
											{ KVC_BACKSPACE,	"K_BACKSPACE" },
											{ KVC_AT,			"K_AT" },
											{ KVC_LBRACKET,		"K_LBRACKET" },
											{ KVC_SEMICOLON,	"K_SEMICOLON" },
											{ KVC_COLON,		"K_COLON" },
											{ KVC_COMMA,		"K_COMMA" },
											{ KVC_PERIOD,		"K_PERIOD" },
											{ KVC_SLASH,		"K_SLASH" },
											{ KVC_SPACE,		"K_SPACE" },

											{ KVC_ESC,			"K_ESC" },
											{ KVC_HANZEN,		"K_HANZEN" },
											{ KVC_TAB,			"K_TAB" },
											{ KVC_CAPSLOCK,		"K_CAPSLOCK" },
											{ KVC_ENTER,		"K_ENTER" },
											{ KVC_LCTRL,		"K_LCTRL" },
											{ KVC_RCTRL,		"K_RCTRL" },
											{ KVC_LSHIFT,		"K_LSHIFT" },
											{ KVC_RSHIFT,		"K_RSHIFT" },
											{ KVC_LALT,			"K_LALT" },
											{ KVC_RALT,			"K_RALT" },
											{ KVC_PRINT,		"K_PRINT" },
											{ KVC_SCROLLLOCK,	"K_SCROLLLOCK" },
											{ KVC_PAUSE,		"K_PAUSE" },
											{ KVC_INSERT,		"K_INSERT" },
											{ KVC_DELETE,		"K_DELETE" },
											{ KVC_HOME,			"K_HOME" },
											{ KVC_END,			"K_END" },
											{ KVC_PAGEUP,		"K_PAGEUP" },
											{ KVC_PAGEDOWN,		"K_PAGEDOWN" },

											{ KVC_UP,			"K_UP" },
											{ KVC_DOWN,			"K_DOWN" },
											{ KVC_LEFT,			"K_LEFT" },
											{ KVC_RIGHT,		"K_RIGHT" },

											{ KVC_P0,			"K_P_0" },
											{ KVC_P1,			"K_P_1" },
											{ KVC_P2,			"K_P_2" },
											{ KVC_P3,			"K_P_3" },
											{ KVC_P4,			"K_P_4" },
											{ KVC_P5,			"K_P_5" },
											{ KVC_P6,			"K_P_6" },
											{ KVC_P7,			"K_P_7" },
											{ KVC_P8,			"K_P_8" },
											{ KVC_P9,			"K_P_9" },
											{ KVC_NUMLOCK,		"K_NUMLOCK" },
											{ KVC_P_PLUS,		"K_P_PLUS" },
											{ KVC_P_MINUS,		"K_P_MINUS" },
											{ KVC_P_MULTIPLY,	"K_P_MULTIPLY" },
											{ KVC_P_DIVIDE,		"K_P_DIVIDE" },
											{ KVC_P_PERIOD,		"K_P_PERIOD" },
											{ KVC_P_ENTER,		"K_P_ENTER" },

											// 日本語キーボードのみ
											{ KVC_YEN,			"K_YEN" },
											{ KVC_RBRACKET,		"K_RBRACKET" },
											{ KVC_UNDERSCORE,	"K_UNDERSCORE" },
											{ KVC_MUHENKAN,		"K_MUHENKAN" },
											{ KVC_HENKAN,		"K_HENKAN" },
											{ KVC_HIRAGANA,		"K_HIRAGANA" },

											// 英語キーボードのみ
											{ KVE_BACKSLASH,	"K_BACKSLASH" },

											// 追加キー
											{ KVX_RMETA,		"K_RMETA" },
											{ KVX_LMETA,		"K_LMETA" },
											{ KVX_MENU,			"K_MENU" },
									   };


const VKeyConv CFG6::KeyIni[] = {	// 仮想キーコード -> P6キーコード定義初期値
									{ KVC_1,			KP6_1 },			// 1	!
									{ KVC_2,			KP6_2 },			// 2	"
									{ KVC_3,			KP6_3 },			// 3	#
									{ KVC_4,			KP6_4 },			// 4	$
									{ KVC_5,			KP6_5 },			// 5	%
									{ KVC_6,			KP6_6 },			// 6	&
									{ KVC_7,			KP6_7 },			// 7	'
									{ KVC_8,			KP6_8 },			// 8	(
									{ KVC_9,			KP6_9 },			// 9	)
									{ KVC_0,			KP6_0 },			// 0

									{ KVC_A,			KP6_A },			// a	A
									{ KVC_B,			KP6_B },			// b	B
									{ KVC_C,			KP6_C },			// c	C
									{ KVC_D,			KP6_D },			// d	D
									{ KVC_E,			KP6_E },			// e	E
									{ KVC_F,			KP6_F },			// f	F
									{ KVC_G,			KP6_G },			// g	G
									{ KVC_H,			KP6_H },			// h	H
									{ KVC_I,			KP6_I },			// i	I
									{ KVC_J,			KP6_J },			// j	J
									{ KVC_K,			KP6_K },			// k	K
									{ KVC_L,			KP6_L },			// l	L
									{ KVC_M,			KP6_M },			// m	M
									{ KVC_N,			KP6_N },			// n	N
									{ KVC_O,			KP6_O },			// o	O
									{ KVC_P,			KP6_P },			// p	P
									{ KVC_Q,			KP6_Q },			// q	Q
									{ KVC_R,			KP6_R },			// r	R
									{ KVC_S,			KP6_S },			// s	S
									{ KVC_T,			KP6_T },			// t	T
									{ KVC_U,			KP6_U },			// u	U
									{ KVC_V,			KP6_V },			// v	V
									{ KVC_W,			KP6_W },			// w	W
									{ KVC_X,			KP6_X },			// x	X
									{ KVC_Y,			KP6_Y },			// y	Y
									{ KVC_Z,			KP6_Z },			// z	Z

									{ KVC_F1,			KP6_F1 },			// F1
									{ KVC_F2,			KP6_F2 },			// F2
									{ KVC_F3,			KP6_F3 },			// F3
									{ KVC_F4,			KP6_F4 },			// F4
									{ KVC_F5,			KP6_F5 },			// F5

									{ KVC_MINUS,		KP6_MINUS },		// -	=
									{ KVC_CARET,		KP6_CARET },		// ^	~
									{ KVC_BACKSPACE,	KP6_DEL },			// BackSpace
									{ KVC_AT,			KP6_AT },			// @	`
									{ KVC_LBRACKET,		KP6_LBRACKET },		// [	{
									{ KVC_SEMICOLON,	KP6_SEMICOLON },	// ;	+
									{ KVC_COLON,		KP6_COLON },		// :	*
									{ KVC_COMMA,		KP6_COMMA },		// ,	<
									{ KVC_PERIOD,		KP6_PERIOD },		// .	>
									{ KVC_SLASH,		KP6_SLASH },		// /	?
									{ KVC_SPACE,		KP6_SPACE },		// Space

									{ KVC_ESC,			KP6_ESC },			// ESC
									{ KVC_HANZEN,		KP6_UNKNOWN },		// 半角/全角
									{ KVC_TAB,			KP6_TAB },			// Tab
									{ KVC_CAPSLOCK,		KP6_UNKNOWN },		// CapsLock
									{ KVC_ENTER,		KP6_RETURN },		// Enter
									{ KVC_LCTRL,		KP6_CTRL },			// L-Ctrl
									{ KVC_RCTRL,		KP6_CTRL },			// R-Ctrl
									{ KVC_LSHIFT,		KP6_SHIFT },		// L-Shift
									{ KVC_RSHIFT,		KP6_SHIFT },		// R-Shift
									{ KVC_LALT,			KP6_GRAPH },		// L-Alt
									{ KVC_RALT,			KP6_GRAPH },		// R-Alt
									{ KVC_PRINT,		KP6_UNKNOWN },		// PrintScreen
									{ KVC_SCROLLLOCK,	KP6_CAPS },			// ScrollLock
									{ KVC_PAUSE,		KP6_KANA },			// Pause
									{ KVC_INSERT,		KP6_INS },			// Insert
									{ KVC_DELETE,		KP6_DEL },			// Delete
									{ KVC_END,			KP6_STOP },			// End
									{ KVC_HOME,			KP6_HOME },			// Home
									{ KVC_PAGEUP,		KP6_PAGE },			// PageUp
									{ KVC_PAGEDOWN,		KP6_MODE },			// PageDown

									{ KVC_UP,			KP6_UP },			// ↑
									{ KVC_DOWN,			KP6_DOWN },			// ↓
									{ KVC_LEFT,			KP6_LEFT },			// ←
									{ KVC_RIGHT,		KP6_RIGHT },		// →

									{ KVC_P0,			KP6_P0 },			// [0]
									{ KVC_P1,			KP6_P1 },			// [1]
									{ KVC_P2,			KP6_P2 },			// [2]
									{ KVC_P3,			KP6_P3 },			// [3]
									{ KVC_P4,			KP6_P4 },			// [4]
									{ KVC_P5,			KP6_P5 },			// [5]
									{ KVC_P6,			KP6_P6 },			// [6]
									{ KVC_P7,			KP6_P7 },			// [7]
									{ KVC_P8,			KP6_P8 },			// [8]
									{ KVC_P9,			KP6_P9 },			// [9]
									{ KVC_NUMLOCK,		KP6_UNKNOWN },		// NumLock
									{ KVC_P_PLUS,		KP6_PPLUS },		// [+]
									{ KVC_P_MINUS,		KP6_PMINUS },		// [-]
									{ KVC_P_MULTIPLY,	KP6_PMULTIPLY },	// [*]
									{ KVC_P_DIVIDE,		KP6_PDIVIDE },		// [/]
									{ KVC_P_PERIOD,		KP6_PPERIOD },		// [.]
									{ KVC_P_ENTER,		KP6_PRETURN },		// [Enter]

									// 日本語キーボードのみ
									{ KVC_YEN,			KP6_YEN },			// ¥	|
									{ KVC_RBRACKET,		KP6_RBRACKET },		// ]	}
									{ KVC_UNDERSCORE,	KP6_UNDERSCORE },	// \	_
									{ KVC_MUHENKAN,		KP6_UNKNOWN },		// 無変換
									{ KVC_HENKAN,		KP6_UNKNOWN },		// 変換
									{ KVC_HIRAGANA,		KP6_KANA },     	// ひらがな

									// 英語キーボードのみ
									{ KVE_BACKSLASH,	KP6_YEN	 },			// 			BackSlash	|

									// 追加キー
									{ KVX_RMETA,		KP6_UNKNOWN },		// L-Meta
									{ KVX_LMETA,		KP6_UNKNOWN },		// R-Meta
									{ KVX_MENU,			KP6_UNKNOWN },		// Menu

									// 各種機能キー (今のところ無効)
									//	{ KVC_F6,			KFN_1 },			// F6
									//	{ KVC_F7,			KFN_2 },			// F7
									//	{ KVC_F8,			KFN_3 },			// F8
									//	{ KVC_F9,			KFN_4 },			// F9
									//	{ KVC_F10,			KFN_5 },			// F10
									//	{ KVC_F11,			KFN_6 },			// F11
									//	{ KVC_F12,			KFN_7 },			// F12

									{ KVC_LAST,			KP6_LAST }
								};


const COLOR24 CFG6::STDColor[] = {	// 標準カラーデータ ( R,G,B,0  0-255 )
									// システムカラー
									{   0,   0,   0, 0 },	// 00:
									{   0,   0, 128, 0 },	// 01:
									{   0, 128,   0, 0 },	// 02:
									{   0, 128, 128, 0 },	// 03:
									{ 128,   0,   0, 0 },	// 04:
									{ 128,   0, 128, 0 },	// 05:
									{ 128, 128,   0, 0 },	// 06:
									{ 128, 128, 128, 0 },	// 07:
									{  64,  64,  64, 0 },	// 08:
									{   0,   0, 255, 0 },	// 09:
									{   0, 255,   0, 0 },	// 10:
									{   0, 255, 255, 0 },	// 11:
									{ 255,   0,   0, 0 },	// 12:
									{ 255,   0, 255, 0 },	// 13:
									{ 255, 255,   0, 0 },	// 14:
									{ 255, 255, 255, 0 },	// 15:

									{  30,  30,  30, 0 },	// 16:黒(mode 1,2 ボーダー)

									// mode 1
									{  50, 238,  15, 0 },	// 17:緑(Set1)
									{   4, 106,   4, 0 },	// 18:深緑
									{ 252, 100,  35, 0 },	// 19:橙(Set2)
									{ 180,   6,   4, 0 },	// 20:深橙
									// mode 2
									{  50, 238,  15, 0 },	// 21:緑
									{ 234, 224,   4, 0 },	// 22:黄
									{  68,  10, 244, 0 },	// 23:青
									{ 226,   6,  12, 0 },	// 24:赤
									{ 214, 208, 246, 0 },	// 25:白
									{  52, 186, 236, 0 },	// 26:シアン
									{ 244,  10, 244, 0 },	// 27:マゼンタ
									{ 252,  90,   4, 0 },	// 28:橙
									// mode 3
									{  50, 238,  15, 0 },	// 29:緑
									{ 234, 224,   4, 0 },	// 30:黄
									{  68,  10, 244, 0 },	// 31:青
									{ 226,   6,  12, 0 },	// 32:赤
									{ 214, 208, 246, 0 },	// 33:白
									{  52, 186, 236, 0 },	// 34:シアン
									{ 244,  10, 244, 0 },	// 35:マゼンタ
									{ 252,  90,   4, 0 },	// 36:橙
									// mode 4
									{   4, 106,   4, 0 },	// 37:深緑(Set1)
									{  83, 242,  55, 0 },	// 38:緑
									{  30,  30,  30, 0 },	// 39:黒(Set2)
									{ 224, 255, 208, 0 },	// 40:白

									{ 146,  82,  13, 0 },	// 41:にじみ 赤(Set1)
									{  28, 202, 121, 0 },	// 42:にじみ 青(Set1)
									{ 120,  60,  95, 0 },	// 43:にじみ 桃(Set1)
									{  80, 190,  80, 0 },	// 44:にじみ 緑(Set1)

									{  81, 192,  14, 0 },	// 45:にじみ 明赤(Set1)
									{  69, 130,   9, 0 },	// 46:にじみ 暗赤(Set1)
									{  21, 221,  47, 0 },	// 47:にじみ 明青(Set1)
									{  14, 156,  50, 0 },	// 48:にじみ 暗青(Set1)

									{ 113, 210,  15, 0 },	// 49:にじみ 明桃(Set1)
									{  52, 132,  38, 0 },	// 50:にじみ 暗桃(Set1)
									{  70, 200,  60, 0 },	// 51:にじみ 明緑(Set1)
									{   4, 145,  47, 0 },	// 52:にじみ 暗緑(Set1)

									{ 255,  48,   0, 0 },	// 53:にじみ 赤(Set2)
									{  38, 201, 255, 0 },	// 54:にじみ 青(Set2)
									{ 255,  0,  236, 0 },	// 55:にじみ 桃(Set2)
									{  38, 255,  92, 0 },	// 56:にじみ 緑(Set2)

									{ 255, 140,  64, 0 },	// 57:にじみ 明赤(Set2)
									{ 150,  45,  00, 0 },	// 58:にじみ 暗赤(Set2)
									{ 119, 207, 255, 0 },	// 59:にじみ 明青(Set2)
									{  30,  80, 150, 0 },	// 60:にじみ 暗青(Set2)

									{ 255,  64, 131, 0 },	// 61:にじみ 明桃(Set2)
									{ 151,   0, 105, 0 },	// 62:にじみ 暗桃(Set2)
									{ 119, 255, 167, 0 },	// 63:にじみ 明緑(Set2)
									{  30, 151, 100, 0 },	// 64:にじみ 暗緑(Set2)

									// mk2
									{  20,  20,  20, 0 },	// 65:透明(黒)
									{ 255, 172,   0, 0 },	// 66:橙
									{   0, 255, 172, 0 },	// 67:青緑
									{ 172, 255,   0, 0 },	// 68:黄緑
									{ 172,   0, 255, 0 },	// 69:青紫
									{ 255,   0, 172, 0 },	// 70:赤紫
									{   0, 172, 255, 0 },	// 71:空色
									{ 172, 172, 172, 0 },	// 72:灰色
									{  20,  20,  20, 0 },	// 73:黒
									{ 255,   0,   0, 0 },	// 74:赤
									{   0, 255,   0, 0 },	// 75:緑
									{ 255, 255,   0, 0 },	// 76:黄
									{   0,   0, 255, 0 },	// 77:青
									{ 255,   0, 255, 0 },	// 78:マゼンタ
									{   0, 255, 255, 0 },	// 79:シアン
									{ 255, 255, 255, 0 },	// 80:白
								 };




////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
CFG6::CFG6( void ) : Ini(NULL)
  #ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  , MonDisp(false)
  #endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
{
	// INIファイルのパスを設定
	OSD_AddPath( IniPath, OSD_GetModulePath(), CONF_FILE );
	
	INITARRAY( Caption, '\0' );		// ウィンドウキャプション
	INITARRAY( DokoFile, '\0' );	// どこでもSAVEファイルパス
	INITARRAY( RomPath, '\0' );		// ROMパス
	INITARRAY( ExtRomPath, '\0' );	// 拡張ROMパス
	INITARRAY( ExtRomFile, '\0' );	// 拡張ROMファイル名
	INITARRAY( TapePath, '\0' );	// TAPEパス
	INITARRAY( TapeFile, '\0' );	// TAPEファイル名
	INITARRAY( SaveFile, '\0' );	// TAPE(SAVE)ファイル名
	INITARRAY( DiskPath, '\0' );	// DISKパス
	INITARRAY( DiskFile1, '\0' );	// DISK1ファイル名
	INITARRAY( DiskFile2, '\0' );	// DISK2ファイル名
	INITARRAY( ImgPath, '\0' );		// スクリーンショット格納パス
	INITARRAY( WavePath, '\0' );	// WAVE格納パス
	INITARRAY( PrinterFile, '\0' );	// プリンタファイル名
	INITARRAY( DokoSavePath, '\0' );// どこでもSAVEパス

}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
CFG6::~CFG6( void )
{
	if( Ini ){
		if( GetSaveQuit() ) Ini->Write();	// INIファイルに書込み
		delete Ini;
	}
}


////////////////////////////////////////////////////////////////
// 初期化(INIファイル読込み)
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool CFG6::Init( void )
{
	// INIオブジェクトが存在していたら一旦開放
	if( Ini ) delete Ini;
	
	try{
		// INIファイルがないならデフォルトで作成
		if( !OSD_FileExist( IniPath ) ){
			FILE *fp = FOPENEN( IniPath, "wt" );
			if( !fp ) throw Error::IniWriteFailed;
			// タイトル行を出力して一旦閉じる
			fprintf( fp, TRANS(MSINI_TITLE) );
			fclose( fp );
			
			// INIファイルを開く
			Ini = new cIni();
			if( !Ini->Init( IniPath ) ) throw Error::IniDefault;
			InitIni( Ini, true );	// INIオブジェクト初期値設定
			Ini->Write();
		}else{
			// INIファイルを開く
			Ini = new cIni();
			if( !Ini->Init( IniPath ) ) throw Error::IniDefault;
			InitIni( Ini, false );	// INIオブジェクト初期値設定
		}
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		if( Ini ) delete Ini;
		Ini = NULL;
		return false;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// INIファイル書込み
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool CFG6::Write( void )
{
	if( Ini ){
		// INIファイルに書込み
		return Ini->Write();
	}
	return false;
}


////////////////////////////////////////////////////////////////
// メンバアクセス関数
////////////////////////////////////////////////////////////////
// 機種取得
int CFG6::GetModel( void )
{
	int st = DEFAULT_MODEL;
	Ini->GetInt( "CONFIG", "Model", &st, st );
	return st;
}

// 機種設定
void CFG6::SetModel( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_Model, "Model", "%02d", data );
}

// ウィンドウキャプション取得
char *CFG6::GetCaption( void )
{
	switch( GetModel() ){	// 機種取得
	case 61: sprintf( Caption, APPNAME " (" P61NAME ") Ver." VERSION ); break;
	case 62: sprintf( Caption, APPNAME " (" P62NAME ") Ver." VERSION ); break;
	case 66: sprintf( Caption, APPNAME " (" P66NAME ") Ver." VERSION ); break;
	case 64: sprintf( Caption, APPNAME " (" P64NAME ") Ver." VERSION ); break;
	case 68: sprintf( Caption, APPNAME " (" P68NAME ") Ver." VERSION ); break;
	default: sprintf( Caption, APPNAME " (" P60NAME ") Ver." VERSION );
	}
	return Caption;
}

// どこでもSAVEファイル名取得
char *CFG6::GetDokoFile( void )
{
	return DokoFile;
}

// どこでもSAVEファイル名設定
void CFG6::SetDokoFile( const char *str )
{
	strncpy( DokoFile, str, PATH_MAX );
	OSD_DelDelimiter( DokoFile );
}

// オーバークロック率取得
int CFG6::GetOverClock( void )
{
	int st = 100;
	Ini->GetInt( "CONFIG", "OverClock", &st, st );
	return st;
}

// オーバークロック率設定
void CFG6::SetOverClock( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_OverClock, "OverClock", "%d", data );
}

// ROMパス取得
char *CFG6::GetRomPath( void )
{
	Ini->GetPath( "PATH", "RomPath", RomPath, RomPath );
	OSD_AddDelimiter( RomPath );
	return RomPath;
}

// ROMパス設定
void CFG6::SetRomPath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_RomPath, "RomPath", temp );
}

// 拡張RAMを使う取得
bool CFG6::GetUseExtRam( void )
{
	bool st = DEFAULT_EXTRAM;
	Ini->GetTruth( "CONFIG", "ExtRam", &st, st );
	return st;
}

// 拡張RAMを使う設定
void CFG6::SetUseExtRam( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_ExtRam, "ExtRam", "%s", yn ? "Yes" : "No" );
}

// 拡張ROMパス取得
char *CFG6::GetExtRomPath( void )
{
	Ini->GetPath( "PATH", "ExtRomPath", ExtRomPath, ExtRomPath );
	OSD_AddDelimiter( ExtRomPath );
	return ExtRomPath;
}
// 拡張ROMパス設定
void CFG6::SetExtRomPath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_ExtRomPath, "ExtRomPath", temp );
}

// 拡張ROMファイル名取得
char *CFG6::GetExtRomFile( void )
{
	Ini->GetPath( "FILES", "ExtRom", ExtRomFile, ExtRomFile );
	return ExtRomFile;
}

// 拡張ROMファイル名設定
void CFG6::SetExtRomFile( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_RelativePath( temp );
	Ini->PutEntry( "FILES", MSINI_ExtRom, "ExtRom", temp );
}

// CRCチェック取得
bool CFG6::GetCheckCRC( void )
{
	bool st = true;
	Ini->GetTruth( "CONFIG", "CheckCRC", &st, st );
	return st;
}
// CRCチェック設定
void CFG6::SetCheckCRC( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_CheckCRC, "CheckCRC", "%s", yn ? "Yes" : "No" );
}

// ROMパッチ取得
bool CFG6::GetRomPatch( void )
{
	bool st = true;
	Ini->GetTruth( "CONFIG", "RomPatch", &st, st );
	return st;
}
// ROMパッチ設定
void CFG6::SetRomPatch( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_RomPatch, "RomPatch", "%s", yn ? "Yes" : "No" );
}

// キーリピート取得
int CFG6::GetKeyRepeat( void )
{
	int st = DEFAULT_REPEAT;
	Ini->GetInt( "KEY", "KeyRepeat", &st, st );
	return st;
}

// キーリピート設定
void CFG6::SetKeyRepeat( int data )
{
	Ini->PutEntry( "KEY", MSINI_KeyRepeat, "KeyRepeat", "%d", data );
}

// サンプリングレート取得
int CFG6::GetSampleRate( void )
{
	int st = DEFAULT_SAMPLE_RATE;
	Ini->GetInt( "SOUND", "SampleRate", &st, st );
	return st;
}

// サンプリングレート設定
void CFG6::SetSampleRate( int data )
{
	Ini->PutEntry( "SOUND", MSINI_SampleRate, "SampleRate", "%d", data );
}

// サウンドバッファ長倍率取得
int CFG6::GetSoundBuffer( void )
{
	int st = SOUND_BUFFER_SIZE;
	Ini->GetInt( "SOUND", "SoundBuffer", &st, st );
	return st;
}

// サウンドバッファ長倍率設定
void CFG6::SetSoundBuffer( int data )
{
	Ini->PutEntry( "SOUND", MSINI_SoundBuffer, "SoundBuffer", "%d", data );
}

// マスター音量取得
int CFG6::GetMasterVol( void )
{
	int st = DEFAULT_MASTERVOL;
	Ini->GetInt( "SOUND", "MasterVolume", &st, st );
	return st;
}

// マスター音量設定
void CFG6::SetMasterVol( int data )
{
	Ini->PutEntry( "SOUND", MSINI_MasterVolume, "MasterVolume", "%d", data );
}


// PSG音量取得
int CFG6::GetPsgVol( void )
{
	int st = DEFAULT_PSGVOL;
	Ini->GetInt( "SOUND", "PsgVolume", &st, st );
	return st;
}

// PSG音量設定
void CFG6::SetPsgVol( int data )
{
	Ini->PutEntry( "SOUND", MSINI_PsgVolume, "PsgVolume", "%d", data );
}

// PSG LPFカットオフ周波数取得
int CFG6::GetPsgLPF( void )
{
	int st = DEFAULT_PSGLPF;
	Ini->GetInt( "SOUND", "PsgLPF", &st, st );
	return st;
}

// PSG LPFカットオフ周波数設定
void CFG6::SetPsgLPF( int data )
{
	Ini->PutEntry( "SOUND", MSINI_PsgLPF,  "PsgLPF",  "%d", data );
}

// WAVEパス取得
char *CFG6::GetWavePath( void )
{
	Ini->GetPath( "PATH", "WavePath", WavePath, WavePath );
	OSD_AddDelimiter( WavePath );
	return WavePath;
}

// WAVEパス設定
void CFG6::SetWavePath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_WavePath, "WavePath", temp );
}

// どこでもSAVEパス取得
char *CFG6::GetDokoSavePath( void )
{
	Ini->GetPath( "PATH", "DokoSavePath", DokoSavePath, DokoSavePath );
	OSD_AddDelimiter( DokoSavePath );
	return DokoSavePath;
}

// どこでもSAVEパス設定
void CFG6::SetDokoSavePath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_DokoSavePath, "DokoSavePath", temp );
}

// 音声合成音量取得
int CFG6::GetVoiceVol( void )
{
	int st = DEFAULT_VOICEVOL;
	Ini->GetInt( "SOUND", "VoiceVolume", &st, st );
	return st;
}

// 音声合成音量設定
void CFG6::SetVoiceVol( int data )
{
	Ini->PutEntry( "SOUND", MSINI_VoiceVolume, "VoiceVolume", "%d", data );
}

// TAPEパス取得
char *CFG6::GetTapePath( void )
{
	Ini->GetPath( "PATH", "TapePath", TapePath, TapePath );
	OSD_AddDelimiter( TapePath );
	return TapePath;
}

// TAPEパス設定
void CFG6::SetTapePath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_TapePath, "TapePath", temp );
}

// TAPEファイル名取得
char *CFG6::GetTapeFile( void )
{
	Ini->GetPath( "FILES", "tape", TapeFile, TapeFile );
	return TapeFile;
}

// TAPEファイル名設定
void CFG6::SetTapeFile( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "FILES", MSINI_tape, "tape", temp );
}

// TAPEモニタ音量取得
int CFG6::GetCmtVol( void )
{
	int st = DEFAULT_TAPEVOL;
	Ini->GetInt( "SOUND", "TapeVolume", &st, st );
	return st;
}

// TAPEモニタ音量設定
void CFG6::SetCmtVol( int data )
{
	Ini->PutEntry( "SOUND", MSINI_TapeVolume, "TapeVolume", "%d", data );
}

// Turbo TAPE 有効フラグ取得
bool CFG6::GetTurboTAPE( void )
{
	bool st = DEFAULT_TURBO;
	Ini->GetTruth( "CONFIG", "TurboTAPE", &st, st );
	return st;
}

// Turbo TAPE 有効フラグ設定
void CFG6::SetTurboTAPE( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_TurboTAPE, "TurboTAPE", "%s", yn ? "Yes" : "No" );
}

// Boost Up 有効フラグ取得
bool CFG6::GetBoostUp( void )
{
	bool st = DEFAULT_BOOST;
	Ini->GetTruth( "CONFIG", "BoostUp", &st, st );
	return st;
}

// BoostUp 有効フラグ設定
void CFG6::SetBoostUp( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_BoostUp, "BoostUp", "%s", yn ? "Yes" : "No" );
}

// BoostUp 最大倍率(N60モード)取得
int CFG6::GetMaxBoost1( void )
{
	int st = DEFAULT_MAXBOOST60;
	Ini->GetInt( "CONFIG", "MaxBoost60", &st, st );
	return st;
}

// BoostUp 最大倍率(N60モード)設定
void CFG6::SetMaxBoost1( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_MaxBoost60, "MaxBoost60", "%d", data );
}

// BoostUp 最大倍率(N60m/N66モード)取得
int CFG6::GetMaxBoost2( void )
{
	int st = DEFAULT_MAXBOOST62;
	Ini->GetInt( "CONFIG", "MaxBoost62", &st, st );
	return st;
}
// BoostUp 最大倍率(N60m/N66モード)設定
void CFG6::SetMaxBoost2( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_MaxBoost62, "MaxBoost62", "%d", data );
}

// TAPE(SAVE)ファイル名取得
char *CFG6::GetSaveFile( void )
{
	Ini->GetPath( "FILES", "save", SaveFile, SaveFile );
	return SaveFile;
}

// TAPE(SAVE)ファイル名設定
void CFG6::SetSaveFile( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "FILES", MSINI_save, "save", temp );
}

// DISKパス取得
char *CFG6::GetDiskPath( void )
{
	Ini->GetPath( "PATH", "DiskPath", DiskPath, DiskPath );
	OSD_AddDelimiter( DiskPath );
	return DiskPath;
}

// DISKパス設定
void CFG6::SetDiskPath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_DiskPath, "DiskPath", temp );
}

// DISKファイル名取得
char *CFG6::GetDiskFile( int drv )
{
	switch( drv ){
	case 1:	Ini->GetPath( "FILES", "disk1", DiskFile1, DiskFile1 );
		return DiskFile1;
	case 2: Ini->GetPath( "FILES", "disk2", DiskFile2, DiskFile2 );
		return DiskFile2;
	}
	return NULL;
}

// DISKファイル名設定
void CFG6::SetDiskFile( int drv, const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	switch( drv ){
	case 1: Ini->PutEntry( "FILES", MSINI_disk1, "disk1", temp ); break;
	case 2: Ini->PutEntry( "FILES", MSINI_disk2, "disk2", temp ); break;
	}
}

// FDD接続台数取得
int CFG6::GetFddNum( void )
{
	int st = DEFAULT_FDD;
	Ini->GetInt( "CONFIG", "FDD", &st, st );
	return st;
}

// FDD接続台数設定
void CFG6::SetFddNum( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_FDD, "FDD", "%d", data );
}

// FDDウェイト有効フラグ取得
bool CFG6::GetFddWaitEnable( void )
{
	bool st = DEFAULT_FDDWAIT;
	Ini->GetTruth( "CONFIG", "FDDWait", &st, st );
	return st;
}

// FDDウェイト有効フラグ設定
void CFG6::SetFddWaitEnable( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_FDDWait, "FDDWait", "%s", yn ? "Yes" : "No" );
}

// スクリーンショット格納パス取得
const char *CFG6::GetImgPath( void )
{
	Ini->GetPath( "PATH", "ImgPath", ImgPath, ImgPath );
	OSD_AddDelimiter( ImgPath );
	return ImgPath;
}

// スクリーンショット格納パス設定
void CFG6::SetImgPath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_ImgPath, "ImgPath", temp );
}


// モード4カラーモード取得
int CFG6::GetMode4Color( void ) const
{
	int st = DEFAULT_MODE4_COLOR;
	Ini->GetInt( "DISPLAY", "Mode4Color", &st, st );
	return st;
}

// モード4カラーモード設定
void CFG6::SetMode4Color( int data )
{
	Ini->PutEntry( "DISPLAY", MSINI_Mode4Color, "Mode4Color", "%d", data );
}

// スキャンライン取得
bool CFG6::GetScanLine( void ) const
{
	bool st = DEFAULT_SCANLINE;
	Ini->GetTruth( "DISPLAY", "ScanLine", &st, st );
	return st;
}

// スキャンライン設定
void CFG6::SetScanLine( bool yn )
{
	Ini->PutEntry( "DISPLAY", MSINI_ScanLine, "ScanLine", "%s", yn ? "Yes" : "No" );
}

// スキャンライン輝度取得
int CFG6::GetScanLineBr( void ) const
{
	int st = DEFAULT_SCANLINEBR;
	Ini->GetInt( "DISPLAY", "ScanLineBr", &st, st );
	return st;
}

// スキャンライン輝度設定
void CFG6::SetScanLineBr( int data )
{
	Ini->PutEntry( "DISPLAY", MSINI_ScanLineBr, "ScanLineBr", "%d", data );
}

// 4:3表示取得
bool CFG6::GetDispNTSC( void ) const
{
	bool st = DEFAULT_DISPNTSC;
	Ini->GetTruth( "DISPLAY", "DispNTSC", &st, st );
	return st;
}

// 4:3表示設定
void CFG6::SetDispNTSC( bool yn )
{
	Ini->PutEntry( "DISPLAY", MSINI_DispNTSC, "DispNTSC", "%s", yn ? "Yes" : "No" );
}

// フルスクリーン取得
bool CFG6::GetFullScreen( void ) const
{
	bool st = false;
	Ini->GetTruth( "DISPLAY", "FullScreen", &st, st );
	return st;
}

// フルスクリーン設定
void CFG6::SetFullScreen( bool yn )
{
	Ini->PutEntry( "DISPLAY", MSINI_FullScreen, "FullScreen", "%s", yn ? "Yes" : "No" );
}

// ステータスバー表示状態取得
bool CFG6::GetDispStat( void ) const
{
	bool st = true;
	Ini->GetTruth( "DISPLAY", "DispStatus", &st, st );
	return st;
}

// ステータスバー表示状態設定
void CFG6::SetDispStat( bool yn )
{
	Ini->PutEntry( "DISPLAY", MSINI_DispStatus, "DispStatus", "%s", yn ? "Yes" : "No" );
}

// フレームスキップ取得
int CFG6::GetFrameSkip( void ) const
{
	int st = DEFAULT_FRAMESKIP;
	Ini->GetInt( "DISPLAY", "FrameSkip", &st, st );
	return st;
}

// フレームスキップ設定
void CFG6::SetFrameSkip( int data )
{
	Ini->PutEntry( "DISPLAY", MSINI_FrameSkip, "FrameSkip", "%d", data );
}

// ビデオキャプチャ色深度取得
int CFG6::GetAviBpp()
{
	int st = 24;
	Ini->GetInt( "MOVIE", "AviBpp", &st, st );
	return st;
}

// ビデオキャプチャ色深度設定
void CFG6::SetAviBpp( int bpp )
{
	Ini->PutEntry( "MOVIE", MSINI_AviBpp, "AviBpp", "%d", bpp );
}

// プリンタファイル名取得
char *CFG6::GetPrinterFile( void )
{
	Ini->GetPath( "FILES", "printer", PrinterFile, PrinterFile );
	return PrinterFile;
}

// プリンタファイル名設定
void CFG6::SetPrinterFile( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "FILES", MSINI_printer, "printer", temp );
}

// 終了時確認取得
bool CFG6::GetCkQuit( void )
{
	bool st = false;
	Ini->GetTruth( "CHECK", "CkQuit", &st, st );
	return st;
}

// 終了時確認設定
void CFG6::SetCkQuit( bool yn )
{
	Ini->PutEntry( "CHECK", MSINI_CkQuit, "CkQuit", "%s", yn ? "Yes" : "No" );
}

// 終了時INI保存取得
bool CFG6::GetSaveQuit( void )
{
	bool st = false;
	Ini->GetTruth( "CHECK", "SaveQuit", &st, st );
	return st;
}

// 終了時INI保存設定
void CFG6::SetSaveQuit( bool yn )
{
	Ini->PutEntry( "CHECK", MSINI_SaveQuit, "SaveQuit", "%s", yn ? "Yes" : "No" );
}

// 戦士のカートリッジ使うフラグ取得
bool CFG6::GetUseSoldier()
{
	bool st = true;
	Ini->GetTruth( "OPTION", "UseSoldier", &st, st );
	return st;
}

// 戦士のカートリッジ使うフラグ設定
void CFG6::SetUseSoldier( bool yn )
{
	Ini->PutEntry( "OPTION", MSINI_UseSoldier, "UseSoldier", "%s", yn ? "Yes" : "No" );
}

// カラーデータ取得
COLOR24 *CFG6::GetColor( int num, COLOR24 *col )
{
	if( num < (int)(sizeof(STDColor)/sizeof(STDColor[0])) ){
		char strin[16], str[PATH_MAX];
		sprintf( strin, "COL%03d", num );
		sprintf( str,   "%02X%02X%02X", STDColor[num].r, STDColor[num].g, STDColor[num].b );
		Ini->GetString( "COLOR", strin, str, str );
		int st = strtol( str, NULL, 16 );
		col->r        = (st>>16)&0xff;
		col->g        = (st>> 8)&0xff;
		col->b        = (st    )&0xff;
		col->reserved = 0;
	}else{
		col->r        = 0;
		col->g        = 0;
		col->b        = 0;
		col->reserved = 0;
	}
	return col;
}

// カラーデータ設定
void CFG6::SetColor( int num, const COLOR24 *col )
{
	char stren[16];
	sprintf( stren, "COL%03d", num );
	if( col )
		Ini->PutEntry( "COLOR", OSD_ColorName( num-16 ), stren, "%02X%02X%02X", col->r, col->g, col->b );
}

// キー定義取得
P6KEYsym CFG6::GetVKey( PCKEYsym pcs )
{
	char str[PATH_MAX] = "";
	
	// キーコードから名称取得
	const char *k1 = GetPCKeyName( pcs );
	
	if( k1 ){
		Ini->GetString( "KEY", k1, str, str );
		return GetP6KeyCode( str );
	}
	return KP6_UNKNOWN;
}

// キー定義設定
void CFG6::SetVKey( PCKEYsym pcs, P6KEYsym p6s )
{
	const char *k1 = GetPCKeyName( pcs );
	const char *k2 = GetP6KeyName( p6s );
	if( k1 && k2 ){
		Ini->PutEntry( "KEY", OSD_KeyName( pcs ), k1, k2 );
	}
}

// キー定義配列取得
int CFG6::GetVKeyDef( VKeyConv **kdef )
{
	VKeyConv *key = NULL;
	
	try{
		// キー定義テーブル作成
		key = new VKeyConv[COUNTOF(KeyIni)];
		for( int i=0; i<COUNTOF(KeyIni); i++ ){
			key[i].PCKey = KeyIni[i].PCKey;
			key[i].P6Key = GetVKey( key[i].PCKey );
		}
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		*kdef = NULL;
		return 0;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		if( key ) delete [] key;
		*kdef = NULL;
		return 0;
	}
	
	*kdef = key;
	return COUNTOF(KeyIni);
}

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// モニタウィンドウ表示状態取得
bool CFG6::GetMonDisp( void ) const
{
	return MonDisp;
}

// モニタウィンドウ表示状態設定
void CFG6::SetMonDisp( bool yn )
{
	MonDisp = yn;
}


#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


////////////////////////////////////////////////////////////////
// INIオブジェクト初期値設定
//
// 引数:	ini		INIオブジェクトへのポインタ
//			over	true:上書き false:ノードが存在していたらパス
// 返値:	なし
////////////////////////////////////////////////////////////////
void CFG6::InitIni( cIni *ini, bool over )
{
	if( !ini ) return;
	
	char str[PATH_MAX];
	
	// [CONFIG]
	// 機種
	if( over || !ini->GetString( "CONFIG", "Model", str, str ) )
		SetModel( DEFAULT_MODEL );
	
	// FDD
	if( over || !ini->GetString( "CONFIG", "FDD", str, str ) )
		SetFddNum( DEFAULT_FDD );
	
	// 拡張RAM使用
	if( over || !ini->GetString( "CONFIG", "ExtRam", str, str ) )
		SetUseExtRam( DEFAULT_EXTRAM );
	
	// Turbo TAPE
	if( over || !ini->GetString( "CONFIG", "TurboTAPE", str, str ) )
		SetTurboTAPE( DEFAULT_TURBO );
	
	// Boost Up
	if( over || !ini->GetString( "CONFIG", "BoostUp", str, str ) )
		SetBoostUp( DEFAULT_BOOST );
	
	// BoostUp 最大倍率(N60モード)
	if( over || !ini->GetString( "CONFIG", "MaxBoost60", str, str ) )
		SetMaxBoost1( DEFAULT_MAXBOOST60 );
	
	// BoostUp 最大倍率(N60m/N66モード)
	if( over || !ini->GetString( "CONFIG", "MaxBoost62", str, str ) )
		SetMaxBoost2( DEFAULT_MAXBOOST62 );
	
	// オーバークロック率
	if( over || !ini->GetString( "CONFIG", "OverClock", str, str ) )
		SetOverClock( DEFAULT_OVERCLOCK );
	
	// CRCチェック
	if( over || !ini->GetString( "CONFIG", "CheckCRC", str, str ) )
		SetCheckCRC( DEFAULT_CHECKCRC );
	
	// ROMパッチ
	if( over || !ini->GetString( "CONFIG", "RomPatch", str, str ) )
		SetRomPatch( DEFAULT_ROMPATCH );
	
	// FDDウェイト有効フラグ
	if( over || !ini->GetString( "CONFIG", "FDDWait", str, str ) )
		SetFddWaitEnable( DEFAULT_FDDWAIT );
	
	// [DISPLAY]
	// MODE4カラー
	if( over || !ini->GetString( "DISPLAY", "Mode4Color", str, str ) )
		SetMode4Color( DEFAULT_MODE4_COLOR );
	
	// スキャンライン
	if( over || !ini->GetString( "DISPLAY", "ScanLine", str, str ) )
		SetScanLine( DEFAULT_SCANLINE );
	
	// スキャンライン輝度
	if( over || !ini->GetString( "DISPLAY", "ScanLineBr", str, str ) )
		SetScanLineBr( DEFAULT_SCANLINEBR );
	
	// 4:3表示
	if( over || !ini->GetString( "DISPLAY", "DispNTSC", str, str ) )
		SetDispNTSC( DEFAULT_DISPNTSC );
	
	// フルスクリーン
	if( over || !ini->GetString( "DISPLAY", "FullScreen", str, str ) )
		SetFullScreen( DEFAULT_FULLSCREEN );
	
	// ステータスバー表示状態
	if( over || !ini->GetString( "DISPLAY", "DispStatus", str, str ) )
		SetDispStat( DEFAULT_DISPSTATUS );
	
	// フレームスキップ
	if( over || !ini->GetString( "DISPLAY", "FrameSkip", str, str ) )
		SetFrameSkip( DEFAULT_FRAMESKIP );
	
	
	// [SOUND]
	// サンプリングレート
	if( over || !ini->GetString( "SOUND", "SampleRate", str, str ) )
		SetSampleRate( DEFAULT_SAMPLE_RATE );
	
	// サウンドバッファ長倍率
	if( over || !ini->GetString( "SOUND", "SoundBuffer", str, str ) )
		SetSoundBuffer( SOUND_BUFFER_SIZE );
	
	// マスター音量
	if( over || !ini->GetString( "SOUND", "MasterVolume", str, str ) )
		SetMasterVol( DEFAULT_MASTERVOL );
	
	// PSG音量
	if( over || !ini->GetString( "SOUND", "PsgVolume", str, str ) )
		SetPsgVol( DEFAULT_PSGVOL );
	
	// PSG LPFカットオフ周波数
	if( over || !ini->GetString( "SOUND", "PsgLPF", str, str ) )
		SetPsgLPF( DEFAULT_PSGLPF );
	
	// 音声合成音量
	if( over || !ini->GetString( "SOUND", "VoiceVolume", str, str ) )
		SetVoiceVol( DEFAULT_VOICEVOL );
	
	// TAPEモニタ音量
	if( over || !ini->GetString( "SOUND", "TapeVolume", str, str ) )
		SetCmtVol( DEFAULT_TAPEVOL );
	
	
	// [MOVIE]
	// ビデオキャプチャ色深度
	if( over || !ini->GetString( "MOVIE", "AviBpp", str, str ) )
		SetAviBpp( DEFAULT_AVIBPP );
	
	
	//[FILES]
	// 拡張ROMファイル名(起動時に自動マウント)
	if( over || !ini->GetString( "FILES", "ExtRom", str, str ) )
		SetExtRomFile( "" );
	
	// TAPEファイル名(起動時に自動マウント)
	if( over || !ini->GetString( "FILES", "tape", str, str ) )
		SetTapeFile( "" );
	
	// TAPE(SAVE)ファイル名(SAVE時に自動マウント)
	if( over || !ini->GetString( "FILES", "save", str, str ) ){
		OSD_AddPath( str, TAPE_DIR, SAVE_FILE );
		OSD_AddPath( str, OSD_GetModulePath(), (const char *)str );
		SetSaveFile( str );
	}
	
	// DISK1ファイル名(起動時に自動マウント)
	if( over || !ini->GetString( "FILES", "disk1", str, str ) )
		SetDiskFile( 1, "" );
	
	// DISK2ファイル名(起動時に自動マウント)
	if( over || !ini->GetString( "FILES", "disk2", str, str ) )
		SetDiskFile( 2, "" );
	
	// プリンタファイル名
	if( over || !ini->GetString( "FILES", "printer", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), PRINTER_FILE );
		SetPrinterFile( str );
	}
	
	// [PATH]
	// ROMパス
	if( over || !ini->GetString( "PATH", "RomPath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), ROM_DIR );
		SetRomPath( str );
	}
	
	// TAPEパス
	if( over || !ini->GetString( "PATH", "TapePath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), TAPE_DIR );
		SetTapePath( str );
	}
	
	// DISKパス
	if( over || !ini->GetString( "PATH", "DiskPath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), DISK_DIR );
		SetDiskPath( str );
	}
	
	// 拡張ROMパス
	if( over || !ini->GetString( "PATH", "ExtRomPath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), EXTROM_DIR );
		SetExtRomPath( str );
	}
	
	// WAVEパス
	if( over || !ini->GetString( "PATH", "WavePath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), WAVE_DIR );
		SetWavePath( str );
	}

	// IMGパス
	if( over || !ini->GetString( "PATH", "ImgPath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), IMAGE_DIR );
		SetImgPath( str );
	}
	
	// どこでもSAVEパス
	if( over || !ini->GetString( "PATH", "DokoSavePath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), DOKOSAVE_DIR );
		SetDokoSavePath( str );
	}

	// どこでもSAVEパス
	if( over || !ini->GetString( "PATH", "DokoSavePath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), DOKOSAVE_DIR );
		ini->PutEntry( "PATH",	MSINI_DokoSavePath,	"DokoSavePath",	str );
	}

	// [CHECK]
	// 終了時確認
	if( over || !ini->GetString( "CHECK", "CkQuit", str, str ) )
		SetCkQuit( DEFAULT_CKQUIT );
	
	// 終了時INI保存
	if( over || !ini->GetString( "CHECK", "SaveQuit", str, str ) )
		SetSaveQuit( DEFAULT_SAVEQUIT );
	
	
	// [OPTION]
	// 戦士のカートリッジ使うフラグ
	if( over || !ini->GetString( "OPTION", "UseSoldier", str, str ) )
		SetUseSoldier( DEFAULT_SOLDIER );
	
	
	// [COLOR]
	// パレット
	for( int i=16; i<COUNTOF(STDColor); i++ ){
		char stren[16];
		sprintf( stren, "COL%03d", i );
		if( over || !ini->GetString( "COLOR", stren, str, str ) )
			SetColor( i, &STDColor[i] );
	}
	
	// [KEY]
	// キーリピート
	if( over || !ini->GetString( "KEY", "KeyRepeat", str, str ) )
		SetKeyRepeat( DEFAULT_REPEAT );
	
	// キー定義
	int i = 0;
	while( KeyIni[i].PCKey != KVC_LAST ){
		// キーコードから名称取得
		const char *k1 = GetPCKeyName( KeyIni[i].PCKey );
		if( k1 && ( over || !ini->GetString( "KEY", k1, str, str ) ) )
			SetVKey( KeyIni[i].PCKey, KeyIni[i].P6Key );
		i++;
	}
	
}


////////////////////////////////////////////////////////////////
// 仮想キーコードから名称取得
//
// 引数:	sym		仮想キーコード
// 返値:	char *	名称文字列へのポインタ(見つからなければNULL)
////////////////////////////////////////////////////////////////
const char *CFG6::GetPCKeyName( PCKEYsym sym )
{
	const char *str = NULL;
	for( int i=0; i<(int)(sizeof(PCKeyNameDef)/sizeof(PCKeyName)); i++ ){
		if( PCKeyNameDef[i].PCKey == sym ){
			str = PCKeyNameDef[i].Name;
			break;
		}
	}
	return str;
}


////////////////////////////////////////////////////////////////
// P6キーコードから名称取得
//
// 引数:	sym		P6キーコード
// 返値:	char *	名称文字列へのポインタ(見つからなければNULL)
////////////////////////////////////////////////////////////////
const char *CFG6::GetP6KeyName( P6KEYsym sym )
{
	const char *str = NULL;
	for( int i=0; i<(int)(sizeof(P6KeyNameDef)/sizeof(P6KeyName)); i++ ){
		if( P6KeyNameDef[i].P6Key == sym ){
			str = P6KeyNameDef[i].Name;
			break;
		}
	}
	return str;
}


////////////////////////////////////////////////////////////////
// キー名称から仮想キーコードを取得
//
// 引数:	str			名称文字列へのポインタ
// 返値:	PCKEYsym	仮想キーコード
////////////////////////////////////////////////////////////////
PCKEYsym CFG6::GetPCKeyCode( char *str )
{
	PCKEYsym sym = KVC_UNKNOWN;
	for( int i=0; i<(int)(sizeof(PCKeyNameDef)/sizeof(PCKeyName)); i++ ){
		if( !stricmp( PCKeyNameDef[i].Name, str ) ){
			sym = PCKeyNameDef[i].PCKey;
			break;
		}
	}
	return sym;
}


////////////////////////////////////////////////////////////////
// キー名称からP6キーコードを取得
//
// 引数:	str			名称文字列へのポインタ
// 返値:	P6KEYsym	P6キーコード
////////////////////////////////////////////////////////////////
P6KEYsym CFG6::GetP6KeyCode( char *str )
{
	P6KEYsym sym = KP6_UNKNOWN;
	for( int i=0; i<(int)(sizeof(P6KeyNameDef)/sizeof(P6KeyName)); i++ ){
		if( !stricmp( P6KeyNameDef[i].Name, str ) ){
			sym = P6KeyNameDef[i].P6Key;
			break;
		}
	}
	return sym;
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool CFG6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	// 共通
	Ini->PutEntry( "GLOBAL", NULL, "Version",		VERSION );
	Ini->PutEntry( "GLOBAL", NULL, "Model",			"%02d",	GetModel() );
	Ini->PutEntry( "GLOBAL", NULL, "FDD",			"%d",	GetFddNum() );
	Ini->PutEntry( "GLOBAL", NULL, "ExtRam",		"%s",	GetUseExtRam()  ? "Yes" : "No" );
	Ini->PutEntry( "GLOBAL", NULL, "RomPatch",		"%s",	GetRomPatch()   ? "Yes" : "No" );
	// OPTION
	Ini->PutEntry( "OPTION", NULL, "UseSoldier",	"%s",	GetUseSoldier() ? "Yes" : "No" );
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool CFG6::DokoLoad( cIni *Ini )
{
	int st;
	bool yn;
	char strva[256];
	
	if( !Ini ) return false;
	
	// 共通
	Ini->GetString( "GLOBAL", "Version", strva, "" );
	if( strcmp( strva, VERSION ) ){
		Error::SetError( Error::DokoDiffVersion );
		return false;
	}
	
	Ini->GetInt(   "GLOBAL", "Model",      &st, GetModel() );		SetModel( st );
	Ini->GetInt(   "GLOBAL", "FDD",        &st, GetFddNum() );		SetFddNum( st );
	Ini->GetTruth( "GLOBAL", "ExtRam",     &yn, GetUseExtRam() );	SetUseExtRam( yn );
	Ini->GetTruth( "GLOBAL", "RomPatch",   &yn, GetRomPatch() );	SetRomPatch( yn );
	Ini->GetTruth( "OPTION", "UseSoldier", &yn, GetUseSoldier() );	SetUseSoldier( yn );
	
	return true;
}
