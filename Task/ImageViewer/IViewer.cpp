//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IViewer.cpp,v $
// $Revision: 1.6 $
// $Date: 2006/06/22 01:56:20 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IViewer.cpp,v $
// Revision 1.6  2006/06/22 01:56:20  zstein
// - changed MACRO name.
//
// Revision 1.5  2006/06/22 01:37:12  zstein
// - added 'delete playlist' menu.
//
// Revision 1.4  2006/05/22 03:00:04  zstein
// - removing a noise on playing a music file.
//
// Revision 1.3  2006/05/10 02:34:40  zstein
// - fixed unwakeup-hdd bug.
//
// Revision 1.2  2006/03/13 02:33:55  zstein
// - repeat/single event mode in Image viewer.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.49  2006/02/10 06:27:32  zstein
// - fixed system down error after changing screen resolution.
//
// Revision 1.48  2006/01/24 00:54:08  zstein
// *** empty log message ***
//
// Revision 1.47  2006/01/19 05:26:33  zstein
// *** empty log message ***
//
// Revision 1.46  2005/12/28 07:50:31  zstein
// - fixed ui bug
//
// Revision 1.45  2005/12/01 01:03:57  zstein
// *** empty log message ***
//
// Revision 1.44  2005/11/28 00:50:46  zstein
// *** empty log message ***
//
// Revision 1.43  2005/11/18 09:16:12  zstein
// *** empty log message ***
//
// Revision 1.42  2005/11/11 12:29:00  zstein
// *** empty log message ***
//
// Revision 1.41  2005/11/10 01:09:23  zstein
// *** empty log message ***
//
// Revision 1.40  2005/11/08 01:18:53  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <unistd.h>

#include "../Msg.h"
#include "IViewer.h"
#include "../../linuxfs.h"
#include "../../jpeg/jpeg.h"
#include <Component/FileUtil.h>
#include <Component/image.h>
#include <Component/Bitmap.h>
#include <Task/TopPanel.h>
#include <Task/AudioPlayer/AudioPlayer.h>
#include "IVInfoWnd.h"
#include "ImageViewer.h"
#include <Lib/LibImage.h>
#include <Lib/LibMsgBox.h> // for MessageBox

// Macros
#define TIMEOUT						5000 // 5sec
#define BOTTOM_HIDE_TIMEOUT		7000 //sec

// Extern Variables
extern RMuint32* thumbnailY;
extern RMuint32* thumbnailUV;
extern RMuint32* backgroundY;
extern RMuint32* backgroundUV;
extern RMuint32* scaled_backgroundY;
extern RMuint32* scaled_backgroundUV;
extern int jpeg_real_width;
extern int jpeg_real_height;
extern CTopPanel* g_TopPanel;
extern void showbackground( int nMenuID );

// Local variables
static char* pY;
static char* pUV;
CWindow* pSetupWnd;
CWindow* pInfoWnd;
static int nSlideshowTime[4] = {5, 10, 20, 30};

CIViewer::CIViewer( CWindow* parent, CIVListView* pview/*const CString& szDir, CList< CListItem >* pList, int start */) :
	CWindow( parent ),
	m_plist( pview )
{
	op_ClearAreaOSD( 0, 30, 320, 160 );

	m_offset = 0; // # of folders
	CListItem* p;
	for( int i=0; i<m_plist->GetCount(); i++ ) {
		p = m_plist->GetItem( i );
		if( p->m_nType == NODE_TYPE_FILE ) break;
		m_offset++;
	}
	m_nCurrent = m_plist->GetSelectedIndex()-m_offset;
	
	m_pBottomWnd = new CIViewerBottomWnd( this );
	m_pBottomWnd->Resize( CRect(0, 169, 319, 209), false );
	//m_pBottomWnd->HideLeftButton();
	if( TaviSysParam.photo.info_display == ALWAYS_HIDE ) {
		m_pBottomWnd->SetVisible( false );
		g_TopPanel->Hide();
		g_TopPanel->SetVisible( false );
	}
		
	m_nPlayMode = PAUSE;
	m_nTimerID = -1;

	m_pRotatedY = NULL;
	m_pRotatedUV = NULL;
	m_nRotate = 0;

	// No repeat, No single mode
	SetTaviKeyMode( 0 ); 
	mep_report( MEP_REPORT_ABSOLUTE );

	// Create ContextMenu
	m_pCtxMenuWnd = new CContextMenu( this );
	m_pCtxMenuWnd->SetVisible( false );
	m_pCtxMenuWnd->SetWindowList( IV_GetWndList() );

	SetWindowTitle( PHOTO_VIEWER );

	m_fUpdateImage = true;
	m_fAutoHide = false;
	m_pMsg = NULL;
}

