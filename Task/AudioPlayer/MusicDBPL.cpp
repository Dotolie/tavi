#include <time.h>

#include <rmbasic.h>
#include <fatfs.h>

#include <Component/Stack.h>
#include <Component/String.h>
#include <Component/FileUtil.h>
#include <Task/tavi_global.h>
#include "MusicDBPL.h"

CMusicDBPL::CMusicDBPL( CDeviceMusicDB* db, int type, int sort, progresspfn pfn ) :
	CPlaylist( CString(""), sort, pfn )
{
	m_handle = db;
	m_type = type;
	m_same = false;
	MakeFolderlist();

	m_id = CPlaylist::ID3DB;
}

CMusicDBPL::~CMusicDBPL( void )
{
}

void CMusicDBPL::MakeArtistlist( void )
{
	int count;
	CArray< CString* > artistlist;
	CString* name;
	uchar_t buf[256];

	// Scan all aritists.
	void *plink = m_handle->m_a_ArtistList.GetHeadPtr();

	LPWSTR s_name = NULL;
	s_name = (LPWSTR)m_handle->m_a_ArtistList.Get_ItemFromPtrItem(plink);

	int i_count = m_handle->m_a_ArtistList.GetCount();

	for(int i=0;i<i_count;i++)
	{
		if(s_name)
		{
//			여기서 s_name 을 쓴다.
			name = new CString;
			name->SetString( (char*)s_name, true );
			artistlist.PushBack( name );
			plink = m_handle->m_a_ArtistList.Get_NextPtrItem(plink);
			if(plink==NULL)break;
			s_name = (LPWSTR)m_handle->m_a_ArtistList.Get_ItemFromPtrItem(plink);
		}
	}

	// scan albums of each artist
	uchar_t* u_name;
	FolderNode* pnode;
	for( int j=0; j<artistlist.Count(); j++ ) {
		u_name = ( uchar_t* )artistlist[j]->GetString();
		m_handle->Get_AlbumListFromArtist( u_name );

		count =(int)m_handle->m_a_CurrentAlbumList.GetCount();
		LPWSTR szName;
		for(int i=0;i<count;i++)
		{
			unsigned int *i_palbumkey = (unsigned int *)m_handle->m_a_CurrentAlbumList.GetAt(i);
			szName = m_handle->Get_AlbumName(*i_palbumkey);
			ustrcpy( buf, u_name );
			ustrcat( buf, (uchar_t*)"\01\0\0" );
			ustrcat( buf, (uchar_t*)szName );
			pnode = new FolderNode;
			pnode->path.SetString( (char*)buf, true );
			pnode->m_state = NOT_PLAYED;
			pnode->date = 0;
			pnode->playlist = 0;
			m_FolderList.PushBack( pnode );
 		}
	}
}

void CMusicDBPL::MakeAlbumlist( void )
{
	void *plink = m_handle->m_a_AlbumList.GetHeadPtr();
	FolderNode* pnode;

	LPWSTR s_name = NULL;
	s_name = (LPWSTR)m_handle->m_a_AlbumList.Get_ItemFromPtrItem(plink);

	int i_count = m_handle->m_a_AlbumList.GetCount();

	for(int i=0;i<i_count;i++)
	{
		if(s_name)
		{
//			여기서 s_name 을 쓴다.
			pnode = new FolderNode;
			pnode->date = 0;
			pnode->m_state = NOT_PLAYED;
			pnode->path.SetString( (char*)s_name, true );
			pnode->playlist = 0;
			m_FolderList.PushBack( pnode );
			
			plink = m_handle->m_a_AlbumList.Get_NextPtrItem(plink);
			if(plink==NULL)break;
			s_name = (LPWSTR)m_handle->m_a_AlbumList.Get_ItemFromPtrItem(plink);
		}
	}
}

void CMusicDBPL::MakeGenrelist( void )
{
	void *plink = m_handle->m_a_GenreList.GetHeadPtr();
	FolderNode* pnode;

	LPWSTR s_name = NULL;
	s_name = (LPWSTR)m_handle->m_a_GenreList.Get_ItemFromPtrItem(plink);

	int i_count = m_handle->m_a_GenreList.GetCount();

	for(int i=0;i<i_count;i++)
	{
		if(s_name)
		{
//			여기서 s_name 을 쓴다.
			pnode = new FolderNode;
			pnode->date = 0;
			pnode->m_state = NOT_PLAYED;
			pnode->path.SetString( (char*)s_name, true );
			pnode->playlist = 0;
			m_FolderList.PushBack( pnode );
			
			plink = m_handle->m_a_GenreList.Get_NextPtrItem(plink);
			if(plink==NULL)break;
			s_name = (LPWSTR)m_handle->m_a_GenreList.Get_ItemFromPtrItem(plink);
		}
	}
}

