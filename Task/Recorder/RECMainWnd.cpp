//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Recorder Main Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RECMainWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: RECMainWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.14  2006/02/09 05:19:08  zstein
// *** empty log message ***
//
// Revision 1.13  2005/11/10 01:09:16  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <Task/tavi_global.h>
#include <Task/ExtLCD.h>
#include <Task/MiscCtrl.h>
#include <Lib/LibImage.h>
#include "RecorderMain.h"
#include "RECMainWnd.h"
#include "RecordWnd.h"
#include "RecFolderView.h"

// macros

// extern variables
extern CMiscCtrl* g_MiscCtrl;
extern CExtLCD* g_ExtLCD;
extern CTextEngine_BDF* pT;

static unsigned char* GetImageElement( BITMAPFILEINFO256 bmp, int dh, int id, int count )
{
	unsigned char* p;
	int w = bmp.bitmapinfoheader.biWidth;
	p = bmp.image_buffer+w*dh*id;

	return p;
}

CRECMainWnd::CRECMainWnd( CWindow* parent, const CString& szRoot ) :
	CWindow( parent ),
	m_szRootPath( szRoot )
{
	CListItem* puzItem;
	m_pListView = new CRECMainListView( this );
	m_pListView->Resize( CRect(13, 0, 318, 180), false );

	#if 0
	puzItem = new CListItem( 0, 26, 27, ML_VOICE_RECORDING );
	m_pListView->AddItem( puzItem );
	if( g_MiscCtrl->CradleIN() ) {
		puzItem = new CListItem( 1, 38, 39, ML_LINE_IN_RECORDING );
	}
	m_pListView->AddItem( puzItem );
	puzItem = new CListItem( 2, 4, 5, ML_FOLDER );
	m_pListView->AddItem( puzItem );
	#endif
	m_pListView->SetWindowList( REC_GetWndList() );
	m_pListView->RegisterWndList();

	m_pRecWnd = NULL;

	// Ext. LCD
	if( g_MiscCtrl->FolderOpen() == false ) g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
	puzItem = m_pListView->GetSelectedItem();
	CExtLCD::ExtLCDListItem item;
	item.m_nIndex = puzItem->m_nIdx;
	item.m_nTotal = m_pListView->GetCount();
	item.m_nIcon = CExtLCD::EXT_LCD_ICON_RECORD;
	item.m_szText.SetString( (char*)pT->GetTextRes(puzItem->m_nResID), true );
	g_ExtLCD->SetListItem( item );
	if( g_MiscCtrl->FolderOpen() == false ) g_ExtLCD->Update();
}

CRECMainWnd::~CRECMainWnd( void )
{
}

void CRECMainWnd::OnWindowEvent( const WindowEvent& evt )
{
	CEvent event;
	
	if( evt.nWindowID == m_nWindowID ) {
		switch( evt.nMessage ) {
		case ON_VOICERECORD:
			OnVoiceRecord();
			break;
		case ON_LINEINRECORD:
			OnLineinRecord();
			break;
		case ON_FOLDERVIEW:
			OnFolderView();
			break;
		case WINDOW_NOTIFY:
			event.type = EVENT_WINDOW;
			event.u.window = evt;
			m_pListView->SendEvent( event );
			if( g_pFocusedWnd != m_pListView ) {
				g_pFocusedWnd->SendEvent( event );
			}
			break;
		}
	}
	else {
		event.type = EVENT_WINDOW;
		event.u.window = evt;
		if( g_pFocusedWnd ) SendEvent( event, g_pFocusedWnd );
	}
}

void CRECMainWnd::OnVoiceRecord( void )
{
	REC_Kill_Mp3Player();
	
	CRecorderWnd* pWnd = new CRecorderWnd( NULL, m_szRootPath, CRecorderWnd::RECORD_VOICE );
	pWnd->Resize( CRect(0, 30, 319, 239), false );
	CWindow::GetFocusedWindow()->Hide();
	pWnd->SetFocus();
	pWnd->SetWindowList( REC_GetWndList() );
	pWnd->RegisterWndList();
	CWindow::GetFocusedWindow()->Show();

	m_pRecWnd = pWnd;
	mep_eat_event();
}

void CRECMainWnd::OnLineinRecord( void )
{
	REC_Kill_Mp3Player();
	
	CRecorderWnd* pWnd = new CRecorderWnd( NULL, m_szRootPath, CRecorderWnd::RECORD_LINEIN );
	pWnd->Resize( CRect(0, 30, 319, 239), false );
	CWindow::GetFocusedWindow()->Hide();
	pWnd->SetWindowList( REC_GetWndList() );
	pWnd->RegisterWndList();
	pWnd->SetFocus();
	if( pWnd->IsValid() == false ) {
		DrawMessageBox("Record", "Record Module has a some problem.", 10, 79, 300, 100 );
		sleep( 5 );
		#if 0
		delete m_pFocusedWnd;
		m_pFocusedWnd = m_pListView;
		m_pFocusedWnd->Show();
		#endif
	}
	m_pRecWnd = pWnd;
	mep_eat_event();
}

void CRECMainWnd::OnFolderView( void )
{
	uchar_t buf[32];
	str2ustr( buf, _RECORD_DIR, strlen(_RECORD_DIR) );
	CString szDir;
	szDir.SetString( (char*)buf, true );
	CRecFolderView* pWnd = new CRecFolderView( NULL, szDir );
	pWnd->Resize( CRect(13, 30, 319, 216), false );
	CWindow::GetFocusedWindow()->Hide();
	pWnd->SetWindowList( REC_GetWndList() );
	pWnd->RegisterWndList();
	pWnd->SetFocus();
	#if 0
	m_pFocusedWnd->Hide();
	m_pFocusedWnd = pWnd;
	m_pFocusedWnd->Show();
	#endif
	m_pRecWnd = pWnd;
	mep_eat_event();
}

void CRECMainWnd::Show( void )
{
	m_pListView->SetFocus();
}

void CRECMainWnd::SetFocus( void )
{
	m_pListView->SetFocus();
}
