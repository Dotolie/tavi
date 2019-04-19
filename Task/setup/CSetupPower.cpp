//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Power Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupPower.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 07:57:07 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupPower.cpp,v $
// Revision 1.2  2006/06/07 07:57:07  zstein
// - added power management preset.
// - moved sleep timer to general.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.10  2005/11/22 11:01:08  ywkim
// Change setup items
//
// Revision 1.9  2005/10/25 02:34:35  ywkim
// Add control for hdd sleep
//
// Revision 1.8  2005/08/12 12:44:00  ywkim
// change some resource of item
//
// Revision 1.7  2005/07/21 09:54:30  ywkim
// change submenu title
//
// Revision 1.6  2005/07/18 06:48:16  ywkim
// Add item of Output & change other items
//
// Revision 1.5  2005/06/21 12:03:27  ywkim
// remove unused parameterset for CLabel
//
// Revision 1.4  2005/05/09 14:51:21  ywkim
// Change Charactor string to Resouce Id for m-language
//
// Revision 1.3  2005/05/03 11:53:20  ywkim
// Some change with m-language
//
// Revision 1.2  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.1  2005/04/27 05:30:28  ywkim
// Modify show or onPaint for m-language
//
//*****************************************************************************

#include "CSetupPower.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include "../Lib/LibImage.h"
#include "setup.h"

extern SysParameter_t			TaviSysParam;
extern CTextEngine_BDF* pT;

int	nStopPower[14] = {0, 1, 2, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
int	nLcdOff[8]= {0, 5, 10, 20, 30, 40, 50, 60};


CSetupPowerWnd::CSetupPowerWnd( CWindow* parent ) :
	CSetupWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_fAction = NULL;
	m_nTitleResID = ML_POWER;
	m_nExitResID = ML_EXIT;
}

CSetupPowerWnd::~CSetupPowerWnd( void )
{
}

void CSetupPowerWnd::CreateSetup( void )
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

  // PM preset
  pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_POWER );
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
	pSpinBar->Add( ML_SETUP_PM_NORMAL );
	pSpinBar->Add( ML_SETUP_PM_AUTO_NORMAL ); 
	pSpinBar->Add( ML_SETUP_PM_AUTO_MAX ); 
	pSpinBar->SetSelectIndex( (int)TaviSysParam.power.preset );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// stop power off
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_AUTO_POWER_OFF );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->AddPreset( nStopPower, sizeof(nStopPower)/sizeof(int) );
	pSlideBar->SetValue( TaviSysParam.power.stoptimer );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// lcd off
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_LCD_OFF );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->AddPreset( nLcdOff, sizeof(nLcdOff)/sizeof(int) );
	pSlideBar->SetValue( TaviSysParam.power.lcdofftimer );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
/*
	// button led on/off
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SETUP_POWER_BUTTON_LED );
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
	pRadioBtn->SetSelectIndex( TaviSysParam.power.ledonoff);
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pRadioBtn );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );
*/
	// auto lcd brightness on/off
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SETUP_AUTO_LCD_BRIGHTNESS );
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
	pRadioBtn->SetSelectIndex( TaviSysParam.power.autolcdbright);
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pRadioBtn );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// hdd sleep on/off
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_HDD_SLEEP );
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
	pRadioBtn->SetSelectIndex( ((TaviSysParam.power.hddtimer==0)?1:0) );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pRadioBtn );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

  Preset( TaviSysParam.power.preset );
}

void CSetupPowerWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;

	event.type = EVENT_KEY;
	event.u.key = evt;
	m_pCMListView->SendEvent( event );
}

void CSetupPowerWnd::Preset( int preset )
{
  int cnt = m_pCMListView->GetCount();
  CCMListItem* item;
  CComplexLabel* l;
  CWindow* w;
  CLabel* lb;
  bool disable = false;
  int *p;

  if( preset != PM_PRESET_OFF ) {
    disable = true;
  }
  p = (int*)&TaviSysParam.pm_preset[preset];
  TaviSysParam.power.stoptimer = TaviSysParam.pm_preset[preset].idle_off;
  TaviSysParam.power.lcdofftimer = TaviSysParam.pm_preset[preset].lcd_off;
  TaviSysParam.power.autolcdbright = TaviSysParam.pm_preset[preset].auto_br;
  TaviSysParam.power.hddtimer = TaviSysParam.pm_preset[preset].hdd_sleep;

  for( int i=1; i<cnt; i++ ) {
    item = ( CCMListItem* )m_pCMListView->GetItem( i );
    l = ( CComplexLabel* )item->m_pComponent;
    w = l->GetComponent();
    lb = l->GetLabel();

    w->SetDisable( disable );
    lb->SetDisable( disable );
    switch( w->GetWindowType() ) {
    case WINDOW_TYPE_SLIDEBAR:
      ((CSlideBar*)w)->SetValue( p[i-1] );
      break;
    case WINDOW_TYPE_SPINBAR:
      ((CSpinSelectBar*)w)->SetSelectIndex( p[i-1] );
      break;
    case WINDOW_TYPE_RADIOBUTTON:
      if( i==(cnt-1) ) {
        ((CRadioButton*)w)->SetSelectIndex( p[i-1]? 0 : 1 );
      }
      else {
        ((CRadioButton*)w)->SetSelectIndex( p[i-1]? 1 : 0 );
      }
      break;
    }
  }
  Show();
}
