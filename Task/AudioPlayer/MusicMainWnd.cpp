//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Music Main Window
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/MusicMainWnd.cpp,v $
// $Revision: 1.4 $
// $Date: 2006/04/17 04:29:30 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: MusicMainWnd.cpp,v $
// Revision 1.4  2006/04/17 04:29:30  zstein
// - invisible TAVICAST when TAVICAST  foler is not exist.
//
// Revision 1.3  2006/04/15 01:24:28  zstein
// - TAVICAST.
//
// Revision 1.2  2006/03/07 02:33:54  zstein
// - delete "NOW PLAYING" item when a music is stopped.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.27  2006/02/07 09:10:56  zstein
// - changed JOG1_KEY_ENTER's action.
//
// Revision 1.26  2006/01/19 08:43:33  zstein
// *** empty log message ***
//
// Revision 1.25  2005/12/20 11:54:12  zstein
// *** empty log message ***
//
// Revision 1.24  2005/12/16 12:16:01  yong
// JOG2_UP/DONW corresponds to VK_DOWN/UP!!!
//
// Revision 1.23  2005/12/16 09:57:27  zstein
// *** empty log message ***
//
// Revision 1.22  2005/12/14 11:34:57  yong
// external LCD
//
// Revision 1.21  2005/12/12 10:42:41  yong
// for external LCD
//
// Revision 1.20  2005/12/12 07:37:53  yong
// massive reworking abount sub LCD behaviour
//
// Revision 1.19  2005/12/05 04:57:52  zstein
// *** empty log message ***
//
// Revision 1.18  2005/12/01 01:04:08  zstein
// *** empty log message ***
//
// Revision 1.17  2005/11/25 10:21:58  zstein
// *** empty log message ***
//
// Revision 1.16  2005/11/10 01:09:39  zstein
// *** empty log message ***
//
// Revision 1.15  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
//******************************************************************************

#include "Task/tavi_global.h"
#include "Task/MiscCtrl.h"
#include "Task/ExtLCD.h"
#include "Task/TopPanel.h"
#include "Lib/LibUtilities.h"
#include "Lib/LibWideString.h"
#include "Lib/LibText.h"
#include "tavi_text_resources.h"
#include "ImageResource.h"
#include "Driver/touchpad/meplib.h"

#include "AudioPlayer.h"
#include "MusicMainWnd.h"
#include "FolderView.h"
#include "ID3View.h"
#include "PlayListView.h"
#include "AudioCmd.h"

#define NODE_TYPE_IDDB_ARTIST		(NODE_TYPE_USER+1)
#define NODE_TYPE_IDDB_ALBUM		(NODE_TYPE_USER+2)
#define NODE_TYPE_IDDB_GENRE		(NODE_TYPE_USER+3)
#define NODE_TYPE_IDDB_ALLTRACK	    (NODE_TYPE_USER+4)
#define NODE_TYPE_PLAYLIST			(NODE_TYPE_USER+5)
#define NODE_TYPE_BACKTOPLAYER	    (NODE_TYPE_USER+6)
#define NODE_TYPE_TAVICAST          (NODE_TYPE_USER+7)

extern CExtLCD* g_ExtLCD;
extern CMiscCtrl* g_MiscCtrl;
extern CTopPanel* g_TopPanel;
extern CTextEngine_BDF* pT;
extern SysParameter_t	TaviSysParam;

static unsigned char* GetImageElement( BITMAPFILEINFO256 bmp, int dh, int id, int count )
{
	unsigned char* p;
	int w = bmp.bitmapinfoheader.biWidth;

	p = bmp.image_buffer+w*dh*id;

	return p;
}


