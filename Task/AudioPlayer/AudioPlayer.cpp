//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Music Player 
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/AudioPlayer.cpp,v $
// $Revision: 1.6 $
// $Date: 2006/06/08 07:15:53 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: AudioPlayer.cpp,v $
// Revision 1.6  2006/06/08 07:15:53  zstein
// - additional functions.
//
// Revision 1.5  2006/04/17 04:27:19  zstein
// - added AP_HDD_Wakeup().
//
// Revision 1.4  2006/03/16 03:36:40  zstein
// - fixed UI bug.
//
// Revision 1.3  2006/03/07 03:08:09  ywkim
// Change Kill sequence for 3Deffect off
//
// Revision 1.2  2006/03/07 02:32:01  zstein
// - fixed bug when a music is stopped on MAIN MODE.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.119  2006/02/16 05:46:03  zstein
// - force to configurate sound driver.
//
// Revision 1.118  2006/01/20 04:47:45  zstein
// *** empty log message ***
//
// Revision 1.117  2006/01/06 11:20:08  zstein
// *** empty log message ***
//
// Revision 1.116  2005/12/29 09:21:34  zstein
// *** empty log message ***
//
//******************************************************************************

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>	// memmove/memcpy
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sched.h>

#include "jpeg/jpeg.h"
#include "mp3/mp3.h"
#include "Lib/LibImage.h"
#include "meplib.h"

#include "Component/object.h"
#include "Component/event.h"
#include "Component/fbdev.h"
#include "Component/Stack.h"
#include "Component/ProgressDlg.h"
#include "Task/tavi_global.h"
#include "Task/SubLCD.h"
#include "Task/ExtLCD.h"
#include "Task/TopPanel.h"

#include "fatfs.h"

#include "AudioPlayer.h"
#include "PlayListView.h"
#include "FolderView.h"
#include "ID3View.h"
#include "PlayerWnd.h"
#include "AudioCmd.h"
#include "MusicMainWnd.h"
#include "musicdef.h"

// MACROS
#define		AUTO_BACK_2_PLAYER_WINDOW_TIME	17	// seconds

// Extern Variables
extern CTopPanel				*g_TopPanel;
extern CMiscCtrl				*g_MiscCtrl;
extern CExtLCD					*g_ExtLCD;
extern SysParameter_t			TaviSysParam;
extern BITMAPFILEINFO256 		TAVI_IMAGES[];
extern CTextEngine_BDF			*pT;

CSoundPaletteDbx				*g_pSndPalette;

typedef struct _tag_TAUIDOPLAYER_CONFIG {
	CList< CWindow >	m_WndList;
	CWindow				*m_pmainwnd;
	CPlayerWnd			*m_pplayerwnd;
	AudioCmd			m_uzCmd;
	bool				m_fExit;
	int					m_ExitState;
	int					m_playstate;
} TAUIDOPLAYER_CONFIG;

extern void showbackground( int id );

// Global Variables
TAUIDOPLAYER_CONFIG			prop;
CMusicDBPL						*g_MusicDBPL;
CFolderPL						*g_FolderPL;
CPlaylistPL						*g_PlaylistPL;
CDeviceMusicDB					*g_MusicDB;
CPlayerWnd						*g_PlayerWnd;
CArray<CListItem*> 				g_AllTrackList;
int								g_PlayKey;

// ++ LOCAL VARIABLES
CProgressDlg* _progress;
static unsigned int _dbdate;
// --

static int OnKeyPress( int key, int press )
{
	CEvent evt;
	int vkey;

	if ( key == TAVI_KEY_SHUTDOWN ) {
		prop.m_fExit		= true;
		prop.m_ExitState	= TaskExit_Normal;

		return 0;
	} else if ( key == TAVI_KEY_LEXIT ) {
		prop.m_fExit		= true;
		if ( g_MiscCtrl->USBConnected() ) {
			prop.m_ExitState = TaskExit_Normal;
		} else {
			prop.m_ExitState = TaskExit_ExitAndSwitch;
		}
		return 0;
	}

 	vkey				= MapToVirtualKey( key );
	evt.type			= EVENT_KEY;
	evt.u.key.value		= vkey;
	evt.u.key.pressed	= press;
	g_pFocusedWnd->SendEvent( evt );

	return 0;
}

