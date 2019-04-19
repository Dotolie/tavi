//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CLibSubtitleSyncDbx Class Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSubtitleSyncDbx.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSubtitleSyncDbx.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.4  2005/09/16 01:11:18  etnlwind
// Update for FileManager USB Host Related
//
// Revision 1.3  2005/07/29 06:52:51  etnlwind
// Update for MACRONAME changes
//
// Revision 1.2  2005/07/16 06:15:26  etnlwind
// Update for LibSubtitleSyncDbx
//
// Revision 1.1  2005/07/15 09:47:39  etnlwind
// Add for LibuSubtitleSyncDbx
//
//
//*****************************************************************************


#include "LibSubtitleSyncDbx.h"
#include "LibDefines.h"

extern CTextEngine_BDF* pT;
extern CSubtitle* pSubtitle;

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// CLibSubtitleSyncDbx Class Figure
//
//
#define GRID_WIDTH 20
#define GRID_TOTAL_WIDTH (GRID_WIDTH*12+1)
#define GRID_HALF_WIDTH (GRID_TOTAL_WIDTH/2)
#define GRID_BASE_HEIGHT 4
#define GRID_BIG_HEIGHT 7
#define GRID_SMALL_HEIGHT 3
#define GRID_TOTAL_HEIGHT (GRID_BASE_HEIGHT+GRID_BIG_HEIGHT)
#define DIFF_GRID_BIG_AND_SMALL (GRID_BIG_HEIGHT-GRID_SMALL_HEIGHT)
#define CURSOR_CENTER_FROM_LEFT_EDGE 5
#define CURSOR_WIDTH 11
#define CURSOR_HEIGHT 6

RMuint8 BottomDirectCursor[CURSOR_WIDTH*CURSOR_HEIGHT] = { 	
	101, 101, 101, 101, 101, 101,101, 101, 101, 101, 101,
	0, 101, 101, 101, 101, 101, 101,101, 101, 101, 0,
	0, 0, 101, 101, 101, 101, 101,101, 101, 0, 0,
	0, 0, 0, 101, 101, 101, 101,101, 0, 0, 0,
	0, 0, 0, 0, 101, 101, 101, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0 
	};

CLibSubtitleSyncDbx::CLibSubtitleSyncDbx( U32 DbxTitleRes, int x, int y, int w, int h, int SubtitleShiftSeconds, U32 option )
{
	DEBUGMSG(1,("CLibSubtitleSyncDbx: %d\n-x: %d\n-y: %d\n-w: %d\n-h: %d\n-SubtitleShiftSeconds: %d\n-option: 0x%x\n", DbxTitleRes, x, y, w, h, SubtitleShiftSeconds,option ));

	sx = x;
	sy = y;
	width = w;
	height = h;

	gsx = sx+(width/2-GRID_TOTAL_WIDTH/2);
	gsy = sy+60;

	if( SubtitleShiftSeconds>60 || SubtitleShiftSeconds<-60 )
		{
		m_SubtitleShiftSeconds = 0;
		m_Pre_SyncSecShiftValue = 0;
		}
	else
		{
		m_SubtitleShiftSeconds = SubtitleShiftSeconds;
		m_Pre_SyncSecShiftValue = SubtitleShiftSeconds;
		}

	pCursor = BottomDirectCursor;

	BtnOption = option;
	CreateDbxBackground();
	SetTitle( DbxTitleRes );
	
	isVisible = FALSE;
	
}

CLibSubtitleSyncDbx::~CLibSubtitleSyncDbx()
{
}

void CLibSubtitleSyncDbx::Show()
{
	Redraw();
	//pMpegDecoder->RefreshOsd();
}

void CLibSubtitleSyncDbx::Hide()
{
	
}


void CLibSubtitleSyncDbx::Redraw()
{
	ClearDbxContents();
	DrawGrid();
	DrawCursor();
	DrawTextData();

	if( BtnOption )
		BuildButtons( BtnOption );

	pMpegDecoder->RefreshOsd();
	isVisible = TRUE;
}

