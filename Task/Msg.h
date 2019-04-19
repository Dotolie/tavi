//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : Tavi Message Process Headers
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Msg.h,v $
// $Revision: 1.3 $
// $Date: 2006/04/15 01:32:29 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: Msg.h,v $
// Revision 1.3  2006/04/15 01:32:29  zstein
// - changed auto scroll values of textviewer.
//
// Revision 1.2  2006/03/09 02:06:34  zstein
// - improved volume interface
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.38  2006/02/10 07:56:42  ywkim
// Remove  __ATTRIBUTE_PACKED__  on UsbStrgMsgBuf_t
//
// Revision 1.37  2005/12/25 16:40:41  yong
// remove excessive call of gettime_sec()
//
// Revision 1.36  2005/12/22 05:21:06  etnlwind
// Update for New Version
//
// Revision 1.35  2005/12/20 15:30:30  yong
// add declaration of MSG_Peek_Ex(), more finer control than MSG_Peek()
//
// Revision 1.34  2005/12/09 11:05:55  zstein
// *** empty log message ***
//
// Revision 1.33  2005/12/05 08:30:21  ywkim
// Add definition of MSG_EXECUTE_TASK
//
// Revision 1.32  2005/11/24 10:56:48  etnlwind
// Update for New EQ
//
// Revision 1.31  2005/11/23 09:33:54  etnlwind
// Update for TAVI-EQ
//
// Revision 1.30  2005/11/17 04:25:13  zstein
// *** empty log message ***
//
// Revision 1.29  2005/11/11 04:26:00  etnlwind
// Update for VideoPlayer Screen Clean Method
//
// Revision 1.28  2005/11/10 08:52:54  zstein
// - alarm
//
// Revision 1.27  2005/11/10 07:33:09  akaiaphanu
// updated to g_ExitByAlarm for Extra submode
//
// Revision 1.26  2005/11/07 10:26:44  etnlwind
// 텍스트뷰어 기능추가
// -한페이지에 13라인 출력하게끔 수정 (기존:16라인)
// -페이지 이동 처음/끝 추가
// -자동 라인 스크롤 추가 (slow=9초, mid=6초, fast=3초 임의지정 )
//
// Revision 1.25  2005/10/31 02:44:07  zstein
// - EARPHONE NOTIFY MACRO
//
// Revision 1.24  2005/10/10 01:35:37  etnlwind
// Update for Removing Duplicated Macro Define
//
// #define MSG_QUEUE_DEPTH 	5
// #define MSG_PIPE_NUM    		2
//
// Revision 1.23  2005/09/28 02:53:56  zstein
// - delete DoShutdown(). updated USB connect routine
//
// Revision 1.22  2005/09/21 06:57:07  etnlwind
// Update for VideoPlayer, FileManager Battery Refresh
//
// Revision 1.21  2005/09/20 12:27:21  etnlwind
// Update for VideoPlayer FileManger Shutdown
//
// Revision 1.20  2005/08/27 10:22:04  zstein
// - added cradle message
//
// Revision 1.19  2005/08/19 11:20:04  etnlwind
// Update for __ATTRIBUTE_PACKED__ and FileBrowser Delete Func.
//
// Revision 1.18  2005/08/17 02:02:35  zstein
// - added  MSG_Send( int ssTaskId, int ulMsgId, int usParam1, int usParam2 )
//
// Revision 1.17  2005/06/21 06:12:55  zstein
// - added MSG_FOLDER for future.
//
// Revision 1.16  2005/05/15 03:15:24  ywkim
// export CalcBatteryLevel()
//
// Revision 1.15  2005/04/20 11:40:22  zstein
// *** empty log message ***
//
// Revision 1.14  2005/04/20 11:28:12  etnlwind
// Update for ES4
//
//*****************************************************************************


#ifndef __MSG_H__
#define __MSG_H__

#include <Config.h>

#include <fcntl.h>
#include <tavi_key.h>
#include <sound.h> 		// Volume

#define KEY_DRIVER_PATH			"/dev/misc/tkey"
#define CHARGER_DRIVER_PATH		"/dev/misc/tcharger"
#define VOLUME_DRIVER_PATH		"/dev/misc/sound"
#define TOUCHPAD_DRIVER_PATH		"/dev/misc/tp"
#define RTC_DRIVER_PATH         		"/dev/misc/rtc"

#include "Task.h"
#include "SndCtrl.h"	// added by Lee Seok
#include "MiscCtrl.h"


