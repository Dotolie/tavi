//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Touchpad library
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/touchpad/meplib.c,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: meplib.c,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/08/25 06:04:56  ywkim
// Merge to main trunk
//
// Revision 1.1.1.1.2.18  2005/06/29 07:23:11  zstein
// - fixed bug
//
// Revision 1.1.1.1.2.17  2005/06/24 05:43:11  zstein
// - fixed bug.
//
// Revision 1.1.1.1.2.16  2005/06/24 05:22:40  zstein
// - fixed bugs.
//
// Revision 1.1.1.1.2.15  2005/06/24 05:10:04  zstein
// - bug fixed.
//
// Revision 1.1.1.1.2.14  2005/06/24 01:21:38  zstein
// - send end signal when invalid end.
//
// Revision 1.1.1.1.2.13  2005/06/16 23:51:01  zstein
// - bug fixed.
//
// Revision 1.1.1.1.2.12  2005/06/15 12:41:34  etnlwind
// Update for Video Touch Seek Process
//
// Revision 1.1.1.1.2.11  2005/06/15 11:51:30  etnlwind
// Update for Video Player Touch Control And Background Savefile Process
//
// Revision 1.1.1.1.2.10  2005/06/15 01:25:20  zstein
// - changed touch I/F.
//
// Revision 1.1.1.1.2.9  2005/06/14 09:24:19  zstein
// - added void mep_set_velocity( MEP_REL_VELOCITY velocity ) and
//         MEP_REL_VELOCITY mep_get_velocity( void ).
//
// Revision 1.1.1.1.2.8  2005/06/14 08:24:54  zstein
// - changed touchpad I/F
//
// Revision 1.1.1.1.2.7  2005/06/03 01:15:30  zstein
// - added mep_is_start() and mep_is_end() functions.
//
// Revision 1.1.1.1.2.6  2005/05/27 09:10:22  zstein
// - updated
//
// Revision 1.1.1.1.2.5  2005/05/19 01:39:40  zstein
// - added mep_eat_event(...)
//
// Revision 1.1.1.1.2.4  2005/05/12 04:35:31  zstein
// - deleted debug message.
//
// Revision 1.1.1.1.2.3  2005/04/28 05:58:41  zstein
// demo merge.
//
// Revision 1.1.1.1.2.2  2005/04/22 12:29:55  zstein
// added error exception.
//
// Revision 1.1.1.1.2.1  2005/04/14 23:50:10  zstein
// Added an exception routine when device doesn't work.
//
//
//*******************************************************************************

#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "tavi_tp.h"
#include "meplib.h"

#define TOUCHPAD_PATH			"/dev/misc/tp"
#define LOW						0
#define HIGH						1

static int mep_handle;
static char cmd[8];
static char rcv[8];
static struct mep_property_t mep_property;
static int s_Start;
static int s_Ing;
static int s_End;

