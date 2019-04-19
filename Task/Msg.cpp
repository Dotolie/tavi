//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : Tavi Message Process Source
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Msg.cpp,v $
// $Revision: 1.25 $
// $Date: 2006/06/21 10:53:20 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: Msg.cpp,v $
// Revision 1.25  2006/06/21 10:53:20  ywkim
// Remove some delay in _MSG_USB_Client_Connected()
//
// Revision 1.24  2006/06/19 04:54:20  ywkim
// Remove check routine of g_HddSleep.m_bLcdEnable in SetAutoLCD_Brightness()
//
// Revision 1.23  2006/06/13 07:53:41  zstein
// - fixed alarm bug.
//
// Revision 1.22  2006/06/12 05:39:12  zstein
// - fixed preset power management bug.
//
// Revision 1.21  2006/06/12 04:35:42  ywkim
// Change usb related function for melon
//
// Revision 1.20  2006/05/27 10:15:16  etnlwind
// - power off on usb.
//
// Revision 1.19  2006/05/13 05:38:12  zstein
// - removed MSG_Peek_Rawxx() and fixed idle stop bug.
//
// Revision 1.18  2006/04/18 07:06:12  ywkim
// Change count value of battery emptiness to 60sec for low battery
//
// Revision 1.17  2006/04/15 01:31:39  zstein
// - Ignores hold key when LCD is off.
// - fixed alarm bug.
//
// Revision 1.16  2006/04/12 04:30:42  ywkim
// Change function for battery emptinss
//
// Revision 1.15  2006/04/07 08:24:43  ywkim
// Change some functions & definitions for HOST
//
// Revision 1.14  2006/03/27 11:34:40  zstein
// - removed unnecessary routine.
//
// Revision 1.13  2006/03/17 07:09:13  etnlwind
// Changne values of battery level
//
// Revision 1.12  2006/03/16 07:31:17  etnlwind
// Activate original App-Specific routine in MSG_Peek_Ex()
//
// Revision 1.11  2006/03/16 05:15:32  etnlwind
// Change value of Cutoff level
//
// Revision 1.10  2006/03/13 07:17:14  etnlwind
// Fix some bug for average of nBatteryLevelList
//
// Revision 1.9  2006/03/10 10:08:03  etnlwind
// Change value of battery levels
//
// Revision 1.8  2006/03/09 12:27:32  etnlwind
// Change count of empty battery check to 20 for all task
//
// Revision 1.7  2006/03/09 03:49:02  etnlwind
// Change routine of battery calc & BAT_LEVEL
// Change interval of monitoring time to 6 sec
// Display battery icon on isVideoPlaying with controlpannel when ac was connected/disconnected
//
// Revision 1.6  2006/03/09 02:06:34  zstein
// - improved volume interface
//
// Revision 1.5  2006/03/08 12:59:30  zstein
// - turn ext EL backlight off on usb connected.
//
// Revision 1.4  2006/03/08 12:30:16  zstein
// - handled an alarm error
//
// Revision 1.3  2006/03/08 12:23:52  etnlwind
// Disable EXT_EL at usb connecting time on _MSG_USB_Client_Connected()
// Clear nBatteryEmptyCount on CalcBatteryLevel() if nLevelSym is more than equal BATTERY_LEVEL_0
//
// Revision 1.2  2006/03/07 03:34:18  etnlwind
// Increase BATTERY_LEVEL_SAMPLE_SIZE to 30
// Disable EXT_EL_Enable( true ) when usb was connected
// Insert ClearBit( USB_CONNECT ) when usb is disconnected
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*****************************************************************************

#include <string.h>
#include <errno.h>
#include <linux/module.h>
#include <time.h>
#include "Msg.h"
#include "../Driver/key/tavi_key.h"
#include "../Driver/touchpad/meplib.h"
#include "../Driver/rtc/tavi_rtc.h"
#include <Component/timer.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <util.h>
#include <unistd.h>

//#include <dirent.h>
//#include <sys/stat.h>
//#include <sys/mount.h>
#include "linuxfs.h"

#include "tavi_global.h"
#include "MiscCtrl.h"
#include "SndCtrl.h"
#include "TopPanel.h"
#include "SubLCD.h"
#include "ExtLCD.h"
#include "Lib/LibBuzzer.h"
#include "Lib/LibText.h"
#include <fatfs.h>

#include "AudioPlayer/AudioPlayer.h"
#include "AudioPlayer/AudioCmd.h"
#include "tavi_mq.h"
#include "VideoPlayer/VideoPlayer.h"
#include "Recorder/RecorderMain.h"
#include "lcd/lcd.h"
#include "TextViewer/TextViewer.h"
#include "ImageViewer/ImageViewer.h"
#include "Extras/CExtraClock.h"

#include "./AudioPlayer/Melon/Melon.h"

#if 0
	#define debug_outs	printf
	static void debug_break (void)
	{
	}
	#define ASSERT(exp)					((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
	#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))
#else
	#define ASSERT(exp)
	#define DEBUGMSG(cond,printf_exp)
#endif

#define KEY_DECO_TIME_DIFF			5	/* second(s) */
#define BATTERY_CHECK_INTERVAL		3	/* second(s) */
#define GO_AHEAD					1
#define STOP_RETURN					0
#define ADC_10BIT_UNIT				32

#define _AC_STATUS(x)	((x) & (1 << AC_DETECT))

typedef struct _tag_TMsgQueue
{
   int 			head;
   int 			tail;
   TMsgEntry 	queue[MSG_QUEUE_DEPTH];
} _ATTRIBUTE_PACKED_ TMsgQueue;

static TMsgQueue tMsgQueueList[MSG_PIPE_NUM];
static int s_disableUSBConnect = 0;

int handle_key;
int handle_rtc;

bool g_PowerOff;
bool g_Hold;

extern CSndCtrl* g_SndCtrl;
extern CMiscCtrl* g_MiscCtrl;
extern CTopPanel* g_TopPanel;
extern CExtLCD* g_ExtLCD;
extern SysParameter_t TaviSysParam;
extern CTextEngine_BDF* pT;
extern CFileBrowser *pLFBrowser;
extern CControlPannel *pCp;
extern TTAVI_GLOBAL_CONFIG* pTAVI;
extern RMint32	toggle_display;
extern HDDsleepReg g_HddSleep;		// power save

ExitState_t g_ExitState;

//extern char gCurrentVolume;

long CP_RegisteredUTime=0;	// Control Pannel
long FB_RegisteredUTime=0;	// File Browser
long VP_RegisteredUTime=0;	// Video Player
long VS_RegisteredUTime=0;	// Video Seek
long VSF_RegisteredUTime=0;	// Video Seek Free
long RegUT_SoundPaletteCtrlHide = 0;
long RegUT_SoundPaletteCtrlHideTouchApplyTimer = 0;
long TV_AutoScroll_RegUT1=0;	// TextViewer AutoScroll
long TV_AutoScroll_RegUT2=0;	// TextViewer AutoScroll
long TV_AutoScroll_RegUT3=0;	// TextViewer AutoScroll
	
int	nIdle_lcdofftimer = 0;	
int	nIdle_sleeptimer = 0;	
int	nIdle_stoptimer = 0;	
long	nIdle_lcdofftimer_t0 = 0;
long	nIdle_sleeptimer_t1 = 0;
long	nIdle_stoptimer_t2 = 0;

static int	nFolderOpenTime;
static int	s_nELTime;
static int	s_nCurLCDBracklight;
static int	s_nExtOldMode;
static int	s_nExtLockTime;
static int	s_nMsgPeekTime;						// 2005-12-25 Christmas!

/* variables related to SNOOZE */
static int s_SnoozeInterval;        // minute
static long s_SnoozeTime;   // sec
bool g_Snooze;            // sec

//static volatile int	s_hasAlarmToNotify = 0;	// 2005-11-06
//static volatile int	s_alarmActivated = 0;	// 2005-11-06
int	qidUSB;
int	nQkey;
UsbStrgMsgBuf_t* pMsgBuf;
int	g_qidMusic;

static long CP_ProgressedUTime;
static long FB_ProgressedUTime;
static long VP_ProgressedUTime;
static long VS_ProgressedUTime;
static long VSF_ProgressedUTime;
static long PrgsUT_SoundPaletteCtrlHide;
static long PrgsUT_SoundPaletteCtrlHideTouchApplyTimer = 0;
static long TV_AutoScroll_PgrsUT1;
static long TV_AutoScroll_PgrsUT2;
static long TV_AutoScroll_PgrsUT3;
static long its = 0;
//static int ThisTime;
extern void showbackground (char *bkgnd);

#include <Lib/LibImage.h>
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern int nCheckUpdate;
extern SysParameter_t	TaviSysParam;

CMsgBox* pMbx;

int MSG_AddToQueue( TMsgQueue* tQueue, TMsgEntry* tMsg );
int MSG_DeleteFromQueue( TMsgQueue* tQueue, TMsgEntry* tMsg );

#define BATTERY_LEVEL_SAMPLE_SIZE	10	// This SHOULB be at least 2

#define		BAT_LVL_0		35500
#define		BAT_LVL_1		35800
#define		BAT_LVL_2		36600
#define		BAT_LVL_3		37400
#define		BAT_LVL_4		38700
#define		BAT_LVL_RECRG	40300
#define		BAT_LVL_FULL	44000
#define		BAT_LVL_HIGH	99999


