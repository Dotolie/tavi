//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : M3U Library declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3/m3u.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: m3u.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/11/08 01:19:50  zstein
// *** empty log message ***
//
// Revision 1.10  2005/09/28 02:52:00  zstein
// - update UI & fixed bugs
//*******************************************************************************

#ifndef __M3U_LIBRARY_H__
#define __M3U_LIBRARY_H__

#include <stdio.h>

#include <Lib/LibWideString.h>

#define DEBUG
#ifdef DEBUG
#define M3U_DEBUGMSG		printf
#else
#define M3U_DEBUGMSG
#endif

typedef struct {
	char* m_szTitle;
	int m_nTitleUnicode;
	int m_nTitleLen;
	int m_nTime; /* total time. unit: sec */
	char* m_szPath;
	int m_nPathLen;
} M3UTag;
typedef int	M3UHandle;

enum {
	M3U_READ,
	M3U_WRITE,
	M3U_APPEND
};

extern M3UHandle M3UInit( const uchar_t* filename, int mode );
extern void M3UClose( M3UHandle handle );
extern M3UTag* M3UGetNext( M3UHandle handle );
extern int M3UAdd( M3UHandle handle, const M3UTag* tag );
extern int M3UMakeHead( M3UHandle handle );
extern M3UHandle M3UCreate( const uchar_t* path );
extern void M3UFreeTag( M3UTag** tag );


#endif /* __M3U_LIBRARY_H__ */
