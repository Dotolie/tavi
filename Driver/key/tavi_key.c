//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Key Device Driver
// 	TAVI uses ADC(MCP3004, 4CH) for capturing key value.
//	ADC provides SPI for communication with MCU.
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/key/tavi_key.c,v $
// $Revision: 1.3 $
// $Date: 2006/06/07 08:35:39 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: tavi_key.c,v $
// Revision 1.3  2006/06/07 08:35:39  zstein
// - removed unused codes.
//
// Revision 1.2  2006/04/15 02:18:27  zstein
// - removed TAVI_KEY_HOLDOFF.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.23  2006/02/13 07:47:27  zstein
// - executes key thread when deive is opened.
//
// Revision 1.22  2006/02/07 09:18:46  zstein
// *** empty log message ***
//
// Revision 1.21  2006/01/16 02:41:47  zstein
// *** empty log message ***
//
// Revision 1.20  2005/12/26 04:55:15  zstein
// - added TAVI_KEY_IOCS_THREAD_LOCK, TAVI_KEY_IOCS_THREAD_UNLOCK.
//
// Revision 1.19  2005/12/01 01:07:03  zstein
// *** empty log message ***
//
// Revision 1.18  2005/11/28 02:42:24  zstein
// *** empty log message ***
//
// Revision 1.17  2005/11/18 05:05:26  zstein
// *** empty log message ***
//
// Revision 1.16  2005/11/16 07:20:38  zstein
// *** empty log message ***
//
// Revision 1.15  2005/11/11 12:30:52  zstein
// *** empty log message ***
//
// Revision 1.14  2005/11/10 08:53:51  zstein
// *** empty log message ***
//
//******************************************************************************

 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <asm/semaphore.h>
#include <asm/arch/hardware.h>
#include <asm/delay.h>
#include <asm/uaccess.h>
#include <linux/pbi.h>

#include "tavi_key.h"
#include "../misc/misc_ctrl.h"
#include "../i2cm/i2cm.h"
#include "../rtc/tavi_rtc.h"

// Macros
#define TAVI_KEY_VER_MAJOR		2
#define TAVI_KEY_VER_MINOR		0

#define ADC_CLK					9		// PIO0
#define ADC_CS					10		// PIO0
#define ADC_OUT					7		// PIO1. Viewpoint of ADC
#define ADC_DIN					8		// PIO1
#define DEFAULT_HEADER			0xC0	// Start, single

#define SAMPLE_RATE				HZ/10	// MAXIMUM SPEED
#define OFFSET					0x80

#define MAX_C_QUEUE_SIZE		8
#define LONGKEY_REPEAT_COUNT	7

#define I2C_ADC_ADDR			0x48

#define DEBUG
#ifdef DEBUG
#define DPRINTK printk
#else
#define DPRINTK
#endif /* DEBUG */



// Register pointers
#define REG_CONV			0x00
#define REG_ALERT 			0x01
#define REG_CONF			0x02
#define REG_CYCLE			0x03
#define REG_CH1_DATAL		0x04
#define REG_CH1_DATAH		0x05
#define REG_CH1_HYS			0x06
#define REG_CH2_DATAL		0x07
#define REG_CH2_DATAH		0x08
#define REG_CH2_HYS			0x09
#define REG_CH3_DATAL		0x0a
#define REG_CH3_DATAH		0x0b
#define REG_CH3_HYS			0x0c
#define REG_CH4_DATAL		0x0d
#define REG_CH4_DATAH		0x0e
#define REG_CH4_HYS			0x0f

#define CHANNEL_NUM		4


static short keymap1[8] = {
	TAVI_KEY_EXIT,
	TAVI_KEY_MENU,
	TAVI_KEY_DOWN,
	TAVI_KEY_UP,
	TAVI_KEY_RIGHT,
	TAVI_KEY_LEFT,
	TAVI_KEY_ENTER,
	TAVI_KEY_EQ 
};