int CalcBatteryLevel( int h )
{

	extern int 	g_debug_battery;
	extern int		g_debug_no_shutdown_on_low_battery;

	int	ac_detected;
	int 	adc_level;
	int 	nLevelSym;
	int 	level;
	int 	mLevel;

	static int 	called				= 0;
	static int 	nLastBatteryLevel	= 0;
	static int 	nBatteryEmptyCount	= 0;
	static int	nIdx				= 0;		// index for array of nBatteryLevelList[]
	static int 	nNoOfQueued		= 0;		// number of stored data
	static int 	sum					= 0;		// sum of Queued datas
	static int nBatteryLevelList[ BATTERY_LEVEL_SAMPLE_SIZE ];

	if ( h == 0 ) {
		sum = 0;
		nNoOfQueued = 0;
		nIdx = 0;
		}
	
	called++;

	// Calcurate Battery level & display level

	ioctl( handle_key, TAVI_KEY_IOCG_BATTERY, &adc_level );

	if( adc_level < 0 ) {
		// XXX
		// failed?
		// use current value

		if (nNoOfQueued == 0) {
			nLevelSym = (int)CTopPanel::BATTERY_LEVEL_1;
			}
		}
	else {
		// Convert into Voltage. ex) 36300 --> 3.63 V
		level = adc_level * ADC_10BIT_UNIT * 2;
//		if (0 && level > BAT_LVL_4 + 900) { level = BAT_LVL_4 + 900; }

		/* ### calculate smart mean value :-) ### */
		if ( nNoOfQueued < BATTERY_LEVEL_SAMPLE_SIZE )
			nNoOfQueued++;
		else
			sum -= nBatteryLevelList[nIdx];
		
		nBatteryLevelList[nIdx] = level;
		sum +=nBatteryLevelList[nIdx];

		nIdx++;
		nIdx%=BATTERY_LEVEL_SAMPLE_SIZE;
		
		mLevel = sum / nNoOfQueued;

#define	IRANGE(x, low, high)	( (low) <= (x) && (x) < (high) )

		if ( IRANGE(mLevel, 0, BAT_LVL_1) ) {		// 300
			nLevelSym = CTopPanel::BATTERY_LEVEL_0;
		} else if ( IRANGE(mLevel, BAT_LVL_1, BAT_LVL_2) ) {		// 900
			nLevelSym = CTopPanel::BATTERY_LEVEL_1;
		} else if ( IRANGE(mLevel, BAT_LVL_2, BAT_LVL_3) ) {		// 900
			nLevelSym = CTopPanel::BATTERY_LEVEL_2;
		} else if ( IRANGE(mLevel, BAT_LVL_3, BAT_LVL_4) ) {		// 900
			nLevelSym = CTopPanel::BATTERY_LEVEL_3;
		} else if ( IRANGE(mLevel, BAT_LVL_4, BAT_LVL_HIGH ) ) {
			nLevelSym = CTopPanel::BATTERY_LEVEL_4; 			// max 410
		}

		if ( mLevel >= BAT_LVL_0 ) {
			nBatteryEmptyCount = 0;
			}
		else {
			nBatteryEmptyCount++;
			}
		}

	if (!g_debug_no_shutdown_on_low_battery) {
		int nStopCount;

		switch( pTAVI->ssTaskId ) {
			case TASK_ID_FILEMANAGER:
				nStopCount = TAVI_BATTERY_EMPTY_CNT04;	// 12 sec 
				break;
			case TASK_ID_VIDEO:
				nStopCount = TAVI_BATTERY_EMPTY_CNT20;	// 60 sec
				break;
			default:
				nStopCount = TAVI_BATTERY_EMPTY_CNT20;	// 60 sec
				break;
			}
		if ( nBatteryEmptyCount >= nStopCount ) {
			return -1;	// Emergency!!!
			}
		}
    
#if 1 // for debug. DO NOT DELETE IT!!!
	if ( g_debug_battery) {
		char buf[100];
		sprintf( buf, "#%3d: Cur/Mean: %5d/%5d D: %+3d S: <%d>", called, level, mLevel, level - mLevel, nNoOfQueued );
		uchar_t wbuf[100];
		str2ustr( wbuf, buf );
              pMpegDecoder->RenderText_WithoutRefresh(0, 0, 2, 200-8, 300,16);
		pT->DrawUC16Text( wbuf, 2, 200-8, _WHITE, 12, DT_TRANSPARENCY);
		op_Flush();
		}
#endif
	if ( g_MiscCtrl->ACDetected() ) {
		nLevelSym = (int)CTopPanel::BATTERY_LEVEL_AC_ON;
		}
	return (nLastBatteryLevel = nLevelSym);
	
}

static void Ext_ShowKeyLockMsg( int msg )
{
	int mode;
	if( g_MiscCtrl->FolderOpen() == false ) {
		mode = g_ExtLCD->GetCurrentMode();
		if( mode != CExtLCD::EXT_STAGE_MESSAGE ) s_nExtOldMode = mode;
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_MESSAGE );
		g_ExtLCD->ReservedMessage( msg );
		g_ExtLCD->Update();
		s_nExtLockTime = gettime_sec();
	}
}

static void Ext_Restore( void )
{
	if( s_nExtOldMode == -1 ) return;
	g_ExtLCD->ChangeMode( (CExtLCD::ExtLCD_Mode)s_nExtOldMode );
	if( g_MiscCtrl->FolderOpen() == false ) {
		g_ExtLCD->Update();
	}
	s_nExtLockTime = 0;
	s_nExtOldMode = -1;
}

static void SetAutoLCD_Brightness( void )
{
	int level;
	
	if( ioctl( handle_key, TAVI_KEY_IOCG_CDS, &level ) < 0 ) return;
	level = ~level & 0x000003ff;
	level -= level%100;
	level /= 100;
	if( level > 9 ) level = 10;
	if( s_nCurLCDBracklight != level ) {
		g_MiscCtrl->LCDBrightness( level );
		s_nCurLCDBracklight = level;
	}
}

int  do_idle (int  ssTaskId, TMsgEntry* tMsg, int now)
{
	int	nReturn=0;

	if (nIdle_lcdofftimer == 0)
	{
		nIdle_lcdofftimer++;		// only one entered befor cleared to 0
		nIdle_lcdofftimer_t0 = now;
		DEBUGMSG(1,(" lcd off : nIdle_lcdofftimer_t0 = %d\n", nIdle_lcdofftimer_t0) );
	}
	if (nIdle_sleeptimer == 0)
	{
		nIdle_sleeptimer++;		// only one entered befor cleared to 0
		nIdle_sleeptimer_t1 = now;
		DEBUGMSG(1,(" sleep down : nIdle_sleeptimer_t1 = %d\n", nIdle_sleeptimer_t1) );
	}
	if (nIdle_stoptimer == 0)
	{
		nIdle_stoptimer++;		// only one entered befor cleared to 0
		nIdle_stoptimer_t2 = now;
		DEBUGMSG(1,(" idle down : nIdle_stoptimer_t2 = %d\n", nIdle_stoptimer_t2) );
	}
	
	if ( TaviSysParam.power.lcdofftimer > 0 &&  toggle_display == 0 )
	{
		if ( ((now - nIdle_lcdofftimer_t0) >= (int)TaviSysParam.power.lcdofftimer) && 
			g_MiscCtrl->GetLCDOnOff() )
		{	// excute lcd off
			DEBUGMSG(1,("do_idle:now=%d, nIdle_lcdofftimer_t0=%d, lcdoff time=%d\n",now,nIdle_lcdofftimer_t0,now - nIdle_sleeptimer_t1 ) );
			nIdle_lcdofftimer_t0 = now;						// reset nIdle_lcdofftimer_t0 to now	
			if ( !g_MiscCtrl->ACDetected() ) {
				if( pTAVI->isVideoPlaying == 0  					// when video not played.
					&& !TV_IsViewingOnForeground()   		//does not view of text
					&& !IV_IsViewingOnForeground() 			// does not view of image
					)
				{
					g_MiscCtrl->MainLCDOnOff(FALSE);
				}
			}
		}
	}
	else {
	  nIdle_lcdofftimer_t0 = now;
	}
	
	if ( TaviSysParam.power.sleeptimer > 0 )
	{
		if ((now - nIdle_sleeptimer_t1) >= (int)TaviSysParam.power.sleeptimer*60 )
		{	// excute sleep power down
			DEBUGMSG(1,(" Msg.cpp : do_idle : sleep time=%d\n", 	now - nIdle_sleeptimer_t1 ) );
			tMsg->ulMsgId	= MSG_BUTTON_UP;
			tMsg->ssToTaskId	=  ssTaskId;
			tMsg->usParam1	= TAVI_KEY_SHUTDOWN;
			tMsg->usParam2	= 0;
			g_PowerOff = TRUE;
			TaviSysParam.power.sleeptimer = 0;			// reset registor of steeptimer
			nReturn = 1;
		}
	}
	else {
	  nIdle_sleeptimer_t1 = now;
	}
	
	if ( TaviSysParam.power.stoptimer > 0 )
	{
		if ((now - nIdle_stoptimer_t2) >= (int)TaviSysParam.power.stoptimer*60 )
		{	// excute idle shutdown
			DEBUGMSG(1,(" Msg.cpp : do_idle : stop time=%d\n", 	now - nIdle_stoptimer_t2 ) );			
			if( pTAVI->isVideoPlaying == 0 && 
				(AP_GetPlayState()==AUDIO_STATE_PAUSE || AP_GetPlayState()==AUDIO_STATE_STOP )	 &&
				!REC_IsRecording()	)	// when video ,audio & record not played.
			{
				nIdle_stoptimer_t2 = now;
				if ( g_MiscCtrl->ACDetected() )
				{
					if ( !Extra_Clock_isDisplaying( ) ) {
						MSG_Send( ssTaskId, MSG_BUTTON_UP,  TAVI_KEY_LEXIT, 0 );
						MSG_Send( TASK_ID_MAIN, MSG_EXECUTE_TASK,  MENU_SLOT7, 0 );	// excute etc menu
						MSG_Send( TASK_ID_ETC, MSG_BUTTON_UP,  TAVI_KEY_ENTER, 0 );	// enter clock
						}
				} else {
					tMsg->ulMsgId	= MSG_BUTTON_UP;
					tMsg->ssToTaskId	= ssTaskId ;
					tMsg->usParam1	= TAVI_KEY_SHUTDOWN;
					tMsg->usParam2	= 0;
					g_PowerOff = TRUE;
					nReturn =  1;
				}
			}
		}
	}
	else {
	  nIdle_stoptimer_t2 = now;
	}

	// turn off EL backligh after 5sec. when a folder is closed or jog key pressed.
	if( g_MiscCtrl->EXT_EL_Backlit_Status() ) {
		if( (now - nFolderOpenTime) > TURNOFF_EL_TIME ) {
			g_MiscCtrl->EXT_EL_Enable( false );
		}
	}

	if( g_ExtLCD->GetCurrentMode() == CExtLCD::EXT_STAGE_MESSAGE ) {
		if( abs(now-s_nExtLockTime) > TURNOFF_EXTLCD_MSG ) {
			Ext_Restore();
			ClearKeyBuffer();
		}
	}

	g_ExtLCD->IdleAction();
	
	return nReturn;
}

