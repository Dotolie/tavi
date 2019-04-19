//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CSetupDialogBox Class Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSetupDialogBox.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSetupDialogBox.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.32  2006/01/09 08:31:45  etnlwind
// Update for Saturation Control
//
// Revision 1.31  2005/12/30 04:24:42  etnlwind
// Update for New Font Adatio
//
// Revision 1.30  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.29  2005/12/14 09:56:17  etnlwind
// Update for Shutdown on VideoPlayer
// - Brightness & Contrast Range를 0~60으로 수정
// - Brightness 대화상자의 크기를 줄여 하단으로 옮김
// - Brightness 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Brightness 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Bookmark 대화상자 키입력 부분을 좀 더 매끄럽게 수정
// - Bookmark 대화상자에서의 Shutdown/Mode/LongMode 키 관련 버그수정
// - Shutdown으로 종료시 화면의 LCD를 Off, 이후 Shutdown 이미지 출력전에 On 처
//
// Revision 1.28  2005/12/13 09:03:09  etnlwind
// *** empty log message ***
//
// Revision 1.27  2005/11/08 07:16:12  etnlwind
// 전반
// -EXIT키 용도 재정의
// -서브메뉴에서 Close, Back삭제
//
// Revision 1.26  2005/10/25 07:09:58  etnlwind
// Update for Palette Adjust
//
// Revision 1.25  2005/10/19 05:31:51  etnlwind
// DEBUG Removed
//
// Revision 1.24  2005/10/18 09:05:13  etnlwind
// Update for New DialgBox, MessageBox UI
//
// Revision 1.23  2005/08/19 05:45:41  etnlwind
// Update for VideoPlayer EQ BugFix
//
// Revision 1.22  2005/08/18 12:35:28  etnlwind
// Update for VideoPlayer EQ Dbx
//
// Revision 1.21  2005/08/18 10:38:17  etnlwind
// Update for Video Player Sound Palette Force Closing
//
// Revision 1.20  2005/08/12 11:17:24  etnlwind
// Update for Sound Palette
//
// Revision 1.19  2005/08/09 10:52:09  etnlwind
// Update for
// 1. CSoundPaletteDbx Added
// 2. Video Control Pannel UI changed
//
// Revision 1.18  2005/08/08 10:49:41  etnlwind
// Update for FileManger, VideoPlayer
//
// Revision 1.17  2005/07/21 01:54:14  etnlwind
// Update for
// 1. AllocBitmap256( _IMAGE_ICON"/bu_left.bmp", &TAVI_IMAGES[ICON_BOTTOM_LEFT] ); changed to
//    AllocBitmap256( _IMAGE_ICON"/eq_sound.bmp", &TAVI_IMAGES[ICON_BOTTOM_LEFT] );
// 2. videoPlayer filename location changed
// 3. videoPlayer bottom right icon ordering changed
//
// Revision 1.16  2005/07/13 13:41:35  etnlwind
// Update for Brightness And Contrast DialogBox
//
// Revision 1.15  2005/07/13 12:57:32  etnlwind
// Update for DEBUGMSG Related Functions
//
// Revision 1.14  2005/07/13 06:02:31  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.13  2005/05/18 17:54:47  etnlwind
// Update for Video Player Timer Related Fix
//
// Revision 1.12  2005/05/17 18:02:05  etnlwind
// Update for Video Configuration Save
//
// Revision 1.11  2005/05/17 15:10:41  etnlwind
// Update for BG change
//
// Revision 1.10  2005/05/11 08:03:46  etnlwind
// Update for MPEG4 and Divx3 low profile Codec Related
//
// Revision 1.9  2005/04/29 14:32:44  etnlwind
// Update for ES4
//
// Revision 1.8  2005/04/27 17:01:58  etnlwind
// New Type MsgBox Library Update
//
// Revision 1.7  2005/04/26 10:06:56  etnlwind
// Upate for ES4 UI
//
// Revision 1.6  2005/03/05 13:38:16  etnlwind
// Timer
//
// Revision 1.5  2005/03/05 13:00:45  etnlwind
// Timer관련 App수정
//
// Revision 1.4  2005/03/03 18:25:36  etnlwind
// Library 수정
//
// Revision 1.3  2005/03/02 11:58:41  etnlwind
// Msg 처리 추가
//
// Revision 1.2  2005/03/01 05:43:49  ywkim
// Remark SetTitle() in CSetupDialogBox
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

#include <Task/tavi_global.h>

#include "LibDefines.h"

