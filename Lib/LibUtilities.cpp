//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibUtilities Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibUtilities.cpp,v $
// $Revision: 1.4 $
// $Date: 2006/06/07 08:16:57 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibUtilities.cpp,v $
// Revision 1.4  2006/06/07 08:16:57  zstein
// - removed unused codes.
//
// Revision 1.3  2006/04/12 04:29:28  ywkim
// Add procedure for extention of smil
//
// Revision 1.2  2006/04/07 08:15:57  ywkim
// Change many function for Host
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.72  2006/01/11 09:29:54  etnlwind
// Update for TextLib
//
// Revision 1.71  2005/12/30 04:24:42  etnlwind
// Update for New Font Adatio
//
// Revision 1.70  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.69  2005/12/09 08:18:27  ywkim
// Exchange order of display of ucTimeDigitTxt & AmPmTxt in SetTimeStr
//
// Revision 1.68  2005/11/30 11:27:14  etnlwind
// Update for Alarm
//
// Revision 1.67  2005/11/30 10:05:13  etnlwind
// Update for Savefile Size
//
// Revision 1.66  2005/11/25 05:10:50  etnlwind
// Update for New EQ
//
// Revision 1.65  2005/11/24 10:56:46  etnlwind
// Update for New EQ
//
// Revision 1.64  2005/11/23 09:33:53  etnlwind
// Update for TAVI-EQ
//
// Revision 1.63  2005/11/22 04:55:07  etnlwind
// Update for SoundPalette New UI
//
// Revision 1.62  2005/11/07 10:26:42  etnlwind
// 텍스트뷰어 기능추가
// -한페이지에 13라인 출력하게끔 수정 (기존:16라인)
// -페이지 이동 처음/끝 추가
// -자동 라인 스크롤 추가 (slow=9초, mid=6초, fast=3초 임의지정 )
//
// Revision 1.61  2005/10/14 11:39:38  etnlwind
// Update for TextViwer
//
// Revision 1.60  2005/10/12 09:24:06  etnlwind
// 1. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.59  2005/10/07 10:12:57  etnlwind
// Update for TextViewer
//
// Revision 1.58  2005/10/05 09:43:19  etnlwind
// Update for
// 1. FileManager Copy/Move/Delete Related
// 2. VideoPlayer Bookmarking/Bookmark Loading/Resume Related
//
// Revision 1.57  2005/09/29 10:56:58  etnlwind
// Update for FileManager
//
// Revision 1.56  2005/09/21 05:24:35  etnlwind
// Update for VideoPlayer / FileManager Shutdown
//
// Revision 1.55  2005/09/20 12:47:20  etnlwind
// Update for VideoPlayer
//
// Revision 1.54  2005/09/16 01:11:18  etnlwind
// Update for FileManager USB Host Related
//
// Revision 1.53  2005/09/08 04:32:25  etnlwind
// Update for MPEG4 Simple Profile (mp4v) Playback
//
// Revision 1.52  2005/08/31 13:38:56  etnlwind
// Update for Library
//
// Revision 1.51  2005/08/30 04:31:01  etnlwind
// Upate for VideoPlayer / FileManager
//
// Revision 1.50  2005/08/29 07:26:27  etnlwind
// Upate for FileManager BugFIx
//
// Revision 1.49  2005/08/24 13:07:55  etnlwind
// Update for Virtual Keyboard
//
// Revision 1.48  2005/08/23 13:54:32  etnlwind
// Update for LibUtilities
//
// Revision 1.47  2005/08/23 07:12:23  etnlwind
// Update for FileManager
//
// Revision 1.46  2005/08/19 05:45:41  etnlwind
// Update for VideoPlayer EQ BugFix
//
// Revision 1.45  2005/08/17 08:44:18  etnlwind
// Update for SoundPalette Affect
//
// Revision 1.44  2005/08/17 08:16:49  etnlwind
// Update for Sound Palette
//
// Revision 1.43  2005/08/17 03:48:02  etnlwind
// Update for Sound Palette Related Func.
//
// Revision 1.42  2005/08/14 09:38:33  etnlwind
// Update for
// 1. VideoPlayer Bookmark Delete
// 2. VideoPlayer FileBrowser Subtitle Ready Video File Icon Specificationized
// 3. VideoPlayer UI minor updates
//
// Revision 1.41  2005/07/12 12:45:35  etnlwind
// Update for LibFileDbx
//
// Revision 1.40  2005/07/07 14:33:22  etnlwind
// Update for Subtitle Bug Fix
//
// Revision 1.39  2005/06/30 13:12:33  ywkim
// Remark mp4 play routine
//
// Revision 1.38  2005/06/30 07:01:35  etnlwind
// Update for *.mp4 file detection
//
// Revision 1.37  2005/06/22 08:46:44  etnlwind
// Update for DIV3 Seek And Playback
//
// Revision 1.36  2005/06/16 09:16:00  zstein
// - added ClearKeyBuffer().
//
// Revision 1.35  2005/06/16 08:43:27  etnlwind
// Update for Video Resolution Detect, Codec Type Check And Caption Data Realloc Process
//
// Revision 1.34  2005/06/15 12:41:34  etnlwind
// Update for Video Touch Seek Process
//
// Revision 1.33  2005/06/15 11:51:30  etnlwind
// Update for Video Player Touch Control And Background Savefile Process
//
// Revision 1.32  2005/06/13 08:48:13  etnlwind
// Update for Savetime Saving
//
// Revision 1.31  2005/06/13 03:45:56  etnlwind
// Update for MPEG video ration detection related codes
//
// Revision 1.30  2005/05/27 13:15:18  etnlwind
// Update for Video Subtitle
//
// Revision 1.29  2005/05/27 11:59:06  etnlwind
// Update for Video Subtitle
//
// Revision 1.28  2005/05/18 17:54:47  etnlwind
// Update for Video Player Timer Related Fix
//
// Revision 1.27  2005/05/18 14:48:38  etnlwind
// Update for Timer Related Problems
//
// Revision 1.26  2005/05/15 16:27:52  etnlwind
// Update for Battety Msg Skip on VideoPlayer
//
// Revision 1.25  2005/05/15 11:44:40  etnlwind
// Update for ES4
//
// Revision 1.24  2005/05/14 06:10:05  etnlwind
// Update for ES4
//
// Revision 1.23  2005/05/09 03:07:17  etnlwind
// Update for ES4
//
// Revision 1.22  2005/05/06 03:30:24  etnlwind
// Update for ES4
//
// Revision 1.21  2005/05/04 06:39:38  etnlwind
// Update For Video Player Touch & Slide Button
//
// Revision 1.20  2005/04/29 14:32:44  etnlwind
// Update for ES4
//
// Revision 1.19  2005/04/26 10:06:56  etnlwind
// Upate for ES4 UI
//
// Revision 1.18  2005/04/22 14:37:35  etnlwind
// Library Update for UI changes
//
// Revision 1.17  2005/04/20 11:28:46  etnlwind
// update for ES4
//
// Revision 1.16  2005/04/18 14:30:43  etnlwind
// update for ES4
//
// Revision 1.15  2005/04/18 09:18:25  etnlwind
// TextEngine Add
// Update for ES4
//
// Revision 1.14  2005/03/06 12:36:38  etnlwind
// *** empty log message ***
//
// Revision 1.13  2005/03/06 11:06:07  etnlwind
// ...
//
// Revision 1.12  2005/03/06 08:39:22  etnlwind
// Volume, ControlPannel
//
// Revision 1.11  2005/03/05 15:51:12  etnlwind
// 볼륨수정
//
// Revision 1.10  2005/03/05 13:39:47  etnlwind
// Timer
//
// Revision 1.9  2005/03/05 13:38:16  etnlwind
// Timer
//
// Revision 1.8  2005/03/05 13:00:45  etnlwind
// Timer관련 App수정
//
// Revision 1.7  2005/03/05 12:02:11  etnlwind
// key related
//
// Revision 1.6  2005/03/04 07:42:24  etnlwind
// keystruct위치 변경 및 VP 수정
//
// Revision 1.5  2005/03/03 18:25:36  etnlwind
// Library 수정
//
// Revision 1.4  2005/03/03 09:11:49  etnlwind
// USB연결관련 코드 추가
//
// Revision 1.3  2005/03/03 04:58:13  etnlwind
// VideoPlayer, FileBrowser 부분 수정
//
// Revision 1.2  2005/03/02 11:58:41  etnlwind
// Msg 처리 추가
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library 정리
//
//
//*****************************************************************************

