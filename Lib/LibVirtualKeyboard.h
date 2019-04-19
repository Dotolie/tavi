//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Virtual Keyboard Libaray class declaration
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibVirtualKeyboard.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//******************************************************************************
// $Log: LibVirtualKeyboard.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.9  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.8  2005/10/28 11:15:15  etnlwind
// Update for Palette
//
// Revision 1.7  2005/10/19 05:31:51  etnlwind
// DEBUG Removed
//
// Revision 1.6  2005/10/18 09:05:14  etnlwind
// Update for New DialgBox, MessageBox UI
//
// Revision 1.5  2005/09/29 05:01:11  etnlwind
// Update for FileManager
//
// Revision 1.4  2005/08/25 09:53:45  etnlwind
// Update for VideoPlayer Confirm Check Func.
//
// Revision 1.3  2005/08/25 09:26:57  etnlwind
// Update for FileManager / Virtual Keyboard Update
//
// Revision 1.2  2005/08/24 13:30:50  etnlwind
// Update for VirtualKeyboard MsgProc
//
// Revision 1.1  2005/08/24 13:08:42  etnlwind
// LibVirtualKeyboard Added
//
//
//*******************************************************************************




 #ifndef __LIB_VIRTUAL_KEYBOARD_H__
 #define __LIB_VIRTUAL_KEYBOARD_H__

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
// CVirtualKeyboard Class
// --------------------------
//

#define DT_RENAME_KBD 		0x00000001
#define DT_NEWFOLDER_KBD 	0x00000002

#if 0
#define KBD_BG_NORMAL 83
#define KBD_BG_GRAY 20
#endif
#define KBD_BG_SELECTED	220		//64
#define KBD_BG_NORMAL 		221		//89
#define KBD_BG_GRAY		222		//91
#define TEXTBOX_OUTLINE	92
#define TEXTBOX_OUTLINE_SHADOW	90

#define KEY_W 24
#define KEY_H 19
#define KEY_D 2

#define KEY_LW (((KEY_W+KEY_D)*4)-KEY_D)
#define KEY_MW (((KEY_W+KEY_D)*3)-KEY_D)


#define TA_CURSOR_HEIGHT 13
#define TA_CURSOR_DIFF_Y 3

#define TEXT_DIFF_X 3
#define TEXT_DIFF_Y 6

typedef struct _tag_VIRTUAL_KEYBOARD
{
	int MaxCol;
	int MaxRow;
	int fUnicode;
	char* KeyArray;

}__ATTRIBUTE_PACKED__ VIRTUAL_KEYBOARD;

class CVirtualKeyboard : public CMsgBox
{
	int kCol, kRow;
	VIRTUAL_KEYBOARD* Vk; 
	VIRTUAL_KEYBOARD* VkEngBe; 
	VIRTUAL_KEYBOARD* VkEngLe;

	//uchar_t* DisplayText;
	uchar_t* DisplayText;
	char KeyPrintfBuf[10];

	int taW, taH, taX, taY, tX, tY;	// Text Area Position
	int etx;	// End of Text X pos.

	int ConfirmPass;
	int CurrentUsedBuffer;
	
public:
//	CVirtualKeyboard( U32 riTitle, MSG_PROCESSOR pfpMsgProc, U32 option );
	CVirtualKeyboard::CVirtualKeyboard( U32 riTitle, uchar_t* SrcFileName, U32 option );
	virtual ~CVirtualKeyboard();

	void BuildSpecialKey( char* KeyPrint, int x, int y, int isSelected, int KeyWidth );
	void BuildImageKey( U16 PictureRes, int x, int y, int isSelected );
	void BuildNormalKey( uchar_t KeyPrint, int x, int y, int isSelected );

	void BuildDbxBackground();
	void BuildTextArea();
	void ClearTextArea();
	void BuildCursor( int ix );
	void BuildKeys();
	void BuildButtons();
	void BuildText();

	int CursorUp();
	int CursorDown();
	int CursorLeft();
	int CursorRight();
	int CursorOK();

	void SetKBD( VIRTUAL_KEYBOARD* input );
	char GetKeyValue();
	uchar_t GetKeyValue_UC16();

	uchar_t* GetString() { return DisplayText; }

	void SetUsedStringBuffer( int FullpathNameLength);

	void Show();
	void ReDraw();
	void Hide() { isVisible = FALSE; }
	int IsVisible() { return isVisible; }
	int IsConfirmed() { return ConfirmPass; }

	int MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );

};



#endif

