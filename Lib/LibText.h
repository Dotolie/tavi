//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibText Source Code File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibText.h,v $
// $Revision: 1.6 $
// $Date: 2006/06/12 04:41:03 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibText.h,v $
// Revision 1.6  2006/06/12 04:41:03  ywkim
// Increment MAX_TEXT_RES_LINE to 520 from 512 for melon
//
// Revision 1.5  2006/04/05 01:47:11  zstein
// - modified drawcharacter(...) for caption.
//
// Revision 1.4  2006/03/27 11:37:51  zstein
// - modified DT_OUTLINE option.
//
// Revision 1.3  2006/02/27 02:39:20  akaiaphanu
// add DrawScrollTxt()
//
// Revision 1.2  2006/02/24 02:08:20  akaiaphanu
// add DT_SCROLL
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.37  2006/01/24 13:26:08  ywkim
// Remove definition of DT_RADIO_FONT_OPT
// Add definitions of DT_NUMBER_6WIDTH & DT_NUMBER_8WIDTH
// Change function of GetStringWidth(),DrawCharactor(), DrawEffectiveCharactor() & IsStaticWidth for above option
//
// Revision 1.36  2006/01/11 09:29:54  etnlwind
// Update for TextLib
//
// Revision 1.35  2006/01/02 06:54:40  etnlwind
// Update for New Font Bug-Fix
//
// Revision 1.34  2005/12/30 04:24:42  etnlwind
// Update for New Font Adatio
//
// Revision 1.33  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.32  2005/12/03 03:52:07  etnlwind
// Update for FileBrowser Bottom Menu
//
// Revision 1.31  2005/10/17 06:14:36  etnlwind
// Update for TextViewer
//
// Revision 1.30  2005/10/14 11:39:38  etnlwind
// Update for TextViwer
//
// Revision 1.29  2005/10/07 10:12:57  etnlwind
// Update for TextViewer
//
// Revision 1.28  2005/08/27 06:20:05  etnlwind
// Upate for FileManager
//
// Revision 1.27  2005/08/22 07:16:11  etnlwind
// Update for
// 1. Video Player Clock Added.
// 2. Clock Draw Func. Added on TopPannel Class.
// 3. DrawUC16Text DT_TOP_PANNEL option Added.
//
// Revision 1.26  2005/08/17 03:22:22  etnlwind
// Update for FileManager
//
// Revision 1.25  2005/08/08 11:00:35  ywkim
// Add function of GetDividerEnclosedStringWidth with parameter of resID
//
// Revision 1.24  2005/08/08 10:49:41  etnlwind
// Update for FileManger, VideoPlayer
//
// Revision 1.23  2005/07/26 04:50:02  etnlwind
// Update for FileManager Basic
//
// Revision 1.22  2005/07/26 01:31:03  etnlwind
// Update for
//
// 1. BDF font changed;
//    16pix font -> 16pix bold font, 12pix font -> English Alphabet Size Increased
//
// 2. AVI playback routine has changed by external source
//
// Revision 1.21  2005/07/20 17:56:27  etnlwind
// Update for Subtitle Releates Issues
//
// Revision 1.20  2005/07/16 02:07:56  etnlwind
// Update for LibFileDbx
//
// Revision 1.19  2005/07/07 13:58:26  etnlwind
// Update for Subtitle Releate Bug Fix
//
// Revision 1.18  2005/07/05 11:23:57  etnlwind
// Update for Subtitle Font & Video SubMenu
//
// Revision 1.17  2005/05/15 11:13:38  zstein
// - changed GetBoundaryLen(...).
//
// Revision 1.16  2005/05/14 06:14:12  zstein
// *** empty log message ***
//
// Revision 1.15  2005/05/13 11:41:21  zstein
// - changed ::GetBoundaryLength(...).
//
// Revision 1.14  2005/05/13 11:39:24  etnlwind
// Update for Limited X res
//
// Revision 1.13  2005/05/10 05:05:21  etnlwind
// Multi-Line Text Process
//
// Revision 1.12  2005/05/04 14:07:39  etnlwind
// Update for Multi-Line MessageBox
//
// Revision 1.11  2005/05/04 12:52:51  etnlwind
// Update for MessageBox Related
//
// Revision 1.10  2005/04/29 14:32:44  etnlwind
// Update for ES4
//
// Revision 1.9  2005/04/27 17:01:58  etnlwind
// New Type MsgBox Library Update
//
// Revision 1.8  2005/04/26 03:41:42  ywkim
// Add function of GetTextRes() for returning string pointer
//
// Revision 1.7  2005/04/23 01:26:28  zstein
// Added - CSize GetStringSize( const uchar_t* psz, int len, int nFontSize );
//       - CSize GetCharSize( uchar_t ch, int nFontSize );
// 	  - int GetBoundaryLength( uchar_t* pwc, int nLength, int nFontSize, int nBoundaryWidth );
//
// Revision 1.6  2005/04/22 14:37:35  etnlwind
// Library Update for UI changes
//
// Revision 1.5  2005/04/22 01:58:39  etnlwind
// MultiLanguage Support
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

