//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibSubtitle Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSubtitle.h,v $
// $Revision: 1.7 $
// $Date: 2006/06/23 08:18:12 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSubtitle.h,v $
// Revision 1.7  2006/06/23 08:18:12  zstein
// - fixed buffer overflow bug.
//
// Revision 1.6  2006/06/22 01:50:02  zstein
// - supported multi-language caption.
//
// Revision 1.5  2006/04/05 01:46:31  zstein
// - fixed caption bugs.
//
// Revision 1.4  2006/03/27 11:37:14  zstein
// - fixed bugs and implemented FONT tag.
//
// Revision 1.3  2006/03/22 02:36:35  zstein
// - corrected comment parse error.
//
// Revision 1.2  2006/03/15 11:48:22  zstein
// - fixed SMI parse error.
// - remake.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.13  2006/01/05 09:14:15  etnlwind
// Update for Video Caption Align Related
//
// Revision 1.12  2006/01/02 09:06:21  etnlwind
// Update for Subtitle Maxline Expanded
//
// Revision 1.11  2005/12/30 04:24:42  etnlwind
// Update for New Font Adatio
//
// Revision 1.10  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.9  2005/09/16 01:11:18  etnlwind
// Update for FileManager USB Host Related
//
// Revision 1.8  2005/07/20 17:56:27  etnlwind
// Update for Subtitle Releates Issues
//
// Revision 1.7  2005/07/16 02:13:58  etnlwind
// Update for FileDbx
//
// Revision 1.6  2005/07/15 09:46:28  etnlwind
// Update for Video Player SubtitleSyncDbx
//
// Revision 1.5  2005/07/07 13:58:26  etnlwind
// Update for Subtitle Releate Bug Fix
//
// Revision 1.4  2005/06/17 07:11:40  etnlwind
// Update for Caption Realloc Time Decrease
//
// Revision 1.3  2005/06/15 11:51:30  etnlwind
// Update for Video Player Touch Control And Background Savefile Process
//
// Revision 1.2  2005/05/27 13:15:18  etnlwind
// Update for Video Subtitle
//
// Revision 1.1  2005/05/27 11:59:06  etnlwind
// Update for Video Subtitle
//
//
//*****************************************************************************


#ifndef __LIB_SUBTITLE_H__
#define __LIB_SUBTITLE_H__

#define _CONSOLE_MODE_  0
#define _REAL_MODE_     1

#include "unistd.h"
#include "rmbasic.h"


#if _REAL_MODE_
#include <fs.h>
#include <fatfs.h>
#include <tavi_lib.h>
#include <Lib/LibImage.h>
#endif


#define MAX_CAPTION_BUFFER      10
#define MAX_LINE_PER_CAPTION    256
#define MAX_CAPTION_WIDTH       313
#define CAPTION_DATA_BLOCK      307200*2      // 600KB


#define SMI_BUFFER_SIZE         8192
#define PARSER_BUFFER_SIZE      8192

#define MAX_SUPPORT_MULTI_LANG  2

typedef struct _tag_Caption {
    char* str;
    int lColor;
//    int lWidth;
    int tag;
} Caption;

typedef struct _tag_CaptionNode
{
	int		sec;
  Caption caption[MAX_CAPTION_BUFFER];
  int     lSubtitles;
} CAPTION_NODE;

typedef enum {
    SAMI_TAG_SAMI,
    SAMI_TAG_HEAD,
    SAMI_TAG_TITLE,
    SAMI_TAG_STYLE,
    SAMI_TAG_BODY,
    SAMI_TAG_SYNC,
    SAMI_TAG_P,
    SAMI_TAG_BR,
    SAMI_TAG_FONT,
    SAMI_TAG_COMMENT,
    SAMI_TAG_UNKNOWN
} SAMITagID;


typedef struct _tag_SAMITag {
    bool endmark;
    char tagname[32];
    char propname[32];
    char propvalue[32];
    SAMITagID tagid;
} SAMITag;

typedef struct _tag_SAMILang {
  char classname[32];
  char langname[32];
} SAMILang;

typedef struct _tag_CapMergeNode {
  int time;
  int cap_idx[MAX_SUPPORT_MULTI_LANG];
} CapMergeNode;
///////////////////////////////////////////////////////////////////////
//
// CSubtitle Class
// --------------------------

class CSubtitle
{
  bool m_bSAMI;
	CAPTION_NODE* 			pNode;


	RMuint32 				fd;

	int	SyncSecShiftValue;
	int Pre_SyncSecShiftValue;
  int	iCIB[MAX_SUPPORT_MULTI_LANG];
  CapMergeNode* merged_list;
	int	CurrentIndex;

	int	LastIndex[MAX_SUPPORT_MULTI_LANG];
  int m_captionbufidx;
  int m_chunkidx;
  int m_chunksize;
  int m_tagcount;
  int m_contentidx;
	
	CAPTION_NODE*			ciblk[MAX_SUPPORT_MULTI_LANG];
	
	int		MaxCaptionSlot;
	char* CaptionDataBlock;
	char* pPBuf;
  char* m_pSAMIChunk;
  char* m_pContent;
  int m_colorlist[16];
  int m_colors;
  int m_currentcolor;
  int m_lidx;
  int m_mgcnt;
    

#if _REAL_MODE_
	FATFS_ERROR				err;
#endif

protected:
  SAMILang lang_name[MAX_SUPPORT_MULTI_LANG];
  bool DoParseSAMI( void );
  SAMITagID MatchTagName( const SAMITag& tag );
  bool ParseSyncTag( const SAMITag& tag );
  bool ParsePTag( const SAMITag& tag );
  bool ParseFontTag( const SAMITag& tag );
  bool ParseHeadTag( const SAMITag& tag );
  bool ParseTitleTag( const SAMITag& tag );
  bool ParseBRTag( const SAMITag& tag );
  bool ParseTag( SAMITag& tag );
  int ParseSpecialChar( void );
  bool MakeupTag( const char* chunk, int size, SAMITag& tag );

  int FlushContent( void );
  int GetChunk( void );
  int GetChar( void );
  char* Alloc( int offset );
  int Merge( CapMergeNode* list, int cnt0, 
             CAPTION_NODE* b1, int cnt1,
             CAPTION_NODE* b2, int cnt2 );
	
public:
	CSubtitle();
	~CSubtitle();

	bool LoadSMI( char* filename );
	bool LoadSMI( RMuint16* filename );
	bool ParseSMI();
  #if _CONSOLE_MODE_
	int PrintSMI();
  #endif
	int CloseSMI();

	int CurrentItemMSec();
	Caption GetCurrentCaption( int index );
	int ChunkToAdjacentCaption( int MSec );
	char* GetCurrentCaption();
	int ChunkToNext();
    int ChunkToPrev( void );
	int ChunkToHead();

	int SetSyncSecShiftValue( int value );
	int GetSyncSecShiftValue();

  bool GetColor( int index, int& color );
  int GetColors( void ) const { return m_colors; }
  int GetCurrentCaptionCount( void );
  void SetLanguage( int lidx );
  int GetLanguages( void ) const;
  const SAMILang* GetLanguage( int lidx ) const;
};

#endif

