//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Simple Window class declaration

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/Window.cpp,v $

// $Revision: 1.4 $

// $Date: 2006/06/07 08:26:52 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: Window.cpp,v $
// Revision 1.4  2006/06/07 08:26:52  zstein
// - added SetDisable() method.
//
// Revision 1.3  2006/03/10 08:07:46  zstein
// - removed test codes.
//
// Revision 1.2  2006/03/09 03:05:16  zstein
// *** empty log message ***
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.23  2005/12/08 06:41:00  zstein
// *** empty log message ***
//
// Revision 1.22  2005/10/17 08:10:43  yong
// add GetVisible() method
//
// Revision 1.21  2005/10/17 01:25:34  zstein
// - updated UI.
//
// Revision 1.20  2005/09/14 02:30:46  zstein
// - update
//
// Revision 1.19  2005/08/25 12:50:57  zstein
// - updated ui.
//
// Revision 1.18  2005/08/25 08:20:13  zstein
// - changed SendMessage(...)
//
// Revision 1.17  2005/08/23 13:47:00  zstein
// - update & fixed bugs
//
// Revision 1.16  2005/08/03 03:23:20  zstein
// - updated.
//
// Revision 1.15  2005/07/20 06:09:43  zstein
// - updated.
//
// Revision 1.14  2005/07/16 04:13:52  zstein
// - fixed bugs & added functions.
//
// Revision 1.13  2005/04/05 08:02:36  zstein
// *** empty log message ***
//
// Revision 1.12  2005/03/08 06:31:23  zstein
// BUG FIXED
//
// Revision 1.11  2005/03/05 16:38:47  zstein
// BUG FIXED
//
// Revision 1.10  2005/03/04 13:16:30  zstein
// BUG FIXED:
//
// Revision 1.9  2005/03/03 03:17:33  ywkim
// *** empty log message ***
//
// Revision 1.8  2005/03/01 02:11:05  zstein
// *** empty log message ***
//
// Revision 1.7  2005/02/25 13:56:35  zstein
// m_nWindowType member variable.
// Added WindowType GetWindowType() const
//
// Revision 1.6  2005/02/25 06:11:15  zstein
// Fixed UI bug
//
// Revision 1.5  2005/02/25 01:13:09  zstein
// *** empty log message ***
//
// Revision 1.4  2005/02/22 17:49:52  zstein
// *** empty log message ***
//
// Revision 1.3  2005/02/21 04:07:49  zstein
// File utility
//
// Revision 1.2  2005/02/21 02:10:14  zstein
// BUG Fixed & Update
//
// Revision 1.1  2005/02/16 16:03:25  zstein
// *** empty log message ***
//

//******************************************************************************

#include <stdlib.h>
#include "window.h"
#include <Task/Msg.h>		// dependent 


int GenerateWindowID( void )
{
	static int wid = 0x1000;

	return wid++;
}

int g_CurrentTaskID;

CWindow::CWindow( CWindow* parent ) 
{
	Init( parent, CRect(0, 0, 10, 10) );
}

CWindow::CWindow( CWindow* parent, const CRect& rc, int borderw, int borderc, int background, int mask ) :
		m_nBorderWidth( borderw ),
		m_nBorderColor( borderc ),
		m_nBackgroundColor( background ),
		m_mask( mask )
{ 
	Init( parent, rc );
}

// g_WndList has all window.
CList< CWindow > g_WndList;
CWindow* g_pFocusedWnd;
CWindow* g_pRootWnd;

CWindow::CWindow() 
{
	Init( NULL, CRect(0, 0, 10, 10) );
}

CWindow::~CWindow( void ) 
{
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), 0 );
	m_uzChildList.FreeAll();
	DeregisterWndList();
}

void CWindow::RegisterWndList( void )
{
	if( m_pWndList ) {
		m_pWndList->AddHead( this );
	}
}

void CWindow::DeregisterWndList( void )
{
	if( m_pWndList ) {
		CNode< CWindow >* pNode = m_pWndList->Detach( this );
		if( pNode ) {
			pNode->m_Data = NULL;
			delete pNode;
		}
	}
}

void CWindow::Init( CWindow* parent, const CRect& rc ) 
{
	m_fVisible = true;
	m_disable = false;
	
	m_parent = parent;
	rcClient = rc;
	if( m_parent ) m_parent->AddChild( this );

	m_nWindowID = GenerateWindowID();
	m_nTaskID = g_CurrentTaskID;

	m_nWindowType = WINDOW_TYPE_NORMAL;
	m_pWndList = NULL;

	// default size
	Resize( rc, false );
}

void CWindow::SetCoordinate( int x, int y, int w, int h ) 
{
	if( m_parent ) {
		CPoint p1 = m_parent->GetABSLeftTop();
		CPoint p2 = m_parent->GetABSRightBottom();
		rcABS.x1 = p1.px + x;
		rcABS.y1 = p1.py + y;
		rcABS.x2 = p1.px + w;
		rcABS.y2 = p1.py + h;				
	}
	else {
		rcABS.x1 = x; rcABS.y1 = y;
		rcABS.x2 = x+w; rcABS.y2 + y+h;
	}
}