#include "Lib/LibNFile.h"
CMusicMainWnd::CMusicMainWnd( CWindow* parent, CDeviceMusicDB* pMusicDB ) : 
	CListView( parent, -1 ),
	m_pMusicDB( pMusicDB )
{
	CListItem* puzItem;

	// Find TAVICAST folder
	bool bTaviCast = false;
	FATFS_DIR_ENTRY_INFO info;
    bTaviCast = NFile::Exist( "/hd1", "TAVICAST", info );

	// Playlist
	puzItem = new CListItem( -1, LII_PLAYLIST_U, LII_PLAYLIST_S, ML_PLAYLIST );
	puzItem->m_nType = NODE_TYPE_PLAYLIST;
	AddItem( puzItem );
	// Folder
	puzItem = new CListItem( -1, LII_FOLDER_U, LII_FOLDER_S, ML_FOLDER );
	puzItem->m_nType = NODE_TYPE_FOLDER;
	AddItem( puzItem );
	// Add Artist Item
	puzItem = new CListItem( -1, LII_ARTIST_U, LII_ARTIST_S, ML_ARTIST );
	puzItem->m_nType = NODE_TYPE_IDDB_ARTIST;
	AddItem( puzItem );
	// Add Album Item
	puzItem = new CListItem( -1, LII_ALBUM_U, LII_ALBUM_S, ML_ALBUM );
	puzItem->m_nType = NODE_TYPE_IDDB_ALBUM;
	AddItem( puzItem );
	// Add Genre Item
	puzItem = new CListItem( -1, LII_GENRE_U, LII_GENRE_S, ML_GENRE );
	puzItem->m_nType = NODE_TYPE_IDDB_GENRE;
	AddItem( puzItem );
	// Add Title Item
	puzItem = new CListItem( -1, LII_MUSIC_U, LII_MUSIC_S, ML_ALL_TRACK );
	puzItem->m_nType = NODE_TYPE_IDDB_ALLTRACK;
	AddItem( puzItem );
	// Add TAVICAST
	if( bTaviCast ) {
    	puzItem = new CListItem( -1, LII_VOICE_RECORD_U, LII_VOICE_RECORD_S, info.short_name );
    	puzItem->m_nType = NODE_TYPE_TAVICAST;
    	AddItem( puzItem );
	}

	// Set Ext. LCD Mode to list
	if( 1 || g_MiscCtrl->FolderClosed() ) {
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
	}
	
	puzItem = GetSelectedItem();
	CExtLCD::ExtLCDListItem item;
	item.m_nIndex = puzItem->m_nIdx;
	item.m_nTotal = GetCount();
	item.m_nIcon = CExtLCD::EXT_LCD_ICON_FOLDER;
	item.m_szText.SetString( (char*)pT->GetTextRes(puzItem->m_nResID), true );
	g_ExtLCD->SetListItem( item );
	if( 1 || g_MiscCtrl->FolderClosed() ) {
		g_ExtLCD->Update();
	}

	// configurate touchpad and key
	mep_report( MEP_REPORT_RELATIVE );
	SetTaviKeyMode( TK_REPEAT | TK_SINGLE );
}

CMusicMainWnd::~CMusicMainWnd( void )
{
}

void CMusicMainWnd::OnKeypressed( const KeyEvent& evt )
{
	KeyEvent kevt = evt;

	if( 1 || g_MiscCtrl->FolderClosed() ) {
		switch( evt.value ) {

		case TAVI_KEY_JOG1_UP:
			AP_Volume( 1 );
			break;

		case TAVI_KEY_JOG1_DOWN:
			AP_Volume( 0 );
			break;

		case TAVI_KEY_JOG1_ENTER:
			kevt.value = VK_ESC;
			break;

		case TAVI_KEY_JOG2_ENTER:
			kevt.value = VK_ENTER;
			break;

		case TAVI_KEY_JOG2_UP:
			kevt.value = VK_DOWN;
			break;

		case TAVI_KEY_JOG2_DOWN:
			kevt.value = VK_UP;
			break;
		}
	}
	
	CListView::OnKeypressed( kevt );

	switch( kevt.value ) {

	case VK_RIGHT:
	case VK_ENTER:
		DoSubMenu();
		return;

	case VK_ESC: // BG OR EXIT? That is the question
		MSG_Send( m_nTaskID, MSG_WINDOW, m_nWindowID, WINDOW_KILL_TASK );
		break;

	case VK_F3:
		MSG_Send( m_nTaskID, MSG_WINDOW, m_nWindowID, WINDOW_SET_BG );
		break;

	case TAVI_KEY_LMODE:
		MSG_Send( m_nTaskID, MSG_WINDOW, m_nWindowID, WINDOW_TASK_SWITCH );
		break;

	case VK_EXT_ESC:
		MSG_Send( m_nTaskID, MSG_WINDOW, m_nWindowID, WINDOW_TASK_KILL_AND_SWITCH );
		break;

	}

	SetListItem( (CListItem*)GetSelectedItem(), m_uzList.Count() );
}

