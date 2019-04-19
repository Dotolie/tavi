//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibText Source Code File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibText.cpp,v $
// $Revision: 1.7 $
// $Date: 2006/04/19 13:07:44 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibText.cpp,v $
// Revision 1.7  2006/04/19 13:07:44  zstein
// - deleted useless debug messages.
//
// Revision 1.6  2006/04/15 03:01:16  ywkim
// Change some procedure for stopofftimer
//
// Revision 1.5  2006/04/05 01:47:11  zstein
// - modified drawcharacter(...) for caption.
//
// Revision 1.4  2006/03/27 11:37:51  zstein
// - modified DT_OUTLINE option.
//
// Revision 1.3  2006/02/27 02:39:19  akaiaphanu
// add DrawScrollTxt()
//
// Revision 1.2  2006/02/24 02:08:20  akaiaphanu
// add DT_SCROLL
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.85  2006/01/24 13:26:08  ywkim
// Remove definition of DT_RADIO_FONT_OPT
// Add definitions of DT_NUMBER_6WIDTH & DT_NUMBER_8WIDTH
// Change function of GetStringWidth(),DrawCharactor(), DrawEffectiveCharactor() & IsStaticWidth for above option
//
// Revision 1.84  2006/01/16 09:42:23  zstein
// *** empty log message ***
//
// Revision 1.83  2006/01/13 08:29:14  ywkim
// Modify GetStringWidth() & DrawEffectCharacter() for opt of DT_RADIO_FONT_OPT
//
// Revision 1.82  2006/01/11 09:29:54  etnlwind
// Update for TextLib
//
// Revision 1.81  2006/01/09 05:32:22  etnlwind
// Update for Text
//
// Revision 1.80  2006/01/05 09:14:15  etnlwind
// Update for Video Caption Align Related
//
// Revision 1.79  2006/01/03 09:31:57  etnlwind
// Update for Static Text Align
//
// Revision 1.78  2006/01/02 06:54:40  etnlwind
// Update for New Font Bug-Fix
//
// Revision 1.77  2005/12/30 04:24:42  etnlwind
// Update for New Font Adatio
//
// Revision 1.76  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.75  2005/12/03 04:40:24  etnlwind
// Update for LibFileBrowser
//
// Revision 1.74  2005/12/03 03:52:07  etnlwind
// Update for FileBrowser Bottom Menu
//
// Revision 1.73  2005/11/30 10:04:15  etnlwind
// Update for TextViewer Width
//
// Revision 1.72  2005/11/03 03:14:33  etnlwind
// AVI 비디오 재생시 프레임레이트 정보가 잘못되어 있는지 검사해보고
// 잘못되어 있을경우 대화상자로 사용자에게 현상황을 알리고 재생여부를 확인
// 사용자가 재생을 시도하면, Seek기능을 막고 재생하도록 수정
// (프레임레이트 정보가 깨진 비디오 파일은 주로 Seek기능에는
// 문제가 생기고, 가끔씩 재생중 멈추거나 끊어지는 경우도 발생함)
//
// 비디오 재생중 TASK버튼 누를경우 배경을 지우지 않고 메인으로 가는경우 수정
//
// 비디오 디렉토리 반복 수정
// 비디오 재생시간 잔상제거
// 비디오 전체시간 위치 수정
// 비디오 재생중 볼륨조절시 비디오재생이 좀더 매끄럽게 수정
//
// Revision 1.71  2005/10/31 10:43:47  etnlwind
// Update for VideoPlayer BugFix
//
// Revision 1.70  2005/10/26 09:04:36  etnlwind
// Update for TextViewer
//
// Revision 1.69  2005/10/24 09:39:39  etnlwind
// Update for LibText
//
// Revision 1.68  2005/10/18 10:21:37  etnlwind
// Update for Dbx Auto Limiter
//
// Revision 1.67  2005/10/18 09:05:13  etnlwind
// Update for New DialgBox, MessageBox UI
//
// Revision 1.66  2005/10/17 06:49:33  zstein
// - ConvertEUC8StrtoUnicode16Str: boundary exception.
//
// Revision 1.65  2005/10/17 06:14:36  etnlwind
// Update for TextViewer
//
// Revision 1.64  2005/10/14 11:39:38  etnlwind
// Update for TextViwer
//
// Revision 1.63  2005/10/12 09:24:06  etnlwind
// 1. 텍스트뷰어 관련 루틴 추가
//
// Revision 1.62  2005/10/07 10:12:57  etnlwind
// Update for TextViewer
//
// Revision 1.61  2005/09/14 02:28:37  zstein
// - update CSize GetStringSize(...).
//
// Revision 1.60  2005/08/27 12:59:27  etnlwind
// Update for FileManager
//
// Revision 1.59  2005/08/27 06:20:05  etnlwind
// Upate for FileManager
//
// Revision 1.58  2005/08/25 09:26:57  etnlwind
// Update for FileManager / Virtual Keyboard Update
//
// Revision 1.57  2005/08/22 09:40:32  etnlwind
// Update for
// 1. Loading Progress
// 2. Video Player Related Patch
//
// Revision 1.56  2005/08/22 07:16:11  etnlwind
// Update for
// 1. Video Player Clock Added.
// 2. Clock Draw Func. Added on TopPannel Class.
// 3. DrawUC16Text DT_TOP_PANNEL option Added.
//
// Revision 1.55  2005/08/17 03:22:22  etnlwind
// Update for FileManager
//
// Revision 1.54  2005/08/09 10:52:09  etnlwind
// Update for
// 1. CSoundPaletteDbx Added
// 2. Video Control Pannel UI changed
//
// Revision 1.53  2005/08/08 12:01:36  etnlwind
// Update for GetDividerEnclosedStringWidth()
//
// Revision 1.52  2005/08/08 11:30:01  etnlwind
// Update for GetNthStringWidthWithDivider()
//
// Revision 1.51  2005/08/08 11:18:09  etnlwind
// Updatef for GetDividerEnclosedStringWidth
//
// Revision 1.50  2005/08/08 11:00:35  ywkim
// Add function of GetDividerEnclosedStringWidth with parameter of resID
//
// Revision 1.49  2005/08/08 10:49:41  etnlwind
// Update for FileManger, VideoPlayer
//
// Revision 1.48  2005/07/29 06:41:26  etnlwind
// Update for FileManger Related
//
// Revision 1.47  2005/07/26 09:53:43  etnlwind
// Update for File Manager
//
// Revision 1.46  2005/07/26 04:50:02  etnlwind
// Update for FileManager Basic
//
// Revision 1.45  2005/07/26 01:31:03  etnlwind
// Update for
//
// 1. BDF font changed;
//    16pix font -> 16pix bold font, 12pix font -> English Alphabet Size Increased
//
// 2. AVI playback routine has changed by external source
//
// Revision 1.44  2005/07/20 17:56:27  etnlwind
// Update for Subtitle Releates Issues
//
// Revision 1.43  2005/07/16 02:13:58  etnlwind
// Update for FileDbx
//
// Revision 1.42  2005/07/16 02:07:56  etnlwind
// Update for LibFileDbx
//
// Revision 1.41  2005/07/12 12:45:35  etnlwind
// Update for LibFileDbx
//
// Revision 1.40  2005/07/07 13:58:26  etnlwind
// Update for Subtitle Releate Bug Fix
//
// Revision 1.39  2005/07/07 01:15:24  etnlwind
// Update for
// 1. Xen Sound Off on PCM Audio included Video.
// 2. Subtitle Outline Color Changed to No Transparency Black
// 3. Text Resources
//
// Revision 1.38  2005/07/05 13:11:06  etnlwind
// Update for Video Player
//
// Revision 1.37  2005/07/05 11:23:57  etnlwind
// Update for Subtitle Font & Video SubMenu
//
// Revision 1.36  2005/07/05 05:58:49  etnlwind
// Update for macro name fix (ML_NAME_DECENT -> ML_NAME_DESCENT)
//
// Revision 1.35  2005/06/27 13:25:41  etnlwind
// Update for VideoPlayer FileBrowser Related Problem
//
// Revision 1.34  2005/06/16 02:37:31  etnlwind
// Update for Video Touchpad Calibration
//
// Revision 1.33  2005/06/15 11:51:30  etnlwind
// Update for Video Player Touch Control And Background Savefile Process
//
// Revision 1.32  2005/05/27 11:59:06  etnlwind
// Update for Video Subtitle
//
// Revision 1.31  2005/05/15 12:41:14  zstein
// *** empty log message ***
//
// Revision 1.30  2005/05/15 11:13:38  zstein
// - changed GetBoundaryLen(...).
//
// Revision 1.29  2005/05/14 11:47:11  etnlwind
// Update for FontEngine Bug Fix
//
// Revision 1.28  2005/05/14 06:14:12  zstein
// *** empty log message ***
//
// Revision 1.27  2005/05/13 11:41:21  zstein
// - changed ::GetBoundaryLength(...).
//
// Revision 1.26  2005/05/13 11:39:24  etnlwind
// Update for Limited X res
//
// Revision 1.25  2005/05/13 11:28:27  zstein
// - changed DrawCharacter( ... ).
//
// Revision 1.24  2005/05/13 06:55:13  etnlwind
// Update for USB connect/disconnect
//
// Revision 1.23  2005/05/12 14:42:35  etnlwind
// Update for Japanise Language Support
//
// Revision 1.22  2005/05/12 14:41:46  zstein
// - changed return value of ConvertEUC8StrToUnicode16Str(...).
//
// Revision 1.21  2005/05/12 04:37:53  zstein
// - fixed bug GetStringSize(...) maded by Lee Seok.
//
// Revision 1.20  2005/05/11 08:03:46  etnlwind
// Update for MPEG4 and Divx3 low profile Codec Related
//
// Revision 1.19  2005/05/10 05:16:15  etnlwind
// Update For ES4
//
// Revision 1.18  2005/05/10 05:05:21  etnlwind
// Multi-Line Text Process
//
// Revision 1.17  2005/05/09 03:07:17  etnlwind
// Update for ES4
//
// Revision 1.16  2005/05/04 14:07:39  etnlwind
// Update for Multi-Line MessageBox
//
// Revision 1.15  2005/05/04 12:52:51  etnlwind
// Update for MessageBox Related
//
// Revision 1.14  2005/05/03 12:22:38  etnlwind
// Update for ES4
//
// Revision 1.13  2005/05/03 09:00:04  etnlwind
// Update for ES4
//
// Revision 1.12  2005/04/29 14:32:44  etnlwind
// Update for ES4
//
// Revision 1.11  2005/04/28 12:19:15  etnlwind
// Update for FATFS/PBI Selection
// If you want to change File System Driver, Change tavi/include/define_fs.h
//
// Revision 1.10  2005/04/28 06:04:42  zstein
// Not changed.
//
// Revision 1.9  2005/04/28 05:58:44  zstein
// demo merge.
//
// Revision 1.8  2005/04/27 17:01:58  etnlwind
// New Type MsgBox Library Update
//
// Revision 1.7  2005/04/26 03:39:13  ywkim
// Add function of GetTextRes() for returning string pointer
//
// Revision 1.6  2005/04/23 01:26:28  zstein
// Added - CSize GetStringSize( const uchar_t* psz, int len, int nFontSize );
//       - CSize GetCharSize( uchar_t ch, int nFontSize );
// 	  - int GetBoundaryLength( uchar_t* pwc, int nLength, int nFontSize, int nBoundaryWidth );
//
// Revision 1.5  2005/04/22 14:37:35  etnlwind
// Library Update for UI changes
//
// Revision 1.4  2005/04/21 05:01:59  etnlwind
// Multi-Langul Text Data Use Added.
//
// Revision 1.3  2005/04/20 11:28:46  etnlwind
// update for ES4
//
// Revision 1.2  2005/04/18 14:30:43  etnlwind
// update for ES4
//
// Revision 1.1  2005/04/18 09:18:25  etnlwind
// TextEngine Add
// Update for ES4
//
//
//*****************************************************************************

#include <string.h>
#include <Component/object.h>
#include <Component/image.h>

#include <Lib/LibText.h>
#include <Lib/LibImage.h>
#include <Lib/LibScrollBar.h>
#include <Lib/LibDialogBox.h>
#include <Lib/LibBookmarkDialogBox.h>
#include <Lib/LibSetupDialogBox.h>
#include <Lib/LibControlPannel.h>
#include <Lib/LibListBox.h>
#include <Lib/LibFileBrowser.h>

