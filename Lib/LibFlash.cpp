//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : tested only on 2MB AMD29LV160DB  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibFlash.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: LibFlash.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.16  2005/12/06 06:34:53  ywkim
// Change ProgramOneWord()
//
// Revision 1.15  2005/08/08 11:36:38  ywkim
// Remark verifying routine for flash
//
// Revision 1.14  2005/06/16 02:01:26  ywkim
// Change flash_xx for flash_progress
//
// Revision 1.13  2005/06/01 05:32:55  ywkim
// add function of disable & enable global interrupt for flash update
//
// Revision 1.12  2005/05/27 16:01:19  ywkim
// add delay time for erase
//
// Revision 1.11  2005/05/27 13:16:35  ywkim
// Change erase routine
//
// Revision 1.10  2005/05/14 06:02:41  ywkim
// increase erase time 25ms to 50ms
//
// Revision 1.9  2005/05/13 11:34:04  ywkim
// Add Confirmation for flash_progress
//
// Revision 1.8  2005/05/13 05:30:22  ywkim
// disable verification for update
//
// Revision 1.7  2005/05/12 09:34:48  ywkim
// Change ProgramOneWord() for flash update
//
// Revision 1.6  2005/05/09 15:15:06  ywkim
// *** empty log message ***
//
// Revision 1.5  2005/05/03 11:39:43  ywkim
// use gpio for buzzer
//
// Revision 1.4  2005/03/06 11:09:39  ywkim
// Remake routine for verify
//
// Revision 1.3  2005/03/04 16:10:36  ywkim
// modify parameter of function for flash_progress
//
//*****************************************************************************
#include "LibFlash.h"

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#define MACHINE_TAVI 1
#if 1
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

// assume this is _after_ remap
#define FLASH_BASE_ADDRESS			0x400000
#define FLASH_BASE_ADDRESS_CFI		0
#define DWORD		unsigned long
#define WORD		unsigned short

#define REG_BYTE( addr ) ( *((volatile unsigned char*)(addr)) )
#define REG_WORD( addr ) ( *((volatile unsigned short int*)(addr)) )
#define REG_DWORD( addr ) ( *((volatile unsigned int*)(addr)) )

#define DISABLE_GLOBAL_INT	(*((volatile int *)0x00500224) &= ~0x80000000)
#define ENABLE_GLOBAL_INT	(*((volatile int *)0x00500224) |= 0x80000000)

static int cfi_initialized = 0;
static FLASH_PROGRESS flash_progress = 0;
static unsigned char cfi_data[0x100];
static int shift = 2;

static void cfi_wait_embedded_erase_done ()
{	
	sleep (1);
}

