//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CListBox Class Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibListBox.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: LibListBox.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.18  2006/01/24 13:19:06  ywkim
// Change option of DT_RADIO_FONT_OPT to DT_NUMBER_8WIDTH
//
// Revision 1.17  2006/01/13 08:26:27  ywkim
// Add option of DT_RADIO_FONT_OPT for new font
//
// Revision 1.16  2006/01/02 09:06:21  etnlwind
// Update for Subtitle Maxline Expanded
//
// Revision 1.15  2005/12/01 12:04:48  ywkim
// Change Delete(), ReDraw() & Display() for Scrollbar
//
// Revision 1.14  2005/11/22 05:44:04  ywkim
// Change CreateTextData() for list icon of pause/play
//
// Revision 1.13  2005/06/21 12:09:35  ywkim
// remark printf for debug
//
// Revision 1.12  2005/06/21 12:06:56  ywkim
// *** empty log message ***
//
// Revision 1.11  2005/06/15 11:51:30  etnlwind
// Update for Video Player Touch Control And Background Savefile Process
//
// Revision 1.10  2005/06/14 14:01:32  ywkim
// Change touchpad func.
//
// Revision 1.9  2005/05/18 16:00:17  ywkim
// Change list control
//
// Revision 1.8  2005/05/17 10:43:57  ywkim
// Modify attribute of string
//
// Revision 1.7  2005/05/15 10:59:59  ywkim
// Fixed bug for scroll
//
// Revision 1.6  2005/05/12 09:33:05  ywkim
// Change list control to looping
//
// Revision 1.5  2005/05/09 15:06:28  ywkim
// Add new creator with resId for CListBox
//
// Revision 1.4  2005/05/03 11:37:03  ywkim
// Remove func. of BuzzerTick
//
// Revision 1.3  2005/04/22 01:58:39  etnlwind
// MultiLanguage Support
//
// Revision 1.2  2005/03/29 01:51:44  ywkim
// Add BuzzerTick()
//
// Revision 1.1  2005/02/26 10:10:16  etnlwind
// Library Á¤¸®
//
//
//*****************************************************************************

#include <string.h>
#include <Component/object.h>
#include <Component/image.h>

#include <Lib/LibImage.h>
#include <Lib/LibScrollBar.h>
#include <Lib/LibDialogBox.h>
#include <Lib/LibBookmarkDialogBox.h>
#include <Lib/LibSetupDialogBox.h>
#include <Lib/LibControlPannel.h>
#include <Lib/LibListBox.h>
#include <Lib/LibFileBrowser.h>
#include <Lib/LibBuzzer.h>

#include "LibDefines.h"

extern CTextEngine_BDF* pT;

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// CListBox Class Figure
//
//

CListBox::CListBox( void )
:CDialogBox()
{
	DEBUGMSG(1,("CListBox( void )") );

//	memset( m_pcLists,0, 30 );
	
	m_nCount = 0;
	m_nTopIndex = 0;
	m_nCurrentRow = 0;
	m_nMaxRow = 0;
	m_nSelectedIdx = 0;
	m_nOnState = 0;
}


CListBox::CListBox( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option )
:CDialogBox()
{
	DEBUGMSG(1,("CListBox( [%s], %d, %d, %d, %d)\n", sDbxTitle, x, y, w, h ));

	choice = 0;
	sx = x;
	sy = y;
	width = w;
	height = h;

	fpDialogBoxMsgProc =  pfpMsgProc;

	m_nMaxRow = 8;			// max row

	pSb1 = new CScrollBar( 
		sx + width - (8+5), 
		sy +_DBX_SELECTBAR_START_Y, 
		_DBX_SELECTBAR_HEIGHT * 8, 
		m_nCount,
		m_nTopIndex,
		MIN( (m_nCount - m_nTopIndex), 8)
		);

	memcpy(title,sDbxTitle,strlen(sDbxTitle));

}

CListBox::CListBox( U32 ResIndex, int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option )
:CDialogBox()
{
	DEBUGMSG(1,("CListBox( [%d], %d, %d, %d, %d)\n", ResIndex, x, y, w, h ));

	
	choice = 0;
	sx = x;
	sy = y;
	width = w;
	height = h;

	fpDialogBoxMsgProc =  pfpMsgProc;

	m_nMaxRow = 8;			// max row

	m_titleIndex=ResIndex;

	pSb1 = new CScrollBar( 
		sx + width - (8+5), 
		sy +_DBX_SELECTBAR_START_Y, 
		_DBX_SELECTBAR_HEIGHT * 8, 
		m_nCount,
		m_nTopIndex,
		MIN( (m_nCount - m_nTopIndex), 8)
		);

}

CListBox::~CListBox( void)
{
      delete  pSb1;
}

