//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Slide Bar class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/SlideBar.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:25:47 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: SlideBar.cpp,v $
// Revision 1.2  2006/06/07 08:25:47  zstein
// - added an disable to select function.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.10  2005/10/17 01:25:34  zstein
// - updated UI.
//
// Revision 1.9  2005/08/16 06:50:39  etnlwind
// Update for Loading Animation
//
// Revision 1.8  2005/05/17 10:43:28  ywkim
// Change SetValue() for preset
//
// Revision 1.7  2005/05/14 14:16:59  ywkim
// delete DT_OUTILE for value
//
// Revision 1.6  2005/05/09 14:57:34  ywkim
// Change attribute of text with DT_BOLD
//
// Revision 1.5  2005/05/03 13:10:27  ywkim
// Bug fix on SetValue()
//
// Revision 1.4  2005/05/02 07:47:21  zstein
// BUG FIXED:  SetPreset( int nIndex )
//
// Revision 1.3  2005/04/28 06:53:54  ywkim
// Change color of slider
//
// Revision 1.2  2005/04/27 05:37:41  ywkim
// Modify show & onPaint for m-language
//
// Revision 1.1  2005/03/29 03:33:57  ywkim
// Add slidebar class
//
//*****************************************************************************


#include "SlideBar.h"
#include "fbdev.h"
#include "../Lib/LibImage.h"

// Macros
#define COLOR_FOCUSED			27
#define BACKGROUND_COLOR		22
#define FORGROUND_COLOR		23
#define B						28
#define T						0
#define F						23
#define E						21
#define W						17
// Extern variables
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern MpegDecoder* pMpegDecoder;
extern CTextEngine_BDF* pT;

// Local variables
static char szSlideBar[10][5] = {
	{ T, T, T, T, T },
	{ T, T, T, T, T },
	{ T, T, T, T, T },
	{ T, T, T, T, T },
	{ F, F, F, F, F },
	{ F, F, F, F, F },
	{ T, T, T, T, T },
	{ T, T, T, T, T },
	{ T, T, T, T, T },
	{ T, T, T, T, T },
};

static char szSlider[10][5] = {
	{ T, F, F, F, T},
	{ F, E, W, E, F },
	{ F, W, W, W, F },
	{ F, W, W, W, F },
	{ F, W, W, W, F },
	{ F, W, W, W, F },	
	{ F, W, W, W, F },
	{ F, W, W, W, F },
	{ F, E, W, E, F },
	{ T, F, F, F, T},

};

CSlideBar::CSlideBar( CWindow* parent )
{
	int i=0;

	m_nCurrentValue = 50;
	m_nMaxOfValue = 100;
	m_nMinOfValue = 0;
	m_parent = parent;
	m_puzSlideBar = new CPixmap;
	m_puzSlideBar->CreatePixmap(5, 10,	8, (char*)szSlideBar, false );
	m_puzSlider = new CPixmap;
	m_puzSlider->CreatePixmap(5, 10, 8, (char*)szSlider, false );

	m_nWindowType = WINDOW_TYPE_SLIDEBAR;
	m_nFocusedColor = 28;
	m_isPresetType = FALSE;
}

CSlideBar::~CSlideBar( void )
{
	if( m_puzSlideBar ) delete m_puzSlideBar;
	if( m_puzSlider ) delete m_puzSlider;	
}

void CSlideBar::OnFocus( bool fFocus )
{
	m_fFocus = fFocus;
	OnPaint( rcClient );
}

void CSlideBar::Resize( const CRect& rc, bool fUpdate )
{
	if( rc.GetWidth() < rcClient.GetWidth() ||
	     rc.GetHeight() < rcClient.GetHeight() ) return;
	CWindow::Resize( rc, fUpdate );
}

