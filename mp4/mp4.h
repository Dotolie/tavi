//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : MPEG4 Demux Header
// 
// $Source: /home/cvsroot/TAVI020/tavi/mp4/mp4.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: mp4.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/10/13 01:25:28  etnlwind
// Update for Header Info
//
//*****************************************************************************

#ifndef _MP4_H
#define _MP4_H

#include "rmbasic.h"
#define __ATTRIBUTE_PACKED__	__attribute__((packed))

#define MP4_MAX_TRACKS				32
#define MP4_MAX_AUDIOTRACKS			2
#define MP4_MAX_SUBPICTRACKS		2

#define MP4_FOURCC(ch0, ch1, ch2, ch3)	(((RMuint32)(RMuint8)(ch0) << 24) | ((RMuint32)(RMuint8)(ch1) << 16) | ((RMuint32)(RMuint8)(ch2) << 8) | ((RMuint32)(RMuint8)(ch3) << 0))

#define MP4_FTYP	MP4_FOURCC('f','t','y','p')
#define MP4_ISOM	MP4_FOURCC('i','s','o','m')

#define MP4_MOOV	MP4_FOURCC('m','o','o','v')
#define MP4_MVHD	MP4_FOURCC('m','v','h','d')
#define MP4_TRAK	MP4_FOURCC('t','r','a','k')
#define MP4_TKHD	MP4_FOURCC('t','k','h','d')
#define MP4_TREF	MP4_FOURCC('t','r','e','f')
#define MP4_EDTS	MP4_FOURCC('e','d','t','s')
#define MP4_ELST	MP4_FOURCC('e','l','s','t')
#define MP4_MDIA	MP4_FOURCC('m','d','i','a')
#define MP4_MDHD	MP4_FOURCC('m','d','h','d')
#define MP4_HDLR	MP4_FOURCC('h','d','l','r')
#define MP4_MINF	MP4_FOURCC('m','i','n','f')
#define MP4_NMHD	MP4_FOURCC('n','m','h','d')
#define MP4_VMHD	MP4_FOURCC('v','m','h','d')
#define MP4_SMHD	MP4_FOURCC('s','m','h','d')
#define MP4_HMHD	MP4_FOURCC('h','m','h','d')
#define MP4_DINF	MP4_FOURCC('d','i','n','f')
#define MP4_DREF	MP4_FOURCC('d','r','e','f')
#define MP4_STBL	MP4_FOURCC('s','t','b','l')
#define MP4_STSD	MP4_FOURCC('s','t','s','d')
#define MP4_STTS	MP4_FOURCC('s','t','t','s')
#define MP4_CTTS	MP4_FOURCC('c','t','t','s')
#define MP4_STSC	MP4_FOURCC('s','t','s','c')
#define MP4_STSZ	MP4_FOURCC('s','t','s','z')
#define MP4_STZ2	MP4_FOURCC('s','t','z','2')
#define MP4_STC0	MP4_FOURCC('s','t','c','o')
#define MP4_STSS	MP4_FOURCC('s','t','s','s')
#define MP4_PADB	MP4_FOURCC('p','a','d','b')
#define MP4_STDP	MP4_FOURCC('s','t','d','p')
#define MP4_MVEX	MP4_FOURCC('m','v','e','x')
#define MP4_TREX	MP4_FOURCC('t','r','e','x')
#define MP4_IODS	MP4_FOURCC('i','o','d','s')

#define MP4_MOOF	MP4_FOURCC('m','o','o','f')
#define MP4_MDAT	MP4_FOURCC('m','d','a','t')
#define MP4_FREE	MP4_FOURCC('f','r','e','e')
#define MP4_SKIP	MP4_FOURCC('s','k','i','p')
#define MP4_UDTA	MP4_FOURCC('u','d','t','a')
#define MP4_CHPL	MP4_FOURCC('c','h','p','l')

typedef enum
{
	MP4_ERROR_NO_ERROR,
	MP4_ERROR_FILE_TOO_SHORT,
	MP4_ERROR_FILE_NOT_SUPPORTED,
	MP4_ERROR_FILE_NOT_FOUND,
	MP4_ERROR_NOT_IMPLEMENTED,
	MP4_ERROR_NOT_ALLOWED,
	MP4_ERROR_END_OF_FILE

} MP4_ERROR; 

