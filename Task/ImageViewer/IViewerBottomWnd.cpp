//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Bottom Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IViewerBottomWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IViewerBottomWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.20  2005/12/01 01:03:57  zstein
// *** empty log message ***
//
// Revision 1.19  2005/08/29 08:05:55  zstein
// - updated ui
//
// Revision 1.18  2005/08/25 07:54:18  zstein
// - updated ui
//
// Revision 1.17  2005/07/20 11:47:19  zstein
// - Updated UI.
//
// Revision 1.16  2005/07/20 06:13:44  zstein
// - updated UI.
//
// Revision 1.15  2005/06/28 01:19:40  zstein
// - fixed bugs.
//
// Revision 1.14  2005/06/14 08:26:19  zstein
// - changed touchpad I/F.
//
// Revision 1.13  2005/06/01 13:24:14  zstein
// - bug fixed: timer
//
// Revision 1.12  2005/05/18 15:19:45  zstein
// - bug fixed
//
// Revision 1.11  2005/05/17 19:52:29  zstein
// - bug fixed.
//
// Revision 1.10  2005/05/17 16:41:05  zstein
// - bug fixed
//
// Revision 1.9  2005/05/17 09:25:37  zstein
// - BUG FIXED
//
// Revision 1.8  2005/05/16 05:37:41  zstein
// *** empty log message ***
//
// Revision 1.7  2005/05/15 10:54:37  zstein
// - changed label i/f.
//
// Revision 1.6  2005/05/12 04:48:57  zstein
// *** empty log message ***
//
// Revision 1.5  2005/05/03 02:13:51  zstein
// *** empty log message ***
//
// Revision 1.4  2005/04/27 05:02:53  zstein
// Support M-langauges.
//
// Revision 1.3  2005/03/05 16:39:06  zstein
// BUG FIXED.
//
// Revision 1.2  2005/03/05 13:45:24  zstein
// *** empty log message ***
//
// Revision 1.1  2005/03/04 13:08:57  zstein
// New version Imager Viewer
//
//

//*******************************************************************************

#include <Lib/LibImage.h>
#include "IViewer.h"
#include "IViewerBottomWnd.h"
#include <Task/TopPanel.h>
#include <Task/tavi_global.h>

// Macros
#define PLAY_STATUS_ICON_COUNT	5
#define TIMEOUT						5000 // 5sec

// Extern Variables
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CTopPanel* g_TopPanel;


CIViewerBottomWnd::CIViewerBottomWnd( CWindow* parent ) :
	CBottomWnd( parent, CBottomWnd::BW_NORMAL )
{
	int iw = TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS].bitmapinfoheader.biWidth;
	int ih = TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS].bitmapinfoheader.biHeight/PLAY_STATUS_ICON_COUNT;
	char* pImage = ( char* )TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS].image_buffer;

	// Play Pixmap
	m_pmPlay = new CPixmap;
	m_pmPlay->CreatePixmap( iw, ih, 8, pImage, false );
	m_pmPause = new CPixmap;
	m_pmPause->CreatePixmap( iw, ih, 8, pImage+iw*ih, false );

	iw = TAVI_IMAGES[ICON_REPEAT].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[ICON_REPEAT].bitmapinfoheader.biHeight/REPEAT_ICON_COUNT;
	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer+13*iw*ih;
	m_pmUnRepeat = new CPixmap;
	m_pmUnRepeat->CreatePixmap( iw, ih, 8, pImage, false );

	// Repeat icon
	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + iw*ih;
	m_pmRepeat = new CPixmap;
	m_pmRepeat->CreatePixmap( iw, ih, 8, pImage, false );

	// Angle icon	
	iw = TAVI_IMAGES[ICON_ROTATE].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[ICON_ROTATE].bitmapinfoheader.biHeight/ROTATE_ICON_COUNT;
	pImage = ( char* )TAVI_IMAGES[ICON_ROTATE].image_buffer;
	printf( "ROTATE: w(%d), h(%d)\n", iw, ih );
	for( int i=0; i<ROTATE_ICON_COUNT; i++ ) {
		m_pmRotate[i] = new CPixmap;
		m_pmRotate[i]->CreatePixmap( iw, ih, 8, pImage+i*iw*ih, false );
	}
	
	// Scoreboard label
	m_plbTotal = new CLabel( this, _WHITE, 24, "" );
	m_plbTotal->Resize( CRect(200, 27, 284, 38 ), false );
	m_plbTotal->m_nBackgroundColor = 24;
	m_plbTotal->m_nFontHeight = 12;
	m_plbTotal->SetJustify( CLabel::CLABEL_RIGHT_JUSTIFY );
	int nTotal = ((CIViewer*)m_parent)->GetTotal();
	int nCurrent = ((CIViewer*)m_parent)->GetCurrent();
	char buf[30];
	sprintf( buf, "%d/%d", nCurrent, nTotal );
	m_plbTotal->SetText( buf, false );

	// ProgressBar
	m_pProgressBar = new CProgressBar( this );
	m_pProgressBar->Resize( CRect(32, 20, 287, 25), false );
	m_pProgressBar->SetRange( 0, nTotal );
	m_pProgressBar->SetPos( nCurrent );
	m_pProgressBar->SetMode( CProgressBar::SLIDE_BAR );
	
	
	m_plbName = new CLabel( this, _WHITE, 24, "" );
	m_plbName->Resize( CRect(47, 2, 220, 18), false );
	m_plbName->m_nFontHeight = 12;
	
	m_plbName->SetText( ((CIViewer*)m_parent)->GetCurrentFilename(), false );
	m_plbName->m_nBackgroundColor = 24;
	m_plbName->m_nFontHeight = 12;

	m_nRotate = 0;
}

