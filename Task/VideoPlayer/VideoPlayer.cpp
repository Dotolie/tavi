//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : VideoPlayer Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/VideoPlayer/VideoPlayer.cpp,v $
// $Revision: 1.28 $
// $Date: 2006/06/23 09:23:15 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: VideoPlayer.cpp,v $
// Revision 1.28  2006/06/23 09:23:15  zstein
// - fixed multi-language caption bug.
//
// Revision 1.27  2006/06/23 08:19:18  zstein
// - fixed buffer-overflow bug.
//
// Revision 1.26  2006/06/22 02:25:53  zstein
// - fixed bug.
//
// Revision 1.25  2006/06/22 01:45:52  zstein
// - supported multi-language caption.
// - fixed 'wide-optimised' bug.
//
// Revision 1.24  2006/06/21 10:40:40  ywkim
// Add instruction of _SoundPalette_RealApply() in VP_PrepareAndPlay() for EQ
//
// Revision 1.23  2006/06/12 02:37:23  zstein
// - removed unused codes.
//
// Revision 1.22  2006/06/09 05:52:24  etnlwind
// -
//
// Revision 1.21  2006/06/07 07:59:35  zstein
// - added Wide(16x9) modes.
//
// Revision 1.20  2006/05/23 10:09:35  zstein
// - mpeg decoder stop when exit
//
// Revision 1.19  2006/05/13 05:30:15  zstein
// - stop mpeg decoder when videoplayer exit.
//
// Revision 1.18  2006/05/10 02:31:38  zstein
// - removed unused functions and fixed the mismatched time-progress bar bug.
//
// Revision 1.17  2006/04/19 04:04:27  zstein
// - fixed memory leak.
//
// Revision 1.16  2006/04/19 02:31:37  zstein
// - To make EQ disabled on buffering mode.
//
// Revision 1.15  2006/04/18 06:04:20  zstein
// - fixed touchpad bug.
//
// Revision 1.14  2006/04/18 04:55:04  zstein
// - show message box on adding bookmark.
//
// Revision 1.13  2006/04/17 07:16:49  zstein
// - fixed auto-entering bug.
//
// Revision 1.12  2006/04/15 03:05:55  ywkim
// Include extern variable of nIdle_stoptimer, nIdle_lcdofftimer for stopofftimer in VP_SetPlayStatus()
//
// Revision 1.11  2006/04/15 01:21:43  zstein
// - SMIL extension.
//
// Revision 1.10  2006/04/07 04:35:35  zstein
// - video buffering.
//
// Revision 1.9  2006/04/05 01:51:02  zstein
// - updated displaying caption routine.
//
// Revision 1.6  2006/03/17 07:07:00  etnlwind
// Add return code when pSubtitle is null in VP_ResyncSMI
//
// Revision 1.5  2006/03/10 06:44:27  ywkim
// Active MSG_Peek_Ex with MSG_PEEK_ALL in VP_DialogBox_MessagePeeking()
//
// Revision 1.4  2006/03/09 03:49:38  etnlwind
// Active MSG_Peek_Ex with MSG_PEEK_ALL
//
// Revision 1.3  2006/03/09 02:08:52  zstein
// - removed unused functions and improved volume interface.
//
// Revision 1.2  2006/03/07 04:57:04  zstein
// - changed to the original volume level.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*****************************************************************************

#include "VideoPlayer.h"
 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>	// memmove/memcpy
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mount.h>

#include <jpeg/jpeg.h>
#include <mp3/mp3.h>
#include <avi/avi.h>
#include <ac3/ac3.h>
#include <mp4/mp4.h>
#include <aac/aac.h>

#include <mpgparse/mpgparse.h>
#include <Lib/LibDefines.h>
#include <Lib/LibImage.h>
#include <Lib/LibFileBrowser.h>
#include <Lib/LibBuzzer.h>
#include <Lib/LibSubtitle.h>
#include <Lib/LibSubMenu.h>

#include <Task/tavi_global.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <util.h>

#include <scanner.h>
#include <3Dsound/3Dsound.h>
#include <jpeg/jpeg.h>
#include "../fs_fat.h"
#include "../../lcd/lcd.h"
#include "mfileio.h"

#define _USABLE_AREA_Y 		30
#define _TITLE_START_Y		_USABLE_AREA_Y + 2
#define _MENUITEM_START_Y	_TITLE_START_Y + 23
#define IsControlPannelVisible()	pMpegDecoder->GetPixelTransparent( 10, 240 - 43 )
#define _MAX_MOVIE_PLAYTIME_IN_SEC		32768

#define SY_ON_SUBTITLEDBX 		(35-1+120 + 5)
#define SY_ON_CONTROLPANNEL 	(_MENUITEM_START_Y)

//#define dbg	printf
#define dbg


extern TTAVI_GLOBAL_CONFIG* 	pTAVI;
extern BITMAPFILEINFO256 		TAVI_IMAGES[];
extern SysParameter_t			TaviSysParam;
extern FileBrowser* 			pFileBrowser;
extern CTopPanel*				g_TopPanel;


extern TAVI_TIME playtime, totaltime;
extern VIDEOFILE_PLAYINFO gVideofilePlayinfo;

extern CTextEngine_BDF* pT;
extern RMint16 selected_audio_stream_format;

extern int IsXvidGmcEnabled;
extern RMuint32 MAIN_RenderVideoFile (uchar_t* filename, int type, void *context);
extern RMuint32 fb_isrenderdone (void *context);
extern RMuint32 fb_renderstop (void *context);
extern int TAVI_LoadPaletteTransparencyData( int PaletteTransparencyType );
extern int Seek7(int t);
extern void SetPlayTime();
extern void showbackground( int nMenuID );

static uchar_t FullPath[260];
static RMuint32 t0, t1;
static RMint64 currentTime;
static RMint64 t;

keystruct tempkey;

TVIDEOPLAYER_CONFIG*			pVideoPlayer;
VIDEOPLAYER_CALLBACK_TABLE		cbtVideo;
static VIDEO_RENDERER			fpVideoRenderer;
static VIDEO_RENDERSTOPPER		fpVideoRenderStopper;

static RMint64 stc;

static RMint8 	filename[260];
static RMint64 		SeekPosition_SecOnly;
static RMuint16	usSeekPoint_AccValue, usSeekPoint_ProgressCounter;

static const uchar_t sSlash[2] = {0x002f, 0x0000};	
static uchar_t sSavefileDir[FATFS_MAX_PATHNAME_LENGTH];
static uchar_t sSavefilename[FATFS_MAX_PATHNAME_LENGTH];
static char cTemp[FATFS_MAX_PATHNAME_LENGTH];
static uchar_t ucTemp[FATFS_MAX_PATHNAME_LENGTH];
static char aCurrentFullPathDir[FATFS_MAX_PATHNAME_LENGTH];

static CPixmap* _pmCaption[MAX_CAPTION_BUFFER];

int isCaptionEnabled;

CControlPannel* 			pCp;
CBookmarkDialogBox* 		pBookmarkDbx;
CSetupDialogBox* 			pSetupDbx;
CSubtitle*				pSubtitle;
CFileDbx*				pLoadSmiDbx;
CLibSubtitleSyncDbx* 		pSubtitleSyncDbx;
CSoundPaletteDbx* 		pSndPltDbx;
CPopUpSubMenu* 			pPopUpSub_Sort;
extern CPopUpMenu* pmFileBrowser;
CPopUpMenu*	pmVideoPlay;
CPopUpMenu*	pVideoMenu;
CPopUpSubMenu* psBookmark;
CPopUpSubMenu* psSubtitleEnabled;
CPopUpSubMenu* psSubtitleDisabled;
CPopUpSubMenu* psSubtitleNotFound;
CPopUpSubMenu* psPlayBoundary;
CPopUpSubMenu* psRepeat;
CPopUpSubMenu* psVideoMode;
CPopUpSubMenu* psVideoOutput;
CPopUpSubMenu* psInfoDisplay;

extern CFileBrowser*		pLFBrowser;
extern CSndCtrl* 			g_SndCtrl;
static CMsgBox*			pMbx;

#define USE_PROGRESS_PALETTE	1
#define TOP_FRAME			0x0000ffff

extern int handle_key;

void VP_DrawFrame( U32 flags, int fRefresh )
{
	if( (flags&TOP_FRAME) )
		{
		g_TopPanel->SetTitle( (char*)pLFBrowser->Filename, true );
		g_TopPanel->SetVisible(true);
		g_TopPanel->Update();
		}

	if(fRefresh)
		pMpegDecoder->RefreshOsd();
 }

int VP_IsStatusbarVisible()
{
	if( IsControlPannelVisible() && pCp->isVisible == TRUE ) { pVideoPlayer->isControlPannelVisible = TRUE; return TRUE; }
	else { pVideoPlayer->isControlPannelVisible = FALSE; return FALSE; }
}



int _SoundPalette_RealApply( RMuint32 option )
{// Do not use timer !!
	SOUNDEFFECT_t output;
	CMD_t CMD;
	CMD_t CurrentStatus;
	int* surround;
	int* bass;

#if 1
	if( TaviSysParam.sound.eq == SOUND_eXTX_PRESET  && (!g_TopPanel->GetSpeakerOnOff()) )
		output = (SOUNDEFFECT_t)SOUND_REX_WIDE_PRESET;
	else
		output = (SOUNDEFFECT_t)TaviSysParam.sound.eq;
#else
	output = (SOUNDEFFECT_t)TaviSysParam.sound.eq;
#endif


	if( output == SOUND_MEX_USER
		|| output == SOUND_REX_WIDE_USER
		|| output == SOUND_REX_LIVE_USER )
		goto _2BAND_USER_EQ_PROCESS;

	else if( output == SOUND_SEQ_USER )
		goto _5BAND_USER_EQ_PROCESS;
	
_PRESET_EQ_PROCESS:
	DEBUGMSG(1,("-mode:_PRESET_EQ_PROCESS\n"));
	XEN_Preset( output );
	return TRUE;

_2BAND_USER_EQ_PROCESS:
	DEBUGMSG(1,("-mode:_2BAND_USER_EQ_PROCESS\n"));
	XEN_Preset( output );

	if( output == SOUND_MEX_USER )
		{
		surround = &TaviSysParam.sound.surround_mex;
		bass = &TaviSysParam.sound.bass_mex;
		}
	else if( output == SOUND_REX_WIDE_USER )
		{
		surround = &TaviSysParam.sound.surround_rex_wide;
		bass = &TaviSysParam.sound.bass_rex_wide;

		}
	else if( output == SOUND_REX_LIVE_USER )
		{
		surround = &TaviSysParam.sound.surround_rex_live;
		bass = &TaviSysParam.sound.bass_rex_live;
		}
	else
		{
		ASSERT(0);
		}

	DEBUGMSG(1,("-surround : [%d]\n", *(surround) ));
	CMD.nMode = SET_3DPARAMETER;
	CMD.Param.PARAMETER.TypeNo = XEN_PTYPE_SURR;
       CMD.Param.PARAMETER.Value = (ePARAM_VALUE)*(surround);
	XEN_SetEffect (CMD);
	
	DEBUGMSG(1,("-bass : [%d]\n", *(bass) ));
	CMD.nMode = SET_3DPARAMETER;
	CMD.Param.PARAMETER.TypeNo = XEN_PTYPE_BASS;
	CMD.Param.PARAMETER.Value = (ePARAM_VALUE)*(bass);
	XEN_SetEffect (CMD);	

	return TRUE;
	
_5BAND_USER_EQ_PROCESS:
	DEBUGMSG(1,("-mode:_5BAND_USER_EQ_PROCESS\n"));
	XEN_Preset( output );

	if( XEN_Get_UserEQ_BandValue( EQ_PTYPE_BAND_0 ) != (int)TaviSysParam.sound.band0 )
		{
		CMD.nMode = SET_EQPARAMETER;
		CMD.Param.PARAMETER.TypeNo = EQ_PTYPE_BAND_0;
		CMD.Param.PARAMETER.Value = (ePARAM_VALUE)(TaviSysParam.sound.band0);
		XEN_SetEffect (CMD);
		}

	if( XEN_Get_UserEQ_BandValue( EQ_PTYPE_BAND_1 ) != (int)TaviSysParam.sound.band1 )
		{
		CMD.nMode = SET_EQPARAMETER;
		CMD.Param.PARAMETER.TypeNo = EQ_PTYPE_BAND_1;
		CMD.Param.PARAMETER.Value = (ePARAM_VALUE)(TaviSysParam.sound.band1);
		XEN_SetEffect (CMD);
		}

	if( XEN_Get_UserEQ_BandValue( EQ_PTYPE_BAND_2 ) != (int)TaviSysParam.sound.band2 )
		{
		CMD.nMode = SET_EQPARAMETER;
		CMD.Param.PARAMETER.TypeNo = EQ_PTYPE_BAND_2;
		CMD.Param.PARAMETER.Value = (ePARAM_VALUE)(TaviSysParam.sound.band2);
		XEN_SetEffect (CMD);
		}

	if( XEN_Get_UserEQ_BandValue( EQ_PTYPE_BAND_3 ) != (int)TaviSysParam.sound.band3 )
		{
		CMD.nMode = SET_EQPARAMETER;
		CMD.Param.PARAMETER.TypeNo = EQ_PTYPE_BAND_3;
		CMD.Param.PARAMETER.Value = (ePARAM_VALUE)(TaviSysParam.sound.band3);
		XEN_SetEffect (CMD);
		}

	if( XEN_Get_UserEQ_BandValue( EQ_PTYPE_BAND_4 ) != (int)TaviSysParam.sound.band4 )
		{
		CMD.nMode = SET_EQPARAMETER;
		CMD.Param.PARAMETER.TypeNo = EQ_PTYPE_BAND_4;
		CMD.Param.PARAMETER.Value = (ePARAM_VALUE)(TaviSysParam.sound.band4);
		XEN_SetEffect (CMD);
		}

	return TRUE;
}

static int VP_SetSoundPalette( int* parameters )
{
	Reset_SoundPaletteCtrlHideTimer();

	if( parameters != NULL && *(parameters) == 1 ) {
		Reset_SoundPaletteTouchApplyTimer();
	}
	else {
		_SoundPalette_RealApply(0);
	}

	return TRUE;

}

static void VP_BuildSavePathname( uchar_t* BookmarkDir )
{
	memset( ucTemp, 0, FATFS_MAX_PATHNAME_LENGTH*sizeof(uchar_t) );
	memset( BookmarkDir, 0, FATFS_MAX_PATHNAME_LENGTH*sizeof(uchar_t) );
	ustrcpy( BookmarkDir, pLFBrowser->CurrentFullPathDir );
	sprintf( cTemp, "/%s", _VIDEOPLAYER_BOOKMARK_DATA );
	str2ustr( ucTemp, cTemp );
	ustrcat( BookmarkDir, ucTemp );
}
static void VP_BuildSaveFilename( uchar_t* Filename, int isBookmark )
{
	RMuint32 crc = crc32( (char*)Filename, ustrlen(Filename)*2 );
	memset(cTemp, 0, FATFS_MAX_PATHNAME_LENGTH*sizeof(char));
	memset( ucTemp, 0, FATFS_MAX_PATHNAME_LENGTH*sizeof(uchar_t) );
	memset( sSavefilename, 0, FATFS_MAX_PATHNAME_LENGTH*sizeof(uchar_t) );
	
	if(isBookmark)
		sprintf( cTemp, "%08x.b", crc );
	else
		sprintf( cTemp, "%08x.r", crc );

	str2ustr( ucTemp, cTemp );
	ustrcpy( sSavefilename, sSavefileDir);
	adduchar( sSavefilename, 0x002f );
	ustrcat( sSavefilename, ucTemp );
}
static FATFS_ERROR VP_ReadSaveData( U32* pulSaveData, uchar_t* Savefile_FullPath )
{
	RMuint32 rc;	// read counter
	RMuint32 fd;	// file descriptor
	FATFS_ERROR err = FATFS_ERROR_NO_ERROR;

	DEBUGMSG(1,("VP_ReadSaveData()"));
	
	if( (err = fatfs_wfopen( (RMuint16*)Savefile_FullPath, _O_RDONLY, &fd )) == FATFS_ERROR_NO_ERROR)
		{
		if( (err = fatfs_fread( fd, (RMuint8*)pulSaveData, (sizeof(U32)*MAXIMUM_BOOKMARK_ALLOW), &rc )) == FATFS_ERROR_NO_ERROR )
			{
			DEBUGMSG(1,("-read [%d] bytes ... pass. \n", rc ));
			fatfs_fclose(fd);
			}
		}
	
	return err;
}

static FATFS_ERROR VP_SaveData( U32* pulSaveData, U32 ulFilepositionInSeconds, int slot, uchar_t* Savefile_FullPath, RMuint32 SAVE_FLAGS )
{
	FATFS_ERROR err;
	RMuint32 rc, wc, fd;
	U32 ulTempTime = ulFilepositionInSeconds;
	int slotindex;

	DEBUGMSG(1,("VP_SaveData()"));

	if( slot < 0 )  // Auto Detect
		{
		for( slotindex = 0 ; slotindex<MAXIMUM_BOOKMARK_ALLOW-1; slotindex++)
			{
			if( pulSaveData[slotindex] == 0 )
				{
				pulSaveData[slotindex] = ulTempTime;
				break;
				}
			}
		pulSaveData[slotindex] = ulTempTime;      // slotindex == MAXIMUM_BOOKMARK_ALLOW-1
		}
	else
		{
	        pulSaveData[slot] = ulTempTime;
		}	

	// Save Bookmark Data
	err =  FATFS_ERROR_NO_ERROR;
	if( (err = fatfs_wfopen( (RMuint16*)sSavefilename, SAVE_FLAGS, &fd )) >0 )
		{
		printf("-Failed to open as Write Mode ... err[%x]\n", err );
		DEBUG_U16MSG(sSavefilename, "-sSavefilename");
		fatfs_fclose( fd );
		return err;
		}
	else
		{
		err =  FATFS_ERROR_NO_ERROR;
//		if( (err=fatfs_fwrite( fd, (RMuint8*)pulSaveData, (sizeof(U32)*MAXIMUM_BOOKMARK_ALLOW), &wc )) == 0 )
		if( (err=fatfs_fwrite( fd, (RMuint8*)pulSaveData, MINIMUM_FILESIZE, &wc )) == 0 )
			{
			printf("(Bookmark) Write ... pass.\n");
			}
		else
			{
			printf("(Bookmark) Write ... failed. err=%x\n", err );
			}		
		fatfs_fclose( fd );
		return err;
		}
}
static FATFS_ERROR VP_CreateSaveDir( uchar_t* SaveDirnameOnly )
{
	RMuint32 dd;
	FATFS_ERROR err = FATFS_ERROR_NO_ERROR;
	DEBUGMSG(1,("VP_CreateSaveDir()"));

	if( (err = fatfs_wdopen((RMuint16*)pLFBrowser->CurrentFullPathDir, &dd )) > FATFS_ERROR_NO_ERROR ) 	
		{
		DEBUGMSG(1,("-fatfs_wdopen:FALSE, err=0x%x\n", err));
		VP_ShowException( err );
		fatfs_dclose(dd);
		return err; 
		}
	else 
		{
		DEBUGMSG(1,("-fatfs_wdopen:TRUE, err=0x%x\n", err));

		if( (err = fatfs_wdcreate_dir( dd, (RMuint16*)SaveDirnameOnly )) > FATFS_ERROR_NO_ERROR ) 	
			{
			DEBUG_U16MSG( SaveDirnameOnly, "-SaveDirnameOnly");
			if( err == FATFS_ERROR_NAME_ALREADY_DOES_EXIST )
				{
				fatfs_dclose(dd);
				return FATFS_ERROR_NO_ERROR;
				}
			
			DEBUGMSG(1,("-fatfs_wdcreate_dir:FALSE, err=0x%x\n", err));
			VP_ShowException( err );
			DEBUGMSG(1,("-Unable to create SaveDir : err=0x%x\n",err ));
			fatfs_dclose(dd);
			return err; 
			}
		DEBUGMSG(1,("-fatfs_wdcreate_dir:TRUE, err=0x%x\n", err));
		fatfs_dclose(dd);
		return err;

		}
}
//V3
static int VP_Bookmarking( uchar_t sPathname[], uchar_t sFilename[], int iFilepositionInSeconds, int slot )
{
	U32 ulSaveData[ MAXIMUM_BOOKMARK_ALLOW ];
	RMuint32 fd;
	RMuint32 rc, wc;
	FATFS_ERROR err;
	RMuint32 SAVE_FLAGS;
	
	U32 ulTempTime = (U32) iFilepositionInSeconds;

    mio_wakeup();

	memset( ulSaveData, 0, sizeof(U32)*MAXIMUM_BOOKMARK_ALLOW );
	VP_BuildSavePathname( sSavefileDir);
	VP_BuildSaveFilename( sFilename, TRUE );

	if( VP_ReadSaveData( ulSaveData, sSavefilename ) == FATFS_ERROR_NO_ERROR ) {
		SAVE_FLAGS = (_O_WRONLY|_O_TRUNC);
	}
	else {
		VP_CreateSaveDir(sFilename);
		SAVE_FLAGS = (_O_WRONLY|_O_CREAT);
	}

	VP_SaveData( ulSaveData, (U32)iFilepositionInSeconds, slot, sSavefilename, SAVE_FLAGS );


	return TRUE;
}