#define MP4_MSG_TRACK_TIMESCALE				1
typedef struct tagTRACK_TIMESCALE
{
	RMuint32	videoTimeScale;
	RMuint32	audioTimeScale[MP4_MAX_AUDIOTRACKS];
	RMuint32	subpiTimeScale[MP4_MAX_SUBPICTRACKS];
} __ATTRIBUTE_PACKED__ TRACK_TIMESCALE;
#define MP4_MSG_VIDEO_DIMENSIONS			2
typedef struct tagVIDEO_DIMENSIONS
{
	RMuint32	visualWidth;
	RMuint32	visualHeight;
} __ATTRIBUTE_PACKED__ VIDEO_DIMENSIONS;
#define MP4_MSG_SEEK_START					3
#define MP4_MSG_SEEK_PERCENTAGE				4
#define MP4_MSG_SPU_DIMENSIONS				5
typedef struct tagSPU_DIMENSIONS
{
	RMuint32	spWidth;
	RMuint32	spHeight;
} __ATTRIBUTE_PACKED__ SPU_DIMENSIONS;
#define MP4_MSG_READ_ERROR					6
#define MP4_MSG_FLUSH_START					7
#define MP4_MSG_FLUSH_END					8

typedef RMuint32 (*MP4_fopen) (RMuint16 *filename, void *context);
typedef RMuint32 (*MP4_fread) (RMuint32 handle, void *buf, RMuint32 length, void *context);
typedef RMuint32 (*MP4_fseek) (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context);
typedef RMuint32 (*MP4_ftell) (RMuint32 handle, void *context);
typedef RMuint32 (*MP4_fclose)(RMuint32 handle, void *context);
typedef RMuint32 (*MP4_addref) (RMuint8 *pBuffer, void *context);
typedef RMuint32 (*MP4_release) (RMuint8 *pBuffer, void *context);
typedef RMuint32 (*MP4_info) (RMint32 msg, void *info, void *context);
typedef RMuint32 (*MP4_getData) (RMuint8 **pBuffer, RMuint32 *length, void *context);
typedef RMuint32 (*MP4_putDSI) (RMuint32 trackid, RMuint32 flags, RMuint8 *pBuffer, RMuint32 length, RMuint64 time, void *context);
typedef RMuint32 (*MP4_putChunk) (RMuint32 trackid, RMuint32 flags, RMuint64 time, RMuint8 *pBuffer, RMuint32 length, void *context);
typedef RMuint32 (*MP4_loading) (RMuint32 percent, void *context);

typedef struct tagMP4_CALLBACK_TABLE
{
	void *context;			// saved context information for application
	MP4_fopen fopen;		// fopen for file decoding
	MP4_fseek fseek;		// seek to the specified position
	MP4_ftell ftell;		// tell the current position
	MP4_fread fread;		// fread for file decoding
	MP4_fclose fclose;		// fclose for file decoding
	MP4_addref addref;		// addref a buffer
	MP4_release release;	// release a buffer
	MP4_info info;			// inform app of some mp4 information
	MP4_getData getData;	// get a buffer to read data
	MP4_putDSI putDSI;		// output decoder specific information
	MP4_putChunk putChunk;	// output video/audio data
	MP4_loading loading;
} __ATTRIBUTE_PACKED__ MP4_CALLBACK_TABLE;

#define MP4_MIN_READ_LENGTH		(8192)
#define MP4_MAX_VIDEO_DSI		(1024)
#define MP4_MAX_AUDIO_DSI		(128)
#define MP4_MAX_SP_DSI			(64)

class MP4Demux;

class MP4TableHelper
{
	MP4_CALLBACK_TABLE	m_CallbackTable;
	RMuint32			m_handle;
	RMuint32			m_IsInitialized;
	RMuint32			m_entrycount;

