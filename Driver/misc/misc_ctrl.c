//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Misc. control driver
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/misc/misc_ctrl.c,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: misc_ctrl.c,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.24  2006/02/17 13:03:29  ywkim
// Remove CHARGER related function or macro
//
// Revision 1.23  2006/02/11 04:59:00  zstein
// - deleted comments.
//
// Revision 1.22  2006/02/10 10:58:29  ywkim
// Add selection macro for CHARGER
//
// Revision 1.21  2006/02/10 09:45:39  ywkim
// Clear a bit of POWER_ON on ctrl_in, when MISC_ReadData was readed
//
// Revision 1.20  2006/02/08 04:38:52  ywkim
// Change inital value of pPIO0_b9 to 0
//
// Revision 1.19  2006/02/06 02:03:33  ywkim
// Add routine for checking of charger_status in tavi_misc_init()
//
// Revision 1.18  2006/02/04 06:00:26  ywkim
// Change port & initial values for HW0602
//
// Revision 1.17  2006/01/13 08:33:28  ywkim
// Change some function for New H/W, it enabled with compile option of HW0601
//
// Revision 1.16  2005/12/10 08:44:33  ywkim
// Add instruncton of disable & enable global_interrupt on ReadData & SendData
//
// Revision 1.15  2005/12/10 06:09:38  ywkim
// Remove reset of UART_FCR
//
// Revision 1.14  2005/12/09 08:16:34  ywkim
// Add asm("nop") in TAVI_MISC_ReadData
//
// Revision 1.13  2005/12/09 03:39:57  ywkim
// Add reset of UART_FCR on TAVI_MISC_ReadData & TAVI_MISC_SendData
//
// Revision 1.12  2005/12/03 05:55:43  zstein
// *** empty log message ***
//
// Revision 1.11  2005/12/02 10:53:24  ywkim
// Set PIO2_DATA to 0x00ff00ff in TAVI_MISC_ReadData()
//
// Revision 1.10  2005/12/02 06:54:50  zstein
// *** empty log message ***
//
// Revision 1.9  2005/11/17 11:22:37  zstein
// *** empty log message ***
//
// Revision 1.8  2005/11/14 04:45:53  ywkim
// Change routine of tavi_misc_init() for SPEAKER_AMP off
//
// Revision 1.7  2005/11/11 12:31:07  zstein
// *** empty log message ***
//
// Revision 1.6  2005/10/28 09:34:29  ywkim
// Bugfix for power led
//******************************************************************************

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/completion.h>
#include <asm/semaphore.h>
#include <asm/arch/hardware.h>
#include <asm/delay.h>
#include <asm/uaccess.h>
#include <linux/pbi.h>

#include "misc_ctrl.h"
#include "../misc_group.h"
#include "../i2cm/i2cm.h"

#define TAVI_MISC_VERSION_HEAD		"ES3-"
#define TAVI_MISC_VER_MAJOR			0
#define TAVI_MISC_VER_MINOR			3

#define TX_CLK_HIGH		( *pPIO0_DATA = MK_REG( 14, 1 ) )
#define TX_CLK_LOW		( *pPIO0_DATA = MK_REG( 14, 0 ) )
#define RX_CLK_HIGH		( *pPIO0_DATA = MK_REG( 13, 1 ) )
#define RX_CLK_LOW		( *pPIO0_DATA = MK_REG( 13, 0 ) )
#define RX_OE_ENABLE	( *pPIO0_DATA = MK_REG( 2, 0 ) )
#define RX_OE_DISABLE	( *pPIO0_DATA = MK_REG( 2, 1 ) )

#define DISABLE_GLOBAL_INT		(*((volatile int *)0x00500224) &= ~0x80000000)
#define ENABLE_GLOBAL_INT		(*((volatile int *)0x00500224) |= 0x80000000)


#define DAC_ADDR		0x40

static struct tavi_misc_private {
	struct misc_ctrl_struct miscstruct;
	int busy;
} tavi_misc_priv;


