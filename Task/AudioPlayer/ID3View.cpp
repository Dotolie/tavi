//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : ID3View class definition
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3View.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/05/24 03:19:05 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ID3View.cpp,v $
// Revision 1.3  2006/05/24 03:19:05  zstein
// - fixed artist name or album name with '/' bug.
//
// Revision 1.2  2006/03/10 02:48:52  zstein
// - fixed bug; displaying music name on the ext lcd.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.55  2006/01/19 08:43:25  zstein
// *** empty log message ***
//
// Revision 1.54  2006/01/16 02:38:28  zstein
// *** empty log message ***
//
// Revision 1.53  2005/12/27 09:44:45  zstein
// *** empty log message ***
//
// Revision 1.52  2005/12/22 04:28:12  zstein
// *** empty log message ***
//
// Revision 1.51  2005/12/16 12:16:01  yong
// JOG2_UP/DONW corresponds to VK_DOWN/UP!!!
//
// Revision 1.50  2005/12/14 11:34:57  yong
// external LCD
//
// Revision 1.49  2005/12/12 07:37:53  yong
// massive reworking abount sub LCD behaviour
//
// Revision 1.48  2005/12/05 04:57:52  zstein
// *** empty log message ***
//
// Revision 1.47  2005/12/01 09:25:53  zstein
// *** empty log message ***
//
// Revision 1.46  2005/12/01 01:04:08  zstein
// *** empty log message ***
//
// Revision 1.45  2005/11/25 12:47:37  zstein
// *** empty log message ***
//
// Revision 1.44  2005/11/25 10:21:58  zstein
// *** empty log message ***
//
// Revision 1.43  2005/11/24 03:02:04  zstein
// *** empty log message ***
//
// Revision 1.42  2005/11/16 07:13:57  zstein
// *** empty log message ***
//
// Revision 1.41  2005/11/11 12:28:47  zstein
// *** empty log message ***
//
// Revision 1.40  2005/11/10 01:09:39  zstein
// *** empty log message ***
//
// Revision 1.39  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
// Revision 1.36  2005/10/17 01:26:47  zstein
// - updated UI.
//*******************************************************************************
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <Task/Msg.h>
#include <Component/FileUtil.h>
#include <Task/tavi_global.h>
#include <Task/ExtLCD.h>
#include <Lib/LibImage.h>
#include "AudioCmd.h"
#include "ID3View.h"
#include "AudioPlayer.h"
#include "MP3HeaderInfo.h"
#include "ID3/libid3.h"
#include "musicdef.h"
#include <fatfs.h>

extern CTextEngine_BDF* pT;
extern CTopPanel* g_TopPanel;
extern CExtLCD* g_ExtLCD;

CID3ListView::CID3ListView( CWindow* parent, int type, CDeviceMusicDB* pDB ) :
	CListView( parent, TaviSysParam.sort ),
	m_nType( type ),
	m_pDB( pDB )
{
	Init();
}

CID3ListView::~CID3ListView( void )
{
	if( m_pMsg ) delete m_pMsg;

	if( m_nType == ID3LIST_ALLTRACKS ) {
		g_AllTrackList.Copy( m_uzList );
		m_uzList.FreeFake();
	}
}

void CID3ListView::Init( void )
{
	// create bottom window
	m_nOffset = 13;
	m_nDepth = 0;
	m_pMsg = 0;
	m_pCtxMenuWnd = new CContextMenu( this );
	m_pCtxMenuWnd->SetVisible( false );

	ScanDB();
}

int CID3ListView::GetIconIndex( int nType )
{
	int nIconIdx = -1;
	
	switch( nType ) {
	case ID3LIST_ARTIST:
		nIconIdx = LII_ARTIST_U;
		break;
	case ID3LIST_ALBUM:
		nIconIdx = LII_ALBUM_U;
		break;
	case ID3LIST_GENRE:
		nIconIdx = LII_GENRE_U;
		break;
	case ID3LIST_ALLTRACKS:
		nIconIdx = LII_MUSIC_U;
		break;
	}

	return nIconIdx;
}

