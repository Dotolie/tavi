//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Sound Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupSound.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupSound.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.3  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.2  2005/04/27 05:30:28  ywkim
// Modify show or onPaint for m-language
//
// Revision 1.1  2005/03/29 01:43:14  ywkim
// Initial revision for sound setup
//
//*****************************************************************************

#ifndef __SETUPSOUND_WINDOW_H__
#define __SETUPSOUND_WINDOW_H__

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"
#include "CSetupWnd.h"

class CSetupSoundWnd : public CSetupWnd
{
private:
protected:
	void OnKeypressed( const KeyEvent& evt );
	
public:
	CSetupSoundWnd( CWindow* parent );
	~CSetupSoundWnd( void );

	void CreateSetup( void );
};

#endif /* __SETUPSOUND_WINDOW_H__ */
