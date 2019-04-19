#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#include "rmbasic.h"
#include "fatfs.h"

#include "Component/Stack.h"
#include "Component/String.h"
#include "Component/FileUtil.h"
#include "Task/tavi_global.h"
#include "FolderPL.h"

CFolderPL::CFolderPL( const CString& path, int sort, progresspfn pfn ) :
	CPlaylist( path, sort, pfn )
{
  m_id = CPlaylist::FOLDER;
	MakeFolderlist();
	//m_pnode = NULL;
}

CFolderPL::~CFolderPL( void )
{
	//if( m_pnode ) delete m_pnode;
}


int CFolderPL::MakeFolderlist( void )
{
	m_FolderList.FreeAll();

    AddFolders( m_root, true, false );
	
	return m_FolderList.Count();
}


int CFolderPL::MakePlaylist( void )
{
	int media;
	RMuint32 fd;
	FATFS_ERROR err;
	FATFS_DIR_ENTRY_INFO info;
	PLNode* pnode;

	if( !m_pCur ) return -1;

	m_Playlist.FreeAll();

	err = fatfs_wdopen( (uchar_t*)m_pCur->path, &fd );
	if( err != FATFS_ERROR_NO_ERROR ) return -1;
	while( (err=fatfs_dread(fd, &info)) == FATFS_ERROR_NO_ERROR ) {
		if( info.attributes&ATTR_DIRECTORY ) continue;
		
		media = CheckMedia(info.name, true);
		switch( media ) {
		case MEDIA_MP3:
		case MEDIA_OGG:
		case MEDIA_WMA:
		case MEDIA_DCF_MP3:
			break;
		default:
			continue;
		}
		
		pnode = new PLNode;
		pnode->m_name.SetString( info.name, true );
		pnode->m_date = info.file_write_date<<16|info.file_write_time;
		pnode->m_state = NOT_PLAYED;
		m_Playlist.PushBack( pnode );
	}
	fatfs_dclose( fd );
	SortPlaylist();

	#if 0
	for( int i=0; i<m_Playlist.Count(); i++ ) {
		printf( "PLAYLIST[%03d]: ", i );
		m_Playlist[i]->m_name.Print();
		printf( "\n" );
	}
	#endif

	m_pCur->playlist = &m_Playlist;
	
	return m_Playlist.Count();
}


void CFolderPL::MakePlaypath( void )
{
	m_currentfile = m_pCur->path;
	m_currentfile.Add( (uchar_t*)"/\0" );
	m_currentfile.Add( m_Playlist[m_playindex]->m_name );
}

bool CFolderPL::SetCurrentDir( const CString& path )
{
    bool ret;
    int cnt=0;
    
    ret = CPlaylist::SetCurrentDir(path);

    if( ret == false ) {
        cnt = AddFolders( path, false, true );
    }

    if( cnt >= 0 ) {
        ret = CPlaylist::SetCurrentDir( path );
    }

    return ret;
}



