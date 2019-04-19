//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Recorder MAIN List View Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RECMainListView.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: RECMainListView.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/12/05 04:57:26  zstein
// *** empty log message ***
//
// Revision 1.10  2005/11/18 09:14:49  zstein
// *** empty log message ***
//
// Revision 1.9  2005/11/11 12:29:20  zstein
// *** empty log message ***
//
// Revision 1.8  2005/11/10 01:09:15  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <Task/Msg.h>
#include <Task/tavi_global.h>
#include <Task/MiscCtrl.h>
#include <Task/ExtLCD.h>
#include <Lib/LibImage.h>
#include "RECMainListView.h"
#include "RECMainWnd.h"

extern CMiscCtrl* g_MiscCtrl;
extern CExtLCD* g_ExtLCD;
extern CTextEngine_BDF* pT;

CRECMainListView::CRECMainListView( CWindow* parent ) :
	CListView( parent, -1 )
{
	MakeList();
}

CRECMainListView::~CRECMainListView( void )
{
	printf( "CRECMainListView::~CRECMainListView( void )\n" );
}

void CRECMainListView::MakeList( void )
{
	m_uzList.FreeAll();
	CListItem* puzItem;
	puzItem = new CListItem( 0, 26, 27, ML_VOICE_RECORDING );
	AddItem( puzItem );
	if( g_MiscCtrl->CradleIN() ) {
		puzItem = new CListItem( 1, 38, 39, ML_LINE_IN_RECORDING );
		AddItem( puzItem );
	}
	puzItem = new CListItem( 2, 4, 5, ML_FOLDER );
	AddItem( puzItem );
}

void CRECMainListView::OnKeypressed( const KeyEvent& evt )
{
	CListItem* puzItem;
	TMsgEntry msg;
	CExtLCD::ExtLCDListItem item;
	KeyEvent kevt = evt;

	puzItem = GetSelectedItem();
	switch( evt.value ) {
	case TAVI_KEY_JOG2_ENTER:
	case TAVI_KEY_JOG2_UP:
		if( g_MiscCtrl->FolderOpen() ) break;
	case VK_ENTER:
		if( puzItem->m_nIdx == 0 ) { // voice record
			SendMessage( ON_VOICERECORD, m_parent->GetWindowID() );
		}
		else if( puzItem->m_nIdx == 1 ) { // line-in record or Folderviewer
			if( g_MiscCtrl->CradleIN() )
				SendMessage( ON_LINEINRECORD, m_parent->GetWindowID() );
			else
				SendMessage( ON_FOLDERVIEW, m_parent->GetWindowID() );
		}
		else { // get in record folder
			SendMessage( ON_FOLDERVIEW, m_parent->GetWindowID() );
		}
		break;
	case TAVI_KEY_JOG1_ENTER:
		if( g_MiscCtrl->FolderOpen() ) break;
	case VK_ESC:
	case VK_F3:
		SendMessage( WINDOW_KILL, m_parent->GetWindowID() );
		break;
	case TAVI_KEY_JOG1_UP:
		if( g_MiscCtrl->FolderOpen() ) break;
		kevt.value = VK_UP;
		break;
	case TAVI_KEY_JOG1_DOWN:
		if( g_MiscCtrl->FolderOpen() ) break;
		kevt.value = VK_DOWN;
		break;
	}
	CListView::OnKeypressed( kevt );

	puzItem = GetSelectedItem();
	item.m_nIndex = puzItem->m_nIdx;
	item.m_nTotal = GetCount();
	if( puzItem->m_nIdx < 2 ) 
		item.m_nIcon = CExtLCD::EXT_LCD_ICON_RECORD;
	else
		item.m_nIcon = CExtLCD::EXT_LCD_ICON_FOLDER;
	item.m_szText.SetString( (char*)pT->GetTextRes(puzItem->m_nResID), true );
	g_ExtLCD->SetListItem( item );
	if( g_MiscCtrl->FolderOpen() == false ) {
		g_ExtLCD->Update();
	}
}

void CRECMainListView::OnWindowEvent( const WindowEvent& evt )
{
	switch( evt.nMessage ) {
	case ON_FOLDER:
		if( evt.nParam ) g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
		else g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
		g_ExtLCD->Update();
		break;
	case WINDOW_NOTIFY:
		if( evt.noti.param1 == NOTIFY_CRADLE ) {
			MakeList();
			OnPaint( rcClient );
			Flush();
			#if 0
			CListItem* puzItem = m_uzList[1];
			puzItem->m_nEnable = evt.noti.param2;
			bool selected;
			selected = ( m_nSelectedIdx == 1 ) ? true : false;
			if( g_pFocusedWnd == this ) {
				DrawItem( puzItem, puzItem->m_nIdx, selected );
				Flush();
			}
			#endif
		}
		break;
	}
}

void CRECMainListView::Show( void )
{
	CListItem* pItem = ( CListItem* )GetSelectedItem();
	CExtLCD::ExtLCDListItem item;
	if( pItem->m_nIdx < 2 ) 
		item.m_nIcon = CExtLCD::EXT_LCD_ICON_RECORD;
	else
		item.m_nIcon = CExtLCD::EXT_LCD_ICON_FOLDER;
	item.m_nIndex = pItem->m_nIdx;
	item.m_nTotal = GetCount();
	item.m_szText.SetString( (char*)pT->GetTextRes(pItem->m_nResID), true );
	g_ExtLCD->SetListItem( item );
	if( g_MiscCtrl->FolderOpen() == false ) {
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
		g_ExtLCD->Update();
	}
	//CWindow::Show();
	DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME );
	OnPaint( rcClient );
}
