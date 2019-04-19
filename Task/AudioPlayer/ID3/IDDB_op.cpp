#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#include "libid3.h"
#include "rmbasic.h"
#include "fatfs.h" 
#include <Lib/LibWideString.h>
#include <Component/FileUtil.h>
#include <Component/Stack.h>
#include <Task/tavi_global.h>

#include "WMATag.h"
#include "OGGTag.h"
#include "IDDB_op.h"

#define DEBUG
using namespace NML;

static int ConvertUTF8toUnicode( uchar_t* u16String, int nMax, RMuint8* szUTF8 )
{
	uchar_t ch;
	int i=0, j=0, count;		
	if( !u16String || !szUTF8 ) return 0;	
	while( (ch=szUTF8[i++]) ) {
		count = ch >> 4;
		count = 4-(count^0x0f);
		if( count < 0 ) count = 1;
		switch( count ) {
		case 1:
			u16String[j] = ch;
			break;
		case 2:
			u16String[j] = (szUTF8[i++]&0x3f) | (ch&0x3f)<<6;
			break;
		case 3:
			u16String[j] = (szUTF8[i+1]&0x3f) | (szUTF8[i]&0x3f)<<6 | (ch&0x1f)<<12;
			i+=2;
			break;
		}

		if( j == nMax-1 ) break;
		j++;
	}	
	u16String[j] = 0;

	return j;
}

static int ParseID3( const char* path, IDNode* node )
{
	ID3Tag* tag;
	char* unknown = "unknown";

	node->nUnicode_A = 0;
	node->nUnicode_Al = 0;
	node->nUnicode_G = 0;
	node->nUnicode_T = 0;
	tag = new ID3Tag;
	memset( tag, 0, sizeof(ID3Tag) );
	if( GetID3v2Tag( tag, (uchar_t*)path, true ) ) {
		GetID3v1Tag( tag, (uchar_t*)path, true );
	}
	
	if( tag->ver_major == 1 ) {
		memcpy( node->szTitle, tag->title, sizeof(tag->title) );
		memcpy( node->szArtist, tag->artist, sizeof(tag->artist) );
		memcpy( node->szAlbum, tag->album, sizeof(tag->album) );
	}
	else {
		// title
		// check unknown title
		if( tag->title[0] == 0 && tag->title[1] == 0 ) {
			memset( node->szTitle, 0, sizeof(node->szTitle) );
		}
		else {
			if( tag->title[0] == 0xff && tag->title[1] == 0xfe ) { // unicode
				memcpy( node->szTitle, &(tag->title[2]), sizeof(tag->title)-2 );
				node->nUnicode_T = 1;
			}
			else {
				memcpy( node->szTitle, tag->title, sizeof(tag->title) );
			}
		}
		// artist
		// check unknown artist
		if( tag->artist[0] == 0 && tag->artist[1] == 0 ) {
			strcpy( node->szArtist, unknown );
		}
		else {
			if( tag->artist[0] == 0xff && tag->artist[1] == 0xfe ) { // unicode
				memcpy( node->szArtist, &(tag->artist[2]), sizeof(tag->artist)-2 );
				node->nUnicode_A = 1;
			}
			else {
				memcpy( node->szArtist, tag->artist, sizeof(tag->artist) );
			}
		}
		// album
		// check unknown album
		if( tag->album[0] == 0 && tag->album[1] == 0 ) {
			strcpy( node->szAlbum, unknown );
		}
		else {
			if( tag->album[0] == 0xff && tag->album[1] == 0xfe ) { // unicode
				memcpy	( node->szAlbum, &(tag->album[2]), sizeof(tag->album)-2 );
				node->nUnicode_Al = 1;
			}
			else {
				memcpy	( node->szAlbum, tag->album, sizeof(tag->album) );
			}
		}
	}
	if( tag->ver_major == 1 ) {
		char* szGenre;
		if( tag->genre[0] > NUM_GENRES-1 ) szGenre = unknown;
		else szGenre = g_szGenreList[tag->genre[0]];
		node->nUnicode_G = 0;
		strcpy( node->szGenre, szGenre );
	}
	else {
		memcpy( node->szGenre, tag->genre, sizeof(tag->genre) );
		char buf[10]={0,};
		if( node->szGenre[0] == '(' ) {
			int end=0, j=0;
			for( int i=1; i<10; i++ ) {
				if( node->szGenre[i] == ')' ) {
					end = 1;
					break;
				}
				if( node->szGenre[i] ) buf[j++] = node->szGenre[i];
			}
			int sidx = atoi(buf);
			if( end ) {
				if( sidx < NUM_GENRES ) 
				strcpy( node->szGenre, g_szGenreList[sidx] );
			}
		}
	}

	delete tag;

	return 0;
}

