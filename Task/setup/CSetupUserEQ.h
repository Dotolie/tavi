//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup User EQ Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupUserEQ.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupUserEQ.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/11/25 09:08:33  ywkim
// Add setting function of SetUserEQ, SetUserEffect for sound effect
//
// Revision 1.1  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
//*****************************************************************************

#ifndef __SETUPUSEREQ_WINDOW_H__
#define __SETUPUSEREQ_WINDOW_H__

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"
#include "CSetupWnd.h"
#include "../../3Dsound/3Dsound.h"

enum {
	USEREQ_BAND0 = 0,
	USEREQ_BAND1,
	USEREQ_BAND2,
	USEREQ_BAND3,
	USEREQ_BAND4,	
};

class CSetupUserEQWnd : public CSetupWnd
{
private:
protected:
	int	m_nUserEQ[5];
	void OnKeypressed( const KeyEvent& evt );

public:
	CSetupUserEQWnd( CWindow* parent );
	~CSetupUserEQWnd( void );

	void CreateSetup( void );
	void GetParameter( void );
	void SetParameter( void );
	void SetUserEQ( ePARAM_TYPE eType, int eValue );

};

#endif /* __SETUPUSEREQ_WINDOW_H__ */
