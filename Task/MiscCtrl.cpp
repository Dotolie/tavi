//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Misc. Control Class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/MiscCtrl.cpp,v $

// $Revision: 1.4 $

// $Date: 2006/06/07 08:02:51 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: MiscCtrl.cpp,v $
// Revision 1.4  2006/06/07 08:02:51  zstein
// - fixed a bad sound quality bug when handling volume with jog key.
//
// Revision 1.3  2006/04/07 08:23:50  ywkim
// Change some functions for HOST
//
// Revision 1.2  2006/03/27 11:35:53  zstein
// - compatible old version H/W.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.31  2006/02/17 13:42:23  ywkim
// Remove variable of m_charge_time
//
// Revision 1.30  2006/02/17 12:58:14  ywkim
// Remove CHARGER related function or macro
//
// Revision 1.29  2006/02/10 10:59:49  ywkim
// Add selection macro for CHARGER
//
// Revision 1.28  2006/02/10 09:53:09  ywkim
// Remove variable of nIsLCDoff
// Add function of GetLCDOnOff()
//
// Revision 1.27  2006/02/08 12:07:39  ywkim
// Change USBEnable for charger management
//
// Revision 1.26  2006/02/08 11:16:26  ywkim
// Add macro ifdef/endif in Charger related function for HW0602
//
// Revision 1.25  2006/02/08 09:35:13  ywkim
// Change function of ChargerOnOff()
// Add function of CheckCharger()
// Insert ChargerOnOff() in USBEnable()
//
// Revision 1.24  2006/02/07 11:43:37  ywkim
// Change function of ChargerOnOff() for charger timeout
//
// Revision 1.23  2006/02/04 05:49:07  ywkim
// Change HostEnable for HW0602
// Add function of ChargerOnOff for HW0602
//
// Revision 1.22  2006/01/24 00:54:44  zstein
// *** empty log message ***
//
// Revision 1.21  2006/01/13 08:24:45  ywkim
// Replaced or Added some function for New H/W, it enabled with compile option HW0601
//
// Revision 1.20  2005/12/21 02:48:27  zstein
// *** empty log message ***
//
// Revision 1.19  2005/12/20 03:03:44  zstein
// - deleted key deco control methods
//
// Revision 1.18  2005/12/13 07:40:42  yong
// add EXT_EL_Backlit_Status() method
//
// Revision 1.17  2005/12/12 03:39:27  yong
// add convenience method FolderClosed()
//
// Revision 1.16  2005/12/11 18:26:49  zstein
// *** empty log message ***
//
// Revision 1.15  2005/12/10 06:11:40  ywkim
// Add instruction of ioctl in GetValueCtrl_Out()
//
// Revision 1.14  2005/12/06 08:01:21  ywkim
// Add function of GetValueCtrl_Out()
//
// Revision 1.13  2005/12/03 05:56:09  zstein
// *** empty log message ***
//
// Revision 1.12  2005/10/26 02:02:19  zstein
// - enabled KeyDeco led.
//
// Revision 1.11  2005/10/25 02:40:07  ywkim
// Change name of function to PowerLEDOnOff from FMEnable
//
// Revision 1.10  2005/09/08 08:16:49  ywkim
// Change function of Update for disconnecting USB_Host
//
// Revision 1.9  2005/08/22 10:09:23  ywkim
// Remark KeyDecoEnable()
//
// Revision 1.8  2005/08/18 13:44:30  ywkim
// add m_bLcdOnOff for MainLcdOnOff
//
// Revision 1.7  2005/08/16 06:13:41  ywkim
// Change func. of LCDBrightness & MainLCDOnOff
//
// Revision 1.6  2005/08/12 03:14:11  zstein
// - T0
//
// Revision 1.5  2005/05/17 19:54:53  zstein
// *** empty log message ***
//
// Revision 1.4  2005/04/20 11:40:31  zstein
// *** empty log message ***
//
// Revision 1.3  2005/04/14 03:12:44  zstein
// changed i/f.
//
// Revision 1.2  2005/03/06 23:29:29  zstein
// changed power off
//
// Revision 1.1  2005/03/06 19:31:49  zstein
// NEW:Miscellaneous Control Class
//
//

//*******************************************************************************

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "MiscCtrl.h"

#define pPIO0_DIR 		( volatile int* )( 0x00500600+0x08 )
#define pPIO0_DATA 		( volatile int* )( 0x00500600+0x04 )

#define pPIO1_DIR 		( volatile int* )( 0x00500A00+0x08 )
#define pPIO1_DATA 		( volatile int* )( 0x00500A00+0x04 )

#define MK_REG( _bit, _level ) 		( (0x10000<<_bit) |(_level<<_bit) ) 