static void OnMouseEvent( const MouseEvent& evt )
{
	CEvent	event;

	event.type		= EVENT_MOUSE;
	event.u.mouse	= evt;
	g_pFocusedWnd->SendEvent( event );
}

static CWindow* GetWindow( int nWindowID )
{
	CWindow		*pWnd = NULL;
	int 		i;

	for ( i=0; i < prop.m_WndList.Count(); i++ ) {
		pWnd = prop.m_WndList[i];
		if( pWnd->GetWindowID() == nWindowID ) break;
	}

	if ( i == prop.m_WndList.Count() ) return NULL;

	return pWnd;
}

static void OnWindowEvent( const WindowEvent& evt )
{
	CNode< CWindow >	*pNode;
	CEvent				event;

	event.type			= EVENT_WINDOW;
	event.u.window		= evt;
	
	// search window with window id.
	prop.m_WndList.Rewind();
	CWindow* pWnd		= prop.m_WndList.Head();

	while ( pWnd ) {
		if ( pWnd->GetWindowID() == evt.nWindowID ) break;
		pWnd = prop.m_WndList.Next();
	}

	switch ( evt.nMessage ) {

	case WINDOW_KILL_TASK:
		prop.m_ExitState	= TaskExit_Normal;
		prop.m_fExit		= true;
		return;

	case WINDOW_KILL:
		if ( pWnd == prop.m_pplayerwnd ) {
		    prop.m_pplayerwnd = NULL;
		    g_PlayerWnd = NULL;
	    }
		if ( pWnd ) delete pWnd;
		if ( prop.m_WndList.Head() ) {
		    prop.m_WndList.Head()->SetFocus();
	    }
		break;

	case WINDOW_KILL_FOCUS:
		pNode = prop.m_WndList.Detach( pWnd );

		if ( pNode ) {
			CWindow* wnd	= ( CWindow* )pNode->m_Data;
			wnd->Hide();
			wnd->SetVisible( false );

			prop.m_WndList.AddNodeToTail( pNode );
			prop.m_WndList.Head()->SetFocus();
		}
		break;

	case WINDOW_SET_BG:
		prop.m_fExit		= true;
		prop.m_ExitState	= TaskExit_Background;
		break;

	case WINDOW_TASK_SWITCH:
		prop.m_fExit		= true;
		prop.m_ExitState	= TaskExit_Switch;
		break;

	case WINDOW_TASK_KILL_AND_SWITCH:
		prop.m_fExit		= true;
		prop.m_ExitState	= TaskExit_ExitAndSwitch;
		break;

	case WINDOW_QUEUE:
		event.u.window.nParam	= (int)(&prop.m_uzCmd);
		pWnd					= prop.m_pplayerwnd;

	default:
		if ( pWnd ) pWnd->SendEvent( event );
		else prop.m_pmainwnd->SendEvent( event );

	}
}

static unsigned int _GetDBWriteDate( void )
{
	RMuint32 handle;
	FATFS_DIR_ENTRY_INFO entry;
	FATFS_ERROR err;
	uchar_t buf[20];

	str2ustr( buf, "music.db" );
	err = fatfs_dopen( (RMuint8*)_AUDIO_DIR, &handle );

	if( err == FATFS_ERROR_NO_ERROR ) {
		while( 1 ) {
			err = fatfs_dread( handle, &entry );
			if( err != FATFS_ERROR_NO_ERROR ) break;
			if( entry.attributes&ATTR_DIRECTORY ) continue;
			if( ustrcasecmp(buf, (uchar_t*)entry.name) == 0 ) {
				break;
			}
		}
	}
	fatfs_dclose( handle );

	
	unsigned int date = ((unsigned int)entry.file_write_date)<<16 | entry.file_write_time;
	printf( "date: 0x%x, time: 0x%x, date: 0x%x\n", entry.file_write_date, entry.file_write_time, date );
	return date;
}

