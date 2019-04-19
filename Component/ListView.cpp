//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : List view class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ListView.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/04/15 02:09:18 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ListView.cpp,v $
// Revision 1.2  2006/04/15 02:09:18  zstein
// - enabled item color.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.46  2006/01/19 05:25:53  zstein
// *** empty log message ***
//
// Revision 1.45  2005/12/27 09:57:59  zstein
// *** empty log message ***
//
// Revision 1.44  2005/12/21 03:17:20  zstein
// *** empty log message ***
//
// Revision 1.43  2005/12/16 04:29:33  zstein
// *** empty log message ***
//
// Revision 1.42  2005/12/02 05:09:35  zstein
// *** empty log message ***
//
// Revision 1.41  2005/12/01 01:03:44  zstein
// *** empty log message ***
//
// Revision 1.40  2005/11/22 09:09:30  zstein
// *** empty log message ***
//
// Revision 1.39  2005/11/16 06:42:13  zstein
// - changed compare functions.
//
// Revision 1.38  2005/11/08 07:28:18  zstein
// *** empty log message ***
//
// Revision 1.37  2005/11/08 01:30:14  zstein
// *** empty log message ***
//
// Revision 1.36  2005/11/07 04:19:10  zstein
// *** empty log message ***
//
//*******************************************************************************
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <bitmap.h>

#include "fbdev.h"
#include "ListView.h"
#include "../Lib/LibImage.h"
#include <Lib/LibBuzzer.h>
#include "CharSet.h"

#define DEBUG
#ifdef DEBUG
#define CListViewDPrint( _fmt, args... )		printf( "[CListView]"_fmt, ## args )
#else
#define CListViewDPrint( _fmt, args... )
#endif /* DEBUG */

#define SCROLL_OFFSET				100
#define SCROLLBAR_RIGTHMARGIN		6

extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CTextEngine_BDF* pT;


// Local Variables & Functions
#if 0
static int NameAscentCompare( CListItem* pNewItem, CListItem* pivot )
{
	int ret;
	//printf( "comp: " );
	//pNewItem->m_szText.Print();
	//printf( ", " );
	//pivot->m_szText.Print();
	if( pNewItem->m_nType != pivot->m_nType ) {
		if( pNewItem->m_nType > pivot->m_nType ) ret = -1;
		else ret = 1;
	}
	else {
		ret = ustrcmp( (uchar_t*)pNewItem->m_szText, (uchar_t*)pivot->m_szText );
		//if( pNewItem->m_szText > pivot->m_szText ) ret = 1;
		//else ret = -1;
	}
	//printf( ": %d\n", ret );

	return ret;
}

static int NameDescentCompare( CListItem* pNewItem, CListItem* pivot )
{
	int ret;

	//printf( "comp: " );
	//pNewItem->m_szText.Print();
	//printf( ", " );
	//pivot->m_szText.Print();
	if( pNewItem->m_nType != pivot->m_nType ) {
		if( pNewItem->m_nType > pivot->m_nType ) ret = 1;
		else ret = -1;
	}
	else {
		//if( pNewItem->m_szText < pivot->m_szText ) ret = -1;
		//else ret = 1;
		ret = ustrcmp( (uchar_t*)pivot->m_szText, (uchar_t*)pNewItem->m_szText );
	}
	//printf( ": %d\n", ret );

	return ret;
}

static int DateCompareAscent( CListItem* pNewItem, CListItem* pivot )
{
	int ret;
	if( pNewItem->m_nType != pivot->m_nType ) {
		if( pNewItem->m_nType > pivot->m_nType ) ret = -1;
		else ret = 1;
	}
	else {
		ret = ( pNewItem->m_nDate - pivot->m_nDate );
		//if( pNewItem->m_nDate > pivot->m_nDate ) ret = 1;
		//else if( pNewItem->m_nDate < pivot->m_nDate ) ret = -1;
		//else ret = 0;
	}

	return ret;
}

