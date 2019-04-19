//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Test program for FM Radio(GNT-702S) on TAVI
// 
// $Source: /home/cvsroot/TAVI020/tavi/Driver/fmradio/fmtest.c,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: fmtest.c,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/03/01 14:09:59  ywkim
// initial revision for testing of fmradio
//
//*****************************************************************************

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "fmtest.h"

#define DEBUG

#ifdef DEBUG
	#define debug_outs	printf
	static void debug_break (void)
	{
	}
	#define ASSERT(exp)				((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
	#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))
#else
	#define ASSERT(exp)
	#define DEBUGMSG(cond,printf_exp)
#endif

#define FR_DEV_PATH			"/dev/misc/fm"

typedef union {
    struct {
        unsigned char cByte1;
        unsigned char cByte2;
        unsigned char cByte3;
        unsigned char cByte4;
        unsigned char cByte5;
    } Byte5Type;
    struct {
        unsigned        pllhi:6;    	// cByte1[5:0] 0~63		pll[15:8]
        unsigned        blf:1;       	// cByte1[6]			band limit flag (1:reached to band limit
        unsigned        rf:1;     	  	// cByte1[7]			ready flag (1:found station)

        unsigned char plllo;      	// cByte2[7:0]  255~0	pll[7:0]

        unsigned        ifc:7;     	// cByte3[6:0]			if counter result [6:0]
        unsigned        stereo:1;      // cByte3[7]			stereo indication (1:stereo reception)

        unsigned        none0:1;      // cByte4[0]			not used
        unsigned        ci:3;       	// cByte4[3:1]			chip identification
        unsigned        lev:4;     	// cByte4[7:4]			level adc 
        
        unsigned char none1;   	// cByte5				not used
    } RxFormatType;
} FMRXDATA_TAG;

typedef union {
    struct {
        unsigned char cByte1;
        unsigned char cByte2;
        unsigned char cByte3;
        unsigned char cByte4;
        unsigned char cByte5;
    } Byte5Type;
    struct {
        unsigned        pllhi:6;    // cByte1[5:0] 0~63	pll[15:8]
        unsigned        sm:1;       // cByte1[6]			(1:search mode)
        unsigned        mute:1;     // cByte1[7]			(1:mute)

        unsigned char plllo;      // cByte2[7:0]  255~0	pll[7:0]

        unsigned        swp1:1;     // cByte3[0]			software programmable port1
        unsigned        mr:1;       // cByte3[1]			(1:mute right & force mono)
        unsigned        ml:1;       // cByte3[2]			(1:mute left & force mono)
        unsigned        ms:1;       // cByte3[3]			mono/stereo (1:mono , 0:stereo)
        unsigned        hlsi:1;     // cByte3[4]			hi/low side injection (1: high side, 0:low)
        unsigned        ssl:2;      // cByte3[6:5]  0,32,64,96	search stop level
        						//					00:not allowed in search mode
        						//					01:low		adc out = 5
        						//					10:middle	adc out = 7
        						//					11:high		adc out =10
        unsigned        sud:1;      // cByte3[7]			search up / down (1: up, 0: down)

        unsigned        si:1;       // cByte4[0]			search indicator (1: swp1 is search flag out, 0: swp1 is swp1)
        unsigned        snc:1;      // cByte4[1]			stereo noise cancelling (1:on, 0:off)
        unsigned        hcc:1;      // cByte4[2]			high cut control (1: on, 0:off)
        unsigned        smute:1;    // cByte4[3]		software mute (1:mute on, 0:mute off)
        unsigned        xtal:1;     // cByte4[4]			xtal (1: f-32khz, 0:f-13Mhz)
        unsigned        bl:1;       // cByte4[5]			band limit (1: japan, 0:us.euc)
        unsigned        stby:1;     // cByte4[6]			(1:standby, 0:normal)
        unsigned        swp2:1;     // cByte4[7]			software programmable port2 (1:high, 0:low)

        unsigned        none:6;     // cByte5[5:0] 0~63	not used
        unsigned        dtc:1;      // cByte5[6]			de-emphasis time constant (1: 75us, 0:50us
        unsigned        pllref:1;   // cByte5[7]			pll reference enable (1:enable 6.5Mhz)
    } TxFormatType;
} FMTXDATA_TAG;


