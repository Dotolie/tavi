//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Scrollbar Class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/ScrollBar.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: ScrollBar.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/07/20 06:09:43  zstein
// - updated.
//
// Revision 1.2  2005/07/16 04:13:51  zstein
// - fixed bugs & added functions.
//
// Revision 1.1  2005/02/23 02:46:31  zstein
// ScrollBar Base Class
//
//

//******************************************************************************

#ifndef __SCROLLBAR_CLASS_H__
#define __SCROLLBAR_CLASS_H__

#include "window.h"

class CCScrollBar : public CWindow
{
private:
protected:
	int m_nTotalCount;
	int m_nStart;
	int m_nViewCount;
	int m_nScrollColor;
	int m_nScrollBGColor;
	int m_nScrollHeight;
public:
	CCScrollBar( CWindow* parent, int nViewCount=0, int nTotal=0 ) :
		CWindow( parent ),
		m_nTotalCount( nTotal ),
		m_nViewCount( nViewCount  )
	{
		m_nStart = 0;
		m_nBorderWidth = 0;
		m_nBorderColor = 0;
		m_nScrollColor = 0;
		m_nScrollBGColor = 0;
	}
	~CCScrollBar( void ) {}

	void SetTotal( int nTotal ) { 
		m_nTotalCount = nTotal; 
		ResizeScrollBar();
	}
	void SetViewCount( int nViewCount ) { m_nViewCount = nViewCount; }
	void SetStart( int nStart ) { m_nStart = nStart; }
	void SetScrollColor( int nColor ) { m_nScrollColor = nColor; }
	virtual void SetScrollBackgroundColor( int nColor ) { m_nScrollBGColor = nColor; }

	int GetViewCount( void ) { return m_nViewCount; }


	// Virutal methods
	virtual void UpdateScrollBar( int nStart )=0;
	virtual void ResizeScrollBar( void ) = 0;
};

#endif /* __SCROLLBAR_CLASS_H__ */