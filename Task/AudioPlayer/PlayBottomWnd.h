//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Music Player Bottom Window class declaration
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/PlayBottomWnd.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: PlayBottomWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.31  2005/11/10 01:09:39  zstein
// *** empty log message ***
//
// Revision 1.30  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
//******************************************************************************
#ifndef __BOTTOM_WINDOW_H__
#define __BOTTOM_WINDOW_H__

#include <Component/window.h>
#include <Component/ProgressBar.h>
#include <Component/Pixmap.h>
#include <Component/Label.h>
#include <Component/BottomWnd.h>
#include "VolProgressBar.h"
#include <Task/SndCtrl.h>
#include <Task/tavi_global.h>


extern CSndCtrl* g_SndCtrl;

class CPlayerBottomWnd : public CBottomWnd
{
private:
	CProgressBar* m_puzProgressBar;
	CVolProgressBar* m_pVolCtrl;
	//CPixmap* m_puzBlank;
	CPixmap* m_puzShuffle;
	CPixmap* m_puzUnShuffle;
	CPixmap* m_puzRepeat;
	CPixmap* m_puzUnRepeat;
	//CPixmap* m_puzRepeat1;
	CPixmap* m_puzBoundary[3];
	CPixmap* m_puzA;
	CPixmap* m_puzAB;
	CPixmap* m_puzEQ[EQ_ICON_COUNT];
	CPixmap* m_pmPlayState[4];
	int m_nAB;
	int m_nRepeat;
	int m_nShuffle;
	int m_nTouchMenu;
	CWindow* m_pTouchWnd;
	int m_nTotalTime;
	int m_nCurrentTime;
	int m_nTouchSec;
	int m_nPlayMode;
	int m_seekdir;
protected:
	void OnPaint( const CRect& rc );
	void OnMouseEvent( const MouseEvent& evt );
	void OnKeypressed( const KeyEvent& evt );
	void OnKeyReleased( const KeyEvent& evt );
	void OnWindowEvent( const WindowEvent& evt );
	void DoSeek( void );
public:
	bool m_fExtLCDEQMode;
	bool m_fExtLCDVolMode;
	bool m_fSeek;
	struct timeval m_tvExtLCDEQ;
	CPlayerBottomWnd( CWindow* parent, int nAB, int nRepeat, int nShuffle );
	~CPlayerBottomWnd();

	enum {
		PBW_EQ_NORMAL,
		PBW_EQ_ROCK,
		PBW_EQ_POP,
		PBW_EQ_JAZZ,
		PBW_EQ_CLASSIC,
		PBW_EQ_DANCE,
		PBW_EQ_HEAVY,
		PBW_EQ_DISCO,
		PBW_EQ_SOFT,
		PBW_EQ_USER,
		PBW_EQ_SRS,
		PBW_EQ_TRUEBASS,
		PBW_EQ_WOWPRESET,
		PBW_EQ_WOWUSER
	};
	enum {
		NORMAL,
		TOOL,
		SEEK,
		VOLUME
	};
	enum {
		PBW_PLAY,
		PBW_PAUSE,
		PBW_FF,
		PBW_REW
	};
	void SetMode( int mode ) { m_nMode = mode; }
	void UpdateEQ( bool fFlush );
	void UpdateShuffle( bool fFlush );
	void UpdateRepeat( bool fFlush );
	void UpdateAB( int mode, bool fFlush );
	void UpdateBoundary( bool fFlush );
	void UpdateCurrentTime( int sec, bool fUpdate );
	void UpdateTotalTime( int sec, bool fUpdate );
	bool IsTouchMenu( void );
	void ChangeTouchMenu( int menu, bool fUpdate=true );
	int GetVolumeRange( void ) { return m_pVolCtrl->GetVolumeRange(); }
	int GetVolume( void ) { return m_pVolCtrl->GetVolume(); }
	void SetVolume( int level );
	int GetCurrentTime( void ) { return m_nCurrentTime; }
	int GetTotalTime( void ) {return m_nTotalTime; };
	int GetCurrentTouchMenu( void ) { return m_nTouchMenu; }
	void UpdatePlayState( int state, bool fUpdate );
	bool IsSeeking( void );
	void ResetTime( void ) { m_nCurrentTime=0, m_nTotalTime=0; }
	//void Seek10Sec( int dir );
	void SetSeekMode( void );
	void ResetProgressBar( void );
	void Hide( void );
	void CancelSeek( void );
};

#endif /* __BOTTOM_WINDOW_H__ */

