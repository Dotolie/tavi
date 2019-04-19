//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Windows BITMAP Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/Bitmap.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: Bitmap.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.10  2006/01/19 05:25:30  zstein
// *** empty log message ***
//
// Revision 1.9  2005/09/28 03:34:01  zstein
// - updated image process routine
//
// Revision 1.8  2005/07/16 04:13:51  zstein
// - fixed bugs & added functions.
//
// Revision 1.7  2005/06/16 23:49:48  zstein
// - bug fixed
//
// Revision 1.6  2005/05/18 11:29:27  zstein
// - supported unicode pathname.
//
// Revision 1.5  2005/05/16 05:37:23  zstein
// *** empty log message ***
//
// Revision 1.4  2005/05/03 02:14:13  zstein
// *** empty log message ***
//
// Revision 1.3  2005/04/27 04:53:20  zstein
// Support Multi-languages.
// Support resource text.
//
// Revision 1.2  2005/04/23 06:08:07  zstein
// added m-language
//
// Revision 1.1  2005/04/05 08:00:22  zstein
// Bitmap class
//
//
//*******************************************************************************

#include <stdlib.h>
#include <string.h>
#include "Bitmap.h"

#define HEADER_SIZE		54

#if TEST
#define LINUX_FS_DRV 1
#else
#include <rmbasic.h>
#include <fatfs.h>
#endif /* TEST */

static char rgb2y (char r, char g, char b)
{           
        int f = 257*(int)r + 504*(int)g + 98*(int)b + 16000;
            if (f > 255000)
                        f = 255000;
                f = f / 1000;
                    return (char)f;
}           
static char rgb2u (char r, char g, char b)
{           
        int f = -148*(int)r - 291*(int)g + 439*(int)b + 128000;
            if (f > 255000)
                        f = 255000;
                if (f < 0)
                            f = 0;
                    f = f / 1000;
                        return (char)f;
}
static char rgb2v (char r, char g, char b)
{               
        int f = 439*(int)r - 368*(int)g - 71*(int)b + 128000;
            if (f > 255000)
                        f = 255000;
                if (f < 0)
                            f = 0;
                    f = f / 1000;
                        return (char)f;
}

CBitmap::CBitmap( void )
{
	#if !TEST
	m_hFile = -1;
	#else 
	m_hFile = NULL;
	#endif
	m_fFlip = false;
}

CBitmap::~CBitmap( void )
{
	#if LINUX_FS_DRV
	if( m_hFile ) fclose( m_hFile );
	#else
	fatfs_fclose( (RMuint32)m_hFile );
	#endif
}

#include <unistd.h>
int CBitmap::DecodeBMP( const char* path, bool fUnicode )
{
	int ret = BMP_ERROR_NOERROR;

	#if LINUX_FS_DRV
	m_hFile = fopen( path, "rb" );
	#else 
	FATFS_ERROR sdFatfsErr;
	if( fUnicode ) 
		sdFatfsErr = fatfs_wfopen( (RMuint16*)path, _O_RDONLY, (RMuint32*)&m_hFile );
	else 
		sdFatfsErr = fatfs_fopen( (RMuint8*)path, _O_RDONLY, (RMuint32*)&m_hFile );
//	sdFatfsErr = fatfs_fopen( (RMuint8*)path, _O_RDONLY, (RMuint32*)&m_hFile );
	if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) {
		printf( "file open error: 0x%x\n", sdFatfsErr );
		return false;
	}
	#endif
	if( m_hFile <= 0 ) {
		return BMP_ERROR_FILE_NOT_FOUND;
	}

	#if LINUX_FS_DRV
	fread( (void*)&m_Header, 1, sizeof(m_Header), m_hFile );
	fread( (void*)&m_Info, 1, sizeof(m_Info), m_hFile );
	#else
	RMuint32 nBytes;
	fatfs_fread( (RMuint32)m_hFile, (RMuint8*)&m_Header, sizeof(m_Header), &nBytes );
	fatfs_fread( (RMuint32)m_hFile, (RMuint8*)&m_Info, sizeof(m_Info), &nBytes );
	#endif
	// if height is negative integer, image is flipped.
	if( m_Info.height < 0 ) {
		m_Info.height = -m_Info.height;
		m_fFlip = true;
	}

	if( m_Header.type != 0x4d42 ) return BMP_ERROR_NOT_BMP;

	if( m_Info.compression ) return BMP_ERROR_NOTSUPPORT_COMPRESS;
	
	int size = m_Info.height * m_Info.width * 3;
	
	if( size > MEMORY_LIMITATION ) 	return BMP_ERROR_IMAGE_TOO_BIG;

	return ret;
}

