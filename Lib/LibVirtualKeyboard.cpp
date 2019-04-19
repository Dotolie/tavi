//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Virtual Keyboard Libaray class declaration
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibVirtualKeyboard.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//******************************************************************************
// $Log: LibVirtualKeyboard.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.14  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.13  2005/10/28 11:15:15  etnlwind
// Update for Palette
//
// Revision 1.12  2005/10/19 05:31:51  etnlwind
// DEBUG Removed
//
// Revision 1.11  2005/10/18 10:21:37  etnlwind
// Update for Dbx Auto Limiter
//
// Revision 1.10  2005/10/18 09:05:14  etnlwind
// Update for New DialgBox, MessageBox UI
//
// Revision 1.9  2005/10/06 08:29:13  etnlwind
// Update for TextViewer,  MainMenu
//
// Revision 1.8  2005/09/29 05:01:11  etnlwind
// Update for FileManager
//
// Revision 1.7  2005/08/31 13:38:56  etnlwind
// Update for Library
//
// Revision 1.6  2005/08/29 07:26:27  etnlwind
// Upate for FileManager BugFIx
//
// Revision 1.5  2005/08/27 06:20:05  etnlwind
// Upate for FileManager
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

#include "LibVirtualKeyboard.h"
#include "LibDefines.h"

extern CTextEngine_BDF* pT;

char KeyArray_ENG_BE[50] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', '1', '2', '3',
	'H', 'I', 'J', 'K', 'L', 'M', 'N', '4', '5', '6',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', '7', '8', '9',
	'V', 'W', 'X', 'Y', 'Z', '.', '$', '~', '0', '~',		// $ : [<-] (3,6)
	'@', '@', '@', '@', '%', '%', '%', '~', '~', '~'	// @ : Enter(4,0),  % : CapsLock(4,1)
};
char KeyArray_ENG_LE[50] =
{
	'a', 'b', 'c', 'd', 'e', 'f', 'g', '1', '2', '3',
	'h', 'i', 'j', 'k', 'l', 'm', 'n', '4', '5', '6',
	'o', 'p', 'q', 'r', 's', 't', 'u', '7', '8', '9',
	'v', 'w', 'x', 'y', 'z', '.', '$', '~', '0', '~',
	'@', '@', '@', '@', '%', '%', '%', '~', '~', '~'
};

#define COLORINDEX_MSGBOX_TITLE_BG 126

extern U16 TopLeftRound[4][4];
extern U16 TopRightRound[4][4];
extern U16 BottomLeftRound[4][4];
extern U16 BottomRightRound[4][4];

//CVirtualKeyboard::CVirtualKeyboard( U32 riTitle, MSG_PROCESSOR pfpMsgProc, U32 option )
CVirtualKeyboard::CVirtualKeyboard( U32 riTitle, uchar_t* SrcFileName, U32 option )
{
	width = 300;
	height = 210;
	sx = 320/2 - width/2;
	sy = 240/2 - height/2 + 5;
	kCol = 0;
	kRow = 0;
	CurrentUsedBuffer = 0;

	VkEngBe = new VIRTUAL_KEYBOARD;
	VkEngBe->MaxRow = 4;
	VkEngBe->MaxCol = 9;
	VkEngBe->fUnicode = 0;
	VkEngBe->KeyArray = KeyArray_ENG_BE;
	
	VkEngLe = new VIRTUAL_KEYBOARD;
	VkEngLe->MaxRow = 4;
	VkEngLe->MaxCol = 9;
	VkEngLe->fUnicode = 0;
	VkEngLe->KeyArray = KeyArray_ENG_LE;

	SetKBD( VkEngBe );

//	fpMsgProc_External = pfpMsgProc;
	TitleRes = riTitle;
	BoxOptions = option;
	ConfirmPass = FALSE;

	DisplayText = (uchar_t*) malloc(sizeof(uchar_t)*255);
	memset( DisplayText, 0, sizeof(uchar_t)*255 );

	if(riTitle==ML_RENAME)
		ustrcpy( DisplayText, SrcFileName );
	
}