static int handle_fm;
static int initialized;
static FR_INJECTION_t	current_Injectionside;
static unsigned int nPresetList[FR_PRESETLIST_MAX];
static unsigned int 	current_freq;
static int nIndex_PresetList;


static FMTXDATA_TAG tTxDataBuff;
static FMRXDATA_TAG tRxDataBuff;

// FM Module Operations 
// For more detail description, Plz. refer to ntea5767.pdf



char
FR_InitPresetList( void )
{
	bzero( nPresetList, sizeof(nPresetList[FR_PRESETLIST_MAX]) );
}

unsigned int
FR_GetPreset( char cIndex )
{
	if( cIndex >= FR_PRESETLIST_MAX )
		return -1;					// out of index range : 20
	return(nPresetList[cIndex]);
}

char
FR_AddPreset( char cIndex, unsigned int tFrequency )
{
	if ( cIndex >= FR_PRESETLIST_MAX ) 
		return -1;					// out of index range :20

	nPresetList[cIndex] = tFrequency;
	
	return 0;
}

char
FR_DeletePreset( char cIndex )
{
	if( cIndex >= FR_PRESETLIST_MAX )
		return -1;					// out of index range : 20

	nPresetList[cIndex] = 0;

	return 0;
}

char
FR_GetPresetList( void )
{
	char	cIndex;
	int	nMajor;
	int	nMinor;
	
	for(cIndex=0;cIndex<FR_PRESETLIST_MAX;cIndex++)
	{
	
		nMajor =nPresetList[cIndex]/1000;
		nMinor =nPresetList[cIndex]-nMajor*1000;
		printf("Preset List : Ch%02d %03d.%02d \n", cIndex,nMajor,nMinor );
	}
	return 0;
}

char
FR_SetStandby( FR_STANDBY_t tStandby )
{
	FMTXDATA_TAG *pTxBuf;
	int nResult;
	
	pTxBuf = &tTxDataBuff;
	pTxBuf->TxFormatType.stby = tStandby;

	nResult = write( handle_fm, pTxBuf, 0, NULL);
	if (nResult <0) 
		return -1;											// write fail

	return 0;
}

char
FR_SetMute( FR_MUTESTATE_t tMute )
{
	FMTXDATA_TAG *pTxBuf;
	int nResult;
	
	pTxBuf = &tTxDataBuff;
	pTxBuf->TxFormatType.mute = tMute;
	pTxBuf->TxFormatType.sm = FR_SEARCHMODE_OFF;	// clear search command

	nResult = write( handle_fm, pTxBuf, 0, NULL);
	if (nResult <0) 
		return -1;											// write fail

	return 0;
}

char
FR_GetLevel( void )
{
	FMRXDATA_TAG *pRxBuf;
	int	nResult;
	
	pRxBuf = &tRxDataBuff;
	
	nResult = read( handle_fm, pRxBuf, 0, NULL);
	if (nResult <0)
		return -1;									// read fail

	return ( pRxBuf->RxFormatType.lev );
}


char
FR_SetPll( FR_INJECTION_t tInjectionSide, unsigned int tFreq )
{
	FMTXDATA_TAG *pTxBuf;
	FMRXDATA_TAG *pRxBuf;
	unsigned int	nFreq;
	int 	nResult;
	
	pTxBuf = &tTxDataBuff;
	pRxBuf = &tRxDataBuff;
	
	
	if (tInjectionSide == FR_INJECTION_HI)
		nFreq = (4000*(tFreq+225))/32768;				// freq of High side injection
	else	 	// FR_INJECTION_LO
		nFreq = (4000*(tFreq-225))/32768;				// freq of low side injection freq


	pTxBuf->TxFormatType.pllhi = ((nFreq>>8) & 0x3f);	// high part of pll[15:8] freq
	pTxBuf->TxFormatType.plllo = (nFreq & 0xff);			// low part of pll[7:0]  freq
	pTxBuf->TxFormatType.sm = FR_SEARCHMODE_OFF;	// clear search command
	pTxBuf->TxFormatType.hlsi = tInjectionSide;			// set high / low side injection
	current_Injectionside = tInjectionSide;			// save injection side

	nResult = write( handle_fm, pTxBuf, 0, NULL);
	if (nResult <0) 
		return -1;									// write fail

	tFreq = ((tFreq + 50)/100)*100;

	current_freq = tFreq;								// save freq

	DEBUGMSG(0,( "FR_SetPll:\n 0x%x,0x%x,0x%x,0x%x,0x%x \n", 
		pTxBuf->Byte5Type.cByte1,
		pTxBuf->Byte5Type.cByte2,
		pTxBuf->Byte5Type.cByte3,
		pTxBuf->Byte5Type.cByte4,
		pTxBuf->Byte5Type.cByte5
		)
	);
	return 0;										// set ok
}