#define MSG_CREATE    		0x00000001
#define MSG_DESTROY   		0x00000002
#define MSG_DRAW      		0x00000004
#define MSG_CALL_BY_FILEMANAGER	0x00000008

#define MSG_BUTTON_UP 		0x00001000
#define MSG_BUTTON_DN	 	0x00002000
#define MSG_BUTTON_REAL_UP 0x00002001

#define MSG_POINT_UP  		0x00004000
#define MSG_POINT_DN  		0x00008000

#define MSG_SETFOCUS  		0x00010000
#define MSG_KILLFOCUS 		0x00020000

#define MSG_TIMER     		0x00040000
#define MSG_BATTERY		0x00080000
#define MSG_TOUCHPAD		0x00100000
#define MSG_WINDOW		0x00200000	// Added by Lee Seok for window messages.
#define MSG_USBSTORAGE		0x00400000	// add for usbstorage
#define MSG_USB_CLIENT		0x00500000	// Param 0 : Disconnected, Param 1 : Connected
#define MSG_DETECT			0x00600000
#define MSG_FOLDER			0x00700000	// param 0: closed, 1: open
#define MSG_NOTIFY			0x00800000
#define MSG_EXECUTE_TASK	0x01000000	// param 1: MenuID
#define MSG_ALL       			0xffffffff 

#define MAX_SAMPLES 		5
#define MSG_QUEUE_DEPTH 	5
#define MSG_PIPE_NUM    		2

#define USBSTORAGE_MSG_KEY	0xDA08;


#define TIMER_ID_CP_HIDE						0x00000001
#define TIMER_ID_FB_SCROLL						0x00000002
#define TIMER_ID_VP_BASIC						0x00000003
#define TIMER_ID_VP_SEEK						0x00000004
#define TIMER_ID_VP_SEEK_FREE					0x00000005
#define TIMER_ID_TV_AUTOSCROLL_SLOW			0x00000006
#define TIMER_ID_TV_AUTOSCROLL_MID			0x00000007
#define TIMER_ID_TV_AUTOSCROLL_FAST			0x00000008
#define TIMER_ID_SOUND_PALETTE				0x00000009
#define TIMER_ID_SOUND_PALETTE_TOUCH_APPLY 	0x00000010

#define TIMER_VALUE_TV_AUTOSCROLL_SLOW	6
#define TIMER_VALUE_TV_AUTOSCROLL_MID	3
#define TIMER_VALUE_TV_AUTOSCROLL_FAST	1


enum NotifyElement {
	NOTIFY_CRADLE,
	NOTIFY_USBHOST,
	NOTIFY_EARPHONE
};

typedef struct _tag_TMsgEntry{
	U32 ulMsgId;
	int ssToTaskId;
	int usParam1;
	int usParam2;
	int usParam3;
}__ATTRIBUTE_PACKED__ TMsgEntry;

typedef struct _tag_TMsgInitParam{
	//
}__ATTRIBUTE_PACKED__ TMsgInitParam;




typedef struct {
	long	lMsgId;
	int	nAction;
	int	nHostNo;
	unsigned int nArGid[3];
} UsbStrgMsgBuf_t;


extern TTAVI_GLOBAL_CONFIG* pTAVI;
extern CSndCtrl* g_SndCtrl;
extern CMiscCtrl* g_MiscCtrl;
extern bool g_PowerOff;
extern int g_qidMusic;


int MSG_Init( TMsgInitParam* tParam );
int MSG_Exit();
int SpeakerOnOff( bool fOnOff );
int AppTimerCheck(int ssTaskId,  TMsgEntry* tMsg);
int AppEventCheck( int ssTaskId,  TMsgEntry* tMsg );
int MSG_Get( int ssTaskId,  TMsgEntry* tMsg );
int MSG_Peek( int ssTaskId,  TMsgEntry* tMsg );
int MSG_Peek_Ex( int ssTaskId,  TMsgEntry* tMsg, int checkFlag );
int MSG_Peek_Time( void );
int MSG_PeekRawPoint( TMsgEntry* tMsg );
int MSG_Send(int ssTaskId, TMsgEntry* tMsg);
int read_message( int qid, long type, UsbStrgMsgBuf_t *pbuf );
int CalcBatteryLevel( int h=200 );
int MSG_Send( int ssTaskId, int ulMsgId, int usParam1, int usParam2 );
#endif

