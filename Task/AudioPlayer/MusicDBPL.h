#ifndef __MUSICDB_PLAYLIST_H__
#define __MUSICDB_PLAYLIST_H__

#include <Component/String.h>
#include "Playlist.h"
#include "MusicDB.h"

class CMusicDBPL : public CPlaylist
{
public:
	enum {
		ARTIST,
		ALBUM,
		GENRE,
		ALLTRACK
	};
protected:
	CDeviceMusicDB* m_handle;
	int m_type;
	bool m_same;
	
	int MakeFolderlist( void );
	void MakePlaypath( void );

	void MakeArtistlist( void );
	void MakeAlbumlist( void );
	void MakeGenrelist( void );
	void MakeAlltracklist( void );

	void MakeArtistPlaylist( void );
	void MakeGroupPlaylist( void );
	void MakeAllPlaylist( void );
public:
	CMusicDBPL( CDeviceMusicDB* handle, int type, int sort, progresspfn pfn=NULL );
	~CMusicDBPL( void );

	int MakePlaylist( void );
	int SetType( int type );
	bool SetCurrentDir( const CString& path );
	bool SetCurrentFile( const CString& filename );
};

#endif /* __MUSICDB_PLAYLIST_H__ */