#define DISPLAY_MEP_INFORMATION		1
int mep_init( void )
{
	struct tp_link_state_t state;
	int hello;
	int len;

	mep_handle = open( TOUCHPAD_PATH, O_RDWR );
	if( mep_handle < 0 ) return -1;

	// Need to know link state
	ioctl( mep_handle, TAVI_IOCGLINK, &state );
	if( state.data == LOW && state.clock == LOW && state.ack == HIGH ) {
		ioctl( mep_handle, TAVI_IOCFLUSH );
		ioctl( mep_handle, TAVI_IOCGHELLO, &hello );
		if( hello ) {
			printf( "ScrollStrip said HELLO\n" );
		}
	}

	// get base information
	SET_MODULE_ADDRESS( cmd, 0x00 );
	len = MEP_QUERY( cmd, 0x00 );
	write( mep_handle, cmd, len );
	len = read( mep_handle, rcv, 8 );	

	if( is_ack(rcv, 0) == 0 ) {
		close( mep_handle );
		mep_handle = -1;
		return -1;
	}

	mep_property.Manufacture_id = rcv[2];
	mep_property.MEP_Major_ver = (rcv[3]&0xf0) >> 4;
	mep_property.MEP_Minor_ver = rcv[3]&0x0f;
	mep_property.Class_Major_ver = (rcv[4]&0xf0) >> 4;
	mep_property.Class_Minor_ver = rcv[4] & 0x0f;
	mep_property.Module_ClassID = rcv[5];
	mep_property.SubClass_Major_ver = (rcv[6]&0xf0) >> 4;
	mep_property.SubClass_Minor_ver = rcv[6] & 0x0f;
	mep_property.Module_SubClassID = rcv[7];

	len = MEP_QUERY( cmd, 1 );
	write( mep_handle, cmd, len );
	len = read( mep_handle, rcv, 8 );

	if( is_ack(rcv, 0) == 0 ) {
		close( mep_handle );
		mep_handle = -1;
		return -1;
	}

	mep_property.Module_Major_ver = rcv[2];
	mep_property.Module_Minor_ver = rcv[3];
	mep_property.ProductID = ((short int)rcv[4])<<8 | rcv[5];
	mep_property.CanDoze = rcv[7] & 0x02;
	mep_property.GuestPort = rcv[7] & 0x01;

	// Get MEP configuration
	len = LONG_CMD_GET_PARAMETER( cmd, 0x00 );
	write( mep_handle, cmd, len );
	len = read( mep_handle, rcv, 8 );
	if( is_ack(rcv, 0) == 0 ) {
		close( mep_handle );
		mep_handle = -1;
		return -1;
	}
	mep_property.AutoReport = rcv[3]&0x80 ? 1 : 0;
	mep_property.NoDoze = rcv[3]&0x10 ? 1 : 0;
	mep_property.SleepMode = rcv[3]&0x08 ? 1 : 0;
	mep_property.Hibernate = rcv[3]&0x04 ? 1 : 0;
	mep_property.SensoryWakeup = rcv[3]&0x02 ? 1 : 0;
	mep_property.EventWakeup = rcv[3]&0x01 ? 1 : 0;

	// Get report mode
	len = LONG_CMD_GET_PARAMETER( cmd, 0x20 );
	write( mep_handle, cmd, len );
	len = read( mep_handle, rcv, 8 );
	if( is_ack(rcv, 0) == 0 ) {
		close( mep_handle );
		mep_handle = -1;
		return -1;
	}
	mep_property.ReportMode = (rcv[3]&0x03) == 0x01 ? MEP_REPORT_ABSOLUTE : MEP_REPORT_RELATIVE;
	mep_property.Rate = (rcv[3]&0xc0)>>6;
	mep_property.NoFilter = rcv[3]&0x20 ? 1 : 0;
	mep_property.Button = rcv[3]&0x40 ? 1 : 0;

	#if DISPLAY_MEP_INFORMATION
	printf( "Touchpad Information\n" );
	printf( "Manufacture ID: %d\n", mep_property.Manufacture_id );
	printf( "MEP Version   : %d.%d\n", mep_property.MEP_Major_ver, mep_property.MEP_Minor_ver );
	printf( "Class Version : %d.%d\n", mep_property.Class_Major_ver, mep_property.Class_Minor_ver );
	printf( "Module ClassID: %d\n", mep_property.Module_ClassID );
	printf( "Sub Class Ver : %d.%d\n", mep_property.SubClass_Major_ver, mep_property.SubClass_Minor_ver );
	printf( "Module Subclass ID: %d\n", mep_property.Module_SubClassID );
	printf( "Module Version : %d.%d\n", mep_property.Module_Major_ver, mep_property.Module_Minor_ver );
	printf( "Product ID    : 0x%04lx\n", mep_property.ProductID );
	printf( "Can Doze      : %d\n", mep_property.CanDoze );
	printf( "Guest Port    : %d\n", mep_property.GuestPort );
	printf( "Auto Reporting: %d\n", mep_property.AutoReport );
	printf( "No Doze       : %d\n", mep_property.NoDoze );
	printf( "Sleep Mode    : %d\n", mep_property.SleepMode );
	printf( "Hibernate     : %d\n", mep_property.Hibernate );
	printf( "SensoryWakeup : %d\n", mep_property.SensoryWakeup );
	printf( "EventWakeup   : %d\n", mep_property.EventWakeup );
	printf( "ReportMode    : %d\n", mep_property.ReportMode );
	printf( "Rate          : %d\n", mep_property.Rate );
	printf( "No Filter     : %d\n", mep_property.NoFilter );
	printf( "Button        : %d\n", mep_property.Button );
	#endif /* DISPLAY_MEP_INFORMATION */

	printf( "MEP SUCCESS!!\n" );
	mep_property.CurPos = 0;
	mep_property.Velocity = 80;
	
	return 0;
}

