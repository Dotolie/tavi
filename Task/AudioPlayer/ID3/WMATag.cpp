//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : WMA Tag 
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3/WMATag.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//*****************************************************************************
// $Log: WMATag.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/06/01 05:43:01  zstein
// *** empty log message ***
//
//*****************************************************************************

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "WMATag.h"

using namespace NML;

#define WM_ALBUM_TITLE			"W\0M\0/\0A\0l\0b\0u\0m\0T\0i\0t\0l\0e\0\0"
#define WM_ALBUM_TITLE_SIZE	28
#define WM_GENRE				"W\0M\0/\0G\0e\0n\0r\0e\0\0"
#define WM_GENRE_SIZE			18

static GUID_t _WMA_ASF_OBJ_GUIDs[2] = {
	// g1              g2         g3         g4
	{0x75B22630, 0x668E, 0x11CF, {0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}},
	{0x75b22636, 0x668e, 0x11cf, {0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c}}
};

static GUID_t _WMA_HEADER_OBJ_GUIDs[4] = {
	{0x8CABDCA1, 0xA947, 0x11CF, { 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 }},
	{0xB7DC0791, 0xA9B7, 0x11CF, { 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 }},
	{0x75b22633, 0x668e, 0x11cf, { 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c }},
	{0xD2D0A440, 0xE307, 0x11D2, { 0x97, 0xF0, 0x00, 0xA0, 0xC9, 0x5E, 0xA8, 0x50 }}
};

#if 0
void PrintGUID( GUID_t* pGuid )
{
	printf( "%02lx-%02lx-%02lx-", pGuid->g1, pGuid->g2, pGuid->g3 );
	printf( "%02lx%02lx-", pGuid->g4[0], pGuid->g4[1] );
	for( int i=0; i<6; i++ ) printf( "%02lx", pGuid->g4[2+i] );
	printf( "\n" );
}
#endif

CWMATag::CWMATag( void )
{
	m_pszAlbum = 0;
	m_pszArtist = 0;
	m_pszTitle = 0;
	m_pszGenre = 0;

	m_nError = WMA_ERROR_NO_ERROR;
}

CWMATag::~CWMATag( void )
{
	if( m_pszAlbum ) delete[] m_pszAlbum;
	if( m_pszArtist ) delete[] m_pszArtist;
	if( m_pszTitle ) delete[] m_pszTitle;
	if( m_pszGenre ) delete[] m_pszGenre;
}

void CWMATag::RegisterFileOps( FileOps_t* fops )
{
	m_Fops = *fops;
}

