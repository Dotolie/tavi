//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : PlaylistView class definition
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/PlayListView.cpp,v $
// $Revision: 1.7 $
// $Date: 2006/06/22 01:39:42 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: PlayListView.cpp,v $
// Revision 1.7  2006/06/22 01:39:42  zstein
// - added 'delete playlist' menu.
//
// Revision 1.6  2006/06/08 06:49:12  zstein
// - skip invalid playlist file.
//
// Revision 1.5  2006/05/10 02:28:02  zstein
// - fixed undeleted playlist item bug.
//
// Revision 1.4  2006/04/19 09:32:23  zstein
// - fixed bug.
//
// Revision 1.3  2006/04/15 01:25:34  zstein
// - changed instance playlist to i-Playlist.
//
// Revision 1.2  2006/03/07 02:38:26  zstein
// - fixed a bug that do not showing a playing filename on EXT LCD.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*******************************************************************************

#include <sys/time.h>
#include <unistd.h>

#include <Component/FileUtil.h>
#include <Task/tavi_global.h>
#include <Task/Msg.h>
#include <Task/TopPanel.h>
#include <Task/ExtLCD.h>
#include "AudioPlayer.h"
#include "ID3/m3u.h"
#include "ID3/IDDB.h"
#include "PlayListView.h"
#include "PlayerWnd.h"
#include "musicdef.h"

#define PLAYLIST_EXT	".TPL"

// Extern Variables
extern CTopPanel* g_TopPanel;
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CTextEngine_BDF* pT;
extern CExtLCD* g_ExtLCD;
// Local Variables

static int prevStartIdx;
static int prevSelectedIdx;

class CPLListItem : public CListItem
{
public:
	CString m_szPath;
	CPLListItem( int nIdx, int nIconIdx, int nSelectedIconIdx, char* szText, bool fUnicode=false ) : 
		CListItem( nIdx, nIconIdx, nSelectedIconIdx, szText, fUnicode ) { }
	~CPLListItem() {}
};

CPlaylistView::CPlaylistView( CWindow* parent ) :
	CListView( parent )
{
	m_base = PLAYLIST_BASE;
	m_base.toUTF16();
	Init();
}

CPlaylistView::~CPlaylistView( void )
{
	// NOTE: DO NOT delete pBottomWnd
	// CWindow::~CWindow() will delete her children.
	if( m_pMsg ) delete m_pMsg;
}

void CPlaylistView::Init( void )
{
	m_nOffset = 13;
	m_nListCount = 8;
	m_nDepth = 0;
	m_pMsg = 0;
	m_instance = false;

	m_pCtxMenuWnd = new CContextMenu( this );
	m_pCtxMenuWnd->SetVisible( false );
	ScanPlaylist();
}

