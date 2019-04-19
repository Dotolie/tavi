//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : MPEG1, MPEG2 Demux Header
// 
// $Source: /home/cvsroot/TAVI020/tavi/mpgparse/mpgparse.h,v $
// $Revision: 1.2 $
// $Date: 2006/04/05 02:11:40 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: mpgparse.h,v $
// Revision 1.2  2006/04/05 02:11:40  zstein
// - added ReInitCallbackTable(...).
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.9  2005/12/22 05:22:09  etnlwind
// Update for New Version
//
// Revision 1.8  2005/10/13 01:25:32  etnlwind
// Update for Header Info
//
//*****************************************************************************

#ifndef _MPGPARSE_H
#define _MPGPARSE_H

#include "rmbasic.h"
#include "m1s.h"
#include "pes.h"
#include "m2t.h"
#define __ATTRIBUTE_PACKED__	__attribute__((packed))

#define MSG_MSG_MPEG12_ELEMENTARY		0
#define MSG_MSG_MPEG4_ELEMENTARY		1
typedef struct tagAC3DTS_INFO
{
	RMuint8		nFrameHeaders;
	RMuint16	FirstAccessUnitPointer;
} __ATTRIBUTE_PACKED__ AC3DTS_INFO;
#define MSG_MSG_AC3DTS_INFO				2
typedef struct tagLPCM_INFO
{
	RMuint8		nFrameHeaders;
	RMuint16	FirstAccessUnitPointer;
	RMuint32	Frequency;
	RMuint8		NumberOfChannels;
	RMuint8		QuantizationWordLength;
} __ATTRIBUTE_PACKED__ LPCM_INFO;
#define MSG_MSG_LPCM_INFO				3

typedef struct _t_SEQ_LAYER
{
	int HorizontalSize;
	int VerticalSize;
	int PelAspectRatio;
	int PictureRate;
	int BitRate;
	int VBVBufferSize;
} __ATTRIBUTE_PACKED__ SEQ_LAYER;

typedef enum
{
	MPG_DEMUX_ERROR_NO_ERROR,
	MPG_DEMUX_ERROR_NOT_IMPLEMENTED,
	MPG_DEMUX_ERROR_NOT_AN_MPEG_FILE,
	MPG_DEMUX_ERROR_FILE_DONE,
	MPG_DEMUX_ERROR_SEEK_FAILED,

} MPG_DEMUX_ERROR;

#define MPG_LOOKING_FOR_SEQUENCE_HEADER			1	 
#define MPG_LOOKING_FOR_1ST_PICTURE_HEADER		2
#define MPG_LOOKING_FOR_2ND_PICTURE_HEADER		3
#define MPG_FOUND_2ND_PICTURE_HEADER			4
#define MPG_ERROR_SAME_IFRAME_DURING_REVERSE	5

#define PROGRAM_DEMUX	1
#define TRANSPORT_DEMUX	2
#define DVD_DEMUX		3
#define SYSTEM_DEMUX	4
#define DAT_DEMUX		5
#define NO_DEMUX		6

// open the mpg file
typedef RMuint32 (*MPG_fopen) (RMuint16 *filename, void *context);
// read some data from the mpg file
typedef RMuint32 (*MPG_fread) (RMuint32 handle, void *buf, RMuint32 length, void *context);
// seek to some position from the mpg file
typedef RMuint32 (*MPG_fseek) (RMuint32 handle, RMint32 pos, RMint32 whence, void *context);

typedef RMuint32 (*MPG_ftell) (RMuint32 handle, void * context);
// close the mpg file
typedef RMuint32 (*MPG_fclose)(RMuint32 handle, void *context);
// increment the reference counter of the buffer
typedef RMuint32 (*MPG_addref) (RMuint8 *pBuffer, void *context);
// decrement the reference counter of the buffer
typedef RMuint32 (*MPG_release) (RMuint8 *pBuffer, void *context);
// the class is reporting some information about the mpe file
typedef RMuint32 (*MPG_info) (RMint32 msg, void *info, void *context);
// the class is requesting the application to render some payload data
typedef RMuint32 (*MPG_putpayload) (RMuint8 *pData, RMuint32 Length, RMuint8 StreamId, RMuint8 SubStreamId,
	RMint64 Scr, RMint64 Pts, RMint64 Dts, RMuint8 Flags, void *context);
// the class is requesting a buffer to read the mpg data into
typedef RMuint32 (*MPG_getMPG) (RMuint8 **pBuffer, RMuint32 *plength, void *context);
// the class is requesting a buffer for file detection - 16k is required
// this buffer needs to only be valid during the context of MPEGDemux::DecodeFile
typedef RMuint32 (*MPG_getDetectionBuffer) (RMuint8 **pBuffer, RMuint32 *plength, void *context);