int CID3ListView::ScanMusicFromGroup(GROUP_TYPE i_type) //  Artist, Album, Genre별로 구분된 그룹내의 뮤직을 얻는다 
{
	ST_DEVICEGROUPHASH *st_phash = NULL;
	int i_groupkey = m_pDB->Generate_Key( (uchar_t*)m_szS1 );
	WCHAR buf[MAX_PATH] = {0,};
	WCHAR base[] = { '/', 'h', 'd', '1', 0 };

	switch(i_type)
	{
	case ARTIST :
		st_phash = m_pDB->Get_ArtistHash(i_groupkey);
		break;
	case ALBUM:
		st_phash = m_pDB->Get_AlbumHash(i_groupkey);
		break;
	case GENRE :
		st_phash = m_pDB->Get_GenreHash(i_groupkey);
		break;
	}

	if(st_phash==NULL)return 0;

	ST_DEVICEMUSICINFO *st_info = st_phash->st_pfirstmusic;
	int i_count = st_phash->i_includemusiccount;
//	unsigned int i_musickey = st_phash->st_pfirstmusic->i_musickey;
	CID3ListItem* pItem;
	LPWSTR path;
	for(int i=0;i<i_count;i++)
	{
		path = (LPWSTR)st_info->st_musicfileinfo.s_nfilepath;
		pItem = new CID3ListItem( -1, LII_MUSIC_U, 0, 
			(char*)st_info->st_musicfileinfo.s_ntitle, true, path );
		pItem->m_nType = NODE_TYPE_FILE;
		m_uzList.Add( pItem );
		ustrcpy( buf, base );
		ustrcat( buf, path );
		pItem->m_nkey = CDeviceMusicDB::Generate_Key( buf );

		switch(i_type)
		{
		case ARTIST :
			//i_musickey = st_musicinfo->i_next_artist_musickey;
			st_info = ( ST_DEVICEMUSICINFO * )st_info->pnext_artist_music;
			break;
		case ALBUM:
			st_info = ( ST_DEVICEMUSICINFO * )st_info->pnext_album_music;
//			i_musickey = st_musicinfo->i_next_album_musickey;
			break;
		case GENRE :
			st_info = ( ST_DEVICEMUSICINFO * )st_info->pnext_genre_music;
//			i_musickey = st_musicinfo->i_next_genre_musickey;
			break;
		}
		if(st_info==NULL)break;
	}

	return i_count;
}

int CID3ListView::ScanAllArtist( void )
{
	void *plink = m_pDB->m_a_ArtistList.GetHeadPtr();
	CID3ListItem* pItem;

	LPWSTR s_name = NULL;

	s_name = (LPWSTR)m_pDB->m_a_ArtistList.Get_ItemFromPtrItem(plink);
	int i_count = m_pDB->m_a_ArtistList.GetCount();

	for(int i=0;i<i_count;i++)
	{
		if(s_name)
		{
//			여기서 s_name 을 쓴다.
			pItem = new CID3ListItem( -1, LII_ARTIST_U, 0, (char*)s_name, true );
			pItem->m_nType = NODE_TYPE_FOLDER;
			m_uzList.Add( pItem );
			
			plink = m_pDB->m_a_ArtistList.Get_NextPtrItem(plink);
			if(plink==NULL)break;
			s_name = (LPWSTR)m_pDB->m_a_ArtistList.Get_ItemFromPtrItem(plink);
		}
	}

	return i_count;
}

int CID3ListView::ScanAlbumFromArtist( void )
{
	m_pDB->Get_AlbumListFromArtist( (uchar_t*)m_szS1 );

	int count =(int)m_pDB->m_a_CurrentAlbumList.GetCount();
	CID3ListItem* pItem;
	LPWSTR szName;
	for(int i=0;i<count;i++)
	{
		unsigned int *i_palbumkey = (unsigned int *)m_pDB->m_a_CurrentAlbumList.GetAt(i);
		szName = m_pDB->Get_AlbumName(*i_palbumkey);
		pItem = new CID3ListItem( -1, LII_ALBUM_U, 0, (char*)szName, true );
		pItem->m_nType = NODE_TYPE_FOLDER;
		m_uzList.Add( pItem );
	}

	return count;
}

