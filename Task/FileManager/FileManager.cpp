//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : FileManager Task Source Code File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/FileManager/FileManager.cpp,v $
// $Revision: 1.8 $
// $Date: 2006/06/22 01:44:19 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: FileManager.cpp,v $
// Revision 1.8  2006/06/22 01:44:19  zstein
// - removed unused codes.
// - added a functin checking enjoyed movies.
//
// Revision 1.7  2006/06/12 04:17:30  ywkim
// change printf to DEBUGMSG
// change file access function to low level one
//
// Revision 1.6  2006/05/22 02:55:06  zstein
// - sleep on exiting file browser.
//
// Revision 1.5  2006/04/15 02:56:32  zstein
// - removed TAVI_KEY_HOLDOFF routine.
//
// Revision 1.4  2006/04/13 01:43:30  ywkim
// Change some function for delete
//
// Revision 1.3  2006/04/10 02:05:22  ywkim
// Add _CloseDir() on _xx4CopyRet_CANCEL & _xx4MoveRet_CANCEL
//
// Revision 1.2  2006/04/07 08:25:17  ywkim
// Change many functions for HOST
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*****************************************************************************

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
#include <fcntl.h>
#include <errno.h>
#include <sched.h>

//#include <dac/dac.h>
#include <jpeg/jpeg.h>
#include <mp3/mp3.h>
#include <avi/avi.h>
#include <Lib/LibImage.h>
#include <Lib/LibFileBrowser.h>


#include <Component/object.h>
#include <Component/event.h>
#include <Component/fbdev.h>
#include <Component/timer.h>
#include <Component/image.h>

#include <Task/Task.h>
#include <Task/Msg.h>
#include <Task/AudioPlayer/AudioPlayer.h>
#include <Task/VideoPlayer/VideoPlayer.h>
#include <Task/ImageViewer/ImageViewer.h>
#include <Task/FMRadio/FMRadio.h>
#include <Task/setup/setup.h>

#include "FileManager.h"

#define MAX_FULLPATH_NAME_LENGTH 259
char* funcName = "FileManager";
#define DEBUGINFO printf("### %s:%d ### ==> %s\n", __FILE__, __LINE__, funcName)
extern int errno;


#define MAX(X,Y) (((X)>=(Y))?(X):(Y))	// to find the max between two number.
#define MIN(X,Y) (((X)<=(Y))?(X):(Y))	// to find the min between two number.

/*
    Copy function has some problem.
    So, we remove this.
*/
#define MP_REMOVE_COPY_FUNC     0

#define	__TV_DISPLAY_WIDTH	720
#define	__TV_DISPLAY_HEIGHT	480
#define	__LCD_DISPLAY_WIDTH	320
#define	__LCD_DISPLAY_HEIGHT 	240

tFILEMANAGER_CONFIG*	pFileManager;
extern CFileBrowser*		pLFBrowser;
extern MpegDecoder *		pMpegDecoder;
extern CTopPanel*			g_TopPanel;

static char sTaskName[256];
TTaskInitParam tInitParam;
static int checkpoint;
static TMsgEntry tMsgTemp;
extern int handle_key;
extern keystruct tempkey;

static int iFMTouchPreParam1 = 0;
static int iFMTouchPreParam2= 0;
static int iFMTouchSum= 0;
static int iFMTouchCounter= 0;
static int iFMTouchMeanValue= 0;

// main.cpp
extern TTAVI_GLOBAL_CONFIG* 	pTAVI;
extern BITMAPFILEINFO256 		TAVI_IMAGES[];
extern void showbackground (char *bkgnd);
extern void showbackground (int menuID);
extern RMuint32 render_avifile ( uchar_t* fullname, RMuint32 blockinghandle);
extern RMuint32 MAIN_RenderVideoFile (RMint8 *filename, void *context);
extern RMuint32 fb_isrenderdone (void *context);
extern int TAVI_CreateTask_For_CallByFileManager(  int ssTaskId, TTaskInitParam* tParam );

int FMMC_CC( uchar_t* filename );

// Setup.cpp
extern int Setup_Diskfree(void);

// LibText.cpp
extern CTextEngine_BDF* pT;

// MsgBox 
extern CMsgBox *pMbx;

// Menu Items
CPopUpMenu* pmCurrentMenu;

CPopUpMenu* pmFM_NoFile;
CPopUpMenu* pmFM_SingleFile;
CPopUpMenu* pmFM_SingleFolder;
CPopUpMenu* pmFM_MultiItems;
	
CPopUpMenu* pmFM_Copy_SameFolder;
CPopUpMenu* pmFM_Copy_DiffFolder;
CPopUpMenu* pmFM_Move_SameFolder;
CPopUpMenu* pmFM_Move_DiffFolder;

extern CPopUpSubMenu* psSort;
CVirtualKeyboard* pVirtualKbd;

// function proto type
U32 __DrawPasteDbxContents();
int _ExistFile( uchar_t *u16FileName, RMint64 *p64FileSize, FATFS_ERROR *sdFatfsErr, int nConfirmOverWrite );
int _IsNeededToExitForBatterynKey( int ssTaskId );
RMuint32 _DeleteFile( uchar_t *u16FileName);
RMuint32 _OpenDir( uchar_t *pu16FolderPath, RMuint32 *pFd );
RMuint32 _ReadDir( RMuint32 fd, FATFS_DIR_ENTRY_INFO *pDirEntry, uchar_t *pu16FolderPath );
RMuint32 _CloseDir( RMuint32 fd, uchar_t *pu16FolderPath );
RMuint32 _DeleteFolder(uchar_t *pu16FolderPath );


enum 
{
KILOBYTE = 1024,
MEGABYTE = (KILOBYTE*1024),
GIGABYTE = (MEGABYTE*1024)
};

RMuint64 FM_RefreshDiskSpace()
{
	if ( pLFBrowser->IsThisLinuxFs() ) {
		pFileManager->u64DiskFreeSizeInBytes = pLFBrowser->df_linux("/usb0");
		printf("&&& 10th :: FM_RefreshDiskSpace (linux)= %lld\n", pFileManager->u64DiskFreeSizeInBytes);
		}
	else {
		pFileManager->u64DiskFreeSizeInBytes = pLFBrowser->df_fatfs(1);
		printf("&&& 10th :: FM_RefreshDiskSpace (fatfs)= %lld\n", pFileManager->u64DiskFreeSizeInBytes);
		}
	pFileManager->i32DiskFreeSizeInMB = pFileManager->u64DiskFreeSizeInBytes / MEGABYTE;
	return pFileManager->u64DiskFreeSizeInBytes;
}
void FM_DrawDiskFreeSpace()
{
	char szInfomation[20];
	uchar_t ucInfomation[20];
	uchar_t ucFreeSpaceText[40];
	int nFreeGB, nFreeGBby1K;
	nFreeGB = pFileManager->i32DiskFreeSizeInMB / 1024;
	nFreeGBby1K = (pFileManager->i32DiskFreeSizeInMB - ((pFileManager->i32DiskFreeSizeInMB/1024)*1024))/10;
	sprintf(szInfomation, "%d.%dGB Free", nFreeGB, nFreeGBby1K );
//	printf("-szInfomation : [%s]\n", szInfomation );
	str2ustr( ucFreeSpaceText, szInfomation, strlen(szInfomation) );
	pT->DrawUC16Text( ucFreeSpaceText, 10, 240-16, _WHITE, 12, DT_TRANSPARENCY|DT_ALIGN_RIGHT );
}

void FM_DrawSelectedItemText()
{
	char szInfomation[40];
	uchar_t ucInfomation[40];
	RMuint64 totalsize = pLFBrowser->Get_CurrentFI_Size();
	int eot;

	if( totalsize < KILOBYTE )
		sprintf(szInfomation, "%d items (%dbytes)", pLFBrowser->Get_CurrentFI_Count(), totalsize );
	
	else if( totalsize < MEGABYTE )
		sprintf(szInfomation, "%d items (%dKB)", pLFBrowser->Get_CurrentFI_Count(), (totalsize/KILOBYTE) );

	else if( totalsize < GIGABYTE )
		sprintf(szInfomation, "%d items (%dMB)", pLFBrowser->Get_CurrentFI_Count(), (totalsize/MEGABYTE) );

	else
		{
		int nFreeGB = (totalsize/GIGABYTE);
		RMuint64 nFreeKB = totalsize - (nFreeGB*GIGABYTE);
		int nFreeGBby1K = (int)((totalsize*10/GIGABYTE)-(nFreeGB*10));
		sprintf(szInfomation, "%d items (%d.%1dGB)", pLFBrowser->Get_CurrentFI_Count(), nFreeGB, nFreeGBby1K );
		}

	str2ustr( ucInfomation, szInfomation, strlen(szInfomation) );

	if( pFileManager->SubmenuStatus == _CMD_COPY_STATE )
		{
		DrawItemWithoutRefresh( &TAVI_IMAGES[ICON_COPYMOVE], 1, 10, 240-17, 32, 14);
		pT->DrawUC16Text( ucInfomation, 10+32+2, 240-16, _WHITE, 12, DT_TRANSPARENCY );
		}
	else if( pFileManager->SubmenuStatus == _CMD_MOVE_STATE )
		{
		DrawItemWithoutRefresh( &TAVI_IMAGES[ICON_COPYMOVE], 2, 10, 240-17, 32, 14);
		pT->DrawUC16Text( ucInfomation, 10+32+2, 240-16, _WHITE, 12, DT_TRANSPARENCY );
		}
	else
		{
		pT->DrawUC16Text( ucInfomation, 10, 240-16, _WHITE, 12, DT_TRANSPARENCY );
		}
	
}

void FM_DrawBottomItems( U32 option )
{
	uchar_t ucFreeSpaceText[40];
	DrawBottomGuideBGPannel( SKIP_OSD_REFRESH );

	if( (option&SHOW_BTN_GUIDE) == SHOW_BTN_GUIDE )	{
			DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU|BGPID_SELECT) );
		}
	else	{
		if( pLFBrowser->Get_CurrentFI_Count() > 0 ) {
	 		FM_DrawSelectedItemText();
			}
		else {
				DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU|BGPID_SELECT) );
			}
		}

	FM_DrawDiskFreeSpace();
	if( (option&OSD_REFRESH)==OSD_REFRESH )
		pMpegDecoder->RefreshOsd();
}

void FM_CursorLoopInit()
{
	iFMTouchPreParam1 = 0;
	iFMTouchPreParam2= 0;
	iFMTouchSum= 0;
	iFMTouchCounter= 0;
	iFMTouchMeanValue= 0;
}

RMuint32 FM_DrawMsgBox( RMuint32 msg_idx, RMuint32 option, uchar_t* msg )
{
	uchar_t ucTemp[128];
	char temp[128];

	ClearKeyBuffer();
	
	switch( msg_idx )
		{
		case FMM_OPEN_FAILED_LONG_PATH:
			Stop_FileBrowserTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_OPEN_FAILED_LONG_PATH, -1, -1, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			FM_DbxMsgPeek();
			pMbx->Hide();
			break;
		case FMM_COPY_FAILED_LONG_PATH:
			Stop_FileBrowserTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_COPY_FAILED_LONG_PATH, -1, -1, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			FM_DbxMsgPeek();
			pMbx->Hide();
			break;
		case FMM_MOVE_FAILED_LONG_PATH:
			Stop_FileBrowserTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_MOVE_FAILED_LONG_PATH, -1, -1, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			FM_DbxMsgPeek();
			pMbx->Hide();
			break;
		case FMM_COPY_FAILED_NAME_ALREADY_DOES_EXIST:
			Stop_FileBrowserTimer();
			
			if( msg )
				{
				pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_COPY_FAILED_NAME_ALREADY_DOES_EXIST, 280, 150, 0, DL_BTN_OK_CANCEL );
				}
			else
				{
				pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_COPY_FAILED_NAME_ALREADY_DOES_EXIST, 280, 150, 0, DL_BTN_OK_CANCEL );
				}
			
			pMbx->Show();
			ClearKeyBuffer();
			FM_DbxMsgPeek();
			pMbx->Hide();
			ClearKeyBuffer();
			return pMbx->GetCurrentChoice();
			break;
		case FMM_MOVE_FAILED_NAME_ALREADY_DOES_EXIST:
			Stop_FileBrowserTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_MOVE_FAILED_NAME_ALREADY_DOES_EXIST, 280, 150, 0, DL_BTN_OK_CANCEL );
			pMbx->Show();
			ClearKeyBuffer();
			FM_DbxMsgPeek();
			pMbx->Hide();
			ClearKeyBuffer();
			return pMbx->GetCurrentChoice();
			break;
			
		case FMM_DELETE_FAILED_CLUS_NEXT_IS_OUT_OF_VOLUME:
			Stop_FileBrowserTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_DELETE_FAILED_CLUS_NEXT_IS_OUT_OF_VOLUME, 280, 150, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			FM_DbxMsgPeek();
			pMbx->Hide();
			break;

		case FMM_VOLUME_FULL:
			Stop_FileBrowserTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_VOLUME_FULL, 244, 150, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			FM_DbxMsgPeek();
			pMbx->Hide();
			break;

		case FMM_EXCEPTION_FOUND:
			Stop_FileBrowserTimer();
			sprintf( temp, "Exception Error.|(0x%x)", option );
			str2ustr( ucTemp, temp, strlen(temp) );
			pMbx->Reset( ML_INFORMATION, _ORANGE, ucTemp, 244, 150, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			FM_DbxMsgPeek();
			pMbx->Hide();
			break;

		case FMM_WAIT:
			Stop_FileBrowserTimer();
			pMbx->Reset( ML_INFORMATION, _ORANGE, ML_MSG_PLZ_WAIT, 200, 80, 0, DL_BG_BLANK );
			pMbx->Show();
			ClearKeyBuffer();
			break;
			
		}

	ClearKeyBuffer();
	return TRUE;

}

int FM_ShowFSE( FATFS_ERROR err )	 // FSE; File System Error
{
//	if( err==FATFS_ERROR_VOLUME_FULL || err==FATFS_ERROR_VOLUME_LOW_ON_DISK_SPACE  )
		{
//		return ( FM_DrawMsgBox(FMM_VOLUME_FULL,err) );
		return ( FM_DrawMsgBox(FMM_EXCEPTION_FOUND,err) );

		}
}

int FM_CommonKeyProcess_LEFT()
{
	int isRoot = FALSE;

	if( pLFBrowser->CursorLeft()==0)
		{
		Reset_FileBrowserTimer();
		if( pLFBrowser->Get_CurrentFI_Count()> 0 && pFileManager->SubmenuStatus == _CMD_READY_STATE )
			{
			pLFBrowser->Clear_IL_Only();
			pFileManager->SubmenuStatus = _CMD_READY_STATE;
			}

		pLFBrowser->ReDraw(SKIP_OSD_REFRESH);

		if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()))
			Reset_FileBrowserTimer();
		FM_RefreshDiskSpace();
		FM_DrawBottomItems(OSD_REFRESH);
		}

	else
		{
		isRoot = TRUE;
		}

	return isRoot;		
}


