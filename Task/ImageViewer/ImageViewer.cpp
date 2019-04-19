//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/ImageViewer.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ImageViewer.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.53  2006/01/16 02:40:47  zstein
// *** empty log message ***
//
// Revision 1.52  2006/01/10 11:40:12  zstein
// *** empty log message ***
//
// Revision 1.51  2005/12/29 07:40:24  ywkim
// Add function of IV_IsViewingOnForeground() for lcd off
//
// Revision 1.50  2005/12/22 04:30:53  zstein
// *** empty log message ***
//
// Revision 1.49  2005/12/03 09:23:47  zstein
// *** empty log message ***
//
// Revision 1.48  2005/12/03 08:49:14  zstein
// *** empty log message ***
//
// Revision 1.47  2005/12/01 01:03:57  zstein
// *** empty log message ***
//
// Revision 1.46  2005/11/25 10:22:17  zstein
// *** empty log message ***
//
// Revision 1.45  2005/11/24 03:04:17  zstein
// *** empty log message ***
//
// Revision 1.44  2005/11/17 11:20:33  zstein
// *** empty log message ***
//
// Revision 1.43  2005/11/08 01:18:53  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <dirent.h>
#include <unistd.h>
#include "../Lib/LibImage.h"
#include "ImageViewer.h"
#include "../../jpeg/jpeg.h"
#include <Task/TopPanel.h>
#include <Task/Msg.h>
#include <Task/SubLCD.h>
#include <Task/ExtLCD.h>
#include <Task/tavi_global.h>
#include "IVMainWnd.h"

// Macros
#define DEBUG
#ifdef DEBUG
#define DEBUGMSG							printf
#else
#define DEBUGMSG
#endif


// Extern Variables 
extern CTopPanel* g_TopPanel;
extern int g_CurrentTaskID;
extern CTextEngine_BDF* pT;
extern CExtLCD* g_ExtLCD;

// Extern Functions
extern void showbackground (char *bkgnd);
extern void showbackground( int nMenuID );

// Local Variables
struct IVProperty_t {
	CIVMainWnd* m_pMainWnd;
	CList< CWindow > m_WndList;
	bool m_fExit;
	int m_ExitState;
} _IVProperty;

CThumbDB* g_p_ThumbDB;


static void OnWindowEvent( const WindowEvent& evt )
{
	CEvent event;
	event.type = EVENT_WINDOW;
	event.u.window = evt;

	_IVProperty.m_WndList.Rewind();
	CWindow* pWnd = _IVProperty.m_WndList.Head();
	while( pWnd ) {
		if( pWnd->GetWindowID() == evt.nWindowID ) break;
		pWnd = _IVProperty.m_WndList.Next();
	}
	
	switch( evt.nMessage ) {
	case WINDOW_KILL_TASK:
		_IVProperty.m_ExitState = TaskExit_Normal;
		_IVProperty.m_fExit = true;
		break;
	case WINDOW_TASK_KILL_AND_SWITCH:
		_IVProperty.m_fExit = true;
		_IVProperty.m_ExitState = TaskExit_ExitAndSwitch;
		break;
	case WINDOW_TASK_SWITCH:
		_IVProperty.m_fExit = true;
		_IVProperty.m_ExitState = TaskExit_Switch;
		break;
	case WINDOW_KILL:
		if( pWnd ) delete pWnd;	
		g_pFocusedWnd = _IVProperty.m_WndList.Head();
		if( g_pFocusedWnd ) g_pFocusedWnd->Show();
		break;
	case WINDOW_SET_BG:
		_IVProperty.m_ExitState = TaskExit_Background;
		_IVProperty.m_fExit = true;
		break;
	case WINDOW_IDLE:
		pWnd = g_pFocusedWnd;
	default:
		if( pWnd ) pWnd->SendEvent( event );
		else g_pRootWnd->SendEvent( event );
	}
}

static void OnKeyEvent( const KeyEvent& evt )
{
	switch( evt.value ) {
	case TAVI_KEY_SHUTDOWN:
		_IVProperty.m_ExitState = TaskExit_Normal;
		_IVProperty.m_fExit = true;
		break;
	case TAVI_KEY_JOG1_ENTER:
		if( g_MiscCtrl->FolderOpen() ) break;
	case VK_EXT_ESC:
		_IVProperty.m_ExitState = TaskExit_ExitAndSwitch;
		_IVProperty.m_fExit = true;
		return;
	case TAVI_KEY_JOG1_UP:
		TAVI_BG_Volume( 1 );
		return;
	case TAVI_KEY_JOG1_DOWN:
		TAVI_BG_Volume( 0 );
		return;
	case TAVI_KEY_JOG2_UP:
		if( evt.pressed ) TAVI_BG_Next();
		return;
	case TAVI_KEY_JOG2_DOWN:
		if( evt.pressed ) TAVI_BG_Prev();
		return;
	case TAVI_KEY_JOG2_ENTER:
		if( evt.pressed ) TAVI_BG_PlayPause();
		break;
	}
	
	CEvent event;
	event.type = EVENT_KEY;
	event.u.key = evt;
	g_pFocusedWnd->SendEvent( event );
}

static void OnMouseEvent( const MouseEvent& evt )
{
	CEvent event;
	event.type = EVENT_MOUSE;
	event.u.mouse = evt;
	g_pFocusedWnd->SendEvent( event );
}

