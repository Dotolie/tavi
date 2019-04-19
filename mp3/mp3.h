// this is a class that can decode an mp3 file or stream
#ifndef _MP3_H
#define _MP3_H

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "rmbasic.h"
#define __ATTRIBUTE_PACKED__	__attribute__((packed))

#ifdef _WIN32
extern "C"
{
#define FPM_INTEL	1
#include "E:\Caribbean\developers_quick_tests\dvdtest\win32\madlib\frame.h"
#include "E:\Caribbean\developers_quick_tests\dvdtest\win32\madlib\synth.h"
#include "z:\em85xx.pvp\libraries\libEqualizer\equalizer.h"
}
#else
#include "mad.h"
//#include "equalizer.h"
#endif

typedef enum
{
	MP3_DECODER_ERROR_NO_ERROR,
	MP3_DECODER_ERROR_NOT_IMPLEMENTED,
	MP3_DECODER_ERROR_NOT_A_MP3_FILE,
	MP3_DECODER_ERROR_FILE_DONE,
	MP3_DECODER_ERROR_BAD_MP3_BUFFER,
	MP3_DECODER_ERROR_INVALID_PARAMETER,

} MP3_DECODER_ERROR;

typedef struct tagMP3_DECODE_INFO
{
	RMuint32 SamplesPerSecond;
	RMuint32 NumberOfChannels;
	RMuint32 NumberOfBitsPerSample;
	RMuint32 Bitrate;

} __ATTRIBUTE_PACKED__ MP3_DECODE_INFO;
#define MP3_MSG_MP3_DECODE_INFO		0

// open the mp3 file
typedef RMuint32 (*MP3_fopen) (char *filename, void *context);
// read some data from the mp3 file
typedef RMuint32 (*MP3_fread) (RMuint32 handle, void *buf, RMuint32 length, void *context);
// seek to a specified position
typedef RMuint32 (*MP3_seek) (RMuint32 handle, RMuint32 pos, RMuint32 whence, void *context);
// return the current file position
typedef RMuint32 (*MP3_ftell) (RMuint32 handle, void *context);
// close the mp3 file
typedef RMuint32 (*MP3_fclose)(RMuint32 handle, void *context);
// increment the reference counter of the buffer
typedef RMuint32 (*MP3_addref) (RMuint8 *pBuffer, void *context);
// decrement the reference counter of the buffer
typedef RMuint32 (*MP3_release) (RMuint8 *pBuffer, void *context);
// the class is reporting some information about the mpe file
typedef RMuint32 (*MP3_info) (RMint32 msg, void *info, void *context);
// the class has decoded an mp3 frame and is requesting the application to render the pcm data
typedef RMuint32 (*MP3_putPCM) (RMuint8 *pBuffer, RMuint32 length, void *context);
// the class is requesting a buffer to put the decoded pcm sample in.  the length of the buffer should be at least 1152*4 bytes
typedef RMuint32 (*MP3_getPCM) (RMuint8 **pBuffer, RMuint32 *plength, void *context);
// the class is requesting a buffer to read the mp3 data into
typedef RMuint32 (*MP3_getMP3) (RMuint8 **pBuffer, RMuint32 *plength, void *context);
// talk reaching eof
typedef RMuint32 (*MP3_reachEof) ( void );		// add for power save

// definition of the callback table that the application must provide to the class
typedef struct tagMP3_CALLBACK_TABLE
{
	void *context;			// saved context information for application
	MP3_fopen fopen;		// fopen for file decoding
	MP3_seek fseek;			// seek to a position
	MP3_ftell ftell;		// return the current file position
	MP3_fread fread;		// fread for file decoding
	MP3_fclose fclose;		// fclose for file decoding
	MP3_addref addref;		// addref a buffer
	MP3_release release;	// release a buffer
	MP3_info info;			// inform app of some mp3 information
	MP3_putPCM putPCM;		// output PCM bytes
	MP3_getPCM getPCM;		// get a buffer for PCM data
	MP3_getMP3 getMP3;		// get a buffer for reading MP3 data
	MP3_reachEof reachEof;	// talk reaching eof		// add for power save
} __ATTRIBUTE_PACKED__ MP3_CALLBACK_TABLE;

#define MUTE_TIMEOUT			1
#define MAX_QUEUED_MP3_BUFFERS	32

typedef struct tagMP3_BUFFER
{
	RMuint8		*buffer;
	RMuint32	length;
	struct tagMP3_BUFFER *next;
} __ATTRIBUTE_PACKED__ MP3_BUFFER;

class mp3Q
{
	MP3_BUFFER	m_Elements[MAX_QUEUED_MP3_BUFFERS];
	MP3_BUFFER	*m_headFree;
	MP3_BUFFER	*m_headBusy;

public:
	mp3Q ();
	~mp3Q ();

	void reset ();
	RMint32 queue (RMuint8 *pbuffer, RMuint32 length);
	MP3_BUFFER *dequeue (void);
	MP3_BUFFER *canqueue ();
};

// the class definition
class MP3Decoder
{
	MP3_CALLBACK_TABLE	m_CallbackTable;
	RMint32				m_handle;
	RMuint32			m_State;
	RMuint32			m_SubState;
	RMuint32			m_TotalBytesProcessed;

	mad_timer_t			m_mp3Timer;
	struct mad_stream	m_Stream;
	struct mad_frame	m_Frame;
	struct mad_synth	m_Synth;

	RMuint8				m_MP3Data[1024 * 4];	// max length of 1 mp3 frame?
	RMuint32			m_MP3DataUsed;
	RMuint8				*m_mp3buffer;
	RMuint8				*m_pmp3;
	RMuint32			m_mp3length;
	RMuint8				*m_pcmbuffer;
	RMuint32			m_pcmlength;
	RMuint8				*m_pBufferStart;
	RMuint32			m_mutePCM;
	RMuint32			m_decodeError;
//	EQUALIZER			m_Equalizer;
	RMuint32			m_filelength;
	RMint32				m_enableMP3Q;
	mp3Q				m_mp3Q;
	RMuint32			m_eof;
	RMuint8			m_Qcount;
public:
	MP3Decoder ();
	~MP3Decoder ();

	// call these functions after creation
	MP3_DECODER_ERROR Init (void);
	MP3_DECODER_ERROR InitCallbackTable (MP3_CALLBACK_TABLE *pCallbackTable);

	// call this function if you want to queue mp3 buffers, not waiting for the
	// current buffer to finish
	MP3_DECODER_ERROR EnableMP3BufferQ (RMint32 enable);

	// for file streaming
	// call DecodeFile once
	MP3_DECODER_ERROR DecodeFile (RMint8 *filename);
	// repeatedly call Schedule () to allow the demux to occur	
	MP3_DECODER_ERROR Schedule ();
	// seek to specified seconds
	MP3_DECODER_ERROR Seek (RMint32 seconds);

	// for buffer streaming
	// call DecodeBufferStart () once before starting buffer decoding mode
	MP3_DECODER_ERROR DecodeBufferStart (void);
	// repeatedly call DecodeBuffer () to decode a mp3 buffer stream
	MP3_DECODER_ERROR DecodeBuffer (RMuint8 *pBuffer, RMuint32 length);
	// call DecodeBufferEnd () once after buffer decoding is finished
	MP3_DECODER_ERROR DecodeBufferEnd (void);

	// Equalizer
	void SetEqualizer (RMint32 Band, RMint32 Gain);
};

#endif
