//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : FM Radio(GNT-702S) code file  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/FMRadio/FMRadio.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/03/09 02:07:57 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: FMRadio.cpp,v $
// Revision 1.2  2006/03/09 02:07:57  zstein
// - improved volume interface
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.124  2006/02/16 02:57:37  ywkim
// Add func. of g_MiscCtrl->Mute() in FR_PlayPause()
//
// Revision 1.123  2006/02/10 06:42:58  zstein
// - exit when alarm is occured.
//
// Revision 1.122  2006/02/09 05:18:24  zstein
// - removed pop-noise
//
// Revision 1.121  2006/02/02 06:46:47  ywkim
// Change procedure of Mono_setting & AutoScan
//
// Revision 1.120  2006/01/31 12:03:49  ywkim
// Add instruction of failure checking for driver openning
//
// Revision 1.119  2006/01/24 13:28:46  ywkim
// Change option of DT_RADIO_FONT_OPT to DT_NUMBER_8WIDTH
//
// Revision 1.118  2006/01/13 08:25:29  ywkim
// Added white space for ext lcd
//
// Revision 1.117  2006/01/12 01:28:21  ywkim
// Add option of DT_RADIO_FONT_OPT for output of channel info
//
// Revision 1.116  2006/01/06 11:36:16  zstein
// *** empty log message ***
//
// Revision 1.115  2005/12/29 02:55:28  zstein
// - release record
//
// Revision 1.114  2005/12/28 05:37:02  zstein
// - block recording routine.
//
// Revision 1.113  2005/12/26 05:06:18  zstein
// - released recording radio.
//
// Revision 1.112  2005/12/19 10:43:41  yong
// Back/Exit key to `exit' the radio (back to old good days!)
//
// Revision 1.111  2005/12/17 01:40:58  ywkim
// Modify JOG2_LENTER related event in FR_MsgProcForBottonUpInUiNormal
// Modify LLEFT & LRIGHT related event
//
// Revision 1.110  2005/12/14 12:33:36  yong
// add a call ExtLCD->ChangeMenu()
//
// Revision 1.109  2005/12/14 02:35:46  ywkim
// Disable FR_SetMute() at pressed key of LLEFT & LRIGHT on preset off mode
//
// Revision 1.108  2005/12/11 20:18:30  zstein
// *** empty log message ***
//
// Revision 1.107  2005/12/11 13:58:38  zstein
// *** empty log message ***
//
// Revision 1.106  2005/12/07 11:12:19  ywkim
// Set SetTaviKeyMode(TK_REPEAT) at resume
//
// Revision 1.105  2005/12/07 04:22:02  ywkim
// Change routine of keymode setting for repeat preset tunning
//
// Revision 1.104  2005/12/05 09:46:11  ywkim
// Add touch control on dialog box
// Clear vertical scroll bar at time for initial scan querying
//
// Revision 1.103  2005/12/05 03:28:13  ywkim
// Change argument of function for DrawBottomGuidePannel()
//
// Revision 1.102  2005/12/01 12:30:00  ywkim
// Change control of preset list for deleting preset
//
// Revision 1.101  2005/11/30 13:19:43  ywkim
// Change remark TAVI_KEY_JOG2_ENTER: in FR_MsgProcForButtonUpInUiNormal()
//
// Revision 1.100  2005/11/30 04:58:01  ywkim
// Change structure of config file	for stereo mode
//
// Revision 1.99  2005/11/28 02:45:17  ywkim
// Change speaker related function
//
// Revision 1.98  2005/11/25 13:38:38  ywkim
// Add case of MSG_NOTIFY for speaker on/off when earphone connect/disconnect
//
// Revision 1.97  2005/11/25 12:18:03  ywkim
// Add function of speaker on/off in popup menu
//
// Revision 1.96  2005/11/25 10:42:19  ywkim
// Set cUiState to FR_UI_NORMAL when exit
//
// Revision 1.95  2005/11/23 08:26:53  ywkim
// Remark mep_set_verocity()
//
// Revision 1.94  2005/11/22 11:00:05  ywkim
// Change FR_SetBandLimit() for setup
//
// Revision 1.93  2005/11/22 05:52:13  ywkim
// Add some functions of play/pause
//
// Revision 1.92  2005/11/12 05:32:47  ywkim
// Change displaying routine for ext lcd
//
// Revision 1.91  2005/11/11 12:23:12  ywkim
// Add instruction for displaying play status
//
// Revision 1.90  2005/11/10 07:40:03  ywkim
// Modify FR_DispExtLcd & FR_DispPreset2extlcd for displaying at extlcd
//
// Revision 1.89  2005/11/09 07:46:22  ywkim
// fix bug of freq display for ext lcd
//
// Revision 1.88  2005/11/03 02:26:19  ywkim
// Change title display routine
//
// Revision 1.87  2005/11/02 03:25:16  ywkim
// Fix bug merging display message for freq.
//
// Revision 1.86  2005/11/02 02:58:08  ywkim
// Fix bug when channels was scanned initially
//
// Revision 1.85  2005/11/01 05:40:44  ywkim
// Change display freq. routine for sublcd
//
// Revision 1.84  2005/10/31 14:07:51  zstein
// *** empty log message ***
//
// Revision 1.83  2005/10/25 08:49:54  ywkim
// Change call routine related with TAVI_CannotSleep()
//
// Revision 1.82  2005/10/25 03:19:48  ywkim
// Add routine for Hdd power saving
//
// Revision 1.81  2005/10/18 01:07:34  zstein
// - deleted initializing top-panel routine.
//
// Revision 1.80  2005/10/17 02:39:22  zstein
// - update top-panel
//
// Revision 1.79  2005/10/07 07:09:02  zstein
// - fixed switching task problem.
//
// Revision 1.78  2005/10/06 02:11:57  ywkim
// Remark definition of DEBUG
//
// Revision 1.77  2005/10/06 01:51:50  ywkim
// Change preset related function for deleting
//
// Revision 1.76  2005/10/05 12:22:17  ywkim
// Change icon related function for preset list with LibListBox.cpp & .h
//
// Revision 1.75  2005/10/05 07:52:51  ywkim
// Add function for channel scan when LONG KEY pressed
//
// Revision 1.74  2005/09/27 07:41:14  ywkim
// change ui of searching for turn arounding
//
// Revision 1.73  2005/09/14 01:52:00  zstein
// - g_TopPanel->SetTitleResID( int id ) to g_TopPanel->SetTitle( int id )
//
// Revision 1.72  2005/09/14 01:31:40  ywkim
// Change popup menu for initial setting
//
// Revision 1.71  2005/09/08 07:18:05  ywkim
// Align text of freq at preset list
//
// Revision 1.70  2005/09/08 06:12:11  ywkim
// Add UI of bottom frame
//
// Revision 1.69  2005/09/05 12:13:02  ywkim
// Change control of touchpad for toggling volume & seek
//
// Revision 1.68  2005/08/30 12:19:56  ywkim
// Add volume display for ext_lcd
//
// Revision 1.67  2005/08/30 06:17:44  ywkim
// Add popup menu with record
//
// Revision 1.66  2005/08/30 05:06:26  ywkim
// Add record on popup menu
//
// Revision 1.65  2005/08/25 11:48:39  ywkim
// Bugfix returning to normal mode from list mode for popup menu
//
// Revision 1.64  2005/08/25 09:29:24  etnlwind
// Update for LibVirtualKeyboard
//
// Revision 1.63  2005/08/22 10:04:26  ywkim
// add FR_MUTE_OFF at rescan
//
// Revision 1.62  2005/08/19 05:04:38  ywkim
// change name of function FR_Kill()
//
// Revision 1.61  2005/08/18 03:12:11  ywkim
// Add instruction for background playing
//
// Revision 1.60  2005/08/12 12:45:00  ywkim
// add linebypass(false) for ending
//
// Revision 1.59  2005/08/11 02:35:24  ywkim
// Remove RBUTTON
//
// Revision 1.58  2005/08/03 06:37:06  ywkim
// change FR_CONFIGFILE_PATH
//
// Revision 1.57  2005/07/22 10:43:17  ywkim
// change popup control to touchpad click
//
// Revision 1.56  2005/07/18 10:07:01  zstein
// - changed i/f.
//
// Revision 1.55  2005/07/18 03:38:27  ywkim
// Change control ui to popup
//
// Revision 1.54  2005/07/16 04:51:38  zstein
// - added Recorder window
//
// Revision 1.53  2005/06/30 07:59:24  ywkim
// Change bug on rescan msgbox
//
// Revision 1.52  2005/06/29 07:43:04  ywkim
// Change jog control for folderopen
//
// Revision 1.51  2005/06/28 09:44:44  ywkim
// Add jog control on control menu mode
//
// Revision 1.50  2005/06/27 02:19:59  ywkim
// Change extlcd related func.
//
// Revision 1.49  2005/06/24 08:20:16  ywkim
// Change JOG1 control at presetlist mode
//
// Revision 1.48  2005/06/24 05:00:57  ywkim
// Remark initially display title on EXTLCD
//
// Revision 1.47  2005/06/24 04:51:07  ywkim
// Add jog control
//
// Revision 1.46  2005/06/23 03:09:31  ywkim
// Add BackOn timer
//
// Revision 1.45  2005/06/23 02:13:25  ywkim
// Change EXTLCD related func.
//
// Revision 1.44  2005/06/22 04:53:05  ywkim
// add EXTLCD function
//
// Revision 1.43  2005/06/22 02:16:12  ywkim
// Change icon,title & battery display to g_toppannel()
//
// Revision 1.42  2005/06/21 12:04:15  ywkim
// remark sublcd_battery()
//
// Revision 1.41  2005/06/21 11:48:17  ywkim
// Change Earphone related routine
//
// Revision 1.40  2005/06/16 02:00:42  ywkim
// Change func. because touch func. changed
//
// Revision 1.39  2005/06/14 14:00:43  ywkim
// Change touchpad func.
//
// Revision 1.38  2005/06/01 14:54:02  ywkim
// Remove nOnconfirm for HeadphoneConfirm
//
// Revision 1.37  2005/06/01 13:39:04  ywkim
// ExChange jog2_up & jog2_down
//
// Revision 1.36  2005/06/01 13:19:16  ywkim
// Changer display value of volume
//
// Revision 1.35  2005/05/18 20:51:30  ywkim
// Change control pannel
//
// Revision 1.34  2005/05/18 16:20:18  ywkim
// Modify setup box coordinate
//
// Revision 1.33  2005/05/18 15:57:55  ywkim
// Modify UI
//
// Revision 1.32  2005/05/17 19:18:36  ywkim
// Change msgbox clean
//
// Revision 1.31  2005/05/17 13:52:44  ywkim
// Some modify
//
// Revision 1.30  2005/05/17 09:58:35  ywkim
// Some change
//
// Revision 1.29  2005/05/17 09:13:05  ywkim
// modify string related function
//
// Revision 1.28  2005/05/16 03:28:38  ywkim
// Add control of jog_key on state of presetlist
//
// Revision 1.27  2005/05/15 10:39:28  ywkim
// add function for sublcd
//
// Revision 1.26  2005/05/15 03:14:50  ywkim
// *** empty log message ***
//
// Revision 1.25  2005/05/14 14:20:13  ywkim
// Change func. for battery icon
//
// Revision 1.24  2005/05/13 05:27:53  ywkim
// modify routine for headphone detection
//
// Revision 1.23  2005/05/12 09:25:41  ywkim
// Remove remark for headphone detection
//
// Revision 1.22  2005/05/11 08:03:46  etnlwind
// Update for MPEG4 and Divx3 low profile Codec Related
//
// Revision 1.21  2005/05/09 14:49:32  ywkim
// Add confirm msgbox for no headphone
//
// Revision 1.20  2005/05/06 02:54:05  ywkim
// Add routine for JOG_KEY
//
// Revision 1.19  2005/05/03 11:53:44  ywkim
// Change some UI
//
// Revision 1.18  2005/05/03 07:16:50  ywkim
// Add tunner function for europe
//
// Revision 1.17  2005/05/02 07:22:04  ywkim
// Add information box ui
//
// Revision 1.16  2005/04/28 12:26:05  ywkim
// Modify function of Fatfs_get
//
// Revision 1.15  2005/04/28 07:19:04  ywkim
// Modifiy function of file read & write
//
// Revision 1.14  2005/04/28 05:59:11  zstein
// demo merge.
//
// Revision 1.13  2005/04/18 12:42:00  etnlwind
// update for ES4
//
// Revision 1.12  2005/04/18 07:12:07  ywkim
// Added sigma fs i/f. by Lee Seok
//
// Revision 1.11  2005/03/28 13:52:40  ywkim
// Insert routine of handler open
//
// Revision 1.10  2005/03/06 10:26:49  ywkim
// Change threshold for touchpad
//
// Revision 1.9  2005/03/05 12:51:41  ywkim
// Add memver of SysParameter_t for managing version
//
// Revision 1.8  2005/03/04 16:13:55  ywkim
// Change function for autoscan
//
// Revision 1.7  2005/03/03 09:11:49  etnlwind
// USB연결관련 코드 추가
//
// Revision 1.6  2005/03/03 03:20:19  ywkim
// Modify ioctl of volume control
//
// Revision 1.5  2005/03/01 05:48:26  ywkim
// Add touchpad routine
//
// Revision 1.4  2005/02/26 09:18:58  ywkim
// Add function of touchpad & setup
//
// Revision 1.3  2005/02/21 03:56:56  ywkim
// add scrollor bar cpntrol for preset list
//
// Revision 1.2  2005/02/07 06:11:37  ywkim
// *** empty log message ***
//
// Revision 1.1  2005/02/05 13:58:23  ywkim
// New Task
//
//*****************************************************************************


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>	// memmove/memcpy
#include <Lib/LibImage.h>

#include<Component/object.h>
#include <Component/event.h>
#include <Component/fbdev.h>
#include <Component/timer.h>

#include "../../Driver/sound/sound.h"
#include "FMRadio.h"
#include <Lib/LibBuzzer.h>
#include <Task/SubLCD.h>
#include <Task/ExtLCD.h>
#include <Lib/LibSubMenu.h> 
#include <Task/tavi_global.h>
#include <Task/Recorder/RecorderMain.h>
#include <Task/Recorder/RecordWnd.h>
#include <Task/TopPanel.h>

//#define DEBUG

#define FR_DEV_PATH			"/dev/misc/fm"
#define FR_CONFIGFILE_PATH	_SYSTEMDIR"/frconfig.txt"

#ifdef DEBUG
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

typedef union {
    struct {
        unsigned char cByte1;
        unsigned char cByte2;
        unsigned char cByte3;
        unsigned char cByte4;
        unsigned char cByte5;
    } Byte5Type;
    struct {
        unsigned        pllhi:6;    	// cByte1[5:0] 			0~63		pll[15:8]
        unsigned        blf:1;       	// cByte1[6]			band limit flag (1:reached to band limit
        unsigned        rf:1;     	  	// cByte1[7]			ready flag (1:found station)

        unsigned char plllo;      	// cByte2[7:0]  		255~0	pll[7:0]

        unsigned        ifc:7;     		// cByte3[6:0]			if counter result [6:0]
        unsigned        stereo:1;     	// cByte3[7]			stereo indication (1:stereo reception)

        unsigned        none0:1;      	// cByte4[0]			not used
        unsigned        ci:3;       	// cByte4[3:1]			chip identification
        unsigned        lev:4;     	// cByte4[7:4]			level adc 
        
        unsigned char none1;   	// cByte5				not used
    } RxFormatType;
} FR_RXDATA_t;

typedef union {
    struct {
        unsigned char cByte1;
        unsigned char cByte2;
        unsigned char cByte3;
        unsigned char cByte4;
        unsigned char cByte5;
    } Byte5Type;
    struct {
        unsigned        pllhi:6;    	// cByte1[5:0] 			0~63	pll[15:8]
        unsigned        sm:1;       	// cByte1[6]			(1:search mode)
        unsigned        mute:1;     	// cByte1[7]			(1:mute)

        unsigned char plllo;      	// cByte2[7:0]  		255~0	pll[7:0]

        unsigned        swp1:1;     	// cByte3[0]			software programmable port1
        unsigned        mr:1;       	// cByte3[1]			(1:mute right & force mono)
        unsigned        ml:1;       	// cByte3[2]			(1:mute left & force mono)
        unsigned        ms:1;       	// cByte3[3]			mono/stereo (1:mono , 0:stereo)
        unsigned        hlsi:1;     	// cByte3[4]			hi/low side injection (1: high side, 0:low)
        unsigned        ssl:2;      	// cByte3[6:5]  		0,32,64,96	search stop level
        						//					00:not allowed in search mode
        						//					01:low		adc out = 5
        						//					10:middle	adc out = 7
        						//					11:high		adc out =10
        unsigned        sud:1;      	// cByte3[7]			search up / down (1: up, 0: down)

        unsigned        si:1;       	// cByte4[0]			search indicator (1: swp1 is search flag out, 0: swp1 is swp1)
        unsigned        snc:1;      	// cByte4[1]			stereo noise cancelling (1:on, 0:off)
        unsigned        hcc:1;      	// cByte4[2]			high cut control (1: on, 0:off)
        unsigned        smute:1;    	// cByte4[3]			software mute (1:mute on, 0:mute off)
        unsigned        xtal:1;     	// cByte4[4]			xtal (1: f-32khz, 0:f-13Mhz)
        unsigned        bl:1;       	// cByte4[5]			band limit (1: japan, 0:us.euc)
        unsigned        stby:1;     	// cByte4[6]			(1:standby, 0:normal)
        unsigned        swp2:1;     	// cByte4[7]			software programmable port2 (1:high, 0:low)

        unsigned        none:6;     	// cByte5[5:0] 			0~63	not used
        unsigned        dtc:1;      	// cByte5[6]			de-emphasis time constant (1: 75us, 0:50us
        unsigned        pllref:1;   	// cByte5[7]			pll reference enable (1:enable 6.5Mhz)
    } TxFormatType;
} FR_TXDATA_t;


typedef struct _tag_FMRADIO_CONFIG
{
	int	task_id;
	int	cInitialized;
	int 	nIndexOfPreset;
	int	nNoOfPreset;
	int	nMaxChannelNo;
	int	nArPresetedFrequncy[FR_PRESETLIST_MAX];
	int	nArChannelNo[FR_PRESETLIST_MAX];
	char	szArPresetStrings [FR_PRESETLIST_MAX][27];
	unsigned int		nCurrentFrequency;
	FR_INJECTION_t	tCurrentInjectionSide;
	FR_TUNEMODE_t	cTuneMode;
	FR_REGION_t		cRegion;
	char				cStereo;
	FR_UISTATE_t		cUiState;
	FR_RECEIVEDAUDIO_t	cReceiveStereo;
	CList< CWindow > m_WndList;
	bool m_fExit;
	int 	m_ExitState;
	char m_cMute;
	char m_cSpeaker;
} FR_CONFIG_t;

FILE* fd;
int	handle_fm;
int	nIdTimer4Menu;
int	nIdTimer4BackOn;
int	nOnConfirm;
int	nNoHeadPhone;

FR_TXDATA_t 	tTxDataBuff;
FR_RXDATA_t	tRxDataBuff;	
FR_CONFIG_t	tFrConfig;
CListBox 		*pFrListBox;
CTimer 		*pFrTimer;
CMsgBox		*pMsgBox;

CPopUpMenu*		pPopUpMenu;
CPopUpMenu*		pPopUpMain1;
CPopUpMenu*		pPopUpMain2;

