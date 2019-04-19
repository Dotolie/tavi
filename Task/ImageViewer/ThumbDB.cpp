#include <rmbasic.h>
#include <fatfs.h>
#include <Task/tavi_global.h>
#include "ThumbDB.h"

extern void TAVI_WakeUp( int );

static unsigned int Generate_Key( const char* s_sz)
{
	unsigned int i_length=(unsigned int)ustrlen((uchar_t*)s_sz)*2;
	unsigned int i_key=0;
	unsigned int i;
	unsigned char* p = ( unsigned char* )s_sz;
	
	if( !p ) return 0;

	for (i=0; i<i_length; ++i) 
	{
		i_key += p[i];
		i_key += (i_key << 10);
		i_key ^= (i_key >> 6);
	}
	i_key += (i_key << 3);
	i_key ^= (i_key >> 11);
	i_key += (i_key << 15);

	return i_key;
}

CThumbDB::CThumbDB( void )
{
	m_hFile = -1;
	m_pHash = 0;
}

//CThumbDB::CThumbDB( const CString& path )
//{
//	Load( path );
//}

CThumbDB::~CThumbDB( void )
{
	UnLoad();
}

//bool CThumbDB::Load( const CString& path )
bool CThumbDB::Load( void )
{
	FATFS_ERROR err = FATFS_ERROR_NO_ERROR;
	//CString thumb( THUMBDB_FILE );
	CString fullpath( THUMBDB_FILE );

	//thumb.toUTF16();
	//fullpath = path;
	//fullpath.Add( (uchar_t*)"/\0\0\0" );
	//fullpath.Add( thumb );
	fullpath.toUTF16();
	
	printf( "CThumbDB::Load( " );
	fullpath.Print();
	printf( " )\n" );
	
	m_pHash = 0;
	TAVI_WakeUp( 1000 );
	err = fatfs_wfopen( (uchar_t*)fullpath, _O_RDONLY, (RMuint32*)&m_hFile );
	if( err != FATFS_ERROR_NO_ERROR ) {
		m_hFile = -1;
		return false;
	}
	
	// check id
	int nbytes;
	err = fatfs_fread( (RMuint32)m_hFile, (RMuint8*)&m_header, sizeof(m_header), (RMuint32*)&nbytes );
	if( strcmp(m_header.s_cpcode, THUMBDB_ID) ) {
		fatfs_fclose( (RMuint32)m_hFile );
		m_hFile = -1;
		return false;
	}

	printf( "NML THUMBDB Ver %d.%d\n", 
		m_header.i_majorversion, m_header.i_minorversion );
	printf( "TOTAL COUNT: %d\n", m_header.i_totalcount );

	m_pHash = new ST_THUMBHASH[THUMBHASHSIZE];

	// load hash table
	err = fatfs_fread( (RMuint32)m_hFile, (RMuint8*)m_pHash, 
			THUMBHASHSIZE*sizeof(ST_THUMBHASH), (RMuint32*)&nbytes );

	return true;
}

bool CThumbDB::UnLoad( void )
{
	if( m_pHash ) delete[] m_pHash;
	if( m_hFile != -1 ) fatfs_fclose( (RMuint32)m_hFile );

	m_pHash = 0;
	m_hFile = -1;
}

//bool CThumbDB::FetchThumb( const CString& path, char* pY, char* pUV )
bool CThumbDB::FetchThumb( const uchar_t* path, char* pY, char* pUV )
{
	unsigned int key;
	int len;
	int bucket;
	int count;

	if( m_hFile == -1 ) return false;

	TAVI_WakeUp( 1000 );
	//len = path.GetSize() * ((int)path.IsUnicode() + 1 );
	path += 4;
	
	len = ustrlen( path );
	utoupper( (uchar_t*)path );
	key = Generate_Key((const char*)path);
	bucket = key % THUMBHASHSIZE;
	//printf( "fetch: " );
	//CString::Print( path );
	//printf( "\n" );

	bool find = false;
	count = 0;
	while( 1 ) {
		if( m_pHash[bucket].i_thumbkey == key ) {
			find = true;
			break;
		}
		bucket++;
		bucket %= THUMBHASHSIZE;
		
		count++;
		if( count == THUMBHASHSIZE ) break;
	}


	if( !find ) return false;

	int index = m_pHash[bucket].i_thumbindex;
	int offset = sizeof(ST_P_HEADER) + THUMBHASHSIZE*sizeof(ST_THUMBHASH) + 
			index*THUMB_M_SIZE;
	int nbytes;
	FATFS_ERROR err;
	err = fatfs_fseek( (RMuint32)m_hFile, offset, SEEK_SET );
	err = fatfs_fread( (RMuint32)m_hFile, (RMuint8*)pY, THUMB_M_YSIZE, (RMuint32*)&nbytes );
	err = fatfs_fread( (RMuint32)m_hFile, (RMuint8*)pUV, THUMB_M_UVSIZE, (RMuint32*)&nbytes );
	if( err != FATFS_ERROR_NO_ERROR ) return false;
	
	return true;
}
