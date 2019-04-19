#include "wma.h"

#if 0
// to enable WMA playback you must get a license from microsoft
// after you have obtained a proper WMA license, contact Sigma Designs,
// for the WMA library 
WMADecoder::WMADecoder ()
{
}

WMADecoder::~WMADecoder ()
{
}

WMA_DECODER_ERROR WMADecoder::Init (void)
{
	return WMA_DECODER_ERROR_NOT_IMPLEMENTED;
}
WMA_DECODER_ERROR WMADecoder::InitCallbackTable (WMA_CALLBACK_TABLE *pCallbackTable)
{
	return WMA_DECODER_ERROR_NOT_IMPLEMENTED;
}
WMA_DECODER_ERROR WMADecoder::DecodeFile (RMint8 *filename)
{
	return WMA_DECODER_ERROR_NOT_IMPLEMENTED;
}
WMA_DECODER_ERROR WMADecoder::Schedule ()
{
	return WMA_DECODER_ERROR_NOT_IMPLEMENTED;
}
WMA_DECODER_ERROR WMADecoder::DecodeBuffer (RMuint8 *pBuffer, RMuint32 length)
{
	return WMA_DECODER_ERROR_NOT_IMPLEMENTED;
}
RMuint32 WMADecoder::WMAFileCBGetData (tHWMAFileState state, RMuint32 offset, RMuint32 num_bytes, RMuint8 **ppData)
{
	return 0;
}

#else

#if 1
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
//#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
#define DEBUGMSG(cond,printf_exp)
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif	 

#define WMA_STATE_INIT			0
#define WMA_STATE_DECODE_DATA	1
#define WMA_STATE_GET_PCM		2
#define WMA_STATE_PUT_PCM		3
#define WMA_STATE_FILE_DONE		4

#define WMA_ALBUM_TAG_NAME	"W\0M\0/\0A\0l\0b\0u\0m\0T\0i\0t\0l\0e\0\0"
#define WMA_ALBUM_TAG_LEN	28
#define WMA_GENRE_TAG_NAME	"W\0M\0/\0G\0e\0n\0r\0e\0\0"
#define WMA_GENRE_TAG_LEN		18

static WMADecoder *pWMADecoder = 0;

// misc functions needed by libwma
tWMA_U32 WMAFileCBGetData (tHWMAFileState state, tWMA_U32 offset, tWMA_U32 num_bytes, unsigned char **ppData)
{
	return pWMADecoder->WMAFileCBGetData (state, offset, num_bytes, ppData); 
}
tWMA_U32 WMAFileCBGetLicenseData (tHWMAFileState *state, tWMA_U32 offset, tWMA_U32 num_bytes, unsigned char **ppData) 
{
	// we don't support DRM
    return 0;
}
#include <stdarg.h>
void WMADebugMessage (const char* pszFmt, ...) 
{
	DEBUGMSG (1, ("WMADebugMessage ...\n"));
	va_list argList;
	va_start(argList, pszFmt);
	vprintf (pszFmt, argList);
	va_end (argList);
}
// end functions required by libwma

static inline void wma_swapWord (RMuint8 *buffer, const RMuint32 bufferLength) 
{
	RMuint16 temp;
	RMuint16 *s;
	RMuint32 i;
	s = (RMuint16 *)buffer;	
	ASSERT (((RMuint32)buffer & 1) == 0);
	for (i=0; i<bufferLength/2; i++)
	{
		temp = *s;						
		*s++ = (temp >> 8) | ((temp & 0xff) << 8);
	}
}

WMADecoder::WMADecoder ()
{	   
	DEBUGMSG (1, ("WMADecoder::WMADecoder\n"));
	m_CallbackTable.fclose = 0;
	m_wmaLength = 0;
	m_wmaOffset = 0;
	m_wmaValid = 0;
	m_pWMA = 0;
	// alignment
	//m_wmatmp = (RMuint8 *)((RMuint32)(m_wmatmpbuf + 4) & 0xfffffffc);
	m_wmatmp = new RMuint8[256];
	Init ();
	ASSERT (pWMADecoder == 0);
	pWMADecoder = this;

	m_pContent = new tWMAFileContDesc;
	m_pContent->author_len = 64;
	m_pContent->pAuthor = new unsigned char[m_pContent->author_len];
	m_pContent->title_len = 64;
	m_pContent->pTitle = new unsigned char[m_pContent->title_len];
}

