//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CBookmarDialogBox Class Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibBookmarkDialogBox.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibBookmarkDialogBox.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.28  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.27  2005/12/14 09:56:17  etnlwind
// Update for Shutdown on VideoPlayer
// - Brightness & Contrast Range를 0~60으로 수정
// - Brightness 대화상자의 크기를 줄여 하단으로 옮김
// - Brightness 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Brightness 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Bookmark 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Bookmark 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Shutdown으로 종료시 화면의 LCD를 Off, 이후 Shutdown 이미지 출력전에 On 처
//
// Revision 1.26  2005/12/07 10:12:28  etnlwind
// Update for TextViewer TASK_KEY
//
// Revision 1.25  2005/12/07 03:03:25  etnlwind
// Update for Emergency Exit On Bookmark DialogBox
//
// Revision 1.24  2005/10/14 11:39:38  etnlwind
// Update for TextViwer
//
// Revision 1.23  2005/10/12 09:24:05  etnlwind
// 1. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.22  2005/09/21 05:51:10  etnlwind
// Update for Video Bookmark Delete And Controlbar BugFix
//
// Revision 1.21  2005/09/16 02:51:31  etnlwind
// Update for Bookmark Dbx Touchpad Input Filtering Case By Case
//
// Revision 1.20  2005/08/29 03:24:41  etnlwind
// Update for Bookmark Dbx Bug Fix
//
// Revision 1.19  2005/08/29 03:02:46  etnlwind
// Update for Bookmark Dbx BugFix
//
// Revision 1.18  2005/08/29 01:26:00  etnlwind
// Update for Video Player
//
// Revision 1.17  2005/08/16 05:28:01  etnlwind
// Update for VideoPlayer
//
// Revision 1.16  2005/08/14 09:38:33  etnlwind
// Update for
// 1. VideoPlayer Bookmark Delete
// 2. VideoPlayer FileBrowser Subtitle Ready Video File Icon Specificationized
// 3. VideoPlayer UI minor updates
//
// Revision 1.15  2005/07/11 13:02:13  etnlwind
// Update for CVS
//
// Revision 1.14  2005/05/18 17:54:47  etnlwind
// Update for Video Player Timer Related Fix
//
// Revision 1.13  2005/05/15 16:27:52  etnlwind
// Update for Battety Msg Skip on VideoPlayer
//
// Revision 1.12  2005/05/11 08:03:46  etnlwind
// Update for MPEG4 and Divx3 low profile Codec Related
//
// Revision 1.11  2005/05/07 03:40:38  etnlwind
// Update for ES4
//
// Revision 1.10  2005/04/26 10:06:57  etnlwind
// Upate for ES4 UI
//
// Revision 1.9  2005/04/22 01:58:39  etnlwind
// MultiLanguage Support
//
// Revision 1.8  2005/04/20 11:28:46  etnlwind
// update for ES4
//
// Revision 1.7  2005/03/05 13:38:16  etnlwind
// Timer
//
// Revision 1.6  2005/03/05 13:00:45  etnlwind
// Timer관련 App수정
//
// Revision 1.5  2005/03/04 20:00:20  etnlwind
// error fix
//
// Revision 1.4  2005/03/04 19:38:28  etnlwind
// VP repeat option
//
// Revision 1.2  2005/03/03 18:25:36  etnlwind
// Library 수정
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

extern CTextEngine_BDF* pT;

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// CBookmarkDialogBox Class Figure
//
//

CBookmarkDialogBox::CBookmarkDialogBox( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option  )
{
	DEBUGMSG(1,("CBookmarkDialogBox( [%s], %d, %d, %d, %d, ... , %d)\n", sDbxTitle, x, y, w, h, option ));

	Stop_VideoControlPannelTimer();

	memset( title, 0, 256 );
	//memset( item, 0, 8*256 );
	LastSlot 	= 0;



	// Indexes
	SelectedAbsoluteIndex	= 0;	// Cursor
	AbsoluteIndex			= 0;	// 0 ~ LASTINDEX
	FirstShownIndex		= 0;	// Absolute Index			
	LastIndex			= 0;	// Absolute Index
	pSb1				= NULL;


//	FirstShownSlot		= 0;
//	slot			= 0;

	sx = x;
	sy = y;
	width = w;
	height = h;

	fpDialogBoxMsgProc =  pfpMsgProc;

	if( (option&_BOOKMARK_DBX_AT_PLAYING) == _BOOKMARK_DBX_AT_PLAYING )
		{
		mode = _BOOKMARK_DBX_AT_PLAYING;
		choice 		= 0;
		}
	else if( (option&_BOOKMARK_DBX_AT_BEGINNIG) == _BOOKMARK_DBX_AT_BEGINNIG )
		{
		mode = _BOOKMARK_DBX_AT_BEGINNIG;
		choice 		= -2;
		ulBookmarkingTime =  option;
		DEBUGMSG(1,(" - ulBookmarkingTime = %d\n", ulBookmarkingTime ));
		Convert_SecOnly2TimeString( BookmarkingTimeString, (int)ulBookmarkingTime);
		DEBUGMSG(1,(" - BookmarkingTimeString = [%s]\n", BookmarkingTimeString) );
		}


	BtnOption = option;
	CreateDbxBackground();
	SetTitle( sDbxTitle );
}