extern CTextEngine_BDF* pT;
extern SysParameter_t	TaviSysParam;

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// CSetupDialogBox Class Figure
//
//
CSetupDialogBox::CSetupDialogBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR fpMsgProc, SETUP_PROCESSOR fpSetupProc, U32 option  )
{
	Stop_VideoControlPannelTimer();
	LastSlot 	= 0;

	// Indexes
	SelectedAbsoluteIndex	= 0;	// Cursor
	AbsoluteIndex			= 0;	// 0 ~ LASTINDEX
	FirstShownIndex		= 0;	// Absolute Index			
	LastIndex			= 0;	// Absolute Index
	TitleRes				= ResIndex;
		
	sx = x;
	sy = y;
	width = w;
	height = h;
	iVisibleStatus= FALSE;

	BtnOption			= option;
	fpDialogBoxMsgProc 	= fpMsgProc;
	UpdateSetting			= fpSetupProc;
	pSurround			= NULL;
	pBass				= NULL;

}

CSetupDialogBox::CSetupDialogBox( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR fpMsgProc, SETUP_PROCESSOR fpSetupProc, U32 option  )
{
	DEBUGMSG(1,("CSetupDialogBox( [%s], %d, %d, %d, %d, ... , %d)\n", sDbxTitle, x, y, w, h, option ));

	Stop_VideoControlPannelTimer();
	memset( title, 0, 256 );
	sprintf( title, "%s", sDbxTitle );
	//memset( item, 0, 8*256 );
	LastSlot 	= 0;

	// Indexes
	SelectedAbsoluteIndex	= 0;	// Cursor
	AbsoluteIndex			= 0;	// 0 ~ LASTINDEX
	FirstShownIndex		= 0;	// Absolute Index			
	LastIndex			= 0;	// Absolute Index
	
	sx = x;
	sy = y;
	width = w;
	height = h;
	iVisibleStatus= FALSE;

	fpDialogBoxMsgProc 	= fpMsgProc;
	UpdateSetting			= fpSetupProc;
	BtnOption			= option;
	pSurround			= NULL;
	pBass				= NULL;	
}


//#define SCROLLBAR_SPACE	15
void CSetupDialogBox::CreateSelectBar()
{

	RelativeIndex = SelectedAbsoluteIndex - FirstShownIndex;

	if( RelativeIndex < 0)
		RelativeIndex = 0;
	else if( RelativeIndex >= _DBX_MAX_DISPLAY_INDEX )
		RelativeIndex  = _DBX_MAX_DISPLAY_INDEX-1;
 

	sby 				= sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex * _DBX_SELECTBAR_HEIGHT );
	halfOfWidth		= (width-14-SCROLLBAR_SPACE)/2;
	sx2sp			= sx+_DBX_SELECTBAR_START_X+halfOfWidth;

	DrawBoxWithoutRefresh( 
		_DBX_SELECTED_ITEM_BAR_COLOR, 
		sx+_DBX_SELECTBAR_START_X, 
		sby, 
		halfOfWidth,
		_DBX_ROW_HEIGHT-1
		);

	DrawBoxWithoutRefresh( 
		_DBX_SELECTED_ITEM_BAR_TRANSPARENCY_COLOR, 
		sx2sp, 
		sby, 
		halfOfWidth,
		_DBX_ROW_HEIGHT-1
		);

	DrawBoxWithoutRefresh( 
		_DBX_SELECTED_ITEM_BAR_SHADOW_COLOR, 
		sx+_DBX_SELECTBAR_START_X+3, 
		sby+_DBX_ROW_HEIGHT-1, 
		width-14-SCROLLBAR_SPACE-3,
		1
		);

	DrawItemWithoutRefresh( &TAVI_IMAGES[SELECTBAR_EDGE], 0, sx+_DBX_TITLE_RELATIVE_X, sby, 4,22 );
	DrawItemWithoutRefresh( &TAVI_IMAGES[SELECTBAR_EDGE], 3, sx+_DBX_TITLE_RELATIVE_X+width-(_DBX_TITLE_RELATIVE_X*2)-SCROLLBAR_SPACE, sby, 4,22 );

	if( ItemList[SelectedAbsoluteIndex]->optionset != NULL )
		{
		DrawItemWithoutRefresh( &TAVI_IMAGES[ICON_SMALL_LEFT_RIGHT_ARROW], 0, sx2sp+5, sby+8, 8, 7 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[ICON_SMALL_LEFT_RIGHT_ARROW], 1, sx2sp+halfOfWidth-8, sby+8, 8, 7 );
		}

}

/*
int CBookmarkDialogBox::AddBookmark(  tBookmarkNode* pNode )
{
	if( pNode->iSavetime > 0 )
		{
		BookmarkList.Add( pNode );
		LastSlot++;
		}
	
	return TRUE;
}

int CDialogBox::AddItem(int idx, char contents[] )
*/

