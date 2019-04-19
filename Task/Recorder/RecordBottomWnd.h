//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Recorder Bottom Window
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecordBottomWnd.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: RecordBottomWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/11/22 09:07:10  zstein
// *** empty log message ***
//
//*******************************************************************************

#ifndef __RECORD_BOTTOM_WINDOW_H__
#define __RECORD_BOTTOM_WINDOW_H__

#include <Component/BottomWnd.h>
#include <Component/ProgressBar.h>
#include <Component/Label.h>
#include "RecVolCtrl.h"

class CRecordBottomWnd : public CBottomWnd
{
private:
protected:
	CPixmap* m_pmRecord;
	CPixmap* m_pmPause;
	CPixmap* m_pmStatus;
	CPixmap* m_pmAGCOn;
	CPixmap* m_pmAGCOff;
	CPixmap* m_pmBitrate[2];
	CProgressBar* m_pProgressBar;
	int m_nCurrentTime;
	int m_nAvailTime;
	
	void OnKeypressed( const KeyEvent& evt );
	void OnPaint( const CRect& rc );
	void OnWindowEvent( const WindowEvent& evt );
public:
	CRecordBottomWnd( CWindow* parent, int nCurrentTime, int nAvailTime );
	~CRecordBottomWnd( void );

	void SetStatus( int nRecord );

	void UpdateCurrentTime( int sec, bool fUpdate );
	void UpdateTotalTime( int sec, bool fUpdate );
	void UpdateBitrate( int bitrate );
	int GetTotalTime( void ) const { return m_nAvailTime; }
};

#endif /* __RECORD_BOTTOM_WINDOW_H__ */
