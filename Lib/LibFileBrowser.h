/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * LibFileBrowser.h
 * Sang-jin Ryu (ew@newmedialife.com)
 *
 */

#ifndef __LIBFILEBROWSER_H__
#define __LIBFILEBROWSER_H__


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

#include <unistd.h>
#include <dirent.h>

#include <stdio.h>	
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "Component/object.h"


#include "fs.h"
#include "fatfs.h"

#include "Lib/LibImage.h"

typedef enum
{
	_FBMODE_FILEMANAGER=0,
	_FBMODE_VIDEOPLAYER,
	_FBMODE_AUDIOPLAYER,
	_FBMODE_IMAGEVIEWER,
	_FBMODE_TEXTVIEWER,
	_FBMODE_ALARM_SETTTING
	
} eFILEBROWSER_ACTIVE_MODE;


typedef enum
{
	ENCLOSE_UNDER_CURSOR=0,
	ENCLOSE_SELECTED,
	ENCLOSE_ALL

}eFILEITEM_ENCLOSE_OPTION;


#define __ATTRIBUTE_PACKED__	__attribute__((packed))

#define FILEBROWSER_RETURN_EXIT (-1)
#define FILEBROWSER_RETURN_NORMAL (0)
#define FILEBROWSER_RETURN_FILE_SELECTED (1)

typedef enum
{
	FATFS=0,
	LINUXFS
} eCURRENT_DIR_FS_TYPE;

typedef struct _tag_FILE_NODE
{
	int iCurrentItemIndex;
	int iMaxItemIndex;
	int isSelected;
	FATFS_DIR_ENTRY_INFO D;
	eFILE_TYPE filetype;
	eCURRENT_DIR_FS_TYPE filesystem;
	char ext[4];
} __ATTRIBUTE_PACKED__ FILE_NODE;

typedef struct _tag_CRC_NODE
{
	int CRC;

} CRC_NODE;

typedef struct _tag_FOLDER_INFO
{
	int FolderItemCount;
	RMuint64 ItemTotalSize;
} FOLDER_INFO;

typedef struct _tag_FOLDER_RESUME_INFO
{
	RMuint32 DirectoryLevel;
	uchar_t Fullpath[260];
} FOLDER_RESUME_INFO;

typedef struct video_acc_t {
  int name_crc;
  int exist;
  int access;
} video_acc_t;
typedef struct _tag_access_info {
  uchar_t path[MAX_PATH];
  int cnt;
  video_acc_t info[1];
} acc_info_t;

typedef enum
{
	ADD_FILE_ITEM=0,		// FILE_ITEM means "Files And Folders"
	REMOVE_FILE_ITEM,
	ADD_FILE,
	REMOVE_FILE,
	ADD_FOLDER,
	REMOVE_FOLDER,
	GET_ONLY_TFI
} eRECURSIVE_SCAN_OPTION;

typedef bool (*SORTING_METHOD)( FILE_NODE* pNewItem, FILE_NODE* pOldItem );
bool fpNameAscentSort( FILE_NODE* pNewItem, FILE_NODE* pOldItem );
bool fpNameDescentSort( FILE_NODE* pNewItem, FILE_NODE* pOldItem );
bool fpTimeAscentSort( FILE_NODE* pNewItem, FILE_NODE* pOldItem );
bool fpTimeDescentSort( FILE_NODE* pNewItem, FILE_NODE* pOldItem );

class CFileBrowser
{
	eFILEBROWSER_ACTIVE_MODE	eActiveMode;
		
	CList<FILE_NODE> 	ItemList_Files;
	CList<CRC_NODE>	SubtitleCrcList;

	CScrollBar2*					pSb1;
	uchar_t* 						StaticTitle;
	uchar_t* 						DynamicTitle;
	uchar_t* 						SrcPathname;
	int							TitleResIdx;
	int							TitleIcon;
	int							sx;
	int 							sy;
	int							width;
	int 							height;
	int 							iVisibleStatus;
  int*            enjoyed;
  int             nfiles;
	
	MSG_PROCESSOR 				fpDialogBoxMsgProc;
	CMsgBox* 					CD;

