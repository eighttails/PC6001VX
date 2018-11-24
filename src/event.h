#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "typedef.h"

#include "keydef.h"


/////////////////////////////////////////////////////////////////////////////
// イベント定義
// ほぼSDL準拠
/////////////////////////////////////////////////////////////////////////////
// イベントID
typedef enum {
	EV_NOEVENT = 0,			// Unused (do not remove)
	EV_KEYDOWN,				// Keys pressed
	EV_KEYUP,				// Keys released
	EV_MOUSEBUTTONDOWN,		// Mouse button pressed
	EV_MOUSEBUTTONUP,		// Mouse button released
	EV_JOYAXISMOTION,		// Joystick axis motion
	EV_JOYBUTTONDOWN,		// Joystick button pressed
	EV_JOYBUTTONUP,			// Joystick button released
	EV_QUIT,				// User-requested quit
	EV_RESTART,
	EV_DOKOLOAD,
	EV_REPLAY,
	EV_FPSUPDATE,
	EV_DEBUGMODEBP,
	EV_DROPFILE,
	EV_CONTEXTMENU,
	EV_REPLAYMOVIE,
	EndofEvent
} EventType;


// キーボード
typedef struct {
	EventType type;		// EV_KEYDOWN or EV_KEYUP
	bool state;			// true:ON false:OFF
	PCKEYsym sym;		// virtual keysym
	PCKEYmod mod;		// current key modifiers
	uint16_t unicode;	// translated character
} Event_Keyboard;

// マウスボタン
typedef struct {
	EventType type;		// EV_MOUSEBUTTONDOWN or EV_MOUSEBUTTONUP
	MouseButton button;	// The mouse button index
	bool state;			// true:ON false:OFF
	uint16_t x, y;		// The X/Y coordinates of the mouse at press time
} Event_MouseButton;

// ジョイスティック 軸
typedef struct {
	EventType type;		// EV_JOYAXISMOTION
	uint8_t idx;		// The joystick device index
	uint8_t axis;		// The joystick axis index
	int16_t value;		// The axis value (range: -32768 to 32767)
} Event_JoyAxis;

// ジョイスティック ボタン
typedef struct {
	EventType type;		// EV_JOYBUTTONDOWN or EV_JOYBUTTONUP
	uint8_t idx;		// The joystick device index
	uint8_t button;		// The joystick button index
	bool state;			// true:ON false:OFF
} Event_JoyButton;

// 終了
typedef struct {
	EventType type;		// EV_QUIT
} Event_Quit;

// 再起動
typedef struct {
	EventType type;		// EV_RESTART
} Event_Restart;

// どこでもLOAD
typedef struct {
	EventType type;		// EV_DOKOLOAD
} Event_DOKOLoad;

// リプレイ
typedef struct {
	EventType type;		// EV_REPLAY
} Event_Replay;

// FPS表示
typedef struct {
	EventType type;		// EV_FPSUPDATE
	int fps;
} Event_FPSUpdate;

// ブレークポイント
typedef struct {
	EventType type;		// EV_DEBUGMODEBP
	uint16_t addr;
} Event_BreakPoint;

// Drag & Drop
typedef struct {
	EventType type;		// EV_DROPFILE
	char *file;			// イベント処理後に必ずメモリを開放する
} Event_Drop;

// イベント情報共用体
typedef union Event {
	EventType type;
	Event_Keyboard key;
	Event_MouseButton mousebt;
	Event_JoyAxis joyax;
	Event_JoyButton joybt;
	Event_Quit quit;
	Event_Restart restart;
	Event_DOKOLoad doko;
	Event_Replay replay;
	Event_FPSUpdate fps;
	Event_BreakPoint bp;
	Event_Drop drop;
} Event;


#endif	// EVENT_H_INCLUDED
