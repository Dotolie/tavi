//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Dictionary Keyboard class declaration
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/DicKeyboard.h,v $
// $Revision: 1.5 $
// $Date: 2006/03/03 08:39:39 $
// Author:
//
//******************************************************************************
// $Log: DicKeyboard.h,v $
// Revision 1.5  2006/03/03 08:39:39  akaiaphanu
// working timer
//
// Revision 1.4  2006/03/03 05:15:12  akaiaphanu
// working text scroll
//
// Revision 1.3  2006/02/28 07:08:20  akaiaphanu
// working touchpad
//
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.5  2006/02/20 01:33:56  akaiaphanu
// add ListManager , BitIO
//
// Revision 1.4  2006/02/14 08:37:17  akaiaphanu
// working bottom guide , scrollbar , box
//
// Revision 1.3  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.2  2006/02/06 04:17:15  akaiaphanu
// add to history function
//
// Revision 1.1  2006/01/27 04:40:21  akaiaphanu
// added dictionary
//
//
//*******************************************************************************

 #ifndef __DIC_KEYBOARD_H__
 #define __DIC_KEYBOARD_H__

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

#include "../../Component/object.h"
#include "../../Component/Stack.h"
#include "../../Component/event.h"
#include "../../Component/fbdev.h"
#include "../../Component/image.h"
#include "../../Component/timer.h"

#include <Lib/LibImage.h>
#include "hangul.h"
#include "vkbd.h"
#include "Extra.h"

//#define KBD_BG_SELECTED	220		//64
//#define KBD_BG_NORMAL 		221		//89
//#define KBD_BG_GRAY		222		//91
//#define TEXTBOX_OUTLINE	92
//#define TEXTBOX_OUTLINE_SHADOW	90

//#define ICON_DBXICON_WIDTH 	24
//#define BTN_ICON_MAX_X 		295

#define BOX_TITLE_COLOR			31
#define BOX_BASE_COLOR			24
#define DIC_KEY_W 				51	/* old 56*/
#define DIC_KEY_H 				28	/* old 31*/
#define DIC_KEY_D 				2
#define BOX_DIC_W				300
#define BOX_DIC_H				210
#define BOX_DIC_START_X		10
#define BOX_DIC_START_Y		25
#define BOX_KEY_START_X		(BOX_DIC_START_X + 14 + 1)
#define BOX_KEY_START_Y		(BOX_DIC_START_Y + 45 + 1 + 14)
#define BOX_OUTWIN_START_X	BOX_KEY_START_X
#define BOX_OUTWIN_START_Y	(BOX_DIC_START_Y + 1 + 14 + 5)
#define BOX_OUTWIN_W			(((DIC_KEY_W+DIC_KEY_D)*5)+4)
#define BOX_OUTWIN_H			30
#define OUTWIN_TEXT_START_X	(BOX_OUTWIN_START_X+10)
#define OUTWIN_TEXT_START_Y	(BOX_OUTWIN_START_Y+10)
#define CURSOR_X				0
#define CURSOR_Y				0
#define CURSOR_W				2
#define CURSOR_H				13
#define TA_CURSOR_DIFF_Y 		3
#define KEY_LW 					(((KEY_W+KEY_D)*4)-KEY_D)
#define KEY_MW 					(((KEY_W+KEY_D)*3)-KEY_D)

#define TEXT_DIFF_X 				3
#define TEXT_DIFF_Y 				6

#define KEY_BUFF_SIZE			256
/* Dictionary mode define */
#define DIC_MODE_KOR			0
#define DIC_MODE_ENGU			1	/* English upper */
#define DIC_MODE_ENGL			11	/* English lower */
#define DIC_MODE_DIGIT			2

#define CLEAR_ALL				0

typedef struct _tag_DIC_KEYBOARD
{
	int MaxCol;
	int MaxRow;
	int fUnicode;
	char* Dic_Eng_KeyArray;
	
}__ATTRIBUTE_PACKED__ DIC_KEYBOARD;

typedef struct _tag_DIC_SHIFTMANAGER
{
	int m_pre_hide;
	int m_suf_hide;
	int m_pre_width;
	int m_suf_width;
	int m_total_index;
	int m_pre_index;
	int m_suf_index;
	char m_pre_buffer[KEY_BUFF_SIZE];
	char m_suf_buffer[KEY_BUFF_SIZE];
	
}__ATTRIBUTE_PACKED__ DIC_SHIFTMANAGER;