static int WMAOpen( const char* path ){
	RMuint32 fd;
	FATFS_ERROR error;
	error = fatfs_wfopen( (RMuint16*)path, _O_RDONLY, (RMuint32*)&fd );
	if( error != FATFS_ERROR_NO_ERROR ) return -1;

	return fd;
}static int WMAClose( int handle ){	return fatfs_fclose( (RMuint32)handle );
}static int WMARead( int handle, void* pBuf, int size ){
	fatfs_fread( (RMuint32)handle, (RMuint8*)pBuf, (RMuint32)size, (RMuint32*)&size );
	return size;
}
static int WMASeek( int handle, int pos, int whence ){
	FATFS_ERROR error;
	
	error = fatfs_fseek( (RMuint32)handle, (RMuint32)pos, (RMuint32)whence );
	if( error != FATFS_ERROR_NO_ERROR ) return -1;
	return pos;
}

static int ParseWMATag( const char* path, IDNode* node )
{
	CWMATag tag;
	FileOps_t fops = {	WMAOpen, WMARead,	 WMASeek, WMAClose };
	int len;
	UTF16_t* data;
	
	tag.RegisterFileOps( &fops );
	if( tag.Parse( path ) == false ) return -1;

	data = tag.GetArtist( len );
	if( data ) {
		if( len > sizeof(node->szArtist)-2 ) len = sizeof(node->szArtist)-2;
		memcpy( node->szArtist, data, len );
		node->nUnicode_A = 1;
	}
	else {
		strcpy( node->szArtist, "unknown" );
		node->nUnicode_A = 0;
	}
	data = tag.GetAlbum( len );
	if( data ) {
		if( len > sizeof(node->szAlbum)-2 ) len = sizeof(node->szAlbum)-2;
		memcpy( node->szAlbum, data, len );
		node->nUnicode_Al = 1;
	}
	else {
		strcpy( node->szAlbum, "unknown" );
		node->nUnicode_Al = 0;
	}
	data = tag.GetTitle( len );
	if( data ) {
		if( len > sizeof(node->szTitle)-2 ) len = sizeof(node->szTitle)-2;
		memcpy( node->szTitle, data, len );
		node->nUnicode_T = 1;
	}
	else {
		strcpy( node->szTitle, "unknown" );
		node->nUnicode_T = 0;
	}
	data = tag.GetGenre( len );
	if( data ) {
		if( len > sizeof(node->szGenre)-2 ) len = sizeof(node->szGenre)-2;
		memcpy( node->szGenre, data, len );
		node->nUnicode_G = 1;
	}
	else {
		strcpy( node->szGenre, "unknown" );
		node->nUnicode_G = 0;
	}
	
	return 0;
}

