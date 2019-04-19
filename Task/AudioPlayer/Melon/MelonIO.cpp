//************************ Copyright(C) New Media Life Inc. *******************
//
// Description : melon basic api set for tavi020
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/Melon/MelonIO.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/06/14 02:29:48 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: MelonIO.cpp,v $
// Revision 1.2  2006/06/14 02:29:48  ywkim
// Some change
//
// Revision 1.1  2006/06/12 04:33:50  ywkim
// Initial release for melon
//
//*****************************************************************************
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "../Task/tavi_global.h"
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

extern Melon_Security_t MelonData;

#define USE_FAT_FS

//*****************************************************************************
// File I/O Function Declaration
//*****************************************************************************
DH_FHANDLE DHfsOpen(DH_CHAR *name, DH_FHANDLE_ACCESS_MODE mode) 
{
#ifdef USE_FAT_FS
	RMuint32	fd;
	RMuint32 flag;
	FATFS_ERROR err;
	
	switch ( mode ) {
		case DH_FILE_OPEN_WRTRUNC:	//	   = 0x0001,
			flag = _O_WRONLY | _O_TRUNC;
			break;
		case DH_FILE_OPEN_RDWR:	//	   = 0x0002,
			DEBUGMSG("--- 10th :: DHfsOpen :: falg = _O_RDWR , not implement \n");
			flag = _O_RDWR;
			break;
		case DH_FILE_OPEN_RDONLY:	// 	   = 0x0003,
			flag = _O_RDONLY;
			break;
		case DH_FILE_OPEN_WRONLY:	// 	   = 0x0004
			flag = _O_WRONLY;
			break;
		}

	err = fatfs_wfopen ( (RMuint16 *)name, flag, &fd);
	DEBUGMSG("--- 09th :: DHfsOpen :: fatfs_fopen name=%s, fd=%d, err=0x%x\n", name, fd, err);
	if ( err > FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 10th :: DHfsOpen :: fatfs_fopen err=0x%x\n", err);
		return -1;
		}
	
	return (DH_FHANDLE)fd;
#else
	FILE *hnd = NULL;
	switch(mode) {
		case DH_FILE_OPEN_WRTRUNC:
			hnd = fopen(name, "w+b");
			break;
		case DH_FILE_OPEN_RDWR:
			hnd = fopen(name, "r+b");
			break; 
		case DH_FILE_OPEN_RDONLY:
			hnd = fopen(name, "rb"); 
			break; 
		case DH_FILE_OPEN_WRONLY:
			hnd = fopen(name, "ab");
			break;
			} 
	
	if(hnd == NULL) 
		return (-1);
	
	return (DH_FHANDLE)hnd;
#endif
} 

DH_VOID DHfsClose(DH_FHANDLE handle)
{
#ifdef USE_FAT_FS
	FATFS_ERROR err;

	err = fatfs_fclose ( (RMuint32)handle );
	
	DEBUGMSG("--- 10th :: DHfsClose :: fatfs_fclose, handle=%d, err=0x%x\n", handle, err);
#else
	fclose((FILE*)handle); 
#endif
}

DH_INT32 DHfsRead(DH_FHANDLE handle, DH_VOID *pBuf, DH_INT32 size)
{
#ifdef USE_FAT_FS
	RMuint32 len;
	RMuint32 err;
	err =	fatfs_fread ( (RMuint32)handle, (RMuint8 *)pBuf, (RMuint32)size, &len );
	DEBUGMSG("--- 11th :: DHfsRead :: fatfs_fread : handle=%d, size=%d, len=%d, err=0x%x\n", 
		handle, size, len, err);

	if ( err > FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 11th :: DHfsRead :: fatfs_fread err=0x%x\n", err);
		return -1;
		}
	return len;
#else
	return fread((DH_BYTE*)pBuf, 1, size, (FILE*)handle);
#endif
}

