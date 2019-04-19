#ifndef _JPEG_H
#define _JPEG_H

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdio.h>		// size_t
#include <memory.h>		// memset
#include <setjmp.h>		// error handling in jpeg library
#define YYYY_UV		1	// to allow jpeg_read_image_420
#include "jpeglib.h"

#define JPEG_ERROR_PROGRESSIVE					-0x10
#define JPEG_ERROR_NOTSUPPORTED_COLORSPACE		-0x20
#define JPEG_ERROR_DECODE							-0x30

typedef int (*JPEG_fopen) (char *filename, void *context);
typedef int (*JPEG_fread) (int handle, void *buf, int length, void *context);
typedef int (*JPEG_fclose)(int handle, void *context);
typedef void (*JPEG_progress)(int percent_done, void *context);

typedef struct tagJPEG_CALLBACK_TABLE
{
	void *context;
	JPEG_fopen fopen;
	JPEG_fread fread;
	JPEG_fclose fclose;
	JPEG_progress jpeg_progress;
} JPEG_CALLBACK_TABLE;

int decode_jpeg_image_420 (char *filename, unsigned char *pY, unsigned char *pUV, unsigned long lengthY, unsigned long lengthUV, int *outW, int *outH, JPEG_CALLBACK_TABLE *CallbackTable);
char *jpeg_get_last_error (void);

typedef struct tagJPEG_DECODER
{
	unsigned char *pYUV;	// 4:4:4 [Y][U][V][Y][U][V] etc ...
	unsigned long length;
	int outW; 
	int outH;
	struct jpeg_decompress_struct cinfo;
	JPEG_CALLBACK_TABLE CallbackTable;
	int handle;
	int initialized;
} JPEG_DECODER;

extern int jpeg_real_width;
extern int jpeg_real_height;

#ifdef __cplusplus
}
#endif

#endif
