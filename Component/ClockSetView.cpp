//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : CClockSetView class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ClockSetView.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/06/12 04:46:11 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: ClockSetView.cpp,v $
// Revision 1.3  2006/06/12 04:46:11  ywkim
// Change some routine for melon
//
// Revision 1.2  2006/03/14 11:16:42  ywkim
// Add argument of MAX_COUNT_TIMEZONES for arTimeZone[]
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.20  2006/02/14 03:22:24  zstein
// - save gmtime.
//
// Revision 1.19  2006/02/11 05:01:18  zstein
// - save local time not GMT time.
//
// Revision 1.18  2006/02/08 02:54:37  ywkim
// Fix syntax error in SaveTime
//
// Revision 1.17  2006/02/08 02:43:20  ywkim
// Change function of SaveTime for saving rtc data
//
// Revision 1.16  2005/12/09 02:57:11  ywkim
// Change am 0 hour to am 12 hour for display
//
// Revision 1.15  2005/11/22 08:33:27  ywkim
// Change SaveTime() for rtc driver
//
// Revision 1.14  2005/11/02 06:44:17  ywkim
// Bugfix  boundary problem of am11 ,pm12
//
// Revision 1.13  2005/10/17 01:24:33  zstein
// - deleted m_fParentFlush
//
// Revision 1.12  2005/09/27 09:09:15  etnlwind
// Update for
// 1. DEBUGMSG redefinition removed.
//
// Revision 1.11  2005/09/21 07:13:29  ywkim
// Remark some timezone and add function for DST
//
// Revision 1.10  2005/08/25 07:11:14  ywkim
// divide sideny / mebourne & manila/hongkong
//
// Revision 1.9  2005/08/17 09:23:47  ywkim
// change routine for timezone
//
// Revision 1.8  2005/07/29 03:17:17  ywkim
// Change routine for timezone
//
// Revision 1.7  2005/07/13 12:57:32  etnlwind
// Update for DEBUGMSG Related Functions
//
// Revision 1.6  2005/05/15 10:36:10  ywkim
// Change timesave routine
//
// Revision 1.5  2005/05/04 06:15:32  ywkim
// Add verification fuc. of day
//
// Revision 1.4  2005/05/03 11:43:07  ywkim
// Collecting year base
//
// Revision 1.3  2005/04/28 12:41:15  ywkim
// modify part of date func.
//
// Revision 1.2  2005/04/28 06:54:23  ywkim
// Add function of Readtime & Savetime
//
// Revision 1.1  2005/04/27 15:43:05  ywkim
// Initially build with m-language
//
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include <bitmap.h>
#include <sys/time.h>
#include <time.h>

#include "fbdev.h"
#include "ClockSetView.h"
#include "../Lib/LibImage.h"
#include <Lib/LibBuzzer.h>
#include "../Driver/rtc/tavi_rtc.h"
#include <Lib/LibDefines.h>


// Macros
#define COLOR_FOCUSED			27
#define BACKGROUND_COLOR		22
#define FORGROUND_COLOR		23
#define B						28
#define F						23
/* leap year -- account for gregorian reformation in 1752 */
#define leap_year(yr)    ((yr) <= 1752 ? !((yr) % 4) : (!((yr) % 4) && ((yr) % 100)) || !((yr) % 400))

// Extern variables
extern CTextEngine_BDF* pT;


// Local variables
static char szLeftArrow[7][5] = {
	{ B, B, B, F, F },
	{ B, B, F, F, F },
	{ B, F, F, F, B },
	{ F, F, F, B, B },
	{ B, F, F, F, B },
	{ B, B, F, F, F },
	{ B, B, B, F, F }
};

static char szRightArrow[7][5] = {
	{ F, F, B, B, B },
	{ F, F, F, B, B },
	{ B, F, F, F, B },
	{ B, B, F, F, F },
	{ B, F, F, F, B },
	{ F, F, F, B, B },
	{ F, F, B, B, B }
};

