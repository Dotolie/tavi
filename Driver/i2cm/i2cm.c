/*
 * EM85xx I2C Master Driver
 * 
 * i2cm.c
 * 
 * Copyright(C) New Media Life Inc.
 *
 * Author: Lee Seok<zstein@newmedialife.com>
 * 
 * Update:
 * 2004/11/26: - added em85xx_i2c_set_ctrl() and em85xx_i2c_get_ctrl() functions
 *			   - added i2c burst mode read/write
 */ 

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

#include <asm/param.h>
#include "i2cm.h"

#define SYS_CTRL_RESET				(0x00500020)
#define I2C_MASTER_BASE_ADDRESS		( 0x500800 )
#define I2C_MASTER_CONFIG           (I2C_MASTER_BASE_ADDRESS + 0x00)
#define I2C_MASTER_CLK_DIV          (I2C_MASTER_BASE_ADDRESS + 0x04)
#define I2C_MASTER_DEV_ADDR         (I2C_MASTER_BASE_ADDRESS + 0x08)
#define I2C_MASTER_ADR              (I2C_MASTER_BASE_ADDRESS + 0x0c)
#define I2C_MASTER_DATAOUT          (I2C_MASTER_BASE_ADDRESS + 0x10)
#define I2C_MASTER_DATAIN           (I2C_MASTER_BASE_ADDRESS + 0x14)
#define I2C_MASTER_STATUS           (I2C_MASTER_BASE_ADDRESS + 0x18)
#define I2C_MASTER_STARTXFER        (I2C_MASTER_BASE_ADDRESS + 0x1c)
#define I2C_MASTER_BYTE_CNT         (I2C_MASTER_BASE_ADDRESS + 0x20)
#define I2C_MASTER_INTEN            (I2C_MASTER_BASE_ADDRESS + 0x24)
#define I2C_MASTER_INT              (I2C_MASTER_BASE_ADDRESS + 0x28)

#define REG_DWORD( __addr )			*( (volatile unsigned long*)__addr )
#define REG_WORD( __addr )			*( (volatile unsigned short int*)__addr )
#define REG_BYTE( __addr )			*( (volatile unsigned char*)__addr )

#define VER_MAJOR		0
#define VER_MINOR		2
#define DEVICE_NAME		"i2c"
#define LOOP_TIMEOUT	2000

#define timer_after( unknown, known )	((long)(known)-(long)(unknown) < 0)
#define timer_before( unknown, known )	((long)(unknown)-(long)(known) < 0)

#define DEBUG
#ifdef DEBUG
#define DEBUGMSG					printk
#else
#define DEBUGMSG	
#endif

#define I2C_IS_IDLE	\
({ int timeout = dev.timeout; \
   while( (REG_WORD(I2C_MASTER_STATUS)&0x01)==0 && timeout ) timeout--; \
   timeout; \
 })
#define I2C_TX_COMPLETE \
({ int timeout = dev.timeout; \
   while( (REG_WORD(I2C_MASTER_STATUS)&0x02)==0 && timeout ) timeout--; \
   if( REG_WORD(I2C_MASTER_STATUS)&0x08 ) timeout = 0; \
   timeout; \
 })
#define I2C_RX_READY \
({ int timeout = dev.timeout; \
   while( (REG_WORD(I2C_MASTER_STATUS)&0x04)==0 && timeout ) timeout--; \
   timeout; \
 })
 #define I2C_ACK_ERROR 	((REG_WORD(I2C_MASTER_STATUS)&0x08))
#if 0
 #define I2C_RESET		(REG_DWORD(SYS_CTRL_RESET) = 0x80)
#else
 #define I2C_RESET		({(REG_DWORD(SYS_CTRL_RESET) = 0x80); em85xx_i2c_set_ctrl( &dev );udelay(100000);})
#endif

struct em85xx_i2c_dev dev;