void CMusicMainWnd::OnWindowEvent( const WindowEvent& evt )
{
	switch( evt.nMessage ) {
	case ON_FOLDER:
#if 1
		if( evt.nParam ) {	/* Folder Open */
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
		} else {
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
		}
#endif
		g_ExtLCD->Update();
		break;
	}
}

void CMusicMainWnd::DoSubMenu( void )
{
	CString szRootPath;
	CListItem* pItem = GetSelectedItem();
	uchar_t wstr[100];

	switch( pItem->m_nType ) {
	case NODE_TYPE_BACKTOPLAYER:
		AP_BackToPlayer();
		return;
	case NODE_TYPE_PLAYLIST:
		m_pwnd = new CPlaylistView( NULL );
		m_pwnd->SetWindowTitle( "Playlist" );
		break;

	case NODE_TYPE_FOLDER:
		szRootPath.SetString( TaviSysParam.music.lastaccess, true );
		m_pwnd = new CFolderView( NULL, szRootPath, TaviSysParam.sort );
		m_pwnd->SetWindowTitle( "MusicFolder" );
		break;

	case NODE_TYPE_TAVICAST:
	    str2ustr( wstr, TAVICAST_DIR );
	    szRootPath.SetString( (char*)wstr, true ); 
	    m_pwnd = new CFolderView( NULL, szRootPath, TaviSysParam.sort, false );
	    break;

	case NODE_TYPE_IDDB_ARTIST:
		m_pwnd = new CID3ListView( NULL, CID3ListView::ID3LIST_ARTIST, m_pMusicDB );
		m_pwnd->SetWindowTitle( "MusicArtist" );
		break;

	case NODE_TYPE_IDDB_ALBUM:
		m_pwnd = new CID3ListView( NULL, CID3ListView::ID3LIST_ALBUM, m_pMusicDB );
		m_pwnd->SetWindowTitle( "MusicAlbum" );
		break;

	case NODE_TYPE_IDDB_GENRE:
		m_pwnd = new CID3ListView( NULL, CID3ListView::ID3LIST_GENRE, m_pMusicDB );
		m_pwnd->SetWindowTitle( "MusicGenre" );
		break;

	case NODE_TYPE_IDDB_ALLTRACK:
		m_pwnd = new CID3ListView( NULL, CID3ListView::ID3LIST_ALLTRACKS, m_pMusicDB );
		m_pwnd->SetWindowTitle( "MusicAllTrack" );
		break;

	default:
		return;

	}

	m_pwnd->Resize( CRect(0, 30, 319, 216), false );
	m_pwnd->SetFocus();
	m_pwnd->SetWindowList( AP_GetWndList() );
	m_pwnd->RegisterWndList();
	ClearKeyBuffer();
	mep_eat_event();
}

void CMusicMainWnd::OnPaint( const CRect& rc )
{
	CListView::OnPaint( rc );
	DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME );
}

void CMusicMainWnd::Show( void )
{
	g_TopPanel->SetTitle( ML_MUSIC_LIBRARY );
	g_TopPanel->Update();

	// back to player
	CListItem* pitem = m_uzList[0];
	if( (AP_GetPlayState() != AUDIO_STATE_STOP) ) { 
		if( pitem->m_nType != NODE_TYPE_BACKTOPLAYER ) {
    		CListItem* puzItem= new CListItem( -1, LII_MUSIC_U, LII_MUSIC_S, ML_NOW_PLAYING );
    		puzItem->m_nType = NODE_TYPE_BACKTOPLAYER;
    		puzItem->m_nIdx = m_uzList.Count();
    		puzItem->m_color = _YELLOWISH_GREEN;
    		m_uzList.PushHead( puzItem );
		}
	}
	else {
	    // Delete BACKTOPLAYER.
	    if( pitem->m_nType == NODE_TYPE_BACKTOPLAYER ) {
    	    m_uzList.Delete( 0 );
   	    }
	}
	
	CWindow::Show();

	SetListItem( (CListItem*)GetSelectedItem(), m_uzList.Count() );
}

/* FIN */
