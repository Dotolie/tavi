//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : Tavi Task Process
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Task.cpp,v $
// $Revision: 1.14 $
// $Date: 2006/06/22 01:38:04 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: Task.cpp,v $
// Revision 1.14  2006/06/22 01:38:04  zstein
// - removed unused codes.
//
// Revision 1.13  2006/06/19 04:57:24  ywkim
// Change routine of TAVI_InitSleepReg(), TAVI_ResetSleepTimer(), TAVI_SleepDown() and TAVI_WakeUp()
//
// Revision 1.12  2006/06/12 05:56:37  zstein
// - removed duplicated codes.
//
// Revision 1.11  2006/06/12 05:35:46  zstein
// - fixed alarm bug.
//
// Revision 1.10  2006/06/12 04:36:19  ywkim
// Change external variable to local one for handle_rtc
//
// Revision 1.9  2006/05/24 03:20:57  zstein
// - fixed RTC bug.
//
// Revision 1.8  2006/04/15 01:33:50  zstein
// - removed unused codes.
//
// Revision 1.7  2006/03/17 07:14:10  zstein
// - removed a pop-noise.
//
// Revision 1.6  2006/03/15 03:08:22  ywkim
// Exchange order of function SysLoadParameters & op_InitFramebuffer
//
// Revision 1.5  2006/03/13 02:32:23  zstein
// - fixed volume control bug
//
// Revision 1.4  2006/03/09 02:04:27  zstein
// - improved volume interface.
//
// Revision 1.3  2006/03/08 12:25:47  zstein
// - handled alarm error
//
// Revision 1.2  2006/03/06 06:03:32  zstein
// - Mute volume when power down.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*****************************************************************************

#include <tavi_lib.h>
#include <jpeg/jpeg.h>
//#include <Lib/LibDefines.h>
#include <Lib/LibImage.h>
#include <Lib/LibFileBrowser.h>
#include <Lib/LibText.h>
#include <Lib/LibBuzzer.h>
#include <Lib/LibDefines.h>
#include <meplib.h>

#include "TextViewer/TextViewer.h"
#include "AudioPlayer/AudioCmd.h"
#include "AudioPlayer/AudioPlayer.h"
#include "VideoPlayer/VideoPlayer.h"
#include "ImageViewer/ImageViewer.h"
#include "FMRadio/FMRadio.h"
#include "FileManager/FileManager.h"
#include "setup/setup.h"
#include "Recorder/RecorderMain.h"
#include "ImageViewer/ImageViewer.h"
#include "Extras/Extra.h"
#include "Extras/AlarmSet.h"
#include "SubLCD.h"
#include "fatfs.h"
#include "../lcd/lcd.h"
#include "../Component/FileUtil.h"

#include "Task.h"
#include "ExtLCD.h"
#include <util.h>

#include "../md5/md5.h"

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
#define HOLD_X			(BATTERY_X-16)
#define HOLD_Y			5


#define _SCREEN_WIDTH_					320
#define _SCREEN_HEIGHT_					240

#define _SELECTED_MENU_PART_A_WIDTH_	160	//144
#define _SELECTED_MENU_PART_A_HEIGHT_	56
#define _SELECTED_MENU_PART_B_WIDTH_	132	//148
#define _SELECTED_MENU_PART_B_HEIGHT_	56
#define _NON_SELECTED_MENU_WIDTH_		156	//128
#define _NON_SELECTED_MENU_HEIGHT_		40

#define _MENU_START_X_POINT_		(_SCREEN_WIDTH_/2) - ((_SELECTED_MENU_PART_A_WIDTH_+_SELECTED_MENU_PART_B_WIDTH_)/2)
#define _MENU_START_Y_POINT_		((_SCREEN_HEIGHT_/2)-(_SELECTED_MENU_PART_A_HEIGHT_/2))-(_NON_SELECTED_MENU_HEIGHT_*2) -2

#define _MENUV2_WIDTH					320
#define _MENUV2_HEIGHT					151
#define _MENUV2_START_X 				0
#define _MENUV2_START_Y				    40

extern void showbackground (char *bkgnd);
extern void showbackground (int menuID);

static void ShowBG(int menuID);
static void ShowBG_ByFile(char *bgFilename);

extern int _MSG_Show_Alarm( int ssTaskId,  TMsgEntry* tMsg );


TTAVI_GLOBAL_CONFIG*		pTAVI;
//TTAVI_GLOBAL_CONFIG*		pTAVI = new TTAVI_GLOBAL_CONFIG;

CFileBrowser*				pLFBrowser;
bool 						g_KillAll;

extern BITMAPFILEINFO256	TAVI_IMAGES[];
extern CTopPanel*			g_TopPanel;

HDDsleepReg g_HddSleep;		// power save
static bool					s_EL_status_last;

TTaskEntryBlock TTaskList[MAX_TASK]=
{
	// defined in Task.h
	// {taskId, switchable, state, msgMask, initFunc, msgFunc, exitFunc, SleepingKey}
	{TASK_ID_MAIN, FALSE, TASK_STOP, MSG_ALL, TAVI_MainTask, TAVI_MsgProc, 0, 0},
	{TASK_ID_MUSIC, TRUE, TASK_STOP, MSG_ALL, AudioPlayer_Task, 0, AP_Kill, 0 },
	{TASK_ID_VIDEO, FALSE, TASK_STOP, MSG_ALL, VideoPlayer_Task, VP_MsgProc, 0, 0},
	{TASK_ID_RADIO, TRUE, TASK_STOP, MSG_ALL, FMRadio_Task, FR_MsgProc, FR_Kill, 0},
	{TASK_ID_IMAGE, TRUE, TASK_STOP, MSG_ALL, ImageViewer_Task, 0, IV_Kill, 0 },
	{TASK_ID_TEXT, TRUE, TASK_STOP, MSG_ALL, TextViewer_Task, 0, 0, 0},
	{TASK_ID_RECORD, FALSE, TASK_STOP, MSG_ALL, REC_Task, 0, 0, 0},  // Recorder
	{TASK_ID_ETC, TRUE, TASK_STOP, MSG_ALL, Extra_Task, Extra_MsgProc, Extra_Kill, 0},	// Extra
	{TASK_ID_FILEMANAGER, FALSE, TASK_STOP, MSG_ALL, FileManager_Task, FM_MsgProc, 0, 0 },
	{TASK_ID_SETUP, TRUE, TASK_STOP, MSG_ALL, Setup_Task, Setup_MsgProc, Setup_Kill, 0 }
};

static CList< TTaskEntryBlock > s_TaskList;

FATFS_ERROR _ReadTransparencyFile( RMuint8* TransparencyDataBlock, char* TrasparencyDataFile )
{
	RMuint32 fd, cnt=0, rc;
	FATFS_ERROR err;
	
	DEBUGMSG(1,("_ReadTransparencyFile()\n"));
	DEBUGMSG(1,("-Address of TransparencyDataBlock : 0x%x\n", TransparencyDataBlock ));
	DEBUGMSG(1,("-TrasparencyDataFile : [%s]\n", TrasparencyDataFile ));

	if( (err = fatfs_fopen( (RMuint8*)TrasparencyDataFile, _O_RDONLY, &fd )) == 0 )
		{
		err = fatfs_fread( fd, TransparencyDataBlock, MAX_PALETTE_INDEX, &rc );
		DEBUGMSG(1,("-%ld Bytes Read\n", rc ));
		ASSERT(!err);
		fatfs_fclose( fd );
		}

	DEBUGMSG(1,("------------------------------------------------\n"));
	for(unsigned int idx=0; idx < 256 ; idx++)
	{
		DEBUGMSG(1,("%02x ", TransparencyDataBlock[idx] ));
		if( ((idx+1)%16)==0 && idx!=0) DEBUGMSG(1,("\n"));
	} 
	DEBUGMSG(1,("\n------------------------------------------------\n"));

	DEBUGMSG(1,("-err=0x%xh\n\n", err )); usleep(10);
	return err;
}

void _SetTransparency(int innerPaletteTransparencyType, RMuint8* PaletteDataBlock, RMuint8* TransparencyDataBlock )
{
	RMuint8 r,g,b;

	DEBUGMSG(1,("_SetTransparency()\n"));
	DEBUGMSG(1,("-Address of TransparencyDataBlock : 0x%x\n", TransparencyDataBlock ));
	DEBUGMSG(1,("------------------------------------------------\n"));

	for(unsigned int idx=0; idx < 256 ; idx++)
	{
		r = TAVI_IMAGES[innerPaletteTransparencyType].palette[idx].R;
		g = TAVI_IMAGES[innerPaletteTransparencyType].palette[idx].G;
		b = TAVI_IMAGES[innerPaletteTransparencyType].palette[idx].B; 

		DEBUGMSG(1,("%02x ", TransparencyDataBlock[idx] ));
		if( ((idx+1)%16)==0 && idx!=0) DEBUGMSG(1,("\n"));

		PaletteDataBlock[(idx<<2)+0] = TransparencyDataBlock[idx];
		PaletteDataBlock[(idx<<2)+1] = rgb_Y( r, g, b ); 
		PaletteDataBlock[(idx<<2)+2] = rgb_U( r, g, b ); 
		PaletteDataBlock[(idx<<2)+3] = rgb_V( r, g, b );
	} 
	DEBUGMSG(1,("\n------------------------------------------------\n\n"));


}

int TAVI_LoadPaletteTransparencyData( int IsSubPaletteTransparencyType )
{
	RMuint8* PaletteDataBlock = NULL; 

	DEBUGMSG(1,("\nTAVI_LoadPaletteTransparencyData()\n")); usleep(10);

	if( IsSubPaletteTransparencyType == MAIN_PALETTE )
		{
		DEBUGMSG(1,("-PaletteTransparencyType == MAIN_MENU_BATTERY\n")); usleep(10);
	
		if(pTAVI->MainPalette == NULL)
			{
			DEBUGMSG(1,("-Palette Allocation ... \n")); usleep(10);
			pTAVI->MainPalette = (RMuint8*) malloc(4*MAX_PALETTE_INDEX);
			pTAVI->MainPalette_Transparent = (RMuint8*) malloc(MAX_PALETTE_INDEX);
			_ReadTransparencyFile( pTAVI->MainPalette_Transparent, _MAIN_TRANSPARENCY_FILE );
			_SetTransparency( MAIN_MENU_BATTERY, pTAVI->MainPalette, pTAVI->MainPalette_Transparent );
			DEBUGMSG(1,(" \nDone\n")); usleep(10);
			}

		PaletteDataBlock = pTAVI->MainPalette;
		}
	else if (IsSubPaletteTransparencyType == VIDEO_LOADING_PROGRESS_PALETTE )
		{
		DEBUGMSG(1,("-PaletteTransparencyType == PROGRESS_PALETTE\n")); usleep(10);

		if(pTAVI->VideoLoadingPalette == NULL)
			{
			DEBUGMSG(1,("-Palette Allocation ... ")); usleep(10);
			pTAVI->VideoLoadingPalette = (RMuint8*) malloc(4*MAX_PALETTE_INDEX);
			pTAVI->VideoLoadingPalette_Transparent = (RMuint8*) malloc(MAX_PALETTE_INDEX);	
			memset( pTAVI->VideoLoadingPalette_Transparent, 0xFF, MAX_PALETTE_INDEX );
			_SetTransparency( LOADING_ANIMATION, pTAVI->VideoLoadingPalette, pTAVI->VideoLoadingPalette_Transparent );	
			DEBUGMSG(1,(" Done\n")); usleep(10);
			}

		PaletteDataBlock = pTAVI->VideoLoadingPalette;
		}
	else
		{
		DEBUGMSG(1,("-PaletteTransparencyType == SUB_MENU_BATTERY\n")); usleep(10);
		if(!(pTAVI->SubPalette))
			{
			DEBUGMSG(1,("-Palette Allocation ... ")); usleep(10);
			pTAVI->SubPalette = (RMuint8*) malloc(4*MAX_PALETTE_INDEX);
			pTAVI->SubPalette_Transparent = (RMuint8*) malloc(MAX_PALETTE_INDEX);
			_ReadTransparencyFile( pTAVI->SubPalette_Transparent, _SUB_TRANSPARENCY_FILE );
			_SetTransparency( ICON_TOP_BATTERY, pTAVI->SubPalette, pTAVI->SubPalette_Transparent );	
			DEBUGMSG(1,(" Done\n")); usleep(10);
			}
		PaletteDataBlock = pTAVI->SubPalette;
		}			

	pMpegDecoder->RenderOsdPalette( PaletteDataBlock );

	return TRUE;
}


