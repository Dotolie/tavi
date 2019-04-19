//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Progress dialogue for showing the status of db update or etc.
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ProgressDlg.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ProgressDlg.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/12/26 04:44:25  zstein
// *** empty log message ***
//
// Revision 1.2  2005/09/14 02:30:46  zstein
// - update
//
// Revision 1.1  2005/05/15 11:10:25  zstein
// - [START] Progress dialogue
//
//*******************************************************************************

#include <include/tavi_palette.h>
#include "ProgressDlg.h"

CProgressDlg::CProgressDlg( CWindow* parent, const char* szTitle ) :
	CModalWnd( parent )
{
	m_szTitle = szTitle;
	m_plbText = new CLabel( this, _WHITE, 24, "" );
	m_plbText->Resize( CRect(14, 40, 280, 53), false );
	m_plbText->m_nFontHeight = 12;

	m_plbCount = new CLabel( this, _WHITE, 24, "" );
	m_plbCount->Resize( CRect(14, 60, 200, 73), false );
	m_plbCount->m_nFontHeight = 12;
}

CProgressDlg::~CProgressDlg( void )
{
}

void CProgressDlg::Update( CString& sz, int count )
{
	m_plbText->SetText( sz );
	char buf[10];
	if( count == -1 ) sprintf( buf, "" );
	else sprintf( buf, "Count: %d", count );
	m_plbCount->SetText( buf );
	Flush();
}