void CLibSubtitleSyncDbx::DrawGrid()
{
	DrawBoxWithoutRefresh( _WHITE, gsx, gsy+GRID_BIG_HEIGHT, GRID_TOTAL_WIDTH, GRID_BASE_HEIGHT );

	for( int i=0; i<=12; i++ )
		{
		if( i==0 ||i==6 ||i==12 )
			DrawBoxWithoutRefresh( _WHITE, gsx+(i*GRID_WIDTH), gsy, 1, GRID_BIG_HEIGHT );
		else
			DrawBoxWithoutRefresh( _WHITE, gsx+(i*GRID_WIDTH), (gsy+DIFF_GRID_BIG_AND_SMALL), 1, GRID_SMALL_HEIGHT );
		}
}

void CLibSubtitleSyncDbx::DrawCursor()
{
	cx = (gsx + GRID_HALF_WIDTH - CURSOR_CENTER_FROM_LEFT_EDGE) + (m_SubtitleShiftSeconds*2);
	cy = gsy - 8;

	pMpegDecoder->RenderOsdBits_WithoutRefresh_SkipTransparency( pCursor, cx, cy, CURSOR_WIDTH, CURSOR_HEIGHT );
}

void CLibSubtitleSyncDbx::ClearTimeStringAndCursor( )
{
	DrawBoxWithoutRefresh( _DBX_BASE_COLOR_PALLETTE, sx, cy-14, width, CURSOR_HEIGHT+14 );
}

void CLibSubtitleSyncDbx::RefreshTextData()
{
	int tsx, usw;
	char ctemp[12];
	uchar_t uctemp[12];
	
	if( m_SubtitleShiftSeconds > 0 )
		sprintf( ctemp, "<+00:%02d>", abs(m_SubtitleShiftSeconds) );
	else if( m_SubtitleShiftSeconds < 0 )
		sprintf( ctemp, "<-00:%02d>", abs(m_SubtitleShiftSeconds) );
	else		//( m_SubtitleShiftSeconds == 0 )
		sprintf( ctemp, "<00:%02d>", abs(m_SubtitleShiftSeconds) );

	str2ustr( uctemp, ctemp, strlen(ctemp) );
	usw = pT->GetStringWidth(uctemp, 12, DT_TRANSPARENCY);
	tsx = (cx+CURSOR_CENTER_FROM_LEFT_EDGE) - usw/2;

	if(tsx < sx)
		tsx = sx;
	else if( tsx+usw > sx+width )
		tsx = sx+width-usw;

	pT->DrawUC16Text( uctemp, tsx, cy-14, _WHITE, 12, DT_TRANSPARENCY );
}

void CLibSubtitleSyncDbx::DrawTextData()
{
	char ctemp[12];
	uchar_t uctemp[12];

	sprintf( ctemp, "%s", "-60" );
	str2ustr( uctemp, ctemp, strlen(ctemp) );
	ustrcat( uctemp, pT->GetTextRes(ML_SEC));
	pT->DrawUC16Text( uctemp, gsx - pT->GetStringWidth(uctemp, 12, DT_TRANSPARENCY)/2, gsy+GRID_TOTAL_HEIGHT+2, _WHITE, 12, DT_TRANSPARENCY );

	sprintf( ctemp, "%s", "+60" );
	str2ustr( uctemp, ctemp, strlen(ctemp) );
	ustrcat( uctemp, pT->GetTextRes(ML_SEC));
	pT->DrawUC16Text( uctemp, (gsx+GRID_TOTAL_WIDTH) - pT->GetStringWidth(uctemp, 12, DT_TRANSPARENCY)/2, gsy+GRID_TOTAL_HEIGHT+2, _WHITE, 12, DT_TRANSPARENCY );
	
	RefreshTextData();
	
}

void CLibSubtitleSyncDbx::ResyncSMI()
{
	pMpegDecoder->GetSTC( &PlaytimeMSec, 1000 );

	if( PlaytimeMSec == 0 )
		pSubtitle->ChunkToHead();
	else
		pSubtitle->ChunkToAdjacentCaption( PlaytimeMSec );
}


