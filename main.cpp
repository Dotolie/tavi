//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Main Source Code File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/main.cpp,v $
// $Revision: 1.17 $
// $Date: 2006/06/12 04:45:18 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
// $Log: main.cpp,v $
// Revision 1.17  2006/06/12 04:45:18  ywkim
// Add function of CheckValidationOfSerialNo() & etc for melon
//
// Revision 1.16  2006/06/07 08:30:51  zstein
// *** empty log message ***
//
// Revision 1.15  2006/05/29 03:04:01  etnlwind
// -
//
// Revision 1.14  2006/05/10 02:38:35  zstein
// - changed media buffer size: 2MB to 4MB.
//
// Revision 1.13  2006/05/09 10:23:26  ywkim
// Add TAVI_WakeUp() in showbackgoround() for userbackground image
//
// Revision 1.12  2006/04/15 02:07:43  zstein
// - deleted unused codes.
//
// Revision 1.11  2006/04/14 12:38:18  ywkim
// Disable compare routine on SetPlayTime for wDisable compare routine for section repeat
//
// Revision 1.10  2006/04/07 04:22:37  zstein
// - video buffering and fixed video seek error.
//
// Revision 1.9  2006/04/05 02:40:19  zstein
// - fixed bug.
//
// Revision 1.8  2006/04/05 02:04:40  zstein
// - video buffering.
//
// Revision 1.7  2006/04/03 08:31:51  zstein
// - version back.
//
// Revision 1.6  2006/03/20 01:40:54  zstein
// - video buffering.
//
// Revision 1.5  2006/03/17 07:07:40  etnlwind
// Add header of <sys/shm.h>
//
// Revision 1.4  2006/03/09 01:39:07  zstein
// - deleted unused functions and variables.
//
// Revision 1.2  2006/02/22 13:02:54  zstein
// - removed unused header files.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.176  2006/02/21 04:43:49  ywkim
// Remove variable of sysparm & Move up _PreapareFor_StartupProgressbar() for TaviSysParam
// Replace DrawStartupProgress()
//
// Revision 1.175  2006/02/15 07:45:02  ywkim
// Remove instruction of misc_ctrl.ctrl_out &= ~(1<<MUTE_ON)
//
// Revision 1.174  2006/02/14 05:31:57  zstein
// - changed vfork to Execute.
//
// Revision 1.173  2006/02/14 05:20:19  zstein
// - clear alarm interrupt when booted by alarm.
//
// Revision 1.172  2006/02/10 09:38:51  zstein
// - moved initializing message queue
//
// Revision 1.171  2006/02/10 07:47:04  ywkim
// Move down loading instruction of sound.o after tavi_key.o
//
// Revision 1.170  2006/02/09 05:20:23  zstein
// - removed pop-noise
//
// Revision 1.169  2006/02/06 08:30:35  zstein
// - deleted unnecessary pMpegDecoder->DisplayYUV420(0,0,0,0,0,0,0).
//
// Revision 1.168  2006/02/03 09:35:59  zstein
// - added cvs log and changed clearbackground().
//
//*****************************************************************************

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <pty.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include "jpeg/jpeg.h"
#include "mpegdec/mpegdec.h"
#include "mpegdec/em85xx.h"
#include "util.h"
#include "mp3/mp3.h"
#include "ac3/ac3.h"
#include "avi/avi.h"
#include "aac/aac.h"
#include "mp4/mp4.h"
#include "bitstrm.h"
#include "vol.h"
#include "mpgparse/mpgparse.h"
#include "lcd/lcd.h"
#include "filebrowser/filebrowser.h"

#include "realmagichwl.h"
#include "jasperlib.h"

#include "decrypt/decrypt.h"

#include "linuxfs.h"
#include "fs_fat.h"

#include "queue.h"

#include "scanner.h"

#define MOD_PATH_PREFIX		"/lib/modules/"
#define MOD_NAME_EXT			".o"

#include "fs.h"
#include <fatfs.h>


#include "3Dsound/3Dsound.h"

/* Add for TAVI task*/
#include "TypeDef.h"
#include "Task/Task.h"
#include "Task/tavi_global.h"
#include "include/tavi_structures.h"
#include "include/tavi_palette.h"
#include "include/bitmap.h"
#include "include/define_fs.h"
#include "include/Config.h"
#include "Component/image.h"
#include "Lib/LibNFile.h"
#include "Task/MiscCtrl.h"
#include "Task/AudioPlayer/AudioCmd.h"
#include "Driver/rtc/tavi_rtc.h"

#include "Lib/LibUtilities.h"
#include <sys/shm.h>
#include "mfileio.h"    /* for video buffering */

#define MAX_VIDEO_RESOLUTION (720*576)
//#define MAX_VIDEO_RESOLUTION (720*480)
#define N_MP43_SCAN_BUFFER 8

BITMAPFILEINFO256 TAVI_IMAGES[256];

#if 0
#define DEBUGWAIT(arg1) for(int loop=0;loop<arg1;loop++) { sleep(1); }
#define DEBUGMAINMSG DEBUGMSG
#else
#define DEBUGWAIT(arg1) 
#define DEBUGMAINMSG DEBUGMSG
#endif

#define DEBUG_AVI       0
#if DEBUG_AVI
#define AVI_DEBUG       printf
#else
#define AVI_DEBUG
#endif

///////////////////////////////////////////////////////////////////////////////////////
// Addtional Values
VIDEOPLAYER_CALLBACK_TABLE* pcbtVideo;
VIDEOFILE_PLAYINFO gVideofilePlayinfo;
TAVI_TIME playtime, totaltime;

static int prevSeekValue;


void SetPlayTime();
void sw_decode_mp3 (void);
int Seek7( int t );
void showbackground (char *bkgnd);
void showbackground (int menuID);


#if 0
// 16 MB configuration - more memory is used to buffer data
#if SHM_SCHEME
#define DATA_BUFFER_SIZE				(1024*1024 + 205*1024)
#else
#define DATA_BUFFER_SIZE				(1024*1024*10 + 133*1024)
#endif
extern "C" 
{
static RMuint32 databuffers[DATA_BUFFER_SIZE/4];
}

// memory map for databuffers
// [thumbnailY]							 20480
// [thumbnailUV]						 10240
// [backgroundY]						408960
// [backgroundUV]						204480
// [scaled_backgroundY]					408960	*used by screen saver also
//                                     	------
//                                     1053120  = total so far
// [scaled_backgroundUV/mp3/wma/etc]	MAIN_DATA_BUFFER_SIZE
// [pcm]								PCM_DATA_BUFFER_SIZE

#define SCALED_Y_LENGTH					(852*480)
#if	SHM_SCHEME
#define	MAIN_DATA_BUFFER_SIZE		MB_DATABUFFER_SIZE
#define	PCM_DATA_BUFFER_SIZE			MB_PCMBUFFER_SIZE
#else
#define	MAIN_DATA_BUFFER_SIZE		(1024*1024*7)
#define	PCM_DATA_BUFFER_SIZE			(DATA_BUFFER_SIZE - MAIN_DATA_BUFFER_SIZE - 408960 - 204480 - 408960 - 10240 - 20480)
#endif

// data block sizes
#define AVI_BLOCK_SIZE					(1024*256)	// read this many bytes at a time
#define AVI_NBLOCKS						(28)		// total of this many blocks
#define AVI_LOW_THRESHOLD				(10)
#define MP4_BLOCK_SIZE					(1024*256)	// read this many bytes at a time
#define MP4_NBLOCKS						(28)		// total of this many blocks
#define MP4_LOW_THRESHOLD				(10)
#define MPG_BLOCK_SIZE					(1024*128)
#define MPG_NBLOCKS						(56)
#define MP3_BLOCK_SIZE					(1024*512)
#define MP3_NBLOCKS						(8)
#define WMA_BLOCK_SIZE					(1024*1024*4)
#define WMA_NBLOCKS						(1)
#define OGG_BLOCK_SIZE					(1024*1024*4)
#define OGG_NBLOCKS						(1)
#define AC3_BLOCK_SIZE					(16*1024)
#define AC3_NBLOCKS						(256)
#define	AC3_LOW_THRESHOLD				(64)
#define PCM_BLOCK_SIZE					(1152*4)
#define PCM_NBLOCKS						(64)
#define PCM_MP3_BLOCK_SIZE				(1152*4)
#define PCM_MP3_NBLOCKS					(64)
#define PCM_WMA_BLOCK_SIZE				(1152*4)
#define PCM_WMA_NBLOCKS					(64)
#define PCM_OGG_BLOCK_SIZE				(4096)
#define PCM_OGG_NBLOCKS					(64)
#endif

#if 1
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
//
//#define DATA_BUFFER_SIZE				(1024*3205)
//#define DATA_BUFFER_SIZE				(1024*(3205+1920))
#define DATA_BUFFER_SIZE                (1024*(1030+4096+1152))
extern "C" 
{
	static RMuint32 databuffers[DATA_BUFFER_SIZE/4];
}
// memory map for databuffers
// [thumbnailY]							20480
// [thumbnailUV]							10240
// [backgroundY]							408960
// [backgroundUV]							204480
// [scaled_backgroundY]					408960	*used by screen saver also
//                                     		------
//										1053120  = total so far
// [scaled_backgroundUV/mp3/wma/avi/etc]	MAIN_DATA_BUFFER_SIZE
// [pcm]									PCM_DATA_BUFFER_SIZE
#define SCALED_Y_LENGTH				(852*480)
#define MAIN_DATA_BUFFER_SIZE			(1024*1024*4)
#define PCM_DATA_BUFFER_SIZE			(DATA_BUFFER_SIZE - MAIN_DATA_BUFFER_SIZE - 408960 - 204480 - 408960 - 10240 - 20480)
// data block sizes
#define AVI_BLOCK_SIZE					(1024*64)	// read this many bytes at a time
#define AVI_NBLOCKS						(64)		// total of this many blocks
#define AVI_LOW_THRESHOLD				(48)
#define MP4_BLOCK_SIZE					(1024*64)	// read this many bytes at a time
#define MP4_NBLOCKS						(64)		// total of this many blocks
#define MP4_LOW_THRESHOLD				(48)
#define MPG_BLOCK_SIZE					(1024*128)
#define MPG_NBLOCKS					    (32)
#define MP3_BLOCK_SIZE					(1024*512)
#define MP3_NBLOCKS						(2)
#define WMA_BLOCK_SIZE					(1024*512)
#define WMA_NBLOCKS					(2)
#define OGG_BLOCK_SIZE					(1024*512)
#define OGG_NBLOCKS					(1)
#define AC3_BLOCK_SIZE					(16*1024)
#define AC3_NBLOCKS						(64)
#define AC3_LOW_THRESHOLD				(4)
#define PCM_BLOCK_SIZE					(1152*4)
#define PCM_NBLOCKS						(256)
#define PCM_MP3_BLOCK_SIZE				(1152*4)
#define PCM_MP3_NBLOCKS				(256)
#define PCM_WMA_BLOCK_SIZE			(1152*4)
#define PCM_WMA_NBLOCKS				(256)
#define PCM_OGG_BLOCK_SIZE				(4096)
#define PCM_OGG_NBLOCKS				(256)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

/*

	Global Variables!!!

*/
#define	DEBUG_BATTERY_REQ					"/hd1/debug_battery.txt"
#define	DEBUG_NO_SHUTDOWN_ON_LOW_BATTERY	"/hd1/debug_no_shutdown_on_low_battery.txt"

/* BATTERY DEBUG */
int g_debug_battery						= 0;
int g_debug_no_shutdown_on_low_battery	= 0;

// thumbnail image memory
 RMuint32 *thumbnailY = (databuffers);
 RMuint32 *thumbnailUV = (thumbnailY + 20480/4);
// background image memory
 RMuint32 *backgroundY = (thumbnailUV + 10240/4);
 RMuint32 *backgroundUV = (backgroundY + 408960/4);
 RMuint32 *scaled_backgroundY = (backgroundUV + 204480/4);
 RMuint32 *scaled_backgroundUV = (scaled_backgroundY + 408960/4);
// mp3 buffers
 RMuint32 *mp3_buffer = (scaled_backgroundUV);
 BufferPool BPmp3;
// wma buffers
 RMuint32 *wma_buffer = (mp3_buffer);
 BufferPool BPwma;
// ogg buffers
 RMuint32 *ogg_buffer = (mp3_buffer);
 BufferPool BPogg;
// ac3 buffers
 RMuint32 *ac3_buffer = (mp3_buffer);
 BufferPool BPac3;
 RMint32 ac3_rebuffer = 0;
// decoded mp3/wma/ogg buffers
 RMuint32 *pcm_buffer = (wma_buffer + (MAIN_DATA_BUFFER_SIZE)/4);
 BufferPool BPpcm;
 RMuint32 tbytes = 0;
 RMuint32 NumberOfBitsPerSample = 0;
 RMuint32 NumberOfChannels = 0;
 RMuint32 SamplesPerSecond = 0;
 RMuint32 BytesPerSecond = 0;
 RMuint32 CompressedBytesPerSecond = 0;
 RMuint32 prebuffering = 0;
// mpeg decoder
MpegDecoder *pMpegDecoder = 0;
 RMint32 image_blockinghandle = 0;
 RMint32 image_duration = 0;
 RMint32 image_t0 = 0;
 RMint32 audio_blockinghandle = 0;
 RMint32 video_blockinghandle = 0;
// avi buffers
 BufferPool BPavi;
 RMuint32 *avi_buffer = (mp3_buffer);
 int avi_rebuffer = 0;
 putChunkQueue mp3chunkQ;	// to queue mp3 chunks
 AVI_BITMAPINFO AviBmpInfo;

// mp4 demux
 BufferPool BPmp4;
 RMuint32 *mp4_buffer = (mp3_buffer);
 RMuint8 aacframe[1024*64];	// max length for an aac frame?
 RMuint32 aacframe_valid = 0;
 RMint32 aacframe_count = 0;
 RMint32 aacdecode_time = 0;
 RMint64 mp4_scrDelay = 0;
 RMint64 mp4_scr0 = 0;
 RMuint32 vopTimeIncrRes = 30000;
 RMuint32 videoTimeScale = 30000;
 RMuint32 audioTimeScale = 30000;
 RMuint32 mp4_VOLHdr[64];
 RMuint32 mp4_VOLHdrLength = 0;
 RMint32 selected_audiostream = 0;
 RMint32 selected_spstream = 0;
// decode/demux objects
 AVIDemux *pAVIDemux = 0;
 MP3Decoder *pMP3BufferDecoder = 0;
// WMADecoder *pWMADecoder = 0;
 MP3Decoder *pMP3Decoder = 0;
// OGGDecoder *pOGGDecoder = 0;
 AC3Demux *pAC3Demux = 0;
 MP4Demux *pMP4Demux = 0;
 AACDecoder *pAACDecoder = 0;
// for mpeg playback
 MPEGDemux *pMPGDemux = 0;
 BufferPool BPmpg;
 RMuint32 *mpg_buffer = (mp3_buffer);
 RMuint8 videoStreamId = 0xff;
 RMuint8 audioStreamId = 0xff;
 RMuint8 audioSubStreamId = 0xff;
 RMint32 audioFreq = 0;
// for large jpeg memory allocation in libjpeg
RMuint8 *jpeg_buffer = ((RMuint8 *)mp3_buffer) + SCALED_Y_LENGTH;
RMuint32 jpeg_buffer_length = (MAIN_DATA_BUFFER_SIZE + PCM_DATA_BUFFER_SIZE - SCALED_Y_LENGTH);
// ms-mpeg4v3 scanning
 SCANNER_PARAM MP43Scanner;
 RMint32 mp43scan_status = 0;
 RMuint32 *mp43scan_buffer = backgroundY;
 RMuint32 *mp43pending_buffer = mp43scan_buffer + (1024*128/4);
 BufferPool BPscan;
// for AVI playback
#define AVI_DELAY_MS	250
 RMint64 aviTimeScale = 0;
 RMint64 aviStartupDelay = 0;
 RMint64 aviTime0 = 0;
 RMint64 audioTime = 0;
 RMint64 videoTime = 0;
 RMint64 aviFrameCounter = 0;
 RMint64 aviTicksPerFrame = 0;
 RMint32 ac3FrameSize = 0;
static RMint32 dtsFrameSize = 0;
static RMint32 avi_initial_start = 0;
static RMint32 KeyFramesOnly = 0;
int IsXvidGmcEnabled=FALSE;
int IsInvalidFPSVideoEnabled=FALSE;

RMint32 frmsizecod_table[4][38] = 
{
	// 48
	{64,64,80,80,96,96,112,112,128,128,160,160,192,192,224,224,256,256,
	 320,320,384,384, 448,448,512,512,640,640,768,768,896,896,1024,1024,
	 1152,1152,1280,1280},
	// 44.1
	{69,70,87,88,104,105,121,122,139,140,174,175,208,209,243,244,278,
	 279,348,349,417,418,487,488,557,558,696,697,835,836,975,976,1114,
	 1115,1253,1254,1393,1394},
	// 32
	{96,96,120,120,144,144,168,168,192,192,240,240,288,288,336,336,384,
	 384,480,480,576,576,672,672,768,768,960,960,1152,1152,1344,1344,
	 1536,1536,1728,1728,1920,1920}
};
#define DIVX_FOURCC		AVI_FOURCC('D','I','V','X')
#define divx_FOURCC		AVI_FOURCC('d','i','v','x')
#define _3IVX_FOURCC	AVI_FOURCC('3','I','V','X')
#define _3ivx_FOURCC	AVI_FOURCC('3','i','v','x')
#define DX50_FOURCC		AVI_FOURCC('D','X','5','0')
#define XVID_FOURCC		AVI_FOURCC('X','V','I','D')
#define xvid_FOURCC		AVI_FOURCC('x','v','i','d')
#define XCRD_FOURCC		AVI_FOURCC('X','C','R','D')
#define RMP4_FOURCC		AVI_FOURCC('R','M','P','4')
#define div3_FOURCC		AVI_FOURCC('d','i','v','3')
#define DIV3_FOURCC		AVI_FOURCC('D','I','V','3')
#define mp43_FOURCC		AVI_FOURCC('m','p','4','3')
#define MP43_FOURCC		AVI_FOURCC('M','P','4','3')
#define mp42_FOURCC		AVI_FOURCC('m','p','4','2')
#define MP42_FOURCC		AVI_FOURCC('M','P','4','2')
 RMuint32 currentFOURCC = 0;
 RMint32 currentWidth = 0;
 RMint32 currentHeight = 0;
 RMint32 currentRate = 0;
 RMint32 currentScale = 0;
 RMuint32 transcodeVideo = 0;
 RMint64 currentTime = 0;

