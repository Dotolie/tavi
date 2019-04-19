//************************ Copyright(C) 2004-2005 New Media Life Inc. ************************
//
// Description : Recorder
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecorderMain.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: RecorderMain.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.51  2006/02/02 00:29:57  zstein
// *** empty log message ***
//
// Revision 1.50  2006/02/01 00:34:31  zstein
// *** empty log message ***
//
// Revision 1.48  2006/01/19 09:19:47  zstein
// *** empty log message ***
//
// Revision 1.47  2005/12/29 02:55:11  zstein
// - release record
//
// Revision 1.45  2005/12/27 02:21:18  zstein
// *** empty log message ***
//
// Revision 1.44  2005/12/26 04:37:39  zstein
// *** empty log message ***
//
// Revision 1.40  2005/12/11 16:50:03  zstein
// *** empty log message ***
//
// Revision 1.39  2005/12/11 15:27:14  zstein
// *** empty log message ***
//
// Revision 1.38  2005/12/11 13:07:21  zstein
// *** empty log message ***
//
// Revision 1.37  2005/12/05 02:44:05  ywkim
// Add function of REC_IsRecording() for idle shutdown
//
// Revision 1.36  2005/12/05 02:41:37  zstein
// *** empty log message ***
//
// Revision 1.35  2005/11/28 02:39:46  zstein
// *** empty log message ***
//
// Revision 1.34  2005/11/22 09:07:10  zstein
// *** empty log message ***
//
// Revision 1.33  2005/11/17 04:24:38  zstein
// - disable EQ
//
// Revision 1.32  2005/11/10 01:09:16  zstein
// *** empty log message ***
//
// Revision 1.31  2005/11/08 07:53:26  zstein
// *** empty log message ***
//
// Revision 1.30  2005/11/08 01:25:27  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <Config.h>
#include <Task/tavi_global.h>
#include <Task/TopPanel.h>
#include <Task/MiscCtrl.h>
#include <Task/ExtLCD.h>
#include <Task/SndCtrl.h>
#include <Task/AudioPlayer/PlayerWnd.h>
#include <Component/window.h>
#include <Lib/LibImage.h>
#include "RecorderMain.h"
#include "RECMainWnd.h"

// extern variables
extern CTopPanel* g_TopPanel;
extern int g_CurrentTaskID;
extern CMiscCtrl* g_MiscCtrl;
extern CExtLCD* g_ExtLCD;
extern CFolderPL* g_FolderPL;
extern CPlayerWnd* g_PlayerWnd;

extern void showbackground( int ID );

// Local variables
static struct RecordStruct {
	CWindow* m_pMainWnd;
	CPlayerWnd* m_pPlayer;
	bool m_fExit;
	bool m_recording;
	AudioCmd* m_pCmd;
	CList< CWindow > m_WndList;
} prop;


static void OnKeyEvent( const KeyEvent& evt )
{
	// exit keys
	switch( evt.value ) {
	case TAVI_KEY_JOG1_ENTER:
		if( g_MiscCtrl->FolderOpen() ) break;
	case VK_EXT_ESC:
	case VK_F3:
	case TAVI_KEY_SHUTDOWN:
		//delete g_pRootWnd;
		prop.m_fExit = true;
		return;
	}
	
	CEvent event;
	event.type = EVENT_KEY;
	event.u.key = evt;
	//prop.m_pMainWnd->SendEvent( event );
	CWindow::GetFocusedWindow()->SendEvent( event );
}

static void OnMouseEvent( const MouseEvent& evt )
{
	CEvent event;
	event.type = EVENT_MOUSE;
	event.u.mouse = evt;
	//prop.m_pMainWnd->SendEvent( event );
	CWindow::GetFocusedWindow()->SendEvent( event );
}

