//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : List Manager Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/ListManager.h,v $
// $Revision: 1.2 $
// $Date: 2006/02/27 02:40:17 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: ListManager.h,v $
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.5  2006/02/22 04:16:28  akaiaphanu
// working selectbar, itemlist
//
// Revision 1.4  2006/02/21 05:04:46  akaiaphanu
// exception message : case not k-dic
//
// Revision 1.3  2006/02/21 02:04:44  akaiaphanu
// working title, list manager
//
// Revision 1.2  2006/02/20 08:06:47  akaiaphanu
// change to array of structure from double linked list
//
// Revision 1.1  2006/02/20 01:32:57  akaiaphanu
// add function for dictionary
//
//
//*****************************************************************************
#ifndef __LIST_MANAGER_H__
#define __LIST_MANAGER_H__

#include "CExtraDic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tavi_lib.h>

extern CTextEngine_BDF* pT;

#define LBOX_BASE_COLOR						24
#define LBOX_TITLE_COLOR						31
#define LBOX_SELECT_BAR_COLOR					27
#define LBOX_SELECT_BAR_SHADOW_COLOR		31

#define LM_MODE_PRE							-1
#define LM_MODE_CUR							0
#define LM_MODE_NXT							1

#define LIST_MAX_COUNT							1024

struct WNAME { 
  int 		Num; 
  char 	Name[256];
};

class ListManager 
{
	int 				m_Xpos;
	int				m_Ypos;
	int				m_Width;
	int				m_Height;
	int				m_SelectBar_Xpos;
	int				m_SelectBar_Ypos;
	int				m_SelectBar_Height;
	int				m_SelectBar_Width;
	int				m_ScrollBar_Xpos;
	int				m_ScrollBar_Ypos;
	int				m_ScrollBar_Height;
	int				m_ScrollBar_Width;
	int				m_Title_Xpos;
	int				m_Title_Ypos;
	int				m_Title_Height;
	int 				m_Text_Xpos;
	int 				m_Text_Ypos;
	CScrollBar3 *		m_pScrollbar3;
	
	WNAME			m_taWNameList[1024];
	int				m_WNameList_Idx;
	int				m_WCount;
	int				m_Cur_NamePos;
	int				m_Page;
	int				m_VisibleLine;
	int				m_UpCount;
	int				m_DownCount;
	bool				m_FirstDrawPage;
	int				m_SelectBar_Pos;
	int				m_Start_Pos;
	int				m_Draw_Pos_Idx;
	
public :
		
		ListManager(int, int, int, int);
		~ListManager();
int		LoadFromFile(void);
int		SaveToFile(void);
int 		Init_Member(void);
int		GetWord(int index);
int 		Load_List(char * pData[], int count);
int 		Load_List2(char * pData1[], int count1, char * pData2[], int count2 );

int		Append(char *pData);
int 		Chk_UpDownCount(void);
int 		Get_NameList(int direction);
int 		Remake_NameList(int move_value);
int 		Set_CloseByWord(char* wordname);
void		Counting(void);
int 		ReCounting(int num);
char*	Search(char * word);

int		Redundance_Delete(char* word);
int		Delete(void);

void 	DrawGuide( void);
void		DrawBox(void);
void		DrawPage(void);
void 	DrawSelectbar(void);
void		DrawScrollbar(int total, int start, int maxvisible);
int 		DrawTitle(int xpos, int ypos);

int 		Show(void);
int 		ReShow(void);

void		Up(void);
void		Down(void);
void		Left(void);
void		Right(void);
char* 	Ok(void);
};

#endif /* __LIST_MANAGER_H__ */
