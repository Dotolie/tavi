//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Complex Label Class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/ComplexLabel.cpp,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: ComplexLabel.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/10/17 01:25:34  zstein
// - updated UI.
//
// Revision 1.2  2005/03/01 02:11:05  zstein
// *** empty log message ***
//
// Revision 1.1  2005/02/25 01:12:18  zstein
// Complex Label & View
//
//

//******************************************************************************

#include "ComplexLabel.h"

CComplexLabel::CComplexLabel( CWindow* parent ) :
	CWindow( parent )
{
	m_plbTitle = NULL;
	m_pChild = NULL;
}

CComplexLabel::~CComplexLabel( void )
{
	// NOTE: DO NOT DELETE m_plbTitle and m_pChild!
	// CWindow::~CWindow will automatically deletes her childern.
}

void CComplexLabel::OnFocus( bool fFocus )
{
	//CWindow::OnFocus( fFocus );
	if( m_plbTitle ) m_plbTitle->SetFocus( fFocus );
	if( m_pChild ) m_pChild->SetFocus( fFocus );
}

void CComplexLabel::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;

	event.type = EVENT_KEY;
	event.u.key = evt;
	if( m_pChild ) m_pChild->SendEvent( event );
}

void CComplexLabel::SetLabel( CLabel* pLabel )
{
	m_plbTitle = pLabel;
	if( m_plbTitle == NULL ) return;
	if( m_plbTitle->m_parent != this ) {
		int offset;
		CRect rc = m_plbTitle->rcClient;
		AddChild( m_plbTitle );
		offset = rcClient.GetWidth()/4 - rc.GetWidth()/2;
		rc.x1 += offset;
		rc.x2 += offset;
		m_plbTitle->Resize( rc, false );
	}
}

void CComplexLabel::AddComponent( CWindow* pComponent )
{
	m_pChild = pComponent;
	if( m_pChild == NULL ) return;
	if( m_pChild->m_parent != this ) {
		int half = rcClient.GetWidth()/2;
		int offset;
		CRect rc = m_pChild->rcClient;
		offset = rcClient.GetWidth()/4 - rc.GetWidth()/2;
		rc.x1 += half-1+offset+2;
		rc.x2 += half-1+offset+2;
		AddChild( m_pChild );
		m_pChild->Resize( rc, false );
	}
}