void CSlideBar::OnPaint( const CRect& rc )
{
	CString* text = m_uzList[0];
	int fontidx;
	int hoffset;
	int nPercent;
	int	xoffset;
	int	nFontColor;
	char szTemp[5];
	uchar_t ucText[5];

	hoffset = rcABS.GetHeight()/2-m_puzSlideBar->m_nHeight/2;
	xoffset = 6;
	if( m_fFocus ) {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nFocusedColor );
		for ( int i=0;i< rcABS.GetWidth()*0.7;i+=5)
		{
			pMpegDecoder->RenderOsdBits_WithoutRefresh_WithFixedBackgroundColor( (RMuint8*)szSlideBar, 
				rcABS.x1+i +xoffset , rcABS.y1+hoffset,
				m_puzSlideBar->m_nWidth, m_puzSlideBar->m_nHeight,
				m_nFocusedColor);
		}
		nPercent = (m_nCurrentValue - m_nMinOfValue)*100/(m_nMaxOfValue-m_nMinOfValue);
		pMpegDecoder->RenderOsdBits_WithoutRefresh_WithFixedBackgroundColor( (RMuint8*)szSlider, 
			(long int)rcABS.x1 +xoffset + rcABS.GetWidth()*0.7*nPercent/100, (long int)rcABS.y1+hoffset, 
			m_puzSlider->m_nWidth, m_puzSlider->m_nHeight,
			m_nFocusedColor);
		nFontColor = _BLACK;
	}
	else {
		g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nBackgroundColor );
		for ( int i=0;i< rcABS.GetWidth()*0.7;i+=5)
		{
			pMpegDecoder->RenderOsdBits_WithoutRefresh_WithFixedBackgroundColor( (RMuint8*)szSlideBar, 
				rcABS.x1+i+xoffset , rcABS.y1+hoffset,
				m_puzSlideBar->m_nWidth, m_puzSlideBar->m_nHeight,
				m_nBackgroundColor);
		}

		nPercent = (m_nCurrentValue - m_nMinOfValue)*100/(m_nMaxOfValue-m_nMinOfValue);
		pMpegDecoder->RenderOsdBits_WithoutRefresh_WithFixedBackgroundColor( (RMuint8*)szSlider, 
			(long int)rcABS.x1 +xoffset + rcABS.GetWidth()*0.7*nPercent/100, (long int)rcABS.y1+hoffset, 
			m_puzSlider->m_nWidth, m_puzSlider->m_nHeight,
			m_nBackgroundColor);
		nFontColor = _WHITE;
		if( m_disable ) nFontColor = _GRAY;
	}


	sprintf(szTemp,"%d", m_nCurrentValue );
	str2ustr( ucText, szTemp, strlen(szTemp) );

	int length = pT->GetStringWidth( ucText, 12, NULL );
	int offset = rcClient.GetWidth() - length;
	
	pT->DrawUC16Text( ucText,
		rcABS.x1+offset, rcABS.y1+hoffset-2,
		nFontColor,
		12,
		DT_TRANSPARENCY );
}

void CSlideBar::OnKeypressed( const KeyEvent& evt )
{
  if( m_disable ) {
    return;
  }
  
	switch( evt.value ) {
	case VK_LEFT:
		if (m_isPresetType )
		{
			m_nIndexOfPreset--;
			if(m_nIndexOfPreset < 0 )
			{
				m_nIndexOfPreset = 0;
				return;
			}
			m_nCurrentValue = m_azPreset[m_nIndexOfPreset];
		}
		else
		{
			m_nCurrentValue--;
			if( m_nCurrentValue < m_nMinOfValue ) {
				m_nCurrentValue = m_nMinOfValue;
				return;
			}
		}
		OnPaint( rcClient );
		Flush();
		break;
	case VK_RIGHT:
		if (m_isPresetType )
		{
			m_nIndexOfPreset++;
			if(m_nIndexOfPreset >= m_nCountOfPreset)
			{
				m_nIndexOfPreset = m_nCountOfPreset-1;
				return;
			}
			m_nCurrentValue = m_azPreset[m_nIndexOfPreset];
		}
		else
		{
			m_nCurrentValue++;
			if( m_nCurrentValue > m_nMaxOfValue ) {
				m_nCurrentValue = m_nMaxOfValue;
				return;
			}
		}
		OnPaint( rcClient );
		Flush();
		break;
	}

}

void CSlideBar::Show( void )
{
	OnPaint( rcClient );
}

void CSlideBar::Hide( void )
{
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), COLOR_TRANSPARENT );
	op_Flush();
}

void CSlideBar::AddPreset( int azPreset[], const int length )
{
	int i;
	
	m_isPresetType = TRUE;
	m_azPreset = azPreset;
	m_nCountOfPreset = length;
	m_nIndexOfPreset = 0;
	m_nMinOfValue = m_azPreset[0];
	m_nMaxOfValue = m_azPreset[m_nCountOfPreset-1];
	m_nCurrentValue = m_azPreset[m_nIndexOfPreset];
}

void CSlideBar::SetPreset( int nIndex )
{
	if ( nIndex >=0 && nIndex < m_nCountOfPreset ) {
		m_nIndexOfPreset = nIndex;
		m_nCurrentValue = m_azPreset[m_nIndexOfPreset];
	}
}
int	CSlideBar::GetPreset( void )
{
	return ( m_nIndexOfPreset );
}


void CSlideBar::Add( const CString& item )
{
	CString* pItem = new CString( item );
	m_uzList.Add( pItem );
}

void CSlideBar::SetValue( int nData )
{
	int	i;
	
	if ( nData >= m_nMinOfValue && nData <= m_nMaxOfValue )
	{
		if (m_isPresetType )
		{
			for (i=m_nCountOfPreset-1;i>=0;i--)
			{
				if ( m_azPreset[i]== nData )
					break;
			}
			m_nIndexOfPreset = i;
		}
		m_nCurrentValue = nData;
	}
	else
	{
		m_nCurrentValue = 0;
	}
}
int CSlideBar::GetValue( void )
{ 
//	printf( "m_nCurrentValue: %d\n", m_nCurrentValue );
	return m_nCurrentValue; 
}

