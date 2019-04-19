//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : VideoPlayer Control Pannel Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibControlPannel.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:07:40 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibControlPannel.h,v $
// Revision 1.2  2006/06/07 08:07:40  zstein
// - removed unused codes.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.34  2006/01/04 08:16:42  etnlwind
// Update for Caption Display Methode
//
// Revision 1.33  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.32  2005/11/03 03:14:33  etnlwind
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
// Revision 1.31  2005/10/31 10:43:47  etnlwind
// Update for VideoPlayer BugFix
//
// Revision 1.30  2005/10/07 10:12:57  etnlwind
// Update for TextViewer
//
// Revision 1.29  2005/10/05 09:43:19  etnlwind
// Update for
// 1. FileManager Copy/Move/Delete Related
// 2. VideoPlayer Bookmarking/Bookmark Loading/Resume Related
//
// Revision 1.28  2005/09/01 06:46:20  etnlwind
// Update for VideoPlayer Screen Option
//
// Revision 1.27  2005/08/29 13:04:32  etnlwind
// Update for VideoPlayer Blank Dir BugFix
//
// Revision 1.26  2005/08/12 11:17:24  etnlwind
// Update for Sound Palette
//
// Revision 1.25  2005/08/09 10:52:09  etnlwind
// Update for
// 1. CSoundPaletteDbx Added
// 2. Video Control Pannel UI changed
//
// Revision 1.24  2005/08/08 10:49:41  etnlwind
// Update for FileManger, VideoPlayer
//
// Revision 1.23  2005/07/21 01:54:14  etnlwind
// Update for
// 1. AllocBitmap256( _IMAGE_ICON"/bu_left.bmp", &TAVI_IMAGES[ICON_BOTTOM_LEFT] ); changed to
//    AllocBitmap256( _IMAGE_ICON"/eq_sound.bmp", &TAVI_IMAGES[ICON_BOTTOM_LEFT] );
// 2. videoPlayer filename location changed
// 3. videoPlayer bottom right icon ordering changed
//
// Revision 1.22  2005/07/13 06:02:30  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.21  2005/06/23 04:44:47  etnlwind
// Update for 3D Sound Effect on VideoPlayer
//
// Revision 1.20  2005/06/22 08:46:44  etnlwind
// Update for DIV3 Seek And Playback
//
// Revision 1.19  2005/06/20 12:30:44  etnlwind
// Update for VideoPlayer
//
// Revision 1.18  2005/06/20 11:36:43  etnlwind
// Update for VideoPlayer Related Bug Fix
//
// Revision 1.17  2005/05/18 17:54:47  etnlwind
// Update for Video Player Timer Related Fix
//
// Revision 1.16  2005/05/17 15:10:41  etnlwind
// Update for BG change
//
// Revision 1.15  2005/05/15 11:44:40  etnlwind
// Update for ES4
//
// Revision 1.14  2005/05/14 06:10:05  etnlwind
// Update for ES4
//
// Revision 1.13  2005/05/12 07:04:23  etnlwind
// Update for Video File Detection Relate
//
// Revision 1.12  2005/05/10 03:02:23  etnlwind
// VideoPlayer TouchScreen Senstive Modify
//
// Revision 1.11  2005/05/06 07:49:25  etnlwind
// Toch Control on VP updates
//
// Revision 1.10  2005/05/04 06:39:38  etnlwind
// Update For Video Player Touch & Slide Button
//
// Revision 1.9  2005/05/03 09:00:04  etnlwind
// Update for ES4
//
// Revision 1.8  2005/04/29 14:32:44  etnlwind
// Update for ES4
//
// Revision 1.7  2005/04/26 10:06:57  etnlwind
// Upate for ES4 UI
//
// Revision 1.6  2005/04/18 09:18:25  etnlwind
// TextEngine Add
// Update for ES4
//
// Revision 1.5  2005/03/05 15:51:12  etnlwind
// 볼륨수정
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


 #ifndef __LIB_CONTROL_PANNEL_H__
 #define __LIB_CONTROL_PANNEL_H__

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
#include <3Dsound/3Dsound.h>

///////////////////////////////////////////////////////////////////////
//
// Control Pannel
// --------------

#define ICONWIDTH_SCR_STATUS	20
#define ICONWIDTH_CAPTION		20
#define ICONWIDTH_REPEAT		20
#define ICONWIDTH_EQ			40

#define ICONHEIGHT				14

#define ICONYPOS 				202

