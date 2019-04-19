//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Output Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupOutput.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupOutput.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.1  2005/07/18 06:48:16  ywkim
// Add item of Output & change other items
//
//*****************************************************************************

#ifndef __SETUPOUTPUT_WINDOW_H__
#define __SETUPOUTPUT_WINDOW_H__

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"
#include "CSetupWnd.h"

class CSetupOutputWnd : public CSetupWnd
{
private:
protected:
	void OnKeypressed( const KeyEvent& evt );
	
public:
	CSetupOutputWnd( CWindow* parent );
	~CSetupOutputWnd( void );

	void CreateSetup( void );
};

#endif /* __SETUPOUTPUT_WINDOW_H__ */
