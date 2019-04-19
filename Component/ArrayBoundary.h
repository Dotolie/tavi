//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : List view class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ArrayBoundary.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//******************************************************************************
// $Log: ArrayBoundary.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/11/16 06:39:52  zstein
// - clear source
//
// Revision 1.1  2005/02/16 16:03:25  zstein
// *** empty log message ***
//
//*******************************************************************************

#ifndef __ARRAYOUTOFBOUND_H__
#define __ARRAYOUTOFBOUND_H__

#include <stdio.h>

class ArrayOutOfBound
{
public:
	ArrayOutOfBound( void ) {}
	virtual ~ArrayOutOfBound( void ) {}

	void PrintError( const char* msg ) {
		printf( "ArrayOutOfBound: %s\n", msg );
	}
};

#endif /* __ARRAYOUTOFBOUND_H__ */