#include <tavi_text_resources.h>
#include <KSC5601.h>

#include "LibDefines.h"
#include<Task/tavi_global.h>

#define USE_FAT_FS		1
//#define USE_LINUX_FS	0
//#define USE_DPI_FS		0


#define _OPTIMIZED_BDF_MODE	1

extern SysParameter_t	TaviSysParam;

static const U16 	us16BitMask[4] 		= { 0xf000, 0x0f00, 0x00f0, 0x000f };		// 1111000000000000, 0000111100000000, 0000000011110000, 0000000000001111 
static const U8		ub4BitMask[4] 		= { 8, 4, 2, 1 };							// 1000, 0100, 0010, 0001
static const U8		ubBitShiftRange[4] 	= { 12, 8, 4, 0 };

void DEBUGMEM( char msg[] )
{
	for(int i=0;i<2;i++)
		sleep(2);
	
	printf("-[%s]-----------------------------------------\n", msg);
	system( "free" );
	printf("---------------------------------------------------------------\n\n");
}


CTextEngine_BDF::CTextEngine_BDF( char* FontDir )
{
	char temp[256];
	char temp2[32];														
	FATFS_ERROR fatfs_err;
	RMuint32 fd;

	DEBUGMSG(1,("\nCTextEngine_BDF(%s)\n", FontDir));

	TextResBuf = NULL;

	// Init Text Scroll Buffer
	TextScrollBuffer = (RMuint8*) malloc(TEXT_SCROLL_BUFFER_SIZE);	// 81,920 bytes
	memset( TextScrollBuffer, 27, TEXT_SCROLL_BUFFER_SIZE );

	//sprintf( temp, "%s%s", FontDir, "/b12.bdf.opt" );
	//sprintf( temp, "%s%s", FontDir, "/gulim12.bdf.opt" );
	//fatfs_err = fatfs_fopen ((RMuint8 *)temp, _O_RDONLY, &fd);
	if( TaviSysParam.etc.systemfont ) sprintf( temp, "%s%s", FontDir, "/dotum12.bdf.opt" );
	else  sprintf( temp, "%s%s", FontDir, "/gulim12.bdf.opt" );

	//printf("-temp:[%s]\n",temp);
	fatfs_err = fatfs_fopen ((RMuint8 *)temp, _O_RDONLY, &fd);
	ASSERT (fatfs_err == FATFS_ERROR_NO_ERROR);
	//printf("fatfs_err = [0x%x]\n", fatfs_err );
	if( fatfs_err != FATFS_ERROR_NO_ERROR )
		{
		printf("fatfs_err = [0x%x]\n", fatfs_err );
		}
	T12.fd = fd;
	fatfs_fsize( T12.fd, &(T12.FontFilesize));
	T12.FontSize = 12;
	T12.Glyph = (TOptimizedBDFCharInfo**) malloc( sizeof(TOptimizedBDFCharInfo*) * 0xffff );	// 262,140 bytes
	for(int loop=0; loop<0xffff; loop++) {T12.Glyph[loop] = NULL;}
	FontLoadDiskToRAM_Optimized( &T12 );

	fatfs_err  = fatfs_fclose(T12.fd);


	//sprintf( temp, "%s%s", FontDir, "/b16.bdf.opt" );
	//sprintf( temp, "%s%s", FontDir, "/b16_b.bdf.opt" );
	//fatfs_err = fatfs_fopen ((RMuint8 *)temp, _O_RDONLY, &fd);

	if( TaviSysParam.etc.captionfont ) sprintf( temp, "%s%s", FontDir, "/dotum16.bdf.opt" );
	else  sprintf( temp, "%s%s", FontDir, "/gulim16.bdf.opt" );
	fatfs_err = fatfs_fopen ((RMuint8 *)temp, _O_RDONLY, &fd);
	//ASSERT (fatfs_err == FATFS_ERROR_NO_ERROR);
	//printf("fatfs_err = [0x%x]\n", fatfs_err );
	ASSERT (fatfs_err == FATFS_ERROR_NO_ERROR);
	if( fatfs_err != FATFS_ERROR_NO_ERROR )
		{
		printf("fatfs_err = [0x%x]\n", fatfs_err );
		}

	T16.fd = fd;
	fatfs_fsize( T16.fd, &(T16.FontFilesize));
	T16.FontSize = 16;
	T16.Glyph = (TOptimizedBDFCharInfo**) malloc( sizeof(TOptimizedBDFCharInfo*) * 0xffff );	// 262,140 bytes
	for(int loop=0; loop<0xffff; loop++) {T16.Glyph[loop] = NULL;}
	FontLoadDiskToRAM_Optimized( &T16 );

	fatfs_err  = fatfs_fclose(T16.fd);
	ASSERT (fatfs_err == FATFS_ERROR_NO_ERROR);
	//printf("fatfs_err = [0x%x]\n", fatfs_err );

	Load_SystemTextResources( (eSYSTEM_LANGUAGE) TaviSysParam.etc.language );

}

CTextEngine_BDF::~CTextEngine_BDF()
{
	Remove_SystemLanguageData( );

	if(TextScrollBuffer != NULL)
		{
		free(TextScrollBuffer);
		TextScrollBuffer = NULL;
		}

	if(T12.Glyph != NULL)
		{
		free(T12.Glyph);
		T12.Glyph = NULL;
		}

	if(T16.Glyph != NULL)
		{
		free(T16.Glyph);
		T16.Glyph = NULL;
		}
}

U8 CTextEngine_BDF::Convert1digitBCDCharToHexCode( U8 BCDChar)
{
	// ex) "A" -> 0x0A

	if( BCDChar >= 0x30 && BCDChar < 0x40 ) return (BCDChar - 0x30);
	else if( BCDChar >= 0x41 && BCDChar < 0x47 ) return (BCDChar - 0x37);
	else if( BCDChar >= 0x61 && BCDChar < 0x67 ) return (BCDChar - 0x57);
	return 0;
}

U8 CTextEngine_BDF::Convert2digitBCDStringToHexCode( U8 * BCDString  )
{
	// ex) "AC00" -> 0xAC00
	return ( (Convert1digitBCDCharToHexCode(BCDString[0])<<4) |
			 Convert1digitBCDCharToHexCode(BCDString[1])  );
}

U16 CTextEngine_BDF::Convert4digitBCDStringToHexCode( U8 * BCDString  )
{
	// ex) "AC00" -> 0xAC00
	return ( (Convert1digitBCDCharToHexCode(BCDString[0])<<12) |
			(Convert1digitBCDCharToHexCode(BCDString[1])<<8) |
			(Convert1digitBCDCharToHexCode(BCDString[2])<<4) |
			 Convert1digitBCDCharToHexCode(BCDString[3])  );
}

U16 CTextEngine_BDF::ConvertVarBCDStringTokenToHexCode( U8 ubTokenNumber, U8 * BCDString )
{
	// ex) ConvertVarBCDStringTokenToHexCode(2, "6 12 0 -2")   -> return 12
	// 		ubTokenNumber  =>				   0 1  2  3   4
	
 	U8 	ubTemp[256];
	U8	ubTempCursor	= 0;

	U8	ubCursor		= 0;
	U8	ubTokenCounter	= 0 ;

	memset( ubTemp, '\0', 256 );

	for( ; ; ubCursor++ )
		{
		
		if( BCDString[ubCursor] != ' ' && BCDString[ubCursor] != '\t' && BCDString[ubCursor] != '\n' && BCDString[ubCursor] != '\0' )
			{
			memcpy( (char*)&ubTemp[ubTempCursor], (char*)&BCDString[ubCursor], strlen((char*)&BCDString[ubCursor]) );
			ubTempCursor++;
			}
		else
			{
			ubTokenCounter++;

			if( ubTokenCounter == ubTokenNumber )
				{
				ubTemp[ubTempCursor] = '\0';
				
				break;
				}
			else
				{
				memset( ubTemp, '\0', 256 );
				ubTempCursor	 = 0;
				}
			
			}


		
		}

	return (U16) atoi( (char*)ubTemp );
	
}



U32 CTextEngine_BDF::ConvertVarBCDStringTokenToHexCodeU32( U8 ubTokenNumber, U8 * BCDString )
{
	// ex) ConvertVarBCDStringTokenToHexCode(2, "6 12 0 -2")   -> return 12
	// 		ubTokenNumber  =>				   0 1  2  3   4

	//        1, "3412343 \0");
	
 	U8 	ubTemp[256];
	U8	ubTempCursor	= 0;

	U8	ubCursor		= 0;
	U8	ubTokenCounter	= 0 ;

	memset( ubTemp, '\0', 256 );


	for( ; ubTokenCounter < ubTokenNumber; ubCursor++ )
		{
		
		if( BCDString[ubCursor] != ' ' && BCDString[ubCursor] != '\0' )
			{
			memcpy( (char*)&ubTemp[ubTempCursor], (char*)&BCDString[ubCursor], strlen((char*)&BCDString[ubCursor]) );
			ubTempCursor++;
			}
		else
			{
			ubTokenCounter++;

			if( ubTokenCounter == ubTokenNumber )
				{
				ubTemp[ubTempCursor] = '\0';
				
				break;
				}
			else
				{
				memset( ubTemp, '\0', 256 );
				ubTempCursor	 = 0;
				}
			
			}


		
		}

	return (U32)atol( (char*)ubTemp );
	
}

U16 CTextEngine_BDF::ConvertEUC8StrToUnicode16Str( U16 uc16String[], char euc8String[], int MaxLineLength )
{
	
	int p_uc;
	int p_euc;
//	int ssLengthOfSourceText;
	int ssLengthOfTargetText;
	U16 aus2byteTextArray[TEXTVIEWER_BLOCK_BUFFER_SIZE];

	int UnicodeStrLen;
	int AsciiStrLen;

	UnicodeStrLen = 0;
	AsciiStrLen = 0;

//	ssLengthOfSourceText = strlen( euc8String );
//	ssLengthOfTargetText = 

	if( MaxLineLength > 0 && MaxLineLength < TEXTVIEWER_BLOCK_BUFFER_SIZE )
		ssLengthOfTargetText = MaxLineLength;
	else
		ssLengthOfTargetText = TEXTVIEWER_BLOCK_BUFFER_SIZE;

//        for( p_euc=0, p_uc=0; p_euc< ssLengthOfSourceText; p_euc++ )
        for( p_euc=0, p_uc=0; p_uc< ssLengthOfTargetText; p_euc++)
		{
		if( euc8String[p_euc] == 0 )
			{
			//printf("(cvt-EOT)\n");
			break;
			}
    		if( (U8)euc8String[p_euc] > 0x80 && (U8)euc8String[p_euc+1] > 0x40 )
			{
                	int kc=0;

			switch( ((unsigned char)euc8String[p_euc])&0xF0 )
                		{
                		case 0x80: kc = 0; break;
                		case 0x90: kc = 2670; break;
                		case 0xA0: kc = 5518; break;
                		case 0xB0: kc = 7942; break;
                		case 0xC0: kc = 10790; break;
                 		case 0xD0: kc = 12722; break;
                		case 0xE0: kc = 14226; break;
                		case 0xF0: kc = 15730; break;
				}

                	aus2byteTextArray[p_uc] = ((unsigned char)euc8String[p_euc]<<8) | (unsigned char)euc8String[p_euc+1];

			for( ; kc< __KSC5610_MAX_CODE__; kc++ ) // worst case: KSC5610 AUμa ¼o ¸¸A­ loop
				{
				if( aus2byteTextArray[p_uc] == ausConvertingTableFromEUC8ToUnicode16[kc][0] )
					{
					uc16String[p_uc] = ausConvertingTableFromEUC8ToUnicode16[kc][1];
                                	break;
					}
				}

			p_uc++;
			p_euc++;

			UnicodeStrLen++;
			AsciiStrLen+=2;

			}
#if 0
		else if(  (U8)euc8String[p_euc] < 0x20 )
			{
			printf("(cvt-<0x20)\n");
			break;
			}
#endif
		else if(  (U8)euc8String[p_euc] <= 0xFF )
			{            	
			uc16String[p_uc] = (U16)euc8String[p_euc];
			p_uc++;

			UnicodeStrLen++;
			AsciiStrLen++;
			//printf("(cvt-<0xFF)\n");
			}

		else
			{
    	            	uc16String[p_uc] = (U16)'_';
			}
                
		}

	uc16String[p_uc] = 0x0000;

/*
DEBUGMSG(1,("\nConvertEUC8StrToUnicode16Str()\n"));
DEBUGMSG(1,("-p_uc : %d\n", p_uc ));
DEBUGMSG(1,("-p_euc : %d\n", p_euc ));
DEBUGMSG(1,("-ssLengthOfTargetText : %d\n", ssLengthOfTargetText ));
DEBUG_U16MSG( uc16String, "-uc16String" );
*/
	return UnicodeStrLen;
}

