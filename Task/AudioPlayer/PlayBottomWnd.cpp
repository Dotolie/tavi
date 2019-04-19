//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Bottom Window class definition
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/PlayBottomWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: PlayBottomWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.93  2006/02/14 03:19:10  zstein
// *** empty log message ***
//
// Revision 1.92  2006/01/27 06:46:29  zstein
// *** empty log message ***
//
// Revision 1.91  2006/01/26 06:01:41  zstein
// *** empty log message ***
//
// Revision 1.90  2006/01/20 04:48:24  zstein
// *** empty log message ***
//
// Revision 1.89  2005/12/26 04:14:56  zstein
// - g_ExtLCD->UpdateSeek( string, length ) to g_ExtLCD->UpdateSeek( total, current )
//
// Revision 1.88  2005/12/12 07:37:53  yong
// massive reworking abount sub LCD behaviour
//
// Revision 1.87  2005/12/11 11:38:23  yong
// Touch skip unit change: 3 --> 4 seconds. Very Fast!!!
//
// Revision 1.86  2005/12/11 11:20:23  yong
// adjust FF/FR time according to Touch speed value in Setup
// Each unit is 3 seconds now
//
// Revision 1.85  2005/12/01 01:04:08  zstein
// *** empty log message ***
//
// Revision 1.84  2005/11/28 04:45:37  zstein
// *** empty log message ***
//
// Revision 1.83  2005/11/28 02:36:27  zstein
// *** empty log message ***
//
// Revision 1.82  2005/11/28 00:50:18  zstein
// *** empty log message ***
//
// Revision 1.81  2005/11/26 03:18:26  zstein
// *** empty log message ***
//
// Revision 1.80  2005/11/25 10:21:58  zstein
// *** empty log message ***
//
// Revision 1.79  2005/11/24 03:02:42  zstein
// *** empty log message ***
//
// Revision 1.78  2005/11/17 04:24:50  zstein
// *** empty log message ***
//
// Revision 1.77  2005/11/16 09:46:38  zstein
// *** empty log message ***
//
// Revision 1.76  2005/11/11 12:28:47  zstein
// *** empty log message ***
//
// Revision 1.75  2005/11/10 01:09:39  zstein
// *** empty log message ***
//
// Revision 1.74  2005/11/08 07:26:15  zstein
// *** empty log message ***
//
// Revision 1.73  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
//
//******************************************************************************

#include <sys/time.h>
#include <unistd.h>

#include <tavi_lib.h>
#include "PlayBottomWnd.h"
#include "AudioPlayer.h"
#include <Task/ExtLCD.h>
#include "PlayerWnd.h"
#include <3Dsound/3Dsound.h>

// Macros
#define SCROLL_OFFSET		20

// Extern Variables
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CTextEngine_BDF* pT;
extern CExtLCD* g_ExtLCD;

// Local Variables
static bool s_fSeeking;
static unsigned int s_nSeekAccel;
static int s_nAmoutOfAccel = 10;		// XXX
static int s_nSkipTimeByTouch	= -1;
static int s_nOldValue;
static int s_nCount;
static char* s_szEQs[] = {
	"NORMAL",	
	"T-3D MILD",	
	"T-3D WIDE",	
	"T-3D LIVE",	
	"T-3D SPKR",	
	"R-SND BASS",
	"R-SND LITE",
	"R-SND PRO",	

	"ACOUSTIC",
	"BRIGHT",
	"CLASSIC",	
	"DANCE",	
	"DEEPWARM",	
	"ELECTRONIC",
	"FLAT",
	"FUNK",
	"HARDROCK",	
	"HIPHOP",
	"INSTRMNT",	
	"JAZZ",
	"LATIN",
	"LECTURE",
	"POP",
	"R&B",
	"SLOW",
	"SOFTROCK",	
	"VOICE",	
	"XBASS",	

	"MY T-3D WIDE",
	"MY T-3D LIVE",
	"MY R-SND",	
	"MY EQ"
};