int TAVI_LoadBitmaps()
{

	printf("TAVI_LoadBitmaps() : Bitmap Loading ");

	///////////////////////////////////////////////////////////////////////////////////////////
 	// Shared Icon Images
	AllocBitmap256( _IMAGE_ICON"/battery-sub.bmp", &TAVI_IMAGES[ICON_TOP_BATTERY] );
	AllocBitmap256( _IMAGE_ICON"/StartupProgressItem.bmp", &TAVI_IMAGES[STARTUP_PROGRESS] );	
	AllocBitmap256( _IMAGE_ICON"/list_icons.bmp", &TAVI_IMAGES[TYPE_ICON_LIST]);
	AllocBitmap256( _IMAGE_ICON"/repeat_icons.bmp", &TAVI_IMAGES[ICON_REPEAT] );
	AllocBitmap256( _IMAGE_ICON"/eq.bmp", &TAVI_IMAGES[ICON_EQ] );
	AllocBitmap256( _IMAGE_ICON"/eq_arrow.bmp", &TAVI_IMAGES[ICON_EQ_ARROW] );
	AllocBitmap256( _IMAGE_ICON"/arrow.bmp", &TAVI_IMAGES[ICON_SMALL_LEFT_RIGHT_ARROW] );
	AllocBitmap256( _IMAGE_ICON"/keyindex.bmp", &TAVI_IMAGES[ICON_DBXICON] );
	AllocBitmap256( _IMAGE_ICON"/keyindex_black.bmp", &TAVI_IMAGES[ICON_DBXICON_BLACK] );
	AllocBitmap256( _IMAGE_ICON"/keyindex_gray.bmp", &TAVI_IMAGES[ICON_DBXICON_GRAY] );
	AllocBitmap256( _IMAGE_ICON"/warning.bmp", &TAVI_IMAGES[ICON_DITTO] );
	AllocBitmap256( _IMAGE_ICON"/warning_small.bmp", &TAVI_IMAGES[ICON_DITTO_SMALL] );
	AllocBitmap256( _IMAGE_ICON"/host_connected.bmp", &TAVI_IMAGES[BG_HOST_CONNECT] );
	AllocBitmap256( _IMAGE_ICON"/ethernet_connected.bmp", &TAVI_IMAGES[BG_ETHERNET_CONNECT] );
	AllocBitmap256( _IMAGE_ICON"/hold_icon.bmp", &TAVI_IMAGES[ICON_HOLD] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// Control Images
	//	- Dialog Box Control
	AllocBitmap256( _IMAGE_CONTROL"/dialog_box/dialogbox_edge.bmp", &TAVI_IMAGES[DIALOGBOX_EDGE] );
	AllocBitmap256( _IMAGE_CONTROL"/dialog_box/selectbar_edge.bmp", &TAVI_IMAGES[SELECTBAR_EDGE] );
	AllocBitmap256( _IMAGE_CONTROL"/dialog_box/scroll.bmp", &TAVI_IMAGES[SCROLLBAR_EDGE] );			// 8 * 32
	AllocBitmap256( _IMAGE_CONTROL"/pannel/edge_warning.bmp", &TAVI_IMAGES[DIALOGBOX_EDGE2] );		// 4 * 4 * 8
	AllocBitmap256( _IMAGE_CONTROL"/pannel/warning_selection.jpg.BMP", &TAVI_IMAGES[DIALOGBOX_BUTTON_EDGE] );		// 4 * 4 * 8
	//	- Control Pannel Control
	AllocBitmap256( _IMAGE_CONTROL"/pannel/edge_i24_i25_i26.bmp", &TAVI_IMAGES[CONTROLPANNEL_EDGE] );
	AllocBitmap256( _IMAGE_CONTROL"/progress/edge_i17_i19_i24.bmp", &TAVI_IMAGES[PROGRESSBAR_EDGE] );
	AllocBitmap256( _IMAGE_CONTROL"/touchpad/touchcontrol_edge.bmp", &TAVI_IMAGES[TOUCHPAD_MENU_EDGE] );
	AllocBitmap256( _IMAGE_CONTROL"/touchpad/touchcontrol_edge2.bmp", &TAVI_IMAGES[TOUCHPAD_VOL_N_SEEK_EDGE] );
	AllocBitmap256( _IMAGE_CONTROL"/touchpad/touchball.bmp", &TAVI_IMAGES[TOUCHPAD_BALL] );
	AllocBitmap256( _IMAGE_CONTROL"/touchpad/touchvolume.bmp", &TAVI_IMAGES[TOUCHPAD_VOLUME] );
	AllocBitmap256( _IMAGE_CONTROL"/touchpad/touchseek.bmp", &TAVI_IMAGES[TOUCHPAD_SEEK] );
	AllocBitmap256( _IMAGE_CONTROL"/touchpad/touchcontrol.bmp", &TAVI_IMAGES[TOUCH_MENU] );
	AllocBitmap256( _IMAGE_CONTROL"/touchpad/icons.bmp", &TAVI_IMAGES[TOUCH_MINI_ICON] );
	//   - Sub Menu Control
	AllocBitmap256( _IMAGE_CONTROL"/sub_menu/submenu_arrow_icons.bmp", &TAVI_IMAGES[SUBMENU_ARROW_ICON] );
	AllocBitmap256( _IMAGE_CONTROL"/sub_menu/submenu_cursor.bmp", &TAVI_IMAGES[SUMMENU_CURSOR] );
	AllocBitmap256( _IMAGE_CONTROL"/sub_menu/submenu_edge_icons.bmp", &TAVI_IMAGES[SUBMENU_EDGE_ICON] );
	AllocBitmap256( _IMAGE_CONTROL"/sub_menu/submenu_cancel_arrow.bmp", &TAVI_IMAGES[SUBMENU_CANCEL_ARROW] );
	// 	- Loading Progress Control
	AllocBitmap256(_IMAGE_CONTROL"/progress/loading_movie.bmp", &TAVI_IMAGES[LOADING_ANIMATION] );
	// 	- Loading Virtual Keyboard Control
	AllocBitmap256(_IMAGE_CONTROL"/keyboard/image_keys.bmp", &TAVI_IMAGES[VIRTUAL_KEY] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// Video Player Only Images
	AllocBitmap256( _IMAGE_CONTROL"/video/title-video.bmp", &TAVI_IMAGES[ICON_VIDEOPLAYER] );
	AllocBitmap256( _IMAGE_CONTROL"/video/videomode.bmp", &TAVI_IMAGES[VIDEO_ICON_SCREENTYPE] );
	AllocBitmap256( _IMAGE_CONTROL"/video/music.video_status.bmp", &TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// FM Radio Only Images
	AllocBitmap256( _IMAGE_CONTROL"/radio/current.bmp", &TAVI_IMAGES[RADIO_FREQ_CURSOR] );
	AllocBitmap256( _IMAGE_CONTROL"/radio/mhz.bmp", &TAVI_IMAGES[RADIO_DIGIT_MHZ] );
	AllocBitmap256( _IMAGE_CONTROL"/radio/preset.bmp", &TAVI_IMAGES[RADIO_PRESET] );
	AllocBitmap256( _IMAGE_CONTROL"/radio/radio.bmp", &TAVI_IMAGES[RADIO_BIG_ICON] );
	AllocBitmap256( _IMAGE_CONTROL"/radio/title-radio.bmp", &TAVI_IMAGES[RADIO_SMALL_ICON] );
	AllocBitmap256( _IMAGE_CONTROL"/radio/searchprogress.bmp", &TAVI_IMAGES[RADIO_SEARCH_BAR] );	

	///////////////////////////////////////////////////////////////////////////////////////////
	// Audio Player Only Images
	AllocBitmap256( _IMAGE_CONTROL"/audio/list-selection.bmp", &TAVI_IMAGES[LIST_SELECTBAR] );	
	AllocBitmap256( _IMAGE_CONTROL"/audio/base-bottom.bmp", &TAVI_IMAGES[FRAME_BOTTOM] );
	AllocBitmap256( _IMAGE_CONTROL"/audio/title-music.bmp", &TAVI_IMAGES[TITLE_MUSIC] );
	AllocBitmap256( _IMAGE_CONTROL"/audio/playing.bmp", &TAVI_IMAGES[AUDIO_BIG_ICON] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// Photo Viewer Only Images
	AllocBitmap256( _IMAGE_CONTROL"/photo/thumbnailselection.bmp", &TAVI_IMAGES[THUMBNAIL_SELECTBAR] );
	AllocBitmap256( _IMAGE_CONTROL"/photo/title-photo.bmp", &TAVI_IMAGES[PHOTO_TITLE] );
	AllocBitmap256( _IMAGE_CONTROL"/photo/angle.bmp", &TAVI_IMAGES[ICON_ROTATE] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// File Manager Only Images
	AllocBitmap256( _IMAGE_CONTROL"/browse/title-browse.bmp", &TAVI_IMAGES[ICON_FILEMANAGER] );	
	AllocBitmap256( _IMAGE_CONTROL"/browse/selected.bmp", &TAVI_IMAGES[ICON_SELECTED] );
	AllocBitmap256( _IMAGE_CONTROL"/browse/copymove.bmp", &TAVI_IMAGES[ICON_COPYMOVE] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// Setup Only Images
	AllocBitmap256( _IMAGE_CONTROL"/setup/title-setup.bmp", &TAVI_IMAGES[TITLE_SETUP] );
	AllocBitmap256( _IMAGE_CONTROL"/setup/setuplist-icons.bmp", &TAVI_IMAGES[SETUP_LISTICONS] );
	AllocBitmap256( _IMAGE_CONTROL"/setup/setupslider.bmp", &TAVI_IMAGES[SETUP_SLIDER] );
	AllocBitmap256( _IMAGE_CONTROL"/setup/arrow.bmp", &TAVI_IMAGES[SETUP_ARROW] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// Extra Images
	AllocBitmap256( _IMAGE_CONTROL"/extra/title-extra.bmp", &TAVI_IMAGES[ICON_EXTRA] );
	AllocBitmap256( _IMAGE_CONTROL"/extra/icon_bell.bmp", &TAVI_IMAGES[ICON_BELL] );
	AllocBitmap256( _IMAGE_CONTROL"/extra/AlarmClock.bmp", &TAVI_IMAGES[IMAGE_CLOCK] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// TextViewer Images
	AllocBitmap256( _IMAGE_CONTROL"/text/title-text.bmp", &TAVI_IMAGES[ICON_TEXTVIEWER] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// Recorder Images
	AllocBitmap256( _IMAGE_CONTROL"/record/recording.bmp", &TAVI_IMAGES[RECORD_BIG_ICON] );
	AllocBitmap256( _IMAGE_CONTROL"/record/autogain.bmp", &TAVI_IMAGES[RECORD_AGC] );
	AllocBitmap256( _IMAGE_CONTROL"/record/bitrate.bmp", &TAVI_IMAGES[RECORD_BITRATE] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// Image Fonts
	AllocBitmap256( _IMAGE_FONT"/digit-3.bmp", &TAVI_IMAGES[FONT_DIGIT3] );
	AllocBitmap256( _IMAGE_FONT"/chno.bmp", &TAVI_IMAGES[LARGE_DIGIT] );
	AllocBitmap256( _IMAGE_FONT"/font_alarm.bmp", &TAVI_IMAGES[ALARM_DIGIT] );
	AllocBitmap256( _IMAGE_FONT"/font_eq.bmp", &TAVI_IMAGES[EQ_DIGIT] );
	AllocBitmap256( _IMAGE_FONT"/font_clock.bmp", &TAVI_IMAGES[CLOCK_DIGIT] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// Main Menu
	AllocBitmap256( _IMAGE_MAIN".rotate/m1.bmp", &TAVI_IMAGES[M1] );
	AllocBitmap256( _IMAGE_MAIN".rotate/m2.bmp", &TAVI_IMAGES[M2] );
	AllocBitmap256( _IMAGE_MAIN".rotate/m3.bmp", &TAVI_IMAGES[M3] );
	AllocBitmap256( _IMAGE_MAIN".rotate/m4.bmp", &TAVI_IMAGES[M4] );
	AllocBitmap256( _IMAGE_MAIN".rotate/m5.bmp", &TAVI_IMAGES[M5] );
	AllocBitmap256( _IMAGE_MAIN".rotate/m6.bmp", &TAVI_IMAGES[M6] );
	AllocBitmap256( _IMAGE_MAIN".rotate/m7.bmp", &TAVI_IMAGES[M7] );
	AllocBitmap256( _IMAGE_MAIN".rotate/m8.bmp", &TAVI_IMAGES[M8] );
	AllocBitmap256( _IMAGE_MAIN".rotate/m9.bmp", &TAVI_IMAGES[M9] );

	AllocBitmap256( _IMAGE_MAIN".rotate/d-1.bmp", &TAVI_IMAGES[MM1] );
	AllocBitmap256( _IMAGE_MAIN".rotate/d-2.bmp", &TAVI_IMAGES[MM2] );
	AllocBitmap256( _IMAGE_MAIN".rotate/d-3.bmp", &TAVI_IMAGES[MM3] );

	AllocBitmap256( _IMAGE_MAIN".rotate/nml.bmp", &TAVI_IMAGES[NML_LOGO] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// Sub Menu
	AllocBitmap256( _IMAGE_MAIN".rotate/bottom1-1.bmp", &TAVI_IMAGES[SUBMENU_BASIC] );
	AllocBitmap256( _IMAGE_MAIN".rotate/bottom2-1.bmp", &TAVI_IMAGES[SUBMENU_SELECTED] );

	///////////////////////////////////////////////////////////////////////////////////////////
	// For Test 
	
	//AllocBitmap256( _IMAGE_MAIN".rotate/m8a.bmp", &TAVI_IMAGES[DEMO__RECORD_MAIN_MENU8] );	

	printf("... Pass\n");
	
	return TRUE;

}

int TAVI_UnloadBitmaps()
{
	//for(int i=0;i<256;i++)
	//	FreeBitmap256(&TAVI_IMAGES[i]);

	return TRUE;
}


static int TAVI_DrawMovingTransparencyBackground2( RMuint8* pOsdBuf )
{
	int  sy, height, rate;

	rate = 1;

	for( sy = 213; sy > 116 ; sy-- )		
		{
			DrawBoxWithoutRefresh( BOTTOM_MENU_FILTER_COLOR_PALETTE, 0, sy, 320, 213-sy );
			pMpegDecoder->RefreshOsd();
			sy -= rate;
		}		

	return TRUE;
}

static int DigitToMACRO( int MenuSlotId )
{
	switch( MenuSlotId )
		{
		case MENU_SLOT1:	return M1;
		case MENU_SLOT2:	return M2;
		case MENU_SLOT3:	return M3;
		case MENU_SLOT4:	return M4;
		case MENU_SLOT5:	return M5;
		case MENU_SLOT6:	return M6;
		case MENU_SLOT7:	return M7;
		case MENU_SLOT8:	return M8;
		case MENU_SLOT9:	return M9;
		}

	ASSERT(0);
	return M1;
}



#define _SM_WIDTH			(320-(24*2))
#define _SM_ICON_COUNT		9
#define _SM_ICON_SX			( ( 320 / 2 ) - ( _SM_WIDTH / 2 ) )
#define _SM_BG_SY			176 //(_SM_ICON_SY - 5)
#define _SM_ICON_SY			(_SM_BG_SY+5)	//( 239 - 25 )
#define _SM_ICON_WIDTH		(_SM_WIDTH/_SM_ICON_COUNT)

static const int iSubMenuIconWidth[_SM_ICON_COUNT] ={17, 17, 17, 16, 18, 16, 17, 18, 15};

static void
TAVI_DrawSubMenu2( int menuset, int sy, int height )
{
	int ix 						= _SM_ICON_SX + ((_SM_ICON_WIDTH - iSubMenuIconWidth[8])/2 )+1;
	BITMAPFILEINFO256* pMenu 	= NULL;
	menuset 					= ( menuset / 10 ) - 1;

	DrawBoxWithoutRefresh(
		74,	// RMuint8 color
		0,							// RMuint16 sx
		sy+height, 					// RMuint16 sy
		320, 						//RMuint16 width
		32							// RMuint16 height)
		);

	
	for( int i=0; i<_SM_ICON_COUNT; i++ )
		{
			if( i == menuset )
				pMenu = &TAVI_IMAGES[ SUBMENU_SELECTED];
			else
				pMenu = &TAVI_IMAGES[ SUBMENU_BASIC ];
		
			DrawItemWithoutRefresh ( 
				//&TAVI_IMAGES[ SUBMENU_BASIC ],
				pMenu,
				i,
				ix+(i*_SM_ICON_WIDTH),
				sy+height+5,
				20,
				20
				);
		}

	pMpegDecoder->RefreshOsd();
	
}



static int TAVI_DrawMenuItem3( int menuset )
{
	int	input,test;
	int	first, last;
	int 	nValue;
	
	BITMAPFILEINFO256* pMenu1;
	BITMAPFILEINFO256* pMenu2;
	BITMAPFILEINFO256* pMenu3;
	BITMAPFILEINFO256* pMenu4;
	BITMAPFILEINFO256* pMenu5;

	DEBUGMSG( 1, (" TASK:TAVI_DrawMenuItem2(%d)", menuset) );
	DEBUGMSG(1,("\n---------------------------------------\n before DrawMenuItem. pTAVI->usCurrentMenu : [%d] \n", pTAVI->usCurrentMenu ));

	input = menuset;
//	test = menuset/10;	// 12, 23, 34, 45, 56, 67, 78, 89, 91

	if( input < 1 || input > 98)
		{
		DEBUGMSG(1,("TAVI_DrawMenuItem2() : menuset=[%d] input=[%d] ... return FALSE\n", menuset, input ));
		return FALSE;
		}

	if( input % 10 == 0 )	// 10 20 30 40 50 60 70 80 90 
		{
		pMenu5 		= &TAVI_IMAGES[ DigitToMACRO( input ) ];
		pMpegDecoder->RenderOsdBits_WithoutRefresh( pMenu5->image_buffer, 0, _MENUV2_START_Y, 320, 151 );	
		TAVI_DrawSubMenu2(  pTAVI->usCurrentMenu, 0, 213 ); 
		pTAVI->usCurrentMenu = input;

		DEBUGMSG(1,("TAVI_DrawMenuItem2() : pTAVI->usCurrentMenu=[%d] input=[%d] ... return TRUE\n", 
			pTAVI->usCurrentMenu, input ));

		return TRUE;
		
		}
	
	first			= DigitToMACRO( (input/10)*10 );		// 12  ...  1
	last			= DigitToMACRO( (input%10)*10 );		// 12 ...  2

	DEBUGMSG( 1, ("  I(%d) : F(%d)->L(%d)", input,  first, last) );

	
 	pMenu1 		= &TAVI_IMAGES[first];


	if(  first==M1&&last==M9 )
		{
		pMenu2 		= &TAVI_IMAGES[MM1];
		pMenu3 		= &TAVI_IMAGES[MM2];
		pMenu4 		= &TAVI_IMAGES[MM3];
		}
	else if (first==M9&&last==M1 )
		{
		pMenu2 		= &TAVI_IMAGES[MM3];		
		pMenu3 		= &TAVI_IMAGES[MM2];		
		pMenu4 		= &TAVI_IMAGES[MM1];				
		}

	else if( first > last )
		{
		pMenu2 		= &TAVI_IMAGES[MM1];
		pMenu3 		= &TAVI_IMAGES[MM2];
		pMenu4 		= &TAVI_IMAGES[MM3];
		}
	else 
		{
		pMenu2 		= &TAVI_IMAGES[MM3];		
		pMenu3 		= &TAVI_IMAGES[MM2];		
		pMenu4 		= &TAVI_IMAGES[MM1];				
		}


	pMenu5 		= &TAVI_IMAGES[last];


	pMpegDecoder->RenderOsdBits( 
			pMenu1->image_buffer, 
			_MENUV2_START_X,
			_MENUV2_START_Y,
			_MENUV2_WIDTH,
			_MENUV2_HEIGHT
			);
	usleep(10000);
	pMpegDecoder->RenderOsdBits( 
			pMenu2->image_buffer, 
			_MENUV2_START_X,
			_MENUV2_START_Y,
			_MENUV2_WIDTH,
			_MENUV2_HEIGHT
			);
	usleep(10000);
	pMpegDecoder->RenderOsdBits( 
			pMenu3->image_buffer, 
			_MENUV2_START_X,
			_MENUV2_START_Y,
			_MENUV2_WIDTH,
			_MENUV2_HEIGHT
			);
	usleep(10000);
	pMpegDecoder->RenderOsdBits( 
			pMenu4->image_buffer, 
			_MENUV2_START_X,
			_MENUV2_START_Y,
			_MENUV2_WIDTH,
			_MENUV2_HEIGHT
			);
	usleep(10000);	
	pMpegDecoder->RenderOsdBits( 
			pMenu5->image_buffer, 
			_MENUV2_START_X,
			_MENUV2_START_Y,
			_MENUV2_WIDTH,
			_MENUV2_HEIGHT
			);
	


	pTAVI->usCurrentMenu = (menuset%10)*10;
	TAVI_DrawSubMenu2(  pTAVI->usCurrentMenu, 0, 213 ); 

	DEBUGMSG(1,("\n---------------------------------------\n after DrawMenuItem. pTAVI->usCurrentMenu : [%d] \n", pTAVI->usCurrentMenu ));
	
	return TRUE;
}



#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

#include <unistd.h>
#include <dirent.h>

#include <stdio.h>	// need sprintf
#include <string.h>
#include <stdlib.h>
#include <memory.h>	// memmove/memcpy
#include <errno.h>
#include "tavi_global.h"

CSndCtrl			*g_SndCtrl;
CMiscCtrl			*g_MiscCtrl;
CTopPanel			*g_TopPanel;
CExtLCD				*g_ExtLCD;
//extern RMint32 toggle_display;

static void InitDeviceInfo( void )
{
	// Sound Control Class: 
	// NOTE: DO NOT USE CSndCtrl::SetEarphone/SpeakerVolume(). These method are depricated.
	//           CSncCtrl::SetVolume() is recommended. 
	// Lee Seok<zstein@newmedialife.com>
	g_SndCtrl = new CSndCtrl( "/dev/misc/sound" );

	// Set last volume
	g_SndCtrl->SetVolume( TaviSysParam.sound.volume );
	g_SndCtrl->SetBalance( TaviSysParam.sound.balance );


	// Miscellaneous Control Class:
	// Input AC dectect, USB connect, ...
	// Output Power-off signal
	g_MiscCtrl = new CMiscCtrl( "/dev/misc/tmisc" );
	// set the intensity of backlight
	g_MiscCtrl->LCDBrightness( TaviSysParam.etc.lcdbright );
	// CTopPanel Class:
	// This class controls states of battery level, alaram, hold, headphone/speaker.
	// The first arg. of constructor is parent window, second battery level, next headphone(true headphone, false speaker),
	// next alaram, last hold.
	g_TopPanel = new CTopPanel( NULL, CTopPanel::BATTERY_LEVEL_4, false, false );
	g_TopPanel->Resize( CRect(0, 0, 319, 23), false );

	char nDirty;
	g_MiscCtrl->Update( &nDirty );
	// check AC power & battery level
	//g_TopPanel->SetBatteryLevel( (CTopPanel::BatteryLevel)CalcBatteryLevel() );

	// Check whether folder is open or not.
	g_MiscCtrl->EXT_EL_Enable( false );
	
	if( g_MiscCtrl->FolderOpen() ) {
		g_MiscCtrl->MainLCDOnOff( true );
		g_MiscCtrl->EXT_EL_Enable( false ); // turn off sub lcd backlight
	}
	else {
		g_MiscCtrl->MainLCDOnOff( false );
		g_MiscCtrl->EXT_EL_Enable( true ); 
	}
	// Check whether a headphone is connected or not
	if( g_MiscCtrl->HeadphoneIN() ) {
		TaviSysParam.sound.speaker_mute = 1;
		// do something when connected headphone
		g_MiscCtrl->AMPEnable( false );
		g_SndCtrl->SpeakerMute( true );
		g_SndCtrl->EarphoneMute( false );
	}
	else {
		TaviSysParam.sound.speaker_mute = 0;
		// do something when disconnected headphone
		g_SndCtrl->SpeakerMute( false );
		g_MiscCtrl->AMPEnable( true );		
		g_SndCtrl->EarphoneMute( true );
	}
	g_TopPanel->SetSpeaker( !TaviSysParam.sound.speaker_mute );
}

CTextEngine_BDF* pT;

#include "../Lib/LibFlash.h"

#define 	_FLASHPROGRESS_SX					110
#define 	_FLASHPROGRESS_CX					160
#define 	_FLASHPROGRESS_SY					161
#define 	_FLASHPROGRESS_W					100
#define 	_FLASHPROGRESS_H					6

#define FLASHUPDATE_PATH					"/hd1/Update/romfs.bin"
#ifdef CHARGER
#define FLASH_UPDATE_BASE					0x41000
#else	// CHARGER
#define FLASH_UPDATE_BASE					0x40000
#endif 	// CHARGER
#define LOADERUPDATE_PATH					"/hd1/Update/loader.bin"
#define LOADER_UPDATE_BASE				0
#ifdef CHARGER
#define BUFFERSIZE4LOADER					(1024*260+20)	// 266,240 Bytes
#else	// CHARGER
#define BUFFERSIZE4LOADER					(1024*256+20)	// 262,164 Bytes
#endif	// CHARGER
#define BUFFERSIZE4ROMFS					(1024*1024*4-576- BUFFERSIZE4LOADER) // 3,931,564Bytes

#define SET_SYSTEMCONTROLREG(x)			*((volatile unsigned long*)(0x00500020))=(x)

#define __MESSAGE_BASE_BG					_IMAGE_DIR"/bg/message_base.jpg"
#define __FIRMWARE_BG						_IMAGE_DIR"/bg/firmware.jpg"
#define __FIRMWARE_COMPLETE_BG			_IMAGE_DIR"/bg/firmware_complete.jpg"
#define MAKE_VERSION(a, b, c, d)				(((a) << 24) | ((b) << 16) | (c)<< 8 | (d) )

#define ROMFS_MAGIC_1 						('-' + ('r'<<8) + ('o'<<16) + ('m'<<24))
#define ROMFS_MAGIC_2 						('1' + ('f'<<8) + ('s'<<16) + ('-'<<24))

extern RMuint32	*shm_buffer;

CMsgBox			*pFirmUpMsgBox	= NULL;
int				nFirmUpConfirm	= 0;
int				nCheckUpdate;

typedef int (*FLASH_PROGRESS)(int percent);

// Check the romfs starting at 'addr' (with -romfs- marker)
static unsigned int romfs_check(unsigned int *addr)
{

    if(((*addr)==ROMFS_MAGIC_1) && ((*(addr+1))==ROMFS_MAGIC_2))
        return 1;
    else
        return 0;

}

int RecursiveCopy( const uchar_t* pathSource, const uchar_t* pathTarget, int total ,FLASH_PROGRESS progress, int base, int portion)
{
	char szCurPathSou[2048];
	char szCurPathTar[2048];
	CStack< CString > dirStackSou( 100 );
	CStack< CString > dirStackTar( 100 );
	CString szDirPathSou;
	CString szDirPathTar;
	uchar_t* wszPath;
	FATFS_ERROR sdFatfsErr;
	FATFS_ERROR cpFatfsErr;
	FATFS_DIR_ENTRY_INFO sdDirEntrySou;
	FATFS_DIR_ENTRY_INFO sdDirEntryTar;
	RMuint32 hDirSou;
	RMuint32 hDirTar;
	char delimeter[4] = { '/', 0, 0, 0 };
	CString szPathSou;
	CString szPathTar;
	CString szTmp;
	RMuint32		fdsrc;
	RMuint32		fddst;
	RMuint32 	nLength;
	RMuint32 	nWlength;
	RMuint8 	cpdatebuf[16*1024];
	int	count=0;
	
	printf( "+RecursiveCopy\n" );

	szPathSou.SetString( (char*)pathSource, true );
	szPathTar.SetString( (char*)pathTarget, true );	
	dirStackSou.Push( szPathSou );
	dirStackTar.Push( szPathTar );
	while( dirStackSou.IsEmpty() == false ) {
		szDirPathSou = dirStackSou.Pop();
		szDirPathTar = dirStackTar.Pop();
		ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
		ustrcpy( (uchar_t*)szCurPathTar, (uchar_t*)szDirPathTar );
		wszPath = ( uchar_t* )szDirPathSou;
		sdFatfsErr = fatfs_wdopen( (RMuint16*)wszPath, (RMuint32*)&hDirSou );
		while( sdFatfsErr == FATFS_ERROR_NO_ERROR ) {
			sdFatfsErr = fatfs_dread( hDirSou, &sdDirEntrySou );
			if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) break;
			if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 && 
			     sdDirEntrySou.name[2] == 0 && sdDirEntrySou.name[3] == 0 ) continue;
			if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 &&
				sdDirEntrySou.name[2] == '.' && sdDirEntrySou.name[3] == 0 ) continue;

			ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
			ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)delimeter );
			ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)sdDirEntrySou.name );
	
			ustrcpy( (uchar_t*)szCurPathTar, (uchar_t*)szDirPathTar );
			ustrcat( (uchar_t*)szCurPathTar, (uchar_t*)delimeter );
			ustrcat( (uchar_t*)szCurPathTar, (uchar_t*)sdDirEntrySou.name );


			if( sdDirEntrySou.attributes&ATTR_DIRECTORY ) {
				szPathSou.SetString( szCurPathSou, true );
				dirStackSou.Push( szPathSou );
				szPathTar.SetString( szCurPathTar, true );
				dirStackTar.Push( szPathTar );
#if 0				
				printf("-dir-sou->" );
				szTmp.SetString( szCurPathSou, true );
				szTmp.Print();
				printf( "\n" );
#endif				
				if( progress ) progress( (int)( base +( portion*count )/total ) );
				count++;
//				printf(" ---- dir change =%d\n", count );
			}
			else {
#if 0
				printf("-file-sou->" );
				szTmp.SetString( szCurPathSou, true );
				szTmp.Print();
				printf( "\n" );
#endif
				cpFatfsErr = fatfs_wfopen( (RMuint16*)szCurPathSou, _O_RDONLY, &fdsrc );
				if ( cpFatfsErr == FATFS_ERROR_NO_ERROR ) {
					cpFatfsErr = fatfs_wfopen( (RMuint16*)szCurPathTar, _O_WRONLY|_O_CREAT, &fddst );
					if ( cpFatfsErr == FATFS_ERROR_NO_ERROR ) {
						do {
							cpFatfsErr = fatfs_fread( fdsrc, cpdatebuf, sizeof(cpdatebuf), &nLength );
							cpFatfsErr = fatfs_fwrite( fddst, cpdatebuf, nLength, &nWlength );
//							printf(" source = %d, dest = %d\n", nLength, nWlength );
							} while (nLength > 0 );
						}
					}
				fatfs_fclose( fdsrc );
				fatfs_fclose( fddst );

#if 0
				printf("-file-tar->" );
				szTmp.SetString( szDirPathTar, true );
				szTmp.Print();
				printf( "\n" );
#endif
				if( progress ) progress( (int)( base +( portion*count)/total )  );
				count++;
//				printf(" ---- file copy =%d\n", count );
			}
		}
		fatfs_dclose( hDirSou );
	}

	printf( "-RecursiveCopy=%d\n",count );
	return 0;
}

int RecursivePreScan( const uchar_t* pathSource )
{
	char szCurPathSou[2048];
	CStack< CString > dirStackSou( 100 );
	CString szDirPathSou;
	uchar_t* wszPath;
	FATFS_ERROR sdFatfsErr;
	FATFS_DIR_ENTRY_INFO sdDirEntrySou;
	RMuint32 hDirSou;
	char delimeter[4] = { '/', 0, 0, 0 };
	CString szPathSou;
	CString szTmp;
	int count=0;
	

	printf( "+RecursivePreScan\n" );

	szPathSou.SetString( (char*)pathSource, true );
	dirStackSou.Push( szPathSou );
	while( dirStackSou.IsEmpty() == false ) {
		szDirPathSou = dirStackSou.Pop();
		ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
		wszPath = ( uchar_t* )szDirPathSou;
		sdFatfsErr = fatfs_wdopen( (RMuint16*)wszPath, (RMuint32*)&hDirSou );
		while( sdFatfsErr == FATFS_ERROR_NO_ERROR ) {
			sdFatfsErr = fatfs_dread( hDirSou, &sdDirEntrySou );
			if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) break;
			if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 && 
			     sdDirEntrySou.name[2] == 0 && sdDirEntrySou.name[3] == 0 ) continue;
			if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 &&
				sdDirEntrySou.name[2] == '.' && sdDirEntrySou.name[3] == 0 ) continue;

			ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
			ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)delimeter );
			ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)sdDirEntrySou.name );
	
			if( sdDirEntrySou.attributes&ATTR_DIRECTORY ) {
				szPathSou.SetString( szCurPathSou, true );
				dirStackSou.Push( szPathSou );

				count++;
#if 0
				printf("-dir-sou->" );
				szTmp.SetString( szCurPathSou, true );
				szTmp.Print();
				printf( " = %d\n", count );
#endif
				
			}
			else {
				count++;
#if 0
				printf("-file-sou->" );
				szTmp.SetString( szCurPathSou, true );
				szTmp.Print();
				printf( " = %d\n", count );
#endif				
			}
		}
		fatfs_dclose( hDirSou );
	}

	printf( "-RecursivePreScan = %d\n", count );
	return count;
}


