//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : FM Radio(GNT-702S) header file for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/FMRadio/FMRadio.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: FMRadio.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.21  2005/12/11 20:18:30  zstein
// *** empty log message ***
//
// Revision 1.20  2005/08/19 05:04:38  ywkim
// change name of function FR_Kill()
//
// Revision 1.19  2005/08/18 03:12:11  ywkim
// Add instruction for background playing
//
// Revision 1.18  2005/07/16 04:51:06  zstein
// - added FR_UI_RECORD.
//
// Revision 1.17  2005/06/21 11:48:17  ywkim
// Change Earphone related routine
//
// Revision 1.16  2005/05/18 20:51:31  ywkim
// Change control pannel
//
// Revision 1.15  2005/05/18 15:57:55  ywkim
// Modify UI
//
// Revision 1.14  2005/05/17 19:18:36  ywkim
// Change msgbox clean
//
// Revision 1.13  2005/05/17 13:52:44  ywkim
// Some modify
//
// Revision 1.12  2005/05/17 09:13:05  ywkim
// modify string related function
//
// Revision 1.11  2005/05/13 05:27:53  ywkim
// modify routine for headphone detection
//
// Revision 1.10  2005/05/03 11:53:44  ywkim
// Change some UI
//
// Revision 1.9  2005/05/03 07:16:50  ywkim
// Add tunner function for europe
//
// Revision 1.8  2005/05/02 07:22:04  ywkim
// Add information box ui
//
// Revision 1.7  2005/04/28 07:19:04  ywkim
// Modifiy function of file read & write
//
// Revision 1.6  2005/03/04 16:13:56  ywkim
// Change function for autoscan
//
// Revision 1.5  2005/03/03 03:20:19  ywkim
// Modify ioctl of volume control
//
// Revision 1.4  2005/03/01 05:48:26  ywkim
// Add touchpad routine
//
// Revision 1.3  2005/02/26 09:18:58  ywkim
// Add function of touchpad & setup
//
// Revision 1.2  2005/02/21 03:56:56  ywkim
// add scrollor bar cpntrol for preset list
//
// Revision 1.1  2005/02/05 13:58:23  ywkim
// New Task
//
//*****************************************************************************

#ifndef __FM_RADIO_H__
#define __FM_RADIO_H__

#include <tavi_lib.h>
#include <ImageFont.h>



/*
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
*/
#define FRICON				0x00000001
#define FRTITLE				0x00000002
#define FRPRESETTITLE		0x00000004
#define FRBIGICON			0x00000008
#define FREQBAR				0x00000010
#define BATTERY				0x00000020
#define TOPFRAME_ONLY		0x00000040

#define LBUTTON				0x00010000
#define RBUTTON				0x00020000
#define FREQUENCYBAR		0x00040000
#define BOTTOMFRAME		0x00080000
#define VOLUMEBAR			0x00100000
#define CONTROLBAR			0x00200000
#define AUTOSCAN			0x00400000
#define PRESETMENUBAR		0x00800000
#define SETUPFRAME			0x01000000

#define TOP_FRAME			0x0000ffff
#define BOTTOM_FRAME		0xffff0000

// Limitation
#define FR_PRESETLIST_MAX			25

#define FR_USFREQUENCY_MIN		87500L
#define FR_USFREQUENCY_MAX		108000L

#define FR_JAPANFREQUENCY_MIN		76000L
#define FR_JAPANFREQUENCY_MAX		108000L

#define FR_RANGEOFFREQUENCY		(FR_USFREQUENCY_MAX -FR_USFREQUENCY_MIN)

#define FR_SEARCHTIMEOUT			20

#define FR_DISPTITLEICON_X			5
#define FR_DISPTITLEICON_Y			4

#define FR_DISPTITLETEXT_X			32
#define FR_DISPTITLETEXT_Y			4

#define FR_DISPBIGICON_X			30
#define FR_DISPBIGICON_Y			49
#define FR_DISPBIGICON_W			64
#define FR_DISPBIGICON_H			81

#define FR_DISPCHNO_X				40
#define FR_DISPCHNO_Y				113

#define FR_DISPFREQUENCY_X			94
#define FR_DISPFREQUENCY_Y			60

