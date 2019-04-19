//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Image Related Library Source
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibImage.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:08:26 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibImage.cpp,v $
// Revision 1.2  2006/06/07 08:08:26  zstein
// - removed unused codes.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.77  2006/01/31 12:44:48  zstein
// - changed fs_xxx to fs_fat_xxx.
//
// Revision 1.76  2005/12/28 05:14:38  ywkim
// Add instruction of RELOAD_WATCHDOGTIMER_9SEC in DrawStartupProgress()
//
// Revision 1.75  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.74  2005/12/05 03:53:09  etnlwind
// Update for DrawBottomGuidePannel BugFix
//
// Revision 1.73  2005/12/05 03:07:44  etnlwind
// Update for DrawBottomGuidePannel
//
// Revision 1.72  2005/12/03 03:52:07  etnlwind
// Update for FileBrowser Bottom Menu
//
// Revision 1.71  2005/12/02 11:06:13  etnlwind
// Update for SoundPalette UI
//
// Revision 1.70  2005/12/01 09:32:55  etnlwind
// Update for LibSoundPalette
//
// Revision 1.69  2005/11/22 05:42:52  ywkim
// Change parameter of DrawStartupProgress() for progress
//
// Revision 1.68  2005/11/21 12:00:29  etnlwind
// Update for AlarmClock & Sound Palette UI
//
// Revision 1.67  2005/11/21 02:51:32  etnlwind
// Update for EQ Select UI
//
// Revision 1.66  2005/11/11 10:24:26  etnlwind
// Update for DIV3 Video Play/Stop Problems
//
// Revision 1.65  2005/11/03 03:14:33  etnlwind
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
// Revision 1.64  2005/10/28 11:15:15  etnlwind
// Update for Palette
//
// Revision 1.63  2005/10/22 05:07:28  etnlwind
// Udpate for GetASCIITextSize
//
// Revision 1.62  2005/10/22 02:31:48  etnlwind
// Update for DRAW_ASCII_TEXT
//
// Revision 1.61  2005/10/22 02:09:33  etnlwind
// Update for ALARM_DIGIT
//
// Revision 1.60  2005/10/21 09:43:56  etnlwind
// Update for TextViwer Scroll, Booting Progress
//
// Revision 1.59  2005/10/21 05:35:34  etnlwind
// Update for Booting Progress
// Update for Booting Progress, Alarm MsgBox
//
// Revision 1.58  2005/10/19 09:48:09  etnlwind
// Update for Startup Progress Display
//
// Revision 1.57  2005/10/14 11:39:38  etnlwind
// Update for TextViwer
//
//*****************************************************************************


/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * LibImage.cpp
 * Sang-jin Ryu (etnlwind@newmedialife.com)
 *
 */

#include "LibImage.h"
#include "LibDefines.h"

extern CControlPannel* pCp;
extern CTextEngine_BDF* pT;



const int stDigit3[] = {
	2,	// BLANK
	4,	// .
	6,	// /
	6,	// 0
	3,	// 1
	6,	// 2
	6,	// 3
	6,	// 4
	6,	// 5
	6,	// 6
	7,	// 7
	6,	// 8
	6,	// 9
	4	// :	
};

const int stAlarmDigit[16] = {
	12,	// ' '
	12,	// .
	26,  // Mhz
	25,  // 0
	14,  // 1
	24,  // 2
	
	25,  // 3
	26,  // 4
	25,  // 5
	25,  // 6
	22,  // 7

	25,  // 8
	25,  // 9
	18,  // :
	26,  // AM
	26  // PM	
};

const int stChno[17] = {
	28,	// -
	4,	// SPACE
	18,	// .
	36,  // Mhz
	28,  // 0
	
	28,  // 1
	28,  // 2
	28,  // 3
	28,  // 4
	28,  // 5

	28,  // 6
	28,  // 7
	28,  // 8
	28,  // 9
	10,  // :

	34,  // AM
	34  // PM
};


