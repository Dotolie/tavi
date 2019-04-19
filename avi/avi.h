//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : AVI(DivX, XviD) Demux Header
// 
// $Source: /home/cvsroot/TAVI020/tavi/avi/avi.h,v $
// $Revision: 1.3 $
// $Date: 2006/04/07 04:29:37 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: avi.h,v $
// Revision 1.3  2006/04/07 04:29:37  zstein
// - added GetVideoCompression( void ) and
//   released over 30fps error.
//
// Revision 1.2  2006/04/05 02:07:36  zstein
// - added ReInitCallback(...)
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.13  2006/01/06 09:08:59  etnlwind
// Update for Broken Video File
//
// Revision 1.12  2005/11/03 03:14:44  etnlwind
// AVI 비디오 재생시 프레임레이트 정보가 잘못되어 있는지 검사해보고
// 잘못되어 있을경우 대화상자로 사용자에게 현상황을 알리고 재생여부를 확인
// 사용자가 재생을 시도하면, Seek기능을 막고 재생하도록 수정
// (프레임레이트 정보가 깨진 비디오 파일은 주로 Seek기능에는
// 문제가 생기고, 가끔씩 재생중 멈추거나 끊어지는 경우도 발생함)
//
// 비디오 재생중 TASK버튼 누를경우 배경을 지우지 않고 메인으로 가는경우 수정
//
// 비디오 디렉토리 반복 수정
// 비디오 재생시간 잔상제거
// 비디오 전체시간 위치 수정
// 비디오 재생중 볼륨조절시 비디오재생이 좀더 매끄럽게 수정
//
// Revision 1.11  2005/11/01 04:25:00  etnlwind
// Update for DX50 Invalid Header
//
// Revision 1.10  2005/10/13 01:25:24  etnlwind
// Update for Header Info
//
//*****************************************************************************


// this is an implementation of a class the demuxes AVI files
#ifndef _AVI_H
#define _AVI_H

#include "rmbasic.h"

// *** if your CPU has data alignment requirements, you should access 
//     any field in a packed structure using the pointer to the structure.
//     this ensures the compiler will do the correct dereference for you.

#define AVI_PACKED_STRUCTURE_ACCESS(x,y)	((&(x))->y)

#ifdef _WIN32
#define __ATTRIBUTE_PACKED__
#else
#define __ATTRIBUTE_PACKED__	__attribute__((packed))
#endif

#define AVI_MAX_STREAMS_SUPPORTED		32

#define AVI_FOURCC(ch0, ch1, ch2, ch3)	((RMuint32)(RMuint8)(ch0) | ((RMuint32)(RMuint8)(ch1) << 8) | ((RMuint32)(RMuint8)(ch2) << 16) | ((RMuint32)(RMuint8)(ch3) << 24 ))

typedef struct tagAVI_MAIN_HEADER
{
	RMuint32 MicroSecPerFrame;
	RMuint32 MaxBytesPerSec;
	RMuint32 Reserved1;
	RMuint32 Flags;
	RMuint32 TotalFrames;
	RMuint32 InitialFrames;
	RMuint32 Streams;
	RMuint32 SuggestedBufferSize;
	RMuint32 Width;
	RMuint32 Height;
	RMuint32 Scale;
	RMuint32 Rate;
	RMuint32 Start;
	RMuint32 Length;
	
} __ATTRIBUTE_PACKED__ AVI_MAIN_HEADER;

typedef struct tagAVI_STREAM_HEADER
{
	RMuint32 fccType;
	RMuint32 fccHandler;
	RMuint32 Flags;
	RMuint32 Reserved1;
	RMuint32 InitialFrames;
	RMuint32 Scale;
	RMuint32 Rate;
	RMuint32 Start;
	RMuint32 Length;
	RMuint32 SuggestedBufferSize;
	RMuint32 Quality;
	RMuint32 SampleSize;

} __ATTRIBUTE_PACKED__ AVI_STREAM_HEADER;

typedef struct tagAVI_PCMWAVEFORMAT
{
    RMuint16 wFormatTag;        // format type
    RMuint16 nChannels;         // number of channels (i.e. mono, stereo, etc.)
    RMuint32 nSamplesPerSec;    // sample rate
    RMuint32 nAvgBytesPerSec;   // for buffer estimation
    RMuint16 nBlockAlign;       // block size of data
	RMuint16 wBitsPerSample;
} __ATTRIBUTE_PACKED__ AVI_PCMWAVEFORMAT; 

typedef struct tagAVI_WAVEFORMATEX
{
    RMuint16 wFormatTag;         // format type
    RMuint16 nChannels;          // number of channels (i.e. mono, stereo...)
    RMuint32 nSamplesPerSec;     // sample rate
    RMuint32 nAvgBytesPerSec;    // for buffer estimation
    RMuint16 nBlockAlign;        // block size of data
    RMuint16 wBitsPerSample;     // number of bits per sample of mono data
} __ATTRIBUTE_PACKED__ AVI_WAVEFORMATEX; 

