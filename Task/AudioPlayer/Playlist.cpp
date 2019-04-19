#include <time.h>

#include "Playlist.h"

#define MARK_PLAYING_NOW		(m_Playlist[m_playindex]->m_state = PLAYING)

static int _f_name_compare_asc( const void* a, const void* b )
{
	CPlaylist::FolderNode* pa = *( CPlaylist::FolderNode** )a;
	CPlaylist::FolderNode* pb = *( CPlaylist::FolderNode** )b;

	return ustrcmp( (uchar_t*)pa->path, (uchar_t*)pb->path );
}

static int _f_name_compare_desc( const void* a, const void* b )
{
	CPlaylist::FolderNode* pa = *( CPlaylist::FolderNode** )a;
	CPlaylist::FolderNode* pb = *( CPlaylist::FolderNode** )b;

	return ustrcmp( (uchar_t*)pb->path, (uchar_t*)pa->path );
}

static int _f_date_compare_asc( const void* a, const void* b )
{
	CPlaylist::FolderNode* pa = *( CPlaylist::FolderNode** )a;
	CPlaylist::FolderNode* pb = *( CPlaylist::FolderNode** )b;

	return (pa->date-pa->date);
}

static int _f_date_compare_desc( const void* a, const void* b )
{
	CPlaylist::FolderNode* pa = *( CPlaylist::FolderNode** )a;
	CPlaylist::FolderNode* pb = *( CPlaylist::FolderNode** )b;

	return (pb->date-pb->date);
}

static int _p_name_compare_asc( const void* a, const void* b )
{
	CPlaylist::PLNode* pa = *( CPlaylist::PLNode** )a;
	CPlaylist::PLNode* pb = *( CPlaylist::PLNode** )b;
	return ustrcmp( (uchar_t*)pa->m_name, (uchar_t*)pb->m_name );
}

static int _p_name_compare_desc( const void* a, const void* b )
{
	CPlaylist::PLNode* pa = *( CPlaylist::PLNode** )a;
	CPlaylist::PLNode* pb = *( CPlaylist::PLNode** )b;
	return ustrcmp( (uchar_t*)pb->m_name, (uchar_t*)pa->m_name );
}

static int _p_date_compare_asc( const void* a, const void* b )
{
	CPlaylist::PLNode* pa = *( CPlaylist::PLNode** )a;
	CPlaylist::PLNode* pb = *( CPlaylist::PLNode** )b;
	return( pa->m_date-pb->m_date );
}

static int _p_date_compare_desc( const void* a, const void* b )
{
	CPlaylist::PLNode* pa = *( CPlaylist::PLNode** )a;
	CPlaylist::PLNode* pb = *( CPlaylist::PLNode** )b;
	return( pb->m_date-pa->m_date );
}

CPlaylist::CPlaylist( const CString& path, int sort, progresspfn prg )
{
	m_root = path;
	if( sort > DATE_DESCENT || sort < NAME_ASCENT ) 
		m_sort = NAME_ASCENT;
	else m_sort = sort;

	m_playindex = -1;
	m_dirindex = -1;
	m_prg = prg;
}

CPlaylist::~CPlaylist( void )
{
}

void CPlaylist::SetShuffle( bool shuffle )
{ 
	m_shuffle = shuffle; 
	//printf( "SHUFFLE: %d\n", m_shuffle );
}

void CPlaylist::SetRepeat( bool repeat )
{ 
	m_repeat = repeat; 
	//printf( "REPEAT: %d\n", m_repeat );
}

void CPlaylist::SetBoundary( int boundary ) 
{ 
	if( boundary < BOUNDARY_ALL ||
		boundary > BOUNDARY_ONE ) return;
	m_boundary = boundary; 
	//printf( "boundary: %d\n", m_boundary );
}

bool CPlaylist::SetCurrentFile( const CString& filename ) 
{
	int count = 0;
	bool find = false;

	for( int i=0; i<m_Playlist.Count(); i++ ) {
		#if TRACE_DEBUG
		printf( "compare: " );
		m_Playlist[i]->m_name.Print();
		printf( ", " );
		filename.Print();
		printf( "\n" );
		#endif
		if( m_Playlist[i]->m_name == filename ) {
			m_playindex = count;
			find = true;
			MakePlaypath();
			break;
		}
		count++;
	}

	printf( "find: %d, index: %d\n", find, m_playindex );
	return find;
}

