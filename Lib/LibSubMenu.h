//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibSubMenu Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSubMenu.h,v $
// $Revision: 1.3 $
// $Date: 2006/06/22 01:50:37 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSubMenu.h,v $
// Revision 1.3  2006/06/22 01:50:37  zstein
// - added InsertSubMenu(...).
//
// Revision 1.2  2006/06/07 08:10:51  zstein
// - added int DeleteMenuItem( RMuint32 menuid ) member method.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.12  2005/12/22 07:42:25  etnlwind
// Update for Round Submenu
//
// Revision 1.11  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.10  2005/11/24 10:56:46  etnlwind
// Update for New EQ
//
// Revision 1.9  2005/07/16 06:15:26  etnlwind
// Update for LibSubtitleSyncDbx
//
// Revision 1.8  2005/07/12 02:24:33  etnlwind
// Update for LibSubMenu Destructor Changed to Virtual
//
// Revision 1.7  2005/07/12 02:20:41  etnlwind
// Update for LibSubMenu Destructor Changed to Virtual
//
// Revision 1.6  2005/07/11 03:22:09  etnlwind
// Update for SubMenu Library
//
// Revision 1.5  2005/07/07 05:16:47  etnlwind
// Update for SubMenu method add
//
// Revision 1.4  2005/07/07 03:49:55  etnlwind
// Update for SubMenu Default & User Define Screen Size
//
// Revision 1.3  2005/07/05 11:23:57  etnlwind
// Update for Subtitle Font & Video SubMenu
//
// Revision 1.2  2005/07/05 09:42:10  etnlwind
// Update for SubMenu
//
// Revision 1.1  2005/07/04 21:04:13  etnlwind
// Update for LibSubMenu Class Add
//
//
//*****************************************************************************


 #ifndef __LIB_SUB_MENU_H__
 #define __LIB_SUB_MENU_H__

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
// CSubMenu Class
// --------------------------
#define MENU0_SX 8
#define MENU0_SY 40

#define SELECT_BAR_SX_DIFF 4
#define SELECT_BAR_SY_DIFF 4
#define SELECT_BAR_HEIGHT 16
#define SELECT_BAR_Y_SPC 2
#define CHILDMENU_INFORM_ICON_REVERSE_SX_DIFF 9
#define DIFF_ENDOFTEXT_TO_CHILDMENU_INFORM_ICON 10
#define DIFF_BETWEEN_MOTHER_AND_CHILD_SM 2
#define PARENTMENU_TEXT_SX_DIFF 12
#define CHILDMENU_TEXT_SX_DIFF 18
#define CHILDMENU_SELECTOR_SX_DIFF 8

#define SM_ARROW_W 4
#define SM_ARROW_H 7
#define SM_ARROW_TH 14
#define SM_CURSOR_W 8
#define SM_CURSOR_H 6
#define SM_CURSOR_TH 12
#define SM_EDGE_W 4
#define SM_EDGE_H 3
#define SM_EDGE_TH 24

#define SUBMENU_MOTHER 0
#define SUBMENU_CHILD 1

#define SUBMENU_MOTHER_BG 27
#define SUBMENU_CHILD_BG 26

#ifdef _WIN32
#define __ATTRIBUTE_PACKED__
#else
#define __ATTRIBUTE_PACKED__	__attribute__((packed))
#endif

#define SELECTED_CORNER_PIX_COLOR_INDEX 90
#define UNSELECTED_CORNER_PIX_COLOR_INDEX 91

//#define SELECTED_BASE_COLOR_INDEX 88
//#define UNSELECTED_BASE_COLOR_INDEX 90
#define SELECTED_BASE_COLOR_INDEX 178
#define UNSELECTED_BASE_COLOR_INDEX 179

#define CURRENT_SELECTED_BAR_COLOR_INDEX 1
#define PREV_SELECTED_BAR_COLOR_INDEX 107

#define ARROW_SY ((SELECT_BAR_HEIGHT/2) - (SM_ARROW_H/2))


