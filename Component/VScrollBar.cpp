//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Vertical ScrollBar Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/VScrollBar.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: VScrollBar.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.15  2005/12/02 08:29:03  zstein
// *** empty log message ***
//
// Revision 1.14  2005/12/02 08:28:38  zstein
// *** empty log message ***
//
// Revision 1.13  2005/12/02 05:09:54  zstein
// *** empty log message ***
//
// Revision 1.12  2005/11/24 03:10:27  zstein
// *** empty log message ***
//
// Revision 1.11  2005/11/22 09:09:40  zstein
// *** empty log message ***
//
// Revision 1.10  2005/10/17 01:25:34  zstein
// - updated UI.
//
// Revision 1.9  2005/08/03 03:23:20  zstein
// - updated.
//
// Revision 1.8  2005/07/16 04:13:52  zstein
// - fixed bugs & added functions.
//
// Revision 1.7  2005/06/21 06:08:29  zstein
// - bug fixed.
//
// Revision 1.6  2005/06/16 23:49:48  zstein
// - bug fixed
//
// Revision 1.5  2005/04/28 05:58:24  zstein
// *** empty log message ***
//
// Revision 1.4  2005/03/01 02:11:05  zstein
// *** empty log message ***
//
// Revision 1.3  2005/02/25 13:57:17  zstein
// Added WindowType
//
// Revision 1.2  2005/02/25 01:13:09  zstein
// *** empty log message ***
//
// Revision 1.1  2005/02/23 02:47:05  zstein
// Vertical ScrollBar Class
//
//
//******************************************************************************

#include "VScrollBar.h"

// Macros
#define DEFAULT_WIDTH				8		// Minimum width 8
#define DEFAULT_HEIGHT				8		// Minimum height 8
#define MINIMUM_SCROLLBAR_HEIGHT	6		// Minimum scrollbar height
#define DEBUG_SCROLLBAR			0

// Extern Variables
extern CPixmap g_SysPixmap;

// Local Variables
static char l_bmLeftTop[4][4] = {
	{ 0, 0, 24, 24 },
	{ 0, 24, 25, 0 },
	{ 24, 25, 0, 0 },
	{ 24, 0, 0, 0 }
};
static char l_bmRightTop[4][4] = {
	{ 24, 24, 0, 0 },
	{ 0, 25, 24, 0 },
	{ 0, 0, 25, 24 },
	{ 0, 0, 0, 24 }
};
static char l_bmLeftBottom[4][4] = {
	{ 24, 0, 0, 0 },
	{ 24, 25, 0, 0 },
	{ 0, 24, 25, 0 },
	{ 0, 0, 24, 24 }
};
static char l_bmRightBottom[4][4] = {
	{ 0, 0, 0, 24 },
	{ 0, 0, 25, 24 },
	{ 0, 25, 24, 0 },
	{ 24, 24, 0, 0 }
};


CVScrollBar::CVScrollBar( CWindow* parent, int nViewCount, int nTotal ) :
	CCScrollBar( parent, nViewCount, nTotal )
{
	m_pmLeftBottom = new CPixmap;
	m_pmLeftBottom->CreatePixmap( 4, 4, 8, (char*)l_bmLeftBottom, false );
	m_pmLeftTop = new CPixmap;
	m_pmLeftTop->CreatePixmap( 4, 4, 8, (char*)l_bmLeftTop, false );
	m_pmRightBottom = new CPixmap;
	m_pmRightBottom->CreatePixmap( 4, 4, 8, (char*)l_bmRightBottom, false );
	m_pmRightTop = new CPixmap;
	m_pmRightTop->CreatePixmap( 4, 4, 8, (char*)l_bmRightTop, false );
	m_pmScrollBar = NULL;

	m_nBorderWidth = 1;
	m_nBorderColor = 24;
	m_nScrollBGColor = 24;
	m_nScrollColor = 17; // Whilte
	m_nYPos = 0;

	m_nWindowType = WINDOW_TYPE_VSCROLL;
}

