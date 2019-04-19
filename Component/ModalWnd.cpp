//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Modal Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ModalWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************

// $Log: ModalWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.17  2005/10/20 09:48:45  yong
// add _SetupPixmap(), _ClearPixmap()
//
// Revision 1.16  2005/10/17 07:53:06  yong
// refactoring
// always use font size 12 whether using resource ID or CString
//
// Revision 1.15  2005/09/14 02:30:46  zstein
// - update
//
// Revision 1.14  2005/09/14 01:25:53  ywkim
// change No. of icons in ICON_DBXICON from 4 to 5
//
// Revision 1.13  2005/08/25 09:30:50  etnlwind
// ICON_OK_LEFT_EXIT macro changed to ICON_DBXICON
//
// Revision 1.12  2005/08/18 13:46:38  ywkim
// add settine of initial value for m_nEditResID( -1 )
//
// Revision 1.11  2005/08/18 12:18:30  zstein
// - update
//
// Revision 1.10  2005/08/17 06:12:21  ywkim
// Change count of image for ICON_DBXICON
//
// Revision 1.9  2005/05/17 19:19:55  ywkim
// Remove DT_BOLD on title
//
// Revision 1.8  2005/05/17 10:42:49  ywkim
// Move down title from 2 to 4
//
// Revision 1.7  2005/05/15 11:08:49  zstein
// *** empty log message ***
//
// Revision 1.6  2005/05/12 09:38:13  ywkim
// Remove DT_OUTLINE
//
// Revision 1.5  2005/05/12 04:18:32  zstein
// *** empty log message ***
//
// Revision 1.4  2005/04/27 11:40:01  zstein
// change bool OnOK(void) to bool OnOK(int& state).
// @state has three values. MODAL_OK, MODAL_CANCEL, MODAL_EXIT.
//
// Revision 1.3  2005/04/27 08:57:20  zstein
// added ok/cancel/exit button.
// changed title string to title resource id.
//
// Revision 1.2  2005/04/27 04:53:21  zstein
// Support Multi-languages.
// Support resource text.
//
// Revision 1.1  2005/03/01 02:02:02  zstein
// *** empty log message ***
//
//

//*******************************************************************************

#include <Lib/LibImage.h>
#include "ModalWnd.h"
#include "CharSet.h"
#include <Task/tavi_global.h>

// Macros
#define DEFAULT_TITLE_COLOR			31
#define BACKGROUND_COLOR			24
#define BORDER_COLOR				25
#define EDGE_SIZE					4
#define ICON_SIZE					14

// Extern Variables
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CTextEngine_BDF* pT;

CModalWnd::CModalWnd( CWindow* parent ) :
	CWindow( parent ),
	m_nTitleColor( DEFAULT_TITLE_COLOR ),
	m_fOK( false ),
	m_nTitleResID( -1 ), 
	m_nOkResID( -1 ),
	m_nCancelResID( -1 ),
	m_nExitResID( -1 ),
	m_nEditResID( -1 )
{
	changed				= 0;
	m_nBackgroundColor	= BACKGROUND_COLOR;
	m_nBorderColor		= m_nBackgroundColor;
	m_nTitleHeight		= 20;	// XXX

	m_nButtonState		= -1;
	
	m_pLeftTopEdge		= NULL;
	m_pRightTopEdge		= NULL;
	m_pLeftBottomEdge	= NULL;
	m_pRightBottomEdge	= NULL;
	m_pOK				= NULL;
	m_pCancel			= NULL;
	m_pExit				= NULL;
	m_pEdit				= NULL;

	this->_SetupPixmap();
}

CModalWnd::~CModalWnd( void )
{
	this->_ClearPixmap();
}