BMPHEADER CBitmap::GetHeader( void ) 
{
	return m_Header;
}

BMPINFOHEADER CBitmap::GetInfoHeader( void )
{
	return m_Info;
}

bool CBitmap::GetImageData( char* pData )
{
	int padding;
	
	if( m_hFile <= 0 ) return false;
	if( pData == NULL ) return false;
	#if LINUX_FS_DRV
	fseek( m_hFile, m_Header.offset, SEEK_SET );
	#else
	RMuint32 nBytes;
	fatfs_fseek( (RMuint32)m_hFile, m_Header.offset, SEEK_SET );
	#endif

	padding = m_Info.width%4;
	if( padding ) padding = 4- padding;
	
	int size;
	bool fIndexed;
	int stride;
	if( m_Header.offset == HEADER_SIZE ) { // 54 = size of header + size of info , 24-bit color
		size = m_Info.width * m_Info.height * 3;
		fIndexed = false;
		stride = 3*m_Info.width;
	}
	else { // indexed-color
		size = m_Info.width * m_Info.height;
		fIndexed = true;
		stride = m_Info.width;
	}

	char* p;
	if( fIndexed ) p = pData + m_Info.width * m_Info.height;
	else p = pData + m_Info.width * m_Info.height * 3;
	for(unsigned int idx =0 ; idx <m_Info.height; idx++) {
		p -= stride;
		#if LINUX_FS_DRV
		fread( p, stride, 1, m_hFile );
		#else
		fatfs_fread( (RMuint32)m_hFile, (RMuint8*)p, (RMuint32)stride, &nBytes );
		fatfs_fseek( (RMuint32)m_hFile, padding, SEEK_CUR );
		#endif
	}

	return true;
}