CVScrollBar::~CVScrollBar( void )
{
	delete m_pmLeftBottom;
	delete m_pmLeftTop;
	delete m_pmRightBottom;
	delete m_pmRightTop;
	if( m_pmScrollBar ) delete m_pmScrollBar;
}

void CVScrollBar::ResizeScrollBar( void )
{
	if( m_pmScrollBar ) {
		delete m_pmScrollBar;
		m_pmScrollBar = NULL;
	}
	#if DEBUG_SCROLLBAR
	printf( "CVScrollBar::Resize()->x1: %d, y1: %d, x2: %d, y2: %d\n",
		rcABS.x1, rcABS.y1, rcABS.x2, rcABS.y2 );
	fflush( stdout );
	#endif /* DEBUG_SCROLLBAR */

	if( m_nTotalCount == 0 ) return;
	
	int nHeight = (rcABS.GetHeight() - 2*m_nBorderWidth);
	m_nScrollHeight = (m_nViewCount*nHeight/m_nTotalCount);
	if( m_nScrollHeight < MINIMUM_SCROLLBAR_HEIGHT )
		m_nScrollHeight = MINIMUM_SCROLLBAR_HEIGHT;
	#if DEBUG_SCROLLBAR
	printf( "total: %d, viewcount: %d, m_nScrollHeight: %d, width: %d, height: %d\n", 
		m_nTotalCount, m_nViewCount, m_nScrollHeight, rcABS.GetWidth(), nHeight );
	fflush( stdout );
	#endif /* DEBUG_SCROLLBAR */
	m_pmScrollBar = new CPixmap;
	m_pmScrollBar->CreatePixmap( 
		rcABS.GetWidth()-2*m_nBorderWidth,
		m_nScrollHeight, 
		8, NULL, true );
	m_pmScrollBar->Fill( 0, 0, m_pmScrollBar->m_nWidth, m_pmScrollBar->m_nHeight, m_nScrollColor );
	m_pmScrollBar->Fill( 0, 0, 1, 1, m_nScrollBGColor );
	m_pmScrollBar->Fill( 1, 0, 1, 1, 19 );
	m_pmScrollBar->Fill( 0, 1, 1, 1, 19 );
	m_pmScrollBar->Fill( m_pmScrollBar->m_nWidth-1, 0, 1, 1, m_nScrollBGColor );
	m_pmScrollBar->Fill( m_pmScrollBar->m_nWidth-2, 0, 1, 1, 19 );
	m_pmScrollBar->Fill( m_pmScrollBar->m_nWidth-1, 1, 1, 1, 19 );
	m_pmScrollBar->Fill( 0, m_pmScrollBar->m_nHeight-1, 1, 1, m_nScrollBGColor );
	m_pmScrollBar->Fill( 0, m_pmScrollBar->m_nHeight-2, 1, 1, 19 );
	m_pmScrollBar->Fill( 1, m_pmScrollBar->m_nHeight-1, 1, 1, 19 );
	m_pmScrollBar->Fill( m_pmScrollBar->m_nWidth-1, m_pmScrollBar->m_nHeight-1, 1, 1, m_nScrollBGColor );
	m_pmScrollBar->Fill( m_pmScrollBar->m_nWidth-2, m_pmScrollBar->m_nHeight-1, 1, 1, 19 );
	m_pmScrollBar->Fill( m_pmScrollBar->m_nWidth-1, m_pmScrollBar->m_nHeight-2, 1, 1, 19 );

	// Fill background color in the edge
	
}

void CVScrollBar::Resize( const CRect& rc, bool fUpdate )
{
	CWindow::Resize( rc, false );

	ResizeScrollBar();
}

