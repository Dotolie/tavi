//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Pixmap class declaration

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/Pixmap.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $

//

//******************************************************************************

// $Log: Pixmap.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/06/21 06:08:29  zstein
// - bug fixed.
//
// Revision 1.4  2005/05/15 11:08:49  zstein
// *** empty log message ***
//
// Revision 1.3  2005/04/23 06:08:07  zstein
// added m-language
//
// Revision 1.2  2005/02/25 01:13:09  zstein
// *** empty log message ***
//
// Revision 1.1  2005/02/16 16:03:25  zstein
// *** empty log message ***
//

//*******************************************************************************

#ifndef __PIXMAP_H__
#define __PIXMAP_H__

#include "object.h"
/*
 * CPixmap class
 * pixmap class is used to store temporay image data.
 */
class CPixmap
{
public:
	int m_nWidth;
	int m_nHeight;
	int m_nStride;
	char* m_pBuf;
	char m_nBpp;
	bool m_fAutoDelete;

	CPixmap( void );
	CPixmap( const CPixmap& pixmap );
	// NOTE: DON'T DELETE pImage after call CreatePixmap.
	CPixmap( const CSize& size, int bpp, char* pImage=NULL );
	CPixmap( int w, int h, int bpp, char* pImage=NULL );
	~CPixmap( void );

	char* GetImageBuffer( void );
	CSize GetSize( void );
	bool CreatePixmap( int w, int h, int bpp, char* pImageBuf=0, bool fDelete=false );
	void Fill( int x, int y, int w, int h, int color );
	int GetStride( void );
	void SetPixel( unsigned int x, unsigned int y, int color );
	void Copy( CPixmap* pixmap );
	void Not( int x, int y, int w, int h );
	void Print( void );
};

#endif /* __PIXMAP_H__ */
