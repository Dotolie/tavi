//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup User Effect Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupUserEffect.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/06/12 04:24:02 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: CSetupUserEffect.cpp,v $
// Revision 1.3  2006/06/12 04:24:02  ywkim
// Add delete instruction for m_pCMListView in ~CSetupUserEffectWnd()
//
// Revision 1.2  2006/06/07 07:57:36  zstein
// - fixed a bug.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/12/01 06:16:21  ywkim
// Add function of SetTemporaryPreset()
//
// Revision 1.3  2005/11/30 12:33:27  ywkim
// Change event processing routine of  VK_LEFT & VK_RIGTH in OnKeypressed()
//
// Revision 1.2  2005/11/25 09:08:33  ywkim
// Add setting function of SetUserEQ, SetUserEffect for sound effect
//
// Revision 1.1  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
//*****************************************************************************

#include "CSetupUserEffect.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include "../Lib/LibImage.h"
#include "setup.h"

extern TSETUP_CONFIG		_SetupProperty;
extern SysParameter_t		TaviSysParam;
extern CTextEngine_BDF	*pT;
extern RMint32			toggle_display;

CSetupUserEffectWnd::CSetupUserEffectWnd( CWindow* parent ) :
	CSetupWnd( parent )
{
	m_pCMListView = new CComplexListView( this );
	m_pCMListView->m_nBackgroundColor = 24;
	m_nBackgroundColor = 24;
	m_fAction = NULL;
	m_nTitleResID = ML_USER_EFFECT;
	m_nOkResID = ML_OK;
	m_nExitResID = ML_EXIT;
}

CSetupUserEffectWnd::~CSetupUserEffectWnd( void )
{
	delete m_pCMListView;
}

void CSetupUserEffectWnd::GetParameter( void )
{
	m_nUserEffect = TaviSysParam.sound.user_effect;
	m_nSurround[EFFECT_MEXUSER] = TaviSysParam.sound.surround_mex;
	m_nBass[EFFECT_MEXUSER] = TaviSysParam.sound.bass_mex;
	m_nSurround[EFFECT_REXWIDE] = TaviSysParam.sound.surround_rex_wide;
	m_nBass[EFFECT_REXWIDE] = TaviSysParam.sound.bass_rex_wide;
	m_nSurround[EFFECT_REXLIVE] = TaviSysParam.sound.surround_rex_live;
	m_nBass[EFFECT_REXLIVE] = TaviSysParam.sound.bass_rex_live;
}

void CSetupUserEffectWnd::SetTemporaryPreset( int Effect )
{
	switch( m_nUserEffect ) {
		case EFFECT_MEXUSER:
			pMpegDecoder->XEN_Preset( SOUND_MEX_USER );
			break;
		case EFFECT_REXWIDE:
			pMpegDecoder->XEN_Preset( SOUND_REX_WIDE_USER );
			break;
		case EFFECT_REXLIVE:
			pMpegDecoder->XEN_Preset( SOUND_REX_LIVE_USER );
			break;
		}

}

void CSetupUserEffectWnd::SetParameter( void )
{
	TaviSysParam.sound.user_effect = m_nUserEffect;
	TaviSysParam.sound.surround_mex = m_nSurround[EFFECT_MEXUSER];
	TaviSysParam.sound.bass_mex = m_nBass[EFFECT_MEXUSER];
	TaviSysParam.sound.surround_rex_wide = m_nSurround[EFFECT_REXWIDE];
	TaviSysParam.sound.bass_rex_wide = m_nBass[EFFECT_REXWIDE];
	TaviSysParam.sound.surround_rex_live = m_nSurround[EFFECT_REXLIVE];
	TaviSysParam.sound.bass_rex_live = m_nBass[EFFECT_REXLIVE];

	switch( TaviSysParam.sound.eq ) {
		case SOUND_MEX_USER:
		case SOUND_REX_WIDE_USER:
		case SOUND_REX_LIVE_USER:
			switch ( m_nUserEffect ) {
				case EFFECT_MEXUSER:	// 0
					TaviSysParam.sound.eq = SOUND_MEX_USER;
					break;
				case EFFECT_REXWIDE:	// 1
					TaviSysParam.sound.eq = SOUND_REX_WIDE_USER;
					break;
				case EFFECT_REXLIVE:		// 2
					TaviSysParam.sound.eq = SOUND_REX_LIVE_USER;
					break;
				}
			break;
		default:
			break;
		}
}

