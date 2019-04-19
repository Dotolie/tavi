/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * VP_FileBrowser.cpp
 * Sang-jin Ryu (ew@newmedialife.com)
 *
 */

#include <stdio.h>
#include <linuxfs.h>
//#include "VP_FileBrowsing.h"
#include <Task/Msg.h>
#include <bitmap.h>
#include <Lib/LibImage.h>
#include "VP_FileBrowsing.h"
//#include "VideoPlayer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

#include <unistd.h>
#include <dirent.h>

#include <stdio.h>	// need sprintf
#include <string.h>
#include <stdlib.h>
#include <memory.h>	// memmove/memcpy


#if 0
#ifdef _WIN32
#include <windows.h>
static void debug_outs (char *szMsg, ...)
{
	char szBuf[256];
	wvsprintf (szBuf, szMsg, (LPSTR)(&szMsg + 1));
	OutputDebugString (szBuf);
}
#else
#include <stdio.h>
#define debug_outs	printf
#endif
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

extern TTAVI_GLOBAL_CONFIG* pTAVI;
extern MpegDecoder *pMpegDecoder;
extern FileBrowser* pFileBrowser;
extern BITMAPFILEINFO256 TAVI_IMAGES[];

static int    ii=0;
static char * gfile_name;

//static BITMVPFILEINFO skin_bmp;
extern U16 DrawItemWithoutRefresh( BITMAPFILEINFO256* pBitmap, U16 usPictureID, U16 usX, U16 usY, U16 usItemWidth, U16 usItemHeight );


RMuint32 VPFB_getrootdir (RMint8 *dir, RMuint32 length, void *context)
{
	//  return linux_getrootdir (dir, length, context);

  	strcpy(dir, "/hd1/VIDEO");
	return 0;
}

RMuint32 VPFB_opendir (RMint8 *dir, void *context)
{
    return linux_opendir (dir, context);
}

RMuint32 VPFB_readdir (RMuint32 handle, DIR_ENTRY_INFO *pDirInfo, void *context)
{
	printf("VPFB_readdir()\n");
    return linux_readdir (handle, pDirInfo, context);
}

RMuint32 VPFB_closedir (RMuint32 handle, void *context)
{
    return linux_closedir (handle, context);
}

RMuint32 VPFB_filenameoffset()
{
    return linux_filenameoffset();
}

RMuint32 VPFB_rendererase(void * context)
{
    //printf("rendererase --------\n");
//    pMpegDecoder->RenderText(0,0,0,0,320,240);
	pMpegDecoder->RenderText(0,0,0,130,320,195);
    return 0;
}

//RMuint32 VPFB_rendericon(FILEBROWSER_ICON_TYPE type , RMint32 x,RMint32 y,RMint32 w,RMint32 h,RMuint32 sel,void * context)
RMuint32 VPFB_rendericon( U16 usPictureID, U16 usX, U16 usY, U16 usItemWidth, U16 usItemHeight )
{
    //printf("rendericon --------\n");
    DrawItemWithoutRefresh( &TAVI_IMAGES[TYPE_ICON_LIST], usPictureID, usX, usY, usItemWidth, usItemHeight );
    return 0;
}

RMuint32 VPFB_rendertext(RMint8 * text,RMint32 x,RMint32 y,RMint32 w,RMint32 h,
                                                                   RMuint32 color,void * context)
{
	pMpegDecoder->RenderText(text,color,x,y,w,h);
    return 0;
}

RMuint32 VPFB_rendertextwithoutrefresh(RMint8 *text, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMuint32 colour, void* context)
{
	pMpegDecoder->RenderText_WithoutRefresh( text, colour, x, y, w, h);
	return 0;
}

RMuint32 VPFB_clearosdwithoutrefresh( int x, int y, int w, int h )
{
	pMpegDecoder->ClearAreaOSD_WithoutRefresh(x,y,w,h);
	return 0;
}


#include <linux/reboot.h>
extern "C" int reboot (int);

/*
extern RMuint32 fb_upgrade (RMint8 *filename, void *context);
*/

RMuint32 VPFB_renderfile(RMint8 * filename , void * context)
{
    //printf("renderfile --------\n");
    unsigned int	ui_len = strlen(filename);
    strcpy(gfile_name,filename);
    ii=1;
    
    //Firmware Upgrade File Check
//	 	fb_upgrade(filename, context);

    return 0;
}

RMuint32 VPFB_isrenderdone(void * context)
{
    //printf("isrenderdone --------\n");
    return 0;
}

