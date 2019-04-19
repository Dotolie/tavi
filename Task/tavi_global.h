//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : Declaration of system parameter structures and functions.
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/tavi_global.h,v $
// $Revision: 1.13 $
// $Date: 2006/06/12 05:38:39 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: tavi_global.h,v $
// Revision 1.13  2006/06/12 05:38:39  zstein
// - changed SysSetRTC(struct rtc_time) to SysSetRTC(int, struct rtc_time).
// - changed data type of scroll_speed which is member var. of TextConf_t to int.
//
// Revision 1.12  2006/06/12 04:38:13  ywkim
// Change structure of SysParameter_t for melon
//
// Revision 1.11  2006/06/07 08:00:16  zstein
// - added power management preset.
//
// Revision 1.10  2006/05/24 03:23:41  zstein
// - added SysSetRTC(...).
//
// Revision 1.9  2006/04/18 07:06:12  ywkim
// Change count value of battery emptiness to 60sec for low battery
//
// Revision 1.8  2006/04/15 01:36:30  zstein
// - added member variable 'volume' as alarm structure.
//
// Revision 1.7  2006/04/12 04:30:42  ywkim
// Change function for battery emptinss
//
// Revision 1.6  2006/04/10 07:24:57  zstein
// -
//
// Revision 1.5  2006/04/07 08:24:43  ywkim
// Change some functions & definitions for HOST
//
// Revision 1.4  2006/04/05 01:55:46  zstein
// - added MAX_VIDEO_BITRATE
//
// Revision 1.3  2006/03/10 10:07:38  etnlwind
// Change TAVI_BATTERY_EMPTY_CNT10 & XX_CNT20 to 40
//
// Revision 1.2  2006/03/09 01:51:30  zstein
// - TaviSysParam::sound::headphone_vol and speaker_vol are removed.
//   TaviSysParam::sound::volume replaces them.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.111  2006/02/22 02:04:51  ywkim
// change member of struct SysParameter_t for nVersion
//
// Revision 1.110  2006/02/17 06:48:27  ywkim
// Change type of ProductID to char[5]
// Change order of struct SysParameter_t
//
// Revision 1.109  2006/02/16 03:13:18  ywkim
// Change macro name of TAVI1600_XXX to TAVI020_XXX
// Change variable type to 13bytes of char. for serial no.
//
// Revision 1.108  2006/01/26 09:00:02  etnlwind
// Update for VideoPlayer Exit Proceed Optimize
//
// Revision 1.107  2006/01/24 00:55:20  zstein
// *** empty log message ***
//
// Revision 1.106  2006/01/16 23:59:47  zstein
// *** empty log message ***
//
// Revision 1.105  2006/01/16 02:36:04  zstein
// *** empty log message ***
//
// Revision 1.104  2006/01/06 11:35:56  zstein
// *** empty log message ***
//
// Revision 1.103  2005/12/30 04:25:43  etnlwind
// Update for New Font Adaption
//
// Revision 1.102  2005/12/29 09:24:43  zstein
// *** empty log message ***
//
// Revision 1.101  2005/12/28 05:12:53  ywkim
// Add definition of RELOAD_WATCHDOGTIMER_9SEC for short time reset
//
// Revision 1.100  2005/12/26 04:43:56  zstein
// *** empty log message ***
//
// Revision 1.99  2005/12/21 22:22:25  yong
// add g_debug_no_shutdown_on_low_battery variable to control whether to shutdown on low battery event
//
// Revision 1.98  2005/12/19 10:43:11  yong
// add MSG_Peek_Ex() to get a finer control
//
// Revision 1.97  2005/12/15 11:09:19  akaiaphanu
// added to adjust screen image
//
// Revision 1.96  2005/12/14 11:26:39  yong
// add sound.output_level
//
// Revision 1.95  2005/12/01 01:04:55  zstein
// *** empty log message ***
//
// Revision 1.94  2005/11/30 04:56:50  ywkim
// Add member of cStereo in TaviSysParam.radio
//
// Revision 1.93  2005/11/25 12:49:06  zstein
// *** empty log message ***
//
// Revision 1.92  2005/11/24 09:14:34  ywkim
// Add member of sound.user_effect on TaviSysParam
// Change default value of sound.band0~4 to 0
//
// Revision 1.91  2005/11/24 03:06:49  zstein
// *** empty log message ***
//
//*****************************************************************************
#ifndef __SYSTEM_PARAMETER_H__
#define __SYSTEM_PARAMETER_H__

#include "Driver/key/tavi_key.h"
#include "Driver/rtc/tavi_rtc.h"
#include "Component/event.h"
#include "version.h"

// debug message
#define TAVI_DEBUG
#ifdef TAVI_DEBUG
#define TAVI_DEBUGMSG       printf
#else
#define TAVI_DEBUGMSG
#endif