int FM_CommonKeyProcess_RIGHT()
{
//	DEBUGMSG(2,("FM_CommonKeyProcess_RIGHT\n" ));
	if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) )
		Reset_FileBrowserTimer();
	
	return TRUE;	
}

int FM_CommonKeyProcess_UP()
{
//	DEBUGMSG(2,("FM_CommonKeyProcess_UP\n" ));

	pLFBrowser->CursorUp();
	pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
	if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) )
		Reset_FileBrowserTimer();

	FM_DrawBottomItems(OSD_REFRESH);

	return TRUE;	
}

int FM_CommonKeyProcess_DOWN()
{
//	DEBUGMSG(2,("FM_CommonKeyProcess_DOWN\n" ));

	pLFBrowser->CursorDown();
	pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
	if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) )
		Reset_FileBrowserTimer();

	FM_DrawBottomItems(OSD_REFRESH);


	return TRUE;	
}

int FM_CommonKeyProcess_Select()
{
	DEBUGMSG(1,("FM_CommonKeyProcess_Select\n" ));

	if( pLFBrowser->ItemList.Count() < 1 )
		return FALSE;

	if( pFileManager->SubmenuStatus == _CMD_READY_STATE )
		{
		if( pLFBrowser->IsIL() )	// IsSelected?
			pLFBrowser->Remove_IL_CheckTag(ENCLOSE_UNDER_CURSOR);
		else
			pLFBrowser->Add_IL_CheckTag(ENCLOSE_UNDER_CURSOR);

		}
	else
		{
		// _CMD_COPY_STATE/_CMD_MOVE_STATE 상태에서
		// 항목이 선택된 경우 COPY/MOVE 취소, IL/SIL 초기화, 해당항목을 선택
		pLFBrowser->Clear_IL_SIL_All();
		pFileManager->SubmenuStatus = _CMD_READY_STATE;
		pLFBrowser->Add_IL_CheckTag(ENCLOSE_UNDER_CURSOR);
		}

	if( !pLFBrowser->IsLastItem() )
		pLFBrowser->CursorDown();

	pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
	if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) )
		Reset_FileBrowserTimer();

	FM_DrawBottomItems(OSD_REFRESH);

	return TRUE;	
}


int FM_CommonKeyProcess_EXIT()
{

//	DEBUGMSG(2,("FM_CommonKeyProcess_EXIT\n" ));

	if ( toggle_display == 0 )              // for lcd
		pMpegDecoder->SetVideoDestination (0, 0, 320, 240);
	else                                                    // for TV
		pMpegDecoder->SetVideoDestination (0, 0, 720, 480);

	return -1;
}

void FM_DbxMsgPeek()
{
	TMsgEntry tMsg;
	for(;;)
	{
//		RELOAD_WATCHDOGTIMER;
	
//		if( AppEventCheck( pFileManager->TaskId, &tMsg) == 0 )
//			{ MSG_Send( pFileManager->TaskId, &tMsg ); break; }
	
		if( MSG_Peek( pFileManager->TaskId, &tMsg ) == 0)
			{
			if( FM_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) < 0)
				{
				break;
				}
			}
	}	
}

int FM_MsgProc_FileBrowser( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
//	DEBUGMSG(2,("[FM_MsgProc_FileBrowser] : ssParam1[0x%x]\n",  ssParam1));

	//////////////////////////////////////////////////////////////////////
	//
	//	FM_MsgProc_FileBrowser
	//	Touch Pad Message Process
	//
	if( ulMsgId == MSG_TOUCHPAD ) {
		if( ssParam1 == 1 ) FM_CommonKeyProcess_DOWN();
		else if( ssParam1 == -1 ) FM_CommonKeyProcess_UP();
    }

	//////////////////////////////////////////////////////////////////////
	//
	//	FM_MsgProc_FileBrowser
	//	Key Message Process
	//
	else if( ulMsgId == MSG_BUTTON_UP )
		{

		switch( ssParam1 )
			{
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_UP: FM_CommonKeyProcess_UP(); break;
			case TAVI_KEY_DOWN: FM_CommonKeyProcess_DOWN(); break;
			case TAVI_KEY_LEFT: 
				FM_CommonKeyProcess_LEFT();
				break;
			case TAVI_KEY_EXIT:
				if( FM_CommonKeyProcess_LEFT() )
					return -1;
				break;
			case TAVI_KEY_EQ:	FM_CommonKeyProcess_Select(); break;
			case TAVI_KEY_RIGHT:
			case TAVI_KEY_ENTER:
				int FullpathNameLength;
				
				if( pLFBrowser->ItemList.Count() == 0 ) break;

				FullpathNameLength =  ustrlen(pLFBrowser->CurrentFullPathDir) + pLFBrowser->GetCurrentListItemNameLength() + 1;

				// 현재 선택된 파일의 FullPath가 259자를 초과한다면 SIL해제후 이후과정 무시
				if( FullpathNameLength > MAX_FULLPATH_NAME_LENGTH )
					{
					FM_DrawMsgBox(FMM_OPEN_FAILED_LONG_PATH);
					pLFBrowser->Clear_IL_SIL_All();
					pFileManager->SubmenuStatus = _CMD_READY_STATE;
					pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
					FM_DrawBottomItems(OSD_REFRESH);					
					break;
					}

				//현재 디렉토리가 선택목록에 포함되어 있었다면 SIL해제
				if( pLFBrowser->IsSIL() )
					{
					DEBUGMSG(1,("-pLFBrowser->IsSIL(%d) : TRUE\n",pLFBrowser->GetCurrentChoice() ));
					DEBUGMSG(1,("-pLFBrowser->ClearSIL())\n"));
					pLFBrowser->Clear_IL_SIL_All();
					pFileManager->SubmenuStatus = _CMD_READY_STATE;
					}
				else
					DEBUGMSG(1,("-pLFBrowser->IsSIL(%d) : FALSE\n",pLFBrowser->GetCurrentChoice() ));

				if( !pLFBrowser->Select(SKIP_OSD_REFRESH) ) {
					if( pFileManager->SubmenuStatus == _CMD_READY_STATE )
						pLFBrowser->Clear_IL_SIL_All();

					pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
					if( pLFBrowser->IsThisLinuxFs() ) FM_RefreshDiskSpace();
					FM_DrawBottomItems(OSD_REFRESH);
				}

				
				break;
				
						
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_MENU:

				if( pLFBrowser->Get_CurrentFI_Count() == 0 )
					{
					pFileManager->SubmenuStatus = _CMD_READY_STATE;
					ASSERT( !pLFBrowser->SelectedItemList.Count() );
					}
					
				switch( pFileManager->SubmenuStatus )
					{
					
					case _CMD_COPY_STATE:

						if( pLFBrowser->Compare_SD_Folder() )
							pmCurrentMenu = pmFM_Copy_SameFolder;
						else
							pmCurrentMenu = pmFM_Copy_DiffFolder;

						if( !pmCurrentMenu->IsVisible() ) 
							{
							Stop_FileBrowserTimer();
							pmCurrentMenu->Show();
							}
						else 
							{
							pmCurrentMenu->Hide(0);						
							Reset_FileBrowserTimer();
							}

						break;

					case _CMD_MOVE_STATE:
						if( pLFBrowser->Compare_SD_Folder() )
							pmCurrentMenu = pmFM_Move_SameFolder;
						
						else
							pmCurrentMenu = pmFM_Move_DiffFolder;

						if( !pmCurrentMenu->IsVisible() ) 
							{
							Stop_FileBrowserTimer();
							pmCurrentMenu->Show();
							}
						
						else 
							{
							pmCurrentMenu->Hide(0);
							Reset_FileBrowserTimer();
							}

						break;
						

					case _CMD_READY_STATE:
					default:

						if( pLFBrowser->ItemList.Count() < 1 )
							pmCurrentMenu = pmFM_NoFile;

						else if( pLFBrowser->Get_CurrentFI_Count() < 1 )
							{

							if( pLFBrowser->GetCurrentListItemType() <= _NETWORK_DIRECTORY )
								pmCurrentMenu = pmFM_SingleFolder;

							else if( pLFBrowser->GetCurrentListItemType() <= _TEXT_FILE )
								pmCurrentMenu = pmFM_SingleFile;

							else
								pmCurrentMenu = pmFM_SingleFolder;

							}
						else
							{
							pmCurrentMenu = pmFM_MultiItems;
							}

						
						if( !pmCurrentMenu->IsVisible() ) 
							{
							Stop_FileBrowserTimer();
							pmCurrentMenu->Show();
							}
						else 
							{
							pmCurrentMenu->Hide(0);
							Reset_FileBrowserTimer();
							}
						
						break;
					}
							
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LEXIT:				
			case TAVI_KEY_MODE:
			case TAVI_KEY_LMODE:
			case TAVI_KEY_SHUTDOWN:
				return -1;
				break;
			}
		}

	return 0;
}


