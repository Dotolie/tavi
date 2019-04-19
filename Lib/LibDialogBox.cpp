//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : VideoPlayer Control Pannel Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibDialogBox.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:15:30 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibDialogBox.cpp,v $
// Revision 1.2  2006/06/07 08:15:30  zstein
// - removed unused codes.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.25  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.24  2005/12/14 09:56:17  etnlwind
// Update for Shutdown on VideoPlayer
// - Brightness & Contrast Range를 0~60으로 수정
// - Brightness 대화상자의 크기를 줄여 하단으로 옮김
// - Brightness 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Brightness 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Bookmark 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Bookmark 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Shutdown으로 종료시 화면의 LCD를 Off, 이후 Shutdown 이미지 출력전에 On 처
//
// Revision 1.23  2005/11/25 08:40:39  etnlwind
// Update for New EQ
//
// Revision 1.22  2005/11/21 02:51:31  etnlwind
// Update for EQ Select UI
//
// Revision 1.21  2005/11/11 04:25:58  etnlwind
// Update for VideoPlayer Screen Clean Method
//
// Revision 1.20  2005/08/29 03:24:41  etnlwind
// Update for Bookmark Dbx Bug Fix
//
// Revision 1.19  2005/08/29 01:26:00  etnlwind
// Update for Video Player
//
// Revision 1.18  2005/08/25 09:26:57  etnlwind
// Update for FileManager / Virtual Keyboard Update
//
// Revision 1.17  2005/08/17 03:22:22  etnlwind
// Update for FileManager
//
// Revision 1.15  2005/08/14 09:38:33  etnlwind
// Update for
// 1. VideoPlayer Bookmark Delete
// 2. VideoPlayer FileBrowser Subtitle Ready Video File Icon Specificationized
// 3. VideoPlayer UI minor updates
//
// Revision 1.14  2005/08/12 11:17:24  etnlwind
// Update for Sound Palette
//
// Revision 1.13  2005/08/10 09:54:00  etnlwind
// Update for Video Player Sound Palette Dbx
//
// Revision 1.12  2005/07/15 09:46:28  etnlwind
// Update for Video Player SubtitleSyncDbx
//
// Revision 1.11  2005/07/13 06:02:30  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.10  2005/07/12 12:45:35  etnlwind
// Update for LibFileDbx
//
// Revision 1.9  2005/05/18 17:54:47  etnlwind
// Update for Video Player Timer Related Fix
//
// Revision 1.8  2005/05/18 14:48:38  etnlwind
// Update for Timer Related Problems
//
// Revision 1.7  2005/04/22 14:37:35  etnlwind
// Library Update for UI changes
//
// Revision 1.6  2005/04/22 01:58:39  etnlwind
// MultiLanguage Support
//
// Revision 1.5  2005/03/05 13:38:16  etnlwind
// Timer
//
// Revision 1.4  2005/03/05 13:00:45  etnlwind
// Timer관련 App수정
//
// Revision 1.3  2005/03/03 18:25:36  etnlwind
// Library 수정
//
// Revision 1.2  2005/03/02 11:58:41  etnlwind
// Msg 처리 추가
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
// CDialogBox Class Figure
//
//

CDialogBox::CDialogBox( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option )
{
	DEBUGMSG(1,("CDialogBox( [%s], %d, %d, %d, %d)\n", sDbxTitle, x, y, w, h ));

	Stop_VideoControlPannelTimer();
	memset( title, 0, 256 );
	memset( item, 0, 8*256 );

	choice = -1;
	sx = x;
	sy = y;
	width = w;
	height = h;

	DialogType = 0;

//	if ( pfpMsgProc == 0 )
//		fpDialogBoxMsgProc = MsgProc_WaitButtonPress;
//	else
	fpDialogBoxMsgProc = pfpMsgProc;

	CreateDbxBackground();
	SetTitle( sDbxTitle );
}