bool CPlaylist::SetCurrentDir( const CString& path )
{
	bool success = false;
	int count = 0;

	for( int i=0; i<m_FolderList.Count(); i++ ) {
		#if TRACE_DEBUG
		printf( "compare: " );
		m_FolderList[i]->path.Print();
		printf( ", " );
		path.Print();
		printf( "\n" );
		#endif
		if( m_FolderList[i]->path == path ) {
			// If not a working category.
			if( m_pCur != m_FolderList[i] ) {
				printf( "NOT SAME\n" );
				m_FolderList[i]->playlist = NULL;
			}
			m_pCur = m_FolderList[i];
			success = true;
			m_dirindex = count;
			break;
		}
		count++;
	}

	#if 0
	printf( "SetCurrentDir: " );
	if( m_pCur ) m_pCur->path.Print();
	else printf( "NULL" );
	printf( "\n" );
	#endif

	return success;
}


char* CPlaylist::BoundaryOne( void )
{
	char* name = NULL;
	if( m_repeat ) name = ( char* )m_currentfile;

	return name;
}

char* CPlaylist::BoundaryAll( int dir )
{
	char* name = NULL;
	int dirindex;
	int total = m_FolderList.Count();

	#if TRACE_DEBUG
	printf( "BoundaryAll(dir: %d)\n", dir );
	#endif

	while( !(name=BoundaryCategory(dir, false)) ) {
		total--;
		// Each of all folders has no music files.
		if( total < 0 ) break;
		dirindex = MoveFolder( dir );
		if( dirindex != -1 ) {
			MakePlaylist();
			if( dir ) m_playindex = -1;
			else m_playindex = m_Playlist.Count();
			m_dirindex = dirindex;
		}
		else break;
	}
	
	return name;
}

// dir: 0 prev, 1 next
char* CPlaylist::BoundaryCategory( int dir, bool repeat )
{
	char* name = NULL;
	int index;
	int total = m_Playlist.Count();
	#if TRACE_DEBUG
	printf( "BoundaryCategory(%d, %d), m_playindex: %d, total: %d\n", 
		dir, repeat, m_playindex, total );
	#endif
	
	if( m_shuffle ) {
		index = DoShuffleFiles();
		if( index == -1 ) {
			if( repeat ) {
				ClearPlaylistState();
				index = DoShuffleFiles();
			}
		}
	}
	else {
		index = m_playindex;
		if( dir ) {
			index++;
			if( index >= m_Playlist.Count() ) {
				if( repeat ) index = 0;
				else index = -1;
			}
		}
		else {
			index--;
			if( index < 0 ) {
				if( m_repeat ) index = total-1;
				else index = -1;
			}
		}
	}
	
	if( index != -1 ) {
		m_playindex = index;
		MakePlaypath();
		MARK_PLAYING_NOW;
		name = ( char* )m_currentfile;
	}

	if( !name ) m_pCur->m_state = PLAYDONE;
	#if TRACE_DEBUG
	printf( "BoundaryCategory>>index: %d: ", index );
	m_currentfile.Print();
	printf( "\n" );
	#endif
	
	return name;
}

int CPlaylist::DoShuffleFiles( void )
{
	time_t tm;
	int total = m_Playlist.Count();
	
	tm = time( NULL );
	srand( (int)tm );
	int index = rand() % total;
	int i, c=index;
	bool find = true;
	
	printf( "DoShuffle: index(%d), total(%d)\n", index, total );
	if( m_Playlist[index]->m_state != NOT_PLAYED ) {
		for( i=0; i<total; i++ ) {
			c++;
			if( c == total ) c = 0;
			//printf( "m_Playlist[%d].m_state: %d\n", c, m_Playlist[c]->m_state );
			if( m_Playlist[c]->m_state == NOT_PLAYED ) break;
		}
		if( i == total ) find = false;
	}

	if( find ) index = c;
	else index = -1;
	printf( "DoSuffleFiles: find(%d)\n", find );

	return index;
}

int CPlaylist::DoShuffleFolders( void )
{
	time_t tm;
	int total = m_FolderList.Count();
	
	tm = time( NULL );
	srand( (int)tm );
	int index = rand() % total;
	int i, c=index;
	bool find = true;
	if( m_FolderList[index]->m_state != NOT_PLAYED ) {
		for( i=1; i<total; i++ ) {
			c++;
			if( c == total ) c = 0;
			//printf( "m_FolderList[%d]->m_state: %d\n", c, m_FolderList[c]->m_state );
			if( m_FolderList[c]->m_state == NOT_PLAYED ) break;
		}
		if( i == total ) find = false;
	}

	if( find ) index = c;
	else index = -1;

	return index;
}

