//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : m3u definition
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3/m3u.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: m3u.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.13  2005/12/09 03:39:03  zstein
// *** empty log message ***
//
// Revision 1.12  2005/11/08 01:19:50  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <rmbasic.h>
#include <fatfs.h>
#include "m3u.h"
#include <Lib/LibWideString.h>
#include <Task/tavi_global.h>
#include <Component/FileUtil.h>
#include <Component/object.h>

#define M3U_MAGIC			"#EXTM3U"
#define M3U_INFO			"#EXTINF"

static int nBufIndex = 0;
static int nMaxSize;
static int nMode;
static CList< M3UTag > taglist;

M3UHandle M3UInit( const uchar_t* path, int mode )
{
	char buf[100] = {0,};
	RMuint32 fd;
	RMuint32 nBytes;
	FATFS_ERROR err;
	RMuint32 flag;

	switch( mode ) {
	case M3U_READ:
		flag = _O_RDONLY;
		break;
	case M3U_WRITE:
		flag = _O_WRONLY | _O_TRUNC;
		break;
	case M3U_APPEND:
		flag = _O_APPEND | _O_WRONLY;
		break;
	default:
		flag = _O_RDONLY;
	}

	nMode = mode;
	nBufIndex = 0;

	printf( "open: " );
	CString::Print( path );
	printf( "\n" );

	err = fatfs_wfopen ((RMuint16 *)path,  _O_RDONLY, &fd);
	if( err != FATFS_ERROR_NO_ERROR ) {
		M3U_DEBUGMSG( "failed to open file: %s(err:0x%02lx)\n", path, err );
		return 0;
	}
	// Check M3U header string
	//M3U_fgets( fd, buf );
	FATFS_fResetIndex();
	FATFS_fgets( fd, buf, sizeof(buf) );
	if( strncmp(buf, M3U_MAGIC, 7) ) {
		CString::Print( path );
		printf( " is not m3u file: %s\n", buf );
		fatfs_fclose( fd );
		return 0;
	}

	if( nMode != M3U_READ ) {
		// make list
		M3UTag* ptag;
		while( (ptag=M3UGetNext(fd)) ) {
			taglist.Add( ptag );
		}
		fatfs_fclose( fd );
		fatfs_wfopen( (RMuint16*)path,  flag, &fd );
		nBufIndex = 0;
	}

__end:
	return ( M3UHandle )fd;
}

void M3UClose( M3UHandle handle )
{
	fatfs_fclose( (RMuint32)handle );
	taglist.FreeAll();
	
	return;
}

// returns pointer. YOU MUST CALL M3UFreeTag TO FREE RETURN POINTER!!
M3UTag* M3UGetNext( M3UHandle handle )
{
	M3UTag* pTag;
	char* buf=NULL;
	char* p;
	char* pStart;
	char ch;
	int len, i;
	FATFS_ERROR err;
	RMuint32 nBytes;

	if( handle == NULL ) return NULL;
	if( nMode == M3U_WRITE ) return NULL;

	pTag = new M3UTag;
	//pTag = new M3UTag;
	if( !pTag ) {
		goto __error;
	}
	pTag->m_szPath = NULL;
	pTag->m_szTitle = NULL;
	
	buf = new char[1024];
	p = buf;
	//fgets( buf, sizeof(buf), handle );
	//if( M3U_fgets( handle, buf ) == NULL ) goto __error;
	if( !FATFS_fgets( handle, buf, 1024 ) ) goto __error;
	if( strncmp(buf, M3U_INFO, 7) ) {
		M3U_DEBUGMSG( "NOT M3U FILE(%s, pos:%d)\n", buf, nBufIndex );
		goto __error;
	}
	p += 7;
	if( *p != ':' ) {
		goto __error;
	}
	// Get total time
	p++;
	pStart = p;
	while( *p != ',' ) {
		p++;
	}
	*p = 0;
	pTag->m_nTime = atoi( pStart );
	//printf( "time: %d, %s\n", pTag->m_nTime, pStart );
	
	// Get title
	p++;
	pStart = p;
	while( *p != '\n' ) {
		p++;
	}
	*p = 0;
	if( *(p-1) == '\r' ) *(p-1) = 0;
	len = p - pStart + 1;
	pTag->m_szTitle = ( char* )malloc( len );
	//pTag->m_szTitle = new char[len];
	memcpy( pTag->m_szTitle, pStart, len );
	pTag->m_szTitle[len-1] = 0;
	
	// Get path
	memset( buf, 0, 1024 );
	i = FATFS_fgets( handle, buf, 1024 );
	len = ustrlen( (uchar_t*)buf );

	if( buf[i-1] == '\n' ) {
		i--;
		buf[i] = 0;
		if( buf[i-1] == '\r' ) { 
			i--;
			buf[i] = 0;
		}
	}
	pTag->m_szPath = new char[2*(len+1)];
	pTag->m_nPathLen = i;
	ustrcpy( (uchar_t*)pTag->m_szPath, (uchar_t*)buf );
	delete[] buf;
	return pTag;
	
__error:
	if( pTag ) {
		if( pTag->m_szPath ) free( pTag->m_szPath );
		if( pTag->m_szTitle ) free( pTag->m_szTitle );
		free( pTag );
		//if( pTag->m_szPath ) delete[] pTag->m_szPath;
		//if( pTag->m_szTitle ) delete[] pTag->m_szTitle;
		//delete pTag;
	}
	if( buf ) delete[] buf;

	return NULL;
}