static bool _LoadMusicDB( void )
{
	bool ret = true;
	if( !g_MusicDB ) {
		int lastMode;
		CString sz = "Loading database...";

		lastMode = (int)g_ExtLCD->GetCurrentMode();
		g_ExtLCD->ReservedMessage(CExtLCD::EXT_MSG_LOAD_MUSIC_DB);
		g_ExtLCD->ChangeMode(CExtLCD::EXT_STAGE_MESSAGE);
		g_ExtLCD->Update();

		// SHOW MESSAGE
		if( !_progress ) _progress = new CProgressDlg( NULL, "Load Music Library..." );
		_progress->Resize( CRect(10, 80, 312, 160), false );
		_progress->Show();
		_progress->Update( sz, -1 );
		
		g_MusicDB = new CDeviceMusicDB;
		g_MusicDB->Init_Hash(); // Hash table ÃÊ±âÈ­
		ret = g_MusicDB->Load_AllMusicFromLibFile();

		if( ret == false ) {
			delete g_MusicDB;
			g_MusicDB = NULL;
		}
		
		// CLEAR MESSAGE
		delete _progress;
		_progress = NULL;

		g_ExtLCD->ChangeMode((CExtLCD::ExtLCD_Mode)lastMode);
		g_ExtLCD->Update();

		_dbdate = _GetDBWriteDate();
	}

	return ret;
}

static void _LoadingProgress( CString& sz, int count ) {
	if( _progress ) 
		_progress->Update( sz, count );
}

static void _CheckUpdate( void )
{
	// force to make folder list
	if( !g_FolderPL ) TaviSysParam.music.update = 1;
	
	if( TaviSysParam.music.update ) {
		TaviSysParam.music.update = 0;

		// Clear playlist
		if( g_PlaylistPL ) {
		    delete g_PlaylistPL;
	    }
		g_PlaylistPL = NULL;
		
		int date = _GetDBWriteDate();
		// Clear Music Database
		if( g_MusicDB ) {
			if( date != _dbdate ) {
				delete g_MusicDB;
				g_MusicDB = NULL;

				// Clear Alltrack list
				g_AllTrackList.FreeAll();

				if( g_MusicDBPL ) delete g_MusicDBPL;
				g_MusicDBPL = NULL;

				_dbdate = date;
			}
		}
	}
}

static bool _InstancePlay( const CString& path )
{
	struct PlayInfo* pInfo = ( struct PlayInfo* )&prop.m_uzCmd.szArg;
	AudioCmd* pCmd = ( AudioCmd* )&prop.m_uzCmd;
	QBUF_t* buf = ( QBUF_t* )pCmd;
	// PLAY
	TAVI_WakeUp(100);
	pInfo->lStartTime = 0;
	const uchar_t* tmp = ( const uchar_t* )path;
	ustrcpy( (uchar_t*)pInfo->szFilename, (uchar_t*)tmp );
	pCmd->lMsgID = MSG_SEND_ID; // don't set 0
	pCmd->lLength = sizeof(AudioCmd) - 4;
	pCmd->lCmd = AUDIO_CMD_PLAY;
	SysMSGSend( g_qidMusic, buf, buf->lLength, 0 );

	return true;
}

int AP_Stop( void )
{
	TAVI_WakeUp(100);
	AudioCmd	*pCmd	= ( AudioCmd* )&prop.m_uzCmd;
	QBUF_t		*buf	= ( QBUF_t* )pCmd;

	pCmd->lMsgID		= MSG_SEND_ID; // don't set 0
	pCmd->lLength		= sizeof(AudioCmd) - 4;
	pCmd->lCmd			= AUDIO_CMD_STOP;
	SysMSGSend( g_qidMusic, buf, buf->lLength, 0 );
	return 0;
}

#include <sys/time.h>
#include <sys/resource.h>

