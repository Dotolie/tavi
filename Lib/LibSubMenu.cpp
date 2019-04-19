//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibSubMenu Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSubMenu.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/06/22 01:50:37 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSubMenu.cpp,v $
// Revision 1.3  2006/06/22 01:50:37  zstein
// - added InsertSubMenu(...).
//
// Revision 1.2  2006/06/07 08:10:51  zstein
// - added int DeleteMenuItem( RMuint32 menuid ) member method.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.26  2005/12/22 07:42:25  etnlwind
// Update for Round Submenu
//
// Revision 1.25  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.24  2005/12/12 01:49:38  etnlwind
// Update for MenuClose Method
//
// Revision 1.23  2005/12/11 20:26:43  yong
// LEXIT, MODE, LMODE cancels menu only if not g_KillAll (e.g. Alarm)
//
// Revision 1.22  2005/11/17 05:52:21  etnlwind
// Update for Alarm
//
// Revision 1.21  2005/11/17 03:03:55  etnlwind
// Update for SubMenu ClosingProc Related
//
// Revision 1.20  2005/10/28 07:10:28  etnlwind
// Update for Pop-Up Submenu
// 1. Add Executeable Submenu
//
// Revision 1.19  2005/08/30 12:23:23  etnlwind
// Update for FileManager SubMenu Related
//
// Revision 1.18  2005/08/30 04:31:01  etnlwind
// Upate for VideoPlayer / FileManager
//
// Revision 1.17  2005/08/12 11:17:24  etnlwind
// Update for Sound Palette
//
// Revision 1.16  2005/08/09 10:52:09  etnlwind
// Update for
// 1. CSoundPaletteDbx Added
// 2. Video Control Pannel UI changed
//
// Revision 1.15  2005/07/16 06:15:26  etnlwind
// Update for LibSubtitleSyncDbx
//
// Revision 1.14  2005/07/16 02:07:56  etnlwind
// Update for LibFileDbx
//
// Revision 1.13  2005/07/13 06:02:31  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.12  2005/07/12 12:45:35  etnlwind
// Update for LibFileDbx
//
// Revision 1.11  2005/07/12 01:23:54  etnlwind
// Update for  LibSubMenu Bug Fix
//
// Revision 1.10  2005/07/11 03:22:09  etnlwind
// Update for SubMenu Library
//
// Revision 1.9  2005/07/07 13:58:26  etnlwind
// Update for Subtitle Releate Bug Fix
//
// Revision 1.8  2005/07/07 05:16:47  etnlwind
// Update for SubMenu method add
//
// Revision 1.7  2005/07/07 03:49:55  etnlwind
// Update for SubMenu Default & User Define Screen Size
//
// Revision 1.6  2005/07/07 01:15:24  etnlwind
// Update for
// 1. Xen Sound Off on PCM Audio included Video.
// 2. Subtitle Outline Color Changed to No Transparency Black
// 3. Text Resources
//
// Revision 1.5  2005/07/05 11:23:57  etnlwind
// Update for Subtitle Font & Video SubMenu
//
// Revision 1.4  2005/07/05 09:42:10  etnlwind
// Update for SubMenu
//
// Revision 1.3  2005/07/05 08:38:52  etnlwind
// Update for SubMenu UI Changes
//
// Revision 1.2  2005/07/05 05:58:49  etnlwind
// Update for macro name fix (ML_NAME_DECENT -> ML_NAME_DESCENT)
//
// Revision 1.1  2005/07/04 21:04:13  etnlwind
// Update for LibSubMenu Class Add
//
//
//*****************************************************************************


#include "LibSubMenu.h"
#include <Lib/LibDefines.h>
#include <Lib/LibImage.h>


extern CTextEngine_BDF* pT;



///////////////////////////////////////////////////////////////////////////////////////////////////
// Class CPopUpSubMenu

CPopUpSubMenu::CPopUpSubMenu()
{
	DisplayNameRes = 0;
	MenuItemProc = NULL;
	PointedMenuItemIdx = 0;
	LargestSubMenuItem_StringLength = 100;
}

CPopUpSubMenu::~CPopUpSubMenu()
{
	SubMenuItemList.FreeAll();
	DisplayNameRes = 0;
	MenuItemProc = NULL;
	SelectedMenuItemIdx = 0;	
}

int CPopUpSubMenu::AddSubMenuItem( RMuint32 DisplayNameRes, MENU_PROCESSOR MenuItemProc, RMuint8* Parameters )
{
	return( AddSubMenuItem( DisplayNameRes, FALSE, MenuItemProc, Parameters ) );
}


int CPopUpSubMenu::AddSubMenuItem( RMuint32 DisplayNameRes, char fMarkable, MENU_PROCESSOR MenuItemProc, RMuint8* Parameters )
{
	int temp=0;

	CPopUpSubMenuItem* pNewNode = new CPopUpSubMenuItem;

	pNewNode->DisplayNameRes = DisplayNameRes;
	temp = pT->GetStringWidth( DisplayNameRes, 12, DT_TRANSPARENCY );

	if( LargestSubMenuItem_StringLength < temp )
		{
		LargestSubMenuItem_StringLength = temp;
		}

	if( MenuItemProc )
		{
		pNewNode->MenuItemProc = MenuItemProc;
		pNewNode->Parameters = Parameters;
		}
	else
		{
		pNewNode->MenuItemProc = NULL;
		pNewNode->Parameters = NULL;
		}

	pNewNode->Idx = SubMenuItemList.Count()-1;
	pNewNode->isMarkable = fMarkable;
	SubMenuItemList.Add( pNewNode );

	return( pNewNode->Idx );
	
}

