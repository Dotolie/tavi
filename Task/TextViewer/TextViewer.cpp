//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : TextViewer Task Source Code File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/TextViewer/TextViewer.cpp,v $
// $Revision: 1.6 $
// $Date: 2006/04/19 13:56:24 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: TextViewer.cpp,v $
// Revision 1.6  2006/04/19 13:56:24  zstein
// - fixed MAX-FILE-OPEN bug and ui bug.
//
// Revision 1.5  2006/04/17 07:17:56  zstein
// - resized background osd area.
//
// Revision 1.4  2006/04/15 04:27:37  ywkim
// Change interval time for autoscroll
//
// Revision 1.3  2006/04/13 08:46:00  ywkim
// Draw osd on overall background, when viewing text
//
// Revision 1.2  2006/04/13 01:47:03  ywkim
// Add instruction of SaveDirInfo() on end of task
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*****************************************************************************
#include <Lib/LibDefines.h>
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

#include "Lib/LibImage.h"
#include "Lib/LibFileBrowser.h"

#include "Task/TopPanel.h"
#include "Task/tavi_global.h"
#include "fs_fat.h"

#include "TextViewer.h"

#define EBOOK_DEBUG         0
#if EBOOK_DEBUG
#define EBOOK_DPRINT        printf
#else
#define EBOOK_DPRINT
#endif

#define MAX(X,Y) (((X)>=(Y))?(X):(Y))	// to find the max between two number.
#define MIN(X,Y) (((X)<=(Y))?(X):(Y))	// to find the min between two number.

// Local Values
static const uchar_t sSlash[2] = {0x002f, 0x0000};	
static const uchar_t sDot[2] = {0x002e, 0x0000};	
static uchar_t sSavefileDir[FATFS_MAX_PATHNAME_LENGTH];
static uchar_t sSavefilename[FATFS_MAX_PATHNAME_LENGTH];
static uchar_t sLast[32];
static char cTemp[FATFS_MAX_PATHNAME_LENGTH];
static uchar_t ucTemp[FATFS_MAX_PATHNAME_LENGTH];

static char BlockBuffer[TEXTVIEWER_BLOCK_BUFFER_SIZE];

static uchar_t Fullpath[FATFS_MAX_PATHNAME_LENGTH];
static uchar_t Filename[FATFS_MAX_PATHNAME_LENGTH];
static char sTaskName[256];

static FATFS_DIR_ENTRY_INFO dd;
static FATFS_ERROR err;
static RMuint32 FD;
static FILE* pF; 
static CMsgBox* pMbx;
static TMsgEntry tMsgTemp;

//TTaskInitParam tInitParam;
tTEXTVIEWER_CONFIG*		pTV;

CPopUpMenu*	pmView;
extern CPopUpMenu* pmFileBrowser;
extern CPopUpSubMenu* psBookmark;
CPopUpSubMenu* psMoveTo;
CPopUpSubMenu* psAutoScroll;
extern CPopUpSubMenu* psSort;


// External Values
extern CFileBrowser*			pLFBrowser;
extern MpegDecoder *			pMpegDecoder;
extern TTAVI_GLOBAL_CONFIG* 	pTAVI;
extern CTopPanel* g_TopPanel;
extern BITMAPFILEINFO256 		TAVI_IMAGES[];
extern CTextEngine_BDF* 		pT;
extern int handle_key;
extern keystruct tempkey;
extern CBookmarkDialogBox* 		pBookmarkDbx;
CScrollBar3*				pSb3;
extern int errno;

extern long TV_AutoScroll_RegUT1;	// TextViewer AutoScroll Slow
extern long TV_AutoScroll_RegUT2;	// TextViewer AutoScroll Mid
extern long TV_AutoScroll_RegUT3;	// TextViewer AutoScroll Fast


extern void showbackground (char *bkgnd);
extern void showbackground (int menuID);
extern int TAVI_CreateTask_For_CallByFileManager(  int ssTaskId, TTaskInitParam* tParam );

int TV_RestartScrollWithLastSpeed( void );
int TV_SetAutoScrollOff( RMuint8* parameter );
///////////////////////////////////////////////////////////
// MessageBox Process