DH_INT32 DHfsSeek(DH_FHANDLE handle, DH_INT32 offset, DH_FHANDLE_SET_POSITION origin) 
{
#ifdef USE_FAT_FS
	FATFS_ERROR err;

	switch(origin) { 
		case DH_FILE_SEEK_CUR:
			err = fatfs_fseek ((RMuint32)handle, (RMint32)offset, SEEK_CUR); 
			break; 
		case DH_FILE_SEEK_END: 
			err = fatfs_fseek ( (RMuint32)handle, (RMint32)offset, SEEK_END); 		
			break; 
		case DH_FILE_SEEK_SET:
			err = fatfs_fseek ( (RMuint32)handle, (RMint32)offset, SEEK_SET);
			break;
		} 

	DEBUGMSG("--- 12th :: DHfsSeek :: fatfs_fseek : handle=%d, offset=%d, org=%d, err=0x%x\n", 
		handle, offset, origin, err);

	if ( err > FATFS_ERROR_NO_ERROR) {
		return -1;
		}
	else {
		err = fatfs_ftell ( (RMuint32)handle, (RMuint32*)&offset);
		}
	
	return (DH_INT32)offset;

#else
	DH_INT32 err = -1;
	switch(origin) { 
		case DH_FILE_SEEK_CUR:
			err = fseek((FILE*)handle, offset, SEEK_CUR);
			break; 
		case DH_FILE_SEEK_END: 
			err = fseek((FILE*)handle, offset, SEEK_END);
			break; 
		case DH_FILE_SEEK_SET:
			err = fseek((FILE*)handle, offset, SEEK_SET);
			break;
		} 
	
	if(err < 0) 
		return (-1);

	return ftell((FILE*)handle);
#endif	
} 

DH_INT32 DHfsTell(DH_FHANDLE handle) 
{ 
#ifdef USE_FAT_FS
	FATFS_ERROR err;
	RMuint32 pos = 0;
	
	err = fatfs_ftell (handle, &pos);

	DEBUGMSG("--- 13th :: DHfsTell :: fatfs_ftell : handle=%d, pos=%d, err=0x%x\n", handle, pos, err);
	
	if ( err > FATFS_ERROR_NO_ERROR) {
		return -1;
		}
	
	return (DH_INT32)pos;
#else
	return ftell((FILE*)handle);
#endif
}

DH_INT32 DHfsSize(DH_CHAR *name) 
{ 
#ifdef USE_FAT_FS
	FATFS_ERROR err;
	RMuint32 fd;
	RMuint32 size;
	
	err = fatfs_wfopen ( (RMuint16 *)name, _O_RDONLY , &fd);
	if ( err > FATFS_ERROR_NO_ERROR ) {
		size = 0;
		}
	else {
		err = fatfs_fsize( fd,  &size );
		}

	fatfs_fclose( fd );
	DEBUGMSG("--- 14th :: DHfsSize :: fatfs_fsize : name=%s, size=%d, err=0x%x\n", 
		name, size, err);
	
	return (DH_INT32)size;
#else
	struct stat lstat;
	DH_INT32 ret; 
	DH_INT32 size;
	
	ret = stat(name, &lstat);
	if(ret < 0) 
		size = 0;
	else 
		size = lstat.st_size;
	
	return (size);
#endif	
}

//*****************************************************************************
// Memory I/O Function Declaration
//*****************************************************************************
DH_VOID *DHmalloc(DH_INT32 size) 
{ 
	return malloc(size); 
} 

DH_VOID DHfree(DH_VOID *ptr) 
{ 
	free(ptr);
} 

DH_VOID DHmemset(DH_VOID *ptr, DH_BYTE set, DH_INT32 size)
{
	memset(ptr, set, size); 
} 

DH_VOID DHmemcpy(DH_VOID *ptr, DH_VOID *pBuf, DH_INT32 size) 
{ 
	memcpy(ptr, pBuf, size); 
} 

DH_INT32 DHmemcmp(DH_VOID *des, DH_VOID *src, DH_INT32 size) 
{
	return memcmp(des, src, size); 
} 