CVirtualKeyboard::~CVirtualKeyboard()
{
	if( VkEngBe != NULL )
		{
		delete VkEngBe;
		VkEngBe = NULL;
		}

	if( VkEngLe != NULL )
		{
		delete VkEngLe;
		VkEngLe = NULL;
		}

	if( DisplayText != NULL )
		{
		free( DisplayText );
		DisplayText = NULL;
		}
	
}
/*
#define KBD_BG_SELECTED	64
#define KBD_BG_NORMAL 		89
#define KBD_BG_GRAY		91
#define TEXTBOX_OUTLINE	92
#define TEXTBOX_OUTLINE_SHADOW	90
*/

void CVirtualKeyboard::BuildNormalKey( uchar_t KeyPrint, int x, int y, int isSelected )
{
	U16 BgColor;
	U16 TxtColor;
	CSize ChrSize;
	uchar_t KeyPrintString[2];

	if( isSelected == 1)
		{
		BgColor = KBD_BG_SELECTED;	//_ORANGE;
		TxtColor = _WHITE;
		}
	else if( isSelected == -1)
		{
		BgColor = KBD_BG_GRAY;
		}
	else
		{
		BgColor = KBD_BG_NORMAL;
		TxtColor = _BLACK;
		}

	DrawBoxWithoutRefresh( BgColor, x+1, y, KEY_W-2, 1 );
	DrawBoxWithoutRefresh( BgColor, x, y+1, KEY_W, KEY_H-2 );
	DrawBoxWithoutRefresh( BgColor, x+1, y+KEY_H-1, KEY_W-2, 1 );

	KeyPrintString[0] = KeyPrint;
	KeyPrintString[1] = 0x0000;

	pT->GetCharSize(KeyPrint, 12, &ChrSize);
	pT->DrawUC16Text( KeyPrintString, x+(KEY_W/2- ChrSize.width/2)-1, y+4, TxtColor, 12, DT_TRANSPARENCY );
	
}

//BuildSpecialKey( "Caps Lock", kx+NumPadSpace+(col*(KEY_W+KEY_D)), ky+(row*(KEY_H+KEY_D)), isSelected, KEY_MW );
void CVirtualKeyboard::BuildSpecialKey( char* KeyPrint, int x, int y, int isSelected, int KeyWidth )
{
	U16 BgColor;
	U16 TxtColor;
	int IconIdx;

	if( isSelected )
		{
		BgColor = KBD_BG_SELECTED;	//_ORANGE;
		TxtColor = _WHITE;
		}
	else
		{
		BgColor = KBD_BG_NORMAL;
		TxtColor = _BLACK;
		}
	DrawBoxWithoutRefresh( BgColor, x+1, y, KeyWidth-2, 1 );
	DrawBoxWithoutRefresh( BgColor, x, y+1, KeyWidth, KEY_H-2 );
	DrawBoxWithoutRefresh( BgColor, x+1, y+KEY_H-1, KeyWidth-2, 1 );

	pT->DrawEUC8Text( KeyPrint, x+(KeyWidth/2 - pT->GetStringWidth( KeyPrint, 12, DT_TRANSPARENCY )/2 ), y+4, TxtColor, 12, DT_TRANSPARENCY );
	
}

void CVirtualKeyboard::BuildImageKey( U16 PictureID, int x, int y, int isSelected )
{
	U16 BgColor;
	U16 RealPictureID;

	if( isSelected )
		{
		BgColor = KBD_BG_SELECTED;
		RealPictureID  = PictureID+1;
		}
	else
		{
		BgColor = KBD_BG_NORMAL;		//KBD_BG_NORMAL;
		RealPictureID =  PictureID;
		}

	DrawBoxWithoutRefresh( BgColor, x+1, y, KEY_W-2, 1 );
	DrawBoxWithoutRefresh( BgColor, x, y+1, KEY_W, KEY_H-2 );
	DrawBoxWithoutRefresh( BgColor, x+1, y+KEY_H-1, KEY_W-2, 1 );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[VIRTUAL_KEY], RealPictureID, x+(KEY_W/2- 12/2)+1, y+6, 12, 9 );
	
}

