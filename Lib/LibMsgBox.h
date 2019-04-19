//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibMsgBox Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibMsgBox.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibMsgBox.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.17  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.16  2005/12/19 06:40:56  etnlwind
// Update for Reset parameters
//
// Revision 1.15  2005/11/24 10:56:46  etnlwind
// Update for New EQ
//
// Revision 1.14  2005/11/21 12:00:29  etnlwind
// Update for AlarmClock & Sound Palette UI
//
// Revision 1.13  2005/11/16 08:48:49  etnlwind
// Update for AlarmSet
//
// Revision 1.12  2005/10/28 11:15:15  etnlwind
// Update for Palette
//
// Revision 1.11  2005/10/21 05:35:35  etnlwind
// Update for Booting Progress
// Update for Booting Progress, Alarm MsgBox
//
// Revision 1.10  2005/10/18 09:05:13  etnlwind
// Update for New DialgBox, MessageBox UI
//
// Revision 1.9  2005/09/27 09:11:07  etnlwind
// Update for VideoPlayer, FileManager
//
// Revision 1.8  2005/08/24 13:07:55  etnlwind
// Update for Virtual Keyboard
//
// Revision 1.7  2005/08/08 10:49:41  etnlwind
// Update for FileManger, VideoPlayer
//
// Revision 1.6  2005/07/13 06:02:31  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.5  2005/07/12 02:30:14  etnlwind
// Update for Base Class Destructor Changed to Virtual
//
// Revision 1.4  2005/05/04 14:57:36  etnlwind
// Update for MessageBox Button Location
//
// Revision 1.3  2005/05/04 12:52:51  etnlwind
// Update for MessageBox Related
//
// Revision 1.2  2005/05/04 11:47:44  etnlwind
// Update for ES4
//
// Revision 1.1  2005/04/27 17:01:58  etnlwind
// New Type MsgBox Library Update
//
//*****************************************************************************


 #ifndef __LIB_MSG_BOX_H__
 #define __LIB_MSG_BOX_H__

#include "LibDefines.h"
#include <stdio.h>
#include <stdlib.h>
#include <rmbasic.h>
#include <linuxfs.h>
#include <jpeg/jpeg.h>

#include <tavi_lib.h>
#include <tavi_structures.h>
#include <bitmap.h>
#include <ImageFont.h>
#include <ImageResource.h>
#include <meplib.h>


#include "../Component/object.h"
#include "../Component/Stack.h"
#include "../Component/event.h"
#include "../Component/fbdev.h"
#include "../Component/image.h"
#include "../Component/timer.h"

#include <Lib/LibImage.h>



///////////////////////////////////////////////////////////////////////
//
// CDialogBox Class
// --------------------------
#define DL_BG_BLANK			0x0001
#define DL_BG_ICON			0x0002
#define DL_BG_ETHERNET		0x0004
#define DL_BG_HOST			0x0008
#define DL_BTN_OK_ONLY	 	0x0010		//		[ OK ]
#define DL_BTN_CANCEL_ONLY	 0x0020		//		[ OK ]
#define DL_BTN_OK_CANCEL 	0x0040		//	[ OK ] [ Cancel ]
#define DL_VERSION_INFO 	0x0080
#define DL_OK				0x0100		//	Cursor Locates On [OK]

#define DL_PROGRESS_MODE 	0x0200
#define DL_ALARM_MBX_MODE 0x0400

#define DL_DEFAULT_W		244
#define DL_DEFAULT_H		123
#define DBBTN_W				66
#define DBBTN_H 				22

#define MSGBTN_BG_SELECTED	177 //219		//64
#define MSGBTN_BG_UNSELECTED 	176 //218		//89

//int DrawMessageBox3( U32 riTitle, U16 TitleBackgroundColor, U32 riTextContents, int width, int height, U32 option, MSG_PROCESSOR pfpMsgProc );

#define CSX_CLOCK 44
#define CSY_CLOCK 92
#define W_CLOCK	68
#define H_CLOCK 60

#define X_DIFF_CLOCK_AND_DIGIT 8
#define Y_DIFF_CLOCK_AND_DIGIT 17

#define CSX_CLOCKDIGIT (CSX_CLOCK+W_CLOCK+X_DIFF_CLOCK_AND_DIGIT)
#define CSY_CLOCKDIGIT (CSY_CLOCK+Y_DIFF_CLOCK_AND_DIGIT)
#define W_CLOCKDIGIT 32
#define H_CLOCKDIGIT 35




class CMsgBox
{
protected:
	U32 BoxOptions;
	int choice;
	U32 TitleRes;
	U16 TitleBG;
	char* pText;
	uchar_t* TextRes;
	uchar_t* TextResBuffer;
	int sx, sy;			// dbx
	int csx, csy;			// contents?
	int psx, psy;			// progress bar
	int tcw;				// Total Character Width
	int sdc;				// String Divider Count
	int RelativeHeight;
	int width;
	int height;
	int isVisible;

	MSG_PROCESSOR fpMsgBoxMsgProc;

public:
	CMsgBox(){}
	CMsgBox( U32 riTitle, U16 TitleBackgroundColor, U32 riTextContents, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option );
	CMsgBox( U32 riTitle, U16 TitleBackgroundColor, uchar_t* wTextContents, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option );
	CMsgBox(  U32 riTitle,  U16 TitleBackgroundColor,  char* cTextContents, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option );

	virtual ~CMsgBox();

	void BuildDbxBackground();
	void BuildTitle();
	void BuildContents();
	void BuildButtonEdge( int isSelectedBtn, int BtnSx, int BtnSy, int BtnW, int BtnH );
	void BuildButtons(int newchoice);
	void BuildProgressBar( int percentage );
	void BuildProgressContents( int mode, uchar_t* uFilename, uchar_t* uSrcPath, uchar_t* uDesPath );

	int CursorLeft();
	int CursorRight();
	void Show();
	void ReDraw( eOSD_REFRESH_OPTION refresh=OSD_REFRESH );
	void Hide() { isVisible = FALSE; }
	int IsVisible() { return isVisible; }

	int GetCurrentChoice() { return choice; }

	void Reset( U32 riTitle, U16 TitleBackgroundColor, U32 riTextContents, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option );
	void Reset( U32 riTitle, U16 TitleBackgroundColor, uchar_t* wTextContents, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option );
	void Reset( U32 riTitle, U16 TitleBackgroundColor, char* wTextContents, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option );

	int MsgBtnUp( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );
	int MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );

};



#endif