CBookmarkDialogBox::CBookmarkDialogBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 ulPlaytime, U32 option  )
{
	DEBUGMSG(1,("CBookmarkDialogBox( [%d, %d, %d, %d, %d, ... , %d)\n", ResIndex, x, y, w, h, option ));

	Init( ResIndex, x, y, w, h, pfpMsgProc, option );
	
	if( (option&(U32)_BOOKMARK_DBX_AT_PLAYING) == (U32)_BOOKMARK_DBX_AT_PLAYING )
		{
		mode 				= _BOOKMARK_DBX_AT_PLAYING;
		choice 				= 0;
		}
	else
		{
		mode 				= _BOOKMARK_DBX_AT_BEGINNIG;
		choice 				= -2;
		ulBookmarkingTime 	=  ulPlaytime;
		DEBUGMSG(1,(" - ulBookmarkingTime = %d\n", ulBookmarkingTime ));
		Convert_SecOnly2TimeString( BookmarkingTimeString, (int)ulBookmarkingTime);
		DEBUGMSG(1,(" - BookmarkingTimeString = [%s]\n", BookmarkingTimeString) );
		}
}
CBookmarkDialogBox::CBookmarkDialogBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 AbsoluteCursor, U32 LineCounter, U32 option  )
{

	Init( ResIndex, x, y, w, h, pfpMsgProc, option );

	if( (option&(U32)_BOOKMARK_DBX_AT_PLAYING) == (U32)_BOOKMARK_DBX_AT_PLAYING )
		{
		mode 				= _BOOKMARK_DBX_AT_PLAYING;
		choice 				= 0;
		}
	else
		{
		mode 				= _BOOKMARK_DBX_AT_BEGINNIG;
		choice 				= -2;
		ulBookmarkingTime 	= AbsoluteCursor;
		LineCounter			= LineCounter;
		DEBUGMSG(1,(" - ulBookmarkingTime = %d\n", ulBookmarkingTime ));
		Convert_SecOnly2TimeString( BookmarkingTimeString, (int)ulBookmarkingTime);
		DEBUGMSG(1,(" - BookmarkingTimeString = [%s]\n", BookmarkingTimeString) );
		}

}

CBookmarkDialogBox::~CBookmarkDialogBox()
{
	if( pSb1 != NULL )
		delete pSb1;
}


void CBookmarkDialogBox::Init( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option )
{
	Stop_VideoControlPannelTimer();
	memset( title, 0, 256 );
	LastSlot 	= 0;

	// Indexes
	SelectedAbsoluteIndex	= 0;	// Cursor
	AbsoluteIndex			= 0;	// 0 ~ LASTINDEX
	FirstShownIndex		= 0;	// Absolute Index			
	LastIndex			= 0;	// Absolute Index
	pSb1				= NULL;

	sx = x;
	sy = y;
	width = w;
	height = h;

	fpDialogBoxMsgProc =  pfpMsgProc;

	BtnOption = option;
	CreateDbxBackground();
	SetTitle( ResIndex );
	
}


int CBookmarkDialogBox::LoadingBookmarkData()
{
	return TRUE;
}


