//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Extra Clock
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/CExtraClock.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CExtraClock.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.35  2006/02/15 01:43:44  ywkim
// Add function of Extra_Clock_isDisplaying() for checking state of clock
//
// Revision 1.34  2005/12/13 07:44:21  akaiaphanu
// cleaning
//
// Revision 1.33  2005/12/12 06:48:45  akaiaphanu
// Updated to HDD Sleep , Background and Switch mode
//
// Revision 1.32  2005/12/09 07:53:31  akaiaphanu
// updated shutdown function for clock
//
// Revision 1.31  2005/12/09 04:42:00  akaiaphanu
// Updated
//
// Revision 1.30  2005/12/01 02:50:11  akaiaphanu
// cleaning functions
//
// Revision 1.29  2005/11/25 02:08:53  akaiaphanu
// cleaning functions
//
// Revision 1.28  2005/11/23 08:13:49  akaiaphanu
// disabled to alarm bell function
//
// Revision 1.27  2005/11/21 08:33:21  akaiaphanu
// updated to clock digit fonts
//
// Revision 1.26  2005/11/19 05:00:13  akaiaphanu
// updated to 100ms sleep function in clockmode & cleaning
//
// Revision 1.25  2005/11/18 06:22:49  akaiaphanu
// add to function jogkey2
//
// Revision 1.24  2005/11/17 09:04:33  akaiaphanu
// cleaning functions
//
// Revision 1.23  2005/11/17 04:17:32  akaiaphanu
// cleaning function
//
// Revision 1.22  2005/11/15 02:20:12  akaiaphanu
// Updated to process alarmworking
//
// Revision 1.21  2005/11/10 07:35:21  akaiaphanu
// merge Extra clock & alarm
//
// Revision 1.20  2005/11/09 06:38:50  akaiaphanu
// updated to timeformat & cleaning
//
// Revision 1.19  2005/11/08 08:10:54  akaiaphanu
// updated to long exit key & draw date func
//
// Revision 1.18  2005/10/28 09:15:47  akaiaphanu
// modify datefunction
//
// Revision 1.17  2005/10/27 08:30:43  akaiaphanu
// updated & cleaning
//
// Revision 1.16  2005/10/27 02:49:10  akaiaphanu
// modify sleep mode
//
// Revision 1.15  2005/10/25 09:48:26  akaiaphanu
// add to function HDD Sleep
//
// Revision 1.14  2005/10/25 01:45:20  akaiaphanu
// updated to Draw position
//
// Revision 1.13  2005/10/24 02:31:28  akaiaphanu
// add to adjust volume on the background play
//
// Revision 1.12  2005/10/22 05:43:02  akaiaphanu
// modified Extra_Clock_DrawDate()
//
// Revision 1.11  2005/10/21 08:47:44  akaiaphanu
// Cleaning
//
// Revision 1.10  2005/10/20 06:58:31  yong
// refactor Extra_Clock_DrawDate() to use array of Resource ID
//
// Revision 1.9  2005/10/20 06:28:20  akaiaphanu
//
// working on :-)
// ----------------------------------------------------------------------
//
// Revision 1.8  2005/10/19 12:45:54  akaiaphanu
// add Extra_Get_WhatDay() & Extra_Get_Itoa()
//
// Revision 1.7  2005/10/19 06:15:07  akaiaphanu
// Cleanning
//
// Revision 1.6  2005/10/19 05:30:14  etnlwind
// DEBUG Removed
//
// Revision 1.5  2005/10/18 12:40:51  akaiaphanu
// Add Image icons & working
//
// Revision 1.4  2005/10/18 07:32:02  akaiaphanu
// working Draw function
//
// Revision 1.3  2005/10/17 07:32:33  akaiaphanu
// update top-panel
//
// Revision 1.2  2005/10/17 06:24:38  akaiaphanu
// Add to update clock time & date
//
// Revision 1.1  2005/10/12 02:22:59  akaiaphanu
// Add to Extra.cpp/h
//
//*****************************************************************************


#include "CExtraClock.h"
#include "Extra.h"

EXTRA_CLOCK_CONFIG	 _ExtraClockProperty;
extern EXTRA_CONFIG _ExtraProperty;