static int days_in_month[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};
/*
char	*arTimeZone[]={
	"TZ=GMT+9",						// seoul
	"TZ=GMT-4DST,M04.01,M10.28",		// New York/Miami
	"TZ=GMT-5DST,M04.01,M10.28",		// Dallas
	"TZ=GMT-6DST,M04.01,M10.28",		// Denver
	"TZ=GMT-4DST,M04.01,M10.28",		// Detroit	
	"TZ=GMT+1DST,M03.25,M10.28",		// London
	"TZ=GMT+2DST,M03.25,M10.28",		// Rome
	"TZ=GMT-7DST,M04.01,M10.,28",		// Los Angeles
	"TZ=GMT+1DST,M03.25,M10.28",		// Lisbon
	"TZ=GMT-3DST,M10.14,M02.18",		// Rio De Janeiro

	"TZ=GMT+8",						// Manila
	"TZ=GMT+8",						// Hong Kong
	"TZ=GMT+2DST,M03.25,M10.28",		// Madrid
	"TZ=GMT-5DST,M04.01,M10.28",		// Mexico city
	"TZ=GMT+4DST,M03.25,M10.28",		// Moscow
	"TZ=GMT-4DST,M04.01,M10.28",		// Montreal
	"TZ=GMT+4DST,M04.01,M10.01",		// Baghdad
	"TZ=GMT+7",						// Bangkok
	"TZ=GMT+2DST,M03.25,M10.28",		// Berlin
	"TZ=GMT+2DST,M03.25,M10.28",		// Beirut
	"TZ=GMT-4DST,M04.01,M10.28",		// Boston

	"TZ=GMT+8",						// Beijing
	"TZ=GMT+2DST,M03.25,M10.28",		// Vienna
	"TZ=GMT-11",					// Samoa
	"TZ=GMT-7DST,M04.01,M10.28",		// San Francisco
	"TZ=GMT+10DST,M10.28,M03.25",	// Sidney
	"TZ=GMT+10DST,M10.28,M03.25",	// Melbourne 
	"TZ=GMT-7DST,M04.01,M10.28",		// Seattle
	"TZ=GMT-5DST,M04.01,M10.28",		// Chicago
	"TZ=GMT+8",						// Singapore
	"TZ=GMT+3DST,M03.25,M10.28",		// Athens
	"TZ=GMT+2DST,M04.01,M10.28",		// Amsterdam

	"TZ=GMT-4DST,M04.01,M10.28",		// Washington
	"TZ=GMT+12DST,M10.07,M03.18",	// Wellington
	"TZ=GMT-4DST,M04.01,M10.28",		// George Town
	"TZ=GMT+3DST,M04.27,M09.28",		// Cairo
	"TZ=GMT+8",						// Taipei
	"TZ=GMT-4:30DST,M03.21,M09.23",	// Tehran
	"TZ=GMT+2DST,M03.25,M10.28",		// Paris
	"TZ=GMT-7",						// Phoenix
	"TZ=GMT+2DST,M03.25,M10.28",		// Hannover
	"TZ=GMT+3DST,M03.25,M10.28",		// Helsinki

	"TZ=GMT-10"						// Honolulu
};
*/
char	*arTimeZone[MAX_COUNT_TIMEZONES]={
	"TZ=GMT-9",						// seoul
	"TZ=GMT+5DST,M04.01,M10.28",		// New York/Miami
	"TZ=GMT+6DST,M04.01,M10.28",		// Dallas
	"TZ=GMT+7DST,M04.01,M10.28",		// Denver
	"TZ=GMT+5DST,M04.01,M10.28",		// Detroit	
	"TZ=GMT-0DST,M03.25,M10.28",		// London
	"TZ=GMT-1DST,M03.25,M10.28",		// Rome
	"TZ=GMT+8DST,M04.01,M10.28",		// Los Angeles
	"TZ=GMT-0DST,M03.25,M10.28",		// Lisbon
//	"TZ=GMT+4DST,M10.14,M02.18",		// Rio De Janeiro

	"TZ=GMT-8",						// Manila
	"TZ=GMT-8",						// Hong Kong
	"TZ=GMT-1DST,M03.25,M10.28",		// Madrid
	"TZ=GMT+6DST,M04.01,M10.28",		// Mexico city
	"TZ=GMT-3DST,M03.25,M10.28",		// Moscow
	"TZ=GMT+5DST,M04.01,M10.28",		// Montreal
	"TZ=GMT-3DST,M04.01,M10.01",		// Baghdad
	"TZ=GMT-6",						// Bangkok
	"TZ=GMT-1DST,M03.25,M10.28",		// Berlin
	"TZ=GMT-1DST,M03.25,M10.28",		// Beirut

	"TZ=GMT+5DST,M04.01,M10.28",		// Boston
	"TZ=GMT-8",						// Beijing
	"TZ=GMT-1DST,M03.25,M10.28",		// Vienna
	"TZ=GMT+11",					// Samoa
	"TZ=GMT+8DST,M04.01,M10.28",		// San Francisco
//	"TZ=GMT-9DST,M10.28,M03.25",		// Sidney 
//	"TZ=GMT-9DST,M10.28,M03.25",		// Melbourne 
	"TZ=GMT+8DST,M04.01,M10.28",		// Seattle
	"TZ=GMT+6DST,M04.01,M10.28",		// Chicago
	"TZ=GMT-8",						// Singapore

	"TZ=GMT-2DST,M03.25,M10.28",		// Athens
	"TZ=GMT-1DST,M04.01,M10.28",		// Amsterdam
	"TZ=GMT+5DST,M04.01,M10.28",		// Washington
//	"TZ=GMT-11DST,M10.07,M03.18",	// Wellington
	"TZ=GMT+5DST,M04.01,M10.28",		// George Town
	"TZ=GMT-2DST,M04.27,M09.28",		// Cairo
	"TZ=GMT-8",						// Taipei
	"TZ=GMT+5:30DST,M03.21,M09.23",	// Tehran
	"TZ=GMT-1DST,M03.25,M10.28",		// Paris
	"TZ=GMT+7",						// Phoenix

	"TZ=GMT-1DST,M03.25,M10.28",		// Hannover
	"TZ=GMT-2DST,M03.25,M10.28",		// Helsinki
	"TZ=GMT+10"						// Honolulu
};