int AP_Play( int type, const CString& path, CDeviceMusicDB* handle, const CString& filename )
{
	CPlayerWnd		*pPlayer;
	CWindow			*pWnd;

	TAVI_WakeUp( 100 );

	if ( type == TYPE_FILE ) {
		_InstancePlay( filename );
		return 0;
	}
	
	prop.m_WndList.Rewind();
	pWnd = prop.m_WndList.Head();

	while ( pWnd ) {
		if ( strcmp(pWnd->GetWindowTitle().GetString(), MUSIC_TITLE_STRING) == 0 ) break;
		pWnd = prop.m_WndList.Next();
	}

	pPlayer = ( CPlayerWnd* )pWnd;

	if ( pPlayer == NULL ) {
		pPlayer = new CPlayerWnd( NULL );
		pPlayer->Resize( CRect(0, 30, 319, 209), false );
		pPlayer->SetWindowList( &prop.m_WndList );
		pPlayer->RegisterWndList();
		pPlayer->SetFocus();
		prop.m_pplayerwnd = pPlayer;
		g_PlayerWnd = pPlayer;
	}

    pPlayer->Play( type, path, handle, filename );
	if ( pPlayer->IsShow() == false ) { // not focused
		pPlayer->SetFocus();
	}

	// eat touch event
	mep_eat_event();
  // clear key buffer
  ClearKeyBuffer();

	return 0;
}

int AP_Pause( void )
{
	if( prop.m_pplayerwnd == NULL ) return -1;
	prop.m_pplayerwnd->Pause();
	op_Flush();
	return 0;
}

int AP_Resume( void )
{
	if( prop.m_pplayerwnd == NULL ) return -1;
	prop.m_pplayerwnd->Resume(); 
	op_Flush();
	return 0;
}

int AP_Seek( int sec, int dir )
{
	if( prop.m_pplayerwnd == NULL ) return -1;
	prop.m_pplayerwnd->Seek( sec, dir );
	return 0;
}

int AP_GetPlayState()
{
	if( g_PlayerWnd ) 
		return g_PlayerWnd->GetPlayState();
	else 
		return AUDIO_STATE_STOP;
}

int AP_Next( void )
{
	if( prop.m_pplayerwnd == NULL ) {
		return -1;
	}

	prop.m_pplayerwnd->Next();
	return 0;
}

int AP_Prev( void )
{
	if( prop.m_pplayerwnd == NULL ) {
		return -1;
	}

	prop.m_pplayerwnd->Prev();
	return 0;
}

int AP_Kill( void )
{
	if( prop.m_pmainwnd == NULL ) { // deleted before
		return 0;
	}

	// DO NOT USE CList::FreeAll()!!!
	CNode< CWindow >* pNode = prop.m_WndList.DetachHead();

	while( pNode ) {
		delete pNode;

		pNode = prop.m_WndList.DetachHead();
	}

	prop.m_pmainwnd = NULL;
	prop.m_pplayerwnd = NULL;
	g_PlayerWnd = NULL;

	if( g_MiscCtrl->FolderClosed() ) {
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_MENU );
		g_ExtLCD->Update();
	}

	memset( (void*)&prop, 0, sizeof(prop) );

#if XEN_LIB
		pMpegDecoder->XEN_SetPower( false );	// XEN_LIB power off
#endif	// XEN_LIB
	return 0;
}

int AP_Volume( int dir )
{
	if( prop.m_pplayerwnd == NULL ) return -1;
	if( dir ) prop.m_pplayerwnd->IncVol();
	else prop.m_pplayerwnd->DecVol();
}

void AP_DoIdle( void* arg )
{
	CEvent			event;
	WindowEvent		*wevt = ( WindowEvent* )arg;
	event.type		= EVENT_WINDOW;
	event.u.window	= *wevt;
	g_pFocusedWnd->SendEvent( event );
}

bool AP_SetPlaystate( int state )
{
	if( state < AUDIO_STATE_READY || state > AUDIO_STATE_STOP ) 
		return false;
	prop.m_playstate = state;
	
	return true;
}

