//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IViewer.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IViewer.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.15  2006/01/24 00:54:20  zstein
// *** empty log message ***
//
// Revision 1.14  2006/01/19 05:26:43  zstein
// *** empty log message ***
//
// Revision 1.13  2005/12/01 01:03:57  zstein
// *** empty log message ***
//
// Revision 1.12  2005/11/18 09:16:12  zstein
// *** empty log message ***
//
//*******************************************************************************

#ifndef __IMAGE_VIEWER_WINDOW_CLASS_H__
#define __IMAGE_VIEWER_WINDOW_CLASS_H__

#include <sys/time.h>
#include <unistd.h>

#include <Component/window.h>
#include <Component/ModalWnd.h>
#include <Component/ContextMenu.h>
#include "IVListView.h"
#include "IViewerBottomWnd.h"

#define ON_IV_INFO				ON_MESSAGE( 0x200<<16 )
#define ON_IV_SETBACKGROUND	ON_MESSAGE( 0x300<<16 )

class CIViewer : public CWindow
{
private:
protected:
	int m_nPlayMode;
	int m_nImageWidth;
	int m_nImageHeight;
	int m_nRotate; // 0: 0, 1: 90, 2: 180, 3: 270
	int m_nPlayCount;
	int m_offset;
	bool m_fAutoHide;
	bool m_success;
	unsigned char* m_pRotatedY;
	unsigned char* m_pRotatedUV;
	struct timeval m_tvPanel;
	struct timeval m_tvTouch;
	struct timeval m_tvSlideshow;
	struct timeval m_tvMsg;
	void OnKeypressed( const KeyEvent& evt );
	void OnPaint( const CRect& rc );
	void OnMouseEvent( const MouseEvent& evt );
	void OnWindowEvent( const WindowEvent& evt );

	void Play( void );
	void Pause( void );
	bool ShowJPEG( uchar_t* path );
	bool ShowBMP( uchar_t* path );
	void ShowImage( void );
    bool DisplayYUV420( RMuint8* py, RMuint8* puv, int iw, int ih, int ow, int oh );

	void CreateContextMenu( void );
	void Show();
	
public:
	bool m_fUpdateImage;
	CContextMenu* m_pCtxMenuWnd;
	CString m_szPath;
	int m_nCurrent;
	CIViewerBottomWnd* m_pBottomWnd;
	int m_nCurrentMedia;
	CMsgBox* m_pMsg;
	CIVListView* m_plist;
	enum {
		PAUSE,
		PLAY
	};
	//CIViewer( CWindow* parent, const CString& szPath, CList< CListItem >* pList, int start );
	CIViewer( CWindow* parent, CIVListView* pview );
	~CIViewer( void );

	void SetPath( const CString& szPath ) { m_szPath = szPath; }
	int GetTotal( void );
	int GetCurrent( void ) { return m_nCurrent; }
	CString& GetCurrentFilename( void );
	CString& GetCurrentPath( void );
	int GetPlayStatus( void ) { return m_nPlayMode; }
	void PrevImage( void );
	void NextImage( void );
	void RotateCCW90( void );
	void RotateCW90( void );
	void ShowPanel();
	void HidePanel();
	//void AutoHide( bool hide );
};

#endif /* __IMAGE_VIEWER_WINDOW_CLASS_H__ */
