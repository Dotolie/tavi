//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Context Menu
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ContextMenu.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ContextMenu.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/12/08 06:40:22  zstein
// *** empty log message ***
//
// Revision 1.10  2005/12/01 01:03:24  zstein
// *** empty log message ***
//
// Revision 1.9  2005/11/25 10:24:50  zstein
// *** empty log message ***
//
// Revision 1.8  2005/11/22 09:09:57  zstein
// *** empty log message ***
//
// Revision 1.7  2005/11/17 11:18:41  zstein
// *** empty log message ***
//
// Revision 1.6  2005/11/10 01:12:27  zstein
// *** empty log message ***
//
// Revision 1.5  2005/09/16 05:37:46  zstein
// - fixed ui bugs
//
// Revision 1.4  2005/08/25 03:40:08  zstein
// - bug fixed
//
// Revision 1.3  2005/08/23 13:47:00  zstein
// - update & fixed bugs
//
// Revision 1.2  2005/08/03 03:23:20  zstein
// - updated.
//
// Revision 1.1  2005/07/16 04:43:30  zstein
// - created.
//
//******************************************************************************

#include "ContextMenu.h"

CContextMenu::CContextMenu( CWindow* parent ) :
	CWindow( parent )
{
	m_pCtxMenu = NULL;
}

CContextMenu::~CContextMenu( void )
{
	if( m_pCtxMenu ) delete m_pCtxMenu;
}

void CContextMenu::OnKeypressed( const KeyEvent& evt )
{
	switch( evt.value ) {
	case VK_UP:
		m_pCtxMenu->CursorUp();
		break;
	case VK_DOWN:
		m_pCtxMenu->CursorDown();
		break;
	case VK_ESC:
	case VK_F2:
	case VK_LEFT:
		m_pCtxMenu->CursorLeft();
		break;
	case VK_RIGHT:
	case VK_ENTER:
		m_pCtxMenu->CursorRight();
		break;
	//case VK_ESC:
	//	Hide();
		break;
	}

	if( m_pCtxMenu->IsVisible() == false ) {
		m_fVisible = false;
		if( m_pWndList ) {
			m_pWndList->Head()->SetFocus();
		}
		else {
			m_parent->SetFocus();
		}
		//if( g_pFocusedWnd == this ) {
		//	g_pFocusedWnd = m_parent;
		//}
	}
}

void CContextMenu::OnMouseEvent( const MouseEvent& evt )
{
	KeyEvent kevt;

	//printf( "CListView::OnMouseEvent()\n" );
	if( evt.click ) {
		kevt.value = VK_ENTER;
	}
	else {
		if( evt.x > 0 ) kevt.value = VK_DOWN;
		else if( evt.x < 0  ) kevt.value = VK_UP;
	}

	OnKeypressed( kevt );
}

void CContextMenu::OnWindowEvent( const WindowEvent& evt )
{
	CEvent event;
	event.type = EVENT_WINDOW;
	event.u.window = evt;
	SendEvent( event, m_parent );
}

void CContextMenu::Show( void )
{
	if( m_fVisible == false ) return;
	
	if( m_pCtxMenu ) m_pCtxMenu->Show();
}

void CContextMenu::Hide( void )
{
	m_fVisible = false;
	if( m_pCtxMenu ) m_pCtxMenu->Hide( 0, false );
}

void CContextMenu::Reset( void )
{
	if( m_pCtxMenu ) delete m_pCtxMenu;
	m_pCtxMenu = new CPopUpMenu( 0, 30, 310, 120 );
	//m_pCtxMenu = new CPopUpMenu( 0, 0, 0, 0 );
}