CDialogBox::CDialogBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option )
{
	DEBUGMSG(1,("CDialogBox( [%d], %d, %d, %d, %d)\n", ResIndex, x, y, w, h ));
	Stop_VideoControlPannelTimer();
	memset( title, 0, 256 );
	memset( item, 0, 8*256 );
	choice = -1;
	sx = x;	sy = y;	width = w;	height = h;	DialogType = 0;
	fpDialogBoxMsgProc = pfpMsgProc;
	CreateDbxBackground();
	SetTitle( ResIndex );
	BtnOption = option;
}

CDialogBox::~CDialogBox()
{
	pMpegDecoder->RenderText( 0, 0, 0, 30, 320, 210 );
}

void CDialogBox::SetChoice( int value )
{
	choice = value;
}

#if 0
int CDialogBox::SetChoiceToModeReverse()
{
	choice = TAVI_KEY_MODE_REVERSE;
	return choice;
}
int CDialogBox::SetChoiceToLongModeReverse()
{
	choice = TAVI_KEY_LMODE_REVERSE;
	return choice;
}
int CDialogBox::SetChoiceToExitReverse()
{
	choice = TAVI_KEY_EXIT_REVERSE;
	return choice;
}
int CDialogBox::SetChoiceToLongExitReverse()
{
	choice = TAVI_KEY_LEXIT_REVERSE;
	return choice;
}
#endif

void CDialogBox::CreateDbxBackground()
{
	DEBUGMSG(1, (" - CreateDbxBackground()\n"));

	DrawBoxWithoutRefresh(
		_DBX_BASE_COLOR_PALLETTE,	// RMuint8 color
		sx,							// RMuint16 sx
		sy, 							// RMuint16 sy
		width, 						//RMuint16 width
		height						// RMuint16 height)
		);

	DrawBoxWithoutRefresh(
		_DBX_TITLE_COLOR_PALLETTE,	// RMuint8 color
		sx,							// RMuint16 sx
		sy, 							// RMuint16 sy
		width, 						//RMuint16 width
		_DBX_TITLE_HEIGHT			// RMuint16 height)
		);

	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 0, sx, sy, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 1, sx+width-4, sy, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 2, sx, sy+height-4, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 3, sx+width-4, sy+height-4, 4,4 );

	
	
}

void CDialogBox::BuildControlPannelBackground()
{
	DrawBoxWithoutRefresh(
		_CP_BASE_COLOR_PALLETTE_INDEX,	// RMuint8 color
		0,							// RMuint16 sx
		_CP_BASE_START_Y_POS,
		320, 						//RMuint16 width
		_CP_HEIGHT+1
		);

	DrawItemWithoutRefresh( &TAVI_IMAGES[CONTROLPANNEL_EDGE], 0, 0, _CP_BASE_START_Y_POS, 4,4 );
	DrawItemWithoutRefresh( &TAVI_IMAGES[CONTROLPANNEL_EDGE], 1, 319-4, _CP_BASE_START_Y_POS, 4,4 );
}
void CDialogBox::ClearControlPannelBackground()
{
	DrawBoxWithoutRefresh(
		0,
		0,							// RMuint16 sx
		_CP_BASE_START_Y_POS,
		320, 						//RMuint16 width
		_CP_HEIGHT+1
		);
}


void CDialogBox::ClearDbxContents()
{
	DrawBoxWithoutRefresh(
		_DBX_BASE_COLOR_PALLETTE,	// RMuint8 color
		sx+2,							// RMuint16 sx
		sy+_DBX_TITLE_HEIGHT, 		// RMuint16 sy
		width-4, 						//RMuint16 width
		height-_DBX_TITLE_HEIGHT-2		// RMuint16 height)
		);			
}




