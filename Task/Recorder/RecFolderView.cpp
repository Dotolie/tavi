//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Recorder Folder View Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecFolderView.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: RecFolderView.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.30  2005/12/22 05:21:39  zstein
// *** empty log message ***
//
// Revision 1.29  2005/12/05 05:59:07  zstein
// *** empty log message ***
//
// Revision 1.28  2005/12/05 04:57:37  zstein
// *** empty log message ***
//
// Revision 1.27  2005/12/01 01:06:36  zstein
// *** empty log message ***
//
// Revision 1.26  2005/11/24 03:03:41  zstein
// *** empty log message ***
//
// Revision 1.25  2005/11/18 09:14:49  zstein
// *** empty log message ***
//
// Revision 1.24  2005/11/16 07:12:41  zstein
// *** empty log message ***
//
// Revision 1.23  2005/11/10 01:09:16  zstein
// *** empty log message ***
//
// Revision 1.22  2005/11/08 07:53:26  zstein
// *** empty log message ***
//
// Revision 1.21  2005/11/08 01:25:27  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <Component/FileUtil.h>
#include <Task/Msg.h>
#include <Task/tavi_global.h>
#include <Task/MiscCtrl.h>
#include <Task/ExtLCD.h>
#include <Task/AudioPlayer/AudioPlayer.h>
#include <Task/AudioPlayer/ID3/m3u.h>
#include <Task/AudioPlayer/ID3/IDDB.h>
#include <Lib/LibImage.h>

#include <Task/AudioPlayer/AudioCmd.h>
#include "RecorderMain.h"
#include "RecFolderView.h"

// Extern variables
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CMiscCtrl* g_MiscCtrl;
extern CExtLCD* g_ExtLCD;
extern CTopPanel* g_TopPanel;
extern CTextEngine_BDF* pT;


static unsigned char* GetImageElement( BITMAPFILEINFO256 bmp, int dh, int id, int count )
{
	unsigned char* p;
	int w = bmp.bitmapinfoheader.biWidth;
	int h = bmp.bitmapinfoheader.biHeight;

	p = bmp.image_buffer+w*dh*id;

	return p;
}

CRecFolderView::CRecFolderView( CWindow* parent, const CString& szRootPath ) :
	CListView( parent ),
	m_szRootPath( szRootPath )
{
	CPixmap* puzPixmapIcon;
	
	for( int i=0; i<TYPE_ICON_COUNT; i++ ) {
		puzPixmapIcon = new CPixmap;
		puzPixmapIcon->CreatePixmap( 12, 12, 8, (char*)GetImageElement(TAVI_IMAGES[TYPE_ICON_LIST], 
			TYPE_ICON_HEIGHT, i, TYPE_ICON_COUNT), false );
		AddPixmapIcon( puzPixmapIcon );
	}

	// Set selectbar image
	CPixmap* pBarPixmap = new CPixmap;
	pBarPixmap->CreatePixmap( 
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biWidth, 
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biHeight,
		8,
		(char*)TAVI_IMAGES[LIST_SELECTBAR].image_buffer,
		false );
		
	SetSelectBarPixmap( pBarPixmap );
	
	m_pStack = new CStack< FolderPrev >( 20 );
	m_pCtxMenuWnd = new CContextMenu( this );
	m_pCtxMenuWnd->SetVisible( false );


	m_nStartIdx = 0;
	m_nSelectedIdx = 0;
	m_nListCount = 8;
	m_szCurrentDir = szRootPath;
	ScanDir( m_szRootPath );
}

CRecFolderView::~CRecFolderView( void )
{
	REC_Kill_Mp3Player();
	delete m_pStack;
	if( m_pMsg ) delete m_pMsg;	
}

