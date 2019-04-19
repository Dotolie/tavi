//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/ImageViewer.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ImageViewer.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/12/29 07:40:24  ywkim
// Add function of IV_IsViewingOnForeground() for lcd off
//
// Revision 1.10  2005/11/25 10:22:17  zstein
// *** empty log message ***
//
// Revision 1.9  2005/10/22 05:47:01  zstein
// - deleted unused functions.
//
// Revision 1.8  2005/10/21 09:43:56  etnlwind
// Update for TextViwer Scroll, Booting Progress
//
// Revision 1.7  2005/08/03 03:41:11  zstein
// - updated
//
// Revision 1.6  2005/07/20 06:13:44  zstein
// - updated UI.
//
//******************************************************************************

#ifndef __IMAGE_VIEWER_H__
#define __IMAGE_VIEWER_H__

#include <tavi_lib.h>
#include <Component/object.h>
#include "ThumbDB.h"

extern CThumbDB* g_p_ThumbDB;

int 	IV_MsgProc( TMsgEntry* msg );
int 	ImageViewer_Task( TTaskInitParam* tParam );
extern CList< CWindow >* IV_GetWndList( void );
extern int IV_Kill( void );
bool IV_IsViewingOnForeground( void );

#endif

