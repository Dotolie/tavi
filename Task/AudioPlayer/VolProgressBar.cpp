//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Volume ProgressBar class definition
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/VolProgressBar.cpp,v $
// $Revision: 1.4 $
// $Date: 2006/03/13 02:32:51 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: VolProgressBar.cpp,v $
// Revision 1.4  2006/03/13 02:32:51  zstein
// - fixed volume control bug.
//
// Revision 1.3  2006/03/09 02:12:34  zstein
// - changed volume interface.
//
// Revision 1.2  2006/03/07 06:27:15  zstein
// -
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.27  2005/12/14 11:34:57  yong
// external LCD
//
// Revision 1.26  2005/11/25 10:21:58  zstein
// *** empty log message ***
//
// Revision 1.25  2005/11/17 11:19:57  zstein
// *** empty log message ***
//
// Revision 1.24  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
//******************************************************************************
#include <Lib/LibImage.h>
#include <meplib.h>
#include <Task/ExtLCD.h>
#include "VolProgressBar.h"
#include "PlayerWnd.h"

// Macros
#define SCROLL_OFFSET		40
#define OFFSET 				18  // VOLUME PROGRESS BAR Y-OFFSET
#define TIMEOUT				5000 // 5 sec. 
#define PROGRESS_RANGE		40

// Extern Variables
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CSndCtrl* g_SndCtrl;
extern CExtLCD* g_ExtLCD;

// Local Variables
static int s_nCount;

CVolProgressBar::CVolProgressBar( CWindow* parent ) :
	CWindow( parent )
{
	int iw = TAVI_IMAGES[TOUCH_MINI_ICON].bitmapinfoheader.biWidth;
	int ih = TAVI_IMAGES[TOUCH_MINI_ICON].bitmapinfoheader.biHeight/4;
	char* pImage = ( char* )TAVI_IMAGES[TOUCH_MINI_ICON].image_buffer;
	CPlayerBottomWnd* pBottom = ( CPlayerBottomWnd* )parent;
	
	m_pVolMinus = new CPixmap;
	m_pVolMinus->CreatePixmap( iw, ih, 8, pImage, false );
	m_pVolPlus = new CPixmap;
	m_pVolPlus->CreatePixmap( iw, ih, 8, pImage+iw*ih, false );

	m_nVolLevel = g_SndCtrl->GetVolume();
	m_nVolRange = g_SndCtrl->GetVolumeRange();
 
	m_pProgressBar = new CProgressBar( this );
	m_pProgressBar->Resize( CRect(4, 17, 259, 22), false );
	m_pProgressBar->SetRange( 0, PROGRESS_RANGE );
	m_pProgressBar->ShowBall( true );
	m_pProgressBar->SetPos( m_nVolLevel );

	m_plbVolume = new CLabel( this, _WHITE, 24, ML_VOLUME );
	m_plbVolume->Resize( CRect(60, 0, 132, 14), false );
	m_plbVolume->m_nFontHeight = 12;
	m_plbVolume->SetJustify( CLabel::CLABEL_RIGHT_JUSTIFY );

	char buf[10];
	sprintf( buf, "%d", m_nVolLevel );
	m_plbLevel = new CLabel( this, _GREEN, 24, buf );
	m_plbLevel->Resize( CRect(138, 0, 200, 14), false );
	m_plbLevel->m_nFontHeight = 12;
	m_plbLevel->SetJustify( CLabel::CLABEL_LEFT_JUSTIFY );

	m_fVolumeStart = false;
}

CVolProgressBar::~CVolProgressBar( void )
{
	if( m_pVolMinus ) delete m_pVolMinus;
	if( m_pVolPlus ) delete m_pVolPlus;
}

void CVolProgressBar::OnMouseEvent( const MouseEvent& evt )
{
	CPlayerBottomWnd* pBottom = ( CPlayerBottomWnd* )m_parent;
	int level;

	// change touchmenu
	if( evt.click ) {
		pBottom->ChangeTouchMenu( CPlayerBottomWnd::NORMAL );
		mep_eat_event();
		return;
	}

	level = m_nVolLevel;
	if( evt.x > 0 ) {
	    level++;
    }
	else if( evt.x < 0 ) {
	    level--;
    }
	else {
	    return;
    }
	
	if( level > m_nVolRange ) {
		return;
	}
	else if( level < 0 ) {
		return;
	}
	
	SetVolume( level );
}