WMADecoder::~WMADecoder ()
{
	DEBUGMSG (1, ("WMADecoder::~WMADecoder\n"));
	if (m_pWMA)
	{
		ASSERT (m_CallbackTable.release);
		m_CallbackTable.release (m_pWMA, m_CallbackTable.context);
		m_pWMA = 0;
		m_wmaOffset = 0;
		m_wmaLength = 0;
		m_wmaValid = 0;
	}
	if (m_handle)
	{
		ASSERT (m_CallbackTable.fclose);
		WMAFileDecodeClose (&m_sState);
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
	}
	pWMADecoder = 0;
	delete[] m_wmatmp;

	// release m_pContent
	delete[] m_pContent->pAuthor;
	delete[] m_pContent->pTitle;
	delete m_pContent;
}

WMA_DECODER_ERROR WMADecoder::Init (void)
{
	DEBUGMSG (1, ("WMADecoder::Init\n"));
	if (m_handle)
	{
		if (m_CallbackTable.fclose)
			m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
	}
	m_handle = 0;
	m_CallbackTable.context = 0;
	m_CallbackTable.fopen = 0;
	m_CallbackTable.fread = 0;
	m_CallbackTable.fclose = 0;
	m_CallbackTable.addref = 0;
	m_CallbackTable.release = 0;
	m_CallbackTable.info = 0;
	m_CallbackTable.putPCM = 0;
	m_CallbackTable.getPCM = 0;
	m_CallbackTable.getWMA = 0;
	m_iSampleCount = 0;
	m_TotalBytesProcessed = 0;
	m_iSamples = 0;
	m_pcmbuffer = 0;
	m_wmaLength = 0;
	m_wmaOffset = 0;
	m_wmaValid = 0;
	m_pWMA = 0;
	m_State = WMA_STATE_INIT;

	return WMA_DECODER_ERROR_NO_ERROR;
}

WMA_DECODER_ERROR WMADecoder::InitCallbackTable (WMA_CALLBACK_TABLE *pCallbackTable)
{
	//DEBUGMSG (1, ("WMADecoder::InitCallbackTable\n"));
	m_CallbackTable.context = pCallbackTable->context;
	m_CallbackTable.fopen = pCallbackTable->fopen;
	m_CallbackTable.fseek = pCallbackTable->fseek;
	m_CallbackTable.fread = pCallbackTable->fread;
	m_CallbackTable.fclose = pCallbackTable->fclose;
	m_CallbackTable.addref = pCallbackTable->addref;
	m_CallbackTable.release = pCallbackTable->release;
	m_CallbackTable.info = pCallbackTable->info;
	m_CallbackTable.putPCM = pCallbackTable->putPCM;
	m_CallbackTable.getPCM = pCallbackTable->getPCM;
	m_CallbackTable.getWMA = pCallbackTable->getWMA;
	return WMA_DECODER_ERROR_NO_ERROR;
}

// for file streaming
static int _samplingrate;
WMA_DECODER_ERROR WMADecoder::DecodeFile (RMint8 *filename)
{
	//ASSERT (m_handle == 0);
	//ASSERT (m_CallbackTable.fopen);
	//ASSERT (m_State == WMA_STATE_INIT);

	//DEBUGMSG (1, ("WMADecoder::DecodeFile (%s)\n", filename));

	RMuint32 r = m_CallbackTable.getWMA (&m_pWMA, &m_wmaLength, m_CallbackTable.context);
	//ASSERT (m_pWMA);
	//ASSERT (m_wmaLength);
	//ASSERT (m_wmaValid == 0);
	//ASSERT (m_wmaOffset == 0);
	ASSERT (r == 0);
	m_CallbackTable.addref (m_pWMA, m_CallbackTable.context);
	m_handle = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	ASSERT (m_handle);
	if (m_handle == 0)
	{
		//DEBUGMSG (1, ("fopen (%s) failed\n", filename));
		return WMA_DECODER_ERROR_NOT_A_WMA_FILE;
	}
	tWMAFileStatus wmaStatus = WMAFileIsWMA (&m_sFileHeaderState);
	if (wmaStatus != cWMA_NoErr) 
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		//DEBUGMSG (1, ("WMAFileIsWMA failed (%s): %d\n", filename, (int)wmaStatus));
		return WMA_DECODER_ERROR_NOT_A_WMA_FILE;
	}
	m_sState = 0;

	// create wma decoder
    if (WMAFileDecodeCreate (&m_sState) != cWMA_NoErr) 
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
        WMAFileDecodeClose (&m_sState);
		return WMA_DECODER_ERROR_NOT_A_WMA_FILE;
    }	