typedef struct tagAVI_AVI_BITMAPINFO
{
    RMuint32 Size;
    RMint32 Width;
    RMint32 Height;
    RMuint16 Planes;
    RMuint16 BitCount;
    RMuint32 Compression;
    RMuint32 SizeImage;
    RMint32 XPelsPerMeter;
    RMint32	YPelsPerMeter;
    RMuint32 ClrUsed;
    RMuint32 ClrImportant;

} __ATTRIBUTE_PACKED__ AVI_BITMAPINFO; 

typedef struct tagAVI_STREAM_FORMAT
{
	union
	{
		AVI_WAVEFORMATEX	wfx;
		AVI_BITMAPINFO		bmi;
	};
} __ATTRIBUTE_PACKED__ AVI_STREAM_FORMAT;

typedef enum
{
	AVI_DEMUX_ERROR_NO_ERROR,
	AVI_DEMUX_ERROR_NOT_IMPLEMENTED,
	AVI_DEMUX_ERROR_NO_BUFFER_AVAILABLE,
	AVI_DEMUX_ERROR_NOT_AN_AVI_FILE,
	AVI_DEMUX_ERROR_FILE_DONE,
	AVI_DEMUX_ERROR_INVALID_PARAMETER,
	AVI_DEMUX_ERROR_NO_KEYFRAME,
	AVI_DEMUX_ERROR_FPS_VALUE_INVALID,
	AVI_DEMUX_ERROR_INVALID_CHUNK_NAME
} AVI_DEMUX_ERROR;

#define AVI_DEMUX_MSG_AVIHDR		1
#define AVI_DEMUX_MSG_STREAMHDR		2
#define AVI_DEMUX_MSG_BITMAPINFO	3
#define AVI_DEMUX_MSG_WAVEFORMATEX	4
#define AVI_DEMUX_KEYFRAME_POSITION	5
#define AVI_DEMUX_MSG_INDEX_CHECK	6

// open a file for reading
typedef RMuint32 (*AVI_fopen) (RMuint16*filename, void *context);
// file read
typedef RMuint32 (*AVI_fread) (RMuint32 handle, void *buf, RMuint32 length, void *context);
// file seek - offset is from start of file
typedef RMuint32 (*AVI_fseek)(RMuint32 handle, RMuint32 offset, void *context);
// must return the current file position
typedef RMuint32 (*AVI_ftell) (RMuint32 handle, void *context);
// close an opened file
typedef RMuint32 (*AVI_fclose)(RMuint32 handle, void *context);
// add reference to a buffer
typedef RMuint32 (*AVI_addref) (RMuint8 *pBuffer, void *context);
// decrement reference to a buffer
typedef RMuint32 (*AVI_release) (RMuint8 *pBuffer, void *context);
// provide some information about the current avi file
typedef RMuint32 (*AVI_info) (RMint32 msg, void *info, void *context);
// get a buffer
// this buffer will be used to read the avi file into
typedef RMuint32 (*AVI_getbuffer) (RMuint8 **pBuffer, RMuint32 *plength, void *context);
// called by the demux object when any data chunk is encountered during demux
// the buffer must be consumed immediately
typedef RMuint32 (*AVI_putChunk) (RMuint8 *chunkid, RMuint8 *buffer, RMuint32 buflen, RMuint32 flags, void *context);
// called by the demux during the initial loading of the file (during ::DemuxFile)
// this can allow the application to display some message to the user to
// indicate he/she must wait a while
typedef RMuint32 (*AVI_loading) (RMuint32 percent, void *context);
// return the time in milliseconds
typedef RMuint32 (*AVI_gettimems) (void *context);

typedef struct tagAVI_CALLBACK_TABLE
{
	void			*context;
	AVI_fopen		fopen;
	AVI_fread		fread;
	AVI_ftell		ftell;
	AVI_fseek		fseek;
	AVI_fclose		fclose;
	AVI_addref		addref;
	AVI_release		release;
	AVI_info		info;
	AVI_getbuffer	getbuffer;
	AVI_putChunk	putChunk;
	AVI_loading		loading;
	AVI_gettimems	gettimems;

} __ATTRIBUTE_PACKED__ AVI_CALLBACK_TABLE;

typedef struct tagAVI_CHUNK
{
	RMuint32 Name;
	RMuint32 Length;
} __ATTRIBUTE_PACKED__ AVI_CHUNK;

#define AVI_MAX_INDEX_HELPER	(1024)
typedef struct tagAVIIndexHelper
{
	RMuint32 indexOffset;
	RMuint32 chunkOffset;
	RMuint32 videoFrameCount;
	RMuint32 audioByteOrFrameCount[AVI_MAX_STREAMS_SUPPORTED];
} __ATTRIBUTE_PACKED__ AVIIndexHelper;

#define AVI_FLAGS_LIST			0x00000001
#define AVI_FLAGS_KEYFRAME		0x00000010
#define AVI_FLAGS_NOTIME	    0x00000100

typedef struct tagAVI_INDEXENTRY
{
	RMuint32	ckid;
	RMuint32	dwFlags;
	RMuint32	dwChunkOffset;
	RMuint32	dwChunkLength;

}  __ATTRIBUTE_PACKED__ AVI_INDEXENTRY;