static int s_EQ;
static bool s_touchevent;
static int s_touchtime;

static void SecToFormat( int sec, char* buf )
{
	int h = sec/3600;
	int m = (sec-h*3600)/60;
	int s = sec%60;

	sprintf( buf, "%02d:%02d:%02d", h, m, s );
}

#define BOUNDARY_ALL 			0
#define BOUNDARY_CATEGORY		1
#define BOUNDARY_ONE			2

CPlayerBottomWnd::CPlayerBottomWnd( CWindow* parent,  int nAB, int nRepeat, int nShuffle ) :
	CBottomWnd( parent, CBottomWnd::BW_NORMAL ),
	m_nAB( nAB )
{
	//HideLeftButton();
	//HideRightButton();
	
	int iw, ih;
	char* pImage;
	
	iw = TAVI_IMAGES[ICON_REPEAT].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[ICON_REPEAT].bitmapinfoheader.biHeight/REPEAT_ICON_COUNT;

	// Shuffle icon
	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + 3*iw*ih;
	m_puzShuffle = new CPixmap;
	m_puzShuffle->CreatePixmap( iw, ih, 8, pImage, false );
	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + 14*iw*ih;
	m_puzUnShuffle = new CPixmap;
	m_puzUnShuffle->CreatePixmap( iw, ih, 8, pImage, false );

	// Repeat icon
	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + iw*ih;
	m_puzRepeat = new CPixmap;
	m_puzRepeat->CreatePixmap( iw, ih, 8, pImage, false );
	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + 13*iw*ih;
	m_puzUnRepeat = new CPixmap;
	m_puzUnRepeat->CreatePixmap( iw, ih, 8, pImage, false );

	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + 8*iw*ih;
	m_puzBoundary[BOUNDARY_ALL] = new CPixmap;
	m_puzBoundary[BOUNDARY_ALL]->CreatePixmap( iw, ih, 8, pImage, false );

	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + 7*iw*ih;
	m_puzBoundary[BOUNDARY_CATEGORY] = new CPixmap;
	m_puzBoundary[BOUNDARY_CATEGORY]->CreatePixmap( iw, ih, 8, pImage, false );

	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + 9*iw*ih;
	m_puzBoundary[BOUNDARY_ONE] = new CPixmap;
	m_puzBoundary[BOUNDARY_ONE]->CreatePixmap( iw, ih, 8, pImage, false );

	// A-> icon
	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + 5*iw*ih;
	m_puzA = new CPixmap;
	m_puzA->CreatePixmap( iw, ih, 8, pImage, false );

	// A<->B icon
	pImage = ( char* )TAVI_IMAGES[ICON_REPEAT].image_buffer + 6*iw*ih;
	m_puzAB = new CPixmap;
	m_puzAB->CreatePixmap( iw, ih, 8, pImage, false );

	// EQ Icons
	iw = TAVI_IMAGES[ICON_EQ].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[ICON_EQ].bitmapinfoheader.biHeight/EQ_ICON_COUNT;
	pImage = ( char* )TAVI_IMAGES[ICON_EQ].image_buffer;
	int i;
	for( i=0; i<EQ_ICON_COUNT; i++ ) {
		m_puzEQ[i] = new CPixmap;
		m_puzEQ[i]->CreatePixmap( iw, ih, 8, pImage+i*iw*ih, false );
	}

	// Play state ICON
	iw = TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS].bitmapinfoheader.biHeight/5;
	for( i=0; i<4; i++ ) {
		m_pmPlayState[i] = new CPixmap;
		m_pmPlayState[i]->CreatePixmap(
			iw, ih, 8,
			(char*)TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS].image_buffer+i*iw*ih,
			false
			);
	}
	
	// Create progressbar
	m_puzProgressBar = new CProgressBar( this );
	m_puzProgressBar->ShowBall( true );
	m_puzProgressBar->Resize( CRect(32, 20, 287, 25), false );
	m_puzProgressBar->SetRange( 0, 0 );
	m_puzProgressBar->SetPos( 0 );

	m_pVolCtrl = new CVolProgressBar( this );
	m_pVolCtrl->Resize( CRect(28, 2, 291, 37), false );
	m_pVolCtrl->SetVisible( false );

	m_nTouchMenu = NORMAL;
	m_pTouchWnd = NULL;
	m_nAB = 0;
	m_fExtLCDEQMode	= false;
	m_fExtLCDVolMode = false;
}