RMuint32 VPFB_renderstop(void * context)
{
    printf("renderstop --------\n");
    return 0;
}

RMuint32 VPFB_info(RMint32 msg , void * info, void * context)
{
    printf("info --------\n");
    return 0;
}

RMuint32 VPFB_showsettings(void * context)
{
    printf("showsettings --------\n");
    return 0;
}



RMuint32 VPFB_osdrefresh()
{
   printf("VPFB_osdrefresh --------\n");
 
	pMpegDecoder->RefreshOsd();
	return 0;
}

/*
RMuint16 VPFB_ScanDir(RMint8 *dir, DIR_ENTRY_INFO* entry, void* context);	
{
	int number = -1;
	struct dirent** aNameList;
	struct stat
	

	number = scandir( dir, aNameList


	
 
	if (number < 0)
	{
		perror("Failed to scandir() in RefreshList ");
	}
	else
	{
		for( i=0, j=0; j<number; j++ )
		{	

			if( aNameList[j]->d_name[0] == '.' )
				continue;

			sprintf( sdir, "%s/%s", m_currentdir, aNameList[j]->d_name );
			
			DEBUGMSG(1,( "sdir : [%s]\n", sdir  ));

//			if( lstat( aNameList[j]->d_name, &statbuf ) < 0 ) 
			if( lstat( sdir, &statbuf ) < 0 ) 
			{
				DEBUGMSG(1,( "failed to lstat( %s )\n", aNameList[i]->d_name));
				continue;
			}

			if( S_ISREG(statbuf.st_mode) ) 
				filetype = DIR_ENTRY_INFO_IS_FILE;
			else if( S_ISDIR(statbuf.st_mode) ) 
				filetype = DIR_ENTRY_INFO_IS_DIRECTORY;
			else 
				continue;

			m_DirectoryEntries[i].flags		= filetype;
//			m_DirectoryEntries[i].handle	=
//			m_DirectoryEntries[i].name 	= 	aNameList[j]->d_name;
			sprintf( m_DirectoryEntries[i].name, "%s", aNameList[j]->d_name );
			free(aNameList[j]);
//			m_DirectoryEntries[i].namelength
			m_DirectoryEntries[i].parentdir	= m_currentdir;

			i++;

		}
		free( aNameList );
		m_TotalItemCount 	= i;
		printf("\n------------------------------------------\n");
		printf("m_TotalItemCount=[%d]  i=[%d]\n", m_TotalItemCount, i );
		printf("------------------------------------------\n");
		
	}
}
*/