#if 1
#define ICONXPOS_END 			287
#define ICONXPOS_CAPTION			(287-ICONWIDTH_CAPTION)
#define ICONXPOS_SECTION_REPEAT	(ICONXPOS_CAPTION-2-ICONWIDTH_REPEAT)
#define ICONXPOS_PLAYLIST_REPEAT 	(ICONXPOS_SECTION_REPEAT-2-ICONWIDTH_REPEAT)
#define ICONXPOS_EQ 				(ICONXPOS_PLAYLIST_REPEAT-2-ICONWIDTH_EQ)
//#define ICONXPOS_SCR_STATUS 		(ICONXPOS_CAPTION-2-ICONWIDTH_SCR_STATUS)
#else
#define ICONXPOS_END 			287
#define ICONXPOS_EQ 			(ICONXPOS_END-ICONWIDTH_EQ)
#define ICONXPOS_REPEAT 		(ICONXPOS_EQ-2-ICONWIDTH_REPEAT)
#define ICONXPOS_CAPTION		(ICONXPOS_REPEAT-2-ICONWIDTH_CAPTION)
#define ICONXPOS_SCR_STATUS 	(ICONXPOS_CAPTION-2-ICONWIDTH_SCR_STATUS)
#endif
/*
#define LEFTBALL_X				5
#define LEFTBALL_Y				218
#define RIGHTBALL_X				298
#define RIGHTBALL_Y				218
*/

#define TIME_Y_POS				229

#define CO_SKIP_CAPTION 0x00000001

// 
// Common Type Definition

typedef enum
{
	_CONTROL_PANNEL_PROGRESS_MODE,
	_CONTROL_PANNEL_VOLUME_MODE,
	_CONTROL_PANNEL_SUBMENU_MODE,
	_CONTROL_PANNEL_SUBMENU_CAPTION_MODE,
	_CONTROL_PANNEL_SEEK_MODE
} eCONTROL_PANNEL_STATUS;

typedef enum
{
	_CONTROL_PANNEL_SETUP_ICON,
	_CONTROL_PANNEL_SEEK_ICON,
	_CONTROL_PANNEL_VOLUME_ICON,
} eCONTROL_PANNEL_ACTION_ICON;

typedef enum
{
	_NORMAL,
	_REDRAW
} eDRAW_METHOD;

typedef struct _tagMENUBAR_OPTION
{
	char* sLeftMenu;
	char* sCenterMenu;
	char* sRightMenu;
	MSG_PROCESSOR fpLeftMenu;
	MSG_PROCESSOR fpCenterMenu;
	MSG_PROCESSOR fpRightMenu;
	
} tMENUBAR_OPTION;


// Video Player Type Definition
typedef enum
{
	_VCTRL_PLAY,
	_VCTRL_PAUSE,
	_VCTRL_FF,
	_VCTRL_REW
} eVIDEOPLAYER_CONTROL_ICONS;


typedef enum
{
	_VSR_NORMAL,
	_VSR_OPTIMIZED,
	_VSR_FULLSCREEN,
	_VSR_16X19
} eVIDEOPLAYER_SCREEN_RATE_STATUS;


typedef enum
{
	CAPTION_NOTFOUND,
	CAPTION_DISABLED,
	CAPTION_ENABLED
} eVIDEOPLAYER_CAPTION_STATUS;

#if 1
typedef enum
{
	SECTION_REPEAT_DISABLE,
	SECTION_REPEAT_MARK_A,
	SECTION_REPEAT_MARK_AB
	
} eVIDEOPLAYER_SECTION_REPEAT_STATUS;

typedef enum
{
	PLAYLIST_REPEAT_NOREPEAT,	// No Repeat
	PLAYLIST_REPEAT_FOLDER
	
} eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS;
#else
typedef enum
{
	_VRPT_BLANK,
	_VRPT_LOOP,
	_VRPT_ONE_FILE_LOOPING,
	_VRPT_SHUFFLE,
	_VRPT_INTRO,
	_VRPT_MARKING_A,
	_VRPT_MARKING_B
} eVIDEOPLAYER_REPEAT_ICONS;
#endif

typedef enum
{
	_VSI_BLANK=0,
										_VSI_AUDIO_OPTION1,
										_VSI_AUDIO_OPTION2,
										_VSI_AUDIO_OPTION3,
										_VSI_AUDIO_OPTION4,
	_VSI_SECTION_REPEAT_MARK_A,
	_VSI_SECTION_REPEAT_MARK_AB,
										_VSI_AUDIO_OPTION7,
										_VSI_AUDIO_OPTION8,
	_VSI_PLAYLIST_REPEAT_NOREPEAT,
	_VSI_PLAYLIST_REPEAT_FOLDER,
	_VSI_SUBTITLE_DISABLED,
	_VSI_SUBTITLE_ENABLED,
										_VSI_AUDIO_OPTION13,
										_VSI_AUDIO_OPTION14,
	_VSI_SECTION_REPEAT_DISABLE

} eVIDEOPLAYER_STATUS_ICONS;

typedef enum
{
	_VEQ_NORMAL,
	_VEQ_DRAMA,
	_VEQ_ADVENTURE,
	_VEQ_SF,
	_VEQ_ACTION,
	_VEQ_USER
} eVIDEOPLAYER_EQUALIZER_ICONS;



class CControlPannel
{
	int sx, sy;
	int width, height;

	eCONTROL_PANNEL_STATUS			CurrentStatus;
	
	int LeftIconStatus;
	eCONTROL_PANNEL_ACTION_ICON	RightIconStatus; 