int FM_MsgProc( 		int ssTaskId,
					U32 ulMsgId,
					int ssParam1,
					int ssParam2	)
{
	RMint64 stc, t;

#if 0
	if( ulMsgId != MSG_BATTERY )
		DEBUGMSG(2,("FM_MsgProc() : ssTaskId[0x%x], ulMsgId [0x%x], ssParam1[0x%x], ssParam2[%x]\n", ssTaskId, ulMsgId, ssParam1, ssParam2 ));
#endif

	switch( ulMsgId )
	{
		case MSG_CREATE:	DEBUGMSG(2,("[1]FM_MsgProc() : MSG_CREATE()\n")); 	ClearKeyBuffer(); break;
		case MSG_DRAW: 	DEBUGMSG(2,("[1]FM_MsgProc() : MSG_DRAW()\n"));	ClearKeyBuffer(); break;
		case MSG_TIMER:
			DEBUGMSG(2,("[1]FM_MsgProc() : MSG_TIMER()\n"));
			if( (pLFBrowser->isVisible()) && ssParam1==2 && ssParam2== 2 )
				pLFBrowser->ProcFBAutoScrollTimer(ssTaskId, ulMsgId, ssParam1, ssParam2);
			break;

		case MSG_USBSTORAGE:
			printf("++ USB HOST MESSAGE \n" );
			if( pLFBrowser->isVisible() )
				{

				if( ssParam1 == 1 )
					{// Connected
					pMbx->Reset( ML_INFORMATION, _GREEN, ML_USB_CONNECT, -1, -1, 0, DL_BG_HOST );
					pMbx->Show();

					}
				
				else	
					{// Disconnected
					pMbx->Reset( ML_INFORMATION, _GREEN, ML_USB_DISCONNECT, -1, -1, 0, DL_BG_HOST );
					pMbx->Show();
					}
				
				for(int i=0;i<8;i++)
					sleep(1);
				
				pMbx->Hide();
				pLFBrowser->SetActiveMode( _FBMODE_FILEMANAGER, FM_MsgProc_FileBrowser, TRUE );
				pLFBrowser->Show();
				FM_RefreshDiskSpace();
				FM_DrawBottomItems(OSD_REFRESH);
				}
		
			break;

		case MSG_TOUCHPAD:

			if( pVirtualKbd != NULL )
				return ( pVirtualKbd->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );

			if( pMbx->IsVisible() )
				return ( pMbx->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );

			else if( pmCurrentMenu != NULL && pmCurrentMenu->IsVisible() )				
				return ( pmCurrentMenu->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
			
			else {
				// action like enter key if touch tapped.
				if( ssParam1 == 0 && ssParam2 == 1 ) {
					ulMsgId = MSG_BUTTON_UP;
					ssParam1 = TAVI_KEY_ENTER;
					}
				if( FM_MsgProc_FileBrowser( ssTaskId, ulMsgId, ssParam1, ssParam2 ) == -1 )
					return -1;
				}
			break;
		
		case MSG_BUTTON_UP:
//			DEBUGMSG(2,("[1]FM_MsgProc() : MSG_BUTTON_UP()\n"));
            #if 0
			if( ssParam1 == TAVI_KEY_HOLD )
				{
				pTAVI->isHoldState = TRUE;
				if( pLFBrowser->isVisible() )
					{
					g_TopPanel->DrawHold(1);
					pMpegDecoder->RefreshOsd();
					Reset_FileBrowserTimer();
					}
				}

			else if( ssParam1 == TAVI_KEY_HOLDOFF )
				{

				pTAVI->isHoldState = FALSE;
				if( pLFBrowser->isVisible())
					{
					g_TopPanel->DrawHold(0);
					pMpegDecoder->RefreshOsd();
					Reset_FileBrowserTimer();
					}

				}
			#endif

			if( pVirtualKbd != NULL )
				return ( pVirtualKbd->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
			
			else if( pMbx->IsVisible() )
				return ( pMbx->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );

			else if( pmCurrentMenu != NULL && pmCurrentMenu->IsVisible() )				
				return ( pmCurrentMenu->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
			
			else if( FM_MsgProc_FileBrowser( ssTaskId, ulMsgId, ssParam1, ssParam2 ) == -1 )
				return -1;

			break;
	}

	return 0;
}

int FMMC_Play( RMuint8* parameters )
{
	int ret = FALSE;

	DEBUGMSG(1,("FMMC_Play()\n"));
	DEBUG_U16MSG( pLFBrowser->Filename , "-pLFBrowser->Filename");

	pLFBrowser->Select(SKIP_OSD_REFRESH);

	switch( CheckExtension_UC16( pLFBrowser->Filename ) )
		{
		case _VIDEO_FILE:

			DEBUGMSG(1,("-CheckExtension : _VIDEO_FILE\n"));
			
			sprintf( sTaskName, "%s", "VideoPlayer" );
			tInitParam.pTaskName	= sTaskName;
			tInitParam.ssTaskId 		= 2;
			TAVI_CreateTask_For_CallByFileManager( tInitParam.ssTaskId, &tInitParam );

			break;

		case _AUDIO_FILE:
			DEBUGMSG(1,("-CheckExtension : _AUDIO_FILE\n"));

			sprintf( sTaskName, "%s", "AudioPlayer" );
			tInitParam.pTaskName	= sTaskName;
			tInitParam.ssTaskId 		= 1;
			TAVI_CreateTask_For_CallByFileManager( tInitParam.ssTaskId, &tInitParam );
			
			break;

		case _IMAGE_FILE:
			DEBUGMSG(1,("-CheckExtension : _IMAGE_FILE\n"));

			sprintf( sTaskName, "%s", "ImageViewer" );
			tInitParam.pTaskName	= sTaskName;
			tInitParam.ssTaskId 		= 3;
			TAVI_CreateTask_For_CallByFileManager( tInitParam.ssTaskId, &tInitParam );
			
			break;

		case _TEXT_FILE:
			DEBUGMSG(1,("-CheckExtension : _TEXT_FILE\n"));
/*
			sprintf( sTaskName, "%s", "TextViewer" );
			tInitParam.pTaskName	= sTaskName;
			tInitParam.ssTaskId 		= 4;
			TAVI_CreateTask_For_CallByFileManager( tInitParam.ssTaskId, &tInitParam );
*/
			break;

		default:
			return 0;
			break;
		}

	pmCurrentMenu->Hide(0);
	pmCurrentMenu = NULL;
	
	return ret;
}

// FMMC; File Manager Menu  Command
int FMMC_Copy( RMuint8* parameters )
{
	int ret = FALSE;

	DEBUGMSG(2,("\nFMMC_Copy()\n"));
	DEBUGMSG(2,("-pLFBrowser->SelectedItemList.Count():%d\n", pLFBrowser->SelectedItemList.Count() ));

	if( pLFBrowser->Get_CurrentFI_Count() == 0)
		pLFBrowser->Add_IL_To_SIL(ENCLOSE_UNDER_CURSOR);
	else
		pLFBrowser->Add_IL_To_SIL(ENCLOSE_SELECTED);

	pFileManager->SubmenuStatus = _CMD_COPY_STATE;
	ret = TRUE;


	pmCurrentMenu->Hide(0);
	pmCurrentMenu = NULL;
	
	pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	
	return ret;
}


int FMMC_Move( RMuint8* parameters )
{
	int ret = FALSE;

	DEBUGMSG(2,("\nMMC_Move()\n"));

	if( pLFBrowser->Get_CurrentFI_Count() == 0)
		pLFBrowser->Add_IL_To_SIL(ENCLOSE_UNDER_CURSOR);
	else
		pLFBrowser->Add_IL_To_SIL(ENCLOSE_SELECTED);

	pFileManager->SubmenuStatus = _CMD_MOVE_STATE;
	ret = TRUE;

	pmCurrentMenu->Hide(0);
	pmCurrentMenu = NULL;

	pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();

	return ret;
}

int DeleteRecursive( const uchar_t* DesDir, RMint64 *pDeletedItemSize )
{
	CStack< CString > DirStack( 1024 );
	uchar_t TempString[2048];
	CString CurDir;

	FATFS_ERROR err;
	FATFS_ERROR err_temp;
	FATFS_DIR_ENTRY_INFO DirEntry;

	RMuint32 hDir;
	
	int progress;
	RMint64 i64FileSize;
	RMuint32	fdsrc;
	uchar_t udst[520];	
	int	i;
	
	DEBUGMSG(1,( "+DeleteRecursive\n" ));


	//////////////////////////////////////////////////////////////////////////////////////////
	// DELETE LOOP
	for( i=0; i<pLFBrowser->SelectedItemList.Count(); i++ ) {
		ustrcpy( udst, pLFBrowser->CurrentFullPathDir ); 
		adduchar( udst, 0x002f ); 
		ustrcat( udst, (uchar_t*)pLFBrowser->SelectedItemList[i]->name );

		pMbx->BuildProgressContents(	0, 
									(uchar_t*)pLFBrowser->SelectedItemList[i]->name, 
									pLFBrowser->CurrentFullPathDir, 
									pLFBrowser->CurrentFullPathDir );
		pMpegDecoder->RefreshOsd();
		//////////////////////////////////////////////////////////////////////////////////////////
		// FOLDER
		if( (pLFBrowser->SelectedItemList[i]->attributes&0x10) )	{
			///////////////////////////////////////////////////////////////////////////////
			// this is 
			CurDir.SetString((char*)udst,1);
			DirStack.Push( CurDir );
			printf("*** 61th :: Delete Recursive : current dir [\n");	// for debug
			CurDir.Print();
			printf("] pushed\n");
			while(DirStack.IsEmpty()==false) {
				// 1. CurDir 디렉토리를 연다
				CurDir = DirStack.Pop();
				printf("*** 69th :: Enter sub directory : [");	// for debug
				CurDir.Print();
				printf("]  poped\n");					// for debug
			
				
				///////////////////////////////////////////////////////////////////////////////
				// Open directory
				err = (FATFS_ERROR)_OpenDir( (uchar_t *)CurDir, &hDir );
				while( err == FATFS_ERROR_NO_ERROR )	{
					//////////////////////////////////////////////////////////////////////////////
					// Check Key & battery					
					if ( _IsNeededToExitForBatterynKey(pFileManager->TaskId ) > 0 ) {
						printf("[!] Cancel By User\n");
						_CloseDir( hDir, (uchar_t *)CurDir );
						ClearKeyBuffer();
						return -2; 
						}			
			
					//////////////////////////////////////////////////////////////////////////////
					// Read entries
					err =(FATFS_ERROR)_ReadDir( hDir, &DirEntry, (uchar_t *)CurDir );
					if( err != FATFS_ERROR_NO_ERROR ) break;
					
					if( DirEntry.name[0] == '.' && DirEntry.name[1] == 0 && DirEntry.name[2] == 0 && DirEntry.name[3] == 0 ) continue;
					if( DirEntry.name[0] == '.' && DirEntry.name[1] == 0 &&DirEntry.name[2] == '.' && DirEntry.name[3] == 0 ) continue;
			
					ustrcpy( TempString, (uchar_t*)CurDir ); adduchar( TempString, 0x002f ); ustrcat( TempString, (uchar_t*)DirEntry.name );
			
					if( DirEntry.attributes&ATTR_DIRECTORY )	{
						// 3. Directory 일 경우 스택에 저장하여 나중에 처리
						DirStack.Push( CurDir );
						
						printf("*** 62th:: Push CurDir to stack \n\n"); // for debug
						CurDir.Print(); 			// for debug
						printf(" -- pushed\n"); 	// for debug					
			
						CurDir.SetString((char*)TempString, 1); 		
						DirStack.Push( CurDir );
						
						printf("*** 62th:: Push SubDir to stack \n\n"); // for debug
						CurDir.Print(); 			// for debug
						printf(" -- pushed\n"); 	// for debug
						break;
						}
					else	{
						// 4. File 인 경우 바로 삭제
						printf("*** 63th:: Delete file \n\n");	// for debug
			
						err_temp	 =(FATFS_ERROR) _DeleteFile( (RMuint16*)TempString );
			
						*pDeletedItemSize +=DirEntry.file_size; 		
			
						pMbx->BuildProgressContents(0,(uchar_t*)DirEntry.name, (uchar_t*)CurDir, 0);
						progress = (int)((*pDeletedItemSize*100)/pLFBrowser->Get_CurrentFI_Size());
						pMbx->BuildProgressBar( progress );
						
						if(err_temp>0) { 
							FM_DrawMsgBox(FMM_DELETE_FAILED_CLUS_NEXT_IS_OUT_OF_VOLUME);
							printf("\t FileDelete Failed : %x\n", err_temp); 
							_CloseDir( hDir, (uchar_t *)CurDir );
							return -1; 
							}
						}
					}
				//////////////////////////////////////////////////////////////////////////////
				// Close directory	
				_CloseDir( hDir, (uchar_t *)CurDir );
			
				//////////////////////////////////////////////////////////////////////////////
				// Delete directory 		
				if ( pLFBrowser->IsThisFatFs() ) {
					// for fatfs
					if ( err == FATFS_ERROR_NO_MORE_ENTRIES ) {
						printf("*** 66th :: No more entries \n");	// for debug
						_DeleteFolder( CurDir );
						}		
					}
				else {
					// for linuxfs
					if ( err== 11 ) {
						// end of entry
						printf("*** 65th :: Delete folder\n");		// for debug
						_DeleteFolder( CurDir );
						}
					}
				}

			}
		//////////////////////////////////////////////////////////////////////////////////////////
		// FILE
		else {
			//////////////////////////////////////////////////////////////////////////////
			// Check Key & battery					
			if ( _IsNeededToExitForBatterynKey(pFileManager->TaskId ) > 0 ) {
				printf("[!] Cancel By User\n");
				ClearKeyBuffer();
				return -2;
				}			
			
//			_ExistFile((RMuint16*)udst, &i64FileSize, &err, 0);
//			Completed+=i64FileSize;

			err = (FATFS_ERROR) _DeleteFile( udst );

			*pDeletedItemSize+= pLFBrowser->SelectedItemList[i]->file_size;
			progress = (int)( (*pDeletedItemSize*100) / pLFBrowser->Get_CurrentFI_Size() );
			pMbx->BuildProgressBar( progress );

			if( err != FATFS_ERROR_NO_ERROR ) {
				FM_DrawMsgBox(FMM_DELETE_FAILED_CLUS_NEXT_IS_OUT_OF_VOLUME);
				printf("\t FileDelete Failed : %x\n", err_temp); 
				return -1;
				}
			
			printf(" -- 22th :: FMMC_Delete : file size=%d: progress[%d%%]\n", pLFBrowser->SelectedItemList[i]->file_size, progress );	// for debug
			}

		}

	return 0;
}



int FMMC_Delete( RMuint8* parameters )
{
	TMsgEntry tMsg;
	TTaskInitParam tParam;
	U32 TitleRes;
	
	FATFS_ERROR err;

	char TmpString[260];
	RMint64 Completed=0;
	int	progress=0;
	int	i;
	int	nReturn;
	
	DEBUGMSG(1,("\n+FMMC_Delete()\n"));

	if( pLFBrowser->ItemList.Count() < 1 )
		return FALSE;

	/////////////////////////////////////////////
	// Confirm Delete
	pMbx->Reset( ML_DELETE, _ORANGE, ML_MSG_DELETE_CONFIRM, -1, -1, NULL, DL_BG_BLANK|DL_BTN_OK_CANCEL);
	pMbx->Show();
	FM_DbxMsgPeek();
	pMbx->Hide();

	if( pMbx->GetCurrentChoice() == 2 ) 
		{
		FMMC_ReleaseAll(0);
		pLFBrowser->Refresh(SKIP_OSD_REFRESH);
		FM_RefreshDiskSpace();
		FM_DrawBottomItems(OSD_REFRESH);
		if( pLFBrowser->ItemList.Count() > 0 ) {Reset_FileBrowserTimer();}
		ClearKeyBuffer();
		return FALSE;
		}

	pMbx->Reset( ML_DELETE, _GREEN, (uchar_t*)NULL, -1, -1, NULL, DL_PROGRESS_MODE );
	pMbx->Show();
	pMbx->BuildProgressBar( progress );

	printf("--- 30th :: Get_CurrentFI_Count=%d\n",  pLFBrowser->Get_CurrentFI_Count() ); // for debug
	if( pLFBrowser->Get_CurrentFI_Count() == 0)
		pLFBrowser->Add_IL_To_SIL(ENCLOSE_UNDER_CURSOR);
	else
		pLFBrowser->Add_IL_To_SIL(ENCLOSE_SELECTED);
	printf("--- 31th :: Get_CurrentFI_Count=%d\n",  pLFBrowser->Get_CurrentFI_Count() ); // for debug

	nReturn = DeleteRecursive(NULL, &Completed);
	
	//////////////////////////////////////////////////////////////////////////////////////////
	if ( nReturn == 0 ) {
		pMbx->BuildProgressBar( 100 );
		}
	printf("----------------------------------------------------\n");

	for(i=0;i<20;i++)	sleep(1);

	pMbx->Hide();

	FMMC_ReleaseAll(0);
	pLFBrowser->Refresh(SKIP_OSD_REFRESH);
	FM_RefreshDiskSpace();
	FM_DrawBottomItems(OSD_REFRESH);
	if( pLFBrowser->ItemList.Count() > 0 ) {Reset_FileBrowserTimer();}
	ClearKeyBuffer();
	printf("----------------------------------------------------\n");

	DEBUGMSG(1,("\n-FMMC_Delete()\n"));
	return TRUE;
}

int FMMC_Rename( RMuint8* parameters )
{
	FATFS_DIR_ENTRY_INFO *pD;
	RMuint32 Dir;
	RMuint8 bTypeDirOrFile;
	FATFS_ERROR err;
	int FullpathNameLength;

	FullpathNameLength = ustrlen(pLFBrowser->CurrentFullPathDir);
	pD = &(pLFBrowser->ItemList[pLFBrowser->GetCurrentChoice()]->D);

	DEBUGMSG(2,("\n+FMMC_Rename()\n"));
	pVirtualKbd = new CVirtualKeyboard( ML_RENAME, (uchar_t*)pD->name, DL_BTN_OK_CANCEL);
	pVirtualKbd->SetUsedStringBuffer(FullpathNameLength);
	pVirtualKbd->Show();

	FM_DbxMsgPeek();

	if( pVirtualKbd->IsConfirmed() )
		{
		DEBUGMSG(1,("-Rename ... Confirmed\n"));
		
		if( (pD->attributes&0x10) )
			bTypeDirOrFile = ATTR_DIRECTORY;
		else
			bTypeDirOrFile = ATTR_FILE;
		if( pLFBrowser->IsThisLinuxFs(pLFBrowser->CurrentFullPathDir) ) {
			// linuxfs
			char DstName[1024];
			char SrcName[1024];
			uchar_t uDstName[260];
			uchar_t uSrcName[260];

			ustrcpy( uSrcName, pLFBrowser->CurrentFullPathDir );
			adduchar( uSrcName, (uchar_t)'/');
			ustrcat( uSrcName, (uchar_t*)pD->name);

			ustrcpy( uDstName, pLFBrowser->CurrentFullPathDir );
			adduchar( uDstName, (uchar_t)'/');
			ustrcat( uDstName, (uchar_t*)pVirtualKbd->GetString());
			
			pLFBrowser->ConvertUnicodeToUniXlate(SrcName, uSrcName);
			pLFBrowser->ConvertUnicodeToUniXlate(DstName, uDstName);
			
			if ( (rename ( SrcName, DstName ) ) < 0 ) {
				// error 
				printf("+++ FMMC_Rename :: linuxfs DstName[%s], errno=%d \n",DstName, errno);
				}
			}
		else {
			// fatfs
			err = fatfs_wdopen( (RMuint16*)pLFBrowser->CurrentFullPathDir, &Dir);
			if(err != FATFS_ERROR_NO_ERROR) { ASSERT(0); printf("\t FMMC_Rename fatfs_wdopen Failed : %x\n", err); fatfs_dclose( Dir ); }

			err = fatfs_wdrename(Dir, (RMuint16*)pD->name, (RMuint16*)pVirtualKbd->GetString(), bTypeDirOrFile );
			if(err != FATFS_ERROR_NO_ERROR) { ASSERT(0); printf("\t FMMC_Rename fatfs_wdrename Failed : %x\n", err); fatfs_dclose( Dir ); }

			fatfs_dclose(Dir);
			}
		}
	else
		{
		DEBUGMSG(1,("-Rename ... Cancel\n"));
		}

	pVirtualKbd->Hide();
	delete pVirtualKbd;
	pVirtualKbd = NULL;
	
	DEBUGMSG(2,("\n-FMMC_Rename()\n"));

	FMMC_ReleaseAll(0);
	pLFBrowser->Refresh(SKIP_OSD_REFRESH);
	FM_DrawBottomItems(OSD_REFRESH);
	ClearKeyBuffer();
	
	return TRUE;
}

int _IsLessFreeSpace( RMint64 i64FileSize, RMuint64 u64TempFreeDiskSpace)
{
	TMsgEntry tMsg;
	TTaskInitParam tParam;
	tParam.ssTaskId = 8;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. 디스크 용량이 충분한지 확인
	//     충분하지 않으면 메세지박스 출력 후 복사 중지

	if( u64TempFreeDiskSpace < (RMuint64)i64FileSize )
		{
		DEBUGMSG(2,("-Failed To Paste; Not Enough Disk Space.\n"));
		printf("++ Not Enough Free space =%lld\n", u64TempFreeDiskSpace );
		
		pMbx->Reset( ML_INFORMATION, _ORANGE, ML_DISK_FULL, -1, -1, NULL, DL_BG_ICON|DL_BTN_OK_ONLY );
		pMbx->Show();
		FM_TaskStep2( &tParam,  &tMsg );
		pMbx->Hide();

		pmCurrentMenu->Hide(0);
		pmCurrentMenu = NULL;
		pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
		FM_DrawBottomItems(OSD_REFRESH);
		return 1;			
		}
	else
		return -1;
}

int _ExistFile( uchar_t *u16FileName, RMint64 *p64FileSize, FATFS_ERROR *sdFatfsErr, int nConfirmOverWrite )
{
	RMuint32		fd;
	RMuint32 	nReturn4MsgBox;
	int			nReturn;
	CString		szTmp;

	FILE* pfd;
	char szFilePath[260*2];
	eCURRENT_DIR_FS_TYPE filesystem;

	filesystem = ((pLFBrowser->IsThisLinuxFs(u16FileName))?LINUXFS:FATFS);
	print_ustr(u16FileName, "--- 50th :: _ExistFile =" );		// for debug
	
	if ( filesystem == FATFS ) {
		*sdFatfsErr=fatfs_wfopen( (RMuint16*)u16FileName, _O_RDONLY|_O_FIX_CORRUPTED, &fd );

		if ( *sdFatfsErr == FATFS_ERROR_NO_ERROR ) {
			fatfs_fsize64( fd, p64FileSize );
			fatfs_fclose( fd );
			nReturn = 2;
			}
		else {
			nReturn = -1;
			DEBUGMSG(1,("--- 51th ::  _ExistFile : fatfs_wfopen : error =0x%x\n", *sdFatfsErr ));
			}
		}
	else {
		pLFBrowser->ConvertUnicodeToUniXlate(szFilePath, u16FileName);
		if ( (pfd=fopen( szFilePath, "r" ) )> 0) {
			fseek( pfd, 0L, SEEK_END);
			*p64FileSize = (RMint64) ftell( pfd );
			DEBUGMSG(1,("--- 52th :: _IsExitFile : fopen[%s], size=%lld\n", szFilePath, *p64FileSize));
			fclose(pfd);
			nReturn = 2;
			}
		else {
			nReturn = -1;
			*sdFatfsErr = (FATFS_ERROR)errno;
			DEBUGMSG(1,("--- 53th :: _IsExitFile :  fopen[%s] NoExist, errno=%d\n", szFilePath, *p64FileSize));
			}
		
		}
	
	if ( nConfirmOverWrite ) {
		if ( *sdFatfsErr == FATFS_ERROR_NO_ERROR ) {
			// target file aleady exists

			nReturn4MsgBox = FM_DrawMsgBox( FMM_COPY_FAILED_NAME_ALREADY_DOES_EXIST, 0 );
			__DrawPasteDbxContents();

			if( nReturn4MsgBox == 1 ) {		// yes overwrite
				DEBUGMSG(1,("++ _ExistFile :: file exitst & overwrite : \n"));		// for debug
				nReturn = 1;
				}
			else {						// no cancel
				DEBUGMSG(1,("++ _ExistFile :: file exist & next : \n"));			// for debug
				nReturn = 2;
				}
			}
		else {
			// taget file don't exit
			nReturn = -1;
			}
		}
		
	return nReturn;
}

int _IsLongPathName(RMuint16* PathName)
{	
	int nReturn;
	int FullpathNameLength;


	FullpathNameLength =  ustrlen((uchar_t*)PathName);
	// 현재 선택된 파일의 FullPath가 259자를 초과한다면 SIL해제후 이후과정 무시
	if( FullpathNameLength > MAX_FULLPATH_NAME_LENGTH ) {
		if( pFileManager->SubmenuStatus == _CMD_COPY_STATE ) {
			FM_DrawMsgBox(FMM_COPY_FAILED_LONG_PATH);
			__DrawPasteDbxContents();
			}
		else if( pFileManager->SubmenuStatus == _CMD_MOVE_STATE ) {
			FM_DrawMsgBox(FMM_MOVE_FAILED_LONG_PATH);
			__DrawPasteDbxContents();
			}
		nReturn = 1;
		}
	else {
		nReturn = -1;
		}

	return nReturn;
}
int _CreateTargetFolder( uchar_t *u16SourceFolder, uchar_t *u16TargetFolder)
{
	int	nReturn;
	uchar_t usrc[520];
	uchar_t udst[520];
	uchar_t utmp[520];
	
	FATFS_ERROR err_temp;
	RMuint32 hDirTar;
	eCURRENT_DIR_FS_TYPE filesystem;

	nReturn = 0;
	filesystem = ((pLFBrowser->IsThisLinuxFs(u16TargetFolder))?LINUXFS:FATFS);


	if ( filesystem == FATFS ) {
		ustrcpy( usrc, u16SourceFolder );
		GetLastItem_In_UC16FullDirString_WithUserDefineSplitter(utmp, usrc, (uchar_t)'/');
		GetParentDir_In_UC16FullDirString(udst, u16TargetFolder);

		err_temp = fatfs_wdopen( (RMuint16*)udst, &hDirTar );
		if( err_temp != FATFS_ERROR_NO_ERROR ) {
			nReturn = -1;
			}

		err_temp = fatfs_wdcreate_dir( hDirTar, (RMuint16*) utmp );
		if( err_temp != FATFS_ERROR_NO_ERROR &&  err_temp != FATFS_ERROR_NAME_ALREADY_DOES_EXIST ) {
			if( err_temp == FATFS_ERROR_NAME_DOES_EXIST_AS_FILE ) {
				nReturn = -2;
				}
			else {
				nReturn = -1;
				}
			}
		fatfs_dclose(hDirTar);
		
		DEBUGMSG(1,("--- 102th :: _CreateTargetFolder : ret=%d, fatfs_wdcreate_dir =", nReturn));	// for debug
		//print_ustr(u16TargetFolder, 0);							// for debug
		}
	else {
		pLFBrowser->ConvertUnicodeToUniXlate( (char *)udst, u16TargetFolder);
		if ( (nReturn=mkdir( (char *)udst, 0777) )< 0 ) {
			nReturn = -1;
			if ( errno == EEXIST ) {
				nReturn = -2;
				}
			}
		DEBUGMSG(1,("--- 102th :: _CreateTargetFolder : mkdir[%s] =%d, errno=%d\n", (char *)udst, nReturn, errno ));	// for debug
		}
	
	return nReturn;
}

int _IsNeededToExitForBatterynKey( int ssTaskId )
{
	int nReturn;
	TMsgEntry tMsg;

	nReturn = -1;
	
	if( MSG_Peek_Ex( ssTaskId, &tMsg, (MSG_PEEK_BATTERY|MSG_PEEK_KEY|MSG_PEEK_USB|MSG_PEEK_POWER))==0 ) {
		if ( tMsg.ulMsgId == MSG_BUTTON_UP ) {
			switch ( tMsg.usParam1) {
//				case TAVI_KEY_ENTER:
				case TAVI_KEY_EXIT:
				case TAVI_KEY_LEXIT:
				case TAVI_KEY_SHUTDOWN:
					nReturn = 1;
					break;
				}
			}
		}
	return nReturn;
}

RMuint32 __CloseFile( RMuint32 fd, eCURRENT_DIR_FS_TYPE filesystem ) 
{
	RMuint32 unReturn;

	if ( filesystem == FATFS ) {
		unReturn = fatfs_fclose( fd );
		}
	else {
		unReturn = (RMuint32)close( fd );
//		unReturn = (RMuint32)fclose((FILE*) fd );
		}

	DEBUGMSG(1,("*** 59th :: __CloseFile : close ,return=%d\n", unReturn));	// for debug

	return unReturn;
}


// share buffer for copy
extern RMuint32 *mp3_buffer;
int __CopyBlock( RMuint32 fdsrc, eCURRENT_DIR_FS_TYPE fs4Source, RMuint32 fddst, eCURRENT_DIR_FS_TYPE fs4Target, RMint64 *pPasteItemSize, RMuint64 *pu64TempFreeDiskSpace )
{
	int	nReturn;
	int 	progress;
	int 	nLength;
	FATFS_ERROR sdFatfsErr;
	
	nLength = 0;
	nReturn = 0;

	do {	
		if ( fs4Source == FATFS ) {
			sdFatfsErr = fatfs_fread ( fdsrc, (RMuint8*)mp3_buffer, szCopyBuffer, (RMuint32*)&nLength);
			printf("++ 41th :: fatfs_fread : readed size=%d, sdFatfsErr=0x%x\n", nLength, sdFatfsErr );	// for debug
			if ( sdFatfsErr != FATFS_ERROR_NO_ERROR && sdFatfsErr !=FATFS_ERROR_FILE_END_OF_FILE) {
				nReturn = -1;
				break;
				}
			}
		else {
			nLength = read( fdsrc, (RMuint8*)mp3_buffer, szCopyBuffer );
//			nLength = fread( (RMuint8*)mp3_buffer, 1, szCopyBuffer, (FILE*)fdsrc );
			DEBUGMSG(1,("++ 43th :: read : readed size=%d\n", nLength ));	// for debug
			if ( nLength < 0 ) {
				nReturn = -1;
				break;
				}			
			}
		
		if (nLength > 0) { 
			if ( fs4Target == FATFS ) {
				sdFatfsErr = fatfs_fwrite ( fddst, (RMuint8*)mp3_buffer, nLength, (RMuint32*)&nLength);
				DEBUGMSG(1,("++ 42th :: fatfs_fwrite : writed size=%d, sdFatfsErr=0x%x\n", nLength, sdFatfsErr ));	// for debug
				if ( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
					nReturn = -1;
					break;
					}
				}
			else {
				nLength = write(fddst, (RMuint8*)mp3_buffer, nLength );
//				nLength = fwrite((RMuint8*)mp3_buffer, 1,  nLength, (FILE*)fddst );
				DEBUGMSG(1,("++ 44th :: write : writed size=%d, fddst=%d\n", nLength, fddst));	// for debug
				if ( nLength < 0 ) {
					nReturn = -1;
					break;
					}
				}

			*pPasteItemSize+=nLength;
			*pu64TempFreeDiskSpace -=nLength;
			}

		progress = (int)((*pPasteItemSize*100)/pLFBrowser->Get_CurrentFI_Size());
		pMbx->BuildProgressBar( progress );

		DEBUGMSG(1,("++ 49th :: __CopyBlock : Progress[%d%%]\n", progress ));		// for debug

		//////////////////////////////////////////////////////////////////////////////
		// Check Key & battery					
		if ( _IsNeededToExitForBatterynKey(pFileManager->TaskId ) > 0 ) {
			printf("[!] Cancel By User\n");
			ClearKeyBuffer();
			nReturn = -1;
			break;
			}			
		} while (nLength > 0 );

	return nReturn;
}
int __OpenSourceFile( uchar_t *pu16PathSource, eCURRENT_DIR_FS_TYPE fs4Source, RMuint32 *pfdsrc, FATFS_ERROR *pErr )
{
	int nReturn;
	char szPathSource[260*2];

	nReturn = 0;
	
	if ( fs4Source == FATFS ) {
		*pErr = fatfs_wfopen( pu16PathSource, _O_RDONLY, pfdsrc );
		}
	else {
		pLFBrowser->ConvertUnicodeToUniXlate(szPathSource, pu16PathSource);
		if ( ( *pfdsrc = open( szPathSource, O_RDONLY|O_NONBLOCK)) < 0) {
//		if ( ( *pfdsrc = (RMuint32)fopen( szPathSource, "rb")) < 0) {

			// open error
			*pErr = (FATFS_ERROR) errno;
			DEBUGMSG(1,("=== 300th:: _OpenSourceFile error : szPathSource[%s]=%d\n", szPathSource, errno ));
			}
		}
	
	return nReturn;
}

int __OpenTargetFile( uchar_t *pu16PathTarget, eCURRENT_DIR_FS_TYPE fs4Target, RMuint32 *pfddst, FATFS_ERROR *pErr )
{
	int nReturn;
	char szPathTarget[260*2];

	nReturn = 0;	// successful open
	
	if ( fs4Target == FATFS ) {
		*pErr = fatfs_wfopen( (RMuint16*)pu16PathTarget, _O_WRONLY|_O_CREAT, pfddst );
		if ( *pErr != FATFS_ERROR_NO_ERROR) {
			if( *pErr == FATFS_ERROR_NAME_DOES_EXIST_AS_DIR ||
				*pErr == FATFS_ERROR_NAME_DOES_EXIST_AS_FILE ||
				*pErr == FATFS_ERROR_NAME_ALREADY_DOES_EXIST ) {

				if( FM_DrawMsgBox(FMM_COPY_FAILED_NAME_ALREADY_DOES_EXIST, 0) == 0) {
					// yes, overwrite
					*pErr = fatfs_wfopen( (RMuint16*)pu16PathTarget, _O_WRONLY|_O_CREAT|_O_TRUNC, pfddst ); 
					if ( *pErr != FATFS_ERROR_NO_ERROR) {
						// copy failure, target didn't opened
						nReturn = -1;
						return nReturn; 			
						}
					}
				else {
					// no, cancel copy
					nReturn = -2;
					return nReturn;
					}
				}
			else {
				FM_ShowFSE(*pErr);
				nReturn = -1;
				return nReturn;
				}
			}
		}
	else {
		////////////////////////////////////////////////////////////////////////////////
		// open target for LINUXFS
		pLFBrowser->ConvertUnicodeToUniXlate(szPathTarget, pu16PathTarget);
		*pfddst = open( szPathTarget, O_WRONLY|O_CREAT|O_NONBLOCK );
//		*pfddst = (RMuint32)fopen( szPathTarget, "wb" );
		if ( (int)*pfddst < 0) {
			// open error
			DEBUGMSG(1,("=== 202th:: _CopyFile : open error : szPathTarget[%s]=%d\n", szPathTarget, errno )); // for debug
			if ( errno == EEXIST ||
				errno == EISDIR 
				) {
				if( FM_DrawMsgBox(FMM_COPY_FAILED_NAME_ALREADY_DOES_EXIST, 0) == 0) {
					// yes, overwrite
					*pfddst = open( szPathTarget, O_WRONLY|O_CREAT|O_TRUNC|O_NONBLOCK  );
					*pfddst = (RMuint32)fopen( szPathTarget, "wb"  );
					if ( *pfddst < 0 )	{
						// copy failure, target didn't opened
						nReturn = -1;
						return nReturn; 			
						}
					}
				else {
					// no, cancel copy
					nReturn = -2;
					return nReturn;
					}
				}
			else {
				FM_ShowFSE( (FATFS_ERROR)errno );
				nReturn = -1;
				return nReturn;
				}
			nReturn = -1;
			return nReturn;
			}
		else {
			// successfully open		
			DEBUGMSG(1,("=== 203th:: _CopyFile : open : szPathTarget[%s], fddst=%d, err=%d\n", szPathTarget, *pfddst, errno )); // for debug
			}
		}
	return nReturn;
}

int _CopyFile( uchar_t *pu16PathSource, uchar_t *pu16PathTarget, FATFS_ERROR *pErr, RMint64 *pPasteItemSize, RMuint64 *pu64TempFreeDiskSpace )
{
	RMuint32	fdsrc;
	RMuint32	fddst;
	int	nReturn;
	eCURRENT_DIR_FS_TYPE fs4Source;
	eCURRENT_DIR_FS_TYPE fs4Target;

	nReturn = 0;
	
	fs4Source = ((pLFBrowser->IsThisLinuxFs(pu16PathSource))?LINUXFS:FATFS);
	fs4Target = ((pLFBrowser->IsThisLinuxFs(pu16PathTarget))?LINUXFS:FATFS);
	//////////////////////////////////////////////////////////////////////////////
	// open source file
	if ( __OpenSourceFile( pu16PathSource, fs4Source, &fdsrc, pErr )  < 0 ) {
		// open error
		}
	//////////////////////////////////////////////////////////////////////////////
	// open target file
	if ( __OpenTargetFile( pu16PathTarget, fs4Target, &fddst, pErr ) < 0 ) {
		// open error
		__CloseFile ( fdsrc, fs4Source );	
		nReturn = -1;
		return nReturn; 			
		}
	//////////////////////////////////////////////////////////////////////////////
	// excute copy file
	if ( __CopyBlock( fdsrc, fs4Source, fddst, fs4Target, pPasteItemSize, pu64TempFreeDiskSpace) < 0 ) {
		// cancel copy
		nReturn = -2;
		}
	else {
		// complete copy
		nReturn = 0;
		}
	//////////////////////////////////////////////////////////////////////////////
	// close files
	__CloseFile ( fdsrc, fs4Source );
	__CloseFile ( fddst, fs4Target );
	
	printf("+++ 209th :: _CopyFile : close files\n\n");				// for debug

	return nReturn;
}

RMuint32 _DeleteFile( uchar_t *u16FileName)
{
	RMuint32 unReturn;
	char szFileName[260*2];
	eCURRENT_DIR_FS_TYPE filesystem;

	filesystem = ((pLFBrowser->IsThisLinuxFs(u16FileName))?LINUXFS:FATFS);
	
	if (filesystem == FATFS ) {
		unReturn = fatfs_wdelete_file( (RMuint16*)u16FileName );
		}
	else {
		pLFBrowser->ConvertUnicodeToUniXlate(szFileName, u16FileName);
		if ( (unReturn=remove( szFileName ) )< 0 ) {
			// occures error
		 	unReturn = errno;
			}
		}
	
	DEBUGMSG(1,("+++ 20th :: _DeleteFile : filesystem=%d, ret=%d,  file*************", filesystem, unReturn));	// for debug
	//print_ustr(u16FileName, 0 );

	return unReturn;
}
RMuint32 _OpenDir( uchar_t *pu16FolderPath, RMuint32 *pFd ) 
{
	RMuint32 unReturn;
	char szFileName[260*2];
	DIR *pDir;
	eCURRENT_DIR_FS_TYPE filesystem;

	filesystem = ((pLFBrowser->IsThisLinuxFs(pu16FolderPath))?LINUXFS:FATFS);
	DEBUGMSG(1,("*** 50th :: _OpenDir : filesystem=%d*************", filesystem));	// for debug
	print_ustr(pu16FolderPath, 0 );
	if ( filesystem == FATFS ) {
		unReturn = fatfs_wdopen( (RMuint16*)pu16FolderPath, pFd );
		}
	else {
		pLFBrowser->ConvertUnicodeToUniXlate(szFileName, pu16FolderPath);
		if ( ( pDir=opendir( szFileName ) )== NULL ) {
			// open error
			DEBUGMSG(1,("*** 53th :: _OpenDir[%s], error =%d\n", szFileName, errno)); // for debug
			unReturn = errno;
			}
		else {
			*pFd=(RMuint32)pDir;
			unReturn = 0;
			DEBUGMSG(1,("*** 54th :: _OpenDir[%s], *pFd=%d\n", szFileName, *pFd )); // for debug
			}
		}
	DEBUGMSG(1,("*** 51th :: _OpenDir : *pFd=%d, ret=%d\n", *pFd, unReturn));	// for debug	
	return unReturn;
}

RMuint32 _ReadDir( RMuint32 fd, FATFS_DIR_ENTRY_INFO *pDirEntry, uchar_t *pu16FolderPath ) 
{
	RMuint32 unReturn;
	char parent[260*2];
	char fullpath[260*2];
	struct dirent* items;
	struct stat fstat;
	DIR *pDir;
	eCURRENT_DIR_FS_TYPE filesystem;

	filesystem = ((pLFBrowser->IsThisLinuxFs(pu16FolderPath))?LINUXFS:FATFS);

	DEBUGMSG(1,("-- 45th :: _ReadDir : filesystem=%d , ", filesystem ));	// for debug
	print_ustr(pu16FolderPath, 0);						// for debug

	if ( filesystem == FATFS ) {
		unReturn = fatfs_dread( fd, pDirEntry );
		}
	else {
		pDir = (DIR *)(fd);
		pLFBrowser->ConvertUnicodeToUniXlate(parent, pu16FolderPath); 

		if ( (items=readdir(pDir) )!= NULL ) {
			sprintf( fullpath, "%s/%s", parent,  items->d_name );
			if( lstat( fullpath, &fstat ) >= 0 ) {	
				if( S_ISREG(fstat.st_mode) ) {
					pLFBrowser->ConvertVFATTextToUnicode16Str( (uchar_t *)pDirEntry->name, (char*)items->d_name);
					pDirEntry->namelength = ustrlen((uchar_t *)pDirEntry->name);
					pDirEntry->attributes = ATTR_FILE;
					pDirEntry->file_size = (RMuint32)fstat.st_size;
					DEBUGMSG(1,("-- 44th :: _ReadDir : file size =%d\n", fstat.st_size));	// for debug
					}	
				else if( S_ISDIR(fstat.st_mode) ) {
					pLFBrowser->ConvertVFATTextToUnicode16Str( (uchar_t *)pDirEntry->name, (char*)items->d_name);
					pDirEntry->namelength = ustrlen((uchar_t *)pDirEntry->name);
					pDirEntry->attributes = ATTR_DIRECTORY;
					pDirEntry->file_size = (RMuint32)fstat.st_size;
					}
//				printf("-- 46th :: _ReadDir : lstat >= 0\n");		// for debug
				unReturn = 0;
				}
			else {
				// stat error
				DEBUGMSG(1,("-- 47th :: _ReadDir : lstat < 0, errno=%d\n", errno));	// for debug
				unReturn = errno;
				}
			}
		else {
			// readdir error
			DEBUGMSG(1,("--- 49th :: _ReadDir : return = NULL \n"));				// for debug
			unReturn = errno;
			}

		DEBUGMSG(1,("--- 48th :: _ReadDir[%s], fd=%d, items->d_name[%s], nReturn=%d\n", fullpath, fd, items->d_name, unReturn));	// for debug

		}

	return unReturn;
}

RMuint32 _CloseDir( RMuint32 fd, uchar_t *pu16FolderPath ) 
{
	RMuint32 unReturn;
	eCURRENT_DIR_FS_TYPE filesystem;

	filesystem = ((pLFBrowser->IsThisLinuxFs(pu16FolderPath))?LINUXFS:FATFS);
	
	if ( filesystem == FATFS ) {
		unReturn = fatfs_dclose( fd );
		}
	else {
		unReturn = closedir( (DIR*)fd);
		}

	DEBUGMSG(1,("*** 60th :: _CloseDir : fd= %d, filesystem=%d*********", fd, filesystem));	// for debug
	//print_ustr(pu16FolderPath, 0);						// for debug

	return TRUE;
}

RMuint32 _DeleteFolder(uchar_t *pu16FolderPath ) 
{
	RMuint32 unReturn;
	char szName[260*2];
	eCURRENT_DIR_FS_TYPE filesystem;

	filesystem = ((pLFBrowser->IsThisLinuxFs(pu16FolderPath))?LINUXFS:FATFS);
	
	if ( filesystem == FATFS ) {
		unReturn= fatfs_wdelete_dir_tree( (RMuint16*)pu16FolderPath , 0);
		}
	else {
		pLFBrowser->ConvertUnicodeToUniXlate(szName, pu16FolderPath);
		if ( (unReturn=rmdir( szName ) ) < 0 ) {
			unReturn = errno;
			}
		else {
			unReturn = 0;
			}
		}

	return TRUE;
}

int ___MovePasteFat2Fat( uchar_t* uDestFile, uchar_t* uSrcFile )
{
	FATFS_ERROR err;
	int	nReturn;
	uchar_t uDestDir[260];
	
	nReturn = 0;

	GetParentDir_In_UC16FullDirString( uDestDir, uDestFile);
	
	print_ustr(uSrcFile,"--- ___MovePasteFat2Fat : uSrcFile " );
	print_ustr(uDestFile,"--- ___MovePasteFat2Fat : uDestFile " );
	print_ustr(uDestDir,"--- ___MovePasteFat2Fat : uDestDir " );
	
	err = fatfs_wmove_file( (RMuint16*)uSrcFile, (RMuint16*)uDestDir );
	DEBUGMSG(1,("--- ___MovePasteFat2Fat :: fatfs_wmove_file=0x%x\n", err));
	if( err != FATFS_ERROR_NO_ERROR )	{
		switch(err) {
			case FATFS_ERROR_FILE_ALREADY_DOES_EXIST:
				if( (err=fatfs_wdelete_file((RMuint16*)uDestFile)) == FATFS_ERROR_NO_ERROR ) {
					if( (err=fatfs_wmove_file( (RMuint16*)uSrcFile, (RMuint16*)uDestDir)) == FATFS_ERROR_NO_ERROR ) {
						// success move
						}
					else {
						// fail move
						nReturn = -1;
						DEBUGMSG(1,("== MoveOneFile :: move err=0x%x\n", err));
						}
					}
				else {
					// fail delete on destination directory
					nReturn = -2;
					DEBUGMSG(1,("== MoveOneFile :: delete err=0x%x\n", err));
					}
				break;

			default:
				// unknown error
				nReturn = -3;
				break;
			}
		}
	return nReturn;
}

int ___MovePasteLinux2Linux( uchar_t *uDestFile, uchar_t *uSrcFile )
{
	uchar_t uDestDir[260];

	char DstName[2048];
	char SrcName[2048];
	int	nReturn;

	nReturn = 0;

	GetParentDir_In_UC16FullDirString( uDestDir, uDestFile);

	print_ustr(uSrcFile,"--- ___MovePasteLinux2Linux : uSrcFile " );
	print_ustr(uDestFile,"--- ___MovePasteLinux2Linux : uDestFile " );
	print_ustr(uDestDir,"--- ___MovePasteLinux2Linux : uDestDir " );


	pLFBrowser->ConvertUnicodeToUniXlate(SrcName, uSrcFile);
	pLFBrowser->ConvertUnicodeToUniXlate(DstName, uDestFile);

	if ( (nReturn = rename ( SrcName, DstName ) ) < 0 ) {
		// error 
		nReturn = errno;
		}
	
	DEBUGMSG(1,("--- ___MovePasteLinux2Linux :: rename : ret=%d, err=%d\n", nReturn, errno));

	return nReturn;

}

int ___MovePasteFat2Linux( uchar_t* uDestFile, uchar_t* uSrcFile, RMint64 *pPasteItemSize, RMuint64 *pu64TempFreeDiskSpace  )
{
	FATFS_ERROR err;
	int	nReturn;
	RMuint32 fdsrc;
	RMuint32 fddst;

	nReturn = 0;

	if ( (nReturn = _CopyFile( uSrcFile, uDestFile, &err, pPasteItemSize, pu64TempFreeDiskSpace ) ) != 0 ) {
		// failure for copy
//		PasteItemSize += i64FileSize;
		switch ( nReturn ) {
			case -1:
				// failure, target didn't opened
				break;
			case -2:
				// cancel copy
				break;
			default:
				break;
			}
		DEBUGMSG(1,("=== 175th :: ___MovePasteFat2Linux : error\n"));
		}
	else {
		// complete copy
		DEBUGMSG(1,("=== 175th :: ___MovePasteFat2Linux : complete Move file\n"));
		}
	
	return nReturn;
}

int __MovePasteOneFile( uchar_t* uDestFile, uchar_t* uSrcFile, RMint64 *pPasteItemSize, RMuint64 *pu64TempFreeDiskSpace  )
{
	FATFS_ERROR err;
	int	nReturn;
	eCURRENT_DIR_FS_TYPE fs4Source;
	eCURRENT_DIR_FS_TYPE fs4Target;

	nReturn = 0;
	
	fs4Source = ((pLFBrowser->IsThisLinuxFs(uSrcFile))?LINUXFS:FATFS);
	fs4Target = ((pLFBrowser->IsThisLinuxFs(uDestFile))?LINUXFS:FATFS);


	if ( fs4Source == FATFS && fs4Target == FATFS ) {
		nReturn = ___MovePasteFat2Fat( uDestFile, uSrcFile );
		}
	else if ( fs4Source == FATFS && fs4Target == LINUXFS ) {
		nReturn = ___MovePasteFat2Linux( uDestFile, uSrcFile, pPasteItemSize, pu64TempFreeDiskSpace );
		_DeleteFile(uSrcFile);
		}
	else if ( fs4Source == LINUXFS && fs4Target == FATFS ) {
		nReturn = ___MovePasteFat2Linux( uDestFile, uSrcFile, pPasteItemSize, pu64TempFreeDiskSpace );
		_DeleteFile(uSrcFile);		
		}
	else {
		nReturn = ___MovePasteLinux2Linux( uDestFile, uSrcFile );
		}
		
	return nReturn;
}

int _PasteOneFile4Move( 	eFILEMANAGER_COMMAND_STATUS SubmenuStatus, 
					uchar_t* FileName, 
					uchar_t* SrcDir, 
					uchar_t* DesDir, 
					RMuint64* pu64TempFreeDiskSpace,
					RMint64* pPasteItemSize )

{

	RMuint32 fdsrc;
	RMuint32 fddst;
	FATFS_ERROR fatfs_err;
	uchar_t usrc[260];
	uchar_t udst[260];
	int progress;
	int nReturn = 0;
	
	RMint64 i64FileSize;
	RMint64 i64FileSizeTmp;

	

	ustrcpy( usrc, SrcDir ); adduchar( usrc, 0x002f ); ustrcat( usrc, FileName );
	ustrcpy( udst, DesDir ); adduchar( udst, 0x002f ); ustrcat( udst, FileName );

	print_ustr(usrc, "=== 10th:: SrcDir ");
	print_ustr(udst, "=== 11th:: DesDir ");
	//////////////////////////////////////////////////////////////////////////////
	// Check Key & battery					
	if ( _IsNeededToExitForBatterynKey(pFileManager->TaskId ) > 0 ) {
		printf("[!] Cancel By User\n");
		ClearKeyBuffer();
		return -2;
		}	
	//////////////////////////////////////////////////////////////////////////////////////////
	// get size of source file for progress
	if ( _ExistFile( (RMuint16*)usrc, &i64FileSize, &fatfs_err, 0 ) < 0 ) {
		// error, file was not open
		printf("=== 12th :: _PasteOneFile4Copy :: fopen error =0x%x\n", fatfs_err );
		return -11;
		}
	//////////////////////////////////////////////////////////////////////////////////////////
	// check existance of target file 
	if ( _ExistFile( (RMuint16*)udst, &i64FileSizeTmp, &fatfs_err, 1) == 2 ) {
		// exist target file & skip to next
		return -13;
		}
	//////////////////////////////////////////////////////////////////////////////////////////
	// check length of file name for target
	if ( _IsLongPathName( (RMuint16 *) udst) > 0 ) {
		return -14;
		}
	//////////////////////////////////////////////////////////////////////////////////////////
	// redraw name & directory
	pMbx->BuildProgressContents( SubmenuStatus, FileName,  SrcDir, DesDir );
	progress = (int)((*pPasteItemSize*100)/pLFBrowser->Get_CurrentFI_Size());
	pMbx->BuildProgressBar( progress );
	//////////////////////////////////////////////////////////////////////////////
	// excute move
	nReturn = __MovePasteOneFile( udst, usrc, pPasteItemSize, pu64TempFreeDiskSpace );

	return nReturn;
}

int _PasteOneFile4Copy( 	eFILEMANAGER_COMMAND_STATUS SubmenuStatus, 
					uchar_t* FileName, 
					uchar_t* SrcDir, 
					uchar_t* DesDir, 
					RMuint64* pu64TempFreeDiskSpace,
					RMint64* pPasteItemSize )
{

	RMuint32 fdsrc;
	RMuint32 fddst;
	FATFS_ERROR fatfs_err;
	uchar_t usrc[260];
	uchar_t udst[260];
	int progress;
	int nReturn = 0;
	
	RMint64 i64FileSize;
	RMint64 i64FileSizeTmp;

	

	ustrcpy( usrc, SrcDir ); adduchar( usrc, 0x002f ); ustrcat( usrc, FileName );
	ustrcpy( udst, DesDir ); adduchar( udst, 0x002f ); ustrcat( udst, FileName );

	print_ustr(usrc, "=== 10th:: SrcDir ");
	print_ustr(udst, "=== 11th:: DesDir ");
	//////////////////////////////////////////////////////////////////////////////////////////
	// get size of source file for progress
	if ( _ExistFile( (RMuint16*)usrc, &i64FileSize, &fatfs_err, 0 ) < 0 ) {
		// error, file was not open
		DEBUGMSG(1,("=== 12th :: _PasteOneFile4Copy :: fopen error =0x%x\n", fatfs_err ));
		return -11;
		}
	//////////////////////////////////////////////////////////////////////////////////////////
	// check free space for copy
	if ( _IsLessFreeSpace( i64FileSize, *pu64TempFreeDiskSpace) > 0) {
		return -12;
		}
	//////////////////////////////////////////////////////////////////////////////////////////
	// check existance of target file 
	if ( _ExistFile( (RMuint16*)udst, &i64FileSizeTmp, &fatfs_err, 1) == 2 ) {
		// exist target file & skip to next
		*pPasteItemSize += i64FileSize;
		return -13;
		}
	//////////////////////////////////////////////////////////////////////////////////////////
	// check length of file name for target
	if ( _IsLongPathName( (RMuint16 *) udst) > 0 ) {
		*pPasteItemSize += i64FileSize;
		return -14;
		}
	//////////////////////////////////////////////////////////////////////////////////////////
	// redraw name & directory
	pMbx->BuildProgressContents( SubmenuStatus, FileName,  SrcDir, DesDir );
	progress = (int)((*pPasteItemSize*100)/pLFBrowser->Get_CurrentFI_Size());
	pMbx->BuildProgressBar( progress );
	//////////////////////////////////////////////////////////////////////////////////////////
	// copy
	if ( (nReturn = _CopyFile( (uchar_t*)usrc, (uchar_t*)udst, &fatfs_err, pPasteItemSize, pu64TempFreeDiskSpace ) ) != 0 ) {
		// failure for copy
		*pPasteItemSize += i64FileSize;
		}
	else {
		// complete copy
		}

	return nReturn;
}

U32 __DrawPasteDbxContents()
{
	U32 TitleRes;
	
	if( pFileManager->SubmenuStatus == _CMD_COPY_STATE ) 		TitleRes = ML_COPY;
	else if( pFileManager->SubmenuStatus == _CMD_MOVE_STATE ) 	TitleRes = ML_MOVE;
	else { ASSERT(0); return FALSE; }

	pMbx->Reset( TitleRes, _GREEN, (uchar_t*)NULL, -1, -1, NULL, (DL_PROGRESS_MODE|DL_BTN_CANCEL_ONLY) );
	pMbx->ReDraw(SKIP_OSD_REFRESH);

	return TitleRes;
}

int _PasteRecursive4Move(eFILEMANAGER_COMMAND_STATUS SubmenuStatus, 
					RMuint64* pu64TempFreeDiskSpace,
					RMint64* pPasteItemSize )

{
	uchar_t usrc[520];
	uchar_t udst[520];
	char szCurPathSou[2048];
	char szCurPathTar[2048];
	CStack< CString > dirStackSou( 1024 );
	CStack< CString > dirStackTar( 1024 );
	CString szDirPathSou;
	CString szDirPathTar;
	uchar_t* wszPath;
	FATFS_ERROR sdFatfsErr;
	
	FATFS_DIR_ENTRY_INFO sdDirEntrySou;
	FATFS_DIR_ENTRY_INFO sdDirEntryTar;
	RMuint32 hDirSou;

	CString szPathSou;
	CString szPathTar;
	CString szTmp;

	int progress;
	int ret;
	int i;
	int nReturn4MsgBox;
	
	for( i=0; i<pLFBrowser->SelectedItemList.Count(); i++ )	{
		progress = (int)((*pPasteItemSize*100)/pLFBrowser->Get_CurrentFI_Size());
		DEBUGMSG(1,("-- 200th :: _PasteRecursive4Copy i=%d/%d : progress=%d ( completed =%lld,	file =%lld )\n", i, pLFBrowser->SelectedItemList.Count(), progress, *pPasteItemSize, pLFBrowser->Get_CurrentFI_Size() ));	


		if( (pLFBrowser->SelectedItemList[i]->attributes&0x10) ) {		
			////////////////////////////////////////////////////////////////////////////////////////////
			// FOLDER
			ustrcpy( usrc, (uchar_t*)pLFBrowser->SelectedItemList[i]->parentdir );
			adduchar( usrc, (uchar_t)'/' );
			ustrcat( usrc, (uchar_t*)pLFBrowser->SelectedItemList[i]->name );
			print_ustr(usrc, "-- 201th :: SourceDir ");

			ustrcpy( udst, pLFBrowser->CurrentFullPathDir );
			adduchar( udst, (uchar_t)'/' ); 
			ustrcat( udst, (uchar_t*)pLFBrowser->SelectedItemList[i]->name );
			print_ustr(udst, "-- 202th :: TargetDir ");
			
			szPathSou.SetString( (char*)usrc, true );
			szPathTar.SetString( (char*)udst, true );	
			dirStackSou.Push( szPathSou );					DEBUG_U16MSG( (uchar_t*)usrc, "-dirStackSou.Push(S-A):" );
			dirStackTar.Push( szPathTar );					DEBUG_U16MSG( (uchar_t*)udst, "-dirStackTar.Push(T-A):" );


			//////////////////////////////////////////////////////////////////////////////////////////
			// recursive loop
			while( dirStackSou.IsEmpty() == false ) {
				szDirPathSou = dirStackSou.Pop();			DEBUG_U16MSG( (uchar_t*)szDirPathSou, "-dirStackSou.Pop(S-1):" );
				szDirPathTar = dirStackTar.Pop();			DEBUG_U16MSG( (uchar_t*)szDirPathTar, "-dirStackTar.Pop(T-1):" );
														//-dirStackTar.Pop(T-1)::[/ h d 1 / H H H H H H H H V . Z / T e s t F o l d e r ]

				//////////////////////////////////////////////////////////////////////////////
				// create target folder
				if ( (ret =_CreateTargetFolder( (RMuint16 *)szDirPathSou, (RMuint16 *)szDirPathTar ) ) < 0 ) {
					// create error
					if ( ret == -2 ) {
						// exist file with same name of folder
						nReturn4MsgBox = FM_DrawMsgBox(FMM_COPY_FAILED_NAME_ALREADY_DOES_EXIST);
						__DrawPasteDbxContents();
						pMbx->BuildProgressContents( 
							SubmenuStatus,
							(uchar_t*)sdDirEntrySou.name,
							(uchar_t*)szDirPathSou,
							(uchar_t*)szDirPathTar );
						pMpegDecoder->RefreshOsd();
						if( nReturn4MsgBox == 1 ) { 	
							// yes overwrite
							}
						else {						
							// no cancel
							goto _PasteRecur4MoveRet_FALSE;
							}
						}
					else {
						goto _PasteRecur4MoveRet_FALSE;
						}
					}
				
				//////////////////////////////////////////////////////////////////////////////
				ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
				ustrcpy( (uchar_t*)szCurPathTar, (uchar_t*)szDirPathTar );
				wszPath = ( uchar_t* )szDirPathSou;

				//////////////////////////////////////////////////////////////////////////////////////////
				// Directory Open
				sdFatfsErr = (FATFS_ERROR)_OpenDir( wszPath, &hDirSou );

				//////////////////////////////////////////////////////////////////////////////
				// folder loop
				while( sdFatfsErr == FATFS_ERROR_NO_ERROR ) {					
					sdFatfsErr = (FATFS_ERROR)_ReadDir( hDirSou, &sdDirEntrySou, wszPath );

					// prepass directory
					if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) break;
					if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 && 
					     sdDirEntrySou.name[2] == 0 && sdDirEntrySou.name[3] == 0 ) continue;
					if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 &&
						sdDirEntrySou.name[2] == '.' && sdDirEntrySou.name[3] == 0 ) continue;
					// make soure path
					ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
					adduchar( (uchar_t*)szCurPathSou, (uchar_t)'/' );
					ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)sdDirEntrySou.name );
					// make target path
					ustrcpy( (uchar_t*)szCurPathTar, (uchar_t*)szDirPathTar );
					adduchar( (uchar_t*)szCurPathTar, (uchar_t)'/' );
					ustrcat( (uchar_t*)szCurPathTar, (uchar_t*)sdDirEntrySou.name );

					if( sdDirEntrySou.attributes&ATTR_DIRECTORY ) {
						/////////////////////////////////////////////////////////////
						// push to stack for directory
						szPathSou.SetString( szCurPathSou, true );		
						dirStackSou.Push( szPathSou );
						
						szPathTar.SetString( szCurPathTar, true );		
						dirStackTar.Push( szPathTar );				
						}
					else {
						/////////////////////////////////////////////////////////////
						// Move file
						ret = _PasteOneFile4Move( SubmenuStatus,
							(uchar_t*)sdDirEntrySou.name,
							(uchar_t*)szDirPathSou, 
							(uchar_t*)szDirPathTar,
							pu64TempFreeDiskSpace,
							pPasteItemSize
							);
						
						/////////////////////////////////////////////////////////////////////////////////////////
						// Draw progress
						{
						eCURRENT_DIR_FS_TYPE fs4Source;
						eCURRENT_DIR_FS_TYPE fs4Target;
						fs4Source = ((pLFBrowser->IsThisLinuxFs((uchar_t*)szDirPathSou))?LINUXFS:FATFS);
						fs4Target = ((pLFBrowser->IsThisLinuxFs((uchar_t*)szDirPathTar))?LINUXFS:FATFS);

						if ( fs4Source == fs4Target ) {
							*pPasteItemSize+=sdDirEntrySou.file_size;
							}
						progress = (int)((*pPasteItemSize*100)/pLFBrowser->Get_CurrentFI_Size());
						pMbx->BuildProgressBar( progress );						
						}
						
						DEBUGMSG(1,("=== 212th :: _PasteRecursive4Move : folder, file size=%d, progress[%d%%], ret=%d\n", sdDirEntrySou.file_size, progress, ret )); 	// for debug

						switch ( ret ) {
							case -1:
								// target open error & skip
								break;
							case -2:
								// cancel copy & stop
								goto _PasteRecur4MoveRet_CANCEL;
								break;
							case -11:
								// source does not open & skip
								break;
							case -12:
								// less free space & stop
								goto _PasteRecur4MoveRet_CANCEL;
								break;
							case -13:
								// exist target file & skip to next
								break;
							case -14:
								// long path name & skip to next
								break;
							}
						// complete copy
						DEBUGMSG(1,("=== 220th :: _PasteRecursive4Move : folder : complete copy file\n"));	
						}
					}
				//////////////////////////////////////////////////////////////////////////////////////////
				// Directory Close
				_CloseDir( hDirSou, wszPath );
				}

				DEBUGMSG(1,("-- 221th :: _PasteRecursive4Move : foler, i=%d, PasteItemSize =%ld\n", i, *pPasteItemSize));

				DEBUGINFO;
				_DeleteFolder( usrc );
				
			}
			else {
				/////////////////////////////////////////////////////////////////////////////////////////
				// FILE
				ret = _PasteOneFile4Move( SubmenuStatus, 
					(uchar_t*)pLFBrowser->SelectedItemList[i]->name, 
					(uchar_t*)pLFBrowser->SelectedItemList[i]->parentdir, 
					pLFBrowser->CurrentFullPathDir,
					pu64TempFreeDiskSpace,
					pPasteItemSize
					);
				
				/////////////////////////////////////////////////////////////////////////////////////////
				// Draw progress
				*pPasteItemSize+=sdDirEntrySou.file_size;
				progress = (int)((*pPasteItemSize*100)/pLFBrowser->Get_CurrentFI_Size());
				pMbx->BuildProgressBar( progress );
				
				
				DEBUGMSG(1,("-- 259th :: _PasteRecursive4Move : file, progress[%d%%], ret=%d, i=%d, PasteItemSize =%ld\n", progress, ret, i, *pPasteItemSize));
				switch ( ret ) {
					case -1:
						// target open error & skip
						continue;
						break;
					case -2:
						// cancel copy & stop
						goto _PasteRecur4MoveRet_CANCEL;
						break;
					case -11:
						// source does not open & skip
						continue;
						break;
					case -12:
						// less free space & stop
						goto _PasteRecur4MoveRet_CANCEL;
						break;
					case -13:
						// exist target file & skip to next
						continue;
						break;
					case -14:
						// long path name & skip to next
						continue;
						break;
					}
					
				}
		}

	
	goto _PasteRecur4MoveRet_TRUE;
	
