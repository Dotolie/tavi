//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Task
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/setup.cpp,v $
// $Revision: 1.12 $
// $Date: 2006/06/22 01:32:42 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: setup.cpp,v $
// Revision 1.12  2006/06/22 01:32:42  zstein
// - fixed sound balance error.
//
// Revision 1.11  2006/06/20 08:22:35  ywkim
// Change size of CListView() for displaying serialNo
//
// Revision 1.10  2006/06/19 05:00:09  ywkim
// Remove check routine of g_HddSleep.m_bLcdEnable
//
// Revision 1.9  2006/06/14 06:03:16  ywkim
// Change displaying routine for serial no
//
// Revision 1.8  2006/06/12 07:04:36  zstein
// - disable alarm when user does initialize system.
//
// Revision 1.7  2006/06/12 04:25:59  ywkim
// Change informatin message of free space to serial no
//
// Revision 1.6  2006/06/07 07:58:02  zstein
// - added power management preset.
//
// Revision 1.5  2006/05/17 04:52:46  zstein
// - fixed system down bug.
//
// Revision 1.4  2006/03/16 02:46:32  zstein
// - fixed audio output bug.
//
// Revision 1.3  2006/03/13 07:59:03  zstein
// - fixed volume control bug when set up defaults.
//
// Revision 1.2  2006/03/09 02:09:34  zstein
// - changed volume interface
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.101  2006/02/22 04:56:35  ywkim
// Change notation of version no.
//
// Revision 1.100  2006/02/22 03:37:41  ywkim
// Add extern variable of nIdle_stoptimer
//
// Revision 1.99  2006/02/18 02:11:55  ywkim
// Print H/W RevNo. for version
//
// Revision 1.98  2006/02/14 01:22:17  zstein
// *** empty log message ***
//
// Revision 1.97  2006/02/11 05:01:55  zstein
// - save time when changed time zone.
//
// Revision 1.96  2006/02/10 07:58:33  ywkim
// Insert instruction of SetupDisplay() in Setup_LoadDefaultConfirmMsgProc
//
// Revision 1.95  2006/02/09 12:19:05  ywkim
// Add instruction of strcpy for timezonestring in Setup_ClockActionProc
//
// Revision 1.94  2006/01/11 09:29:10  ywkim
// Insert white space in string for new font
//
// Revision 1.93  2005/12/22 02:51:30  ywkim
// Add exiting event of jog1_enter with state of folderclosed
//
// Revision 1.92  2005/12/16 11:06:57  ywkim
// Return source to 1.90 version
//
// Revision 1.91  2005/12/16 10:41:37  yong
// resolve conflict
//
// Revision 1.90  2005/12/16 09:25:43  ywkim
// Add call of SetVolume(0) in Setup_SoundActionProc() & Setup_LoadDefaultConfirmMsgProc()
//
// Revision 1.89  2005/12/15 11:09:58  akaiaphanu
// added to adjust screen image
//
// Revision 1.88  2005/12/15 08:27:13  akaiaphanu
// Updated to brightness & contrast functions
//
// Revision 1.87  2005/12/14 11:33:56  yong
// sysparam.sound.output_level
//
// Revision 1.86  2005/12/11 18:28:05  zstein
// *** empty log message ***
//
// Revision 1.85  2005/12/06 07:51:01  ywkim
// Add function call of lcd_enable & lcd_disable in Setup_OutputActionProc()
//
// Revision 1.84  2005/11/28 04:54:31  zstein
// - Can choose prev/next music/channel when playing.
//
// Revision 1.83  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.82  2005/11/23 08:34:18  ywkim
// Add item of setup control for speed of touchpad
//
// Revision 1.81  2005/11/22 11:01:08  ywkim
// Change setup items
//
// Revision 1.80  2005/11/10 07:50:45  ywkim
// Change Setup_OutputActionProc() for removing flicker noise on lcd
//
// Revision 1.79  2005/10/31 06:49:39  ywkim
// Modify Setup_OutputActionProc() for display output
//
// Revision 1.78  2005/10/31 01:01:30  zstein
// - 종료시 property변수 초기화
//
// Revision 1.77  2005/10/28 06:11:25  ywkim
// Add TAVI_CannotSleep() for changing backgound
//
// Revision 1.76  2005/10/28 02:46:58  ywkim
// Add TAVI_CannotSleep for loading default
//
// Revision 1.75  2005/10/27 09:08:41  zstein
// - changed icon list.
//
// Revision 1.74  2005/10/27 06:37:18  ywkim
// Move TAVI_CanSleep( 940 );
//
// Revision 1.73  2005/10/27 05:32:18  ywkim
// Add TAVI_CannotSleep for TV Adjusting routine
//
// Revision 1.72  2005/10/26 07:54:42  ywkim
// 1. Add routine for hdd power saving
// 2. Decrease time of DELAYTIME_HDD_SLEEP to 10 sec
//
// Revision 1.71  2005/10/25 06:21:28  ywkim
// Add definition of DELAYTIME_HDD_SLEEP to 20 sec
//
// Revision 1.70  2005/10/25 02:35:47  ywkim
// Add controls for brightness, contrast & hdd sleep
//
// Revision 1.69  2005/10/19 05:30:22  etnlwind
// DEBUG Removed
//
// Revision 1.68  2005/10/18 01:07:58  zstein
// - deleted intilizing top-panel routine.
//
// Revision 1.67  2005/10/17 02:39:54  zstein
// - updated top-panel
//
// Revision 1.66  2005/09/28 11:12:39  ywkim
// Add function of ClearKeyBuffer() for task switching
//
// Revision 1.65  2005/09/27 05:34:11  ywkim
// Remark item of screen ratio
//
// Revision 1.64  2005/09/22 07:21:20  ywkim
// Bug fix for TV adjusting
//
// Revision 1.63  2005/09/22 06:15:32  ywkim
// Remove instrunction of "delete _SetupProperty.m_pMsgBox"
//
// Revision 1.62  2005/09/20 05:41:10  ywkim
// Remove routine of delete pMsgBox at Setup_LoadDefaultConfirmMsgProc
//
// Revision 1.61  2005/09/14 02:23:43  zstein
// - g_TopPanel->SetTitleResID( ML_SETUP ) to g_TopPanel->SetTitle( ML_SETUP ).
//
// Revision 1.60  2005/09/08 05:06:02  ywkim
// Add UI on bottom frame
//
// Revision 1.59  2005/08/31 03:58:17  ywkim
// change version notation
//
// Revision 1.58  2005/08/30 06:54:53  ywkim
// bug fix on MODE key
//
// Revision 1.57  2005/08/30 04:34:51  ywkim
// bug fix for tv adjust
//
// Revision 1.56  2005/08/23 06:08:39  ywkim
// change function of OnKeypressed()
//
// Revision 1.55  2005/08/22 00:59:28  ywkim
// Change funciton of SetupDisplay() for HDcomponent
//
// Revision 1.54  2005/08/19 11:20:04  etnlwind
// Update for __ATTRIBUTE_PACKED__ and FileBrowser Delete Func.
//
// Revision 1.53  2005/08/18 13:40:18  ywkim
// add instruction of LCDBrightness
//
// Revision 1.52  2005/08/18 03:03:13  ywkim
// Add routine for background playing
//
// Revision 1.51  2005/08/17 09:21:57  ywkim
// Change function for timezone & display
//
// Revision 1.50  2005/08/16 06:16:10  ywkim
// add control for lcdbrigthness
//
// Revision 1.49  2005/08/12 12:44:00  ywkim
// change some resource of item
//
// Revision 1.48  2005/08/09 08:14:13  ywkim
// Add function for screen ratio
//
// Revision 1.47  2005/08/03 05:30:05  ywkim
// Add new item of TVadjust for output setup
//
// Revision 1.46  2005/07/29 03:12:52  ywkim
// Change routine for timezoen & adjust TV
//
// Revision 1.45  2005/07/21 09:54:30  ywkim
// change submenu title
//
// Revision 1.44  2005/07/18 11:52:13  ywkim
// change tavisysparam.etc to tavisysparam.power
//
// Revision 1.43  2005/07/18 06:48:16  ywkim
// Add item of Output & change other items
//
// Revision 1.42  2005/06/30 11:30:31  ywkim
// fix initial bug
//
// Revision 1.41  2005/06/24 08:22:35  ywkim
// add ExtLcd->Update for clock
//
// Revision 1.40  2005/06/24 05:10:05  ywkim
// add mep_set_velocity()
//
// Revision 1.39  2005/06/16 01:59:10  ywkim
// Change func. because touch func. changed
//
// Revision 1.38  2005/06/01 13:39:54  ywkim
// add g_TopPanel->Show() for display update
//
// Revision 1.37  2005/05/27 13:09:35  ywkim
// Change setup of firmware update
//
// Revision 1.36  2005/05/18 20:50:24  ywkim
// some
//
// Revision 1.35  2005/05/18 15:57:30  ywkim
// Add battery level check for firmware update
//
// Revision 1.34  2005/05/17 19:13:37  ywkim
// Change displaysetup
//
// Revision 1.33  2005/05/17 13:51:51  ywkim
// remove parameter of TaviSysParam.theme
//
// Revision 1.32  2005/05/17 10:03:15  ywkim
// add refreshosd
//
// Revision 1.31  2005/05/17 09:14:53  ywkim
// modify string related function
//
// Revision 1.30  2005/05/16 01:58:09  ywkim
// Remove time display of top screen
//
// Revision 1.29  2005/05/15 13:27:27  ywkim
// Remove TaviSysParam.etc.display
//
// Revision 1.28  2005/05/14 14:20:43  ywkim
// Change func. for msgbox
//
// Revision 1.27  2005/05/14 06:05:17  ywkim
// Change Flashupdate
//
// Revision 1.26  2005/05/13 11:31:46  ywkim
// Align spacing of strings
//
// Revision 1.25  2005/05/13 05:28:45  ywkim
// Change Display setup for MainLCD off
//
// Revision 1.24  2005/05/12 09:24:07  ywkim
// Remark some items
//
// Revision 1.23  2005/05/09 14:51:21  ywkim
// Change Charactor string to Resouce Id for m-language
//
// Revision 1.22  2005/05/06 02:56:25  ywkim
// Add routine for LONG_KEY
//
// Revision 1.21  2005/05/03 12:25:41  ywkim
// Define some BG image
//
// Revision 1.20  2005/05/03 11:53:20  ywkim
// Some change with m-language
//
// Revision 1.19  2005/04/28 12:28:07  ywkim
// Modify part of Clock setup
//
// Revision 1.18  2005/04/28 07:10:57  ywkim
// remove remake of include header
//
// Revision 1.17  2005/04/28 06:53:17  ywkim
// Change clocksetup
//
// Revision 1.16  2005/04/28 06:50:43  zstein
// demo merge 2.
//
// Revision 1.15  2005/04/28 05:59:19  zstein
// demo merge.
//
// Revision 1.14  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.13  2005/04/27 05:31:27  ywkim
// Modify some routine for m-language
//
// Revision 1.12  2005/04/22 01:59:52  etnlwind
// Multi Language Support
//
// Revision 1.11  2005/04/18 12:42:29  etnlwind
// update for ES4
//
// Revision 1.10  2005/03/29 01:42:10  ywkim
// Add class of CSetupSound
//
// Revision 1.9  2005/03/06 19:13:50  ywkim
// Remove setosddestination
//
// Revision 1.8  2005/03/06 10:25:41  ywkim
// Remove routine for reboot
//
// Revision 1.7  2005/03/05 19:07:45  ywkim
// Add function for diskspace & sw version
//
// Revision 1.6  2005/03/05 06:29:37  ywkim
// Add function of touchpad on sub-setup-menu
//
// Revision 1.5  2005/03/04 16:08:24  ywkim
// add fuction of progressbar for flashupdate
//
// Revision 1.4  2005/03/03 02:45:59  ywkim
// add function
//
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <linux/reboot.h>
#include "../Lib/LibImage.h"
#include "../Lib/LibFlash.h"
#include <Component/fbdev.h>
#include <Component/ComplexLabel.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/ClockSetView.h>
#include <Component/SlideBar.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "../TopPanel.h"
#include "CSetupGeneral.h"
#include "CSetupDisplay.h"
#include "CSetupSound.h"
#include "CSetupPower.h"
#include "CSetupClock.h"
#include "CSetupOutput.h"
#include "CTvAdjust.h"
#include "CSetupUserEffect.h"
#include "CSetupUserEQ.h"
#include "CSetupAdjustBrightWnd.h"