#define TOUCH_MENU_HIDE_TIME		5 // sec
#define TURNOFF_EL_TIME			    5
#define TURNOFF_EXTLCD_MSG		    3
#define BMP_MAX_SIZE				486400	// 800X608
#define BACKGROUND_PATH				"/hd2/system/background.yuv"
#define BACKGROUND_WIDTH			320
#define BACKGROUND_HEIGHT			240
#define BACKGROUND_YLENGTH		    408960	// 852*480
#define EPROMID						0x20040601
#define EPROMADDR					0x3ff000
#define TAVI_BATTERY_EMPTY_COUNT	30	// consecutive number of low battery
#define TAVI_BATTERY_EMPTY_CNT04	4
#define TAVI_BATTERY_EMPTY_CNT20	20
#define TAVI_BATTERY_EMPTY_CNT30	30
#define TAVI_BATTERY_EMPTY_CNT40	40
#define AUTO_WND_SWITCH_TIME		10	// sec

#define LEN_SERIALNO				14
#define DECRYPTIONKEY				0xA5

#define MSG_SEND_ID				    0x10
#define MSG_RECV_ID					0x20

// MSG Peek related MACROS
#define MSG_PEEK_KEY				0x000001
#define MSG_PEEK_TOUCH				0x000002
#define MSG_PEEK_AUDIO_PLAYER		0x000004
#define MSG_PEEK_MSG_QUEUE		    0x000008
#define MSG_PEEK_POWER			    0x000010
#define MSG_PEEK_USB				0x000020
#define MSG_PEEK_FOLDER			    0x000040
#define MSG_PEEK_EARPHONE			0x000080
#define MSG_PEEK_IDLE				0x000100
#define MSG_PEEK_SLEEP				0x000200
#define MSG_PEEK_AUTOBRIGHT		    0x000400
#define MSG_PEEK_BATTERY			0x000800
#define MSG_PEEK_TIMER				0x001000
#define MSG_PEEK_SNOOZE			    0x002000




#define MSG_PEEK_MINIMUM		(MSG_PEEK_KEY \
	| MSG_PEEK_TOUCH \
	| MSG_PEEK_AUDIO_PLAYER \
	| MSG_PEEK_MSG_QUEUE \
	| MSG_PEEK_POWER \
	| MSG_PEEK_IDLE \
	| MSG_PEEK_SLEEP \
	| MSG_PEEK_AUTOBRIGHT \
	| MSG_PEEK_BATTERY \
	| MSG_PEEK_TIMER \
	| MSG_PEEK_SNOOZE \
	)
#define MSG_PEEK_NORMAL		(MSG_PEEK_MINIMUM | MSG_PEEK_FOLDER | MSG_PEEK_EARPHONE)
#define MSG_PEEK_ALL			(MSG_PEEK_NORMAL | MSG_PEEK_USB)

enum SortType {
	Sort_Type_Name_Ascent,
	Sort_Type_Name_Descent,
	Sort_Type_Date_Ascent,
	Sort_Type_Date_Descent
};

enum {
	PLAYLIST_ID_INSTANCE = 0x1000,
	PLAYLIST_ID_FAVORITE,
	PLAYLIST_ID_DOMESTIC,
	PLAYLIST_ID_CLASSIC,
	PLAYLIST_ID_WPOP,
	PLAYLIST_ID_ID3ARTIST = 0x2000,
	PLAYLIST_ID_ID3ALBUM,
	PLAYLIST_ID_ID3GENRE,
	PLAYLIST_ID_ID3ALLTRACKS,
	PLAYLIST_ID_USER = 0x3000
};

// POWER MANAGEMENT PRESET
enum {
  PM_PRESET_OFF,
  PM_PRESET_MAX,
  PM_PRESET_NORMAL
};

enum {
  MULTI_CAPTION_0,
  MULTI_CAPTION_1,
  MULTI_CAPTION_ALL
};

// Music Menu Parameters
typedef struct {
	int startTime;
	int repeat;	// boolean
	int shuffle;	// boolean
	int boundary;
	int total; // the number of music
	int update;
	int updatefolder;
	char lastaccess[512];
} MusicConf_t;

typedef struct pm_preset_t {
  int idle_off;
  int lcd_off;
  int auto_br;
  int hdd_sleep;
} pm_preset_t;

// Video Parameters
typedef struct {
	int brightness;
	int contrast;
	int saturation;
	int diaplay_volume;
	int screen;
	int video_output;	// 0: lcd, 1: composite, 2: component
	int playlist_repeat;
	int multi_caption;
	char lastaccess[512];
} VideoConf_t;