static bool TurnOnEL( int key )
{
	/*
		true:	key has precessed
		false:	key has NOT processed here
	*/
	if( g_MiscCtrl->FolderOpen() ) {
		return true;
	}
	
	if( ( key >=TAVI_KEY_JOG1_DOWN && key <= TAVI_KEY_JOG2_UP)
			|| ( key >= TAVI_KEY_JOG1_LDOWN && key <= TAVI_KEY_JOG2_LUP ) ) {
		nFolderOpenTime = gettime_sec();
		g_MiscCtrl->EXT_EL_Enable( true );
	}

	return true;
}

void do_ResetLcdOff()
{
	nIdle_lcdofftimer = 0;
	nIdle_stoptimer = 0;
	if( g_MiscCtrl->FolderOpen() && toggle_display == 0 )  {
		g_MiscCtrl->MainLCDOnOff(TRUE);		// LCD on
	}
}

void SendMelonGetDrmInfo(void)
{
	// Send Melon security infomation
	AudioCmd Cmd;
	QBUF_t* buf = ( QBUF_t* )&Cmd;
	Cmd.lMsgID = MSG_SEND_ID; // don't set 0
	Cmd.lLength = sizeof(AudioCmd) - 4;
	Cmd.lCmd = AUDIO_CMD_MELONUPDATEINFO;
	Cmd.szArg[0] = 'x';
	SysMSGSend( g_qidMusic, buf, buf->lLength, 0 );
	printf("--- 0th :: pvp : MSG_Init : AUDIO_CMD_MELONUPDATEINFO\n" );
}

int MSG_Init( TMsgInitParam* tParam )
{
	int		idx_pipe;
	int		idx_entry;
	struct rtc_time rtime;
    char buf[100];
    uchar_t wbuf[100];

    for (idx_pipe = 0; idx_pipe < MSG_PIPE_NUM ; idx_pipe++) {
    	tMsgQueueList[idx_pipe].head	= -1; 
    	tMsgQueueList[idx_pipe].tail	= -1; 
    	for (idx_entry = 0 ; idx_entry < MSG_QUEUE_DEPTH ; idx_entry++) {
    		tMsgQueueList[idx_pipe].queue[idx_entry].ulMsgId		= 0; 
    		tMsgQueueList[idx_pipe].queue[idx_entry].ssToTaskId		= -1;
    		tMsgQueueList[idx_pipe].queue[idx_entry].usParam1		= 0;
    		tMsgQueueList[idx_pipe].queue[idx_entry].usParam2		= 0;
    	}
    }

    /*
        Open RTC
    */
    handle_rtc = open( RTC_DRIVER_PATH, O_RDONLY );
    if ( handle_rtc  < 0 ) {
    	printf( "failed to open /dev/misc/rtc\n" );
    } else {
    	time_t tm;
    	struct tm* tt;
    	ioctl( handle_rtc, RTC_RD_TIME, &rtime );

	close (handle_rtc);
    	tm = mktime( (struct tm*)&rtime );
    	stime( &tm );

    	// Apply timezone
    	putenv(TaviSysParam.etc.timezoneString);
    	tzset();
    }
    /*
        Open touchpad device
    */
    if( mep_init() < 0 ) {
    	fprintf( stderr, "failed to initialize touchpad\n" );
    }
    if( mep_enable_autoreport( 0 ) == 0 ) {
    	printf( "failed to enable auto report\n" );
    }

    // Initialize timer
    if( InitTimer() < 0 ) {
    	printf( "failed to initialize timer\n" );
    }

    do_ResetLcdOff();

    s_nCurLCDBracklight	= TaviSysParam.etc.lcdbright;

    nFolderOpenTime		= gettime_sec();

    pMbx = new CMsgBox();

     
    /*
        Open key device
    */
    handle_key = open( KEY_DRIVER_PATH, O_RDWR|O_NONBLOCK );
    if ( handle_key < 0 ) {
    	printf( "failed to open device file\n" );
    }

    return TRUE;
}

int MSG_Exit()
{
	return TRUE;
}

int MSG_Get( int ssTaskId,  TMsgEntry* tMsg )
{
	return FALSE;
}

int SpeakerOnOff( bool fOnOff )
{
	TaviSysParam.sound.speaker_mute=fOnOff;
	g_MiscCtrl->AMPEnable( !fOnOff);
	g_SndCtrl->SpeakerMute( fOnOff );
	g_SndCtrl->EarphoneMute( !fOnOff );
	g_TopPanel->SetSpeaker( !fOnOff );
	g_TopPanel->Update();
}

#define TIMER_ACTIVATED 0
#define TIMER_NOT_FOUND (-1)

static bool _CheckAlarm( void )
{
	int alarm = 0;
	int handle_rtc;

	handle_rtc = open( RTC_DRIVER_PATH, O_RDWR );
	if ( handle_rtc  < 0 ) {
		}
	else {
		ioctl( handle_rtc, RTC_CHECK_INT, &alarm );
		close ( handle_rtc );
		}
    return (bool)alarm;
}

static bool _ClearAlarm( void )
{
	bool success = true;
	int handle_rtc;

	handle_rtc = open( RTC_DRIVER_PATH, O_RDWR );
	if ( handle_rtc  < 0 ) {
		success = false;
		}
	else {
		if( ioctl( handle_rtc, RTC_CLEAR_INT ) < 0 ) 
			success = false;
		close( handle_rtc );
		}
	
	return success; 
}

static int _CheckTimer( int TaskId, TMsgEntry* tMsg, int TimerId, long* pRegUtime, long* pPrgrsUtime, int DelayTime )
{
	if( *(pRegUtime) > 0 )
		{

		*(pPrgrsUtime) = gettime_sec() -*(pRegUtime);

		if( *(pPrgrsUtime) > DelayTime )
			{
			tMsg->ulMsgId 		= MSG_TIMER;
			tMsg->ssToTaskId		= TaskId;
			tMsg->usParam1		= DelayTime;
			tMsg->usParam2		= TimerId;
			*(pRegUtime) 		= 0;
			return TIMER_ACTIVATED;
			}
		
		else if( its > 20 ) 
			{ 
			its = 0; 
			}
		}

	return TIMER_NOT_FOUND;
}


int AppTimerCheck(int ssTaskId,  TMsgEntry* tMsg)
{
	// Common Used
	if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_FB_SCROLL, &FB_RegisteredUTime, &FB_ProgressedUTime, 2 )==0 ) return TIMER_ACTIVATED;

	if( ssTaskId == TASK_ID_VIDEO )
		{
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_CP_HIDE, &CP_RegisteredUTime, &CP_ProgressedUTime, 3 )==0 ) return TIMER_ACTIVATED;
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_VP_BASIC, &VP_RegisteredUTime, &VP_ProgressedUTime, 5 )==0 ) return TIMER_ACTIVATED;
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_VP_SEEK, &VS_RegisteredUTime, &VS_ProgressedUTime, 1 )==0 ) return TIMER_ACTIVATED;
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_VP_SEEK_FREE, &VSF_RegisteredUTime, &VSF_ProgressedUTime, 3 )==0 ) return TIMER_ACTIVATED;
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_SOUND_PALETTE, &RegUT_SoundPaletteCtrlHide, &PrgsUT_SoundPaletteCtrlHide, 15 )==0 ) return TIMER_ACTIVATED;
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_SOUND_PALETTE_TOUCH_APPLY, &RegUT_SoundPaletteCtrlHideTouchApplyTimer, &PrgsUT_SoundPaletteCtrlHideTouchApplyTimer, 1 )==0 ) return TIMER_ACTIVATED;
		}
	
	else if( ssTaskId == TASK_ID_TEXT )
		{
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_TV_AUTOSCROLL_SLOW, &TV_AutoScroll_RegUT1, &TV_AutoScroll_PgrsUT1, TIMER_VALUE_TV_AUTOSCROLL_SLOW )==0 ) return TIMER_ACTIVATED;
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_TV_AUTOSCROLL_MID, &TV_AutoScroll_RegUT2, &TV_AutoScroll_PgrsUT2, TIMER_VALUE_TV_AUTOSCROLL_MID )==0 ) return TIMER_ACTIVATED;
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_TV_AUTOSCROLL_FAST, &TV_AutoScroll_RegUT3, &TV_AutoScroll_PgrsUT3, TIMER_VALUE_TV_AUTOSCROLL_FAST )==0 ) return TIMER_ACTIVATED;	
		}

	else if( ssTaskId == TASK_ID_MUSIC )
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_SOUND_PALETTE, &RegUT_SoundPaletteCtrlHide, &PrgsUT_SoundPaletteCtrlHide, 3 )==0 ) return TIMER_ACTIVATED;
		if( _CheckTimer( ssTaskId, tMsg, TIMER_ID_SOUND_PALETTE_TOUCH_APPLY, &RegUT_SoundPaletteCtrlHideTouchApplyTimer, &PrgsUT_SoundPaletteCtrlHideTouchApplyTimer, 2 )==0 ) return TIMER_ACTIVATED;
		
	return TIMER_NOT_FOUND;

}

