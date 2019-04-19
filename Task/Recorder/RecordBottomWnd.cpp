//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Recorder Bottom Window
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecordBottomWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: RecordBottomWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2005/12/26 04:37:23  zstein
// *** empty log message ***
//
// Revision 1.6  2005/11/28 00:50:12  zstein
// *** empty log message ***
//
// Revision 1.5  2005/11/22 09:07:10  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <Task/tavi_global.h>
#include <Lib/LibImage.h>
#include <Component/Bitmap.h>
#include "RecordBottomWnd.h"
#include "RecordWnd.h"

extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CTextEngine_BDF* pT;

static unsigned char* GetImageElement( BITMAPFILEINFO256 bmp, int id, int count )
{
	unsigned char* p;
	int w = bmp.bitmapinfoheader.biWidth;
	int h = bmp.bitmapinfoheader.biHeight;
	int dh = h/count;

	p = bmp.image_buffer+w*dh*id;

	return p;
}

CRecordBottomWnd::CRecordBottomWnd( CWindow* parent, int nCurrentTime, int nAvailTime ) :
	CBottomWnd( parent, CBottomWnd::BW_NORMAL ),
	m_nCurrentTime( nCurrentTime ),
	m_nAvailTime( nAvailTime )
{
	m_pmRecord = new CPixmap;
	m_pmRecord->CreatePixmap( 12, 10, 8, 
		(char*)GetImageElement(TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS], 4, 5 ), false );
	m_pmPause = new CPixmap;
	m_pmPause->CreatePixmap( 12, 10, 8, 
		(char*)GetImageElement(TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS], 1, 5 ), false );
	m_pmStatus = m_pmPause;
	
	m_pProgressBar = new CProgressBar( this );
	m_pProgressBar->Resize( CRect(32, 19, 287, 24), false );
	m_pProgressBar->SetRange( 0, m_nAvailTime );
	m_pProgressBar->SetPos( 0 );

	m_pmAGCOn = NULL;
	m_pmAGCOff = NULL;
	char* pimage = (char*)TAVI_IMAGES[RECORD_AGC].image_buffer;
	int iw = TAVI_IMAGES[RECORD_AGC].bitmapinfoheader.biWidth;
	int ih = TAVI_IMAGES[RECORD_AGC].bitmapinfoheader.biHeight/2;
	m_pmAGCOn = new CPixmap;
	m_pmAGCOn->CreatePixmap( iw, ih, 8, pimage, false );
	m_pmAGCOff = new CPixmap;
	m_pmAGCOff->CreatePixmap( iw, ih, 8, pimage+iw*ih, false );

	m_pmBitrate[0] = NULL;
	m_pmBitrate[1] = NULL;
	pimage = (char*)TAVI_IMAGES[RECORD_BITRATE].image_buffer;
	iw = TAVI_IMAGES[RECORD_BITRATE].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[RECORD_BITRATE].bitmapinfoheader.biHeight/2;

	m_pmBitrate[0] = new CPixmap; // 64kbps
	m_pmBitrate[0]->CreatePixmap( iw, ih, 8, pimage, false );
	m_pmBitrate[1] = new CPixmap; // 128kbps
	m_pmBitrate[1]->CreatePixmap( iw, ih, 8, pimage+iw*ih, false );
}

CRecordBottomWnd::~CRecordBottomWnd( void )
{
	if( m_pmRecord ) delete m_pmRecord;
	if( m_pmPause ) delete m_pmPause;
	if( m_pmAGCOn ) delete m_pmAGCOn;
	if( m_pmAGCOff ) delete m_pmAGCOff;
	if( m_pmBitrate[0] ) delete m_pmBitrate[0];
	if( m_pmBitrate[1] ) delete m_pmBitrate[1];
}

void CRecordBottomWnd::OnKeypressed( const KeyEvent& evt )
{
	CWindow* focusedWnd;
	
	switch( evt.value ) {
	case VK_F2: // menu
		break;
	case VK_ESC:
		break;
	case VK_UP:
		break;
	case VK_DOWN:
		break;
	}
}