bool CPlaylistView::ScanPlaylist( void )
{
	FATFS_ERROR err;
	FATFS_DIR_ENTRY_INFO entry;
	RMuint32 fd;
	int nMedia;
	CPLListItem* pitem;
	uchar_t* szFullPath;
	char* filename;

  TAVI_WakeUp( TASK_ID_MUSIC );
    
	szFullPath = new uchar_t[1024];
	m_uzList.FreeAll();
	if( m_nDepth == 0 ) {
    char u8filename[MAX_PATH];
	    
		err = fatfs_dopen( (RMuint8*)PLAYLIST_BASE, &fd );
		#if 0
		// If there is no playlist directory, create.
		if( err == FATFS_ERROR_DIR_NAME_NOT_FOUND ) {
			err = fatfs_dopen( (RMuint8*)_AUDIO_DIR, &fd );
			err = fatfs_dcreate_dir( fd, (RMuint8*)PLAYLIST_DIR );
			fatfs_dclose( fd );
			err = fatfs_dopen( (RMuint8*)PLAYLIST_BASE, &fd );
		}
		#endif
		if ( err != FATFS_ERROR_NO_ERROR ) {
			goto __scan_done;
		}
		while( (err=fatfs_dread(fd, &entry)) == FATFS_ERROR_NO_ERROR ) {
			if( ustrcmp((uchar_t*)entry.name, (uchar_t*)".\0") == 0 ) continue;
			if( ustrcmp((uchar_t*)entry.name, (uchar_t*)".\0.\0") == 0 ) continue;

			if( entry.attributes&0x10 ) continue;
			else {
				nMedia = CheckMedia(entry.name, true);
				switch( nMedia ) {
				case MEDIA_TPL:
					break;
				default:
					continue;
				}
			}

      ustr2str( u8filename, (uchar_t*)entry.name );

      if( !strcasecmp( u8filename, INSTANCE_PLAYLIST_NAME ) ) {
          filename = (char*)pT->GetTextRes( ML_INSTANT_PLAYLIST );
      }
      else {
          filename = GetFilename((char*)entry.name, true);
      }
            
			pitem = new CPLListItem( 
			                -1, 
			                LII_PLAYLIST_U, 
			                LII_PLAYLIST_S, 
			                filename, 
			                true );
			pitem->m_nType = NODE_TYPE_FOLDER;
			AddItem( pitem );
		}
		fatfs_dclose( fd );
	}
	else {
		bool instance = false;
		uchar_t* path = new uchar_t[1024];
		uchar_t ext[10];
		if( ustrcasecmp( 
		        pT->GetTextRes(ML_INSTANT_PLAYLIST), 
		        (uchar_t*)m_szPlaylistname ) == 0 ) {
		    m_instance = true;
	    }
	    else {
	        m_instance = false;
	    }

		// make path
		str2ustr( ext, PLAYLIST_EXT, 4 );
		str2ustr( path, PLAYLIST_BASE, strlen(PLAYLIST_BASE) );
		if( m_instance ) {
      uchar_t iplaylist[100];
      str2ustr( iplaylist, INSTANCE_PLAYLIST_NAME );
      ustrcat( path, iplaylist );
 		}
 		else {
   		ustrcat( path, (uchar_t*)m_szPlaylistname );
  		ustrcat( path, ext );
 		}
		M3UHandle handle = M3UInit( path, M3U_READ );
		delete[] path;
		if( !handle ) goto __scan_done;
		M3UTag* ptag;
		while( (ptag=M3UGetNext(handle)) ) {
			pitem = new CPLListItem( -1, LII_MUSIC_U, LII_MUSIC_S, ptag->m_szTitle, true );
			pitem->m_nType = NODE_TYPE_FILE;
			pitem->m_nDate = ptag->m_nTime;
			pitem->m_szPath.SetString( ptag->m_szPath, true );
			str2ustr( szFullPath, _HDD_ROOT );
			ustrcat( szFullPath, (uchar_t*)ptag->m_szPath );
			pitem->m_nkey = CDeviceMusicDB::Generate_Key( (LPWSTR)szFullPath );
			AddItem( pitem );
			M3UFreeTag( &ptag );
		}
		M3UClose( handle );
	}

__scan_done:
	delete[] szFullPath;

	ResetScrollBar(); 

  TAVI_CanSleep( TASK_ID_MUSIC );

	return true;
}