int M3UMakeHead( M3UHandle handle )
{
	int nSize, nBytes;
	FATFS_ERROR err;
	char buf[10];
	
	nSize = sprintf( buf, "%s\r\n", M3U_MAGIC );
	err = fatfs_fwrite( (RMuint32)handle, (RMuint8*)buf, (RMuint32)nSize, (RMuint32*)&nBytes );
	if( err != FATFS_ERROR_NO_ERROR ) {
		TAVI_DEBUGMSG( "failed to write err(0x%x)\n", err );
		return -1;
	}

	return 0;
}

int M3UAdd( M3UHandle handle, const M3UTag* pTag )
{
	RMuint32 nSize, nBytes;
	FATFS_ERROR err;
	char* path = pTag->m_szPath;
	char buf[64];
	char* szBuf = new char[1024];

	if( nMode == M3U_READ ) return -1;

	M3UTag* tag;
	taglist.Rewind();
	tag = taglist.Head();
	while( tag ) {
		if( ustrcmp((uchar_t*)tag->m_szPath, (uchar_t*)pTag->m_szPath ) == 0 ) 
			return -2; 
		tag = taglist.Next();
	}
	
	nSize = sprintf( szBuf, "%s:%d," , M3U_INFO, pTag->m_nTime );
	//nBytes = ustrlen((uchar_t*)pTag->m_szTitle)*2;
	memcpy( &szBuf[nSize], (void*)pTag->m_szTitle, pTag->m_nTitleLen );
	nSize += pTag->m_nTitleLen;
	memcpy( &szBuf[nSize], "\r\n", 2 );
	nSize += 2;
	memcpy( &szBuf[nSize], (void*)pTag->m_szPath, pTag->m_nPathLen );
	nSize += pTag->m_nPathLen;
	memcpy( &szBuf[nSize], "\r\n", 2 );
	nSize += 2;	
	err = fatfs_fwrite( (RMuint32)handle, (RMuint8*)szBuf, nSize, &nBytes );
	if( err != FATFS_ERROR_NO_ERROR ) {
		TAVI_DEBUGMSG( "failed to write err(0x%x)\n", err );
		return -1;
	}

	delete[] szBuf;
	
	return 0;
}

M3UHandle M3UCreate( const uchar_t* path )
{
	RMuint32 fd;
	FATFS_ERROR err;

	nBufIndex = 0;
	nMode = M3U_WRITE;
	err = fatfs_wfopen ((RMuint16*)path,  _O_TRUNC | _O_WRONLY, &fd);
	if( err != FATFS_ERROR_NO_ERROR ) {
		err = fatfs_wfopen ((RMuint16*)path,  _O_WRONLY | _O_CREAT, &fd);
		if( err != FATFS_ERROR_NO_ERROR ) {
			printf( "M3UCreate: error(0x%x)\n", err );
			return 0;
		}
	}

	M3UMakeHead( (M3UHandle)fd );

	return (M3UHandle)fd;
}

void M3UFreeTag( M3UTag** tag ) 
{
	if( !*tag ) return;
	if( (*tag)->m_szTitle ) delete[] (*tag)->m_szTitle;
	if( (*tag)->m_szPath ) delete[] (*tag)->m_szPath;
	delete *tag;
	*tag = 0;
}