int RecursiveDelete( const uchar_t* pathSource, int total, FLASH_PROGRESS progress, int base, int portion )
{
	char szCurPathSou[2048];
	CStack< CString > dirStackSou( 100 );
	CString szDirPathSou;
	uchar_t* wszPath;
	FATFS_ERROR sdFatfsErr;
	FATFS_DIR_ENTRY_INFO sdDirEntrySou;
	RMuint32 hDirSou;
	char delimeter[4] = { '/', 0, 0, 0 };
	CString szPathSou;
	CString szTmp;
	CString szTmp2;
	CString szRomfsName;
	CString szLoaderName;
	RMuint32 nLength;
	uchar_t ucText[10];
	int	count=0;

	printf( "+RecursiveDelete\n" );
	
	sprintf( szCurPathSou, "romfs.bin" );
	str2ustr( ucText , szCurPathSou, strlen(szCurPathSou) );
	szRomfsName.SetString( (char*)ucText, true);
	sprintf( szCurPathSou, "loader.bin" );
	str2ustr( ucText , szCurPathSou, strlen(szCurPathSou) );
	szLoaderName.SetString( (char*)ucText, true);


	szPathSou.SetString( (char*)pathSource, true );
	dirStackSou.Push( szPathSou );
	szDirPathSou = szPathSou;
	ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
	wszPath = ( uchar_t* )szCurPathSou;
#if 0
	printf("-dir-start->" );
	szTmp.SetString( szCurPathSou, true );
	szTmp.Print();
	printf( "\n" );
#endif

	while( dirStackSou.IsEmpty() == false ) {

		wszPath = ( uchar_t* )szCurPathSou;
		szDirPathSou.SetString( szCurPathSou, true );
		sdFatfsErr = fatfs_wdopen( (RMuint16*)wszPath, (RMuint32*)&hDirSou );
#if 0
		printf("-dir-open->" );
		szTmp.SetString( szCurPathSou, true );
		szTmp.Print();
		printf( "\n" );
#endif
		while( sdFatfsErr == FATFS_ERROR_NO_ERROR ) {
			sdFatfsErr = fatfs_dread( hDirSou, &sdDirEntrySou );
			if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) break;
			if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 && 
			     sdDirEntrySou.name[2] == 0 && sdDirEntrySou.name[3] == 0 ) continue;
			if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 &&
				sdDirEntrySou.name[2] == '.' && sdDirEntrySou.name[3] == 0 ) continue;

			ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
			ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)delimeter );
			ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)sdDirEntrySou.name );
	
			if( sdDirEntrySou.attributes&ATTR_DIRECTORY ) {
				szPathSou.SetString( szCurPathSou, true );
				dirStackSou.Push( szDirPathSou );
#if 0
				printf("-dir-push->" );
				szTmp.SetString( szDirPathSou, true );
				szTmp.Print();
				printf( "\t" );
				szTmp.SetString( sdDirEntrySou.name, true );
				szTmp.Print();
				printf( "\n" );
#endif
				szTmp2.SetString( sdDirEntrySou.name, true );
				break;
			}
			else {
#if 0
				printf("-file-sou->" );
				szTmp.SetString( szCurPathSou, true );
				szTmp.Print();
				printf( "\t" );
				szTmp.SetString( sdDirEntrySou.name, true );
				szTmp.Print();
				printf( "\n" );
#endif
				szTmp.SetString(sdDirEntrySou.name, true );
				if ( szTmp == szRomfsName || szTmp ==szLoaderName )
				{
				}
				else
				{
					nLength=fatfs_wddelete_file( hDirSou, (RMuint16*)sdDirEntrySou.name );
//					printf("fatfs_wddelete_file =0x%x\n",nLength );

					if( progress ) progress( (int)( base +( portion*count)/total ) );
					count++;
//					printf(" ---- file delete =%d\n", count );
	
				}
			}
		}
		fatfs_dclose( hDirSou );
		if (sdFatfsErr == FATFS_ERROR_NO_ERROR)
		{
		}
		else
		{
			szDirPathSou = dirStackSou.Pop();
			ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
			wszPath = ( uchar_t* )szCurPathSou;
#if 0
			printf("-dir-pop-->" );
			szTmp.SetString( szCurPathSou, true );
			szTmp.Print();
			printf( "\t" );
			szTmp2.Print();
			printf("\n" );			
#endif
			sdFatfsErr = fatfs_wdopen( (RMuint16*)wszPath, (RMuint32*)&hDirSou );
			nLength=fatfs_wddelete_dir( hDirSou, (RMuint16*)szTmp2 );
//			printf("fatfs_wddelete_dir =0x%x\n",nLength );
			szTmp2.SetString( GetFilename( szCurPathSou ), true );

			if( progress ) progress( (int)( base +( portion*count)/total ) );
			count++;
//			printf(" ---- dir delete =%d\n", count );

		}
		fatfs_dclose( hDirSou );
	}

	printf( "-RecursiveDelete=%d\n",count );
	return 0;
}
// ======== power save ===============
void TAVI_InitSleepReg( void )
{
	g_HddSleep.m_bOnSleep = false;
	g_HddSleep.m_lLastAccess = gettime_sec();
	g_HddSleep.m_nHddSleeping = 0;
	g_HddSleep.m_nHddSleeping4Second= 0;
}