#include <util.h>
#include "LibUtilities.h"
#include "LibDefines.h"
#include "LibWideString.h"

extern CTextEngine_BDF* pT;

int GetNthString( char sTarget[], char sSource[], char cDivChar, int N )
{
    int iDivFound       = 0;
    int iStrLen         = strlen(sSource);
    int aDivIndex[32]   = {0};
    int iPrevDivIndex;

    for( int i=0; i<=iStrLen; i++ )
	{
	
        if( sSource[i] == cDivChar
            || sSource[i] == '\0' ) // i == iCurrentDivIndex
		{
			iDivFound++;
			aDivIndex[ iDivFound ] = i;
		}


        if( iDivFound == N )
		{
			iPrevDivIndex = aDivIndex[iDivFound-1];

//			printf("[%d %d %d]\n", iPrevDivIndex, i, iDivFound );   // 0, 7, 1

			if( iPrevDivIndex == 0)
				{
				strncpy( sTarget, sSource, i );
				sTarget[i] = '\0';
				}
			else
				{
				strncpy( sTarget, (sSource + iPrevDivIndex + 1), (i - iPrevDivIndex) );
				sTarget[i-iPrevDivIndex-1] = '\0';
				}

			if( sSource[i] == '\0' )
				return -1;
			else
				return (i+1);
			
			break;
		}
	else
		{
		if( sSource[i] == '\0' )
			return -1;
		
		}


	}

    return FALSE;
}


int GetCurrentDir_InFullDirString( char target[], char source[] )
{
	int 	szSrcDir;
	int	NewDirStrLength;
//	char 	target[1024];

	szSrcDir = strlen( source );

	DEBUGMSG(0, ("GetCurrentDir_InFullDirString()  : source:[%s]\n", source ));

	for( NewDirStrLength=szSrcDir;NewDirStrLength>=0; NewDirStrLength--)
	{
		if( source[NewDirStrLength] == '/' )
		{
			strcpy( target, source+NewDirStrLength+1 );
//			target[NewDirStrLength+1] = '\0';
			return TRUE;
			break;
		}
	}

	return FALSE;
}

int GetCurrentDir_InFullDirString_UC16( uchar_t target[], uchar_t source[] )
{
	int 	szSrcDir;
	int	NewDirStrLength;

	szSrcDir = ustrlen( source );

	for( NewDirStrLength=szSrcDir;NewDirStrLength>=0; NewDirStrLength--)
	{
		if( source[NewDirStrLength] == 0x002f )
		{
			ustrcpy( target, source+NewDirStrLength+1 );
//			target[NewDirStrLength+1] = '\0';
			return TRUE;
			break;
		}
	}

	return FALSE;
}


int GetLastItem_In_FullDirString( char target[], char source[] )
{
	int 	szSrcDir;
	int	NewDirStrLength;
//	char 	target[1024];

	szSrcDir = strlen( source );

	DEBUGMSG(0, ("GetCurrentDir_InFullDirString()  : source:[%s]\n", source ));

	for( NewDirStrLength=szSrcDir;NewDirStrLength>=0; NewDirStrLength--)
	{
		if( source[NewDirStrLength] == '/' )
		{
			strcpy( target, source+NewDirStrLength+1 );
//			target[NewDirStrLength+1] = '\0';
			return TRUE;
			break;
		}
	}

	return FALSE;
}

int GetLastItem_In_FullDirString_WithUserDefineSplitter( char target[], char source[], char splitter )
{
	int 	szSrcDir;
	int	NewDirStrLength;
//	char 	target[1024];

	szSrcDir = strlen( source );

	DEBUGMSG(0, ("GetCurrentDir_InFullDirString()  : source:[%s]\n", source ));

	for( NewDirStrLength=szSrcDir;NewDirStrLength>=0; NewDirStrLength--)
	{
		if( source[NewDirStrLength] == splitter )
		{
			strcpy( target, source+NewDirStrLength+1 );
//			target[NewDirStrLength+1] = '\0';
			return TRUE;
			break;
		}
	}

	return FALSE;
}


