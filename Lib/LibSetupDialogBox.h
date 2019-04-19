//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CSetupDialogBox Class Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSetupDialogBox.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSetupDialogBox.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.10  2006/01/09 08:31:45  etnlwind
// Update for Saturation Control
//
// Revision 1.9  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.8  2005/12/14 09:56:17  etnlwind
// Update for Shutdown on VideoPlayer
// - Brightness & Contrast Range를 0~60으로 수정
// - Brightness 대화상자의 크기를 줄여 하단으로 옮김
// - Brightness 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Brightness 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Bookmark 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Bookmark 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Shutdown으로 종료시 화면의 LCD를 Off, 이후 Shutdown 이미지 출력전에 On 처
//
// Revision 1.7  2005/11/08 01:28:46  zstein
// - TaviSysParam의 멤버인자 데이터형 변경에 따른 관련 데이터형 변경.
// - char* pSurround; char* pBass; -> int* pSurround; int* pBass
//
// Revision 1.6  2005/08/19 05:45:41  etnlwind
// Update for VideoPlayer EQ BugFix
//
// Revision 1.5  2005/08/12 11:17:24  etnlwind
// Update for Sound Palette
//
// Revision 1.4  2005/07/13 06:02:31  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.3  2005/04/26 10:06:57  etnlwind
// Upate for ES4 UI
//
// Revision 1.2  2005/03/02 11:58:41  etnlwind
// Msg 처리 추가
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library 정리
//
//
//*****************************************************************************


 #ifndef __LIB_SETUP_DIALOG_BOX_H__
 #define __LIB_SETUP_DIALOG_BOX_H__



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
// CSetupDialogBox Class
// --------------------------

typedef enum
{
	HORIZONTAL_SELECT_BAR=0,
	HORIZONTAL_SLIDER

} eSETUP_ITEM_TYPE;

typedef struct _tag_SETUPOPTIONSET
{
//	U32 ParentResIndex;
//	U32 ItemCount;
	U32 ResIndexes[32];
} _ATTRIBUTE_PACKED_ SETUPOPTIONSET;

typedef struct _tag_SETUPOPTIONBAR
{
	U32 ParentResIndex;
	U32 CursorPosition;	// 0 ~ 100
} _ATTRIBUTE_PACKED_ SETUPOPTIONBAR;


#define _LSDBX_UNICODE_ONLY_MODE 0

typedef struct _tag_SetupDbxNode
{
	int	iCurrentItemIndex;
	int 	iMaxItemIndex;
	eSETUP_ITEM_TYPE ItemType;

#if _LSDBX_UNICODE_ONLY_MODE
	U32 riName;
	SETUPOPTIONSET* optionset;
#else	
	U32 riName;
	SETUPOPTIONSET* optionset;
	char sName[256];
	char sOption[32][256];
#endif

} _ATTRIBUTE_PACKED_ tSetupDbxNode;

class CSetupDialogBox: public CDialogBox
{
	CList<tSetupDbxNode> 	ItemList;
	int	halfOfWidth;
	int	sx2sp;
	int 	iVisibleStatus;
	int 	sby;

	char 	title[256];

	// Original Video Size;
	int 	ovx, ovy, ovw, ovh;

	// Slots
//	int					choice;

	// Indexes
	int					SelectedAbsoluteIndex;			// Cursor
	int					AbsoluteIndex;					// 0 ~ LASTINDEX
	int					RelativeIndex;
	int					FirstShownIndex;				
	int					LastIndex;

	CScrollBar2*			pSb1;


	// Brightness & Contrast
	char					PreVideoBrightness;
	char					PreVideoContrast;
	char					PreVideoSaturation;
	RMint32*				pBrightnessValue;
	RMint32*				pContrastValue;
	RMint32*				pSaturationValue;

	// 2 Band EQ
	char bevSurround;	// bev = Before Edit Value
	char bevBass;
	int* pSurround;
	int* pBass;

	// 5 Band EQ
	char bevBand0;
	char bevBand1;
	char bevBand2;
	char bevBand3;
	char bevBand4;
	
	void CreateSelectBar();
	void CreateTextData();
	void CreateVScrollBar();
	void DrawScrollBar();

	void Prepare_5BandEQ();
	void Prepare_2BandEQ( int type );
	void Prepare_BrightnessAndContrast();

	void Process_5BandEQ();
	void Process_2BandEQ();
	void Process_BrightnessAndContrast();	

	void Cancel_5BandEQ();
	void Cancel_2BandEQ( int type );
	void Cancel_BrightnessAndContrast();	

	
public:
	CSetupDialogBox(){}
	CSetupDialogBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR fpMsgProc, SETUP_PROCESSOR fpSetupProc, U32 option  ) ;
	CSetupDialogBox( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR fpMsgProc, SETUP_PROCESSOR fpSetupProc, U32 option  ) ;

	SETUP_PROCESSOR	UpdateSetting;

//	int AddSetupItem( int idx, char sContents[], char sOptionsWithBar[] );
	int AddSetupItem( int idx, char sContents[], char sOptionsWithBar[], int iDefaultValue );
	int AddSetupItem( int idx, U32 ResIndex, const U32 option[], int iDefaultValue, int iMaxItemIndex );
	int AddSetupBarItem( int idx, U32 ResIndex, int iDefaultValue );
	int AddSetupBarItem( int idx, U32 ResIndex, int iDefaultValue, int MaxValue=100 );
//	int AddItem( tSetupDbxNode* pNode );
//	int GetSecondsToSave();
//	int SetSecondsToSave(int seconds);
	int GetCurrentChoice();
	int GetCurrentItemIndex();
	int GetSetupStatus( int ChoiceIndex );
//	int GetSeekPoint();
	int CursorUp();
	int CursorDown();
	int CursorLeft();
	int CursorRight();

	int  isVisible();
	void Cancel();
	void ReDraw();
	int Show();
	int Hide();
};




#endif