int VP_FileBrowsing(int ssTaskId , char * file_name, unsigned int name_len, int opt )
{
	TMsgEntry tMsg;

	FILEBROWSER_CALLBACK_TABLE 	cbtFileBrowser;  

	DEBUGMSG(1, ("[2]VP_FileBrowsing()\n"));
//printf("------------------------------------------------------(1)\n");

//	cbtFileBrowser.ssTaskId	 		= ssTaskId;
	cbtFileBrowser.context     			= pFileBrowser;
	cbtFileBrowser.getrootdir  			= VPFB_getrootdir;
	cbtFileBrowser.opendir     			= VPFB_opendir;
	cbtFileBrowser.readdir     			= VPFB_readdir;
	cbtFileBrowser.closedir    			= VPFB_closedir;
	cbtFileBrowser.rendererase  			= VPFB_rendererase;
	cbtFileBrowser.rendericon   			= VPFB_rendericon;
	//cbtFileBrowser.rendericon   		=fb_rendericon;

	cbtFileBrowser.rendertext   			= VPFB_rendertext;
	cbtFileBrowser.renderfile   			= VPFB_renderfile;
	cbtFileBrowser.isrenderdone 			= VPFB_isrenderdone;
	cbtFileBrowser.renderstop   			= VPFB_renderstop;
	cbtFileBrowser.info         			= VPFB_info;
	//   cbtFileBrowser.showsettings 		= VPFB_showsettings;

	cbtFileBrowser.rendertextwithoutrefresh = VPFB_rendertextwithoutrefresh;
	cbtFileBrowser.osdrefresh			= VPFB_osdrefresh;
	cbtFileBrowser.DrawASCIITextWithoutRefresh =	DrawASCIITextWithoutRefresh;
	cbtFileBrowser.DrawBoxWithoutRefresh	= DrawBoxWithoutRefresh;
	cbtFileBrowser.DrawItemWithoutRefresh	= DrawItemWithoutRefresh;
	cbtFileBrowser.DrawItemWithoutRefreshSkipTransparency= DrawItemWithoutRefreshSkipTransparency;
	
//printf("------------------------------------------------------(2)\n");
	if( opt != 1)
		pFileBrowser->m_currentdir[0] = '\0';
	
	pFileBrowser->ReuseFileBrowser();
	pFileBrowser->InitCallbackTable(&cbtFileBrowser);
	pFileBrowser->m_filenameoffset		= VPFB_filenameoffset();
	pFileBrowser->m_HorizontalOverscan 	= 20;
	pFileBrowser->m_VerticalOverscan	= 32;
	pFileBrowser->SetTextHeight(16);
	pFileBrowser->SetDisplayDimensions(320,240);


	
	//pFileBrowser->SetDisplayDimensions(320,194-30);
	ii=0;
	gfile_name=file_name;

	tMsg.ulMsgId 		= 0;
	tMsg.ssToTaskId 	= -1;
	tMsg.usParam1 		= 0;
	tMsg.usParam2 		= 0;	

	for(;;)
	{
		DEBUGMSG(1,("VP_FileBrowsing() ... for(;;) ...\n"));
		pFileBrowser->Schedule(); 
		if(ii != 0) break;

		if( MSG_Peek( ssTaskId, &tMsg) == 0)  // get message OK
		{
			DEBUGMSG(1, ("[2]VP_FileBrowsing() :tMsg.ulMsgId=[0x%x] tMsg.usParam1=[0x%x]\n", tMsg.ulMsgId, tMsg.usParam1));
		
			switch(tMsg.ulMsgId )
			{
			case MSG_BATTERY:
				if(IsBatteryLevelChanged(tMsg.usParam1))
					DrawItem( &TAVI_IMAGES[ICON_TOP_BATTERY], SetBatteryLevel(tMsg.usParam1), 291, 4, 24, 13 ); 
				
				break;

			case MSG_BUTTON_UP:

				DEBUGMSG(1, ("[2]VP_FileBrowsing() : MSG_BUTTON_UP : tMsg.ulMsgId=[0x%x] tMsg.usParam1=[0x%x]\n", tMsg.ulMsgId, tMsg.usParam1));
				
				switch(tMsg.usParam1)
				{
				case TAVI_KEY_EXIT: 
					DEBUGMSG(1, ("[2]VP_FileBrowsing() : TAVI_KEY_EXIT\n"));
					if( pFileBrowser->ChangeToParentDir() == -1 )
						{
						pFileBrowser->Stop(); 
						tMsg.ulMsgId = 0;
						tMsg.ssToTaskId = -1;
						tMsg.usParam1 = 0;
						tMsg.usParam2 = 0;
		
						return -1;
						}
					break;
					
				case TAVI_KEY_RIGHT: 
					DEBUGMSG(1, ("[2]VP_FileBrowsing() : TAVI_KEY_RIGHT\n"));
				
					
					break;
					
				case TAVI_KEY_UP: 
					DEBUGMSG(1, ("[2]VP_FileBrowsing() : TAVI_KEY_UP\n"));
					pFileBrowser->Up(); 
					break;
					
				case TAVI_KEY_DOWN:
					DEBUGMSG(1, ("[2]VP_FileBrowsing() : TAVI_KEY_DOWN\n"));
					pFileBrowser->Down(); 
					break;
					
				case TAVI_KEY_LEFT: 
					DEBUGMSG(1, ("[2]VP_FileBrowsing() : TAVI_KEY_LEFT\n"));
					break;
					
				case TAVI_KEY_ENTER: 
					DEBUGMSG(1, ("[2]VP_FileBrowsing() : TAVI_KEY_ENTER\n"));
					if( pFileBrowser->Select() == FILEBROWSER_ERROR_NO_MORE_ENTRIES )
					{
						if( pFileBrowser->ChangeToParentDir() == -1 )
						{
						pFileBrowser->Stop(); 
						tMsg.ulMsgId = 0;
						tMsg.ssToTaskId = -1;
						tMsg.usParam1 = 0;
						tMsg.usParam2 = 0;						
						return -1;
						}
					}
					break;
				}

				break;
	       		}
		}
	}

	DEBUGMSG(1, ("[2]VP_FileBrowsing() : out of for(;;)\n"));
	pFileBrowser->ReCloseFileBrowser();

	return 0;
}