static int ParseOGGTag( const char* path, IDNode* node )
{
	COGGTag tag;
	char* data;
	int len;
	
	if( tag.Parse( path ) == false ) return -1;

	data = tag.GetArtist( len );
	if( data ) {
		if( len > sizeof(node->szArtist)-2 ) len = sizeof(node->szArtist)-2;
		ConvertUTF8toUnicode( (uchar_t*)node->szArtist, len, (RMuint8*)data );
		node->nUnicode_A = 1;
	}
	else {
		strcpy( node->szArtist, "unknown" );
		node->nUnicode_A = 0;
	}
	data = tag.GetAlbum( len );
	if( data ) {
		if( len > sizeof(node->szAlbum)-2 ) len = sizeof(node->szAlbum)-2;
		ConvertUTF8toUnicode( (uchar_t*)node->szAlbum, len, (RMuint8*)data );
		node->nUnicode_Al = 1;
	}
	else {
		strcpy( node->szAlbum, "unknown" );
		node->nUnicode_Al = 0;
	}
	data = tag.GetTitle( len );
	if( data ) {
		if( len > sizeof(node->szTitle)-2 ) len = sizeof(node->szTitle)-2;
		ConvertUTF8toUnicode( (uchar_t*)node->szTitle, len, (RMuint8*)data );
		node->nUnicode_T = 1;
	}
	else {
		strcpy( node->szTitle, "unknown" );
		node->nUnicode_T = 0;
	}
	data = tag.GetGenre( len );
	if( data ) {
		if( len > sizeof(node->szGenre)-2 ) len = sizeof(node->szGenre)-2;
		ConvertUTF8toUnicode( (uchar_t*)node->szGenre, len, (RMuint8*)data );
		node->nUnicode_G = 1;
	}
	else {
		strcpy( node->szGenre, "unknown" );
		node->nUnicode_G = 0;
	}

	return 0;
}

static void LoadFreeLink( IDDB_Desc* desc )
{
	int link = desc->pHeader->nFreeLinkHead;
	int offset, i=0;
	RMuint32 nBytes;
	FATFS_ERROR err;

	#if 0
	memset( desc->pFreeLinkList, -1, sizeof(short)*MAX_FILE_HASH_NUM );
	while( link != -1 ) {
		desc->pFreeLinkList[i++] = link;
		offset = IDDB_HASH_BLOCK_SIZE+link*IDDB_FNODE_SIZE;
		fatfs_fseek( (RMuint32)desc->nFD, offset, SEEK_SET );
		err = fatfs_fread( (RMuint32)desc->nFD, (RMuint8*)desc->pNode, IDDB_FNODE_SIZE, &nBytes );
		if( err != FATFS_ERROR_NO_ERROR ) break;
		link = desc->pNode->nFreeLink;
	}
	#endif
	offset = IDDB_HASH_BLOCK_SIZE+IDDB_FILE_HASH_NUM*IDDB_FNODE_SIZE;
	fatfs_fseek( desc->nFD, offset, SEEK_SET );
	fatfs_fread( desc->nFD, (RMuint8*)desc->pFreeLinkList, sizeof(short)*IDDB_FILE_HASH_NUM, &nBytes );
	desc->nLinkIdx = 0;
	#if 1
	int c = IDDB_FILE_HASH_NUM - desc->pHeader->nTotal;
	DBDEBUG( "FREE LINK COUNT: %d/%d\n", i, IDDB_FILE_HASH_NUM );
	for( i=0; i<c; i++ ) {
		DBDEBUG( "%04d ", desc->pFreeLinkList[i] );
		if( i && i%20 == 0 ) DBDEBUG( "\n" );
	}
	DBDEBUG( "\n" );
	#endif /* DEBUG */
}

