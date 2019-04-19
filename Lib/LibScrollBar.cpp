//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CScrollBar Class Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibScrollBar.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibScrollBar.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2006/02/06 04:22:37  akaiaphanu
// modify to scrollbar3 class
//
// Revision 1.4  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.3  2005/11/29 05:14:43  etnlwind
// Update for Scrollbar Bug Fix
//
// Revision 1.2  2005/10/12 09:24:06  etnlwind
// 1. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library 정리
//
//
//*****************************************************************************

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
#include <Lib/LibFileBrowser.h>

#include "LibDefines.h"

///////////////////////////////////////////////////////////////////////
//
// CScrollBar Class
// --------------------------


CScrollBar::CScrollBar( int iInitBodySX, int iInitBodySY, int iInitBodyHeight, int iTotalItemCount, int iStartItemIndex, int iMaxVisibleItemCount )
{
	iBodySX		= iInitBodySX;
	iBodySY		= iInitBodySY;
	iBodyHeight	= iInitBodyHeight;
	if ( iTotalItemCount == 0)
	{
		iTotalItemCount=1;
		iStartItemIndex =0;
		iMaxVisibleItemCount =1;
	}

	iCursorSY		= iBodySY + (int)( iBodyHeight * iStartItemIndex / iTotalItemCount );
 	iCursorHeight 	= (int)( iBodyHeight * iMaxVisibleItemCount /iTotalItemCount  );

	DEBUGMSG(1,("\nCScrollBar::CScrollBar()\n"));
	DEBUGMSG(1,(" - (@param)iInitBodySX : %d\n", iInitBodySX ) );
	DEBUGMSG(1,(" - (@param)iInitBodySY : %d\n", iInitBodySY ) );
	DEBUGMSG(1,(" - (@param)iInitBodyHeight : %d\n", iInitBodyHeight ) );
	DEBUGMSG(1,(" - (@param)iTotalItemCount : %d\n", iTotalItemCount ) );
	DEBUGMSG(1,(" - (@param)iStartItemIndex : %d\n", iStartItemIndex ) );
	DEBUGMSG(1,(" - (@param)iMaxVisibleItemCount : %d\n", iMaxVisibleItemCount ) );
	DEBUGMSG(1,(" - iCursorSY : %d\n", iCursorSY ) );
	DEBUGMSG(1,(" - iCursorHeight : %d\n", iCursorHeight ) );	
	
	Draw();
}


int CScrollBar::Draw()
{
	DEBUGMSG(1,("CScrollBar::Draw()\n"));
	DEBUGMSG(1,(" - iBodySX : %d\n", iBodySX));
	DEBUGMSG(1,(" - iBodySY : %d\n", iBodySY));
	DEBUGMSG(1,(" - iBodyHeight : %d\n", iBodyHeight));	
	DEBUGMSG(1,(" - iCursorSY : %d\n", iCursorSY));
	DEBUGMSG(1,(" - iCursorHeight : %d\n", iCursorHeight));	

#if 0
	DrawBoxWithoutRefresh( _SB_YELLOW, iBodySX+1, iBodySY, _SB_WIDTH-2, iBodyHeight );
	DrawItemWithoutRefresh( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_YELLOW_HEAD_HALFTONE_BG, iBodySX, iBodySY, _SB_WIDTH, _SB_EDGE_HEIGHT );
	DrawItemWithoutRefresh( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_YELLOW_TAIL_HALFTONE_BG, iBodySX, iBodySY+iBodyHeight, _SB_WIDTH, _SB_EDGE_HEIGHT );	

	DrawBoxWithoutRefresh( _SB_WHITE, iBodySX+1, iCursorSY+4, _SB_WIDTH-2, iCursorHeight-4 );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_WHITE_HEAD_TRANSPARENCY_BG, iBodySX, iCursorSY, _SB_WIDTH, _SB_EDGE_HEIGHT );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_WHITE_TAIL_TRANSPARENCY_BG, iBodySX, iCursorSY+iCursorHeight, _SB_WIDTH, _SB_EDGE_HEIGHT );	
