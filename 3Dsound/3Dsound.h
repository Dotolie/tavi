//************************ Copyright(C) New Media Life Inc. ********************
//
// Description : Emersys 3D sound effect header for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/3Dsound/3Dsound.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//******************************************************************************
// $Log: 3Dsound.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.14  2005/10/31 11:44:35  ywkim
// Add function of XEN_ESA_Clear()
//
// Revision 1.13  2005/10/21 01:33:55  ywkim
// Add XEN_SoundProcessWithNoESA() for VideoPlayer
//
// Revision 1.12  2005/10/19 02:07:35  ywkim
// Change .cpp to .c for sharing to kernelmodule
//
// Revision 1.11  2005/09/14 01:54:29  ywkim
// add funciton of void XEN_Init(long *)
//
// Revision 1.10  2005/09/08 02:40:25  ywkim
// Change library with v050906_1 for adding EQ preset
//
// Revision 1.9  2005/08/19 11:20:04  etnlwind
// Update for __ATTRIBUTE_PACKED__ and FileBrowser Delete Func.
//
// Revision 1.8  2005/08/18 09:34:32  etnlwind
// Update for VideoPlayer UserEQ Setup Related
//
// Revision 1.7  2005/08/17 03:43:23  ywkim
// change libXen.a XEN_Interface.h with v050803_1
//
// Revision 1.6  2005/07/21 06:44:51  ywkim
// Change function type void to int*
//
// Revision 1.5  2005/07/21 05:32:29  ywkim
// change libXen.a with v050714
//
// Revision 1.4  2005/07/14 08:08:08  ywkim
// Add XEN_soundProcess2() for pcm
//
// Revision 1.3  2005/06/28 01:44:12  ywkim
// Change libXen.a with 050627 version.
//
// Revision 1.2  2005/06/21 05:12:18  ywkim
// change include path of emx_3d
//
// Revision 1.1  2005/06/17 08:18:31  ywkim
// initially adding for emersys 3d
//
//******************************************************************************
#ifndef __3D_SOUND_H__
#define __3D_SOUND_H__

#include "XEN_Interface.h"

#define __ATTRIBUTE_PACKED__	__attribute__((packed))

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
typedef enum {
	SOUND_NORMAL = 0,
	SOUND_eXTX_PRESET,
	SOUND_MEX_BASS_PRESET,
	SOUND_MEX_LITE_PRESET,
	SOUND_MEX_PRO_PRESET,
	SOUND_MEX_USER,
	SOUND_REX_MILD_PRESET,
	SOUND_REX_WIDE_PRESET,
	SOUND_REX_LIVE_PRESET,
	SOUND_REX_WIDE_USER,
	SOUND_REX_LIVE_USER,	
	SOUND_SEQ_FLAT,
	SOUND_SEQ_VOICE,
	SOUND_SEQ_POWERBASS,
	SOUND_SEQ_BRIGHT,
	SOUND_SEQ_DARKWARM,
	SOUND_SEQ_CLASSIC,
	SOUND_SEQ_HARDROCK,
	SOUND_SEQ_SOFTROCK,
	SOUND_SEQ_JAZZ,
	SOUND_SEQ_DANCE,
	SOUND_SEQ_SLOW,
	SOUND_SEQ_POP,
	SOUND_SEQ_BLUES,
	SOUND_SEQ_INSTRUMENTAL,
	SOUND_SEQ_FUNK,
	SOUND_SEQ_ACOUSTIC,
	SOUND_SEQ_ELECTRONIC,
	SOUND_SEQ_HIPHOP,
	SOUND_SEQ_LATIN,
	SOUND_SEQ_LECTURE,
	SOUND_SEQ_USER,
	SOUND_SEQ_MAXCOUNT
} SOUNDEFFECT_t;
*/
typedef enum {
	SOUND_NORMAL = 0,
	SOUND_REX_MILD_PRESET,
	SOUND_REX_WIDE_PRESET,
	SOUND_REX_LIVE_PRESET,
	SOUND_eXTX_PRESET,
	SOUND_MEX_BASS_PRESET,
	SOUND_MEX_LITE_PRESET,
	SOUND_MEX_PRO_PRESET,

	SOUND_SEQ_ACOUSTIC,
	SOUND_SEQ_BRIGHT,
	SOUND_SEQ_CLASSIC,
	SOUND_SEQ_DANCE,
	SOUND_SEQ_DARKWARM,
	SOUND_SEQ_ELECTRONIC,
	SOUND_SEQ_FLAT,
	SOUND_SEQ_FUNK,
	SOUND_SEQ_HARDROCK,
	SOUND_SEQ_HIPHOP,
	SOUND_SEQ_INSTRUMENTAL,
	SOUND_SEQ_JAZZ,
	SOUND_SEQ_LATIN,
	SOUND_SEQ_LECTURE,
	SOUND_SEQ_POP,
	SOUND_SEQ_BLUES,
	SOUND_SEQ_SLOW,
	SOUND_SEQ_SOFTROCK,
	SOUND_SEQ_VOICE,
	SOUND_SEQ_POWERBASS,

	SOUND_REX_WIDE_USER,
	SOUND_REX_LIVE_USER,		
	SOUND_MEX_USER,
	SOUND_SEQ_USER,
	SOUND_SEQ_MAXCOUNT
} SOUNDEFFECT_t;

typedef enum {
	SET_PROCPARAMETER = 0,
	SET_3DPRESET,
	SET_EQPRESET,
	SET_3DPARAMETER,
	SET_EQPARAMETER,
	SET_LEVELMETER,
	GET_PROCPARAMETER,
	GET_3DPARAMETER,	
	GET_EQPARAMETER,	
} CONTROLMODE_t;

typedef union{
	struct {
		eXEN_PRESET PresetNo;
	} __ATTRIBUTE_PACKED__ S3DPRESET;
	struct {
		eEQ_PRESET PresetNo;
	} __ATTRIBUTE_PACKED__ EQPRESET;
	struct {
		ePARAM_TYPE TypeNo;
		ePARAM_VALUE Value;
	} __ATTRIBUTE_PACKED__ PARAMETER;
	struct {
		ePARAM_TYPE TypeNo;
		ePROC_MODE ModeNo;
	} __ATTRIBUTE_PACKED__ PROCPARAMETER;

} __ATTRIBUTE_PACKED__ EFFECT_t;

typedef struct {
	CONTROLMODE_t	nMode;
	EFFECT_t	Param;
} __ATTRIBUTE_PACKED__ CMD_t;

typedef enum {
	eXENsetPower = 0,
	eXENsetPreset,
	eXENsetEffect,
	eXENsetContentsProp,
	eXENclearESA,
	
	eXENgetPower,
	eXENgetSpectrum,
	eXENgetEffect
} XENPROPIDS_t;

int XEN_GetEffect( CMD_t *p  );
void XEN_SetEffect( CMD_t CMD );
void XEN_Preset( SOUNDEFFECT_t Mode);
void XEN_SoundProcess(unsigned char *pBuffer, unsigned int length);
void XEN_SoundProcessWithNoESA(unsigned char *pBuffer, unsigned int length);
void XEN_ESA_Clear( void );
void XEN_Init(void);
void XEN_Exit(void);
int XEN_Get_UserEQ_BandValue( int BandName );


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __3D_SOUND_H__ */