U16 CTextEngine_BDF::ConvertVFATTextToUnicode16Str( U16 uc16String[], char euc8String[] )
{
	int p_uc;
	int p_euc;
	int ssLengthOfSourceText = strlen( euc8String );
	U16 aus2byteTextArray[256];

	for( p_euc=0, p_uc=0; p_euc< ssLengthOfSourceText;  )
		{

		if( euc8String[p_euc] == ':' )
			{
			p_euc++;
			uc16String[p_uc] = Convert4digitBCDStringToHexCode( (U8*)(euc8String+p_euc) );
			p_euc = p_euc+4;
			p_uc++;
			}

		else
			{
			uc16String[p_uc] = (U16)euc8String[p_euc];
			p_euc++;
			p_uc++;
			}

		}
	
	uc16String[p_uc] = 0x0000;
	return 0x0020;
}
#define SzChrInfo 38
static RMuint8 readbuf[2048*SzChrInfo];
int CTextEngine_BDF::FontLoadDiskToRAM_Optimized( TOptimizedBitmapDataFont* tFont  )
{
	int iTSize = sizeof( TOptimizedBDFCharInfo );
	char temp[256];
	RMuint32 n, t, loop, maxloop;
	FATFS_ERROR fatfs_err;
	TOptimizedBDFCharInfo* ptCurrentCharCode;
	RMuint8* RawBuffer;

	memset( readbuf, 0, 2048*SzChrInfo );
	tFont->Raw = (RMuint8*)malloc( tFont->FontFilesize );
	RawBuffer = tFont->Raw;

	t=0;
	n=0;
	do
	{
		fatfs_err = fatfs_fread (tFont->fd, readbuf, sizeof(readbuf), &n);
		memcpy( RawBuffer + t, readbuf, n );
		t += n;		
	} while(n);

	maxloop = (tFont->FontFilesize/SzChrInfo); 
	for( loop=0 ; loop< maxloop; loop++ )
		{
		ptCurrentCharCode = (TOptimizedBDFCharInfo*)(&RawBuffer[loop*SzChrInfo]);
		tFont->Glyph[ ptCurrentCharCode->encoding ] = ptCurrentCharCode;
		}

	return TRUE;
}

U16 CTextEngine_BDF::DrawCharacter( TOptimizedBDFCharInfo* ptCurrentCharCode, U16 x, U16 y, U16 color, U16 fontsize, U32 option )
{
	return ( DrawEffectCharacter(ptCurrentCharCode, x, y, color, _SHADOW, fontsize, option) );
}

typedef void (*PIXEL_EFFECTER)(U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2);
typedef void (*PIXEL_EFFECTER_NoTr)(U16 usTemp, U16 usABitOf4Bits, U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2);

static void SetPixel_Shadow_Bold_BG( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+2,  y+usRow+1 );
}
static void SetPixel_Shadow_Bold_FG( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	pMpegDecoder->SetPixel( color, x+usABitOf16Bits+1,  y+usRow );
	pMpegDecoder->SetPixel( color, x+usABitOf16Bits,  y+usRow );
}
static void SetPixel_Shadow_Normal_BG( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+1,  y+usRow+1 );
}
static void SetPixel_Normal_FG( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	pMpegDecoder->SetPixel( color, x+usABitOf16Bits,  y+usRow );
}
static void SetPixel_Outline_Bold_BG( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-2,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-1,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+2,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+3,  y+usRow );

	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-1,  y+usRow-1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-1,  y+usRow+1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow-1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow+1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+1,  y+usRow-1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+1,  y+usRow+1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+2,  y+usRow-1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+2,  y+usRow+1 );


	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow-2 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow+2 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+1,  y+usRow-2 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+1,  y+usRow+2 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+2,  y+usRow-2 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+2,  y+usRow+2 );								

	//								pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-1,  y+usRow );
	//								pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow-1 );
	//								pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+2,  y+usRow );
	//								pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow+1 );

}
/*
void SetPixel_Outline_Bold_BG( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-1,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow-1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+2,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow+1 );
}
*/
static void SetPixel_Outline_Bold_FG( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
pMpegDecoder->SetPixel( color, x+usABitOf16Bits,  y+usRow );
pMpegDecoder->SetPixel( color, x+usABitOf16Bits+1,  y+usRow );
}
static void SetPixel_Outline_Normal_BG( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-1,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow-1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+1,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow+1 );
}
static void SetPixel_Caption_BG( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-1,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+1,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow-1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow+1 );

	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-2,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+2,  y+usRow );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow-2 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits,  y+usRow+2 );

	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-1,  y+usRow-1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits-1,  y+usRow+1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+1,  y+usRow-1 );
	pMpegDecoder->SetPixel( color2 , x+usABitOf16Bits+1,  y+usRow+1 );
}
static void SetPixel_Bold( U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	pMpegDecoder->SetPixel( color, x+usABitOf16Bits,  y+usRow );
	pMpegDecoder->SetPixel( color, x+usABitOf16Bits+1,  y+usRow );
}
static void SetPixel_NoTransparency( U16 usTemp, U16 usABitOf4Bits, U16 usABitOf16Bits, U16 x, U16 y, U16 usRow, U16 color, U16 color2 )
{
	if( ( usTemp & ub4BitMask[usABitOf4Bits] ) == ub4BitMask[usABitOf4Bits]  )  
		pMpegDecoder->SetPixel( color, x+usABitOf16Bits,  y+usRow );
	else 
		pMpegDecoder->SetPixel( 0, x+usABitOf16Bits,  y+usRow );
}
#define T12_X_ORIGIN 2
#define T12_Y_ORIGIN 10
#define T16_X_ORIGIN 2
#define T16_Y_ORIGIN 14
static U16 _DrawCore(TOptimizedBDFCharInfo* ptCurrentCharCode, U16 x, U16 y, U16 color, U16 color2, PIXEL_EFFECTER fpPixelEffecter, U8 ubDigitShiftValue )
{
	U16 	usTemp, us16Digit, usRow, usColum, usABitOf16Bits, usABitOf4Bits;

//	printf("-code:0x%X  RowShift:%d  BBYoff:%d sBBYoff:%d\n", ptCurrentCharCode->encoding, RowShift, ptCurrentCharCode->BByoff, (signed int)ptCurrentCharCode->BByoff );
//	printf("\n");
	for( usRow = 0 ; usRow < ptCurrentCharCode->BBh ; usRow++ )		
		{
		us16Digit = (ptCurrentCharCode->fontdata[usRow])<<ubDigitShiftValue;
		for( usColum = 0, usABitOf16Bits=0 ; usABitOf16Bits <= ptCurrentCharCode->BBw;  usColum++ )
			{
			usTemp = (us16Digit & us16BitMask[usColum]) >> ubBitShiftRange[usColum];
			for(  usABitOf4Bits=0;  usABitOf4Bits< 4 && usABitOf16Bits <= ptCurrentCharCode->BBw; usABitOf16Bits++, usABitOf4Bits++ )
				if( ( usTemp & ub4BitMask[usABitOf4Bits] ) == ub4BitMask[usABitOf4Bits]  )  
					{
					fpPixelEffecter( usABitOf16Bits, x, y, usRow, color, color2 );
//					printf("O");
					}
/*
				else
					{
//					printf("_");
					}
*/
			}
//		printf("\n");
		}
//	printf("\n");
	return usABitOf16Bits;
}
static int IsStaticWidth( RMuint32 option, TOptimizedBDFCharInfo* ptCurrentCharCode = NULL )
{
	if( (option & DT_STATIC_6WIDTH) == DT_STATIC_6WIDTH ) return 6;
	else if( (option & DT_STATIC_8WIDTH) == DT_STATIC_8WIDTH ) return 8;
	else if( (option & DT_STATIC_10WIDTH) == DT_STATIC_10WIDTH ) return 10;
	else if( (option & DT_STATIC_12WIDTH) == DT_STATIC_12WIDTH ) return 12;
	else if( (option & DT_STATIC_14WIDTH) == DT_STATIC_14WIDTH ) return 14;
	else if( (option & DT_NUMBER_8WIDTH) == DT_NUMBER_6WIDTH 
		&& (ptCurrentCharCode->encoding >= 0x0030 && ptCurrentCharCode->encoding < 0x003B) ) return 6;
	else if( (option & DT_NUMBER_8WIDTH) == DT_NUMBER_8WIDTH 
		&& (ptCurrentCharCode->encoding >= 0x0030 && ptCurrentCharCode->encoding < 0x003B) ) return 8;

	else return FALSE;
}

#define COLON 58
U16 CTextEngine_BDF::DrawEffectCharacter( TOptimizedBDFCharInfo* ptCurrentCharCode, U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 option )
{
	U16 	usTemp, us16Digit, usRow, usColum, usABitOf16Bits, usABitOf4Bits;
	U8	ubDigitShiftValue;
	int isBold = 0;
	int StaticWidth;

	if(ptCurrentCharCode == 0) return 0;
	if(ptCurrentCharCode->encoding == 0) return 0;
	if(ptCurrentCharCode->encoding < 0x0020) return 0;
	if(ptCurrentCharCode->encoding == 0x0020) return 4;

//	printf("\n(0x%04X)\n", ptCurrentCharCode->encoding);

	if( StaticWidth=IsStaticWidth(option, ptCurrentCharCode) ){
/*
		printf("-x:%d\n", x );
		printf("-StaticWidth:%d\n",StaticWidth);
		printf("-ptCurrentCharCode->BBw:%d\n",ptCurrentCharCode->BBw);
		printf("-T:%d\n\n",(StaticWidth-ptCurrentCharCode->BBw)/2);
*/
		int xp = (StaticWidth-ptCurrentCharCode->BBw)/2;
		if(xp) x += xp;
		}

		
	if( ptCurrentCharCode->BBw <= 8 ) ubDigitShiftValue = 8;	// 폭이 8bit 이하
	else ubDigitShiftValue = 0;

	usABitOf16Bits = 0;

	if( fontsize == 16 )
		{
		y += (T16_Y_ORIGIN - (int)ptCurrentCharCode->BBh  + ((-1)*((int)ptCurrentCharCode->BByoff)) );
		}
	else
		{
		y += (T12_Y_ORIGIN - (int)ptCurrentCharCode->BBh  + ((-1)*((int)ptCurrentCharCode->BByoff)) );
		}

	// Transparenced Draw
	if( (option & DT_TRANSPARENCY) == DT_TRANSPARENCY )
		{
		if( (option & DT_BOLD) == DT_BOLD ) isBold = 1;

		if( (option & DT_CAPTION) == DT_CAPTION )
			{
			_DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Caption_BG, ubDigitShiftValue );
			usABitOf16Bits = _DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Normal_FG, ubDigitShiftValue );
			}
		else if( (option & DT_OUTLINE) == DT_OUTLINE )
			{
			if( isBold )
				{
				_DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Outline_Bold_BG, ubDigitShiftValue );
				usABitOf16Bits = _DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Outline_Bold_FG, ubDigitShiftValue );
				usABitOf16Bits++;
				}
			else
				{
				usABitOf16Bits = _DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Outline_Normal_BG, ubDigitShiftValue );
				_DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Normal_FG, ubDigitShiftValue );
				}
			}
		else if( (option & DT_SHADOW) == DT_SHADOW )
			{
			if( isBold )
				{
				_DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Shadow_Bold_BG, ubDigitShiftValue );
				usABitOf16Bits = _DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Shadow_Bold_FG, ubDigitShiftValue );
				usABitOf16Bits++;
				}
			else
				{
				_DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Shadow_Normal_BG, ubDigitShiftValue );
				usABitOf16Bits =  _DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Normal_FG, ubDigitShiftValue );
				}
			}
		else
			{
			if( isBold )
				{
				usABitOf16Bits = _DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Bold, ubDigitShiftValue );
				usABitOf16Bits++;
				}
			else
				{
				usABitOf16Bits = _DrawCore( ptCurrentCharCode, x, y, color, color2, SetPixel_Normal_FG, ubDigitShiftValue );
				//printf("<%d> ", usABitOf16Bits);
				}
			}
		}

	// Normal Draw
	else
		{
		usABitOf16Bits = 0;
		for( usRow = 0 ; usRow < ptCurrentCharCode->BBh ; usRow++ )		
			{
			us16Digit = (ptCurrentCharCode->fontdata[usRow])<<ubDigitShiftValue;
			for( usColum = 0, usABitOf16Bits=0 ; usABitOf16Bits <= ptCurrentCharCode->BBw;  usColum++ )
				{
				usTemp = (us16Digit & us16BitMask[usColum]) >> ubBitShiftRange[usColum];
				for(  usABitOf4Bits=0;  usABitOf4Bits< 4 && usABitOf16Bits <= ptCurrentCharCode->BBw; usABitOf16Bits++, usABitOf4Bits++ )
					{
					if( ( usTemp & ub4BitMask[usABitOf4Bits] ) == ub4BitMask[usABitOf4Bits]  )  
						pMpegDecoder->SetPixel( color, x+usABitOf16Bits,  y+usRow );
					else 
						pMpegDecoder->SetPixel( 0, x+usABitOf16Bits,  y+usRow );
					}
				}
			}
		}
	if( StaticWidth=IsStaticWidth(option, ptCurrentCharCode) )
		return StaticWidth;
	else
		return (usABitOf16Bits+1);	//기본적으로 폰트에 1dot 공백이 있음
}

