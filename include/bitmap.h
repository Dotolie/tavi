//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Bitmap Image Structure Define
// 
// $Source: /home/cvsroot/TAVI020/tavi/include/bitmap.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: bitmap.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/10/13 01:25:25  etnlwind
// Update for Header Info
//
//*****************************************************************************


/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * bitmap.h
 * Sang-jin Ryu (ew@newmedialife.com)
 *
 */

#ifndef __BITMAP_H__
#define __BITMAP_H__

#define _ATTRIBUTE_PACKED_		__attribute__ ((packed)) 

typedef struct  _tag_BITMAPFILEHEADER
{
  unsigned short bfType;
  unsigned int   bfSize;
  unsigned short bfReserved1;
  unsigned short bfReserved2;
  unsigned int   bfOffBits;
} _ATTRIBUTE_PACKED_ BITMAPFILEHEADER;

typedef struct _tag_BITMAPINFOHEADER
{
  unsigned int   biSize;
  unsigned int   biWidth;
  unsigned int   biHeight;
  unsigned short biPlanes;
  unsigned short biBitCount;
  unsigned int   biCompression;
  unsigned int   biSizeImage;
  unsigned int   biXPelsPerMeter;
  unsigned int   biYPelsPerMeter;
  unsigned int   biClrUsed;
  unsigned int   biCirImportant;
} _ATTRIBUTE_PACKED_ BITMAPINFOHEADER;

typedef struct   _tag_RGBQUARD
{
  unsigned char B;
  unsigned char G;
  unsigned char R;
  unsigned char x1;   // reserved byte....
} _ATTRIBUTE_PACKED_ RGBQUARD;

typedef struct   _tag_BITMAPFILEINFO
{
   BITMAPFILEHEADER bitmapfileheader;
   BITMAPINFOHEADER bitmapinfoheader;
   void     * image_buffer;
} BITMAPFILEINFO;

typedef struct   _tag_BITMAPFILEINFO256
{
  BITMAPFILEHEADER bitmapfileheader;
  BITMAPINFOHEADER bitmapinfoheader;

  RGBQUARD      * palette;
  unsigned char 	* image_buffer;

} BITMAPFILEINFO256;

#endif