int CSetupDialogBox::AddSetupItem( int idx, char sContents[], char sOptionsWithBar[], int iDefaultValue )
{
	char target[256];
	int iStrStartPoint, temp;
	iStrStartPoint = 0;
	tSetupDbxNode* pNode;

	DEBUGMSG(1,( "%d: %s, [%s]\n",idx, sContents, sOptionsWithBar ));
	
	pNode 					= new tSetupDbxNode;
	pNode->iCurrentItemIndex	= 0;
	pNode->iMaxItemIndex		= 0;
	pNode->ItemType			= HORIZONTAL_SELECT_BAR;
	sprintf( pNode->sName, "%s", sContents );

	for( int i=0; i < 32; i++ )
		{
		temp = GetNthString( target, sOptionsWithBar+iStrStartPoint, '|', 1 );

		if ( temp > 1 || temp == -1)
			{
			iStrStartPoint += temp;

			////////////////////////////////////////////////////
			// Add Node
			sprintf( (pNode->sOption[i]), "%s", target );
			pNode->iCurrentItemIndex = iDefaultValue;
			pNode->iMaxItemIndex = i;
			}

		if( temp == FALSE || temp == -1)
			break;
		}

	ItemList.Add( pNode );
	LastSlot++;

	return TRUE;
}


int CSetupDialogBox::AddSetupItem( int idx, U32 ResIndex, const U32 option[], int iDefaultValue, int iMaxItemIndex )
{
	tSetupDbxNode* pNode;
	SETUPOPTIONSET* optionset;
	int iItemIndex;

	optionset = new SETUPOPTIONSET;

	for( iItemIndex=0; iItemIndex< iMaxItemIndex; iItemIndex++ )
		optionset->ResIndexes[iItemIndex] 	= (U32) option[iItemIndex];

	pNode 					= new tSetupDbxNode;
	pNode->iCurrentItemIndex	= iDefaultValue;
	pNode->iMaxItemIndex		= iItemIndex-1;
	pNode->ItemType			= HORIZONTAL_SELECT_BAR;
	pNode->riName 			= ResIndex;
	pNode->optionset			= optionset;


	ItemList.Add( pNode );
	LastSlot++;

	return TRUE;
}

int CSetupDialogBox::AddSetupBarItem( int idx, U32 ResIndex, int iDefaultValue, int MaxValue )
{
	tSetupDbxNode* pNode;

	pNode 					= new tSetupDbxNode;
	pNode->iCurrentItemIndex	= iDefaultValue;
	pNode->iMaxItemIndex		= MaxValue;
	pNode->ItemType			= HORIZONTAL_SLIDER;	
	pNode->riName 			= ResIndex;
	pNode->optionset			= NULL;
	
	ItemList.Add( pNode );
	LastSlot++;

	return TRUE;
}

#define 	LIBSETUP_SLIDERLINE_COLOR 23
#define 	LIBSETUP_SLIDERLINE_SX 165