int GetLastItem_In_UC16FullDirString_WithUserDefineSplitter( uchar_t target[], uchar_t source[], uchar_t splitter )
{
	int 	szSrcDir;
	int	NewDirStrLength;
//	char 	target[1024];

	szSrcDir = ustrlen( source );

	DEBUGMSG(0, ("GetCurrentDir_InFullDirString()  : source:[%s]\n", source ));

	for( NewDirStrLength=szSrcDir;NewDirStrLength>=0; NewDirStrLength--)
	{
		if( source[NewDirStrLength] == splitter )
		{
			ustrcpy( target, source+NewDirStrLength+1 );
//			target[NewDirStrLength+1] = '\0';
			return TRUE;
			break;
		}
	}

	return FALSE;
}

int GetParentDir_In_UC16FullDirString( uchar_t target[], uchar_t source[] )
{
	int 	szSrcDir;
	int	NewDirStrLength;
	int 	UpperEdge;
//	char 	target[1024];

	szSrcDir = ustrlen( source );

	DEBUGMSG(0, ("GetCurrentDir_InFullDirString()  : source:[%s]\n", source ));

#if 1

	for( int i=szSrcDir; i>=0; i-- )
		{
		if( source[i] == 0x002f )
			{
			UpperEdge = i;
			break;
			}
		}

	ustrncpy( target, source, UpperEdge );
	target[UpperEdge] = 0x0000;

#else
	for( NewDirStrLength=szSrcDir;NewDirStrLength>=0; NewDirStrLength--)
	{
		if( source[NewDirStrLength] == 0x002f )
		{
			ustrcpy( target, source+NewDirStrLength+1 );
//			target[NewDirStrLength+1] = '\0';
			return TRUE;
			break;
		}
	}
#endif

	return FALSE;
}

int GetParentDir_In_UTF8FullDirString( char target[], char source[] )
{
	int 	szSrcDir;
	int	NewDirStrLength;
	int 	UpperEdge;
//	char 	target[1024];

	szSrcDir = strlen( source );

	DEBUGMSG(0, ("GetCurrentDir_InFullDirString()  : source:[%s]\n", source ));

	for( int i=szSrcDir; i>=0; i-- )
		{
		if( source[i] == 0x2f )
			{
			UpperEdge = i;
			break;
			}
		}

	strncpy( target, source, UpperEdge );
	target[UpperEdge] = 0x00;

	return FALSE;
}


int CaseNormalize( char Target[], char Source[] )
{
	int sz = strlen( Source );
	int i;

	memset( Target, 0, sizeof( Target ) );

	for( i=0; i < sz ; i++ )
		{
		if( Source[i] >= 'A' && Source[i] <= 'Z' )
			Target[i] = Source[i] + 32;
		else
			Target[i] = Source[i];
		}

//	Target[i+1] = '\0';
	
	return TRUE;
}

#define UC16_A 0x0041
#define UC16_Z 0x005A

int CaseNormalize_UC16( uchar_t Target[], uchar_t Source[] )
{
	int sz = ustrlen( Source );
	int i;

	memset( Target, 0, sizeof( Target ) );

	for( i=0; i < sz ; i++ )
		{
		if( Source[i] >= (uchar_t)UC16_A && Source[i] <= (uchar_t)UC16_Z )
			Target[i] = Source[i] + (uchar_t)32;
		else
			Target[i] = Source[i];
		}

//	Target[i+1] = '\0';
	
	return TRUE;
}


int SetBatteryLevel( U16 Parameter )
{

#if 0
	// Battery Level Request
	if( Parameter == 0xFFFF)
		return (pTAVI->BatteryLevel); 

	// Batter Level Set

	if( Parameter > 100)
		Parameter = 100;

	pTAVI->BatteryLevel = (Parameter-1)/20 + 1;
#else
	if( Parameter > 6 )
		Parameter = 1;

	pTAVI->BatteryLevel  = Parameter;

#endif	

	return (pTAVI->BatteryLevel);
}



/////////////////////////////////
// Battery Level Check
// return TRUE : BatteryLevel has changed
// return FALSE: BatteryLevel has not changed
int IsBatteryLevelChanged( U16 Parameter )
{
	static U16 PreBatteryLevel;

	#if 0
	if( Parameter > 100 )
		Parameter = 100;

	if( pTAVI->BatteryLevel == (Parameter-1)/20+1  )
		return  FALSE;
	else
		return TRUE;
	#endif

//	if( Parameter > 7 ) return FALSE;

	if( PreBatteryLevel == Parameter )
		{
		return FALSE;
		}
	else
		{
		PreBatteryLevel = Parameter;
		return TRUE;
		}

}


char* Convert_SecOnly2TimeString( char sTimeString[], int iSeconds )
{
	int hours, minutes, seconds;

	hours 	= iSeconds / 3600 ;
	minutes 	= (iSeconds - hours*3600) /60;
	seconds 	= iSeconds - hours*3600 - minutes*60;

	if( hours == 0 )
		sprintf( sTimeString, "%02d:%02d%c", minutes, seconds, '\0' );
	else
		sprintf( sTimeString, "%02d:%02d:%02d%c", hours,minutes, seconds, '\0' );

//	DEBUGMSG(1,("Convert_SecOnly2TimeString : iSeconds: [%d], sTimeString:[%s]\n", iSeconds, sTimeString));

	return (sTimeString);
}


//#define THIS_IS_CEBIT_VERSION
int CheckExtension( char Source[] )
{	
	char ext[32] = {0};
	char temp[32] = {0};

	GetLastItem_In_FullDirString_WithUserDefineSplitter( temp, Source, '.' );
	CaseNormalize( ext, temp );

	if( (!strcmp( ext, "avi" )) || (!strcmp( ext, "mpg" )) || (!strcmp( ext, "mpeg" )) || (!strcmp( ext, "mp4" )) )
		return _VIDEO_FILE;
	
	else if( (!strcmp( ext, "mp3" )) || (!strcmp( ext, "ogg" )) || (!strcmp( ext, "wma" )) )
		return _AUDIO_FILE;

	else if( (!strcmp( ext, "jpeg" )) || (!strcmp( ext, "jpg" )) || (!strcmp( ext, "bmp" )) )
		return _IMAGE_FILE;

	else if( (!strcmp( ext, "txt" ))  )
		return _TEXT_FILE;		

	else 
		return _ETC_FILE;

}




