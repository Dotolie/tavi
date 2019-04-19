//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Preview List Viewer
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVPreView.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVPreView.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.9  2006/01/24 00:53:52  zstein
// *** empty log message ***
//
// Revision 1.8  2005/12/02 13:40:47  zstein
// *** empty log message ***
//
// Revision 1.7  2005/11/25 10:22:17  zstein
// *** empty log message ***
//
// Revision 1.6  2005/11/08 01:18:53  zstein
// *** empty log message ***
//
//******************************************************************************

#ifndef __IMAGE_VIEWER_PREVIEW_LIST_H__
#define __IMAGE_VIEWER_PREVIEW_LIST_H__

#include "IVListView.h"
#include "ThumbDB.h"

class CIVPreViewList : public CIVListView
{
private:
protected:
	struct ImageNode {
		unsigned char* pY;
		unsigned char* pUV;
		bool success;
	};
	CList< struct ImageNode > m_uzImageList;
	void DrawItem( CListItem* puzItem, int idx, bool fSelect );

	unsigned char* m_Folder_Y;
	unsigned char* m_Folder_UV;
	unsigned char* m_JPG_Y;
	unsigned char* m_JPG_UV;
	unsigned char* m_BMP_Y;
	unsigned char* m_BMP_UV;
    unsigned char* m_pY;
    unsigned char* m_pUV;

	bool ScrollText( void );
	void BeforeScan( void );
public:
	CIVPreViewList( CWindow* parent, const CString& szCurrentPath, int nSort, CStack<FolderPrev>* pStack );
	~CIVPreViewList( void );
};

#endif /* __IMAGE_VIEWER_PREVIEW_LIST_H__ */
