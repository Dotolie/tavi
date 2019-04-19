#include <stdlib.h>
#include <string.h>
#include <rmbasic.h>
#include <linuxfs.h>
#include <jpeg/jpeg.h>
#include "image.h"
#include <fatfs.h>
#include "FileUtil.h"
#include "Bitmap.h"

static char* image_ext[] = {
	"jpg", "gif", "bmp", "png"
};

static void ReduceRAW( unsigned char* pSrc, unsigned char* pDST, 
				int currwidth, int currheight, 
				int sw, int sh, int* outw, int* outh,
				PFN_Progress progress )
{
	int y, x;
	int yp, yw;
	int pixhigh;
	int pixwide, count;
	int lastyp, a, b, tmp, xypos;
	
	if( currwidth%2 ) currwidth--;
	if( currheight%2 ) currheight--;
	if( outw ) *outw = currwidth;
	if( outh ) *outh = currheight;
	
	if (currwidth > sw) {  /* It has been reduced. */
		lastyp  = -1;
		pixhigh = (int)( ((float)currheight) / ((float)sh) + 0.5 );
		pixwide = (int)( ((float)currwidth)  / ((float)sw) + 0.5 );
		pixhigh++;
		pixwide++;
		xypos = 0;
		for(y=0; y < currheight; y++) {
			yp = (y * sh) / currheight;
			if (yp != lastyp) {
				yw = (y * currwidth);
	
				/* we try to resample a bit. get that pentium RSN :) */
				for (x=0; x < currwidth; x++,yw++) {
					tmp = 0;
					count = 0;
					for (b=0; (b < pixhigh) && (y+b < currheight); b++) {
						for (a=0; (a < pixwide) && (x+a < currwidth); a++) {
							tmp += *(pSrc+yw+a+b*currwidth);
							count++;
						}
					}
					xypos = (((x*sw)/currwidth)+yp*sw);
					pDST[xypos] = tmp/count;
				}
				lastyp = yp;
				if( progress ) progress( 0, NULL );
			}
		}
	} 
}

void ReduceSizeYUV420( 
	unsigned char* pY, unsigned char* pUV, int w, int h ,
	unsigned char* pDY, unsigned char* pDUV, int sw, int sh,
	PFN_Progress progress )
{
	unsigned char* pSU, *pSV;
	unsigned char* pDU, *pDV;
	int i, j, uw, uh;

	pSU = ( unsigned char* )malloc( w*h/4 );
	pSV = ( unsigned char* )malloc( w*h/4 );
	pDU = ( unsigned char* )malloc( sw*sh/4 );
	pDV = ( unsigned char* )malloc( sw*sh/4 );

	uw = w/2;
	uh = h/2;
	for( i=0; i<uh; i++ ) {
		for( j=0; j<uw; j++ ) {
			pSU[i*uw+j] = pUV[i*w+2*j];
			pSV[i*uw+j] = pUV[i*w+2*j+1];
		}
	}

	ReduceRAW( pY, 	pDY, w, h, sw, sh, NULL, NULL, progress );
	ReduceRAW( pSU, pDU, w/2, h/2, sw/2, sh/2, NULL, NULL, progress );
	ReduceRAW( pSV, pDV, w/2, h/2, sw/2, sh/2, NULL, NULL, progress );

	uw = sw/2;
	uh = sh/2;
	for( i=0; i<uh; i++ ) {
		for( j=0; j<uw; j++ ) {
			pDUV[i*sw+2*j] = pDU[i*uw+j];
			pDUV[i*sw+2*j+1] = pDV[i*uw+j];
		}
		if( progress ) progress( 0, NULL );
	}
	
	free( pSU );
	free( pSV );
	free( pDU );
	free( pDV );
}		