void CMusicDBPL::MakeAlltracklist( void )
{
	FolderNode* pnode = new FolderNode;
	pnode->date = 0;
	pnode->m_state = NOT_PLAYED;
	pnode->path.SetString( (char*)"All tracks", false );
	pnode->playlist = 0;
	m_FolderList.PushBack( pnode );
}

int CMusicDBPL::MakeFolderlist( void )
{
	int count = 0;
	if( !m_handle ) return 0;

	m_FolderList.FreeAll();
	switch( m_type ) {
	case ARTIST:
		MakeArtistlist();
		break;
	case ALBUM:
		MakeAlbumlist();
		break;
	case GENRE:
		MakeGenrelist();
		break;
	case ALLTRACK:
		MakeAlltracklist();
		break;
	}

	count = m_FolderList.Count();

	SortFolderlist();
	
	// test
	#if TRACE_DEBUG
	printf( "total: %d\n", count );
	for( int i=0; i<m_FolderList.Count(); i++ ) {
		printf( "FOLDER[%03d]: ", i );
		m_FolderList[i]->path.Print();
		printf( "\n" );
	}
	#endif

	return count;
}

void CMusicDBPL::MakeArtistPlaylist( void )
{
	// ONE-TIME-MAKE PLAYLIST if all track.
	if( m_type == ALLTRACK ) {
		if( m_Playlist.Count() ) return;
	}
	
	int size = m_pCur->path.GetSize();
	uchar_t* artist = new uchar_t[size+1];
	uchar_t* album = new uchar_t[size+1];
	memset( artist, 0, 2*(size+1) );
	memset( album, 0, 2*(size+1) );
	uchar_t* p = artist;
	uchar_t wch;
	int i, j=0;
	for( i=0; i<size; i++ ) {
		wch = (uchar_t)m_pCur->path[i];
		if( wch == 0x01 ) {
			p[j] = 0;
			j = 0;
			p = album;
		}
		else {
			p[j] = wch;
			j++;
		}
	}
	unsigned int  i_artistkey = m_handle->Generate_Key(artist);
	unsigned int i_album = m_handle->Generate_Key(album);
	ST_DEVICEGROUPHASH *st_phash = NULL;
	PLNode* pnode;
	
	st_phash = m_handle->Get_ArtistHash(i_artistkey);

	if(st_phash==NULL) return;

	ST_DEVICEMUSICINFO *st_musicinfo = NULL;
	//int i_count = st_phash->i_includemusiccount;
	//unsigned int i_musickey = st_phash->i_firstmusickey;
	char* szName;
	int match=0;
	st_musicinfo = st_phash->st_pfirstmusic;
	//for(int i=0;i<i_count;i++)
	while( st_musicinfo ) 
	{
		if(st_musicinfo->i_albumkey == i_album)
		{
			szName = (char*)st_musicinfo->st_musicfileinfo.s_ntitle;
			pnode = new PLNode;
			pnode->m_date = 0;
			pnode->m_name.SetString( szName, true );
			pnode->m_path = st_musicinfo->st_musicfileinfo.s_nfilepath;
			pnode->m_state = NOT_PLAYED;
			m_Playlist.PushBack( pnode );
			match++;
		}

		//i_musickey = st_musicinfo->i_next_artist_musickey;
		//if(i_musickey==0)break;
		st_musicinfo = ( ST_DEVICEMUSICINFO* )st_musicinfo->pnext_artist_music;
	}
}

