/*
 * $Id: tavi_key.h,v 1.2 2006/04/15 02:18:27 zstein Exp $
* 
 * TAVI PMP key module support
 * 
 * Copyright(C) 2004, New Media Life Inc, 
 *
 * Author: Lee Seok<zstein@newmedialife.com>
 * Update: 
 *	- 12/17/2004: swaped between TAVI_KEY_MODE and TAVI_KEY_MENU by zstein
 * 
 * $Log: tavi_key.h,v $
 * Revision 1.2  2006/04/15 02:18:27  zstein
 * - removed TAVI_KEY_HOLDOFF.
 *
 * Revision 1.1.1.1  2006/02/22 12:33:31  zstein
 * TAVI020 REBUILD
 *
 * Revision 1.13  2005/12/26 04:55:15  zstein
 * - added TAVI_KEY_IOCS_THREAD_LOCK, TAVI_KEY_IOCS_THREAD_UNLOCK.
 *
 * Revision 1.12  2005/11/10 08:53:51  zstein
 * *** empty log message ***
 *
 * Revision 1.11  2005/08/25 05:52:59  ywkim
 * merge & correct source
 *
 * Revision 1.3.2.8  2005/08/12 01:57:55  zstein
 * - T0
 *
 * Revision 1.3.2.7  2005/06/16 09:15:01  zstein
 * - added TAVI_KEY_IOCS_CLEAR for buffer queue.
 *
 * Revision 1.3.2.6  2005/05/16 05:37:27  zstein
 * *** empty log message ***
 *
 * Revision 1.3.2.5  2005/04/21 05:22:02  zstein
 * Newest version
 *
 * Revision 1.3.2.4  2005/03/04 13:13:53  zstein
 * added power key & hold key
 *
 * Revision 1.3.2.3  2005/03/03 13:43:42  zstein
 * *** empty log message ***
 *
 * Revision 1.3.2.2  2005/03/03 06:14:08  ywkim
 * Some modify
 *
 * Revision 1.3.2.1  2005/03/03 05:13:12  zstein
 * - changed key value.
 * - removed unused IO COMMAND
 * - Added keystruct structure.
 *
 */ 

#ifndef __TAVI_KEY_H__
#define __TAVI_KEY_H__

#include <asm/ioctl.h>
#include "../misc_group.h"

/* ioctl command */
#define TAVI_KEY_MAGIC			'K'
#define TAVI_KEY_IOCS_REPEATMODE	_IOW( TAVI_KEY_MAGIC, 1, int )
#define TAVI_KEY_IOCS_SPEED		_IOW( TAVI_KEY_MAGIC, 2, int )
#define TAVI_KEY_IOCS_SINGLEEVENT	_IOW( TAVI_KEY_MAGIC, 3, int )
#define TAVI_KEY_IOCG_BATTERY		_IOR( TAVI_KEY_MAGIC, 4, int )
#define TAVI_KEY_IOCS_CLEAR		_IO( TAVI_KEY_MAGIC, 5 )
#define TAVI_KEY_IOCG_CDS			_IOR( TAVI_KEY_MAGIC, 6, int )
#define TAVI_KEY_IOCS_CHANNEL_LOCK		_IOW( TAVI_KEY_MAGIC, 7, int )
#define TAVI_KEY_IOCS_CHANNEL_UNLOCK	_IOW( TAVI_KEY_MAGIC, 8, int )
#define TAVI_KEY_IOCS_THREAD_LOCK		_IO( TAVI_KEY_MAGIC, 9 )
#define TAVI_KEY_IOCS_THREAD_UNLOCK	_IO( TAVI_KEY_MAGIC, 10 )


#define TAVI_KEY_EXIT               	0x1000
#define TAVI_KEY_DOWN			0x1001
#define TAVI_KEY_RIGHT			0x1002
#define TAVI_KEY_UP				0x1003
#define TAVI_KEY_LEFT			0x1004
#define TAVI_KEY_ENTER			0x1005
#define TAVI_KEY_MODE			0x1006
#define TAVI_KEY_EQ				0x1007
#define TAVI_KEY_AB				0x1008
#define TAVI_KEY_MENU			0x1009
#define TAVI_KEY_JOG1_DOWN	0x100A // volume & mode
#define TAVI_KEY_JOG1_ENTER	0x100B
#define TAVI_KEY_JOG1_UP		0x100C
#define TAVI_KEY_JOG2_DOWN	0x100D // playback
#define TAVI_KEY_JOG2_ENTER	0x100E
#define TAVI_KEY_JOG2_UP		0x100F
#define TAVI_KEY_LEXIT               	0x2000
#define TAVI_KEY_LDOWN			0x2001
#define TAVI_KEY_LRIGHT			0x2002
#define TAVI_KEY_LUP			0x2003
#define TAVI_KEY_LLEFT			0x2004
#define TAVI_KEY_LENTER			0x2005
#define TAVI_KEY_LMODE			0x2006
#define TAVI_KEY_LEQ			0x2007
#define TAVI_KEY_LAB			0x2008
#define TAVI_KEY_LMENU			0x2009
#define TAVI_KEY_JOG1_LDOWN	0x200A
#define TAVI_KEY_JOG1_LENTER	0x200B
#define TAVI_KEY_JOG1_LUP		0x200C
#define TAVI_KEY_JOG2_LDOWN	0x200D
#define TAVI_KEY_JOG2_LENTER	0x200E
#define TAVI_KEY_JOG2_LUP		0x200F

#define TAVI_KEY_HOLD			0x3006
#define TAVI_KEY_SHUTDOWN		0x3007
//#define TAVI_KEY_HOLDOFF		0x3008
#define TAVI_KEY_UNKNOWN		0x03FF

#define TAVI_CH_BATTERY	0
#define TAVI_CH_KEY1		1
#define TAVI_CH_KEY2		2
#define TAVI_CH_CDS			3
struct keystruct {
	unsigned short key;
	unsigned short press;
};

#endif /* __TAVI_KEY_H__ */