#ifdef _WIN32
	if (WMAFileDecodeInit (m_sState) != cWMA_NoErr) 
#else
	if (WMAFileDecodeInit (m_sState, 0) != cWMA_NoErr) 
#endif
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
        WMAFileDecodeClose (&m_sState);
		return WMA_DECODER_ERROR_NOT_A_WMA_FILE;
    }
	if (WMAFileDecodeInfo (m_sState, &m_sFileHeader) != cWMA_NoErr)
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
        WMAFileDecodeClose (&m_sState);
		return WMA_DECODER_ERROR_NOT_A_WMA_FILE;
	}
    //DEBUGMSG (1, ("- WMA version : %d\n", m_sFileHeader.version));
	//DEBUGMSG (1, ("- Sample Rate : %ld\n", m_sFileHeader.sample_rate));
   // DEBUGMSG (1, ("- Channels    : %d\n", m_sFileHeader.num_channels));
   // DEBUGMSG (1, ("- Bit Rate    : %ld bps\n", m_sFileHeader.bitrate));
  //  DEBUGMSG (1, ("- Duration    : %ld ms\n", m_sFileHeader.duration));
   // DEBUGMSG (1, ("- DRM         : %s\n", m_sFileHeader.has_DRM ? "Yes" : "No"));
	if (m_sFileHeader.has_DRM)
	{
	//	DEBUGMSG (1, ("DRM not supported\n"));
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
        WMAFileDecodeClose (&m_sState);
		return WMA_DECODER_ERROR_NOT_A_WMA_FILE;
	}
	WMA_DECODE_INFO info;
	info.NumberOfBitsPerSample = 16;
	info.NumberOfChannels = m_sFileHeader.num_channels;
	switch (m_sFileHeader.sample_rate)
	{
	case cWMA_SR_08kHz:
		info.SamplesPerSecond = 8000;
		break;
	case cWMA_SR_11_025kHz:
		info.SamplesPerSecond = 11025;
		break;
	case cWMA_SR_22_05kHz:
		info.SamplesPerSecond = 22050;
		break;
	case cWMA_SR_16kHz:
		info.SamplesPerSecond = 16000;
		break;
	case cWMA_SR_32kHz:
		info.SamplesPerSecond = 32000;
		break;
	case cWMA_SR_44_1kHz:
		info.SamplesPerSecond = 44100;
		break;
	case cWMA_SR_48kHz:
		info.SamplesPerSecond = 48000;
		break;
	default:
		ASSERT (0);
		info.SamplesPerSecond = 44100;
		break;
	}
	info.Bitrate = m_sFileHeader.bitrate;
	_samplingrate = info.SamplesPerSecond;
	
	m_CallbackTable.info (WMA_MSG_MP3_DECODE_INFO, &info, m_CallbackTable.context);
	m_State = WMA_STATE_DECODE_DATA;

	// Loading Content Descriptions
	tWMAFileStatus status = WMAFileContentDesc( m_sState, m_pContent );
	if( status != cWMA_NoErr ) {
		printf( "failed to WMAFileContentDesc(%d)\n", status );
	}
	// Loading extension content description
	WMAFileExtendedContentDesc( m_sState, (const tWMAExtendedContentDesc**)&m_pExtContent );
	
	return WMA_DECODER_ERROR_NO_ERROR;
}

