//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Bit  I/O header
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/BitIO.h,v $
// $Revision: 1.2 $
// $Date: 2006/02/27 02:40:17 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: BitIO.h,v $
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.1  2006/02/20 01:32:57  akaiaphanu
// add function for dictionary
//
//
//*****************************************************************************
#ifndef	_BITIO_HEADER_
#define _BITIO_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PACIFIER_COUNT 2047

typedef struct _bit_file {
//    FILE *file;
	unsigned char	*buf;
	unsigned long	size;
	unsigned long	pos;
	unsigned char mask;
	int rack;
	int pacifier_counter;
} BIT_FILE;

#define INDEX_BIT_COUNT      9

#define LENGTH_BIT_COUNT     4
#define WINDOW_SIZE          ( 1 << INDEX_BIT_COUNT )
#define RAW_LOOK_AHEAD_SIZE  ( 1 << LENGTH_BIT_COUNT )
#define BREAK_EVEN           ( ( 1 + INDEX_BIT_COUNT + LENGTH_BIT_COUNT ) / 9 )
#define LOOK_AHEAD_SIZE      ( RAW_LOOK_AHEAD_SIZE + BREAK_EVEN )
#define TREE_ROOT            WINDOW_SIZE
#define END_OF_STREAM        0
#define UNUSED               0
#define MOD_WINDOW( a )      ( ( a ) & ( WINDOW_SIZE - 1 ) )

int 			InputBit( BIT_FILE *bit_file );
unsigned long 	InputBits( BIT_FILE *bit_file, int bit_count );
void 		ExpandMem( BIT_FILE *input, unsigned char  *output);
BIT_FILE	*	InitBitFile(BIT_FILE	*bit_file);
void 		DeleteBitFile(BIT_FILE	*bit_file);






#endif
