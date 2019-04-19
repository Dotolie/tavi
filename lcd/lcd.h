//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Main LCD header file  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/lcd/lcd.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: lcd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/12/01 05:30:15  ywkim
// Change lcd_enable ( char data ) include  instruction of lcd reset
//
// Revision 1.3  2005/08/25 06:11:20  ywkim
// merge to main trunk
//
// Revision 1.2.2.2  2005/04/28 05:59:23  zstein
// demo merge.
//
// Revision 1.2.2.1  2005/03/04 16:12:52  ywkim
// Change parameters for gamma
//
// Revision 1.2  2005/02/05 13:40:38  ywkim
// Insert routine for samsung LCD
//
//*****************************************************************************


#ifndef _LCD_H
#define _LCD_H


extern int lcd_enable ( char data );
extern int lcd_brightness (int direction);
extern int lcd_contrast (int direction);
extern int lcd_comdc (int direction);
extern int lcd_ledcontrol (int direction);
extern int lcd_disable (void);

extern int lcd_dim (void);
extern int lcd_normal (void);
extern int lcd_black (void);

//int lcd_SetIndexReg(unsigned char cRegNo);

//int lcd_WriteControlReg(unsigned short sRegData);
#endif