#ifndef __LIBTEXT_H__
#define __LIBTEXT_H__


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

#include <unistd.h>
#include <dirent.h>

#include <stdio.h>	
#include <string.h>
#include <stdlib.h>
#include <memory.h>
//#include <linux/nls.h>

//#ifndef _CONSOLE_MODE_
#include <fs.h>
#include <fatfs.h>
#include <Lib/LibImage.h>
//#endif

#include <Lib/LibWideString.h>
#include <tavi_text_resources.h>
#include <Component/Pixmap.h>
#include <Component/object.h>


#define SIZE_SMALL_BUFFER 256
#define _ATTRIBUTE_PACKED_		__attribute__ ((packed)) 
#define MAX_TEXT_RES_LINE (520)
#define MAX_TEXT_RES_FILE_SIZE (4096*10)

#define DT_TRANSPARENCY		0x00000001
#define DT_SCROLLTEXT			0x00000002
#define DT_TEXTVIEWER			0x00000004
#define DT_FILEBROWSER			0x00000008
#define DT_BOLD					0x00000010
#define DT_SHADOW				0x00000020
#define DT_OUTLINE				0x00000040
#define DT_CONTROLPANNEL		0x00000080
#define DT_ENABLE_DIVIDER		0x00000100
#define DT_ALIGN_CENTER		0x00000200
#define DT_ALIGN_LEFT			0x00000400
#define DT_ALIGN_RIGHT			0x00000800
#define DT_CAPTION				0x00001000
#define DT_PROGRESS_DBX		0x00002000
#define DT_TOP_PANNEL			0x00004000
#define DT_TEXT_AREA			0x00008000
#define DT_STATIC_6WIDTH		0x00010000
#define DT_STATIC_8WIDTH		0x00020000
#define DT_STATIC_10WIDTH		0x00040000
#define DT_STATIC_12WIDTH		0x00080000
#define DT_STATIC_14WIDTH		0x00100000
#define DT_NUMBER_6WIDTH		0x00200000
#define DT_NUMBER_8WIDTH		0x00400000
#define DT_SCROLL_TITLE			0x00800000
#define DT_SCROLL_SELECTBAR	0x01000000
typedef unsigned short int 	uchar_t; 		// 0x0000 ~ 0xFFFF
typedef unsigned long 		U32;
typedef unsigned short int 	U16;
typedef signed short int 	S16;
typedef unsigned char 		U8;

typedef enum { DISK, RAM, ROM } TTargetMedia;

typedef struct _tag_TBDFCharInfo {
	U16	encoding;
	U16	swidth_x, swidth_y;
	U16	dwidth_x, dwidth_y;
	S16	BBw, BBh, BBxoff, BByoff;
	U16	fontdata[16];
} _ATTRIBUTE_PACKED_ TBDFCharInfo;