#define MPEG4_MAX_VOLHDR_SIZE	1024	// max length for a vol header?
#define AVI_INDEX_CACHE_SIZE	1024

class AVIDemux
{
	RMuint32			m_State;
	RMuint32			m_handle;
	AVI_CALLBACK_TABLE	m_CallbackTable;

	RMuint32			m_RIFFlength;
	AVI_MAIN_HEADER		m_avih;
	RMuint32			m_moviOffset;
	RMuint32			m_moviLength;
	RMuint32			m_idx1Offset;
	RMuint32			m_idx1Length;
	AVI_STREAM_HEADER	m_StreamHeaders[AVI_MAX_STREAMS_SUPPORTED];
	RMuint32			m_nStreamHeader;
	AVI_STREAM_FORMAT	m_StreamFormats[AVI_MAX_STREAMS_SUPPORTED];	
	RMuint32			m_nStreamFormat;
	RMint32				m_vbrmp3[AVI_MAX_STREAMS_SUPPORTED];
	RMuint32			m_nIndexHelper;
	AVIIndexHelper		m_IndexHelper[AVI_MAX_INDEX_HELPER];
	RMuint8				m_videoVOLHdr[MPEG4_MAX_VOLHDR_SIZE];

	RMuint8				m_chunkid[4];
	RMuint32			m_chunklength;
	RMuint32			m_demuxstate;
	RMuint32			m_sizeOfFile;
	RMuint32			m_chunkstart;
	RMuint32			m_bytecounter;
	RMuint32			m_currentFramePosition;

	AVI_INDEXENTRY		m_IndexCache[AVI_INDEX_CACHE_SIZE];
	RMuint32			m_IndexCacheOffset;
	RMint32				m_IndexCacheValid;
	RMint32				m_KeyFramesOnly;
	RMint32				m_CurrentIndex;
	RMuint32			m_KeyFrameOnlyState;
	RMuint32			m_KeyFrameFlags;
	RMuint32			m_KeyFrameOffset;
	RMuint32			m_KeyFrameLength;
	RMint32				m_KeyFrameDelayInMS;
	RMuint32			m_KeyFrameDelay_t0;
	RMuint8				*m_savedp;
	RMuint8				*m_savedbuf;
	RMuint32			m_savedbuflen;
	RMuint32			m_savedstate;

	RMint32			m_IndexIsRelativeToStartOfFile;
	RMuint32			m_IsValidScaleAndRate_OnAviMainHeader;

	RMuint32			CheckChunkId ();
	AVI_DEMUX_ERROR		ScheduleKeyFramesOnly ();



public:
	AVIDemux ();
	~AVIDemux ();

	RMuint32 SuggestedScale;
	RMuint32 SuggestedRate;
	RMuint32 m_IsInvalidFPSVideoEnabled;

	// here is the external API for the avi demux object	

	// call this first after object creation
	AVI_DEMUX_ERROR Init ();
	// call this next, and initialize the callback table	
	AVI_DEMUX_ERROR InitCallbackTable (AVI_CALLBACK_TABLE *pCallbackTable);
	// call this next to start the demux of the requested file
	// this function may take some time because it will also process
	// the index chunk
	AVI_DEMUX_ERROR DemuxFile_V19 (RMuint16 *filename); 	// Version 1.9
	AVI_DEMUX_ERROR DemuxFile (RMuint16 *filename);
	// call this function repeatly to allow the demux to run
	AVI_DEMUX_ERROR Schedule ();
	// call this function if you want to seek in the file	
	// [IN]  audiostreamno - the audio stream that is currently selected
	// [OUT] paudioposition - the audio position in bytes OR frames
	// [OUT] paudiobyte - 1 if paudioposition is in bytes, 0 if paudioposition is in frames
	//                  - used when seeking in variable bitrate mp3 files
	AVI_DEMUX_ERROR Seek (RMuint32 seconds, RMuint32 audiostreamno, RMuint32 *pvideoposition, RMuint32 *paudioposition, RMuint32 *paudiobyte);
	// call this function to tell the demux to send only key frames
	AVI_DEMUX_ERROR KeyFramesOnly (RMint32 direction);
	// get a specific frame - only frame 0 is implemented
	AVI_DEMUX_ERROR GetFrame (RMuint32 frameno, RMuint8 *buffer, RMuint32 *length);
	// set the delay between sending key frames in milliseconds
	AVI_DEMUX_ERROR SetKeyFrameDelay (RMuint32 delay_ms);
	// check if the requested stream is variable bitrate mp3
	AVI_DEMUX_ERROR IsStreamNumberVBR (RMuint32 audiostreamno, RMuint32 *is_vbr);


    AVI_DEMUX_ERROR AVIDemux::ReInitCallbackTable( AVI_CALLBACK_TABLE *pCallback );
    int GetFileSize( void ) const { return m_sizeOfFile; }
    int AVIDemux::GetVideoCompression( void ) const;

};

#define AVI_FLAG_CHUNK_START	1
#define AVI_FLAG_CHUNK_END		2
#endif