U16 CTextEngine_BDF::InitTextScrollBuffer( U16 BackGroundPalette )
{
	int errcnt=0;
	memset( TextScrollBuffer, BackGroundPalette, TEXT_SCROLL_BUFFER_SIZE );
	return TRUE;
}

U16 CTextEngine_BDF::DrawCharacterOnScrollBuffer( TOptimizedBDFCharInfo* ptCurrentCharCode, int rx, int ry, int vx, int* vy, U16 color, U16 fontsize, U32 option )
{

	int usRow, y;
	U16 usTemp, us16Digit, usColum, usABitOf16Bits, usABitOf4Bits;
	U8 ubDigitShiftValue;

	if(ptCurrentCharCode == 0) return 0;
	if(ptCurrentCharCode->encoding == 0) return 0;
	if(ptCurrentCharCode->encoding == 0x0020)	
		{
		if((option&DT_TRANSPARENCY) == DT_TRANSPARENCY)
			{
			return 4;
			}
		else
			{
			for(int y=0; y<fontsize; y++)
				for(int x=0; x<4; x++)
					TextScrollBuffer[(y*TEXT_SCROLL_BUFFER_WIDTH)+(vx+x)] = (RMuint8)0;
			return 4;
			}
		}
		
	if( ptCurrentCharCode->BBw <= 8 ) ubDigitShiftValue = 8;	// 폭이 8bit 이하
	else ubDigitShiftValue = 0;

	usABitOf16Bits = 0;

	if( fontsize == 16 ) y = (T16_Y_ORIGIN - (int)ptCurrentCharCode->BBh  + ((-1)*((int)ptCurrentCharCode->BByoff)) );
	else y = (T12_Y_ORIGIN - (int)ptCurrentCharCode->BBh  + ((-1)*((int)ptCurrentCharCode->BByoff)) );

	if( (option & DT_TRANSPARENCY) == DT_TRANSPARENCY )
		{

		for( usRow = 0 ; usRow < ptCurrentCharCode->BBh ; usRow++ )		
			{
			us16Digit = (ptCurrentCharCode->fontdata[usRow])<<ubDigitShiftValue;
			for( usColum = 0, usABitOf16Bits=0 ; usABitOf16Bits <= ptCurrentCharCode->BBw;  usColum++ )
				{
				usTemp = (us16Digit & us16BitMask[usColum]) >> ubBitShiftRange[usColum];
				for(  usABitOf4Bits=0;  usABitOf4Bits< 4 && usABitOf16Bits <= ptCurrentCharCode->BBw; usABitOf16Bits++, usABitOf4Bits++ )
					if( ( usTemp & ub4BitMask[usABitOf4Bits] ) == ub4BitMask[usABitOf4Bits]  )  
						TextScrollBuffer[((usRow+y)*TEXT_SCROLL_BUFFER_WIDTH)+(vx+usABitOf16Bits)] = (RMuint8)color;
//					else
//						TextScrollBuffer[((usRow)*TEXT_SCROLL_BUFFER_WIDTH)+(vx+usABitOf16Bits)] = 27;
 				}
			}
		}

	else
		{
		for( usRow = 0 ; usRow < ptCurrentCharCode->BBh ; usRow++ )		
			{
			us16Digit = (ptCurrentCharCode->fontdata[usRow])<<ubDigitShiftValue;
			for( usColum = 0, usABitOf16Bits=0 ; usABitOf16Bits <= ptCurrentCharCode->BBw;  usColum++ )
				{
				usTemp = (us16Digit & us16BitMask[usColum]) >> ubBitShiftRange[usColum];
				for(  usABitOf4Bits=0;  usABitOf4Bits< 4 && usABitOf16Bits <= ptCurrentCharCode->BBw; usABitOf16Bits++, usABitOf4Bits++ )
					{
					if( ( usTemp & ub4BitMask[usABitOf4Bits] ) == ub4BitMask[usABitOf4Bits]  )  
						TextScrollBuffer[((usRow+y)*TEXT_SCROLL_BUFFER_WIDTH)+(vx+usABitOf16Bits)] = (RMuint8)color;					
					else 
						TextScrollBuffer[((usRow+y)*TEXT_SCROLL_BUFFER_WIDTH)+(vx+usABitOf16Bits)] = 0;
					}
				}
			}
		}

	int temp;
	temp = *vy;

	if( temp < usRow+y )
		*vy = usRow+y;

	return (usABitOf16Bits+1);

}


int CTextEngine_BDF::Load_SystemTextResources( eSYSTEM_LANGUAGE language )
{
	uchar_t fullpath[1024];
	char cTemp[1024];
	int templang;
	RMuint32 fd, rc;
	FATFS_ERROR err;
	int res_index=0;	// resouce index
	int ecc;	// "equal char" count
	int ccol;	// char counter on line
	int sp;	// start point
	int ep;	// end point
	int loops;

	DEBUGMSG(1,("\nLoad_SystemTextResources(%d)\n", language ));

	switch( language )
		{
		case SL_KOREAN:
			str2ustr( fullpath, _LANGUAGE_KOREAN, strlen(_LANGUAGE_KOREAN) );
			break;

		case SL_SPANISH:
			str2ustr( fullpath, _LANGUAGE_SPANISH, strlen(_LANGUAGE_SPANISH) );
			break;

		case SL_ITALIAN:
			str2ustr( fullpath, _LANGUAGE_ITALIAN, strlen(_LANGUAGE_ITALIAN) );
			break;

		case SL_FRENCH:
			str2ustr( fullpath, _LANGUAGE_FRENCH, strlen(_LANGUAGE_FRENCH) );
			break;

		case SL_GERMAN:
			str2ustr( fullpath, _LANGUAGE_GERMAN, strlen(_LANGUAGE_GERMAN) );
			break;

		case SL_RUSSIAN:
			str2ustr( fullpath, _LANGUAGE_RUSSIAN, strlen(_LANGUAGE_RUSSIAN) );
			break;

		case SL_JAPANESE:
			str2ustr( fullpath, _LANGUAGE_JAPANESE, strlen(_LANGUAGE_JAPANESE) );
			break;

		case SL_CHINESE:	
			str2ustr( fullpath, _LANGUAGE_CHINESE, strlen(_LANGUAGE_CHINESE) );
			break;

		case SL_ENGLISH:
		default:
			str2ustr( fullpath, _LANGUAGE_ENGLISH, strlen(_LANGUAGE_ENGLISH) );
			break;
		}


	if( (err=fatfs_wfopen( (RMuint16*)fullpath, _O_RDONLY, &fd )) == 0 )
		{
		TextResBuf = (uchar_t*) malloc(  sizeof(uchar_t) * MAX_TEXT_RES_FILE_SIZE  );

		err=fatfs_fread( fd, (RMuint8*)TextResBuf, (sizeof(uchar_t) * MAX_TEXT_RES_FILE_SIZE), &rc );
		DEBUGMSG(1,("(((((  err=fatfs_fread : %x     rc: %d))))))\n", err, rc ));
		fatfs_fclose(fd);
		}
	else
		{
		DEBUGMSG(1,("(((((  err=fatfs_fread : %x     rc: %d))))))\n", err, rc ));
		printf("err:[%s]\n", GetErrMsg( cTemp, (int)err) );
		//print_ustr( fullpath );		
		
		// ASSERT
		uchar_t tmsg[1024];
		str2ustr( tmsg, "Can't open message resource file : ", strlen("Can't open message resource file : "));
		ustrcat(tmsg, fullpath);
		DrawUC16Text( tmsg, 10, 10, _WHITE, 12, 0 );
		ASSERT(0);
		fatfs_fclose(fd);
		for(;;) {sleep(10);}
		return FALSE;
		}


		ecc=0; ccol=0; sp=-1; ep=-1;
		for(loops=0; loops<rc; loops++)
			{
			if( TextResBuf[loops] == 0x003d )
				{
				ecc++;
				if( ecc == 2 )
					sp = loops+1;
				}

			else if( TextResBuf[loops] == 0x000a || TextResBuf[loops] == 0x000d )
				{ 
				ep=loops;
				if( sp != -1 )
					{
					TextRes[res_index] = &(TextResBuf[sp]);
					res_index++;
					}
				TextResBuf[ep] = 0x0000;
				ecc=0; ccol=0; sp=-1; ep=-1;
				}
			}
	
	return TRUE;
}

int CTextEngine_BDF::Remove_SystemLanguageData( )
{
	DEBUGMSG(1,("\nRemove_SystemLanguageData()\n" ));

	if( TextResBuf != NULL )
		{
		DEBUGMSG(1,("-Text Resource Buffer Release ... " ));
		free(TextResBuf );
		TextResBuf  = NULL ;
		DEBUGMSG(1,("done\n" ));
		}

	DEBUGMSG(1,("-Text Resource Clear ... " ));
	memset( TextRes, 0, sizeof(uchar_t*)*MAX_TEXT_RES_LINE );
	DEBUGMSG(1,("done\n" ));

	return TRUE;
}

int CTextEngine_BDF::Change_SystemLanguage( eSYSTEM_LANGUAGE language )
{
	int ret;
	DEBUGMSG(1,("\nChange_SystemLanguage(%d)\n", language ));
	ret = Remove_SystemLanguageData();
	ASSERT(ret );
	return ( Load_SystemTextResources(language) );
}

U16 CTextEngine_BDF::DrawResText( U32 ResIndex, U16 x, U16 y, U16 color, U16 fontsize, U32 option )
{
	return ( DrawUC16Text( TextRes[ResIndex], x, y, color, _SHADOW, fontsize, option ) );
}
uchar_t* CTextEngine_BDF::GetTextRes( U32 ResIndex )
{
	return( (uchar_t*)TextRes[ResIndex] );
}

