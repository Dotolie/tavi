//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CBookmarDialogBox Class Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibBookmarkDialogBox.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibBookmarkDialogBox.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.12  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.11  2005/12/14 09:56:17  etnlwind
// Update for Shutdown on VideoPlayer
// - Brightness & Contrast Range를 0~60으로 수정
// - Brightness 대화상자의 크기를 줄여 하단으로 옮김
// - Brightness 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Brightness 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Bookmark 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Bookmark 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Shutdown으로 종료시 화면의 LCD를 Off, 이후 Shutdown 이미지 출력전에 On 처
//
// Revision 1.10  2005/12/07 10:12:28  etnlwind
// Update for TextViewer TASK_KEY
//
// Revision 1.9  2005/12/07 03:03:25  etnlwind
// Update for Emergency Exit On Bookmark DialogBox
//
// Revision 1.8  2005/10/14 11:39:38  etnlwind
// Update for TextViwer
//
// Revision 1.7  2005/09/16 02:51:31  etnlwind
// Update for Bookmark Dbx Touchpad Input Filtering Case By Case
//
// Revision 1.6  2005/08/14 09:38:33  etnlwind
// Update for
// 1. VideoPlayer Bookmark Delete
// 2. VideoPlayer FileBrowser Subtitle Ready Video File Icon Specificationized
// 3. VideoPlayer UI minor updates
//
// Revision 1.5  2005/07/11 13:02:13  etnlwind
// Update for CVS
//
// Revision 1.4  2005/05/15 16:27:52  etnlwind
// Update for Battety Msg Skip on VideoPlayer
//
// Revision 1.3  2005/04/26 10:06:57  etnlwind
// Upate for ES4 UI
//
// Revision 1.2  2005/04/20 11:28:46  etnlwind
// update for ES4
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library 정리
//
//
//*****************************************************************************


 #ifndef __LIB_BOOKMARK_DIALOG_BOX_H__
 #define __LIB_BOOKMARK_DIALOG_BOX_H__

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
// CBookmarkDialogBox Class
// --------------------------


typedef struct _tag_BookmarkNode
{
	//RMint64 i64Savetime;
//	int iSavetime;
	U32 ulSavetime;
	U32 LineCounter;
} tBookmarkNode;

typedef enum
{
	_BOOKMARK_DBX_AT_BEGINNIG = 0x10000000,
	_BOOKMARK_DBX_AT_PLAYING = 0x20000000
} eBOOKMARK_TYPE;

class CBookmarkDialogBox: public CDialogBox
{
	CList<tBookmarkNode> 	BookmarkList;
	eBOOKMARK_TYPE 		mode;
	char 					BookmarkingTimeString[10];
	U32	 					ulBookmarkingTime;
	U32						LineCounter;
	


	// Slots
//	int					FirstShownSlot;
//	int					choice;
	//int					slot;							// 0 ~ LASTITEM ...(MAX 256)

	// Indexes
	int					SelectedAbsoluteIndex;			// Cursor
	int					AbsoluteIndex;					// 0 ~ LASTINDEX
	int					RelativeIndex;
	int					FirstShownIndex;				
	int					LastIndex;

	CScrollBar*			pSb1;
	void Init( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option );
	int LoadingBookmarkData();
	void CreateSelectBar();
	void CreateTextData();
	void CreateVScrollBar();

	
public:
	CBookmarkDialogBox(){}
	CBookmarkDialogBox( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option  ) ;
	CBookmarkDialogBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 ulPlaytime, U32 option  );
	CBookmarkDialogBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 AbsoluteCursor, U32 LineCounter, U32 option  );
	~CBookmarkDialogBox();

	int AddBookmark( tBookmarkNode* pNode );
	int ClearItems();
	int GetItemCount();
	int IsBlankDbx();
	U32 GetSecondsToSave();
	U32 SetSecondsToSave(U32 seconds);

	int GetCurrentChoice();
	U32 GetSeekPoint();
	U32 GetLineCounter();
	int CursorUp();
	int CursorDown();
	void Display();
	void ReDraw();
};

 #endif


