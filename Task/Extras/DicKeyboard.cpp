//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Dictionary Keyboard class declaration
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/DicKeyboard.cpp,v $
// $Revision: 1.6 $
// $Date: 2006/03/06 11:18:22 $
// Author:
//
//******************************************************************************
// $Log: DicKeyboard.cpp,v $
// Revision 1.6  2006/03/06 11:18:22  akaiaphanu
// working UI
//
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
// Revision 1.9  2006/02/23 01:56:58  akaiaphanu
// working search function
//
// Revision 1.8  2006/02/22 07:55:52  akaiaphanu
// working text resource , cleaning
//
// Revision 1.7  2006/02/22 04:16:28  akaiaphanu
// working selectbar, itemlist
//
// Revision 1.6  2006/02/20 01:33:56  akaiaphanu
// add ListManager , BitIO
//
// Revision 1.5  2006/02/14 08:37:16  akaiaphanu
// working bottom guide , scrollbar , box
//
// Revision 1.4  2006/02/11 12:40:14  akaiaphanu
// working dictionary keyboard
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

#include "DicKeyboard.h"
#include "../../Lib/LibDefines.h"

extern CTextEngine_BDF* pT;
extern EXTRA_DIC_CONFIG	 _ExDicPro;

char 			Dic_Eng_KeyArray[20] = {
				'a', 'b', 'c', '1', '2', 
				'd', 'e', 'f', '3', '4', 
				'g', 'h', 'i', '5', '6',
				'j', 'k', 'l', '7', '8' 
};
unsigned short	Dic_Kor_KeyArray[12][3] = {
				{'R', 'Z', 'r'}, {'S', 'F', 0x00}, {'E', 'X', 'e'},
				{'A', 'T', 't'}, {'H', 'Y', 0x00}, {'Q', 'V', 'q'},
				{'J', 'U', 0x00}, {'L', 'M', 0x00}, {'K','I',0x00},
				{'W', 'C', 'w'}, {'N','B',0x00}, {'D','G',0x00}
};

struct timeval tv_cur = { 0, 0 };
struct timeval tv_last;

#define COLORINDEX_MSGBOX_TITLE_BG 126

DicKeyboard::DicKeyboard( int x, int y, int box_width, int box_height)
{
	sx = 					x;
	sy = 					y;
	width = 					box_width;
	height = 					box_height;
	kCol = 					0;
	kRow = 					0;

	m_Xpos = 				x;
	m_Ypos = 				y;
	m_Width = 				box_width;
	m_Height = 				box_height;

	m_Title_Xpos = 			m_Xpos + 10;
	m_Title_Ypos = 			m_Ypos + 5;
	m_Title_Height = 			21;
	
	m_mode = 				DIC_MODE_KOR;
	m_isTimeRemain = 		false;
	m_isChangeKeyboard =		false;
	m_isToggle = 				false;
	m_Time_Proc = 			false;
	m_PassByKeyPress = 		false;
	m_PassByKorKeyPress =	false;
	m_PassByKeyShift = 		false;
	m_PassByTimer = 			false;
	m_BackToKeyboard = 		false;
	m_Key_New = 			'a';
	m_Key_Old = 				'0';
	m_cursor_pos = 			'a';
	m_kor_code = 			0x00;
	
	Vk = 					new DIC_KEYBOARD;
	Vk->MaxRow = 			3;
	Vk->MaxCol = 			4;
	Vk->fUnicode = 			0;
	Vk->Dic_Eng_KeyArray = 	Dic_Eng_KeyArray;

	vk_kor.HAN_INPUT_KEY_BUFFER_Create(&m_kor_key_buf, 100);
	vk_kor.isSHIFT = 			0;
	
	SetKBD(DIC_MODE_KOR);

	m_DisplayText_Pre = (uchar_t*) malloc(sizeof(uchar_t)*255);
	memset( m_DisplayText_Pre, 0, sizeof(uchar_t)*255 );
	
	m_DisplayText_Suf = (uchar_t*) malloc(sizeof(uchar_t)*255);
	memset( m_DisplayText_Suf, 0, sizeof(uchar_t)*255 );

	m_DisplayText_Kor = (uchar_t*) malloc(sizeof(uchar_t)*255);
	memset( m_DisplayText_Kor, 0, sizeof(uchar_t)*255 );
	
	Key_Buffer_Clear(CLEAR_ALL);
	Key_Index_Clear(CLEAR_ALL);

}

DicKeyboard::~DicKeyboard()
{
	if( Vk != NULL )
	{
		delete Vk;
		Vk = NULL;
	}
	
	if( m_DisplayText_Pre != NULL )
	{
		free( m_DisplayText_Pre );
		m_DisplayText_Pre = NULL;
	}

	if( m_DisplayText_Suf != NULL )
	{
		free( m_DisplayText_Suf);
		m_DisplayText_Suf = NULL;
	}
	if( m_DisplayText_Kor != NULL )
	{
		free( m_DisplayText_Kor);
		m_DisplayText_Kor = NULL;
	}
	
}

void  DicKeyboard::Key_Index_Clear(int mode) 
{
	switch(mode)
	{
		case CLEAR_ALL :	/* All */
			m_shift_index = 					0;
			m_Key_Buffer_Index = 				0;
			m_Key_Buffer_Pre_Index = 			0;
			m_shift_manager.m_pre_width = 		0;
			m_shift_manager.m_suf_width = 		0;
			m_shift_manager.m_pre_hide= 		0;
			m_shift_manager.m_suf_hide= 		0;
			m_shift_manager.m_pre_index = 	0;
			m_shift_manager.m_suf_index = 		0;
			m_shift_manager.m_total_index = 	0;
			break;
		case 1: /* key shift index */
			m_shift_index = 0;
			break;
		case 2: /* Key buffer index */
			m_Key_Buffer_Index = 0;
			break;
		case 3: /* Key buffer prefix index */
			m_Key_Buffer_Pre_Index = 0;
			break;
		default :
			break;
	}
//	fprintf(stderr, "Key_Index_Clear()\n");
}
void  DicKeyboard::Key_Buffer_Clear(int mode) 
{
	int k=0;
	
	switch(mode)
	{
		case CLEAR_ALL :	/* All */
			memset( m_Key_Buffer, '\0', 3);
			//fprintf(stderr, "m_Key_Buffer ");
			//k=2;
			//while(k>-1)
			//{
			//	fprintf(stderr, "[%c] ",m_Key_Buffer[k]);
			//	k--;
			//}
			
			memset(m_Key_Buffer_Pre,'\0', KEY_BUFF_SIZE);
			//fprintf(stderr, "\nm_Key_Buffer_Pre ");
			//k=0;
			//while(k<10)
			//{
			//	fprintf(stderr, "[%c] ",m_Key_Buffer_Pre[k]);
			//	k++;
			//}
			
			memset( m_Key_Buffer_Suf, '\0', 3 );
			//fprintf(stderr, "\nm_Key_Buffer_Suf ");
			//k=2;
			//while(k>-1)
			//{
			//	fprintf(stderr, "[%c] ",m_Key_Buffer_Suf[k]);
			//	k--;
			//}
			memset( m_Key_Pressed_Info, 0, 20 );
			memset(m_shift_manager.m_pre_buffer, '\0', KEY_BUFF_SIZE);
			memset(m_shift_manager.m_suf_buffer, '\0', KEY_BUFF_SIZE);
			memset(m_DisplayText_Pre, 0, sizeof(uchar_t)*255 );
			//fprintf(stderr, "\nm_DisplayText_Pre ");
			//k=0;
			//while(k<10)
			//{
			//	fprintf(stderr, "[%d] ",m_DisplayText_Pre[k]);
			//	k++;
			//}
			break;
		case 1: /* Key Buffer */
			memset( m_Key_Buffer, '\0', 3);
			break;
		case 2: /* Key Buffer Prefix */
			memset(m_Key_Buffer_Pre,'\0', KEY_BUFF_SIZE);
			break;
		case 3: /* Key Buffer Sufix */
			memset( m_Key_Buffer_Suf, '\0', 3 );
			break;
		case 4: /* Key Pressed Info array */
			while(k<20)
			{
				m_Key_Pressed_Info[k++] = 0;
			}
			break;
		default :
			break;
	}

}
int  DicKeyboard::Get_Member(int whichmember)
{
	switch(whichmember)
	{
		case 1:
			return m_isTimeRemain;
		case 2:
			return m_Time_Proc;
		case 3:
			return m_CheckShiftStart;
		default :
			return -1;

	}
}
int  DicKeyboard::Set_Member(int whichmember, int value)
{
	switch(whichmember)
	{
		case 1:
			return m_isTimeRemain = 		value;
		case 2:
			return m_isToggle = 			value;
		case 3:
			return m_PassByTimer = 		value;
		case 4:
			return m_PassByKeyPress = 	value;
		case 5:
			return m_BackToKeyboard = 	value;
		case 6:
			return m_PassByKorKeyPress = 	value;
		default :
			return -1;

	}
}
int DicKeyboard::Check_Time_Remain()
{
	gettimeofday( &tv_cur, NULL );

	if( m_CheckTimeStart )
	{	
		tv_last.tv_sec = tv_cur.tv_sec;
		m_Time_Proc = 			true;
		m_isTimeRemain = 		true; /*  Time remain */
		m_isToggle = 				true;
		m_isShiftToggle = 			true;
		m_PassByTimer = 			false; /* No draw text */
		//m_cmd_insert = 			false; /* No draw text in shift mode */
		//fprintf(stderr,"m_Time_Proc = 	%s\n", m_Time_Proc ? "True":"False");
		//fprintf(stderr,"m_isTimeRemain = %s\n", m_isTimeRemain ? "True":"False");
		//fprintf(stderr,"m_isToggle = 		%s\n", m_isToggle ? "True":"False");
		//fprintf(stderr,"m_isShiftToggle = 	%s\n", m_isShiftToggle ? "True":"False");
		//fprintf(stderr,"m_PassByTimer = 	%s\n", m_PassByTimer ? "True":"False");
		//fprintf(stderr,"m_cmd_insert = 	%s\n", m_cmd_insert ? "True":"False");
	}
	
	/* check  per 2 sec */
	if( abs(tv_cur.tv_sec - tv_last.tv_sec) > 2 )
	{
		//fprintf(stderr,"[cur_sec] -[last_sec] = [%d]\n",abs(tv_cur.tv_sec - tv_last.tv_sec));
		m_isTimeRemain = 		false; /* No time remain */
		m_isToggle = 				false;
		m_isShiftToggle = 			false;
		//m_cmd_insert = 			true;  /* Can draw text in shift mode */
		m_PassByTimer = 			true; /* Can draw text */
		m_PassByKeyShift =		true;
		m_PassByKeyPress =		false;
		
		//fprintf(stderr,"m_Time_Proc = 	%s\n", m_Time_Proc ? "True":"False");
		//fprintf(stderr,"m_isTimeRemain = %s\n", m_isTimeRemain ? "True":"False");
		//fprintf(stderr,"m_isToggle = 		%s\n", m_isToggle ? "True":"False");
		//fprintf(stderr,"m_isShiftToggle = 	%s\n", m_isShiftToggle ? "True":"False");
		//fprintf(stderr,"m_PassByTimer = 	%s\n", m_PassByTimer ? "True":"False");
		//fprintf(stderr,"m_cmd_insert = 		%s\n", m_cmd_insert ? "True":"False");
		//fprintf(stderr,"m_PassByKeyShift = 	%s\n", m_PassByKeyShift ? "True":"False");
		
	}
	
	return 0;
}

