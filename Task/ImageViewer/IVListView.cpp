//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer List Viewer
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVListView.cpp,v $
// $Revision: 1.6 $
// $Date: 2006/05/30 01:46:27 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVListView.cpp,v $
// Revision 1.6  2006/05/30 01:46:27  zstein
// - fixed saving a background image bug.
//
// Revision 1.5  2006/05/23 07:18:04  zstein
// - fixed saving last access folder bug.
//
// Revision 1.4  2006/05/22 02:59:23  zstein
// - removing a noise on playing a music play.
//
// Revision 1.3  2006/04/17 04:34:10  zstein
// - make TAVICAST to be invisible when TAVICAST doesn't exist.
//
// Revision 1.2  2006/04/15 01:38:21  zstein
// - TAVICAST.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*******************************************************************************

#include <Component/FileUtil.h>
#include <Lib/LibImage.h>
#include <fatfs.h>

#include "../Msg.h"
#include <Task/TopPanel.h>
#include <Task/tavi_global.h>
#include <Task/AudioPlayer/AudioPlayer.h>
#include "ImageViewer.h"
#include "IVMainWnd.h"
#include "IViewer.h"

// Macros
#define DIRECTORY_DEPTH	100
#define ML_AP_PLZ_WAIT		ML_MSG_PLZ_WAIT

// Extern Variables
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CTextEngine_BDF* pT;
extern CTopPanel* g_TopPanel;
extern SysParameter_t TaviSysParam;


extern void showbackground( int nMenuID );
// Local Variables

// Class Member Methods
CIVListView::CIVListView( CWindow* parent, const CString& szCurrentPath, int nSort, int nViewCount, CStack<FolderPrev>* pStack ) :
	CListView( parent, nSort, nViewCount ),
	m_szCurrentPath( szCurrentPath )
{
	m_guide = true;
	// FIXME: if directory depth is bigger than DIRECOTYR_DEPTH??
  m_pDirStack = new CStack< FolderPrev >( DIRECTORY_DEPTH );

	// check valid path
	uchar_t* wsz = new uchar_t[1024];
	RMuint32 fd;
	FATFS_ERROR err;
	err = fatfs_wdopen( (uchar_t*)m_szCurrentPath, &fd );
	if( err != FATFS_ERROR_NO_ERROR ) {
		str2ustr( wsz, _PICTURE_DIR, strlen(_PICTURE_DIR) );
	}
	else {
		ustrcpy( wsz, (uchar_t*)m_szCurrentPath );
		fatfs_dclose( fd );
	}
	
	m_szCurrentPath.SetString( (char*)wsz, true );

	int slashcnt = 0;
	int i=0, j=0;
	CString path;
	uchar_t tmp[1024];
	// Check TAVICAST FOLDER
	str2ustr( tmp, TAVICAST_DIR );
	if( ustrcasecmp(tmp, wsz) == 0 ) {
	  path.SetString( _PICTURE_DIR, false );
	  path.toUTF16();
	  FolderPrev p = { path, 0, 0 };
	  m_pDirStack->Push( p );
	}
	
	while( 1 ) {
		if( wsz[i] == '/' ) {
			tmp[j] = 0;
			path.SetString( (char*)tmp, true );
			slashcnt++;
			if( slashcnt > 2 ) {
				FolderPrev p = { path, 0, 0 };
				m_pDirStack->Push( p );
			}
			tmp[j++] = wsz[i];
		}
		else if( wsz[i] == 0 ) {
			break;
		}
		else {
			tmp[j] = wsz[i];
			j++;
		}
		i++;
	}

	ScanDir();

	m_pCtxMenuWnd = new CContextMenu( this );
	m_pCtxMenuWnd->SetVisible( false );

	mep_report( MEP_REPORT_RELATIVE );

	m_pMsg = NULL;
	delete[] wsz;
}

CIVListView::~CIVListView( void )
{
	delete m_pDirStack;
	if( m_pMsg ) delete m_pMsg;
	memset( TaviSysParam.photo.lastaccess, 0, sizeof(TaviSysParam.photo.lastaccess) );
	memcpy( TaviSysParam.photo.lastaccess, (char*)m_szCurrentPath, 2*m_szCurrentPath.GetSize() );
}