char* CPlaylist::GetNext( void )
{
	char* path = NULL;

	printf( "GetNext: m_boundary(%d)\n", m_boundary );
	m_Playlist[m_playindex]->m_state = PLAYDONE;
	switch( m_boundary ) {
	case BOUNDARY_ALL:
		path = BoundaryAll( 1 );
		break;
	case BOUNDARY_CATEGORY:
		path = BoundaryCategory( 1, m_repeat );
		break;
	case BOUNDARY_ONE:
		path = BoundaryOne();
		break;
	}
	m_Playlist[m_playindex]->m_state = PLAYING;

	return path;
}


char* CPlaylist::GetPrev( void )
{
	char* path = NULL;

	m_Playlist[m_playindex]->m_state = PLAYDONE;
	switch( m_boundary ) {
	case BOUNDARY_ALL:
		path = BoundaryAll( 0 );
		break;
	case BOUNDARY_CATEGORY:
		path = BoundaryCategory( 0, m_repeat );
		break;
	case BOUNDARY_ONE:
		path = BoundaryOne();
		break;
	}

	return path;
}

// dir: 0 prev, 1 next
int CPlaylist::MoveFolder( int dir )
{
	int index;

	printf( "+MoveFolder(%d), m_dirindex: %d\n", dir, m_dirindex );
	m_pCur->m_state = PLAYDONE;
	if( m_shuffle ) {
		index = DoShuffleFolders();
		if( index == -1 ) {
			if( m_repeat ) {
				ClearFolderlistState();
				index = DoShuffleFolders();
			}
		}
	}
	else {
		index = m_dirindex;
		if( dir ) { // next
			index++;
			if( index >= m_FolderList.Count() ) {
				if( m_repeat ) {
					index = 0;
					ClearFolderlistState();
				}
				else index = -1;
			}
		}
		else { // prev
			index--;
			if( index < 0 ) {
				if( m_repeat ) {
					index = m_FolderList.Count()-1;
					ClearFolderlistState();
				}
				else index = -1;
			}
		}
	}

	if( index != -1 ) {
		m_pCur->playlist = NULL;
		m_dirindex = index;
		m_pCur = m_FolderList[m_dirindex];
	}

	printf( "-MoveFolder-> %d, %d\n", index, m_dirindex );
	
	return index;
}

void CPlaylist::ClearPlaylistState( void )
{
	for( int i=0; i<m_Playlist.Count(); i++ ) {
		m_Playlist[i]->m_state = NOT_PLAYED;
	}
}

void CPlaylist::ClearFolderlistState( void )
{
	for( int i=0; i<m_FolderList.Count(); i++ ) {
		m_FolderList[i]->m_state = NOT_PLAYED;
	}
}

void CPlaylist::SortPlaylist( void )
{
	switch( m_sort ) {
	case NAME_ASCENT:
		m_Playlist.Sort( _p_name_compare_asc );
		break;
	case NAME_DESCENT:
		m_Playlist.Sort( _p_name_compare_desc );
		break;
	case DATE_ASCENT:
		m_Playlist.Sort( _p_date_compare_asc );
		break;
	case DATE_DESCENT:
		m_Playlist.Sort( _p_date_compare_desc );
		break;
	}
}

void CPlaylist::SortFolderlist( void )
{
	switch( m_sort ) {
	case NAME_ASCENT:
		m_FolderList.Sort( _f_name_compare_asc );
		break;
	case NAME_DESCENT:
		m_FolderList.Sort( _f_name_compare_desc );
		break;
	case DATE_ASCENT:
		m_FolderList.Sort( _f_date_compare_asc );
		break;
	case DATE_DESCENT:
		m_FolderList.Sort( _f_date_compare_desc );
		break;
	}
}

void CPlaylist::Sort( void )
{
	SortFolderlist();
	SortPlaylist();
}

void CPlaylist::SetSort( int sort )
{
	if( m_sort == sort ) return;
	m_sort = sort;
	Sort();
}

uchar_t* CPlaylist::GetCurrentFile( void )
{
	MARK_PLAYING_NOW;
	return (uchar_t*)m_currentfile;
}

void CPlaylist::SetProgressPfn( progresspfn pfn )
{
	m_prg = pfn;
}

int CPlaylist::PlaylistCount( void ) const
{
  return m_Playlist.Count();
}

int CPlaylist::GetID( void ) const
{
  return m_id;
}

const uchar_t* CPlaylist::GetCurrentFolder( void )
{
  return ( const uchar_t* )m_pCur->path;
}

int CPlaylist::DeleteFolder( int i )
{
  if( i< 0 || i >= m_FolderList.Count() ) {
    return -1;
  }

  m_FolderList.Delete( i );

  return 0;
}