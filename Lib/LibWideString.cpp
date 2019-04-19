//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibWideString Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibWideString.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/04/26 08:11:22 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibWideString.cpp,v $
// Revision 1.2  2006/04/26 08:11:22  zstein
// - ustrcpy( uchar_t* dst, uchar_t* src ) to ustrcpy( uchar_t*, const uchar_t* ).
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.9  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.8  2005/12/16 04:32:42  zstein
// - modified ustrncpy(...)
//
// Revision 1.7  2005/11/25 10:24:03  zstein
// - utoupper(...)Ãß°¡
//
// Revision 1.6  2005/10/07 06:38:22  zstein
// - fixed ustrcasecmp bug
//
// Revision 1.5  2005/10/05 09:43:19  etnlwind
// Update for
// 1. FileManager Copy/Move/Delete Related
// 2. VideoPlayer Bookmarking/Bookmark Loading/Resume Related
//
// Revision 1.4  2005/09/28 03:37:14  zstein
// - added ustrcasecmp.
//
// Revision 1.3  2005/07/26 04:50:02  etnlwind
// Update for FileManager Basic
//
// Revision 1.2  2005/06/01 07:02:39  zstein
// - bug fixed: ustrncpy(...) fixed boundary exception error.
//
// Revision 1.1  2005/04/20 11:29:27  etnlwind
// update for ES4
//
//*****************************************************************************

#include "LibWideString.h"

int ustr2str( char* String, uchar_t* UnicodeString )
{
	int i;

	for (i=0; i<1024; i++)
	{
		if( UnicodeString[0] == (uchar_t)0 )
			{
			break;
			}
		String[0] = UnicodeString[0];
		String++;
		UnicodeString ++;
	}
	String[0] = 0;

	return 1;	
}


// change a non-unicode string to unicode 
// (just truncates the upper byte)
int str2ustr( uchar_t* UnicodeString, char* String, int StringLength )
{
	int i;

	if( StringLength > 1024 )
		return 0;
	
	for (i=0; i<StringLength; i++)
	{
		UnicodeString[0] = (0<<8)|String[0];
		String++;
		UnicodeString++;
	}
	UnicodeString[0] = 0;

	return 1;
}
int str2ustr( uchar_t* UnicodeString, char* String )
{
	int i;
	int StringLength = strlen(String);

	if( StringLength > 1024 )
		return 0;
	
	for (i=0; i<StringLength; i++)
	{
		UnicodeString[0] = (0<<8)|String[0];
		String++;
		UnicodeString++;
	}
	UnicodeString[0] = 0;

	return 1;
}



uchar_t* ustrcpy( uchar_t* dest, const uchar_t* src )
{
	int i;
	for(i=0; i<1024; i++)
		{
		if( src[i] == (uchar_t)0 ) 
			{
			dest[i] = (uchar_t)0;
			break;
			}
		dest[i] = src[i];
		}
	return dest;
}

uchar_t* ustrncpy( uchar_t* dest, uchar_t* src, size_t n )
{
	int i;
	for(i=0; i<1024; i++)
		{
		if( !src[i] ) break;
		if( i >= n ) break;
		dest[i] = src[i];
		}
	dest[i] = 0;
	return dest;
}

size_t ustrlen( const uchar_t *s )
{
	int i;
	for(i=0; i<1024; i++)
		{
		if( s[i] == (uchar_t)0 ) 
			{
			return i;
			break;
			}
		}
	return (0);
}


uchar_t* ustrcat( uchar_t* dest, const uchar_t* src )
{
	int i, n;

	n = 0;
	n = ustrlen( dest );
	
	for(i=0; i<1024; i++)
		{
		if( src[i] == 0 ) 
			{
			dest[n+i] = 0;
			break;
			}
		dest[n+i] = src[i];
		}

	return dest;
}
uchar_t* ustrncat( uchar_t* dest, const uchar_t* src, size_t n )
{
	int i;

	n = ustrlen( dest );
	
	for(i=n; i<1024; i++)
		{
		if( i > n ) 
			{
			dest[i] = (uchar_t)0;
			break;
			}
		dest[i] = src[i];
		}
	return dest;
}

int ustrcmp( const uchar_t* s1, const uchar_t* s2 )
{
	int i;
	for(i=0; i<1024; i++)
		{

		if( s1[i] < s2[i] ) return -1;
		else if( s1[i] > s2[i] ) return 1;

		if( s1[i] == (uchar_t) 0 )
			if( s2[i] == (uchar_t) 0 )
				return 0;
		}
	return 0;
}


int ustrncmp( const uchar_t* s1, const uchar_t* s2, size_t n )
{
	int i;
	for(i=0; i<1024 && i<n; i++)
		{
		if( s1[i] < s2[i] ) return -1;
		else if( s1[i] > s2[i] ) return 1;
		}
	return 0;
}


void adduchar( uchar_t* ustr, const uchar_t uchr )
{
	int len = ustrlen( ustr );
	ustr[len+1] = ustr[len];
	ustr[len] = uchr;
}

// this func. Only works on ASCII char
// (just truncates the upper byte)
int addchar( uchar_t* ustr, const char chr )
{
	uchar_t uchr;
	
	if( chr > 256 )
		return 0;

	uchr = (0<<8)|chr;
	adduchar( ustr, uchr );
	return 1;
}

#include <ctype.h>
#define TOLOWER( __uchar ) 		( isupper(__uchar) ? tolower(__uchar) : __uchar )
int ustrcasecmp( const uchar_t* src, const uchar_t* dst )
{
	uchar_t s1, s2;
	int i = 0;

	if( src == dst ) return 0;
	
	do {
		s1 = TOLOWER( src[i] );
		s2 = TOLOWER( dst[i] );
		if( !s1 || !s2 ) break;
		i++;
	} while ( s1 == s2 );

	return ( s1-s2 );
}

int utoupper( uchar_t* src )
{
	int count = 0;
	int i = 0;
	while( 1 ) {
		if( !src[i] ) break;
		if( islower(src[i]) ) {
			src[i] = toupper( src[i] );
			count++;
		}
		i++;
	}

	return count;
}
