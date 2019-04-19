//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : ProgressBar class definition

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/ProgressBar.cpp,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: ProgressBar.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.12  2005/07/20 06:09:43  zstein
// - updated.
//
// Revision 1.11  2005/07/16 04:13:51  zstein
// - fixed bugs & added functions.
//
// Revision 1.10  2005/06/16 23:49:48  zstein
// - bug fixed
//
// Revision 1.9  2005/05/18 15:21:28  zstein
// - added some functions.
//
// Revision 1.8  2005/05/17 09:31:22  zstein
// - added slidebar routine.
//
// Revision 1.7  2005/05/12 04:18:32  zstein
// *** empty log message ***
//
// Revision 1.6  2005/05/03 02:14:13  zstein
// *** empty log message ***
//
// Revision 1.5  2005/04/27 04:53:21  zstein
// Support Multi-languages.
// Support resource text.
//
// Revision 1.4  2005/03/04 13:16:30  zstein
// BUG FIXED:
//
// Revision 1.3  2005/02/25 13:57:17  zstein
// Added WindowType
//
// Revision 1.2  2005/02/21 02:10:14  zstein
// BUG Fixed & Update
//
// Revision 1.1  2005/02/16 16:03:25  zstein
// *** empty log message ***
//

//*******************************************************************************

#include "ProgressBar.h"

#define W	17
#define F	27
#define B	24

static char szRightRound[6][2] = {
	{ F, B },
	{ F, F },
	{ F, F },
	{ F, F },
	{ F, F },
	{ F, B }
};
static char szLeftRound[6][2] = {
	{ B, F },
	{ F, F },
	{ F, F },
	{ F, F },
	{ F, F },
	{ B, F }	
};
static char szBall[8][8] = {
	{ B, W, W, W, W, W, W, B },
	{ W, W, B, B, B, B, W, W },
	{ W, B, B, B, B, B, B, W },
	{ W, B, B, B, B, B, B, W },
	{ W, B, B, B, B, B, B, W },
	{ W, B, B, B, B, B, B, W },
	{ W, W, B, B, B, B, W, W },
	{ B, W, W, W, W, W, W, B },
};
static char szSlideBarL[6][1] = {
	{ F },
	{ W },
	{ W },
	{ W },
	{ W },
	{ F }	
};

static char szSlideBarR[6][1] = {
	{ F },
	{ W },
	{ W },
	{ W },
	{ W },
	{ F }	
};


CProgressBar::CProgressBar( CWindow* parent, int start, int end ) :
	CWindow( parent ),
	m_nStart( start ),
	m_nEnd( end )
{
	m_nRange = end - start;
	m_nCurPos = 0;
	m_nPrevPos = 0;
	m_fValid = true;

	m_pRRound = new CPixmap;
	if( m_pRRound ) 
		m_pRRound->CreatePixmap( 2, 6, 8, ( char* )szRightRound, false );
	else m_fValid = false;
	m_pLRound = new CPixmap;
	if( m_pLRound )
		m_pLRound->CreatePixmap( 2, 6, 8, ( char* )szLeftRound, false );
	else m_fValid = false;

	m_pBall = new CPixmap;
	m_pBall->CreatePixmap( 8, 8, 8, (char*)szBall, false );

	m_pSlideBarL = new CPixmap;
	m_pSlideBarL->CreatePixmap( 1, 6, 8, (char*)szSlideBarL, false );

	m_pSlideBarR = new CPixmap;
	m_pSlideBarR->CreatePixmap( 1, 6, 8, (char*)szSlideBarR, false );

	m_nWindowType = WINDOW_TYPE_PROGRESSBAR;

	m_fShowBall = false;

	m_nMode = PROGRESS_BAR;
}

CProgressBar::~CProgressBar( void )
{
	if( m_pRRound ) delete m_pRRound;
	if( m_pLRound ) delete m_pLRound;
	if( m_pBall ) delete m_pBall;
	if( m_pSlideBarL ) delete m_pSlideBarL;
	if( m_pSlideBarR ) delete m_pSlideBarR;
}

void CProgressBar::OnPaint( const CRect& rc )
{
	if( m_fValid == false ) return;
	if( m_parent && m_parent->m_fVisible == false ) return;
	DrawBoundary();
	UpdateProgress();

	op_Flush();
}