static int DateCompareDescent( CListItem* pNewItem, CListItem* pivot )
{
	int ret;
	if( pNewItem->m_nType != pivot->m_nType ) {
		if( pNewItem->m_nType > pivot->m_nType ) ret = 1;
		else ret = -1;
	}
	else {
		ret = ( pivot->m_nDate - pNewItem->m_nDate );
		//if( pNewItem->m_nDate > pivot->m_nDate ) ret = 1;
		//else if( pNewItem->m_nDate < pivot->m_nDate ) ret = -1;
		//else ret = 0;
	}

	return ret;
}
#else
//static CArray< CListItem* >* pp;
static int NameAscentCompare( const void* a, const void* b )
{
	int ret;
	CListItem* pa = *( CListItem** )a;
	CListItem* pb = *( CListItem** )b;

	#if 0
	CListItem* p;
	for( int i=0; i<pp->Count(); i++ ) {
		p = (*pp)[i];
		p->m_szText.Print();
		printf( ", " );
	}
	printf( "\n" );
	printf( "NameAscent:(%d, %d) ", pa->m_nType, pb->m_nType );
	pa->m_szText.Print();
	printf( ", " );
	pb->m_szText.Print();
	#endif
	if( pa->m_nType != pb->m_nType ) {
		if( pa->m_nType > pb->m_nType ) ret = -1;
		else ret = 1;
	}
	else {
		ret = ustrcmp( (uchar_t*)pa->m_szText, (uchar_t*)pb->m_szText );
	}
	//printf( "->%d\n", ret );

	return ret;
}

static int NameDescentCompare( const void* a, const void* b )
{
	int ret;
	CListItem* pa = *( CListItem** )a;
	CListItem* pb = *( CListItem** )b;

	#if 0
	CListItem* p;
	for( int i=0; i<pp->Count(); i++ ) {
		p = (*pp)[i];
		p->m_szText.Print();
		printf( ", " );
	}
	printf( "\n" );
	printf( "NameDescent:(%d, %d) ", pa->m_nType, pb->m_nType );
	pa->m_szText.Print();
	printf( ", " );
	pb->m_szText.Print();
	#endif
	if( pa->m_nType != pb->m_nType ) {
		if( pa->m_nType > pb->m_nType ) ret = -1;
		else ret = 1;
	}
	else {
		ret = ustrcmp( (uchar_t*)pb->m_szText, (uchar_t*)pa->m_szText );
	}
	//printf( "->%d\n", ret );
	
	return ret;
}

static int DateCompareAscent( const void* a, const void* b )
{
	int ret;
	CListItem* pa = *( CListItem** )a;
	CListItem* pb = *( CListItem** )b;
	
	if( pa->m_nType != pb->m_nType ) {
		if( pa->m_nType > pb->m_nType ) ret = -1;
		else ret = 1;
	}
	else {
		ret = ( pa->m_nDate - pb->m_nDate );
	}

	return ret;
}

static int DateCompareDescent( const void* a, const void* b )
{
	int ret;
	CListItem* pa = *( CListItem** )a;
	CListItem* pb = *( CListItem** )b;
	
	if( pa->m_nType != pb->m_nType ) {
		if( pa->m_nType > pb->m_nType ) ret = -1;
		else ret = 1;
	}
	else {
		ret = ( pb->m_nDate - pa->m_nDate );
	}

	return ret;
}
#endif


// CListItem Class
CListItem::CListItem( void ) :
	m_nIconIdx( -1 ),
	m_nIdx( -1 ),
	m_nSelectedIconIdx( -1 )
{
	m_nEnable = true;
}

CListItem::CListItem( int nIdx, int nIconIdx, int nSelectedIconIdx, char* szText, bool fUnicode ) :
	m_nIdx( nIdx ),
	m_nIconIdx( nIconIdx ),
	m_nSelectedIconIdx( nSelectedIconIdx ),
	m_nResID( -1 )
{
	m_szText.SetString( szText, fUnicode );
	m_nEnable = true;
	m_color = _WHITE;
}

