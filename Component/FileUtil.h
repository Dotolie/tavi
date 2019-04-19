//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : File utility functions declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/FileUtil.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/12 04:50:26 $
// $Author: ywkim $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: FileUtil.h,v $
// Revision 1.2  2006/06/12 04:50:26  ywkim
// Add media type of MEDIA_DCF_MP3 with ext name of "dcf" for melon
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/12/09 03:24:29  zstein
// *** empty log message ***
//
// Revision 1.10  2005/11/07 01:24:21  zstein
// *** empty log message ***
//
// Revision 1.9  2005/09/14 02:29:38  zstein
// - change GetFilename( char* ) to GetFilename( char* , int )
//*******************************************************************************

#ifndef __FILE_UTILITY_H__
#define __FILE_UTILITY_H__

#include <Lib/LibWideString.h>
#include <rmbasic.h>
#include <fatfs.h>
#include "String.h"

typedef enum {
	MEDIA_UNKNOWN,
	MEDIA_MP3,
	MEDIA_WMA,
	MEDIA_OGG,
	MEDIA_DCF_MP3,
	MEDIA_PCM,
	MEDIA_JPEG,
	MEDIA_BMP,
	MEDIA_PNG,
	MEDIA_GIF,
	MEDIA_TIFF,
	MEDIA_M3U,
	MEDIA_TPL,
} MediaType;

extern int CheckMedia( const char* filename, bool fUnicode );
extern char* GetFilename( const char* szPath, int unicode=1, bool ext=false );
extern int CreateFolder( const char* szpath, int parent );
extern int DeleteAllFiles( const char* folder );
extern int GetFreeSpace( const char* dev );
extern int GetTotalSpace( const char* dev );
//extern void Unicode2Char (char *unicode, char *name, int l);
extern int FATFS_fgets( RMuint32 handle, char* pBuf, int size );
extern char FATFS_fgetc( RMuint32 handle );
extern int FATFS_access( const char* szPath );
extern void FATFS_fResetIndex( void );
extern char* GetPath( CString& pathname );
extern char* GetPath( uchar_t* pathname );

#endif /* __FILE_UTILITY_H__ */
