//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Bottom Window class definition

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/BottomWnd.cpp,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************
// $Log: BottomWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.9  2005/12/01 01:02:54  zstein
// *** empty log message ***
//
// Revision 1.8  2005/11/28 00:50:02  zstein
// *** empty log message ***
//
// Revision 1.7  2005/07/20 06:09:43  zstein
// - updated.
//
// Revision 1.6  2005/05/18 16:01:41  ywkim
// Change coordinate of leftbutton
//
// Revision 1.5  2005/05/18 15:21:28  zstein
// - added some functions.
//
// Revision 1.4  2005/04/05 08:02:36  zstein
// *** empty log message ***
//
// Revision 1.3  2005/03/04 16:24:59  zstein
// change left&right button position
//
// Revision 1.2  2005/03/04 13:16:30  zstein
// BUG FIXED:
//
// Revision 1.1  2005/03/01 01:57:10  zstein
// *** empty log message ***
//
// Revision 1.3  2005/02/25 01:13:54  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/21 02:11:39  zstein
// BUG fixed & update
//
// Revision 1.1  2005/02/16 16:17:37  zstein
// *** empty log message ***
//
//******************************************************************************

#include <tavi_lib.h>
#include "BottomWnd.h"

extern BITMAPFILEINFO256 TAVI_IMAGES[];

CBottomWnd::CBottomWnd( CWindow* parent, int mode ) :
	CWindow( parent ),
	m_nMode( mode )
{
	int iw, ih;
	char* pImage;
	int offset;

	if( mode == BW_TRANSPARENT ) offset = 1;
	else offset = 0;
	#if 0
	iw = TAVI_IMAGES[ICON_BOTTOM_LEFT].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[ICON_BOTTOM_LEFT].bitmapinfoheader.biHeight/2;
	pImage = ( char* )TAVI_IMAGES[ICON_BOTTOM_LEFT].image_buffer+offset*iw*ih;
	m_puzLeftBtn = new CPixmap;
	if( m_puzLeftBtn ) {
		m_puzLeftBtn->CreatePixmap( iw, ih,	8, pImage, false );
	}
	iw = TAVI_IMAGES[ICON_BOTTOM_RIGHT].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[ICON_BOTTOM_RIGHT].bitmapinfoheader.biHeight/8;
	pImage = ( char* )TAVI_IMAGES[ICON_BOTTOM_RIGHT].image_buffer;
	for( int i=0; i<8; i++ ) {
		m_puzRightBtn[i].CreatePixmap( iw, ih, 8, pImage+i*iw*ih, false );
	}
	#endif
	iw = TAVI_IMAGES[FRAME_BOTTOM].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[FRAME_BOTTOM].bitmapinfoheader.biHeight;
	pImage = ( char* )TAVI_IMAGES[FRAME_BOTTOM].image_buffer;
	m_puzBottom = new CPixmap;
	if( m_puzBottom ) 
		m_puzBottom->CreatePixmap( iw, ih, 8, pImage, false );

	m_flButtonShow = true;
	m_frButtonShow = true;
	m_nCurrentRButton = RB_TOOL;
	
}

CBottomWnd::~CBottomWnd( void )
{
	if( m_puzLeftBtn ) delete m_puzLeftBtn;
	if( m_puzBottom ) delete m_puzBottom;
}

#if 0
void CBottomWnd::ShowLeftButton( bool fFlush )
{
	m_flButtonShow = true;
	op_BitBlt( &g_SysPixmap, 
		rcABS.x1+5/*3*/, 
		rcABS.y1+rcABS.GetHeight()-m_puzLeftBtn->m_nHeight-2, m_puzLeftBtn,
		0, 0, m_puzLeftBtn->m_nWidth, m_puzLeftBtn->m_nHeight );

	if( fFlush ) op_Flush();
}

void CBottomWnd::SetRightButton( int idx )
{
	int offset;
	
	if( m_nMode == BW_TRANSPARENT) offset = 1;
	else offset = 0;
	m_nCurrentRButton = idx;
}

void CBottomWnd::ShowRightButton( int idx, bool fFlush )
{
	int offset;

	m_frButtonShow = true;
	if( m_nMode == BW_TRANSPARENT) offset = 1;
	else offset = 0;
	m_nCurrentRButton = idx;
	idx = 2*idx + offset;
	op_BitBlt( &g_SysPixmap, 
		rcABS.x2-m_puzRightBtn[idx].m_nWidth-3, 
		rcABS.y1+rcABS.GetHeight()-m_puzRightBtn[idx].m_nHeight-2, 
		&m_puzRightBtn[idx], 0, 0, 
		m_puzRightBtn[idx].m_nWidth, 
		m_puzRightBtn[idx].m_nHeight );

	//printf( "%d, %d, %d, %d\n", rcABS.x2-m_puzRightBtn[idx].m_nWidth-6, 
	//	rcABS.y1+rcABS.GetHeight()-m_puzRightBtn[idx].m_nHeight-2, 
	//	m_puzRightBtn[idx].m_nWidth, 
	//	m_puzRightBtn[idx].m_nHeight );
	
	if( fFlush ) op_Flush();
}

void CBottomWnd::HideLeftButton( void )
{
	m_flButtonShow = false;
}

void CBottomWnd::HideRightButton( void )
{
	m_frButtonShow = false;
}
#endif

void CBottomWnd::OnPaint( const CRect& rc )
{
	CWindow::OnPaint( rcClient );
	if( m_fVisible == false ) return;
	
	if( m_nMode != BW_TRANSPARENT ) {
		op_BitBlt( &g_SysPixmap, rcABS.x1, rcABS.y1, 
			m_puzBottom, 0, 0, m_puzBottom->m_nWidth, m_puzBottom->m_nHeight );
	}
	// Draw left bottom button
	//if( m_flButtonShow ) ShowLeftButton( false );
	// Draw Right bottom button
	//if( m_frButtonShow ) ShowRightButton( m_nCurrentRButton, false );
}

