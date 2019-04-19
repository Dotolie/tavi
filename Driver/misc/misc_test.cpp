#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "misc_ctrl.h"

#define JASPER_PIO0_BASE		0x00500600
#define     PIO_DATA			0x00000004
#define     PIO_DIR			0x00000008
#define pPIO0_DIR                   ( volatile int* )( JASPER_PIO0_BASE + PIO_DIR )
#define pPIO0_DATA                  ( volatile int* )( JASPER_PIO0_BASE + PIO_DATA )

class CMiscCtrl
{
private:
protected:
	int m_hMisc;
	char m_nDirty;
	struct misc_ctrl_struct m_uzMiscDev;
public:
	CMiscCtrl( const char* szDevPath );
	~CMiscCtrl( void );

	bool Update( char* nDirty );
	
	bool ACDetected( void );
	bool RTC_IRQ( void );
	bool FolderOpen( void );
	bool CradleIN( void );
	bool HeadphoneIN( void );
	bool PowerOffHold( void );
	bool USBConnected( void );
	bool USBHost_IN( void );


	void MainLCDOnOff( bool onoff );
	void USBEnable( bool fOnOff );
	void PowerOff( void );
	void FMEnable( bool fOnOff );
	void AMPEnable( bool fOnOff );
	void EXT_EL_Enable( bool fOnOff ); // Ext. LCD backlight
	void KeyDecoEnable( bool fOnOff );
	void ResetATA( void );

	void OutputTest( char data );
};

CMiscCtrl::CMiscCtrl( const char* szDevPath )
{
	m_hMisc = open( szDevPath, O_RDONLY );
	if( m_hMisc < 0 ) {
		printf( "failed to open misc. contro device driver\n" );
	}
	m_nDirty = 0;

	m_uzMiscDev.ctrl_out = 0;
	m_uzMiscDev.ctrl_in = 0;
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
	m_uzMiscDev.ctrl_out &= ~(1<<MAIN_LCD_ONOFF);
	m_uzMiscDev.ctrl_out |= ((int)onoff<<MAIN_LCD_ONOFF);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

void CMiscCtrl::PowerOff( void )
{
	m_uzMiscDev.ctrl_out &= ~(1<<POWER_OFF);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

void CMiscCtrl::USBEnable( bool fOnOff )
{
	m_uzMiscDev.ctrl_out &= ~(1<<USB_ENABLE);
	m_uzMiscDev.ctrl_out |= ((int)fOnOff << USB_ENABLE );
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

void CMiscCtrl::FMEnable( bool fOnOff )
{
	m_uzMiscDev.ctrl_out &= ~(1<<FM_POWER);
	m_uzMiscDev.ctrl_out |= ((int)fOnOff << FM_POWER);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

void CMiscCtrl::AMPEnable( bool fOnOff )
{
	m_uzMiscDev.ctrl_out &= ~(1<<AMP_ON);
	m_uzMiscDev.ctrl_out |= ((int)fOnOff << AMP_ON);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

void CMiscCtrl::EXT_EL_Enable( bool fOnOff )
{
	m_uzMiscDev.ctrl_out &= ~(1<<EXT_EL_ON);
	m_uzMiscDev.ctrl_out |= ((int)!fOnOff << EXT_EL_ON);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

void CMiscCtrl::KeyDecoEnable( bool fOnOff )
{
	fOnOff = !fOnOff;
	m_uzMiscDev.ctrl_out &= ~(1<<KEY_LED_ON);
	m_uzMiscDev.ctrl_out |= ((int)fOnOff << KEY_LED_ON);
	ioctl( m_hMisc, TAVI_MISC_IOCS_CONTROL, &m_uzMiscDev );
}

bool CMiscCtrl::Update( char* nDirty ) 
{
	if( m_hMisc < 0 ) return false;

	char old = m_uzMiscDev.ctrl_in;
	if( ioctl( m_hMisc, TAVI_MISC_IOCG_CONTROL, &m_uzMiscDev) < 0 ) return false;

	//printf( "Ctrl IN: 0x%02lx\n", (long)m_uzMiscDev.ctrl_in );

	m_nDirty = old^m_uzMiscDev.ctrl_in;

	*nDirty = m_nDirty;
	if( m_nDirty ) return true;
	else return false;
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

bool CMiscCtrl::USBConnected( void )
{
	long data;
	*pPIO0_DIR = 0x00200000;
	data = *pPIO0_DATA;
	if( data&0x20 ) return true;
	else return false;
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

int main( int argc, char** argv )
{
	CMiscCtrl MiscCtrl( "/dev/misc/tmisc" );
	char nDirty;

	// Tri-state IDE Bus
	while( 1 ) {
		if( MiscCtrl.USBConnected() ) {
			*(volatile int *)(0x500B40) = 0x18800;
			// ATA Reset
			MiscCtrl.ResetATA();
			for( int i=0; i>1000; i++ ) usleep (10000 );
			usleep (10000 );
			// Enable USB ATA Bridge
			MiscCtrl.USBEnable( true );
			while( MiscCtrl.USBConnected() ) sleep( 1 );
			// Disable USB ATA Bridge
			MiscCtrl.USBEnable( false );
			// Untristate IDE Bus
			*(volatile int *)(0x500B40) = 0x18000;
			*(volatile int *)(0x500C00) = 0x00;
			//BI_SL_CTRL_REG = PBI_MODE_IDE_PADS;
			for( int i=0; i>100; i++ ) usleep (10000 );
			printf( "usb disconnected\n" );
		}
	}
	
	#if 0
	char c = *argv[1] - '0';
	char v = *argv[2] - '0';
	printf( "c: %d, v: %d\n", c, v );
	char data = v<<c;
	printf( "data: 0x%02lx\n", data );
	MiscCtrl.OutputTest( data );
	#endif
	#if 0
	while( 1 ) {
		usleep( 2000000 ); // sleep 200ms
		if( !MiscCtrl.Update() ) continue;

		if( MiscCtrl.ACDetected() ) {
			printf( "AC Detected\n" );
		}
		else {
			printf( "AC Disconnected\n" );
		}

		if( MiscCtrl.FolderOpen() ) {
			printf( "Folder open\n" );
		}
		else {
			printf( "Folder close\n" );
		}

		if( MiscCtrl.RTC_IRQ() ) {
			printf( "RTC Interrupted\n" );
		}
		else {
			printf( "RTC not interrupted yet\n" );
		}

		if( MiscCtrl.CradleIN() ) {
			printf( "Cradle connected\n" );
		}
		else {
			printf( "Cradle disconnected\n" );
		}

		if( MiscCtrl.HeadphoneIN() ) {
			printf( "Headphone connected\n" );
		}
		else {
			printf( "Headphone disconnected\n" );
		}

		if( MiscCtrl.PowerOffHold() ) {
			printf( "PowerOff/Hold pressed\n" );
		}
		else {
			printf( "PowerOff/Hold released\n" );
		}
	}
	#endif
	
	return 0;
}
