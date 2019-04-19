//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Tile viewer
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVTileViewer.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/04/15 01:38:42 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVTileViewer.cpp,v $
// Revision 1.2  2006/04/15 01:38:42  zstein
// - TAVICAST.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.8  2005/12/05 03:02:21  zstein
// *** empty log message ***
//
// Revision 1.7  2005/12/02 13:40:47  zstein
// *** empty log message ***
//
// Revision 1.6  2005/12/02 05:14:22  zstein
// *** empty log message ***
//
// Revision 1.5  2005/12/01 01:03:57  zstein
// *** empty log message ***
//
// Revision 1.4  2005/11/25 10:22:17  zstein
// *** empty log message ***
//
// Revision 1.3  2005/11/22 09:06:44  zstein
// *** empty log message ***
//
// Revision 1.2  2005/11/08 07:25:54  zstein
// *** empty log message ***
//
// Revision 1.1  2005/11/08 01:19:24  zstein
// *** empty log message ***
//
//******************************************************************************

#include "IVTileViewer.h"
#include <Lib/LibImage.h>
#include <Component/image.h>
#include <Component/FileUtil.h>
#include <Component/Bitmap.h>
#include <Task/tavi_global.h>
#include "ImageViewer.h"


// Macros
#define GAP						2
#define THUMB_WIDTH			56
#define THUMB_HEIGHT			40
#define BAR_WIDTH				(THUMB_WIDTH+2*GAP)
#define BAR_HEIGHT				(THUMB_HEIGHT+2*GAP)
#define ROWS					4
#define COLS						5
#define VIEW_ITEMS				(ROWS*COLS)
#define BORDER_WIDTH			6


#define PREVIEW_ICON_COUNT	3

// Extern variables
extern RMuint32* thumbnailY;
extern RMuint32* thumbnailUV;
extern CTextEngine_BDF* pT;
extern void showbackground( int id );

// Local variables
CIVTileViewer::CIVTileViewer( CWindow* parent, const CString& szPath, int nSort, CStack<FolderPrev>* pStack ) :
	CIVListView( parent, szPath, nSort, VIEW_ITEMS, pStack )
{
	m_puzBarPixmap = new CPixmap;
	m_puzBarPixmap->CreatePixmap( BAR_WIDTH, BAR_HEIGHT, 8,  NULL, true );
	m_puzBarPixmap->Fill( 0, 0, BAR_WIDTH, BAR_HEIGHT, _WHITE );
	m_puzBarPixmap->Fill( 2, 2, THUMB_WIDTH, THUMB_HEIGHT, _TRANSPARENT );
	m_EmptyFrame = new CPixmap;
	m_EmptyFrame->CreatePixmap( BAR_WIDTH, BAR_HEIGHT, 8,  NULL, true );
	m_EmptyFrame->Fill( 0, 0, BAR_WIDTH, BAR_HEIGHT, _LEVELMETER_BG );
	m_EmptyFrame->Fill( 2, 2, THUMB_WIDTH, THUMB_HEIGHT, _TRANSPARENT );

	int size = THUMB_WIDTH*THUMB_HEIGHT;
	m_Folder_Y = new unsigned char[size];
	m_Folder_UV = new unsigned char[size/2];
	m_BMP_Y = new unsigned char[size];
	m_BMP_UV = new unsigned char[size/2];
	m_JPG_Y = new unsigned char[size];
	m_JPG_UV = new unsigned char[size/2];

	char* folderpath = _IMAGE_DIR"/controls/photo/folder.jpg";
	char* jpgpath = _IMAGE_DIR"/controls/photo/jpg.jpg";
	char* bmppath = _IMAGE_DIR"/controls/photo/bmp.jpg";
	uchar_t path[MAX_PATH];

	// 54*40Àº 16ÀÇ ¹è¼ö°¡ ¾Æ´Ï¹Ç·Î °¡Àå å°¡±î¿î 16ÀÇ ¹è¼ö·Î ¹öÆÛ ¼³Á¤
	unsigned char* buf_Y = new unsigned char[64*48];
	unsigned char* buf_UV = new unsigned char[64*48/2];
	unsigned char* sy, *suv, *dy, *duv;
	int i, j;

	int iw, ih;
	str2ustr( path, folderpath );
	LoadImageYUV420( (char*)path, 
		buf_Y, buf_UV,
		size, size/2, &iw, &ih, NULL );
	sy = buf_Y;
	suv = buf_UV;
	dy = m_Folder_Y;
	duv = m_Folder_UV;
	for( i=0; i<THUMB_HEIGHT; i++ ) {
		memcpy( dy, sy, THUMB_WIDTH );
		memcpy( duv, suv, THUMB_WIDTH/2 );
		sy += 64;
		dy += THUMB_WIDTH;
		suv += 32;
		duv += THUMB_WIDTH/2;
	}
	str2ustr( path, jpgpath );
	LoadImageYUV420( (char*)path, 
		buf_Y, buf_UV,
		size, size/2, &iw, &ih, NULL );
	sy = buf_Y;
	suv = buf_UV;
	dy = m_JPG_Y;
	duv = m_JPG_UV;
	for( i=0; i<THUMB_HEIGHT; i++ ) {
		memcpy( dy, sy, THUMB_WIDTH );
		memcpy( duv, suv, THUMB_WIDTH/2 );
		sy += 64;
		dy += THUMB_WIDTH;
		suv += 32;
		duv += THUMB_WIDTH/2;
	}
	str2ustr( path, bmppath );
	LoadImageYUV420( (char*)path, 
		buf_Y, buf_UV,
		size, size/2, &iw, &ih, NULL );
	sy = buf_Y;
	suv = buf_UV;
	dy = m_BMP_Y;
	duv = m_BMP_UV;
	for( i=0; i<THUMB_HEIGHT; i++ ) {
		memcpy( dy, sy, THUMB_WIDTH );
		memcpy( duv, suv, THUMB_WIDTH/2 );
		sy += 64;
		dy += THUMB_WIDTH;
		suv += 32;
		duv += THUMB_WIDTH/2;
	}

	delete buf_Y;
	delete buf_UV;

	m_guide = false;
}