#include "setup.h"
#include "../../lcd/lcd.h"

#include <mntent.h>
#include <sys/vfs.h>
#include <getopt.h>
#include <Task/tavi_global.h>
#include "CSetupWnd.h"
#include <fatfs.h>
#include <time.h>
#include <Task/ExtLCD.h>

//#include "../SndCtrl.h"

#define _SCREEN_WIDTH_						320
#define _SCREEN_HEIGHT_					240

#define 	_FLASHPROGRESS_BASE_COLOR		24
#define 	_FLASHPROGRESS_TITLE_COLOR		31
#define 	_FLASHPROGRESS_SX					110
#define 	_FLASHPROGRESS_CX					160
#define 	_FLASHPROGRESS_SY					161
#define 	_FLASHPROGRESS_W					100
#define 	_FLASHPROGRESS_H					6
#define 	_FLASHPROGRESS_TITLE_H			21

#define FLASHUPDATE_PATH					"/hd1/Update/romfs.bin"
#define FLASH_UPDATE_BASE					0x40000
#define SET_SYSTEMCONTROLREG(x)			*((volatile unsigned long*)(0x00500020))=(x)

#define __MESSAGE_BASE_BG					_IMAGE_DIR"/bg/message_base.jpg"
#define __FIRMWARE_BG						_IMAGE_DIR"/bg/firmware.jpg"
#define MAKE_VERSION(a, b, c, d)				(((a) << 24) | ((b) << 16) | (c)<< 8 | (d) )

#define DISPLAY_LCD				0
#define DISPLAY_COMPOSITE		1
#define DISPLAY_COMPONENT		2
#define DISPLAY_HDCOMPONENT	3


#if 0	
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
#endif


enum {  
    KILOBYTE = 1024,
    MEGABYTE = (KILOBYTE*1024),
    GIGABYTE = (MEGABYTE*1024)
}; 


TSETUP_CONFIG	_SetupProperty;

extern CTextEngine_BDF* pT;
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern int		g_CurrentTaskID;
extern int		*thumbnailY;
extern SysParameter_t	TaviSysParam;
extern CTopPanel	*g_TopPanel;
extern RMint32	toggle_display;
extern void showbackground( char* path );
extern void showbackground( int nMenuId );
extern int TAVI_LoadPaletteTransparencyData( int IsSubPaletteTransparencyType );
extern CSndCtrl* g_SndCtrl;
extern CExtLCD	*g_ExtLCD;
extern int nIdle_stoptimer;
extern int nIdle_sleeptimer;
extern int nIdle_lcdofftimer;

extern int FR_SetBandLimit(RMuint8 *pRegion);

int	InitFlag;
////////////////////////////////////////////////////
void OnPaint( void );

////////////////////////////////////////////////////

