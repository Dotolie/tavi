//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : MP3 Record Interface
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/MP3Rec.pcm.crash.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: MP3Rec.pcm.crash.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2006/01/19 05:44:14  zstein
// *** empty log message ***
//
// Revision 1.10  2006/01/16 11:11:08  zstein
// *** empty log message ***
//
// Revision 1.9  2006/01/16 02:26:50  zstein
// - QC pass version
//
// Revision 1.8  2005/12/29 02:55:48  zstein
// *** empty log message ***
//
// Revision 1.7  2005/12/26 04:39:31  zstein
// *** empty log message ***
//
// Revision 1.6  2005/12/10 06:53:31  zstein
// *** empty log message ***
//
// Revision 1.5  2005/12/09 01:06:33  zstein
// - reduced write buffer size and deleted debug messages.
//
// Revision 1.4  2005/12/02 11:00:15  zstein
// *** empty log message ***
//
// Revision 1.3  2005/11/11 12:29:46  zstein
// *** empty log message ***
//
// Revision 1.2  2005/11/08 01:16:38  zstein
// *** empty log message ***
//
// Revision 1.1  2005/07/16 04:31:35  zstein
// - MP3 encoder
//
//
//*******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <mpegdec/em85xx.h>
#include <../libmp3enc/libmp3enc.h>
#include <Component/Stack.h>
#include <fatfs.h>
#include <Driver/key/tavi_key.h>

#include "MP3Rec.h"
// Macros

#define I2S_CTRL						((volatile RMuint32 *)(0x501600))
#define I2S_PROG_LEN				((volatile RMuint32 *)(0x501604))
#define I2S_STATUS					((volatile RMuint32 *)(0x501608))
#define I2S_FRAME_CNTR				((volatile RMuint32 *)(0x50160C))

#define SPII2S_DMA_CNTRL			((volatile RMuint32 *)(0x501700))
#define SPII2S_DMA_BASE_ADD		((volatile RMuint32 *)(0x501704))
#define SPII2S_DMA_SIZE				((volatile RMuint32 *)(0x501708))
#define SPII2S_DMA_WR_PTR			((volatile RMuint32 *)(0x50170C))
#define SPII2S_DMA_RD_PTR			((volatile RMuint32 *)(0x501710))
#define SPII2S_DMA_TRSH			((volatile RMuint32 *)(0x501714))
#define SPII2S_DMA_INT_EN			((volatile RMuint32 *)(0x501718))
#define SPII2S_DMA_INT				((volatile RMuint32 *)(0x50171C))
#define SPII2S_DMA_INT_POLL		((volatile RMuint32 *)(0x501720))
#define SPII2S_DMA_INTER_FIFO		((volatile RMuint32 *)(0x501724))
#define SPII2S_DMA_CNT				((volatile RMuint32 *)(0x501728))

#define CAPTURE_BUFFER_LENGTH		(1152*496)
#define PCM_CHUNK_COUNT			2
//#define CAPTURE_HALF_LENGTH		(1152*496/PCM_CHUNK_COUNT)
#define CAPTURE_HALF_LENGTH		(1024*496/PCM_CHUNK_COUNT)
#define ENCODED_BUFSIZE			(25600)
#define TEMPORARY_BUFSIZE			(25600)

#define DEFAULT_LAYER				3
#define DEFAULT_SAMPLERATE		44100
#define DEFAULT_BITRATE			128

// Extern variables
extern MpegDecoder* pMpegDecoder;
extern RMuint32* thumbnailY;

// Local variables
//static RMuint8* i2s_buf = (RMuint8*)thumbnailY;
static char* i2s_buf;
static char* pTempBuf;
MP3CODEC* pMP3Codec;
layer* pMP3Info;
static int nCurrentIdx;
static int nCurrentOffset;
static int nChunkIdx;
static int nEncodedOffset;
static int nPCMChunkSize;
static int nLimit;
static bool fInitialized;
static RMuint32 hFile;

struct PCM_Chunk {
	RMuint8* m_pBuf;
	bool m_fUsed;
};
static struct PCM_Chunk pPCMChunk[PCM_CHUNK_COUNT];

static void SwapBytes( char* pByte, int size )
{
	i32 i;    
	char *data, tmp;
	data = (char *)pByte;	
	for ( i = 0; i < size; i++ ) {
		tmp = data[0];
		data[0] = data[1];
		data[1] = tmp;
		data += 2;
	}
}

