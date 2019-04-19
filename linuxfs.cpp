//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : 	
// 				linux file system callbacks - source
// 				required by the filebrowser and the decoders/demuxers
// 
// $Source: /home/cvsroot/TAVI020/tavi/linuxfs.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: linuxfs.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/10/13 01:25:21  etnlwind
// Update for Header Info
//
//*****************************************************************************

//////////////////////////////////////////////////////////////////
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include "linuxfs.h"

//#define CDROM_ACCESS	1
#define HD_ACCESS		1

#if 0
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("***ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

RMuint32 linux_mount (void)
{
#ifdef CDROM_ACCESS
	return mount ("/dev/cdroms/cdrom0", "/hd1", "iso9660", MS_RDONLY | MS_NOSUID | MS_NODEV, NULL);
#endif
#ifdef HD_ACCESS
//	mount ("/dev/discs/disc0/part2", "/hd2", "vfat", MS_RDONLY | MS_NOSUID | MS_NODEV, NULL);
	return mount ("/dev/discs/disc0/part1", "/hd1", "vfat", MS_NOSUID | MS_NODEV, NULL);
#endif
}

RMuint32 linux_unmount (void)
{
	return umount ("/hd1");
}

//////////////////////////////////////////////////////////////////////////
#ifdef CDROM_ACCESS
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
int Atapi_TrayOperation (int handle, int open_tray)
{
	struct request_sense sense_info;
	struct cdrom_generic_command pc;	
	struct timeval tv1, tv2;
	struct timezone tz;
	int t1, t2;

	memset (&pc, 0, sizeof(pc));
	pc.cmd[0] = 0x00;
	pc.cmd[1] = 0x00;
	pc.cmd[2] = 0x00;
	pc.cmd[3] = 0x00;
	pc.cmd[4] = 0x00;
	pc.cmd[5] = 0x00;
	pc.cmd[6] = 0x00;
	pc.cmd[7] = 0x00;
	pc.cmd[8] = 0x00;
	pc.cmd[9] = 0x00;
	pc.cmd[10] = 0x00;
	pc.cmd[11] = 0x00;
	pc.buffer  = 0;
	pc.buflen  = 0;
	pc.sense   = &sense_info;
	pc.data_direction = 2;
	pc.timeout = 0;

	// allow eject/load
	pc.cmd[0] = 0x1e;
	ioctl (handle, CDROM_SEND_PACKET, &pc);

	pc.cmd[0] = 0x1b;
	pc.cmd[1] = 0x01;
	if (open_tray)
		pc.cmd[4] = 0x02;
	else
		pc.cmd[4] = 0x03;
	gettimeofday (&tv1, &tz);
	t1 = tv1.tv_sec * 1000 + tv1.tv_usec/1000;	
	if (ioctl (handle, CDROM_SEND_PACKET, &pc) == 0)
	{
		// wait some time to allow the tray to open
		// the drive in the prosonic player returns very quickly
		// from this ioctl, so the drive door is still closed ...
		// this causes us to display "please wait", when we do not
		// want to
		t2 = tv1.tv_sec * 1000 + tv1.tv_usec/1000;
		if ((t2 - t1) < 2000)
		{
			usleep ((2000-t2+t1)*1000);
		}
		return 0;
	}
	else
	{
		return -1;
	}
}
#endif
///////////////////////////////////////////////////////////////////////////

RMuint32 linux_eject (void)
{
#ifdef CDROM_ACCESS
	int handle = open ("/dev/cdroms/cdrom0", O_RDONLY | O_NONBLOCK);
	Atapi_TrayOperation (handle, 1);
	close (handle);
	return 1;
#endif
#ifdef HD_ACCESS
	ASSERT (0);
	return 0;
#endif
}

RMuint32 linux_can_eject (void)
{
#ifdef CDROM_ACCESS
	return 1;
#endif
#ifdef HD_ACCESS
	return 0;
#endif
}

RMint32 linux_filenameoffset (void)
{
	// the is no extra characters in the filename
	// what this filesystem returns is what the real filename is
	return 0;
}

static int linux_partition = 0;
RMuint32 linux_set_logical_drive (int index)
{
	linux_partition = index;
	return 0;
}

RMuint32 linux_getrootdir (RMint8 *dir, RMuint32 length, void *context)
{
	if (linux_partition)
		strcpy (dir, "/hd2");
	else
		strcpy (dir, "/hd1");
	return 0;
}

RMuint32 linux_opendir (RMint8 *dir, void *context)
{
	DEBUGMSG (1, ("opendir (%s)\n", dir));
	return (RMuint32)opendir (dir);
}

RMuint32 linux_readdir (RMuint32 handle, DIR_ENTRY_INFO *pDirInfo, void *context)
{
	RMint8 fullname[512];
	struct dirent *dir;
	struct stat sstat;

	printf("linux_readdir()\n");

	ASSERT(pDirInfo);
	dir = readdir ((DIR *)handle);
	if (dir == 0)
		return 0;
	pDirInfo->handle = 0;
	pDirInfo->flags = DIR_ENTRY_INFO_IS_FILE;
	sprintf (fullname, "%s/%s", pDirInfo->parentdir, dir->d_name);
	DEBUGMSG (1, ("stat (%s)\n", fullname));
	stat (fullname, &sstat);

	if( dir->d_name[0] == '.' )
		return DIR_ENTRY_INFO_IS_ROOT_OR_PARENT_OR_HIDDEN;

	if (S_ISDIR (sstat.st_mode))
	{
		DEBUGMSG (1, ("%s is a directory\n", fullname));
		pDirInfo->flags = DIR_ENTRY_INFO_IS_DIRECTORY;
	}

	strcpy (pDirInfo->name, dir->d_name);
	pDirInfo->namelength = strlen (dir->d_name);
	printf("linux_readdir(): file: [%s]\n", pDirInfo->name );

	return 1;
}

RMuint32 linux_closedir (RMuint32 handle, void *context)
{
	return closedir ((DIR *)handle);
}

RMuint32 linux_fopen (RMint8 *filename, void *context)
{
	RMuint32 handle;
	handle = (RMuint32)fopen (filename, "rb");
	DEBUGMSG ((handle == 0), ("fopen (%s) returns %d\n", filename, (int)handle));
	return handle;
}

RMuint32 linux_fread (RMuint32 handle, void *buf, RMuint32 length, void *context)
{
	return (RMuint32)fread (buf, 1, length, (FILE *)handle);
}

RMuint32 linux_fseek (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context)
{
	return (RMuint32)fseek ((FILE *)handle, pos, whence);
}

RMuint32 linux_ftell (RMuint32 handle, void *context)
{
	return (RMuint32)ftell ((FILE *)handle);
}

RMuint32 linux_feof (RMuint32 handle, void *context)
{
	return (RMuint32)feof ((FILE *)handle);
}

RMuint32 linux_fclose (RMuint32 handle, void *context)
{
	return (RMuint32)fclose ((FILE *)handle);
}

RMuint32 linux_powerdown ()
{
	// not implemented
	return 0;
}

RMuint32 linux_powerup ()
{
	// not implemented
	return 0;
}