void CBookmarkDialogBox::CreateSelectBar()
{
	//int sby = sy+ _DBX_SELECTBAR_START_Y + ( (choice-1)*_DBX_SELECTBAR_HEIGHT );


/*
	if( mode == _BOOKMARK_SET_DBX )
		//RelativeIndex = choice + 1;

	else
		RelativeIndex = choice ;
*/

//	if( BookmarkList.Count() == 0-mode )
	if( BookmarkList.Count() == 0 &&  mode == _BOOKMARK_DBX_AT_PLAYING )
		{
			char NoBookmarkExist[20];
			sprintf( NoBookmarkExist, "%s", "No Bookmark Exists" );
			pT->DrawEUC8Text( NoBookmarkExist, (U16)(sx + _DBX_ITEM_COL_SPX), (U16)(sy+ _DBX_SELECTBAR_START_Y + 3), (U16)_WHITE, (U16)12, (U32)DT_TRANSPARENCY );
		
		}

	else
		{
		RelativeIndex = SelectedAbsoluteIndex - FirstShownIndex;

		if( RelativeIndex < 0)
			RelativeIndex = 0;
		else if( RelativeIndex >= _DBX_MAX_DISPLAY_INDEX )
			RelativeIndex  = _DBX_MAX_DISPLAY_INDEX-1;
	 

		int sby = sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex * _DBX_SELECTBAR_HEIGHT );

		DrawBoxWithoutRefresh( 
			_DBX_SELECTED_ITEM_BAR_COLOR, 
			sx+_DBX_SELECTBAR_START_X, 
			sby, 
			width-14-SCROLLBAR_SPACE,
			_DBX_ROW_HEIGHT-1
			);

		DrawBoxWithoutRefresh( 
			_DBX_SELECTED_ITEM_BAR_SHADOW_COLOR, 
			sx+_DBX_SELECTBAR_START_X, 
			sby+_DBX_ROW_HEIGHT-1, 
			width-14-SCROLLBAR_SPACE,
			1
			);

		DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE], 0, sx+_DBX_TITLE_RELATIVE_X, sby, 4,22 );
		DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE], 1, sx+_DBX_TITLE_RELATIVE_X+width-(_DBX_TITLE_RELATIVE_X*2)-SCROLLBAR_SPACE, sby, 4,22 );
		}
}



/*

index       

FirstShownSlot ...
choice ... selected slot


index bookmark_cursor







 T    index       FirstShownSlot        choice      FirstShownIndex        slot       LastIndex LastSlot
|-+-----+---------+------+--------------+----+-------+--------|
|S|          0|                  0|           2|                               1|       1|              4|                3|
|N|    0|        0|    2|            1|   1|     4|       3|
|N|    1|        0|    2|            1|   1|     4|       3|
|N|    2|        0|    2|            1|   1|     4|       3|
|N|    3|        0|    2|            1|   1|     4|       3|
|B|    4|        0|    2|            1|   1|     4|       3|
*/


