#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "tavi_key.h"
#include "../misc/misc_ctrl.h"

#define KEYDRV_PATH		"/dev/misc/tkey"

int main( int argc, char** argv )
{
	int fd, fd2;
	struct keystruct key;

	fd = open( KEYDRV_PATH, O_RDONLY|O_NONBLOCK );
	if( fd < 0 ) {
		fprintf( stderr, "failed to open %s\n", KEYDRV_PATH );
		return 1;
	}

	ioctl( fd, TAVI_KEY_IOCS_REPEATMODE, 1 );

	while( 1 ) {
		memset( (void*)&key, 0, sizeof(key) );
		read( fd, &key, sizeof(key) );
		switch( key.key ) {
			#if 1
		case TAVI_KEY_EXIT:
			printf( "TAVI_KEY_EXIT pressed\n" );
			break;
		case TAVI_KEY_DOWN:
			printf( "TAVI_KEY_DOWN pressed\n" );
			break;
		case TAVI_KEY_RIGHT:
			printf( "TAVI_KEY_RIGHT pressed\n" );
			break;
		case TAVI_KEY_LEFT:
			printf( "TAVI_KEY_LEFT pressed\n" );
			break;
		case TAVI_KEY_UP:
			printf( "TAVI_KEY_UP pressed\n" );
			break;
		case TAVI_KEY_ENTER:
			printf( "TAVI_KEY_ENTER pressed\n" );
			break;
		case TAVI_KEY_AB:
			printf( "TAVI_KEY_AB pressed\n" );
			break;
		case TAVI_KEY_MODE:
			printf( "TAVI_KEY_MODE pressed\n" );
			break;
		#if 0
		case TAVI_KEY_JOG1_UP:
			printf( "TAVI_KEY_JOG_UP pressed\n" );
			break;
		case TAVI_KEY_JOG1_DOWN:
			printf( "TAVI_KEY_JOG_DOWN pressed\n" );
			break;
		case TAVI_KEY_JOG_MENU:
			printf( "TAVI_KEY_JOG_MENU pressed\n" );
			break;
		case TAVI_KEY_JOG_RIGHT:
			printf( "TAVI_KEY_JOG_RIGHT pressed\n" );
			break;
		case TAVI_KEY_JOG_LEFT:
			printf( "TAVI_KEY_JOG_LEFT pressed\n" );
			break;
		case TAVI_KEY_JOG_PLAYSTOP:
			printf( "TAVI_KEY_JOG_PLAYSTOP pressed\n" );
			break;
		#endif
		case TAVI_KEY_LEQ:
			printf( "TAVI_KEY_LEQ\n" );
			break;
		case TAVI_KEY_LMODE:
			printf( "TAVI_KEY_LMODE\n" );
			break;
		case TAVI_KEY_LEXIT:
			printf( "TAVI_KEY_LEXIT\n" );
			break;
		case TAVI_KEY_LLEFT:
			printf( "TAVI_KEY_LLEFT\n" );
			break;
		case TAVI_KEY_LRIGHT:
			printf( "TAVI_KEY_LRIGHT\n" );
			break;
		case TAVI_KEY_LENTER:
			printf( "TAVI_KEY_LENTER\n" );
			break;
		case TAVI_KEY_EQ:
			printf( "TAVI_KEY_EQ\n" );
			break;
		case TAVI_KEY_MENU:
			printf( "TAVI_KEY_MENU\n" );
			break;
		case TAVI_KEY_HOLD:
			printf( "TAVI_KEY_HOLD\n" );
			break;
			#endif
		case TAVI_KEY_SHUTDOWN:
			break;
		default:
		}
	}

	return 0;
}
