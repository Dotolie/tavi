//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : VideoPlayer Control Pannel Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibDialogBox.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibDialogBox.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.18  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.17  2005/12/14 09:56:17  etnlwind
// Update for Shutdown on VideoPlayer
// - Brightness & Contrast Range를 0~60으로 수정
// - Brightness 대화상자의 크기를 줄여 하단으로 옮김
// - Brightness 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Brightness 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Bookmark 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Bookmark 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Shutdown으로 종료시 화면의 LCD를 Off, 이후 Shutdown 이미지 출력전에 On 처
//
// Revision 1.16  2005/11/25 08:40:40  etnlwind
// Update for New EQ
//
// Revision 1.15  2005/11/11 04:25:58  etnlwind
// Update for VideoPlayer Screen Clean Method
//
// Revision 1.14  2005/10/12 09:24:05  etnlwind
// 1. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.13  2005/08/14 09:38:33  etnlwind
// Update for
// 1. VideoPlayer Bookmark Delete
// 2. VideoPlayer FileBrowser Subtitle Ready Video File Icon Specificationized
// 3. VideoPlayer UI minor updates
//
// Revision 1.12  2005/08/12 11:17:24  etnlwind
// Update for Sound Palette
//
// Revision 1.11  2005/08/10 09:54:00  etnlwind
// Update for Video Player Sound Palette Dbx
//
// Revision 1.10  2005/07/15 09:46:28  etnlwind
// Update for Video Player SubtitleSyncDbx
//
// Revision 1.9  2005/07/14 07:38:47  etnlwind
// Update for Video Player
//
// Revision 1.8  2005/07/13 06:02:30  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.7  2005/07/12 12:45:35  etnlwind
// Update for LibFileDbx
//
// Revision 1.6  2005/07/12 02:30:14  etnlwind
// Update for Base Class Destructor Changed to Virtual
//
// Revision 1.5  2005/04/26 10:06:56  etnlwind
// Upate for ES4 UI
//
// Revision 1.4  2005/04/22 14:37:35  etnlwind
// Library Update for UI changes
//
// Revision 1.3  2005/04/22 01:58:39  etnlwind
// MultiLanguage Support
//
// Revision 1.2  2005/03/02 11:58:41  etnlwind
// Msg 처리 추가
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library 정리
//
//
//*****************************************************************************


 #ifndef __LIB_DIALOG_BOX_H__
 #define __LIB_DIALOG_BOX_H__

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

/*
#include "../Component/object.h"
#include "../Component/Stack.h"
#include "../Component/event.h"
#include "../Component/fbdev.h"
#include "../Component/image.h"
#include "../Component/timer.h"
*/

#include <Lib/LibImage.h>



///////////////////////////////////////////////////////////////////////
//
// CDialogBox Class
// --------------------------

#define _DBX_TITLE_RELATIVE_X				7
#define _DBX_TITLE_RELATIVE_Y				3
#define _DBX_TITLE_HEIGHT					21
#define _DBX_ROW_SPACE						9
#define _DBX_ROW_HEIGHT					22
#define _DBX_TITLE_COLOR_PALLETTE 			31
#define _DBX_BASE_COLOR_PALLETTE 			24
#define _DBX_SELECTED_ITEM_BAR_COLOR			17
#define _DBX_SELECTED_ITEM_BAR_TRANSPARENCY_COLOR		28
#define _DBX_SELECTED_ITEM_BAR_SHADOW_COLOR	31
#define _DBX_ITEM_COL_SPX					15


#define _DBX_SELECTBAR_START_X				7
#define _DBX_SELECTBAR_START_Y				30
#define _DBX_SELECTBAR_HEIGHT				22

#define _DBX_ITEM_ROW_SPACE				6

#if 0
#define _DBX_MAX_DISPLAY_SLOT		6
#define _DBX_MAX_DISPLAY_INDEX		7
#else
#define _DBX_MAX_DISPLAY_SLOT		5
#define _DBX_MAX_DISPLAY_INDEX	6
#endif

#define ICON_DBXICON_WIDTH 	24
#define BTN_ICON_MAX_X 			295

#define ID_OK 							0x00000001
#define ID_LEFT 							0x00000002
#define ID_BACK 							0x00000004
#define ID_CANCEL 						0x00000008
#define ID_EXIT 							0x00000010
#define ID_EDIT 							0x00000020
#define ID_APPLY 						0x00000040
#define ID_SELECT 						0x00000080
#define ID_DELETE 						0x00000100

#define ID_TEXTVIEWER_BOOKMARK		0x00010000

#define ID_EQ_MEX_2BAND_SLIDER	 	0x00010000
#define ID_EQ_REXWIDE_2BAND_SLIDER 	0x00020000
#define ID_EQ_REXLIVE_2BAND_SLIDER 	0x00040000
#define ID_EQ_5BAND_SLIDER 			0x00080000




class CDialogBox
{
public:
	int 	choice;
	char 	title[256];
	char 	item[8][256];
	U32 	itemRes[8];
	U32 TextRes;
	U32 TitleRes;
	int	sx;
	int 	sy;
	int	width;
	int 	height;
	int 	LastSlot;
	int 	DialogType;
	U32 BtnOption;
	
	MSG_PROCESSOR fpDialogBoxMsgProc;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// V2
	int					SelectedAbsoluteIndex;			// Cursor
	int					AbsoluteIndex;					// 0 ~ LASTINDEX
	int					RelativeIndex;
	int					FirstShownIndex;				
	int					LastIndex;

	CScrollBar*			pSb1;	
	/////////////////////////////////////////////////////////////////////////////////////////////

	void CreateDbxBackground();
	void BuildControlPannelBackground();
	void ClearControlPannelBackground();
	void ClearDbxContents();
	void CreateSelectBar();
	void CreateTextData();


	CDialogBox(){}
	CDialogBox( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option  );
	CDialogBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option  );
	virtual ~CDialogBox();
	
	int SetTitle( char text[] );
	int SetTitle( U32 ResIndex );
	int AddMsg( char contents[] );
	int AddItem(int idx, char contents[] );
	int AddItem(int idx, U32 ResIndex );

	int CursorUp();
	int CursorDown();

	int VisibleCursorUp();
	int VisibleCursorDown();
	int GetCurrentChoice();
	U32 GetCurrentParameters();

	void SetChoice( int value );

#if 0
	int SetChoiceToExitReverse();
	int SetChoiceToLongExitReverse();
	int SetChoiceToModeReverse();
	int SetChoiceToLongModeReverse();
#endif

	int BuildButtons( U32 option );
	void Display();
	void ReDraw();
	
	int MsgBtnUp( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
	int MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
		
};




#endif