int Setup_LoadDefaultConfirmMsgProc ( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	TMsgEntry tMsg;
	CSetupWnd*	pSetupWnd;
	switch( ssMsgId )
	{
		case MSG_BUTTON_UP:
			switch( usParam1 )
			{
				case TAVI_KEY_LEFT:
					_SetupProperty.m_OnConfirm = _SetupProperty.m_pMsgBox->CursorLeft();
					break;
				case TAVI_KEY_RIGHT:
					_SetupProperty.m_OnConfirm = _SetupProperty.m_pMsgBox->CursorRight();
					break;

				case TAVI_KEY_ENTER:
					if (_SetupProperty.m_OnConfirm == 1)
					{
						DEBUGMSG(1,("[1]Setup_LoadDefaultConfirmMsgProc : TAVI_KEY_ENTER\n" ));
						TAVI_CannotSleep( 960 );					//  power save
						SysSetDefaultParameters( &TaviSysParam );
						SysSaveParameters(&TaviSysParam);
						pT->Change_SystemLanguage((eSYSTEM_LANGUAGE) TaviSysParam.etc.language);
						pSetupWnd =(CSetupWnd*) (_SetupProperty.m_FocusedWindow);
						pSetupWnd->ClearList();
						pSetupWnd->CreateSetup();
						_SetupProperty.m_FocusedWindow->Show();						
						g_TopPanel->Show();
						pMpegDecoder->Resume_ColorTemperature( 
							TaviSysParam.video.contrast, TaviSysParam.video.brightness, TaviSysParam.video.saturation
							);
						pMpegDecoder->SetupDisplay( toggle_display );
						FR_SetBandLimit( (RMuint8 *)&TaviSysParam.radio.cRegion );
						showbackground(ID_BG_SETUP);			// setup
						g_SndCtrl->SetVolume( TaviSysParam.sound.volume );
						TAVI_CanSleep( 960 );					// power save

						/* 
						  clear alarm registers and global variables.
						*/
						extern bool g_Snooze;
						int devfd;
						
						g_Snooze = false;
						pTAVI->isAlarmState = 0;

						devfd = open( RTC_DRIVER_PATH, O_RDWR );
						ioctl( devfd, RTC_AIE_OFF );
						close( devfd );
					}
					else
					{
						DEBUGMSG(1,("[1]Setup_LoadDefaultConfirmMsgProc : TAVI_KEY_EXIT\n" ));
					
					}
					_SetupProperty.m_OnConfirm = 0;
					_SetupProperty.m_FocusedWindow->Show();

					break;

			}
			break;
			
		case MSG_BATTERY:
			break;
		case MSG_TOUCHPAD:
			break;

	}
}

////////////////////////////////////////////////////

int Setup_AdjustTVoutputConfirmMsgProc ( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	TMsgEntry tMsg;
	CSetupWnd*	pSetupWnd;
	switch( ssMsgId )
	{
		case MSG_BUTTON_UP:
			switch( usParam1 )
			{
/*				case TAVI_KEY_LEFT:
					_SetupProperty.m_OnConfirm = _SetupProperty.m_pMsgBox->CursorLeft();
					break;
				case TAVI_KEY_RIGHT:
					_SetupProperty.m_OnConfirm = _SetupProperty.m_pMsgBox->CursorRight();
					break;

				case TAVI_KEY_ENTER:
					if (_SetupProperty.m_OnConfirm == 1)
					{
						DEBUGMSG(1,("[1]Setup_LoadDefaultConfirmMsgProc : TAVI_KEY_ENTER\n" ));
						SysSetDefaultParameters( &TaviSysParam );
						pT->Change_SystemLanguage((eSYSTEM_LANGUAGE) TaviSysParam.etc.language);
						pSetupWnd =(CSetupWnd*) (_SetupProperty.m_FocusedWindow);
						pSetupWnd->ClearList();
						pSetupWnd->CreateSetup();
						g_TopPanel->Show();
						 showbackground(ID_BG_SETUP);			// setup
					}
					else
					{
						DEBUGMSG(1,("[1]Setup_LoadDefaultConfirmMsgProc : TAVI_KEY_EXIT\n" ));
					
					}
					delete _SetupProperty.m_pMsgBox;

					_SetupProperty.m_OnConfirm = 0;
					_SetupProperty.m_FocusedWindow->Show();

					break;
*/
				case TAVI_KEY_ENTER:
				case TAVI_KEY_EXIT:
					_SetupProperty.m_OnConfirm = 0;
					_SetupProperty.m_FocusedWindow->Show();
					break;

			}
			break;
			
		case MSG_BATTERY:
			break;
		case MSG_TOUCHPAD:
			break;

	}
}

void Setup_GeneralActionProc( int nItem, int nComponent )
{
	int	nMajor;
	int	nMinor;
	int	nPatch;
	int 	nExtra;
	int	hw;
	char	szTemp[32];
	uchar_t ucText[32];
	
	DEBUGMSG(1,("Setup.cpp : GeneralActionProc : Item=%d, comp=%d\n", nItem, nComponent) );
	
	switch( nItem )
	{
		case 0:	// language	for general setup
			TAVI_CannotSleep( 920 );					// power save
			TaviSysParam.etc.language = nComponent; // english=0, korean=1,
			DEBUGMSG(1,("Setup_GeneralActionProc : language=[%d]\n", (eSYSTEM_LANGUAGE) TaviSysParam.etc.language ));
			pT->Change_SystemLanguage((eSYSTEM_LANGUAGE) TaviSysParam.etc.language);
			TAVI_CanSleep( 930 );					// power save
			_SetupProperty.m_FocusedWindow->Show();
			g_TopPanel->Show();
			break;
			
/*
		case 1:	// startup	for general setup
			TaviSysParam.etc.startup = nComponent;	// resume = 0, main mode = 1;
			break;
*/
		case 1:	// tune region	for general setup
			TaviSysParam.radio.cRegion = nComponent;	// resume = 0, main mode = 1;
			FR_SetBandLimit( (RMuint8 *)&TaviSysParam.radio.cRegion );
			break;

		case 2:	// beep on/off		for sound setup
			TaviSysParam.etc.buzzer= nComponent;			// on =0, off=1;
			break;

		case 3:	// touch speed
			TaviSysParam.etc.touchspeed= nComponent;
			mep_set_velocity( (MEP_REL_VELOCITY ) TaviSysParam.etc.touchspeed );
			break;
			
		case 4:	// sleep timer	for power setup
			TaviSysParam.power.sleeptimer = nComponent;
			nIdle_sleeptimer = 0;
			break;
			
		case 5:	// load default for general setup
			if ( nComponent > 0 )
			{
				_SetupProperty.m_pMsgBox->Reset( ML_LOAD_DEFAULT,
					_ORANGE,
					MML_CONFIRM,
					244, 123,
					Setup_LoadDefaultConfirmMsgProc,
					DL_BG_BLANK | DL_BTN_OK_CANCEL
					);
				_SetupProperty.m_pMsgBox->Show();
				_SetupProperty.m_OnConfirm = 2;
			}
			break;

/*
		case 3:	// Firmware upgrade  for general setup
			if ( nComponent > 0 )
			{
				if ( CalcBatteryLevel()  > CTopPanel::BATTERY_LEVEL_1)
				{
					_SetupProperty.m_pMsgBox = new CMsgBox( ML_FIRMWARE_UPGRADE,
						_ORANGE,
						MML_CONFIRM,
						244, 123,
						Setup_FirmwareConfirmMsgProc,
						DL_BG_BLANK | DL_BTN_OK_CANCEL
						);
					_SetupProperty.m_pMsgBox->Show();
					_SetupProperty.m_OnConfirm = 2;
					
					nExtra = TaviSysParam.nVersion &0xff;
					nPatch = (TaviSysParam.nVersion >> 8 ) & 0xff;
					nMinor = (TaviSysParam.nVersion >> 16 ) & 0xff;
					nMajor = (TaviSysParam.nVersion >> 24 ) & 0xff;
					sprintf( szTemp,"Ver. %d.%d.%d",nMajor,nMinor,nPatch);
					str2ustr( ucText, szTemp, strlen(szTemp) );
					pT->DrawUC16Text(ucText, 120, 94, _ORANGE, 12, DT_TRANSPARENCY);

				}
				else
				{
					_SetupProperty.m_pMsgBox = new CMsgBox( ML_FIRMWARE_UPGRADE,
						_ORANGE,
						ML_FW_UPGRADE_FAILED2,
						244, 123,
						Setup_UpdateBatteryLowMsgProc,
						DL_BG_BLANK 
						);
					_SetupProperty.m_pMsgBox->Show();
					_SetupProperty.m_OnConfirm = 2;
				}	
			}
			break;
*/
	}	
}