void CVolProgressBar::OnKeypressed( const KeyEvent& evt )
{
	CPlayerBottomWnd* pBottom = ( CPlayerBottomWnd* )m_parent;
	bool fChanged = false;
	int level = m_nVolLevel;

	//printf( "CVolProgressBar::OnKeypressed(0x%x, %d)\n", evt.value, evt.pressed );
	
	switch( evt.value ) {
	case VK_DOWN:
	case TAVI_KEY_JOG1_DOWN:
	    level--;
		fChanged = true;
		break;
	case VK_UP:
	case TAVI_KEY_JOG1_UP:
	    level++;
		fChanged = true;
		break;
	case VK_F1: //EQ
	case VK_F2: // MENU
	case VK_ESC:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_ENTER:
	case TAVI_KEY_JOG2_UP:
	case TAVI_KEY_JOG2_DOWN:
	case TAVI_KEY_JOG2_ENTER:
	case TAVI_KEY_JOG1_ENTER:
		pBottom->ChangeTouchMenu( CPlayerBottomWnd::NORMAL );
		break;
	default:
		pBottom->ChangeTouchMenu( CPlayerBottomWnd::NORMAL );
		MSG_Send( m_nTaskID, MSG_BUTTON_UP, evt.value, 1 );
		break;
	}

	if( fChanged == false ) return;
	SetVolume( level );
}

void CVolProgressBar::OnPaint( const CRect& rc )
{
	// draw left volume icon
	op_BitBlt( &g_SysPixmap, rcABS.x1+4, rcABS.y1+26, 
		m_pVolMinus, 0, 0, m_pVolMinus->m_nWidth, m_pVolMinus->m_nHeight );
	// draw right volume icon
	op_BitBlt( &g_SysPixmap, rcABS.x2-4-m_pVolPlus->m_nWidth+2, rcABS.y1+26,
		m_pVolPlus, 0, 0, m_pVolPlus->m_nWidth, m_pVolPlus->m_nHeight );
	op_Flush();
}

void CVolProgressBar::Show( void )
{
	if( m_fVisible == false ) {
	    return;
    }

	// ¿ÜºÎ¿¡¼­ º¼·ýÁ¶Àý åÇßÀ» °æ¿ì¸¦ À§ÇØ¼­
	char buf[8];

	m_nVolLevel = g_SndCtrl->GetVolume();
	m_pProgressBar->SetPos( m_nVolLevel );
	sprintf( buf, "%d", m_nVolLevel );
	m_plbLevel->SetText( buf, false );
	
	CWindow::Show();
	if( m_fVisible ) {
		extern int handle_key;
		ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
		ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );
	}
}

void CVolProgressBar::Hide( void )
{
	CWindow::Hide();
	SetTaviKeyMode( 0 );
}

void CVolProgressBar::SetVolume( int level )
{
	extern SysParameter_t TaviSysParam;

	char buf[8];
	int p;
	bool show;
	CWindow* wnd = m_parent->GetParent();

    // Out of range
    if( level > m_nVolRange || level < 0 ) {
        return;
    }
	
	m_nVolLevel	= level;
	show		= wnd->IsShow();
	
	g_SndCtrl->SetVolume( m_nVolLevel );

	m_pProgressBar->SetPos( m_nVolLevel );
	m_pProgressBar->Update();

	sprintf( buf, "%d", m_nVolLevel );
	m_plbLevel->SetText( buf, show );

	if ( show ) Flush();

	if ( g_MiscCtrl->FolderClosed() ) {
	    DrawExtLCDVol( m_nVolLevel );
    }
}

void CVolProgressBar::DrawExtLCDVol( int vol )
{
	CPlayerBottomWnd* p = ( CPlayerBottomWnd* )m_parent;
	struct timeval* tv = &(p->m_tvExtLCDEQ );
	gettimeofday( tv, NULL );

	if( g_ExtLCD->GetCurrentMode() != CExtLCD::EXT_STAGE_VOLUME ) {
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_VOLUME );
		g_ExtLCD->Clear();
	}

	g_ExtLCD->UpdateVolume( vol );
	p->m_fExtLCDVolMode = true;
}
