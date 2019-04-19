#ifndef __TAVI_TOUCHPAD_H__
#define __TAVI_TOUCHPAD_H__

#include <asm/ioctl.h>

#define TAVI_TOUCH_MAGIC			'T'
#define TAVI_IOCSDATA				_IOW( TAVI_TOUCH_MAGIC, 1, int )
#define TAVI_IOCSACK				_IOW( TAVI_TOUCH_MAGIC, 2, int )
#define TAVI_IOCGCLK				_IOR( TAVI_TOUCH_MAGIC, 3, int )
#define TAVI_IOCGDATA				_IOR( TAVI_TOUCH_MAGIC, 4, int )
#define TAVI_IOCGLINK				_IOR( TAVI_TOUCH_MAGIC, 5, struct tp_link_state_t )
#define TAVI_IOCFLUSH				_IO( TAVI_TOUCH_MAGIC, 6 )
#define TAVI_IOCGHELLO				_IOR( TAVI_TOUCH_MAGIC, 7, int )

struct touchpad_phy
{
	void( *set_ack )( int );
	void( *set_data )( int );
	int( *get_data )( void );
	int( *get_clk )( void );
	int( *get_ack )( void );
};

struct tp_link_state_t 
{
	int clock;
	int data;
	int ack;
};

#endif /* __TAVI_TOUCHPAD_H__ */

