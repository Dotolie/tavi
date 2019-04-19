//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Simple List Viewer
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVSListView.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVSListView.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2005/11/10 01:09:23  zstein
// *** empty log message ***
//
// Revision 1.6  2005/08/27 10:19:42  zstein
// - updated ui.
//
// Revision 1.5  2005/05/03 02:13:51  zstein
// *** empty log message ***
//
// Revision 1.4  2005/04/27 05:02:53  zstein
// Support M-langauges.
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
//*******************************************************************************

#include <sys/types.h>
#include <dirent.h>
#include <Component/FileUtil.h>
#include <Lib/LibImage.h>

#include "IVSListView.h"

// Macros

// Extern Variables

// Local Variables

// Local functions

// Class Member Methods
CIVSListView::CIVSListView( CWindow* parent, const CString& szCurrentPath, int nSort, CStack<FolderPrev>* pStack ) :
	CIVListView( parent, szCurrentPath, nSort, 8, pStack )
{
}

CIVSListView::~CIVSListView( void )
{
}


