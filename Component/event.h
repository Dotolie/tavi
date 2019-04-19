/*
 * $Id: event.h,v 1.1.1.1 2006/02/22 12:33:31 zstein Exp $ - event.h
 * 
 * Copyright(C) 2004 New Media Life Inc.
 * 
 * Declare event structure.
 *
 * Author: Lee Seok<zstein@newmeidalife.com>
 * Date: 12/13/2004
 * Update:
 * History:
 */

#ifndef __EVENT_H__
#define __EVENT_H__

typedef struct {
	int value;
	#define KEY_RELEASED	0
	#define KEY_PRESSED		1
	int pressed;
} KeyEvent;
typedef struct {
	int x;
	int tapping;
	#define TOUCHPAD_MODE_ABOSULTE		0
	#define TOUCHPAD_MODE_RELATIVE		1
	int mode;
} TouchEvent;
typedef struct {
	int percent;
	char amp;
} PowerEvent;
typedef struct {
	int x;
	int y;
	int click;
} MouseEvent;
typedef struct {
	int x1, y1;
	int x2, y2;
} PaintEvent;

// Window Messages
#define WINDOW_KILL				0x00010000
#define WINDOW_KILL_FOCUS			0x00020000
#define WINDOW_SET_FOCUS			0x00040000
#define WINDOW_SHOW				0x00080000
#define WINDOW_HIDE				0x00100000
#define WINDOW_QUEUE				0x00200000
#define WINDOW_IDLE				0x00300000
#define WINDOW_KILL_TASK			0x00400000
#define WINDOW_SET_BG				0x00500000
#define WINDOW_TASK_SWITCH		0x00600000
#define WINDOW_TASK_KILL_AND_SWITCH		0x00700000
#define WINDOW_NOTIFY				0x00800000
#define WINDOW_USER_MESSAGE		0x10000000
#define ON_FOLDER					0x40000000

typedef struct {
	int param1;
	int param2;
} NotifyEvent;
typedef struct {
	int nWindowID;
	int nMessage;
	int nParam;
	NotifyEvent noti;
} WindowEvent;
typedef struct {
	int nWindowID;
	int nTimerID;
} TimerEvent;
typedef struct Event {
#define EVENT_KEY					( 1<<0 )
#define EVENT_MOUSE				( 1<<1 )
#define EVENT_POWER				( 1<<2 )
#define EVENT_PAINT					( 1<<3 )
#define EVENT_WINDOW				( 1<<4 )
#define EVENT_TIMER					( 1<<5 )
#define EVENT_IDLE					( 1<<6 )
	int type;
	union {
		KeyEvent key;
		MouseEvent mouse;
		PaintEvent paint;
		WindowEvent window;
		PowerEvent power;
		TimerEvent timer;
	} u;
} CEvent;

enum {
	VK_0=0x0130,
	VK_1,
	VK_2,
	VK_3,
	VK_4,
	VK_5,
	VK_6,
	VK_7,
	VK_8,
	VK_9,
	VK_A=0x0141,
	VK_B,
	VK_C,
	VK_D,
	VK_E,
	VK_F,
	VK_G,
	VK_H,
	VK_I,
	VK_J,
	VK_K,
	VK_L,
	VK_M,
	VK_N,
	VK_O,
	VK_P,
	VK_Q,
	VK_R,
	VK_S,
	VK_T,
	VK_U,
	VK_V,
	VK_W,
	VK_X,
	VK_Y,
	VK_Z,
	VK_LEFT=0x4001,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,
	VK_ENTER,
	VK_F1, // EQ
	VK_F2, // Menu
	VK_F3, // Mode
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
	VK_POWER,
	VK_ESC,
	// extended key
	VK_EXT_F1,
	VK_EXT_F2,
	VK_EXT_F3,
	VK_EXT_F4,
	VK_EXT_F5,
	VK_EXT_F6,
	VK_EXT_F7,
	VK_EXT_F8,
	VK_EXT_F9,
	VK_EXT_F10,
	VK_EXT_F11,
	VK_EXT_F12,
	VK_EXT_ESC,
	VK_META_ENTER=0x5001,
	VK_META_LEFT,
	VK_META_RIGHT,
	VK_META_UP,
	VK_META_DOWN,
};


#endif /* __EVENT_H__ */