CIVTileViewer::~CIVTileViewer( void )
{
	// free pixmap
	delete[] m_Folder_Y;
	delete[] m_Folder_UV;
	delete[] m_BMP_Y;
	delete[] m_BMP_UV;
	delete[] m_JPG_Y;
	delete[] m_JPG_UV;
}

void CIVTileViewer::DrawItem( CListItem* puzItem, int idx, bool fSelect )
{
	int nOutW, nOutH;
	int nSpacing=0;
	CPixmap* pm;
	PFN_Progress prg;
	char* pY, *pUV;
	bool success;
	int x, y;
	uchar_t path[MAX_PATH];
	uchar_t del[] = {'/', 0};

	pY = ( char* )thumbnailY;
	pUV = ( char* )thumbnailUV;

	// Load thumbnail
	ustrcpy( path, (uchar_t*)m_szCurrentPath );
	ustrcat( path, del );
	ustrcat( path, (uchar_t*)puzItem->m_szText );
	success = g_p_ThumbDB->FetchThumb( path, (char*)pY, (char*)pUV );

	int ridx = idx % COLS;
	int cidx = idx / COLS;
	x = rcABS.x1+BORDER_WIDTH + ridx*(THUMB_WIDTH+GAP);
	y = rcABS.y1+BORDER_WIDTH + cidx*(THUMB_HEIGHT+GAP);

	
	// DRAW SELECT-BAR
	if( fSelect ) {
		pm = m_puzBarPixmap;
	}
	else {
		if( m_norestore ) {
			// CLEAR DRAWING AREA
			g_SysPixmap.Fill( x, y, THUMB_WIDTH, THUMB_HEIGHT, 0 );
			pm = NULL;
		}
		else {
			pm = m_EmptyFrame;
		}
	}
	if( pm ) {
		op_BitBlt( &g_SysPixmap, x-2, y-2, pm, 0, 0, pm->m_nWidth, pm->m_nHeight );
	}
	
	if( puzItem->m_nType == NODE_TYPE_FOLDER ||
	    puzItem->m_nType == NODE_TYPE_VIRTUAL ) {
		op_DrawYUV420( m_Folder_Y, m_Folder_UV, 
				x, y, THUMB_WIDTH, THUMB_HEIGHT );
	}
	else {
		if( success ) {
			op_DrawYUV420( 
			    (unsigned char*)pY, (unsigned char*)pUV, 
			    x, y, 
			    THUMB_WIDTH, THUMB_HEIGHT );
		}
		else {
			unsigned char* py, *puv;
			if( puzItem->m_nMedia == MEDIA_BMP ) {
				py = m_BMP_Y;
				puv = m_BMP_UV;
			}
			else {
				py = m_JPG_Y;
				puv = m_JPG_UV;
			}
			op_DrawYUV420( py, puv, 
				x, y, THUMB_WIDTH, THUMB_HEIGHT );
		}
	}
}

void CIVTileViewer::BeforeScan( void )
{
	//m_ThumbDB.UnLoad();
	//m_ThumbDB.Load( m_szCurrentPath );
}