CPopUpSubMenu* 	pPopUpMain_Preset;
CPopUpSubMenu*	pPopUpMain_Speaker;
CPopUpSubMenu*	pPopUpMain_Stereo;

CPopUpMenu*		pPopUpList;

extern void showbackground (char *bkgnd);
extern void showbackground (int nMenuId);
extern int SpeakerOnOff( bool fOnOff );
extern int TAVI_LoadPaletteTransparencyData( int IsSubPaletteTransparencyType );

//extern bool g_Play;
extern CSndCtrl	*g_SndCtrl;
extern CMiscCtrl	*g_MiscCtrl;
extern CTopPanel	*g_TopPanel;
extern CExtLCD	*g_ExtLCD;
extern CTextEngine_BDF	*pT;
extern TTAVI_GLOBAL_CONFIG	*pTAVI;
extern BITMAPFILEINFO256 		TAVI_IMAGES[];
extern SysParameter_t	TaviSysParam;

int FR_LoadPallette(RMuint8* palette);
int FR_AutoScan( void );
int FR_DispFrequencyBar( unsigned int nFrequency);
int FR_NoHeadphoneConfirmMsgProc(int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 );
int FR_ChannelScan( RMuint8 *dummy );
int FR_DeletePreset( RMuint8 *dummy );
int FR_SetPreset( RMuint8* dummy );
int FR_SetSpeaker( RMuint8* pOffOn );
int FR_SetBandLimit( RMuint8* pRegion );
int FR_SetMono( RMuint8 *pMono );
int FR_AddChannel( RMuint8 *dummy );
int FR_Record( RMuint8 *dummy );

void FR_DrawFrame(U32 flags);
void FR_DrawFrame(U32 flags );
void FR_DispFrequency(void);
void FR_DispTuneMode(void);
void FR_DispRegion(void);
void FR_DispExtLcd(char*);
void FR_DispPreset2extlcd( int nIndex );
void FR_StopTimer(int& nIdTimer);

FR_RECEIVEDAUDIO_t FR_DispStereo(void);

// Local variables
#define 	F	0
#define	T	_DBX_SELECTED_ITEM_BAR_COLOR	// 17

char szTunnerTriagleNormal[5][7] = {
	{ T, T, T, T, T, T, T },
	{ F, T, T, T, T, T, F },
	{ F, F, T, T, T, F, F },
	{ F, F, F, T, F, F, F}
};
char szTunnerTriagleEraze[5][7] = {
	{ F, F, F, F, F, F, F},
	{ F, F, F, F, F, F, F},
	{ F, F, F, F, F, F, F},
	{ F, F, F, F, F, F, F}
};



#define FS_DRV_SIGMA	1
#if FS_DRV_SIGMA
#include <stdarg.h>
#include <rmbasic.h>
#include <fatfs.h>

#define FILE_BUFSIZE        8192
char szFileBuf[FILE_BUFSIZE];
int nBufIndex = 0;
int nMaxSize;

extern int vsscanf( const char* str, const char* fmt, va_list ap );
static char* FATFS_fgets( RMuint32 handle, char* pBuf )
{
	FATFS_ERROR sdFatfsErr;
	RMuint32 nBytes;
	char ch;
	int i;

	if( handle <= 0 ) return NULL;
	i = 0;
	while( 1 )
	{
	   	if( nBufIndex == 0 || nBufIndex >= nMaxSize )
	   	{
			sdFatfsErr = fatfs_fread( handle, (RMuint8*)szFileBuf, FILE_BUFSIZE, &nBytes );
			if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) 
			{
				return NULL;
	        	}
			nBufIndex = 0;
			nMaxSize = nBytes;
	        }
		ch = szFileBuf[nBufIndex++];
		pBuf[i++] = ch;
		if( ch == '\n' || ch =='\t' ) break;
	}

	pBuf[i] = 0;
	return pBuf;
}

#if 0
int FATFS_fscanf( RMuint32 handle, const char* fmt, ... )
{
	char buf[128] = { 0, };
	va_list ap;
	int ret;

	if( FATFS_fgets( handle, buf ) == NULL ) return-1;
	va_start( ap, fmt );
	ret = vsscanf( buf, fmt, ap );
	va_end( ap );

	return ret;
}
#endif
#endif /* FS_DRV_SIGMA */

// FM Module Operations 
// For more detail description, Plz. refer to ntea5767.pdf
// ------------------- FM Radio functions ------------------------------------

void FR_InitPopUpMenu( void )
{

	/////////////////////////////////////////////////////////////////
	pPopUpList = new CPopUpMenu;
	pPopUpList->AddSubMenu( ML_AUTO_SCAN, NULL, -1, FR_ChannelScan, NULL );
	pPopUpList->AddSubMenu( ML_DEL_CHANNEL, NULL, -1, FR_DeletePreset, (RMuint8*)&tFrConfig.nIndexOfPreset );	
	pPopUpList->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );


	/////////////////////////////////////////////////////////////////
	// 1. preset
	pPopUpMain_Preset = new CPopUpSubMenu;
	pPopUpMain_Preset->AddSubMenuItem( ML_ON, TRUE, FR_SetPreset, (RMuint8*)&pPopUpMain_Preset->SelectedMenuItemIdx );
	pPopUpMain_Preset->AddSubMenuItem( ML_OFF, TRUE, FR_SetPreset, (RMuint8*)&pPopUpMain_Preset->SelectedMenuItemIdx );
//	pPopUpMain_Preset->AddSubMenuItem( ML_BACK, NULL, NULL );
	// 3. tune region
/*
	pPopUpMain_Region = new CPopUpSubMenu;
	pPopUpMain_Region->AddSubMenuItem( ML_USA, TRUE, FR_SetBandLimit, (RMuint8*)&pPopUpMain_Region->SelectedMenuItemIdx );
	pPopUpMain_Region->AddSubMenuItem( ML_KOREA, TRUE, FR_SetBandLimit, (RMuint8*)&pPopUpMain_Region->SelectedMenuItemIdx );
	pPopUpMain_Region->AddSubMenuItem( ML_EU, TRUE, FR_SetBandLimit, (RMuint8*)&pPopUpMain_Region->SelectedMenuItemIdx );
	pPopUpMain_Region->AddSubMenuItem( ML_JAPAN, TRUE, FR_SetBandLimit, (RMuint8*)&pPopUpMain_Region->SelectedMenuItemIdx );	
//	pPopUpMain_Region->AddSubMenuItem( ML_BACK, NULL, NULL );
*/
	// 3. speaker on/off
	pPopUpMain_Speaker = new CPopUpSubMenu;
	pPopUpMain_Speaker->AddSubMenuItem( ML_ON, TRUE, FR_SetSpeaker, (RMuint8*)&pPopUpMain_Speaker->SelectedMenuItemIdx );
	pPopUpMain_Speaker->AddSubMenuItem( ML_OFF, TRUE, FR_SetSpeaker, (RMuint8*)&pPopUpMain_Speaker->SelectedMenuItemIdx );
	
	// 4. stereo
	pPopUpMain_Stereo = new CPopUpSubMenu;
	pPopUpMain_Stereo->AddSubMenuItem( ML_STEREO, TRUE, FR_SetMono, (RMuint8*)&pPopUpMain_Stereo->SelectedMenuItemIdx );
	pPopUpMain_Stereo->AddSubMenuItem( ML_MONO, TRUE, FR_SetMono, (RMuint8*)&pPopUpMain_Stereo->SelectedMenuItemIdx );
//	pPopUpMain_Stereo->AddSubMenuItem( ML_BACK, NULL, NULL );

	// Main1
	pPopUpMain1 = new CPopUpMenu;
	pPopUpMain1->AddSubMenu( ML_AUTO_SCAN, NULL, -1, FR_ChannelScan, NULL );
	pPopUpMain1->AddSubMenu( ML_RADIO_RECORD, NULL, -1, FR_Record, NULL );	
	pPopUpMain1->AddSubMenu( ML_PRESET, pPopUpMain_Preset, 0, NULL, NULL);
//	pPopUpMain1->AddSubMenu( ML_TUNER_REGION, pPopUpMain_Region, tFrConfig.cRegion, NULL, NULL );
	pPopUpMain1->AddSubMenu( ML_SPEAKER, pPopUpMain_Speaker, tFrConfig.m_cSpeaker, NULL, NULL );
	pPopUpMain1->AddSubMenu( ML_STEREO, pPopUpMain_Stereo, tFrConfig.cStereo, NULL, NULL );
	pPopUpMain1->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );

	// Main2
	pPopUpMain2 = new CPopUpMenu;
	pPopUpMain2->AddSubMenu( ML_ADD_CHANNEL, NULL, -1, FR_AddChannel, NULL);
	pPopUpMain2->AddSubMenu( ML_RADIO_RECORD, NULL, -1, FR_Record, NULL);
	pPopUpMain2->AddSubMenu( ML_PRESET, pPopUpMain_Preset, 0, NULL, NULL  );
//	pPopUpMain2->AddSubMenu( ML_TUNER_REGION, pPopUpMain_Region, tFrConfig.cRegion, NULL, NULL );
	pPopUpMain2->AddSubMenu( ML_SPEAKER, pPopUpMain_Speaker, tFrConfig.m_cSpeaker, NULL, NULL );
	pPopUpMain2->AddSubMenu( ML_STEREO, pPopUpMain_Stereo,  tFrConfig.cStereo, NULL, NULL );	
	pPopUpMain2->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );

	pPopUpMenu = pPopUpList;
}

void FR_DeInitPopUpMenu( void )
{
//	delete pPopUpMain_Preset;
//	delete pPopUpMain_Region;
//	delete pPopUpMain_Stereo;
	delete pPopUpMain1;
	delete pPopUpMain2;
	delete pPopUpList;
}

void
FR_InitPresetList( void )
{
	int i;
	
	for (i=0;i<FR_PRESETLIST_MAX;i++)
	{
		tFrConfig.nArPresetedFrequncy[i] = 0;
		tFrConfig.nArChannelNo[i]=0;
		bzero(tFrConfig.szArPresetStrings[i],sizeof(tFrConfig.szArPresetStrings[i]) ) ;
//		sprintf(tFrConfig.szArPresetStrings[i],"Ch%02d      ---.-- MHz",i+1) ;
	}
	tFrConfig.nIndexOfPreset = 0;
	tFrConfig.nNoOfPreset = 0;
	tFrConfig.nMaxChannelNo = 0;
}



int
FR_AddPreset( char cIndex, unsigned int tFrequency, int nMaxChannelNo )
{
	int	nMajor;
	int	nMinor;
	
	if ( cIndex >= FR_PRESETLIST_MAX ) 
		return -1;												// out of index range :20
	tFrConfig.nMaxChannelNo++;									// increase Max ch no.
	tFrConfig.nArChannelNo[cIndex] = 	nMaxChannelNo;
	tFrConfig.nArPresetedFrequncy[cIndex] = tFrequency;
//	tFrConfig.nArChannelNo[cIndex] = 	tFrConfig.nMaxChannelNo;

	nMajor = tFrConfig.nArPresetedFrequncy[cIndex]/1000;
	nMinor = (tFrConfig.nArPresetedFrequncy[cIndex]-nMajor*1000)/10;
	if (nMajor < 100 )
		sprintf(tFrConfig.szArPresetStrings[cIndex],"Ch%02d          %d.%02d MHz", tFrConfig.nArChannelNo[cIndex],nMajor,nMinor ); // add white space +1
	else
		sprintf(tFrConfig.szArPresetStrings[cIndex],"Ch%02d        %d.%02d MHz", tFrConfig.nArChannelNo[cIndex],nMajor,nMinor );

	pFrListBox->Insert( tFrConfig.szArPresetStrings[cIndex]);

	tFrConfig.nNoOfPreset++;										// increase No of Preset
//	tFrConfig.nIndexOfPreset++;

	return 0;
}


int
FR_WriteConfigFile( void )
{
	int	i;

	#if FS_DRV_SIGMA
	FATFS_ERROR sdFatfsErr;

	sdFatfsErr = fatfs_fopen( (RMuint8*)FR_CONFIGFILE_PATH, _O_WRONLY|_O_TRUNC|_O_CREAT, (RMuint32*)&fd );

	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
        	DEBUGMSG(1,("[1]FR_WriteConfigFile : fail to configfile open\n") );
		return (-1);
	}
	else
	{
		char buf[128];
		int len, nBytes;
        	DEBUGMSG(1,("[1]FR_WriteConfigFile : success to configfile open\n"));
		
//		len = sprintf( buf, "%d\n", tFrConfig.cRegion );
//		fatfs_fwrite( (RMuint32)fd, (RMuint8*)buf, len, (RMuint32*)&nBytes );
//		DEBUGMSG(1,("region: %s", buf ) );
		TaviSysParam.radio.cRegion = tFrConfig.cRegion;

		len = sprintf( buf, "%d\n", tFrConfig.nIndexOfPreset );
		fatfs_fwrite( (RMuint32)fd, (RMuint8*)buf, len, (RMuint32*)&nBytes );
		DEBUGMSG(1,("index: %s", buf ) );

//		len = sprintf( buf, "%d\n", tFrConfig.cStereo );
//		fatfs_fwrite( (RMuint32)fd, (RMuint8*)buf, len, (RMuint32*)&nBytes );
//		DEBUGMSG(1,("index: %s", buf ) );
		TaviSysParam.radio.cStereo = tFrConfig.cStereo;

		for (i=0;i<tFrConfig.nNoOfPreset;i++)
		{
			if (tFrConfig.nArChannelNo[i] == 0 || tFrConfig.nArPresetedFrequncy[i] == 0 )
				break;
			len = sprintf( buf, "%d\t", tFrConfig.nArChannelNo[i] );
			DEBUGMSG(1,("nArChannelNo[%d] = %s",i,buf) );
			fatfs_fwrite( (RMuint32)fd, (RMuint8*)buf, len, (RMuint32*)&nBytes );
			len = sprintf( buf, "%d\n", tFrConfig.nArPresetedFrequncy[i] );
			DEBUGMSG(1,("nArPresetedFrequncy[%d] = %s",i,buf) );
			fatfs_fwrite( (RMuint32)fd, (RMuint8*)buf, len, (RMuint32*)&nBytes );
		}
		fatfs_fclose( (RMuint32)fd );
	}
	#else
	if ( (fd=fopen(FR_CONFIGFILE_PATH, "w" )) == NULL )
	{
        	DEBUGMSG(1,("[1]FR_WriteConfigFile : fail to configfile open\n") );
		return (-1);
	}
	else
	{
        	DEBUGMSG(1,("[1]FR_WriteConfigFile : success to config file open\n"));

		fprintf(fd,"%d\n", tFrConfig.cRegion);   		// Last selected region
		fprintf(fd,"%d\n", tFrConfig.nIndexOfPreset);   // last selected channel no

		for (i=0;i<FR_PRESETLIST_MAX;i++)
		{
			if (tFrConfig.nArChannelNo[i] == 0 || tFrConfig.nArPresetedFrequncy[i] == 0 )
				break;
			fprintf(fd,"%d\t", tFrConfig.nArChannelNo[i]);   		// current preset channel no
			fprintf(fd,"%d\n", tFrConfig.nArPresetedFrequncy[i]);   // current preset channel no
		}
		fclose(fd);
	}
	#endif
	return 0;
}
    

int
FR_ReadConfigFile( void )
{
	int	i;
	int 	nMajor;
	int 	nMinor;
	int	nResult;
	int	nTemp;
	int	nRegionNo;
	int	nSelectNo;
	int	nMaxChNo;
	int	nFrequency;
	int	nChannelNo;
	int	nStereo;

	FR_InitPresetList();

#if FS_DRV_SIGMA
	FATFS_ERROR sdFatfsErr;

	sdFatfsErr = fatfs_fopen( (RMuint8*)FR_CONFIGFILE_PATH, _O_RDONLY, (RMuint32*)&fd );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) 
	{
        	DEBUGMSG(1,("[1]FR_ReadConfig File : fail to config file open\n") );
		return -1;	// 	no config file
	}
	else
	{
		char buf[128];
        	DEBUGMSG(1,("[1]FR_ReadConfigFile : success to open configfiel\n") );

//		FATFS_fgets( (RMuint32)fd, buf );
//		sscanf( buf, "%d", &nRegionNo );
		FATFS_fgets( (RMuint32)fd, buf );
		sscanf( buf, "%d", &nSelectNo );
//		FATFS_fgets( (RMuint32)fd, buf );
//		sscanf( buf, "%d", &nStereo );

//		FR_SetBandLimit( (RMuint8 *)&nRegionNo );
		FR_SetBandLimit( (RMuint8 *)&TaviSysParam.radio.cRegion);
		tFrConfig.nIndexOfPreset = nSelectNo;

		i=0;
		while (1)
		{
			nTemp = 0;
			if( FATFS_fgets( (RMuint32)fd, buf ) == NULL ) nTemp = EOF;
			sscanf( buf, "%d", &nChannelNo );
			if( FATFS_fgets( (RMuint32)fd, buf ) == NULL ) nTemp = EOF;
			sscanf( buf, "%d", &nFrequency );
			if ( nTemp == EOF ) 
				break;
			FR_AddPreset( i++, nFrequency, nChannelNo);
			tFrConfig.nMaxChannelNo = nChannelNo;
			DEBUGMSG(1,("[1] FR_ReadConfigFile : i=%d, ch=%d, freq=%d, Count=%d\n",i, nChannelNo, nFrequency,tFrConfig.nNoOfPreset) );
		}

		fatfs_fclose( (RMuint32)fd );
		if (i==0)
			return -1;	// have not preset
		else
			return 0;	// exist preset
	}
#else
	if ( (fd=fopen(FR_CONFIGFILE_PATH, "r" )) == NULL )
	{
        	DEBUGMSG(1,("[1]FR_ReadConfigFile : fail to configfile open\n") );
        	nResult = FR_WriteConfigFile();
        	return nResult;
	}
	else
	{
        	DEBUGMSG(1,("[1]FR_ReadConfigFile : success to open configfiel\n") );

		fscanf(fd,"%d", &nRegionNo);			// read region
		fscanf(fd,"%d", &nSelectNo);			// read SelectionNo

		FR_SetBandLimit( (RMuint8 *)&nRegionNo );
		tFrConfig.nIndexOfPreset = nSelectNo;
		
		i=0;
		while (1)
		{
			nTemp = fscanf(fd,"%d", &nChannelNo);
			nTemp = fscanf(fd,"%d", &nFrequency);
			if ( nTemp == EOF ) 
				break;
			FR_AddPreset( i++, nFrequency, nChannelNo);
			tFrConfig.nMaxChannelNo = nChannelNo;
			DEBUGMSG(1,("[1] FR_ReadConfigFile : i=%d, ch=%d, freq=%d\n",i, nChannelNo, nFrequency) );
		}

		fclose(fd);
	}
	return 0;

#endif /* FS_DRV_SIGMA */
}

int
FR_GetPreset( char cIndex )
{
	if( cIndex >= FR_PRESETLIST_MAX )
		return -1;												// out of index range : 20
	return(tFrConfig.nArPresetedFrequncy[cIndex]);
}