int cfi_query_parameters ()
{
	// query command
	*((volatile unsigned char*)(0x5555*shift+FLASH_BASE_ADDRESS_CFI)) = 0xAA;
	*((volatile unsigned char*)(0xAAAA*shift+FLASH_BASE_ADDRESS_CFI)) = 0x55;
	*((volatile unsigned char*)(0x5555*shift+FLASH_BASE_ADDRESS_CFI)) = 0x98;
	cfi_data[0x20] = *((volatile unsigned char*)(0x20+FLASH_BASE_ADDRESS));
	cfi_data[0x22] = *((volatile unsigned char*)(0x22+FLASH_BASE_ADDRESS));
	cfi_data[0x24] = *((volatile unsigned char*)(0x24+FLASH_BASE_ADDRESS));
	printf ("query->%c%c%c\n", cfi_data[0x20], cfi_data[0x22], cfi_data[0x24]);
	cfi_data[0x4e] = *((volatile unsigned char *)(0x4e + FLASH_BASE_ADDRESS));
	printf ("device size: %dMB\n", (1 << cfi_data[0x4e])/(1024*1024) );

	cfi_data[0x58] = *((volatile unsigned char *)(0x58 + FLASH_BASE_ADDRESS));
	printf ("number of regions: %d\n", cfi_data[0x58]);

	cfi_data[0x5a] = *((volatile unsigned char *)(0x5a + FLASH_BASE_ADDRESS));
	cfi_data[0x5c] = *((volatile unsigned char *)(0x5c + FLASH_BASE_ADDRESS));
	cfi_data[0x5e] = *((volatile unsigned char *)(0x5e + FLASH_BASE_ADDRESS));
	cfi_data[0x60] = *((volatile unsigned char *)(0x60 + FLASH_BASE_ADDRESS));
	printf ("region 1: %d * 0x%04x\n", cfi_data[0x5a]+1,
	((unsigned short)cfi_data[0x5c] <<  0) |
	((unsigned short)cfi_data[0x5e] <<  8) |
	((unsigned short)cfi_data[0x60] << 16));

	cfi_data[0x62] = *((volatile unsigned char *)(0x62 + FLASH_BASE_ADDRESS));
	cfi_data[0x64] = *((volatile unsigned char *)(0x64 + FLASH_BASE_ADDRESS));
	cfi_data[0x66] = *((volatile unsigned char *)(0x66 + FLASH_BASE_ADDRESS));
	cfi_data[0x68] = *((volatile unsigned char *)(0x68 + FLASH_BASE_ADDRESS));
	printf ("region 2: %d * 0x%04x\n", cfi_data[0x62]+1,
	((unsigned short)cfi_data[0x64] <<  0) |
	((unsigned short)cfi_data[0x66] <<  8) |
	((unsigned short)cfi_data[0x68] << 16));

	cfi_data[0x6a] = *((volatile unsigned char *)(0x6a + FLASH_BASE_ADDRESS));
	cfi_data[0x6c] = *((volatile unsigned char *)(0x6c + FLASH_BASE_ADDRESS));
	cfi_data[0x6e] = *((volatile unsigned char *)(0x6e + FLASH_BASE_ADDRESS));
	cfi_data[0x70] = *((volatile unsigned char *)(0x70 + FLASH_BASE_ADDRESS));
	printf ("region 3: %d * 0x%04x\n", cfi_data[0x6a]+1,
	((unsigned short)cfi_data[0x6c] << 16) |
	((unsigned short)cfi_data[0x6e] <<  8) |
	((unsigned short)cfi_data[0x70] <<  0));
	cfi_data[0x72] = *((volatile unsigned char *)(0x72 + FLASH_BASE_ADDRESS));
	cfi_data[0x74] = *((volatile unsigned char *)(0x74 + FLASH_BASE_ADDRESS));
	cfi_data[0x76] = *((volatile unsigned char *)(0x76 + FLASH_BASE_ADDRESS));
	cfi_data[0x78] = *((volatile unsigned char *)(0x78 + FLASH_BASE_ADDRESS));
	printf ("region 4: %d * 0x%06x\n", cfi_data[0x72]+1,
	((unsigned short)cfi_data[0x74] <<  0) |
	((unsigned short)cfi_data[0x76] <<  8) |
	((unsigned short)cfi_data[0x78] << 16));
	// reset command
	*((volatile unsigned char *)(FLASH_BASE_ADDRESS_CFI+0x5555*shift)) = 0xAA;
	*((volatile unsigned char *)(FLASH_BASE_ADDRESS_CFI+0x2AAA*shift)) = 0x55;
	*((volatile unsigned char *)(FLASH_BASE_ADDRESS_CFI+0x5555*shift)) = 0xF0; 
	return ((int)1 << cfi_data[0x4e]);
}