int mep_exit( void )
{
	close( mep_handle );

	return 0;
}


struct mep_property_t mep_get_property( void )
{
	return mep_property;
}

int mep_send_cmd( const char* cmd, int size )
{
	if( mep_handle < 0 ) return -1;
	return write( mep_handle, cmd, size ); 
}

int mep_recv_packet( char* buf, int size )
{
	if( mep_handle < 0 ) {
		return mep_init();
		//return -1;
	}
	return read( mep_handle, buf, size );
}

// return true on success, otherwise return zero.
int mep_enable_autoreport( int onoff )
{
	char cmd[8] = {0,};
	char buf[8] = {0,};
	int len, i;

	if( mep_handle < 0 ) return -1;
	
	SET_MODULE_ADDRESS( cmd, 0x00 );
	SET_PACKET_GLOBAL( cmd );
	SHORT_CMD_ENABLE_AUTOREPORTING( cmd );
	write( mep_handle, cmd, 1 );
	len = read( mep_handle, buf, 8 );
	
	return is_ack( buf, 1 );
}

int mep_report( int coord )
{
	char cmd[8] = {0,};
	char rcv[8] = {0,};
	int len;
	int ack, i;

	if( mep_handle < 0 ) return -1;
	
	if( mep_property.ReportMode == coord ) return;

	if( coord > MEP_REPORT_RELATIVE ) return 0;

	len = LONG_CMD_SET_PARAMETER( cmd, 0x20, 
			coord|mep_property.Rate|mep_property.NoFilter|mep_property.Button );
	write( mep_handle, cmd, len );
	len = read( mep_handle, rcv, 8 );

	ack = is_ack( rcv, 1 );
	if( ack ) mep_property.ReportMode = coord;

	return ack;
}

// On success, returns 1 otherwise return zero.
static int interpret_abs( const char* packet, struct Mep_1D_Coord* coord )
{
	int i;
	if( mep_property.Module_SubClassID == 0x01 ) {
		if( (packet[0]&0x08 != 0x08) && (packet[0]&0x07 != 3) ) return -1;
	}
	else if( mep_property.Module_SubClassID = 0x02 ) {
		if( (packet[0]&0x08 != 0x08) && (packet[0]&0x07 != 4) ) return -1;
	}
	coord->x = ((unsigned short)(packet[1]&0xf0))<<4 | packet[2];
	coord->click = packet[1]&0x02 ? 1 : 0;
	coord->z = (packet[3]&0x3f);
	coord->finger = (packet[2]&0x01);

	if( !coord->x && !coord->click && !coord->z && !coord->finger ) {
		if( s_Start ) {
			s_End = 1;
			s_Start = 0;
			s_Ing = 0;
		}
		else {
			s_Start = 1;
			s_Ing = 0;
			s_End = 0;
		}
	}
	else s_Ing = 1;

	return 0;
}

static int interpret_rel( const char* packet, struct Mep_1D_Coord* coord )
{
	if( (packet[0]&0x18)>>3 != 3 ) return 0;

	coord->x = (signed char)packet[2];
	coord->click = packet[1] & 0x02 ? 1 : 0;
	coord->finger = packet[2]&0x01;

	if( !coord->x && !coord->click && !coord->finger ) {
		if( s_Start && s_Ing ) {
			s_End = 1;
			s_Start = 0;
			s_Ing = 0;
		}
		else {
			s_Start = 1;
			s_Ing = 0;
			s_End = 0;
		}
	}
	else {
		if( !s_Start && s_End ) {
			s_Start = 1;
			s_End = 0;
			s_Ing = 0;
		}
		else {
			s_Ing = 1;
		}
	}
	
	return 0;
}