int
FR_DeletePreset( RMuint8 *dummy )
{
	int	i;
	int 	Index;

	Index = tFrConfig.nIndexOfPreset;

	if ( Index <  pFrListBox->GetSelectedIndx()  )
		pFrListBox->SetSelectedIndx( pFrListBox->GetSelectedIndx() -1 );
	else if (Index ==  pFrListBox->GetSelectedIndx() )
		pFrListBox->SetOnState( 0 );

	if( Index >= FR_PRESETLIST_MAX ||tFrConfig.nNoOfPreset <=0  )
	{
		return -1;								// out of index range : 25
	}
	else if ( Index == tFrConfig.nNoOfPreset-1 )	// is last item
	{
		DEBUGMSG(1,("-----Index:%d, NoOfPreset:%d nIndexOfPreset:%d\n",
			Index, tFrConfig.nNoOfPreset, tFrConfig.nIndexOfPreset) );
		tFrConfig.nArChannelNo[Index] = 0;
		tFrConfig.nArPresetedFrequncy[Index] = 0;
		bzero(tFrConfig.szArPresetStrings[Index], sizeof(tFrConfig.szArPresetStrings[Index]) ) ;
		tFrConfig.nIndexOfPreset--;
		if ( tFrConfig.nIndexOfPreset < 0)
			tFrConfig.nIndexOfPreset =0;
	}
	else
	{
		for ( i=Index;i<tFrConfig.nNoOfPreset;i++)
		{
			tFrConfig.nArChannelNo[i] = tFrConfig.nArChannelNo[i+1];
			tFrConfig.nArPresetedFrequncy[i] = tFrConfig.nArPresetedFrequncy[i+1];
			strcpy(tFrConfig.szArPresetStrings[i], tFrConfig.szArPresetStrings[i+1] );

			DEBUGMSG(1,( "-----nIndex:%d, i:%d, NoOfPreset:%d\n",Index,i,tFrConfig.nNoOfPreset) );
		}
	}

	tFrConfig.nNoOfPreset--;
	pFrListBox->Delete(Index);

	pPopUpMenu->Hide(false);
	DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );
	pFrListBox->ReDraw();
	pPopUpMenu->Show();

	FR_DispPreset2extlcd(pFrListBox->GetCurrentChoice());
	
	DEBUGMSG(1,("-----IndexOfPreset:%d, i:%d, NoOfPreset:%d\n",
		tFrConfig.nIndexOfPreset,i,tFrConfig.nNoOfPreset) );
	
	return 0;
}


int
FR_IsStereo( void )
{
	FR_RXDATA_t *pRxBuf;
	int nResult;
	
	pRxBuf = &tRxDataBuff;

	nResult = read( handle_fm, pRxBuf, 0);
	if (nResult <0)
		return -1;				// read fail
	DEBUGMSG(0,( "[2]FR_IsStereo: %d\n", pRxBuf->RxFormatType.stereo ) );
		
	if ( pRxBuf->RxFormatType.stereo == FR_RECEIVED_STEREO)
		return FR_RECEIVED_STEREO;				// stereo
	else
		return FR_RECEIVED_MONO;				// mono
}

int
FR_GetMono( void )
{
	FR_TXDATA_t *pTxBuf;
	int nResult;
	
	pTxBuf = &tTxDataBuff;

	// 0 is stereo
	// 1 is mono
	DEBUGMSG(0,( "[3]FR_GetMono: ms=%d\n", pTxBuf->TxFormatType.ms ) );

	return (pTxBuf->TxFormatType.ms);
}

int FR_SetMono( RMuint8 *pMono )
{
	FR_TXDATA_t *pTxBuf;
	int nResult;
	
	pTxBuf = &tTxDataBuff;

	// 0 is stereo
	// 1 is mono
	if ( *pMono == 0 )	// set stereo
		pTxBuf->TxFormatType.ms = 0;
	else					// set mono
		pTxBuf->TxFormatType.ms = 1;

	tFrConfig.cStereo = *pMono;

	nResult = write( handle_fm, pTxBuf, 0);
	if (nResult <0) 
		return -1;											// write fail

	DEBUGMSG(1,( "[3]FR_SetMono: ms=%d\n", pTxBuf->TxFormatType.ms ) );

	FR_DispStereo();
	pMpegDecoder->RefreshOsd ();							// refresh screen
	return 0;
}


int
FR_IsStationFound( void )
{
	FR_RXDATA_t *pRxBuf;
	int	nResult;
	
	pRxBuf = &tRxDataBuff;

	nResult = read( handle_fm, pRxBuf, 0);
	if (nResult <0)
		return -1;				// read fail
		
	if ( pRxBuf->RxFormatType.rf == FR_STATION_FOUND)
		return 1;				// stereo
	else 
		return 0;				// mono
}

int
FR_SetStandby( FR_STANDBY_t tStandby )
{
	FR_TXDATA_t *pTxBuf;
	int nResult;
	
	pTxBuf = &tTxDataBuff;
	pTxBuf->TxFormatType.stby = tStandby;

	nResult = write( handle_fm, pTxBuf, 0);
	if (nResult <0) 
		return -1;											// write fail

	return 0;
}

int
FR_SetMute( FR_MUTESTATE_t tMute )
{
	FR_TXDATA_t *pTxBuf;
	int nResult;
	
	pTxBuf = &tTxDataBuff;
	pTxBuf->TxFormatType.mute = tMute;
	pTxBuf->TxFormatType.sm = FR_SEARCHMODE_OFF;	// clear search command

	nResult = write( handle_fm, pTxBuf, 0);
	if (nResult <0) 
		return -1;											// write fail

	return 0;
}

int
FR_SetSoftMute( FR_MUTESTATE_t tMute )
{
	FR_TXDATA_t *pTxBuf;
	int nResult;
	
	pTxBuf = &tTxDataBuff;
	pTxBuf->TxFormatType.smute = tMute;
	pTxBuf->TxFormatType.sm = FR_SEARCHMODE_OFF;	// clear search command

	nResult = write( handle_fm, pTxBuf, 0);
	if (nResult <0) 
		return -1;											// write fail

	return 0;
}

int
FR_GetLevel( void )
{
	FR_RXDATA_t *pRxBuf;
	int	nResult;
	
	pRxBuf = &tRxDataBuff;
	
	nResult = read( handle_fm, pRxBuf, 0);
	if (nResult <0)
		return -1;									// read fail

	return ( pRxBuf->RxFormatType.lev );
}


int
FR_SetPll( FR_INJECTION_t tInjectionSide, unsigned int tFreq )
{
	FR_TXDATA_t *pTxBuf;
	FR_RXDATA_t *pRxBuf;
	unsigned int	nFreq;
	int 	nResult;
	
	pTxBuf = &tTxDataBuff;
	pRxBuf = &tRxDataBuff;
	
	
	if (tInjectionSide == FR_INJECTION_HI)
		nFreq = (4000*(tFreq+225))/32768;				// freq of High side injection
	else	 	// FR_INJECTION_LO
		nFreq = (4000*(tFreq-225))/32768;				// freq of low side injection freq


	pTxBuf->TxFormatType.pllhi = ((nFreq>>8) & 0x3f);	// high part of pll[15:8] freq
	pTxBuf->TxFormatType.plllo = (nFreq & 0xff);			// low part of pll[7:0]  freq
	pTxBuf->TxFormatType.sm = FR_SEARCHMODE_OFF;	// clear search command
	pTxBuf->TxFormatType.hlsi = tInjectionSide;			// set high / low side injection
	tFrConfig.tCurrentInjectionSide = tInjectionSide;		// save injection side

	nResult = write( handle_fm, pTxBuf, 0);
	if (nResult <0) 
		return -1;									// write fail
	if ( tFrConfig.cRegion == FR_REGION_EUROPE )
	{
		int 	tFreqTemp;
		
		tFreqTemp = (tFreq /100)*100;
		tFreq -= tFreqTemp;
		if ( tFreq >= 75 )
			tFreq = tFreqTemp+100;
		else if ( tFreq < 75 && tFreq > 25 )
			tFreq = tFreqTemp+50;
		else
			tFreq = tFreqTemp;
	}
	else
	{
		tFreq = ((tFreq + 50)/100)*100;
	}
	tFrConfig.nCurrentFrequency = tFreq;				// save freq
	DEBUGMSG(0,("----tFreq=%d, result=%d\n",tFreq,nResult) );

	return 0;										// set ok
}


FR_INJECTION_t
FR_CheckOptimalInjection( unsigned int tFreq )
{
	FR_TXDATA_t *pTxBuf;
	FR_RXDATA_t *pRxBuf;
	int nTemp;
	int nLevelHigh;
	int nLevelLow;

	DEBUGMSG(0,("Otima Hi ") );
	FR_SetPll(FR_INJECTION_HI, tFreq+225);		// set freq of High side image
	nLevelHigh = FR_GetLevel();
	DEBUGMSG(0,( "Otima Lo ") );
	FR_SetPll(FR_INJECTION_LO, tFreq-225);		// set freq of low side image
	nLevelLow = FR_GetLevel();

//	nTemp = nLevelHigh - nLevelLow;
//	if ( abs(nTemp) < 2 )
//		return FR_INJECTION_CHECKFAIL;
	
	if ( nLevelHigh < nLevelLow  )
		return FR_INJECTION_HI;					// set high side injection
	else
		return FR_INJECTION_LO;					// set low side injection

}

int
FR_CheckOutOfBand( unsigned int nFreq )
{
	FR_TXDATA_t *pTxBuf;

	pTxBuf = &tTxDataBuff;


	if( pTxBuf->TxFormatType.bl == FR_BANDLIMIT_US )	// US, EUC Band limited
	{
	
		if( nFreq >= FR_USFREQUENCY_MIN && nFreq <= FR_USFREQUENCY_MAX ) 
		{
			return 0;							// allowd freq
		}
		else
		{
			return -1;							// disallowd 
		}
	}
	else				// JAPAN band limited
	{
		if( nFreq >= FR_JAPANFREQUENCY_MIN && nFreq <= FR_JAPANFREQUENCY_MAX )
			return 0;							// allowed freq
		else
			return -1;							// diaallowd
	}
}


int
FR_TunningFreq( unsigned int tFreq )
{

	FR_TXDATA_t *pTxBuf;
	FR_RXDATA_t *pRxBuf;
	FR_INJECTION_t tInjectionSide;
	int nResult;
	char cLevel;
	
	pTxBuf = &tTxDataBuff;
	pRxBuf = &tRxDataBuff;

	tInjectionSide = FR_CheckOptimalInjection( tFreq );
	DEBUGMSG(0,("Tunning  ") );
	nResult = FR_SetPll( tInjectionSide, tFreq );		// check Optimal injection side
												// tunning with low side injection
	if( nResult < 0 )
		return -1;								// tunning fail

	cLevel = FR_GetLevel();
	if ( cLevel > 4 )
		return 0;								// fine tune
	else 
		return 1;								// low gain
}

int 
FR_PrintData( void )
{
	FR_RXDATA_t *pRxBuf;
	unsigned int nFreq;
	int nResult;
	int nPll;
	int tPll;
	
	pRxBuf = &tRxDataBuff;

	nResult = read( handle_fm, pRxBuf, 0);
	if (nResult <0)
		return -1;									// read fail

	nPll = (pRxBuf->RxFormatType.plllo | (pRxBuf->RxFormatType.pllhi << 8 )) * 32769 / 4000;
	if( tFrConfig.tCurrentInjectionSide == FR_INJECTION_HI )
		tPll = nPll -225;
	else
		tPll = nPll + 225;

	nFreq = ((tFrConfig.nCurrentFrequency + 50)/100)*100;

	printf( ">ready flag : %d\n", pRxBuf->RxFormatType.rf );
	printf( ">bandlimit  : %d\n", pRxBuf->RxFormatType.blf );
	printf( ">pll (hex)  : 0x%x\n", nPll );
	printf( ">pll (dec)  : %d\n", nPll);
	printf( ">tpll (dec) : %d\n", tPll);
	printf( ">IF count   : 0x%x\n", pRxBuf->RxFormatType.ifc );
	printf( ">stereo     : %d\n", pRxBuf->RxFormatType.stereo );
	printf( ">level      : %d\n", pRxBuf->RxFormatType.lev );
	printf( ">Injection  : %d\n", tFrConfig.tCurrentInjectionSide);
	printf( ">currertfreq: %d\n", nFreq);
	return 0;										// read ok
}

int
FR_GetPll( void )
{
	FR_RXDATA_t *pRxBuf;
	int nResult;
	int sPll;
	int tPll;
	
	sPll = 0;
	pRxBuf = &tRxDataBuff;

	nResult = read( handle_fm, pRxBuf, 0);
	if (nResult <0)
		return -1;										// read fail
		
	sPll = (pRxBuf->RxFormatType.plllo | (pRxBuf->RxFormatType.pllhi << 8))*32768/4000;


	if( tFrConfig.tCurrentInjectionSide == FR_INJECTION_HI )
		tPll = sPll -225;
	else
		tPll = sPll + 225;

	return tPll;								// return current freq
}


FR_SEARCHRESULT_t
FR_SearchStation( FR_SEARCHDIRECTION_t tDir, FR_SSL_t tSsl )
{
	FR_TXDATA_t *pTxBuf;
	FR_RXDATA_t *pRxBuf;
	FR_INJECTION_t tInjectionSide;
	int tFrequency;
	int nResult;
	int nTimeOut;

	nTimeOut = FR_SEARCHTIMEOUT;					// set search timer
	pTxBuf = &tTxDataBuff;
	pRxBuf = &tRxDataBuff;

	if ( tDir == FR_SEARCH_UP )
		if ( tFrConfig.cRegion == FR_REGION_EUROPE )
			tFrequency = tFrConfig.nCurrentFrequency + 50;	
		else
			tFrequency = tFrConfig.nCurrentFrequency + 100;	
	else		// FR_SEARCH_DOWN
		if ( tFrConfig.cRegion == FR_REGION_EUROPE )
			tFrequency = tFrConfig.nCurrentFrequency - 50;	
		else
			tFrequency = tFrConfig.nCurrentFrequency - 100;	
	
	if (tFrConfig.tCurrentInjectionSide == FR_INJECTION_HI)
		tFrequency = (4000*(tFrequency + 225))/32768;	// freq of High side injection
	else	 	// FR_INJECTION_LO
		tFrequency = (4000*(tFrequency -225))/32768;		// freq of low side injection freq

	pTxBuf->TxFormatType.pllhi = ((tFrequency >>8) & 0x3f);	// high part of pll[15:8] freq
	pTxBuf->TxFormatType.plllo = (tFrequency & 0xff);			// low part of pll[7:0]  freq
	
	pTxBuf->TxFormatType.mute = FR_MUTE_ON;			// mute on
	pTxBuf->TxFormatType.sm = FR_SEARCHMODE_ON;
	pTxBuf->TxFormatType.ssl = tSsl;					// set search stop level
	pTxBuf->TxFormatType.sud = tDir;					// set search direction

	pTxBuf->TxFormatType.hlsi = tFrConfig.tCurrentInjectionSide;
	
	nResult = write( handle_fm, pTxBuf, 0);
	if (nResult <0) 
	{
//		FR_SetMute( FR_MUTE_OFF );					// mute off
		return FR_STATION_ACCESSFAIL;				// write fail
	}

	while ( nTimeOut )
	{
		usleep(100000);								// delay 100ms
		tFrequency = FR_GetPll();
		if ( tFrequency < 0)
		{
//			FR_SetMute( FR_MUTE_OFF );				// mute off
			return FR_STATION_ACCESSFAIL;			// read fail
		}		
		if ( pRxBuf->RxFormatType.rf == FR_STATION_FOUND)
			break;
		nTimeOut--;									// decrease timer
	};

	tFrequency = FR_GetPll();							// save new searched frequency
//	FR_SetMute( FR_MUTE_OFF );						// mute off

	if ( pRxBuf->RxFormatType.blf == FR_BANDLIMIT_REACHED )
	{
		FR_TunningFreq( tFrequency );
		return FR_STATION_OUTOFBAND;
	};

	DEBUGMSG(1,( "FR_SearchStation: TimeOut:%d, ready:%d, BL:%d, ifc:0x%x, cfreq:%d, tfreq:%d, lev:%d\n", 
		nTimeOut,
		pRxBuf->RxFormatType.rf,
		pRxBuf->RxFormatType.blf,
		pRxBuf->RxFormatType.ifc,
		tFrConfig.nCurrentFrequency,
		tFrequency,
		pRxBuf->RxFormatType.lev
		)
	);

	FR_TunningFreq( tFrequency );

	if (nTimeOut == 0)
		return FR_STATION_ACCESSFAIL;

	if( FR_CheckOutOfBand( tFrequency ) < 0 ) 
		return FR_STATION_OUTOFBAND;				// out of band
	

	return FR_STATION_FOUND;						// find station
}

int 
FR_TunningUpDown( FR_TUNEDIRECTION_t nDirection )
{
	FR_RXDATA_t *pRxBuf;
	FR_TXDATA_t *pTxBuf;
	FR_INJECTION_t tInjectionSide;
	int 	tFrequency;
	int 	nResult;
	int 	nTempIndex;
	int 	nTimeOut;

	nTimeOut = FR_SEARCHTIMEOUT;
	pRxBuf = &tRxDataBuff;
	pTxBuf = &tTxDataBuff;
	tFrequency = tFrConfig.nCurrentFrequency;


	if ( tFrConfig.cTuneMode == FR_TUNEMODE_PRESET )
	{
		nTempIndex = tFrConfig.nIndexOfPreset;
		
		if ( nDirection == FR_TUNEDIRECTION_UP) 
			nTempIndex++;
		else	 if ( nDirection == FR_TUNEDIRECTION_DOWN )
			nTempIndex--;

		if ( nTempIndex >= tFrConfig.nNoOfPreset) 
			nTempIndex = 0;
		else if ( nTempIndex < 0 )
			nTempIndex = tFrConfig.nNoOfPreset-1;

		tFrequency = tFrConfig.nArPresetedFrequncy[nTempIndex];
		tFrConfig.nIndexOfPreset = nTempIndex;
	}
	else 		// tMode == FR_TUNEMODE_MANUAL
	{

		if( nDirection == FR_TUNEDIRECTION_UP )
			if ( tFrConfig.cRegion == FR_REGION_EUROPE )
				tFrequency = tFrConfig.nCurrentFrequency + 50;		// increase 50khz
			else		
				tFrequency = tFrConfig.nCurrentFrequency + 100;		// increase 100khz
		else	 if ( nDirection == FR_TUNEDIRECTION_DOWN )
			if ( tFrConfig.cRegion == FR_REGION_EUROPE )
				tFrequency = tFrConfig.nCurrentFrequency -50;		// decrease 50khz
			else
				tFrequency = tFrConfig.nCurrentFrequency -100;		// decrease 100khz
		if ( FR_CheckOutOfBand(tFrequency) < 0 )
		{
			if( nDirection == FR_TUNEDIRECTION_UP )
			{
				if( pTxBuf->TxFormatType.bl == FR_BANDLIMIT_US )	// US, EUC Band limited
				{
					tFrequency = FR_USFREQUENCY_MIN;
				}
				else
				{
					tFrequency = FR_JAPANFREQUENCY_MIN;
				}
			}
			else	 if ( nDirection == FR_TUNEDIRECTION_DOWN )
			{
				if( pTxBuf->TxFormatType.bl == FR_BANDLIMIT_US )	// US, EUC Band limited
				{
					tFrequency = FR_USFREQUENCY_MAX;
				}
				else
				{
					tFrequency = FR_JAPANFREQUENCY_MAX;
				}
			}
		}
	}
	nResult = FR_TunningFreq( tFrequency );

	return nResult;								// Tune ok
}