_PasteRecur4MoveRet_CANCEL:
	DEBUGINFO;
	_CloseDir( hDirSou, wszPath );
	return -1;

_PasteRecur4MoveRet_FALSE:
	DEBUGINFO;
	return -2;

_PasteRecur4MoveRet_TRUE:
	DEBUGINFO;
	return 0;
}

int _PasteRecursive4Copy(eFILEMANAGER_COMMAND_STATUS SubmenuStatus, 
					RMuint64* pu64TempFreeDiskSpace,
					RMint64* pPasteItemSize )

{
	uchar_t usrc[520];
	uchar_t udst[520];
	char szCurPathSou[2048];
	char szCurPathTar[2048];
	CStack< CString > dirStackSou( 1024 );
	CStack< CString > dirStackTar( 1024 );
	CString szDirPathSou;
	CString szDirPathTar;
	uchar_t* wszPath;
	FATFS_ERROR sdFatfsErr;
	
	FATFS_DIR_ENTRY_INFO sdDirEntrySou;
	FATFS_DIR_ENTRY_INFO sdDirEntryTar;
	RMuint32 hDirSou;

	CString szPathSou;
	CString szPathTar;
	CString szTmp;

	int progress;
	int ret;
	int i;
	int nReturn4MsgBox;
	
	for( i=0; i<pLFBrowser->SelectedItemList.Count(); i++ )	{
		progress = (int)((*pPasteItemSize*100)/pLFBrowser->Get_CurrentFI_Size());
		DEBUGMSG(1,("-- 200th :: _PasteRecursive4Copy i=%d/%d : progress=%d ( completed =%lld,	file =%lld )\n", i, pLFBrowser->SelectedItemList.Count(), progress, *pPasteItemSize, pLFBrowser->Get_CurrentFI_Size() ));	


		if( (pLFBrowser->SelectedItemList[i]->attributes&0x10) ) {		
			////////////////////////////////////////////////////////////////////////////////////////////
			// FOLDER
			ustrcpy( usrc, (uchar_t*)pLFBrowser->SelectedItemList[i]->parentdir );
			adduchar( usrc, (uchar_t)'/' );
			ustrcat( usrc, (uchar_t*)pLFBrowser->SelectedItemList[i]->name );
			print_ustr(usrc, "-- 201th :: SourceDir ");

			ustrcpy( udst, pLFBrowser->CurrentFullPathDir );
			adduchar( udst, (uchar_t)'/' ); 
			ustrcat( udst, (uchar_t*)pLFBrowser->SelectedItemList[i]->name );
			print_ustr(udst, "-- 202th :: TargetDir ");
			
			szPathSou.SetString( (char*)usrc, true );
			szPathTar.SetString( (char*)udst, true );	
			dirStackSou.Push( szPathSou );					DEBUG_U16MSG( (uchar_t*)usrc, "-dirStackSou.Push(S-A):" );
			dirStackTar.Push( szPathTar );					DEBUG_U16MSG( (uchar_t*)udst, "-dirStackTar.Push(T-A):" );


			//////////////////////////////////////////////////////////////////////////////////////////
			// recursive loop
			while( dirStackSou.IsEmpty() == false ) {
				szDirPathSou = dirStackSou.Pop();			DEBUG_U16MSG( (uchar_t*)szDirPathSou, "-dirStackSou.Pop(S-1):" );
				szDirPathTar = dirStackTar.Pop();			DEBUG_U16MSG( (uchar_t*)szDirPathTar, "-dirStackTar.Pop(T-1):" );
														//-dirStackTar.Pop(T-1)::[/ h d 1 / H H H H H H H H V . Z / T e s t F o l d e r ]

				//////////////////////////////////////////////////////////////////////////////
				// create target folder
				if ( (ret =_CreateTargetFolder( (RMuint16 *)szDirPathSou, (RMuint16 *)szDirPathTar ) ) < 0 ) {
					// create error
					if ( ret == -2 ) {
						// exist file with same name of folder
						nReturn4MsgBox = FM_DrawMsgBox(FMM_COPY_FAILED_NAME_ALREADY_DOES_EXIST);
						__DrawPasteDbxContents();
						pMbx->BuildProgressContents( 
							SubmenuStatus,
							(uchar_t*)sdDirEntrySou.name,
							(uchar_t*)szDirPathSou,
							(uchar_t*)szDirPathTar );
						pMpegDecoder->RefreshOsd();
						
						if( nReturn4MsgBox == 1 ) { 	
							// yes overwrite
							}
						else {						
							// no cancel
							goto _PasteRecur4CopyRet_FALSE;
							}
						}
					else {
						goto _PasteRecur4CopyRet_FALSE;
						}
					}
				
				//////////////////////////////////////////////////////////////////////////////
				ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
				ustrcpy( (uchar_t*)szCurPathTar, (uchar_t*)szDirPathTar );
				wszPath = ( uchar_t* )szDirPathSou;

				//////////////////////////////////////////////////////////////////////////////////////////
				// Directory Open
				sdFatfsErr = (FATFS_ERROR)_OpenDir( wszPath, &hDirSou );

				//////////////////////////////////////////////////////////////////////////////
				// folder loop
				while( sdFatfsErr == FATFS_ERROR_NO_ERROR ) {
					sdFatfsErr = (FATFS_ERROR)_ReadDir( hDirSou, &sdDirEntrySou, wszPath );

					// prepass directory
					if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) break;
					if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 && 
					     sdDirEntrySou.name[2] == 0 && sdDirEntrySou.name[3] == 0 ) continue;
					if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 &&
						sdDirEntrySou.name[2] == '.' && sdDirEntrySou.name[3] == 0 ) continue;
					// make soure path
					ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
					adduchar( (uchar_t*)szCurPathSou, (uchar_t)'/' );
					ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)sdDirEntrySou.name );
					// make target path
					ustrcpy( (uchar_t*)szCurPathTar, (uchar_t*)szDirPathTar );
					adduchar( (uchar_t*)szCurPathTar, (uchar_t)'/' );
					ustrcat( (uchar_t*)szCurPathTar, (uchar_t*)sdDirEntrySou.name );

					if( sdDirEntrySou.attributes&ATTR_DIRECTORY ) {
						/////////////////////////////////////////////////////////////
						// push to stack for directory
						szPathSou.SetString( szCurPathSou, true );		
						dirStackSou.Push( szPathSou );
						
						szPathTar.SetString( szCurPathTar, true );		
						dirStackTar.Push( szPathTar );				
						}
					else {
						/////////////////////////////////////////////////////////////
						// copy file
						ret = _PasteOneFile4Copy( SubmenuStatus,
							(uchar_t*)sdDirEntrySou.name,
							(uchar_t*)szDirPathSou, 
							(uchar_t*)szDirPathTar,
							pu64TempFreeDiskSpace,
							pPasteItemSize
							);
						DEBUGMSG(1,("=== 210th :: _PasteRecursive4Copy : folder, ret=%d, \n", ret));	

						switch ( ret ) {
							case -1:
								// target open error & skip
								break;
							case -2:
								// cancel copy & stop
								goto _PasteRecur4CopyRet_CANCEL;
								break;
							case -11:
								// source does not open & skip
								break;
							case -12:
								// less free space & stop
								goto _PasteRecur4CopyRet_CANCEL;
								break;
							case -13:
								// exist target file & skip to next
								break;
							case -14:
								// long path name & skip to next
								break;
							}
						// complete copy
						DEBUGMSG(1,("=== 220th :: _PasteRecursive4Copy : folder : complete copy file\n"));	
						}
					}
				//////////////////////////////////////////////////////////////////////////////////////////
				// Directory Close
				_CloseDir( hDirSou, wszPath );
				}

				DEBUGMSG(1,("-- 221th :: _PasteRecursive4Copy : foler, i=%d, PasteItemSize =%ld\n", i, *pPasteItemSize));
			}
			else {
				/////////////////////////////////////////////////////////////////////////////////////////
				// FILE
				ret = _PasteOneFile4Copy( SubmenuStatus, 
					(uchar_t*)pLFBrowser->SelectedItemList[i]->name, 
					(uchar_t*)pLFBrowser->SelectedItemList[i]->parentdir, 
					pLFBrowser->CurrentFullPathDir,
					pu64TempFreeDiskSpace,
					pPasteItemSize
					);
				DEBUGMSG(1,("-- 259th :: _PasteRecursive4Copy : file ret=%d, i=%d, PasteItemSize =%ld\n", ret, i, *pPasteItemSize));
				switch ( ret ) {
					case -1:
						// target open error & skip
						continue;
						break;
					case -2:
						// cancel copy & stop
						goto _PasteRecur4CopyRet_CANCEL;
						break;
					case -11:
						// source does not open & skip
						continue;
						break;
					case -12:
						// less free space & stop
						goto _PasteRecur4CopyRet_CANCEL;
						break;
					case -13:
						// exist target file & skip to next
						continue;
						break;
					case -14:
						// long path name & skip to next
						continue;
						break;
					}
					
				}
		}
	
	goto _PasteRecur4CopyRet_TRUE;
	
