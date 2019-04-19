//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Image Viewer Preview List Viewer
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ImageViewer/IVPreView.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: IVPreView.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.20  2006/01/24 00:53:40  zstein
// *** empty log message ***
//
// Revision 1.19  2005/12/20 03:02:27  zstein
// *** empty log message ***
//
// Revision 1.18  2005/12/02 13:40:47  zstein
// *** empty log message ***
//
// Revision 1.17  2005/11/25 12:47:22  zstein
// *** empty log message ***
//
// Revision 1.16  2005/11/25 10:22:17  zstein
// *** empty log message ***
//
// Revision 1.15  2005/11/08 07:25:54  zstein
// *** empty log message ***
//
// Revision 1.14  2005/11/08 01:18:53  zstein
// *** empty log message ***
//
//******************************************************************************

#include "IVPreView.h"
#include <Lib/LibImage.h>
#include <Component/image.h>
#include <Component/FileUtil.h>
#include <Component/Bitmap.h>
#include <Task/tavi_global.h>
#include "ImageViewer.h"

// Macros
#define BAR_WIDTH				284
#define BAR_HEIGHT				45
#define BAR_COLOR				27
#define BAR_BORDER_COLOR		36
#define PICTURE_FRAME_COLOR	17
#define PREVIEW_WIDTH			56
#define PREVIEW_HEIGHT			40
#define BAR_TEXT_OFFSET		64
 #define TEXT_SCROLL_DELAY		300
 #define TEXT_X_OFFSET			BAR_TEXT_OFFSET+4


#define PREVIEW_ICON_COUNT	8

// Extern variables
extern RMuint32* thumbnailY;
extern RMuint32* thumbnailUV;
extern RMuint32* backgroundY;
extern RMuint32* backgroundUV;
extern CTextEngine_BDF* pT;

// Local variables

