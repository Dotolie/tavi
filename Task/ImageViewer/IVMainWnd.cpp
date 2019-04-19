//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Main Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVMainWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVMainWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.25  2006/01/24 00:53:24  zstein
// *** empty log message ***
//
// Revision 1.24  2006/01/18 05:34:39  zstein
// *** empty log message ***
//
// Revision 1.23  2005/11/22 09:06:44  zstein
// *** empty log message ***
//
// Revision 1.22  2005/11/16 07:11:54  zstein
// - unblock tileviewer
//
// Revision 1.21  2005/11/08 07:25:54  zstein
// *** empty log message ***
//
// Revision 1.20  2005/11/08 01:18:53  zstein
// *** empty log message ***
//
//******************************************************************************

#include <unistd.h>
#include <sys/time.h>
#include <Task/TopPanel.h>
#include <Task/Msg.h>
#include <Task/tavi_global.h>
#include <meplib.h>
#include <Component/image.h>
#include <Lib/LibUtilities.h>

#include "ImageViewer.h"
#include "IVMainWnd.h"
#include "IVSListView.h"
#include "IVPreView.h"
#include "IVTileViewer.h"

// Macros

// Extern Variables
extern CTopPanel* g_TopPanel;
extern void showbackground( int nMenuID );

// Local Variables

CIVMainWnd::CIVMainWnd( CWindow* parent, IVViewMode nViewMode ) :
	CWindow( parent ),
	m_nViewMode( nViewMode )
{
	CIVListView* pListView;

	CString sz;
	sz.SetString( TaviSysParam.photo.lastaccess, true );

	switch( m_nViewMode ) {
	case IV_LISTMODE_SIMPLE:
		pListView = new CIVSListView( NULL, sz, CIVSListView::SORT_NAME_ASCENT, NULL );
		pListView->Resize( CRect(13, 30, 319, 216), false );
		break;
	case IV_LISTMODE_PREVIEW:
		pListView = new CIVPreViewList( NULL, sz, CIVListView::SORT_NAME_ASCENT, NULL );
		pListView->Resize( CRect(13, 30, 319, 216), false );
		break;
	case IV_LISTMODE_THUMBNAIL:
		pListView = new CIVTileViewer( NULL, sz, CIVListView::SORT_NAME_ASCENT, NULL );
		pListView->Resize( CRect(6, 32, 319, 216), false );
		break;
	}

	pListView->SetWindowList( IV_GetWndList() );
	pListView->RegisterWndList();
	pListView->SetFocus();

	// Create TouchMenu
	mep_report( MEP_REPORT_RELATIVE );
}

CIVMainWnd::~CIVMainWnd( void )
{
}

void CIVMainWnd::OnWindowEvent( const WindowEvent& evt )
{
	// hide touch menu
	struct timeval tvCur;

	switch( evt.nMessage ) {
	case ON_VIEWTYPE:
		OnViewType( evt.nParam );
		break;
	case WINDOW_IDLE:
		break;
	}
}

void CIVMainWnd::OnViewType( int type )
{
	CIVListView* pOldView = ( CIVListView* )g_pFocusedWnd;
	CIVListView* pView;


	switch( type ) {
	case IV_LISTMODE_SIMPLE:
		pView = new CIVSListView( NULL, pOldView->m_szCurrentPath, TaviSysParam.sort, NULL );
		pView->Resize( CRect(13, 30, 319, 216), false );
		break;
	case IV_LISTMODE_PREVIEW:
		pView = new CIVPreViewList( NULL, pOldView->m_szCurrentPath, TaviSysParam.sort, NULL );
		pView->Resize( CRect(13, 30, 319, 216), false );
		break;
	case IV_LISTMODE_THUMBNAIL:
		pView = new CIVTileViewer( NULL, pOldView->m_szCurrentPath, TaviSysParam.sort, NULL );
		pView->Resize( CRect(6, 32, 319, 216), false );
		break;
	}
    // clear background
        showbackground( ID_BG_PHOTO );
       delete pOldView;
	
	
	TaviSysParam.photo.view = type;
	pView->SetWindowList( IV_GetWndList() );
	pView->RegisterWndList();
	pView->SetFocus();

	// eat touchpad event
	mep_eat_event();
}