/* ++

    AddFolders( const CString& path )

Description:
    AddFolders adds path and subfolders of path to m_FolderList.

Arguments:
    path is a directory full path.
    root describes an argument 'path' is root directory or not.
    If bchecksame is set AddFolders function compares subfolder path with a path
    in the folder list.

Return:
    Returns the number of subfolder. -1 on error.

-- */
int CFolderPL::AddFolders( const CString& path, bool root, bool bchecksame )
{
    uchar_t* szCurPath = new uchar_t[MAX_PATH];
	FATFS_DIR_ENTRY_INFO entry;
	FATFS_ERROR err;
	RMuint32 fd;
	char delimeter[4] = { '/', 0, 0, 0 };
	CString szPath;
	FolderNode* pnode, *parent;
    bool bHasSame;
    int subfolders=0, match;

    bHasSame = false;
    parent = NULL;
    if( bchecksame ) {
        for( int i=0; i<m_FolderList.Count() && !bHasSame; i++ ) {
            if( path == m_FolderList[i]->path ) {
                bHasSame = true;
                match = i;
            }
        }
    }

    if( bHasSame == false ) {
    	pnode = new FolderNode;
    	pnode->path.SetString( (const char*)path, true );
    	pnode->date = 0;
    	pnode->playlist = 0;
        if( root ) {
            pnode->m_parent = 0;
        }
        else {
            pnode->m_parent = -1;
        }
        pnode->m_subfolders = -1;
        parent = pnode;
    	m_FolderList.PushBack( pnode );
    }
    else {
        parent = m_FolderList[match];
    }

    const uchar_t* ppath = ( const uchar_t* )path;
	err = fatfs_wdopen( (uchar_t*)ppath, (RMuint32*)&fd );
	if( err != FATFS_ERROR_NO_ERROR ) {
		printf( "<%s:%d> err: 0x%x\n", __FILE__, __LINE__, err );
        return -1;
	}
	while( 1 ) {
		err = fatfs_dread( fd, &entry );
		if( err != FATFS_ERROR_NO_ERROR ) break;
		if( entry.name[0] == '.' && entry.name[1] == 0 && 
		     entry.name[2] == 0 && entry.name[3] == 0 ) continue;
		if( entry.name[0] == '.' && entry.name[1] == 0 &&
			entry.name[2] == '.' && entry.name[3] == 0 ) continue;
		
		if( entry.attributes&ATTR_DIRECTORY ) {
			ustrcpy( szCurPath, (uchar_t*)ppath );
			ustrcat( szCurPath, (uchar_t*)delimeter );
			ustrcat( szCurPath, (uchar_t*)entry.name );

            if( bchecksame ) {
                int i;
                bHasSame = false;
                for( i=0; i<m_FolderList.Count() && !bHasSame; i++ ) {
                    if( !ustrcmp((uchar_t*)m_FolderList[i]->path, szCurPath ) ) {
                        bHasSame = true;
                    }
                }

                if( bHasSame ) continue;
            }
            
			pnode = new FolderNode;
			pnode->path.SetString( (char*)szCurPath, true );
			pnode->date = entry.file_write_date<<16|entry.file_write_time;
			pnode->playlist = 0;
			pnode->m_state = NOT_PLAYED;
            pnode->m_parent = 1;
            pnode->m_subfolders = -1;
			m_FolderList.PushBack( pnode );

            // limitation
            if( m_FolderList.Count() == MAX_FOLDERS ) {
                break;
            }
			if( m_prg ) m_prg( pnode->path, m_FolderList.Count() );
            subfolders++;
		}
	}
	fatfs_dclose( fd );

    parent->m_subfolders = subfolders;

	SortFolderlist();

	delete[] szCurPath;

	// test
	#if 0
    printf( "+++#################+++\n" );
	for( int i=0; i<m_FolderList.Count(); i++ ) {
		printf( "FOLDER: " );
		m_FolderList[i]->path.Print();
		printf( "\n" );
	}
    printf( "---#################---\n" );
	#endif
	
	return subfolders;
}


int CFolderPL::MoveFolder( int dir )
{
    FolderNode* pCurrent;

    pCurrent = m_FolderList[m_dirindex];

    // Scan subfolders
    if( pCurrent->m_subfolders == -1 ) {
        pCurrent->m_subfolders = AddFolders( pCurrent->path, false, true );
    }

    // Scan parent folder
    if( pCurrent->m_parent == -1 ) {
        CString szParent = pCurrent->path;
        int cnt = 0;
        szParent.Shear( '/', CString::TAIL );
        if( szParent != m_root ) {
            cnt = AddFolders( szParent, false, true );
        }
        pCurrent->m_parent = 1;

        // rearrange index
        int i = m_dirindex - cnt;
        for( ; i<m_FolderList.Count(); i++ ) {
            if( pCurrent->path == m_FolderList[i]->path ) {
                m_dirindex = i;
                break;
            }
        }
    }

    return CPlaylist::MoveFolder(dir);
}