CIViewer::~CIViewer( void )
{
	showbackground( ID_BG_PHOTO );

	g_TopPanel->SetVisible( true );
	g_TopPanel->Show();
	mep_report( MEP_REPORT_RELATIVE );

	if( m_pRotatedY ) delete[] m_pRotatedY;
	if( m_pRotatedUV ) delete[] m_pRotatedUV;

	if( m_pMsg ) delete m_pMsg;

    // Restore Screen Resolution
    pMpegDecoder->SetVideoDestination( 
		TaviSysParam.output[toggle_display].x,
		TaviSysParam.output[toggle_display].y,
		TaviSysParam.output[toggle_display].w,
		TaviSysParam.output[toggle_display].h );
}

void CIViewer::OnKeypressed( const KeyEvent& evt )
{
	TMsgEntry msg;
	CEvent event;
	event.type = EVENT_KEY;
	event.u.key= evt;

	if( m_pMsg ) {
		delete m_pMsg;
		m_pMsg = NULL;
		if( m_pBottomWnd->IsShow() ) m_pBottomWnd->Show();
		return;
	}
	
	gettimeofday( &m_tvPanel, NULL );
	
	ShowPanel();

	switch( evt.value ) {
	case VK_UP:
		RotateCCW90();
		break;
	case VK_DOWN:
		RotateCW90();
		break;
	case VK_LEFT:
		if( m_nPlayMode == PLAY ) break;
		PrevImage();
		break;
	case VK_RIGHT:
		if( m_nPlayMode == PLAY ) break;
		NextImage();
		break;
	case VK_F1: // EQ
		break;
	case VK_F2:
		if( m_nPlayMode == PLAY ) break;
		CreateContextMenu();
		m_pCtxMenuWnd->SetFocus();
		m_fAutoHide = false;
		m_fUpdateImage = false;
		mep_report( MEP_REPORT_RELATIVE );
		break;
	case VK_ESC:
		SendMessage( WINDOW_KILL );
		break;
	case VK_EXT_ESC:
		SendMessage( WINDOW_TASK_KILL_AND_SWITCH );
		break;
	case VK_ENTER:
		if( m_nPlayMode == PAUSE ) {
			m_nPlayMode = PLAY;
			Play();
		}
		else {
			m_nPlayMode = PAUSE;
			Pause();
		}
		return;
	case VK_F3:
		if( m_nPlayMode != PLAY ) {
			SendMessage( WINDOW_SET_BG );
			Pause();
		}
		break;
	case TAVI_KEY_LMODE:
		if( m_nPlayMode != PLAY ) {
			SendMessage( WINDOW_TASK_SWITCH );
		}
		break;
	}	
}

void CIViewer::OnWindowEvent( const WindowEvent& evt )
{
	if( evt.nWindowID == m_nWindowID ) {
		switch( evt.nMessage ) {
		}
	}
	else {
		CEvent event;
		event.type = EVENT_WINDOW;
		event.u.window = evt;
		SendEvent( event, m_pBottomWnd );
	}

	// idle
	struct timeval tvCur;
	gettimeofday( &tvCur, NULL );
	if( evt.nMessage == WINDOW_IDLE ) {
		// slideshow
		if( m_nPlayMode == PLAY ) {
			if( abs(tvCur.tv_sec-m_tvSlideshow.tv_sec) > 
			    nSlideshowTime[TaviSysParam.photo.slideshowtime] ) {
				// show next image
				if( TaviSysParam.photo.repeat == 0 ) {
					if( m_nPlayCount == m_plist->GetCount() ) {
						Pause();
						return;
					}
				}
				m_nPlayCount++;
				NextImage();
				gettimeofday( &m_tvSlideshow, NULL );
			}
		}
		
		if( m_pBottomWnd->IsShow() && m_fAutoHide ) {
			if( abs(tvCur.tv_sec-m_tvPanel.tv_sec) > TOUCH_MENU_HIDE_TIME ) {
				HidePanel();
			}
		}

		if( m_pMsg ) {
			if( abs(tvCur.tv_sec-m_tvMsg.tv_sec) > 3 ) {
				delete m_pMsg;
				m_pMsg = NULL;
				if( m_pBottomWnd->IsShow() ) m_pBottomWnd->Show();
			}
		}
	}
}