void CSetupUserEffectWnd::CreateSetup( void )
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


	// Effect Mode
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_EFFECT_MODE);
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
	pSpinBar->Add( ML_SOUND_PALETTE_MY_REAL_SOUND );
	pSpinBar->Add( ML_SOUND_PALETTE_MY_3D_WIDE );
	pSpinBar->Add( ML_SOUND_PALETTE_MY_3D_LIVE );
	pSpinBar->SetSelectIndex( m_nUserEffect );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSpinBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// surround
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_SURROUND );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(0, 6);
	pSlideBar->SetValue( m_nSurround[m_nUserEffect] );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

	// bass
	pLabel = new CLabel( NULL, COLOR_WHITE, COLOR_TRANSPARENT, ML_BASS );
	pLabel->m_nBackgroundColor = bgcolor;
	pLabel->m_nBorderColor = bgcolor;
	pLabel->Resize( CRect(0, 0, hw, h), false );
	pLabel->m_nFontWidth = 16;
	pLabel->m_nFontHeight = 15;
	pSlideBar = new CSlideBar( NULL );
	pSlideBar->m_nBackgroundColor = bgcolor;
	pSlideBar->m_nBorderColor = bgcolor;
	pSlideBar->Resize( CRect(0, 0, hw, h), false );
	pSlideBar->SetMinMax(0, 6);
	pSlideBar->SetValue( m_nBass[m_nUserEffect] );
	pCxLabel = new CComplexLabel( NULL );
	pCxLabel->m_nBackgroundColor = bgcolor;
	pCxLabel->m_nBorderColor = bgcolor;
	pCxLabel->Resize( CRect(0, 0, w, h), false );
	pCxLabel->SetLabel( pLabel );
	pCxLabel->AddComponent( pSlideBar );
	pItem = new CCMListItem( pCxLabel );
	m_pCMListView->AddItem( pItem );

}

void CSetupUserEffectWnd::UpdateParameter(void )
{
	CCMListItem* pItem;
	CWindow *pComponentWnd;

	pItem = ( CCMListItem* )m_pCMListView->GetItem(1);			// surround
	pComponentWnd = ((CComplexLabel*)(pItem->m_pComponent))->GetComponent();
	((CSlideBar*)pComponentWnd)->SetValue( m_nSurround[m_nUserEffect] );
	
	pItem = ( CCMListItem* )m_pCMListView->GetItem(2);			// bass
	pComponentWnd = ((CComplexLabel*)(pItem->m_pComponent))->GetComponent();
	((CSlideBar*)pComponentWnd)->SetValue( m_nBass[m_nUserEffect] );
}

void CSetupUserEffectWnd::SetUserEffect( ePARAM_TYPE eType, int eValue )
{
	CMD_t	CMD;
	CMD_t	*pCMD;

	CMD.nMode = SET_3DPARAMETER;
	CMD.Param.PARAMETER.TypeNo = eType;
	CMD.Param.PARAMETER.Value = (ePARAM_VALUE)eValue;
//	printf("SetUserEffect: eType=%d, eValue=%d\n", eType, eValue );
	pMpegDecoder->XEN_SetEffect( CMD );
}

