//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Music Main Window
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/MusicMainWnd.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: MusicMainWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.10  2006/01/19 08:43:38  zstein
// *** empty log message ***
//
// Revision 1.9  2005/12/16 09:57:27  zstein
// *** empty log message ***
//
// Revision 1.8  2005/12/01 01:04:08  zstein
// *** empty log message ***
//
// Revision 1.7  2005/11/25 10:21:58  zstein
// *** empty log message ***
//
// Revision 1.6  2005/11/10 01:09:39  zstein
// *** empty log message ***
//
// Revision 1.5  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
// Revision 1.4  2005/10/31 13:51:44  zstein
// *** empty log message ***
//
// Revision 1.3  2005/09/12 03:09:40  zstein
// - updated DB
//
// Revision 1.2  2005/09/09 05:52:48  zstein
// - updated IDDB
//
// Revision 1.1  2005/07/16 05:17:06  zstein
// - created.
//
//******************************************************************************

#ifndef __MUSIC_MAIN_WINDOW_H__
#define __MUSIC_MAIN_WINDOW_H__

#include <Component/ListView.h>
#include "AudioPlayer.h"

class CMusicMainWnd : public CListView
{
protected:
	void OnKeypressed( const KeyEvent& evt );
	void OnWindowEvent( const WindowEvent& evt );
	void DoSubMenu( void );
	void OnPaint( const CRect& rc );
	void Show( void );

	CDeviceMusicDB* m_pMusicDB;
	CWindow* m_pwnd;

public:
	CMusicMainWnd( CWindow* parent, CDeviceMusicDB* pMusicDB );
	~CMusicMainWnd( void );

	CWindow* GetListView( void ) { return m_pwnd; }
};

#endif /* __MUSIC_MAIN_WINDOW_H__ */