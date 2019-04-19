//************************ Copyright(C) New Media Life Inc. ********************
//
// Description : Emersys 3D sound effect for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/3Dsound/3Dsound.c,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//******************************************************************************
// $Log: 3Dsound.c,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.10  2006/01/21 04:47:25  ywkim
// Change structure of OutSpectrum for ESA
//
// Revision 1.9  2006/01/05 05:46:29  ywkim
// Change refresh rate to 1/2
//
// Revision 1.8  2005/12/28 06:07:14  ywkim
// Include SEQ_Coeff.c for custom setting EQ preset
//
// Revision 1.7  2005/12/20 07:33:55  ywkim
// exchange order of function call, firstly AEffectXEN_ESA() & lastly AEffectSoundProcess()
//
// Revision 1.6  2005/12/01 02:58:30  ywkim
// Fix syntax error
//
// Revision 1.5  2005/12/01 02:56:49  ywkim
// Change some parameter of debugmsg
//
// Revision 1.4  2005/10/31 11:44:35  ywkim
// Add function of XEN_ESA_Clear()
//
// Revision 1.3  2005/10/21 01:33:55  ywkim
// Add XEN_SoundProcessWithNoESA() for VideoPlayer
//
// Revision 1.2  2005/10/19 03:34:42  ywkim
// Change DEBUG macro
//
// Revision 1.1  2005/10/19 02:07:35  ywkim
// Change .cpp to .c for sharing to kernelmodule
//
//******************************************************************************
#ifdef MODULE
#include <linux/kernel.h>
#else
#include <stdio.h>
#endif

#include "3Dsound.h"

#define DEBUG
#ifdef DEBUG
static void debug_break (void)
{
}
#ifdef MODULE
#define ASSERT(exp)
#define DEBUGMSG
#else
#define ASSERT(exp)					((void)((exp)?1:(printf ("***ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
//#define DEBUGMSG( fmg, args... )		printf( "[MusicPlayer]"fmt, ## args )
#define DEBUGMSG					printf
#endif // MODULE

#else
#define ASSERT(exp)
#define DEBUGMSG
#endif // DEBUG

#define SWAP_WORD( x )  ( x=((x<<8)&0xff00ff00) | ((x>>8)&0x00ff00ff) )
#define MAX_QUEUED_BUFFERS	42
#define REFRESHRATE				2

#include "SEQ_Coeff.c"

AEFFECT_EMERSYS_INSTANCE *gpInstM3D_XEN;
long SpectrumData[MAX_QUEUED_BUFFERS][10];
long *OutSpectrum;
int	IndexOfSpectrum;

int XEN_GetEffect( CMD_t *p  )
{
	switch ( p->nMode )
	{
		case GET_PROCPARAMETER:
			switch ( p->Param.PROCPARAMETER.TypeNo )
			{
				case XEN_PTYPE_PROC_MODE:
					p->Param.PROCPARAMETER.ModeNo = 
						(ePROC_MODE)AEffectGetParamValue( gpInstM3D_XEN, p->Param.PROCPARAMETER.TypeNo );
					DEBUGMSG ("XEN_GetEffect : PROCPARAM : TypeNo=%d, ModeNo=%d\n",
						p->Param.PROCPARAMETER.TypeNo, p->Param.PROCPARAMETER.ModeNo );

					return 0;
					break;
				default:
					break;
			}
			break;
		case GET_3DPARAMETER:
		case GET_EQPARAMETER:
			switch ( p->Param.PARAMETER.TypeNo )
			{
				case XEN_PTYPE_SURR:
				case XEN_PTYPE_BASS:
				case XEN_PTYPE_TREB:
				case EQ_PTYPE_BAND_0:
				case EQ_PTYPE_BAND_1:
				case EQ_PTYPE_BAND_2:
				case EQ_PTYPE_BAND_3:
				case EQ_PTYPE_BAND_4:
					p->Param.PARAMETER.Value = 
						(ePARAM_VALUE)AEffectGetParamValue( gpInstM3D_XEN, p->Param.PARAMETER.TypeNo );
					DEBUGMSG ("XEN_GetEffect : PARAM : TypeNo=%d, Value=%d\n",
						p->Param.PARAMETER.TypeNo, p->Param.PARAMETER.Value);

					return 0;
					break;
				default:
					break;
			}
			break;
			
		default:
			break;
	}
	return -1;
}