#if 0
// Version ES3
void CBookmarkDialogBox::CreateTextData()
{
	int description, time;
	RMuint16 BookmarkPID, AddBookmarkPID;
	U16	color;
	U32 option;

	char timestring[10];
	uchar_t ucTimeString[10];

	uchar_t ucTemp[64];


	DEBUGMSG(1,(" - BookmarkList.Count()  : %d\n", BookmarkList.Count() ));
	DEBUGMSG(1,(" - LastSlot : %d\n", LastSlot));
	

	if( mode == _BOOKMARK_SET_DBX )
		{
		LastIndex = BookmarkList.Count();
		RelativeIndex = 0;
		}
	else
		{
		LastIndex = BookmarkList.Count()-1;
		RelativeIndex = 0;
		}

	

	DEBUGMSG(1,(" T RelativeIndex AbsoluteIndex FirstShownIndex SelectedAbsoluteIndex LastIndex choice\n"));
	DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", 'S', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));
	DEBUGMSG(1,("+-+-------------+-------------+---------------+---------------------+---------+------+\n"));

	for( ; RelativeIndex <= MIN(_DBX_MAX_DISPLAY_INDEX-1, LastIndex); RelativeIndex++)
		{

		AbsoluteIndex = FirstShownIndex + RelativeIndex;

 		if( AbsoluteIndex == SelectedAbsoluteIndex )
			{
			AddBookmarkPID	= 43;
			BookmarkPID		= 45;

			color			= _BLACK;
			description 		= GULIM_13_BLACK_BOLD;
			time				= GULIM_13_BLACK_NORMAL;
			//option			= (DT_TRANSPARENCY|DT_BOLD);
			option			= (DT_TRANSPARENCY);
			}
		else
			{
			AddBookmarkPID	= 42;
			BookmarkPID		= 44;

			color			= _WHITE;
			description 		= GULIM_13_WHITE_BOLD;
			time				= GULIM_13_WHITE_NORMAL_BORDER;
//			option			= (DT_TRANSPARENCY|DT_OUTLINE);
			option			= (DT_TRANSPARENCY);
			}		

	//	printf("(((((((((((((((((((((mode = %x)))))))))))))))))))))))\n", mode );
		
		if( mode == _BOOKMARK_SET_DBX && (AbsoluteIndex == 0) )		// 
			{

			DrawItemWithoutRefreshSkipTransparency(
				&TAVI_IMAGES[TYPE_ICON_LIST],
				AddBookmarkPID,
				sx + _DBX_ITEM_COL_SPX, 
				sy+ _DBX_SELECTBAR_START_Y + 4,
				12, 12
				);

			pT->DrawResText( 
				ML_BOOKMARK_SAVE, 
				sx + _DBX_ITEM_COL_SPX + 14,
				sy+ _DBX_SELECTBAR_START_Y + 5,
				color,
				12,
				option
				);
				
/*	
			DrawASCIITextWithoutRefresh(
				"Set Bookmark",
				16, 
				15, 
				&TAVI_IMAGES[description],
				sx + _DBX_ITEM_COL_SPX,
				sy+ _DBX_SELECTBAR_START_Y + 3,
				__SKIP_TRANSPARENCY_COLOR
				);
		
			DrawASCIITextWithoutRefresh(
				BookmarkingTimeString,
				16, 
				15, 
				&TAVI_IMAGES[time],
				sx + _DBX_ITEM_COL_SPX+100,
				sy+ _DBX_SELECTBAR_START_Y  + 3,
				__SKIP_TRANSPARENCY_COLOR
				);
*/
			DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", 'S', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));


			}
		else
			{

			//Text Description
			DrawItemWithoutRefreshSkipTransparency(
				&TAVI_IMAGES[TYPE_ICON_LIST],
				BookmarkPID,
				sx + _DBX_ITEM_COL_SPX, 
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 4,
				12, 12
				);

			ustrcpy( ucTemp, pT->GetTextRes(ML_BOOKMARK_PLAY) );
			sprintf( timestring, "%d", AbsoluteIndex );
			str2ustr( ucTimeString, timestring, strlen(timestring) );
			ustrcat ( ucTemp, ucTimeString );

//			pT->DrawResText( 
//				ML_BOOKMARK_PLAY, 
			pT->DrawUC16Text(
				ucTemp,
				sx + _DBX_ITEM_COL_SPX + 14,
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5,
				color,
				12,
				option
				);



			if( (mode&_BOOKMARK_SET_DBX) == _BOOKMARK_SET_DBX )
				Convert_SecOnly2TimeString( timestring, BookmarkList[ AbsoluteIndex -1]->ulSavetime);
			else
				Convert_SecOnly2TimeString( timestring, BookmarkList[ AbsoluteIndex ]->ulSavetime);


			str2ustr( ucTimeString, timestring, strlen(timestring) );

			

			pT->DrawUC16Text( 
				ucTimeString, 
				(280 - pT->GetStringWidth( ucTimeString, 12, 0 )),
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 4,
				color,
				12,
				option
				);			

/*
			DrawASCIITextWithoutRefresh(
				"Play Bookmark",
				16, 
				15, 
				&TAVI_IMAGES[description],
				sx + _DBX_ITEM_COL_SPX,
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 3,
				__SKIP_TRANSPARENCY_COLOR
				);


			
			//Save Time Text Description
			DrawASCIITextWithoutRefresh(
				timestring,
				16, 
				15, 
				&TAVI_IMAGES[time],
				sx + _DBX_ITEM_COL_SPX+100,
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 3,
				__SKIP_TRANSPARENCY_COLOR
				);
*/

			if( description == GULIM_13_BLACK_BOLD )
				DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", '>', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));
			else
				DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", ' ', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));


			}
		
		
		}

	DEBUGMSG(1,("+-+-------------+-------------+---------------+---------------------+---------+------+\n"));


}
#endif

