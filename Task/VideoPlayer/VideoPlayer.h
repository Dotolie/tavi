//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : VideoPlayer Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/VideoPlayer/VideoPlayer.h,v $
// $Revision: 1.6 $
// $Date: 2006/06/07 07:59:35 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: VideoPlayer.h,v $
// Revision 1.6  2006/06/07 07:59:35  zstein
// - added Wide(16x9) modes.
//
// Revision 1.5  2006/05/23 10:10:01  zstein
// - deleted unused function declarations.
//
// Revision 1.4  2006/04/07 04:36:04  zstein
// - deleted unused functions declarations.
//
// Revision 1.3  2006/04/05 01:50:05  zstein
// - deleted unused function declarations.
//
// Revision 1.2  2006/03/07 04:57:04  zstein
// - changed to the original volume level.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*****************************************************************************

#ifndef __VIDEO_PLAYER_H__
#define __VIDEO_PLAYER_H__

#include <tavi_lib.h>
#include <ImageFont.h>

#define VideoPlayerReturn_NOTUSED (-2)
#define VideoPlayerReturn_EXIT (-1)
#define VideoPlayerReturn_NORMAL (0)

#define TOUCH_SCREEN_MAX_VALUE (4095)

#define DisplayVolume_MIN (0)
#define DisplayVolume_MAX (40)
#define HWRealVolume_MIN (0)
#define HWRealVolume_MAX (79)
//#define VideoVolume_DIFF (HWRealVolume_MAX-DisplayVolume_MAX)
#define VideoVolume_DIFF (0)

typedef enum
{
	_DISPLAY_RATIO,
	_DISPLAY_DEVICE,
	_PLAY_OPTION
} eVP_SETUP_ITEM;

typedef enum
{
	_PLAY_ONCE,
	_REPEAT_THIS,
	_REPEAT_CURRENTDIR
} eVP_PLAY_OPTION;

typedef enum
{
	_PLAY_BOOKMARK,
	_SET_BOOKMARK
} eVP_BOOKMARK_TYPE;

typedef enum
{
	_WH_MIN,
	_WH_MAX,
	_FULL_SCREEN
	
} eVP_ScreenType;

typedef enum
{
	_VideoExit_NORMAL,
	_VideoExit_FILEBROWSER,
	_VideoExit_SKIPSAVE
	
} eVP_VideoExitOption;

typedef enum 
{
  	__VP_PLAY					= 0,
	__VP_PAUSE					= 1,
	__VP_FF					= 2,
	__VP_REW					= 3,
	__VP_NEW					= 4,
  	__VP_STOP					= 5,
	__VP_PREV					= 6,
	__VP_NEXT					= 7,
	__VP_EXIT					= 8,

	__VP_LISTVIEW_ARTISTS		= 0x00000100,
	__VP_LISTVIEW_ALBUMS			= 0x00000200,
	__VP_LISTVIEW_GENRES			= 0x00000400,
	__VP_LISTVIEW_TITLES			= 0x00000800,
	__VP_LISTVIEW_YEARS			= 0x00001000,
	__VP_LISTVIEW_MUSICLIBRARY	= 0x00002000,
	__VP_LISTVIEW_PLAYLISTS		= 0x00004000,
	__VP_LISTVIEW_MUSICFOLDER	= 0x00008000
	
} eVP_Status;


typedef struct tagVIDEOFILE_NODE {
	char* 	Pathname;		// Pathame Only 
	char* 	Filename;		// Filename Only
	RMint64	Filesize;
	RMint64	Playtime;		// Total Playtime
} tVIDEOFILE_NODE;