//*****************************************************************************
// C Runtime Library Function Declaration 
//*****************************************************************************
DH_INT32 DHstrlen(DH_CHAR *ptr) 
{ 
	return strlen(ptr); 
} 

DH_CHAR *DHstrcpy(DH_CHAR *des, DH_CHAR *src) 
{ 
	return strcpy(des, src);
} 

DH_CHAR *DHstrncpy(DH_CHAR *des, DH_CHAR *src, DH_INT32 size)
{ 
	return strncpy(des, src, size); 
}

DH_INT32 DHstrcmp(DH_CHAR *des, DH_CHAR *src) 
{ 
	return strcmp(des, src); 
}

DH_INT32 DHstrncmp(DH_CHAR *des, DH_CHAR *src, DH_INT32 size) 
{ 
	return strncmp(des, src, size);
}

DH_INT32 DHstrnicmp(DH_CHAR *des, DH_CHAR *src, DH_INT32 size) 
{ 
	DH_CHAR chsrc, chdes; 
	
	while ( (size > 0) && *src != '\0' && *des != '\0') { 
		chsrc = *src++;
		chdes = *des++;
		if (chsrc >= 'a' && chsrc <= 'z') 
			chsrc = (DH_CHAR)(chsrc - 'a' + 'A'); 
		if (chdes >= 'a' && chdes <= 'z') 
			chdes = (DH_CHAR)(chdes - 'a' + 'A'); 
		if (chsrc != chdes) return (DH_CHAR)(chdes - chsrc); size--; 
		}
	
	if (size > 0) return (DH_CHAR)(*des - *src); return 0; 
} 

DH_CHAR *DHstrcat(DH_CHAR *des, DH_CHAR *src) 
{ 
	return strcat(des, src);
} 

DH_CHAR *DHstrncat(DH_CHAR *des, DH_CHAR *src, DH_INT32 size) 
{
	return strncat(des, src, size);
} 

DH_INT32 DHatoi(DH_CHAR *src) 
{ 
	return atoi(src);
}

DH_CHAR *DHstrstr(DH_CHAR *src, DH_CHAR *find) 
{
	return (DH_CHAR *)strstr(src, find);
}

DH_CHAR *DHstrchr(DH_CHAR *src, DH_CHAR find)
{ 
	return (DH_CHAR *)strchr(src, find);
} 

DH_CHAR *DHstristr(DH_CHAR *src, DH_CHAR *find) 
{ 
	DH_CHAR *t_src; 
	DH_CHAR *t_find; 
	DH_CHAR *p;
	t_src = (DH_CHAR *)DHmalloc(sizeof(DH_CHAR) * DHstrlen(src) +1);
	t_find = (DH_CHAR *)DHmalloc(sizeof(DH_CHAR) * DHstrlen(find) +1);
	if ( t_src == NULL) return NULL;
	if ( t_find == NULL) { 
		DHfree(t_src);
		return NULL; 
		} 
	DHstrcpy(t_src, src); 
	DHstrcpy(t_find, find);
	DHstrlwr(t_src); 
	DHstrlwr(t_find);
	p = (DH_CHAR *)DHstrstr(t_src, t_find);
	if ( p != NULL) 
		p = src + ( p - t_src); // return offset DHfree(t_src); DHfree(t_find);
		
	return p; 
}

DH_INT32 DHisdigit(DH_CHAR src) 
{ 
	return isdigit(src); 
}

DH_CHAR DHtoupper(DH_CHAR c) 
{ 
	return (DH_CHAR)(toupper(c)); 
}

DH_CHAR *DHstrlwr(DH_CHAR * src)
{ 
	int i;
	int len;
	char *pString;

	pString = src;
	len = strlen ( pString );
	
	for (i=0;i<len;i++) {
		*pString = tolower( *pString );
		pString++;
		}
	return (DH_CHAR *)(src);
}