int CID3ListView::ScanMusicFromAA( void )
{
	CID3ListItem* pItem;
	unsigned int  i_artistkey = m_pDB->Generate_Key((uchar_t*)m_szS1);
	unsigned int i_album = m_pDB->Generate_Key((uchar_t*)m_szS2);
	ST_DEVICEGROUPHASH *st_phash = NULL;
	WCHAR buf[MAX_PATH] = {0,};
	WCHAR base[] = { '/', 'h', 'd', '1', 0 };
	
	st_phash = m_pDB->Get_ArtistHash(i_artistkey);

	if(st_phash==NULL) return 0;

	ST_DEVICEMUSICINFO *st_musicinfo = st_phash->st_pfirstmusic;
	int i_count = st_phash->i_includemusiccount;
	char* szName;
	LPWSTR path;
	int match=0;
	for(int i=0;i<i_count;i++)
	{
		//st_musicinfo = m_pDB->Get_Music(i_musickey);
		if(st_musicinfo->i_albumkey == i_album)
		{
			szName = (char*)st_musicinfo->st_musicfileinfo.s_ntitle;
			path = st_musicinfo->st_musicfileinfo.s_nfilepath;
			pItem = new CID3ListItem( -1, LII_MUSIC_U, 0, (char*)szName, true, path );
			pItem->m_nType = NODE_TYPE_FILE;
			m_uzList.Add( pItem );
			match++;
			ustrcpy( buf, base );
			ustrcat( buf, path );
			pItem->m_nkey = CDeviceMusicDB::Generate_Key( buf );
		}

		//i_musickey = st_musicinfo->i_next_artist_musickey;
		//if(i_musickey==0)break;
		st_musicinfo = ( ST_DEVICEMUSICINFO* )st_musicinfo->pnext_artist_music;
	}		

	return match;
}

int CID3ListView::ScanArtist( void )
{
	int count=0;

	switch( m_nDepth ) {
	case 0:
		count = ScanAllArtist();
		break;
	case 1:
		count = ScanAlbumFromArtist();
		break;
	case 2:
		count = ScanMusicFromAA();
		break;
	}

	return count;
}

int CID3ListView::ScanAllAlbum( void )
{
	void *plink = m_pDB->m_a_AlbumList.GetHeadPtr();
	CID3ListItem* pItem;

	LPWSTR s_name = NULL;
	s_name = (LPWSTR)m_pDB->m_a_AlbumList.Get_ItemFromPtrItem(plink);

	int i_count = m_pDB->m_a_AlbumList.GetCount();

	for(int i=0;i<i_count;i++)
	{
		if(s_name)
		{
//			여기서 s_name 을 쓴다.
			pItem = new CID3ListItem( -1, LII_ALBUM_U, 0, (char*)s_name, true );
			pItem->m_nType = NODE_TYPE_FOLDER;
			m_uzList.Add( pItem );
			
			plink = m_pDB->m_a_AlbumList.Get_NextPtrItem(plink);
			if(plink==NULL)break;
			s_name = (LPWSTR)m_pDB->m_a_AlbumList.Get_ItemFromPtrItem(plink);
		}
	}

	return i_count;
}


int CID3ListView::ScanMusicFromAlbum( void )
{
	return ScanMusicFromGroup( ALBUM );
}

int CID3ListView::ScanAlbum( void )
{
	int count;
	switch( m_nDepth ) {
	case 0:
		count = ScanAllAlbum();
		break;
	case 1:
		count = ScanMusicFromAlbum();
		break;
	}
	return count;
}

int CID3ListView::ScanAllGenre( void )
{
	void *plink = m_pDB->m_a_GenreList.GetHeadPtr();
	CID3ListItem* pItem;

	LPWSTR s_name = NULL;
	s_name = (LPWSTR)m_pDB->m_a_GenreList.Get_ItemFromPtrItem(plink);

	int i_count = m_pDB->m_a_GenreList.GetCount();

	for(int i=0;i<i_count;i++)
	{
		if(s_name)
		{
//			여기서 s_name 을 쓴다.
			pItem = new CID3ListItem( -1, LII_GENRE_U, 0, (char*)s_name, true );
			pItem->m_nType = NODE_TYPE_FOLDER;
			m_uzList.Add( pItem );
			
			plink = m_pDB->m_a_GenreList.Get_NextPtrItem(plink);
			if(plink==NULL)break;
			s_name = (LPWSTR)m_pDB->m_a_GenreList.Get_ItemFromPtrItem(plink);
		}
	}

	return i_count;
}

int CID3ListView::ScanMusicFromGenre( void )
{
	return ScanMusicFromGroup( GENRE );
}

