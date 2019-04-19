//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : TAVI Data Structures
// 
// $Source: /home/cvsroot/TAVI020/tavi/include/tavi_structures.h,v $
// $Revision: 1.4 $
// $Date: 2006/06/07 08:33:12 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: tavi_structures.h,v $
// Revision 1.4  2006/06/07 08:33:12  zstein
// - removed unused codes.
//
// Revision 1.3  2006/04/13 01:46:05  ywkim
// Add definition for TAVICAST
//
// Revision 1.2  2006/04/05 02:08:30  zstein
// changed typedef int(*VP_RESYNC_SMI)() to typedef int(*VP_RESYNC_SMI)( int );
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.26  2005/12/22 05:13:43  etnlwind
// Update for New Version
//
// Revision 1.25  2005/12/08 08:17:42  etnlwind
// update for Task Switching
//
// Revision 1.24  2005/11/01 04:27:25  etnlwind
// ERR_VR_FPS_VALUE_INVALID Macro Added
//
// Revision 1.23  2005/10/17 06:14:38  etnlwind
// Update for TextViewer
//
// Revision 1.22  2005/10/13 01:29:18  etnlwind
// *** empty log message ***
//
// Revision 1.21  2005/10/13 01:25:25  etnlwind
// Update for Header Info
//
//*****************************************************************************


/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * tavi_structure.h
 * Sang-jin Ryu (etnlwind@newmedialife.com)
 * 
 */

#ifndef __TAVI_STRUCTURES_H__
#define __TAVI_STRUCTURES_H__

#include "rmbasic.h"
#include "bitmap.h"

typedef unsigned short int uchar_t;
typedef unsigned short int U16;
typedef unsigned long U32;

typedef struct _tag_TAVI_TIME {
	int			hours;
	int			minutes;
	int			seconds;
	RMint64 		SecOnly;
} _ATTRIBUTE_PACKED_ TAVI_TIME;

typedef struct _tag_VIDEOFILE_PLAYINFO {
	RMint32 x;
	RMint32 y;
	RMint32 width;
	RMint32 height;
	RMint32 w1;
	RMint32 h1;

	RMint32 OriginalX;
	RMint32 OriginalY;
	RMint32 OriginalWidth;
	RMint32 OriginalHeight;

	RMint32 OptimizedX;
	RMint32 OptimizedY;
	RMint32 OptimizedWidth;
	RMint32 OptimizedHeight;
} _ATTRIBUTE_PACKED_ VIDEOFILE_PLAYINFO;

// Shared 
typedef enum {
	__SPEAKER=0,
	__HEADSET
} eSOUND_OUTPUT_METHOD;

// Shared
typedef enum {
	RESUME0=0,
	RESUME1,
	RESUME2,
	PLAYLIST0,
	PLAYLIST1,
	ARTIST0,
	ARTIST1,
	FOLDER_OPEN0,
	FOLDER_OPEN1,
	FOLDER_CLOSE0,
	FOLDER_CLOSE1,
	ALBUM0,
	ALBUM1,
	GENRE0,
	GENRE1,
	TITLE0,
	TITLE1,
	YEARS0,
	YEARS1,
	SCRIPT0,
	SCRIPT1,
	INFO0,
	INFO1,
	VIDEO0,
	VIDEO1,
	PICTURE0,
	PICTURE1,
	TEXT0,
	TEXT1
} eICON_LIST;
	
typedef enum
{
	// Normal Option
	OSD_REFRESH=0,
	SKIP_OSD_REFRESH=1,

	// Special Option
	CALCULATION_ONLY	=0x00010000,
	SHOW_BTN_GUIDE	=0x00020000
	
} eOSD_REFRESH_OPTION;

typedef enum {
	NAME_ASCEND,
	NAME_DESCEND,
	DATE_ASCEND,
	DATE_DESCEND
} eSortingMethod;


typedef struct _tag_DirFactor {
	char Title[256];		// 
	char Fullpath[256];
	char PathName[256];
	U32	ulLevel;		// 0: root,  1:MUSIC,  2: Playlists
	U32 ulType;		// 0: folder, 1: collect
} _ATTRIBUTE_PACKED_ tDirFactor;


////////////////////////////////////////////////////////////////////////////////////////////////////////
// AudioPlayer Callback Function Definition
typedef int (*AP_DRAW_PROGRESS_BAR)(int value);		//int AP_DrawProgressBar( int percentage );
typedef int (*AP_DRAW_PLAY_TIME)( TAVI_TIME* playtime );
typedef int (*AP_DRAW_TOTAL_TIME)( TAVI_TIME* totaltime );
typedef U16 (*AP_DRAW_ITEM)( BITMAPFILEINFO256* pBitmap, U16 usPictureID, U16 usX, U16 usY, U16 usItemWidth, U16 usItemHeight );
typedef U16 (*AP_DRAW_ITEM_WITHOUT_REFRESH)( BITMAPFILEINFO256* pBitmap, U16 usPictureID, U16 usX, U16 usY, U16 usItemWidth, U16 usItemHeight );



