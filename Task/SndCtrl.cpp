//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Sound Control class 

// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/SndCtrl.cpp,v $

// $Revision: 1.3 $

// $Date: 2006/06/22 01:33:13 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: SndCtrl.cpp,v $
// Revision 1.3  2006/06/22 01:33:13  zstein
// - fixed sound balance bug.
//
// Revision 1.2  2006/03/09 01:53:46  zstein
// - added ::VolumeUp() and ::VolumeDown().
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.34  2006/02/15 12:14:23  ywkim
// Remark instructions of SPEAKERAMP_ONOFF in SpeakerMute()
//
// Revision 1.33  2005/12/16 12:07:19  ywkim
// Remove inst. in SetEarphoneVolume()
//
// Revision 1.32  2005/12/16 10:42:40  yong
// add GetNormalRatio() method
//
// Revision 1.31  2005/12/16 09:23:33  ywkim
// Change some instruction in SetEarphoneVolume()
//
// Revision 1.30  2005/12/14 11:33:21  yong
// SetEarphoneVolume() uses sysparam.sound.output_level to adjust sound output
//
// Revision 1.29  2005/12/11 12:56:44  zstein
// *** empty log message ***
//
// Revision 1.28  2005/11/11 12:29:37  zstein
// *** empty log message ***
//
// Revision 1.27  2005/11/03 03:13:06  ywkim
// Exchange left, right for SetBalance
//
// Revision 1.26  2005/10/31 05:18:12  zstein
// *** empty log message ***
//
// Revision 1.25  2005/10/25 03:28:46  zstein
// - changed speaker control
//
// Revision 1.24  2005/10/07 06:47:13  zstein
// - fixed wrong samplingrate.
//
// Revision 1.23  2005/09/28 02:54:23  zstein
// - deleted eq.
//
// Revision 1.22  2005/09/20 12:23:45  zstein
// - update
//
// Revision 1.21  2005/09/14 02:25:59  zstein
// - remove comment
//
// Revision 1.20  2005/08/18 03:09:30  ywkim
// Change instruction for balance
//
// Revision 1.19  2005/07/20 06:12:42  zstein
// - fixed bug
//
// Revision 1.18  2005/07/16 04:30:02  zstein
// - added record routine.
//
// Revision 1.17  2005/06/16 23:51:38  zstein
// - bug fixed.
//
// Revision 1.16  2005/05/27 09:11:38  zstein
// - added SetTreble, SetBass().
//
// Revision 1.15  2005/05/17 20:45:29  ywkim
// Change balance value type
//
// Revision 1.14  2005/05/17 19:15:34  ywkim
// Change volumne control for balance
//
// Revision 1.13  2005/05/04 03:38:20  ywkim
// Add member func. of SetBalance()
//
// Revision 1.12  2005/05/03 11:52:10  ywkim
// Remove BuzzerTick()
//
// Revision 1.11  2005/04/28 12:19:15  etnlwind
// Update for FATFS/PBI Selection
// If you want to change File System Driver, Change tavi/include/define_fs.h
//
// Revision 1.10  2005/04/28 05:58:59  zstein
// demo merge.
//
// Revision 1.9  2005/04/20 11:40:31  zstein
// *** empty log message ***
//
// Revision 1.8  2005/03/29 01:50:41  ywkim
// Add BuzzerTick()
//
// Revision 1.7  2005/03/06 18:43:40  etnlwind
// 사운드라이브러리 수정
//
// Revision 1.6  2005/03/05 20:51:02  zstein
// BUG FIXED: GetVolume()
//
// Revision 1.5  2005/03/05 15:55:19  zstein
// Not Changed.
//
// Revision 1.4  2005/03/05 15:49:52  zstein
// BUG FIXED: SetVolume() -> boundary exception error.
//
// Revision 1.3  2005/03/03 03:18:52  ywkim
// *** empty log message ***
//
// Revision 1.2  2005/02/21 02:13:16  zstein
// *** empty log message ***
//
// Revision 1.1  2005/02/16 16:22:33  zstein
// Sound Control Class
//