void Setup_DisplayActionProc( int nItem, int nComponent)
{
	int nDisplay;
	DEBUGMSG(1,("Setup.cpp : DisplayActionProc : Item=%d, comp=%d\n", nItem, nComponent) );

	switch( nItem )
	{
		case 0:	// background	for display setup
			TAVI_CannotSleep( 970 );				// power save
			TaviSysParam.gui.isUserBackground = nComponent;	// default = 0, 
			showbackground(ID_BG_SETUP);					// setup
			TAVI_CanSleep( 970 );				// power save
			break;

		case 1: // adjust brightness & contrast
			if ( nComponent > 0 )
			{
				TAVI_CannotSleep( 970 );
				if(TaviSysParam.gui.isUserBackground)
				{
					TaviSysParam.gui.isUserBackground = 0;
					showbackground(ID_BG_ADJUST_SCREEN);
					TaviSysParam.gui.isUserBackground = 1;
				}
				else
				{
					showbackground(ID_BG_ADJUST_SCREEN);
				}
				TAVI_CanSleep( 970 );
				// create adjust brightness & contrast dialog
				_SetupProperty.m_pAdjustBright = new CSetupAdjustBrightWnd(NULL);
				((CSetupAdjustBrightWnd *)_SetupProperty.m_pAdjustBright)->Resize( CRect(10, 110, 310, 235), false );
				((CSetupAdjustBrightWnd *)_SetupProperty.m_pAdjustBright)->CreateSetup();

				_SetupProperty.m_FocusedWindow->Hide();
				_SetupProperty.m_WndList.AddHead( _SetupProperty.m_pAdjustBright);
				_SetupProperty.m_FocusedWindow = _SetupProperty.m_pAdjustBright;
				_SetupProperty.m_FocusedWindow->Show();
			}
			break;

#if 0
		case 1:	// brightness for display setup
			TaviSysParam.video.brightness = nComponent;
			pMpegDecoder->Resume_ColorTemperature(TaviSysParam.video.brightness, TaviSysParam.video.contrast );
			break;

		case 2:	// contrast for display setup
			TaviSysParam.video.contrast = nComponent;
			pMpegDecoder->Resume_ColorTemperature(TaviSysParam.video.brightness, TaviSysParam.video.contrast );
			break;

#endif			
		case 2:	// lcd brightness for display setup
			TaviSysParam.etc.lcdbright = nComponent;
			g_MiscCtrl->LCDBrightness( TaviSysParam.etc.lcdbright );
			break;

	}

//	pMpegDecoder->SetOutDisplayOption(TaviSysParam.etc.screenratio+1);
}
void Setup_OutputActionProc( int nItem, int nComponent)
{
	static int nDisplay = DISPLAY_LCD;
	
	DEBUGMSG(1,("Setup.cpp : DisplayActionProc : Item=%d, comp=%d\n", nItem, nComponent) );

	if( toggle_display == 5 || toggle_display == 6 || toggle_display == 7 )
		nDisplay = 3;
	else if( toggle_display == 3 || toggle_display == 4 )
		nDisplay = 2;
	else if( toggle_display == 1 || toggle_display == 2 )
		nDisplay = 1;
	else
		nDisplay = 0;	

	switch( nItem )
	{
		case 0:	// video out	for display setup
			nDisplay = nComponent;		// lcd=0, composite=1, component=2, hdcomponent=3
			break;

		case 1:	// tv type	for display setup
			TaviSysParam.etc.tvtype= nComponent;		// ntsc=0, pal=1
			break;
/*
		case 2:	// screen ratio	for display setup
			TaviSysParam.etc.screenratio = nComponent ;	// Normal = 0, Letter Box=1, 4:3 Panscan=2, 16:9=3
			pMpegDecoder->SetOutDisplayOption(TaviSysParam.etc.screenratio);
			break;
*/
		case 2:	// HD component for display setup
			TaviSysParam.etc.hdcomponent = nComponent; 	// 480p=0, 720p=1, 1080i=2
			break;

		case 3:	// adjust tv screen
			if ( toggle_display )
			{
				TAVI_CannotSleep( 920 );			// power save
				_SetupProperty.m_FocusedWindow->Hide();
				_SetupProperty.m_WndList.AddHead( _SetupProperty.m_pTvAdjust );
				_SetupProperty.m_FocusedWindow = _SetupProperty.m_pTvAdjust;
				_SetupProperty.m_FocusedWindow->Show();
			}
			else
			{
				_SetupProperty.m_pMsgBox->Reset( ML_INFORMATION,
					_ORANGE,
					ML_MSG_WARNING,
					244, 123,
					Setup_AdjustTVoutputConfirmMsgProc,
					DL_BG_ICON
					);
				_SetupProperty.m_pMsgBox->Show();
				_SetupProperty.m_OnConfirm = 2;

			}
			break;
	}

	DEBUGMSG(1,("\n\n Setup_OutputActionProc =%d\n",nDisplay) );

	if ( nDisplay == DISPLAY_LCD )				// 0
	{
		if ( toggle_display !=0 )
		{
			toggle_display = 0;					// lcd
			pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
			lcd_enable( g_MiscCtrl->GetValueCtrl_Out() );
			g_MiscCtrl->MainLCDOnOff( TRUE );
		}
	}
	else if ( nDisplay == DISPLAY_COMPOSITE )	// 1
	{
		g_MiscCtrl->MainLCDOnOff( FALSE );
		lcd_disable();
		if ( TaviSysParam.etc.tvtype == NTSC )	// 0
			toggle_display = 1;				// composite-ntsc
		else
			toggle_display = 2;				// composite-pal
			
		pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
	}
	else if ( nDisplay == DISPLAY_COMPONENT )	// 2
	{
		g_MiscCtrl->MainLCDOnOff( FALSE );
		lcd_disable();
		if ( TaviSysParam.etc.tvtype == NTSC )	// 0
			toggle_display = 3;				// component-ntsc
		else
			toggle_display = 4;;				// component-pal

		pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
	}
	else if ( nDisplay == DISPLAY_HDCOMPONENT )	// 3
	{
		g_MiscCtrl->MainLCDOnOff( FALSE );
		lcd_disable();
		switch( TaviSysParam.etc.hdcomponent )
		{
			case 0:
				toggle_display = 5;			// 480p
				pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
				break;
				
			case 1:
				toggle_display = 6;			// 720p
				pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
				break;
				
			case 2:
				toggle_display = 7;			// 1080i
				pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
				break;

		}

	}
//	pMpegDecoder->SetOutDisplayOption(TaviSysParam.etc.screenratio+1);
}
void Setup_SoundActionProc( int nItem, int nComponent)
{
	DEBUGMSG(1,("Setup.cpp : SoundActionProc() : Item=%d, comp=%d\n", nItem, nComponent) );

	switch( nItem )
	{

/*
		case 0:	// speaker on/off	for sound setup
			TaviSysParam.sound.speaker_mute = nComponent;	// off =0, on=1;
			SpeakerOnOff(nComponent);
			break;
*/
/*
		case 0:	// beep on/off		for sound setup
			TaviSysParam.etc.buzzer= nComponent;			// on =0, off=1;
			break;
*/
		case 0:	// balance		for sound setup
			TaviSysParam.sound.balance = nComponent;
			g_SndCtrl->SetBalance(nComponent);
			DEBUGMSG(1,("[2]Setup_SoundActionProc() : SetBalance(%d)\n",100*nComponent/20) );
			break;

		case 1:
			TaviSysParam.sound.output_level	= nComponent;
			g_SndCtrl->SetVolume( g_SndCtrl->GetVolume() );
			break;

		case 2:	// user effect for sound setup
			if ( nComponent > 0 )
			{
				// create user effect dialog
				_SetupProperty.m_pUserEffect = new CSetupUserEffectWnd(NULL);
				((CSetupUserEffectWnd *)_SetupProperty.m_pUserEffect)->Resize( CRect(10, 35, 310, 225), false );
				((CSetupUserEffectWnd *)_SetupProperty.m_pUserEffect)->CreateSetup();

				//((CSetupUserEffectWnd *)_SetupProperty.m_pUserEffect)->GetParameter();	// load system parameter
				_SetupProperty.m_FocusedWindow->Hide();
				_SetupProperty.m_WndList.AddHead( _SetupProperty.m_pUserEffect );
				_SetupProperty.m_FocusedWindow = _SetupProperty.m_pUserEffect;
				_SetupProperty.m_FocusedWindow->Show();
			}
			break;

		case 3:	// user eq for sound setup
			if ( nComponent > 0 )
			{
				// create user EQ dialog
				_SetupProperty.m_pUserEQ = new CSetupUserEQWnd(NULL);
				((CSetupUserEQWnd *)_SetupProperty.m_pUserEQ)->Resize( CRect(10, 35, 310, 225), false );
				((CSetupUserEQWnd *)_SetupProperty.m_pUserEQ)->CreateSetup();

				//((CSetupUserEQWnd *)_SetupProperty.m_pUserEQ)->GetParameter();	// load system parameter	
				_SetupProperty.m_FocusedWindow->Hide();
				_SetupProperty.m_WndList.AddHead( _SetupProperty.m_pUserEQ );
				_SetupProperty.m_FocusedWindow = _SetupProperty.m_pUserEQ;
				_SetupProperty.m_FocusedWindow->Show();
			}
			break;
			
	}	

}