void CPlaylistView::OnKeypressed( const KeyEvent& evt )
{
	CPLListItem* puzItem;
	KeyEvent kevt = evt;
	int len;

	puzItem = ( CPLListItem* )GetSelectedItem();
	if( m_pMsg ) {
		delete m_pMsg;
		m_pMsg = NULL;
		if( m_nDepth == 0 ) {
			SendMessage( WINDOW_KILL );
			return;
		}
		kevt.value = VK_LEFT;
	}

	switch( kevt.value ) {

	case TAVI_KEY_JOG1_UP:
		AP_Volume( 1 );
		break;

	case TAVI_KEY_JOG1_DOWN:
		AP_Volume( 0 );
		break;

	case TAVI_KEY_JOG2_UP:
		if( g_MiscCtrl->FolderOpen() ) {
			AP_Next();
			break;
		} else {
			kevt.value = VK_DOWN;
		}
		break;

	case TAVI_KEY_JOG2_DOWN:
		if( g_MiscCtrl->FolderOpen() ) {
			AP_Prev();
			break;
		} else {
			kevt.value = VK_UP;
		};
		break;

	case TAVI_KEY_JOG2_ENTER:
		if( g_MiscCtrl->FolderOpen() ) {
			if( AP_GetPlayState() == AUDIO_STATE_PAUSE )
				AP_Resume();
			else
				AP_Pause();
			break;
		}
	case VK_ENTER:
	case VK_RIGHT:
		//puzItem = GetSelectedItem();
		if( !puzItem ) {
		    return;
		}

		if( m_nDepth == 1 ) {
			if( kevt.value == VK_RIGHT ) {
			    break;
			}
			if( !puzItem ) {
			    return;
			}
			if( m_instance ) {
			    CString path = IPLAYLIST_NAME;
			    path.toUTF16();
			    AP_Play( TYPE_PLAYLIST, path, NULL, puzItem->m_szText );
			}
			else {
    			AP_Play( TYPE_PLAYLIST, m_szPlaylistname, NULL, puzItem->m_szText );
    		}
		} else {
			m_nDepth++;
			m_szPlaylistname = puzItem->m_szText;
			prevStartIdx = m_nStartIdx;
			prevSelectedIdx = m_nSelectedIdx;
			m_nStartIdx = 0;
			m_nSelectedIdx = 0;
			if( ScanPlaylist() == false ) return;
			OnPaint( rcClient );
		}
		return;

	case TAVI_KEY_JOG1_ENTER:
		if( g_MiscCtrl->FolderOpen() ) break;
        if( g_ExtLCD->GetCurrentMode() == CExtLCD::EXT_STAGE_VOLUME ) {
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
			break;
		}

	case VK_ESC: // Ver 0.9.9.0 이후 수정사항
	case VK_LEFT:
		if( m_nDepth == 0 ) {
			// Exit playlist viewer
			SendMessage( WINDOW_KILL );
			return;
		}
		m_nDepth--;
		m_uzList.FreeAll();
		m_nStartIdx = prevStartIdx;
		m_nSelectedIdx = prevSelectedIdx;
		ScanPlaylist();
		OnPaint( rcClient );
		break;

	case VK_F2: // Menu
		CreateContextMenu();
		m_pCtxMenuWnd->SetFocus();
		break;

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

	CListView::OnKeypressed( kevt );

    // draw ext. lcd
    SetListItem( (CListItem*)GetSelectedItem(), m_uzList.Count() );

    // Clear Key buffer
    ClearKeyBuffer();
}

void CPlaylistView::OnPaint( const CRect& rc )
{
    // EXT. LCD
   SetListItem( (CListItem*)GetSelectedItem(), m_uzList.Count() );

    // No items.
    if( m_uzList.Count() == 0 ) {
        g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), 0 );
        pT->DrawResText( ML_NO_FILES, 
        	320/2 - (pT->GetStringWidth(ML_NO_FILES, 12, DT_TRANSPARENCY|DT_OUTLINE)/2), 
        	110, _WHITE, 12, DT_TRANSPARENCY|DT_OUTLINE );
        DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME| BGPID_MENU );
        Flush();

        return;
    }

	if( m_nDepth == 1 ) g_TopPanel->SetTitle( m_szPlaylistname );
	else g_TopPanel->SetTitle( ML_PLAYLIST );

	g_TopPanel->Update();

	// CHECK PLAYING FILE
	CListItem* pitem;
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
	if( m_nDepth ) {
		DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME| BGPID_MENU );
	}
	else {
		DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME );
	}
	CListView::OnPaint( rc );
}

/* CPlaylistView::Flickering( bool fShow )
 * @fShow: If true then shows text, otherwise hide text
 */