void CBookmarkDialogBox::CreateTextData()
{
	int description, time;
	RMuint16 BookmarkPID, AddBookmarkPID;
	U16	color;
	U32 option;

	char timestring[10];
	uchar_t ucTimeString[10];

	uchar_t ucTemp[64];


	DEBUGMSG(1,(" - BookmarkList.Count()  : %d\n", BookmarkList.Count() ));
	DEBUGMSG(1,(" - LastSlot : %d\n", LastSlot));
	

	if( mode == _BOOKMARK_DBX_AT_BEGINNIG )
		{
		// Last Position, 
		// From Begin .. 
		// Bookmark 0 
		// Bookmark 1
		// Bookmark 2 

		LastIndex = BookmarkList.Count()+1;				
		RelativeIndex = 0;

		}
	else
		{

		// Bookmark 0 
		// Bookmark 1
		// Bookmark 2
		// Bookmark 3
		// Bookmark 4

		LastIndex = BookmarkList.Count()-1;
		RelativeIndex = 0;

		}

	DEBUGMSG(1,(" T RelativeIndex AbsoluteIndex FirstShownIndex SelectedAbsoluteIndex LastIndex choice\n"));
	DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", 'S', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));
	DEBUGMSG(1,("+-+-------------+-------------+---------------+---------------------+---------+------+\n"));

	for( ; RelativeIndex <= MIN(_DBX_MAX_DISPLAY_INDEX-1, LastIndex); RelativeIndex++)
		{

		AbsoluteIndex = FirstShownIndex + RelativeIndex;

 		if( AbsoluteIndex == SelectedAbsoluteIndex )
			{
			AddBookmarkPID	= 43;
			BookmarkPID		= 45;

			color			= _BLACK;
//			description 		= GULIM_13_BLACK_BOLD;
//			time				= GULIM_13_BLACK_NORMAL;
			//option			= (DT_TRANSPARENCY|DT_BOLD);
			option			= (DT_TRANSPARENCY);
			}
		else
			{
			AddBookmarkPID	= 42;
			BookmarkPID		= 44;

			color			= _WHITE;
//			description 		= GULIM_13_WHITE_BOLD;
//			time				= GULIM_13_WHITE_NORMAL_BORDER;
//			option			= (DT_TRANSPARENCY|DT_OUTLINE);
			option			= (DT_TRANSPARENCY);
			}		

	//	printf("(((((((((((((((((((((mode = %x)))))))))))))))))))))))\n", mode );

		
		if( mode == _BOOKMARK_DBX_AT_BEGINNIG && AbsoluteIndex==0 )
			{
/*
			DrawItemWithoutRefreshSkipTransparency(
				&TAVI_IMAGES[TYPE_ICON_LIST],
				AddBookmarkPID,
				sx + _DBX_ITEM_COL_SPX, 
				sy+ _DBX_SELECTBAR_START_Y + 4,
				12, 12
				);

			pT->DrawResText( 
				ML_RESUME, 
				sx + _DBX_ITEM_COL_SPX + 14,
				sy+ _DBX_SELECTBAR_START_Y + 5,
				color,
				12,
				option
				);
*/
			pT->DrawResText( 
				ML_RESUME, 
				sx + _DBX_ITEM_COL_SPX,
				sy+ _DBX_SELECTBAR_START_Y + 5,
				color,
				12,
				option
				);

			DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", 'S', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));


			}
		else if( mode == _BOOKMARK_DBX_AT_BEGINNIG && AbsoluteIndex==1 )
			{

/*
			DrawItemWithoutRefreshSkipTransparency(
				&TAVI_IMAGES[TYPE_ICON_LIST],
				AddBookmarkPID,
				sx + _DBX_ITEM_COL_SPX, 
				sy+ _DBX_SELECTBAR_START_Y + 4,
				12, 12
				);

			pT->DrawResText( 
				ML_FROM_BEGINNING, 
				sx + _DBX_ITEM_COL_SPX + 14,
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5,
				color,
				12,
				option
				);
*/				

			pT->DrawResText( 
				ML_FROM_BEGINNING, 
				sx + _DBX_ITEM_COL_SPX, 
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5,
				color,
				12,
				option
				);

			DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", 'S', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));


			}

		else
			{

			//Text Description
			DrawItemWithoutRefreshSkipTransparency(
				&TAVI_IMAGES[TYPE_ICON_LIST],
				BookmarkPID,
				sx + _DBX_ITEM_COL_SPX, 
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 4,
				12, 12
				);

			ustrcpy( ucTemp, pT->GetTextRes(ML_BOOKMARK_PLAY) );

			if( mode == _BOOKMARK_DBX_AT_BEGINNIG )
				sprintf( timestring, "%d", AbsoluteIndex-2 );
			else
				sprintf( timestring, "%d", AbsoluteIndex );
			
			str2ustr( ucTimeString, timestring, strlen(timestring) );
			ustrcat ( ucTemp, ucTimeString );

			pT->DrawUC16Text(
				ucTemp,
				sx + _DBX_ITEM_COL_SPX + 14,
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5,
				color,
				12,
				option
				);



			if( (BtnOption&ID_TEXTVIEWER_BOOKMARK) == ID_TEXTVIEWER_BOOKMARK )
				{
				RMuint32 Line;

				if( (mode&_BOOKMARK_DBX_AT_BEGINNIG) == _BOOKMARK_DBX_AT_BEGINNIG )
					Line = BookmarkList[ AbsoluteIndex -2]->LineCounter;
				else
					Line = BookmarkList[ AbsoluteIndex]->LineCounter;

				sprintf( timestring, "%ld Lines", Line );

				}
			else
				{

				if( (mode&_BOOKMARK_DBX_AT_BEGINNIG) == _BOOKMARK_DBX_AT_BEGINNIG )
					Convert_SecOnly2TimeString( timestring, BookmarkList[ AbsoluteIndex -2]->ulSavetime);
				else
					Convert_SecOnly2TimeString( timestring, BookmarkList[ AbsoluteIndex ]->ulSavetime);

				}

			str2ustr( ucTimeString, timestring, strlen(timestring) );

			pT->DrawUC16Text( 
				ucTimeString, 
				(280 - pT->GetStringWidth( ucTimeString, 12, 0 )),
				sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 4,
				color,
				12,
				option
				);			

 
			if( color = _BLACK )
				DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", '>', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));
			else
				DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", ' ', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));

 
			}
		
		
		}

	DEBUGMSG(1,("+-+-------------+-------------+---------------+---------------------+---------+------+\n"));


}