enum DIC_LANGUAGE
{ 
	KOREAN, ENGLISH, CHINA, JAPAN
};

class DicKeyboard : public CMsgBox
{
	int 						kCol, kRow;		/* Keyboard */
	int 						m_mode;		/* Dic mode */
	char 					m_cursor_pos;	/* Cursor position */
	int						m_shift_index;	/* shifted cursor position */

	int 						m_Xpos;
	int						m_Ypos;
	int						m_Width;
	int						m_Height;
	int						m_Title_Xpos;
	int						m_Title_Ypos;
	int						m_Title_Height;

	bool						m_fExit;
	bool						m_isSHIFT;
	bool						m_isVisible;
	bool						m_isBackspace;
	bool						m_isSpace;
	bool						m_isToggle;
	bool						m_isShiftToggle;
	bool						m_isTimeRemain;
	bool						m_isChangeKeyboard;
	bool						m_CheckTimeStart ;
	bool						m_CheckShiftStart ;
	bool						m_Time_Proc;
	bool						m_DirecKey_Proc;
	bool						m_PassByTimer;
	bool						m_PassByKeyShift;
	bool						m_PassByKeyPress; /* Finaly input key state */
	bool						m_PassByKorKeyPress;
	bool						m_BackToKeyboard;
	
	char						m_Key_Buffer[3]; /* Input key buffer */
	char						m_Key_Buffer_Pre[ KEY_BUFF_SIZE ];	/* Input key buffer Pre */
	char						m_Key_Buffer_Suf[3];	/* Input key buffer Suf */

	uchar_t* 					m_DisplayText_Pre;	/* Text  |*/
	uchar_t* 					m_DisplayText_Suf;	/* | Text */
	uchar_t* 					m_DisplayText_Kor;

	int						m_Key_Buffer_Index;
	int						m_Key_Buffer_Pre_Index;
	int 						m_Key_Pressed_Info[20];		/* Pressed Key Information */
	char						m_Key_Old;						/* Old key position */
	char						m_Key_New;					/* New key position */
	
	DIC_KEYBOARD* 			Vk;
	DIC_LANGUAGE			LANGUAGE;
	DIC_SHIFTMANAGER		m_shift_manager;	/* manager for shift state */
	char  					KeyPrintfBuf[20];

	VirtualKeyboard 			vk_kor;
	HAN_INPUT_KEY_BUFFER  	m_kor_key_buf;
	HCODE 					m_kor_code;
	char 					m_kor_text[256];
	int 						m_kor_len;
	char 					m_kor_ks_text[256];
	
	int 						taW, taH, taX, taY, tX, tY;	/* Text Area Position */
	int 						etx;						/* End of Text X pos */
	
public:

	DicKeyboard::DicKeyboard(int, int, int, int);
	virtual ~DicKeyboard();

	void 	BuildSpecialKey( char* KeyPrint, int x, int y, int isSelected, int KeyWidth );
	void 	BuildImageKey( U16 PictureRes, int x, int y, int isSelected );
	void 	BuildNormalKey( uchar_t KeyPrint, int x, int y, int isSelected );
	void 	Draw_ButtonBox(char* KeyPrint, int x, int y, int isSelected, int width);
	void 	Draw_KeyboardBox(void);
	void 	Draw_Guide(void);
	int 		Draw_Scroll_Start_Pos(int type);
	int 		DrawText_Cursor(int mode);	
	void 	BuildTextArea();
	void 	BuildCursor( int ix );
	void 	BuildKeysKor(void);
	void 	BuildKeysEngUp(void);
	void 	BuildKeysEngLo(void);
	void 	BuildKeysDigit(void);
	
	int 		Cursor_LShift();
	int 		Cursor_RShift();
	int 		CursorLongRight();
	int 		CursorUp();
	int 		CursorDown();
	int 		CursorLeft();
	int 		CursorRight();
	int 		CursorOK(int msg);
	int 		GoSearch(int mode);
	void		Exit();

	void 	ClearTextArea();
	void 	Key_Index_Clear(int mode) ;
	void 	Key_Buffer_Clear(int mode);
	
	int 		Get_Member(int whichmember);
	int 		Set_Member(int whichmember, int value);
	void 	SetKBD( int dic_mode);
	int 		Check_Time_Remain(void);
	void 	Kor_Keys_Proc(void);
	void 	Show();
	void 	ReDraw();

	

};
#endif