void CIVListView::OnKeypressed( const KeyEvent& evt )
{
	CFolderListItem* pItem;
	TMsgEntry msg;

	switch( evt.value ) {
	case VK_RIGHT:
		if( pItem->m_nType == NODE_TYPE_FILE ) break;
	case VK_ENTER:
		// No files or directories in the directory.
		if( m_uzList.Count() == 0 ) return;
		pItem = ( CFolderListItem* )GetSelectedItem();
		
		if( pItem->m_nType == NODE_TYPE_FILE ) {
			g_SysPixmap.Fill( 0, 30, 319, 239, 0 );
			CIViewer* pViewer = new CIViewer( NULL, this/*szDir, pFileList, GetSelectedIndex()-nCount*/ );
			pViewer->Resize( CRect(0, 30, 319, 239), false );
			pViewer->SetWindowList( IV_GetWndList() );
			pViewer->RegisterWndList();
			pViewer->SetFocus();
			pViewer->SetWindowTitle( "ImageViewer" );
			return;
		}
		else if( pItem->m_nType == NODE_TYPE_FOLDER ) { // Folder
			FolderPrev p = { m_szCurrentPath, m_nStartIdx, m_nSelectedIdx };
			m_pDirStack->Push( p );
			m_nStartIdx = 0;
			m_nSelectedIdx = 0;
			m_szCurrentPath.Add( (uchar_t*)"/\0\0\0" );
			m_szCurrentPath.Add( pItem->m_szText );
			if( ScanDir() == false ) {
				p = m_pDirStack->Pop();
				m_szCurrentPath = p.folder;
				return;
			}
			OnPaint( rcClient );
		}
		else { // Virtual Folder(TaviCast)
	    uchar_t wstr[100];
	    str2ustr( wstr, TAVICAST_DIR );
	    FolderPrev p = { m_szCurrentPath, m_nStartIdx, m_nSelectedIdx };
			m_pDirStack->Push( p );
			m_nStartIdx = 0;
			m_nSelectedIdx = 0;
			m_szCurrentPath.SetString( (char*)wstr, true );
			if( ScanDir() == false ) {
				p = m_pDirStack->Pop();
				m_szCurrentPath = p.folder;
				return;
			}
			OnPaint( rcClient );
		}
		break;
	case VK_ESC: // Ver 0.9.9.0 이후 수정사항
	case VK_LEFT:
		// 최상위 폴더
		if( m_pDirStack->IsEmpty() ) {
			if( evt.value == VK_ESC ) 
          SendMessage( WINDOW_SET_BG );
		}
		else {
			FolderPrev p = m_pDirStack->Pop();
			m_szCurrentPath = p.folder;
			m_nStartIdx = p.start_idx;
			m_nSelectedIdx = p.sel_idx;
			ScanDir();
			OnPaint( rcClient );
		}
		break;
	case VK_F2: // menu
		if( m_uzList.Count() == 0 ) break;
		CreateContextMenu();
		m_pCtxMenuWnd->SetFocus();
		break;
	//case VK_ESC: Ver 0.9.9.0
	//	SendMessage( WINDOW_KILL_TASK );
	//	break;
	case VK_EXT_ESC:
		SendMessage( WINDOW_TASK_KILL_AND_SWITCH );
		break;
	case VK_F3:
		SendMessage( WINDOW_SET_BG );
		break;
	case TAVI_KEY_LMODE:
		SendMessage( WINDOW_TASK_SWITCH );
		break;
	}

	CListView::OnKeypressed( evt );
}

void CIVListView::OnWindowEvent( const WindowEvent& evt )
{
	struct timeval tv;
	switch( evt.nMessage ) {
	case WINDOW_IDLE:
		if( g_pFocusedWnd == this ) ScrollText();
		break;
	}
}