void DicKeyboard::BuildNormalKey( uchar_t KeyPrint, int x, int y, int isSelected )
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

	DrawBoxWithoutRefresh( BgColor, x+1, y, DIC_KEY_W-2, 1 );
	DrawBoxWithoutRefresh( BgColor, x, y+1, DIC_KEY_W, DIC_KEY_H-2 );
	DrawBoxWithoutRefresh( BgColor, x+1, y+DIC_KEY_H-1, DIC_KEY_W-2, 1 );

	KeyPrintString[0] = KeyPrint;
	KeyPrintString[1] = 0x0000;

	pT->GetCharSize(KeyPrint, 12, &ChrSize);
	pT->DrawUC16Text( KeyPrintString, x+(DIC_KEY_W/2- ChrSize.width/2)-1, y+4, TxtColor, 12, DT_TRANSPARENCY );
	
}

void DicKeyboard::Draw_ButtonBox( char* KeyPrint, int x, int y, int isSelected, int width )
{
	U16 BgColor;
	U16 TxtColor;
	int IconIdx;

	width = width*2+2;

	if( isSelected )
	{
	
		BgColor = KBD_BG_NORMAL;
		TxtColor = _BLACK;
	}
	else
	{
	
		BgColor = 31;
		TxtColor = _WHITE;
	}
	DrawBoxWithoutRefresh( BgColor, x+1, y, width-2, 1 );
	DrawBoxWithoutRefresh( BgColor, x, y+1, width, DIC_KEY_H-2 );
	DrawBoxWithoutRefresh( BgColor, x+1, y+DIC_KEY_H-1, width-2, 1 );

	pT->DrawEUC8Text( KeyPrint, 
					    x+(width/2 - pT->GetStringWidth( KeyPrint, 12, DT_TRANSPARENCY )/2 ),
					    y+8, 
					    TxtColor, 
					    12,
					    DT_TRANSPARENCY );
	
}

void DicKeyboard::BuildSpecialKey( char* KeyPrint, int x, int y, int isSelected, int KeyWidth )
{
	U16 BgColor;
	U16 TxtColor;
	int IconIdx;

	if( isSelected )
	{
	//	BgColor = KBD_BG_SELECTED;	//_ORANGE;
		BgColor = KBD_BG_NORMAL;
		TxtColor = _BLACK;
	}
	else
	{
	//	BgColor = KBD_BG_NORMAL;
		BgColor = 31;
		TxtColor = _WHITE;
	}
	DrawBoxWithoutRefresh( BgColor, x+1, y, KeyWidth-2, 1 );
	DrawBoxWithoutRefresh( BgColor, x, y+1, KeyWidth, DIC_KEY_H-2 );
	DrawBoxWithoutRefresh( BgColor, x+1, y+DIC_KEY_H-1, KeyWidth-2, 1 );

	pT->DrawEUC8Text( KeyPrint, x+(KeyWidth/2 - pT->GetStringWidth( KeyPrint, 12, DT_TRANSPARENCY )/2 ), y+8, TxtColor, 12, DT_TRANSPARENCY );
	
}

void DicKeyboard::BuildImageKey( U16 PictureID, int x, int y, int isSelected )
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
		BgColor = KBD_BG_NORMAL;
		RealPictureID =  PictureID;
	}

	DrawBoxWithoutRefresh( BgColor, x+1, y, DIC_KEY_W-2, 1 );
	DrawBoxWithoutRefresh( BgColor, x, y+1, DIC_KEY_W, DIC_KEY_H-2 );
	DrawBoxWithoutRefresh( BgColor, x+1, y+DIC_KEY_H-1, DIC_KEY_W-2, 1 );
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[VIRTUAL_KEY], RealPictureID, x+(DIC_KEY_W/2- 12/2)+1, y+6, 12, 9 );
	
}
void DicKeyboard::Draw_KeyboardBox(void)
{
	int csx, csy;

	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos+3, m_Ypos, m_Width-6, 1);
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos+1, m_Ypos+1, m_Width-2, 2);
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos, m_Ypos+3, m_Width, 1);
	
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos+4, m_Ypos, m_Width-8, 1); 
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos+4, m_Ypos+1, m_Width-8, 3);
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos+1, m_Ypos+2, m_Width-2, m_Title_Height);
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos, m_Ypos+4, 1, m_Height-8);
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos+m_Width-1, m_Ypos+4, 1, m_Height-8);

	csx = m_Xpos+1;
	csy = m_Ypos+1+m_Title_Height;
		
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, csx, csy, m_Width-2, m_Height-1-m_Title_Height-4 );
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos+4, csy+m_Height-1-m_Title_Height-4, m_Width-8, 3 );
	DrawBoxWithoutRefresh( BOX_BASE_COLOR, m_Xpos+4, csy+m_Height-2-m_Title_Height, m_Width-8, 1 );

	//DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 0, m_Xpos, m_Ypos, 4,4 );
	//DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 1, m_Xpos+m_Width-4, m_Ypos, 4,4 );
	
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 2, m_Xpos, m_Ypos+m_Height-4, 4,4 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[DIALOGBOX_EDGE], 3, m_Xpos+m_Width-4, m_Ypos+m_Height-4, 4,4 );

	Draw_Guide();
	
	return ;
}

void DicKeyboard::BuildTextArea()
{
	taW = BOX_OUTWIN_W;	/* Text Area Width */
	taH = BOX_OUTWIN_H;		/* Text Area Height */
	taX = BOX_OUTWIN_START_X;	/* Text Area X pos */
	taY = BOX_OUTWIN_START_Y;	/* Text Area Y pos */
	tX = OUTWIN_TEXT_START_X;	/* Text X pos */
	tY = OUTWIN_TEXT_START_Y;	/* Text Y pos */
	
	DrawBoxWithoutRefresh( _GRAY80, taX, taY, taW, taH );
//	DrawBoxWithoutRefresh( TEXTBOX_OUTLINE_SHADOW, taX+1, taY+1, taW-1, taH-2 );
	DrawBoxWithoutRefresh( _GRAY40, taX+taW-1, taY+1, 1, taH-2 );
	DrawBoxWithoutRefresh( _GRAY40, taX+1, taY+taH-1, taW-1, 1 );
	DrawBoxWithoutRefresh( _WHITE, taX+2, taY+2, taW-3, taH-3 );
}

void DicKeyboard::ClearTextArea()
{
	DrawBoxWithoutRefresh( _WHITE, taX+2, taY+2, taW-3, taH-3 );
}

void DicKeyboard::BuildCursor( int x )
{
	DrawBoxWithoutRefresh( _BLACK, x, tY-2, CURSOR_W, CURSOR_H );
}