int AppEventCheck( int ssTaskId,  TMsgEntry* tMsg )
{
	static long LastCheckTime;
	static int level_saved;
	int level;
//	char nDirty;
//printf("----------AppEventCheck\n");
	if( abs(gettime_sec() - LastCheckTime) > 10 || !LastCheckTime ) 
		{ 


		////////////////////////////////////////////////////////////////////////
		// Update Ext lcd time
		if( g_ExtLCD->GetCurrentMode() == CExtLCD::EXT_STAGE_READY ) 
			g_ExtLCD->Update(); 

		////////////////////////////////////////////////////////////////////////
		// Update Time
		if( pLFBrowser->isVisible() || pCp->isVisible)
			{
			g_TopPanel->DrawTime(TRUE);
			pMpegDecoder->RefreshOsd();
			}

		////////////////////////////////////////////////////////////////////////
		// Update Battery Level
		level = CalcBatteryLevel();

		if( level > 0 )
			pTAVI->BatteryLevel = level;
		
		if( level < 0 ) 
			{
			// Send low-battery event
			tMsg->ulMsgId = MSG_BUTTON_UP;
			tMsg->ssToTaskId = ssTaskId;
			tMsg->usParam1	= TAVI_KEY_SHUTDOWN;
			tMsg->usParam2	= 0;
			g_PowerOff = true;
			g_ExitState = ExitState_Lowbattery;
			return 0;
			}

		else 
			{
			if ( level != level_saved ) 
				{
				g_TopPanel->SetBatteryLevel( (CTopPanel::BatteryLevel)level );
				g_ExtLCD->SetBatteryLevel( (CExtLCD::ExtLCD_Battery)level );

				tMsg->ulMsgId 		= MSG_BATTERY;
				tMsg->ssToTaskId		= ssTaskId;
				tMsg->usParam1		= level;
				tMsg->usParam2		= 0;
				
				level_saved=level;
				}
			}

		if( tMsg->ulMsgId == MSG_BATTERY ) return 0;
		
		LastCheckTime = gettime_sec();
		
	}

	return -1;
}

static int _MSG_Battery_Check_Update(int ssTaskId, TMsgEntry *tMsg)
{
	/* 
		return value:
		1: go ahead
		0: stop processing and return immediately!
	*/
	static char 	*funcName				= "_MSG_Battery_Check_Update";
	static long	nBatteryLastCheckTime		= 0;
	static int		level_saved				= 0;

	long			cur_time				= s_nMsgPeekTime;
	int			level;

	if ( abs(cur_time - nBatteryLastCheckTime) < BATTERY_CHECK_INTERVAL ) {
		return GO_AHEAD;
	}

	// Update Ext LCD Time
	if( g_ExtLCD->GetCurrentMode() == CExtLCD::EXT_STAGE_READY ) {
		g_ExtLCD->Update();
	}

	level	= CalcBatteryLevel();

	if ( level > 0 ) {
		pTAVI->BatteryLevel = level;			
	}

	if ( level < 0 ) {
		/*
			In case of Battery Low Event,
			send (Button Up, Key Shutdown) event
		*/
		tMsg->ulMsgId		= MSG_BUTTON_UP;
		tMsg->ssToTaskId	= ssTaskId;
		tMsg->usParam1		= TAVI_KEY_SHUTDOWN;
		tMsg->usParam2		= 0;

		g_PowerOff			= true;
		g_ExitState			= ExitState_Lowbattery;

		//printf("[%s] battery low event at level: %d\n", funcName, level);

		return STOP_RETURN;

	} else if ( level != level_saved ) {
		g_TopPanel->SetBatteryLevel( (CTopPanel::BatteryLevel)level );
		g_ExtLCD->SetBatteryLevel( (CExtLCD::ExtLCD_Battery)level );
					
		tMsg->ulMsgId 		= MSG_BATTERY;
		tMsg->ssToTaskId	= ssTaskId;
		tMsg->usParam1		= level;
		tMsg->usParam2		= 0;

		level_saved		= level;
		//printf("[%s] generating battery event %d\n", funcName, level);
	}

	nBatteryLastCheckTime = cur_time;

	if ( g_TopPanel->IsShow() ) g_TopPanel->Show();

	if ( tMsg->ulMsgId == MSG_BATTERY ) {
		return STOP_RETURN;
	}

#if 0
	static long nBatteryCheck2;

	// for operating time check
	if ( (cur_time - nBatteryCheck2) >= 300 ) { // 5 min
		char buf[256];
		int len;
		int nBytes;
		int nBattery;
		int fd;
		int level;
		time_t t;
		
		t = time( NULL );
		struct tm* tm = localtime( &t );

		ioctl( handle_key, TAVI_KEY_IOCG_BATTERY, &nBattery );
		level = 2000*((float)nBattery*ADC_10BIT_UNIT);

        	DEBUGMSG(1,("[1]FR_WriteConfigFile : success to configfile open\n") );

		fatfs_fopen( (RMuint8*)"/hd1/Batter.txt", _O_WRONLY|_O_APPEND|_O_CREAT, (RMuint32*)&fd );			
		len = sprintf( buf, "Time : %04ld-%02ld-%02ld, %02ld:%02ld:%02ld, Battery : %d [mV], ssTaskId:%d \r\n",
			 (1900+tm->tm_year), (tm->tm_mon+1), tm->tm_mday,
		        tm->tm_hour, tm->tm_min, tm->tm_sec,
			level, ssTaskId
			);
		fatfs_fwrite( (RMuint32)fd, (RMuint8*)buf, len, (RMuint32*)&nBytes );
		fatfs_fclose((RMuint32)fd );
		nBatteryCheck2 = cur_time;
	}
#endif	

	return GO_AHEAD;	// go on message processing
}

static int _MSG_LCD_Check(int ssTaskId,  TMsgEntry* tMsg)
{
	/*
		global vars: nIsLCDoff, nIdle_lcdofftimer
		extern vars: TaviSysParam
	*/
	if ( g_MiscCtrl->FolderClosed() )
		return GO_AHEAD;
	
	if ( (TaviSysParam.power.lcdofftimer > 0 || TaviSysParam.power.stoptimer > 0) ) {
            nIdle_lcdofftimer = 0;
            nIdle_stoptimer = 0;			
            if( toggle_display == 0 ) {
                if ( !g_MiscCtrl->GetLCDOnOff() ) {
                    do_ResetLcdOff();
                    return STOP_RETURN;
                }
            }
	}
	return GO_AHEAD;
}

static int _MSG_Check_Key(int ssTaskId,  TMsgEntry* tMsg)
{
	struct keystruct keystruct;

	if ( read( handle_key, &keystruct, sizeof(keystruct) ) <= 0 ) {
		return GO_AHEAD;
	}

	TurnOnEL( keystruct.key );

    /*
        Do not process any key when the main LCD is turned off.
    */
	if ( _MSG_LCD_Check(ssTaskId, tMsg) == STOP_RETURN )	{
		return GO_AHEAD;
	}
	
	if( g_Hold == 0 ) {	/* Not Locked */
		if( keystruct.key == TAVI_KEY_SHUTDOWN ) {
			// Send power-off event
			tMsg->ulMsgId		= MSG_BUTTON_UP;
			tMsg->ssToTaskId	= ssTaskId;
			tMsg->usParam1		= TAVI_KEY_SHUTDOWN;
			tMsg->usParam2		= 0;

			g_PowerOff			= true;
			g_ExitState			= ExitState_Shutdown;

			return STOP_RETURN;
		}

		//DEBUGMSG(1,("--MSG_Peek() - Key Read: 0x%08x \n", key));

		if ( keystruct.press ) {	/* 1 */
			tMsg->ulMsgId 		= MSG_BUTTON_UP; // must be changed to MSG_BUTTON_DOWN ?
			tMsg->ssToTaskId	= ssTaskId;
			tMsg->usParam1		= keystruct.key;
			tMsg->usParam2		= keystruct.press;

			if( keystruct.key == TAVI_KEY_LEQ ) {
				g_TopPanel->ToggleMute();
				if( g_TopPanel->IsMute() ) { // mute
					g_MiscCtrl->AMPEnable( false );
					g_SndCtrl->DACMute( true );
				}
				else {
					g_SndCtrl->DACMute( false );
					g_MiscCtrl->AMPEnable( true );
				}
				if( g_TopPanel->IsShow() ) {
					g_TopPanel->Show();
				} else {
					g_TopPanel->Update();
				}
			} else if( keystruct.key == TAVI_KEY_HOLD ) {
				g_Hold = true;
				g_TopPanel->SetHold( g_Hold );
				g_TopPanel->Show();
				Ext_ShowKeyLockMsg( CExtLCD::EXT_MSG_KEY_LOCKED );

				return STOP_RETURN;
				
			}
		} else {			/* 0 */
			tMsg->ulMsgId 		= MSG_BUTTON_REAL_UP;
			// this value must be changed to MSG_BUTTON_UP
			tMsg->ssToTaskId	= ssTaskId;
			tMsg->usParam1		= keystruct.key;
			tMsg->usParam2		= keystruct.press;
		}

		return STOP_RETURN;
	} else {	/* Locked */
		tMsg->ulMsgId 		= MSG_BUTTON_UP;
		tMsg->ssToTaskId	= ssTaskId;
		tMsg->usParam1		= TAVI_KEY_HOLD;	/* default */
		tMsg->usParam2		= 0;

        /* ignores relese event */
		if( keystruct.key == TAVI_KEY_HOLD && keystruct.press ) {
			g_Hold = false;

			tMsg->usParam1	= keystruct.key;
			g_TopPanel->SetHold( g_Hold );
			g_TopPanel->Show();
			Ext_ShowKeyLockMsg( CExtLCD::EXT_MSG_KEY_UNLOCKED );
		} else {
			Ext_ShowKeyLockMsg( CExtLCD::EXT_MSG_KEY_LOCKED );
		}
		return STOP_RETURN;
	}

	return GO_AHEAD;
}