///////////////////////////////////////////////////////////////////////////////////////////////////
// CPopUpMenu Classes

class CPopUpSubMenuItem		// Data Class
{
public:
	int Idx;
	U32 DisplayNameRes;
	MENU_PROCESSOR MenuItemProc;
	RMuint8* Parameters;
	char isMarkable;
	int GetIdx() { return Idx; }
};

class CPopUpSubMenu			// Data And Method
{
public:
	CPopUpSubMenu();
	virtual ~CPopUpSubMenu();

	int Idx;	

	U32 DisplayNameRes;
	CList<CPopUpSubMenuItem> SubMenuItemList;
	MENU_PROCESSOR MenuItemProc;
	RMuint8* Parameters;

	int PointedMenuItemIdx;
	int SelectedMenuItemIdx;
	int LargestSubMenuItem_StringLength;
	int AddSubMenuItem( RMuint32 DisplayNameRes, MENU_PROCESSOR MenuItemProc, RMuint8* Parameters );
	int AddSubMenuItem( RMuint32 DisplayNameRes, char fMarkable, MENU_PROCESSOR MenuItemProc, RMuint8* Parameters );
  int DeleteMenuItem( RMuint32 menuid );
	int ChangeSubMenuItem( RMuint32 DisplayNameRes, char fMarkable, MENU_PROCESSOR MenuItemProc, RMuint8* Parameters );
	int GetIdx() { return Idx; }	
};

class CPopUpMenu
{
	int Idx;
	
	int w0;
	int h0;
	int Cursor0;
	bool isVisible0;
	
	int w1;
	int h1;
	int Cursor1;
	bool isVisible1;

	bool isSubMenuSelected;
	bool isChildMenu;

	int LargestSubMenu_StringLength;
	int LargestSubMenuItem_StringLength;

	int sx0;
	int sy0;
	int sx1;
	int sy1;
	
	U16 Color0;
	U16 Color1;
	U16 BarColor0;
	U16 BarColor1;
	U16 BackgroundColor0;
	U16 BackgroundColor1;	

	int sbsx;
	int sbsy;
	int sbw;
	int sbh;

	RMuint8* pO;
	CList<CPopUpSubMenu> SubMenuList;
	MENU_PROCESSOR ClosingProc;
	//CPopUpSubMenu* CurrentSelected_SubMenu;
	//CPopUpSubMenuItem* CurrentSelected_SubMenuItem;
	
	void Init( int x, int y, int w, int h );
	U16 GetBitmapColor( U16 input, U16 FG_Color, U16 BG_Color );
	
public:
	
	CPopUpMenu();
	CPopUpMenu( int x, int y, int w, int h );
	virtual ~CPopUpMenu();

	int IsVisible() { return isVisible0; }
	
	void BuildBackground();
	void BuildSelectBar();
	void BuildForegroundContents();

	int AddSubMenu( RMuint32 DisplayNameRes, CPopUpSubMenu* SubMenu, int DefaultValue, MENU_PROCESSOR MenuProc, RMuint8* Parameters );
	int ChangeSubMenu( RMuint32 DisplayNameRes, CPopUpSubMenu* SubMenu, int DefaultValue, MENU_PROCESSOR MenuProc, RMuint8* Parameters );
  int InsertSubMenu( RMuint32 previd, RMuint32 id, CPopUpSubMenu* submenu, 
                     int DefaultValue, MENU_PROCESSOR MenuProc, RMuint8* Parameters );
	int DeleteSubMenu( RMuint32 DisplayNameRes );

	int CursorUp();
	int CursorDown();
	int CursorLeft();
	int CursorRight();
	int Select();
	
	//CPopUpSubMenu* GetCurrentMenu();

	void HighlightOn( int Menu );	// Selected

	void Show();
	void Redraw();
	void Hide( int Menu, int fRecoverOSD=TRUE );

	void PushOsd();
	void PopOsd();

	int GetIdx() { return Idx; }

	void ShowStatus();

	int MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );	
};








#endif


