//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : memory expand
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/BitIO.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/02/27 02:40:17 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: BitIO.cpp,v $
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.1  2006/02/20 01:32:57  akaiaphanu
// add function for dictionary
//
//
//*****************************************************************************
#include "BitIO.h"

/*
 * These are the two global data structures used in this program.
 * The window[] array is exactly that, the window of previously seen
 * text, as well as the current look ahead text.  The tree[] structure
 * contains the binary tree of all of the strings in the window sorted
 * in order.
*/

unsigned char window[ WINDOW_SIZE ];

int InputBit( BIT_FILE *bit_file )
{
	int value;

	if ( bit_file->mask == 0x80 ) {
		if(bit_file->pos < bit_file->size)
		{
			bit_file->rack = *bit_file->buf++;
			bit_file->pos++;
		}
		else
			bit_file->rack = -1;
		if ( bit_file->rack == -1 )
			printf( "Fatal error in InputBit!\n" );
		if ( ( bit_file->pacifier_counter++ & PACIFIER_COUNT ) == 0 )
			printf(".");
	}
	value = bit_file->rack & bit_file->mask;
	bit_file->mask >>= 1;
	if ( bit_file->mask == 0 )
		bit_file->mask = 0x80;
	return( value ? 1 : 0 );
}

unsigned long InputBits( BIT_FILE *bit_file, int bit_count )
{
	unsigned long mask;
	unsigned long return_value;

	mask = 1L << ( bit_count - 1 );
	return_value = 0;
	while ( mask != 0) {
		if ( bit_file->mask == 0x80 ) {
			if(bit_file->pos < bit_file->size)
			{
				bit_file->rack = *bit_file->buf++;
				bit_file->pos++;
			}
			else
				bit_file->rack = -1;
		if ( bit_file->rack == -1 )
			printf( "Fatal error in InputBit!\n" );
		if ( ( bit_file->pacifier_counter++ & PACIFIER_COUNT ) == 0 )
			printf(".");
		}
		if ( bit_file->rack & bit_file->mask )
			return_value |= mask;
		mask >>= 1;
		bit_file->mask >>= 1;
		if ( bit_file->mask == 0 )
			bit_file->mask = 0x80;
	}
	return( return_value );
}

void ExpandMem( BIT_FILE *input, unsigned char  *output)
{
	int i;
	int current_position;
	int c;
	int match_length;
	int match_position;
	unsigned char	*dummy = output;
	unsigned long	pos = 0;
	current_position = 1;
	for ( ; ; ) {
		if ( InputBit( input ) ) {
			c = (int) InputBits( input, 8 );
			//putc( c, output );
			output[pos++] = c;
			window[ current_position ] = (unsigned char) c;
			current_position = MOD_WINDOW( current_position + 1 );
		} else {
			match_position = (int) InputBits( input, INDEX_BIT_COUNT );
			if ( match_position == END_OF_STREAM )
				break;
			match_length = (int) InputBits( input, LENGTH_BIT_COUNT );
			match_length += BREAK_EVEN;
			for ( i = 0 ; i <= match_length ; i++ ) {
				c = window[ MOD_WINDOW( match_position + i ) ];
				//putc( c, output );
				output[pos++] = c;
				window[ current_position ] = (unsigned char) c;
				current_position = MOD_WINDOW( current_position + 1 );
			}
		}
	}
}

BIT_FILE	*InitBitFile(BIT_FILE	* bit_file)
{
	bit_file = (BIT_FILE *) malloc(sizeof( BIT_FILE ) );
	if ( bit_file == NULL ){
		printf("Memory Allocation Error\n");
		return NULL;
	}
	bit_file->buf = (unsigned char *)malloc(8192);
	if ( bit_file->buf == NULL ){
		printf("Memory Allocation Error\n");
		return NULL;
	}
	
	memset(bit_file->buf, 0, 8192);
	bit_file->pos = 0;
	bit_file->size = 0;
	//bit_file->file = fp;
	bit_file->rack = 0;
	bit_file->mask = 0x80;
	bit_file->pacifier_counter = 0;
	return bit_file;
}

void 	DeleteBitFile(BIT_FILE	*bit_file)
{
	if(bit_file->buf)
		free(bit_file->buf);
	free(bit_file);
}

