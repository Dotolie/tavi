//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibWideString Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibWideString.h,v $
// $Revision: 1.2 $
// $Date: 2006/04/26 08:11:22 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibWideString.h,v $
// Revision 1.2  2006/04/26 08:11:22  zstein
// - ustrcpy( uchar_t* dst, uchar_t* src ) to ustrcpy( uchar_t*, const uchar_t* ).
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.5  2005/11/25 10:24:03  zstein
// - utoupper(...)Ãß°¡
//
// Revision 1.4  2005/10/05 09:43:19  etnlwind
// Update for
// 1. FileManager Copy/Move/Delete Related
// 2. VideoPlayer Bookmarking/Bookmark Loading/Resume Related
//
// Revision 1.3  2005/09/28 03:37:14  zstein
// - added ustrcasecmp.
//
// Revision 1.2  2005/07/26 04:50:02  etnlwind
// Update for FileManager Basic
//
// Revision 1.1  2005/04/20 11:29:27  etnlwind
// update for ES4
//
//*****************************************************************************

 #ifndef __LIB_WINDESTRING_H__
 #define __LIB_WINDESTRING_H__

#include <stdio.h>
#include <stdlib.h>

typedef unsigned short int 	uchar_t; 		// 0x0000 ~ 0xFFFF

int ustr2str( char* String, uchar_t* UnicodeString );
int str2ustr( uchar_t* UnicodeString, char* String, int StringLength );
int str2ustr( uchar_t* UnicodeString, char* String );

uchar_t* ustrcpy( uchar_t* dest, const uchar_t* src );
uchar_t* ustrncpy( uchar_t* dest, uchar_t* src, size_t n );
size_t ustrlen( const uchar_t *s );

uchar_t* ustrcat( uchar_t* dest, const uchar_t* src );
uchar_t* ustrncat( uchar_t* dest, const uchar_t* src, size_t n );

int ustrcmp( const uchar_t* s1, const uchar_t* s2 );
int ustrncmp( const uchar_t* s1, const uchar_t* s2, size_t n );
int ustrcasecmp( const uchar_t* s1, const uchar_t* s2 );
int utoupper( uchar_t* src );

void adduchar( uchar_t* ustr, const uchar_t uchr );
int addchar( uchar_t* ustr, const char chr );

#endif

