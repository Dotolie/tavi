//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Main LCD code file  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/lcd/lcd.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/03/07 02:08:15 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: lcd.cpp,v $
// Revision 1.3  2006/03/07 02:08:15  ywkim
// Disable oragne lcd & Enable samsung lcd
//
// Revision 1.2  2006/03/03 08:48:00  ywkim
// Insert instruction of  	*(UART1_ALTFUNCTION) = 0x00 at end of lcd_enable
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.15  2006/02/15 07:47:55  ywkim
// Move down instruction of lcd_SetIndexReg(9)
//
// Revision 1.14  2006/01/26 12:00:54  ywkim
// Change parameter of lcd for orange lcd
//
// Revision 1.13  2005/12/10 06:49:52  ywkim
// Remove delay in lcd_enable
//
// Revision 1.12  2005/12/10 06:32:56  ywkim
// Add delay time at end of lcd_enable
//
// Revision 1.11  2005/12/10 06:08:28  ywkim
// Change lcd reset routine
//
// Revision 1.10  2005/12/07 04:18:58  ywkim
// Add asm("nop") between TX_CLK_LOW & TX_CLK_HIGH
//
// Revision 1.9  2005/12/06 08:58:26  ywkim
// Add delay on reset & lcd_disable
//
// Revision 1.8  2005/12/01 06:37:21  ywkim
// Increase delay for lcd reset
//
// Revision 1.7  2005/12/01 05:30:15  ywkim
// Change lcd_enable ( char data ) include  instruction of lcd reset
//
// Revision 1.6  2005/12/01 03:17:47  ywkim
// disabel lcd reset
//
// Revision 1.5  2005/12/01 02:34:06  ywkim
// Change reset sequency of lcd
//
// Revision 1.4  2005/10/27 05:35:12  ywkim
// Remark access routine of poio0_11
//
// Revision 1.3  2005/08/25 06:11:20  ywkim
// merge to main trunk
//
// Revision 1.2.2.9  2005/08/16 05:19:00  ywkim
// Remove old lcd_reset routine
//
// Revision 1.2.2.8  2005/06/30 07:58:42  ywkim
// Change lcd_enable()
//
// Revision 1.2.2.7  2005/06/20 12:00:18  ywkim
// Change VBP parameter for samsung lcd
//
// Revision 1.2.2.6  2005/06/14 07:24:40  ywkim
// change lcd parameter
//
// Revision 1.2.2.5  2005/04/28 05:59:23  zstein
// demo merge.
//
// Revision 1.2.2.4  2005/03/06 19:12:27  ywkim
// Insert lcd enable & disable with pio0_4
//
// Revision 1.2.2.3  2005/03/05 12:37:39  ywkim
// Change some routine for only samsung lcd
//
// Revision 1.2.2.2  2005/03/04 16:12:52  ywkim
// Change parameters for gamma
//
// Revision 1.2.2.1  2005/03/03 07:24:00  ywkim
// setup for SAMSUNG LCD.
//
// Revision 1.2  2005/02/05 13:40:38  ywkim
// Insert routine for samsung LCD
//
//*****************************************************************************

#include "lcd.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "../Driver/misc/misc_ctrl.h"


#define FIP_COMMAND		((volatile unsigned long *)(0x500d00))
#define FIP_DISPLAY_DATA	((volatile unsigned long *)(0x500d04))
#define FIP_LED_DATA		((volatile unsigned long *)(0x500d08))
#define FIP_CLK_DIV			((volatile unsigned long *)(0x500d20))
#define FIP_TRISTATE		((volatile unsigned long *)(0x500d24))


static unsigned char ledcontrol;
static unsigned char comdc;
static unsigned char brightness;
static unsigned char contrast;


#define UART0_ALTFUNCTION		((volatile unsigned short*)(0x500530))
#define UART1_ALTFUNCTION		((volatile unsigned short *)(0x501330))
#define JASPER_PIO0_BASE		0x00500600
#define JASPER_PIO2_BASE		0x00501B00

#define pPIO0_DIR                   		( volatile int* )( JASPER_PIO0_BASE + 0x08 )
#define pPIO0_DATA                  	( volatile int* )( JASPER_PIO0_BASE + 0x04 )

#define pPIO2_DIR                   		( volatile int* )( JASPER_PIO2_BASE + 0x08 )
#define pPIO2_DATA                  	( volatile int* )( JASPER_PIO2_BASE + 0x04 )

#define LCD_CS					8
#define LCD_DATA				12
#define LCD_CLK					15

#define JASPER_PBI_SLAVE_BASE	0x008009C0
#define JASPER_PIO2_BASE		0x00501B00