////////////////////////////////////////////////////////////////////////////////////////////////////////
// VideoPlayer Callback Function Definition
typedef int (*VP_DRAW_PROGRESS_BAR)(int value);		//int VP_DrawProgressBar( int percentage );
typedef int (*VP_DRAW_PLAY_TIME)( TAVI_TIME* playtime, eOSD_REFRESH_OPTION RefreshOption );
typedef int (*VP_DRAW_TOTAL_TIME)( TAVI_TIME* totaltime, eOSD_REFRESH_OPTION RefreshOption );
typedef U16 (*VP_DRAW_ITEM)( BITMAPFILEINFO256* pBitmap, U16 usPictureID, U16 usX, U16 usY, U16 usItemWidth, U16 usItemHeight );
typedef U16 (*VP_DRAW_ITEM_WITHOUT_REFRESH)( BITMAPFILEINFO256* pBitmap, U16 usPictureID, U16 usX, U16 usY, U16 usItemWidth, U16 usItemHeight );
typedef int (*VP_IS_STATUSBAR_VISIBLE)( );
typedef U16(*VP_DRAW_ASCII_TEXT)(char text [ ], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256 * pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 type);
typedef U16(*VP_DRAW_ASCII_TEXT_WITHOUT_REFRESH)(char text [ ], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256 * pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 type);
typedef void(*VP_REDRAW_PROGRESSBAR)( int fWithoutRefresh  );
typedef int(*VP_GET_PROGRESS_PERCENTAGE)();
typedef int(*VP_SET_VIDEO_RENDERER)(int value);
typedef int(*VP_DRAW_MSG_BOX)(RMuint32 value);
typedef RMuint32 (*VIDEOFILE_RENDERER)( uchar_t* filename, RMuint32 decoder );
typedef void(*SET_SEEKABLE)(int value);
typedef int(*GET_SEEKABLE)();
typedef int(*IS_SEEKING_STATE)();
typedef int(*VP_RESYNC_SMI)( int );
typedef int(*VP_CLOSE_SMI)();
typedef int(*VP_SET_SCREEN_DISPLAY_RATIO)();

////////////////////////////////////////////////////////////////////////////////////////////////////////
// VideoPlayer Callback Table
typedef struct _tag_VIDEOPLAYER_CALLBACK_TABLE {
	VP_DRAW_PROGRESS_BAR 				DrawProgressBar;
	VP_DRAW_PLAY_TIME					DrawPlayTime;
	VP_DRAW_TOTAL_TIME					DrawTotalTime;
	VP_DRAW_ITEM						DrawItem;
	VP_DRAW_ITEM_WITHOUT_REFRESH		DrawItemWithoutRefresh;
	VP_IS_STATUSBAR_VISIBLE				IsStatusbarVisible;
	VP_DRAW_ASCII_TEXT					DrawASCIIText;
	VP_DRAW_ASCII_TEXT_WITHOUT_REFRESH	DrawASCIITextWithoutRefresh;
	VP_REDRAW_PROGRESSBAR			RedrawProgressBar;
	VP_GET_PROGRESS_PERCENTAGE		GetProgressPercentage;
	VP_SET_VIDEO_RENDERER				SetVideoRenderer;
	VP_DRAW_MSG_BOX					DrawMsgBox;
	SET_SEEKABLE						SetSeekable;
	GET_SEEKABLE						GetSeekable;
	IS_SEEKING_STATE					IsSeekingState;
	VP_RESYNC_SMI						ResyncSMI;
	VP_CLOSE_SMI						CloseSMI;
	VP_SET_SCREEN_DISPLAY_RATIO		SetScreenDisplayRatio;
} _ATTRIBUTE_PACKED_ VIDEOPLAYER_CALLBACK_TABLE;

typedef enum
{
	_DIRECTORY,
	_DIRECTORY_LINK,		
	_USB_DIRECTORY,
	_NETWORK_DIRECTORY,
	_AUDIO_FILE,
	_VIDEO_FILE,
	_IMAGE_FILE,
	_TEXT_FILE,
	_SUBTITLE_FILE,
	_ETC_FILE
} eFILE_TYPE;

typedef enum
{
	ERR_VR_NOERROR 		= 0x00000000,
	ERR_VR_UNKNOWN 		= 0x00000100,
	ERR_VR_RES_OVER		= 0x00000400,
	ERR_VR_QPEL			= 0x00000800,
	ERR_VR_GMC				= 0x00001000,
	ERR_VR_MP42			= 0x00002000,
	ERR_VR_MP4V			= 0x00004000,
	ERR_VR_FPS_VALUE_INVALID	= 0x00008000
} eVIDEO_RENDER_ERROR;

#endif