int CheckExtension_UC16( uchar_t* ucSource )
{	
	char ext[32] = {0};
	uchar_t ext_uc[32] = {0};
	uchar_t temp[32] = {0};

	DEBUGMSG(1,("CheckExtension_UC16()\n"));
	DEBUG_U16MSG( ucSource, "-ucSource");

	GetLastItem_In_UC16FullDirString_WithUserDefineSplitter( temp, ucSource, 0x002e );
	CaseNormalize_UC16( ext_uc, temp );

	DEBUG_U16MSG( ucSource, "-ext_uc");
	ustr2str( ext, ext_uc );

	DEBUGMSG(1,("-ext:[%s]\n", ext ));

#if _MPEG4_ENABLE
	if( (!strcmp( ext, "avi" )) || (!strcmp( ext, "mpg" )) || (!strcmp( ext, "mpeg" )) || (!strcmp( ext, "mp4" )) )
#else
	if( (!strcmp( ext, "avi" )) || (!strcmp( ext, "mpg" )) || (!strcmp( ext, "mpeg" )) )
#endif
		return _VIDEO_FILE;
	
	else if( (!strcmp( ext, "mp3" )) || (!strcmp( ext, "ogg" )) || (!strcmp( ext, "wma" )) )
		return _AUDIO_FILE;

	else if( (!strcmp( ext, "jpeg" )) || (!strcmp( ext, "jpg" )) || (!strcmp( ext, "bmp" )) )
		return _IMAGE_FILE;

	else if( (!strcmp( ext, "txt" ))  )
		return _TEXT_FILE;		

	else if(  (!strcmp( ext, "smi" ))  ||  (!strcmp( ext, "smil" )))
		return _SUBTITLE_FILE;

	else 
		return _ETC_FILE;

	
}


int CheckVideoExtension( char Source[] )
{	
	char ext[32] = {0};
	char temp[32] = {0};

	GetLastItem_In_FullDirString_WithUserDefineSplitter( temp, Source, '.' );
	CaseNormalize( ext, temp );

	if(!strcmp( ext, "avi" )) return 3;
	else if(!strcmp( ext, "mp4" )) return 4;
	else if(!strcmp( ext, "mpg" )) return 5;
	else if(!strcmp( ext, "mpeg" )) return 5;
//	else if(!strcmp( ext, "vob" ) ) return 5;
	else if(!strcmp( ext, "smi" ) ) return 31;
	else if(!strcmp( ext, "smil" ) ) return 32;

	return 0;
}

int CheckVideoExtension_UC16( uchar_t ucSource[] )
{	


	
#if 0
	char ext[32] = {0};
	char temp[32] = {0};
	char Source[1024];
	
	ustr2str( Source, ucSource );

	DEBUGMSG(0,("CheckExtension( %s )\n",  Source ));

//	while(1) DEBUGMSG(1,("_"));
//	GetLastItem_In_FullDirString_WithUserDefineSplitter( temp, pLFBrowser->Filename, '.' );
	GetLastItem_In_FullDirString_WithUserDefineSplitter( temp, Source, '.' );
	DEBUGMSG(0,(" - Source : [%s]\n", Source ));
	DEBUGMSG(0,(" - temp : [%s]\n", temp ));
	CaseNormalize( ext, temp );
	DEBUGMSG(0,(" - ext (CaseNormalized temp) : [%s][%d][%d]\n", ext, strlen(ext), strlen("avi") ));
#else

	char ext[32] = {0};
	uchar_t ext_uc[32] = {0};
	uchar_t temp[32] = {0};

	GetLastItem_In_UC16FullDirString_WithUserDefineSplitter( temp, ucSource, 0x002e );
	CaseNormalize_UC16( ext_uc, temp );
	ustr2str( ext, ext_uc );

#endif

	if( !strcmp( ext, "avi" ) )
		return 3;
	else if(!strcmp( ext, "mp4" ) )
		return 4;
	else if(!strcmp( ext, "mpg" ) )
		return 5;
	else if(!strcmp( ext, "mpeg" ) )
		return 5;
//	else if(!strcmp( ext, "vob" ) )
//		return 5;
	else if(!strcmp( ext, "smi" ) )
		return 31;
	else if(!strcmp( ext, "smil" ) )
		return 32;

	return 0;
}


int CompareExtension( char SourceFilename[], char SourceExtension[] )
{	
	char ext[32] = {0};
	char temp[32] = {0};
	char temp_targetExt[32] = {0};

	DEBUGMSG(0,("CheckExtension( %s )\n",  SourceFilename ));

	GetLastItem_In_FullDirString_WithUserDefineSplitter( temp, SourceFilename, '.' );
	DEBUGMSG(0,(" - SourceFilename : [%s]\n", SourceFilename ));
	DEBUGMSG(0,(" - SourceExtension : [%s]\n", SourceExtension ));
	DEBUGMSG(0,(" - temp : [%s]\n", temp ));
	CaseNormalize( ext, temp );
	CaseNormalize( temp_targetExt, SourceExtension );
	DEBUGMSG(0,(" - ext (CaseNormalized temp) : [%s][%d][%d]\n", ext, strlen(ext), strlen(temp_targetExt) ));

	if( (!strcmp( ext, temp_targetExt )) )
		return TRUE;
	else
		return FALSE;
	
}

#if 0
// change a unicode string to non-unicode
// (just truncates the upper byte)
void unicode2char (RMuint8 *unicode, char *name, int l)
{
	int i;
	for (i=0; i<l; i++)
	{
		name[0] = unicode[0];
		name++;
		unicode += 2;
	}
	name[0] = 0;
}

#endif


void
print_ustr( const uchar_t* uString )
{
	int sl, i2;
	sl = ustrlen( uString );

printf("-------------------------------------------\n");
	for(int i=0; i<sl; i++)
		{
		if( uString[i] == 0x007c || uString[i] == 0x0000 )
			{
			printf("|\n");
			break;
			}
		else
			printf("%c ", uString[i]);
		}
printf("\n-------------------------------------------\n");

	for( i2=0; uString[i2] != 0x0000; i2++)
		{
		
		if( i2%16 == 0 )
			printf("\n");
		printf("%04x ",uString[i2]);
		}
//	printf("(%04x ",uString[i2]);
//	printf("%04x ",uString[i2+1]);
//	printf("%04x )",uString[i2+2]);
printf("\n-------------------------------------------\n\n");
}