const int stEqFontWidth[] = {
2,// ' '
3,// A
3,//B
3,//C
3,//D
3,//E
3,//F
3,//G
3,//H
3,//I
3,//J
3,//K
3,//L
3,//M
3,//N
3,//O
3,//P
3,//Q
3,//R
3,//S
3,//T
3,//U
3,//V
3,//W
3,//X
3,//Y
3,//Z
3,// 1
3,// 2
3,// 3
3,// 3
3,// 5
3,// 6
3,// 7
3,// 8
3,// 9
3,//0
3,//.
4//&
};

RMuint8 rgb_Y(RMuint8 r , RMuint8 g , RMuint8 b)
{
     RMint32 f=257*(RMint32)r + 504*(RMint32)g + 98*(RMint32)b + 16000;

     if(f > 255000) f=255000;
     return f/1000;
}

RMuint8 rgb_U(RMuint8 r , RMuint8 g , RMuint8 b)
{
     RMint32 f=-148*(RMint32)r -291*(RMint32)g + 439*(RMint32)b + 128000;

     if(f < 0)      f=0;
     if(f > 255000) f=255000;
     return f/1000;
}

RMuint8 rgb_V(RMuint8 r , RMuint8 g , RMuint8 b)
{
     RMint32 f=439*(RMint32)r -368*(RMint32)g -71*(RMint32)b + 128000;

     if(f < 0)      f=0;
     if(f > 255000) f=255000;

     return f/1000;
}

int 
AllocBitmap256( char * file, BITMAPFILEINFO256* bitmapheader )
{
	return AllocBitmap256_FATFs( file, bitmapheader );
}

#define PRINT_DBG0 1
#define PRINT_DBG 0

