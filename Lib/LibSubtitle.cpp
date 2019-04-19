//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibSubtitle Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSubtitle.cpp,v $
// $Revision: 1.10 $
// $Date: 2006/06/23 08:18:12 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSubtitle.cpp,v $
// Revision 1.10  2006/06/23 08:18:12  zstein
// - fixed buffer overflow bug.
//
// Revision 1.9  2006/06/22 01:50:02  zstein
// - supported multi-language caption.
//
// Revision 1.8  2006/05/17 06:07:45  zstein
// - ignore invalid tag format.
//
// Revision 1.7  2006/04/19 09:35:16  zstein
// - fixed out-of-color range bug.
//
// Revision 1.6  2006/04/05 01:46:31  zstein
// - fixed caption bugs.
//
// Revision 1.5  2006/03/27 11:37:14  zstein
// - fixed bugs and implemented FONT tag.
//
// Revision 1.4  2006/03/22 02:36:34  zstein
// - corrected comment parse error.
//
// Revision 1.3  2006/03/21 06:30:57  zstein
// - ignore tag errors.
//
// Revision 1.2  2006/03/15 11:48:22  zstein
// - fixed SMI parse error.
// - remake.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*****************************************************************************

#include "LibSubtitle.h"

#if _REAL_MODE_
#include <Lib/LibImage.h>
#include <Lib/LibDefines.h>
extern CTextEngine_BDF* pT;
#endif

#define MAX_COLORS      16
#if _CONSOLE_MODE_
#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
static void debug_break (void){}
#define ASSERT(exp)					((void)((exp)?1:(printf ("***ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
#endif

typedef struct _tag_ReservedSAMITag {
    char name[32];
    SAMITagID id;
} ReservedSAMITag;

static ReservedSAMITag _SAMITags[] = {
    { "SAMI",   SAMI_TAG_SAMI },
    { "HEAD",   SAMI_TAG_HEAD },
    { "TITLE",  SAMI_TAG_TITLE },
    { "STYLE",  SAMI_TAG_STYLE },
    { "BODY",   SAMI_TAG_BODY },
    { "SYNC",   SAMI_TAG_SYNC },
    { "P",      SAMI_TAG_P },
    { "BR",     SAMI_TAG_BR },
    { "",       SAMI_TAG_UNKNOWN }
};

typedef struct _tag_ColorTable {
    char name[30];
    int color;
} ColorTable;

