//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Libaray Common Definition Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibDefines.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:11:42 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibDefines.h,v $
// Revision 1.2  2006/06/07 08:11:42  zstein
// - added some macros.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.36  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.35  2005/11/30 10:05:13  etnlwind
// Update for Savefile Size
//
// Revision 1.34  2005/11/11 10:24:26  etnlwind
// Update for DIV3 Video Play/Stop Problems
//
// Revision 1.33  2005/11/11 04:25:58  etnlwind
// Update for VideoPlayer Screen Clean Method
//
// Revision 1.32  2005/11/08 07:16:12  etnlwind
// 전반
// -EXIT키 용도 재정의
// -서브메뉴에서 Close, Back삭제
//
// Revision 1.31  2005/11/01 11:19:11  etnlwind
// Update for VideoPlayer Buffer Redefine
//
// Revision 1.30  2005/10/25 07:09:58  etnlwind
// Update for Palette Adjust
//
// Revision 1.29  2005/10/21 05:35:34  etnlwind
// Update for Booting Progress
// Update for Booting Progress, Alarm MsgBox
//
// Revision 1.28  2005/10/18 09:05:13  etnlwind
// Update for New DialgBox, MessageBox UI
//
// Revision 1.27  2005/10/17 10:17:15  etnlwind
// Update for VideoPlayer, TextViewer, FileManager TopPannel
//
// Revision 1.26  2005/10/14 11:39:38  etnlwind
// Update for TextViwer
//
// Revision 1.25  2005/10/12 09:24:05  etnlwind
// 1. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.24  2005/09/29 05:01:11  etnlwind
// Update for FileManager
//
// Revision 1.23  2005/09/08 04:32:25  etnlwind
// Update for MPEG4 Simple Profile (mp4v) Playback
//
// Revision 1.22  2005/08/30 12:23:23  etnlwind
// Update for FileManager SubMenu Related
//
// Revision 1.21  2005/08/30 04:31:01  etnlwind
// Upate for VideoPlayer / FileManager
//
// Revision 1.20  2005/08/29 13:04:32  etnlwind
// Update for VideoPlayer Blank Dir BugFix
//
// Revision 1.19  2005/08/29 04:18:11  etnlwind
// Update for DEBUGMSG skip
//
// Revision 1.18  2005/08/29 03:02:46  etnlwind
// Update for Bookmark Dbx BugFix
//
// Revision 1.17  2005/08/14 09:38:33  etnlwind
// Update for
// 1. VideoPlayer Bookmark Delete
// 2. VideoPlayer FileBrowser Subtitle Ready Video File Icon Specificationized
// 3. VideoPlayer UI minor updates
//
// Revision 1.16  2005/08/12 11:17:24  etnlwind
// Update for Sound Palette
//
// Revision 1.15  2005/08/09 10:52:09  etnlwind
// Update for
// 1. CSoundPaletteDbx Added
// 2. Video Control Pannel UI changed
//
// Revision 1.14  2005/08/03 07:55:07  ywkim
// Remark dubug msg
//
// Revision 1.13  2005/07/13 12:57:32  etnlwind
// Update for DEBUGMSG Related Functions
//
// Revision 1.12  2005/07/12 12:45:35  etnlwind
// Update for LibFileDbx
//
// Revision 1.11  2005/07/11 13:02:13  etnlwind
// Update for CVS
//
// Revision 1.10  2005/07/04 21:03:17  etnlwind
// Update for SubMenu Class
//
// Revision 1.9  2005/05/18 17:54:47  etnlwind
// Update for Video Player Timer Related Fix
//
// Revision 1.8  2005/05/03 13:56:29  etnlwind
// Touch Pad Enable
//
// Revision 1.7  2005/04/27 17:01:58  etnlwind
// New Type MsgBox Library Update
//
// Revision 1.6  2005/04/18 09:18:25  etnlwind
// TextEngine Add
// Update for ES4
//
// Revision 1.5  2005/03/06 18:43:40  etnlwind
// 사운드라이브러리 수정
//
// Revision 1.4  2005/03/06 11:06:07  etnlwind
// ...
//
// Revision 1.3  2005/03/05 23:25:09  etnlwind
// Hide Debugging Message For CeBIT And Testing
//
// Revision 1.2  2005/03/03 09:11:49  etnlwind
// USB연결관련 코드 추가
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library 정리
//
//
//*****************************************************************************

 #ifndef __LIB_DEFINES_H__
 #define __LIB_DEFINES_H__

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


