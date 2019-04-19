#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "tavi_tp.h"
#include "meplib.h"

int main( int argc, char** argv )
{
#if 0
	int fd, i, nbytes;
	char cmd[8];
	char buf[8];

	fd = open( "/dev/misc/tp", O_RDWR );

	printf( "Active Mode\n" );
	cmd[0] = 0x19;
	write( fd, cmd, 1 );
	nbytes = read( fd, buf, 8 );
	for( i=0; i<nbytes; i++ ) {
		printf( "buf[%d] -. 0x%02lx\n", i, buf[i] );
	}
	printf( "Disable Auto reporting\n" );
	cmd[0] = 0x1c;
	write( fd, cmd, 1 );
	nbytes = read( fd, buf, 8 );
	for( i=0; i<nbytes; i++ ) {
		printf( "buf[%d] -. 0x%02lx\n", i, buf[i] );
	}

	printf( "MEP Query: 0x00\n" );
	cmd[0] = 0x11;
	cmd[1] = 0x80;
	write( fd, cmd, 2 );
	nbytes = read( fd, buf, 8 );
	for( i=0; i<nbytes; i++ ) {
		printf( "buf[%d] -. 0x%02lx\n", i, buf[i] );
	}

	printf( "MEP Query: 0x01\n" );
	cmd[0] = 0x11;
	cmd[1] = 0x81;
	write( fd, cmd, 2 );
	nbytes = read( fd, buf, 8 );
	for( i=0; i<nbytes; i++ ) {
		printf( "buf[%d] -. 0x%02lx\n", i, buf[i] );
	}
	usleep( 100000 );

	printf( "MEP Parameter: 0x00\n" );
	cmd[0] = 0x11;
	cmd[1] = 0x40;
	write( fd, cmd, 2 );
	nbytes = read( fd, buf, 8 );
	for( i=0; i<nbytes; i++ ) {
		printf( "buf[%d] -. 0x%02lx\n", i, buf[i] );
	}
	usleep( 100000 );

#if 0
	printf( "MEP Parameter: 0x20\n" );
	cmd[0] = 0x11;
	cmd[1] = 0x60;
	write( fd, cmd, 2 );
	nbytes = read( fd, buf, 8 );
	for( i=0; i<nbytes; i++ ) {
		printf( "buf[%d] -. 0x%02lx\n", i, buf[i] );
	}
#endif


#if 1
	usleep( 100000 );
	printf( "Enable Auto reporting\n" );
	cmd[0] = 0x1b;
	write( fd, cmd, 1 );
	nbytes = read( fd, buf, 8 );
	for( i=0; i<nbytes; i++ ) {
		printf( "buf[%d] -. 0x%02lx\n", i, buf[i] );
	}
	
	while( 1 ) {
		if( (nbytes=read( fd, buf, 8)) <= 0 ) {
			usleep( 10000 );
			continue;
		}
		for( i=0; i<nbytes; i++ ) {
			printf( "buf[%d] -. 0x%02lx\n", i, buf[i] );
		}
	}
#endif
#else
	char buf[8];
	struct Mep_1D_Coord coord;

	if( mep_init() < 0 ) {
		printf( "failed to initialize\n" );
		return 1;
	}

	if( mep_report( MEP_REPORT_ABSOLUTE ) == 0 ) {
		printf( "failed to change report mode(%d)\n", MEP_REPORT_RELATIVE );
	}

	if( mep_enable_autoreport( 0 ) == 0 ) {
		printf( "failed to enable auto report\n" );
		return 1;
	}

	while( 1 ) {
		if( mep_recv_packet( buf, 8 ) <= 0 ) {
			usleep( 100000 );
			continue;
		}
		
		mep_interpret_packet( buf, &coord );
		printf( "x : %d, click: %d, z: %d, finger: %d\n", 
				coord.x, coord.click, coord.z, coord.finger );
	}
#endif
	return 0;
}