void CIViewer::OnMouseEvent( const MouseEvent& evt )
{
	gettimeofday( &m_tvPanel, NULL );
	ShowPanel();
	if( m_nPlayMode == PLAY ) return;

	if( !evt.click ) return;

	int pos  = evt.x;
	CIViewer* pViewer = ( CIViewer* )m_parent;

	if( pos == 1 ) PrevImage();
	else if( pos == 3 ) NextImage();
}

void CIViewer::OnPaint( const CRect& rc )
{
	if( m_fUpdateImage ) ShowImage();	
	m_fUpdateImage = true;
	ClearKeyBuffer();
}

void CIViewer::Play( void )
{
	m_nRotate = 0;
	m_nPlayMode = PLAY;
	m_pBottomWnd->Update();
	m_pBottomWnd->Flush();
	m_pBottomWnd->m_pProgressBar->SetPos( m_nCurrent );
	m_pBottomWnd->m_pProgressBar->Update();
	m_nPlayCount = 0;
	gettimeofday( &m_tvSlideshow, NULL );
}

void CIViewer::Pause( void )
{
	m_nPlayMode = PAUSE;
	m_pBottomWnd->Update();
	m_pBottomWnd->Flush();
	ShowPanel();
	
}

void CIViewer::PrevImage( void )
{
	int total = m_plist->GetCount()-m_offset;
	int idx;
	do {
		m_nCurrent--;
		if( m_nCurrent < 0 ) m_nCurrent = total -1;
		idx = m_nCurrent + m_offset;
		if( m_plist->GetItem(idx)->m_nType == NODE_TYPE_FILE ) 
			break;
	} while( 1 );
	ShowImage();
}

void CIViewer::NextImage( void )
{
	int total = m_plist->GetCount()-m_offset;
	int idx;
	do {
		m_nCurrent++;
		if( m_nCurrent >= total ) m_nCurrent = 0;
		idx = m_nCurrent + m_offset;
		if( m_plist->GetItem(idx)->m_nType == NODE_TYPE_FILE ) 
			break;
	} while( 1 );
	ShowImage();
}

void CIViewer::RotateCCW90( void )
{
    int nW, nH;
    PFN_Progress prg;

    if( m_nPlayMode == PLAY ) return;

    if( AP_HasPlayer() ) prg = AP_Wait;
    else prg = 0;

    m_nRotate--;
    if( m_nRotate < 0 ) m_nRotate = 3;
    if( m_nRotate == 0 || m_nRotate == 2 ) { nW = m_nImageWidth, nH = m_nImageHeight; }
    else { nW = m_nImageHeight, nH = m_nImageWidth; }
    RotateYUV420( (unsigned char*)pY, (unsigned char*)pUV,
    	m_nImageWidth, m_nImageHeight,
    	m_pRotatedY, m_pRotatedUV, nW, nH, m_nRotate, prg );

    DisplayYUV420( m_pRotatedY, m_pRotatedUV, nW, nH, nW, nH );
    m_pBottomWnd->UpdateRotate( m_nRotate );
    op_Flush();
}

void CIViewer::RotateCW90( void )
{
    int nW, nH;
    PFN_Progress prg;

    if( m_nPlayMode == PLAY ) return;

    if( AP_HasPlayer() ) prg = AP_Wait;
    else prg = 0;

    m_nRotate++;
    m_nRotate %= 4;
    if( m_nRotate == 0 || m_nRotate == 2 ) { nW = m_nImageWidth, nH = m_nImageHeight; }
    else { nW = m_nImageHeight, nH = m_nImageWidth; }
    
    RotateYUV420( (unsigned char*)pY, (unsigned char*)pUV, 
                            m_nImageWidth, m_nImageHeight,
                            m_pRotatedY, m_pRotatedUV, nW, nH, m_nRotate, prg );

    DisplayYUV420( m_pRotatedY, m_pRotatedUV, nW, nH, nW, nH );
    m_pBottomWnd->UpdateRotate( m_nRotate );
    op_Flush();
}