#endif
	DrawBoxWithoutRefresh( _SB_YELLOW, iBodySX, iBodySY, _SB_WIDTH, iBodyHeight );
	DrawItemWithoutRefresh( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_YELLOW_HEAD_HALFTONE_BG, iBodySX, iBodySY, _SB_WIDTH, _SB_EDGE_HEIGHT );
	DrawItemWithoutRefresh( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_YELLOW_TAIL_HALFTONE_BG, iBodySX, iBodySY+iBodyHeight, _SB_WIDTH, _SB_EDGE_HEIGHT );	

	DrawBoxWithoutRefresh( _SB_WHITE, iBodySX, iCursorSY+4, _SB_WIDTH, iCursorHeight-4 );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_WHITE_HEAD_TRANSPARENCY_BG, iBodySX, iCursorSY, _SB_WIDTH, _SB_EDGE_HEIGHT );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_WHITE_TAIL_TRANSPARENCY_BG, iBodySX, iCursorSY+iCursorHeight, _SB_WIDTH, _SB_EDGE_HEIGHT );	


	return TRUE;
}

int CScrollBar::ReDraw( int iTotalItemCount, int iStartItemIndex, int iMaxVisibleItemCount )
{
	if ( iTotalItemCount == 0)
	{
		iTotalItemCount=1;
		iStartItemIndex =0;
		iMaxVisibleItemCount =1;
	}

	iCursorSY		= iBodySY + (int)( iBodyHeight * iStartItemIndex / iTotalItemCount );
 	iCursorHeight 	= (int)( iBodyHeight * iMaxVisibleItemCount /iTotalItemCount  );

	DEBUGMSG(1,("CScrollBar::ReDraw( int iTotalItemCount(%d) int iStartItemIndex(%d), int iMaxVisibleItemCount(%d) ) \n",
		iTotalItemCount, iStartItemIndex, iMaxVisibleItemCount ) );

	DEBUGMSG(1,(" - iCursorSY(%d) = iBodySY(%d) + (int)( iBodyHeight(%d) * ( iStartItemIndex(%d) / iTotalItemCount(%d) ))\n", 
		iCursorSY, iBodySY, iBodyHeight, iStartItemIndex, iTotalItemCount ));

	DEBUGMSG(1,(" - iCursorHeight(%d) = iBodyHeight(%d) + (int)( iInitBodyHeight(%d) * ( iMaxVisibleItemCount(%d) / iTotalItemCount(%d) )", 
		iCursorHeight, iBodyHeight, iMaxVisibleItemCount, iTotalItemCount));

	return ( Draw() );

}




///////////////////////////////////////////////////////////////////////
//
// CScrollBar2 Class
// --------------------------
CScrollBar2::CScrollBar2( int iInitBodySX, int iInitBodySY, int iInitBodyHeight, int iTotalItemCount, int iStartItemIndex, int iMaxVisibleItemCount )
{
	iBodySX		= iInitBodySX;
	iBodySY		= iInitBodySY;
	iBodyHeight	= iInitBodyHeight;
	iCursorSY		= iBodySY + (int)( iBodyHeight * iStartItemIndex / iTotalItemCount );
 	iCursorHeight 	= (int)( iBodyHeight * iMaxVisibleItemCount /iTotalItemCount  );

	DEBUGMSG(1,("\nCScrollBar::CScrollBar()\n"));
	DEBUGMSG(1,(" - (@param)iInitBodySX : %d\n", iInitBodySX ) );
	DEBUGMSG(1,(" - (@param)iInitBodySY : %d\n", iInitBodySY ) );
	DEBUGMSG(1,(" - (@param)iInitBodyHeight : %d\n", iInitBodyHeight ) );
	DEBUGMSG(1,(" - (@param)iTotalItemCount : %d\n", iTotalItemCount ) );
	DEBUGMSG(1,(" - (@param)iStartItemIndex : %d\n", iStartItemIndex ) );
	DEBUGMSG(1,(" - (@param)iMaxVisibleItemCount : %d\n", iMaxVisibleItemCount ) );
	DEBUGMSG(1,(" - iCursorSY : %d\n", iCursorSY ) );
	DEBUGMSG(1,(" - iCursorHeight : %d\n", iCursorHeight ) );	
	
}


