//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : File I/O for Media 
// 
// $Source: /home/cvsroot/TAVI020/tavi/mfileio.cpp,v $
// $Revision: 1.8 $
// $Date: 2006/05/10 02:41:40 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: mfileio.cpp,v $
// Revision 1.8  2006/05/10 02:41:40  zstein
// - changed chunk count.
//
// Revision 1.7  2006/04/19 09:34:16  zstein
// - deleted sleep on hdd wakeup.
//
// Revision 1.6  2006/04/19 04:11:36  zstein
// - add mio_enabled().
//
// Revision 1.5  2006/04/18 04:58:18  zstein
// - chnaged mio_wakeup(void) to mio_wakeup(bool foreward=false).
//
// Revision 1.4  2006/04/17 10:06:58  zstein
// - fixed bugs.
//
// Revision 1.3  2006/04/17 04:39:06  zstein
// *** empty log message ***
//
// Revision 1.2  2006/04/05 02:06:39  zstein
// - added hdd wakeup, sleep functions.
//
// Revision 1.1  2006/03/20 01:43:28  zstein
// - Adapter of buffering file I/O.
//
//*******************************************************************************

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include "bytestream.h"
#include "rmbasic.h"
#include "fatfs.h"
#include "fs_fat.h"
#include "mfileio.h"
#include "Task/AudioPlayer/AudioPlayer.h"

#define MIO_DEBUG       0
#if MIO_DEBUG
#define MIO_PRINT       printf
#else
#define MIO_PRINT
#endif

static ByteStream* s_pbytestream;

static int _hdd_wakeup( void )
{
    extern int ata_powerdown;
    
    ata_powerdown = 0;
    AP_HDD_Wakeup();
}


void mio_destroyer( void )
{
    if( s_pbytestream ) {
        delete s_pbytestream;
    }
    s_pbytestream = NULL;
}

RMuint32 mio_wfopen( RMuint16* path, void* context )
{
	RMuint32 fd = 0;
	FATFS_ERROR err;

	if( s_pbytestream ) {
	    delete s_pbytestream;
	}
	
	err = fatfs_wfopen ((RMuint16*)path, _O_RDONLY, &fd);
	if( err != FATFS_ERROR_NO_ERROR ) {
	    printf( "failed to open err(0x%x)\n", err );
	    return 0;
	}

	s_pbytestream = new ByteStream( 
	                        (int)fd,
	                        112,
	                        (int(*)(void))fs_fat_powerdown,
	                        (int(*)(void))_hdd_wakeup );
	                        
	
	return fd;
}


RMuint32 mio_fclose( RMuint32 handle, void* context )
{
    if( s_pbytestream ) {
        delete s_pbytestream;
    }

    s_pbytestream = NULL;
    
    return fatfs_fclose( handle );
}


RMuint32 mio_fread( RMuint32 handle, void* buf, RMuint32 size, void* context )
{
    RMuint32 n;
    if( s_pbytestream ) {
	    n = s_pbytestream->GetBytes( (char*)buf, (unsigned long)size );
    }
    else {
        n = 0;
    }
    MIO_PRINT( "mio_fread: %d, pos: %d\n", n, mio_ftell(handle, NULL) );

    return n;
}


RMuint32 mio_fseek( RMuint32 handle, RMuint32 pos, void* context )
{
    if( s_pbytestream ) {
	    return (RMuint32)s_pbytestream->Seek( ByteStream::BS_SEEK_SET, pos );
	}
	else {
	    return 0;
	}
}


RMuint32 mio_fseek2( RMuint32 handle, RMuint32 pos, RMuint32 whence, void* context )
{
    if( s_pbytestream ) {
	    return (RMuint32)s_pbytestream->Seek( (ByteStream::Seek_t)whence, pos );
	}
	else {
	    return 0;
	}
}


RMuint32 mio_ftell( RMuint32 handle, void* context )
{
    if( s_pbytestream ) {
        return (RMuint32)s_pbytestream->Tell();
    }
    else {
        return 0;
    }
}


void mio_schedule( void )
{
    if( s_pbytestream ) {
        struct timeval tv;
        gettimeofday( &tv, NULL );
        s_pbytestream->Schedule( tv.tv_sec, tv.tv_usec/1000 );
    }
}


/* ++

    mio_hddsleep( void )

Description:
    It returns whether the HDD is sleep or not.

Arguments:
    None

Return:
    true if the HDD is sleep, otherwise false.

-- */
bool mio_hddsleep( void )
{
    bool bsleep = false;

    if( s_pbytestream ) {
        bsleep = s_pbytestream->IsSleep();
    }

    return bsleep;
}


/* ++ 

    mio_wakeup( void )

Description:
    Wake up HDD.

Arguments:
    None

Return:
    None

-- */
void mio_wakeup( bool foreward )
{
    if( s_pbytestream ) {
        if( foreward ) {
            s_pbytestream->WakeupHDD2();
        }
        else {
            s_pbytestream->WakeupHDD();
        }
    }
}


/* ++

    mio_enabled()

Description:

Arguments:

Return:

-- */
bool mio_enabled()
{
    if( s_pbytestream ) {
        return true;
    }
    else {
        return false;
    }
}