static int InsertMusicFile( IDDB_Desc* hDB, char* arg, int media )
{
	uchar_t* szFilepath = ( uchar_t* )arg;
	IDNode* node;
	extern char* g_szGenreList[];
	int r, key;

	DBDEBUG( "+InsertMusicFile[" );
	//CString::Print( (uchar_t*)arg );
	DBDEBUG( "]\n" );
	int len = ustrlen( (uchar_t*)arg )*2;
	if( len > sizeof(node->szFullpath) ) len = sizeof(node->szFullpath)-1;

	// check exist file
	key = IDDB_GenKey( (char*)arg, len );
	if( IDDB_FindKey( hDB->f_HashTable, key ) != -1 ) { // If a given key is exist 
		DBDEBUG( "0x%x is exist\n", key );
		return -1;
	}
	
	node = IDDB_AllocFileNode( hDB );
	ustrcpy( (uchar_t*)node->szFullpath, (uchar_t*)arg );
	node->nPathLen = len;
	
	switch( media ) {
	case MEDIA_MP3:
		r = ParseID3( arg, node );
		break;
	case MEDIA_OGG:
		r = ParseOGGTag( arg, node );
		break;
	case MEDIA_WMA:
		r = ParseWMATag( arg, node );
		break;
	default:
		printf( "UNKNOWN MEDIA: %d\n", media );
		return -1;
	}
	if( !r ) {
		if( node->nUnicode_A ) node->nArtistSize = ustrlen( (uchar_t*)node->szArtist );
		else node->nArtistSize = strlen( node->szArtist );
		if( node->nUnicode_Al ) node->nAlbumSize = ustrlen( (uchar_t*)node->szAlbum );
		else node->nAlbumSize = strlen( node->szAlbum );
		if( node->nUnicode_G ) node->nGenreSize = ustrlen( (uchar_t*)node->szGenre );
		else node->nGenreSize = strlen( node->szGenre );
		if( node->nUnicode_T ) node->nTitleSize = ustrlen( (uchar_t*)node->szTitle );
		else node->nTitleSize = strlen( node->szTitle );
		node->nArtistSize = CString::ClearWhite( node->szArtist, node->nUnicode_A, node->nArtistSize );
		node->nTitleSize = CString::ClearWhite( node->szTitle, node->nUnicode_T, node->nTitleSize );
		node->nAlbumSize = CString::ClearWhite( node->szAlbum, node->nUnicode_Al, node->nAlbumSize );
		node->nGenreSize = CString::ClearWhite( node->szGenre, node->nUnicode_G, node->nGenreSize );
		
		// check null string
		if( node->nArtistSize == 0 ) {
			strcpy( node->szArtist, "unknown" );
			node->nArtistSize = 7;
		}
		if( node->nTitleSize == 0 ) {
			uchar_t* title = ( uchar_t* )GetFilename( (char*)node->szFullpath, true );
			int len = ustrlen( title );
			if( len >= sizeof(node->szTitle)/2 ) ustrncpy( (uchar_t*)node->szTitle, title, sizeof(node->szTitle)/2-1 );
			else ustrcpy( (uchar_t*)node->szTitle, title );
			node->nTitleSize = len*2;
			node->nUnicode_T = 1;
		}
		if( node->nAlbumSize == 0 ) {
			strcpy( node->szAlbum, "unknown" );
			node->nAlbumSize = 7;
		}
		if( node->nGenreSize == 0 ) {
			strcpy( node->szGenre, "unknown" );
			node->nGenreSize = 7;
		}
		#if 0
		DBDEBUG( "######### tags ########\n" );
		DBDEBUG( "artist: %s\n", node->szArtist );
		DBDEBUG( "album: %s\n", node->szAlbum );
		DBDEBUG( "genre: %s\n", node->szGenre );
		DBDEBUG( "title: %s\n", node->szTitle );
		DBDEBUG( "########################\n" );
		#endif
		r = IDDB_Insert( hDB, node, key );
	}
	else {
		printf( "falied to parse!!\n" );
	}
	DBDEBUG( "-InsertMusicFile\n" );
	
	return r;
}

/*
 * wide character only
 */