/*
    AVI Informations
*/
static int _aviauBlockPerSec;
static int _aviauAvgBytesPerSec;
static int _aviauLength;
static int _aviMoviChunkSize;
static int _avividFramesPerSec;
static AVI_WAVEFORMATEX _aviWaveFmt;


/*
    Video Informations
*/
static int _vidTotalFrames;

// allowed AVI fourccs
 RMint32 IsFOURCCSupported (RMuint32 fourcc)
{
	// iso mpeg-4
	if ((fourcc == DIVX_FOURCC) ||
		(fourcc == divx_FOURCC) ||
		(fourcc == _3IVX_FOURCC) ||
		(fourcc == _3ivx_FOURCC) ||
		(fourcc == DX50_FOURCC) ||
		(fourcc == XVID_FOURCC) ||
		(fourcc == xvid_FOURCC) ||
		(fourcc == XCRD_FOURCC) ||
		(fourcc == RMP4_FOURCC))
	{
		transcodeVideo = 0;
		return 1;
	}
	// microsoft mpeg-4 version 3
	if ((fourcc == div3_FOURCC) ||
		(fourcc == DIV3_FOURCC) ||
		(fourcc == mp43_FOURCC) ||
		(fourcc == MP43_FOURCC))
	{
		transcodeVideo = 1;
		return 1;
	}
	return 0;
}

// min (x,y) macro
#define MY_MIN(X,Y) (((X)<=(Y))?(X):(Y))	// to find the min between two number.
// max (x,y) macro
#define MY_MAX(X,Y) (((X)>=(Y))?(X):(Y))	// to find the max between two number.


RMint32 toggle_display = 0;

RMuint32 *shm_buffer = mp3_buffer;				// for shared memory


/////////////////////////////////////////////////////////////////
// file system callbacks:
// required by the various decoders/demuxers
//
//#define USE_LINUX_FS	1
#define USE_FAT_FS		1
//#define USE_DPI_FS		1



// addref a buffer - increment its reference counter
static RMuint32 my_addref (RMuint8 *pBuffer, void *context)
{
	BufferPool *BP;
	if (((int)pBuffer >= (int)mp3_buffer) && 
		((int)pBuffer < ((int)mp3_buffer + (MAIN_DATA_BUFFER_SIZE))))
	{
		if (pMP3Decoder)
		{
			ASSERT (pAVIDemux == 0);
			ASSERT (pMPGDemux == 0);
			ASSERT (pAC3Demux == 0);
			ASSERT (pMP4Demux == 0);
			BP = &BPmp3;
		}

		else if (pAVIDemux)
		{
			ASSERT (pMPGDemux == 0);
			ASSERT (pAC3Demux == 0);
			ASSERT (pMP4Demux == 0);
			BP = &BPavi;
		}
		else if (pMPGDemux)
		{
			ASSERT (pAC3Demux == 0);
			ASSERT (pMP4Demux == 0);
			BP = &BPmpg;			
		}
		else if (pAC3Demux)
		{
			ASSERT (pMP4Demux == 0);
			BP = &BPac3;
		}
		else if (pMP4Demux)
		{
			BP = &BPmp4;
		}
		else
			ASSERT (0);
	}
	else
	{
		BP = &BPpcm;
	}
	int idx = ((int)pBuffer - (int)BP->start_address) / BP->buffers_size;
	ASSERT (idx >= 0);
	addref_buffer (&(BP->B[idx]));
	return 0;
}

// release a buffer - decrement it's reference counter
static RMuint32 my_release (RMuint8 *pBuffer, void *context)
{
	BufferPool *BP;
	if (((int)pBuffer >= (int)mp3_buffer) && 
		((int)pBuffer < ((int)mp3_buffer + (MAIN_DATA_BUFFER_SIZE))))
	{
		if (pMP3Decoder)
		{
			ASSERT (pAVIDemux == 0);
			ASSERT (pMPGDemux == 0);
			ASSERT (pAC3Demux == 0);
			ASSERT (pMP4Demux == 0);
			BP = &BPmp3;
		}
		else if (pAVIDemux)
		{
			ASSERT (pMPGDemux == 0);
			ASSERT (pAC3Demux == 0);
			ASSERT (pMP4Demux == 0);
			BP = &BPavi;
		}
		else if (pMPGDemux)
		{
			ASSERT (pAC3Demux == 0);
			ASSERT (pMP4Demux == 0);
			BP = &BPmpg;			
		}
		else if (pAC3Demux)
		{
			ASSERT (pMP4Demux == 0);
			BP = &BPac3;
		}
		else if (pMP4Demux)
		{
			BP = &BPmp4;
		}
		else
			ASSERT (0);
	}
	else
	{
		BP = &BPpcm;
	}
	int idx = ((int)pBuffer - (int)BP->start_address) / BP->buffers_size;

	release_buffer (&(BP->B[idx]));
	return 0;
}


// wma/mp3 callback - the wma/mp3 decoding objects are requesting
// the application render some pcm samples
static RMuint32 mp3wma_putPCM (RMuint8 *pBuffer, RMuint32 length, void *context)
{
	BufferPool *BP;
	ASSERT (((int)pBuffer >= (int)pcm_buffer) && ((int)pBuffer < ((int)pcm_buffer + PCM_DATA_BUFFER_SIZE)));
	BP = &BPpcm;
	int idx = ((int)pBuffer - (int)BP->start_address) / BP->buffers_size;
	addref_buffer (&(BP->B[idx]));
	DEBUGMAINMSG (0, ("mp3wma_putPCM (%d)\n", idx));
	DEBUGMAINMSG (0, ("mp3wma_putPCM: address = 0x%08lx, length = %d, idx = %d\n", 
		(RMuint32)pBuffer, (RMint32)length, idx));
	RMuint32 flag = 0;
	if (aviTimeScale)
		flag = CTS_AVAILABLE_FLAG;
	DEBUGMAINMSG (0, ("WritePCM (%d, %d)\n", (RMint32)audioTime, (RMint32)aviTime0));

	XEN_SoundProcessWithNoESA( pBuffer, length/4 );	// emx 3d
	
	if (pMpegDecoder->WritePCM (pBuffer, length, &(BP->B[idx]), flag, aviTime0, audioTime) == MPEG_DECODER_ERROR_NO_ERROR)
	{
		DEBUGMAINMSG (0, ("WritePCM (%d)\n", (RMint32)audioTime));
		tbytes += length;
		ASSERT (BytesPerSecond);
		audioTime = aviStartupDelay + 
		            ((RMint64)tbytes * (RMint64)aviTimeScale / (RMint64)BytesPerSecond);
		return 0;
	}
	release_buffer (&(BP->B[idx]));
	return 1;
}

// wma/mp3 callback - the mp3/wma decoding objects are requesting
// the application to provide a buffer to place the decoded
// pcm samples into
static RMuint32 mp3wma_getPCM (RMuint8 **pBuffer, RMuint32 *plength, void *context)
{
	DEBUGMAINMSG (0, ("mp3wma_getPCM\n"));

	int idx = find_free_buffer (&BPpcm, 0);
	if (idx < 0)
	{
		if (prebuffering)
		{
			fs_powerdown ();
			DEBUGMAINMSG (1, ("start decode due to pcm buffer full\n"));
			prebuffering = 0;
			pMpegDecoder->Play ();
		}
		if (audio_blockinghandle)
			fs_powerdown ();
		return 1;
	}
	DEBUGMAINMSG (0, ("mp3wma_getPCM (%d)\n", idx));
	*plength = (RMuint32)BPpcm.buffers_size;
	*pBuffer = (RMuint8 *)get_buffer_address (&BPpcm, idx);
	ASSERT (((int)*pBuffer >= (int)pcm_buffer) && ((int)*pBuffer < ((int)pcm_buffer + PCM_DATA_BUFFER_SIZE)));

	return 0;
}

// avi demux callback function - the avi demux is providing the
// application with some useful information about the avi file 
// that is currently being demuxed.  this is called withing the context
// of ::DemuxFile
static RMint32 video_stream_count = 0;
static RMint32 audio_stream_count = 0;
static RMint32 avi_stream_count = 0;
static RMint8 selected_audio_stream[3];
RMint16 selected_audio_stream_format = 0;

// Test Function ... by Sangjin
void clearbackground()
{
	unsigned char* pY = (unsigned char*)backgroundY;
	unsigned char* pUV = (unsigned char*)backgroundUV;

	memset( pY, 0, 720*480 );
	memset( pUV, 0x80, 720*480*2 );
	pMpegDecoder->DisplayYUV420( (RMuint8*)pY, (RMuint8*)pUV, 0, 0, 720, 480, 720, 480);
	usleep( 100 );
}