int AP_GetPlaystate( void )
{
	return AP_GetPlayState();
}

void AP_BackToPlayer( void )
{
	if ( !prop.m_pplayerwnd ) return;		// Not Yet!
	if( prop.m_pplayerwnd->IsShow() ) return; 
	
	CNode< CWindow >	*pnode = prop.m_WndList.Detach( prop.m_pplayerwnd );

	if ( !pnode ) { return; }

	CWindow* wnd    = ( CWindow* )pnode->m_Data;

	if ( 0 && g_pFocusedWnd->GetWindowID() == wnd->GetWindowID()) {	// Already got focus
		return;
	}

	prop.m_WndList.AddNodeToHead( pnode );
	prop.m_WndList.Head()->Show();
	prop.m_WndList.Head()->SetFocus();

	g_pFocusedWnd = prop.m_pplayerwnd;
}

int AP_MsgProc( TMsgEntry* pMsg )
{
	MouseEvent		mevt;
	WindowEvent		wevt;
	TimerEvent		tevt;
	CEvent			evt;
	static int		lastIdleTime	= 0;

	switch ( pMsg->ulMsgId ) {

	case MSG_DRAW:
		break;

	case MSG_BUTTON_UP:
	case MSG_BUTTON_REAL_UP:
		lastIdleTime	= 0;
		return OnKeyPress( pMsg->usParam1, pMsg->usParam2 );

	case MSG_TOUCHPAD:
		mevt.click	= pMsg->usParam2;
		mevt.x		= pMsg->usParam1;
		lastIdleTime	= 0;
		OnMouseEvent( mevt );
		break;		

	case MSG_WINDOW:
		wevt.nWindowID	= pMsg->usParam1;
		wevt.nMessage	= pMsg->usParam2 & 0xffff0000;
		wevt.nParam		= pMsg->usParam2 & 0xffff;
		OnWindowEvent( wevt );
		break;

	case MSG_FOLDER:
		evt.type				= EVENT_WINDOW;
		evt.u.window.nWindowID	= g_pFocusedWnd->GetWindowID();
		evt.u.window.nMessage	= ON_FOLDER;
		evt.u.window.nParam		= g_MiscCtrl->FolderOpen();
		g_pFocusedWnd->SendEvent( evt );
		lastIdleTime			= 0;
		break;

	default:
		wevt.nWindowID	= 0;
		wevt.nMessage	= WINDOW_IDLE;
		OnWindowEvent( wevt );
		AP_DoIdle( (void*)&wevt );

		if (lastIdleTime == 0) {
			lastIdleTime			= MSG_Peek_Time();
		} else {
			if (MSG_Peek_Time() - lastIdleTime >= AUTO_BACK_2_PLAYER_WINDOW_TIME) {	// seconds
				AP_BackToPlayer();
				lastIdleTime			= 0;
			}
		}
		break;
	}

	return 0;
}


