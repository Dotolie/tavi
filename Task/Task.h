//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : Tavi Task Process Headers
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Task.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/19 04:59:02 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: Task.h,v $
// Revision 1.2  2006/06/19 04:59:02  ywkim
// Remove member of m_bLcdEnable ,m_nSavedLcdBright in struct of _tag_HddSleep
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.32  2005/12/24 08:31:36  yong
// refactoring on calling TAVI_CreateTask()
//
// Revision 1.31  2005/12/22 05:13:19  etnlwind
// Update for New Version
//
// Revision 1.30  2005/12/08 07:39:28  ywkim
// Add member of m_nHddSleeping4Second in g_HddSleep & func of TAVI_CannotSleepAudio()
//
// Revision 1.29  2005/11/18 01:53:26  zstein
// - TAVI_BG_PlayPause
//
// Revision 1.28  2005/11/18 01:49:07  zstein
// - PLAY_PAUSE
//
// Revision 1.27  2005/11/17 11:19:47  zstein
// *** empty log message ***
//
// Revision 1.26  2005/11/10 08:53:11  zstein
// *** empty log message ***
//
// Revision 1.25  2005/10/26 07:45:09  ywkim
// 1. Change member name of g_HddSleep from m_bLcd to m_bLcdEnable
// 2. Add logic for Brigthness & AutoBright
//
// Revision 1.24  2005/10/25 07:09:58  etnlwind
// Update for Palette Adjust
//
// Revision 1.23  2005/10/24 07:58:07  ywkim
// Change some routines for hdd power saving
// 1. Add member of SleepingKey in TTaskEntryBlock
// 2. Add checking point in MSG_Peek for hdd sleeping
// 3. Add Hdd sleep related functions with prefix TAVI_
//
// Revision 1.22  2005/10/19 09:48:10  etnlwind
// Update for Startup Progress Display
//
// Revision 1.21  2005/10/12 09:24:34  etnlwind
// 1. MENU_ID_XXX를 MENU_SLOT1 ~ MENU_SLOT9로 변경
// 2. TASK_ID_XXX가 들어갈자리에 있던 MENU_ID_XXX들을 TASK_ID_XXX로 수정
// 3. MENU_ID_XXX 정의 삭제
// 4. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.20  2005/10/10 14:23:42  yong
// _Set_InitParam_Task_Name_Id() added by refactoring
// MENU_ID_* macros in Task.h
//
// Revision 1.19  2005/10/08 02:10:24  zstein
// -
//
// Revision 1.18  2005/10/07 06:55:07  zstein
// *** empty log message ***
//
// Revision 1.17  2005/10/06 08:29:15  etnlwind
// Update for TextViewer,  MainMenu
//
// Revision 1.16  2005/09/06 10:33:54  etnlwind
// Update for VideoPlayer
//
// Revision 1.15  2005/08/19 11:20:04  etnlwind
// Update for __ATTRIBUTE_PACKED__ and FileBrowser Delete Func.
//
// Revision 1.14  2005/08/03 03:34:12  zstein
// - added background task structure.
//
// Revision 1.13  2005/07/08 08:23:38  etnlwind
// Update for
// 1. System Palette Loading Method Changed
// 2. Video Repeat Related Method Changed
//
// Revision 1.12  2005/06/03 01:18:51  zstein
// - added hold routine.
//
// Revision 1.11  2005/04/20 11:28:12  etnlwind
// Update for ES4
//
//*****************************************************************************


#ifndef __TASK_H__
#define __TASK_H__


#define BOTTOM_MENU_FILTER_COLOR_PALETTE	27
#define BG_FILTER_COLOR_PALETTE	67

#define MAX_TASK 	10
#define TASK_STOP	0x0001
#define TASK_RUN	0x0002
#define TASK_PAUSE	0x0003

enum {
	TaskExit_Normal=1,
	TaskExit_Background,
	TaskExit_Switch,
	TaskExit_ExitAndSwitch
};