static int 
AllocBitmap256_FATFs( char * file, BITMAPFILEINFO256* bitmapheader )
{

	RMuint32 fd;
	unsigned char * buffer;
	int szPaletteArea, cnt, szImageBuffer;
//	int padding_loop, padding;
	
#if PRINT_DBG0
	DEBUGMSG(1,("LibImage(): AllocBitmap256():file[%s]\n", file));
#endif
	bitmapheader->palette     		= NULL;
	bitmapheader->image_buffer 	= NULL;
  
	// bmp open
	fd = fs_fopen(file, 0 );


	// bitmapfileheader process
	cnt = 0;	
	cnt = fs_fat_fread( fd, &bitmapheader->bitmapfileheader, sizeof(BITMAPFILEHEADER), 0 );
#if PRINT_DBG
	DEBUGMSG(1,("-fs_fread(bitmapfileheader) cnt : [%d]\n", cnt ));	
	DEBUGMSG(1,("-bitmapfileheader.bfType:[0x%x]\n", bitmapheader->bitmapfileheader.bfType));
	DEBUGMSG(1,("-bitmapfileheader.bfSize:[0x%x]\n", bitmapheader->bitmapfileheader.bfSize));
	DEBUGMSG(1,("-bitmapfileheader.bfReserved1:[0x%x]\n", bitmapheader->bitmapfileheader.bfReserved1));
	DEBUGMSG(1,("-bitmapfileheader.bfReserved2:[0x%x]\n", bitmapheader->bitmapfileheader.bfReserved2));
	DEBUGMSG(1,("-bitmapfileheader.bfOffBits:[0x%x]\n", bitmapheader->bitmapfileheader.bfOffBits));
#endif
	if(bitmapheader->bitmapfileheader.bfType != 0x4d42)
		goto ALLOC_BITMAP256_ERROR;


	// bitmapinfoheader process
	cnt = 0;
	cnt = fs_fat_fread( fd, &bitmapheader->bitmapinfoheader, sizeof(BITMAPINFOHEADER), 0 );
#if PRINT_DBG
	DEBUGMSG(1,("-fs_fread(bitmapinfoheader) cnt : [%d]\n", cnt ));	
	DEBUGMSG(1,("-bitmapinfoheader.biSize:[0x%x]\n", bitmapheader->bitmapinfoheader.biSize));
	DEBUGMSG(1,("-bitmapinfoheader.biWidth:[0x%x]\n", bitmapheader->bitmapinfoheader.biWidth));	
	DEBUGMSG(1,("-bitmapinfoheader.biHeight:[0x%x]\n", bitmapheader->bitmapinfoheader.biHeight));
	DEBUGMSG(1,("-bitmapinfoheader.biPlanes:[0x%x]\n", bitmapheader->bitmapinfoheader.biPlanes));
	DEBUGMSG(1,("-bitmapinfoheader.biBitCount:[0x%x]\n", bitmapheader->bitmapinfoheader.biBitCount));
	DEBUGMSG(1,("-bitmapinfoheader.biCompression:[0x%x]\n", bitmapheader->bitmapinfoheader.biCompression));	
	DEBUGMSG(1,("-bitmapinfoheader.biSizeImage:[0x%x]\n", bitmapheader->bitmapinfoheader.biSizeImage));
	DEBUGMSG(1,("-bitmapinfoheader.biXPelsPerMeter:[0x%x]\n", bitmapheader->bitmapinfoheader.biXPelsPerMeter));
	DEBUGMSG(1,("-bitmapinfoheader.biYPelsPerMeter:[0x%x]\n", bitmapheader->bitmapinfoheader.biYPelsPerMeter));
	DEBUGMSG(1,("-bitmapinfoheader.biClrUsed:[0x%x]\n", bitmapheader->bitmapinfoheader.biClrUsed));	
	DEBUGMSG(1,("-bitmapinfoheader.biCirImportant:[0x%x]\n", bitmapheader->bitmapinfoheader.biCirImportant));
#endif

	// palette process
	szPaletteArea = (bitmapheader->bitmapfileheader.bfOffBits - 54);
	bitmapheader->palette = (RGBQUARD *)malloc( szPaletteArea );

	if(bitmapheader->palette == NULL)
		goto ALLOC_BITMAP256_ERROR;

	cnt = 0;
	cnt = fs_fat_fread( fd, bitmapheader->palette, szPaletteArea, 0 );
#if PRINT_DBG
	DEBUGMSG(1,("-szPaletteArea : [%d]\n", szPaletteArea ));
	DEBUGMSG(1,("-fs_fread(palette) cnt : [%d]\n", cnt ));
#endif

#if 0
	  if( bitmapheader->bitmapinfoheader.biWidth%4 ) 
		padding = 4 - bitmapheader->bitmapinfoheader.biWidth%4;
	  else 
	  	padding = 0;
#endif

	// image buffer process
	szImageBuffer = 0;
//	szImageBuffer = (bitmapheader->bitmapinfoheader.biWidth + padding) * bitmapheader->bitmapinfoheader.biHeight;
	szImageBuffer = bitmapheader->bitmapinfoheader.biWidth  * bitmapheader->bitmapinfoheader.biHeight;
#if PRINT_DBG
	DEBUGMSG(1,("-szImageBuffer : [%d] \n", szImageBuffer ));
#endif
	bitmapheader->image_buffer = (unsigned char *)malloc( szImageBuffer );
	
	if( bitmapheader->image_buffer == NULL )
		goto ALLOC_BITMAP256_ERROR;
 
	buffer = bitmapheader->image_buffer + szImageBuffer;

//	printf("(((((((((((((((((((( padding : %d )))))))))))))))))))))\n", padding );

	cnt=0;
	for(unsigned int idx =0 ; idx <bitmapheader->bitmapinfoheader.biHeight;idx++)
		{
		buffer -= bitmapheader->bitmapinfoheader.biWidth;
		cnt+=fs_fat_fread( fd, buffer, (bitmapheader->bitmapinfoheader.biWidth), 0 );

		#if 0
		for( padding_loop=0; padding_loop<padding; padding_loop++ ) 
			*(buffer+bitmapheader->bitmapinfoheader.biWidth+padding_loop) = 0x00;

		if( padding ) 
			fatfs_fseek( fd, padding, SEEK_CUR );
		#endif
		}
#if PRINT_DBG
	DEBUGMSG(1,("-fs_fread(buffer) cnt : [%d]\n", cnt ));
#endif
	
	// bitmap close
	fs_fat_fclose( fd, 0 );
	return 0; 


ALLOC_BITMAP256_ERROR:
	DEBUGMSG(1,("ALLOC_BITMAP256_ERROR:  \n"));

	if(bitmapheader->image_buffer != NULL) 
		{
		free(bitmapheader->image_buffer);
		bitmapheader->image_buffer=NULL;
		}

	if(bitmapheader->palette != NULL)
		{
		free(bitmapheader->palette);
		bitmapheader->palette=NULL;
		}
 
	fs_fat_fclose( fd, 0 );
	return (-1);
}