CIVPreViewList::CIVPreViewList( CWindow* parent, const CString& szPath, int nSort, CStack<FolderPrev>* pStack ) :
	CIVListView( parent, szPath, nSort, 4, pStack )
{
	m_puzBarPixmap = new CPixmap;
	m_puzBarPixmap->CreatePixmap( BAR_WIDTH, BAR_HEIGHT, 8,  NULL, true );
	// Draw Bar
	m_puzBarPixmap->Fill( 0, 0, BAR_TEXT_OFFSET, BAR_HEIGHT, COLOR_TRANSPARENT );
	m_puzBarPixmap->Fill( BAR_TEXT_OFFSET, 0, 220, BAR_HEIGHT, BAR_COLOR );
	// Draw border
	m_puzBarPixmap->Fill( 0, 0, BAR_WIDTH, 1, BAR_BORDER_COLOR );
	m_puzBarPixmap->Fill( 0, 0, 1, BAR_HEIGHT, BAR_BORDER_COLOR );
	m_puzBarPixmap->Fill( 0, BAR_HEIGHT-1, BAR_WIDTH, 1, BAR_BORDER_COLOR );
	m_puzBarPixmap->Fill( BAR_WIDTH-1, 0, 1, BAR_HEIGHT, BAR_BORDER_COLOR );
	// Draw a picture frame
	m_puzBarPixmap->Fill( 1, 1, BAR_TEXT_OFFSET-1, 3, PICTURE_FRAME_COLOR ); // top
	m_puzBarPixmap->Fill( 1, 1, 5, BAR_HEIGHT-2, PICTURE_FRAME_COLOR ); // left
	m_puzBarPixmap->Fill( 1, BAR_HEIGHT-5, BAR_TEXT_OFFSET-1, 4, PICTURE_FRAME_COLOR ); // bottom
	m_puzBarPixmap->Fill( BAR_TEXT_OFFSET-4, 1, 4, BAR_HEIGHT-2, PICTURE_FRAME_COLOR ); // right
	m_puzBarPixmap->Fill( 6, 4, 1, 1, PICTURE_FRAME_COLOR );
	m_puzBarPixmap->Fill( 7, 4, 52, 1, COLOR_BLACK );
	m_puzBarPixmap->Fill( 59, 4, 1, 1, PICTURE_FRAME_COLOR );
	m_puzBarPixmap->Fill( 6, 49, 1, 1, PICTURE_FRAME_COLOR );
	m_puzBarPixmap->Fill( 59, 49, 1, 1, PICTURE_FRAME_COLOR );
	m_puzBarPixmap->Fill( 6, 5, 1, 36, COLOR_BLACK );
	// Draw edge
	// left-top
	m_puzBarPixmap->Fill( 0, 2, 1, 1, 26 );
	m_puzBarPixmap->Fill( 0, 3, 1, 1, 25 );
	m_puzBarPixmap->Fill( 1, 1, 1, 1, 25 );
	m_puzBarPixmap->Fill( 1, 2, 1, 1, 25 );
	m_puzBarPixmap->Fill( 1, 3, 1, 1, 19 );
	m_puzBarPixmap->Fill( 1, 4, 1, 1, 18 );
	m_puzBarPixmap->Fill( 2, 0, 1, 1, 26 );
	m_puzBarPixmap->Fill( 2, 1, 1, 1, 25 );
	m_puzBarPixmap->Fill( 2, 2, 1, 1, 18 );
	m_puzBarPixmap->Fill( 3, 0, 1, 1, 25 );
	m_puzBarPixmap->Fill( 3, 1, 1, 1, 19 );
	m_puzBarPixmap->Fill( 4, 1, 1, 1, 18 );
	m_puzBarPixmap->Fill( 0, 0, 2, 1, COLOR_TRANSPARENT );
	m_puzBarPixmap->Fill( 0, 1, 1, 1, COLOR_TRANSPARENT );

	// left-bottom
	m_puzBarPixmap->Fill( 0, 42, 1, 1, 26 );
	m_puzBarPixmap->Fill( 0, 41, 1, 1, 25 );
	m_puzBarPixmap->Fill( 1, 43, 1, 1, 25 );
	m_puzBarPixmap->Fill( 1, 42, 1, 1, 25 );
	m_puzBarPixmap->Fill( 1, 41, 1, 1, 19 );
	m_puzBarPixmap->Fill( 1, 40, 1, 1, 18 );
	m_puzBarPixmap->Fill( 2, 44, 1, 1, 26 );
	m_puzBarPixmap->Fill( 2, 43, 1, 1, 25 );
	m_puzBarPixmap->Fill( 2, 42, 1, 1, 18 );
	m_puzBarPixmap->Fill( 3, 44, 1, 1, 25 );
	m_puzBarPixmap->Fill( 3, 43, 1, 1, 19 );
	m_puzBarPixmap->Fill( 4, 43, 1, 1, 18 );
	m_puzBarPixmap->Fill( 0, 43, 1, 2, COLOR_TRANSPARENT );
	m_puzBarPixmap->Fill( 1, 44, 1, 1, COLOR_TRANSPARENT );

	// right-top
	m_puzBarPixmap->Fill( 280, 0, 1, 1, 25 );
	m_puzBarPixmap->Fill( 280, 1, 1, 1, 28 );
	m_puzBarPixmap->Fill( 281, 0, 1, 1, 26 );
	m_puzBarPixmap->Fill( 281, 1, 1, 1, 25 );
	m_puzBarPixmap->Fill( 282, 1, 1, 1, 25 );
	m_puzBarPixmap->Fill( 282, 2, 1, 1, 25 );
	m_puzBarPixmap->Fill( 282, 3, 1, 1, 28 );
	m_puzBarPixmap->Fill( 283, 2, 1, 1, 26 );
	m_puzBarPixmap->Fill( 283, 3, 1, 1, 25 );
	m_puzBarPixmap->Fill( 282, 0, 2, 1, COLOR_TRANSPARENT );
	m_puzBarPixmap->Fill( 283, 1, 1, 1, COLOR_TRANSPARENT );

	// right-bottom
	m_puzBarPixmap->Fill( 283, 41, 1, 1, 25 );
	m_puzBarPixmap->Fill( 283, 42, 1, 1, 26 );
	m_puzBarPixmap->Fill( 282, 41, 1, 1, 28 );
	m_puzBarPixmap->Fill( 282, 42, 1, 1, 25 );
	m_puzBarPixmap->Fill( 282, 43, 1, 1, 25 );
	m_puzBarPixmap->Fill( 281, 43, 1, 1, 25 );
	m_puzBarPixmap->Fill( 281, 44, 1, 1, 26 );
	m_puzBarPixmap->Fill( 280, 43, 1, 1, 28 );
	m_puzBarPixmap->Fill( 280, 44, 1, 1, 25 );
	m_puzBarPixmap->Fill( 282, 44, 2, 1, COLOR_TRANSPARENT );
	m_puzBarPixmap->Fill( 283, 43, 1, 1, COLOR_TRANSPARENT );

	#if 0
	// allocate preview buffer
	struct ImageNode* pNode;
	for( int i=0; i<m_nListCount; i++ ) {
		pNode = new struct ImageNode;
		pNode->pY = ( unsigned char* )thumbnailY+(PREVIEW_WIDTH*PREVIEW_HEIGHT)*i;
		pNode->pUV = ( unsigned char* )thumbnailUV+(PREVIEW_WIDTH*PREVIEW_HEIGHT)*i;
		pNode->success = false;
		m_uzImageList.Add( pNode );
	}
        #else
        m_pY = ( unsigned char* )thumbnailY;
        m_pUV = ( unsigned char* )thumbnailUV;
        #endif

	int size = PREVIEW_WIDTH*PREVIEW_HEIGHT;
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
	for( i=0; i<PREVIEW_HEIGHT; i++ ) {
		memcpy( dy, sy, PREVIEW_WIDTH );
		memcpy( duv, suv, PREVIEW_WIDTH/2 );
		sy += 64;
		dy += PREVIEW_WIDTH;
		suv += 32;
		duv += PREVIEW_WIDTH/2;
	}
	str2ustr( path, jpgpath );
	LoadImageYUV420( (char*)path, 
		buf_Y, buf_UV,
		size, size/2, &iw, &ih, NULL );
	sy = buf_Y;
	suv = buf_UV;
	dy = m_JPG_Y;
	duv = m_JPG_UV;
	for( i=0; i<PREVIEW_HEIGHT; i++ ) {
		memcpy( dy, sy, PREVIEW_WIDTH );
		memcpy( duv, suv, PREVIEW_WIDTH/2 );
		sy += 64;
		dy += PREVIEW_WIDTH;
		suv += 32;
		duv += PREVIEW_WIDTH/2;
	}
	str2ustr( path, bmppath );
	LoadImageYUV420( (char*)path, 
		buf_Y, buf_UV,
		size, size/2, &iw, &ih, NULL );
	sy = buf_Y;
	suv = buf_UV;
	dy = m_BMP_Y;
	duv = m_BMP_UV;
	for( i=0; i<PREVIEW_HEIGHT; i++ ) {
		memcpy( dy, sy, PREVIEW_WIDTH );
		memcpy( duv, suv, PREVIEW_WIDTH/2 );
		sy += 64;
		dy += PREVIEW_WIDTH;
		suv += 32;
		duv += PREVIEW_WIDTH/2;
	}

	delete[] buf_Y;
	delete[] buf_UV;
}

