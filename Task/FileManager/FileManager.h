//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : FileManager Task Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/FileManager/FileManager.h,v $
// $Revision: 1.3 $
// $Date: 2006/06/12 04:17:30 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: FileManager.h,v $
// Revision 1.3  2006/06/12 04:17:30  ywkim
// change printf to DEBUGMSG
// change file access function to low level one
//
// Revision 1.2  2006/04/13 01:43:30  ywkim
// Change some function for delete
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.15  2005/12/22 05:13:21  etnlwind
// Update for New Version
//
// Revision 1.14  2005/12/19 06:41:24  etnlwind
// Update for Copy-Move Operatrion Dbx Proc
//
// Revision 1.13  2005/12/15 09:11:58  etnlwind
// Update for File Copy
//
// Revision 1.12  2005/12/08 08:16:00  etnlwind
// Update for Task Switching
//
// Revision 1.11  2005/11/04 08:37:10  etnlwind
// 비디오 디렉토리 반복
// -[||]PAUSE 가 화면에 잠시 나오는 현상 수정
// -디렉토리안에 다른디렉토리가 있으면 반복이 한번 더 되는 현상 수정
//
// 파일브라우저
// -파일정렬방법이 매번 초기화 되지않고 저장되도록 수정
//
// Revision 1.10  2005/10/05 09:43:24  etnlwind
// Update for
// 1. FileManager Copy/Move/Delete Related
// 2. VideoPlayer Bookmarking/Bookmark Loading/Resume Related
//
// Revision 1.9  2005/09/12 00:53:52  etnlwind
// Update for FileManager
//
// Revision 1.8  2005/08/24 13:32:01  etnlwind
// Update for Virtual Keyboard Related
//
// Revision 1.7  2005/08/19 11:20:04  etnlwind
// Update for __ATTRIBUTE_PACKED__ and FileBrowser Delete Func.
//
// Revision 1.6  2005/08/08 10:49:41  etnlwind
// Update for FileManger, VideoPlayer
//
// Revision 1.5  2005/07/29 06:41:26  etnlwind
// Update for FileManger Related
//
// Revision 1.4  2005/07/26 04:50:02  etnlwind
// Update for FileManager Basic
//
// Revision 1.3  2005/04/18 09:22:24  etnlwind
// update for ES4
//
// Revision 1.2  2005/02/26 07:59:26  etnlwind
// FileBrowser Step-1 Passed
//
// Revision 1.1  2005/02/25 14:03:44  etnlwind
// FileManager Add
//
// Revision 1.1  2005/02/25 13:37:44  etnlwind
// FileMnager Add
//
//*****************************************************************************

#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#include <tavi_lib.h>
#include <ImageFont.h>

//#define szCopyBuffer (1024*1024)
#define szCopyBuffer (512*1024)


typedef enum 
{
	_CMD_READY_STATE=0,
	_CMD_COPY_STATE,
	_CMD_MOVE_STATE
}eFILEMANAGER_COMMAND_STATUS ;

typedef int (*PASTE_METHOD)(	eFILEMANAGER_COMMAND_STATUS SubmenuStatus, 
								uchar_t* FolderName, 
								uchar_t* SrcDir, 
								uchar_t* DesDir, 
								RMuint64* u64TempFreeDiskSpace,
								int PasteItemCounts );

typedef int (*DELETE_METHOD)( uchar_t* FileName, uchar_t* DesDir, int DeleteItemCounts );

typedef struct _PATH_ITEM
{
	CString Path;
	CString Item;
} __ATTRIBUTE_PACKED__ PATH_ITEM;

typedef struct _tag_FILEBMANAGER_CONFIG {
	TTaskInitParam*			Param;
	int 						TaskId;
	char*					CurrentFullPathDir;
	U16 						CurrentMenu;
	int						CurrentItemNumber;		// 0 ~ MAX
	RMuint64					u64DiskFreeSizeInBytes;
	int						i32DiskFreeSizeInMB;
	eFILEMANAGER_COMMAND_STATUS SubmenuStatus;
} tFILEMANAGER_CONFIG;

// "FMMC" Means "FileManagerMenuCommand"
int FMMC_Play( RMuint8* parameters );
int FMMC_Copy( RMuint8* parameters );
int FMMC_ReleaseAll( RMuint8* parameters );

// "FM" Means "FileManager"
void FM_DbxMsgPeek();
static int FM_TaskStep2( TTaskInitParam* tParam, TMsgEntry* tMsg );
int FileManager_Task( TTaskInitParam* tParam );
int FM_MsgProc( int ssTaskId, U32 ulMsgId, int usParam1 , int usParam2 );
int FM_MsgProc_FileBrowser( int ssTaskId, U32 ulMsgId, int usParam1, int usParam2 );


#define FMM_OPEN_FAILED_LONG_PATH						0x00001000
#define FMM_COPY_FAILED_LONG_PATH						0x00002000
#define FMM_MOVE_FAILED_LONG_PATH						0x00003000
#define FMM_COPY_FAILED_NAME_ALREADY_DOES_EXIST		0x00004000
#define FMM_MOVE_FAILED_NAME_ALREADY_DOES_EXIST 		0x00005000
#define FMM_DELETE_FAILED_CLUS_NEXT_IS_OUT_OF_VOLUME	0x00006000
#define FMM_VOLUME_FULL									0x00007000
#define FMM_EXCEPTION_FOUND								0x00008000
#define FMM_WAIT											0x00009000
RMuint32 FM_DrawMsgBox( RMuint32 msg_idx, RMuint32 option=0, uchar_t* msg=0 );


#endif

