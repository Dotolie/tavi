//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : SigmaDesigns FileSystem Source
// 
// $Source: /home/cvsroot/TAVI020/tavi/fs.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: fs.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/10/13 01:25:21  etnlwind
// Update for Header Info
//
//*****************************************************************************

#include "linuxfs.h"
#include "fs.h"

 RMint32 fs_filenameoffset (void)
{
#if USE_LINUX_FS
	return linux_filenameoffset ();
#elif USE_FAT_FS
	return fs_fat_filenameoffset ();
#elif USE_DPI_FS
	return dpi_filenameoffset ();
#endif
}

 RMuint32 fs_mountmedia (void)
{
#if USE_LINUX_FS
	return linux_mount ();
#elif USE_FAT_FS
	return fs_fat_mount ();
#elif USE_DPI_FS
	return dpi_mount ();
#endif
}

 RMuint32 fs_set_logical_drive (int drive)
{
#if USE_LINUX_FS
	return linux_set_logical_drive (drive);
#elif USE_FAT_FS
	return fs_fat_set_logical_drive (drive);
#endif
	return 0;
}

 RMuint32 fs_unmountmedia (void)
{
#if USE_LINUX_FS
	return linux_unmount ();
#elif USE_FAT_FS
	return fs_fat_unmount ();
#elif USE_DPI_FS
	return dpi_unmount ();
#endif
}

 RMuint32 fs_eject (void)
{
#if USE_LINUX_FS
	return linux_eject ();
#elif USE_FAT_FS
	return fs_fat_eject ();
#elif USE_DPI_FS
	return dpi_eject ();
#endif
}

 RMuint32 fs_can_eject (void)
{
#if USE_LINUX_FS
	return linux_can_eject ();
#elif USE_FAT_FS
	return fs_fat_can_eject ();
#elif USE_DPI_FS
	return dpi_can_eject ();
#endif
}

 RMuint32 fs_powerdown (void)
{
return 0;
#if USE_LINUX_FS
	return linux_powerdown ();
#elif USE_FAT_FS
	return fs_fat_powerdown ();
#elif USE_DPI_FS
	return dpi_powerdown ();
#endif
}

 RMuint32 fs_powerup (void)
{
return 0;
#if USE_LINUX_FS
	return linux_powerup ();
#elif USE_FAT_FS
	return fs_fat_powerup ();
#elif USE_DPI_FS
	return dpi_powerup ();
#endif
}

 RMuint16 media_flags = 0;
 void fs_set_flags (RMuint16 *flags)
{
#if USE_LINUX_FS
	return;
#elif USE_FAT_FS
	return;
#elif USE_DPI_FS
	return dpi_set_flags (flags);
#endif
}

 RMuint32 fs_fopen (RMint8 *filename, void *context)
{
#if USE_LINUX_FS
	return linux_fopen (filename, context);
#elif USE_FAT_FS
	return fs_fat_fopen (filename, context);
#elif USE_DPI_FS
	return dpi_fopen (filename, context);
#endif
}

 RMuint32 fs_fread (RMuint32 handle, void *buf, RMuint32 length, void *context)
{
#if USE_LINUX_FS
	return linux_fread (handle, buf, length, context);
#elif USE_FAT_FS
	return fs_fat_fread (handle, buf, length, context);
#elif USE_DPI_FS
	return dpi_fread (handle, buf, length, context);
#endif
}

 RMuint32 fs_fseek (RMuint32 handle, RMuint32 pos, void *context)
{
#if USE_LINUX_FS
	return linux_fseek (handle, pos, SEEK_SET, context);
#elif USE_FAT_FS
	return fs_fat_fseek (handle, pos, SEEK_SET, context);
#elif USE_DPI_FS
	return dpi_fseek (handle, pos, SEEK_SET, context);
#endif
}

 RMuint32 fs_fseek2 (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context)
{
#if USE_LINUX_FS
	return linux_fseek (handle, pos, whence, context);
#elif USE_FAT_FS
	return fs_fat_fseek (handle, pos, whence, context);
#elif USE_DPI_FS
	return dpi_fseek (handle, pos, whence, context);
#endif
}

 RMuint32 fs_ftell (RMuint32 handle, void *context)
{
#if USE_LINUX_FS
	return linux_ftell (handle, context);
#elif USE_FAT_FS
	return fs_fat_ftell (handle, context);
#elif USE_DPI_FS
	return dpi_ftell (handle, context);
#endif
}

 RMuint32 fs_feof (RMuint32 handle, void *context)
{
#if USE_LINUX_FS
	return linux_feof (handle, context);
#elif USE_FAT_FS
	return fs_fat_feof (handle, context);
#endif
#if USE_DPI_FS
	return dpi_feof (handle, context);
#endif
}

 RMuint32 fs_fclose (RMuint32 handle, void *context)
{
#if USE_LINUX_FS
	return linux_fclose (handle, context);
#elif USE_FAT_FS
	return fs_fat_fclose (handle, context);
#elif USE_DPI_FS
	return dpi_fclose (handle, context);
#endif
}

 RMuint32 fs_getrootdir (RMint8 *dir, RMuint32 length, void *context)
{
#if USE_LINUX_FS
	return linux_getrootdir (dir, length, context);
#elif USE_FAT_FS
	return fs_fat_getrootdir (dir, length, context);
#elif USE_DPI_FS
	return dpi_getrootdir (dir, length, context);
#endif
}

 RMuint32 fs_opendir (RMint8 *dir, void *context)
{
#if USE_LINUX_FS
	return linux_opendir (dir, context);
#elif USE_FAT_FS
	return fs_fat_opendir (dir, context);
#elif USE_DPI_FS
	return dpi_opendir (dir, context);
#endif
}

 RMuint32 fs_readdir (RMuint32 handle, DIR_ENTRY_INFO *pDirInfo, void *context)
{
#if USE_LINUX_FS
	return linux_readdir (handle, pDirInfo, context);
#elif USE_FAT_FS
	return fs_fat_readdir (handle, pDirInfo, context);
#elif USE_DPI_FS
	return dpi_readdir (handle, pDirInfo, context);
#endif
}

 RMuint32 fs_closedir (RMuint32 handle, void *context)
{
#if USE_LINUX_FS
	return linux_closedir (handle, context);
#elif USE_FAT_FS
	return fs_fat_closedir (handle, context);
#elif USE_DPI_FS
	return dpi_closedir (handle, context);
#endif
}

