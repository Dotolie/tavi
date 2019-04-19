//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Real Time Clock (PCF8563)  driver for TAVI
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/rtc/tavi_rtc.c,v $
// $Revision: 1.3 $
// $Date: 2006/06/12 04:42:24 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: tavi_rtc.c,v $
// Revision 1.3  2006/06/12 04:42:24  ywkim
// Add function of pcf8563_timer_value() for melon
// Change some routine for above
//
// Revision 1.2  2006/03/08 12:16:57  zstein
// - error exception handle.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.14  2006/02/11 05:00:03  zstein
// - added periodic timer functions but they are commented.
//
// Revision 1.13  2006/02/07 09:16:40  zstein
// - renamed DEVICE_ADDRESS
//
// Revision 1.12  2005/12/11 15:28:08  zstein
// *** empty log message ***
//
// Revision 1.11  2005/12/01 09:24:28  zstein
// - fixed timeset bug
//
// Revision 1.10  2005/12/01 03:30:42  zstein
// *** empty log message ***
//
// Revision 1.9  2005/12/01 01:07:09  zstein
// *** empty log message ***
//
// Revision 1.8  2005/11/28 08:37:48  zstein
// *** empty log message ***
//
// Revision 1.7  2005/11/18 05:05:19  zstein
// *** empty log message ***
//
// Revision 1.6  2005/11/11 12:31:31  zstein
// *** empty log message ***
//
// Revision 1.5  2005/11/10 08:54:40  zstein
// *** empty log message ***
//
// Revision 1.4  2005/11/10 01:11:30  zstein
// *** empty log message ***
//
// Revision 1.3  2005/08/12 03:12:53  zstein
// - T0
//
// Revision 1.2  2005/05/12 04:34:49  zstein
// - automatically convert 19xx and 2xxx.
//
// Revision 1.1  2005/03/30 06:13:10  ywkim
// Initially add
//
//*****************************************************************************

#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fcntl.h>
#include <linux/miscdevice.h>
#include <asm/arch/hardware.h>
#include <asm/uaccess.h>

#include "../i2cm/i2cm.h"
#include "tavi_rtc.h"

#define DEBUG
#ifdef DEBUG
#define DEBUGMSG				printk
#else
#define DEBUGMSG
#endif


 // Seconds/VL
#define VL						0x80

#define DECIMAL_TO_BCD( dec )	( (dec/10)<<4 | (dec%10) )
#define BCD_TO_DECIMAL( bcd )	( (bcd>>4)*10 + (bcd&0x0f) )

#define BIT_RTC					13
#define VER_MAJOR				0
#define VER_MINOR				1

static struct tavi_rtc_dev {
    char busy;
    char irq;
}rtc_dev;

static int alarm = 0;

MODULE_PARM( alarm, "i" );

/* PCF8563 operators
 */
static struct rtc_time tavi_rtc_date_read( void )
{
	struct rtc_time rtime;
	char data;

	em85xx_i2c_read( I2C_RTC_ADDR, SECONDS_VL, (char*)&data, 1 );
	rtime.tm_vl  = (data&VL) ? 1 : 0;
	data &= 0x7f;
	rtime.tm_sec = BCD_TO_DECIMAL( data );
	em85xx_i2c_read( I2C_RTC_ADDR, MINUTES, (char*)&data, 1 );
	data &= 0x7f;
	rtime.tm_min = BCD_TO_DECIMAL( data );
	em85xx_i2c_read( I2C_RTC_ADDR, HOURS, (char*)&data, 1 );
	data &= 0x3f;
	rtime.tm_hour = BCD_TO_DECIMAL( data );
	em85xx_i2c_read( I2C_RTC_ADDR, DAYS, (char*)&data, 1 );
	data &= 0x3f;
	rtime.tm_mday = BCD_TO_DECIMAL( data );
	em85xx_i2c_read( I2C_RTC_ADDR, MONTHS_CENTURY, (char*)&data, 1 );
	rtime.tm_century = (data&0x80) ? 0 : 1;
	data &= 0x1f;
	rtime.tm_mon = BCD_TO_DECIMAL( data );
	em85xx_i2c_read( I2C_RTC_ADDR, YEARS, (char*)&data, 1 );
	rtime.tm_year = BCD_TO_DECIMAL( data );
	em85xx_i2c_read( I2C_RTC_ADDR, WEEKDAYS, (char*)&rtime.tm_wday, 1 );
	rtime.tm_wday &= 0x7;
	rtime.tm_yday = 0;
	rtime.tm_isdst = 0;