int CID3ListView::ScanGenre( void )
{
	int count;
	switch( m_nDepth ) {
	case 0:
		count = ScanAllGenre();
		break;
	case 1:
		count = ScanMusicFromGenre();
		break;
	}
	return count;
}

int CID3ListView::ScanAllSongs( void )
{
	int i_count;
	if( (i_count=g_AllTrackList.Count()) ) {
            printf( "total songs; %d\n", i_count );
		m_uzList.Copy( g_AllTrackList );
	}
	else {
		void *plink = m_pDB->m_a_MusicInfo.GetHeadPtr();

		ST_DEVICEMUSICINFO *st_musicinfo = NULL;
		st_musicinfo = (ST_DEVICEMUSICINFO *)m_pDB->m_a_MusicInfo.Get_ItemFromPtrItem(plink);
		WCHAR buf[MAX_PATH] = {0,};
		WCHAR base[] = { '/', 'h', 'd', '1', 0 };

		i_count = m_pDB->m_a_MusicInfo.GetCount();
		CID3ListItem* pItem;
		LPWSTR path;
		for(int i=0;i<i_count;i++)
		{
			if(st_musicinfo)
			{
				path = st_musicinfo->st_musicfileinfo.s_nfilepath;
				pItem = new CID3ListItem( 
					-1, LII_MUSIC_U, 0, (char*)st_musicinfo->st_musicfileinfo.s_ntitle, 
					true, path );
				pItem->m_nType = NODE_TYPE_FILE;
				m_uzList.Add( pItem );
				ustrcpy( buf, base );
				ustrcat( buf, path );
				pItem->m_nkey = CDeviceMusicDB::Generate_Key( buf );
				plink = m_pDB->m_a_MusicInfo.Get_NextPtrItem(plink);
				if(plink==NULL)break;
				st_musicinfo = (ST_DEVICEMUSICINFO *)m_pDB->m_a_MusicInfo.Get_ItemFromPtrItem(plink);
			}
		}
	}

	return i_count;
}

int CID3ListView::ScanDB( void )
{
	m_uzList.FreeAll();

	if( !m_pDB ) return 0;

	int count = 0;
	switch( m_nType ) {
	case ID3LIST_ARTIST:
		count = ScanArtist();
		break;
	case ID3LIST_ALBUM:
		count = ScanAlbum();
		break;
	case ID3LIST_GENRE:
		count = ScanGenre();
		break;
	case ID3LIST_ALLTRACKS:
		count = ScanAllSongs();
		break;
	}
	Sort( TaviSysParam.sort );

    #if 0
	CID3ListItem* pItem = (CID3ListItem*)GetSelectedItem();

	if ( pItem ) {
		CExtLCD::ExtLCDListItem item;
		item.m_nIcon = (pItem->m_nType == NODE_TYPE_FOLDER) ?
				CExtLCD::EXT_LCD_ICON_FOLDER : CExtLCD::EXT_LCD_ICON_MUSIC;
		item.m_nIndex = GetSelectedIndex();
		item.m_nTotal = GetCount();
		item.m_szText = pItem->m_szText;
		g_ExtLCD->SetListItem( item );
		if( g_MiscCtrl->FolderClosed() ) g_ExtLCD->Update();
	}
    #endif
	#if 0
	if( m_uzList.Count() == 0 ) {
		m_pMsg = new CMsgBox( ML_INFORMATION, _ORANGE, ML_NO_MUSIC, 244, 123, 0, DL_BG_ICON );
		m_pMsg->Show();
		gettimeofday( &m_tv, NULL );
		mep_eat_event();
		return false;
	}
	#endif

	return true;
}

void CID3ListView::MenuDown( void )
{
	m_nDepth++;
	CID3ListItem* pItem = (CID3ListItem*)GetSelectedItem();
	StackItem item = { m_nStartIdx, m_nSelectedIdx };
	m_uzStack.Push( item );
	m_uzTitleStack.Push( pItem->m_szText );
	if( m_nDepth == 1 ) m_szS1 = pItem->m_szText;
	else if( m_nDepth == 2 ) m_szS2 = pItem->m_szText;
	m_nStartIdx = 0;
	m_nSelectedIdx = 0;
	ScanDB();
}

