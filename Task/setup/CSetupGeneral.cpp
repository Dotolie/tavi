//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup General Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupGeneral.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 07:56:19 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupGeneral.cpp,v $
// Revision 1.2  2006/06/07 07:56:19  zstein
// - added sleep timer item.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.25  2005/12/11 11:21:13  yong
// add WARNING comment about Touch speed index and Music player's FF/FR time
//
// Revision 1.24  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.23  2005/11/23 08:34:18  ywkim
// Add item of setup control for speed of touchpad
//
// Revision 1.22  2005/11/22 11:01:08  ywkim
// Change setup items
//
// Revision 1.21  2005/08/12 12:44:00  ywkim
// change some resource of item
//
// Revision 1.20  2005/07/21 09:54:30  ywkim
// change submenu title
//
// Revision 1.19  2005/07/18 06:48:16  ywkim
// Add item of Output & change other items
//
// Revision 1.18  2005/06/21 12:03:27  ywkim
// remove unused parameterset for CLabel
//
// Revision 1.17  2005/05/27 13:09:35  ywkim
// Change setup of firmware update
//
// Revision 1.16  2005/05/17 20:44:26  ywkim
// add color setting on spinselectbar
//
// Revision 1.15  2005/05/17 20:01:37  ywkim
// Add null spinselectbar for ">" prompt
//
// Revision 1.14  2005/05/17 13:51:51  ywkim
// remove parameter of TaviSysParam.theme
//
// Revision 1.13  2005/05/13 11:32:38  ywkim
// Change resource num for selection language
//
// Revision 1.12  2005/05/12 14:42:35  etnlwind
// Update for Japanise Language Support
//
// Revision 1.11  2005/05/09 14:51:21  ywkim
// Change Charactor string to Resouce Id for m-language
//
// Revision 1.10  2005/05/03 11:53:20  ywkim
// Some change with m-language
//
// Revision 1.9  2005/04/28 06:53:17  ywkim
// Change clocksetup
//
// Revision 1.8  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.7  2005/04/27 05:30:28  ywkim
// Modify show or onPaint for m-language
//
// Revision 1.6  2005/03/29 01:42:10  ywkim
// Add class of CSetupSound
//
// Revision 1.5  2005/03/05 19:07:45  ywkim
// Add function for diskspace & sw version
//
// Revision 1.4  2005/03/05 06:29:37  ywkim
// Add function of touchpad on sub-setup-menu
//
// Revision 1.3  2005/03/04 16:08:24  ywkim
// add fuction of progressbar for flashupdate
//
// Revision 1.2  2005/03/03 16:39:08  ywkim
// Change parameter on Resize routine
//
// Revision 1.1  2005/03/03 04:55:07  ywkim
// New release
//
//*****************************************************************************

#include "CSetupGeneral.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include "../Lib/LibImage.h"

extern BITMAPFILEINFO256 		TAVI_IMAGES[];
extern CTextEngine_BDF* pT;
extern SysParameter_t TaviSysParam;

static int	nSleepTimer[14] = {0, 1, 2, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };


CSetupGeneralWnd::CSetupGeneralWnd( CWindow* parent ) :
	CSetupWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_fAction = NULL;
	m_nTitleResID = ML_GENERAL;
	m_nExitResID = ML_EXIT;

}

CSetupGeneralWnd::~CSetupGeneralWnd( void )
{
}
void CSetupGeneralWnd::CreateSetup( void )
{
	CLabel* pLabel;
	CSpinSelectBar* pSpinBar;
	CSlideBar *pSlideBar;
	CRadioButton* pRadioBtn;
	CComplexLabel* pCxLabel;
	CListItem* pItem;
	int bgcolor = 24;
	int focusedcolor = 28;
	int w = m_pCMListView->GetBarWidth()-4;
	int hw = w/2-8; //8 left and right margin
	int h = m_pCMListView->GetBarHeight()-2;

//	printf( "hw: %d, h: %d\n", hw, h );
	
	// Language
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_LANGUAGE );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL, TRUE, 3 );
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	pSpinBar->Add( ML_NULL );
	pSpinBar->Add( ML_NULL ); 
	pSpinBar->Add( ML_NULL ); 
	pSpinBar->SetSelectIndex( (int)TaviSysParam.etc.language );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
/*
	// BackGround
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_BG );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL, TRUE, 2 );
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	pSpinBar->Add( ML_DEFAULT_THEME );
	pSpinBar->Add( ML_SETUP_USER_DEFINE );
	pSpinBar->SetSelectIndex( (int)TaviSysParam.gui.isUserBackground );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
*/

/*
	// Startup
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_STARTUP );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL, TRUE, 2 );
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	pSpinBar->Add( ML_RESUME );
	pSpinBar->Add( ML_SETUP_MAIN_MODE );
	pSpinBar->SetSelectIndex( (int)TaviSysParam.etc.startup );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
*/
	// Tun region
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_TUNER_REGION );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL, TRUE, 4 );
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	pSpinBar->Add( ML_USA );
	pSpinBar->Add( ML_KOREA );
	pSpinBar->Add( ML_EU );
	pSpinBar->Add( ML_JAPAN );	
	pSpinBar->SetSelectIndex( (int)TaviSysParam.radio.cRegion );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// Beep on/off
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_BEEP );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pRadioBtn = new CRadioButton( NULL, TRUE );
	pRadioBtn->m_nBackgroundColor = bgcolor;
	pRadioBtn->m_nBorderColor = bgcolor;
	pRadioBtn->Resize( CRect(0, 0, hw, h), false );
	pRadioBtn->AddItem( ML_ON, true );
	pRadioBtn->AddItem( ML_OFF, false );
	pRadioBtn->SetSelectIndex( TaviSysParam.etc.buzzer);
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pRadioBtn );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// touch speed
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_TOUCH_SPEED );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL, TRUE, 4 );
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	/*
		WARNING!

		if these SPEED indexes are changed, you SHOULD also change below:

		CPlayerBottomWnd::OnMouseEvent()

	*/
	pSpinBar->Add( ML_TOUCH_SPEED_VERYFAST );
	pSpinBar->Add( ML_TEXT_AUTOSCROLL_FAST );
	pSpinBar->Add( ML_TEXT_AUTOSCROLL_MID );
	pSpinBar->Add( ML_TEXT_AUTOSCROLL_SLOW );	
	pSpinBar->SetSelectIndex( (int)TaviSysParam.etc.touchspeed);

	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// sleep timer
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SLEEP_TIMER );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->AddPreset( nSleepTimer, sizeof(nSleepTimer)/sizeof(int) );
	pSlideBar->SetValue( TaviSysParam.power.sleeptimer );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );


	// Load Default
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_LOAD_DEFAULT );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL, TRUE, 0 );
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
/*
	// Firmware Upgrade
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_FIRMWARE_UPGRADE );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontType = __SKIP_TRANSPARENCY_COLOR;
	pLabel->m_nFontIdx = GULIM_13_WHITE_NORMAL_BORDER;
	pLabel->m_nFocusedFontIdx = GULIM_13_BLACK_BOLD;	
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL, TRUE, 0 );
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
*/
}

void CSetupGeneralWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;

	event.type = EVENT_KEY;
	event.u.key = evt;
	m_pCMListView->SendEvent( event );
}