void TAVI_CanSleep( int debug )
{
	if ( g_HddSleep.m_nHddSleeping > 0 )
		g_HddSleep.m_nHddSleeping = 0;

	g_HddSleep.m_lLastAccess = gettime_sec();

	DEBUGMSG(1, ("-- (%03d) CanSleep    : Key1[%d], Key2[%d] :: time =%d\n", 
		debug, g_HddSleep.m_nHddSleeping, g_HddSleep.m_nHddSleeping4Second, g_HddSleep.m_lLastAccess ) );

}

void TAVI_CannotSleep( int debug )
{
	if ( g_HddSleep.m_bOnSleep )
		TAVI_WakeUp( debug );
	
	g_HddSleep.m_nHddSleeping = 1;
	DEBUGMSG(1, ("++ (%03d) CannotSleep : Key1[%d], Key2[%d]\n", 
		debug,  g_HddSleep.m_nHddSleeping, g_HddSleep.m_nHddSleeping4Second ) );

}

void TAVI_CanSleepAudio( int debug )
{
	TTaskList[TASK_ID_MUSIC].SleepingKey = 0;

	if ( pTAVI->usCurrentMenu  != (TASK_ID_MUSIC * 10) &&
	    pTAVI->usCurrentMenu  != (TASK_ID_RECORD * 10) ) {
		g_HddSleep.m_nHddSleeping4Second = 0;
	} else {
		g_HddSleep.m_nHddSleeping = 0;
	}
	g_HddSleep.m_lLastAccess = gettime_sec();
	DEBUGMSG(1, ("-- (%03d) CanSleepAudio : Key@Audio[%d], Key1[%d], Key2[%d] :: time =%d\n", 
		debug, TTaskList[TASK_ID_MUSIC].SleepingKey,
		g_HddSleep.m_nHddSleeping, g_HddSleep.m_nHddSleeping4Second,
		g_HddSleep.m_lLastAccess ) );

}

void TAVI_CannotSleepAudio( int debug )
{
	if ( g_HddSleep.m_bOnSleep )
		TAVI_WakeUp( debug );

	TTaskList[TASK_ID_MUSIC].SleepingKey = 1;

	if ( pTAVI->usCurrentMenu  != (TASK_ID_MUSIC * 10) &&
	    pTAVI->usCurrentMenu  != (TASK_ID_RECORD * 10) ) {
		g_HddSleep.m_nHddSleeping4Second = 1;
	} else {
		g_HddSleep.m_nHddSleeping = 1;
	}
	DEBUGMSG(1, ("++ (%03d) CannotSleepAudio : Key@Audio[%d], Key1[%d], Key2[%d]\n", 
		debug,  TTaskList[TASK_ID_MUSIC].SleepingKey,
		g_HddSleep.m_nHddSleeping, g_HddSleep.m_nHddSleeping4Second ) );

}

void TAVI_ResetSleepTimer(void)
{
	g_HddSleep.m_lLastAccess = gettime_sec();
//	DEBUGMSG(1, (" ::TAVI_ResetSleepTimer=%d\n", g_HddSleep.m_lLastAccess ) );
}

void TAVI_SleepDown( long lCurrentTime )
{  
	if (g_MiscCtrl->ACDetected() )
		return;
	
	if ( TaviSysParam.power.hddtimer > 0 && 
		( ( lCurrentTime-g_HddSleep.m_lLastAccess) == TaviSysParam.power.hddtimer )  )
	{
		if ( g_HddSleep.m_bOnSleep )
		{
		}
		else
		{
			if( g_HddSleep.m_nHddSleeping == 0  && g_HddSleep.m_nHddSleeping4Second == 0 ) 
			{
				fs_fat_powerdown();
				g_HddSleep.m_bOnSleep = TRUE;
				DEBUGMSG(1, ("!!       SleepDown   : %d sec(=%d-%d)\n", 
					(lCurrentTime-g_HddSleep.m_lLastAccess), lCurrentTime, g_HddSleep.m_lLastAccess) );
			}
		}
	}
}

