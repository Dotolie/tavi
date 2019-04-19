//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : SigmaDesigns FATFS header
// 
// $Source: /home/cvsroot/TAVI020/tavi/fs_fat.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: fs_fat.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/10/13 01:25:21  etnlwind
// Update for Header Info
//
//*****************************************************************************

#ifndef FS_FAT_H
#define FS_FAT_H

#include "rmbasic.h"
#include "filebrowser/filebrowser.h"

// power down/up media device
RMuint32 fs_fat_powerdown ();
RMuint32 fs_fat_powerup ();

// mount/unmount filesystem
RMuint32 fs_fat_mount (void);
RMuint32 fs_fat_unmount (void);
RMint32  fs_fat_filenameoffset (void);
RMuint32 fs_fat_eject (void);
RMuint32 fs_fat_can_eject (void);
RMuint32 fs_fat_set_logical_drive (int index);

// directory functions
RMuint32 fs_fat_getrootdir (RMint8 *dir, RMuint32 length, void *context);
RMuint32 fs_fat_opendir (RMint8 *dir, void *context);
RMuint32 fs_fat_readdir (RMuint32 handle, DIR_ENTRY_INFO *pDirInfo, void *context);
RMuint32 fs_fat_closedir (RMuint32 handle, void *context);

// file functions
RMuint32 fs_fat_fopen (RMint8 *filename, void *context);
RMuint32 fs_fat_wfopen (RMuint16 *filename, void *context);
RMuint32 fs_fat_wfopen_wronly (RMuint16 *filename, void *context);
RMuint32 fs_fat_fread (RMuint32 handle, void *buf, RMuint32 length, void *context);
RMuint32 fs_fat_fseek (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context);
RMuint32 fs_fat_ftell (RMuint32 handle, void *context);
RMuint32 fs_fat_feof (RMuint32 handle, void *context);

char* GetErrMsg( char* dest, int err );


RMuint32 fs_fat_fclose (RMuint32 handle, void *context);

#endif