FR_BANDLIMIT_t
FR_GetBandLimit( FR_REGION_t tRegion )
{
	switch (tRegion)
	{
		case FR_REGION_JAPAN:
			return FR_BANDLIMIT_JAPAN;
			break;
		case FR_REGION_KOREA:
		case FR_REGION_USA:
		case FR_REGION_EUROPE:
		default:
			return FR_BANDLIMIT_US;			
			break;
	}
}

int FR_SetBandLimit( RMuint8* pRegion )
{
	FR_TXDATA_t *pTxBuf;
	int nResult;
	
	pTxBuf = &tTxDataBuff;

	switch ( (FR_REGION_t)*pRegion)
	{
		case FR_REGION_JAPAN:
			pTxBuf->TxFormatType.bl = FR_BANDLIMIT_JAPAN;
			break;
		case FR_REGION_KOREA:
		case FR_REGION_USA:
		case FR_REGION_EUROPE:
			pTxBuf->TxFormatType.bl = FR_BANDLIMIT_US;			
			break;
	}
	tFrConfig.cRegion = (FR_REGION_t)*pRegion;
	nResult = write( handle_fm, pTxBuf, 0);
	if (nResult <0) 
		return -1;											// write fail

	DEBUGMSG(1,("[3]FR_SetBandLimit  : bandlimit=%d, Region=%d\n", pTxBuf->TxFormatType.bl, *pRegion ));
	if ( tFrConfig.cUiState == FR_UI_NORMAL && g_CurrentTaskID==TASK_ID_RADIO )
	{
		FR_DrawFrame( FREQUENCYBAR );
		FR_DispFrequencyBar( tFrConfig.nCurrentFrequency );
		FR_DispRegion();
		pMpegDecoder->RefreshOsd ();						// refresh screen
	}

	return 0;
}

void FR_Close( void )
{

	FR_SetMute(FR_MUTE_ON);						// mute on
	FR_SetStandby(FR_STANDBY_ON);				// standby mode	
	FR_WriteConfigFile();							// save preset list
	// do save last parameters 
	tFrConfig.cInitialized = 0;
	close(handle_fm);
}

void FR_DispNoHeadPhoneMsgBox( U32 riTextContents, int sx, int sy, int width, int height )
{
//	DrawBoxWithoutRefresh( _BLACK, sx, sy, width, height ); 
	
	DrawBoxWithoutRefresh( _WHITE, sx, sy, width, height );	
	DrawItemWithoutRefresh(&TAVI_IMAGES[DIALOGBOX_EDGE2], 0, sx, sy, 4,4 );
	DrawItemWithoutRefresh(&TAVI_IMAGES[DIALOGBOX_EDGE2], 1, sx+width-4, sy, 4,4 );
	DrawItemWithoutRefresh(&TAVI_IMAGES[DIALOGBOX_EDGE2], 2, sx, sy+height-4, 4,4 );
	DrawItemWithoutRefresh(&TAVI_IMAGES[DIALOGBOX_EDGE2], 3, sx+width-4, sy+height-4, 4,4 );

	DrawItemWithoutRefresh( &TAVI_IMAGES[ICON_DITTO_SMALL], 0, sx +6, sy + 9, 28, 28 );
	pT->DrawResText(riTextContents,											// disp "Plug-in an"
		sx + 43,sy + 4,
		_BLACK,
		12,
		DT_TRANSPARENCY|DT_ENABLE_DIVIDER|DT_ALIGN_LEFT
		);

}


int FR_CheckHeadPhone(MSG_PROCESSOR MsgProc)
{
	int nReturn;
	
	if (g_MiscCtrl->HeadphoneIN() )
	{
		nReturn = 0;
	}
	else
	{
		pMsgBox->Reset( ML_INFORMATION,
			_ORANGE,
			ML_PLZ_INS_EARPHONE,
			244, 123,
			MsgProc,
			DL_BG_ICON
			);
		pMsgBox->Show();
		nOnConfirm = 2;
		nReturn = 1;
	}
	
/*
	if( mep_report( MEP_REPORT_RELATIVE) == 0 )
	{
		printf( "failed to change report mode : FR_LstBxBtnUpProc\n" );
	}
*/
	return nReturn;
}

int
FR_InitScanConfirmMsgProc(int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	int nResult;
	
	DEBUGMSG(1,("[1]FR_InitScanConfirmMsgProc : usParam1 =0x%08lx\n", usParam1 ));
	switch( ssMsgId )
	{
		case MSG_BUTTON_UP:
			switch( usParam1 )
			{
				case TAVI_KEY_SHUTDOWN:
				case TAVI_KEY_LEXIT:
				case TAVI_KEY_LMODE:
				case TAVI_KEY_MODE:
					return -1;
					break;

				case TAVI_KEY_LEFT:
					nOnConfirm = pMsgBox->CursorLeft();
					break;
				case TAVI_KEY_RIGHT:
					nOnConfirm = pMsgBox->CursorRight();
					break;

				case TAVI_KEY_ENTER:
				case TAVI_KEY_JOG2_ENTER:	
				
					if ( nOnConfirm == 1 )
					{
						nResult = FR_AutoScan();
						DEBUGMSG(1,("FR_InitScanConfirmMsgProc : Autscan return=%d\n", nResult ) );
						tFrConfig.cUiState = FR_UI_PRESETLIST;
						tFrConfig.cTuneMode = FR_TUNEMODE_PRESET;		// init cTunemode to manual			
						pMpegDecoder->RenderText_WithoutRefresh(0,0,0,22,320,240-2);	// Clear OSD
						DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );
						pFrListBox->Display(tFrConfig.nIndexOfPreset);		// get last choice
					}
					else
					{
						return -1;
/*						tFrConfig.cUiState = FR_UI_NORMAL;
						tFrConfig.cTuneMode = FR_TUNEMODE_MANUAL;		// init cTunemode to manual			

						pMpegDecoder->RenderText_WithoutRefresh(0,0,0,22,320,240-22);	// Clear OSD	
						FR_SetPll( tFrConfig.tCurrentInjectionSide,89100);
						FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR );
						FR_DispFrequency();
						FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
						FR_DispTuneMode();
						FR_DispRegion();
						tFrConfig.cReceiveStereo = FR_DispStereo();
						FR_SetMute( FR_MUTE_OFF );						// mute off
						nOnConfirm = 0;

						if ( g_MiscCtrl->HeadphoneIN() )
						{
							nNoHeadPhone = 0;
						}
						else
						{
							pMsgBox->Reset( ML_INFORMATION,
								_ORANGE,
								ML_PLZ_INS_EARPHONE,
								244, 123,
								FR_NoHeadphoneConfirmMsgProc,
								DL_BG_ICON
								);
							pMsgBox->Show();
							nOnConfirm = 2;
							nNoHeadPhone = 1;
						}
*/					}
					nOnConfirm = 0;
					break;
			}
			break;
			
		case MSG_BATTERY:
			DrawItemWithoutRefresh( &TAVI_IMAGES[ICON_TOP_BATTERY], usParam1,
				FR_DISPBATTERY_X, FR_DISPBATTERY_Y, 24, 13 ); 
			break;
		case MSG_TOUCHPAD:
			if ( usParam1 > 0 )	//	move down
			{
				nOnConfirm = pMsgBox->CursorRight();	
			}
			if ( usParam1 < 0 )	// move up
			{
				nOnConfirm = pMsgBox->CursorLeft();
			}

			break;

	}
	return 0;

}


int
FR_ReScanConfirmMsgProc(int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	int nResult;
	
	switch( ssMsgId )
	{
		case MSG_BUTTON_UP:
			switch( usParam1 )
			{
				case TAVI_KEY_SHUTDOWN:
				case TAVI_KEY_LEXIT:
				case TAVI_KEY_LMODE:
				case TAVI_KEY_MODE:			
					return -1;
					break;

				case TAVI_KEY_LEFT:
					nOnConfirm = pMsgBox->CursorLeft();
					break;
				case TAVI_KEY_RIGHT:
					nOnConfirm = pMsgBox->CursorRight();
					break;
										
				case TAVI_KEY_ENTER:
					DEBUGMSG(1,("[1]FR_ReScanConfirmMsgProc : TAVI_KEY_ENTER\n" ));
					if ( nOnConfirm == 1 )
					{
						g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_STOP );
						g_TopPanel->Update();

						nResult = FR_AutoScan();
						pMpegDecoder->RenderText_WithoutRefresh(0,0,0,22,320,240-22);	// Clear OSD
						if ( tFrConfig.cUiState == FR_UI_PRESETLIST )
						{
							DEBUGMSG(1,("[1]FR_ReScanConfirmMsgProc : Autoscan return =%d\n",nResult ) );	
							DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );
							pFrListBox->SetOnState( 0 );							// disappear play icon 
							pFrListBox->ReDraw(tFrConfig.nIndexOfPreset);				// display last choice
							FR_DispPreset2extlcd(tFrConfig.nIndexOfPreset);
							FR_SetMute( FR_MUTE_ON );					// mute on

							FR_StopTimer(nIdTimer4BackOn);
						}
						else
						{
							FR_TunningUpDown( FR_TUNEDIRECTION_DOWN );	// tunning down
							FR_TunningUpDown( FR_TUNEDIRECTION_UP );		// tunning up	for last ch.
							
							FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR |FRBIGICON);
							FR_DispFrequency();
							FR_DispTuneMode();
							tFrConfig.cReceiveStereo = FR_DispStereo();
							FR_DispRegion();
							FR_DispFrequencyBar( tFrConfig.nCurrentFrequency );
							
							if ( tFrConfig.m_cMute )	{
								FR_SetMute( FR_MUTE_ON );					// mute on								
								g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_PAUSE);
								g_TopPanel->Update();
								pFrListBox->SetOnState( 0 );					// set pause icon on lis
								}
							else {
								FR_SetMute( FR_MUTE_OFF );					// mute off
								g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_PLAY );
								g_TopPanel->Update();
								pFrListBox->SetOnState( 1 );					// set play icon on lis
								}
						}
						
						
					}
					else
					{
						pMpegDecoder->RenderText_WithoutRefresh(0,0,0,22,320,240-22);	// Clear OSD
						if ( tFrConfig.cUiState == FR_UI_PRESETLIST)
						{
							DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );
							pFrListBox->ReDraw();									// display last choice
							FR_DispPreset2extlcd(pFrListBox->GetCurrentChoice());
						}
						else
						{
							FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR | FRBIGICON);
							FR_DispFrequency();
							FR_DispTuneMode();
							tFrConfig.cReceiveStereo = FR_DispStereo();
							FR_DispRegion();
							FR_DispFrequencyBar( tFrConfig.nCurrentFrequency );
						}

					}
					nOnConfirm = 0;
//					mep_report( MEP_REPORT_RELATIVE);
					break;
			}
			break;
			
		case MSG_BATTERY:
			break;

		case MSG_TOUCHPAD:
			if ( usParam1 > 0 )	//	move down
			{
				nOnConfirm = pMsgBox->CursorRight();	
			}
			if ( usParam1 < 0 )	// move up
			{
				nOnConfirm = pMsgBox->CursorLeft();
			}
			break;

	}
	return 0;
}



int
FR_NoHeadphoneConfirmMsgProc(int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	switch( ssMsgId )
	{
		case MSG_BATTERY:
			break;

		case MSG_BUTTON_UP:
			switch( usParam1 )
			{
				case TAVI_KEY_SHUTDOWN:
				case TAVI_KEY_LEXIT:
				case TAVI_KEY_LMODE:
				case TAVI_KEY_MODE:			
					return -1;
					break;

				case TAVI_KEY_JOG1_ENTER:	
				case TAVI_KEY_JOG2_ENTER:	
				case TAVI_KEY_ENTER:				
				case TAVI_KEY_EXIT:
					DEBUGMSG(1,("[1]FR_NoHeadphoneConfirmMsgProc : TAVI_KEY_EXIT\n" ));
					nOnConfirm = 0;
/*					FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR );
					FR_DispFrequency();
					FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
					FR_DispTuneMode();
					FR_DispRegion();					
*/
					if (tFrConfig.cUiState == FR_UI_PRESETLIST ) 
					{
						DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );
						pFrListBox->ReDraw();
					}
				break;
			}
			break;
		case MSG_TOUCHPAD:
			break;

	}
	return 0;
}

int
FR_NoHeadphoneInitConfirmMsgProc(int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	switch( ssMsgId )
	{
		case MSG_BATTERY:
			break;

		case MSG_BUTTON_UP:
			switch( usParam1 )
			{
				case TAVI_KEY_SHUTDOWN:
				case TAVI_KEY_LEXIT:
				case TAVI_KEY_LMODE:
				case TAVI_KEY_MODE:			
					return -1;
					break;

				case TAVI_KEY_JOG1_ENTER:	
				case TAVI_KEY_JOG2_ENTER:	
				case TAVI_KEY_ENTER:			
				case TAVI_KEY_EXIT:
					DEBUGMSG(1,("[1]FR_NoHeadphoneConfirmMsgProc0 : TAVI_KEY_EXIT\n" ));
					nOnConfirm = 0;
					return -1;		// exit to menu
					break;
			}
			break;
		case MSG_TOUCHPAD:
			break;

	}

	return 0;
}


void FR_DispPreset2extlcd( int nIndex )
{
	char szTemp[32];
	char szTemp2[8];
	char	szUnit[4];
	char szChno[12];
	char szMajor[4];
	char szMinor[4];
	char szSpc[4];
	int	nMajor;
	int 	nMinor;

	strcpy(szUnit,"MHz");

	if ( tFrConfig.nNoOfPreset == 0  )
	{
		sprintf( szTemp,"---- ----------");
	}
	else
	{	
		if ( tFrConfig.cUiState == FR_UI_PRESETLIST || tFrConfig.cTuneMode == FR_TUNEMODE_PRESET )
		{	
			sscanf( tFrConfig.szArPresetStrings[nIndex], "%[a-zA-Z0-9] %[0-9].%[0-9]", szChno, szMajor, szMinor );
			nMajor = atoi ( szMajor );
			nMinor = atoi ( szMinor );
			if ( nMajor >= 100 )
				sprintf ( szSpc, " ");
			else
				sprintf ( szSpc, "  ");
			sprintf( szTemp,"%s%s%3d.%02d%s", szChno, szSpc, nMajor, nMinor, szUnit );
		}
		else		// UI_NORMAL
		{
			if ( tFrConfig.cTuneMode == FR_TUNEMODE_MANUAL )
			{
				nMajor =tFrConfig.nCurrentFrequency/1000;				// for Mhz scale
				nMinor =(tFrConfig.nCurrentFrequency -nMajor*1000)/10; 	// for cKhz
				if ( nMajor >= 100 )
					sprintf(szChno, "%s", "     ");
				else
					sprintf(szChno, "%s", "      ");					
				sprintf ( szTemp, "%s%3d.%02d%s", szChno, nMajor, nMinor, szUnit );			
			}
		}
	}

	FR_DispExtLcd(szTemp);
}

int
FR_Initialize( int nId )
{
	int i;
	int nResult;

	DEBUGMSG(1,("[1]FR_initialize  : %d\n",tFrConfig.cInitialized ));

	if( tFrConfig.cInitialized ) 
		return -1;

	handle_fm = open( FR_DEV_PATH, O_RDWR );
	if( handle_fm < 0 ) 
		return -1;

	memset( (void*)&tTxDataBuff, 0, sizeof(tTxDataBuff) );
	memset( (void*)&tRxDataBuff, 0, sizeof(tRxDataBuff) );

	// initialize Preset list
	tFrConfig.task_id = nId;
	tFrConfig.cRegion = FR_REGION_USA;					// init InjectionSide flag
	tFrConfig.tCurrentInjectionSide = FR_INJECTION_LO;		// init InjectionSide flag
	tFrConfig.cReceiveStereo = FR_RECEIVED_MONO;			// init Receive Stereo style
	tFrConfig.cUiState = FR_UI_STARTUP;
	tFrConfig.m_cMute = FR_MUTE_OFF;
	tFrConfig.m_cSpeaker = 1;								// init speaker off
	tFrConfig.cStereo = TaviSysParam.radio.cStereo;			// set last value of Stereo
	
	tTxDataBuff.TxFormatType.mute = FR_MUTE_ON;			// mute on
	tTxDataBuff.TxFormatType.stby = FR_STANDBY_OFF;		// standby mode off
	tTxDataBuff.TxFormatType.pllref = FR_REFERENCEPLL_ON;	// reference pll enable
	tTxDataBuff.TxFormatType.bl = FR_GetBandLimit(tFrConfig.cRegion);	// set band limit
	tTxDataBuff.TxFormatType.xtal = FR_XTAL_32KHZ;			// xtal 32khz
	tTxDataBuff.TxFormatType.hcc = 1;						// high cut control on
	tTxDataBuff.TxFormatType.snc = 1;						// stereo noise cancelling on
	tTxDataBuff.TxFormatType.smute = 0;					// software mute off
	tTxDataBuff.TxFormatType.ml = 0;						// left mute off
	tTxDataBuff.TxFormatType.mr = 0;						// right mute off

	nOnConfirm = 0;										// no confirm box

	TAVI_CannotSleep( 510 );								// power save
	nResult = FR_ReadConfigFile();
	TAVI_CanSleep( 510 );								// power save
	if ( nResult ==0 )
	{
		tFrConfig.cTuneMode = FR_TUNEMODE_PRESET;		// init cTunemode to manual			
		tFrConfig.cUiState = FR_UI_PRESETLIST;
		DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );		
		pFrListBox->Display(tFrConfig.nIndexOfPreset);		// get last choice
		FR_DispPreset2extlcd(tFrConfig.nIndexOfPreset);
	}
	else
	{
		DrawBoxWithoutRefresh( 0, 307, 30, 8, 206 );		// clear vertical scroll bar
		if (g_MiscCtrl->HeadphoneIN() )
		{
			nNoHeadPhone = 0;
		}
		else
		{
			pMsgBox->Reset( ML_INFORMATION,
				_ORANGE,
				ML_PLZ_INS_EARPHONE,
				244, 123,
				FR_NoHeadphoneInitConfirmMsgProc,
				DL_BG_ICON
				);
			pMsgBox->Show();
			nOnConfirm = 2;
			nNoHeadPhone = 1;
		}

		if ( nNoHeadPhone == 0 )
		{
			pMsgBox->Reset( ML_INFORMATION,
				_ORANGE,
				ML_AUTO_SCAN_QUERY,
				244, 123,
				FR_InitScanConfirmMsgProc,
				DL_BG_BLANK |DL_BTN_OK_CANCEL
				);
			pMsgBox->Show();
			nOnConfirm = 2;
		}
	}
 	tFrConfig.cInitialized = 1;

        FR_SetMute( FR_MUTE_ON );
	//g_SndCtrl->LineInBypass( true );
 	
	DEBUGMSG(1,("FR_Initialize : nResult =%d\n", nResult) );
	return 0;
}


RMuint16 FR_DrawIcons(  RMuint8 cSelect, RMuint16 fontWidth, RMuint16 fontHeigth,BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY)
{

	if ( cSelect < FR_DRAWICON_MAX )
	{
		pMpegDecoder->RenderOsdBits_WithoutRefresh (
			pBitmap->image_buffer+(fontWidth*fontHeigth*cSelect), usX, usY, fontWidth, fontHeigth );
	}
}