CListItem::CListItem( int nIdx, int nIconIdx, int nSelectedIconIdx, int nResID ) :
	m_nIdx( nIdx ),
	m_nIconIdx( nIconIdx ),
	m_nSelectedIconIdx( nSelectedIconIdx ),
	m_nResID( nResID )
{
	m_nEnable = true;
	m_color = _WHITE;
}


CListItem::~CListItem( void )
{
}

#define BAR_HEIGHT		23
#define LEFT_MARGIN		2
#define RIGHT_MARGIN	2

#define LIST_BAR_BORDER_COLOR			24

static unsigned char* GetImageElement( BITMAPFILEINFO256 bmp, int dh, int id, int count )
{
	unsigned char* p;
	int w = bmp.bitmapinfoheader.biWidth;

	p = bmp.image_buffer+w*dh*id;

	return p;
}

CListView::CListView( CWindow* parent, int nSort, int nViewCount ) :
	CWindow( parent, CRect(0, 0, 0, 0), 0, 0, 0, 0 ),
	m_nOffset( 0 ),
	m_nListCount( nViewCount )
{
	m_nStartIdx = 0;
	m_nSelectedIdx = 0;
	m_nBarHeight = BAR_HEIGHT; // default

	m_pScrollBar = new CVScrollBar( this, m_nListCount, 0 );
	m_pScrollBar->SetBorderWidth( 1 );
	m_pScrollBar->SetScrollBackgroundColor( 28 );
	m_pScrollBar->SetScrollColor( 17 ); // White
	m_pScrollBar->SetBorderColor( 24 );
	m_nScrollBarTM = 0;
	m_nScrollBarBM = 0;

	m_puzBarPixmap = new CPixmap;
	m_puzBarPixmap->CreatePixmap( 
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biWidth, 
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biHeight,
		8,
		(char*)TAVI_IMAGES[LIST_SELECTBAR].image_buffer,
		false );

	int count = TAVI_IMAGES[TYPE_ICON_LIST].bitmapinfoheader.biHeight/TYPE_ICON_HEIGHT;
	for( int i=0; i<count; i++ ) {
		CPixmap* pixmap = new CPixmap;
		pixmap->CreatePixmap( 12, 12, 8, (char*)GetImageElement(TAVI_IMAGES[TYPE_ICON_LIST], 
			TYPE_ICON_HEIGHT, i, count), false );
		m_uzIconList.Add( pixmap );
	}

	m_nSort = nSort;
	m_fUpdate = false;
	m_puzBarText = NULL;

	//pp = &m_uzList;
}

/* CListVew Class
 */
CListView::~CListView()
{
	m_uzList.FreeAll();
	m_uzIconList.FreeAll();

	if( m_puzBarPixmap ) {
		delete m_puzBarPixmap;
	}

	if( m_puzBarText ) {
		delete m_puzBarText;
	}

	// NOTE: DO NOT DELETE m_pScrollBar.
	// CWindow::~CWindow will delete her children.
}

int CListView::GetCount( void )
{
	return m_uzList.Count();
}

void CListView::SetSelectBarPixmap( CPixmap* pixmap )
{
	#if 0
	if( !pixmap ) return;
	m_puzBarPixmap = pixmap;
	m_nBarHeight = pixmap->m_nHeight;
	#endif
}

int CListView::AddItem( CListItem* uzItem )
{
	int count;

	uzItem->m_nIdx = m_uzList.Count();
	m_uzList.Add( uzItem );

	return count;
}

int CListView::AddPixmapIcon( CPixmap* pixmap )
{
	return m_uzIconList.Add( pixmap );
}

