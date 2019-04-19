//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : TV Adjust Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CTvAdjust.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CTvAdjust.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.12  2005/10/27 05:32:18  ywkim
// Add TAVI_CannotSleep for TV Adjusting routine
//
// Revision 1.11  2005/10/19 05:30:22  etnlwind
// DEBUG Removed
//
// Revision 1.10  2005/08/31 03:55:18  ywkim
// change limit of lowerright corner
//
// Revision 1.9  2005/08/29 10:15:04  ywkim
// change method of TVadjust
//
// Revision 1.8  2005/08/23 06:08:39  ywkim
// change function of OnKeypressed()
//
// Revision 1.7  2005/08/22 00:59:28  ywkim
// Change funciton of SetupDisplay() for HDcomponent
//
// Revision 1.6  2005/08/09 08:14:13  ywkim
// Add function for screen ratio
//
// Revision 1.5  2005/08/08 12:02:26  etnlwind
// Update for GetDividerEnclosedStringWidth()
//
// Revision 1.4  2005/08/08 12:01:36  etnlwind
// Update for GetDividerEnclosedStringWidth()
//
// Revision 1.3  2005/08/08 11:33:28  ywkim
// Change soume routine
//
// Revision 1.2  2005/08/03 06:38:34  ywkim
// Change no of horizontal line for ntsc
//
// Revision 1.1  2005/08/03 05:30:05  ywkim
// Add new item of TVadjust for output setup
//
//*****************************************************************************
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>	// memmove/memcpy
#include <Lib/LibImage.h>

//#include <Config.h>
#include <Task/tavi_global.h>

#include "CTvAdjust.h"

#if 0
#define DEBUG
#ifdef DEBUG
	#define debug_outs	printf
	static void debug_break (void)
	{
	}
	#define ASSERT(exp)					((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
	#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))
#else
	#define ASSERT(exp)
	#define DEBUGMSG(cond,printf_exp)
#endif
#endif

extern SysParameter_t	TaviSysParam;
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern SysParameter_t	TaviSysParam;
extern RMint32	toggle_display;
extern MpegDecoder* pMpegDecoder;
extern CTextEngine_BDF	*pT;

extern void showbackground( char* path );
extern void showbackground( int nMenuId );

CTvAdjust::CTvAdjust( void )
{
	m_isVisible = false;
	m_cAdjustType = SET_LEFTUPPER;
}
CTvAdjust::~CTvAdjust( void )
{
}
void CTvAdjust::SetLimit(int mode)
{
	switch( mode )
	{
		case 0:
			// go to digital output	
			limit.x = 0;
			limit.y = 0;
			limit.w = 320;
			limit.h = 240;
			break;
		case 1:
			// ntsc - composite/svideo
			limit.x = 0;
			limit.y = 0;
			limit.w = 720;
			limit.h = 480;
			break;
		case 2:
			// pal - composite/svideo
			limit.x = 0;
			limit.y = 0;
			limit.w = 720;
			limit.h = 576;
			break;
		case 3:
			// ntsc - component
			limit.x = 0;
			limit.y = 0;
			limit.w = 720;
			limit.h = 480;			
			break;
		case 4:
			// pal - component
			limit.x = 0;
			limit.y = 0;
			limit.w = 720;
			limit.h = 576;			
			break;
		case 5:
			// 480p
			limit.x = 0;
			limit.y = 0;
			limit.w = 720;
			limit.h = 480;
			break;
		case 6:
			// 720p
			limit.x = 0;
			limit.y = 0;
			limit.w = 1280;
			limit.h = 720;
			break;
		case 7:
			// 1080i
			limit.x = 0;
			limit.y = 0;
			limit.w = 1920;
			limit.h = 1080;
			break;

	}
}

void CTvAdjust::GetParameter(int mode)
{
	switch( mode )
	{
		case 0:
			// go to digital output	
			m_pWnd = &TaviSysParam.output[0];
			break;
		case 1:
			// ntsc - composite/svideo
			m_pWnd = &TaviSysParam.output[1];
			break;
		case 2:
			// pal - composite/svideo
			m_pWnd = &TaviSysParam.output[2];
			break;
		case 3:
			// ntsc - component
			m_pWnd = &TaviSysParam.output[3];
			break;
		case 4:
			// pal - component
			m_pWnd = &TaviSysParam.output[4];
			break;
		case 5:
			// 480p
			m_pWnd = &TaviSysParam.output[5];
			break;
		case 6:
			// 720p
			m_pWnd = &TaviSysParam.output[6];
			break;
		case 7:
			// 1080i
			m_pWnd = &TaviSysParam.output[7];
			break;
	}
	DEBUGMSG(1,("CTvAdjust : GetParameter :mode=%d, x=%d,y=%d,w=%d,h=%d\n", mode,m_pWnd->x,m_pWnd->y,m_pWnd->w,m_pWnd->h) );
}

void CTvAdjust::OnKeypressed( const KeyEvent& evt )
{
	DEBUGMSG(1,("CTvAdjust : OnkeyPress = 0x%x\n", evt.value) );
	switch ( evt.value)
	{
		case VK_ENTER:
			if ( m_cAdjustType == SET_LEFTUPPER)	m_cAdjustType = SET_RIGHTLOWER;
			else m_cAdjustType = SET_LEFTUPPER;
			OnPaint();
			break;
		case VK_LEFT:
			if ( m_cAdjustType == SET_LEFTUPPER )	MoveLeftUpperLeft( 10 );
			else		MoveRightLowerLeft( 10 );
			break;
		case VK_RIGHT:
			if ( m_cAdjustType == SET_LEFTUPPER )	MoveLeftUpperRight( 10 );
			else		MoveRightLowerRight( 10 );
			break;
		case VK_UP:
			if ( m_cAdjustType == SET_LEFTUPPER )	MoveLeftUpperUp( 10 );
			else 	MoveRightLowerUp( 10 );
			break;
		case VK_DOWN:
			if ( m_cAdjustType == SET_LEFTUPPER )	MoveLeftUpperDown( 10 );
			else 	MoveRightLowerDown( 10 );
			break;
		case VK_ESC:
			Hide();
			break;
	}
}