DH_INT32 DHsprintf(DH_CHAR *buf, DH_CHAR *format,...) 
{ 
	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	return 0;
} 
//*****************************************************************************
// Date Time Handle Library Extern Function Declaration 
//*****************************************************************************
DH_UINT32 DHcurrentTime (DH_VOID) 
{
	time_t t;
	int iOffset;

	
	if ( MelonData.cTimeValid == MELON_VALIDTIME ) {
		time(&t);
		DEBUGMSG("--- 700th :: DHcurrentTime =%d,%s\n", t, ctime(&t) );
			
		iOffset = MelonData.uGap;
		if ( MelonData.cSign & 0x80 ) {
			iOffset = MelonData.uGap * -1;
			}
		t += iOffset;
		DEBUGMSG("--- 703th :: offset[%d], time[%d] =%s", iOffset, t, ctime(&t));
		
		return (DH_UINT32)t;
		}
	else {
		return 0;
		}

}

DH_UINT32 DHmktime(GenericTime tm)
{ 
	struct tm tmr;
	DH_UINT32 dwTime; 

	DEBUGMSG("--- 710th :: DHmktime\n");

	if (tm.nYear >= 1970)
		tmr.tm_year = tm.nYear - 1900;
	else 
		return 0; 
	if (tm.nMonth < 1) 
		tm.nMonth = 1; 
	tmr.tm_mon = tm.nMonth - 1; 
	tmr.tm_mday = tm.nDay; 
	tmr.tm_hour = tm.nHour; 
	tmr.tm_min = tm.nMin; 
	tmr.tm_sec = tm.nSec; 
	dwTime = mktime(&tmr); 
	
	DEBUGMSG("year =%d, month=%d, day=%d, hour=%d, min=%d, sec=%d, [%d] \n",
		tm.nYear, tm.nMonth, tm.nDay, tm.nHour, tm.nMin, tm.nSec, dwTime);

	return dwTime; 
}

DH_VOID DHgetDate(DH_UINT32 time, GenericTime *tmo) 
{ 
	struct tm *tmr;
	tmr = gmtime((const time_t *)&time);
	
	tmo->nYear = tmr->tm_year + 1900;
	tmo->nMonth = tmr->tm_mon + 1;
	tmo->nDay = tmr->tm_mday;
	tmo->nHour = tmr->tm_hour;
	tmo->nMin = tmr->tm_min;
	tmo->nSec = tmr->tm_sec;

	
	DEBUGMSG("--- 720th :: DHgetDate\n");
	
	DEBUGMSG("year =%d, month=%d, day=%d, hour=%d, min=%d, sec=%d, [%d]\n",
		tmo->nYear, tmo->nMonth, tmo->nDay, tmo->nHour, tmo->nMin, tmo->nSec );
}

/**********************************************************************
* Cluster, client control Part Function Define    
***********************************************************************/ 
DH_INT32 DHgetDeviceID(DH_CHAR *src)
{
	strcpy(src, MelonData.scSerialNo);
//	strcpy( src, "T20NM5C00171G");
	DEBUGMSG("--- 90th :: DHgetDeviceID : [%s]\n", src);
	return (0);
}

