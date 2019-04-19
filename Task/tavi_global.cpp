//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Functions for system parameters.
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/tavi_global.cpp,v $
// $Revision: 1.12 $
// $Date: 2006/06/14 03:06:05 $
// $Author: ywkim $
//
//******************************************************************************
// $Log: tavi_global.cpp,v $
// Revision 1.12  2006/06/14 03:06:05  ywkim
// Disable clean variable of ValidSerNo
//
// Revision 1.11  2006/06/12 08:41:33  zstein
// - set default value of lcd off time to 0.
//
// Revision 1.10  2006/06/12 05:38:39  zstein
// - changed SysSetRTC(struct rtc_time) to SysSetRTC(int, struct rtc_time).
// - changed data type of scroll_speed which is member var. of TextConf_t to int.
//
// Revision 1.9  2006/06/12 04:37:27  ywkim
// Change external variable to local one for handle_rtc
//
// Revision 1.8  2006/06/07 08:00:16  zstein
// - added power management preset.
//
// Revision 1.7  2006/05/24 03:23:41  zstein
// - added SysSetRTC(...).
//
// Revision 1.6  2006/04/15 01:35:03  zstein
// - added member variable 'volume' as alarm structure.
//
// Revision 1.5  2006/03/16 04:48:50  zstein
// - set TILE_VIEW as default list view of photo.
//
// Revision 1.4  2006/03/15 03:07:05  ywkim
// Change some routine related timezone in SysSetDefaultParameters()
//
// Revision 1.3  2006/03/14 11:15:14  ywkim
// Change SysSetDefaultParameter() for timezone
//
// Revision 1.2  2006/03/09 01:52:13  zstein
// - changed default volume level to 20.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "tavi_global.h"
#include "Lib/LibFlash.h"
#include <memory.h>
#include <time.h>
#include "Component/ClockSetView.h"
#include <fcntl.h>

/* TaviSysParam variable is a system parameter variable.
 * It is used as parameters of LCD/TV, Network, GUI and so on.
 * Also, It is loaded and stored from/to FLASH MEMORY.
 * Lee Seok<zstein@newmiedialife.com>
 */

#define DEBUG
#ifdef DEBUG

	#define debug_outs	printf
	static void debug_break (void)
	{
	}
	#define ASSERT(exp)				((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
	#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))

#else
	#define ASSERT(exp)
	#define DEBUGMSG(cond,printf_exp)
#endif


#define SIZE_PARAMETER 		(1024*4)

SysParameter_t TaviSysParam;

pm_preset_t pm_preset[3] = {
  { 0, 0, 1, 0 }, /* off */
  { 2, 30, 1, 30 }, /* normal */
  { 1, 10, 1, 30 } /* max */
};


int SysLoadParameters( SysParameter_t* param )
{
	// copy from flash to mem
	memcpy( param, (SysParameter_t*)EPROMADDR, sizeof(SysParameter_t) );

	if ( param->MEMID !=  EPROMID || param->nVersion != TAVI020_VERSION) {
		DEBUGMSG(1,("SysLoadParameters : MEMID incorrect, Now set default value & write \n") );
		SysSetDefaultParameters( &TaviSysParam );
		SysSaveParameters( &TaviSysParam );
	}
	return 0;
}

int SysSaveParameters( SysParameter_t* param )
{
	//Not implemented yet
	int	nSize;
	unsigned short pll;
	unsigned short uart;
	int 	nWriteError=0;

	DEBUGMSG( 1,("Tavi_global.cpp : SysSetParameters: address= 0x%08lx, size=%d\n", (int)param, param->nSize ) );
	
	nSize = param->nSize;
	if( nSize%2 ) nSize++;

	if ( nSize > SIZE_PARAMETER )
		nSize = SIZE_PARAMETER;
	
	if ( flash_init() != 0 )	
	{
		nWriteError= 1;
		DEBUGMSG( 1,( "Tavi_global.cpp : flash init error.\n" ) );
	}
	else
	{
		if ( flash_erase( EPROMADDR, nSize, NULL, 0,100 ) ) 
		{
			nWriteError= 1;
			DEBUGMSG( 1,( "Tavi_global.cpp : flash erase error.\n" ) );
		}
		else
		{
			if ( flash_write((unsigned short *)&TaviSysParam, EPROMADDR, nSize, NULL, 0, 100) ) 
			{
				nWriteError= 1;
				DEBUGMSG( 1,( "Tavi_global.cpp : flash write error\n" ) );
			}
		}
	}
	if ( nWriteError )
	{
		return -1;
	}

	return 0;
}