U16 CTextEngine_BDF::DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 fontsize, U32 option )
{
	return DrawUC16Text( usString, x, y, color, _SHADOW, fontsize, option  );
}
U16 CTextEngine_BDF::DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 option )
{
	return DrawUC16Text( usString, x, y, color, color2, fontsize, 320, option );
}
U16 CTextEngine_BDF::DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 limited_x, U32 option )
{
	return DrawUC16Text( usString, x, y, color, color2, fontsize, limited_x, NULL, option );
}
U16 CTextEngine_BDF::DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 limited_x, int* pIsAutoSplitActivated, U32 option )
{
	int	vx			= 0;
	int	vy			= 0;
	int	rx			= (int)x;		// Start Point of Screen 
	int	ry			= (int)y;
	int	usIndex		= 0;
	int	eovy		= 16;
	U16 ret;

//print_ustr( usString, 2 );

	TOptimizedBDFCharInfo** ppGlyph;

	switch(fontsize)
		{
		case 16:
			ppGlyph = T16.Glyph;
			break;
		case 12:
		default:
			ppGlyph = T12.Glyph;
		}

	if( (option & DT_SCROLLTEXT) == DT_SCROLLTEXT )	
	{
		limited_x = (320-38-x);
		InitTextScrollBuffer(27);

		for( usIndex=0; usString[usIndex] != '\0' ; usIndex++ )
			{
			ret = DrawCharacterOnScrollBuffer( ppGlyph[ usString[usIndex] ], rx, ry, vx, &vy, color, fontsize, option );
			rx += ret;
			vx += ret;
			}

		InitScrollText( x, y, limited_x, 0, vx, MIN(vy,eovy) );
	}
	else if( (option & DT_SCROLL_TITLE) == DT_SCROLL_TITLE )	
	{
		limited_x = (240-x);
		InitTextScrollBuffer(color2);

		for( usIndex=0; usString[usIndex] != '\0' ; usIndex++ )
			{
			ret = DrawCharacterOnScrollBuffer( ppGlyph[ usString[usIndex] ], rx, ry, vx, &vy, color, fontsize, option );
			rx += ret;
			vx += ret;
			}

		InitScrollText( x, y, limited_x, 0, vx, MIN(vy,eovy) );
	}
	else if( (option & DT_SCROLL_SELECTBAR) == DT_SCROLL_SELECTBAR )	
	{
		limited_x = (320-38-x);
		InitTextScrollBuffer(color2);

		for( usIndex=0; usString[usIndex] != '\0' ; usIndex++ )
			{
			ret = DrawCharacterOnScrollBuffer( ppGlyph[ usString[usIndex] ], rx, ry, vx, &vy, color, fontsize, option );
			rx += ret;
			vx += ret;
			}

		InitScrollText( x, y, limited_x, 0, vx, MIN(vy,eovy) );
	}
	else if( (option & DT_TEXT_AREA) == DT_TEXT_AREA )	
		{

		limited_x = (320-38-x);
		InitTextScrollBuffer(_WHITE);

		for( usIndex=0; usString[usIndex] != '\0' ; usIndex++ )
			{
			ret = DrawCharacterOnScrollBuffer( ppGlyph[ usString[usIndex] ], rx, ry, vx, &vy, color, fontsize, option );
			rx += ret;
			vx += ret;
			}

		InitScrollText( x, y, limited_x, 0, vx, MIN(vy,eovy) );

		}
	else
		{
		if( option == CALCULATION_ONLY )
			{
			int EucLength=0;
			CSize Sz;

			limited_x = (320-20);

			for( usIndex=0;; usIndex++, EucLength++ )
				{
				if( usString[usIndex] == 0x0000 ) 
					{
					if(pIsAutoSplitActivated)
						*(pIsAutoSplitActivated) = FALSE;
					return EucLength;
					break;
					}
				if( usString[usIndex] == (uchar_t)'\n' )
					{ 
					usIndex++; 
					EucLength++;
					if(pIsAutoSplitActivated)
						*(pIsAutoSplitActivated) = FALSE;
					return EucLength;
					break;
					}
				
				if( rx >= limited_x )  
					{
					if(pIsAutoSplitActivated)
						*(pIsAutoSplitActivated) = TRUE;
					return EucLength;
					break;
					}
				
				GetCharSize(usString[usIndex], usString[usIndex], &Sz);
				rx += Sz.width;

				if( usString[usIndex] > 0x00ff )
					EucLength++;
				
				}

			return EucLength;
			}
		else if( (option & DT_TEXTVIEWER) == DT_TEXTVIEWER )
			{
			int EucLength=0;

			limited_x = (320-28);

			for( usIndex=0;; usIndex++, EucLength++ )
				{
				if( usString[usIndex] == 0x0000 ) 
					{
printf("(0x0000)\n");
					if(pIsAutoSplitActivated)
						*(pIsAutoSplitActivated) = FALSE;
					return EucLength;
					break;
					}
				if( usString[usIndex] == (uchar_t)'\n' )
					{ 
					usIndex++; 
					EucLength++;
					if(pIsAutoSplitActivated)
						*(pIsAutoSplitActivated) = FALSE;
					return EucLength;
					break;
					}
				
				if( rx >= limited_x )  
					{
					if(pIsAutoSplitActivated)
						*(pIsAutoSplitActivated) = TRUE;
					return EucLength;
					break;
					}
				
				rx += DrawEffectCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, color2, fontsize, (option|DT_TRANSPARENCY|DT_OUTLINE) );
			
				if( usString[usIndex] > 0x00ff )
					EucLength++;
				
				}

			return EucLength;
			}

		else if( (option & DT_FILEBROWSER) == DT_FILEBROWSER )
			{
			limited_x = (320-x+8);
			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				{
				rx += DrawEffectCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, color2, fontsize, option );
				}
			}

		else if( (option & DT_TOP_PANNEL) == DT_TOP_PANNEL )
			{
			limited_x = (BATTERY_X-82);
			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				rx += DrawEffectCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, color2, fontsize, option );

			if( x+GetStringWidth( usString, 12, option) > limited_x )
				{
				DrawEffectCharacter( ppGlyph[0x002e] , rx, ry, color, color2, fontsize, option ); rx+=5;
				DrawEffectCharacter( ppGlyph[0x002e] , rx, ry, color, color2, fontsize, option ); rx+=5;
				DrawEffectCharacter( ppGlyph[0x002e] , rx, ry, color, color2, fontsize, option );
				}
			}		

		else if( (option & DT_CONTROLPANNEL) == DT_CONTROLPANNEL)
			{
			limited_x = ICONXPOS_EQ-8;
			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				rx += DrawEffectCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, color2, fontsize, option );
			}		

		else if( (option&DT_PROGRESS_DBX) == DT_PROGRESS_DBX )
			{

			limited_x = 250;

			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				rx += DrawEffectCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, color2, fontsize, option );

			if( x+GetStringWidth( usString, 12, DT_TRANSPARENCY) > limited_x )
				{
				DrawEffectCharacter( ppGlyph[0x002e] , rx, ry, color, color2, fontsize, option ); rx+=3;
				DrawEffectCharacter( ppGlyph[0x002e] , rx, ry, color, color2, fontsize, option ); rx+=3;
				DrawEffectCharacter( ppGlyph[0x002e] , rx, ry, color, color2, fontsize, option );
				}

			}	

		else if( (option&DT_ENABLE_DIVIDER) == DT_ENABLE_DIVIDER )
			{
			int sl, lsp, sdc;
			uchar_t* sp;
			sdc 		= GetStringDividerCount( usString );
			sl 		= ustrlen( usString );
			limited_x = 320;
			lsp 		= y;
			
			if( sdc )
				{
				uchar_t buffer[128];

				for( int loop=0; loop <= sdc ; loop++ )
					{
					sp = SetNthStringWithDivider( buffer, usString, 0x007c, loop );

					if( (option&DT_ALIGN_CENTER)==DT_ALIGN_CENTER )
						{
						//rx = (320/2) - (GetStringWidth(buffer, fontsize, option )/2);
						//rx=  (320-GetDividerEnclosedStringWidth(usString, fontsize, option ))/2;

						rx = (320 - GetStringWidth(buffer, fontsize, option ))/2;

//						printf("-StrWidth(1) : %d\n",GetStringWidth(buffer, fontsize, option ));
//						printf("-rx : %d\n",rx);
						
						for( usIndex=0; buffer[usIndex] != '\0' && rx<limited_x; usIndex++ )
							rx += DrawEffectCharacter( ppGlyph[ buffer[usIndex] ], rx, lsp+(loop*14) , color, color2, fontsize, option );
						}

					else if( (option&DT_ALIGN_RIGHT)==DT_ALIGN_RIGHT )
						{
						//rx = 320 - GetStringWidth(buffer, fontsize, option ) - x;
						rx = 320 - GetStringWidth(buffer, fontsize, option ) - x;
						for( usIndex=0; buffer[usIndex] != '\0' && rx<limited_x; usIndex++ )
							rx += DrawEffectCharacter( ppGlyph[ buffer[usIndex] ], rx, lsp+(loop*14) , color, color2, fontsize, option );
						}
					
					else 
						{
						rx = x;
						for( usIndex=0; buffer[usIndex] != '\0' && rx<limited_x; usIndex++ )
							rx += DrawEffectCharacter( ppGlyph[ buffer[usIndex] ], rx, lsp+(loop*14) , color, color2, fontsize, option );
						}
			
					}
				
				}
		
			}

		else if( (option&DT_ALIGN_CENTER)==DT_ALIGN_CENTER )
			{

#if 1		
			int real_x, StrWidth;
			limited_x = 313;

			StrWidth = GetStringWidth(usString, fontsize, option );

			#if 0
			CSize cxz;
			for( int x=0; x<ustrlen(usString); x++ )
				{
				GetCharSize( usString[x], fontsize, &cxz);
				printf("-0x%x : %d\n", usString[x], cxz.width );
				}
			#endif


			if( (rx=(320-StrWidth)/2) < 0 ) rx = 0;

#if 0
printf("-StrWidth : %d\n",StrWidth);
printf("-rx : %d\n",rx);
#endif
			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				{

/*
				printf("%c(%d, %x,%d) ", 
					(char)(usString[usIndex]&0x00FF), 
					rx,
					usString[usIndex], 
					GetCharSize( usString[usIndex], 12 ) 
					);
*/
				rx += DrawEffectCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, color2, fontsize, option );
				}
/*
//			printf("\n-----------\n\n");

			for(int hy=100; hy<200; hy++) 
				{
				pMpegDecoder->SetPixel(_RED, (320/2), hy);
				pMpegDecoder->SetPixel(_RED, (320/2) + (320/2)/2, hy);
				pMpegDecoder->SetPixel(_RED, (320/2) - (320/2)/2, hy);
				}
*/
#else

			int _tsw;
			int _tw;

			limited_x = 318;

			_tsw = GetStringWidth(usString, fontsize, option );
			printf("[[[ _tsw=%d  ", _tsw);
			
			rx = ( 320/2 -_tsw/2);
			printf("rx=%d ]]]\n", rx);


			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				{

				printf( "-0x%04x width=%d, total=%d\n", 
					usString[usIndex], 
					(_tw=DrawEffectCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, color2, fontsize, option )), 
					(rx += _tw) );
				}

			printf("\n");
#endif
			}

		else if( (option&DT_ALIGN_RIGHT)==DT_ALIGN_RIGHT )
			{
			limited_x = 318;
			rx = 320 - GetStringWidth(usString, fontsize, option ) - x;
			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				rx += DrawEffectCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, color2, fontsize, option );

			}

		else
			{
			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				rx += DrawEffectCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, color2, fontsize, option );
			}
		}

	//return usIndex;
	return rx;
}


int CTextEngine_BDF::InitScrollText( 
	int ftx, 	// drawing area X
	int fty, 	// drawing area y
	int ftw, 	// drawing area with
	int fsx,  // fontdata using start X
	int feotx,// end of text x
	int feoty// end of text x
	)
{

	int ex	= sx;
	int h	= 16;
	int x,y;
	int color, fx, fy;

	tx = ftx; 	// drawing area X
	ty = fty; 	// drawing area y
	tw = ftw; 	// drawing area with
	sx = fsx;  // fontdata using start X
	eotx = feotx;// end of text x
	eoty = feoty;// end of text x	


	for( y = 0; y < MIN(eoty,16) ; y++ )
		{
		for( x = 0; x < tw; x++)
			{
			color = (int)TextScrollBuffer[ (y*TEXT_SCROLL_BUFFER_WIDTH)+x ];
			fx = tx+x;
			fy = ty+y;
			pMpegDecoder->SetPixel( color, fx, fy);
			}
		}

	return ex;
}

int CTextEngine_BDF::DrawScrollTxt( int sx, int title_width )  // fontdata using start X
{
	int h	= 16;
	int x,y;
	int color, fx, fy;
	tw = title_width - tx;

	for( y = 0; y < MIN(eoty,16) ; y++ )
	{

		for( x=0; x < tw; x++)
		{
			color = (int)TextScrollBuffer[ (y*TEXT_SCROLL_BUFFER_WIDTH)+x+sx ];
			fx = tx+x;
			fy = ty+y;
			pMpegDecoder->SetPixel( color, fx, fy);
		}

	}

	pMpegDecoder->RefreshOsd();

	return sx;
}