void CDialogBox::CreateSelectBar()
{
	//int sby = sy+ _DBX_SELECTBAR_START_Y + ( (choice-1)*_DBX_SELECTBAR_HEIGHT );
	int sby = sy+ _DBX_SELECTBAR_START_Y + ( choice * _DBX_SELECTBAR_HEIGHT );

	DrawBoxWithoutRefresh( 
		_DBX_SELECTED_ITEM_BAR_COLOR, 
		sx+_DBX_SELECTBAR_START_X, 
		sby, 
		width-14,
		_DBX_ROW_HEIGHT-1
		);

	DrawBoxWithoutRefresh( 
		_DBX_SELECTED_ITEM_BAR_SHADOW_COLOR, 
		sx+_DBX_SELECTBAR_START_X, 
		sby+_DBX_ROW_HEIGHT-1, 
		width-14,
		1
		);

	DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE], 0, sx+_DBX_TITLE_RELATIVE_X, sby, 4,22 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE], 1, sx+_DBX_TITLE_RELATIVE_X+width-(_DBX_TITLE_RELATIVE_X*2), sby, 4,22 );
}




void CDialogBox::CreateTextData()
{
	U16 color;
	U32 option;
	int iFont;

#if 1		
		for( int i=0; i<MIN(8,LastSlot+1); i++)
			{
			if( choice == i )
				{
				color = _BLACK;
				option = (DT_TRANSPARENCY);
				}
			else
				{
				color = _WHITE;
				option = (DT_TRANSPARENCY);
				}

			pT->DrawResText( 
				itemRes[i], 
				sx + _DBX_ITEM_COL_SPX, 
				sy+ _DBX_SELECTBAR_START_Y + ( i*_DBX_SELECTBAR_HEIGHT ) + 3 + 3,
				color,
				12,
				option
				);
			}
#else
		}

	else
		{
	
		for( int i=0; i<8; i++)
			{
			if( choice == i )
				iFont = GULIM_13_GRAY_BOLD;
			else
				iFont = GULIM_13_WHITE_NORMAL_BORDER;
			
			DrawASCIITextWithoutRefresh(
				item[i], 
				16, 
				15, 
				&TAVI_IMAGES[iFont],
				sx + _DBX_ITEM_COL_SPX,
				sy+ _DBX_SELECTBAR_START_Y + ( i*_DBX_SELECTBAR_HEIGHT ) + 3,
				__SKIP_TRANSPARENCY_COLOR
				);
			}

		}
#endif
	
}



int CDialogBox::SetTitle( U32 ResIndex )
{
	return (pT->DrawResText( ResIndex, sx+_DBX_TITLE_RELATIVE_X, sy+_DBX_TITLE_RELATIVE_Y+3, _WHITE, 12, DT_TRANSPARENCY ));
}


int CDialogBox::SetTitle( char text[] )
{
	int szStrlen=-1;

/*	
	if( (szStrlen = strlen(text)) > 253 )
		{
		text[252] = '.';
		text[253] = '.';
		text[254] = '.';
		text[255] = '\0';
		}





	
	DrawASCIITextWithoutRefresh(
		text, 
		16, 
		15, 
		&TAVI_IMAGES[GULIM_13_GRAY_BOLD],
		sx + _DBX_TITLE_RELATIVE_X,
		sy + _DBX_TITLE_RELATIVE_Y,
		__SKIP_TRANSPARENCY_COLOR
		);

	sprintf( title, "%s", text );
	
	return szStrlen;

*/

	return (pT->DrawEUC8Text( text, sx+_DBX_TITLE_RELATIVE_X, sy+_DBX_TITLE_RELATIVE_Y+3, _WHITE, 12, DT_TRANSPARENCY ));
}

int CDialogBox::AddMsg( char contents[] )
{
	return FALSE;
}



int CDialogBox::AddItem(int idx, char contents[] )
{
	
	int szStrlen=-1;

	DialogType = 1;

	if(idx > 7 )
		return FALSE;
		
	if( (szStrlen = strlen(item[idx])) > 253 )
		{
		item[idx][252] = '.';
		item[idx][253] = '.';
		item[idx][254] = '.';
		item[idx][255] = '\0';
		}
	 
	sprintf( item[idx], "%s", contents );

	LastSlot = idx;

	return szStrlen;
	 
}

int CDialogBox::AddItem(int idx, U32 ResIndex )
{
	
	int szStrlen=-1;
	DialogType = 1;

	if(idx > 7 )
		return FALSE;

	itemRes[idx] = ResIndex;
	LastSlot = idx;

	return szStrlen;
	 
}

