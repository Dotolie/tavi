//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Extra Clock
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/CExtraClock.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CExtraClock.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.10  2006/02/15 01:43:45  ywkim
// Add function of Extra_Clock_isDisplaying() for checking state of clock
//
// Revision 1.9  2005/12/13 07:44:21  akaiaphanu
// cleaning
//
// Revision 1.8  2005/11/19 05:00:13  akaiaphanu
// updated to 100ms sleep function in clockmode & cleaning
//
// Revision 1.7  2005/11/17 09:04:33  akaiaphanu
// cleaning functions
//
// Revision 1.6  2005/10/21 08:47:44  akaiaphanu
// Cleaning
//
// Revision 1.5  2005/10/20 06:28:20  akaiaphanu
//
// working on :-)
// ----------------------------------------------------------------------
//
// Revision 1.4  2005/10/19 12:45:54  akaiaphanu
// add Extra_Get_WhatDay() & Extra_Get_Itoa()
//
// Revision 1.3  2005/10/18 07:32:02  akaiaphanu
// working Draw function
//
// Revision 1.2  2005/10/17 06:24:38  akaiaphanu
// Add to update clock time & date
//
// Revision 1.1  2005/10/12 02:22:59  akaiaphanu
// Add to Extra.cpp/h
//
//*****************************************************************************

#ifndef __CExtraClock_WINDOW_H__
#define __CExtraClock_WINDOW_H__

#include "Extra.h"

typedef struct  {		
		int m_hours;
		int m_minutes; 
		int m_seconds;
		int m_tmp;
		
	} _ClockTime ;

typedef struct  {		
		int m_year;
		int m_month; 
		int m_day;
		
	} _ClockDate ;

typedef struct _tag_EXTRA_CLOCK_CONFIG {

	_ClockTime m_ClockTime;
	_ClockDate m_ClockDate;
	bool m_fExit;
	bool m_Resume;
	int 	m_ExitState;
	bool m_fAlarmSetting;
	bool m_fSec;
	bool m_fClearDisplay;
	
} _ATTRIBUTE_PACKED_ EXTRA_CLOCK_CONFIG;


/* leap year -- account for gregorian reformation in 1752 */
#define leap_year(yr)    ((yr) <= 1752 ? !((yr) % 4) : (!((yr) % 4) && ((yr) % 100)) || !((yr) % 400))

static int days_in_month[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};
static struct tm g_time;
static int g_dm;
static int g_timezone;
static char g_szTimeZone[30];

extern int Extra_Clock_Task(void);
extern void Extra_Clock_DrawDate( int fontSize );
extern void Extra_Clock_DrawTime( bool fSec, char fontsize );
extern void Extra_Clock_SetTime(void);
void Extra_Clock_MainScreen(void);
void Extra_Clock_FinishScreen(void);
int Extra_Clock_OnKeyPress( int key );
int Extra_Clock_MsgProc( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 );
bool Extra_Clock_isDisplaying( void );

#endif /* __CExtraClock_WINDOW_H__ */