#define MAX_RESERVED_COLORS     140
static ColorTable _ColorTable[MAX_RESERVED_COLORS] = {
    { "red", 0xFF0000 },
    { "crimson", 0xDC143C },
    { "firebrick", 0xB22222 },
    { "maroon", 0x800000 },
    { "darkred", 0x8B0000 },
    { "brown", 0xA52A2A },
    { "sienna", 0xA0522D },
    { "saddlebrown", 0x8B4513 },
    { "indianred", 0xCD5C5C },
    { "rosybrown", 0xBC8F8F },
    { "lightcoral", 0xF08080 }, 
	  { "salmon", 0xFA8072 },
    { "darksalmon", 0xE9967A }, 
    { "coral", 0xFF7F50 },
    { "tomato", 0xFF6347 },
    { "sandybrown", 0xF4A460 },
    { "lightsalmon", 0xFFA07A },
    { "peru", 0xCD853F },
    { "chocolate", 0xD2691E },
    { "orangered", 0xFF4500 },
    { "orange", 0xFFA500 },
    { "darkorange", 0xFF8C00 },
    { "tan", 0xD2B48C },
    { "peachpuff", 0xFFDAB9 },
    { "bisque", 0xFFE4C4 },
    { "moccasin", 0xFFE4B5 },
    { "navajowhite", 0xFFDEAD },
    { "wheat", 0xF5DEB3 },
    { "burlywood", 0xDEB887 },
    { "darkgoldenrod", 0xB8860B },
    { "goldenrod", 0xDAA520 },
    { "gold", 0xFFD700 },
    { "yellow", 0xFFFF00 },
    { "lightgoldenrodyellow", 0xFAFAD2 },
    { "palegoldenrod", 0xEEE8AA },
    { "khaki", 0xF0E68C },
    { "darkkhaki", 0xBDB76B },
    { "lawngreen", 0x7CFC00 },
    { "greenyellow", 0xADFF2F },
    { "chartreuse", 0x7FFF00 },
    { "lime", 0x00FF00 },
    { "limegreen", 0x32CD32 },
    { "yellowgreen", 0x9ACD32 },
    { "olive", 0x808000 },
    { "olivedrab", 0x6B8E23 },
    { "darkolivegreen", 0x556B2F },
    { "forestgreen", 0x228B22 },
    { "darkgreen", 0x006400 },
    { "green", 0x008000 },
    { "seagreen", 0x2E8B57 },
    { "mediumseagreen", 0x3CB371 },
    { "darkseagreen", 0x8FBC8F },
    { "lightgreen", 0x90EE90 },
    { "palegreen", 0x98FB98 },
    { "springgreen", 0x00FF7F },
    { "mediumspringgreen", 0x00FA9A },
    { "teal", 0x008080 },
    { "darkcyan", 0x008B8B },
    { "lightseagreen", 0x20B2AA },
    { "mediumaquamarine", 0x66CDAA },
    { "cadetblue", 0x5F9EA0 },
    { "steelblue", 0x4682B4 },
    { "aquamarine", 0x7FFFD4 },
    { "powderblue", 0xB0E0E6 },
    { "paleturquoise", 0xAFEEEE },
    { "lightblue", 0xADD8E6 },
    { "lightsteelblue", 0xB0C4DE },
    { "skyblue", 0x87CEEB },
    { "lightskyblue", 0x87CEFA },
    { "mediumturquoise", 0x48D1CC },
    { "turquoise", 0x40E0D0 },
    { "darkturquoise", 0x00CED1 },
    { "aqua", 0x00FFFF },
    { "cyan", 0x00FFFF },
    { "deepskyblue", 0x00BFFF },
    { "dodgerblue", 0x1E90FF },
    { "cornflowerblue", 0x6495ED },
    { "royalblue", 0x4169E1 },
    { "blue", 0x0000FF },
    { "mediumblue", 0x0000CD },
    { "navy", 0x000080 },
    { "darkblue", 0x00008B },
    { "midnightblue", 0x191970 },
    { "darkslateblue", 0x483D8B },
    { "slateblue", 0x6A5ACD },
    { "mediumslateblue", 0x7B68EE },
    { "mediumpurple", 0x9370DB },
    { "darkorchid", 0x9932CC },
    { "darkviolet", 0x9400D3 },
    { "blueviolet", 0x8A2BE2 },
    { "mediumorchid", 0xBA55D3 },
    { "plum", 0xDDA0DD },
    { "lavender", 0xE6E6FA },
    { "thistle", 0xD8BFD8 },
    { "orchid", 0xDA70D6 },
    { "violet", 0xEE82EE },
    { "indigo", 0x4B0082 },
    { "darkmagenta", 0x8B008B },
    { "purple", 0x800080 },
    { "mediumvioletred", 0xC71585 },
    { "deeppink", 0xFF1493 },
    { "fuchsia", 0xFF00FF },
    { "magenta", 0xFF00FF },
    { "hotpink", 0xFF69B4 },
    { "palevioletred", 0xDB7093 },
    { "lightpink", 0xFFB6C1 },
    { "pink", 0xFFC0CB },
    { "mistyrose", 0xFFE4E1 },
    { "blanchedalmond", 0xFFEBCD },
    { "lightyellow", 0xFFFFE0 },
    { "cornsilk", 0xFFF8DC },
    { "antiquewhite", 0xFAEBD7 },
    { "papayawhip", 0xFFEFD5 },
    { "lemonchiffon", 0xFFFACD },
    { "beige", 0xF5F5DC },
    { "linen", 0xFAF0E6 },
    { "oldlace", 0xFDF5E6 },
    { "lightcyan", 0xE0FFFF },
    { "aliceblue", 0xF0F8FF },
    { "whitesmoke", 0xF5F5F5 },
    { "lavenderblush", 0xFFF0F5 },
    { "floralwhite", 0xFFFAF0 },
    { "mintcream", 0xF5FFFA },
    { "ghostwhite", 0xF8F8FF },
    { "honeydew", 0xF0FFF0 },
    { "seashell", 0xFFF5EE },
    { "ivory", 0xFFFFF0 },
    { "azure", 0xF0FFFF },
    { "snow", 0xFFFAFA },
    { "white", 0xFFFFFF },
    { "gainsboro", 0xDCDCDC },
    { "lightgrey", 0xD3D3D3 },
    { "silver", 0xC0C0C0 },
    { "darkgray", 0xA9A9A9 },
    { "lightslategray", 0x778899 },
    { "slategray", 0x708090 },
    { "gray", 0x808080 },
    { "dimgray", 0x696969 },
    { "darkslategray", 0x2F4F4F },
    { "black", 0x000000 }
};

static int flushbytes;
static int syncsec;

static bool _IsHex( const char* str )
{
    bool ret = true;
    char ch;

    for( int i=0; (ch=str[i]); i++ ) {
        if( '0' > ch || ch > '9' ) {
            if( ch > 'Z' ) {
                // lower character
                ch -= ('a' - 'A');
            }

            if( 'A' > ch || ch > 'F' ) {
                ret = false;
                break;
            }
        }
    }

    return ret;
}