char
FR_CheckOptimalInjection( unsigned int tFreq )
{
	FMTXDATA_TAG *pTxBuf;
	FMRXDATA_TAG *pRxBuf;

	char nLevelHigh;
	char nLevelLow;

	
	FR_SetPll(FR_INJECTION_HI, tFreq+225);		// set freq of High side image
	nLevelHigh = FR_GetLevel();
	FR_SetPll(FR_INJECTION_LO, tFreq-225);		// set freq of low side image
	nLevelLow = FR_GetLevel();
	
	if (nLevelHigh < nLevelLow ) 
		return FR_INJECTION_HI;					// set high side injection
	else
		return FR_INJECTION_LO;					// set low side injection

}

int
FR_TunningFreq( int tFreq )
{

	FMTXDATA_TAG *pTxBuf;
	FMRXDATA_TAG *pRxBuf;
	FR_INJECTION_t tInjectionSide;
	int nResult;
	char cLevel;
	
	pTxBuf = &tTxDataBuff;
	pRxBuf = &tRxDataBuff;

	if( FR_CheckOutOfBand(tFreq) < 0 ) 
		return -1;								// this freq is out of band

	tInjectionSide = FR_CheckOptimalInjection( tFreq );
	nResult = FR_SetPll( tInjectionSide, tFreq );		// check Optimal injection side
												// tunning with low side injection
	if( nResult < 0 )
		return -1;								// tunning fail

	cLevel = FR_GetLevel();
	if ( cLevel > 4 )
		return 0;								// fine tune
	else 
		return 1;								// low gain
}

int 
FR_PrintData( void )
{
	FMRXDATA_TAG *pRxBuf;
	unsigned int nFreq;
	int nResult;
	int nPll;
	int tPll;
	
	pRxBuf = &tRxDataBuff;

	nResult = read( handle_fm, pRxBuf, 0, NULL);
	if (nResult <0)
		return -1;									// read fail

	nPll = (pRxBuf->RxFormatType.plllo | (pRxBuf->RxFormatType.pllhi << 8 )) * 32769 / 4000;
	if( current_Injectionside == FR_INJECTION_HI )
		tPll = nPll -225;
	else
		tPll = nPll + 225;

	nFreq = ((current_freq + 50)/100)*100;

	printf( ">ready flag : %d\n", pRxBuf->RxFormatType.rf );
	printf( ">bandlimit  : %d\n", pRxBuf->RxFormatType.blf );
	printf( ">pll (hex)  : 0x%x\n", nPll );
	printf( ">pll (dec)  : %d\n", nPll);
	printf( ">tpll (dec) : %d\n", tPll);
	printf( ">IF count   : 0x%x\n", pRxBuf->RxFormatType.ifc );
	printf( ">stereo     : %d\n", pRxBuf->RxFormatType.stereo );
	printf( ">level      : %d\n", pRxBuf->RxFormatType.lev );
	printf( ">Injection  : %d\n", current_Injectionside);
	printf( ">currertfreq: %d\n", nFreq);
	return 0;										// read ok
}

int
FR_CheckOutOfBand( unsigned int nFreq )
{
	FMTXDATA_TAG *pTxBuf;

	pTxBuf = &tTxDataBuff;


	if( pTxBuf->TxFormatType.bl == FR_BAND_US )	// US, EUC Band limited
	{
	
		if( nFreq >= FR_USFREQUENCY_MIN && nFreq <= FR_USFREQUENCY_MAX ) 
		{
			return 0;							// allowd freq
		}
		else
		{
			return -1;							// disallowd 
		}
	}
	else											// JAPAN band limited
	{
		if( nFreq >= FR_JAPANFREQUENCY_MIN && nFreq <= FR_JAPANFREQUENCY_MAX )
			return 0;							// allowed freq
		else
			return -1;							// diaallowd
	}
}