CIViewerBottomWnd::~CIViewerBottomWnd( void )
{
	delete m_pmPlay;
	delete m_pmPause;
	delete m_pmRepeat;
	delete m_pmUnRepeat;
	for( int i=0; i<ROTATE_ICON_COUNT; i++ ) delete m_pmRotate[i];
}

void CIViewerBottomWnd::OnPaint( const CRect& rc )
{
	if( m_fVisible == false ) {
		return;
	}
	
	if( m_plbTotal->IsShow() == false ) m_plbTotal->SetVisible( true );
	
	CIViewer* pViewer = ( CIViewer* )m_parent;
	if( m_fVisible == false ) {
		return;
	}
	
	CBottomWnd::OnPaint( rc );

	CPixmap* pm;
	if( pViewer->GetPlayStatus() == CIViewer::PLAY ) pm = m_pmPlay;
	else pm = m_pmPause;

	// Draw play status image
	int offset_h = 5;
	op_BitBlt( &g_SysPixmap, rcABS.x1+32, rcABS.y1+offset_h, 
		pm, 0, 0, pm->m_nWidth, pm->m_nHeight );

	// Draw repeat icon
	UpdateRepeat();
	// Draw rotate icon
	UpdateRotate();
}

void CIViewerBottomWnd::Update( void )
{
	CIViewer* pViewer = ( CIViewer* )m_parent;
	int status;
	char buf[256];
	CPixmap* pixmap;
	
	// Update total count;
	sprintf( buf, "%d/%d", pViewer->GetCurrent()+1, pViewer->GetTotal() );
	m_plbTotal->SetText( buf, m_fVisible );

	// Update filename
	m_plbName->SetText( pViewer->GetCurrentFilename(), m_fVisible );

	// Update play status
	if( m_fVisible ) {
		status = pViewer->GetPlayStatus();
		if( status ) pixmap = m_pmPlay;
		else pixmap = m_pmPause;
		op_BitBlt( &g_SysPixmap, rcABS.x1 + 32, rcABS.y1+5,
			pixmap, 0, 0, pixmap->m_nWidth, pixmap->m_nHeight );
	}
}

void CIViewerBottomWnd::Show( void )
{
	CWindow::Show();
	//gettimeofday( &m_tvHide, NULL );
}

void CIViewerBottomWnd::UpdateRepeat( void )
{
	CPixmap* pixmap = NULL;
	
	if( TaviSysParam.photo.repeat ) pixmap = m_pmRepeat;
	else pixmap = m_pmUnRepeat;

	op_BitBlt( &g_SysPixmap, 
		rcABS.x2-60, rcABS.y1+3,
		pixmap, 0, 0, pixmap->m_nWidth, pixmap->m_nHeight );
}

void CIViewerBottomWnd::UpdateRotate( int rotate )
{
	if( rotate != -1 ) m_nRotate = rotate;
	//printf( "CIViewerBottomWnd::UpdateRotate( %d )\n", m_nRotate );
	CPixmap* pixmap = m_pmRotate[m_nRotate];
	op_BitBlt( &g_SysPixmap, 
		rcABS.x2-60-pixmap->m_nWidth-2, rcABS.y1+3,
		pixmap, 0, 0, pixmap->m_nWidth, pixmap->m_nHeight );
}