void
CModalWnd::_ClearPixmap( void )
{
	if (this->m_pLeftTopEdge) { delete this->m_pLeftTopEdge; }
	this->m_pLeftTopEdge = NULL;

	if (this->m_pRightTopEdge) { delete this->m_pRightTopEdge; }
	this->m_pRightTopEdge = NULL;

	if (this->m_pLeftBottomEdge) { delete this->m_pLeftBottomEdge; }
	this->m_pLeftBottomEdge = NULL;

	if (this->m_pRightBottomEdge) { delete this->m_pRightBottomEdge; }
	this->m_pRightBottomEdge = NULL;

	if (this->m_pOK) { delete this->m_pOK; }
	this->m_pOK = NULL;

	if (this->m_pCancel) { delete this->m_pCancel; }
	this->m_pCancel = NULL;

	if (this->m_pExit) { delete this->m_pExit; }
	this->m_pExit = NULL;

	if (this->m_pEdit) { delete this->m_pEdit; }
	this->m_pEdit = NULL;
}

void
CModalWnd::_SetupPixmap( void )
{
	m_pLeftTopEdge = new CPixmap;
	m_pLeftTopEdge->CreatePixmap( EDGE_SIZE, EDGE_SIZE, 8, NULL, true );
	m_pLeftTopEdge->Fill( 0, 0, EDGE_SIZE, EDGE_SIZE, m_nTitleColor );
	m_pLeftTopEdge->Fill( 0, 0, 1, 1, COLOR_TRANSPARENT );
	m_pLeftTopEdge->Fill( 0, 1, 1, 1, 26 );
	m_pLeftTopEdge->Fill( 0, 2, 1, 1, 25 );
	m_pLeftTopEdge->Fill( 0, 3, 1, 1, 24 );
	m_pLeftTopEdge->Fill( 1, 0, 1, 1, 26 );
	m_pLeftTopEdge->Fill( 2, 0, 1, 1, 25 );
	m_pLeftTopEdge->Fill( 3, 0, 1, 1, 24 );
	m_pLeftTopEdge->Fill( 1, 1, 1, 1, 24 );

	m_pRightTopEdge = new CPixmap;
	m_pRightTopEdge->CreatePixmap( EDGE_SIZE, EDGE_SIZE, 8, NULL, true );
	m_pRightTopEdge->Fill( 0, 0, EDGE_SIZE, EDGE_SIZE, m_nTitleColor );
	m_pRightTopEdge->Fill( 0, 0, 1, 1, 24 );
	m_pRightTopEdge->Fill( 1, 0, 1, 1, 25 );
	m_pRightTopEdge->Fill( 2, 0, 1, 1, 26 );
	m_pRightTopEdge->Fill( 3, 0, 1, 1, COLOR_TRANSPARENT );
	m_pRightTopEdge->Fill( 3, 1, 1, 1, 26 );
	m_pRightTopEdge->Fill( 3, 2, 1, 1, 25 );
	m_pRightTopEdge->Fill( 3, 3, 1, 1, 24 );
	m_pRightTopEdge->Fill( 2, 1, 1, 1, 24 );

	m_pLeftBottomEdge = new CPixmap;
	m_pLeftBottomEdge->CreatePixmap( EDGE_SIZE, EDGE_SIZE, 8, NULL );
	m_pLeftBottomEdge->Fill( 0, 0, EDGE_SIZE, EDGE_SIZE, m_nBackgroundColor );
	m_pLeftBottomEdge->Fill( 0, 0, 1, 2, m_nBorderColor );
	m_pLeftBottomEdge->Fill( 0, 2, 1, 1, 26 );
	m_pLeftBottomEdge->Fill( 0, 3, 1, 1, COLOR_TRANSPARENT );
	m_pLeftBottomEdge->Fill( 1, 3, 1, 1, 26 );
	m_pLeftBottomEdge->Fill( 2, 3, 2, 1, m_nBorderColor );
	m_pLeftBottomEdge->Fill( 1, 2, 1, 1, m_nBorderColor );

	m_pRightBottomEdge = new CPixmap;
	m_pRightBottomEdge->CreatePixmap( EDGE_SIZE, EDGE_SIZE, 8, NULL );
	m_pRightBottomEdge->Fill( 0, 0, EDGE_SIZE, EDGE_SIZE, m_nBackgroundColor );
	m_pRightBottomEdge->Fill( 0, 3, 2, 1, m_nBorderColor );
	m_pRightBottomEdge->Fill( 2, 3, 1, 1, 26 );
	m_pRightBottomEdge->Fill( 3, 3, 1, 1, COLOR_TRANSPARENT );
	m_pRightBottomEdge->Fill( 3, 2, 1, 1, 26 );
	m_pRightBottomEdge->Fill( 3, 0, 1, 2, m_nBorderColor );
	m_pRightBottomEdge->Fill( 2, 2, 1, 1, m_nBorderColor );

	m_pOK = new CPixmap;
	int iw = TAVI_IMAGES[ICON_DBXICON].bitmapinfoheader.biWidth;
	//int ih = TAVI_IMAGES[ICON_DBXICON].bitmapinfoheader.biHeight;
	int ih = ICON_SIZE;
	char* p = ( char* )TAVI_IMAGES[ICON_DBXICON].image_buffer;
	m_pOK->CreatePixmap( iw, ih, 8, p, false );
	m_pCancel = new CPixmap;
	m_pCancel->CreatePixmap( iw, ih, 8, p+iw*ih, false );
	m_pExit = new CPixmap;
	m_pExit->CreatePixmap( iw, ih, 8, p+2*iw*ih, false );
	m_pEdit = new CPixmap;
	m_pEdit->CreatePixmap( iw, ih, 8, p+3*iw*ih, false );
}