//*******************************************************************************

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include "SndCtrl.h"
#include "Task/tavi_global.h"
#include <Driver/sound/sound.h>


/* #### IMPORTANT #### */
#define	OUTPUT_LEVEL_ADJUST				92		// percentage!

CSndCtrl::CSndCtrl( const CString& szDev )
{
	m_fValid = true;
	m_nAttenRight = 100;	// no attenuation
	m_nAttenLeft = 100;	// no attenuation
	m_hSound = open( ( const char* )szDev, O_RDWR );
	if( m_hSound < 0 ) {
		fprintf( stderr, "failed to open sound device: %s\n", strerror(errno) );
		m_fValid = false;
	}

	if( ioctl( m_hSound, TAVI_IOCG_SND_REGISTERS, &m_uzDev ) < 0 ) {
		fprintf( stderr, "failed to get register values from sound device: %s\n", strerror(errno) );
		m_fValid = false;
	}

	m_nLO1Vol = m_uzDev.lineout1.vol_left;
	m_nLO2Vol = m_uzDev.lineout2.vol_left;
	m_halfvol = m_nLO1Vol/2;
}

CSndCtrl::~CSndCtrl( void )
{
	close( m_hSound );
}

bool CSndCtrl::SetEarphoneVolume( int level )
{
	extern SysParameter_t TaviSysParam;

	struct snd_dev_t tDev;

	if( m_fValid == false ) {
		return false;
	}

	if( level >= m_uzDev.lineout1.range ) {
		level = m_uzDev.lineout1.range - 1;
	}

	if( level < 0 ) {
		level = 0;
	}

	m_uzDev.lineout1.vol_left		= level;
	m_uzDev.lineout1.vol_right		= level;

	tDev							= m_uzDev;
	tDev.lineout1.vol_left			= m_uzDev.lineout1.vol_left  * m_nAttenLeft  / 100;
	tDev.lineout1.vol_right			= m_uzDev.lineout1.vol_right * m_nAttenRight / 100;
	
	if (TaviSysParam.sound.output_level == 0) {
		tDev.lineout1.vol_left = (tDev.lineout1.vol_left * OUTPUT_LEVEL_ADJUST)/ 100;
		tDev.lineout1.vol_right = (tDev.lineout1.vol_right * OUTPUT_LEVEL_ADJUST)/ 100;
	}

	if( ioctl(m_hSound, TAVI_IOCS_SND_LINEOUT1_VOL, &tDev ) < 0 ) {
		fprintf( stderr, "failed to control lineout1 volume: %s(%d)\n", strerror(errno), m_hSound );
		return false;
	}

	return true;
}

bool CSndCtrl::SetSpeakerVolume( int level )
{
	struct snd_dev_t tDev;

	if( m_fValid == false ) return false;

	if( m_uzDev.lineout2.range <= level ) level = m_uzDev.lineout1.range-1;
	if( level < 0 ) return false;

	m_uzDev.lineout2.vol_left = level * m_nAttenLeft/100;
	m_uzDev.lineout2.vol_right = level * m_nAttenRight/100;

	tDev = m_uzDev;
	tDev.lineout2.vol_left =m_uzDev.lineout2.vol_left * m_nAttenLeft/100;
	tDev.lineout2.vol_right =m_uzDev.lineout2.vol_right * m_nAttenRight/100;
	
	if( ioctl(m_hSound, TAVI_IOCS_SND_LINEOUT2_VOL, &tDev ) < 0 ) {
		fprintf( stderr, "failed to control lineout1 volume: %s(%d)\n", strerror(errno), m_hSound );
		return false;
	}

	return true;
}