static bool _MatchColor( const char* colorname, int& color ) 
{
    bool match = false;
    
    for( int i=0; i<MAX_RESERVED_COLORS; i++ ) {
        if( strcasecmp(colorname, _ColorTable[i].name) == 0 ) {
            color = _ColorTable[i].color;
            match = true;
            break;
        }
    }

    return match;
}

static unsigned int htoi(const char *str)
{
	const char *cp;
	unsigned int data, bdata;

	for (cp = str, data = 0; *cp != 0; ++cp) {
		if (*cp >= '0' && *cp <= '9')
			bdata = *cp - '0';
		else if (*cp >= 'A' && *cp <= 'F')
			bdata = *cp - 'A' + 10;
		else if (*cp >= 'a' && *cp <= 'f')
			bdata = *cp - 'a' + 10;
		else
			break;
		data = (data << 4) | bdata;
	}

	return data;
}

static int cur_lang = 0;

CSubtitle::CSubtitle()
{
	ciblk[0] = NULL;
	ciblk[1] = NULL;
	CaptionDataBlock = NULL;

	m_pSAMIChunk = NULL;

	m_colors = 1;
	m_currentcolor = 0xffffff;
	memset( (void*)m_colorlist, 0, sizeof(m_colorlist) );
	// default color
	m_colorlist[0] = 0xffffff;

  for( int i=0; i<MAX_SUPPORT_MULTI_LANG; i++ ) {
    memset( &lang_name[i], 0, sizeof(SAMILang) );
  }

	cur_lang = 0;
	m_lidx = 0;
}

CSubtitle::~CSubtitle()
{
	CloseSMI();

	// Releases resources
	if( m_pSAMIChunk ) {
    delete[] m_pSAMIChunk;
	}

	if( merged_list ) {
	  delete[] merged_list;
	}
}

bool CSubtitle::DoParseSAMI( void )
{
  bool ret;
    
  // Initializes member variables.
	m_bSAMI = false;
	if( ciblk[0] == NULL && 
	    CaptionDataBlock == NULL ) {
		ciblk[0]	= (CAPTION_NODE*) malloc( sizeof(CAPTION_NODE)*8192);
		ciblk[1]	= (CAPTION_NODE*) malloc( sizeof(CAPTION_NODE)*8192);
		CaptionDataBlock 	= (char*) malloc( sizeof(char)*CAPTION_DATA_BLOCK );
		memset( ciblk[0], '\0', sizeof(CAPTION_NODE)*8192 );
		memset( ciblk[1], '\0', sizeof(CAPTION_NODE)*8192 );
		memset( CaptionDataBlock, '\0', sizeof(char)*CAPTION_DATA_BLOCK ); 
	}
	
	ret = ParseSMI();
	for( int i=0; i<MAX_SUPPORT_MULTI_LANG; i++ ) {
  	LastIndex[i] = iCIB[i]-1;
	}
	CurrentIndex = 0;
	SyncSecShiftValue = 0;

  // NOW!! WE SUPPORT ONLY TWO MULTICAPTION.
  // TODO: SUPPORT N-MULTICAPTION!
  int max = (LastIndex[0]>LastIndex[1]?LastIndex[0]:LastIndex[1]);
  max *= 2;
  merged_list = new CapMergeNode[max];

  if( cur_lang ) {
  	m_mgcnt = Merge( 
  	            merged_list, max, 
  	            ciblk[0], iCIB[0], 
  	            ciblk[1], iCIB[1] );
	}
	else {
	  for( int i=0; i<iCIB[0]; i++ ) {
	    merged_list[i].time = ciblk[0][i].sec;
	    merged_list[i].cap_idx[0] = i;
	    merged_list[i].cap_idx[1] = -1;
	  }
	  m_mgcnt = iCIB[0];
	}

	return ret;
}