void TAVI_WakeUp( int debug )
{
	if ( TaviSysParam.power.hddtimer > 0 )
	{

		DEBUGMSG(1,("!! (%03d) WakeUp      : Key1[%d], Key2[%d]", 
			debug, g_HddSleep.m_nHddSleeping, g_HddSleep.m_nHddSleeping4Second) );

		if ( g_HddSleep.m_bOnSleep )
		{
			fs_fat_powerup();
			g_HddSleep.m_bOnSleep = FALSE;
			DEBUGMSG(1, (" :: HDD Power Up" ) );
		}
		g_HddSleep.m_lLastAccess = gettime_sec();
		DEBUGMSG(1, (" :: time =%d\n", g_HddSleep.m_lLastAccess  ) );
	}
}
// ======== power save ===============


int TAVI_FlashProgress( int nPercent )
{
	int 	i;
	int	nMajor;
	int	nMinor;
	char szTemp[25];
	uchar_t 	ucText[25];
	int	nHalfWidth;

	DEBUGMSG (0, ("flash_progress (%d)", nPercent));

	if ( nPercent == 0 )
	{
		op_DrawOsdBox( 0, _FLASHPROGRESS_SX-60, _FLASHPROGRESS_SY+13 , 250, 16);	// clear text message

		pT->DrawResText(ML_FIRMWARE_UPGRADE,								// disp "Firmware upgrade"
			0, _FLASHPROGRESS_SY,
			_WHITE,
			12,
			DT_TRANSPARENCY | DT_ALIGN_CENTER );
		DrawBoxWithoutRefresh( 139, _FLASHPROGRESS_SX, _FLASHPROGRESS_SY+22, _FLASHPROGRESS_W,_FLASHPROGRESS_H);
		DrawItemWithoutRefresh( 	&TAVI_IMAGES[PROGRESSBAR_EDGE], 5,
			_FLASHPROGRESS_SX,	_FLASHPROGRESS_SY+22, 4,_FLASHPROGRESS_H);			// display left part of progress bar 

		DrawItemWithoutRefresh( 	&TAVI_IMAGES[PROGRESSBAR_EDGE], 6,
			_FLASHPROGRESS_SX + 98, _FLASHPROGRESS_SY+22 , 4,_FLASHPROGRESS_H);	// display right part of progress bar

		pT->DrawResText(ML_MSG_FWUP,										// disp warning
			0, _FLASHPROGRESS_SY+60,
			_RED,
			12,
			DT_TRANSPARENCY | DT_ALIGN_CENTER );
	}
	else if ( nPercent >= 99 )
	{
		DrawItemWithoutRefresh( 	&TAVI_IMAGES[PROGRESSBAR_EDGE], 4,
			_FLASHPROGRESS_SX + _FLASHPROGRESS_W -2, _FLASHPROGRESS_SY+22, 4,_FLASHPROGRESS_H);	// display right part of progress bar
	}
	else 
	{
		DrawBoxWithoutRefresh( 17, _FLASHPROGRESS_SX+2, _FLASHPROGRESS_SY+22, (int)(nPercent*0.98),_FLASHPROGRESS_H);
	}

	op_DrawOsdBox( 0, _FLASHPROGRESS_CX - 20, _FLASHPROGRESS_SY + 32, 50, 16);	// clear text message

	sprintf( szTemp,"%d %%",nPercent+1 );
	str2ustr( ucText, szTemp, strlen(szTemp) );
	nHalfWidth = pT->GetStringWidth(ucText , 12, 0 )/2;
	pT->DrawUC16Text( ucText,
		_FLASHPROGRESS_CX - nHalfWidth, _FLASHPROGRESS_SY+32,
		_GREEN,
		12,
		DT_TRANSPARENCY
		);

	pMpegDecoder->RefreshOsd();

	return 0;
}

static bool KillTask( TTaskEntryBlock* task )
{
	if( !task ) return false;
	
	if ( task->fpExitFunc ) task->fpExitFunc();
	TTaskList[task->taskID].usTaskState = TASK_STOP;
}

#define LENGTH_TAG			3
#define LENGTH_KEY			16
#define LENGTH_TAGNKEY 	(LENGTH_TAG+LENGTH_KEY)

int TAVI_CheckMd5Key( char *buffer, int length )
{
	int	i;
	int	ret;
	int	length_withNokey;
	unsigned char FileMd5[LENGTH_KEY];
	unsigned char *pGenMd5;
	CMD5 *pMd5 = new CMD5();

	ret = -1;	// not matched md5
/*
	for ( int i=LENGTH_TAGNKEY;i>0;i--)
		printf("%d=[0x%02x]\n", i, buffer[length -i] );
*/	
	if ( buffer[length-(LENGTH_TAGNKEY)] == 'm' && 
		buffer[length-(LENGTH_TAGNKEY)+1] == 'd' &&
		buffer[length-(LENGTH_TAGNKEY)+2] == '5' ) 	{
		// exist md5 tag
		length_withNokey = length - (LENGTH_TAGNKEY);
		memcpy( FileMd5, &buffer[length - LENGTH_KEY], LENGTH_KEY );
		pMd5->md5_update( (unsigned char *)buffer, length_withNokey);
		pMd5->md5_finish();

		if ( memcmp( pMd5->md5_get(), FileMd5, LENGTH_KEY ) == 0 ) {
			// match md5 key
			ret = 0;
			}

		pGenMd5=pMd5->md5_get();
		printf("MD5@FILE  ");
		for ( i=0;i<LENGTH_KEY;i++) {
			printf(":%02X", FileMd5[i] );
			}
		printf("\nGenerated ");
		for ( i=0;i<LENGTH_KEY;i++) {
			printf(":%02X", pGenMd5[i] );
			}
		printf(" [%s]\n", (ret==0?"Match":"Different" ) );

		}

	delete pMd5;
	return ret;
}


int TAVI_DoFirmwareUpgrade(void)
{
	TMsgEntry 	tMsg;
	RMuint32* updatebuf4loader = shm_buffer;
	RMuint32* updatebuf4romfs = updatebuf4loader + (BUFFERSIZE4LOADER/4);
	unsigned short 	pll;
	unsigned short 	uart;
	int	nReturn;
	int	fd;
	int	nLlength;
	int	nRlength;
	int	nTotalLength;
	int 	base1;
	int 	base2;
	int 	base3;
	int 	base4;
	int	portion1;
	int	portion2;
	int	portion3;
	int	portion4;
	char szTemp[32];
	uchar_t ucSource[32];
	uchar_t ucTarget[32];
	int	count;
	int	i;
	int	nValid4loader;
	int	nValid4romfs;
	
	nReturn = 1;
	nLlength = 0;
	nRlength = 0;
	base1 = 0;	
	base2 = 0;	
	base3 = 0;	
	base4 = 0;	
	portion1 = 0;	
	portion2 = 0;	
	portion3 = 0;	
	portion4 = 0;	

	op_ClearAreaOSD( 0, 0, 320, 240 );
	showbackground( __FIRMWARE_BG );	
	TAVI_LoadPaletteTransparencyData(TRUE);
	TAVI_FlashProgress(0);
	//=============== update ========================

	fd = fs_fopen( LOADERUPDATE_PATH, O_RDONLY );
	nLlength = fs_fread( fd, updatebuf4loader, BUFFERSIZE4LOADER, NULL );
	fs_fclose( fd, NULL );
	if ( fd ) {
		if ( nLlength < BUFFERSIZE4LOADER ) {
			nValid4loader =TAVI_CheckMd5Key((char *)updatebuf4loader, nLlength);
			if ( nValid4loader == 0 ) {
				nLlength -= (LENGTH_TAGNKEY);	// remove md5 tag & key
				if( nLlength%2 ) nLlength++;
				DEBUGMSG( 1,( "Task.cpp : loader.bin file size : 0x%08lx\n", nLlength ) );
				}
			}
		else {
			nValid4loader = -1;
			nLlength = 0;
			}
		}
	else {
		nValid4loader = 0;
		nLlength = 0;
		}
	
	fd = fs_fopen( FLASHUPDATE_PATH, O_RDONLY );
	nRlength= fs_fread( fd, updatebuf4romfs, BUFFERSIZE4ROMFS, NULL );
	fs_fclose( fd, NULL );
	if ( fd && ( nRlength < BUFFERSIZE4ROMFS) ) {
		nValid4romfs = TAVI_CheckMd5Key((char *)updatebuf4romfs, nRlength);
		if ( nValid4romfs == 0 ) {
			nRlength -= (LENGTH_TAGNKEY);	// remove md5 tag & key
			if( nRlength%2 ) nRlength++;
			DEBUGMSG( 1,( "Task.cpp : romfs.bin file size : 0x%08lx\n", nRlength ) );
			}
		}
	else	{
		nValid4romfs = -1;		
		nRlength = 0;
		}

	nTotalLength = nRlength + nLlength;


	if ( nValid4romfs==0 && nValid4loader == 0)
	{
		sprintf(szTemp,"/hd1/Update/system");
		str2ustr(ucSource, szTemp, strlen(szTemp) );
		sprintf(szTemp,_SYSTEMDIR);
		str2ustr(ucTarget, szTemp, strlen(szTemp) );
		count = RecursivePreScan(ucSource);							// query how many files exist on 

		if ( count > 0 )
		{
			base1 = 0;
			portion1 = ( (count/25)>10 )?10:( ((count/25)<1)?1:(count/25) );
			base2 = portion1;
			portion2 = portion1;
			RecursiveCopy(ucSource, ucTarget, count,TAVI_FlashProgress, base1, portion1);					// folder copy

			sprintf(szTemp,"/hd1/Update");
			str2ustr(ucSource, szTemp, strlen(szTemp) );
			RecursiveDelete(ucSource, count+3, TAVI_FlashProgress, base2, portion2);						// folder delete

		}

		base3 = base2+portion2;
		portion3 =( nRlength * (100-portion2-portion1) )/nTotalLength;
		base4 = base3 + portion3;
		portion4 =(100-portion3-portion2-portion1);

		printf("base1=%d,prot1=%d,base2=%d,prot2=%d,base3=%d,prot3=%d,base4=%d,prot4=%d \n",
			base1,portion1,base2,portion2,base3,portion3,base4,portion4 );

		if ( flash_init() == 0 )	
		{
			if ( flash_erase( FLASH_UPDATE_BASE, nRlength, TAVI_FlashProgress, base3, portion3 ) == 0 ) 
			{
				if ( flash_write((unsigned short *)updatebuf4romfs, FLASH_UPDATE_BASE, nRlength, TAVI_FlashProgress, base3, portion3 ) == 0 ) 
				{
					if ( nValid4loader == 0 && nLlength > 0 )
					{
						if ( flash_erase( LOADER_UPDATE_BASE, nLlength, TAVI_FlashProgress, base4, portion4 ) == 0 ) 
						{
							if ( flash_write((unsigned short *)updatebuf4loader, LOADER_UPDATE_BASE, nLlength, TAVI_FlashProgress, base4, portion4 ) == 0) 
							{
								fatfs_delete_file ( (RMuint8 *)LOADERUPDATE_PATH );
								nReturn= 0;
							}
							else
							{
								DEBUGMSG(1,("Task.cpp : Loader write error\n") );
							}
						}
						else
						{
							DEBUGMSG(1,("Task.cpp : Loader erase error\n") );
						}
					}
					else
					{
						nReturn= 0;
					}	
				}
				else
				{
					DEBUGMSG(1,("Task.cpp : rom write error\n") );
				}
			}
			else
			{
				DEBUGMSG(1,("Task.cpp : rom erase error\n") );
			}
		}
		else
		{
			DEBUGMSG(1,("Task.cpp : flash init error\n") );
		}

	}
	//=============== loader.bin update ========================
	TAVI_LoadPaletteTransparencyData( FALSE );
	op_ClearAreaOSD( 0, 0, 320, 240 );
	
	return nReturn;

}

int TAVI_UpdateBatteryLowMsgProc ( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	TMsgEntry tMsg;
	
	switch( ssMsgId )
	{
		case MSG_BUTTON_UP:
			switch( usParam1 )
			{
				case TAVI_KEY_ENTER:
					DEBUGMSG(1,("Task.cpp : TAVI_UpdateBatteryLowMsgProc : TAVI_KEY_ENTER\n" ));
				
					nFirmUpConfirm =0;

					delete pFirmUpMsgBox;
					pFirmUpMsgBox=NULL;
					DrawBoxWithoutRefresh(67, 0, g_TopPanel->rcABS.GetHeight(), 
						320, 240-g_TopPanel->rcABS.GetHeight());
					g_TopPanel->Update();
					TAVI_DrawMenuItem3( pTAVI->usCurrentMenu );
					break;
			}
			break;
	}
	return 0;	
}

int TAVI_UpdateFailureMsgProc ( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	TMsgEntry tMsg;
	
	switch( ssMsgId )
	{
		case MSG_BUTTON_UP:
			switch( usParam1 )
			{
				case TAVI_KEY_ENTER:
					DEBUGMSG(1,("Task.cpp : TAVI_UpdateFailureMsgProc : TAVI_KEY_ENTER\n" ));
				
					nFirmUpConfirm =0;

					delete pFirmUpMsgBox;
					pFirmUpMsgBox=NULL;
					DrawBoxWithoutRefresh(67, 0, g_TopPanel->rcABS.GetHeight(), 
						320, 240-g_TopPanel->rcABS.GetHeight());
					g_TopPanel->Update();
					TAVI_DrawMenuItem3( pTAVI->usCurrentMenu );
					break;
			}
			break;
	}
	return 0;	
}