#define PBI_SL_CTRL				0x00
#define PBI_SL_CTRL_REG			*(volatile unsigned long *)(JASPER_PBI_SLAVE_BASE + PBI_SL_CTRL)
#define PBI_MODE_LBC_PADS		0x0
#define PBI_MODE_IDE_PADS		0x8

//#define EX_LCD_CS				5

#define MK_REG( _bit, _level ) 		( (0x10000<<_bit) |(_level<<_bit) ) 

#define LCD_CS_HI				(*pPIO2_DATA =MK_REG(LCD_CS,1))
#define LCD_CS_LO				(*pPIO2_DATA =MK_REG(LCD_CS,0))

#define LCD_SCL_HI				(*pPIO2_DATA =MK_REG(LCD_CLK,1))
#define LCD_SCL_LO				(*pPIO2_DATA =MK_REG(LCD_CLK,0))

#define LCD_SDI_HI				(*pPIO2_DATA =MK_REG(LCD_DATA,1))
#define LCD_SDI_LO				(*pPIO2_DATA =MK_REG(LCD_DATA,0))

#define TX_CLK_HIGH				( *pPIO0_DATA = MK_REG( 14, 1 ) )
#define TX_CLK_LOW				( *pPIO0_DATA = MK_REG( 14, 0 ) )


//#define LCD_DISABLE			(*pPIO0_DATA = MK_REG(EX_LCD_CS,0) )
//#define LCD_ENABLE				(*pPIO0_DATA = MK_REG(EX_LCD_CS,1) )

#define DISABLE_GLOBAL_INT		(*((volatile int *)0x00500224) &= ~0x80000000)
#define ENABLE_GLOBAL_INT		(*((volatile int *)0x00500224) |= 0x80000000)

#define LCD_GETBIT(_sTemp, _bit )	( ( _sTemp & ( 0x0001 << _bit)) ? 1: 0 )				


typedef struct _tag_LEDREG {
	unsigned char		cRegNo;
	unsigned short 	sRegData;
}	LCD_REGSET_t;
LCD_REGSET_t	tLCDFUN1[3] =
{
	{ 0x09,	0x4000 },
	{ 0x0a,	0x2000 },
	{ 0xff, 0xff }
};
LCD_REGSET_t	tLCDFUN2[2] =
{
	{ 0x09,	0x4055 },
	{ 0xff, 0xff }
};
LCD_REGSET_t	tLCDDSPSET[19] =
{
	// new parameter for gamma 05.03.04
	{ 0x01,	0x409d },
	{ 0x02,	0x0204 },
	{ 0x03,	0x0100 },
	{ 0x04,	0x3000 },
	{ 0x05,	0x4003 },
#if 1	
	{ 0x06,	0x0007 },
	{ 0x07,	0x0014 },
#else	// for orange lcd
	{ 0x06,	0x0013 },
	{ 0x07,	0x0044 },
#endif
	{ 0x08,	0x0c00 },
	{ 0x10,	0x0103 },
	{ 0x11,	0x0301 },
	{ 0x12,	0x1f0f },
	{ 0x13,	0x1f0f },
	{ 0x14,	0x0707 },
	{ 0x15,	0x0307 },
	{ 0x16,	0x0707 },
	{ 0x17,	0x0000 },
	{ 0x18,	0x0004 },
	{ 0x19,	0x0000 },
	{ 0xff, 0xff }
};
LCD_REGSET_t	tLCDDSPON[3] =
{
	{ 0x09,	0x4a55 },
	{ 0x05,	0x5003 },
	{ 0xff, 0xff }
};

LCD_REGSET_t	tLCDDSPOFF0[3] =
{
	{ 0x09,	0x4055 },
	{ 0x05,	0x4003 },
	{ 0xff, 0xff }
};

LCD_REGSET_t	tLCDDSPOFF1[2] =
{
	{ 0x0a,	0x0000 },
	{ 0xff, 0xff }
};

LCD_REGSET_t	tLCDDSPOFF2[2] =
{
	{ 0x09,	0x4000 },
	{ 0xff, 0xff }
};
LCD_REGSET_t	tLCDDSPBLACK[3] =
{
	{ 0x09,	0x4055 },
	{ 0x01,	0x401d },
	{ 0xff, 0xff }
};
LCD_REGSET_t	tLCDDSPNORMAL[3] =
{
	{ 0x01,	0x409d },
	{ 0x09,	0x4a55 },
	{ 0xff, 0xff }
};
int
lcd_SetIndexReg(unsigned char cRegNo)
{
	int	i;
	int	j;
	unsigned short sTemp;
	unsigned char cBit;

	LCD_CS_LO;
	LCD_SCL_HI;


	sTemp = ( 0x0e |0x20 |0x00);	// b7: r/w, b6 c/i, b5~b0 :0x00x00111[id]
	for (i=0;i<8;i++)
	{
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_LO;
		cBit = LCD_GETBIT(sTemp, i );
		if ( cBit > 0 )
			LCD_SDI_HI;
		else
			LCD_SDI_LO;
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_HI;
	}
	sTemp = 0x00;
	for (i=0;i<8;i++)
	{
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_LO;
		cBit = LCD_GETBIT(sTemp, i );
		if ( cBit > 0 )
			LCD_SDI_HI;
		else
			LCD_SDI_LO;
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_HI;
	}
	sTemp = cRegNo;
	for (i=7;i>=0;i--)
	{
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_LO;
		cBit = LCD_GETBIT(sTemp, i );
		if ( cBit > 0 )
			LCD_SDI_HI;
		else
			LCD_SDI_LO;
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_HI;
	}
	for(j=0;j<100;j++);	// for 3us delay
	LCD_CS_HI;

	return 0;
}