/////////////////////////////////////////////////////
// Drawing Functions

RMuint16 DrawItem( BITMAPFILEINFO256* pBitmap, RMuint16 usPictureID, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight )
{
	pMpegDecoder->RenderOsdBits( 
		pBitmap->image_buffer+(usItemWidth*usItemHeight*usPictureID), usX, usY, usItemWidth, usItemHeight);

//	printf("DrawItem: %d %d %d %d %d\n", (usItemWidth*usY*usPictureID), usX, usY, usItemWidth, usItemHeight );
	
	return TRUE;
}

RMuint16 DrawItemWithoutRefresh( BITMAPFILEINFO256* pBitmap, 
	RMuint16 usPictureID, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight )
{
	DEBUGMSG(0,	("DrawItemWithoutRefresh():(0)\n"));

	pMpegDecoder->RenderOsdBits_WithoutRefresh( 
		pBitmap->image_buffer+(usItemWidth*usItemHeight*usPictureID), usX, usY, usItemWidth, usItemHeight);

	DEBUGMSG(0,	("DrawItemWithoutRefresh():(1)\n"));
//	printf("DrawItem: %d %d %d %d %d\n", (usItemWidth*usY*usPictureID), usX, usY, usItemWidth, usItemHeight );

	return TRUE;
}

RMuint16 DrawItemWithoutRefreshSkipTransparency( BITMAPFILEINFO256* pBitmap, 
	RMuint16 usPictureID, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight )
{

	pMpegDecoder->RenderOsdBits_WithoutRefresh_SkipTransparency( 
		pBitmap->image_buffer+(usItemWidth*usItemHeight*usPictureID), usX, usY, usItemWidth, usItemHeight);

	return TRUE;
}


RMuint16 DrawBox( RMuint8 color, RMuint16 sx, RMuint16 sy, RMuint16 width, RMuint16 height )
{
DEBUGMSG(0,	("DrawBox():(0)\n"));
	pMpegDecoder->RenderOsdBox( color, sx, sy, width, height );
DEBUGMSG(0,	("DrawBox():(1)\n"));
	return TRUE;
}
RMuint16 DrawBoxWithoutRefresh( RMuint8 color, RMuint16 sx, RMuint16 sy, RMuint16 width, RMuint16 height )
{
DEBUGMSG(0,	("DrawBoxWithoutRefresh():(0)\n"));
	pMpegDecoder->RenderOsdBox_WithoutRefresh( color, sx, sy, width, height );
DEBUGMSG(0,	("DrawBoxWithoutRefresh():(1)\n"));
	return TRUE;
}
RMuint16 DrawPixelWithoutRefresh( RMuint8 color, RMuint16 sx, RMuint16 sy )
{
	pMpegDecoder->SetPixel_SkipTransparency( color, sx, sy );
	return TRUE;
}



RMuint16 DrawASCIIChar( char code, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight )
{
	if( code == 32 )
		return ( usX + (usItemWidth/2) - 5 );	
	
	else if( code < 33 || code > 126 )	// 33 = !, 126 = ~
		code = 63-33;			// 63 = ?

	else
		code -= 33;

	pMpegDecoder->RenderOsdBits_WithoutRefresh (
		pBitmap->image_buffer+(usItemWidth*usItemHeight*code), usX, usY, usItemWidth, usItemHeight );


	return 0;
		
}