CPlayerBottomWnd::~CPlayerBottomWnd( void )
{
	//delete m_puzBlank;
	if( m_puzShuffle ) delete m_puzShuffle;
	if( m_puzUnShuffle ) delete m_puzUnShuffle;
	if( m_puzRepeat ) delete m_puzRepeat;
	if( m_puzUnRepeat ) delete m_puzUnRepeat;

	int i;	
	for( i=0; i<EQ_ICON_COUNT; i++ ) delete m_puzEQ[i];
	for( i = 0; i<3; i++ ) delete m_puzBoundary[i];
 }

void CPlayerBottomWnd::OnPaint( const CRect& rc )
{
	CBottomWnd::OnPaint( rcClient );
	if( m_fVisible == false ) return;
	if( m_nTouchMenu != VOLUME ) {
		UpdatePlayState( m_nPlayMode, m_fVisible );
	  	// display AB key
		UpdateAB( m_nAB, false );
		// display repeat
		UpdateRepeat( false );
		// Draw shuffle
		UpdateShuffle( false );
		// Draw EQ
		UpdateEQ( false );
		UpdateBoundary( false );
		UpdateCurrentTime( m_nCurrentTime, true );
		UpdateTotalTime( m_nTotalTime, true );
	}
	Flush();
}

void CPlayerBottomWnd::OnMouseEvent( const MouseEvent& evt )
{
	CEvent event;
	struct timeval tv;	
	CPlayerWnd* pPlayer = ( CPlayerWnd* )m_parent;

	if (s_nSkipTimeByTouch == -1) {
		// XXX
		// 0: Very Fast
		// 1: Fast
		int speedIdx = (int)TaviSysParam.etc.touchspeed;
		s_nSkipTimeByTouch = (4 - speedIdx) * 4;	// seconds
	}

	gettimeofday( &tv, NULL );
	m_nTouchSec = tv.tv_sec;
	
	if( IsTouchMenu() ) {
		event.type = EVENT_MOUSE;
		event.u.mouse = evt;
		if( m_pTouchWnd ) m_pTouchWnd->SendEvent( event );
		return;
	}
	
	s_touchtime = tv.tv_sec;
	if( evt.click ) {
		ChangeTouchMenu( VOLUME );
		return;
	}
	

	if( !m_fSeek && evt.x ) { // if evt.x is 0 it is start/end mark
		s_touchevent = true;
		m_fSeek = true;
		if( m_fSeek == false ) {	/* XXX ? */
			m_nTouchMenu = SEEK;
			m_fSeek = true;
		}
		m_nTouchSec = tv.tv_sec;

		return;
	}

	if( m_fSeek ) {
		if( evt.x == 0 ) {
			DoSeek();
			s_touchevent = 0;
			return;
		}

		if( evt.x > 0 ) {
			m_nCurrentTime += s_nSkipTimeByTouch;
			m_seekdir = 1;
		}
		else if( evt.x < 0 ) {
			m_nCurrentTime -= s_nSkipTimeByTouch;
			m_seekdir  = 0;
		}

		// out of range
		if( m_nCurrentTime >= (m_nTotalTime - 5) ) {
			m_nCurrentTime = m_nTotalTime - 5;
		}
		if( m_nCurrentTime < 0 ) {
			m_nCurrentTime = 0;
		}

		UpdateCurrentTime( m_nCurrentTime, true );

		// Seek direction
		if( m_seekdir ) {
			UpdatePlayState( PBW_FF, true );
		} else {
			UpdatePlayState( PBW_REW, true );
		}
	}
}

