//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Vertical Scrollbar Class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/VScrollBar.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: VScrollBar.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/07/20 06:09:43  zstein
// - updated.
//
// Revision 1.3  2005/07/16 04:13:52  zstein
// - fixed bugs & added functions.
//
// Revision 1.2  2005/02/25 01:13:09  zstein
// *** empty log message ***
//
// Revision 1.1  2005/02/23 02:47:05  zstein
// Vertical ScrollBar Class
//
//

//******************************************************************************

#ifndef __VERTICAL_SCROLLBAR_H__
#define __VERTICAL_SCROLLBAR_H__

#include "ScrollBar.h"
#include "Pixmap.h"

class CVScrollBar : public CCScrollBar
{
private:
protected:
	CPixmap* m_pmLeftTop;
	CPixmap* m_pmRightTop;
	CPixmap* m_pmLeftBottom;
	CPixmap* m_pmRightBottom;
	CPixmap* m_pmScrollBar;
	int m_nYPos;
	
	void OnPaint( const CRect& rc );
	void DrawScrollBar( void );
public:
	CVScrollBar( CWindow* parent, int nViewCount, int nTotal );
	virtual ~CVScrollBar( void );

	void Resize( const CRect& rc, bool fUpdate );
	void UpdateScrollBar( int nStart );
	void ResizeScrollBar( void );
	void SetScrollBackgroundColor( int nColor );
};

#endif /* __VERTICAL_SCROLLBAR_H__ */