void CModalWnd::OnPaint( const CRect& rc )
{
	CWindow::OnPaint( rc );

	g_SysPixmap.Fill( rcABS.x1, rcABS.y1, rcABS.GetWidth(), m_nTitleHeight, m_nTitleColor );
	
	if( m_fVisible == false ) {
		return;
	}

	if (this->changed) {
		this->_ClearPixmap();
		this->_SetupPixmap();
		this->changed = 0;
	}

	op_BitBlt( &g_SysPixmap, rcABS.x1, rcABS.y1, m_pLeftTopEdge, 
		0, 0, m_pLeftTopEdge->m_nWidth, m_pLeftTopEdge->m_nHeight );

	op_BitBlt( &g_SysPixmap, rcABS.x2-m_pRightTopEdge->m_nWidth+1, rcABS.y1, m_pRightTopEdge, 
		0, 0, m_pRightTopEdge->m_nWidth, m_pRightTopEdge->m_nHeight );

	op_BitBlt( &g_SysPixmap, rcABS.x1, rcABS.y2-m_pLeftBottomEdge->m_nHeight+1, 
		m_pLeftBottomEdge, 0, 0, m_pLeftBottomEdge->m_nWidth, m_pLeftBottomEdge->m_nHeight );

	op_BitBlt( &g_SysPixmap, rcABS.x2-m_pRightBottomEdge->m_nWidth+1, rcABS.y2-m_pLeftBottomEdge->m_nHeight+1, 
		m_pRightBottomEdge, 0, 0, m_pRightBottomEdge->m_nWidth, m_pRightBottomEdge->m_nHeight );

	int color		= _WHITE;
	int fontSize	= 12;
	int dtMode		= DT_TRANSPARENCY;

	// draw Title
	if( m_nTitleResID > -1 )
		pT->DrawResText( m_nTitleResID, rcABS.x1+8, rcABS.y1+4, color, fontSize, dtMode );
	else {
		if( m_szTitle.IsUnicode() ) {
			pT->DrawUC16Text( (uchar_t*)m_szTitle, rcABS.x1+8, rcABS.y1+4, color, fontSize, dtMode );
		} else {
			uchar_t wsz[128];
			ConvertUnicode( (char*)m_szTitle, m_szTitle.GetSize(), wsz, sizeof(wsz), TaviSysParam.etc.language );
			pT->DrawUC16Text( wsz, rcABS.x1+8, rcABS.y1+4, color, fontSize, dtMode );
		}
	}

	int offset_y = m_pOK->m_nHeight + 8; // 8: space between bottom of the window and the bottom of the button.
	int offset_x = 16;
	int start_x;
	int totalWidth = 0;
	int nOkTextWidth=0;
	int nCancelTextWidth = 0;
	int nExitTextWidth = 0;
	int nEditTextWidth = 0;
	int nSpacing = 4;
	if( m_nExitResID > -1 ) {
		nExitTextWidth = pT->GetStringWidth( m_nExitResID, 12, 0 );
		totalWidth += nExitTextWidth;
		totalWidth += m_pOK->m_nWidth;
		totalWidth += nSpacing;
	}
	if( m_nCancelResID > -1 ) {
		nCancelTextWidth = pT->GetStringWidth( m_nCancelResID, 12, 0 );
		totalWidth += nCancelTextWidth;
		totalWidth += m_pOK->m_nWidth;
		totalWidth += nSpacing;
		if( totalWidth ) totalWidth += nSpacing;
	}
	if( m_nOkResID > -1 ) {
		nOkTextWidth =  pT->GetStringWidth( m_nOkResID, 12, 0 );
		totalWidth += nOkTextWidth;
		totalWidth += m_pOK->m_nWidth;
		totalWidth += nSpacing;
		if( totalWidth ) totalWidth += nSpacing;
	}
	if( m_nEditResID > -1 ) {
		nEditTextWidth = pT->GetStringWidth( m_nEditResID, 12, 0 );
		totalWidth += nEditTextWidth;
		totalWidth += m_pEdit->m_nWidth;
		totalWidth += nSpacing;
		if( totalWidth ) totalWidth += nSpacing;
	}
	start_x = rcABS.GetWidth()-totalWidth-offset_x;

	dtMode = DT_TRANSPARENCY | DT_OUTLINE;

	// draw edit button
	if( m_nEditResID > -1 ) {
		op_BitBlt( &g_SysPixmap, rcABS.x1+start_x, rcABS.y2-offset_y, 
			m_pEdit, 0, 0, m_pEdit->m_nWidth, m_pEdit->m_nHeight );
		// draw edit text
		start_x += m_pEdit->m_nWidth+nSpacing;
		pT->DrawResText( m_nEditResID, rcABS.x1+start_x, rcABS.y2-offset_y+2, color, fontSize, dtMode);
		start_x += nEditTextWidth + nSpacing;
	}
	if( m_nOkResID > -1 ) {
		op_BitBlt( &g_SysPixmap, rcABS.x1+start_x, rcABS.y2-offset_y, 
			m_pOK, 0, 0, m_pOK->m_nWidth, m_pOK->m_nHeight );
		// draw ok text
		start_x += m_pOK->m_nWidth+nSpacing;
		pT->DrawResText( m_nOkResID, rcABS.x1+start_x, rcABS.y2-offset_y+2, color, fontSize, dtMode);
		start_x += nOkTextWidth + nSpacing;
	}
	if( m_nCancelResID > -1 ) {
		op_BitBlt( &g_SysPixmap, rcABS.x1+start_x, rcABS.y2-offset_y, 
			m_pCancel, 0, 0, m_pCancel->m_nWidth, m_pCancel->m_nHeight );
		// draw cancel text
		start_x += m_pCancel->m_nWidth+nSpacing;
		pT->DrawResText( m_nCancelResID, rcABS.x1+start_x, rcABS.y2-offset_y+2, color, fontSize, dtMode);
		start_x += nCancelTextWidth + nSpacing;
	}
	if( m_nExitResID > -1 ) {
		op_BitBlt( &g_SysPixmap, rcABS.x1+start_x, rcABS.y2-offset_y, 
			m_pExit, 0, 0, m_pExit->m_nWidth, m_pExit->m_nHeight );
		// draw exit text
		start_x += m_pExit->m_nWidth+nSpacing;
		pT->DrawResText( m_nExitResID, rcABS.x1+start_x, rcABS.y2-offset_y+2, color, fontSize, dtMode);
	}
	
}
