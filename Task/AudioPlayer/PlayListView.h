//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : PlayListView class declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/PlayListView.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/08 06:49:12 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: PlayListView.h,v $
// Revision 1.2  2006/06/08 06:49:12  zstein
// - skip invalid playlist file.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.12  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
// Revision 1.11  2005/10/17 01:26:47  zstein
// - updated UI.
//*******************************************************************************
#ifndef __PLAYLISTVIEW_H__
#define __PLAYLISTVIEW_H__

#include <tavi_lib.h>
#include <Component/ListView.h>
#include <Component/String.h>
#include <Component/ContextMenu.h>

class CPlaylistView : public CListView
{
friend int DeletePlaylist( RMuint8* arg );
friend int DeleteFile( CPlaylistView* pview );
friend int DeleteFolder( CPlaylistView* pview );
friend int MovePlaylist( RMuint8* arg );
private:
	void Init( void );
protected:
	CString m_base;
	CString m_playlistname;
	int m_nDepth;
	void OnKeypressed( const KeyEvent& evt );
	void OnWindowEvent( const WindowEvent& evt );
	void OnPaint( const CRect& rc );
	void Flickering( bool fShow );
	bool ScanPlaylist( void );
	void Show( void );
	void CreateContextMenu( void );
	CString m_szPlaylistname;
	struct timeval m_tv;
	CMsgBox* m_pMsg;
	bool m_instance;
	 CContextMenu* m_pCtxMenuWnd;
public:
	CPlaylistView( CWindow* parent );
	~CPlaylistView( void );

	void AddToPlaylist( void );
};

#endif /* __PLAYLISTVIEW_H__ */