bool CID3ListView::MenuUp( void )
{
	m_nDepth--;
	if( m_uzStack.IsEmpty() ) {
		m_nStartIdx = 0;
		m_nSelectedIdx = 0;
	}
	else {
		StackItem item = m_uzStack.Pop();
		m_nStartIdx = item.m_nStartIdx;
		m_nSelectedIdx = item.m_nSelectedIdx;
	}
	ScanDB();

	return true;
}

void CID3ListView::OnPaint( const CRect& rc )
{
	int key;
	
	// show path in the top panel.
	if( m_nDepth == 0 ) {
		int id;
		switch( m_nType ) {
		case ID3LIST_ARTIST: id = ML_ARTIST; break;
		case ID3LIST_ALBUM: id = ML_ALBUM; break;
		case ID3LIST_GENRE: id = ML_GENRE; break;
		case ID3LIST_ALLTRACKS: id = ML_ALL_TRACK; break;
		}
		g_TopPanel->SetTitle( id );
	}
	else {
		g_TopPanel->SetTitle( m_uzTitleStack.Pop() );
	}
	g_TopPanel->Update();

    // Draw Ext. LCD
    SetListItem( (CListItem*)GetSelectedItem(), m_uzList.Count() );
     
	if( m_uzList.Count() == 0 ) {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), 0 );
		pT->DrawResText( ML_NO_FILES, 
			320/2 - (pT->GetStringWidth(ML_NO_FILES, 12, DT_TRANSPARENCY|DT_OUTLINE)/2), 
			110, _WHITE, 12, DT_TRANSPARENCY|DT_OUTLINE );
		DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME| BGPID_MENU );
		return;
	}
	
	ResetScrollBar();
	
	// check a playing file
	CID3ListItem* pitem;
	for( int i=0; i<m_uzList.Count(); i++ ) {
		pitem = (CID3ListItem*)m_uzList[i];
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
	CListView::OnPaint( rc );
	DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME| BGPID_MENU );
}

void CID3ListView::OnKeypressed( const KeyEvent& evt )
{
	CID3ListItem* puzItem;
	KeyEvent kevt = evt;

	if( m_pMsg ) {
		delete m_pMsg;
		m_pMsg = NULL;
		SendMessage( WINDOW_KILL );
		return;
	}
	
	puzItem = (CID3ListItem*)GetSelectedItem();
	switch( kevt.value ) {

	case TAVI_KEY_JOG2_DOWN:
		if( g_MiscCtrl->FolderOpen() ) {
			AP_Prev();
			break;
		} else {
			kevt.value = VK_UP;
		}
		break;

	case TAVI_KEY_JOG2_UP:
		if( g_MiscCtrl->FolderOpen() ) {
			AP_Next();
			break;
		} else {
			kevt.value = VK_DOWN;
		}
		break;

	case TAVI_KEY_JOG1_ENTER:
		if( g_MiscCtrl->FolderOpen() ) break;
		if( g_ExtLCD->GetCurrentMode() == CExtLCD::EXT_STAGE_VOLUME ) {
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
			break;
		}

	case VK_ESC: // Ver 0.9.9.0이후 수정
	case VK_LEFT:
		if( m_nDepth == 0 ) {
			SendMessage( WINDOW_KILL );
			return;
		} else {
			if( MenuUp() == false ) 	return;
			OnPaint( rcClient );
			Flush();
		}
		break;

	case TAVI_KEY_JOG2_ENTER:
		if( g_MiscCtrl->FolderOpen() ) {
			if( AP_GetPlayState() == AUDIO_STATE_PLAYING ) {
				AP_Pause();
			} else {
				AP_Resume();
			}
			break;
		}

	case VK_RIGHT:
	case VK_ENTER:
		if( m_uzList.Count() == 0 ) return;

		if( puzItem->m_nType == NODE_TYPE_FOLDER ) {
			MenuDown();
			OnPaint( rcClient );
			Flush();
		} else {
			CID3ListItem* pItem = (CID3ListItem*)GetSelectedItem();
			if( evt.value == VK_RIGHT || 
				evt.value == TAVI_KEY_JOG2_UP ) break;
			CString path;
			if( m_nType == ID3LIST_ARTIST ) {
				path = m_szS1;
				path.Add( (uchar_t*)"\01\0\0" );
				path.Add( m_szS2 );
			}
			else if( m_nType == ID3LIST_ALBUM ) {
				path = m_szS1;
			}
			else if( m_nType == ID3LIST_GENRE ) {
				path = m_szS1;
			}
			else {
				path = "All tracks";
			}
			CString filepath;
			filepath.SetString( (char*)pItem->m_szPath, true );
			AP_Play( TYPE_MUSICDB |m_nType, path, m_pDB, filepath ); // Send play command
			return;
		}
		break;
	// Ver. 0.9.9.0
	//case VK_ESC:
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
	case VK_F2: // Menu
	        if( m_uzList.Count() ) {
        		CreateContextMenu();
        		m_pCtxMenuWnd->SetFocus();
               }
		break;
	case TAVI_KEY_JOG1_UP: 
		AP_Volume( 1 ); 
		break;
	case TAVI_KEY_JOG1_DOWN: 
		AP_Volume( 0 ); 
		break;
	}

	CListView::OnKeypressed( kevt );

    SetListItem( (CListItem*)GetSelectedItem(), m_uzList.Count() );

    // Clear Key buffer
    ClearKeyBuffer();
}