#include "TopPanel.h" 	// added by Lee Seok

//task_init_param;

typedef struct _tag_TTaskInitParam {
	char*		pTaskName;
	int    		ssTaskId;
	int 		resume;
} __ATTRIBUTE_PACKED__ TTaskInitParam;

typedef int (*TaskInitFunc)(	TTaskInitParam* tParam );

typedef int (*TaskMsgProc)(	int ssTaskId, U32 ulMsgId, int usParam1, int usParam2 );
typedef int (*TaskExitFunc)( void );

enum TaskIDs {
	TASK_ID_MAIN,
	TASK_ID_MUSIC,
	TASK_ID_VIDEO,
	TASK_ID_RADIO,
	TASK_ID_IMAGE,
	TASK_ID_TEXT,
	TASK_ID_RECORD,
	TASK_ID_ETC,
	TASK_ID_FILEMANAGER,
	TASK_ID_SETUP,
	TASK_ID_END
};

#define MENU_SLOT1	10
#define MENU_SLOT2	20
#define MENU_SLOT3	30
#define MENU_SLOT4	40
#define MENU_SLOT5	50
#define MENU_SLOT6	60
#define MENU_SLOT7	70
#define MENU_SLOT8	80
#define MENU_SLOT9	90

typedef struct  _tag_TTaskEntryBlock {
	int					taskID;
	int					switchable;
	U16 				usTaskState;
	U32 				ulMsgMask;
	TaskInitFunc 		fpInitFunc;
	TaskMsgProc 		fpMsgProc;
	TaskExitFunc		fpExitFunc;
	int					SleepingKey;	// power save
} __ATTRIBUTE_PACKED__ TTaskEntryBlock;
// ========== add for power save =======
typedef struct _tag_HddSleep {
	bool m_bOnSleep;
	long	m_lLastAccess;
	int 	m_nHddSleeping;
	int 	m_nHddSleeping4Second;
} __ATTRIBUTE_PACKED__ HDDsleepReg;
// ========== add for power save =======
static int TAVI_MsgProc( int ssTaskId, U32 ulMsgId, int usParam1 , int usParam2 );
//static int TAVI_MsgProc( int ssTaskId, U32 ulMsgId, int usParam1 , int usParam2 );
//static int TAVI_MsgProc( int ssTaskId, U32 ulMsgId, U16 usParam1, U16 usParam2 );

int TAVI_LoadPalette();
int TAVI_LoadPaletteTransparencyData( int IsSubPaletteTransparencyType );

int TAVI_LoadBitmaps();
int TAVI_UnloadBitmaps();
static int DigitToMACRO( int Digit );
static void TAVI_DrawSubMenu2( int menuset, int sy, int height );
static int TAVI_DrawMenuItem3( int menuset );

//U16 TAVI_DrawMenuItem( U16 usMenuSet );
static int TAVI_DrawMovingTransparencyBackground( int option );
int TAVI_MainTask( TTaskInitParam* tParam );
int TAVI_CreateTask( int ssTaskId, TTaskInitParam* tParam );
int TAVI_CreateTask_For_CallByFileManager(  int ssTaskId, TTaskInitParam* tParam );
void TAVI_BG_Volume( int dir );
void TAVI_BG_VolTimeout( void );
void TAVI_BG_Next( void );
void TAVI_BG_Prev( void );
void TAVI_BG_PlayPause( void );
void _PreapareFor_StartupProgressbar();

// power save
void TAVI_InitSleepReg( void );
void TAVI_CanSleep( int debug=0 );
void TAVI_CanSleepAudio( int debug=0 );
void TAVI_CannotSleep( int debug=0 );
void TAVI_CannotSleepAudio( int debug=0 );
void TAVI_ResetSleepTimer(void);
void TAVI_SleepDown(  long lCurrentTime );
void TAVI_WakeUp( int debug=0 );
int TAVI_KillAllTasks( void );

#endif