void CVScrollBar::OnPaint( const CRect& rc )
{
	if( m_fVisible == false ) return;
	
	if( m_nTotalCount <= m_nViewCount ) {
		//Hide();
		return;
	}

	CWindow::OnPaint( rc );
	// draw scroll frame
	int w = rcABS.GetWidth() - 8;
	int h = rcABS.GetHeight() -8;
	if( w < 0 ) w = 0;
	if ( h < 0 ) h = 0;

	// Fill background.
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nScrollBGColor );
	
	// Draw round of bar
	op_BitBlt( &g_SysPixmap, rcABS.x1, rcABS.y1, 
		m_pmLeftTop, 0, 0, m_pmLeftTop->m_nWidth, m_pmLeftTop->m_nHeight );
	op_BitBlt( &g_SysPixmap, rcABS.x1+DEFAULT_WIDTH/2+w, rcABS.y1,
		m_pmRightTop, 0, 0, m_pmRightTop->m_nWidth, m_pmRightTop->m_nHeight );
	op_BitBlt( &g_SysPixmap, rcABS.x1, rcABS.y1+DEFAULT_HEIGHT/2+h,
		m_pmLeftBottom, 0, 0, m_pmLeftBottom->m_nWidth, m_pmLeftBottom->m_nHeight );
	op_BitBlt( &g_SysPixmap, rcABS.x1+DEFAULT_WIDTH/2+w, rcABS.y1+DEFAULT_HEIGHT/2+h,
		m_pmRightBottom, 0, 0, m_pmRightBottom->m_nWidth, m_pmRightBottom->m_nHeight );
	

	// Draw scrollbar
	int nHeight = rcABS.GetHeight() - 2*m_nBorderWidth;
	//if( m_nStart == (m_nTotalCount-m_nViewCount) ) m_nYPos = nHeight-m_pmScrollBar->m_nHeight;
	//else m_nYPos = (m_nStart*nHeight/m_nTotalCount) - m_pmScrollBar->m_nHeight;
	m_nYPos = m_nStart*nHeight/m_nTotalCount;
	if( m_nYPos > (nHeight-m_pmScrollBar->m_nHeight) ) 
		m_nYPos = nHeight-m_pmScrollBar->m_nHeight;
	op_BitBlt( &g_SysPixmap, rcABS.x1+m_nBorderWidth, rcABS.y1+m_nBorderWidth+m_nYPos,
		m_pmScrollBar, 0, 0, m_pmScrollBar->m_nWidth, m_pmScrollBar->m_nHeight );

	// Draw border line
	g_SysPixmap.Fill( rcABS.x1+DEFAULT_WIDTH/2-2, rcABS.y1, w+4, 1, m_nBorderColor ); // top line
	g_SysPixmap.Fill( rcABS.x1+1, rcABS.y1+1, 1, 1, m_nBorderColor );
	g_SysPixmap.Fill( rcABS.x2-1, rcABS.y1+1, 1, 1, m_nBorderColor );
	g_SysPixmap.Fill( rcABS.x1+DEFAULT_WIDTH/2-2, rcABS.y2, w+4, 1, m_nBorderColor ); // bottom line
	g_SysPixmap.Fill( rcABS.x1+1, rcABS.y2-1, 1, 1, m_nBorderColor );
	g_SysPixmap.Fill( rcABS.x2-1, rcABS.y2-1, 1, 1, m_nBorderColor );
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1+DEFAULT_HEIGHT/2-2, 1, h+4, m_nBorderColor ); // left vertical line
	g_SysPixmap.Fill( rcABS.x2, rcABS.y1+DEFAULT_HEIGHT/2-2, 1, h+4, m_nBorderColor ); // right vertical line

	// Delete Edge
	int bgcolor = ( m_parent ? m_parent->m_nBackgroundColor : 0 );
	// left top
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, 2, 1, bgcolor );
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1+1, 1, 1, bgcolor );
	// right top
	g_SysPixmap.Fill( rcABS.x2-1, rcABS.y1, 2, 1, bgcolor );
	g_SysPixmap.Fill( rcABS.x2, rcABS.y1+1, 1, 1, bgcolor );
	// left bottom
	g_SysPixmap.Fill( rcABS.x1, rcABS.y2, 2, 1, bgcolor );
	g_SysPixmap.Fill( rcABS.x1, rcABS.y2-1, 1, 1, bgcolor );
	// right bottom
	g_SysPixmap.Fill( rcABS.x2-1, rcABS.y2, 2, 1, bgcolor );
	g_SysPixmap.Fill( rcABS.x2, rcABS.y2-1, 1, 1, bgcolor );

	#if DEBUG_SCROLLBAR
	printf( "w: %d, h: %d, m_nBorderWidth: %d\n", rcABS.GetWidth(), rcABS.GetHeight(), m_nBorderWidth );
	printf( "m_nYPos: %d, m_nTotalCount: %d, m_nStart: %d, nHeight: %d\n", 
		m_nYPos, m_nTotalCount, m_nStart, nHeight );
	printf( "x: %d, y: %d, w: %d, h: %d\n",
		rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight() );
	printf( " scrollbar: ypos: %d, x: %d, y: %d, w: %d, h: %d\n",
		m_nYPos, rcABS.x1+m_nBorderWidth, rcABS.y1+m_nBorderWidth+m_nYPos, 
		m_pmScrollBar->m_nWidth, m_pmScrollBar->m_nHeight );
	#endif /* DEBUG_SCROLLBAR */
}