bool CIVTileViewer::MoveUP( int prev )
{
	//printf( "total: %d, m_nSelectedIdx: %d\n", 
	//	m_uzList.Count(),  m_nSelectedIdx );
	if( m_nSelectedIdx < 0 ) {
		// if first row
		m_nStartIdx -= m_nListCount;
		if( m_nStartIdx < 0 ) {
			m_nStartIdx += m_nListCount;
			return false;
		}
		#if 0
		if( m_nSelectedIdx == -1 ) { // move one step
			m_nSelectedIdx = m_nListCount-1;
		}
		else {
			m_nSelectedIdx = m_nListCount - COLS;
		}
		#else
		else {
			m_nSelectedIdx += m_nListCount;
		}
		#endif
		//showbackground( ID_BG_PHOTO );
		OnPaint( rcClient );
	} 
	else {
		UpdateList( prev, m_nSelectedIdx );
	}
	return true;
}

bool CIVTileViewer::MoveDown( int prev )
{
	int idx = m_nStartIdx + m_nSelectedIdx;

	//printf( "idx: %d, total: %d, m_nSelectedIdx: %d\n", 
	//	idx, m_uzList.Count(),  m_nSelectedIdx );
	if( idx >= m_uzList.Count() ) {
		if( m_uzList.Count() < m_nListCount ) return false;
		// ¹öÆ° ´Ù¿îÀ¸·Î ³»·Áå°¥¶§ ÇöÀç À§Ä¡¿¡´Â ¾øÁö¸¸ ÀÌÀü ¼¿¿¡ »çÁøÀÌ åÀÖÀ» å¶§
		idx = idx - idx%COLS;
		//if( idx >= m_uzList.Count() ) return false;
		if( idx >= COLS ) return false;
		else m_nSelectedIdx = idx-m_nStartIdx;
	}
	
	if( m_nSelectedIdx >= m_nListCount ) {
		m_nSelectedIdx -= m_nListCount;
		m_nStartIdx += m_nListCount;
		//showbackground( ID_BG_PHOTO );
		OnPaint( rcClient );
	}
	else {
		UpdateList( prev, m_nSelectedIdx );
	}
	
	return true;
}

void CIVTileViewer::OnKeypressed( const KeyEvent& evt )
{
	int prev = m_nSelectedIdx;
	bool touch = true;
	switch( evt.value ) {
	case VK_UP:
		m_nSelectedIdx-= COLS;
		if( MoveUP( prev ) == false ) {
			// ROTATE
			int last;
			last = (m_uzList.Count()+COLS)/COLS;
			last = last*COLS + m_nSelectedIdx;
			if( last >= m_uzList.Count() ) last -= COLS;
			m_nStartIdx = (last / m_nListCount)*m_nListCount;
			last %= m_nListCount;
			m_nSelectedIdx = last;
			OnPaint( rcClient );
		}
		break;
	case VK_LEFT:
		m_nSelectedIdx--;
		if( MoveUP( prev ) == false ) {
			// ROTATE
			m_nStartIdx = (m_uzList.Count()/m_nListCount)*m_nListCount;
			// 20ÀÇ ¹è¼öÀÎ °æ¿ì
			if( m_nStartIdx == m_uzList.Count() ) m_nStartIdx -= m_nListCount;
			m_nSelectedIdx = m_uzList.Count()%m_nListCount-1;
			if( m_nSelectedIdx == -1 ) m_nSelectedIdx += m_nListCount;
			OnPaint( rcClient );
		}
		break;
	case VK_DOWN:
		//check boundary
		m_nSelectedIdx+=COLS;
		if( MoveDown( prev ) == false ) {
			m_nStartIdx = 0;
			m_nSelectedIdx %= COLS;
			OnPaint( rcClient );
		}
		break;
	case VK_RIGHT:
		m_nSelectedIdx++;
		if( MoveDown( prev ) == false ) {
			// ROTATE
			m_nStartIdx = 0;
			m_nSelectedIdx = 0;
			OnPaint( rcClient );
		}
		break;
	default:
		touch = false;
	}

	if( touch ) {
		DrawGuide();
		Flush();
		return;
	}
	
	CIVListView::OnKeypressed( evt );
}

void CIVTileViewer::OnMouseEvent( const MouseEvent& evt )
{
	KeyEvent kevt = { 0, 0 };

	if( evt.click ) {
		kevt.value = VK_ENTER;
		kevt.pressed = 1;
	}
	else {
		if( evt.x > 0 ) kevt.value = VK_RIGHT;
		else if( evt.x < 0  ) kevt.value = VK_LEFT;
	}
	if( kevt.value ) OnKeypressed( kevt );
}
	