	if( rtime.tm_century ) rtime.tm_year += 100;
	if( rtime.tm_mon ) rtime.tm_mon--; // 0..11 for linux time system

	return rtime;
}

static void pcf8563_setdate( struct rtc_time rtime )
{
	char data;

    //printk( "setdate: yy: %d, mm: %d, day: %d, h: %d, m: %d, s: %d\n",
    //    rtime.tm_year, rtime.tm_mon, rtime.tm_mday, 
    //    rtime.tm_hour, rtime.tm_min, rtime.tm_sec );
	data = DECIMAL_TO_BCD( rtime.tm_sec );
	data |= rtime.tm_vl<<7;
	em85xx_i2c_write( I2C_RTC_ADDR, SECONDS_VL, (char*)&data, 1 );
	data = DECIMAL_TO_BCD( rtime.tm_min );
	em85xx_i2c_write( I2C_RTC_ADDR, MINUTES, (char*)&data, 1 );
	data = DECIMAL_TO_BCD( rtime.tm_hour );
	em85xx_i2c_write( I2C_RTC_ADDR, HOURS, (char*)&data, 1 );
	data = DECIMAL_TO_BCD( rtime.tm_mon );
	data |= (rtime.tm_century ? 0 : 1)<<7 ;
	em85xx_i2c_write( I2C_RTC_ADDR, MONTHS_CENTURY, (char*)&data, 1 );
	data = DECIMAL_TO_BCD( rtime.tm_year );
	em85xx_i2c_write( I2C_RTC_ADDR, YEARS, (char*)&data, 1 );
	data = rtime.tm_wday%10;
	em85xx_i2c_write( I2C_RTC_ADDR, WEEKDAYS, (char*)&data, 1 );
	data = DECIMAL_TO_BCD( rtime.tm_mday );
	em85xx_i2c_write( I2C_RTC_ADDR, DAYS, (char*)&data, 1 );
	data = rtime.tm_wday;
	em85xx_i2c_write( I2C_RTC_ADDR, WEEKDAYS, (char*)&data, 1 );
}

static int pcf8563_setalarm( struct rtc_time* rtctime )
{
	char min, hour, day, wday;
	//if( rtctime->tm_sec ) goto __invalid;
	if( rtctime->tm_min < 0 || rtctime->tm_min > 59 ) {
		printk( "invalid min: %d\n", rtctime->tm_min );
		goto __invalid;
	}
	if( rtctime->tm_hour < 0 || rtctime->tm_hour > 23 ) {
		printk( "invalid hour: %d\n", rtctime->tm_hour );
		goto __invalid;
	}

	min = DECIMAL_TO_BCD( rtctime->tm_min );
	hour = DECIMAL_TO_BCD( rtctime->tm_hour );
	//day = DECIMAL_TO_BCD( rtctime->tm_mday );
	//wday = DECIMAL_TO_BCD( rtctime->tm_wday );
	//printk( "alarm: %02x:%02x\n", hour, min );
	day = 0x80;
	wday = 0x80;
	em85xx_i2c_write( I2C_RTC_ADDR, MINUTE_ALARM, &min, 1 );
	em85xx_i2c_write( I2C_RTC_ADDR, HOUR_ALARM, &hour, 1 );
	em85xx_i2c_write( I2C_RTC_ADDR, DAY_ALARM, &day, 1 );
	em85xx_i2c_write( I2C_RTC_ADDR, WEEKDAY_ALARM, &wday, 1 );

	return 0;
__invalid:
    printk( "failed to setalarm\n" );
	return -EINVAL;
}

static int pcf8563_getalarm( struct rtc_time* rtime )
{
	char data;
	em85xx_i2c_read( I2C_RTC_ADDR, HOUR_ALARM, (char*)&data, 1 );
	rtime->tm_hour = data;
	em85xx_i2c_read( I2C_RTC_ADDR, MINUTE_ALARM, (char*)&data, 1 );
	rtime->tm_min = data;
}