void CListView::DrawItem( CListItem* puzItem, int idx, bool fSelect )
{
	CPixmap* pixmap;
	//char szCount[10];
	int color;
	int fontidx;
	uchar_t wsz[256];
	int option;

	if( fSelect ) {
		color = _BLACK;
		option = DT_TRANSPARENCY;
		fontidx = m_nSelectedFontIdx;
	}
	else {
		color = puzItem->m_color;
		option = DT_TRANSPARENCY|DT_OUTLINE;
		fontidx = m_nNormalFontIdx;
	}

	if( puzItem->m_nEnable == false ) color = _GRAY60;
	
	// Draw selectbar
	if( fSelect ) {
		op_BitBlt( &g_SysPixmap, rcABS.x1+2+m_nOffset, rcABS.y1+2+idx*m_puzBarPixmap->m_nHeight,
			m_puzBarPixmap, 0, 0, m_puzBarPixmap->m_nWidth, m_puzBarPixmap->m_nHeight );
		pixmap = m_uzIconList[puzItem->m_nIconIdx+1];
	}
	else {
		if( m_puzBarPixmap ) {
			g_SysPixmap.Fill( rcABS.x1+2+m_nOffset, rcABS.y1+2+idx*m_puzBarPixmap->m_nHeight,
				m_puzBarPixmap->m_nWidth, m_puzBarPixmap->m_nHeight, m_nBackgroundColor);
		}
		pixmap = m_uzIconList[puzItem->m_nIconIdx];
	}

	// Draw Icon
	if( puzItem->m_nIconIdx >= 0 ) {
		op_BitBlt( &g_SysPixmap, rcABS.x1+m_nOffset+9, rcABS.y1+7+idx*m_puzBarPixmap->m_nHeight,
				pixmap, 0, 0, pixmap->m_nWidth, pixmap->m_nHeight );
	}
	
	uchar_t* pwcString;
	// normal text
	int x, len, nBoundaryLen;
	if( puzItem->m_nIconIdx == -1 ) x = rcABS.x1 + 10 + m_nOffset;
	else x = rcABS.x1+30+m_nOffset;
	if( puzItem->m_nResID == -1 ) {
		len = puzItem->m_szText.GetSize();
		if( puzItem->m_szText.IsUnicode() ) {
			pwcString = ( uchar_t* )puzItem->m_szText;
			
		}
		else {
			len = ConvertUnicode( (char*)puzItem->m_szText, puzItem->m_szText.GetSize(), 
				wsz, sizeof(wsz), TaviSysParam.etc.language );
			pwcString = wsz;
		}
		nBoundaryLen = pT->GetBoundaryLength( pwcString, len, 12, option, m_puzBarPixmap->m_nWidth-34 );	
		if( nBoundaryLen < len ) puzItem->m_fScroll = true;
		else puzItem->m_fScroll = false;
		
		for( int i=0; i<nBoundaryLen; i++ ) {
			x += pT->DrawCharacter( pwcString[i], x, rcABS.y1+7+idx*m_puzBarPixmap->m_nHeight,
					color, 12, option );
		}
	}
	else { // Resource text
		pT->DrawResText( puzItem->m_nResID, x, rcABS.y1+7+idx*m_puzBarPixmap->m_nHeight,
					color, 12, option );
	}

	if( fSelect && (puzItem->m_nResID == -1) ) {
		if( m_puzBarText ) {
			delete m_puzBarText;
			m_puzBarText = NULL;
		}

		if( puzItem->m_fScroll == false ) return;

		CSize size = pT->GetStringSize( pwcString, len, 12 );
		m_puzBarText = new CPixmap;
		m_puzBarText->CreatePixmap( size.width, size.height, 8, NULL, true );
		m_puzBarText->Fill( 0, 0, size.width, size.height, 27 );
		puzItem->m_nWidthInPixel = size.width;
		x = 0;
		for( int i=0; i<len; i++ ) {
                    if( pwcString[i] == 0x20 ) {
                        x+=4;
                    } else {
                        x += pT->DrawCharacter( pwcString[i], x, 0, color, 12, option, m_puzBarText );
                        x++;
                    }
		}

		// scrollwidth
		size = pT->GetStringSize( pwcString, nBoundaryLen, 12 );
		puzItem->m_nScrollWidth = size.width;

		m_nTextScrollPos = 0;
		m_nStartWaitCnt = 0;
	}
}