int em85xx_i2c_set_ctrl( struct em85xx_i2c_dev* pdev )
{
	unsigned short reg;
	dev = *pdev;
	reg = (dev.reg_addr_len-1) << 5 | (dev.dev_addr_len-1) << 2 |
		  dev.reg_addr_disable << 1 | dev.dev_addr_disable;
	REG_WORD( I2C_MASTER_CONFIG ) = reg;
	REG_WORD( I2C_MASTER_CLK_DIV ) = dev.clk_div;

	return 0;
}

struct em85xx_i2c_dev em85xx_i2c_get_ctrl( void )
{
	return dev;
}


// assume that ADDRDIS is enabled.
int em85xx_i2c_noreg_write( char addr, char* data, int size )
{
	int i, ret=0;
	uint32_t flags;

    spin_lock( &dev.lock );
	if( size <= 0 ) return -1;
	
	__save_flags (flags);
	__clf ();
	__cli ();

	if( !I2C_IS_IDLE ) {
		ret = -1;
		goto __end;
	}
	
	REG_WORD( I2C_MASTER_DEV_ADDR ) = addr>>1;
	REG_DWORD( I2C_MASTER_BYTE_CNT ) = size-1;
	REG_WORD( I2C_MASTER_STARTXFER ) = 0;
	// wait until DOUTEMPTY is true.
	if( I2C_TX_COMPLETE == 0 ) {
		ret = -2;
		goto __end;
	}
	if( I2C_ACK_ERROR ) {
		ret = -3;
		goto __end;
	}
	for( i=0; i<size; i++ ) {
		REG_WORD( I2C_MASTER_DATAOUT ) = data[i];
		if( I2C_TX_COMPLETE == 0 ) {
			ret = -4;
			goto __end;
		}
		if( I2C_ACK_ERROR ) {
			ret = -3;
			goto __end;
		}
	}

__end:
	__restore_flags( flags );
    
    spin_unlock( &dev->lock );

	if( ret < 0 ) {
        I2C_RESET;
        printk( "<error>em85xx_i2c_noreg_write: %d\n", ret );
       }
	return ret;

}

// assume that ADDRDIS is enabled.
int em85xx_i2c_noreg_read( char addr, char* byte, int size )
{
	int i=0;
	int ret = 0;
	uint32_t flags;

    spin_lock( &dev.lock );
    
	__save_flags (flags);
	__clf ();
	__cli ();

	if( I2C_IS_IDLE == 0 ) {
		ret = -1;
		goto __end;
	}

	REG_DWORD( I2C_MASTER_BYTE_CNT ) = size-1;
	REG_WORD( I2C_MASTER_DEV_ADDR ) = addr>>1;
	REG_WORD( I2C_MASTER_STARTXFER ) = 1;

	if( I2C_TX_COMPLETE == 0 ) {
		printk( "failed to complete\n" );
		ret = -2;
		goto __end;
	}
	if( I2C_ACK_ERROR ) {
		printk( "failed to receive ack: 0x%x\n", addr );
		ret = -3;
		goto __end;
	}

	for( i=0; i<size; i++ ) {
		if( I2C_RX_READY == 0 ) {
			ret = -4;
			printk( "error! RX unready : 0x%x\n", addr );
			goto __end;
		}
		byte[i] = ( char )REG_WORD( I2C_MASTER_DATAIN );
	}

__end:
    spin_unlock( &dev.lock );
    
	if( ret < 0 ) {
        I2C_RESET;
        printk( "<error>em85xx_i2c_noreg_read: %d\n", ret );
       }
	__restore_flags( flags );
	
	return ret;
}