_PasteRecur4CopyRet_CANCEL:
	DEBUGINFO;
	_CloseDir( hDirSou, wszPath );
	return -1;

_PasteRecur4CopyRet_FALSE:
	DEBUGINFO;
	return -2;

_PasteRecur4CopyRet_TRUE:
	DEBUGINFO;
	return 0;
}

int FMMC_Paste4Copy( RMuint8* parameters )
{
	RMuint64 u64TempFreeDiskSpace = FM_RefreshDiskSpace();
	U32 TitleRes;
	RMint64 Completed=0;
	int nReturn;
	int i=0;
	
	TitleRes = __DrawPasteDbxContents();
	pMbx->BuildProgressBar( 0 );

	DEBUGMSG(1,("~~ 20th ::\n"));
		
	nReturn = _PasteRecursive4Copy( pFileManager->SubmenuStatus, 
			&u64TempFreeDiskSpace, 
			&Completed 
			);
	
	if ( nReturn  == 0 ) {
		pMbx->BuildProgressBar( 100 );
		}

	printf("----------------------------------------------------\n");
	
_PASTE_ENCLOSE:
	for(i=0;i<20;i++)	sleep(1);

	pMbx->Hide();
	FMMC_ReleaseAll(0);
	pLFBrowser->Refresh(SKIP_OSD_REFRESH);
	FM_RefreshDiskSpace();	
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	printf("----------------------------------------------------\n");	
	return TRUE;
}

