//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Adjust Brightness & Contrast Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupAdjustBrightWnd.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupAdjustBrightWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/12/15 08:29:44  akaiaphanu
// added new component
//
//
//*****************************************************************************

#ifndef __SETUPADJUSTBRIGHTNESS_WINDOW_H__
#define __SETUPADJUSTBRIGHTNESS_WINDOW_H__

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"
#include "CSetupWnd.h"

class CSetupAdjustBrightWnd : public CSetupWnd
{
private:
protected:
	void OnKeypressed( const KeyEvent& evt );
	
public:
	CSetupAdjustBrightWnd( CWindow* parent );
	~CSetupAdjustBrightWnd( void );

	void CreateSetup( void );
};

#endif /* __SETUPADJUSTBRIGHTNESS_WINDOW_H__ */