typedef RMuint32 (*MPG_loading) (RMuint32 percent, void *context);

// definition of the callback table that the application must provide to the class
typedef struct tagMPG_CALLBACK_TABLE
{
	void *context;						// saved context information for application
	MPG_fopen fopen;					// fopen for file decoding
	MPG_fread fread;					// fread for file decoding
	MPG_fseek fseek;					// fseek for file decoding
	MPG_ftell ftell;
	MPG_fclose fclose;					// fclose for file decoding
	MPG_addref addref;					// addref a buffer
	MPG_release release;				// release a buffer
	MPG_info info;						// inform app of some mp3 information
	MPG_putpayload putpayload;			// write some payload data to the application
	MPG_getMPG getMPG;					// request a buffer to read the mpeg data into
	MPG_getDetectionBuffer getDetectionBuffer;	// request a buffer used to do file detection
	MPG_loading loading;

} __ATTRIBUTE_PACKED__ MPG_CALLBACK_TABLE;

class MPEGDemux
{
	MPG_CALLBACK_TABLE	m_CallbackTable;
	SEQ_LAYER			m_SEQ;

	// no support for transport streams
	MPEG1Demux			m_MPEG1Demux;
	PESDemux			m_ProgramDemux;
	RMint32				m_DemuxType;
	RMuint32				m_file;
	RMuint32				m_mpeg12;

	// required by I frame parser
	RMint32				m_ReverseErrorLoop;
	RMint64				m_StartPosition;
	RMint64				m_CurrentPosition;
	RMint64				m_IFramePosition;
	RMint32				m_BytesToSkip;
	RMuint32				m_IFrameState;
	RMuint32				m_IFrameSubState;
	RMint64				m_FileSize;
	RMuint32				m_LoadingProgress;

	RMint32 IsTransportStream (RMuint16 *filename);
	RMint32 IsProgramStream (RMuint16 *filename);
	RMint32 IsMPEG1SystemStream (RMuint16 *filename);
	RMint32 IsDVDStream (RMuint16 *filename);
	RMint32 IsVCDStream (RMuint16 *filename);
	RMint32 IsMPEG124ElementaryStream (RMuint16 *filename, RMuint32 *pmpeg12);
	RMint32 reformat_vcdsectors (RMuint8 *p, RMint32 l);
	RMint32	demux (RMuint8 *p, RMuint32 l);
	RMint32 iframe_parser (RMuint8 *p, RMint32 len, RMuint32 *sendit);

public:
	MPEGDemux ();
	~MPEGDemux ();

	// call this after creation
	MPG_DEMUX_ERROR Init ();
	// call this to initalize the required callbacks
	MPG_DEMUX_ERROR InitCallbackTable (MPG_CALLBACK_TABLE *pCallbackTable);
    MPG_DEMUX_ERROR ReInitCallbackTable( MPG_CALLBACK_TABLE* pCallbackTable );
	// call this to identify the file type
	MPG_DEMUX_ERROR DecodeFile (RMuint16 *filename);
	// call this repeatly to do work
	MPG_DEMUX_ERROR Schedule ();
	// call this to send only I frames (forward or reverse)
	// BytesToSkip = positive for forward scan
	//             = negative for backward scan
	//             = 0 for resume back to normal mode	
	MPG_DEMUX_ERROR SendIFramesOnly (RMint32 BytesToSkip);
	RMint64 mpg_info( RMuint16* filename );
	MPG_DEMUX_ERROR Seek (RMuint32 t);
	// call this to get the detected file type
	RMuint32 GetDemuxType (void);
	// call this _after_ to assume all program files follow dvd standard for packets
	RMuint32 AssumeDVDPacketStructure (RMint32 followdvd);
	
	// not part of the api - do not call or use these functions
	void MPEG1DemuxCallback (RMuint8 *pData, RMuint32 Length, RMuint8 PacketId, 
		RMint64 Scr, RMint64 Pts, RMint64 Dts, RMuint8 Flags, RMint64 offset);
	void PESDemuxCallback (RMuint8 *pData, RMuint32 Length, RMuint8 StreamId, RMuint8 SubStreamId, 
		RMint64 Scr, RMint64 Pts, RMint64 Dts, RMuint8 Flags, RMint64 offset);
	void AC3DTSInfoCallback (RMuint8 numberOfFrameHeaders, RMuint16 firstAccessUnitPointer);
	void LPCMInfoCallback (RMuint8 numberOfFrameHeaders, RMuint16 firstAccessUnitPointer, RMuint32 frequency, 
		RMuint8 numberOfChannels, RMuint8 quantizationWordLength);
    int GetVideoBitrate( void ) const { return m_SEQ.BitRate; }
};

#endif