void CLibSubtitleSyncDbx::CursorLeft()
{
	
	
	if( m_SubtitleShiftSeconds > -60 )
		{
		ClearTimeStringAndCursor();
		pSubtitle->SetSyncSecShiftValue( --m_SubtitleShiftSeconds );
		DrawCursor();
		RefreshTextData();
		ResyncSMI();
		pMpegDecoder->RefreshOsd();
		}
}

void CLibSubtitleSyncDbx::CursorRight()
{
	if( m_SubtitleShiftSeconds < 60 )
		{
		ClearTimeStringAndCursor();
		pSubtitle->SetSyncSecShiftValue( ++m_SubtitleShiftSeconds );
		DrawCursor();
		RefreshTextData();
		ResyncSMI();
		pMpegDecoder->RefreshOsd();
		}
}


int CLibSubtitleSyncDbx::MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{

	DEBUGMSG(1,("CLibSubtitleSyncDbx::MsgProc( %d, %d, %d, %d )\n", ssTaskId, ulMsgId, ssParam1, ssParam2 ));

	//////////////////////////////////////////////////////////////////////
	//	MsgBtnUp
	//	Key Message Process
	
	if( ulMsgId == MSG_BUTTON_UP )
		{

		switch( ssParam1 )
			{
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_UP:
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_DOWN:
				break;
				
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LEFT:
			case TAVI_KEY_LLEFT:
				CursorLeft();
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_RIGHT:
			case TAVI_KEY_LRIGHT:
				CursorRight();
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_ENTER:
				return -1;
				break;
	
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_MENU:
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_EXIT:
				pSubtitle->SetSyncSecShiftValue(m_Pre_SyncSecShiftValue);
				ResyncSMI();
				choice = TAVI_KEY_EXIT_REVERSE;
				return -1;
				break;
				
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LEXIT:				
			case TAVI_KEY_MODE:
			case TAVI_KEY_LMODE:
				return -1;
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_SHUTDOWN:
				return -1;
				break;
			}
		}


	//////////////////////////////////////////////////////////////////////
	//	MsgBtnUp
	//	Touch Pad Message Process

	else if( ulMsgId == MSG_TOUCHPAD )
		{

		if( ssParam1 == -1 )
			{
			CursorLeft();
			}
		else if( ssParam1 == 1 )
			{
			CursorRight();
			}
		else if( ssParam2 == 1 )
			{
			//CursorRight();
			return -1;
			}
		}

	return 0;
}



#if 0