void CPlaylistView::Flickering( bool fShow )
{
	CPixmap* pixmap;
	CListItem* puzItem = GetSelectedItem();
	int idx = m_nSelectedIdx;
	
	op_BitBlt( &g_SysPixmap, rcABS.x1+2+m_nOffset, rcABS.y1+2+idx*m_puzBarPixmap->m_nHeight,
			m_puzBarPixmap, 0, 0, m_puzBarPixmap->m_nWidth, m_puzBarPixmap->m_nHeight );
	pixmap = m_uzIconList[puzItem->m_nSelectedIconIdx];
	op_BitBlt( &g_SysPixmap, rcABS.x1+m_nOffset+9, rcABS.y1+7+idx*m_puzBarPixmap->m_nHeight,
				pixmap, 0, 0, pixmap->m_nWidth, pixmap->m_nHeight );

	// Show or Hide text
	if( fShow ) {
		#if 0
		int len = puzItem->m_szText.GetSize();
		uchar_t* pwcString = puzItem->m_szText.ConvertWideString();
		int nBoundaryLen = pT->GetBoundaryLength( pwcString, len, 12, m_puzBarPixmap->m_nWidth-34 );
		int x = rcABS.x1+30+m_nOffset;
		for( int i=0; i<nBoundaryLen; i++ ) {
			x += pT->DrawCharacter( pwcString[i], 
				x, rcABS.y1+7+idx*m_puzBarPixmap->m_nHeight,
				_BLACK, 12, DT_TRANSPARENCY );
		}
		#endif
	}

	Flush();
}

void CPlaylistView::OnWindowEvent( const WindowEvent& evt )
{
	struct timeval tv;

	switch( evt.nMessage ) {

	case WINDOW_IDLE:
		if( g_MiscCtrl->FolderOpen() ) {
			if( g_pFocusedWnd == this ) ScrollText();
		} else {
			g_ExtLCD->ScrollText();
		}

		gettimeofday( &tv, NULL );

		if( m_pMsg ) {
			if( abs(tv.tv_sec-m_tv.tv_sec) > 3 ) {
				delete m_pMsg;
				m_pMsg = NULL;
				if( m_nDepth == 0 ) {
					SendMessage( WINDOW_KILL );
				} else {
					KeyEvent evt = { VK_LEFT, 1 };
					OnKeypressed( evt );
				}
			}
		}
		break;

	case ON_FOLDER:
#if 0
		if( evt.nParam ) {	/* Folder is open */
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
		} else {
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
		}
		g_ExtLCD->Update();
#endif
		break;
	}
}

void CPlaylistView::Show( void )
{
	CListView::Show();
	ClearKeyBuffer();
	mep_eat_event();
	
	extern int handle_key;
	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );
}

static void ShowMessage( const char* msg )
{
	uchar_t* sz = new uchar_t[100];
	str2ustr( sz, (char*)msg, strlen(msg) );
	CMsgBox* pMsg = new CMsgBox( ML_INFORMATION, _ORANGE, sz, 244, 123, 0, DL_BG_ICON );
	pMsg->Show();
	struct timeval tv1, tv2;
	gettimeofday( &tv1, NULL );
	while( 1 ) {
		gettimeofday( &tv2, NULL );
		if( abs(tv2.tv_sec-tv1.tv_sec) > 1 ) break;
	}
	delete pMsg;
}

static void SavePlaylist( CArray<CListItem*>& list, uchar_t* path ) 
{
    TAVI_WakeUp( TASK_ID_MUSIC );
    
	M3UHandle h = M3UCreate( path );
	if( h ) {
		int n = list.Count();
		CPLListItem* pitem;
		M3UTag tag;
		for( int i=0; i<n; i++ ) {
			pitem = ( CPLListItem* )list[i];
			tag.m_szTitle = ( char* )pitem->m_szText;
			tag.m_nTitleLen = pitem->m_szText.GetSize();
			tag.m_nTitleUnicode = pitem->m_szText.IsUnicode();
			if( pitem->m_szText.IsUnicode() ) tag.m_nTitleLen *= 2;
			tag.m_szPath = ( char* )pitem->m_szPath;
			tag.m_nPathLen = pitem->m_szPath.GetSize();
			tag.m_nTime = pitem->m_nDate;
			if( pitem->m_szPath.IsUnicode() ) tag.m_nPathLen *= 2;
			M3UAdd( h, &tag );
		}
		M3UClose( h );
	}
	else {
		ShowMessage( "Failed to save a playlist" );
	}

    TAVI_CanSleep( TASK_ID_MUSIC );
}

