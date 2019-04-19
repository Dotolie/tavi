//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.5  2005/05/14 06:04:53  ywkim
// Enable mouse event
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

#include "CSetupSound.h"
#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/SlideBar.h>
#include "../Lib/LibImage.h"
#include "CSetupWnd.h"

void CSetupWnd::OnMouseEvent( const MouseEvent& evt )
{
	CEvent event;

	event.type = EVENT_MOUSE;
	event.u.mouse = evt;
	m_pCMListView->SendEvent( event );
}

void CSetupWnd::Resize( const CRect& rc, bool fUpdate )
{
	CWindow::Resize( rc, fUpdate );
	// Resize Complext ListView
	CRect rc2 = rc;
	rc2.x1 = 2;
	rc2.y1 = 30;
	rc2.x2 = rc.GetWidth()-1;
	rc2.y2 = rc.GetHeight()-30;
	m_pCMListView->Resize( rc2, fUpdate );
	m_pCMListView->m_nBackgroundColor = m_nBackgroundColor;
}
void CSetupWnd::ClearList( void )
{
	m_pCMListView->ClearList();
}
void CSetupWnd::SetActionProc( fDoActionProc fProc)
{
	m_fAction = fProc;
}
void CSetupWnd::DoActionProc( int nItem, int nComponent )
{
	m_fAction ( nItem,  nComponent );
}

CCMListItem* CSetupWnd::GetSelectedItem( void )
{
	CCMListItem* pItem = ( CCMListItem* )m_pCMListView->GetSelectedItem();

	return pItem;
}