	CDialogBox*					pDlg;
	CMsgBox*					pMbx;

	int 							DirectoryLevel;

	// Slots
	int							choice;
	int 							LastSlot;

	// Indexes
	int							SelectedAbsoluteIndex;			// Cursor
	int							AbsoluteIndex;					// 0 ~ LASTINDEX
	int							RelativeIndex;
	int							FirstShownIndex;				
	int							LastIndex;

	int 							TouchPreParam1;
	int 							TouchPreParam2;
	int 							TouchSum;
	int 							TouchCounter;
	int 							TouchMeanValue;

	SORTING_METHOD				CurrentSortingMethod;

	void CreateTitle( uchar_t text[] );
	void CreateSelectBar();
	void CreateTextData();
	void CreateVScrollBar();

	int ScanDir( uchar_t targetdir[], int option );
	int ScanDir_linuxfs( uchar_t ufullpathDir[] );

	FOLDER_INFO CurrentFI;
	RMint32	crcRootDir;
	RMuint32 crcDbDir;

	int SaveDirCheck();
	int SaveSdiFile( FOLDER_RESUME_INFO* pR,  RMuint32 option=0 );

	void _OpenDir_Fullpath_Root(char* cTemp);
	void _OpenDir_Fullpath(uchar_t* targetdir, uchar_t* ucTemp, char* cTemp);
	void _OpenDir_Onepath(uchar_t* targetdir, uchar_t* ucTemp);
	void _OpenDir_NonRoot( );


public:
	uchar_t* CurrentFullPathDir;
	uchar_t* RootDir;
	uchar_t* ParentDir;
	uchar_t* Filename;
	uchar_t* TaviCastDir;
	RMuint32 currdir_fd;
	char* cTemp;

	int DirRepeatStartPoint;

	char* CurrentFileExt;
	int CurrentFileType;
	eCURRENT_DIR_FS_TYPE CurrentFileSystem;

	CList<FILE_NODE> ItemList;						// IL
	CList<FATFS_DIR_ENTRY_INFO> 	SelectedItemList;	// SIL
	CList<FATFS_DIR_ENTRY_INFO> 	QueuedItemList;		// QIL
	RMuint64 TotalSizeOfSelectedFiles;

	RMuint32 crcBatteryDebugEnableDir;
	RMuint32 crcSystemVolumeInformationDir;

	// for auto scroll
	int checkpoint;
	TMsgEntry tMsgTemp;

	CFileBrowser(){}
	CFileBrowser( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option  ) ;
	virtual ~CFileBrowser();

	void SetSortingMethod( SORTING_METHOD fpNewMethod );
	SORTING_METHOD GetCurrentSortingMethod( );
	void RefreshForSortingMethodChange(SORTING_METHOD fpNewMethod, eOSD_REFRESH_OPTION option);
	
