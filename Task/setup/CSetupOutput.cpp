//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Output Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupOutput.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupOutput.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.9  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.8  2005/09/27 05:34:54  ywkim
// Remark item of screen ratio
//
// Revision 1.7  2005/08/23 06:07:41  ywkim
// Add item of ML_NORMAL on TV Ratio
//
// Revision 1.6  2005/08/22 10:08:05  ywkim
// Change routine for toggle_diaplay
//
// Revision 1.5  2005/08/09 08:14:13  ywkim
// Add function for screen ratio
//
// Revision 1.4  2005/08/08 11:33:28  ywkim
// Change soume routine
//
// Revision 1.3  2005/07/29 03:12:06  ywkim
// Add item for adjust Tv
//
// Revision 1.2  2005/07/21 09:54:30  ywkim
// change submenu title
//
// Revision 1.1  2005/07/18 06:48:16  ywkim
// Add item of Output & change other items
//
//*****************************************************************************

#include "CSetupOutput.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include "../Lib/LibImage.h"

extern SysParameter_t			TaviSysParam;
extern CTextEngine_BDF* pT;
extern RMint32	toggle_display;

CSetupOutputWnd::CSetupOutputWnd( CWindow* parent ) :
	CSetupWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_fAction = NULL;
	m_nTitleResID = ML_SETUP_OUTPUT;
	m_nExitResID = ML_EXIT;
}

CSetupOutputWnd::~CSetupOutputWnd( void )
{
}

void CSetupOutputWnd::CreateSetup( void )
{
	CLabel* pLabel;
	CSpinSelectBar* pSpinBar;
	CRadioButton* pRadioBtn;
	CComplexLabel* pCxLabel;
	CListItem* pItem;
	int bgcolor = 24;
	int focusedcolor = 28;
	int w = m_pCMListView->GetBarWidth()-4;
	int hw = w/2-8; //8 left and right margin
	int h = m_pCMListView->GetBarHeight()-2;
	int display;
//	printf( "CreateSetupDisplay: hw=%d, h=%d\n", hw, h );


	// Video Output
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_VIDEO_OUTPUT );
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
	pSpinBar->Add( ML_LCD );
	pSpinBar->Add( ML_SETUP_COMPOSITE );
	pSpinBar->Add( ML_SETUP_COMPONENT );
	pSpinBar->Add( ML_SETUP_HD_COMPONENT );
	switch( toggle_display )
	{
		case 0:
			display =0;
			break;
		case 1:
		case 2:
			display = 1;
			break;
		case 3:
		case 4:
			display = 2;
			break;
		default:
			display = 3;
			break;
	}
	pSpinBar->SetSelectIndex( display );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// Video Type
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SETUP_TV_TYPE );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pRadioBtn = new CRadioButton( NULL, TRUE );
	pRadioBtn->m_nBackgroundColor = bgcolor;
	pRadioBtn->m_nBorderColor = bgcolor;
	pRadioBtn->Resize( CRect(0, 0, hw, h), false );
	pRadioBtn->AddItem( ML_NTSC, true );
	pRadioBtn->AddItem( ML_PAL, false );
	pRadioBtn->SetSelectIndex( TaviSysParam.etc.tvtype);
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pRadioBtn );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
/*
	// Screen Ratio
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SETUP_TV_RATIO );
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
	pSpinBar->Add( ML_NORMAL );
	pSpinBar->Add( ML_LETTER_BOX );
	pSpinBar->Add( ML_43_PAN_SCAN );
	pSpinBar->Add( ML_SETUP_1609 );
	pSpinBar->SetSelectIndex( TaviSysParam.etc.screenratio);
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
*/
	// HD Component
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SETUP_HD_COMPONENT );
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
	pSpinBar->Add( ML_480P );
	pSpinBar->Add( ML_720P );
	pSpinBar->Add( ML_SETUP_1080 );
//	pSpinBar->Add( ML_OFF );
	pSpinBar->SetSelectIndex( TaviSysParam.etc.hdcomponent );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// adjust Tv Screen Default
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SET_OUT_ADJUST_TVSCREEN );
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

void CSetupOutputWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;

	event.type = EVENT_KEY;
	event.u.key = evt;
	m_pCMListView->SendEvent( event );
}

