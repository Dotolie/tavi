//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Sound control class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/SndCtrl.h,v $
// $Revision: 1.2 $
// $Date: 2006/03/09 02:06:54 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: SndCtrl.h,v $
// Revision 1.2  2006/03/09 02:06:54  zstein
// - improved volume interface
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.15  2006/01/24 07:48:40  zstein
// *** empty log message ***
//
// Revision 1.14  2005/12/16 10:42:40  yong
// add GetNormalRatio() method
//
// Revision 1.13  2005/11/25 13:46:00  zstein
// *** empty log message ***
//
// Revision 1.12  2005/11/25 12:48:15  zstein
// *** empty log message ***
//
// Revision 1.11  2005/10/07 06:48:11  zstein
// - removed unused functions.
//
// Revision 1.10  2005/09/28 02:54:23  zstein
// - deleted eq.
//
// Revision 1.9  2005/07/16 04:30:08  zstein
// *** empty log message ***
//
// Revision 1.8  2005/06/16 23:51:38  zstein
// - bug fixed.
//
// Revision 1.7  2005/05/27 09:12:07  zstein
// - added SetTreble(), SetBass().
//
// Revision 1.6  2005/05/16 05:37:33  zstein
// *** empty log message ***
//
// Revision 1.5  2005/05/04 03:42:36  ywkim
// Add member func. of SetBalance()
//
// Revision 1.4  2005/04/28 05:58:59  zstein
// demo merge.
//
// Revision 1.3  2005/04/20 11:40:22  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/21 02:12:53  zstein
// Update volume & equalizer
//
// Revision 1.1  2005/02/16 16:22:33  zstein
// Sound Control Class
//
//*******************************************************************************

#ifndef __SOUND_CONTROL_H__
#define __SOUND_CONTROL_H__

#include "Component/String.h"
#include "Driver/sound/sound.h"

#define EQ_COUNT		9
#define MAX_VOLUME      40

class CSndCtrl 
{
private:
protected:
    int m_halfvol;
	int m_nLO1Vol;
	int m_nLO2Vol;
	int m_hSound;
	bool m_fValid;
	int m_nAttenRight;		// attenuation value is min 100 & max 0 for right channel 
	int m_nAttenLeft;			// attenuation value is min 100 & max 0 for left
	struct snd_dev_t m_uzDev;

	void SetupFMRecord( void );
	void SetupLineInRecord( void );
	void SetupMicRecord( void );
public:
	
	CSndCtrl( const CString& szDev );
	~CSndCtrl( void );
	typedef enum {
		SNDCTRL_EQ_NORMAL, 
		SNDCTRL_EQ_ROCK,
		SNDCTRL_EQ_POP,
		SNDCTRL_EQ_JAZZ,
		SNDCTRL_EQ_CLASSIC,
		SNDCTRL_EQ_DANCE,
		SNDCTRL_EQ_HEAVY,
		SNDCTRL_EQ_DISCO,
		SNDCTRL_EQ_SOFT,
		SNDCTRL_EQ_3D,
		SNDCTRL_EQ_HALL
	} EQType;
	typedef enum {
		INPUT_SEL_FM,
		INPUT_SEL_LINEIN,
		INPUT_SEL_MIC
	} INPUT_Type;
	typedef enum {
		RECORD_FM,
		RECORD_LINEIN,
		RECORD_MIC,
	} Record_Type;
	bool SetEarphoneVolume( int level );
	int GetEarphoneVolume( void ) { return m_nLO1Vol; }
	bool SetSpeakerVolume( int level );
	int GetSpeakerVolume( void ) { return m_nLO2Vol; }
	bool SpeakerMute( bool fOnOff );
	bool EarphoneMute( bool fOnOff );
	bool LineInBypass( bool fOnOff, int line=0 );
	int GetVolumeRange( void ) { return 40; }
	int GetLineinVolumeRange( void );
	int GetLineinVolume( void );
	int GetVolume( void );
	int GetNormalRatio( void );
	void SetVolume( int vol );
    bool VolumeUp( void );
    bool VolumeDown( void );
	void DACMute( bool fMute );
	void SetBalance( int nBalance );
	void PowerOff( void );
	bool SetLineInVolume( int vol );
	void SetInputSelect( INPUT_Type type );
	void SetADCOn( bool fLeftOn, bool fRightOn );
	void SetLineInMute( bool onoff );
	void RecordMode( Record_Type type );
	void PlaybackMode( void );
	void ALCOn( bool fOnOff, int Channel/* 0: left only, 1: stereo*/ );
    struct snd_dev_t GetDev() const { return m_uzDev; }
};

#endif /* __SOUND_CONTROL_H__ */