int	
FR_DispAutoScanBox(unsigned int nFrequency)
{
	int 	i;
	int	nMajor;
	int	nMinor;
	char szTemp[20];
	uchar_t ucText[20];
	unsigned int	nPercent;
	
	if (	tFrConfig.cRegion == FR_REGION_JAPAN )
		nPercent = (100*(nFrequency - FR_JAPANFREQUENCY_MIN))/(FR_JAPANFREQUENCY_MAX-FR_JAPANFREQUENCY_MIN);
	else
		nPercent = (100*(nFrequency - FR_USFREQUENCY_MIN))/(FR_USFREQUENCY_MAX-FR_USFREQUENCY_MIN);
	
	if ( nFrequency == 0 )
	{
		DrawBoxWithoutRefresh(
			_DBX_BASE_COLOR_PALLETTE,	// RMuint8 color
			FR_DISPAUTOSCANBOX_X,		// RMuint16 sx
			FR_DISPAUTOSCANBOX_Y,		// RMuint16 sy
			FR_DISPAUTOSCANBOX_W,		//RMuint16 width
			FR_DISPAUTOSCANBOX_H		// RMuint16 height)
			);
		DrawBoxWithoutRefresh(
			_DBX_TITLE_COLOR_PALLETTE,	// RMuint8 color
			FR_DISPAUTOSCANBOX_X,		// RMuint16 sx
			FR_DISPAUTOSCANBOX_Y, 		// RMuint16 sy
			FR_DISPAUTOSCANBOX_W, 		//RMuint16 width
			_DBX_TITLE_HEIGHT			// RMuint16 height)
			);

		DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 0, 
			FR_DISPAUTOSCANBOX_X, FR_DISPAUTOSCANBOX_Y, 4,4 );			// upper left conner
		DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 1, 
			FR_DISPAUTOSCANBOX_X+FR_DISPAUTOSCANBOX_W-4, FR_DISPAUTOSCANBOX_Y, 4,4 );	// upper right conner
		DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 2, 
			FR_DISPAUTOSCANBOX_X, FR_DISPAUTOSCANBOX_Y+FR_DISPAUTOSCANBOX_H-4, 4,4 );	// lower left conner
		DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 3, 
			FR_DISPAUTOSCANBOX_X+FR_DISPAUTOSCANBOX_W-4, FR_DISPAUTOSCANBOX_Y+FR_DISPAUTOSCANBOX_H-4, 4,4 );	// lower right conner

		DrawItemWithoutRefresh(	&TAVI_IMAGES[ICON_DBXICON], 2, 
			FR_DISPAUTOSCANBOX_X+218, FR_DISPAUTOSCANBOX_Y+170, 24, 14 );	// cancel button
		pT->DrawResText(ML_CANCEL,											// disp "Cancel"
			FR_DISPAUTOSCANBOX_X + 249,FR_DISPAUTOSCANBOX_Y + 172,
			_DBX_SELECTED_ITEM_BAR_COLOR,
			12,
			DT_TRANSPARENCY
			);
		pT->DrawResText(ML_CHANNEL_SCAN,									// disp "Channel Scan"
			FR_DISPAUTOSCANBOX_X + 7,FR_DISPAUTOSCANBOX_Y + 4,
			_DBX_SELECTED_ITEM_BAR_COLOR,
			12,
			DT_TRANSPARENCY
			);
		pT->DrawResText(ML_SCANNING,										// disp "scanning..."
			FR_DISPAUTOSCANBOX_X+22, FR_DISPAUTOSCANBOX_Y+64,
			_DBX_SELECTED_ITEM_BAR_COLOR,
			12,
			DT_TRANSPARENCY
			);

 		DrawBoxWithoutRefresh( 27, FR_DISPAUTOSCANBOX_X + 20, FR_DISPAUTOSCANBOX_Y + 105,256,6);
		DrawItemWithoutRefresh( 	&TAVI_IMAGES[PROGRESSBAR_EDGE], 3,
			FR_DISPAUTOSCANBOX_X + 20,	FR_DISPAUTOSCANBOX_Y + 105,	4,6);	// display left part of progress bar 
		DrawItemWithoutRefresh( 	&TAVI_IMAGES[PROGRESSBAR_EDGE], 0,
			FR_DISPAUTOSCANBOX_X + 20 + 254, FR_DISPAUTOSCANBOX_Y + 105, 4,6);	// display right part of progress bar
	}
	else if ( nPercent == 100 )
	{
		DrawItemWithoutRefresh( 	&TAVI_IMAGES[PROGRESSBAR_EDGE], 2,
			FR_DISPAUTOSCANBOX_X + 20 + 254, FR_DISPAUTOSCANBOX_Y + 105, 4,6);	// display right part of progress bar
	}
	else 
	{
		DrawBoxWithoutRefresh( 17, FR_DISPAUTOSCANBOX_X + 20+2, FR_DISPAUTOSCANBOX_Y + 105,(int)(nPercent*2.52),6);
	}

	nMajor =tFrConfig.nCurrentFrequency/1000;				// for Mhz scale
	nMinor =(tFrConfig.nCurrentFrequency -nMajor*1000)/10; 	// for cKhz
	if ( nMajor < 100 )
		sprintf(szTemp,"CH %02d      %d.%02dMHz",tFrConfig.nNoOfPreset, nMajor, nMinor);
	else
		sprintf(szTemp,"CH %02d    %d.%02dMHz",tFrConfig.nNoOfPreset, nMajor, nMinor);
	str2ustr( ucText, szTemp, strlen(szTemp) );
	
	DrawBoxWithoutRefresh( _DBX_BASE_COLOR_PALLETTE, 
		FR_DISPAUTOSCANBOX_X + 22, FR_DISPAUTOSCANBOX_Y + 86, 120, 15);		// clear box

	pT->DrawUC16Text( ucText,
		FR_DISPAUTOSCANBOX_X+22, FR_DISPAUTOSCANBOX_Y+86,
		_DBX_SELECTED_ITEM_BAR_COLOR,
		12,
		DT_TRANSPARENCY | DT_NUMBER_8WIDTH
		); 
}

int 
FR_DispVolumeBar( char cVolume)
{
	int	i;
	int	nPercent;
	char	szTemp[10];
	int	nHalfWidth;
	int	tpv;
	
	nPercent = cVolume;

	DrawBoxWithoutRefresh( 24, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y-16, 256, 16 );	// clear  volume progress bar upper line & "volume"
	DrawBoxWithoutRefresh( 24, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y+6, 256, 1 );		// clear  volume progress bar lower line
	DrawBoxWithoutRefresh( 27, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, 256, 6 );

	if( nPercent == 0 )
		{
		tpv = 0;
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 1, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, FR_DISPCONTROLBAR_X+255-2, FR_DISPCONTROLBAR_Y, 4, 6 );
		}

	else if( nPercent  >= 40 ) {
		tpv = 248;
		DrawBoxWithoutRefresh( _WHITE, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, 256, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 3, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 2, FR_DISPCONTROLBAR_X+255-2, FR_DISPCONTROLBAR_Y, 4, 6 );
		}

	else {
		tpv = (int)(nPercent*6.2);
		DrawBoxWithoutRefresh( _WHITE, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, tpv, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 3, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, FR_DISPCONTROLBAR_X+255-2, FR_DISPCONTROLBAR_Y, 4, 6 );
		}

	DrawItemWithoutRefresh( 
		&TAVI_IMAGES[TOUCHPAD_VOL_N_SEEK_EDGE], 
		4, 
		FR_DISPCONTROLBAR_X + tpv,
		FR_DISPCONTROLBAR_Y-1,
		8, 
		8 
		);

	DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCH_MINI_ICON], 0, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y+9, 12, 7 ); 
	DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCH_MINI_ICON], 1, FR_DISPCONTROLBAR_X+255-12, FR_DISPCONTROLBAR_Y+9, 12, 7 ); 
	nHalfWidth = pT->GetStringWidth( ML_VOLUME, 12, 0 )/2;
	sprintf( szTemp, "%d", nPercent );
	pT->DrawResText( ML_VOLUME, 160-nHalfWidth-9, FR_DISPCONTROLBAR_Y-16, _WHITE, 12, DT_TRANSPARENCY );
	pT->DrawEUC8Text( szTemp, 160+nHalfWidth, FR_DISPCONTROLBAR_Y-16, _GREEN, 12, DT_TRANSPARENCY );

	if( g_MiscCtrl->FolderOpen() == false ) 
	{
		if( g_ExtLCD->GetCurrentMode() != CExtLCD::EXT_STAGE_VOLUME ) {
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_VOLUME );
			g_ExtLCD->Clear();
			//g_ExtLCD->Update();
		}
		g_ExtLCD->UpdateVolume( nPercent );
	}

	return 0;
}

int 
FR_DispFrequencyBar( unsigned int nFrequency)
{
	int nPercent;
	static int nX=nFrequency;
	
	if ( nFrequency == 0 )
		nPercent = 0;
	else
	{
		if (	tFrConfig.cRegion == FR_REGION_JAPAN )
			nPercent = (100*(nFrequency - FR_JAPANFREQUENCY_MIN))/(FR_JAPANFREQUENCY_MAX-FR_JAPANFREQUENCY_MIN);
		else
			nPercent = (100*(nFrequency - FR_USFREQUENCY_MIN))/(FR_USFREQUENCY_MAX-FR_USFREQUENCY_MIN);
	}
	
	DrawBoxWithoutRefresh(	27,
		FR_DISPTUNNERBAR_X+(int)(nX*2.52), FR_DISPTUNNERBAR_Y, 1, 6);			// clear vertical bar
	DrawBoxWithoutRefresh(	_DBX_SELECTED_ITEM_BAR_COLOR,
		FR_DISPTUNNERBAR_X+(int)(nPercent*2.52), FR_DISPTUNNERBAR_Y, 1, 6);		// draw vertical bar
	pMpegDecoder->RenderOsdBits_WithoutRefresh_WithFixedBackgroundColor( 		// clear triangle
		(RMuint8*)szTunnerTriagleEraze, 
		FR_DISPTUNNERBAR_X-3+(int)(nX*2.52),
		FR_DISPTUNNERBAR_Y-5,
		7,		// width
		5,		// height
		_DBX_BASE_COLOR_PALLETTE);

	pMpegDecoder->RenderOsdBits_WithoutRefresh_WithFixedBackgroundColor( 		// draw triangle
		(RMuint8*)szTunnerTriagleNormal, 
		FR_DISPTUNNERBAR_X-3+(int)(nPercent*2.52),
		FR_DISPTUNNERBAR_Y-5,
		7,		// width
		5,		// height
		_DBX_BASE_COLOR_PALLETTE);

	nX = nPercent;
	return TRUE;
}




void FR_DispRegion(void)
{
	switch(tFrConfig.cRegion)
	{
		case FR_REGION_JAPAN:
			FR_DrawIcons(FR_DRAWICON_JAPAN, FR_DRAWICON_W, FR_DRAWICON_H, 
				&TAVI_IMAGES[RADIO_PRESET], FR_DISPREGION_X, FR_DISPREGION_Y);
			break;
		case FR_REGION_EUROPE:
			FR_DrawIcons(FR_DRAWICON_EUROPE, FR_DRAWICON_W, FR_DRAWICON_H, 
				&TAVI_IMAGES[RADIO_PRESET], FR_DISPREGION_X, FR_DISPREGION_Y);
			break;			
		case FR_REGION_KOREA:
			FR_DrawIcons(FR_DRAWICON_KOREA, FR_DRAWICON_W, FR_DRAWICON_H, 
				&TAVI_IMAGES[RADIO_PRESET], FR_DISPREGION_X, FR_DISPREGION_Y);
			break;
		case FR_REGION_USA:
		default :
			FR_DrawIcons(FR_DRAWICON_ENGLISH, FR_DRAWICON_W, FR_DRAWICON_H, 
				&TAVI_IMAGES[RADIO_PRESET], FR_DISPREGION_X, FR_DISPREGION_Y);
			break;
	}
}

void
FR_DispTuneMode(void)
{
	switch(tFrConfig.cTuneMode)
	{
		case FR_TUNEMODE_PRESET:
			FR_DrawIcons(FR_DRAWICON_PRESET, FR_DRAWICON_W, FR_DRAWICON_H, 
				&TAVI_IMAGES[RADIO_PRESET], FR_DISPTUNEMODE_X, FR_DISPTUNEMODE_Y);
			break;			
		case FR_TUNEMODE_MANUAL:
		default :
			FR_DrawIcons(FR_DRAWICON_BLANK, FR_DRAWICON_W, FR_DRAWICON_H, 
				&TAVI_IMAGES[RADIO_PRESET], FR_DISPTUNEMODE_X, FR_DISPTUNEMODE_Y);
			break;
	}
}


FR_RECEIVEDAUDIO_t
FR_DispStereo(void)
{
	FR_RECEIVEDAUDIO_t	tStereo;

	tStereo = (FR_RECEIVEDAUDIO_t)FR_IsStereo();

	if ( FR_GetMono() )
	{
		FR_DrawIcons(FR_DRAWICON_MONO, FR_DRAWICON_W, FR_DRAWICON_H, 
			&TAVI_IMAGES[RADIO_PRESET], FR_DISPSTEREO_X, FR_DISPSTEREO_Y);
		return  FR_RECEIVED_MONO;
	}

	switch(  tStereo )
	{
		case FR_RECEIVED_STEREO:
			FR_DrawIcons(FR_DRAWICON_STEREO, FR_DRAWICON_W, FR_DRAWICON_H, 
				&TAVI_IMAGES[RADIO_PRESET], FR_DISPSTEREO_X, FR_DISPSTEREO_Y);
			break;			
		case FR_RECEIVED_MONO:
		default :
			FR_DrawIcons(FR_DRAWICON_MONO, FR_DRAWICON_W, FR_DRAWICON_H, 
				&TAVI_IMAGES[RADIO_PRESET], FR_DISPSTEREO_X, FR_DISPSTEREO_Y);
			break;
	}
	return tStereo;
}

RMuint16 FR_DrawASCIINumber( char text[], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY)
{
	int i;
	char code;
	int text_length = strlen(text);

	for( i=0; i < text_length; i++ )
	{
//		if ( code >= '.' && code <='9')
		{
			code = text[i] -'-';
			DEBUGMSG(0,("\n[2]FR_DrawASCIINumber %c [0x%x]\n", text[i],code));

			pMpegDecoder->RenderOsdBits_WithoutRefresh (
				pBitmap->image_buffer+(fontWidth*fontHeigth*code), usX, usY, fontWidth, fontHeigth );
		}
		switch( text[i])
		{	
			case '.':
				usX += 18;
				break;
			default:
				usX += fontWidth-6;
		}

	}

	return TRUE;
	
}

void FR_DispExtLcd( char *szTemp )
{
	CExtLCD::ExtLCDListItem item;

	if ( tFrConfig.cUiState == FR_UI_PRESETLIST )
	{
		 g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );
		item.m_nIcon = CExtLCD::EXT_LCD_ICON_RADIO;
		item.m_nIndex = tFrConfig.nIndexOfPreset;
		item.m_nTotal = tFrConfig.nNoOfPreset;
		item.m_szText = szTemp;
		g_ExtLCD->SetListItem( item );

	}
	else
	{
           	CString szTitle = szTemp;
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_PLAY );
//		g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_PLAY );
		g_ExtLCD->SetPlayTitle( szTitle );
	}	

	 if ( !g_MiscCtrl->FolderOpen() )
		g_ExtLCD->Update();

}

void FR_DispFrequency(void)
{
	int	nMajor;
	int	nMinor;
	unsigned int nPercent;
	char szTemp[32];
	char szTemp2[32];
	uchar_t ucChNo[10];
	uchar_t ucChPrev[32];
	uchar_t ucChNext[32];
	int	nChNo;
		
	
	DrawBoxWithoutRefresh( 0, 0,22,320, 170);					// clear warnning box for headphone

	FR_DrawFrame(FRBIGICON);

	if ( FR_CheckOutOfBand( tFrConfig.nCurrentFrequency ) < 0 )
	{
		sprintf(szTemp, "---.--/");
	}
	else
	{
		nMajor =tFrConfig.nCurrentFrequency/1000;				// for Mhz scale
		nMinor =(tFrConfig.nCurrentFrequency -nMajor*1000)/10; 	// for cKhz
		sprintf ( szTemp, "%3d.%02d/", nMajor,nMinor );
	}
	DEBUGMSG(1,("\nCFreq=%d, DispFrequency : %s\n",tFrConfig.nCurrentFrequency, szTemp) );

	FR_DrawASCIINumber(szTemp, 36, 37,  &TAVI_IMAGES[LARGE_DIGIT], 
		FR_DISPFREQUENCY_X, FR_DISPFREQUENCY_Y);			// disp frequency
	if ( nMajor < 100)											// clear freq's 0 
	{
		pMpegDecoder->RenderText_WithoutRefresh(0, 0,
			FR_DISPFREQUENCY_X, FR_DISPFREQUENCY_Y,
			30,
			37
			);
	}


	if ( tFrConfig.cTuneMode == FR_TUNEMODE_MANUAL )
	{
		FR_DispPreset2extlcd( tFrConfig.nIndexOfPreset );
		
		DrawBoxWithoutRefresh( _DBX_SELECTED_ITEM_BAR_COLOR, 
			FR_DISPCHNO_X, FR_DISPCHNO_Y,
			46,
			16
			);		// clear channel no box
	}	
	else		// FR_TUNEMODE_PRESET
	{
		FR_DispPreset2extlcd( tFrConfig.nIndexOfPreset );

		sprintf( szTemp,"Ch %02d",tFrConfig.nArChannelNo[tFrConfig.nIndexOfPreset]);

		DrawBoxWithoutRefresh( _DBX_SELECTED_ITEM_BAR_COLOR,
			FR_DISPCHNO_X, FR_DISPCHNO_Y,
			46,
			16
			);		// clear channel no box
		str2ustr( ucChNo, szTemp, strlen(szTemp) );
		pT->DrawUC16Text( ucChNo, FR_DISPCHNO_X, FR_DISPCHNO_Y,
			21,
			16,
			DT_TRANSPARENCY
			);

		if ( tFrConfig.nIndexOfPreset  == 0 )
		{
			sscanf(tFrConfig.szArPresetStrings[tFrConfig.nNoOfPreset-1],"%s", szTemp);
			strncat(szTemp,tFrConfig.szArPresetStrings[tFrConfig.nNoOfPreset-1]+strlen(szTemp)+6,15);
//			sprintf( szTemp,"----");
		}
		else
		{
			sscanf(tFrConfig.szArPresetStrings[tFrConfig.nIndexOfPreset-1],"%s", szTemp);
			strncat(szTemp,tFrConfig.szArPresetStrings[tFrConfig.nIndexOfPreset-1]+strlen(szTemp)+6,15);
		}
		str2ustr( ucChPrev, szTemp, strlen(szTemp) );
		pT->DrawUC16Text( ucChPrev, 90, 142,						// disp " Ch00  00.00 MHz"
			_DBX_SELECTED_ITEM_BAR_COLOR,
			12,
			DT_TRANSPARENCY | DT_OUTLINE | DT_NUMBER_8WIDTH
			);
		pT->DrawResText( ML_PREV , 46, 142,						// disp "Prev"
			_DBX_SELECTED_ITEM_BAR_COLOR, 
			12,
			DT_TRANSPARENCY| DT_OUTLINE
			);
		DrawItemWithoutRefresh(&TAVI_IMAGES[TYPE_ICON_LIST],	// draw icon
			32,
			30,
			142,
			12, 
			12 
			);

		if ( tFrConfig.nIndexOfPreset + 1 == tFrConfig.nNoOfPreset )
		{
			sscanf(tFrConfig.szArPresetStrings[0],"%s", szTemp);
			strncat(szTemp,tFrConfig.szArPresetStrings[0]+strlen(szTemp)+6,15);
//			sprintf( szTemp,"----");
		}
		else
		{
			sscanf(tFrConfig.szArPresetStrings[tFrConfig.nIndexOfPreset+1],"%s", szTemp);
			strncat(szTemp,tFrConfig.szArPresetStrings[tFrConfig.nIndexOfPreset+1]+strlen(szTemp)+6,15);
		}
		str2ustr( ucChNext, szTemp, strlen(szTemp) );
		pT->DrawUC16Text( ucChNext, 90, 159, 						// disp " Ch00  00.00 MHz"
			_DBX_SELECTED_ITEM_BAR_COLOR,
			12,
			DT_TRANSPARENCY | DT_OUTLINE | DT_NUMBER_8WIDTH
			);
		pT->DrawResText(ML_NEXT , 46, 159,						// disp "Next"
			_DBX_SELECTED_ITEM_BAR_COLOR,
			12,
			DT_TRANSPARENCY | DT_OUTLINE
			);
		DrawItemWithoutRefresh(&TAVI_IMAGES[TYPE_ICON_LIST],	// draw icon
			34,
			30,
			159,
			12, 
			12 
			);

//		FR_DispPreset2extlcd(tFrConfig.nIndexOfPreset);				// display sub lcd

	}
	
	if ( g_MiscCtrl->HeadphoneIN() )
	{
		nNoHeadPhone = 0;
	}
	else
	{
		FR_DispNoHeadPhoneMsgBox( ML_PLZ_INS_EARPHONE, 
			FR_DISPHEADPHONE_X,FR_DISPHEADPHONE_Y,
			FR_DISPHEADPHONE_W, FR_DISPHEADPHONE_H
			);
		nNoHeadPhone = 1;
	}

}