void XEN_SetEffect( CMD_t CMD )
{
	switch ( CMD.nMode )
	{
		case SET_PROCPARAMETER:
			AEffectSetParamValue( gpInstM3D_XEN, CMD.Param.PROCPARAMETER.TypeNo, CMD.Param.PROCPARAMETER.ModeNo);
			DEBUGMSG ("Sound_SetEffect : SET_PROCPARAMETER= %d\n",  CMD.Param.PROCPARAMETER.TypeNo);
			break;

		case SET_3DPRESET:
			AEffectSetPreset( gpInstM3D_XEN, CMD.Param.S3DPRESET.PresetNo);
			DEBUGMSG ("Sound_SetEffect : SET_3DPRESET= %d\n",  CMD.Param.S3DPRESET.PresetNo);
			break;
			
		case SET_EQPRESET:
			AEffectSetPreset( gpInstM3D_XEN, CMD.Param.EQPRESET.PresetNo);
			DEBUGMSG ("Sound_SetEffect : SET_EQPRESET= %d\n",  CMD.Param.EQPRESET.PresetNo);
			break;
			
		case SET_3DPARAMETER:
		case SET_EQPARAMETER:
			AEffectSetParamValue( gpInstM3D_XEN, CMD.Param.PARAMETER.TypeNo, CMD.Param.PARAMETER.Value );
			DEBUGMSG ("Sound_SetEffect : SET_Parameter type= %d, value= %d\n",  CMD.Param.PARAMETER.TypeNo, CMD.Param.PARAMETER.Value );
			break;
			
		case SET_LEVELMETER:
			break;

		default:
			break;

	}
}

int XEN_Get_UserEQ_BandValue( int BandName )
{
	switch ( BandName )
    	{
		case XEN_PTYPE_SURR:
		case XEN_PTYPE_BASS:
		case XEN_PTYPE_TREB:
		case EQ_PTYPE_BAND_0:
		case EQ_PTYPE_BAND_1:
		case EQ_PTYPE_BAND_2:
		case EQ_PTYPE_BAND_3:
		case EQ_PTYPE_BAND_4:
			return AEffectGetParamValue( gpInstM3D_XEN, (ePARAM_TYPE)BandName );
			break;
	}
	return -1;
}