void TAVI_MISC_SendData( char data )
{
	uint32_t flags;

	__save_flags (flags);
	__clf ();
	__cli ();
//	DISABLE_GLOBAL_INT;
	*pUART0_ALTERNATE_FUNCTION = 0x01;
	*pPIO2_DATA = 0x00ff0000 | data;
	*pPIO2_DIR = 0x00ff00ff;
	asm ("nop");
	TX_CLK_LOW;
	asm ("nop");
	TX_CLK_HIGH;
	asm ("nop");
//	*pPIO2_DIR = 0x00ff0000;
	*pPIO2_DATA = 0x00ff00ff;		// 05.10.28 fix power led problem
	*pUART0_ALTERNATE_FUNCTION = 0x00;
//	ENABLE_GLOBAL_INT;
	__restore_flags (flags);

	udelay( 100 );
}

char TAVI_MISC_ReadData( void )
{
	int data;
	uint32_t flags;

	__save_flags (flags);
	__clf ();
	__cli ();
//	DISABLE_GLOBAL_INT;
	*pUART0_ALTERNATE_FUNCTION = 0x01;
	*pPIO2_DIR = 0x00ff0000;
	RX_OE_ENABLE;
	RX_CLK_LOW;
	asm ("nop");
	RX_CLK_HIGH;
	asm ("nop");
	data = *pPIO2_DATA;
	asm ("nop");	
	RX_OE_DISABLE;
	*pPIO2_DATA = 0x00ff00ff;		// 05.12.02 for unknown death
	*pPIO2_DIR = 0x00ff00ff;
	*pUART0_ALTERNATE_FUNCTION = 0x00;
//	ENABLE_GLOBAL_INT;
	__restore_flags (flags);

	return (char)data;
}

int speaker_amp_onoff( int onoff )
{
	if( onoff ) tavi_misc_priv.miscstruct.ctrl_out &= ~(1<<AMP_ON);
	else tavi_misc_priv.miscstruct.ctrl_out |= (1<<AMP_ON);
	TAVI_MISC_SendData( tavi_misc_priv.miscstruct.ctrl_out );

	return 0;
}

#define MAX		1000
#define MIN		0
#define TERM		100
#define SY 		1 // sync
#define SD 		2 // shutdown
#define BG  		4 // bandgap 
#define DEFAULT	0

static void set_lcd_brightness( int level )
{
	int data = MIN+(level)*TERM;

	em85xx_i2c_seqwrite( DAC_ADDR, DEFAULT, (char*)&data, 2 );
}

#ifndef HW0601
static void mute( int onoff )
{
	// ON
	if( onoff ) {
		*pPIO1_DATA = MK_REG( 8, 0 );
	}
	else {
		*pPIO1_DATA = MK_REG( 8, 1 );
	}
}
#endif

/* File Operations
 */  
static int tavi_misc_open( struct inode* inode, struct file* filep )
{
	if( tavi_misc_priv.busy ) return -EBUSY;
	tavi_misc_priv.busy = 1;

	return 0;
}

static int tavi_misc_release( struct inode* inode, struct file* filep )
{
	tavi_misc_priv.busy = 0;

	return 0;
}

static int tavi_misc_ioctl( struct inode* inode, struct file* filep, unsigned int cmd, 
		unsigned long arg )
{
	struct tavi_misc_private* dev = &tavi_misc_priv;
	struct misc_ctrl_struct* pDev = &dev->miscstruct;
	struct misc_ctrl_struct* pUserDev = ( struct misc_ctrl_struct* )arg;
//	char old;

	if( arg == 0 ) return -EFAULT;
	
	switch( cmd ) {
	case TAVI_MISC_IOCS_CONTROL:
		copy_from_user( &pDev->ctrl_out, &pUserDev->ctrl_out, sizeof(char) );
		TAVI_MISC_SendData( pDev->ctrl_out );
		break;
	case TAVI_MISC_IOCG_CONTROL:
		pDev->ctrl_in = TAVI_MISC_ReadData();
		pDev->ctrl_in &= ~(1<<POWER_ON);
		copy_to_user( pUserDev, pDev, sizeof(struct misc_ctrl_struct) );
		break;
	case TAVI_MISC_IOCS_BRIGHTNESS:
		copy_from_user( &pDev->brightness, &pUserDev->brightness, sizeof(char) );
		set_lcd_brightness( pDev->brightness );
		break;
#ifndef HW0601
	case TAVI_MISC_IOCS_MUTE_ON:
		mute( 1 );
		break;
	case TAVI_MISC_IOCS_MUTE_OFF:
		mute( 0 );
		break;
#endif		
	}
	
	return 0;
}