static void cfi_erase (int addr, int len, int base, int portion)
{
	int nSectors;
	int nSize;
	int nRemoveSector;
	int i;
	int sa;
	int s, e;
	int j;
		
	nSectors = ( WORD )( cfi_data[0x5A] | cfi_data[0x5C] << 8 ) + 1;
	nSize    = ( WORD )( cfi_data[0x5E] | cfi_data[0x60] << 8 ) * 256;
	printf( "Number of Sectors %d\n", nSectors ) ;
	printf( "Sector Size : %dKBytes\n", nSize/1024 );
	s = addr / nSize;
//	e = s + (len+nSize/2)/nSize;
	e = s + (len+nSize-1)/nSize;
	if( s == e ) e++;
	printf( "s -> %d, e -> %d, len->%d\n", s, e, len );

	for( i=s; i<e; i++ ) {
		DISABLE_GLOBAL_INT;
		REG_WORD( 0x5555*shift+FLASH_BASE_ADDRESS_CFI) = 0xaa;
		REG_WORD( 0x2AAA*shift+FLASH_BASE_ADDRESS_CFI) = 0x55;
		REG_WORD( 0x5555*shift+FLASH_BASE_ADDRESS_CFI) = 0x80;
		REG_WORD( 0x5555*shift+FLASH_BASE_ADDRESS_CFI) = 0xaa;
		REG_WORD( 0x2AAA*shift+FLASH_BASE_ADDRESS_CFI) = 0x55;
		addr = i*nSize;
		if( addr < 0x2000 )
			REG_WORD( FLASH_BASE_ADDRESS+addr ) = 0x0030;
		else
			REG_WORD( addr ) = 0x0030;
		for( j=0;j<1000000;j++);
			asm ("nop");
		ENABLE_GLOBAL_INT;
//		usleep( 80000 ); // wait max 50ms for erasing one sector
		printf( "erasing.....Sector Block #%d [0x0%08lx]\r", i, addr );
		if( flash_progress ) flash_progress( (int)( base +( portion*(i-s) )/(e-s)/2 ) );
	}
}


int Check_Toggle_Ready( DWORD dst, DWORD MaxCycles )
{
	WORD CurrData, PreData;
	DWORD  TimeOut = 0;
	DWORD addr;
	int ret = 0, i;

	addr = dst;
	if( addr < 0x2000 ) addr = FLASH_BASE_ADDRESS + addr;
	PreData = *(WORD *) (addr);      // read data
	PreData = PreData & 0x0040;                              // get DQ6
	while (TimeOut < MaxCycles)                          // check if time-out
	{
		CurrData = *(WORD *) (addr); // read again
		CurrData = CurrData & 0x0040;                        // retrieve bit DQ6 
		if (PreData == CurrData)
		{
			for( i=0; i<100; i++ ) __asm__ ( "nop" );
			ret = 1;
			break;
		}
		PreData = CurrData;
		TimeOut++;
	}
	if( ret == 0 ) printf( "failed to write\n" ), exit( 0 );
	return ret;   // timeout error
}

int ProgramOneWord( DWORD dst, WORD data )
{
	int ret;

	DISABLE_GLOBAL_INT;

	REG_WORD(0x5555*shift+FLASH_BASE_ADDRESS_CFI) = 0xaa;
	REG_WORD(0x2aaa*shift+FLASH_BASE_ADDRESS_CFI) = 0x55;
	REG_WORD(0x5555*shift+FLASH_BASE_ADDRESS_CFI) = 0xa0;
	if( dst< 0x2000 )
	{
		*((volatile WORD*)(FLASH_BASE_ADDRESS+dst)) = data;
	}
	else
	{
		*((volatile WORD*)(dst)) = data;
	}
	ret = Check_Toggle_Ready( dst, 500 );

	ENABLE_GLOBAL_INT;

	if( !ret ) {
		// once again
		DISABLE_GLOBAL_INT;

		REG_WORD(0x5555*shift+FLASH_BASE_ADDRESS_CFI) = 0xaa;
		REG_WORD(0x2aaa*shift+FLASH_BASE_ADDRESS_CFI) = 0x55;
		REG_WORD(0x5555*shift+FLASH_BASE_ADDRESS_CFI) = 0xa0;
		if( dst< 0x2000 )
		{
			*((volatile WORD*)(FLASH_BASE_ADDRESS+dst)) = data;
		}
		else
		{
			*((volatile WORD*)(dst)) = data;
		}
		ret = Check_Toggle_Ready( dst, 500 );

		ENABLE_GLOBAL_INT;
		// once again
		if ( !ret ) {
			printf( "failed to write one word\n" );
			fflush( stdout );
		}
	}

	return ret;
}