void CListView::UpdateList( int prev, int next )
{
	CListItem* uzItem;
	CPixmap* pixmap;
	int color;
	char szCount[10];

	if( m_fVisible == false ) return;

	m_fUpdate = false;
	uzItem = m_uzList[m_nStartIdx+prev];
	DrawItem( uzItem, prev, false );
	uzItem = m_uzList[m_nStartIdx+next];
	DrawItem( uzItem, next, true );
	op_Flush();
}

void CListView::OnPaint( const CRect& rc )
{
	if( m_fVisible == false ) return;
	if( m_uzList.Count() == 0 ) return;
	
	int i, nCount, nRemind;
	CListItem* uzItem;
	// substract scrollbar area
	CRect rcTmp = rc;
	rcTmp.x2 = rcTmp.x1 + m_pScrollBar->rcClient.x1-1;
	int dw = rcABS.GetWidth() - rcTmp.GetWidth();
	int dh = rcABS.GetHeight() - rcTmp.GetHeight();
	bool fShowScrollBar;

	if( dw < 0 ) dw = 0;
	if( dh < 0 ) dh = 0;

	if( m_uzList.Count() > m_nListCount ) fShowScrollBar = true;
	else {
		fShowScrollBar = false;
		dw = dh = 0;
	}

	nRemind = m_uzList.Count() - m_nStartIdx;
	//g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nBackgroundColor );
	//g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), _GRAY20 );
	DrawBackground();
	for( i=0; i<nRemind && i<m_nListCount; i++ ) {
		uzItem = m_uzList[m_nStartIdx+i];
		if( m_nSelectedIdx == i ) DrawItem( uzItem, i, true );
		else DrawItem( uzItem, i, false );
	}

	// Draw scrollbar
	if( fShowScrollBar ) {
		m_pScrollBar->SetVisible( true );
		m_pScrollBar->Show();
	} 
	else {
		m_pScrollBar->SetVisible( false );
	}
	op_Flush();
}

void CListView::OnKeypressed( const KeyEvent& evt )
{
	int prev;

	prev = m_nSelectedIdx;
	switch( evt.value ) {
	case VK_UP:
		m_nSelectedIdx--;
		if( m_nSelectedIdx < 0 ) {
			m_nSelectedIdx = 0;
			m_nStartIdx--;
			if( m_nStartIdx < 0 ) {
				if( m_uzList.Count() <= m_nListCount ) { 
					m_nStartIdx = 0;
					m_nSelectedIdx = m_uzList.Count()-1;
				}
				else {
					m_nStartIdx = m_uzList.Count() - m_nListCount;
					m_nSelectedIdx = m_nListCount-1;
				}
			}
			if( m_uzList.Count() > m_nListCount ) 
				//m_pScrollBar->UpdateScrollBar( m_nStartIdx );
				m_pScrollBar->SetStart( m_nStartIdx );
			m_fUpdate = true;
			m_nUpdateDir = 0; // up
			OnPaint( rcClient );
			m_fUpdate = false;
		} 
		else {
			UpdateList( prev, m_nSelectedIdx );
		}
		break;
	case VK_DOWN:
		//check boundary
		if( (m_nSelectedIdx+m_nStartIdx) >= (m_uzList.Count()-1) ) {
			//if( m_uzList.Count() <= m_nListCount ) return;
			m_nSelectedIdx = 0;
			m_nStartIdx = 0;
			if( m_uzList.Count() > m_nListCount ) 
				//m_pScrollBar->UpdateScrollBar(m_nStartIdx );
				m_pScrollBar->SetStart( m_nStartIdx );
			m_fUpdate = true;
			OnPaint( rcClient );
			m_fUpdate = false;
			return;
		}
		m_nSelectedIdx++;
		if( m_nSelectedIdx >= m_nListCount ) {
			m_nSelectedIdx = m_nListCount-1;
			m_nStartIdx++;
			if( m_uzList.Count() > m_nListCount ) 
				//m_pScrollBar->UpdateScrollBar(m_nStartIdx );
				m_pScrollBar->SetStart( m_nStartIdx );
			m_fUpdate = true;
			m_nUpdateDir = 1; // down
			OnPaint( rcClient );
			m_fUpdate = false;
		}
		else {
			UpdateList( prev, m_nSelectedIdx );
		}
		break;
	}
}