static void jpeg_decoding_progress( int p, void* arg ) {}
extern RMuint32 jpeg_fclose( RMuint32 handle, void* pContext );
extern RMuint32 jpeg_fopen( RMuint8* filename, void* pContext );
extern RMuint32 jpeg_fread ( RMuint32 handle, void *buf, RMuint32 length, void *context);

bool CIViewer::DisplayYUV420( RMuint8* py, RMuint8* puv, int iw, int ih, int ow, int oh )
{
    int x, y;
    RMint32 w, h, n, d;

    if( !py || !puv ) return false;
    
    //pMpegDecoder->GetScreenDimensions (&w, &h, &n, &d);
    w = 320;
    h = 240;

    x = 0;
    y = 0;
    float ratio;
    if( ow > w ) {
    	ratio = (float)ow/(float)w;
    	w = ow;
    	h = (int)((float)h*ratio);
    	h = (h+15)&~0x0f;
    }

    if( oh > h ) {
    	ratio = (float)oh/(float)h;
    	h = oh;
    	w = w*ratio;
    	w = (w+15)&~0x0f;
    }

    x = (w-iw)/2;
    x = (x+8) & ~0x0f;
    y = (h-ih)/2;
    y = (y+8) & ~0x0f;

   // pMpegDecoder->SetVideoDestination( 0, 0, w, h );
    pMpegDecoder->DisplayYUV420(0, 0, 0, 0, 0, 0, 0, 0 );
    sleep( 1 );
    pMpegDecoder->DisplayYUV420(py, puv, x, y, ow, oh, w, h );

    return true;
}

bool CIViewer::ShowJPEG( uchar_t* szPath )
{
    JPEG_CALLBACK_TABLE callback;
    static int r=0;
    int outW, outH;
    int mOutW, mOutH;
    int i, j;

    pY = ( char* )scaled_backgroundY;
    pUV = ( char* )scaled_backgroundUV;

    callback.context = 0;
    callback.fopen = (JPEG_fopen)jpeg_fopen;
    callback.fclose = (JPEG_fclose)jpeg_fclose;
    callback.fread = (JPEG_fread)jpeg_fread;
    if( AP_HasPlayer() ) callback.jpeg_progress = AP_Wait;
    else callback.jpeg_progress = jpeg_decoding_progress;

    r = decode_jpeg_image_420 ( (char*)szPath,
    		(RMuint8 *)pY, (RMuint8 *)pUV, 
    		852*480, (852*480)/2, &outW, &outH, &callback);


    // failed to decode JPEG
    if (r != 0) return false;

    mOutW = (outW + 15) & ~0xf;
    mOutH = (outH + 15) & ~0xf;

    m_nImageWidth = mOutW;
    m_nImageHeight = mOutH;

  
    // black out non-multiple of 16
    RMuint8 *p;
    p = (RMuint8 *)pY;
    for (j=0; j<mOutH; j++) {
    	for (i=0; i<mOutW; i++) {
    		if ((i >= outW) || (j >= outH))
    			*p = 0;
    		p++;
    	} 
    }
    p = (RMuint8 *)pUV;
    for (j=0; j<mOutH; j+=2)	{
    	for (i=0; i<mOutW; i+=2) {
    		if ((i >= outW) || (j >= outH)) {
    			*p = 0x80;
    			*(p+1) = 0x80;
    		}
    		p += 2;
    	}
    }

    return DisplayYUV420( 
                    (RMuint8*)pY, (RMuint8*)pUV, 
                    outW, outH,
                    mOutW, mOutH );
}

bool CIViewer::ShowBMP( uchar_t* szPath )
{
    char* szErrorMsg;
    PFN_Progress prg;

    if( AP_HasPlayer() ) prg = AP_Wait;
    else prg = 0;

    CBitmap bitmap;
    int error;
    int padding, outW, outH, i, j;
    int mOutW, mOutH;
    int size;
    char* p;
    BMPHEADER header;
    BMPINFOHEADER info;

    if( (error=bitmap.DecodeBMP( (char*)szPath )) ) goto __bmp_error;		
    header = bitmap.GetHeader();
    info = bitmap.GetInfoHeader();

    pY = (char*)backgroundY;
    pUV = (char*)backgroundY+BMP_MAX_SIZE;

    if( bitmap.GetYUV420Data( pY, pUV, &mOutW, &mOutH, prg )  ) goto __bmp_error;

    m_nImageWidth = mOutW;
    m_nImageHeight = mOutH;

    return DisplayYUV420( 
                    (RMuint8*)pY, (RMuint8*)pUV, 
                    mOutW, mOutH, 
                    mOutW, mOutH );
__bmp_error:
	return false;
}