#include "Lib/LibNFile.h"
bool CIVListView::ScanDir( void )
{
	bool ret=true;
	int nCount;
	int type;
	int media;
	CFolderListItem* puzItem;
	FATFS_ERROR sdFatfsErr;
	FATFS_DIR_ENTRY_INFO sdDirEntry;
	RMuint32 sdDirFD;
	CString foldername;

	TAVI_WakeUp();

	uchar_t* pwChar;
	int len = m_szCurrentPath.GetSize();
	
	pwChar = ( uchar_t* )m_szCurrentPath;
	sdFatfsErr = fatfs_wdopen( (RMuint16*)pwChar, &sdDirFD );

	m_uzList.FreeAll();
	BeforeScan();
	nCount = 0;
	while( (sdFatfsErr=fatfs_dread(sdDirFD, &sdDirEntry)) == FATFS_ERROR_NO_ERROR ) {
		if( ustrcmp((uchar_t*)sdDirEntry.name, (uchar_t*)".\0") == 0 ) continue;
		if( ustrcmp((uchar_t*)sdDirEntry.name, (uchar_t*)".\0.\0") == 0 ) continue;

		if( sdDirEntry.attributes&0x10 ) {
			type = NODE_TYPE_FOLDER;
		}
		else {
			media = CheckMedia( sdDirEntry.name, true );
			switch( media ) {
			case MEDIA_JPEG:
			case MEDIA_BMP:
				break;
			default:
				continue;
			}
			type = NODE_TYPE_FILE;
		}

		if( type == NODE_TYPE_FOLDER )
			puzItem = new CFolderListItem( nCount, 4, 5, (char*)sdDirEntry.name, sdDirEntry.file_last_access_date );
		else 
			puzItem = new CFolderListItem( nCount, 22, 23, (char*)sdDirEntry.name, sdDirEntry.file_last_access_date );
		puzItem->m_nType = type;
		puzItem->m_pY = NULL;
		puzItem->m_pUV = NULL;
		puzItem->m_nMedia = media;
		m_uzList.Add( (CListItem*)puzItem );
		nCount++;
		if( nCount == MAX_FILES ) break;
	}

	if( sdFatfsErr != FATFS_ERROR_NO_MORE_ENTRIES ) {
		printf( "failed to fatfs_dread \n" );
	}
	fatfs_dclose( sdDirFD );

	// Add TaviCast
	if( m_pDirStack->IsEmpty() ) {
    FATFS_DIR_ENTRY_INFO info;
    bool bTaviCast = NFile::Exist( "/hd1", "TAVICAST", info);

    if( bTaviCast ) {
    	puzItem = new CFolderListItem( 
    	                nCount, 
    	                LII_VOICE_RECORD_U, 
    	                LII_VOICE_RECORD_S, 
    	                (char*)info.name, 
    	                0 );
      puzItem->m_nType = NODE_TYPE_VIRTUAL;
      puzItem->m_pY = NULL;
      puzItem->m_pUV = NULL;
      m_uzList.Add( (CListItem*)puzItem );    
    }
  }
	
	Sort( TaviSysParam.sort );
	ResetScrollBar();
	
	// display directory name
	SetTopTitle();

	showbackground( ID_BG_PHOTO );

	return true;
}

void CIVListView::SetTopTitle( void )
{
	if( m_pDirStack->Count() ) {
		uchar_t name[MAX_PATH];
		uchar_t* folder = ( uchar_t* )GetFilename( (char*)m_szCurrentPath, true );
		uchar_t parent[] = { '.', '.', '/', 0 };
		ustrcpy( name, parent );
		ustrcat( name, folder );
		g_TopPanel->SetTitle( (char*)name, true );
	}
	else {
		g_TopPanel->SetTitle( ML_PHOTO );
	}
	g_TopPanel->Update();
}

void CIVListView::Update( void )
{
	m_nSelectedIdx = 0;
	m_nStartIdx = 0;
	ResetScrollBar();
	CWindow::Update();
}

void CIVListView::OnPaint( const CRect& rc )
{
	if( m_uzList.Count() == 0 ) {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), 0 );
		pT->DrawResText( ML_NO_FILES, 
			320/2 - (pT->GetStringWidth(ML_NO_FILES, 12, DT_TRANSPARENCY|DT_OUTLINE)/2), 
			110, _WHITE, 12, DT_TRANSPARENCY|DT_OUTLINE );
		Flush();
		return;
	}

	if( m_guide ) {
		DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME| BGPID_MENU );
	}
	CListView::OnPaint( rc );
}

void CIVListView::Show( void )
{
	SetTopTitle();
	CWindow::Show();

	extern int handle_key;
	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );
	ClearKeyBuffer();
}

void CIVListView::Hide( void )
{
	CWindow::Hide();
}

static void ResetPrevStatus( CIVListView* pview )
{
	CStack< FolderPrev > stack;
	FolderPrev p;
	while( pview->m_pDirStack->IsEmpty() == false ) {
		p = pview->m_pDirStack->Pop();
		p.sel_idx = 0;
		p.start_idx = 0;
		stack.Push( p );
	}

	while( stack.IsEmpty() == false ) {
		p = stack.Pop();
		pview->m_pDirStack->Push( p );
	}
}