void CIVTileViewer::OnPaint( const CRect& rc )
{
    // draw background 
    int x = rcABS.x1;
    int y = rcABS.y1;
    int h = 4*THUMB_HEIGHT+3*GAP+2*BORDER_WIDTH;
    int w = 5*THUMB_WIDTH+4*GAP+2*BORDER_WIDTH;
    g_SysPixmap.Fill( x, y, w, h, _LEVELMETER_BG );
    g_SysPixmap.Fill( x+BORDER_WIDTH, y+BORDER_WIDTH, w-12, h-12, _BLACK );
    // clear left-top 
    g_SysPixmap.Fill( x, y, 4, 1, 0 );
    g_SysPixmap.Fill( x, y, 1, 4, 0 );
    g_SysPixmap.Fill( x+1, y+1, 1, 1, 0 );
    // clear left-bottom
    g_SysPixmap.Fill( x, y+h-1, 4, 1, 0 );
    g_SysPixmap.Fill( x, y+h-4, 1, 4, 0 );
    g_SysPixmap.Fill( x+1, y+h-2, 1, 1, 0 );
    // clear right-top
    g_SysPixmap.Fill( x+w-4, y, 4, 1, 0 );
    g_SysPixmap.Fill( x+w-1, y, 1, 4, 0 );
    g_SysPixmap.Fill( x+w-2, y+1, 1, 1, 0 );
    // clear right-bottom
    g_SysPixmap.Fill( x+w-4, y+h-1, 4, 1, 0 );
    g_SysPixmap.Fill( x+w-1, y+h-4, 1, 4, 0 );
    g_SysPixmap.Fill( x+w-2, y+h-2, 1, 1, 0 );

    x += BORDER_WIDTH;
    y += BORDER_WIDTH;
    int i;
    for( i=0; i<4; i++ ) {
	x += THUMB_WIDTH;
       g_SysPixmap.Fill( x, y, GAP, h-2*BORDER_WIDTH, _LEVELMETER_BG ); 
	x += GAP;
    }
    x = rcABS.x1+BORDER_WIDTH;
    for( i=0; i<3; i++ ) {
	y += THUMB_HEIGHT;
        g_SysPixmap.Fill( x, y, w-2*BORDER_WIDTH, GAP, _LEVELMETER_BG );
        y += GAP;
    }

    // draw guide
    DrawGuide();
	
    m_norestore = true;
    ResetScrollBar();
    CIVListView::OnPaint( rc );
    m_norestore = false;
}

void CIVTileViewer::ResetScrollBar( void )
{
	int start = m_nStartIdx/VIEW_ITEMS;
	int total = (m_uzList.Count()+VIEW_ITEMS-1)/VIEW_ITEMS;
	//int total = (m_uzList.Count() + VIEW_ITEMS)/VIEW_ITEMS;
	m_pScrollBar->SetViewCount( 1 );
	m_pScrollBar->SetStart( start  );
	m_pScrollBar->SetTotal( total  );
}

#define GUIDE_BG_HEIGHT 22
#define BTN_X_SHIFT_VALUE 9
#define BTN_Y_SHIFT_VALUE 5
#define BTN_Y (240-GUIDE_BG_HEIGHT+BTN_Y_SHIFT_VALUE)
void CIVTileViewer::DrawGuide( void )
{
	CListItem* item = GetSelectedItem();
	int size;
	
	if( !item ) return;
	
	extern RMuint32 jpeg_fclose( RMuint32 handle, void* pContext );
	extern RMuint32 jpeg_fopen( RMuint8* filename, void* pContext );
	extern RMuint32 jpeg_fread (RMuint32 handle, void *buf, RMuint32 length, void *context);

	// draw panel
	g_SysPixmap.Fill( 
		0, 240-GUIDE_BG_HEIGHT, 
		320, GUIDE_BG_HEIGHT, _CP_BASE_COLOR_PALLETTE_INDEX );
	// draw filename
	size = ustrlen( (uchar_t*)item->m_szText );
	if( size > 27 ) { // ±ä¹®ÀÚ´Â Àß¶ó¼­ Ç¥½Ã
		int x = BTN_X_SHIFT_VALUE;
		int i;
		uchar_t* p = ( uchar_t* )item->m_szText;
		for( i=0; i<24; i++ ) {
			x += pT->DrawCharacter( p[i], x, BTN_Y+2, _WHITE, 12, DT_TRANSPARENCY );
		}
		for( i=0; i<3; i++ ) {
			x += pT->DrawCharacter( '.', x, BTN_Y+2, _WHITE, 12, DT_TRANSPARENCY );
		}
	}
	else {
		pT->DrawUC16Text( (uchar_t*)item->m_szText,  BTN_X_SHIFT_VALUE, BTN_Y+2,
			_WHITE, 12, DT_TRANSPARENCY );
	}
	
	if( item->m_nType == NODE_TYPE_FOLDER ) return;
}

void CIVTileViewer::DrawBackground( void )
{
	g_SysPixmap.Fill( rcABS.x2-12, rcABS.y1, 12, rcABS.GetHeight(), m_nBackgroundColor );
}