static RMuint32 avi_info_V19 (RMint32 msg, void *info, void *context)
{
	AVI_MAIN_HEADER *pAVIHdr;
	AVI_STREAM_HEADER *pStreamHdr;
	AVI_BITMAPINFO *pBitmapInfo;
	AVI_WAVEFORMATEX *pWaveFormatEx;
	RMint32 w, h, n, d;

	switch (msg)
	{
	case AVI_DEMUX_MSG_AVIHDR:
		pAVIHdr = (AVI_MAIN_HEADER *)info;
		_vidTotalFrames = pAVIHdr->TotalFrames;
		AVI_DEBUG("AVI_MAIN_HEADER(avi_info_V19):\n");
		AVI_DEBUG(" MicroSecPerFrame = %lu\n", pAVIHdr->MicroSecPerFrame);
		AVI_DEBUG(" MaxBytesPerSec = %lu\n", pAVIHdr->MaxBytesPerSec);
		AVI_DEBUG(" Flags = %lu\n", pAVIHdr->Flags);
		AVI_DEBUG(" TotalFrames = %lu\n", pAVIHdr->TotalFrames);
		AVI_DEBUG(" InitialFrames = %lu\n", pAVIHdr->InitialFrames);
		AVI_DEBUG(" Streams = %lu\n", pAVIHdr->Streams);
		AVI_DEBUG(" SuggestedBufferSize = %lu\n", pAVIHdr->SuggestedBufferSize);
		AVI_DEBUG(" Width = %lu\n", pAVIHdr->Width);
		AVI_DEBUG(" Height = %lu\n", pAVIHdr->Height);
		AVI_DEBUG(" Scale = %lu\n", pAVIHdr->Scale);
		AVI_DEBUG(" Rate = %lu\n", pAVIHdr->Rate);
		AVI_DEBUG(" Start = %lu\n", pAVIHdr->Start);
		AVI_DEBUG(" Length = %lu\n", pAVIHdr->Length);
		break;
	case AVI_DEMUX_MSG_STREAMHDR:
		pStreamHdr = (AVI_STREAM_HEADER *)info;
		if (pStreamHdr->fccType == AVI_FOURCC ('v','i','d','s'))
		{
			AVI_DEBUG("AVI_STREAM_HEADER (video %d):\n", video_stream_count);
			// take only the first video stream
			if (video_stream_count == 0)
			{
				MPEGVIDEO_PARAMETERS videoparams;
				ASSERT (pMpegDecoder);
				if (pStreamHdr->Scale < 1000)
				{
					AVI_DEBUG("pStreamHdr->Scale < 1000: adjusting scale/rate\n");
					AVI_DEBUG("Scale: %d, Rate: %d\n", pStreamHdr->Scale, pStreamHdr->Rate);
					RMuint32 scale = 1000 / pStreamHdr->Scale;
					pStreamHdr->Scale = pStreamHdr->Scale * scale;
					pStreamHdr->Rate = pStreamHdr->Rate * scale;
					AVI_DEBUG("ADJUSTED: Scale: %d, Rate: %d\n", 
					            pStreamHdr->Scale, pStreamHdr->Rate);
				}
				if (pStreamHdr->Rate > 64000)
				{
					AVI_DEBUG("pStreamHdr->Scale > 64000: adjusting scale/rate\n");
					AVI_DEBUG("Scale: %d, Rate: %d\n", 
					            AVI_PACKED_STRUCTURE_ACCESS (*pStreamHdr, Scale), 
					            AVI_PACKED_STRUCTURE_ACCESS (*pStreamHdr, Rate));
					pStreamHdr->Rate = (RMuint32) (((RMuint64)pStreamHdr->Rate) * (RMuint64)1000
									/ (RMuint64)pStreamHdr->Scale);
					pStreamHdr->Scale = 1000;
					AVI_DEBUG("ADJUSTED: Scale: %d, Rate: %d\n", 
					            pStreamHdr->Scale, pStreamHdr->Rate);
				}
				// we assume if the avi file says 23.975 or 23.976 frames/second,
				// then the _real_ value is 24000/1001
				if ((pStreamHdr->Scale == 1000) && (pStreamHdr->Rate == 23975))
				{
					DEBUGMAINMSG (1, ("optimizing rate of 23975\n"));
					pStreamHdr->Rate = 24000;
					pStreamHdr->Scale = 1001;
				}
				else if ((pStreamHdr->Scale == 1000) && (pStreamHdr->Rate == 23976))
				{
					DEBUGMAINMSG (1, ("optimizing rate of 23976\n"));
					pStreamHdr->Rate = 24000;
					pStreamHdr->Scale = 1001;
				}
				// try to intelligently fix some bad avi headers
				// normally the frame rate should be 29.97 (30000/1001)
				else if ((pStreamHdr->Scale == 1000) && 
					((pStreamHdr->Rate >= 29970) && (pStreamHdr->Rate <= 30007)) &&
					(pStreamHdr->Rate != 30000))
				{
					AVI_DEBUG("assuming 29.97 (%d,%d)\n", pStreamHdr->Scale, pStreamHdr->Rate);
					pStreamHdr->Rate = 30000;
					pStreamHdr->Scale = 1001;
				}
				else if ((pStreamHdr->Scale == 1000) && (pStreamHdr->Rate >= 28986) && (pStreamHdr->Rate <= 29006))
				{
					pStreamHdr->Rate = 29000;
					pStreamHdr->Scale = 1001;
					AVI_DEBUG("assuming 29.00 (%d,%d)\n", pStreamHdr->Scale, pStreamHdr->Rate);					
				}
				else if ((pStreamHdr->Scale == 1000) && (pStreamHdr->Rate == 15000))
				{
					pStreamHdr->Rate = 15000;				
					pStreamHdr->Scale = 1000;
					AVI_DEBUG("assuming 10.00 (%d,%d)\n", pStreamHdr->Scale, pStreamHdr->Rate);					
				}
				else if ((pStreamHdr->Scale == 1000) && (pStreamHdr->Rate == 23970))
				{
					pStreamHdr->Rate = 24000;
					pStreamHdr->Scale = 1001;
					AVI_DEBUG("assuming 23.970 (%d,%d)\n", pStreamHdr->Scale, pStreamHdr->Rate);					
				}
				videoparams.isMPEG4 = 1;

				if( pStreamHdr->Rate > 60000 ) {
					AVI_DEBUG("((Original))\n" );
					AVI_DEBUG("pStreamHdr->Rate : %lu\n", pStreamHdr->Rate );
					AVI_DEBUG("pStreamHdr->Scale : %lu\n", pStreamHdr->Scale );
					AVI_DEBUG("pAVIHdr->Rate : %lu\n", pAVIHdr->Rate );
					AVI_DEBUG("pAVIHdr->Scale : %lu\n", pAVIHdr->Scale );
				}
				

				videoparams.vopTimeIncrRes = pStreamHdr->Rate;
				videoparams.videoTimeScale = pStreamHdr->Rate;
				videoparams.audioTimeScale = pStreamHdr->Rate;
				videoparams.fixedVOPRate = 1;
				videoparams.vopTimeIncr = pStreamHdr->Scale;				
				aviTimeScale = videoparams.videoTimeScale;
				aviTicksPerFrame = videoparams.vopTimeIncr;
				aviTime0 = 1;
				aviStartupDelay = AVI_DELAY_MS * aviTimeScale / 1000;
				audioTime = aviTime0 + aviStartupDelay;
				videoTime = aviTime0 + aviStartupDelay;
				aviFrameCounter = 0;
				pMpegDecoder->SetMpegVideoParameters (&videoparams);
				currentRate = pStreamHdr->Rate;
				currentScale = pStreamHdr->Scale;
				_avividFramesPerSec = pStreamHdr->Rate/pStreamHdr->Scale;
			}
			video_stream_count++;
		}
		else if (pStreamHdr->fccType == AVI_FOURCC ('a','u','d','s')) {
			AVI_DEBUG("AVI_STREAM_HEADER (audio %d):\n", audio_stream_count);
			_aviauBlockPerSec += pStreamHdr->Rate/pStreamHdr->Scale;
			_aviauLength += pStreamHdr->Length;
			audio_stream_count++;
		}
		else {
			AVI_DEBUG("AVI_STREAM_HEADER:\n");
		}
		avi_stream_count++;
		AVI_DEBUG(" fccType = %c%c%c%c\n",
			(RMint8)(pStreamHdr->fccType >>  0),
			(RMint8)(pStreamHdr->fccType >>  8),
			(RMint8)(pStreamHdr->fccType >> 16),
			(RMint8)(pStreamHdr->fccType >> 24));
		AVI_DEBUG(" fccHandler = %c%c%c%c\n", 
			(RMint8)(pStreamHdr->fccHandler >>  0),
			(RMint8)(pStreamHdr->fccHandler >>  8),
			(RMint8)(pStreamHdr->fccHandler >> 16),
			(RMint8)(pStreamHdr->fccHandler >> 24));
		AVI_DEBUG(" Flags = %lu\n", pStreamHdr->Flags);
		AVI_DEBUG(" InitialFrames = %lu\n", pStreamHdr->InitialFrames);
		AVI_DEBUG(" Scale = %lu\n", pStreamHdr->Scale);
		AVI_DEBUG(" Rate = %lu\n", pStreamHdr->Rate);
		AVI_DEBUG(" Start = %lu\n", pStreamHdr->Start);
		AVI_DEBUG(" Length = %lu\n", pStreamHdr->Length);
		AVI_DEBUG(" SuggestedBufferSize = %lu\n", pStreamHdr->SuggestedBufferSize);
		AVI_DEBUG(" Quality = %lu\n", pStreamHdr->Quality);
		AVI_DEBUG(" SampleSize = %lu\n", pStreamHdr->SampleSize);	
		break;
	case AVI_DEMUX_MSG_BITMAPINFO:
		pBitmapInfo = (AVI_BITMAPINFO *)info;
		pMpegDecoder->GetScreenDimensions (&w, &h, &n, &d);
		if (video_stream_count == 1)
		{
			ASSERT (pMpegDecoder);
			RMint32 x, y;
//			pMpegDecoder->ClearScreen ();

			if ((pBitmapInfo->Compression == div3_FOURCC) ||
				(pBitmapInfo->Compression == DIV3_FOURCC) ||
				(pBitmapInfo->Compression == mp43_FOURCC) ||
				(pBitmapInfo->Compression == MP43_FOURCC))
			{		
				MPEGVIDEO_PARAMETERS videoparams;
				videoparams.isMPEG4 = 2;
				videoparams.vopTimeIncrRes = aviTimeScale;
				videoparams.videoTimeScale = aviTimeScale;
				videoparams.audioTimeScale = aviTimeScale;
				videoparams.fixedVOPRate = 1;
				videoparams.vopTimeIncr = aviTicksPerFrame;
				pMpegDecoder->SetMpegVideoParameters (&videoparams);

				ASSERT (((RMuint32)mp4_VOLHdr & 0x3) == 0);
				mp4_VOLHdrLength = MP43_DSI_Generator (mp4_VOLHdr, currentScale, currentRate, pBitmapInfo->Width, pBitmapInfo->Height);
				DEBUGMAINMSG (1, ("Created VOlHdr with length: %d bytes\n", (RMint32)mp4_VOLHdrLength));
				DEBUGMAINMSG (1, ("scale: %d, rate: %d, width: %d, height: %d\n",
					(RMint32)currentScale, (RMint32)currentRate, 
					(RMint32)pBitmapInfo->Width, (RMint32)pBitmapInfo->Height));				
				RMuint8 *p = (RMuint8 *)mp4_VOLHdr;
				DEBUGMAINMSG (1, ("%02x %02x %02x %02x %02x %02x %02x %02x\n",
					*p++, *p++, *p++, *p++, *p++, *p++, *p++, *p++));
				DEBUGMAINMSG (1, ("%02x %02x %02x %02x %02x %02x %02x %02x\n",
					*p++, *p++, *p++, *p++, *p++, *p++, *p++, *p++));
				DEBUGMAINMSG (1, ("%02x %02x %02x %02x %02x %02x %02x %02x\n",
					*p++, *p++, *p++, *p++, *p++, *p++, *p++, *p++));
				DEBUGMAINMSG (1, ("%02x %02x %02x %02x %02x %02x %02x %02x\n",
					*p++, *p++, *p++, *p++, *p++, *p++, *p++, *p++));
			}


			memcpy( &AviBmpInfo, pBitmapInfo, sizeof(AVI_BITMAPINFO) );

		}
		DEBUGMAINMSG (1, ("AVI_BITMAPINFO:\n"));
		DEBUGMAINMSG (1, (" Size = %lu\n", pBitmapInfo->Size));
		DEBUGMAINMSG (1, (" Width = %lu\n", pBitmapInfo->Width));
		DEBUGMAINMSG (1, (" Height = %lu\n", pBitmapInfo->Height));
		DEBUGMAINMSG (1, (" Planes = %lu\n", pBitmapInfo->Planes));
		DEBUGMAINMSG (1, (" BitCount = %lu\n", pBitmapInfo->BitCount));
		DEBUGMAINMSG (1, (" Compression = %c%c%c%c\n", 
			(RMint8)(pBitmapInfo->Compression >>  0),
			(RMint8)(pBitmapInfo->Compression >>  8),
			(RMint8)(pBitmapInfo->Compression >> 16),
			(RMint8)(pBitmapInfo->Compression >> 24)));			
		DEBUGMAINMSG (1, (" SizeImage = %lu\n", pBitmapInfo->SizeImage));
		DEBUGMAINMSG (1, (" XPelsPerMeter = %lu\n", pBitmapInfo->XPelsPerMeter));
		DEBUGMAINMSG (1, (" YPelsPerMeter = %lu\n", pBitmapInfo->YPelsPerMeter));
		DEBUGMAINMSG (1, (" ClrUsed = %lu\n", pBitmapInfo->ClrUsed));
		DEBUGMAINMSG (1, (" ClrImportant = %lu\n", pBitmapInfo->ClrImportant));
		currentFOURCC = pBitmapInfo->Compression;
		currentWidth = pBitmapInfo->Width;
		currentHeight = pBitmapInfo->Height;
		break;
	case AVI_DEMUX_MSG_WAVEFORMATEX:
		pWaveFormatEx = (AVI_WAVEFORMATEX *)info;
		// select the first mp3 audio track
		if ((pWaveFormatEx->wFormatTag == 0x55) && 
			(selected_audio_stream[0] == 0xff) &&
			(selected_audio_stream[1] == 0xff))
		{
			// mp3
			ASSERT (avi_stream_count > 0);
			sprintf (selected_audio_stream, "%02d", avi_stream_count - 1);
			ASSERT (pMP3BufferDecoder == 0);
			pMP3BufferDecoder = (MP3Decoder *) new MP3Decoder;
			ASSERT (pMP3BufferDecoder);
			mp3chunkQ.Reset ();

			selected_audio_stream_format = 0x55;

			MP3_CALLBACK_TABLE callback_table;
			callback_table.context = &BPmp3;		// saved context information for application
			callback_table.fopen = 0;				// fopen for file decoding
			callback_table.fread = 0;				// fread for file decoding
			callback_table.fseek = 0;				// fseek for file decoding
			callback_table.fclose = 0;				// fclose for file decoding
			callback_table.ftell = 0;				// ftell for file decoding
			callback_table.addref = my_addref;		// addref a buffer
			callback_table.release = my_release;	// release a buffer
			callback_table.info = 0;				// inform app of some mp3 info
			callback_table.putPCM = mp3wma_putPCM;	// output PCM bytes
			callback_table.getPCM = mp3wma_getPCM;	// get a buffer for PCM data
			callback_table.getMP3 = 0;				// get a buffer for reading MP3 data
			callback_table.reachEof = 0;

			SamplesPerSecond = 0;
			init_buffer_pool (&BPpcm, pcm_buffer, PCM_BLOCK_SIZE, PCM_NBLOCKS);
			ASSERT (PCM_DATA_BUFFER_SIZE >= (PCM_BLOCK_SIZE*PCM_NBLOCKS));
			ASSERT (pMP3BufferDecoder);
			pMP3BufferDecoder->Init ();
			ASSERT (pMP3BufferDecoder);
			pMP3BufferDecoder->InitCallbackTable (&callback_table);
			pMP3BufferDecoder->DecodeBufferStart ();

			PCM_PARAMETERS pcmparams;
			pcmparams.NumberOfBitsPerSample = 16;
			pcmparams.NumberOfChannels = pWaveFormatEx->nChannels;
			pcmparams.SamplesPerSecond = pWaveFormatEx->nSamplesPerSec;
			AEffectSetContentsProp( (int)pcmparams.SamplesPerSecond, 2);	// emx 3d			
			pMpegDecoder->SetPCMParameters (&pcmparams);
			NumberOfBitsPerSample = pcmparams.NumberOfBitsPerSample;
			NumberOfChannels = pcmparams.NumberOfChannels;
			SamplesPerSecond = pcmparams.SamplesPerSecond;
			BytesPerSecond = (NumberOfBitsPerSample / 8) * NumberOfChannels * SamplesPerSecond;
			CompressedBytesPerSecond = pWaveFormatEx->nAvgBytesPerSec;
		}
		else if ((pWaveFormatEx->wFormatTag == 0x2000) && 
			(selected_audio_stream[0] == 0xff) &&
			(selected_audio_stream[1] == 0xff))
		{
			// ac3
			ASSERT (avi_stream_count > 0);
			sprintf (selected_audio_stream, "%02d", avi_stream_count - 1);
			selected_audio_stream_format = 0x2000;
			AC3_PARAMETERS ac3params;
			ac3params.rawOutput = 0;	// decode ac3
			ac3params.SamplesPerSecond = pWaveFormatEx->nSamplesPerSec;
			NumberOfBitsPerSample = 16;
			SamplesPerSecond = pWaveFormatEx->nSamplesPerSec;
			BytesPerSecond = pWaveFormatEx->nAvgBytesPerSec;
			CompressedBytesPerSecond = pWaveFormatEx->nAvgBytesPerSec;
			ASSERT (pMpegDecoder);
			pMpegDecoder->SetAC3Parameters (&ac3params);
			tbytes = 0;
			ac3FrameSize = 0;
		}
		else if ((pWaveFormatEx->wFormatTag == 0x1) && 
			(pWaveFormatEx->wBitsPerSample == 16) &&
			(selected_audio_stream[0] == 0xff) &&
			(selected_audio_stream[1] == 0xff))
		{
			// 16 bit pcm
			ASSERT (avi_stream_count > 0);
			sprintf (selected_audio_stream, "%02d", avi_stream_count - 1);
			selected_audio_stream_format = 0x1;

			PCM_PARAMETERS pcmparams;
			pcmparams.NumberOfBitsPerSample = 16;
			pcmparams.NumberOfChannels = pWaveFormatEx->nChannels;
			pcmparams.SamplesPerSecond = pWaveFormatEx->nSamplesPerSec;
			pMpegDecoder->SetPCMParameters (&pcmparams);
			NumberOfBitsPerSample = pcmparams.NumberOfBitsPerSample;
			NumberOfChannels = pcmparams.NumberOfChannels;
			SamplesPerSecond = pcmparams.SamplesPerSecond;
			BytesPerSecond = (NumberOfBitsPerSample / 8) * NumberOfChannels * SamplesPerSecond;
			CompressedBytesPerSecond = pWaveFormatEx->nAvgBytesPerSec;
		}
		_aviauAvgBytesPerSec += pWaveFormatEx->nAvgBytesPerSec;
		_aviWaveFmt = *pWaveFormatEx;
		AVI_DEBUG("AVI_WAVEFORMATEX:\n");
		AVI_DEBUG(" wFormatTag = %lu\n", pWaveFormatEx->wFormatTag);
		AVI_DEBUG(" nChannels = %lu\n", pWaveFormatEx->nChannels);
		AVI_DEBUG(" nSamplesPerSec = %lu\n", pWaveFormatEx->nSamplesPerSec);
		AVI_DEBUG(" nAvgBytesPerSec = %lu\n", pWaveFormatEx->nAvgBytesPerSec);
		AVI_DEBUG(" nBlockAlign = %lu\n", pWaveFormatEx->nBlockAlign);
		AVI_DEBUG(" wBitsPerSample = %lu\n", pWaveFormatEx->wBitsPerSample);
		break;
	default:
		ASSERT (0);
		printf("Unkown Message : 0x%x\n", msg );
		return 10;
		break;
	}

	return 0;
}


// avi demux callback function - the avi demux object is requesting
// a buffer to read avi data into
static RMuint32 avi_getAVI (RMuint8 **pBuffer, RMuint32 *plength, void *context)
{
	// low water mark
	if (prebuffering == 0) {
		if (avi_rebuffer == 0) {
			int i, x;
			static int prevx = -1;
			x = 0;
			for (i=0; i<BPavi.nb_buffers; i++)
			{
				if (BPavi.B[i])
					x++;
			}
			prevx = x;
			if (x >= AVI_LOW_THRESHOLD)
			{
				return 1;
			}
			fs_powerup ();
			DEBUGMAINMSG (1, ("(rebuffer)"));			
			avi_rebuffer = 1;
		}
	}

	int idx = find_free_buffer (&BPavi, 0);
	if (idx < 0)
	{
		if (prebuffering)
		{
			fs_powerdown ();
			DEBUGMAINMSG (1, ("start decode due to avi buffer full\n"));
			prebuffering = 0;
			pMpegDecoder->Play ();
		}
		DEBUGMAINMSG (1, ("(stop rebuffer)"));
		fs_powerdown ();
		avi_rebuffer = 0;
		sw_decode_mp3 ();
		return 1;
	}

	*plength = (RMuint32)BPavi.buffers_size;
	*pBuffer = (RMuint8 *)get_buffer_address (&BPavi, idx);
	return 0;
}

static RMuint8 pic_startcode[4];
// avi callback function - the avi demux object has demuxed some
// data, and is requesting the application to render this data

