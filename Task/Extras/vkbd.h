//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Korean Language Virtual Keyboard
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/vkbd.h,v $
// $Revision: 1.2 $
// $Date: 2006/02/27 02:40:17 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: vkbd.h,v $
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.2  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.1  2006/01/27 05:16:26  akaiaphanu
// added dictionary
//
// 
// 
//
//*****************************************************************************
#ifndef __VIRTUAL_KBD_H__
#define __VIRTUAL_KBD_H__

#define _Y_DIV0_	5
#define _Y_DIV1_	27
#define _Y_DIV2_	49
#define _Y_DIV3_	70
// #define _Y_DIV4_	86

#define	_ROW_TAB_A_	30  // (width space before X - 2)
#define	_ROW_TAB_B_	18 // (width space before X - 2)
#define	_ROW_TAB_C_	6  // (width space before X - 2)

#define	_BUTTON_WIDTH_	24
#define	_VKBD_Y_SP_	166	// MAX_Y_SIZE - KBD_Y_SIZE

#define	_KBD_TAG_BTN_WX_	2 
#define	_KBD_TAG_BTN_WY_	222
#define	_KBD_TAG_BTN_EX_	31
#define	_KBD_TAG_BTN_EY_	240

/**
#define _BACK_BTN_RX_ 192
#define _VOICE_BTN_RX_ 225
#define _HISTY_BTN_RX_ 261
**/
#define __LANG_BTN_X__    4
#define __UNDO_BTN_X__    184
#define __VOICE_BTN_X__   216
#define __HISTORY_BTN_X__ 248
#define __HELP_BTN_X__    280

#define	_KBD_BODY_HEIGHT_	76

#define _KBD_CHAG_X_POS_ 8
#define _KBD_CHAG_Y_POS_ 26
#define _KBD_CHAG_W_H_ 28

/*
#include "../../mpegdec/em85xx.h"
#include "../glib/glib.h"
#include "../task/task.h"
#include "../task/msg.h"
*/
#include "hangul.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <fcntl.h>

typedef unsigned char RMuint8;
typedef unsigned long RMuint32;

enum 	language { ENG = 0, KOR, CHN, JPN };

static const char kbd[][12] = {
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '$', '$',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '_', '^', '^',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M', ' ', ' ', ' ', '!', '!'
};

class VirtualKeyboard
{

public:

	language	LANGUAGE;
	bool		isSHIFT;
	bool		isVisible;
	
	int		px, py;
 	int		kbdFlag; // Language Flag
	int		historyFlag;
	int		VHistoryFlag;

	char		buffer[ 256 ];
	
	VirtualKeyboard()
	{
			LANGUAGE 		= ENG;
			isVisible			= false;
			isSHIFT			= false;

#ifdef __EXPORT__
			kbdFlag     = 1;
#else
			kbdFlag     = 0;
#endif

			px			= 0;
			py			= 0; 

			buffer_clear();
	}
	
	~VirtualKeyboard()
	{
		
	}

//	char get_key(BITMAPFILEINFO256 *, int, int, int);
//	char keyInput(BITMAPFILEINFO256 *, int, int, int);
//	int rollOver(BITMAPFILEINFO256 *, int, int, int);
//	int get_area(int, int);
//	void shiftSS(BITMAPFILEINFO256 *); 
//	void VirtualKeyboard::retHangulKbd(BITMAPFILEINFO256 *, int, int, int, int, int, RMuint8 *);
	void buffer_clear()
	{
		memset( buffer, '\0', 256 );
		printf("Testing...bufferclear()\n");
	}

	/* functions for korean language */

	void HAN_INPUT_KEY_BUFFER_Clear(HAN_INPUT_KEY_BUFFER *);
	bool HAN_INPUT_KEY_BUFFER_Create(HAN_INPUT_KEY_BUFFER *, int);
	void HAN_INPUT_KEY_BUFFER_Delete(HAN_INPUT_KEY_BUFFER *);
	void HAN_INPUT_KEY_BUFFER_COMBI_BufPrint(HAN_INPUT_KEY_BUFFER *, char *);
	HCODE COMBI_Key_to_HCODE(char);
	void COMBI_Han_DelPreChar(HAN_INPUT_KEY_BUFFER *);
	bool COMBI_Eng_putch(HAN_INPUT_KEY_BUFFER *, HCODE);
	HCODE COMBI_MixJaumCho(HCODE, HCODE);
	HCODE COMBI_MixJaumJong(HCODE, HCODE);
	HCODE COMBI_Han_Cho_to_Jong(HCODE);
	HCODE COMBI_Han_cho_cho2_jung_jong_jong2_to_Hstr(HCODE, HCODE, HCODE, HCODE, HCODE);
	bool COMBI_Han_putch(HAN_INPUT_KEY_BUFFER *, HCODE, HCODE, HCODE, HCODE, HCODE);
	HCODE COMBI_MixDoubleVowel(HCODE, HCODE);
	void COMBI_Han_Input(HAN_INPUT_KEY_BUFFER *, HCODE);
	int COMBI_Han_Get_Type(HCODE);
	int VirtualKeyboard::COMBI_to_KS(char *, const char *);
	int Binary_Search(const UINT16 *, UINT16, INT32);
	UINT16 HCODE_JASO_cmb2ks(UINT16);
	void COMBI_Str_to_KS_Str(char *, const char *);

};

#endif