int SysSetDefaultParameters( SysParameter_t* param )
{

	param->MEMID = EPROMID;
	param->nSize = sizeof(SysParameter_t);

	sprintf( param->ProductID, "%s", TAVI020_PRODUCTID);	// ex) T2F3
	param->nVersion=TAVI020_VERSION;					// sw version ex) 0.5.0.0
	sprintf( param->CompileDate,"%s",TAVI020_COMPILETIME);
//	sprintf( param->SerialNo,"%s", TAVI020_SERIALNO);		// "T20NM5C00171G"
//	param->ValidSerNo = 0;
	param->sort = Sort_Type_Name_Ascent;

	param->sort = 0;					// Name Ascent

	// Load Video parameters
	param->video.brightness = 28;		// Mean Value
	param->video.contrast = 28;		// Mean Value
	param->video.saturation = 32;		// Mean Value
	param->video.multi_caption = MULTI_CAPTION_ALL; 
	param->video.playlist_repeat = 0; 	// PLAYLIST_REPEAT_NOREPEAT
	param->video.screen = 0;			// _DISPLAY_NORMAL
	param->video.video_output = 0; 	// LCD
	
	param->sound.volume = 20;
	param->sound.spk_eq = 0;
	param->sound.eq = 0;
	param->sound.surround = 3;
	param->sound.surround_mex = 3;
	param->sound.surround_rex_wide = 3;
	param->sound.surround_rex_live = 3;
	param->sound.bass = 3;
	param->sound.bass_mex = 3;
	param->sound.bass_rex_wide = 3;
	param->sound.bass_rex_live = 3;
	param->sound.band0 = 9;
	param->sound.band1 = 9;	
	param->sound.band2 = 9;	
	param->sound.band3 = 9;	
	param->sound.band4 = 9;
	param->sound.user_effect = 0; 			// mex user mode
	param->sound.input_vol = 50;			// center
	param->sound.samplingrate = 0;
	param->sound.speaker_mute = 0;		// no mute
	param->sound.balance = 0;
	param->sound.input_vol = 40;
	param->sound.output_level = 0;		// Output Level (2005-12-14) 0: Normal, 1: High

	param->etc.buzzer = 0;				// on
	if ( param->etc.language < NLS_ENGLISH || param->etc.language > NLS_JIS )
		param->etc.language = 0;				// english
//	param->etc.display = DISPLAY_LCD;
	param->etc.tvtype = NTSC;
	param->etc.hdcomponent = P480;
	param->etc.startup = 0;				// resume
	param->etc.timeformat = 0;               	// 12h

	//	param->etc.timezone = 0;				// seoul
	//	strcpy(param->etc.timezoneString, "TZ=GMT-9");	// seoul

	if( 	(param->etc.timezone < 0) || (param->etc.timezone >= MAX_COUNT_TIMEZONES) ) {
		param->etc.timezone = 1;				// NewYork
		}
	CClockSetView *pClockView;
	pClockView = new CClockSetView( NULL );
	pClockView->SetTimeZone(param->etc.timezone);
	strcpy(param->etc.timezoneString, pClockView->m_szTimeZone );
	if ( pClockView ) 	delete pClockView;


	param->etc.lcdbright = 5;
	param->etc.lastmenu = 20;		// second app.
	param->etc.touchspeed = 2;		// middle of touch speed 
	param->etc.systemfont = 0;		// 0: Gulime, 1: Dotum
	param->etc.captionfont = 0;		// 0: Gulime, 1: Dotum
	param->etc.ResVal = -1;
	param->etc.ExitOption = -1;
	
	// Alarm
	param->alarm.useAlarm = 1;  // Boolean:
	param->alarm.hour = 6;      // [0 .. 23]
	param->alarm.min = 0;       // [0 .. 59]
	param->alarm.repeat = 0;    // [0 .. 4]
	param->alarm.interval = 0;  // [0 .. 2]
	param->alarm.volume = 30;   // [0 .. 40]
	param->alarm.AlarmType=0;	// 0: Default,  1: UserDefine
	param->alarm.UserMusicFullpath[0] =0;
	param->alarm.UserMusicFullpath[1] =0;

	// GUI Parameters
	param->gui.isUserBackground = 0;

	// Load Music parameters
	param->music.repeat = 0;
	param->music.shuffle = 0;
	param->music.boundary = 1; // all: 0, category: 1, single: 2
	param->music.startTime = 0;
	param->music.update = 0;
	param->music.updatefolder = 0;
	memset( param->music.lastaccess, 0, sizeof(param->music.lastaccess) );

	// Load Photo parameters
	param->photo.slideshowtime = 1;
	param->photo.repeat = 1;
	param->photo.view = PHOTO_VIEW_THUMBNAIL;
	param->photo.info_display = AUTO_HIDE;
	memset( param->photo.lastaccess, 0, sizeof(param->photo.lastaccess) );

	// Load Radio parameters
	param->radio.cRegion = 0;		// set region to u.s.a
	param->radio.cStereo = 0;		// set stereo mode
	
	// Load Text parameters

	// Load Recorder parameters
	param->record.input_vol = 50;
	param->record.bitrate = BITRATE_128K;
	param->record.samplingrate = SAMPLINGRATE_32KHZ;
	memset( param->record.lastaccess, 0, sizeof(param->photo.lastaccess) );

	// Load power timer paramters
	param->power.sleeptimer = 0;
	param->power.stoptimer = 0;
	param->power.lcdofftimer = 0;
	param->power.autolcdbright=1;	// auto off
	param->power.preset = PM_PRESET_OFF;
	memcpy( param->pm_preset, pm_preset, sizeof(pm_preset) );
	
	param->power.hddtimer = 0;	// power save

	// load display screen parameters
	param->output[0].x = 0;
	param->output[0].y = 0;
	param->output[0].w = 320;
	param->output[0].h = 240;
	// ntsc - composite/svideo
	param->output[1].x = 0;
	param->output[1].y = 0;
	param->output[1].w = 720;
	param->output[1].h = 480;
	// pal - composite/svideo
	param->output[2].x = 0;
	param->output[2].y = 0;
	param->output[2].w = 720;
	param->output[2].h = 576;
	// ntsc - component
	param->output[3].x = 0;
	param->output[3].y = 0;
	param->output[3].w = 720;
	param->output[3].h = 480;			
	// pal - component
	param->output[4].x = 0;
	param->output[4].y = 0;
	param->output[4].w = 720;
	param->output[4].h = 576;			
	// 480p
	param->output[5].x = 0;
	param->output[5].y = 0;
	param->output[5].w = 720;
	param->output[5].h = 480;
	// 720p
	param->output[6].x = 0;
	param->output[6].y = 0;
	param->output[6].w = 1280;
	param->output[6].h = 720;
	// 1080i
	param->output[7].x = 0;
	param->output[7].y = 0;
	param->output[7].w = 1920;
	param->output[7].h = 1080;

	return 0;
}