void CIViewer::ShowImage( void )
{
	int media;
	int idx = m_nCurrent+m_offset;
	CFolderListItem* pItem = ( CFolderListItem* )m_plist->GetItem(idx);
	uchar_t path[MAX_PATH];
	uchar_t del[] = { '/', 0 };

	TAVI_WakeUp( TASK_ID_IMAGE );
	
	media = CheckMedia( (char*)pItem->m_szText, true );

	if( m_pRotatedY ) delete[] m_pRotatedY;
	if( m_pRotatedUV ) delete[] m_pRotatedUV;

	CString dir = m_plist->GetCurrentPath();
	ustrcpy( path, (uchar_t*)dir );
	ustrcat( path, del );
	ustrcat( path, (uchar_t*)pItem->m_szText );
	m_szPath = path;
	
	switch( media ) {
	case MEDIA_JPEG:
		m_success = ShowJPEG( path );
		break;
	case MEDIA_BMP:
		m_success = ShowBMP( path );
		break;
	case MEDIA_PNG:
		break;
	case MEDIA_TIFF:
		break;
	}
	m_nCurrentMedia = media;

	// Display Filename & Info.
	m_pBottomWnd->m_pProgressBar->SetPos( m_nCurrent );
	if( m_pBottomWnd->IsShow() ) {
		m_nRotate = 0;
		m_pBottomWnd->m_pProgressBar->Update();
		m_pBottomWnd->UpdateRotate( m_nRotate );
		m_pBottomWnd->Update();
		m_pBottomWnd->Flush();
	}

	m_pRotatedY = new unsigned char[m_nImageWidth*m_nImageHeight];
	m_pRotatedUV = new unsigned char[m_nImageWidth*m_nImageHeight/2];
	ClearKeyBuffer();

	if( m_success == false ) {
		// clear background
		pMpegDecoder->DisplayYUV420 (0, 0, 0, 0, 0, 0, 0, 0 );
		// show error message
		m_pMsg = new CMsgBox( 
			ML_INFORMATION, _ORANGE, ML_FAILED_TO_PLAY, 244, 100, 0, DL_BG_ICON );
		m_pMsg->Show();
		gettimeofday( &m_tvMsg, NULL );
	}
}

