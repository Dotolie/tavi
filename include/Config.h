//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : TAVI System Configuration
// 
// $Source: /home/cvsroot/TAVI020/tavi/include/Config.h,v $
// $Revision: 1.4 $
// $Date: 2006/06/12 04:39:11 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: Config.h,v $
// Revision 1.4  2006/06/12 04:39:11  ywkim
// Add definition of _SYSTEMCONFIG
//
// Revision 1.3  2006/04/13 01:46:05  ywkim
// Add definition for TAVICAST
//
// Revision 1.2  2006/02/22 13:05:43  zstein
// - deleted unused headers.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.45  2006/01/03 09:33:43  etnlwind
// Update for e-Book Changed to eBook
//
// Revision 1.44  2005/12/30 04:26:50  etnlwind
// Update for New Font Adaption
//
// Revision 1.43  2005/12/15 11:10:11  akaiaphanu
// added to adjust screen image
//
// Revision 1.42  2005/12/14 09:55:28  etnlwind
// Update for Shutdown on VideoPlayer
//
// Revision 1.41  2005/11/28 01:16:34  etnlwind
// Update for e-Book
//
// Revision 1.40  2005/11/16 08:48:55  etnlwind
// Update for AlarmSet
//
// Revision 1.39  2005/11/03 03:14:46  etnlwind
// AVI 비디오 재생시 프레임레이트 정보가 잘못되어 있는지 검사해보고
// 잘못되어 있을경우 대화상자로 사용자에게 현상황을 알리고 재생여부를 확인
// 사용자가 재생을 시도하면, Seek기능을 막고 재생하도록 수정
// (프레임레이트 정보가 깨진 비디오 파일은 주로 Seek기능에는
// 문제가 생기고, 가끔씩 재생중 멈추거나 끊어지는 경우도 발생함)
//
// 비디오 재생중 TASK버튼 누를경우 배경을 지우지 않고 메인으로 가는경우 수정
//
// 비디오 디렉토리 반복 수정
// 비디오 재생시간 잔상제거
// 비디오 전체시간 위치 수정
// 비디오 재생중 볼륨조절시 비디오재생이 좀더 매끄럽게 수정
//
// Revision 1.38  2005/10/21 05:37:16  etnlwind
// Update for Booting Progress
// Update for Booting Progress, Alarm MsgBox, Palette Management
//
// Revision 1.37  2005/10/18 12:36:34  akaiaphanu
// Add Extra Image & Id
//
// Revision 1.36  2005/10/17 10:17:28  etnlwind
// Update for VideoPlayer, TextViewer, FileManager TopPannel
//
// Revision 1.35  2005/10/13 01:29:18  etnlwind
// *** empty log message ***
//
// Revision 1.34  2005/10/13 01:25:25  etnlwind
// Update for Header Info
//
//*****************************************************************************

/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * Config.h
 * Sang-jin Ryu (etnlwind@newmedialife.com)
 *
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "TypeDef.h"
#include <mpegdec/mpegdec.h>
#include <mpegdec/em85xx.h>
#include <rmbasic.h>
#include <define_fs.h>

#define __IMAGE_ON_HDD_

#define _HDD_ROOT			"/hd1"
#define _HDD2_ROOT			"/hd2"
#define _SYSTEMDIR			_HDD2_ROOT"/system"
#define _AUDIO_DIR			_HDD_ROOT"/Music"
#define _VIDEO_DIR			_HDD_ROOT"/Video"
#define _PICTURE_DIR		_HDD_ROOT"/Photo"
#define _eBOOK_DIR			_HDD_ROOT"/eBook"
#define _RECORD_DIR			_HDD_ROOT"/Record"
#define _TAVICAST_DIR		_HDD_ROOT"/TAVICAST"


#ifdef __IMAGE_ON_HDD_
//#define _IMAGE_DIR			_SYSTEMDIR"/img3"	// Image on HDD
#define _IMAGE_DIR			_SYSTEMDIR"/img4"	// Image on HDD
#else
#define _IMAGE_DIR			"/img"				// Image on ROMfs
#endif

#define _PALETTE_DIR		_SYSTEMDIR"/palettes"
#define _FONT_DIR			_SYSTEMDIR"/fonts"
#define _LANGUAGE_DIR		_SYSTEMDIR"/language"
#define _SAVEDATA_DIR		"savedata"
#define _SAVEDATA_FULLPATH _SYSTEMDIR"/"_SAVEDATA_DIR
#define _TEMP_DIR			_HDD_ROOT"/temp"
#define _SKIN_DIR			"/skins"				// Skins on ROMfs
#define _DEFAULT_ALARM		_SYSTEMDIR"/alarm.mp3"

#define _SYSTEM_FONT_GULIM		_FONT_DIR"/gulim12.bdf.opt"
#define _SYSTEM_FONT_DOTUM		_FONT_DIR"/dotum12.bdf.opt"
#define _CAPTION_FONT_GULIM		_FONT_DIR"/gulim16.bdf.opt"
#define _CAPTION_FONT_DOTUM		_FONT_DIR"/dotum16.bdf.opt"

///////////////////////////////////////////////////////////////////
// _IMAGE_DIR
#define _IMAGE_MAIN		_IMAGE_DIR"/main"
#define _IMAGE_CONTROL		_IMAGE_DIR"/controls"
#define _IMAGE_FRAME		_IMAGE_DIR"/frame"
#define _IMAGE_BG			_IMAGE_DIR"/bg"
#define _IMAGE_ICON			_IMAGE_DIR"/icons"
#define _IMAGE_FONT		_IMAGE_DIR"/fonts"