int 
CSubtitle::Merge( 
  CapMergeNode* list, int cnt0,
  CAPTION_NODE* b1, int cnt1,
  CAPTION_NODE* b2, int cnt2 )
{
  int idx[2] = { 0, 0 };
  int br1, br2, i;

  br1 = br2 = 0;
  for( i=0; i<cnt0; i++ ) {
    if( idx[0] >= cnt1 ) {
      br1 = 1;
      break;
    }
    if( idx[1] >= cnt2 ) {
      br2 = 1;
      break;
    }
    if( abs(b1[idx[0]].sec-b2[idx[1]].sec) < 200 ) {
      list[i].time = b1[idx[0]].sec;
      list[i].cap_idx[0] = idx[0]++;
      list[i].cap_idx[1] = idx[1]++;
    }
    else if( b1[idx[0]].sec < b2[idx[1]].sec ) {
      list[i].time = b1[idx[0]].sec;
      list[i].cap_idx[0] = idx[0]++;
      list[i].cap_idx[1] = idx[1] ? (idx[1]-1) : -1;
    }
    else {
      list[i].time = b2[idx[1]].sec;
      list[i].cap_idx[0] = idx[0] ? (idx[0]-1) : -1;
      list[i].cap_idx[1] = idx[1]++;
    }
  }

  if( br1 ) {
    for( ;idx[1]<cnt2&&i<cnt0; idx[1]++ ) {
      list[i].time = b2[idx[1]].sec;
      list[i].cap_idx[0] = -1;
      list[i].cap_idx[1] = idx[1]++;
      i++;
    }
  }
  if( br2 ) {
    for( ;idx[0]<cnt1&&i<cnt0; idx[0]++ ) {
      list[i].time = b1[idx[0]].sec;
      list[i].cap_idx[0] = idx[0]++;
      list[i].cap_idx[1] = -1;
      i++;
    }
  }
  
  return i;
}

bool CSubtitle::LoadSMI( char* filename )
{
	bool ret;
	
	fd = open( filename, O_RDONLY );

    ret = DoParseSAMI();
	
	close( fd );

	return ret;
}


bool CSubtitle::LoadSMI( RMuint16* filename )
{
	bool ret=0;

#if _REAL_MODE_
	if( (err=fatfs_wfopen( filename, _O_RDONLY, &fd ))>0 ) {
		fatfs_fclose( fd );
		ASSERT(0);
	}

	ret = DoParseSAMI();
	
	fatfs_fclose( fd );
#endif

	return ret;
}


/* ++ 

    ParseTag( char* samichunk )

Description:
    Parses SAMI tag

Arguments:

-- */
bool CSubtitle::MakeupTag( const char* samichunk, int size, SAMITag& tag )
{
    const char* p = samichunk;

    memset( (void*)&tag, 0, sizeof(SAMITag) );

    if( samichunk[0] == '!' && 
        samichunk[1] == '-' &&
        samichunk[2] == '-' ) {
        // Comment
        // Check EndMark(-->)
        if( p[size-2] != '-' && p[size-1] != '-' ) {
            int ch;
            while( 1 ) {
                ch = GetChar();
                if( ch == 0 ) {
                    break;
                }
                else if( ch == '-' ) {
                    ch = GetChar();
                    if( ch == '-' ) {
                        ch = GetChar();
                        if( ch == '>' ) {
                            break;
                        }
                    }
                }
            }
        }
        return false;
    }

    if( samichunk[0] == '/' ) {
        tag.endmark = true;
        p++;
    }
    
    // get tag name
    int i;
    for( i=0; i<sizeof(tag.tagname)-1 && (*p && *p != ' '); p++ ) {
        tag.tagname[i++] = *p;
    }

    tag.tagid = MatchTagName( tag );
    // </TAGNAME>
    if( tag.endmark ) {
        return true;
    }

    // A tag has no property.
    if( *p == 0 ) {
        goto __MakeupTag_End;
    }
    p++;
    
    // get property
    for( i=0; i<sizeof(tag.propname)-1 && *p; p++, i++ ) {
        if( *p == ' ' ) {
            continue;
        }
        else if( *p == ';' ) {
            break;
        }
        else if( *p == '=' ) {
            break;
        }
        else {
            tag.propname[i] = *p;
        }
    }

    // get value
    if( *p != '=' || *p == 0 ) {
        // invalid value of property
        return false;
    }
    p++;
    for( i=0; i<sizeof(tag.propvalue)-1 && *p; p++ ) {
        if( *p == '\"' ) {
            continue;
        }
        tag.propvalue[i++] = *p;
    }

__MakeupTag_End:

    return true;
}


/* ++

    MatchTagName( SAMITag& tag )

Description:

Arguments:

Return:

-- */
SAMITagID CSubtitle::MatchTagName( const SAMITag& tag )
{
    int i;

    for( i=0; _SAMITags[i].id != SAMI_TAG_UNKNOWN; i++ ) {
        if( !strcasecmp(_SAMITags[i].name, tag.tagname) ) {
            break;
        }
    }

    return (SAMITagID)i;
}



/* ++

    ParseTag( const SAMITag& tag )

Description

Arguments

Return:

-- */
bool CSubtitle::ParseTag( SAMITag& tag )
{
    SAMITagID id;
    bool ret = false;

    switch( tag.tagid ) {
    case SAMI_TAG_SAMI:
        m_bSAMI = true;
        break;
    case SAMI_TAG_HEAD:
        break;
    case SAMI_TAG_TITLE:
        break;
    case SAMI_TAG_STYLE:
        break;
    case SAMI_TAG_BODY:
        break;
    case SAMI_TAG_SYNC:
        ret = ParseSyncTag( tag );
        break;
    case SAMI_TAG_P:
        ret = ParsePTag( tag );
        break;
    case SAMI_TAG_BR:
        ret = ParseBRTag( tag );
        break;
    case SAMI_TAG_FONT:
        ret = ParseFontTag( tag );
        break;
    default:
        ret = false;
    }

    return ret;
}



