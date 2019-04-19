//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Spin Selectbar Class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/SpinSelectBar.cpp,v $

// $Revision: 1.2 $

// $Date: 2006/06/07 08:26:20 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: SpinSelectBar.cpp,v $
// Revision 1.2  2006/06/07 08:26:20  zstein
// - added a disable to select function.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.13  2005/11/22 09:11:42  ywkim
// Change OnPaint() for icon of left arrow
//
// Revision 1.12  2005/11/16 08:48:47  etnlwind
// Update for AlarmSet
//
// Revision 1.11  2005/10/17 01:25:34  zstein
// - updated UI.
//
// Revision 1.10  2005/05/18 16:02:16  ywkim
// add function of GetCount()
//
// Revision 1.9  2005/05/18 11:29:44  zstein
// - supported rotate.
//
// Revision 1.8  2005/05/17 19:52:50  zstein
// - added some functions.
//
// Revision 1.7  2005/05/17 19:41:38  zstein
// *** empty log message ***
//
// Revision 1.6  2005/05/15 11:08:49  zstein
// *** empty log message ***
//
// Revision 1.5  2005/04/27 04:53:21  zstein
// Support Multi-languages.
// Support resource text.
//
// Revision 1.4  2005/03/01 02:11:05  zstein
// *** empty log message ***
//
// Revision 1.3  2005/02/25 13:57:17  zstein
// Added WindowType
//
// Revision 1.2  2005/02/25 06:11:15  zstein
// Fixed UI bug
//
// Revision 1.1  2005/02/25 01:13:09  zstein
// *** empty log message ***
//

//*******************************************************************************

#include "SpinSelectBar.h"
#include "CharSet.h"
#include "fbdev.h"
#include "../Lib/LibImage.h"
#include <Task/tavi_global.h>

// Macros
#define COLOR_FOCUSED			27
#define BACKGROUND_COLOR		22
#define FORGROUND_COLOR		23
#define B						28
#define F						23

// Extern variables
extern CTextEngine_BDF* pT;


// Local variables
static char szLeftArrow[7][5] = {
	{ B, B, B, F, F },
	{ B, B, F, F, F },
	{ B, F, F, F, B },
	{ F, F, F, B, B },
	{ B, F, F, F, B },
	{ B, B, F, F, F },
	{ B, B, B, F, F }
};

static char szRightArrow[7][5] = {
	{ F, F, B, B, B },
	{ F, F, F, B, B },
	{ B, F, F, F, B },
	{ B, B, F, F, F },
	{ B, F, F, F, B },
	{ F, F, F, B, B },
	{ F, F, B, B, B }
};
	
CSpinSelectBar::CSpinSelectBar( CWindow* parent, bool fResID, int nCount  ) :
	m_fResID( fResID ),
	m_nCount( nCount ),
	m_pResIDs( NULL ),
	m_nItemCount( 0 )
{
	int i=0;

//	m_pfn = pfn;
	m_pfnarg = 0;
	
	m_nSelectedIdx = 0;
	m_parent = parent;
	m_puzLArrow = new CPixmap;
	m_puzLArrow->CreatePixmap( 5, 7, 8, (char*)szLeftArrow, false );
	m_puzRArrow = new CPixmap;
	m_puzRArrow->CreatePixmap( 5, 7, 8, (char*)szRightArrow, false );

	m_nWindowType = WINDOW_TYPE_SPINBAR;
	m_nFocusedColor = 28;

	if( m_fResID ) {
		m_pResIDs = new int[m_nCount];
	}

	m_TxtBuf = NULL;
	
}

CSpinSelectBar::~CSpinSelectBar( void )
{
	if( m_puzLArrow ) delete m_puzLArrow;
	if( m_puzRArrow ) delete m_puzRArrow;	
	if( m_pResIDs ) delete[] m_pResIDs;
	if(m_TxtBuf) free(m_TxtBuf);	
}

void CSpinSelectBar::OnFocus( bool fFocus )
{
	m_fFocus = fFocus;
	OnPaint( rcClient );
}

void CSpinSelectBar::Resize( const CRect& rc, bool fUpdate )
{
	if( rc.GetWidth() < rcClient.GetWidth() ||
	     rc.GetHeight() < rcClient.GetHeight() ) return;
	CWindow::Resize( rc, fUpdate );
}