///////////////////////////////////////////////////////////////////
// _LANGUAGE_DIR
typedef enum
{
	SL_ENGLISH=0,
	SL_KOREAN,
	SL_JAPANESE,
	SL_SPANISH,
	SL_ITALIAN,
	SL_FRENCH,
	SL_GERMAN,
	SL_RUSSIAN,
	SL_CHINESE
} eSYSTEM_LANGUAGE;



///////////////////////////////////////////////////////////////////
// _SAVEDATA_FILES
#define _VIDEO_FILEBROWSER_SAVEFILE		_SAVEDATA_FULLPATH"/video.filebrowser.save"
#define _TEXT_FILEBROWSER_SAVEFILE		_SAVEDATA_FULLPATH"/text.filebrowser.save"
#define _FILEMANAGER_SAVEFILE				_SAVEDATA_FULLPATH"/filemanager.save"

#define _VIDEOPLAYER_BOOKMARK_DATA		"_db"
#define _TEXTVIEWER_BOOKMARK_DATA		"_db"

///////////////////////////////////////////////////////////////////
// _PALETTE_RELATED_FILES
#define _MAIN_TRANSPARENCY_FILE				_PALETTE_DIR"/main.transparency"
#define _SUB_TRANSPARENCY_FILE				_PALETTE_DIR"/sub.transparency"

///////////////////////////////////////////////////////////////////
// _LANGUAGE_FILES
#define _LANGUAGE_ENGLISH		_LANGUAGE_DIR"/ENGLISH"
#define _LANGUAGE_KOREAN		_LANGUAGE_DIR"/KOREAN"
#define _LANGUAGE_SPANISH		_LANGUAGE_DIR"/SPANISH"
#define _LANGUAGE_ITALIAN		_LANGUAGE_DIR"/ITALIAN"
#define _LANGUAGE_FRENCH		_LANGUAGE_DIR"/FRENCH"
#define _LANGUAGE_GERMAN		_LANGUAGE_DIR"/GERMAN"
#define _LANGUAGE_RUSSIAN		_LANGUAGE_DIR"/RUSSIAN"
#define _LANGUAGE_JAPANESE	_LANGUAGE_DIR"/JAPANESE"
#define _LANGUAGE_CHINESE		_LANGUAGE_DIR"/CHINESE"


///////////////////////////////////////////////////////////////////
// _TEMP_DIR
#define _VIDEO_BOOKMARK	_TEMP_DIR"/bookmark"
#define _VIDEO_RESUME		_TEMP_DIR"/resume"

#define __TAVI_LOGO			_IMAGE_DIR"/bg/startup.jpg"
#define __NML_LOGO			_IMAGE_DIR"/bg/shutdown.jpg"

#define __TAVI_MAIN_BG		_IMAGE_DIR"/bg/main-bg-blue2.jpg"
#define __TEXT_VIEWER_BG	_IMAGE_DIR"/bg/bg-text.jpg"
#define __VIDEO_PLAYER_BG	_IMAGE_DIR"/bg/bg-video.jpg"
#define __AUDIO_PLAYER_BG	_IMAGE_DIR"/bg/bg-music.jpg"
#define __IMAGE_VIEWER_BG	_IMAGE_DIR"/bg/bg-photo.jpg"
#define __FM_RADIO_BG		_IMAGE_DIR"/bg/bg-radio.jpg"
#define __FILE_MANAGER_BG	_IMAGE_DIR"/bg/bg-browse.jpg"
#define __TAVI_SETUP_BG	_IMAGE_DIR"/bg/bg-setup.jpg"
#define __RECORD_BG			_IMAGE_DIR"/bg/bg-record.jpg"
#define __ETC_BG			_IMAGE_DIR"/bg/bg-extra.jpg"
#define __ADJUST_SCREEN_BG	 _IMAGE_DIR"/bg/bg-adjust-screen.jpg"

///////////////////////////////////////////////////////////////////
// config file
#define	_SYSTEMCONFIG		_HDD_ROOT"/config.bin"

#define	VIDEO_TIMER_ID0	0
#define	VIDEO_TIMER_ID1	1
#define	VIDEO_TIMER_ID2	2
#define	VIDEO_TIMER_ID3	3

#define MAX_PALETTE_INDEX	256

typedef struct _tag_TTAVI_GLOBAL_CONFIG
{
	U16 	usCurrentMenu;

	U16 	isHoldState;			// 0 = NotHold , 1= Hold
	U16	isControlShown;

	U16	Volume;				// 0 ~ 79
	U16 	BatteryLevel;		// 0 ~ 3
	U16 IsBatteryDebugMode;
	int nBatteryLevel;
	U16	SoundOutput;		// 0 = Speaker, 1 = Headset
	//U16 isUSBStorageConnected;
	U16 isTaviWorkingAsUsbClient;	// 0 = Normal, 1 = TAVI Activates As Client
	U16 isTaviWorkingAsUsbHost;		// 0 = Normal, 1 = TAVI Activates As Host
	U16 isVideoPlaying;
	int iVideoTimerId;
	int BatteryLevelRequests;
	int ssTaskId;
	U16 isAlarmState;
	int AlarmHour;	// 0~23
	int AlarmMin;	// 0~59
	int isShutdownNeeds;

	RMuint8* MainPalette;
	RMuint8* MainPalette_Transparent;
	RMuint8* SubPalette;
	RMuint8* SubPalette_Transparent;
	RMuint8* VideoLoadingPalette;
	RMuint8* VideoLoadingPalette_Transparent;
	RMuint8* FlexablePalette;
	RMuint8* FlexablePalette_Transparent;

} __ATTRIBUTE_PACKED__ TTAVI_GLOBAL_CONFIG;




extern MpegDecoder * pMpegDecoder;

#endif

