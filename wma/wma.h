// a class which implements the rendering of a wma file
// wma version 8 or below is supported
// wma version 9 non-professional is supported
#ifndef _WMA_H
#define _WMA_H

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "rmbasic.h"

#ifdef _WIN32
#define WMAAPI_NO_DRM
#include "E:\WMSDK\WMPK\WMADEC\src\scrunch\wmaudio\integer\asfparse_s\wmaudio.h" 
#include "E:\WMSDK\WMPK\WMADEC\src\scrunch\wmaudio\integer\asfparse_s\wmaudio_type.h"		// for tWMAFileHdrStateInternal
#include "E:\WMSDK\WMPK\WMADEC\src\scrunch\wmaudio\integer\decoder\wmadecs_api.h"	// for PLAYOPT_HALFTRANSFORM
#else
#define WMAAPI_NO_DRM
#define _Embedded_x86				// for tWMA_U64 declaration
#include "asf/wmaudio.h" 
#include "asf/wmaudio_type.h"		// for tWMAFileHdrStateInternal
#include "decoder/wmadecs_api.h"	// for PLAYOPT_HALFTRANSFORM
#endif

typedef enum
{
	WMA_DECODER_ERROR_NO_ERROR,
	WMA_DECODER_ERROR_NOT_IMPLEMENTED,
	WMA_DECODER_ERROR_NOT_A_WMA_FILE,
	WMA_DECODER_ERROR_FILE_DONE,
	WMA_DECODER_ERROR_BAD_WMA_BUFFER,
	WMA_DECODER_ERROR_INVALID_PARAMETER

} WMA_DECODER_ERROR;

typedef struct tagWMA_DECODE_INFO
{
	RMuint32 SamplesPerSecond;
	RMuint32 NumberOfChannels;
	RMuint32 NumberOfBitsPerSample;
	RMuint32 Bitrate;

} WMA_DECODE_INFO;
#define WMA_MSG_MP3_DECODE_INFO		0

// open the file
typedef RMuint32 (*WMA_fopen) (char *filename, void *context);
// read some data from the file
typedef RMuint32 (*WMA_fread) (RMuint32 handle, void *buf, RMuint32 length, void *context);
// seek to a position in the file relative to the start of the file
typedef RMuint32 (*WMA_fseek)(RMuint32 handle, RMuint32 offset, RMuint32 whence, void *context);
//close the file
typedef RMuint32 (*WMA_fclose)(RMuint32 handle, void *context);
// increment the reference counter associated with a buffer
typedef RMuint32 (*WMA_addref) (RMuint8 *pBuffer, void *context);
// decrement the reference counter associated with a buffer
typedef RMuint32 (*WMA_release) (RMuint8 *pBuffer, void *context);
// the class informs the application of some information
typedef RMuint32 (*WMA_info) (RMint32 msg, void *info, void *context);
// the class is requesting the application to render some pcm data
typedef RMuint32 (*WMA_putPCM) (RMuint8 *pBuffer, RMuint32 length, void *context);
// the class is requesting the application to provide a buffer for the pcm data
typedef RMuint32 (*WMA_getPCM) (RMuint8 **pBuffer, RMuint32 *plength, void *context);
// the class is requesting the application to provide a buffer for the wma data 
typedef RMuint32 (*WMA_getWMA) (RMuint8 **pBuffer, RMuint32 *plength, void *context);

typedef struct tagWMA_CALLBACK_TABLE
{
	void *context;			// saved context information for application
	WMA_fopen fopen;		// fopen for file decoding
	WMA_fseek fseek;		// fseek for file decoding
	WMA_fread fread;		// fread for file decoding
	
	WMA_fclose fclose;		// fclose for file decoding
	WMA_addref addref;		// addref a buffer
	WMA_release release;	// release a buffer
	WMA_info info;			// inform app of some mp3 information
	WMA_putPCM putPCM;		// output PCM bytes
	WMA_getPCM getPCM;		// get a buffer for PCM data
	WMA_getWMA getWMA;		// get a buffer for reading WMA data
} WMA_CALLBACK_TABLE;

// the class definition
class WMADecoder
{
	WMA_CALLBACK_TABLE	m_CallbackTable;
	RMint32				m_handle;
	RMuint32			m_State;
	RMuint32			m_TotalBytesProcessed;

	tWMAFileHdrState	m_sFileHeaderState;
	tWMAFileHeader		m_sFileHeader;
	tHWMAFileState		m_sState;
	tWMA_U32			m_iSampleCount;
	tWMA_U32			m_iSamples;
	RMuint8				*m_pcmbuffer;

	RMuint8				*m_pWMA;
	RMuint32			m_wmaLength;
	RMuint32			m_wmaOffset;
	RMuint32			m_wmaValid;
	//RMuint8				m_wmatmpbuf[256];
	RMuint8				*m_wmatmp;
	tWMAFileContDesc* m_pContent;
	tWMAExtendedContentDesc* m_pExtContent;

public:
	WMADecoder ();
	~WMADecoder ();

	// call Init () after object creation
	WMA_DECODER_ERROR Init (void);
	// call InitCallbackTable () after Init () to setup the required callbacks
	WMA_DECODER_ERROR InitCallbackTable (WMA_CALLBACK_TABLE *pCallbackTable);

	// for file streaming
	// call DecodeFile () once to start the decoding process
	WMA_DECODER_ERROR DecodeFile (RMint8 *filename);
	// repeatedly call Schedule () for the decoding process to continue
	WMA_DECODER_ERROR Schedule ();
	// seek to specified position
	// returns the new actual position
	WMA_DECODER_ERROR Seek (RMuint32 seconds, RMuint32 *pactual);

	// for buffer streaming
	// currently not supported
	WMA_DECODER_ERROR DecodeBuffer (RMuint8 *pBuffer, RMuint32 length);

	// for wma library
	RMuint32 WMAFileCBGetData (tHWMAFileState state, RMuint32 offset, RMuint32 num_bytes, RMuint8 **ppData);
	int GetTotalTime( void );
	int GetBitrate( void );
	int GetSamplingrate( void );
	char* GetArtist( int* len );
	char* GetAlbum( int* len );
	char* GetGenre( int* len );
	char* GetTitle( int* len );
};

#endif