static int pcf8563_aie_on( void )
{
	int control2 = 0x1A; // refer to pcf8563 datasheet
	int ret;
	ret = em85xx_i2c_write( I2C_RTC_ADDR, CONTROL_STAT2, (char*)&control2, 1 );
	return ret;
}

static int pcf8563_aie_off( void )
{
	//char control2 = 0x12;
	char data = 0x80;
	
	int ret;
	//ret = em85xx_i2c_write( I2C_RTC_ADDR, CONTROL_STAT2, (char*)&control2, 1 );
	ret = em85xx_i2c_write( I2C_RTC_ADDR, MINUTE_ALARM, &data, 1 );
	ret = em85xx_i2c_write( I2C_RTC_ADDR, HOUR_ALARM, &data, 1 );
	ret = em85xx_i2c_write( I2C_RTC_ADDR, DAY_ALARM, &data, 1 );
	ret = em85xx_i2c_write( I2C_RTC_ADDR, WEEKDAY_ALARM, &data, 1 );
	printk( "pcf8563_aie_off(%d)\n", ret );
	return 0;
}

static int pcf8563_timer_value( int flag, char *pdata )
{
	int ret;
	if ( flag ) {
		ret = em85xx_i2c_write( I2C_RTC_ADDR, TIMER, pdata, 1 );
		}
	else {
		ret = em85xx_i2c_read( I2C_RTC_ADDR, TIMER, pdata, 1 );
		}

	return ret;	
}


#if 0
/* ++

    pcf_8563_pie_on( void )

Description:
    Enables periodic timer.

Arguments:
    None

Return:
    Returns 0 on success otherwise 1.

-- */
static int pcf8563_pie_on( void )
{
    int ret = 0;
    char ctrl2;
    char 

    ret = em85xx_i2c_read( I2C_RTC_ADDR, CONTROL_STAT2, &ctrl2, 1 );
    if( ret < 0 ) goto __pie_on_end;
    // Set timer register
    
    ctrl2 |= (1<<TIE);
    ret = em85xx_i2c_write( I2C_RTC_ADDR, CONTROL_STAT2, &ctrl2, 1 );

__pie_on_end:
    return ret;
}

/* ++

    pcf8563_pie_off( void )

Description:
    Disables periodic timer.

Arguments:
    None

Return:
    Returns 0 on success, otherwise 1

-- */
static int pcf8563_pie_off( void )
{
    int ret = 0;
    char ctrl2;

    ret = em85xx_i2c_read( I2C_RTC_ADDR, CONTROL_STAT2, &ctrl2, 1 );
    if( ret < 0 ) goto __pie_off_end;
    ctrl2 &= ~(1<<TIE );
    ret = em85xx_i2c_write( I2C_RTC_ADDR, CONTROL_STAT2, &ctrl2, 1 );

__pie_off_end:
    return ret;
}

/* ++

    pcf8563_pie_set_timer( unsigned char time )

Description:
    Set periodic timer.

Arguments:
    time: countdown value

Return:
    Returns 0 on success otherwise 1

-- */
static int pcf8563_pie_set( unsigned char time )
{
    int ret = 0;

    
}
#endif

static void clear_irq( void )
{
	char old;
	em85xx_i2c_read( I2C_RTC_ADDR, CONTROL_STAT2, (char*)&old, 1 );
	if( old&(1<<3) ) {
		printk( "ALARM IS ASSERTED!!\n" );
		old &= 0x1f;
		old &= ~(1<<3);
		em85xx_i2c_write( I2C_RTC_ADDR, CONTROL_STAT2, (char*)&old, 1 );
	}
}

static int check_irq( void ) 
{
	char reg_stat2=0;
	if( em85xx_i2c_read( I2C_RTC_ADDR, CONTROL_STAT2, (char*)&reg_stat2, 1 ) <= 0 ) {
		printk( "failed to check_irq \n" );
        return 0;
	}
	//printk( "reg_stat2: 0x%02lx\n", reg_stat2 );
	// check alarm flag & interrupt enable
	if( reg_stat2&(1<<3) ) {
		if( reg_stat2&0x02 ) return 1;
		else clear_irq();
	}
	
	return 0;
}

#define EPROMADDR					0x3ff000