int
lcd_WriteControlReg(unsigned short sRegData)
{
	int	i;
	int	j;
	unsigned short sTemp;
	unsigned char cBit;

	usleep( 100 );
	
	LCD_CS_LO;
	LCD_SCL_HI;
	for(j=0;j<100;j++);	// for 3us delay
	sTemp = ( 0x0e | 0x20 | 0x40);	// b7: r/w, b6 c/i, b5~b0 :0x00x00111[id]
	for (i=0;i<8;i++)
	{
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_LO;
		cBit = LCD_GETBIT(sTemp, i );
		if ( cBit > 0 )
			LCD_SDI_HI;
		else
			LCD_SDI_LO;
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_HI;
	}
	sTemp = sRegData;
	for (i=15;i>=0;i--)
	{
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_LO;
		cBit = LCD_GETBIT(sTemp, i );
		if ( cBit > 0 )
			LCD_SDI_HI;
		else
			LCD_SDI_LO;
		for(j=0;j<100;j++);	// for 3us delay
		LCD_SCL_HI;
	}
	for(j=0;j<100;j++);	// for 3us delay
	LCD_CS_HI;

	return 0;
}



int lcd_enable ( char data)
{
	int 	i;
	int	j;
	unsigned char cNo;
	unsigned short sData;

	PBI_SL_CTRL_REG = PBI_MODE_LBC_PADS;
 	*(UART1_ALTFUNCTION) = 0x01;
  	
	// for LCD_CS
	*pPIO2_DIR = MK_REG( LCD_CS, 1 );			// pio2 direction output
	*pPIO2_DATA = MK_REG( LCD_CS, 1 );			// pio b12 = 1
	// for LCD_SCL
	*pPIO2_DIR = MK_REG( LCD_CLK, 1 );			// pio2 direction output
	*pPIO2_DATA = MK_REG( LCD_CLK, 1 );			// pio b12 = 1
	// for LCD_SDI
	*pPIO2_DIR = MK_REG( LCD_DATA, 1 );			// pio2 direction output
	*pPIO2_DATA = MK_REG( LCD_DATA, 1 );			// pio b12 = 1

	DISABLE_GLOBAL_INT;
	// RESETB for LCD
	*(UART0_ALTFUNCTION) = 0x01;

	*pPIO2_DATA = 0x00ff0000 | (0xfd & data);		// pio2_b2 = 0
	*pPIO2_DIR = 0x00ff00ff;

	*pPIO0_DIR = MK_REG( 14, 1 );
	TX_CLK_LOW;
	asm ("nop");
	TX_CLK_HIGH;
	*pPIO2_DATA = 0x00ff00ff;

	for(j=0;j<500;j++);  							// delay

	*pPIO2_DATA = (0x00ff0000 | (0xff & data));	// pio2_b2 = 1
	*pPIO2_DIR = 0x00ff00ff;

	TX_CLK_LOW;
	asm ("nop");
	TX_CLK_HIGH;
	*pPIO2_DATA = 0x00ff00ff;

	*(UART0_ALTFUNCTION) = 0x00;	
	// RESETB for LCD
	ENABLE_GLOBAL_INT;

	lcd_SetIndexReg(9);
	lcd_WriteControlReg(0);
	
	for(j=0;j<800000;j++);  						// for 20mS delay

	i=0;            // function 1
	while ( 1 )
	{
		cNo = tLCDFUN1[i].cRegNo;
		sData = tLCDFUN1[i].sRegData;
		if( cNo == 0xff )
		    break;
		lcd_SetIndexReg(cNo);
		lcd_WriteControlReg(sData);
		i++;
	};

	i=0;            // function 2
	while ( 1 )
	{
		cNo = tLCDFUN2[i].cRegNo;
		sData = tLCDFUN2[i].sRegData;
		if( cNo == 0xff )
		    break;
		lcd_SetIndexReg(cNo);
		lcd_WriteControlReg(sData);
		i++;
	};
	//usleep (50000);
	for(j=0;j<2000000;j++);  // for 50mS delay

	i=0;            // display setting
	 while ( 1 )
	 {
		cNo = tLCDDSPSET[i].cRegNo;
		sData = tLCDDSPSET[i].sRegData;
		if( cNo == 0xff )
		    break;
		lcd_SetIndexReg(cNo);
		lcd_WriteControlReg(sData);
		i++;
	};
	//usleep (30000);
	for(j=0;j<1200000;j++);  // for 30mS delay

	i=0;            // display on
	while ( 1 )
	{
		cNo = tLCDDSPON[i].cRegNo;
		sData = tLCDDSPON[i].sRegData;
		if( cNo == 0xff )
		    break;
		lcd_SetIndexReg(cNo);
		lcd_WriteControlReg(sData);
		i++;
	};
	
 	*(UART1_ALTFUNCTION) = 0x00;
 	PBI_SL_CTRL_REG = PBI_MODE_IDE_PADS;
	return 0;
}