int
FR_GetPll( void )
{
	FMRXDATA_TAG *pRxBuf;
	int nResult;
	int sPll;
	int tPll;
	
	sPll = 0;
	pRxBuf = &tRxDataBuff;

	nResult = read( handle_fm, pRxBuf, 0, NULL);
	if (nResult <0)
		return -1;										// read fail
		
	sPll = (pRxBuf->RxFormatType.plllo | (pRxBuf->RxFormatType.pllhi << 8))*32768/4000;


	if( current_Injectionside == FR_INJECTION_HI )
		tPll = sPll -225;
	else
		tPll = sPll + 225;

	return tPll;								// return current freq
}


FR_SEARCHRESULT_t
FR_SearchStation( FR_SEARCHDIRECTION_t tDir, FR_SSL_t tSsl )
{
	FMTXDATA_TAG *pTxBuf;
	FMRXDATA_TAG *pRxBuf;
	FR_INJECTION_t tInjectionSide;
	int tFrequency;
	int nResult;
	int nTimeOut;

	nTimeOut = FR_SEARCHTIMEOUT;					// set search timer
	pTxBuf = &tTxDataBuff;
	pRxBuf = &tRxDataBuff;
#if 1
	if ( tDir == FR_SEARCH_UP )
		tFrequency = current_freq + 100;				// for europian
	else		// FR_SEARCH_DOWN
		tFrequency = current_freq - 100;				// for europian
#else
		tFrequency = current_freq;
#endif
	
	if (current_Injectionside == FR_INJECTION_HI)
		tFrequency = (4000*(tFrequency + 225))/32768;	// freq of High side injection
	else	 	// FR_INJECTION_LO
		tFrequency = (4000*(tFrequency -225))/32768;		// freq of low side injection freq

	pTxBuf->TxFormatType.pllhi = ((tFrequency >>8) & 0x3f);	// high part of pll[15:8] freq
	pTxBuf->TxFormatType.plllo = (tFrequency & 0xff);			// low part of pll[7:0]  freq
	
	pTxBuf->TxFormatType.mute = FR_MUTE_ON;			// mute on
	pTxBuf->TxFormatType.sm = FR_SEARCHMODE_ON;
	pTxBuf->TxFormatType.ssl = tSsl;					// set search stop level
	pTxBuf->TxFormatType.sud = tDir;					// set search direction

	pTxBuf->TxFormatType.hlsi = current_Injectionside;
	
	nResult = write( handle_fm, pTxBuf, 0, NULL);
	if (nResult <0) 
		return FR_STATION_ACCESSFAIL;				// write fail


	while ( nTimeOut )
	{
		usleep(100000);								// delay 100ms
		nResult = FR_GetPll();
		if ( nResult < 0)
			return FR_STATION_ACCESSFAIL;			// read fail
			
		if ( pRxBuf->RxFormatType.rf == FR_STATION_FOUND)
			break;
		nTimeOut--;									// decrease timer
	};

	
	tFrequency = FR_GetPll();
	
	if ( pRxBuf->RxFormatType.blf == FR_BANDLIMIT_REACHED )
	{
		return FR_STATION_OUTOFBAND;
	};
	
	if( FR_CheckOutOfBand( tFrequency ) < 0 ) 
		return FR_STATION_OUTOFBAND;				// out of band

	tInjectionSide = FR_CheckOptimalInjection( tFrequency );
	nResult = FR_SetPll( tInjectionSide, tFrequency );

	DEBUGMSG(0,( "FR_SearchStation:\n TimeOut:%d, ready flag:%d, band limit:%d, ifc:0x%x, cfreq:%d, lev:%d\n", 
		nTimeOut,
		pRxBuf->RxFormatType.rf,
		pRxBuf->RxFormatType.blf,
		pRxBuf->RxFormatType.ifc,
		current_freq,
		pRxBuf->RxFormatType.lev
		)
	);

	FR_SetMute( FR_MUTE_OFF );						// mute off
	return FR_STATION_FOUND;						// find station
}