void CVirtualKeyboard::BuildDbxBackground()
{
	// Background
	DrawBoxWithoutRefresh( _BLACK, sx+4, sy, width-8, 1); 
	DrawBoxWithoutRefresh( COLORINDEX_MSGBOX_TITLE_BG, sx+4, sy+1, width-8, 3);
	DrawBoxWithoutRefresh( COLORINDEX_MSGBOX_TITLE_BG, sx+1, sy+2, width-2, 24 );
	DrawBoxWithoutRefresh( _BLACK, sx, sy+4, 1, height-8);
	DrawBoxWithoutRefresh( _BLACK, sx+width-1, sy+4, 1, height-8);

	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			DrawPixelWithoutRefresh( TopLeftRound[y][x] , sx+x, sy+y);

	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			DrawPixelWithoutRefresh( TopRightRound[y][x] , sx+width-4+x, sy+y);

	csx = sx+1;
	csy = sy+1+24;
		
	DrawBoxWithoutRefresh( _WHITE, csx, csy, width-2, height-1-24-4 );
	DrawBoxWithoutRefresh( _WHITE, sx+4, csy+height-1-24-4, width-8, 3 );
	DrawBoxWithoutRefresh( _BLACK, sx+4, csy+height-2-24, width-8, 1 );


	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			DrawPixelWithoutRefresh( BottomLeftRound[y][x] , sx+x, sy+height-4+y);

	for(int y=0; y<4; y++)
		for(int x=0; x<4; x++)
			DrawPixelWithoutRefresh( BottomRightRound[y][x] , sx+width-4+x, sy+height-4+y);

}


void CVirtualKeyboard::BuildTextArea()
{
	taW = ((KEY_W+KEY_D)*10)+8-KEY_D;
	taH = 21;
	taX = csx+14;
	taY = csy+38-4-taH;
	tX = taX+TEXT_DIFF_X;
	tY = taY+TEXT_DIFF_Y;
	
#if 0
	DrawBoxWithoutRefresh( _GRAY80, taX, taY, taW, taH );
	DrawBoxWithoutRefresh( _BLACK, taX+1, taY+1, taW-1, taH-2 );
	DrawBoxWithoutRefresh( _GRAY40, taX+taW-1, taY+1, 1, taH-2 );
	DrawBoxWithoutRefresh( _GRAY40, taX+1, taY+taH-1, taW-1, 1 );
	DrawBoxWithoutRefresh( _WHITE, taX+2, taY+2, taW-3, taH-3 );
#else
	DrawBoxWithoutRefresh( _GRAY80, taX, taY, taW, taH );
	DrawBoxWithoutRefresh( TEXTBOX_OUTLINE_SHADOW, taX+1, taY+1, taW-1, taH-2 );
	DrawBoxWithoutRefresh( _GRAY40, taX+taW-1, taY+1, 1, taH-2 );
	DrawBoxWithoutRefresh( _GRAY40, taX+1, taY+taH-1, taW-1, 1 );
	DrawBoxWithoutRefresh( _WHITE, taX+2, taY+2, taW-3, taH-3 );
#endif
}



void CVirtualKeyboard::ClearTextArea()
{
	etx = 0;
	DrawBoxWithoutRefresh( _WHITE, taX+2, taY+2, taW-3, taH-3 );
}

void CVirtualKeyboard::BuildCursor( int ix )
{
	//DrawBoxWithoutRefresh( _BLACK, (tX+ix), tY, 1, TA_CURSOR_HEIGHT );
	DrawBoxWithoutRefresh( _BLACK, (ix), tY, 1, TA_CURSOR_HEIGHT );
}

