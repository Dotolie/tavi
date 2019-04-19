//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Extra Task
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/Extra.h,v $
// $Revision: 1.2 $
// $Date: 2006/02/27 02:40:17 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: Extra.h,v $
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.12  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.11  2005/12/14 02:35:19  akaiaphanu
// cleaning functions
//
// Revision 1.10  2005/11/25 02:08:54  akaiaphanu
// cleaning functions
//
// Revision 1.9  2005/11/15 02:20:12  akaiaphanu
// Updated to process alarmworking
//
// Revision 1.8  2005/11/10 07:35:21  akaiaphanu
// merge Extra clock & alarm
//
// Revision 1.7  2005/10/19 12:45:54  akaiaphanu
// add Extra_Get_WhatDay() & Extra_Get_Itoa()
//
// Revision 1.6  2005/10/19 06:15:07  akaiaphanu
// Cleanning
//
// Revision 1.5  2005/10/18 07:32:02  akaiaphanu
// working Draw function
//
// Revision 1.4  2005/10/17 06:24:38  akaiaphanu
// Add to update clock time & date
//
// Revision 1.3  2005/10/12 12:27:08  akaiaphanu
// Cleaning and ClockAction
//
// Revision 1.2  2005/10/12 05:40:54  akaiaphanu
// cleaning
//
// Revision 1.1  2005/10/12 02:22:59  akaiaphanu
// Add to Extra.cpp/h
//
//*****************************************************************************

#ifndef __EXTRA_H__
#define __EXTRA_H__

#include <stdio.h>
#include <stdlib.h>
#include <tavi_lib.h>
#include <linux/reboot.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <mntent.h>
#include <getopt.h>
#include <fatfs.h>
#include <Task/ExtLCD.h>
#include <time.h>

#include <Component/fbdev.h>
#include <Component/ComplexLabel.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include <Component/ClockSetView.h>

#include "../Lib/LibImage.h"
#include "../Lib/LibFlash.h"
#include "../Lib/LibScrollBar.h"
#include "../TopPanel.h"
#include "../tavi_global.h"
#include "../Recorder/RecordWnd.h"
#include "../../lcd/lcd.h"
#include "CExtraClock.h"
#include "AlarmSet.h"
#include "CExtraDic.h"

#define EXTRA_ITEM_CLOCK 	0
#define EXTRA_ITEM_ALARM 	1
#define EXTRA_ITEM_DIC	 	2

#define EXTRA_EXIT_NORMAL			1
#define EXTRA_EXIT_BACKGROUND		2
#define EXTRA_EXIT_SWITCH			3
#define EXTRA_EXIT_EXITSWITCH		4

#define CLOCK_EXIT_NORMAL			1
#define CLOCK_EXIT_BACKGROUND		2
#define CLOCK_EXIT_SWITCH			3
#define CLOCK_EXIT_EXITSWITCH		4
#define CLOCK_EXIT_ALARM			5

#define ALARM_EXIT_NORMAL			1
#define ALARM_EXIT_BACKGROUND		2
#define ALARM_EXIT_SWITCH			3
#define ALARM_EXIT_EXITSWITCH		4
#define ALARM_EXIT_ALARM			5

#define DIC_EXIT_NORMAL			1
#define DIC_EXIT_BACKGROUND		2
#define DIC_EXIT_SWITCH			3
#define DIC_EXIT_EXITSWITCH		4
#define DIC_EXIT_ALARM				5


typedef struct _tag_EXTRA_CONFIG {
	CListView* m_puzListView;
	CWindow* m_FocusedWindow;
	CWindow* m_ChildWnd;
	CList< CWindow > m_WndList;
	char m_CurrentItem;
	char m_SubClockState;
	char m_SubAlarmState;
	char m_SubDicState;
	char m_SubState;
	bool m_fExit;
	int 	m_ExitState;
	int	m_OnConfirm;
	CMsgBox* m_pMsgBox;
	
	
} _ATTRIBUTE_PACKED_ EXTRA_CONFIG;

extern SysParameter_t	TaviSysParam;
extern CTextEngine_BDF* pT;
extern BITMAPFILEINFO256 TAVI_IMAGES[];

extern int		g_CurrentTaskID;
extern CTopPanel	*g_TopPanel;

extern void showbackground( char* path );
extern void showbackground( int nMenuId );
extern int TAVI_LoadPaletteTransparencyData( int IsSubPaletteTransparencyType );

int 	Extra_MsgProc( int ssTaskId, U32 ssMsgId, int usParam1 , int usParam2 );
int 	Extra_Task( TTaskInitParam* tParam );
int   Extra_Kill(void);
void Extra_MainScreen(void);
void Extra_FinishScreen(void);
int Extra_CurrentFocusedCheck(void);
int 	Extra_OnKeyPress( int key );
void Extra_OnMouseEvent( const MouseEvent& evt );
static void Extra_CreateItems(void);
unsigned char* Extra_GetImageElement( BITMAPFILEINFO256 bmp, int id, int count );

#endif /* __EXTRA_H__ */
