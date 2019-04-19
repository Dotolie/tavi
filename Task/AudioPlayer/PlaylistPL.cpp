#include <time.h>

#include "rmbasic.h"
#include "fatfs.h"

#include "Component/Stack.h"
#include "Component/String.h"
#include "Component/FileUtil.h"
#include "Task/tavi_global.h"
#include "ID3/m3u.h"
#include "PlaylistPL.h"
#include "musicdef.h"

CPlaylistPL::CPlaylistPL( const CString& path, int sort, progresspfn pfn ) :
	CPlaylist( CString(""), sort, pfn )
{
	m_base = PLAYLIST_BASE;
	m_base.toUTF16();
	
	MakeFolderlist();
	m_id = CPlaylist::PLAYLIST;
}

CPlaylistPL::~CPlaylistPL( void )
{
}

int CPlaylistPL::MakeFolderlist( void )
{
	RMuint32 fd;
	FATFS_ERROR err;
	FATFS_DIR_ENTRY_INFO info;
	FolderNode* pnode;
	int media;

	m_FolderList.FreeAll();
	err = fatfs_wdopen( (uchar_t*)m_base, &fd );
	if( err != FATFS_ERROR_NO_ERROR ) {
		printf( "err: %x\n", err );
		return -1;
	}
	while( err == FATFS_ERROR_NO_ERROR ) {
		err = fatfs_dread( fd, &info );
		if( err != FATFS_ERROR_NO_ERROR ) break;
		if( info.attributes&ATTR_DIRECTORY ) continue;
		media = CheckMedia( info.name, true );
		if( media == MEDIA_TPL ) {
			pnode = new FolderNode;
			pnode->path = ( uchar_t* )GetFilename( info.name, true );
			pnode->date = 0;
			pnode->playlist = 0;
			pnode->m_state = NOT_PLAYED;
			m_FolderList.PushBack( pnode );
			//printf( "add name: " );
			//pnode->path.Print();
			//printf( "\n" );
		}
	}
	fatfs_dclose( fd );
	int count = m_FolderList.Count();
	return count;
}

int CPlaylistPL::MakePlaylist( void )
{
	M3UHandle h;
	M3UTag* tag;
	PLNode* pnode;
	uchar_t path[MAX_PATH];
	uchar_t ext[] = { '.', 't', 'p', 'l', 0 };

	if( !m_pCur ) return 0;

	m_Playlist.FreeAll();

	ustrcpy( path, (uchar_t*)m_base );
	ustrcat( path, (uchar_t*)m_pCur->path );
	ustrcat( path, ext );
	h = M3UInit( path, M3U_READ );
	if( !h ) return 0;
	while( (tag = M3UGetNext( h )) ) {
		pnode = new PLNode;
		pnode->m_name = ( uchar_t* )tag->m_szTitle;
		pnode->m_state = NOT_PLAYED;
		pnode->m_path = ( uchar_t*)tag->m_szPath;
		m_Playlist.PushBack( pnode );
		M3UFreeTag( &tag );
	}
	M3UClose( h );
}

void CPlaylistPL::MakePlaypath( void )
{
	uchar_t mount[] = { '/', 'h', 'd', '1', 0 };

	if( !m_pCur ) return;
	
	m_currentfile = mount;
	m_currentfile.Add( m_Playlist[m_playindex]->m_path );
}