void CVirtualKeyboard::BuildText()
{
	int TxtBoxW = taW-3-8;
	int TxtW = pT->GetStringWidth( DisplayText, 12, DT_TRANSPARENCY );
	int RelativeTxtStartX;

	ClearTextArea();
	etx = pT->DrawUC16Text( DisplayText, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
		
	if( TxtBoxW > TxtW )
		{
		pT->DrawScrollText(0);
		BuildCursor(etx+2);
		}

	else
		{
		pT->DrawScrollText((RelativeTxtStartX=TxtW-TxtBoxW));
		BuildCursor(etx-RelativeTxtStartX+2);
		}

	

}

/*
	'A', 'B', 'C', 'D', 'E', 'F', 'G', '1', '2', '3',
	'H', 'I', 'J', 'K', 'L', 'M', 'N', '4', '5', '6',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', '7', '8', '9',
	'V', 'W', 'X', 'Y', 'Z', '_', '$', ' ', '0', ' ',		// $ : [<-] (3,6)
	'@', '%', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '			// @ : Enter(4,0),  % : CapsLock(4,1)


	'a', 'b', 'c', 'd', 'e', 'f', 'g', '1', '2', '3',
	'h', 'i', 'j', 'k', 'l', 'm', 'n', '4', '5', '6',
	'o', 'p', 'q', 'r', 's', 't', 'u', '7', '8', '9',
	'v', 'w', 'x', 'y', 'z', '_', '$', '~', '0', '~',
	'@', '@', '@', '@', '%', '%', '%', '~', '~', '~'
	
*/
void CVirtualKeyboard::BuildKeys()
{
	int kx = csx+14;
	int ky = csy+38;
	int row, col, isSelected;
	int NumPadSpace;
	int is_ENTER_Drawed = 0;
	int is_CAPSLOCK_Drawed = 0;

	for( row=0; row<=Vk->MaxRow; row++ )
		{


		for( col=0; col<=Vk->MaxCol; col++ )
			{

			if( kRow == row && kCol == col ) isSelected = TRUE;
			else isSelected = FALSE;

			if( col >= 7 ) NumPadSpace = 8;
			else NumPadSpace = 0;
			
			if( Vk->KeyArray[row*(Vk->MaxCol+1)+col] == '$' )
				BuildImageKey( 0, kx+NumPadSpace+(col*(KEY_W+KEY_D)), ky+(row*(KEY_H+KEY_D)), isSelected );

			else if( Vk->KeyArray[row*(Vk->MaxCol+1)+col] == '@'  )
				{

				if( is_ENTER_Drawed==0 || isSelected==TRUE)
					{
					sprintf( KeyPrintfBuf, "%s", "Enter" );
					BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(0*(KEY_W+KEY_D)), ky+(row*(KEY_H+KEY_D)), isSelected, KEY_LW );
					is_ENTER_Drawed = TRUE;
					}
				}

			else if( Vk->KeyArray[row*(Vk->MaxCol+1)+col] == '%' )
				{
				if( is_CAPSLOCK_Drawed==0 ||isSelected==TRUE )
					{
					sprintf( KeyPrintfBuf, "%s", "Caps Lock" );
					BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(4*(KEY_W+KEY_D)), ky+(row*(KEY_H+KEY_D)), isSelected, KEY_MW );
					is_CAPSLOCK_Drawed = TRUE;
					}
				}

			else if( Vk->KeyArray[row*(Vk->MaxCol+1)+col] == '~' )
				BuildNormalKey( ' ', kx+NumPadSpace+(col*(KEY_W+KEY_D)), ky+(row*(KEY_H+KEY_D)), -1 );

			else if( Vk->KeyArray[row*(Vk->MaxCol+1)+col] == ' ' )
				{;}

			else
				BuildNormalKey( (uchar_t)Vk->KeyArray[row*(Vk->MaxCol+1)+col], kx+NumPadSpace+(col*(KEY_W+KEY_D)), ky+(row*(KEY_H+KEY_D)), isSelected );


			}


		}

	
	
}