static void pcf8563_init( void )
{
	char control1;
	char control2;
	char clkctrl;

	printk( "alarm: %d\n", alarm );
	control1 = 0;
	control2 = 0x08; // refer to pcf8563 datasheet
	if( alarm ) control2 |= 0x02;
	
	//control2 = 0x12; // rtc clear
	clkctrl  = 0;

	em85xx_i2c_write( I2C_RTC_ADDR, CONTROL_STAT1, (char*)&control1, 1 );
	em85xx_i2c_write( I2C_RTC_ADDR, CONTROL_STAT2, (char*)&control2, 1 );
	em85xx_i2c_write( I2C_RTC_ADDR, CLK_CONTROL,   (char*)&clkctrl, 1 );
}

static int tavi_rtc_open( struct inode* inode, struct file* filep )
{
	if( rtc_dev.busy ) return -EBUSY;

	rtc_dev.busy = 1;

	return 0;
}

static int tavi_rtc_release( struct inode* inode, struct file* filep )
{
	rtc_dev.busy = 0;

	return 0;
}

static int tavi_rtc_ioctl( struct inode* inode, struct file* filep, unsigned int cmd,
		unsigned long arg ) 
{
	int ret=0;
	int level;
	char data;
	
	struct rtc_time rtime;

	switch( cmd ) {
	case RTC_AIE_ON:
		pcf8563_aie_on();
		break;
	case RTC_AIE_OFF:
		pcf8563_aie_off();
		break;
	case RTC_UIE_ON:
	case RTC_UIE_OFF:
	case RTC_PIE_ON:
	case RTC_PIE_OFF:
	case RTC_WIE_ON:
	case RTC_WIE_OFF:
		ret = -EINVAL;
		break;
	case RTC_ALM_SET:
		ret = pcf8563_setalarm( (struct rtc_time*)arg );
		break;
	case RTC_ALM_READ:
		ret = pcf8563_getalarm( (struct rtc_time*)arg );
		break;
	case RTC_RD_TIME:
		rtime = tavi_rtc_date_read();
		if( copy_to_user( (void*)arg, &rtime, sizeof(struct rtc_time) ) ) return -EFAULT;
		break;
	case RTC_SET_TIME:
		if( copy_from_user(&rtime, (struct rtime*)arg, sizeof(struct rtc_time)) ) return -EFAULT;
		pcf8563_setdate( rtime );
		break;
	case RTC_IRQP_READ:
	case RTC_IRQP_SET:
	case RTC_EPOCH_READ:
	case RTC_EPOCH_SET:
	case RTC_WKALM_SET:
	case RTC_WKALM_RD:
		ret = -EINVAL;
		break;
	case RTC_CLEAR_INT:
		clear_irq();
		break;
	case RTC_CHECK_INT:
		level = check_irq();
		if( copy_to_user( (void*)arg, &level, sizeof(int) ) ) return -EFAULT;
		break;
	case RTC_RD_TIMER:
		ret = pcf8563_timer_value( 0, &data );
		copy_to_user( (char*)arg, &data, sizeof(data) );
		break;
	case RTC_WR_TIMER:
		copy_from_user( &data, (char*)arg, sizeof(data) );
		ret = pcf8563_timer_value( 1, &data );
		break;
	}
	return ret;
}

static struct file_operations fops = {
	owner:	THIS_MODULE,
	ioctl:	tavi_rtc_ioctl,
	open:	tavi_rtc_open,
	release: tavi_rtc_release
};

static struct miscdevice rtc_misc_dev = {
	RTC_MINOR,
	"rtc",
	&fops
};


static int __init tavi_rtc_init( void )
{
	int ret=0;
	void* dev_id;

	rtc_dev.busy = 0;

	printk( "TAVI RTC drvier (alarm test) ver. %d.%d\n", VER_MAJOR, VER_MINOR );
	pcf8563_init();

	dev_id = ( void* )&rtc_dev;

	ret = misc_register( &rtc_misc_dev );

	return ret;
}

static void __exit tavi_rtc_exit( void )
{
}

module_init( tavi_rtc_init );
module_exit( tavi_rtc_exit );

MODULE_AUTHOR( "Lee Seok<zstein@newmedialife.com>" );
MODULE_DESCRIPTION( "RTC for TAVI" );
MODULE_LICENSE( "GPL" );
