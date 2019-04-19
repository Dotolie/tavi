//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Radio Button class definition

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/RadioButton.cpp,v $

// $Revision: 1.2 $

// $Date: 2006/06/07 08:24:53 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: RadioButton.cpp,v $
// Revision 1.2  2006/06/07 08:24:53  zstein
// - added disable function.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/10/18 01:09:03  zstein
// - extended gap between radio button and radio button.
//
// Revision 1.10  2005/10/17 03:33:41  yong
// AddItem(char *) length bug fix
//
// Revision 1.9  2005/10/13 10:46:40  zstein
// - changed managing objects.
//
// Revision 1.8  2005/05/15 11:08:49  zstein
// *** empty log message ***
//
// Revision 1.7  2005/04/27 06:18:12  zstein
// BUG FIXED: invalid position.
//
// Revision 1.6  2005/04/27 04:53:21  zstein
// Support Multi-languages.
// Support resource text.
//
// Revision 1.5  2005/03/04 16:02:34  ywkim
// Change initial value of offsetx for text align
//
// Revision 1.4  2005/03/01 02:11:05  zstein
// *** empty log message ***
//
// Revision 1.3  2005/02/25 13:57:17  zstein
// Added WindowType
//
// Revision 1.2  2005/02/25 01:11:38  zstein
// Radio Button Class
//
// Revision 1.1  2005/02/16 16:03:25  zstein
// *** empty log message ***
//

//******************************************************************************

#include <string.h>
#include "fbdev.h"
#include "RadioButton.h"
#include "../Lib/LibImage.h"

// Macros
#define COLOR_FOCUSED			27
#define TRANSPARENT_COLOR		0
#define ITEM_WIDTH				70
#define W						COLOR_WHITE
#define B						27
#define SPACING_RT				2	// between radio icon and text
#define SPACING_TR				16	// between text and next radioicon
#define MARGIN					2
#define FONT_HEIGHT				12


// Extern Variables
extern CTextEngine_BDF* pT;

// Local Variables
static char szSelectedImage[10][10] = {
	W, W, W, W, W, W, W, W, W, W,
	W, B, B, B, B, B, B, B, B, W,
	W, B, W, W, W, W, W, W, B, W,
	W, B, W, W, W, W, W, W, B, W,
	W, B, W, W, W, W, W, W, B, W,
	W, B, W, W, W, W, W, W, B, W,
	W, B, W, W, W, W, W, W, B, W,
	W, B, W, W, W, W, W, W, B, W,
	W, B, B, B, B, B, B, B, B, W,
	W, W, W, W, W, W, W, W, W, W
};

static char szUnSelectedImage[10][10] = {
	W, W, W, W, W, W, W, W, W, W,
	W, B, B, B, B, B, B, B, B, W,
	W, B, B, B, B, B, B, B, B, W,
	W, B, B, B, B, B, B, B, B, W,
	W, B, B, B, B, B, B, B, B, W,
	W, B, B, B, B, B, B, B, B, W,
	W, B, B, B, B, B, B, B, B, W,
	W, B, B, B, B, B, B, B, B, W,
	W, B, B, B, B, B, B, B, B, W,
	W, W, W, W, W, W, W, W, W, W
};

CRadioButton::CRadioButton( CWindow* parent, bool fItemType ) :
	CWindow( parent )
{
	m_parent = parent;
	m_nSelectedIdx = -1;

	m_uzSelectedIcon = new CPixmap;
	m_uzSelectedIcon->CreatePixmap( 10, 10, 8, (char*)szSelectedImage, false );
	m_uzUnSelectedIcon = new CPixmap;
	m_uzUnSelectedIcon->CreatePixmap( 10, 10, 8, (char*)szUnSelectedImage, false );

	// Minimun size
	CWindow::Resize( CRect(0, 0, 139, 22), false );

	m_nWindowType = WINDOW_TYPE_RADIOBUTTON;
	m_nFocusedColor = 28;
	m_nItemCount = 0;
}

