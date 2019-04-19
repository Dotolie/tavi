//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : File utility functions definition

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/FileUtil.cpp,v $

// $Revision: 1.2 $

// $Date: 2006/06/12 04:50:26 $

// $Author: ywkim $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: FileUtil.cpp,v $
// Revision 1.2  2006/06/12 04:50:26  ywkim
// Add media type of MEDIA_DCF_MP3 with ext name of "dcf" for melon
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.15  2005/12/09 03:24:29  zstein
// *** empty log message ***
//
// Revision 1.14  2005/11/07 01:24:20  zstein
// *** empty log message ***
//
// Revision 1.13  2005/09/14 02:29:38  zstein
// - change GetFilename( char* ) to GetFilename( char* , int )
//
// Revision 1.12  2005/06/01 12:43:37  zstein
// - bug fixed.
//
// Revision 1.11  2005/05/27 09:06:01  zstein
// - updated.
//
// Revision 1.10  2005/05/15 11:08:49  zstein
// *** empty log message ***
//
// Revision 1.9  2005/05/12 04:18:32  zstein
// *** empty log message ***
//
// Revision 1.8  2005/05/07 02:14:24  zstein
// support unicode
//
// Revision 1.7  2005/05/03 02:14:13  zstein
// *** empty log message ***
//
// Revision 1.6  2005/04/23 06:08:07  zstein
// added m-language
//
// Revision 1.5  2005/04/05 08:02:36  zstein
// *** empty log message ***
//
// Revision 1.4  2005/03/27 04:59:56  zstein
// *** empty log message ***
//
// Revision 1.3  2005/03/07 10:24:53  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/22 07:30:06  zstein
// fixed GetFileName() bug.
//
// Revision 1.1  2005/02/21 04:07:49  zstein
// File utility
//
//

//******************************************************************************

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "FileUtil.h"
#include "ListView.h"

#define FILE_BUFSIZE		4096
static char szFileBuf[FILE_BUFSIZE];
static int nBufIndex = 0;
static int nMaxSize;

void FATFS_fResetIndex( void )
{
	nBufIndex = 0;
}

int FATFS_fgets( RMuint32 handle, char* pBuf, int size )
{
	FATFS_ERROR sdFatfsErr;
	RMuint32 nBytes;
	char ch;
	int i;

	if( handle <= 0 ) return 0;
	i = 0;
	while( 1 ) {
		if( nBufIndex == 0 || nBufIndex >= nMaxSize ) {
			sdFatfsErr = fatfs_fread( handle, (RMuint8*)szFileBuf, FILE_BUFSIZE, &nBytes );
			if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
				return 0;
			}
			nBufIndex = 0;
			nMaxSize = nBytes;
		}
		ch = szFileBuf[nBufIndex++];
		pBuf[i++] = ch;
		if( ch == '\n' ) break;
		//if( i >= size ) break;
	}

	pBuf[i] = 0;
	return i;
}


char FATFS_fgetc( RMuint32 handle )
{
	FATFS_ERROR sdFatfsErr;
	RMuint32 nBytes;

	if( handle <= 0 ) return -1;
	
	if( nBufIndex == 0 || nBufIndex >= nMaxSize ) {
		sdFatfsErr = fatfs_fread( handle, (RMuint8*)szFileBuf, FILE_BUFSIZE, &nBytes );
		if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
			return 0;
		}
		nBufIndex = 0;
		nMaxSize = nBytes;
	}

	return szFileBuf[nBufIndex++];
}

