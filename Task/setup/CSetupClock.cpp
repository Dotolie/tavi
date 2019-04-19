//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Clock Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupClock.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/06/12 04:21:40 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: CSetupClock.cpp,v $
// Revision 1.2  2006/06/12 04:21:40  ywkim
// Change external variable to local one for handle_rtc
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.18  2006/02/08 02:44:57  ywkim
// Remark instruction of ioctl in ~CSetupClockWnd
//
// Revision 1.17  2005/12/09 09:30:10  ywkim
// Change order of ML_MAX_12HOUR to first
//
// Revision 1.16  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.15  2005/11/18 03:03:45  zstein
// - save time to rtc when time changed.
//
// Revision 1.14  2005/10/17 06:52:06  zstein
// - changed ComplexListView I/F.
//
// Revision 1.13  2005/09/21 07:12:29  ywkim
// Remark some timezone
//
// Revision 1.12  2005/08/25 07:10:38  ywkim
// divide sideney/melbourne & manila hongkong
//
// Revision 1.11  2005/08/17 09:21:57  ywkim
// Change function for timezone & display
//
// Revision 1.10  2005/07/29 03:12:28  ywkim
// Add item for timezone
//
// Revision 1.9  2005/07/21 09:54:30  ywkim
// change submenu title
//
// Revision 1.8  2005/06/21 12:03:27  ywkim
// remove unused parameterset for CLabel
//
// Revision 1.7  2005/05/14 14:20:43  ywkim
// Change func. for msgbox
//
// Revision 1.6  2005/05/09 14:51:21  ywkim
// Change Charactor string to Resouce Id for m-language
//
// Revision 1.5  2005/05/06 02:55:23  ywkim
// Add set routine of parameter for timeformat
//
// Revision 1.4  2005/05/03 11:53:19  ywkim
// Some change with m-language
//
// Revision 1.3  2005/04/28 06:53:17  ywkim
// Change clocksetup
//
// Revision 1.2  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.1  2005/04/27 05:32:41  ywkim
// Initially make for clock setup
//
//*****************************************************************************

#include "CSetupClock.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/ClockSetView.h>
#include <Driver/rtc/tavi_rtc.h>
#include "../Lib/LibImage.h"

extern BITMAPFILEINFO256 		TAVI_IMAGES[];
extern CTextEngine_BDF* pT;
extern SysParameter_t TaviSysParam;
extern void SendMelonGetDrmInfo( void );

CSetupClockWnd::CSetupClockWnd( CWindow* parent ) :
	CSetupWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_fAction = NULL;
	m_nTitleResID = ML_CLOCK;
	m_nExitResID = ML_EXIT;

}

CSetupClockWnd::~CSetupClockWnd( void )
{
	// SAVE CLOCK TO RTC
	time_t t;
	struct tm* tm;
	struct rtc_time rtime;
	int handle_rtc;
	char Data;
	
	t = time( NULL );
	tm = gmtime( &t );

	if( tm->tm_year >= 100 ) {
		rtime.tm_century = 1;
		rtime.tm_year = tm->tm_year - 100;
	}
	else {
		rtime.tm_century = 0;
		rtime.tm_year = tm->tm_year;
	}
	rtime.tm_mon  = tm->tm_mon+1;
	rtime.tm_mday = tm->tm_mday;
	rtime.tm_hour = tm->tm_hour;
	rtime.tm_min  = tm->tm_min;
	rtime.tm_sec  = tm->tm_sec; 
	rtime.tm_wday = tm->tm_wday;

	int nLastIdx;
	nLastIdx = m_pCMListView->GetCount();
	CListItem *pItem = m_pCMListView->GetItem( nLastIdx );
	CCMListItem *pCMItem = (CCMListItem*)pItem;
	CComplexLabel *pCxLabel =  (CComplexLabel *)pCMItem->m_pComponent;
	CWindow *pWindow = pCxLabel->m_pChild;
	CClockSetView *pClockView = (CClockSetView *)pWindow ;
	
	if ( pClockView->m_changed ) {
		printf("--- 1000th :: pvp : ~CSetupClockWnd : pClockView->m_changed=%d\n", pClockView->m_changed );
		handle_rtc = open( RTC_DRIVER_PATH, O_WRONLY );
		if ( handle_rtc  < 0 ) {
//			printf( "failed to open /dev/misc/rtc\n" );
			} 
		else {
			Data =0;
			ioctl( handle_rtc, RTC_WR_TIMER, &Data);
			close ( handle_rtc );
			} 
		
		SendMelonGetDrmInfo();
		}

}
void CSetupClockWnd::CreateSetup( void )
{
	CLabel* pLabel;
	CSpinSelectBar* pSpinBar;
	CRadioButton* pRadioBtn;
	CComplexLabel* pCxLabel;
	CClockSetView *pClockView;
	CListItem* pItem;
	int bgcolor = 24;
	int focusedcolor = 28;
	int w = m_pCMListView->GetBarWidth()-4;
	int hw = w/2-8; //8 left and right margin
	int h = m_pCMListView->GetBarHeight()-2;

//	printf( "hw: %d, h: %d\n", hw, h );
	

	// Time format
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SETUP_TIME_FORMAT );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL, TRUE, 2);
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	pSpinBar->Add( ML_MAX_12HOUR );
	pSpinBar->Add( ML_MAX_24HOUR );
	pSpinBar->SetSelectIndex( (int)TaviSysParam.etc.timeformat);
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// Time zone
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SET_CLOCK_TIMEZONE );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL, TRUE, 39);
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	pSpinBar->Add( ML_WT_SEOUL );
	pSpinBar->Add( ML_WT_NY );
	pSpinBar->Add( ML_WT_DALLAS  );
	pSpinBar->Add( ML_WT_DENVER );
	pSpinBar->Add( ML_WT_DETROIT );
	pSpinBar->Add( ML_WT_LONDON );
	pSpinBar->Add( ML_WT_ROME );
	pSpinBar->Add( ML_WT_LA );
	pSpinBar->Add( ML_WT_LISBON );
