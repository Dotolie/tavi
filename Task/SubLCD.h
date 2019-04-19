//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Sub LCD Control functions
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/SubLCD.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: SubLCD.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.8  2005/06/24 05:11:00  zstein
// - deleted unused functions.
//
// Revision 1.7  2005/06/21 06:13:45  zstein
// - deleted some functions.
//
// Revision 1.6  2005/05/16 03:29:37  ywkim
// Change value of SUBLCD_TEXT_AREA_X & SUBLCD_TEXT_AREA_WIDTH
//
// Revision 1.5  2005/05/15 11:25:30  zstein
// - added a parameter for select plane.
//   SubLCD_GetPlaneBuffer(...), SubLCD_Battery(...), SubLCD_SetIcon(...),
//   SubLCD_ClearArea(...), SubLCD_ClearTextArea(...), SubLCD_ClearIconArea(...).
//   The above functions are compatible with an old version.
//
// Revision 1.4  2005/05/14 13:47:01  zstein
// - added SubLCD_ClearTextArea( x, y, width, height ).
//   You need to call *SubLCD_Flush(0)* after this fuction called.
//
// Revision 1.3  2005/05/11 13:47:20  zstein
// - added void SubLCD_ClearIconArea( void ).
//
// Revision 1.2  2005/04/27 05:06:18  zstein
// added SetIcon(...)
// added ClearArea( ... )
//
// Revision 1.1  2005/04/20 11:39:05  zstein
// SUB-LCD interfaece.
//
//
//*******************************************************************************

#ifndef __SUB_LCD_H__
#define __SUB_LCD_H__

extern void SubLCD_Contrast( int contrast );
extern void SubLCD_ON( bool onoff );
extern void SubLCD_Flush( int plane );
extern char* SubLCD_GetPlaneBuffer( int plane=0 );
extern void SubLCD_Reverse( bool onoff );
extern void SubLCD_BitBlt( char* pDst, int dx, int dy, char* pSrc, int sx, int sy, int w, int h );
extern void SubLCD_Init( void );
extern void SubLCD_Exit( void );
extern void SubLCD_ShowLOGO( void );
extern void SubLCD_ShowShutDown( void );
extern void SubLCD_Clear( void );
extern void SubLCD_ClearArea( int x, int y, int w, int h, int planeno=0 );
extern int SubLCD_XResolution( void );
extern int SubLCD_YResolution( void );

#endif /* __SUB_LCD_H__ */