	RMuint32			m_StartingOffset;
	RMuint32			m_TableLength;
	RMuint32			m_TableLeft;
	RMuint32			m_CurrentOffset;

	RMuint32			m_valid;
	RMuint32			m_tmpidx;
	RMuint8				m_tmp[MP4_MIN_READ_LENGTH];

	MP4Demux			*m_pMP4Demux;

	RMuint32			reload ();

public:
	MP4TableHelper ();
	~MP4TableHelper ();

	RMuint32 Init (RMuint32 filehandle, RMuint64 offset, RMuint32 length, MP4Demux *pMP4Demux, RMuint32 entrycount);
	RMuint32 InitCallbackTable (MP4_CALLBACK_TABLE *pTable);
	RMuint32 IsInitialized ();
	RMuint32 entrycount ();
	RMuint8  get8 (RMuint32 *error);
	RMuint32 advance8 (RMuint32 *error);
	RMuint32 get32 (RMuint32 *error);
	RMuint32 advance32 (RMuint32 *error);
	RMuint64 get64 (RMuint32 *error);
	RMuint32 advance64 (RMuint32 *error);
	RMuint32 reset ();
	RMuint32 seek (RMuint32 pos);
	RMuint32 tell ();
	RMuint32 rewind32 (RMuint32 *error);
};

#define CHUNK_FLAGS_TIME_VALID		1
#define CHUNK_FLAGS_SAMPLE_START	2
#define CHUNK_FLAGS_SAMPLE_END		4

#define MP4_MAX_CHAPTER_TEXT		64
#define MP4_MAX_CHAPTERS			100

typedef struct tagMP4_CHAPTER
{
	RMint8			name[MP4_MAX_CHAPTER_TEXT];
	RMint64			time;
} __ATTRIBUTE_PACKED__ MP4_CHAPTER;

class MP4Demux
{
	RMuint32	m_handle;
	MP4_CALLBACK_TABLE m_CallbackTable;
	RMuint64	m_boxlength;
	RMuint32	m_boxtype;
	MP4_ERROR	m_error;

	RMuint32	m_ReadLength;
	RMuint32	m_valid;
	RMuint32	m_tmpidx;
	RMuint8		m_tmp[MP4_MIN_READ_LENGTH];

	RMuint8		m_videoDSI[MP4_MAX_VIDEO_DSI];
	RMuint32	m_videoDSILength;
	RMuint8		m_audioDSI[MP4_MAX_AUDIOTRACKS][MP4_MAX_AUDIO_DSI];
	RMuint32	m_audioDSILength[MP4_MAX_AUDIOTRACKS];
	RMint32		m_naudioStreams;

	RMuint8		m_spDSI[MP4_MAX_SUBPICTRACKS][MP4_MAX_SP_DSI];
	RMuint32	m_spDSILength[MP4_MAX_SUBPICTRACKS];
	RMuint32	m_spWidth[MP4_MAX_TRACKS];
	RMuint32	m_spHeight[MP4_MAX_TRACKS];
	RMint32		m_nspStreams;

	RMuint32	m_currentHandlerType;
	RMuint32	m_mdatpos;
	RMuint32	m_mdatlength;

	RMuint32	m_State;
	RMuint32	m_currentOffset;
	RMuint32	m_mdatLeft;
	RMuint8		*m_pData;
	RMuint32	m_DataLength;
	RMuint8		*m_pPutData;
	RMuint32	m_PutDataLength;
	RMuint32	m_DataLeft;

	RMint32		m_trackindex;
	RMuint32	m_trackids[MP4_MAX_TRACKS];
	RMuint32	m_tracktimescales[MP4_MAX_TRACKS];
	RMuint32	m_trackdurations[MP4_MAX_TRACKS];
	RMint32		m_videoTrackIndex;
	RMint32		m_currentAudioTrack;
	RMint32		m_currentSubpiTrack;
	RMint32		m_audioTrackIndex[MP4_MAX_AUDIOTRACKS];
	RMint32		m_subpiTrackIndex[MP4_MAX_SUBPICTRACKS];
	RMuint32	m_mvhdDuration;
	RMuint32	m_mvhdTimeScale;