int CTextEngine_BDF::DrawScrollText( int sx )  // fontdata using start X
{
	int h	= 16;
	int x,y,ox;
	int color, fx, fy;
	int ImportantInterrupt=FALSE;

	tw = 320-38-tx;

	for( y = 0; y < MIN(eoty,16) ; y++ )
		{
		RELOAD_WATCHDOGTIMER;
		ox=0;

		if((y%4))
			{
			if( AppEventCheck( pTAVI->ssTaskId, &tMsgTemp) == 0 )
				{
				if( tMsgTemp.ulMsgId != MSG_BATTERY )
					{
					MSG_Send( pTAVI->ssTaskId, &tMsgTemp ); 
					ImportantInterrupt = TRUE;
//					printf("(ImportantInterrupt)1: %d-%x-%x-%x\n", pTAVI->ssTaskId, tMsgTemp.ulMsgId, tMsgTemp.usParam1, tMsgTemp.usParam2 );
					}
				}
				
			if( MSG_Peek( pTAVI->ssTaskId, &tMsgTemp ) == 0 )
				{
				if( tMsgTemp.ulMsgId != MSG_BATTERY )
					{
					ImportantInterrupt = TRUE;
					if ( tMsgTemp.ulMsgId == MSG_BUTTON_UP && tMsgTemp.usParam1 == TAVI_KEY_LEXIT ) {
						return -2;
						}
					else {
						MSG_Send( pTAVI->ssTaskId, &tMsgTemp ); 
						}
//					printf("(ImportantInterrupt)2: %d-%x-%x-%x\n", pTAVI->ssTaskId, tMsgTemp.ulMsgId, tMsgTemp.usParam1, tMsgTemp.usParam2 );
					}
				}
			}

		for( x=0; x < tw; x++)
			{
			RELOAD_WATCHDOGTIMER;
			color = (int)TextScrollBuffer[ (y*TEXT_SCROLL_BUFFER_WIDTH)+x+sx ];
			fx = tx+x;
			fy = ty+y;
			pMpegDecoder->SetPixel( color, fx, fy);
			}

		}

	pMpegDecoder->RefreshOsd();

	if( ImportantInterrupt == TRUE )
		{
/*
		printf("(ImportantInterrupt): %d-%x-%x-%x\n", pTAVI->ssTaskId, tMsgTemp.ulMsgId, tMsgTemp.usParam1, tMsgTemp.usParam2 );
		MSG_Send( pTAVI->ssTaskId, &tMsgTemp ); 
*/
		return -1; 
		}
	else
		return sx;
}

#define X_RESOLUTION 320
#define MIN_FONT_WIDTH 4
#define MAX_CHAR_ONE_LINE (X_RESOLUTION/MIN_FONT_WIDTH)

U16 CTextEngine_BDF::DrawEUC8LineText( char euc8String[], U16 x, U16 y, U16 color, U16 SubColor, U16 fontsize, U32 option, int* pIsAutoSplitActivated )
{
	U16 uc16String[MAX_CHAR_ONE_LINE];
	memset( uc16String, 0x0000, MAX_CHAR_ONE_LINE*sizeof(U16));

	if( SubColor > 256 )
		SubColor == _SHADOW;

	if( ConvertEUC8StrToUnicode16Str( uc16String, euc8String, MAX_CHAR_ONE_LINE ))
		{
		return ( DrawUC16Text( uc16String, x, y, color, SubColor, fontsize, 292, pIsAutoSplitActivated, option ) );
		}
	else
		{
		printf("(F)\n");
		return false;
		}
}

U16 CTextEngine_BDF::DrawEUC8Text( char euc8String[], U16 x, U16 y, U16 color, U16 fontsize, U32 option )
{
	U16 uc16String[ TEXTVIEWER_BLOCK_BUFFER_SIZE ] = {0};

	if( ConvertEUC8StrToUnicode16Str( uc16String, euc8String ))
		return ( DrawUC16Text( uc16String, x, y, color, fontsize, option ) );
	else
		return false;
}
U16 CTextEngine_BDF::DrawEUC8Text( char euc8String[], U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 option )
{
	U16 uc16String[ TEXTVIEWER_BLOCK_BUFFER_SIZE ] = {0};

	if( ConvertEUC8StrToUnicode16Str( uc16String, trimh(euc8String) ))
		return ( DrawUC16Text( uc16String, x, y, color, color2, fontsize, option ) );
	else
		return false;
}
U16 CTextEngine_BDF::DrawVFATText( char euc8String[], U16 x, U16 y, U16 color, U16 fontsize, U32 option )
{
	U16 uc16String[ TEXTVIEWER_BLOCK_BUFFER_SIZE ] = {0};


	if( ConvertVFATTextToUnicode16Str( uc16String, euc8String ) )
		return ( DrawUC16Text( uc16String, x, y, color, fontsize, option ) );
	else
		return false;

}

int CTextEngine_BDF::GetStartXPosOfTxtBuf() { return tx; }
int CTextEngine_BDF::GetEndXPosOfTxtBuf() { return eotx; }
int CTextEngine_BDF::GetStringWidth() { return tw; }
int CTextEngine_BDF::GetStringWidth( U32 ResIndex, U16 fontsize, U32 option )
{
	return (GetStringWidth( TextRes[ResIndex], fontsize, option ));
}
int CTextEngine_BDF::GetStringWidth( char euc8String[], U16 fontsize, U32 option )
{
	U16 uc16String[ TEXTVIEWER_BLOCK_BUFFER_SIZE ] = {0};

	if( ConvertEUC8StrToUnicode16Str( uc16String, euc8String ))
		return ( GetStringWidth( uc16String, fontsize, option ) );
	else
		return 0;
}

int CTextEngine_BDF::GetStringWidth( U16 usString[], U16 fontsize, U32 option )
{ 
	TOptimizedBDFCharInfo** ppGlyph;
	int StringWidth = 0;
	int StaticWidth=0;
	int AdditionalSpace;
	int AddValueOfMultiDividedString=0;
	U16* usRealString;
	CSize sz;
	int loop=0;
	int StringLength = ustrlen( usString );
	AdditionalSpace=2; // 기본공백은 2

	if( (option&DT_BOLD) == DT_BOLD ) AdditionalSpace++;

	if( (option&DT_SHADOW) == DT_SHADOW ) AdditionalSpace++;
	else if( (option&DT_OUTLINE) == DT_OUTLINE ) AdditionalSpace++;

	if( (option & DT_STATIC_6WIDTH) == DT_STATIC_6WIDTH ) StaticWidth=6;
	else if( (option & DT_STATIC_8WIDTH) == DT_STATIC_8WIDTH ) StaticWidth=8;
	else if( (option & DT_STATIC_10WIDTH) == DT_STATIC_10WIDTH ) StaticWidth=10;
	else if( (option & DT_STATIC_12WIDTH) == DT_STATIC_12WIDTH ) StaticWidth=12;
	else if( (option & DT_STATIC_14WIDTH) == DT_STATIC_14WIDTH ) StaticWidth=14;
	else if( (option & DT_NUMBER_6WIDTH) == DT_NUMBER_6WIDTH ) StaticWidth =6;	
	else if( (option & DT_NUMBER_8WIDTH) == DT_NUMBER_8WIDTH ) StaticWidth =8;
//printf("-GSW:");
	for( ; usString[loop] != 0x0000 ; loop++ )
		{
		if( usString[loop] == 0x0020 ) 
			{
			StringWidth+=4;
//printf("%d(1) ", 4);
			}
		else if( usString[loop] > 0x0020 )
			{
			GetCharSize(usString[loop], (int)fontsize, &sz);
			if(sz.width>16) 
				{
				continue;
				}
			else if( StaticWidth>0 )
				{
				if ( (option & DT_NUMBER_6WIDTH) == DT_NUMBER_6WIDTH ) {
					if (usString[loop] >= 0x0030 && usString[loop]< 0x003B) StringWidth += StaticWidth;
					else StringWidth += (sz.width+AdditionalSpace);
					}
				else if ( (option & DT_NUMBER_8WIDTH) == DT_NUMBER_8WIDTH ) {
					if (usString[loop] >= 0x0030 && usString[loop]< 0x003B) StringWidth += StaticWidth;
					else StringWidth += (sz.width+AdditionalSpace);
					}
				else {
					StringWidth += StaticWidth;
					}
//printf("%d(2) ", StaticWidth);
				}
			else 
				{
				if( loop==StringLength-1 )
					{
					StringWidth += sz.width;
//printf("%d(3) ", sz.width);
					}
				else
					{
					StringWidth += (sz.width+AdditionalSpace);
//printf("%d(4) ", sz.width+AdditionalSpace);
					}
				}
			}

		}
//printf("=%d\n", StringWidth);
	return StringWidth;
}
int CTextEngine_BDF::ConvertEUC8cToU16c( U16* UC16c, char H_EUC8c, char L_EUC8c  )
{
	U16 KeyChar;

	if( H_EUC8c > 0x80 && H_EUC8c > 0x40 )
		{
        	int kc=0;
		switch( ((unsigned char)H_EUC8c)&0xF0 )
        		{
        		case 0x80: kc = 0; break;
        		case 0x90: kc = 2670; break;
        		case 0xA0: kc = 5518; break;
        		case 0xB0: kc = 7942; break;
        		case 0xC0: kc = 10790; break;
         		case 0xD0: kc = 12722; break;
        		case 0xE0: kc = 14226; break;
        		case 0xF0: kc = 15730; break;
			}

		KeyChar = ((unsigned char)H_EUC8c<<8) | (unsigned char)L_EUC8c;
        
		for( ; kc< __KSC5610_MAX_CODE__; kc++ ) // worst case: KSC5610 AUμa ¼o ¸¸A­ loop
			{
			if( KeyChar == ausConvertingTableFromEUC8ToUnicode16[kc][0] )
				{
				*(UC16c) = ausConvertingTableFromEUC8ToUnicode16[kc][1];
                        	break;
				}
			}

		return 2;	// 2bytes consumed

		}

	else if( H_EUC8c <= 0xFF )
		{            	
		*(UC16c) = (U16)H_EUC8c;
		return 1;	// 1byte consumed
		}

	else
		{
		*(UC16c) = (U16)'?';
		return 1;	// 1byte consumed
		}
        
}
int CTextEngine_BDF::ConvertEUC8sToU16s( U16* UC16s, char* EUC8s, int MaxLineLength )
{
	int CurUch = 0;
	int CurCh = 0;
	int nConvCh = 0;

	for( int loop=0; loop<(MaxLineLength-1); loop++ )
		{

		if( EUC8s[CurCh] == '\n' ||EUC8s[CurCh] == '\0' )
			break;

		else if( (nConvCh=ConvertEUC8cToU16c( &(UC16s[CurUch]), EUC8s[CurCh], EUC8s[CurCh+1] )) == 1 )
			{
			CurCh++;
			CurUch++;
			}
		
		else if( nConvCh == 2 )
			{
			CurCh += 2;
			CurUch++;
			}
		
		else
			{
			ASSERT(0);
			break;
			}
		}

	return nConvCh;
}
int CTextEngine_BDF::GetNumOfTokens( char* BlockBuffer, int StartPoint, int BufferSize, int FontSize, int Width, U32 Option )
{
	U16 uc16String[TEXTVIEWER_BLOCK_BUFFER_SIZE] = {0};
//	int StartPointOnUnicodeBuffer;
	DEBUGMSG(1,("GetNumOfTokens(char*)\n"));

	if( ConvertEUC8StrToUnicode16Str( uc16String, &(BlockBuffer[StartPoint])) )
		return GetNumOfTokens( uc16String, 0, BufferSize, FontSize, Width, Option );
	else
		return -1;
}
int CTextEngine_BDF::GetNumOfTokens( uchar_t* UnicodeBlockBuffer, int StartPointOnUnicodeBuffer, int BufferSize, int FontSize, int Width, U32 Option )
{
	int AdditionalSpace	= 1;
	int StringPixelWidth	= 0;
	int NumOfTokens		= 0;	
	TOptimizedBDFCharInfo** ppGlyph;
	CSize sz;	
	
	if( (Option&DT_BOLD) == DT_BOLD ) 		AdditionalSpace += 1;
	if( (Option&DT_SHADOW) == DT_SHADOW ) 			AdditionalSpace += 1;
	else if( (Option&DT_OUTLINE) == DT_OUTLINE )
		{
		AdditionalSpace += 2;
		if( (Option&DT_BOLD) == DT_BOLD ) AdditionalSpace += 2;
		}


	for( int loop=StartPointOnUnicodeBuffer; 	UnicodeBlockBuffer[loop]!=0x000a  && UnicodeBlockBuffer[loop]!=0x0000 && loop<(BufferSize-StartPointOnUnicodeBuffer) ; 	loop++ )
		{
		if( UnicodeBlockBuffer[loop] > 0x0019 )
			{
			sz =  GetCharSize(UnicodeBlockBuffer[loop], 12);
			StringPixelWidth += (sz.width + AdditionalSpace);

			if( (StringPixelWidth+AdditionalSpace) >= Width)
				{
				NumOfTokens++;	
				StringPixelWidth=0; 		// NewLine... Recount;
				}
			}
		}

	return NumOfTokens;

}