char * Extra_Get_itoa( int i )
{
		static char local[22];
		char *p = &local[21];
		*p-- = '\0';
		do {
			*p-- = '0' + i % 10;
			i /= 10;
		} while (i > 0);
		return p + 1;
}
int Extra_Get_WhatDay(int year, int month, int day)
{
	int mon[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	unsigned long tot = 0UL;
	int i;

	if(!(year%4) && (year%100) || !(year%400))
	mon[1]++; 

	for(i=0; i<month-1; i++)
	tot += mon[i];

	tot += --year*365 + year/4 - year/100 + year/400 + day;

	return (int)(tot%7L);
} 
extern void Extra_Clock_DrawDate( int fontSize )
{

	char years[5];
	char month[3];
	char days[3];
	char *p;
	char arrMergeDMY[25];
	int iMonth;
	int xPositionMonth;
	int yPositionMonth = 140;
	int xPositionDay;
	int yPositionDay = 170;
	int dayOfWeekResID[] = {ML_SUN, ML_MON, ML_TUE, ML_WED, ML_THU, ML_FRI, ML_SAT};
	char * monthEng[] = {
			"Jannuary", "Febuary", "March", "April", "May", "June", 
			"July", "August", "September", "October", "November", "December" 
		};
	
	/* Check Font size */
	if( fontSize == 12 )
		DEBUGMSG(1,("Font Size = 12 \n"));
	if( fontSize == 16 )
		DEBUGMSG(1,("Font Size = 16 \n"));
		
	/* Init array */
	memset(years, 0, 5);
	memset(month, 0, 3);
	memset(days, 0, 3);
	/* change to char from int */
	strcpy(years , Extra_Get_itoa(_ExtraClockProperty.m_ClockDate.m_year));	
	strcpy(month , Extra_Get_itoa(_ExtraClockProperty.m_ClockDate.m_month));
	strcpy(days , Extra_Get_itoa(_ExtraClockProperty.m_ClockDate.m_day));
	
	iMonth = _ExtraClockProperty.m_ClockDate.m_month-1;

	if(TaviSysParam.etc.language == 0) /* case English */ 
	{
		sprintf( arrMergeDMY, "%s %s %s", days, monthEng[iMonth] ,years );
		pT->DrawEUC8Text( arrMergeDMY, 0, yPositionMonth, _WHITE, fontSize, DT_TRANSPARENCY|DT_ALIGN_CENTER|DT_OUTLINE);
	}

	else if(TaviSysParam.etc.language == 1) /* case Korean  */
	{

	 	sprintf( arrMergeDMY, "%s³â %s¿ù %sÀÏ", years,  month, days);
		pT->DrawEUC8Text( arrMergeDMY, 0, yPositionMonth, _WHITE, fontSize, DT_TRANSPARENCY|DT_ALIGN_CENTER|DT_OUTLINE);
	}
	
	else if(TaviSysParam.etc.language == 2) /* case Japanese  */
	{

	 	sprintf( arrMergeDMY, "%sÒ´ %sêÅ %sìí", years,  month, days);
		pT->DrawEUC8Text( arrMergeDMY, 0, yPositionMonth, _WHITE, fontSize, DT_TRANSPARENCY|DT_ALIGN_CENTER|DT_OUTLINE);
	}
	
	/* Switch Day */
	int dayOfWeek = Extra_Get_WhatDay(
								_ExtraClockProperty.m_ClockDate.m_year,
							    _ExtraClockProperty.m_ClockDate.m_month,
							    _ExtraClockProperty.m_ClockDate.m_day
								);
	if (dayOfWeek >= 0 && dayOfWeek <= 6) {
		pT->DrawResText(dayOfWeekResID[dayOfWeek], 0, yPositionDay, _WHITE, fontSize, DT_TRANSPARENCY|DT_ALIGN_CENTER|DT_OUTLINE);
	}

	return;
	
}

bool Extra_Clock_isDisplaying( void )
{
	return _ExtraClockProperty.m_fClearDisplay;
}

extern void Extra_Clock_DrawTime(bool fSec, char fontsize)
{
	/* 	Image file name = list_icons.bmp  */
	/*   Order = "-", " ", ".", "MHz", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", "AM", "PM"  */
	
	int width = TAVI_IMAGES[CLOCK_DIGIT].bitmapinfoheader.biWidth;
	int height = 46;
	int xPosition ;
	int yPosition ;
	int tp;
	int op;
	unsigned char* p;
	bool isAm = false;
	bool fTimeformat_24 = false;

	int hours = _ExtraClockProperty.m_ClockTime.m_hours;
	int minutes = _ExtraClockProperty.m_ClockTime.m_minutes;
	int seconds = _ExtraClockProperty.m_ClockTime.m_seconds;

	/* Set seconds Position */
	if(fSec)
	{
		xPosition = 21;
		yPosition = 80;
		_ExtraClockProperty.m_fSec = true;
	}
	else
	{
		xPosition = 21;
		yPosition = 80;
		_ExtraClockProperty.m_fSec = false;
	}
	
	/* AM/PM check */
	if(hours<12)
		isAm = true;

	/* Time format check */
	if( TaviSysParam.etc.timeformat ) 
	{
		DEBUGMSG(1,("Time Format = 24H\n"));
		fTimeformat_24 = true;
		
		if(hours < 10)
			xPosition = 12;
		else
			xPosition = 33;
	}
	else 
	{
		DEBUGMSG(1,("Time Format = 12H\n"));
		if(hours >= 12)
		{
			hours = hours -12;
		}
		
		if(hours == 0)
			hours=12;
		else
			;

		if(hours < 10)
			xPosition = 4;
		else
			xPosition = 14;
		
	}
	/* Draw Big Font */
	if((fontsize == 'B')||(fontsize == 'b'))
	{
		/* hours */
		tp = hours /10;
		op = hours%10;
		if(!(tp == 0))
		{
			p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + (tp+3)*width*height;
			pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition, yPosition, width, height );
		}
		
		p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + (op+3)*width*height;
		xPosition += width-8;
		pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition, yPosition, width, height );

		/* colon */
		p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + 13*width*height;
		xPosition += width-8;

		pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition,yPosition, width, height );

		/* minutes */
		tp = minutes/10;
		op = minutes%10;
		
		p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + (tp+3)*width*height;
		xPosition += 17;
		pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition, yPosition, width, height );

		p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + (op+3)*width*height;
		xPosition += width-8;
		pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition, yPosition, width, height );

		if(fSec) /* Set seconds [Show = true]  or  [Hide = false]  */
		{
			/* colon */
			p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + 13*width*height;
			xPosition += width-8;
			
			pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition, yPosition, width, height );

			/* seconds */
			tp = seconds/10;
			op = seconds%10;
			
			p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + (tp+3)*width*height;
			xPosition += 17;
			pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition, yPosition, width, height );

			p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + (op+3)*width*height;
			xPosition += width-8;
			pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition, yPosition, width, height);
		}
		
		if(!fTimeformat_24) /*  Timeformat 12  */
		{
			
			if(isAm) /*  AM  */
			{
				p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + 14*width*height;
				xPosition += width-8;
				pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition, yPosition, width, height );
			}
			else /* PM */
			{
				p = TAVI_IMAGES[CLOCK_DIGIT].image_buffer + 15*width*height;
				xPosition += width-8;
				pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, xPosition, yPosition, width, height );
			}
		}
	}
	else if((fontsize == 'M')||(fontsize == 'm'))
		DEBUGMSG(1,("Font Size = Medium \n"));
	else if((fontsize == 'S')||(fontsize == 's'))
		DEBUGMSG(1,("Font Size = Big \n"));
	else
		DEBUGMSG(1,("Error = Not Exist so that \n"));
	
	return;
	
}