RMuint16 DrawASCIIChar_SkipTransparency( char code, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight )
{
	if( code == 32 )
		return ( usX + (usItemWidth/2) - 5 );	
	else if( code < 33 || code > 126 )	// 33 = !, 126 = ~
		code = 63-33;			// 63 = ?
	else
		code -= 33;

	pMpegDecoder->RenderOsdBits_WithoutRefresh_SkipTransparency( 
		pBitmap->image_buffer+(usItemWidth*usItemHeight*code), usX, usY, usItemWidth, usItemHeight );

	return 0;
		
}

RMuint16 DrawDigit3( int code, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight )
{
	if( code < 46 || code > 58 )	// 48 = 0, 57 = 9, 58 = :, 
		return ( usX + (usItemWidth/2) - 5 );	
	else
		code -= 45;

	pMpegDecoder->RenderOsdBits_WithoutRefresh (
		pBitmap->image_buffer+(usItemWidth*usItemHeight*code), usX, usY, usItemWidth, usItemHeight );

	return (usX + stDigit3[code]);
 }

RMuint16 DrawAlarmDigit( int code, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight )
{
	int idx;

	if(code == 0x20 ) 					idx = 0;
	else if( code == 0x2e )				idx = 1;
	else if( code >= '0' && code < ':'+1)	idx = code-'0'+3;
	else if( code == 'A' )					idx = 14;
	else if( code == 'P' )					idx = 15;
	else return 0;
		
	pMpegDecoder->RenderOsdBits_WithoutRefresh_SkipTransparency (
		pBitmap->image_buffer+(usItemWidth*usItemHeight*idx), usX, usY, usItemWidth, usItemHeight );

	return (usX + stAlarmDigit[idx]);
 }

#define _FIXED_EQ_FONT_WIDTH 4
RMuint16 DrawEqDigit( int code, RMuint8 color, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 usItemWidth, RMuint16 usItemHeight )
{
	int idx;

	if( code == 0x20 )					idx = 0;
	else if( code >= 'A' && code < 'Z'+1	 ) 	idx = code - 65 + 1;
	else if( code >= '1' && code < '9'+1 )	idx = code - 22;
	else if( code == '0' )					idx = 36;
	else if( code == '.' )					idx = 37;
	else if( code == '&' )					idx = 38;
	else return 0;
		
	pMpegDecoder->RenderOsdBits_WithoutRefresh_SkipTransparency_WithVariableFontColor (
		pBitmap->image_buffer+(usItemWidth*usItemHeight*idx), color, usX, usY, usItemWidth, usItemHeight );

	return (usX +stEqFontWidth[idx]);
}

RMuint16 DrawASCIITextWithoutRefresh( char text[], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 type )
{
	return _DrawASCIITextWithoutRefresh( text, fontWidth, fontHeigth, pBitmap, usX, usY, type, 0 );
}

RMuint16 _DrawASCIITextWithoutRefresh( char text[], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 type, RMuint8 color )
{
	int i;
	int text_length = strlen(text);

	if( type == EQ_DIGIT )
		{
		for( i=0; i < text_length; i++ )
			usX = 1 + DrawEqDigit( text[i], color, pBitmap, usX, usY, fontWidth, fontHeigth );
		}	
	else if( type == __TEXT )
		{
		for( i=0; i < text_length&&usX<300; i++ )
			usX = 1 + DrawASCIIChar( text[i], pBitmap, usX, usY, fontWidth, fontHeigth );  //DrawASCIIChar( text[i], pBitmap, usX, usY, fontWidth, fontHeigth, &fptFONT);
		}
	else if( type == __HSCROLL_TEXT )
		{
		for( i=0; i < text_length&&usX<300; i++ )
			usX = 1 +DrawASCIIChar( text[i], pBitmap, usX, usY, fontWidth, fontHeigth );  //DrawASCIIChar( text[i], pBitmap, usX, usY, fontWidth, fontHeigth, &fptFONT);
		}
	else if( type == __SKIP_TRANSPARENCY_COLOR )
		{
		for( i=0; i < text_length&&usX<280; i++ )
			usX = 1 + DrawASCIIChar_SkipTransparency( text[i], pBitmap, usX, usY, fontWidth, fontHeigth );  //DrawASCIIChar( text[i], pBitmap, usX, usY, fontWidth, fontHeigth, &fptFONT);
		}
	else if( type == __DIGIT3 )
		{
		for( i=0; i < text_length; i++ )
			usX = 1 + DrawDigit3( text[i], pBitmap, usX, usY, fontWidth, fontHeigth );
		}	
	
	else if( type == ALARM_DIGIT )
		{
		for( i=0; i < text_length; i++ )
			usX = 4 + DrawAlarmDigit( text[i], pBitmap, usX, usY, fontWidth, fontHeigth );
		}	



	return TRUE;
}