static short keymap2[8] = {
	TAVI_KEY_AB,
	TAVI_KEY_JOG2_ENTER,
	TAVI_KEY_JOG2_DOWN,
	TAVI_KEY_JOG2_UP,
	TAVI_KEY_JOG1_DOWN,
	TAVI_KEY_JOG1_UP,
	TAVI_KEY_MODE,
	TAVI_KEY_JOG1_ENTER
};
static short repeatkeymap[16] = {
	TAVI_KEY_LEXIT,
	TAVI_KEY_LMENU,
	TAVI_KEY_LDOWN,
	TAVI_KEY_LUP,
	TAVI_KEY_LRIGHT,
	TAVI_KEY_LLEFT,
	TAVI_KEY_LENTER,
	TAVI_KEY_LEQ,
	TAVI_KEY_LAB,
	TAVI_KEY_JOG2_LENTER,
	TAVI_KEY_JOG2_LDOWN,
	TAVI_KEY_JOG2_LUP,
	TAVI_KEY_JOG1_LDOWN,
	TAVI_KEY_JOG1_LUP,
	TAVI_KEY_LMODE,
	TAVI_KEY_JOG1_LENTER
};

static struct tavi_key_private {
	int busy;
	int pid;
	int samplerate;
	int key;
	int press;
	int repeatmode;
	int singleevent;
	int hold;
	int battery;
	int powercount;
	unsigned long repeatkeymap;
	wait_queue_head_t wq;
	struct semaphore lock;
	struct keystruct Q[MAX_C_QUEUE_SIZE];
	short front;
	short rear;
	unsigned int last_jiffies;
	int ch_lock[4];
	int thr_lock;
} tavi_key_priv;

static DECLARE_COMPLETION( exited );

enum TaviKeyNumbering {
	Key_Exit,
	Key_Menu,
	Key_Down,
	Key_Up,
	Key_Right,
	Key_Left,
	Key_Enter,
	Key_Eq,
	Key_AB,
	Key_Jog2_Enter,
	Key_Jog2_Down,
	Key_Jog2_Up,
	Key_Jog1_Down,
	Key_Jog1_Up,
	Key_Mode,
	Key_Jog1_Enter
};

static int get_adc( int ch )
{
	char data[4];
	int ret, c, *p;
	short sdata;

	if( tavi_key_priv.ch_lock[ch] ) {
		if( ch == 1 || ch == 2 ) return TAVI_KEY_UNKNOWN;
		return -1;
	}
	p = (int*)data;
	*p = 0xffff03ff;
	if( ch >= CHANNEL_NUM ) return -1;
	
	data[0] = (0x10<<ch);
	em85xx_i2c_noreg_write( I2C_ADC_ADDR, data, 1 );
	udelay( 100 );
	if( em85xx_i2c_noreg_read( I2C_ADC_ADDR, data, 2 ) < 0 ) {
		printk( "failed to em85xx_i2c_noreg_read\n" );
		return -1;
	}
	if( data[0]&0x80 ) return -1;
	
	c = (data[0]&0x30) >> 4;
	udelay( 100 );
	ret = ((int)data[0]&0x0f) << 8 | data[1];
	ret >>= 2;

	sdata = ((int)data[0])<<8 | data[1];

	//printk( "channel: %d, value: 0x%x, serial: 0x%x\n", ch, ret, sdata );
	
	return ret;
}

static void InitADC( void )
{
	char data[4];
	struct em85xx_i2c_dev dev, olddev;

	olddev = em85xx_i2c_get_ctrl();
	dev = olddev;
	dev.reg_addr_disable = 1;
	em85xx_i2c_set_ctrl( &dev );

	// Select channels
	data[0] = REG_CONF;
	//data[1] = 0xf0;
	data[1] = 0x00;
	em85xx_i2c_noreg_write( I2C_ADC_ADDR, data, 2 );

	em85xx_i2c_set_ctrl( &olddev );
}

static int read_battery( void )
{
	struct em85xx_i2c_dev olddev, dev;
	int level;

	olddev = em85xx_i2c_get_ctrl();
	dev = olddev;
	dev.reg_addr_disable = 1;
	em85xx_i2c_set_ctrl( &dev );
	level = get_adc( 0 );
	em85xx_i2c_set_ctrl( &olddev );
	level += 17;
	return level;
}

static int read_cds( void )
{
	struct em85xx_i2c_dev olddev, dev;
	int level;
	olddev = em85xx_i2c_get_ctrl();
	dev = olddev;
	dev.reg_addr_disable = 1;
	em85xx_i2c_set_ctrl( &dev );

	level = get_adc( 3 );

	em85xx_i2c_set_ctrl( &olddev );
	return level;
}