static int  VP_BookmarkLoading( uchar_t sPathname[], uchar_t sFilename[] )// V3
{
	U32 ulSaveData[ MAXIMUM_BOOKMARK_ALLOW ];
	RMuint32 fd;
	RMuint32 rc, wc;
	FATFS_ERROR err;

	U32 ulTempTime;

	memset( ulSaveData, 0, sizeof(U32)*MAXIMUM_BOOKMARK_ALLOW );

	VP_BuildSavePathname( sSavefileDir);
	VP_BuildSaveFilename( sFilename, TRUE );

	// Check Bookmark Data Aleady Exist
	if( (err=fatfs_wfopen( (RMuint16*)sSavefilename, _O_RDONLY, &fd )) == FATFS_ERROR_NO_ERROR )
		{
		if( (err = fatfs_fread( fd, (RMuint8 *)ulSaveData, (sizeof(U32)*MAXIMUM_BOOKMARK_ALLOW), &rc )) > FATFS_ERROR_NO_ERROR )
			{
			// Bookmark File Read Exception
			//VP_ShowException( err );
			DEBUGMSG(1,("-Bookmark File Read Exception. : err=0x%xh\n", err ));
			if(fd)fatfs_fclose( fd );
			return FALSE;
			}
		// Bookmark File Read Proceed
		DEBUGMSG(1,("-Bookmark File Read Proceed. : err=0x%xh\n", err ));
		if(fd)fatfs_fclose( fd );	
		}
	else
		{
		// Bookmark File Not Found
		//VP_ShowException( err );
		DEBUGMSG(1,("-Bookmark File Not Found : err=0x%xh\n", err ));
		if(fd)fatfs_fclose( fd );
		return FALSE;	
		}

	showdata_U32( ulSaveData );

	for( int index=0; index<MAXIMUM_BOOKMARK_ALLOW; index++)
		{
		if( ulSaveData[index] > 0 )
			{
			tBookmarkNode* pNode = new tBookmarkNode;
			pNode->ulSavetime =  ulSaveData[index] ;
			pBookmarkDbx->AddBookmark( pNode );
			DEBUGMSG(1,(" - ulSaveData[%d] : [%d]\n", index, pNode->ulSavetime ));
			}
		}

	DEBUGMSG(1,("-VP_BookmarkLoading()\n"));
	if(fd)fatfs_fclose( fd );
	return TRUE;
	
}
int VP_DeleteOneBookmark( uchar_t sPathname[], uchar_t sFilename[], int BeDeletedSlot )
{
	U32 ulNewSaveData[ MAXIMUM_BOOKMARK_ALLOW ];
	U32 ulSaveData[ MAXIMUM_BOOKMARK_ALLOW ];
	RMuint32 fd;
	RMuint32 rc, wc;
	FATFS_ERROR err;

	memset( ulNewSaveData, 0, sizeof(U32)*MAXIMUM_BOOKMARK_ALLOW );
	memset( ulSaveData, 0, sizeof(U32)*MAXIMUM_BOOKMARK_ALLOW );

	VP_BuildSavePathname( sSavefileDir);
	VP_BuildSaveFilename( sFilename, TRUE );


	// Check Bookmark Data Aleady Exist
	if( (err = fatfs_wfopen( (RMuint16*)sSavefilename, _O_RDONLY, &fd )) == 0 )
		{
		// Exist
		if( (err = fatfs_fread( fd, (RMuint8 *)ulSaveData, (sizeof(U32)*MAXIMUM_BOOKMARK_ALLOW), &rc )) > FATFS_ERROR_NO_ERROR )
			{
			VP_ShowException( err );
			if(fd) fatfs_fclose(fd);
			return FALSE;
			}
		if(fd) fatfs_fclose(fd);
		}
	else
		{
		// Bookmark Not Found !!!!
		if(fd) fatfs_fclose(fd);
		return FALSE;
		}

	if( BeDeletedSlot > (MAXIMUM_BOOKMARK_ALLOW-1) )
		{
		ASSERT(0);
		return FALSE;
		}

	for( int slotindex=0; slotindex<MAXIMUM_BOOKMARK_ALLOW-1; slotindex++)
		{
		if( slotindex < BeDeletedSlot )
			ulNewSaveData[slotindex] = ulSaveData[slotindex];
		else
			ulNewSaveData[slotindex] = ulSaveData[slotindex+1];
		
		}

	showdata_U32( ulNewSaveData );
	
	// Save Bookmark Data
	if( (err = fatfs_wfopen( (RMuint16*)sSavefilename, _O_WRONLY|_O_CREAT, &fd )) > FATFS_ERROR_NO_ERROR)
		{
		VP_ShowException( err );
		if(fd) fatfs_fclose(fd);
		return FALSE;
		}
	

	err = fatfs_fwrite( fd, (RMuint8*)ulNewSaveData, MINIMUM_FILESIZE, &wc );
	ASSERT(!err);
	fatfs_fclose( fd );

	pBookmarkDbx->ClearItems();

	for( int index=0; index<MAXIMUM_BOOKMARK_ALLOW; index++)
		{
		if( ulNewSaveData[index] > 0 )
			{
			tBookmarkNode* pNode = new tBookmarkNode;
			pNode->ulSavetime =  ulNewSaveData[index] ;
			pBookmarkDbx->AddBookmark( pNode );
			}
		}
		
	
	return TRUE;
}

U32 VP_CheckLastResumeFile( uchar_t sFullPathDir[], uchar_t sFilename[] )
{
	RMuint32 fd;
	FATFS_ERROR err;
	RMuint32 ret;
	U32 u32FilepositionInSeconds;	// 4bytes

	VP_BuildSavePathname( sSavefileDir);
	VP_BuildSaveFilename( sFilename, FALSE );

	if( (err = fatfs_wfopen( (RMuint16*)sSavefilename, _O_RDONLY, &fd )) == FATFS_ERROR_NO_ERROR)
		{
		u32FilepositionInSeconds = 0;
		if( (err=fatfs_fread( fd, (RMuint8*)&u32FilepositionInSeconds, sizeof(U32), &ret )) == FATFS_ERROR_NO_ERROR )
			{
			// Is "u32FilepositionInSeconds"  valid value?
			if( u32FilepositionInSeconds > 0 && u32FilepositionInSeconds < _MAX_MOVIE_PLAYTIME_IN_SEC )
				{
				// Resume Process
				if(fd) fatfs_fclose(fd);	
				return u32FilepositionInSeconds;
				}
			}
		else
			{
			// Unknown Error, Exception.
			//VP_ShowException( err );
			if(fd) fatfs_fclose(fd);
			return FALSE;
			}
			
		}
	else
		{
		// Resume File Not Found
		if(fd) fatfs_fclose(fd);
		return FALSE;
		}

}


//V2
static int 
VP_SaveCurrentPosition( 
  uchar_t sPathname[], 
  uchar_t sFilename[], 
  U16 usFilepositionInSeconds )
{
	RMuint32 fd;
	RMuint32 nw;
	FATFS_ERROR err;
	U32 u32FilepositionInSeconds;
	RMuint32 savetime = GetMSTime();

	u32FilepositionInSeconds = (U32) usFilepositionInSeconds;

	VP_BuildSavePathname( sSavefileDir);
	VP_BuildSaveFilename( sFilename, FALSE );

	if( fatfs_wfopen(  (RMuint16*)sSavefileDir, _O_RDONLY, &fd ) == FATFS_ERROR_NO_ERROR )
		{
		// _db file exists
		fatfs_fclose(fd);
		fatfs_wdelete_file( (RMuint16*)sSavefileDir );
		}

	if( (err = fatfs_wfopen( (RMuint16*)sSavefilename, _O_WRONLY|_O_TRUNC, &fd )) == FATFS_ERROR_NO_ERROR )
		{
		if( (err= fatfs_fwrite( fd, (RMuint8*)&u32FilepositionInSeconds, MINIMUM_FILESIZE, &nw )) == FATFS_ERROR_NO_ERROR )
			{
			//Savefile Exists. Overwrite.
			if(fd)fatfs_fclose(fd);
			return TRUE;
			}
		// Write Failed.
		// Unknown Error, Exception.

		if(fd)fatfs_fclose(fd);
		return FALSE;
		}
	else
		{
		//Savefile Not Exists. Create.
		if( (err=fatfs_wfopen((RMuint16*)sSavefilename, _O_WRONLY|_O_CREAT, &fd )) == FATFS_ERROR_NO_ERROR )
			{
			if( (err= fatfs_fwrite( fd, (RMuint8*)&u32FilepositionInSeconds, MINIMUM_FILESIZE, &nw )) == FATFS_ERROR_NO_ERROR )
				{
				//Savefile Not Exists. Create.
				if(fd)fatfs_fclose(fd);
				return TRUE;
				}
			}
		// Write Failed.
		// Unknown Error, Exception.
		if(fd)fatfs_fclose(fd);
		return FALSE;
		}

  return TRUE;
}


static void _LoadCaptionPalette( void )
{
  /*
  Set palette
  */
  int ncolors = pSubtitle->GetColors();
  OSDBuffer osd = pMpegDecoder->GetOSDFrameBuffer();
  int* palette = ( int* )(osd.framebuffer+8);
  int start = 239;
  int color;
  char* p1 = (char*)&color;
  char* p2;

  for( int i=0; i<ncolors && i<16; i++ ) {
    pSubtitle->GetColor( i, color );
    p2 = ( char* )&palette[i+start];
    p2[0] = 0xff; // alpha, opaque
    p2[1] = rgb2y( p1[2], p1[1], p1[0] );
    p2[2] = rgb2u( p1[2], p1[1], p1[0] );
    p2[3] = rgb2v( p1[2], p1[1], p1[0] );
  }
}

static int VP_PrepareSMI( uchar_t* filename, U32 option = 0 ) 
{
    RMuint32 fd;
    FATFS_ERROR err;
    int success = 1;
    int len;
    int dotpos=0;
    uchar_t path[MAX_PATH];

    if( option ) { // Has a file extension
        err = fatfs_wfopen( (RMuint16*)filename, _O_RDONLY, &fd );
        if( err ) {
            success = 0;
        }
    }
    else {
        ustrcpy( path, FullPath );
        len = ustrlen( path );
        for( int i=len; i>=0; i-- ) {
            if( path[i] == '.' ) {
                dotpos = i;
                break;
            }
        }

        if( dotpos ) {
            // *.smi
            dotpos++;
            ustrcpy( &path[dotpos], (uchar_t*)"s\0m\0i\0\0\0" );
            err = fatfs_wfopen( path, _O_RDONLY, &fd );
            if( err ) {
                // *.smil
                ustrcpy( &path[dotpos], (uchar_t*)"s\0m\0i\0l\0\0\0" );
                err = fatfs_wfopen( path, _O_RDONLY, &fd );
                if( err ) {
                    success = 0;
                }
            }
        }
    }

  RMuint8 Option;
  if( success ) {
    fatfs_fclose( fd );

    pSubtitle = new CSubtitle;

  	if( option == 0 ) {
	    pSubtitle->LoadSMI( path );
    }
  	else {
	    pSubtitle->LoadSMI( filename );
    }

		Option = SKIP_OSD_REFRESH;
		VP_SubtitleEnabled( &Option );

    // set palette.
    _LoadCaptionPalette();

    for( int i=0; i<MAX_CAPTION_BUFFER; i++ ) {
      _pmCaption[i] = new CPixmap;
      _pmCaption[i]->CreatePixmap( 
              MAX_CAPTION_WIDTH,
              16,
              8,
              NULL,
              true );
  	}
  }
  else {
    Option = SKIP_OSD_REFRESH;
    VP_SubtitleNotFound( &Option );
  }

  return success;
}

int VP_CloseSMI()
{
	if( pSubtitle != NULL ) {
        delete pSubtitle;
        pSubtitle = NULL;
	}

	for( int i=0; i<MAX_CAPTION_BUFFER; i++ ) {
    if( _pmCaption[i] ) {
      delete _pmCaption[i];
      _pmCaption[i] = NULL;
    }
  }
    
  return 0;
}

static int VP_ResyncSMI( int seektime )
{
	int DiffPNC;
	int CaptionMSec;
	RMint64 PlaytimeMSec;

	pMpegDecoder->GetSTC(&PlaytimeMSec,1000);

	if( !pSubtitle ) return false;

	if( PlaytimeMSec == 0 ) {
		pSubtitle->ChunkToHead();
	}
	else {
	    int time;
	    int diff;
	    seektime *= 1000;
	    diff= abs((int)PlaytimeMSec-seektime);
	    if( diff > 10000 ) {
	        time = seektime;
	    }
	    else {
	        time = (int)PlaytimeMSec;
	    }
		pSubtitle->ChunkToAdjacentCaption( time );
	}

	return TRUE;
}


static Caption caption[MAX_CAPTION_BUFFER];

static void _ProcNBSP()
{
	if(pSubtitleSyncDbx != NULL ) {
		pMpegDecoder->RenderText_WithoutRefresh(0,0,0, SY_ON_SUBTITLEDBX, 320, (240-SY_ON_SUBTITLEDBX) );
	}
	else if( pCp->isVisible ) {
		pMpegDecoder->RenderText_WithoutRefresh(0,0,0, SY_ON_CONTROLPANNEL, 320, (240-SY_ON_CONTROLPANNEL-_CP_HEIGHT) );
	}
	else {
		// Blank On OSD
		ClearOsd();
	}
}

static int 
VP_ChunkSMI( void )
{
	int DiffPNC;
	int CaptionMSec;
	RMint64 PlaytimeMSec;
	int mc = TaviSysParam.video.multi_caption;

	pMpegDecoder->GetSTC(&PlaytimeMSec,1000);
	CaptionMSec = pSubtitle->CurrentItemMSec();
	DiffPNC = (int)PlaytimeMSec - CaptionMSec;

  // Error capturing current play time.
	if( PlaytimeMSec < 100 ) {
	    return FALSE;
	}

	if(  DiffPNC > -100 && DiffPNC <= 500  ) {
		int sidx;	// Source Index;
		int ridx; 
		int ShiftByCP;

		if( PlaytimeMSec == 0  || pSndPltDbx->IsVisible() )	{
			return FALSE;
		}

		if( (!pmVideoPlay->IsVisible() && isCaptionEnabled == TRUE ) 
			|| pSubtitleSyncDbx != NULL ) {
			ShiftByCP = 0;

			if( pSubtitleSyncDbx != NULL ) {
		    ShiftByCP = 0;
	    }
			else if( pCp->isVisible ) {
		    ShiftByCP = 43;
	    }

			for(int i=0;i<MAX_CAPTION_BUFFER;i++) {
				caption[i].str = NULL;
				_pmCaption[i]->Fill( 
				    0, 0, 
				    _pmCaption[i]->m_nWidth, _pmCaption[i]->m_nHeight,
				    0 );
			}

			
			bool bclear = false;
			for( sidx=0; sidx<MAX_CAPTION_BUFFER; sidx++ ) {
		    caption[sidx] = pSubtitle->GetCurrentCaption( sidx );
		    if( !caption[sidx].str ) {
		        break;
		    }

		    if( !strcmp(caption[sidx].str, " ") && 
		        pSubtitle->GetCurrentCaptionCount() == 1 ) {
		        bclear = true;
		        break;
		    }
      }

            

      int x = 4;
      int y;
      int w;
      int pmwidth[MAX_CAPTION_BUFFER];
      int lang[MAX_SUPPORT_MULTI_LANG];
      uchar_t* szBuf = new uchar_t[4096];

      for( int i=0; i<MAX_SUPPORT_MULTI_LANG; i++ ) {
        lang[i] = i;
      }
      int mcnt;
      if( mc == MULTI_CAPTION_ALL ) {
        mcnt = MAX_SUPPORT_MULTI_LANG;
      }
      else {
        mcnt = 1;
        lang[0] = pSubtitle->GetLanguages() == 1 ? 0 : mc;
      }

      if( pSubtitle->GetLanguages() == 1 ) {
        mcnt = 1;
      }

      pT->SetOutlineColor( _BLACK );
      ridx = 0;
      for( int j=0; j<mcnt; j++ ) {
        // newline
        if( x != 4 ) {
          pmwidth[ridx++] = x;
          x = 4;
        }
        pSubtitle->SetLanguage( lang[j] );
  			for( sidx=0; sidx<MAX_CAPTION_BUFFER ; sidx++ ) {
  		    caption[sidx] = pSubtitle->GetCurrentCaption( sidx );
  		    if( !caption[sidx].str ) {
  		        break;
  		    }

          pT->ConvertEUC8StrToUnicode16Str( 
                  szBuf, caption[sidx].str, 4096 );

          for( int i=0; szBuf[i]; i++ ) {
              if( x > (MAX_CAPTION_WIDTH-20) || szBuf[i] == '\n' ) {
                  pmwidth[ridx] = x;
                  if( szBuf[i+1] ) {
                      x = 4;
                      ridx++;
                  }

                  if( szBuf[i] == '\n' ) {
                    continue;
                  }
              }

              x += pT->DrawCharacter( 
                      szBuf[i],
                      x,
                      0,
                      caption[sidx].lColor+239,
                      16,
                      DT_TRANSPARENCY|DT_BOLD|DT_OUTLINE|DT_TOP_PANNEL,
                      _pmCaption[ridx] );
              
          }
  			}
 			}
			pT->SetOutlineColor( _SHADOW );
			delete[] szBuf;
			
			if( !bclear ) {
  			pmwidth[ridx++] = x;
			}

      _ProcNBSP();
      y = 240 - ridx*(16+2) - ShiftByCP - 10 /* from bottom */;
      for( int i=0; i<ridx; i++ ) {
          x = (320-pmwidth[i])/2;
          op_BitBlt( &g_SysPixmap, x, y, 
                      _pmCaption[i], 0, 0, 
                      pmwidth[i], 
                      _pmCaption[i]->m_nHeight );
          y += 20;
      }
		}
		pMpegDecoder->RefreshOsd();
		pSubtitle->ChunkToNext();
	}
	else if( DiffPNC > 500 ) { // Over State
		if( PlaytimeMSec == 0  ) {
			return FALSE;
		}

		for(;;) {
			if(pSubtitle->ChunkToNext() ==0) {
				return FALSE;
			}
			CaptionMSec =  pSubtitle->CurrentItemMSec();
			if( PlaytimeMSec <= CaptionMSec ) {
				break;
			}
		}
  }
  else if( DiffPNC < -10000 ) { // Invalid time stamp
    while( 1 ) {
      if( !pSubtitle->ChunkToPrev() ) {
        return FALSE;
      }
      CaptionMSec = pSubtitle->CurrentItemMSec();
      if( PlaytimeMSec > CaptionMSec ) {
        break;
      }
    }
  }

  return 0;
}
 