void CSetupUserEffectWnd::OnKeypressed( const KeyEvent& evt )
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
	switch (nIdxCMItem ) {
		case 0:	// spinselectbar for selection of user effect
			nIdxComponent = ((CSpinSelectBar*)pComponentWnd)->GetSelectedIndex();
			break;
		case 1:	// slidebar for selection of surround
			nIdxComponent = ((CSlideBar*)pComponentWnd)->GetValue();
			break;
		case 2:	// slidebar for selection of bass
			nIdxComponent = ((CSlideBar*)pComponentWnd)->GetValue();
			break;
		}
	DEBUGMSG(1, ( "CSetupUserEffectWnd: nIdxCMItem=%d, nIdxComponent=%d\n", nIdxCMItem, nIdxComponent) );
	
	switch( evt.value ) {
		case VK_ENTER:
			SetParameter();
			MSG_Send( _SetupProperty.m_nTaskID, MSG_DRAW, 0, WINDOW_KILL_FOCUS );
			DEBUGMSG(1, ("CSetupUserEffectWnd: OK :TaskId=%d\n", _SetupProperty.m_nTaskID) );
			break;

		case VK_ESC:
			switch ( TaviSysParam.sound.eq ) {
				case SOUND_MEX_USER:
					SetUserEffect( XEN_PTYPE_SURR, TaviSysParam.sound.surround_mex );
					SetUserEffect( XEN_PTYPE_BASS, TaviSysParam.sound.bass_mex );
					break;
				case SOUND_REX_WIDE_USER:
					SetUserEffect( XEN_PTYPE_SURR, TaviSysParam.sound.surround_rex_wide);
					SetUserEffect( XEN_PTYPE_BASS, TaviSysParam.sound.bass_rex_wide);
					break;					
				case SOUND_REX_LIVE_USER:
					SetUserEffect( XEN_PTYPE_SURR, TaviSysParam.sound.surround_rex_live );
					SetUserEffect( XEN_PTYPE_BASS, TaviSysParam.sound.bass_rex_live );
					break;
				default:
					break;
				}
			
			MSG_Send( _SetupProperty.m_nTaskID, MSG_DRAW, 0, WINDOW_KILL_FOCUS );
			DEBUGMSG(1, ("CSetupUserEffectWnd: EXIT :TaskId=%d\n", _SetupProperty.m_nTaskID) );
			break;
			
		case VK_LEFT:
			switch ( nIdxCMItem ) {
				case 0:	// user effect
					m_nUserEffect --;
					if ( m_nUserEffect < EFFECT_MEXUSER )
						m_nUserEffect = EFFECT_REXLIVE;
					UpdateParameter();
					switch( TaviSysParam.sound.eq ) {
						case SOUND_MEX_USER:
						case SOUND_REX_WIDE_USER:
						case SOUND_REX_LIVE_USER:
							SetTemporaryPreset( m_nUserEffect );
							SetUserEffect( XEN_PTYPE_SURR, m_nSurround[m_nUserEffect] );
							SetUserEffect( XEN_PTYPE_BASS, m_nBass[m_nUserEffect] );
							break;
						default:
							break;
						}
					MSG_Send( _SetupProperty.m_nTaskID, MSG_DRAW, 0, 0 );	// redraw
					break;
				case 1:	// surround
					m_nSurround[m_nUserEffect]=nIdxComponent;
					switch ( TaviSysParam.sound.eq ) {
						case SOUND_MEX_USER:
						case SOUND_REX_WIDE_USER:
						case SOUND_REX_LIVE_USER:
							SetUserEffect( XEN_PTYPE_SURR, nIdxComponent );
							break;
						default:
							break;
						}
					break;

				case 2:	// bass
					m_nBass[m_nUserEffect]=nIdxComponent;	
					switch ( TaviSysParam.sound.eq ) {
						case SOUND_MEX_USER:
						case SOUND_REX_WIDE_USER:
						case SOUND_REX_LIVE_USER:
							SetUserEffect( XEN_PTYPE_BASS, nIdxComponent );
							break;
						default:
							break;
						}
					break;
				}
			break;
			
		case VK_RIGHT:
			switch ( nIdxCMItem ) {
				case 0:	// user effect
					m_nUserEffect ++;
					if ( m_nUserEffect >= EFFECT_MAXCOUNT )
						m_nUserEffect = EFFECT_MEXUSER;
					UpdateParameter();
					switch( TaviSysParam.sound.eq ) {
						case SOUND_MEX_USER:
						case SOUND_REX_WIDE_USER:
						case SOUND_REX_LIVE_USER:
							SetTemporaryPreset( m_nUserEffect );
							SetUserEffect( XEN_PTYPE_SURR, m_nSurround[m_nUserEffect] );
							SetUserEffect( XEN_PTYPE_BASS, m_nBass[m_nUserEffect] );
							break;
						default:
							break;
						}
					
					MSG_Send( _SetupProperty.m_nTaskID, MSG_DRAW, 0, 0 );	// redraw
					break;
				case 1:	// surround
					m_nSurround[m_nUserEffect]=nIdxComponent;
					switch ( TaviSysParam.sound.eq ) {
						case SOUND_MEX_USER:
						case SOUND_REX_WIDE_USER:
						case SOUND_REX_LIVE_USER:
							SetUserEffect( XEN_PTYPE_SURR, nIdxComponent );
							break;
						default:
							break;
						}
					break;

				case 2:	// bass
					m_nBass[m_nUserEffect]=nIdxComponent;	
					switch ( TaviSysParam.sound.eq ) {
						case SOUND_MEX_USER:
						case SOUND_REX_WIDE_USER:
						case SOUND_REX_LIVE_USER:
							SetUserEffect( XEN_PTYPE_BASS, nIdxComponent );
							break;
						default:
							break;
						}

					break;
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