static RMuint32 avi_putChunk_V19 (RMuint8 *chunkid, RMuint8 *buffer, RMuint32 buflen, RMuint32 flags, void *context)
{
	MPEG_DECODER_ERROR mpegerr;
	AVI_DEBUG("avi_putChunk (%c%c%c%c, 0x%08lx, %lu, 0x%08lx)\n", 
		    chunkid[0], chunkid[1], chunkid[2], chunkid[3],
		    buffer, buflen, flags);

	if ((buffer == 0) || (buflen == 0))
	{
		if ((chunkid[2] == 'd') && ((chunkid[3] == 'c') || (chunkid[3] == 'b')))
		{
			if (flags & AVI_FLAG_CHUNK_START)
			{
				aviFrameCounter++;
				DEBUGMAINMSG (0, ("(1) aviFrameCounter: %lu\n", aviFrameCounter));
			}
		}
		return 0;
	}

	if ((chunkid[2] == 'd') && ((chunkid[3] == 'c') || (chunkid[3] == 'b')))
	{
		// video chunk
		if (transcodeVideo)
		{	
			// send 0x000001b6 at the start of every picture
			// for ms-mpeg4v3
			if (flags & AVI_FLAG_CHUNK_START)
			{
				pic_startcode[0] = 0x00;
				pic_startcode[1] = 0x00;
				pic_startcode[2] = 0x01;
				pic_startcode[3] = 0xb6;
				videoTime = aviStartupDelay + ((RMint64)aviFrameCounter * (RMint64)aviTicksPerFrame);
				//AVI_DEBUG("WriteMpegVideo (aviTime0 = %lld, videoTime = %lld, framecounter: %lld)\n", 
				//            aviTime0, videoTime, aviFrameCounter );
				mpegerr = pMpegDecoder->WriteMpegVideo (pic_startcode, 4, 0, CTS_AVAILABLE_FLAG, aviTime0, videoTime);
				ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);
				aviFrameCounter++;				
			}
			flags = 0;
		}
		ASSERT (pMpegDecoder);
		BufferPool *BP;
		ASSERT (((int)buffer >= (int)avi_buffer) && ((int)buffer < ((int)avi_buffer + (MAIN_DATA_BUFFER_SIZE))));
		BP = &BPavi;
		int idx = ((int)buffer - (int)BP->start_address) / BP->buffers_size;
		ASSERT (idx >= 0);
		// break this chunk into < 64k blocks
		RMuint8 *p = buffer;
		RMuint32 tl = buflen;
		RMuint32 flag = 0;

		if (flags & AVI_FLAG_CHUNK_START)
		{			
			if (buflen > 4)
			{
				// if I or P frame
				if (((buffer[4] & 0xc0) == 0x40) || 
					((buffer[4] & 0xc0) == 0x00))
				{
					flag = CTS_AVAILABLE_FLAG;
					videoTime = aviStartupDelay + ((RMint64)aviFrameCounter * (RMint64)aviTicksPerFrame);
				}
			}
			aviFrameCounter++;
		}

        int rcount = 0;
		while (tl) {
			addref_buffer (&(BP->B[idx]));
			buflen = MY_MIN (0xffff, tl);
			DEBUGMAINMSG (0, ("WriteMpegVideo (%d, %d, %d)\n", (RMint32)videoTime, (RMint32)buflen, aviFrameCounter));
			DEBUGMAINMSG (0, ("%02x %02x %02x %02x\n", p[0], p[1], p[2], p[3]));
			ASSERT (buflen);

			if (KeyFramesOnly) flag = 0;
		
			//AVI_DEBUG("WriteMpegVideo (KeyFramesOnly = %d, flag = %d, aviTime0 = %lld, videoTime = %lld, COUNTER: %lld)\n", 
			//	        KeyFramesOnly, flag, aviTime0, videoTime, aviFrameCounter);

			if (flag) mpegerr = pMpegDecoder->WriteMpegVideo (p, buflen, &(BP->B[idx]), flag, aviTime0, videoTime);
			else	    mpegerr = pMpegDecoder->WriteMpegVideo (p, buflen, &(BP->B[idx]), 0, 0, 0);

			ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);
			if (mpegerr == MPEG_DECODER_ERROR_NO_ROOM)
			{
				release_buffer (&(BP->B[idx]));
				rcount++;
			}
			tl -= buflen;
			p += buflen;
			flag = 0;
		}
	}
	else if ((chunkid[2] == 'w') && (chunkid[3] == 'b'))
	{
		// audio chunk
		if ((chunkid[0] == selected_audio_stream[0]) &&
			(chunkid[1] == selected_audio_stream[1]))
		{
			DEBUGMAINMSG (0, ("audio chunk (%c%c%c%c, 0x%08lx, %lu, 0x%08lx)\n", 
				chunkid[0], chunkid[1], chunkid[2], chunkid[3],
				buffer, buflen, flags));
				
			if (selected_audio_stream_format == 0x55)
			{
				// mp3
				if (pMP3BufferDecoder)
				{			
					putChunk *chunk;
					RMuint32 queued = 0;

					// queue this putChunk
					if (mp3chunkQ.CanQueue ())
					{
						if (mp3chunkQ.Queue (buffer, buflen) == 0)
						{
							my_addref (buffer, context);
							queued = 1;
						}
						else
						{
							ASSERT (0);
						}
					}					
					if (queued)
					{
						return 0;
					}
					// decode no matter what
					chunk = mp3chunkQ.DeQueue ();
					ASSERT (chunk);
					if (chunk)
					{
						my_release (chunk->buffer, context);
						if (pMP3BufferDecoder->DecodeBuffer (chunk->buffer, chunk->length) != MP3_DECODER_ERROR_NO_ERROR)
						{
							// there was a decoding error
							// should we adjust the time stamp to compensate?
////							tbytes += (1152 * 2 * NumberOfChannels);	//--
						}
					}
					if (mp3chunkQ.Queue (buffer, buflen) == 0)
					{
						my_addref (buffer, context);
					}
					else
					{
						ASSERT (0);
					}
				}			
			}
			else if (selected_audio_stream_format == 0x2000)
			{	
				// ac3
				BufferPool *BP;
				BP = &BPavi;
				int idx = ((int)buffer - (int)BP->start_address) / BP->buffers_size;
				ASSERT (idx >= 0);
				// break this chunk into < 64k blocks
				RMuint8 *p = buffer;
				RMuint32 tl = buflen;
				RMuint32 flag = 0;

				avi_initial_start = 0;

				// find the ac3FrameSize
				if ((buflen > 4) && (ac3FrameSize == 0))
				{
					RMint32 ac3l = buflen;
					RMuint8 *pac3 = buffer;
					while (ac3l > 4)
					{
						if ((pac3[0] == 0x0b) && (pac3[1] == 0x77))
						{
							int fscod = (pac3[4] >> 6) & 0x3;
							int frmsizecod = pac3[4] & 0x3f;
							if ((fscod < 3) && (frmsizecod <= 0x25))
							{
								ac3FrameSize = frmsizecod_table[fscod][frmsizecod] * 2;
								DEBUGMAINMSG (1, ("ac3FrameSize: %d\n", ac3FrameSize));
								break;
							}
						}
						pac3++;
						ac3l--;
					}
				}
				if (ac3FrameSize)
				{
					RMint64 nac3frames = (RMint64)tbytes / ac3FrameSize;
					if (tbytes % ac3FrameSize)
						nac3frames++;
					flag = CTS_AVAILABLE_FLAG;
					audioTime = aviStartupDelay + 
						((RMint64)nac3frames * 256 * 6 * (RMint64)aviTimeScale / (RMint64)SamplesPerSecond);
				}
				// SDK1.2 - S
				// fill out the firstaccessunitpointer field
				// to make ac3 sync better
				RMuint32 firstaccessunitpointer = 0;
				if (flag && ac3FrameSize)
					firstaccessunitpointer = ac3FrameSize - (tbytes % ac3FrameSize);
				// SDK1.2 - E
				tbytes += buflen;
				while (tl)
				{
					addref_buffer (&(BP->B[idx]));
					buflen = MY_MIN (0x2000, tl);
					ASSERT (pMpegDecoder);
					DEBUGMAINMSG (0, ("WriteAC3 (%d)\n", (RMint32)audioTime));
					mpegerr = pMpegDecoder->WriteAC3 (p, buflen, &(BP->B[idx]), flag, aviTime0, audioTime, firstaccessunitpointer);
					ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);
					tl -= buflen;
					p += buflen;
					flag = 0;
				}				
			} 
			else if (selected_audio_stream_format == 0x2001)
			{	
				// dts
				BufferPool *BP;
				ASSERT (((int)buffer >= (int)avi_buffer) && ((int)buffer < ((int)avi_buffer + (AVI_BLOCK_SIZE * AVI_NBLOCKS))));
				BP = &BPavi;
				int idx = ((int)buffer - (int)BP->start_address) / BP->buffers_size;
				ASSERT (idx >= 0);
				// break this chunk into < 4k blocks
				RMuint8 *p = buffer;
				RMuint32 tl = buflen;
				RMuint32 flag = 0;

				// find the dts frame size
				if ((buflen > 4) && ((dtsFrameSize == 0) || (avi_initial_start)))
				{
					RMint32 dtsl = buflen;
					RMuint8 *pdts = buffer;
					while (dtsl > 4)
					{
						if ((pdts[0] == 0x7f) && 
							(pdts[1] == 0xfe) &&
							(pdts[2] == 0x80) &&
							(pdts[3] == 0x01))
						{
							dtsFrameSize = (
								((RMuint32)(pdts[5] & 0x03) << 12) | 
								((RMuint32)(pdts[6]       ) <<  4) |
								((RMuint32)(pdts[7] & 0xf0) >>  4));
							dtsFrameSize++;							
							DEBUGMAINMSG (1, ("dtsFrameSize = %d\n", (RMint32)dtsFrameSize));
							p = pdts;
							buflen = dtsl;
							tl = buflen;
							break;
						}
						pdts++;
						dtsl--;
					}
				}
				if (dtsFrameSize == 0)
					return 0;
								
				if ((tbytes == 0) || (avi_initial_start))
				{
					avi_initial_start = 0;
					RMint64 ndtsframes = (RMint64)tbytes / dtsFrameSize;
					if (tbytes && (tbytes % dtsFrameSize))
						ndtsframes++;
					flag = CTS_AVAILABLE_FLAG;
					audioTime = aviStartupDelay + 
						((RMint64)ndtsframes * 512 * (RMint64)aviTimeScale / (RMint64)SamplesPerSecond);
				}				

				tbytes += buflen;

				while (tl)
				{
					addref_buffer (&(BP->B[idx]));
					buflen = MY_MIN (0x1000, tl);
					ASSERT (pMpegDecoder);
					DEBUGMAINMSG (0, ("WriteDTS (%d, %d)\n", (RMint32)buflen, (RMint32)audioTime));
					DEBUGMAINMSG (flag, ("WriteDTS (%02x %02x %02x %02x, %d)\n", p[0], p[1], p[2], p[3], (RMint32)audioTime));
					mpegerr = pMpegDecoder->WriteDTS (p, buflen, &(BP->B[idx]), flag, aviTime0, audioTime, buflen/dtsFrameSize, 0);
					ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);
					tl -= buflen;
					p += buflen;
					flag = 0;
				}				
			}
			else if ( (selected_audio_stream_format == 0x1) || (selected_audio_stream_format == 0x2) )
			{	
				// pcm
				BufferPool *BP;
				ASSERT (((int)buffer >= (int)avi_buffer) && ((int)buffer < ((int)avi_buffer + (AVI_BLOCK_SIZE * AVI_NBLOCKS))));
				BP = &BPavi;
				int idx = ((int)buffer - (int)BP->start_address) / BP->buffers_size;
				ASSERT (idx >= 0);
				// swap bytes
				RMuint8 *p = buffer;
				RMuint32 tl = buflen;
				RMuint32 flag = CTS_AVAILABLE_FLAG;
				audioTime = aviStartupDelay + ((RMint64)tbytes * (RMint64)aviTimeScale / (RMint64)BytesPerSecond);
				RMuint8 tmp;
				for (RMuint32 i=0; i<buflen; i+=2)
				{
					tmp = p[0];
					p[0] = p[1];
					p[1] = tmp;
					p += 2;
				}
				p = buffer;
				tbytes += buflen;
				// break this chunk into < 8k blocks
				while (tl)
				{
					addref_buffer (&(BP->B[idx]));
					buflen = MY_MIN (0x2000, tl);
					ASSERT (pMpegDecoder);
					DEBUGMAINMSG (0, ("WritePCM (%d)\n", (RMint32)audioTime));
					
					if(selected_audio_stream_format == 0x1)
						mpegerr = pMpegDecoder->WritePCM (p, buflen, &(BP->B[idx]), flag, aviTime0, audioTime);
					else // 8bit pcm
						mpegerr = pMpegDecoder->WritePCM (p, buflen*2, &(BP->B[idx]), flag, aviTime0, audioTime);
						
					ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);
					if (mpegerr == MPEG_DECODER_ERROR_NO_ROOM)
					{
						release_buffer (&(BP->B[idx]));
					}
					tl -= buflen;
					p += buflen;
					flag = 0;
				}				
			}
		}
		else
		{
			// ok - multiple audio streams
		}
	}
	else if ((chunkid[2] == 'd') && (chunkid[3] == 'd'))
	{
		// divx drm chunk
//		ASSERT (0);
		printf("divx drm chunk \n");
	}
	else
	{
		// unknown chunk
		printf("unknown chunk \n");		
	}

	if( KeyFramesOnly )
		pMpegDecoder->SetSTC(videoTime, 1000);
	return 0;
}

// jpeg callback - this callback is called when a jpeg is decoded.
// it allows the application to interleave process handling, if
// the jpeg decoding takes a long time.
// it also allows the application to display a percentage if
// required
static void jpeg_decoding_progress (int percent_done, void *context)
{
}

// mp4 callback functions
static RMuint32 mp4_info (RMint32 msg, void *info, void *context)
{
	if (msg == MP4_MSG_TRACK_TIMESCALE)
	{
		TRACK_TIMESCALE *ts;
		ts = (TRACK_TIMESCALE *)info;
		videoTimeScale = ts->videoTimeScale;
		audioTimeScale = ts->audioTimeScale[selected_audiostream];
		DEBUGMAINMSG (1, ("videoTimeScale = %d\n", (RMint32)videoTimeScale));
		DEBUGMAINMSG (1, ("audioTimeScale = %d\n", (RMint32)audioTimeScale));
	}
	else if (msg == MP4_MSG_VIDEO_DIMENSIONS)
	{
		RMint32 w, h, n, d, x, y;
		VIDEO_DIMENSIONS *vd;
		vd = (VIDEO_DIMENSIONS *)info;

		ASSERT (pMpegDecoder);
		pMpegDecoder->GetScreenDimensions (&w, &h, &n, &d);

		DEBUGMAINMSG (1, ("%d x %d\n", vd->visualWidth, vd->visualHeight)); 

		if ((vd->visualWidth*1000*d/(w*n)) > vd->visualHeight*1000/h)
		{
			ASSERT (d);
			ASSERT (vd->visualWidth);
			y = (vd->visualHeight * w / vd->visualWidth) * n / d;
			ASSERT (y <= h);
			x = (h - y) / 2;
			ASSERT (x >= 0);
			ASSERT (w > 0);
			ASSERT (y > 0);
			DEBUGMAINMSG (!(x >= 0) || !(w > 0) || !(y > 0), 
				("x = %d, y = %d, w = %d, h = %d\n", x, y, w, h));
			DEBUGMAINMSG (1, ("(%d, %d, %d, %d)\n", 0, x, w, y)); 
			pMpegDecoder->SetVideoDestination (0, x, w, y);
		}
		else
		{
			ASSERT (d);
			ASSERT (vd->visualHeight);
			y = (vd->visualWidth * h / vd->visualHeight) * d / n;
			ASSERT (y <= w);
			x = (w - y) / 2;
			ASSERT (x >= 0);
			DEBUGMAINMSG (!(x >= 0) || !(y > 0) || !(h > 0), 
				("x = %d, y = %d, w = %d, h = %d\n", x, y, w, h));
			ASSERT (y > 0);
			ASSERT (h > 0);
			DEBUGMAINMSG (1, ("(%d, %d, %d, %d)\n", x, 0, y, h));
			pMpegDecoder->SetVideoDestination (x, 0, y, h);
		}
	}
	else
		ASSERT (0);
	return 0;
}

static RMuint32 mp4_getData (RMuint8 **pBuffer, RMuint32 *length, void *context)
{
	int idx = find_free_buffer (&BPmp4, 0);
	if (idx < 0)
	{
		DEBUGMAINMSG (0, ("mp4_getData (%d)\n", idx));
		if (prebuffering)
		{
			pMpegDecoder->Play ();
			prebuffering = 0;
			DEBUGMAINMSG (1, ("play due to mp4 buffer full\n"));
		}
		return 1;
	}
	*length = (RMuint32)BPmp4.buffers_size;
	*pBuffer = (RMuint8 *)get_buffer_address (&BPmp4, idx);	
	DEBUGMAINMSG (0, ("mp4_getData (%d)\n", idx));
	return 0;
}

static RMint32 ParseVOLHdr (RMuint8 *pDSI, RMuint32 length)
{
	int Error = 0;
	BitStream DSIInformation (pDSI, length, &Error);
	RMuint32 vop_time_increment_resolution;
	RMuint32 temp;

	vopTimeIncrRes = 30000;

	while (1)
	{
		temp = DSIInformation.showbits(32);
		if ((temp >= 0x00000100) && (temp <= 0x0000011F))
			break;

		temp = DSIInformation.getbits (8, &Error);	// USER_DATA_START_CODE, user data
		if (Error)
		{
			DEBUGMAINMSG (1, ("ParseDSIInformation: NO video object start code\n"));
			return -1;
		}
	}
	temp = DSIInformation.getbits (32,&Error);	// video_object_start_code
	temp = DSIInformation.showbits (32);		// video_object_layer_start_code
	if ((temp & 0x000001E0) == 0x00000120)
	{
		// short_video_header = 0
		temp = DSIInformation.getbits (32,&Error);	// video_object_layer_start_code
		temp = DSIInformation.getbits (1,&Error);	// random_accessible_vol
		temp = DSIInformation.getbits (8,&Error);	// video_object_type_indication
		temp = DSIInformation.getbits (1,&Error);	// is_object_layer_identifier
		if (temp == 1)
		{
			temp = DSIInformation.getbits (4, &Error);	// video_object_layer_verid should be 1
			temp = DSIInformation.getbits (3, &Error);	// video_object_layer_priority
		}
		temp = DSIInformation.getbits (4, &Error);	// aspect_ratio_info
		if (temp == 0xf) // extended par
		{
			RMuint32 par_width = DSIInformation.getbits (8, &Error);	// par_width
			RMuint32 par_height = DSIInformation.getbits (8, &Error);	// par_height
		}
		temp = DSIInformation.getbits (1, &Error);	// vol_control_parameters
		if (temp == 1)
		{
			temp = DSIInformation.getbits (2, &Error);	// chroma_format
			temp = DSIInformation.getbits (1, &Error);	// low_delay
			temp = DSIInformation.getbits (1, &Error);	// vbv_parameters
			if (temp == 1)
			{
				temp = DSIInformation.getbits (15, &Error);// first_half_bit_rate
				temp = DSIInformation.getbits (1, &Error);	// marker_bit
				temp = DSIInformation.getbits (15, &Error);// latter_half_bit_rate
				temp = DSIInformation.getbits (1, &Error);	// marker_bit
				temp = DSIInformation.getbits (15, &Error);// first_half_vbv_buffer_size
				temp = DSIInformation.getbits (1, &Error);	// marker_bit
				temp = DSIInformation.getbits (3, &Error);	// latter_half_vbv_buffer_size
				temp = DSIInformation.getbits (11, &Error);// first_half_vbv_occupancy
				temp = DSIInformation.getbits (1, &Error);	// marker_bit
				temp = DSIInformation.getbits (15, &Error);// latter_half_vbv_occupancy
				temp = DSIInformation.getbits (1, &Error);	// marker_bit
			}
		}
		RMuint8 video_object_layer_shape = DSIInformation.getbits (2, &Error);
		temp = DSIInformation.getbits (1, &Error);	// marker_bit
		vop_time_increment_resolution = DSIInformation.getbits (16, &Error);
		DEBUGMAINMSG (1, ("vop_time_increment_resolution = %d\n",
			vop_time_increment_resolution));
		temp = DSIInformation.getbits (1, &Error);	// marker_bit
		temp = DSIInformation.getbits (1, &Error);	// fixed_vop_rate
		if (temp)
		{
			DEBUGMAINMSG (1, ("fixed_vop_rate\n"));

			RMint32 nbits;
			for (nbits=16; nbits>0; nbits--)
			{
				if ((vop_time_increment_resolution >> nbits) == 1)
					break;
			}
			nbits++;
			RMint32 fixed_vop_time_increment = DSIInformation.getbits (nbits, &Error);
			DEBUGMAINMSG (1, ("fixed_vop_time_increment = %d\n", fixed_vop_time_increment));
		}
		vopTimeIncrRes = vop_time_increment_resolution;

		if (video_object_layer_shape == 0)	// Rectangular
		{
			temp = DSIInformation.getbits(1,&Error);	// marker_bit
			temp = DSIInformation.getbits(13,&Error);	// width
			temp = DSIInformation.getbits(1,&Error);	// marker_bit
			temp = DSIInformation.getbits(13,&Error);	// height
		}
		else
			return 0;

		return 0;
	}
	else
		return -1;
}

static RMuint32 mp4_putDSI (RMuint32 trackid, RMuint32 flags, RMuint8 *pBuffer, RMuint32 length, RMuint64 time, void *context)
{
	MPEG_DECODER_ERROR mpegerr;

	DEBUGMAINMSG (0, ("putDSI:\n"));
	DEBUGMAINMSG (0, ("  trackid %d: %lu @ 0x%08lx\n", (int)trackid, (RMuint32)length, (RMuint32)pBuffer));
	
	if (trackid == 0)
	{
		int idx = find_free_buffer (&BPmp4, 0);
		if (idx < 0)
		{
			return 1;
		}

		pMpegDecoder->Stop ();

		ParseVOLHdr (pBuffer, length);
		
		mp4_scr0 = time;
		mp4_scrDelay = vopTimeIncrRes / 10;

		MPEGVIDEO_PARAMETERS videoparams;
		videoparams.isMPEG4 = 1;
		videoparams.vopTimeIncrRes = vopTimeIncrRes;
		videoparams.videoTimeScale = vopTimeIncrRes;
		videoparams.audioTimeScale = vopTimeIncrRes;
		videoparams.fixedVOPRate = 0;
		videoparams.vopTimeIncr = 0;
		DEBUGMAINMSG (1, ("MP4:\n"));
		DEBUGMAINMSG (1, ("vopTimeIncrRes: %lu\n", videoparams.vopTimeIncrRes));
		DEBUGMAINMSG (1, ("videoTimeScale: %lu\n", videoparams.videoTimeScale));
		DEBUGMAINMSG (1, ("audioTimeScale: %lu\n", videoparams.audioTimeScale));
		DEBUGMAINMSG (1, ("fixedVOPRate: %lu\n", videoparams.fixedVOPRate));
		DEBUGMAINMSG (1, ("vopTimeIncr: %lu\n", videoparams.vopTimeIncr));
		pMpegDecoder->SetMpegVideoParameters (&videoparams);
		pMpegDecoder->Pause ();
		prebuffering = 1;

		RMuint32 l;
		RMuint8 *p;
		l = (RMuint32)BPmp4.buffers_size;
		p = (RMuint8 *)get_buffer_address (&BPmp4, idx);
		ASSERT (l > length);
		ASSERT (p);
		memcpy (p, pBuffer, length);		
		addref_buffer (&(BPmp4.B[idx]));
		mpegerr = pMpegDecoder->WriteMpegVideo (p, length, &(BPmp4.B[idx]), 0, 0, 0);
		ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);		
	}
	else if (trackid == 1)
	{
		if (videoTimeScale == 0)
		{
			DEBUGMAINMSG (1, ("videoTimeScale == 0\n"));
			vopTimeIncrRes = 90000;

			MPEGVIDEO_PARAMETERS videoparams;
			videoparams.isMPEG4 = 0;
			pMpegDecoder->SetMpegVideoParameters (&videoparams);
			
			mp4_scr0 = time;
			mp4_scrDelay = vopTimeIncrRes / 10;
			mp4_VOLHdrLength = MP43_DSI_Generator (mp4_VOLHdr, audioTimeScale, audioTimeScale/30, 320, 240);
			mpegerr = pMpegDecoder->WriteMpegVideo ((RMuint8 *)mp4_VOLHdr, mp4_VOLHdrLength, 0, CTS_AVAILABLE_FLAG, mp4_scr0, mp4_scr0 + mp4_scrDelay);
			ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);
			pMpegDecoder->Play ();
		}
		ASSERT (pAACDecoder);
		if (pAACDecoder->dsiInit (pBuffer, length) == AAC_SUCCESS)
		{
			// XXX should do error handing (wrong sampling rate, nchannels)
			PCM_PARAMETERS pcmparams;
			pcmparams.NumberOfBitsPerSample = 16;
			pcmparams.NumberOfChannels = pAACDecoder->m_channelCount;			
			pcmparams.SamplesPerSecond = pAACDecoder->m_sampleRate;
			pMpegDecoder->SetPCMParameters (&pcmparams);
			NumberOfBitsPerSample = pcmparams.NumberOfBitsPerSample;
			NumberOfChannels = pcmparams.NumberOfChannels;
			SamplesPerSecond = pcmparams.SamplesPerSecond;
			DEBUGMAINMSG (1, ("NumberOfChannels = %d\n", (int)pcmparams.NumberOfChannels));
			DEBUGMAINMSG (1, ("SamplesPerSecond = %d\n", (int)pcmparams.SamplesPerSecond));
		}
		else
		{
			// XXX should do error handing
			ASSERT (0);
		}
	}
	return 0;
}

