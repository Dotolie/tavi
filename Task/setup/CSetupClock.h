//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Clock Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupClock.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupClock.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.3  2005/04/28 06:53:17  ywkim
// Change clocksetup
//
// Revision 1.2  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.1  2005/04/27 05:32:41  ywkim
// Initially make for clock setup
//
//*****************************************************************************

#ifndef __SETUPCLOCK_WINDOW_H__
#define __SETUPCLOCK_WINDOW_H__

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"
#include "CSetupWnd.h"


class CSetupClockWnd : public CSetupWnd
{
private:
protected:
	void OnKeypressed( const KeyEvent& evt );
	
public:
	CSetupClockWnd( CWindow* parent );
	~CSetupClockWnd( void );

	void CreateSetup( void );
};

#endif /* __SETUPCLOCK_WINDOW_H__ */
