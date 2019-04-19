//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Tileviewer 
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVTileViewer.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVTileViewer.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/12/02 13:40:47  zstein
// *** empty log message ***
//
// Revision 1.5  2005/12/01 01:03:57  zstein
// *** empty log message ***
//
// Revision 1.4  2005/11/25 10:22:17  zstein
// *** empty log message ***
//
// Revision 1.3  2005/11/22 09:06:44  zstein
// *** empty log message ***
//
// Revision 1.2  2005/11/08 07:25:54  zstein
// *** empty log message ***
//
// Revision 1.1  2005/11/08 01:19:24  zstein
// *** empty log message ***
//
//******************************************************************************

#ifndef __TILE_VIEWER_H__
#define __TILE_VIEWER_H__

#include "IVListView.h"
#include "ThumbDB.h"

class CIVTileViewer : public CIVListView
{
private:
protected:
	void DrawItem( CListItem* puzItem, int idx, bool fSelect );

	unsigned char* m_Folder_Y;
	unsigned char* m_Folder_UV;
	unsigned char* m_JPG_Y;
	unsigned char* m_JPG_UV;
	unsigned char* m_BMP_Y;
	unsigned char* m_BMP_UV;
	CPixmap* m_EmptyFrame;

	//CThumbDB m_ThumbDB;
	char* m_pBMPBuf;
	bool m_norestore;

	void BeforeScan( void );
	void OnKeypressed( const KeyEvent& evt );
	void OnMouseEvent( const MouseEvent& evt );
	void OnPaint( const CRect& rc );
	void DrawBackground( void );
	void DrawGuide();
	
	bool MoveUP( int prev );
	bool MoveDown( int prev );
public:
	CIVTileViewer( CWindow* parent, const CString& szCurrentPath, int nSort, CStack<FolderPrev>* pStack );
	~CIVTileViewer( void );

	bool ScrollText( void ) {}
	void ResetScrollBar( void );
};

#endif /* __TILE_VIEWER_H__ */