int 
FR_TunningUpDown( FR_TUNEDIRECTION_t nDirection, FR_TUNEMODE_t tMode )
{
	FMRXDATA_TAG *pRxBuf;
	FR_INJECTION_t tInjectionSide;
	int 	tFrequency;
	int 	nResult;
	int 	nTimeOut = FR_SEARCHTIMEOUT;

	pRxBuf = &tRxDataBuff;
	tFrequency = current_freq;
	
	if ( tMode == FR_TUNEMODE_PRESET )
	{
		if ( nDirection == FR_TUNEDIRECTION_UP) 
			nIndex_PresetList++;
		else			// nDirection == FR_TUNEDIRECTION_DOWN
			nIndex_PresetList--;
		
		if ( nIndex_PresetList >= FR_PRESETLIST_MAX ) 
			nIndex_PresetList = FR_PRESETLIST_MAX -1;
		else if ( nIndex_PresetList < 0 )
			nIndex_PresetList = 0;

		tFrequency = nPresetList[nIndex_PresetList];
	}
	else 		// tMode == FR_TUNEMODE_MANUAL
	{

		if( nDirection == FR_TUNEDIRECTION_UP )
			tFrequency = current_freq + 100;				// increase 100khz
		else		// nDirection == FR_TUNEDIRECTION_DOWN
			tFrequency = current_freq -100;				// decrease 100khz
		
		if( FR_CheckOutOfBand( tFrequency ) < 0 ) 
			return -2;
	}

	tInjectionSide = FR_CheckOptimalInjection( tFrequency );
	nResult = FR_SetPll( tInjectionSide, tFrequency );

	DEBUGMSG(1,( "FR_TunningUpDown:\n nResult:%d, Injection:%d, cfreq:%d \n", 
		nResult,
		tInjectionSide,
		current_freq)
	);
	
	return 0;								// Tune ok
}

int
FR_AutoScan( void )
{
	FMTXDATA_TAG *pTxBuf;
	FMRXDATA_TAG *pRxBuf;
	FR_INJECTION_t tInjectionSide;
	FR_SEARCHRESULT_t	tResult;
	int	nResult;
	int 	nStartFreq;
	int 	nEndFreq;
	int 	nCurrentFreq;
	char cIndex;
	
	//unsigned char status, curlev, curifc;
	
	pTxBuf = &tTxDataBuff;
	pRxBuf = &tRxDataBuff;
	cIndex = 0;						// clear index

	if( pTxBuf->TxFormatType.bl == FR_BAND_US )
	{
		nStartFreq = FR_USFREQUENCY_MIN;
		nEndFreq = FR_USFREQUENCY_MAX;
	}
	else
	{
		nStartFreq = FR_JAPANFREQUENCY_MIN;
		nEndFreq = FR_JAPANFREQUENCY_MAX;
	}
	
	nCurrentFreq = nStartFreq;

	tInjectionSide = FR_CheckOptimalInjection( nStartFreq );
	nResult = FR_SetPll( tInjectionSide, nStartFreq );
	if ( nResult < 0 )
		return -1;								// freq set fail
		
	while( nCurrentFreq <= nEndFreq )
	{
		tResult = FR_SearchStation( FR_SEARCH_UP, FR_SSL_MID );

		nCurrentFreq = FR_GetPll();
		
		if ( tResult == FR_STATION_OUTOFBAND )
		{
			return 1;							// band limit
		}
		else if ( tResult == FR_STATION_FOUND )
		{
			DEBUGMSG(1,( "FR_AutoScan:\n eFreq:%d, cFreq:%d, Injection:%d \n", 
					nEndFreq,
					current_freq,
					current_Injectionside
					)
			);
			// do add preset list
			FR_AddPreset( cIndex++, current_freq );
		}
	}

	return 0;									// complete sacn
}