static RMuint32 mp4_putChunk (RMuint32 trackid, RMuint32 flags, RMuint64 time, RMuint8 *pBuffer, RMuint32 length, void *context)
{
	MPEG_DECODER_ERROR mpegerr;

	DEBUGMAINMSG (0, ("putChunk:\n"));
	DEBUGMAINMSG (0, ("  trackid %d: %lu @ 0x%08lx\n", (int)trackid, (RMuint32)length, (RMuint32)pBuffer));

	if (length == 0)
		return 0;
			
	if (trackid == 0)
	{
		BufferPool *BP;
		BP = &BPmp4;
		int idx = ((int)pBuffer - (int)BP->start_address) / BP->buffers_size;
		ASSERT (idx >= 0);

		RMuint32 buflen;

		ASSERT (videoTimeScale);
		time = time * vopTimeIncrRes / videoTimeScale;

		while (length)
		{
			addref_buffer (&(BP->B[idx]));
			buflen = MY_MIN (0xffff, length);
			if (flags & CHUNK_FLAGS_TIME_VALID)
			{
				DEBUGMAINMSG (1, ("video cts = %d\n", (RMint32)time));
				mpegerr = pMpegDecoder->WriteMpegVideo (pBuffer, buflen, &(BP->B[idx]), CTS_AVAILABLE_FLAG, 
					mp4_scr0, mp4_scr0 + mp4_scrDelay + time);
			}
			else
			{
				mpegerr = pMpegDecoder->WriteMpegVideo (pBuffer, buflen, &(BP->B[idx]), 0, 0, 0);
			}
			ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);
			if (mpegerr != MPEG_DECODER_ERROR_NO_ERROR)
			{
				ASSERT (0);
				release_buffer (&(BP->B[idx]));
				return 0;
			}
			flags = 0;
			pBuffer += buflen;
			length -= buflen;
			ASSERT ((RMint32)length >= 0);
		}
	}
	else if (trackid == 1)
	{
		RMint32 n, t0, t1, acount;
		RMint16 *pcm;
		RMint32 aaclen = (RMint32)length;

		ASSERT (audioTimeScale);
		// VERY important - change audio time scale to video time scale
		// it is best if both audio and video have the same timescales
		if (flags & CHUNK_FLAGS_TIME_VALID)
		{
			time = time * vopTimeIncrRes / audioTimeScale;
			aacframe_valid = 0;
		}

		acount = 0;
		t0 = gettime_ms ();
		while (aaclen > 0)
		{
			if (aacframe_valid)
			{
				RMint32 templen = MY_MIN (aaclen, sizeof (aacframe) - aacframe_valid);
				memcpy (aacframe+aacframe_valid, pBuffer, templen);

				int idx;
				do
				{
					idx = find_free_buffer (&BPpcm, 0);
					if ((idx < 0) && prebuffering)
					{
//						pMpegDecoder->RenderText (0, 0, 0, 0, 320, 240);
						pMpegDecoder->Play ();
						prebuffering = 0;
						DEBUGMAINMSG (1, ("play due to pcm buffer full\n"));
					}
				} while (idx < 0);
				pcm = (RMint16 *)get_buffer_address (&BPpcm, idx);	

				n = pAACDecoder->DecodeFrame (aacframe, aacframe_valid+templen, pcm, 4096);
				if (n > 0)
				{
					MPEG_DECODER_ERROR err;
					addref_buffer (&(BPpcm.B[idx]));
					err = pMpegDecoder->WritePCM ((RMuint8 *)pcm, 2048*NumberOfChannels, &(BPpcm.B[idx]), 0, 0, 0);
					ASSERT (err == MPEG_DECODER_ERROR_NO_ERROR);
				}
				ASSERT (n > 0);
				ASSERT (n/8 <= (RMint32)(aacframe_valid+templen));
				ASSERT ((n/8 - aacframe_valid) >= 0);
				pBuffer += (n/8 - aacframe_valid);
				aaclen -= (n/8 - aacframe_valid);
				aacframe_valid = 0;
			}
			if (aaclen)
			{
				ASSERT (aaclen > 0);

				int idx;
				do
				{
					idx = find_free_buffer (&BPpcm, 0);
					if ((idx < 0) && prebuffering)
					{
//						pMpegDecoder->RenderText (0, 0, 0, 0, 320, 240);
						pMpegDecoder->Play ();
						prebuffering = 0;
						DEBUGMAINMSG (1, ("play due to pcm buffer full\n"));
					}
				} while (idx < 0);
				pcm = (RMint16 *)get_buffer_address (&BPpcm, idx);	

				n = pAACDecoder->DecodeFrame (pBuffer, aaclen, pcm, 4096);
				if (n > 0)
				{
					MPEG_DECODER_ERROR err;
					addref_buffer (&(BPpcm.B[idx]));
					if (flags & CHUNK_FLAGS_TIME_VALID)
					{
						DEBUGMAINMSG (0, ("audio cts = %d\n", (RMint32)time));
						err = pMpegDecoder->WritePCM ((RMuint8 *)pcm, 2048*NumberOfChannels, &(BPpcm.B[idx]), CTS_AVAILABLE_FLAG, 
							mp4_scr0, mp4_scr0 + mp4_scrDelay + time);
					}
					else
						err = pMpegDecoder->WritePCM ((RMuint8 *)pcm, 2048*NumberOfChannels, &(BPpcm.B[idx]), 0, 0, 0);
					flags = 0;
					ASSERT (err == MPEG_DECODER_ERROR_NO_ERROR);
				}
				if (n < 0)
				{
					memcpy (aacframe, pBuffer, aaclen);
					aacframe_valid = aaclen;
					break;
				}
				pBuffer += (n/8);
				aaclen -= (n/8);
				ASSERT (aaclen >= 0);
			}
			acount++;
		}
		t1 = gettime_ms ();
		DEBUGMAINMSG (0, ("%d frames, %d ms\n", acount, t1-t0));
		aacframe_count += acount;
		aacdecode_time += (t1-t0);
	}
	return 0;
}



// avi callback function - the initial examination of the avi file
// may take some time.  the avi demux object is calling back the 
// application to give it a percentage of where the current loading
// process is.  this callback is called within the context of ::DemuxFile
// it is provided so the application can give the user some kind
// of feedback when the loading process is occuring

// avi callback function - required by the avi demux object
// to get the current time in ms
static RMuint32 avi_gettime (void *context)
{
	return gettime_ms ();
}

static RMuint32 mpg_info (RMint32 msg, void *info, void *context)
{
	return 0;
}

// mpeg demux file callback
static RMuint32 mpg_putpayload (RMuint8 *pData, RMuint32 Length, RMuint8 StreamId, RMuint8 SubStreamId,
	RMint64 Scr, RMint64 Pts, RMint64 Dts, RMuint8 Flags, void *context)
{
	if (Length == 0)
		return 0;
	if (videoStreamId == 0xff)
	{
		if ((StreamId & 0xf0) == 0xe0)
		{
			// mpeg1/2 video
			videoStreamId = StreamId;
			DEBUGMAINMSG (1, ("videoStreamId = %02x\n", videoStreamId));
		}
	}
	if (audioStreamId == 0xff)
	{
		if ((StreamId & 0xf0) == 0xc0)
		{
			// mpeg audio
			// check sampling rate
			RMuint8 *p = pData;
			for (RMuint32 i=0; i<Length-3; i++)
			{
				// check header
				if ((p[0] == 0xff) && ((p[1] & 0xf0) == 0xf0))
				{
					// check layer
					if ((p[1] & 0x6) != 0)
					{
						// check bitrate index
						if ((p[2] & 0xf0) != 0xf0)
						{
							// check sample frequency
							if ((p[2] & 0xc) != 0xc)
							{
								RMint32 audioFreqTable[] = {44100, 48000, 32000, 44100};
								// assume ok ...
								audioStreamId = StreamId;
								audioSubStreamId = SubStreamId;
								audioFreq = audioFreqTable[(p[2] & 0xc) >> 2];
								DEBUGMAINMSG (1, ("StreamId = %02x, SubStreamId = %02x, mpeg @ %d\n", audioStreamId, audioSubStreamId, audioFreq));
								MPEGAUDIO_PARAMETERS mpeginfo;
								mpeginfo.layer = 2;
								mpeginfo.rawOutput = 0;
								mpeginfo.SamplesPerSecond = audioFreq;
								pMpegDecoder->SetMpegAudioParameters (&mpeginfo);
								break;
							}
						}
					}
				}
				p++;				
			}
		}
		if (StreamId == 0xbd)
		{
			if ((SubStreamId & 0x88) == 0x80)
			{
				// ac3
				audioStreamId = StreamId;
				audioSubStreamId = SubStreamId;
				// assume 48000
				audioFreq = 48000;
				DEBUGMAINMSG (1, ("StreamId = %02x, SubStreamId = %02x, ac3\n", audioStreamId, audioSubStreamId));
				AC3_PARAMETERS ac3info;
				ac3info.rawOutput = 0;
				ac3info.SamplesPerSecond = audioFreq;
				pMpegDecoder->SetAC3Parameters (&ac3info);
			}
		}
	}
	if (videoStreamId == StreamId)
	{
		BufferPool *BP = &BPmpg;
		int idx = ((int)pData - (int)BP->start_address) / BP->buffers_size;
		ASSERT (idx >= 0);
		addref_buffer (&(BP->B[idx]));
		if (Flags & PTS_VALID_FLAG)
		{
			Flags = PTS_AVAILABLE_FLAG;
			videoTime = Pts;
			if (Scr == 0)
				Scr = 1;
		}
		else
			Flags = 0;
		
		pMpegDecoder->WriteMpegVideo (pData, Length, &(BP->B[idx]), Flags, Scr, Pts);
	}
	if ((audioStreamId == StreamId) && (audioSubStreamId == SubStreamId))
	{
		BufferPool *BP = &BPmpg;
		int idx = ((int)pData - (int)BP->start_address) / BP->buffers_size;
		ASSERT (idx >= 0);
		addref_buffer (&(BP->B[idx]));
		if (Flags & PTS_VALID_FLAG)
		{
			Flags = PTS_AVAILABLE_FLAG;
			audioTime = Pts;
			if (Scr == 0)
				Scr = 1;
		}
		else
			Flags = 0;
		pMpegDecoder->WriteMpegAudio (pData, Length, &(BP->B[idx]), Flags, Scr, Pts);
	}
	return 0;
}

static RMuint32 mpg_getDetectionBuffer (RMuint8 **pBuffer, RMuint32 *plength, void *context)
{
	// supplied buffer must be of at least *plength (16k)
	*pBuffer = (RMuint8 *)mpg_buffer;
	return 0;
}

static RMuint32 mpg_getMPG (RMuint8 **pBuffer, RMuint32 *plength, void *context)
{
	int idx = find_free_buffer (&BPmpg, 0);
	if (idx < 0)
	{
		if (prebuffering)
		{
			fs_powerdown ();
//			pMpegDecoder->RenderText (0, 0, 0, 0, 320, 240);
			DEBUGMAINMSG (1, ("start decode due to mpeg buffer full\n"));
			prebuffering = 0;
			pMpegDecoder->Play ();
		}
		return 1;
	}
	*plength = (RMuint32)BPmpg.buffers_size;
	*pBuffer = (RMuint8 *)get_buffer_address (&BPmpg, idx);
	return 0;
}

// render a mpeg file
static RMuint32 render_mpegfile (RMuint16 *fullname, RMuint32 blockinghandle)
{
    bool bHddSleep = false;
	DEBUGMAINMSG (1, ("render mpeg file %s with ID %d\n", fullname, blockinghandle));
	ASSERT (video_blockinghandle == 0);
	ASSERT (audio_blockinghandle == 0);
	ASSERT (image_blockinghandle == 0);
	ASSERT (pAVIDemux == 0);
	ASSERT (pMP3Decoder == 0);
	ASSERT (pMPGDemux == 0);

	videoStreamId = 0xff;
	audioStreamId = 0xff;
	audioSubStreamId = 0xff;
	audioFreq = 0;
	videoTime = 0;
	audioTime = 0;

	MPG_CALLBACK_TABLE callback;
	pMPGDemux = (MPEGDemux *) new MPEGDemux;
	ASSERT (pMPGDemux);
	if (pMPGDemux == 0)
	{
		ASSERT (0);
		return ERR_VR_UNKNOWN;
	}

	init_buffer_pool (&BPmpg, mpg_buffer, MPG_BLOCK_SIZE, MPG_NBLOCKS);
	pMPGDemux->Init ();
	callback.context = 0;
	#ifdef MEM64MB
  if( TaviSysParam.power.hddtimer /*&& !g_MiscCtrl->ACDetected()*/ ) {
      bHddSleep = true;
      callback.fopen = mio_wfopen;
      callback.fread = mio_fread;
      callback.fseek = (MPG_fseek)mio_fseek2;
      callback.ftell = mio_ftell;
      callback.fclose = mio_fclose;
  }
  else {
  #endif
      callback.fopen = fs_fat_wfopen;
      callback.fread = fs_fread;
      callback.fseek = (MPG_fseek)fs_fseek2;
      callback.ftell = fs_ftell;
      callback.fclose = fs_fclose;
  #if MEM64MB
  }
  #endif
	callback.addref = my_addref;
	callback.release = my_release;
	callback.info = mpg_info;
	callback.putpayload = mpg_putpayload;
	callback.getMPG = mpg_getMPG;
	callback.getDetectionBuffer = mpg_getDetectionBuffer;
	callback.loading = DrawLoadingProgressV4;


	pMPGDemux->InitCallbackTable (&callback);
	if (pMPGDemux->DecodeFile (fullname) == MPG_DEMUX_ERROR_NO_ERROR) {
		MPEGVIDEO_PARAMETERS videoparams;
		videoparams.isMPEG4 = 0;
		videoparams.videoTimeScale = 0;
		videoparams.fixedVOPRate = 0;
		videoparams.vopTimeIncr = 0;
		pMpegDecoder->SetMpegVideoParameters (&videoparams);

		currentTime = 0;
		video_blockinghandle = blockinghandle;
		pMpegDecoder->Stop ();
		prebuffering = 1;

        printf( "MPEG BITRATE: %d\n", pMPGDemux->GetVideoBitrate() );
		if( pMPGDemux->GetVideoBitrate() > MAX_VIDEO_BITRATE && bHddSleep ) {
		    callback.fopen = fs_fat_wfopen;
            callback.fread = fs_fread;
            callback.fseek = (MPG_fseek)fs_fseek2;
            callback.ftell = fs_ftell;
            callback.fclose = fs_fclose;
            mio_destroyer();
            pMPGDemux->ReInitCallbackTable( &callback );
		}
	}
	else {
		DEBUGMAINMSG (1, ("render mpeg file %s failed\n", fullname, blockinghandle));
		deinit_buffer_pool (&BPmpg);
		return ERR_VR_UNKNOWN;
	}
	return ERR_VR_NOERROR;
}