static RMint64 PauseSec;
static int VP_CommonKeyProcessOK( int ssTaskId )
{
	ClearKeyBuffer();

	if( pVideoPlayer->Status == __VP_PAUSE ||
	    pVideoPlayer->Status == __VP_STOP )
	{
	  // NOTE: DONT DELETE THE BELOW CODES!!!
		//if( transcodeVideo ) {
		//	Seek7(PauseSec );
		//}
		//else {
			pMpegDecoder->Play();
		//}

		VP_SetPlayStatus( __VP_PLAY );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );

		Reset_VideoControlPannelTimer();
	}
	else if( pVideoPlayer->Status == __VP_PLAY )
	{
		pMpegDecoder->GetSTC (&PauseSec, 1);
		pMpegDecoder->Pause();

		VP_SetPlayStatus( __VP_PAUSE );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );

		Reset_VideoControlPannelTimer();
	}

	ClearKeyBuffer();		

	return TRUE;
}


static int VP_CommonKeyProcess_LEFT( int ssTaskId )
{
	switch( pCp->GetCurrentStatus()  ) {
    case _CONTROL_PANNEL_PROGRESS_MODE:
    case _CONTROL_PANNEL_VOLUME_MODE:
    case _CONTROL_PANNEL_SEEK_MODE:
        if( pCp->GetSeekable() && (!pCp->IsSeekingState()) && (playtime.SecOnly>0)) {
            RMint64  t;
            pCp->SetSeekingState(TRUE);
            VP_SetPlayStatus( __VP_REW );

            t = playtime.SecOnly - 30;

            if( t < 10 ) {
                t = 10;
            }

            if( Seek7(t) == 0 ) {
                pCp->BuildSeekTimeText(t);
                pCp->BuildProgressBar( _NORMAL );
                pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE, 1 );
                pMpegDecoder->RefreshOsd();
            }

            VP_SetPlayStatus( __VP_PLAY );
            pCp->SetSeekingState(FALSE);
        }

        pCp->SetSeekingState(FALSE);
        break;
    }

    return 0;
}



static int VP_CommonKeyProcess_RIGHT( int ssTaskId )
{
	switch( pCp->GetCurrentStatus()  ) {
	case _CONTROL_PANNEL_PROGRESS_MODE:
	case _CONTROL_PANNEL_VOLUME_MODE:
	case _CONTROL_PANNEL_SEEK_MODE:
		
		if( pCp->GetSeekable() && 
		    (!pCp->IsSeekingState()) && (playtime.SecOnly>0)) {
			RMint64  t;
			pCp->SetSeekingState(TRUE);
			VP_SetPlayStatus( __VP_FF );

			t = playtime.SecOnly + 30;

			if( t > (totaltime.SecOnly-30) ) {
				t = totaltime.SecOnly-5;
			}


			if( Seek7(t) ==0 ) { 
				pCp->BuildSeekTimeText(t);
				pCp->BuildProgressBar( _NORMAL );
				pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE, 1 );
				pMpegDecoder->RefreshOsd();
			}

			VP_SetPlayStatus( __VP_PLAY );
			pCp->SetSeekingState(FALSE);

			}

		pCp->SetSeekingState(FALSE);
		return 0;
	}

	return 0;
}


static int VP_CommonKeyProcess_UP( int ssTaskId )
{
	DEBUGMSG(1,("VP_CommonKeyProcess_UP\n" ));
	if( pCp->GetCurrentStatus() == _CONTROL_PANNEL_PROGRESS_MODE
		|| pCp->GetCurrentStatus() == _CONTROL_PANNEL_SEEK_MODE )
		{
		}
	
	else if( pCp->GetCurrentStatus() == _CONTROL_PANNEL_VOLUME_MODE )
		{
		g_SndCtrl->VolumeUp();
		pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
		usleep(50);
		ClearKeyBuffer();
		}

	return TRUE;
}

static int VP_CommonKeyProcess_DOWN( int ssTaskId )
{
	if( pCp->GetCurrentStatus() == _CONTROL_PANNEL_VOLUME_MODE )
	{
	  g_SndCtrl->VolumeDown();
		pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
		usleep(50);
		ClearKeyBuffer();
	}				
	return TRUE;
}

extern void clearbackground();

static int VP_CommonKeyProcess_EXIT( int ssTaskId, int ExitOption )
{
	RMint32 temp;
	int ResumeValue =  toggle_display;
	TaviSysParam.etc.ResVal = toggle_display;
	TaviSysParam.etc.ExitOption = ExitOption;
	
	Stop_VideoControlPannelTimer();
	Stop_SoundPaletteCtrlHideTimer();

	if( pTAVI->isVideoPlaying ) {
		VP_PrepareToStopPlaying(0);
	}
	
	pSndPltDbx->Hide(SKIP_OSD_REFRESH);
	pmVideoPlay->Hide( FALSE, FALSE );


	if( ExitOption ) {
		showbackground(2);	
		pLFBrowser->Show();

		if( ResumeValue != 0 ) {
			toggle_display = ResumeValue;
		}

		return VideoPlayerReturn_NORMAL;
	}
	else {
		if( ResumeValue != 0 ) {
			toggle_display = ResumeValue;
			pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
		}

		return ExitOption;
	}
}


int VP_MsgboxButtonUpProc_Bookmark( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( ulMsgId == MSG_TOUCHPAD )
		{

		if( ssParam1 == 1 )
			{
			pBookmarkDbx->CursorDown();
			pBookmarkDbx->ReDraw();
			}
		else if( ssParam1 == -1 )
			{
			pBookmarkDbx->CursorUp();
			pBookmarkDbx->ReDraw();
			}
		else if( ssParam2 == 1 )
			{
			if( !pBookmarkDbx->IsBlankDbx() )
				return -1;
			}
		}

	else
		{

		switch( ssParam1 )
			{
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_UP:
			case TAVI_KEY_LUP:
			case TAVI_KEY_JOG1_UP:
			case TAVI_KEY_JOG1_LUP:
				pBookmarkDbx->CursorUp();
				pBookmarkDbx->ReDraw();
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_DOWN:
			case TAVI_KEY_LDOWN:
			case TAVI_KEY_JOG1_DOWN:
			case TAVI_KEY_JOG1_LDOWN:
				pBookmarkDbx->CursorDown();
				pBookmarkDbx->ReDraw();
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LEFT:
				if( ((pBookmarkDbx->GetCurrentParameters())&_BOOKMARK_DBX_AT_PLAYING) == _BOOKMARK_DBX_AT_PLAYING )
					{
					VP_DeleteOneBookmark( pLFBrowser->CurrentFullPathDir,  
											pLFBrowser->Filename, 
											pBookmarkDbx->GetCurrentChoice() );

					pBookmarkDbx->ReDraw();
 					}

				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_RIGHT:
				if( ((pBookmarkDbx->GetCurrentParameters())&_BOOKMARK_DBX_AT_PLAYING) == _BOOKMARK_DBX_AT_PLAYING )
					break;
				
			case TAVI_KEY_ENTER:
			case TAVI_KEY_JOG1_ENTER:

				if( ((pBookmarkDbx->GetCurrentParameters())&_BOOKMARK_DBX_AT_PLAYING) == _BOOKMARK_DBX_AT_PLAYING )
					{
					if( pBookmarkDbx->GetItemCount() )
						{
						DEBUGMSG(1,("-choice : %d\n", pBookmarkDbx->GetCurrentChoice() ));
						return -1;
						}
					}
				else
					{
					DEBUGMSG(1,("-choice : %d\n", pBookmarkDbx->GetCurrentChoice() ));
					return -1;
					}
				
				break;
				
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_EQ:
				break;
						
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_MENU:
				break;
	
			case TAVI_KEY_MODE:
				pBookmarkDbx->SetChoice(TAVI_KEY_MODE_REVERSE);
				return -1;
				break;
				
			case TAVI_KEY_LMODE:
				pBookmarkDbx->SetChoice(TAVI_KEY_LMODE_REVERSE);
				return -1;
				break;
				
			case TAVI_KEY_EXIT:
				pBookmarkDbx->SetChoice(TAVI_KEY_EXIT_REVERSE);
				return -1;
				break;
				
			case TAVI_KEY_LEXIT:
				pBookmarkDbx->SetChoice(TAVI_KEY_LEXIT_REVERSE);
				return -1;
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_SHUTDOWN:
				pTAVI->isShutdownNeeds = TRUE;
				if( pTAVI->isVideoPlaying ) VP_CommonKeyProcess_EXIT(TASK_ID_VIDEO, TAVI_KEY_SHUTDOWN);
				pVideoPlayer->IsEmergencyExitFlagEnabled = TRUE;
				return -1;
				break;

			}
		}

	return 0;
}





int VP_MsgboxButtonUpProc_Setup( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( ulMsgId == MSG_TOUCHPAD )
		{
		if( ssParam1 == 1 )
			{
			pSetupDbx->CursorDown();
			pSetupDbx->ReDraw();
			}
		else if( ssParam1 == -1 )
			{
			pSetupDbx->CursorUp();
			pSetupDbx->ReDraw();
			}
		}


	else
		{

		switch( ssParam1 )
			{
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_UP:
			case TAVI_KEY_LUP:
				pSetupDbx->CursorUp();
				pSetupDbx->ReDraw();
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_DOWN:
			case TAVI_KEY_LDOWN:
				pSetupDbx->CursorDown();
				pSetupDbx->ReDraw();
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LEFT:
			case TAVI_KEY_LLEFT:
				pSetupDbx->CursorLeft();
				pSetupDbx->ReDraw();
				break;
		

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_RIGHT:
			case TAVI_KEY_LRIGHT:
				pSetupDbx->CursorRight();
				pSetupDbx->ReDraw();
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_ENTER:
				// Exit DialogBox
				return -1;
				break;
				
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_EQ:
				break;
						
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_MENU:
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_EXIT:
				pSetupDbx->Cancel();
				return -1;
				break;

			case TAVI_KEY_MODE:
				pSetupDbx->SetChoice(TAVI_KEY_MODE);
				return -1;
			case TAVI_KEY_LMODE:
				pSetupDbx->SetChoice(TAVI_KEY_LMODE);
				return -1;
			case TAVI_KEY_LEXIT:
				pSetupDbx->SetChoice(TAVI_KEY_LEXIT);
				return -1;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_SHUTDOWN:
				pTAVI->isShutdownNeeds = TRUE;
				if( pTAVI->isVideoPlaying ) VP_CommonKeyProcess_EXIT(TASK_ID_VIDEO, TAVI_KEY_SHUTDOWN);
				pVideoPlayer->IsEmergencyExitFlagEnabled = TRUE;
				return -1;
				break;		
			}
		}

	return 0;
}

int VP_UpdateSetting( int* parameter  )
{
	return TRUE;
}


int _CpProc_SectionRepeat( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( pCp->GetCurrentSectionRepeatOptionStatus() == SECTION_REPEAT_MARK_A )
		{
		if( ulMsgId == MSG_BUTTON_UP && ssParam1 == TAVI_KEY_AB )
			{
			 // goto B
			pCp->MarkingB = playtime.SecOnly;
			pCp->RepeatOptionStatusCounter = 0;
			pCp->SetCurrentSectionRepeatOptionStatus( SECTION_REPEAT_MARK_AB );
			pMpegDecoder->RefreshOsd();
			return 0;
			}

		else
			{
			// stop (cancel)
			pCp->SetCurrentSectionRepeatOptionStatus( SECTION_REPEAT_DISABLE );
			pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );

			Reset_VideoControlPannelTimer();

			return 0;
			}
		}

	else if( pCp->GetCurrentSectionRepeatOptionStatus() == SECTION_REPEAT_MARK_AB  )
		{

		// stop (cancel)
		pCp->SetCurrentSectionRepeatOptionStatus( SECTION_REPEAT_DISABLE );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );

		Reset_VideoControlPannelTimer();

		return 0;

		}

	return -2;
}

void _CpTpProc_ProgressMode( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( ssParam1 == 0 && ssParam2 == 1 )
		{// Click TouchPad
		DEBUGMSG(1,("_CONTROL_PANNEL_PROGRESS_MODE : Click TouchPad\n"));
		pCp->SetSeekingState(FALSE);
		pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
		}

	else if( pCp->GetSeekable() )
		{
		DEBUGMSG(1,("_CONTROL_PANNEL_PROGRESS_MODE : NOT Click TouchPad\n"));
		pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
		}
}
void _CpTpProc_SeekMode( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	int newvalue;

	if( ssParam1 == 0 && ssParam2 == 0 )
		return;

	else if( ssParam1 == 0 && ssParam2 == 1 )
	 	{// Click TouchPad
		DEBUGMSG(1,("_CONTROL_PANNEL_SEEK_MODE : Click TouchPad\n"));
		if( pCp->IsSeekingState()  )
	 		{
			Stop_VideoSeekTimer();
			pCp->SetSeekingState(FALSE);
	 		}
		pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
	 	}

	else if( pCp->GetSeekable() )
		{

		if( !pCp->IsSeekingState() )	// IsSeekingState == FALSE
			{// Click TouchPad
			SetTaviTouchpadMode( MEP_REPORT_RELATIVE );
			pCp->SetSeekValue( playtime.SecOnly );
			pCp->SetSeekingState(TRUE);
			}

		else if( ssParam1 == 1 || ssParam1 == -1 )
			{
			newvalue = pCp->GetSeekValue() + (30*ssParam1);
			if( newvalue > 0 )
				{
				
				if( newvalue >  totaltime.SecOnly)
					newvalue = totaltime.SecOnly-15;

				pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
				pCp->BuildSeekTimeText( newvalue);
				pCp->BuildSeekControlBar( _REDRAW, 0, newvalue );
				pCp->SetSeekValue( newvalue );
				playtime.SecOnly = (RMuint64) newvalue;

				pMpegDecoder->RefreshOsd();
				Reset_VideoSeekTimer();
				}

			}
		}
}
void _CpTpProc_VolumeMode( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	SetTaviTouchpadMode( MEP_REPORT_RELATIVE );

	if( ssParam2 == 1 )	{	// Click TouchPad
		Stop_VideoSeekTimer();
		pCp->SetSeekingState(FALSE);
		pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
		return;
	}

    if( ssParam1 == 1 ) {
        g_SndCtrl->VolumeUp();
    }
    else if( ssParam1 == -1 ) {
        g_SndCtrl->VolumeDown();
    }
    int vol = g_SndCtrl->GetVolume();
	pCp->BuildVolumeText( vol );
	pCp->BuildVolumeControlBar(_NORMAL, vol );
	pMpegDecoder->RefreshOsd();

}
int _CpTouchpadProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( pBookmarkDbx != NULL )
		{
		Stop_VideoControlPannelTimer();
		return (VP_MsgboxButtonUpProc_Bookmark( ssTaskId, ulMsgId, ssParam1, ssParam2));
		}
	else if( pmVideoPlay->IsVisible() )
		{
		Stop_VideoControlPannelTimer();
		return( pmVideoPlay->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
		}
	else
		{

		Reset_VideoControlPannelTimer();
		
		switch( pCp->GetCurrentStatus() )
			{
			case _CONTROL_PANNEL_PROGRESS_MODE:
				_CpTpProc_ProgressMode( ssTaskId, ulMsgId, ssParam1, ssParam2 );
				break;

			case _CONTROL_PANNEL_SEEK_MODE:									
				_CpTpProc_SeekMode( ssTaskId, ulMsgId, ssParam1, ssParam2 );
				break;				

			case _CONTROL_PANNEL_VOLUME_MODE:
				_CpTpProc_VolumeMode( ssTaskId, ulMsgId, ssParam1, ssParam2 );
				break;

			}

		}
	return -2;
}
void _CpKpProc_LongUp( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	TMsgEntry tMsg;
	
	if( pCp->GetCurrentStatus() == _CONTROL_PANNEL_PROGRESS_MODE || 
		pCp->GetCurrentStatus() == _CONTROL_PANNEL_VOLUME_MODE )
		{

		pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
		Stop_VideoControlPannelTimer();

		for(;;)
			{

			if( AppEventCheck( pVideoPlayer->TaskId, &tMsg) == 0 )
				{ MSG_Send( pVideoPlayer->TaskId, &tMsg ); break; }

			if( read( handle_key, &tempkey, sizeof(keystruct) ) > 0 )
				{

				Reset_VideoControlPannelTimer();
				
				break;
				}
			else {
				if( g_SndCtrl->VolumeUp() == false ) {
					Reset_VideoControlPannelTimer();
					return;
				}
				int vol = g_SndCtrl->GetVolume();
				pCp->BuildVolumeText( vol );
				pCp->BuildVolumeControlBar(_NORMAL, vol );
				pMpegDecoder->RefreshOsd();
			}		

			if( pVideoPlayer->Status == __VP_PLAY )
				if( fpVideoRenderer( &cbtVideo ) != 0)
					break;

			if( pSubtitle != NULL && pSubtitleSyncDbx != NULL ) {
				VP_ChunkSMI();
			}

			usleep(100);

			}	

		}

}
void _CpKpProc_LongDown( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	TMsgEntry tMsg;
	
	if( pCp->GetCurrentStatus() == _CONTROL_PANNEL_PROGRESS_MODE || 
		pCp->GetCurrentStatus() == _CONTROL_PANNEL_VOLUME_MODE )
		{
		pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
		Stop_VideoControlPannelTimer();

		for(;;)
			{

			if( AppEventCheck( pVideoPlayer->TaskId, &tMsg) == 0 )
				{ MSG_Send( pVideoPlayer->TaskId, &tMsg ); break; }

			if( read( handle_key, &tempkey, sizeof(keystruct) ) > 0 )
				{
				Reset_VideoControlPannelTimer();
				break;
				}
			else {
				if( g_SndCtrl->VolumeDown() == false ) {
					Reset_VideoControlPannelTimer();
					return;
				}
				int vol = g_SndCtrl->GetVolume();
				pCp->BuildVolumeText( vol );
				pCp->BuildVolumeControlBar(_NORMAL, vol );
				pMpegDecoder->RefreshOsd();
			}		

			if( pVideoPlayer->Status == __VP_PLAY )
				if( fpVideoRenderer( &cbtVideo ) != 0)
					break;

			if( pSubtitle != NULL && pSubtitleSyncDbx != NULL )
				VP_ChunkSMI();

			usleep(100);
			}	

		}
		
}
void _CpKpProc_LongLeft( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( pCp->GetCurrentStatus() == _CONTROL_PANNEL_VOLUME_MODE )
		{
		pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
		}

	if ( pCp->GetSeekable() && 
		(pCp->GetCurrentStatus() == _CONTROL_PANNEL_SEEK_MODE || pCp->GetCurrentStatus() == _CONTROL_PANNEL_PROGRESS_MODE) 	)
		{

		TMsgEntry tMsg;
		RMint32 percentage;

		pCp->SetSeekingState(TRUE);
		if( ssParam2 == 1 )
			{

			VP_SetPlayStatus( __VP_REW );

			SeekPosition_SecOnly = playtime.SecOnly;

			percentage = ( playtime.SecOnly * 100 / totaltime.SecOnly );
			
			//////////////////////////////////////////////////////////////////////
			usSeekPoint_AccValue			= 1;
			usSeekPoint_ProgressCounter 	= 0;

			ClearKeyBuffer();
			pMpegDecoder->Pause();
			
			for(;;)
				{
				if( read( handle_key, &tempkey, sizeof(keystruct) ) > 0 )
					{
					Seek7( SeekPosition_SecOnly );
					VP_SetPlayStatus( __VP_PLAY );
					pCp->SetSeekingState(FALSE);
					playtime.SecOnly = SeekPosition_SecOnly;
					//pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
					// Clear caption
					pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE, 1 );
					break;
					}
				else
					{

					if( SeekPosition_SecOnly <= 0 )
						{
						Seek7( SeekPosition_SecOnly );
						VP_SetPlayStatus( __VP_PLAY );
						pCp->SetSeekingState(FALSE);
						playtime.SecOnly = SeekPosition_SecOnly;	
						// Clear caption
						pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE, 1 );
						break;
						}

					else
						{
						SeekPosition_SecOnly = SeekPosition_SecOnly - usSeekPoint_AccValue;
						usSeekPoint_ProgressCounter++;
						
						pCp->BuildSeekTimeText( (int)SeekPosition_SecOnly);

						if( pCp->GetCurrentStatus() == _CONTROL_PANNEL_PROGRESS_MODE )
							{
							playtime.SecOnly = SeekPosition_SecOnly;
							pCp->SetSeekingState(FALSE);
							pCp->Show(  _CONTROL_PANNEL_SEEK_MODE );
							}
						else
							{
							pCp->BuildSeekTimeText( (int)SeekPosition_SecOnly );
							pCp->BuildSeekControlBar( _REDRAW, 0, (int)SeekPosition_SecOnly );
							}

						pMpegDecoder->RefreshOsd();

						if( (usSeekPoint_ProgressCounter%15 == 0)
							&& usSeekPoint_AccValue < 20 )
							usSeekPoint_AccValue = usSeekPoint_AccValue *2;

						if( SeekPosition_SecOnly < 0 )
							SeekPosition_SecOnly = 0;
						

						}
					}
				}
			//////////////////////////////////////////////////////////////////////
			}
		}
}

