//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : ImageFont Width Define
// 
// $Source: /home/cvsroot/TAVI020/tavi/include/ImageFont.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:32:49 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: ImageFont.h,v $
// Revision 1.2  2006/06/07 08:32:49  zstein
// - removed unused codes.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/12/22 05:13:43  etnlwind
// Update for New Version
//
// Revision 1.10  2005/11/21 12:00:33  etnlwind
// Update for AlarmClock & Sound Palette UI
//
// Revision 1.9  2005/11/21 02:51:47  etnlwind
// Update for EQ Select UI
//
// Revision 1.8  2005/11/02 03:27:08  ywkim
// Change width of '.' to 4 from2 for digit3 font
//
// Revision 1.7  2005/10/21 05:37:16  etnlwind
// Update for Booting Progress
// Update for Booting Progress, Alarm MsgBox, Palette Management
//
// Revision 1.6  2005/10/13 01:29:18  etnlwind
// *** empty log message ***
//
// Revision 1.5  2005/10/13 01:25:25  etnlwind
// Update for Header Info
//
//*****************************************************************************


/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * ImageFont.h
 * Sang-jin Ryu (etnlwind@newmedialife.com)
 *
 */

#ifndef __IMAGE_FONT_H__
#define __IMAGE_FONT_H__

typedef enum {
	__DIGIT=0,
	__TEXT,
	__HSCROLL_TEXT,
	__VSCROLL_TEXT,
	__SKIP_TRANSPARENCY_COLOR,
	__DIGIT2,
	__DIGIT3
} eFONT_TYPE;


const extern int stDigit3[];
const extern int stAlarmDigit[];
const extern int stChno[];
const extern int stEqFontWidth[];

#endif