void CSetupDialogBox::CreateTextData()
{
	U16 color;
	U32 ResIndex, CurPos;
	int RelativePos;
	char timestring[10];

	DEBUGMSG(1, ("CSetupDialogBox::CreateTextData()\n"));

	LastIndex = ItemList.Count()-1;

	for( RelativeIndex=0; RelativeIndex <= MIN(_DBX_MAX_DISPLAY_INDEX-1, LastIndex); RelativeIndex++)
		{
		AbsoluteIndex = FirstShownIndex + RelativeIndex;
 		if( AbsoluteIndex == SelectedAbsoluteIndex ) color = _BLACK;
		else color = _WHITE;

		pT->DrawResText( ItemList[AbsoluteIndex]->riName, sx + _DBX_ITEM_COL_SPX, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5, color, 12, DT_TRANSPARENCY );

		if( ItemList[AbsoluteIndex]->ItemType == HORIZONTAL_SLIDER )
			{
			// Bar Type
			DEBUGMSG(1, ("-HORIZONTAL_SLIDER(%d)\n",ItemList[AbsoluteIndex]->iCurrentItemIndex));
			CurPos = ItemList[AbsoluteIndex]->iCurrentItemIndex;
			DEBUGMSG(1, ("-CurPos : %d\n", CurPos));
			DrawBoxWithoutRefresh( LIBSETUP_SLIDERLINE_COLOR, LIBSETUP_SLIDERLINE_SX, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 10, 100, 2 );

			if( ItemList[AbsoluteIndex]->riName == ML_VS_BRIGHTNESS
				|| ItemList[AbsoluteIndex]->riName == ML_VS_CONTRAST
				|| ItemList[AbsoluteIndex]->riName == ML_VS_SATURATION )
				{

				#if 0
				RelativePos = (int) (CurPos * 323)/100;

				if( RelativePos > 99 ) 	sprintf( timestring, "%d", 99 );
				else					sprintf( timestring, "%d", RelativePos );
				#else
				RelativePos = (int) (CurPos*100/60);
//				RelativePos = (int) (CurPos * 323)/100;
				sprintf( timestring, "%d", CurPos );
				#endif

				DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[SETUP_SLIDER], 1, LIBSETUP_SLIDERLINE_SX+RelativePos-2, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 6, 8, 10 );
				pT->DrawEUC8Text( timestring, LIBSETUP_SLIDERLINE_SX + 102 + 2, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5, color, 12, DT_TRANSPARENCY );
				}

			else if( ItemList[AbsoluteIndex]->riName == ML_SURROUND
				|| ItemList[AbsoluteIndex]->riName == ML_BASS )
				{
				RelativePos = (int) (CurPos*100/6);
				sprintf( timestring, "%d", CurPos );
				DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[SETUP_SLIDER], 1, LIBSETUP_SLIDERLINE_SX+RelativePos-2, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 6, 8, 10 );
				pT->DrawEUC8Text( timestring, LIBSETUP_SLIDERLINE_SX + 102 + 2, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5, color, 12, DT_TRANSPARENCY );
				}

			else if( ItemList[AbsoluteIndex]->riName > ML_SOUND_PALETTE_USER_EQ_B125-1 && ItemList[AbsoluteIndex]->riName < ML_SOUND_PALETTE_USER_EQ_B8K+1 )
				{
				int EQValue = (int) CurPos;
				DEBUGMSG(1, ("-5 Band EQ : riName=%d\n", ItemList[AbsoluteIndex]->riName));
				RelativePos = (int) (CurPos*100/18);
				DEBUGMSG(1, ("-RelativePos : %d\n", RelativePos ));
				sprintf( timestring, "%d", EQValue-9 );
				DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[SETUP_SLIDER], 1, LIBSETUP_SLIDERLINE_SX+RelativePos-2, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 6, 8, 10 );
				pT->DrawEUC8Text( timestring, LIBSETUP_SLIDERLINE_SX + 102 + 2, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5, color, 12, DT_TRANSPARENCY );
				}

			else
				{
				sprintf( timestring, "%d", CurPos );
				DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[SETUP_SLIDER], 1, LIBSETUP_SLIDERLINE_SX+CurPos-2, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 6, 8, 10 );
				pT->DrawEUC8Text( timestring, LIBSETUP_SLIDERLINE_SX + 102 + 2, sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5, color, 12, DT_TRANSPARENCY );
				}

			}

		else
			{
			ResIndex = ItemList[AbsoluteIndex]->optionset->ResIndexes[ ItemList[AbsoluteIndex]->iCurrentItemIndex ];
			pT->DrawResText( ResIndex, sx2sp + ((halfOfWidth-3)/2 - (pT->GetStringWidth( ResIndex, 12, DT_TRANSPARENCY))/2), sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 5, color, 12, DT_TRANSPARENCY );
			}

		}

}


int CSetupDialogBox::GetCurrentChoice()
{
	printf("CSetupDialogBox::GetCurrentChoice()\n-choice:0x%x\n", choice);
	return choice;
}


int CSetupDialogBox::GetCurrentItemIndex()
{
	return ( ItemList[ choice ]->iCurrentItemIndex ) ;
}

int CSetupDialogBox::GetSetupStatus( int ChoiceIndex )
{
	return ( ItemList[ ChoiceIndex ]->iCurrentItemIndex ) ;
}


// pDbx1->AddItem(0, "After Playing", "File Browser|Repeat|Next File"  );


int CSetupDialogBox::CursorUp()
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

	choice = SelectedAbsoluteIndex;

	return choice;
}


int CSetupDialogBox::CursorDown()
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
	

	choice = SelectedAbsoluteIndex ;

	return choice;
	

}