static int _MSG_Check_TouchPad(int ssTaskId,  TMsgEntry* tMsg)
{
	int nClick;
	int nStart;
	int nStep = mep_get_step( &nClick, &nStart );

	if ( nStep == -2 ) {	// No data
		return GO_AHEAD;
	}

	if ( _MSG_LCD_Check(ssTaskId, tMsg) == STOP_RETURN ) {
		return STOP_RETURN;
	}
	
	if ( g_Hold ) {
		if ( nStart ) {
			tMsg->ulMsgId		= MSG_BUTTON_UP;
			tMsg->ssToTaskId	= ssTaskId;
			tMsg->usParam1		= TAVI_KEY_HOLD;
			tMsg->usParam2		= 0;

			return STOP_RETURN;
		}
	} else {
		if ( nStep ) BuzzerTick();	// Tick, Tick, Tick !
		tMsg->ulMsgId		= MSG_TOUCHPAD;
		tMsg->ssToTaskId	= ssTaskId;
		tMsg->usParam1		= nStep;
		tMsg->usParam2		= nClick;

		return STOP_RETURN;
	}

	return GO_AHEAD;
}

static int _MSG_Check_Misc_Event(int ssTaskId,  TMsgEntry* tMsg)
{
	/*
		extern vars: toggle_display
	*/
	char 		nDirty;
	int         ret = GO_AHEAD;

	if( !g_MiscCtrl->Update( &nDirty ) ) {	// do nothing
		return GO_AHEAD;
	}

 	extern bool	g_KillAll;
	int			level;

	if ( nDirty & (1 << AC_DETECT) ) {
		g_MiscCtrl->ClearBit( AC_DETECT );

		level = CalcBatteryLevel(0);

		if( level < 0 ) {
			// Send low-battery level event.
			tMsg->ulMsgId		= MSG_BUTTON_UP;
			tMsg->ssToTaskId		= ssTaskId;
			tMsg->usParam1		= TAVI_KEY_SHUTDOWN;
			tMsg->usParam2		= 0;

			g_PowerOff			= true;
			g_ExitState			= ExitState_Lowbattery;
			g_KillAll				= true;

            ret = STOP_RETURN;
            goto __end_misc_event;
        }
		else {
			pTAVI->BatteryLevel = level;
        }

		TAVI_WakeUp(10);	// power save
		
		g_ExtLCD->SetBatteryLevel( (CExtLCD::ExtLCD_Battery)level );
		g_ExtLCD->Update(); 			

		g_TopPanel->SetBatteryLevel( (CTopPanel::BatteryLevel)level );

		if( g_TopPanel->IsShow() ) {
		    g_TopPanel->Show();
		}
		
		tMsg->ulMsgId		= MSG_BATTERY;
		tMsg->ssToTaskId		= ssTaskId;
		tMsg->usParam1		= level;
		tMsg->usParam2		= 0;

		ret = STOP_RETURN;
		goto __end_misc_event;
    }

	if( nDirty & (1 << FOLDER_OPEN) ) {
        g_MiscCtrl->ClearBit( FOLDER_OPEN );
		if( g_MiscCtrl->FolderOpen() ) {
			if ( toggle_display == 0 ) {
				// when TV out,  LCD must be off
				g_MiscCtrl->MainLCDOnOff( true );
			}

			g_MiscCtrl->EXT_EL_Enable( false ); // turn off sub lcd backlight

			// UNLOCK KEY CHANNEL 1 
			int ch = 1;
			ioctl( handle_key, TAVI_KEY_IOCS_CHANNEL_UNLOCK, &ch );
		} else {
			g_MiscCtrl->MainLCDOnOff( false );
			g_MiscCtrl->EXT_EL_Enable( true ); 

			nFolderOpenTime = s_nMsgPeekTime;
			// LOCK KEY CHANNEL 1 
			int ch = 1;
			ioctl( handle_key, TAVI_KEY_IOCS_CHANNEL_LOCK, &ch );
		}

		tMsg->ulMsgId		= MSG_FOLDER;
		tMsg->ssToTaskId	= ssTaskId;
		tMsg->usParam1		= g_MiscCtrl->FolderOpen();

		ret = STOP_RETURN;
		goto __end_misc_event;
	}

	if( nDirty & (1 << RTC_INTR) ) {
    	g_MiscCtrl->ClearBit( RTC_INTR );
		if( _CheckAlarm() ) {
			pTAVI->isAlarmState = 1;
			g_KillAll = true;
			if( ssTaskId ) {
				tMsg->ulMsgId		= MSG_BUTTON_UP;
				tMsg->ssToTaskId	= ssTaskId;
				tMsg->usParam1		= TAVI_KEY_LEXIT;
				tMsg->usParam2		= 0;
      }
			else {
			  TAVI_KillAllTasks();
				tMsg->ulMsgId		= MSG_DRAW;
				tMsg->ssToTaskId		= 0;
				tMsg->usParam1		= 0;
				tMsg->usParam2		= 0;
      }
			ret = STOP_RETURN;
		}
		else {
	    ret = GO_AHEAD;
		}
		_ClearAlarm();

		goto __end_misc_event;
	}

	if( nDirty & (1 << CRADLE_IN) ) {
		int attach = g_MiscCtrl->CradleIN();
            g_MiscCtrl->ClearBit( CRADLE_IN );
		if ( attach == 0 && toggle_display ) {
			// auto return to lcd output if not lcd output
			toggle_display = 0;	// lcd
			pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
			lcd_enable(  g_MiscCtrl->GetValueCtrl_Out() );
			VP_SetScreenDisplayRatio();
			g_MiscCtrl->MainLCDOnOff( TRUE );
		}			

		tMsg->ulMsgId 		= MSG_NOTIFY;
		tMsg->ssToTaskId	= ssTaskId;
		tMsg->usParam1		= NOTIFY_CRADLE;
		tMsg->usParam2		= attach;

		ret = STOP_RETURN;
		goto __end_misc_event;
	} 

	if( nDirty & (1 << HP_DETECT) ) {
        g_MiscCtrl->ClearBit( HP_DETECT );
		if( g_MiscCtrl->HeadphoneIN() ) {
			TaviSysParam.sound.speaker_mute = 1;
			// do something when connected headphone
			g_MiscCtrl->AMPEnable( false );
			g_SndCtrl->SpeakerMute( true );
			g_SndCtrl->EarphoneMute( false );
			g_TopPanel->SetSpeaker( false );
			if( TaviSysParam.sound.eq == SOUND_eXTX_PRESET ) {
				if ( pTAVI->isVideoPlaying ) 
					XEN_Preset( SOUND_REX_WIDE_PRESET );
				else if (AP_GetPlayState() !=AUDIO_STATE_STOP )
					pMpegDecoder->XEN_Preset( SOUND_REX_WIDE_PRESET );
			}
		} else {
			TaviSysParam.sound.speaker_mute = 0;
			// do something when disconnected headphone
			g_SndCtrl->SpeakerMute( false );
			g_MiscCtrl->AMPEnable( true );
			g_SndCtrl->EarphoneMute( true );
			g_TopPanel->SetSpeaker( true );
			if( TaviSysParam.sound.eq == SOUND_eXTX_PRESET ) {
				if ( pTAVI->isVideoPlaying ) 
					XEN_Preset( SOUND_eXTX_PRESET );
				else if (AP_GetPlayState() !=AUDIO_STATE_STOP )
					pMpegDecoder->XEN_Preset( SOUND_eXTX_PRESET );
			}
			if ( toggle_display ) {
				// auto return to lcd output if not lcd output
				toggle_display = 0;	// lcd
				pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
				lcd_enable(  g_MiscCtrl->GetValueCtrl_Out() );
				if ( pTAVI->isVideoPlaying )			// is playing video
					VP_SetScreenDisplayRatio();
				g_MiscCtrl->MainLCDOnOff( TRUE );
			}
		}

		int attach = g_MiscCtrl->HeadphoneIN();
		tMsg->ulMsgId 		= MSG_NOTIFY;
		tMsg->ssToTaskId		= ssTaskId;
		tMsg->usParam1		= NOTIFY_EARPHONE;
		tMsg->usParam2		= attach;
		if( g_TopPanel->IsShow() ) {
		    g_TopPanel->Show();
        }
		else {
		    g_TopPanel->Update();
		}

		ret = STOP_RETURN;
		goto __end_misc_event;
	}

#ifdef HW0601
	printf("++ nDirty =0x%x, HostIn=%d\n", nDirty, g_MiscCtrl->USBHost_IN() );

	if( nDirty & (1 << HOST_IN) ) {
		if ( g_MiscCtrl->USBHost_IN() ){
			g_MiscCtrl->ClearBit( HOST_IN );
			g_MiscCtrl->HostEnable(TRUE);
			printf("** Host Power On\n");
        }
		else {
			if ( !pTAVI->isTaviWorkingAsUsbHost ) {
				g_MiscCtrl->ClearBit( HOST_IN );
				g_MiscCtrl->HostEnable(FALSE);
				printf("** Host Power Off\n");
            }
        }
    }
#endif

__end_misc_event:
    /* ### OK! Something's happend! ### */
	if ( ret == STOP_RETURN ) {
	    _MSG_LCD_Check(ssTaskId, tMsg);
	}

	return ret;
}
#include <sched.h>