void CListView::OnMouseEvent( const MouseEvent& evt )
{
	KeyEvent kevt = { 0, 0 };

	if( evt.click ) {
		kevt.value = VK_ENTER;
		kevt.pressed = 1;
	}
	else {
		if( evt.x > 0 ) kevt.value = VK_DOWN;
		else if( evt.x < 0  ) kevt.value = VK_UP;
	}
	if( kevt.value ) OnKeypressed( kevt );
}

void CListView::ClearList( void )
{
	m_uzList.FreeAll();

	m_nSelectedIdx = 0;
	m_nStartIdx = 0;
}

CListItem* CListView::GetSelectedItem( void )
{
	if( m_uzList.Count() == 0 ) return NULL;
	return m_uzList[m_nSelectedIdx+m_nStartIdx];
}

void CListView::Resize( const CRect& rc, bool fUpdate )
{
	CWindow::Resize( rc, fUpdate );
	m_pScrollBar->Resize( 
		CRect(rc.GetWidth()-13, 0+m_nScrollBarTM, rc.GetWidth()-SCROLLBAR_RIGTHMARGIN, 
		rc.GetHeight()-1-m_nScrollBarBM), false );
	if( m_puzBarPixmap->m_nWidth > rcABS.GetWidth() ) {
		delete m_puzBarPixmap;
		int w = rcABS.GetWidth()-m_pScrollBar->rcABS.GetWidth()-SCROLLBAR_RIGTHMARGIN-2;
		m_puzBarPixmap = new CPixmap;
		m_puzBarPixmap->CreatePixmap( w, BAR_HEIGHT, 8, NULL, true );
		m_puzBarPixmap->Fill( 0, 0, 
			m_puzBarPixmap->m_nWidth, m_puzBarPixmap->m_nHeight, 17 );
	}
	//CRect rc3 = m_pScrollBar->rcABS;
	//printf( "scroll x: %d, y: %d, w: %d, h: %d\n", rc3.x1, rc3.y1, rc3.GetWidth(), rc3.GetHeight() );
}

void CListView::ResetScrollBar( void )
{
	m_pScrollBar->SetStart( m_nStartIdx );
	m_pScrollBar->SetTotal( m_uzList.Count() );
}

CListItem* CListView::GetItem( int index )
{
	if( index >= m_uzList.Count() ) return NULL;

	return m_uzList[index];
}

void CListView::Sort( int type )
{
	if( m_nSort != type ) {
		m_nStartIdx = 0;
		m_nSelectedIdx = 0;	
		m_nSort = type;
	}

	if( m_uzList.Count() == 0 ) return;
	
	switch( type ) {
	case NAME_ASCENT:
		m_uzList.Sort( NameAscentCompare );
		break;
	case NAME_DESCENT:
		m_uzList.Sort( NameDescentCompare );
		break;
	case DATE_ASCENT:
		m_uzList.Sort( DateCompareAscent );
		break;
	case DATE_DESCENT:
		m_uzList.Sort( DateCompareDescent );
		break;
	}
}

/* 
 * return true when scrolls all characters.
 */
 #define TEXT_SCROLL_DELAY		2
 #define TEXT_START_X_OFFSET	30