#define	__TV_DISPLAY_WIDTH	720
#define	__TV_DISPLAY_HEIGHT	480
#define	__LCD_DISPLAY_WIDTH	320
#define	__LCD_DISPLAY_HEIGHT 	240

#define _SCREEN_WIDTH_		320
#define _SCREEN_HEIGHT_	240
#define MAX_LIST_ROW		6

#define _USABLE_AREA_X 		0
#define _USABLE_AREA_Y 		30
#define _SUBTITLE_TAB		15
#define _SUB_SUBTITLE_TAB	20
#define _ROWSIZE			17
#define _TITLE_START_Y		_USABLE_AREA_Y + 2
#define _MENUITEM_START_Y	_TITLE_START_Y + 23

#define 	_CP_BASE_COLOR_PALLETTE_INDEX	24

#define SCROLLBAR_SPACE	15

#define TITLE_ICON_X	5
#define TITLE_ICON_Y	4
#define TITLE_TEXT_X	32
#define TITLE_TEXT_Y	4
#define BATTERY_X		292
#define BATTERY_Y		5
#define LEFTBALL_X		5
#define LEFTBALL_Y		218
#define RIGHTBALL_X		296
#define RIGHTBALL_Y		218

#define _DISPLAY_NORMAL               0
#define _DISPLAY_ZOOM                 1
#define _DISPLAY_FULLSCREEN           2
#define _DISPLAY_16X9_OPTIMIZED       3
#define _DISPLAY_16X9_FULL            4

#define _VIDEO_LCD_OUT 0
#define _VIDEO_COMPOSITE_OUT 1
#define _VIDEO_COMPONENT_OUT 2
#define _VIDEO_HD_COMPONENT_OUT 3

#define _CONTROL_PANNEL_AUTO_HIDE 0
#define _CONTROL_PANNEL_ALWAYS_HIDE 1
#define _CONTROL_PANNEL_ALWAYS_SHOW 2

#define MAX_FULLPATH_NAME_LENGTH 259

extern TTAVI_GLOBAL_CONFIG* pTAVI;
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern MpegDecoder *pMpegDecoder;
extern VIDEOFILE_PLAYINFO gVideofilePlayinfo;
extern TAVI_TIME playtime;
extern TAVI_TIME totaltime;
extern char gCurrentVolume;
extern RMint32 toggle_display;
extern RMint32 i32VideoWidth, i32VideoHeight;

#define _SHADOW 24
#define _MPEG4_ENABLE 1	// o: Disable MPEG4 Video (mpv)   1: Enable MPEG4 Video (mpv)

#define MINIMUM_FILESIZE 4096
#define MAXIMUM_BOOKMARK_ALLOW 	128
#define TEXTVIEWER_BLOCK_BUFFER_SIZE 4096

#define MAIN_PALETTE 0
#define SUB_PALETTE 1
#define VIDEO_LOADING_PROGRESS_PALETTE 103

#define _AVI_ADV		3
#define _MP4V 			4
#define _MPEG 			5



#define 	_CP_BASE_COLOR_PALLETTE_INDEX	24
#define 	_CP_HEIGHT	42
#define 	_CP_BASE_START_Y_POS (_SCREEN_HEIGHT_ - _CP_HEIGHT - 1)

#if 0
	#define debug_outs	printf
	static void debug_break (void)
	{
	}
	#define ASSERT(exp)					((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
	#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))
	#define DEBUG_U16MSG(ustr, str) (print_ustr(ustr, str ))
#else
	#define ASSERT(exp)
	#define DEBUGMSG(cond,printf_exp)
	#define DEBUG_U16MSG(ustr, str)
#endif

#define MAX(X,Y) (((X)>=(Y))?(X):(Y))	// to find the max between two number.
#define MIN(X,Y) (((X)<=(Y))?(X):(Y))	// to find the min between two number.
#define OptionCheck(option,id) ((((option)&(id))==(id))?(1):(0))
#define ICON_VIDEOwSUBTITLE (54)

#endif

