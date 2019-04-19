//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : Authentification functions for melon
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/Melon/Melon.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/06/21 10:41:42 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: Melon.cpp,v $
// Revision 1.3  2006/06/21 10:41:42  ywkim
// Change printf to DEBUGMSG
//
// Revision 1.2  2006/06/14 02:29:48  ywkim
// Some change
//
// Revision 1.1  2006/06/12 04:33:50  ywkim
// Initial release for melon
//
//*****************************************************************************
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../../../Driver/rtc/tavi_rtc.h"

#include "rmbasic.h"
#include <fatfs.h>

#include "SKTDRM.h"
#include "Melon.h"
#include "MelonIO.h"


//#define DEBUG
#ifdef DEBUG
#define debug_outs	printf
static void debug_break (void)
{
}
#define ASSERT(exp)				((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
//#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))
#define DEBUGMSG	debug_outs
#else
#define ASSERT(exp)
//#define DEBUGMSG(cond,printf_exp)
#define DEBUGMSG
#endif


Melon_Security_t	MelonData;

int MELON_Init( char *pId )
{
	int nRet;
	
	nRet = (int)DRM_Init();
	strcpy( MelonData.scSerialNo, pId );	

	MELON_TimeAdjust();
	MELON_DrmTimeInfo(0);
	DEBUGMSG("--- 10th :: MELON_Init : scSerialNo=%s, len=%d\n",MelonData.scSerialNo, strlen(MelonData.scSerialNo) );

	return nRet;
}
void MELON_Destroy( void )
{
	DRM_Destroy();
}
int MELON_CreateDID( void )
{
	DH_CHAR Did[16]={0,};
	DH_UINT32 iUnixTime;
	DH_CHAR outBuf[512]={0,};	// sizeof(inDid) + 41 bytes
	DH_INT32 outBufSize=0;
	DH_INT32 err;
	DH_INT16 size;


	RMuint32	fd;
	FATFS_ERROR fderr;
	RMuint32	len = 0;

	iUnixTime = time ( NULL );
	DHgetDeviceID(Did);
	err = DRM_CreateDID ( Did, iUnixTime, MelonData.scNonce, outBuf, &outBufSize );
	
	printf("---20th:: MELON_CreateDID : iUnixTime=%d, ctime=%s, Did=%s, outNonce=%s, outBufSize=%d, err=%d, \n",
		iUnixTime, ctime((time_t*)&iUnixTime), Did, MelonData.scNonce, outBufSize, err );

	if ( err < 0 ) return -1;
	
	// ============== create did file ==============================
	fderr = fatfs_fopen ( (RMuint8 *)PATH_TO_DIDFILE, _O_WRONLY|_O_CREAT|_O_TRUNC, &fd);
	if ( fderr > FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 300th :: MELON_CreateDID :: fatfs_fopen, did err=0x%x\n", fderr);
		return -1;
		}
	// did size
	size = ((outBufSize&0x00ff)<<8) | ((outBufSize&0xff00)>> 8);	// convert little endian to big endian
	fderr = fatfs_fwrite ( fd, (RMuint8 *)&size, 2, &len );
	if ( fderr > FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 301th :: MELON_CreateDID :: fatfs_fwrite, did size, err=0x%x\n", fderr);
		fatfs_fclose(fd);
		return -1;
		}
	// did data
	fderr = fatfs_fwrite ( fd, (RMuint8 *)outBuf, outBufSize, &len );
	if ( fderr > FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 302th :: MELON_CreateDID :: fatfs_fwrite, did data, err=0x%x\n", fderr);
		fatfs_fclose(fd);
		return -1;
		}
	fatfs_fclose(fd);

	return 0;
}

int MELON_DeleteDID( void )
{
	FATFS_ERROR fderr;

	fderr = fatfs_delete_file( (RMuint8*)PATH_TO_DIDFILE );
	DEBUGMSG("--- 600th :: MELON_DeleteDID, err=%d\n", fderr);

	return (int)fderr;
}

