//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : CPixmap class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/Pixmap.cpp,v $

// $Revision: 1.2 $

// $Date: 2006/06/07 08:22:52 $

// $Author: zstein $

//

//******************************************************************************

// $Log: Pixmap.cpp,v $
// Revision 1.2  2006/06/07 08:22:52  zstein
// - fixed ui bug.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/06/21 06:08:29  zstein
// - bug fixed.
//
// Revision 1.5  2005/05/15 11:08:49  zstein
// *** empty log message ***
//
// Revision 1.4  2005/04/23 06:08:07  zstein
// added m-language
//
// Revision 1.3  2005/02/25 01:13:09  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/22 15:14:29  zstein
// CPixmap::CreatePixmap(): fixed negative width or height bug.
//
// Revision 1.1  2005/02/16 16:03:25  zstein
// *** empty log message ***
//

//*******************************************************************************
#include <stdio.h>
#include <string.h>
#include "Pixmap.h"
#include "fbdev.h"

CPixmap::CPixmap( void ) :
	m_nBpp( 0 ),
	m_nWidth( 0 ),
	m_nHeight( 0 ),
	m_pBuf( 0 ),
	m_nStride( 0 )
{
}

CPixmap::CPixmap( const CPixmap& pixmap )
{
	m_nBpp = pixmap.m_nBpp;
	m_nWidth = pixmap.m_nWidth;
	m_nHeight = pixmap.m_nHeight;
	if( pixmap.m_pBuf == 0 ) return;
	m_nStride = m_nWidth*m_nBpp/8;
	m_pBuf = new char[m_nStride*m_nHeight];
	m_fAutoDelete = true;
	memcpy( m_pBuf, pixmap.m_pBuf, m_nStride*m_nHeight );
}


/* NOTE: DON'T DELETE pImageBuf after call CPixmap( w, h, bpp, pImageBuf ) or 
 * CPixmap( size, bpp, pImageBuf ). This occurs system crash
 */
CPixmap::CPixmap( int w, int h, int bpp, char* pImageBuf ) :
	m_nBpp( bpp ),
	m_nWidth( w ),
	m_nHeight( h ),
	m_pBuf( pImageBuf ),
	m_fAutoDelete( true )
{
	m_nStride = m_nWidth*m_nHeight/8;
	if( pImageBuf == NULL ) {
		m_pBuf = new char[w*h];
	}
}

CPixmap::CPixmap( const CSize& size, int bpp, char* pImageBuf )
{
	m_nBpp = bpp;
	m_nWidth = size.GetWidth();
	m_nHeight = size.GetHeight();
	m_nStride = m_nWidth*m_nHeight/8;
	m_pBuf = pImageBuf;
	m_fAutoDelete = true;
	if( m_pBuf == NULL ) {
		m_pBuf = new char[size.GetWidth()*size.GetHeight()];
	}
}

CPixmap::~CPixmap( void )
{
	if( m_fAutoDelete && m_pBuf ) {
		delete[] m_pBuf;
	}
}

/*
 * CreatePixmap( int w, int h, int bpp, char* pImageBuf, bool fDelete )
 * @w: width of pixmap
 * @h: height of pixmap
 * @bpp: bits per pixel.(8, 16, 32 )
 * @pImageBuf: temporary image buffer to be allocated. If pImageBuf is 0 this function
 *                     will allocate memory to pImagebuf.
 * @fDelete: if true destructor wil delete pImagebuf, false destructor won't delete pImageBuf
 */
bool CPixmap::CreatePixmap( int w, int h, int bpp, char* pImageBuf, bool fDelete )
{
	if( w < 0 ) w = 0;
	if( h < 0 ) h = 0;
	
	m_nBpp = bpp;
	m_nWidth = w;
	m_nHeight = h;
	m_nStride = w*bpp/8;
	if( pImageBuf == 0 ) {
		if( m_pBuf ) delete[] m_pBuf;
		m_pBuf = new char[m_nStride*h];
		if( m_pBuf == 0 ) return false;
	}
	else {
		m_pBuf = pImageBuf;
	}

	m_fAutoDelete = fDelete;

	return true;
}
	
char* CPixmap::GetImageBuffer( void )
{
	return m_pBuf;
}

CSize CPixmap::GetSize( void )
{
	return CSize( m_nWidth, m_nHeight );
}

int CPixmap::GetStride( void )
{
	return m_nStride;
}

void CPixmap::Fill( int x, int y, int w, int h, int color )
{
	char* p = m_pBuf;
	int cw = m_nBpp/8;
	int stride = cw*m_nWidth;

	p += y*m_nWidth+x;
	for( int i=0; i<h; i++ ) {
		memset( p, color, w );
		p += m_nWidth;
	}
}

void CPixmap::SetPixel( unsigned int x, unsigned int y, int color )
{
	if( x >= m_nWidth || y >= m_nHeight ) return;
	m_pBuf[ (y*m_nWidth) + x ] = color;
}

void CPixmap::Copy( CPixmap* pixmap )
{
	if( m_pBuf ) delete[] m_pBuf;

	int size = pixmap->m_nWidth*pixmap->m_nHeight*pixmap->m_nBpp/8;
	m_pBuf = new char[size];
	memcpy( m_pBuf, pixmap->m_pBuf, size );
	m_nWidth = pixmap->m_nWidth;
	m_nHeight = pixmap->m_nHeight;
	m_nBpp = pixmap->m_nBpp;
	m_nStride = pixmap->m_nStride;
	m_fAutoDelete = true;
}

void CPixmap::Not( int x, int y, int w, int h )
{
	register char pixel;
	
	// Not support 16bit, 24bit and 32bit pixel. only 8-bit pixel
	for( int i=0; i<h; i++ ) {
		for( int j=0; j<w; j++ ) {
			pixel = m_pBuf[(i+y)*m_nWidth+x+j];
			pixel = !pixel;
			m_pBuf[(i+y)*m_nWidth+x+j] = pixel;
		}
	}
}

void CPixmap::Print( void )
{
	printf( "m_nWidth: %d, m_nHeight: %d\n", m_nWidth, m_nHeight );
	for( int i=0; i<m_nHeight; i++ ) {
		for( int j=0; j<m_nWidth; j++ ) {
			printf( "%d", m_pBuf[(i*m_nWidth)+j] );
		}
		printf( "\n" );
	}
	printf( "\n" );
}