/* ++

    ParseSyncTag( const SAMITag& tag )

Description:
    Parses sync tag

Arguments:

Return:

-- */
bool CSubtitle::ParseSyncTag( const SAMITag& tag )
{
  flushbytes = FlushContent();
  m_currentcolor = 0;
  syncsec = atoi( tag.propvalue );


  return true;
}


/* ++

    ParsePTag( const SAMITag& tag )

Description:
    Parses P(Paragraph) tag.

Arguments:

Return:

-- */
bool CSubtitle::ParsePTag( const SAMITag& tag )
{
  #define LANG_KOREAN     0
  #define LANG_ENGLISH    1
  #define LANG_JAPANISES  2
  #define LANG_UNKNOWN    255
  int lang_id;
  char* name;
  
  if( !strcasecmp(tag.propname, "CLASS" ) ) {
    // TODO: support multi-language
    if( !strcasecmp(tag.propvalue, "encc") ) {
      lang_id = LANG_ENGLISH;
      name = "english";
    }
    else if( !strcasecmp(tag.propvalue, "krcc") ) {
      lang_id = LANG_KOREAN;
      name = "korean";
    }
    else {
      lang_id = LANG_UNKNOWN;
      name = "unknown";
    }

    if( strcasecmp(lang_name[cur_lang].classname, tag.propvalue) ) {
      if( lang_name[cur_lang].classname[0] ) {
        cur_lang++;
      }
      strcpy(lang_name[cur_lang].classname, tag.propvalue );
      strcpy(lang_name[cur_lang].langname, name );
    }
  }

  pNode = &(ciblk[cur_lang][iCIB[cur_lang]++]);
  pNode->sec = syncsec;
  pNode->lSubtitles = 0;
  pNode->caption[pNode->lSubtitles].str = Alloc(flushbytes);
  pNode->caption[pNode->lSubtitles].lColor = m_currentcolor;
  pNode->caption[pNode->lSubtitles].tag = -1;
  pNode->lSubtitles++;

  flushbytes = 0;

  return true;
}


/* ++

    ParseFontTag( const SAMITag& tag )

Description:
    Parses Font tag.

Arguments:

Return:

-- */
bool CSubtitle::ParseFontTag( const SAMITag& tag )
{
    int color = 0xffffff;
    int nbytes;

    nbytes = FlushContent();
    pNode->caption[pNode->lSubtitles].str = Alloc(nbytes);
    pNode->caption[pNode->lSubtitles].lColor = m_currentcolor;
    pNode->caption[pNode->lSubtitles].tag = -1;
    pNode->lSubtitles++;
    m_currentcolor = 0;
    m_contentidx = 0;

    if( !tag.endmark && m_colors < MAX_COLORS ) {
        if( tag.propvalue[0] == '#' ) {
        // RGB color value(#ffffff)
            color = htoi(&tag.propvalue[1]);
        }
        else {
            if( _IsHex(tag.propvalue) ) {
                color = htoi(tag.propvalue);
            }
            else {
                !_MatchColor( tag.propvalue, color );
            }
        }

        bool bexist = false;
        int i;
        for( i=0; i<m_colors; i++ ) {
            if( m_colorlist[i] == color ) {
                bexist = true;
                break;
            }
        }

        if( bexist ) {
            m_currentcolor = i;
        }
        else {
            m_colorlist[m_colors] = color;
            m_colors++;
            m_currentcolor = m_colors-1;
        }
    }

    return true;
}


/* ++

    ParseHeadTag( const SAMITag& tag )

Description:
    Parses HEAD tag.

Arguments:

Return:

-- */
bool CSubtitle::ParseHeadTag( const SAMITag& tag )
{
    if( tag.endmark ) {
        m_contentidx = 0;
    }

    return true;
}


/* ++

    ParseTitleTag( cosnt SAMITag& tag )

Description:
    Parses TITLE tag.

Arguments:

Return:

-- */
bool CSubtitle::ParseTitleTag( const SAMITag& tag )
{
    if( tag.endmark ) {
        m_contentidx = 0;
    }
    
    return true;
}