int CheckMedia( const char* filename, bool fUnicode )
{
	if( !filename ) return MEDIA_UNKNOWN;

	char* ascii = NULL;
	const char* p;
	int len;
	int media = MEDIA_UNKNOWN;
	if( fUnicode ) {
		len = ustrlen( (uchar_t*)filename );
		ascii = new char[len+1];
		ustr2str( ascii, (uchar_t*)filename );
		p = ascii+len-1;
	}
	else {
		len = strlen( filename );
		ascii = ( char* )filename;
		p = filename+len-1;
	}

	int i;
	for( i=len-1; i>=0; i--, p-- ) {
		if( *p == '.' ) break;
	}
	if( i < 0 ) {
		media = MEDIA_UNKNOWN;
	}
	else {
		p++;
		if( strncasecmp( "mp3", p, 3 ) == 0 ) media = MEDIA_MP3;
		else if( strncasecmp( "jpg", p, 3 ) == 0 ) media = MEDIA_JPEG;
		else if( strncasecmp( "jpeg", p, 3 ) == 0 ) media = MEDIA_JPEG;
		else if( strncasecmp( "bmp", p, 3 ) == 0 ) media = MEDIA_BMP;
		else if( strncasecmp( "m3u", p, 3 ) == 0 ) media = MEDIA_M3U;
		else if( strncasecmp( "ogg", p, 3 ) == 0 ) media = MEDIA_OGG;
		else if( strncasecmp( "wma", p, 3 ) == 0 ) media = MEDIA_WMA;
		else if( strncasecmp( "pcm", p, 3 ) == 0 ) media = MEDIA_PCM;
		else if( strncasecmp( "png", p, 3 ) == 0 ) media = MEDIA_PNG;
		else if( strncasecmp( "gif", p, 3 ) == 0 ) media = MEDIA_GIF;
		else if( strncasecmp( "m3u", p, 3 ) == 0 ) media = MEDIA_M3U;
		else if( strncasecmp( "tpl", p, 3 ) == 0 ) media = MEDIA_TPL;
		else if( strncasecmp( "dcf", p, 3 ) == 0 ) media = MEDIA_DCF_MP3;
	}

	if( ascii != filename ) {
		delete[] ascii;
	}

	return media;
}


char* GetFilename( const char* szPath, int unicode, bool ext )
{
	int extpos = 0;
	int strpos = 0;
	static char szFilename[256] = {0,};
	int len;

	if( unicode ) { // unicode
		uchar_t* pwsz = ( uchar_t* )szPath;
		len = ustrlen( (uchar_t*)pwsz );
		for( int i=len-1; i>=0; i-- ) {
			if( pwsz[i] == '.' ) {
				if( extpos == 0 ) extpos = i;
			}
			if( !strpos && pwsz[i] == '/' ) {
				strpos = i+1;
				break;
			}
		}

		// no extension
		if( ext ) extpos = 0;
		if( extpos == 0 ) len = len - strpos + 1;
		else len = extpos - strpos + 1;
		ustrncpy( (uchar_t*)szFilename, (uchar_t*)(pwsz+strpos), len );
		pwsz = (uchar_t*)szFilename;
		pwsz[len-1] = 0;
	}
	else {
		len = strlen( szPath );
		for( int i=len-1; i>=0; i-- ) {
			if( szPath[i] == '.' ) extpos = i;
			if( !strpos && szPath[i] == '/' ) strpos = i+1;
		}
		if( ext ) extpos = 0;
		len = extpos - strpos + 1;
		strncpy( szFilename, (const char*)(szPath+strpos), len );
		szFilename[len-1] = 0;
	}

	return szFilename;
}

/*
 * @szPath: absolute path
 * @parent: recursive
 */