bool CListView::ScrollText( void )
{
	CListItem* puzItem = m_uzList[m_nStartIdx+m_nSelectedIdx];
	struct timeval tv1, tv2;
	int scrollwidth;
	if( !puzItem ) return false;

	if( puzItem->m_fScroll == false ) return false;

	gettimeofday( &tv1, NULL );
	if( m_nStartWaitCnt == 0 ) {
		gettimeofday( &tv2, NULL );
		m_nStartWaitCnt = tv2.tv_sec;
	}
	if( (tv1.tv_sec-m_nStartWaitCnt) < TEXT_SCROLL_DELAY ) return true;

	int w;
	scrollwidth = m_puzBarPixmap->m_nWidth - TEXT_START_X_OFFSET-4;
	w = m_puzBarText->m_nWidth - m_nTextScrollPos; 
	if( w > scrollwidth ) w = scrollwidth;
	// draw string until reach the half of bar length
	//if( m_nTextScrollPos >= puzItem->m_nWidthInPixel/2 ) {
	if( w < (scrollwidth/2) ) {
		m_nTextScrollPos = 0;
		m_nStartWaitCnt = 0;
		op_BitBlt( &g_SysPixmap, 
			rcABS.x1+2+m_nOffset+TEXT_START_X_OFFSET, 
			rcABS.y1+2+m_nSelectedIdx*m_puzBarPixmap->m_nHeight,
			m_puzBarPixmap, 30, 0, 
			m_puzBarPixmap->m_nWidth-TEXT_START_X_OFFSET, 
			m_puzBarPixmap->m_nHeight );
		op_BitBlt( &g_SysPixmap, 
			rcABS.x1+TEXT_START_X_OFFSET+m_nOffset, 
			rcABS.y1+7+m_nSelectedIdx*m_puzBarPixmap->m_nHeight,
			m_puzBarText, m_nTextScrollPos, 0, 
			scrollwidth, m_puzBarText->m_nHeight );
		op_Flush();
		return true;
	}

	// DrawItem converts puzItem->m_szText to wide character.
	if( w <= 0 ) return true;
	op_BitBlt( &g_SysPixmap, 
		rcABS.x1+2+m_nOffset+TEXT_START_X_OFFSET, 
		rcABS.y1+2+m_nSelectedIdx*m_puzBarPixmap->m_nHeight,
		m_puzBarPixmap, TEXT_START_X_OFFSET, 0, 
		m_puzBarPixmap->m_nWidth-TEXT_START_X_OFFSET, 
		m_puzBarPixmap->m_nHeight );
	op_BitBlt( &g_SysPixmap, 
		rcABS.x1+TEXT_START_X_OFFSET+m_nOffset, 
		rcABS.y1+7+m_nSelectedIdx*m_puzBarPixmap->m_nHeight,
		m_puzBarText, m_nTextScrollPos, 0, 
		w, m_puzBarText->m_nHeight );
	
	op_Flush();

	m_nTextScrollPos++;
	return false;
}


int CListView::DeleteItem( int idx )
{
	bool success;
	success = m_uzList.Delete( idx );
	if( success == false ) return -1;

	//printf( "DeleteItem: %d, %d\n", (m_nSelectedIdx+m_nStartIdx), m_uzList.Count() );
	if( (m_nSelectedIdx+m_nStartIdx) >= m_uzList.Count() ) {
		if( m_nStartIdx ) {
			m_nStartIdx--;
			ResetScrollBar();
		}
		else {
			m_nSelectedIdx--;
		}
	}
	return m_uzList.Count();
}

void CListView::OnWindowEvent( const WindowEvent& evt )
{
	switch( evt.nMessage ) {
	case WINDOW_IDLE:
		if( m_fVisible ) ScrollText();
		break;
	}
}

void CListView::ResetIconList( void )
{
	m_uzIconList.FreeAll();
}

void CListView::DrawBackground( void )
{
	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), rcABS.GetHeight(), m_nBackgroundColor );
}