static int _MSG_USB_Client_Connected(int ssTaskId,  TMsgEntry* tMsg)
{
	extern bool g_KillAll;
	extern void checkDebugFlags();

	struct keystruct keystruct;


#ifdef HW0601	
	if( !g_MiscCtrl->USBConnected() || g_MiscCtrl->USBHost_IN() ||pTAVI->isTaviWorkingAsUsbHost) {
		return GO_AHEAD;
	}
#else
	static int nUsbDelay=0;

	if ( g_MiscCtrl->USBHost_IN() ) nUsbDelay = 10;
	else
		if ( nUsbDelay > 0 ) nUsbDelay--;

	if( !g_MiscCtrl->USBConnected() || g_MiscCtrl->USBHost_IN() || nUsbDelay>0 ) {	
		return GO_AHEAD;
		}
#endif



	g_KillAll = true;

	if( ssTaskId == 0 ) {
		char nDirty;

		TAVI_KillAllTasks();
		
		DISABLE_WATCHDOGTIMER;
		
		TAVI_WakeUp(1);		// power save

		ClearOsd();
		showbackground( _IMAGE_BG"/usb-connected.jpg" );
		pT->DrawResText( ML_USB_CONNECT,
			0, 178,  _WHITE, 16, DT_TRANSPARENCY| DT_ALIGN_CENTER );
		pMpegDecoder->RefreshOsd();
		g_MiscCtrl->LCDBrightness( 0 );
#if 1	
// =================== for melon =============================
		// create did.bin for melon
		AudioCmd cmd;
		QBUF_t* buf = ( QBUF_t* )&cmd;
		cmd.lCmd = AUDIO_CMD_MELONCREATEDID;
		cmd.lMsgID = MSG_SEND_ID;
		cmd.lLength = sizeof(cmd)-4;
		strcpy( cmd.szArg, TaviSysParam.SerialNo );
		SysMSGSend( g_qidMusic, buf, buf->lLength, 0 );

		// wait ACK
		AudioCmd* pCmd = &cmd;
		pCmd->lLength = sizeof( struct AudioCmd ) -4;
		AckInfo* pack = ( AckInfo* )pCmd->szArg;
		int iTimeout=300;
		while(iTimeout--) {
			usleep(1000000);
			if( SysMSGRecv( g_qidMusic, buf, MSG_RECV_ID, IPC_NOWAIT ) == QError_NOERROR ) {
				DEBUGMSG(1, ("--- 78th:: pvp : USB_Connected : SysMsgRecv , lCmd=%d, pack->done=%d\n",
					pCmd->lCmd, pack->done) );
				if ( pCmd->lCmd == AUDIO_CMD_ACK ) {
					if ( pack->done ) {
						}
					break;						
					}
				}
			}
		DEBUGMSG(1, ("--- 79th:: pvp : USB_Connected : SysMsgRecv : timeout loop =%d\n", iTimeout ) );

// =================== for melon =============================		
#endif

		// unmount /hd1
		fs_unmountmedia();

		// Tri-state IDE Bus
		*(volatile int *)(0x500B40) = 0x18800;


#if 0
		// ATA Reset
		g_MiscCtrl->ResetATA();

		for( int i=0; i > 100000; i++ ) {
			usleep (10000);
		}
#endif

		TaviSysParam.music.update = 1;
		TaviSysParam.music.updatefolder = 1;
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_USB );
		g_ExtLCD->Clear();
		g_ExtLCD->ReservedMessage( CExtLCD::EXT_MSG_USB_CONNECT );
		g_ExtLCD->Update();

		// Enable USB ATA Bridge
		g_MiscCtrl->USBEnable( true );
		usleep (10000 ); 

		// Loop
		g_MiscCtrl->EXT_EL_Enable( false );
		while( g_MiscCtrl->USBConnected() ) {
			g_MiscCtrl->Update( &nDirty );
			if( nDirty & (1 << FOLDER_OPEN) ) {
				if( g_MiscCtrl->FolderOpen() ) {
					if ( toggle_display == 0 ) {
						// when tv out,  lcd must off
						g_MiscCtrl->MainLCDOnOff( true );
					}
					g_MiscCtrl->EXT_EL_Enable( false ); // turn off sub lcd backlight
				} else {
					g_MiscCtrl->MainLCDOnOff( false );
					g_MiscCtrl->EXT_EL_Enable( false ); 

					nFolderOpenTime = s_nMsgPeekTime;
				}
			}

#if 0
			if( read( handle_key, &keystruct, sizeof(keystruct) ) > 0 ) {
				if( keystruct.key == TAVI_KEY_SHUTDOWN ) {
					// Disable USB ATA Bridge
					g_MiscCtrl->USBEnable( false );
					// Send power-off event
					g_PowerOff = true;
					g_ExitState = ExitState_Shutdown;

					return STOP_RETURN;
				}
			}
#endif
			g_ExtLCD->ScrollText();
			usleep( 100000 );
			if( CalcBatteryLevel() < 0 ) {
				g_PowerOff = true;
				g_ExitState = ExitState_Lowbattery;
				break;
			}
		}
		g_MiscCtrl->ClearBit( USB_CONNECT );
		// Disable USB ATA Bridge
		g_MiscCtrl->USBEnable( false );
//		for(int i=0; i<15; i++ )	usleep(100);

		// Untristate IDE Bus
		*(volatile int *)(0x500B40) = 0x18000;
		*(volatile int *)(0x500C00) = 0x00;

		//BI_SL_CTRL_REG = PBI_MODE_IDE_PADS;
//		for(int i=0; i<15; i++ )	usleep(100);

		// ATA Reset
//		g_MiscCtrl->ResetATA();
            // When display mode is LCD.
            if( !toggle_display ) {
		g_MiscCtrl->MainLCDOnOff( true );
            }

		// mount /HD1
		fs_mountmedia ();
//		for(int i=0; i<30; i++ )	usleep(100);

		ClearOsd();
		showbackground( _IMAGE_BG"/usb-disconnected.jpg" );	
		pT->DrawResText( ML_USB_DISCONNECT, 
					0,
					178,
					_WHITE,
					16,
					DT_TRANSPARENCY|DT_ALIGN_CENTER
					);
		pMpegDecoder->RefreshOsd();
		g_MiscCtrl->LCDBrightness( TaviSysParam.etc.lcdbright );

		// show usb diconnect message into ext lcd
		g_ExtLCD->ReservedMessage( CExtLCD::EXT_MSG_USB_DISCONNECT );
		g_ExtLCD->Clear();
		g_ExtLCD->Update();
			
		// REMOUNT /HD1 for MUSIC PLAYER
//		AudioCmd cmd;
//		QBUF_t* buf = ( QBUF_t* )&cmd;
		cmd.lCmd = AUDIO_CMD_REMOUNT;
		cmd.lMsgID = MSG_SEND_ID;
		cmd.lLength = sizeof(cmd)-4;
		SysMSGSend( g_qidMusic, buf, buf->lLength, 0 );

//		for(int i=0; i<15; i++ )	usleep(50000000);
//		for(int i=0; i<15; i++ )	usleep(100);

		tMsg->ulMsgId 		= MSG_DRAW;
		tMsg->ssToTaskId	= 0;
		tMsg->usParam1		= 0;
		tMsg->usParam2		= 0;

		nCheckUpdate = true;			// check romfs.bin for update

		// change ext lcd mode to ready
		g_ExtLCD->Clear();
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
		g_ExtLCD->Update();

		checkDebugFlags();

		// ¸Þ´º µé¾î°¡¼­ ÃÊ±â Áö¿¬À» ¸·±â À§ÇØ¼­ ÇÑ¹ø å¾²±â ½ÇÇà.
		RMuint32 fd;
		fatfs_dopen( (RMuint8*)_HDD_ROOT, &fd );
		fatfs_dcreate_dir( fd, (RMuint8*)"Music" );
		fatfs_dclose( fd );

		g_KillAll = false;

		// when alarm is occurred. 
		if( _CheckAlarm() ) {
			time_t t;
			struct tm* tm;
			time( &t );
			tm = localtime( &t );
			if( tm->tm_hour == TaviSysParam.alarm.hour ) {
				if( (tm->tm_min-TaviSysParam.alarm.min) < 3 ) {
					// Activate alarm
					pTAVI->isAlarmState = 1;
				}
			}
			// expired alarm
			if( !pTAVI->isAlarmState ) _ClearAlarm();
		}

		// check headphone
		if( g_MiscCtrl->HeadphoneIN() ) {
			g_MiscCtrl->AMPEnable( false );
			g_SndCtrl->SpeakerMute( true );
			g_SndCtrl->EarphoneMute( false );
			g_TopPanel->SetSpeaker( false );
		}
		else {
			g_SndCtrl->SpeakerMute( false );
			g_MiscCtrl->AMPEnable( true );			
			g_SndCtrl->EarphoneMute( true );
			g_TopPanel->SetSpeaker( true );
		}

		// check lcd & key lock
		int ch = 1;
		if( g_MiscCtrl->FolderOpen() ) {
//			if( !toggle_display ) {
//                        g_MiscCtrl->MainLCDOnOff( true );
//                     }
			do_ResetLcdOff();

			ioctl( handle_key, TAVI_KEY_IOCS_CHANNEL_UNLOCK, &ch );
		}
		else {
			if( !toggle_display ) {
                        g_MiscCtrl->MainLCDOnOff( false );
                     }
			ioctl( handle_key, TAVI_KEY_IOCS_CHANNEL_LOCK, &ch );
		}
		
		ENABLE_WATCHDOGTIMER;
		RELOAD_WATCHDOGTIMER;
	} else {
		tMsg->ulMsgId		= MSG_BUTTON_UP;
		tMsg->ssToTaskId	= ssTaskId;
		tMsg->usParam1		= TAVI_KEY_LEXIT;
		tMsg->usParam2		= 0;
	}

	return STOP_RETURN;
}