typedef struct _tag_TOptimizedBDFCharInfo {
	U16	encoding;
	U8	BBw, BBh;
	char BBxoff, BByoff;
	U16	fontdata[16];
} _ATTRIBUTE_PACKED_ TOptimizedBDFCharInfo;

typedef struct _tag_TOptimizedBitmapDataFont
{
	RMuint32 				fd;
	RMuint32					FontFilesize;
	int        			     		FontSize;
	TOptimizedBDFCharInfo**  	Glyph;
	RMuint8*					Raw;
} _ATTRIBUTE_PACKED_ TOptimizedBitmapDataFont;

typedef struct _tag_TTokenStringIndex
{
	// Input
	int FontSize;
	int DisplayableWidth;
	int StartPointOfPage;
	int SizeOfPageBuffer;

	// Output
	int NumOfTokens;
	int PixelWidthOfLastTokenString;
	int NewCursor;

} __ATTRIBUTE_PACKED__ TTokenStringIndex;

class CTextEngine_BDF
{
	TOptimizedBitmapDataFont T12;
	TOptimizedBitmapDataFont T16;

	int tx; 	// drawing area X
	int ty; 	// drawing area y
	int tw; 	// drawing area with
	int sx;  // fontdata using start X
	int eotx;// end of text x
	int eoty;// end of text x
	int m_outlinecolor;

	eSYSTEM_LANGUAGE CurrentLanguage;

	uchar_t* TextRes[MAX_TEXT_RES_LINE];
	uchar_t*	TextResBuf;
	uchar_t* usp;	

	#define TEXT_SCROLL_BUFFER_WIDTH	(320*16)
	#define TEXT_SCROLL_BUFFER_SIZE (TEXT_SCROLL_BUFFER_WIDTH*16)
	RMuint8* TextScrollBuffer;
	TMsgEntry tMsgTemp;

	int FontLoadDiskToRAM_Optimized( TOptimizedBitmapDataFont* tFont );
	int Load_SystemTextResources( eSYSTEM_LANGUAGE language );
	int Remove_SystemLanguageData();

public:
//	CTextEngine_BDF() {}
	CTextEngine_BDF( char* FontDir );
	virtual ~CTextEngine_BDF();

	U8 Convert1digitBCDCharToHexCode( U8 BCDChar );
	U8 Convert2digitBCDStringToHexCode( U8 * BCDString );
	U16 Convert4digitBCDStringToHexCode( U8 * BCDString  )	;
	U16 ConvertVarBCDStringTokenToHexCode( U8 ubTokenNumber, U8 * BCDString );
	U32 ConvertVarBCDStringTokenToHexCodeU32( U8 ubTokenNumber, U8 * BCDString );
	int ConvertEUC8cToU16c( U16* UC16c, char H_EUC8c, char L_EUC8c );
	int ConvertEUC8sToU16s( U16* UC16s, char* EUC8s, int MaxLineLength );
	U16 ConvertEUC8StrToUnicode16Str( U16 uc16String[], char euc8String[],  int MaxLineLength=0 );
	U16 ConvertVFATTextToUnicode16Str( U16 uc16String[], char euc8String[] );

	int DebugPrintAFontMemory( int fontsize, uchar_t code );
	int FontPrintWithBinary_Optimized( TOptimizedBDFCharInfo* ptCurrentCharCode  );
	U16 InitTextScrollBuffer( U16 BackGroundPalette=27);