DH_INT32 DHgetCluster (DH_CHAR *buf, DH_INT32 *ipSize)
{
#ifdef USE_FAT_FS
		RMuint32	fd;
		FATFS_ERROR err;
		RMuint32	len = 0;
		
		DH_CHAR temp[512] = {0,};
		DH_INT16 size = 0;
		
		err = fatfs_fopen ( (RMuint8 *)PATH_TO_CLUSTERSEC, _O_RDONLY, &fd);
		DEBUGMSG("--- 100th :: DHgetCluster :: fatfs_fopen , fd=%d, err=0x%x\n", fd, err);
		if ( err > FATFS_ERROR_NO_ERROR ) {
			DEBUGMSG("--- 100th :: DHgetCluster :: fatfs_fopen err=0x%x\n", err);
			return -1;
			}
	
		// time info
		err = fatfs_fread ( fd, (RMuint8 *)&size, 2, &len );
		if ( err > FATFS_ERROR_NO_ERROR ) {
			DEBUGMSG("--- 101th :: DHgetCluster :: fatfs_fread, time info size, err=0x%x\n", err);
			}
		size = ((size&0x00ff)<<8) | ((size&0xff00)>> 8);
		DEBUGMSG("--- 106th :: DHgetCluster ::  time size=%d\n", size);
	
		err = fatfs_fread ( fd, (RMuint8 *)temp, size, &len );
		if ( err > FATFS_ERROR_NO_ERROR ) {
			DEBUGMSG("--- 102th :: DHgetCluster :: fatfs_fread, time info data, err=0x%x\n", err);
			}
		DEBUGMSG("--- 107th :: DHgetCluster ::  time len=%d, temp=[%s]\n", len, temp);

		if ( len != size ) {
			fatfs_fclose(fd);
			return (-1); 
			}

		
		// cluster
		err = fatfs_fread ( fd, (RMuint8 *)&size, 2, &len );
		if ( err > FATFS_ERROR_NO_ERROR ) {
			DEBUGMSG("--- 103th :: DHgetCluster :: fatfs_fread, clustor size, err=0x%x\n", err);
			}
		size = ((size&0x00ff)<<8) | ((size&0xff00)>> 8);
		DEBUGMSG("--- 108th :: DHgetCluster ::  cluster size=%d\n", size);
	
		err = fatfs_fread ( fd, (RMuint8 *)temp, size, &len );
		if ( err > FATFS_ERROR_NO_ERROR ) {
			DEBUGMSG("--- 104th :: DHgetCluster :: fatfs_fread, clustor data, err=0x%x\n", err);
			}
		DEBUGMSG("--- 109th :: DHgetCluster ::  clustor len=%d, temp=[%s]\n", len, temp);
		
		if ( len != size ) {
			fatfs_fclose(fd);
			return (-1);
			}

		fatfs_fclose(fd); 
		if(len <= 0) { 
			return (-1);
			}
		DEBUGMSG("--- 109th :: DHgetCluster ::  fatfs_fclose, fd=%d\n", fd);
		memcpy(buf, temp, len);
		*ipSize = len;
		
		return (0);
#else
		FILE *fp = NULL;
		DH_CHAR temp[512] = {0,};
		DH_INT32 readed = 0;
		DH_INT16 size = 0;
		
		if((fp = fopen("../Cluster/melon.bin", "rb")) == NULL) { 
			return (-1);
			} 
		// time info
		readed = fread(&size, 2, 1, fp); 
		size = ((size&0x00ff)<<8) | ((size&0xff00)>> 8);
		readed = fread(temp, 1, size, fp);
		
		if ( readed != size ) {
			fclose(fp);
			return (-1); 
			}
		// cluster 
		readed = fread(&size, 2, 1, fp);
		size = ((size&0x00ff)<<8) | ((size&0xff00)>> 8);
		readed = fread(temp, 1, size, fp);
		if ( readed != size ) {
			fclose(fp);
			return (-1);
			}
		
		fclose(fp); 
		if(readed < 0) { 
			return (-1);
			}
		memcpy(buf, temp, readed);
		*ipSize = readed;
		return (0); 	
#endif

}

