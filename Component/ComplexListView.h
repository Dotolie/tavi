//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Complex ListView Class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/ComplexListView.h,v $

// $Revision: 1.2 $

// $Date: 2006/06/12 04:48:45 $

// $Author: ywkim $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: ComplexListView.h,v $
// Revision 1.2  2006/06/12 04:48:45  ywkim
// Add function of ~CCMListItem(void) {delete m_pComponent;}
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/10/17 05:22:17  zstein
// - deleted AddItem(...)
//
// Revision 1.3  2005/03/01 07:35:30  ywkim
// *** empty log message ***
//
// Revision 1.2  2005/03/01 06:58:04  zstein
// Added CCMListItem::CCMListItem( CWindow* component, int index )
//
// Revision 1.1  2005/02/25 01:12:18  zstein
// Complex Label & View
//
//

//******************************************************************************

#ifndef __COMPLEX_LISTVIEW_CLASS_H__
#define __COMPLEX_LISTVIEW_CLASS_H__

#include <Component/ListView.h>

class CCMListItem : public CListItem
{
public:
	CCMListItem( CWindow* component ) : 
		CListItem( -1, -1, -1, 0 ),
		m_pComponent( component )
	{
	}
	CCMListItem( CWindow* component, int index ) :
		CListItem( index, -1, -1, 0 ),
		m_pComponent( component )
	{
	}
	CCMListItem( void ) {}
	~CCMListItem(void) {
		delete m_pComponent;
		}
	CWindow* m_pComponent;
};

class CComplexListView : public CListView
{
private:
protected:
	void OnKeypressed( const KeyEvent& evt );
	void DrawItem( CListItem* puzItem, int idx, bool fFocused );
public:
	CComplexListView( CWindow* parent );
	~CComplexListView( void );

	void Resize( const CRect& rc, bool fUpdate );
};

#endif /* __COMPLEX_LISTVIEW_CLASS_H__ */
