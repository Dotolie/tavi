//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Sound Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupSound.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupSound.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.12  2005/12/15 03:59:17  akaiaphanu
// updated ML_SOUND_OUTPUT_LEVEL
//
// Revision 1.11  2005/12/14 11:33:56  yong
// sysparam.sound.output_level
//
// Revision 1.10  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.9  2005/11/22 11:01:08  ywkim
// Change setup items
//
// Revision 1.8  2005/08/12 12:44:00  ywkim
// change some resource of item
//
// Revision 1.7  2005/07/21 09:54:30  ywkim
// change submenu title
//
// Revision 1.6  2005/06/21 12:03:27  ywkim
// remove unused parameterset for CLabel
//
// Revision 1.5  2005/05/09 14:51:21  ywkim
// Change Charactor string to Resouce Id for m-language
//
// Revision 1.4  2005/05/03 11:53:20  ywkim
// Some change with m-language
//
// Revision 1.3  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.2  2005/04/27 05:30:28  ywkim
// Modify show or onPaint for m-language
//
// Revision 1.1  2005/03/29 01:43:14  ywkim
// Initial revision for sound setup
//
//*****************************************************************************

#include "CSetupSound.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include "../Lib/LibImage.h"

extern SysParameter_t			TaviSysParam;
extern CTextEngine_BDF* pT;

CSetupSoundWnd::CSetupSoundWnd( CWindow* parent ) :
	CSetupWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_fAction = NULL;
	m_nTitleResID = ML_SOUND;
	m_nExitResID = ML_EXIT;

}

CSetupSoundWnd::~CSetupSoundWnd( void )
{
}

void CSetupSoundWnd::CreateSetup( void )
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


#if 0

	// Speaker on/off
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SPEAKER );
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
	pRadioBtn->SetSelectIndex( TaviSysParam.sound.speaker_mute);
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pRadioBtn );
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

#endif

	// Balance
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_BALANCE );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(-20, 20);
	pSlideBar->SetValue(TaviSysParam.sound.balance);
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );


    // Output Level
    pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SOUND_OUTPUT_LEVEL);
    pLabel->m_nBackgroundColor = bgcolor;
    pLabel->m_nBorderColor = bgcolor;
    pLabel->Resize( CRect(0, 0, hw, h), false );
    pLabel->m_nFontWidth = 16;
    pLabel->m_nFontHeight = 15;

    pRadioBtn = new CRadioButton( NULL, TRUE );
    pRadioBtn->m_nBackgroundColor = bgcolor;
    pRadioBtn->m_nBorderColor = bgcolor;
    pRadioBtn->Resize( CRect(0, 0, hw, h), false );
	// XXX
    pRadioBtn->AddItem( ML_SOUND_OUTPUT_LEVEL_NORMAL, true );
    pRadioBtn->AddItem( ML_SOUND_OUTPUT_LEVEL_HIGH, false );
    pRadioBtn->SetSelectIndex(TaviSysParam.sound.output_level);

    pCxLabel = new CComplexLabel( NULL );
    pCxLabel->m_nBackgroundColor = bgcolor;
    pCxLabel->m_nBorderColor = bgcolor;
    pCxLabel->Resize( CRect(0, 0, w, h), false );
    pCxLabel->SetLabel( pLabel );
    pCxLabel->AddComponent( pRadioBtn );
    pItem = new CCMListItem( pCxLabel );
    m_pCMListView->AddItem( pItem );

	// User Effect
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_USER_EFFECT );
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

	// User EQ
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_USER_EQ );
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

}

void CSetupSoundWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;

	event.type = EVENT_KEY;
	event.u.key = evt;
	m_pCMListView->SendEvent( event );
}