WMA_DECODER_ERROR WMADecoder::Schedule ()
{
	tWMAFileStatus iResult;
	RMuint32 pcmlength;

	switch (m_State)
	{
	case WMA_STATE_FILE_DONE:
		//DEBUGMSG (1, ("WMADecoder::Schedule (WMA_STATE_FILE_DONE)\n"));
		return WMA_DECODER_ERROR_FILE_DONE;
		break;
	case WMA_STATE_INIT:
		//DEBUGMSG (0, ("WMADecoder::Schedule (WMA_STATE_INIT)\n"));
		break;
	case WMA_STATE_DECODE_DATA:
		//DEBUGMSG (0, ("WMADecoder::Schedule (WMA_STATE_DECODE_DATA)\n"));
		//ASSERT (m_pcmbuffer == 0);
		//ASSERT (m_iSamples == 0);
#ifdef _WIN32
		iResult = WMAFileDecodeData (m_sState);
		m_iSampleCount = 1;
#else
		iResult = WMAFileDecodeData (m_sState, &m_iSampleCount);
#endif
		if (iResult != cWMA_NoErr)
		{		
			DEBUGMSG (1, ("WMAFileDecodeData returns %d\n", (int)iResult));
			m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
			m_handle = 0;
			WMAFileDecodeClose (&m_sState);
			m_State = WMA_STATE_FILE_DONE;
			return WMA_DECODER_ERROR_FILE_DONE;
		}		
		if (m_iSampleCount)
			m_State = WMA_STATE_GET_PCM;		
		break;
	case WMA_STATE_GET_PCM:
		DEBUGMSG (0, ("WMADecoder::Schedule (WMA_STATE_GET_PCM)\n"));
		ASSERT (m_pcmbuffer == 0);
		ASSERT (m_iSamples == 0);
		if (m_CallbackTable.getPCM (&m_pcmbuffer, &pcmlength, m_CallbackTable.context) == 0)
		{
			m_CallbackTable.addref (m_pcmbuffer, m_CallbackTable.context);
			ASSERT (m_iSamples == 0);
			m_iSamples = WMAFileGetPCM (m_sState, (tWMA_I16*)m_pcmbuffer, NULL, pcmlength/(m_sFileHeader.num_channels*sizeof(short)));
			DEBUGMSG (0, ("WMAFileGetPCM (%lu)\n", m_iSamples));
			if (m_iSamples == 0)
			{
				m_CallbackTable.release (m_pcmbuffer, m_CallbackTable.context);
				m_pcmbuffer = 0;
				m_iSamples = 0;
				m_State = WMA_STATE_DECODE_DATA;
			}
			else
			{
				wma_swapWord (m_pcmbuffer, m_iSamples * m_sFileHeader.num_channels * sizeof(short));
				m_State = WMA_STATE_PUT_PCM;
			}
		}
		break;
	case WMA_STATE_PUT_PCM:
		ASSERT (m_iSamples);		
		ASSERT (m_pcmbuffer);
		DEBUGMSG (0, ("putPCM (%lu)\n", m_iSamples * m_sFileHeader.num_channels * sizeof(short)));
		if (m_CallbackTable.putPCM (m_pcmbuffer, m_iSamples * m_sFileHeader.num_channels * sizeof(short), m_CallbackTable.context) == 0)
		{
			m_CallbackTable.release (m_pcmbuffer, m_CallbackTable.context);
			m_pcmbuffer = 0;
			m_iSamples = 0;
			m_State = WMA_STATE_GET_PCM;
		}
		break;
	default:
		ASSERT (0);
		break;
	}
	return WMA_DECODER_ERROR_NO_ERROR;
}

WMA_DECODER_ERROR WMADecoder::Seek (RMuint32 seconds, RMuint32 *pactual)
{
	RMuint32 actual_position;
	ASSERT (m_pWMA);
	// do actual seeking
	actual_position = WMAFileSeek (m_sState, seconds * 1000);
	// flush file reading cache
	m_wmaValid = 0;
	m_wmaOffset = 0;
	// release any pending pcm buffer
	if (m_pcmbuffer)
	{
		ASSERT (m_CallbackTable.release);
		m_CallbackTable.release (m_pcmbuffer, m_CallbackTable.context);
		m_pcmbuffer = 0;
	}
	// update new position
	if (pactual)
		*pactual = actual_position / 1000;
	// resume decoding state
	m_State = WMA_STATE_DECODE_DATA;
	// ok!
	return WMA_DECODER_ERROR_NO_ERROR;
}

// for buffer streaming
WMA_DECODER_ERROR WMADecoder::DecodeBuffer (RMuint8 *pBuffer, RMuint32 length)
{
	DEBUGMSG (1, ("WMADecoder::DecodeBuffer ()\n"));
	return WMA_DECODER_ERROR_NOT_IMPLEMENTED;
}