int CSetupDialogBox::CursorLeft()
{
	DEBUGMSG(1, ("CSetupDialogBox::CursorLeft()\n"));

	if( ItemList[choice]->ItemType == HORIZONTAL_SLIDER )
		{
		
 		if( ItemList[choice]->iCurrentItemIndex > 1 )
			ItemList[choice]->iCurrentItemIndex--;
		else
			ItemList[choice]->iCurrentItemIndex = 0;

		switch( ItemList[choice]->riName )
			{
			case ML_VS_BRIGHTNESS:
 				pMpegDecoder->Brightness(-1, &((RMint32)ItemList[choice]->iCurrentItemIndex)  );
				TaviSysParam.video.brightness = ItemList[choice]->iCurrentItemIndex;
				break;

			case ML_VS_CONTRAST:
 				//pMpegDecoder->Saturation(-1, &((RMint32)ItemList[choice]->iCurrentItemIndex)  );
 				pMpegDecoder->Contrast(-1, &((RMint32)ItemList[choice]->iCurrentItemIndex)  );
				TaviSysParam.video.contrast = ItemList[choice]->iCurrentItemIndex;
				break;

			case ML_VS_SATURATION:
 				pMpegDecoder->Saturation(-1, &((RMint32)ItemList[choice]->iCurrentItemIndex)  );
				TaviSysParam.video.saturation= ItemList[choice]->iCurrentItemIndex;
				break;
			}

		DEBUGMSG(1, ("-HORIZONTAL_SLIDER(%d)\n",ItemList[choice]->iCurrentItemIndex));
		
		}
	else
		{

		if( ItemList[choice]->iCurrentItemIndex > 0 )
			ItemList[choice]->iCurrentItemIndex--;
		else
			ItemList[choice]->iCurrentItemIndex = ItemList[choice]->iMaxItemIndex;

		if( choice == 2 )
			TaviSysParam.video.playlist_repeat = ItemList[2]->iCurrentItemIndex;

		if( UpdateSetting )
			UpdateSetting(NULL);
		}

	return ItemList[choice]->iCurrentItemIndex;
}

int CSetupDialogBox::CursorRight()
{
	DEBUGMSG(1, ("CSetupDialogBox::CursorRight()\n"));

	if( ItemList[choice]->ItemType == HORIZONTAL_SLIDER )
		{
		DEBUGMSG(1, ("-HORIZONTAL_SLIDER(%d)\n",ItemList[choice]->iCurrentItemIndex));

		printf("CursorRight()\n");
		printf("-choice : %d\n", choice);
		printf("-ItemList[choice]->iCurrentItemIndex : %d\n", ItemList[choice]->iCurrentItemIndex );
		printf("-ItemList[choice]->iMaxItemIndex : %d\n", ItemList[choice]->iMaxItemIndex );
		
		// It means Bar Type
		if( ItemList[choice]->iCurrentItemIndex  < ItemList[choice]->iMaxItemIndex )
			{
			ItemList[choice]->iCurrentItemIndex++;

			switch( ItemList[choice]->riName )
				{
				case ML_VS_BRIGHTNESS:
					pMpegDecoder->Brightness( 1, &((RMint32)ItemList[choice]->iCurrentItemIndex)  );
					TaviSysParam.video.brightness = ItemList[choice]->iCurrentItemIndex;
					if( UpdateSetting ) UpdateSetting(NULL);
					break;
				case ML_VS_CONTRAST:
					pMpegDecoder->Contrast( 1, &((RMint32)ItemList[choice]->iCurrentItemIndex)  );
					TaviSysParam.video.contrast = ItemList[choice]->iCurrentItemIndex;
					if( UpdateSetting ) UpdateSetting(NULL);
					break;
				case ML_VS_SATURATION:
	 				pMpegDecoder->Saturation(1, &((RMint32)ItemList[choice]->iCurrentItemIndex)  );
					TaviSysParam.video.saturation = ItemList[choice]->iCurrentItemIndex;
					if( UpdateSetting ) UpdateSetting(NULL);
					break;

				}
			}
		
		else
			{
			ItemList[choice]->iCurrentItemIndex = ItemList[choice]->iMaxItemIndex;
			}


		}
	else
		{
		if( ItemList[choice]->iCurrentItemIndex < ItemList[choice]->iMaxItemIndex )
			ItemList[choice]->iCurrentItemIndex++;
		else
			ItemList[choice]->iCurrentItemIndex = 0;

		if( choice == 2 )
			TaviSysParam.video.playlist_repeat = ItemList[2]->iCurrentItemIndex;

		if( UpdateSetting )
			UpdateSetting(NULL);
		}


	
	return ItemList[choice]->iCurrentItemIndex;
}

