/*
 * image.h
 *
 * Copyright(C) 2004 New Media Life Inc.
 *
 * Image Process Methods
 *
 * $Author: zstein $: Lee Seok<zstein@newmedialife.com>
 * Data: 12/15/2004
 * Update:
 * History:
 */
#ifndef __IMAGE_PROCESS_H__
#define __IMAGE_PROCESS_H__

#define ROTATE_90		1
#define ROTATE_180		2
#define ROTATE_270		3

typedef struct {
	char m_szMagicKey[4]; /* 'Y', 'U', 'V', '\0' */
	int m_nWidth;
	int m_nHeight;
	int m_nFileSize;
} YUV420Raw;

typedef void( *PFN_Progress )( int percent, void* ctx );

extern void ReduceSizeYUV420( unsigned char* pY, unsigned char* pUV, int w, int h,
					  unsigned char* pDY, unsigned char* pDUV, int sw, int sh, PFN_Progress progress );
extern int LoadImageYUV420( const char* filename, unsigned char* pY, unsigned char* pUV, int ylen, int uvlen,
					int* w, int* h, PFN_Progress progress );
extern void RotateYUV420( unsigned char* pSrcY, unsigned char* pSrcUV, int sw, int sh,
		unsigned char* pDstY, unsigned char* pDstUV, int dw, int dh, int rotate, PFN_Progress progress );

/* YUV420Raw operations */
extern int LoadYUV420Raw( const char* filename, YUV420Raw* header, unsigned char* pY, unsigned char* pUV, PFN_Progress progress );
extern int SaveYUV420Raw( const char* filename, unsigned char* pY, unsigned char* pUV, int w, int h, PFN_Progress progress );
#endif /* __IMAGE_PROCESS_H__ */