int FMMC_Paste4Move( RMuint8* parameters )
{
	RMuint64 u64TempFreeDiskSpace = FM_RefreshDiskSpace();
	U32 TitleRes;
	RMint64 Completed=0;
	int nReturn;
	int i=0;
	
	TitleRes = __DrawPasteDbxContents();
	pMbx->BuildProgressBar( 0 );

	DEBUGMSG(1,("~~ 22th ::\n"));
		
	nReturn = _PasteRecursive4Move( pFileManager->SubmenuStatus, 
			&u64TempFreeDiskSpace, 
			&Completed 
			);
	
	if ( nReturn  == 0 ) {
		pMbx->BuildProgressBar( 100 );
		}

	printf("----------------------------------------------------\n");
	
_PASTE_ENCLOSE:
	for(i=0;i<20;i++)	sleep(1);

	pMbx->Hide();
	FMMC_ReleaseAll(0);
	pLFBrowser->Refresh(SKIP_OSD_REFRESH);
	FM_RefreshDiskSpace();	
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	printf("----------------------------------------------------\n");	
	return TRUE;
}

int FMMC_CreateNewFolder( RMuint8* parameters )
{

	FATFS_DIR_ENTRY_INFO *pD;
	FATFS_ERROR err;
	RMuint32 Dir;
	RMuint8 bTypeDirOrFile;
	int FullpathNameLength;

	FullpathNameLength = ustrlen(pLFBrowser->CurrentFullPathDir);
	DEBUGMSG(1,("- ustrlen(pLFBrowser->CurrentFullPathDir):%d\n", ustrlen(pLFBrowser->CurrentFullPathDir)));

	pD = &(pLFBrowser->ItemList[pLFBrowser->GetCurrentChoice()]->D);
	
	DEBUGMSG(1,("\n+FMMC_CreateNewFolder()\n"));
	pVirtualKbd = new CVirtualKeyboard( ML_NEW_FOLDER, (uchar_t*)0, DL_BTN_OK_CANCEL);
	pVirtualKbd->SetUsedStringBuffer(FullpathNameLength);
	pVirtualKbd->Show();

	FM_DbxMsgPeek();

	if( pVirtualKbd->IsConfirmed() )
		{
		DEBUGMSG(1,("-ustrlen( pVirtualKbd->GetString() ) > 0 : %d\n", ustrlen(pVirtualKbd->GetString()) ));
		DEBUG_U16MSG( (uchar_t*)pLFBrowser->CurrentFullPathDir, "-CurrentFullPathDir");
		if ( pLFBrowser->IsThisLinuxFs(pLFBrowser->CurrentFullPathDir) ) {
			// linuxfs
			char DstName[1024];
			uchar_t uDstName[260];

			ustrcpy( uDstName, pLFBrowser->CurrentFullPathDir );
			adduchar( uDstName, (uchar_t)'/');
			ustrcat( uDstName, (uchar_t*)pVirtualKbd->GetString());
			
			pLFBrowser->ConvertUnicodeToUniXlate(DstName, uDstName);
			
			if ( (mkdir( DstName, 0777 ) ) < 0 ) {
				// error 
				printf("+++ FMMC_CreateNewFolder :: linuxfs DstName[%s], errno=%d \n",DstName, errno);
				}
			}
		else {
			// fatfs
			err = fatfs_wdopen( (RMuint16*)pLFBrowser->CurrentFullPathDir, &Dir);
			if(err>0) { ASSERT(0); printf("\t fatfs_wdopen Failed : %x\n", err); }

			DEBUG_U16MSG( (uchar_t*)pVirtualKbd->GetString() , "-pVirtualKbd->GetString()");
			DEBUGMSG(1,("-Dir : %d\n", Dir ));

			err = fatfs_wdcreate_dir( Dir, (RMuint16*)pVirtualKbd->GetString() );
			if(err>0) { ASSERT(0); printf("\t fatfs_wdcreate_dir Failed : %x\n", err); }

			fatfs_dclose(Dir);
			}
		}
	else
		{
		DEBUGMSG(1,("-Cancel\n"));
		}

	pVirtualKbd->Hide();
	delete pVirtualKbd;
	pVirtualKbd = NULL;
	
	DEBUGMSG(1,("-FMMC_CreateNewFolder()\n"));

	FMMC_ReleaseAll(0);
	pLFBrowser->Refresh(SKIP_OSD_REFRESH);
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	
	return TRUE;
}