void RotateYUV420( unsigned char* pSrcY, unsigned char* pSrcUV, int sw, int sh,
		unsigned char* pDstY, unsigned char* pDstUV, int dw, int dh, int rotate,
		PFN_Progress progress )
{
	unsigned char* pSU, *pSV;
	unsigned char* pDU, *pDV;
	int x, y;
	int i, j, uw, uh, hw, hh;
	int yw, yh;

	pSU = ( unsigned char* )malloc( sw*sh/4 );
	pSV = ( unsigned char* )malloc( sw*sh/4 );
	pDU = ( unsigned char* )malloc( dw*dh/4 );
	pDV = ( unsigned char* )malloc( dw*dh/4 );

	uw = sw/2;
	uh = sh/2;
	for( i=0; i<uh; i++ ) {
		for( j=0; j<uw; j++ ) {
			pSU[i*uw+j] = pSrcUV[i*sw+2*j];
			pSV[i*uw+j] = pSrcUV[i*sw+2*j+1];
		}
	}
	
	yw = sw/2;
	yh = sh/2;
	switch( rotate ) {
	case ROTATE_90:
		// Y
		for( y=0; y<sh; y++ ) {
			for( x=0; x<sw; x++ ) {
				*(pDstY+x*sh+(sh-y-1)) = *(pSrcY+y*sw+x);
			}
			if( progress && !(y%10) ) progress( 0, NULL );
		}
		// UV
		for( y=0; y<yh; y++ ) {
			for( x=0; x<yw; x++ ) {
				*(pDU+x*yh+(yh-y-1)) = *(pSU+y*yw+x);
				*(pDV+x*yh+(yh-y-1)) = *(pSV+y*yw+x);
			}
			if( progress && !(y%10) ) progress( 0, NULL );
		}
		break;
	case ROTATE_180:
		// Y
		for( y=0; y<sh; y++ ) {
			for( x=0; x<sw; x++ ) {
				*(pDstY+(sh-y-1)*sw+(sw-x-1)) = *(pSrcY+y*sw+x);
			}
			if( progress && !(y%10) ) progress( 0, NULL );
		}
		// UV
		for( y=0; y<yh; y++ ) {
			for( x=0; x<yw; x++ ) {
				*(pDU+(yh-y-1)*yw+(yw-x-1)) = *(pSU+y*yw+x);
				*(pDV+(yh-y-1)*yw+(yw-x-1)) = *(pSV+y*yw+x);
			}
			if( progress && !(y%10) ) progress( 0, NULL );
		}
		break;
	case ROTATE_270:
		// Y
		for( y=0; y<sh; y++ ) {
			for( x=0; x<sw; x++ ) {
				*(pDstY+(sw-x-1)*sh+y) = *(pSrcY+y*sw+x);
			}
			if( progress && !(y%10) ) progress( 0, NULL );
		}
		// UV
		for( y=0; y<yh; y++ ) {
			for( x=0; x<yw; x++ ) {
				*(pDU+(yw-x-1)*yh+y) = *(pSU+y*yw+x);
				*(pDV+(yw-x-1)*yh+y) = *(pSV+y*yw+x);
			}
			if( progress && !(y%10) ) progress( 0, NULL );
		}
		break;
	default:
		memcpy( pDstY, pSrcY, dw*dh );
		memcpy( pDstUV, pSrcUV, dw*dh/2 );
		break;
	}

	if( rotate >= ROTATE_90 && rotate <= ROTATE_270 ) {
		uw = dw/2;
		uh = dh/2;
		for( i=0; i<uh; i++ ) {
			for( j=0; j<uw; j++ ) {
				pDstUV[i*dw+2*j] = pDU[i*uw+j];
				pDstUV[i*dw+2*j+1] = pDV[i*uw+j];
			}
			if( progress && !(y%10) ) progress( 0, NULL );
		}
	}

	free( pSU );
	free( pSV );
	free( pDU );
	free( pDV );
}

static void jpeg_decoding_progress (int percent_done, void *context)
{
}

extern RMuint32 jpeg_fclose( RMuint32 handle, void* pContext );
extern RMuint32 jpeg_fopen( RMuint8* path, void* pContext );
extern RMuint32 jpeg_fread( RMuint32 handle, void* buf, RMuint32 length, void* pContext );
int LoadImageYUV420( const char* filename, unsigned char* pY, unsigned char* pUV, 
			int ylen, int uvlen, int* outW, int* outH,
			PFN_Progress progress )
{
	int nMedia;

	nMedia = CheckMedia( filename, true );
	if( nMedia == MEDIA_JPEG ) {
		JPEG_CALLBACK_TABLE jpeg_callback_table;
		int error=0;
		jpeg_callback_table.context = 0;
		jpeg_callback_table.fclose = (JPEG_fclose)jpeg_fclose;
		jpeg_callback_table.fopen = (JPEG_fopen)jpeg_fopen;
		jpeg_callback_table.fread = (JPEG_fread)jpeg_fread;
		if( progress ) jpeg_callback_table.jpeg_progress = progress;
		else jpeg_callback_table.jpeg_progress = jpeg_decoding_progress;
		error = decode_jpeg_image_420 (
			(char*)filename, 
			(RMuint8 *)pY, (RMuint8 *)pUV, 
			ylen, uvlen, 
			outW, outH, 
			&jpeg_callback_table);
		if ( error < 0)
		{
			printf( "LoadImageYUV420 : %d\n", error );
			return error;
		}

		int x = *outW;
		int y = *outH;
		int i, j;
		*outW = (*outW + 15) & ~0xF;
		*outH = (*outH + 15) & ~0xF;
		// black out non-multiple of 16
		RMuint8 *p;
		p = (RMuint8 *)pY;
		for (j=0; j<*outH; j++) {
			for (i=0; i<*outW; i++) {
				if ((i >= x) || (j >= y))
					*p = 0;
				p++;
			} 
		}
		p = (RMuint8 *)pUV;
		for (j=0; j<*outH; j+=2)	{
			for (i=0; i<*outW; i+=2) {
				if ((i >= x) || (j >= y)) {
					*p = 0x80;
					*(p+1) = 0x80;
				}
				p += 2;
			}
		}
	}
	else if( nMedia == MEDIA_BMP ) {
		CBitmap bmp;
		if( bmp.DecodeBMP( filename ) ) return -1;
		bmp.GetYUV420Data( (char*)pY, (char*)pUV, outW, outH, progress );
		BMPINFOHEADER info = bmp.GetInfoHeader();
	}
	else {
		printf( "LoadImageYUV420: media: %d\n", nMedia );
		return -1;
	}

	return 0;
}