int CreateFolder( const char* szPath, int parent )
{
	const char* p = szPath;
	int len = strlen( szPath );
	char *pString;
	int i;

	pString = new char[len+1];
	#if 0
	if( mkdir(szPath, 0755) < 0 ) {
		if( !parent ) return -1;
	}

	pString = ( char* )malloc( len+1 );
	if( pString == NULL ) return -1;
	pString[len-1] = 0;
	i = 0;
	while( 1 ) {
		if( *p == 0 ) break;
		pString[i] = *p;
		if( *p == '/' ) {
			if( access(pString, F_OK) ) {
				printf( "%s not exist!!\n", pString );
				if( mkdir( pString, 0755 ) < 0 ) return -1;
			}
		}
		i++;
		p++;
	}
	free( pString );
	#else
	RMuint32 sdDirFd;
	FATFS_ERROR sdFatfsErr;
	char szName[256];
	int j;

	if( len < 5 ) return -1; // Invalid path. need absolute path
	if( szPath[0] != '/' ) return -1;

	sdFatfsErr = fatfs_dopen( (RMuint8*)"/hd1", &sdDirFd );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) return -1;
	i = 5;
	strncpy( pString, p, i );
	p += i;
	j = 0;
	while( 1 ) {
		if( *p == 0 ) {
			fatfs_dclose( sdDirFd );
			break;
		}
		if( *p == '/'  ) {
			pString[i] = 0;
			szName[j] = 0;
			fatfs_dclose( sdDirFd );
			sdFatfsErr = fatfs_dopen( (RMuint8*)pString, &sdDirFd );
			if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
				if( sdFatfsErr == FATFS_ERROR_DIR_NAME_NOT_FOUND ) {
					sdFatfsErr = fatfs_dcreate_dir( sdDirFd, (RMuint8*)szName );
					if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
						fatfs_dclose( sdDirFd );
						delete pString;
						return -1;
					}
					fatfs_dopen( (RMuint8*)pString, &sdDirFd );
				}
				else {
					printf( "error?? %s(%d)\n", pString, sdFatfsErr );
					delete pString;
					return -1;
				}
			}
			j = 0;
		}
		pString[i] = *p;
		szName[j++] = *p;
		i++;
		p++;
	}
	#endif

	delete pString;
	

	return 0;
}

int DeleteAllFiles( const char* folder )
{
	#if 0
	DIR* pDir;
	struct dirent* pEntry;
	struct stat info;
	char pString[1024];
	

	pDir = opendir( folder );
	if( !pDir ) return -1;

	while( (pEntry=readdir(pDir)) ) {
		if( strcmp(pEntry->d_name, ".") == 0 ) continue;
		if( strcmp(pEntry->d_name, "..") == 0 ) continue;
		sprintf( pString, "%s/%s", folder, pEntry->d_name );
		lstat( pString, &info );
		if( S_ISDIR(info.st_mode) ) continue;
		unlink( pString );
	}

	closedir( pDir );
	#else
	RMuint32 sdDirFd;
	FATFS_DIR_ENTRY_INFO sdDirEntry;
	FATFS_ERROR sdFatfsErr;
	char szNonUnicodeName[256];
	char *szTemp;

	sdFatfsErr = fatfs_dopen( (RMuint8*)folder, &sdDirFd );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		return -1;
	}

	szTemp = new char[4096];
	while( (sdFatfsErr=fatfs_dread(sdDirFd, &sdDirEntry)) == FATFS_ERROR_NO_ERROR ) {
		//Unicode2Char( sdDirEntry.name, szNonUnicodeName, sdDirEntry.namelength );
		if( strcmp(szNonUnicodeName, ".") == 0 ) continue;
		if( strcmp(szNonUnicodeName, "..") == 0 ) continue;
		sprintf( szTemp, "%s/%s", (char*)folder, szNonUnicodeName );

		if( sdDirEntry.attributes&0x10 ) continue;
		fatfs_ddelete_file( sdDirFd, (RMuint8*)szNonUnicodeName );
	}
	
	fatfs_dclose( sdDirFd );
	delete szTemp;
	#endif
	
	return 0;
}

/*
 * returns size in MegaByte.
 */
int GetFreeSpace( const char* dev )
{
	#if 0
	struct statfs s;
	int kblocks;
	
	if (statfs(dev, &s) != 0) {
		printf( "%s: do_df Error at %s", __FILE__, dev );
		return 0;
	}

	kblocks = s.f_bsize / 1024; // 1k-blocks

	return ( kblocks*s.f_bavail/1024 );
	#else
	RMuint64 nSize;
	int nSizeInMB;
	if( fatfs_get_vol_free_size( 1, &nSize ) != FATFS_ERROR_NO_ERROR ) return -1;

	nSizeInMB = nSize/(1024*1024);
	return nSizeInMB;
	#endif
}