int AudioPlayer_Task( TTaskInitParam* tParam )
{
	TMsgEntry		tMsg;
	RMuint32		fd;
	CMusicMainWnd	*pMainWnd;
	FATFS_ERROR		err;

  /* 
      Reconfigurate sound driver
  */
  g_MiscCtrl->Mute( true );
  g_SndCtrl->PlaybackMode();
  g_SndCtrl->LineInBypass( false, 0 );
  g_SndCtrl->LineInBypass( false, 1 );
  g_SndCtrl->LineInBypass( false, 2 );
  g_MiscCtrl->Mute( false );
    
	/* Setup */
	showbackground( ID_BG_AUDIO );
	TAVI_LoadPaletteTransparencyData(TRUE);
	mep_report( MEP_REPORT_RELATIVE );

	// Set current task id
	g_CurrentTaskID = tParam->ssTaskId;

	g_ExtLCD->ChangeMenu(CExtLCD::EXT_MENU_MUSIC);

	if ( tParam->resume ) {
    if( AP_GetPlayState() == AUDIO_STATE_STOP && prop.m_pplayerwnd ) {
      delete prop.m_pplayerwnd;
      prop.m_pplayerwnd = NULL;
      g_PlayerWnd = NULL;
    }
		goto __msg_proc;
	}

	// Clear & Update Folder palylist.
	_CheckUpdate();

	TAVI_CanSleepAudio(100);	// power save

	fatfs_dopen( (RMuint8*)_HDD_ROOT, &fd );
	fatfs_dcreate_dir( fd, (RMuint8*)"Music" );
	fatfs_dclose( fd );

	// playlistÆÄÀÏÀÌ ¾øÀ¸¸é »ý¼º.
	err = fatfs_fopen( (RMuint8*)INSTANCE_PLAYLIST, _O_RDONLY, &fd );

	if ( err != FATFS_ERROR_NO_ERROR ) {
		err = fatfs_fopen( (RMuint8*)INSTANCE_PLAYLIST, _O_WRONLY | _O_CREAT, &fd );
	}

	if ( err == FATFS_ERROR_NO_ERROR ) {
		fatfs_fclose( fd );
	}

	_LoadMusicDB();

__msg_proc:

	// Create Main Window
	if ( !prop.m_pmainwnd ) {
		pMainWnd = new CMusicMainWnd( NULL, g_MusicDB );
		pMainWnd->Resize( CRect(13, 30, 319, 216), false );
		pMainWnd->SetWindowTitle( "MusicMain" );
		pMainWnd->SetWindowList( &prop.m_WndList );
		pMainWnd->RegisterWndList();
		pMainWnd->SetFocus();

		prop.m_pmainwnd = pMainWnd;
	} else {
	  prop.m_WndList[0]->SetFocus();
	}

	if( g_pSndPalette ) {
    g_pSndPalette->ApplyUserSet();
  }
    
	op_Flush();
	ClearKeyBuffer();
	
	prop.m_fExit = false;

	while ( prop.m_fExit == false ) {
		int peek_flags;
		int playState;

		playState = AP_GetPlayState();

		peek_flags = MSG_PEEK_ALL;

		memset( (void*)&tMsg, 0, sizeof(tMsg) );

		if ( MSG_Peek_Ex( g_CurrentTaskID, &tMsg, peek_flags ) < 0 ) {
			tMsg.ulMsgId = 0;
		}

		if ( g_pSndPalette == NULL ) {
			AP_MsgProc( &tMsg );
		} else if( !g_pSndPalette->IsVisible() ) {
			AP_MsgProc( &tMsg );
		} else {
			static struct timeval stv;
			struct timeval etv;
			gettimeofday( &etv, NULL );

			//¸Þ¼¼Áö°¡ ¾øÀ¸¸é idle ½Ã°£À¸·Î Ã³¸®. ÀÌ´Â ÇöÀç Æ÷Ä¿½ºµÈ À©µµ¿ì°¡ °¡Á®°¨.
			if( tMsg.ulMsgId == MSG_WINDOW || tMsg.ulMsgId == 0 ) {
				// ¾Æ¹«ÀÛ¾÷µµ ¾ÈÇÏ°í åÀÖÀ¸¸é 15ÃÊÈÄ¿¡ »ç¶óÁü.
				if( !stv.tv_sec ) gettimeofday( &stv, NULL );
				if( abs(etv.tv_sec-stv.tv_sec) > 15 ) {
					g_pSndPalette->Hide( SKIP_OSD_REFRESH );
					g_TopPanel->SetVisible( true );
					g_TopPanel->Update();
					prop.m_pplayerwnd->m_puzBottomWnd->SetVisible( true );
					prop.m_pplayerwnd->m_puzBottomWnd->Show();
					g_pFocusedWnd = prop.m_pplayerwnd;
					stv.tv_sec = 0;
				}
				AP_MsgProc( &tMsg );
			} else {
				int ret;
				ret = g_pSndPalette->MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 );
				if( ret < 0 ) {
					g_pSndPalette->Hide( SKIP_OSD_REFRESH );
					g_TopPanel->SetVisible( true );
					g_TopPanel->Update();
					prop.m_pplayerwnd->m_puzBottomWnd->SetVisible( true );
					prop.m_pplayerwnd->m_puzBottomWnd->Show();
					g_pFocusedWnd = prop.m_pplayerwnd;

					prop.m_fExit	= false;
					stv.tv_sec		= 0;
				} else {
					// Update hide timer
					stv = etv;
				}
			}
		}
		usleep( 100 );
	}

	/////////////////////////////////////////////////////////////
	// Exit Audio Player Task

	int ret = prop.m_ExitState;
	if ( prop.m_ExitState == TaskExit_Normal ||
	    prop.m_ExitState == TaskExit_ExitAndSwitch ) {
		AP_Kill();
	} else {
    prop.m_pmainwnd->Hide();
    prop.m_pmainwnd->SetVisible( false );
		if ( prop.m_pplayerwnd ) {
			prop.m_pplayerwnd->Hide();
		}
	}

	extern int handle_key;
	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );

	return ret;
}