extern void Extra_Clock_SetTime(void)
{
	time_t	ttime;
	struct tm* t;
		
	putenv(g_szTimeZone);
	tzset();
	
	time( &ttime );
	DEBUGMSG(1,("Extra_Clock_SetTime() : get ctime=%s\n", ctime(&ttime)) );

	memcpy(&g_time, localtime( &ttime ), sizeof(g_time) );
	DEBUGMSG(1,( "localtime:%04ld-%02ld-%02ld [%d] %02ld:%02ld:%02ld, %02ldweek days\n",
	        g_time.tm_year, g_time.tm_mon, g_time.tm_mday, t->tm_yday,
	        g_time.tm_hour, g_time.tm_min, g_time.tm_sec,
	        g_time.tm_wday ) );
	
	g_dm = days_in_month[leap_year(g_time.tm_year+1900)][g_time.tm_mon+1];	

	t = localtime( &ttime );
	DEBUGMSG(1,( "localtime =%04ld-%02ld-%02ld [%d] %02ld:%02ld:%02ld, %02ldweek days, %s\n",
	        (1900+t->tm_year), (t->tm_mon+1), t->tm_mday, t->tm_yday,
	        t->tm_hour, t->tm_min, t->tm_sec,
	        t->tm_wday, tzname[t->tm_isdst]) );
	
	/* Set time & date*/
	_ExtraClockProperty.m_ClockTime.m_hours = t->tm_hour;
	_ExtraClockProperty.m_ClockTime.m_minutes = t->tm_min;
	_ExtraClockProperty.m_ClockTime.m_seconds= t->tm_sec;

	_ExtraClockProperty.m_ClockDate.m_year = 1900+t->tm_year;
	_ExtraClockProperty.m_ClockDate.m_month = t->tm_mon+1;
	_ExtraClockProperty.m_ClockDate.m_day = t->tm_mday;
		
	return;

}

 
void Extra_Clock_MainScreen(void)
{
	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(TRUE);
	showbackground(9);
	op_ClearAreaOSD( 0, 0, 320, 240 );

	/* Top Panel */
	g_TopPanel->SetTitle(ML_EXTRA_CLOCK);
	g_TopPanel->SetLogo(CTopPanel::LOGO_ID_ETC);
	g_TopPanel->SetBatteryLevel( (CTopPanel::BatteryLevel)CalcBatteryLevel() );
	g_TopPanel->SetHideItems(7);
	g_TopPanel->SetVisible( true );
	g_TopPanel->Show();
	
	
	DEBUGMSG(1,("TopPanel Pass\n"));

	return;

}

