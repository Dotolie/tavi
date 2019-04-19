/*
 * $Id: fbdev.h,v 1.1.1.1 2006/02/22 12:33:31 zstein Exp $ - fdev.h
 *
 * Copyright(C) 2004 New Media Life Inc.
 *
 * Operators of frame buffer
 *
 * Author: Lee Seok<zstein@newmedialife.com>
 * Date: 12/13/2004
 * Update:
 * History
 */

#ifndef __FBDEV_H__
#define __FBDEV_H__

#include "../mpegdec/mpegdec.h"
#include "object.h"
#include "Pixmap.h"


#define COLOR_BLACK				23
#define COLOR_WHITE			17
#define COLOR_TRANSPARENT		0
#define COLOR_GRAY				20


typedef struct {
	int m_nWidth;
	int m_nHeight;
	int m_nBpp;
	char* m_pFramebuffer;
} FrameBuffer;

extern CPixmap g_SysPixmap;

extern void op_InitFramebuffer( void );
extern void op_DrawOsd( unsigned char* buf, int x, int y, int w, int h );
extern void op_DrawYUV420( unsigned char* pY, unsigned char* pUV, int x, int y, int w, int h );
extern void op_DrawText( char* str, int len, int color, int x, int y, int w, int h );
extern void op_DrawText16( char* str, int len, int color, int x, int y, int w, int h );
extern void op_DrawOsdBox( int color, int x, int y, int w, int h );
extern void op_ClearArea( int x, int y, int w, int h );
extern void op_ClearAreaOSD( int x, int y, int w, int h );
extern void op_SetPixel( int color, int x, int y );
extern void op_Flush( void );

extern void op_BitBlt( CPixmap* pDst, int dx, int dy, CPixmap* pSrc, int sx, int sy, int w, int h );
extern void op_BitBlt_SkipTr( CPixmap* pDst, int dx, int dy, CPixmap* pSrc, int sx, int sy, int w, int h , RMuint8 FixedBackgroundColor  );
extern FrameBuffer* op_GetFrameBuffer( void );


#endif /* __FBDEV_H__ */
