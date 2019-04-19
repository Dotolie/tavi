//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : 	
// 				linux file system callbacks - header
// 				required by the filebrowser and the decoders/demuxers
// 
// $Source: /home/cvsroot/TAVI020/tavi/linuxfs.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: linuxfs.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/10/13 01:25:21  etnlwind
// Update for Header Info
//
//*****************************************************************************

#ifndef _LINUX_FS_H
#define _LINUX_FS_H

#include "rmbasic.h"
#include "filebrowser/filebrowser.h"

// power down/up media device
RMuint32 linux_powerdown ();
RMuint32 linux_powerup ();

// mount/unmount filesystem
RMuint32 linux_mount (void);
RMuint32 linux_unmount (void);
RMint32  linux_filenameoffset (void);
RMuint32 linux_eject (void);
RMuint32 linux_can_eject (void);
RMuint32 linux_set_logical_drive (int index);

// directory functions
RMuint32 linux_getrootdir (RMint8 *dir, RMuint32 length, void *context);
RMuint32 linux_opendir (RMint8 *dir, void *context);
RMuint32 linux_readdir (RMuint32 handle, DIR_ENTRY_INFO *pDirInfo, void *context);
RMuint32 linux_closedir (RMuint32 handle, void *context);

// file functions
RMuint32 linux_fopen (RMint8 *filename, void *context);
RMuint32 linux_fread (RMuint32 handle, void *buf, RMuint32 length, void *context);
RMuint32 linux_fseek (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context);
RMuint32 linux_ftell (RMuint32 handle, void *context);
RMuint32 linux_feof (RMuint32 handle, void *context);
RMuint32 linux_fclose (RMuint32 handle, void *context);

#endif