/* ++

    ParseSpecailChar()

Description:

Arguments:

Return:

-- */
int CSubtitle::ParseSpecialChar( void )
{
    int ch;
    int cnt = 0;
    char spcbuf[10];

    while( 1 ) {
        ch = GetChar();
        if( ch == ';' || 
            ch == ' ' ||
            ch == '\r' ||
            ch == '\n' ||
            ch == '\t' ) {   // end mark
            break;
        }
        if( ch == 0 ) {
            break;
        }
        spcbuf[cnt++] = ch;

        // The length of special character is less than 10.
        if( cnt > 9 ) {
            break;
        }
    }
    spcbuf[cnt] = 0;

    if( !strcasecmp(spcbuf, "nbsp") ) {
        ch = ' ';
    }
    else if( !strcasecmp(spcbuf, "amp") ) {
        ch = '&';
    }
    else if( !strcasecmp(spcbuf, "quot") ) {
        ch = '\"';
    }
    else if( !strcasecmp(spcbuf, "lt") ) {
        ch = '<';
    }
    else if( !strcasecmp(spcbuf, "gt") ) {
        ch = '>';
    }
    else {
        printf( "unknown character: %s\n", spcbuf );
        m_pContent[m_contentidx++] = '&';
        for( int i=0; i<cnt; i++ ) {
            m_pContent[m_contentidx++] = spcbuf[i];
        }
    }

    return (char)ch;
}

/* ++

    ParseBRTag( const SAMITag& tag )

Description:
    Parses BT tag.

Arguments:

Return:

-- */
bool CSubtitle::ParseBRTag( const SAMITag& tag )
{
    #if 0
    int r;
    int nbytes;

    nbytes = FlushContent();
    if( !pNode ) {
      return false;
    }
    pNode->caption[pNode->lSubtitles].str = Alloc(nbytes);
    pNode->caption[pNode->lSubtitles].lColor = m_currentcolor;
    pNode->caption[pNode->lSubtitles].tag = SAMI_TAG_BR;
    pNode->lSubtitles++;
    pNode->caption[pNode->lSubtitles].str = Alloc(2);
    pNode->caption[pNode->lSubtitles].lColor = m_currentcolor;
    pNode->caption[pNode->lSubtitles].tag = -1;
    pNode->lSubtitles++;

    // clear content buffer
    m_contentidx = 0;
    #endif
    m_pContent[m_contentidx++] = '\n';
    return true;
}

/* ++

    AllocBuffer();

Description:

Arguments:

Return:

-- */
char* CSubtitle::Alloc( int offset )
{
    int r;
    char* p;
    
    r = offset%4;
    if( !r ) {
        r = 4;
    }
    m_captionbufidx += offset + r;
    p = &CaptionDataBlock[m_captionbufidx];

    return p;
}


/* ++

    FlushContent( void )

Description:

Argument:

Return:
    Returns the number of bytes.
-- */
int  CSubtitle::FlushContent( void )
{   
    int nbytes = m_contentidx;

    m_pContent[m_contentidx] = 0;
    m_contentidx = 0;
    if( pNode && nbytes ) {
        strcpy( pNode->caption[pNode->lSubtitles-1].str, m_pContent );
        pNode->caption[pNode->lSubtitles-1].lColor = m_currentcolor;
        nbytes++;
    }
    return nbytes;
}
    
/* ++

    GetChunk( char* buf )

Description:
    Get a chunk of SMI file.

Arguments:
    buf is a pointer of buffer.

Return:
    the number of bytes

--*/
int CSubtitle::GetChunk( void )
{
    int nbytes;

    if( !m_pSAMIChunk ) {
        m_pSAMIChunk = new char[SMI_BUFFER_SIZE];
    }
#if _CONSOLE_MODE_
    nbytes = read( fd, m_pSAMIChunk, SMI_BUFFER_SIZE );
#else
    err = fatfs_fread( 
            fd, 
            (RMuint8*)m_pSAMIChunk, 
            SMI_BUFFER_SIZE, 
            (RMuint32*)&nbytes );
    if( err ) {
        if( err==FATFS_ERROR_FILE_END_OF_FILE ) {
            fatfs_fclose( fd );
        }

        fatfs_fclose( fd );
        ASSERT(0);
    }
#endif
    m_chunksize = nbytes;
    m_chunkidx = 0;

    return nbytes;
}


/* ++

    GetChar()

Description:
    Get one character from SAMI chunk.

Arguments:
    None

Return:
    Returns a character. -1(0xffff) on EOF or error

-- */
int CSubtitle::GetChar( void )
{
    if( m_chunkidx == m_chunksize ) {
        if( !GetChunk() ) {
            // EOF
            return -1;
        }
    }

    // failed to allocate memory
    if( !m_pSAMIChunk ) {
        return -1;
    }
    
    return m_pSAMIChunk[m_chunkidx++];
}


/* ++

    GetColor( int index, int& color )

Description:
    Get color from color list.

Arguments:
    index: palette index.
    color: RGB 

Return:
    Return true on sucess otherwise false.

-- */
bool CSubtitle::GetColor( int index, int& color )
{
    if( index < 0 || index >= m_colors ) {
        return false;
    }

    color = m_colorlist[index];
    
    return true;
}
    