void print_ustr( const uchar_t* uString, const char* msg )
{
	int sl, i2;
	sl = ustrlen( uString );

	printf("\t%s:[", msg);
	for(int i=0; i<sl; i++)
		{
		if( uString[i] == 0x007c || uString[i] == 0x0000 )
			{
//				printf("|\n");
			break;
			}
		else
			printf("%c ", uString[i]);
		}
	printf("]\n");

}

void printu( const char* msg, const uchar_t* uString )
{
	int sl, i2;
	sl = ustrlen( uString );

	printf("%s : [", msg);
	for(int i=0; i<sl; i++)
		{
		if( uString[i] == 0x007c || uString[i] == 0x0000 )
			{
//				printf("|\n");
			break;
			}
		else
			printf("%c", uString[i]);
		}
	printf("]\n");

}


void
print_ustr( const uchar_t* uString, int option )
{
	int sl, i2;
	sl = ustrlen( uString );

	if( option == 0 )
		{
		printf("\t[");
		for(int i=0; i<sl; i++)
			{
			if( uString[i] == 0x007c || uString[i] == 0x0000 )
				{
//				printf("|\n");
				break;
				}
			else
				printf("%c ", uString[i]);
			}
		printf("]\n");
		}

	else if( option >= 1 )
		{
		printf("\t-------------------------------------------\n\t");
		for(int i=0; i<sl; i++)
			{
			if( uString[i] == 0x007c || uString[i] == 0x0000 )
				{
				printf("|\n");
				break;
				}
			else
				printf("%c ", uString[i]);
			}
		printf("\n-------------------------------------------\n");
		}

	if( option >= 2 )
		{
		for( i2=0; uString[i2] != 0x0000; i2++)
			{
			
			if( i2%16 == 0 )
				printf("\n");
			printf("%04x ",uString[i2]);
			}
		printf("\n-------------------------------------------\n");
		}

	if( option >= 3 )
		{
		printf("(%04x ",uString[i2]); printf("%04x ",uString[i2+1]); printf("%04x )",uString[i2+2]);
		printf("\n-------------------------------------------\n\n");
		}
	

}


////////////////////////////////////////////////////////////////////////////////////
// GetVirtualButtonClick works on Absolute Position

#define EOLEFT		1365
#define EOCENTER	2730
#define EORIGHT		4096
static RMint64 i64ClickBuffer=0;
static int iMessageGetCount=0;
static int iMeanValue=0;

eTOUCHPAD_CLICK GetVirtualButtonClick( int Param1, int Param2, int* MeanValue )
{
	eTOUCHPAD_CLICK eRetButton = _TOUCHPAD_NULL;
	int iTempMeanValue = 0;

	printf("GetVirtualButtonClick( %d, %d )\t", Param1, Param2 );
	printf("- nButton : [%d]\t", eRetButton );


//	if( (Param1 == 0 && Param2 == 0)	|| (Param1 == 0 && Param2 == 1) )
	if( Param1 == 0 && Param2 == 1 )
		{
		iTempMeanValue = (int)(i64ClickBuffer/iMessageGetCount);

		if ( (iTempMeanValue-1365*2)>0 )		// Button 3
			{
			eRetButton = _TOUCHPAD_RIGHT;
			}
		else if ( (iTempMeanValue -1356) > 0)	// Botton2
			{
			eRetButton = _TOUCHPAD_CENTER;
			}
		else if ( (iTempMeanValue) > 0)			// Botton1
			{
			eRetButton = _TOUCHPAD_LEFT;
			}

		MeanValue = &iTempMeanValue;

		printf("*** TOUCHPAD : eRetButton[%d] Clicked. (iTempMeanValue:%d) ***\n", eRetButton, iTempMeanValue );

 		i64ClickBuffer = 0;
		iMessageGetCount = 0;
		iMeanValue = 0;

		}
	else if( iMessageGetCount < 4 )
		{

		if( i64ClickBuffer == 0 )
			{
			i64ClickBuffer = Param1;
			iMessageGetCount = 1;
			iMeanValue = i64ClickBuffer;
			}
		else
			{
			i64ClickBuffer += Param1;
			iMessageGetCount++;
			iMeanValue = (int)(i64ClickBuffer/iMessageGetCount);
			}
		}
	else if( abs(iMeanValue - Param1) < 200 )
		{
		i64ClickBuffer += Param1;
		iMessageGetCount++;
		iMeanValue = (int)(i64ClickBuffer/iMessageGetCount);
		}

	return eRetButton;
}

eTOUCHPAD_CLICK GetVirtualButtonClick( int Param1, int Param2 )
{
	eTOUCHPAD_CLICK eRetButton = _TOUCHPAD_NULL;
	int iTempMeanValue = 0;

//	printf("GetVirtualButtonClick( %d, %d )\t", Param1, Param2 );
//	printf("- nButton : [%d]\t", eRetButton );


//	if( (Param1 == 0 && Param2 == 0)	|| (Param1 == 0 && Param2 == 1) )
	if( Param1 == 0 && Param2 == 1 )
		{
		iTempMeanValue = (int)(i64ClickBuffer/iMessageGetCount);

		if ( (iTempMeanValue-1365*2)-500 >0 )		// Button 3
			{
			eRetButton = _TOUCHPAD_RIGHT;
			}
		else if ( (iTempMeanValue -1356) +500 > 0)	// Botton2
			{
			eRetButton = _TOUCHPAD_CENTER;
			}
		else if ( (iTempMeanValue) > 0)			// Botton1
			{
			eRetButton = _TOUCHPAD_LEFT;
			}

//		printf("*** TOUCHPAD : eRetButton[%d] Clicked. (iTempMeanValue:%d) ***\n", eRetButton, iTempMeanValue );

 		i64ClickBuffer = 0;
		iMessageGetCount = 0;
		iMeanValue = 0;

		}
	else if( iMessageGetCount < 4 )
		{

		if( i64ClickBuffer == 0 )
			{
			i64ClickBuffer = Param1;
			iMessageGetCount = 1;
			iMeanValue = i64ClickBuffer;
			}
		else
			{
			i64ClickBuffer += Param1;
			iMessageGetCount++;
			iMeanValue = (int)(i64ClickBuffer/iMessageGetCount);
			}
		}
	else if( abs(iMeanValue - Param1) < 200 )
		{
		i64ClickBuffer += Param1;
		iMessageGetCount++;
		iMeanValue = (int)(i64ClickBuffer/iMessageGetCount);
		}

	return eRetButton;
}



