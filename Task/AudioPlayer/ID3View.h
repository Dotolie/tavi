//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : ID3View class declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/ID3View.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ID3View.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.15  2005/12/01 01:04:08  zstein
// *** empty log message ***
//
// Revision 1.14  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
// Revision 1.13  2005/09/20 12:22:45  zstein
// - update
//
//*******************************************************************************
#ifndef __ID3VIEW_H__
#define __ID3VIEW_H__

#include <Task/Msg.h>
#include <Component/ListView.h>
#include <Component/Stack.h>
#include <Component/ContextMenu.h>
#include <Lib/LibMsgBox.h>
#include "AudioPlayer.h"
#include "ID3/m3u.h"
#include "MusicDB.h"

typedef struct {
	int m_nStartIdx;
	int m_nSelectedIdx;
} StackItem;

class CID3ListItem : public CListItem 
{
public:
	CID3ListItem( int nIdx, int nIcon1, int nIcon2, char* sz, bool unicode, uchar_t* path=0 ) :
		CListItem( nIdx, nIcon1, nIcon2, sz, true )
	{
		m_szPath = path;
		m_nResID = -1;
		return;
	}
	uchar_t* m_szPath;
};

class CID3ListView : public CListView
{
private:
	void Init( void );
	bool MenuUp( void );
	void MenuDown( void );
	int ScanDB();
	int ScanFile( CListItem* puzItem, int bucket );
	int GetIconIndex( int nType );
	void DBtoPlaylist( M3UHandle handle );
	void TableToPlaylist( M3UHandle handle, const char* cmp1, const char* cmp2 );
protected:
	int m_nType;
	int m_nDepth;
	CString m_szS1; // search string
	CString m_szS2; // search string
	CMsgBox* m_pMsg;
	CDeviceMusicDB* m_pDB;
	
	struct timeval m_tv;

	void OnKeypressed( const KeyEvent& evt );
	void OnPaint( const CRect& rc );
	void OnWindowEvent( const WindowEvent& evt );
	void OnMouseEvent( const MouseEvent& evt );
	void CreateContextMenu( void );

	int ScanArtist( void );
	int ScanAllArtist( void );
	int ScanAlbumFromArtist( void);
	int ScanMusicFromAA( void );
	int ScanAlbum( void );
	int ScanAllAlbum( void );
	int ScanMusicFromAlbum( void );
	int ScanGenre( void );
	int ScanAllGenre( void );
	int ScanMusicFromGenre( void );
	int ScanAllSongs( void );
	int ScanMusicFromGroup(GROUP_TYPE i_type);
public:
	CStack< StackItem > m_uzStack;
	CStack< CString > m_uzTitleStack;
	CContextMenu* m_pCtxMenuWnd;
	enum {
		FILTER_ARTIST = 0x01,
		FILTER_ALBUM = 0x02,
		FILTER_GENRE = 0x04
	};
	enum {
		ID3LIST_ARTIST,
		ID3LIST_ALBUM,
		ID3LIST_GENRE,
		ID3LIST_ALLTRACKS
	};
	enum {
		EXIT_NORMAL,
		EXIT_RETRY
	};
	CID3ListView( CWindow* parent, int type, CDeviceMusicDB* pMusicDB );
	~CID3ListView( void );

	static int AddToPlaylist( RMuint8* arg );
	void Show( void );
};

#endif /* __ID3VIEW_H__ */