/*
 * returns size in MegaByte
 */
int GetTotalSpace( const char* dev )
{
	#if 0
	struct statfs s;
	int kblocks;
	
	if (statfs(dev, &s) != 0) {
		printf( "%s: do_df Error at %s", __FILE__, dev );
		return 0;
	}

	kblocks = s.f_bsize / 1024; // 1k-blocks

	return ( kblocks*s.f_blocks/1024 );
	#else
	RMuint64 nSizeInByte;
	int nSizeInMB;

	if( fatfs_get_vol_size( 1, &nSizeInByte ) != FATFS_ERROR_NO_ERROR )
		return -1;

	nSizeInMB = nSizeInByte/(1024*1024);

	return nSizeInMB;
	#endif
}

/*
 * return 0 on success, otherwise return -1.
 */
int FATFS_access( const char* szPath )
{
	#if 0
	RMuint32 hDir;
	FATFS_ERROR sdFatfsErr;
	FATFS_DIR_ENTRY_INFO sdDirEntry;
	uchar_t pWName[256] = {0,};
	uchar_t* pWPath;
	int j, len;
	bool fWChar = false;
	int fAccess = -1;

	if( szPath[0] == 0 && szPath[1] ) {
		fWChar = true;
		len = ustrlen( pWPath );
	}
	else {
		len = strlen( szPath );
	}

	pWPath = new uchar_t[len+1];

	if( fWChar ) ustrcpy( pWPath, (uchar_t*)szPath );
	else str2ustr( pWPath, (char*)szPath, len );
	
	for( j=len-1; j>-1; j-- ) {
		if( pWPath[j] == '/' ) {
			if( j == len-1 ) continue;
			break;
		}
	}

	ustrncpy( pWName, pWPath+j+1, len-j-1 );
	for( ; j<len; j++ ) pWPath[j] = 0;

	sdFatfsErr = fatfs_wdopen( (RMuint16*)pWPath, &hDir );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		printf( "FATFS_access: fatfs_wdopen(err:%d)\n", sdFatfsErr );
		return 0;
	}
	while( (sdFatfsErr=fatfs_dread(hDir, &sdDirEntry)) == FATFS_ERROR_NO_ERROR ) {
		if( ustrcmp((uchar_t*)sdDirEntry.name, pWName) == 0 ) {
			fAccess = 0;
			break;
		}
	}


	if( fWChar == false ) delete[] pWPath;

	fatfs_dclose( hDir );

	if( fAccess == -1 ) printf( "%s not exist\n", szPath );
	else printf( "%s exist\n", szPath );
	
	return fAccess;
	#endif
	return 0;
}

char* GetPath( CString& pathname )
{
	int len = pathname.GetSize();
	int i;
	uchar_t* pwsz;
	char* path = NULL;
	if( pathname.IsUnicode() ) {
		pwsz = ( uchar_t* )pathname;
		for( i=len-1; i>-1; i-- ) {
			if( pwsz[i] == '/' ) break;
		}
		path = ( char* )malloc( (i+1)*2 );
		memcpy( path, pwsz, 2*i );
		((uchar_t*)path)[i+1] = 0;
	}
	else {
		char* p = ( char* )pathname;
		for( i=len-1; i>-1; i-- ) {
			if( p[i] == '/' ) break;
		}
		path = ( char* )malloc( i+1 );
		memcpy( path, p, i );
		path[i+1] = 0;
	}

	return path;
}

char* GetPath( uchar_t* pathname )
{
	int len = ustrlen(pathname);
	int i;
	uchar_t* pwsz;
	char* path = NULL;
	
	pwsz = ( uchar_t* )pathname;
	for( i=len-1; i>-1; i-- ) {
		if( pwsz[i] == '/' ) break;
	}
	path = ( char* )malloc( (i+1)*2 );
	memcpy( path, pwsz, 2*i );
	((uchar_t*)path)[i+1] = 0;

	return path;
}
