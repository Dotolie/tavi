//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Extra Dictionary
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/CExtraDic.h,v $
// $Revision: 1.5 $
// $Date: 2006/03/06 11:18:22 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: CExtraDic.h,v $
// Revision 1.5  2006/03/06 11:18:22  akaiaphanu
// working UI
//
// Revision 1.4  2006/02/28 07:08:20  akaiaphanu
// working touchpad
//
// Revision 1.3  2006/02/28 04:46:11  akaiaphanu
// working kor,eng filename
//
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.12  2006/02/23 01:56:58  akaiaphanu
// working search function
//
// Revision 1.11  2006/02/22 04:16:28  akaiaphanu
// working selectbar, itemlist
//
// Revision 1.10  2006/02/21 05:04:46  akaiaphanu
// exception message : case not k-dic
//
// Revision 1.9  2006/02/21 02:04:43  akaiaphanu
// working title, list manager
//
// Revision 1.8  2006/02/20 08:06:47  akaiaphanu
// change to array of structure from double linked list
//
// Revision 1.7  2006/02/20 01:33:56  akaiaphanu
// add ListManager , BitIO
//
// Revision 1.6  2006/02/14 08:37:16  akaiaphanu
// working bottom guide , scrollbar , box
//
// Revision 1.5  2006/02/11 12:40:14  akaiaphanu
// working dictionary keyboard
//
// Revision 1.4  2006/02/11 09:16:28  akaiaphanu
// working text resource
//
// Revision 1.3  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.2  2006/02/06 04:17:15  akaiaphanu
// add to history function
//
// Revision 1.1  2006/01/27 04:38:14  akaiaphanu
// added dictionary
//
// 
//*****************************************************************************

#ifndef __CExtraDic_WINDOW_H__
#define __CExtraDic_WINDOW_H__

#include <unistd.h>
#include <string.h>

#include "../../Lib/LibDefines.h"
#include "../../Lib/LibDialogBox.h"
#include "../../Lib/LibSubMenu.h"
#include "../../Lib/LibMsgBox.h"
#include "../../Lib/LibText.h"
#include "../../util.h"
#include "../tavi_global.h"

#include "Extra.h"
#include "hangul.h"
#include "vkbd.h"
#include "DicKeyboard.h"
#include "HistoryManager.h"
#include "DataManager.h"
#include "ListManager.h"
#include "BitIO.h"

#define DIC_STATE_NO_DATA				0
#define DIC_STATE_KEYBOARD_KOR		1
#define DIC_STATE_KEYBOARD_ENGU		2
#define DIC_STATE_KEYBOARD_ENGL		3
#define DIC_STATE_KEYBOARD_DGT		4
#define DIC_STATE_OUTWIN				5
#define DIC_STATE_HISTORY				6
#define DIC_STATE_DATAMANAGER		7
#define DIC_STATE_LISTMANAGER			8
#define DIC_STATE_NOT_SELECT_DATA	9
#define DIC_STATE_NOT_KDIC_TYPE		10

#define DIC_SEARCH_KOR					0
#define DIC_SEARCH_ENGU				1
#define DIC_SEARCH_ENGL				11
#define DIC_SEARCH_DGT					2

#define DIC_SRCH_CASE_OUT_SECTION		0
#define DIC_SRCH_CASE_IN_SECTION		1
#define DIC_SRCH_CASE_INTER_SECTION	2

#define DIC_DATA_TYPE_PDB				0
#define DIC_DATA_TYPE_TXT				1
#define DIC_DATA_TYPE_NML				2
#define DIC_DATA_TYPE_UNKNOWN		3

#define DIC_KEY_NORMAL					0
#define DIC_KEY_DELETE					1

#define DIC_DB_PATH						"/hd1/dic/"
#define DIC_SIGNATUREIDX				0x406f8282
#define DIC_MAKE_INDEX_SIZE			30

#define DIC_OUTWIN_VISIBLE_LINES		10
#define DIC_OUTWIN_VISIBLE_LINE_LEN	48
#define DIC_OUTWIN_TITLE_COLOR		31
#define DIC_OUTWIN_BASE_COLOR			24
#define DIC_OUTWIN_TITLE_SCROLL_W	240

#define DIC_SELECTBAR_TEXT_SCROLL_W	282

#define HISTORY_CHK_LOAD_STATE		0
#define HISTORY_CHK_LOAD_COUNT		1

#define	DOCHEADSZ						78
#define	SIGNATUREPDB					0x406f8000


typedef unsigned char  	u8;		/* 8-bit */
typedef char  			s8;		/* 8-bit */
typedef unsigned short 	u16;	/* 16-bit */
typedef unsigned long  	u32;	/* 32-bit */

