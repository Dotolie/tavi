//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : OGG Tag Parser
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3/OGGTag.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: OGGTag.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/09/16 05:39:06  zstein
// - update db
//
//*******************************************************************************

#include <stdio.h>
#include <string.h>

#include <rmbasic.h>
#include <fatfs.h>

#include "OGGTag.h"

using namespace NML;

COGGTag::COGGTag( void )
{
	m_szAlbum = 0;
	m_szArtist = 0;
	m_szGenre = 0;
	m_szTitle = 0;
}

COGGTag::~COGGTag( void )
{
	if( m_szAlbum ) delete[] m_szAlbum;
	if( m_szArtist ) delete[] m_szArtist;
	if( m_szGenre ) delete[] m_szGenre;
	if( m_szTitle ) delete[] m_szTitle;
}

bool COGGTag::Parse( const char* path )
{
	RMuint32 fd;
	FATFS_ERROR err;
	char buf[30];
	char* tag;
	int pos;
	int size;
	int cnt;
	
	err = fatfs_wfopen( (RMuint16*)path, _O_RDONLY, &fd );
	if( err != FATFS_ERROR_NO_ERROR ) {
		m_nError = OGGTAG_ERROR_OPEN;
		printf( "failed to open(0x%x\n", err );
		return false;
	}
	tag = new char[1024];
	
	fatfs_fsize( fd, (RMuint32*)&size );
	fatfs_fread( fd, (RMuint8*)buf, 4, (RMuint32*)&pos );
	cnt = buf[3];
	buf[3] = 0;
	if( strcasecmp( buf, "ogg" ) ) goto __failed;

	//Find Comment header
	fatfs_fseek( fd, cnt, SEEK_CUR );
	for( int pos=0; pos<1024; pos++ ) {
		fatfs_fread( fd, (RMuint8*)buf, 1, (RMuint32*)&size );
		if( buf[0] != 0xFF ) {
			fatfs_fread( fd, (RMuint8*)buf, 1, (RMuint32*)&size );
			break;
		}
	}

	if( buf[0] != 0x03 ) {
		goto __failed;
	}

	fatfs_fread( fd, (RMuint8*)buf, 6, (RMuint32*)&size );
	if( strncmp(buf,"vorbis", 6) ) goto __failed;
	fatfs_fread( fd, (RMuint8*)&pos, sizeof(pos), (RMuint32*)&size );
	// read vendor tag
	fatfs_fread( fd, (RMuint8*)buf, pos, (RMuint32*)&size );
	// read the number of count;
	fatfs_fread( fd, (RMuint8*)&cnt, sizeof(cnt), (RMuint32*)&size );

	for( int i=0; i<cnt; i++ ) {
		// read size
		fatfs_fread( fd, (RMuint8*)&pos, sizeof(pos), (RMuint32*)&size );
		if( pos > 1024 ) goto __failed;
		if( (pos&0x0501) == 0x0501 ) break; // end of comment
		fatfs_fread( fd, (RMuint8*)tag, pos, (RMuint32*)&size );
		if( strncmp(tag, "TITLE", 5) == 0 ) {
			m_szTitle = new char[pos-5];
			memcpy( m_szTitle, tag+6, pos-6 );
			m_szTitle[pos-6] = 0;
			m_nTitleLen = pos-6;
		}
		else if( strncmp(tag, "ALBUM", 5) == 0 ) {
			m_szAlbum = new char[pos-5];
			memcpy( m_szAlbum, tag+6, pos-6 );
			m_szAlbum[pos-6] = 0;
			m_nAlbumLen = pos-6;
		}
		else if( strncmp(tag, "ARTIST", 6) == 0 ) {
			m_szArtist = new char[pos-6];
			memcpy( m_szArtist, tag+7, pos-7 );
			m_szArtist[pos-7] = 0;
			m_nArtistLen = pos-7;
		}
		else if( strncmp(tag, "GENRE", 5) == 0 ) {
			m_szGenre = new char[pos-6];
			memcpy( m_szGenre, tag+6, pos-6 );
			m_szGenre[pos-6] = 0;
			m_nGenreLen = pos-6;
		}
	}

	delete[] tag;
	
	return true;
__failed:
	delete[] tag;
	
	fatfs_fclose( fd );
	return false;
}