typedef struct _tag_TVIDEOPLAYER_CONFIG {
	int 						TaskId;
	eVP_Status 				Status;
	eVP_ScreenType			ScreenType;
	char*					CurrentFullPathDir;
	U16 					CurrentMenu;
	int						CurrentItemNumber;		// 0 ~ MAX
	int						isControlPannelVisible;
	U16						ProgressPercentage;
	U16						LastBookmarkingInSeconds;
	int						IsTouchValueGrabStarted;
	int						IsControlPannelShowed;
	U16						CurrentSeekBarProgress;
	int						IsVideoScreenInitialized;
	int						IsVideoPlayerCreated;
	int						IsVideoShowState;
	int 					IsEmergencyExitFlagEnabled;
} _ATTRIBUTE_PACKED_ TVIDEOPLAYER_CONFIG;

typedef struct _tag_MENU_OPTION_INT
{
	int value;
} MENU_OPTION_INT;

typedef RMuint32 (*VIDEO_RENDERER)(void *context);
typedef RMuint32 (*VIDEO_RENDERSTOPPER)(void *context);

// "VP" Means "VideoPlayer"
void VP_SetPlayStatus(eVP_Status status, eOSD_REFRESH_OPTION refresh=OSD_REFRESH );
int 	VP_LoadPallette(RMuint8* palette);
void VP_DrawFrame( U32 flags, int fRefresh=1 );
int VP_DrawMsgBox( RMuint32 msg );
int VP_MsgboxButtonUpProc_Startpoint( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
int VP_MsgboxButtonUpProc_Bookmark( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
int VP_MsgboxButtonUpProc_Setup( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
int VP_UpdateSetting();
int VP_CtrlPannelButtonUpProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
int 	VP_MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1 , int ssParam2 );
int VP_MsgProc_FileBrowser( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
void VP_DialogBox_MessagePeeking();
void VP_ShowException( FATFS_ERROR err );

int 	VideoPlayer_Task( TTaskInitParam* tParam );
static int VP_TaskStep2( TTaskInitParam* tParam, TMsgEntry* ptMsg );
int _CommonProcess_RendererEnclose( TTaskInitParam* tParam, TMsgEntry* ptMsg );

int VP_FileBrowsing2(int ssTaskId , char * file_name, unsigned int name_len, int opt );

static void TimerHandlerMenuHide( int signo );
static void TimerHandlerMenuShow(int signo);
int VP_IsStatusbarVisible();
void VP_RedrawProgressBar( int fWithoutRefresh  );
int VP_SaveCurrentPosition( char sPathname[], char sFullpathFilename[], U16 usFilepositionInSeconds );
U32 VP_CheckLastResumeFile( char sPathname[], char sFullpathFilename[] );
int VP_Bookmarking( char sPathname[], char sFullpathFilename[], int iFilepositionInSeconds, int slot );

int VP_SetVideoRenderer( int id );
//static int VP_PrepareAndPlay(RMint8 filename[]);
static int VP_PrepareAndPlay( uchar_t filename[], char ext[] );
int VP_SeekingDoneProcess();

int VP_FileBrowserNameAscentSort( RMuint8* parameter );
int VP_FileBrowserNameDescentSort( RMuint8* parameter );
int VP_FileBrowserTimeAscentSort( RMuint8* parameter );
int VP_FileBrowserTimeDescentSort( RMuint8* parameter );

int VP_HideSubmenu( RMuint8* parameter=NULL );
int VP_SubtitleEnabled( RMuint8* parameter );
int VP_SubtitleDisabled( RMuint8* parameter );
int VP_SubtitleNotFound( RMuint8* parameter );
int VP_AdjustSubtitleSync( RMuint8* parameter );

int VP_RepeatOff( RMuint8* parameter );
int VP_RepeatFolder( RMuint8* parameter );

int VP_CloseSoundPaletteDbx();
int VP_SoundPaletteDbx( RMuint8* parameter );
int VP_ScreenNormal( RMuint8* parameter );
int VP_ScreenZoom( RMuint8* parameter );
int VP_ScreenFullScreen( RMuint8* parameter );
int VP_SetScreenDisplayRatio();

int VP_CheckBookmark( U32 option );
static void VP_PrepareToStopPlaying(int option);
int VP_ChangeToNextEQ();
void VP_RecoveryDisplayRatio();

#endif