	int MsgBtnUp( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
	int AddFileItem( int idx, char name[], int filesize, eFILE_TYPE filetype );
	int AddFileItem_UC16( int idx, FILE_NODE* pN, eCURRENT_DIR_FS_TYPE filesystem=FATFS );
	int AddFileItem_UC16( int idx, uchar_t name[], int filesize, eFILE_TYPE filetype, char ext[] );
	int AddFileItem_UC16( int idx, FATFS_DIR_ENTRY_INFO* pD, eFILE_TYPE filetype, char ext[] );
	int AddFileItem_UC16_OnTop( int idx, uchar_t name[], int filesize, eFILE_TYPE filetype, char ext[], eCURRENT_DIR_FS_TYPE filesystem );	// added by who
	int AddFileItem_UTF8( int idx, char name[], int filesize, eFILE_TYPE filetype, char ext[], eCURRENT_DIR_FS_TYPE filesystem );		// added by who
	int AddFileItem_UC16_WithSorting( int idx, FATFS_DIR_ENTRY_INFO* pD, eFILE_TYPE filetype, char ext[], eCURRENT_DIR_FS_TYPE filesystem=FATFS );
	int AddFileItem_UC16_OnTop( int idx, FATFS_DIR_ENTRY_INFO* pD, eFILE_TYPE filetype, char ext[], eCURRENT_DIR_FS_TYPE filesystem );	// added by who
	int AddSubtitleCRC( int CRC );
	
	int Reset();
	#define _REDO_CURSOR 	0x00000001
	#define _KEEP_CURSOR 	0x00000002
	#define _SKIP_CURSOR 	0x00000003
	#define _CLEAR_CURSOR 	0x00000004
	int ClearFileItem( U32 option );

	int SetActiveMode( U32 ActiveMode, MSG_PROCESSOR pfpMsgProc, U32 StartAtLastDir );
	int SetCurrentChoice(int NewChoice);
	int SetCurrentChoiceToNext();
	int RememberDirRepeatStartPoint();

	int GetActiveMode( );
	int GetCurrentChoice();

	eFILE_TYPE GetListItemType( int index );
	eFILE_TYPE GetCurrentListItemType( );
	RMuint32 GetCurrentListItemFilesize();
	int GetCurrentListItemNameLength();
	int GetListItemSize( int index );
	int GetListItemName( int index, char StringBuffer[] );
	int GetListItemName_UC16( int index, uchar_t StringBuffer[] );
	int GetListItemCount()	;
	int Get_IL_SelectedCount();
	
	int CursorUp();
	int CursorDown();
	int CursorLeft();
	int CursorRight();

	void InitCursorLoop();
	void CursorLoop( int Direction, int RawLoopData );

	int MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
	int ProcFBAutoScrollTimer( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );

	int Select( ) { Select( OSD_REFRESH ); }
	int Select( eOSD_REFRESH_OPTION option );
	int isVisible();
	int ChangeToParentDirectory( U32 option );
	int Show();
	int Hide();
	
	void ReDraw(eOSD_REFRESH_OPTION option);
	void ReDraw();
	void Refresh(eOSD_REFRESH_OPTION option);
	void Refresh();

	int RecursiveScan( const uchar_t* pathSource, FOLDER_INFO* pFI, eRECURSIVE_SCAN_OPTION option );

	// ItemList
	int IsIL();
	int Add_IL_CheckTag( eFILEITEM_ENCLOSE_OPTION option );
	int Remove_IL_CheckTag( eFILEITEM_ENCLOSE_OPTION option );
	int Clear_IL_Only();

	// SelectedItemList
	int IsSIL();
	int IsSIL(int index);
	int Add_IL_To_SIL( eFILEITEM_ENCLOSE_OPTION option );
	int RemoveOn_SIL( eFILEITEM_ENCLOSE_OPTION option );
	int Clear_SIL_Only();
	int Clear_IL_SIL_All();

	int Get_CurrentFI_Count();
	RMuint64 Get_CurrentFI_Size();

	void SetSrcPathname();
	int Compare_SD_Folder();

	int IsLastItem();

	int SaveDirInfo();

	int LoadDirInfo();
	int SetSelectbarValues( RMuint32 CRC );
	RMuint64 du_linuxfs(char *filename, FOLDER_INFO *pTFI );	// added by who
	RMuint64 df_linux(char *filename);
	RMuint64 df_fatfs( RMint32 NoVol);
	RMuint64 GetFreeSpace( void );

	
	int IsThisLinuxFs();
	int IsThisLinuxFs_Root();
	int IsThisFatFs();
	int IsThisFatFs_Root();
	
	int IsThisLinuxFs(uchar_t* u16String );		// added by who
	RMuint8 Convert1digitBCDCharToHexCode( RMuint8 BCDChar);	// added by who
	RMuint16 Convert4digitBCDStringToHexCode( RMuint8 *BCDString  );	// added by who
	RMuint16 ConvertVFATTextToUnicode16Str( uchar_t *uc16String, char *euc8String );	// added by who
	RMuint16 ConvertUnicodeToUniXlate( char* euc8String, uchar_t* u16String );	// added by who
	int ExistTaviCastOnParentDir( uchar_t *puDirName, FATFS_DIR_ENTRY_INFO *pDirInfo4TaviCast );

};


#endif