#define RET_SCAN_OUTOFBAND	0
#define RET_FREQ_SETFAIL		-1
#define RET_SCAN_TIMEOUT		-2
#define RET_CANCEL_AUTOSCAN	-3
#define RET_ACCESS_FAIL			-4

int
FR_AutoScan( void )
{
	FR_TXDATA_t *pTxBuf;
	FR_RXDATA_t *pRxBuf;
	FR_INJECTION_t tInjectionSide;
	FR_SEARCHRESULT_t	tResult;
	int	nResult;
	int 	nStartFreq;
	int 	nEndFreq;
	int 	nCurrentFreq;
	int 	nPercent;
	int 	nTimeOut;
	int 	nSavedFrequency;
	TMsgEntry tMsg;

	nResult	= RET_SCAN_OUTOFBAND;
	
	pTxBuf = &tTxDataBuff;
	pRxBuf = &tRxDataBuff;
//	tFrConfig.cUiState = FR_UI_SCAN;					// ui state to scan

	FR_InitPresetList();								// clear Preset list
	FR_SetMute( FR_MUTE_ON );						// mute off
	pFrListBox->DeleteAll();

	pMpegDecoder->RenderText_WithoutRefresh(0,0,0,22,320,240-22);		// Clear OSD
	FR_DrawFrame( AUTOSCAN );						// display autoscan box
	

	if( pTxBuf->TxFormatType.bl == FR_BANDLIMIT_US )
	{
		nStartFreq = FR_USFREQUENCY_MIN;
		nEndFreq = FR_USFREQUENCY_MAX;
	}
	else
	{
		nStartFreq = FR_JAPANFREQUENCY_MIN;
		nEndFreq = FR_JAPANFREQUENCY_MAX;
	}

	nTimeOut = FR_SEARCHTIMEOUT;
	nCurrentFreq = nStartFreq;
	nSavedFrequency = tFrConfig.nCurrentFrequency;
	
	DEBUGMSG(1,("start freq=%d\n",nStartFreq) );

	nResult = FR_SetPll( FR_INJECTION_LO, nStartFreq );
	if ( nResult < 0 )
		return RET_FREQ_SETFAIL;						// freq set fail
		
	while( nCurrentFreq <= nEndFreq )
	{

		FR_DispAutoScanBox( nCurrentFreq );
		pMpegDecoder->RefreshOsd();	

		if( MSG_Peek( tFrConfig.task_id, &tMsg ) == 0)
		{
			DEBUGMSG(1,("[0]FMRadio.cpp: FR_AutoScan()\n"));
			if ( tMsg.ulMsgId == MSG_BUTTON_UP )
				if (tMsg.usParam1 == TAVI_KEY_EXIT)
				{
					nResult = RET_CANCEL_AUTOSCAN;	// cancel autoscan
					break;
				}
		}

		
		tResult = FR_SearchStation( FR_SEARCH_UP, FR_SSL_MID );
		nCurrentFreq = FR_GetPll();

		if ( nTimeOut == 0)
		{
			nResult = RET_SCAN_TIMEOUT;
			break;
		}
		if ( tResult == FR_STATION_FOUND )
		{
			FR_AddPreset( tFrConfig.nNoOfPreset, tFrConfig.nCurrentFrequency,  tFrConfig.nMaxChannelNo +1);
			tFrConfig.nIndexOfPreset = tFrConfig.nNoOfPreset -1;
			nTimeOut = FR_SEARCHTIMEOUT;
		}
		else if ( tResult == FR_STATION_OUTOFBAND )
		{
			DEBUGMSG(1,("FMRadio.cpp: FR_AutoScan : FR_STATION_OUTOFBAND\n"));
			nResult = RET_SCAN_OUTOFBAND;
			break;								// band limit
		}
		else		
		{
			//  FR_STATION_ACCESSFAIL 
			//  FR_STATION_NOTFOUND
			nResult = RET_ACCESS_FAIL;
			break;
		}
		DEBUGMSG(1,( "[2]FR_AutoScan: nPreIdx:%d, cFreq:%d, Injec:%d, tResult:%d, timeOut:%d \n", 
				tFrConfig.nIndexOfPreset,
				tFrConfig.nCurrentFrequency,
				tFrConfig.tCurrentInjectionSide,
				tResult,
				nTimeOut
				)
		);		
		nTimeOut--;									// decrease timer

	}
//	FR_SetMute( FR_MUTE_OFF );						// mute off
	FR_SetMute( FR_MUTE_ON );						// mute on

	return nResult;									// complete sacn
}


void FR_DrawFrame(U32 flags )
{
	int	i;

	if ( FRICON & flags )
	{
		DrawItemWithoutRefresh(&TAVI_IMAGES[RADIO_SMALL_ICON],0,
			FR_DISPTITLEICON_X,FR_DISPTITLEICON_Y, 24,18 );			// disp icon
	}
	if ( FRTITLE & flags )
	{	// disp "Radio"
		pT->DrawResText(ML_RADIO, 
			FR_DISPTITLETEXT_X, FR_DISPTITLETEXT_Y,
			_DBX_SELECTED_ITEM_BAR_COLOR,
			16,
			DT_TRANSPARENCY | DT_OUTLINE | DT_BOLD
			);
	}
	if ( FRPRESETTITLE & flags )
	{	// disp "Preset Channel List"
		pT->DrawResText(ML_PRESET_LIST,
			FR_DISPTITLETEXT_X, FR_DISPTITLETEXT_Y, 
			_DBX_SELECTED_ITEM_BAR_COLOR,
			16,
			DT_TRANSPARENCY | DT_BOLD
			);
	}
	if ( FRBIGICON & flags )
	{
		DrawItemWithoutRefresh(&TAVI_IMAGES[RADIO_BIG_ICON],0,
			FR_DISPBIGICON_X, FR_DISPBIGICON_Y, 
			FR_DISPBIGICON_W, FR_DISPBIGICON_H 
			);		// disp big icon
	}
	if ( BOTTOMFRAME & flags )
	{
		DrawBoxWithoutRefresh(
			24,			// RMuint8 color
			0,			// RMuint16 sx
			198, 		// RMuint16 sy
			320, 		//RMuint16 width
			42			// RMuint16 height)
		);
		DrawItemWithoutRefresh(&TAVI_IMAGES[CONTROLPANNEL_EDGE], 0, 0, 198, 4,4 );		// upper left conner
		DrawItemWithoutRefresh(&TAVI_IMAGES[CONTROLPANNEL_EDGE], 1, 316, 198, 4,4 );		// upper right connner
//		DrawItemWithoutRefresh( &TAVI_IMAGES[ICON_BOTTOM_RIGHT], 0,
//			FR_DISPRBUTTON_X, FR_DISPRBUTTON_Y, 20, 20 ); 	// lower right button	
	}
	if ( FREQUENCYBAR & flags )
	{
		DrawBoxWithoutRefresh( 27, FR_DISP875MHZ_X, FR_DISPCONTROLBAR_Y, 256, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 1, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, FR_DISPCONTROLBAR_X+255-2, FR_DISPCONTROLBAR_Y, 4, 6 );

		DrawBoxWithoutRefresh( 24, 0, FR_DISP875MHZ_Y, 320, 7 );			// clean frequency "76MHz 108MHz"
		
		if (	tFrConfig.cRegion == FR_REGION_JAPAN )
		{
			DrawASCIITextWithoutRefresh( "76", 8, 7, &TAVI_IMAGES[FONT_DIGIT3],
				FR_DISP875MHZ_X, FR_DISP875MHZ_Y,__DIGIT3);				// disp "76"
			DrawItemWithoutRefresh( &TAVI_IMAGES[RADIO_DIGIT_MHZ], 0, 	
				FR_DISP875MHZ_X+20, FR_DISP875MHZ_Y, 16, 7 ); 				// disp "MHz"
		}
		else
		{
			DrawASCIITextWithoutRefresh( "87.5", 8, 7, &TAVI_IMAGES[FONT_DIGIT3],
				FR_DISP875MHZ_X, FR_DISP875MHZ_Y,__DIGIT3);				// disp "87.5"
			DrawItemWithoutRefresh( &TAVI_IMAGES[RADIO_DIGIT_MHZ], 0, 	
				FR_DISP875MHZ_X+28, FR_DISP875MHZ_Y, 16, 7 ); 				// disp "MHz"
		}
		DrawASCIITextWithoutRefresh( "108", 8, 7, &TAVI_IMAGES[FONT_DIGIT3],
			FR_DISP1080MHZ_X, FR_DISP1080MHZ_Y,__DIGIT3);				// disp "108"
		DrawItemWithoutRefresh( &TAVI_IMAGES[RADIO_DIGIT_MHZ], 0, 
			FR_DISP1080MHZ_X+20, FR_DISP875MHZ_Y, 16, 7 ); 				// disp "MHz"
	}
	if ( VOLUMEBAR & flags )
	{
		DrawBoxWithoutRefresh( 27, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, 256, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 1, FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, FR_DISPCONTROLBAR_X+255-2, FR_DISPCONTROLBAR_Y, 4, 6 );
	}
	if ( AUTOSCAN & flags )
	{
		FR_DispAutoScanBox( 0 );
	}
/*	
	if ( PRESETMENUBAR & flags )
	{

		DrawBoxWithoutRefresh( 
			_DBX_SELECTED_ITEM_BAR_COLOR,
			FR_DISPCONTROLBAR_X,
			FR_DISPCONTROLBAR_Y+1,
			320 - FR_DISPCONTROLBAR_X *2,
			16
			);

		DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCHPAD_MENU_EDGE], 0, 		// left edge
			FR_DISPCONTROLBAR_X, FR_DISPCONTROLBAR_Y, 8, 18 );
		
		DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCHPAD_MENU_EDGE], 1, 		// right edge
			320 - FR_DISPCONTROLBAR_X -8 , FR_DISPCONTROLBAR_Y, 8, 18 );

		DrawBoxWithoutRefresh( 
			24, 
			116,				//MENUBAR_SX + (int)(MENUBAR_WIDTH/3),
			FR_DISPCONTROLBAR_Y + 4,
			1,
			10
			);
			
		DrawBoxWithoutRefresh( 
			24, 
			202,				//MENUBAR_SX + (int)((int)(MENUBAR_WIDTH/3)*2),
			FR_DISPCONTROLBAR_Y + 4,
			1,
			10
			);

		i = pT->GetStringWidth( ML_AUTO_SCAN, 12, DT_TRANSPARENCY );	// disp "auto scan"
		i = FR_DISPCONTROLSETUP_X + FR_DISPCONTROLBAR_W- (i/2);
		pT->DrawResText( ML_AUTO_SCAN, i, FR_DISPCONTROLSETUP_Y , _BLACK, 12, DT_TRANSPARENCY );

		i = pT->GetStringWidth( ML_DELETE, 12, DT_TRANSPARENCY );		// disp "delete"
		i = FR_DISPCONTROLPRESET_X + FR_DISPCONTROLBAR_W - (i/2);
		pT->DrawResText( ML_DELETE, i, FR_DISPCONTROLSETUP_Y , _BLACK, 12, DT_TRANSPARENCY );
	
	}
*/	
}

void FR_StartTimer(int nMsec, int& nIdTimer )
{
	if ( nIdTimer > -1)									// reset timer to 5sec
	{
		DeregisterTimer( nIdTimer );
		DEBUGMSG(1,("\n[3]FR_StartTimer Deregister=%d\n", nIdTimer ));
	}
	
	pFrTimer = new CTimer();								// create timer
	pFrTimer->m_nExpireTime = nMsec;						// set 5 sec for timeout
	pFrTimer->m_nTaskID = tFrConfig.task_id;
	pFrTimer->m_nType = CTimer::TIMER_ONESHOT;

	if ( (nIdTimer=RegisterTimer( pFrTimer )) < 0 )		// regist timer
	{
		fprintf(stderr,"FR_StartTimer : Error at RegisterTimer IdTimer=%d \n", nIdTimer);
		delete pFrTimer;
		nIdTimer = -1;
	}
	

	DEBUGMSG(1,("\n[3]FR_StartTimer Register=%d\n", nIdTimer ));
}

void FR_StopTimer(int& nIdTimer)
{
	if ( nIdTimer > -1) 								// if exist then deregist
	{
		DeregisterTimer( nIdTimer );
		DEBUGMSG(1,("\n[3]FR_StopTimer Deregister=%d\n", nIdTimer ));
	}
	nIdTimer = -1;
}

int 
FR_MsgProcForPresetList( int usParam1 )
{
	TMsgEntry	tMsg;
	unsigned int	nIndex;


	switch( usParam1 )
	{

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_JOG1_DOWN:
		    g_SndCtrl->VolumeDown();
			break;			

		case TAVI_KEY_JOG2_UP:
		case TAVI_KEY_UP:
			pFrListBox->CursorUp();
			DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );
			pFrListBox->ReDraw();
			tFrConfig.nIndexOfPreset = pFrListBox->GetCurrentChoice();
			FR_DispPreset2extlcd(pFrListBox->GetCurrentChoice());
			break;

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_JOG1_UP:
		    g_SndCtrl->VolumeUp();
			break;

		case TAVI_KEY_JOG2_DOWN:
		case TAVI_KEY_DOWN:
			pFrListBox->CursorDown();
			DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );
			pFrListBox->ReDraw();
			tFrConfig.nIndexOfPreset = pFrListBox->GetCurrentChoice();
			FR_DispPreset2extlcd(pFrListBox->GetCurrentChoice());		
			break;

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_EQ:
			break;
		
		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_JOG2_ENTER:
			if ( g_MiscCtrl->FolderOpen() ) {
				FR_PlayPause();
				break;
			}
		case TAVI_KEY_RIGHT:
		case TAVI_KEY_ENTER:
			// Exit DialogBox
			DEBUGMSG(1,("[1]FR_MsgProcForPresetList :  TAVI_KEY_ENTER\n" ));

                    
                    if( g_TopPanel->GetPlayMode() == CTopPanel::PLAYMODE_STOP ) {
                        // Avoid pop-noise!!!
                        g_MiscCtrl->Mute( true );
                        
                        g_SndCtrl->LineInBypass( true );
                        
                        g_MiscCtrl->Mute( false );
                    }
                    
			tFrConfig.cUiState = FR_UI_NORMAL;
			pMpegDecoder->RenderText_WithoutRefresh(0,0,0,22,320,240-2);		// Clear OSD	


			tFrConfig.nIndexOfPreset= pFrListBox->GetCurrentChoice();
			pFrListBox->SetOnState( 1 );							// maker on at presetlist
			g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_PLAY );
			g_TopPanel->Update();
			
			FR_TunningFreq( tFrConfig.nArPresetedFrequncy[tFrConfig.nIndexOfPreset]);
			tFrConfig.cTuneMode = FR_TUNEMODE_PRESET;			// init cTunemode to manual
			SetTaviKeyMode(TK_REPEAT);
			pPopUpMain_Preset->SelectedMenuItemIdx = 0;
			pPopUpMenu=pPopUpMain1;
			FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR );
			FR_DispFrequency();
			FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
			FR_DispTuneMode();
			FR_DispRegion();
			FR_SetMono( (RMuint8*)&tFrConfig.cStereo);
			tFrConfig.cReceiveStereo = FR_DispStereo();
			FR_SetMute( FR_MUTE_OFF );							// mute off
			break;
			
		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_MENU:
			DEBUGMSG(1,("[1]FR_MsgProcForPresetList :  TAVI_KEY_MENU\n" ));

			if ( pPopUpMenu->IsVisible() )
				pPopUpMenu->Hide(false);
			else
				pPopUpMenu->Show();
			break;
			
		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_LEFT:
			break;

		case TAVI_KEY_LEXIT:
			tFrConfig.m_ExitState = TaskExit_ExitAndSwitch;
			tFrConfig.m_fExit = true;
			return -1;
			break;

		case TAVI_KEY_LMODE:
			tFrConfig.m_ExitState = TaskExit_Switch;
			tFrConfig.m_fExit = true;
			return -1;
			break;
			
		case TAVI_KEY_MODE:
			DEBUGMSG(1,("[1]FR_MsgProcForButtonUpInUiNormal() : TAVI_LEXIT\n" ));
			tFrConfig.m_ExitState = TaskExit_Background;
			tFrConfig.m_fExit = true;		
			return -1;
			break;

		case TAVI_KEY_JOG1_ENTER:
			if ( g_MiscCtrl->FolderOpen() ) break;
		case TAVI_KEY_EXIT:
			if ( pPopUpMenu->IsVisible() )
				pPopUpMenu->Hide(false);
			else
			{
				if ( pFrListBox->GetOnState() > 0 ) {
					tFrConfig.cUiState = FR_UI_NORMAL;
					pPopUpMain1->ChangeSubMenu(ML_SPEAKER, pPopUpMain_Speaker, 1, 0, 0);
					pPopUpMenu=pPopUpMain1;
				}
				tFrConfig.m_ExitState = TaskExit_Normal;
				tFrConfig.m_fExit = true;
				return -1;
			}
			break;
/*
		case TAVI_KEY_JOG1_UP:
			DEBUGMSG(0,("[1]FR_MsgProcForPresetList() :  TAVI_KEY_JOG1_UP\n" ));
			if ( pFrListBox->GetOnState() )
			{
				SetVolume( 1 );			// volume down
				SetVolume( 1 );			// volume down
			}
			break;

		case TAVI_KEY_JOG1_DOWN:
			DEBUGMSG(0,("[1]FR_MsgProcForPresetList() :  TAVI_KEY_JOG1_DOWN\n" ));
			SetVolume( -1 );			// volume down
			SetVolume( -1 );			// volume down
			break;
*/
	}
	return 0;
}

