//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Recorder Folder View Class
// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecFolderView.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: RecFolderView.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/09/05 12:53:02  zstein
// - fixed ui bugs.
//
// Revision 1.3  2005/08/27 10:20:03  zstein
// - updated UI & fixed bugs
//
// Revision 1.2  2005/08/03 07:34:34  zstein
// - update
//
// Revision 1.1  2005/07/16 03:09:37  zstein
// - start
//
//

//*******************************************************************************

#ifndef __RECORD_FOLDER_VIEW_H__
#define __RECORD_FOLDER_VIEW_H__

#include <Component/ListView.h>
#include <Component/Stack.h>
#include <Component/ContextMenu.h>
#include <Lib/LibImage.h>

typedef struct {
	CString folder;
	int start_idx;
	int sel_idx;
} FolderPrev;

class CFolderListItem : public CListItem
{
public:
	int m_nDate;

	CFolderListItem( int nIdx, int nIconIdx, int nSelectedIconIdx, char* szText, int date ) :
		CListItem( nIdx, nIconIdx, nSelectedIconIdx, szText ),
		m_nDate( date )
	{
		m_szText.SetString( szText, true );
	}
	~CFolderListItem( void ) {}
};

class CRecFolderView : public CListView
{
private:
protected:
	CString m_szRootPath;
	CString m_szCurrentDir;
	CWindow* m_pMainWnd;
	struct timeval m_tv;
	CMsgBox* m_pMsg;

	void OnWindowEvent( const WindowEvent& evt );
	void OnKeypressed( const KeyEvent& evt );
	bool ScanDir( CString& szPath );
	void OnPaint( const CRect& rc );

	void CreateContextMenu( void );
public:
	CStack< FolderPrev >* m_pStack;
	CContextMenu* m_pCtxMenuWnd;
	CRecFolderView( CWindow* parent, const CString& szRootPath );
	~CRecFolderView( void );

	void Show( void );
	int NumOfFolders();

};

#endif /* __RECORD_FOLDER_VIEW_H__ */