int GetAbsoluteClickPosition( int Param1, int Param2 )
{
	static int ClickPosition;
	static int nLastParam2;
	int	nRetVal;

	ClickPosition = Param1;

	
	if ( Param2 == 0 && nLastParam2 == 1)
	{
		nRetVal = ClickPosition;				// tapping
		printf("\n-------------------------------\nGetAbsoluteClickPosition( %d, %d )\n", Param1, Param2 );
		printf("- nRetVal : [%d]\n", nRetVal );
		printf("-------------------------------\n");
		
	}
	else
	{
		nRetVal = -1;						// no tapping
	}
	
	nLastParam2 = Param2;

	return nRetVal;

}


#define DISABLE_TAVI_TIMER 1



////////////////////////////////////////////////////////////////////////////////
// FileBrowser Timer
extern long FB_RegisteredUTime;
void Reset_FileBrowserTimer()
{
	//DEBUGMSG(1,("Reset_FileBrowserTimer : "));
	FB_RegisteredUTime = gettime_sec();
//	printf("Reset_FileBrowserTimer():%d\n", FB_RegisteredUTime );
	//DEBUGMSG(1,("%d ", FB_RegisteredUtime));
}
void Stop_FileBrowserTimer()
{
//	printf("Stop_FileBrowserTimer()\n");
	FB_RegisteredUTime = 0;
}


////////////////////////////////////////////////////////////////////////////////
// VideoPlayer ControlPannelTimer
extern long CP_RegisteredUTime;
void Reset_VideoControlPannelTimer()
{
	CP_RegisteredUTime = gettime_sec();
}
void Stop_VideoControlPannelTimer()
{
	CP_RegisteredUTime = 0;
}


////////////////////////////////////////////////////////////////////////////////
// VideoPlayer Chunk Timer
extern long VP_RegisteredUTime;
void Reset_VideoChunkTimer()
{
	VP_RegisteredUTime = gettime_sec();
}
void Stop_VideoChunkTimer()
{
	VP_RegisteredUTime = 0;
}

////////////////////////////////////////////////////////////////////////////////
// VideoPlayer Seek Timer
extern long VS_RegisteredUTime;
void Reset_VideoSeekTimer()
{
//	DEBUGMSG(1,("Reset_VideoControlPannelTimer : "));
	VS_RegisteredUTime = gettime_sec();
//	DEBUGMSG(1,("%d ", VS_RegisteredUTime));
}
void Stop_VideoSeekTimer()
{
	VS_RegisteredUTime = 0;
}


////////////////////////////////////////////////////////////////////////////////
// VideoPlayer Seek Free Timer
extern long VSF_RegisteredUTime;
void Reset_VideoSeekFreeTimer()
{
	VSF_RegisteredUTime = gettime_sec();
}
void Stop_VideoSeekFreeTimer()
{
	VSF_RegisteredUTime = 0;
}


////////////////////////////////////////////////////////////////////////////////
// SoundPalette Timer
extern long RegUT_SoundPaletteCtrlHide;
void Reset_SoundPaletteCtrlHideTimer()
{
//	Reset_SoundPaletteTouchApplyTimer();
	RegUT_SoundPaletteCtrlHide = gettime_sec();
	printf("(Reset_SPCHT)\n");
}
void Stop_SoundPaletteCtrlHideTimer()
{
//	Stop_SoundPaletteTouchApplyTimer();
	RegUT_SoundPaletteCtrlHide = 0;
	printf("(Stop_SPCHT)\n");
}

extern long RegUT_SoundPaletteCtrlHideTouchApplyTimer;
void Reset_SoundPaletteTouchApplyTimer()
{
	RegUT_SoundPaletteCtrlHideTouchApplyTimer = gettime_sec();
}
void Stop_SoundPaletteTouchApplyTimer()
{
	RegUT_SoundPaletteCtrlHideTouchApplyTimer = 0;
}


void SetTimer( long* pRegTimer )
{
	*(pRegTimer) = gettime_sec();
}
void ClearTimer( long* pRegTimer )
{
	*(pRegTimer) = 0;
}



long Check_VideoSeekTimer()
{
	return VS_RegisteredUTime;
}


#if 0
int Reset_AppTimer( int iTimerID, CTimer* pTimer )
{
//	DEBUGMSG(1,("Reset_VideoControlPannelTimer : "));
	CP_RegisteredUTime = gettime_sec();
//	DEBUGMSG(1,("%d ", CP_RegisteredUTime));

#if 0
	#ifndef DISABLE_TAVI_TIMER

	int iNewTimerID = -1;

	DeregisterTimer( iTimerID  );

	pTimer					= new CTimer;
	pTimer->m_nTaskID		= 2;		// Video Player
	pTimer->m_nWindowID	= _CONTROL_PANNEL_PROGRESS_MODE;		// Param1
	pTimer->m_nType		= CTimer::TIMER_ONESHOT;
	pTimer->m_nExpireTime	= 5000;
	#if 0
	if ( (iNewTimerID=RegisterTimer( pTimer )) < 0 )		// regist timer
		{
		fprintf(stderr,"Reset_AppTimer : Error at RegisterTimer IdTimer=%d \n", pTimer->m_nTimerID );
		pTimer->m_nTimerID = -1;
		}
	else
		{
		pTimer->m_nTimerID = iNewTimerID;
		DEBUGMSG(1,("\nReset_AppTimer m_nTimerID=%d\n", pTimer->m_nTimerID  ));
		}

	return pTimer->m_nTimerID;
	#endif

	iNewTimerID = RegisterTimer( pTimer );
	usleep(50000);

	return  iNewTimerID;
	#endif
#endif
}