void CPlayerBottomWnd::OnKeypressed( const KeyEvent& kevt )
{
	struct timeval tv;
	CPlayerWnd* pPlayer = ( CPlayerWnd* )m_parent;
	CEvent event;

	gettimeofday( &tv, NULL );
	m_nTouchSec = tv.tv_sec;

	if( m_fSeek ) {
		DoSeek();
		return;
	}
	
	if( g_MiscCtrl->FolderClosed() && m_fExtLCDEQMode ) {
		gettimeofday( &m_tvExtLCDEQ, NULL );
	}
	
	if( IsTouchMenu() ) {
		gettimeofday( &tv, NULL );
		m_nTouchSec = tv.tv_sec;
		event.type = EVENT_KEY;
		event.u.key = kevt;
		SendEvent( event, m_pTouchWnd );
		return;
	}

	switch( kevt.value ) {

	case TAVI_KEY_JOG1_UP:
		if( g_MiscCtrl->FolderClosed() && m_fExtLCDEQMode ) {
			s_EQ = (++s_EQ) % SOUND_SEQ_MAXCOUNT;

			pPlayer->SetEQ( s_EQ );

			CExtLCD::ExtLCDListItem item;
			item.m_nIcon = CExtLCD::EXT_LCD_ICON_MUSIC;
			item.m_nIndex = s_EQ;
			item.m_nTotal = sizeof(s_szEQs)/sizeof(char*);
			item.m_szText = s_szEQs[s_EQ];
			g_ExtLCD->SetListItem( item );
			g_ExtLCD->Update();
		} else {
			ChangeTouchMenu( VOLUME );
		}
		break;

	case TAVI_KEY_JOG1_DOWN:
		if( g_MiscCtrl->FolderClosed() && m_fExtLCDEQMode ) {
			s_EQ--;
			if( s_EQ < 0 ) 
				s_EQ = SOUND_SEQ_MAXCOUNT - 1;
			pPlayer->SetEQ( s_EQ );
			CExtLCD::ExtLCDListItem item;
			item.m_nIcon = CExtLCD::EXT_LCD_ICON_MUSIC;
			item.m_nIndex = s_EQ;
			item.m_nTotal = sizeof(s_szEQs)/sizeof(char*);
			item.m_szText = s_szEQs[s_EQ];
			g_ExtLCD->SetListItem( item );
			g_ExtLCD->Update();
		} else {
			ChangeTouchMenu( VOLUME );
		}
		break;

	case VK_UP:
	case TAVI_KEY_LUP:
		if( g_MiscCtrl->FolderClosed() ) break;

	case VK_DOWN:
	case TAVI_KEY_LDOWN:
		if( g_MiscCtrl->FolderClosed() ) break;
		if( s_fSeeking ) break;
		ChangeTouchMenu( VOLUME );
		// Volume down
		break;

	case TAVI_KEY_JOG1_LUP:
	case TAVI_KEY_JOG1_LDOWN:
		if( s_fSeeking ) break;
		ChangeTouchMenu( VOLUME );
		break;

	case TAVI_KEY_JOG1_LENTER:
		if( pPlayer->m_eq_enable == false ) {
			/* in case, EQ itself is NOT possible! ex) WMA, OGG now */
			break;
		}

		/*
			XXX External EQ Adjustment IS Disabeld!!!
			Too complex :-(
		*/
		break;

#if 0
		if( g_MiscCtrl->FolderOpen() ) {
			break;
		} else {
			if( m_fExtLCDEQMode == false ) {
				CExtLCD::ExtLCDListItem item;
				item.m_nIcon = CExtLCD::EXT_LCD_ICON_MUSIC;
				item.m_nIndex = TaviSysParam.sound.eq;
				item.m_nTotal = sizeof(s_szEQs)/sizeof(char*);
				item.m_szText = s_szEQs[TaviSysParam.sound.eq];
				g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
				g_ExtLCD->SetListItem( item );
				g_ExtLCD->Update();
				m_fExtLCDEQMode = true;		// This is the only place m_fExtLCDEQMode set to TRUE!!!
				gettimeofday( &m_tvExtLCDEQ, NULL );
				s_EQ = TaviSysParam.sound.eq;
			}
		}
		break;
#endif

	case TAVI_KEY_LLEFT:
	case TAVI_KEY_JOG2_LDOWN:
		m_seekdir = 0;
		SetSeekMode();
		break;

	case TAVI_KEY_LRIGHT:
	case TAVI_KEY_JOG2_LUP:
		m_seekdir = 1;
		SetSeekMode();
		break;

	case TAVI_KEY_JOG2_UP:
		pPlayer->Next();
		break;

	case TAVI_KEY_JOG2_DOWN:
		pPlayer->Prev();
		break;

	case TAVI_KEY_JOG2_ENTER:
		if( m_fExtLCDEQMode ) {

			TaviSysParam.sound.eq = s_EQ;
			pPlayer->SetEQ();
			UpdateEQ( true );
			m_fExtLCDEQMode = false;
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_PLAY );
			g_ExtLCD->Update();

		} else {

			if( pPlayer->GetPlayState() == AUDIO_STATE_PLAYING ) {
				pPlayer->Pause();
			} else {
				pPlayer->Resume();
			}

		}
		break;
	}
}

