//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Bottom Window class declaration

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/BottomWnd.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: BottomWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/11/28 00:50:02  zstein
// *** empty log message ***
//
// Revision 1.1  2005/03/01 01:57:10  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/21 02:11:39  zstein
// BUG fixed & update
//
// Revision 1.1  2005/02/16 16:17:37  zstein
// *** empty log message ***
//

//*******************************************************************************

#ifndef __BOTTOM_WINDOW_CLASS_H__
#define __BOTTOM_WINDOW_CLASS_H__

#include <Component/window.h>
#include <Component/Pixmap.h>

class CBottomWnd : public CWindow
{
private:
protected:
	CPixmap* m_puzLeftBtn;
	CPixmap m_puzRightBtn[8];
	CPixmap* m_puzBottom;
	bool m_flButtonShow;
	bool m_frButtonShow;
	int m_nCurrentRButton;
	int m_nMode;

	void OnPaint( const CRect& rc );
public:
	enum {
		BW_NORMAL,
		BW_TRANSPARENT
	};
	enum {
		RB_TOOL=0,
		RB_SEEK,
		RB_VOLUME,
		RB_PLAY
	};
	CBottomWnd( CWindow* parent, int mode  );
	~CBottomWnd( void );

	//void ShowRightButton( int idx, bool fFlush=false );
	//void SetRightButton( int idx );
	//void ShowLeftButton( bool fFlush=false );
	void SetMode( int mode ) { m_nMode = mode; }
	//void HideRightButton( void );
	//void HideLeftButton( void );
};

#endif /* __BOTTOM_WINDOW_CLASS_H__ */