int em85xx_i2c_write( char addr, char reg, const char* data, int size )
{
	int i, ret=size;
	const char* p = data;
	uint32_t flags;

    spin_lock( &dev.lock );
    
	__save_flags (flags);
	__clf ();
	__cli ();

	if( !I2C_IS_IDLE ) {
		ret = -1;
		goto __end;
	}

	REG_WORD( I2C_MASTER_DEV_ADDR ) = addr>>1;

	for( i=0; i<size; i++ ) {
		REG_DWORD( I2C_MASTER_BYTE_CNT ) = 0;
		REG_WORD( I2C_MASTER_ADR ) = reg++;
		REG_WORD( I2C_MASTER_DATAOUT ) = *p++;
		REG_WORD( I2C_MASTER_STARTXFER ) = 0;
		// wait until DOUTEMPTY is true.
		//mdelay(2);
		if( I2C_TX_COMPLETE == 0 ) {
			ret = -2;
			printk("error! tx uncomplete@em85xx_i2c_write dev=0x%x\n", addr);
			goto __end;
		}
		if( I2C_ACK_ERROR ) {
			ret = -3;
			goto __end;
		}
		
	}

__end:
    spin_unlock( &dev.lock );
    
	if( ret < 0 ) {
        I2C_RESET;
        printk( "<error>em85xx_i2c_write: %d\n", ret );
       }
	__restore_flags( flags );

	return ret;
}

int em85xx_i2c_seqwrite( char addr, char reg, const char* data, int size )
{
	int i, ret=size;
	uint32_t flags;

    spin_lock( &dev.lock );
    
	__save_flags (flags);
	__clf ();
	__cli ();

	if( I2C_IS_IDLE == 0 ) {
		ret = -1;
		goto __end;
	}

	REG_DWORD( I2C_MASTER_BYTE_CNT ) = size-1;
	REG_WORD( I2C_MASTER_DEV_ADDR ) = addr>>1;
	REG_WORD( I2C_MASTER_ADR ) = reg;
	REG_WORD( I2C_MASTER_STARTXFER ) = 0;

	if( I2C_TX_COMPLETE == 0 ) {
		ret = -1;
		goto __end;
	}
	if( I2C_ACK_ERROR ) {
		printk( "failed to receive ack(write)\n" );
		ret = -2;
		goto __end;
	}
	for( i=0; i<size; i++ ) {
		REG_WORD( I2C_MASTER_DATAOUT ) = data[i];
		//mdelay(2);
		if( I2C_TX_COMPLETE == 0 ) {
			ret = -3;
			printk("error! tx uncomplete@em85xx_i2c_seqwrite dev=0x%x\n", addr);
			goto __end;
		}
		if( I2C_ACK_ERROR ) {
			printk( "failed to receive ack(write)\n" );
			ret = -2;
			goto __end;
		}
		
	}

__end:
    spin_unlock( &dev.lock );
    
	if( ret < 0 ) {
        I2C_RESET;
            printk( "<error>em85xx_i2c_seqwrite: %d\n", ret );
       }
	__restore_flags( flags );

	return ret;
}


int em85xx_i2c_read( char addr, char reg, char* data, int size )
{
	int i=0, ret=size;
	unsigned short old;
	char* pdata = data;
	uint32_t flags;

    spin_lock( &dev.lock );
    
	__save_flags (flags);
	__clf ();
	__cli ();

	while( (REG_WORD(I2C_MASTER_STATUS)&1) == 0 ) {
		if( REG_WORD(I2C_MASTER_STATUS)&8 ) {
			ret = -1;
			goto __end;
		}
	}

	// disable to transfer register address.
	old = REG_WORD( I2C_MASTER_CONFIG );
	for( i=0; i<size; i++ ) {
		REG_WORD( I2C_MASTER_CONFIG ) = 0xFA; 
		REG_WORD( I2C_MASTER_BYTE_CNT ) = 0;
		REG_WORD( I2C_MASTER_DEV_ADDR ) = addr>>1;
		REG_WORD( I2C_MASTER_DATAOUT ) = reg++;
		REG_WORD( I2C_MASTER_STARTXFER ) = 0x04;
		while( (REG_WORD(I2C_MASTER_STATUS)&2) == 0 );
		while( (REG_WORD(I2C_MASTER_STATUS)&1) == 0 );
		REG_WORD( I2C_MASTER_CONFIG ) = 0xFA; 
		REG_WORD( I2C_MASTER_BYTE_CNT ) = 0;
		REG_WORD( I2C_MASTER_DEV_ADDR ) = addr>>1;
		REG_WORD( I2C_MASTER_STARTXFER ) = 1;

		while( (REG_WORD(I2C_MASTER_STATUS)&4) == 0 );
		while( (REG_WORD(I2C_MASTER_STATUS)&1) == 0 );
		*pdata++ = ( char )REG_WORD( I2C_MASTER_DATAIN );
	}

	// restore configure register.
	REG_WORD( I2C_MASTER_CONFIG ) = old;

__end:
    spin_unlock( &dev.lock );
    
    if( ret < 0 ) {
        I2C_RESET;
        printk( "<error>em85xx_i2c_read: %d\n", ret );
    }
	__restore_flags( flags );
	return ret;
}

