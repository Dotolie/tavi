//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup User Effect Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupUserEffect.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: CSetupUserEffect.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/12/01 06:16:21  ywkim
// Add function of SetTemporaryPreset()
//
// Revision 1.2  2005/11/25 09:08:33  ywkim
// Add setting function of SetUserEQ, SetUserEffect for sound effect
//
// Revision 1.1  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
//*****************************************************************************

#ifndef __SETUPUSEREFFECT_WINDOW_H__
#define __SETUPUSEREFFECT_WINDOW_H__

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"
#include "CSetupWnd.h"
#include "../../3Dsound/3Dsound.h"

enum {
	EFFECT_MEXUSER = 0,
	EFFECT_REXWIDE,
	EFFECT_REXLIVE,
	EFFECT_MAXCOUNT
};

class CSetupUserEffectWnd : public CSetupWnd
{
private:
protected:
	int	m_nUserEffect;	
	int 	m_nSurround[3];
	int	m_nBass[3];

	void OnKeypressed( const KeyEvent& evt );
	
public:
	CSetupUserEffectWnd( CWindow* parent );
	~CSetupUserEffectWnd( void );

	void CreateSetup( void );
	void GetParameter( void );
	void SetParameter( void );
	void UpdateParameter(void );
	void SetUserEffect( ePARAM_TYPE eType, int eValue );
	void SetTemporaryPreset( int Effect );

};

#endif /* __SETUPUSEREFFECT_WINDOW_H__ */