int lcd_brightness (int direction)
{
	return -1;
}

int lcd_contrast (int direction)
{
	return -1;
}

int lcd_comdc (int direction)
{
	return -1;
}

int lcd_ledcontrol (int direction)
{
	return -1;
}

int lcd_disable (void)
{	
	int 	i;
	unsigned char cNo;
	unsigned short sData;

	PBI_SL_CTRL_REG = PBI_MODE_LBC_PADS;
 	*(UART1_ALTFUNCTION) = 0x01;
  	
  	
	// for LCD_CS
	*pPIO2_DIR = MK_REG( LCD_CS, 1 );				// pio2 direction output
	*pPIO2_DATA = MK_REG( LCD_CS, 1 );			// pio b2 = 1
	// for LCD_SCL
	*pPIO2_DIR = MK_REG( LCD_CLK, 1 );				// pio2 direction output
	*pPIO2_DATA = MK_REG( LCD_CLK, 1 );			// pio b2 = 1
	// for LCD_SDI
	*pPIO2_DIR = MK_REG( LCD_DATA, 1 );				// pio2 direction output
	*pPIO2_DATA = MK_REG( LCD_DATA, 1 );			// pio b2 = 1

 	i=0;			// off0
	while ( 1 )
	{
		cNo = tLCDDSPOFF0[i].cRegNo;
		sData = tLCDDSPOFF0[i].sRegData;
		if( cNo == 0xff ) 
			break;
		lcd_SetIndexReg(cNo);
		lcd_WriteControlReg(sData);
		i++;
	};
 	for(i=0;i<40000;i++);

 	i=0;			// off1
	while ( 1 )
	{
		cNo = tLCDDSPOFF1[i].cRegNo;
		sData = tLCDDSPOFF1[i].sRegData;
		if( cNo == 0xff ) 
			break;
		lcd_SetIndexReg(cNo);
		lcd_WriteControlReg(sData);
		i++;
	};
//	usleep (1000);	// delay 1mS
	for(i=0;i<40000;i++);
	
	i=0;			// off 2
	while ( 1 )
	{
		cNo = tLCDDSPOFF2[i].cRegNo;
		sData = tLCDDSPOFF2[i].sRegData;
		if( cNo == 0xff ) 
			break;
		lcd_SetIndexReg(cNo);
		lcd_WriteControlReg(sData);
		i++;
	};

 	*(UART1_ALTFUNCTION) = 0x00;
	PBI_SL_CTRL_REG = PBI_MODE_IDE_PADS;

	for(i=0;i<400000;i++);
	
	return 0;
}

static int in_dim_mode = 0;
int lcd_dim (void)
{	
	return 0;
}

int lcd_normal (void)
{	
	int fd;
	fd = open( "/dev/tmisc", O_RDONLY );
	if( fd < 0 ) {
		printf( "failed to open /dev/tmisc" );
	}
	else {
		struct misc_ctrl_struct dev;
		ioctl( fd, TAVI_MISC_IOCG_CONTROL, &dev );
		dev.ctrl_out |= ( 1<<MAIN_LCD_ONOFF );
		ioctl( fd, TAVI_MISC_IOCS_CONTROL, &dev );
		close( fd );
	}
	return 0;
}

int lcd_black (void)
{	
	int fd;
	fd = open( "/dev/tmisc", O_RDONLY );
	if( fd < 0 ) {
		printf( "failed to open /dev/tmisc" );
	}
	else {
		struct misc_ctrl_struct dev;
		ioctl( fd, TAVI_MISC_IOCG_CONTROL, &dev );
		dev.ctrl_out &= ~( 1<<MAIN_LCD_ONOFF );
		ioctl( fd, TAVI_MISC_IOCS_CONTROL, &dev );
		close( fd );
	}
	return 0;
}
