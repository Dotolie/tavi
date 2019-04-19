#include "fs_fat.h"
#include "fatfs.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <malloc.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/time.h>
#include <define_fs.h>
#include "rmbasic.h"
#include "ata_ioctl.h"
#if USE_PBI_DRV
#include "atapi_ioctl.h"
#endif /* USE_PBI_DRV */
#include "util.h"

#define PUT_HD_TO_SLEEP



#if 0
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("***ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
static RMint32 fat_filepos = 0;
#define SET_FAT_FILEPOS(x)	{fat_filepos = (x);}
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#define SET_FAT_FILEPOS(x)
#endif

// power management for hdd
int ata_powerdown = 0;

//////////////////////////////////////////////////////////////////////
// required fatfs callbacks
static int ata_fd = -1;

#if USE_PBI_DRV
static RMuint32 fatfs_pbi_SendCmdRWS (
#ifdef FATFS_PBI_V_3_0_3
	RMuint32 dwHandle,
#endif
	RMuint8  bDrive, 
	RMuint8  bCmd, 
	RMuint32 nSectorLBA, 
	RMuint16 nNumSectors, 
	RMuint8  *pbBuf, 
	RMuint32 SizeInBytes)
{
	RMint32		result;
	ATA_COMMAND	AtaCommand;
	CMD_PACKET	*pCmdPacket;
	RMuint8		CmdPacket[16];

	if (ata_fd == -1)
	{
		printf ("***Error: Ata_ReadSectorsLBA: Invalid gAtaFd. Kernel Driver not open.\n");
		return (RMuint32)-1;
	}

	pCmdPacket = (CMD_PACKET *)&(CmdPacket[0]);

	pCmdPacket->bCmd				= bCmd;
	pCmdPacket->dwLBA				= nSectorLBA;
	pCmdPacket->wXxLength			= nNumSectors;		// N sectors

	AtaCommand.pbCmdPkt				= CmdPacket;
	AtaCommand.wPacketSize			= 12;
	AtaCommand.bDrv					= bDrive;	// Primary Drive
	AtaCommand.pbBuffer				= pbBuf;
	AtaCommand.dwRequestedXferCount	= SizeInBytes;		// SizeOf AtaCommand.pbBuffer
	AtaCommand.dwActualXferCount	= 0;
	AtaCommand.wAtaStatus			= 0;
	AtaCommand.wAtaError			= 0;
	AtaCommand.wExStatusBytes		= 0;

	result = ioctl (ata_fd, ATA_IOCTL_ISSUE_ATA_COMMAND, &AtaCommand);
	if (AtaCommand.wAtaError != 0) 
	{
		// Some Error
		printf ("***Error: Ata_SendCmd(): ioctl(): AtaCommand.wAtaError = 0x%04X.\n", AtaCommand.wAtaError);
	}
	return ((RMuint32)AtaCommand.wAtaError);
}

static RMuint32 fatfs_pbi_SendCmd (
#ifdef FATFS_PBI_V_3_0_3
	RMuint32 dwHandle,
#endif
	RMuint8 bDrive, 
	RMuint8	bCmd, 
	RMuint8 bFeature, 
	RMuint8	bSectCounter, 
	RMuint8 bSectNumber, 
	RMuint8 bCylinderLo,
	RMuint8	bCylinderHi, 
	RMuint8 bDevHead, 
	RMuint8 *pbIoBuf,
	RMuint32 dwBufSizeInBytes)
{
	RMint32 result;
	ATA_COMMAND AtaCommand;
	RMuint8 CmdPacket[16];

	if (ata_fd == -1)
	{
		printf ("***Error: Ata_ReadSectorsLBA: Invalid gAtaFd. Kernel Driver not open.\n");
		return (RMuint32)-1;
	}

	// Universal control - Send values for all registers + bDrv
	CmdPacket[0]	= bCmd;
	CmdPacket[1]	= bFeature;
	CmdPacket[2]	= bSectCounter;
	CmdPacket[3]	= bSectNumber;
	CmdPacket[4]	= bCylinderLo;
	CmdPacket[5]	= bCylinderHi;
	CmdPacket[6]	= bDevHead;

	AtaCommand.pbCmdPkt				= CmdPacket;
	AtaCommand.wPacketSize			= 12;
	AtaCommand.bDrv					= bDrive;			// Primary Drive
	AtaCommand.pbBuffer				= pbIoBuf;
	AtaCommand.dwRequestedXferCount	= dwBufSizeInBytes;	// SizeOf AtaCommand.pbBuffer
	AtaCommand.dwActualXferCount	= 0;
	AtaCommand.wAtaStatus			= 0;
	AtaCommand.wAtaError			= 0;
	AtaCommand.wExStatusBytes		= 0;

	result = ioctl (ata_fd, ATA_IOCTL_ISSUE_ATA_COMMAND, &AtaCommand);
	if (AtaCommand.wAtaError != 0) 
	{
		// Some Error
		printf ("***Error: Ata_SendCmd(): %02x %02x %02x %02x %02x %02x %02x %02x\n", 
			bDrive, bCmd, bFeature, bSectCounter, 
			bSectNumber, bCylinderLo, bCylinderHi, bDevHead);
		printf ("***Error: Ata_SendCmd(): ioctl(): AtaCommand.wAtaError = 0x%04X.\n", AtaCommand.wAtaError);
	}
	return ((RMuint32)AtaCommand.wAtaError);
}

#elif USE_ATA_DRV
static RMuint32 fatfs_Ata_SendCmdRWS (
	RMuint8  bDrive, 
	RMuint8  bCmd, 
	RMuint32 nSectorLBA, 
	RMuint16 nNumSectors, 
	RMuint8  *pbBuf, 
	RMuint32 SizeInBytes)
{
	RMint32		result;
	ATA_COMMAND	AtaCommand;
	CMD_PACKET	*pCmdPacket;
	RMuint8		CmdPacket[16];

	DEBUGMSG (1, ("fatfs_Ata_SendCmdRWS (%d): %02x\n", ata_fd, bCmd));

	if (ata_fd == -1)
	{
		DEBUGMSG (1, ("***Error: Ata_ReadSectorsLBA: Invalid gAtaFd. Kernel Driver not open.\n"));
		return (RMuint32)-1;
	}

	if (ata_powerdown && (bDrive == 0))
		fs_fat_powerup ();

	pCmdPacket = (CMD_PACKET *)&(CmdPacket[0]);

	pCmdPacket->bCmd				= bCmd;
	pCmdPacket->dwLBA				= nSectorLBA;
	pCmdPacket->wXxLength			= nNumSectors;		// N sectors

	AtaCommand.pbCmdPkt				= CmdPacket;
	AtaCommand.wPacketSize			= 12;
	AtaCommand.bDrv					= bDrive;	// Primary Drive
	AtaCommand.pbBuffer				= pbBuf;
	AtaCommand.dwRequestedXferCount	= SizeInBytes;		// SizeOf AtaCommand.pbBuffer
	AtaCommand.dwActualXferCount	= 0;
	AtaCommand.wAtaStatus			= 0;
	AtaCommand.wAtaError			= 0;
	AtaCommand.wExStatusBytes		= 0;

	result = ioctl (ata_fd, ATA_IOCTL_ISSUE_ATA_COMMAND, &AtaCommand);
	if ((AtaCommand.wAtaError != 0) || (result != 0))
	{
		// Some Error
		DEBUGMSG (1, ("***Error: result: %d\n", result));
		DEBUGMSG (1, ("***Error: Ata_SendCmd(): ioctl(): AtaCommand.wAtaError = 0x%04X.\n", AtaCommand.wAtaError));
	}
	return ((RMuint32)AtaCommand.wAtaError);
}

static RMuint32 fatfs_Ata_SendCmd (
	RMuint8 bDrive, 
	RMuint8	bCmd, 
	RMuint8 bFeature, 
	RMuint8	bSectCounter, 
	RMuint8 bSectNumber, 
	RMuint8 bCylinderLo,
	RMuint8	bCylinderHi, 
	RMuint8 bDevHead, 
	RMuint8 *pbIoBuf,
	RMuint32 dwBufSizeInBytes)
{
	RMint32 result;
	ATA_COMMAND AtaCommand;
	RMuint8 CmdPacket[16];

	DEBUGMSG (1, ("fatfs_Ata_SendCmd (%d): %02x\n", ata_fd, bCmd));
	if (ata_fd == -1)
	{
		DEBUGMSG (1, ("***Error: Ata_ReadSectorsLBA: Invalid gAtaFd. Kernel Driver not open.\n"));
		return (RMuint32)-1;
	}

	if (ata_powerdown && (bDrive == 0))
		fs_fat_powerup ();

	// Universal control - Send values for all registers + bDrv
	CmdPacket[0]	= bCmd;
	CmdPacket[1]	= bFeature;
	CmdPacket[2]	= bSectCounter;
	CmdPacket[3]	= bSectNumber;
	CmdPacket[4]	= bCylinderLo;
	CmdPacket[5]	= bCylinderHi;
	CmdPacket[6]	= bDevHead;

	AtaCommand.pbCmdPkt				= CmdPacket;
	AtaCommand.wPacketSize			= 12;
	AtaCommand.bDrv					= bDrive;			// Primary Drive
	AtaCommand.pbBuffer				= pbIoBuf;
	AtaCommand.dwRequestedXferCount	= dwBufSizeInBytes;	// SizeOf AtaCommand.pbBuffer
	AtaCommand.dwActualXferCount	= 0;
	AtaCommand.wAtaStatus			= 0;
	AtaCommand.wAtaError			= 0;
	AtaCommand.wExStatusBytes		= 0;

	result = ioctl (ata_fd, ATA_IOCTL_ISSUE_ATA_COMMAND, &AtaCommand);
	if ((AtaCommand.wAtaError != 0) || (result != 0))
	{
		// Some Error
		DEBUGMSG (1, ("***Error: result: %d\n", result));
		DEBUGMSG (1, ("***Error: Ata_SendCmd(): ioctl(): AtaCommand.wAtaError = 0x%04X.\n", AtaCommand.wAtaError));
	}
	return( (RMuint32)AtaCommand.wAtaError );
}
#endif

static void *fatfs_malloc (RMuint32 s)
{
	void *p;	
	p = malloc (s);
	DEBUGMSG (1, ("%08lx = fatfs_malloc (%d)\n", (RMuint32)p, (RMint32)s));
	return p;
}

static void fatfs_free (void *p)
{
	DEBUGMSG (1, ("fatfs_free (%08lx)\n", (RMuint32)p));
	return free (p);
}
////////////////////////////////////////////////////////////////////////

// power down/up media device
RMuint32 fs_fat_powerdown ()
{
#ifdef PUT_HD_TO_SLEEP
	RMint32 result;
	ATA_COMMAND AtaCommand;
	RMuint8 CmdPacket[16];

	if (ata_powerdown)
		return 0;

	DEBUGMSG (1, ("SLEEP command to hard drive\n"));

	CmdPacket[0]	= 0xe6;
	CmdPacket[1]	= 0;
	CmdPacket[2]	= 0;
	CmdPacket[3]	= 0;
	CmdPacket[4]	= 0;
	CmdPacket[5]	= 0;
	CmdPacket[6]	= 0;

	AtaCommand.pbCmdPkt				= CmdPacket;
	AtaCommand.wPacketSize			= 12;
	AtaCommand.bDrv					= 0;
	AtaCommand.pbBuffer				= 0;
	AtaCommand.dwRequestedXferCount	= 0;
	AtaCommand.dwActualXferCount	= 0;
	AtaCommand.wAtaStatus			= 0;
	AtaCommand.wAtaError			= 0;
	AtaCommand.wExStatusBytes		= 0;

	result = ioctl (ata_fd, ATA_IOCTL_ISSUE_ATA_COMMAND, &AtaCommand);
	if (result != 0)
	{
		DEBUGMSG (1, ("ioctl error: %d (ata.o not loaded?)\n", (int)result));
		return (RMuint32)result;
	}
	if (AtaCommand.wAtaError != 0) 
	{
		DEBUGMSG (1, ("SLEEP command error\n"));
		DEBUGMSG (1, ("error:  %04x\n", AtaCommand.wAtaError));
		DEBUGMSG (1, ("status: %04x\n", AtaCommand.wAtaStatus));
	}
	ata_powerdown = 1;
	return (RMuint32)AtaCommand.wAtaError;
#else
	return 0;
#endif
}

#define pPIO0_DIR 		( volatile int* )( 0x00500600+0x08 )
#define pPIO0_DATA 		( volatile int* )( 0x00500600+0x04 )

RMuint32 fs_fat_powerup ()
{
#ifdef PUT_HD_TO_SLEEP
	if (ata_powerdown == 0)
		return 0;

	// ATA reset

	*pPIO0_DATA = 0x00020002;
	*pPIO0_DIR = 0x00020002;
	*pPIO0_DATA = 0x00020000;
	usleep( 100 );
	*pPIO0_DATA = 0x00020002;

	ata_powerdown = 0;

	ATA_COMMAND AtaCommand;
	RMuint8 CmdPacket[16];

	DEBUGMSG (1, ("IDLE IMMEDIATE command to hard drive\n"));

	CmdPacket[0]	= 0xe1;
	CmdPacket[1]	= 0;
	CmdPacket[2]	= 0;
	CmdPacket[3]	= 0;
	CmdPacket[4]	= 0;
	CmdPacket[5]	= 0;
	CmdPacket[6]	= 0;

	AtaCommand.pbCmdPkt				= CmdPacket;
	AtaCommand.wPacketSize			= 12;
	AtaCommand.bDrv					= 0;
	AtaCommand.pbBuffer				= 0;
	AtaCommand.dwRequestedXferCount	= 0;
	AtaCommand.dwActualXferCount	= 0;
	AtaCommand.wAtaStatus			= 0;
	AtaCommand.wAtaError			= 0;
	AtaCommand.wExStatusBytes		= 0;

	ioctl (ata_fd, ATA_IOCTL_ISSUE_ATA_COMMAND, &AtaCommand);
	if (AtaCommand.wAtaError != 0) 
	{
		DEBUGMSG (1, ("IDLE IMMEDIATE command error\n"));
		DEBUGMSG (1, ("error:  %04x\n", AtaCommand.wAtaError));
		DEBUGMSG (1, ("status: %04x\n", AtaCommand.wAtaStatus));
	}
#endif
	return 0;
}

// mount/unmount filesystem
RMuint32 fs_fat_mount (void)
{
	FATFS_ERROR err;
	FATFS_CALLBACK_TABLE callbacktable;

	#if USE_ATA_DRV
	if (ata_fd == -1)
		ata_fd = open ("/dev/ata", O_RDWR | O_NONBLOCK); 
	ASSERT (ata_fd != -1);
	if (ata_fd == -1)
	{
		DEBUGMSG (1, ("error opening /dev/ata\n"));
		return (RMuint32)-1;
	}

	callbacktable.Ata_SendCmd = fatfs_Ata_SendCmd;
	callbacktable.Ata_SendCmdRWS = fatfs_Ata_SendCmdRWS;
	#elif USE_PBI_DRV
	if (ata_fd == -1)
		ata_fd = open ("/dev/atapi", O_RDWR | O_NONBLOCK); 
	ASSERT (ata_fd != -1);
	if (ata_fd == -1)
	{
		DEBUGMSG (1, ("error opening /dev/atapi\n"));
		return (RMuint32)-1;
	}
	callbacktable.Ata_SendCmd = fatfs_pbi_SendCmd;
	callbacktable.Ata_SendCmdRWS = fatfs_pbi_SendCmdRWS;
	#endif
	callbacktable.malloc = fatfs_malloc;
	callbacktable.mfree = fatfs_free;

	DEBUGMSG (1, ("mounting the file system ...\n"));
	err = fatfs_mount (0, &callbacktable, 0xff);
	ASSERT (err == FATFS_ERROR_NO_ERROR);
	if (err != FATFS_ERROR_NO_ERROR)
	{
		DEBUGMSG (1, ("error mounting filesystem.\n"));
		close (ata_fd);
		ata_fd = -1;
		return (RMuint32)-1;
	}
	return 0;
}

RMuint32 fs_fat_unmount (void)
{
	fatfs_umount (0);
	close (ata_fd);
	ata_fd = -1;
}

RMuint32 fs_fat_set_logical_drive (int index)
{
	RMuint32 err;
	err = fatfs_setcurlogdrive (index);
	ASSERT (err == FATFS_ERROR_NO_ERROR);
	return err;
}

RMint32  fs_fat_filenameoffset (void)
{
	return 0;
}

RMuint32 fs_fat_eject (void)
{
	return 0;
}

RMuint32 fs_fat_can_eject (void)
{
	return 0;
}

// directory functions
RMuint32 fs_fat_getrootdir (RMint8 *dir, RMuint32 length, void *context)
{
	FATFS_ERROR err;
	err = fatfs_getrootdir (dir, length);
	ASSERT (err == FATFS_ERROR_NO_ERROR);
	DEBUGMSG (1, ("fs_fat_getrootdir: %s\n", dir));
	return 0;
}

RMuint32 fs_fat_opendir (RMint8 *dir, void *context)
{
	RMuint32 handle;
	if (fatfs_dopen ((RMuint8 *)dir, &handle) == FATFS_ERROR_NO_ERROR)
	{
		DEBUGMSG (1, ("fs_fat_opendir (%s) = 0x%08lx\n", dir, handle));
		return handle;
	}
	ASSERT (0);
	return (RMuint32)-1;
}

// ENGLISH only!
static void unicode2char (RMuint8 *unicode, char *name, int l)
{
	int i;
	for (i=0; i<l; i++)
	{
		name[0] = unicode[0];
		name++;
		unicode += 2;
	}
	name[0] = 0;
}

RMuint32 fs_fat_readdir (RMuint32 handle, DIR_ENTRY_INFO *pDirInfo, void *context)
{
	FATFS_ERROR err;
	FATFS_DIR_ENTRY_INFO dirinfo;

	err = fatfs_dread (handle, &dirinfo);
	if (err != FATFS_ERROR_NO_ERROR)
		return 0;
	pDirInfo->handle = 0;
	if (dirinfo.attributes & 0x10)
		pDirInfo->flags = DIR_ENTRY_INFO_IS_DIRECTORY;
	else
		pDirInfo->flags = DIR_ENTRY_INFO_IS_FILE;
	dirinfo.short_name[12] = 0;
	DEBUGMSG (0, ("fs_fat_readdir:\n"));
	DEBUGMSG (0, (" dirinfo.attrib_ln: 0x%08lx\n", dirinfo.attrib_ln));
	DEBUGMSG (0, (" dirinfo.name: %s\n", dirinfo.name));
	DEBUGMSG (0, (" dirinfo.namelength: %d\n", (RMint32)dirinfo.namelength));
	DEBUGMSG (0, (" dirinfo.short_name: %s\n", dirinfo.short_name));
	pDirInfo->namelength = dirinfo.namelength;
	unicode2char ((RMuint8 *)dirinfo.name, pDirInfo->name, dirinfo.namelength);
	return 1;
}

RMuint32 fs_fat_closedir (RMuint32 handle, void *context)
{
	FATFS_ERROR err;
	DEBUGMSG (1, ("fs_fat_closedir (0x%08lx)\n", handle));
	err = fatfs_dclose (handle);
	ASSERT (err == FATFS_ERROR_NO_ERROR);
	return 0;
}

// file functions
RMuint32 fs_fat_fopen (RMint8 *filename, void *context)
{
	FATFS_ERROR err;
	RMuint32 fd = 0;
	err = fatfs_fopen ((RMuint8 *)filename, _O_RDONLY, &fd);
	ASSERT (err == FATFS_ERROR_NO_ERROR);
	SET_FAT_FILEPOS (0);
	return fd;
}


RMuint32 fs_fat_wfopen (RMuint16 *filename, void *context)
{
	FATFS_ERROR err;
	RMuint32 fd = 0;
	err = fatfs_wfopen ((RMuint16*)filename, _O_RDONLY, &fd);
	ASSERT (err == FATFS_ERROR_NO_ERROR);
	SET_FAT_FILEPOS (0);
	return fd;
}


RMuint32 fs_fat_wfopen_wronly (RMuint16 *filename, void *context)
{
   FATFS_ERROR err;
   RMuint32 fd = 0;

   err = fatfs_wfopen ((RMuint16 *)filename, _O_WRONLY, &fd);
   
   ASSERT (err == FATFS_ERROR_NO_ERROR);
   SET_FAT_FILEPOS (0);
   return fd;
}


RMuint32 fs_fat_fread (RMuint32 handle, void *buf, RMuint32 length, void *context)
{		
	FATFS_ERROR err;
	RMuint32 n = 0;
	DEBUGMSG (1, ("fs_fat_fread (0x%08lx, 0x%08lx, %d)\n", handle, (RMuint32)buf, (RMint32)length));
//	int t0, t1;
//	t0 = gettime_ms ();
	err = fatfs_fread (handle, (RMuint8 *)buf, length, &n);
//	t1 = gettime_ms ();
	if (err != FATFS_ERROR_NO_ERROR)
		ASSERT (n == 0);
	DEBUGMSG (1, ("fs_fat_fread = %d\n", (RMint32)n));
//	DEBUGMSG (1, ("time = %d\n", (RMint32)(t1-t0)));
	SET_FAT_FILEPOS (fat_filepos + n);
#if 0
	// debug code
	RMuint32 i;
	RMuint8 *p = (RMuint8 *)buf;
	for (i=0; i<n; i++)
	{
		if (i > 64)
			break;
		DEBUGMSG (1, ("%02x ", p[i]));
		if (i && (((i+1) % 8) == 0))
			DEBUGMSG (1, ("\n"));
	}
	DEBUGMSG (1, ("\n"));
#endif
	return n;
}

RMuint32 fs_fat_fseek( RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context )
{
	FATFS_ERROR err;
	if (whence == SEEK_SET)
	{
		DEBUGMSG (1, ("fs_fat_fseek (%d, SEEK_SET)\n", (RMint32)pos));
		err = fatfs_fseek (handle, pos, SEEK_SET);
		ASSERT (err == FATFS_ERROR_NO_ERROR);
	}
	else if (whence == SEEK_CUR)
	{
		DEBUGMSG (1, ("fs_fat_fseek (%d, SEEK_CUR)\n", (RMint32)pos));
		RMuint32 curpos = 0;
		err = fatfs_ftell (handle, &curpos);
		ASSERT (err == FATFS_ERROR_NO_ERROR);
		pos = (RMuint32)((RMint32)curpos + (RMint32)pos);
		err = fatfs_fseek (handle, pos, SEEK_SET); 
		ASSERT (err == FATFS_ERROR_NO_ERROR);
	}
	else if (whence == SEEK_END)
	{
		err = fatfs_fseek (handle, pos, SEEK_END); 
		ASSERT (err == FATFS_ERROR_NO_ERROR);
		err = fatfs_ftell (handle, &pos);
		ASSERT (err == FATFS_ERROR_NO_ERROR);
	}
	else
		ASSERT (0);
	SET_FAT_FILEPOS (pos);
	return pos;
}

RMuint32 fs_fat_ftell (RMuint32 handle, void *context)
{
	FATFS_ERROR err;
	RMuint32 pos = 0;
	err = fatfs_ftell (handle, &pos);
	ASSERT (err == FATFS_ERROR_NO_ERROR);
	ASSERT (fat_filepos == pos);
	DEBUGMSG (1, ("fs_fat_ftell: fat_filepos = %d, pos = %d\n", (RMint32)fat_filepos, (RMint32)pos));
	return pos;
}

RMuint32 fs_fat_feof (RMuint32 handle, void *context)
{
	FATFS_ERROR err;
	RMuint32 eof = 0;
	err = fatfs_feof (handle, &eof);
	ASSERT (err == FATFS_ERROR_NO_ERROR);
	return eof;
}

RMuint32 fs_fat_fclose (RMuint32 handle, void *context)
{
	FATFS_ERROR err;
	err = fatfs_fclose (handle);
	ASSERT (err == FATFS_ERROR_NO_ERROR);
	return 0;
}



char* GetErrMsg( char* str, int err )
{

	switch( err )
	{
	case FATFS_ERROR_NO_ERROR: sprintf( str, "%s", "FATFS_ERROR_NO_ERROR" ); break;
	case FATFS_ERROR_NO_MORE_ENTRIES: sprintf( str, "%s", "FATFS_ERROR_NO_MORE_ENTRIES" ); break;
	case FATFS_ERROR_NO_REQUESTED_NUM_ENTRIES: sprintf( str, "%s", "FATFS_ERROR_NO_REQUESTED_NUM_ENTRIES" ); break;
	case FATFS_ERROR_NO_FREE_ENTRIES_IN_SECTOR: sprintf( str, "%s", "FATFS_ERROR_NO_FREE_ENTRIES_IN_SECTOR" ); break;
	case FATFS_ERROR_DIVISION_BY_ZERO: sprintf( str, "%s", "FATFS_ERROR_DIVISION_BY_ZERO" ); break;
	case FATFS_ERROR_NOT_SUPPORTED_VERSION: sprintf( str, "%s", "FATFS_ERROR_NOT_SUPPORTED_VERSION" ); break;
	case FATFS_ERROR_NOT_SUPPORTED_VOLUME_TYPE: sprintf( str, "%s", "FATFS_ERROR_NOT_SUPPORTED_VOLUME_TYPE" ); break;
	case FATFS_ERROR_BUFFER_TOO_SMALL: sprintf( str, "%s", "FATFS_ERROR_BUFFER_TOO_SMALL" ); break;
	case FATFS_ERROR_MEM_CAN_NOT_ALLOCATE_MEMORY: sprintf( str, "%s", "FATFS_ERROR_MEM_CAN_NOT_ALLOCATE_MEMORY" ); break;
	case FATFS_ERROR_DIR_INVALID_HANDLE: sprintf( str, "%s", "FATFS_ERROR_DIR_INVALID_HANDLE" ); break;
	case FATFS_ERROR_DIR_INVALID_DRIVE: sprintf( str, "%s", "FATFS_ERROR_DIR_INVALID_DRIVE" ); break;
	case FATFS_ERROR_DIR_INVALID_PATH: sprintf( str, "%s", "FATFS_ERROR_DIR_INVALID_PATH" ); break;
	case FATFS_ERROR_DIR_INVALID_ENTRY: sprintf( str, "%s", "FATFS_ERROR_DIR_INVALID_ENTRY" ); break;
	case FATFS_ERROR_DIR_NAME_NOT_FOUND: sprintf( str, "%s", "FATFS_ERROR_DIR_NAME_NOT_FOUND" ); break;
	case FATFS_ERROR_DIR_NOT_EMPTY: sprintf( str, "%s", "FATFS_ERROR_DIR_NOT_EMPTY" ); break;
	case FATFS_ERROR_VOLUME_FULL: sprintf( str, "%s", "FATFS_ERROR_VOLUME_FULL" ); break;
	case FATFS_ERROR_VOLUME_LOW_ON_DISK_SPACE: sprintf( str, "%s", "FATFS_ERROR_VOLUME_LOW_ON_DISK_SPACE" ); break;
	case FATFS_ERROR_VOLUME_INVALID_DRIVE_SPECIFIED: sprintf( str, "%s", "FATFS_ERROR_VOLUME_INVALID_DRIVE_SPECIFIED" ); break;
	case FATFS_ERROR_PATH_DIR_NAME_NOT_FOUND: sprintf( str, "%s", "FATFS_ERROR_PATH_DIR_NAME_NOT_FOUND" ); break;
	case FATFS_ERROR_PATH_DIR_NAME_INVALID: sprintf( str, "%s", "FATFS_ERROR_PATH_DIR_NAME_INVALID" ); break;
	case FATFS_ERROR_PATH_DIR_NAME_TOO_LONG: sprintf( str, "%s", "FATFS_ERROR_PATH_DIR_NAME_TOO_LONG" ); break;
	case FATFS_ERROR_PATH_DIR_NAME_SYNTAX: sprintf( str, "%s", "FATFS_ERROR_PATH_DIR_NAME_SYNTAX" ); break;
	case FATFS_ERROR_PATH_NAME_NOT_FOUND: sprintf( str, "%s", "FATFS_ERROR_PATH_NAME_NOT_FOUND" ); break;
	case FATFS_ERROR_PATH_INVALID_DRIVE_SPECIFIED: sprintf( str, "%s", "FATFS_ERROR_PATH_INVALID_DRIVE_SPECIFIED" ); break;
	case FATFS_ERROR_NAME_SYNTAX: sprintf( str, "%s", "FATFS_ERROR_NAME_SYNTAX" ); break;
	case FATFS_ERROR_NAME_TOO_LONG: sprintf( str, "%s", "FATFS_ERROR_NAME_TOO_LONG" ); break;
	case FATFS_ERROR_NAME_WRONG_LONG_SEQUENCE_TYPE: sprintf( str, "%s", "FATFS_ERROR_NAME_WRONG_LONG_SEQUENCE_TYPE" ); break;
	case FATFS_ERROR_NAME_WRONG_LONG_SEQUENCE_ORD: sprintf( str, "%s", "FATFS_ERROR_NAME_WRONG_LONG_SEQUENCE_ORD" ); break;
	case FATFS_ERROR_NAME_WRONG_LONG_SEQUENCE_ORD_LIMITS: sprintf( str, "%s", "FATFS_ERROR_NAME_WRONG_LONG_SEQUENCE_ORD_LIMITS" ); break;
	case FATFS_ERROR_NAME_DOES_EXIST_AS_FILE: sprintf( str, "%s", "FATFS_ERROR_NAME_DOES_EXIST_AS_FILE" ); break;
	case FATFS_ERROR_NAME_DOES_EXIST_AS_DIR: sprintf( str, "%s", "FATFS_ERROR_NAME_DOES_EXIST_AS_DIR" ); break;
	case FATFS_ERROR_NAME_ALREADY_DOES_EXIST: sprintf( str, "%s", "FATFS_ERROR_NAME_ALREADY_DOES_EXIST" ); break;
	case FATFS_ERROR_NAME_WRONG_LONG_NAME: sprintf( str, "%s", "FATFS_ERROR_NAME_WRONG_LONG_NAME" ); break;
	case FATFS_ERROR_READ_SECTOR_FAILED: sprintf( str, "%s", "FATFS_ERROR_READ_SECTOR_FAILED" ); break;
	case FATFS_ERROR_WRITE_SECTOR_FAILED: sprintf( str, "%s", "FATFS_ERROR_WRITE_SECTOR_FAILED" ); break;
	case FATFS_ERROR_HDD_CAN_NOT_READ_DEV_INFO: sprintf( str, "%s", "FATFS_ERROR_HDD_CAN_NOT_READ_DEV_INFO" ); break;
	case FATFS_ERROR_FAT_TYPE_NOT_SUPPORTED: sprintf( str, "%s", "FATFS_ERROR_FAT_TYPE_NOT_SUPPORTED" ); break;
	case FATFS_ERROR_FAT_WRONG_ENTRY: sprintf( str, "%s", "FATFS_ERROR_FAT_WRONG_ENTRY" ); break;
	case FATFS_ERROR_FAT_MAX_LOG_DRIVES_REACHED: sprintf( str, "%s", "FATFS_ERROR_FAT_MAX_LOG_DRIVES_REACHED" ); break;
	case FATFS_ERROR_SYS_MARK_AA55_NOT_FOUND: sprintf( str, "%s", "FATFS_ERROR_SYS_MARK_AA55_NOT_FOUND" ); break;
	case FATFS_ERROR_SYS_MARK_41615252_NOT_FOUND: sprintf( str, "%s", "FATFS_ERROR_SYS_MARK_41615252_NOT_FOUND" ); break;
	case FATFS_ERROR_SYS_MARK_61417272_NOT_FOUND: sprintf( str, "%s", "FATFS_ERROR_SYS_MARK_61417272_NOT_FOUND" ); break;
	case FATFS_ERROR_SYS_MARK_AA550000_NOT_FOUND: sprintf( str, "%s", "FATFS_ERROR_SYS_MARK_AA550000_NOT_FOUND" ); break;
	case FATFS_ERROR_TOK_NO_TOKEN_FOUND: sprintf( str, "%s", "FATFS_ERROR_TOK_NO_TOKEN_FOUND" ); break;
	case FATFS_ERROR_TOK_THE_SEPARATOR_ITSELF: sprintf( str, "%s", "FATFS_ERROR_TOK_THE_SEPARATOR_ITSELF" ); break;
	case FATFS_ERROR_SEC_INVALID_SECTOR: sprintf( str, "%s", "FATFS_ERROR_SEC_INVALID_SECTOR" ); break;
	case FATFS_ERROR_SEC_INVALID_FAT_SECTOR: sprintf( str, "%s", "FATFS_ERROR_SEC_INVALID_FAT_SECTOR" ); break;
	case FATFS_ERROR_NO_ATA_DEVICES_FOUND: sprintf( str, "%s", "FATFS_ERROR_NO_ATA_DEVICES_FOUND" ); break;
	case FATFS_ERROR_NO_ATAPI_DEVICES_FOUND: sprintf( str, "%s", "FATFS_ERROR_NO_ATAPI_DEVICES_FOUND" ); break;
	case FATFS_ERROR_NO_DEVICES_FOUND: sprintf( str, "%s", "FATFS_ERROR_NO_DEVICES_FOUND" ); break;
	case FATFS_ERROR_LANG_NOT_SUPPORTED_LANGUAGE: sprintf( str, "%s", "FATFS_ERROR_LANG_NOT_SUPPORTED_LANGUAGE" ); break;
	case FATFS_ERROR_LANG_CODE_PAGE_TABLES_NOT_LOADED: sprintf( str, "%s", "FATFS_ERROR_LANG_CODE_PAGE_TABLES_NOT_LOADED" ); break;
	case FATFS_ERROR_LANG_INCORRECT_CODE_PAGE_TABLES_949: sprintf( str, "%s", "FATFS_ERROR_LANG_INCORRECT_CODE_PAGE_TABLES_949" ); break;
	case FATFS_ERROR_LANG_INCORRECT_CODE_PAGE_TABLES_950: sprintf( str, "%s", "FATFS_ERROR_LANG_INCORRECT_CODE_PAGE_TABLES_950" ); break;
	case FATFS_ERROR_LANG_INCORRECT_CODE_PAGE_TABLES_932: sprintf( str, "%s", "FATFS_ERROR_LANG_INCORRECT_CODE_PAGE_TABLES_932" ); break;
	case FATFS_ERROR_LANG_INCORRECT_CODE_PAGE_TABLES_936: sprintf( str, "%s", "FATFS_ERROR_LANG_INCORRECT_CODE_PAGE_TABLES_936" ); break;
	case FATFS_ERROR_FILE_NOT_FOUND: sprintf( str, "%s", "FATFS_ERROR_FILE_NOT_FOUND" ); break;
	case FATFS_ERROR_FILE_NAME_TOO_LONG: sprintf( str, "%s", "FATFS_ERROR_FILE_NAME_TOO_LONG" ); break;
	case FATFS_ERROR_FILE_NAME_ALREADY_EXIST_AS_DIR: sprintf( str, "%s", "FATFS_ERROR_FILE_NAME_ALREADY_EXIST_AS_DIR" ); break;
	case FATFS_ERROR_FILE_NAME_ALREADY_EXIST_AS_FILE: sprintf( str, "%s", "FATFS_ERROR_FILE_NAME_ALREADY_EXIST_AS_FILE" ); break;
	case FATFS_ERROR_FILE_NAME_NOT_FOUND: sprintf( str, "%s", "FATFS_ERROR_FILE_NAME_NOT_FOUND" ); break;
	case FATFS_ERROR_FILE_MAX_OPEN_FILES_REACHED: sprintf( str, "%s", "FATFS_ERROR_FILE_MAX_OPEN_FILES_REACHED" ); break;
	case FATFS_ERROR_FILE_INVALID_HANDLE: sprintf( str, "%s", "FATFS_ERROR_FILE_INVALID_HANDLE" ); break;
	case FATFS_ERROR_FILE_END_OF_FILE: sprintf( str, "%s", "FATFS_ERROR_FILE_END_OF_FILE" ); break;
	case FATFS_ERROR_FILE_WRITE_PROHIBITED: sprintf( str, "%s", "FATFS_ERROR_FILE_WRITE_PROHIBITED" ); break;
	case FATFS_ERROR_FILE_READ_PROHIBITED: sprintf( str, "%s", "FATFS_ERROR_FILE_READ_PROHIBITED" ); break;
	case FATFS_ERROR_FILE_SEEK_PROHIBITED: sprintf( str, "%s", "FATFS_ERROR_FILE_SEEK_PROHIBITED" ); break;
	case FATFS_ERROR_FILE_UNDEFINED_OPERATION: sprintf( str, "%s", "FATFS_ERROR_FILE_UNDEFINED_OPERATION" ); break;
	case FATFS_ERROR_FILE_SIZE_GREATER_THEN_RETURNED: sprintf( str, "%s", "FATFS_ERROR_FILE_SIZE_GREATER_THEN_RETURNED" ); break;
	case FATFS_ERROR_FILE_CURPOS_GREATER_THEN_RETURNED: sprintf( str, "%s", "FATFS_ERROR_FILE_CURPOS_GREATER_THEN_RETURNED" ); break;
	case FATFS_ERROR_FILE_INCORRECT_OPEN_MODE: sprintf( str, "%s", "FATFS_ERROR_FILE_INCORRECT_OPEN_MODE" ); break;
	case FATFS_ERROR_FILE_FAR_BEYOND_END_OF_FILE: sprintf( str, "%s", "FATFS_ERROR_FILE_FAR_BEYOND_END_OF_FILE" ); break;
	case FATFS_ERROR_FILE_INCORRECT_SIZE: sprintf( str, "%s", "FATFS_ERROR_FILE_INCORRECT_SIZE" ); break;
	case FATFS_ERROR_FILE_ALREADY_DOES_EXIST: sprintf( str, "%s", "FATFS_ERROR_FILE_ALREADY_DOES_EXIST" ); break;
	case FATFS_ERROR_FILE_CHAIN_IS_CORRUPTED: sprintf( str, "%s", "FATFS_ERROR_FILE_CHAIN_IS_CORRUPTED" ); break;
	case FATFS_ERROR_CLUS_INVALID_CLUSTER: sprintf( str, "%s", "FATFS_ERROR_CLUS_INVALID_CLUSTER" ); break;
	case FATFS_ERROR_CLUS_BAD_CLUSTER: sprintf( str, "%s", "FATFS_ERROR_CLUS_BAD_CLUSTER" ); break;
	case FATFS_ERROR_CLUS_END_CLUSTER: sprintf( str, "%s", "FATFS_ERROR_CLUS_END_CLUSTER" ); break;
	case FATFS_ERROR_CLUS_NO_FREE_CLUS_FOUND: sprintf( str, "%s", "FATFS_ERROR_CLUS_NO_FREE_CLUS_FOUND" ); break;
	case FATFS_ERROR_CLUS_NEXT_IS_BAD: sprintf( str, "%s", "FATFS_ERROR_CLUS_NEXT_IS_BAD" ); break;
	case FATFS_ERROR_CLUS_NEXT_IS_ZERO: sprintf( str, "%s", "FATFS_ERROR_CLUS_NEXT_IS_ZERO" ); break;
	case FATFS_ERROR_CLUS_NEXT_IS_OUT_OF_VOLUME: sprintf( str, "%s", "FATFS_ERROR_CLUS_NEXT_IS_OUT_OF_VOLUME" ); break;
	case FATFS_ERROR_CLUS_ENDLESS_CLUSTER_LOOP: sprintf( str, "%s", "FATFS_ERROR_CLUS_ENDLESS_CLUSTER_LOOP" ); break;
	case FATFS_ERROR_CLUS_INVALID_CLUSTER_CHAIN: sprintf( str, "%s", "FATFS_ERROR_CLUS_INVALID_CLUSTER_CHAIN" ); break;
	case FATFS_DRV_ERROR_NOT_SUPPORTED_CMD_ATA: sprintf( str, "%s", "FATFS_DRV_ERROR_NOT_SUPPORTED_CMD_ATA" ); break;
	case FATFS_DRV_ERROR_NOT_SUPPORTED_CMD_ATAPI: sprintf( str, "%s", "FATFS_DRV_ERROR_NOT_SUPPORTED_CMD_ATAPI" ); break;
	case FATFS_DRV_ERROR_NOT_SUPPORTED_CMD_SCSI: sprintf( str, "%s", "FATFS_DRV_ERROR_NOT_SUPPORTED_CMD_SCSI" ); break;
	case FATFS_DRV_ERROR_INVALID_HDD: sprintf( str, "%s", "FATFS_DRV_ERROR_INVALID_HDD" ); break;					// This ERR may come and from FATFS
	case FATFS_DRV_ERROR_INVALID_TRANSFER_MODE: sprintf( str, "%s", "FATFS_DRV_ERROR_INVALID_TRANSFER_MODE" ); break;			// This ERR may come and from FATFS
	case FATFS_DRV_ERROR_NOT_SUPPORTED_TRANSFER_MODE: sprintf( str, "%s", "FATFS_DRV_ERROR_NOT_SUPPORTED_TRANSFER_MODE" ); break;	// This ERR may come and from FATFS
	case FATFS_DRV_ERROR_NOT_SUPPORTED_48bit_LBA_MODE: sprintf( str, "%s", "FATFS_DRV_ERROR_NOT_SUPPORTED_48bit_LBA_MODE" ); break;
	case FATFS_DRV_ERROR_GENERAL: sprintf( str, "%s", "FATFS_DRV_ERROR_GENERAL" ); break;
	case FATFS_ERROR_GENERAL: sprintf( str, "%s", "FATFS_ERROR_GENERAL" ); break;
	case FATFS_ERROR_MAX: sprintf( str, "%s", "FATFS_ERROR_MAX" ); break;
	case FATFS_ERROR_NOT_SUPPORTED_SCSI_CMD: sprintf( str, "%s", "FATFS_ERROR_NOT_SUPPORTED_SCSI_CMD" ); break;
	case FATFS_ERROR_NOT_SUPPORTED_ATAPI_CMD: sprintf( str, "%s", "FATFS_ERROR_NOT_SUPPORTED_ATAPI_CMD" ); break;
	case FATFS_ERROR_NOT_SUPPORTED_ATA_CMD: sprintf( str, "%s", "FATFS_ERROR_NOT_SUPPORTED_ATA_CMD" ); break;
	}

	return(str);
}