	RMuint32	m_visualHeight;
	RMuint32	m_visualWidth;

	MP4TableHelper	m_video_stco;	// chunk offset
	RMuint32		m_video_chunk_index;
	RMuint32		m_video_sample_count;
	RMint32			m_video_samples_per_chunk;
	RMuint32		m_video_sample_size;
	MP4TableHelper	m_video_stsz;	// sample size
	MP4TableHelper	m_video_stsc;	// samples per chunk
	MP4TableHelper	m_video_stts;
	RMuint32		m_video_stts_sample_delta_count;
	RMuint32		m_video_stts_sample_delta;
	MP4TableHelper	m_video_ctts;	
	RMuint32		m_video_ctts_sample_delta_count;
	RMuint32		m_video_ctts_sample_delta;
	RMuint64		m_videoTime;
	RMuint32		m_firstVideoChunk;
	MP4TableHelper	m_video_stss;

	MP4TableHelper	m_audio_stco[MP4_MAX_AUDIOTRACKS];
	RMuint32		m_audio_chunk_index[MP4_MAX_AUDIOTRACKS];
	RMuint32		m_audio_sample_count[MP4_MAX_AUDIOTRACKS];
	RMint32			m_audio_samples_per_chunk[MP4_MAX_AUDIOTRACKS];
	RMuint32		m_audio_sample_size[MP4_MAX_AUDIOTRACKS];
	MP4TableHelper	m_audio_stsz[MP4_MAX_AUDIOTRACKS];
	MP4TableHelper	m_audio_stsc[MP4_MAX_AUDIOTRACKS];
	MP4TableHelper	m_audio_stts[MP4_MAX_AUDIOTRACKS];
	RMuint32		m_audio_stts_sample_delta_count[MP4_MAX_AUDIOTRACKS];
	RMuint32		m_audio_stts_sample_delta[MP4_MAX_AUDIOTRACKS];
	MP4TableHelper	m_audio_ctts[MP4_MAX_AUDIOTRACKS];
	RMuint32		m_audio_ctts_sample_delta_count[MP4_MAX_AUDIOTRACKS];
	RMuint32		m_audio_ctts_sample_delta[MP4_MAX_AUDIOTRACKS];
	RMuint64		m_audioTime[MP4_MAX_AUDIOTRACKS];

	MP4TableHelper	m_subpi_stco[MP4_MAX_SUBPICTRACKS];
	RMuint32		m_subpi_chunk_index[MP4_MAX_SUBPICTRACKS];
	RMuint32		m_subpi_sample_count[MP4_MAX_SUBPICTRACKS];
	RMint32			m_subpi_samples_per_chunk[MP4_MAX_SUBPICTRACKS];
	RMuint32		m_subpi_sample_size[MP4_MAX_SUBPICTRACKS];
	MP4TableHelper	m_subpi_stsz[MP4_MAX_SUBPICTRACKS];
	MP4TableHelper	m_subpi_stsc[MP4_MAX_SUBPICTRACKS];
	MP4TableHelper	m_subpi_stts[MP4_MAX_SUBPICTRACKS];
	RMuint32		m_subpi_stts_sample_delta_count[MP4_MAX_SUBPICTRACKS];
	RMuint32		m_subpi_stts_sample_delta[MP4_MAX_SUBPICTRACKS];
	MP4TableHelper	m_subpi_ctts[MP4_MAX_SUBPICTRACKS];
	RMuint32		m_subpi_ctts_sample_delta_count[MP4_MAX_SUBPICTRACKS];
	RMuint32		m_subpi_ctts_sample_delta[MP4_MAX_SUBPICTRACKS];
	RMuint64		m_subpiTime[MP4_MAX_SUBPICTRACKS];

	RMint32			m_current_sp_index;
	RMint32			m_current_au_index;
	RMint32			m_samples_per_chunk;
	RMuint64		m_chunkTime;
	RMuint32		m_chunkFlags;
	RMuint32		m_currentChunkLength;
	RMuint32		m_currentChunkID;
	RMuint32		m_currentID;
	RMuint32		m_total_chunk_size;