bool CWMATag::Parse( const char* szPath )
{
	int hFile;
	GUID_t guid;
	int nObjs;
	u_int64_t nObjSize;

	m_nError = WMA_ERROR_NO_ERROR;
	if( m_pszAlbum ) delete[] m_pszAlbum;
	if( m_pszArtist ) delete[] m_pszArtist;
	if( m_pszGenre ) delete[] m_pszGenre;
	if( m_pszTitle ) delete[] m_pszTitle;
	
	if( !m_Fops.open || ! m_Fops.close ||
		!m_Fops.read || !m_Fops.seek ) {
		m_nError = WMA_ERROR_NO_FILEOPS;
		return false;
	}

	if( (hFile=m_Fops.open(szPath)) < 0 ) {
		m_nError = WMA_ERROR_OPEN;
		return false;
	}

	// ASF Header Object
	m_Fops.read( hFile, (void*)&guid, sizeof(guid) );
	m_Fops.read( hFile, (void*)&nObjSize, sizeof(nObjSize) );
	m_Fops.read( hFile, (void*)&nObjs, sizeof(nObjs) );
	m_Fops.seek( hFile, 2, SEEK_CUR ); // reserved1, 2
	// Check if header is valid or not.
	if( memcmp((void*)&_WMA_ASF_OBJ_GUIDs[ASF_Header_Object], 
		(void*)&guid, sizeof(guid) ) ) goto __failed;

	while( 1 ) {
		m_Fops.read( hFile, (void*)&guid, sizeof(guid) );
		m_Fops.read( hFile, (void*)&nObjSize, sizeof(nObjSize) );
		//PrintGUID( &guid );
		//printf( "Object Size: %d\n", nObjSize );
		if( !memcmp((void*)&_WMA_HEADER_OBJ_GUIDs[ASF_File_Properties_Object],
			(void*)&guid, sizeof(guid)) ) {
			FileProp_t fProperty;
			m_Fops.read( hFile, (void*)&fProperty, sizeof(fProperty) );
			m_nPlayingTime = fProperty.playDuration/10000000L; // 
		}
		else if( !memcmp((void*)&_WMA_HEADER_OBJ_GUIDs[ASF_Stream_Properties_Object],
			(void*)&guid, sizeof(guid)) ) {
			m_Fops.seek( hFile, nObjSize-sizeof(GUID_t)-8, SEEK_CUR );
		}
		else if( !memcmp((void*)&_WMA_ASF_OBJ_GUIDs[ASF_Data_Object],
			(void*)&guid, sizeof(guid)) ) {
			break;
		}
		else if( !memcmp((void*)&_WMA_HEADER_OBJ_GUIDs[ASF_Content_Description_Object],
			(void*)&guid, sizeof(guid)) ) {
			ContentDescSize_t contentDesc;
			m_Fops.read( hFile, (void*)&contentDesc, sizeof(contentDesc) );
			int nTotal = contentDesc.TitleLength + contentDesc.AuthorLength +
				contentDesc.CopyrightLength + contentDesc.DescriptionLength +
				contentDesc.RatingLength;
			if( nTotal == 0 ) continue;
			u_int8_t* pData = new u_int8_t[nTotal];
			m_Fops.read( hFile, (void*)pData, nTotal );

			// retrieve title
			if( contentDesc.TitleLength ) {
				(u_int8_t*)m_pszTitle = new u_int8_t[contentDesc.TitleLength];
				memcpy( (void*)m_pszTitle, pData, contentDesc.TitleLength );
				m_nTitleLen = contentDesc.TitleLength;
			}
			// retrieve Author
			if( contentDesc.AuthorLength ) {
				(u_int8_t*)m_pszArtist = new u_int8_t[contentDesc.AuthorLength];
				memcpy( m_pszArtist, pData+contentDesc.TitleLength, contentDesc.AuthorLength );
				m_nArtistLen = contentDesc.AuthorLength;
			}
			delete[] pData;
		}
		else if( !memcmp((void*)&_WMA_HEADER_OBJ_GUIDs[ASF_Extended_Content_Description_Object],
			(void*)&guid, sizeof(guid)) ) {
			u_int16_t nDescCount;
			u_int16_t nNameLength;
			u_int16_t nDataType;
			u_int16_t nDataLength;
			UTF16_t* pName;
			u_int8_t* pValue;
			m_Fops.read( hFile, (void*)&nDescCount, sizeof(nDescCount) );
			// retrieve ONLY genre and album.
			for( int i=0; i<nDescCount; i++ ) {
				m_Fops.read( hFile, (void*)&nNameLength, sizeof(nNameLength) );
				( u_int8_t* )pName = new u_int8_t[nNameLength];
				m_Fops.read( hFile, (void*)pName, nNameLength );
				m_Fops.read( hFile, (void*)&nDataType, sizeof(nDataType) );
				m_Fops.read( hFile, (void*)&nDataLength, sizeof(nDataLength) );
				pValue = new u_int8_t[nDataLength];
				m_Fops.read( hFile, (void*)pValue, nDataLength );
				if( !memcmp(pName, WM_ALBUM_TITLE, WM_ALBUM_TITLE_SIZE) ) {
					( u_int8_t* )m_pszAlbum = new u_int8_t[nDataLength];
					memcpy( (void*)m_pszAlbum, (void*)pValue, nDataLength );
					m_nAlbumLen = nDataLength;
				}
				else if( !memcmp(pName, WM_GENRE, WM_GENRE_SIZE) ) {
					( u_int8_t*)m_pszGenre = new u_int8_t[nDataLength];
					memcpy( (void*)m_pszGenre, (void*)pValue, nDataLength );
					m_nGenreLen = nDataLength;
				}
				delete[] pName;
				delete[] pValue;
			}
		}
		else {
			if( m_Fops.seek( hFile, nObjSize-sizeof(GUID_t)-8, SEEK_CUR ) < 0 ) {
				m_nError = WMA_ERROR_SEEK;
				goto __failed;
			}
		}
	}
	
	return true;
__failed:
	m_Fops.close( hFile );
	return false;
}

#if 0

static int WMAOpen( const char* path )
{
	int fd = open( path, O_RDONLY );
	return fd;
}

static int WMAClose( int handle )
{
	return close( handle );
}

static int WMARead( int handle, void* pBuf, int size )
{
	return read( handle, pBuf, size );
}

static int WMASeek( int handle, int pos, int whence )
{
	return lseek( handle, pos, whence );
}

static int PrintUTF16( UTF16_t* wsz )
{
	int i;
	for( i=0; wsz[i]; i++ ) printf( "%c", wsz[i] );

	return i;
}

int main( int argc, char** argv )
{
	if( argc < 2 ) {
		printf( "%s <filename>\n", argv[0] );
		return 1;
	}

	CWMATag wmaTag;
	FileOps_t fops = {
		WMAOpen,
		WMARead,
		WMASeek,
		WMAClose		
	};

	wmaTag.RegisterFileOps( &fops );
	if( wmaTag.Parse( argv[1] ) == false ) {
		printf( "failed to parse %s\n", argv[1] );
		return 1;
	}

	printf( "Author: " );
	PrintUTF16( wmaTag.GetArtist() );
	printf( "\n" );
	printf( "Album: " );
	PrintUTF16( wmaTag.GetAlbum() );
	printf( "\n" );
	printf( "Genre: " );
	PrintUTF16( wmaTag.GetGenre() );
	printf( "\n" );
	printf( "Title: " );
	PrintUTF16( wmaTag.GetTitle() );
	printf( "\n" );
	printf( "Total time: %d\n", wmaTag.GetTotalTime() );

	return 0;
}
#endif