void CProgressBar::UpdateProgress( void )
{
	if( m_fVisible == false ) return;
	if( m_parent && m_parent->m_fVisible == false ) return;

 	int pos;
	
	// clear inner box
	DrawBoundary();
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1-1, rcABS.GetWidth(), 1, B );
	g_SysPixmap.Fill( rcABS.x1, rcABS.y2+1, rcABS.GetWidth(), 1, B );
	g_SysPixmap.Fill( rcABS.x1+m_pLRound->m_nWidth, rcABS.y1, rcABS.GetWidth()-4, rcABS.GetHeight(), F );

	if( m_fShowBall )
		pos = (rcClient.GetWidth()-m_pBall->m_nWidth-2)*m_nCurPos/m_nRange;
	else 
		pos = (rcClient.GetWidth()-2)*m_nCurPos/m_nRange;
	if( m_nMode == SLIDE_BAR ) {
		pos = rcClient.GetWidth()*m_nCurPos/m_nRange;
		int nBarSize = rcABS.GetWidth() / m_nRange;
		if( m_nCurPos == 0 ) {
			szSlideBarL[0][0] = B;
			szSlideBarL[5][0] = B;
		}
		else {
			if( m_nCurPos == m_nRange ) {
				szSlideBarR[0][0] = B;
				szSlideBarR[5][0] = B;
			}
			else {
				szSlideBarL[0][0] = F;
				szSlideBarL[5][0] = F;
				szSlideBarR[0][0] = F;
				szSlideBarR[5][0] = F;
			}
		}
		if( nBarSize < 6 ) nBarSize = 6;
		g_SysPixmap.Fill( rcABS.x1+pos+1, rcABS.y1, 
			nBarSize-2, m_pSlideBarL->m_nHeight, W );
		op_BitBlt( &g_SysPixmap, rcABS.x1+pos, rcABS.y1, 
			m_pSlideBarL, 0, 0, m_pSlideBarL->m_nWidth, m_pSlideBarL->m_nHeight );
		op_BitBlt( &g_SysPixmap, rcABS.x1+pos+nBarSize-1, rcABS.y1, 
			m_pSlideBarR, 0, 0, m_pSlideBarL->m_nWidth, m_pSlideBarR->m_nHeight );
	}
	else {
		if( m_nCurPos > 0 ) g_SysPixmap.Fill( rcABS.x1, rcABS.y1+1, 1, 4, W );
		
		if( m_nCurPos == m_nRange && m_nCurPos ) {
			if( m_fShowBall == false && pos > 2 ) pos -= 1;
			g_SysPixmap.Fill( rcABS.x2-1, rcABS.y1+1, 1, 4, W );
		}
		g_SysPixmap.Fill( rcABS.x1+m_pLRound->m_nWidth-1, rcABS.y1, 
			pos, rcABS.GetHeight(), W );
	}

	if( m_fShowBall ) {
		//delete an afterimage
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1-1, rcABS.GetWidth(), 1, B );
		g_SysPixmap.Fill( rcABS.x1, rcABS.y2+1, rcABS.GetWidth(), 1, B );
		#if 0
		// draw point
		if( m_nCurPos >= (rcABS.GetWidth()-2) ) m_nCurPos = rcABS.GetWidth() - m_pBall->m_nWidth;
		op_BitBlt( &g_SysPixmap, rcABS.x1+m_nCurPos, rcABS.y1-1, 
			m_pBall, 0, 0, m_pBall->m_nWidth, m_pBall->m_nHeight );
		#else
		op_BitBlt( &g_SysPixmap, rcABS.x1+pos, rcABS.y1-1, 
			m_pBall, 0, 0, m_pBall->m_nWidth, m_pBall->m_nHeight );
		#endif
	}

	op_Flush();
}

void CProgressBar::DrawBoundary( void )
{
	if( m_fValid == false ) return;
	
	// draw left blank boundary
	op_BitBlt( &g_SysPixmap, rcABS.x1, rcABS.y1, 
		m_pLRound, 0, 0, m_pLRound->m_nWidth, m_pLRound->m_nHeight );
	// draw right blank boundary
	op_BitBlt( &g_SysPixmap, rcABS.x2-m_pRRound->m_nWidth, rcABS.y1,
		m_pRRound, 0, 0, m_pRRound->m_nWidth, m_pRRound->m_nHeight );
}

#if 0
void CProgressBar::SetPercent( int percent, bool fUpdate )
{
	if( percent < 0 ) return;
	if( percent > 100 ) return;

	m_nRange = rcClient.GetWidth()-2*(m_pLRound->m_nWidth-1);
	if( m_nMode == SLIDE_BAR ) m_nRange -= m_pSlideBarL->m_nWidth;
	
	m_nPrevPos = m_nCurPos;
	m_nPercent = percent;
	if( m_nPercent == 0 ) m_nCurPos = 0;
	else {
		if( m_fShowBall ) {
			m_nCurPos = (int)((float)(m_nRange-m_pBall->m_nWidth)*((float)m_nPercent/100));
		}
		else {
			m_nCurPos = (int)((float)m_nRange*((float)m_nPercent/100));
		}
	}
	if( m_nPercent == 100 ) m_nCurPos--;

	if( fUpdate ) UpdateProgress();
}

void CProgressBar::SetPosition( int nIndex, bool fUpdate )
{
	if( nIndex > m_nEnd ) return;
	int width = rcClient.GetWidth()-2; // subtract L/R boundary pixel
	if( m_nMode == SLIDE_BAR ) width -= m_pSlideBarL->m_nWidth;
	int nCount = m_nEnd - m_nStart;
	if( nCount ) m_nCurPos = nIndex*width/nCount;
	else m_nCurPos = width;

	if( fUpdate ) UpdateProgress();
}
#endif

void CProgressBar::ShowBall( bool fShow )
{
	m_fShowBall = fShow;
	if( m_fShowBall ) {
		m_nCurPos = (int)((float)(m_nRange-m_pBall->m_nWidth)*((float)m_nPercent/100));
	}
	else {
		m_nCurPos = (int)((float)m_nRange*((float)m_nPercent/100));
	}
	//OnPaint( rcClient );
}

void CProgressBar::SetRange( int min, int max )
{
	if( min >= max ) return;
	
	m_nRange = abs(max - min);
	m_nStart = min;
	m_nEnd = max;
}

void CProgressBar::SetPos( int pos )
{
	if( pos > m_nRange ) return;
	m_nCurPos = pos;
}

int CProgressBar::GetPos( void )
{
	return m_nCurPos;
}

int CProgressBar::GetRange( void )
{
	return m_nRange;
}
