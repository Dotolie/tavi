//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Label class definition
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/Label.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:20:34 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: Label.cpp,v $
// Revision 1.2  2006/06/07 08:20:34  zstein
// - added UI function.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.30  2006/02/07 09:19:16  zstein
// - fixed bugs.
//
// Revision 1.29  2006/01/03 09:36:44  etnlwind
// Update for Outline Output on Scroll Text
//
// Revision 1.28  2005/12/16 12:23:01  yong
// add SetScrollStep() method
//
// Revision 1.27  2005/12/16 06:01:32  zstein
// *** empty log message ***
//
// Revision 1.26  2005/12/16 04:29:30  zstein
// *** empty log message ***
//
// Revision 1.25  2005/11/08 07:28:18  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <string.h>
#include "Label.h"
#include "fbdev.h"
#include "../Lib/LibImage.h"
#include <Task/tavi_global.h>
#include "CharSet.h"

extern CTextEngine_BDF* pT;

CLabel::CLabel( CWindow* parent, int nFGColor, int nBGColor, const char* szText ) :
	CWindow( parent ),
	m_nTextColor( nFGColor ),
	m_szText( szText )
{
	m_nBackgroundColor = nBGColor;
	m_nTextColor = nFGColor;
	m_nJustify = CLABEL_LEFT_JUSTIFY;
	m_nResID = -1;
	m_nFontOption = DT_TRANSPARENCY | DT_OUTLINE;
	m_bScrollable = false;
	m_pmText = NULL;
	m_nScrollStep = 8;
}

CLabel::CLabel( CWindow* parent, int nFGColor, int nBGColor, int nResID ) :
	CWindow( parent ),
	m_nTextColor( nFGColor ),
	m_nResID( nResID )
{
	m_nBackgroundColor = nBGColor;
	m_nTextColor = _WHITE;
	m_nJustify = CLABEL_LEFT_JUSTIFY;
	m_nFontOption = DT_TRANSPARENCY | DT_OUTLINE;
	m_nScrollStep = 8;
}


CLabel::~CLabel( void )
{
	if( m_pmText ) delete m_pmText;
}