void CID3ListView::TableToPlaylist( M3UHandle handle, const char* cmp1, const char* cmp2 )
{
	#if 0
	char* szPath;
	ID3Tag* pTag;
	M3UTag m3u;
	MP3HdrInfo mp3info;

	uzDBHandle = ID3DB_GetHandle( ID3DB_FILE );

	lbKey = gdbm_firstkey( uzDBHandle );
	while( lbKey.dptr ) {
		lbData = gdbm_fetch( uzDBHandle, lbKey );
		if( lbData.dptr == NULL ) break;
		pTag = ( ID3Tag* )lbData.dptr;
		if( cmp1 ) {
			if( strcmp( cmp1, pTag->artist ) ) goto __next;
			if( cmp2 ) {
				if( strcmp(cmp2, pTag->album) ) goto __next;
			}
		}
		szPath = ( char* )lbKey.dptr;
		if( mp3info.Decode( szPath ) ) {
			m3u.m_nTime = mp3info.m_nTotalTime;
			m3u.m_szTitle = pTag->title;
			m3u.m_szPath = szPath;
			M3UAdd( handle, &m3u );
		}
		
__next:
		free( lbData.dptr );
		lbData = gdbm_nextkey( uzDBHandle, lbKey );
		free( lbKey.dptr );
		lbKey.dptr = NULL;
		lbKey = lbData;
	}

	if( lbKey.dptr ) free( lbKey.dptr );
	if( lbData.dptr ) free( lbData.dptr );
	#endif
}

void CID3ListView::DBtoPlaylist( M3UHandle handle )
{
	#if 0
	char *sz1 = NULL;
	char* sz2 = NULL;
	if( m_nType == ID3LIST_ARTIST ) {
		StackItem item = m_uzStack->Peek();
		sz1 = ( char* )item.m_szArtist;
		sz2 = ( char* )GetSelectedItem()->m_szText;
	}
	else if( m_nType == ID3LIST_ALBUM ) {
		sz1 = ( char* )GetSelectedItem()->m_szText;
	}
	else if( m_nType == ID3LIST_GENRE ) {
		sz1 = ( char* )GetSelectedItem()->m_szText;
	}
	else if( m_nType == ID3LIST_ALLTRACKS ) {
		sz1 = sz2 = NULL;
	}
	TableToPlaylist( handle, sz1, sz2 );
	#endif
}

void CID3ListView::OnWindowEvent( const WindowEvent& evt )
{
	struct timeval tv;
	switch( evt.nMessage ) {
	case WINDOW_IDLE:
		if( g_MiscCtrl->FolderOpen() ) {
			if( g_pFocusedWnd == this ) ScrollText();
		} else {
			g_ExtLCD->ScrollText();
		}
		if( m_pMsg ) {
			gettimeofday( &tv, NULL );
			if( abs(tv.tv_sec-m_tv.tv_sec) > 3 ) {
				delete m_pMsg;
				m_pMsg = NULL;
				if( m_nDepth == 0 ) SendMessage( WINDOW_KILL );
			}
		}
		// Hide touch menu after 5 secs.
		break;
	case ON_FOLDER:
		if( evt.nParam ) { // open
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
		}
		else { // close
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
		}
		g_ExtLCD->Update();
		break;
	}
}

