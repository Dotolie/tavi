//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Image Viewer Simple List Viewer
// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVSListView.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: IVSListView.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/08/27 10:19:42  zstein
// - updated ui.
//
// Revision 1.4  2005/05/03 02:13:51  zstein
// *** empty log message ***
//
// Revision 1.3  2005/04/05 08:04:37  zstein
// *** empty log message ***
//
// Revision 1.2  2005/03/05 13:45:24  zstein
// *** empty log message ***
//
// Revision 1.1  2005/03/04 13:08:57  zstein
// New version Imager Viewer
//
//

//******************************************************************************

#ifndef __IMAGER_VIEWR_SIMPLE_LISTVIEW_H__
#define __IMAGER_VIEWR_SIMPLE_LISTVIEW_H__

#include "IVListView.h"

class CIVSListView : public CIVListView
{
private:
protected:
public:
	CIVSListView( CWindow* parent, const CString& szCurrentPath, int nSort, CStack<FolderPrev>* pStack );
	~CIVSListView( void );
};

#endif /* __IMAGER_VIEWR_SIMPLE_LISTVIEW_H__ */