bool CRecFolderView::ScanDir( CString& szPath )
{
	bool ret=true;
	int nCount;
	int type, nMedia;
	CFolderListItem* puzItem;
	FATFS_ERROR sdFatfsErr;
	FATFS_DIR_ENTRY_INFO sdDirEntry;
	RMuint32 sdDirFD;
	uchar_t* szFullPath;

	m_uzList.FreeAll();
	m_nStartIdx = 0;
	m_nSelectedIdx = 0;
	sdFatfsErr = fatfs_wdopen( (uchar_t*)szPath, &sdDirFD );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		return false;
	}

	szFullPath = new uchar_t[1024];
	
	while( (sdFatfsErr=fatfs_dread(sdDirFD, &sdDirEntry)) == FATFS_ERROR_NO_ERROR ) {
		if( ustrcmp((uchar_t*)sdDirEntry.name, (uchar_t*)".\0") == 0 ) continue;
		if( ustrcmp((uchar_t*)sdDirEntry.name, (uchar_t*)".\0.\0") == 0 ) continue;
		
		if( sdDirEntry.attributes&0x10 ) type = NODE_TYPE_FOLDER;
		else {
			nMedia = CheckMedia(sdDirEntry.name, true);
			switch( nMedia ) {
			case MEDIA_MP3:
			case MEDIA_OGG:
			case MEDIA_WMA:
				break;
			default:
				continue;
			}
			type = NODE_TYPE_FILE;
		}

		if( type == NODE_TYPE_FOLDER )
			puzItem = new CFolderListItem( nCount, 4, 5, (char*)sdDirEntry.name, sdDirEntry.file_last_access_date );
		else {
			puzItem = new CFolderListItem( nCount, 10, 11, (char*)sdDirEntry.name, sdDirEntry.file_last_access_date );
			ustrcpy( szFullPath, (uchar_t*)szPath );
			ustrcat( szFullPath, (uchar_t*)"/\0\0" );
			ustrcat( szFullPath, (uchar_t*)sdDirEntry.name );
			puzItem->m_nkey = CDeviceMusicDB::Generate_Key( (LPWSTR)szFullPath );
		}
		puzItem->m_nType = type;
		AddItem( (CListItem*)puzItem );
		if( m_uzList.Count() == MAX_FILES ) break;
	}
	fatfs_dclose( sdDirFD );
	
	Sort( TaviSysParam.sort );
	ResetScrollBar();

	g_TopPanel->SetTitle( GetFilename((char*)m_szCurrentDir, true), true );
	g_TopPanel->Update();

	delete[] szFullPath;

	return ret;
}

void CRecFolderView::OnPaint( const CRect& rc )
{
	if( m_uzList.Count() == 0 ) {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), 0 );
		pT->DrawResText( ML_NO_FILES, 
			320/2 - (pT->GetStringWidth(ML_NO_FILES, 12, DT_TRANSPARENCY|DT_OUTLINE)/2), 
			110, _WHITE, 12, DT_TRANSPARENCY|DT_OUTLINE );
		op_Flush();
		return;
	}
	
	// CHECK PLAYING FILE
	CListItem* pitem;
	extern int g_PlayKey;
	for( int i=0; i<m_uzList.Count(); i++ ) {
		pitem = m_uzList[i];
		if( pitem->m_nType == NODE_TYPE_FILE ) {
			if( pitem->m_nkey == g_PlayKey )  {
				if( AP_GetPlayState() == AUDIO_STATE_PLAYING ) {
					pitem->m_nSelectedIconIdx = LII_PLAY_S;
					pitem->m_nIconIdx = LII_PLAY_U;
				}
				else {
					pitem->m_nSelectedIconIdx = LII_PAUSE_S;
					pitem->m_nIconIdx = LII_PAUSE_U;
				}
			}
			else {
				pitem->m_nSelectedIconIdx = LII_MUSIC_S;
				pitem->m_nIconIdx = LII_MUSIC_U;
			}
		}
	}
	DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME| BGPID_MENU );
	CListView::OnPaint( rc );
}

