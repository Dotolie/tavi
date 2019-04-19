//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Recorder Setup Window

// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecSetupWnd.cpp,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: RecSetupWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/07/16 03:09:37  zstein
// - start
//
//

//*******************************************************************************
#include "RecSetupWnd.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include "../Lib/LibImage.h"

// Macros

// Extern Variables

// Local Variables
static char* szBitrate[] = { "8", "16", "20", "24", "32" };
#define BITRATE_COUNT			5
static char* szSamplingrate[] = { "44100", "32000" };
#define SAMPLINGRATE_COUNT	3

CRecSetupWnd::CRecSetupWnd( CWindow* parent ) :
	CModalWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_szTitle = "Setup";
}

CRecSetupWnd::~CRecSetupWnd( void )
{
}

void CRecSetupWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;

	event.type = EVENT_KEY;
	event.u.key = evt;
	m_pCMListView->SendEvent( event );
	switch( evt.value ) {
	case VK_ESC:
		m_fOK = true;
		break;
	}
}

void CRecSetupWnd::OnMouseEvent( const MouseEvent& evt )
{
	CEvent event;

	event.type = EVENT_MOUSE;
	event.u.mouse = evt;
	m_pCMListView->SendEvent( event );
}

void CRecSetupWnd::Resize( const CRect& rc, bool fUpdate )
{
	CWindow::Resize( rc, fUpdate );
	// Resize Complext ListView
	CRect rc2 = rc;
	rc2.x1 = 2;
	rc2.y1 = 30;
	rc2.x2 = rc.GetWidth()-1;
	rc2.y2 = 177;
	m_pCMListView->Resize( rc2, fUpdate );
	m_pCMListView->m_nBackgroundColor = m_nBackgroundColor;
}

// Create(...) must be called after Resize(...)
void CRecSetupWnd::CreateSetup( void )
{
	CLabel* pLabel;
	CSpinSelectBar* pSpinBar;
	CRadioButton* pRadioBtn;
	CComplexLabel* pCxLabel;
	CSlideBar* pSlideBar;
	CListItem* pItem;
	int bgcolor = 24;
	int focusedcolor = 28;
	int w = m_pCMListView->GetBarWidth()-4;
	int hw = w/2-8; //8 left and right margin
	int h = m_pCMListView->GetBarHeight()-2;
	int i;

	// Input Volume
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, "In Volume" );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw-20, h), false );
	pLabel->m_nFontType = __SKIP_TRANSPARENCY_COLOR;
	pLabel->m_nFontIdx = GULIM_13_WHITE_NORMAL_BORDER;
	pLabel->m_nFocusedFontIdx = GULIM_13_BLACK_BOLD;
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(0, 63);
	pSlideBar->SetValue( TaviSysParam.record.input_vol );
	pSlideBar->Add( "  " );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
	// Bitrate
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, "Bitrate" );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw-20, h), false );
	pLabel->m_nFontType = __SKIP_TRANSPARENCY_COLOR;
	pLabel->m_nFontIdx = GULIM_13_WHITE_NORMAL_BORDER;
	pLabel->m_nFocusedFontIdx = GULIM_13_BLACK_BOLD;
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL );
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	for( i=0; i<BITRATE_COUNT; i++ ) pSpinBar->Add( szBitrate[i] );
	pSpinBar->SetSelectIndex( TaviSysParam.record.bitrate );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
	// sampling rate
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, "Samplingrate" );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw-20, h), false );
	pLabel->m_nFontType = __SKIP_TRANSPARENCY_COLOR;
	pLabel->m_nFontIdx = GULIM_13_WHITE_NORMAL_BORDER;
	pLabel->m_nFocusedFontIdx = GULIM_13_BLACK_BOLD;
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSpinBar = new CSpinSelectBar( NULL );
	pSpinBar->m_nFocusedColor = focusedcolor;
	pSpinBar->m_nBackgroundColor = bgcolor;
	pSpinBar->m_nBorderColor = bgcolor;
	pSpinBar->Resize( CRect(0, 0, hw, h), false );
	for( i=0; i<SAMPLINGRATE_COUNT; i++ ) pSpinBar->Add( szSamplingrate[i] );
	pSpinBar->SetSelectIndex( TaviSysParam.record.samplingrate );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
	// Auto gain
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, "Auto Gain" );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw-20, h), false );
	pLabel->m_nFontType = __SKIP_TRANSPARENCY_COLOR;
	pLabel->m_nFontIdx = GULIM_13_WHITE_NORMAL_BORDER;
	pLabel->m_nFocusedFontIdx = GULIM_13_BLACK_BOLD;
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pRadioBtn = new CRadioButton( NULL );
	pRadioBtn->m_nBackgroundColor = bgcolor;
	pRadioBtn->m_nBorderColor = bgcolor;
	pRadioBtn->Resize( CRect(0, 0, hw, h), false );
	pRadioBtn->AddItem( "On", true );
	pRadioBtn->AddItem( "Off", false );
	pRadioBtn->SetSelectIndex( !TaviSysParam.record.AGC ); // 0: on, 1: off
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pRadioBtn );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
}

int CRecSetupWnd::GetBitrate( void )
{
	CCMListItem* pItem = ( CCMListItem* )m_pCMListView->GetItem( 1 );
	CComplexLabel* pCxLabel = ( CComplexLabel* )pItem->m_pComponent;
	CSpinSelectBar* pSpin = ( CSpinSelectBar* )pCxLabel->GetComponent();

	return pSpin->GetSelectedIndex(); 
}

int CRecSetupWnd::GetInputVol( void )
{
	CCMListItem* pItem = ( CCMListItem* )m_pCMListView->GetItem( 0 );
	CComplexLabel* pCxLabel = ( CComplexLabel* )pItem->m_pComponent;
	CSlideBar* pSlideBar = ( CSlideBar* )pCxLabel->GetComponent();

	return pSlideBar->GetValue();
}

int CRecSetupWnd::GetSamplingrate( void )
{
	CCMListItem* pItem = ( CCMListItem* )m_pCMListView->GetItem( 2 );
	CComplexLabel* pCxLabel = ( CComplexLabel* )pItem->m_pComponent;
	CSpinSelectBar* pSpin = ( CSpinSelectBar* )pCxLabel->GetComponent();

	return pSpin->GetSelectedIndex(); 
}

bool CRecSetupWnd::GetAutoGain( void )
{
	bool ret;
	CCMListItem* pItem = ( CCMListItem* )m_pCMListView->GetItem( 3 );
	CComplexLabel* pCxLabel = ( CComplexLabel* )pItem->m_pComponent;
	CRadioButton* pRadio = ( CRadioButton* )pCxLabel->GetComponent();

	// 0 -> On
	// 1 -> Off
	ret = !(bool)pRadio->GetSelectedIndex();
	return ret;
}