void Setup_PowerActionProc( int nItem, int nComponent)
{
	DEBUGMSG(1,("Setup.cpp : PowerActionProc() : Item=%d, comp=%d\n", nItem, nComponent) );
  CSetupPowerWnd* pw = ( CSetupPowerWnd* )_SetupProperty.m_FocusedWindow;
	switch( nItem )
	{
		case 0:
		  pw->Preset( nComponent );
		  TaviSysParam.power.preset = nComponent;
		  break;

		case 1:	// stop power timer	for power setup
			TaviSysParam.power.stoptimer = nComponent;
			TaviSysParam.pm_preset[PM_PRESET_OFF].idle_off = nComponent;
			nIdle_stoptimer = 0;
			break;

		case 2:	//  lcd off timer for power setup
			TaviSysParam.power.lcdofftimer= nComponent;
			TaviSysParam.pm_preset[PM_PRESET_OFF].lcd_off = nComponent;
			nIdle_lcdofftimer = 0;
			break;
/*			
		case 3:	// led on off for power setup
			TaviSysParam.power.ledonoff= nComponent;
			if ( nComponent )
				g_MiscCtrl->KeyDecoEnable( false );	/// led off
			else
				g_MiscCtrl->KeyDecoEnable( true );	// led on
			break;
*/			
		case 3: 	// auto lcd brightness for power setup
			TaviSysParam.power.autolcdbright = nComponent;
			TaviSysParam.pm_preset[PM_PRESET_OFF].auto_br = nComponent;
			g_MiscCtrl->LCDBrightness( TaviSysParam.etc.lcdbright );
			// do auto lcd brigth
			break;

		case 4: 	// hdd sleep for power setup
			if ( nComponent )	// disable sleep 
			{
				//TaviSysParam.power.hddtimer = 0;
				TAVI_WakeUp( 900 );
				TaviSysParam.power.hddtimer = 0;
			}
			else					// enable sleep
			{
				TaviSysParam.power.hddtimer= DELAYTIME_HDD_SLEEP;
			}
			TaviSysParam.pm_preset[PM_PRESET_OFF].hdd_sleep 
			  = TaviSysParam.power.hddtimer;
			break;

	}	

}

void Setup_ClockActionProc( int nItem, int nComponent)
{

	CSetupWnd* pSetupWnd;
	CClockSetView *pClockView;

	pSetupWnd = ( CSetupClockWnd* )( _SetupProperty.m_FocusedWindow );
	CCMListItem* pItem = pSetupWnd->GetSelectedItem();
	pClockView = ( CClockSetView* ) pItem->m_pComponent;

	switch( nItem )
	{
		case 0:	// time formart 	for clock setup
			TaviSysParam.etc.timeformat = nComponent;
			pClockView->SetTimeFormat( nComponent );
			pSetupWnd->ClearList();
			pSetupWnd->CreateSetup();
			pSetupWnd->Show();
			break;
		case 1:	// time zone 	for clock setup
			TaviSysParam.etc.timezone = nComponent;
			pClockView->SetTimeZone( nComponent );
			pClockView->ReadTime();
			pSetupWnd->Show();
			strcpy(TaviSysParam.etc.timezoneString, pClockView->m_szTimeZone );
			break;
		default:
			pClockView->SaveTime();
			break;
	}
	g_TopPanel->Update();
	g_ExtLCD->Update();
	DEBUGMSG(1,("Setup.cpp : ClockActionProc() : Item=%d, comp=%d\n", nItem, nComponent) );

}

int Setup_DoSubMenu( CListItem* puzItem )
{
	CWindow* pChild;
//	CPixmap* pBarPixmap = new CPixmap;
	CPixmap* puzPixmapIcon = new CPixmap;
	int ret;
/*	
	pBarPixmap->CreatePixmap( 
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biWidth,
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biHeight, 
		8,
		(char*)TAVI_IMAGES[LIST_SELECTBAR].image_buffer, false );
*/	
	switch( puzItem->m_nIdx ) 
	{
		case 0: 		// general 
			pChild = new CSetupGeneralWnd( NULL );
			((CSetupGeneralWnd*)pChild)->Resize( CRect(10, 35, 310, 225), false );
			((CSetupGeneralWnd*)pChild)->CreateSetup();
			((CSetupGeneralWnd*)pChild)->SetActionProc( Setup_GeneralActionProc   );
			break;
			
		case 1:		// Display
			pChild = new CSetupDisplayWnd( NULL );
			((CSetupDisplayWnd*)pChild)->Resize( CRect(10, 35, 310, 225), false );
			((CSetupDisplayWnd*)pChild)->CreateSetup();
			((CSetupDisplayWnd*)pChild)->SetActionProc( Setup_DisplayActionProc   );
			break;

		case 2: 		// Sound
			pChild = new CSetupSoundWnd( NULL );
			((CSetupSoundWnd*)pChild)->Resize( CRect(10, 35, 310, 225), false );
			((CSetupSoundWnd*)pChild)->CreateSetup();
			((CSetupSoundWnd*)pChild)->SetActionProc( Setup_SoundActionProc   );
			break;

		case 3: 		// output
			pChild = new CSetupOutputWnd( NULL );
			((CSetupSoundWnd*)pChild)->Resize( CRect(10, 35, 310, 225), false );
			((CSetupSoundWnd*)pChild)->CreateSetup();
			((CSetupSoundWnd*)pChild)->SetActionProc( Setup_OutputActionProc   );
			break;


		case 4:		// Power
			pChild = new CSetupPowerWnd( NULL );
			((CSetupPowerWnd*)pChild)->Resize( CRect(10, 35, 310, 225), false );
			((CSetupPowerWnd*)pChild)->CreateSetup();
			((CSetupPowerWnd*)pChild)->SetActionProc( Setup_PowerActionProc   );
			break;
			
		case 5:		// clock
			pChild = new CSetupClockWnd( NULL );
			((CSetupClockWnd*)pChild)->Resize( CRect(10, 35, 310, 225), false );
			((CSetupClockWnd*)pChild)->CreateSetup();
			((CSetupClockWnd*)pChild)->SetActionProc( Setup_ClockActionProc   );
			break;

		case 6:		// Network
		default:
			return -1;
	}
	op_ClearAreaOSD( 0, 35, 320, 205 );

//	_SetupProperty.m_tCurrentMenu = (SETUP_SUBMENU_t)puzItem->m_nIdx;
	_SetupProperty.m_ChildSetupWnd = pChild;
	_SetupProperty.m_FocusedWindow->Hide();
	_SetupProperty.m_WndList.AddHead( pChild );
	_SetupProperty.m_FocusedWindow = pChild;
	_SetupProperty.m_FocusedWindow->Show();
	
	return ret;
}



int Setup_Diskfree(void)
{
#if 0
	struct statfs s;
	long blocks_used;
	long blocks_percent_used;
	char mount_point[5] = "/hd1";
	
	if (statfs(mount_point, &s) != 0) {
		printf("Setup.cpp : do_df Error at %s", mount_point);
		return FALSE;
	}

	if (s.f_blocks > 0) {
		blocks_used = s.f_blocks - s.f_bfree;
		if(blocks_used == 0)
			blocks_percent_used = 0;
		else
			blocks_percent_used = (long) (blocks_used * 100.0 / (blocks_used + s.f_bavail) + 0.5);
		
		printf("%9ld %9ld %9ld %9ld%% %s\n", 
			(s.f_blocks * (s.f_bsize / (double)MEGABYTE)),
			((s.f_blocks - s.f_bfree)*(s.f_bsize/(double)MEGABYTE)),
			(s.f_bavail * (s.f_bsize / (double)MEGABYTE)),
			blocks_percent_used,
			mount_point);

	}
	return ((int)(s.f_bavail * (s.f_bsize / (double)MEGABYTE)) );
#else

	RMuint64 qwVolFreeSizeInBytes;
	RMuint64 qwVolSizeInBytes;
	fatfs_get_vol_size(1, &qwVolSizeInBytes);
	fatfs_get_vol_free_size(1, &qwVolFreeSizeInBytes);

	DEBUGMSG(0, (" \n volume size = %d KB, free space = %d KB\n",
		(int)(qwVolSizeInBytes/(double)MEGABYTE),
		(int)(qwVolFreeSizeInBytes/(double)MEGABYTE) ) 
		);
	return ((int)(qwVolFreeSizeInBytes/(double)MEGABYTE) );

#endif
}


