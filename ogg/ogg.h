#ifndef _OGGDECODER_H
#define _OGGDECODER_H

#include <stdio.h>
#include "ivorbisfile.h"
#include "rmbasic.h"

typedef enum
{
	OGG_DECODER_ERROR_NO_ERROR,
	OGG_DECODER_ERROR_NOT_IMPLEMENTED,
	OGG_DECODER_ERROR_NOT_AN_OGG_FILE,
	OGG_DECODER_ERROR_FILE_DONE,
	OGG_DECODER_ERROR_BAD_OGG_BUFFER,
	OGG_DECODER_ERROR_INVALID_PARAMETER

} OGG_DECODER_ERROR;

typedef struct tagOGG_DECODE_INFO
{
	RMuint32 SamplesPerSecond;
	RMuint32 NumberOfChannels;
	RMuint32 NumberOfBitsPerSample;
	RMuint32 Bitrate;

} OGG_DECODE_INFO;
#define OGG_MSG_OGG_DECODE_INFO		0

// open the ogg file
typedef RMuint32 (*OGG_fopen) (char *filename, void *context);
// read some data from the ogg file
typedef RMuint32 (*OGG_fread) (RMuint32 handle, void *buf, RMuint32 length, void *context);
// seek to the specified position
typedef RMuint32 (*OGG_fseek) (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context);
// must return the current file position
typedef RMuint32 (*OGG_ftell) (RMuint32 handle, void *context);
// close the ogg file
typedef RMuint32 (*OGG_fclose)(RMuint32 handle, void *context);
// increment the reference counter of the buffer
typedef RMuint32 (*OGG_addref) (RMuint8 *pBuffer, void *context);
// decrement the reference counter of the buffer
typedef RMuint32 (*OGG_release) (RMuint8 *pBuffer, void *context);
// the class is reporting some information about the ogg file
typedef RMuint32 (*OGG_info) (RMint32 msg, void *info, void *context);
// the class has decoded an mp3 frame and is requesting the application to render the pcm data
typedef RMuint32 (*OGG_putPCM) (RMuint8 *pBuffer, RMuint32 length, void *context);
// the class is requesting a buffer to put the decoded pcm sample in.  the length of the buffer should be at least 1152*4 bytes
typedef RMuint32 (*OGG_getPCM) (RMuint8 **pBuffer, RMuint32 *plength, void *context);
// the class is requesting a buffer to read the ogg data into
typedef RMuint32 (*OGG_getOGG) (RMuint8 **pBuffer, RMuint32 *plength, void *context);

// definition of the callback table that the application must provide to the class
typedef struct tagOGG_CALLBACK_TABLE
{
	void *context;			// saved context information for application
	OGG_fopen fopen;		// fopen for file decoding
	OGG_fseek fseek;		// seek to the specified position
	OGG_ftell ftell;		// tell the current position
	OGG_fread fread;		// fread for file decoding
	OGG_fclose fclose;		// fclose for file decoding
	OGG_addref addref;		// addref a buffer
	OGG_release release;	// release a buffer
	OGG_info info;			// inform app of some mp3 information
	OGG_putPCM putPCM;		// output PCM bytes
	OGG_getPCM getPCM;		// get a buffer for PCM data
	OGG_getOGG getOGG;		// get a buffer for reading OGG data
} OGG_CALLBACK_TABLE;

class OGGDecoder
{
private:
	RMuint32			m_handle;
	RMuint32			m_OGGvalid;
	OggVorbis_File		m_OGGFile;
	ov_callbacks		m_ovcallbacks;
	OGG_CALLBACK_TABLE	m_CallbackTable;
	RMuint32			m_State;
	RMuint8				*m_pPCMBuffer;
	RMuint32			m_PCMBufferLength;

	RMuint8				*m_pOGGBuffer;
	RMuint32			m_OGGBufferLength;
	RMuint8				*m_pOGGBufferStart;
	RMuint8				*m_pOGGBufferEnd;

	// New Media Life Extension.
	vorbis_comment* m_OGGvc; // Comment descriptor.

public:
	OGGDecoder ();
	~OGGDecoder ();

	// do not call - used by object only
	RMint32 seek (RMint64 pos, RMint32 whence);
	RMint32 read (void *p, RMuint32 len);
	RMint32 tell ();
	RMint32 close ();

	// call these functions after creation
	OGG_DECODER_ERROR Init (void);
	OGG_DECODER_ERROR InitCallbackTable (OGG_CALLBACK_TABLE *pCallbackTable);

	// for file streaming
	// call DecodeFile once
	OGG_DECODER_ERROR DecodeFile (RMint8 *filename);
	// repeatedly call Schedule () to allow the demux/decode to occur
	OGG_DECODER_ERROR Schedule ();
	// seek to a specified position
	OGG_DECODER_ERROR Seek (RMuint32 seconds, RMuint32 *pseconds);

	char* GetArtist( void );
	char* GetAlbum( void );
	char* GetGenre( void );
	char* GetTitle( void );
	int GetTotalTime( void );
	int GetBitrate( void );
	int GetSamplingrate( void );
};

#endif