void TV_DbxMsgPeek()
{
	TMsgEntry tMsg;
	usleep(500);
	for(;;)
		{
		#if 0
		usleep(500);
		if( AppEventCheck( pTV->TaskId, &tMsg) == 0 )
			{ 
			MSG_Send( pTV->TaskId, &tMsg ); 
			break;
			}
		#endif
		usleep(500);
		if( MSG_Peek( pTV->TaskId, &tMsg ) == 0)
			{
			DEBUGMSG(1,("TV_DbxMsgPeek()\n"));
			if( TV_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) < 0)
				break;
			}
		}	
}
RMuint32 TV_DrawMsgBox( RMuint32 msg, RMuint32 option )
{
	switch( msg )
		{
		case TVM_OPEN_FAILED_LONG_PATH:
			Stop_FileBrowserTimer();
			sprintf( cTemp, "%s", "Unable to open a directory.|Path name is too long.");
			str2ustr( ucTemp, cTemp, strlen(cTemp) );
			pMbx->Reset( ML_INFORMATION, _ORANGE, ucTemp, -1, -1, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			TV_DbxMsgPeek();
			pMbx->Hide();
			break;
		case TVM_OPEN_FAILED_LONG_LINE:
			Stop_FileBrowserTimer();
			sprintf( cTemp, "%s", "Unable to open a file.|Max line over.");
			str2ustr( ucTemp, cTemp, strlen(cTemp) );
			pMbx->Reset( ML_INFORMATION, _ORANGE, ucTemp, -1, -1, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			TV_DbxMsgPeek();
			pMbx->Hide();
			break;
		case TVM_VOLUME_FULL:
			Stop_FileBrowserTimer();
			sprintf( cTemp, "%s", "Harddisk Full!|(0x%xh)", option );
			str2ustr( ucTemp, cTemp, strlen(cTemp) );
			pMbx->Reset( ML_INFORMATION, _ORANGE, ucTemp, 244, 150, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			TV_DbxMsgPeek();
			pMbx->Hide();
			break;
		case TVM_EXCEPTION_FOUND:
			Stop_FileBrowserTimer();
			sprintf( cTemp, "Exception Error.|(0x%x)", option );
			str2ustr( ucTemp, cTemp, strlen(cTemp) );
			pMbx->Reset( ML_INFORMATION, _ORANGE, ucTemp, 244, 150, 0, DL_BTN_OK_ONLY );
			pMbx->Show();
			ClearKeyBuffer();
			TV_DbxMsgPeek();
			pMbx->Hide();
			break;
		}
	return TRUE;
}


int TV_ShowFSE( FATFS_ERROR err )	 // FSE; File System Error
{
    EBOOK_DPRINT( "TV_ShowFSE(err:0x%x)\n", err );
	if( err==FATFS_ERROR_VOLUME_FULL || err==FATFS_ERROR_VOLUME_LOW_ON_DISK_SPACE  )
		return ( TV_DrawMsgBox(TVM_EXCEPTION_FOUND,err) );
}
void TV_DrawPageInfo()
{
	char szInfomation[64];
	uchar_t ucPageInfoText[64];
	RMint64 temp;
	int num;
	int bp;

	temp = (pTV->AbsoluteCursor+pTV->CurrentPage_RealSize) * 1000;
	temp = temp/pTV->TotalFileSize;
	num = (int)(temp/10);
	bp = (int)(temp-(num*10));
	sprintf(szInfomation, "Line %ld (%d.%d%%)", pTV->LineCounter, num, bp );
//	DEBUGMSG(1,("((szInformation:[%s]))\n", szInfomation ));
	str2ustr( ucPageInfoText, szInfomation );
	pT->DrawUC16Text( ucPageInfoText, 10, 240-16, _WHITE, 12, DT_TRANSPARENCY );
}
void TV_DrawBottomItems( eOSD_REFRESH_OPTION refresh )
{
//	uchar_t ucFreeSpaceText[40];
	DrawBottomGuideBGPannel( SKIP_OSD_REFRESH );
	TV_DrawPageInfo();

	if( refresh==OSD_REFRESH )
		pMpegDecoder->RefreshOsd();
}
void TV_DrawTopItems( eOSD_REFRESH_OPTION refresh )
{
	g_TopPanel->SetTitle((char*)pLFBrowser->Filename, true);
	g_TopPanel->SetVisible(true);
	g_TopPanel->Update();

	if( refresh==OSD_REFRESH )
		pMpegDecoder->RefreshOsd();	
}



int CheckBlockNumberDescent(RMuint32 AbsoluteCursor )
{
	int BlockNumber;
	RMuint32 BlockCursor;
	RMuint32 PageCursor;

	BlockNumber = (int)(AbsoluteCursor/ACTUAL_BUFFER_SIZE);
	BlockCursor = (RMuint32)(BlockNumber * ACTUAL_BUFFER_SIZE);
	PageCursor = AbsoluteCursor % ACTUAL_BUFFER_SIZE;

	if( PageCursor < UPPER_BORDERLINE && BlockCursor > 0 )
		BlockNumber--;

	return BlockNumber;
}
 
int CheckBlockNumber(RMuint32 AbsoluteCursor )
{
	int BlockNumber;
	BlockNumber = (int)(AbsoluteCursor/LOWER_BORDERLINE);
	return (BlockNumber);
}
int CalcBufferPosition( int Direction )
{
	RMuint32 preBC = pTV->CurrentBlock_Cursor;
	RMuint32 prePC = pTV->CurrentPage_Cursor;
	int BlockNum;
	RMuint32 PageCur;
	int fIsBorderSection = FALSE;

	if( Direction < 0 ) // Up
		{

		BlockNum = CheckBlockNumber(pTV->AbsoluteCursor);
		PageCur = (RMuint32)(pTV->AbsoluteCursor % ACTUAL_BUFFER_SIZE);

		if( PageCur < UPPER_BORDERLINE )
			{
			if( pTV->LineIndex[pTV->LineCounter-1]  < BlockNum*ACTUAL_BUFFER_SIZE )
				fIsBorderSection = TRUE;
			}

		if( BlockNum == 0)
			{
			pTV->CurrentBlock_Cursor = 0;
			pTV->CurrentPage_Cursor = pTV->AbsoluteCursor;
			return CHK_BP_RETURN_NORMAL;
			}
		else
			{
			if( fIsBorderSection )
				{
				pTV->CurrentBlock_Cursor = (RMuint32) (((int)(pTV->AbsoluteCursor/ACTUAL_BUFFER_SIZE)-1) * ACTUAL_BUFFER_SIZE);
				pTV->CurrentPage_Cursor  = (RMuint32) PageCur+ACTUAL_BUFFER_SIZE; //pTV->AbsoluteCursor - pTV->CurrentBlock_Cursor; //(RMuint32) PageCur+ACTUAL_BUFFER_SIZE;

				if( preBC != pTV->CurrentBlock_Cursor )
					{
					return CHK_BP_RETURN_NEW_BLOCK_NEEDS;
					}
				else
					{
					return CHK_BP_RETURN_NORMAL;
					}
				}
			else
				{
				pTV->CurrentBlock_Cursor = (RMuint32)(((int)(pTV->AbsoluteCursor / ACTUAL_BUFFER_SIZE)) * ACTUAL_BUFFER_SIZE);
				pTV->CurrentPage_Cursor = pTV->AbsoluteCursor - pTV->CurrentBlock_Cursor;
				return CHK_BP_RETURN_NORMAL;
				}
			}
		}

	else if( Direction > 0 )	// Down
		{
		pTV->CurrentBlock_Cursor = (RMuint32)((int)(pTV->AbsoluteCursor/ACTUAL_BUFFER_SIZE) * ACTUAL_BUFFER_SIZE);
		pTV->CurrentPage_Cursor  = (RMuint32)((int)pTV->AbsoluteCursor)%ACTUAL_BUFFER_SIZE;

		if( (preBC !=  pTV->CurrentBlock_Cursor||pTV->CurrentPage_Cursor > LOWER_BORDERLINE) )
			{
			return CHK_BP_RETURN_NEW_BLOCK_NEEDS;
			}
		else
			{
			return CHK_BP_RETURN_NORMAL;
			}
		}
	else
		ASSERT(0);

}



///////////////////////////////////////////////////////////
// Bookmark Process

static void TV_BuildSavePathname( uchar_t* BookmarkDir )
{
	memset( ucTemp, 0, FATFS_MAX_PATHNAME_LENGTH*sizeof(uchar_t) );
	memset( BookmarkDir, 0, FATFS_MAX_PATHNAME_LENGTH*sizeof(uchar_t) );
	ustrcpy( BookmarkDir, pLFBrowser->CurrentFullPathDir );
	sprintf( cTemp, "/%s", _TEXTVIEWER_BOOKMARK_DATA );
	str2ustr( ucTemp, cTemp );
	ustrcat( BookmarkDir, ucTemp );
}
static void TV_BuildSaveFilename( uchar_t* Filename, int isBookmark )
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

static FATFS_ERROR TV_ReadSaveData( tTEXTVIEWER_BOOKMARKDATA* pBOOKMARK_SET, uchar_t* Savefile_FullPath )
{
	RMuint32 rc;	// read counter
	RMuint32 fd;	// file descriptor
	FATFS_ERROR err = FATFS_ERROR_NO_ERROR;

	TAVI_WakeUp();

	err = fatfs_wfopen( (RMuint16*)Savefile_FullPath, _O_RDONLY, &fd );
	if( err == FATFS_ERROR_NO_ERROR) {
	    err = fatfs_fread( fd, (RMuint8*)pBOOKMARK_SET, 
	            (sizeof(tTEXTVIEWER_BOOKMARKDATA)*MAXIMUM_BOOKMARK_ALLOW), 
	            &rc );
		if( err == FATFS_ERROR_NO_ERROR ) {
			fatfs_fclose(fd);
		}
	}
	
	return err;
}
static FATFS_ERROR TV_SaveData( tTEXTVIEWER_BOOKMARKDATA* pBOOKMARK_SET, U32 AbsoluteCursor, U32 LineCounter, int slot, uchar_t* Savefile_FullPath, RMuint32 SAVE_FLAGS )
{
	FATFS_ERROR err;
	RMuint32 rc, wc, fd;
	int slotindex;

	if( slot < 0 ) {// Auto Detect
		for( slotindex = 0 ; slotindex<MAXIMUM_BOOKMARK_ALLOW-1; slotindex++)
			{
			if( pBOOKMARK_SET[slotindex].AbsoluteCursor == 0 )
				{
				pBOOKMARK_SET[slotindex].AbsoluteCursor = AbsoluteCursor;
				pBOOKMARK_SET[slotindex].LineCounter = LineCounter;
				break;
				}
			}
		pBOOKMARK_SET[slotindex].AbsoluteCursor = AbsoluteCursor; // slotindex == MAXIMUM_BOOKMARK_ALLOW-1
		pBOOKMARK_SET[slotindex].LineCounter = LineCounter;
	}
	else {
		pBOOKMARK_SET[slot].AbsoluteCursor = AbsoluteCursor; 
		pBOOKMARK_SET[slot].LineCounter = LineCounter;		
	}	

	TAVI_WakeUp();

	// Save Bookmark Data
	err = fatfs_wfopen( (RMuint16*)sSavefilename, SAVE_FLAGS, &fd );
	if( err == FATFS_ERROR_NO_ERROR ) {
		err=fatfs_fwrite( fd, (RMuint8*)pBOOKMARK_SET, 
		        (sizeof(tTEXTVIEWER_BOOKMARKDATA)*MAXIMUM_BOOKMARK_ALLOW), 
		        &wc );
	}
	fatfs_fclose( fd );

	
	return err;
}

#include "Component/String.h"
static FATFS_ERROR TV_CreateSaveDir( uchar_t* SaveDirnameOnly )
{
	RMuint32 dd;
	FATFS_ERROR err = FATFS_ERROR_NO_ERROR;

	TAVI_WakeUp();
	if( (err = fatfs_wdopen((RMuint16*)pLFBrowser->CurrentFullPathDir, &dd )) > FATFS_ERROR_NO_ERROR ) 	
		{
		TV_ShowFSE( err );
		fatfs_dclose(dd);
		return err; 
		}
	else 
		{
		if( (err = fatfs_wdcreate_dir( dd, (RMuint16*)SaveDirnameOnly )) > FATFS_ERROR_NO_ERROR ) 	
			{
			if( err == FATFS_ERROR_NAME_ALREADY_DOES_EXIST )
				{
				fatfs_dclose(dd);
				return FATFS_ERROR_NO_ERROR;
				}
			
			TV_ShowFSE( err );
			fatfs_dclose(dd);
			return err; 
			}
		fatfs_dclose(dd);
		return err;
		}
}
static int TV_Bookmarking( uchar_t sPathname[], uchar_t sFilename[], RMuint32 AbsoluteCursor, RMuint32 LineCounter, int slot )
{
	tTEXTVIEWER_BOOKMARKDATA BOOKMARK_SET[ MAXIMUM_BOOKMARK_ALLOW ];

	RMuint32 fd;
	RMuint32 rc, wc;
	FATFS_ERROR err;
	RMuint32 SAVE_FLAGS;
	int t0, t1, t2, t3, t4, t5, t6;

	//U32 ulTempTime = BookmarkingTime;
//	Temp1.BookmarkingTime = BookmarkingTime;
//	Temp1.LineCursor = LineCursor;

	

	t0 = GetMSTime();
	memset( BOOKMARK_SET, 0, sizeof(tTEXTVIEWER_BOOKMARKDATA)*MAXIMUM_BOOKMARK_ALLOW );
	TV_BuildSavePathname( sSavefileDir);
	TV_BuildSaveFilename( sFilename, TRUE );
	DEBUG_U16MSG( sSavefileDir, "-sSavefileDir");
	DEBUG_U16MSG( sSavefilename, "-sSavefilename");
	//sSavefilename is FULL-PATH
	// 1.Directory Exist Check
	// 2.Create Directory
	// 3.Create File
	t1 = GetMSTime();

	TAVI_WakeUp();

	if( TV_ReadSaveData( BOOKMARK_SET, sSavefilename ) == FATFS_ERROR_NO_ERROR )
		{
		SAVE_FLAGS = (_O_WRONLY|_O_TRUNC);
		}
	else
		{
		TV_CreateSaveDir(sFilename);
		SAVE_FLAGS = (_O_WRONLY|_O_CREAT);
		}

	TV_SaveData( BOOKMARK_SET, AbsoluteCursor, LineCounter, slot, sSavefilename, SAVE_FLAGS );

	//fatfs_fclose( fd );
	t4 = GetMSTime();

	return TRUE;
}

static int  TV_BookmarkLoading( uchar_t sPathname[], uchar_t sFilename[] )// V3
{
	tTEXTVIEWER_BOOKMARKDATA BOOKMARK_SET[ MAXIMUM_BOOKMARK_ALLOW ];
	RMuint32 fd;
	RMuint32 rc, wc;
	FATFS_ERROR err;

	memset( BOOKMARK_SET, 0, sizeof(tTEXTVIEWER_BOOKMARKDATA)*MAXIMUM_BOOKMARK_ALLOW );

	TV_BuildSavePathname( sSavefileDir);
	TV_BuildSaveFilename( sFilename, TRUE );

	TAVI_WakeUp();
		
	// Check Bookmark Data Aleady Exist
	err=fatfs_wfopen( (RMuint16*)sSavefilename, _O_RDONLY, &fd );
	if( err == FATFS_ERROR_NO_ERROR ) {
		err = fatfs_fread( fd, (RMuint8 *)BOOKMARK_SET, 
		            (sizeof(tTEXTVIEWER_BOOKMARKDATA)*MAXIMUM_BOOKMARK_ALLOW), 
		            &rc );
	}

    if( !err ) {
    	for( int index=0; index<MAXIMUM_BOOKMARK_ALLOW; index++) {
    		if( BOOKMARK_SET[index].AbsoluteCursor > 0 ) {
    			tBookmarkNode* pNode = new tBookmarkNode;
    			pNode->ulSavetime =  BOOKMARK_SET[index].AbsoluteCursor;
    			pNode->LineCounter =  BOOKMARK_SET[index].LineCounter;
    			pBookmarkDbx->AddBookmark( pNode );
    		}
    	}
    }

	if( fd ) {
	    fatfs_fclose( fd );
	}

	return ( err ? FALSE : TRUE );
}


int TV_DeleteAnBookmark( uchar_t sPathname[], uchar_t sFilename[], int BeDeletedSlot )
{
	tTEXTVIEWER_BOOKMARKDATA NEW_BOOKMARK_SET[ MAXIMUM_BOOKMARK_ALLOW ];
	tTEXTVIEWER_BOOKMARKDATA BOOKMARK_SET[ MAXIMUM_BOOKMARK_ALLOW ];

	RMuint32 fd;
	RMuint32 rc, wc;
	FATFS_ERROR err;

	memset( NEW_BOOKMARK_SET, 0, sizeof(tTEXTVIEWER_BOOKMARKDATA)*MAXIMUM_BOOKMARK_ALLOW );
	memset( BOOKMARK_SET, 0, sizeof(tTEXTVIEWER_BOOKMARKDATA)*MAXIMUM_BOOKMARK_ALLOW );


	TV_BuildSavePathname( sSavefileDir);
	TV_BuildSaveFilename( sFilename, TRUE );

	// Check Bookmark Data Aleady Exist
	TAVI_WakeUp();
	
	if( (err = fatfs_wfopen( (RMuint16*)sSavefilename, _O_RDONLY, &fd )) == 0 )
		{
		// Exist
		usleep(500);
		if( (err = fatfs_fread( fd, (RMuint8 *)BOOKMARK_SET, (sizeof(tTEXTVIEWER_BOOKMARKDATA)*MAXIMUM_BOOKMARK_ALLOW), &rc )) > FATFS_ERROR_NO_ERROR )
			{
			usleep(500);
			TV_ShowFSE( err );
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
			{
			NEW_BOOKMARK_SET[slotindex].AbsoluteCursor= BOOKMARK_SET[slotindex].AbsoluteCursor;
			NEW_BOOKMARK_SET[slotindex].LineCounter= BOOKMARK_SET[slotindex].LineCounter;
			}
		else
			{
			NEW_BOOKMARK_SET[slotindex].AbsoluteCursor= BOOKMARK_SET[slotindex+1].AbsoluteCursor;
			NEW_BOOKMARK_SET[slotindex].LineCounter = BOOKMARK_SET[slotindex+1].LineCounter;
			}
		
		}

	usleep(500);
	// Save Bookmark Data
	if( (err = fatfs_wfopen( (RMuint16*)sSavefilename, _O_WRONLY|_O_CREAT, &fd )) > FATFS_ERROR_NO_ERROR)
		{
		usleep(500);
		TV_ShowFSE( err );
		if(fd) fatfs_fclose(fd);
		return FALSE;
		}
	
	usleep(500);
	err = fatfs_fwrite( fd, (RMuint8*)NEW_BOOKMARK_SET, (sizeof(tTEXTVIEWER_BOOKMARKDATA)*MAXIMUM_BOOKMARK_ALLOW), &wc );
	ASSERT(!err);
	fatfs_fclose( fd );

	pBookmarkDbx->ClearItems();

	for( int index=0; index<MAXIMUM_BOOKMARK_ALLOW; index++)
		{
		usleep(500);
		if( NEW_BOOKMARK_SET[index].AbsoluteCursor > 0 )
			{
			tBookmarkNode* pNode = new tBookmarkNode;
			pNode->ulSavetime = NEW_BOOKMARK_SET[index].AbsoluteCursor;
			pNode->LineCounter = NEW_BOOKMARK_SET[index].LineCounter;
			pBookmarkDbx->AddBookmark( pNode );
			}
		}
		
	
	return TRUE;
}

tTEXTVIEWER_BOOKMARKDATA* TV_CheckLastResumeFile( 
    uchar_t sFullPathDir[], 
    uchar_t sFilename[], 
    tTEXTVIEWER_BOOKMARKDATA* pRESUME )
{
	RMuint32 fd;
	FATFS_ERROR err;
	RMuint32 ret;
	tTEXTVIEWER_BOOKMARKDATA* p=NULL;

	pRESUME->AbsoluteCursor = 0;
	pRESUME->LineCounter = 1;

	TV_BuildSavePathname( sSavefileDir);
	TV_BuildSaveFilename( sFilename, FALSE );

	TAVI_WakeUp();

    err = fatfs_wfopen( (RMuint16*)sSavefilename, _O_RDONLY, &fd );
	if( err == FATFS_ERROR_NO_ERROR ) {
		pRESUME->AbsoluteCursor = 0;
		pRESUME->LineCounter = 0;
		err=fatfs_fread( fd, (RMuint8*)pRESUME, 
		        sizeof(tTEXTVIEWER_BOOKMARKDATA), &ret );
		if( err == FATFS_ERROR_NO_ERROR ) {
			// Is "u32FilepositionInSeconds"  valid value?
			if( pRESUME->AbsoluteCursor > 0 && pRESUME->AbsoluteCursor < 32768 ) {
				// Resume Process
				p = pRESUME;
			}
		}
	}

	fatfs_fclose( fd );

    return p;
}
int TV_SaveCurrentPosition( uchar_t sPathname[], uchar_t sFilename[], U32 AbsoluteCursor, U32 LineCounter )
{
	RMuint32 fd;
	RMuint32 ret;
	RMuint32 dd;
	RMuint32 nw;
	FATFS_ERROR err;
	U32 u32Fileposition;
	RMuint32 savetime = GetMSTime();
	tTEXTVIEWER_BOOKMARKDATA BOOKMARK;

	BOOKMARK.AbsoluteCursor = AbsoluteCursor;
	BOOKMARK.LineCounter = LineCounter;

	TV_BuildSavePathname( sSavefileDir);
	TV_BuildSaveFilename( sFilename, FALSE );

	TAVI_WakeUp();

	// force to create a _db folder
	uchar_t path[100];
	str2ustr( path, _TEXTVIEWER_BOOKMARK_DATA );
	TV_CreateSaveDir( path );

    err = fatfs_wfopen( (RMuint16*)sSavefilename, _O_WRONLY|_O_TRUNC, &fd );
    if( err == FATFS_ERROR_FILE_NAME_NOT_FOUND ) {
        err=fatfs_wfopen((RMuint16*)sSavefilename, _O_WRONLY|_O_CREAT, &fd );
    }
    
	if( err == FATFS_ERROR_NO_ERROR ) {
	    err= fatfs_fwrite( fd, (RMuint8*)&BOOKMARK, 
	            sizeof(tTEXTVIEWER_BOOKMARKDATA), &nw );
		// Write Failed.
		// Unknown Error, Exception.
		TV_ShowFSE( err );
	}

	fatfs_fclose( fd );

    return ( err ? FALSE : TRUE );
}



///////////////////////////////////////////////////////////
// Text Data Process

static RMuint32 TV_BlockRead( RMuint32 BlockStartPoint )
{
	RMuint32 nBytesRead;

	TAVI_WakeUp();

	err=fatfs_fseek( FD, (RMint32)BlockStartPoint, SEEK_SET);
	if( err > FATFS_ERROR_NO_ERROR ) {
		TV_ShowFSE(err);
		return FALSE;
	}
	memset( BlockBuffer, 0, TEXTVIEWER_BLOCK_BUFFER_SIZE*sizeof(char));
	err=fatfs_fread( FD, (RMuint8*)BlockBuffer, 
	                 TEXTVIEWER_BLOCK_BUFFER_SIZE, &nBytesRead );
	if( err > FATFS_ERROR_NO_ERROR ) {
		TV_ShowFSE(err);
		return FALSE;
	}
	
	pTV->CurrentBlock_RealSize = nBytesRead;
	pTV->CurrentBlock_EndPoint = pTV->CurrentBlock_Cursor + pTV->CurrentBlock_RealSize;
	
	return nBytesRead;
}


RMuint32 TV_BlockCheck()
{
//	DEBUGMSG(1,("TV_BlockCheck()\n"));

	int bc_rows=0;
	int bc_tmp=0;
	int bc_ByteDisplayed=0;
#if 0
	DEBUGMSG(1,("-pTV->NextLine : %d \n", pTV->NextLine));
	DEBUGMSG(1,("-CurrentPage_Cursor:%ld\n", pTV->CurrentPage_Cursor));
	DEBUGMSG(1,("-CurrentBlock_Cursor:%ld\n", pTV->CurrentBlock_Cursor));
	DEBUGMSG(1,("-(pTV->LastBlock+1)*(TEXTVIEWER_BLOCK_BUFFER_SIZE-1024):%ld\n",(pTV->LastBlock+1)*(TEXTVIEWER_BLOCK_BUFFER_SIZE-1024)));
#endif
	if( (pTV->CurrentPage_Cursor< UPPER_BORDERLINE) && pTV->CurrentBlock_Cursor > 0 )
		{
		return TVB_CHK_RETURN_UPPERBLOCK_NEEDS;
		}

	else if( pTV->NextLine> LOWER_BORDERLINE )
		{
		if( pTV->CurrentBlock_Cursor < ((pTV->LastBlock+1)*(TEXTVIEWER_BLOCK_BUFFER_SIZE-1024)) )
			{
			return TVB_CHK_RETURN_LOWERBLOCK_NEEDS;
			}
		}

	else
		{
		return TVB_CHK_RETURN_NORMAL;
		}
}

int TV_DisplayOnePage( eOSD_REFRESH_OPTION option )
{
	int dop_rows=0;
	int dop_ByteDisplayed=0;
	int dop_tmp=0;
	int fIsAutoSplitActivated;
	int nSplited=0;
	int nConstantSplited_FromZero=0;

	pTV->LastView = _TVLS_TEXT_VIEW;

	if( option != CALCULATION_ONLY)
		{
		ClearOsd();
		}
	
	DrawBoxWithoutRefresh( _CP_BASE_COLOR_PALLETTE_INDEX, 0, 24, 320, 240-24-22-1 );
	for( dop_rows = 0; dop_rows< TV_MAX_ROWS; dop_rows++ ) {
		dop_tmp = pT->DrawEUC8LineText( &(BlockBuffer[pTV->CurrentPage_Cursor+dop_ByteDisplayed]), 6, 30+(dop_rows*15), _WHITE, _SHADOW, 12, DT_TEXTVIEWER, &fIsAutoSplitActivated );
		pTV->RowSp[dop_rows] = (pTV->CurrentPage_Cursor+dop_ByteDisplayed);
		if( (pTV->IsSplited[dop_rows] = fIsAutoSplitActivated) ) nSplited++;
		dop_ByteDisplayed += dop_tmp; 
	}

	pTV->nByteDisplayed = dop_ByteDisplayed;
	pTV->CurrentPage_RealSize = pTV->nByteDisplayed;
	pTV->CurrentPage_EndPoint = pTV->CurrentPage_Cursor + pTV->nByteDisplayed;


	if( option != CALCULATION_ONLY) {
		TV_DrawBottomItems(SKIP_OSD_REFRESH);
		TV_DrawTopItems(SKIP_OSD_REFRESH);
		pSb3->ReDraw( pTV->TotalFileSize, pTV->AbsoluteCursor, pTV->CurrentPage_RealSize );
	
		if( option == OSD_REFRESH ) {
			pMpegDecoder->RefreshOsd();
		}
	}

	return nSplited;
}


RMuint32 TV_CalcTotalLines()
{
	RMuint32 TotalLines = 0;
	int nByteRead;

	for(int i=0; i<=pTV->LastBlock; i++ )
		{
		nByteRead=TV_BlockRead( (i*TEXTVIEWER_BLOCK_BUFFER_SIZE) );
		for( int j=0; j< MIN(nByteRead,TEXTVIEWER_BLOCK_BUFFER_SIZE); j++ )
			{
			if( BlockBuffer[j] == '\n' )
				TotalLines++;
			}
		}

	TotalLines++;


	return TotalLines;

}
RMuint32* TV_CreatLineIndexBuffer( RMuint32 TotalLines )
{
	return (pTV->LineIndex = (RMuint32*) malloc( sizeof(RMuint32)*(TotalLines+1) ));
}
void TV_CreatLineIndex()
{
	int idx=0;
	int nByteRead;

	pTV->LineIndex[idx] = 0;
	pTV->LineIndex[++idx] = 0;

	for(int i=0; i<=pTV->LastBlock; i++ ) {
		nByteRead=TV_BlockRead( (i*TEXTVIEWER_BLOCK_BUFFER_SIZE) );
		for( int j=0; j< MIN(nByteRead,TEXTVIEWER_BLOCK_BUFFER_SIZE); j++ ) {
			if( BlockBuffer[j] == '\n' ) {
				pTV->LineIndex[++idx] = i*TEXTVIEWER_BLOCK_BUFFER_SIZE + j +1;
			}
		}
	}
}


int TV_OpenFATFS( uchar_t* Filename )
{
	BuildFullPath( Fullpath, pLFBrowser->CurrentFullPathDir, Filename );

	TAVI_WakeUp();

	err=fatfs_wfopen( (RMuint16*)Fullpath, _O_RDONLY, &FD );
	if( err > FATFS_ERROR_NO_ERROR ) {
		TV_ShowFSE(err);
		if(FD) fatfs_fclose(FD);
		return FALSE;
	}

	pTV->TotalFileSize = pLFBrowser->GetCurrentListItemFilesize();

	pTV->LastBlock = (int)(pTV->TotalFileSize/TEXTVIEWER_BLOCK_BUFFER_SIZE);
	pTV->LastBlockSize = pTV->TotalFileSize - (TEXTVIEWER_BLOCK_BUFFER_SIZE*(pTV->LastBlock+1));

	// Add for New Indexing
	pTV->TotalLines = TV_CalcTotalLines();
	if(!TV_CreatLineIndexBuffer( pTV->TotalLines )) {
		TV_DrawMsgBox( TVM_OPEN_FAILED_LONG_LINE, 0 );
		if(FD) fatfs_fclose(FD);
		return FALSE;
	}

	TV_CreatLineIndex();

	return TRUE;
}

void TV_CloseFATFS()
{
	TAVI_WakeUp();

	if( FD ) {
		err = fatfs_fclose(FD);
		FD = 0;
	}
	
	pTV->TotalFileSize = 0;
	pTV->LastBlock = 0;
	pTV->LastBlockSize = 0;

	if( pTV->LineIndex ) {
		free( pTV->LineIndex );
	}
}

static int TV_StartView( uchar_t* Filename )
{
	TAVI_WakeUp();

	if( !TV_OpenFATFS(Filename) ) {
		return FALSE;
	}
	
	CalcBufferPosition(+1);
	pSb3 = new CScrollBar3( (320-8-4),  (24), (240-24-22-8),  1, 1, 1 );	
	TV_BlockRead(pTV->CurrentBlock_Cursor);
	TV_DisplayOnePage(OSD_REFRESH);

	return TRUE;
}



int TV_SetAbsoluteCursor_ToPrevToken()
{
	int nGets=0;
	int nStrWidth=0;
	int TokenStatus=0;
	int NumOfDividers=0;
	int StartPointOnPage=0;
	int TempSp;
	CSize sz;

	
	TokenStatus = TOKEN_STATUS_OVERFLOW;


/*

277: 0x2eh(.)
276: 0xah(
)
)75: 0xdh(
274: 0x7dh(})
273: 0x20h( )
272: 0x3bh(;)

	... pTV->CurrentLine = 277 ...
	S.P must be 275 
	So, i=(pTV->CurrentLine-2)

	

*/

	for( int i=(pTV->CurrentLine-2); i >=0; i--)
		{
		if( pTV->CurrentBlock_Cursor == 0 && i==0 )
			{
			StartPointOnPage = i;
			TokenStatus = TOKEN_STATUS_TOP;
			break;
			}
		
		if( BlockBuffer[i] == '\n' )
			{
			StartPointOnPage = i+1;
			TokenStatus = TOKEN_STATUS_NORMAL;
			break;
			}
			
		}

	if( TokenStatus == TOKEN_STATUS_OVERFLOW )
		return TOKEN_STATUS_OVERFLOW;


	
	int CurUch = 0;
	int CurCh = 0;
	int nConvCh = 0;
	U16 UC16c;
	int StringPixelWidth=0;
	int LastStringPixelWidth=0;
	int MaxStringPixelWidth = 300-9;
	int UC16sDivPoint[16];

	NumOfDividers = 0;

	for( int loop=StartPointOnPage; loop<=(pTV->CurrentLine-2) && loop<(TEXTVIEWER_BLOCK_BUFFER_SIZE-1); loop++ )
		{
		nConvCh=0;
		
		if( BlockBuffer[loop] == '\n' ) break;
		else if( (nConvCh=pT->ConvertEUC8cToU16c( &UC16c, BlockBuffer[loop], BlockBuffer[loop+1] )) == 1 ) {  }
		else if( nConvCh == 2 ) { loop++; }
		else { ASSERT(0); break; }

		pT->GetCharSize(	UC16c, 12, &sz);
		StringPixelWidth += (sz.width+1);

		if( StringPixelWidth >= MaxStringPixelWidth )
			{
			UC16sDivPoint[NumOfDividers] = loop+1;
			NumOfDividers++;
			StringPixelWidth=0;
			}
	
		}

	LastStringPixelWidth = StringPixelWidth;


	if( NumOfDividers > 0 && StringPixelWidth!=0 )
		StartPointOnPage = UC16sDivPoint[NumOfDividers-1];

	switch( TokenStatus )
		{

		case TOKEN_STATUS_TOP:
			pTV->CurrentBlock_Cursor = 0;
			pTV->CurrentPage_Cursor = 0;
			pTV->AbsoluteCursor = 0;
			pTV->LineCounter = 1;
//			DEBUGMSG(1,("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
			return TOKEN_STATUS_TOP;
			break;

		case TOKEN_STATUS_NORMAL:
			if( NumOfDividers > 0 )
				{
				pTV->CurrentPage_Cursor = StartPointOnPage;
				pTV->AbsoluteCursor = pTV->CurrentBlock_Cursor + StartPointOnPage;
				pTV->LineCounter--;
				}

			else
				{
				pTV->CurrentPage_Cursor = StartPointOnPage;
				pTV->AbsoluteCursor = pTV->CurrentBlock_Cursor + StartPointOnPage;
				pTV->LineCounter--;
				}
			return TOKEN_STATUS_NORMAL;

			break;

		default:
			ASSERT(0);
		}
}




int TV_CommonKeyProcess_UP2( int ssTaskId )
{
	int BlockNumberBefore;
	int BlockNumberAfter;
	int TokenStatus;

	if( pTV->LineCounter <= 1 || pTV->AbsoluteCursor == 0 )
		return FALSE;

	pTV->AbsoluteCursor = pTV->LineIndex[ --(pTV->LineCounter) ];


	if( CalcBufferPosition(-1) == CHK_BP_RETURN_NEW_BLOCK_NEEDS )
		{
//		DEBUGMSG(1,("(((TOKEN_STATUS_OVERFLOW)))\n"));
		TV_BlockRead(pTV->CurrentBlock_Cursor);
		TV_DisplayOnePage(OSD_REFRESH);
		}
	else
		{
		TV_DisplayOnePage(OSD_REFRESH);
		}

	return TEXTVIEWER_RETURN_NORMAL;	
}

int TV_CommonKeyProcess_DOWN2( int ssTaskId )
{
	int BlockNumberBefore;
	int BlockNumberAfter;

	// EOF
	if( pTV->LineCounter >= pTV->TotalLines || (pTV->CurrentBlock_Cursor + pTV->CurrentPage_Cursor + pTV->CurrentPage_RealSize) >= pTV->TotalFileSize )
		return TEXTVIEWER_RETURN_EOF;

	pTV->AbsoluteCursor = pTV->LineIndex[ ++(pTV->LineCounter) ];
	if( CalcBufferPosition(+1) == CHK_BP_RETURN_NEW_BLOCK_NEEDS) 
		{
		TV_BlockRead(pTV->CurrentBlock_Cursor);
		TV_DisplayOnePage(OSD_REFRESH);
		}
	else
		{
		TV_DisplayOnePage(OSD_REFRESH);
		}

	return TEXTVIEWER_RETURN_NORMAL;	

}
int TV_CommonKeyProcess_LEFT2( int ssTaskId )
{

	int BlockNumberBefore;
	int BlockNumberAfter;
	int TokenStatus;
	int nSplited;

	RMuint32 LP = pTV->LineIndex[ pTV->LineCounter ];

	if( pTV->LineCounter <= 1 || pTV->AbsoluteCursor == 0 )
	{
	return false;
	}

	nSplited = 0;

	for(int loop=0; loop<TV_MAX_ROWS-2; loop++ )
		{

		if( pTV->LineCounter <= 1 || pTV->AbsoluteCursor == 0 || nSplited+loop >= TV_MAX_ROWS-2 )
			{
			break;
			}

		pTV->LineCounter--;
		pTV->AbsoluteCursor = pTV->LineIndex[ pTV->LineCounter ];

		if( CalcBufferPosition(-1) == CHK_BP_RETURN_NEW_BLOCK_NEEDS )
			{
			TV_BlockRead(pTV->CurrentBlock_Cursor);
			nSplited = TV_DisplayOnePage(CALCULATION_ONLY);
			}
		else
			{
			nSplited = TV_DisplayOnePage(CALCULATION_ONLY);
			}


		}

	if( CalcBufferPosition(-1) == CHK_BP_RETURN_NEW_BLOCK_NEEDS )
		{
		TV_BlockRead(pTV->CurrentBlock_Cursor);
		nSplited = TV_DisplayOnePage(OSD_REFRESH);
		}
	else
		{
		nSplited = TV_DisplayOnePage(OSD_REFRESH);
		}


	return TEXTVIEWER_RETURN_NORMAL;	
}
int TV_CommonKeyProcess_RIGHT2( int ssTaskId )
{
	int BlockNumberBefore;
	int BlockNumberAfter;
	RMuint32 EOP = pTV->CurrentBlock_Cursor + pTV->CurrentPage_Cursor + pTV->CurrentPage_RealSize;

	// EOF
	if( pTV->LineCounter >= pTV->TotalLines || EOP >= pTV->TotalFileSize )
		return FALSE;

	for(int i=0 ; i<TV_MAX_ROWS; i++ )
		{
		if( 	pTV->LineCounter >= pTV->TotalLines || 
			pTV->LineIndex[pTV->LineCounter+1] >= EOP )
			break;

		pTV->LineCounter++;
			}
	
	pTV->AbsoluteCursor = pTV->LineIndex[ pTV->LineCounter ];
	if( CalcBufferPosition(+1) == CHK_BP_RETURN_NEW_BLOCK_NEEDS) 
		{
		TV_BlockRead(pTV->CurrentBlock_Cursor);
		TV_DisplayOnePage(OSD_REFRESH);
		}
	else
		{
		TV_DisplayOnePage(OSD_REFRESH);
		}

	return TEXTVIEWER_RETURN_NORMAL;	


}


int TV_CommonKeyProcess_EXIT( int ssTaskId )
{
	TV_CloseFATFS();
	TV_SaveCurrentPosition( pLFBrowser->CurrentFullPathDir,  pLFBrowser->Filename, pTV->AbsoluteCursor, pTV->LineCounter );
	return TEXTVIEWER_RETURN_NORMAL;
}

int TV_CommonTouchpadProcess_LEFTDRAG( )
{
	return TEXTVIEWER_RETURN_NORMAL;
}
int TV_CommonTouchpadProcess_RIGHTDRAG( )
{
	return TEXTVIEWER_RETURN_NORMAL;
}
int TV_CommonTouchpadProcess_CLICK( )
{
	return TEXTVIEWER_RETURN_NORMAL;
}


int TV_MoveToTop(RMuint8* option)
{
	pTV->LineCounter = 1;
	pTV->AbsoluteCursor = pTV->LineIndex[pTV->LineCounter];

	CalcBufferPosition(+1);
	TV_BlockRead(0);
	TV_DisplayOnePage(OSD_REFRESH);
	return TEXTVIEWER_RETURN_NORMAL;	
}
int TV_MoveToBottom(RMuint8* option)
{
	int BlockNumberBefore;
	int BlockNumberAfter;

	pTV->LineCounter = pTV->TotalLines-TV_MAX_ROWS;
	pTV->AbsoluteCursor = pTV->LineIndex[pTV->LineCounter];
	
	CalcBufferPosition(+1);
	TV_BlockRead(pTV->CurrentBlock_Cursor);
	TV_DisplayOnePage(SKIP_OSD_REFRESH);

	for(int loop=0; loop <TV_MAX_ROWS; loop++ )
		{
		// EOF
		if( pTV->LineCounter >= pTV->TotalLines || (pTV->CurrentBlock_Cursor + pTV->CurrentPage_Cursor + pTV->CurrentPage_RealSize) >= pTV->TotalFileSize )
			break;

		pTV->AbsoluteCursor = pTV->LineIndex[ ++(pTV->LineCounter) ];

		if( CalcBufferPosition(+1) == CHK_BP_RETURN_NEW_BLOCK_NEEDS) 
			{
			TV_BlockRead(pTV->CurrentBlock_Cursor);
			TV_DisplayOnePage(SKIP_OSD_REFRESH);
			}
		else
			{
			TV_DisplayOnePage(SKIP_OSD_REFRESH);
			}
		}

	pMpegDecoder->RefreshOsd();

	return TEXTVIEWER_RETURN_NORMAL;
}


int TV_BookmarkDbxMsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	TMsgEntry tMsg;
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
			case TAVI_KEY_JOG1_UP:
			case TAVI_KEY_JOG1_LUP:
				pBookmarkDbx->CursorUp();
				pBookmarkDbx->ReDraw();
				break;
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LUP:
				SetTaviKeyMode(TK_REPEAT|TK_VARIOUS);
				for(;;)
					{


					usleep(500);
					if( MSG_Peek( pTV->TaskId, &tMsg ) == 0)
						{
						if( TV_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) < 0)
							break;
						}
					else
						{
						pBookmarkDbx->CursorUp();
						pBookmarkDbx->ReDraw();
						}
					}
				SetTaviKeyMode(TK_RESUME);
				break;		
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_DOWN:
			case TAVI_KEY_JOG1_DOWN:
			case TAVI_KEY_JOG1_LDOWN:
				pBookmarkDbx->CursorDown();
				pBookmarkDbx->ReDraw();
				break;
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LDOWN:
				SetTaviKeyMode(TK_REPEAT|TK_VARIOUS);
				for(;;)
					{
					usleep(500);
					if( MSG_Peek( pTV->TaskId, &tMsg ) == 0)
						{
						if( TV_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) < 0)
							break;
						}					
					else
						{
						pBookmarkDbx->CursorDown();
						pBookmarkDbx->ReDraw();
						}
					}
				SetTaviKeyMode(TK_RESUME);
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
						return -1;
						}
					}
				else
					{
					return -1;
					}
				break;
			case TAVI_KEY_EQ:
				break;
			case TAVI_KEY_MENU:
				break;
			case TAVI_KEY_LEFT:
				if( ((pBookmarkDbx->GetCurrentParameters())&_BOOKMARK_DBX_AT_PLAYING) == _BOOKMARK_DBX_AT_PLAYING )
					{
					TV_DeleteAnBookmark( pLFBrowser->CurrentFullPathDir,  pLFBrowser->Filename, pBookmarkDbx->GetCurrentChoice() );
					pBookmarkDbx->ReDraw();
					break;
 					}

			case TAVI_KEY_MODE:
			case TAVI_KEY_LMODE:
			case TAVI_KEY_EXIT:
				pBookmarkDbx->SetChoice(ssParam1*(-1));
				return -1;
				break;
			case TAVI_KEY_SHUTDOWN:
				if( pTAVI->isVideoPlaying )
					TV_CommonKeyProcess_EXIT(ssTaskId);
				return -1;
				break;
			}
		}

	return 0;
}