int TAVI_FirmwareConfirmMsgProc ( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	TMsgEntry tMsg;
	int	n;

	ClearKeyBuffer();

	switch( usParam1 )
	{
		case TAVI_KEY_LEFT:
			nFirmUpConfirm = pFirmUpMsgBox->CursorLeft();
			break;
		case TAVI_KEY_RIGHT:
			nFirmUpConfirm = pFirmUpMsgBox->CursorRight();
			break;
	
		case TAVI_KEY_ENTER:
			DEBUGMSG(1,("Task.cpp : FirmwareConfirmMsgProc : TAVI_KEY_ENTER\n" ));
			
			if (nFirmUpConfirm == 1)
			{
				if ( CalcBatteryLevel()  <= CTopPanel::BATTERY_LEVEL_1)
				{
					pFirmUpMsgBox->Reset( ML_FIRMWARE_UPGRADE,
						_ORANGE,
						ML_FW_UPGRADE_FAILED2,
						244, 123,
						TAVI_UpdateBatteryLowMsgProc,
						DL_BG_BLANK | DL_BTN_OK_ONLY
						);
					pFirmUpMsgBox->Show();
					nFirmUpConfirm =2;
				}
				else
				{
					DISABLE_WATCHDOGTIMER;
					nFirmUpConfirm =0;
					if ( TAVI_DoFirmwareUpgrade() == 0 )
					{
						// success upgrade
						showbackground( __FIRMWARE_COMPLETE_BG );	

						pT->DrawResText(ML_SETUP_MSG_RESET_TAVI,				// disp warning
							_FLASHPROGRESS_CX - 100, _FLASHPROGRESS_SY,
							_WHITE,
							12,
							DT_TRANSPARENCY | DT_ENABLE_DIVIDER | DT_ALIGN_CENTER
							);
						pMpegDecoder->RefreshOsd();

						for (n=0; n<30;n++)
							usleep(1000000);

						g_MiscCtrl->MainLCDOnOff( FALSE );
						g_MiscCtrl->PowerLEDOnOff( FALSE );
						g_SndCtrl->DACMute( true );
						// Clear Ext. LCD buffer and power-off
						SubLCD_Exit();
						lcd_disable();

                                        /*
                                            Disable RTC. 
                                         */
						int handle_rtc;

						handle_rtc = open( RTC_DRIVER_PATH, O_RDWR );
						if ( handle_rtc  < 0 ) {
							}
						else {
							ioctl( handle_rtc, RTC_AIE_OFF );
							close ( handle_rtc );
							}
                                        
						if (g_MiscCtrl->ACDetected() ) {
							// ============== system reset ====================
							SET_SYSTEMCONTROLREG(0x80000000);		//res_all
							// ============== system reset ====================
						} else {
							g_MiscCtrl->PowerOff();
						}

						while(1)
							MSG_Peek( g_CurrentTaskID, &tMsg );
					}
					else
					{
						// upgade failure
						DEBUGMSG(1,("Task.cpp : FirmwareConfirmMsgProc : failure firmware update \n") );
						showbackground( ID_BG_MAIN );
						TAVI_LoadPaletteTransparencyData(FALSE);
						DrawBoxWithoutRefresh(67, 0, g_TopPanel->rcABS.GetHeight(), 
							320, 240-g_TopPanel->rcABS.GetHeight());
						g_TopPanel->Update();
						TAVI_DrawMenuItem3( pTAVI->usCurrentMenu );
						
						pFirmUpMsgBox->Reset( ML_FIRMWARE_UPGRADE,
							_ORANGE,
							ML_FW_UPGRADE_FAILED3,
							244, 123,
							TAVI_UpdateFailureMsgProc,
							DL_BG_BLANK | DL_BTN_OK_ONLY
							);
						pFirmUpMsgBox->Show();
						nFirmUpConfirm =2;
						
					}
					ENABLE_WATCHDOGTIMER;
					RELOAD_WATCHDOGTIMER;
				}
			}
			else
			{
				DEBUGMSG(1,("Task.cpp : FirmwareConfirmMsgProc : update EXIT\n" ));
				delete pFirmUpMsgBox;
				pFirmUpMsgBox = NULL;
				DrawBoxWithoutRefresh(67, 0, g_TopPanel->rcABS.GetHeight(), 
					320, 240-g_TopPanel->rcABS.GetHeight());
				TAVI_DrawMenuItem3( pTAVI->usCurrentMenu );
				nFirmUpConfirm =0;
			}

			break;
	}
	return 0;	
}

int TAVI_CheckFirmware( void )
{
	char Buffer[32];
	char	szTemp[32];
	uchar_t ucSource[32];
	
	char szModel[5];
	char szMajor[5];
	char szMinor[5];
	char szPatch[5];
	char szExtra[5];

	int	nMajor;
	int 	nMinor;
	int	nPatch;
	int	nExtra;
	int	nVersion;

	int fd;
	int n;
	int nReturn;



	fd = fs_fopen( FLASHUPDATE_PATH, O_RDONLY );
	DEBUGMSG(1,("Task.cpp : FUpgrade : fs_fopen =%d\n", fd) );

	nReturn  = -1;
	
	if( fd == 0 )
	{
		DEBUGMSG(1,( "Not exist file %s\n", FLASHUPDATE_PATH ) );	
	}
	else
	{
		n = fs_fread( fd, Buffer, 32, NULL );
		fs_fclose( fd, NULL );
		if ( romfs_check( (unsigned int *) Buffer) )	//same as "-rom1fs-" 
		{

			sprintf( szTemp,"%s", Buffer+16 );
			DEBUGMSG( 1,( "Task.cpp : FUpgrade : Volume Name : %s\n", szTemp ) );
			sscanf(szTemp,"%[a-zA-Z0-9]:%[0-9].%[0-9].%[0-9].%[0-9]",szModel,szMajor,szMinor,szPatch,szExtra );
			nMajor=atoi(szMajor );
			nMinor=atoi(szMinor);
			nPatch=atoi(szPatch);
			nExtra=atoi(szExtra);
			nVersion = MAKE_VERSION(nMajor, nMinor, nPatch, nExtra);
			DEBUGMSG( 1,( "Task.cpp : Current ver=0x%x, New ver=0x%x\n", TaviSysParam.nVersion, nVersion ) );

			if ( nMajor < 2 ) {
				// for protection of serial no, it reject to update firmwre with version below 2.0.0.0
				return nReturn;
				}
			if ( (TaviSysParam.nVersion != nVersion) && (strncmp( szModel, TaviSysParam.ProductID, 4) == 0) )
			{
				if ( pFirmUpMsgBox == NULL )
				{
					pFirmUpMsgBox = new CMsgBox( ML_FIRMWARE_UPGRADE,
						_ORANGE,
						MML_CONFIRM,
						244, 123,
						TAVI_FirmwareConfirmMsgProc,
						DL_BG_BLANK | DL_BTN_OK_CANCEL
						);
				}
				else
				{
					pFirmUpMsgBox->Reset( ML_FIRMWARE_UPGRADE,
						_ORANGE,
						MML_CONFIRM,
						244, 123,
						TAVI_FirmwareConfirmMsgProc,
						DL_BG_BLANK | DL_BTN_OK_CANCEL
						);
				}
				pFirmUpMsgBox->Show();
				nFirmUpConfirm = 2;
				
/*
				nExtra = TaviSysParam.nVersion &0xff;
				nPatch = (TaviSysParam.nVersion >> 8 ) & 0xff;
				nMinor = (TaviSysParam.nVersion >> 16 ) & 0xff;
				nMajor = (TaviSysParam.nVersion >> 24 ) & 0xff;
*/
				sprintf( szTemp,"Ver. %d.%d.%d.%d",nMajor,nMinor,nPatch,nExtra);
				str2ustr( ucSource, szTemp, strlen(szTemp) );
				pT->DrawUC16Text(ucSource, 0, 94, _ORANGE, 12, (DT_TRANSPARENCY|DT_ALIGN_CENTER) );
				pMpegDecoder->RefreshOsd();
				nReturn = 0;
			}
		}
	}

	return nReturn;
}

void _PreapareFor_StartupProgressbar(  )
{	
	pTAVI = (TTAVI_GLOBAL_CONFIG*)malloc( sizeof(TTAVI_GLOBAL_CONFIG) );
	memset( pTAVI, 0, sizeof(TTAVI_GLOBAL_CONFIG));
	pTAVI->MainPalette = NULL;
	pTAVI->SubPalette = NULL;
	pTAVI->VideoLoadingPalette = NULL;
	pTAVI->FlexablePalette = NULL;

	op_InitFramebuffer(); // Get Main framebuffer
	SysLoadParameters( &TaviSysParam );

	AllocBitmap256( _IMAGE_ICON"/battery-main.bmp", &TAVI_IMAGES[MAIN_MENU_BATTERY] );	 	

	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(MAIN_PALETTE);	
}

// Kill all tasks except main task
int TAVI_KillAllTasks( void )
{
	CNode< TTaskEntryBlock >*	pNode;
	TTaskEntryBlock* t;
	int count = 0;

	printf( "TAVI_KillAllTasks\n" );
	while( (pNode=s_TaskList.DetachHead()) ) {
		t = ( TTaskEntryBlock* )pNode->m_Data;
		KillTask( t );
		delete pNode;
		count++;
	}

	return count;
}

