/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include <string>

#include "typedef.h"
#include "keydef.h"


/////////////////////////////////////////////////////////////////////////////
// イベント定義
// ほぼSDL準拠
/////////////////////////////////////////////////////////////////////////////
// イベントID
typedef enum {
	EV_RESTART = 0,				// (SDL User event)
	EV_DOKOLOAD,				// (SDL User event)
	EV_REPLAYPLAY,				// (SDL User event)
	EV_REPLAYRESUME,			// (SDL User event)
	EV_REPLAYMOVIE,				// (SDL User event)
	EV_FPSUPDATE,				// (SDL User event)
	EV_DEBUGMODEBP,				// (SDL User event)
	EV_DEBUGMODETOGGLE,			// (SDL User event)
	EV_RENDER,					// (SDL User event)
	EV_CAPTURE,					// (SDL User event)
	
								// Fix order
	EV_QUIT,					// User-requested quit
	EV_DROPFILE,				// File dropped
	EV_KEYDOWN,					// Keys pressed
	EV_KEYUP,					// Keys released
	EV_MOUSEMOTION,				// Mouse moved
	EV_MOUSEBUTTONDOWN,			// Mouse button pressed
	EV_MOUSEBUTTONUP,			// Mouse button released
	EV_MOUSEWHEEL,				// Mouse wheel motion
	EV_JOYAXISMOTION,			// Joystick axis motion
	EV_JOYBUTTONDOWN,			// Joystick button pressed
	EV_JOYBUTTONUP,				// Joystick button released
	EV_JOYDEVICEADDED,			// Joystick added
	EV_JOYDEVICEREMOVED,		// Joystick removed
	EV_WINDOWRESIZED,			// Window resized
	EV_WINDOWSIZECHANGED,		// Window size changed
	EV_WINDOWEVENT_MINIMIZED,	// Window minimized
	EV_WINDOWEVENT_MAXIMIZED,	// Window maximized
	EV_WINDOWEVENT_RESTORED,	// Window restored to normal size and position
	
	EV_NOEVENT
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
	int32_t x, y;		// The X/Y coordinates of the mouse at press time
} Event_MouseButton;

// マウスホイール
typedef struct {
	EventType type;		// EV_MOUSEWHEEL
	int32_t x;			// the amount scrolled horizontally +:away from the user -:toward the user
	int32_t y;			// the amount scrolled vertically   +:right              -:left
} Event_MouseWheel;

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

// ジョイスティック 接続
typedef struct {
	EventType type;		// SDL_JOYDEVICEADDED or SDL_JOYDEVICEREMOVED
	uint8_t idx;		// The joystick device index
} Event_JoyDevice;

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

// リプレイ再生
typedef struct {
	EventType type;		// EV_REPLAYPLAY
} Event_ReplayPlay;

// リプレイ保存再開
typedef struct {
	EventType type;		// EV_REPLAYRESUME
} Event_ReplayResume;

// リプレイを動画に変換
typedef struct {
	EventType type;		// EV_REPLAYMOVIE
} Event_ReplayMovie;

// FPS表示
typedef struct {
	EventType type;		// EV_FPSUPDATE
} Event_FPSUpdate;

// ブレークポイント
typedef struct {
	EventType type;		// EV_DEBUGMODEBP
	uint16_t addr;
} Event_BreakPoint;

// モニタモードトグル
typedef struct {
	EventType type;		// EV_DEBUGMODETOGGLE
} Event_MonitorMode;

// 画面描画
typedef struct {
	EventType type;		// EV_RENDER
} Event_Render;

// ビデオキャプチャ
typedef struct {
	EventType type;		// EV_CAPTURE
} Event_Capture;

// Drag & Drop
typedef struct {
	EventType type;		// EV_DROPFILE
	char *file;			// イベント処理後に必ずメモリを開放する
} Event_Drop;

// ウィンドウ
typedef struct {
	EventType type;		// EV_WINDOWSIZECHANGED
	int32_t w, h;		// Window size
} Event_Window;


// イベント情報共用体
typedef union Event {
	EventType type;
	Event_Keyboard key;
	Event_MouseButton mousebt;
	Event_MouseWheel mousewh;
	Event_JoyAxis joyax;
	Event_JoyButton joybt;
	Event_JoyDevice joydev;
	Event_Quit quit;
	Event_Restart restart;
	Event_DOKOLoad doko;
	Event_ReplayPlay replay;
	Event_ReplayResume resume;
	Event_FPSUpdate fps;
	Event_BreakPoint bp;
	Event_MonitorMode mon;
	Event_Render render;
	Event_Capture capture;
	Event_Drop drop;
	Event_Window window;
} Event;


#endif	// EVENT_H_INCLUDED
