//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Windows BITMAP Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/Bitmap.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: Bitmap.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2006/01/19 05:25:30  zstein
// *** empty log message ***
//
// Revision 1.6  2005/09/28 03:34:01  zstein
// - updated image process routine
//
// Revision 1.5  2005/05/18 11:29:27  zstein
// - supported unicode pathname.
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

#ifndef __WINDOWS_BITMAP_H__
#define __WINDOWS_BITMAP_H__

#include <stdio.h>

#define TEST		0

typedef struct {
	unsigned short int type;                 /* Magic identifier            */
	unsigned int size;                       /* File size in bytes          */
	unsigned short int reserved1, reserved2;
	unsigned int offset;                     /* Offset to image data, bytes */
} __attribute__((packed)) BMPHEADER;

typedef struct {
	unsigned int size;               /* Header size in bytes      */
	int width,height;                /* Width and height of image */
	unsigned short int planes;       /* Number of colour planes   */
	unsigned short int bits;         /* Bits per pixel            */
	unsigned int compression;        /* Compression type          */
	unsigned int imagesize;          /* Image size in bytes       */
	int xresolution,yresolution;     /* Pixels per meter          */
	unsigned int ncolours;           /* Number of colours         */
	unsigned int importantcolours;   /* Important colours         */
} BMPINFOHEADER;

typedef struct {
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char x1;   // reserved byte....
} RGB;

#define MEMORY_LIMITATION			1440000		// 800*600*3

class CBitmap
{
protected:
	BMPHEADER m_Header;
	BMPINFOHEADER m_Info;

	int m_hFile;
	bool m_fFlip;
public:
	CBitmap( void );
	~CBitmap( void );

	enum {
		BMP_ERROR_NOERROR,
		BMP_ERROR_NOT_BMP,
		BMP_ERROR_FILE_NOT_FOUND,
		BMP_ERROR_IMAGE_TOO_BIG,
		BMP_ERROR_NOTSUPPORT_COMPRESS
	};

	int DecodeBMP( const char* path, bool fUnicode=true );
	BMPHEADER GetHeader( void );
	BMPINFOHEADER GetInfoHeader( void );
	bool GetImageData( char* pImageData );
	bool GetYUV420Data( char* pY, char* pUV, int* nOutW, int* nOutH, void(*progress)(int, void*) );
	bool GetPalette( char* pPalette );
};

#endif /* __WINDOWS_BITMAP_H__ */