bool CBitmap::GetYUV420Data( char* pY, char* pUV, int* nOutW, int* nOutH, void(*progress)(int, void*) )
{
	RGB* pPalette = NULL;
	unsigned char* pRGB = NULL;
	int ret = BMP_ERROR_NOERROR;
	
	int nBytes;
	bool fIndexed;
	int stride;
	int padding = 0, wpadding=0;
	int y, u, v;
	int k, l, uv, idx,  w, h, pw, ph, i, j;
	unsigned char* line1;
	unsigned char* line2;
	int cw = m_Info.bits/8;

	if( m_Header.offset == HEADER_SIZE ) { // 54 = size of header + size of info , 24-bit color
		fIndexed = false;
		stride = 3*m_Info.width;
	}
	else { // indexed-color
		fIndexed = true;
		stride = m_Info.width;
		pPalette = ( RGB* )malloc( m_Header.offset - HEADER_SIZE );
		int nPaletteSize = m_Header.offset - 54;
		#if LINUX_FS_DRV
		fseek( m_hFile, 54, SEEK_SET );
		fread( pPalette, 1, nPaletteSize, m_hFile );
		#else
		RMuint32 nBytes;
		fatfs_fseek( (RMuint32)m_hFile, 54, SEEK_SET );
		fatfs_fread( (RMuint32)m_hFile, (RMuint8*)pPalette, (RMuint32)nPaletteSize, (RMuint32*)&nBytes );
		#endif
		printf( "Indexed-BMP\n" );
	}

	if( m_Info.width%4 )
		padding = 4-(m_Info.width*cw)%4;
	else padding = 0;

	//printf( "w: %d, h: %d\n", m_Info.width, m_Info.height );
	//printf( "padding: %d\n", padding );
	//printf( "stride: %d\n", stride );
	int imagesize = m_Info.imagesize;
	
	if( imagesize == 0 ) {
		imagesize = (unsigned)m_Info.width*(unsigned)m_Info.bits/8*(unsigned)m_Info.height;
	}
	if( imagesize > MEMORY_LIMITATION ) {
		ret = BMP_ERROR_IMAGE_TOO_BIG;
		goto error;
	}
	
	h = m_Info.height;
	w = m_Info.width;

	//stride = m_Info.width*(m_Info.bits+7)/8+(4-padding);
	#if LINUX_FS_DRV
	fseek( m_hFile, m_Header.offset, SEEK_SET );
	#else
	fatfs_fseek( (RMuint32)m_hFile, (RMuint32)m_Header.offset, SEEK_SET );
	#endif
	
	pRGB = ( unsigned char* )malloc( imagesize );
	#if LINUX_FS_DRV
	fread( pRGB, 1, imagesize, m_hFile );
	#else
	fatfs_fread( (RMuint32)m_hFile, (RMuint8*)pRGB, (RMuint32)imagesize, (RMuint32*)&nBytes );
	#endif

	uv = 0;
	//int i, j, o1, o2;
    pw = (w+15)&~0x0f;
    ph = (h+15)&~0x0f;

    *nOutW = pw;
    *nOutH = ph;

    line1 = new unsigned char[pw*cw];
    line2 = new unsigned char[pw*cw];
	for( idx=0 ; idx<ph; idx+=2) {
		k = l = 0;
            memset( line1, 0, pw*cw );
            memset( line2, 0, pw*cw );
            #if 0
		if( m_fFlip ) {
			line1 = pRGB+idx*(m_Info.width*cw+padding);
			line2 = pRGB+(idx+1)*(m_Info.width*cw+padding);
		}
		else {
			line1 = pRGB+(m_Info.height-idx-1)*(m_Info.width*cw+padding);
			line2 = pRGB+(m_Info.height-idx-2)*(m_Info.width*cw+padding);
		}
            #else
            if( idx < h ) {
                if( m_fFlip ) {
                    memcpy( line1, pRGB+idx*(m_Info.width*cw+padding), w*cw );
                    memcpy( line2, pRGB+(idx+1)*(m_Info.width*cw+padding), w*cw );
                }
                else {
                    memcpy( line1, pRGB+(m_Info.height-idx-1)*(m_Info.width*cw+padding), w*cw );
                    memcpy( line2, pRGB+(m_Info.height-idx-2)*(m_Info.width*cw+padding), w*cw );
                }
            }
            #endif
		// convert yuv420
		if( fIndexed ) {
			for( j=0; j<pw; j+=2 ) {
				u = v = 0;
                        if( j<w && idx<h ) {
				y = rgb2y( pPalette[line1[j]].R, pPalette[line1[j]].G, pPalette[line1[j]].B );
				u += rgb2u( pPalette[line1[j]].R, pPalette[line1[j]].G, pPalette[line1[j]].B );
				v += rgb2v( pPalette[line1[j]].R, pPalette[line1[j]].G, pPalette[line1[j]].B );
				pY[pw*idx+k++] = y;
				y = rgb2y( pPalette[line1[j+1]].R, pPalette[line1[j+1]].G, pPalette[line1[j+1]].B );
				u += rgb2u( pPalette[line1[j+1]].R, pPalette[line1[j+1]].G, pPalette[line1[j+1]].B );
				v += rgb2v( pPalette[line1[j+1]].R, pPalette[line1[j+1]].G, pPalette[line1[j+1]].B );
				pY[pw*idx+k++] = y;
				y = rgb2y( pPalette[line2[j]].R, pPalette[line2[j]].G, pPalette[line2[j]].B );
				u += rgb2u( pPalette[line2[j]].R, pPalette[line2[j]].G, pPalette[line2[j]].B );
				v += rgb2v( pPalette[line2[j]].R, pPalette[line2[j]].G, pPalette[line2[j]].B );
				pY[(idx+1)*pw+l++] = y;
				y = rgb2y( pPalette[line2[j+1]].R, pPalette[line2[j+1]].G, pPalette[line2[j+1]].B );
				u += rgb2u( pPalette[line2[j+1]].R, pPalette[line2[j+1]].G, pPalette[line2[j+1]].B );
				v += rgb2v( pPalette[line2[j+1]].R, pPalette[line2[j+1]].G, pPalette[line2[j+1]].B );
				pY[(idx+1)*pw+l++] = y;
                        }
                        else {
                            pY[pw*idx+k++] = 0;
                            pY[pw*idx+k++] = 0;
                            pY[(idx+1)*pw+l++] = 0;
                            pY[(idx+1)*pw+l++] = 0;
                            u = v = 0x200;
                        }
			pUV[uv++] = u/4;
			pUV[uv++] = v/4;
			}
			
		}
		else {
                    register int mstride = pw*cw;
                    register int stride = w*cw;
			for( j=0; j<mstride; j+=6 ) {
				u = v = 0;
				if( j<stride && idx<h ) {
					y = rgb2y( line1[j+2], line1[j+1],  line1[j] );
					u += rgb2u( line1[j+2], line1[j+1],  line1[j] );
					v += rgb2v( line1[j+2], line1[j+1],  line1[j] );
					pY[pw*idx+k++] = y;
					y = rgb2y( line1[j+5], line1[j+4],  line1[j+3] );
					u += rgb2u( line1[j+5], line1[j+4],  line1[j+3] );
					v += rgb2v( line1[j+5], line1[j+4],  line1[j+3] );
					pY[pw*idx+k++] = y;
					y = rgb2y( line2[j+2], line2[j+1],  line2[j] );
					u += rgb2u( line2[j+2], line2[j+1],  line2[j] );
					v += rgb2v( line2[j+2], line2[j+1],  line2[j] );
					pY[(idx+1)*pw+l++] = y;
					y = rgb2y( line2[j+5], line2[j+4],  line2[j+3] );
					u += rgb2u( line2[j+5], line2[j+4],  line2[j+3] );
					v += rgb2v( line2[j+5], line2[j+4],  line2[j+3] );
					pY[(idx+1)*pw+l++] = y;
				}
                            else {
                                pY[pw*idx+k++] = 0;
                                pY[pw*idx+k++] = 0;
                                pY[pw*(idx+1)+l++] = 0;
                                pY[pw*(idx+1)+l++] = 0;
                                u = v = 0x200;
                            }
				pUV[uv++] = u/4;
				pUV[uv++] = v/4;
			}
			if( progress ) progress( 0, 0 );
		}
	}

    delete[] line1;
    delete[] line2;

error:
	if( pPalette ) free( pPalette );
	if( pRGB ) free( pRGB );

	return ret;

}