CIVPreViewList::~CIVPreViewList( void )
{
	// free pixmap
	delete[] m_Folder_Y;
	delete[] m_Folder_UV;
	delete[] m_BMP_Y;
	delete[] m_BMP_UV;
	delete[] m_JPG_Y;
	delete[] m_JPG_UV;
}

void CIVPreViewList::DrawItem( CListItem* puzItem, int idx, bool fSelect )
{
	int color;
	int nOutW, nOutH;
	CFolderListItem* pFolderItem = ( CFolderListItem* )puzItem;
	int nSpacing=0;
	PFN_Progress prg;

    #if 0
	if( m_fUpdate ) {
 		CNode< struct ImageNode >* pNode;
		if( m_nUpdateDir == 0 ) { // up
			if( (m_nSelectedIdx+m_nStartIdx+1) == m_uzList.Count() ) {
				pFolderItem->m_pY = NULL;
				pFolderItem->m_pUV = NULL;
			}
			else {
	 			if( idx == 0 ) {
	 				pNode = m_uzImageList.DetachTail();
					m_uzImageList.AddNodeToHead( pNode );
					pFolderItem->m_pY = NULL;
					pFolderItem->m_pUV = NULL;
				}
			}
		}
		else {
			if( m_nSelectedIdx == 0 ) {
				pFolderItem->m_pY = NULL;
				pFolderItem->m_pUV = NULL;
			}
			else {
	 			if( idx == m_nListCount-1 ) {
					pNode = m_uzImageList.DetachHead();
					m_uzImageList.AddNodeToTail( pNode );
					pFolderItem->m_pY = NULL;
					pFolderItem->m_pUV = NULL;
				}
			}
		}
 	}
    #endif


	// Load thumbnail
	if( pFolderItem->m_pY == NULL && pFolderItem->m_nType == NODE_TYPE_FILE ) {
		uchar_t path[MAX_PATH];
		uchar_t del[] = { '/', 0 };
		bool success;

		//memset( m_uzImageList[idx]->pY, 0, THUMB_M_YSIZE );
		//memset( m_uzImageList[idx]->pUV, 0x80, THUMB_M_UVSIZE );
		memset( m_pY, 0, THUMB_M_YSIZE );
            memset( m_pUV, 0x80, THUMB_M_UVSIZE );
		//pFolderItem->m_pY = m_uzImageList[idx]->pY;
		//pFolderItem->m_pUV = m_uzImageList[idx]->pUV;
		ustrcpy( path, (uchar_t*)m_szCurrentPath );
		ustrcat( path, del );
		ustrcat( path, (uchar_t*)pFolderItem->m_szText );
		//success = g_p_ThumbDB->FetchThumb( pFolderItem->m_szText, 
		//	(char*)pFolderItem->m_pY, (char*)pFolderItem->m_pUV );
		//success = g_p_ThumbDB->FetchThumb( path, 
		//	(char*)pFolderItem->m_pY, (char*)pFolderItem->m_pUV );
		success = g_p_ThumbDB->FetchThumb( path, 
			(char*)m_pY, (char*)m_pUV );
		pFolderItem->m_fSuccess = success;
		if( success == false ) {
			printf( "failed to fetch\n" );
		}
 	}
	
	if( fSelect ) color = _BLACK;
	else color = _WHITE;
	CRect rc = m_pScrollBar->GetClientRect();
	//nSpacing = (rcClient.GetHeight() - m_nListCount*m_puzBarPixmap->m_nHeight)/(m_nListCount-1);
	nSpacing = 1;

	// clear thumbnails
	int x, y;
        #if 0
	if( idx == 0 && m_fUpdate ) {
		int size = PREVIEW_WIDTH*PREVIEW_HEIGHT;
		unsigned char* pY = new unsigned char[size];
		unsigned char* pUV = new unsigned char[size];
		memset( (void*)pY, 0x00, size );
		memset( (void*)pUV, 0x80, size );
		for( int i=0; i<m_nListCount && m_uzList.Count() >= m_nListCount; i++ ) {
			x = rcABS.x1+4+m_nOffset;
			y = rcABS.y1+4+i*(m_puzBarPixmap->m_nHeight+nSpacing);
			x += (4-x%4);
			y -= y%2;
			//printf( "x: %d, y: %d, y1: %d, height: %d\n", x, y, rcABS.y1, rcABS.GetHeight() );
			op_DrawYUV420( pY, pUV, x, y, PREVIEW_WIDTH, PREVIEW_HEIGHT );
		}
		delete[] pY;
		delete[] pUV;
	}
        #endif

	// Draw selectbar
	y = rcABS.y1+2+idx*(m_puzBarPixmap->m_nHeight+nSpacing);
	if( fSelect ) {
		op_BitBlt( &g_SysPixmap, rcABS.x1+2+m_nOffset, y,
			m_puzBarPixmap, 0, 0, m_puzBarPixmap->m_nWidth, m_puzBarPixmap->m_nHeight );
		op_Flush();
	}
	else {
		g_SysPixmap.Fill( rcABS.x1+2+m_nOffset, y,
			m_puzBarPixmap->m_nWidth, m_puzBarPixmap->m_nHeight, 0 );
	}

	// Draw filename. 
	uchar_t* pwcString = (uchar_t*)puzItem->m_szText;
	int len = puzItem->m_szText.GetSize();
	int nBoundaryLen = pT->GetBoundaryLength( pwcString, len, 12, DT_TRANSPARENCY | DT_SHADOW, 
		m_puzBarPixmap->m_nWidth-86-m_nOffset );
	if( nBoundaryLen < len ) puzItem->m_fScroll = true;
	else puzItem->m_fScroll = false;
	
	x = rcABS.x1+TEXT_X_OFFSET+m_nOffset+2;
	for( int i=0; i<nBoundaryLen; i++ ) {
		if( fSelect ) {
			x += pT->DrawCharacter( pwcString[i], x, rcABS.y1+10+idx*(m_puzBarPixmap->m_nHeight+nSpacing),
				color, 12, DT_TRANSPARENCY );
		}
		else {
			x += pT->DrawCharacter( pwcString[i], x, rcABS.y1+10+idx*(m_puzBarPixmap->m_nHeight+nSpacing),
				color, 12, DT_TRANSPARENCY | DT_SHADOW );
		}
	}
	
	x = rcABS.x1+4+m_nOffset;
	y = rcABS.y1+4+idx*(m_puzBarPixmap->m_nHeight+nSpacing);
	x += (4-x%4);
	y -= y%2;

	// draw thumbnail
	if( puzItem->m_nType == NODE_TYPE_FOLDER ) {
		op_DrawYUV420( m_Folder_Y, m_Folder_UV, 
				x, y, THUMB_M_WIDTH, THUMB_M_HEIGHT );
	}
	else {
		if( pFolderItem->m_fSuccess ) {
			//op_DrawYUV420( pFolderItem->m_pY, pFolderItem->m_pUV, 
			//	x, y, THUMB_M_WIDTH, THUMB_M_HEIGHT );
			op_DrawYUV420( m_pY, m_pUV, 
				x, y, THUMB_M_WIDTH, THUMB_M_HEIGHT );
		}
		else {
			unsigned char* py, *puv;
			if( pFolderItem->m_nMedia == MEDIA_BMP ) {
				py = m_BMP_Y;
				puv = m_BMP_UV;
			}
			else {
				py = m_JPG_Y;
				puv = m_JPG_UV;
			}
			op_DrawYUV420( py, puv, 
				x, y, THUMB_M_WIDTH, THUMB_M_HEIGHT );
		}
	}

	
	
	// If the size of the text is longer than bar size, 
	// caputre image buffer which has all characters.
	if( fSelect ) {
		if( m_puzBarText ) {
			delete m_puzBarText;
			m_puzBarText = NULL;
		}

		if( puzItem->m_fScroll == false ) return;

		CSize size = pT->GetStringSize( pwcString, len, 12 );
		m_puzBarText = new CPixmap;
		m_puzBarText->CreatePixmap( size.width, size.height, 8, NULL, true );
		m_puzBarText->Fill( 0, 0, size.width, size.height, 27 );
		// whole length
		puzItem->m_nWidthInPixel = size.width;
		// length of view
		size.width = 0;
		CSize size2 = pT->GetStringSize( pwcString, nBoundaryLen, 12 );
		puzItem->m_nScrollWidth = size2.width;
		x = 0;
		for( int i=0; i<len; i++ ) {
			x += pT->DrawCharacter( pwcString[i], x, 0, color, 12, DT_TRANSPARENCY, m_puzBarText );
		}

		m_nTextScrollPos = 0;
		m_nStartWaitCnt = 0;
	}
}