#define FR_DISPTUNEMODE_X			184			// disp "     "/"preset"
#define FR_DISPTUNEMODE_Y			201
#define FR_DISPREGION_X			219			// disp "english"
#define FR_DISPREGION_Y			FR_DISPTUNEMODE_Y
#define FR_DISPSTEREO_X			255			// disp "mono"/"stereo"
#define FR_DISPSTEREO_Y			FR_DISPTUNEMODE_Y

#define FR_DRAWICON_W				28			// control pannel icon width
#define FR_DRAWICON_H				14			// control pannel icon height
#define FR_DRAWICON_MAX			8			// number of icons in image file
#define FR_DRAWICON_BLANK			0			// 1st is blank
#define FR_DRAWICON_PRESET		1			// 2nd is preset
#define FR_DRAWICON_STEREO		2			// 3rd is stereo
#define FR_DRAWICON_MONO			3			// 4th is mono
#define FR_DRAWICON_ENGLISH		4			// 5th is english
#define FR_DRAWICON_KOREA			5			// 5th is korea
#define FR_DRAWICON_EUROPE		6			// 5th is Europe
#define FR_DRAWICON_JAPAN			7			// 5th is japan

#define FR_DISP875MHZ_X			33			// 87.5MHz display x_coord.
#define FR_DISP875MHZ_Y			229			// 87.5MHz display y_coord.
#define FR_DISP1080MHZ_X			249			// 108.0MHz display x_coord.
#define FR_DISP1080MHZ_Y			FR_DISP875MHZ_Y		// 108.0MHz display y_coord.

#define FR_DISPAUTOSCANBOX_X		10			// autoscan box x_coord.
#define FR_DISPAUTOSCANBOX_Y		35			// autoscan box y_coord.
#define FR_DISPAUTOSCANBOX_W		300			// autoscan box width
#define FR_DISPAUTOSCANBOX_H		190			// autoscan box height

#define FR_DISPCONTROLFRAME_X		0			// control frame x_coord.
#define FR_DISPCONTROLFRAME_Y		215			// control frame y_coord.
#define FR_DISPCONTROLFRAME_W	326			// control frame x_coord.
#define FR_DISPCONTROLFRAME_H		25			// control frame y_coord.

#define FR_DISPCONTROLBAR_X		33			// control touch pad bar x_coord.
#define FR_DISPCONTROLBAR_Y		220			// control touch pad bar y_coord.
#define FR_DISPCONTROLBAR_W		39			// control pannel width /3
#define FR_DISPCONTROLSETUP_X		FR_DISPCONTROLBAR_X + 4	// disp "Radio Setup"
#define FR_DISPCONTROLSETUP_Y		FR_DISPCONTROLBAR_Y + 3
#define FR_DISPCONTROLPRESET_X	FR_DISPCONTROLBAR_X + 88	// disp "Preset ON"
#define FR_DISPCONTROLPRESET_Y	FR_DISPCONTROLSETUP_Y
#define FR_DISPCONTROLCHANNEL_X	FR_DISPCONTROLBAR_X + 174	// disp "Add Channel"
#define FR_DISPCONTROLCHANNEL_Y	FR_DISPCONTROLSETUP_Y
#define FR_DISPTUNNERBAR_X			34						// tunner bar x_coord. +1
#define FR_DISPTUNNERBAR_Y			FR_DISPCONTROLBAR_Y	// tunner bar y_coord.

#define FR_DISPVOLUMETEXT_X		120			// volume text  frame x_coord.
#define FR_DISPVOLUMETEXT_Y		200			// volume text  frame y_coord.

#define FR_DISPSEEKTEXT_X			160			// volume text  frame x_coord.
#define FR_DISPSEEKTEXT_Y			FR_DISPVOLUMETEXT_Y			// volume text  frame y_coord.

#define FR_DISPHEADPHONE_X		30			// Headphone warnning  frame x_coord.
#define FR_DISPHEADPHONE_Y		141			// Headphone warnning  frame y_coord.
#define FR_DISPHEADPHONE_W		264			// Headphone warnning  frame width.
#define FR_DISPHEADPHONE_H		47			// Headphone warnning  frame height.

