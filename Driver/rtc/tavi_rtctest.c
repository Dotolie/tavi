//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Real Time Clock (PCF8563) test prog for TAVI
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/rtc/tavi_rtctest.c,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: tavi_rtctest.c,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2006/01/16 02:42:13  zstein
// *** empty log message ***
//
// Revision 1.4  2005/11/10 01:11:30  zstein
// *** empty log message ***
//
// Revision 1.3  2005/08/12 03:12:53  zstein
// - T0
//
// Revision 1.2  2005/04/22 12:29:29  zstein
// *** empty log message ***
//
// Revision 1.1  2005/03/30 06:13:10  ywkim
// Initially add
//
//*****************************************************************************

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "tavi_rtc.h"
#include "../misc/misc_ctrl.h"

int SetAlarm( int handle, int min )
{
	struct rtc_time rtime;
	memset( (void*)&rtime, 0, sizeof(rtime) );
	rtime.tm_min = min;
	ioctl( handle, RTC_ALM_SET, &rtime );
}

int main( int argc, char** argv )
{
	int fd, misc_fd;
	struct rtc_time rtime;
	int century;
	int year;
	int min;
	struct misc_ctrl_struct dev;

	printf( "rtc test program...\n" );
	if( (fd = open("/dev/misc/rtc", O_RDONLY)) < 0 ) {
		printf( "failed to open /dev/misc/rtc\n" );
		return 1;
	}

#if 1
	if( (misc_fd = open("/dev/misc/tmisc", O_RDONLY)) < 0 ) {
		fprintf( stderr, "failed to open /dev/misc/tmisc\n" );
		return 1;
	}

	ioctl( misc_fd, TAVI_MISC_IOCG_CONTROL, &dev );
	if( dev.ctrl_in&(1<<RTC_INT) ) {
		printf( "rtc interrupted\n" );
	}
#endif
	ioctl( fd, RTC_CLEAR_INT );

#if 1
	rtime.tm_century = 1;
	rtime.tm_year = 5; /* 2004 */
	rtime.tm_mon  = 4;
	rtime.tm_mday = 19; /* 1..31 */
	rtime.tm_hour = 0;
	rtime.tm_min  = 0;
	rtime.tm_sec  = 0; 
	rtime.tm_wday = 2;
	ioctl( fd, RTC_SET_TIME, &rtime );
#endif

	min = 1;
	SetAlarm( fd, min );

	while( 1 ) {
		usleep( 100000 );
		ioctl( fd, RTC_RD_TIME, &rtime );
		century = rtime.tm_century;
		year = rtime.tm_year-100;
		if( century ) year += 2000;
		else year += 1900;
		printf( "%04ld-%02ld-%02ld, %02ld:%02ld:%02ld, %02ldweek days, vl:%d\r",
				year, rtime.tm_mon&0x7f, rtime.tm_mday,
				rtime.tm_hour, rtime.tm_min, rtime.tm_sec,
				rtime.tm_wday, rtime.tm_vl );
		ioctl( misc_fd, TAVI_MISC_IOCG_CONTROL, &dev );
		if( !(dev.ctrl_in&(1<<RTC_INTR)) ) {
			printf( "\nRTC INTERRUPT\n" );
			ioctl( fd, RTC_CLEAR_INT );
			min++;
			SetAlarm( fd, min );
		}
	}

	return 0;
}