	MP4TableHelper	m_chpl_box;
	RMuint32		m_chpl_present;
	RMuint32		m_nchapters;
	MP4_CHAPTER		m_mp4Chapters[MP4_MAX_CHAPTERS];

	RMuint32		m_currentRandomAccessPoint;
	RMint32			m_RandomAccessPointsOnly;
	RMuint8			*m_pRandomAccessData;
	RMuint32		m_RandomLength;
	RMuint32		m_RandomAccessSampleSize;
	RMuint32		m_RandomAccessState;
	RMuint32		m_next_next_chunk;
	RMuint32		m_next_next_frame_no;

	RMuint32	reload ();
	RMuint32	tell ();
	RMuint32	seek (RMuint64 offset);
	RMuint32	seekcur (RMuint64 offset);
	RMuint8		get8 (RMuint32 *error);
	RMuint16	get16 (RMuint32 *error);
	RMuint32	get32 (RMuint32 *error);
	RMuint64	get64 (RMuint32 *error);
			 
	RMuint32	ftyp_box ();
	RMuint32	moov_box ();
	RMuint32	moof_box ();
	RMuint32	mdat_box ();
	RMuint32	free_box ();
	RMuint32	skip_box ();
	RMuint32	udta_box ();

	RMuint32	mvhd_box ();
	RMuint32	trak_box ();
	RMuint32	tkhd_box ();
	RMuint32	tref_box ();
	RMuint32	edts_box ();
	RMuint32	elst_box ();
	RMuint32	mdia_box ();
	RMuint32	mdhd_box ();
	RMuint32	hdlr_box ();
	RMuint32	minf_box ();
	RMuint32	vmhd_box ();
	RMuint32	smhd_box ();
	RMuint32	hmhd_box ();
	RMuint32	dinf_box ();
	RMuint32	dref_box ();
	RMuint32	stbl_box ();
	RMuint32	stsd_box ();
	RMuint32	stts_box ();
	RMuint32	ctts_box ();
	RMuint32	stsc_box ();
	RMuint32	stsz_box ();
	RMuint32	stz2_box ();
	RMuint32	stco_box ();
	RMuint32	stss_box ();
	RMuint32	stsh_box ();
	RMuint32	padb_box ();
	RMuint32	stdp_box ();
	RMuint32	mvex_box ();
	RMuint32	trex_box ();
	RMuint32	iods_box ();
	RMuint32	nmhd_box ();

	RMuint32	chpl_box ();

	RMuint32	NextBox (RMuint64 stop_position);
	RMuint32	RandomAccessPointsOnly ();

	MP4_ERROR	videoSeek (RMuint64 position_in_ms, RMuint32 *fileoffset);
	MP4_ERROR	audioSeek (RMuint64 position_in_ms, RMuint32 *fileoffset, RMuint32 isByteOffset);
	MP4_ERROR	subpiSeek (RMuint64 position_in_ms, RMuint32 *fileoffset, RMuint32 isByteOffset);

public:
	MP4Demux ();
	~MP4Demux ();

	MP4_ERROR	Init ();
	MP4_ERROR	InitCallbackTable (MP4_CALLBACK_TABLE *pTable);
	MP4_ERROR	Demux (RMuint16 *filename);
	MP4_ERROR	Schedule ();
	RMuint64	Duration ();
	MP4_ERROR	Seek (RMint64 position_in_ms);
	MP4_ERROR	Next (RMint64 currentTime);
	MP4_ERROR	Previous (RMint64 currentTime);
	MP4_ERROR	RandomAccessPointsOnly (RMint64 currentTime, RMint32 direction);
	RMint32		GetNumberOfAudioStreams ();
	RMint32		GetNumberOfSubpictureStreams ();
	RMint32		GetAudioDSI (RMint32 streamno, RMuint8 **ppdsi);
	RMint32		GetSubpictureDSI (RMint32 streamno, RMuint8 **ppdsi);
	MP4_ERROR	SetCurrentAudioStream (RMint32 streamno);
	MP4_ERROR	SetCurrentSubpictureStream (RMint32 streamno);
};

#endif