typedef struct {
	int scroll_speed;	// Off, Slow, Mid, Fast
	char lastaccess[512];
} TextConf_t;


// Photo Menu Parameters
typedef struct {
	#define PHOTO_VIEW_LISTVIEW	0
	#define PHOTO_VIEW_PREVIEW	1
	#define PHOTO_VIEW_THUMBNAIL	2
	int view;
	int repeat; // slide show repeat
	#define AUTO_HIDE 		0
	#define ALWAYS_HIDE	1
	#define ALWAYS_SHOW	2
	int info_display; // 0: hide, 1: auto hide, 2: always on
	char lastaccess[512];
	int slideshowtime; // 1, 5, 10, 20sec
} PhotoConf_t;

typedef struct {
	int input_vol;
	#define BITRATE_32K		0	
	#define BITRATE_64K		1
	#define BITRATE_128K	2
	#define BITRATE_192K	3
	#define BITRATE_256K	4
	#define BITRATE_320K	5
	int bitrate;
	#define SAMPLINGRATE_32KHZ		0
	int samplingrate;
	int AGC; // AUTO GAIN CONTROL
	char lastaccess[512];
} RecordConf_t;

typedef struct {
	int	x;
	int	y;
	int	w;
	int	h;
} Wnd_t;

typedef struct {
	char cRegion;
	char cStereo;
}RadioConf_t;

typedef struct {
	int MEMID;					// flash memory id
	int nSize;					// structure size
	int nVersion;					// sw version ex) 0x0500
	char ProductID[8];			// ex) T2F3
	char CompileDate[24];		// Jan  5 2005 17:30:30
	char SerialNo[16];	// ex) T20NM5C00171G
	char ValidSerNo;
	int sort;						// 0:Name Ascent, 1:Name Descent, 2:Time Ascent, 3:Time Descent
	struct {
	#define NLS_ENGLISH			0
	#define NLS_KSC5601			1
	#define NLS_JIS				2
		int language;
		int buzzer; // 0: on, 1: off
//		#define DISPLAY_LCD 		0
//		#define DISPLAY_COMPOSITE	1
//		#define DISPLAY_COMPONENT	2
//		#define DISPLAY_HDCOMPONENT 3
//		int display;
	#define NTSC		0
	#define PAL		1
		int tvtype;
	#define P480		0
	#define P720		1
	#define I1080	2
		int hdcomponent;
		int startup;
	#define SCREEN_LETTERBOX	0
	#define SCREEN_4_3PANSCAN	1
	#define SCREEN_16_9		2
		int timeformat;
		int timezone;
		int lcdbright;
		int lastmenu;
		char timezoneString[30];
		int touchspeed;
		int systemfont; 	// 0: Gulime, 1: Dotum
		int captionfont;		// 0: Gulime, 1: Dotum
		int ResVal;
		int ExitOption;
	} etc;
	
	struct {
        int volume;
		int eq;
		int spk_eq;
		int input_vol;
		int samplingrate;
		int speaker_mute;
		int headphone_mute;
		int balance;
		int treble;
		int surround;
		int surround_mex;
		int surround_rex_wide;
		int surround_rex_live;
		int bass;
		int bass_mex;
		int bass_rex_wide;
		int bass_rex_live;
		int band0;
		int band1;
		int band2;
		int band3;
		int band4;
		int user_effect;
		int output_level;
	} sound;
	struct {
#define ALARM_PROCEED 0
#define ALARM_ALIVE 1
#define ALARM_SNOOZED 2
		int AlarmStatus;	//0:Normal, Alarm Poceed   //1:Alarm Alive  //2:Snoozed
		int useAlarm;	// 0: On, 1: Off
		int hour;		// 0 ~ 23
		int min;		// 0 ~ 59
		int repeat;	// 0 ~
		int interval;	// 0 ~
		int onHolidays; // 0 ~
		int AlarmType;	// 0: Default,  1: UserDefine
		int volume;
		char UserMusicFullpath[260*2];
	} alarm;

	Wnd_t output[8];

	struct {
		int	sleeptimer;
		int stoptimer;
		int lcdofftimer;
		int ledonoff;
		int autolcdbright;
		int hddtimer;	// 0 : off, other delay time
		int preset;
	} power;
  pm_preset_t pm_preset[3];
  
	// GUI Parameters
	struct {
		int isUserBackground;
	} gui;

	// Music Menu Parameters
	MusicConf_t music;
	// Video Menu Parameters
	VideoConf_t video;
	// Photo Menu Parameters
	PhotoConf_t photo;
	// Text Menu Parameters
	TextConf_t text;
	// Record parameters
	RecordConf_t record;
	// FM parameter
	RadioConf_t radio;

}SysParameter_t;