static int read_key( int* map_index )
{
	int group=0, index;
	int key;
	int tol;
	struct em85xx_i2c_dev olddev, dev;

	olddev = em85xx_i2c_get_ctrl();
	dev = olddev;
	dev.reg_addr_disable = 1;
	em85xx_i2c_set_ctrl( &dev );

	if( (key = get_adc(1)) > 0x3a0 ) {
		udelay( 30 );
		if( (key=get_adc(2)) > 0x3a0 ) key = TAVI_KEY_UNKNOWN;
		else group = 1;
	}
	else {
		group = 0;
	}

	if( key == -1 ) {
		key = -EAGAIN;
		goto __end;
	}

 	key += 0x20;
	key &= ~0x0f;
	index = key/OFFSET;
	if( index > 7 ) {
		key = TAVI_KEY_UNKNOWN;
		goto __end;
	}
	tol = key % OFFSET;
	if( tol > 0x40 ) {
		key = TAVI_KEY_UNKNOWN;
		goto __end;
	}

	if( tavi_key_priv.repeatmode ) {
		if( (jiffies-tavi_key_priv.last_jiffies) < 20 ) {
			key = -EAGAIN;
			goto __end;
		}
	}
	tavi_key_priv.last_jiffies = jiffies;
	
	if( group == 0 ) key = keymap1[index];
	else key = keymap2[index];

	*map_index = 8*group + index;

__end:
	em85xx_i2c_set_ctrl( &olddev );
	
	return key;
}

static void insert_Q( short key, short press )
{
	short front = tavi_key_priv.front;

	if( front >= MAX_C_QUEUE_SIZE ) front = 0;
	if( key == -EAGAIN ) return;
	tavi_key_priv.Q[front].key = key;
	tavi_key_priv.Q[front].press = press;
	tavi_key_priv.front++;
	tavi_key_priv.front %= MAX_C_QUEUE_SIZE;
	if( tavi_key_priv.front == tavi_key_priv.rear ) 
		tavi_key_priv.rear++;
	tavi_key_priv.rear %= MAX_C_QUEUE_SIZE;
}

static int delete_Q( struct keystruct* key )
{
	short front = tavi_key_priv.front;
	short rear = tavi_key_priv.rear;
	// no key data
	if( front == rear ) return -1;
	*key = tavi_key_priv.Q[rear];
	rear++;
	rear %= MAX_C_QUEUE_SIZE;
	tavi_key_priv.rear = rear;
	return 0;
}