void CPlayerBottomWnd::OnWindowEvent( const WindowEvent& evt )
{
	struct timeval tv;
	
	switch( evt.nMessage ) {

	case WINDOW_IDLE:
		gettimeofday( &tv, NULL );

		if( m_fExtLCDEQMode || m_fExtLCDVolMode ) {
			/*
				Automatic conversion into normal mode from EQ, Vol mode
			*/
			if( abs(tv.tv_sec-m_tvExtLCDEQ.tv_sec) > AUTO_WND_SWITCH_TIME ) {
				m_fExtLCDEQMode		= false;
				m_fExtLCDVolMode	= false;

				g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_PLAY );
				g_ExtLCD->Update();
			}
		}
		
		if( !m_fSeek ) {
			if( m_nTouchMenu == NORMAL ) {
				return;
			}
			if( abs(tv.tv_sec-m_nTouchSec) > TOUCH_MENU_HIDE_TIME ) {
				ChangeTouchMenu( NORMAL );
			}
		} else {
			if( s_touchevent ) {
				if( abs(tv.tv_sec - s_touchtime) > 1 ) {
					DoSeek();
					s_touchevent = 0;
				}
				return;
			}
			
			s_nCount++;
			if( m_seekdir == 1 ) { // FF
				if( m_nCurrentTime >= m_nTotalTime ) return;
				if( s_nCount%3 ) return;
				s_nSeekAccel += s_nAmoutOfAccel;
				m_nCurrentTime += (int)((float)s_nSeekAccel*0.1);
				if( m_nCurrentTime > m_nTotalTime ) m_nCurrentTime = m_nTotalTime;
			}
			else if( m_seekdir == 0 ) { // Rew
				if( m_nCurrentTime == 0 ) return;
				if( s_nCount%3 ) return;
				s_nSeekAccel += s_nAmoutOfAccel;
				m_nCurrentTime -= (int)((float)s_nSeekAccel*0.1);
				if( m_nCurrentTime < 0 ) m_nCurrentTime = 1;
			} else {
				return;
			}

			if( s_nOldValue == m_nCurrentTime ) return;

			s_nOldValue = m_nCurrentTime;

			UpdateCurrentTime( m_nCurrentTime, true );

			if( g_MiscCtrl->FolderClosed() ) {
				char buf1[20], buf3[50];
				int len;
				SecToFormat( m_nCurrentTime, buf1 );
				len = sprintf( buf3, "%s", buf1 );

				if( g_ExtLCD->GetCurrentMode() != CExtLCD::EXT_STAGE_SEEK ) {
					g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_SEEK );
					g_ExtLCD->Clear();
					m_fExtLCDVolMode = true;
				}

				g_ExtLCD->UpdateSeek( m_nTotalTime, m_nCurrentTime );
				//g_ExtLCD->UpdateSeek( buf3, len );
				m_tvExtLCDEQ = tv;
			}
		}

		break;
	}
}

