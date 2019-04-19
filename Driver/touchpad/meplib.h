/*
 * MEP Library
 *
 * All Copyright(C) 2004 New Media Life Inc.
 * 
 * Author: Lee Seok<zstein@newmedialife.com>
 * Update:
 */
 
#ifndef __MEP_LIB_H__
#define __MEP_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GET_PACKET_LENGTH( _packet ) 			( _packet[0] & 0x07 )
#define GET_PACKET_CONTROL( _packet ) 			( (_packet[0]>>3)&0x03 )
#define SET_PACKET_GLOBAL( _packet ) 			( _packet[0] |= 0x10 )
#define SET_PACKET_LENGTH( _packet, _len )		( _packet[0] = (_packet[0]&0xf8) | _len&0x07 )
#define SET_MODULE_ADDRESS( _packet, _addr ) 	( _packet[0] = (_packet[0]&0x1f) | _addr<<5 )

#define PACKET_ID_ERROR		0
#define PACKET_ID_HELLO		1
#define PACKET_ID_ACK		2

/*
 * Short Command Set
 */
#define SHORT_CMD( _packet ) ( _packet[0] |= 0x08 )
#define SHORT_CMD_RESET( _packet ) \
({ SHORT_CMD( _packet ); \
   _packet[0] |= 0x00; \
 })
#define SHORT_CMD_ACTIVE( _packet ) \
({ SHORT_CMD( _packet ); \
   _packet[0] |= 0x01; \
 })
#define SHORT_CMD_SLEEP( _packet ) \
({ SHORT_CMD( _packet ); \
   _packet[0] |= 0x02; \
 })
#define SHORT_CMD_ENABLE_AUTOREPORTING( _packet ) \
({ SHORT_CMD( _packet ); \
   _packet[0] |= 0x03; \
 })
#define SHORT_CMD_DISABLE_AUTOREPORTING( _packet ) \
({ SHORT_CMD( _packet ); \
   _packet[0] |= 0x04; \
 })
#define SHORT_CMD_POLLREPORTING( _packet ) \
({ SHORT_CMD( _packet ); \
   _packet[0] |= 0x05; \
 })

#define LONG_CMD( _packet ) ( _packet[0] &= 0xf7 )
#define LONG_CMD_DEFAULT_STATE( _packet ) \
({ int len = 2; \
   LONG_CMD( _packet ); \
   SET_PACKET_GLOBAL( _packet ); \
   SET_PACKET_LENGTH( _packet, 1 ); \
   _packet[1] |= 0x08; \
   len; \
 }) 
#define LONG_CMD_GET_PARAMETER( _packet, _param ) \
({ int len = 2; \
   LONG_CMD( _packet ); \
   SET_PACKET_GLOBAL( _packet ); \
   SET_PACKET_LENGTH( _packet, 1 ); \
   _packet[1] = _param + 0x40; \
   len; \
 })
#define LONG_CMD_SET_PARAMETER( _packet, _param, _value ) \
({ int len = 4; \
   LONG_CMD( _packet ); \
   SET_PACKET_GLOBAL( _packet ); \
   SET_PACKET_LENGTH( _packet, 3 ); \
   _packet[1] = _param + 0x40; \
   _packet[2] = (_value&0xff00)>>8; \
   _packet[3] = (_value&0xff); \
   len; \
 })
#define MEP_QUERY( _packet, _cmd ) \
({ int len = 2; \
   LONG_CMD( _packet ); \
   SET_PACKET_GLOBAL( _packet ); \
   SET_PACKET_LENGTH( _packet, 1 ); \
   _packet[1] = _cmd + 0x80; \
   len; \
 })

typedef enum {
	MEP_VELOCITY_UNKNOWN=(-1),
	MEP_VELOCITY_10,
	MEP_VELOCITY_20,
	MEP_VELOCITY_40,
	MEP_VELOCITY_80
} MEP_REL_VELOCITY;


struct mep_property_t 
{
	// Base Information
	char Manufacture_id;
	char MEP_Major_ver;
	char MEP_Minor_ver;
	char Class_Major_ver;
	char Class_Minor_ver;
	char Module_ClassID;
	char SubClass_Major_ver;
	char SubClass_Minor_ver;
	char Module_SubClassID;
	// Product Information
	char Module_Major_ver;
	char Module_Minor_ver;
	short int ProductID;
	char CanDoze;
	char GuestPort;
	// MEP Parameter
	char AutoReport;
	char NoDoze;
	char SleepMode;
	char Hibernate;
	char SensoryWakeup;
	char EventWakeup;
	// MEP Report mode
#define MEP_REPORT_ABSOLUTE			0x01
#define MEP_REPORT_RELATIVE			0x02
	char ReportMode;
#define MEP_RATE_UNKNOWN			0x00
#define MEP_RATE_40HZ				0x01
#define MEP_RATE_80HZ				0x10
	char Rate;
	char NoFilter;
	char Button;
	short int CurPos;
	short int Velocity;
} __attribute__((packed));

/* Error Code */
enum Enum_TouchPad_Error {
	TP_Error_General_Error,
	TP_Error_Bad_Command,
	TP_Error_Bad_Command_Parameter,
	TP_Error_Incomplete_Transmission,
	TP_Error_Address,
	TP_Error_Parity
};

struct Mep_1D_Coord {
	int x;
	int y; // reserved
	int click;
	int z;
	int finger;
};

// Hostward packet definitions
static int inline is_hello( const char* packet )
{
	if( GET_PACKET_CONTROL( packet ) == 0x03 && packet[1] == 0x10 ) return 1;
	else return 0;
}

static int inline is_ack( const char* packet, int single )
{
	if( GET_PACKET_CONTROL( packet ) == 0x03 ) {
		if( single ) return 1;
		else {
			if( (packet[1]>>4) == 0x02 ) return 1;
		}
	}	

	return 0;
}

static int inline is_error( const char* packet )
{
	if( GET_PACKET_CONTROL( packet ) == 0x03 && (packet[0]&0x01) == 0x01 ) {
		if( (packet[1]&0xf0)>>4 == PACKET_ID_ERROR ) return 1;
	}

	return 0;
}	

extern int mep_init( void );
extern int mep_exit( void );
extern struct mep_property_t mep_get_property( void );
extern int mep_send_cmd( const char* cmd, int size );
extern int mep_recv_packet( char* buf, int size );
extern int mep_enable_autoreport( int onoff );
extern int mep_interpret_packet( const char* packet, struct Mep_1D_Coord* coord );
extern int mep_report( int mode );
extern void mep_eat_event( void );
extern int mep_is_start( void );
extern int mep_is_end( void );
extern int mep_get_step( int* click, int* start );
extern void mep_set_velocity( MEP_REL_VELOCITY velocity );
extern MEP_REL_VELOCITY mep_get_velocity( void );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MEP_LIB_H__ */


