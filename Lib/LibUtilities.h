//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibUtilities Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibUtilities.h,v $
// $Revision: 1.3 $
// $Date: 2006/06/07 08:16:57 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibUtilities.h,v $
// Revision 1.3  2006/06/07 08:16:57  zstein
// - removed unused codes.
//
// Revision 1.2  2006/04/07 08:15:57  ywkim
// Change many function for Host
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.42  2006/01/11 09:29:54  etnlwind
// Update for TextLib
//
// Revision 1.41  2005/12/30 04:24:42  etnlwind
// Update for New Font Adatio
//
// Revision 1.40  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.39  2005/11/30 10:05:13  etnlwind
// Update for Savefile Size
//
// Revision 1.38  2005/11/24 10:56:46  etnlwind
// Update for New EQ
//
// Revision 1.37  2005/11/23 09:33:53  etnlwind
// Update for TAVI-EQ
//
// Revision 1.36  2005/11/22 04:55:07  etnlwind
// Update for SoundPalette New UI
//
// Revision 1.35  2005/11/07 10:26:42  etnlwind
// 텍스트뷰어 기능추가
// -한페이지에 13라인 출력하게끔 수정 (기존:16라인)
// -페이지 이동 처음/끝 추가
// -자동 라인 스크롤 추가 (slow=9초, mid=6초, fast=3초 임의지정 )
//
// Revision 1.34  2005/10/12 09:24:06  etnlwind
// 1. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.33  2005/10/07 10:12:57  etnlwind
// Update for TextViewer
//
// Revision 1.32  2005/10/05 09:43:19  etnlwind
// Update for
// 1. FileManager Copy/Move/Delete Related
// 2. VideoPlayer Bookmarking/Bookmark Loading/Resume Related
//
// Revision 1.31  2005/09/29 10:56:58  etnlwind
// Update for FileManager
//
// Revision 1.30  2005/09/16 01:11:18  etnlwind
// Update for FileManager USB Host Related
//
// Revision 1.29  2005/08/31 13:38:56  etnlwind
// Update for Library
//
// Revision 1.28  2005/08/24 13:07:55  etnlwind
// Update for Virtual Keyboard
//
// Revision 1.27  2005/08/23 07:12:23  etnlwind
// Update for FileManager
//
// Revision 1.26  2005/08/17 08:44:18  etnlwind
// Update for SoundPalette Affect
//
// Revision 1.25  2005/08/17 08:16:49  etnlwind
// Update for Sound Palette
//
// Revision 1.24  2005/08/17 03:48:02  etnlwind
// Update for Sound Palette Related Func.
//
// Revision 1.23  2005/08/14 09:38:33  etnlwind
// Update for
// 1. VideoPlayer Bookmark Delete
// 2. VideoPlayer FileBrowser Subtitle Ready Video File Icon Specificationized
// 3. VideoPlayer UI minor updates
//
// Revision 1.22  2005/07/07 14:33:22  etnlwind
// Update for Subtitle Bug Fix
//
// Revision 1.21  2005/06/22 08:46:44  etnlwind
// Update for DIV3 Seek And Playback
//
// Revision 1.20  2005/06/16 09:16:00  zstein
// - added ClearKeyBuffer().
//
// Revision 1.19  2005/06/15 12:41:34  etnlwind
// Update for Video Touch Seek Process
//
// Revision 1.18  2005/06/13 03:45:56  etnlwind
// Update for MPEG video ration detection related codes
//
// Revision 1.17  2005/05/18 17:54:47  etnlwind
// Update for Video Player Timer Related Fix
//
// Revision 1.16  2005/05/10 03:02:23  etnlwind
// VideoPlayer TouchScreen Senstive Modify
//
// Revision 1.15  2005/05/06 03:30:24  etnlwind
// Update for ES4
//
// Revision 1.14  2005/05/04 06:39:38  etnlwind
// Update For Video Player Touch & Slide Button
//
// Revision 1.13  2005/04/29 14:32:44  etnlwind
// Update for ES4
//
// Revision 1.12  2005/04/26 10:06:56  etnlwind
// Upate for ES4 UI
//
// Revision 1.11  2005/04/20 11:28:46  etnlwind
// update for ES4
//
// Revision 1.10  2005/04/18 14:30:43  etnlwind
// update for ES4
//
// Revision 1.9  2005/04/18 09:18:25  etnlwind
// TextEngine Add
// Update for ES4
//
// Revision 1.8  2005/03/06 11:06:07  etnlwind
// ...
//
// Revision 1.7  2005/03/05 13:38:16  etnlwind
// Timer
//
// Revision 1.6  2005/03/05 13:00:45  etnlwind
// Timer관련 App수정
//
// Revision 1.5  2005/03/05 12:02:11  etnlwind
// key related
//
// Revision 1.4  2005/03/03 18:25:36  etnlwind
// Library 수정
//
// Revision 1.3  2005/03/03 04:58:13  etnlwind
// VideoPlayer, FileBrowser 부분 수정
//
// Revision 1.2  2005/03/02 11:58:41  etnlwind
// Msg 처리 추가
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library 정리
//
//
//*****************************************************************************

 #ifndef __LIB_UTILITIES_H__
 #define __LIB_UTILITIES_H__

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