void CPlayerBottomWnd::UpdateEQ( bool fFlush )
{
	int eq = TaviSysParam.sound.eq;

	if( m_fVisible ) {
		op_BitBlt( &g_SysPixmap, 
			rcABS.x2-33-m_puzEQ[0]->m_nWidth-3*(m_puzUnShuffle->m_nWidth+2), rcABS.y1+3, 
			m_puzEQ[eq], 0, 0, m_puzEQ[eq]->m_nWidth, m_puzEQ[eq]->m_nHeight );

		Flush();
	}
}

void CPlayerBottomWnd::UpdateBoundary( bool fFlush )
{
	int b = TaviSysParam.music.boundary;
	op_BitBlt( &g_SysPixmap,
		rcABS.x2-33-3*(m_puzUnRepeat->m_nWidth+2)+2, rcABS.y1+3,
		m_puzBoundary[b], 0, 0,
		m_puzBoundary[b]->m_nWidth, 
		m_puzBoundary[b]->m_nHeight );
	if( fFlush ) Flush();
}

void CPlayerBottomWnd::UpdateShuffle( bool fFlush )
{
	CPixmap* pixmap = NULL;
	if( TaviSysParam.music.shuffle ) pixmap = m_puzShuffle;
	else pixmap = m_puzUnShuffle;
	if( pixmap ) {
		op_BitBlt( &g_SysPixmap, 
			rcABS.x2-33-pixmap->m_nWidth, rcABS.y1+3, 
			pixmap, 0, 0, pixmap->m_nWidth, pixmap->m_nHeight );
	}
	if( fFlush ) Flush();
}

void CPlayerBottomWnd::UpdateRepeat( bool fFlush )
{
	CPixmap* pixmap = NULL;
	if( m_nAB ) return;
	
	if( TaviSysParam.music.repeat ) pixmap = m_puzRepeat;
	else pixmap = m_puzUnRepeat;

	if( pixmap ) {
		op_BitBlt( &g_SysPixmap, 
			rcABS.x2-33-2*pixmap->m_nWidth-2, rcABS.y1+3,
			pixmap, 0, 0, pixmap->m_nWidth, pixmap->m_nHeight );
	}
	if( m_fVisible ) Flush();
}

void CPlayerBottomWnd::UpdateAB( int mode, bool fFlush )
{
	CPixmap* pixmap = NULL;

	m_nAB = mode;
	if( m_nAB == AB_A ) pixmap = m_puzA;
	else if( m_nAB == AB_AB ) pixmap = m_puzAB;
	else return;
	op_BitBlt( &g_SysPixmap, 
		rcABS.x2-33-2*pixmap->m_nWidth-2, rcABS.y1+3,
		//rcABS.x2-33-m_puzEQ[0]->m_nWidth-2*(pixmap->m_nWidth+2), rcABS.y1+3,
		pixmap, 0, 0, pixmap->m_nWidth, pixmap->m_nHeight );
	if( m_fVisible ) Flush();
}