struct tm CClockSetView::m_time;
int CClockSetView::m_is24hformat;
int CClockSetView::m_dm;
int CClockSetView::m_timezone;
char CClockSetView::m_szTimeZone[30];
int CClockSetView::m_changed;

CClockSetView::CClockSetView( CWindow* parent  )
{
	int i=0;
	
	m_parent = parent;
	m_puzLArrow = new CPixmap;
	m_puzLArrow->CreatePixmap( 5, 7, 8, (char*)szLeftArrow, false );
	m_puzRArrow = new CPixmap;
	m_puzRArrow->CreatePixmap( 5, 7, 8, (char*)szRightArrow, false );

	m_nWindowType = WINDOW_TYPE_CLOCKVIEW;
	m_nFocusedColor = 28;
	m_changed = 0;
}

CClockSetView::~CClockSetView( void )
{
	if( m_puzLArrow ) delete m_puzLArrow;
	if( m_puzRArrow ) delete m_puzRArrow;
}

void CClockSetView::ReadTime( void )
{
	time_t	ttime;
	struct tm* t;
		
	putenv(m_szTimeZone);
	tzset();
	
	time( &ttime );
	DEBUGMSG(1,("CClockSetView.cpp : get ctime=%s\n", ctime(&ttime)) );

	memcpy(&m_time, localtime( &ttime ), sizeof(m_time) );
	DEBUGMSG(1,( "localtime:%04ld-%02ld-%02ld [%d] %02ld:%02ld:%02ld, %02ldweek days\n",
	        m_time.tm_year, m_time.tm_mon, m_time.tm_mday, t->tm_yday,
	        m_time.tm_hour, m_time.tm_min, m_time.tm_sec,
	        m_time.tm_wday ) );
	m_dm = days_in_month[leap_year(m_time.tm_year+1900)][m_time.tm_mon+1];	

#if 0	
	t = gmtime( &ttime );
	printf( "   gmtime =%04ld-%02ld-%02ld [%d] %02ld:%02ld:%02ld, %02ldweek days, %s\n",
	        (1900+t->tm_year), (t->tm_mon+1), t->tm_mday, t->tm_yday,
	        t->tm_hour, t->tm_min, t->tm_sec,
	        t->tm_wday, tzname[t->tm_isdst] );
	t = localtime( &ttime );
	printf( "localtime =%04ld-%02ld-%02ld [%d] %02ld:%02ld:%02ld, %02ldweek days, %s\n",
	        (1900+t->tm_year), (t->tm_mon+1), t->tm_mday, t->tm_yday,
	        t->tm_hour, t->tm_min, t->tm_sec,
	        t->tm_wday, tzname[t->tm_isdst] );

	printf("m_szTimeZone = %s\n", m_szTimeZone);
#endif
}