CMiscCtrl::CMiscCtrl( const char* szDevPath )
{
	m_hMisc = open( szDevPath, O_RDONLY );

	if( m_hMisc < 0 ) {
		printf( "failed to open misc. contro device driver\n" );
	}

	m_nDirty					= 0;
	m_nBrightness				= 6;
	m_uzMiscDev.ctrl_out		= 0;
	m_uzMiscDev.ctrl_in			= 0;
	m_bLcdOnOff					= true;
	m_bEXT_EL_Backlit_Status	= true;

	if( ioctl( m_hMisc, TAVI_MISC_IOCG_CONTROL, &m_uzMiscDev ) < 0 ) {
		printf( "failed to send command to Misc. control driver\n" );
	}
}

CMiscCtrl::~CMiscCtrl( void )
{
	if( m_hMisc > 0 ) close( m_hMisc );
}

void CMiscCtrl::OutputTest( char data )
{
	m_uzMiscDev.ctrl_out = data;
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

void CMiscCtrl::MainLCDOnOff( bool onoff )
{
	m_bLcdOnOff = onoff;
	if ( onoff ) m_uzMiscDev.brightness=m_nBrightness;
	else{
		if (m_uzMiscDev.brightness!=0) m_nBrightness = m_uzMiscDev.brightness;
		m_uzMiscDev.brightness=0;
		}

	ioctl( m_hMisc, TAVI_MISC_IOCS_BRIGHTNESS, &m_uzMiscDev);
}

void CMiscCtrl::PowerOff( void )
{
#ifdef HW0601
	m_uzMiscDev.ctrl_out |= (1<<MAIN_LCD_ONOFF);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );

	*pPIO0_DATA = MK_REG(10, 0);		// for new hw 
#else
	m_uzMiscDev.ctrl_out &= ~(1<<POWER_OFF);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
#endif
}