static int NameAscentSort( RMuint8* arg )
{
	CIVListView* pView = ( CIVListView* )arg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	TaviSysParam.sort = CListView::NAME_ASCENT;
	pView->Sort( CListView::NAME_ASCENT );
	pView->Update();
	ResetPrevStatus( pView );

	return 0;
}

static int NameDescentSort( RMuint8* arg )
{
	CIVListView* pView = ( CIVListView* )arg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	TaviSysParam.sort = CListView::NAME_DESCENT;
	pView->Sort( CListView::NAME_DESCENT );
	pView->Update();
	ResetPrevStatus( pView );

	return 0;
}

static int DateAscentSort( RMuint8* arg )
{
	CIVListView* pView = ( CIVListView* )arg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	TaviSysParam.sort = CListView::DATE_ASCENT;
	pView->Sort( CListView::DATE_ASCENT );
	pView->Update();
	ResetPrevStatus( pView );

	return 0;
}

static int DateDescentSort( RMuint8* arg )
{
	CIVListView* pView = ( CIVListView* )arg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	TaviSysParam.sort = CListView::DATE_DESCENT;
	pView->Sort( CListView::DATE_DESCENT );
	pView->Update();
	ResetPrevStatus( pView );

	return 0;
}

static int ViewModeList( RMuint8* arg )
{
	CIVListView* pView = ( CIVListView* )arg;
	TMsgEntry msg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	if( TaviSysParam.photo.view != PHOTO_VIEW_LISTVIEW ) {
		TaviSysParam.photo.view = PHOTO_VIEW_LISTVIEW;
		msg.ssToTaskId = pView->GetTaskID();
		msg.ulMsgId = MSG_WINDOW;
		msg.usParam1 = g_pRootWnd->GetWindowID(); // Virtual Root Window
		msg.usParam2 = ON_VIEWTYPE | 0;
		MSG_Send( pView->GetTaskID(), &msg );
	}
	
	return 0;
}

static int ViewModePreview( RMuint8* arg )
{
	CIVListView* pView = ( CIVListView* )arg;
	TMsgEntry msg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	if( TaviSysParam.photo.view != PHOTO_VIEW_PREVIEW ) {
		msg.ssToTaskId = pView->GetTaskID();
		msg.ulMsgId = MSG_WINDOW;
		msg.usParam1 = g_pRootWnd->GetWindowID(); // Virtual Root Window
		msg.usParam2 = ON_VIEWTYPE | 1;
		MSG_Send( pView->GetTaskID(), &msg );
		TaviSysParam.photo.view = PHOTO_VIEW_PREVIEW;
	}
	return 0;
}

static int ViewModeSlide( RMuint8* arg )
{
	CIVListView* pView = ( CIVListView* )arg;
	TMsgEntry msg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	if( TaviSysParam.photo.view != PHOTO_VIEW_THUMBNAIL ) {
		msg.ssToTaskId = pView->GetTaskID();
		msg.ulMsgId = MSG_WINDOW;
		msg.usParam1 = g_pRootWnd->GetWindowID(); // Virtual Root Window
		msg.usParam2 = ON_VIEWTYPE | 2;
		MSG_Send( pView->GetTaskID(), &msg );
		TaviSysParam.photo.view = PHOTO_VIEW_THUMBNAIL;
	}
	return 0;
}

