//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : History Manager Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/HistoryManager.h,v $
// $Revision: 1.4 $
// $Date: 2006/03/06 11:18:22 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: HistoryManager.h,v $
// Revision 1.4  2006/03/06 11:18:22  akaiaphanu
// working UI
//
// Revision 1.3  2006/03/03 05:15:12  akaiaphanu
// working text scroll
//
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.4  2006/02/20 01:33:56  akaiaphanu
// add ListManager , BitIO
//
// Revision 1.3  2006/02/14 08:37:17  akaiaphanu
// working bottom guide , scrollbar , box
//
// Revision 1.2  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.1  2006/02/06 04:15:29  akaiaphanu
// add to History Manager
//
// 
//
//*****************************************************************************

#ifndef __HISTORY_MANAGER_H__
#define __HISTORY_MANAGER_H__

#include "CExtraDic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tavi_lib.h>

extern CTextEngine_BDF* pT;

#define HBOX_BASE_COLOR						24
#define HBOX_TITLE_COLOR						31
#define HBOX_SELECT_BAR_COLOR					27
#define HBOX_SELECT_BAR_SHADOW_COLOR		31

#define DIC_HISTORY_DATA_PATH					"/hd1/dic/_db/history.nml"
#define DIC_HISTORY_MAX_COUNT					256

struct HMNode { 
  int Num; 
  char Data[512]; 
  char Filepath[512];
  HMNode* pPrev;
  HMNode* pNext;
};

class HistoryManager 
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
	
	HMNode * 		m_Head;
	HMNode * 		m_Tail;
	int				m_count;
	CScrollBar3 *		m_pScrollbar3;
	int				m_total_index;
	int				m_cur_index;
	int				m_visibleline;
	int				m_cur_page;
	bool				m_history_load;
	int				m_redundance_delete;
	
public :
		
	HistoryManager(int, int, int, int);
	~HistoryManager();
int	LoadFromFile(void);
int	SaveToFile(void);
int 	Chk_History(int flags);
int 	SetMember(void);
char*  GetWord(int index);
int	Append(char *pData);
int 	ReCounting(void);
HMNode * Search(int pos, char * word);
int	Redundance_Delete(char* word);
int	Delete(void);
void DrawGuide( void);
void	DrawBox(void);
void	DrawPage(int cur_page, int max_line);
void DrawSelectbar(void);
void	DrawScrollbar(int total, int start, int maxvisible);
int 	DrawTitle(int xpos, int ypos);
int 	Show(void);
void	Counting(void);
void	Up(void);
void	Down(void);
void	Left(void);
void	Right(void);
char* Ok(void);
};

#endif /* __HISTORY_MANAGER_H__ */