void CTvAdjust::MoveLeftUpperLeft(int length)
{
	if ( (m_pWnd->x -length) < 0 ) return;
	m_pWnd->x -=length;
	m_pWnd->w +=length;
	ReDraw();
}
void CTvAdjust::MoveLeftUpperRight(int length)
{
	if ( (m_pWnd->x +length) > limit.w/2 ) return;
	m_pWnd->x +=length;
	m_pWnd->w -=length;
	ReDraw();
}
void CTvAdjust::MoveLeftUpperUp(int length)
{
	if ( ( m_pWnd->y -length) < 0 ) return;
	m_pWnd->y -=length;
	m_pWnd->h +=length;
	ReDraw();	
}
void CTvAdjust::MoveLeftUpperDown(int length)
{
	if ( ( m_pWnd->y +length) > limit.h/2 ) return;
	m_pWnd->y +=length;
	m_pWnd->h -=length;
	ReDraw();	
}
void CTvAdjust::MoveRightLowerLeft(int length)
{
	if ( ( m_pWnd->w -length) < limit.w/2 ) return;
	m_pWnd->w -= length;
	ReDraw();
}
void CTvAdjust::MoveRightLowerRight(int length)
{
	if ( ( m_pWnd->w +length) > (limit.x+limit.w) ) return;
	m_pWnd->w += length;
	ReDraw();
}
void CTvAdjust::MoveRightLowerUp(int length)
{
	if ( ( m_pWnd->h -length) < limit.h/2 ) return;
	m_pWnd->h -= length;
	ReDraw();	
}
void CTvAdjust::MoveRightLowerDown(int length)
{
	if ( ( m_pWnd->h +length) > (limit.y+limit.h) ) return;
	m_pWnd->h += length;
	ReDraw();	
}

void CTvAdjust::Show(void)
{
	m_isVisible = true;
	OnPaint();
}
void CTvAdjust::Hide(void)
{
	showbackground( ID_BG_SETUP );		// setup
	m_isVisible  = false;
}
void CTvAdjust::OnPaint(void)
{
	int 	nHalfWidth;
	GetParameter( (int)toggle_display );
	SetLimit( (int)toggle_display );

	if ( m_cAdjustType == SET_LEFTUPPER)
	{
		showbackground(_IMAGE_BG"/tv-position.jpg");

		nHalfWidth = pT->GetDividerEnclosedStringWidth(ML_MSG_ADJUST_POS,12,DT_TRANSPARENCY |DT_ENABLE_DIVIDER)/2;
//printf("\n\nnHalfWidth:%d\n\n",nHalfWidth);
		pMpegDecoder->RenderText_WithoutRefresh(0,0,0,173,320,50);		// Clear OSD
		pT->DrawResText(ML_MSG_ADJUST_POS,					// disp "Use arrow ..."
			160-nHalfWidth, 173,
			_WHITE,
			12,
			DT_TRANSPARENCY |DT_ENABLE_DIVIDER
			);
	}
	else
	{
		showbackground(_IMAGE_BG"/tv-size.jpg");
		nHalfWidth = pT->GetDividerEnclosedStringWidth(ML_MSG_ADJUST_SIZE,12,DT_TRANSPARENCY |DT_ENABLE_DIVIDER)/2;
//printf("\n\nnHalfWidth:%d\n\n",nHalfWidth);
		pMpegDecoder->RenderText_WithoutRefresh(0,0,0,173,320,50);		// Clear OSD		
		pT->DrawResText(ML_MSG_ADJUST_SIZE,				// disp "Use arrow ..."
			160-nHalfWidth, 173,
			_WHITE,
			12,
			DT_TRANSPARENCY |DT_ENABLE_DIVIDER
			);
	}
	nHalfWidth = pT->GetStringWidth(ML_SET_OUT_TVSCREEN_POS,12,0)/2;
	pT->DrawResText(ML_SET_OUT_TVSCREEN_POS,				// disp "adjust screen position"
		88-nHalfWidth, 50,
		_WHITE,
		12,
		DT_TRANSPARENCY
		);
	nHalfWidth = pT->GetStringWidth(ML_SET_OUT_TVSCREEN_SIZE,12,0)/2;
	pT->DrawResText(ML_SET_OUT_TVSCREEN_SIZE,				// disp "adjust screen position"
		232-nHalfWidth, 50,
		_WHITE,
		12,
		DT_TRANSPARENCY
		);		
	pMpegDecoder->RefreshOsd();

	ReDraw();
}

void CTvAdjust::ReDraw(void)
{
	pMpegDecoder->SetOSDDestination(m_pWnd->x, m_pWnd->y, m_pWnd->w, m_pWnd->h);
	pMpegDecoder->SetVideoDestination(m_pWnd->x, m_pWnd->y, m_pWnd->w, m_pWnd->h);
	DEBUGMSG(1,("CTvAdjust::ReDraw() x=%d,y=%d,w=%d,h=%d\n",m_pWnd->x,m_pWnd->y,m_pWnd->w,m_pWnd->h) );
}

bool CTvAdjust::isVisible(void)
{
	return m_isVisible;
}

