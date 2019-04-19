//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Radio Button class declaration

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/RadioButton.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: RadioButton.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2005/10/17 11:26:05  yong
// nothing special :-(
//
// Revision 1.6  2005/10/13 10:46:40  zstein
// - changed managing objects.
//
// Revision 1.5  2005/04/27 06:18:12  zstein
// BUG FIXED: invalid position.
//
// Revision 1.4  2005/04/27 04:53:21  zstein
// Support Multi-languages.
// Support resource text.
//
// Revision 1.3  2005/03/01 02:11:05  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/25 01:11:38  zstein
// Radio Button Class
//
// Revision 1.1  2005/02/16 16:03:25  zstein
// *** empty log message ***
//

//*******************************************************************************

#ifndef __RADIO_BUTTON_H__
#define __RADIO_BUTTON_H__

#include "window.h"
#include "String.h"

class CRadioButton : public CWindow
{
private:
protected:
	class RadioItem {
	public:
		CString m_szText;
		int m_nResID;
		bool m_fText; // true: text, false: resource id
		int m_nWidth;
		RadioItem( const CString& sz ) { m_szText = sz; m_fText = true; }
		RadioItem( int resid ) { m_nResID = resid; m_fText = false; }
	};
	CList< RadioItem > m_uzList;
	CPixmap* m_uzSelectedIcon;
	CPixmap* m_uzUnSelectedIcon;
 	int m_nSelectedIdx;
	bool m_fItemType;
	int m_nItemCount;
	CSize m_CanvasSize;
	
	void OnPaint( const CRect& rc );
	void OnKeypressed( const KeyEvent& evt );
public:
	CRadioButton( CWindow* parent=0, bool fItemType=false );
	~CRadioButton( void );

	void AddItem( const CString& szText, bool fSelect );
	void AddItem( int id, bool fSelect );
	int GetSelectedIndex( void ) {
		return m_nSelectedIdx;
	}
	void SetSelectIndex( int idx ) { m_nSelectedIdx = idx; }
};

#endif /* __RADIO_BUTTON_H__ */
