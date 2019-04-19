//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Power Window class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupPower.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 07:57:07 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupPower.h,v $
// Revision 1.2  2006/06/07 07:57:07  zstein
// - added power management preset.
// - moved sleep timer to general.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.2  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.1  2005/04/27 05:30:28  ywkim
// Modify show or onPaint for m-language
//
//*****************************************************************************

#ifndef __SETUPPOWER_WINDOW_H__
#define __SETUPPOWER_WINDOW_H__

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"
#include "CSetupWnd.h"


class CSetupPowerWnd : public CSetupWnd
{
private:
protected:
	void OnKeypressed( const KeyEvent& evt );
	
public:
	CSetupPowerWnd( CWindow* parent );
	~CSetupPowerWnd( void );

	void CreateSetup( void );
  void Preset( int preset );
};

#endif /* __SETUPPOWER_WINDOW_H__ */