static void _CpKpProc_LongRight( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( pCp->GetCurrentStatus() == _CONTROL_PANNEL_VOLUME_MODE ) {
		pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
	}

	if ( pCp->GetSeekable() && 
		(pCp->GetCurrentStatus() == _CONTROL_PANNEL_SEEK_MODE || 
		 pCp->GetCurrentStatus() == _CONTROL_PANNEL_PROGRESS_MODE) 	) {
		TMsgEntry tMsg;
		RMint32 percentage;
		pCp->SetSeekingState(TRUE);
						
		if( ssParam2 == 1 )
			{
			VP_SetPlayStatus( __VP_FF);

			if( playtime.SecOnly != 0 ) {
				SeekPosition_SecOnly = playtime.SecOnly;
			}

			percentage = ( playtime.SecOnly * 100 / totaltime.SecOnly );

			//////////////////////////////////////////////////////////////////////
			usSeekPoint_AccValue			= 1;
			usSeekPoint_ProgressCounter 	= 0;

			ClearKeyBuffer();
			pMpegDecoder->Pause();
			
			for(;;) {
				if( read( handle_key, &tempkey, sizeof(keystruct) ) > 0 ) {
					Seek7( SeekPosition_SecOnly );
					VP_SetPlayStatus( __VP_PLAY );
					pCp->SetSeekingState(FALSE);
					playtime.SecOnly = SeekPosition_SecOnly;
					pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE, 1 );
					break;
				}
				else {
					if( (totaltime.SecOnly - 10) <= SeekPosition_SecOnly )
						{
						Seek7(totaltime.SecOnly - 10);
						VP_SetPlayStatus( __VP_PLAY );
						pCp->SetSeekingState(FALSE);
						playtime.SecOnly = (totaltime.SecOnly - 10);
						//pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
						// Clear caption.
						pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE, 1 );
						break;
						}
					else
						{
						SeekPosition_SecOnly = SeekPosition_SecOnly + usSeekPoint_AccValue;
						usSeekPoint_ProgressCounter++;
						
						DEBUGMSG(1,("Cursor Position : %d\r", SeekPosition_SecOnly));

						if( pCp->GetCurrentStatus() == _CONTROL_PANNEL_PROGRESS_MODE )
							{
							playtime.SecOnly = SeekPosition_SecOnly;
							pCp->Show(  _CONTROL_PANNEL_SEEK_MODE );
							}
						else
							{
							pCp->BuildSeekTimeText( (int)SeekPosition_SecOnly );
							pCp->BuildSeekControlBar( _REDRAW, 0, (int)SeekPosition_SecOnly );
							}

						pMpegDecoder->RefreshOsd();

						if( (usSeekPoint_ProgressCounter%15 == 0)
							&& usSeekPoint_AccValue < 20 )
							usSeekPoint_AccValue = usSeekPoint_AccValue *2;

						if( SeekPosition_SecOnly > totaltime.SecOnly )
							SeekPosition_SecOnly = totaltime.SecOnly - 5;

						}
					}
			
				}
			}
		}
}




int _CpKeypadProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	switch( ssParam1 )
		{
		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_UP:
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			VP_CommonKeyProcess_UP( ssTaskId );
			break;

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_DOWN:
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			VP_CommonKeyProcess_DOWN( ssTaskId );
			break;

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_LEFT:
			//pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			VP_CommonKeyProcess_LEFT(ssTaskId);
			break;

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_RIGHT:
			//pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			VP_CommonKeyProcess_RIGHT(ssTaskId);
			break;
			
		//////////////////////////////////////////////////////////////////////				
		case TAVI_KEY_JOG1_DOWN:
		    g_SndCtrl->VolumeDown();
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			break;

		//////////////////////////////////////////////////////////////////////				
		case TAVI_KEY_JOG1_UP:	// Volume
		    g_SndCtrl->VolumeUp();
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			break;
							
		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_JOG2_DOWN:
			pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			VP_CommonKeyProcess_LEFT(ssTaskId);
			
			break;

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_JOG2_UP:	// Seek
			pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			VP_CommonKeyProcess_RIGHT(ssTaskId);
			break;


		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_JOG1_LDOWN:	// Volume
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			VP_CtrlPannelButtonUpProc( ssTaskId, MSG_BUTTON_UP, TAVI_KEY_LDOWN, ssParam2 );
			break;

		case TAVI_KEY_JOG2_LDOWN:  // Seek	
			pCp->SetSeekingState(TRUE);
			pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			goto F_TAVI_KEY_LLEFT;
			break;

		case TAVI_KEY_LUP:	// Volume up
F_TAVI_KEY_LUP:
			_CpKpProc_LongUp( ssTaskId, ulMsgId, ssParam1, ssParam2 );
			break;

		case TAVI_KEY_LDOWN:	// Volume Down
F_TAVI_KEY_LDOWN:
			_CpKpProc_LongDown( ssTaskId, ulMsgId, ssParam1, ssParam2 );
			break;

		case TAVI_KEY_LLEFT:
F_TAVI_KEY_LLEFT:
			_CpKpProc_LongLeft( ssTaskId, ulMsgId, ssParam1, ssParam2 );
			break;

		case TAVI_KEY_JOG1_LUP:	// Volume
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			VP_CtrlPannelButtonUpProc( ssTaskId, MSG_BUTTON_UP, TAVI_KEY_LUP, ssParam2 );
			break;
			
		case TAVI_KEY_JOG2_LUP:  // Seek	
			pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			pCp->SetSeekingState(TRUE);
			goto F_TAVI_KEY_LRIGHT;
			
		case TAVI_KEY_LRIGHT:
F_TAVI_KEY_LRIGHT:
			_CpKpProc_LongRight( ssTaskId, ulMsgId, ssParam1, ssParam2 );
			break;

		case TAVI_KEY_ENTER:
		case TAVI_KEY_JOG2_ENTER:
			VP_CommonKeyProcessOK(ssTaskId);
			break;

		case TAVI_KEY_LENTER:
			if( pVideoPlayer->LastBookmarkingInSeconds + 5 < (U16)playtime.SecOnly  )
				{
				VP_Bookmarking( pLFBrowser->CurrentFullPathDir,  pLFBrowser->Filename, (U16)playtime.SecOnly, -1 );
				VP_DrawMsgBox( ML_VP_ADD_BOOKMARK );
				}
			break;

		case TAVI_KEY_EQ:
			if( (pLFBrowser->CurrentFileType== 4 || pLFBrowser->CurrentFileType== 5 ) || selected_audio_stream_format != 0x55 )
				pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
			else {
			    if( !mio_enabled() ) {
    				VP_SoundPaletteDbx(NULL);
    			}
    			else {
    			    pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
    			}
			}
			break;

		//////////////////////////////////////////////////////////////////////ll
		// In play mode. AB means [Repeat A]
		case TAVI_KEY_AB:
			pCp->MarkingA = playtime.SecOnly;
			pCp->SetCurrentSectionRepeatOptionStatus( SECTION_REPEAT_MARK_A );
			pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
			Stop_VideoControlPannelTimer();
			break;
					
		case TAVI_KEY_MENU:
			if( !pmVideoPlay->IsVisible() )
				{
				pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE, CO_SKIP_CAPTION );
				Stop_VideoControlPannelTimer();
				Stop_FileBrowserTimer();
				pmVideoPlay->Show();
				}
			break;
		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_EXIT:
			pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
			break;

		case TAVI_KEY_LEXIT:
		case TAVI_KEY_MODE:
		case TAVI_KEY_LMODE:
		case TAVI_KEY_JOG1_LENTER:
			pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
			VP_CommonKeyProcess_EXIT(ssTaskId, _VideoExit_NORMAL );
			return -1;
			break;

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_SHUTDOWN:
			pTAVI->isShutdownNeeds = TRUE;
			if( pTAVI->isVideoPlaying ) VP_CommonKeyProcess_EXIT(TASK_ID_VIDEO, TAVI_KEY_SHUTDOWN);
			pVideoPlayer->IsEmergencyExitFlagEnabled = TRUE;
			return -1;
			break;
		}

	return -2;
}

int VP_CtrlPannelButtonUpProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	int ret;
	int t, MeanValue;

	if( pCp->GetSeekable() )
		if( (ret=_CpProc_SectionRepeat( ssTaskId, ulMsgId, ssParam1, ssParam2 )) > -2 )
			return ret;

	if( ulMsgId == MSG_TOUCHPAD )
		{
		if( (ret=_CpTouchpadProc( ssTaskId, ulMsgId, ssParam1, ssParam2 )) > -2 )
			return ret;
		}
	else if( ulMsgId == MSG_BUTTON_UP )
		{
		if( (ret=_CpKeypadProc( ssTaskId, ulMsgId, ssParam1, ssParam2 )) > -2 )
			return ret;
		}

	return 0;
}

extern int nIdle_stoptimer;
extern int nIdle_lcdofftimer;

void VP_SetPlayStatus( eVP_Status PlayerState, eOSD_REFRESH_OPTION refresh )
{
	eVIDEOPLAYER_CONTROL_ICONS IconState;

	pVideoPlayer->Status = PlayerState;

	if( PlayerState >= __VP_NEW ) // NEW STOP PREV NEXT
		{
		pTAVI->isVideoPlaying = 0;
		IconState = _VCTRL_PAUSE;
		nIdle_stoptimer = 0;
		nIdle_lcdofftimer = 0;
		}
	else							// PLAY PAUSE FF REW
		{
		pTAVI->isVideoPlaying = 1;
		IconState = (eVIDEOPLAYER_CONTROL_ICONS)PlayerState;
		}

	pCp->SetPlayStatus( IconState );

	if( 	IsControlPannelVisible() && 
		(refresh == OSD_REFRESH ||refresh == SKIP_OSD_REFRESH) )
		{
		pCp->RedrawPlayStatus( IconState, refresh );
		}
}

void VP_DialogBox_MessagePeeking()
{
	TMsgEntry tMsg;

	while( 1 ) {
		////////////////////////////////////////////////////////////////////////////
		// Video Player DialogBox Message Peeking
    memset( (void*)&tMsg, 0, sizeof(TMsgEntry) );

		if( MSG_Peek_Ex( pVideoPlayer->TaskId, &tMsg, MSG_PEEK_ALL ) == 0) {
			if( VP_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) < 0)
				break;
		}

		if( pVideoPlayer->Status == __VP_PLAY ) {
			if( fpVideoRenderer( &cbtVideo ) != 0)
				break;
		}

		if( pSubtitle != NULL && pSubtitleSyncDbx != NULL ) {
			VP_ChunkSMI();
		}
	}	
}

int VP_DrawMsgBox( RMuint32 msg )
{
	char temp[256];
	uchar_t ucTemp[256];

	switch( msg )
		{
		case ML_VP_ADD_BOOKMARK:
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_VP_ADD_BOOKMARK, 200, 80, 0, DL_BG_BLANK );
			pMbx->Show();
			for(int il=0;il<20;il++) usleep(50);
			fpVideoRenderer( &cbtVideo );
			for(int il=0;il<30;il++) usleep(50);
			pCp->Hide();
			pMbx->Hide();
			ClearOsd();
			pMpegDecoder->RefreshOsd();
			return TRUE;
			break;
		
		case 2:
		case ML_VP_SAVE_FILE_POSITION:
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_VP_SAVE_FILE_POSITION, -1, -1, 0, DL_BG_ICON );
			pMbx->Show();
			return TRUE;
			break;

		case 3:
		case ML_USB_CONNECT:
			pMbx->Reset( ML_INFORMATION, _GREEN, ML_USB_CONNECT, -1, -1, 0, DL_BG_HOST );
			pMbx->Show();
			return TRUE;
			break;

		case 4:
		case ML_USB_DISCONNECT:
			pMbx->Reset( ML_INFORMATION, _GREEN, ML_USB_DISCONNECT, -1, -1, 0, DL_BG_HOST );
			pMbx->Show();
			return TRUE;
			break;

		case ERR_VR_GMC:
			Stop_VideoControlPannelTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_XVID_GMC_CONFIRM, 280, 150, 0, (DL_BG_BLANK|DL_BTN_OK_CANCEL) );
			pMbx->Show();
			ClearKeyBuffer();
			VP_DialogBox_MessagePeeking();
			pMbx->Hide();
			if( pMbx->GetCurrentChoice() == 1 ) 
				{
				pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1);
				return TRUE;
				}
			else
				{
				pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1);
				return FALSE;
				}

			break;


		case ERR_VR_MP4V:
			Stop_VideoControlPannelTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_MPEG4_CONFIRM, 280, 150, 0, (DL_BG_BLANK|DL_BTN_OK_CANCEL) );
			pMbx->Show();
			ClearKeyBuffer();
			VP_DialogBox_MessagePeeking();
			pMbx->Hide();

			if( pMbx->GetCurrentChoice() == 1 ) 
				{
				pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1);
				return TRUE;
				}
			else
				{
				pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1);
				return FALSE;
				}

			break;
			
		case ERR_VR_QPEL:
			Stop_VideoControlPannelTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_QPEL_CONFIRM, 280, 150, 0, (DL_BG_BLANK|DL_BTN_OK_ONLY) );
			pMbx->Show();
			ClearKeyBuffer();
			VP_DialogBox_MessagePeeking();
			pMbx->Hide();
			pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 0);
			return TRUE;
			break;

		case ERR_VR_MP42:
			Stop_VideoControlPannelTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_MSMPEG4_V2_CONFIRM, 280, 150, 0, (DL_BG_BLANK|DL_BTN_OK_ONLY) );
			pMbx->Show();
			ClearKeyBuffer();
			VP_DialogBox_MessagePeeking();
			pMbx->Hide();
			pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 0);
			return TRUE;
			break;

		case ERR_VR_FPS_VALUE_INVALID:
			Stop_VideoControlPannelTimer();
			sprintf( temp, "%s", "This file has invalid frame rate info.|Video seeking disabled|Do you want to play this video file?");
			str2ustr( ucTemp, temp, strlen(temp) );
			pMbx->Reset( ML_INFORMATION, _ORANGE, ucTemp, 300, 150, 0, (DL_BG_BLANK|DL_BTN_OK_CANCEL) );
			pMbx->Show();
			ClearKeyBuffer();
			VP_DialogBox_MessagePeeking();
			pMbx->Hide();

			if( pMbx->GetCurrentChoice() == 1 ) 
				{
				//pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
				pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1);
				return TRUE;
				}
			else
				{
				pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1);
				return FALSE;
				}

			break;

		case 1:
		case ML_FAILED_TO_PLAY:
		default:
			Stop_VideoControlPannelTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_FAILED_TO_PLAY, -1, -1, 0, (DL_BG_BLANK|DL_BTN_OK_ONLY));
			pMbx->Show();
			ClearKeyBuffer();
			VP_DialogBox_MessagePeeking();
			pMbx->Hide();
			return TRUE;
			break;
		}

}

void VP_ShowException( FATFS_ERROR err )
{
	Stop_FileBrowserTimer();
	sprintf( cTemp, "Exception Error.|(0x%x)", err );
	str2ustr( ucTemp, cTemp );
	pMbx->Reset( ML_INFORMATION, _ORANGE, ucTemp, 244, 150, 0, DL_BTN_OK_ONLY );
	pMbx->Show();
	ClearKeyBuffer();
	VP_DialogBox_MessagePeeking();
	pMbx->Hide();
}

static void 
VP_PrepareToStopPlaying( int option )
{
  VP_CloseSMI();
    
  // For removing a screen noise 
  if( toggle_display == 0 ) {
      g_MiscCtrl->MainLCDOnOff( false );
      lcd_disable();
  }
    
	Stop_VideoControlPannelTimer();
	Stop_SoundPaletteCtrlHideTimer();
	VP_SetPlayStatus( __VP_STOP, CALCULATION_ONLY);

	pVideoPlayer->IsVideoShowState = FALSE;
	fpVideoRenderStopper(0);

	if( pCp->GetSeekable() ) {
		VP_SaveCurrentPosition( 
		  pLFBrowser->CurrentFullPathDir,  
		  pLFBrowser->Filename, 
		  (U16)playtime.SecOnly );
	}


	pMpegDecoder->Reset();

	pVideoPlayer->ProgressPercentage			= 0;
	pVideoPlayer->LastBookmarkingInSeconds 	= 0;
	pVideoPlayer->isControlPannelVisible 		= -1;
	pVideoPlayer->IsTouchValueGrabStarted		= 0;
	pVideoPlayer->IsControlPannelShowed		= FALSE;
	pVideoPlayer->IsVideoScreenInitialized 		= FALSE;

	memset( &playtime, 0, sizeof( TAVI_TIME ) );
	memset( &totaltime, 0, sizeof( TAVI_TIME ) );

	ASSERT(0); // Checkout And Fix Below

	if( TaviSysParam.video.playlist_repeat != 1 ) {
		memset( &gVideofilePlayinfo, 0 , sizeof( VIDEOFILE_PLAYINFO ) );
		memset( &totaltime, 0, sizeof( TAVI_TIME ) );	
		pLFBrowser->Filename[0] = 0;
		filename[0] = '\0';
		pCp->Reset();
	}	

	if( pSubtitle != NULL ) {
		delete pSubtitle;
		pSubtitle = NULL;
	}

  // clear screen and Restore resolution.
  clearbackground();
  
  // Recover resolution
  pMpegDecoder->SetVideoDestination( 
      	TaviSysParam.output[toggle_display].x,
      	TaviSysParam.output[toggle_display].y,
      	TaviSysParam.output[toggle_display].w,
      	TaviSysParam.output[toggle_display].h );


  if( toggle_display == 0 ) {
    lcd_enable( g_MiscCtrl->GetValueCtrl_Out() );
    for( int i=0; i<10; i++ ) usleep( 1000 );
    g_MiscCtrl->MainLCDOnOff( true );
  }
}