int FMMC_SelectAll( RMuint8* parameters )
{
	DEBUGMSG(2,("\nFMMC_SelectAll()\n"));
	pLFBrowser->Add_IL_CheckTag( ENCLOSE_ALL );
	pFileManager->SubmenuStatus = _CMD_READY_STATE;
	pmCurrentMenu->Hide(0);
	pmCurrentMenu = NULL;
	pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}

int FMMC_ReleaseAll( RMuint8* parameters )
{
	RELOAD_WATCHDOGTIMER;
	pLFBrowser->Clear_IL_SIL_All();
	pFileManager->SubmenuStatus = _CMD_READY_STATE;
	if( pmCurrentMenu ) {
		pmCurrentMenu->Hide(0);
		pmCurrentMenu = NULL;
	}
	pLFBrowser->ReDraw(SKIP_OSD_REFRESH);
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}

// FMSMC; File Manager Sub Menu Command
int FMSMC_FileBrowserNameAscentSort( RMuint8* parameter )
{
	DEBUGMSG(2,("FMSMC_FileBrowserNameAscentSort( option=%x )\n", parameter ));
	pLFBrowser->RefreshForSortingMethodChange( fpNameAscentSort, SKIP_OSD_REFRESH);
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}
int FMSMC_FileBrowserNameDescentSort( RMuint8* parameter )
{
	DEBUGMSG(2,("FMSMC_FileBrowserNameDescentSort( option=%x )\n", parameter ));
	pLFBrowser->RefreshForSortingMethodChange( fpNameDescentSort, SKIP_OSD_REFRESH );
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}
int FMSMC_FileBrowserTimeAscentSort( RMuint8* parameter )
{
	DEBUGMSG(2,("FMSMC_FileBrowserTimeAscentSort( option=%x )\n", parameter ));
	pLFBrowser->RefreshForSortingMethodChange( fpTimeAscentSort, SKIP_OSD_REFRESH );
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}
int FMSMC_FileBrowserTimeDescentSort( RMuint8* parameter )
{
	DEBUGMSG(2,("FMSMC_FileBrowserTimeDescentSort( option=%x )\n", parameter ));
	pLFBrowser->RefreshForSortingMethodChange( fpTimeDescentSort, SKIP_OSD_REFRESH );
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}