bool CSndCtrl::SpeakerMute( bool fMute )
{
	if( m_fValid == false ) return false;
	if( fMute ) m_uzDev.lineout2.mute = 1;
	else m_uzDev.lineout2.mute = 0;
	if( ioctl(m_hSound, TAVI_IOCS_SND_LINEOUT2_MUTE, &m_uzDev) < 0 ) {
		fprintf( stderr, "failed to control lineout2 mute: %s(%d)\n", strerror(errno), m_hSound );
		return false;
	}
/*
	m_uzDev.speaker = (int)fMute;
	if( ioctl(m_hSound, TAVI_IOCS_SND_SPEAKERAMP_ONOFF, &m_uzDev) < 0 ) {
		fprintf( stderr, "failed to control speaker amp.: %s(%d)\n", strerror(errno), m_hSound );
		return false;
	}
*/
	return true;
}

bool CSndCtrl::EarphoneMute( bool fMute )
{	
	if( m_fValid == false ) return false;
	if( fMute ) m_uzDev.lineout1.mute = 1;
	else m_uzDev.lineout1.mute = 0;
	if( ioctl(m_hSound, TAVI_IOCS_SND_LINEOUT1_MUTE, &m_uzDev) < 0 ) {
		fprintf( stderr, "failed to control lineout1 mute: %s(%d)\n", strerror(errno), m_hSound );
		return false;
	}

	return true;
}

int CSndCtrl::GetNormalRatio( void )
{
	return OUTPUT_LEVEL_ADJUST;
}

int CSndCtrl::GetVolume( void )
{
	// Lineout1 and lineout2 are same
	// left & right same
	return m_halfvol;
}

void CSndCtrl::SetVolume( int level )
{
    int vol;
    if( level < 0 || level > MAX_VOLUME ) {
        return;
    }
    m_halfvol = level;

    vol = m_halfvol*2;
	SetEarphoneVolume( vol );
	SetSpeakerVolume( vol );
}

bool CSndCtrl::VolumeUp( void )
{
    int vol = m_halfvol;
    
    vol++;
    if( vol > MAX_VOLUME ) {
        return false;
    }
    SetVolume( vol );

    return true;
}

bool CSndCtrl::VolumeDown( void )
{
    int vol = m_halfvol;

    vol--;
    if( vol < 0 ) {
        return false;
    }
    SetVolume( vol );

    return true;
}

bool CSndCtrl::LineInBypass( bool fOnOff, int line )
{
	m_uzDev.linein.bypass = (int)fOnOff;
	m_uzDev.linein.line_sel = line;
	if( ioctl( m_hSound, TAVI_IOCS_SND_INPUT_BYPASS, &m_uzDev ) < 0 )
	{           
		return false;
	}

	return true;
}

void CSndCtrl::DACMute( bool fMute ) 
{
	m_uzDev.dac_mute = (fMute) ? 1 : 0;
	ioctl( m_hSound, TAVI_IOCS_SND_DAC_MUTE, &m_uzDev );
}

void CSndCtrl::SetBalance( int nBalance )
{
  nBalance *= 5;
	m_nAttenLeft = m_nAttenRight = 100;

	if ( nBalance > 0 )
		m_nAttenLeft = 100 - nBalance;
	else if ( nBalance < 0 )
		m_nAttenRight = 100 + nBalance;

	SetVolume( m_halfvol );

}

void CSndCtrl::PowerOff( void )
{
	ioctl( m_hSound, TAVI_IOCS_SND_POWER_OFF, &m_uzDev );
}

bool CSndCtrl::SetLineInVolume( int vol )
{
	if( vol > m_uzDev.linein.range-1 ) return false;
	if( vol < 0 ) return false;

	m_uzDev.linein.vol_left = vol;
	m_uzDev.linein.vol_right = vol;
	if( ioctl( m_hSound, TAVI_IOCS_SND_LINEIN_VOL, &m_uzDev ) < 0 ) return false;

	return true;		
}

void CSndCtrl::SetInputSelect( INPUT_Type type )
{
	switch( type ) {
	case INPUT_SEL_FM:
		m_uzDev.linein.line_sel = LINE_SEL_LINE1;
		break;
	case INPUT_SEL_LINEIN:
		m_uzDev.linein.line_sel = LINE_SEL_LINE2;
		break;
	case INPUT_SEL_MIC:
		m_uzDev.linein.line_sel = LINE_SEL_LINE3;
		break;
	default:
		return;
	}
	ioctl( m_hSound, TAVI_IOCS_SND_INPUT_SEL, &m_uzDev );
}