void OnPaint( void )
{
	char szInfomation[20];
	uchar_t ucText[20];
	union {
		int	nNo;
		struct {
			char patch2;
			char patch1;
			char Minor;
			char Major;
		} c; 
	} Ver;
	int	nFreeSpace;
	int	nFreeGB;
	int	nFree10KB;

	g_TopPanel->Show();
	_SetupProperty.m_FocusedWindow->Show();
	
	if(_SetupProperty.m_pAdjustBright)
	{
		TAVI_CannotSleep(9);
		if(TaviSysParam.gui.isUserBackground)
		{
			TaviSysParam.gui.isUserBackground = 0;
			showbackground(ID_BG_ADJUST_SCREEN);
			TaviSysParam.gui.isUserBackground = 1;
		}
		else
		{
			showbackground(ID_BG_ADJUST_SCREEN);
		}
		TAVI_CanSleep(9);
	}
	if ( _SetupProperty.m_FocusedWindow == _SetupProperty.m_puzListView )
	{
		TAVI_WakeUp( 900 );						// power save
		// Firmware version
		Ver.nNo = TaviSysParam.nVersion;
		nFreeSpace = Setup_Diskfree();
		nFreeGB = nFreeSpace/1024;
		nFree10KB = (nFreeSpace - nFreeGB*1024 )/10;

		DrawBottomGuideBGPannel( SKIP_OSD_REFRESH );
		
		sprintf(szInfomation, " : %d.%02dGB", nFreeGB, nFree10KB );
		str2ustr( ucText, szInfomation, strlen(szInfomation) );
		pT->DrawResText(ML_FREE_SPACE,			// disp "Free Space"
			20, 222,
			_DBX_SELECTED_ITEM_BAR_COLOR,
			12,
			DT_TRANSPARENCY| DT_OUTLINE
			);
		
		pT->DrawUC16Text(ucText,					// disp "xx.xxGB"
			20+ pT->GetStringWidth( ML_FREE_SPACE, 12, DT_TRANSPARENCY) ,
			222, 
			_DBX_SELECTED_ITEM_BAR_COLOR,
			12,
			DT_TRANSPARENCY | DT_OUTLINE
			);
		
		sprintf(szInfomation, " : %d.%d.%d.%d-%c%c",
			Ver.c.Major,Ver.c.Minor, Ver.c.patch1, Ver.c.patch2,
			TaviSysParam.ProductID[2], TaviSysParam.ProductID[3] );
		str2ustr( ucText, szInfomation, strlen(szInfomation) );
		pT->DrawResText(ML_FIRMWARE, 180, 222, _DBX_SELECTED_ITEM_BAR_COLOR, 12, DT_TRANSPARENCY| DT_OUTLINE);
		pT->DrawUC16Text(ucText,
			180+ pT->GetStringWidth( ML_FIRMWARE, 12, DT_TRANSPARENCY) ,
			222, _DBX_SELECTED_ITEM_BAR_COLOR, 12, DT_TRANSPARENCY| DT_OUTLINE);

		
		if ( TaviSysParam.ValidSerNo == DECRYPTIONKEY ) {
			sprintf(szInfomation, "S/N : %s", TaviSysParam.SerialNo);
			printf(szInfomation, "S/N : %s, len=%d", TaviSysParam.SerialNo, strlen(TaviSysParam.SerialNo));

			str2ustr( ucText, szInfomation, strlen(szInfomation) );

			DrawBoxWithoutRefresh( _CP_BASE_COLOR_PALLETTE_INDEX, 0, 240-22*2, 320, 22 );

			pT->DrawUC16Text(ucText,					// disp "S/N:xxxxxxx"
				0,
				205, 
				_DBX_SELECTED_ITEM_BAR_COLOR,
				12,
				DT_TRANSPARENCY | DT_OUTLINE |DT_ALIGN_CENTER
				);
			}
	}
	
	op_Flush();
}


int OnKeyPress( int key )
{
	CEvent evt;
	CListItem* puzItem;
	CCMListItem* pCMItem;
	CListItem* pListItem;
	int vkey;
	CNode< CWindow >* pNode;
	CComplexListView* pCMView;
	CWindow *pWnd;
	CWindow *pComponentWnd;


	vkey = MapToVirtualKey( key );
	evt.type = EVENT_KEY;
	evt.u.key.value = vkey;
	evt.u.key.pressed = KEY_PRESSED;
	_SetupProperty.m_FocusedWindow->SendEvent( evt );


	if( _SetupProperty.m_FocusedWindow == _SetupProperty.m_ChildSetupWnd) 
	{
		int 	nComponentIndex;
		int 	nCountTotalItem;
		int	i;

		pWnd = ( CSetupWnd* )_SetupProperty.m_ChildSetupWnd;
		pCMItem = ((CSetupWnd *)pWnd)->GetSelectedItem();
		pComponentWnd = ((CComplexLabel*)(pCMItem->m_pComponent))->GetComponent();
		switch( vkey ) 
		{
			case VK_ENTER:
/*				
				pWnd = ( CSetupWnd* )_SetupProperty.m_ChildSetupWnd;
				nCountTotalItem = ((CSetupWnd *)pWnd)->GetCount();
				for (i=0;i< nCountTotalItem;i++)
				{
					printf("Setup.cpp :OnKeyPress(): nCountTotalItem= %d,current=%d\n",nCountTotalItem,i);
					pListItem = ((CSetupWnd *)pWnd)->GetItem(i);
					pCMItem = (CCMListItem*)pListItem;
					pComponentWnd = ((CComplexLabel*)(pCMItem->m_pComponent))->GetComponent();
					switch ( pComponentWnd->GetWindowType() )
					{
						case WINDOW_TYPE_SPINBAR:
							nComponentIndex = ((CSpinSelectBar*)pComponentWnd)->GetSelectedIndex();
							break;
						
						case WINDOW_TYPE_RADIOBUTTON:
							nComponentIndex = ((CRadioButton*)pComponentWnd)->GetSelectedIndex();
							break;
							
						case WINDOW_TYPE_SLIDEBAR:
							nComponentIndex = ((CSlideBar*)pComponentWnd)->GetValue();
							break;
							
						default:
							if ( vkey == VK_RIGHT )			// general setup sub menu 3,4 is only activate on right key
								nComponentIndex = 1;			// action activate
							break;
					}
					((CSetupGeneralWnd *)pWnd)->DoActionProc(pCMItem->m_nIdx, nComponentIndex);
				}
				break;
*/
        if( pComponentWnd->GetWindowType() != WINDOW_TYPE_SPINBAR ) {
          break;
        }

			case VK_LEFT:
			case VK_RIGHT:
				switch ( pComponentWnd->GetWindowType() )
				{
					case WINDOW_TYPE_SPINBAR:				
						nComponentIndex = ((CSpinSelectBar*)pComponentWnd)->GetSelectedIndex();
						if ( (vkey == VK_RIGHT||vkey == VK_ENTER)  && 
						     ((CSpinSelectBar*)pComponentWnd)->GetCount() == 0)	{		// general setup sub menu 3,4 is only activate on right key
							nComponentIndex = 1;			// action activate
						}
						break;
					
					case WINDOW_TYPE_RADIOBUTTON:
						nComponentIndex = ((CRadioButton*)pComponentWnd)->GetSelectedIndex();
						break;

					case WINDOW_TYPE_SLIDEBAR:
						nComponentIndex = ((CSlideBar*)pComponentWnd)->GetValue();
						break;
/*
					default:
						if ( vkey == VK_RIGHT )			// general setup sub menu 3,4 is only activate on right key
							nComponentIndex = 1;			// action activate
						break;
*/				}
				DEBUGMSG(1, ("Setup.cpp : OnkeyPress : WindowType =0x%08lx\n", pComponentWnd->GetWindowType()));
				((CSetupWnd *)pWnd)->DoActionProc(pCMItem->m_nIdx, nComponentIndex);
				pMpegDecoder->RefreshOsd ();					// refresh screen		
				break;
										
			case VK_ESC:
				pNode = _SetupProperty.m_WndList.DetachHead();
				delete pNode;
				_SetupProperty.m_FocusedWindow = _SetupProperty.m_WndList[0];
				OnPaint();
				break;

			case VK_F3:
				_SetupProperty.m_ExitState = TaskExit_Background;
				_SetupProperty.m_fExit = true;
				return -1;
				break;
				
			case TAVI_KEY_LMODE:
				_SetupProperty.m_ExitState = TaskExit_Switch;
				_SetupProperty.m_fExit = true;
				return -1;
				break;
				
			case VK_EXT_ESC:
				_SetupProperty.m_ExitState = TaskExit_ExitAndSwitch;
				_SetupProperty.m_fExit = true;
				return -1;
				break;
		}
		
	}
	else if( _SetupProperty.m_FocusedWindow == _SetupProperty.m_puzListView ) 
	{
		switch( vkey )
		{
			case VK_ENTER:
				puzItem = _SetupProperty.m_puzListView->GetSelectedItem();
				Setup_DoSubMenu( puzItem );
				break;
				
			case VK_F3:
				_SetupProperty.m_ExitState = TaskExit_Background;
				_SetupProperty.m_fExit = true;
				return -1;
				break;
				
			case TAVI_KEY_LMODE:
				_SetupProperty.m_ExitState = TaskExit_Switch;
				_SetupProperty.m_fExit = true;
				return -1;
				break;
				
			case VK_EXT_ESC:
				_SetupProperty.m_ExitState = TaskExit_ExitAndSwitch;
				_SetupProperty.m_fExit = true;
				return -1;
				break;
				
			case VK_ESC:
				_SetupProperty.m_ExitState = TaskExit_Normal;
				_SetupProperty.m_fExit = true;
				return -1;
				break;
		}
	}
	else if( _SetupProperty.m_FocusedWindow == _SetupProperty.m_pTvAdjust) 
	{
		switch( vkey )
		{
			case VK_ESC:
				pNode = _SetupProperty.m_WndList.DetachHead();
				_SetupProperty.m_FocusedWindow = _SetupProperty.m_WndList[0];
				OnPaint();
				TAVI_CanSleep( 940 );					// power save
				break;

			case VK_F3:
				_SetupProperty.m_ExitState = TaskExit_Background;
				_SetupProperty.m_fExit = true;
				return -1;
				break;
				
			case TAVI_KEY_LMODE:
				_SetupProperty.m_ExitState = TaskExit_Switch;
				_SetupProperty.m_fExit = true;
				return -1;
				break;
								
			case VK_EXT_ESC:
				_SetupProperty.m_ExitState = TaskExit_Normal;
				_SetupProperty.m_fExit = true;
				return -1;
				break;

		}
	}
	return 0;
}