void Stop_AppTimer( int iTimerID, CTimer* pTimer )
{

	CP_RegisteredUTime = 0;

/*

#if0
#ifndef DISABLE_TAVI_TIMER

#if 0


	for( iTimerID = 0; iTimerID < 10; iTimerID++ )
		DeregisterTimer( iTimerID );
	DEBUGMSG(1,("\nStop_AppTimer m_nTimerID=%d\n", iTimerID ));

#endif
	DeregisterTimer( iTimerID );
	usleep(50000);
#endif
#endif
*/
}
#endif


/*
#define MEP_REPORT_ABSOLUTE			0x01
#define MEP_REPORT_RELATIVE			0x02
*/
static int iMAP_MODE=-1;
void SetTaviTouchpadMode( int mode )
{
//	printf("SetTaviTouchpadMode(%x->%x)\n", iMAP_MODE, mode);

	if( iMAP_MODE != 0x01 && mode == 0x01 )
		{
		mep_report(mode);
		iMAP_MODE = 0x01;
//		printf("\t... pass 0x01\n", mode);
		}

	else if( iMAP_MODE != 0x02 && mode == 0x02  )
		{
		mep_report(mode);
		iMAP_MODE = 0x02;
//		printf("\t... pass 0x02\n", mode);
		}

	else if( iMAP_MODE == -1 )
		{
//		printf("\t... first Running\n", mode);
		iMAP_MODE = mode;
		mep_report(mode);
		}
//	else
//		printf("\t... skip\n");

}
	
static int iTK_REPEAT;
static int iTK_SINGLE;
static int iTK_PRE_REPEAT=-1;
static int iTK_PRE_SINGLE=-1;

extern int handle_key;
void SetTaviKeyMode( U16 flags )
{
	if( (TK_RESUME & flags) == TK_RESUME )
		{
		if( iTK_PRE_REPEAT > 0 )
			iTK_REPEAT = iTK_PRE_REPEAT;
		
		if(iTK_PRE_SINGLE > 0 )
			iTK_SINGLE = iTK_PRE_SINGLE;
		
		}
	
	else
		{

		iTK_PRE_REPEAT = iTK_REPEAT;
		iTK_PRE_SINGLE = iTK_SINGLE;

		if( (TK_REPEAT & flags) == TK_REPEAT )
			{
			if( iTK_REPEAT != 1 )
				{
				ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
				iTK_REPEAT = 1;
				}
			}
		else	
			{// TK_LONG
			if( iTK_REPEAT != 0 )
				{
				ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 0 );
				iTK_REPEAT = 0;
				}
			}



		if( (TK_SINGLE & flags) == TK_SINGLE )
			{
			if( iTK_SINGLE != 1)
				{		
				ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );
				iTK_SINGLE = 1;
				}
			}
		else		// TK_VARIOUS
			{
			if( iTK_SINGLE != 0)
				{		
				ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 0 );
				iTK_SINGLE = 0;
				}
			}
		}

}


/*
void LongKey_Enable()
{
	// Change Key repeat mode;
	extern int handle_key;
	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 0 );
	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 0 );
}

void LongKey_Disable()
{
	// Change Key repeat mode;
	extern int handle_key;
	ioctl( handle_key, TAVI_KEY_IOCS_REPEATMODE, 1 );
	ioctl( handle_key, TAVI_KEY_IOCS_SINGLEEVENT, 1 );
}
*/

char* trimh( char input[] )
{
	int sz = strlen(input);
	int i = 0;
		
	for( ; i< sz ; i++ )
		{

		if( input[i] > 32 )
			break;
			
		}
	return &input[i];
	
}

int GetMSTime (void)
{
	struct timeval tv1;
	struct timezone tz;
	gettimeofday (&tv1, &tz);
	return tv1.tv_sec * 1000 + tv1.tv_usec/1000;
}


void ClearKeyBuffer( void )
{
	ioctl( handle_key, TAVI_KEY_IOCS_CLEAR, 0 );
}


int crc32(char *s, int len)
{
// www.codegure.com/Cpp/Cpp/algorithms/checks

	int perByte;
	int perBit;
	const unsigned long poly = 0xedb88320;
	unsigned long crc_value = 0xffffffff;

	for(perByte = 0; perByte < len; perByte++) {
		unsigned char c;
		c = *(s++);
		for(perBit=0; perBit <8; perBit++){
			crc_value = (crc_value >> 1) ^
				(((crc_value ^ c) & 0x1) ? poly : 0);
			c >>= 1;
		}
	}
	return crc_value;
}

unsigned long crcu32(char *s, int len)
{
// www.codegure.com/Cpp/Cpp/algorithms/checks

	int perByte;
	int perBit;
	const unsigned long poly = 0xedb88320;
	unsigned long crc_value = 0xffffffff;

	for(perByte = 0; perByte < len; perByte++) {
		unsigned char c;
		c = *(s++);
		for(perBit=0; perBit <8; perBit++){
			crc_value = (crc_value >> 1) ^
				(((crc_value ^ c) & 0x1) ? poly : 0);
			c >>= 1;
		}
	}
	return crc_value;
}

SOUNDEFFECT_t GetSFX_ValueByPID( int picture_id )
{
	/*
	input : Sound Palette Icon ID
	output : SOUNDEFFECT_t
	*/
/*
	for( int i=0; i<=(int)SOUND_SEQ_USER; i++ )
		{
		if( arraySFXtoMAC[i] == GetSFX_MacroByPID(picture_id) )
			return (SOUNDEFFECT_t) i;
		}

	return SOUND_NORMAL;
*/

	if( TableEQ[picture_id][1] == -1 ) 
		return SOUND_NORMAL;
	else 
		return (SOUNDEFFECT_t) TableEQ[picture_id][1];
}

SOUNDEFFECT_t GetSFX_ValueByMacro( int macro_name )
{
	/*
	input : SOUND_EFFECT_MACRO (ML_SOUND_PALETTE_TRUE_3D_MILD ~ ML_SOUND_PALETTE_MY_EQ)
	output : SOUNDEFFECT_t
	*/

#if 0	
	for( int i=0; i<=(int)SOUND_SEQ_USER; i++ )
		{
		if( arraySFXtoMAC[i] == macro_name )
			return (SOUNDEFFECT_t) i;
		}

	return SOUND_NORMAL;
#endif

	for( int i=0; i<=TableEQMaxRow; i++ )
		{
		if( TableEQ[i][0] == macro_name )
			{
			if( TableEQ[i][1] == -1 ) 
				return SOUND_NORMAL;
			else 
				return (SOUNDEFFECT_t) TableEQ[i][1];
			}
		}
	
	return SOUND_NORMAL;
	
}