RMuint16 DrawASCIIText( char text[], RMuint16 fontWidth, RMuint16 fontHeigth, BITMAPFILEINFO256* pBitmap, RMuint16 usX, RMuint16 usY, RMuint16 type)
{
	int i;
	int text_length = strlen(text);

//DEBUGMSG(1,("text : [%s]\n", text));
//DEBUGMSG(1,("text : [%d]\n", text_length));

	if( type == __TEXT )
		{
		for( i=0; i < text_length; i++ )
			usX = 1 + DrawASCIIChar( text[i], pBitmap, usX, usY, fontWidth, fontHeigth);
		}
	else if( type == __DIGIT3 )
		{
		for( i=0; i < text_length; i++ )
			usX = 1 + DrawDigit3( text[i], pBitmap, usX, usY, fontWidth, fontHeigth );
		}	
	else if( type == ALARM_DIGIT )
		{
		for( i=0; i < text_length; i++ )
			usX = 1 + DrawAlarmDigit( text[i], pBitmap, usX, usY, fontWidth, fontHeigth );
		}		

	pMpegDecoder->RefreshOsd ();

	return TRUE;
	
}


#define D4sx ((320/2)-(64/2))
#define D4sy ((240/2)-(64/2))
static int D4step, D4flag;

RMuint32 DrawLoadingProgressV4( RMuint32 option, void *context )
{
	if( option == 5000)
		{
		D4step = 0;
		D4flag = 0;
		DrawItemWithoutRefresh( &TAVI_IMAGES[LOADING_ANIMATION], D4step, D4sx, D4sy,64,64 );
		pMpegDecoder->RefreshOsd();
		}

	else if( option == 4000 )
		{
		if( D4step >= 7 ) D4step = 0;
		else D4step++;
		DrawItemWithoutRefresh( &TAVI_IMAGES[LOADING_ANIMATION], D4step, D4sx, D4sy,64,64 );
		pMpegDecoder->RefreshOsd();
		}

	else if( (option%=100)%10 == 0 )
		{
		if( D4flag == 0 )
			D4flag = 1;

		if( D4step >= 7 ) D4step = 0;
		else D4step++;
		DrawItemWithoutRefresh( &TAVI_IMAGES[LOADING_ANIMATION], D4step, D4sx, D4sy,64,64 );
		pMpegDecoder->RefreshOsd();
		}
		
	return 0;
}