int IV_MsgProc( TMsgEntry* pMsg )
{
	CEvent event;
	MouseEvent mouse;
	KeyEvent key;
	WindowEvent window;
	TimerEvent tevt;

	TAVI_BG_VolTimeout();
	
	switch( pMsg->ulMsgId ) {
	case MSG_SETFOCUS : 	break;
	case MSG_KILLFOCUS: 	break;
	case MSG_DESTROY:		break;
	case MSG_BUTTON_UP:
	case MSG_BUTTON_REAL_UP:
		key.value = MapToVirtualKey( pMsg->usParam1 );
		key.pressed = pMsg->usParam2;
		OnKeyEvent( key );
		break;
	case MSG_TOUCHPAD:
		mouse.x = pMsg->usParam1;
		mouse.click = pMsg->usParam2;
		OnMouseEvent( mouse );
		break;
	case MSG_WINDOW:
		window.nWindowID = pMsg->usParam1;
		window.nMessage = pMsg->usParam2&0xffff0000;
		window.nParam = pMsg->usParam2&0xffff;
		OnWindowEvent( window );
		break;
	case MSG_FOLDER:
		if( pMsg->usParam1 ) {
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
			g_ExtLCD->Update();
		}
		break;
	default:
		window.nWindowID = g_pFocusedWnd->GetWindowID();
		window.nMessage = WINDOW_IDLE;
		OnWindowEvent( window );
		break;
	}

	return TRUE;
}

int ImageViewer_Task( TTaskInitParam* tParam )
{
	TMsgEntry tMsg;

	TAVI_LoadPaletteTransparencyData(TRUE);
	SetTaviKeyMode(TK_REPEAT);

	showbackground( ID_BG_PHOTO );
	// CWindow constructor refers g_CurrentTaskID for its task id.
	g_CurrentTaskID = tParam->ssTaskId;

	if( tParam->resume ) {
		_IVProperty.m_WndList[0]->SetFocus();
		goto __msg_proc;
	}

	// Load photo thumbdb.
	g_p_ThumbDB = new CThumbDB;
	g_p_ThumbDB->Load();
	
	// force to create photo directory.
	// If the directory to be created is exist, mkdir will return error. 
	RMuint32 hDir;
	fatfs_dopen( (RMuint8*)"/hd1", &hDir );
	fatfs_dcreate_dir( hDir, (RMuint8*)"Photo" );
	fatfs_dclose( hDir );
	
	// Create main window.
	if( TaviSysParam.photo.view > PHOTO_VIEW_THUMBNAIL ||
		TaviSysParam.photo.view < PHOTO_VIEW_LISTVIEW ) 
		TaviSysParam.photo.view = PHOTO_VIEW_LISTVIEW;
	_IVProperty.m_pMainWnd = new CIVMainWnd( NULL, (CIVMainWnd::IVViewMode)TaviSysParam.photo.view );
	_IVProperty.m_pMainWnd->Resize( CRect(0, 30, 319, 239), false );
	g_pRootWnd = _IVProperty.m_pMainWnd;

	// HDD Sleep
	TAVI_CanSleep( TASK_ID_IMAGE );
__msg_proc:
	op_Flush();
	struct timeval stv, etv;
	int a, b;
	gettimeofday( &stv, NULL );
	ClearKeyBuffer();
#if 0
        while( tParam->resume ) {
        if( MSG_Peek( g_CurrentTaskID, &tMsg ) >= 0 ) {
        gettimeofday( &stv, NULL );
        }
        gettimeofday( &etv, NULL );
        a = etv.tv_sec*1000 + etv.tv_usec/1000;
        b = stv.tv_sec*1000 + stv.tv_usec/1000;
        if( abs(a-b) > 500 ) break;
        }
#endif
	_IVProperty.m_fExit = false;
	while( _IVProperty.m_fExit == false ) {
		memset( (void*)&tMsg, 0, sizeof(tMsg) );
		MSG_Peek( g_CurrentTaskID, &tMsg );
		IV_MsgProc( &tMsg );
		usleep( 100 );
	}

	int ret = _IVProperty.m_ExitState;
	if( _IVProperty.m_ExitState == TaskExit_Normal ||
		_IVProperty.m_ExitState == TaskExit_ExitAndSwitch ) {
		IV_Kill();
	}

	TAVI_WakeUp( TASK_ID_IMAGE );
	return ret;
}

CList< CWindow >* IV_GetWndList( void )
{
	return &_IVProperty.m_WndList;
}

int IV_Kill( void )
{
	delete g_pRootWnd;
	g_pRootWnd = NULL;

	TAVI_CannotSleep( TASK_ID_IMAGE );

	// DO NOT USE CList::FreeAll()!!!
	CNode< CWindow >* pNode = _IVProperty.m_WndList.DetachHead();
	while( pNode ) {
		delete pNode;
		pNode = _IVProperty.m_WndList.DetachHead();
	}
	memset( (void*)&_IVProperty, 0, sizeof(_IVProperty) );

	if( g_p_ThumbDB ) delete g_p_ThumbDB;
	g_p_ThumbDB = 0;
}

bool IV_IsViewingOnForeground( void )
{
    if ( pTAVI->ssTaskId == TASK_ID_IMAGE ) {
        if (  strcmp( _IVProperty.m_WndList[0]->GetWindowTitle().GetString(), "ImageViewer" ) == 0 )
            return TRUE;
    }

    return FALSE;	
}