int DeleteFolder( CPlaylistView* pview )
{
  int idx;
  CPLListItem* item;
  uchar_t path[MAX_PATH];
  uchar_t ext[6];

  idx = pview->GetSelectedIndex();
  item = ( CPLListItem* )pview->GetSelectedItem();
  str2ustr( ext, PLAYLIST_EXT, 4 );

  if( !ustrcmp(pT->GetTextRes(ML_INSTANT_PLAYLIST), (uchar_t*)item->m_szText) ) {
    ShowMessage( "Can't delete the I-Playlist" );
  }
  else {
    bool dodelete = true;
    if( g_PlaylistPL ) {
      uchar_t* n1 = (uchar_t*)g_PlaylistPL->GetCurrentFolder();
      uchar_t* n2 = (uchar_t*)item->m_szText;
      if( !ustrcasecmp(n1, n2) ) {
        ShowMessage( "This file is on playing." );
        dodelete = false;
      }
      else {
        g_PlaylistPL->DeleteFolder( idx );
      }
    }

    if( dodelete ) {
      FATFS_ERROR err;
      str2ustr( path, PLAYLIST_BASE );
      ustrcat( path, (uchar_t*)item->m_szText );
      ustrcat( path, ext );
      err = fatfs_wdelete_file( path );
      if( !err ) {
        pview->DeleteItem( idx );
      }
    }
  }

  g_pFocusedWnd = pview;
  pview->m_pCtxMenuWnd->SetVisible( false );
	pview->m_pCtxMenuWnd->Hide();
	delete pview->m_pCtxMenuWnd->m_pCtxMenu;
	pview->m_pCtxMenuWnd->m_pCtxMenu = NULL;
	pview->Update();
  
  return 0;
}

int DeleteFile( CPlaylistView* pview )
{
	CPlayerWnd* pPlayer;
	if( (pPlayer=(CPlayerWnd*)AP_HasPlayer()) &&
		(pPlayer->m_nPlayType == TYPE_PLAYLIST) ) {
			ShowMessage("Can't change while playing a file");
	}
	else {
		int idx = pview->m_nSelectedIdx + pview->m_nStartIdx;
		pview->DeleteItem( idx );
		
		uchar_t ext[6];
		uchar_t path[1024];
    str2ustr( ext, PLAYLIST_EXT, 4 );
		str2ustr( path, PLAYLIST_BASE, strlen(PLAYLIST_BASE) );
		if( ustrcmp(pT->GetTextRes(ML_INSTANT_PLAYLIST), 
					pview->m_szPlaylistname) == 0 ) {
      uchar_t wstr[100];
      str2ustr( wstr, IPLAYLIST_NAME );
      ustrcat( path, wstr );
		}
		else {
		  ustrcat( path, (uchar_t*)pview->m_szPlaylistname );
		}
		ustrcat( path, ext );
		SavePlaylist( pview->m_uzList, path );
	}
	g_pFocusedWnd = pview;
	pview->m_pCtxMenuWnd->SetVisible( false );
	pview->m_pCtxMenuWnd->Hide();
	delete pview->m_pCtxMenuWnd->m_pCtxMenu;
	pview->m_pCtxMenuWnd->m_pCtxMenu = NULL;
	if( pview->GetCount() == 0 ) { // go backward
		KeyEvent evt = { VK_LEFT, 1 };
		pview->OnKeypressed( evt );
	}
	else pview->Update();

	return 0;
}

int DeletePlaylist( RMuint8* arg )
{
  CPlaylistView* pView = ( CPlaylistView* )arg;
  if( !pView->m_nDepth ) {
    DeleteFolder( pView );
  }
  else {
    DeleteFile( pView );
  }
	return 0;
}


void CPlaylistView::CreateContextMenu( void )
{
	int* idx = new int;
	*idx = m_nSelectedIdx+m_nStartIdx;
	m_pCtxMenuWnd->Reset();
	m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_DELETE, 0, 0, DeletePlaylist, (RMuint8*)this );
}

/* FIN */