void CPlayerBottomWnd::ChangeTouchMenu( int mode, bool fUpdate )
{
	if( mode < NORMAL ) return;
	if( mode > VOLUME ) return;

	if( fUpdate == false ) {
		m_nTouchMenu = mode;
		return;
	}
	
	if( m_pVolCtrl->IsShow() ) {
		m_pVolCtrl->SetVisible( false );
		m_pVolCtrl->Hide();
	}

	m_nTouchMenu = mode;

	if( m_nTouchMenu == NORMAL ) {
		extern int handle_key;
		ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 0 );
		ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 0 );
		Show();
	} else if( m_nTouchMenu == VOLUME ) {
		s_fSeeking = false;
		//SetRightButton( CBottomWnd::RB_PLAY );
		OnPaint( rcClient );
		m_pVolCtrl->SetVisible( true );
		m_pVolCtrl->Show();
		m_pTouchWnd = m_pVolCtrl;

		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_VOLUME );
	}

	if( (m_nTouchMenu != NORMAL) || s_fSeeking ) {
		struct timeval tv;
		gettimeofday( &tv, NULL );
		m_nTouchSec = tv.tv_sec;
	}

	mep_eat_event();
}

void CPlayerBottomWnd::UpdateCurrentTime( int sec, bool fUpdate )
{
	char buf[20] = { 0, };

	m_nCurrentTime = sec;
        m_puzProgressBar->SetPos( sec );
	if( m_nTouchMenu == VOLUME || m_nTouchMenu == TOOL )return;
	if( m_parent->IsShow() == false ) return;

	SecToFormat( sec, buf );
	if( m_fVisible ) DrawASCIIText( buf, 8, 7, &TAVI_IMAGES[FONT_DIGIT3], 32, 229, __DIGIT3);
	//m_puzProgressBar->SetPos( sec );
	if( m_fVisible ) m_puzProgressBar->Update();
}

void CPlayerBottomWnd::UpdateTotalTime( int sec, bool fUpdate )
{
	char buf[20] = {0,};

	m_puzProgressBar->SetRange( 0, sec );
	m_nTotalTime = sec;
	if( IsTouchMenu() ) return;
	SecToFormat( sec, buf );
	if( m_fVisible ) {
            g_SysPixmap.Fill( 250-14, 229, 80, 7, 24 );
            DrawASCIIText( buf, 8, 7, &TAVI_IMAGES[FONT_DIGIT3], 250-14, 229, __DIGIT3 );
       }
}

void CPlayerBottomWnd::SetVolume( int level )
{
	m_pVolCtrl->SetVolume( level );
}

void CPlayerBottomWnd::UpdatePlayState( int state, bool fUpdate )
{
	if( m_pVolCtrl->IsShow() ) return;

	CPixmap* pixmap;
	int rid;

	switch( state ) {

	case PBW_PLAY:
		rid = ML_PLAY;
		break;
	case PBW_PAUSE:
		rid = ML_PAUSE;
		break;
	case PBW_REW:
		rid = ML_REWIND;
		break;
	case PBW_FF:
		rid = ML_FF;
		break;
	}

	if( state < PBW_PLAY ) return;
	if( state > PBW_REW ) return;
	m_nPlayMode = state;
	pixmap = m_pmPlayState[state];

	if( m_fVisible ) {
		g_SysPixmap.Fill( rcABS.x1+33+m_pmPlayState[0]->m_nWidth+2, rcABS.y1+3,
			60, 12, 24 );
		op_BitBlt( &g_SysPixmap, 
			rcABS.x1+33, rcABS.y1+3, 
			pixmap, 0, 0, pixmap->m_nWidth, pixmap->m_nHeight );
	
		pT->DrawResText( rid, 
			rcABS.x1+33+2+m_pmPlayState[0]->m_nWidth, rcABS.y1+3,
			_WHITE, 12, DT_TRANSPARENCY );
	}

	// Draw Sub LCD
	switch( state ) {
	case PBW_PLAY: g_ExtLCD->UpdatePlayState( CExtLCD::EXT_LCD_PLAY ); break;
#if 0
	case PBW_REW: g_ExtLCD->UpdatePlayState( CExtLCD::EXT_LCD_REW ); break;
	case PBW_FF: g_ExtLCD->UpdatePlayState( CExtLCD::EXT_LCD_FF ); break;
#endif
	case PBW_PAUSE: g_ExtLCD->UpdatePlayState( CExtLCD::EXT_LCD_PAUSE ); break;
	}

	if( 1 || g_MiscCtrl->FolderClosed() ) g_ExtLCD->Flush();
}