int CScrollBar2::Draw()
{
	DEBUGMSG(1,("CScrollBar::Draw()\n"));
	DEBUGMSG(1,(" - iBodySX : %d\n", iBodySX));
	DEBUGMSG(1,(" - iBodySY : %d\n", iBodySY));
	DEBUGMSG(1,(" - iBodyHeight : %d\n", iBodyHeight));	
	DEBUGMSG(1,(" - iCursorSY : %d\n", iCursorSY));
	DEBUGMSG(1,(" - iCursorHeight : %d\n", iCursorHeight));	

	DrawBoxWithoutRefresh( _SB_YELLOW, iBodySX, iBodySY, _SB_WIDTH, iBodyHeight );
	DrawItemWithoutRefresh( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_YELLOW_HEAD_HALFTONE_BG, iBodySX, iBodySY, _SB_WIDTH, _SB_EDGE_HEIGHT );
	DrawItemWithoutRefresh( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_YELLOW_TAIL_HALFTONE_BG, iBodySX, iBodySY+iBodyHeight, _SB_WIDTH, _SB_EDGE_HEIGHT );	

	DrawBoxWithoutRefresh( _SB_WHITE, iBodySX, iCursorSY+4, _SB_WIDTH, iCursorHeight-4 );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_WHITE_HEAD_TRANSPARENCY_BG, iBodySX, iCursorSY, _SB_WIDTH, _SB_EDGE_HEIGHT );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_WHITE_TAIL_TRANSPARENCY_BG, iBodySX, iCursorSY+iCursorHeight, _SB_WIDTH, _SB_EDGE_HEIGHT );	


	return TRUE;
}

/*
CScrollBar::ReDraw( int iTotalItemCount(512) int iStartItemIndex(0), int iMaxVisibleItemCount(8) ) 
 - iCursorSY(34) = iBodySY(34) + (int)( iBodyHeight(176) * ( iStartItemIndex(0) / iTotalItemCount(512) ))
CScrollBar::ReDraw( int iTotalItemCount(512) int iStartItemIndex(0), int iMaxVisibleItemCount(8) ) 
 - iCursorSY(34) = iBodySY(34) + (int)( iBodyHeight(176) * ( iStartItemIndex(0) / iTotalItemCount(512) ))
 - iCursorHeight(2) = iBodyHeight(176) + (int)( iInitBodyHeight(8) * ( iMaxVisibleItemCount(512) / iTotalItemCount(512) )FM_MsgProc() : ssTaskId[0x8], ulMsgId [0x2001], ssParam1[0x1001], ssParam2[0]

*/

int CScrollBar2::ReDraw( int iTotalItemCount, int iStartItemIndex, int iMaxVisibleItemCount )
{
	iCursorSY		= iBodySY + (int)( iBodyHeight * iStartItemIndex / iTotalItemCount );
 	iCursorHeight 		= (int)( iBodyHeight * iMaxVisibleItemCount /iTotalItemCount  );
	if(iCursorHeight<5) 
		{
		iCursorHeight = 5;
		iCursorSY	= iBodySY + (int)( iBodyHeight * iStartItemIndex / iTotalItemCount ) -3;
		}
		

	DEBUGMSG(1,("CScrollBar::ReDraw( int iTotalItemCount(%d) int iStartItemIndex(%d), int iMaxVisibleItemCount(%d) ) \n",
		iTotalItemCount, iStartItemIndex, iMaxVisibleItemCount ) );

	DEBUGMSG(1,(" - iCursorSY(%d) = iBodySY(%d) + (int)( iBodyHeight(%d) * ( iStartItemIndex(%d) / iTotalItemCount(%d) ))\n", 
		iCursorSY, iBodySY, iBodyHeight, iStartItemIndex, iTotalItemCount ));

	DEBUGMSG(1,(" - iCursorHeight(%d) = iBodyHeight(%d) + (int)( iInitBodyHeight(%d) * ( iMaxVisibleItemCount(%d) / iTotalItemCount(%d) )", 
		iCursorHeight, iBodyHeight, iMaxVisibleItemCount, iTotalItemCount));

	return ( Draw() );
	//return TRUE;
}