static int OnSetBackground( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	
	CString szPath;
	CListItem* pItem;
	extern RMuint32 *backgroundY;
	extern RMuint32* backgroundUV;	
	unsigned char* pBGY=NULL, *pBGUV=NULL;
	CMsgBox* pMsgBox;
	PFN_Progress prg=0;
	extern int handle_key;
	int speed;

  TAVI_WakeUp( 100 );

	// CHECK LOW BATTERY
	if( g_TopPanel->GetBatteryLevel() == CTopPanel::BATTERY_LEVEL_0 ) {
		// SHOW MESSAGE
		pMsgBox =  new CMsgBox( 
			ML_INFORMATION, 
			_ORANGE, ML_LOW_BETTRY, 
			244, 123, 0, DL_BG_ICON );
		pMsgBox->Show();
		for( int i=0; i<100; i++ ) usleep( 10000 );
		delete pMsgBox;
		return -1;
	}

  TAVI_BG_PlayPause();
  
	// SPEED DOWN KEY SAMPLINGRATE
	speed = 1; /*slowest*/
	if( ioctl( handle_key, TAVI_KEY_IOCS_SPEED, &speed ) < 0 ) {
		perror( "ioctl(TAVI_KEY_IOCS_SPEED)\n" );
	}
	
	// SHOW MESSAGE
	pMsgBox =  new CMsgBox( 
		ML_INFORMATION, 
		_ORANGE, ML_MSG_PLZ_WAIT, 
		244, 123, 0, DL_BG_ICON );
	pMsgBox->Show();

	szPath = pViewer->GetCurrentPath();
	int len = szPath.GetSize();
	
	// Load Image
	int outW, outH;
	unsigned char* pY = ( unsigned char* )backgroundY;
	unsigned char* pUV = pY + BMP_MAX_SIZE;
	if( LoadImageYUV420( (char*)szPath, pY, pUV, 
		BACKGROUND_YLENGTH, BACKGROUND_YLENGTH/2,
		&outW, &outH, prg ) ) {
		printf( "failed to load image\n" );
		goto __end;
	}

	// load image, resize and save
	TaviSysParam.gui.isUserBackground = 1;
	if( outW > BACKGROUND_WIDTH || outH > BACKGROUND_HEIGHT ) {
		int sw, sh;
		if( outW > BACKGROUND_WIDTH && outH > BACKGROUND_HEIGHT ) {
			sw = BACKGROUND_WIDTH;
			sh = BACKGROUND_HEIGHT;
		}
		else if( outW <= BACKGROUND_WIDTH && outH > BACKGROUND_HEIGHT ) {
			sw = BACKGROUND_HEIGHT*outW/outH;
			sw -= sw%16;
			sh = BACKGROUND_HEIGHT;
		}
		else if( outW > BACKGROUND_WIDTH && outH <= BACKGROUND_HEIGHT ) {
			sw = BACKGROUND_WIDTH;
			sh = BACKGROUND_WIDTH*outH/outW;
		}
		pBGY = ( unsigned char* )malloc( BACKGROUND_WIDTH*BACKGROUND_HEIGHT );
		pBGUV = ( unsigned char* )malloc( BACKGROUND_WIDTH*BACKGROUND_HEIGHT/2 );
		ReduceSizeYUV420( pY, pUV, outW, outH, pBGY, pBGUV, sw, sh, prg );
		// Save background image
		SaveYUV420Raw( BACKGROUND_PATH, pBGY, pBGUV, sw, sh, prg );
	}
	else if( outW <= BACKGROUND_WIDTH && outH <= BACKGROUND_HEIGHT ) {
		SaveYUV420Raw( BACKGROUND_PATH, pY, pUV, outW, outH, prg );
	}
	
	if( pBGY ) free( pBGY );
	if( pBGUV ) free( pBGUV );

	// RESTORE KEY SAMPLINGRATE
	speed = 10; /* fastest */
	if( ioctl( handle_key, TAVI_KEY_IOCS_SPEED, &speed ) < 0 ) {
		perror( "ioctl(TAVI_KEY_IOCS_SPEED)\n" );
	}
	
__end:
	delete pMsgBox;
	TAVI_BG_PlayPause();
}

static int OnInfo( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	pViewer->m_pBottomWnd->Hide();
	pViewer->m_fUpdateImage = false;

	CIVInfoWnd* pInfo = new CIVInfoWnd( 
		NULL, pViewer->GetCurrentPath(), pViewer->m_nCurrentMedia );
	pInfo->Resize( CRect(8, 35, 309, 225), false );
	pInfo->SetWindowList( IV_GetWndList() );
	pInfo->RegisterWndList();
	pInfo->SetFocus();
	return 0;
}

static int OnSlideTime5Sec( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	TaviSysParam.photo.slideshowtime = 0;
	mep_report( MEP_REPORT_ABSOLUTE );
	mep_eat_event();
	pViewer->m_fUpdateImage = false;

	return 0;
}

static int OnSlideTime10Sec( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	TaviSysParam.photo.slideshowtime = 1;
	mep_report( MEP_REPORT_ABSOLUTE );
	mep_eat_event();
	pViewer->m_fUpdateImage = false;

	return 0;
}

static int OnSlideTime20Sec( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	TaviSysParam.photo.slideshowtime = 2;
	mep_report( MEP_REPORT_ABSOLUTE );
	mep_eat_event();
	pViewer->m_fUpdateImage = false;

	return 0;
}

static int OnSlideTime30Sec( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	TaviSysParam.photo.slideshowtime = 3;
	mep_report( MEP_REPORT_ABSOLUTE );
	mep_eat_event();
	pViewer->m_fUpdateImage = false;

	return 0;
}

static int RepeatOn( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	TaviSysParam.photo.repeat = 1;
	mep_report( MEP_REPORT_ABSOLUTE );
	mep_eat_event();
	pViewer->m_fUpdateImage = false;

	return 0;
}

static int RepeatOff( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	TaviSysParam.photo.repeat = 0;
	mep_report( MEP_REPORT_ABSOLUTE );
	mep_eat_event();
	pViewer->m_fUpdateImage = false;

	return 0;
}