int TAVI_CreateTask( int ssTaskId, TTaskInitParam* tParam )
{
	int 						ssRetVal;
	TMsgEntry					tMsg;
	TTaskEntryBlock*			task	= NULL;
	CNode< TTaskEntryBlock >	*pNode	= NULL;

	int nTitleID[] = { 
		ML_NULL,
		ML_MUSIC_LIBRARY,
		ML_MOVIE,
		ML_RADIO,
		ML_PHOTO,
		ML_TEXT,
		ML_RECORD,
		ML_EXTRA,
		ML_BROWSER,
		ML_SETUP
	};

	/* if this task is not switchable, kill other tasks */
	if ( TTaskList[ssTaskId].switchable == false ) {
		while ( (pNode = s_TaskList.DetachHead()) ) {
			task = pNode->m_Data;
			KillTask( task );
			delete pNode;
		}
	}

	if ( s_TaskList.Count() == 0 ) {
		task = new TTaskEntryBlock;
		*task = TTaskList[ssTaskId];
		s_TaskList.AddHead( task );

		TAVI_CannotSleep(10);	// power save
		TTaskList[task->taskID].SleepingKey = g_HddSleep.m_nHddSleeping;	// power save
		g_HddSleep.m_nHddSleeping4Second = 0;			// power save -has no second task

	} else {
		bool	fExit = false;
		int		i;
		
		// Resume task?
		s_TaskList.Rewind();
		task = s_TaskList.Head();
		while ( task ) {
			if( task->taskID == ssTaskId ) 	break;
			task = s_TaskList.Next();
		}

		/* This task is already launched !!! */
		if( task && (TTaskList[ssTaskId].usTaskState == TASK_PAUSE) ) {
			pNode = s_TaskList.Detach( task );
			if( !pNode ) return 0;
			tParam->resume = 1;
			s_TaskList.AddNodeToHead( pNode );
			g_HddSleep.m_nHddSleeping = TTaskList[task->taskID].SleepingKey;	// power save
			if ( s_TaskList.Count() > 1 ) {
				g_HddSleep.m_nHddSleeping4Second = s_TaskList[1]->SleepingKey;	// power save
			} else {
				g_HddSleep.m_nHddSleeping4Second = 0;						// power save
			}
			TAVI_WakeUp(20);												// power save

			goto __run;
		}

		// New task is NOT running now
		if( s_TaskList.Count() == 2 ) {
			for ( i=0; i < 2; i++ ) {
				// 라디오와 뮤직은 백그라운드...
				if( s_TaskList[i]->taskID == TASK_ID_MUSIC || s_TaskList[i]->taskID == TASK_ID_RADIO ) {
					break;
				}
			}

			if ( i == 1 ) {
				pNode = s_TaskList.DetachHead();
				TTaskList[s_TaskList.Head()->taskID].SleepingKey = g_HddSleep.m_nHddSleeping4Second;	// power save
			} else {
				pNode = s_TaskList.DetachTail();
				TTaskList[s_TaskList.Head()->taskID].SleepingKey = g_HddSleep.m_nHddSleeping;		// power save
			}

			task = ( TTaskEntryBlock* )pNode->m_Data;
			KillTask( task );
			delete pNode;
		}
		
		task = s_TaskList[0];
		if ( task->switchable == FALSE ) {
			KillTask( task );
			pNode = s_TaskList.DetachHead();
			delete pNode;
		} else {
			if( ssTaskId == TASK_ID_RECORD ) {
				TAVI_KillAllTasks();
			} else {
				if ( task->taskID == ssTaskId ) {
					tParam->resume = 1;

					g_HddSleep.m_nHddSleeping = TTaskList[ssTaskId].SleepingKey;	// power save
					TAVI_WakeUp(30);											// power save

					goto __run;
				}

				switch ( task->taskID ) {

				case TASK_ID_FILEMANAGER:
					break;

				case TASK_ID_MUSIC:
					if(
						TTaskList[ssTaskId].taskID == TASK_ID_VIDEO ||
					    TTaskList[ssTaskId].taskID == TASK_ID_RADIO ||
					    TTaskList[ssTaskId].taskID == TASK_ID_RECORD
						) {
						fExit = true;
					}
					break;

				case TASK_ID_RADIO:
					if(
						TTaskList[ssTaskId].taskID == TASK_ID_VIDEO ||
					    TTaskList[ssTaskId].taskID == TASK_ID_RECORD ||
					    TTaskList[ssTaskId].taskID == TASK_ID_MUSIC
						) {
						fExit = true;
					}
					break;

				case TASK_ID_RECORD:
					if(
						TTaskList[ssTaskId].taskID == TASK_ID_VIDEO ||
					    TTaskList[ssTaskId].taskID == TASK_ID_RECORD ||
					    TTaskList[ssTaskId].taskID == TASK_ID_MUSIC
						) {
						fExit = true;
					}
					break;

				case TASK_ID_SETUP:
					break;

				case TASK_ID_TEXT:
					break;

				case TASK_ID_ETC:
					break;

				case TASK_ID_VIDEO:
					fExit = true;
					break;
				}

				if ( fExit ) {
					KillTask( task );
					pNode = s_TaskList.DetachHead();
					delete pNode;
				} else {
					g_HddSleep.m_nHddSleeping4Second = TTaskList[s_TaskList[0]->taskID].SleepingKey;		// power save
				}
			}
		}

		task = new TTaskEntryBlock;
		*task = TTaskList[ssTaskId];
		s_TaskList.AddHead( task );

		TAVI_CannotSleep(30);							// power save

	}
	
	tMsg.ulMsgId 				= MSG_CREATE;
	tMsg.ssToTaskId				= tParam->ssTaskId;
	tMsg.usParam1				= 0;
	tMsg.usParam2				= 0;	
	MSG_Send( tParam->ssTaskId, &tMsg);

	tMsg.ulMsgId 				= MSG_DRAW;
	tMsg.ssToTaskId				= tParam->ssTaskId;
	tMsg.usParam1				= 0;
	tMsg.usParam2				= 0;	
	MSG_Send( tParam->ssTaskId, &tMsg);

__run:

	// Convert ssTaskId To SlotNumber
	pTAVI->usCurrentMenu		= ssTaskId * 10;
	pTAVI->ssTaskId				= ssTaskId;

	// Clear work space
	int h						= g_TopPanel->rcABS.GetHeight();
	op_ClearAreaOSD( 0, h, 320, 240-h );

	TTaskList[ssTaskId].usTaskState = TASK_RUN;
	g_TopPanel->SetLogo( (CTopPanel::LogoType)ssTaskId );
	g_TopPanel->SetTitle( nTitleID[ssTaskId] );
	g_TopPanel->SetVisible( true );
	g_TopPanel->Update();
	ssRetVal 					= TTaskList[ssTaskId].fpInitFunc(tParam);

  	if ( ssRetVal == TaskExit_Normal ) { // TASK EXITED
		pNode = s_TaskList.DetachHead();
		if( pNode ) delete pNode;
		TTaskList[ssTaskId].usTaskState = TASK_STOP;

		g_HddSleep.m_nHddSleeping = TTaskList[TASK_ID_MAIN].SleepingKey;			// power save
		TAVI_WakeUp(50);	// power save
	} else if ( ssRetVal == TaskExit_Switch || ssRetVal == TaskExit_ExitAndSwitch ) {
		bool fswitch = true;
		TTaskList[ssTaskId].SleepingKey = g_HddSleep.m_nHddSleeping;				// power save

		if ( ssRetVal == TaskExit_ExitAndSwitch ) {
			pNode = s_TaskList.DetachHead();
			if ( pNode ) delete pNode;
			if ( s_TaskList.Count() == 0 ) {
				fswitch = false;
				g_HddSleep.m_nHddSleeping = TTaskList[TASK_ID_MAIN].SleepingKey;	// power save
			} 
			
			TTaskList[ssTaskId].usTaskState = TASK_STOP;
			g_HddSleep.m_nHddSleeping4Second = 0;								// power save -set m_nHddSleeping4Second to 0 for second task

		} else {
			if ( s_TaskList.Count() == 1 ) {
				fswitch = false;
				g_HddSleep.m_nHddSleeping4Second = g_HddSleep.m_nHddSleeping;	// power save -set m_nHddSleeping4Second to 0 for second task
				g_HddSleep.m_nHddSleeping = TTaskList[TASK_ID_MAIN].SleepingKey;	// power save
			} else {
				pNode = s_TaskList.DetachHead();
				s_TaskList.AddNodeToTail( pNode );
				task = s_TaskList[0];
				int	temp;
				temp = g_HddSleep.m_nHddSleeping;								// power save -save data
				g_HddSleep.m_nHddSleeping = g_HddSleep.m_nHddSleeping4Second;	// power save -switch register
				g_HddSleep.m_nHddSleeping4Second = temp;							// power save -set m_nHddSleeping4Second to 0 for second task
			}
			TTaskList[ssTaskId].usTaskState = TASK_PAUSE;

		}

		if ( g_KillAll ) fswitch = false;

		if ( fswitch ) {
			task = s_TaskList.Head();
			ssTaskId = task->taskID;
			tParam->ssTaskId = ssTaskId;
			tParam->resume = 1;
			
			g_HddSleep.m_nHddSleeping = TTaskList[ssTaskId].SleepingKey;		// power save
			TAVI_WakeUp(60);												// power save

			goto __run;
		}

		TAVI_WakeUp(70);													// power save

	} else {
		TTaskList[ssTaskId].usTaskState = TASK_PAUSE;

		TTaskList[ssTaskId].SleepingKey = g_HddSleep.m_nHddSleeping;			// power save

		if( g_TopPanel->GetPlayMode() == CTopPanel::PLAYMODE_STOP ) {
			g_HddSleep.m_nHddSleeping = TTaskList[TASK_ID_MAIN].SleepingKey;	// power save
		}

		TAVI_WakeUp(80);	// power save
	}


	// kill all
	if ( g_KillAll ) {
		g_KillAll = false;
		g_HddSleep.m_nHddSleeping = TTaskList[TASK_ID_MAIN].SleepingKey;		// power save
		g_HddSleep.m_nHddSleeping4Second = 0;								// power save
		TAVI_WakeUp(90);													// power save
		TAVI_KillAllTasks();
	}
	
__end:
	mep_report( MEP_REPORT_RELATIVE );

	g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
	if ( true || g_MiscCtrl->FolderClosed() ) {
		g_ExtLCD->Update();
	}
	// Restore Top Panel
	g_TopPanel->SetLogo( CTopPanel::LOGO_ID_NML );
	g_TopPanel->Update();

	// Restore touchpad properties
	mep_report( MEP_REPORT_RELATIVE );

	// Restore ext lcd menu.
	g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
	g_ExtLCD->Update();

	// Restore key properties
	extern int handle_key;
	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );

	pTAVI->usCurrentMenu = ssTaskId * 10;

	return ssRetVal;
}

int TAVI_CreateTask_For_CallByFileManager( int ssTaskId, TTaskInitParam* tParam )
{
	int			ssRetVal;
	TMsgEntry	tMsg;

#if 0
	tMsg.ulMsgId 					= MSG_CALL_BY_FILEMANAGER;
	tMsg.ssToTaskId					= tParam->ssTaskId;
	tMsg.usParam1					= 0;
	tMsg.usParam2					= 0;	
	MSG_Send( tParam->ssTaskId, &tMsg);

	tMsg.ulMsgId 					= MSG_DRAW;
	tMsg.ssToTaskId					= tParam->ssTaskId;
	tMsg.usParam1					= 0;
	tMsg.usParam2					= 0;	
	MSG_Send( tParam->ssTaskId, &tMsg);


	TTaskList[ssTaskId].usTaskState 	= TASK_RUN;
	ssRetVal 							= TTaskList[ssTaskId].fpInitFunc(tParam);
#endif

	return ssRetVal;
}

static int s_tmExtLCD;
static bool TAVI_HasPlayer( void )
{
	return (
		TTaskList[TASK_ID_MUSIC].usTaskState == TASK_PAUSE || 
		TTaskList[TASK_ID_RADIO].usTaskState == TASK_PAUSE
		) ? true : false;
}

void TAVI_BG_Volume( int direct ) 
{
	/*
		@direct: 1 is volume up, 0 volume down.
	 */
	if( TAVI_HasPlayer() ) {
		struct timeval tv;
		int vol;
		gettimeofday( &tv, NULL );

		if ( g_ExtLCD->GetCurrentMode() != CExtLCD::EXT_STAGE_VOLUME ) {
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_VOLUME );
		}

		s_tmExtLCD = tv.tv_sec;
		if( direct == 1 ) {
	    g_SndCtrl->VolumeUp();
		}
		else {
	    g_SndCtrl->VolumeDown();
		}
		vol = g_SndCtrl->GetVolume();
		
		g_ExtLCD->SetVolume( vol );
		g_ExtLCD->Update();
	}
}

void TAVI_BG_Next( void )
{
	if( TTaskList[TASK_ID_MUSIC].usTaskState == TASK_PAUSE ) {
		AP_Next();
	}
	else if( TTaskList[TASK_ID_RADIO].usTaskState == TASK_PAUSE ) {
		extern int FR_TunningUpDown( FR_TUNEDIRECTION_t nDirection );
		FR_TunningUpDown( FR_TUNEDIRECTION_UP );
	}
}

void TAVI_BG_Prev( void )
{
	if( TTaskList[TASK_ID_MUSIC].usTaskState == TASK_PAUSE ) {
		AP_Prev();
	}
	else if( TTaskList[TASK_ID_RADIO].usTaskState == TASK_PAUSE ) {
		extern int FR_TunningUpDown( FR_TUNEDIRECTION_t nDirection );
		FR_TunningUpDown( FR_TUNEDIRECTION_DOWN );
	}
}

void TAVI_BG_PlayPause( void )
{
	if( TTaskList[TASK_ID_MUSIC].usTaskState == TASK_PAUSE ) {
		int state = AP_GetPlayState();
		if( state == AUDIO_STATE_PLAYING ) AP_Pause();
		else if( state == AUDIO_STATE_PAUSE ) AP_Resume();
		op_Flush();
	}
	else if( TTaskList[TASK_ID_RADIO].usTaskState == TASK_PAUSE ) {
		FR_PlayPause();
		op_Flush();
	}
}

void TAVI_BG_VolTimeout( void )
{
	struct timeval tv;
	if( s_tmExtLCD ) {
		gettimeofday( &tv, NULL );
		if( abs(tv.tv_sec-s_tmExtLCD) > AUTO_WND_SWITCH_TIME ) {
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
			s_tmExtLCD = 0;
			if( !g_MiscCtrl->FolderOpen() ) g_ExtLCD->Update();
		}
	}
}


static void
_Set_InitParam_Task_Name_Id(TTaskInitParam *tInitParam, int MenuID)
{
	switch (MenuID) {
		case MENU_SLOT1:
			tInitParam->ssTaskId 	= TASK_ID_MUSIC;
			break;

		case MENU_SLOT2:
			tInitParam->ssTaskId 	= TASK_ID_VIDEO;
			break;

		case MENU_SLOT3:
			tInitParam->ssTaskId 	= TASK_ID_RADIO;
			break;

		case MENU_SLOT4:
			tInitParam->ssTaskId 	= TASK_ID_IMAGE;
			break;

		case MENU_SLOT5:
			tInitParam->ssTaskId 	= TASK_ID_TEXT;
			break;

		case MENU_SLOT6:
			tInitParam->ssTaskId 	= TASK_ID_RECORD;
			break;

		case MENU_SLOT7:
			tInitParam->ssTaskId 	= TASK_ID_ETC;
			break;				

		case MENU_SLOT8:
			tInitParam->ssTaskId 	= TASK_ID_FILEMANAGER;
			break;

		case MENU_SLOT9:
			tInitParam->ssTaskId 	= TASK_ID_SETUP;
			break;

		default:
			break;
	}

	pTAVI->ssTaskId = tInitParam->ssTaskId;
	tInitParam->resume = 0;
}

static int _Menu_Rotating(int direct, int curMenu)
{
	/*
		direct:
			+1: To right
			-1: To Left
	*/
	int key=-1;

	if( direct > 0 ) {	// To Right
		if( curMenu == MENU_SLOT9 )
			key = 91;	// 91 means movement from MENU_SLOT9 to MENU_SLOT1
		else
			key = curMenu + ((curMenu/10)+1) ;
				
	} else if( direct < 0 ) {
		if( curMenu == MENU_SLOT1 )
			key = 19;	// 19 means movement from MENU_SLOT1to MENU_SLOT9
		else
			key = curMenu + ((curMenu/10)-1);
	}
	return key;
}

int _DrawMainScreen(int curMenu)
{
	pLFBrowser->Hide();
	ClearOsd();
	showbackground( ID_BG_MAIN );
	TAVI_LoadPaletteTransparencyData( MAIN_PALETTE );
	DrawBoxWithoutRefresh( BG_FILTER_COLOR_PALETTE,
		0, g_TopPanel->rcABS.GetHeight(), 320, 240-g_TopPanel->rcABS.GetHeight());
	pTAVI->ssTaskId = TASK_ID_MAIN;
	g_TopPanel->SetLogo( CTopPanel::LOGO_ID_NML );
	g_TopPanel->SetVisible( true );
	g_TopPanel->Update();
	pTAVI->BatteryLevelRequests = TRUE;
	//pMpegDecoder->RefreshOsd();

	if( curMenu < MENU_SLOT1 || curMenu > MENU_SLOT9) {
		curMenu = pTAVI->usCurrentMenu = MENU_SLOT1;
	}

	TAVI_DrawMenuItem3( curMenu );

#if 1
//	showbackground( ID_BG_MAIN );
//	pMpegDecoder->RefreshOsd();
#endif
	return curMenu;
}


/* ++

    _ResetAlarm( void )

Description:
    TAVI will clear alarm settings when a battery is not enough to boot.
    After recharging a battery, Alarm is set automatically.
    _ResetAlarm() is responsible for resetting a alarm.

Return:
    None.
    
-- */
static void
_ResetAlarm( void )
{
  struct rtc_time 	rtime;
  int alarmset = 1;
  int handle_rtc;

  /*
      Read alarm register: CONTROL/STAT2
  */
	handle_rtc = open( RTC_DRIVER_PATH, O_RDWR );
	if ( handle_rtc  < 0 ) {
	  return;
	}
	else {
		ioctl( handle_rtc, RTC_ALM_READ, &rtime );

		if( rtime.tm_min&0x80 && rtime.tm_hour&0x80 ) {
			alarmset = 0;
		}

    if( !TaviSysParam.alarm.useAlarm ) {
      if( !alarmset ) {
        rtime.tm_hour = TaviSysParam.alarm.hour;
        rtime.tm_min = TaviSysParam.alarm.min;
        SysSetRTC( handle_rtc, rtime );
      }
    }
		close ( handle_rtc );
	}

  if( rtime.tm_min&0x80 && rtime.tm_hour&0x80 ) {
    alarmset = 0;
  }
}