static int VP_PrepareToCurrentFileRepeat( int option )
{
	VP_PrepareToStopPlaying(1);
	pCp->SetPlayStatus(_VCTRL_PAUSE);		// for Icon
	return TRUE;
}

static int VP_PrepareToCurrentDirRepeat( int option )
{
	int NewIdx;
	int NewVideoType;
	int iResourceCount=0;
	int iResourceCurrentSelected = 0;
	uchar_t StringTemp[260];
	
	VP_PrepareToCurrentFileRepeat(0);
	filename[0] = '\0';

	iResourceCount = pLFBrowser->GetListItemCount();
	iResourceCurrentSelected = pLFBrowser->SetCurrentChoiceToNext();

	for( int idx=iResourceCurrentSelected; idx<iResourceCount; idx++ )
		{

		if( 	pLFBrowser->GetListItemType( idx ) > _NETWORK_DIRECTORY )		
			{

			StringTemp[0] = '\0';
			pLFBrowser->GetListItemName_UC16( idx, StringTemp );

			if( CheckExtension_UC16( StringTemp ) == _VIDEO_FILE )
				{
				pLFBrowser->SetCurrentChoice(idx);

				if( idx == pLFBrowser->DirRepeatStartPoint )
					{
					return FALSE;
					}
				else
					{
					pCp->SetFilename(StringTemp);
 					ustrcpy( pLFBrowser->Filename, StringTemp );

					NewVideoType = CheckVideoExtension_UC16( pLFBrowser->Filename );
					VP_SetVideoRenderer(NewVideoType);

					return TRUE;

					}
				}
			}
	
		}

	return TRUE;
}


int VP_SetVideoRenderer( int id )
{
	switch( id )
		{
		case 3:
			fpVideoRenderer 			= fb_isrenderdone; 
			fpVideoRenderStopper		= fb_renderstop;
			pLFBrowser->CurrentFileType 	= 3;
			break;

		case 4:
			fpVideoRenderer 			= fb_isrenderdone;
			fpVideoRenderStopper		= fb_renderstop;
			pLFBrowser->CurrentFileType	= 4;
			break;

		case 5:
			t0 = 0;
			currentTime = -1;
			fpVideoRenderer 			= fb_isrenderdone; 
			fpVideoRenderStopper		= fb_renderstop;
			pLFBrowser->CurrentFileType	= 5;
			break;

		default:
			VP_DrawMsgBox(1);
			for(int i=0; i<20; i++ )sleep(1);
			return FALSE;

		}

	return TRUE;

}

static int VP_PrepareAndPlay( uchar_t filename[] )
{
	int tLastResume=-1, tBookmark=-1, choice=-1;
	TMsgEntry tMsg;
	TTaskInitParam tParam;
	tParam.ssTaskId = 2;
	RMuint32 ret;
	uchar_t FileExt[4];

	ClearKeyBuffer();

	pVideoPlayer->ProgressPercentage = 0;
	memset( &playtime, 0, sizeof(int)*3+sizeof(RMint64) );
	memset( &totaltime, 0, sizeof(int)*3+sizeof(RMint64) );

	pVideoPlayer->isControlPannelVisible = -1;

	if(!VP_SetVideoRenderer( CheckVideoExtension_UC16( filename ) ) )
		return TAVI_KEY_EXIT_REVERSE;
		
	pCp->SetFilename( filename );
	pLFBrowser->RememberDirRepeatStartPoint();

	ustrcpy( FullPath, pLFBrowser->CurrentFullPathDir );
	ustrcat( FullPath, sSlash );
	ustrcat( FullPath, filename );

	pVideoPlayer->CurrentItemNumber = pLFBrowser->GetCurrentChoice();

	tLastResume = (int)VP_CheckLastResumeFile( pLFBrowser->CurrentFullPathDir,  filename );				
	DEBUG_U16MSG( pLFBrowser->CurrentFullPathDir, "-pLFBrowser->CurrentFullPathDir");
	DEBUG_U16MSG( pLFBrowser->Filename, "-pLFBrowser->Filename");
	DEBUG_U16MSG( filename, "-filename");

	
	if( tLastResume > 10 && pLFBrowser->CurrentFileType != 4)
		{
	
		while(1)
			{

			////////////////////////////////////////////////////////////////////////////////////
			// Startup
			pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 0);
			pVideoPlayer->isControlPannelVisible = FALSE;
			VP_DrawFrame(TOP_FRAME, 0);
			
			pBookmarkDbx = new CBookmarkDialogBox( ML_START_POINT, 10-1, 35-1, 320-(10*2)-1, 240-35-10-1, VP_MsgboxButtonUpProc_Bookmark, 0, ID_SELECT|ID_CANCEL|_BOOKMARK_DBX_AT_BEGINNIG ); 
			VP_BookmarkLoading( pLFBrowser->CurrentFullPathDir, filename );
			pBookmarkDbx->Display();
			SetTaviTouchpadMode( MEP_REPORT_RELATIVE );

			mep_eat_event();
			ClearKeyBuffer();
			VP_DialogBox_MessagePeeking();

			tBookmark = pBookmarkDbx->GetSeekPoint();
			choice = pBookmarkDbx->GetCurrentChoice();

			delete pBookmarkDbx;
			pBookmarkDbx = NULL;
			ClearDbxOsd();

			switch( choice )
				{
				case TAVI_KEY_MODE_REVERSE:
					pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
					pVideoPlayer->isControlPannelVisible = FALSE;		
					return TAVI_KEY_MODE_REVERSE;
					break;

				case TAVI_KEY_LMODE_REVERSE:
					pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
					pVideoPlayer->isControlPannelVisible = FALSE;		
					return TAVI_KEY_LMODE_REVERSE;
					break;

				case TAVI_KEY_EXIT_REVERSE:
					pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
					pVideoPlayer->isControlPannelVisible = FALSE;		
					return TAVI_KEY_EXIT_REVERSE;
					break;

				case TAVI_KEY_LEXIT_REVERSE:
					DEBUGMSG(1,("VP_PrepareAndPlay : choice == TAVI_KEY_LEXIT_REVERSE\n"));
					pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
					pVideoPlayer->isControlPannelVisible = FALSE;		
					return TAVI_KEY_LEXIT_REVERSE;
					break;
				}			
			
			if( tLastResume > 10 || tBookmark > 0 )
				break;

			}
		}

	ClearScreen();

#if USE_PROGRESS_PALETTE
	usleep(10);
	pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(VIDEO_LOADING_PROGRESS_PALETTE);
	ClearOsd();
	pMpegDecoder->RefreshOsd();
#else
	//pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
#endif

	
	if( (ret=MAIN_RenderVideoFile( FullPath, pLFBrowser->CurrentFileType, &cbtVideo)) == ERR_VR_NOERROR)
		{
		DrawLoadingProgressV4(5000,0);

#if USE_PROGRESS_PALETTE
		pMpegDecoder->TAVI_InitSystemPalette();
		TAVI_LoadPaletteTransparencyData(SUB_PALETTE);
		ClearOsd();
		pMpegDecoder->RefreshOsd();
		VP_SetPlayStatus( __VP_PLAY );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
#else
		VP_SetPlayStatus( __VP_PLAY );
#endif

        // Load caption file.
        VP_PrepareSMI( FullPath );

		if(pLFBrowser->CurrentFileType== 3) {
			Seek7(0);
		}

		if( (pLFBrowser->CurrentFileType== 4 || pLFBrowser->CurrentFileType== 5 ) ||
			selected_audio_stream_format != 0x55 )
			{
			pmVideoPlay = pVideoMenu;
			pCp->SetEqualizerStatus(SOUND_NORMAL);
			}
		else
			{
			pCp->SetEqualizerStatus((SOUNDEFFECT_t)TaviSysParam.sound.eq);
			}
		_SoundPalette_RealApply(0);

		switch( choice )
			{

			//Resume
			case -2:
				if( tLastResume > 0 ) 
					{
					if( Seek7(tLastResume) == 0 )
						{
						pCp->SetVisualPlaytime((RMint64)tLastResume);
						}
					else
						{
						pCp->SetVisualPlaytime((RMint64)0);
						}
					}
				else
					{
					if(pLFBrowser->CurrentFileType== 3)
						Seek7(0);
					pCp->SetVisualPlaytime((RMint64)0);
					}
				break;

			// From The Beggining
			case -1:

				switch(pLFBrowser->CurrentFileType)
					{
					case 3:
						//Seek7(0);
						pCp->SetVisualPlaytime((RMint64)0);
						break;
					case 4:
						pCp->SetVisualPlaytime((RMint64)0);
						break;
					case 5:
						pCp->SetVisualPlaytime((RMint64)0);
						break;
					}

					

				break;

			// Bookmarks
			default:
				if( tBookmark > 0 ) 
					{
					if( Seek7(tBookmark) == 0 ) pCp->SetVisualPlaytime((RMint64)tBookmark);
					else pCp->SetVisualPlaytime((RMint64)0);
					}
				else
					{
					if(pLFBrowser->CurrentFileType== 3)
						Seek7(0);
					pCp->SetVisualPlaytime((RMint64)0);
					}
				break;
			}

		}
	else
		{
#if USE_PROGRESS_PALETTE
		TAVI_LoadPaletteTransparencyData(SUB_PALETTE);
#endif
		VP_SetPlayStatus( __VP_STOP );
		return TAVI_KEY_EXIT_REVERSE; 
		}

	ClearKeyBuffer();

	pVideoPlayer->IsVideoShowState = TRUE;

	return 0;
}	

int VP_MsgProc_FileBrowser( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{

	//////////////////////////////////////////////////////////////////////
	//
	//	VP_MsgProc_FileBrowser
	//	Touch Pad Message Process
	//
	if( ulMsgId == MSG_TOUCHPAD ) {
		if( ssParam1 == 1 ) {
			pLFBrowser->CursorDown();
			pLFBrowser->ReDraw(OSD_REFRESH);
			Reset_FileBrowserTimer();
		}
		else if( ssParam1 == -1 ) {
			pLFBrowser->CursorUp();
			pLFBrowser->ReDraw(OSD_REFRESH);
			Reset_FileBrowserTimer();
		}
		else if( ssParam2 == 1 ) {
		    if( pLFBrowser->GetListItemCount() <= 0 ) {
		        return FILEBROWSER_RETURN_NORMAL;
		    }
			Stop_FileBrowserTimer();
		
			if( pLFBrowser->Select(OSD_REFRESH) ) {
				pCp->SetFilename( pLFBrowser->Filename );

				switch( CheckExtension_UC16( pLFBrowser->Filename ) ) {
					case _VIDEO_FILE:
						ClearOsd();
						return FILEBROWSER_RETURN_FILE_SELECTED;			// Exit DialogBox
						break;

					case _AUDIO_FILE:
						break;

					case _IMAGE_FILE:
						break;

					case _TEXT_FILE:
						break;

					default:
						return 0;
						break;
				}
			}			
			else {
				if( pLFBrowser->GetListItemCount()>0  && 
				    ( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) ) )
					Reset_FileBrowserTimer();
			}			
		}
	}
	//////////////////////////////////////////////////////////////////////
	//
	//	VP_MsgProc_FileBrowser
	//	Key Message Process
	//
	else if( ulMsgId == MSG_BUTTON_UP ) {
		switch( ssParam1 ) {
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_JOG2_UP:			
			case TAVI_KEY_UP:
				if( pLFBrowser->GetListItemCount() > 0 ) {
					pLFBrowser->CursorUp();
					pLFBrowser->ReDraw(OSD_REFRESH);
					if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) )
						Reset_FileBrowserTimer();
				}
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_JOG2_DOWN:
			case TAVI_KEY_DOWN:
				if( pLFBrowser->GetListItemCount() > 0 ) {
					pLFBrowser->CursorDown();
					pLFBrowser->ReDraw(OSD_REFRESH);
					if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) )
						Reset_FileBrowserTimer();
					
				}
				break;
				
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_MODE:	
			case TAVI_KEY_LMODE:
			case TAVI_KEY_SHUTDOWN:
			case TAVI_KEY_LEXIT:
				Stop_FileBrowserTimer();
				pLFBrowser->SaveDirInfo();
				return FILEBROWSER_RETURN_EXIT;
				break;

			case TAVI_KEY_JOG1_ENTER:
				if( g_MiscCtrl->FolderClosed() ) {
					Stop_FileBrowserTimer();
					pLFBrowser->SaveDirInfo();
					return FILEBROWSER_RETURN_EXIT;
				}
				break;
			case TAVI_KEY_LEFT:
				if( pLFBrowser->CursorLeft()==0) {
					Reset_FileBrowserTimer();
					pLFBrowser->ReDraw(OSD_REFRESH);
				}
				break;
			case TAVI_KEY_EXIT:
				if( pLFBrowser->CursorLeft()==0) {
					Reset_FileBrowserTimer();
					pLFBrowser->ReDraw(OSD_REFRESH);
				}
				else {
					pLFBrowser->SaveDirInfo();
					return FILEBROWSER_RETURN_EXIT;
				}
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_RIGHT:
			case TAVI_KEY_ENTER:
			case TAVI_KEY_JOG2_ENTER:
				if( pLFBrowser->GetListItemCount() > 0 )
					{
					int FullpathNameLength;
					Stop_FileBrowserTimer();
					
				
					FullpathNameLength =  ustrlen(pLFBrowser->CurrentFullPathDir) + pLFBrowser->GetCurrentListItemNameLength() + 1;

					// 현재 선택된 파일의 FullPath가 259자를 초과한다면 SIL해제후 이후과정 무시
					if( FullpathNameLength > MAX_FULLPATH_NAME_LENGTH )
						{

						uchar_t ucTemp[128];
						char temp[128];

						pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_OPEN_FAILED_LONG_PATH, -1, -1, 0, DL_BTN_OK_ONLY );
						pMbx->Show();
						ClearKeyBuffer();
						VP_DialogBox_MessagePeeking();
						pMbx->Hide();

						pLFBrowser->ReDraw(OSD_REFRESH);
						return FILEBROWSER_RETURN_NORMAL;
						break;
						}

					if( pLFBrowser->Select(OSD_REFRESH) )
						{

						pCp->SetFilename( pLFBrowser->Filename );

						switch( CheckExtension_UC16( pLFBrowser->Filename ) )
							{
							case _VIDEO_FILE:
								ClearOsd();
								return FILEBROWSER_RETURN_FILE_SELECTED;			// Exit DialogBox
								break;

							case _AUDIO_FILE:
								break;

							case _IMAGE_FILE:
								break;

							case _TEXT_FILE:
								break;

							default:
								return FILEBROWSER_RETURN_NORMAL;
								break;
							}

						}

					else
						{
						
						if( pLFBrowser->GetListItemCount()>0  && ( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) ) )
							Reset_FileBrowserTimer();
						}
					
					}

				break;
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_EQ:
				break;
						
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_MENU:
				if( !pmFileBrowser->IsVisible() ) {
					Stop_FileBrowserTimer();
					pmFileBrowser->Show();
				}
				break;
			}
		}

	return FILEBROWSER_RETURN_NORMAL;
}


int VP_SeekingDoneProcess()
{
	Stop_VideoSeekTimer();
	Seek7( pCp->GetSeekValue() );

	pCp->SetSeekingState(FALSE);
	pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
	return TRUE;
}



static int __ProcEmergencyKey( int ssTaskId, U32 ulMsgId, int ssParam1,int ssParam2 )
{
	if( ssParam1 == TAVI_KEY_HOLD ) {
    if( pLFBrowser->isVisible() || 
        pCp->isVisible || 
        pBookmarkDbx != NULL || 
        pLoadSmiDbx != NULL ) {
        // DO NOTHING!
      }
		else {
			pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);
    }
    return 0;
	}

	return -2;
}