static int TV_PrepareAndPlay( uchar_t filename[] )
{
	int choice=-1;
	TMsgEntry tMsg;
	TTaskInitParam tParam;
	tParam.ssTaskId = 2;
	RMuint32 ret;

	tTEXTVIEWER_BOOKMARKDATA RESUME;
	tTEXTVIEWER_BOOKMARKDATA BOOKMARK;

	pTV->CurrentBlock_Cursor = 0;
	pTV->CurrentPage_Cursor = 0;
	pTV->AbsoluteCursor = 0;
	pTV->LineCounter = 1;		// Default Value is 1, Not 0.

	uchar_t FileExt[4];
	
	ClearKeyBuffer();

	ustrcpy( Fullpath, pLFBrowser->CurrentFullPathDir );
	ustrcat( Fullpath, sSlash );
	ustrcat( Fullpath, filename );
	TV_CheckLastResumeFile( pLFBrowser->CurrentFullPathDir,  filename, &RESUME );				

	if( RESUME.AbsoluteCursor > 10 ) {
		while(1) {
			////////////////////////////////////////////////////////////////////////////////////
			// Startup
			pTV->LastView = _TVLS_STARTUP_DBX;
			TV_DrawTopItems(SKIP_OSD_REFRESH);
			pBookmarkDbx = new CBookmarkDialogBox( ML_START_POINT, 10-1, 35-1, 320-(10*2)-1, 240-35-10-1, TV_BookmarkDbxMsgProc, 0, ID_SELECT|ID_CANCEL|_BOOKMARK_DBX_AT_BEGINNIG|ID_TEXTVIEWER_BOOKMARK ); 
			TV_BookmarkLoading( pLFBrowser->CurrentFullPathDir, filename );
			pBookmarkDbx->Display();
			usleep(5000);
			mep_eat_event();
			ClearKeyBuffer();
			usleep(5000);
			TV_DbxMsgPeek();

			BOOKMARK.AbsoluteCursor = pBookmarkDbx->GetSeekPoint();
			BOOKMARK.LineCounter = pBookmarkDbx->GetLineCounter();
			choice = pBookmarkDbx->GetCurrentChoice();

			delete pBookmarkDbx;
			pBookmarkDbx = NULL;
			ClearDbxOsd();
			pMpegDecoder->RefreshOsd();


			if( choice == TAVI_KEY_EXIT_REVERSE ||
				choice == TAVI_KEY_MODE_REVERSE ||
				choice == TAVI_KEY_LMODE_REVERSE )
				return choice;
			
			else if( RESUME.AbsoluteCursor > 0 || BOOKMARK.AbsoluteCursor > 0 ) 
				break;

			}

		switch( choice )
			{

			//Resume
			case -2:
				if( RESUME.AbsoluteCursor > 0 ) 
					{
					pTV->AbsoluteCursor  = (RMuint32)RESUME.AbsoluteCursor;
					pTV->LineCounter = (RMuint32)RESUME.LineCounter;
					CalcBufferPosition(+1);
					}
				break;

			// From The Beggining
			case -1:
				pTV->CurrentBlock_Cursor = 0;
				pTV->CurrentPage_Cursor = 0;
				pTV->AbsoluteCursor = 0;
				pTV->LineCounter = 1;
				break;

			// Bookmarks
			default:
				if( BOOKMARK.AbsoluteCursor > 0 ) 
					{
					pTV->AbsoluteCursor  =  (RMuint32)BOOKMARK.AbsoluteCursor;
					pTV->LineCounter  =  (RMuint32)BOOKMARK.LineCounter;
					CalcBufferPosition(+1);
					}
				else
					{
					pTV->AbsoluteCursor = 0;
					pTV->LineCounter = 1;
					}
				break;
		}
	}
		
	ClearKeyBuffer();
	return 0;
	
}	

