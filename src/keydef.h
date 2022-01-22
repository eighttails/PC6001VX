/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef KEYDEF_H_INCLUDED
#define KEYDEF_H_INCLUDED

#include <string>
#include "typedef.h"

// P6キーコード定義
typedef enum {
	KP6_UNKNOWN		= 0,
	
	KP6_1			= 1,
	KP6_2			= 2,
	KP6_3			= 3,
	KP6_4			= 4,
	KP6_5			= 5,
	KP6_6			= 6,
	KP6_7			= 7,
	KP6_8			= 8,
	KP6_9			= 9,
	KP6_0			= 10,
	
	KP6_A			= 11,
	KP6_B			= 12,
	KP6_C			= 13,
	KP6_D			= 14,
	KP6_E			= 15,
	KP6_F			= 16,
	KP6_G			= 17,
	KP6_H			= 18,
	KP6_I			= 19,
	KP6_J			= 20,
	KP6_K			= 21,
	KP6_L			= 22,
	KP6_M			= 23,
	KP6_N			= 24,
	KP6_O			= 25,
	KP6_P			= 26,
	KP6_Q			= 27,
	KP6_R			= 28,
	KP6_S			= 29,
	KP6_T			= 30,
	KP6_U			= 31,
	KP6_V			= 32,
	KP6_W			= 33,
	KP6_X			= 34,
	KP6_Y			= 35,
	KP6_Z			= 36,
	
	KP6_F1			= 37,
	KP6_F2			= 38,
	KP6_F3			= 39,
	KP6_F4			= 40,
	KP6_F5			= 41,
	
	KP6_MINUS		= 42,
	KP6_CARET		= 43,
	KP6_YEN			= 44,
	KP6_AT			= 45,
	KP6_LBRACKET	= 46,
	KP6_RBRACKET	= 47,
	KP6_SEMICOLON	= 48,
	KP6_COLON		= 49,
	KP6_COMMA		= 50,
	KP6_PERIOD		= 51,
	KP6_SLASH		= 52,
	KP6_UNDERSCORE	= 53,
	KP6_SPACE		= 54,
	
	KP6_ESC			= 55,
	KP6_TAB			= 56,
	KP6_CTRL		= 57,
	KP6_SHIFT		= 58,
	KP6_GRAPH		= 59,
	KP6_HOME		= 60,
	KP6_STOP		= 61,
	KP6_PAGE		= 62,
	KP6_RETURN		= 63,
	KP6_KANA		= 64,
	KP6_INS			= 65,
	KP6_DEL			= 66,
	
	KP6_UP			= 67,
	KP6_DOWN		= 68,
	KP6_LEFT		= 69,
	KP6_RIGHT		= 70,
	
	KP6_MODE		= 71,
	KP6_CAPS		= 72,
	
	
	// テンキー部拡張
	KP6_P0			= 73,
	KP6_P1			= 74,
	KP6_P2			= 75,
	KP6_P3			= 76,
	KP6_P4			= 77,
	KP6_P5			= 78,
	KP6_P6			= 79,
	KP6_P7			= 80,
	KP6_P8			= 81,
	KP6_P9			= 82,
	KP6_PPLUS		= 83,
	KP6_PMINUS		= 84,
	KP6_PMULTIPLY	= 85,
	KP6_PDIVIDE		= 86,
	KP6_PPERIOD		= 87,
	KP6_PRETURN		= 88,
	
	
	// 各種機能キー
	KFN_1			= 89,
	KFN_2			= 90,
	KFN_3			= 91,
	KFN_4			= 92,
	KFN_5			= 93,
	KFN_6			= 94,
	KFN_7			= 95,
	KFN_8			= 96,
	KFN_9			= 97,
	
	KP6_LAST
} P6KEYsym;

