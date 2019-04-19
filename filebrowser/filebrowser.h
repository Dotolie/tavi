#ifndef _FILEBROWSER_H
#define _FILEBROWSER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

#include <unistd.h>
#include <dirent.h>

#include <stdio.h>	// need sprintf
#include <string.h>
#include <stdlib.h>
#include <memory.h>	// memmove/memcpy
#include "rmbasic.h"

#include "../include/bitmap.h"
#include "../include/tavi_palette.h"
#include "../include/ImageResource.h"

/*

#include <stdio.h>

#include <rmbasic.h>
#include <linuxfs.h>
#include <jpeg/jpeg.h>

#include <tavi_lib.h>
#include <tavi_structures.h>
#include <bitmap.h>
#include <ImageFont.h>
*/


//#include <memory.h> // memmove/memcpy

#define FILESYSTEM_MEDIA_INSERTED			0xFF000003

#define FILEBROWSER_MAX_FILENAME_LENGTH		1024
#define FILEBROWSER_MAX_PLAYLIST_LENGTH		1000
#define FILEBROWSER_MAX_LINES_ON_SCREEN		20

typedef enum
{
	FILEBROWSER_ERROR_NO_ERROR,
	FILEBROWSER_ERROR_NOT_IMPLEMENTED,
	FILEBROWSER_ERROR_COMMAND_NOT_ALLOWED,
	FILEBROWSER_ERROR_NO_REPAINT_REQUIRED,
	FILEBROWSER_ERROR_NO_MORE_ENTRIES,
	FILEBROWSER_ERROR_NOT_SPECIFIED

} FILEBROWSER_ERROR;

typedef enum
{
	FILEBROWSER_ICON_UNKNOWN,
	FILEBROWSER_ICON_AUDIO,
	FILEBROWSER_ICON_VIDEO,
	FILEBROWSER_ICON_IMAGE,
	FILEBROWSER_ICON_DIRECTORY
} FILEBROWSER_ICON_TYPE;

#define DIR_ENTRY_INFO_IS_FILE			1
#define DIR_ENTRY_INFO_IS_DIRECTORY		2
#define DIR_ENTRY_INFO_IS_ROOT_OR_PARENT_OR_HIDDEN 3

typedef struct tagDIR_ENTRY_INFO
{
	RMuint32	handle;
	RMint8		parentdir[FILEBROWSER_MAX_FILENAME_LENGTH];
//	RMint8		name[FILEBROWSER_MAX_FILENAME_LENGTH];
//	RMint8		parentdir[256];
	RMint8		name[256];
	RMuint32	namelength;
	RMuint32	flags;

} DIR_ENTRY_INFO;

//typedef unsigned int 		RMuint16;
//typedef unsigned long 		U32;


typedef RMuint32 (*FILEBROWSER_GETROOTDIRECTORY)(RMint8 *dir, RMuint32 length, void *context);
typedef RMuint32 (*FILEBROWSER_OPENDIRECTORY)(RMint8 *dir, void *context);
typedef RMuint32 (*FILEBROWSER_READDIRECTORY)(RMuint32 handle, DIR_ENTRY_INFO *pDirInfo, void *context);
typedef RMuint32 (*FILEBROWSER_CLOSEDIRECTORY)(RMuint32 handle, void *context);
typedef RMuint32 (*FILEBROWSER_RENDER_ERASE)(void *context);
//typedef RMuint32 (*FILEBROWSER_RENDER_ICON)(FILEBROWSER_ICON_TYPE type, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMuint32 selected, void *context);
typedef RMuint32 (*FILEBROWSER_RENDER_ICON)( RMuint16 usPictureID, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight );
typedef RMuint32 (*FILEBROWSER_RENDER_TEXT)(RMint8 *text, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMuint32 selected, void *context);
typedef RMuint32 (*FILEBROWSER_RENDER_TEXT_WITHOUT_REFRESH)(RMint8 *text, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMuint32 selected, void *context);
typedef RMuint32 (*FILEBROWSER_OSD_REFRESH)();
typedef RMuint32 (*FILEBROWSER_RENDER_FILE)(RMint8 *text, void *context);
typedef RMuint32 (*FILEBROWSER_IS_RENDER_DONE)(void *context);
typedef RMuint32 (*FILEBROWSER_RENDER_STOP)(void *context);
typedef RMuint32 (*FILEBROWSER_INFO)(RMint32 Msg, void *info, void *context);
typedef RMuint32 (*FILEBROWSER_EXIT_ROOT)(void *context);
typedef RMuint32 (*FILEBROWSER_CLEAR_OSD_WITHOUT_REFRESH)( int x, int y, int w, int h );
typedef RMuint16 (*DRAW_ASCII_TEXT_WITHOUT_REFRESH)( char text[], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 type);
typedef RMuint16 (*DRAW_BOX_WITHOUT_REFRESH)( RMuint8 color, RMuint16 sx, RMuint16 sy, RMuint16 width, RMuint16 height );
typedef RMuint16 (*DRAW_ITEM_WITHOUT_REFRESH)( BITMAPFILEINFO256* pBitmap, RMuint16 usPictureID, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight );
typedef RMuint16 (*DRAW_ITEM_WITHOUT_REFRESH_SKIP_TRANSPARENCY)( BITMAPFILEINFO256* pBitmap, RMuint16 usPictureID, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight );
typedef RMuint16 (*FILEBROWSER_SCANDIRECTORY)(RMint8 *dir, DIR_ENTRY_INFO* entry, void* context);






