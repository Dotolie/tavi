#include "ogg.h"
#include <errno.h>
#include <memory.h>

#if 0
static void debug_break (void)
{
	fflush (stdout);
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

#define OGG_DECODE_STATE_INIT		0
#define OGG_DECODE_GET_PCM			1
#define OGG_DECODE_OGG				2
#define OGG_DECODE_PUT_PCM			3
#define OGG_DECODE_DONE				4

static size_t ov_readfile (void *ptr, size_t size, size_t nmemb, void *datasource)
{
	OGGDecoder *pOGG = (OGGDecoder *)datasource;
	return pOGG->read (ptr, size*nmemb);
}

static int ov_seekfile (void *datasource, ogg_int64_t offset, int whence)
{
	OGGDecoder *pOGG = (OGGDecoder *)datasource;
	return pOGG->seek (offset, whence);
}

static int ov_closefile (void *datasource)
{
	OGGDecoder *pOGG = (OGGDecoder *)datasource;
	return pOGG->close ();
}

static long ov_tellfile (void *datasource)
{
	OGGDecoder *pOGG = (OGGDecoder *)datasource;
	return pOGG->tell ();
}

OGGDecoder::OGGDecoder ()
{
	m_CallbackTable.context = 0;
	m_CallbackTable.fopen = 0;	
	m_CallbackTable.fread = 0;
	m_CallbackTable.ftell = 0;
	m_CallbackTable.fseek = 0;
	m_CallbackTable.fclose = 0;
	m_CallbackTable.addref = 0;
	m_CallbackTable.release = 0;
	m_CallbackTable.info = 0;
	m_CallbackTable.putPCM = 0;
	m_CallbackTable.getPCM = 0;
	m_CallbackTable.getOGG = 0;
	m_OGGvalid = 0;

	m_OGGvc = NULL;
}

OGGDecoder::~OGGDecoder ()
{
	if (m_OGGvalid)
	{
		ov_clear (&m_OGGFile);
		m_OGGvalid = 0;
	}
	if (m_handle)
	{
		ASSERT (m_CallbackTable.fclose);
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
	}
	if (m_pOGGBuffer)
	{
		m_CallbackTable.release (m_pOGGBuffer, m_CallbackTable.context);
		m_pOGGBuffer = 0;
	}
}

OGG_DECODER_ERROR OGGDecoder::Init (void)
{
	m_handle = 0;
	m_OGGvalid = 0;
	m_State = OGG_DECODE_STATE_INIT;
	return OGG_DECODER_ERROR_NO_ERROR;
}

OGG_DECODER_ERROR OGGDecoder::InitCallbackTable (OGG_CALLBACK_TABLE *pCallbackTable)
{
	DEBUGMSG (1, ("OGGDecoder::InitCallbackTable\n"));

	m_CallbackTable.context = pCallbackTable->context;
	m_CallbackTable.fopen = pCallbackTable->fopen;	
	m_CallbackTable.fread = pCallbackTable->fread;
	m_CallbackTable.fseek = pCallbackTable->fseek;
	m_CallbackTable.ftell = pCallbackTable->ftell;
	m_CallbackTable.fclose = pCallbackTable->fclose;
	m_CallbackTable.addref = pCallbackTable->addref;
	m_CallbackTable.release = pCallbackTable->release;
	m_CallbackTable.info = pCallbackTable->info;
	m_CallbackTable.putPCM = pCallbackTable->putPCM;
	m_CallbackTable.getPCM = pCallbackTable->getPCM;
	m_CallbackTable.getOGG = pCallbackTable->getOGG;

	m_pPCMBuffer = 0;
	m_pOGGBuffer = 0;
	m_State = OGG_DECODE_GET_PCM;

	return OGG_DECODER_ERROR_NO_ERROR;
}

OGG_DECODER_ERROR OGGDecoder::DecodeFile (RMint8 *filename)
{
	ASSERT (m_CallbackTable.fopen);
	m_handle = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	if (m_handle == 0)
	{
		ASSERT (0);
		return OGG_DECODER_ERROR_NOT_AN_OGG_FILE;
	}

	m_ovcallbacks.read_func = ov_readfile;
	m_ovcallbacks.seek_func = ov_seekfile;
	m_ovcallbacks.tell_func = ov_tellfile;
	m_ovcallbacks.close_func = ov_closefile;

	RMint32 r = ov_open_callbacks (this, &m_OGGFile, 0, 0, m_ovcallbacks);
	if (r < 0) 
	{
		DEBUGMSG (1, ("input does not appear to be an ogg bitstream (%d)\n", r));
		if (m_handle)
		{
			ASSERT (m_CallbackTable.fclose);
			ASSERT (m_handle);
			m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
			m_handle = 0;
		}
		return OGG_DECODER_ERROR_NOT_AN_OGG_FILE;
	}

	vorbis_info *vi = ov_info (&m_OGGFile, -1);
	if (vi->channels != 1 && vi->channels != 2) 
	{
		DEBUGMSG (1, ("input does not appear to be an ogg bitstream\n"));
		ov_clear (&m_OGGFile);
		ASSERT (m_handle);
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		return OGG_DECODER_ERROR_NOT_AN_OGG_FILE;
	}

	OGG_DECODE_INFO info;
	info.NumberOfBitsPerSample = 16;
	info.NumberOfChannels = vi->channels;
	info.SamplesPerSecond = vi->rate;
	info.Bitrate = vi->bitrate_nominal;
	DEBUGMSG (1, ("OGG file: %d channels, %d samples/second\n", (RMint32)vi->channels, (RMint32)vi->rate));
	m_CallbackTable.info (OGG_MSG_OGG_DECODE_INFO, &info, m_CallbackTable.context);

	m_OGGvalid = 1;

	m_CallbackTable.getOGG (&m_pOGGBuffer, &m_OGGBufferLength, m_CallbackTable.context);
	ASSERT (m_pOGGBuffer);
	if (m_pOGGBuffer)
		m_CallbackTable.addref (m_pOGGBuffer, m_CallbackTable.context);
	m_pOGGBufferStart = m_pOGGBuffer;
	m_pOGGBufferEnd = m_pOGGBuffer;

	m_OGGvc = ov_comment( &m_OGGFile, -1 );

	return OGG_DECODER_ERROR_NO_ERROR;
}

OGG_DECODER_ERROR OGGDecoder::Schedule ()
{
	RMint32 section = 0, r;

	ASSERT (m_State != OGG_DECODE_STATE_INIT);

	switch (m_State)
	{
	case OGG_DECODE_GET_PCM:
		DEBUGMSG (0, ("OGG_DECODE_GET_PCM\n"));
		ASSERT (m_pPCMBuffer == 0);
		if (m_CallbackTable.getPCM (&m_pPCMBuffer, &m_PCMBufferLength, m_CallbackTable.context))
		{
			break;
		}
		m_CallbackTable.addref (m_pPCMBuffer, m_CallbackTable.context);
		m_State = OGG_DECODE_OGG;
		break;
	case OGG_DECODE_OGG:
		DEBUGMSG (0, ("OGG_DECODE_OGG\n"));
		ASSERT (m_pPCMBuffer);
		ASSERT (m_PCMBufferLength);
		r = ov_read (&m_OGGFile, (RMint8 *)m_pPCMBuffer, m_PCMBufferLength, (int *)&section);
		if (r == 0)
		{
			ov_clear (&m_OGGFile);
			m_State = OGG_DECODE_DONE;
			return OGG_DECODER_ERROR_FILE_DONE;
		}
		DEBUGMSG ((r < 0), ("OGG decode error (%d)...\n", r));
		if (r > 0)
		{
			DEBUGMSG (0, ("OGG decoded %d PCM bytes\n", r));
			m_State = OGG_DECODE_PUT_PCM;
			m_PCMBufferLength = r;
		}
		break;
	case OGG_DECODE_PUT_PCM:
		DEBUGMSG (0, ("OGG_DECODE_PUT_PCM\n"));
		if (m_CallbackTable.putPCM (m_pPCMBuffer, m_PCMBufferLength, m_CallbackTable.context) == 0)
		{
			m_CallbackTable.release (m_pPCMBuffer, m_CallbackTable.context);
			m_pPCMBuffer = 0;
			m_State = OGG_DECODE_GET_PCM;
		}
		break;
	case OGG_DECODE_DONE:
		return OGG_DECODER_ERROR_FILE_DONE;
	}
	return OGG_DECODER_ERROR_NO_ERROR;
}

OGG_DECODER_ERROR OGGDecoder::Seek (RMuint32 seconds, RMuint32 *pseconds)
{
	
	DEBUGMSG (1, ("OGGDecoder::Seek (%d)\n", (int)seconds));
	if (ov_seekable (&m_OGGFile))
	{
		ov_time_seek_page (&m_OGGFile, seconds * 1000);
		if (pseconds)
			*pseconds = seconds;
		if (m_pPCMBuffer)
		{
			m_CallbackTable.release (m_pPCMBuffer, m_CallbackTable.context);
			m_pPCMBuffer = 0;
		}
		m_pOGGBufferStart = m_pOGGBuffer;
		m_pOGGBufferEnd = m_pOGGBuffer;
		m_State = OGG_DECODE_GET_PCM;
		
		return OGG_DECODER_ERROR_NO_ERROR;
	}
	else
		DEBUGMSG (1, ("ov_seekable returns 0\n"));
	return OGG_DECODER_ERROR_INVALID_PARAMETER;
}

RMint32 OGGDecoder::seek (RMint64 pos, RMint32 whence)
{
	ASSERT (pos < 0x100000000LL);
	ASSERT (m_CallbackTable.fseek);
	ASSERT (m_handle);
	DEBUGMSG (0, ("OGG: seek (%d, %d)\n", (RMint32)pos, whence));
	m_pOGGBufferStart = m_pOGGBuffer;
	m_pOGGBufferEnd = m_pOGGBuffer;
	return m_CallbackTable.fseek (m_handle, (RMuint32)pos, whence, m_CallbackTable.context);
}

RMint32 OGGDecoder::read (void *p, RMuint32 len)
{	
	ASSERT (m_handle);	
	if (m_pOGGBuffer == 0)
	{
		RMint32 tmp;
		tmp = m_CallbackTable.fread (m_handle, p, len, m_CallbackTable.context);
		// important - vorbis library depends uses errno
		errno = 0;
		return tmp;
	}
	ASSERT (m_pOGGBuffer);
	RMint32 available, n, total_bytes_transferred;
	total_bytes_transferred = 0;
	available = m_pOGGBufferEnd - m_pOGGBufferStart;
	DEBUGMSG (0, ("OGG available: %d, required: %d\n", (RMint32)available, (RMint32)len));
	if (available < len)
	{
		memcpy (p, m_pOGGBufferStart, available);
		(RMuint8 *)p += available;
		len -= available;
		total_bytes_transferred += available;
		ASSERT (m_pOGGBuffer);
		ASSERT (m_OGGBufferLength);		
		n = m_CallbackTable.fread (m_handle, m_pOGGBuffer, m_OGGBufferLength, m_CallbackTable.context);
		m_pOGGBufferStart = m_pOGGBuffer;
		m_pOGGBufferEnd = m_pOGGBuffer + n;
		available = n;
	}
	if (len < available)
	{
		memcpy (p, m_pOGGBufferStart, len);
		total_bytes_transferred += len;
		m_pOGGBufferStart += len;
	}
	else
	{
		memcpy (p, m_pOGGBufferStart, available);
		total_bytes_transferred += available;
		m_pOGGBufferStart += available;
	}	
	return total_bytes_transferred;	
}

RMint32 OGGDecoder::tell ()
{
	RMint32 tmp;
	ASSERT (m_handle);
	tmp = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
	DEBUGMSG (0, ("OGG: tell (%d)\n", tmp));
	return tmp;
}

RMint32 OGGDecoder::close ()
{
	ASSERT (m_handle);
	DEBUGMSG (0, ("OGG: close ()\n"));
	m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
	m_handle = 0;
	return 0; 
}

char* OGGDecoder::GetArtist( void )
{
	if( !m_OGGvc ) return NULL;
	return vorbis_comment_query( m_OGGvc, "artist", 0 );
}

char* OGGDecoder::GetAlbum( void )
{
	if( !m_OGGvc ) return NULL;
	return vorbis_comment_query( m_OGGvc, "album", 0 );
}

char* OGGDecoder::GetGenre( void )
{
	if( !m_OGGvc ) return NULL;
	return vorbis_comment_query( m_OGGvc, "genre", 0 );
}

char* OGGDecoder::GetTitle( void )
{
	if( !m_OGGvc ) return NULL;
	return vorbis_comment_query( m_OGGvc, "title", 0 );
}

// total time: seconds
int OGGDecoder::GetTotalTime( void )
{
	int sec;

	sec = ov_time_total( &m_OGGFile, -1 );
	sec /= 1000;

	return sec;
}

int OGGDecoder::GetBitrate( void )
{
	int bitrate = ov_bitrate( &m_OGGFile, -1 );
	bitrate /= 1000;

	return bitrate;
}

int OGGDecoder::GetSamplingrate( void )
{
	vorbis_info *vi = ov_info (&m_OGGFile, -1);
	if( !vi ) return 0;
	
	return vi->rate;
}