CRadioButton::~CRadioButton( void )
{
	m_uzList.FreeAll();
}

 void CRadioButton::AddItem( const CString& szText, bool fSelect )
{
	int w;
	RadioItem* pItem = new RadioItem( szText );
	if( pItem->m_szText.IsUnicode() == false ) pItem->m_szText.toUTF16();
	const uchar_t* sz = ( const uchar_t* )pItem->m_szText;
	// guessing size
	if( m_nItemCount > 1 ) m_CanvasSize.width += SPACING_TR;
	m_CanvasSize.width += m_uzSelectedIcon->m_nWidth;
	m_CanvasSize.width += SPACING_RT;
	w = pT->GetStringWidth( (uchar_t*)sz, FONT_HEIGHT, 0 );
	m_CanvasSize.width += w;

	pItem->m_nWidth = w;
	m_nItemCount = m_uzList.Add( pItem );
	if( fSelect ) m_nSelectedIdx = m_nItemCount-1;
}

void CRadioButton::AddItem( int nResID, bool fSelect )
{
	int w;
	
	// guessing size
	if( m_nItemCount > 1 ) m_CanvasSize.width += SPACING_TR;
	m_CanvasSize.width += m_uzSelectedIcon->m_nWidth;
	m_CanvasSize.width += SPACING_RT;
	w = pT->GetStringWidth( nResID, FONT_HEIGHT, 0 );
	m_CanvasSize.width += w;
	
	RadioItem* pItem = new RadioItem( nResID );
	pItem->m_nWidth = w;
	m_nItemCount = m_uzList.Add( pItem );
	if( fSelect ) m_nSelectedIdx = m_nItemCount-1;
}

void CRadioButton::OnPaint( const CRect& rc )
{
	int color, bgcolor;

	if( m_fFocus ) {
		bgcolor = m_nFocusedColor;
		color = _BLACK;
	}
	else {
		bgcolor = m_nBackgroundColor;
		color = _WHITE;
		if( m_disable ) color = _GRAY;
	}
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), bgcolor);

	// guessing size of canvas
	int gw, gh;
	int x, offset;
	gw = ( rcABS.GetWidth() > m_CanvasSize.width ) ? rcABS.GetWidth() : m_CanvasSize.width;
	gh = ( rcABS.GetHeight() > m_CanvasSize.height ) ? rcABS.GetHeight() : m_CanvasSize.height;
	CPixmap* pmBuf = new CPixmap;
	CPixmap* pm;
	RadioItem* pItem;
	uchar_t* szText;
	pmBuf->CreatePixmap( gw, gh, 8, NULL, true );
	pmBuf->Fill( 0, 0, gw, gh, bgcolor );
	x = MARGIN;
	offset = (gh-FONT_HEIGHT)/2;
	for( int i=0; i<m_nItemCount; i++ ) {
		if( m_nSelectedIdx == i ) pm = m_uzSelectedIcon;
		else pm = m_uzUnSelectedIcon;
		if( i ) x += SPACING_TR;
		op_BitBlt( pmBuf, x, offset+1, 
			pm, 0, 0, pm->m_nWidth, pm->m_nHeight );
		// draw text
		x += pm->m_nWidth;
		x += SPACING_RT;
		pItem = m_uzList[i];
		if( pItem->m_fText ) szText = (uchar_t*)pItem->m_szText;
		else szText = ( uchar_t* )pT->GetTextRes( pItem->m_nResID );
		pT->DrawUC16Text( szText, x, offset, color, FONT_HEIGHT, DT_TRANSPARENCY, pmBuf );
		x += pItem->m_nWidth;
	}
	op_BitBlt( &g_SysPixmap, rcABS.x1, rcABS.y1,
		pmBuf, 0, 0, rcABS.GetWidth(), rcABS.GetHeight() );
	delete pmBuf;
}

void CRadioButton::OnKeypressed( const KeyEvent& evt )
{
  if( m_disable ) {
    return;
  }
  
	switch( evt.value ) {
	case VK_LEFT:
		m_nSelectedIdx--;
		if( m_nSelectedIdx < 0 ) {
			m_nSelectedIdx = 0;
			return;
		}
		OnPaint( rcClient );
		Flush();
		break;
	case VK_RIGHT:
		m_nSelectedIdx++;
		if( m_fItemType ) {
			if( m_nSelectedIdx >= m_nItemCount ) {
				m_nSelectedIdx = m_nItemCount - 1;
				return;
			}
		}
		else {
			if( m_nSelectedIdx >= m_uzList.Count() ) {
				m_nSelectedIdx = m_uzList.Count() - 1;
				return;
			}
		}
		OnPaint( rcClient );
		Flush();
		break;
	case VK_ESC:
		break;
	}
}