#define TIME_Y_POS				229
int DrawPlayTime( TAVI_TIME* playtime, eOSD_REFRESH_OPTION RefreshOption )
{
	if( pCp->GetCurrentStatus() != _CONTROL_PANNEL_PROGRESS_MODE
		&& pCp->GetCurrentStatus() != _CONTROL_PANNEL_SEEK_MODE )
		return FALSE;

	char playtime_buffer[20];

	if( playtime->hours == 0 )
		{
		if(playtime->minutes < 10 )	sprintf( playtime_buffer, "0%d", playtime->minutes);
		else							sprintf( playtime_buffer, "%d", playtime->minutes);

		if(playtime->seconds < 10 )	sprintf( playtime_buffer+2, ":0%d ", playtime->seconds);
		else							sprintf( playtime_buffer+2, ":%d ", playtime->seconds);
		}

	else
		{
		if(playtime->hours < 10 ) 		sprintf( playtime_buffer, "0%d", playtime->hours);
		else 						sprintf( playtime_buffer, "%d", playtime->hours);
		
		if(playtime->minutes < 10 )	sprintf( playtime_buffer+2, ":0%d", playtime->minutes);
		else							sprintf( playtime_buffer+2, ":%d", playtime->minutes);

		if(playtime->seconds < 10 )	sprintf( playtime_buffer+5, ":0%d ", playtime->seconds);
		else							sprintf( playtime_buffer+5, ":%d ", playtime->seconds);
		}
	DrawBoxWithoutRefresh( _CP_BASE_COLOR_PALLETTE_INDEX, 32, TIME_Y_POS, 50, 8);
	DrawASCIITextWithoutRefresh( playtime_buffer, 8, 7, &TAVI_IMAGES[FONT_DIGIT3], 32, TIME_Y_POS, __DIGIT3);
	DEBUGMSG(1,("-%d(%s)\n", pCp->GetCurrentStatus(), playtime_buffer));

	if( RefreshOption == OSD_REFRESH )
		pMpegDecoder->RefreshOsd();
		
	return TRUE;
}

int DrawTotalTime( TAVI_TIME* totaltime, eOSD_REFRESH_OPTION RefreshOption )
{

	char totaltime_buffer[20];

	if( totaltime->hours == 0 )
		{
		if(totaltime->minutes < 10 )			sprintf( totaltime_buffer, "0%d", totaltime->minutes);
		else									sprintf( totaltime_buffer, "%d", totaltime->minutes);

		if(totaltime->seconds < 10 )			sprintf( totaltime_buffer+2, ":0%d", totaltime->seconds);
		else									sprintf( totaltime_buffer+2, ":%d ", totaltime->seconds);

		DrawASCIITextWithoutRefresh( totaltime_buffer, 8, 7, &TAVI_IMAGES[FONT_DIGIT3], 256, TIME_Y_POS, __DIGIT3 );
		}

	else
		{
		if(totaltime->hours < 10 )				sprintf( totaltime_buffer, "0%d", totaltime->hours);
		else									sprintf( totaltime_buffer, "%d", totaltime->hours);
		
		if(totaltime->minutes < 10 )			sprintf( totaltime_buffer+2, ":0%d", totaltime->minutes);
		else									sprintf( totaltime_buffer+2, ":%d", totaltime->minutes);

		if(totaltime->seconds < 10 )			sprintf( totaltime_buffer+5, ":0%d", totaltime->seconds);
		else									sprintf( totaltime_buffer+5, ":%d ", totaltime->seconds);

		DrawASCIITextWithoutRefresh( totaltime_buffer, 8, 7, &TAVI_IMAGES[FONT_DIGIT3], 255+5-18, TIME_Y_POS, __DIGIT3 );		
		}

	if( RefreshOption == OSD_REFRESH )
		pMpegDecoder->RefreshOsd();

	return TRUE;
}


int DrawMessageBox( char Title[], char Contents[], int sx, int sy, int width, int height )
{
	return FALSE;
}


#define GUIDE_BG_HEIGHT 22
#define BTN_X_SHIFT_VALUE 9
#define BTN_Y_SHIFT_VALUE 5
#define BTN_Y (240-GUIDE_BG_HEIGHT+BTN_Y_SHIFT_VALUE)
#define KEYINDEX_IMAGE_WIDTH 24
#define KEYINDEX_IMAGE_HEIGHT 14
#define KEYINDEX_IMAGE_SPACE 6
#define ITEM_SPACE 11
void DrawBottomGuideBGPannel( eOSD_REFRESH_OPTION option )
{

	DrawBoxWithoutRefresh( _CP_BASE_COLOR_PALLETTE_INDEX, 0, 240-GUIDE_BG_HEIGHT, 320, GUIDE_BG_HEIGHT );

	if( option == OSD_REFRESH )
		pMpegDecoder->RefreshOsd();
}