#include "LibWideString.h"
#include "LibImage.h"
#include "../3Dsound/3Dsound.h"

int GetParentDir_In_UC16FullDirString( uchar_t target[], uchar_t source[] );
int GetParentDir_In_UTF8FullDirString( char target[], char source[] );
int GetCurrentDir_InFullDirString( char target[], char source[] );
int GetCurrentDir_InFullDirString_UC16( uchar_t target[], uchar_t source[] );
int SetBatteryLevel( U16 Parameter );
int IsBatteryLevelChanged( U16 Parameter );
int GetLastItem_In_FullDirString( char target[], char source[] );
int GetLastItem_In_FullDirString_WithUserDefineSplitter( char target[], char source[], char splitter );
int GetLastItem_In_UC16FullDirString_WithUserDefineSplitter( uchar_t target[], uchar_t source[], uchar_t splitter );
char* Convert_SecOnly2TimeString( char sTimeString[], int iSeconds );
int GetNthString( char sTarget[], char sSource[], char cDivChar, int N );
int CaseNormalize( char Target[], char Source[] );
int CheckExtension( char Source[] );
int CheckExtension_UC16( uchar_t* ucSource );
int CheckVideoExtension( char Source[] );
int CheckVideoExtension_UC16( uchar_t ucSource[] );


char* trimh( char input[] );
void print_ustr( const uchar_t* uString );
void print_ustr( const uchar_t* uString, const char* msg );
void print_ustr( const uchar_t* uString, int option );
void printu( const char* msg, const uchar_t* uString );

int CompareExtension( char SourceFilename[], char SourceExtension[] );
int GetMSTime (void);

typedef enum
{
	_TOUCHPAD_NULL=0,
	_TOUCHPAD_LEFT,
	_TOUCHPAD_CENTER,
	_TOUCHPAD_RIGHT
} eTOUCHPAD_CLICK;
eTOUCHPAD_CLICK GetVirtualButtonClick( int Param1, int Param2 );
eTOUCHPAD_CLICK GetVirtualButtonClick( int Param1, int Param2, int* MeanValue );
int GetAbsoluteClickPosition( int Param1, int Param2 );

void Reset_VideoControlPannelTimer();
void Reset_FileBrowserTimer();
void Reset_VideoChunkTimer();
void Reset_VideoSeekTimer();
void Reset_VideoSeekFreeTimer();
void Reset_SoundPaletteCtrlHideTimer();

void Stop_VideoControlPannelTimer();
void Stop_FileBrowserTimer();
void Stop_VideoChunkTimer();
void Stop_VideoSeekTimer();
void Stop_VideoSeekFreeTimer();
void Stop_SoundPaletteCtrlHideTimer();

void Reset_SoundPaletteTouchApplyTimer();
void Stop_SoundPaletteTouchApplyTimer();


void SetTimer( long* pRegTimer );
void ClearTimer( long* pRegTimer );

long Check_VideoSeekTimer();


#define TK_REPEAT	0x0001
#define TK_LONG	 	0x0002
#define TK_SINGLE 	0x0004
#define TK_VARIOUS 	0x0008
#define TK_RESUME	0x0010
void SetTaviKeyMode( U16 flags );
void SetTaviTouchpadMode( int mode );
extern void ClearKeyBuffer( void );
extern int crc32(char *s, int len);
extern unsigned long crcu32(char *s, int len);

// 3D Sound Palette Related
extern SOUNDEFFECT_t GetSFX_ValueByPID( int picture_id );		// Convert "SoundPalette Icon Index" to SOUNDEFFECT_t
extern SOUNDEFFECT_t GetSFX_ValueByMacro( int macro_name );	// Convert MACRO to SOUNDEFFECT_t
extern int GetSFX_MacroByPID( int picture_id );						// Convert "SoundPalette Icon Index"  to MACRO
extern int GetSFX_MacroBySFX( SOUNDEFFECT_t sound_effect );			// Convert SOUNDEFFECT_t  to MACRO
extern int GetSFX_IconIndexByMacro( int macro_name );					// Convert MACRO to "SoundPalette Icon Index"
extern int GetSFX_IconIndexBySFX( SOUNDEFFECT_t sound_effect );		// Convert	 SOUNDEFFECT_t to "SoundPalette Icon Index"
extern int BuildFullPath( uchar_t* Fullpath, uchar_t* CurrentDir, uchar_t* Filename );

extern void showdata( int iSaveData[] );
extern void showdata_U32( U32 SaveData[], char* msg );
extern void showdata_U32( U32 SaveData[] );
void UpdateAlarmStatus();
extern uchar_t* SetTimeStr( int AmPmTxtRes, int TimeDigit, uchar_t* pBuffer );

#endif
