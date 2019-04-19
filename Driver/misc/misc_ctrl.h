//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Misc. control driver
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/misc/misc_ctrl.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: misc_ctrl.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.12  2006/02/17 13:03:30  ywkim
// Remove CHARGER related function or macro
//
// Revision 1.11  2006/02/10 13:27:22  ywkim
// Change macro HW0602 to CHARGER at definition of CHARGER_ON
//
// Revision 1.10  2006/02/08 09:30:35  ywkim
// Review macro
//
// Revision 1.9  2006/02/04 06:00:26  ywkim
// Change port & initial values for HW0602
//
// Revision 1.8  2006/01/13 08:33:29  ywkim
// Change some function for New H/W, it enabled with compile option of HW0601
//
// Revision 1.7  2005/12/21 03:26:11  zstein
// *** empty log message ***
//
// Revision 1.6  2005/12/03 05:55:43  zstein
// *** empty log message ***
//
// Revision 1.5  2005/11/14 04:45:53  ywkim
// Change routine of tavi_misc_init() for SPEAKER_AMP off
//
// Revision 1.4  2005/11/11 12:31:07  zstein
// *** empty log message ***
//
// Revision 1.3  2005/10/25 02:45:33  ywkim
// 1. Change name of definition to POWER_LED from FM_POWER
// 2. Change initial value of tavi_misc_priv.miscstruct.ctrl_out to 86 from 96
//******************************************************************************

#ifndef __MISCELLANEOUS_CONTROL_DRIVER_H__
#define __MISCELLANEOUS_CONTROL_DRIVER_H__

#include <asm/ioctl.h>

struct misc_ctrl_struct {
	#define AC_DETECT			0
	#define USB_CONNECT		1
	#define FOLDER_OPEN			2
	#define RTC_INTR				3
	#define CRADLE_IN			4
	#define POWER_ON			5
	#define HP_DETECT			6
	#define HOST_IN				7
	char ctrl_in;
	#define MAIN_LCD_ONOFF		0	/* active high */
	#define MAIN_LCD_RESET		1	/* active low */
#ifdef HW0601
	#define MUTE_ON				2	/* active low */
#else
	#define POWER_OFF			2	/* active low */
#endif	
	#define USB_ENABLE			3	/* active high */
#ifndef HW0601
	#define POWER_LED			4	/* active low */
#endif	
	#define AMP_ON				5	/* active high */
#ifndef HW0601
	#define EXT_EL_ON			6	/* active high */
	#define KEY_LED_ON			7	/* active low */
#endif	
	char ctrl_out;
	char brightness; // range: 0 ~ 9
};

/* ioctl command */
#define TAVI_MISC_MAGIC			'M'
#define TAVI_MISC_IOCS_CONTROL	_IOW( TAVI_MISC_MAGIC, 1, struct misc_ctrl_struct )
#define TAVI_MISC_IOCG_CONTROL	_IOW( TAVI_MISC_MAGIC, 2, struct misc_ctrl_struct )
#define TAVI_MISC_IOCS_BRIGHTNESS	_IOW( TAVI_MISC_MAGIC, 3, struct misc_ctrl_struct )
#ifndef HW0601
#define TAVI_MISC_IOCS_MUTE_ON	_IO( TAVI_MISC_MAGIC, 4 )
#define TAVI_MISC_IOCS_MUTE_OFF	_IO( TAVI_MISC_MAGIC, 5 )
#endif
extern int speaker_amp_onoff( int onoff );
extern char TAVI_MISC_ReadData( void );
extern void TAVI_MISC_SendData( char data );

#endif /* __MISCELLANEOUS_CONTROL_DRIVER_H__ */