void CSndCtrl::SetADCOn( bool fLeftOn, bool fRightOn )
{
	if( fLeftOn ) m_uzDev.linein.adc_onoff |= (1<<LEFT_ADC_ON);
	else m_uzDev.linein.adc_onoff &= ~(1<<LEFT_ADC_ON);
	if( fRightOn ) m_uzDev.linein.adc_onoff |= (1<<RIGHT_ADC_ON);
	else m_uzDev.linein.adc_onoff &= ~(1<<RIGHT_ADC_ON);

	ioctl( m_hSound, TAVI_IOCS_SND_ADC_ON, &m_uzDev );
}

void CSndCtrl::SetLineInMute( bool onoff )
{
	if( onoff ) m_uzDev.linein.mute = 1;
	else m_uzDev.linein.mute = 0;

	ioctl( m_hSound, TAVI_IOCS_SND_LINEIN_MUTE, &m_uzDev );
}

void CSndCtrl::RecordMode( Record_Type type )
{
	switch( type ) {
	case RECORD_FM: 
		SetupFMRecord();
		break;
	case RECORD_LINEIN:
		SetupLineInRecord();
		break;
	case RECORD_MIC:
		SetupMicRecord();
		break;
	}
}

void CSndCtrl::SetupFMRecord( void )
{
	union sndreg_t reg;

	reg.r.reg = POWER_MGT1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data |= 0x3c; // turn on ADCL/R and AIN L/R.
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	reg.r.reg = ADCL_SIG_PATH<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0xf0; // clear, LINE1
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	reg.r.reg = ADCR_SIG_PATH<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0xf0; // clear, LINE1
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// audio format: left-justified
	reg.r.reg = AUDIO_INF<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x0f; // clear
	reg.r.data |= 0x01; // audio format: left-justified, 16-bit
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// data select: left data to left adc, right data to right adc.
	reg.r.reg = ADD_CTRL1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x0c;  
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// 
	reg.r.reg = LEFTOUT_MIX1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x07;
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );
	reg.r.reg = RIGHTOUT_MIX1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x07;
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );
}

void CSndCtrl::SetupLineInRecord( void )
{
	union sndreg_t reg;

	reg.r.reg = POWER_MGT1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	//reg.r.data |= 0x3c; // turn on ADCL/R and AIN L/R.
	reg.r.data |= 0xfc; // turn on ADCL/R and AIN L/R.
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	reg.r.reg = ADCL_SIG_PATH<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0xf0; // clear
	reg.r.data |= 0x40; // LINE SELECT: LINPUT2
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	reg.r.reg = ADCR_SIG_PATH<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0xf0; // clear
	reg.r.data |= 0x40; // LINE SELECT: RINPUT2
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// audio format: left-justified
	reg.r.reg = AUDIO_INF<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x0f; // clear
	reg.r.data |= 0x01; // audio format: left-justified, 16-bit
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// data select: left data to left adc, right data to right adc.
	reg.r.reg = ADD_CTRL1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x0c;  
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// 
	reg.r.reg = LEFTOUT_MIX1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x07;
	reg.r.data |= 0x01; // line 2
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );
	reg.r.reg = RIGHTOUT_MIX1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x07;
	reg.r.data |= 0x01; // line 2
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	LineInBypass( true );
}