void CMusicDBPL::MakeGroupPlaylist( void )
{
	ST_DEVICEGROUPHASH *st_phash = NULL;
	int i_groupkey = m_handle->Generate_Key( (uchar_t*)m_pCur->path );

	switch(m_type)
	{
	case ALBUM:
		st_phash = m_handle->Get_AlbumHash(i_groupkey);
		break;
	case GENRE :
		st_phash = m_handle->Get_GenreHash(i_groupkey);
		break;
	default:
		return;
	}

	if(st_phash==NULL) return;

	ST_DEVICEMUSICINFO *st_musicinfo = NULL;
	//int i_count = st_phash->i_includemusiccount;
	//unsigned int i_musickey = st_phash->i_firstmusickey;
	PLNode* pnode;
	st_musicinfo = st_phash->st_pfirstmusic;
	//for(int i=0;i<i_count;i++)
	while( st_musicinfo )
	{
		//st_musicinfo = m_handle->Get_Music(i_musickey);
		pnode = new PLNode;
		pnode->m_date = 0;
		pnode->m_name.SetString( (char*)st_musicinfo->st_musicfileinfo.s_ntitle, true );
		pnode->m_path = st_musicinfo->st_musicfileinfo.s_nfilepath;
		pnode->m_state = NOT_PLAYED;
		m_Playlist.PushBack( pnode );

		switch(m_type)
		{
		case ALBUM:
			//i_musickey = st_musicinfo->i_next_album_musickey;
			st_musicinfo = ( ST_DEVICEMUSICINFO* )st_musicinfo->pnext_album_music;
			break;
		case GENRE :
			//i_musickey = st_musicinfo->i_next_genre_musickey;
			st_musicinfo = ( ST_DEVICEMUSICINFO* )st_musicinfo->pnext_genre_music;
			break;
		default:
			st_musicinfo = NULL;
		}
		//if(i_musickey==0)break;
	}
}

void CMusicDBPL::MakeAllPlaylist( void )
{
	if( m_Playlist.Count() ) return;
	
	void *plink = m_handle->m_a_MusicInfo.GetHeadPtr();

	ST_DEVICEMUSICINFO *st_musicinfo = NULL;
	st_musicinfo = (ST_DEVICEMUSICINFO *)m_handle->m_a_MusicInfo.Get_ItemFromPtrItem(plink);

	int i_count = m_handle->m_a_MusicInfo.GetCount();
	PLNode* pnode;
	for(int i=0;i<i_count;i++)
	{
		if(st_musicinfo)
		{
			pnode = new PLNode;
			pnode->m_date = 0;
			pnode->m_name.SetString( (char*)st_musicinfo->st_musicfileinfo.s_ntitle, true );
			pnode->m_path = st_musicinfo->st_musicfileinfo.s_nfilepath;
			pnode->m_state = NOT_PLAYED;
			m_Playlist.PushBack( pnode );
			plink = m_handle->m_a_MusicInfo.Get_NextPtrItem(plink);
			if(plink==NULL)break;
			st_musicinfo = (ST_DEVICEMUSICINFO *)m_handle->m_a_MusicInfo.Get_ItemFromPtrItem(plink);
		}
	}

	return;
}

int CMusicDBPL::MakePlaylist( void )
{
	int count;

	switch( m_type ) {
	case ARTIST:
		m_Playlist.FreeAll();
		MakeArtistPlaylist();
		break;
	case ALBUM:
	case GENRE:
		m_Playlist.FreeAll();
		MakeGroupPlaylist();
		break;
	case ALLTRACK:
		MakeAllPlaylist();
		break;
	default:
		// SO WHAT
		printf( "INVALID TYPE: %d\n", m_type );
		return -1;
	}
	
	SortPlaylist();
	count = m_Playlist.Count();
	m_pCur->playlist = &m_Playlist;
	// test
	#if TRACE_DEBUG
	for( int i=0; i<m_Playlist.Count(); i++ ) {
		printf( "PLAYLIST[%03d]: ", i );
		m_Playlist[i]->m_name.Print();
		printf( "\n" );
	}
	#endif
	return count;
}


void CMusicDBPL::MakePlaypath( void )
{
	uchar_t drive[] = { '/', 'h', 'd', '1', 0 };
	m_currentfile = drive;
	m_currentfile.Add( m_Playlist[m_playindex]->m_path );
}

int CMusicDBPL::SetType( int type )
{
	if( m_type == type ) return 0; // same
	if( m_type < ARTIST ||
		m_type > ALLTRACK ) return -1; // invalid
	m_type = type;
	MakeFolderlist();
	m_Playlist.FreeAll();
	return 1;
}

bool CMusicDBPL::SetCurrentDir( const CString& path )
{
	if( m_type == ALLTRACK ) {
		m_pCur = m_FolderList[0];
		return true;
	}
	return CPlaylist::SetCurrentDir( path );
}

bool CMusicDBPL::SetCurrentFile( const CString& filename ) 
{
	int count = 0;
	bool find = false;
	for( int i=0; i<m_Playlist.Count(); i++ ) {
		#if TRACE_DEBUG
		printf( "compare: " );
		m_Playlist[i]->m_path.Print();
		printf( ", " );
		filename.Print();
		printf( "\n" );
		#endif
		if( ustrcmp((uchar_t*)m_Playlist[i]->m_path, (const uchar_t*)filename) == 0 ) {
			m_playindex = count;
			find = true;
			MakePlaypath();
			break;
		}
		count++;
	}

	return find;
}
