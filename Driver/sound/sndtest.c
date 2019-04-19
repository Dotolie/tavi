#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include "sound.h"

int main( int argc, char** argv )
{
	int fd;
	int c;
	int onoff, format;
	struct snd_dev_t dev;

	fd = open( "/dev/misc/sound", O_RDWR );
	if( fd < 0 ) {
		fprintf( stderr, "failed to open\n" );
		return 1;
	}
	ioctl( fd, TAVI_IOCG_SND_REGISTERS, &dev );

	printf( "vol_l: %d, vol_r: %d\n", dev.lineout1.vol_left, dev.lineout1.vol_right );
	if( (c=getopt(argc, argv, "bflmvVe:t:s:T:B:")) != 1 ) {
		switch( c ) {
		case 'b':
			dev.linein.bypass = atoi( argv[2] );
			ioctl( fd, TAVI_IOCS_SND_INPUT_BYPASS, &dev );
		case 'f':
			dev.linein.audio_format = atoi( argv[2] );
			ioctl( fd, TAVI_IOCS_SND_AUDIO_FORMAT, &dev );
			break;
		case 'l':
			dev.linein.onoff = atoi( argv[2] );
			ioctl( fd, TAVI_IOCS_SND_LINEIN_ON, &dev );
			break;
		case 'm':
			dev.linein.micon = atoi( argv[2] );
			ioctl( fd, TAVI_IOCS_SND_MIC_ON, &dev );
			break;
		case 'v':
			dev.lineout1.vol_left = atoi( argv[2] );
			dev.lineout1.vol_right = atoi( argv[2] );
			ioctl( fd, TAVI_IOCS_SND_LINEOUT1_VOL, &dev );
			break;
		case 'V':
			dev.lineout2.vol_left = atoi( argv[2] );
			dev.lineout2.vol_right = atoi( argv[2] );
			ioctl( fd, TAVI_IOCS_SND_LINEOUT2_VOL, &dev );
			break;
		case 'e':
			format = atoi(optarg);
			dev.preset_eq = format;
			ioctl( fd, TAVI_IOCS_SND_EQUALIZER, &dev );
			break;
		case 't':
			sscanf( optarg, "%x", &format );
			dev.threed = format;
			ioctl( fd, TAVI_IOCS_SND_3D, &dev );
			break;
		case 's':
			format = atoi(optarg);
			dev.speaker = format;
			ioctl( fd, TAVI_IOCS_SND_SPEAKERAMP_ONOFF, &dev );
			break;
		case 'T': // treble
			sscanf( optarg, "%x", &format );
			dev.treble = format;
			ioctl( fd, TAVI_IOCS_SND_TREBLE, &dev );
			break;
		case 'B': // Bass
			sscanf( optarg, "%x", &format );
			dev.bass = format;
			ioctl( fd, TAVI_IOCS_SND_BASS, &dev );
			break;
		}
	}

	return 0;
}