void CListBox::CreateTextData()
{
	int iIcon;
	uchar_t ucText[128];
	U16 color;
	U32 option;
		
	for( int i=0; i<m_nMaxRow; i++)
	{

		if( choice == i )
		{
			color = _BLACK;
			option = (DT_TRANSPARENCY| DT_NUMBER_8WIDTH);
			if( m_nSelectedIdx == ( i + m_nTopIndex) && m_nOnState )
				if ( m_nOnState == 1 )
					iIcon = 19;	// play
				else
					iIcon = 67;	// pause
			else
				iIcon = 27;
		}
		else
		{
			color = _WHITE;
			option = (DT_TRANSPARENCY|DT_OUTLINE|DT_NUMBER_8WIDTH);
			if( m_nSelectedIdx ==( i + m_nTopIndex) && m_nOnState)
				if ( m_nOnState  == 1 )
					iIcon = 18;	// play
				else
					iIcon = 66;	// pause
			else
				iIcon = 26;

		}

		if ( m_pcLists[m_nTopIndex+i] !=NULL )
		{
			DrawItemWithoutRefresh(&TAVI_IMAGES[TYPE_ICON_LIST],	// draw icon
				iIcon,
				sx+_DBX_SELECTBAR_START_X + 6,
				sy+ _DBX_SELECTBAR_START_Y + ( i*_DBX_SELECTBAR_HEIGHT ) + 5,
				12, 
				12 
				);

			str2ustr( ucText, m_pcLists[m_nTopIndex+i], strlen(m_pcLists[m_nTopIndex+i]) );

			pT->DrawUC16Text(
				ucText,
				sx + _DBX_SELECTBAR_START_X + 36 ,
				sy+ _DBX_SELECTBAR_START_Y + ( i*_DBX_SELECTBAR_HEIGHT ) + 3 + 3,
				color,
				12,
				option
				);

		}

	}
}


void CListBox::CreateSelectBar()
{
	//int sby = sy+ _DBX_SELECTBAR_START_Y + ( (choice-1)*_DBX_SELECTBAR_HEIGHT );
	int sby = sy+ _DBX_SELECTBAR_START_Y + ( choice * _DBX_SELECTBAR_HEIGHT );


	DrawBoxWithoutRefresh( 
		27, 
		sx+_DBX_SELECTBAR_START_X, 
		sby, 
		width-(_DBX_SELECTBAR_START_X*2) -14,
		_DBX_ROW_HEIGHT-1
		);

	DrawBoxWithoutRefresh( 
		_DBX_SELECTED_ITEM_BAR_COLOR, 
		sx+_DBX_SELECTBAR_START_X, 
		sby, 
		23,
		_DBX_ROW_HEIGHT-1
		);

	DrawBoxWithoutRefresh( 
		_DBX_SELECTED_ITEM_BAR_SHADOW_COLOR, 
		sx+_DBX_SELECTBAR_START_X, 
		sby+_DBX_ROW_HEIGHT-1, 
		width-(_DBX_SELECTBAR_START_X*2) - 14,
		1
		);

	DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE], 	// left edge
		4, sx+_DBX_TITLE_RELATIVE_X, sby, 4,22 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE], 
		7, sx+width-(_DBX_TITLE_RELATIVE_X*2) -10, sby, 4,22 );

}


void CListBox::DrawTitle()
{
	pT->DrawResText(
		m_titleIndex,
		sx + _DBX_TITLE_RELATIVE_X,
		sy + _DBX_TITLE_RELATIVE_Y,
		_WHITE,
		12,
		DT_TRANSPARENCY | DT_BOLD
		);

	DrawBoxWithoutRefresh( 17, sx, sy+20, width, 1);
}

void CListBox::ReDraw()
{
	DrawBoxWithoutRefresh(
		0,	// RMuint8 color
		sx+2,							// RMuint16 sx
		sy+_DBX_TITLE_HEIGHT, 		// RMuint16 sy
		width-4, 						//RMuint16 width
		height-_DBX_TITLE_HEIGHT-2		// RMuint16 height)
		);			
//	ClearDbxContents();
	CreateSelectBar();
	CreateTextData();
	if ( m_nCount > 8 ) {
		pSb1->ReDraw(
			m_nCount ,
			m_nTopIndex,
			MIN( (m_nCount - m_nTopIndex), 8 )
			);
		}
	pMpegDecoder->RefreshOsd();
}

