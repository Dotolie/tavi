//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : File Manager Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/DataManager.h,v $
// $Revision: 1.4 $
// $Date: 2006/03/06 11:18:22 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: DataManager.h,v $
// Revision 1.4  2006/03/06 11:18:22  akaiaphanu
// working UI
//
// Revision 1.3  2006/02/28 04:46:11  akaiaphanu
// working kor,eng filename
//
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.7  2006/02/22 04:16:28  akaiaphanu
// working selectbar, itemlist
//
// Revision 1.6  2006/02/21 05:04:46  akaiaphanu
// exception message : case not k-dic
//
// Revision 1.5  2006/02/20 01:33:56  akaiaphanu
// add ListManager , BitIO
//
// Revision 1.4  2006/02/14 08:37:16  akaiaphanu
// working bottom guide , scrollbar , box
//
// Revision 1.3  2006/02/11 12:40:14  akaiaphanu
// working dictionary keyboard
//
// Revision 1.2  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.1  2006/02/09 07:45:58  akaiaphanu
// add to Data manager class
//
//
//*****************************************************************************

#ifndef __DATA_MANAGER_H__
#define __DATA_MANAGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Component/FileUtil.h"
#include "CExtraDic.h"

extern CTextEngine_BDF* pT;

#define DBOX_BASE_COLOR					24
#define DBOX_TITLE_COLOR					31
#define DBOX_TITLE_HEIGHT					21

#define DBOX_SELECTED_BAR_COLOR			27
#define DBOX_SELECTED_BAR_SHADOW_COLOR	31

#define DM_DATA_DIR_PATH					"/hd1/dic/"
#define DM_DATA_FILE_PATH					"/hd1/dic/_db/SaveLast.nml"
#define DM_MAX_COUNT						512
#define DM_VISIBLE_LINE						6
#define DM_SIGNATUREIDX					0x406f8282
#define DM_MAKE_INDEX_SIZE				30

typedef unsigned char  u8;	/* 8-bit */
typedef char  s8;	/* 8-bit */
typedef unsigned short u16;	/* 16-bit */
typedef unsigned long  u32;	/* 32-bit */

struct DMNode { 
  int 			num; 
  char 		file_name[512];
  uchar_t 		ufile_name[512];
  char 		file_path[512];
  DMNode* 	pPrev;
  DMNode* 	pNext;
};

class DataManager 
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
	
	DMNode * 		m_Head;
	DMNode * 		m_Tail;
	int				m_count;
	CScrollBar3 *		m_pScrollbar3;
	int				m_total_index;
	int				m_cur_index;
	int				m_visibleline;
	int				m_cur_page;
	
	bool				m_isTxt;
	bool				m_isIdx;
	
	
public :
		
		DataManager(int, int, int, int);
		~DataManager();
int  		Load_LastAccess_File(void);
int  		Save_LastAccess_File(void);

int 		Chk_isKDicType(char * filename);
void 	LoadFromFile(void);

int 		Append(char* filename, uchar_t* ufilename );
DMNode* Search(int pos, uchar_t* word);
char* 	GetFilename(int index);
void 	Delete(void);

void 	DrawGuide( void);
void		DrawBox(void);
void		DrawPage(int cur_page, int max_line);
void 	DrawSelectbar(void);
void		DrawScrollbar(int total, int start, int maxvisible);
int 		DrawTitle(int xpos, int ypos);
int 		Show(void);
void		Counting(void);

void		Up(void);
void		Down(void);
void		Left(void);
void		Right(void);
int	 	Ok(void);

int 		strpos(char *buf, char ch);
int 		strncpy2(char *dst, char *src, int start, int count);
int 		strcmp2(char *s1, char *s2);
int 		Load_Pdb(char* filename);

};

#endif /* __DATA_MANAGER_H__ */
