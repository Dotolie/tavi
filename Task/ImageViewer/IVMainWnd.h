//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Main Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVMainWnd.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVMainWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/10/29 00:47:05  zstein
// *** empty log message ***
//
// Revision 1.5  2005/08/03 03:41:11  zstein
// - updated
//
// Revision 1.4  2005/07/20 06:13:44  zstein
// - updated UI.
//
// Revision 1.3  2005/05/03 02:13:51  zstein
// *** empty log message ***
//
// Revision 1.2  2005/04/27 05:02:53  zstein
// Support M-langauges.
//
// Revision 1.1  2005/04/05 08:04:37  zstein
// *** empty log message ***
//
// Revision 1.2  2005/03/05 19:26:59  zstein
// added & fixed bugs.
//
// Revision 1.1  2005/03/04 13:08:57  zstein
// New version Imager Viewer
//
//
//*******************************************************************************

#ifndef __IMAGE_VIEWER_MAIN_WINDOW_H__
#define __IMAGE_VIEWER_MAIN_WINDOW_H__

#include <sys/time.h>
#include <unistd.h>

#define ON_VIEWTYPE		ON_MESSAGE( 0x400<<16 )

class CIVMainWnd: public CWindow
{
public:
	typedef enum {
		IV_LISTMODE_SIMPLE,
		IV_LISTMODE_PREVIEW,
		IV_LISTMODE_THUMBNAIL
	} IVViewMode;
private:
protected:
	IVViewMode m_nViewMode;
	struct timeval m_tv;
	
	void OnWindowEvent( const WindowEvent& evt );
	void OnViewType( int type );
public:

	CIVMainWnd( CWindow* parent, IVViewMode nViewMode );
	~CIVMainWnd( void );
};

#endif /* __IMAGE_VIEWER_MAIN_WINDOW_H__ */