int CTextEngine_BDF::GetLastTokenStringIndex( char* BlockBuffer, TTokenStringIndex* pTSI, U32 option )
{
	U16 uc16String[TEXTVIEWER_BLOCK_BUFFER_SIZE] = {0};

	if( pTSI->StartPointOfPage < 0 || 
		pTSI->StartPointOfPage >= TEXTVIEWER_BLOCK_BUFFER_SIZE )
		return 0;

	if( ConvertEUC8StrToUnicode16Str( uc16String, &(BlockBuffer[pTSI->StartPointOfPage]) ))
		return ( GetLastTokenStringIndex( uc16String, pTSI, option ) );
	else
		return 0;
}

int CTextEngine_BDF::GetLastTokenStringIndex( uchar_t usString[], TTokenStringIndex* pTSI, U32 option )
{
	TOptimizedBDFCharInfo** ppGlyph;
	CSize sz;
	int AdditionalSpace;
	int StringPixelWidth;

	// Output
	int LastTokenStringWidth;
	int NumOfTokens;
	int NewCursor;

	DEBUGMSG(1,("GetLastTokenStringIndex()\n"));

	AdditionalSpace 	= 1;
	StringPixelWidth	= 0;
	NumOfTokens		= 0;

	if( (option&DT_BOLD) == DT_BOLD ) 		AdditionalSpace += 1;
	if( (option&DT_SHADOW) == DT_SHADOW ) 			AdditionalSpace += 1;
	else if( (option&DT_OUTLINE) == DT_OUTLINE )
		{
		AdditionalSpace += 2;
		if( (option&DT_BOLD) == DT_BOLD ) AdditionalSpace += 2;
		}

	for( int i=pTSI->StartPointOfPage; 	(i < pTSI->DisplayableWidth) && (usString[i] != 0x0000) && (usString[i] != 0x000A); 		i++, StringPixelWidth++ )
		{
		if( usString[i] > 0x0019 )
			{
			sz =  GetCharSize(usString[i], 12);
			StringPixelWidth += (sz.width + AdditionalSpace);
			//DEBUGMSG(1,("%d\n",TotalStringWidth ));

			if( (StringPixelWidth+AdditionalSpace) >= pTSI->DisplayableWidth)
				{
				NumOfTokens++;	
				StringPixelWidth=0; 		// NewLine... Recount;
				NewCursor = i+1;
				//DEBUGMSG(1,("*** limit_x(%d) Overflow\n", limit_x ));
				//DEBUGMSG(1,("*** LimitOverCount : %d\n", LimitOverCount ));
				}
			}
		}

	pTSI->NumOfTokens				= NumOfTokens;
	pTSI->PixelWidthOfLastTokenString 	= StringPixelWidth;
	pTSI->NewCursor					= NewCursor;
	
	return NewCursor;
	
}



int CTextEngine_BDF::GetDividerEnclosedStringWidth( U32 ResIndex, U16 fontsize, U32 option )
{
	return (GetDividerEnclosedStringWidth( TextRes[ResIndex], fontsize, option ));
}
int CTextEngine_BDF::GetDividerEnclosedStringWidth( U16 usString[], U16 fontsize, U32 option )
{ 
	TOptimizedBDFCharInfo** ppGlyph;
	int StringWidth = 0;
	int MaxStringWidth=0;
	int MaxStringIndicator=-1;
	int AdditionalSpace;
	int AddValueOfMultiDividedString=0;
	U16* usRealString;

//printf("(1)\n");
	switch(fontsize)
		{
		case 16:
			ppGlyph = T16.Glyph;
			AdditionalSpace=1;
			break;
		case 12:
		default:
			ppGlyph = T12.Glyph;
			AdditionalSpace=1;
		}

	AdditionalSpace=1;

	if( (AddValueOfMultiDividedString=GetStringDividerCount(usString)) > 0 )
		{
//printf("(2:AddValueOfMultiDividedString=%d)\n",AddValueOfMultiDividedString);
		for( int i=0; i<=AddValueOfMultiDividedString; i++ )
			{
			if( MaxStringWidth < (StringWidth = GetNthStringWidth( usString, AdditionalSpace, i )) )
				{
				MaxStringWidth = StringWidth;
				MaxStringIndicator = i;
				}

//printf("(%d)MaxStringWidth:%d\n",i,MaxStringWidth);
//printf("(%d)MaxStringIndicator:%d\n",i,MaxStringIndicator);

			
			}
		}
	else
		{
//printf("(3)\n");		
		return( GetStringWidth( usString, fontsize, option ) );
		}

	usRealString  = GetNthStringWithDivider( usString, 0x007c, MaxStringIndicator );
	//usRealString = &(usString[MaxStringIndicator]);
	StringWidth = 0;

	for( int i=0; usRealString[i] != 0x0000 ; i++ )
		{

		if( usRealString[i] == 0x007c )
			break;

		else if( usRealString[i] > 0x0019 )
			{
//printf("(4)\n");
			StringWidth += (ppGlyph[usRealString[i]]->BBw + AdditionalSpace );
			}
		else
			{
//printf("(5)\n");
			;
			}
		}

//	if( (option&DT_SHADOW) == DT_SHADOW ) StringWidth += 1;
//	else if( (option&DT_OUTLINE) == DT_OUTLINE ) StringWidth += 1;

	if( (option&DT_BOLD) == DT_BOLD ) StringWidth += 1;

	return StringWidth;
}

int CTextEngine_BDF::GetNthStringWidth( uchar_t* ucString, int AdditionalSpace, int N )
{
	return ( GetNthStringWidthWithDivider( ucString, 0x007c, AdditionalSpace, N ) );
}

int CTextEngine_BDF::GetNthStringWidthWithDivider( uchar_t* ucString, uchar_t divider, int AdditionalSpace, int N )
{
	TOptimizedBDFCharInfo** ppGlyph;
	int StringWidth = 0;
//	int TotalDivider = 0;
	uchar_t* ucTemp;

/*
	if( (TotalDivider=GetStringDividerCount(ucString)) == 0 )
		{
		ucTemp = ucString[0];
		}

	for( int i=0; i<TotalDivider; i++ )
		{
		GetStringLength
			GetNthStringWithDivider( ucString, divider, i )
		}
*/

	ucTemp = GetNthStringWithDivider( ucString, divider, N );

	for( int i=0; ucTemp[i] != 0x0000 ; i++ )
		{

		if( ucTemp[i] == 0x007c )
			break;

		else if( ucTemp[i] > 0x0019 )
			{
			StringWidth += (ppGlyph[ucTemp[i]]->BBw + AdditionalSpace );
			}
		else
			{
			;
			}
		}

	return StringWidth;
}


int CTextEngine_BDF::GetStringDividerCount( uchar_t* ucString )
{
	int count = 0;

	for(int i=0; i<TEXTVIEWER_BLOCK_BUFFER_SIZE; i++ )
		{
		if( ucString[i] == 0x0000 || ucString[i] == 0x000d || ucString[i] == 0x000a )
			break;
			
		else if( ucString[i] == 0x007c )
			count++;
		}

	return count;
}

uchar_t* CTextEngine_BDF::GetNthStringWithDivider( uchar_t* ucString, uchar_t divider, int N )
{
	int count = 0;
	int locator=0;

	for( ; locator<TEXTVIEWER_BLOCK_BUFFER_SIZE; locator++ )
		{
		if( ucString[locator] == 0x0000 || ucString[locator] == 0x000d || ucString[locator] == 0x000a )
			break;

		else if( count >= N )
			break;
			
		else if( ucString[locator] == divider )
			{
			count++;
			}
		}

	return &(ucString[locator]);

}