static int tavi_key_thread( void* arg )
{
	struct tavi_key_private* dev = ( struct tavi_key_private* )arg;
	int key=TAVI_KEY_UNKNOWN, oldkey;
	int mindex;
	static int repeatcount=0;
	char rx_buf;

	daemonize();

	strcpy( current->comm, "ktavikeyd" );

	oldkey = 0xffff;
	dev->powercount = 0;
	
	do {
		if( dev->thr_lock ) goto __p4;
        
		// Check Power-OFF
		rx_buf = TAVI_MISC_ReadData();
		if( !(rx_buf&(1<<POWER_ON)) ) {
			char level;
			// check RTC interrupt line.
			// RTC interrupt line°ú ÆÄ¿ö¹öÆ° ½ÅÈ£¶óÀÎÀÌ °øÀ¯µÇ¹Ç·Î Ã¼Å©ÇØ¾ßÇÔ.
			if( em85xx_i2c_read( I2C_RTC_ADDR, CONTROL_STAT2, (char*)&level, 1 ) == 1 ) {
				/*
				    Check alarm flag
				*/
				if( (level&(1<<AF)) == 0 ) {
					dev->powercount++;
				}
			}
		}
		else {
			if( dev->powercount ) {
				key = TAVI_KEY_HOLD;
				dev->hold = 1;
				dev->powercount = 0;
				goto __p2;
			}
		}

		// power-off
		if( dev->powercount == 10 ) {
			key = TAVI_KEY_SHUTDOWN;
			dev->powercount = 0;
			goto __p2;
		}

		// read battery & key from adc
		oldkey = key;
		key = read_key( &mindex );
		if( key == TAVI_KEY_UNKNOWN ) {
			repeatcount = 0;
			dev->repeatkeymap = 0;
			goto __p2;
		}
		else if( key == -EAGAIN ) goto __p3;
			
		// check long-key
		if( dev->repeatmode == 0 ) {
__longkey:
			if( dev->repeatkeymap & (1<<mindex) ) {
				oldkey = key;
				key = repeatkeymap[mindex];
				//if( dev->repeatmode && dev->singleevent ) goto __p2;
				//else goto __p3;
				goto __p3;
			}
			else {
				int i;
				dev->repeatkeymap = 0;
				for( i=0; i<LONGKEY_REPEAT_COUNT; i++ ) {
					set_current_state( TASK_INTERRUPTIBLE );
					schedule_timeout( dev->samplerate );
					oldkey = key;
					do {
						key = read_key( &mindex );
						if( key == -EAGAIN ) mdelay( 20 );
						else break;
					} while( 1 );
					if( oldkey != key ) {
						key = oldkey;
						break;
					}
				}
				if( i == LONGKEY_REPEAT_COUNT ) {
					key = repeatkeymap[mindex];
					dev->repeatkeymap = (1<<mindex);
				}
			}
		}
		else { // repeat key mode
			// EXIT, MODE and EQ MUST keep long-key mode 
			// even if repeat-key mode.
			switch( key ) {
			//case TAVI_KEY_EXIT:
			case TAVI_KEY_EQ:
			case TAVI_KEY_MODE:
			case TAVI_KEY_AB:
			case TAVI_KEY_MENU:
			case TAVI_KEY_JOG1_ENTER:
			case TAVI_KEY_JOG2_ENTER:
				goto __longkey;
			}
		}

__p2:
		if( key != TAVI_KEY_UNKNOWN ) {
			//down_interruptible( &(dev->lock) );
			insert_Q( key, 1 );
			wake_up_interruptible( &dev->wq );
			//up( &dev->lock );
		}
		else {
			if( !dev->singleevent && oldkey != TAVI_KEY_UNKNOWN )  { // Release Event
				//down_interruptible( &(dev->lock) );
				insert_Q( oldkey, 0 );
				wake_up_interruptible( &dev->wq );
				//up( &dev->lock );
			}
		}

__p3:
		up( &dev->lock );
__p4:
		set_current_state( TASK_INTERRUPTIBLE );
		schedule_timeout( dev->samplerate );
	} while( !signal_pending(current) || dev->key != TAVI_KEY_SHUTDOWN );

	complete_and_exit( &exited, 0 );
}


/* File Operations
 */  
static int tavi_key_open( struct inode* inode, struct file* filep )
{
    if( tavi_key_priv.busy ) return -EBUSY;
    tavi_key_priv.busy = 1;
    
    tavi_key_priv.pid = kernel_thread( tavi_key_thread, (void*)&tavi_key_priv, 0 );

    return 0;
}

static int tavi_key_release( struct inode* inode, struct file* filep )
{
	struct tavi_key_private* dev = &tavi_key_priv;

	dev->busy = 0;

	return 0;
}

static int tavi_key_read( struct file* filep, char* buf, size_t size, loff_t* offp )
{
	struct keystruct keystruct;
	int ret;
	struct tavi_key_private* dev = &tavi_key_priv;
		
	if( !buf ) {
		return -EFAULT;
	}

	if( filep->f_flags & O_NONBLOCK ) {
		
		if( down_interruptible( &dev->lock ) ) return -EAGAIN;
		ret = delete_Q( &keystruct );
		up( &dev->lock );
		if( ret < 0 ) return -EAGAIN;
		copy_to_user( buf, &keystruct, sizeof(struct keystruct) );
		//printk( "read key: 0x%x, press: %d\n", keystruct.key, keystruct.press );
		return sizeof(struct keystruct);
	}

	interruptible_sleep_on( &dev->wq );

	if( signal_pending(current) ) return -EINTR;

	if( down_interruptible( &dev->lock ) ) return -EINTR;
	ret = delete_Q( &keystruct );
	up( &dev->lock );
	if( ret < 0 ) return -EAGAIN;
	
	copy_to_user( buf, (char*)&keystruct, sizeof(int) );
	
	return sizeof( size );
}