// 仮想キーコード定義
typedef enum {
	KVC_UNKNOWN	= 0,
	
	KVC_1			= 1,
	KVC_2			= 2,
	KVC_3			= 3,
	KVC_4			= 4,
	KVC_5			= 5,
	KVC_6			= 6,
	KVC_7			= 7,
	KVC_8			= 8,
	KVC_9			= 9,
	KVC_0			= 10,
	
	KVC_A			= 11,
	KVC_B			= 12,
	KVC_C			= 13,
	KVC_D			= 14,
	KVC_E			= 15,
	KVC_F			= 16,
	KVC_G			= 17,
	KVC_H			= 18,
	KVC_I			= 19,
	KVC_J			= 20,
	KVC_K			= 21,
	KVC_L			= 22,
	KVC_M			= 23,
	KVC_N			= 24,
	KVC_O			= 25,
	KVC_P			= 26,
	KVC_Q			= 27,
	KVC_R			= 28,
	KVC_S			= 29,
	KVC_T			= 30,
	KVC_U			= 31,
	KVC_V			= 32,
	KVC_W			= 33,
	KVC_X			= 34,
	KVC_Y			= 35,
	KVC_Z			= 36,
	
	KVC_F1			= 37,
	KVC_F2			= 38,
	KVC_F3			= 39,
	KVC_F4			= 40,
	KVC_F5			= 41,
	KVC_F6			= 42,
	KVC_F7			= 43,
	KVC_F8			= 44,
	KVC_F9			= 45,
	KVC_F10			= 46,
	KVC_F11			= 47,
	KVC_F12			= 48,
	
	KVC_MINUS		= 49,
	KVC_CARET		= 50,
	KVC_BACKSPACE	= 51,
	KVC_AT			= 52,
	KVC_LBRACKET	= 53,
	KVC_SEMICOLON	= 54,
	KVC_COLON		= 55,
	KVC_COMMA		= 56,
	KVC_PERIOD		= 57,
	KVC_SLASH		= 58,
	KVC_SPACE		= 59,
	
	KVC_ESC			= 60,
	KVC_HANZEN		= 61,
	KVC_TAB			= 62,
	KVC_CAPSLOCK	= 63,
	KVC_ENTER		= 64,
	KVC_LCTRL		= 65,
	KVC_RCTRL		= 66,
	KVC_LSHIFT		= 67,
	KVC_RSHIFT		= 68,
	KVC_LALT		= 69,
	KVC_RALT		= 70,
	KVC_PRINT		= 81,
	KVC_SCROLLLOCK	= 82,
	KVC_PAUSE		= 83,
	KVC_INSERT		= 84,
	KVC_DELETE		= 85,
	KVC_HOME		= 86,
	KVC_END			= 87,
	KVC_PAGEUP		= 88,
	KVC_PAGEDOWN	= 89,
	
	KVC_UP			= 90,
	KVC_DOWN		= 91,
	KVC_LEFT		= 92,
	KVC_RIGHT		= 93,
	
	KVC_P0			= 94,
	KVC_P1			= 95,
	KVC_P2			= 96,
	KVC_P3			= 97,
	KVC_P4			= 98,
	KVC_P5			= 99,
	KVC_P6			= 100,
	KVC_P7			= 101,
	KVC_P8			= 102,
	KVC_P9			= 103,
	KVC_NUMLOCK		= 104,
	KVC_P_PLUS		= 105,
	KVC_P_MINUS		= 106,
	KVC_P_MULTIPLY	= 107,
	KVC_P_DIVIDE	= 108,
	KVC_P_PERIOD	= 109,
	KVC_P_ENTER		= 110,
	
	// 日本語キーボードのみ
	KVC_YEN			= 111,
	KVC_RBRACKET	= 112,
	KVC_UNDERSCORE	= 113,
	KVC_MUHENKAN	= 114,
	KVC_HENKAN		= 115,
	KVC_HIRAGANA	= 116,
	
	// 英語キーボードのみ
	KVE_BACKSLASH	= 117,
	
	// 追加キー
	KVX_RMETA		= 118,
	KVX_LMETA		= 119,
	KVX_MENU		= 120,
	
	KVC_LAST
} PCKEYsym;

// 仮想キーモディファイア定義
typedef enum {
	KVM_NONE	= 0x0000,
	KVM_LSHIFT	= 0x0001,
	KVM_RSHIFT	= 0x0002,
	KVM_LCTRL	= 0x0040,
	KVM_RCTRL	= 0x0080,
	KVM_LALT	= 0x0100,
	KVM_RALT	= 0x0200,
	KVM_LMETA	= 0x0400,
	KVM_RMETA	= 0x0800,
	KVM_NUM		= 0x1000,
	KVM_CAPS	= 0x2000,
	KVM_MODE	= 0x4000,
} PCKEYmod;

#define KVM_CTRL	(KVM_LCTRL|KVM_RCTRL)
#define KVM_SHIFT	(KVM_LSHIFT|KVM_RSHIFT)
#define KVM_ALT		(KVM_LALT|KVM_RALT)
#define KVM_META	(KVM_LMETA|KVM_RMETA)


// 仮想キーコード -> P6キーコード定義
typedef struct {
	PCKEYsym PCKey;		// 仮想キーコード
	P6KEYsym P6Key;		// P6キーコード
} VKeyConv;


// 仮想キーコード 名称定義
typedef struct {
	PCKEYsym Key;		// 仮想キーコード
	std::string Name;	// キー名
} PCKeyName;


// P6キーコード 名称定義
typedef struct {
	P6KEYsym Key;		// P6キーコード
	std::string Name;	// キー名
} P6KeyName;


// P6キーコード -> P6キーマトリクス定義
typedef struct {
	P6KEYsym P6Key;		// P6キーコード
	BYTE Mat;			// P6のキーマトリクス(bit7-4:Y bit3-0:X)
} P6KeyMatrix;


// マウスボタン定義
typedef enum {
	MBT_NONE		= 0,
	MBT_LEFT		= 1,
	MBT_MIDDLE		= 2,
	MBT_RIGHT		= 3
} MouseButton;



#endif	// KEYDEF_H_INCLUDED
