//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : FolderView class declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/FolderView.h,v $
// $Revision: 1.2 $
// $Date: 2006/04/17 04:28:12 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: FolderView.h,v $
// Revision 1.2  2006/04/17 04:28:12  zstein
// - added SetTopTitle() method and fixed TaviCast bug.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.16  2005/11/25 10:21:58  zstein
// *** empty log message ***
//
// Revision 1.15  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
// Revision 1.14  2005/09/20 12:22:45  zstein
// - update
//*******************************************************************************
#ifndef __FOLDER_VIEW_H__
#define __FOLDER_VIEW_H__

#include <tavi_lib.h>
#include <Component/Stack.h>
#include <Component/ListView.h>
#include <Component/ContextMenu.h>
#include "ID3/m3u.h"

typedef struct {
	CString folder;
	int start_idx;
	int sel_idx;
} FolderPrev;

class CFolderView : public CListView
{
private:
	void Init( CString& szRootPath );
	bool ScanDir( CString& szPath );
protected:
	int m_nSort;
	int m_nFolders;
    bool m_bSave;
	CMsgBox* m_pMsg;
	struct timeval m_tv;
	
	void OnKeypressed( const KeyEvent& evt );
	void OnPaint( const CRect& rc );
	void OnWindowEvent( const WindowEvent& evt );
	void OnMouseEvent( const MouseEvent& evt );
	void Flickering( bool fShow );
	void CreateContextMenu();
    void SetTopTitle( void );
public:
	CContextMenu* m_pCtxMenuWnd;
	CStack< FolderPrev > m_Stack;
	CString m_szCurrentDir;
	enum {
		SORT_NAME_ASCENT,
		SORT_NAME_DESCENT,
		SORT_DATE_ASCENT,
		SORT_DATE_DESCENT
	};
	
	CFolderView( CWindow* parent, CString& szRootPath, int sort=SORT_NAME_ASCENT, bool bSave=true  );
	~CFolderView( void );

	static int AddToPlaylist( RMuint8* arg );
	void Update( void );
	void Show( void );
};

#endif /* __FOLDER_VIEW_H__ */