int FMMC_Cancel( RMuint8* parameters )
{
	DEBUGMSG(2,("\nFMMC_Cancel()\n"));
	DEBUGMSG(2,("-pLFBrowser->SelectedItemList.Count(): %d ->", pLFBrowser->SelectedItemList.Count() ));
	FMMC_ReleaseAll(0);
	DEBUGMSG(2,(" %d\n", pLFBrowser->SelectedItemList.Count() ));
	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}

int FM_CreateMenuItems()
{
	/////////////////////////////////////////////////////////////////
	// SubMenu psSort  
	psSort = new CPopUpSubMenu;
	psSort->AddSubMenuItem( ML_NAME_ASCENT, TRUE, FMSMC_FileBrowserNameAscentSort, NULL );
	psSort->AddSubMenuItem( ML_NAME_DESCENT, TRUE, FMSMC_FileBrowserNameDescentSort, NULL );
	psSort->AddSubMenuItem( ML_TIME_ASCENT, TRUE, FMSMC_FileBrowserTimeAscentSort, NULL );
	psSort->AddSubMenuItem( ML_TIME_DESCENT, TRUE, FMSMC_FileBrowserTimeDescentSort, NULL );
//	psSort->AddSubMenuItem( ML_BACK, NULL, NULL);

	/////////////////////////////////////////////////////////////////
	// FileManager - No File Found
	pmFM_NoFile = new CPopUpMenu( 0, 0, 320, 220 );
	pmFM_NoFile->AddSubMenu( ML_NEW_FOLDER, NULL, -1, FMMC_CreateNewFolder, NULL );
//	pmFM_NoFile->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );

	/////////////////////////////////////////////////////////////////
	// FileManager - Single File Selected
	pmFM_SingleFile = new CPopUpMenu( 0, 0, 320, 220 );
//	pmFM_SingleFile->AddSubMenu( ML_PLAY, NULL, -1, FMMC_Play, NULL );
    #if !MP_REMOVE_COPY_FUNC
	pmFM_SingleFile->AddSubMenu( ML_COPY, NULL, -1, FMMC_Copy, NULL );
    #endif
	pmFM_SingleFile->AddSubMenu( ML_MOVE, NULL, -1, FMMC_Move, NULL );
	pmFM_SingleFile->AddSubMenu( ML_DELETE, NULL, -1, FMMC_Delete, NULL );
	pmFM_SingleFile->AddSubMenu( ML_RENAME, NULL, -1, FMMC_Rename, NULL );	 
	pmFM_SingleFile->AddSubMenu( ML_NEW_FOLDER, NULL, -1, FMMC_CreateNewFolder, NULL );
	pmFM_SingleFile->AddSubMenu( ML_SELECT_ALL, NULL, -1, FMMC_SelectAll, NULL );
	pmFM_SingleFile->AddSubMenu( ML_SORTING_METHOD, psSort, TaviSysParam.sort, NULL, NULL );
//	pmFM_SingleFile->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );


	/////////////////////////////////////////////////////////////////
	// FileManager - Single Folder Selected
	pmFM_SingleFolder = new CPopUpMenu( 0, 0, 320, 220 );
    #if !MP_REMOVE_COPY_FUNC
	pmFM_SingleFolder->AddSubMenu( ML_COPY, NULL, -1, FMMC_Copy, NULL );
    #endif
	pmFM_SingleFolder->AddSubMenu( ML_MOVE, NULL, -1, FMMC_Move, NULL );
	pmFM_SingleFolder->AddSubMenu( ML_DELETE, NULL, -1, FMMC_Delete, NULL );
	pmFM_SingleFolder->AddSubMenu( ML_RENAME, NULL, -1, FMMC_Rename, NULL );	 
	pmFM_SingleFolder->AddSubMenu( ML_NEW_FOLDER, NULL, -1, FMMC_CreateNewFolder, NULL );
	pmFM_SingleFolder->AddSubMenu( ML_SELECT_ALL, NULL, -1, FMMC_SelectAll, NULL );
	pmFM_SingleFolder->AddSubMenu( ML_SORTING_METHOD, psSort, TaviSysParam.sort, NULL, NULL );
//	pmFM_SingleFolder->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );


	/////////////////////////////////////////////////////////////////
	// FileManager - Multi File/Folder Selected
	pmFM_MultiItems = new CPopUpMenu( 0, 0, 320, 220 );
    #if !MP_REMOVE_COPY_FUNC
	pmFM_MultiItems->AddSubMenu( ML_COPY, NULL, -1, FMMC_Copy, NULL );
    #endif
	pmFM_MultiItems->AddSubMenu( ML_MOVE, NULL, -1, FMMC_Move, NULL );
	pmFM_MultiItems->AddSubMenu( ML_DELETE, NULL, -1, FMMC_Delete, NULL );
	pmFM_MultiItems->AddSubMenu( ML_RELEASE_ALL, NULL, -1, FMMC_ReleaseAll, NULL );
//	pmFM_MultiItems->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );

	pmFM_Copy_SameFolder = new CPopUpMenu( 0, 0, 320, 220 );
	pmFM_Copy_SameFolder->AddSubMenu( ML_CANCEL, NULL, -1, FMMC_Cancel, NULL );
//	pmFM_Copy_SameFolder->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );

	pmFM_Copy_DiffFolder = new CPopUpMenu( 0, 0, 320, 220 );
	pmFM_Copy_DiffFolder->AddSubMenu( ML_PASTE, NULL, -1, FMMC_Paste4Copy, NULL );
	pmFM_Copy_DiffFolder->AddSubMenu( ML_CANCEL, NULL, -1, FMMC_Cancel, NULL );
//	pmFM_Copy_DiffFolder->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );


	pmFM_Move_SameFolder = new CPopUpMenu( 0, 0, 320, 220 );
	pmFM_Move_SameFolder->AddSubMenu( ML_CANCEL, NULL, -1, FMMC_Cancel, NULL );
//	pmFM_Move_SameFolder->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );

	pmFM_Move_DiffFolder = new CPopUpMenu( 0, 0, 320, 220 );
	pmFM_Move_DiffFolder->AddSubMenu( ML_MOVE, NULL, -1, FMMC_Paste4Move, NULL );
	pmFM_Move_DiffFolder->AddSubMenu( ML_CANCEL, NULL, -1, FMMC_Cancel, NULL );
//	pmFM_Move_DiffFolder->AddSubMenu( ML_CLOSE, NULL, -1, NULL, NULL );

	return TRUE;
}


int FM_DeleteMenuItems()
{
	if(pmFM_SingleFile != NULL) { 
	  delete pmFM_SingleFile; 
	  pmFM_SingleFile = NULL; 
  }
	if(pmFM_SingleFolder != NULL) { 
	  delete pmFM_SingleFolder; 
	  pmFM_SingleFolder = NULL; 
  }
	if(pmFM_MultiItems != NULL) { 
	  delete pmFM_MultiItems; 
	  pmFM_MultiItems = NULL; 
  }
	if(pmFM_Copy_SameFolder != NULL) { 
	  delete pmFM_Copy_SameFolder; 
	  pmFM_Copy_SameFolder = NULL; 
  }
	if(pmFM_Copy_DiffFolder != NULL) { 
	  delete pmFM_Copy_DiffFolder; 
	  pmFM_Copy_DiffFolder = NULL; 
  }
	if(pmFM_Move_SameFolder != NULL) { 
	  delete pmFM_Move_SameFolder; 
	  pmFM_Move_SameFolder = NULL; 
  }
	if(pmFM_Move_DiffFolder != NULL) { 
	  delete pmFM_Move_DiffFolder; 
	  pmFM_Move_DiffFolder = NULL; 
  }

	return TRUE;
}

static int 
FM_TaskStep2( TTaskInitParam* tParam, TMsgEntry* tMsg )
{
	for(;;)
		{
		////////////////////////////////////////////////////////////////////////////
		// FileManager Task Message Peeking
		if( MSG_Peek( tParam->ssTaskId, tMsg ) == 0)
			{
			// DEBUGMSG(2,("[0]This is FileManager_Task: Before FM_MsgProc()\n"));
			if( FM_MsgProc( tMsg->ssToTaskId, tMsg->ulMsgId, tMsg->usParam1, tMsg->usParam2 ) == -1)
				{
				break;
				}
			}
		usleep(100);
		}

	return TRUE;
}


int FileManager_Task( TTaskInitParam* tParam )
{
	////////////////////////////////////////////////////////////////////////////
	// Init Video Player Task
	TMsgEntry tMsg;
	RMuint8* palette					= NULL;
	static char* aCurrentFullPathDir = (char*)malloc(1024);

	g_TopPanel->SetTitle( ML_BROWSER);
	g_TopPanel->SetLogo( CTopPanel::LOGO_ID_FILEMANAGER );
	g_TopPanel->Update();

	FM_CursorLoopInit();
	
	pFileManager						= (tFILEMANAGER_CONFIG*) new tFILEMANAGER_CONFIG;
	pFileManager->Param				= tParam;
	pFileManager->TaskId				= tParam->ssTaskId;
	pFileManager->CurrentFullPathDir		=  aCurrentFullPathDir;
	pFileManager->CurrentFullPathDir[0]	= '\0';

	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(TRUE);

	FM_CreateMenuItems();

	ClearOsd();
	showbackground(ID_BG_FILEMANAGER);
	pLFBrowser->SetActiveMode( _FBMODE_FILEMANAGER, FM_MsgProc_FileBrowser, FALSE );
	pLFBrowser->Show();
	FM_RefreshDiskSpace();
	FM_DrawBottomItems((OSD_REFRESH|SHOW_BTN_GUIDE));
	Reset_FileBrowserTimer();

	pmCurrentMenu = NULL;

	FM_TaskStep2( tParam, &tMsg );

	/////////////////////////////////////////////////////////////
	// Exit Video Player Task
	AP_Remount();
	FM_DrawMsgBox( FMM_WAIT, 0 );
	for( int i=0; i<20; i++ ) {
  	sched_yield();
	}
	g_TopPanel->SetVisible(false);		
	g_TopPanel->Update();

	tMsg.usParam1=0;
	tMsg.usParam2=0;

	free(aCurrentFullPathDir);

	pLFBrowser->Clear_IL_SIL_All();
	FM_DeleteMenuItems();

	pMbx->Hide();
	return TaskExit_Normal;
	
}


 
