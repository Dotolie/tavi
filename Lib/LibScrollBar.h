//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CScrollBar Class Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibScrollBar.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibScrollBar.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.2  2005/10/12 09:24:06  etnlwind
// 1. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library 정리
//
//
//*****************************************************************************


 #ifndef __LIB_SCROLL_BAR_H__
 #define __LIB_SCROLL_BAR_H__

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

///////////////////////////////////////////////////////////////////////
//
// CScrollBar Class
// --------------------------

#define _SB_WHITE_HEAD_TRANSPARENCY_BG		0
#define _SB_WHITE_TAIL_TRANSPARENCY_BG		1
#define _SB_YELLOW_HEAD_TRANSPARENCY_BG		2
#define _SB_YELLOW_TAIL_TRANSPARENCY_BG		3
#define _SB_WHITE_HEAD_HALFTONE_BG			0
#define _SB_WHITE_TAIL_HALFTONE_BG			1
#define _SB_YELLOW_HEAD_HALFTONE_BG			2
#define _SB_YELLOW_TAIL_HALFTONE_BG			3

#define _SB_YELLOW								28
#define _SB_WHITE								17

#define _SB_WIDTH								8
#define _SB_EDGE_HEIGHT						4

class CScrollBar
{
	int iBodySX;
	int iBodySY;
	int iBodyHeight;
	int iCursorSY;
	int iCursorHeight;

public:
	CScrollBar() {}
	CScrollBar( int iInitBodySX, int iInitBodySY, int iInitBodyHeight, int iTotalItemCount, int iStartItemIndex, int iMaxVisibleItemCount );
	CScrollBar( int iInitBodySX, int iInitBodySY, int iInitBodyHeight, int iCursorSY, int iCursorHeight );

	int Draw();
	int ReDraw( int iTotalItemCount, int iStartItemIndex, int iMaxVisibleItemCount );
	int ReDraw( int iNewCursorSY, int iNewCursorHeight );
};

class CScrollBar2
{
	int iBodySX;
	int iBodySY;
	int iBodyHeight;
	int iCursorSY;
	int iCursorHeight;

public:
	CScrollBar2() {}
	CScrollBar2( int iInitBodySX, int iInitBodySY, int iInitBodyHeight, int iTotalItemCount, int iStartItemIndex, int iMaxVisibleItemCount );

	int Draw();
	int ReDraw( int iTotalItemCount, int iStartItemIndex, int iMaxVisibleItemCount );
	int ReDraw( int iNewCursorSY, int iNewCursorHeight );
};


class CScrollBar3
{
	int iBodySX;
	int iBodySY;
	int iBodyHeight;
	int iCursorSY;
	int iCursorHeight;

public:
	CScrollBar3() {}
	CScrollBar3( int iInitBodySX, int iInitBodySY, int iInitBodyHeight, RMuint32 TotalItemCount, RMuint32 StartItemIndex, RMuint32 MaxVisibleItemCount );

	int Draw();
	int ReDraw( RMuint32 TotalItemCount, RMuint32 StartItemIndex, RMuint32 MaxVisibleItemCount );
	int ReDraw( int iNewCursorSY, int iNewCursorHeight );
};







#endif