// render an avi file
static RMuint32 render_avifile (RMuint16 *fullname, RMuint32 blockinghandle)
{
    bool bHddSleep = false;
	AVI_DEMUX_ERROR DmxErr;
	RMuint32 ret = 0;
	AVI_CALLBACK_TABLE callback_table;

	_aviauBlockPerSec = 0;
    _aviauAvgBytesPerSec = 0;
    _aviauLength = 0;
    _aviMoviChunkSize = 0;


	DEBUGMAINMSG (1, ("render avi file %s with ID %d\n", fullname, blockinghandle));
	ASSERT (video_blockinghandle == 0);
	ASSERT (audio_blockinghandle == 0);
	ASSERT (image_blockinghandle == 0);
	ASSERT (pAVIDemux == 0);
	ASSERT (pMP3Decoder == 0);
	pAVIDemux = (AVIDemux *) new AVIDemux;
	ASSERT (pAVIDemux);
	if (pAVIDemux)
	{		
		callback_table.context = &BPavi;		// saved context information for application
		#ifdef MEM64MB
    if( TaviSysParam.power.hddtimer /*&& !g_MiscCtrl->ACDetected()*/ ) {
        bHddSleep = true;
        callback_table.fopen = mio_wfopen;
        callback_table.fread = mio_fread;
        callback_table.ftell = mio_ftell;
        callback_table.fseek = mio_fseek;
        callback_table.fclose = mio_fclose;
    }
    else {
    #endif
        callback_table.fopen = fs_fat_wfopen;
        callback_table.fread = fs_fread;
        callback_table.ftell = fs_ftell;
        callback_table.fseek = fs_fseek;
        callback_table.fclose = fs_fclose;
    #ifdef MEM64MB
    }
    #endif
		callback_table.addref = my_addref;
		callback_table.release = my_release;
		callback_table.info = avi_info_V19;
		callback_table.getbuffer = avi_getAVI;
		callback_table.putChunk = avi_putChunk_V19;
		callback_table.loading = DrawLoadingProgressV4;
		callback_table.gettimems = avi_gettime;

		init_buffer_pool (&BPavi, avi_buffer, AVI_BLOCK_SIZE, AVI_NBLOCKS);
		ASSERT (MAIN_DATA_BUFFER_SIZE >= (AVI_BLOCK_SIZE*AVI_NBLOCKS));

		avi_rebuffer = 0;
		video_stream_count = 0;
		audio_stream_count = 0;
		avi_stream_count = 0;
		selected_audio_stream[0] = 0xff;
		selected_audio_stream[1] = 0xff;
		selected_audio_stream[2] = 0;
		selected_audio_stream_format = 0;
		pAVIDemux->Init ();
		pAVIDemux->InitCallbackTable (&callback_table);
		pAVIDemux->m_IsInvalidFPSVideoEnabled = IsInvalidFPSVideoEnabled;
		
		if ( (DmxErr=pAVIDemux->DemuxFile_V19(fullname)) != AVI_DEMUX_ERROR_NO_ERROR) {
			DEBUGMAINMSG (1, ("pAVIDemux->DemuxFile (%s) failed : DmxErr=0x%x\n", fullname, DmxErr));		
			delete pAVIDemux;
			pAVIDemux = 0;
			deinit_buffer_pool (&BPavi);

			if( DmxErr ==  AVI_DEMUX_ERROR_FPS_VALUE_INVALID )
				return ERR_VR_FPS_VALUE_INVALID;
				
			if( ret != ERR_VR_RES_OVER && ret != ERR_VR_FPS_VALUE_INVALID)	//4444
				ret = ERR_VR_UNKNOWN;	//1234
		}

		if (IsFOURCCSupported (currentFOURCC) == 0)
		{
			DEBUGMAINMSG (1, ("not supported FOURCC\n"));		
			delete pAVIDemux;
			pAVIDemux = 0;
			deinit_buffer_pool (&BPavi);
			ret = ERR_VR_UNKNOWN;
		}
		else {
			if( (currentWidth*currentHeight) > MAX_VIDEO_RESOLUTION )
				{
				printf("%d x %d Video does not supported.\n", currentWidth, currentHeight);
				delete pAVIDemux;
				pAVIDemux = 0;
				deinit_buffer_pool (&BPavi);
				ret = ERR_VR_RES_OVER;
				}
			else if(  currentWidth > 800 || currentHeight > 800 )
				{
				printf("%d x %d Video does not supported.\n", currentWidth, currentHeight);
				delete pAVIDemux;
				pAVIDemux = 0;
				deinit_buffer_pool (&BPavi);
				ret = ERR_VR_RES_OVER;
				}
		}
		
		if ((pAVIDemux == 0) && (pMP3BufferDecoder))
		{
			pMP3BufferDecoder->DecodeBufferEnd ();
			delete pMP3BufferDecoder;
			pMP3BufferDecoder = 0;
			deinit_buffer_pool (&BPpcm);		

			if( ret != ERR_VR_RES_OVER )
				ret = ERR_VR_UNKNOWN;

			return ret;			
		}
	}
	if (pAVIDemux)
	{
		if (transcodeVideo) {
			init_buffer_pool (&BPscan, mp43pending_buffer, 1024*16, 24);
			// initialize the mp43 scanning library
			RMint32 err;
			MP43Scanner.width = currentWidth;
			MP43Scanner.height = currentHeight;
			MP43Scanner.iRate = currentRate;
			MP43Scanner.iScale = currentScale;
			MP43Scanner.buffer = mp43scan_buffer;
			err = MP43_filter_init (&MP43Scanner);
			ASSERT (err == FILTER_OK);
			mp43scan_status = 0;
		}
		else if( IsXvidGmcEnabled == FALSE && currentFOURCC == XVID_FOURCC ) {
			RMint32 length, i;
			RMint32 idx = find_free_buffer (&BPavi, 0);
			ASSERT (idx >= 0);
			RMuint8 *p = (RMuint8 *)get_buffer_address (&BPavi, idx);
			length = BPavi.buffers_size;
			pAVIDemux->GetFrame (0, p, (RMuint32 *)&length);
			p += 8;
			for (i=0; i<length-4; i++)
			{
				if ((p[0] == 0x00) && 
					(p[1] == 0x00) &&
					(p[2] == 0x01) &&
					((p[3] >= 0x20) && (p[3] <= 0x2f)))
				{
					RMint32 gmc, qpel;
					ParseVOL (p, length, &gmc, &qpel);

					if (gmc || qpel)
						{
						delete pAVIDemux;
						pAVIDemux = 0;
						deinit_buffer_pool (&BPavi);
						if (pMP3BufferDecoder)
						{
							pMP3BufferDecoder->DecodeBufferEnd ();
							delete pMP3BufferDecoder;
							pMP3BufferDecoder = 0;
							deinit_buffer_pool (&BPpcm);            
						}
						currentTime = 0;
						video_blockinghandle = blockinghandle;
						pMpegDecoder->Stop ();
						prebuffering = 1;

						if(gmc)
							return ERR_VR_GMC;

						return ERR_VR_QPEL;
						}
					
					break;
				}
				p++;
			}
		}

		currentTime = 0;
		video_blockinghandle = blockinghandle;
		pMpegDecoder->Stop ();
		prebuffering = 1;

		/*
		    Calcurates approximate video bitrate
		*/
		int SizeOfHeader = _vidTotalFrames*8*audio_stream_count;
		int SizeOfAudio = _aviauAvgBytesPerSec/_aviauBlockPerSec*_aviauLength;
		int SizeOfVideo = pAVIDemux->GetFileSize() - SizeOfAudio - SizeOfHeader;
		int compression = pAVIDemux->GetVideoCompression();
		bool sleepable = true;
		float a;
		a = (float)SizeOfVideo/1000;
		a /= (float)_vidTotalFrames;
		a *= (float)(_avividFramesPerSec)*8;
		int bps = (int)a;

		printf( "VideoKilobitsPerSecond: %d\n", bps );
		if( bps > MAX_VIDEO_BITRATE ) {
		    sleepable = false;
		}

		// DX50 is not sleepable
		#if 0
		if( !strncasecmp((char*)&compression, "DX50", 4) ) {
		    printf( "DX50!!\n" );
		    sleepable = false;
		}
		#endif
		
		if( !sleepable && bHddSleep ) {
		    callback_table.fopen = fs_fat_wfopen;
            callback_table.fread = fs_fread;
            callback_table.ftell = fs_ftell;
            callback_table.fseek = fs_fseek;
            callback_table.fclose = fs_fclose;
            mio_destroyer();
            pAVIDemux->ReInitCallbackTable( &callback_table );
		}
	}
	else {
		if( ret != ERR_VR_RES_OVER) {
			ret = ERR_VR_UNKNOWN;	
		}
	}

	IsXvidGmcEnabled = FALSE;
	return ret;
	//return 0;
}

// render a mp4 file
static RMuint32 render_mp4file (RMuint16 *fullname, RMuint32 blockinghandle)
{
	DEBUGMAINMSG (1, ("render mp4 file %s with ID %d\n", fullname, blockinghandle));
	ASSERT (video_blockinghandle == 0);
	ASSERT (audio_blockinghandle == 0);
	ASSERT (image_blockinghandle == 0);
	ASSERT (pAVIDemux == 0);
	ASSERT (pMP3Decoder == 0);
	ASSERT (pMP4Demux == 0);
	ASSERT (pAACDecoder == 0);

	pMP4Demux = (MP4Demux *)new MP4Demux;
	ASSERT (pMP4Demux);
	if (pMP4Demux == 0)
	{
		DEBUGMAINMSG (1, ("failed to create mp4 demux object\n"));
		return ERR_VR_UNKNOWN;		
	}
	pAACDecoder = (AACDecoder *)new AACDecoder;
	ASSERT (pAACDecoder);
	if (pAACDecoder == 0)
	{
		DEBUGMAINMSG (1, ("failed to create mp4 demux object\n"));
		delete pMP4Demux;
		pMP4Demux = 0;
		return ERR_VR_UNKNOWN;
	}
	init_buffer_pool (&BPmp4, (RMuint32 *)mp4_buffer, MP4_BLOCK_SIZE, MP4_NBLOCKS);
	init_buffer_pool (&BPpcm, (RMuint32 *)pcm_buffer, PCM_BLOCK_SIZE, PCM_NBLOCKS);

	pAACDecoder->Init ();

	MP4_CALLBACK_TABLE CallbackTable;
	CallbackTable.addref = my_addref;
	CallbackTable.context = 0;
    CallbackTable.fclose = fs_fclose;
    CallbackTable.fopen = fs_fat_wfopen;
    CallbackTable.fread = fs_fread;
    CallbackTable.fseek = fs_fseek2;
    CallbackTable.ftell = fs_ftell;
	CallbackTable.info = mp4_info;
	CallbackTable.getData = mp4_getData;
	CallbackTable.putDSI = mp4_putDSI;
	CallbackTable.putChunk = mp4_putChunk;
	CallbackTable.release = my_release;
	CallbackTable.loading = DrawLoadingProgressV4;
	pMP4Demux->Init ();
	pMP4Demux->InitCallbackTable (&CallbackTable);
	selected_spstream = 0;
	selected_audiostream = 0;
	if (pMP4Demux->Demux (fullname) == MP4_ERROR_NO_ERROR)
	{
		currentTime = 0;
		video_blockinghandle = blockinghandle;
	}
	else
	{
		DEBUGMAINMSG (1, ("not a valid .mp4 file\n"));
		deinit_buffer_pool (&BPmp4);
		deinit_buffer_pool (&BPpcm);
		delete pAACDecoder;
		delete pMP4Demux;
		pMP4Demux = 0;
		pAACDecoder = 0;

		return ERR_VR_UNKNOWN;		
	}

	return ERR_VR_NOERROR;
}

// filebrowser callback - the user has pressed "stop", and the
// filebrowser is telling the application to perform a stop
// on the current media
RMuint32 fb_renderstop (void *context)
{
	ASSERT (audio_blockinghandle || video_blockinghandle);
	if (audio_blockinghandle)
	{
		ASSERT (pMP3Decoder || pAC3Demux);		
		if (pMP3Decoder)
		{
			delete pMP3Decoder;
			pMP3Decoder = 0;
			deinit_buffer_pool (&BPmp3);
			deinit_buffer_pool (&BPpcm);
		}
		if (pAC3Demux)
		{
			delete pAC3Demux;
			pAC3Demux = 0;
			deinit_buffer_pool (&BPac3);
		}
		tbytes = 0;
		pMpegDecoder->Stop ();
	}
	if (video_blockinghandle)
	{
	    pMpegDecoder->Stop ();
		if (pMP3BufferDecoder)
		{
			pMP3BufferDecoder->DecodeBufferEnd ();
			delete pMP3BufferDecoder;
			pMP3BufferDecoder = 0;
			deinit_buffer_pool (&BPpcm);			
		}
		ASSERT (pAVIDemux || pMPGDemux || pMP4Demux);
		if (pAVIDemux)
		{
			delete pAVIDemux;
			pAVIDemux = 0;			
			deinit_buffer_pool (&BPavi);	
			tbytes = 0;
			if (transcodeVideo)
			{
				deinit_buffer_pool (&BPscan);
				RMint32 err;
				err = MP43_filter_exit (&MP43Scanner);
				ASSERT (err == FILTER_OK);
			}
		}
		if (pMPGDemux)
		{
			delete pMPGDemux;
			pMPGDemux = 0;
			deinit_buffer_pool (&BPmpg);
		}
		if (pMP4Demux)
		{
			delete pMP4Demux;
			pMP4Demux = 0;
			ASSERT (pAACDecoder);
			delete pAACDecoder;
			pAACDecoder = 0;
			deinit_buffer_pool (&BPmp4);
			deinit_buffer_pool (&BPpcm);
		}
	}
	image_blockinghandle = 0;
	audio_blockinghandle = 0;
	video_blockinghandle = 0;
	ASSERT (image_blockinghandle == 0);
	ASSERT (audio_blockinghandle == 0);
	ASSERT (video_blockinghandle == 0);
	return 0;
}

// renders a jpeg image as the background
#include <Lib/LibWideString.h>
RMuint32 jpeg_fclose( RMuint32 handle, void* pContext )
{
	return fatfs_fclose( handle );
}

RMuint32 jpeg_fopen( RMuint8* filename, void* pContext )
{
	RMuint32 handle;
	FATFS_ERROR fatfs_err;

	// wide-character
	if( (filename[1] == 0&&filename[0]) || filename[1]&0x80 ) {
		fatfs_err = fatfs_wfopen( (RMuint16*)filename, _O_RDONLY, &handle );
	}
	else {
		fatfs_err = fatfs_fopen  ( filename, _O_RDONLY, &handle );
	}
	if( fatfs_err != FATFS_ERROR_NO_ERROR ) {
		printf( "failed to JPEG_fopen(err: %x), %s\n", fatfs_err, (char*)filename );
		return 0;
	}
	return handle;
}

RMuint32 jpeg_fread (RMuint32 handle, void *buf, RMuint32 length, void *context)
{
	FATFS_ERROR fatfs_err;
	RMuint32 n;
	fatfs_err = fatfs_fread (handle, (RMuint8 *)buf, length, &n);
	if( fatfs_err != FATFS_ERROR_NO_ERROR ) {
		printf( "failed to JPEG_fread\n" );
		ASSERT(0);
		return 0;
	}

	return n;
}

// filebrowser callback - the filebrowser is asking the application
// if the current file is done rendering
//static 


RMuint32 fb_isrenderdone (void *context)
{
    RMuint32 ret;
    
	if (video_blockinghandle) {
	    ret = 0;

		SetPlayTime();
		if (pAVIDemux) {
			sw_decode_mp3 ();

      AVI_DEMUX_ERROR err = pAVIDemux->Schedule();
			//if (pAVIDemux->Schedule () == AVI_DEMUX_ERROR_FILE_DONE)
			if( err == AVI_DEMUX_ERROR_FILE_DONE ) {
				if (prebuffering) {
					prebuffering = 0;
					pMpegDecoder->Play ();
				}
				if (pMpegDecoder->EndOfStream (MY_MAX (videoTime, audioTime), aviTimeScale) == MPEG_DECODER_ERROR_EOF)
				{
					if (pMP3BufferDecoder)
					{
						pMP3BufferDecoder->DecodeBufferEnd ();
						delete pMP3BufferDecoder;
						pMP3BufferDecoder = 0;
						deinit_buffer_pool (&BPpcm);					
					}
					delete pAVIDemux;
					pAVIDemux = 0;
					video_blockinghandle = 0;
					deinit_buffer_pool (&BPavi);
					tbytes = 0;
					if (transcodeVideo)
					{
						deinit_buffer_pool (&BPscan);
						RMint32 err;
						err = MP43_filter_exit (&MP43Scanner);
						ASSERT (err == FILTER_OK);
					}
					pMpegDecoder->Stop ();				
					return 1;
				}
				ret = 0;
			}
			else {
				ret = 0;
			}

			return ret;
		}
		else if (pMPGDemux)
		{
			if (pMPGDemux->Schedule () == MPG_DEMUX_ERROR_FILE_DONE)
			{
				if (prebuffering)
				{
					DEBUGMAINMSG (1, ("start decode due to mpg eos\n"));
					prebuffering = 0;
					pMpegDecoder->Play ();
				}
				fs_powerdown ();
				if (pMpegDecoder->EndOfStream (MY_MAX (videoTime, audioTime), 90000) == MPEG_DECODER_ERROR_EOF)
				{
					fs_powerup ();
					sleep (1);
					delete pMPGDemux;
					pMPGDemux = 0;
					video_blockinghandle = 0;
					deinit_buffer_pool (&BPmpg);
					pMpegDecoder->Stop ();				
					return 1;
				}
			}
			else
			{
				return 0;
			}
		}
		else if (pMP4Demux)
		{
			if (pMP4Demux->Schedule () == MP4_ERROR_END_OF_FILE)
			{
				if (prebuffering)
				{
					DEBUGMAINMSG (1, ("start decode due to mp4 eos\n"));
					prebuffering = 0;
					pMpegDecoder->Play ();
				}
				fs_powerdown ();
				if (pMpegDecoder->EndOfStream (-1, 0) == MPEG_DECODER_ERROR_EOF)
				{
					fs_powerup ();
					sleep (1);
					ASSERT (pMP4Demux);
					delete pMP4Demux;
					pMP4Demux = 0;
					ASSERT (pAACDecoder);
					delete pAACDecoder;
					pAACDecoder = 0;	
					ASSERT (video_blockinghandle);
					video_blockinghandle = 0;
					deinit_buffer_pool (&BPmp4);
					deinit_buffer_pool (&BPpcm);
					pMpegDecoder->Stop ();				
					return 1;
				}
			}
			else
				return 0;
		}
		else
			ASSERT (0);
	}
	if ((image_blockinghandle) || (audio_blockinghandle) || (video_blockinghandle))
		return 0;
	return 1;
}

int _SeekMpeg4( int t )
{
	MPEG_DECODER_ERROR mpegerr;
	RMuint32 vp, ap, ab;
#if 0		
		RMint64 i64NewSeekSec = (RMint64)t * 1000;
		DEBUGMAINMSG(1,("Seek7(%d)\n-i64NewSeekSec:%lld\n",t, i64NewSeekSec));
		pMP4Demux->Seek ( i64NewSeekSec );
/*		
		pMpegDecoder->Pause();

		deinit_buffer_pool( &BPmp4 );
		deinit_buffer_pool( &BPpcm );
		init_buffer_pool (&BPmp4, (RMuint32 *)mp4_buffer, MP4_BLOCK_SIZE, MP4_NBLOCKS);
		init_buffer_pool (&BPpcm, (RMuint32 *)pcm_buffer, PCM_BLOCK_SIZE, PCM_NBLOCKS);
*/
//		pMpegDecoder->Play();
#endif		
	return 0;
}
int _SeekMpeg( int t )
{
	MPEG_DECODER_ERROR mpegerr;
	RMuint32 vp, ap, ab;
	
	pMPGDemux->Seek( t );
	pMpegDecoder->Stop();		

	deinit_buffer_pool (&BPmpg);
	deinit_buffer_pool (&BPpcm);
	init_buffer_pool (&BPpcm, pcm_buffer, PCM_BLOCK_SIZE, PCM_NBLOCKS);
	init_buffer_pool (&BPmpg, mpg_buffer, MPG_BLOCK_SIZE, MPG_NBLOCKS);

	pMpegDecoder->Play ();
	return 0;
}