void Extra_Clock_FinishScreen(void)
{
	g_TopPanel->SetShowItems(7);
	ClearOsd();
	return;
}


int Extra_Clock_OnKeyPress( int key )
{
	CEvent evt;
	CListItem* puzItem;
	CCMListItem* pCMItem;
	CListItem* pListItem;
	int vkey;
	CNode< CWindow >* pNode;
	CComplexListView* pCMView;
	CWindow *pwnd;
	CWindow *pComponentwnd;

	DEBUGMSG(1,("In Extra_Clock_OnKeyPress()\n"));
	
	vkey = MapToVirtualKey( key );
	evt.type = EVENT_KEY;
	evt.u.key.value = vkey;
	evt.u.key.pressed = KEY_PRESSED;
	
	switch( vkey )
	{
		
		// EQ
		case VK_F1:		DEBUGMSG(1,("EQ KEY\n")); break;

		// Menu
		case VK_F2:		DEBUGMSG(1,("Menu KEY\n")); break;

		/* Mode Short */
		case VK_F3:		DEBUGMSG(1,("Mode KEY pressed\n"));
						_ExtraProperty.m_SubClockState = CLOCK_EXIT_BACKGROUND;
						_ExtraClockProperty.m_fExit = true;
						DEBUGMSG(1,("Out Extra_OnKeyPress() as Background\n"));
						return -1;
						break;
		/* Exit Short */
		case VK_ESC:	DEBUGMSG(1,("Exit KEY pressed\n"));
						_ExtraProperty.m_SubClockState = CLOCK_EXIT_NORMAL;
						_ExtraClockProperty.m_fExit = true;
						DEBUGMSG(1,("Out Extra_OnKeyPress() as Normal\n"));
						return -1;
						break;
		/* Mode Long */				
		case TAVI_KEY_LMODE: DEBUGMSG(1,("Mode Long KEY pressed\n"));
						_ExtraProperty.m_SubClockState = CLOCK_EXIT_SWITCH;
						_ExtraClockProperty.m_fExit = true;
						DEBUGMSG(1,("Out Extra_OnKeyPress() as Switch\n"));
						return -1;
						break;
		/* Exit Long */		
		case VK_EXT_ESC:	DEBUGMSG(1,("Exit Long KEY pressed\n"));
						/* case alarm working value : 5 */
						_ExtraProperty.m_SubClockState = CLOCK_EXIT_ALARM;
						_ExtraClockProperty.m_fExit = true;
						DEBUGMSG(1,("Out Extra_OnKeyPress() as ExitAndSwitch\n"));
						return -1;
						break;
						
		/* Up volume on the background mode */
		case TAVI_KEY_JOG1_UP:
						TAVI_BG_Volume( 1 );
						break;
		/* Down volume on the background mode */
		case TAVI_KEY_JOG1_DOWN:
						TAVI_BG_Volume( 0 );
						break;
		/* Move to next music on the background mode */				
		case TAVI_KEY_JOG2_UP:
						TAVI_BG_Next(  );
						break;
		/* Move to prev music on the background mode */				
		case TAVI_KEY_JOG2_DOWN:
						TAVI_BG_Prev(  );
						break;
		/* Play & Pause music on the background mode */				
		case TAVI_KEY_JOG2_ENTER:
						TAVI_BG_PlayPause(  );
						break;
		
						
		default: DEBUGMSG(1,("Other KEY pressed\n"));
			break;
		
	}

	DEBUGMSG(1,("Out Extra_Clock_OnKeyPress()\n"));
	
	return 0;
			
}