CList< CWindow >* AP_GetWndList( void )
{
	return &prop.m_WndList;
}

int AP_GetQueue( void )
{
	QID_t qid;
	QBUF_t* pBuf = ( QBUF_t* )&prop.m_uzCmd;
	pBuf->lLength = sizeof( AudioCmd ) -4;

	if( prop.m_pplayerwnd ) {
		qid = prop.m_pplayerwnd->GetQID();
		pBuf->lLength = sizeof(AudioCmd)-4;
		if( SysMSGRecv( qid, pBuf, MSG_RECV_ID, IPC_NOWAIT ) == QError_NOERROR ) {
			return 0;
		}
	}
	return -1;
}

CWindow* AP_HasPlayer( void ) 
{
	return prop.m_pplayerwnd;
}

void AP_Wait( int, void* )
{
	//usleep( 10 );
	sched_yield();
}

int AP_MsgHandler( int id, U32 msgid, int param1, int param2 )
{
	/*
		Return Value:
			 1: handled an event
			 0: ate an event
			-1: skip an event
	*/
	int handled = 0;
	static timeval stv = { 0, 0 };
	struct timeval tv;

	if( !stv.tv_sec ) gettimeofday( &stv, NULL );
	gettimeofday( &tv,NULL );
	if( abs(tv.tv_sec-stv.tv_sec) > 3 ) {
		handled = 1;
	}
	else {
		if( msgid == MSG_BUTTON_DN ) {
			switch( param1 ) {
			case VK_ENTER:
			case VK_ESC:
				handled = 1;
				break;
			case VK_EXT_ESC:
				handled = -1;
				break;
			}
		}
	}

	if ( handled == 1 ) {
		stv.tv_sec = 0;
		stv.tv_usec = 0;
	}
	
	return handled;
}

void AP_Remount( void )
{
	AudioCmd	*pCmd	= ( AudioCmd* )&prop.m_uzCmd;
	QBUF_t		*buf	= ( QBUF_t* )pCmd;
	pCmd->lMsgID		= MSG_SEND_ID; // don't set 0
	pCmd->lLength		= sizeof(AudioCmd) - 4;
	pCmd->lCmd			= AUDIO_CMD_REMOUNT;
	SysMSGSend( g_qidMusic, buf, buf->lLength, 0 );

	TaviSysParam.music.update = 1;
	TaviSysParam.music.updatefolder = 1;
}


void AP_HDD_Wakeup( void )
{
    AudioCmd	*pCmd	= ( AudioCmd* )&prop.m_uzCmd;
	QBUF_t		*buf	= ( QBUF_t* )pCmd;
	pCmd->lMsgID		= MSG_SEND_ID; // don't set 0
	pCmd->lLength		= sizeof(AudioCmd) - 4;
	pCmd->lCmd			= AUDIO_CMD_HDD_WAKEUP;
	SysMSGSend( g_qidMusic, buf, buf->lLength, 0 );
}

/* FIN */