void OnMouseEvent( const MouseEvent& evt )
{
	CEvent event;
	CCMListItem* pCMItem;
	CComplexListView* pCMView;
	CWindow* pWnd;
	CWindow *pComponentWnd;
	CNode< CWindow >* pNode;
	
	event.type = EVENT_MOUSE;
	event.u.mouse = evt;
	_SetupProperty.m_FocusedWindow->SendEvent( event );

	if( _SetupProperty.m_FocusedWindow == _SetupProperty.m_ChildSetupWnd) 
	{
/*
		if( evt.click == 0 && nSavedClick == 1  ) 
		{
			int nComponentIndex;

			switch ( _SetupProperty.m_tCurrentMenu )
			{
				case SETUP_GENERAL:
					pWnd = ( CSetupGeneralWnd* )_SetupProperty.m_ChildSetupWnd;
					pCMItem = ((CSetupGeneralWnd *)pWnd)->GetSelectedItem();
					pComponentWnd = ((CComplexLabel*)(pCMItem->m_pComponent))->GetComponent();
					switch ( pComponentWnd->GetWindowType() )
					{
						case WINDOW_TYPE_SPINBAR:
							nComponentIndex = ((CSpinSelectBar*)pComponentWnd)->GetSelectedIndex();
							break;
						
						case WINDOW_TYPE_RADIOBUTTON:
							nComponentIndex = ((CRadioButton*)pComponentWnd)->GetSelectedIndex();
							break;
					}
					((CSetupGeneralWnd *)pWnd)->DoActionProc(pCMItem->m_nIdx, nComponentIndex);
					break;
				case SETUP_SOUND:
					break;
				case SETUP_DISPLAY:
					pWnd = ( CSetupDisplayWnd * )_SetupProperty.m_ChildSetupWnd;
					pCMItem = ((CSetupDisplayWnd *)pWnd)->GetSelectedItem();
					pComponentWnd = ((CComplexLabel*)(pCMItem->m_pComponent))->GetComponent();
					switch ( pComponentWnd->GetWindowType() )
					{
						case WINDOW_TYPE_SPINBAR:
							nComponentIndex = ((CSpinSelectBar*)pComponentWnd)->GetSelectedIndex();
							break;
						
						case WINDOW_TYPE_RADIOBUTTON:
							nComponentIndex = ((CRadioButton*)pComponentWnd)->GetSelectedIndex();
							break;
					}
					((CSetupDisplayWnd *)pWnd)->DoActionProc(pCMItem->m_nIdx, nComponentIndex);

					break;
				case SETUP_POWER:
					break;
				case SETUP_CLOCK:
					break;
				case SETUP_NETWORK:
					break;
			}
			
			pNode = _SetupProperty.m_WndList.DetachHead();
			delete pNode;
			_SetupProperty.m_FocusedWindow = _SetupProperty.m_WndList[0];
			OnPaint();
		}
*/
	}
	else if( _SetupProperty.m_FocusedWindow == _SetupProperty.m_puzListView ) 
	{
		if( evt.click == 1 )
		{
			CListItem* puzItem;
			puzItem = _SetupProperty.m_puzListView->GetSelectedItem();
			Setup_DoSubMenu( puzItem );
		}
	}

}

int Setup_MsgProc( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	MouseEvent mevt;
	KeyEvent key;
	CNode< CWindow >* pNode;

	DEBUGMSG(1, ("[3]Setup_MsgProcss : MsgId [%x], Param1[%x]\n", ssMsgId,usParam1 ) );
//	CRect rc(0, 0, 319, 239 );

	switch( ssMsgId )
	{
	case MSG_CREATE:
		break;
	case MSG_DRAW:	
		if ( usParam2 == WINDOW_KILL_FOCUS ) {
			_SetupProperty.m_FocusedWindow->Hide();
			pNode = _SetupProperty.m_WndList.DetachHead();
			delete pNode;
			_SetupProperty.m_FocusedWindow = _SetupProperty.m_WndList[0];
			}
		OnPaint();
		InitFlag = 1;
		break;
	case MSG_TIMER:			break;
	case MSG_SETFOCUS : 	break;
	case MSG_KILLFOCUS: 	break;
	case MSG_DESTROY:		break;
	case MSG_BUTTON_DN: 	break;
	case MSG_BUTTON_UP:
		if ( InitFlag == 0 ) 	break;
		if (usParam1 == TAVI_KEY_SHUTDOWN || ( usParam1 == TAVI_KEY_JOG1_ENTER && g_MiscCtrl->FolderClosed()) )
		{
			DEBUGMSG(1,("[1]FR_MsgProc() : TAVI_KEY_SHUTDOWN\n" ));
			_SetupProperty.m_ExitState = TaskExit_Normal;
			_SetupProperty.m_fExit = true;
			return -1;
		}
		switch( usParam1)
		{
			case TAVI_KEY_JOG1_UP:
				TAVI_BG_Volume(1);
				break;
			case TAVI_KEY_JOG1_DOWN:
				TAVI_BG_Volume(-1);
				break;
			case TAVI_KEY_JOG2_ENTER:
				TAVI_BG_PlayPause();
				break;
			case TAVI_KEY_JOG2_UP:
				TAVI_BG_Next();
				break;
			case TAVI_KEY_JOG2_DOWN:
				TAVI_BG_Prev();
				break;
		}
		if ( _SetupProperty.m_OnConfirm)
			_SetupProperty.m_pMsgBox->MsgBtnUp( ssTaskId, ssMsgId, usParam1, usParam2);
		else
			return OnKeyPress( usParam1 );
		break;
	case MSG_BATTERY:
		break;
	case MSG_TOUCHPAD:
		mevt.click = usParam2;
		mevt.x = usParam1;
		if ( _SetupProperty.m_OnConfirm)
			_SetupProperty.m_pMsgBox->MsgBtnUp( ssTaskId, ssMsgId, usParam1, usParam2);
		else
			OnMouseEvent( mevt );
	default:
		return FALSE;
		break;
	}
	pMpegDecoder->RefreshOsd ();					// refresh screen							

	return 0;
}



