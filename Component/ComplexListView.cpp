//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Complex ListView Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ComplexListView.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ComplexListView.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.8  2005/11/24 03:09:54  zstein
// *** empty log message ***
//
// Revision 1.7  2005/10/17 05:22:17  zstein
// - deleted AddItem(...)
//
// Revision 1.6  2005/05/18 20:52:14  ywkim
// fix bitmap pixel color
//
// Revision 1.5  2005/04/27 08:56:36  zstein
// support auto center align.
//
// Revision 1.4  2005/03/01 07:01:36  zstein
// *** empty log message ***
//
// Revision 1.3  2005/03/01 02:11:05  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/25 06:11:15  zstein
// Fixed UI bug
//
// Revision 1.1  2005/02/25 01:12:18  zstein
// Complex Label & View
//
//
//*******************************************************************************

#include "ComplexListView.h"

CComplexListView::CComplexListView( CWindow* parent ) :
	CListView( parent )
{
	m_nBarHeight = 22;

	m_pScrollBar->SetBorderColor( 24 );
	m_pScrollBar->m_nBackgroundColor = 24;
}

CComplexListView::~CComplexListView( void )
{
}

void CComplexListView::OnKeypressed( const KeyEvent& evt )
{
	CListView::OnKeypressed( evt );

	CEvent event;
	event.type = EVENT_KEY;
	event.u.key = evt;
	CCMListItem* pItem = ( CCMListItem* )m_uzList[m_nStartIdx+m_nSelectedIdx];
	pItem->m_pComponent->SendEvent( event );
}

void CComplexListView::DrawItem( CListItem* pItem, int idx, bool fSelected )
{
	CCMListItem* pCMItem = ( CCMListItem* )pItem;
	int h = m_nBarHeight*idx;
	int offset;
	CRect rc = pCMItem->m_pComponent->rcClient;
	int nBarXOffset = 6;

	offset = (rcABS.GetWidth() - m_puzBarPixmap->m_nWidth)/2;
	if( m_uzList.Count() > m_nListCount ) {
		offset -= m_pScrollBar->GetClientRect().GetWidth();
	}
	CRect tmp = rc;
	rc.x1 += rcABS.x1+offset;
	rc.y1 += rcABS.y1+h;
	rc.x2 += rcABS.x1+offset;
	rc.y2 += rcABS.y2+h;

	// SetFocus(...) calls OnPaint().
	if( fSelected ) {
		op_BitBlt( &g_SysPixmap, rcABS.x1+offset, rcABS.y1+idx*m_puzBarPixmap->m_nHeight,
			m_puzBarPixmap, 0, 0, m_puzBarPixmap->m_nWidth, m_puzBarPixmap->m_nHeight );
	}
	else {
		g_SysPixmap.Fill( rcABS.x1+offset, rcABS.y1+idx*m_puzBarPixmap->m_nHeight,
			m_puzBarPixmap->m_nWidth, m_puzBarPixmap->m_nHeight, m_nBackgroundColor );
	}
	
	pCMItem->m_pComponent->Resize( rc, false );
	pCMItem->m_pComponent->rcClient = tmp;
	pCMItem->m_pComponent->SetFocus( fSelected );
}

void CComplexListView::Resize( const CRect& rc, bool fUpdate )
{
	CListView::Resize( rc, fUpdate );

	m_nListCount = rcClient.GetHeight() / m_nBarHeight;
	m_pScrollBar->SetViewCount( m_nListCount );

	int w = rcClient.GetWidth() - m_pScrollBar->rcClient.GetWidth() - 20;
	int h = m_nBarHeight;
	if( w <= 0 || h<=0 ) return;
	// Draw Selectbar
	if( m_puzBarPixmap ) delete m_puzBarPixmap;
	m_puzBarPixmap = new CPixmap;
	m_puzBarPixmap->CreatePixmap( w, h, 8, NULL, false );
	m_puzBarPixmap->Fill( 0, 0, m_puzBarPixmap->m_nWidth/2, m_puzBarPixmap->m_nHeight, COLOR_WHITE );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth/2, 0, m_puzBarPixmap->m_nWidth/2, m_puzBarPixmap->m_nHeight, 28 );
	// left round edge( white )
	m_puzBarPixmap->Fill( 0, 0, 1, 1, 24 );
	m_puzBarPixmap->Fill( 1, 0, 1, 1, 19 );
	m_puzBarPixmap->Fill( 2, 0, 1, 1, 18 );
	m_puzBarPixmap->Fill( 0, 1, 1, 1, 19 );
	m_puzBarPixmap->Fill( 0, 2, 1, 1, 18 );
	m_puzBarPixmap->Fill( 0, m_puzBarPixmap->m_nHeight-4, 1, 1, 18 );
	m_puzBarPixmap->Fill( 0, m_puzBarPixmap->m_nHeight-3, 1, 1, 19 );
	m_puzBarPixmap->Fill( 0, m_puzBarPixmap->m_nHeight-2, 1, 2, 24 );
	m_puzBarPixmap->Fill( 1, m_puzBarPixmap->m_nHeight-3, 1, 1, 18 );
	m_puzBarPixmap->Fill( 1, m_puzBarPixmap->m_nHeight-2, 1, 1, 19 );
	m_puzBarPixmap->Fill( 1, m_puzBarPixmap->m_nHeight-1, 1, 1, 32 );
	// right round edge( ?? )
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-3, 0, 1, 1, 21 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-2, 0, 1, 1, 32 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-1, 0, 1, 1, 24 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-2, 1, 1, 1, 21 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-1, 1, 1, 1, 32 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-1, 2, 1, m_nBarHeight-4, 31 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-1, m_puzBarPixmap->m_nHeight-1, 1, 1, 24 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-2, m_puzBarPixmap->m_nHeight-3, 1, 1, 21 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-2, m_puzBarPixmap->m_nHeight-2, 2, 1, 32 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-2, m_puzBarPixmap->m_nHeight-1, 1, 1, 32 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-3, m_puzBarPixmap->m_nHeight-2, 1, 1, 21 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth-3, m_puzBarPixmap->m_nHeight-2, 1, 1, 21 );
	// line
	m_puzBarPixmap->Fill( 2, m_puzBarPixmap->m_nHeight-2, m_puzBarPixmap->m_nWidth/2-2, 1, 18 );
	m_puzBarPixmap->Fill( 2, m_puzBarPixmap->m_nHeight-1, m_puzBarPixmap->m_nWidth/2-2, 1, 31 );
	m_puzBarPixmap->Fill( m_puzBarPixmap->m_nWidth/2-1, m_puzBarPixmap->m_nHeight-1, m_puzBarPixmap->m_nWidth/2, 1, 31);

	// repositioning scrollbar
	CRect& rc2 = m_pScrollBar->rcClient;
	rc2.x1 -= 4;
	rc2.x2 -= 4;
	m_pScrollBar->Resize( rc2, false );
}