void CSpinSelectBar::OnPaint( const CRect& rc )
{
	CString* text; 
	int hoffset;
	int color;

	hoffset = rcABS.GetHeight()/2-m_puzLArrow->m_nHeight/2;
	if( m_fFocus ) {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nFocusedColor );

		//if( m_uzList.Count() ||m_nItemCount ) {
		if( m_uzList.Count()>1 || m_nItemCount ) {
			op_BitBlt( &g_SysPixmap, rcABS.x1, rcABS.y1+hoffset, 
				m_puzLArrow, 0, 0, m_puzLArrow->m_nWidth, m_puzLArrow->m_nHeight );
			op_BitBlt( &g_SysPixmap, rcABS.x2-m_puzRArrow->m_nWidth, rcABS.y1+hoffset, 
				m_puzRArrow, 0, 0, m_puzRArrow->m_nWidth, m_puzRArrow->m_nHeight );
		}
		else {
			op_BitBlt( &g_SysPixmap, rcABS.x2-m_puzRArrow->m_nWidth, rcABS.y1+hoffset, 
				m_puzRArrow, 0, 0, m_puzRArrow->m_nWidth, m_puzRArrow->m_nHeight );
		}
		color = _BLACK;
	}
	else {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nBackgroundColor );
		color = _WHITE;
		if( m_disable ) color = _GRAY;
	}

	if( m_uzList.Count() == 0 && m_nItemCount == 0 ) return;
	
	text = m_uzList[m_nSelectedIdx];
	hoffset = (rcABS.GetHeight()-12)/2;
	int length, offset;
	if( m_fResID == 0 ) {
		uchar_t wsz[256];
		uchar_t* pwsz;
		if( text ) {
			if( text->IsUnicode() ) {
				pwsz = ( uchar_t* )text->GetString();
			}
			else {
				ConvertUnicode( text->GetString(), text->GetSize(), wsz, sizeof(wsz), 
					TaviSysParam.etc.language );
				pwsz = ( uchar_t* )wsz;
			}
			length = pT->GetStringWidth( pwsz, 12, 0 );
			offset = (rcABS.GetWidth()-length)/2;
			if( offset < 0 ) offset = 0;
			pT->DrawUC16Text( pwsz, rcABS.x1+offset, rcABS.y1+hoffset,
				color, 12, DT_TRANSPARENCY );
		}
	}
	else {
		length = pT->GetStringWidth( m_pResIDs[m_nSelectedIdx], 12, 0 );
		offset = (rcABS.GetWidth()-length)/2;
		if( offset < 0 ) offset = 0;
		pT->DrawResText( m_pResIDs[m_nSelectedIdx],
			rcABS.x1+offset, rcABS.y1+hoffset,
			color, 12, DT_TRANSPARENCY );
	}

}

void CSpinSelectBar::OnKeypressed( const KeyEvent& evt )
{
  if( m_disable ) {
    return;
  }
  
	switch( evt.value ) {
	case VK_LEFT:
		m_nSelectedIdx--;
		printf( "m_nSelectedIdx: %d, count: %d\n", m_nSelectedIdx, m_uzList.Count() );
		if( m_nSelectedIdx < 0 ) {
			if( m_fResID ) m_nSelectedIdx = m_nItemCount-1;
			else m_nSelectedIdx = m_uzList.Count()-1;
		}
		OnPaint( rcClient );
		Flush();
		if( m_pfn ) m_pfn( m_nSelectedIdx, m_pfnarg );
		break;
	case VK_RIGHT:
		m_nSelectedIdx++;
		if( m_fResID ) {
			if( m_nSelectedIdx >= m_nCount ) m_nSelectedIdx = 0;
		}
		else {
			if( m_nSelectedIdx >= m_uzList.Count() ) m_nSelectedIdx = 0;
		}
		OnPaint( rcClient );
		if( m_pfn ) m_pfn( m_nSelectedIdx, m_pfnarg );
		Flush();
		break;
	}
}

void CSpinSelectBar::Show( void )
{
	OnPaint( rcClient );
}

void CSpinSelectBar::Hide( void )
{
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), COLOR_TRANSPARENT );
	op_Flush();
}

void CSpinSelectBar::Add( const CString& item)
{
	CString* pItem = new CString( item );
	m_uzList.Add( pItem );
}

void CSpinSelectBar::Add( int id )
{
	if( m_nItemCount >= m_nCount ) return;
	m_pResIDs[m_nItemCount++] = id;
}
int CSpinSelectBar::GetCount(void)
{
	if (m_fResID)
	{
		return m_nItemCount;
	}
	else
	{
		return m_uzList.Count();
	}
}

void CSpinSelectBar::Clear( void ) 
{
	m_uzList.FreeAll();
	m_nItemCount = 0;
}

void CSpinSelectBar::RegisterPFN( PFN_SPINBAR pfn, void * arg)
{
	m_pfn = pfn;
	m_pfnarg = arg;
}

void CSpinSelectBar::ReplaceUniqueText(const CString& item)
{
	Clear();
	Add(item);
}
void CSpinSelectBar::ReplaceUniqueText( int id )
{
	Clear();
	Add(id);
}