static int _MSG_USB_Host_Connected(int ssTaskId,  TMsgEntry* tMsg)
{
#ifdef HW0601
	int nResult;
	char szTemp[45];

	if( g_MiscCtrl->USBHost_IN() ||  pTAVI->isTaviWorkingAsUsbHost ) {
		nResult = read_message( qidUSB, 1, pMsgBuf );		
		if ( nResult > -1 ) {
			printf("--pMsgBuf->lMsgId =%d : nAction=%d, HostNo=%d, Guid=0x%x\n", 
				pMsgBuf->lMsgId, pMsgBuf->nAction, pMsgBuf->nHostNo, pMsgBuf->nArGid[0] );
			if ( pMsgBuf->lMsgId == 1 ) { // usb storage

				sprintf(szTemp,"/dev/scsi/host%1d/bus0/target0/lun0/part1",pMsgBuf->nHostNo);
					
				if ( pMsgBuf->nAction == 1 ) { // connect usb storage
					nResult = mount (szTemp, "/usb0", "vfat",  MS_NOSUID | MS_NODEV, "uni_xlate");
					printf("-- USBstorage mount = %d\n", nResult );
					pTAVI->isTaviWorkingAsUsbHost = TRUE;
				} else {
					nResult = umount ("/usb0");
					printf("-- USBstorage umount = %d\n", nResult );
					pTAVI->isTaviWorkingAsUsbHost = FALSE;
				}
				if ( nResult == 0 ) {
					tMsg->ulMsgId		= MSG_USBSTORAGE;
					tMsg->ssToTaskId	= ssTaskId;
					tMsg->usParam1		= pMsgBuf->nAction;
					tMsg->usParam2		= pMsgBuf->nHostNo;
					}
				else {
					printf(" USB Host errno=%d\n", errno );
					}
				return STOP_RETURN;
			}
		}
	}
#else
// ---------------------------- old function --------------------------------------------------
	int nResult;

	if( g_MiscCtrl->USBHost_IN() ||  pTAVI->isTaviWorkingAsUsbHost ) {
		nResult = read_message( qidUSB, 1, pMsgBuf );
		if ( nResult > -1 ) {
			printf("-pMsgBuf->lMsgId =%d : nAction=%d, HostNo=%d, Guid=0x%x\n", 
				pMsgBuf->lMsgId, pMsgBuf->nAction, pMsgBuf->nHostNo, pMsgBuf->nArGid[0] );
			if ( pMsgBuf->lMsgId == 1 ) { // usb storage
				char szTemp[45];

				bzero(szTemp,45);
					
				tMsg->ulMsgId		= MSG_USBSTORAGE;
				tMsg->ssToTaskId	= ssTaskId;
				tMsg->usParam1		= pMsgBuf->nAction;
				tMsg->usParam2		= pMsgBuf->nHostNo;

				sprintf(szTemp,"/dev/scsi/host%1d/bus0/target0/lun0/part1",pMsgBuf->nHostNo);
				
				if ( pMsgBuf->nAction == 1 ) { // connect usb storage
					mount (szTemp, "/usb0", "vfat",  MS_NOSUID | MS_NODEV, NULL);
					printf("Msg.cpp : USBstorage mount\n" );
					pTAVI->isTaviWorkingAsUsbHost = TRUE;
				} else {
					umount ("/usb0");
					printf("Msg.cpp : USBstorage umount\n");
					pTAVI->isTaviWorkingAsUsbHost = FALSE;
				}

				printf("-End of USB Host Detection\n");

				return STOP_RETURN;
			}
		}
	}
// ---------------------------- old function --------------------------------------------------
#endif
	return GO_AHEAD;
}

int _MSG_Show_Alarm( int ssTaskId,  TMsgEntry* tMsg )
{
	char sTemp[80];
	int hour	= TaviSysParam.alarm.hour;
	int min	= TaviSysParam.alarm.min;
	int isPM 	= 0;
	int vol;
	CString MusicFullPath;
	AudioCmd* uzCmd = new AudioCmd;
	QBUF_t* buf = ( QBUF_t* )uzCmd;
	int RepeatLeft = TaviSysParam.alarm.repeat;

	if (hour >= 12) {
		isPM = 1;
		if (hour >= 13) hour -= 12;
	}
	
	pTAVI->isAlarmState = 0;

  if( !g_Snooze ) {
      _ClearAlarm();
  }
  g_Snooze = false;
    
	ClearKeyBuffer();

	sprintf(sTemp, "%2d:%02d%s", hour, min, (isPM ? "P" : "A"));
	pMbx->Reset( ML_EXTRA_ALARM, _WHITE, sTemp, 300, 200, NULL, DL_ALARM_MBX_MODE|DL_BTN_OK_CANCEL|DL_OK);
	pMbx->Show();

	if( TaviSysParam.alarm.AlarmType ) { // UserDefine
		MusicFullPath.SetString(TaviSysParam.alarm.UserMusicFullpath, true);
	}
	else { // Default
		MusicFullPath.SetString(_DEFAULT_ALARM, false);
		MusicFullPath.toUTF16();
	}

    // Set Alarm Volume
    vol = g_SndCtrl->GetVolume();
    g_SndCtrl->SetVolume( TaviSysParam.alarm.volume );
    
	// Music Start
 	AP_Play( TYPE_FILE, "", NULL, MusicFullPath );	
	
	/* ### Alarm Main Loop (START) ### */

	s_disableUSBConnect = 1;	// On alarm, disable USB connect temporarily!!!

	for(;;) {
		// Get music player's message
		buf->lLength = sizeof( AudioCmd ) -4;
		if( SysMSGRecv( g_qidMusic, buf, MSG_RECV_ID, IPC_NOWAIT ) == QError_NOERROR ) {
			if( uzCmd->lCmd == AUDIO_CMD_INFO && uzCmd->lState == AUDIO_STATE_STOP ) {
				if( RepeatLeft > 0 ) {
					RepeatLeft--;
					AP_Play( TYPE_FILE, "", NULL, MusicFullPath );
				}
			}
		}
		usleep(5);
		
		if( MSG_Peek( ssTaskId, tMsg ) == 0) {
			usleep(5);
			if( pMbx->MsgProc( ssTaskId, tMsg->ulMsgId, tMsg->usParam1, tMsg->usParam2) < 0 )
				break;
		}
		usleep(5);
	}

	s_disableUSBConnect = 0;

	/* ### Alarm Main Loop (END) ### */
    g_Snooze = false;
    s_SnoozeTime = 0;
    s_SnoozeInterval = 0;
    if( pMbx->GetCurrentChoice() == 1 ) { // Snooze
        s_SnoozeTime = gettime_sec();
        g_Snooze = true;
        switch( TaviSysParam.alarm.interval ) {
        case 0: 
            s_SnoozeInterval=5*60; 
            break; 
        case 1: 
            s_SnoozeInterval=10*60; 
            break; 
        case 2: 
            s_SnoozeInterval=15*60; 
            break; 
        default:
            ASSERT(0);
        }
    }

    // Music Stop
    AP_Stop();
    pMpegDecoder->XEN_SetPower( false );

    // Restore volume
    g_SndCtrl->SetVolume( vol );
    
    pMbx->Hide();

    delete uzCmd;

    return -1;
}



/* ++

    _MSG_Snoozer( int taskid, TMsgEntry* msg )

Description:
    Countdown timer. 
    If timer is less than 0 than occurs alarm.

Argument:
    taskid: 
    msg: 

Return:
    Returns STOP_RETURN if timer value is less than 0,
    otherwise GO_AHEAD

-- */
static int _MSG_Snoozer( int ssTaskId,  TMsgEntry* tMsg )
{
    extern bool g_KillAll;
    /* 
        Countdown snooze timer.
    */
    if( g_Snooze ) {
        if( (s_nMsgPeekTime-s_SnoozeTime) > s_SnoozeInterval ) {
            g_KillAll = true;
            pTAVI->isAlarmState = 1;
            if( ssTaskId ) {
                tMsg->ulMsgId		= MSG_BUTTON_UP;
                tMsg->ssToTaskId	= ssTaskId;
                tMsg->usParam1		= TAVI_KEY_LEXIT;
                tMsg->usParam2		= 0;
            } 
            else {
                tMsg->ulMsgId		= MSG_DRAW;
                tMsg->ssToTaskId	= 0;
                tMsg->usParam1		= 0;
                tMsg->usParam2		= 0;
            }
            return STOP_RETURN;
        }
    }

    return GO_AHEAD;
}

int MSG_Peek_Time( void )
{
	return s_nMsgPeekTime;
}