unsigned char* Setup_GetImageElement( BITMAPFILEINFO256 bmp, int id, int count )
{
	unsigned char* p;
	int w = bmp.bitmapinfoheader.biWidth;
	int h = bmp.bitmapinfoheader.biHeight;
	int dh = h/count;

	p = bmp.image_buffer+w*dh*id;

	return p;
}

int Setup_Kill(void)
{
	/////////////////////////////////////////////////////////////
	// Exit  Task
	TAVI_CannotSleep( 990 );				// power save
	
	if (_SetupProperty.m_pMsgBox) 
		delete _SetupProperty.m_pMsgBox;
	if (_SetupProperty.m_pTvAdjust) 
		delete _SetupProperty.m_pTvAdjust;
	//if (_SetupProperty.m_pUserEffect) 
	//	delete _SetupProperty.m_pUserEffect;
	//if (_SetupProperty.m_pUserEQ) 
	//	delete _SetupProperty.m_pUserEQ;
	_SetupProperty.m_WndList.FreeAll();
	
//	showbackground(0);			// main

	//memset( (void*)&_SetupProperty, 0, sizeof(_SetupProperty) );
	_SetupProperty.m_ChildSetupWnd = NULL;
	_SetupProperty.m_FocusedWindow = NULL;
	_SetupProperty.m_pAdjustBright = NULL;
	_SetupProperty.m_pMsgBox       = NULL;
	_SetupProperty.m_pTvAdjust     = NULL;
	_SetupProperty.m_pUserEffect   = NULL;
	_SetupProperty.m_pUserEQ       = NULL;
	_SetupProperty.m_puzListView   = NULL;
	
	return 0;
}

int Setup_Task( TTaskInitParam* tParam )
{
	TMsgEntry tMsg;
	CPixmap* puzPixmapIcon;
	CPixmap *pBarPixmap;
	CListItem* puzItem;

	DEBUGMSG(1,("[0]etup_Task(): step1\n") );
	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(TRUE);
	showbackground( ID_BG_SETUP );		// setup
	op_ClearAreaOSD( 
		0, g_TopPanel->rcABS.GetHeight(), 
		320, 240-g_TopPanel->rcABS.GetHeight() );

	SetTaviKeyMode(TK_REPEAT);
	mep_report( MEP_REPORT_RELATIVE);
//	mep_set_velocity( MEP_VELOCITY_80 );					// change touch speed

	// Set current task id
	g_CurrentTaskID = tParam->ssTaskId;

	_SetupProperty.m_nTaskID = tParam->ssTaskId;
	_SetupProperty.m_fExit = false;
	if( tParam->resume ) {
		OnPaint();
		goto __msg_proc;
	}

	TAVI_CanSleep( 900 );				// power save

	// Create ListView Control
	_SetupProperty.m_puzListView = new CListView( NULL );
	_SetupProperty.m_puzListView->Resize( CRect(18, 33, 319-18, 239-66), false );

	//Add General Item
	puzItem = new CListItem( 0, 0, 1, ML_GENERAL );
	_SetupProperty.m_puzListView->AddItem( puzItem );
	// Add Display Item
	puzItem = new CListItem( 1, 4, 5, ML_DISPLAY );
	_SetupProperty.m_puzListView->AddItem( puzItem );
	// Add Sound Item
	puzItem = new CListItem( 2, 2, 3, ML_SOUND );
	_SetupProperty.m_puzListView->AddItem( puzItem );
	// Add Ouput Item
	puzItem = new CListItem( 3, 12, 13, ML_SETUP_OUTPUT );
	_SetupProperty.m_puzListView->AddItem( puzItem );
	// Add Power Item
	puzItem = new CListItem( 4, 6, 7, ML_POWER );
	_SetupProperty.m_puzListView->AddItem( puzItem );
	// Add Clock Item
	puzItem = new CListItem( 5, 8, 9, ML_CLOCK );
	_SetupProperty.m_puzListView->AddItem( puzItem );
	// Add Network Item
//	puzItem = new CListItem( 5, 10, 11, "Network" );
//	_SetupProperty.m_puzListView->AddItem( puzItem );

	// Clear default icon list
	_SetupProperty.m_puzListView->ResetIconList();
	// Add Icons
	for( int i=0; i<14; i++ ) {
		puzPixmapIcon = new CPixmap;
		puzPixmapIcon->CreatePixmap( 
			12,
			12,
			8,
			(char*)Setup_GetImageElement(TAVI_IMAGES[SETUP_LISTICONS], i, 14)
			, false );
		_SetupProperty.m_puzListView->AddPixmapIcon( puzPixmapIcon );
	}
	// Set selectbar image
	pBarPixmap = new CPixmap;
	pBarPixmap->CreatePixmap( 
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biWidth, 
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biHeight,
		8,
		(char*)TAVI_IMAGES[LIST_SELECTBAR].image_buffer, false
		);
		
	_SetupProperty.m_puzListView->SetSelectBarPixmap( pBarPixmap );
	_SetupProperty.m_FocusedWindow = _SetupProperty.m_puzListView;
	_SetupProperty.m_WndList.AddHead( _SetupProperty.m_FocusedWindow );
	_SetupProperty.m_fExit = false;
	_SetupProperty.m_OnConfirm = 0;
/*
	// create user effect dialog
	_SetupProperty.m_pUserEffect = new CSetupUserEffectWnd(NULL);
	((CSetupUserEffectWnd *)_SetupProperty.m_pUserEffect)->Resize( CRect(10, 35, 310, 225), false );
	((CSetupUserEffectWnd *)_SetupProperty.m_pUserEffect)->CreateSetup();
	// create user EQ dialog
	_SetupProperty.m_pUserEQ = new CSetupUserEQWnd(NULL);
	((CSetupUserEQWnd *)_SetupProperty.m_pUserEQ)->Resize( CRect(10, 35, 310, 225), false );
	((CSetupUserEQWnd *)_SetupProperty.m_pUserEQ)->CreateSetup();
*/
	// create TV Adjust class
	_SetupProperty.m_pTvAdjust = new CTvAdjust();
	// create Msg dialog
	_SetupProperty.m_pMsgBox = new CMsgBox( ML_LOAD_DEFAULT,
			_ORANGE,
			MML_CONFIRM,
			244, 123,
			Setup_LoadDefaultConfirmMsgProc,
			DL_BG_BLANK | DL_BTN_OK_CANCEL
			);

	InitFlag = 0;

__msg_proc:
 	ClearKeyBuffer();

	while( _SetupProperty.m_fExit == false ) 
	{
		if( MSG_Peek( tParam->ssTaskId, &tMsg ) == 0)
		{
			DEBUGMSG(1,("[0]This is Setup_Task: Setup_MsgProc\n") );
			if( Setup_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) == -1)
			{
				// break means End of Task
				break;
			}
		}
		else
		{
			usleep(10);	// 1,000,000 usec = 1sec
		}
	}

	// End
	//ClearOsd();

	int ret = _SetupProperty.m_ExitState;
	if( _SetupProperty.m_ExitState == TaskExit_Normal || _SetupProperty.m_ExitState == TaskExit_ExitAndSwitch ) 
	{
		Setup_Kill();
	}

	printf( "SETUP EXIT: %d\n", ret );

	return ret;

}