static void OnWindowEvent( const WindowEvent& evt )
{
	CEvent event;
	event.type = EVENT_WINDOW;
	event.u.window = evt;

	CWindow* pWnd;
	CNode< CWindow >* pNode;

	if( evt.nMessage == WINDOW_QUEUE ) {
		event.u.window.nParam = (int)prop.m_pCmd;
	}
	
	prop.m_WndList.Rewind();
	pWnd = prop.m_WndList.Head();
	while( pWnd ) {
		if( pWnd->GetWindowID() == evt.nWindowID ) break;
		pWnd = prop.m_WndList.Next();
	}

	if( pWnd ) {
		pWnd->SendEvent( event );
	}
	else {
		prop.m_pMainWnd->SendEvent( event );
	}

	switch( evt.nMessage ) {
	case WINDOW_KILL:
		if( pWnd->GetWindowID() == prop.m_pPlayer->GetWindowID() ) {
			REC_Kill_Mp3Player();
		}
		else {
			delete pWnd;
		}
		g_pFocusedWnd = prop.m_WndList.Head();
		if( g_pFocusedWnd ) g_pFocusedWnd->Show();
		else {
			prop.m_fExit = true;
			// killed main window
			prop.m_pMainWnd = NULL;
		}
		break;
	case WINDOW_KILL_TASK:
		prop.m_fExit = true;
		break;
	case WINDOW_KILL_FOCUS:
		#if 0
		pNode = prop.m_WndList.GetNode( 0 );
		if( pNode->m_Data == g_pFocusedWnd ) {
			g_pFocusedWnd->Hide();
			g_pFocusedWnd->SetVisible( false );
			pNode = prop.m_WndList.DetachHead();
			prop.m_WndList.AddNodeToTail( pNode );
		}
		prop.m_WndList.Head()->SetFocus();
		#endif
		pNode = prop.m_WndList.Detach( pWnd );
		if( pNode ) {
			CWindow* wnd = ( CWindow* )pNode->m_Data;
			wnd->Hide();
			wnd->SetVisible( false );
			prop.m_WndList.AddNodeToTail( pNode );
			prop.m_WndList.Head()->SetFocus();
		}
		break;
	}
}

int REC_MsgProc( TMsgEntry* msg )
{
	CEvent event;
	MouseEvent mouse;
	KeyEvent key;
	WindowEvent window;
	TimerEvent tevt;
	bool fExpire = false;

	switch( msg->ulMsgId )
	{
	case MSG_CREATE:
		break;
	case MSG_DRAW:
		if( prop.m_pMainWnd ) prop.m_pMainWnd->Show();
		else printf( "Main Window is NULL\n" );
		break;
	case MSG_SETFOCUS: 	break;
	case MSG_KILLFOCUS: break;
	case MSG_DESTROY:	break;
	case MSG_BUTTON_UP:
	case MSG_BUTTON_REAL_UP:
		key.value = MapToVirtualKey( msg->usParam1 );
		key.pressed = msg->usParam2;
		OnKeyEvent( key );
		break;
	case MSG_TOUCHPAD:
		mouse.x = msg->usParam1;
		mouse.click = msg->usParam2;
		OnMouseEvent( mouse );
		break;
	case MSG_WINDOW:
		window.nWindowID = msg->usParam1;
		window.nMessage = msg->usParam2&0xffff0000;
		window.nParam = msg->usParam2&0xffff;
		OnWindowEvent( window );
		break;
	case MSG_FOLDER:
		window.nWindowID = g_pFocusedWnd->GetWindowID();
		window.nMessage = ON_FOLDER;
		window.nParam = g_MiscCtrl->FolderOpen();
		OnWindowEvent( window );
		break;
	case MSG_NOTIFY:
		window.nWindowID = prop.m_pMainWnd ->GetWindowID();
		window.nMessage = WINDOW_NOTIFY;
		window.noti.param1 = msg->usParam1;
		window.noti.param2 = msg->usParam2;
		OnWindowEvent( window );
		break;
	default:
		if( CWindow::GetFocusedWindow() ) {
			window.nWindowID = CWindow::GetFocusedWindow()->GetWindowID();
			window.nMessage = WINDOW_IDLE;
			OnWindowEvent( window );
		}
		break;
	}

	return TRUE;

}

