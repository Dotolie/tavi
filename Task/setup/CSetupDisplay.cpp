//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup General Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupDisplay.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupDisplay.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.20  2006/02/21 09:05:13  zstein
// - changed label name.(ML_VS_BRIGHTNESS to ML_VP_BRIGHTNESS_CONTRAST)
//
// Revision 1.19  2005/12/15 08:44:00  akaiaphanu
// Updated to adjust brightness & contrast functions
//
// Revision 1.18  2005/12/13 09:00:51  ywkim
// Change max value of brightness & contrast
//
// Revision 1.17  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.16  2005/10/25 02:34:16  ywkim
// Add control IF for brightness & contrast
//
// Revision 1.15  2005/07/29 03:11:07  ywkim
// Replaye ML_BG to ML_BG_IMAGE
//
// Revision 1.14  2005/07/21 09:54:30  ywkim
// change submenu title
//
// Revision 1.13  2005/07/18 06:48:16  ywkim
// Add item of Output & change other items
//
// Revision 1.12  2005/06/21 12:03:27  ywkim
// remove unused parameterset for CLabel
//
// Revision 1.11  2005/05/17 19:13:37  ywkim
// Change displaysetup
//
// Revision 1.10  2005/05/15 13:27:27  ywkim
// Remove TaviSysParam.etc.display
//
// Revision 1.9  2005/05/12 09:23:53  ywkim
// Change text to resource
//
// Revision 1.8  2005/05/09 14:51:21  ywkim
// Change Charactor string to Resouce Id for m-language
//
// Revision 1.7  2005/05/03 11:53:19  ywkim
// Some change with m-language
//
// Revision 1.6  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.5  2005/03/29 01:42:10  ywkim
// Add class of CSetupSound
//
// Revision 1.4  2005/03/05 19:07:45  ywkim
// Add function for diskspace & sw version
//
// Revision 1.3  2005/03/05 06:29:37  ywkim
// Add function of touchpad on sub-setup-menu
//
// Revision 1.2  2005/03/04 16:08:24  ywkim
// add fuction of progressbar for flashupdate
//
// Revision 1.1  2005/03/03 04:55:07  ywkim
// New release
//
//*****************************************************************************

#include "CSetupDisplay.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include "../Lib/LibImage.h"

extern SysParameter_t			TaviSysParam;
extern CTextEngine_BDF* pT;
extern RMint32	toggle_display;

CSetupDisplayWnd::CSetupDisplayWnd( CWindow* parent ) :
	CSetupWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_fAction = NULL;
	m_nTitleResID = ML_DISPLAY;
	m_nExitResID = ML_EXIT;
}

CSetupDisplayWnd::~CSetupDisplayWnd( void )
{
}

void CSetupDisplayWnd::CreateSetup( void )
{
	CLabel* pLabel;
	CSpinSelectBar* pSpinBar;
	CRadioButton* pRadioBtn;
	CSlideBar *pSlideBar;
	CComplexLabel* pCxLabel;
	CListItem* pItem;
	int bgcolor = 24;
	int focusedcolor = 28;
	int w = m_pCMListView->GetBarWidth()-4;
	int hw = w/2-8; //8 left and right margin
	int h = m_pCMListView->GetBarHeight()-2;

//	printf( "CreateSetupDisplay: hw=%d, h=%d\n", hw, h );


	// BackGround
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_BG_IMAGE);
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

	// brightness & contrast
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_VP_BRIGHTNESS_CONTRAST );
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

	// lcd brigth
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_LCD_BRIGHT );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(0, 10);
	pSlideBar->SetValue(TaviSysParam.etc.lcdbright);
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

}

void CSetupDisplayWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;

	event.type = EVENT_KEY;
	event.u.key = evt;
	m_pCMListView->SendEvent( event );
}