int 
CPopUpSubMenu::DeleteMenuItem( RMuint32 menuid )
{
  int cnt = SubMenuItemList.Count();

  for( int i=0; i<cnt; i++ ) {
    if( SubMenuItemList[i]->DisplayNameRes == menuid ) {
      delete SubMenuItemList.Detach( SubMenuItemList[i] );
    }
  }

  return SubMenuItemList.Count();
}

int CPopUpSubMenu::ChangeSubMenuItem( RMuint32 DisplayNameRes, char fMarkable, MENU_PROCESSOR MenuItemProc, RMuint8* Parameters )
{
	for( int i=0; i<SubMenuItemList.Count(); i++ )
		{
		if( SubMenuItemList[i]->DisplayNameRes == DisplayNameRes )
			{
			SubMenuItemList[i]->MenuItemProc = MenuItemProc;
			SubMenuItemList[i]->Parameters = Parameters;
			SubMenuItemList[i]->isMarkable = fMarkable;
			return i;
			}
		}

	return -1;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// Class CPopUpMenu

CPopUpMenu::CPopUpMenu()
{
	DEBUGMSG( 1,("CPopUpMenu Created\n"));
	Init( 0, 0, 320, 240 );
}

CPopUpMenu::CPopUpMenu( int x, int y, int w, int h )
{
	DEBUGMSG( 1,("CPopUpMenu(%d, %d, %d, %d) Created\n", x, y, w, h ));
	Init( x, y, w, h );
}

void CPopUpMenu::Init( int x, int y, int w, int h )
{
	DEBUGMSG( 1,("CPopUpMenu Initialized\n"));

	isVisible0 = FALSE;
	isVisible1 = FALSE;
	Cursor0 = 0;
	Cursor1 = 0;
	isSubMenuSelected = FALSE;		// It means SubMenu May Selected by User.

	sbsx = x;
	sbsy = y;
	sbw = w;
	sbh = h;

//	BackgroundColor = SUBMENU_MOTHER_BG;

	LargestSubMenu_StringLength = 100;
	LargestSubMenuItem_StringLength = 100;
	ClosingProc = NULL;

	ASSERT( SubMenuList[Cursor0] );
	
	//CurrentSelected_SubMenu = SubMenuList[Cursor0];
	//CurrentSelected_SubMenuItem = NULL;

	pO = (RMuint8*)malloc( sbw*sbh*sizeof(RMuint8) );

}

CPopUpMenu::~CPopUpMenu()
{
	free( pO ); pO = NULL;
	
//	for( int i=0; i<SubMenuList.Count(); i++ )
//		delete(SubMenuList[i]);

	SubMenuList.FreeAll();
}

U16 TopLeftRd[4][4] = 
{
0,0,0,0,
0,0,1,1,
0,1,2,2,
0,1,2,2
};

U16 TopRightRd[4][4] = 
{
0,0,0,0,
1,1,0,0,
2,2,1,0,
2,2,1,0,
};

U16 BottomLeftRd[4][4] = 
{
0,1,2,2,
0,1,2,2,
0,0,1,1,
0,0,0,0
};

U16 BottomRightRd[4][4] = 
{
2,2,1,0,
2,2,1,0,
1,1,0,0,
0,0,0,0,
};

U16 CPopUpMenu::GetBitmapColor( U16 input, U16 FG_Color, U16 BG_Color )
{
	U16 output;
	switch( input )
		{
		case 0: output=0; break;
		case 1: output=_BLACK; break;
		case 2: output=BG_Color; break;
		}

	return output;
}

void CPopUpMenu::BuildBackground()
{

	DEBUGMSG(1, ("BuildBackground()\n"));

	if( isSubMenuSelected )
		{
		DEBUGMSG(1, ("-isSubMenuSelected=TRUE\n"));

		Color0 = UNSELECTED_CORNER_PIX_COLOR_INDEX;
		BackgroundColor0 = UNSELECTED_BASE_COLOR_INDEX; 

		Color1 = SELECTED_CORNER_PIX_COLOR_INDEX;
		BackgroundColor1= SELECTED_BASE_COLOR_INDEX;
		
		}
	else
		{
		DEBUGMSG(1, ("-isSubMenuSelected=FALSE\n"));
		
		Color0 = SELECTED_CORNER_PIX_COLOR_INDEX;
		BackgroundColor0= SELECTED_BASE_COLOR_INDEX;

		Color1 = UNSELECTED_CORNER_PIX_COLOR_INDEX;
		BackgroundColor1 = UNSELECTED_BASE_COLOR_INDEX; 

		}


	if( isVisible0 )
		{
		U16 cc;
		DEBUGMSG(1, ("-isVisible0=TRUE\n"));
		
		sx0= MENU0_SX;
		sy0= MENU0_SY;
		w0 = LargestSubMenu_StringLength + DIFF_ENDOFTEXT_TO_CHILDMENU_INFORM_ICON + CHILDMENU_INFORM_ICON_REVERSE_SX_DIFF + SELECT_BAR_SX_DIFF + SM_ARROW_W;
		//h0 =  SubMenuList.Count() * ( SELECT_BAR_HEIGHT + (SELECT_BAR_Y_SPC*2) + 1 ) ;
		h0 =  SubMenuList.Count() * ( SELECT_BAR_HEIGHT + SELECT_BAR_Y_SPC ) + (SELECT_BAR_Y_SPC*3) ;
#if 0
		pMpegDecoder->RenderOsdBox_WithoutRefresh( _BLACK, sx0+1, sy0, w0-2, h0 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( _BLACK, sx0, sy0+1, w0, h0-2 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( Color0, sx0+1, sy0+2, w0-2, h0-3 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor0, sx0+1, sy0+1, w0-3, h0-3 );
#endif
	 	pMpegDecoder->RenderOsdBox_WithoutRefresh( _BLACK, sx0+4, sy0, w0-8, h0 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( _BLACK, sx0, sy0+4, w0, h0-8 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor0, sx0+4, sy0+1, w0-8, h0-2 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor0, sx0+1, sy0+4, w0-2, h0-8 );

		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				DrawPixelWithoutRefresh( GetBitmapColor(TopLeftRd[y][x],Color0,BackgroundColor0), sx0+x, sy0+y);

		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				DrawPixelWithoutRefresh( GetBitmapColor(TopRightRd[y][x],Color0,BackgroundColor0), sx0+w0-4+x, sy0+y );

		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				DrawPixelWithoutRefresh( GetBitmapColor(BottomLeftRd[y][x],Color0,BackgroundColor0), sx0+x, sy0+h0-4+y);

		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				DrawPixelWithoutRefresh( GetBitmapColor(BottomRightRd[y][x],Color0,BackgroundColor0), sx0+w0-4+x, sy0+h0-4+y);

		}

	if( isVisible1 &&  ( SubMenuList[Cursor0]->MenuItemProc != NULL || SubMenuList[Cursor0]->SubMenuItemList.Count() > 0 ) )
		{

		DEBUGMSG(1, ("-isVisible1=TRUE\n"));

/*
		if( LargestSubMenuItem_StringLength < SubMenu->LargestSubMenuItem_StringLength )
			LargestSubMenuItem_StringLength = SubMenu->LargestSubMenuItem_StringLength;
*/
		LargestSubMenuItem_StringLength = SubMenuList[Cursor0]->LargestSubMenuItem_StringLength;

		sx1 = sx0 + ( w0 + DIFF_BETWEEN_MOTHER_AND_CHILD_SM );
		sy1 = sy0;
		w1 = LargestSubMenuItem_StringLength + DIFF_ENDOFTEXT_TO_CHILDMENU_INFORM_ICON + CHILDMENU_INFORM_ICON_REVERSE_SX_DIFF
			+ SELECT_BAR_SX_DIFF + CHILDMENU_SELECTOR_SX_DIFF + 2;
//		h1 =  SubMenuList[Cursor0]->SubMenuItemList.Count() * ( SELECT_BAR_HEIGHT + (SELECT_BAR_Y_SPC*2) + 1 ) ;	
//		h1 =  SubMenuList.Count() * ( SELECT_BAR_HEIGHT + SELECT_BAR_Y_SPC ) + (SELECT_BAR_Y_SPC*3);
		h1 =  SubMenuList[Cursor0]->SubMenuItemList.Count() * ( SELECT_BAR_HEIGHT + SELECT_BAR_Y_SPC ) + (SELECT_BAR_Y_SPC*3);
	
	 	pMpegDecoder->RenderOsdBox_WithoutRefresh( _BLACK, sx1+4, sy1, w1-8, h1 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( _BLACK, sx1, sy1+4, w1, h1-8 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor1, sx1+4, sy1+1, w1-8, h1-2 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor1, sx1+1, sy1+4, w1-2, h1-8 );

		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				DrawPixelWithoutRefresh( GetBitmapColor(TopLeftRd[y][x],Color1,BackgroundColor1), sx1+x, sy1+y);

		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				DrawPixelWithoutRefresh( GetBitmapColor(TopRightRd[y][x],Color1,BackgroundColor1), sx1+w1-4+x, sy1+y );

		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				DrawPixelWithoutRefresh( GetBitmapColor(BottomLeftRd[y][x],Color1,BackgroundColor1), sx1+x, sy1+h1-4+y);

		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
				DrawPixelWithoutRefresh( GetBitmapColor(BottomRightRd[y][x],Color1,BackgroundColor1), sx1+w1-4+x, sy1+h1-4+y);

		}

}


void CPopUpMenu::BuildSelectBar()
{
	U16 tx, ty;

	DEBUGMSG(1, ("BuildSelectBar()\n"));

	if( isSubMenuSelected )
		{
		BarColor0 = PREV_SELECTED_BAR_COLOR_INDEX;
		BarColor1 = CURRENT_SELECTED_BAR_COLOR_INDEX;
		}
	else
		{
		BarColor0 = CURRENT_SELECTED_BAR_COLOR_INDEX;
		BarColor1 = PREV_SELECTED_BAR_COLOR_INDEX;
		}

	if( isVisible0 )
		{
		DEBUGMSG(1, ("-isVisible0=TRUE\n"));
		DEBUGMSG(1, ("-Cursor0=%d\n", Cursor0));
		
		tx = sx0 + SELECT_BAR_SX_DIFF;
		ty = sy0 + SELECT_BAR_SY_DIFF + ( Cursor0 * (SELECT_BAR_HEIGHT + SELECT_BAR_Y_SPC));

		pMpegDecoder->RenderOsdBox_WithoutRefresh( BarColor0, tx, ty, w0-(SELECT_BAR_SX_DIFF*2), SELECT_BAR_HEIGHT );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor0, tx, ty, 1, 1 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor0, tx+w0-(SELECT_BAR_SX_DIFF*2)-1, ty, 1, 1 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor0, tx, ty+SELECT_BAR_HEIGHT-1, 1, 1 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor0, tx+w0-(SELECT_BAR_SX_DIFF*2)-1, ty+SELECT_BAR_HEIGHT-1, 1, 1 );

		}

	if( isVisible1 && ( SubMenuList[Cursor0]->MenuItemProc != NULL || SubMenuList[Cursor0]->SubMenuItemList.Count() > 0 ) )
		{
		
		DEBUGMSG(1, ("-isVisible1=TRUE\n"));
		DEBUGMSG(1, ("-Cursor1=%d -> ", Cursor1 ));
		Cursor1 = SubMenuList[Cursor0]->PointedMenuItemIdx;
		DEBUGMSG(1, ("%d\n", Cursor1 ));
		
		tx = sx1 + SELECT_BAR_SX_DIFF;
		ty = sy1 + SELECT_BAR_SY_DIFF + ( Cursor1 * (SELECT_BAR_HEIGHT + SELECT_BAR_Y_SPC));

		pMpegDecoder->RenderOsdBox_WithoutRefresh( BarColor1, tx, ty, w1-(SELECT_BAR_SX_DIFF*2), SELECT_BAR_HEIGHT );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor1, tx, ty, 1, 1 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor1, tx+w1-(SELECT_BAR_SX_DIFF*2)-1, ty, 1, 1 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor1, tx, ty+SELECT_BAR_HEIGHT-1, 1, 1 );
		pMpegDecoder->RenderOsdBox_WithoutRefresh( BackgroundColor1, tx+w1-(SELECT_BAR_SX_DIFF*2)-1, ty+SELECT_BAR_HEIGHT-1, 1, 1 );
	
		}
	

}

void CPopUpMenu::BuildForegroundContents()
{
	U16 color;
	U16 tx, ty;
	U16 LinkedMenuItemSelected;

	DEBUGMSG(1, ("BuildForegroundContents()\n"));

	if( isVisible0 )
		{

		for( int row=0; row<SubMenuList.Count(); row++ )
			{
			
			if( row == Cursor0 )
				{
				color = _WHITE;
				LinkedMenuItemSelected = 1;
				}
			else
				{
				color = _BLACK;
				LinkedMenuItemSelected = 0;
				}

			ty = sy0+ SELECT_BAR_SY_DIFF + (row * (SELECT_BAR_HEIGHT + SELECT_BAR_Y_SPC)) + 2;
			tx = sx0 + PARENTMENU_TEXT_SX_DIFF;

#if 1
				pT->DrawResText( SubMenuList[row]->DisplayNameRes, tx, ty, color, 12, DT_TRANSPARENCY );

				if( SubMenuList[row]->SubMenuItemList.Count()>0 )
					DrawItemWithoutRefreshSkipTransparency( 	&TAVI_IMAGES[SUBMENU_ARROW_ICON], 
																!LinkedMenuItemSelected, 
																sx0+w0-CHILDMENU_INFORM_ICON_REVERSE_SX_DIFF-2 , 
																ty+ARROW_SY-3, 
																SM_ARROW_W, 
																SM_ARROW_H );
#else
			if( SubMenuList[row]->DisplayNameRes == ML_CLOSE )
				{
				DrawItemWithoutRefreshSkipTransparency( 	&TAVI_IMAGES[SUBMENU_CANCEL_ARROW], 
															LinkedMenuItemSelected, 
															tx+1,
															ty+2,
															12,
															9);
				}
			else
				{
				pT->DrawResText( SubMenuList[row]->DisplayNameRes, tx, ty, color, 12, DT_TRANSPARENCY );

				if( SubMenuList[row]->SubMenuItemList.Count()>0 )
					DrawItemWithoutRefreshSkipTransparency( 	&TAVI_IMAGES[SUBMENU_ARROW_ICON], 
																!LinkedMenuItemSelected, 
																sx0+w0-CHILDMENU_INFORM_ICON_REVERSE_SX_DIFF-2 , 
																ty+ARROW_SY-3, 
																SM_ARROW_W, 
																SM_ARROW_H );
				}
#endif			
			}
		}


	if( isVisible1 && ( SubMenuList[Cursor0]->MenuItemProc != NULL || SubMenuList[Cursor0]->SubMenuItemList.Count() > 0 ) )
		{

		for( int row=0; row<SubMenuList[Cursor0]->SubMenuItemList.Count(); row++ )
			{
			
			if( row == Cursor1 )
				{
				color = _WHITE;
				LinkedMenuItemSelected = 1;
				}
			else
				{
				color = _BLACK;
				LinkedMenuItemSelected = 0;
				}

			ty = sy1+ SELECT_BAR_SY_DIFF + (row * (SELECT_BAR_HEIGHT + SELECT_BAR_Y_SPC)) + 2;
			tx = sx1 + CHILDMENU_TEXT_SX_DIFF;

			pT->DrawResText( SubMenuList[Cursor0]->SubMenuItemList[row]->DisplayNameRes, tx, ty, color, 12, DT_TRANSPARENCY );

//			if( SubMenuList[row]->SubMenuItemList )
			if( row == SubMenuList[Cursor0]->SelectedMenuItemIdx )
				DrawItemWithoutRefreshSkipTransparency( 	&TAVI_IMAGES[SUMMENU_CURSOR], 
															!LinkedMenuItemSelected, 
															sx1+CHILDMENU_SELECTOR_SX_DIFF, 
															ty+3, 
															SM_CURSOR_W, 
															SM_CURSOR_H );
			
			}
		}

	
}

int CPopUpMenu::AddSubMenu( RMuint32 DisplayNameRes, CPopUpSubMenu* SubMenu, int DefaultValue, MENU_PROCESSOR MenuProc, RMuint8* Parameters )
{
	int temp=0;

	DEBUGMSG(1,("CPopUpMenu::AddSubMenu(%d, 0x%x)\n", DisplayNameRes, SubMenu));

	//temp = ustrlen( pT->GetTextRes(DisplayNameRes));

	temp = pT->GetStringWidth( DisplayNameRes, 12, DT_TRANSPARENCY );

	if( LargestSubMenu_StringLength < temp )
		{
		LargestSubMenu_StringLength = temp;
		}

	
	if( SubMenu )
		{
		SubMenu->DisplayNameRes = DisplayNameRes;
		DEBUGMSG(1,(" -LargestSubMenu_StringLength : %d\n",  LargestSubMenu_StringLength ));
		DEBUGMSG(1,(" -LargestSubMenuItem_StringLength : %d\n",  LargestSubMenuItem_StringLength ));		
		DEBUGMSG(1,(" -SubMenu->LargestSubMenuItem_StringLength : %d\n",  SubMenu->LargestSubMenuItem_StringLength ));
	
		SubMenu->SelectedMenuItemIdx = DefaultValue;
		SubMenu->MenuItemProc = MenuProc;
		SubMenu->Parameters = Parameters;
		SubMenuList.Add( SubMenu );
		}
	else
		{
		if( DisplayNameRes == ML_CLOSE )
			{
			if(MenuProc != NULL)
				ClosingProc = MenuProc;
			}
		else
			{
			CPopUpSubMenu* pNewNode = new CPopUpSubMenu;
			pNewNode->DisplayNameRes = DisplayNameRes;
			pNewNode->MenuItemProc = MenuProc;
			pNewNode->SelectedMenuItemIdx = DefaultValue;
			pNewNode->LargestSubMenuItem_StringLength = LargestSubMenuItem_StringLength;
			pNewNode->Parameters = Parameters;
			SubMenuList.Add( pNewNode );
			}
		}

	return( (Idx=SubMenuList.Count()-1) );
}

/*
  return 0 on success, otherwise -1.
*/
int CPopUpMenu::InsertSubMenu( 
  RMuint32 previd, 
  RMuint32 id, 
  CPopUpSubMenu* submenu, 
  int DefaultValue, 
  MENU_PROCESSOR MenuProc, 
  RMuint8* Parameters )
{
  int temp=0;
  CNode<CPopUpSubMenu>* n;

	temp = pT->GetStringWidth( id, 12, DT_TRANSPARENCY );

	if( LargestSubMenu_StringLength < temp ) {
		LargestSubMenu_StringLength = temp;
	}

	
	if( submenu ) {
		submenu->DisplayNameRes = id;
		submenu->SelectedMenuItemIdx = DefaultValue;
		submenu->MenuItemProc = MenuProc;
		submenu->Parameters = Parameters;
	}
	else {
		if( id == ML_CLOSE ) {
			if(MenuProc != NULL)
				ClosingProc = MenuProc;
		}
		else {
			submenu = new CPopUpSubMenu;
			submenu->DisplayNameRes = id;
			submenu->MenuItemProc = MenuProc;
			submenu->SelectedMenuItemIdx = DefaultValue;
			submenu->LargestSubMenuItem_StringLength = LargestSubMenuItem_StringLength;
			submenu->Parameters = Parameters;
		}
	}

  n = NULL;
	for( int i=0; i<SubMenuList.Count(); i++ ) {
		if( SubMenuList[i]->DisplayNameRes == previd ) {
		  n = SubMenuList.GetNode(i);
		  break;
		}
	}
  return( SubMenuList.Insert(n, submenu) ? 0 : -1 );
}


int CPopUpMenu::ChangeSubMenu( RMuint32 DisplayNameRes, CPopUpSubMenu* SubMenu, int DefaultValue, MENU_PROCESSOR MenuProc, RMuint8* Parameters )
{

	DEBUGMSG(1,("ChangeSubMenu(%d, 0x%x, %d : 0x%x)\n", DisplayNameRes, SubMenu, DefaultValue ));
	
	for( int i=0; i<SubMenuList.Count(); i++ )
		{

		DEBUGMSG(1,("-SubMenuList[%d]->DisplayNameRes : %d ", i, DisplayNameRes ));
		
		if( SubMenuList[i]->DisplayNameRes == DisplayNameRes )
			{
			DEBUGMSG(1,("== DisplayNameRes : %d\n", DisplayNameRes ));
			
			SubMenu->Idx = SubMenuList[i]->Idx;
			SubMenu->DisplayNameRes = DisplayNameRes;
			SubMenu->MenuItemProc = MenuProc;
			SubMenu->Parameters = Parameters;
			SubMenu->PointedMenuItemIdx = DefaultValue;
			SubMenu->SelectedMenuItemIdx = DefaultValue;
			SubMenuList.GetNode(i)->m_Data = SubMenu;
			return i;
			}
		else
			DEBUGMSG(1,("!= DisplayNameRes : %d\n", DisplayNameRes ));
		
		}

	return -1;

}	


int CPopUpMenu::DeleteSubMenu( RMuint32 DisplayNameRes )
{
	for( int i=0; i<SubMenuList.Count(); i++ )
		{
		if( SubMenuList[i]->DisplayNameRes == DisplayNameRes )
			{
			SubMenuList[i]->SubMenuItemList.FreeAll();
			delete SubMenuList.Detach(SubMenuList[i]);
			return i;
			}
		}

	return -1;
}


int CPopUpMenu::CursorUp()
{
	DEBUGMSG(1,("CPopUpMenu::CursorUp()\n"));
	
	if( !isSubMenuSelected )
		{
		if( Cursor0 <= 0 )	Cursor0 = SubMenuList.Count()-1;
		else Cursor0--;
		//CurrentSelected_SubMenu = SubMenuList[Cursor0];
		}
	else
		{
		if( Cursor1 <= 0 )	Cursor1 = SubMenuList[Cursor0]->SubMenuItemList.Count() - 1;
		else Cursor1--;
		//CurrentSelected_SubMenuItem = SubMenuList[Cursor0]->SubMenuItemList[Cursor1];
		SubMenuList[Cursor0]->PointedMenuItemIdx = Cursor1;
		}

	Redraw();
	ShowStatus();
	return TRUE;
}

int CPopUpMenu::CursorDown()
{
	DEBUGMSG(1,("CPopUpMenu::CursorDown()\n"));

	if( !isSubMenuSelected )
		{
		DEBUGMSG(1,("-Cursor0 : %d -> ", Cursor0));

		if( Cursor0 >= SubMenuList.Count()-1 )
			Cursor0 = 0;
		else 
			Cursor0++;

		DEBUGMSG(1,("%d\n", Cursor0));

		DEBUGMSG(1,("-SubMenuList[Cursor0]->MenuItemProc : 0x%x -> ", SubMenuList[Cursor0]->MenuItemProc ));
		//CurrentSelected_SubMenu = SubMenuList[Cursor0];
		DEBUGMSG(1,("0x%x  MenuItemProc:0x%x\n", SubMenuList[Cursor0]->MenuItemProc, SubMenuList[Cursor0]->MenuItemProc ));
		}
	else
		{

		DEBUGMSG(1,("-Cursor1 : %d -> ", Cursor1));

		if( Cursor1 >= SubMenuList[Cursor0]->SubMenuItemList.Count()-1 ) 
			Cursor1 = 0;
		else 
			Cursor1++;

		DEBUGMSG(1,("%d\n", Cursor1));
		
		DEBUGMSG(1,("-CurrentSelected_SubMenuItem->MenuItemProc : 0x%x -> ", SubMenuList[Cursor0]->SubMenuItemList[Cursor1]->MenuItemProc) );
		//CurrentSelected_SubMenuItem = SubMenuList[Cursor0]->SubMenuItemList[Cursor1];
		SubMenuList[Cursor0]->PointedMenuItemIdx = Cursor1;
		DEBUGMSG(1,("0x%x\n", SubMenuList[Cursor0]->SubMenuItemList[Cursor1]->MenuItemProc ));
		}

	Redraw();
	ShowStatus();
	return TRUE;

	
}

int CPopUpMenu::CursorLeft()
{
	DEBUGMSG(1,("CPopUpMenu::CursorLeft()\n"));

	if( !isSubMenuSelected )
		{
		DEBUGMSG(1,("-isSubMenuSelected=FALSE ... Hide(SUBMENU_MOTHER)\n"));
		Hide(SUBMENU_MOTHER);	// Hide All

 		if( ClosingProc != NULL )	
			{
			DEBUGMSG(1,("-CurrentSelected_SubMenu->MenuItemProc=TRUE ... CurrentSelected_SubMenu->MenuItemProc()\n" ));
			ClosingProc( SubMenuList[Cursor0]->Parameters );
			}
 
		}
	else
		{
		DEBUGMSG(1,("-isSubMenuSelected=TRUE ... HighlightOn(SUBMENU_MOTHER)\n"));
		HighlightOn(SUBMENU_MOTHER);
		isVisible1 = FALSE;
		}
	Redraw();
	return TRUE;
}

int CPopUpMenu::CursorRight()
{
	DEBUGMSG(1,("CPopUpMenu::CursorRight()\n"));

/*
	if( SubMenuList[Cursor0]->PointedMenuItemIdx < 0 )	
		Cursor1 = SubMenuList[Cursor0]->SubMenuItemList.Count() - 1;
	else if( SubMenuList[Cursor0]->PointedMenuItemIdx >=0 )
		Cursor1 = 0;
*/
	
	if( (Cursor1 = SubMenuList[Cursor0]->PointedMenuItemIdx) < 0 )
		{
		DEBUGMSG(1,("-Cursor1 : %d -> ", Cursor1 ));
		Cursor1 = 0;
		DEBUGMSG(1,("%d\n", Cursor1 ));
		}
	else if( Cursor1 > 20 )
		{
		DEBUGMSG(1,("-Cursor1 : %d -> ", Cursor1 ));
		Cursor1 = 0;
		DEBUGMSG(1,("%d\n", Cursor1 ));
		}

	//CurrentSelected_SubMenuItem = SubMenuList[Cursor0]->SubMenuItemList[Cursor1];
	//SubMenuList[Cursor0]->PointedMenuItemIdx = Cursor1;

	if( !isSubMenuSelected )
		{
		DEBUGMSG(1,("-isSubMenuSelected=FALSE\n"));
		DEBUGMSG(1,("-CurrentSelected_SubMenu->MenuItemProc : 0x%x\n", SubMenuList[Cursor0]->MenuItemProc));

//		if( SubMenuList[Cursor0]->DisplayNameRes == ML_CLOSE )
		if( 	SubMenuList[Cursor0]->SubMenuItemList.Count() == 0 &&
			SubMenuList[Cursor0]->MenuItemProc != NULL )	
			{
			if( SubMenuList[Cursor0]->DisplayNameRes == ML_CLOSE )
				{
				CursorLeft();
				}
			else
				{
				return( SubMenuList[Cursor0]->MenuItemProc( SubMenuList[Cursor0]->Parameters ) );
				}
			}

#if 0
		else if( SubMenuList[Cursor0]->MenuItemProc != NULL  || SubMenuList[Cursor0]->SubMenuItemList.Count() > 0)
			{
			DEBUGMSG(1,("-HighlightOn(SUBMENU_CHILD)\n"));
			HighlightOn(SUBMENU_CHILD);
			Redraw();
			}
#else
		else if( SubMenuList[Cursor0]->MenuItemProc != NULL || SubMenuList[Cursor0]->SubMenuItemList.Count() > 0)
			{

			if( SubMenuList[Cursor0]->MenuItemProc != NULL )
				SubMenuList[Cursor0]->MenuItemProc( SubMenuList[Cursor0]->Parameters );

			if( SubMenuList[Cursor0]->SubMenuItemList.Count() > 0 )
				{
				DEBUGMSG(1,("-HighlightOn(SUBMENU_CHILD)\n"));
				HighlightOn(SUBMENU_CHILD);
				Redraw();
				}
			}

#endif

		else
			{
			CursorLeft();
			}
		}
	else
		{
		DEBUGMSG(1,("-isSubMenuSelected=TRUE ... Select()\n"));
		return Select();
		}
	
	return TRUE;	
}


int CPopUpMenu::Select()
{
	MENU_PROCESSOR fpMenuProc;
	RMuint8* Parameters;
	
	DEBUGMSG(1,("CPopUpMenu::Select()\n" ));


	if( !isSubMenuSelected )
		{

		DEBUGMSG(1,("-isSubMenuSelected=FALSE\n" ));
		
		if( SubMenuList[Cursor0]->MenuItemProc != NULL 
			|| SubMenuList[Cursor0]->SubMenuItemList.Count() > 0)	
			{
			DEBUGMSG(1,("-CurrentSelected_SubMenu->MenuItemProc=TRUE ... CurrentSelected_SubMenu->MenuItemProc()\n" ));
			fpMenuProc = SubMenuList[Cursor0]->MenuItemProc;
			Parameters = SubMenuList[Cursor0]->Parameters;
			Hide(FALSE);
			return (fpMenuProc( Parameters ));
			}
		else
			{
			DEBUGMSG(1,("-CurrentSelected_SubMenu->MenuItemProc=FALSE ... CursorLeft()\n" ));
			CursorLeft();
			}
		}
	else
		{
		DEBUGMSG(1,("-isSubMenuSelected=TRUE\n" ));
		
		if( SubMenuList[Cursor0]->SubMenuItemList[Cursor1]->MenuItemProc != NULL )	
			{
			DEBUGMSG(1,("-CurrentSelected_SubMenu->MenuItemProc=TRUE ... CurrentSelected_SubMenu->MenuItemProc()\n" ));

			if( SubMenuList[Cursor0]->SelectedMenuItemIdx > -1 
				&& SubMenuList[Cursor0]->SubMenuItemList[Cursor1]->isMarkable == TRUE )
				SubMenuList[Cursor0]->SelectedMenuItemIdx = Cursor1;

			DEBUGMSG(1,("-SubMenuList[Cursor0]->SelectedMenuItemIdx : %d\n", SubMenuList[Cursor0]->SelectedMenuItemIdx ));
			
			fpMenuProc = SubMenuList[Cursor0]->SubMenuItemList[Cursor1]->MenuItemProc;
			DEBUGMSG(1,("-fpMenuProc : 0x%x\n", fpMenuProc ));
			
			Parameters = SubMenuList[Cursor0]->SubMenuItemList[Cursor1]->Parameters;
			DEBUGMSG(1,("-Parameters : %d\n", Parameters ));
			Hide(FALSE);
			return( fpMenuProc( Parameters ) );
			}
		else
			{
			DEBUGMSG(1,("-CurrentSelected_SubMenu->MenuItemProc=FALSE ... CursorLeft()\n" ));
			CursorLeft();
			}
		}

	return TRUE;
}


void CPopUpMenu::HighlightOn( int Menu )
{
	DEBUGMSG(1,("CPopUpMenu::HighlightOn()\n" ));

	if( !Menu ) 	// SUBMENU_MOTHER
		{
		DEBUGMSG(1,("-Menu : FALSE\n" ));
		isVisible0 = TRUE;
		isSubMenuSelected = FALSE;
		}
	else
		{
		DEBUGMSG(1,("-Menu : TRUE\n" ));
		isVisible0 = TRUE;
		isVisible1 = TRUE;
		isSubMenuSelected = TRUE;
		}
}

void CPopUpMenu::Show()
{
	DEBUGMSG(1,("CPopUpMenu::Show()\n" ));

	if(!isVisible0)
		{
		pMpegDecoder->SaveOsdBits( pO, sbsx, sbsy, sbw, sbh );
		isVisible0 = TRUE;
		}

	Redraw();
}

void CPopUpMenu::Redraw()
{
	DEBUGMSG(1,("CPopUpMenu::Redraw()\n" ));
	pMpegDecoder->RenderOsdBits_WithoutRefresh( pO, sbsx, sbsy, sbw, sbh );	
	BuildBackground();
	BuildSelectBar();
	BuildForegroundContents();
	pMpegDecoder->RefreshOsd();
}

void CPopUpMenu::Hide( int Menu, int fRecoverOSD )
{

	DEBUGMSG(1,("CPopUpMenu::Hide()\n" ));

	if( !Menu )
		isVisible0 = FALSE;
	else
		isVisible0 = TRUE;
	
	isVisible1 = FALSE;
	isSubMenuSelected = FALSE;

	if(fRecoverOSD)
		pMpegDecoder->RenderOsdBits( pO, sbsx, sbsy, sbw, sbh );
}

void CPopUpMenu::PushOsd()
{

}

void CPopUpMenu::PopOsd()
{
}


void CPopUpMenu::ShowStatus()
{
	int i;
	
	DEBUGMSG(1,("CPopUpMenu::ShowStatus()\n" ));
	DEBUGMSG(1,("-sx0 : %d\n-sx1 : %d\n-sy0 : %d\n-sy1 : %d\n-w0 : %d\n-w1 : %d\n-h0 : %d\n-h1 : %d\n-Cursor0 : %d\n-Cursor1 : %d\n-isVisible0 : %d\n-isVisible1 : %d\n-Color0 : %d\n-Color1 : %d\n-BarColor0 : %d\n-BarColor1 : %d\n-BackgroundColor0 : %d\n-BackgroundColor1 : %d\n-isSubMenuSelected : %d\n-isChildMenu : %d\n-LargestSubMenu_StringLength : %d\n-LargestSubMenuItem_StringLength : %d\n",
		sx0,	sx1,	sy0,	sy1,	w0,	w1,	h0,	h1,	Cursor0,	Cursor1,	isVisible0, isVisible1, Color0, Color1, BarColor0, BarColor1, BackgroundColor0, BackgroundColor1, isSubMenuSelected, isChildMenu, LargestSubMenu_StringLength, LargestSubMenuItem_StringLength ));

	
	DEBUGMSG(1,("-SubMenuList[%d] (%d): 0x%x\n", 0, SubMenuList[0]->DisplayNameRes, SubMenuList[0] ));
	DEBUGMSG(1,("-SubMenuList[%d] (%d): 0x%x\n", 1, SubMenuList[1]->DisplayNameRes, SubMenuList[1] ));
	DEBUGMSG(1,("-SubMenuList[%d] (%d): 0x%x\n", 2, SubMenuList[2]->DisplayNameRes, SubMenuList[2] ));
	
}

int CPopUpMenu::MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{

	DEBUGMSG(1,("CPopUpMenu::MsgProc( %d, %d, %d, %d )\n", ssTaskId, ulMsgId, ssParam1, ssParam2 ));

/*

	if( fpMenuItemMsgProc != NULL )
		{
		return fpMenuItemMsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2 );
		}
*/

	//////////////////////////////////////////////////////////////////////
	//	MsgBtnUp
	//	Key Message Process
	
	if( ulMsgId == MSG_BUTTON_UP )
		{
		extern int g_KillAll;

		switch( ssParam1 )
			{
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_UP:
				CursorUp();
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_DOWN:
				CursorDown();
				break;
				
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LEFT:
			case TAVI_KEY_EXIT:
				CursorLeft();
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_RIGHT:
			case TAVI_KEY_ENTER:
				CursorRight();
				break;
	
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_MENU:
				if(isSubMenuSelected)
					CursorLeft();	
				CursorLeft();
				break;
				
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LEXIT:				
			case TAVI_KEY_MODE:
			case TAVI_KEY_LMODE:
				if(isSubMenuSelected) CursorLeft();	
				CursorLeft();				
				return -1;
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_SHUTDOWN:
				return -1;
				break;
			}
		}


	//////////////////////////////////////////////////////////////////////
	//	MsgBtnUp
	//	Touch Pad Message Process

	else if( ulMsgId == MSG_TOUCHPAD )
		{

		if( ssParam1 == -1 )
			{
			CursorUp();
			}
		else if( ssParam1 == 1 )
			{
			CursorDown();
			}
		else if( ssParam2 == 1 )
			{
			CursorRight();
			}
		}

	return 0;
}