int CDialogBox::BuildButtons( U32 option )
{
	int x1, x2, x3;
	int tw1, tw2, tw3;
	int btn_sy = sy+height-16;

	if( (option&ID_EDIT)==ID_EDIT && (option&ID_APPLY)==ID_APPLY && (option&ID_CANCEL)==ID_CANCEL )
		{
		tw1 = pT->GetStringWidth( (U32)ML_EDIT, 12, 0 );
		tw2 = pT->GetStringWidth( (U32)ML_APPLY, 12, 0 );
		tw3 = pT->GetStringWidth( (U32)ML_CANCEL, 12, 0 );
		x3 = BTN_ICON_MAX_X - tw3 - (ICON_DBXICON_WIDTH+2);
		x2 = x3 - tw2- (ICON_DBXICON_WIDTH+6);
		x1 = x2 - tw1- (ICON_DBXICON_WIDTH+6);

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 3, x1, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_EDIT, x1+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, x2, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_APPLY, x2+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, x3, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_CANCEL, x3+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
		
		}
	else if( (option&ID_APPLY)==ID_APPLY && (option&ID_CANCEL)==ID_CANCEL )
		{
		tw2 = pT->GetStringWidth( (U32)ML_APPLY, 12, 0 );
		tw3 = pT->GetStringWidth( (U32)ML_CANCEL, 12, 0 );
		x3 = BTN_ICON_MAX_X - tw3 - (ICON_DBXICON_WIDTH+2);
		x2 = x3 - tw2- (ICON_DBXICON_WIDTH+6);		

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, x2, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_APPLY, x2+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, x3, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_CANCEL, x3+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
		}

	else if( (option&ID_SELECT)==ID_SELECT && (option&ID_DELETE)==ID_DELETE && (option&ID_CANCEL)==ID_CANCEL )
		{
		tw1 = pT->GetStringWidth( (U32)ML_SELECT, 12, 0 );
		tw2 = pT->GetStringWidth( (U32)ML_DELETE, 12, 0 );
		tw3 = pT->GetStringWidth( (U32)ML_CANCEL, 12, 0 );
		
		x3 = BTN_ICON_MAX_X - tw3 - (ICON_DBXICON_WIDTH+2);
		x2 = x3 - tw2- (ICON_DBXICON_WIDTH+6);
		x1 = x2 - tw1- (ICON_DBXICON_WIDTH+6);

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, x1, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_SELECT, x1+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 1, x2, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_DELETE, x2+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, x3, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_CANCEL, x3+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
		}

	else if( (option&ID_SELECT)==ID_SELECT  && (option&ID_CANCEL)==ID_CANCEL )
		{
		tw2 = pT->GetStringWidth( (U32)ML_SELECT, 12, 0 );
		tw3 = pT->GetStringWidth( (U32)ML_CANCEL, 12, 0 );
		x3 = BTN_ICON_MAX_X - tw3 - (ICON_DBXICON_WIDTH+2);
		x2 = x3 - tw2- (ICON_DBXICON_WIDTH+6);		

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, x2, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_SELECT, x2+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );

		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, x3, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_CANCEL, x3+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
		}


	else if( option == ID_OK )
		{
		tw1 = pT->GetStringWidth( (U32)ML_OK, 12, 0 );
		x1 = 320/2 - (tw1+24+2)/2;
		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, x1, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_OK, x1+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
		}

	else if( option == ID_CANCEL )
		{
		tw1 = pT->GetStringWidth( (U32)ML_CANCEL, 12, 0 );
		x1 = 320/2 - (tw1+24+2)/2;
		DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, x1, btn_sy-2, 24, 14 );
		pT->DrawResText( ML_CANCEL, x1+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
		}

	else
		{

		

		tw1 = pT->GetStringWidth( (U32)ML_SELECT, 12, 0 );
	//	tw2 = pT->GetStringWidth( (U32)ML_DELETE, 12, 0 );
		tw3 = pT->GetStringWidth( (U32)ML_CANCEL, 12, 0 );

	//	printf("- tw1 : %d   tw3 : %d\n", tw1, tw3 );
		

		x3 = BTN_ICON_MAX_X - tw3 - (ICON_DBXICON_WIDTH+2);
		x1 = x3 - tw1 - (ICON_DBXICON_WIDTH+6);

		if((option&ID_OK) == ID_OK)
			{	
			DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, x1, btn_sy-2, 24, 14 );
			pT->DrawResText( ML_SELECT, x1+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
			}

		if( (option&ID_BACK) == ID_BACK )
			{
			DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, x3, btn_sy-2, 24, 14 );
			pT->DrawResText( ML_BACK, x3+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
			}

		if( (option&ID_CANCEL) == ID_CANCEL )
			{
			DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, x3, btn_sy-2, 24, 14 );
			pT->DrawResText( ML_CANCEL, x3+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
			}

		if( (option&ID_EXIT) == ID_EXIT )
			{
			DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, x3, btn_sy-2, 24, 14 );
			pT->DrawResText( ML_EXIT, x3+26, btn_sy, _WHITE, 12, DT_TRANSPARENCY );
			}

		}
	return TRUE;

}
	