int
FR_Dummy(void)
{
	return 0;
}


int FR_ChannelScan( RMuint8 *dummy )
{
	pPopUpMenu->Hide(false, false);
	if ( g_MiscCtrl->HeadphoneIN() )
	{
		pMsgBox->Reset( ML_INFORMATION,
			_ORANGE,
			ML_AUTO_SCAN_QUERY,
			244, 123,
			FR_ReScanConfirmMsgProc,
			DL_BG_BLANK |DL_BTN_OK_CANCEL
			);
		pMsgBox->Show();
		nOnConfirm = 2;
		nNoHeadPhone = 0;
	}
	else
	{
		pMsgBox->Reset( ML_INFORMATION,
			_ORANGE,
			ML_PLZ_INS_EARPHONE,
			244, 123,
			FR_NoHeadphoneConfirmMsgProc,
			DL_BG_ICON
			);
		pMsgBox->Show();
		nOnConfirm = 2;
		nNoHeadPhone = 1;
	}
	return 0;
}

int FR_SetPreset( RMuint8* OnPreset )
{
	if(  *OnPreset == 0)
	{
		tFrConfig.cTuneMode = FR_TUNEMODE_PRESET;
		pPopUpMenu = pPopUpMain1;
		SetTaviKeyMode(TK_REPEAT);
	}
	else if ( *OnPreset == 1 )
	{
		tFrConfig.cTuneMode = FR_TUNEMODE_MANUAL;
		pPopUpMenu = pPopUpMain2;
		SetTaviKeyMode(TK_LONG);
	}

	FR_DispFrequency();
	FR_DispTuneMode();
	pMpegDecoder->RefreshOsd ();							// refresh screen
	return 0;
}

int FR_SetSpeaker( RMuint8* pOffOn )
{

	if ( *pOffOn == 0)
	{
		SpeakerOnOff( 0 );		// speaker on
	}
	else
	{
		SpeakerOnOff( 1 );		// speaker off
	}
	tFrConfig.m_cSpeaker = *pOffOn;
	return 0;
}

int FR_AddChannel( RMuint8 *dummy )
{
	int i;
	char	szTemp[10];
	uchar_t ucChNo[10];

	pPopUpMenu->Hide(false);
	
	if (tFrConfig.cTuneMode == FR_TUNEMODE_MANUAL)
	{
		FR_AddPreset( tFrConfig.nNoOfPreset, tFrConfig.nCurrentFrequency, tFrConfig.nMaxChannelNo+1 );
		tFrConfig.nIndexOfPreset = tFrConfig.nNoOfPreset -1;

		FR_TunningFreq( tFrConfig.nArPresetedFrequncy[tFrConfig.nIndexOfPreset]);
//		tFrConfig.cTuneMode = FR_TUNEMODE_PRESET;
//		FR_DispFrequency();	
		sprintf( szTemp,"Ch %02d",tFrConfig.nArChannelNo[tFrConfig.nIndexOfPreset]);
		str2ustr( ucChNo, szTemp, strlen(szTemp) );
		pT->DrawUC16Text( ucChNo, FR_DISPCHNO_X, FR_DISPCHNO_Y,
			21,
			16,
			DT_TRANSPARENCY
			);
		pMpegDecoder->RefreshOsd ();							// refresh screen
		for(i=0;i<10;i++)
			usleep(10000);
		DrawBoxWithoutRefresh( _DBX_SELECTED_ITEM_BAR_COLOR,
			FR_DISPCHNO_X, FR_DISPCHNO_Y,
			46,
			16
			);													// clear channel no box
		pMpegDecoder->RefreshOsd ();							// refresh screen
		for(i=0;i<10;i++)
			usleep(10000);
		pT->DrawUC16Text( ucChNo, FR_DISPCHNO_X, FR_DISPCHNO_Y,
			21,
			16,
			DT_TRANSPARENCY
			);
		pMpegDecoder->RefreshOsd ();							// refresh screen
		for(i=0;i<10;i++)
			usleep(10000);
		DrawBoxWithoutRefresh( _DBX_SELECTED_ITEM_BAR_COLOR,
			FR_DISPCHNO_X, FR_DISPCHNO_Y,
			46,
			16
			);													// clear channel no box
	}
	return 0;
}

int FR_Record ( RMuint8 *dummy )
{
	CRecorderWnd* pRecWnd;

	pPopUpMenu->Hide(false);

	pMpegDecoder->RenderText(0,0,0,0,320,240);
	tFrConfig.cUiState = FR_UI_RECORD;
	pRecWnd = new CRecorderWnd( 
		NULL, _RECORD_DIR, 
		CRecorderWnd::RECORD_FM, 
		tFrConfig.nCurrentFrequency );
	pRecWnd->Resize( CRect(0, 30, 319, 239), false );
	pRecWnd->SetFocus();
	pRecWnd->SetWindowList( &tFrConfig.m_WndList );
	pRecWnd->RegisterWndList();
	g_TopPanel->SetTitle( ML_RECORD );
	g_TopPanel->SetLogo( CTopPanel::LOGO_ID_RECORD );
	g_TopPanel->Show();

	return 0;
}

int FR_PlayPause( void )
{
	if ( tFrConfig.m_cMute )
	{
		tFrConfig.m_cMute = FR_MUTE_OFF;		// set play
		pFrListBox->SetOnState( 1 );			// play icon on List
		FR_SetMute( FR_MUTE_OFF );
		g_MiscCtrl->Mute(false);
		g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_PLAY );
		g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_PLAY );	
	}
	else
	{
		tFrConfig.m_cMute = FR_MUTE_ON;		// set pause
		pFrListBox->SetOnState( 2 );			// pause icon on List
		FR_SetMute( FR_MUTE_ON );
		g_MiscCtrl->Mute(true);
		g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_PAUSE );
		g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_PAUSE);	
	}
	g_TopPanel->Update();
	if ( !g_MiscCtrl->FolderOpen() )
		g_ExtLCD->Update();

	return 0;
}

int FR_MsgProcForButtonUpInUiNormal(int usParam1)
{
	switch( usParam1 )
	{

		case TAVI_KEY_LEXIT:
			tFrConfig.m_ExitState = TaskExit_ExitAndSwitch;
			tFrConfig.m_fExit = true;
			return -1;
			break;

		case TAVI_KEY_LMODE:
			tFrConfig.m_ExitState = TaskExit_Switch;
			tFrConfig.m_fExit = true;
			return -1;
			break;
			
		case TAVI_KEY_MODE:
			DEBUGMSG(1,("[1]FR_MsgProcForButtonUpInUiNormal() : TAVI_KEY_MODE\n" ));
			tFrConfig.m_ExitState = TaskExit_Background;
			tFrConfig.m_fExit = true;		
			return -1;
			break;

		case TAVI_KEY_EXIT:
		case TAVI_KEY_JOG1_ENTER:			
			DEBUGMSG(1,("[1]FR_MsgProcForButtonUpInUiNormal() : TAVI_KEY_EXIT\n" ));
			if ( tFrConfig.nNoOfPreset == 0 )
			{
				tFrConfig.m_ExitState = TaskExit_Normal;
				tFrConfig.m_fExit = true;
				return -1;		// exit to main menu
			}
			else
			{
				pMpegDecoder->RenderText_WithoutRefresh(0,0,0,22,320,240-22);	// Clear OSD	
				tFrConfig.cUiState = FR_UI_PRESETLIST;
				SetTaviKeyMode(TK_REPEAT);
				DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );
				pFrListBox->ReDraw( tFrConfig.nIndexOfPreset );
				FR_DispPreset2extlcd( tFrConfig.nIndexOfPreset );
				FR_StartTimer(10000, nIdTimer4BackOn);
				pPopUpMenu = pPopUpList;
			}
			break;
			
		case TAVI_KEY_EQ:
			break;
			
		case TAVI_KEY_LMENU:
			break;

		case TAVI_KEY_MENU:						
			DEBUGMSG(0,("[1]FR_MsgProcForButtonUpInUiNormal() :  (UI_NORMAL) :TAVI_KEY_MENU\n" ));
//			mep_set_velocity( MEP_VELOCITY_80 );			// change touch speed
			pPopUpMenu->Show();
			break;

		case TAVI_KEY_JOG1_UP:
		case TAVI_KEY_UP:
		case TAVI_KEY_JOG1_LUP:
		case TAVI_KEY_LUP:
			tFrConfig.cUiState = FR_UI_VOLUME;
			g_SndCtrl->VolumeUp();
			FR_DrawFrame( BOTTOMFRAME );
			FR_DispVolumeBar( g_SndCtrl->GetVolume() );
			FR_StartTimer(5000, nIdTimer4Menu);	
			SetTaviKeyMode(TK_REPEAT);
			break;
			
		case TAVI_KEY_JOG1_DOWN:
		case TAVI_KEY_DOWN:
		case TAVI_KEY_JOG1_LDOWN:
		case TAVI_KEY_LDOWN:
			tFrConfig.cUiState = FR_UI_VOLUME;
			g_SndCtrl->VolumeDown();
			FR_DrawFrame( BOTTOMFRAME );
			FR_DispVolumeBar( g_SndCtrl->GetVolume() );
			FR_StartTimer(5000, nIdTimer4Menu);	
			SetTaviKeyMode(TK_REPEAT);
			break;

		case TAVI_KEY_JOG2_DOWN:
		case TAVI_KEY_LEFT:
			DEBUGMSG(0,("[1]FR_MsgProcForButtonUpInUiNormal() :  (UI_NORMAL) :TAVI_KEY_LEFT\n" ));
			FR_TunningUpDown( FR_TUNEDIRECTION_DOWN );	// tunning down
			FR_DispFrequency();
			tFrConfig.cReceiveStereo = FR_DispStereo();			// disp stereo/mono
			FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
			break;

		case TAVI_KEY_JOG2_UP:
		case TAVI_KEY_RIGHT:
			DEBUGMSG(1,("[1]FR_MsgProcForButtonUpInUiNormal() :  (UI_NORMAL) :TAVI_KEY_RIGHT\n" ));
			FR_TunningUpDown( FR_TUNEDIRECTION_UP );		// tunning down
			FR_DispFrequency();
			tFrConfig.cReceiveStereo = FR_DispStereo();			// disp stereo/mono
			FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
			break;

		case TAVI_KEY_JOG2_LDOWN:
		case TAVI_KEY_LLEFT:
			DEBUGMSG(0,("[1]FR_MsgProcForButtonUpInUiNormal() :  (UI_NORMAL) :TAVI_KEY_LLEFT\n" ));
			if ( tFrConfig.cTuneMode == FR_TUNEMODE_MANUAL )
			{
				FR_SEARCHRESULT_t	nResult;
				
				nResult = FR_SearchStation( FR_SEARCH_DOWN, FR_SSL_MID );	// scan down
				if ( nResult == FR_STATION_OUTOFBAND )
				{
					FR_TunningUpDown( FR_TUNEDIRECTION_DOWN );
					FR_SearchStation( FR_SEARCH_DOWN, FR_SSL_MID );		// scan down
				}
				if ( tFrConfig.m_cMute == FR_MUTE_OFF )
					FR_SetMute( FR_MUTE_OFF );							// mute off
				FR_DispFrequency();
				tFrConfig.cReceiveStereo = FR_DispStereo();					// disp stereo/mono
				FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
			}
			break;

		case TAVI_KEY_JOG2_LUP:
		case TAVI_KEY_LRIGHT:
			DEBUGMSG(1,("[1]FR_MsgProcForButtonUpInUiNormal() :  (UI_NORMAL) :TAVI_KEY_LRIGHT\n" ));
			if ( tFrConfig.cTuneMode == FR_TUNEMODE_MANUAL )
			{
				FR_SEARCHRESULT_t	nResult;
				
				nResult = FR_SearchStation( FR_SEARCH_UP, FR_SSL_MID );	// scan down
				if ( nResult == FR_STATION_OUTOFBAND )
				{
					FR_TunningUpDown( FR_TUNEDIRECTION_UP );
					FR_SearchStation( FR_SEARCH_UP, FR_SSL_MID );		// scan down
				}
				if ( tFrConfig.m_cMute == FR_MUTE_OFF )
					FR_SetMute( FR_MUTE_OFF );							// mute off
				FR_DispFrequency();
				tFrConfig.cReceiveStereo = FR_DispStereo();					// disp stereo/mono
				FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
			}
			break;

		case TAVI_KEY_LAB:
			FR_Record(NULL);
			break;

		case TAVI_KEY_JOG2_LENTER:
			if ( !g_MiscCtrl->FolderOpen() ) break;

			if( tFrConfig.cTuneMode == FR_TUNEMODE_PRESET)
			{
				tFrConfig.cTuneMode = FR_TUNEMODE_MANUAL;
				pPopUpMain1->ChangeSubMenu(ML_PRESET, pPopUpMain_Preset, 1, 0, 0);
				SetTaviKeyMode(TK_LONG);
			}
			else
			{
				tFrConfig.cTuneMode = FR_TUNEMODE_PRESET;
				pPopUpMain1->ChangeSubMenu(ML_PRESET, pPopUpMain_Preset, 0, 0, 0);
				SetTaviKeyMode(TK_REPEAT);
			}
			FR_DispFrequency();
			FR_DispTuneMode();
			break;

		case TAVI_KEY_JOG2_ENTER:			
		case TAVI_KEY_ENTER:
			#if 0
			if ( tFrConfig.m_cMute )
			{
				tFrConfig.m_cMute = FR_MUTE_OFF;		// set play
				pFrListBox->SetOnState( 1 );			// play icon on List
				FR_SetMute( FR_MUTE_OFF );
				g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_PLAY );
				g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_PLAY );	
			}
			else
			{
				tFrConfig.m_cMute = FR_MUTE_ON;		// set pause
				pFrListBox->SetOnState( 2 );			// pause icon on List
				FR_SetMute( FR_MUTE_ON );
				g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_PAUSE );
				g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_PAUSE);	
			}
			g_TopPanel->Update();
			if ( !g_MiscCtrl->FolderOpen() )
				g_ExtLCD->Update();

			break;
			#else
			FR_PlayPause();
			#endif
	}
	return 0;
}

int FR_MsgProcForButtonUpInUiVolume(int usParam1)
{
	switch( usParam1 )
	{
	
		case TAVI_KEY_LEXIT:
			tFrConfig.m_ExitState = TaskExit_ExitAndSwitch;
			tFrConfig.m_fExit = true;
			return -1;
			break;

		case TAVI_KEY_LMODE:
			DEBUGMSG(1,("[1]FR_MsgProcForButtonUpInUiNormal() : TAVI_LEXIT\n" ));			
			tFrConfig.m_ExitState = TaskExit_Switch;
			tFrConfig.m_fExit = true;
			return -1;
			break;
			
		case TAVI_KEY_MODE:
			DEBUGMSG(1,("[1]FR_MsgProcForButtonUpInUiNormal() : TAVI_LEXIT\n" ));
			tFrConfig.m_ExitState = TaskExit_Background;
			tFrConfig.m_fExit = true;		
			return -1;
			break;

		case TAVI_KEY_JOG1_ENTER:	
		case TAVI_KEY_EXIT:
			DEBUGMSG(0,("[1]FR_MsgProcForButtonUpInUiVolume() : TAVI_KEY_EXIT\n" ));
			tFrConfig.cUiState = FR_UI_NORMAL;
			if( tFrConfig.cTuneMode == FR_TUNEMODE_PRESET)
			{
				SetTaviKeyMode(TK_REPEAT);
			}
			else // FR_TUNEMODE_MANUAL
			{
				SetTaviKeyMode(TK_LONG);
			}
			FR_DrawFrame( BOTTOMFRAME |FREQUENCYBAR );
			FR_DispTuneMode();
			tFrConfig.cReceiveStereo = FR_DispStereo();
			FR_DispRegion();
			FR_DispFrequencyBar( tFrConfig.nCurrentFrequency );
			FR_StopTimer(nIdTimer4Menu);
			break;

		case TAVI_KEY_MENU:
			DEBUGMSG(0,("[1]FR_MsgProcForButtonUpInUiVolume() : TAVI_KEY_MENU\n" ));
			tFrConfig.cUiState = FR_UI_NORMAL;
			if( tFrConfig.cTuneMode == FR_TUNEMODE_PRESET)
			{
				SetTaviKeyMode(TK_REPEAT);
			}
			else // FR_TUNEMODE_MANUAL
			{
				SetTaviKeyMode(TK_LONG);
			}
			FR_DrawFrame( BOTTOMFRAME |FREQUENCYBAR );
			FR_DispTuneMode();
			tFrConfig.cReceiveStereo = FR_DispStereo();
			FR_DispRegion();
			FR_DispFrequencyBar( tFrConfig.nCurrentFrequency );
//			mep_set_velocity( MEP_VELOCITY_80 );			// change touch speed
			pPopUpMenu->Show();
			break;

		case TAVI_KEY_JOG2_DOWN:
		case TAVI_KEY_LEFT:
			DEBUGMSG(0,("[1]FR_MsgProcForButtonUpInUiVolume() :  TAVI_KEY_LEFT\n" ));
			FR_TunningUpDown( FR_TUNEDIRECTION_DOWN );	// tunning down
			FR_DispFrequency();
			tFrConfig.cUiState = FR_UI_NORMAL;
			if( tFrConfig.cTuneMode == FR_TUNEMODE_PRESET)
			{
				SetTaviKeyMode(TK_REPEAT);
			}
			else // FR_TUNEMODE_MANUAL
			{
				SetTaviKeyMode(TK_LONG);
			}
			FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR );
			FR_DispTuneMode();
			tFrConfig.cReceiveStereo = FR_DispStereo();
			FR_DispRegion();
			FR_DispFrequencyBar( tFrConfig.nCurrentFrequency );
			break;
			
		case TAVI_KEY_JOG2_UP:			
		case TAVI_KEY_RIGHT:
			DEBUGMSG(0,("[1]FR_MsgProcForButtonUpInUiVolume() :  TAVI_KEY_RIGHT\n" ));
			FR_TunningUpDown( FR_TUNEDIRECTION_UP );		// tunning down
			FR_DispFrequency();
			tFrConfig.cUiState = FR_UI_NORMAL;
			if( tFrConfig.cTuneMode == FR_TUNEMODE_PRESET)
			{
				SetTaviKeyMode(TK_REPEAT);
			}
			else // FR_TUNEMODE_MANUAL
			{
				SetTaviKeyMode(TK_LONG);
			}
			FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR );
			FR_DispTuneMode();
			tFrConfig.cReceiveStereo = FR_DispStereo();
			FR_DispRegion();
			FR_DispFrequencyBar( tFrConfig.nCurrentFrequency );
			break;

		case TAVI_KEY_JOG1_UP:
		case TAVI_KEY_UP:
			DEBUGMSG(0,("[1]FR_MsgProcForButtonUpInUiVolume() :  TAVI_KEY_UP\n" ));
			FR_StartTimer(3000, nIdTimer4Menu);	
			g_SndCtrl->VolumeUp();
			FR_DispVolumeBar( g_SndCtrl->GetVolume() );
			break;

		case TAVI_KEY_JOG1_DOWN:
		case TAVI_KEY_DOWN:
			DEBUGMSG(0,("[1]FR_MsgProcForButtonUpInUiVolume() :  TAVI_KEY_DOWN\n" ));
			FR_StartTimer(3000, nIdTimer4Menu);	
			g_SndCtrl->VolumeDown();
			FR_DispVolumeBar( g_SndCtrl->GetVolume() );
			break;

		case TAVI_KEY_JOG2_LENTER:
			if ( g_MiscCtrl->FolderOpen() ) break;

			if( tFrConfig.cTuneMode == FR_TUNEMODE_PRESET)
			{
				tFrConfig.cTuneMode = FR_TUNEMODE_MANUAL;
				pPopUpMain1->ChangeSubMenu(ML_PRESET, pPopUpMain_Preset, 1, 0, 0);
				SetTaviKeyMode(TK_LONG);
			}
			else
			{
				tFrConfig.cTuneMode = FR_TUNEMODE_PRESET;
				pPopUpMain1->ChangeSubMenu(ML_PRESET, pPopUpMain_Preset, 0, 0, 0);
				SetTaviKeyMode(TK_REPEAT);
			}
			FR_DispFrequency();
			break;

		case TAVI_KEY_ENTER:
		case TAVI_KEY_JOG2_ENTER:
			if ( tFrConfig.m_cMute )
			{
				tFrConfig.m_cMute = FR_MUTE_OFF;		// set play
				pFrListBox->SetOnState( 1 );			// play icon on List
				FR_SetMute( FR_MUTE_OFF );
				g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_PLAY );
				g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_PLAY );
			}
			else
			{
				tFrConfig.m_cMute = FR_MUTE_ON;		// set pause
				pFrListBox->SetOnState( 2 );			// pause icon on List				
				FR_SetMute( FR_MUTE_ON );
				g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_PAUSE );
				g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_PAUSE );		
			}
			g_TopPanel->Update();
			if ( !g_MiscCtrl->FolderOpen() )
				g_ExtLCD->Update();

			break;
			
	}
	return 0;
}

