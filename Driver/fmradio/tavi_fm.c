//************************ Copyright(C) New Media Life Inc. ********************
//
// Description : FM Radio(GNT-702S) Driver for TAVI
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/fmradio/tavi_fm.c,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: tavi_fm.c,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/08/25 06:07:12  ywkim
// Merge to main trunk
//
// Revision 1.3.2.1  2005/03/03 13:43:41  zstein
// *** empty log message ***
//
// Revision 1.3  2005/03/01 14:10:43  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/11 01:18:18  ywkim
// Modify parts of header
//
// Revision 1.1  2005/02/07 07:15:49  ywkim
// FMRadio driver file
//
//*****************************************************************************

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#include "../i2cm/i2cm.h"
#include "tavi_fm.h"
#include "../misc_group.h"

#define FM_I2C_ADDRESS 		0xC0
/*
#define FM_SSL_NOTALLOWED	0x00
#define FM_SSL_LOW			0x01
#define FM_SSL_MID			0x02
#define FM_SSL_HIGH			0x03

#define FM_AUDIO_LEFT		0x00
#define FM_AUDIO_RIGHT		0x01
#define FM_AUDIO_ALL		0x02

#define FM_XTAL_32KHZ		1
#define FM_XTAL_13MHZ		0


#define FM_BAND_JAPAN		1
#define FM_BAND_US_EU		0
*/
#define TAVI_FM_VER_MAJOR	0
#define TAVI_FM_VER_MINOR	8 		// pre-release version

static char cBusy;

/* FM Module Operations 
 * For more detail description, Plz. refer to ntea5767.pdf
 */

static int FM_SendData( char* pData )
{
	int ret;
	struct em85xx_i2c_dev i2cdev, old;

	old = i2cdev = em85xx_i2c_get_ctrl();
	i2cdev.reg_addr_disable = 1;
	em85xx_i2c_set_ctrl( &i2cdev );
	ret = em85xx_i2c_seqwrite( FM_I2C_ADDRESS, 0, pData, 5 );
	em85xx_i2c_set_ctrl( &old );

	return ret;
}

static int FM_ReadData( char* pData )
{
	int ret;
	struct em85xx_i2c_dev i2cdev, old;

	i2cdev = old = em85xx_i2c_get_ctrl();
	i2cdev.reg_addr_disable = 1;
	em85xx_i2c_set_ctrl( &i2cdev );
	ret = em85xx_i2c_seqread( FM_I2C_ADDRESS, 0, pData, 5 );
	em85xx_i2c_set_ctrl( &old );

	return ret;
}

static int FM_open( struct inode* inode, struct file* pfile )
{
	if( cBusy )
		return -EBUSY;

	MOD_INC_USE_COUNT;
	cBusy = 1;
	return 0;
}

static int FM_read(struct file* pFile, char *pcBuffer, int nCount, loff_t *pPos)
{
	if( FM_ReadData( pcBuffer ) < 0 )
		return -EACCES;
	else
		return 0;
}
static int FM_write(struct file* pFile, char *pcBuffer, int nCount, loff_t *pPos)
{
	if( FM_SendData( pcBuffer ) < 0 ) 
		return -EACCES;
	else
		return 0;
}
static int FM_release( struct inode* inode, struct file* pFile )
{
	cBusy = 0;
	MOD_DEC_USE_COUNT;
	return 0;
}


static struct file_operations fops = {
	owner: THIS_MODULE,
	open: FM_open,
	read: FM_read,
	write: FM_write,
	release: FM_release,
};

static struct miscdevice FM_miscdev = {
	TAVI_FM_MINOR,
	"fm",
	&fops
};

static int FM_init( void )
{
	char acBuff[5];
	
	printk( "FM Radio Driver for TAVI ver %d.%d\n", TAVI_FM_VER_MAJOR, TAVI_FM_VER_MINOR );
	memset(acBuff, 0,sizeof(acBuff) );
	acBuff[3] |= 0x40;					// FM Module STBY =1  set standby

	if( FM_SendData(acBuff) < 0 ) {
		return -1;
	}
	else
		return misc_register( &FM_miscdev );
}

static void FM_exit( void )
{
	misc_deregister( &FM_miscdev );
}

module_init( FM_init );
module_exit( FM_exit );

MODULE_AUTHOR( "Lee Seok<zstein@newmedialife.com>" );
MODULE_DESCRIPTION( "FM Driver for TAVI" );
MODULE_LICENSE( "GPL" );