uchar_t* CTextEngine_BDF::SetNthStringWithDivider( uchar_t* dest, uchar_t* src, uchar_t divider, int N )
{
	int count = 0;
	int locator=0;
	int d_locator = 0;

	for( ; locator<TEXTVIEWER_BLOCK_BUFFER_SIZE; locator++ )
		{
		if( src[locator] == 0x0000 || src[locator] == 0x000d || src[locator] == 0x000a )
			{
			dest[d_locator] = 0x0000;
			break;
			}

		else if( count >= N+1 )
			{
			break;
			}
			
		else if( src[locator] == divider )
			{
			dest[d_locator] = 0x0000;
			count++;
			locator++;
			}

		if( count == N )
			{
			dest[d_locator] = src[locator];
			d_locator++;
			}
		}

	return dest;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Added Method By Lee Seok

U16 CTextEngine_BDF::DrawCharacter( uchar_t ch, U16 x, U16 y, U16 color, U16 fontsize, U32 option, CPixmap* pixmap )
{
	TOptimizedBDFCharInfo** ppGlyph;

	switch(fontsize) {
	case 16:
		ppGlyph = T16.Glyph;
		break;
	case 12:
	default:
		ppGlyph = T12.Glyph;
	}

	if( pixmap == NULL )
		return DrawEffectCharacter( ppGlyph[ch], x, y, color, 24, fontsize, option );
	else
		return DrawCharacter( ppGlyph[ch], x, y, color, fontsize, option, pixmap );		
}

U16 CTextEngine_BDF::DrawCharacter( TOptimizedBDFCharInfo* ptCurrentCharCode, U16 x, U16 y, U16 color, U16 fontsize, U32 option, 
	CPixmap* pixmap )
{

	U16 	usTemp, us16Digit, usRow, usColum, usABitOf16Bits, usABitOf4Bits;
	U8	ubDigitShiftValue;
	int StaticWidth;

	if(ptCurrentCharCode == 0) return 0;
	if(ptCurrentCharCode->encoding == 0) return 0;
	if(ptCurrentCharCode->encoding < 0x0020) return 0;
	if(ptCurrentCharCode->encoding == 0x0020) return 4;

	if ( (option&DT_NUMBER_6WIDTH) == DT_NUMBER_6WIDTH && (ptCurrentCharCode->encoding >= 0x0030 && ptCurrentCharCode->encoding < 0x003B) ) {
		StaticWidth = 6;
		int xp = (StaticWidth-ptCurrentCharCode->BBw)/2;
		if(xp) x += xp;
		}

	if( ptCurrentCharCode->BBw <= 8 )	// 폭이 8bit 이하
		{
		ubDigitShiftValue = 8;
		}
	else
		{
		ubDigitShiftValue = 0;
		}


	if( fontsize == 16 )
		y += (T16_Y_ORIGIN - (int)ptCurrentCharCode->BBh  + ((-1)*((int)ptCurrentCharCode->BByoff)) );
	else
		y += (T12_Y_ORIGIN - (int)ptCurrentCharCode->BBh  + ((-1)*((int)ptCurrentCharCode->BByoff)) );

	if( (option & DT_TRANSPARENCY) == DT_TRANSPARENCY )
		{
		if( (option & DT_OUTLINE) == DT_OUTLINE )
			{		

			for( usRow = 0 ; usRow < ptCurrentCharCode->BBh ; usRow++ )		
				{
				us16Digit = (ptCurrentCharCode->fontdata[usRow])<<ubDigitShiftValue;
				for( usColum = 0, usABitOf16Bits=0 ; usABitOf16Bits <= ptCurrentCharCode->BBw;  usColum++ )
					{
					usTemp = (us16Digit & us16BitMask[usColum]) >> ubBitShiftRange[usColum];
					for(  usABitOf4Bits=0;  usABitOf4Bits< 4 && usABitOf16Bits <= ptCurrentCharCode->BBw; usABitOf16Bits++, usABitOf4Bits++ )
						{
						if( ( usTemp & ub4BitMask[usABitOf4Bits] ) == ub4BitMask[usABitOf4Bits]  )  
							{
							if( pixmap ) {
								pixmap->SetPixel( x+usABitOf16Bits-1, y+usRow, m_outlinecolor );
								pixmap->SetPixel( x+usABitOf16Bits, y+usRow-1, m_outlinecolor );
								pixmap->SetPixel( x+usABitOf16Bits+1, y+usRow, m_outlinecolor );
								pixmap->SetPixel( x+usABitOf16Bits, y+usRow+1, m_outlinecolor );
							}
							else {
								pMpegDecoder->SetPixel( _SHADOW, x+usABitOf16Bits-1,  y+usRow );
								pMpegDecoder->SetPixel( _SHADOW, x+usABitOf16Bits,  y+usRow-1 );
								pMpegDecoder->SetPixel( _SHADOW, x+usABitOf16Bits+1,  y+usRow );
								pMpegDecoder->SetPixel( _SHADOW, x+usABitOf16Bits,  y+usRow+1 );
							}
							}
						}
	 				}
				}


			for( usRow = 0 ; usRow < ptCurrentCharCode->BBh ; usRow++ )		
				{
				us16Digit = (ptCurrentCharCode->fontdata[usRow])<<ubDigitShiftValue;
				for( usColum = 0, usABitOf16Bits=0 ; usABitOf16Bits <= ptCurrentCharCode->BBw;  usColum++ )
					{
					usTemp = (us16Digit & us16BitMask[usColum]) >> ubBitShiftRange[usColum];
					for(  usABitOf4Bits=0;  usABitOf4Bits< 4 && usABitOf16Bits <= ptCurrentCharCode->BBw; usABitOf16Bits++, usABitOf4Bits++ )
						{
						if( ( usTemp & ub4BitMask[usABitOf4Bits] ) == ub4BitMask[usABitOf4Bits]  )  
							{
							if( pixmap ) pixmap->SetPixel( x+usABitOf16Bits, y+usRow, color );
							else pMpegDecoder->SetPixel( color, x+usABitOf16Bits,  y+usRow );
							}
	 					}
					}
				}

			}
		else
			{

			for( usRow = 0 ; usRow < ptCurrentCharCode->BBh ; usRow++ )		
				{
				us16Digit = (ptCurrentCharCode->fontdata[usRow])<<ubDigitShiftValue;
				for( usColum = 0, usABitOf16Bits=0 ; usABitOf16Bits <= ptCurrentCharCode->BBw;  usColum++ )
					{
					usTemp = (us16Digit & us16BitMask[usColum]) >> ubBitShiftRange[usColum];
					for(  usABitOf4Bits=0;  usABitOf4Bits< 4 && usABitOf16Bits <= ptCurrentCharCode->BBw; usABitOf16Bits++, usABitOf4Bits++ )
						{
						if( ( usTemp & ub4BitMask[usABitOf4Bits] ) == ub4BitMask[usABitOf4Bits]  )
							{
							if( pixmap ) pixmap->SetPixel( x+usABitOf16Bits, y+usRow, color );
							else pMpegDecoder->SetPixel( color, x+usABitOf16Bits,  y+usRow );
							}
						}
	 				}
				}

			}
		}

	else
		{
		for( usRow = 0 ; usRow < ptCurrentCharCode->BBh ; usRow++ )		
			{
			us16Digit = (ptCurrentCharCode->fontdata[usRow])<<ubDigitShiftValue;
			for( usColum = 0, usABitOf16Bits=0 ; usABitOf16Bits <= ptCurrentCharCode->BBw;  usColum++ )
				{
				usTemp = (us16Digit & us16BitMask[usColum]) >> ubBitShiftRange[usColum];
				for(  usABitOf4Bits=0;  usABitOf4Bits< 4 && usABitOf16Bits <= ptCurrentCharCode->BBw; usABitOf16Bits++, usABitOf4Bits++ )
					{
					if( ( usTemp & ub4BitMask[usABitOf4Bits] ) == ub4BitMask[usABitOf4Bits]  ) 
						{ 
						if( pixmap ) pixmap->SetPixel( x+usABitOf16Bits, y+usRow, color );
						else pMpegDecoder->SetPixel( color, x+usABitOf16Bits,  y+usRow );
						}
					else  
						{
						if( pixmap ) pixmap->SetPixel( x+usABitOf16Bits, y+usRow, 0 );
						else pMpegDecoder->SetPixel( 0, x+usABitOf16Bits,  y+usRow );
						}
					}
				}
			}
		}

	if ( (option&DT_NUMBER_6WIDTH) == DT_NUMBER_6WIDTH && (ptCurrentCharCode->encoding >= 0x0030 && ptCurrentCharCode->encoding < 0x003B) ) {
		StaticWidth = 6;
		return StaticWidth;
		}
	else
		return (usABitOf16Bits);

}
U16 CTextEngine_BDF::DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 fontsize, U32 option, CPixmap* pixmap )
{
	int	vx			= 0;
	int	vy			= 0;
	int	rx			= (int)x;		// Start Point of Screen 
	int	ry			= (int)y;
	U16	usIndex		= 0;
	int	eovy		= 16;
	U16 ret;
	int limited_x	= __LCD_DISPLAY_WIDTH;	// default = unlimited.

	TOptimizedBDFCharInfo** ppGlyph;

	switch(fontsize)
		{
		case 16:
			ppGlyph = T16.Glyph;
			break;
		case 12:
		default:
			ppGlyph = T12.Glyph;
		}

	if( (option & DT_SCROLLTEXT) == DT_SCROLLTEXT )
		{
		limited_x = (__LCD_DISPLAY_WIDTH-38-x);
		InitTextScrollBuffer();


		for( usIndex=0; usString[usIndex] != '\0' ; usIndex++ )
			{
			ret = DrawCharacterOnScrollBuffer( ppGlyph[ usString[usIndex] ], rx, ry, vx, &vy, color, fontsize, option );

			rx += ret;
			vx += ret;
			}

		InitScrollText( x, y, limited_x, 0, vx, MIN(vy,eovy) );

		}
	else
		{
		if( (option & DT_TEXTVIEWER) == DT_TEXTVIEWER )
			{
			limited_x = __LCD_DISPLAY_WIDTH-9;
			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				{
				if( usString[usIndex] == '\n' )
					{
					return usIndex+1;
					break;
					}
				else
					rx += DrawCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, fontsize, option );
				}

			}

		else if( (option & DT_FILEBROWSER) == DT_FILEBROWSER )
			{
			limited_x = (__LCD_DISPLAY_WIDTH-x);
			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				{
				rx += DrawCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, fontsize, option );
				}
			}

		else
			{
			limited_x = (__LCD_DISPLAY_WIDTH-9);
			for( usIndex=0; usString[usIndex] != '\0' && rx<limited_x; usIndex++ )
				{
				rx += DrawCharacter( ppGlyph[ usString[usIndex] ], rx, ry, color, fontsize, option, pixmap );
				}
			}
		}

	return usIndex;
}


CSize CTextEngine_BDF::GetStringSize( const uchar_t* psz, int nSzLen, int nFontSize )
{
	CSize uzSize;
	int space=2;
	int nMaxHeight = 0;
	TOptimizedBDFCharInfo** ppGlyph;

	if( nFontSize == 12 ) ppGlyph = T12.Glyph;
	else if( nFontSize == 16 ) ppGlyph = T16.Glyph;
	else return CSize(0,0);

	uzSize.height = nFontSize;
	uzSize.width = 0;
	for( int i=0; i<nSzLen; i++ ) {
		if( psz[i] == 0 ) break;
            if( psz[i] == 0x20 ) {
                uzSize.width += 4;
            } else {
                if( ppGlyph[psz[i]]->BBw > 20 ) uzSize.width += 7+1;
                else uzSize.width += ppGlyph[psz[i]]->BBw+space; // add space: 2
            }
		//printf( "ppGlyph[0x%x]->BBw: %d\n", psz[i], ppGlyph[psz[i]]->BBw );
		//if( ppGlyph[psz[i]]->BBh > uzSize.height ) uzSize.height = ppGlyph[psz[i]]->BBh;
	}
	return uzSize;
}


CSize CTextEngine_BDF::GetCharSize( uchar_t ch, int nFontSize )
{
	CSize uzSize;
	int nMaxHeight = 0;
	TOptimizedBDFCharInfo** ppGlyph;

	if( ch == 0x0020 ) return CSize(4,0);
	else if( nFontSize == 12 ) ppGlyph = T12.Glyph;
	else if( nFontSize == 16 ) ppGlyph = T16.Glyph;
	else return CSize(0,0);

	uzSize.width = ppGlyph[ch]->BBw;
	uzSize.height = ppGlyph[ch]->BBh;

	return uzSize;
}

CSize* CTextEngine_BDF::GetCharSize( uchar_t ch, int nFontSize, CSize* pSz )
{
	int nMaxHeight = 0;
	TOptimizedBDFCharInfo** ppGlyph;

	if( ch == 0x0020 ) 
		{
		pSz->width = 4;
		pSz->height = 0;
		return pSz;
		}
	else if( nFontSize == 12 ) ppGlyph = T12.Glyph;
	else if( nFontSize == 16 ) ppGlyph = T16.Glyph;
	else 
		{
		ASSERT(0);
		pSz->width = 0;
		pSz->height = 0;
		return pSz;
		}

	pSz->width = ppGlyph[ch]->BBw;
	pSz->height = ppGlyph[ch]->BBh;
	return pSz;
}



int CTextEngine_BDF::GetBoundaryLength( uchar_t* pwc, int nLength, int nFontSize, int option, int nBoundaryWidth )
//int CTextEngine_BDF::GetBoundaryLength( uchar_t* pwc, int nLength, int nFontSize, int nBoundaryWidth )
{
	TOptimizedBDFCharInfo** ppGlyph;
	int width = 0;
	int i;
	int spacing=2;

       #if 0
	if( (option&DT_BOLD) == DT_BOLD )
		spacing += 1;

	if( (option&DT_SHADOW) == DT_SHADOW )
		{
		spacing += 1;
		}
	else if( (option&DT_OUTLINE) == DT_OUTLINE )
		{
		spacing += 1;
		}
        #endif
	if( nFontSize == 12 ) ppGlyph = T12.Glyph;
	else if( nFontSize == 16 ) ppGlyph = T16.Glyph;
	else ppGlyph = T12.Glyph;

	for( i=0; i<nLength; i++ ) {
		if( ppGlyph[pwc[i]]->encoding == 0x0020 ) width += 4;
		else if( ppGlyph[pwc[i]]->BBw > 20 ) width += 7+spacing;
		else width += ppGlyph[pwc[i]]->BBw+spacing;
		if( width > nBoundaryWidth ) break;
	}

	return i;
}




#if 0
//#ifdef _CONSOLE_MODE_
int
main( int argc, char **argv)
{
	char temp[10];
	CTextEngine_BDF* pT12EBDF;
	int fontsize;
	
	printf("\nTAVI BDF Test\n\n");



	if( argc > 1 ) printf( "- Argument 1 : [%s]\n", argv[1] );		// font
	if( argc > 2 ) printf( "- Argument 2 : [%s]\n", argv[2] );		// size
	else
		return 0;

	
	
	//pT12EBDF = new CTextEngine_BDF( "./b12.bdf", 12 );

	fontsize = atoi( argv[2] );
	pT12EBDF = new CTextEngine_BDF( argv[1], fontsize );

	printf("\n\n");

	for( ;; )
		{

		printf("input : ");
		scanf( "%s",  &temp );

		if( strlen(temp) != 4 )
			{
			printf( "strlen(temp) : [%d]\n", strlen(temp) );
			printf( "value : [%s]\n\n", temp );
			continue;
			}
			
		printf("input value : [0x%s]\n", temp );
		U16 u16char = pT12EBDF->Convert4digitBCDStringToHexCode( (U8*)temp );
		printf("convert value : [0x%0X]\n", u16char );
		pT12EBDF->DebugPrintAFontMemory(  fontsize,  u16char );
		
		printf("-------------------------------------------\n");

		}

	return 1;
}


#endif

 