	// Video Player DataType Definition
	eVIDEOPLAYER_CONTROL_ICONS PlayStatusIconStatus;
	eVIDEOPLAYER_SCREEN_RATE_STATUS WideNormalScreenStatus;
	eVIDEOPLAYER_CAPTION_STATUS CaptionStatus;			//  CaptionStatus 0: Blank,  1: RepeatAll, 
	eVIDEOPLAYER_SECTION_REPEAT_STATUS SectionRepeatStatus;
	eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS PlaylistRepeatStatus;
	SOUNDEFFECT_t EqualizerStatus;						// 3D Sound

	U16 ProgressPercentage;

	TAVI_TIME pt;
	TAVI_TIME tt;
	int ppg;
	int SeekValue;
	int isSeekingState;

	int isSeekable;

	int tw, tx;
	char ts[8];
		
	uchar_t CurrentFilename[FATFS_MAX_PATHNAME_LENGTH];

	MSG_PROCESSOR fpControlPannelMsgProc;

	void BuildControlPannelBackground();


	// ProgressBar
	void BuildPlayStatusIconStatus();
//	void BuildWideNormalScreenStatus();
	void BuildCaptionStatus();
	void BuildSectionRepeatOptionStatus();
	void BuildPlaylistRepeatOptionStatus();
//	void BuildRepeatOptionStatus();
	void BuildEqualizerStatus();	
//	int BuildProgressBar();
//	int BuildProgressBar(eDRAW_METHOD eDrawMethod);




	// MenuBar
//	int BuildMenuBar( char sLeftMenubar[], char sCenterMenubar[], char sRightMenubar[] );
	int BuildMenuBar( int LeftTextRes, int CenterTextRes, int RightTextRes );

public:

	CTimer* pTimer;
	int isVisible;

	RMint32 RepeatOptionStatusCounter;
	RMint64 MarkingA;
	RMint64 MarkingB;
	
	CControlPannel(  MSG_PROCESSOR pfpMsgProc  );

	// Volume
	void BuildVolumeText( int iVolume );
	int BuildVolumeControlBar( eDRAW_METHOD eDrawMethod, int iVolume );

	//Seek
	int BuildSeekControlBar(eDRAW_METHOD eDrawMethod, int iDirection, int CurrentTargetSeekPosition );
	int BuildSeekTimeText( int seektime );

	int BuildProgressBar(int eDrawMethod);
	
	int SetTime( int object, TAVI_TIME* time );
//	int SetFilename( char filename[] );
	int SetFilename( uchar_t filename[] );
	void ClearFilename();

	int SetPlayStatus(int status);
	eVIDEOPLAYER_CONTROL_ICONS GetPlayStatus();
	int SetMenuBar( tMENUBAR_OPTION* ptMenuBarOption);
	int SetScreenRateType( eVIDEOPLAYER_SCREEN_RATE_STATUS input );
	eVIDEOPLAYER_SCREEN_RATE_STATUS GetScreenRateType();
	eCONTROL_PANNEL_STATUS GetCurrentStatus();

	int SetCurrenSubtitleOptionStatus(eVIDEOPLAYER_CAPTION_STATUS input);
	eVIDEOPLAYER_CAPTION_STATUS GetCurrenSubtitleOptionStatus();

	int SetCurrentSectionRepeatOptionStatus( eVIDEOPLAYER_SECTION_REPEAT_STATUS input );
	eVIDEOPLAYER_SECTION_REPEAT_STATUS GetCurrentSectionRepeatOptionStatus();

	int SetCurrentPlaylistRepeatOptionStatus( eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS input );
	eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS GetCurrentPlaylistRepeatOptionStatus();
	

	int SetEqualizerStatus( SOUNDEFFECT_t input );
	SOUNDEFFECT_t GetEqualizerStatus();

	void SetSeekValue( int newvalue );
	int GetSeekValue();

	void SetSeekable( int input );
	int GetSeekable();

	void SetSeekingState( int input );
	int IsSeekingState();

	RMint64 GetPlaySeconds();

	void SetVisualPlaytime( RMint64 playtime );
	void ClearVisualPlaytime( );

	//void BuildRepeatOptionStatus( eVIDEOPLAYER_REPEAT_ICONS input );
	void BuildSectionRepeatOptionStatus( eVIDEOPLAYER_SECTION_REPEAT_STATUS input );
	void BuildPlaylistRepeatOptionStatus( eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS input );

	int Reset();
	
//	int RedrawPlayStatus( eVIDEOPLAYER_CONTROL_ICONS status );
	int RedrawPlayStatus( eVIDEOPLAYER_CONTROL_ICONS status, eOSD_REFRESH_OPTION refresh );
	int Show();
	int Show( eCONTROL_PANNEL_STATUS eNewState );
	int Show( eCONTROL_PANNEL_STATUS eNewState, RMuint32 ClearOption);
	int Hide();
	int Hide( int* VP_HideIndicator, int ClearOption );
	int SlidingHide();
	int MsgBtnUp( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );

	
};


#endif