int MELON_TimeAdjust( void )
{
	DH_CHAR scTimeBuff[64];
	DH_INT32 iSizeBuff;
	DH_INT32 iRet;
	DH_UINT32 uGap;
	DH_CHAR cSign;
	
	iRet = DHgetDrmTimeData ( scTimeBuff, &iSizeBuff);
	if ( iRet == 0 ) {
		iRet = DRM_GetDRMTime( scTimeBuff,  iSizeBuff, &uGap, &cSign );
		DEBUGMSG("--- 500th :: MELON_TimeAdjust : DRM_GetDRMTime : success, size=%d, gap=%d, sign=%d, ret=%d\n", 
			iSizeBuff, uGap, cSign, iRet );
		if ( iRet == 0 ) {
			MelonData.uGap = uGap;
			MelonData.cSign = cSign;
			MelonData.cTimeValid = MELON_VALIDTIME;
			}
		}
	else {
		DEBUGMSG("--- 501th :: MELON_TimeAdjust : DRM_GetDRMTime : error, size=%d, gap=%d, sign=%d, ret=%d\n", 
			iSizeBuff,  uGap, cSign, iRet );
		}

	return (int)iRet;
}

int MELON_MoveCluster( void )
{
	int	nRet;
	RMuint32	fdsrc;
	RMuint32	fddst;
	RMuint32 len;
	RMuint8 Buffer[1024];
	
	FATFS_ERROR fderr;

	fderr = fatfs_fopen ( (RMuint8 *)PATH_TO_CLUSTERORG, _O_RDONLY, &fdsrc);

	if ( fderr == FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 900th :: MELON_MoveCluster : find\n");
		fderr = fatfs_fopen( (RMuint8 *)PATH_TO_CLUSTERSEC, _O_WRONLY|_O_CREAT|_O_CORRUPTED, &fddst );
		if ( fderr == FATFS_ERROR_NO_ERROR ) {
			do {
				fderr = fatfs_fread( fdsrc, Buffer, sizeof(Buffer), &len);
				if ( fderr == FATFS_ERROR_NO_ERROR && len > 0 ) {
					fderr = fatfs_fwrite( fddst, Buffer, len, &len );
					}
				} while ( len > 0 );
			nRet = 1;
			}
		else {
			nRet = -1;
			DEBUGMSG("--- 901th :: MELON_MoveCluster :: fatfs_fopen, cluster, dest err=0x%x\n", fderr);
			}
		
		fatfs_fclose(fdsrc);
		fatfs_fclose(fddst);

		fderr = fatfs_delete_file( (RMuint8 *)PATH_TO_CLUSTERORG);

		}
	else {
		nRet = 0;
		DEBUGMSG("--- 902th :: MELON_MoveCluster :: fatfs_fopen, cluster, not exist melon.bin err=0x%x\n", fderr);
		}
	

	return nRet;
}

int MELON_DrmTimeInfo( int flag )
{
	int nRet;
	int handle_rtc;
	char Data;
	
	handle_rtc = open( "/dev/misc/rtc", O_RDWR );
	if ( handle_rtc  < 0 ) {
		DEBUGMSG( "failed to open /dev/misc/rtc\n" );
		nRet = -1;
		} 
	else {
		if ( flag ) {
			// write 
			Data = MelonData.cTimeValid;
			nRet = ioctl( handle_rtc, RTC_WR_TIMER, &Data);
			}
		else {
			// read
			nRet = ioctl( handle_rtc, RTC_RD_TIMER, &Data );
			MelonData.cTimeValid = Data;
			
			}
		close(handle_rtc);

		DEBUGMSG("--- 751th :: MELON_DrmTimeInfo : handle=%d : flag=%d, Vaild=0x%x\n", handle_rtc, flag, Data );
		DEBUGMSG("--- 752th :: MELON_DrmTimeInfo : uGap=%d, cSign=%d\n", MelonData.uGap,MelonData.cSign );

		}
	return nRet;
}

int MELON_SaveNonce( void )
{
	int	nRet = -1;
	RMuint32	fd;
	RMuint32 len;
	FATFS_ERROR fderr;

	// ============== create nonce file ==============================
	fderr = fatfs_fopen ( (RMuint8 *)PATH_TO_NONCEFILE, _O_WRONLY|_O_CREAT|_O_TRUNC|_O_CORRUPTED, &fd);
	if ( fderr == FATFS_ERROR_NO_ERROR ) {
		fderr = fatfs_fwrite ( fd, (RMuint8 *)MelonData.scNonce, 40, &len );
		if ( fderr == FATFS_ERROR_NO_ERROR ) {
			DEBUGMSG("--- 311th :: MELON_CreateDID :: fatfs_fwrite, nonce, err=0x%x\n", fderr);
			nRet = 0;
			}
		}
	fatfs_fclose(fd);
	
	return nRet;
}