void DrawBottomGuidePannel( eOSD_REFRESH_OPTION refresh, int option )
{
	DrawBottomGuidePannel( refresh, (RMuint32)option );
}
void DrawBottomGuidePannel( eOSD_REFRESH_OPTION refresh, RMuint32 option )
{

	int tw1, tw2, x1, x2, x3;
		
	DrawBottomGuideBGPannel( SKIP_OSD_REFRESH );

	tw1 = pT->GetStringWidth( (U32)ML_HOME, 12, 0 );
	tw2 = pT->GetStringWidth( (U32)ML_MENU, 12, 0 );
	
	x1 = BTN_X_SHIFT_VALUE + KEYINDEX_IMAGE_WIDTH + KEYINDEX_IMAGE_SPACE;
	x2 = x1 + tw1 + ITEM_SPACE + KEYINDEX_IMAGE_WIDTH + KEYINDEX_IMAGE_SPACE;
	x3 = x2 + tw2 + ITEM_SPACE + KEYINDEX_IMAGE_WIDTH + KEYINDEX_IMAGE_SPACE;

	if (option==0)
		{
			;
		}
	else if (option==1)
		{
			;
		}

	else
		{
		int AddSpace, X;

		if( (option&BGPID_HOME) == BGPID_HOME )
			{
			DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 6, BTN_X_SHIFT_VALUE, BTN_Y, KEYINDEX_IMAGE_WIDTH, KEYINDEX_IMAGE_HEIGHT );
			pT->DrawResText( ML_HOME, x1, BTN_Y+2, _WHITE, 12, DT_TRANSPARENCY );			
			}

		if( (option&BGPID_MENU) == BGPID_MENU )
			{

			if( (option&BGPID_HOME) == BGPID_HOME ) { X= x2; AddSpace=(x1 + tw1 + ITEM_SPACE); }
			else { X = x1; AddSpace=BTN_X_SHIFT_VALUE; }

			DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 3, AddSpace, BTN_Y, KEYINDEX_IMAGE_WIDTH, KEYINDEX_IMAGE_HEIGHT );
			pT->DrawResText( ML_MENU, X, BTN_Y+2, _WHITE, 12, DT_TRANSPARENCY );

			}

		if( (option&BGPID_SELECT) == BGPID_SELECT )
			{

			if( (option&BGPID_HOME) == BGPID_HOME ) { X= x2; AddSpace=(x1 + tw1 + ITEM_SPACE); }
			else if( (option&BGPID_MENU) == BGPID_MENU ) { X= x3; AddSpace=(x2 + tw2 + ITEM_SPACE); }
			else { X = x1; AddSpace=BTN_X_SHIFT_VALUE; }

			DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 5, (x2 + tw2 + ITEM_SPACE), BTN_Y, KEYINDEX_IMAGE_WIDTH, KEYINDEX_IMAGE_HEIGHT );
			pT->DrawResText( ML_SELECT, x3, BTN_Y+2, _WHITE, 12, DT_TRANSPARENCY );			
			}		
		}

	if( refresh == OSD_REFRESH )
		pMpegDecoder->RefreshOsd();

}

#define T _TRANSPARENT
#define L _LIGHTGREEN
#define D _DARKGREEN
static int StartupProgress=0;
RMuint8 ProgressCursor[] = 
{
T,L,L,L,L, 		L,L,L,L,L, 		L,L,L,L,L, 		L,L,D,D,T,
L,L,L,L,L, 		L,L,L,L,L, 		L,L,L,L,L, 		L,L,L,D,D,
D,L,L,L,L, 	L,L,L,L,L, 		L,L,L,L,L, 		L,L,L,D,D,
T,D,D,D,D,	D,D,D,D,D,	D,D,D,D,D,	D,D,D,D,T
};

void DrawStartupProgress( int progress )
{
	DEBUGMSG(1,("DrawStartupProgress(%d) : %d\n", progress, StartupProgress ));
	pMpegDecoder->RenderOsdBits( ProgressCursor, 106+((progress)*22), 143, 20, 4 );
	RELOAD_WATCHDOGTIMER_9SEC;
}