static int UpdatePass1( IDDB_Desc* hDB, const uchar_t* pathname, ProgressFN fn )
{
	char szCurPath[2048];
	CStack< CString > dirStack( 100 );
	CString szDirPath;
	uchar_t* wszPath;
	FATFS_ERROR sdFatfsErr;
	FATFS_DIR_ENTRY_INFO sdDirEntry;
	RMuint32 hDir;
	char delimeter[4] = { '/', 0, 0, 0 };
	CString szPath;
	int count = 0;

	int i;
	for( i=0; i<hDB->f_HashTable->nHashSize; i++ ) 
		hDB->f_HashTable->pht[i].nMark = 0;
	
	szPath.SetString( (char*)pathname, true );
	dirStack.Push( szPath );
	while( dirStack.IsEmpty() == false ) {
		szDirPath = dirStack.Pop();
		ustrcpy( (uchar_t*)szCurPath, (uchar_t*)szDirPath );
		wszPath = ( uchar_t* )szDirPath;
		sdFatfsErr = fatfs_wdopen( (RMuint16*)wszPath, (RMuint32*)&hDir );
		while( sdFatfsErr == FATFS_ERROR_NO_ERROR ) {
			sdFatfsErr = fatfs_dread( hDir, &sdDirEntry );
			if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) break;
			if( sdDirEntry.name[0] == '.' && sdDirEntry.name[1] == 0 && 
			     sdDirEntry.name[2] == 0 && sdDirEntry.name[3] == 0 ) continue;
			if( sdDirEntry.name[0] == '.' && sdDirEntry.name[1] == 0 &&
				sdDirEntry.name[2] == '.' && sdDirEntry.name[3] == 0 ) continue;
			ustrcpy( (uchar_t*)szCurPath, (uchar_t*)szDirPath );
			ustrcat( (uchar_t*)szCurPath, (uchar_t*)delimeter );
			ustrcat( (uchar_t*)szCurPath, (uchar_t*)sdDirEntry.name );

			if( sdDirEntry.attributes&ATTR_DIRECTORY ) {
				szPath.SetString( szCurPath, true );
				dirStack.Push( szPath );
			}
			else {
				int m = CheckMedia( szCurPath, true );
				switch( m ) {
				case MEDIA_MP3:
				case MEDIA_OGG:
				case MEDIA_WMA:
					break;
				default:
					continue;
				}
				int l = ustrlen( (uchar_t*)szCurPath );
				int k = IDDB_GenKey( szCurPath, 2*l );
				int b = IDDB_FindKey( hDB->f_HashTable, k );
				//CString::Print( (uchar_t*)szCurPath );
				//DBDEBUG( "CHECK KEY: 0x%x, BUCKET: %d\n", k, b );
				if( b != -1 ) {
					hDB->f_HashTable->pht[b].nMark = 1;
					//DBDEBUG( "MARK: bucket[%d]\n", b );
				}
			}
		}
		fatfs_dclose( hDir );
	}

	// Delete unmark node
	IDNode* p=0;
	HashTable* ht = hDB->f_HashTable;
	count = 0;
	for( i=0; i<ht->nHashSize; i++ ) {
		if( ht->pht[i].nKey != -1 && ht->pht[i].nMark == 0 ) { // delete
			//DBDEBUG( "DELETE: INDEX(%d), KEY(0x%x), MARK(%d)\n",
			//	i, ht->pht[i].nKey, ht->pht[i].nMark );
			IDDB_ScanMode( hDB );
			p = IDDB_FetchNode( hDB, ht->pht[i].nStartPointer );
			if( p ) {
				count++;
				IDDB_Delete( hDB, i, ht->pht[i].nKey, p );
				if( fn ) fn( false, (char*)wszPath, count );
			}
			else {
				//DBDEBUG( "UpdatePass1<%d>: FAILED TO FETCH NODE(%d)\n", 
				//	ht->pht[i].nStartPointer );
			}
		}
	}

	return count;
}