static int AutoHide( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	g_TopPanel->SetVisible( true );
	g_TopPanel->Update();
	pViewer->m_pBottomWnd->SetVisible( true );
	pViewer->m_pBottomWnd->Show();
	TaviSysParam.photo.info_display = AUTO_HIDE;
	mep_report( MEP_REPORT_ABSOLUTE );
	mep_eat_event();
	pViewer->m_fUpdateImage = false;

	return 0;
}

static int AlwaysHide( RMuint8* arg )
{
	CIViewer* pViewer = ( CIViewer* )arg;
	pViewer->m_pCtxMenuWnd->Hide();
	pViewer->HidePanel();
	TaviSysParam.photo.info_display = ALWAYS_HIDE;
	mep_report( MEP_REPORT_ABSOLUTE );
	mep_eat_event();
	pViewer->m_fUpdateImage = false;

	return 0;
}



void CIViewer::CreateContextMenu( void )
{
	int nSelect;
	CPopUpSubMenu* pSubMenu;

	m_pCtxMenuWnd->Reset();
	m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( 
		ML_META_INFO, NULL, -1, OnInfo, (RMuint8*)this );
	if( m_success ) {
		m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( 
			ML_BG_IMAGE, NULL, -1, OnSetBackground, (RMuint8*)this );
	}
	pSubMenu = new CPopUpSubMenu;
	pSubMenu->AddSubMenuItem( ML_5_SEC, TRUE, OnSlideTime5Sec, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_10_SEC, TRUE, OnSlideTime10Sec, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_20_SEC, TRUE, OnSlideTime20Sec, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_30_SEC, TRUE, OnSlideTime30Sec, (RMuint8*)this );
	pSubMenu->PointedMenuItemIdx = TaviSysParam.photo.slideshowtime;
	m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( 
	    ML_SLIDE_SHOW_TIME, pSubMenu, 
	    TaviSysParam.photo.slideshowtime, 
	    NULL, NULL );
	    
	pSubMenu = new CPopUpSubMenu;
	pSubMenu->AddSubMenuItem( ML_ON, TRUE, RepeatOn, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_OFF, TRUE, RepeatOff, (RMuint8*)this );
	pSubMenu->PointedMenuItemIdx = !TaviSysParam.photo.repeat;
	m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( 
	    ML_REPEAT, pSubMenu, !TaviSysParam.photo.repeat, NULL, NULL );

  pSubMenu = new CPopUpSubMenu;
  pSubMenu->AddSubMenuItem( ML_AUTO_HIDE, TRUE, AutoHide, (RMuint8*)this );
  pSubMenu->AddSubMenuItem( ML_ALWAYS_HIDE, TRUE, AlwaysHide, (RMuint8*)this );
  pSubMenu->PointedMenuItemIdx = TaviSysParam.photo.info_display;
  m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu(
      ML_SHOW_INFO, pSubMenu, TaviSysParam.photo.info_display, NULL, NULL );

	extern int handle_key;
	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );
}

void CIViewer::Show( void )
{
	CWindow::Show();
	mep_report( MEP_REPORT_ABSOLUTE );

	gettimeofday( &m_tvPanel, NULL );
	Flush();
	m_fAutoHide = true;
	ClearKeyBuffer();
	extern int handle_key;
	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );
}

void CIViewer::ShowPanel( void )
{
	if( m_pBottomWnd->IsShow() == false &&
	    TaviSysParam.photo.info_display == AUTO_HIDE ) {
		gettimeofday( &m_tvPanel, NULL );
		m_pBottomWnd->SetVisible( true );
		m_pBottomWnd->Show();
		g_TopPanel->SetVisible( true );
		g_TopPanel->Show();
	}
}

void CIViewer::HidePanel( void )
{
	ClearOsd();
	m_pBottomWnd->SetVisible( false );
	g_TopPanel->SetVisible( false );
	Flush();
}

int CIViewer::GetTotal( void )
{
	return( m_plist->GetCount()-m_offset );
}

CString& CIViewer::GetCurrentFilename( void )
{
	int idx = m_nCurrent + m_offset;
	return m_plist->GetItem(idx)->m_szText; 
}

CString& CIViewer::GetCurrentPath( void )
{
	return m_szPath;
}
