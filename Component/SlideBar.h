//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Slide Bar class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/SlideBar.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: SlideBar.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/12/02 05:09:54  zstein
// *** empty log message ***
//
// Revision 1.2  2005/04/27 05:37:41  ywkim
// Modify show & onPaint for m-language
//
// Revision 1.1  2005/03/29 03:33:57  ywkim
// Add slidebar class
//
//*****************************************************************************

#ifndef __SLIDE_BAR_CLASS_H__
#define __SLIDE_BAR_CLASS_H__

#include "window.h"
#include <Component/String.h>

class CSlideBar : public CWindow
{
private:
protected:
	CList< CString > m_uzList;
	CPixmap* m_puzSlideBar;
	CPixmap* m_puzSlider;
	int m_nMaxOfValue;
	int m_nMinOfValue;
	int m_nCurrentValue;
	bool	m_isPresetType;
	int	m_nIndexOfPreset;
	int	m_nCountOfPreset;
	int 	*m_azPreset;
	
	void OnFocus( bool fFocus );
	void OnKeypressed( const KeyEvent& key );
	void OnPaint( const CRect& rc );
public:
	CSlideBar( CWindow* parent );
	~CSlideBar( void );

	void Show( void );
	void Hide( void );
	void Resize( const CRect& rc, bool fUpdate );
	void Add( const CString& item );
	void AddPreset( int azPreset[], const int nCount );
	void SetPreset( int nIndex );
	int	GetPreset( void );
	void SetMinMax( int nMin, int nMax ) {
		m_nMinOfValue = nMin;
		m_nMaxOfValue = nMax;
	}
	void SetValue( int nData );
	int GetValue( void );
};

#endif /* __SLIDE_BAR_CLASS_H__ */