	U16 DrawCharacter( TOptimizedBDFCharInfo* ptCurrentCharCode, U16 x, U16 y, U16 color, U16 fontsize, U32 option );
	U16 DrawEffectCharacter( TOptimizedBDFCharInfo* ptCurrentCharCode, U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 option );
//	U16 DrawCharacterOnScrollBuffer( TOptimizedBDFCharInfo* ptCurrentCharCode, U16 rx, U16 ry, U16 vx, U16* vy, U16 color, U16 fontsize, U32 option );
	U16 DrawCharacterOnScrollBuffer( TOptimizedBDFCharInfo* ptCurrentCharCode, int rx, int ry, int vx, int* vy, U16 color, U16 fontsize, U32 option );
	U16 DrawResText( U32 ResIndex, U16 x, U16 y, U16 color, U16 fontsize, U32 option );
	U16 DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 fontsize, U32 option );
	U16 DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 option );
	U16 DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 limited_x, U32 option );
	U16 DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 limited_x, int* pIsAutoSplitActivated, U32 option );
	U16 DrawEUC8Text( char euc8String[], U16 x, U16 y, U16 color, U16 fontsize, U32 option );
	U16 DrawEUC8Text( char euc8String[], U16 x, U16 y, U16 color, U16 color2, U16 fontsize, U32 option );
	U16 DrawEUC8LineText( char euc8String[], U16 x, U16 y, U16 color, U16 SubColor, U16 fontsize, U32 option, int* pIsAutoSplitActivated );

	U16 DrawVFATText( char euc8String[], U16 x, U16 y, U16 color, U16 fontsize, U32 option );

	int DrawScrollTxt( int sx , int title_width);
	int DrawScrollText( int sx );  // fontdata using start X

	int InitScrollText( 
		int tx, 	// drawing area X
		int ty, 	// drawing area y
		int tw, 	// drawing area with
		int sx,  // fontdata using start X
		int eotx,	// end of text x
		int eoty		// end of text y		
		);

	int GetStartXPosOfTxtBuf();
	int GetEndXPosOfTxtBuf();
	int GetStringWidth();
	int GetStringWidth( U32 ResIndex, U16 fontsize, U32 option );
	int GetStringWidth( char euc8String[], U16 fontsize, U32 option );
	int GetStringWidth( U16 usString[], U16 fontsize, U32 option );
	int GetNumOfTokens( char* BlockBuffer, int StartPoint, int BufferSize, int FontSize, int Width, U32 Option );
	int GetNumOfTokens( uchar_t* UnicodeBlockBuffer, int StartPointOnUnicodeBuffer, int BufferSize, int FontSize, int Width, U32 Option );
	int GetLastTokenStringIndex( char euc8String[], TTokenStringIndex* pTSI, U32 option );
	int GetLastTokenStringIndex( uchar_t usString[], TTokenStringIndex* pTSI, U32 option );
	int GetStringDividerCount( uchar_t* ucString );
	uchar_t* GetTextRes( U32 ResIndex );
	int Change_SystemLanguage( eSYSTEM_LANGUAGE language );
	uchar_t* GetNthStringWithDivider( uchar_t* ucString, uchar_t divider, int N );
	uchar_t* SetNthStringWithDivider( uchar_t* dest, uchar_t* src, uchar_t divider, int N );
	int GetDividerEnclosedStringWidth( U32 ResIndex, U16 fontsize, U32 option );
	int GetDividerEnclosedStringWidth( U16 usString[], U16 fontsize, U32 option );
	int GetNthStringWidth( uchar_t* ucString, int AdditionalSpace, int N );
	int GetNthStringWidthWithDivider( uchar_t* ucString, uchar_t divider, int AdditionalSpace, int N );	

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Added Method By Lee Seok
	U16 DrawCharacter( TOptimizedBDFCharInfo* ptCurrentCharCode, U16 x, U16 y, U16 color, U16 fontsize, U32 option, CPixmap* pixmap );
	U16 DrawCharacter( uchar_t ch, U16 x, U16 y, U16 color, U16 fontsize, U32 option, CPixmap* pixmap=NULL);
	U16 DrawUC16Text( U16 usString[], U16 x, U16 y, U16 color, U16 fontsize, U32 option, CPixmap* pixmap );
	CSize GetStringSize( const uchar_t* psz, int len, int nFontSize );
	CSize GetCharSize( uchar_t ch, int nFontSize );
	CSize* GetCharSize( uchar_t ch, int nFontSize, CSize* pSz );
	int GetBoundaryLength( uchar_t* pwc, int nLength, int nFontSize, int option, int nBoundaryWidth );

    void SetOutlineColor( int color ) { 
        m_outlinecolor = color;
    }
};


#endif



