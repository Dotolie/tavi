//************************ Copyright(C) New Media Life Inc. *************************
// Description : Image Viewer Info. Window Class
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVInfoWnd.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVInfoWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/08/03 03:41:11  zstein
// - updated
//
// Revision 1.4  2005/07/20 11:53:06  zstein
// - Updated UI
//
// Revision 1.3  2005/07/20 11:47:19  zstein
// - Updated UI.
//
// Revision 1.2  2005/07/20 06:13:44  zstein
// - updated UI.
//
// Revision 1.1  2005/04/27 13:29:37  zstein
// Image information window.
//
//
//******************************************************************************

#ifndef __IMAGEVIEWER_INFO_WINDOW_H__
#define __IMAGEVIEWER_INFO_WINDOW_H__ 

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include <Component/VScrollBar.h>
#include <Task/tavi_global.h>
#include "exiftags-1.00/jpeg.h"
#include "exiftags-1.00/exif.h"


class CIVInfoWnd : public CModalWnd
{
private:
protected:
	void OnMouseEvent( const MouseEvent& evt );
	void OnKeypressed( const KeyEvent& evt );
	void OnPaint( const CRect& rc );
	void ParseEXIF( const CString& path );
	void ParseBMP( const CString& path );
	void InsertProp( struct exifprop *list );

	typedef struct EXIFInfo {
		int m_nID;
		CString m_szValue;
	} EXIFInfo;

	CList< EXIFInfo > m_List;
	CRect m_rcText;
	CVScrollBar* m_pScrollBar;
	int m_nLine;
	int m_nMedia;

	void ScrollDown();
	void ScrollUp();
	void DrawEXIFTag();
	void DrawTagInfo();
public:
	CIVInfoWnd( CWindow* parent, const CString& szPath, int nMedia );
	~CIVInfoWnd( void );

	void Resize( const CRect& rc, bool fUpdate );
};

#endif /* __IMAGEVIEWER_INFO_WINDOW_H__ */
