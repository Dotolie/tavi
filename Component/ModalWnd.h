//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Modal Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ModalWnd.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//

//******************************************************************************

// $Log: ModalWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2005/10/20 09:48:45  yong
// add _SetupPixmap(), _ClearPixmap()
//
// Revision 1.6  2005/10/17 07:53:06  yong
// refactoring
// always use font size 12 whether using resource ID or CString
//
// Revision 1.5  2005/08/18 12:18:30  zstein
// - update
//
// Revision 1.4  2005/04/27 11:40:01  zstein
// change bool OnOK(void) to bool OnOK(int& state).
// @state has three values. MODAL_OK, MODAL_CANCEL, MODAL_EXIT.
//
// Revision 1.3  2005/04/27 08:57:20  zstein
// added ok/cancel/exit button.
// changed title string to title resource id.
//
// Revision 1.2  2005/04/27 04:53:21  zstein
// Support Multi-languages.
// Support resource text.
//
// Revision 1.1  2005/03/01 02:02:02  zstein
// *** empty log message ***
//
//

//*******************************************************************************

#ifndef __MODAL_WINDOW_H__
#define __MODAL_WINDOW_H__

#include "window.h"
#include "String.h"
#include "Pixmap.h"

class CModalWnd : public CWindow
{
private:
	int changed;

	void _ClearPixmap(void);
	void _SetupPixmap(void);

protected:
	int m_nTitleColor;
	int m_nTitleHeight;
	bool m_fOK;
	
	CPixmap* m_pLeftTopEdge;
	CPixmap* m_pRightTopEdge;
	CPixmap* m_pLeftBottomEdge;
	CPixmap* m_pRightBottomEdge;

	CPixmap* m_pOK;
	CPixmap* m_pCancel;
	CPixmap* m_pExit;
	CPixmap* m_pEdit;

	int m_nOkResID;
	int m_nCancelResID;
	int m_nExitResID;
	int m_nEditResID;
	int m_nButtonState;

	int m_nTitleResID;
	CString m_szTitle;
	
	void OnPaint( const CRect& rc );
public:
	CModalWnd( CWindow* parent );
	~CModalWnd( void );

	enum {
		MODAL_EXIT,
		MODAL_CANCEL,
		MODAL_OK
	};

	void UnsetTitle( void );
	void SetTitle( int nResID ) { m_nTitleResID = nResID; }
	void SetTitle( char *title ) { m_szTitle = CString(title); }

	void SetTitleColor( int color ) {
		m_nTitleColor	= color;
		changed			= 1;
	}

	bool OnOK( int& state ) { state = m_nButtonState; return m_fOK; }

	void BindKeyLabel( int nOkResID, int nCancelResID, int nExitResID ) {
		m_nOkResID		= nOkResID;
		m_nCancelResID	= nCancelResID;
		m_nExitResID	= nExitResID;
	}
};

#endif /* __MODAL_WINDOW_H__ */
