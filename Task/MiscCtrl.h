//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Misc. Control Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/MiscCtrl.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: MiscCtrl.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.19  2006/02/17 12:58:15  ywkim
// Remove CHARGER related function or macro
//
// Revision 1.18  2006/02/10 10:59:49  ywkim
// Add selection macro for CHARGER
//
// Revision 1.17  2006/02/10 09:53:09  ywkim
// Remove variable of nIsLCDoff
// Add function of GetLCDOnOff()
//
// Revision 1.16  2006/02/08 09:35:13  ywkim
// Change function of ChargerOnOff()
// Add function of CheckCharger()
// Insert ChargerOnOff() in USBEnable()
//
// Revision 1.15  2006/02/04 05:49:07  ywkim
// Change HostEnable for HW0602
// Add function of ChargerOnOff for HW0602
//
// Revision 1.14  2006/01/24 00:54:44  zstein
// *** empty log message ***
//
// Revision 1.13  2006/01/13 08:24:45  ywkim
// Replaced or Added some function for New H/W, it enabled with compile option HW0601
//
// Revision 1.12  2005/12/20 03:03:44  zstein
// - deleted key deco control methods
//
// Revision 1.11  2005/12/13 07:40:42  yong
// add EXT_EL_Backlit_Status() method
//
// Revision 1.10  2005/12/12 03:39:27  yong
// add convenience method FolderClosed()
//
// Revision 1.9  2005/12/06 08:01:21  ywkim
// Add function of GetValueCtrl_Out()
//
// Revision 1.8  2005/12/03 05:56:09  zstein
// *** empty log message ***
//
// Revision 1.7  2005/10/25 02:40:07  ywkim
// Change name of function to PowerLEDOnOff from FMEnable
//
// Revision 1.6  2005/08/18 13:44:30  ywkim
// add m_bLcdOnOff for MainLcdOnOff
//
// Revision 1.5  2005/08/16 06:13:41  ywkim
// Change func. of LCDBrightness & MainLCDOnOff
//
// Revision 1.4  2005/08/12 03:14:11  zstein
// - T0
//
// Revision 1.3  2005/04/20 11:40:22  zstein
// *** empty log message ***
//
// Revision 1.2  2005/04/14 03:12:44  zstein
// changed i/f.
//
// Revision 1.1  2005/03/06 19:31:49  zstein
// NEW:Miscellaneous Control Class
//
//

//*******************************************************************************

#ifndef __MISCELLANEOUS_CONTROL_CLASS_H__
#define __MISCELLANEOUS_CONTROL_CLASS_H__

#include "../Driver/misc/misc_ctrl.h"

class CMiscCtrl
{
private:
protected:
	int m_hMisc;
	char m_nDirty;
	char m_nBrightness;
	struct misc_ctrl_struct m_uzMiscDev;
	bool m_bLcdOnOff;
	bool m_bEXT_EL_Backlit_Status;	

public:
	CMiscCtrl( const char* szDevPath );
	~CMiscCtrl( void );

	bool Update( char* nDirty );
	
	bool ACDetected( void );
	bool RTC_IRQ( void );
	bool FolderOpen( void );
	bool FolderClosed( void );
	bool CradleIN( void );
	bool HeadphoneIN( void );
	bool PowerOffHold( void );
	bool USBConnected( void );
	bool USBHost_IN( void );

	void MainLCDOnOff( bool onoff );
	void USBEnable( bool fOnOff );
	void PowerOff( void );
	void PowerLEDOnOff( bool fOnOff );
	void AMPEnable( bool fOnOff );

	void EXT_EL_Enable( bool fOnOff ); // Ext. LCD backlight
	bool EXT_EL_Backlit_Status( void );

	void ResetATA( void );

	void LCDBrightness( int level );

	void OutputTest( char data );

	void Mute( bool on );
	char GetValueCtrl_Out( void );
	void SetHddPower( bool fOnOff );
	void HostEnable( bool fOnOff );
       void ClearBit( int bit );
	bool GetLCDOnOff( void ) { return m_bLcdOnOff; }
};

#endif /* __MISCELLANEOUS_CONTROL_CLASS_H__ */
