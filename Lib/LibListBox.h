//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CListBox Class Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibListBox.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: LibListBox.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/10/05 12:23:19  ywkim
// Add function of GetSelectedIndx() & SetSelectedIndx()
//
// Revision 1.5  2005/06/23 02:12:30  ywkim
// add GetOnState()
//
// Revision 1.4  2005/06/14 14:01:32  ywkim
// Change touchpad func.
//
// Revision 1.3  2005/05/09 15:07:05  ywkim
// Add new creator with resId for CListBox
//
// Revision 1.2  2005/05/03 11:37:03  ywkim
// Remove func. of BuzzerTick
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library Á¤¸®
//
//
//*****************************************************************************


 #ifndef __LIB_LIST_BOX_H__
 #define __LIB_LIST_BOX_H__

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
// CListControl Class
// --------------------------


typedef enum
{
	_FILEBROWSER,
	_INDEX

} eLISTCONTROL_TYPE;

typedef enum
{
	_LC_ICON_NUMBER,
	_LC_ICON_PICTURE

} eLISTCONTROL_ICONTYPE;

class CListBox: public CDialogBox
{
	protected:
		int	m_nCount;
		int	m_nTopIndex;
		int	m_nCurrentRow;
		int	m_nMaxRow;
		int	m_nSelectedIdx;
		char*	m_pcLists[30];
		int	m_titleIndex;
		int	m_nOnState;
		CScrollBar*			pSb1;

		void CreateTextData(void);
		void CreateSelectBar(void);
		void DrawTitle(void);

	public:
		CListBox( void );
		CListBox( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option  );
		CListBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option  );
		~CListBox( void );
		int Insert(char* pList );
		int Delete(int idx );
		int DeleteAll(void );
		int CursorUp(void);
		int CursorDown(void);
		int GetCurrentChoice(void);
		void Display(void);
		void Display(int nDown);
		void ReDraw(void);
		void ReDraw(int nDown);
		void SetOnState(int state){ m_nOnState = state;}
		int GetOnState(void){ return m_nOnState;}
		int GetSelectedIndx(void){ return m_nSelectedIdx; }
		void SetSelectedIndx(int Idx){ m_nSelectedIdx = Idx; }
};



class CListControl
{
	int 	choice;
	char 	title[256];
	char 	item[8][256];
	int	sx;
	int 	sy;
	int	width;
	int 	height;
	int 	LastItem;
	
	MSG_PROCESSOR fpDialogBoxMsgProc;

	void CreateListControlBackground();
	void ClearListControlContents();
	void CreateSelectBar();
	void CreateTextData( eLISTCONTROL_ICONTYPE ICONTYPE );


public:
	CListControl( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, eLISTCONTROL_TYPE eListType, U32 option );
	
	int SetTitle( char text[] );
	int AddItem(int idx, char contents[] );
	int CursorUp();
	int CursorDown();
	int GetCurrentChoice();
	void Display();
	void ReDraw();
	int MsgBtnUp( int ssTaskId, U32 ulMsgId, int usParam1, int usParam2 );

};
	


#endif