int __ProcComponentKey( int ssTaskId, U32 ulMsgId, int ssParam1,int ssParam2 )
{
	if( pMbx->IsVisible() ) {
		Stop_VideoControlPannelTimer();
		return (pMbx->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2));
	}
	else if( pmFileBrowser->IsVisible() ) {
		Stop_VideoControlPannelTimer();
		return( pmFileBrowser->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
	}
	else if( pLFBrowser->isVisible() ) {
		Stop_VideoControlPannelTimer();
		switch( pLFBrowser->MsgBtnUp( ssTaskId, ulMsgId, ssParam1, ssParam2) )
			{
			case -2:
			case FILEBROWSER_RETURN_EXIT:
				pLFBrowser->Reset();
				return VideoPlayerReturn_EXIT;
				break;

			case FILEBROWSER_RETURN_FILE_SELECTED:
				int ret;
				pLFBrowser->Hide();
				if( (ret=VP_PrepareAndPlay( pLFBrowser->Filename )) == TAVI_KEY_EXIT_REVERSE )
					{
					VP_CommonKeyProcess_EXIT(TASK_ID_VIDEO, _VideoExit_FILEBROWSER);
					}
				else if( ret == TAVI_KEY_LEXIT_REVERSE ||ret == TAVI_KEY_MODE_REVERSE || ret == TAVI_KEY_LMODE_REVERSE)
					{
					pCp->Hide();
					return VideoPlayerReturn_EXIT;
					}
				else
					{
					pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
					}

				return VideoPlayerReturn_NORMAL;
				break;
		}

		return VideoPlayerReturn_NORMAL;
	}
	else if( pBookmarkDbx != NULL ) {
		Stop_VideoControlPannelTimer();
		return ( pBookmarkDbx->MsgBtnUp( ssTaskId, ulMsgId, ssParam1, ssParam2) );
	}
	else if( pSetupDbx->isVisible() ) {
		Stop_VideoControlPannelTimer();
		return ( pSetupDbx->MsgBtnUp( ssTaskId, ulMsgId, ssParam1, ssParam2) );
	}
	else if( pSndPltDbx->IsVisible() ) {
		Reset_SoundPaletteCtrlHideTimer();		
		Stop_VideoControlPannelTimer();
		pSndPltDbx->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2);
		return VideoPlayerReturn_NORMAL;
	}
	else if( pLoadSmiDbx != NULL ) {
		Stop_VideoControlPannelTimer();
		return( pLoadSmiDbx->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
	}
	else if( pSubtitleSyncDbx != NULL && pSubtitleSyncDbx->IsVisible() ) {
		Stop_VideoControlPannelTimer();
		return ( pSubtitleSyncDbx->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
	}		
	else if( pmVideoPlay->IsVisible() ) {
		int ret;
		Stop_VideoControlPannelTimer();

		if( (ret=pmVideoPlay->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2)) == -1 )
			{
			VP_CommonKeyProcess_EXIT(TASK_ID_VIDEO, _VideoExit_NORMAL);
			return VideoPlayerReturn_EXIT;
			}
		else 
			return ret;
	}
	else if( pCp->isVisible ) {
		if( ulMsgId == MSG_BUTTON_UP ) {
			if( Check_VideoSeekTimer() )
				{
				Stop_VideoSeekTimer();
				VP_SeekingDoneProcess();
				}

			Reset_VideoControlPannelTimer();

			return ( pCp->MsgBtnUp( ssTaskId, ulMsgId, ssParam1, ssParam2) );
		}
		else if( ulMsgId == MSG_TOUCHPAD ) {
			Stop_VideoControlPannelTimer();
			return ( pCp->MsgBtnUp( ssTaskId, MSG_TOUCHPAD, ssParam1, ssParam2) );
		}
	}
	else if( ulMsgId == MSG_TOUCHPAD ) {
	 	if( ssParam1 == 0 && ssParam2 == 0 ) return VideoPlayerReturn_NORMAL;
		if( pLFBrowser->isVisible() ) return VideoPlayerReturn_NORMAL;
		if( pBookmarkDbx != NULL ) return VideoPlayerReturn_NORMAL;
		if( pCp->GetSeekable() ) { }

		SetPlayTime();
	 	pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
	 	return VideoPlayerReturn_NORMAL;
	}
	
	return VideoPlayerReturn_NOTUSED;
}
int __ProcKeypad_OnPlay_WithoutControlPannel( int ssTaskId, U32 ulMsgId, int ssParam1,int ssParam2 )
{
	SetPlayTime();

	switch( ssParam1 )
		{
		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_EQ:
			if( (pLFBrowser->CurrentFileType== 4 || pLFBrowser->CurrentFileType== 5 ) ||
				selected_audio_stream_format != 0x55 ) {
				pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
			}
			else {
				if( !mio_enabled() ) {
    				VP_SoundPaletteDbx(NULL);
    			}
    			else {
    			    pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
    			}
			}
			break;

		case TAVI_KEY_LEQ:
			pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);
			break;

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_MENU:
			if( !pmVideoPlay->IsVisible() )
				{
				pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE, CO_SKIP_CAPTION );
				Stop_VideoControlPannelTimer();
				Stop_SoundPaletteCtrlHideTimer();
				Stop_FileBrowserTimer();
				pmVideoPlay->Show();
				}
			break;

		case TAVI_KEY_LMENU:
			if( !pmVideoPlay->IsVisible() )
				pCp->Show();

			break;


		//////////////////////////////////////////////////////////////////////
		// In play mode. UP means [VOLUME DOWN]
		case TAVI_KEY_JOG1_DOWN:
		case TAVI_KEY_DOWN:
		    g_SndCtrl->VolumeDown();
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			break;

		case TAVI_KEY_JOG1_ENTER:
			if( g_MiscCtrl->FolderClosed() )
				{
				if( pTAVI->isVideoPlaying ) VP_CommonKeyProcess_EXIT(ssTaskId, _VideoExit_NORMAL);
				return VideoPlayerReturn_EXIT;
				}
			else
				{
				return (VP_CommonKeyProcess_EXIT(ssTaskId, _VideoExit_FILEBROWSER));
				}

			break;

		//////////////////////////////////////////////////////////////////////
		// In play mode. UP means [VOLUME UP]
		case TAVI_KEY_UP:
		case TAVI_KEY_JOG1_UP:
		    g_SndCtrl->VolumeUp();
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			break;						
								
		case TAVI_KEY_LDOWN:
		case TAVI_KEY_JOG1_LDOWN:
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			VP_CtrlPannelButtonUpProc( ssTaskId, MSG_BUTTON_UP, TAVI_KEY_LDOWN, ssParam2 );
			break;

		case TAVI_KEY_JOG1_LUP:
		case TAVI_KEY_LUP:
			pCp->Show( _CONTROL_PANNEL_VOLUME_MODE );
			VP_CtrlPannelButtonUpProc( ssTaskId, MSG_BUTTON_UP, TAVI_KEY_LUP, ssParam2 );
			break;		
			
		//////////////////////////////////////////////////////////////////////
		// In play mode. ENTER(OK) means [PLAY]/[PAUSE]		
		case TAVI_KEY_JOG2_ENTER:
		case TAVI_KEY_ENTER:
			VP_CommonKeyProcessOK(ssTaskId);
			break;

		//////////////////////////////////////////////////////////////////////
		// In play mode. LEFT means [Reverse]
		case TAVI_KEY_JOG2_DOWN:
		case TAVI_KEY_LEFT:
			pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			VP_CommonKeyProcess_LEFT( ssTaskId );
			break;

		//////////////////////////////////////////////////////////////////////
		// In play mode. RIGHT means [FastFoward]
		case TAVI_KEY_JOG2_UP:
		case TAVI_KEY_RIGHT:
			pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			VP_CommonKeyProcess_RIGHT( ssTaskId );
			break;
			
		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_JOG2_LENTER:
		case TAVI_KEY_LENTER:
			if( pVideoPlayer->LastBookmarkingInSeconds + 5 < (U16)playtime.SecOnly  )
				{
				VP_Bookmarking( pLFBrowser->CurrentFullPathDir,  pLFBrowser->Filename, (U16)playtime.SecOnly, -1 );
				VP_DrawMsgBox( ML_VP_ADD_BOOKMARK );
				}

			break;

		//////////////////////////////////////////////////////////////////////
		// In play mode. LEFT means [Reverse]
		case TAVI_KEY_JOG2_LDOWN:
		case TAVI_KEY_LLEFT:
			pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			VP_CtrlPannelButtonUpProc( ssTaskId, MSG_BUTTON_UP, TAVI_KEY_LLEFT, ssParam2 );
			break;
			
		//////////////////////////////////////////////////////////////////////
		// In play mode. RIGHT means [FastFoward]
		case TAVI_KEY_JOG2_LUP:
		case TAVI_KEY_LRIGHT:
			pCp->Show( _CONTROL_PANNEL_SEEK_MODE );
			VP_CtrlPannelButtonUpProc( ssTaskId, MSG_BUTTON_UP, TAVI_KEY_LRIGHT, ssParam2 );
			break;

		//////////////////////////////////////////////////////////////////////ll
		// In play mode. AB means [Repeat A]
		case TAVI_KEY_AB:
			pCp->MarkingA = playtime.SecOnly;
			pCp->SetCurrentSectionRepeatOptionStatus( SECTION_REPEAT_MARK_A );
			pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
			Stop_VideoControlPannelTimer();
			break;
			
		//////////////////////////////////////////////////////////////////////
		// In Player Mode 		: [EXIT] means [STOP](goto FileBrowser)
		case TAVI_KEY_EXIT:
			return (VP_CommonKeyProcess_EXIT(ssTaskId, _VideoExit_FILEBROWSER));
			break;

		case TAVI_KEY_LEXIT:
		case TAVI_KEY_MODE:
		case TAVI_KEY_LMODE:
			if( pTAVI->isVideoPlaying )
				VP_CommonKeyProcess_EXIT(ssTaskId, _VideoExit_NORMAL);
			return VideoPlayerReturn_EXIT;
			break;

		case TAVI_KEY_SHUTDOWN:
			pTAVI->isShutdownNeeds = TRUE;
			if( pTAVI->isVideoPlaying ) VP_CommonKeyProcess_EXIT(TASK_ID_VIDEO, TAVI_KEY_SHUTDOWN);
			pVideoPlayer->IsEmergencyExitFlagEnabled = TRUE;
			return VideoPlayerReturn_EXIT;
			break;
		}

	return VideoPlayerReturn_NORMAL;
}
int _ProcKeypad( int ssTaskId, U32 ulMsgId, int ssParam1,int ssParam2	)
{
	int ret;

	if( (ret=__ProcEmergencyKey( ssTaskId, ulMsgId, ssParam1, ssParam2 )) > -2 )
		return ret;

	if( (ret=__ProcComponentKey( ssTaskId, ulMsgId, ssParam1, ssParam2 )) > -2 )
		return ret;
	
	else	 if( (ret=__ProcKeypad_OnPlay_WithoutControlPannel( ssTaskId, ulMsgId, ssParam1, ssParam2 )) > -2 )
		return ret;

	return VideoPlayerReturn_NORMAL;
}
int _ProcTouchpad( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	int ret;
	
	if( pCp->GetCurrentSectionRepeatOptionStatus() == SECTION_REPEAT_MARK_A || 
		pCp->GetCurrentSectionRepeatOptionStatus() == SECTION_REPEAT_MARK_AB )
		return VideoPlayerReturn_NORMAL;

	if( (ret=__ProcComponentKey( ssTaskId, ulMsgId, ssParam1, ssParam2 )) > -2 )
		return ret;
}
int _ProcVideoplayerTimer( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( (!pLFBrowser->isVisible()) ) {
		switch ( ssParam2 )
			{
			case TIMER_ID_CP_HIDE:
				if( ssParam1 == 3 && pCp->isVisible == TRUE )
					{ // Control Pannel Auto Hide
					Stop_VideoControlPannelTimer();
					pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
					}
				break;

			case TIMER_ID_VP_SEEK:
				//Video Seek Timer
				if( ssParam1 == 1 ) { VP_SeekingDoneProcess();}
				break;

			case TIMER_ID_VP_SEEK_FREE:
				//Video Seek Free Timer
				if( ssParam1 == 3 ) { pCp->SetSeekable(TRUE);pCp->SetSeekingState(FALSE);}
				break;

			case TIMER_ID_SOUND_PALETTE:
				VP_CloseSoundPaletteDbx();
				break;

			case TIMER_ID_SOUND_PALETTE_TOUCH_APPLY:
				// SoundPalette Timer
				if( ssParam1 == 1 )
					{ // Sound Palette Auto Hide
					Stop_VideoControlPannelTimer();
					Stop_SoundPaletteCtrlHideTimer();
					VP_SetSoundPalette(NULL); // Reset_SoundPaletteCtrlHideTimer will applied in this func.
					}
				break;


			}
		}

		
	else if( (pLFBrowser->isVisible()) && ssParam1==2 && ssParam2== 2 ) {
		if ( pLFBrowser->ProcFBAutoScrollTimer(ssTaskId, ulMsgId, ssParam1, ssParam2) == -2 )
			return VideoPlayerReturn_EXIT;
		}
	return VideoPlayerReturn_NORMAL;
}
void _ProcDeviceNotify( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( 	ssParam1 == NOTIFY_EARPHONE && 
		pLFBrowser->isVisible() == FALSE &&
		pVideoPlayer->IsVideoShowState == TRUE &&
		pBookmarkDbx == NULL &&
		pLoadSmiDbx == NULL )
		{
		if( !pCp->isVisible )
			pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);
		}
}


void _ProcCreate( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( pVideoPlayer->IsVideoPlayerCreated == FALSE )
		{			
		pLFBrowser->SetActiveMode( _FBMODE_VIDEOPLAYER, VP_MsgProc_FileBrowser, TRUE );
		pLFBrowser->Show();
		Reset_FileBrowserTimer();
		pVideoPlayer->IsVideoPlayerCreated = TRUE;
		}
}
void  _PreProcess( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( pVideoPlayer->IsVideoPlayerCreated == FALSE &&
		ulMsgId != MSG_CREATE )
		{
		pLFBrowser->SetActiveMode( _FBMODE_VIDEOPLAYER, VP_MsgProc_FileBrowser, TRUE );
		pLFBrowser->Show();
		Reset_FileBrowserTimer();
		pVideoPlayer->IsVideoPlayerCreated = TRUE;
		}
	else if( ulMsgId == MSG_BATTERY && IsBatteryLevelChanged(ssParam1) )
		{

		if( IsControlPannelVisible() )
			{
			pVideoPlayer->isControlPannelVisible = TRUE;
			}

		else if( pLFBrowser->isVisible() )
			{
			}
		}
}

int VP_MsgProc( 	int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2	)
{
	_PreProcess( ssTaskId, ulMsgId, ssParam1, ssParam2 );

	switch( ulMsgId )
		{
		case MSG_CREATE:
			_ProcCreate( ssTaskId, ulMsgId, ssParam1, ssParam2 );
			break;
			
		case MSG_DRAW:
			break;

		case MSG_TIMER:
			if( _ProcVideoplayerTimer( ssTaskId, ulMsgId, ssParam1, ssParam2 ) < 0 ) 
				return VideoPlayerReturn_EXIT;
			break;

		case MSG_NOTIFY:
			_ProcDeviceNotify( ssTaskId, ulMsgId, ssParam1, ssParam2 );
			break;

		case MSG_TOUCHPAD:
			return _ProcTouchpad( ssTaskId, ulMsgId, ssParam1, ssParam2 );
			break;

		case MSG_BUTTON_UP:
			return _ProcKeypad( ssTaskId, ulMsgId, ssParam1, ssParam2 );
			break;
		}

	return VideoPlayerReturn_NORMAL;
}

void _ProcessSectionMarking_A()
{
	if( ((RMint32)(playtime.SecOnly)&0x00000001 ) == 1 )
		{
		if( pCp->RepeatOptionStatusCounter == 0 )
			{
			pCp->RepeatOptionStatusCounter = 1;
			pCp->BuildSectionRepeatOptionStatus( SECTION_REPEAT_DISABLE );
			pMpegDecoder->RefreshOsd();
			}
		}
	else
		{
		if( pCp->RepeatOptionStatusCounter == 1 )
			{
			pCp->RepeatOptionStatusCounter = 0;
			pCp->BuildSectionRepeatOptionStatus( SECTION_REPEAT_MARK_A );
			pMpegDecoder->RefreshOsd();
			}						
		}
}

void _ProcessSectionMarking_AB()
{
	if( playtime.SecOnly >= pCp->MarkingB ) {
	  playtime.SecOnly = pCp->MarkingA;
		Seek7((int)pCp->MarkingA);
		Stop_VideoControlPannelTimer();
	}


	if( ((RMint32)(playtime.SecOnly)&0x00000001 ) == 1 )
		{
		if( pCp->RepeatOptionStatusCounter == 0 )
			{
			pCp->RepeatOptionStatusCounter = 1;
			pCp->BuildSectionRepeatOptionStatus( SECTION_REPEAT_DISABLE );
			pMpegDecoder->RefreshOsd();
			}
		}
	else
		{
		if( pCp->RepeatOptionStatusCounter == 1 )
			{
			pCp->RepeatOptionStatusCounter = 0;
			pCp->BuildSectionRepeatOptionStatus( SECTION_REPEAT_MARK_AB );
			pMpegDecoder->RefreshOsd();
			}						
		}
}

void _ProcessCheckVideoInit()
{
	pVideoPlayer->IsVideoScreenInitialized = TRUE;
}

int _ProcessVideoRepeat_CurrentDir( int ssTaskId, int option )
{
	RMuint32 ret;
	int ResumeValue =  toggle_display;

	Stop_VideoControlPannelTimer();
	Stop_SoundPaletteCtrlHideTimer();

	ClearOsd();
	clearbackground();
	pSndPltDbx->Hide(SKIP_OSD_REFRESH);
	pmVideoPlay->Hide( FALSE, FALSE );

	VP_RecoveryDisplayRatio();

	ustrcpy( FullPath, pLFBrowser->CurrentFullPathDir );
	ustrcat( FullPath, sSlash );
	ustrcat( FullPath, pLFBrowser->Filename );

	
	usleep(10);
	pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );


	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(VIDEO_LOADING_PROGRESS_PALETTE);
	ClearOsd();
	pMpegDecoder->RefreshOsd();
	toggle_display = ResumeValue;
	pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);

	if( (ret=MAIN_RenderVideoFile( FullPath, pLFBrowser->CurrentFileType, &cbtVideo)) == ERR_VR_NOERROR)
		{
		DrawLoadingProgressV4(5000,0);
		pMpegDecoder->TAVI_InitSystemPalette();
		TAVI_LoadPaletteTransparencyData(SUB_PALETTE);
		ClearOsd();
		pMpegDecoder->RefreshOsd();
		pVideoPlayer->IsVideoShowState = TRUE;

        // Load caption file.
		VP_PrepareSMI( FullPath );
		
		Seek7(0);
		pCp->SetVisualPlaytime((RMint64)0);
		VP_SetPlayStatus( __VP_PLAY );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
		
		if( (pLFBrowser->CurrentFileType== 4 || pLFBrowser->CurrentFileType== 5 ) ||
			selected_audio_stream_format != 0x55 )
			{
			pmVideoPlay = pVideoMenu;
			TaviSysParam.sound.eq = (int)SOUND_NORMAL;
			}

		Seek7(0);
		}
	else
		{
		VP_SetPlayStatus( __VP_STOP );

		if( ret == ERR_VR_RES_OVER )
			{
			pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
			pMpegDecoder->DisplayYUV420( 0, 0, 0, 0, 0, 0, 0, 0 );
			VP_CommonKeyProcess_EXIT(pVideoPlayer->TaskId, _VideoExit_FILEBROWSER);
			}
		else 
			{
			VP_CommonKeyProcess_EXIT(ssTaskId, _VideoExit_NORMAL);
			}
		}

	return VideoPlayerReturn_NORMAL;
}

int _CommonProcess_RendererEnclose( TTaskInitParam* tParam, TMsgEntry* ptMsg )
{
	Stop_VideoControlPannelTimer();
	pCp->ClearVisualPlaytime();
	pmVideoPlay->Hide(FALSE);

	playtime.SecOnly = 0;
	switch( (eVP_PLAY_OPTION)TaviSysParam.video.playlist_repeat )
		{
		case _PLAY_ONCE:	// Play Once
			if( VP_CommonKeyProcess_EXIT( tParam->ssTaskId, _VideoExit_FILEBROWSER) < 0 ) 
				return -1;
			break;

		case _REPEAT_CURRENTDIR:	// Repeat Current Folder
		default:

			if( VP_PrepareToCurrentDirRepeat(0) == FALSE )
				{
				if( VP_CommonKeyProcess_EXIT( tParam->ssTaskId, _VideoExit_FILEBROWSER) < 0 ) 
					return -1;
				}	
			else
				{
				if( _ProcessVideoRepeat_CurrentDir( tParam->ssTaskId, _VideoExit_NORMAL ) < 0 )
					return -1;
				}
			break;
		}

	return TRUE;
}

static int preState = -1;
int _CommonProcess_AB_Repeat()
{

	preState = (int)pCp->GetCurrentSectionRepeatOptionStatus();

	if( pCp->GetCurrentSectionRepeatOptionStatus() == SECTION_REPEAT_MARK_A )
		_ProcessSectionMarking_A();
	else	 if( pCp->GetCurrentSectionRepeatOptionStatus() == SECTION_REPEAT_MARK_AB )
		_ProcessSectionMarking_AB();
	else if( pSubtitle != NULL ) {
		VP_ChunkSMI();
	}

	return TRUE;
}


static int 
VP_TaskStep2( TTaskInitParam* tParam, TMsgEntry* ptMsg )
{
	RMuint32 ret;

	for(;;) {
		if( pVideoPlayer->IsEmergencyExitFlagEnabled ) {
			VP_CommonKeyProcess_EXIT(TASK_ID_VIDEO, _VideoExit_NORMAL);
			break;
		}
		
		if( MSG_Peek_Ex( tParam->ssTaskId, ptMsg, MSG_PEEK_ALL ) == 0) {
			if( VP_MsgProc( ptMsg->ssToTaskId, ptMsg->ulMsgId, ptMsg->usParam1, ptMsg->usParam2 ) == VideoPlayerReturn_EXIT)
				{
				// break means End of VideoPlayer Task
				break;
				}
		}

		if( pVideoPlayer->Status ==__VP_PLAY ) {
			if( !pVideoPlayer->IsVideoScreenInitialized ) {
				_ProcessCheckVideoInit();
			}

			if( pLFBrowser->CurrentFileType  == _AVI_ADV || 	// DivX, XviD
				pLFBrowser->CurrentFileType  == _MPEG )	// MPEG1, MPEG2
				_CommonProcess_AB_Repeat();


			if( (ret=fpVideoRenderer( &cbtVideo )) == 1 )
				if(_CommonProcess_RendererEnclose(tParam, ptMsg)==-1 )
					return -1;
    }

    mio_schedule();
	}
	
	return TRUE;
}

int BuildProgressBar( int eDrawMethod )
{
	return (  pCp->BuildProgressBar( eDrawMethod )  );
}

void SetSeekable(int input)
{
	pCp->SetSeekable(input);
}

int GetSeekable()
{
	return pCp->GetSeekable();
}

int IsSeekingState()
{
	return (pCp->IsSeekingState());
}

static int CaptionSelector( RMuint8* arg )
{
  CPopUpSubMenu* psub = ( CPopUpSubMenu* )arg;

  TaviSysParam.video.multi_caption = psub->SelectedMenuItemIdx;
  pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);

  return 0;
}

