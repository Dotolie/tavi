//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : File I/O for Media 
// 
// $Source: /home/cvsroot/TAVI020/tavi/mfileio.h,v $
// $Revision: 1.4 $
// $Date: 2006/04/19 04:11:52 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: mfileio.h,v $
// Revision 1.4  2006/04/19 04:11:52  zstein
// - add mio_enabled().
//
// Revision 1.3  2006/04/18 04:58:18  zstein
// - chnaged mio_wakeup(void) to mio_wakeup(bool foreward=false).
//
// Revision 1.2  2006/04/05 02:06:56  zstein
// - added hdd wakeup, sleep functions.
//
// Revision 1.1  2006/03/20 01:43:28  zstein
// - Adapter of buffering file I/O.
//
//*******************************************************************************

#ifndef __NML_MEDIA_FILE_IO_H__
#define __NML_MEDIA_FILE_IO_H__

#include "rmbasic.h"

extern RMuint32 mio_wfopen( RMuint16* filename, void* context );
extern RMuint32 mio_fread( RMuint32 handle, void* buf, RMuint32 size, void* context );
extern RMuint32 mio_fseek( RMuint32 handle, RMuint32 pos, void* context );
extern RMuint32 mio_fseek2( RMuint32 handle, RMuint32 pos, RMuint32 whence, void* context );
extern RMuint32 mio_ftell( RMuint32 handle, void* context );
extern RMuint32 mio_fclose( RMuint32 handle, void* context );
extern void mio_destroyer( void );
extern void mio_schedule( void );
extern bool mio_hddsleep( void );
extern void mio_wakeup( bool foreward=false );
extern bool mio_enabled( void );

#endif /* __NML_MEDIA_FILE_IO_H__ */