void CSetupDialogBox::DrawScrollBar()
{
	if( LastIndex > _DBX_MAX_DISPLAY_SLOT )
		{
		if ( pSb1 == NULL )
			{
			pSb1 = new CScrollBar2( 
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
}


void CSetupDialogBox::Prepare_5BandEQ()
{
	bevBand0 = TaviSysParam.sound.band0;
	bevBand1 = TaviSysParam.sound.band1;
	bevBand2 = TaviSysParam.sound.band2;
	bevBand3 = TaviSysParam.sound.band3;
	bevBand4 = TaviSysParam.sound.band4;
}
void CSetupDialogBox::Prepare_2BandEQ( int type )
{
	switch( type )
		{
		case ID_EQ_MEX_2BAND_SLIDER:
			bevSurround = TaviSysParam.sound.surround_mex;
			bevBass = TaviSysParam.sound.bass_mex;
			pSurround = &(TaviSysParam.sound.surround_mex);
			pBass = &(TaviSysParam.sound.bass_mex);
			break;

		case ID_EQ_REXWIDE_2BAND_SLIDER:
			bevSurround = TaviSysParam.sound.surround_rex_wide;
			bevBass = TaviSysParam.sound.bass_rex_wide;
			pSurround = &(TaviSysParam.sound.surround_rex_wide);
			pBass = &(TaviSysParam.sound.bass_rex_wide);
			break;

		case ID_EQ_REXLIVE_2BAND_SLIDER:
			bevSurround = TaviSysParam.sound.surround_rex_live;
			bevBass = TaviSysParam.sound.bass_rex_live;
			pSurround = &(TaviSysParam.sound.surround_rex_live);
			pBass = &(TaviSysParam.sound.bass_rex_live);
			break;

		default:
			ASSERT(0);
			return;
		}


}
void CSetupDialogBox::Prepare_BrightnessAndContrast()
{
	PreVideoBrightness	 = TaviSysParam.video.brightness;
	PreVideoContrast = TaviSysParam.video.contrast;
	PreVideoSaturation = TaviSysParam.video.saturation;
}


/*
ML_SOUND_PALETTE_USER_EQ_B125	125Hz
ML_SOUND_PALETTE_USER_EQ_B250	250Hz
ML_SOUND_PALETTE_USER_EQ_B1K	1KHz
ML_SOUND_PALETTE_USER_EQ_B4K	4KHz
ML_SOUND_PALETTE_USER_EQ_B8K	8KHz
*/
void CSetupDialogBox::Process_5BandEQ()
{
	DEBUGMSG(1, ("CSetupDialogBox::Process_5BandEQ()\n"));
	DEBUGMSG(1, ("-ItemList.Count():%d\n", ItemList.Count() ));

/*
	TaviSysParam.sound.band0 = (char) ItemList[0]->iCurrentItemIndex;
	DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B125(%d)\n",TaviSysParam.sound.band0));

	TaviSysParam.sound.band1 = (char) ItemList[1]->iCurrentItemIndex;
	DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B250(%d)\n",TaviSysParam.sound.band1));

	TaviSysParam.sound.band2 = (char) ItemList[2]->iCurrentItemIndex;
	DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B1K(%d)\n",TaviSysParam.sound.band2));

	TaviSysParam.sound.band3 = (char) ItemList[3]->iCurrentItemIndex;
	DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B4K(%d)\n",TaviSysParam.sound.band3));

	TaviSysParam.sound.band4 = (char) ItemList[4]->iCurrentItemIndex;
	DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B8K(%d)\n",TaviSysParam.sound.band4));
*/


	for(int it=0; it<ItemList.Count(); it++)
		{
		switch( ItemList[it]->riName )
			{
			case ML_SOUND_PALETTE_USER_EQ_B125:
				TaviSysParam.sound.band0 = ItemList[it]->iCurrentItemIndex;
				DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B125(%d)\n",TaviSysParam.sound.band0));
				break;

			case ML_SOUND_PALETTE_USER_EQ_B250:
				TaviSysParam.sound.band1 = ItemList[it]->iCurrentItemIndex;
				DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B250(%d)\n",TaviSysParam.sound.band1));
				break;

			case ML_SOUND_PALETTE_USER_EQ_B1K:
				TaviSysParam.sound.band2 = ItemList[it]->iCurrentItemIndex;
				DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B1K(%d)\n",TaviSysParam.sound.band2));
				break;

			case ML_SOUND_PALETTE_USER_EQ_B4K:
				TaviSysParam.sound.band3 = ItemList[it]->iCurrentItemIndex;
				DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B4K(%d)\n",TaviSysParam.sound.band3));
				break;

			case ML_SOUND_PALETTE_USER_EQ_B8K:
				TaviSysParam.sound.band4 = ItemList[it]->iCurrentItemIndex;
				DEBUGMSG(1, ("-ML_SOUND_PALETTE_USER_EQ_B8K(%d)\n",TaviSysParam.sound.band4));
				break;

			default:
				ASSERT(0);
				return;

			}
		}
}
void CSetupDialogBox::Process_2BandEQ()
{
	int iSurround;
	int iBass;

	for(int it=0; it<ItemList.Count(); it++)
		{
		if( ItemList[it]->riName == ML_SURROUND )
			{
			DEBUGMSG(1, ("CSetupDialogBox::Process_2BandEQ()\n-ItemList[it]->iCurrentItemIndex:%d\n",ItemList[it]->iCurrentItemIndex ));
			*(pSurround) = (int) ItemList[it]->iCurrentItemIndex;
			DEBUGMSG(1, ("-pSurround:%d\n", *(pSurround) ));
			DEBUGMSG(1, ("-TaviSysParam.sound.surround_mex:%d\n", TaviSysParam.sound.surround_mex ));
			DEBUGMSG(1, ("-TaviSysParam.sound.surround_rex_wide:%d\n", TaviSysParam.sound.surround_rex_wide ));
			DEBUGMSG(1, ("-TaviSysParam.sound.surround_rex_live:%d\n", TaviSysParam.sound.surround_rex_live ));
			}
		else if( ItemList[it]->riName == ML_BASS )
			{
			DEBUGMSG(1, ("CSetupDialogBox::Process_2BandEQ()\n-ItemList[it]->iCurrentItemIndex:%d\n",ItemList[it]->iCurrentItemIndex ));
			*(pBass)= (int) ItemList[it]->iCurrentItemIndex;
			DEBUGMSG(1, ("-pBass:%d\n", *(pBass) ));
			DEBUGMSG(1, ("-TaviSysParam.sound.bass_mex:%d\n", TaviSysParam.sound.bass_mex ));
			DEBUGMSG(1, ("-TaviSysParam.sound.bass_rex_wide:%d\n", TaviSysParam.sound.bass_rex_wide ));
			DEBUGMSG(1, ("-TaviSysParam.sound.bass_rex_live:%d\n", TaviSysParam.sound.bass_rex_live ));
			}
		}
}
void CSetupDialogBox::Process_BrightnessAndContrast()
{
	for(int it=0; it<ItemList.Count(); it++)
		{
		if( ItemList[it]->riName == ML_VS_BRIGHTNESS )
			pBrightnessValue = &((RMint32)ItemList[it]->iCurrentItemIndex) ;
		else if( ItemList[it]->riName == ML_VS_CONTRAST )
			pContrastValue = &((RMint32)ItemList[it]->iCurrentItemIndex) ;
		else if( ItemList[it]->riName == ML_VS_SATURATION)
			pSaturationValue = &((RMint32)ItemList[it]->iCurrentItemIndex) ;
		}
}


void CSetupDialogBox::Cancel_5BandEQ()
{
	TaviSysParam.sound.band0 = bevBand0 ;
	TaviSysParam.sound.band1 = bevBand1 ;
	TaviSysParam.sound.band2 = bevBand2 ;
	TaviSysParam.sound.band3 = bevBand3 ;
	TaviSysParam.sound.band4 = bevBand4 ;
 }
void CSetupDialogBox::Cancel_2BandEQ( int type )
{
	switch( type )
		{
		case ID_EQ_MEX_2BAND_SLIDER:
			TaviSysParam.sound.surround_mex = bevSurround;
			TaviSysParam.sound.bass_mex = bevBass;
			break;

		case ID_EQ_REXWIDE_2BAND_SLIDER:
			TaviSysParam.sound.surround_rex_wide = bevSurround;
			TaviSysParam.sound.bass_rex_wide = bevBass;
			break;

		case ID_EQ_REXLIVE_2BAND_SLIDER:
			TaviSysParam.sound.surround_rex_live = bevSurround;
			TaviSysParam.sound.bass_rex_live = bevBass;
			break;

		default:
			ASSERT(0);
			return;
		}
}
void CSetupDialogBox::Cancel_BrightnessAndContrast()
{
	RMint32 CurrentBrightness, CurrentContrast, CurrentSaturation;
	RMint32 b1 = (RMint32)TaviSysParam.video.brightness;
	RMint32 c1 = (RMint32)TaviSysParam.video.contrast;
	RMint32 s1 = (RMint32)TaviSysParam.video.saturation;
	RMint32 b0 = (RMint32)PreVideoBrightness;
	RMint32 c0 = (RMint32)PreVideoContrast;
	RMint32 s0 = (RMint32)PreVideoSaturation;
	RMint32 b, c;

	DEBUGMSG(1,("\n\n-PreVideoBrightness:%d(%d)\n-TaviSysParam.video.brightness:%d(%d)\n", b0, PreVideoBrightness, b1, TaviSysParam.video.brightness ));
	DEBUGMSG(1,("-PreVideoContrast:%d(%d)\n-TaviSysParam.video.contrast:%d(%d)\n\n", c0, PreVideoContrast, c1, TaviSysParam.video.contrast ));

	pMpegDecoder->SetBrightness( (b0-b1), &CurrentBrightness );
	pMpegDecoder->SetContrast( (c0-c1), &CurrentContrast);
	pMpegDecoder->SetSaturation( (s0-s1), &CurrentSaturation);

	TaviSysParam.video.brightness = PreVideoBrightness;
	TaviSysParam.video.contrast = PreVideoContrast;
	TaviSysParam.video.saturation = PreVideoSaturation;

	*(pBrightnessValue) = (RMint32)TaviSysParam.video.brightness;
	*(pContrastValue) = (RMint32)TaviSysParam.video.contrast;
	*(pSaturationValue) = (RMint32)TaviSysParam.video.saturation;

	DEBUGMSG(1,("-CurrentBrightness:%d\n-CurrentContrast:%d\n\n", CurrentBrightness, CurrentContrast ));
}


void CSetupDialogBox::Cancel()
{
	if( OptionCheck(BtnOption,ID_EQ_5BAND_SLIDER) )
		Cancel_5BandEQ();
	
	else if( OptionCheck(BtnOption,ID_EQ_MEX_2BAND_SLIDER) )
		Cancel_2BandEQ(ID_EQ_MEX_2BAND_SLIDER);
	
	else if( OptionCheck(BtnOption,ID_EQ_REXWIDE_2BAND_SLIDER) )
		Cancel_2BandEQ(ID_EQ_REXWIDE_2BAND_SLIDER);
	
	else if( OptionCheck(BtnOption,ID_EQ_REXLIVE_2BAND_SLIDER) )
		Cancel_2BandEQ(ID_EQ_REXLIVE_2BAND_SLIDER);
	
	else
		Cancel_BrightnessAndContrast();
}

int  CSetupDialogBox::isVisible() { 	return iVisibleStatus; }

void CSetupDialogBox::ReDraw()
{
	DEBUGMSG(1, ("CSetupDialogBox::ReDraw()\n"));

	SetTitle( TitleRes );
	ClearDbxContents();
	CreateSelectBar();

	if( OptionCheck(BtnOption,ID_EQ_5BAND_SLIDER) )
		Process_5BandEQ();
	
	else if( OptionCheck(BtnOption,ID_EQ_MEX_2BAND_SLIDER) )
		Process_2BandEQ();

	else if( OptionCheck(BtnOption,ID_EQ_REXWIDE_2BAND_SLIDER) )
		Process_2BandEQ();

	else if( OptionCheck(BtnOption,ID_EQ_REXLIVE_2BAND_SLIDER) )
		Process_2BandEQ();

	CreateTextData();

	DrawScrollBar();
	BuildButtons( BtnOption );
	iVisibleStatus= TRUE;
	pMpegDecoder->RefreshOsd();
}

int CSetupDialogBox::Show()
{
	CreateDbxBackground();
//	SetTitle( title );
	SetTitle( TitleRes );
	CreateSelectBar();


	if( OptionCheck(BtnOption,ID_EQ_5BAND_SLIDER) )
		Prepare_5BandEQ();
	
	else if( OptionCheck(BtnOption,ID_EQ_MEX_2BAND_SLIDER) )
		Prepare_2BandEQ(ID_EQ_MEX_2BAND_SLIDER);

	else if( OptionCheck(BtnOption,ID_EQ_REXWIDE_2BAND_SLIDER) )
		Prepare_2BandEQ(ID_EQ_REXWIDE_2BAND_SLIDER);

	else if( OptionCheck(BtnOption,ID_EQ_REXLIVE_2BAND_SLIDER) )
		Prepare_2BandEQ(ID_EQ_REXLIVE_2BAND_SLIDER);

	else
		{
		Prepare_BrightnessAndContrast();
		Process_BrightnessAndContrast();
		}

	CreateTextData();

	DrawScrollBar();
	BuildButtons( BtnOption );
	pMpegDecoder->RefreshOsd();
/*
	RMint32 w, h, n, d, x, y, w1, h1;
	pMpegDecoder->GetScreenDimensions (&w, &h, &n, &d);
	DEBUGMSG(1, ("GetScreenDimensions: w[%d] h[%d] n[%d] d[%d]\n", w, h, n, d ));
*/
	iVisibleStatus= TRUE;
	return iVisibleStatus;
}

int CSetupDialogBox::Hide()
{
	pMpegDecoder->RenderText(0, 0, sx, sy, width, height );
	iVisibleStatus = FALSE;
	return iVisibleStatus;
}



