#ifndef __PLAYLISTS_PLAYLIST_H__
#define __PLAYLISTS_PLAYLIST_H__

#include <Component/String.h>
#include "Playlist.h"

class CPlaylistPL : public CPlaylist
{
protected:
	CString m_base;
	
	int MakeFolderlist( void );
	void MakePlaypath( void );
public:
	CPlaylistPL( const CString& path, int sort, progresspfn pfn=NULL );
	~CPlaylistPL( void );

	int MakePlaylist( void );
};

#endif /* __PLAYLISTS_PLAYLIST_H__ */