#if 1
void CVirtualKeyboard::BuildButtons()
{
#define ICON_DBXICON_WIDTH 	24
#define BTN_ICON_MAX_X 			295

	int tw2, tw3, x2, x3;
	int btn_sy = sy+height-16-6;
	
	tw2 = pT->GetStringWidth( (U32)ML_SELECT, 12, 0 );
	tw3 = pT->GetStringWidth( (U32)ML_CANCEL, 12, 0 );
	x3 = BTN_ICON_MAX_X - tw3 - (ICON_DBXICON_WIDTH+2);
	x2 = x3 - tw2- (ICON_DBXICON_WIDTH+6);		

	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON_GRAY], 0, x2, btn_sy-2, 24, 14 );
	pT->DrawResText( ML_SELECT, x2+26, btn_sy, _BLACK, 12, DT_TRANSPARENCY );

	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON_GRAY], 2, x3, btn_sy-2, 24, 14 );
	pT->DrawResText( ML_CANCEL, x3+26, btn_sy, _BLACK, 12, DT_TRANSPARENCY );
}
#endif

void CVirtualKeyboard::SetKBD( VIRTUAL_KEYBOARD* input )
{
	Vk = input;
}

void CVirtualKeyboard::SetUsedStringBuffer( int FullpathNameLength)
{
	CurrentUsedBuffer = FullpathNameLength;
}



int CVirtualKeyboard::CursorUp()
{
/*
	if( kRow <= 0 ) kRow = Vk->MaxRow;
	else kRow--;

	for(int loop=0;loop<(Vk->MaxRow*2);loop++)
		{
		if( Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol] == ' ' || Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol] == '~' ) kRow--;
		else break;
		}
*/

	char PreKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	char NextKey;

	if( kRow <= 0 ) kRow = Vk->MaxRow;
	else kRow--;
	
	NextKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];

	for(int loop=0;loop<(Vk->MaxRow*2);loop++)
		{
		if( NextKey == ' ' || NextKey == '~' || NextKey == PreKey ) 
			{
			if( kRow <= 0 ) kRow = Vk->MaxRow;
			else kRow--;
			NextKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
			}
		else 
			break;
		}	
}


int CVirtualKeyboard::CursorDown()
{
	char PreKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	char NextKey;

	if( kRow >= Vk->MaxRow ) kRow = 0;
	else kRow++;

	NextKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];

	for(int loop=0;loop<(Vk->MaxRow*2);loop++)
		{
		if( NextKey == ' ' || NextKey == '~' || NextKey == PreKey ) 
			{
			if( kRow >= Vk->MaxRow ) kRow = 0;
			else kRow++;
			NextKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
			}
		else 
			break;
		}
}

int CVirtualKeyboard::CursorLeft()
{
	char PreKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	char NextKey;

	if( kCol <= 0 ) kCol = Vk->MaxCol;
	else kCol--;

	NextKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	
	for(int loop=0;loop<(Vk->MaxCol*2);loop++)
		{
		if( NextKey == ' ' || NextKey == '~' || NextKey == PreKey ) 
			{
			if( kCol <= 0 ) kCol = Vk->MaxCol;
			else kCol--;
			NextKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
			}
		else 
			break;
		}

}

int CVirtualKeyboard::CursorRight()
{
/*
	if( kCol >= Vk->MaxCol ) kCol = 0;
	else kCol++;

	for(int loop=0;loop<(Vk->MaxCol*2);loop++)
		{
		if( 	Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol] == ' ' || 
			Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol] == '~' ) 
			{
			kCol++;
			}
		else
			{
			break;
			}
		}
*/	

	char PreKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	char NextKey;

	if( kCol >= Vk->MaxCol ) kCol = 0;
	else kCol++;

	NextKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	
	for(int loop=0;loop<(Vk->MaxCol*2);loop++)
		{
		if( NextKey == ' ' || NextKey == '~' || NextKey == PreKey ) 
			{
			if( kCol >= Vk->MaxCol ) kCol = 0;
			else kCol++;
			NextKey = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
			}
		else 
			break;
		}
	
}