char
FR_SetInitial( FR_BAND_t tBandLimit, unsigned int nFreq )
{
	int nResult;
	FR_INJECTION_t tInjectionSide;

	if( initialized ) 
		return 0;

	memset( (void*)&tTxDataBuff, 0, sizeof(tTxDataBuff) );
	memset( (void*)&tRxDataBuff, 0, sizeof(tRxDataBuff) );

	tTxDataBuff.TxFormatType.mute = FR_MUTE_ON;			// mute on
	tTxDataBuff.TxFormatType.stby = FR_STANDBY_ON;		// standby mode
	tTxDataBuff.TxFormatType.pllref = FR_REFERENCEPLL_ON;	// reference pll enable
	tTxDataBuff.TxFormatType.bl = tBandLimit;				// set band limit
	tTxDataBuff.TxFormatType.xtal = FR_XTAL_32KHZ;			// xtal 32khz
	tTxDataBuff.TxFormatType.hcc = 1;						// high cut control off
	tTxDataBuff.TxFormatType.snc = 1;						// stereo noise cancelling off
	tTxDataBuff.TxFormatType.smute = 0;					// software mute off

	tInjectionSide = FR_CheckOptimalInjection( nFreq );
	nResult = FR_SetPll( tInjectionSide, nFreq );
	
	FR_InitPresetList();									// initialize Preset list
	nIndex_PresetList = 0;								// init index of preset list

	initialized = 1;

	return 0;
}

int 
main( int argc, char** argv )
{
	char cmd[100] = {0,};
	int ret;

	
	handle_fm = open( FR_DEV_PATH, O_RDWR );
	if( handle_fm < 0 ) 
		return -1;

	FR_SetInitial( FR_BAND_US, 89100 );

	while( 1 ) {
		printf( "cmd> " );
		fflush( stdout );
		gets( cmd );
		if( !strncmp( cmd, "on", 2 ) )
		{
			FR_SetStandby( FR_STANDBY_OFF );		// tune on
		}
		else if( !strncmp( cmd, "mute", 2 ) ) 
		{
			printf( "1 -> mute on, 0 -> mute off: " );
			memset( cmd, 0, sizeof(cmd) );
			gets( cmd );
			FR_SetMute( atoi(cmd) );					// mute off
		}
		else if( !strncmp( cmd, "off", 2 ) ) 
		{
			FR_SetMute( FR_MUTE_ON );				// mute off
			FR_SetStandby( FR_STANDBY_ON);			// enter standby mode
		}
		else if( !strncmp( cmd, "freq", 2 ) )				// set freq
		{
			printf( "freq> " );
			memset( cmd, 0, sizeof(cmd) );
			gets( cmd );
			ret = FR_TunningFreq( atoi(cmd) );
			printf( "freq -> %d\n", ret);
		}
		else if( !strncmp( cmd, "level", 2 ) )				// freq up down
		{
			printf( "1 -> up, 0 -> down: " );
			memset( cmd, 0, sizeof(cmd) );
			gets( cmd );
			ret = FR_TunningUpDown( atoi(cmd), FR_TUNEMODE_MANUAL );
			printf( "level -> %d\n", ret );
		}
		else if( !strncmp( cmd, "preset", 2 ) )				// freq up down by preset
		{
			printf( "1 -> up, 0 -> down: " );
			memset( cmd, 0, sizeof(cmd) );
			gets( cmd );
			ret = FR_TunningUpDown( atoi(cmd), FR_TUNEMODE_PRESET );
			printf( "level -> %d\n", ret );
		}
		else if( !strncmp( cmd, "search", 2 ) )			// search station
		{
			printf( "1 -> up, 0 -> down: " );
			memset( cmd, 0, sizeof(cmd) );
			gets( cmd );
			ret = FR_SearchStation( atoi(cmd), FR_SSL_MID );
			printf( "search -> %d\n", ret );
		}
		else if( !strncmp( cmd, "scan", 2 ) )				// search station
		{
			ret = FR_AutoScan( );
			printf( "scan -> %d\n", ret );
		}
		else if( !strncmp( cmd, "read", 2 ) ) 				// read param 
		{
			FR_PrintData();
		}
		else if( !strncmp( cmd, "list", 2 ) ) 				// show preset list
		{
			FR_GetPresetList();
		}
		else if( !strncmp( cmd, "quit", 2 ) ) 				// quit
		{
			break;
		}
	}

	printf( "end??\n" );
	return 0;
}