bool CSubtitle::ParseSMI()
{
#define MAX_STACK_SIZE  100
  int ch;
  int nspcnt = 0;
  int bufidx = 0;
  bool ret = true;
  bool bStartTag = false;
  bool bEndTag = false;
  bool bStartSpcChar = false;
  bool bEndSpcChar = false;
  SAMITagID tagStack[MAX_STACK_SIZE];
  int stacktop = 0;
  SAMITag tag;


  pPBuf = new char[PARSER_BUFFER_SIZE];
  m_pContent = new char[PARSER_BUFFER_SIZE];

  pNode = NULL;
  m_tagcount = 0;
  m_captionbufidx = 0;
  m_contentidx = 0;
  flushbytes = 0;
  
	while( 1 ) {
	    if( cur_lang >= MAX_SUPPORT_MULTI_LANG ) {
	      break;
	    }
	    ch = GetChar();
	    // EOF or Error
	    if( ch == -1 ) {
	        break;
	    }

	    switch( ch ) {
        case '<': // start tag
            bStartTag = true;
            break;
        case '>': // end tag
            bEndTag = true;
            break;
        case '#': // start special character
            bStartSpcChar = true;
            break;
        case ';': // end special character
            bEndSpcChar = true;
            break;
        case '&':
            ch = ParseSpecialChar();
            m_pContent[m_contentidx++] = ch;
            break;
        // skip white space
        case '\t':
        case '\r':
        case '\n':
            continue;
        case ' ': 
            // XML based Markup languages accepts only on space character.
            nspcnt++;
            if( nspcnt > 1 ) {
                break;
            }
        default:
            m_pContent[m_contentidx++] = ch;
	    }

	    if( ch != ' ' ) {
	        nspcnt = 0;
	    }

	    if( bStartTag ) {
	        bufidx = 0;
	        while( 1 ) {
	            ch = GetChar();
	            if( bufidx >= 8191 ) {
	                break;
	            }
	            if( !ch || ch == '>' ) {
	                break;
	            }
	            pPBuf[bufidx++] = ch;
	        }
          pPBuf[bufidx]= 0;
          bStartTag = false;

          if( MakeupTag( pPBuf, bufidx, tag ) == false ) {
            continue;
	        }
          m_tagcount++;

	        if( tag.endmark ) {
            if( stacktop > 0 ) {
	            SAMITagID id = tagStack[--stacktop];
	            if( id != tag.tagid ) {
                printf( "unmatch tag<%d, %d, %d>\n", 
                        id, tag.tagid, stacktop );
                ret = false;
                //break;
	            }
            }
	        }
	        else {
	            if( tag.tagid != SAMI_TAG_P &&
	                tag.tagid != SAMI_TAG_SYNC &&
	                tag.tagid != SAMI_TAG_BR &&
	                tag.tagid != SAMI_TAG_UNKNOWN ) {
    	            // push tag id to the tag stack.
    	            if( stacktop < MAX_STACK_SIZE ) {
    	                tagStack[stacktop++] = tag.tagid;
   	                }
  	            }
            }
            ParseTag( tag );
            if( m_tagcount == 1 ) {
                // Invalid SMI tag
                // SMI must start with <SAMI>.
                if( tag.tagid != SAMI_TAG_SAMI ) {
                    ret = false;
                    break;
                }
            }
        }
        
	    if( bEndTag ) {
	        printf( "End Tag\n" );
          printf( "Invalid SAMI tag(%d, %d)\n",
                  bStartTag, bEndTag );
          for( int i=0; i<20; i++ ) {
              printf( "%c", GetChar() );
          }
          printf( "\n" );
          // IGNORE PARSE ERROR!!!
          //ret = false;
          //break;
          bEndTag = false;
	    }

    }

    if( pPBuf ) {
        delete[] pPBuf;
    }
    if( m_pContent ) {
        delete[] m_pContent;
    }
    
	return ret;
}



int CSubtitle::CloseSMI()
{
  for( int i=0; i<MAX_SUPPORT_MULTI_LANG; i++ ) {
  	free( ciblk[i] );
  	ciblk[i] = NULL;
	}

	if( CaptionDataBlock != NULL ) {
		free( CaptionDataBlock );
		CaptionDataBlock = NULL;
	}

	return 1;
}

int CSubtitle::CurrentItemMSec()
{
  return merged_list[CurrentIndex].time;
}


Caption 
CSubtitle::GetCurrentCaption( int index )
{
  int i = merged_list[CurrentIndex].cap_idx[m_lidx];
  if( i<0 ) {
    Caption cap = { NULL, };
    return cap;
  }
	return ciblk[m_lidx][i].caption[index];
}

