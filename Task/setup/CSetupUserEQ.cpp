//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup User EQ Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupUserEQ.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupUserEQ.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/11/30 12:14:41  ywkim
// Add SetUserEQ() in SetParameter for applying & saving
//
// Revision 1.2  2005/11/25 09:08:33  ywkim
// Add setting function of SetUserEQ, SetUserEffect for sound effect
//
// Revision 1.1  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
//*****************************************************************************

#include "CSetupUserEQ.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include "../Lib/LibImage.h"
#include "setup.h"

extern TSETUP_CONFIG		_SetupProperty;
extern SysParameter_t		TaviSysParam;
extern CTextEngine_BDF	* pT;
extern RMint32			toggle_display;

CSetupUserEQWnd::CSetupUserEQWnd( CWindow* parent ) :
	CSetupWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_fAction = NULL;
	m_nTitleResID = ML_USER_EQ;
	m_nOkResID = ML_OK;
	m_nExitResID = ML_EXIT;
}

CSetupUserEQWnd::~CSetupUserEQWnd( void )
{
}

void CSetupUserEQWnd::GetParameter( void )
{
	m_nUserEQ[USEREQ_BAND0] = TaviSysParam.sound.band0;
	m_nUserEQ[USEREQ_BAND1] = TaviSysParam.sound.band1;
	m_nUserEQ[USEREQ_BAND2] = TaviSysParam.sound.band2;
	m_nUserEQ[USEREQ_BAND3] = TaviSysParam.sound.band3;
	m_nUserEQ[USEREQ_BAND4] = TaviSysParam.sound.band4;
}

void CSetupUserEQWnd::SetParameter( void )
{
	TaviSysParam.sound.band0 = m_nUserEQ[USEREQ_BAND0];
	TaviSysParam.sound.band1 = m_nUserEQ[USEREQ_BAND1];
	TaviSysParam.sound.band2 = m_nUserEQ[USEREQ_BAND2];
	TaviSysParam.sound.band3 = m_nUserEQ[USEREQ_BAND3];
	TaviSysParam.sound.band4 = m_nUserEQ[USEREQ_BAND4];	

	SetUserEQ( EQ_PTYPE_BAND_0, TaviSysParam.sound.band0 );
	SetUserEQ( EQ_PTYPE_BAND_1, TaviSysParam.sound.band1 );
	SetUserEQ( EQ_PTYPE_BAND_2, TaviSysParam.sound.band2 );
	SetUserEQ( EQ_PTYPE_BAND_3, TaviSysParam.sound.band3 );
	SetUserEQ( EQ_PTYPE_BAND_4, TaviSysParam.sound.band4 );				

}

void CSetupUserEQWnd::CreateSetup( void )
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


	GetParameter();
//	printf( "CreateSetupDisplay: hw=%d, h=%d\n", hw, h );


	// 125Hz
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SOUND_PALETTE_USER_EQ_B125 );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(-9, 9);
	pSlideBar->SetValue( m_nUserEQ[USEREQ_BAND0]-EQ_VALUE_0DB );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// 250Hz
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SOUND_PALETTE_USER_EQ_B250 );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(-9, 9);
	pSlideBar->SetValue( m_nUserEQ[USEREQ_BAND1]-EQ_VALUE_0DB );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// 1KHz
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SOUND_PALETTE_USER_EQ_B1K );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(-9, 9);
	pSlideBar->SetValue( m_nUserEQ[USEREQ_BAND2]-EQ_VALUE_0DB );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// 4KHz
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SOUND_PALETTE_USER_EQ_B4K );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(-9, 9);
	pSlideBar->SetValue( m_nUserEQ[USEREQ_BAND3]-EQ_VALUE_0DB );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// 8KHz
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SOUND_PALETTE_USER_EQ_B8K );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(-9, 9);
	pSlideBar->SetValue( m_nUserEQ[USEREQ_BAND4]-EQ_VALUE_0DB );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

}

void CSetupUserEQWnd::SetUserEQ( ePARAM_TYPE eType, int eValue )
{
	CMD_t	CMD;
	CMD_t	*pCMD;

	CMD.nMode = SET_EQPARAMETER;
	CMD.Param.PARAMETER.TypeNo = eType;
	CMD.Param.PARAMETER.Value = (ePARAM_VALUE)eValue;
//	printf("SetUserEffect: eType=%d, eValue=%d\n", eType, eValue );
	pMpegDecoder->XEN_SetEffect( CMD );
}

void CSetupUserEQWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;
	CCMListItem* pItem;
	CWindow *pComponentWnd;
	int	nIdxCMItem;
	int 	nIdxComponent;
	
	event.type = EVENT_KEY;
	event.u.key = evt;
	m_pCMListView->SendEvent( event );

	pItem = ( CCMListItem* )m_pCMListView->GetSelectedItem();
	pComponentWnd = ((CComplexLabel*)(pItem->m_pComponent))->GetComponent();
	nIdxCMItem = pItem->m_nIdx;
	nIdxComponent = ((CSlideBar*)pComponentWnd)->GetValue();

	DEBUGMSG(0, ( "CSetupUserEffectWnd: nIdxCMItem=%d, nIdxComponent=%d\n", nIdxCMItem, nIdxComponent) );
	
	switch( evt.value ) {
		case VK_ENTER:
			SetParameter();
			MSG_Send( _SetupProperty.m_nTaskID, MSG_DRAW, 0, WINDOW_KILL_FOCUS );
			DEBUGMSG( 0, ("CSetupUserEQWnd: OK :TaskId=%d\n", _SetupProperty.m_nTaskID) );
			break;

		case VK_ESC:
			if ( TaviSysParam.sound.eq == SOUND_SEQ_USER ) {
				SetUserEQ( EQ_PTYPE_BAND_0, TaviSysParam.sound.band0 );
				SetUserEQ( EQ_PTYPE_BAND_1, TaviSysParam.sound.band1 );
				SetUserEQ( EQ_PTYPE_BAND_2, TaviSysParam.sound.band2 );
				SetUserEQ( EQ_PTYPE_BAND_3, TaviSysParam.sound.band3 );
				SetUserEQ( EQ_PTYPE_BAND_4, TaviSysParam.sound.band4 );				
				}
			MSG_Send( _SetupProperty.m_nTaskID, MSG_DRAW, 0, WINDOW_KILL_FOCUS );
			DEBUGMSG(0, ("CSetupUserEQWnd: EXIT :TaskId=%d\n", _SetupProperty.m_nTaskID) );
			break;
			
		case VK_LEFT:
		case VK_RIGHT:
			m_nUserEQ[nIdxCMItem] = nIdxComponent+EQ_VALUE_0DB;
			DEBUGMSG(1, ("m_nUserEQ[%d]= %d\n",  nIdxCMItem, nIdxComponent) );
//			printf ("m_nUserEQ[%d]= %d, TaviSysParam.sound.eq=%d\n",  nIdxCMItem, nIdxComponent+EQ_VALUE_0DB, TaviSysParam.sound.eq) ;			
			if ( TaviSysParam.sound.eq == SOUND_SEQ_USER ) {
				SetUserEQ( (ePARAM_TYPE)(nIdxCMItem+EQ_PTYPE_BAND_0), nIdxComponent+EQ_VALUE_0DB );
				}			
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
			_SetupProperty.m_ExitState = TaskExit_ExitAndSwitch;
			_SetupProperty.m_fExit = true;
			break;			
						
		}
}