#define FR_DISPRBUTTON_X			296
#define FR_DISPRBUTTON_Y			218

#define FR_DISPBATTERY_X			292
#define FR_DISPBATTERY_Y			5
typedef enum
{
	FR_REGION_USA = 0,
	FR_REGION_KOREA,
	FR_REGION_EUROPE,
	FR_REGION_JAPAN
} FR_REGION_t;

typedef enum
{
	FR_BANDLIMIT_US = 0,
	FR_BANDLIMIT_JAPAN = 1
} FR_BANDLIMIT_t;

typedef enum
{
	FR_SSL_LOW = 1,
	FR_SSL_MID = 2,		
	FR_SSL_HIGH = 3
} FR_SSL_t;

typedef enum
{
	FR_XTAL_13MHZ = 0,
	FR_XTAL_32KHZ = 1
} FR_XTAL_t;

typedef enum
{
	FR_INJECTION_LO = 0,
	FR_INJECTION_HI = 1,
	FR_INJECTION_CHECKFAIL = 0xff
} FR_INJECTION_t;

typedef enum
{
	FR_STANDBY_OFF= 0,
	FR_STANDBY_ON=1,
} FR_STANDBY_t;


typedef enum
{
	FR_TUNEMODE_MANUAL = 0,
	FR_TUNEMODE_PRESET = 1
} FR_TUNEMODE_t;

typedef enum
{
	FR_TUNEDIRECTION_NONE = 0,
	FR_TUNEDIRECTION_UP = 1,
	FR_TUNEDIRECTION_DOWN = -1,
} FR_TUNEDIRECTION_t;

typedef enum
{
	FR_REFERENCEPLL_OFF = 0,
	FR_REFERENCEPLL_ON = 1
} FR_REPERENCEPLLSTATE_t;

typedef enum
{
	FR_STATION_NOTFOUND = 0,
	FR_STATION_FOUND = 1,
	FR_STATION_OUTOFBAND = -2,
	FR_STATION_ACCESSFAIL= -1
} FR_SEARCHRESULT_t;

typedef enum
{
	FR_RECEIVED_MONO = 0,
	FR_RECEIVED_STEREO = 1
} FR_RECEIVEDAUDIO_t;

typedef enum
{
	FR_BANDLIMIT_NOTREACHED = 0,
	FR_BANDLIMIT_REACHED = 1
} FR_BANDLIMITRESULT_t;


typedef enum
{
	FR_MUTE_OFF = 0,
	FR_MUTE_ON = 1
} FR_MUTESTATE_t;

typedef enum
{
	FR_LEFTMUTE_OFF = 0,
	FR_LEFTMUTE_ON = 1
} FR_LEFTMUTESTATE_t;

typedef enum
{
	FR_RIGHTMUTE_OFF = 0,
	FR_RIGHTMUTE_ON = 1
} FR_RIGHTMUTESTATE_t;

typedef enum
{
	FR_SEARCH_DOWN = 0,
	FR_SEARCH_UP = 1
} FR_SEARCHDIRECTION_t;

typedef enum
{
	FR_SEARCHMODE_OFF = 0,
	FR_SEARCHMODE_ON= 1
} FR_SEARCHMODE_t;

typedef enum
{
	FR_STEREONOISECANCEL_OFF = 0,
	FR_STEREONOISECANCEL_ON= 1
} FR_STEREONOISESTATE_t;

typedef enum
{
	FR_UI_STARTUP = 0,
	FR_UI_PRESETLIST,
	FR_UI_SETUP,
	FR_UI_SCAN,
	FR_UI_NORMAL,
	FR_UI_CONTROL,
	FR_UI_VOLUME,
	FR_UI_RECORD // added by Lee Seok
} FR_UISTATE_t;


// "MR" Means "FMRadio"

int 	FR_MsgProc( int ssTaskId, U32 ulMsgId, int usParam1 , int usParam2 );
int 	FMRadio_Task( TTaskInitParam* tParam );
int 	FR_InitScanConfirmMsgProc(int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 );
int 	FR_CheckHeadPhone(MSG_PROCESSOR MsgProc);
int 	FR_Kill(void);
int	FR_PlayPause(void);
#endif /* __FM_RADIO_H__ */