struct tDocHeader {
	char  sName[32];	// 32 32
	u32 dwUnknown1;	//  4 36
	u32 dwTime1;		//  4 40
	u32 dwTime2;		//  4 44
	u32 dwTime3;		//  4 48
	u32 dwLastSync;	//  4 52
	u32 ofsSort;		//  4 56
	u32 ofsCategories;	//  4 60
	u32 dwCreator;	//  4 64
	u32 dwType;		//  4 68
	u32 dwUnknown2;	//  4 72
	u32 dwUnknown3;	//  4 76
	u16  wNumRecs;		//  2 78 0x4e
};

struct tDicIndex {
	u32	dwRecNo;
	u32	dwIdxpos; //파일 포지션
	u16 	size;
	char	startchar[128];
	char	endchar[128];
};

struct _IdxInformation {
	char	sName[256];
	u32 	ffpos;
	u8	idxon;
};

typedef struct _tag_EXTRA_DIC_CONFIG {

	bool 				m_fExit;
	bool 				m_Resume;
	bool 				m_go_search;
	bool 				m_down_pressed;
	bool 				m_history_load;
	bool 				m_found;
	bool 				m_LastAccessData;
	bool					m_isExist_DicData;
	bool					m_isLoad_DicData;
	
	int 					m_ExitState;
	int					m_dic_pre_state;
	int					m_dic_cur_state;
	int					m_Dic_DataType;
	int					m_current_text_index;
	int					m_total_text_lines;
	int					m_current_text_line;
	int					m_visible_lines;
	int					m_line_length;
	int					m_total_page;
	int					m_current_page;
	uchar_t * 			m_text_display;
		
	CScrollBar3 *				m_pScrollbar3;
	class DataManager *  		m_pDM;	/* Data manager */
	class DicKeyboard * 		m_pDicKeyboard;	/* keyboard */
	class HistoryManager * 	m_pHistory;	/* history manager */
	class ListManager*		m_pLM;
	CMsgBox * 				m_pMBx_NoDic;
	CMsgBox * 				m_pMBx_NoSelectDic;
	CMsgBox * 				m_pMBx_NoKDicType;

	u32					m_dbtxt_total_size;
	u32					m_dbidx_total_count;
	char 				m_last_filename[512];
	uchar_t 				m_last_ufilename[512];

	struct tDocHeader 	tDicH;
	struct tDicIndex*		tDicI;
	u32					reclength;
	s8					pdbFile[1024];
	char*				m_neighbor_word_list[512];
	char*				m_neighbor_word_prelist[512];
	char*				m_neighbor_word_nextlist[512];
	int					m_neighbor_word_list_idx;
	int					m_neighbor_word_list_preidx;
	int					m_neighbor_word_list_nextidx;
	int					m_tDicI_Cur_Idx;
	int					m_search_case;
	int					m_key_idle;
	int					m_sx_scroll;

	char 				m_searched_text[8192];	/* to get from db text */
	char 				m_checked_text[8192];
	char 				m_couple_info[8192];
	int					m_text_line_index[1024];	/* position for each line to text array */
	char 				m_word[1024];	/* word */
	int 					m_line_position_info[1024];
	int					m_loadable_word_count;
	

} _ATTRIBUTE_PACKED_ EXTRA_DIC_CONFIG;


int 	ExDic_Init(void);

int 	ExDic_Free_WordList(void);
int 	ExDic_Free(void);
int 	ExDic_MainScreen(void);
int 	ExDic_FinishScreen(void);
int 	ExDic_Draw_History_Box(void);
int 	ExDic_Draw_DM_Box(void);
int 	ExDic_Draw_LM_Box(void);

int 	ExDic_Load_WNameToLM (void);
int 	ExDic_Load_OneSection(char* file_path, int which_section);
int 	ExDic_Re_Search(void);
int 	ExDic_IsSearchMore(void);
int 	ExDic_SrchFailCase_Proc(int cur_mode, int pre_mode);
int 	ExDic_Search_Word(void);
int 	ExDic_Chk_Text2(void);
int 	ExDic_Output_Screen(void);

int	ExDic_Draw_ScrollTxt(int title_width);
int 	ExDic_Draw_TextTitle(void);
int 	ExDic_Draw_Text(void);
int 	ExDic_Draw_History_Box(void);
int 	ExDic_Draw_DM_Box(void);
int 	ExDic_Draw_LM_Box(void);
void ExDic_DrawBottomGuidePannel( void);
int 	ExDic_Draw_Scrollbar(RMuint32 total, RMuint32 start, RMuint32 maxvisible );
int 	ExDic_Draw_OutWin_Bg(void);

int 	Extra_TimeCal(int mode);
int	fatfs_fgets(RMuint8*buf, int size, RMuint32 fd);

int 			ExDic_OnKeyPress( int key );
int 			ExDic_MsgProc( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 );
extern int 	ExDic_Task(void);

#endif /* __CExtraDic_WINDOW_H__ */
