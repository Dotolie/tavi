#ifndef __FOLDER_PLAYLIST_H__
#define __FOLDER_PLAYLIST_H__

#include <Component/String.h>
#include "Playlist.h"

class CFolderPL : public CPlaylist
{
protected:
	int MakeFolderlist( void );
    int MoveFolder( int dir );
	void MakePlaypath( void );
    int AddFolders( const CString& path, bool broot, bool bchecksame );
	FolderNode* m_pnode;
public:
	CFolderPL( const CString& base, int sort, progresspfn pfn=NULL );
	~CFolderPL( void );

	int MakePlaylist( void );
	bool SetCurrentDir( const CString& path );
};

#endif /* __FOLDER_PLAYLIST_H__ */