int _FileOpenProcess( uchar_t* filename )
{
	int ret;
	pLFBrowser->Hide();

	ret = TV_PrepareAndPlay( filename );

	if( ret == TAVI_KEY_EXIT_REVERSE ) {
		pTV->LastView = _TVLS_FILE_BROWSER;
		pLFBrowser->Show();
	}
	else if( ret == TAVI_KEY_MODE_REVERSE ||
		     ret == TAVI_KEY_LMODE_REVERSE ) {
		return TEXTVIEWER_RETURN_EXIT;
	}
	else {
		if(!TV_StartView( filename ) ) {
			pTV->LastView = _TVLS_FILE_BROWSER;
			pLFBrowser->Show();
		}
		else {
			TV_RestartScrollWithLastSpeed(); 
		}
	}

	return TEXTVIEWER_RETURN_NORMAL;
}
				
int TV_MsgProc_FileBrowser( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	switch( pLFBrowser->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2 ) )
		{
		case FILEBROWSER_RETURN_EXIT: 
			pTV->TaskExitStatus = TaskExit_Normal;
			return TEXTVIEWER_RETURN_EXIT;
			break;

		case FILEBROWSER_RETURN_FILE_SELECTED:
			if( ustrlen(pLFBrowser->Filename ) > 1 ) {
				return _FileOpenProcess( pLFBrowser->Filename );
			}
			else {
				return TEXTVIEWER_RETURN_NORMAL;
			}

		default:
		case FILEBROWSER_RETURN_NORMAL: 
			return TEXTVIEWER_RETURN_NORMAL;
		}
}
int TV_MsgProc_App( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{

	if( ulMsgId == MSG_TOUCHPAD )
		{
		if( ssParam1 == 1 )
			TV_CommonKeyProcess_DOWN2(ssTaskId);
		else if( ssParam1 == -1 )
			TV_CommonKeyProcess_UP2(ssTaskId);
		}
	
	else if( ulMsgId == MSG_BUTTON_UP )
		{
		switch( ssParam1 )
			{
			case TAVI_KEY_EQ: 		break;
			case TAVI_KEY_MENU: 		if( !pmView->IsVisible() ) pmView->Show(); break;
			case TAVI_KEY_ENTER: 	break;
			case TAVI_KEY_LENTER:	TV_Bookmarking( pLFBrowser->CurrentFullPathDir,  pLFBrowser->Filename, pTV->AbsoluteCursor, pTV->LineCounter, -1 ); break;
			case TAVI_KEY_UP:		TV_CommonKeyProcess_UP2(ssTaskId); break;
			case TAVI_KEY_DOWN:	TV_CommonKeyProcess_DOWN2(ssTaskId); break;
			case TAVI_KEY_LEFT:		TV_CommonKeyProcess_LEFT2(ssTaskId); break;
			case TAVI_KEY_RIGHT:	TV_CommonKeyProcess_RIGHT2(ssTaskId); break;
			case TAVI_KEY_LLEFT:		TV_CommonKeyProcess_LEFT2(ssTaskId); break;
			case TAVI_KEY_LRIGHT:	TV_CommonKeyProcess_RIGHT2(ssTaskId); break;
			case TAVI_KEY_EXIT: 		TV_CommonKeyProcess_EXIT(ssTaskId); pTV->LastView = _TVLS_FILE_BROWSER; pLFBrowser->Show(); return TEXTVIEWER_RETURN_STOP_VIEW; break;			
			}
		}
	
	return TEXTVIEWER_RETURN_NORMAL;
}

void _ClearAllTvTimer()
{
	ClearTimer( &TV_AutoScroll_RegUT1 );
	ClearTimer( &TV_AutoScroll_RegUT2 );
	ClearTimer( &TV_AutoScroll_RegUT3 );
}
int TV_SetAutoScrollOff( RMuint8* parameter )
{
	_ClearAllTvTimer();
	TaviSysParam.text.scroll_speed = 0;
	return TRUE;
}
int TV_SetAutoScrollSlow( RMuint8* parameter )
{
	_ClearAllTvTimer();
	TaviSysParam.text.scroll_speed = 1;
	SetTimer( &TV_AutoScroll_RegUT1 );
	return TRUE;
}
int TV_SetAutoScrollMid( RMuint8* parameter )
{
	_ClearAllTvTimer();
	TaviSysParam.text.scroll_speed = 2;
	SetTimer( &TV_AutoScroll_RegUT2 );
	return TRUE;
}
int TV_SetAutoScrollFast( RMuint8* parameter )
{
	_ClearAllTvTimer();
	TaviSysParam.text.scroll_speed = 3;
	SetTimer( &TV_AutoScroll_RegUT3 );
	return TRUE;
}
int TV_RestartScrollWithLastSpeed( void )
{
	switch ( TaviSysParam.text.scroll_speed ) {
		case 1:
			TV_SetAutoScrollSlow( NULL );
			break;
		case 2:
			TV_SetAutoScrollMid( NULL );
			break;
		case 3:
			TV_SetAutoScrollFast( NULL );
			break;
		default:
			TV_SetAutoScrollOff( NULL );
			break;
		}
}
int TV_UpdateAutoScrollValue( RMuint8* parameter )
{
	pmView->ChangeSubMenu( ML_TEXT_AUTOSCROLL, psAutoScroll, (int)TaviSysParam.text.scroll_speed, TV_UpdateAutoScrollValue, NULL );	
	return TRUE;
}


int TV_MsgProc_Timer( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( (pLFBrowser->isVisible()) && ssParam1==2 && ssParam2== 2 )
		return (pLFBrowser->ProcFBAutoScrollTimer( ssTaskId, ulMsgId, ssParam1, ssParam2 ));
	else
		{
		switch( ssParam2 )
			{
			case TIMER_ID_TV_AUTOSCROLL_SLOW:
				if( ssParam1 == TIMER_VALUE_TV_AUTOSCROLL_SLOW)
					{
					_ClearAllTvTimer();
					if( !(pBookmarkDbx != NULL ||
						(pmFileBrowser!=NULL && pmFileBrowser->IsVisible()) ||
						(pmView!=NULL && pmView->IsVisible() ) ||
						pLFBrowser->isVisible() ))
						{
						if( TV_CommonKeyProcess_DOWN2(ssTaskId)==TEXTVIEWER_RETURN_EOF)
							TV_SetAutoScrollOff(NULL);
						else
							SetTimer( &TV_AutoScroll_RegUT1 );
						}
					}
				break;

			case TIMER_ID_TV_AUTOSCROLL_MID:
				if( ssParam1 == TIMER_VALUE_TV_AUTOSCROLL_MID)
					{
					_ClearAllTvTimer();
					if( !(pBookmarkDbx != NULL ||
						(pmFileBrowser!=NULL && pmFileBrowser->IsVisible()) ||
						(pmView!=NULL && pmView->IsVisible() ) ||
						pLFBrowser->isVisible() ))
						{
						if( TV_CommonKeyProcess_DOWN2(ssTaskId)==TEXTVIEWER_RETURN_EOF)
							TV_SetAutoScrollOff(NULL);
						else
							SetTimer( &TV_AutoScroll_RegUT2 );
						}
					}
				break;

			case TIMER_ID_TV_AUTOSCROLL_FAST:
				if( ssParam1 == TIMER_VALUE_TV_AUTOSCROLL_FAST)
					{
					_ClearAllTvTimer();
					if( !(pBookmarkDbx != NULL ||
						(pmFileBrowser!=NULL && pmFileBrowser->IsVisible()) ||
						(pmView!=NULL && pmView->IsVisible() ) ||
						pLFBrowser->isVisible() ))
						{
						if( TV_CommonKeyProcess_DOWN2(ssTaskId)==TEXTVIEWER_RETURN_EOF)
							TV_SetAutoScrollOff(NULL);
						else
							{
							SetTimer( &TV_AutoScroll_RegUT3 );
							}
						}
					}
				break;
				
			}
		}


}
int TV_MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	switch( ulMsgId )
		{
		case MSG_CREATE: 	DEBUGMSG(1,("[1]VP_MsgProc() : MSG_CREATE()\n"));	break;
		case MSG_DRAW: 	DEBUGMSG(1,("[1]VP_MsgProc() : MSG_DRAW()\n"));	break;
		case MSG_TIMER:
			DEBUGMSG(1,("TV_MsgProc() : case MSG_TIMER\n"));
			TV_MsgProc_Timer( ssTaskId, ulMsgId, ssParam1, ssParam2 );
			break;
			
		case MSG_TOUCHPAD:
		case MSG_BUTTON_UP:
			DEBUGMSG(1,("TV_MsgProc() : case MSG_BUTTON_UP|MSG_TOUCHPAD\n"));
			switch( ssParam1 )
				{
				case TAVI_KEY_MODE:	
					DEBUGMSG(1,("TV_MsgProc() : case MSG_BUTTON_UP : case TAVI_KEY_MODE\n"));
					pTV->TaskExitStatus = TaskExit_Background;
					if( pBookmarkDbx != NULL ) pBookmarkDbx->SetChoice(TAVI_KEY_MODE_REVERSE);
					return TEXTVIEWER_RETURN_EXIT;
					break;
				case TAVI_KEY_LMODE:	
					DEBUGMSG(1,("TV_MsgProc() : case MSG_BUTTON_UP : case TAVI_KEY_LMODE\n"));
					pTV->TaskExitStatus = TaskExit_Switch;
					if( pBookmarkDbx != NULL ) pBookmarkDbx->SetChoice(TAVI_KEY_LMODE_REVERSE);
					return TEXTVIEWER_RETURN_EXIT;
					break;
				case TAVI_KEY_JOG1_UP:
				case TAVI_KEY_JOG1_LUP:
					TAVI_BG_Volume(1);
					break;
				case TAVI_KEY_JOG1_DOWN:
				case TAVI_KEY_JOG1_LDOWN:
					TAVI_BG_Volume(0);
					break;
				case TAVI_KEY_JOG2_UP:
				case TAVI_KEY_JOG2_LUP:
					TAVI_BG_Next();
					break;
				case TAVI_KEY_JOG2_DOWN:
				case TAVI_KEY_JOG2_LDOWN:
					TAVI_BG_Prev();
					break;
				case TAVI_KEY_JOG2_ENTER:
					TAVI_BG_PlayPause();
					break;
				case TAVI_KEY_SHUTDOWN:
				case TAVI_KEY_LEXIT:
					pTV->TaskExitStatus = TaskExit_ExitAndSwitch;			
					TV_CommonKeyProcess_EXIT(ssTaskId);
					return TEXTVIEWER_RETURN_EXIT;
					break;
				}
		
			
			if( pBookmarkDbx != NULL )
				{
				return (TV_BookmarkDbxMsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
				}
			
			else if( pmFileBrowser!=NULL && pmFileBrowser->IsVisible() )
				{
				return ( pmFileBrowser->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
				}

			else if( pmView!=NULL && pmView->IsVisible() )
				{
				return( pmView->MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
				}

			else if( pLFBrowser->isVisible() )
				{
				return (TV_MsgProc_FileBrowser( ssTaskId, ulMsgId, ssParam1, ssParam2 ) );
				}

			else
				{
				return (TV_MsgProc_App( ssTaskId, ulMsgId, ssParam1, ssParam2 ) );
				}

			break;
		}

	return TEXTVIEWER_RETURN_NORMAL;
}


// TVSMC; TextViewer Sub Menu Command
int TVSMC_FileBrowserNameAscentSort( RMuint8* parameter )
{
//	DEBUGMSG(2,("FMSMC_FileBrowserNameAscentSort( option=%x )\n", parameter ));
	pLFBrowser->RefreshForSortingMethodChange( fpNameAscentSort, OSD_REFRESH);
//	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}
int TVSMC_FileBrowserNameDescentSort( RMuint8* parameter )
{
//	DEBUGMSG(2,("FMSMC_FileBrowserNameDescentSort( option=%x )\n", parameter ));
	pLFBrowser->RefreshForSortingMethodChange( fpNameDescentSort, OSD_REFRESH );
//	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}
int TVSMC_FileBrowserTimeAscentSort( RMuint8* parameter )
{
//	DEBUGMSG(2,("FMSMC_FileBrowserTimeAscentSort( option=%x )\n", parameter ));
	pLFBrowser->RefreshForSortingMethodChange( fpTimeAscentSort, OSD_REFRESH );
//	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}
int TVSMC_FileBrowserTimeDescentSort( RMuint8* parameter )
{
//	DEBUGMSG(2,("FMSMC_FileBrowserTimeDescentSort( option=%x )\n", parameter ));
	pLFBrowser->RefreshForSortingMethodChange( fpTimeDescentSort, OSD_REFRESH );
//	FM_DrawBottomItems(OSD_REFRESH);
	Reset_FileBrowserTimer();
	ClearKeyBuffer();
	return TRUE;
}

int TV_HideSubmenu( RMuint8* parameter )
{
	pmView->Hide(0, OSD_REFRESH);
	return TRUE;
}

int TV_Add_Bookmark( RMuint8* parameter )
{
	TV_Bookmarking( pLFBrowser->CurrentFullPathDir,  pLFBrowser->Filename, pTV->AbsoluteCursor, pTV->LineCounter, -1 );
	TV_HideSubmenu(0);
	return TRUE;
}

int TV_Show_BookmarkDbx( RMuint8* parameter )
{
	RMint64 t;
	int choice;
	DEBUGMSG(1,("+TV_Show_BookmarkDbx()\n"));

	pTV->LastView = _TVLS_BOOKMARK_DBX;

	// Bookmark
	SetTaviTouchpadMode( MEP_REPORT_RELATIVE );
	mep_eat_event();

	pMpegDecoder->RenderText(0, 0, 0, 24, 320, (240-24) ); 
	pBookmarkDbx = new CBookmarkDialogBox( ML_BOOKMARK, 10-1, 35-1, 320-(10*2)-1, 240-35-10-1, TV_BookmarkDbxMsgProc, pTV->AbsoluteCursor, ID_SELECT|ID_DELETE|ID_CANCEL|_BOOKMARK_DBX_AT_PLAYING|ID_TEXTVIEWER_BOOKMARK); 
	TV_BookmarkLoading( pLFBrowser->CurrentFullPathDir, pLFBrowser->Filename );
	pBookmarkDbx->Display();

	TV_DbxMsgPeek();

	t = pBookmarkDbx->GetSecondsToSave();

	if( pBookmarkDbx->GetCurrentChoice() == -1  &&  t > 0 )
		{
		char temp[256];
		TV_Bookmarking( pLFBrowser->CurrentFullPathDir,  pLFBrowser->Filename, t, pTV->LineCounter, -1 );
		}

	else if( pBookmarkDbx->GetCurrentChoice() >= 0 )
		{
		pTV->AbsoluteCursor = pBookmarkDbx->GetSeekPoint();
		pTV->LineCounter = pBookmarkDbx->GetLineCounter();
		CalcBufferPosition(+1);
		}

	choice = pBookmarkDbx->GetCurrentChoice();

	delete pBookmarkDbx;
	pBookmarkDbx = NULL;
	ClearDbxOsd();
	TV_HideSubmenu(0);	


	if( //choice == TAVI_KEY_EXIT_REVERSE ||
		choice == TAVI_KEY_MODE_REVERSE ||
		choice == TAVI_KEY_LMODE_REVERSE )
		{
		pTV->LastView = _TVLS_BOOKMARK_DBX;
		pTV->IsEmergencyExitFlagEnabled = TRUE;
		return choice;
		}

	
	TV_DisplayOnePage(OSD_REFRESH);
	DEBUGMSG(1,("-VP_Show_BookmarkDbx()\n"));
	return TRUE;
}

int TV_CreateMenuItems()
{
	// SubMenu psSort  
	psSort = new CPopUpSubMenu;
	psSort->AddSubMenuItem( ML_NAME_ASCENT, TRUE, TVSMC_FileBrowserNameAscentSort, NULL );
	psSort->AddSubMenuItem( ML_NAME_DESCENT, TRUE, TVSMC_FileBrowserNameDescentSort, NULL );
	psSort->AddSubMenuItem( ML_TIME_ASCENT, TRUE, TVSMC_FileBrowserTimeAscentSort, NULL );
	psSort->AddSubMenuItem( ML_TIME_DESCENT, TRUE, TVSMC_FileBrowserTimeDescentSort, NULL );

	pmFileBrowser = new CPopUpMenu;
	pmFileBrowser->AddSubMenu( ML_SORTING_METHOD, psSort, TaviSysParam.sort, NULL, NULL );


	/////////////////////////////////////////////////////////////////
	psBookmark = new CPopUpSubMenu;
	psBookmark->AddSubMenuItem( ML_VP_ADD_BOOKMARK, TV_Add_Bookmark, NULL );
	psBookmark->AddSubMenuItem( ML_VP_LOAD_BOOKMARK, TV_Show_BookmarkDbx, NULL );

	psMoveTo = new CPopUpSubMenu;
	psMoveTo->AddSubMenuItem( ML_TEXT_MOVE_TOP, TV_MoveToTop, NULL ); 
	psMoveTo->AddSubMenuItem( ML_TEXT_MOVE_BOTTOM, TV_MoveToBottom, NULL ); 

	psAutoScroll = new CPopUpSubMenu;
	psAutoScroll->AddSubMenuItem( ML_OFF, TRUE, TV_SetAutoScrollOff, NULL ); 
	psAutoScroll->AddSubMenuItem( ML_TEXT_AUTOSCROLL_SLOW, TRUE, TV_SetAutoScrollSlow, NULL ); 
	psAutoScroll->AddSubMenuItem( ML_TEXT_AUTOSCROLL_MID, TRUE, TV_SetAutoScrollMid, NULL ); 
	psAutoScroll->AddSubMenuItem( ML_TEXT_AUTOSCROLL_FAST, TRUE, TV_SetAutoScrollFast, NULL ); 
	
	pmView = new CPopUpMenu( 0, 30, 320, 170 );
	pmView->AddSubMenu( ML_BOOKMARK, psBookmark, -1, NULL, NULL );
	pmView->AddSubMenu( ML_MOVE, psMoveTo, -1, NULL, NULL );
	pmView->AddSubMenu( ML_TEXT_AUTOSCROLL, psAutoScroll, (int)TaviSysParam.text.scroll_speed, TV_UpdateAutoScrollValue, NULL );	
	pmView->AddSubMenu( ML_CLOSE, NULL, -1, TV_HideSubmenu, NULL );
	return TRUE;
}

int TV_DeleteMenuItems()
{
	if(pmView != NULL) { 
	    delete pmView; 
	    pmView = NULL; 
	}
	if(pmFileBrowser != NULL) { 
	    delete pmFileBrowser; 
	    pmFileBrowser = NULL; 
	}
	
	return TRUE;
}

int TV_Kill( void )
{
	/////////////////////////////////////////////////////////////
	// Exit TextViewer_Task

	TV_CommonKeyProcess_EXIT(TASK_ID_TEXT);
	
	if(pMbx) {
	    delete pMbx;
	    pMbx = NULL;
	}
	if(pTV) {
	    delete pTV;
	    pTV = NULL;
	}

	TV_DeleteMenuItems();
	TAVI_CannotSleep(0);
}

bool TV_IsViewingOnForeground( void )
{
	if ( pTAVI->ssTaskId == TASK_ID_TEXT ) {
		if ( pTV->LastView == _TVLS_TEXT_VIEW )
			return TRUE;
	}
	
	return FALSE;	
}

int TextViewer_Task( TTaskInitParam* tParam )
{
	TMsgEntry tMsg;

	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(TRUE);

	if( tParam->resume == FALSE )
		{
		DEBUGMSG(1,("TextViewer_Task() ... New\n"));		
		pTV							= (tTEXTVIEWER_CONFIG*) new tTEXTVIEWER_CONFIG;
		memset( pTV, 0, sizeof(tTEXTVIEWER_CONFIG));
		pTV->Param					= tParam;
		pTV->TaskId					= tParam->ssTaskId;
		pTV->CurrentFullPathDir		= Fullpath;
		pTV->CurrentFullPathDir[0]		= '\0';
		pTV->TaskExitStatus			= TaskExit_Normal;
		pTV->LineIndex 				= NULL;
		pTV->IsEmergencyExitFlagEnabled  = FALSE;

		pLFBrowser->SetActiveMode( _FBMODE_TEXTVIEWER, 0, TRUE );
		ClearOsd();
		showbackground(1);
//		DrawBoxWithoutRefresh( BG_FILTER_COLOR_PALETTE, 0, 0, _SCREEN_WIDTH_, _SCREEN_HEIGHT_);

		g_TopPanel->SetTitle( ML_TEXT );
		g_TopPanel->SetLogo( CTopPanel::LOGO_ID_TEXT );
		g_TopPanel->SetVisible(true);		
		g_TopPanel->Update();

		pTV->LastView = _TVLS_FILE_BROWSER;
		pLFBrowser->Show();
	
		pMbx = new CMsgBox();

		TV_CreateMenuItems();

		TAVI_CanSleep(0);
		TV_SetAutoScrollOff(NULL);

		}
	else
		{
		DEBUGMSG(1,("TextViewer_Task() ... Resume\n"));

		pTV->IsEmergencyExitFlagEnabled  = FALSE;
		
		ClearOsd();
		showbackground(1);

		g_TopPanel->SetTitle( ML_TEXT );
		g_TopPanel->SetLogo( CTopPanel::LOGO_ID_TEXT );
		g_TopPanel->SetVisible(true);		
		g_TopPanel->Update();

		switch( pTV->LastView )
			{
			case _TVLS_FILE_BROWSER:
				pLFBrowser->Show();
				break;

			case _TVLS_TEXT_VIEW:
				TV_DisplayOnePage(OSD_REFRESH);				
				break;

			case _TVLS_STARTUP_DBX:
				_FileOpenProcess( pLFBrowser->Filename );
				break;

			case _TVLS_BOOKMARK_DBX:
				if( TV_Show_BookmarkDbx(NULL) != TRUE )
					goto EXIT_PART;
				break;

			default:
				pTV->LastView = _TVLS_FILE_BROWSER;
				pLFBrowser->Show();
			}

		}

	
	while( 1 ) {
		if( pTV->IsEmergencyExitFlagEnabled ) {
			break;
		}
		
		usleep(500);
		if( MSG_Peek( tParam->ssTaskId, &tMsg ) == 0) {
			usleep(500);
			if( TV_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) == TEXTVIEWER_RETURN_EXIT )
				break;				
		}
	}

EXIT_PART:
	
	TV_SetAutoScrollOff(NULL);
	pLFBrowser->SaveDirInfo();

	if( pmFileBrowser != NULL && 
		pmFileBrowser->IsVisible() ) {
		pmFileBrowser->Hide(0);
	}

	if( pmView != NULL && 
		pmView->IsVisible() ) {
		pmView->Hide(0);
	}

	if( pLFBrowser->isVisible() ) {
		pTV->LastView = _TVLS_FILE_BROWSER;
		pLFBrowser->Hide();
	}
 	else {
		ClearOsd();	
	}

	g_TopPanel->SetVisible(true);
	g_TopPanel->Update();

	pMpegDecoder->RefreshOsd();

	return (pTV->TaskExitStatus);
	
}