int VP_FileBrowserNameAscentSort( RMuint8* parameter )
{
	pLFBrowser->RefreshForSortingMethodChange( fpNameAscentSort, OSD_REFRESH);
	Reset_FileBrowserTimer();
	return TRUE;
}
int VP_FileBrowserNameDescentSort( RMuint8* parameter )
{
	pLFBrowser->RefreshForSortingMethodChange( fpNameDescentSort, OSD_REFRESH );
	Reset_FileBrowserTimer();
	return TRUE;
}
int VP_FileBrowserTimeAscentSort( RMuint8* parameter )
{
	pLFBrowser->RefreshForSortingMethodChange( fpTimeAscentSort, OSD_REFRESH );
	Reset_FileBrowserTimer();
	return TRUE;
}
int VP_FileBrowserTimeDescentSort( RMuint8* parameter )
{
	pLFBrowser->RefreshForSortingMethodChange( fpTimeDescentSort, OSD_REFRESH );
	Reset_FileBrowserTimer();
	return TRUE;
}


int VP_HideSubmenu( RMuint8* parameter )
{
	pmVideoPlay->Hide(0);
  printf( "hide\n" );
	Reset_VideoControlPannelTimer();

	return TRUE;
}

static int VP_Add_Bookmark( RMuint8* parameter )
{
  VP_HideSubmenu();

  VP_DrawMsgBox( ML_VP_ADD_BOOKMARK );
    
	VP_Bookmarking( 
	    pLFBrowser->CurrentFullPathDir,  
	    pLFBrowser->Filename, 
	    (U16)playtime.SecOnly, 
	    -1 );

	return TRUE;
}

static int VP_Show_BookmarkDbx( RMuint8* parameter )
{
	eVIDEOPLAYER_CONTROL_ICONS PreVideoStatus;

	PreVideoStatus = pCp->GetPlayStatus();

	if( !(pCp->GetSeekable()) ) {
		VP_HideSubmenu();
		return FALSE;
	}

  // Foreward process wakes up a HDD.
  mio_wakeup( true ); 
    
	// Bookmark
	Stop_VideoControlPannelTimer();
	mep_eat_event();
	pMpegDecoder->Pause();
	VP_SetPlayStatus( __VP_PAUSE );
	pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
	pMpegDecoder->RenderText(0, 0, 0, 35, 320, 205 ); 
	g_TopPanel->SetVisible( true );
	g_TopPanel->Update();

	pBookmarkDbx = new CBookmarkDialogBox( ML_BOOKMARK, 10-1, 35-1, 320-(10*2)-1, 240-35-10-1, VP_MsgboxButtonUpProc_Bookmark, (U32)playtime.SecOnly, ID_SELECT|ID_DELETE|ID_CANCEL|_BOOKMARK_DBX_AT_PLAYING); 
	VP_BookmarkLoading( pLFBrowser->CurrentFullPathDir, pLFBrowser->Filename );
	pBookmarkDbx->Display();

	ClearKeyBuffer();
	SetTaviKeyMode(TK_REPEAT|TK_VARIOUS);
	VP_DialogBox_MessagePeeking();
	SetTaviKeyMode(TK_RESUME);
	ClearKeyBuffer();

	t = pBookmarkDbx->GetSecondsToSave();

	// Save This Position
	if( pBookmarkDbx->GetCurrentChoice() == -1  &&  t > 0 ) {
		int exitTimer;
		exitTimer = gettime_sec();
		VP_DrawMsgBox(2);
		VP_Bookmarking( pLFBrowser->CurrentFullPathDir,  pLFBrowser->Filename, t, -1 );

		for(;;) {
			if( (gettime_sec()-exitTimer) > 2 ) 
				break;
		}
	}
	else {
		int ret = pBookmarkDbx->GetCurrentChoice();

		if( ret >= 0 ) {
			// Play Saved Bookmark
			t = pBookmarkDbx->GetSeekPoint();
			Seek7(t);
		}
		else {
			// Exit
			switch( pBookmarkDbx->GetCurrentChoice() ) {
			case TAVI_KEY_MODE_REVERSE:
			case TAVI_KEY_LMODE_REVERSE:
			case TAVI_KEY_LEXIT_REVERSE:
				pVideoPlayer->IsEmergencyExitFlagEnabled = TRUE;
				break;
				
			case TAVI_KEY_EXIT_REVERSE:
				break;
			}
	
		}
	}

	delete pBookmarkDbx;
	pBookmarkDbx = NULL;
	
	ClearDbxOsd();

	if( PreVideoStatus == _VCTRL_PAUSE ) {
		pMpegDecoder->Pause();
		VP_SetPlayStatus( __VP_PAUSE );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
	}
	else {
		pMpegDecoder->Play();
		VP_SetPlayStatus( __VP_PLAY );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
    }		

	VP_HideSubmenu();	
 	return TRUE;
}

int VP_SubtitleEnabled( RMuint8* parameter )
{
  CPopUpSubMenu* psub;
  const SAMILang* lang;
  int ccnt;

	isCaptionEnabled = TRUE;
	pCp->SetCurrenSubtitleOptionStatus(CAPTION_ENABLED);
  pVideoMenu->ChangeSubMenu( 
    ML_SUBTITLE, psSubtitleEnabled, 0, NULL, NULL ); 
  pVideoMenu->DeleteSubMenu( ML_CAP_LANG );
	psub = new CPopUpSubMenu;

  ccnt = pSubtitle->GetLanguages();
  

  int langid;
	for( int i=0; i<ccnt; i++ ) {
	  lang = pSubtitle->GetLanguage( i );
	  if( !strcmp(lang->langname, "korean") ) {
	    langid = ML_KOREA;
	  }
	  else if( !strcmp(lang->langname, "english") ) {
	    langid = ML_USA;
	  }
	  else {
	    langid = ML_UNKNOWN;
	  }
	  psub->AddSubMenuItem( 
	      langid, TRUE, CaptionSelector, 
	      (RMuint8*)psub );
	}

	if( ccnt > 1 ) {
    psub->AddSubMenuItem(
      ML_ALL_CAPTIONS, TRUE, CaptionSelector, 
      (RMuint8*)psub );
  }

	pVideoMenu->InsertSubMenu( 
	              ML_SUBTITLE,
	              ML_CAP_LANG, psub, 
	              TaviSysParam.video.multi_caption, 
	              NULL, NULL );	

	if( parameter == NULL || *(parameter) == OSD_REFRESH )
		pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);
	Reset_VideoControlPannelTimer();
	return TRUE;
}

int VP_SubtitleDisabled( RMuint8* parameter )
{
	isCaptionEnabled = FALSE;
	pVideoMenu->DeleteSubMenu( ML_CAP_LANG );
	pCp->SetCurrenSubtitleOptionStatus(CAPTION_DISABLED);
	pVideoMenu->ChangeSubMenu( ML_SUBTITLE, psSubtitleDisabled, 1, NULL, NULL );
	if( parameter == NULL || *(parameter) == OSD_REFRESH )
		pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);
	Reset_VideoControlPannelTimer();
	return TRUE;
}
int VP_SubtitleNotFound( RMuint8* parameter )
{
	isCaptionEnabled = FALSE;
	pVideoMenu->DeleteSubMenu( ML_CAP_LANG );
	pCp->SetCurrenSubtitleOptionStatus(CAPTION_NOTFOUND);
	pVideoMenu->ChangeSubMenu( ML_SUBTITLE, psSubtitleNotFound, 0, NULL, NULL );	
	if( parameter == NULL || *(parameter) == OSD_REFRESH )
		pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);
	Reset_VideoControlPannelTimer();
	return TRUE;
}

static int VP_LoadSubtitle( RMuint8* parameter )
{
	uchar_t temp[260];
	uchar_t sSlash[2] = {0x002f, 0x0000};	
	eVIDEOPLAYER_CONTROL_ICONS PrevStatus;

  // wakes up a HDD.
  mio_wakeup();	

	// Prepare For Dbx
	Stop_VideoControlPannelTimer();
	mep_eat_event();
	PrevStatus = pCp->GetPlayStatus();
	pMpegDecoder->Pause();
	VP_SetPlayStatus( __VP_PAUSE );
	pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
	pMpegDecoder->RenderText(0, 0, 0, 35, 320, 205 ); 
	g_TopPanel->SetVisible( true );
	g_TopPanel->Update();
	
	pLoadSmiDbx = new CFileDbx( ML_SUBTITLE, 10-1, 35-1, 320-(10*2)-1, 240-35-10-1, pLFBrowser->CurrentFullPathDir, "smi", (ID_OK|ID_CANCEL) );
	pLoadSmiDbx->Show();
	Stop_VideoControlPannelTimer();
	
	
	VP_DialogBox_MessagePeeking();

	if( pLoadSmiDbx->GetCurrentChoice() == TAVI_KEY_EXIT_REVERSE ) {
		delete pLoadSmiDbx;
		pLoadSmiDbx = NULL;
	}
	else {
		VP_CloseSMI();
		
		ustrcpy( temp, pLFBrowser->CurrentFullPathDir );
		ustrcat( temp, sSlash );
		ustrcat( temp, pLoadSmiDbx->GetFilenamePtr() );

		delete pLoadSmiDbx;
		pLoadSmiDbx = NULL;

		VP_PrepareSMI( temp, 1 );

		ClearDbxOsd();
	}

	if( PrevStatus == _VCTRL_PAUSE ) {
		VP_SetPlayStatus( __VP_PAUSE );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
	}
	else {
		VP_SetPlayStatus( __VP_PLAY );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
		pMpegDecoder->Play();
	}


	// Prepare For Return
	VP_HideSubmenu();	
	
	return TRUE;
}


int VP_AdjustSubtitleSync( RMuint8* parameter )
{
	eVIDEOPLAYER_CONTROL_ICONS PrevStatus;

	// Prepare For Dbx
	Stop_VideoControlPannelTimer();
	SetTaviTouchpadMode( MEP_REPORT_RELATIVE );
	mep_eat_event();

	PrevStatus = pCp->GetPlayStatus();

	pMpegDecoder->RenderText(0, 0, 0, 35, 320, 205 ); 
	pSubtitleSyncDbx = new CLibSubtitleSyncDbx( 
	      ML_ADJ_SUBTITLE_TIME_SYNC, 10-1, 35-1, 320-(10*2)-1, 120, 
	      pSubtitle->GetSyncSecShiftValue(), (ID_OK|ID_CANCEL) );
	pSubtitleSyncDbx->Show();
	Stop_VideoControlPannelTimer();

	VP_DialogBox_MessagePeeking();	

	delete pSubtitleSyncDbx;
	pSubtitleSyncDbx = NULL;

	ClearDbxOsd();

	if( PrevStatus == _VCTRL_PAUSE )
		{
		VP_SetPlayStatus( __VP_PAUSE );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
		}
	else
		{
		VP_SetPlayStatus( __VP_PLAY );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
		pMpegDecoder->Play();
		}

	VP_HideSubmenu();	

	return TRUE;
}


int VP_RepeatOff( RMuint8* parameter )
{
	TaviSysParam.video.playlist_repeat = 0;
	pmVideoPlay->ChangeSubMenu( ML_BOUNDARY, psRepeat, (int)TaviSysParam.video.playlist_repeat, NULL, NULL );
	pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);
	return TRUE;
}

int VP_RepeatFolder( RMuint8* parameter )
{
	if(TaviSysParam.video.playlist_repeat != 1)
			pLFBrowser->RememberDirRepeatStartPoint();

	TaviSysParam.video.playlist_repeat = 1;

	pmVideoPlay->ChangeSubMenu( ML_BOUNDARY, psRepeat, (int)TaviSysParam.video.playlist_repeat, NULL, NULL );
	pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);
	return TRUE;
}

int VP_CloseSoundPaletteDbx()
{
	Stop_VideoControlPannelTimer();
	Stop_SoundPaletteCtrlHideTimer();

	if( pCp->isVisible ) {
		pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 1 );
	}

	pSndPltDbx->Hide(SKIP_OSD_REFRESH);
	pMpegDecoder->RefreshOsd();
	return TRUE;
}


int VP_SoundPaletteDbx( RMuint8* parameter )
{
	Stop_VideoControlPannelTimer();
	Reset_SoundPaletteCtrlHideTimer();
	SetTaviTouchpadMode( MEP_REPORT_RELATIVE );
	mep_eat_event();

	pCp->Hide( &(pVideoPlayer->isControlPannelVisible), 0 );	
	pmVideoPlay->Hide(0);

	pSndPltDbx->Show();

	return TRUE;
}


void VP_RecoveryDisplayRatio()
{
	pMpegDecoder->SetupDisplay( toggle_display, (RMint32*)&TaviSysParam.output );
}


int VP_SetScreenDisplayRatio()
{
///////////////////////////////////////////////////////////////////////////////
// 

	RMint32 w, h, n, d, x, y, w1, h1;
	RMint32 i32DispSX, i32DispSY;			
	RMint32 i32DisplayWidth, i32DisplayHeight;

	i32DispSX = TaviSysParam.output[toggle_display].x;
	i32DispSY = TaviSysParam.output[toggle_display].y;
	i32DisplayWidth = TaviSysParam.output[toggle_display].w;
	i32DisplayHeight = TaviSysParam.output[toggle_display].h; 

	dbg("VP_SetScreenDisplayRatio()\n");
	dbg("-i32DispSX : %d\n", i32DispSX );
	dbg("-i32DispSY : %d\n", i32DispSY );
	dbg("-i32DisplayWidth : %d\n", i32DisplayWidth );
	dbg("-i32DisplayHeight : %d\n", i32DisplayHeight );
	dbg("-gVideofilePlayinfo.OriginalWidth : %d\n", gVideofilePlayinfo.OriginalWidth );
	dbg("-gVideofilePlayinfo.OriginalHeight : %d\n", gVideofilePlayinfo.OriginalHeight );


	gVideofilePlayinfo.w1 = gVideofilePlayinfo.OriginalWidth * 100 / i32DisplayWidth;
	gVideofilePlayinfo.h1 = gVideofilePlayinfo.OriginalHeight * 100 / i32DisplayHeight;

	dbg("-gVideofilePlayinfo.w1 : %d\n", gVideofilePlayinfo.w1 );
	dbg("-gVideofilePlayinfo.h1 : %d\n", gVideofilePlayinfo.h1 );

  if( gVideofilePlayinfo.w1 >= gVideofilePlayinfo.h1) {
  	// for NORMAL
  	gVideofilePlayinfo.width			= i32DisplayWidth;
  	gVideofilePlayinfo.height 			= gVideofilePlayinfo.OriginalHeight  * i32DisplayWidth / gVideofilePlayinfo.OriginalWidth;
  	gVideofilePlayinfo.x 				= i32DispSX;
  	gVideofilePlayinfo.y				= i32DispSY + ((i32DisplayHeight/2) - (gVideofilePlayinfo.height/2));
	}
  else if( gVideofilePlayinfo.w1 < gVideofilePlayinfo.h1) {
  	// for NORMAL
  	gVideofilePlayinfo.width			= gVideofilePlayinfo.OriginalWidth * i32DisplayHeight / gVideofilePlayinfo.OriginalHeight ;
  	gVideofilePlayinfo.height 			= i32DisplayHeight;
  	gVideofilePlayinfo.x 				= i32DispSX + ((i32DisplayWidth/2) - (gVideofilePlayinfo.width/2));	// 630/2 - 597/2 
  	gVideofilePlayinfo.y				= i32DispSY;
 	}
 	printf( "normal>> x: %d, y: %d, w: %d, h: %d\n",
 	        gVideofilePlayinfo.x, gVideofilePlayinfo.y,
 	        gVideofilePlayinfo.width, gVideofilePlayinfo.height );
				
	switch( TaviSysParam.video.screen ) {
		case 0:	// Normal
		default:
			dbg("- Normal : SetVideoDestination (%d, %d, %d, %d)\n", 
				gVideofilePlayinfo.x,
				gVideofilePlayinfo.y, 
				gVideofilePlayinfo.width, 
				gVideofilePlayinfo.height
				);

			pMpegDecoder->SetVideoDestination ( 
				gVideofilePlayinfo.x,  gVideofilePlayinfo.y,
				gVideofilePlayinfo.width,  gVideofilePlayinfo.height );
			pCp->SetScreenRateType( _VSR_NORMAL );

			break;
			
		case 1:	// Zoom

			if( gVideofilePlayinfo.w1 >= gVideofilePlayinfo.h1)
				{
				// for ZOOM
				gVideofilePlayinfo.OptimizedWidth	= gVideofilePlayinfo.OriginalWidth * i32DisplayHeight / gVideofilePlayinfo.OriginalHeight ;
				gVideofilePlayinfo.OptimizedHeight 	= i32DisplayHeight;
				gVideofilePlayinfo.OptimizedX 		= i32DispSX + ((i32DisplayWidth/2) - (gVideofilePlayinfo.OptimizedWidth/2));	// 630/2 - 597/2 
				gVideofilePlayinfo.OptimizedY		= i32DispSY;
				}
			
			else if( gVideofilePlayinfo.w1 < gVideofilePlayinfo.h1)
				{
				// for ZOOM
				gVideofilePlayinfo.OptimizedWidth	= i32DisplayWidth;
				gVideofilePlayinfo.OptimizedHeight 	= gVideofilePlayinfo.OriginalHeight  * i32DisplayWidth / gVideofilePlayinfo.OriginalWidth;
				gVideofilePlayinfo.OptimizedX 		= i32DispSX;
				gVideofilePlayinfo.OptimizedY		= i32DispSY + ((i32DisplayHeight/2) - (gVideofilePlayinfo.OptimizedHeight/2));
				}
			
			dbg("- Zoom : SetVideoDestination (%d, %d, %d, %d)\n", 
				gVideofilePlayinfo.OptimizedX,
				gVideofilePlayinfo.OptimizedY, 
				gVideofilePlayinfo.OptimizedWidth, 
				gVideofilePlayinfo.OptimizedHeight );

			pMpegDecoder->SetVideoDestination ( 
  			  gVideofilePlayinfo.OptimizedX,  
  			  gVideofilePlayinfo.OptimizedY,  
  			  gVideofilePlayinfo.OptimizedWidth,  
  			  gVideofilePlayinfo.OptimizedHeight );
			pCp->SetScreenRateType( _VSR_OPTIMIZED );
			break;

		case 2:	// Full Screen
			DEBUGMSG(1,("- FullScreen : SetVideoDestination (%d, %d, %d, %d)\n", i32DispSX, i32DispSY, i32DisplayWidth, i32DisplayHeight ));
			pMpegDecoder->SetVideoDestination (i32DispSX, i32DispSY, i32DisplayWidth, i32DisplayHeight);
			pCp->SetScreenRateType( _VSR_FULLSCREEN );
			break;

    case _DISPLAY_16X9_OPTIMIZED:
      {
      float r = gVideofilePlayinfo.width / gVideofilePlayinfo.height;
      if( r < 1.78 ) {
        return FALSE;
      }
      int x, y, w, h;
      h = i32DisplayWidth*9/16;
      w = gVideofilePlayinfo.OriginalWidth*h/gVideofilePlayinfo.OriginalHeight;
      x = (i32DisplayWidth-w)/2;
      x += (4-x%4);
      y = (i32DisplayHeight-h)/2;
      y += (4-y%4);
			pMpegDecoder->SetVideoDestination( x, y, w, h );
      }
      break;
      
		case _DISPLAY_16X9_FULL:
		  {
		  int x = gVideofilePlayinfo.x;
		  int y = gVideofilePlayinfo.y;
		  int w = gVideofilePlayinfo.width;
		  int h = gVideofilePlayinfo.height;
		  h = w*9/16;
		  h += (16-(h%16));
		  y = (240-h)/2;
		  pMpegDecoder->SetVideoDestination( x, y, w, h );
		  }
		  break;

		}

		return TRUE;
}

static void 
screen_ratio( int mode )
{
  int old;
  old = TaviSysParam.video.screen;
  TaviSysParam.video.screen = mode;
  if( VP_SetScreenDisplayRatio() == FALSE ) {
    TaviSysParam.video.screen = old;
  }
  pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);
}