int MSG_Peek_Ex( int ssTaskId,  TMsgEntry* tMsg, int checkFlag )
{
	static int	tictoc;

	int		fProcess = -1;
	long		tmCur;
	int		ret;

	tMsg->ulMsgId	= 0;

	// WatchDog!
	RELOAD_WATCHDOGTIMER;
    
	s_nMsgPeekTime	= gettime_sec();

	if (checkFlag & MSG_PEEK_SNOOZE ) {	
		if( _MSG_Snoozer( ssTaskId, tMsg ) == STOP_RETURN ) {
		   	return STOP_RETURN;
			}
		}    

	if (checkFlag & MSG_PEEK_AUDIO_PLAYER) {
		/* ### Check Audio Player First! ### */
		if( AP_GetQueue() == 0 && ssTaskId != TASK_ID_VIDEO) {
		  nIdle_stoptimer_t2 = s_nMsgPeekTime; // clear sleep time
			tMsg->ulMsgId	= MSG_WINDOW;
			tMsg->usParam2	= WINDOW_QUEUE;
			//if( pTAVI->usCurrentMenu != (TASK_ID_MUSIC * 10) ) {	// XXX
			if( ssTaskId != TASK_ID_MUSIC ) {
				AP_MsgProc( tMsg );
				return -1;
				}
				return 0;
			}
		}

	if (checkFlag & MSG_PEEK_MSG_QUEUE) {
		/* ### MessageQueue-0 Pocess ### */
		fProcess = MSG_DeleteFromQueue( &tMsgQueueList[0], tMsg );
		if( fProcess == 0 && tMsg->ssToTaskId == ssTaskId ) {
			return 0;
			}

		/* ### MessageQueue-1 Pocess ### */
		fProcess = MSG_DeleteFromQueue( &tMsgQueueList[1], tMsg );
		if ( fProcess == 0 && tMsg->ssToTaskId == ssTaskId ) {
			return 0;
			}
		}

	/* ### IDLE ### */
	tmCur = s_nMsgPeekTime;
	if ( checkFlag & MSG_PEEK_IDLE ) {
		if ( do_idle(ssTaskId, tMsg, tmCur) ) {
			return 0;
			}
		}

	/* ### SLEEP ### */
	if ( checkFlag & MSG_PEEK_SLEEP ) {
		TAVI_SleepDown( tmCur );		// power save
		}

	/* ### Auto Control Backlight ### */
	if ( checkFlag & MSG_PEEK_AUTOBRIGHT ) {
		if ( TaviSysParam.power.autolcdbright == 0 ) {
			if( tmCur % 2 != tictoc ) {
				SetAutoLCD_Brightness();
				tictoc = (++tictoc) % 2;
				}
			}
		}
	
#if 0
	/* ### App-Specific Routine? ### */
	if ( ssTaskId == TASK_ID_VIDEO
			|| ssTaskId == TASK_ID_TEXT
			|| ssTaskId == TASK_ID_FILEMANAGER ) {

		// For VideoPlayer, FileManager, TextViewer Only

		if ( AppEventCheck(ssTaskId, tMsg) == 0 )
			return 0;	// Low Battery ?
		if ( AppTimerCheck(ssTaskId, tMsg) == TIMER_ACTIVATED )
			return 0;	// Timer Event
	} else {
		// For Other App Modes
		/* Battery Check & Display & Alert Event */

		if ( (ret = _MSG_Battery_Check_Update(ssTaskId, tMsg)) != GO_AHEAD ) return ret;
		//( AppTimerCheck(ssTaskId, tMsg) == TIMER_ACTIVATED ) return 0;	// For SoundPalette TimerEvent Checking

	}
#else
	// For Other App Modes
	/* Battery Check & Display & Alert Event */
	if (checkFlag & MSG_PEEK_BATTERY ) {
		if ( (ret = _MSG_Battery_Check_Update(ssTaskId, tMsg)) != GO_AHEAD ) return ret;
		}

	
	/* ### App-Specific Routine? ### */
	if ( checkFlag & MSG_PEEK_TIMER ) {
		if ( ssTaskId == TASK_ID_VIDEO
				|| ssTaskId == TASK_ID_TEXT
				|| ssTaskId == TASK_ID_FILEMANAGER ) {
			if ( AppTimerCheck(ssTaskId, tMsg) == TIMER_ACTIVATED ) return 0;	// For SoundPalette TimerEvent Checking
			}
		}
#endif
	if (checkFlag & MSG_PEEK_KEY) {
		/* ### READ KEY ### */
		if ( (ret = _MSG_Check_Key(ssTaskId, tMsg)) != GO_AHEAD ) {
			TAVI_ResetSleepTimer();		// power save
			return ret;
			}
		}

	if (checkFlag & MSG_PEEK_TOUCH && g_MiscCtrl->FolderOpen()) {
		/* ### TOUCH PAD ### */
		// No need to check when folder is not open
		if ( (ret = _MSG_Check_TouchPad(ssTaskId, tMsg)) != GO_AHEAD ) {
			TAVI_ResetSleepTimer();		// power save
			return ret;
			}
		}

	if (checkFlag & MSG_PEEK_FOLDER || checkFlag & MSG_PEEK_POWER || checkFlag & MSG_PEEK_EARPHONE) {
		/* ### Folder Open, Head Phone ... ### */
		if ( (ret = _MSG_Check_Misc_Event(ssTaskId, tMsg)) != GO_AHEAD) {
			return ret;
			}
		}

	if (checkFlag & MSG_PEEK_USB) {
		/* ### USB related check ## */
		if (!s_disableUSBConnect) {
			/* 2005-11-22 */
			/* ### Usb Connect As Client Detect ### */
			if ( (ret = _MSG_USB_Client_Connected(ssTaskId, tMsg)) != GO_AHEAD ) {
			  nIdle_stoptimer_t2 = gettime_sec(); // clear sleep time
				return ret;
				}

			/* ### Usb Connection/Disconnection As Host Detect ### */
			if ((ret = _MSG_USB_Host_Connected(ssTaskId, tMsg)) != GO_AHEAD ) {
//				called = 1;
				return ret;
				}
			}
		}

	/* ### DEFAULT ### */

	tMsg->ulMsgId	= 0;
	tMsg->usParam1	= 0;
	tMsg->usParam2	= 0;

	TAVI_BG_VolTimeout();
 
	return -1;
}

int MSG_Peek( int ssTaskId,  TMsgEntry* tMsg )
{
	return MSG_Peek_Ex( ssTaskId, tMsg, MSG_PEEK_ALL );
}

int MSG_Send(int ssTaskId, TMsgEntry* tMsg)
{
	char *msgType;
	int qSelection;

	if( (tMsg->ulMsgId & 0x000000ff) ) {
		msgType = "A";
		qSelection = 0;
	} else {
		msgType = "B";
		qSelection = 1;
	}

	MSG_AddToQueue(&tMsgQueueList[qSelection], tMsg );

	return 0;
}

int MSG_Send( int ssTaskId, int ulMsgId, int usParam1, int usParam2 )
{
	TMsgEntry msg;

	msg.ssToTaskId	= ssTaskId;
	msg.ulMsgId		= ulMsgId;
	msg.usParam1	= usParam1;
	msg.usParam2	= usParam2;

	MSG_Send( ssTaskId, &msg );
}

static void _Show_Queue_Status( char *msg, TMsgQueue* tQueue, TMsgEntry* tMsg )
{
	DEBUGMSG(1,
		(" - [%s] tQueue->head :  [0x%x][%x][%8x][%x][%x]\n",
		msg, tQueue->head, tMsg->ulMsgId, tMsg->ssToTaskId, tMsg->usParam1, tMsg->usParam2  )
	);

	DEBUGMSG(1,
		(" - [%s] tQueue->tail :  [0x%x][%x][%8x][%x][%x]\n",
		msg, tQueue->tail, tMsg->ulMsgId, tMsg->ssToTaskId, tMsg->usParam1, tMsg->usParam2  )
	);
	return;
}

int MSG_AddToQueue( TMsgQueue* tQueue, TMsgEntry* tMsg )
{
	DEBUGMSG(1, ("========================================\n") );
	DEBUGMSG(1, ("Msg::MSG_AddToQueue()\n") );
	_Show_Queue_Status("Before Queuing", tQueue, tMsg);

	if( ++(tQueue->head) >= MSG_QUEUE_DEPTH )
		tQueue->head = 0;

	if( tQueue->head == tQueue->tail )
		if( ++(tQueue->tail) >= MSG_QUEUE_DEPTH )
			tQueue->tail = 0;

	tQueue->queue[ tQueue->head ].ulMsgId		= tMsg->ulMsgId;
	tQueue->queue[ tQueue->head ].ssToTaskId	= tMsg->ssToTaskId;
	tQueue->queue[ tQueue->head ].usParam1		= tMsg->usParam1;
	tQueue->queue[ tQueue->head ].usParam2		= tMsg->usParam2;

	DEBUGMSG(1, ("----------------------------------------\n") );
	_Show_Queue_Status("After Queuing", tQueue, tMsg);
	DEBUGMSG(1, ("========================================\n\n") );

	// return value validation must be added.
	return 0;
}

int MSG_DeleteFromQueue( TMsgQueue* tQueue, TMsgEntry* tMsg )
{
	if( tQueue->head == tQueue->tail) {
		return (-1);
	}
	
	DEBUGMSG(1, ("========================================\n") );
	DEBUGMSG(1, ("Msg::MSG_DeleteFromQueue()\n") );
	_Show_Queue_Status("Before Queuing", tQueue, tMsg);

	if(++(tQueue->tail) >= MSG_QUEUE_DEPTH )
		tQueue->tail = 0;

	tMsg->ulMsgId		= tQueue->queue[ tQueue->tail ].ulMsgId;
	tMsg->ssToTaskId	= tQueue->queue[ tQueue->tail ].ssToTaskId;
	tMsg->usParam1		= tQueue->queue[ tQueue->tail ].usParam1;
	tMsg->usParam2		= tQueue->queue[ tQueue->tail ].usParam2;

	DEBUGMSG(1, ("----------------------------------------\n") );
	_Show_Queue_Status("After Queuing", tQueue, tMsg);
	DEBUGMSG(1, ("========================================\n\n") );

	// return value validation must be added.

	return 0;
}

int read_message( int qid, long type, UsbStrgMsgBuf_t *pbuf )
{
        int     result, length;

        length = sizeof(UsbStrgMsgBuf_t) - sizeof(long);        

        if ((result = msgrcv( qid, pbuf, length, type,  IPC_NOWAIT)) == -1) {
                return(-1);
        }
        
        return(result);
}

/* END */