bool CBitmap::GetPalette( char* pPalette )
{
	if( pPalette == NULL ) return false;

	if( m_Header.offset == 54 ) return false; // 24-bit true color

	#if LINUX_FS_DRV
	fseek( m_hFile, 54, SEEK_SET );
	#else
	fatfs_fseek( (RMuint32)m_hFile, 54, SEEK_SET );
	#endif

	int size = m_Header.offset - 54;
	if( size%4 ) {
		return false;
	}

	#if LINUX_FS_DRV
	fread( pPalette, 1, size, m_hFile );
	#else
	RMuint32 nBytes;
	fatfs_fread( (RMuint32)m_hFile, (RMuint8*)pPalette, (RMuint32)size, &nBytes );
	#endif
	
	return true;
}

#if TEST
int main( int argc, char** argv )
{
	CBitmap bitmap;
	FILE* fpy, *fpu, *fpv;
	unsigned char* py, *pu, *pv;

	if( argc < 2 ) {
		printf( "%s filename\n", argv[0] );
		return 1;
	}
	fpy = fopen( "y.raw", "wb" );
	if( fpy == NULL ) {
		printf( "failed to open y.raw\n" );
		return 1;
	}
	
	fpu = fopen( "u.raw", "wb" );
	if( fpu == NULL ) {
		printf( "failed to open u.raw\n" );
		return 1;
	}
	fpv = fopen( "v.raw", "wb" );
	if( fpu == NULL ) {
		printf( "failed to open u.raw\n" );
		return 1;
	}
	
	
	bitmap.DecodeBMP( argv[1] );

	BMPHEADER header = bitmap.GetHeader();
	BMPINFOHEADER info = bitmap.GetInfoHeader();

	int h = info.height+info.height%2;
	int rsize = info.width*info.height;
	int size = info.width*info.height;
	printf( "w: %d, h: %d\n", info.width, info.height );
	printf( "size: %d\n", size );
	py = ( unsigned char* )malloc( size );
	pu = ( unsigned char* )malloc( size/4 );
	pv = ( unsigned char* )malloc( size/4 );

	if( bitmap.GetYUV420Data( py, pu, pv ) == false ) {
		printf( "error: %d\n", bitmap.GetError() );
		return 1;
	}

	int i;
	fseek( fpy, 0, SEEK_SET );
	fseek( fpu, 0, SEEK_SET );
	fseek( fpv, 0, SEEK_SET );
	fwrite( py, rsize, 1, fpy );
	fwrite( pu, rsize/4, 1, fpu );
	fwrite( pv, rsize/4, 1, fpv );
	printf( "<1>\n" );
	fclose( fpy );
	printf( "<2>\n" );
	fclose( fpu );
	printf( "<3>\n" );
	fclose( fpv );
	printf( "<4>\n" );
	free( py );
	free( pu );
	free( pv );
	printf( "<5>\n" );
	return 0;
}
#endif
