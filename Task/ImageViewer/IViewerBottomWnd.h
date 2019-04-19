//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Bottom Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IViewerBottomWnd.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IViewerBottomWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.8  2005/12/01 01:03:57  zstein
// *** empty log message ***
//
//*******************************************************************************

#ifndef __IMAGE_VIEWER_BOTTOM_WINDOW_H__
#define __IMAGE_VIEWER_BOTTOM_WINDOW_H__

#include <sys/time.h>
#include <unistd.h>

#include <Component/BottomWnd.h>
#include <Component/ProgressBar.h>
#include <Component/Pixmap.h>
#include <Component/Label.h>

class CIViewerBottomWnd : public CBottomWnd
{
private:
protected:
	CPixmap* m_pmPlay;
	CPixmap* m_pmPause;
	CPixmap* m_pmRepeat;
	CPixmap* m_pmUnRepeat;
	CPixmap* m_pmRotate[4];
	CLabel* m_plbTotal;
	CLabel* m_plbName;
	CString m_szCurrentFile;
	int m_nCurrent;
	int m_nTotal;
	int m_nRotate;
	//struct timeval m_tvHide;
	void OnPaint( const CRect& rc );
	
public:
	CProgressBar* m_pProgressBar;
	CIViewerBottomWnd( CWindow* parent );
	~CIViewerBottomWnd( void );

	void Update( void );
	void Show( void );
	void UpdateRepeat( void );
	void UpdateRotate( int rotate=-1 );
};

#endif /* __IMAGE_VIEWER_BOTTOM_WINDOW_H__ */