int DicKeyboard::Draw_Scroll_Start_Pos(int type)
{
	int TxtBxW = taW-24-10;
	int TxtW, PreW, SufW, Char_size=0;
	int scroll_sx;

	if(type == 1)		 /* Case : Normal mode */
		TxtW = pT->GetStringWidth(m_DisplayText_Pre,12,DT_TRANSPARENCY);
	else if(type == 2) /* Case : Korean mode */
		TxtW = pT->GetStringWidth(m_kor_ks_text,12,DT_TRANSPARENCY);
	

	if( TxtBxW > TxtW )
	{
		//pT->DrawScrollText(0);
		BuildCursor(etx);
	}
	else
	{
		pT->DrawScrollText((scroll_sx=TxtW-TxtBxW));
		etx -= scroll_sx;
		BuildCursor(etx);
	}

	return 0;


}
int DicKeyboard::DrawText_Cursor(int mode)
{
	if(m_BackToKeyboard)
	{
		ClearTextArea();
		fprintf(stderr,"DicKeyboard : DrawText_Cursor : BackToKeyboard\n");
		if(!m_CheckShiftStart)
		{
			str2ustr( m_DisplayText_Pre, m_Key_Buffer_Pre, strlen(m_Key_Buffer_Pre) );
			etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
			Draw_Scroll_Start_Pos(1);
		}
		else
		{
			str2ustr( m_DisplayText_Pre, m_shift_manager.m_pre_buffer, strlen(m_shift_manager.m_pre_buffer) );
			etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);
			Draw_Scroll_Start_Pos(1);
			str2ustr( m_DisplayText_Pre, m_shift_manager.m_suf_buffer, strlen(m_shift_manager.m_suf_buffer) );
			pT->DrawUC16Text( m_DisplayText_Pre, etx+4, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);
		}
		m_Time_Proc =		false;
		m_BackToKeyboard = 	false;
		m_PassByKeyPress = 	false;
		m_PassByKeyShift = 	false;	
		m_PassByTimer = 		false;
		m_isTimeRemain =		false;
		
		return 0;
	}

	if(m_isBackspace == true) /* Process to Backspace */
	{
		ClearTextArea();
		etx = 0;
		if(m_CheckShiftStart == false)
		{
			str2ustr( m_DisplayText_Pre, m_Key_Buffer_Pre, strlen(m_Key_Buffer_Pre) );
			etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
			Draw_Scroll_Start_Pos(1);		
			m_isBackspace = false;
		}
		else
		{
			str2ustr( m_DisplayText_Pre, m_shift_manager.m_pre_buffer, strlen(m_shift_manager.m_pre_buffer) );
			etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
			Draw_Scroll_Start_Pos(1);
			str2ustr( m_DisplayText_Suf, m_shift_manager.m_suf_buffer, strlen(m_shift_manager.m_suf_buffer) );
			pT->DrawUC16Text( m_DisplayText_Suf, etx+4, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);
			m_isBackspace = false;

		}
	}
	else if(m_isSpace == true) /* Process to Space */
	{
		etx = 0;
		ClearTextArea();
		if(m_CheckShiftStart == false)
		{
			str2ustr( m_DisplayText_Pre, m_Key_Buffer_Pre, strlen(m_Key_Buffer_Pre) );
			etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);				
			Draw_Scroll_Start_Pos(1);
			m_Key_Buffer[0] = '\0';
		}
		else
		{
			str2ustr( m_DisplayText_Pre, m_shift_manager.m_pre_buffer, strlen(m_shift_manager.m_pre_buffer) );
			etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
			Draw_Scroll_Start_Pos(1);	
			str2ustr( m_DisplayText_Suf, m_shift_manager.m_suf_buffer, strlen(m_shift_manager.m_suf_buffer) );
			pT->DrawUC16Text( m_DisplayText_Suf, etx+4, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);
		}
		m_isSpace = false;
	}
	else if(m_cursor_pos == '7' ) /* Process to mode & upper & lower */
	{
		switch(m_mode)
		{
			case 0 :
				//fprintf(stderr, "Current Mode = %s\n", "Korean");
				break;
			case 1 :
				//fprintf(stderr, "Current Mode = %s\n", "Eng Upper");
				break;
			case 11 :
				//fprintf(stderr, "Current Mode = %s\n", "Eng Lower");
				break;
			case 2 :
				//fprintf(stderr, "Current Mode = %s\n", "Digit");
				break;

		}
		
	}
	/* Process to text on the text window */
	else if(  !m_CheckShiftStart && ( m_PassByKeyPress ||m_PassByTimer || m_PassByKeyShift ) )
	{
		etx = 0;
		
		if( (m_Key_Buffer_Pre_Index <= 256) && m_Time_Proc)
		{
			ClearTextArea();	
			
			if((strlen(m_Key_Buffer_Pre) == 0 )) /* It's empty buffer */
			{	
				if(m_isToggle)
				{
					fprintf(stderr,"DicKeyboard : DrawText_Cursor : NoShift : EmptyBuffer :Toggle\n");
					BuildCursor(tX-3);
					m_Key_Buffer_Suf[0] = m_Key_Buffer[0];
					m_Key_Buffer_Suf[1] = '\0';									
					str2ustr( m_DisplayText_Suf, m_Key_Buffer_Suf, strlen(m_Key_Buffer_Suf) );
					etx = pT->DrawUC16Text( m_DisplayText_Suf , tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
				 }
				else
				{
					fprintf(stderr,"DicKeyboard : DrawText_Cursor : NoShift : EmptyBuffer :NoToggle\n");
					m_Key_Buffer_Pre[0] = m_Key_Buffer_Suf[0];
					m_Key_Buffer_Suf[0] = '\0';
					m_Key_Buffer_Pre[++m_Key_Buffer_Pre_Index] = '\0';
					str2ustr( m_DisplayText_Pre, m_Key_Buffer_Pre, strlen(m_Key_Buffer_Pre) );
					etx = pT->DrawUC16Text( m_DisplayText_Pre , tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
					Draw_Scroll_Start_Pos(1);
					m_Time_Proc = false;
				}
					
			 }
			 else	/*  There are things in the key buffer */
			 {
			 	if(m_isToggle)
			 	{
			 		fprintf(stderr,"DicKeyboard : DrawText_Cursor : NoShift : NoEmptyBuffer :Toggle\n");
					str2ustr( m_DisplayText_Pre, m_Key_Buffer_Pre, strlen(m_Key_Buffer_Pre) );
					etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
					Draw_Scroll_Start_Pos(1);
					m_Key_Buffer_Suf[0] = m_Key_Buffer[0];
					m_Key_Buffer_Suf[1] = '\0';
					str2ustr( m_DisplayText_Suf, m_Key_Buffer_Suf, strlen(m_Key_Buffer_Suf) );
					pT->DrawUC16Text( m_DisplayText_Suf , etx+4, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);
					
			 	}
				else
				{
					if(m_Key_Buffer[0] != '\0')
					{
						fprintf(stderr,"DicKeyboard : DrawText_Cursor : NoShift : NoEmptyBuffer :NoToggle\n");
						m_Key_Buffer_Pre[m_Key_Buffer_Pre_Index] = m_Key_Buffer[0];
						m_Key_Buffer_Pre[++m_Key_Buffer_Pre_Index] = '\0';
						str2ustr( m_DisplayText_Pre, m_Key_Buffer_Pre, strlen(m_Key_Buffer_Pre) );
						etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
						Draw_Scroll_Start_Pos(1);
					}
					else
					{
						str2ustr( m_DisplayText_Pre, m_Key_Buffer_Pre, strlen(m_Key_Buffer_Pre) );
						etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
						Draw_Scroll_Start_Pos(1);
					}	
					m_Time_Proc = false;
				}
			  }
			 
		}
		
		/* process to flags init */
		m_PassByKeyPress = 	false;
		m_PassByKeyShift = 	false;	
		m_PassByTimer = 		false;
		
	}
	else if(m_CheckShiftStart && (m_mode != DIC_MODE_KOR))
	{
		int TxtBxW = taW-24-10;
		int TxtW=0;

		TxtW = pT->GetStringWidth(m_Key_Buffer_Pre,12,DT_TRANSPARENCY);
	
		if(TxtBxW > TxtW)
		{
			if(m_isShiftToggle && (m_PassByKeyPress && m_isTimeRemain))
			{
				fprintf(stderr,"DicKeyboard : DrawText_Cursor : Shift : Toggle\n");
				char tmparray[2];
				ClearTextArea();
				/* Prefix */
				str2ustr( m_DisplayText_Pre, m_shift_manager.m_pre_buffer, strlen(m_shift_manager.m_pre_buffer) );
				etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
				/* Cursor */
				Draw_Scroll_Start_Pos(1);
				/* Center */
				tmparray[0] = m_Key_Buffer[0];
				tmparray[1] = '\0';
				str2ustr( m_DisplayText_Suf, tmparray, 1 );
				etx = pT->DrawUC16Text( m_DisplayText_Suf, etx+4, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);
				/* Sufix */
				str2ustr( m_DisplayText_Pre, m_shift_manager.m_suf_buffer, strlen(m_shift_manager.m_suf_buffer) );
				pT->DrawUC16Text( m_DisplayText_Pre, etx+2, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);

			}
		
			else if( (!m_isShiftToggle) && (!m_isTimeRemain) && (m_PassByKeyPress ||m_PassByKeyShift || m_PassByTimer))
			{		
				if(m_Key_Buffer[0] != '\0')
				{
					fprintf(stderr,"DicKeyboard : DrawText_Cursor : Shift : NoToggle\n");
					ClearTextArea();
					m_shift_manager.m_total_index += 1;
					strncpy(m_Key_Buffer_Pre, m_shift_manager.m_pre_buffer, m_shift_manager.m_pre_index );
					strncpy(m_Key_Buffer_Pre+m_shift_manager.m_pre_index, m_Key_Buffer, 1 );
					strncpy(m_Key_Buffer_Pre+m_shift_manager.m_pre_index+1, 
							m_shift_manager.m_suf_buffer, 
							m_shift_manager.m_suf_index );
					m_Key_Buffer_Pre[++m_Key_Buffer_Pre_Index] = '\0';
					m_shift_manager.m_pre_index += 1;
					strncpy(m_shift_manager.m_pre_buffer, 
							m_Key_Buffer_Pre,
							m_shift_manager.m_pre_index );
					m_shift_manager.m_pre_buffer[m_shift_manager.m_pre_index] = '\0';
					str2ustr( m_DisplayText_Pre, m_shift_manager.m_pre_buffer, strlen(m_shift_manager.m_pre_buffer) );
					etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
					Draw_Scroll_Start_Pos(1);
					str2ustr( m_DisplayText_Suf, m_shift_manager.m_suf_buffer, strlen(m_shift_manager.m_suf_buffer) );
					pT->DrawUC16Text( m_DisplayText_Suf, etx+4, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);
					
				}
				else
				{
					ClearTextArea();
					str2ustr( m_DisplayText_Pre, m_shift_manager.m_pre_buffer, strlen(m_shift_manager.m_pre_buffer) );
					etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
					Draw_Scroll_Start_Pos(1);
					str2ustr( m_DisplayText_Suf, m_shift_manager.m_suf_buffer, strlen(m_shift_manager.m_suf_buffer) );
					pT->DrawUC16Text( m_DisplayText_Suf, etx+4, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);
					
				}	
				m_Time_Proc =		false;
				m_isShiftToggle = 		true;
			}

			/* process to flags init */
			m_PassByKeyPress = 	false;
			m_PassByKeyShift = 	false;	
			m_PassByTimer = 		false;
		}
		else
		{
			m_Time_Proc =		false;
		}
		
	}

	m_PassByTimer = false;

	fprintf(stderr,"Total_idx = %d, Data = ", m_Key_Buffer_Pre_Index);
	for(int i =0; i<m_Key_Buffer_Pre_Index; i++)
		fprintf(stderr,"%c",m_Key_Buffer_Pre[i]);
	fprintf(stderr,"\n");
	
	return 0;

}

void DicKeyboard::BuildKeysKor()
{
	int kx = BOX_KEY_START_X;
	int ky = BOX_KEY_START_Y;
	int row, col, isSelected;
	int NumPadSpace;
	int is_ENTER_Drawed = 0;
	int is_CAPSLOCK_Drawed = 0;
	bool skip_backspace = false;
	bool skip_space = false;

	for( row=0; row<=Vk->MaxRow; row++ )
	{


		for( col=0; col<=Vk->MaxCol; col++ )
		{

			if( kRow == row && kCol == col ) 
				isSelected = TRUE;
			else 
				isSelected = FALSE;

			if( col >= 7 ) 
				NumPadSpace = 8;
			else 
				NumPadSpace = 0;
			
			if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'a' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㄱㅋㄲ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'b' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㄴ ㄹ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'c' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㄷㅌㄸ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected , DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '1' )
			{
				sprintf( KeyPrintfBuf, "%s", "←" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected ,DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '2' )
			{
				sprintf( KeyPrintfBuf, "%s", "→" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected , DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'd' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㅁㅅㅆ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'e' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㅗ ㅛ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'f' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㅂㅍㅃ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '3' )
			{
				sprintf( KeyPrintfBuf, "%s", "|←" );
				if(!skip_backspace)
				{
					if(isSelected)
					{
						skip_backspace = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_backspace = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
				//BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '4' )
			{
				sprintf( KeyPrintfBuf, "%s", "|←" );
				if(!skip_backspace)
				{
					if(isSelected)
					{
						skip_backspace = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_backspace = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
				//BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
		//	else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '4' )
		//	{
		//		sprintf( KeyPrintfBuf, "%s", "↑" );
		//		BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
		//	}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'g' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㅓ ㅕ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'h' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㅣ ㅡ ㅢ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'i' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㅏ ㅑ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '5' )
			{
				sprintf( KeyPrintfBuf, "%s", "Space" );
				if(!skip_space)
				{
					if(isSelected)
					{
						skip_space = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_space = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '6' )
			{
				sprintf( KeyPrintfBuf, "%s", "Space" );
				if(!skip_space)
				{
					if(isSelected)
					{
						skip_space = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_space = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'j' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㅈㅊㅉ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'k' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㅜ ㅠ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'l' )
			{
				sprintf( KeyPrintfBuf, "%s", "ㅇ ㅎ" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '7' )
			{
				sprintf( KeyPrintfBuf, "%s", "Lang" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '8' )
			{
				sprintf( KeyPrintfBuf, "%s", "OK" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}

			
		}

	}
	
}

void DicKeyboard::BuildKeysEngUp()
{
	int kx = BOX_KEY_START_X;
	int ky = BOX_KEY_START_Y;
	int row, col, isSelected;
	int NumPadSpace;
	int is_ENTER_Drawed = 0;
	int is_CAPSLOCK_Drawed = 0;
	bool skip_backspace = false;
	bool skip_space = false;

	for( row=0; row<=Vk->MaxRow; row++ )
	{


		for( col=0; col<=Vk->MaxCol; col++ )
		{
			if( kRow == row && kCol == col ) 
				isSelected = TRUE;
			else 
				isSelected = FALSE;

			if( col >= 7 ) 
				NumPadSpace = 8;
			else 
				NumPadSpace = 0;
			
			if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'a' )
			{
				sprintf( KeyPrintfBuf, "%s", "Q Z" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'b' )
			{
				sprintf( KeyPrintfBuf, "%s", "ABC" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'c' )
			{
				sprintf( KeyPrintfBuf, "%s", "DEF" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected , DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '1' )
			{
				sprintf( KeyPrintfBuf, "%s", "←" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected ,DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '2' )
			{
				sprintf( KeyPrintfBuf, "%s", "→" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected , DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'd' )
			{
				sprintf( KeyPrintfBuf, "%s", "GHI" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'e' )
			{
				sprintf( KeyPrintfBuf, "%s", "JKL" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'f' )
			{
				sprintf( KeyPrintfBuf, "%s", "MNO" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '3' )
			{
				sprintf( KeyPrintfBuf, "%s", "|←" );
				if(!skip_backspace)
				{
					if(isSelected)
					{
						skip_backspace = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_backspace = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
				//BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '4' )
			{
				sprintf( KeyPrintfBuf, "%s", "|←" );
				if(!skip_backspace)
				{
					if(isSelected)
					{
						skip_backspace = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_backspace = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
				//BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'g' )
			{
				sprintf( KeyPrintfBuf, "%s", "PRS" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'h' )
			{
				sprintf( KeyPrintfBuf, "%s", "TUV" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'i' )
			{
				sprintf( KeyPrintfBuf, "%s", "WXY" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '5' )
			{
				sprintf( KeyPrintfBuf, "%s", "Space" );
				if(!skip_space)
				{
					if(isSelected)
					{
						skip_space = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_space = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '6' )
			{
				sprintf( KeyPrintfBuf, "%s", "Space" );
				if(!skip_space)
				{
					if(isSelected)
					{
						skip_space = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_space = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'j' )
			{
				sprintf( KeyPrintfBuf, "%s", "     " );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'k' )
			{
				sprintf( KeyPrintfBuf, "%s", "Lower" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'l' )
			{
				sprintf( KeyPrintfBuf, "%s", "     " );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '7' )
			{
				sprintf( KeyPrintfBuf, "%s", "Lang" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '8' )
			{
				sprintf( KeyPrintfBuf, "%s", "OK" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}

			
		}


	}

	
	
}

void DicKeyboard::BuildKeysEngLo()
{
	int kx = BOX_KEY_START_X;
	int ky = BOX_KEY_START_Y;
	int row, col, isSelected;
	int NumPadSpace;
	int is_ENTER_Drawed = 0;
	int is_CAPSLOCK_Drawed = 0;
	bool skip_backspace = false;
	bool skip_space = false;

	for( row=0; row<=Vk->MaxRow; row++ )
	{


		for( col=0; col<=Vk->MaxCol; col++ )
		{

			if( kRow == row && kCol == col ) 
				isSelected = TRUE;
			else 
				isSelected = FALSE;

			if( col >= 7 ) 
				NumPadSpace = 8;
			else 
				NumPadSpace = 0;
			
			if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'a' )
			{
				sprintf( KeyPrintfBuf, "%s", "q z" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'b' )
			{
				sprintf( KeyPrintfBuf, "%s", "abc" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'c' )
			{
				sprintf( KeyPrintfBuf, "%s", "def" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected , DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '1' )
			{
				sprintf( KeyPrintfBuf, "%s", "←" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected ,DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '2' )
			{
				sprintf( KeyPrintfBuf, "%s", "→" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected , DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'd' )
			{
				sprintf( KeyPrintfBuf, "%s", "ghi" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'e' )
			{
				sprintf( KeyPrintfBuf, "%s", "jkl" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'f' )
			{
				sprintf( KeyPrintfBuf, "%s", "mno" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '3' )
			{
				sprintf( KeyPrintfBuf, "%s", "|←" );
				if(!skip_backspace)
				{
					if(isSelected)
					{
						skip_backspace = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_backspace = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
				//BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '4' )
			{
				sprintf( KeyPrintfBuf, "%s", "|←" );
				if(!skip_backspace)
				{
					if(isSelected)
					{
						skip_backspace = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_backspace = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
				//BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'g' )
			{
				sprintf( KeyPrintfBuf, "%s", "prs" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'h' )
			{
				sprintf( KeyPrintfBuf, "%s", "tuv" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'i' )
			{
				sprintf( KeyPrintfBuf, "%s", "wxy" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '5' )
			{
				sprintf( KeyPrintfBuf, "%s", "Space" );
				if(!skip_space)
				{
					if(isSelected)
					{
						skip_space = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_space = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '6' )
			{
				sprintf( KeyPrintfBuf, "%s", "Space" );
				if(!skip_space)
				{
					if(isSelected)
					{
						skip_space = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_space = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'j' )
			{
				sprintf( KeyPrintfBuf, "%s", "     " );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'k' )
			{
				sprintf( KeyPrintfBuf, "%s", "Upper" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'l' )
			{
				sprintf( KeyPrintfBuf, "%s", "     " );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '7' )
			{
				sprintf( KeyPrintfBuf, "%s", "Lang" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '8' )
			{
				sprintf( KeyPrintfBuf, "%s", "OK" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}

			
		}


	}

	
	
}
void DicKeyboard::BuildKeysDigit()
{
	int kx = BOX_KEY_START_X;
	int ky = BOX_KEY_START_Y;
	int row, col, isSelected;
	int NumPadSpace;
	int is_ENTER_Drawed = 0;
	int is_CAPSLOCK_Drawed = 0;
	bool skip_backspace = false;
	bool skip_space = false;

	for( row=0; row<=Vk->MaxRow; row++ )
	{
		for( col=0; col<=Vk->MaxCol; col++ )
		{

			if( kRow == row && kCol == col ) 
				isSelected = TRUE;
			else 
				isSelected = FALSE;

			if( col >= 7 ) 
				NumPadSpace = 8;
			else 
				NumPadSpace = 0;
			
			if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'a' )
			{
				sprintf( KeyPrintfBuf, "%s", "1" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'b' )
			{
				sprintf( KeyPrintfBuf, "%s", "2" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'c' )
			{
				sprintf( KeyPrintfBuf, "%s", "3" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected , DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '1' )
			{
				sprintf( KeyPrintfBuf, "%s", "←" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected ,DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '2' )
			{
				sprintf( KeyPrintfBuf, "%s", "→" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected , DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'd' )
			{
				sprintf( KeyPrintfBuf, "%s", "4" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'e' )
			{
				sprintf( KeyPrintfBuf, "%s", "5" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'f' )
			{
				sprintf( KeyPrintfBuf, "%s", "6" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '3' )
			{
				sprintf( KeyPrintfBuf, "%s", "|←" );
				if(!skip_backspace)
				{
					if(isSelected)
					{
						skip_backspace = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_backspace = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
				//BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '4' )
			{
				sprintf( KeyPrintfBuf, "%s", "|←" );
				if(!skip_backspace)
				{
					if(isSelected)
					{
						skip_backspace = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_backspace = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
				//BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'g' )
			{
				sprintf( KeyPrintfBuf, "%s", "7" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'h' )
			{
				sprintf( KeyPrintfBuf, "%s", "8" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'i' )
			{
				sprintf( KeyPrintfBuf, "%s", "9" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '5' )
			{
				sprintf( KeyPrintfBuf, "%s", "Space" );
				if(!skip_space)
				{
					if(isSelected)
					{
						skip_space = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_space = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '6' )
			{
				sprintf( KeyPrintfBuf, "%s", "Space" );
				if(!skip_space)
				{
					if(isSelected)
					{
						skip_space = true;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
					else
					{
						skip_space = false;
						Draw_ButtonBox(KeyPrintfBuf, kx+NumPadSpace+4+((col-1)*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W);
					}
				}
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'j' )
			{
				sprintf( KeyPrintfBuf, "%s", "     " );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'k' )
			{
				sprintf( KeyPrintfBuf, "%s", "0" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == 'l' )
			{
				sprintf( KeyPrintfBuf, "%s", "     " );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '7' )
			{
				sprintf( KeyPrintfBuf, "%s", "Lang" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}
			else if( Vk->Dic_Eng_KeyArray[row*(Vk->MaxCol+1)+col] == '8' )
			{
				sprintf( KeyPrintfBuf, "%s", "OK" );
				BuildSpecialKey( KeyPrintfBuf, kx+NumPadSpace+4+(col*(DIC_KEY_W+DIC_KEY_D)), ky+(row*(DIC_KEY_H+DIC_KEY_D)), isSelected, DIC_KEY_W );
			}

			
		}


	}

	
	
}

void DicKeyboard::Draw_Guide( void)
{

	int tw1, tw2, tw3, x1, x2, x3, x4;
	int AddSpace;
	int img_w = 24;
	int img_h = 14;
	int img_space = 6;
	int item_space = 11;
	int shift_val = BOX_OUTWIN_START_X;
	int btn_y_pos = m_Ypos + m_Height -img_h - 6;

	if(TaviSysParam.etc.language == 0) /* case English */ 
	{
		img_space = 4;
		item_space = 5;
	}
	
	tw1 = pT->GetStringWidth( (U32)ML_SEARCH, 12, 0 );
	tw2 = pT->GetStringWidth( (U32)ML_LANGUAGE, 12, 0 );
	tw3 = pT->GetStringWidth( (U32)ML_SELECT, 12, 0 );
	
	x1 = shift_val + img_w + img_space;
	x2 = x1 + tw1 + item_space + img_w + img_space;
	x3 = x2 + tw2 + item_space + img_w + img_space;
	x4 = x3 + tw3 + item_space + img_w + img_space;
	/* draw ok  */
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 3, shift_val, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_SEARCH, x1, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );			
	/* draw language */	
	AddSpace=(x1 + tw1 + item_space);
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 5, AddSpace, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_LANGUAGE, x2, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );
	/* draw select */	
	AddSpace=(x2 + tw2 + item_space);
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 0, AddSpace, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_SELECT, x3, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );
	/* draw cancel */	
	AddSpace=(x3 + tw3 + item_space);
	DrawItemWithoutRefreshSkipTransparency( &TAVI_IMAGES[ICON_DBXICON], 2, AddSpace, btn_y_pos, img_w, img_h );
	pT->DrawResText( ML_CANCEL, x4, btn_y_pos+2, _WHITE, 12, DT_TRANSPARENCY );			
			
	pMpegDecoder->RefreshOsd();

}
void DicKeyboard::SetKBD( int dic_mode )
{
	switch(dic_mode)
	{
		case 0:
			m_mode = DIC_MODE_KOR;
			m_Key_Buffer[0] = '\0';
			break;
		
		case 1:
			m_mode = DIC_MODE_ENGU;
			m_isChangeKeyboard = true;
			m_Key_Buffer[0] = '\0';
			break;

		case 11:
			m_mode = DIC_MODE_ENGL;
			m_isChangeKeyboard = true;
			m_Key_Buffer[0] = '\0';
			break;

		case 2:
			m_mode = DIC_MODE_DIGIT;
			m_isChangeKeyboard = true;
			m_Key_Buffer[0] = '\0';
			break;
			
		default :
			break;
	}

}
#if 0
int DicKeyboard::Shift_Scroll_Proc(void)
{
	int TxtBxW = taW-24-10;
	int TxtW, PreW, SufW, Char_size=0;
	int scroll_sx;
	
	
	PreW = pT->GetStringWidth(m_shift_manager.m_pre_buffer,12,DT_TRANSPARENCY);
	Char_size = PreW -m_shift_manager.m_pre_width;
	m_shift_manager.m_pre_width = PreW;
	if(abs(Char_size)>20)
		Char_size = 0;
	SufW = pT->GetStringWidth(m_shift_manager.m_suf_buffer,12,DT_TRANSPARENCY);
	m_shift_manager.m_suf_width = SufW;
	TxtW = PreW + SufW;
	fprintf(stderr,"TxtW=%d, PreW=%d, SufW=%d, Size=%d\n",TxtW, PreW, SufW, Char_size);

	return 0;
}
#endif
int DicKeyboard::Cursor_LShift()
{
	
	ClearTextArea();
	
	m_shift_manager.m_pre_index -= 1;
	m_shift_manager.m_suf_index += 1;
	strncpy(m_shift_manager.m_pre_buffer,
			m_Key_Buffer_Pre,
			m_shift_manager.m_pre_index );
	m_shift_manager.m_pre_buffer[m_shift_manager.m_pre_index] = '\0';
	str2ustr( m_DisplayText_Pre, m_shift_manager.m_pre_buffer, strlen(m_shift_manager.m_pre_buffer) );
	etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);				
	BuildCursor(etx);
	strncpy(m_shift_manager.m_suf_buffer,
				m_Key_Buffer_Pre+m_shift_manager.m_pre_index,
				m_shift_manager.m_total_index -m_shift_manager.m_pre_index );
	m_shift_manager.m_suf_buffer[m_shift_manager.m_total_index -m_shift_manager.m_pre_index] = '\0';
	str2ustr( m_DisplayText_Suf, 
				m_shift_manager.m_suf_buffer, 
				strlen(m_shift_manager.m_suf_buffer) - m_shift_manager.m_suf_hide);
	pT->DrawUC16Text( m_DisplayText_Suf, etx+4, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);

	return 0;
}
int DicKeyboard::Cursor_RShift()
{
	ClearTextArea();
	
	m_shift_manager.m_pre_index += 1;
	m_shift_manager.m_suf_index -= 1;
	strncpy(m_shift_manager.m_pre_buffer,
			m_Key_Buffer_Pre,
			m_shift_manager.m_pre_index );
	m_shift_manager.m_pre_buffer[m_shift_manager.m_pre_index] = '\0';
	str2ustr( m_DisplayText_Pre, m_shift_manager.m_pre_buffer, strlen(m_shift_manager.m_pre_buffer) );
	etx = pT->DrawUC16Text( m_DisplayText_Pre, tX, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);
	BuildCursor(etx);
	strncpy(m_shift_manager.m_suf_buffer,
			m_Key_Buffer_Pre+m_shift_manager.m_pre_index,
			m_shift_manager.m_total_index -m_shift_manager.m_pre_index );
	m_shift_manager.m_suf_buffer[m_shift_manager.m_total_index -m_shift_manager.m_pre_index] = '\0';
	str2ustr( m_DisplayText_Suf, m_shift_manager.m_suf_buffer, strlen(m_shift_manager.m_suf_buffer) );
	pT->DrawUC16Text( m_DisplayText_Suf, etx+4, tY, _BLACK, _WHITE, 12, DT_TRANSPARENCY);

	return 0;
}
int DicKeyboard::CursorLongRight()
{	
	if( (('a' <= m_cursor_pos) && ('i' >= m_cursor_pos) && (m_mode == 1 ||m_mode == 11) && (m_Key_Buffer_Suf[0] != '\0') )
		|| ((m_mode == 2) && (m_cursor_pos != 'j')  &&  (m_cursor_pos != 'l') && (m_Key_Buffer_Suf[0] != '\0')) )
	{
		m_isToggle = false;
		m_PassByTimer = true;
		m_Time_Proc = false;
		
	}
	
	if( ((m_CheckShiftStart == true) && ('a' <= m_cursor_pos) && ('i' >= m_cursor_pos) && (m_mode == 1 ||m_mode == 11) && (m_Key_Buffer_Suf[0] != '\0') )
		|| ((m_CheckShiftStart == true) && (m_mode == 2) && (m_cursor_pos != 'j')  &&  (m_cursor_pos != 'l') && (m_Key_Buffer_Suf[0] != '\0')) )
	{
		
		m_isShiftToggle = false;
		m_Time_Proc = false;
		
	}
	
		return 0;
}
int DicKeyboard::CursorUp()
{
	char PreKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	char NextKey;

//	m_Key_Old = PreKey;
	
	if(m_CheckShiftStart && m_isShiftToggle && m_Time_Proc)
	{
		m_isShiftToggle = 			false;
		m_isTimeRemain =			false;
		m_PassByKeyShift =				true;
		m_PassByKeyPress =		false;
		m_PassByTimer =			false;
		
	}
	else if((m_CheckShiftStart == false)&&(m_isTimeRemain && m_isToggle))
	{
		m_isToggle = 				false;
		m_isShiftToggle = 			false;
		
		m_PassByKeyShift =		true; /* Can draw text */
	}
	
	if( kRow <= 0 ) kRow = Vk->MaxRow;
	else kRow--;
	
	NextKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	m_cursor_pos = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	
	for(int loop=0;loop<(Vk->MaxRow*2);loop++)
	{
		if( NextKey == ' ' || NextKey == '~' || NextKey == PreKey ) 
		{
			if( kRow <= 0 ) kRow = Vk->MaxRow;
			else kRow--;
			NextKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
		}
		else 
			break;
	}
	//fprintf(stderr, "CursorUp()\n");
	if(m_mode != DIC_MODE_KOR)
		Key_Buffer_Clear(4);	
	else 	
	{
		if(m_Key_Old != 'g' && m_Key_Old != 'i')
			Key_Buffer_Clear(4);
	}
}


int DicKeyboard::CursorDown()
{
	char PreKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	char NextKey;

//	m_Key_Old = PreKey;
	
	if(m_CheckShiftStart && m_isShiftToggle && m_Time_Proc)
	{
		m_isShiftToggle = 			false;
		m_isTimeRemain =			false;
		m_PassByKeyShift =				true;
		m_PassByKeyPress =		false;
		m_PassByTimer =			false;
		
	}
	else if((m_CheckShiftStart == false)&&(m_isTimeRemain && m_isToggle))
	{
		m_isToggle = 				false;
		m_isShiftToggle = 			false;
		
		m_PassByKeyShift =		true; /* Can draw text */
	}
	
	if( kRow >= Vk->MaxRow ) kRow = 0;
	else kRow++;

	NextKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	m_cursor_pos = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	
	for(int loop=0;loop<(Vk->MaxRow*2);loop++)
		{
		if( NextKey == ' ' || NextKey == '~' || NextKey == PreKey ) 
			{
			if( kRow >= Vk->MaxRow ) kRow = 0;
			else kRow++;
			NextKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
			}
		else 
			break;
		}
	//fprintf(stderr, "CursorDown()\n");
	if(m_mode != DIC_MODE_KOR)
		Key_Buffer_Clear(4);	
	else 	
	{
		if(m_Key_Old != 'g' && m_Key_Old != 'i')
			Key_Buffer_Clear(4);
	}
}

int DicKeyboard::CursorLeft()
{
	char PreKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	char CurKey;

//	m_Key_Old = PreKey;

	if(m_CheckShiftStart && m_isShiftToggle && m_Time_Proc)
	{
		m_isShiftToggle = 			false;
		m_isTimeRemain =			false;
		m_PassByKeyShift =				true;
		m_PassByKeyPress =		false;
		m_PassByTimer =			false;
		
	}
	else if((m_CheckShiftStart == false)&&(m_isTimeRemain && m_isToggle))
	{
		m_isToggle = 				false;
		m_isShiftToggle = 			false;
		
		m_PassByKeyShift =		true; /* Can draw text */
	}
	
	if( kCol <= 0 ) kCol = Vk->MaxCol;
	else
		kCol--;

	
	CurKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	m_cursor_pos = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	if(CurKey == '4' ||CurKey == '6' )
		kCol--;
		
	for(int loop=0;loop<(Vk->MaxCol*2);loop++)
		{
		if( CurKey == ' ' || CurKey == '~' || CurKey == PreKey ) 
			{
			if( kCol <= 0 ) kCol = Vk->MaxCol;
			else kCol--;
			CurKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
			}
		else 
			break;
		}
	//fprintf(stderr, "CursorLeft()\n");
	if(m_mode != DIC_MODE_KOR)
		Key_Buffer_Clear(4);	
	else 	
	{
		if(m_Key_Old != 'g' && m_Key_Old != 'i')
			Key_Buffer_Clear(4);
	}

}

int DicKeyboard::CursorRight()
{
	char PreKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	char CurKey;

//	m_Key_Old = PreKey;
	
	if(m_CheckShiftStart && m_isShiftToggle && m_Time_Proc)
	{
		m_isShiftToggle = 			false;
		m_isTimeRemain =			false;
		m_PassByKeyShift =				true;
		m_PassByKeyPress =		false;
		m_PassByTimer =			false;
		
	}
	else if((m_CheckShiftStart == false)&&(m_isTimeRemain && m_isToggle))
	{
		m_isToggle = 				false;
		m_isShiftToggle = 			false;
		
		m_PassByKeyShift =		true; /* Can draw text */
	}

	if( kCol >= Vk->MaxCol ) kCol = 0;
	else kCol++;

	CurKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	m_cursor_pos = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	if(CurKey == '3' ||CurKey == '5' )
		kCol++;
	
	for(int loop=0;loop<(Vk->MaxCol*2);loop++)
		{
		if( CurKey == ' ' || CurKey == '~' || CurKey == PreKey ) 
			{
			if( kCol >= Vk->MaxCol ) kCol = 0;
			else kCol++;
			CurKey = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
			}
		else 
			break;
		}
	//fprintf(stderr, "CursorRight()\n");
	if(m_mode != DIC_MODE_KOR)
		Key_Buffer_Clear(4);	
	else 	
	{
		if(m_Key_Old != 'g' && m_Key_Old != 'i')
			Key_Buffer_Clear(4);
	}
	
}

int DicKeyboard::CursorOK(int msg)
{
	char input ;
	
	if(msg == DIC_KEY_DELETE)
	{
		input = '3';
	}
	else
	{
		input = Vk->Dic_Eng_KeyArray[kRow*(Vk->MaxCol+1)+kCol];
	}
	
	m_Key_Buffer[1] = '\0';
	
	if( (m_mode != DIC_MODE_KOR) && ((input >= 'a')&&(input <= 'l')) )
	{
		if( (input != 'k') && (m_mode != DIC_MODE_DIGIT) )
		{
			m_CheckTimeStart  = true;

			Check_Time_Remain();		/* Start time */

			m_CheckTimeStart  = false;
		}
		else if( (m_mode == DIC_MODE_DIGIT)&&(input != 'j')&&(input != 'l') )
		{
			m_CheckTimeStart  = true;

			Check_Time_Remain();		/* Start time */

			m_CheckTimeStart  = false;
		}
	}
	else
		m_Time_Proc = false;
	
	/* Check Key Info */
	if( input == 'a' )	/* case (Q Z) , (q z) , (1) , (ㄱ ㅋ ㄲ) */
	{
		m_PassByKeyPress = true;
		m_PassByKorKeyPress = true;
		
		if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
		{
			m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
			if(m_mode == DIC_MODE_ENGU)
				m_Key_Buffer[0] = 'Q';
			else if(m_mode == DIC_MODE_ENGL)
				m_Key_Buffer[0] = 'q';
			else if(m_mode == DIC_MODE_DIGIT)
			{
				m_Key_Buffer[0] = '1';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'R';
		}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
		{
			if(m_mode == DIC_MODE_ENGU)
			{
				m_Key_Buffer[0] = 'Z';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_ENGL)
			{
				m_Key_Buffer[0] = 'z';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_KOR)
			{
				m_Key_Buffer[0] = 'Z';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
			}
		}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
		{
			m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'r';
		}

	}
	else if( input == 'b' )	/* case (A B C) , (a b c) , (2) , ( ㄴ ㄹ ) */
	{	
		m_PassByKeyPress = true;
		m_PassByKorKeyPress = true;
		
		if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
		{
			m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
			if(m_mode == DIC_MODE_ENGU)
				m_Key_Buffer[0] = 'A';
			else if(m_mode == DIC_MODE_ENGL)
				m_Key_Buffer[0] = 'a';
			else if(m_mode == DIC_MODE_DIGIT)
			{
				m_Key_Buffer[0] = '2';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'S';
		}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'B';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'b';
				else if(m_mode == DIC_MODE_KOR)
				{
					m_Key_Buffer[0] = 'F';
					m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				}
			}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'C';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'c';
				
			}
	}	
	else if( input == 'c' )	/* case (D E F) , (d e f) , (3) , (ㄷ ㅌ ㄸ) */
	{	
		m_PassByKeyPress = true;
		m_PassByKorKeyPress = true;
		
		if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
		{
			m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
			if(m_mode == DIC_MODE_ENGU)
				m_Key_Buffer[0] = 'D';
			else if(m_mode == DIC_MODE_ENGL)
				m_Key_Buffer[0] = 'd';
			else if(m_mode == DIC_MODE_DIGIT)
			{
				m_Key_Buffer[0] = '3';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'E';
		}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'E';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'e';
				else if(m_mode == DIC_MODE_KOR)
					m_Key_Buffer[0] = 'X';
			}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'F';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'f';
				else if(m_mode == DIC_MODE_KOR)
					m_Key_Buffer[0] = 'e';
			}
	}	
	else if( input == 'd' )	/* case (G H I) , (g h i) , (4) , (ㅁ ㅅ ㅆ) */
	{	
		m_PassByKeyPress = true;
		m_PassByKorKeyPress = true;
		
		if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
		{
			m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
			if(m_mode == DIC_MODE_ENGU)
				m_Key_Buffer[0] = 'G';
			else if(m_mode == DIC_MODE_ENGL)
				m_Key_Buffer[0] = 'g';
			else if(m_mode == DIC_MODE_DIGIT)
			{
				m_Key_Buffer[0] = '4';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'A';
		}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'H';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'h';
				else if(m_mode == DIC_MODE_KOR)
					m_Key_Buffer[0] = 'T';
			}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'I';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'i';
				else if(m_mode == DIC_MODE_KOR)
					m_Key_Buffer[0] = 't';
			}
	}	
	else if( input == 'e' )	/* case (J K L) , (j k l) , (5) , ( ㅗ ㅛ ) */
	{	
		m_PassByKeyPress = true;
		m_PassByKorKeyPress = true;
		
		if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
		{
			m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
			if(m_mode == DIC_MODE_ENGU)
				m_Key_Buffer[0] = 'J';
			else if(m_mode == DIC_MODE_ENGL)
				m_Key_Buffer[0] = 'j';
			else if(m_mode == DIC_MODE_DIGIT)
			{
				m_Key_Buffer[0] = '5';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'H';
		}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'K';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'k';
				else if(m_mode == DIC_MODE_KOR)
				{
					m_Key_Buffer[0] = 'Y';
					m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				}
			}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'L';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'l';
			}
	}	
	else if( input == 'f' ) /* case (M N O) , (m n o) , (6) , (ㅂ ㅍ ㅃ) */
	{	
		m_PassByKeyPress = true;
		m_PassByKorKeyPress = true;
		
		if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
		{
			m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
			if(m_mode == DIC_MODE_ENGU)
				m_Key_Buffer[0] = 'M';
			else if(m_mode == DIC_MODE_ENGL)
				m_Key_Buffer[0] = 'm';
			else if(m_mode == DIC_MODE_DIGIT)
			{
				m_Key_Buffer[0] = '6';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'Q';
		}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'N';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'n';
				else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'V';
			}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'O';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'o';
				else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'q';
			}
	}	
	else if( input == 'g' )	/* case (P R S) , (p r s) , (7) , (ㅓ ㅕ) */
	{	
		m_PassByKeyPress = true;
		m_PassByKorKeyPress = true;
		
		if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
		{
			m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
			if(m_mode == DIC_MODE_ENGU)
				m_Key_Buffer[0] = 'P';
			else if(m_mode == DIC_MODE_ENGL)
				m_Key_Buffer[0] = 'p';
			else if(m_mode == DIC_MODE_DIGIT)
			{
				m_Key_Buffer[0] = '7';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'J';
		}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'R';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'r';
				else if(m_mode == DIC_MODE_KOR)
				{
					m_Key_Buffer[0] = 'U';
					m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				}
			}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'S';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 's';
			}
	}	
	else if( input == 'h' )	/* case (T U V) , (t u v) , (8) , ( ㅣ ㅡ ㅢ) */
	{	
		m_PassByKeyPress = true;
		m_PassByKorKeyPress = true;
		
		if(m_mode == DIC_MODE_KOR && (m_Key_Old == 'g' || m_Key_Old == 'i'))
		{
			if(m_Key_Old == 'g')
			{
				m_kor_ks_text[strlen(m_kor_ks_text) - 1] = '\0';
				vk_kor.COMBI_Han_Input(&m_kor_key_buf, 0x2400); //Back Space 문자 보냄

				if( m_Key_Pressed_Info[10] == 0) // 'ㅓ + ㅣ
					m_Key_Buffer[0] = 'p';
				else // 'ㅕ + ㅣ
					m_Key_Buffer[0] = 'P';
			}
			if(m_Key_Old == 'i')
			{
				m_kor_ks_text[strlen(m_kor_ks_text) - 1] = '\0';
				vk_kor.COMBI_Han_Input(&m_kor_key_buf, 0x2400); //Back Space 문자 보냄
				if( m_Key_Pressed_Info[12] == 0) // 'ㅏ + ㅣ
					m_Key_Buffer[0] = 'o';
				else // 'ㅑ + ㅣ
					m_Key_Buffer[0] = 'O';
			}
		}
		else
		{
			if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'T';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 't';
				else if(m_mode == DIC_MODE_DIGIT)
				{
					m_Key_Buffer[0] = '8';
					m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				}
				else if(m_mode == DIC_MODE_KOR)
					m_Key_Buffer[0] = 'L';
			}
			else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
				{
					m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
					if(m_mode == DIC_MODE_ENGU)
						m_Key_Buffer[0] = 'U';
					else if(m_mode == DIC_MODE_ENGL)
						m_Key_Buffer[0] = 'u';
					else if(m_mode == DIC_MODE_KOR)
					{
						m_Key_Buffer[0] = 'M';
						//m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
					}
				}
			else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
				{
					m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
					if(m_mode == DIC_MODE_ENGU)
						m_Key_Buffer[0] = 'V';
					else if(m_mode == DIC_MODE_ENGL)
						m_Key_Buffer[0] = 'v';
					else if(m_mode == DIC_MODE_KOR){
						//pT->DrawEUC8Text( "ㅢ", tX + 100, tY, _BLACK, _WHITE, 16, DT_TRANSPARENCY|DT_TEXT_AREA);
						m_Key_Buffer[0] = 'u'; // 'ㅣ'
						m_Key_Buffer[1] = 't'; // 'ㅡ'
						sleep(1);
					}
				}
		}
	}

	else if( input == 'i' )	/* case (W X Y) , (w x y) , (9) , ( ㅏ ㅑ  ) */
	{	
		m_PassByKeyPress = true;
		m_PassByKorKeyPress = true;
		
		if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
		{
			m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
			if(m_mode == DIC_MODE_ENGU)
				m_Key_Buffer[0] = 'W';
			else if(m_mode == DIC_MODE_ENGL)
				m_Key_Buffer[0] = 'w';
			else if(m_mode == DIC_MODE_DIGIT)
			{
				m_Key_Buffer[0] = '9';
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
			}
			else if(m_mode == DIC_MODE_KOR)
				m_Key_Buffer[0] = 'K';
		}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'X';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'x';
				else if(m_mode == DIC_MODE_KOR)
				{
					m_Key_Buffer[0] = 'I';
					m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				}
			}
		else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				if(m_mode == DIC_MODE_ENGU)
					m_Key_Buffer[0] = 'Y';
				else if(m_mode == DIC_MODE_ENGL)
					m_Key_Buffer[0] = 'y';
				
			}
	}
	else if( input == 'j' )	/* case ( ) , ( ) , ( ) , ( ㅈ ㅊ ㅉ ) */
	{	
		if(m_mode == DIC_MODE_KOR)
		{
			m_PassByKorKeyPress = true;
			
			if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
				if(m_mode == DIC_MODE_KOR)
					m_Key_Buffer[0] = 'W';
			}
			else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
				{
					m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 2;
					if(m_mode == DIC_MODE_KOR)
					m_Key_Buffer[0] = 'C';
				}
			else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 2)
				{
					m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
					if(m_mode == DIC_MODE_KOR)
					m_Key_Buffer[0] = 'w';
				}
		}
		else
		{
			m_Key_Buffer[0] = '\0';
			m_Key_Buffer_Suf[0] = '\0';

		}
	}
	else if( input == 'k' ) /* Case change to Upper or Lower  &  Digit 0 */
	{	
		if(m_mode == DIC_MODE_KOR)	/* case  ( ㅜ ㅠ ) */
		{
			m_PassByKorKeyPress = true;
			
			if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
				m_Key_Buffer[0] = 'N';
			}
			else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				m_Key_Buffer[0] = 'B';
			}

		}
		/* Case Lower */
		else if(m_mode == DIC_MODE_ENGU)
		{
			m_PassByKeyPress = false;
			m_Key_Buffer[0] = '\0';
			SetKBD(DIC_MODE_ENGL);
		}
		/* Case Upper */
		else if(m_mode == DIC_MODE_ENGL)
		{
			m_PassByKeyPress = false;
			m_Key_Buffer[0] = '\0';
			SetKBD(DIC_MODE_ENGU);
		}
		/* Case Digit 0 */
		else if(m_mode == DIC_MODE_DIGIT)
		{
			m_Key_Buffer[0] = '0';
			m_PassByKeyPress = true;
		}
	}
	else if( input == 'l' )	/* case ( ) , ( ) , ( ) , ( ㅇ ㅎ ) */
	{	
		if(m_mode == DIC_MODE_KOR)
		{
			m_PassByKorKeyPress = true;
			
			if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 0)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 1;
				m_Key_Buffer[0] = 'D';
			}
			else if(m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] == 1)
			{
				m_Key_Pressed_Info[kRow*(Vk->MaxCol+1)+kCol] = 0;
				m_Key_Buffer[0] = 'G';
			}
		}
		else
		{
			m_Key_Buffer[0] = '\0';
			m_Key_Buffer_Suf[0] = '\0';
		}
		
	}


	/* Case Cursor Left shift */
	if(input == '1')
	{
		int ret;
		int TxtBxW = taW-24-10;
		int TxtW=0;

		TxtW = pT->GetStringWidth(m_Key_Buffer_Pre,12,DT_TRANSPARENCY);
	
		//if(TxtBxW > TxtW)
		//{
			if(m_mode != DIC_MODE_KOR)		
			{
				
				m_Key_Buffer[0] = '\0';
				m_Key_Buffer_Suf[0] = '\0';
				
				if(m_shift_manager.m_total_index == m_shift_manager.m_pre_index)
				{
					m_shift_manager.m_total_index = m_Key_Buffer_Pre_Index;
					m_shift_manager.m_pre_index = m_Key_Buffer_Pre_Index;
				}
				
				if(m_shift_manager.m_pre_index > 0)
				{
					m_CheckShiftStart = true;
					
					ret = Cursor_LShift();
				}
				fprintf(stderr,"L-Shitf : total=%d, pre=%d, suf=%d\n",
					m_shift_manager.m_total_index,
					m_shift_manager.m_pre_index,
					m_shift_manager.m_suf_index);
				
			}
		//}
		//else
		//{
		//	fprintf(stderr,"DicKeyboard : CursorOK : 1 :TextBox width=%d, Text width=%d\n", TxtBxW, TxtBxW);
		//}
		
		
		
	}
	/* Case Cursor Right shift */
	else if(input  == '2')
	{
		int ret;
		int TxtBxW = taW-24-10;
		int TxtW=0;

		TxtW = pT->GetStringWidth(m_Key_Buffer_Pre,12,DT_TRANSPARENCY);
		
		if(m_mode != DIC_MODE_KOR)	
		{
			//if(TxtBxW > TxtW)
			//{
				m_Key_Buffer[0] = '\0';
				m_Key_Buffer_Suf[0] = '\0';
				
				if(m_shift_manager.m_suf_index > 0)
				{	
					m_CheckShiftStart = true;
					
					ret = Cursor_RShift();
				}
				fprintf(stderr,"R-Shift : total=%d, pre=%d, suf=%d\n",
					m_shift_manager.m_total_index,
					m_shift_manager.m_pre_index,
					m_shift_manager.m_suf_index);
			//}
			//else
			//{
			//	fprintf(stderr,"DicKeyboard : CursorOK : 2 :TextBox width=%d, Text width=%d\n", TxtBxW, TxtBxW);
			//}
		}
		else		
		{			
			vk_kor.COMBI_Han_Input(&m_kor_key_buf, 0x2424); //Next : Right
		}
		
	}
	/* Case to use Back Space */
	else if(input  == '3' || input  == '4')
	{
		if(m_mode != DIC_MODE_KOR)
		{
			if(m_CheckShiftStart == false)
			{
				if(m_Key_Buffer_Pre_Index>0)
				{
					--m_Key_Buffer_Pre_Index;
					m_Key_Buffer_Pre[m_Key_Buffer_Pre_Index] = '\0';
				}
				
				m_isBackspace = true;
			}
			else
			{
				if(m_shift_manager.m_pre_index>0)
				{
					strncpy(m_shift_manager.m_pre_buffer,
							m_Key_Buffer_Pre,
							m_shift_manager.m_pre_index-1);
					m_shift_manager.m_pre_buffer[m_shift_manager.m_pre_index-1] ='\0';
					strncpy(m_Key_Buffer_Pre+m_shift_manager.m_pre_index-1, 
							m_shift_manager.m_suf_buffer,
							m_shift_manager.m_suf_index);
					m_Key_Buffer_Pre[m_Key_Buffer_Pre_Index-1] = '\0';
					
					m_shift_manager.m_total_index -= 1;
					m_shift_manager.m_pre_index -= 1;
					m_Key_Buffer_Pre_Index--;
					
					m_isBackspace = true;
				}
			}
		}
		else
		{
			Key_Buffer_Clear(4);
			m_Key_Buffer[0] = '\0';
			m_PassByKorKeyPress = 1;
			m_kor_ks_text[strlen(m_kor_ks_text) - 1] = '\0';
			vk_kor.COMBI_Han_Input(&m_kor_key_buf, 0x2400); //Back Space 문자 보냄
		}
	}

	/* Case space */
	else if(input  == '5' || input  == '6')
	{	
		if(m_mode != DIC_MODE_KOR)		
		{
			
			if(m_CheckShiftStart == false)
			{
				m_Key_Buffer_Pre[m_Key_Buffer_Pre_Index++] = ' ';
				m_Key_Buffer_Pre[m_Key_Buffer_Pre_Index] = '\0';
				m_isSpace = true;
			}
			else
			{
				m_shift_manager.m_pre_buffer[m_shift_manager.m_pre_index++] = ' ';
				m_shift_manager.m_pre_buffer[m_shift_manager.m_pre_index] = '\0'; 
				m_shift_manager.m_total_index++;

				strncpy(m_Key_Buffer_Pre, m_shift_manager.m_pre_buffer, m_shift_manager.m_pre_index);
				strncpy(m_Key_Buffer_Pre+m_shift_manager.m_pre_index,
						m_shift_manager.m_suf_buffer, 
						m_shift_manager.m_suf_index);
				
				m_Key_Buffer_Pre[++m_Key_Buffer_Pre_Index]='\0';
				m_isSpace = true;
			}
		}
		else
		{
			vk_kor.COMBI_Han_Input(&m_kor_key_buf, 0x2020); //Space 문자 보냄
		}
	}
	/* Case change to other mode */
	else if(input  == '7')
	{
		
		if(m_mode == DIC_MODE_KOR)
		{
			m_mode = DIC_MODE_ENGU;
			m_BackToKeyboard = true;
			m_Key_Buffer[0] = '\0';
			_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGU;
		}
		else if( m_mode == DIC_MODE_ENGU)
		{
			m_mode = DIC_MODE_ENGL;
			m_BackToKeyboard = true;
			m_Key_Buffer[0] = '\0';
			_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_ENGL;
		}
		else if(m_mode == DIC_MODE_ENGL )
		{
			m_mode = DIC_MODE_DIGIT;
			m_BackToKeyboard = true;
			m_Key_Buffer[0] = '\0';
			_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_DGT;
		}
		else if( m_mode == DIC_MODE_DIGIT)
		{
			m_mode = DIC_MODE_KOR;
			m_PassByKorKeyPress = true;
			m_Key_Buffer[0] = '\0';
			_ExDicPro.m_dic_cur_state = DIC_STATE_KEYBOARD_KOR;
		}
	}
	/* Case OK or Search */
	else if(input  == '8')
	{		
		switch(m_mode)
		{
			case DIC_MODE_KOR : GoSearch(DIC_MODE_KOR); return 0;		
			case DIC_MODE_ENGU : GoSearch(DIC_MODE_ENGU); return 0;
			case DIC_MODE_ENGL : GoSearch(DIC_MODE_ENGL); return 0;
			case DIC_MODE_DIGIT : GoSearch(DIC_MODE_DIGIT); return 0;
			default : 
				break;
		}		
	}
	
	if((m_mode == DIC_MODE_KOR) && (input  != '7'))
	{
		if(m_Key_Old == input) //같은 위치의 다른 문자 처리시
		{
			printf("타임아웃이 아닙니다[%x]\n", m_Key_Buffer[0]);
			if(input >= 'a' && input <= 'l')
			{
				//m_Key_Buffer[0] = '\0';
				m_PassByKorKeyPress = 1;
				m_kor_ks_text[strlen(m_kor_ks_text) - 1] = '\0';
				vk_kor.COMBI_Han_Input(&m_kor_key_buf, 0x2400); //Back Space 문자 보냄
			}
		}
		else
			m_Key_Old = input;
	}
	else
		m_Key_Old = input;
	
	ReDraw();
	m_PassByKeyPress = 		false;
	m_PassByKorKeyPress = 	false;
	
	return 0;
}
int DicKeyboard::GoSearch(int mode)
{

	fprintf(stderr,"DicKeyboard : GoSearch : \n");

	if( !m_CheckShiftStart )
	{
		if(m_Time_Proc)
		{
			m_Key_Buffer_Pre[m_Key_Buffer_Pre_Index] = m_Key_Buffer[0];
			m_Key_Buffer_Pre[++m_Key_Buffer_Pre_Index] = '\0';
			m_Time_Proc = false;
			//fprintf(stderr,"m_Key_Buffer_Pre = [%s]\n",m_Key_Buffer_Pre);
		}
	}
	else
	{
		m_CheckShiftStart = false;
		char TmpBuff[1024];
		memset(TmpBuff,0x00,1024);
		strncpy(TmpBuff,m_Key_Buffer_Pre,m_shift_manager.m_pre_index);
		//fprintf(stderr,"TmpBuff : pre : %s\n",TmpBuff);
		if(m_Time_Proc)
		{
			TmpBuff[m_shift_manager.m_pre_index] = m_Key_Buffer[0];
			//fprintf(stderr,"TmpBuff : pre+add : %s\n",TmpBuff);
			strncpy(TmpBuff+m_shift_manager.m_pre_index+1,
					m_Key_Buffer_Pre+m_shift_manager.m_pre_index,
					m_shift_manager.m_suf_index);
			m_Time_Proc = false;
			m_Key_Buffer_Pre_Index++;
			//fprintf(stderr,"DicKeyboard : GoSearch : m_CheckShiftStart : m_Time_Proc \n");
		}
		else
		{
			strncpy(TmpBuff+m_shift_manager.m_pre_index,
					m_Key_Buffer_Pre+m_shift_manager.m_pre_index,
					m_shift_manager.m_suf_index);
		}
		//fprintf(stderr,"TmpBuff : pre+add+suf : %s\n",TmpBuff);
		strcpy(m_Key_Buffer_Pre,TmpBuff);
		//fprintf(stderr,"m_Key_Buffer_Pre : %s\n",m_Key_Buffer_Pre);
			
	}

	memset(_ExDicPro.m_word, 0, 1024);
	_ExDicPro.m_go_search = true;
	m_Key_Buffer[0] = '\0';
	m_shift_manager.m_pre_width = 		0;
	m_shift_manager.m_suf_width = 		0;
	m_shift_manager.m_pre_hide = 		0;
	m_shift_manager.m_suf_hide = 		0;
	m_shift_manager.m_pre_index = 	0;
	m_shift_manager.m_suf_index = 		0;
	m_shift_manager.m_total_index = 	0;
	
	switch(mode)
	{
		case DIC_MODE_KOR :
			_ExDicPro.m_dic_pre_state = DIC_STATE_KEYBOARD_KOR;	
			memcpy(_ExDicPro.m_word, m_kor_ks_text, strlen(m_kor_ks_text));
			_ExDicPro.m_word[strlen(m_kor_ks_text)] = '\0';	
			break;
			
		case DIC_MODE_ENGU:
			if(strlen(m_Key_Buffer_Pre) != 0 && (m_Key_Buffer_Pre[0] != 0x20))
			{
				_ExDicPro.m_dic_pre_state = DIC_STATE_KEYBOARD_ENGU;
				strcpy(_ExDicPro.m_word, m_Key_Buffer_Pre);
				_ExDicPro.m_word[m_Key_Buffer_Pre_Index] = '\0';
				fprintf(stderr, "m_word = [%s]\n", _ExDicPro.m_word);
			}
			break;
			
		case DIC_MODE_ENGL:
			if(strlen(m_Key_Buffer_Pre) != 0 && (m_Key_Buffer_Pre[0] != 0x20))
			{
				_ExDicPro.m_dic_pre_state = DIC_STATE_KEYBOARD_ENGL;	
				strcpy(_ExDicPro.m_word, m_Key_Buffer_Pre);
				_ExDicPro.m_word[m_Key_Buffer_Pre_Index] = '\0';
				fprintf(stderr, "m_word = [%s]\n", _ExDicPro.m_word);
			}
			break;

		case DIC_MODE_DIGIT:
			if(strlen(m_Key_Buffer_Pre) != 0 && (m_Key_Buffer_Pre[0] != 0x20))
			{
				_ExDicPro.m_dic_pre_state = DIC_STATE_KEYBOARD_DGT;
				//sprintf(_ExDicPro.m_word, "%s", m_Key_Buffer_Pre);
				strcpy(_ExDicPro.m_word, m_Key_Buffer_Pre);
				_ExDicPro.m_word[m_Key_Buffer_Pre_Index] = '\0';
				fprintf(stderr, "m_word = [%s]\n", _ExDicPro.m_word);
			}
			break;

		default :
			break;
			
	}
	return 1;
}
void DicKeyboard::Exit()
{
	m_fExit = true;
	m_isToggle = false;
	m_isShiftToggle = false;
	m_Time_Proc = false;
	m_PassByTimer = false;

	fprintf(stderr,"DicKeyboard : Exit : \n");
	
	switch(m_mode)
	{
		case DIC_MODE_KOR :
			if(1)
			{
				_ExDicPro.m_dic_pre_state = DIC_STATE_KEYBOARD_KOR;
				_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
			}
			Key_Index_Clear(0);
			Key_Buffer_Clear(0);
			break;
			
		case DIC_MODE_ENGU : 
			_ExDicPro.m_dic_pre_state = DIC_STATE_KEYBOARD_ENGU;
			_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
			Key_Index_Clear(0);
			Key_Buffer_Clear(0);
			break;
			
		case DIC_MODE_ENGL : 
			_ExDicPro.m_dic_pre_state = DIC_STATE_KEYBOARD_ENGL;
			_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
			Key_Index_Clear(0);
			Key_Buffer_Clear(0);
			break;
			
		case DIC_MODE_DIGIT : 

			if(1)
			{
				_ExDicPro.m_dic_pre_state = DIC_STATE_KEYBOARD_DGT;
				_ExDicPro.m_dic_cur_state = DIC_STATE_OUTWIN;
			}
			Key_Index_Clear(0);
			Key_Buffer_Clear(0);
			break;
			
		default : 
			break;
	}

	return ;
	
}
void DicKeyboard::Show()
{
	Draw_KeyboardBox();
	BuildTextArea();
	Draw_Guide();
	if(m_mode == DIC_MODE_KOR)
		m_PassByKorKeyPress = 1;

	if(m_mode != DIC_MODE_KOR)
		m_BackToKeyboard = true;
	
	ReDraw();

	if(m_mode == DIC_MODE_KOR)
		m_PassByKorKeyPress = 0;
	
}

void DicKeyboard::ReDraw()
{
	
	if(m_mode != DIC_MODE_KOR)
		DrawText_Cursor(1);
	
	if(m_mode == DIC_MODE_KOR)
	{
		//printf("Redraw Kor Character\n");
		BuildKeysKor();
		if(m_PassByKorKeyPress)
			Kor_Keys_Proc();
	}
	else if( m_mode == DIC_MODE_ENGU)
		BuildKeysEngUp();
	else if( m_mode == DIC_MODE_ENGL)
		BuildKeysEngLo();
	else if( m_mode == DIC_MODE_DIGIT)
		BuildKeysDigit();
	
	pMpegDecoder->RefreshOsd();
}
void DicKeyboard::Kor_Keys_Proc(void)
{
	if(m_Key_Buffer[1] == '\0')
	{
		if((m_Key_Buffer[0] >= 0x61) && (m_Key_Buffer[0] <= 0x7a) )
		{
			vk_kor.isSHIFT = 1;
			m_Key_Buffer[0] -= 0x20;
		}
		else
		{
			vk_kor.isSHIFT = 0;
		}
	}	
	if(m_Key_Buffer[0] != '\0' && m_Key_Buffer[1] == '\0')
	{
		m_kor_code = vk_kor.COMBI_Key_to_HCODE(m_Key_Buffer[0]);
		vk_kor.COMBI_Han_Input(&m_kor_key_buf, m_kor_code);
	}
	else if(m_Key_Buffer[0] == 'u' && m_Key_Buffer[1] == 't')
		vk_kor.COMBI_Han_Input(&m_kor_key_buf, 0x8781); // 'ㅢ'
		
	memset(m_kor_text, 0x00 , 256);
	vk_kor.HAN_INPUT_KEY_BUFFER_COMBI_BufPrint(&m_kor_key_buf, m_kor_text);
	m_kor_len = strlen(m_kor_text);
	char temp[2];
	int dwstcount = 0;
	memset(m_kor_ks_text, 0x00, 256);
	
	for(int i = 0; m_kor_text[i] != 0x00; i++) 
	{
		temp[0] = m_kor_text[i];
		temp[1] = m_kor_text[i + 1];
		fprintf(stderr,"m_kor_text[%d] = %x, m_kor_text[%d] = %x\n", i, temp[0], i+1, temp[1]);
		if(vk_kor.COMBI_to_KS(&m_kor_ks_text[dwstcount], temp) == 1)
			dwstcount++;
		else 	
		{
			dwstcount += 2;
			i++;
		}
	}
	
	fprintf(stderr,"m_kor_ks_text = [%s]\n", m_kor_ks_text);
	
	strcpy(vk_kor.buffer, m_kor_ks_text);
	ClearTextArea();
	if(strlen(m_kor_ks_text) != 0)
	{
		etx = 0;
		etx = pT->DrawEUC8Text( m_kor_ks_text, tX, tY-1, _BLACK, _WHITE, 12, DT_TRANSPARENCY|DT_TEXT_AREA);
		Draw_Scroll_Start_Pos(2);
	}
	else
	{
		etx = tX-3;
		Draw_Scroll_Start_Pos(2);
	}

	return ;

}