void CBookmarkDialogBox::CreateVScrollBar()
{
	int TotalItem;
	int VisibleItemCnt;
	int StartIndex;


	
}

int CBookmarkDialogBox::AddBookmark(  tBookmarkNode* pNode )
{
	if( pNode->ulSavetime > 0 )
		{
		BookmarkList.Add( pNode );
		LastSlot++;
		}
	
	return TRUE;
}

int CBookmarkDialogBox::ClearItems()
{
	SelectedAbsoluteIndex	= 0;	// Cursor
	AbsoluteIndex			= 0;	// 0 ~ LASTINDEX
	FirstShownIndex		= 0;	// Absolute Index			
	LastIndex			= 0;	// Absolute Index
	choice				= 0;

	BookmarkList.FreeAll();
	LastSlot=0;
	return TRUE;
}

U32 CBookmarkDialogBox::GetSecondsToSave()
{
	return ulBookmarkingTime;
}

U32 CBookmarkDialogBox::SetSecondsToSave(U32 seconds)
{
	ulBookmarkingTime = seconds;
	return ulBookmarkingTime;
}

int CBookmarkDialogBox::GetCurrentChoice()
{
	return choice;
}


U32 CBookmarkDialogBox::GetSeekPoint()
{

	DEBUGMSG(1,("GetSeekPoint() : choice = %d\n", choice ));

	if ( choice >= 0 )
		return  (BookmarkList[choice]->ulSavetime );
	else
		return 0;

}

U32 CBookmarkDialogBox::GetLineCounter()
{

	DEBUGMSG(1,("GetLineCounter() : choice = %d\n", choice ));

	if ( choice >= 0 )
		return  (BookmarkList[choice]->LineCounter );
	else
		return 0;

}

int CBookmarkDialogBox::GetItemCount()
{
	return ( BookmarkList.Count() );
}

int CBookmarkDialogBox::IsBlankDbx()
{
	if( 	BookmarkList.Count() == 0 &&  
		mode == _BOOKMARK_DBX_AT_PLAYING )
		{
		return TRUE;
		}
	else
		{
		return FALSE;
		}
}