int __FindVolHeader_OnTranscodeVideo( RMuint8* p, RMuint32 length, RMint32 idx )
{
	MPEG_DECODER_ERROR mpegerr;
	RMuint32 i;
	RMuint8 *pVOLHdr;

	//MP43_filter_flush (&MP43Scanner);

	DEBUGMAINMSG (1, ("create a vol header (seek)\n"));
	DEBUGMAINMSG(1,("Seek7-transcodeVideo\n-currentScale:%d\n-currentRate:%d\n-currentWidth:%d\n-currentHeight:%d\n",currentScale, currentRate, currentWidth, currentHeight));
	mp4_VOLHdrLength = MP43_DSI_Generator (mp4_VOLHdr, currentScale, currentRate, currentWidth, currentHeight);
	addref_buffer (&BPavi.B[idx]);
	memcpy (p, mp4_VOLHdr, mp4_VOLHdrLength);
	mpegerr = pMpegDecoder->WriteMpegVideo (p, mp4_VOLHdrLength, &BPavi.B[idx], 0, 0, 0);
	ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);

	if (mpegerr == MPEG_DECODER_ERROR_NO_ROOM) {
		release_buffer (&BPavi.B[idx]);
	}

	mp43scan_status = 0;

	return TRUE;
}

int __FindVolHeader( RMuint8* p, RMuint32 length, RMint32 idx )
{
	MPEG_DECODER_ERROR mpegerr;
	RMuint32 i;
	RMuint8 *pVOLHdr;

	DEBUGMAINMSG(1,("(!) there should always be a vol header at frame 0\n"));
	pAVIDemux->GetFrame (0, p, &length);

	p += 8;
	pVOLHdr = p;

	for (i=0; i<length-4; i++) {
		RELOAD_WATCHDOGTIMER;
		if(i>100)
			break;
		DEBUGMAINMSG(1,("(%ld/%ld)", i, length-4));
		if ((p[0] == 0x00) && 
			(p[1] == 0x00) &&
			(p[2] == 0x01) &&
			(p[3] == 0xb6)) {
			ASSERT (pMpegDecoder);
			ASSERT (i);
			if (i) {
				addref_buffer (&BPavi.B[idx]);
				ASSERT (i);
				mpegerr = pMpegDecoder->WriteMpegVideo (pVOLHdr, i, &BPavi.B[idx], 0, 0, 0);
				ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);
				if (mpegerr == MPEG_DECODER_ERROR_NO_ROOM) {
					release_buffer (&BPavi.B[idx]);
				}
			}
			return TRUE;
			break;
		}
		p++;
	}	

	
 	DEBUGMAINMSG (1, ("CAN'T FIND VOLHEADER... sending volheader: %d bytes", i));
	addref_buffer (&BPavi.B[idx]);
	ASSERT (i);
	mpegerr = pMpegDecoder->WriteMpegVideo (pVOLHdr, i, &BPavi.B[idx], 0, 0, 0);
	ASSERT (mpegerr == MPEG_DECODER_ERROR_NO_ERROR);
	if (mpegerr == MPEG_DECODER_ERROR_NO_ROOM) {
		release_buffer (&BPavi.B[idx]);
	}

	return FALSE;
}


int __FlushMpegDecoder()
{
	// flush mpeg decoder
	if( selected_audio_stream_format == 0x55 )
		{
		mp3chunkQ.Reset();
		}

	pMpegDecoder->Stop ();

	deinit_buffer_pool (&BPavi);
	deinit_buffer_pool (&BPpcm);

	if( transcodeVideo )
		{
		deinit_buffer_pool (&BPscan);
		init_buffer_pool (&BPscan, mp43pending_buffer, 1024*16, 24);
		}

	init_buffer_pool (&BPpcm, pcm_buffer, PCM_BLOCK_SIZE, PCM_NBLOCKS);
	init_buffer_pool (&BPavi, avi_buffer, AVI_BLOCK_SIZE, AVI_NBLOCKS);

	//pMpegDecoder->Play ();

	return TRUE;
}


static int __CalculateAVTime( RMuint32* vp, RMuint32* ap, RMuint32* ab )
{
	// calculate the initial audio and video time
	aviFrameCounter = *vp; 
	videoTime = aviStartupDelay + ((RMint64)aviFrameCounter * (RMint64)aviTicksPerFrame);
	if ( (*ab)) {
		if (selected_audio_stream_format == 0x55) {
			audioTime = aviStartupDelay + ((RMint64)(*ap) * (RMint64)aviTimeScale / (RMint64)CompressedBytesPerSecond);
			tbytes = (RMuint32)((RMint64)(*ap) * (RMint64)BytesPerSecond / (RMint64)CompressedBytesPerSecond);

			DEBUGMAINMSG (1, ("[T1-A]calculating the audioTime based on constant bitrate mp3\n"));
			DEBUGMAINMSG (1, ("-aviTimeScale:%d\n", aviTimeScale));
			DEBUGMAINMSG (1, ("-CompressedBytesPerSecond:%d\n", CompressedBytesPerSecond));
			DEBUGMAINMSG (1, ("-audioTime:%d\n", audioTime));
			DEBUGMAINMSG (1, ("-BytesPerSecond:%d\n", BytesPerSecond));
			DEBUGMAINMSG (1, ("-tbytes:%d\n", tbytes));
		}
		else {
			//avi_initial_start = 1;
			tbytes = *(ap);
			audioTime = aviStartupDelay + ((RMint64)tbytes * (RMint64)aviTimeScale / (RMint64)CompressedBytesPerSecond);

			DEBUGMAINMSG (1, ("[T1-B]calculating the audioTime based on constant bitrate\n"));
			DEBUGMAINMSG (1, ("-aviTimeScale:%d\n", aviTimeScale));
			DEBUGMAINMSG (1, ("-CompressedBytesPerSecond:%d\n", CompressedBytesPerSecond));
			DEBUGMAINMSG (1, ("-audioTime:%d\n", audioTime));
			DEBUGMAINMSG (1, ("-BytesPerSecond:%d\n", BytesPerSecond));
			DEBUGMAINMSG (1, ("-tbytes:%d\n", tbytes));

		}
	}
	else {
		// must be MP3
		// TODO: MUST FIND A FOMULA!!!
		ASSERT (selected_audio_stream_format == 0x55);
		audioTime = aviStartupDelay + 
		        ( (RMint64)(*ap) * 
		          _aviWaveFmt.nBlockAlign *
		          (RMint64)aviTimeScale / 
		          (RMint64)SamplesPerSecond );
		tbytes = (RMint64)(*ap) * 
		         _aviWaveFmt.nBlockAlign *
		         (NumberOfBitsPerSample / 8) * 
		         NumberOfChannels;

        #if 0
		printf("[T2]calculating the audioTime based on variable bitrate mp3\n");
		printf("-aviTimeScale:%d\n", aviTimeScale);
		printf("-SamplesPerSecond:%d\n", SamplesPerSecond);
		printf("-audioTime:%d\n", audioTime);
		printf("-NumberOfBitsPerSample:%d\n", NumberOfBitsPerSample);
		printf("-NumberOfChannels:%d\n", NumberOfChannels);			
		printf("-tbytes:%d\n", tbytes);	
		printf("-ap: %d\n", *ap );
		#endif
	}


	aviTime0 = MY_MIN (audioTime, videoTime);
	if (aviTime0 == 0)
		aviTime0 = 1;

	RMint8 text[64];

	RMint64 t, hours, minutes, seconds;

	t = videoTime / aviTimeScale;
	hours = t / 3600;
	minutes = (t - hours * 3600) / 60;
	seconds = t - hours*3600 - minutes*60;

	DEBUGMAINMSG (1, ("videoTime = %lu (%02d:%02d:%02d)\n", (RMuint32)videoTime, 
		(RMint32)hours, (RMint32)minutes, (RMint32)seconds));



	t = audioTime / aviTimeScale;
	hours = t / 3600;
	minutes = (t - hours * 3600) / 60;
	seconds = t - hours*3600 - minutes*60;

	DEBUGMAINMSG (1, ("audioTime = %lu (%02d:%02d:%02d)\n", (RMuint32)audioTime,
		(RMint32)hours, (RMint32)minutes, (RMint32)seconds));

	DEBUGMAINMSG (1, ("aviTime0  = %lu\n", (RMuint32)aviTime0));
	
	
	return TRUE;
}

int _SeekAvi( int t )
{
	MPEG_DECODER_ERROR mpegerr;
	RMuint32 vp, ap, ab;
	RMuint32 length;

	if (pAVIDemux->Seek (t, 1, &vp, &ap, &ab) == AVI_DEMUX_ERROR_NO_ERROR) {
		__FlushMpegDecoder();
		
		__CalculateAVTime(&vp, &ap, &ab);

		// send an initial VOL header
		RMint32 idx = find_free_buffer (&BPavi, 0);
		ASSERT (idx >= 0);
		RMuint8 *p = (RMuint8 *)get_buffer_address (&BPavi, idx);
		length = BPavi.buffers_size;

		if ( transcodeVideo ) 
			__FindVolHeader_OnTranscodeVideo(p, length, idx);
		else 
			__FindVolHeader(p, length, idx);

		pMpegDecoder->Play ();

		pcbtVideo->ResyncSMI( t );

		currentTime = t;
		return 0;	
		
	}
	else {
		return -1;
	}
}


int Seek7( int t )
{
    int ret;

	ClearKeyBuffer();
	prebuffering = 1;

	prevSeekValue = t;

	if( !pcbtVideo->GetSeekable() ){
		ret = 0;
	}
    else {
    	if( pMP4Demux ) {
    		ret = _SeekMpeg4(t);
    	}
    	else if( pMPGDemux ) {
    		ret = _SeekMpeg(t);
    	}
    	else if( selected_audio_stream_format == 0x2001) {
    		ret = -1;
    	}
    	else if( _SeekAvi(t) == 0 ) {
    		ret = 0;
    	}
    	else {
    		ret = -1;
    	}
	}

	return ret;
}

void 
SetPlayTime()
{
	if( pcbtVideo->IsSeekingState() == FALSE ) {
		RMint64 t = 0;
		pMpegDecoder->GetSTC (&t, 1);

    // Invalid time value after seeking a movie file.
		if( abs(currentTime-t) > 20 ) {
		    t = currentTime;
		}
		
		if ( t != currentTime ) {
			currentTime = t;
			playtime.SecOnly = currentTime;
			playtime.hours 	 = currentTime / 3600;
			playtime.minutes = (currentTime - playtime.hours * 3600) / 60;
			playtime.seconds = currentTime - playtime.hours*3600 - playtime.minutes*60;

			if( pcbtVideo->IsStatusbarVisible() && currentTime > 1 ) {
				pcbtVideo->DrawPlayTime( &playtime, OSD_REFRESH );
				pcbtVideo->DrawProgressBar(1);
			}
		}
	}
}


// renders a jpeg image as the background
void 
showbackground (char *bkgnd)
{
	int outW, outH;

	JPEG_CALLBACK_TABLE jpeg_callback_table;

	TAVI_WakeUp(0);
	
	jpeg_callback_table.context = 0;
	jpeg_callback_table.fclose = (JPEG_fclose)jpeg_fclose;
	jpeg_callback_table.fopen = (JPEG_fopen)jpeg_fopen;
	jpeg_callback_table.fread = (JPEG_fread)jpeg_fread;
	jpeg_callback_table.jpeg_progress = jpeg_decoding_progress;

	DEBUGMAINMSG (1, ("showbackground (%s)\n", bkgnd));

	if (decode_jpeg_image_420 (
                bkgnd, (RMuint8 *)backgroundY, (RMuint8 *)backgroundUV, 
                852*480, (852*480)/2, &outW, &outH, &jpeg_callback_table) != 0)
	{
		DEBUGMAINMSG (1, ("%s failed to decode\n", bkgnd));
		return;
	}
//	pMpegDecoder->SetVideoDestination( 0, 0, 320, 240 );
	int x = outW;
	int y = outH;
	int i, j;
	outW = (outW + 15) & ~0xF;
	outH = (outH + 15) & ~0xF;
	// black out non-multiple of 16
	RMuint8 *p;
	p = (RMuint8 *)scaled_backgroundY;
	for (j=0; j<outH; j++) {
		for (i=0; i<outW; i++) {
			if ((i >= x) || (j >= y))
				*p = 0;
			p++;
		} 
	}
	p = (RMuint8 *)scaled_backgroundUV;
	for (j=0; j<outH; j+=2)	{
		for (i=0; i<outW; i+=2) {
			if ((i >= x) || (j >= y)) {
				*p = 0x80;
				*(p+1) = 0x80;
			}
			p += 2;
		}
	}
	 x = 0, y = 0;
	if( outW < 320 ) x = (320-outW)/2;
	if( outH < 240 ) y = ( 240-outH )/2;
	
	pMpegDecoder->DisplayYUV420(
                    (RMuint8 *)backgroundY, (RMuint8 *)backgroundUV, 
                    x, y, outW, outH, 320, 240 );
}

/* showbackground( int nMenuID )
 * @nMenuID:
 * 		0: main, 1: text viewer, 2: video player, 3: audio, 4: photo, 5: fm, 6: file manager, 7: setup
 */
void showbackground( int nMenuID )
{
	if( nMenuID < 0 || nMenuID > 10 ) return;
	// main background is prior to user background.

	if( TaviSysParam.gui.isUserBackground ) {
		TAVI_WakeUp(0);
		unsigned char* pY = (unsigned char*)backgroundY;
		unsigned char* pUV = (unsigned char*)backgroundUV;
		YUV420Raw header;
		TAVI_WakeUp(1);
		if( !LoadYUV420Raw( BACKGROUND_PATH, &header, pY, pUV, jpeg_decoding_progress) ) {
			int x=0, y=0;
			printf( "m_nWidth: %d, m_nHeight: %d\n", header.m_nWidth, header.m_nHeight );
			if( header.m_nWidth < BACKGROUND_WIDTH ) 
				x = (BACKGROUND_WIDTH-header.m_nWidth)/2;
			if( header.m_nHeight < BACKGROUND_HEIGHT )
				y = (BACKGROUND_HEIGHT-header.m_nHeight )/2;
			if( x || y ) {
				pMpegDecoder->DisplayYUV420( 0, 0, 0, 0, 0, 0, 0, 0 );
			}
			pMpegDecoder->DisplayYUV420( 
				(RMuint8*)pY, (RMuint8*)pUV, x, y, 
				header.m_nWidth, header.m_nHeight, 
				BACKGROUND_WIDTH, BACKGROUND_HEIGHT );
			return;
		}
	}
	
	// If failed to load user background or isUserBackground is 0.
	// Show default background.
	
	switch( nMenuID ) {
	case ID_BG_MAIN: // main
		showbackground( __TAVI_MAIN_BG );
		break;
	case ID_BG_TEXT: // text
		showbackground( __TEXT_VIEWER_BG );
		break;
	case ID_BG_VIDEO: // video 
		showbackground( __VIDEO_PLAYER_BG );
		break;
	case ID_BG_AUDIO: // audio
		showbackground( __AUDIO_PLAYER_BG );
		break;
	case ID_BG_PHOTO: // photo
		showbackground( __IMAGE_VIEWER_BG );
		break;
	case ID_BG_RADIO:
		showbackground( __FM_RADIO_BG );
		break;
	case ID_BG_FILEMANAGER:
		showbackground( __FILE_MANAGER_BG );
		break;
	case ID_BG_SETUP:
		showbackground( __TAVI_SETUP_BG );
		break;
	case ID_BG_RECORD:
		showbackground( __RECORD_BG );
		break;
	case ID_BG_ETC:
		showbackground( __ETC_BG );
		break;
	case ID_BG_ADJUST_SCREEN:
		showbackground( __ADJUST_SCREEN_BG );
		break;
	}
}


void 
showbackground_romfs (char *bkgnd)
{
	int outW, outH;
	JPEG_CALLBACK_TABLE jpeg_callback_table;
	jpeg_callback_table.context = 0;

	jpeg_callback_table.fclose = (JPEG_fclose)linux_fclose;
	jpeg_callback_table.fopen = (JPEG_fopen)linux_fopen;
	jpeg_callback_table.fread = (JPEG_fread)linux_fread;

	jpeg_callback_table.jpeg_progress = jpeg_decoding_progress;
	if (decode_jpeg_image_420 (bkgnd, (RMuint8 *)scaled_backgroundY, (RMuint8 *)scaled_backgroundUV, 
		852*480, (852*480)/2, &outW, &outH, &jpeg_callback_table) != 0)
	{
		printf("%s failed to decode\n", bkgnd);
		return;
	}
//	pMpegDecoder->SetVideoDestination( 0, 0, 320, 240 );
	int x = outW;
	int y = outH;
	int i, j;
	outW = (outW + 15) & ~0xF;
	outH = (outH + 15) & ~0xF;
	// black out non-multiple of 16
	RMuint8 *p;
	p = (RMuint8 *)scaled_backgroundY;
	for (j=0; j<outH; j++) {
		for (i=0; i<outW; i++) {
			if ((i >= x) || (j >= y))
				*p = 0;
			p++;
		} 
	}
	p = (RMuint8 *)scaled_backgroundUV;
	for (j=0; j<outH; j+=2)	{
		for (i=0; i<outW; i+=2) {
			if ((i >= x) || (j >= y)) {
				*p = 0x80;
				*(p+1) = 0x80;
			}
			p += 2;
		}
	}
	RMint32 w, h, n, d;
	pMpegDecoder->GetScreenDimensions (&w, &h, &n, &d);
	pMpegDecoder->DisplayYUV420((RMuint8 *)scaled_backgroundY, (RMuint8 *)scaled_backgroundUV, 0, 0, outW, outH, outW, outH );
}