static int UpdatePass2( IDDB_Desc* hDB, const uchar_t* pathname, ProgressFN fn )
{
	char szCurPath[2048];
	CStack< CString > dirStack( 100 );
	CString szDirPath;
	uchar_t* wszPath;
	FATFS_ERROR sdFatfsErr;
	FATFS_DIR_ENTRY_INFO sdDirEntry;
	RMuint32 hDir;
	char delimeter[4] = { '/', 0, 0, 0 };
	CString szPath;
	int count = 0, nbytes;

	szPath.SetString( (char*)pathname, true );
	dirStack.Push( szPath );
	while( dirStack.IsEmpty() == false ) {
		szDirPath = dirStack.Pop();
		ustrcpy( (uchar_t*)szCurPath, (uchar_t*)szDirPath );
		wszPath = ( uchar_t* )szDirPath;
		sdFatfsErr = fatfs_wdopen( (RMuint16*)wszPath, (RMuint32*)&hDir );
		while( sdFatfsErr == FATFS_ERROR_NO_ERROR ) {
			sdFatfsErr = fatfs_dread( hDir, &sdDirEntry );
			if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) break;
			if( sdDirEntry.name[0] == '.' && sdDirEntry.name[1] == 0 && 
			     sdDirEntry.name[2] == 0 && sdDirEntry.name[3] == 0 ) continue;
			if( sdDirEntry.name[0] == '.' && sdDirEntry.name[1] == 0 &&
				sdDirEntry.name[2] == '.' && sdDirEntry.name[3] == 0 ) continue;
			ustrcpy( (uchar_t*)szCurPath, (uchar_t*)szDirPath );
			ustrcat( (uchar_t*)szCurPath, (uchar_t*)delimeter );
			ustrcat( (uchar_t*)szCurPath, (uchar_t*)sdDirEntry.name );

			if( sdDirEntry.attributes&ATTR_DIRECTORY ) {
				szPath.SetString( szCurPath, true );
				dirStack.Push( szPath );
			}
			else {
				int m = CheckMedia( szCurPath, true );
				switch( m ) {
				case MEDIA_MP3:
				case MEDIA_OGG:
				case MEDIA_WMA:
					break;
				default:
					continue;
				}
				wszPath = ( uchar_t* )(szCurPath+10);
				if( !InsertMusicFile( hDB, (char*)szCurPath, m ) ) {
					if( fn ) fn( true, (char*)wszPath, count );
				}
				count++;
			}
		}
		fatfs_dclose( hDir );
	}

	hDB->pHeader->nTotal = count;
	DBDEBUG( "PASS2 TOTAL: %d, nLinkIdx: %d\n", hDB->pHeader->nTotal, hDB->nLinkIdx );
	IDDB_UpdateMode( hDB );
	fatfs_fseek( (RMuint32)hDB->nFD, 0, SEEK_SET );
	fatfs_fwrite( (RMuint32)hDB->nFD, (RMuint8*)hDB->pCache, IDDB_HASH_BLOCK_SIZE, (RMuint32*)&nbytes );
	int offset = IDDB_HASH_BLOCK_SIZE+IDDB_FILE_HASH_NUM*IDDB_FNODE_SIZE;
	fatfs_fseek( (RMuint32)hDB->nFD, (RMuint32)offset, SEEK_SET );
	fatfs_fwrite( (RMuint32)hDB->nFD, (RMuint8*)&hDB->pFreeLinkList[hDB->nLinkIdx], 
		sizeof(short)*(IDDB_FILE_HASH_NUM-hDB->pHeader->nTotal), (RMuint32*)&nbytes );

	return count;
}

 int IDDB_Update( IDDB_Desc* hDB, const uchar_t* pathname, ProgressFN fn )
{
	int total;

	IDDB_ScanMode( hDB );
	LoadFreeLink( hDB );
	total = UpdatePass1( hDB, pathname, fn );
	total = UpdatePass2( hDB, pathname, fn );
	IDDB_Release( hDB );

	DBDEBUG( "total : %d\n", hDB->pHeader->nTotal );

	return total;
}

IDNode* IDDB_FetchID( IDDB_Desc* desc, int id, int start, int& bucket )
{
	static int b;
	int c, nbytes, offset;
	HashTable* ht;

 	switch( id ) {
	case HASH_ARTIST: ht = desc->a_HashTable; break;
	case HASH_ALBUM: ht = desc->al_HashTable; break;
	case HASH_GENRE: ht = desc->g_HashTable; break;
	case HASH_TITLE: ht = desc->f_HashTable; break;
	default: return NULL;
	}

	if( !start ) { // first bucket
		b = ht->nStart;
		c = b;
	}
	else {
		c = ht->pht[b].nNextSlot;
 		b = c;
	}
	if( c < 0 ) return NULL;
	bucket = b;
 	c = ht->pht[b].nStartPointer;
 
 	return IDDB_FetchNode( desc, c );
}
