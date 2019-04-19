//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : MD5  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/md5/genmd5key.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: genmd5key.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2006/02/21 05:15:51  ywkim
// Add print routine for md5 key
//
// Revision 1.4  2006/02/21 05:02:02  ywkim
// Change MD5 tag to md5 in CheckKey()
//
// Revision 1.3  2006/02/17 14:06:29  ywkim
// Change MD5 tag to md5 in RemoveKey
//
// Revision 1.2  2006/02/17 07:07:03  ywkim
// Change MD5 tag to md5
//
// Revision 1.1  2005/11/22 05:54:01  ywkim
// Add new directory & source for MD5 key
//
//*****************************************************************************

#include "md5.h"
#include <stdio.h>
#include <getopt.h>
#include <string.h>


#define	MODE_NONE 				0x00
#define 	MODE_MD5CREATE		0x01
#define	MODE_MD5REMOVE		0x02
#define	MODE_MD5CHECK		0x04
#define 	MODE_MASKLOW			0xf0
#define	MODE_INFILE			0x10
#define	MODE_OUTFILE			0x20

#define LENGTH_FILENAME		16

#define K 	 (1024)
#define M	 (K*1024)
#define BUFFER_SIZE 				4*M

#define TAGMD5					"md5"
#define LENGTH_TAG				3
#define LENGTH_KEY				16
#define LENGTH_TAGNKEY			(LENGTH_TAG+LENGTH_KEY)

enum {
	RET_NOMD5 = 0,
	RET_MATCHMD5,
	RET_DIFFERENTMD5,
	RET_DAMAGEKEY
};

int AppendFile( char infile_name[], CMD5 *pMd5 )
{
	unsigned char *pKey;
	FILE *ifp;
	int	length;
	int	length1;
	int	length2;

	length = 0;
	pKey = pMd5->md5_get();
	
#if 1
	// output md5 key to console 
	printf("\t%s", TAGMD5);
	for( int i=0;i<LENGTH_KEY;i++)
		printf(":%02X", *(pKey+i));
	printf("\n");
#endif

	if( ( ifp = fopen( infile_name, "a" ) ) > 0 )	{
		if ( (length1 = fwrite(TAGMD5, 1, LENGTH_TAG, ifp) ) > 0 ) {
			if ( (length2 = fwrite(pKey, 1, LENGTH_KEY, ifp ) ) > 0) {
				// completely append md5 key
				
				}
			}
		}
	else {
		printf( "%s cannot open.\n", infile_name );
		}
	
	fclose ( ifp );
	
	length = length1 + length2;
	
	return length;
}

int RemoveKey( char infile_name[], char *buffer, int length )
{
	int 	last;
	int	length0;
	char md5[LENGTH_KEY];
	FILE *ofp;
	
	last = 3;
	length0 = 0;
	
	while( last < 20 )	{
		if ( buffer[length-last] == 'm' && buffer[length-last+1] == 'd' && buffer[length-last+2] == '5' ) {
			// match md5 tag
			break;
			}
		last++;		
		}
	if ( last < 20 ) {
		if( ( ofp = fopen( infile_name, "w" ) ) > 0 )	{
			if ( (length0 = fwrite(buffer, 1, length-last, ofp) )> 0 ) {
				// success for write
				}
			else {
				printf( "%s can't be writen.\n", infile_name );
				}
		       }
		else {
			printf( "%s can't be open.\n", infile_name );
			}

		fclose( ofp );

		}
	return length0;
}


int ReadFile(char infile_name[], char *buffer )
{
	FILE *ifp;
	int	length;

	length = 0;
	
	if( ( ifp = fopen( infile_name, "r+" ) ) > 0 )	{
		if ( (length=fread(buffer, 1, BUFFER_SIZE, ifp)) > 0)	{
			// completely readed
			}
		else {
			printf( "%s cannot read.\n", infile_name );
			}	
	       }
	else {
		printf( "%s cannot open.\n", infile_name );
		}

	fclose ( ifp );

	return length;
}

int CreateFile( char outfile_name[], char big_buf[], int length, CMD5 *pMd5 )
{
	unsigned char *pKey;
	int	length0;
	int	length1;
	int	length2;
	int	length3;	
	FILE *ofp;

	length0 = 0;
	pKey = pMd5->md5_get();

#if 1
		// output md5 key to console 
		printf("\t%s", TAGMD5);
		for( int i=0;i<LENGTH_KEY;i++)
			printf(":%02X", *(pKey+i));
		printf("\n");
#endif
	
	if( ( ofp = fopen( outfile_name, "w" ) ) > 0 )	{
		if ( (length1 = fwrite(big_buf, 1, length, ofp) )> 0 ) {
			if ( (length2 = fwrite(TAGMD5, 1, LENGTH_TAG, ofp ) ) > 0 ) {
				if ( (length3 = fwrite(pKey, 1, LENGTH_KEY, ofp ) ) > 0 ) {
					// completely created
					}
				}
			}
		else {
			printf( "%s can't be writen.\n", outfile_name );
			}
	       }
	else {
		printf( "%s can't be open.\n", outfile_name );
		}

	fclose( ofp );

	length0 = length1 + length2 + length3;
	return length0;

}