int em85xx_i2c_seqread( char addr, char reg, char* byte, int size )
{
	int i=0, ret = size;
	uint32_t flags;

    spin_lock( &dev.lock );
    
	__save_flags (flags);
	__clf ();
	__cli ();

	if( I2C_IS_IDLE == 0 ) {
		ret = -1;
		goto __end;
	}

	REG_DWORD( I2C_MASTER_BYTE_CNT ) = size-1;
	REG_WORD( I2C_MASTER_DEV_ADDR ) = addr>>1;
	REG_WORD( I2C_MASTER_ADR ) = reg;
	REG_WORD( I2C_MASTER_STARTXFER ) = 1;

	if( I2C_TX_COMPLETE == 0 ) {
		printk( "failed to complete\n" );
		ret = -2;
		goto __end;
	}

	for( i=0; i<size; i++ ) {
		if( I2C_RX_READY == 0 ) {
			ret = -3;
			goto __end;
		}
		byte[i] = ( char )REG_WORD( I2C_MASTER_DATAIN );
	}

__end:
    spin_unlock( &dev.lock );
    
	if( ret < 0 ) {
        I2C_RESET;
        printk( "<error>em85xx_i2c_seqread: %d\n", ret );
       }
	return ret;
}

static int __init em85xx_i2cm_init( void )
{
	dev.dev_addr_len = 7;
	dev.reg_addr_len = 8;
	dev.dev_addr_disable = 0;
	dev.reg_addr_disable = 0;
	dev.clk_div = 440;
	dev.timeout = LOOP_TIMEOUT;

    spin_lock_init( &dev.lock );

	em85xx_i2c_set_ctrl( &dev );
	printk( "EM85xx I2C Master Driver Ver. %d.%d\n", VER_MAJOR, VER_MINOR );

	//data = 0xc0;
	//em85xx_i2c_write( 0x34, 0x32, &data, 1 );
	return 0;
}

static void __exit em85xx_i2cm_exit( void )
{
}

module_init( em85xx_i2cm_init );
module_exit( em85xx_i2cm_exit );


/* export symbols */
EXPORT_SYMBOL( em85xx_i2c_set_ctrl );
EXPORT_SYMBOL( em85xx_i2c_get_ctrl );
EXPORT_SYMBOL( em85xx_i2c_write );
EXPORT_SYMBOL( em85xx_i2c_read );
EXPORT_SYMBOL( em85xx_i2c_seqwrite );
EXPORT_SYMBOL( em85xx_i2c_seqread );
EXPORT_SYMBOL( em85xx_i2c_noreg_read );
EXPORT_SYMBOL( em85xx_i2c_noreg_write );


MODULE_AUTHOR( "Lee Seok<zstein@newmedialife.com>" );
MODULE_DESCRIPTION( "EM85xx I2C Master driver" );
MODULE_LICENSE( "GPL" );