void CLabel::OnPaint( const CRect& rc )
{
	int nFontIdx;
	uchar_t wsz[256];
	int nOption;
	int fgcolor;
    int offset;

	if( m_fVisible == false ) return;
    
	offset = rcABS.GetHeight()/2 - m_nFontHeight/2;
    
	if( m_fFocus ) {
		fgcolor = _BLACK;
		nOption = DT_TRANSPARENCY;
	}
	else {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nBackgroundColor );
		nOption = m_nFontOption;
		fgcolor = m_nTextColor;
		if( m_disable ) {
		  fgcolor = _GRAY;
		}
	}
	

	//if( m_szText.GetSize() == 0 ) return;
	int len, blen, w;
	uchar_t* pwsz;
	if( m_nResID == -1 ) {
		if( m_szText.IsUnicode() ) {
			len = m_szText.GetSize();
			pwsz = ( uchar_t* )m_szText;
		}
		else {
			len = ConvertUnicode( 
				(char*)m_szText, m_szText.GetSize(), 
				wsz, sizeof(wsz), TaviSysParam.etc.language );
			pwsz = ( uchar_t* )wsz;
		}
	}
	else {
		pwsz = pT->GetTextRes( m_nResID );
		len = ustrlen( pwsz );
	}
	blen = pT->GetBoundaryLength( (uchar_t*)pwsz, len, m_nFontHeight, 0, rcABS.GetWidth() );
	m_pixelwidth = pT->GetStringWidth( pwsz, m_nFontHeight, nOption );

	int x;
	switch( m_nJustify ) {
	case CLABEL_LEFT_JUSTIFY:
		x = rcABS.x1+1;
		break;
	case CLABEL_RIGHT_JUSTIFY:
		x = rcABS.x1 + rcABS.GetWidth() - m_pixelwidth;
		break;
	case CLABEL_CENTER_JUSTIFY:
		x = rcABS.x1 + (rcABS.GetWidth()-m_pixelwidth)/2;
		break;
	}

	int offset_h = (rcABS.GetHeight()-m_nFontHeight)/2+1;
	if( m_bScrollable ) {
		for( int i=0; i<blen; i++ ) {
			x += pT->DrawCharacter( pwsz[i], 
				x, rcABS.y1+offset_h, fgcolor, m_nFontHeight, 
				nOption);
		}
		delete m_pmText;
		m_pmText = NULL;
		if( len > blen ) {
			x = 0;
			m_pmText = new CPixmap;
			m_pmText->CreatePixmap( m_pixelwidth, m_nFontHeight, 8, NULL, true );
			m_pmText->Fill( 0, 0, m_pmText->m_nWidth, m_pmText->m_nHeight, m_nBackgroundColor );
			for( int i=0; i<len; i++ ) {
				x += (pT->DrawCharacter( pwsz[i], x, 0, fgcolor, m_nFontHeight, nOption, m_pmText )+1);
			}
			m_nextscrpos = 0; // start position of m_pmText 
			CSize size;
			size = pT->GetStringSize( pwsz, blen, m_nFontHeight );
			m_pixelscroll = size.width;
			gettimeofday( &m_stv, NULL );
			m_wait = true;
		}
	}
	else {
		int dotdot = 0;
		int i;
		
		if( len > blen ) {
			dotdot = 2;
			blen -= dotdot;
		}

		for( i=0; i<blen; i++ ) {
			x += pT->DrawCharacter( pwsz[i], 
				x, rcABS.y1+offset_h, fgcolor, m_nFontHeight, 
				nOption);
		}
		wsz[0] = '.';
		for( i=0; i<dotdot; i++ ) {
			x += pT->DrawCharacter( wsz[0], 
				x, rcABS.y1+offset_h, fgcolor, m_nFontHeight, 
				nOption);
		}
	}
}

void CLabel::SetText( const char* szText, bool fUpdate )
{
	m_szText = (char*)szText;
	if( fUpdate ) OnPaint( rcClient );
}

void CLabel::SetText( const CString& szText, bool fUpdate )
{
	m_szText = szText;
	if( fUpdate ) OnPaint( rcClient );
}

void CLabel::OnFocus( bool fFocus ) 
{
	m_fFocus = fFocus;
	//m_nTextColor = _BLACK;
	OnPaint( rcClient );
}

void CLabel::SetJustify( int nJustify )
{
	m_nJustify = nJustify;
}

void CLabel::SetTextResID( int nResID, bool fUpdate )
{
	m_nResID = nResID;
	if( fUpdate ) OnPaint( rcClient );
}

void CLabel::ScrollText( void )
{
	if( !m_bScrollable ) return;
	if( !m_pmText ) return;

	if( m_wait ) {
		gettimeofday( &m_etv, NULL );
		if( abs(m_etv.tv_sec-m_stv.tv_sec) > 2 ) m_wait = false;
		return;
	}
	m_nextscrpos += m_nScrollStep;
	
	int ww = m_pixelscroll;
	int w = m_pixelwidth - m_nextscrpos;
	int x = rcABS.x1 + 1; 
	int y = rcABS.y1 + (rcABS.GetHeight()-m_nFontHeight)/2+1;

	if( m_pixelwidth < ww ) return;

	if( w < (ww/2) ) { // end of scroll text
		m_nextscrpos = 0;
		w = ww;
		m_wait = true;
		gettimeofday( &m_stv, NULL );
	}
	else {
		if( w > ww ) w = ww;
	}
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nBackgroundColor );
	op_BitBlt( &g_SysPixmap, x, y, m_pmText, m_nextscrpos, 0, w, m_nFontHeight );
	op_Flush();
}

void CLabel::SetScrollStep(int step)
{
	m_nScrollStep = step;
}