char *CClockSetView::SaveTime( void )
{
	struct rtc_time rtime;
	int 	century;
	int 	year;
	int	fd;
	time_t	ttime;
	struct tm* t;
	int handle_rtc;


	ttime = mktime(&m_time);
	DEBUGMSG(1,("CClockSetView.cpp : get ctime=%s\n", ctime(&ttime)) );
	stime(&ttime);
	
	t = gmtime( &ttime );
	if( t->tm_year >= 100 ) {
		rtime.tm_century = 1;
		rtime.tm_year = t->tm_year - 100;
	}
	else {
		rtime.tm_century = 0;
		rtime.tm_year = t->tm_year;
	}
	rtime.tm_mon  = t->tm_mon+1;
	rtime.tm_mday = t->tm_mday;
	rtime.tm_hour = t->tm_hour;
	rtime.tm_min  = t->tm_min;
	rtime.tm_sec  = t->tm_sec; 
	rtime.tm_wday = t->tm_wday;

	DEBUGMSG(1,( "gmtime:%04ld-%02ld-%02ld [%d] %02ld:%02ld:%02ld, %02ldweek days\n",
	        (rtime.tm_year+1900), (rtime.tm_mon), rtime.tm_mday, t->tm_yday,
	        rtime.tm_hour, rtime.tm_min, rtime.tm_sec,
	        rtime.tm_wday ) );	

	handle_rtc = open ( RTC_DRIVER_PATH, O_WRONLY );
	if ( handle_rtc < 0 ) {

		}
	else {
		ioctl( handle_rtc, RTC_SET_TIME, &rtime );
		close (handle_rtc);
		}
	
	Show();

#if 0
	printf( "   gmtime =%04ld-%02ld-%02ld [%d] %02ld:%02ld:%02ld, %02ldweek days, %s\n",
	        (1900+t->tm_year), (t->tm_mon+1), t->tm_mday, t->tm_yday,
	        t->tm_hour, t->tm_min, t->tm_sec,
	        t->tm_wday, tzname[t->tm_isdst] );


	t = localtime( &ttime );
	printf( "localtime =%04ld-%02ld-%02ld [%d] %02ld:%02ld:%02ld, %02ldweek days, %s\n",
	        (1900+t->tm_year), (t->tm_mon+1), t->tm_mday, t->tm_yday,
	        t->tm_hour, t->tm_min, t->tm_sec,
	        t->tm_wday, tzname[t->tm_isdst] );

	printf("m_szTimeZone = %s\n", m_szTimeZone);
#endif
	return m_szTimeZone;
}

void CClockSetView::OnFocus( bool fFocus )
{
	m_fFocus = fFocus;
	OnPaint( rcClient );
}

void CClockSetView::Resize( const CRect& rc, bool fUpdate )
{
	if( rc.GetWidth() < rcClient.GetWidth() ||
	     rc.GetHeight() < rcClient.GetHeight() ) return;
	CWindow::Resize( rc, fUpdate );
}

void CClockSetView::OnPaint( const CRect& rc )
{
	int hoffset;
	int color;
	char	szTemp[5];
	uchar_t ucText[5];
	int 	length;
	int	 offset;
	
	switch( m_TypeOfView )
	{
		case DAY_VIEW:
			sprintf(szTemp,"%d",m_time.tm_mday );
			break;
		case MONTH_VIEW:
			sprintf(szTemp,"%d",m_time.tm_mon+1 );
			break;
		case YEAR_VIEW:
			sprintf(szTemp,"%d",m_time.tm_year+1900 );
			break;
		case HOUR_VIEW:
			break;
		case MINUTE_VIEW:
			sprintf(szTemp,"%d",m_time.tm_min );			
			break;
		case SECOND_VIEW:
			sprintf(szTemp,"%d",m_time.tm_sec );			
			break;
	}
	
	hoffset = rcABS.GetHeight()/2-m_puzLArrow->m_nHeight/2;
	if( m_fFocus ) {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nFocusedColor );
		op_BitBlt( &g_SysPixmap, rcABS.x1, rcABS.y1+hoffset, 
			m_puzLArrow, 0, 0, m_puzLArrow->m_nWidth, m_puzLArrow->m_nHeight );
		op_BitBlt( &g_SysPixmap, rcABS.x2-m_puzRArrow->m_nWidth, rcABS.y1+hoffset, 
			m_puzRArrow, 0, 0, m_puzRArrow->m_nWidth, m_puzRArrow->m_nHeight );
		color = _BLACK;
	}
	else {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nBackgroundColor );
		color = _WHITE;
	}

	hoffset = (rcABS.GetHeight()-12)/2;
	
	if ( m_TypeOfView == HOUR_VIEW )
	{
		if ( m_is24hformat )
		{
			sprintf(szTemp,"%d",m_time.tm_hour );
			str2ustr(ucText, szTemp, sizeof(szTemp));
		}
		else
		{
			if (m_time.tm_hour > 12) 
			{
				sprintf(szTemp,"%d ", m_time.tm_hour - 12 );
				str2ustr(ucText, szTemp, sizeof(szTemp));
				length =  ustrlen( pT->GetTextRes(ML_PM) );
				ustrcat( ucText, pT->GetTextRes(ML_PM) );
			}
			else if (m_time.tm_hour == 12) 
			{
				sprintf(szTemp,"%d ", m_time.tm_hour );
				str2ustr(ucText, szTemp, sizeof(szTemp));
				length =  ustrlen( pT->GetTextRes(ML_PM) );
				ustrcat( ucText, pT->GetTextRes(ML_PM) );
			}
			else
			{	
				if ( m_time.tm_hour == 0 )
					sprintf(szTemp,"%d ", 12 );
				else
					sprintf(szTemp,"%d ", m_time.tm_hour );
				str2ustr(ucText, szTemp, sizeof(szTemp));
				length =  ustrlen( pT->GetTextRes(ML_AM) );
				ustrcat( ucText, pT->GetTextRes(ML_AM) );
			}
		}
	}
	else
	{
		str2ustr(ucText, szTemp, sizeof(szTemp));
	}
	length = pT->GetStringWidth( ucText, 12, 0 );
	offset = (rcABS.GetWidth()-length)/2;
	if( offset < 0 ) offset = 0;
	pT->DrawUC16Text( ucText, rcABS.x1+offset, rcABS.y1+hoffset, color, 12, DT_TRANSPARENCY );
}