/*
 * LoadYUV420Raw has out of boundary problem.
 * Be careful to use this function!
 * Lee Seok<zstein@newmedialife.com>
 */
int LoadYUV420Raw( const char* filename, YUV420Raw* header, unsigned char* pY, unsigned char* pUV, PFN_Progress progress )
{
	RMuint32 hFile=0;
	FATFS_ERROR sdFatfsErr;
	RMuint32 nBytes;
	int yoffset, uvoffset;
	int ylength, uvlength;

	if( (filename[1] == 0&&filename[0]) || filename[1]&0x80 ) {
		sdFatfsErr = fatfs_wfopen( (RMuint16*)filename, _O_RDONLY, &hFile );
	}
	else {
		sdFatfsErr = fatfs_fopen( (RMuint8*)filename, _O_RDONLY, &hFile );
	}
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		printf( "LoadYUV420 file open error: 0x%x\n", sdFatfsErr );
		return -1;
	}

	fatfs_fread( hFile, (RMuint8*)header, sizeof(YUV420Raw), &nBytes );
	if( nBytes != sizeof(YUV420Raw ) ) goto __error_loadyuv420;
	
	// check magic key.
	if( header->m_szMagicKey[0] != 'Y' || header->m_szMagicKey[1] != 'U' ||
	     header->m_szMagicKey[2] != 'V' || header->m_szMagicKey[3] != '\0' ) goto __error_loadyuv420;
	
	ylength = header->m_nWidth*header->m_nHeight;
	uvlength = ylength/2;
	fatfs_fread( hFile, (RMuint8*)pY, ylength, &nBytes );
	fatfs_fread( hFile, (RMuint8*)pUV, uvlength, &nBytes );
	fatfs_fclose( hFile );
	return 0;

__error_loadyuv420:
	if( hFile ) fatfs_fclose( hFile );
	return -1;
}

int SaveYUV420Raw( const char* filename, unsigned char* pY, unsigned char* pUV, int w, int h, PFN_Progress progress )
{
	YUV420Raw header;
	RMuint32 hFile;
	RMuint32 nBytes;
	FATFS_ERROR sdFatfsErr;

	// Make up header
	header.m_szMagicKey[0] = 'Y';
	header.m_szMagicKey[1] = 'U';
	header.m_szMagicKey[2] = 'V';
	header.m_szMagicKey[3] = 0;

	header.m_nWidth = w;
	header.m_nHeight = h;
	header.m_nFileSize = sizeof(header) + w*h + w*h/2;

	if( (filename[1] == 0&&filename[0]) || filename[1]&0x80 ) {
		sdFatfsErr = fatfs_wfopen( (RMuint16*)filename, _O_WRONLY | _O_CREAT | _O_TRUNC, &hFile );
	}
	else {
		sdFatfsErr = fatfs_fopen( (RMuint8*)filename, _O_WRONLY | _O_CREAT | _O_TRUNC, &hFile );
	}
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		printf( "SaveYUV420Raw file open error: 0x%x\n", sdFatfsErr );
		return -1;
	}

	fatfs_fwrite( hFile, (RMuint8*)&header, sizeof(header), &nBytes );
	if( progress ) progress( 0, 0 );
	fatfs_fwrite( hFile, (RMuint8*)pY, (RMuint32)(w*h), &nBytes );
	if( progress ) progress( 0, 0 );
	fatfs_fwrite( hFile, (RMuint8*)pUV, (RMuint32)(w*h/2), &nBytes );
	if( progress ) progress( 0, 0 );
	fatfs_fclose( hFile );

	return 0;
}