///////////////////////////////////////////////////////////////////////
//
// CScrollBar3 Class
// --------------------------
CScrollBar3::CScrollBar3( int iInitBodySX, int iInitBodySY, int iInitBodyHeight, RMuint32 TotalItemCount, RMuint32 StartItemIndex, RMuint32 MaxVisibleItemCount )
{
	DEBUGMSG(1,("CScrollBar3()\n"));
	iBodySX		= iInitBodySX;
	iBodySY		= iInitBodySY;
	iBodyHeight	= iInitBodyHeight;

/*
 	iCursorHeight 	= (int)( iBodyHeight * MaxVisibleItemCount /TotalItemCount );
	DEBUGMSG(1,("-iCursorHeight: %d\n", iCursorHeight ));

	iCursorSY	= iBodySY + (int)( iBodyHeight * StartItemIndex / TotalItemCount );
	DEBUGMSG(1,("-iCursorSY: %d\n", iCursorHeight ));
*/	

}


int CScrollBar3::Draw()
{
	DrawBoxWithoutRefresh( _SB_YELLOW, iBodySX, iBodySY, _SB_WIDTH, iBodyHeight );
	DrawItemWithoutRefresh( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_YELLOW_HEAD_HALFTONE_BG, iBodySX, iBodySY, _SB_WIDTH, _SB_EDGE_HEIGHT );
	DrawItemWithoutRefresh( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_YELLOW_TAIL_HALFTONE_BG, iBodySX, iBodySY+iBodyHeight, _SB_WIDTH, _SB_EDGE_HEIGHT );	

	DrawBoxWithoutRefresh( _SB_WHITE, iBodySX, iCursorSY+4, _SB_WIDTH, iCursorHeight-4 );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_WHITE_HEAD_TRANSPARENCY_BG, iBodySX, iCursorSY, _SB_WIDTH, _SB_EDGE_HEIGHT );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ SCROLLBAR_EDGE ], _SB_WHITE_TAIL_TRANSPARENCY_BG, iBodySX, iCursorSY+iCursorHeight, _SB_WIDTH, _SB_EDGE_HEIGHT );	

	return TRUE;
}

int CScrollBar3::ReDraw( RMuint32 TotalItemCount, RMuint32 StartItemIndex, RMuint32 MaxVisibleItemCount )
{

	int iScreenPos = StartItemIndex % TotalItemCount;
	
	//iCursorSY	= iBodySY + (int)( (iBodyHeight * StartItemIndex) / TotalItemCount );
 	iCursorHeight 	= (int)( iBodyHeight * MaxVisibleItemCount /TotalItemCount  );
	if (StartItemIndex == TotalItemCount - 1) {
		iCursorSY	= iBodySY+(int)(iBodyHeight - iCursorHeight);
	} else {
		iCursorSY	= iBodySY+(int)((iBodyHeight - iCursorHeight) * StartItemIndex/TotalItemCount);
	}

	if( iCursorHeight < 5 ) 
		iCursorHeight = 5; 
	else if( iCursorHeight >= iBodyHeight ) 
		return FALSE;

//	fprintf(stderr,"CScrollBar::ReDraw( int TotalItemCount(%d) int StartItemIndex(%d), int iMaxVisibleItemCount(%d) ) \n",TotalItemCount, StartItemIndex, MaxVisibleItemCount  );
//	fprintf(stderr," - iCursorSY(%d) = iBodySY(%d) + (RMuint32)( iBodyHeight(%d) * ( StartItemIndex(%d) / TotalItemCount(%d) ))\n", iCursorSY, iBodySY, iBodyHeight, StartItemIndex, TotalItemCount );
//	fprintf(stderr," - iCursorHeight(%d) = iBodyHeight(%d) + (int)( iInitBodyHeight(%d) * ( iMaxVisibleItemCount(%d) / TotalItemCount(%d) )\n", iCursorHeight, iBodyHeight, MaxVisibleItemCount, TotalItemCount);

	return ( Draw() );

}