void CClockSetView::OnKeypressed( const KeyEvent& evt )
{
	switch( evt.value ) {
	case VK_LEFT:
		switch( m_TypeOfView )
		{
			case DAY_VIEW:
				m_changed = 1;
				m_time.tm_mday --;
				if ( m_time.tm_mday == 0 )
					m_time.tm_mday = m_dm;
				break;
			case MONTH_VIEW:
				m_changed = 1;				
				m_time.tm_mon --;
				if ( m_time.tm_mon < 0 )
					m_time.tm_mon = 11;
				m_dm = days_in_month[leap_year(m_time.tm_year+1900)][m_time.tm_mon+1];		
				break;
			case YEAR_VIEW:
				m_changed = 1;				
				m_time.tm_year --;
				if ( m_time.tm_year < 0 )
					m_time.tm_year = 200;
				break;
			case HOUR_VIEW:
				m_changed = 1;				
				m_time.tm_hour --;
				if ( m_time.tm_hour < 0 )
					m_time.tm_hour = 23;
				break;
			case MINUTE_VIEW:
				m_changed = 1;				
				m_time.tm_min --;
				if ( m_time.tm_min < 0 )
					m_time.tm_min = 59;
				break;
			case SECOND_VIEW:
				m_changed = 1;				
				m_time.tm_sec --;
				if ( m_time.tm_sec < 0 )
					m_time.tm_sec = 59;
				break;
		}
		OnPaint( rcClient );
		Flush();
		break;
		
	case VK_RIGHT:
		switch( m_TypeOfView )
		{
			case DAY_VIEW:
				m_changed = 1;				
				m_time.tm_mday ++;
				if ( m_time.tm_mday > m_dm )
					m_time.tm_mday = 1;
				break;
			case MONTH_VIEW:
				m_changed = 1;				
				m_time.tm_mon ++;
				if ( m_time.tm_mon >= 12 )
					m_time.tm_mon = 0;
				m_dm = days_in_month[leap_year(m_time.tm_year+1900)][m_time.tm_mon+1];
				break;
			case YEAR_VIEW:
				m_changed = 1;				
				m_time.tm_year ++;
				if ( m_time.tm_year >= 200 )
					m_time.tm_year = 0;
				break;
			case HOUR_VIEW:
				m_changed = 1;				
				m_time.tm_hour ++;
				if ( m_time.tm_hour >= 24 )
					m_time.tm_hour = 0;
				break;
			case MINUTE_VIEW:
				m_changed = 1;				
				m_time.tm_min ++;
				if ( m_time.tm_min >=  60 )
					m_time.tm_min = 0;
				break;
			case SECOND_VIEW:
				m_changed = 1;				
				m_time.tm_sec ++;
				if ( m_time.tm_sec >= 60 )
					m_time.tm_sec = 0;
				break;
		}
		OnPaint( rcClient );
		Flush();
		break;
	}
}