int CheckKey( char *buffer, int length, CMD5* pMd5 )
{
	int 	last;
	int	length_withNokey;
	char md5[LENGTH_KEY];

	last = 3;
	
	while( last < 20 )	{
		if ( buffer[length-last] == 'm' && buffer[length-last+1] == 'd' && buffer[length-last+2] == '5' ) {
			// exist md5 tag
			if ( last == 19 ) {
				length_withNokey = length - (LENGTH_TAGNKEY);
				memcpy( md5, &buffer[length - LENGTH_KEY], LENGTH_KEY );
				pMd5->md5_update( (unsigned char *)buffer, length_withNokey);
				pMd5->md5_finish();
				if ( memcmp( pMd5->md5_get(), md5, LENGTH_KEY ) == 0 ) {
					// match md5 key
					return RET_MATCHMD5;
					}
				else	{
					// different md5 key
					return RET_DIFFERENTMD5;
					}
				}
			else {
				// damage md5 key
				return RET_DAMAGEKEY;
				}
			}
		else {
			last++;		
			}
		}
	if ( last >= 20 ) {
		// has no md5 key
		length_withNokey = length;
		pMd5->md5_update( (unsigned char *)buffer, length_withNokey);
		pMd5->md5_finish();
		return RET_NOMD5;
		}
}

int main( int argc, char** argv)
{
	int opt;
	int mode;
	int state;
	char outfile_name[LENGTH_FILENAME];
	char infile_name[LENGTH_FILENAME];
	char big_buf[BUFFER_SIZE];
	int	length_org;
	int 	length_last;
	FILE *ifp, *ofp;
	CMD5 *pMd5;

	mode  = MODE_NONE;
	
	while ( (opt =getopt( argc, argv, "cri:o:" )) != -1 )	{
		switch ( opt ) {
			case 'c':				// check existance of md5 key
				mode |= MODE_MD5CHECK;
				break;
			case 'r':				// remove md5 key
				mode |= MODE_MD5REMOVE;
				break;
			case 'o':				// assign output file name
				mode |= MODE_OUTFILE;
				sprintf ( outfile_name, "%s",  optarg );
				break;
			case 'i':				// append 
				mode |= MODE_INFILE|MODE_MD5CREATE;	
				sprintf( infile_name, "%s", optarg );
				break;
			default:				// none excute
				break;
			}
		}
	
	if ( mode == MODE_NONE || !(mode & MODE_INFILE) )	{
		printf("usage : genmd5key [-cr] -i filename [-o filename]\n");
		}
	else	{

		if (  (length_org = ReadFile( infile_name, big_buf ) ) == 0 )
			return -1;

		
		pMd5 = new CMD5();
		state = CheckKey( big_buf, length_org, pMd5 );
		
		if ( mode & MODE_MD5CHECK) {
			switch ( state )	{
				case RET_MATCHMD5:
					// 	match md5 key
					printf("%s already has md5 key.\n", infile_name);
					break;
				case RET_DIFFERENTMD5:
					// 	different md5 key
					printf("%s has md5 key, But key invalid.\n", infile_name);
					break;				
				case RET_NOMD5:
					// 	no md5 key
					printf("%s has no md5 key.\n", infile_name);
					break;
				case RET_DAMAGEKEY:
					// 	have md5 tag but key damage
					printf("%s has md5 tag, but key damaged.\n", infile_name);
					break;					
				}
			}
		else if ( mode & MODE_MD5REMOVE ) {
			switch ( state )	{
				case RET_MATCHMD5:
				case RET_DAMAGEKEY:
				case RET_DIFFERENTMD5:
					length_last = RemoveKey( infile_name, big_buf, length_org );
					break;
				default:
					//	file has not md5 key
					printf("%d has no md5 key yet.\n");
					break;			
				}
			}
		else if ( mode & MODE_MD5CREATE ) 	{
			switch ( state )	{
				case RET_NOMD5:
					if ( mode & MODE_OUTFILE ) {
						printf("%s was created with md5 key.\n", outfile_name);
						length_last=CreateFile( outfile_name, big_buf, length_org, pMd5 );
						}
					else {
						printf("%s was added with md5 key.\n", infile_name);
						length_last=AppendFile( infile_name, pMd5 );
						}
					break;
				default:
					//	file has not md5 key
					printf("%s has md5 key or tag.\n", infile_name);
					break;
				}
			}	

		printf("[%8dB]=>[%8dB]\n", length_org, length_last);

		delete pMd5;
		}
	
	
	return 0;

}
