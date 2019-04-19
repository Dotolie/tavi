//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Label class declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/Label.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: Label.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/12/16 12:23:01  yong
// add SetScrollStep() method
//
// Revision 1.10  2005/12/16 04:29:33  zstein
// *** empty log message ***
//
// Revision 1.9  2005/11/08 07:28:18  zstein
// *** empty log message ***
//
//*******************************************************************************

#ifndef __LABEL_H__
#define __LABEL_H__

#include "window.h"
#include "String.h"
#include "Pixmap.h"

class CLabel : public CWindow
{
private:
protected:
	CString m_szText;
	int m_nTextColor;
	int m_nFGFocusedColor;
	CPixmap* m_pmText;
public:
	int m_nFontWidth;
	int m_nFontHeight;
	int m_nFontType;
	int m_nFontIdx;
	int m_nFocusedFontIdx;
	int m_nJustify;
	int m_nResID;
	int m_nFontOption;
	bool m_bScrollable;
	bool m_wait;
	int m_nextscrpos;
	int m_pixelwidth;
	int m_pixelscroll;
	int m_nScrollStep;
	struct timeval m_stv, m_etv;

	void OnPaint( const CRect& rc );
	void OnFocus( bool fFocus );
public:
	enum {
		CLABEL_LEFT_JUSTIFY,
		CLABEL_RIGHT_JUSTIFY,
		CLABEL_CENTER_JUSTIFY
	};
	CLabel( CWindow* parent, int nFGColor, int nBGColor, const char* szText=0 );
	CLabel( CWindow* parent, int nFGColor, int nBGColor, int nResID );
	~CLabel( void );

	void SetText( const char* szText, bool fUpdate=true );
	void SetText( const CString& szText, bool fUpdate=true );
	void SetTextResID( int nResID, bool fUpdate=true );
	void SetJustify( int nJustify );
	void SetFocusedFGColor( int color ) { m_nFGFocusedColor = color; }
	CString& GetText( void ) { return m_szText; }
	int GetStringSize( void ) { return m_szText.GetSize(); }
	void SetFontOption( int option ) { m_nFontOption = option; }

	void EnableScroll( bool enable ) { m_bScrollable = enable; }
	void ScrollText( void );
	void SetScrollStep( int step );
};

#endif /* __LABEL_H__ */
