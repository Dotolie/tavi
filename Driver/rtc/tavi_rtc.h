//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Real Time Clock (PCF8563)  driver for TAVI
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/rtc/tavi_rtc.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/12 04:43:28 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: tavi_rtc.h,v $
// Revision 1.2  2006/06/12 04:43:28  ywkim
// Add definition of RTC_RD_TIMER & RTC_WR_TIMER for melon
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2006/02/11 05:00:03  zstein
// - added periodic timer functions but they are commented.
//
// Revision 1.5  2006/02/07 09:18:07  zstein
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

#ifndef __TAVI_RTC_H__
#define __TAVI_RTC_H__

#include <asm/ioctl.h>

#define MACHINE_TAVI	1

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
#if MACHINE_TAVI
	int tm_century; // 1 : 20xx, 0: 19xx, not used by Lee Seok
	int tm_vl; // low voltage 
#endif
};

struct rtc_wkalrm {
	unsigned char enabled; /* 0 = alarm disable, 1 = alarm enable */
	unsigned char pending; /* 0 = alarm pending, 1 = alarm not pending */
	struct rtc_time time;  /* time the alarm is set to */
};

#define I2C_RTC_ADDR			0xA2

// Register address
#define CONTROL_STAT1			0x00
#define CONTROL_STAT2			0x01
#define SECONDS_VL				0x02
#define MINUTES					0x03
#define HOURS					0x04
#define DAYS					0x05
#define WEEKDAYS				0x06
#define MONTHS_CENTURY			0x07
#define YEARS					0x08
#define MINUTE_ALARM			0x09
#define HOUR_ALARM				0x0A
#define DAY_ALARM				0x0B
#define WEEKDAY_ALARM			0x0C
#define CLK_CONTROL				0x0D
#define TIMER_CONTROL			0x0E
#define TIMER					0x0F

/*
    Control/Stat 1 BITS
*/
#define TESTC                       3
#define STOP                        5
#define TEST1                       7

/*
    Control/Stat2 BITS
*/
#define TIE                         0
#define AIE                         1
#define TF                          2
#define AF                          3
#define TITF                        4

/*
    CLOCKOUT Control BITS
*/
#define FD0                         0
#define FD1                         1

/*
    TIMER CONTROL BITS
*/
#define TD0                         0
#define TD1                         1


#define RTC_AIE_ON  	_IO('p', 0x01)  /* Alarm int. enable on     */
#define RTC_AIE_OFF 	_IO('p', 0x02)  /* ... off          */
#define RTC_UIE_ON  	_IO('p', 0x03)  /* Update int. enable on    */
#define RTC_UIE_OFF 	_IO('p', 0x04)  /* ... off          */
#define RTC_PIE_ON  	_IO('p', 0x05)  /* Periodic int. enable on  */
#define RTC_PIE_OFF 	_IO('p', 0x06)  /* ... off          */
#define RTC_WIE_ON  	_IO('p', 0x0f)  /* Watchdog int. enable on  */
#define RTC_WIE_OFF 	_IO('p', 0x10)  /* ... off          */

#define RTC_ALM_SET 	_IOW('p', 0x07, struct rtc_time) /* Set alarm time  */
#define RTC_ALM_READ    _IOR('p', 0x08, struct rtc_time) /* Read alarm time */
#define RTC_RD_TIME 	_IOR('p', 0x09, struct rtc_time) /* Read RTC time   */
#define RTC_SET_TIME    _IOW('p', 0x0a, struct rtc_time) /* Set RTC time    */
#define RTC_IRQP_READ   _IOR('p', 0x0b, unsigned long)   /* Read IRQ rate   */
#define RTC_IRQP_SET    _IOW('p', 0x0c, unsigned long)   /* Set IRQ rate    */
#define RTC_EPOCH_READ  _IOR('p', 0x0d, unsigned long)   /* Read epoch      */
#define RTC_EPOCH_SET   _IOW('p', 0x0e, unsigned long)   /* Set epoch       */

#define RTC_WKALM_SET   _IOW('p', 0x0f, struct rtc_wkalrm)/* Set wakeup alarm*/
#define RTC_WKALM_RD    _IOR('p', 0x10, struct rtc_wkalrm)/* Get wakeup alarm*/
/*
    TAVI EXTENSIONS
*/
#define RTC_CLEAR_INT	_IO('p', 0x11 )
#define RTC_CHECK_INT	_IOW('p', 0x12, int)
//efine RTC_PIE_SET         _IOW('p', 0x13, int) /

#define RTC_RD_TIMER	_IOR('p', 0x13, char)
#define RTC_WR_TIMER	_IOW('p', 0x14, char)



#endif /* __TAVI_RTC_H__ */
