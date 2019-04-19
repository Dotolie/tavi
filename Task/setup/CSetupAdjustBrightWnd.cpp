//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Adjust Brightness & Contrast Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupAdjustBrightWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupAdjustBrightWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2006/01/24 04:26:50  ywkim
// Change inital value of Saturation in CreateSetup()
//
// Revision 1.4  2006/01/11 09:28:05  ywkim
// Add control of saturation
//
// Revision 1.3  2005/12/16 06:57:35  akaiaphanu
// Updated text resources
//
// Revision 1.2  2005/12/15 09:40:22  akaiaphanu
// updated to adjust brightness value
//
// Revision 1.1  2005/12/15 08:29:44  akaiaphanu
// added new component
//
// 
//*****************************************************************************

#include "CSetupAdjustBrightWnd.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include "../Lib/LibImage.h"
#include "setup.h"

extern SysParameter_t			TaviSysParam;
extern CTextEngine_BDF* pT;
extern RMint32	toggle_display;
extern TSETUP_CONFIG _SetupProperty;
extern void showbackground( int nMenuId );

CSetupAdjustBrightWnd::CSetupAdjustBrightWnd( CWindow* parent ) :
	CSetupWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_fAction = NULL;
	m_nTitleResID = ML_VP_BRIGHTNESS_CONTRAST;
	m_nExitResID = ML_EXIT;
}

CSetupAdjustBrightWnd::~CSetupAdjustBrightWnd( void )
{
}

void CSetupAdjustBrightWnd::CreateSetup( void )
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

	// brightness
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_VS_BRIGHTNESS );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(0, 60);
	pSlideBar->SetValue( TaviSysParam.video.brightness );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// Contrast
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_VS_CONTRAST );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(0, 60);
	pSlideBar->SetValue( TaviSysParam.video.contrast );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// Saturation
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_VS_SATURATION );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(0, 60);
	pSlideBar->SetValue( TaviSysParam.video.saturation );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

}

void CSetupAdjustBrightWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;
	CCMListItem* pItem;
	CWindow *pComponentWnd;
	int	nIdxCMItem;
	
	event.type = EVENT_KEY;
	event.u.key = evt;
	m_pCMListView->SendEvent( event );

	pItem = ( CCMListItem* )m_pCMListView->GetSelectedItem();
	pComponentWnd = ((CComplexLabel*)(pItem->m_pComponent))->GetComponent();
	nIdxCMItem = pItem->m_nIdx;
	
	switch( evt.value ) {
		case VK_ENTER:
			break;

		case VK_ESC:
			TAVI_CannotSleep(9);
			showbackground(ID_BG_SETUP);
			MSG_Send( _SetupProperty.m_nTaskID, MSG_DRAW, 0, WINDOW_KILL_FOCUS );
			_SetupProperty.m_pAdjustBright = NULL;
			TAVI_CanSleep(9);
			break;
			
		case VK_LEFT:
			switch (nIdxCMItem ) 
			{
				case 0:	// brightness
					if(TaviSysParam.video.brightness > 0)
					{
						 TaviSysParam.video.brightness -= 1;
					}
					break;
					
				case 1:	// contrast
					if(TaviSysParam.video.contrast > 0)
					{
						TaviSysParam.video.contrast -= 1;
					}
					break;

				case 2:	// saturation
					if(TaviSysParam.video.saturation > 0)
					{
						TaviSysParam.video.saturation -= 1;
					}
					break;
					
			}
			pMpegDecoder->Resume_ColorTemperature(
				TaviSysParam.video.brightness, TaviSysParam.video.contrast, TaviSysParam.video.saturation);
			break;
			
		case VK_RIGHT:
			switch (nIdxCMItem ) 
			{
				case 0:	// brightness
					if(TaviSysParam.video.brightness < 60)
					{	
						TaviSysParam.video.brightness += 1;
					}
					break;
				
				case 1:	// contrast
					if(TaviSysParam.video.contrast < 60)
					{
						TaviSysParam.video.contrast += 1;
					}
					break;
					
				case 2:	// saturation
					if(TaviSysParam.video.saturation < 60)
					{
						TaviSysParam.video.saturation += 1;
					}
					break;
			}
			pMpegDecoder->Resume_ColorTemperature(
				TaviSysParam.video.brightness, TaviSysParam.video.contrast, TaviSysParam.video.saturation);
			break;
			
		case VK_F3:
			_SetupProperty.m_ExitState = TaskExit_Background;
			_SetupProperty.m_fExit = true;
			break;
			
		case TAVI_KEY_LMODE:
			_SetupProperty.m_ExitState = TaskExit_Switch;
			_SetupProperty.m_fExit = true;
			break;
			
		case VK_EXT_ESC:
			_SetupProperty.m_pAdjustBright = NULL;
			_SetupProperty.m_ExitState = TaskExit_ExitAndSwitch;
			_SetupProperty.m_fExit = true;
			break;			
						
		}
}

