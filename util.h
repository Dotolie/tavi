//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : util.h
// 
// $Source: /home/cvsroot/TAVI020/tavi/util.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: util.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2006/02/14 03:20:45  zstein
// - added Execute(...)
//
// Revision 1.5  2005/12/01 03:30:24  zstein
// - changed loadmodule(name) to loadmodule(name,arg).
//
// Revision 1.4  2005/10/13 01:25:21  etnlwind
// Update for Header Info
//
//*****************************************************************************

#ifndef _UTIL_H
#define _UTIL_H

#include <unistd.h>
#include <stdio.h>
#include <sys/mount.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "rmbasic.h"
#include "bitstrm.h"


#define INSMOD_PATH "/"
#define INSMOD_PROG "minimod"

// load a kernel module - return 0 if ok, 1 if failed
int loadmodule (char *module, char* arg=NULL);

// mount the media
int mountmedia (void);

// unmount the media
int unmountmedia (void);

// disc inserted and ready?
int is_disc_ready (void);

// open/close tray
int disc_tray_operation (int open);

// tray closed?
int is_tray_closed (void);

// return time in milliseconds
int gettime_ms (void);

long gettime_sec(void);

void ParseVOL (RMuint8 *pBuf, RMuint32 length, RMint32 *pGMC, RMint32 *pQPEL);

extern void Execute( const char* program_name, char* const argv[] );

#endif