#if 0	
	if( (option&ID_BACK) == ID_BACK )
		{

		if((option&ID_LEFT) == ID_LEFT )
			{

			if((option&ID_OK) == ID_OK)
				{
				// ID_OK  ID_LEFT  ID_BACK
				x3 = BTN_ICON_MAX_X - tw3 - (ICON_DBXICON_WIDTH+2);
				x2 = x3 - tw2 -
;
				
DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, x1, sy, 24, 14 );
pT->DrawResText( ML_SELECT, x1+16, sy, _WHITE, 12, DT_TRANSPARENCY );

DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, x2, sy, 24, 14 );
pT->DrawResText( ML_DELETE, x2+16, sy, _WHITE, 12, DT_TRANSPARENCY );

DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, x3, sy, 24, 14 );
pT->DrawResText( ML_CANCEL, x3+16, sy, _WHITE, 12, DT_TRANSPARENCY );
				}
			else
				{
				// ID_LEFT  ID_BACK
				}
			
			}
		else
			{
			if((option&ID_OK) == ID_OK)
				{
				// ID_OK  ID_BACK
				}
			else
				{
				// ID_BACK
				
				}
			}
		
		}
	else
		{
		if((option&ID_LEFT) == ID_LEFT )
			{

			if((option&ID_OK) == ID_OK)
				{
				// ID_OK  ID_LEFT
				}
			else
				{
				// ID_LEFT 
				}
			
			}
		else
			{
			if((option&ID_OK) == ID_OK)
				{
				// ID_OK
				}
			}		
		}
}
#endif

int CDialogBox::CursorUp()
{
	choice--;
	
	if(choice<0)
		choice = LastSlot;

	return choice;
}

int CDialogBox::CursorDown()
{
	choice++;

	if( choice > LastSlot )
		choice = 0;

	return choice;
}

int CDialogBox::VisibleCursorUp()
{
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
 
	choice = SelectedAbsoluteIndex;
	return choice;
}

int CDialogBox::VisibleCursorDown()
{

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

	choice = SelectedAbsoluteIndex ;
	return choice;
}


int CDialogBox::GetCurrentChoice()
{
	return choice;
}

U32 CDialogBox::GetCurrentParameters()
{ 
	return BtnOption;
}

void CDialogBox::Display()
{
	if( DialogType == 0 )
		{
		;
		}
	else
		{
		choice = 0;
		CreateSelectBar();
		CreateTextData();
		}

	BuildButtons( BtnOption );

	pMpegDecoder->RefreshOsd();
}

void CDialogBox::ReDraw()
{
	if( DialogType == 0 )
		{
		;
		}
	else
		{
		ClearDbxContents();
		CreateSelectBar();
		CreateTextData();
		}

	BuildButtons( BtnOption );

	pMpegDecoder->RefreshOsd();
}

int CDialogBox::MsgBtnUp( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	return ( fpDialogBoxMsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
}


int CDialogBox::MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	return ( fpDialogBoxMsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
}






