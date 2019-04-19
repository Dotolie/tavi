#include <stdlib.h>
#include <mpegdec/em85xx.h>

#include "fbdev.h"
#include "Pixmap.h"

extern MpegDecoder* pMpegDecoder;

static FrameBuffer _frameBuffer;
CPixmap g_SysPixmap;

void op_InitFramebuffer( void )
{
	OSDBuffer framebuffer;

	framebuffer = pMpegDecoder->GetOSDFrameBuffer();
	_frameBuffer.m_nBpp = framebuffer.bpp;
	_frameBuffer.m_nHeight = 240;
	_frameBuffer.m_nWidth = 320;
	_frameBuffer.m_pFramebuffer = framebuffer.framebuffer+8+1024;

	g_SysPixmap.CreatePixmap( _frameBuffer.m_nWidth, _frameBuffer.m_nHeight, 
		_frameBuffer.m_nBpp, _frameBuffer.m_pFramebuffer );
}

void op_DrawOsd( unsigned char* buf, int x, int y, int w, int h )
{
	pMpegDecoder->RenderOsdBits_WithoutRefresh( buf, x, y, w, h );
}

void op_DrawYUV420( unsigned char* pY, unsigned char* pUV, int x, int y, int w, int h )
{
	int sw, sh, n, d;


	pMpegDecoder->GetScreenDimensions( (RMint32*)&sw, (RMint32*)&sh, 
										(RMint32*)&n, (RMint32*)&d );
	if( w > sw ) sw = w;
	if( h > sh ) sh = h;

	pMpegDecoder->DisplayYUV420( pY, pUV, x, y, w, h, 320, 240 );
}

void op_DrawText( char* str, int len, int color, int x, int y, int w, int h )
{
	pMpegDecoder->RenderText_WithoutRefresh( str, color, x, y, w, h );
}

void op_DrawText16( char* str, int len, int color, int x, int y, int w, int h )
{
	op_DrawText( str, len, color, x, y, w, h );
}

void op_DrawOsdBox( int color, int x, int y, int w, int h )
{
	pMpegDecoder->RenderOsdBox_WithoutRefresh( color, x, y, w, h );
}

void op_ClearArea( int x, int y, int w, int h )
{
}

void op_ClearAreaOSD( int x, int y, int w, int h )
{
	//pMpegDecoder->ClearAreaOSD( x, y, w, h );
	g_SysPixmap.Fill( x, y, w, h, 0 );
}

void op_SetPixel( int color, int x, int y )
{
	pMpegDecoder->SetPixel( color, x, y );
}

void op_Flush( void )
{
	pMpegDecoder->RefreshOsd();
}

void op_BitBlt( CPixmap* pDst, int dx, int dy, CPixmap* pSrc, int sx, int sy, int w, int h )
{
	int dw;
	int dh;
	int dstride, sstride;
	int i;
	int cw;
	char* pd, *ps;

	if( pDst == NULL || pSrc == NULL ) return;
	if( pDst->m_nBpp != pSrc->m_nBpp ) return;
	
	pd = pDst->m_pBuf;
	ps = pSrc->m_pBuf;
	cw = pDst->m_nBpp/8;
	dw = pDst->m_nWidth - dx;
	dh = pDst->m_nHeight - dy;
	// clip area
	if( dw < w ) w = dw;
	if( dh < h ) h = dh;

	dstride = pDst->m_nWidth*pDst->m_nBpp/8;
	sstride = pSrc->m_nWidth*pSrc->m_nBpp/8;
	pd += dx*cw + dy*dstride;
	ps += sx*cw + sy*sstride;
	for( i=0; i<h; i++ ) {
		memcpy( pd, ps, w*cw );
		pd += dstride;
		ps += sstride ;
	}
}

void op_BitBlt_SkipTr( CPixmap* pDst, int dx, int dy, CPixmap* pSrc, int sx, int sy, int w, int h, RMuint8 FixedBackgroundColor  )
{
	int dw;
	int dh;
	int dstride, sstride;
	int i,j;
	int cw;
	char* pd, *ps;

	if( pDst == NULL || pSrc == NULL ) return;
	if( pDst->m_nBpp != pSrc->m_nBpp ) return;
	
	pd = pDst->m_pBuf;
	ps = pSrc->m_pBuf;
	cw = pDst->m_nBpp/8;
	dw = pDst->m_nWidth - dx;
	dh = pDst->m_nHeight - dy;
	// clip area
	if( dw < w ) w = dw;
	if( dh < h ) h = dh;

	dstride = pDst->m_nWidth*pDst->m_nBpp/8;
	sstride = pSrc->m_nWidth*pSrc->m_nBpp/8;
	pd += dx*cw + dy*dstride;
	ps += sx*cw + sy*sstride;

	for( i=0; i<h; i++ ) {
		for( j=0; j<w; j++ ) {
			if(ps[j]) 
				pd[j] = ps[j];
			else
				pd[j] = FixedBackgroundColor;
		}
		pd += dstride;
		ps += sstride;
	}
}


FrameBuffer* op_GetFrameBuffer( void )
{
	return &_frameBuffer;
}