DH_INT32 DHgetDrmTimeData (DH_CHAR *cpBuf, DH_INT32 *ipBufSize)
{
#ifdef USE_FAT_FS
	RMuint32	fd;
	FATFS_ERROR err;
	RMuint32	len = 0;
	DH_INT16 size = 0;

	if ( NULL == cpBuf ) { 
		return (-1);
		}

	err = fatfs_fopen ( (RMuint8 *)PATH_TO_CLUSTERSEC, _O_RDONLY, &fd);
	DEBUGMSG("--- 100th :: DHgetDrmTimeData :: fatfs_fopen , fd=%d, err=0x%x\n",fd, err);
	if ( err > FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 101th :: DHgetDrmTimeData :: fatfs_fopen err=0x%x\n", err);
		return -1;
		}
	
	// time info 
	err = fatfs_fread ( fd, (RMuint8 *)&size, 2, &len );
	if ( err > FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 102th :: DHgetDrmTimeData :: fatfs_fread, time info size, err=0x%x\n", err);
		fatfs_fclose(fd);
		return -1;
		}
	size = ((size&0x00ff)<<8) | ((size&0xff00)>> 8);
	
	err = fatfs_fread ( fd, (RMuint8 *)cpBuf, size, &len );
	if ( err > FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 103th :: DHgetDrmTimeData :: fatfs_fread, time info, err=0x%x\n", err);
		fatfs_fclose(fd);
		return -1;
		}
	if ( len != size ) { 
		DEBUGMSG("--- 104th :: DHgetDrmTimeData :: fatfs_fclose,err, len!=size\n");
		fatfs_fclose(fd);
		return (-1);
		}
	fatfs_fclose(fd);
	
	if ( len <= 0 ) { 
		DEBUGMSG("--- 105th :: DHgetDrmTimeData :: err, len<=0\n");
		return (-1);
		} 
	*ipBufSize = len;
	
	return (0);
#else
	FILE *fp = NULL;
	DH_INT32 readed = 0; 
	DH_INT16 size = 0;
	
	if ( NULL == cpBuf ) { 
		return (-1);
		}
	if((fp = fopen ( "../Cluster/melon.bin", "rb" )) == NULL) { 
		return (-1);
		} 
	// time info 
	readed = fread(&size, 2, 1, fp);
	size = ((size&0x00ff)<<8) | ((size&0xff00)>> 8);
	readed = fread(cpBuf, 1, size, fp); 
	if ( readed != size ) { 
		fclose(fp);
		return (-1);
		}
	fclose(fp);
	if ( 0 > readed ) { 
		return (-1);
		} 
	*ipBufSize = readed;
	return (0);
#endif
}



DH_INT32 DHgetNonceInfo (DH_CHAR *src)
{
#ifdef USE_FAT_FS
#if 0
	strcpy(src, scNonce );
	printf("--- 300th :: DHgetNonceInfo : [%s]\n", src);
	return 0;
#else
	RMuint32	fd;
	FATFS_ERROR err;
	RMuint32	len = 0;
	char sNonce[44];

	bzero(sNonce, sizeof(sNonce) );

	err = fatfs_fopen ( (RMuint8 *)PATH_TO_NONCEFILE, _O_RDONLY, &fd);
	if ( err > FATFS_ERROR_NO_ERROR ) {
		DEBUGMSG("--- 201th :: DHgetNonceInfo :: fatfs_fopen err=0x%x\n", err);
		return -1;
		}
	
	// time info 
	err = fatfs_fread ( fd, (RMuint8*)sNonce, 40, &len );
	if ( err > FATFS_ERROR_NO_ERROR || len != 40) {
		DEBUGMSG("--- 202th :: DHgetNonceInfo :: fatfs_fread, nonce, err=0x%x\n", err);
		fatfs_fclose(fd);
		return -1;
		}
	
	fatfs_fclose(fd);
	memcpy(src, sNonce, 40);

	DEBUGMSG("--- 200th :: DHgetNonceInfo :: fd=%d, nonce[%s], err=0x%x\n",fd, sNonce, err);
	
	return 0;

#endif	
#else
	FILE *fp = NULL;
	DH_INT32 readed = 0;
	DH_CHAR sNonce[41] = {0,}; 
	if((fp = fopen("../Cluster/nonce.bin", "rb")) == NULL) {
		return (-1);
		} 
	if((readed = fread(sNonce, 1, 40, fp)) != 40) { 
		fclose(fp);
		return (-1);
		}
	fclose(fp);
	memcpy(src, sNonce, 40);
	return (0);
#endif	
}