void CID3ListView::OnMouseEvent( const MouseEvent& evt )
{
	CListView::OnMouseEvent( evt );
}

void CID3ListView::Show( void )
{
    extern int handle_key;
    
    CWindow::Show();
    ClearKeyBuffer();
    mep_eat_event();

    ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
    ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );
}

static int NameAscentSort( RMuint8* arg )
{
	CID3ListView* pView = ( CID3ListView* )arg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	if( TaviSysParam.sort != CListView::NAME_ASCENT ) {
		TaviSysParam.sort = CListView::NAME_ASCENT;
		pView->Sort( CListView::NAME_ASCENT );
		pView->Update();
		pView->m_uzStack.Reset( 10 );
	}

	return 0;
}

static int NameDescentSort( RMuint8* arg )
{
	CID3ListView* pView = ( CID3ListView* )arg;
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
	if( TaviSysParam.sort != CListView::NAME_DESCENT ) {
		TaviSysParam.sort = CListView::NAME_DESCENT;
		pView->Sort( CListView::NAME_DESCENT );
		pView->Update();
		pView->m_uzStack.Reset( 10 );
	}

	return 0;
}

static void FileToPlaylist( M3UHandle hM3U, CID3ListItem* item )
{
	M3UTag tag;

	tag.m_nTime = 0;
	tag.m_szPath = ( char* )item->m_szPath;
	tag.m_nPathLen = ustrlen(item->m_szPath)*2;
	tag.m_szTitle = ( char* )item->m_szText;
	tag.m_nTitleUnicode = 1;
	tag.m_nTitleLen = item->m_szText.GetSize()*2;
	M3UAdd( hM3U, &tag );
}

int CID3ListView::AddToPlaylist( RMuint8* arg )
{
	CID3ListView* pView = ( CID3ListView* )arg;
	M3UHandle hM3U;
	CString playlist = INSTANCE_PLAYLIST;

	playlist.toUTF16();
	hM3U = M3UInit( (uchar_t*)playlist, M3U_APPEND );
	if( hM3U == 0 ) {
		hM3U = M3UCreate( (uchar_t*)playlist );
		M3UClose( hM3U );
		hM3U = M3UInit( (uchar_t*)playlist, M3U_APPEND );
	}
	if( !hM3U ) {
		TAVI_DEBUGMSG( "failed to open M3U file!!\n" );
		// show error message
		return -1;
	}
	CID3ListItem* pitem = (CID3ListItem*)pView->GetSelectedItem();

	CString path;
	if( pitem->m_nType == NODE_TYPE_FOLDER ) {
		//path = pView->m_szCurrentDir + "/" + pitem->m_szText;
		//FolderToPlaylist( pView->m_hDB, hM3U );
	}
	else {
		//path = pView->m_szCurrentDir;
		//path.Add( (uchar_t*)"/\0\0\0" );
		//path.Add( pitem->m_szText );
		CID3ListItem* pitem = (CID3ListItem*)pView->GetSelectedItem();
		FileToPlaylist( hM3U, pitem );
	}

	M3UClose( hM3U );
	pView->m_pCtxMenuWnd->SetVisible( false );
	pView->m_pCtxMenuWnd->Hide();
}

void CID3ListView::CreateContextMenu( void )
{
	int nSelect = TaviSysParam.sort;
	CID3ListItem* pitem = (CID3ListItem*)GetSelectedItem();

	m_pCtxMenuWnd->Reset();
	CPopUpSubMenu* pSubMenu = new CPopUpSubMenu;
	if( pitem->m_nType == NODE_TYPE_FILE ) {
		m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_ADD_TO_IPLAYLIST, NULL, -1, AddToPlaylist, (RMuint8*)this );
	}
	pSubMenu->AddSubMenuItem( ML_NAME_ASCENT, 0, NameAscentSort, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_NAME_DESCENT, 0, NameDescentSort, (RMuint8*)this );
	//pSubMenu->AddSubMenuItem( ML_BACK, FALSE, NULL, NULL );
	pSubMenu->PointedMenuItemIdx = nSelect;
	m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_SORTING_METHOD, pSubMenu, nSelect, NULL, NULL );
	//m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );
}

/* FIN */
