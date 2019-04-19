//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup General Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupDisplay.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupDisplay.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.4  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.3  2005/03/29 01:42:10  ywkim
// Add class of CSetupSound
//
// Revision 1.2  2005/03/05 06:29:37  ywkim
// Add function of touchpad on sub-setup-menu
//
// Revision 1.1  2005/03/03 04:55:07  ywkim
// New release
//
//*****************************************************************************

#ifndef __SETUPDISPLAY_WINDOW_H__
#define __SETUPDISPLAY_WINDOW_H__

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"
#include "CSetupWnd.h"

class CSetupDisplayWnd : public CSetupWnd
{
private:
protected:
	void OnKeypressed( const KeyEvent& evt );
	
public:
	CSetupDisplayWnd( CWindow* parent );
	~CSetupDisplayWnd( void );

	void CreateSetup( void );
};

#endif /* __SETUPGENERAL_WINDOW_H__ */