int mep_interpret_packet( const char* packet, struct Mep_1D_Coord* coord )
{
	if( !coord ) return -1;

	if( mep_property.ReportMode == MEP_REPORT_ABSOLUTE ) 
		return interpret_abs( packet, coord );
	else
		return interpret_rel( packet, coord );
}

void mep_eat_event( void )
{
	char sz[8];
	while( mep_recv_packet(sz, sizeof(sz)) > 0 ) ;
}

int mep_is_start( void ) 
{
	return (s_Start && !s_Ing);
}

int mep_is_end( void )
{
	return s_End;
}

int mep_get_step( int* click, int* start )
{
	char buf[8];
	struct Mep_1D_Coord coord;
	int ret = -2;
	int velocity = mep_property.Velocity;

	if( mep_handle < 0 ) return -2;

	if( mep_recv_packet( buf, sizeof(buf) ) <= 0 ) {
		#if 0
		if( s_Ing ) {
			*click = 0;
			*start = 0;
			coord.x = 0;
			coord.click = 0;
			s_End = 1;
			s_Ing = 0;
			goto __end_packet;
		}
		#endif
		return -2;
	}

	memset( (void*)&coord, 0, sizeof(coord) );
	mep_interpret_packet( buf, &coord );
	
	if( mep_is_start() ) {
		mep_property.CurPos = 0;
		*start = 1;
	}
	else {
		*start = 0;
	}

	if( coord.click ) {
		*click = 1;
		ret = 0;
		if( mep_property.ReportMode == MEP_REPORT_ABSOLUTE ) {
			if( mep_property.CurPos < 1300 && mep_property.CurPos > 100 ) ret = 1;
			else if( mep_property.CurPos > 1430 && mep_property.CurPos < 2660 ) ret = 2;
			else if( mep_property.CurPos > 2800 ) ret = 3;
		}
		mep_property.CurPos = 0;
	}
	else {
		*click = 0;
		if( mep_property.ReportMode == MEP_REPORT_ABSOLUTE ) {
			if( coord.x ) mep_property.CurPos = coord.x;
		}
		else {
			mep_property.CurPos += coord.x;
			if( mep_property.CurPos > velocity ) {
				ret = 1;
				mep_property.CurPos = 0;
			}
			else if( mep_property.CurPos < -velocity ) {
				ret = -1;
				mep_property.CurPos = 0;
			}
		}
	}
	

	#if 0
	if( mep_is_end() ) {
		ret = 0;
		printf( "MEP_END\n" );
	}
	#endif
__end_packet:	
	if( coord.x == 0 && coord.click == 0 ) {
		//printf( "clear..\n" );
		ret = 0;
	}
	//if( coord.x == 0 ) ret = 0;
//	printf( "x: %d, curpos: %d, click: %d, ret: %d, finger: %d\n", 
//		coord.x, mep_property.CurPos, coord.click, ret, coord.finger );

	return ret;
}

void mep_set_velocity( MEP_REL_VELOCITY velocity )
{
	switch( velocity ) {
	case MEP_VELOCITY_10: mep_property.Velocity = 10; break;
	case MEP_VELOCITY_20: mep_property.Velocity = 20; break;
	case MEP_VELOCITY_40: mep_property.Velocity = 40; break;
	case MEP_VELOCITY_80: mep_property.Velocity = 80; break;
	default: mep_property.Velocity = 80;
	}
}

MEP_REL_VELOCITY mep_get_velocity( void )
{
	MEP_REL_VELOCITY velocity;
	
	switch( mep_property.Velocity ) {
	case 10: velocity = MEP_VELOCITY_10; break;
	case 20: velocity = MEP_VELOCITY_20; break;
	case 40: velocity = MEP_VELOCITY_40; break;
	case 80: velocity = MEP_VELOCITY_80; break;
	default: velocity = MEP_VELOCITY_UNKNOWN; break;
	}

	return velocity;
}