int CBookmarkDialogBox::CursorUp()
{

#if 0
	if( SelectedAbsoluteIndex > 0 )
		SelectedAbsoluteIndex--;						// SelectedAbsoluteIndex

	if ( SelectedAbsoluteIndex <  FirstShownIndex  
		&& FirstShownIndex > 0 )
		FirstShownIndex--;
#else

	if( SelectedAbsoluteIndex > 0 )
		{
		SelectedAbsoluteIndex--;
		if ( SelectedAbsoluteIndex <  FirstShownIndex  
			&& FirstShownIndex > 0 )
			FirstShownIndex--;
		}
	else
		{
		SelectedAbsoluteIndex	= LastIndex;				// SelectedAbsoluteIndex
		FirstShownIndex 		= LastIndex - MIN(_DBX_MAX_DISPLAY_SLOT, LastIndex);
		}
 
#endif

	if( mode == _BOOKMARK_DBX_AT_BEGINNIG )
		choice = SelectedAbsoluteIndex-2;				// choice
	else
		choice = SelectedAbsoluteIndex;

	return choice;
}


int CBookmarkDialogBox::CursorDown()
{

#if 0
	if( SelectedAbsoluteIndex < LastIndex )
		SelectedAbsoluteIndex++;

	if ( SelectedAbsoluteIndex >  (FirstShownIndex +  _DBX_MAX_DISPLAY_INDEX -1) 
		&& (FirstShownIndex +  _DBX_MAX_DISPLAY_INDEX -1) < LastIndex )
		FirstShownIndex++;
#else

	if( SelectedAbsoluteIndex < LastIndex )
		{
		SelectedAbsoluteIndex++;

		if ( SelectedAbsoluteIndex >  (FirstShownIndex +  _DBX_MAX_DISPLAY_SLOT) 
			&& (FirstShownIndex +  _DBX_MAX_DISPLAY_SLOT) < LastIndex )
			FirstShownIndex++;
		}
	else
		{
		SelectedAbsoluteIndex	= 0;				// SelectedAbsoluteIndex
		FirstShownIndex		= 0;
		}
#endif

	if( mode == _BOOKMARK_DBX_AT_BEGINNIG )
		choice = SelectedAbsoluteIndex-2;
	else
		choice = SelectedAbsoluteIndex ;

	return choice;

}




void CBookmarkDialogBox::Display()
{
	CreateSelectBar();
	CreateTextData();

	if( BookmarkList.Count() > _DBX_MAX_DISPLAY_SLOT )
		{
		pSb1 = new CScrollBar( 
			sx + width - (8+5), 
			sy +_DBX_SELECTBAR_START_Y, 
			_DBX_SELECTBAR_HEIGHT * _DBX_MAX_DISPLAY_INDEX, 
			LastIndex + 1,
			FirstShownIndex,
			MIN( (LastIndex - FirstShownIndex + 1), _DBX_MAX_DISPLAY_INDEX )
			);
		}

	if( BookmarkList.Count() == 0 &&  
		mode == _BOOKMARK_DBX_AT_PLAYING )
		BuildButtons( ID_CANCEL);
	else
		BuildButtons( BtnOption );

	pMpegDecoder->RefreshOsd();
}



void CBookmarkDialogBox::ReDraw()
{
	ClearDbxContents();
	CreateSelectBar();
	CreateTextData();

	DEBUGMSG( 1, ("CBookmarkDialogBox::ReDraw() \n - LastIndex : [%d]\n -FirstShownIndex : [%d]\n -height : [%d]\n",
		LastIndex, FirstShownIndex, MIN((LastIndex - FirstShownIndex), _DBX_MAX_DISPLAY_INDEX ) ));


	if( BookmarkList.Count() > _DBX_MAX_DISPLAY_SLOT )
		{
		if ( pSb1 == NULL )
			{
			pSb1 = new CScrollBar( 
				sx + width - (8+5), 
				sy +_DBX_SELECTBAR_START_Y, 
				_DBX_SELECTBAR_HEIGHT * _DBX_MAX_DISPLAY_INDEX, 
				LastIndex + 1,
				FirstShownIndex,
				MIN( (LastIndex - FirstShownIndex + 1), _DBX_MAX_DISPLAY_INDEX )
				);
			}
		else
			{
			pSb1->ReDraw(
				LastIndex + 1,
				FirstShownIndex,
				MIN( (LastIndex + 1 - FirstShownIndex), _DBX_MAX_DISPLAY_INDEX )
				);

			}			
		}

	if( BookmarkList.Count() == 0 &&  
		mode == _BOOKMARK_DBX_AT_PLAYING )
		BuildButtons( ID_CANCEL );
	else
		BuildButtons( BtnOption );
	
	pMpegDecoder->RefreshOsd();
}