int CVirtualKeyboard::CursorOK()
{
	char input = Vk->KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	int szText;
	char temp[255];


	DEBUGMSG(1,("+CursorOK()\n"));
	DEBUGMSG(1,("-input:0x%xh\n",input));

	if( 	input >= 'A' && input <= 'Z' ||
		input >= 'a' && input <= 'z' ||
		input >= '0' && input <= '9' ||
		input == '.'  )
		{

		DEBUGMSG(1,("-case:normal\n"));
		DEBUGMSG(1,("-(A)ustrlen(DisplayText) : %d\n", ustrlen(DisplayText) ));
		DEBUGMSG(1,("-(B)CurrentUsedBuffer : %d\n", CurrentUsedBuffer ));
		DEBUGMSG(1,("-(C)FATFS_MAX_PATHNAME_LENGTH : %d\n", FATFS_MAX_PATHNAME_LENGTH));

		if(ustrlen(DisplayText)+CurrentUsedBuffer <= (FATFS_MAX_PATHNAME_LENGTH-3))
			{
			DEBUGMSG(1,("-(A)+(B) ... %d <= (C) ... %d\n", ustrlen(DisplayText)+CurrentUsedBuffer, (FATFS_MAX_PATHNAME_LENGTH-3) ));
			ustr2str( temp, DisplayText );
			szText = ustrlen(DisplayText);
			temp[szText] = input;
			temp[szText+1] = '\0';
			str2ustr( DisplayText, temp, strlen(temp) );
			ReDraw();
			}
		else
			{
			DEBUGMSG(1,("-(A)+(B) ... %d > (C) ... %d\n", ustrlen(DisplayText)+CurrentUsedBuffer, (FATFS_MAX_PATHNAME_LENGTH-3) ));
			}
		}
	else if( input == '$' )	// BS
		{
		DEBUGMSG(1,("-case:basckspace\n"));
		ustr2str( temp, DisplayText );
		szText = ustrlen(DisplayText);
		temp[szText-1] = '\0';
		str2ustr( DisplayText, temp, strlen(temp) );
		ReDraw();
		}
	else if( input == '%')	// CapsLock
		{
		DEBUGMSG(1,("-case:capslock\n"));
		if( Vk == VkEngBe )
			SetKBD(VkEngLe);
		else	 if( Vk == VkEngLe )
			SetKBD(VkEngBe);
		ReDraw();
		}
	else if( input == '@')	// Enter
		{
		DEBUGMSG(1,("-case:enter\n"));
		return -1;
		}
	return 0;
}

void CVirtualKeyboard::Show()
{
	BuildDbxBackground();
	BuildTitle();
	BuildTextArea();
	BuildButtons();
	ReDraw();
}

void CVirtualKeyboard::ReDraw()
{
	BuildText();
	BuildKeys();
	isVisible = TRUE;
	pMpegDecoder->RefreshOsd();
}

int CVirtualKeyboard::MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	DEBUGMSG(1,("CVirtualKeyboard::MsgProc( %d, %d, %d, %d )\n", ssTaskId, ulMsgId, ssParam1, ssParam2 ));

		//////////////////////////////////////////////////////////////////////
		//	MsgProc
		//	Key Message Process
		
		if( ulMsgId == MSG_BUTTON_UP )
			{

			switch( ssParam1 )
				{
				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_UP:
					CursorUp();
					ReDraw();
					break;

				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_DOWN:
					CursorDown();
					ReDraw();
					break;
					
				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_LEFT:
					CursorLeft();
					ReDraw();
					break;

				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_EXIT:
					return -1;
					break;

				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_RIGHT:
					CursorRight();
					ReDraw();
					break;

				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_ENTER:
					if( CursorOK() == -1 )
						{
						ConfirmPass = TRUE;
						return -1;
						}
					break;
		
				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_MENU:
					break;
					
				//////////////////////////////////////////////////////////////////////
				case TAVI_KEY_LEXIT:				
				case TAVI_KEY_MODE:
				case TAVI_KEY_LMODE:
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
				CursorLeft();
				}
			else if( ssParam1 == 1 )
				{
				CursorRight();
				}
			else if( ssParam2 == 1 )
				{
				CursorRight();
				}
			}

		return 0;
}


	