void CListBox::ReDraw( int nDown )
{
	int i;
	
	choice = 0;
	m_nTopIndex =0;
	
	for (i=0;i<nDown;i++)
		CursorDown();
	
	m_nSelectedIdx = nDown;
	DrawBoxWithoutRefresh(			// clear area
		0,								// RMuint8 color
		sx+2,							// RMuint16 sx
		sy+_DBX_TITLE_HEIGHT, 			// RMuint16 sy
		width-4, 							//RMuint16 width
		height-_DBX_TITLE_HEIGHT-2		// RMuint16 height)
		);			
//	ClearDbxContents();
	CreateSelectBar();
	CreateTextData();
	if ( m_nCount > 8 ) {
		pSb1->ReDraw(
			m_nCount,
			m_nTopIndex,
			MIN( (m_nCount - m_nTopIndex), 8 )
			);
		}
//	BuzzerTick();
	pMpegDecoder->RefreshOsd();
}

int CListBox::Insert( char* pList )
{
	m_pcLists[m_nCount] = pList;
	m_nCount++;
	
	return (m_nCount);
}

int CListBox::Delete(int idx )
{
	int	i;

	if ( idx >= m_nCount )
	{
		return -1;
	}

	if ( m_nCount > 8 ) {

		if ( m_nTopIndex > 0 ) {
			m_nTopIndex--;

			if ( (m_nCount - m_nTopIndex) > 8 ) {
				if ( choice < 7 )
					choice++;
				}
			else {
				if ( choice > 0 )
					choice--;
				}
			}
		else {	// m_nTopIndex == 0
			
			}

		}
	else {		// m_nCount <= 8
		if ( choice > 0 )
			choice--;
	}
			
	m_nCount--;
	m_pcLists[m_nCount] = NULL;

	return i;
}

int CListBox::DeleteAll( void )
{
	int	i;
	
	for ( i=0;i<m_nCount;i++)
		m_pcLists[i] = NULL;

	m_nCount = 0;
	m_nTopIndex = 0;
	m_nCurrentRow = 0;
	
	return i;
}
int CListBox::CursorUp()
{
	choice--;
	if(choice<0)
	{
		choice = 0;
		if ( m_nTopIndex > 0 )
		{
			m_nTopIndex--;
		}
		else
		{
			if ( m_nCount < m_nMaxRow )
			{
				m_nTopIndex = 0;
				choice = m_nCount-1;
				if ( choice < 0 ) choice = 0;
			}
			else
			{
				m_nTopIndex = m_nCount - m_nMaxRow;
				choice = m_nMaxRow-1;
			}
		}
	}

	return choice;
}

int CListBox::CursorDown()
{
	choice++;
	if( choice > m_nMaxRow -1)							// over max row
	{
		if ( (m_nTopIndex + m_nMaxRow ) < m_nCount )	// under max
		{
			m_nTopIndex++;
			choice = m_nMaxRow-1;
		}
		else
		{
			choice = 0;
			m_nTopIndex =0;
		}
	}
	else
	{
		if ( (m_nTopIndex + choice ) >= m_nCount )
		{
			choice=0;
			m_nTopIndex=0;
		}
	}

	return choice;
}



void CListBox::Display()
{
//	choice = 0;
//	CreateDbxBackground();
//	DrawTitle();	
	CreateSelectBar();
	CreateTextData();
#if 0	
	pSb1 = new CScrollBar( 
		sx + width - (8+5), 
		sy +_DBX_SELECTBAR_START_Y, 
		_DBX_SELECTBAR_HEIGHT * 8, 
		m_nCount,
		m_nTopIndex,
		MIN( (m_nCount - m_nTopIndex), 8)
		);
#else
	if ( m_nCount > 8 ) {
		pSb1->ReDraw(
			m_nCount,
			m_nTopIndex,
			MIN( (m_nCount - m_nTopIndex), 8 )
			);
		}
#endif
//	printf("***LibImage.cpp : m_nCount=%d,nTopIndex=%d\n",m_nCount,m_nTopIndex);
	pMpegDecoder->RefreshOsd();
}

void CListBox::Display(int nDown)
{
	int i;
	
	choice = 0;
	m_nTopIndex =0;
	
	for (i=0;i<nDown;i++)
		CursorDown();
	
	m_nSelectedIdx = nDown;
//	CreateDbxBackground();
//	DrawTitle();	
	CreateSelectBar();
	CreateTextData();
#if 0
	pSb1 = new CScrollBar( 
		sx + width - (8+5), 
		sy +_DBX_SELECTBAR_START_Y, 
		_DBX_SELECTBAR_HEIGHT * 8, 
		m_nCount ,
		m_nTopIndex,
		MIN( (m_nCount - m_nTopIndex), 8)
		);
#else
	if ( m_nCount > 8 ) {
		pSb1->ReDraw(
			m_nCount,
			m_nTopIndex,
			MIN( (m_nCount - m_nTopIndex), 8 )
			);
		}
#endif
//	printf("***LibImage.cpp : m_nCount=%d,nTopIndex=%d\n",m_nCount,m_nTopIndex);
	pMpegDecoder->RefreshOsd();
}



int CListBox::GetCurrentChoice()
{
	return (choice + m_nTopIndex);
}