static struct file_operations tavi_misc_ops = {
	owner: THIS_MODULE,
	open: tavi_misc_open,
	release: tavi_misc_release,
	ioctl: tavi_misc_ioctl
};

static struct miscdevice tavi_misc_miscdev = {
	TAVI_MISC_MINOR,
	"tmisc",
	&tavi_misc_ops
};

static int __init tavi_misc_init( void )
{
	int ret=0;

	printk( "TAVI Misc. control driver Ver. %s%d.%d, %s\n", 
		TAVI_MISC_VERSION_HEAD, TAVI_MISC_VER_MAJOR, TAVI_MISC_VER_MINOR, __DATE__ );

	ret = misc_register( &tavi_misc_miscdev );

#ifdef HW0601
	// Initialize GPIO
	*pPIO0_DATA = MK_REG( 0, 0 ) |	// buzzer off (host power off)
				MK_REG( 2, 1 ) |		// RX_FF output disable
				MK_REG( 5, 0 ) |		// host power off
				MK_REG( 9, 0 ) |		// power led on
				MK_REG( 10, 1 ) |		// main power on
				MK_REG( 13, 1 ) |		// RX_FF clk
				MK_REG( 14, 1 );		// TX_FF clk
	*pPIO0_DIR = MK_REG( 0, 1 ) |		// buzzer output (host power output)
				MK_REG( 2, 1 ) |		// RX_FF output control
				MK_REG( 5, 1 ) |		// host power control
				MK_REG( 9, 1 ) |		// power led output
				MK_REG( 10, 1 ) |		// main power control
				MK_REG( 13, 1 ) |		// RX_FF clk control
				MK_REG( 14, 1 );		// TX_FF clk control
	
	*pPIO1_DATA = MK_REG( 7, 1 ) |	// EXT EL on
				MK_REG( 8, 1 );		// HDD Enable

	*pPIO1_DIR = MK_REG( 7, 1 ) |		// for hdd enable
				MK_REG( 8, 1 );		// for EX_EL_#ON 
	
	*pPIO2_DATA = 0x00ff00ff;
	*pPIO2_DIR = 0x00ff00ff;

	// MUTE_ON, USB_DIS, SPK_OFF, HOST_OFF,
	tavi_misc_priv.miscstruct.ctrl_out = 0x02;

#else
	// Initialize GPIO
	*pPIO0_DATA = MK_REG( 2, 1 ) |
		MK_REG( 13, 1 ) |
		MK_REG( 14, 1 );
	*pPIO0_DIR = MK_REG( 2, 1 ) |
//		MK_REG( 11, 1 ) |
		MK_REG( 13, 1 ) |
		MK_REG( 14, 1 );

	*pPIO1_DATA = 0x00000000;
	*pPIO1_DIR = MK_REG( 8, 1 );

	*pPIO2_DATA = 0x00ff00ff;
	*pPIO2_DIR = 0x00ff00ff;



	// KEY_LED_OFF, POWER_LED_ON, POWER_ON, HDD_ON
	//tavi_misc_priv.miscstruct.ctrl_out = 0x95;
	tavi_misc_priv.miscstruct.ctrl_out = 0x86; // for PP1
	// SPEAKER_AMP -> OFF
	tavi_misc_priv.miscstruct.ctrl_out |= (1<<AMP_ON);
#endif

	return ret;
}

static void __exit tavi_misc_exit( void )
{
	misc_deregister( &tavi_misc_miscdev );

	return;
}

EXPORT_SYMBOL( speaker_amp_onoff );
EXPORT_SYMBOL( TAVI_MISC_SendData );
EXPORT_SYMBOL( TAVI_MISC_ReadData );

MODULE_AUTHOR( "Lee Seok<zstein@newmedialife.com>" );
MODULE_DESCRIPTION( "TAVI Misc Driver" );

module_init( tavi_misc_init );
module_exit( tavi_misc_exit );