void CRecFolderView::OnKeypressed( const KeyEvent& evt )
{
	CFolderListItem* puzItem;
	TMsgEntry msg;
	KeyEvent kevt = evt;

	
	if( m_pMsg ) {
		delete m_pMsg;
		m_pMsg = NULL;
		if( m_pStack->IsEmpty() ) {
			SendMessage( WINDOW_KILL );
			return;
		}
		else {
			FolderPrev p = m_pStack->Pop();
			m_szCurrentDir = p.folder;
			ScanDir( m_szCurrentDir );
			OnPaint( rcClient );
		}
		return;
	}
	
	switch( evt.value ) {
	case TAVI_KEY_JOG2_UP:
	case TAVI_KEY_JOG2_ENTER:
		if( g_MiscCtrl->FolderOpen() ) break;
	case VK_RIGHT:
	case VK_ENTER:
		puzItem = ( CFolderListItem* )GetSelectedItem();
		if( !puzItem ) return;
		if( puzItem->m_nType == NODE_TYPE_FOLDER ) {
			FolderPrev p = { m_szCurrentDir, m_nStartIdx, m_nSelectedIdx };
			m_pStack->Push( p );
			m_szCurrentDir.Add( (uchar_t*)"/\0\0\0" );
			m_szCurrentDir.Add( puzItem->m_szText );
			ScanDir( m_szCurrentDir );
			OnPaint( rcClient );
		}
		else { // launch music player
			if( evt.value == VK_RIGHT || evt.value == TAVI_KEY_JOG2_UP ) break;
			#if 0
			int temp = NumOfFolders();
			int len = m_szCurrentDir.GetSize()-5; // remove /hd1/
			int size = len*2; // remove /hd1/
			TaviSysParam.music.index = (m_nStartIdx+m_nSelectedIdx)-temp;
			if( g_MusicFilepath ) free( g_MusicFilepath );
			g_MusicFilepath = ( uchar_t* )malloc( size+1 );
			g_MusicFilepath[len] = 0;
			g_MusicFilepathLen = size;
			memcpy( g_MusicFilepath, (char*)m_szCurrentDir+10, size ); // start from MUSIC
			REC_MP3Play();
			#else
			CString fullpath;
			fullpath = m_szCurrentDir;
			fullpath.Add( (uchar_t*)"/\0" );
			fullpath.Add( puzItem->m_szText );
			REC_MP3Play( fullpath );
			#endif
		}
		break;
	case TAVI_KEY_JOG2_DOWN:
		if( g_MiscCtrl->FolderOpen() ) break;
	case TAVI_KEY_JOG1_ENTER:
		if( g_MiscCtrl->FolderOpen() ) break;
	case VK_ESC: // Ver 0.9.9.0 이후 수정사항
	case VK_LEFT:
		if( m_pStack->IsEmpty() ) {
			// Exit folder viewer
			SendMessage( WINDOW_KILL );
			return;
		}
		else {
			FolderPrev p = m_pStack->Pop();
			m_szCurrentDir = p.folder;
			ScanDir( m_szCurrentDir );
			OnPaint( rcClient );
		}
		break;
	//case TAVI_KEY_JOG1_ENTER:
	//	if( g_MiscCtrl->FolderOpen() ) break;
	//case VK_ESC:
	//	// Exit folder viewer
	//	SendMessage( WINDOW_KILL_TASK );
		return;
	case TAVI_KEY_JOG1_UP:
		if( g_MiscCtrl->FolderOpen() ) break;
		kevt.value = VK_UP;
		break;
	case TAVI_KEY_JOG1_DOWN:
		if( g_MiscCtrl->FolderOpen() ) break;
		kevt.value = VK_DOWN;
		break;
	case VK_F2: // menu
		CreateContextMenu();
		m_pCtxMenuWnd->SetFocus();
		break;
	}
	CListView::OnKeypressed( kevt );

	CFolderListItem* pItem = ( CFolderListItem* )GetSelectedItem();
	if( pItem ) {
		CExtLCD::ExtLCDListItem item;
		item.m_nIcon = (pItem->m_nType == NODE_TYPE_FOLDER) ?
			CExtLCD::EXT_LCD_ICON_FOLDER : 
			CExtLCD::EXT_LCD_ICON_MUSIC;
		item.m_nIndex =GetSelectedIndex();
		item.m_nTotal = GetCount();
		item.m_szText = pItem->m_szText;
		g_ExtLCD->SetListItem( item );
		if( g_MiscCtrl->FolderOpen() == false ) g_ExtLCD->Update();
	}
}