static int 
TAVI_MsgProc( int ssTaskId, U32 ulMsgId, int usParam1, int usParam2 )
{
	TTaskInitParam	tInitParam;
	int				key;
	int				temp;
	bool			executeTask	= false;
	int				curMenu;
	static int		jog1_enter	= 0;

	SetTaviKeyMode( TK_REPEAT | TK_SINGLE );

	memset( (void*)&tInitParam, 0, sizeof(tInitParam) );

	bool reset = false;

	if( ulMsgId == MSG_BUTTON_UP || ulMsgId == MSG_BUTTON_REAL_UP || ulMsgId == MSG_TOUCHPAD ) {
		reset = true;
	}

	curMenu = pTAVI->usCurrentMenu;

	switch( ulMsgId ) {

	case MSG_FOLDER:
		if( usParam1 ) {	// Folder Open
			g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
			g_ExtLCD->Update();
		}
		break;

	case MSG_CREATE:
		break;

	case MSG_DRAW:	
		curMenu = _DrawMainScreen(curMenu);
		/* ### Alarm ### */
		if (pTAVI->isAlarmState) {
			TMsgEntry tMsgId;
			_MSG_Show_Alarm( ssTaskId, &tMsgId);
			ClearKeyBuffer();
			_DrawMainScreen(curMenu);
		}
		break;

	case MSG_TIMER:
		break;

	case MSG_SETFOCUS:
		break;

	case MSG_KILLFOCUS:
		break;

	case MSG_DESTROY:
		break;

	case MSG_BUTTON_DN:
		break;

	case MSG_TOUCHPAD: // ONLY TEST !! FIXME

		if (nFirmUpConfirm ) {
			// This is firmware update msgbox
			// It's too dangerous to use Touchpad in Firmware update dialog box!
			break;			
		}	

		if( usParam2 ) {
			/* Pressed */
			executeTask = true;
		} else {
			TAVI_DrawMenuItem3( _Menu_Rotating( usParam1, curMenu ));
		}
		break;

	case MSG_BUTTON_UP:
		if (nFirmUpConfirm) {
			pFirmUpMsgBox->MsgProc (ssTaskId, ulMsgId, usParam1, usParam2 );
			break;
		}
	
		switch (usParam1) {

		case TAVI_KEY_EXIT: 
			break;

		case TAVI_KEY_LEFT: 
			TAVI_DrawMenuItem3( _Menu_Rotating( -1, curMenu ) );
			break;
		
		case TAVI_KEY_RIGHT: 
			TAVI_DrawMenuItem3( _Menu_Rotating( +1, curMenu ) );
			break;
 
		case TAVI_KEY_JOG2_ENTER:
			if( g_MiscCtrl->FolderOpen() ) {
				TAVI_BG_PlayPause();
				break;
			}

			if( g_ExtLCD->GetCurrentMode() == CExtLCD::EXT_STAGE_READY ) {
				TAVI_BG_PlayPause();
				break;
			}

			if( g_ExtLCD->GetCurrentMode() == CExtLCD::EXT_STAGE_MENU ) {

				switch( g_ExtLCD->GetCurrentMenu() ) {

				case CExtLCD::EXT_MENU_MUSIC: 
					pTAVI->usCurrentMenu = TASK_ID_MUSIC*10;
					break;
				case CExtLCD::EXT_MENU_RADIO:
					pTAVI->usCurrentMenu = TASK_ID_RADIO*10;
					break;
				}

				curMenu = pTAVI->usCurrentMenu;
			}

			/* break ??? */
		
		case TAVI_KEY_ENTER: 
			executeTask = true;
			break;

		case TAVI_KEY_JOG2_UP:		// Next Music or Channel
		case TAVI_KEY_JOG2_DOWN:	// Previous Music or channel
			if( g_ExtLCD->GetCurrentMode() == CExtLCD::EXT_STAGE_MENU ) {
				g_ExtLCD->PrevMenu();
				g_ExtLCD->Update();
			} else {
			    if (usParam1 == TAVI_KEY_JOG2_UP) {
    				TAVI_BG_Next();
    			} else {
    				TAVI_BG_Prev();
    			}
			}
			break;

		case TAVI_KEY_SHUTDOWN: 
			return -1;
			break;

		case TAVI_KEY_JOG1_ENTER:
			if( g_MiscCtrl->FolderOpen() ) {
				break;
			}

			if( g_ExtLCD->GetCurrentMode() == CExtLCD::EXT_STAGE_READY ) {
#if 1
				if (s_EL_status_last) {
					g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_MENU );
				} else {
					// MSG_Peek() already turns on EL!
					//g_MiscCtrl->EXT_EL_Enable(true);
					;
				}
#else
				if (jog1_enter) {
					g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_MENU );
					jog1_enter = 0;
				} else {
					jog1_enter = 1;
				}
#endif
			} else {
				g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
			}

			g_ExtLCD->Update();
			break;

		case TAVI_KEY_JOG1_UP:
			TAVI_BG_Volume( 1 );
			break;

		case TAVI_KEY_JOG1_DOWN:
			TAVI_BG_Volume( 0 );
			break;

		case TAVI_KEY_UNKNOWN: 
			break;

		default:
			return FALSE;	// XXX
		}

		break;
		
	case MSG_EXECUTE_TASK:
		curMenu = pTAVI->usCurrentMenu = usParam1;
		executeTask = true;

		break;
	
	default:
		return FALSE;	// XXX

	}

	if (executeTask) {
			/* ### Voila! ### */
			_Set_InitParam_Task_Name_Id(&tInitParam, curMenu);
			TAVI_CreateTask( tInitParam.ssTaskId, &tInitParam );
			TAVI_MsgProc( ssTaskId, MSG_DRAW, 0, 0 );		
	}

	DEBUGMSG(1, ( "-MSGProc()\n" ));

	return TRUE;
}

static unsigned int taviBootTime;
static unsigned int taviShutdownTime;

int TAVI_MainTask( TTaskInitParam* tParam )
{
    extern ExitState_t  g_ExitState;
    extern bool             g_AlarmIntr;
    int              handle_rtc;

    TMsgEntry               tMsg;
    int                         alarm=0;
    int             vol_restore;
	
	/* INIT */
	TAVI_LoadBitmaps();
	InitDeviceInfo(); // Initialize devices

	
    // Avoid pop-noise.
    vol_restore = g_SndCtrl->GetVolume();
    g_SndCtrl->SetVolume( 0 );

	DrawStartupProgress(3);

	pTAVI->ssTaskId					= TASK_ID_MAIN;
	pTAVI->isTaviWorkingAsUsbHost   	= FALSE;
	pTAVI->isTaviWorkingAsUsbClient	= FALSE;
	pTAVI->BatteryLevelRequests		= TRUE;
	pTAVI->IsBatteryDebugMode		= FALSE;


	MSG_Init(NULL);
    
	// Set boot time.
	taviBootTime = (unsigned int)time( NULL );

	DrawStartupProgress(4);
																								
	pT 			= new CTextEngine_BDF( (char*)_FONT_DIR );
	pLFBrowser	= new CFileBrowser(
					"FileBrowser",
					0, 0, _SCREEN_WIDTH_, _SCREEN_HEIGHT_,
					VP_MsgProc_FileBrowser, 1
					);	

	ClearOsd();
	pMpegDecoder->RefreshOsd();
	pMpegDecoder->Resume_ColorTemperature(
			TaviSysParam.video.brightness, TaviSysParam.video.contrast, TaviSysParam.video.saturation
			);		// Set Saved Brightness, Contrast And Saturation

	showbackground( ID_BG_MAIN );

	DrawBoxWithoutRefresh( BG_FILTER_COLOR_PALETTE, 0, 0, _SCREEN_WIDTH_, _SCREEN_HEIGHT_);

	g_TopPanel->SetLogo( CTopPanel::LOGO_ID_NML );

	pTAVI->usCurrentMenu		= TaviSysParam.etc.lastmenu;
	TAVI_DrawMenuItem3(pTAVI->usCurrentMenu);
	SetTaviTouchpadMode( MEP_REPORT_RELATIVE );
	mep_set_velocity( (MEP_REL_VELOCITY ) TaviSysParam.etc.touchspeed );
	
	// 외부LCD초기화면 표시
	g_ExtLCD = new CExtLCD;
	g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
	g_ExtLCD->SetBatteryLevel( (CExtLCD::ExtLCD_Battery)CalcBatteryLevel() );
	g_ExtLCD->Update();
	
	g_PowerOff			= false;
	g_KillAll			= false;
	nCheckUpdate		= true;		// initially check for update

	TAVI_InitSleepReg();			// power save

	g_SndCtrl->LineInBypass( false );

	_ResetAlarm();

    // Mute OFF
    g_MiscCtrl->Mute( false );
    g_SndCtrl->SetVolume( vol_restore );

    g_TopPanel->SetBatteryLevel( (CTopPanel::BatteryLevel)CalcBatteryLevel() );
	
    /*
        Show alarm message when alarm interrupt is asserted.
    */
    if( g_AlarmIntr ) {
        time_t tm;
        struct tm* t;

        tm = time( NULL );
        t = localtime( &tm );
        if( t->tm_hour == TaviSysParam.alarm.hour &&
        t->tm_min == TaviSysParam.alarm.min ) {
            TMsgEntry tMsgId;
            _MSG_Show_Alarm( 0, &tMsgId);
            _DrawMainScreen( 0 );
        }
        else {
		handle_rtc = open( RTC_DRIVER_PATH, O_RDWR );
		if ( handle_rtc  < 0 ) {
			}
		else {
			ioctl( handle_rtc, RTC_CLEAR_INT );
			close ( handle_rtc );
			}
        }
        ClearKeyBuffer();
        g_AlarmIntr = false;
    }

    
	// TASK MAIN LOOP
	while ( !g_PowerOff ) {
		s_EL_status_last	= g_MiscCtrl->EXT_EL_Backlit_Status();

		MSG_Peek( tParam->ssTaskId, &tMsg );

		TAVI_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 );

		if ( nCheckUpdate ) {
			/* Only once after fresh boot or USB disconnection ! */
			TAVI_CheckFirmware();
			nCheckUpdate = false;
		}

    // For removing a pop-noise!!!
    if( AP_GetPlaystate() == AUDIO_STATE_PLAYING ) {
        pMpegDecoder->Refresh();
    }
		usleep( 10000 );
	}

	// Mute
	int restore_vol = g_SndCtrl->GetVolume();
	g_SndCtrl->SetVolume( 0 );

#ifdef HW0601
	g_MiscCtrl->HostEnable(FALSE);					// host power off
#endif	

	TAVI_KillAllTasks();
	
	// show shutdown icon in the ext. lcd

	SubLCD_ShowShutDown();
	SubLCD_Flush( 0 );
	TAVI_WakeUp(99);		// power save
	
	TaviSysParam.sound.volume = restore_vol;
	TaviSysParam.etc.lastmenu = pTAVI->usCurrentMenu;
	SysSaveParameters(&TaviSysParam);
	
	op_ClearAreaOSD( 0, 0, _SCREEN_WIDTH_, _SCREEN_HEIGHT_ );

	if ( g_ExitState == ExitState_Shutdown ) {
		showbackground( _IMAGE_BG"/shutdown.jpg" );
	} else {
		showbackground( _IMAGE_DIR"/bg/batterylow.jpg" );
		pT->DrawResText( ML_LOW_BETTRY, 0, 180, _WHITE, 16, DT_TRANSPARENCY | DT_ALIGN_CENTER );
	}

	if( toggle_display == 0 ) {
		g_MiscCtrl->MainLCDOnOff(true);	// In The "VideoPlayer" Exit Process, MainLCD Off Action Activated.
		usleep(100);
	}

	pMpegDecoder->RefreshOsd();

	// Set shutdown time
	taviShutdownTime = ( unsigned int )time( NULL );

	// Save running time to a file.
	RMuint8 path[] = "/hd1/debug_running_time.txt";
	RMuint32 fd;
	FATFS_ERROR err;
	err = fatfs_fopen( path, _O_WRONLY | _O_TRUNC, &fd );
	if( err == FATFS_ERROR_NO_ERROR ) {
		char buf[1024];
		unsigned int diff;
		int len;
		diff = taviShutdownTime - taviBootTime;
		diff /= 60; // to minute
		len = sprintf( buf, "DURATION: %d hour %d min\r\n", diff/60, diff%60 );
		fatfs_fwrite( fd, (RMuint8*)buf, len, NULL );
		fatfs_fclose( fd );
	}

	// Unmount HDD
	fatfs_umount( 0 );

	for ( int i=0;i<20;i++) { usleep(1000000); }

	// fade out effect
	for ( int i=0;i<256;i+=4) { pMpegDecoder->SetAlpha(0, i); }
		
	g_MiscCtrl->MainLCDOnOff( FALSE );
	g_MiscCtrl->PowerLEDOnOff( FALSE );
	g_SndCtrl->DACMute( true );
	g_SndCtrl->PowerOff();

	// Clear Ext. LCD buffer and power-off
	SubLCD_Exit();
	lcd_disable();

	// Power-Off
	if (g_MiscCtrl->ACDetected() ) {
		SET_SYSTEMCONTROLREG(0x80000000);		// res_all
	} else {
		g_MiscCtrl->PowerOff();
	}
	return TRUE;
}

/* END */