int REC_Task( TTaskInitParam* param )
{
	TMsgEntry msg;
	QID_t qid;

	prop.m_pCmd = new AudioCmd;
	QBUF_t* pBuf = ( QBUF_t* )prop.m_pCmd;

	TAVI_CanSleep( TASK_ID_RECORD );
	
	op_ClearAreaOSD( 
		0, g_TopPanel->rcABS.GetHeight(), 
		320, 240-g_TopPanel->rcABS.GetHeight() );
	showbackground( ID_BG_RECORD );

	g_CurrentTaskID = param->ssTaskId;
	CRECMainWnd* pMainWnd = new CRECMainWnd( NULL, _RECORD_DIR );
	pMainWnd->Resize( CRect(0, 30, 319, 239), false );
	prop.m_pMainWnd = pMainWnd;
	pMainWnd->SetWindowList( &prop.m_WndList );
	pMainWnd->RegisterWndList();
	//pMainWnd->Show();
	//g_pRootWnd = pMainWnd;
	TAVI_LoadPaletteTransparencyData(TRUE);
	// create record folder
	RMuint32 fdDir;
	fatfs_dopen( (RMuint8*)"/hd1", &fdDir );
	fatfs_dcreate_dir( fdDir, (RMuint8*)"Record" );
	fatfs_dclose( fdDir );
	SetTaviKeyMode( TK_REPEAT | TK_SINGLE );
	ClearKeyBuffer();
	mep_eat_event();

	prop.m_fExit = false;
	prop.m_recording = false;
	
	int ret;
	while( prop.m_fExit == false ) {
		if( prop.m_pPlayer ) {
			qid = prop.m_pPlayer->GetQID();
			pBuf->lLength = sizeof(AudioCmd)-4;
			if( SysMSGRecv( qid, pBuf, MSG_RECV_ID, IPC_NOWAIT ) == QError_NOERROR ) {
				msg.ssToTaskId = g_CurrentTaskID;
				msg.ulMsgId = MSG_WINDOW;
				msg.usParam1 = prop.m_pPlayer->GetWindowID();
				msg.usParam2 =  WINDOW_QUEUE;
				REC_MsgProc( &msg );
				continue;
			}
		}

		memset( (void*)&msg, 0, sizeof(msg) );
		MSG_Peek_Ex( 
			param->ssTaskId, 
			&msg, 
			MSG_PEEK_ALL );

		REC_MsgProc( &msg );

		if( prop.m_pPlayer ) usleep( 100 );
	}

	REC_Kill();
	
	if( g_MiscCtrl->FolderOpen() == false ) {
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_MENU );
		g_ExtLCD->Update();
	}

	//g_SndCtrl->LineInBypass( false );

	TAVI_CannotSleep( TASK_ID_RECORD );

	return TaskExit_Normal;
}


static CFolderPL* s_MusicFolderPL;
void REC_MP3Play( const CString& fullpath )
{
	g_pFocusedWnd->Hide();
	if( !prop.m_pPlayer ) {
		s_MusicFolderPL = g_FolderPL;
		g_FolderPL = NULL;
		prop.m_pPlayer = new CPlayerWnd( NULL );
		prop.m_pPlayer->Resize( CRect(0, 30, 319, 209), false );
		prop.m_pPlayer->SetWindowList( &prop.m_WndList );
		prop.m_pPlayer->RegisterWndList();
		prop.m_pPlayer->SetFocus();
		prop.m_pPlayer->EnableEQ( false );
		g_PlayerWnd = prop.m_pPlayer;
	}
	else {
		prop.m_pPlayer->SetFocus();
	}
	CString base = "/hd1/Record";
	base.toUTF16();
	prop.m_pPlayer->Play( TYPE_FOLDER, base, NULL, fullpath );
}

void REC_Kill_Mp3Player( void )
{
	if( prop.m_pPlayer ) {
		delete prop.m_pPlayer;
		prop.m_pPlayer = NULL;
		g_PlayerWnd = NULL;
		delete g_FolderPL;
		g_FolderPL = s_MusicFolderPL;
		s_MusicFolderPL = NULL;
	}
}

CList< CWindow >* REC_GetWndList( void )
{
	return &prop.m_WndList;
}

int REC_Kill( void )
{
	REC_Kill_Mp3Player();

	if( prop.m_pMainWnd ) {
		delete prop.m_pMainWnd;
		prop.m_pMainWnd = 0;
	}

	if( prop.m_pCmd ) delete prop.m_pCmd;
	prop.m_pCmd = 0;

	// DO NOT USE CList::FreeAll()!!!
	CNode< CWindow >* pNode = prop.m_WndList.DetachHead();
	int count=0;
	while( pNode ) {
		delete pNode;
		pNode = prop.m_WndList.DetachHead();
	}

	#if XEN_LIB
	pMpegDecoder->XEN_SetPower( false );	// XEN_LIB power off
	#endif	// XEN_LIB

	
	memset( (void*)&prop, 0, sizeof(prop) );
}

void REC_SetProperty( int name, int value )
{
	switch( name ) {
	case REC_PROP_M_RECORDING:
		prop.m_recording = value;
		break;
	}
}

bool REC_IsRecording( void )
{
	return prop.m_recording;
}