int Extra_Clock_MsgProc( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	MouseEvent mevt;
	KeyEvent key;
	
	DEBUGMSG(1,("In Extra_Clock_MsgProc()\n"));
	DEBUGMSG(1,("TaskId[%x], MsgId[%x], Param1[%x] \n", ssTaskId, ssMsgId, usParam1));
	
	switch( ssMsgId )
	{
		case MSG_CREATE:		break;	// 1
		case MSG_DRAW:			break;	// 4
		case MSG_TIMER:			break;
		case MSG_SETFOCUS : 	break;	// 10000
		case MSG_KILLFOCUS: 	break;	// 20000
		case MSG_DESTROY:		break;
		case MSG_BUTTON_DN: 	break;	// 2000							
		case MSG_BUTTON_UP: 			// 1000	
			DEBUGMSG(1,("button up\n")); 
			if (usParam1 == TAVI_KEY_SHUTDOWN)
			{
				DEBUGMSG(1,("Extra_Clock_MsgProc() : TAVI_KEY_SHUTDOwN\n" ));
				_ExtraProperty.m_SubClockState = CLOCK_EXIT_NORMAL;
				_ExtraClockProperty.m_fExit = true;
				return -1;
			}
			return Extra_Clock_OnKeyPress(usParam1);
			break;
						
		case MSG_BATTERY:		break;		
		case MSG_TOUCHPAD:		DEBUGMSG(1,("touchpad\n"));	// 100000
								break;
			
		default:
			return false;
			break;
	}

	DEBUGMSG(1,("Out Extra_Clock_MsgProc()\n"));
	
	pMpegDecoder->RefreshOsd ();					// refresh screen							

	return 0;
}

extern int Extra_Clock_Task(void)
{
	TMsgEntry tMsg;
	struct timeval tv_cur = { 0, 0 };
	struct timeval tv_last;

	/* Draw main screen */
	Extra_Clock_MainScreen();
	/* Key & TouchPad */
	SetTaviKeyMode(TK_REPEAT);
	mep_report( MEP_REPORT_RELATIVE);	
	
	/* Set Exit State */
	_ExtraClockProperty.m_fExit = false;	
 	/* Task resume for background */
	if( _ExtraClockProperty.m_Resume == true) 
	{
		_ExtraClockProperty.m_Resume = false;
	}	
	/* Main loop */
	ClearKeyBuffer();	
	/* Start HDD Sleep*/
	TAVI_CanSleep(17);
	
	tv_cur = tv_last;
	// for checking state of clock display
	_ExtraClockProperty.m_fClearDisplay = true;
	
	while( _ExtraClockProperty.m_fExit == false )
	{
		gettimeofday( &tv_cur, NULL );
		/* Draw clock per 100ms */
		if( abs(tv_cur.tv_usec-tv_last.tv_usec) > 100000 )
		{
			op_ClearAreaOSD( 20, 20, 300, 220 );
			/* Get & Set Time */
			Extra_Clock_SetTime();
			/* Draw Date */
			Extra_Clock_DrawDate(16);
			/* Draw Time */
			Extra_Clock_DrawTime(true, 'B');
			/* refresh screen */
			pMpegDecoder->RefreshOsd ();	
		}
		
		/* Message peek ,  Extra Task ID = 7 */
		if( MSG_Peek( 7, &tMsg ) == 0)	
		{
			if(Extra_Clock_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) == -1)
			{
				DEBUGMSG(1,("Break while loop\n"));
				break;
			}
		}
		else
		{
			;	
		}
		usleep(100000);	// 1,000,000 usec = 1sec
		
	}

	// for checking state of clock display
	_ExtraClockProperty.m_fClearDisplay = false;

	Extra_Clock_FinishScreen();

	/* Stop HDD Sleep*/
	TAVI_CannotSleep(17);

	return 0;

}