void XEN_Preset( SOUNDEFFECT_t Mode)
{
	CMD_t	eCmd;
	switch( Mode )
	{
		case SOUND_NORMAL:
			DEBUGMSG ("3D : SOUND_NORMAL\n" );
			eCmd.nMode = SET_PROCPARAMETER;
			eCmd.Param.PROCPARAMETER.TypeNo= XEN_PTYPE_PROC_MODE;
			eCmd.Param.PROCPARAMETER.ModeNo= XEN_MODE_NONE;
			break;
		case SOUND_eXTX_PRESET:
			DEBUGMSG ("3D : SOUND_eXTX_PRESET\n" );
			eCmd.nMode = SET_3DPRESET;
			eCmd.Param.S3DPRESET.PresetNo = XEN_eXTX_PRESET;
			break;
		case SOUND_MEX_BASS_PRESET:
			DEBUGMSG ("3D : SOUND_MEX_BASS_PRESET\n" );
			eCmd.nMode = SET_3DPRESET;
			eCmd.Param.S3DPRESET.PresetNo = XEN_MEX_BASS_PRESET;			
			break;
		case SOUND_MEX_LITE_PRESET:
			DEBUGMSG ("3D : SOUND_MEX_LITE_PRESET\n" );
			eCmd.nMode = SET_3DPRESET;
			eCmd.Param.S3DPRESET.PresetNo = XEN_MEX_LITE_PRESET;
			break;
		case SOUND_MEX_USER:
			DEBUGMSG ("3D : SOUND_MEX_USER\n" );
			eCmd.nMode = SET_PROCPARAMETER;
			eCmd.Param.PROCPARAMETER.TypeNo = XEN_PTYPE_PROC_MODE;
			eCmd.Param.PROCPARAMETER.ModeNo = XEN_MODE_HP_MEX;
			break;
		case SOUND_MEX_PRO_PRESET:
			DEBUGMSG ("3D : SOUND_MEX_PRO_PRESET\n" );
			eCmd.nMode = SET_3DPRESET;
			eCmd.Param.S3DPRESET.PresetNo = XEN_MEX_PRO_PRESET;
			break;	
		case SOUND_REX_MILD_PRESET:
			DEBUGMSG ("3D : SOUND_REX_MILD_PRESET\n" );
			eCmd.nMode = SET_3DPRESET;
			eCmd.Param.S3DPRESET.PresetNo = XEN_REX_MILD_PRESET;
			break;
		case SOUND_REX_WIDE_PRESET:
			DEBUGMSG ("3D : SOUND_REX_WIDE_PRESET\n" );
			eCmd.nMode = SET_3DPRESET;
			eCmd.Param.S3DPRESET.PresetNo = XEN_REX_WIDE_PRESET;
			break;
		case SOUND_REX_LIVE_PRESET:
			DEBUGMSG ("3D : SOUND_REX_LIVE_PRESET\n" );
			eCmd.nMode = SET_3DPRESET;
			eCmd.Param.S3DPRESET.PresetNo = XEN_REX_LIVE_PRESET;
			break;
		case SOUND_REX_WIDE_USER:
			DEBUGMSG ("3D : SOUND_REX_WIDE_USER\n" );
			eCmd.nMode = SET_PROCPARAMETER;
			eCmd.Param.PROCPARAMETER.TypeNo = XEN_PTYPE_PROC_MODE;
			eCmd.Param.PROCPARAMETER.ModeNo = XEN_MODE_HP_REX_WIDE;
			break;
		case SOUND_REX_LIVE_USER:
			DEBUGMSG ("3D : SOUND_REX_LIVE_USER\n" );
			eCmd.nMode = SET_PROCPARAMETER;
			eCmd.Param.PROCPARAMETER.TypeNo = XEN_PTYPE_PROC_MODE;
			eCmd.Param.PROCPARAMETER.ModeNo = XEN_MODE_HP_REX_LIVE;
			break;
		case SOUND_SEQ_FLAT:
			DEBUGMSG ("3D : SOUND_SEQ_FLAT\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_FLAT;
			break;
		case SOUND_SEQ_VOICE:
			DEBUGMSG ("3D : SOUND_SEQ_VOICE\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_VOICE;
			break;
		case SOUND_SEQ_POWERBASS:
			DEBUGMSG ("3D : SOUND_SEQ_POWERBASS\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_POWERBASS;
			break;
		case SOUND_SEQ_BRIGHT:
			DEBUGMSG ("3D : SOUND_SEQ_BRIGHT\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_BRIGHT;
			break;	
		case SOUND_SEQ_DARKWARM:
			DEBUGMSG ("3D : SOUND_SEQ_DARKWARM\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_DARKWARM;
			break;
		case SOUND_SEQ_CLASSIC:
			DEBUGMSG ("3D : SOUND_SEQ_CLASSIC\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_CLASSIC;
			break;
		case SOUND_SEQ_HARDROCK:
			DEBUGMSG ("3D : SOUND_SEQ_HARDROCK\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_HARDROCK;
			break;
		case SOUND_SEQ_SOFTROCK:
			DEBUGMSG ("3D : SOUND_SEQ_SOFTROCK\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_SOFTROCK;
			break;
		case SOUND_SEQ_JAZZ:
			DEBUGMSG ("3D : SOUND_SEQ_JAZZ\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_JAZZ;
			break;
		case SOUND_SEQ_DANCE:
			DEBUGMSG ("3D : SOUND_SEQ_DANCE\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_DANCE;
			break;
		case SOUND_SEQ_SLOW:
			DEBUGMSG ("3D : SOUND_SEQ_SLOW\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_SLOW;
			break;
		case SOUND_SEQ_POP:
			DEBUGMSG ("3D : SOUND_SEQ_POP\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_POP;
			break;
		case SOUND_SEQ_BLUES:
			DEBUGMSG ("3D : SOUND_SEQ_BLUES\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_BLUES;
			break;
		case SOUND_SEQ_INSTRUMENTAL:
			DEBUGMSG ("3D : SOUND_SEQ_INSTRUMENTAL\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_INSTRUMENTAL;
			break;
		case SOUND_SEQ_FUNK:
			DEBUGMSG ("3D : SOUND_SEQ_FUNK\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_FUNK;
			break;
		case SOUND_SEQ_ACOUSTIC:
			DEBUGMSG ("3D : SOUND_SEQ_ACOUSTIC\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_ACOUSTIC;
			break;
		case SOUND_SEQ_ELECTRONIC:
			DEBUGMSG ("3D : SOUND_SEQ_ELECTRONIC\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_ELECTRONIC;
			break;
		case SOUND_SEQ_HIPHOP:
			DEBUGMSG ("3D : SOUND_SEQ_HIPHOP\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_HIPHOP;
			break;
		case SOUND_SEQ_LATIN:
			DEBUGMSG ("3D : SOUND_SEQ_LATIN\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_LATIN;
			break;
		case SOUND_SEQ_LECTURE:
			DEBUGMSG ("3D : SOUND_SEQ_LECTURE\n" );
			eCmd.nMode = SET_EQPRESET;
			eCmd.Param.EQPRESET.PresetNo = SEQ_LECTURE;
			break;
		case SOUND_SEQ_USER:
			DEBUGMSG ("3D : SOUND_SEQ_USER\n");
			eCmd.nMode = SET_PROCPARAMETER;
			eCmd.Param.PROCPARAMETER.TypeNo = XEN_PTYPE_PROC_MODE;
			eCmd.Param.PROCPARAMETER.ModeNo = XEN_MODE_HP_EQ;
			break;
		default:
			break;

	}
	XEN_SetEffect ( eCmd );
}

void XEN_SoundProcess(unsigned char *pBuffer, unsigned int length)
{

	int	i;
	unsigned char tmp;
	unsigned int *pData;
	static int	refresh = 0;
#if 1
	pData = (unsigned int *)pBuffer;
	for (i=0; i<length; i++)
		SWAP_WORD(pData[i]);
#else
	for(i=0;i<length*4;i+=2)
	{
		tmp=pBuffer[i];
		pBuffer[i]=pBuffer[i+1];
		pBuffer[i+1]=tmp;
	}
#endif	
	AEffectXEN_ESA((short *) pBuffer, OutSpectrum, length, REFRESHRATE);		// refresh rate = 1/2 refresh/process
	AEffectSoundProcess(gpInstM3D_XEN, (short *) pBuffer,  length);

	if ( refresh++%REFRESHRATE==0 ) {
		IndexOfSpectrum++;
		if (IndexOfSpectrum >= MAX_QUEUED_BUFFERS/REFRESHRATE ) IndexOfSpectrum = 0;
		OutSpectrum = &SpectrumData[IndexOfSpectrum];
		}
#if 1
	for (i=0; i<length; i++)
		SWAP_WORD(pData[i]);
#else
	for(i=0;i<length*4;i+=2)
	{
		tmp=pBuffer[i];
		pBuffer[i]=pBuffer[i+1];
		pBuffer[i+1]=tmp;
	}
#endif
}
void XEN_SoundProcessWithNoESA(unsigned char *pBuffer, unsigned int length)
{

	int	i;
	unsigned char tmp;
	unsigned int *pData;
#if 1
	pData = (unsigned int *)pBuffer;
	for (i=0; i<length; i++)
		SWAP_WORD(pData[i]);
#else
	for(i=0;i<length*4;i+=2)
	{
		tmp=pBuffer[i];
		pBuffer[i]=pBuffer[i+1];
		pBuffer[i+1]=tmp;
	}
#endif	
	AEffectSoundProcess(gpInstM3D_XEN, (short *) pBuffer,  length);
#if 1
	for (i=0; i<length; i++)
		SWAP_WORD(pData[i]);
#else
	for(i=0;i<length*4;i+=2)
	{
		tmp=pBuffer[i];
		pBuffer[i]=pBuffer[i+1];
		pBuffer[i+1]=tmp;
	}
#endif
}
void XEN_ESA_Clear(void)
{
	int	i;
	int	j;

	for (i=0;i<MAX_QUEUED_BUFFERS;i++) {
		for(j=0;j<10;j++)
			SpectrumData[i][j] = 0;
		}
	IndexOfSpectrum = 0;
	OutSpectrum = &SpectrumData[IndexOfSpectrum];
}

void XEN_Init( void )
{
	gpInstM3D_XEN = AEffectInitXEN();
	XEN_ESA_Init();
	DEBUGMSG ("3Dsound gpInstM3D_XEN=0x%x, OutSpectrum=0x%x\n", (int)gpInstM3D_XEN, (int)OutSpectrum);

	XEN_Preset( SOUND_NORMAL );
}

void XEN_Exit(void)
{
	AEffectRelease(gpInstM3D_XEN);
}
