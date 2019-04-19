//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer List Viewer
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVListView.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVListView.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.12  2005/12/02 13:40:47  zstein
// *** empty log message ***
//
// Revision 1.11  2005/12/01 01:03:57  zstein
// *** empty log message ***
//
// Revision 1.10  2005/11/11 12:29:00  zstein
// *** empty log message ***
//
// Revision 1.9  2005/11/08 01:18:53  zstein
// *** empty log message ***
//
//******************************************************************************

#ifndef __IMAGER_VIEWR_LISTVIEW_H__
#define __IMAGER_VIEWR_LISTVIEW_H__

#include <sys/time.h>
#include <unistd.h>
#include <Component/ListView.h>
#include <Component/Stack.h>
#include <Component/ContextMenu.h>
#include <Lib/LibMsgBox.h>

class CFolderListItem : public CListItem
{
public:
	int m_nWidth;
	int m_nHeight;
	bool m_fSuccess;
	unsigned char* m_pY;
	unsigned char* m_pUV;

	CFolderListItem( int nIdx, int nIconIdx, int nSelectedIconIdx, char* szText, int date ) :
		CListItem( nIdx, nIconIdx, nSelectedIconIdx, (char*)NULL )
	{
		m_szText.SetString( szText, true );
		m_nDate = date;
	}
	~CFolderListItem( void ) {}
};

typedef struct {
	CString folder;
	int start_idx;
	int sel_idx;
} FolderPrev;

class CIVListView : public CListView
{
protected:
	void OnKeypressed( const KeyEvent& evt );
	void OnWindowEvent( const WindowEvent& evt );
	bool ScanDir( void );
	void OnPaint( const CRect& rc );
	void CreateContextMenu( void );
	void SetTopTitle();
	virtual void BeforeScan( void ) {}

	CMsgBox* m_pMsg;
	struct timeval m_tv;
	bool m_guide;
public:
	CString m_szCurrentPath;
	CStack< FolderPrev >* m_pDirStack;
	CContextMenu* m_pCtxMenuWnd;
	
	CIVListView( CWindow* parent, const CString& szCurrentPath, int nSort, int nViewCount, CStack<FolderPrev>* pStack );
	~CIVListView( void );

	enum {
		SORT_NAME_ASCENT,
		SORT_NAME_DESCENT,
	};

	void SetSort( int sort );
	const CString& GetCurrentPath( void ) const { return m_szCurrentPath; }

	void Update( void );
	void Show( void );
	void Hide( void );
};

#endif /* __IMAGER_VIEWR_LISTVIEW_H__ */