#define FILEBROWSER_INFO_MSG_CURRENT_DIRECTORY	0

typedef struct tagFILEBROWSER_CALLBACK_TABLE
{
	void *context;
	FILEBROWSER_GETROOTDIRECTORY getrootdir;
	FILEBROWSER_OPENDIRECTORY opendir;
	FILEBROWSER_READDIRECTORY readdir;
	FILEBROWSER_CLOSEDIRECTORY closedir;
	FILEBROWSER_RENDER_ERASE rendererase;
	FILEBROWSER_RENDER_ICON rendericon;
	FILEBROWSER_RENDER_TEXT rendertext;
	FILEBROWSER_RENDER_TEXT_WITHOUT_REFRESH rendertextwithoutrefresh;
	FILEBROWSER_CLEAR_OSD_WITHOUT_REFRESH clearosdwithoutrefresh;
	FILEBROWSER_OSD_REFRESH osdrefresh;
	FILEBROWSER_RENDER_FILE renderfile;
	FILEBROWSER_IS_RENDER_DONE isrenderdone;
	FILEBROWSER_RENDER_STOP renderstop;
	FILEBROWSER_INFO info;
	FILEBROWSER_EXIT_ROOT exitroot;
	
	DRAW_ASCII_TEXT_WITHOUT_REFRESH	DrawASCIITextWithoutRefresh;
	DRAW_BOX_WITHOUT_REFRESH	DrawBoxWithoutRefresh;
	DRAW_ITEM_WITHOUT_REFRESH DrawItemWithoutRefresh;
	DRAW_ITEM_WITHOUT_REFRESH_SKIP_TRANSPARENCY DrawItemWithoutRefreshSkipTransparency;

	FILEBROWSER_SCANDIRECTORY	scandir;

} FILEBROWSER_CALLBACK_TABLE;

class FileBrowser
{

//	RMint32			m_AbsoluteCursor;	// Range : 0 ~ m_TotalItemCount

	RMint32			m_RelativeCursor;	// Range : 0 ~ m_TotalSlotNumber;
	RMint32 			m_TotalSlotNumber;
	RMint32			m_DisplayStartItem;	// Range : 0~  m_MaxCurPos

	RMint32			m_SelectedItem;

	RMuint32			m_State;
	RMint32			m_ScreenWidth;
	RMint32			m_ScreenHeight;
	RMint32			m_TextHeight;
	RMint32			m_IconWidth;
	RMint32			m_IconHeight;

	RMint32			m_CurrentIndex;

	RMint32			m_ValidItems;
	RMint32			m_LinesOnScreen;

	RMuint32			m_handle;
//	RMint8			m_rootdir[FILEBROWSER_MAX_FILENAME_LENGTH];
//	RMint8			m_currentdir[FILEBROWSER_MAX_FILENAME_LENGTH];
	RMuint32			m_RenderPending;
	RMuint32			m_PlayDirectory;	

	FILEBROWSER_CALLBACK_TABLE m_CallbackTable;

//	DIR_ENTRY_INFO	m_DirectoryEntries[FILEBROWSER_MAX_LINES_ON_SCREEN];
	
	FILEBROWSER_ERROR Browse ();
	FILEBROWSER_ERROR WaitForRender ();
	FILEBROWSER_ERROR RefreshList (RMint32 n);	

public:
	
	RMint32			m_AbsoluteCursor;	// Range : 0 ~ m_TotalItemCount
	RMint32			m_TotalItemCount;
	RMint8			m_rootdir[FILEBROWSER_MAX_FILENAME_LENGTH];
	RMint8			m_currentdir[FILEBROWSER_MAX_FILENAME_LENGTH];
	DIR_ENTRY_INFO	m_DirectoryEntries[FILEBROWSER_MAX_LINES_ON_SCREEN];
		
	RMint32			m_HorizontalOverscan;
	RMint32			m_VerticalOverscan;
	RMint32			m_filenameoffset;

	FileBrowser ();
	~FileBrowser ();

	void ReuseFileBrowser ();
	void ReCloseFileBrowser ();

	FILEBROWSER_ERROR Init ();

	FILEBROWSER_ERROR RepaintList (int option);
	FILEBROWSER_ERROR RepaintListV2 (int option);

	FILEBROWSER_ERROR SetDisplayDimensions (RMint32 w, RMint32 h);
	FILEBROWSER_ERROR SetTextHeight (RMint32 h);
	FILEBROWSER_ERROR InitCallbackTable (FILEBROWSER_CALLBACK_TABLE *pCallback);

	FILEBROWSER_ERROR Schedule ();
	FILEBROWSER_ERROR Up ();
	FILEBROWSER_ERROR Down ();
	FILEBROWSER_ERROR Right ();
	FILEBROWSER_ERROR Left ();
	FILEBROWSER_ERROR Select ();
	FILEBROWSER_ERROR Stop ();
	FILEBROWSER_ERROR Idle ();

	int GetParentDir_InCurrentFullDirString( char source[] );
	int ChangeToParentDir();
	int GetPrevFilename( int current, char filename[] );
	int GetNextFilename( int previous, char filename[] );

};

#endif
