//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Image Related Library Header
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibImage.h,v $
// $Revision: 1.3 $
// $Date: 2006/06/07 08:08:26 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibImage.h,v $
// Revision 1.3  2006/06/07 08:08:26  zstein
// - removed unused codes.
//
// Revision 1.2  2006/03/09 02:15:03  zstein
// - changed header notations.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.46  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.45  2005/12/07 10:12:28  etnlwind
// Update for TextViewer TASK_KEY
//
// Revision 1.44  2005/12/07 03:03:25  etnlwind
// Update for Emergency Exit On Bookmark DialogBox
//
// Revision 1.43  2005/12/05 03:07:44  etnlwind
// Update for DrawBottomGuidePannel
//
// Revision 1.42  2005/12/02 11:06:13  etnlwind
// Update for SoundPalette UI
//
// Revision 1.41  2005/11/25 08:40:40  etnlwind
// Update for New EQ
//
// Revision 1.40  2005/11/21 02:51:32  etnlwind
// Update for EQ Select UI
//
// Revision 1.39  2005/10/22 05:07:28  etnlwind
// Udpate for GetASCIITextSize
//
// Revision 1.38  2005/10/19 09:48:09  etnlwind
// Update for Startup Progress Display
//
// Revision 1.37  2005/10/14 11:39:38  etnlwind
// Update for TextViwer
//
//*****************************************************************************

/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * LibImage.h
 * Sang-jin Ryu (ew@newmedialife.com)
 *
 */

 #ifndef __LIB_IMAGE_H__
 #define __LIB_IMAGE_H__

#include <stdio.h>
#include <stdlib.h>

#include "rmbasic.h"
#include "linuxfs.h"
#include "jpeg/jpeg.h"

#include "tavi_lib.h"
#include "tavi_structures.h"
#include "bitmap.h"
#include "ImageFont.h"
#include "ImageResource.h"
#include "meplib.h"
#include "fs.h"

#include "../Component/object.h"
#include "../Component/Stack.h"
#include "../Component/event.h"
#include "../Component/fbdev.h"
#include "../Component/image.h"
#include "../Component/timer.h"

#include <Lib/LibText.h>

#define	TAVI_KEY_EXIT_REVERSE		(TAVI_KEY_EXIT*-1)
#define	TAVI_KEY_LEXIT_REVERSE	(TAVI_KEY_LEXIT*-1)
#define	TAVI_KEY_MODE_REVERSE	(TAVI_KEY_MODE*-1)
#define	TAVI_KEY_LMODE_REVERSE	(TAVI_KEY_LMODE*-1)

#define ClearOsd()			pMpegDecoder->RenderText_WithoutRefresh(0,0,0,0,320,240)
#define ClearDbxOsd()		pMpegDecoder->RenderText_WithoutRefresh(0,0,0,30,320,210)
#define ClearScreen()			pMpegDecoder->DisplayYUV420( 0, 0, 0, 0, 720, 480, 720, 480 )



typedef enum
{
	NORMAL					= 0x00000001,
	WITHOUT_REFRESH			= 0x00000002,
	SKIP_TRANSPARENCY_COLOR	= 0x00000004
	
} eDRAWITEM_OPTION;


typedef int (*MSG_PROCESSOR)( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
typedef int (*SETUP_PROCESSOR)( int* parameters );
typedef int (*MENU_PROCESSOR)( RMuint8* Parameters );


RMuint8 rgb_Y(RMuint8 r , RMuint8 g , RMuint8 b);
RMuint8 rgb_U(RMuint8 r , RMuint8 g , RMuint8 b);
RMuint8 rgb_V(RMuint8 r , RMuint8 g , RMuint8 b);

int AllocBitmap256(char * file, BITMAPFILEINFO256* bitmapheader);
static int AllocBitmap256_FATFs( char * file, BITMAPFILEINFO256* bitmapheader );

RMuint16 DrawItem( BITMAPFILEINFO256* pBitmap, RMuint16 usPictureID, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight );
RMuint16 DrawItemWithoutRefresh( BITMAPFILEINFO256* pBitmap, RMuint16 usPictureID, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight );
RMuint16 DrawItemWithoutRefreshSkipTransparency( BITMAPFILEINFO256* pBitmap, RMuint16 usPictureID, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight );
RMuint16 DrawPixelWithoutRefresh( RMuint8 color, RMuint16 sx, RMuint16 sy );
RMuint16 DrawBox( RMuint8 color, RMuint16 sx, RMuint16 sy, RMuint16 width, RMuint16 height );
RMuint16 DrawBoxWithoutRefresh( RMuint8 color, RMuint16 sx, RMuint16 sy, RMuint16 width, RMuint16 height );
RMuint16 DrawASCIIChar( char code, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight );
RMuint16 DrawDigit1( int code, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight );
RMuint16 _DrawASCIITextWithoutRefresh( char text[], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 type, RMuint8 color );
RMuint16 DrawASCIITextWithoutRefresh( char text[], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 type);
RMuint16 DrawASCIIText( char text[], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 type);
RMuint16 DrawProgressBarWithoutRefresh( RMuint16 percentage );
RMuint32 DrawLoadingProgressV4( RMuint32 option, void *context );
RMuint16 DrawDigit3( int code, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight );
void DrawStartupProgress( int progress );

int DrawProgressBar( int percentage );

int DrawPlayTime( TAVI_TIME* playtime, eOSD_REFRESH_OPTION RefreshOption );
int DrawTotalTime( TAVI_TIME* totaltime, eOSD_REFRESH_OPTION RefreshOption );
int DrawMessageBox( char Title[], char Contents[], int sx, int sy, int width, int height );
void DrawBottomGuideBGPannel( eOSD_REFRESH_OPTION option );

#define BGPID_HOME		0x00000002
#define BGPID_MENU		0x00000004
#define BGPID_SELECT	0x00000008
void DrawBottomGuidePannel( eOSD_REFRESH_OPTION refresh, int option=0);
void DrawBottomGuidePannel( eOSD_REFRESH_OPTION refresh, RMuint32 option );

#include <Lib/LibText.h>
#include <Lib/LibUtilities.h>
#include <Lib/LibScrollBar.h>
#include <Lib/LibDialogBox.h>
#include <Lib/LibMsgBox.h>
#include <Lib/LibBookmarkDialogBox.h>
#include <Lib/LibSetupDialogBox.h>
#include <Lib/LibControlPannel.h>
#include <Lib/LibListBox.h>
#include <Lib/LibFileBrowser.h>
#include <Lib/LibSubMenu.h>
#include <Lib/LibFileDbx.h>
#include <Lib/LibSoundPaletteDbx.h>
#include <Lib/LibSubtitleSyncDbx.h>
#include <Lib/LibVirtualKeyboard.h>
#endif