void CRecordBottomWnd::OnPaint( const CRect& rc )
{
	CRecorderWnd* pRec = ( CRecorderWnd* )m_parent;
	int offset_h = 5;
	int posx = rcABS.x1 + 204;
	
	CBottomWnd::OnPaint( rc );
	op_BitBlt( &g_SysPixmap, rcABS.x1+32, rcABS.y1+offset_h, 
		m_pmStatus, 0, 0, m_pmStatus->m_nWidth, m_pmStatus->m_nHeight );

	// Bitrate
	if( TaviSysParam.record.bitrate == BITRATE_64K ) UpdateBitrate( 64 );
	else UpdateBitrate( 128 );

	// AGC
	CPixmap* pm;
	if( TaviSysParam.record.AGC ) pm = m_pmAGCOn;
	else pm = m_pmAGCOff;
	posx += 36;
	if( pm ) {
		op_BitBlt( &g_SysPixmap, posx, rcABS.y1+2,
			pm, 0, 0, pm->m_nWidth, pm->m_nHeight );
	}

	char buf[20];
	int h = m_nCurrentTime/3600;
	int m = (m_nCurrentTime-h*3600)/60;
	int s = m_nCurrentTime%60;

	if( h/100 ) sprintf( buf, "%03d:%02d:%02d", h, m, s );
	else sprintf( buf, "%02d:%02d:%02d", h, m, s );
	DrawASCIITextWithoutRefresh( buf, 8, 7, &TAVI_IMAGES[FONT_DIGIT3], 32, 229, __DIGIT3);

	h = m_nAvailTime/3600;
	m = (m_nAvailTime-h*3600)/60;
	s = m_nAvailTime%60;	
	int offset = 0;
	if( h/100 ) {
		sprintf( buf, "%03d:%02d:%02d", h, m, s );
		offset = 4;
	}
	else sprintf( buf, "%02d:%02d:%02d", h, m, s );
	DrawASCIITextWithoutRefresh( buf, 8, 7, &TAVI_IMAGES[FONT_DIGIT3], 250-14-offset, 229, __DIGIT3);

	if( pRec->m_fRecording ) pT->DrawResText( ML_RECORD, rcABS.x1+32+20, rcABS.y1+3, _WHITE, 12, DT_TRANSPARENCY );
	else pT->DrawResText( ML_STOP, rcABS.x1+32+20, rcABS.y1+3, _WHITE, 12, DT_TRANSPARENCY );
	
	m_pProgressBar->SetPos( m_nCurrentTime );
	m_pProgressBar->Update();
	mep_eat_event();
}

void CRecordBottomWnd::SetStatus( int record )
{
	int offset_h = 5;

	if( record ) m_pmStatus = m_pmRecord;
	else m_pmStatus = m_pmPause;
	
	op_BitBlt( &g_SysPixmap, rcABS.x1+32, rcABS.y1+offset_h, 
		m_pmStatus, 0, 0, m_pmStatus->m_nWidth, m_pmStatus->m_nHeight );

	g_SysPixmap.Fill( rcABS.x1+32+20, rcABS.y1+3, 60, 12, 24 );
	if( record ) pT->DrawResText( ML_RECORD, rcABS.x1+32+20, rcABS.y1+3, _WHITE, 12, DT_TRANSPARENCY );
	else pT->DrawResText( ML_STOP, rcABS.x1+32+20, rcABS.y1+3, _WHITE, 12, DT_TRANSPARENCY );
}

void CRecordBottomWnd::UpdateCurrentTime( int sec, bool fUpdate )
{
	char buf[20];
	int h = sec/3600;
	int m = (sec-h*3600)/60;
	int s = sec%60;

	m_nCurrentTime = sec;

	if( h/100 ) {
		sprintf( buf, "%03d:%02d:%02d  ", h, m, s );
	}
	else {
		sprintf( buf, "%02d:%02d:%02d  ", h, m, s );
	}
	if( fUpdate ) DrawASCIIText( buf, 8, 7, &TAVI_IMAGES[FONT_DIGIT3], 32, 229, __DIGIT3);
	
	m_pProgressBar->SetPos( sec );
	if( fUpdate ) m_pProgressBar->Update();
}

void CRecordBottomWnd::UpdateTotalTime( int sec, bool fUpdate )
{
	char buf[20];
	int h = sec/3600;
	int m = (sec-h*3600)/60;
	int s = sec%60;

	m_nAvailTime = sec;
	if( h/100 ) {
		sprintf( buf, "%03d:%02d:%02d  ", h, m, s );
	}
	else {
		sprintf( buf, "%02d:%02d:%02d  ", h, m, s );
	}
	if( fUpdate ) DrawASCIIText( buf, 8, 7, &TAVI_IMAGES[FONT_DIGIT3], 250-14, 229, __DIGIT3);
}

void CRecordBottomWnd::UpdateBitrate( int bitrate )
{
	CPixmap* pm = NULL;
	int posx = rcABS.x1 + 204;
	
	if( bitrate == 64 ) {
		pm = m_pmBitrate[0];
	}
	else if( bitrate == 128 ) {
		pm = m_pmBitrate[1];
	}

	if( pm ) {
		op_BitBlt( &g_SysPixmap, posx, rcABS.y1+2,
			pm, 0, 0, pm->m_nWidth, pm->m_nHeight );
	}
}

void CRecordBottomWnd::OnWindowEvent( const WindowEvent& evt )
{
	switch( evt.nMessage ) {
	case WINDOW_NOTIFY:
		SendMessage( WINDOW_KILL, m_parent->GetWindowID() );
		break;
	}
}
