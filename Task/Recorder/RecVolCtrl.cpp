//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Volume ProgressBar class definition
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecVolCtrl.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: RecVolCtrl.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.9  2006/01/06 11:36:57  zstein
// *** empty log message ***
//
// Revision 1.8  2005/12/26 04:37:02  zstein
// *** empty log message ***
//
// Revision 1.7  2005/11/28 00:50:12  zstein
// *** empty log message ***
//
// Revision 1.6  2005/11/22 09:07:10  zstein
// *** empty log message ***
//
//******************************************************************************

#include <Task/tavi_global.h>
#include <Lib/LibImage.h>
#include <meplib.h>
#include "RecVolCtrl.h"
#include "RecordBottomWnd.h"
#include "RecordWnd.h"

// Macros
#define SCROLL_OFFSET		20
#define OFFSET 				18  // VOLUME PROGRESS BAR Y-OFFSET
#define TIMEOUT				5000 // 5 sec. 

// Extern Variables
extern CSndCtrl* g_SndCtrl;

// Local Variables

CRecVolCtrl::CRecVolCtrl( CWindow* parent ) :
	CBottomWnd( parent, CBottomWnd::BW_NORMAL )
{
	m_nVolRange = g_SndCtrl->GetLineinVolumeRange();
	m_nVolume = TaviSysParam.record.input_vol;
	g_SndCtrl->SetLineInVolume( m_nVolume );

	m_pProgressBar = new CProgressBar( this );
	m_pProgressBar->ShowBall( true );
	m_pProgressBar->Resize( CRect(32, 19, 287, 24), false );
	m_pProgressBar->SetRange( 0, 40 );
	int pos = 40*m_nVolume/m_nVolRange;
	m_pProgressBar->SetPos( pos );

	m_plbVolume = new CLabel( this, _WHITE, 24, ML_VOLUME );
	m_plbVolume->Resize( CRect(90, 0, 162, 14), false );
	m_plbVolume->m_nFontHeight = 12;
	m_plbVolume->SetJustify( CLabel::CLABEL_RIGHT_JUSTIFY );

	char buf[10];
	sprintf( buf, "%d", pos );
	m_plbLevel = new CLabel( this, _GREEN, 24, buf );
	m_plbLevel->Resize( CRect(168, 0, 230, 14), false );
	m_plbLevel->m_nFontHeight = 12;
	m_plbLevel->SetJustify( CLabel::CLABEL_LEFT_JUSTIFY );
}

CRecVolCtrl::~CRecVolCtrl( void )
{
}

void CRecVolCtrl::OnMouseEvent( const MouseEvent& evt )
{
	CRecorderWnd* pRec = ( CRecorderWnd* )m_parent;
	KeyEvent kevt;

	if( !evt.click ) return;

	if( evt.click && (m_fVisible == true) ) {
		m_fVisible = false;
		g_pFocusedWnd = m_parent;
		pRec->m_pBottomWnd->Show();
		return;
	}
	
	if( m_fVisible == false ) SetFocus();
	
	gettimeofday( &m_tv, NULL );
	if( evt.x > 0 ) kevt.value = VK_UP;
	else if( evt.x < 0 ) kevt.value = VK_DOWN;
	else return;

	OnKeypressed( kevt );
}

void CRecVolCtrl::OnKeypressed( const KeyEvent& evt )
{
	int vol;
	int pos = m_pProgressBar->GetPos();
	TMsgEntry msg;
	CRecorderWnd* pRec = ( CRecorderWnd* )m_parent;

	gettimeofday( &m_tv, NULL );
	switch( evt.value ) {
	case VK_UP:
		if( pos >= m_pProgressBar->GetRange() ) return;
		pos++;
		break;
	case VK_DOWN:
		if( pos == 0 ) return;
		pos--;
		break;
	case VK_ESC:
		m_fVisible = false;
		g_pFocusedWnd = m_parent;
		pRec->m_pBottomWnd->Show();
		return;
	case VK_ENTER:
		// kill focus
		m_fVisible = false;
		g_pFocusedWnd = m_parent;
		//pRec->m_pBottomWnd->Show();
		// recording
		msg.ssToTaskId = m_nTaskID;
		msg.ulMsgId = MSG_BUTTON_UP;
		msg.usParam1 = TAVI_KEY_ENTER;
		msg.usParam2 = 1;
		MSG_Send( m_nTaskID, &msg );
		break;
	}

	vol = m_nVolRange*pos/40;
	if( vol > m_nVolRange-1 ) vol = m_nVolRange;
	
	m_pProgressBar->SetPos( pos );
	m_pProgressBar->Update();
	char buf[10];
	sprintf( buf, "%d", pos );
	m_plbLevel->SetText( buf );

	m_nVolume = vol;
	TaviSysParam.sound.input_vol = m_nVolume;
	g_SndCtrl->SetLineInVolume( m_nVolume );
	Flush();
}

void CRecVolCtrl::OnWindowEvent( const WindowEvent& evt )
{
	CRecorderWnd* pRec = ( CRecorderWnd* )m_parent;
	struct timeval tv;

	switch( evt.nMessage ) {
	case WINDOW_IDLE:
		gettimeofday( &tv, NULL );
		if( abs(tv.tv_sec-m_tv.tv_sec) > 5 ) {
			m_fVisible = false;
			g_pFocusedWnd = m_parent;
			pRec->m_pBottomWnd->Show();
			
		}
		break;
	case WINDOW_NOTIFY:
		SendMessage( WINDOW_KILL, m_parent->GetWindowID() );
		break;
	}
}

void CRecVolCtrl::Show( void )
{
	CWindow::Show();
	gettimeofday( &m_tv, NULL );
	mep_eat_event();
}