static int OnKeyEvent( const KeyEvent& evt )
{
	switch( evt.value ) {
	case TAVI_KEY_SHUTDOWN:
	case VK_EXT_ESC:
    case TAVI_KEY_LEXIT:
		tFrConfig.m_ExitState = TaskExit_Normal;
		tFrConfig.m_fExit = true;
		return -1;
	}
	
	CEvent event;
	event.type = EVENT_KEY;
	event.u.key = evt;
	if( g_pFocusedWnd ) g_pFocusedWnd->SendEvent( event );
	return 0;
}

static int OnMouseEvent( const MouseEvent& evt )
{
	CEvent event;
	event.type = EVENT_MOUSE;
	event.u.mouse = evt;
	if( g_pFocusedWnd ) g_pFocusedWnd->SendEvent( event );

	return 0;
}

static int OnWindowEvent( const WindowEvent& evt )
{
	CEvent event;
	event.type = EVENT_WINDOW;
	event.u.window = evt;

	CWindow* pWnd;
	CNode< CWindow >* pNode;
	
	tFrConfig.m_WndList.Rewind();
	pWnd = tFrConfig.m_WndList.Head();
	while( pWnd ) {
		if( pWnd->GetWindowID() == evt.nWindowID ) break;
		pWnd = tFrConfig.m_WndList.Next();
	}

	if( pWnd ) pWnd->SendEvent( event );
	else {
		printf( "pWnd is NULL\n" );
		if( g_pFocusedWnd ) g_pFocusedWnd->SendEvent( event );
	}

	switch( evt.nMessage ) {
	case WINDOW_KILL:
		delete pWnd;
		g_pFocusedWnd = tFrConfig.m_WndList.Head();
		if( g_pFocusedWnd ) g_pFocusedWnd->Show();
		else {
			tFrConfig.cUiState = FR_UI_NORMAL;
			FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR | FRBIGICON);
			FR_DispFrequency();
			FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
			FR_DispTuneMode();
			FR_DispRegion();
			FR_DispStereo();
			//pMpegDecoder->RefreshOsd ();
			g_TopPanel->SetLogo( CTopPanel::LOGO_ID_RADIO );
			g_TopPanel->SetTitle( ML_RADIO );
			g_TopPanel->Show();
		}
		break;
	case WINDOW_KILL_FOCUS:
		pNode = tFrConfig.m_WndList.GetNode( 0 );
		if( pNode->m_Data == g_pFocusedWnd ) {
			CNode< CWindow >* pNext = pNode->m_pNext;
			tFrConfig.m_WndList.Swap( pNode, pNext );
		}
		else {
			tFrConfig.m_WndList.Head()->SetFocus();
		}
		break;
	}

	return 0;
}

static int FR_RecMsgProc( TMsgEntry* msg )
{
	CEvent event;
	MouseEvent mouse;
	KeyEvent key;
	WindowEvent window;
	int ret = 0;

	switch( msg->ulMsgId )
	{
	case MSG_BUTTON_UP:
		key.value = MapToVirtualKey( msg->usParam1 );
		key.pressed = msg->usParam2;
		ret = OnKeyEvent( key );
		break;
	case MSG_TOUCHPAD:
		mouse.x = msg->usParam1;
		mouse.click = msg->usParam2;
		ret = OnMouseEvent( mouse );
		break;
	case MSG_WINDOW:
		window.nWindowID = msg->usParam1;
		window.nMessage = msg->usParam2&0xffff0000;
		window.nParam = msg->usParam2&0xffff;
		ret = OnWindowEvent( window );
		break;
	default:
		if( CWindow::GetFocusedWindow() ) {
			window.nWindowID = CWindow::GetFocusedWindow()->GetWindowID();
			window.nMessage = WINDOW_IDLE;
			ret = OnWindowEvent( window );
		}
		break;
	}

	return ret;
}

int FR_MsgProc( int ssTaskId, U32 ulMsgId, int usParam1, int usParam2 )
{
	int	nIndex;
	unsigned int	nPercent;
	static int nSavedKey;
	int	nReturn;
	
	DEBUGMSG(1,("[2]FR_MsgProc() : ssTaskId[0x%x], ulMsgId [0x%x], usParam1[0x%x], usParam2[%x]\n"
		, ssTaskId, ulMsgId, usParam1, usParam2 ));

	nReturn = 0;
	
	if ( nOnConfirm)
	{
		nReturn = pMsgBox->MsgBtnUp(ssTaskId, ulMsgId, usParam1, usParam2);
		pMpegDecoder->RefreshOsd ();					// refresh screen							
		return nReturn;
	}

	switch( ulMsgId )
	{
		/////////////////////////////////////////////////////////////////////
		case MSG_BATTERY:
			break;
			
		/////////////////////////////////////////////////////////////////////
		case MSG_CREATE:
			break;
			
		/////////////////////////////////////////////////////////////////////
		case MSG_DRAW:
			break;
			
		/////////////////////////////////////////////////////////////////////
		case MSG_TIMER:	
			if ( usParam2 == nIdTimer4Menu )
			{
				DEBUGMSG(1,("[1]FR_MsgProc : TimeOutn IdTimer4Menu[%d]\n", nIdTimer4Menu) );
				nIdTimer4Menu = -1;

				switch (  tFrConfig.cUiState )
				{
					case FR_UI_PRESETLIST:
						break;
						
					case FR_UI_NORMAL:
						if( tFrConfig.cTuneMode == FR_TUNEMODE_PRESET)
						{
							SetTaviKeyMode(TK_REPEAT);
						}
						else // FR_TUNEMODE_MANUAL
						{
							SetTaviKeyMode(TK_LONG);
						}
						break;
						
					case FR_UI_VOLUME:
						tFrConfig.cUiState = FR_UI_NORMAL;
						FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR );
						FR_DispTuneMode();
						tFrConfig.cReceiveStereo = FR_DispStereo();
						FR_DispRegion();
						FR_DispFrequencyBar( tFrConfig.nCurrentFrequency );
						FR_DispPreset2extlcd(pFrListBox->GetCurrentChoice());
						break;
				}
			}
			else		// nIdTimer4BackOn
			{
				DEBUGMSG(1,("[1]FR_MsgProc : TimeOut nIdTimer4BackOn[%d]\n", nIdTimer4BackOn) );
				nIdTimer4BackOn = -1;
				if ( tFrConfig.cUiState == FR_UI_PRESETLIST )
				{
					if ( pPopUpMenu->IsVisible() ) {
						FR_StartTimer(10000, nIdTimer4BackOn);
						}
					else {
						tFrConfig.cUiState = FR_UI_NORMAL;
						SetTaviKeyMode(TK_LONG);
						pMpegDecoder->RenderText_WithoutRefresh(0,0,0,22,320,240-2);	// Clear OSD	
						pPopUpMenu=pPopUpMain1;
						FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR );
						FR_DispFrequency();
						FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
						FR_DispTuneMode();
						FR_DispRegion();
						}
				}					
			}
			break;
			
		/////////////////////////////////////////////////////////////////////	
		case MSG_TOUCHPAD:
			if ( pPopUpMenu->IsVisible() )
				pPopUpMenu->MsgProc( ssTaskId, ulMsgId, usParam1, usParam2);
			else
			{							
				switch (  tFrConfig.cUiState )
				{
					case FR_UI_PRESETLIST:
						if ( usParam2 > 0 )		// click
						{
							FR_MsgProcForPresetList	(TAVI_KEY_ENTER);
						}
						else						// slide
						{
//							mep_set_velocity( MEP_VELOCITY_80 );					// change touch speed
							if ( usParam1 > 0 )	//	move down
							{
								FR_MsgProcForPresetList	( TAVI_KEY_DOWN);
							}
							if ( usParam1 < 0 )	// move up
							{
								FR_MsgProcForPresetList	(TAVI_KEY_UP);						
							}
						}
						break;

					case FR_UI_NORMAL:
						if ( usParam2 > 0 )		// click
						{
							tFrConfig.cUiState = FR_UI_VOLUME;
							FR_DrawFrame( BOTTOMFRAME );
							FR_DispVolumeBar( g_SndCtrl->GetVolume() );
							FR_StartTimer(3000, nIdTimer4Menu);
						}
						else						// slide
						{						
/*							if ( tFrConfig.cTuneMode == FR_TUNEMODE_MANUAL)
								mep_set_velocity( MEP_VELOCITY_40 );			// change touch speed
							if ( tFrConfig.cTuneMode == FR_TUNEMODE_PRESET)
								mep_set_velocity( MEP_VELOCITY_80 );			// change touch speed
*/
							if ( usParam1 > 0 )
							{
								FR_MsgProcForButtonUpInUiNormal(TAVI_KEY_RIGHT);
							}
							if ( usParam1 < 0 )
							{
								FR_MsgProcForButtonUpInUiNormal(TAVI_KEY_LEFT);
							}
						}
						break;
					case FR_UI_VOLUME:
						if ( usParam2 > 0 )		// click
						{
							tFrConfig.cUiState = FR_UI_NORMAL;
							FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR );
							FR_DispTuneMode();
							tFrConfig.cReceiveStereo = FR_DispStereo();
							FR_DispRegion();
							FR_DispFrequencyBar( tFrConfig.nCurrentFrequency );
						}
						else						// slide
						{						
//							mep_set_velocity( MEP_VELOCITY_40 );				// change touch speed
							if ( usParam1 > 0 )
							{
								FR_MsgProcForButtonUpInUiVolume(TAVI_KEY_UP);
							}
							if ( usParam1 < 0 )
							{
								FR_MsgProcForButtonUpInUiVolume(TAVI_KEY_DOWN);
							}
						}
						break;
				}
			}
			break;
			
		/////////////////////////////////////////////////////////////////////			
		case MSG_BUTTON_UP:
			if (usParam1 == TAVI_KEY_SHUTDOWN)
			{
				DEBUGMSG(1,("[1]FR_MsgProc() : TAVI_KEY_SHUTDOWN\n" ));
				return -1;
			}
			if ( pPopUpMenu->IsVisible() )
			{
				pPopUpMenu->MsgProc( ssTaskId, ulMsgId, usParam1, usParam2);
			}
			else 
			{
				switch (  tFrConfig.cUiState )
				{
					case FR_UI_PRESETLIST:
						nReturn = FR_MsgProcForPresetList(  usParam1 );
						break;
						
					case FR_UI_NORMAL:
						nReturn = FR_MsgProcForButtonUpInUiNormal(usParam1);
						break;
						
					case FR_UI_VOLUME:
						nReturn = FR_MsgProcForButtonUpInUiVolume(usParam1);
						break;
				}
			}
			break;
		/////////////////////////////////////////////////////////////////////
		case MSG_FOLDER:
			if ( usParam1 ) 							// folder open
			{
				g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
				g_ExtLCD->Update();
			}
			else										// folder cloes
			{
				FR_DispPreset2extlcd(tFrConfig.nIndexOfPreset);
				if ( pPopUpMenu->IsVisible() )
					pPopUpMenu->Hide(0);
					
			}
			break;
			
		case MSG_NOTIFY:
			if ( usParam1 == NOTIFY_EARPHONE ) {
				pPopUpMain1->ChangeSubMenu(ML_SPEAKER, pPopUpMain_Speaker, usParam2, 0, 0);
				}
			break;
	}
	pMpegDecoder->RefreshOsd ();					// refresh screen							

	return nReturn;
}

int FR_Kill(void)
{
	/////////////////////////////////////////////////////////////
	// Exit FMRadio Task
	DEBUGMSG(1,("FMRadio_Task() -Exit FMRadio Task\n") );

    // Avoid pop-noise
    g_MiscCtrl->Mute( true );

	TAVI_CannotSleep( 550 );						// power save

	FR_Close();
	FR_DeInitPopUpMenu();

	if ( pFrTimer )
		delete pFrTimer;
	delete pMsgBox;


	g_TopPanel->SetPlayMode( CTopPanel::PLAYMODE_STOP );
	g_TopPanel->Update();

	// clear window list
	// DO NOT USE CList::FreeAll() !!
	CNode< CWindow >* pNode = tFrConfig.m_WndList.DetachHead();
	while( pNode ) {
		delete pNode;
		pNode = tFrConfig.m_WndList.DetachHead();
	}
    g_SndCtrl->LineInBypass( false );

    g_MiscCtrl->Mute( false );
	
	return 0;
}

int FMRadio_Task( TTaskInitParam* tParam )
{
	////////////////////////////////////////////////////////////////////////////
	// Init Video Player Task
	int 	nResult;
	TMsgEntry tMsg;
	FR_RECEIVEDAUDIO_t 	tSavedStereo,tCurrentStereo;
	const U32 setupitem0[4] = { ML_USA, ML_KOREA, ML_EU, ML_JAPAN };
	const U32 setupitem1[2] = { ML_STEREO, ML_MONO };

	DEBUGMSG(1,("[0]FMRadio_Task(): step1\n"));

//	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(TRUE);
	showbackground(ID_BG_RADIO);		// fmradio

	g_TopPanel->SetTitle( ML_RADIO );
	g_TopPanel->Show();

	op_ClearAreaOSD( 
		0, g_TopPanel->rcABS.GetHeight(), 
		320, 240-g_TopPanel->rcABS.GetHeight() );

	mep_report( MEP_REPORT_RELATIVE);
//	mep_set_velocity( MEP_VELOCITY_80 );					// change touch speed

	// Set current task id
	g_CurrentTaskID = tParam->ssTaskId;

	g_ExtLCD->ChangeMenu( CExtLCD::EXT_MENU_RADIO );
	g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_LIST );

	tFrConfig.m_fExit = false;
	tFrConfig.m_ExitState = TaskExit_Normal;

	if( tParam->resume ) {
		SetTaviKeyMode(TK_REPEAT);
		if ( tFrConfig.cUiState == FR_UI_PRESETLIST)
		{
			DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU) );
			pFrListBox->ReDraw();
			FR_DispPreset2extlcd( tFrConfig.nIndexOfPreset );
		}
		else
		{		
			FR_DrawFrame( BOTTOMFRAME | FREQUENCYBAR | FRBIGICON);
			FR_DispFrequency();
			FR_DispFrequencyBar(tFrConfig.nCurrentFrequency);
			FR_DispTuneMode();
			FR_DispRegion();
		}
		pMpegDecoder->RefreshOsd ();					// refresh screen
		goto __msg_proc;
	}

	TAVI_CanSleep( 500 );		// power save
	
	pFrListBox = new CListBox(ML_PRESET_LIST, 0, 0, 320, 220, NULL, 0);
	pMsgBox = new CMsgBox();
	
	nResult = FR_Initialize(tParam->ssTaskId);
	if ( nResult < 0 ) {
		tFrConfig.m_fExit = true;
		printf("Initialize failure\n");
		}
	FR_InitPopUpMenu();

	
__msg_proc:
	int msgflags = MSG_PEEK_ALL;
	while( tFrConfig.m_fExit == false ) 
	{
		////////////////////////////////////////////////////////////////////////////
		// Video Player Task Message Peeking
		if( tFrConfig.cUiState == FR_UI_RECORD ) {
			memset( &tMsg, 0, sizeof(tMsg) );
			MSG_Peek( tParam->ssTaskId, &tMsg );
			if( FR_RecMsgProc( &tMsg ) == -1 ) break;
			continue;
		}
		
		if( MSG_Peek( tParam->ssTaskId, &tMsg ) == 0)
		{
			DEBUGMSG(0,("[0]FMRadio_Task: Before FR_MsgProc()\n"));
			
			if( FR_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) == -1)
			{
				DEBUGMSG(1,("[0]FMRadio_Task: It means End of FMRadio Task\n "));
				// break means End of FMRadio Task
				break;
			}
		}
		else
		{
			if ( tFrConfig.cUiState == FR_UI_NORMAL )
			{
				tCurrentStereo = FR_DispStereo();
				if ( tFrConfig.cReceiveStereo != tCurrentStereo )		// if changed 
				{
					pMpegDecoder->RefreshOsd();
					tFrConfig.cReceiveStereo = tCurrentStereo;
				}

				if (	nNoHeadPhone  )
				{
					if ( g_MiscCtrl->HeadphoneIN() )
					{
						nNoHeadPhone = 0;
						nOnConfirm = 0;
						FR_DispFrequency();
						pMpegDecoder->RefreshOsd ();			// refresh screen							
						
					}
				}
				else
				{
					if (g_MiscCtrl->HeadphoneIN() ==FALSE )
					{
						nNoHeadPhone = 1;
						FR_DispFrequency();
						pMpegDecoder->RefreshOsd ();			// refresh screen							

					}
				}
			}
			else if ( tFrConfig.cUiState == FR_UI_STARTUP && nNoHeadPhone )
			{
				if ( g_MiscCtrl->HeadphoneIN() )
				{
					pMsgBox->Reset( ML_INFORMATION,
						_ORANGE,
						ML_AUTO_SCAN_QUERY,
						244, 123,
						FR_InitScanConfirmMsgProc,
						DL_BG_BLANK |DL_BTN_OK_CANCEL
						);
					pMsgBox->Show();
					nNoHeadPhone = 0;
					nOnConfirm = 2;
				}
			}
			usleep(1);
		}

	}

	// End
	//ClearOsd();
	if ( tFrConfig.m_cSpeaker == 0 ) {
		tFrConfig.m_cSpeaker = 1;		// speaker off
		FR_SetSpeaker ( (RMuint8 *)&tFrConfig.m_cSpeaker );
		}
	if( tFrConfig.m_ExitState == TaskExit_Normal || tFrConfig.m_ExitState == TaskExit_ExitAndSwitch ) 
	{
		FR_Kill();
	}

	printf( "FM EXIT: %d\n", tFrConfig.m_ExitState );

	return tFrConfig.m_ExitState;
	
}


 
