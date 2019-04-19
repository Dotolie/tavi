//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Progressbar class declaration

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/ProgressBar.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $

//

//******************************************************************************

// $Log: ProgressBar.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/07/20 06:09:43  zstein
// - updated.
//
// Revision 1.5  2005/07/16 04:13:51  zstein
// - fixed bugs & added functions.
//
// Revision 1.4  2005/05/17 09:31:22  zstein
// - added slidebar routine.
//
// Revision 1.3  2005/05/12 04:18:32  zstein
// *** empty log message ***
//
// Revision 1.2  2005/05/03 02:14:13  zstein
// *** empty log message ***
//
// Revision 1.1  2005/02/16 16:03:25  zstein
// *** empty log message ***
//

//*******************************************************************************

#ifndef __PROGRESS_BAR_H__
#define __PROGRESS_BAR_H__

#include "window.h"
#include "Pixmap.h"

class CProgressBar : public CWindow
{
private:
protected:
	int m_nStart;
	int m_nEnd;
	int m_nRange;
	int m_nPercent;
	int m_nCurPos;
	int m_nPrevPos;
	bool m_fValid;
	bool m_fShowBall;
	int m_nMode;
	CPixmap* m_pRRound;
	CPixmap* m_pLRound;
	CPixmap* m_pBall;
	CPixmap* m_pSlideBarL;
	CPixmap* m_pSlideBarR;

	void DrawBoundary( void );
	void OnPaint( const CRect& rc );
	virtual void UpdateProgress( void );
public:
	enum {
		PROGRESS_BAR,
		SLIDE_BAR
	};
	CProgressBar( CWindow* parent, int start=0, int end=0 );
	virtual ~CProgressBar( void );

	//void SetPercent( int percent, bool fUpdate=false );
	//void SetPosition( int nIndex, bool fUpdate=false );
	void SetRange( int nRange ) { m_nStart = 0;  m_nEnd=nRange-1; }
	void ShowBall( bool fShow );
	void SetMode( int nMode ) { m_nMode = nMode; }
	void SetPos( int pos );
	int GetPos( void );
	int GetRange( void );
	void SetRange( int min, int max );
};

#endif /*__PROGRESS_BAR_H__ */