//	pSpinBar->Add( ML_WT_RIO );
	pSpinBar->Add( ML_WT_MANILA );
	pSpinBar->Add( ML_WT_HONGKONG );
	pSpinBar->Add( ML_WT_MADRID );
	pSpinBar->Add( ML_WT_MEXICOCITY );
	pSpinBar->Add( ML_WT_MOSCOW );
	pSpinBar->Add( ML_WT_MONTREAL );
	pSpinBar->Add( ML_WT_BAGHDAD );
	pSpinBar->Add( ML_WT_BANGKOK );
	pSpinBar->Add( ML_WT_BERLIN );
	pSpinBar->Add( ML_WT_BEIRUT );
	pSpinBar->Add( ML_WT_BOSTON );
	pSpinBar->Add( ML_WT_BEIJING );
	pSpinBar->Add( ML_WT_VIENNA );
	pSpinBar->Add( ML_WT_SAMOA );
	pSpinBar->Add( ML_WT_SANFRANCISCO );
//	pSpinBar->Add( ML_WT_SIDNEY );
//	pSpinBar->Add( ML_WT_MELBOURNE );
	pSpinBar->Add( ML_WT_SEATTLE );
	pSpinBar->Add( ML_WT_CHICAGO );
	pSpinBar->Add( ML_WT_SINGAPORE );
	pSpinBar->Add( ML_WT_ATHENS );
	pSpinBar->Add( ML_WT_AMSTERDAM );
	pSpinBar->Add( ML_WT_WASHINGTON );
//	pSpinBar->Add( ML_WT_WELLINGTON );
	pSpinBar->Add( ML_WT_GEORGETOWN );
	pSpinBar->Add( ML_WT_CAIRO );
	pSpinBar->Add( ML_WT_TAIPEI );
	pSpinBar->Add( ML_WT_TEHRAN );
	pSpinBar->Add( ML_WT_PARIS );
	pSpinBar->Add( ML_WT_PHOENIX );
	pSpinBar->Add( ML_WT_HANOVER );
	pSpinBar->Add( ML_WT_HELSINKI );
	pSpinBar->Add( ML_WT_HONOLULU );
	pSpinBar->SetSelectIndex( (int)TaviSysParam.etc.timezone );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );


	// Day
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_DAY);
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pClockView = new CClockSetView( NULL );
	pClockView->SetTimeZone((int)TaviSysParam.etc.timezone);
	pClockView->ReadTime();							// clock read
	pClockView->m_nBackgroundColor = bgcolor;
	pClockView->m_nBorderColor = bgcolor;
	pClockView->Resize( CRect(0, 0, hw, h), false );
	pClockView->SetViewType( DAY_VIEW );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pClockView );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// Month
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_MONTH );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pClockView = new CClockSetView( NULL );
	pClockView->m_nBackgroundColor = bgcolor;
	pClockView->m_nBorderColor = bgcolor;
	pClockView->Resize( CRect(0, 0, hw, h), false );
	pClockView->SetViewType( MONTH_VIEW );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pClockView );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// Year
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_YEAR );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pClockView = new CClockSetView( NULL );
	pClockView->m_nBackgroundColor = bgcolor;
	pClockView->m_nBorderColor = bgcolor;
	pClockView->Resize( CRect(0, 0, hw, h), false );
	pClockView->SetViewType( YEAR_VIEW );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pClockView );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// Hour
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_HOURS );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pClockView = new CClockSetView( NULL );
	pClockView->m_nBackgroundColor = bgcolor;
	pClockView->m_nBorderColor = bgcolor;
	pClockView->Resize( CRect(0, 0, hw, h), false );
	pClockView->SetViewType( HOUR_VIEW );
	pClockView->SetTimeFormat( TaviSysParam.etc.timeformat );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pClockView );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// Minute
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_MINUTES );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pClockView = new CClockSetView( NULL );
	pClockView->m_nBackgroundColor = bgcolor;
	pClockView->m_nBorderColor = bgcolor;
	pClockView->Resize( CRect(0, 0, hw, h), false );
	pClockView->SetViewType( MINUTE_VIEW );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pClockView );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
	m_pCMListView->ResetScrollBar();
/*
	// Second
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SETUP_SEC );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontType = __SKIP_TRANSPARENCY_COLOR;
	pLabel->m_nFontIdx = GULIM_13_WHITE_NORMAL_BORDER;
	pLabel->m_nFocusedFontIdx = GULIM_13_BLACK_BOLD;	
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pClockView = new CClockSetView( NULL );
	pClockView->m_nBackgroundColor = bgcolor;
	pClockView->m_nBorderColor = bgcolor;
	pClockView->Resize( CRect(0, 0, hw, h), false );
	pClockView->SetViewType( SECOND_VIEW );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pClockView );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
*/
}

void CSetupClockWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;

	event.type = EVENT_KEY;
	event.u.key = evt;
	m_pCMListView->SendEvent( event );
}