int VP_ScreenNormal( RMuint8* parameter )
{	
  screen_ratio( _DISPLAY_NORMAL );
	return TRUE;
}
int VP_ScreenZoom( RMuint8* parameter )
{
  screen_ratio( _DISPLAY_ZOOM );
	return TRUE;
}
int VP_ScreenFullScreen( RMuint8* parameter )
{
  screen_ratio( _DISPLAY_FULLSCREEN );
	return TRUE;
}

static int
ratio16x9_optimized( RMuint8* arg )
{
  screen_ratio( _DISPLAY_16X9_OPTIMIZED );
	return TRUE;
}

static int
ratio16x9_full( RMuint8* arg )
{
  screen_ratio( _DISPLAY_16X9_FULL );
  return TRUE;
}

int VP_VideoLcdOut( RMuint8* parameter )
{
	if( toggle_display == 0 )
		return FALSE;

	TaviSysParam.video.video_output = _VIDEO_LCD_OUT;
	toggle_display = 0;					// lcd
	pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
	lcd_enable( g_MiscCtrl->GetValueCtrl_Out() );
	g_MiscCtrl->MainLCDOnOff( TRUE );
	VP_SetScreenDisplayRatio();
	pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);

	return TRUE;
}
int VP_VideoCompositeOut( RMuint8* parameter )
{
	TaviSysParam.video.video_output = _VIDEO_COMPOSITE_OUT;

	g_MiscCtrl->MainLCDOnOff( FALSE );
	lcd_disable();
	if ( TaviSysParam.etc.tvtype == 0 )	// NTSC 
		toggle_display = 1;				// composite-ntsc
	else
		toggle_display = 2;				// composite-pal
		
	pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);
	VP_SetScreenDisplayRatio();

	pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);

}
int VP_VideoComponentOut( RMuint8* parameter )
{
	TaviSysParam.video.video_output = _VIDEO_COMPONENT_OUT;

	g_MiscCtrl->MainLCDOnOff( FALSE );
	lcd_disable();
	if ( TaviSysParam.etc.tvtype == 0 )		// NTSC
		toggle_display = 3;				// component-ntsc
	else
		toggle_display = 4;;				// component-pal

	pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);

	VP_SetScreenDisplayRatio();

	pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);

}
int VP_VideoHdComponentOut( RMuint8* parameter )
{
	TaviSysParam.video.video_output = _VIDEO_HD_COMPONENT_OUT;

	g_MiscCtrl->MainLCDOnOff( FALSE );
	lcd_disable();
	if ( TaviSysParam.etc.hdcomponent == 2 )
		toggle_display = 7;				
	else if ( TaviSysParam.etc.hdcomponent == 1 )
		toggle_display = 6;				
	else
		toggle_display = 5;;				

	pMpegDecoder->SetupDisplay (toggle_display, (RMint32 *)&TaviSysParam.output);

	VP_SetScreenDisplayRatio();

	pCp->Show(_CONTROL_PANNEL_PROGRESS_MODE);

}
int VP_UpdateVideoOutput( RMuint8* parameter )
{
	int OptValue;

	if( toggle_display == 5 || toggle_display == 6 || toggle_display == 7 )
		OptValue = 3;
	else if( toggle_display == 3 || toggle_display == 4 )
		OptValue = 2;
	else if( toggle_display == 1 || toggle_display == 2 )
		OptValue = 1;
	else
		OptValue = 0;
	
	pVideoMenu->ChangeSubMenu( ML_VIDEO_OUTPUT, psVideoOutput, OptValue, VP_UpdateVideoOutput, NULL );

	return TRUE;
}


int VP_SetBrightnessAndContrast( RMuint8* parameter )
{
	uchar_t temp[260];
	uchar_t sSlash[2] = {0x002f, 0x0000};
	eVIDEOPLAYER_CONTROL_ICONS PrevStatus;
	
	DEBUGMSG(1,("\nVP_SetBrightnessAndContrast()\n"));

	// Prepare For Dbx
	Stop_VideoControlPannelTimer();
	SetTaviTouchpadMode( MEP_REPORT_RELATIVE );
	mep_eat_event();

	PrevStatus = pCp->GetPlayStatus();

	pMpegDecoder->Pause();
	VP_SetPlayStatus( __VP_PAUSE );
	pMpegDecoder->RenderText(0, 0, 0, 35, 320, 205 ); 


	pSetupDbx->Show();
	Stop_VideoControlPannelTimer();
	
	DEBUGMSG(1,("\nVP_SetBrightnessAndContrast() : Before VP_DialogBox_MessagePeeking()\n"));

	ClearKeyBuffer();
	SetTaviKeyMode(TK_REPEAT|TK_VARIOUS);
	VP_DialogBox_MessagePeeking();
	SetTaviKeyMode(TK_RESUME);
	ClearKeyBuffer();

	switch( pSetupDbx->GetCurrentChoice() )
		{
		case TAVI_KEY_MODE:
		case TAVI_KEY_LMODE:
		case TAVI_KEY_LEXIT:
//			printf("VP_SetBrightnessAndContrast()\n-ret_choice:0x%x TAVI_KEY_MODE|TAVI_KEY_LMODE\n", ret_choice);
			pVideoPlayer->IsEmergencyExitFlagEnabled = TRUE;
			break;

		case TAVI_KEY_EXIT_REVERSE:
//			printf("VP_SetBrightnessAndContrast()\n-ret_choice:0x%x(TAVI_KEY_EXIT_REVERSE)\n", ret_choice);
			pSetupDbx->Cancel();
			break;

//		default:
//			printf("VP_SetBrightnessAndContrast()\n-ret_choice:0x%x\n", ret_choice);
			
		}

	pSetupDbx->Hide();

	if( PrevStatus == _VCTRL_PAUSE )
		{
		VP_SetPlayStatus( __VP_PAUSE );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
		}
	else
		{
		VP_SetPlayStatus( __VP_PLAY );
		pCp->Show( _CONTROL_PANNEL_PROGRESS_MODE );
		pMpegDecoder->Play();
		}

	DEBUGMSG(1,("\nVP_LoadSubtitle() : After VP_DialogBox_MessagePeeking()\n"));
	VP_HideSubmenu();	
	return TRUE;
}

void __vpInit_Palette()
{
	//////////////////////////////////////////////////////////////////////////////
	//Video Palette Setting
	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(TRUE);
	ClearOsd();
	pMpegDecoder->RefreshOsd();
}

void __vpInit_LocalConfig()
{
	// Video Player Local Configuratrion Init
	pVideoPlayer						= (TVIDEOPLAYER_CONFIG*) new TVIDEOPLAYER_CONFIG;
	pVideoPlayer->TaskId				= TASK_ID_VIDEO;
	pVideoPlayer->Status					= __VP_NEW;
	pVideoPlayer->CurrentFullPathDir		=  aCurrentFullPathDir;
	pVideoPlayer->CurrentFullPathDir[0]	= '\0';
	pVideoPlayer->ProgressPercentage 		= 0;
	pVideoPlayer->LastBookmarkingInSeconds = 0;
	pVideoPlayer->IsTouchValueGrabStarted	= 0;
	pVideoPlayer->IsControlPannelShowed	= FALSE;
	pVideoPlayer->isControlPannelVisible 	= -1;
	pVideoPlayer->IsVideoPlayerCreated	= FALSE;
	pVideoPlayer->IsVideoShowState		= FALSE;
	pVideoPlayer->IsEmergencyExitFlagEnabled = FALSE;
	pTAVI->BatteryLevelRequests 			= FALSE;
	pBookmarkDbx						= NULL;
	pCp									= new CControlPannel( VP_CtrlPannelButtonUpProc );
	pMbx								= new CMsgBox( ML_INFORMATION, _GREEN, ML_FW_UPGRADE_FAILED1, -1, -1, 0, DL_BG_BLANK | DL_BTN_OK_CANCEL );
	pSubtitle							= NULL;
	pSubtitleSyncDbx					= NULL;
	pSndPltDbx 							= NULL;
	IsXvidGmcEnabled					= FALSE;

	Stop_VideoControlPannelTimer();

	SetTaviKeyMode(TK_REPEAT);
	
	pCp->SetScreenRateType( (eVIDEOPLAYER_SCREEN_RATE_STATUS)TaviSysParam.video.screen );

	if( TaviSysParam.video.brightness > 60 || TaviSysParam.video.brightness < 0) 
		TaviSysParam.video.brightness = 32;

	if( TaviSysParam.video.contrast > 60 || TaviSysParam.video.contrast < 0)
		TaviSysParam.video.contrast = 32;

	if( TaviSysParam.video.saturation > 60 || TaviSysParam.video.saturation < 0)
		TaviSysParam.video.saturation = 32;

	pSetupDbx = new CSetupDialogBox( ML_VP_BRIGHTNESS_CONTRAST, 10-1, 105-1, 320-(10*2)-1, 240-105-10-1, VP_MsgboxButtonUpProc_Setup, VP_UpdateSetting, ID_APPLY|ID_CANCEL); 
 	pSetupDbx->AddSetupBarItem( 0, ML_VS_BRIGHTNESS, TaviSysParam.video.brightness, 60 );
 	pSetupDbx->AddSetupBarItem( 1, ML_VS_CONTRAST, TaviSysParam.video.contrast, 60 );
 	pSetupDbx->AddSetupBarItem( 2, ML_VS_SATURATION, TaviSysParam.video.saturation, 60 );
	pSetupDbx->UpdateSetting(NULL);

	pSndPltDbx = new CSoundPaletteDbx( ML_SOUND_PALETTE, 10-1, 35-1, 320-(10*2)-1, 240-35-10-1, VP_SetSoundPalette, 0 );

	#if 1
	g_TopPanel->SetTitle( ML_MOVIE );
	g_TopPanel->SetLogo( CTopPanel::LOGO_ID_VIDEO );
	g_TopPanel->Update();
	#endif

	// Video Player Callback Functions Init
	cbtVideo.DrawItem					= DrawItem;
	cbtVideo.DrawItemWithoutRefresh		= DrawItemWithoutRefresh;
	cbtVideo.DrawPlayTime				= DrawPlayTime;
	cbtVideo.DrawTotalTime				= DrawTotalTime;
	cbtVideo.DrawProgressBar				= BuildProgressBar;
	cbtVideo.IsStatusbarVisible				= VP_IsStatusbarVisible;
	cbtVideo.DrawASCIIText				= DrawASCIIText;
	cbtVideo.DrawASCIITextWithoutRefresh	= DrawASCIITextWithoutRefresh;
	cbtVideo.SetVideoRenderer				= VP_SetVideoRenderer;
	cbtVideo.DrawMsgBox					= VP_DrawMsgBox;
	cbtVideo.SetSeekable					= SetSeekable;
	cbtVideo.GetSeekable					= GetSeekable;
	cbtVideo.IsSeekingState				= IsSeekingState;
	cbtVideo.ResyncSMI					= VP_ResyncSMI;
	cbtVideo.SetScreenDisplayRatio			= VP_SetScreenDisplayRatio;
}

int __vpInit_MenuItems()
{

	/////////////////////////////////////////////////////////////////
	// Video File Browser SubMenu
	pPopUpSub_Sort = new CPopUpSubMenu;
	pPopUpSub_Sort->AddSubMenuItem( ML_NAME_ASCENT, TRUE, VP_FileBrowserNameAscentSort, NULL );
	pPopUpSub_Sort->AddSubMenuItem( ML_NAME_DESCENT, TRUE, VP_FileBrowserNameDescentSort, NULL );
	pPopUpSub_Sort->AddSubMenuItem( ML_TIME_ASCENT, TRUE, VP_FileBrowserTimeAscentSort, NULL );
	pPopUpSub_Sort->AddSubMenuItem( ML_TIME_DESCENT, TRUE, VP_FileBrowserTimeDescentSort, NULL );

	pmFileBrowser = new CPopUpMenu;
	pmFileBrowser->AddSubMenu( ML_SORTING_METHOD, pPopUpSub_Sort, TaviSysParam.sort, NULL, NULL );

	/////////////////////////////////////////////////////////////////
	// Video Player SubMenu In Video Playing Mode
	// 1. Bookmark
	psBookmark = new CPopUpSubMenu;
	psBookmark->AddSubMenuItem( ML_VP_ADD_BOOKMARK, VP_Add_Bookmark, NULL );
	psBookmark->AddSubMenuItem( ML_VP_LOAD_BOOKMARK, VP_Show_BookmarkDbx, NULL );

	// 2. Sub TItle
	psSubtitleEnabled = new CPopUpSubMenu;
	psSubtitleEnabled->AddSubMenuItem( ML_ON, TRUE, VP_SubtitleEnabled, NULL );
	psSubtitleEnabled->AddSubMenuItem( ML_OFF, TRUE, VP_SubtitleDisabled, NULL );
	psSubtitleEnabled->AddSubMenuItem( ML_VP_LOAD_SUBTITLE, FALSE, VP_LoadSubtitle, NULL );
	psSubtitleEnabled->AddSubMenuItem( ML_ADJ_SUBTITLE_TIME_SYNC, FALSE, VP_AdjustSubtitleSync, NULL );	

	psSubtitleDisabled = new CPopUpSubMenu;
	psSubtitleDisabled->AddSubMenuItem( ML_ON, TRUE, VP_SubtitleEnabled, NULL );
	psSubtitleDisabled->AddSubMenuItem( ML_OFF, TRUE, VP_SubtitleDisabled, NULL );
	psSubtitleDisabled->AddSubMenuItem( ML_VP_LOAD_SUBTITLE, FALSE, VP_LoadSubtitle, NULL );

	psSubtitleNotFound = new CPopUpSubMenu;
	psSubtitleNotFound->AddSubMenuItem( ML_OFF, TRUE, VP_SubtitleNotFound, NULL );
	psSubtitleNotFound->AddSubMenuItem( ML_VP_LOAD_SUBTITLE, FALSE, VP_LoadSubtitle, NULL );

	// 3. Repeat
	psRepeat = new CPopUpSubMenu;
	psRepeat->AddSubMenuItem( ML_VP_SINGLE_MOVIE, TRUE, VP_RepeatOff, NULL );
	psRepeat->AddSubMenuItem( ML_FOLDER, TRUE, VP_RepeatFolder, NULL );	

	// 5. Screen Mode
	psVideoMode = new CPopUpSubMenu;
	psVideoMode->AddSubMenuItem( ML_NORMAL, TRUE, VP_ScreenNormal, NULL );
	psVideoMode->AddSubMenuItem( ML_VP_ZOOM, TRUE, VP_ScreenZoom, NULL );
	psVideoMode->AddSubMenuItem( ML_VS_DR_FULLSCREEN, TRUE, VP_ScreenFullScreen, NULL );
	psVideoMode->AddSubMenuItem( ML_VP_WIDE_ZOOM, TRUE, ratio16x9_optimized, NULL );
	psVideoMode->AddSubMenuItem( ML_VP_WIDE_FULLSCREEN, TRUE, ratio16x9_full, NULL );

	// 7. VideoOutput
	psVideoOutput = new CPopUpSubMenu;
	psVideoOutput->AddSubMenuItem( ML_LCD, TRUE, VP_VideoLcdOut, NULL );
	psVideoOutput->AddSubMenuItem( ML_SETUP_COMPOSITE, TRUE, VP_VideoCompositeOut, NULL );
	psVideoOutput->AddSubMenuItem( ML_SETUP_COMPONENT, TRUE, VP_VideoComponentOut, NULL );
	psVideoOutput->AddSubMenuItem( ML_SETUP_HD_COMPONENT, TRUE, VP_VideoHdComponentOut, NULL );

	// 8. Main
	pVideoMenu = new CPopUpMenu( 0, 30, 320, 170 );
	pVideoMenu->AddSubMenu( ML_BOOKMARK, psBookmark, -1, NULL, NULL );
	pVideoMenu->AddSubMenu( ML_SUBTITLE, psSubtitleNotFound, 0, NULL, NULL );	// 0, 1
	pVideoMenu->AddSubMenu( ML_BOUNDARY, psRepeat, (int)TaviSysParam.video.playlist_repeat, NULL, NULL );
	pVideoMenu->AddSubMenu( ML_VP_SCREEN_MODE, psVideoMode, (int)TaviSysParam.video.screen, NULL, NULL );
	pVideoMenu->AddSubMenu( ML_VIDEO_OUTPUT, psVideoOutput, 0, VP_UpdateVideoOutput, NULL );
	pVideoMenu->AddSubMenu( ML_VP_BRIGHTNESS_CONTRAST, NULL, -1, VP_SetBrightnessAndContrast, NULL );
	pVideoMenu->AddSubMenu( ML_CLOSE, NULL, -1, VP_HideSubmenu, NULL );


	pmVideoPlay = pVideoMenu;

	return TRUE;
}

int __vpRemove_MenuItems()
{
	if( pSubtitleSyncDbx != NULL )	{ 
	  delete pSubtitleSyncDbx; 	
	  pSubtitleSyncDbx = NULL; 
	}
	if( pSubtitle != NULL )	{ 
	  delete pSubtitle;
	  pSubtitle = NULL; 
	}
	if( pSetupDbx != NULL )	{ 
	  delete pSetupDbx;	
	  pSetupDbx = NULL; 
	}
	if( pBookmarkDbx != NULL ) {
	  delete pBookmarkDbx;
	  pBookmarkDbx = NULL; 
	}
	if( pSndPltDbx != NULL ) {
	  delete pSndPltDbx;
	  pSndPltDbx = NULL; 
	}
	if(pmFileBrowser != NULL) {
    delete pmFileBrowser; 
    pmFileBrowser = NULL; 
  }
	if(pVideoMenu != NULL) {
	  delete pVideoMenu; 
	  pVideoMenu = NULL; 
	}

	return TRUE;
}


void _vpPrepareTo_Open()
{
	XEN_Init();
	__vpInit_Palette();
	__vpInit_LocalConfig();
	__vpInit_MenuItems();
	showbackground( 2 );
}

void _vpPrepareTo_Exit()
{
	Stop_VideoControlPannelTimer();
	Stop_SoundPaletteCtrlHideTimer();

	if( pSndPltDbx->IsVisible() ) pSndPltDbx->Hide(SKIP_OSD_REFRESH);
	pLFBrowser->Hide();
	g_TopPanel->Hide();
	
	XEN_Exit();

	pTAVI->BatteryLevelRequests = TRUE;
	pVideoPlayer->isControlPannelVisible = -1;

	__vpRemove_MenuItems();

	if( pCp ) {
		delete pCp; 
		pCp = NULL; 
	}
	
	if( pVideoPlayer ) {
		delete pVideoPlayer;		
		pVideoPlayer = NULL; 
	}

	if( TaviSysParam.etc.ResVal == 0 && 
	    TaviSysParam.etc.ExitOption !=TAVI_KEY_SHUTDOWN ) {
		g_MiscCtrl->MainLCDOnOff(true);
	}

	pMpegDecoder->Stop();

}



int VideoPlayer_Task( TTaskInitParam* tParam )
{
	////////////////////////////////////////////////////////////////////////////
	// Init Video Player Task
	TMsgEntry tMsg;

  /* 
      Reconfigurate sound driver
  */
  g_MiscCtrl->Mute( true );
  g_SndCtrl->PlaybackMode();
  g_SndCtrl->LineInBypass( false, 0 );
  g_SndCtrl->LineInBypass( false, 1 );
  g_SndCtrl->LineInBypass( false, 2 );
  g_MiscCtrl->Mute( false );

    
	_vpPrepareTo_Open();

	VP_TaskStep2( tParam, &tMsg );

	_vpPrepareTo_Exit();

	tMsg.usParam1=0;
	tMsg.usParam2=0;


	return TaskExit_Normal;
	
}