void CMiscCtrl::USBEnable( bool fOnOff )
{
	m_uzMiscDev.ctrl_out &= ~(1<<USB_ENABLE);
	m_uzMiscDev.ctrl_out |= ((int)fOnOff << USB_ENABLE );
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

void CMiscCtrl::PowerLEDOnOff( bool fOnOff )
{
#ifdef HW0601
	if ( fOnOff )
		*pPIO0_DATA = MK_REG(9, 0);	// LED on
	else
		*pPIO0_DATA = MK_REG(9, 1);	// LED off
#else
	fOnOff =!fOnOff;
	m_uzMiscDev.ctrl_out &= ~(1<<POWER_LED);
	m_uzMiscDev.ctrl_out |= ((int)fOnOff << POWER_LED);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
#endif	
}

void CMiscCtrl::AMPEnable( bool fOnOff )
{
#ifndef HW0601
    fOnOff = !fOnOff;
#endif
	m_uzMiscDev.ctrl_out &= ~(1<<AMP_ON);
	m_uzMiscDev.ctrl_out |= ((int)fOnOff << AMP_ON);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

void CMiscCtrl::EXT_EL_Enable( bool fOnOff )
{
  if( this->m_bEXT_EL_Backlit_Status == fOnOff ) {
    return;
  }
#ifdef HW0601
	if( fOnOff ) {
		*pPIO1_DATA = MK_REG( 8, 1 );
	}
	else {
		*pPIO1_DATA = MK_REG( 8, 0 );
	}
#else
  if( fOnOff == this->m_bEXT_EL_Backlit_Status ) {
    return;
  }
	// clear bit
	m_uzMiscDev.ctrl_out &= ~(1<<EXT_EL_ON);
	// set bit
	m_uzMiscDev.ctrl_out |= ((int)fOnOff << EXT_EL_ON);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
#endif	
  this->m_bEXT_EL_Backlit_Status = fOnOff;
}

bool CMiscCtrl::EXT_EL_Backlit_Status( void )
{
#ifdef HW0601
	if ( *pPIO1_DATA&0x00000100 )
		return 1;
	else
		return 0;
#else
	bool onoff;
	onoff = (bool)(m_uzMiscDev.ctrl_out&(1<<EXT_EL_ON));
	return onoff;
#endif	
}

bool CMiscCtrl::Update( char* nDirty ) 
{
	static int	HostRemoved=0;
	
	if( m_hMisc < 0 ) return false;

	char old = m_uzMiscDev.ctrl_in;
	if( ioctl( m_hMisc, TAVI_MISC_IOCG_CONTROL, &m_uzMiscDev) < 0 ) return false;

//	printf( "Ctrl IN: 0x%02lx, old=0x%02x\n", (long)m_uzMiscDev.ctrl_in, old );

	if( m_uzMiscDev.ctrl_in & (1<< HOST_IN) )
		m_uzMiscDev.ctrl_in &= ~(1<<USB_CONNECT);

	m_nDirty |= old^m_uzMiscDev.ctrl_in;

	if( ( m_nDirty&0x80 && old&0x80 ) ) {
		HostRemoved = 1;
		}
	if( HostRemoved > 0 ) {
		if ( !(m_uzMiscDev.ctrl_in & (1<<USB_CONNECT))  ) {
			HostRemoved = 0;
			}
//		printf("++ m_nDirty=0x%02x, ctrl_in=0x%02x\n", m_nDirty, m_uzMiscDev.ctrl_in );

		m_uzMiscDev.ctrl_in &= ~(1<<USB_CONNECT);
		m_nDirty &= ~(1<<USB_CONNECT);
		}

	*nDirty = m_nDirty;
	if( m_nDirty ) 
		return true;
	else 
		return false;
}

bool CMiscCtrl::ACDetected( void )
{
	if( m_uzMiscDev.ctrl_in & (1<<AC_DETECT) ) return true;
	return false;
}

bool CMiscCtrl::RTC_IRQ( void )
{
	if( m_uzMiscDev.ctrl_in & (1<<RTC_INTR) ) return false;
	else true;
}

bool CMiscCtrl::FolderOpen( void )
{
	if( m_uzMiscDev.ctrl_in & (1<<FOLDER_OPEN) ) return true;
	return false;
}

bool CMiscCtrl::FolderClosed( void )
{
	return (this->FolderOpen() ? false : true );
}

bool CMiscCtrl::CradleIN( void )
{
	if( m_uzMiscDev.ctrl_in & (1<<CRADLE_IN) ) return false;

	return true;
}

bool CMiscCtrl::HeadphoneIN( void )
{
	if( m_uzMiscDev.ctrl_in&(1<<HP_DETECT) ) return true;
	return false;
}

bool CMiscCtrl::PowerOffHold( void )
{
	if( m_uzMiscDev.ctrl_in&(1<<POWER_ON) ) return false;
	return true;
}

#define PP1	1
bool CMiscCtrl::USBConnected( void )
{
	#if PP1
	if( m_uzMiscDev.ctrl_in&(1<<USB_CONNECT) ) return true;
	return false;
	#else
	long data;
	*pPIO0_DIR = 0x00200000;
	data = *pPIO0_DATA;
	if( data&0x20 ) return true;
	else return false;
	#endif
}

bool CMiscCtrl::USBHost_IN( void )
{
	if( m_uzMiscDev.ctrl_in&(1<<HOST_IN) ) return true;
	return false;
}

void CMiscCtrl::ResetATA( void )
{
	*pPIO0_DATA = 0x00020002;
	*pPIO0_DIR = 0x00020002;
	usleep( 1000 );
	*pPIO0_DATA = 0x00020000;
	usleep( 1000 );
	*pPIO0_DATA = 0x00020002;
}

void CMiscCtrl::LCDBrightness( int level )
{
	if( level < 0 ) return;
	if( level > 10 ) return;
	m_uzMiscDev.brightness = (level==0)?1:level;
	m_nBrightness = m_uzMiscDev.brightness;
	if ( m_bLcdOnOff )
		ioctl( m_hMisc, TAVI_MISC_IOCS_BRIGHTNESS, &m_uzMiscDev);
}

void CMiscCtrl::Mute( bool on )
{
#ifdef HW0601
	on = !on;
	m_uzMiscDev.ctrl_out &= ~(1<<MUTE_ON);
	m_uzMiscDev.ctrl_out |= ((int)on << MUTE_ON);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
#else
	int cmd;
	if( on ) cmd = TAVI_MISC_IOCS_MUTE_ON;
	else cmd = TAVI_MISC_IOCS_MUTE_OFF;
	ioctl( m_hMisc, cmd );
#endif
}

char CMiscCtrl::GetValueCtrl_Out( void )
{
	if( ioctl( m_hMisc, TAVI_MISC_IOCG_CONTROL, &m_uzMiscDev ) < 0 );

	return m_uzMiscDev.ctrl_out;
}

void CMiscCtrl::SetHddPower( bool fOnOff )
{
	if( fOnOff ) {
		*pPIO1_DATA = MK_REG( 7, 1 );		// hdd power on
	}
	else {
		*pPIO1_DATA = MK_REG( 7, 0 );		// hdd power off
	}
}

void CMiscCtrl::HostEnable( bool fOnOff )
{
#ifdef HW0601
#ifdef HW0602
	if ( fOnOff ) {
		*pPIO0_DATA = MK_REG(5, 1);	// Host Power on
		}
	else {
		*pPIO0_DATA = MK_REG(5, 0);	// Host Power off
		}
#else
	if ( fOnOff ) {
		*pPIO0_DATA = MK_REG(0, 1); // Host Power on
		}
	else {
		*pPIO0_DATA = MK_REG(0, 0); // Host Power off
		}
#endif
#endif
}

void CMiscCtrl::ClearBit( int bit )
{
    if( bit < 0 || bit >= 8 ) return;
    m_nDirty &= ~(1<<bit);
}
