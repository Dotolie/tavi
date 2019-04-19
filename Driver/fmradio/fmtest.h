//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Test program header for FM Radio(GNT-702S) on TAVI
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/fmradio/fmtest.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: fmtest.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/03/01 14:09:59  ywkim
// initial revision for testing of fmradio
//
//*****************************************************************************
#ifndef __TAVI_FR_LIB_H__
#define __TAVI_FR_LIB_H__


// Limitation
#define FR_PRESETLIST_MAX			20

#define FR_USFREQUENCY_MIN		87500
#define FR_USFREQUENCY_MAX		108000L

#define FR_JAPANFREQUENCY_MIN		76000
#define FR_JAPANFREQUENCY_MAX		108000L

#define FR_SEARCHTIMEOUT			10


typedef enum
{
	FR_SSL_LOW = 1,
	FR_SSL_MID = 2,		
	FR_SSL_HIGH = 3
} FR_SSL_t;

typedef enum
{
	FR_XTAL_13MHZ = 0,
	FR_XTAL_32KHZ = 1
} FR_XTAL_t;

typedef enum
{
	FR_INJECTION_LO = 0,
	FR_INJECTION_HI = 1,
	FR_INJECTION_CHECKFAIL = 0xff
} FR_INJECTION_t;

typedef enum
{
	FR_STANDBY_OFF= 0,
	FR_STANDBY_ON=1,
} FR_STANDBY_t;

typedef enum
{
	FR_BAND_US = 0,
	FR_BAND_JAPAN = 1
} FR_BAND_t;

typedef enum
{
	FR_TUNEMODE_MANUAL = 0,
	FR_TUNEMODE_PRESET = 1
} FR_TUNEMODE_t;

typedef enum
{
	FR_TUNEDIRECTION_DOWN = 0,
	FR_TUNEDIRECTION_UP = 1
} FR_TUNEDIRECTION_t;

typedef enum
{
	FR_REFERENCEPLL_OFF = 0,
	FR_REFERENCEPLL_ON = 1
} FR_REPERENCEPLLSTATE_t;

typedef enum
{
	FR_STATION_NOTFOUND = 0,
	FR_STATION_FOUND = 1,
	FR_STATION_OUTOFBAND = 0xfe,
	FR_STATION_ACCESSFAIL= 0xff
} FR_SEARCHRESULT_t;

typedef enum
{
	FR_BANDLIMIT_NOTREACHED = 0,
	FR_BANDLIMIT_REACHED = 1
} FR_BANDLIMITRESULT_t;


typedef enum
{
	FR_MUTE_OFF = 0,
	FR_MUTE_ON = 1
} FR_MUTESTATE_t;

typedef enum
{
	FR_LEFTMUTE_OFF = 0,
	FR_LEFTMUTE_ON = 1
} FR_LEFTMUTESTATE_t;

typedef enum
{
	FR_RIGHTMUTE_OFF = 0,
	FR_RIGHTMUTE_ON = 1
} FR_RIGHTMUTESTATE_t;

typedef enum
{
	FR_SEARCH_DOWN = 0,
	FR_SEARCH_UP = 1
} FR_SEARCHDIRECTION_t;

typedef enum
{
	FR_SEARCHMODE_OFF = 0,
	FR_SEARCHMODE_ON= 1
} FR_SEARCHMODE_t;

typedef enum
{
	FR_STEREONOISECANCEL_OFF = 0,
	FR_STEREONOISECANCEL_ON= 1
} FR_STEREONOISESTATE_t;

#endif /* __TAVI_FR_LIB_H__ */
