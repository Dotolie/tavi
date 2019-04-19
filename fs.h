//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : SigmaDesigns FileSystem Header
// 
// $Source: /home/cvsroot/TAVI020/tavi/fs.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: fs.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/10/13 01:25:21  etnlwind
// Update for Header Info
//
//*****************************************************************************


#ifndef __FILESYSTEM_ACCESS_FUNCTIONS_H__
#define __FILESYSTEM_ACCESS_FUNCTIONS_H__

#include "fs_fat.h"

#define USE_FAT_FS		1
#define USE_LINUX_FS	0
#define USE_DPI_FS		0

extern RMint32 fs_filenameoffset (void);
extern RMuint32 fs_mountmedia (void);
extern RMuint32 fs_set_logical_drive (int drive);
extern RMuint32 fs_unmountmedia (void);
extern RMuint32 fs_eject (void);
extern RMuint32 fs_can_eject (void);
extern RMuint32 fs_powerdown (void);
extern RMuint32 fs_powerup (void);
extern void fs_set_flags (RMuint16 *flags);
extern RMuint32 fs_fopen (RMint8 *filename, void *context);
extern RMuint32 fs_fread (RMuint32 handle, void *buf, RMuint32 length, void *context);
extern RMuint32 fs_fseek (RMuint32 handle, RMuint32 pos, void *context);
extern RMuint32 fs_fseek2 (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context);
extern RMuint32 fs_ftell (RMuint32 handle, void *context);
extern RMuint32 fs_feof (RMuint32 handle, void *context);
extern RMuint32 fs_fclose (RMuint32 handle, void *context);
extern RMuint32 fs_getrootdir (RMint8 *dir, RMuint32 length, void *context);
extern RMuint32 fs_opendir (RMint8 *dir, void *context);
extern RMuint32 fs_readdir (RMuint32 handle, DIR_ENTRY_INFO *pDirInfo, void *context);
extern RMuint32 fs_closedir (RMuint32 handle, void *context);

#endif /* __FILESYSTEM_ACCESS_FUNCTIONS_H__ */