void CVScrollBar::UpdateScrollBar( int nStart )
{
	if( m_nTotalCount == 0 || m_nViewCount >= m_nTotalCount ) {
		g_SysPixmap.Fill( 
			rcABS.x1, rcABS.y1,
			rcABS.GetWidth(),	rcABS.GetHeight(),
			m_parent->m_nBackgroundColor );
	}
	else {
		// Clear previous
		g_SysPixmap.Fill( rcABS.x1+m_nBorderWidth, rcABS.y1+m_nBorderWidth+m_nYPos,
			rcABS.GetWidth()-2*m_nBorderWidth, m_pmScrollBar->m_nHeight, m_nScrollBGColor );

		// draw updated scrollbar
		m_nStart = nStart;
		int nHeight = rcABS.GetHeight() - 2*m_nBorderWidth;
		m_nYPos = m_nStart*nHeight/m_nTotalCount;
		if( m_nYPos > (nHeight-m_pmScrollBar->m_nHeight) ) 
			m_nYPos = nHeight-m_pmScrollBar->m_nHeight;
		//if( m_nStart == (m_nTotalCount-m_nViewCount) ) m_nYPos = nHeight-m_pmScrollBar->m_nHeight;
		//else m_nYPos = (m_nStart*nHeight/m_nTotalCount);
		op_BitBlt( &g_SysPixmap, rcABS.x1+m_nBorderWidth, rcABS.y1+m_nBorderWidth+m_nYPos,
			m_pmScrollBar, 0, 0, m_pmScrollBar->m_nWidth, m_pmScrollBar->m_nHeight );
	}
	op_Flush();
}

void CVScrollBar::SetScrollBackgroundColor( int nColor )
{
	CCScrollBar::SetScrollBackgroundColor( nColor );
	// Set background color in left top corner image
	m_pmLeftTop->Fill( 2, 2, 2, 2, m_nScrollBGColor );
	m_pmLeftTop->Fill( 3, 1, 1, 1, m_nScrollBGColor );
	m_pmLeftTop->Fill( 1, 3, 1, 1, m_nScrollBGColor );
	// Set background color in right top corner image
	m_pmRightTop->Fill( 0, 2, 2, 2, m_nScrollBGColor );
	m_pmRightTop->Fill( 0, 1, 1, 1, m_nScrollBGColor );
	m_pmRightTop->Fill( 2, 3, 1, 1, m_nScrollBGColor );
	// Set background color in left bottom coner image.
	m_pmLeftBottom->Fill( 2, 0, 2, 2, m_nScrollBGColor );
	m_pmLeftBottom->Fill( 1, 0, 1, 1, m_nScrollBGColor );
	m_pmLeftBottom->Fill( 3, 2, 1, 1, m_nScrollBGColor );
	// Set background color in right bottom corner image
	m_pmRightBottom->Fill( 0, 0, 2, 2, m_nScrollBGColor );
	m_pmRightBottom->Fill( 2, 0, 1, 1, m_nScrollBGColor );
	m_pmRightBottom->Fill( 0, 2, 1, 1, m_nScrollBGColor );
}