static void cfi_write (unsigned short *flashbuf, int start, int length, int base, int portion)
{
	int i;
	unsigned short *p = ( unsigned short* )flashbuf;
	unsigned char *cp = ( unsigned char* )flashbuf;

	printf( "cfi_write -> %d\n", length );
	if ((unsigned long)p & 1)
	{
		printf ("uh oh ...(%d)\n", (unsigned long)p );
		return;
	}


	for (i=0; i<length; i+=2)
	{
		if ((i % 0x10000) == 0)
		{
			printf ("				address 0x%08lx\r", i);
		}
		while( !ProgramOneWord( start+i, *p ) );
		if( !(i%1024) ) {
			if( flash_progress ) flash_progress( (int)(base + (portion/2)+((portion*i)/length/2)) );
			printf( "writing....%p %dkbytes\r", p, i/1024+1 );
		}
		p++;
	}
	printf( "writing....%p %dkbytes  address 0x%08lx\n", p, i/1024+1,i );

	// exit command
	*((volatile unsigned char *)(FLASH_BASE_ADDRESS_CFI+0x5555*shift)) = 0xaa;
	*((volatile unsigned char *)(FLASH_BASE_ADDRESS_CFI+0x2aaa*shift)) = 0x55;
	*((volatile unsigned char *)(FLASH_BASE_ADDRESS_CFI+0x5555*shift)) = 0xf0;
	// verify
	#if 0

	for( i=0; i<length; i++ ) {
		if( (start+i)<0x2000 ) {
			if( cp[i] != REG_BYTE( FLASH_BASE_ADDRESS+i ) ) {
				printf( "\noriginal: 0x%02lx, read: 0x%02lx\n", cp[i],
					REG_BYTE( FLASH_BASE_ADDRESS + i ) );
				break;
			}
		}
		else {
			if( cp[i] != REG_BYTE( start+i ) ) {
				printf( "\ni=0x%08lx, original: 0x%02lx, read: 0x%02lx\n",start+i, cp[i],
					REG_BYTE( start + i ) );
				break;
			}
		}
		if( !(i%1024) ) {
			if( flash_progress ) flash_progress( (int)( base + (portion*2/3)+((portion*i)/length/3)) );
			printf( "verifying...%dKBytes\r", i/1024+1 );
		}
	}

	printf("\n");
	printf("\n\n..............................................\n");
	fflush (stdout);
	#endif
}

int flash_init (void)
{
	cfi_query_parameters ();
	
	if ((cfi_data[0x20] == 'Q') &&
		(cfi_data[0x22] == 'R') &&
		(cfi_data[0x24] == 'Y'))
	{
		DEBUGMSG (1, ("cfi detected and initialized\n"));
		cfi_initialized = 1;
	}
	else {
		DEBUGMSG (1, ("cfi not detected\n"));
		cfi_initialized = 1;
		ASSERT(0);
		return -1;
	}

	return 0;
}

int flash_get_size() {
	ASSERT(cfi_initialized);
	if(!cfi_initialized) {
		DEBUGMSG (1, ("cfi not initialized\n"));
		return -1;
	}

	DEBUGMSG (1, ("flash size: %d\n", 1 << cfi_data[0x4e]));
	return 1 << cfi_data[0x4e];
}

int flash_erase (int start, int length, FLASH_PROGRESS progress, int base, int portion)
{  
	ASSERT (cfi_initialized);
	if (!cfi_initialized)
	{
		DEBUGMSG (1, ("cfi not initialized\n"));
		return -1;
	}
	flash_progress = progress;
	cfi_erase (start, length, base, portion);
	return 0;
}

int flash_write (unsigned short *p, int start, int length, FLASH_PROGRESS progress, int base, int portion)
{
	printf( "flash_write start\n" );
	printf( "cfi_initialized: %d\n", cfi_initialized );	
	cfi_wait_embedded_erase_done ();

	
	ASSERT (cfi_initialized);
	if (!cfi_initialized)
	{
		printf("cfi not initialized\n");
		return -1;
	}
	flash_progress = progress;
	printf("flash_write (0x%08lx, 0x%08lx, 0x%08lx)\n", 	p, start, length);
	cfi_write (p, start, length, base, portion);
	return 0;
}

int flash_exit (void)
{
	ASSERT (cfi_initialized);
	cfi_initialized = 0;
	return 0;
}