void CClockSetView::Show( void )
{
	OnPaint( rcClient );
}

void CClockSetView::Hide( void )
{
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), COLOR_TRANSPARENT );
	op_Flush();
}

void CClockSetView::SetViewType( const TYPE_OF_CLOCKVIEW_t& ViewType )
{
	m_TypeOfView = ViewType;
}
void CClockSetView::SetSelectedView( int nValue )
{
	switch( m_TypeOfView )
	{
		case DAY_VIEW:
			m_time.tm_mday = nValue;
			break;
		case MONTH_VIEW:
			m_time.tm_mon = nValue;
			break;
		case YEAR_VIEW:
			m_time.tm_year = nValue;
			break;
		case HOUR_VIEW:
			m_time.tm_hour = nValue;
			break;
		case MINUTE_VIEW:
			m_time.tm_min = nValue;
			break;
		case SECOND_VIEW:
			m_time.tm_sec = nValue;
			break;
	}
}
int CClockSetView::GetSelectedView( void )
{
	int	nValue;
	
	switch( m_TypeOfView )
	{
		case DAY_VIEW:
			nValue = m_time.tm_mday;
			break;
		case MONTH_VIEW:
			nValue = m_time.tm_mon;
			break;
		case YEAR_VIEW:
			nValue = m_time.tm_year;
			break;
		case HOUR_VIEW:
			nValue = m_time.tm_hour;
			break;
		case MINUTE_VIEW:
			nValue = m_time.tm_min;
			break;
		case SECOND_VIEW:
			nValue = m_time.tm_sec;
			break;
	}
	return nValue;
}

void CClockSetView::SetTimeFormat(int is24hformat)
{
	m_is24hformat = is24hformat;
}

void CClockSetView::SetTimeZone(int nZone)
{
	char szTemp[64];
	char	szGMT[15];
	char szSDen[2];
	char szStartMon[3];
	char szStartDay[3];
	char szEDen[2];
	char szEndMon[3];
	char szEndDay[3];
	
	int nStartMon;
	int nStartDay;
	int nEndMon;
	int nEndDay;

	int jStart;
	int jEnd;

	time_t	t;
	struct tm *ptt;
	
	m_timezone = nZone;
	
	sscanf(arTimeZone[nZone], "%[a-zA-Z0-9=+-],%[A-Z]%[0-9].%[0-9],%[A-Z]%[0-9].%[0-9]", 
		szGMT, szSDen, szStartMon, szStartDay, szEDen, szEndMon, szEndDay);

    	if ( szSDen[0] == 'M' && szEDen[0] == 'M' )
    	{	// have a DST
		nStartMon = atoi( szStartMon );
		nStartDay = atoi( szStartDay );
		nEndMon = atoi( szEndMon );
		nEndDay = atoi( szEndDay );
//		sprintf(szTemp,"%s,%s%d.%d.00,%s%d.%d.00", 
//		zGMT, szSDen, nStartMon, nStartDay, szEDen, nEndMon, nEndDay);			

		// get current date
		time( &t );
		ptt = gmtime( &t );
		// set start date of DST
		ptt->tm_mon = nStartMon;
		ptt->tm_mday = nStartDay;
		t = mktime( ptt );
		ptt = gmtime( &t );
		// get julian day for starting DST
		jStart = ptt->tm_yday;
		DEBUGMSG(1,("DST Start: %d-%d-%d [%d]\n", ptt->tm_year+1900, ptt->tm_mon+1, ptt->tm_mday, ptt->tm_yday ) );
		// set end date of DST
		ptt->tm_mon = nEndMon;
		ptt->tm_mday = nEndDay;
		t = mktime( ptt );
		ptt = gmtime( &t );
		// get julian day for ending DST
		jEnd = ptt->tm_yday;
		DEBUGMSG(1,("DST END: %d-%d-%d [%d]\n", ptt->tm_year+1900, ptt->tm_mon+1, ptt->tm_mday, ptt->tm_yday ) );		

		sprintf(szTemp,"%s,%d,%d", szGMT, jStart, jEnd );

    	}
    	else
    	{	// have not DST
    		sprintf(szTemp,"%s", szGMT );
    	}
    	strcpy ( m_szTimeZone, szTemp );
	DEBUGMSG(1,("m_szTimeZone :%s\n", m_szTimeZone) );
}