void CSndCtrl::SetupMicRecord( void )
{
	union sndreg_t reg;

	//if( on ) {
		reg.r.reg = POWER_MGT1<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data |= 0x02; // mic-on
		reg.r.data |= 0x08; // turn on ADCL.
		reg.r.data |= 0x20; // turn on AINL
		reg.r.data &= ~0x10; // turn off AINR
		reg.r.data &= ~0x04; // turn off ADCR
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		reg.r.reg = ADCL_SIG_PATH<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data &= ~0xf0; // clear
		reg.r.data |= 0x80; // LINE SELECT: LINPUT3
		reg.r.data |= 0x20; // MIC BOOST 20dB
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		reg.r.reg = NOISE_GATE<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data |= 0x01; // ENABLE NOISE GATE
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		// audio format: left-justified
		reg.r.reg = AUDIO_INF<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data &= ~0x0f; // clear
		reg.r.data |= 0x01; // audio format: left-justified, 16-bit
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		// data select: left data to left adc, right data to left adc and monomix
		reg.r.reg = ADD_CTRL1<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data &= ~0x0c; // clear
		reg.r.data |= 0x04; // left data to left adc, right data to left adc
		reg.r.data |= 0x30; // mono-mix
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );
	//}
	#if 0
	else {
		reg.r.reg = POWER_MGT1<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data = 0xc0;
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		reg.r.reg = ADCL_SIG_PATH<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data &= ~0xf0; // clear
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		reg.r.reg = NOISE_GATE<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data &= ~0x01; // DISABLE NOISE GATE
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		// audio format: left-justified
		reg.r.reg = AUDIO_INF<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data &= ~0x0f; // clear
		reg.r.data |= 0x0a; // audio format: left-justified, 16-bit
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		// data select: left data to left adc, right data to left adc and monomix
		reg.r.reg = ADD_CTRL1<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data = 0xc0;
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );
	}
	#endif
}

int CSndCtrl::GetLineinVolumeRange( void )
{
	return m_uzDev.linein.range;
}

int CSndCtrl::GetLineinVolume( void )
{
	return m_uzDev.linein.vol_left;
}

void CSndCtrl::PlaybackMode( void )
{
	union sndreg_t reg;

	reg.r.reg = POWER_MGT1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	//reg.r.data &= ~0x02; // turn off mic
	//reg.r.data &= ~0x28; // turn off ADCL and AINL
	reg.r.data = 0xc0;
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	reg.r.reg = ADCL_SIG_PATH<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0xf0; // clear
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// turn on DAC L/R
	reg.r.reg = POWER_MGT2<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.reg |= 0x01;
	reg.r.data |= 0x80; // turn on ADCL/R and AIN L/R.
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// disable DAC mute
	reg.r.reg = ADC_DAC_CTRL<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x08; // turn on ADCL/R and AIN L/R.
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// audio format: left-justified
	reg.r.reg = AUDIO_INF<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x0f; // clear
	reg.r.data = 0x0a; // audio format: I2S mode, 24-bit
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	// Disable NOISE GATE
	reg.r.reg = NOISE_GATE<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data &= ~0x01; // DISABLE NOISE GATE
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	reg.r.reg = ADD_CTRL1<<1;
	ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
	reg.r.data = 0xc0;
	ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

	//LineInBypass( false );
}

void CSndCtrl::ALCOn( bool fOnOff, int nChannel )
{
	union sndreg_t reg;
	// ALC1
	if( fOnOff ) {
		reg.r.reg = ALC1 << 1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		switch( nChannel ) {
		case 0: 
			reg.regdata |= 0x10f; // left only
			break;
		case 1: // stereo
			reg.regdata |= 0x18f; 
			break;
		}
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		reg.r.reg = ALC2 << 1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.reg |= 0x01; // hold 2.67ms
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		// NOISE GATE: THD -76.5dBfs, PGA gain constant
		reg.r.reg = NOISE_GATE<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.reg |= 0x01; // enable
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );
	}
	else {
		reg.r.reg = ALC1 << 1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.regdata &= ~0x180;
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		reg.r.reg = ALC2 << 1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data &= ~0x01; // hold 2.67ms
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );

		// NOISE GATE: THD -76.5dBfs, PGA gain constant
		reg.r.reg = NOISE_GATE<<1;
		ioctl( m_hSound, TAVI_IOCG_SND_REGISTER, &reg );
		reg.r.data &= ~0x01; // enable
		ioctl( m_hSound, TAVI_IOCS_SND_REGISTER, &reg );
	}
}

/* FIN */