int MapToVirtualKey( int key )
{
	int vkey = 0;
	
	switch( key ) {
	case TAVI_KEY_UP:
		vkey = VK_UP;
		break;
	case TAVI_KEY_DOWN:
		vkey = VK_DOWN;
		break;
	case TAVI_KEY_RIGHT:
		vkey = VK_RIGHT;
		break;
	case TAVI_KEY_LEFT:
		vkey = VK_LEFT;
		break;
	case TAVI_KEY_ENTER:
		vkey = VK_ENTER;
		break;
	case TAVI_KEY_EXIT:
		vkey = VK_ESC;
		break;
	case TAVI_KEY_LEXIT:
		vkey = VK_EXT_ESC;
		break;
	case TAVI_KEY_EQ:
		vkey = VK_F1;
		break;
	case TAVI_KEY_MENU:
		vkey = VK_F2;
		break;
	case TAVI_KEY_MODE:
		vkey = VK_F3;
		break;
	case TAVI_KEY_AB:
		vkey = VK_F4;
		break;
	default:
		vkey = key;
		break;
	}

	return vkey;
}

int SysSetRTC( int devfd, struct rtc_time rt )
{
  int handle_rtc;
  time_t t;
  struct tm* tm;

  t = time( NULL );
  tm = gmtime( &t );
  tm->tm_hour = rt.tm_hour;
  tm->tm_min  = rt.tm_min;
  t = mktime( tm );
  tm = gmtime( &t );
    
	rt.tm_hour = tm->tm_hour;
	rt.tm_min = tm->tm_min;

	if ( devfd < 0 ) {
		return -1;
	}
	else {
		if( ioctl( devfd, RTC_ALM_SET, &rt ) < 0 ) {
			return -1;
		}
	}
	return 0;
}