int MP3Rec_Init( int nBitrate, int nSamplePerSec, int nChannels )
{
	// allocate buffer
	i2s_buf = new char[CAPTURE_BUFFER_LENGTH];
	pTempBuf = new char[TEMPORARY_BUFSIZE];
    
	decoderproperty Dp;
	RMuint32 bitspersample = 16;
	Dp.PropSet = DECODER_SET;
	Dp.PropId = edecAudioDacBitsPerSample;
	Dp.PropTypeLength = sizeof (RMuint32);
	Dp.pValue = &bitspersample;
	ioctl (pMpegDecoder->m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	PCM_PARAMETERS pcmparams;
	pcmparams.NumberOfBitsPerSample = (RMuint32)bitspersample;
	pcmparams.NumberOfChannels = (RMuint32)nChannels;
	pcmparams.SamplesPerSecond = (RMuint32)nSamplePerSec;
	pMpegDecoder->SetPCMParameters (&pcmparams);
	pMpegDecoder->Play();

	if( fInitialized == false ) {
		pMP3Info = ( layer* )malloc( sizeof(layer) );
		memset( pMP3Info, 0, sizeof(layer) );
		if( !pMP3Info ) return -1;
		pMP3Info->lay = DEFAULT_LAYER; // Layer III
		pMP3Info->mode = MPG_MD_STEREO;
		pMP3Info->mode_ext = 0;
		pMP3Info->emphasis = 0;
		pMP3Info->copyright = 0;
		pMP3Info->original = 0;
		pMP3Info->error_protection = 0;
		pMP3Info->sampling_frequency = SmpFrqIndex( nSamplePerSec, &pMP3Info->version );
		pMP3Info->bitrate_index = getBitRateIndex( pMP3Info->lay, nBitrate, pMP3Info->version );

		// Initialize MP3 Codec
		pMP3Codec = ( MP3CODEC* )malloc( sizeof(MP3CODEC) );
		memset( pMP3Codec, 0, sizeof(MP3CODEC) );
		if( !pMP3Codec ) return -1; // out of memory
		
		if( init_MP3codec( pMP3Codec, pMP3Info ) == 0 ) {
			return -1;
		}

		printf( "version: %d, stereo: %d\n", pMP3Info->version, pMP3Codec->fr_ps.stereo );

		if( pMP3Info->lay == 3 && pMP3Info->version == 0 ) {
			if( pMP3Codec->fr_ps.stereo == 2 ) nPCMChunkSize = 1152;
			else nPCMChunkSize = 576;
		}
		else {
			if( pMP3Codec->fr_ps.stereo == 2 ) nPCMChunkSize = 2304;
			else nPCMChunkSize = 1152;
		}

		printf( "nPCMChunkSize: %d\n", nPCMChunkSize );
		printf( "pMP3Info: %p\n", pMP3Info );
		printf( "pMP3Codec: %p\n", pMP3Codec );
		printf( "pTempBuf: %p\n", pTempBuf );
		fInitialized = true;
	}
	//pEncodedBuf = (char*)i2s_buf + CAPTURE_BUFFER_LENGTH + 1024;
	//pTempBuf = pEncodedBuf + ENCODED_BUFSIZE + 1024;
	nCurrentIdx = -1;
	nChunkIdx = 0;
	nEncodedOffset = 0;
	nCurrentOffset = 0;
	return 0;
}

void MP3Rec_Exit( void )
{
	#if 0
	decoderproperty Dp;
	RMuint32 bitspersample;
	RMuint32 nBytes;
	int size;

	if( hFile ) {
		size = deinit_MP3codec( pMP3Codec, (u8*)pTempBuf );
		//fwrite( pEncodedBuf, 1, size, hFile );
		//fclose( hFile );
		//hFile = NULL;
		if( size ) fatfs_fwrite( hFile, (RMuint8*)pTempBuf, size, &nBytes );
		fatfs_fclose( hFile );
		hFile = 0;
	}
	
	bitspersample = 24;
	Dp.PropSet = DECODER_SET;
	Dp.PropId = edecAudioDacBitsPerSample;
	Dp.PropTypeLength = sizeof (RMuint32);
	Dp.pValue = &bitspersample;
	ioctl (pMpegDecoder->m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	if( pMP3Info ) {
		printf( "delete pMP3Info\n" );
		free( pMP3Info );
		pMP3Info = NULL;
	}
	
	if( pMP3Codec ) {
		printf( "delete pMP3Codec\n" );
		free( pMP3Codec );
		pMP3Codec = NULL;
	}
	#endif
}

bool MP3Rec_SetEncodingPath( const char* path )
{
	FATFS_ERROR error;
	error = fatfs_fopen( (RMuint8*)path, _O_CREAT | _O_TRUNC | _O_WRONLY, &hFile );
	if( error != FATFS_ERROR_NO_ERROR ) return false;

	return true;
}

// buffer is fulled then return 1, otherwise 0
#include <sys/time.h>
int MP3Rec_Recording( void )
{
	RMuint32 nPollState;
	RMuint32 n;
	int nWR;
	int nRD;
	int nBytes, nEncodedSize;

	if( *SPII2S_DMA_INTER_FIFO&0x01 ) {
		//printf( "UNDERFLOW\n" );
	}
	if( *SPII2S_DMA_INTER_FIFO&0x02 ) {
		//printf( "OVERFLOW\n" );
	}
	
	nPollState = *SPII2S_DMA_INT_POLL;
       if( nPollState&1 ) {
		return 0;
       }
       if( !(nPollState&4) ) return 0;

	nWR = *SPII2S_DMA_WR_PTR;
	nRD = *SPII2S_DMA_RD_PTR;
	// save data

	if( nCurrentIdx > -1 ) {
		//printf( "Buffer Full\n" );
		return 0;
	}

	nChunkIdx %= PCM_CHUNK_COUNT;
	nCurrentIdx = nChunkIdx;
	if( nChunkIdx == PCM_CHUNK_COUNT ) nRD = CAPTURE_BUFFER_LENGTH;
	else nRD = (nChunkIdx+1)*CAPTURE_HALF_LENGTH;
	*SPII2S_DMA_RD_PTR = nRD;

	nLimit = CAPTURE_HALF_LENGTH;
	nChunkIdx++;
	return 1;
}

static bool bStart;
void MP3Rec_Start( void )
{
	// reset
	*I2S_CTRL = 0x02;

	// program the i2s dma block
	*SPII2S_DMA_CNTRL = 0x2; // reset
	*SPII2S_DMA_BASE_ADD = (int)i2s_buf;
	*SPII2S_DMA_SIZE = CAPTURE_BUFFER_LENGTH;
	*SPII2S_DMA_RD_PTR = 0;

	// 0x01 : SPI/I2S enable
	// 0x08 : Circular buffer enable
	*SPII2S_DMA_CNTRL = 0x1 | 0x8;

	// after programming the dma engine, you can begin the transfer ...
	// 0x01 = enable i2s receiving
	// 0x80 = non i2s audiomode
	*I2S_CTRL = 0x01 | 0x80;
}

void MP3Rec_Stop( void )
{
    long reg;

    // process remained buffer
    //while( nCurrentIdx != -1 ) MP3Rec_Encoding();
    //MP3Rec_Encoding( true ); // flush
    //fatfs_fclose( hFile );
    //hFile = 0;

    pMpegDecoder->Stop();

    // Process remain DMA buffer
    reg = *SPII2S_DMA_CNTRL;
    reg &= 0x1f;
    int rd = *SPII2S_DMA_RD_PTR;
    int wd = *SPII2S_DMA_WR_PTR;
    int size;

    if( rd > wd ) {
    	size = CAPTURE_BUFFER_LENGTH - rd + wd;
    	nCurrentIdx = 0;
    }
    else {
    	size = wd -rd;
    	nCurrentIdx = 1;
    }

    if( !size ) nCurrentIdx = -1;

    // process remained buffer
    #if 0
    nLimit = size;
    while( nCurrentIdx != -1 ) MP3Rec_Encoding();
    #endif
    MP3Rec_Encoding();
    fatfs_fclose( hFile );
    hFile = 0;
	
    // diable I2S
    *I2S_CTRL = 0x00;

    // disable SPII2S DMA
    *SPII2S_DMA_CNTRL = 0x00;

    decoderproperty Dp;
    RMuint32 bitspersample;
    bitspersample = 24;
    Dp.PropSet = DECODER_SET;
    Dp.PropId = edecAudioDacBitsPerSample;
    Dp.PropTypeLength = sizeof (RMuint32);
    Dp.pValue = &bitspersample;
    ioctl (pMpegDecoder->m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

    // deallocate buffer
    delete[] i2s_buf;
    delete[] pTempBuf;
}

int MP3Rec_Encoding( bool fFlush )
{
    char* p;
    int nEncodedSize;
    int ret = 1;
    int offset;
    RMuint32 nBytes;

    if( hFile == 0 ) {
    	nCurrentIdx = -1;
    	return -1;
    }

    if( nCurrentIdx < 0 || nCurrentIdx > 1 ) {
        nCurrentIdx = -1;
        return 0;
    }

    #if 0
    offset = nCurrentIdx*CAPTURE_HALF_LENGTH+nCurrentOffset;
    if( offset > (CAPTURE_BUFFER_LENGTH-nPCMChunkSize) ) {
    	offset = CAPTURE_BUFFER_LENGTH - nPCMChunkSize;
    }
    p = i2s_buf+offset;
    SwapBytes( (char*)p, nPCMChunkSize );
    memcpy( pMP3Codec->buffer, p, nPCMChunkSize*2 );
    nEncodedSize = MP3_encoding( (u8*)pTempBuf, pMP3Codec );
    // Encoding error!!
    if( nEncodedSize > 0 ) {
        fatfs_fwrite( hFile, (RMuint8*)pTempBuf, nEncodedSize, &nBytes );
    }

    nCurrentOffset += nPCMChunkSize*2;

    if( nCurrentOffset >= nLimit ) {
        nCurrentOffset = 0;
        pPCMChunk[nCurrentIdx].m_fUsed = false;
        nCurrentIdx = -1;
    }
    #else 
    // WRITE PCM
    //p = i2s_buf + nCurrentIdx*CAPTURE_HALF_LENGTH;
    p = i2s_buf;
    fatfs_fwrite( hFile, (RMuint8*)p, CAPTURE_HALF_LENGTH, &nBytes );
    //nCurrentIdx = -1;
    #endif

    return ret;
}
