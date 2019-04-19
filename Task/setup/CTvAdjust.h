//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : TV Adjust Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CTvAdjust.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CTvAdjust.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/08/29 10:15:04  ywkim
// change method of TVadjust
//
// Revision 1.5  2005/08/23 06:08:39  ywkim
// change function of OnKeypressed()
//
// Revision 1.4  2005/08/22 00:59:28  ywkim
// Change funciton of SetupDisplay() for HDcomponent
//
// Revision 1.3  2005/08/09 08:14:13  ywkim
// Add function for screen ratio
//
// Revision 1.2  2005/08/08 11:33:28  ywkim
// Change soume routine
//
// Revision 1.1  2005/08/03 05:30:05  ywkim
// Add new item of TVadjust for output setup
//
//*****************************************************************************

#ifndef __TV_ADJUST_H__
#define __TV_ADJUST_H__

#include <Component/window.h>
#include <Task/tavi_global.h>

//==== ADJUST TYPE =======
#define	SET_LEFTUPPER 		0
#define	SET_RIGHTLOWER 	1
//==== ADJUST TYPE =======

class CTvAdjust : public CWindow
{
private:
protected:
	Wnd_t	limit;
	Wnd_t	*m_pWnd;
	bool		m_isVisible;
	char		m_cAdjustType;

public:
	CTvAdjust( void );
	~CTvAdjust( void );
	void SetLimit( int mode );
	void GetParameter( int mode );
	void OnKeypressed( const KeyEvent& evt );
	void MoveLeftUpperLeft(int length);
	void MoveLeftUpperRight(int length);
	void MoveLeftUpperUp(int length);
	void MoveLeftUpperDown(int length);
	void MoveRightLowerLeft(int length);
	void MoveRightLowerRight(int length);
	void MoveRightLowerUp(int length);
	void MoveRightLowerDown(int length);
	
	void Show( void );
	void Hide( void );
	void OnPaint( void );
	void ReDraw(void);
	bool isVisible( void );
};

#endif /*__TV_ADJUST_H__*/