void CFileDbx::CreateSelectBar()
{
	if( ItemList.Count() == 0 )
		{
			DrawASCIITextWithoutRefresh(
				"No Entry Exists.",
				16, 
				15, 
				&TAVI_IMAGES[GULIM_13_WHITE_NORMAL_BORDER],
				sx + _DBX_ITEM_COL_SPX,
				sy+ _DBX_SELECTBAR_START_Y + 3,
				__SKIP_TRANSPARENCY_COLOR
				);
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
void CFileDbx::CreateTextData()
{
	U16 color;
	U32 option;
	int iFont;

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

		pT->DrawUC16Text( 
			ItemList[i]->uname, 
			sx + _DBX_ITEM_COL_SPX, 
			sy+ _DBX_SELECTBAR_START_Y + ( i*_DBX_SELECTBAR_HEIGHT ) + 3 + 3,
			color,
			12,
			option
			);
		}
}
*/

void CFileDbx::CreateTextData()
{
	U16	color;
	U32 option;

	uchar_t ucTemp[64];
	int X_Limited = sx+_DBX_SELECTBAR_START_X+(width-14-SCROLLBAR_SPACE) - 8;




	// Bookmark 0 
	// Bookmark 1
	// Bookmark 2
	// Bookmark 3
	// Bookmark 4

	LastIndex = ItemList.Count()-1;
	RelativeIndex = 0;

	DEBUGMSG(1,(" T RelativeIndex AbsoluteIndex FirstShownIndex SelectedAbsoluteIndex LastIndex choice\n"));
	DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", 'S', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));
	DEBUGMSG(1,("+-+-------------+-------------+---------------+---------------------+---------+------+\n"));

	for( ; RelativeIndex <= MIN(_DBX_MAX_DISPLAY_INDEX-1, LastIndex); RelativeIndex++)
		{

		AbsoluteIndex = FirstShownIndex + RelativeIndex;

 		if( AbsoluteIndex == SelectedAbsoluteIndex )
			{
			color			= _BLACK;
			option			= (DT_TRANSPARENCY);
			}
		else
			{
			color			= _WHITE;
			option			= (DT_TRANSPARENCY);
			}		
/*
		// Icon
		DrawItemWithoutRefreshSkipTransparency(
			&TAVI_IMAGES[TYPE_ICON_LIST],
			BookmarkPID,
			sx + _DBX_ITEM_COL_SPX, 
			sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex *_DBX_SELECTBAR_HEIGHT ) + 4,
			12, 12
			);
*/

		pT->DrawUC16Text( 
			(uchar_t*)ItemList[AbsoluteIndex]->D.name, //ItemList[AbsoluteIndex]->uname, 
			sx + _DBX_ITEM_COL_SPX,		 //sx + _DBX_ITEM_COL_SPX + 14,
			sy+ _DBX_SELECTBAR_START_Y + ( RelativeIndex*_DBX_SELECTBAR_HEIGHT ) + 5,
			color,
			_SHADOW,
			12,
			X_Limited, 
			option
			);
	

		if( color == _BLACK )
			DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", '>', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));
		else
			DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", ' ', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));

		
		}

	DEBUGMSG(1,("+-+-------------+-------------+---------------+---------------------+---------+------+\n"));


}



uchar_t* CFileDbx::GetFilenamePtr()
{
	//return( ItemList[choice]->uname );
	return( (uchar_t*)ItemList[choice]->D.name );
}

int CFileDbx::AddFileItem_UC16_WithSorting( int idx, uchar_t name[], int filesize )
{
	//tFileBrowserItemNode* pNode;
	FILEDBX_NODE* pNode = new FILEDBX_NODE;

	ustrcpy( (uchar_t*)pNode->D.name, name );
	pNode->D.file_size			= filesize;
	//pNode->filetype			= filetype;
	sprintf( pNode->ext, "%s", m_ext );

	return ( ItemList.AddWithSort( pNode, fpNameAscentSort ) );

/*
	if( filetype == _DIRECTORY )
		{
		return ( ItemList.AddWithSort( pNode, fpNameAscentSort ) );
		}
	else
		{
		return ( ItemList_Files.AddWithSort( pNode, fpNameAscentSort ) );
		}
*/
	
}


int CFileDbx::ScanDir( uchar_t targetdir[] )
{
	FATFS_ERROR err;
	FATFS_DIR_ENTRY_INFO dirinfo;

	int pickup = 0;
	RMuint32 dd;
	
	char cTemp[8192];
//	uchar_t ucTemp[8192];
	
	DEBUGMSG(1, ("CFileDbx::ScanDir()\n"));

	if ( (err=fatfs_wdopen ((RMuint16*)targetdir, &dd)) != FATFS_ERROR_NO_ERROR)
		{
		ASSERT(0);
		printf("- fatfs error: [%x] : [%s]\n", (int)err, GetErrMsg(cTemp, (int)err) );
		}
	
	for(;;)
	{
		err = fatfs_dread (dd, &dirinfo);
		if (err != FATFS_ERROR_NO_ERROR)
			{
			break;
			}

		if( dirinfo.name[0] == 0x002e ) 
			continue;

		if( CheckVideoExtension_UC16( (uchar_t*)dirinfo.name ) == 31 )
			{
			print_ustr((uchar_t*)dirinfo.name);
			AddFileItem_UC16_WithSorting( pickup, (uchar_t*)dirinfo.name, dirinfo.file_size );
			pickup++;
			}
	}

	fatfs_dseek( dd, 0 );
	fatfs_dclose( dd );
	return TRUE;

ERROR__FATFS_ERROR:
	printf ("[!] ERROR__FATFS_ERROR: 0x%x\n", (int)err);

	fatfs_dclose( dd );
	return FALSE;
}



	
#if 0
CFileDbx::CFileDbx( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option  )
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

/*
CDialogBox::CDialogBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option )
{
	DEBUGMSG(1,("CDialogBox( [%d], %d, %d, %d, %d)\n", ResIndex, x, y, w, h ));
	Stop_AppTimer( pTAVI->iVideoTimerId, pCp->pTimer );
	memset( title, 0, 256 );
	memset( item, 0, 8*256 );
	choice = -1;
	sx = x;	sy = y;	width = w;	height = h;	DialogType = 0;
	fpDialogBoxMsgProc = pfpMsgProc;
	CreateDbxBackground();
	SetTitle( ResIndex );
	BtnOption = option;
}
*/
CFileDbx::CFileDbx( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 ulPlaytime, U32 option  )
{
	DEBUGMSG(1,("CBookmarkDialogBox( [%d, %d, %d, %d, %d, ... , %d)\n", ResIndex, x, y, w, h, option ));
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


	DEBUGMSG(1,("-option : %d 0x%x \n", option, option, option ));

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

	BtnOption = option;
	CreateDbxBackground();
	SetTitle( ResIndex );
}

CFileDbx::~CBookmarkDialogBox()
{
	if( pSb1 != NULL )
		delete pSb1;
}

int CFileDbx::LoadingBookmarkData()
{
	return TRUE;
}


void CFileDbx::CreateSelectBar()
{
	//int sby = sy+ _DBX_SELECTBAR_START_Y + ( (choice-1)*_DBX_SELECTBAR_HEIGHT );

/*
	if( mode == _BOOKMARK_SET_DBX )
		//RelativeIndex = choice + 1;

	else
		RelativeIndex = choice ;
*/

	if( BookmarkList.Count() == 0-mode )
		{
			DrawASCIITextWithoutRefresh(
				"No Bookmark Exists.",
				16, 
				15, 
				&TAVI_IMAGES[GULIM_13_WHITE_NORMAL_BORDER],
				sx + _DBX_ITEM_COL_SPX,
				sy+ _DBX_SELECTBAR_START_Y + 3,
				__SKIP_TRANSPARENCY_COLOR
				);
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


void CFileDbx::CreateTextData()
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



			if( (mode&_BOOKMARK_DBX_AT_BEGINNIG) == _BOOKMARK_DBX_AT_BEGINNIG )
				Convert_SecOnly2TimeString( timestring, BookmarkList[ AbsoluteIndex -2]->ulSavetime);
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

			if( description == GULIM_13_BLACK_BOLD )
				DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", '>', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));
			else
				DEBUGMSG(1,("|%c|%13d|%13d|%15d|%21d|%9d|%6d|\n", ' ', RelativeIndex, AbsoluteIndex, FirstShownIndex, SelectedAbsoluteIndex, LastIndex, choice));


			}
		
		
		}

	DEBUGMSG(1,("+-+-------------+-------------+---------------+---------------------+---------+------+\n"));


}


void CFileDbx::CreateVScrollBar()
{
	int TotalItem;
	int VisibleItemCnt;
	int StartIndex;


	
}

int CFileDbx::AddBookmark(  tBookmarkNode* pNode )
{
	if( pNode->ulSavetime > 0 )
		{
		BookmarkList.Add( pNode );
		LastSlot++;
		}
	
	return TRUE;
}

U32 CFileDbx::GetSecondsToSave()
{
	return ulBookmarkingTime;
}

U32 CFileDbx::SetSecondsToSave(U32 seconds)
{
	ulBookmarkingTime = seconds;
	return ulBookmarkingTime;
}

int CFileDbx::GetCurrentChoice()
{
	return choice;
}

int CFileDbx::SetChoiceToExitReverse()
{
	choice = TAVI_KEY_EXIT_REVERSE;
	return choice;
}

U32 CFileDbx::GetSeekPoint()
{

	DEBUGMSG(1,("GetSeekPoint() : choice = %d\n", choice ));

	if ( choice >= 0 )
		return  (BookmarkList[choice]->ulSavetime );
	else
		return 0;

}

int CFileDbx::GetItemCount()
{
	return ( BookmarkList.Count() );
}

#endif
#endif