int GetSFX_MacroByPID( int picture_id )
{
	/*
	input : Sound Palette Icon ID
	output : SOUND_EFFECT_MACRO (ML_SOUND_PALETTE_TRUE_3D_MILD ~ ML_SOUND_PALETTE_MY_EQ)
	*/
	//return arraySFXtoMAC[ (int)GetSFX_ValueByPID(picture_id) ];
	return (TableEQ[picture_id][0]);
}
int GetSFX_MacroBySFX( SOUNDEFFECT_t sound_effect )
{
	/*
	input : SOUNDEFFECT_t
	output : SOUND_EFFECT_MACRO (ML_SOUND_PALETTE_TRUE_3D_MILD ~ ML_SOUND_PALETTE_MY_EQ)
	*/
	//return arraySFXtoMAC[(int)sound_effect];

	for( int i=0; i<=TableEQMaxRow; i++ )
		{
		if( TableEQ[i][1] == (int)sound_effect )
			return TableEQ[i][0];
		}
	return ML_NORMAL;
}

int GetSFX_IconIndexByMacro( int macro_name )
{
	/*
	input : SOUND_EFFECT_MACRO (ML_SOUND_PALETTE_TRUE_3D_MILD ~ ML_SOUND_PALETTE_MY_EQ)
	output : Sound Palette Icon ID
	*/
	/*
	if( macro_name >= ML_SOUND_PALETTE_TRUE_3D_MILD && macro_name < ML_SOUND_PALETTE_MY_EQ+1 )
		return (macro_name-ML_SOUND_PALETTE_TRUE_3D_MILD+1);
	else
		return 0;
	*/

	for( int i=0; i<=TableEQMaxRow; i++ )
		{
		if( TableEQ[i][0] == macro_name )
			return i;
		}	
	return 0;
}
int GetSFX_IconIndexBySFX( SOUNDEFFECT_t sound_effect )
{
	/*
	input : SOUNDEFFECT_t
	output : Sound Palette Icon ID
	*/
/*	
	int input;
	
	input = GetSFX_MacroBySFX( sound_effect );

	if( input >= ML_SOUND_PALETTE_TRUE_3D_MILD && input < ML_SOUND_PALETTE_MY_EQ+1 )
		return (input-ML_SOUND_PALETTE_TRUE_3D_MILD+1);
	else
		return 0;
*/
	for( int i=0; i<=TableEQMaxRow; i++ )
		{
		if( TableEQ[i][1] == (int)sound_effect )
			return i;
		}	
	return 0;	
}
int BuildFullPath( uchar_t* Fullpath, uchar_t* CurrentDir, uchar_t* Filename )
{
	int nCD, nFN;
	nCD = ustrlen( CurrentDir );
	nFN = ustrlen( Filename );

	if( nCD+nFN+1 >= FATFS_MAX_PATHNAME_LENGTH )
		return FALSE;
	
	ustrcpy( Fullpath, CurrentDir );
	adduchar( Fullpath, 0x002f );
	ustrcat( Fullpath, Filename );
	return TRUE;
}
void showdata( int iSaveData[] )
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	DEBUGMSG(1,("----showdata----------------------------------------\n%4d: ", 0));
	for(int i=0; i<MAXIMUM_BOOKMARK_ALLOW; i++)
		{
		if( i!=0 && i %16== 0 )
			DEBUGMSG(1,("\n%04d: ", i));
		DEBUGMSG(1,("%d ",  iSaveData[i] ));

		if( (i+1)%4 == 0 )
			DEBUGMSG(1,(" ", i));
		}
	DEBUGMSG(1,("\n----------------------------------------------------\n"));
	///////////////////////////////////////////////////////////////////////////////////////////////
}
void showdata_U32( U32 SaveData[], char* msg )
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	DEBUGMSG(1,("----showdata:%s----------------------------------------\n%4d: ", msg, 0));
	for(int i=0; i<MAXIMUM_BOOKMARK_ALLOW; i++)
		{
		if( i!=0 && i %16== 0 )
			DEBUGMSG(1,("\n%08d: ", i));
		DEBUGMSG(1,("%d ", SaveData[i] ));

		if( (i+1)%4 == 0 )
			DEBUGMSG(1,(" "));
		}
	DEBUGMSG(1,("\n----------------------------------------------------\n"));
	///////////////////////////////////////////////////////////////////////////////////////////////
}
void showdata_U32( U32 SaveData[] )
{
	showdata_U32( SaveData, "-" );
}

extern CTopPanel* g_TopPanel;
	
void UpdateAlarmStatus()
{
	if( TaviSysParam.alarm.useAlarm == 0 )
		g_TopPanel->SetAlarm(TRUE);
	else
		g_TopPanel->SetAlarm(FALSE);
}

uchar_t* SetTimeStr( int AmPmTxtRes, int TimeDigit, uchar_t* pBuffer )
{
	char cTimeDigitTxt[8];
	uchar_t ucTimeDigitTxt[8];
	uchar_t* AmPmTxt;

//	printf("SetTimeStr()\n");

	AmPmTxt = pT->GetTextRes( AmPmTxtRes );
//	DEBUG_U16MSG(AmPmTxt, "-AmPmTxt");
	
	sprintf( cTimeDigitTxt, "%d", TimeDigit );
//	printf("-cTimeDigitTxt : [%s]\n", cTimeDigitTxt);
	
	str2ustr( ucTimeDigitTxt, cTimeDigitTxt );
//	DEBUG_U16MSG(ucTimeDigitTxt, "-ucTimeDigitTxt");
	
	ustrcpy( pBuffer, ucTimeDigitTxt );
	adduchar( pBuffer, 0x0020 );
	ustrcat( pBuffer, AmPmTxt );	

//	DEBUG_U16MSG(pBuffer, "-pBuffer");
	
	return pBuffer;
}