void CWindow::Resize( const CRect& rc, bool fUpdate ) 
{
	rcClient = rc;

	if( m_parent ) {
		rcABS.x1 = m_parent->rcABS.x1+rcClient.x1;
		rcABS.y1 = m_parent->rcABS.y1+rcClient.y1;
		rcABS.x2 = rcABS.x1 + rcClient.GetWidth()-1;
		rcABS.y2 = rcABS.y1 + rcClient.GetHeight()-1;
	}
	else {
		rcABS = rcClient;
	}
	if( fUpdate ) OnPaint( rcClient );

	int len = m_uzChildList.Count();
	CWindow* pChild;
	for( int i=0; i<len; i++ ) {
		pChild = m_uzChildList[i];
		m_uzChildList[i]->Resize( pChild->rcClient, fUpdate );
	}
}

void CWindow::Show( void ) 
{
	CWindow* pChild;

	if( m_fVisible == false ) return;
	OnPaint( rcClient );
	int len = m_uzChildList.Count();
	for( int i=0; i<len; i++ ) {
		pChild = m_uzChildList[i];
		pChild->Show();	
	}

	// raise
	CNode< CWindow >* pNode = g_WndList.Detach( this );
	if( pNode ) {
		g_WndList.AddNodeToHead( pNode );
	}

	op_Flush();
}

void CWindow::OnPaint( const CRect& rc )
{
	if( m_fVisible == false ) return;
	g_SysPixmap.Fill( rcABS.x1+1, rcABS.y1+1, rcABS.GetWidth()-2, rcABS.GetHeight()-2, m_nBackgroundColor);
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), 1, m_nBorderColor );
	g_SysPixmap.Fill( rcABS.x1, rcABS.y2, rcABS.GetWidth(), 1, m_nBorderColor );
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, 1, rcABS.GetHeight(), m_nBorderColor );
	g_SysPixmap.Fill( rcABS.x2, rcABS.y1, 1, rcABS.GetHeight(), m_nBorderColor );
}

void CWindow::OnTimerEvent( const TimerEvent& evt )
{
	CWindow* pChild;
	int len = m_uzChildList.Count();
	for( int i=0; i<len; i++ ) {
		pChild = m_uzChildList[i];
		pChild->OnTimerEvent( evt );
	}
}

void CWindow::SendEvent( const CEvent& evt, CWindow* wnd )
{
	if( wnd == NULL ) wnd = this;
	if( evt.type & EVENT_KEY ) {
		if( evt.u.key.pressed ) wnd->OnKeypressed( evt.u.key );
		else wnd->OnKeyReleased( evt.u.key );
	}
	if( evt.type & EVENT_MOUSE ) wnd->OnMouseEvent( evt.u.mouse );
	if( evt.type & EVENT_PAINT ) wnd->OnPaint( CRect(evt.u.paint.x1, evt.u.paint.y1, evt.u.paint.x2, evt.u.paint.y2) );
	if( evt.type & EVENT_WINDOW ) wnd->OnWindowEvent( evt.u.window );
	if( evt.type & EVENT_TIMER ) wnd->OnTimerEvent( evt.u.timer );
}

void CWindow::Hide( void )
{
	CWindow* pChild;
	int len = m_uzChildList.Count();
	if( m_fVisible ) g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), COLOR_TRANSPARENT );
	for( int i=0; i<len; i++ ) {
		pChild = m_uzChildList[i];
		pChild->Hide();
	}
	//op_Flush();
}

int CWindow::SetTimer( int msec, CTimer::TimerType type )
{
	CTimer* t = new CTimer;
	t->m_nTaskID = m_nTaskID;
	t->m_nWindowID = m_nWindowID;
	t->m_nType = type;
	t->m_nExpireTime = msec;
	t->m_nTimerID = RegisterTimer( t );
	return t->m_nTimerID;
}

void CWindow::KillTimer( int nID )
{
	DeregisterTimer( nID );
}


bool CWindow::GetVisible( void )
{

	return this->m_fVisible;
}

void CWindow::SetVisible( bool fVisible )
{
	CWindow* pChild;
	
	m_fVisible = fVisible;
	//int len = m_uzChildList.Count();
	//for( int i=0; i<len; i++ ) {
	//	m_uzChildList[i]->SetVisible( fVisible );
	//}
}

void CWindow::SetFocus( void )
{
	if( m_pWndList ) {
		CNode< CWindow >* pNode;
		pNode = m_pWndList->Detach( this );
		m_pWndList->AddNodeToHead( pNode );
	}

	m_fVisible = true; 
	g_pFocusedWnd = this;
	Show();
}

void CWindow::SendMessage( int id, int to )
{
	TMsgEntry msg;
	if( to == -1 ) to = m_nWindowID;
	msg.ssToTaskId = m_nTaskID;
	msg.ulMsgId = MSG_WINDOW;
	msg.usParam1 = to;
	msg.usParam2 = id;
	MSG_Send( m_nTaskID, &msg );
}

void CWindow::SetDisable( bool disable )
{
  m_disable = disable;
}
