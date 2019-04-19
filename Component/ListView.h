//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : List view class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ListView.h,v $
// $Revision: 1.3 $
// $Date: 2006/06/07 08:21:33 $
// $Author: zstein $
//
//******************************************************************************
// $Log: ListView.h,v $
// Revision 1.3  2006/06/07 08:21:33  zstein
// - removed unused codes.
//
// Revision 1.2  2006/04/15 02:09:50  zstein
// - added item color variable.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.26  2005/12/01 01:07:31  zstein
// *** empty log message ***
//
//*******************************************************************************

#ifndef __CLISTVIEW_H__
#define __CLISTVIEW_H__

#include "window.h"
#include "Pixmap.h"
#include "VScrollBar.h"
#include "String.h"
#include "Array.h"
#include <Task/tavi_global.h>


enum {
	NODE_TYPE_FILE,
	NODE_TYPE_FOLDER,
	NODE_TYPE_VIRTUAL,
	NODE_TYPE_USER
};
class CListItem 
{
public:
	CListItem();
	CListItem( int nIdx, int nIconIdx, int nSelectedIconIdx, char* szText, bool fUnicode=false );
	CListItem( int nIdx, int nIconIdx, int nSelectedIconIdx, int nResID );
	virtual ~CListItem();
	int m_nIdx;
	int m_nIconIdx;
	int m_nSelectedIconIdx;
	int m_nMedia;
	CString m_szText;
	int m_nType;
	bool m_fScroll;
	int m_nWidthInPixel;
	int m_nScrollWidth;
	int m_nResID;
	int m_nDate;
	int m_nkey;
	bool m_nEnable;
    int m_color;
};

enum {
	CLIST_COLTYPE_TEXT,
	CLIST_COLTYPE_IMAGE
};


class CListView : public CWindow
{
private:
protected:
	//CList< CListItem > m_uzList;
	CArray< CListItem* > m_uzList;
	CList< CPixmap > m_uzIconList;
	int m_nListCount;
	int m_nStartIdx;
	int m_nSelectedIdx;
	int m_nBarHeight;
	CPixmap* m_puzBarPixmap;
	CSize m_uzBarSize;
	CSize m_uzIconSize;
	int m_nOffset; // x-offset list bar
	int m_nScrollBarTM; // top margin of scrollbar
	int m_nScrollBarBM; // bottom margin of scrollbar
	int m_nSelectedFontIdx;
	int m_nNormalFontIdx;
	int m_nSort;
	CVScrollBar* m_pScrollBar;
	bool m_fUpdate;
	int m_nUpdateDir; // 0: up, 1: down
	CPixmap* m_puzBarText;
	int m_nTextScrollPos;
	int m_nStartWaitCnt;

	virtual void DrawItem( CListItem* uzItem, int idx, bool fSelect );
	virtual void DrawBackground( void );
	
	void OnPaint( const CRect& rc );
	void OnKeypressed( const KeyEvent& evt );
	void OnMouseEvent( const MouseEvent& evt );
	void OnWindowEvent( const WindowEvent& evt );
	void UpdateList( int prev, int next );
	void SetScrollBarPos( const CRect& rc ) { m_pScrollBar->rcClient = rc; }
public:
	enum {
		NAME_ASCENT,
		NAME_DESCENT,
		DATE_ASCENT,
		DATE_DESCENT
	};
	CListView( CWindow* parent=0, int nSort=-1, int nViewCount=8 );
	~CListView();

	CListItem* GetItem( int nIdx );
	int GetCount( void );
	void SetSelectBarPixmap( CPixmap* pixmap );
	int AddPixmapIcon( CPixmap* pixmap );
	void ClearList( void );
	CListItem* GetSelectedItem( void );
	int GetSelectedIndex( void ) { 
		return m_nStartIdx + m_nSelectedIdx; 
	}
	void Resize( const CRect& rc, bool fUpdate );
	int GetBarWidth( void ) const { return m_puzBarPixmap->m_nWidth; }
	int GetBarHeight( void ) const { return m_puzBarPixmap->m_nHeight; }
	void Sort( int type );
	int GetSort( void ) const { return m_nSort; }
	int DeleteItem( int index );
	void ResetIconList( void );
	
	// Virtual methods
	virtual int AddItem( CListItem* uzItem );
	virtual bool ScrollText( void );
	virtual void ResetScrollBar( void );
};

#endif /* __CLISTVIEW_H__ */