static int tavi_key_ioctl( struct inode* inode, struct file* filep, unsigned int cmd, 
		unsigned long arg )
{
	struct tavi_key_private* dev = &tavi_key_priv;
	int value = *( int* )arg;

	switch( cmd ) {
	case TAVI_KEY_IOCS_REPEATMODE:
		if( arg ) dev->repeatmode = 1;
		else dev->repeatmode = 0;
		break;
	case TAVI_KEY_IOCS_SPEED:
		if( value <= 0 ) return -EINVAL;
		if( value > 10 ) value = 10;
		dev->samplerate = HZ/value;
		break;
	case TAVI_KEY_IOCS_SINGLEEVENT:
		dev->singleevent = arg;
		break;
	case TAVI_KEY_IOCG_BATTERY:
		value = read_battery();
		// ½ÇÁ¦ ¹èÅÍ¸® ·¹º§°ú å°è»êµÈ °á°ú°¡ Æ²·Á¼­ º¸»óÇÔ.
		value -= 8;
		copy_to_user( (void*)arg, (void*)&value, sizeof(int) );
		break;
	case TAVI_KEY_IOCG_CDS:
		value = read_cds();
		copy_to_user( (void*)arg, (void*)&value, sizeof(int) );
		break;
	case TAVI_KEY_IOCS_CLEAR:
		if( down_interruptible( &dev->lock ) ) return -EAGAIN;
		dev->front = dev->rear;
		up( &dev->lock );
		break;
	case TAVI_KEY_IOCS_CHANNEL_LOCK:
		if( value >= TAVI_CH_BATTERY || value <= TAVI_CH_CDS ) 
			dev->ch_lock[value] = 1;
		else 
			return -EINVAL;
		break;
	case TAVI_KEY_IOCS_CHANNEL_UNLOCK:
		if( value >= TAVI_CH_BATTERY || value <= TAVI_CH_CDS ) 
			dev->ch_lock[value] = 0;
		else 
			return -EINVAL;
		break;
	case TAVI_KEY_IOCS_THREAD_LOCK:
		dev->thr_lock = 1;
		//while( down_interruptible( &dev->lock ) );
		break;
	case TAVI_KEY_IOCS_THREAD_UNLOCK:
		dev->thr_lock = 0;
		//up( &dev->lock );
		break;
	}
	
	return 0;
}

static struct file_operations tavi_key_ops = {
	owner: THIS_MODULE,
	open: tavi_key_open,
	read: tavi_key_read,
	release: tavi_key_release,
	ioctl: tavi_key_ioctl
};

static struct miscdevice tavi_key_miscdev = {
	TAVI_KEY_MINOR,
	"tkey",
	&tavi_key_ops
};

static int __init tavi_key_init( void )
{
	int ret=0;
	int i;

	tavi_key_priv.busy = 0;
	tavi_key_priv.repeatkeymap = 0;

	InitADC();
	
	// Initialize Queue
	for( i=0; i<MAX_C_QUEUE_SIZE; i++ ) {
		tavi_key_priv.Q[i].key = 0xFFFF;
		tavi_key_priv.Q[i].press = -1;
	}
	tavi_key_priv.front = 0;
	tavi_key_priv.rear = 0;
	
	init_waitqueue_head( &tavi_key_priv.wq );

	tavi_key_priv.samplerate = SAMPLE_RATE;
	tavi_key_priv.hold = 0;
	tavi_key_priv.repeatmode = 0;
	tavi_key_priv.singleevent = 0;
	tavi_key_priv.powercount = 0;
	tavi_key_priv.thr_lock = 0;
	init_MUTEX( &tavi_key_priv.lock );

	printk( "TAVI key driver Ver. %d.%d, %s\n", TAVI_KEY_VER_MAJOR, TAVI_KEY_VER_MINOR, __DATE__ );

	

	ret = misc_register( &tavi_key_miscdev );

	return ret;
}

static void __exit tavi_key_exit( void )
{
	misc_deregister( &tavi_key_miscdev );
	kill_proc( tavi_key_priv.pid, SIGTERM, 1 );

	wait_for_completion( &exited );

	return;
}

MODULE_AUTHOR( "Lee Seok<zstein@newmedialife.com>" );
MODULE_DESCRIPTION( "TAVI Key Driver" );

module_init( tavi_key_init );
module_exit( tavi_key_exit );


