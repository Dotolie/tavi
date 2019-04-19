//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CLibSubtitleDbx Class Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSubtitleSyncDbx.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSubtitleSyncDbx.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.3  2005/09/16 01:11:18  etnlwind
// Update for FileManager USB Host Related
//
// Revision 1.2  2005/07/16 06:15:26  etnlwind
// Update for LibSubtitleSyncDbx
//
// Revision 1.1  2005/07/15 09:47:39  etnlwind
// Add for LibuSubtitleSyncDbx
//
//
//*****************************************************************************


 #ifndef __LIB_SUBTITLE_DBX_H__
 #define __LIB_SUBTITLE_DBX_H__

#include <stdio.h>
#include <stdlib.h>
#include <rmbasic.h>
#include <linuxfs.h>
#include <jpeg/jpeg.h>

#include <tavi_lib.h>
#include <tavi_structures.h>
#include <bitmap.h>
#include <ImageFont.h>
#include <ImageResource.h>
#include <meplib.h>


#include "../Component/object.h"
#include "../Component/Stack.h"
#include "../Component/event.h"
#include "../Component/fbdev.h"
#include "../Component/image.h"
#include "../Component/timer.h"

#include <Lib/LibImage.h>

#include <string.h>
#include <Component/object.h>
#include <Component/image.h>

#include <Lib/LibImage.h>
#include <Lib/LibScrollBar.h>
#include <Lib/LibDialogBox.h>
#include <Lib/LibBookmarkDialogBox.h>
#include <Lib/LibSetupDialogBox.h>
#include <Lib/LibControlPannel.h>
#include <Lib/LibListBox.h>
#include <Lib/LibText.h>
#include <Lib/LibSubtitle.h>


///////////////////////////////////////////////////////////////////////
//
// CLibSubtitleSyncDbx Class
// --------------------------

class CLibSubtitleSyncDbx : public CDialogBox
{
	int gsx;
	int gsy;
	RMint32 cx;
	RMint32 cy;
	int isVisible;

	int m_SubtitleShiftSeconds;
	int m_Pre_SyncSecShiftValue;
	RMint64 PlaytimeMSec;

	RMuint8* pCursor;

	void DrawGrid();
	void DrawTextData();
	void RefreshTextData();
	void DrawCursor();
	void ClearTimeStringAndCursor();
	void ResyncSMI();
	
	void CursorLeft();
	void CursorRight();
	
public:

	CLibSubtitleSyncDbx( U32 DbxTitleRes, int x, int y, int w, int h, int SubtitleShiftSeconds, U32 option );
	virtual ~CLibSubtitleSyncDbx();

	void Show();
	void Hide();
	void Redraw();
	int IsVisible() { return isVisible; }
	int MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );	

};










#endif