int 
CSubtitle::GetCurrentCaptionCount( void )
{
  int i = merged_list[CurrentIndex].cap_idx[m_lidx];
  
  return ciblk[m_lidx][i].lSubtitles;
}
	
int CSubtitle::ChunkToAdjacentCaption( int MSec )
{
	int CurCMS, PreCMS, NextCMS;

	for(;;) {
		if( CurrentIndex == 0 || CurrentIndex == LastIndex[m_lidx] ) {
			return FALSE;
		}
		CurCMS = merged_list[CurrentIndex].time+ (SyncSecShiftValue*1000);

		if( CurrentIndex == 0 ) {
			PreCMS = CurCMS;
		}
		else {
			PreCMS = merged_list[CurrentIndex-1].time + (SyncSecShiftValue*1000);
		}

		if( CurrentIndex == LastIndex[m_lidx]) {
			NextCMS = CurCMS;
		}		
		else {
			NextCMS = merged_list[CurrentIndex+1].time + (SyncSecShiftValue*1000);
		}

		if( MSec < PreCMS ) {
			// Case 1
			CurrentIndex--;
		}
		else if( MSec < CurCMS ) {
			// case 2
			return TRUE;
		}
		else if( MSec < NextCMS ) {
			// case 3
			CurrentIndex++;
		}
		else {
			return FALSE;
		}
	}

	return FALSE;
	
}


int CSubtitle::ChunkToNext()
{
	if( CurrentIndex == m_mgcnt ) {
		return 0;
	}
	return ++CurrentIndex;
}

int CSubtitle::ChunkToPrev( void ) 
{
    if( CurrentIndex == 0 ) {
        return 0;
    }

    return (--CurrentIndex);
}

int CSubtitle::ChunkToHead()
{
	return (CurrentIndex=0);
}

int CSubtitle::GetSyncSecShiftValue()
{
	return SyncSecShiftValue;
}
int CSubtitle::SetSyncSecShiftValue( int value )
{
	if( SyncSecShiftValue > 60 ||SyncSecShiftValue < -60 )
		return FALSE;
	else
		{
		SyncSecShiftValue = value;
		return TRUE;
		}
}

void 
CSubtitle::SetLanguage( int lidx )
{
  if( lidx >= MAX_SUPPORT_MULTI_LANG ) {
    lidx = 0;
  }
  m_lidx = lidx;
}

int 
CSubtitle::GetLanguages( void ) const 
{
  return(cur_lang+1);
}

const SAMILang* 
CSubtitle::GetLanguage( int lidx ) const 
{
  if( lidx > cur_lang ) return NULL;
  return &lang_name[lidx];
}

#if _CONSOLE_MODE_
int 
CSubtitle::PrintSMI()
{
  int i, k;
  printf( "###### COLOR LIST #####\n" );
  for( i=0; i<m_colors; i++ ) {
      printf( "m_colorlist[%02d]: 0x%06x\n", i, m_colorlist[i] );
  }
  printf( "\n" );

  printf( "##### LANGUAGES #####\n" );
  for( i=0; i<=cur_lang; i++ ) {
    printf( "class: %s, name: %s\n", 
            lang_name[i].classname,
            lang_name[i].langname );
  }
  printf( "\n" );

  for( k=0; k<=cur_lang; k++ ) {
  	for( i=0; i<LastIndex[k]; i++) {
  	    printf( "CaptionIndex[%04d] sec(%d): ", 
  	            i, ciblk[k][i].sec );
      for( int j=0; j<ciblk[k][i].lSubtitles; j++ ) {
    		printf("(%s: %d, %d)\n", 
    		    ciblk[k][i].caption[j].str,
    		    ciblk[k][i].caption[j].lColor,
    		    ciblk[k][i].caption[j].tag );
  		}
  	}
	}
	printf( "--------------------------------------------\n" );
	for( i=0; i<m_mgcnt; i++ ) {
	  printf( "[%04d]sec: %d\n", i, merged_list[i].time );
	  for( int j=0; j<2; j++ ) {
  	  printf( "\t" );
	    k = merged_list[i].cap_idx[j];
	    for( int l=0; k>-1&&l<ciblk[j][k].lSubtitles; l++ ) {
	      printf( "%s", ciblk[j][k].caption[l].str );
	    }
	    printf( "\n" );
	  }
	}

	return 1;
}

int
main( int argc, char **argv){	
	printf("\nTAVI SMI Parser Test\n\n");

    if( argc < 2 ) {
        printf( "sub [filename]\n" );
        return 1;
    }

	CSubtitle* smi = new CSubtitle;
   
	smi->LoadSMI( argv[1] );

	smi->PrintSMI();

	delete smi;


	return 1;
}
#endif


