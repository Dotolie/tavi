//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : TextViewer Task Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/TextViewer/TextViewer.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: TextViewer.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.15  2005/12/29 07:42:30  ywkim
// Add function of TV_IsViewingOnForeground() for lcd off
//
// Revision 1.14  2005/12/22 05:13:22  etnlwind
// Update for New Version
//
// Revision 1.13  2005/12/07 10:13:10  etnlwind
// Update for TASK KEY
//
// Revision 1.12  2005/11/25 13:52:49  etnlwind
// Update for New EQ UI
//
// Revision 1.11  2005/11/21 02:51:44  etnlwind
// Update for EQ Select UI
//
// Revision 1.10  2005/11/07 10:26:47  etnlwind
// 텍스트뷰어 기능추가
// -한페이지에 13라인 출력하게끔 수정 (기존:16라인)
// -페이지 이동 처음/끝 추가
// -자동 라인 스크롤 추가 (slow=9초, mid=6초, fast=3초 임의지정 )
//
// Revision 1.9  2005/10/28 07:11:06  etnlwind
// New TextMapping Engine
//
// Revision 1.8  2005/10/17 10:17:22  etnlwind
// Update for VideoPlayer, TextViewer, FileManager TopPannel
//
// Revision 1.7  2005/10/14 11:39:42  etnlwind
// Update for TextViwer
//
// Revision 1.6  2005/10/12 09:24:39  etnlwind
// 1. MENU_ID_XXX를 MENU_SLOT1 ~ MENU_SLOT9로 변경
// 2. TASK_ID_XXX가 들어갈자리에 있던 MENU_ID_XXX들을 TASK_ID_XXX로 수정
// 3. MENU_ID_XXX 정의 삭제
// 4. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.5  2005/10/07 10:12:59  etnlwind
// Update for TextViewer
//
// Revision 1.4  2005/10/06 08:28:42  etnlwind
// New TextViewer Add
//
// Revision 1.2  2005/04/18 09:24:11  etnlwind
// TextViewer alpha test version
//
//
//*****************************************************************************

#ifndef __TEXT_VIEWER_H__
#define __TEXT_VIEWER_H__

#include <tavi_lib.h>
#include <ImageFont.h>

#define TV_MAX_COLUMS	44
#define TV_MAX_ROWS	12	// 16 -> 13->12

typedef enum
{
	_TVLS_FILE_BROWSER=0,
	_TVLS_TEXT_VIEW,
	_TVLS_STARTUP_DBX,
	_TVLS_BOOKMARK_DBX
}eTextViewerLastState;

typedef struct _tag_TEXTVIEWER_CONFIG 
{
	TTaskInitParam* Param;
	int TaskId;
	uchar_t* CurrentFullPathDir;

	RMuint32 CurrentBlock_Cursor;
	RMuint32 CurrentBlock_RealSize;
	RMuint32 CurrentBlock_EndPoint;

	RMuint32 CurrentPage_Cursor;
	RMuint32 CurrentPage_RealSize;
	RMuint32 CurrentPage_EndPoint;

	int CurrentLine;
	int NextLine;
	int nByteDisplayed;

	RMuint32 LineCounter;
	RMuint32 LineCounter_pre;
		
	RMuint32 PrevPage_StartPoint;
	RMuint32 TotalFileSize;

	RMuint32 TotalLines;			// additional value
	RMuint32* LineIndex;			// additional value
	RMuint32 MaxLineIndex;		// additional value

	int LastBlock;
	int LastBlockSize;
	RMuint32 AbsoluteCursor;

	int TaskExitStatus;
	eTextViewerLastState LastView;
	int IsEmergencyExitFlagEnabled;

	int RowSp[TV_MAX_ROWS];
	int IsSplited[TV_MAX_ROWS];
} __ATTRIBUTE_PACKED__  tTEXTVIEWER_CONFIG;

typedef struct _tag_TEXTVIEWER_BOOKMARKDATA 
{
	RMuint32 AbsoluteCursor;
	RMuint32 LineCounter;
}__ATTRIBUTE_PACKED__ tTEXTVIEWER_BOOKMARKDATA;

#define TEXTVIEWER_RETURN_EXIT (-1)
#define TEXTVIEWER_RETURN_NORMAL (0)
#define TEXTVIEWER_RETURN_FILE_SELECTED (1)
#define TEXTVIEWER_RETURN_STOP_VIEW (2)
#define TEXTVIEWER_RETURN_EOF (3)

#define CHK_BP_RETURN_NORMAL (0)
#define CHK_BP_RETURN_BLOCK_ADDRESS_CHANGED (1)
#define CHK_BP_RETURN_NEW_BLOCK_NEEDS (2)

#define TVB_CHK_RETURN_NORMAL (0)
#define TVB_CHK_RETURN_UPPERBLOCK_NEEDS (1)
#define TVB_CHK_RETURN_LOWERBLOCK_NEEDS (2)

#define TOKEN_STATUS_OVERFLOW (0)
#define TOKEN_STATUS_TOP (1)
#define TOKEN_STATUS_NORMAL (2)


#define _FILETYPE_UNICODE_TEXT 0


#define TVM_OPEN_FAILED_LONG_PATH						0x00001000
#define TVM_VOLUME_FULL									0x00007000
#define TVM_EXCEPTION_FOUND								0x00008000
#define TVM_OPEN_FAILED_LONG_LINE							0x00009000

#define BORDERLINE_BUFFER_SIZE 	1024
#define UPPER_BORDERLINE 			BORDERLINE_BUFFER_SIZE
#define LOWER_BORDERLINE 			(TEXTVIEWER_BLOCK_BUFFER_SIZE-BORDERLINE_BUFFER_SIZE)
#define ACTUAL_BUFFER_SIZE			LOWER_BORDERLINE

RMuint32 TV_DrawMsgBox( RMuint32 msg, RMuint32 option=0 );

// "TV" Means "TextViewer"

int _FileOpenProcess( uchar_t* filename );
int TV_MsgProc( int ssTaskId, U32 ulMsgId, int usParam1 , int usParam2 );
int TextViewer_Task( TTaskInitParam* tParam );
int TV_Kill( void );
bool TV_IsViewingOnForeground( void );


#endif