typedef enum {
	ExitState_Shutdown,
	ExitState_Lowbattery
} ExitState_t;

extern SysParameter_t TaviSysParam;
extern unsigned short* g_MusicFilepath;
extern int g_MusicFilepathLen;

extern int SysSaveParameters( SysParameter_t* param );
extern int SysLoadParameters( SysParameter_t* param );
extern int SysSetDefaultParameters( SysParameter_t* param );
extern int MapToVirtualKey( int key );
extern int SysSetRTC( int devfd, struct rtc_time t );

/* 
 * UI Global 
 */

#define FLICKERING_DELAY			500000	// 500ms 
#define TYPE_ICON_COUNT			    55
#define TYPE_ICON_HEIGHT			12
#define REPEAT_ICON_COUNT			16
#define EQ_ICON_COUNT				32
#define ROTATE_ICON_COUNT			4
#define PLAYMODE_ICON_HEIGHT		11


// Background Image IDs
#define ID_BG_MAIN				    0
#define ID_BG_TEXT				    1
#define ID_BG_VIDEO				    2
#define ID_BG_AUDIO				    3
#define ID_BG_PHOTO				    4
#define ID_BG_RADIO				    5
#define ID_BG_FILEMANAGER		    6
#define ID_BG_SETUP				    7
#define ID_BG_RECORD			    8
#define ID_BG_ETC				    9
#define ID_BG_ADJUST_SCREEN	        10

#define MUSIC_TITLE_STRING		    "MusicPlayer"
#define PHOTO_VIEWER			    "PhotoViewer"

enum ListIconIndex {
	LII_PLAYLIST_U,
	LII_PLAYLIST_S,
	LII_ARTIST_U,
	LII_ARTIST_S,
	LII_FOLDER_U,
	LII_FOLDER_S,
	LII_ALBUM_U,
	LII_ALBUM_S,
	LII_GENRE_U,
	LII_GENRE_S,
	LII_MUSIC_U, /* 10 */
	LII_MUSIC_S,
	LII_UNKNOWN_U,
	LII_UNKNOWN_S,
	LII_PLAY_U = 18,
	LII_PLAY_S,
	LII_VIDEO_U,
	LII_VIDEO_S,
	LII_PHOTO_U,
	LII_PHOTO_S,
	LII_TEXT_U,
	LII_TEXT_S,
	LII_VOICE_RECORD_U=26,
	LII_VOICE_RECORD_S,
	LII_KEYLOCK_U=46,
	LII_KEYLOCK_S,
	LII_SPEAKER_ON_U=58,
	LII_SPEAKER_ON_S,
	LII_SPEAKER_OFF_U,
	LII_SPEAKER_OFF_S,
	LII_ALARM_U = 64,
	LII_ALARM_S,
	LII_PAUSE_U,
	LII_PAUSE_S,
	LII_EARPHONE_ON,
	LII_EARPHONE_OFF
};

// SYSTEM RESOURCE ID
// SHARED MEMORY
#define SHMKEY              	    0x1234
#define SHMSIZE             	    1024*1024       	// 256 pages

#define MB_SHMKEY				    0xA0B0C010			// key of shared memory for Media Buffer 
#define MB_SHMPERM 				    0660				// permission of shared memory
#define MB_DATABUFFER_SIZE	        (1024*1024*8)
#define MB_PCMBUFFER_SIZE		    (1024*1024)

#define SHM_SCHEME				    0					// 0: old sheme,  1: shared memory scheme
#define XEN_LIB					    1					// 0: disable, 1: Enable XEN sound effect
#define SPECTRUM				    1					// 0: disable, 1: Enable for audioplayer spectrum meter
#define SPECTRUM_X				    160
#define SPECTRUM_Y				    26

/*
    LIMITATION
*/
#define MAX_PATH				    260
#define MAX_FILES				    1000
#define MAX_FOLDERS			        1000
#define MAX_MUSICDB_FILES		    4001
#define MAX_MUSICDB_CLASS		    3001
#define MAX_VIDEO_BITRATE           2300

#define RELOAD_WATCHDOGTIMER_9SEC	(*((volatile unsigned long*)(0x00500120))=0x2B00)
#define RELOAD_WATCHDOGTIMER		(*((volatile unsigned long*)(0x00500120))=0xffff)
#define DISABLE_WATCHDOGTIMER	    (*((volatile unsigned long*)(0x00500128)) &= ~0x80)
#define ENABLE_WATCHDOGTIMER		(*((volatile unsigned long*)(0x00500128)) |= 0x80 )


/*
    Predefined folder names
*/
#define TAVICAST_DIR                "/hd1/TaviCast"

#define RTC_DRIVER_PATH		"/dev/misc/rtc"

#endif /* __SYSTEM_PARAMETER_H__ */