static int OnSetBackground(  RMuint8* arg )
{
	CIVListView* pView = ( CIVListView* )arg;
	CString szFilepath;
	CListItem* pItem;
	extern RMuint32 *backgroundY;
	extern RMuint32* backgroundUV;
	unsigned char* pBGY=NULL, *pBGUV=NULL;
	unsigned char* pY = NULL;
	unsigned char* pUV = NULL;
	int len;
	PFN_Progress prg=0;
	CMsgBox* pMsgBox;

	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();

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
	extern int handle_key;
	if( ioctl( handle_key, TAVI_KEY_IOCS_SPEED, 1/*slowest*/ ) < 0 ) {
		perror( "ioctl(TAVI_KEY_IOCS_SPEED)\n" );
	}
	
	// Show wait message
	pMsgBox =  new CMsgBox( 
		ML_INFORMATION, 
		_ORANGE, ML_MSG_PLZ_WAIT, 
		244, 123, 0, DL_BG_ICON );
	pMsgBox->Show();
	
	pItem = pView->GetSelectedItem();
	if( pItem->m_nType == NODE_TYPE_FOLDER ) goto __end;
	
	szFilepath = pView->GetCurrentPath();
	szFilepath.Add( (uchar_t*)"/\0\0\0" );
	szFilepath.Add( pItem->m_szText );
	len = szFilepath.GetSize();

	// Load Image
	int outW, outH;
	pY = ( unsigned char* )backgroundY;
	pUV = pY + BMP_MAX_SIZE;
	if( LoadImageYUV420( (char*)szFilepath, pY, pUV, 
		BACKGROUND_YLENGTH, BACKGROUND_YLENGTH/2,
		&outW, &outH, prg ) ) {
		// show error message.
		mep_eat_event();
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
		int x=0, y=0;
		if( sw < BACKGROUND_WIDTH ) x = (BACKGROUND_WIDTH-sw)/2;
		if( sh < BACKGROUND_WIDTH ) y = (BACKGROUND_HEIGHT-sh)/2;
		if( x || y ) {
			pMpegDecoder->DisplayYUV420( 0, 0, 0, 0, 0, 0, 0, 0 );
		}
		pMpegDecoder->DisplayYUV420( (RMuint8*)pBGY, (RMuint8*)pBGUV, 
			x, y, sw, sh, 
			BACKGROUND_WIDTH, BACKGROUND_HEIGHT );
	}
	else if( outW <= BACKGROUND_WIDTH && outH <= BACKGROUND_HEIGHT ) {
		SaveYUV420Raw( BACKGROUND_PATH, pY, pUV, outW, outH, prg );
		pMpegDecoder->DisplayYUV420( 0, 0, 0, 0, 0, 0, 0, 0 );
		int x = (BACKGROUND_WIDTH-outW)/2;
		int y = (BACKGROUND_HEIGHT-outH)/2;
		pMpegDecoder->DisplayYUV420( (RMuint8*)pY, (RMuint8*)pUV, 
			x, y, outW, outH, 
			BACKGROUND_WIDTH, BACKGROUND_HEIGHT );
	}

__end:
	delete pMsgBox;
	if( pBGY ) free( pBGY );
	if( pBGUV ) free( pBGUV );

	// RESTORE KEY SAMPLINGRATE
	extern int handle_key;
	if( ioctl( handle_key, TAVI_KEY_IOCS_SPEED, 10/*fastest*/ ) < 0 ) {
		perror( "ioctl(TAVI_KEY_IOCS_SPEED)\n" );
	}

	// eat touchpad event
	mep_eat_event();
	TAVI_BG_PlayPause();
}

void CIVListView::CreateContextMenu( void )
{
	m_pCtxMenuWnd->Reset();
	CPopUpSubMenu* pSubMenu = new CPopUpSubMenu;
	pSubMenu->AddSubMenuItem( ML_NAME_ASCENT, 0, NameAscentSort, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_NAME_DESCENT, 0, NameDescentSort, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_TIME_ASCENT, 0, DateAscentSort, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_TIME_DESCENT, 0, DateDescentSort, (RMuint8*)this );
	//pSubMenu->AddSubMenuItem( ML_BACK, FALSE, NULL, NULL );
	pSubMenu->PointedMenuItemIdx = TaviSysParam.sort;
	m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_SORTING_METHOD, pSubMenu, TaviSysParam.sort, NULL, NULL );
	#if 1
	pSubMenu = new CPopUpSubMenu;
	pSubMenu->AddSubMenuItem( ML_LIST_VIEW, 0, ViewModeList, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_PREVIEW, 0, ViewModePreview, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_SLIDE_VIEW, 0, ViewModeSlide, (RMuint8*)this );
	//pSubMenu->AddSubMenuItem( ML_BACK, FALSE, NULL, NULL );
	pSubMenu->PointedMenuItemIdx = TaviSysParam.photo.view;
	m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_PV_VIEW_MODE, pSubMenu, TaviSysParam.photo.view, NULL, NULL );
	#endif
	if( GetSelectedItem()->m_nType == NODE_TYPE_FILE )
		m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_BG_IMAGE, NULL, 0, OnSetBackground, (RMuint8*)this );
	//m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );
}