#define TEXT_SCROLL_DELAY		2
bool CIVPreViewList::ScrollText( void )
{
	struct timeval tv1, tv2;
	CListItem* puzItem = m_uzList[m_nStartIdx+m_nSelectedIdx];
	if( !puzItem ) return false;

	if( puzItem->m_fScroll == false ) return false;

	gettimeofday( &tv1, NULL );
	if( m_nStartWaitCnt == 0 ) {
		gettimeofday( &tv2, NULL );
		m_nStartWaitCnt = tv2.tv_sec;
	}
	if( (tv1.tv_sec-m_nStartWaitCnt) < TEXT_SCROLL_DELAY ) return true;

	CRect rc = m_pScrollBar->GetClientRect();
	int nSpacing = (rcClient.GetHeight() - m_nListCount*m_puzBarPixmap->m_nHeight)/(m_nListCount-1);
	int x = rcABS.x1+2+m_nOffset+2;
	x += (4-x%4);

	// draw text at start position.
	int w;
	w = m_puzBarText->m_nWidth - m_nTextScrollPos; 
	if( w > puzItem->m_nScrollWidth ) w = puzItem->m_nScrollWidth;
	//if( m_nTextScrollPos >= puzItem->m_nWidthInPixel ) {
	if( w < (puzItem->m_nScrollWidth/2) ) {
		m_nTextScrollPos = 0;
		m_nStartWaitCnt = 0;
		op_BitBlt( &g_SysPixmap, rcABS.x1+2+m_nOffset+TEXT_X_OFFSET, 
			rcABS.y1+2+m_nSelectedIdx*(m_puzBarPixmap->m_nHeight + nSpacing),
			m_puzBarPixmap, 
			TEXT_X_OFFSET, 0, 
			m_puzBarPixmap->m_nWidth-TEXT_X_OFFSET-10, m_puzBarPixmap->m_nHeight );
		op_BitBlt( &g_SysPixmap, 
			 rcABS.x1+TEXT_X_OFFSET+m_nOffset+2, 
			rcABS.y1+10+m_nSelectedIdx*(m_puzBarPixmap->m_nHeight+nSpacing),
			m_puzBarText, m_nTextScrollPos, 0, 
			puzItem->m_nScrollWidth, m_puzBarText->m_nHeight );
		op_Flush();
		return true;
	}
	// DrawItem converts puzItem->m_szText to wide character.

	// clear text area
	op_BitBlt( &g_SysPixmap, 
		rcABS.x1+2+m_nOffset+TEXT_X_OFFSET, 
		rcABS.y1+2+m_nSelectedIdx*(m_puzBarPixmap->m_nHeight + nSpacing),
		m_puzBarPixmap, TEXT_X_OFFSET, 0, 
		m_puzBarPixmap->m_nWidth-TEXT_X_OFFSET-10, 
		m_puzBarPixmap->m_nHeight );


	op_BitBlt( &g_SysPixmap, 
		 rcABS.x1+TEXT_X_OFFSET+m_nOffset+2, 
		rcABS.y1+10+m_nSelectedIdx*(m_puzBarPixmap->m_nHeight+nSpacing),
		m_puzBarText, m_nTextScrollPos, 0, 
		w, m_puzBarText->m_nHeight );
	op_Flush();

	m_nTextScrollPos++;
	return false;

}

void CIVPreViewList::BeforeScan( void )
{
	//m_ThumbDB.UnLoad();
	//m_ThumbDB.Load( m_szCurrentPath );
}