// for wma library
RMuint32 WMADecoder::WMAFileCBGetData (tHWMAFileState state, RMuint32 offset, RMuint32 num_bytes, RMuint8 **ppData)
{
	tWMA_U32 iBytesRead;
	RMuint8 *pWMA;
	RMuint32 wmaLength;
	//DEBUGMSG (1, ("WMADecoder::WMAFileCBGetData (%lu) @ %lu\n", num_bytes, offset));
	ASSERT (m_pWMA);
	ASSERT (num_bytes <= m_wmaLength);
	if (num_bytes > m_wmaLength)
	{
		//DEBUGMSG (1, ("WMADecoder: WMAFileCBGetData requested too much (%lu bytes)\n", num_bytes));
		num_bytes = m_wmaLength;
    }	
	// is it already in the cache?	
	if ((offset >= m_wmaOffset) && 
		(offset < (m_wmaOffset + m_wmaValid)) && 
		((offset + num_bytes) < (m_wmaOffset + m_wmaValid)))
	{
		// yes
		pWMA = m_pWMA + (offset - m_wmaOffset);
		iBytesRead = num_bytes;
		//DEBUGMSG (1, ("data cached (%lu) @ %lu\n", num_bytes, (offset - m_wmaOffset)));
	}
	else
	{
		int dummy;
		// no - refill
		m_CallbackTable.fseek (m_handle, offset, SEEK_SET, 0);
		m_wmaOffset = offset;
		m_wmaValid = m_CallbackTable.fread (m_handle, m_pWMA, m_wmaLength, m_CallbackTable.context);
		//DEBUGMSG (1, ("refill cache (%lu) @ %lu\n", m_wmaValid, offset));
		pWMA = m_pWMA;
		iBytesRead = num_bytes;
	}
	//DEBUGMSG (1, ("pWma = 0x%08lx\n", (RMuint32)pWMA));
    *ppData = pWMA;
	//ASSERT (iBytesRead <= sizeof (m_wmatmpbuf));
	// for data alignment - we should re-compile the wma library
	// so this copy is not necessary
	// check out LOADBYTE() etc. in asfparse/loadstuff.h 
	memcpy (m_wmatmp, pWMA, iBytesRead);
	*ppData = m_wmatmp;
    return iBytesRead;
}

int WMADecoder::GetTotalTime( void )
{
	return( m_sFileHeader.duration/1000 );
}

int WMADecoder::GetBitrate( void )
{
	return( m_sFileHeader.bitrate/1000 );
}

int WMADecoder::GetSamplingrate( void )
{
	return _samplingrate;
}

char* WMADecoder::GetArtist( int* len )
{
	if( !m_pContent ) {
		*len = 0;
		return NULL;
	}
	*len = m_pContent->author_len;
	return (char*)m_pContent->pAuthor;
}

char* WMADecoder::GetAlbum( int* len )
{
	*len = 0;
	if( !m_pExtContent ) {
		return NULL;
	}
	ECD_DESCRIPTOR* pECD;
	int i;
	for( i=0; i<m_pExtContent->cDescriptors; i++ ) {
		pECD = &m_pExtContent->pDescriptors[i];
		if( memcmp(WMA_ALBUM_TAG_NAME, pECD->pwszName, WMA_ALBUM_TAG_LEN ) == 0 ) break;
	}

	if( i == m_pExtContent->cDescriptors ) return NULL;

	*len = pECD->cbValue;
	return ( char* )pECD->uValue.pwszString;
}

char* WMADecoder::GetGenre( int* len )
{
	*len = 0;
	if( !m_pExtContent ) {
		return NULL;
	}
	ECD_DESCRIPTOR* pECD;
	int i;
	for( i=0; i<m_pExtContent->cDescriptors; i++ ) {
		pECD = &m_pExtContent->pDescriptors[i];
		if( memcmp(WMA_GENRE_TAG_NAME, pECD->pwszName, WMA_GENRE_TAG_LEN ) == 0 ) break;
	}

	if( i == m_pExtContent->cDescriptors ) return NULL;

	*len = pECD->cbValue;
	return ( char* )pECD->uValue.pwszString;
}

char* WMADecoder::GetTitle( int* len )
{
	if( !m_pContent ) {
		*len = 0;
		return NULL;
	}
	*len = m_pContent->title_len;
	return (char*)m_pContent->pTitle;
}

#endif