void CRecFolderView::OnWindowEvent( const WindowEvent& evt )
{
	struct timeval tv;
	
	switch( evt.nMessage ) {
	case ON_FOLDER:
		if( evt.nParam ) g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
		else g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
		g_ExtLCD->Update();
		break;
	case WINDOW_IDLE:
		if( g_MiscCtrl->FolderOpen() ) {
			if( g_pFocusedWnd == this ) ScrollText();
		}
		else g_ExtLCD->ScrollText();	
		if( m_pMsg ) {
			gettimeofday( &tv, NULL );
			if( abs(tv.tv_sec-m_tv.tv_sec) > 3 ) {
				delete m_pMsg;
				m_pMsg = NULL;
				if( m_pStack->IsEmpty() == true ) {
					SendMessage( WINDOW_KILL );
				}
				else {
					FolderPrev p = m_pStack->Pop();
					m_szCurrentDir = p.folder;
					ScanDir( m_szCurrentDir );
					OnPaint( rcClient );
				}
			}
		}
		break;
	}
}

void CRecFolderView::Show( void )
{
	extern int handle_key;
	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );

	op_ClearAreaOSD( 0, 30, 320, 210 );
	CFolderListItem* pItem = ( CFolderListItem* )GetSelectedItem();
	if( pItem ) {
		CExtLCD::ExtLCDListItem item;
		item.m_nIcon = (pItem->m_nType == NODE_TYPE_FOLDER) ?
			CExtLCD::EXT_LCD_ICON_FOLDER : 
			CExtLCD::EXT_LCD_ICON_MUSIC;
		item.m_nIndex = GetSelectedIndex();
		item.m_nTotal = GetCount();
		item.m_szText = pItem->m_szText;
		g_ExtLCD->SetListItem( item );
		//g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
	}
	if( g_MiscCtrl->FolderOpen() == false ) g_ExtLCD->Update();
	g_TopPanel->SetTitle( GetFilename((char*)m_szCurrentDir, true), true );
	g_TopPanel->Update();
	m_pCtxMenuWnd->SetVisible( false );
	CWindow::Show();

	ClearKeyBuffer();
}

int CRecFolderView::NumOfFolders( void )
{
	int nfolder = 0;
	for( int i=0; i<m_uzList.Count(); i++ ) {
		if( m_uzList[i]->m_nType == NODE_TYPE_FOLDER ) {
			nfolder++;
			continue;
		}
	}

	return nfolder;
}

static void ResetPrevStatus( CRecFolderView* pview )
{
	CStack< FolderPrev > stack;
	FolderPrev p;
	while( pview->m_pStack->IsEmpty() == false ) {
		p = pview->m_pStack->Pop();
		p.sel_idx = 0;
		p.start_idx = 0;
		stack.Push( p );
	}

	while( stack.IsEmpty() == false ) {
		p = stack.Pop();
		pview->m_pStack->Push( p );
	}
}


static int NameAscentSort( RMuint8* arg )
{
	CRecFolderView* pView = ( CRecFolderView* )arg;
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
	CRecFolderView* pView = ( CRecFolderView* )arg;
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
	CRecFolderView* pView = ( CRecFolderView* )arg;
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
	CRecFolderView* pView = ( CRecFolderView* )arg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	TaviSysParam.sort = CListView::DATE_DESCENT;
	pView->Sort( CListView::DATE_DESCENT );
	pView->Update();
	ResetPrevStatus( pView );

	return 0;
}

void CRecFolderView::CreateContextMenu( void )
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
	//m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );
}