RMuint32 MAIN_RenderVideoFile ( uchar_t* filename, int type, void *context )
{
	RMuint32  err;
	RMint32 offset;

DEBUGMAINMSG(1,("MAIN_RenderVideoFile()\n"));

	TAVI_WakeUp(0);

	offset = fs_filenameoffset ();
	pcbtVideo = (VIDEOPLAYER_CALLBACK_TABLE*)context;

	switch( type )
		{
		
		case 3:
			DEBUGMAINMSG(1,("render_avifile() ... \n"));
			if((err=render_avifile ( filename, 3 ))== ERR_VR_NOERROR) 
				{ 
				pcbtVideo->SetSeekable(TRUE);
				pcbtVideo->SetVideoRenderer(3);
				pcbtVideo->SetScreenDisplayRatio();
				return ERR_VR_NOERROR; 
				}
			else if(err==ERR_VR_QPEL)
				{
				pcbtVideo->DrawMsgBox(ERR_VR_QPEL); 
				fb_renderstop(0);
				return err; 				
				}
			else if(err==ERR_VR_GMC)
				{
				if( pcbtVideo->DrawMsgBox(ERR_VR_GMC) )
					{
					fb_renderstop(0);
					IsXvidGmcEnabled = TRUE;
					if(  (err=render_avifile ( filename, 3 )) == ERR_VR_NOERROR )
						{
						pcbtVideo->SetSeekable(TRUE);
						pcbtVideo->SetVideoRenderer(3);
						pcbtVideo->SetScreenDisplayRatio();
						IsXvidGmcEnabled = FALSE;
						return ERR_VR_NOERROR;
						}

					IsXvidGmcEnabled = FALSE;
					return ERR_VR_UNKNOWN;
					}
				else
					{
					fb_renderstop(0);
					return ERR_VR_UNKNOWN; 
					}
 				}			
			else if(err==ERR_VR_FPS_VALUE_INVALID)
				{
				if( pcbtVideo->DrawMsgBox(ERR_VR_FPS_VALUE_INVALID) )
					{
					fb_renderstop(0);
					IsInvalidFPSVideoEnabled= TRUE;
					if(  (err=render_avifile ( filename, 3 )) == ERR_VR_NOERROR )
						{
						pcbtVideo->SetSeekable(FALSE);
						pcbtVideo->SetVideoRenderer(3);
						pcbtVideo->SetScreenDisplayRatio();
						IsInvalidFPSVideoEnabled = FALSE;
						return ERR_VR_NOERROR;
						}

					IsInvalidFPSVideoEnabled = FALSE;
					return ERR_VR_UNKNOWN;
					}
				else
					{
					fb_renderstop(0);
					return ERR_VR_UNKNOWN; 
					}
				}

			else //if(	err==ERR_VR_RES_OVER || err==ERR_VR_FPS_VALUE_INVALID||err==ERR_VR_UNKNOWN )
				{ 
				pcbtVideo->DrawMsgBox(1); 
				fb_renderstop(0);
				return err; 
				}


			break;



		case 4:


			if( pcbtVideo->DrawMsgBox(ERR_VR_MP4V) )
				{
				if(  (err=render_mp4file ( filename, 4 )) == ERR_VR_NOERROR )
					{
					pcbtVideo->SetSeekable(FALSE);
					pcbtVideo->SetVideoRenderer(4);
					pcbtVideo->SetScreenDisplayRatio();
					return ERR_VR_NOERROR;
					}
				else
					{
					pcbtVideo->DrawMsgBox(1); 
					fb_renderstop(0);
					return ERR_VR_UNKNOWN;
					}
				}
			else
				{
				return ERR_VR_UNKNOWN; 
				}

			break;
			
		case 5:
			
			if((err=render_mpegfile ( filename, 5 ))== ERR_VR_NOERROR) 
				{ 
				pcbtVideo->SetSeekable(TRUE);
				pcbtVideo->SetVideoRenderer(5);
				pcbtVideo->SetScreenDisplayRatio();
				//memset( databuffers, 0, sizeof(RMuint32)*(DATA_BUFFER_SIZE/4) );
//			pMpegDecoder->DisplayYUV420(0,0,0,0,0,0,0,0);
				return ERR_VR_NOERROR; 
				}
			else
				{ 
				pcbtVideo->DrawMsgBox(1); 
				return err; 
				}		
			
			break;
			
		}

	ASSERT(0);
	return ERR_VR_UNKNOWN; 
}
	


// function to decode the queued mp3 buffers
// this allows the program to read the avi even though the pcm buffer
// is full (only useful when decoding mp3 in the arm)
void sw_decode_mp3 (void)
{
	void *context = 0;
	if (pMP3BufferDecoder)
	{
		putChunk *chunk;
			int i, x = 0;
			for (i=0; i<BPpcm.nb_buffers; i++)
			{
				if (BPpcm.B[i] == 0)
					x++;
			}

		if (x > 3)
		{
			chunk = mp3chunkQ.DeQueue ();
			if (chunk)
			{
				ASSERT (chunk->buffer);
				my_release (chunk->buffer, context);
				if (pMP3BufferDecoder->DecodeBuffer (chunk->buffer, chunk->length) != MP3_DECODER_ERROR_NO_ERROR)
				{
					// there was a decoding error
					// should we adjust the time stamp to compensate?
				}
			}
		}
	}
}

void InitSHM( void )
{
	int MBshmid;
	
	if((MBshmid= shmget( MB_SHMKEY, MB_DATABUFFER_SIZE+MB_PCMBUFFER_SIZE, MB_SHMPERM|IPC_CREAT|IPC_EXCL)) < 0) {
		DEBUGMAINMSG (1,("FAIL shmget for MB SHARED MEMORY\n") );
	}	   
	// mp3 buffers	
	if((shm_buffer= (RMuint32*)shmat(MBshmid, (char *)0, 0) ) == (RMuint32*)(-1) ) {
		DEBUGMAINMSG (1, ("FAIL shmat for MB SHARED MEMORY\n") );
	}
	else
	{
		// wma buffers
		wma_buffer = (shm_buffer);
		// ogg buffers
		ogg_buffer = (shm_buffer);
		// ac3 buffers
		ac3_buffer = (shm_buffer);
		// decoded mp3/wma/ogg buffers
		pcm_buffer = (shm_buffer + (MAIN_DATA_BUFFER_SIZE)/4);
		// mpeg decoder
		// avi buffers
		avi_buffer = (shm_buffer);
		// mp4 demux
		mp4_buffer = (shm_buffer);
		mpg_buffer = (shm_buffer);
		// for large jpeg memory allocation in libjpeg
		jpeg_buffer = ((RMuint8 *)shm_buffer) + SCALED_Y_LENGTH;
		mp43scan_buffer = backgroundY;
		mp43pending_buffer = mp43scan_buffer + (1024*128/4);
		DEBUGMAINMSG (1, ("MB shared mem =0x%lx\n", shm_buffer) );
	}

}


/////////////////////////////////////////////////////////////////////
// main entry point into the program

#include <sys/time.h>
#include <sys/resource.h> // for setpriority(...)

#include "Task/SubLCD.h"
#include "Driver/misc/misc_ctrl.h"

extern RMuint16 media_flags;

static void tavi_load_mod(char *mod_name, char* arg )
{
	// 2005-09-26
	char buf[256];
	char *prefix = MOD_PATH_PREFIX;
	char *suffix = MOD_NAME_EXT;

	sprintf(buf, "%s%s%s", prefix, mod_name, suffix);
	loadmodule(buf, arg);
}

void checkDebugFlags(void)
{
	NFile *chkFile;

	chkFile = new NFile(DEBUG_BATTERY_REQ, "r");

	g_debug_battery = chkFile->getHandle() > 0;

	chkFile->close();
	delete chkFile;

	chkFile = new NFile(DEBUG_NO_SHUTDOWN_ON_LOW_BATTERY, "r");

	g_debug_no_shutdown_on_low_battery = chkFile->getHandle() > 0;

	chkFile->close();
	delete chkFile;

}
void Decrypt_SerialNo( const char *pOrg, char *pDes, int len)
{
	int i;
	char dmmy;
	char key;
 
	for ( i=0;i<len;i++) {
		key = (i==0?DECRYPTIONKEY:pOrg[i-1]);
		pDes[i]=pOrg[i]^key;
		DEBUGMSG(1,("ComputeEOR:: i=%02d, Des=0x%02x, Org=0x%02x, key=0x%02x\n", i, pDes[i], pOrg[i], key) );
       	}
}

char CheckSum_SerialNo( const char *pData, int len)
{
	int i;
	char checksum;

	checksum=DECRYPTIONKEY;

	for(i=0;i<len;i++) {
		DEBUGMSG(1,("Data[%02d]=%c(0x%02x), checksum=0x%02x\n",i, pData[i], pData[i], checksum));
		checksum ^=pData[i];	
		}
	
	return checksum;
}


int CheckValidationOfSerialNo(void)
{
	FATFS_ERROR	err;
	RMuint32	fd;
	RMuint8 serialno[16];
	RMuint8 databuffer[16];
	RMuint8 readed_checksum;
	RMuint8 computed_checksum;
	RMuint32 n;

	if ( TaviSysParam.ValidSerNo == DECRYPTIONKEY )
		return 0;

	bzero(serialno, sizeof(serialno));
	err = fatfs_fopen((RMuint8 *)_SYSTEMCONFIG, _O_RDONLY, &fd);

	if ( err == FATFS_ERROR_NO_ERROR ) {
		err = fatfs_fread(fd, databuffer, LEN_SERIALNO-1, &n );
		if ( err == FATFS_ERROR_NO_ERROR ) {
			err = fatfs_fread(fd, &readed_checksum, 1, &n );
			if ( err == FATFS_ERROR_NO_ERROR ) {

				computed_checksum = CheckSum_SerialNo( (char*)databuffer, LEN_SERIALNO-1 );
				if ( computed_checksum == readed_checksum ) {
					Decrypt_SerialNo( (char*)databuffer, (char*)serialno, LEN_SERIALNO-1 );
					printf("--- 10th :: CheckValidationOfSerialNo : load serial=[%s]\n", serialno );
					strcpy( TaviSysParam.SerialNo, (char *)serialno );
					TaviSysParam.ValidSerNo = DECRYPTIONKEY;
					SysSaveParameters( &TaviSysParam );
					fatfs_fclose( fd );
					fatfs_delete_file( (RMuint8 *)_SYSTEMCONFIG );
					}
				}
				
			}
		} 
	fatfs_fclose( fd );
	return 0;

}

bool g_AlarmIntr;
bool g_PowerEn;

int main (int argc, char *argv[])
{	
	struct misc_ctrl_struct misc_ctrl;
	struct snd_dev_t dev;
	int fd;

	DEBUGMAINMSG (1, ("TAVI-PMP on PVP main()\n"));

	SubLCD_Init();
	SubLCD_ShowLOGO();
	SubLCD_Flush( 0 );
	
	tavi_load_mod("i2cm", NULL);
	tavi_load_mod("misc_ctrl", NULL);	// EL Backlight
	
	// Initialize Sub-LCD and Show tavi logo.
	// sub-lcd provides 4 planes.
	fd = open( "/dev/misc/tmisc", O_RDONLY );
	ioctl( fd, TAVI_MISC_IOCG_CONTROL, &misc_ctrl );
#ifdef HW0601
	*( volatile int* )( 0x00500A00+0x04 ) = 0x01000100;	// EXT_EL on (PIO1_DATA_b8 = 1)
#else	
	misc_ctrl.ctrl_out |= (1<<EXT_EL_ON);
	ioctl( fd, TAVI_MISC_IOCS_CONTROL, &misc_ctrl );
#endif

#if USE_PBI_DRV
	tavi_load_mod("atapi", NULL);
#else
	tavi_load_mod("ata", NULL);
#endif
    
	// mount the media	
	fs_mountmedia();
	//fs_set_logical_drive (0);
	//fs_set_flags (&media_flags);

	RMuint32 hDir;
	FATFS_ERROR err;
	err = fatfs_dopen( (RMuint8*)"/hd1", &hDir );
	//fatfs_dcreate_dir( hDir, (RMuint8*)"Music" );
	fatfs_dcreate_dir( hDir, (RMuint8*)"Video" );
	//fatfs_dcreate_dir( hDir, (RMuint8*)"Record" );
	fatfs_dcreate_dir( hDir, (RMuint8*)"eBook" );	
	fatfs_dclose( hDir );

	misc_ctrl.brightness = 0;
	ioctl( fd, TAVI_MISC_IOCS_BRIGHTNESS, &misc_ctrl);

	//misc_ctrl.ctrl_out |= (1<<MAIN_LCD_ONOFF);
	//ioctl( fd, TAVI_MISC_IOCS_CONTROL, &misc_ctrl );

	//////////////////////////////////////////////
	// load the required drivers
	tavi_load_mod("khwl", NULL);		// mpeg driver

	//////////////////////////////////////////////
	// create an instance of the mpeg decoder	
	pMpegDecoder = (MpegDecoder *) new MpegDecoder;
	ASSERT (pMpegDecoder);
	pMpegDecoder->Init ();

	//////////////////////////////////////////////
	// set the mpeg decoder to output digital RGB with VSYNCS
	pMpegDecoder->SetupDisplay (toggle_display);

	lcd_enable( misc_ctrl.ctrl_out );

	_PreapareFor_StartupProgressbar();
	
    /*
        Load RTC Driver
    */

    if( TaviSysParam.alarm.useAlarm == 0 ) 	// RTC
        tavi_load_mod( "tavi_rtc", "alarm=1" );	
    else 
        tavi_load_mod( "tavi_rtc", "alarm=0" );

    
	/*
	    Check power key
	*/
	if( !(misc_ctrl.ctrl_in&(1<<POWER_ON)) ) {
	    g_PowerEn = true;
	}

	/*
	    Check Alaram Interrupt
	*/
	if( g_PowerEn ) {
	    int alarmfd;
	    int alarm;
	    alarmfd = open( RTC_DRIVER_PATH, O_RDONLY );
	    if( ioctl( alarmfd, RTC_CHECK_INT, &alarm ) < 0 ) {
	        printf( "failed to RTC_CHECK_INT\n" );
	    }
	    if( alarm ) {
	        g_AlarmIntr = true;
	        ioctl( alarmfd, RTC_CLEAR_INT );
	    }
	    close( alarmfd );
	}

	// HDD 마운트 실패하면 무조건 reboot 표시. 임시코드임.
	if( err != FATFS_ERROR_NO_ERROR ) {
            // Disable alaram
            int alarmfd;
            tavi_load_mod( "tavi_rtc", NULL );
            alarmfd = open( RTC_DRIVER_PATH, O_RDONLY );
            ioctl( alarmfd, RTC_AIE_OFF );
            close( alarmfd );
            
		showbackground_romfs( "/bg/reboot.jpg" );

		misc_ctrl.brightness = 5;
		ioctl( fd, TAVI_MISC_IOCS_BRIGHTNESS, &misc_ctrl);

		for( int i=0; i<8; i++ ) sleep(1);

		misc_ctrl.brightness = 0;
		ioctl( fd, TAVI_MISC_IOCS_BRIGHTNESS, &misc_ctrl);

		sleep(1);

		misc_ctrl.ctrl_out |= (1<<MAIN_LCD_ONOFF);
		ioctl( fd, TAVI_MISC_IOCS_CONTROL, &misc_ctrl );

		ioctl( fd, TAVI_MISC_IOCG_CONTROL, &misc_ctrl );
		if (misc_ctrl.ctrl_in & (1<<AC_DETECT) )
			*((volatile unsigned long*)(0x00500020))=0x80000000;	// reset all
		else
		{
#ifdef HW0601
			*( volatile int* )( 0x00500600+0x04 ) = 0x04000000;	// main Power off  (PIO0_DATA_b10 = 0)
#else
			misc_ctrl.ctrl_out &= ~(1<<POWER_OFF);
			misc_ctrl.ctrl_out |= (1<<POWER_LED);
			ioctl( fd, TAVI_MISC_IOCS_CONTROL, &misc_ctrl );
#endif
		}

	}

	showbackground(_IMAGE_BG"/start-up-4.jpg");

	// Turn-ON TFT-LCD
	misc_ctrl.brightness = 5;
	ioctl( fd, TAVI_MISC_IOCS_BRIGHTNESS, &misc_ctrl);
	//misc_ctrl.ctrl_out &= ~(1<<MAIN_LCD_ONOFF);
	//ioctl( fd, TAVI_MISC_IOCS_CONTROL, &misc_ctrl );
	close( fd );

	//////////////////////////////////////////////
	// load the rest of the kernel modules
	//////////////////////////////////////////////

	tavi_load_mod("tavi_tp", NULL);	// ScrollStrip driver
	tavi_load_mod("tavi_fm", NULL);	// FM radio driver

	DrawStartupProgress(0);

	tavi_load_mod("tavi_key", NULL);	// TAVI keypad driver
	//tavi_load_mod("tavi_midi", NULL);	// hwa sound source driver
	tavi_load_mod("sound", NULL);		// Sound driver


	#if SHM_SCHEME
	InitSHM();	// for shared memory sheme
	#endif

	DrawStartupProgress(1);
	CheckValidationOfSerialNo();

	/////////////////////////////////////////////////////////////////////
	// Create Music player process
	char	*args[] = {"MusicPlayer", TaviSysParam.SerialNo, (char *)0};
	Execute( "/bin/MusicPlayer", args);
	/////////////////////////////////////////////////////////////////////
	
	checkDebugFlags();

	//////////////////////////////////////////////
	// add for launching 'TAVI_MainTask'
	TTaskInitParam tParam ={ "TAVI_MainTask", 0, 0 };


	// Create Message queue for Music Player
	extern int g_qidMusic;
	if( (g_qidMusic = msgget( AUDIO_MSG_KEY, IPC_CREAT|0666 )) < 0 ) {
	    perror( "msgget(AUDIO_MSG_KEY)" );
	}

	// USB Message Queue
	extern int nQkey;
	extern int qidUSB;
	nQkey = USBSTORAGE_MSG_KEY;
	if ( (qidUSB = msgget( nQkey, IPC_CREAT | 0660 )) == -1) {
	    fprintf(stderr, "msgget fail Key=0x%x, qid=0x%x\n", nQkey,qidUSB );
	} else {
	    extern UsbStrgMsgBuf_t* pMsgBuf;
	    DEBUGMSG(1,(" msgget OK Key=0x%x, qid=0x%x\n", nQkey,qidUSB) );
	    if ( (pMsgBuf = (UsbStrgMsgBuf_t *) malloc(sizeof(UsbStrgMsgBuf_t)) )== NULL) {
	        fprintf(stderr, "Msg.cpp : msgget : Out of memory\n");
	    } else {
	        memset(pMsgBuf, 0, sizeof(UsbStrgMsgBuf_t));
	    }
	}
	
	DrawStartupProgress(2);

	TAVI_MainTask( &tParam );

	//////////////////////////////////////////////
	// we should never come to this point
	ASSERT (0);
	pMpegDecoder->Exit ();

	return 0;
}

/* END */