bool CPlayerBottomWnd::IsSeeking( void )
{
	return m_fSeek;
}

void CPlayerBottomWnd::SetSeekMode( void )
{
	if( m_nCurrentTime == 0 ) return;
	
	//struct timeval tv;
	gettimeofday( &m_tvExtLCDEQ, NULL );
	//m_nTouchSec = m_tvExtLCDEQ.tv_sec;
	//s_fSeeking = true;
	m_fSeek = true;
	//s_fSeekingDir = dir;
	s_nSeekAccel = 0;
	if( m_seekdir ) UpdatePlayState( PBW_FF, true );
	else UpdatePlayState( PBW_REW, true );

	//m_nTouchMenu = SEEK;
}

#if 0
void CPlayerBottomWnd::Seek10Sec( int dir )
{
	CPlayerWnd* pPlayer = ( CPlayerWnd* )m_parent;
	
	if( dir ) m_nCurrentTime += 10;
	else m_nCurrentTime -= 10;
	
	if( m_nCurrentTime >= m_nTotalTime ) {
		pPlayer->Next();
		return;
	}
	else if( m_nCurrentTime < 0 ) {
		pPlayer->Prev();
		return;
	}
	else {
		UpdatePlayState( PBW_REW-dir, true );
		UpdateCurrentTime( m_nCurrentTime, true );
		pPlayer->Seek( m_nCurrentTime, 1 );
		UpdatePlayState( PBW_PLAY, true );
	}
}
#endif

bool CPlayerBottomWnd::IsTouchMenu( void )
{
	if( m_nTouchMenu == NORMAL || m_nTouchMenu == SEEK ) {
		return false;
	} else {
		return true;
	}
}

void CPlayerBottomWnd::ResetProgressBar( void )
{
	if( m_parent->IsShow() == false ) return;
	m_puzProgressBar->Update();
	//s_fSeeking = false;
	m_fSeek = false;
	m_nCurrentTime = 0;
}

void CPlayerBottomWnd::OnKeyReleased( const KeyEvent& evt )
{
	switch( evt.value ) {
	case TAVI_KEY_LLEFT:
	case TAVI_KEY_JOG2_LDOWN:
		//DoSeek( 0 );
		//break;
	case TAVI_KEY_LRIGHT:
	case TAVI_KEY_JOG2_LUP:
		DoSeek();
		break;
	}
}

void CPlayerBottomWnd::DoSeek( void )
{
	if( m_fSeek == false ) return;
	
	CPlayerWnd* pPlayer = ( CPlayerWnd* )m_parent;
	int mode;

	if( pPlayer->m_state == AUDIO_STATE_PLAYING ) mode = PBW_PLAY;
	else if( pPlayer->m_state == AUDIO_STATE_PAUSE ) mode = PBW_PAUSE;

	pPlayer->Seek( m_nCurrentTime, m_seekdir );

	//s_fSeeking = false;
	m_fSeek = false;
	UpdatePlayState( mode, true );
}

void CPlayerBottomWnd::Hide( void )
{
	CWindow::Hide();
	#if 0
	if( s_fSeeking ) {
		DoSeek( m_seekdir );
		s_fSeeking = false;
		m_fSeek = false;
	}
	#endif
	if( m_fSeek ) {
		m_fSeek = false;
		DoSeek();
	}
}

void CPlayerBottomWnd::CancelSeek( void )
{
	s_fSeeking = false;
	m_fSeek = false;
}

/* FIN */
