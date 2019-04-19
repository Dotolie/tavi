#include "mp3.h"

// 1 to enable verbose debug output
#define MP3_VERBOSE	0

#if 0
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

#define MP3_STATE_INIT					0
#define MP3_STATE_DECODING_FILE			1
	#define MP3_SUBSTATE_GET_MP3_BUFFER	0
	#define MP3_SUBSTATE_GET_PCM_BUFFER	1
	#define MP3_SUBSTATE_DECODE_DATA	2
	#define MP3_SUBSTATE_PUT_PCM_1		3
	#define MP3_SUBSTATE_PUT_PCM_2		4
#define MP3_STATE_DECODING_BUFFER		2
#define MP3_STATE_EOF					3

MP3Decoder::MP3Decoder ()
{
	DEBUGMSG (1, ("MP3Decoder::MP3Decoder\n"));
	m_CallbackTable.fclose = 0;
	Init ();
}

MP3Decoder::~MP3Decoder ()
{
	DEBUGMSG (1, ("MP3Decoder::~MP3Decoder\n"));
	if (m_handle)
	{
		ASSERT (m_CallbackTable.fclose);
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
	}
}

MP3_DECODER_ERROR MP3Decoder::EnableMP3BufferQ (RMint32 enable)
{
	m_enableMP3Q = enable;
	return MP3_DECODER_ERROR_NO_ERROR;
}

MP3_DECODER_ERROR MP3Decoder::Init (void)
{
	DEBUGMSG (1, ("MP3Decoder::Init\n"));
	if (m_handle)
	{
		if (m_CallbackTable.fclose)
			m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
	}
	m_handle = 0;
	m_CallbackTable.context = 0;
	m_CallbackTable.fopen = 0;
	m_CallbackTable.fseek = 0;
	m_CallbackTable.ftell = 0;
	m_CallbackTable.fread = 0;
	m_CallbackTable.fclose = 0;
	m_CallbackTable.addref = 0;
	m_CallbackTable.release = 0;
	m_CallbackTable.info = 0;
	m_CallbackTable.putPCM = 0;
	m_CallbackTable.getPCM = 0;
	m_CallbackTable.getMP3 = 0;
	m_State = MP3_STATE_INIT;
	m_MP3DataUsed = 0;
	m_pcmbuffer = 0;
	m_mp3buffer = 0;
	m_TotalBytesProcessed = 0;
	m_mutePCM = 0;
	m_enableMP3Q = 0;
	m_Qcount = 0;
	return MP3_DECODER_ERROR_NO_ERROR;
}

MP3_DECODER_ERROR MP3Decoder::InitCallbackTable (MP3_CALLBACK_TABLE *pCallbackTable)
{
	DEBUGMSG (1, ("MP3Decoder::InitCallbackTable\n"));
	m_CallbackTable.context = pCallbackTable->context;
	m_CallbackTable.fopen = pCallbackTable->fopen;	
	m_CallbackTable.fseek = pCallbackTable->fseek;	
	m_CallbackTable.ftell = pCallbackTable->ftell;	
	m_CallbackTable.fread = pCallbackTable->fread;
	m_CallbackTable.fclose = pCallbackTable->fclose;
	m_CallbackTable.addref = pCallbackTable->addref;
	m_CallbackTable.release = pCallbackTable->release;
	m_CallbackTable.info = pCallbackTable->info;
	m_CallbackTable.putPCM = pCallbackTable->putPCM;
	m_CallbackTable.getPCM = pCallbackTable->getPCM;
	m_CallbackTable.getMP3 = pCallbackTable->getMP3;
	m_CallbackTable.reachEof= pCallbackTable->reachEof;		// power save
	return MP3_DECODER_ERROR_NO_ERROR;
}

MP3_DECODER_ERROR MP3Decoder::DecodeFile (RMint8 *filename)
{
	DEBUGMSG (1, ("MP3Decoder::DecodeFile (%s)\n", filename));

	ASSERT (m_handle == 0);
	ASSERT (m_CallbackTable.fopen);

#if 0
	// XXX code disabled ...
	// purpose: check that this really is an mp3 file
	// why disabled? this method is crappy - what is a better method?
	RMint32 length = strlen (filename);
	if (filename[length-4] != '.')
	{
		return MP3_DECODER_ERROR_NOT_A_MP3_FILE;
	}
	if ((filename[length-3] != 'm') && (filename[length-3] != 'M'))
	{
		return MP3_DECODER_ERROR_NOT_A_MP3_FILE;
	}
	if ((filename[length-2] != 'p') && (filename[length-2] != 'P'))
	{
		return MP3_DECODER_ERROR_NOT_A_MP3_FILE;
	}
	if (filename[length-1] != '3')
	{
		return MP3_DECODER_ERROR_NOT_A_MP3_FILE;
	}
#endif

	ASSERT (m_CallbackTable.fopen);
	m_handle = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	ASSERT (m_handle);
	if (m_handle == 0)
	{
		return MP3_DECODER_ERROR_NOT_A_MP3_FILE;
	}	

	mad_stream_init (&m_Stream);
	mad_frame_init (&m_Frame);
	mad_synth_init (&m_Synth);
	mad_timer_reset (&m_mp3Timer);

	ASSERT (m_MP3DataUsed == 0);
	m_MP3DataUsed = 0;
	m_State = MP3_STATE_DECODING_FILE;
	m_SubState = MP3_SUBSTATE_GET_MP3_BUFFER;

	ASSERT (m_CallbackTable.fseek);
	ASSERT (m_CallbackTable.ftell);

	m_CallbackTable.fseek (m_handle, 0, SEEK_END, m_CallbackTable.context);
	m_filelength = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
	ASSERT ((RMint32)m_filelength > 0);
	DEBUGMSG (1, ("MP3 filelength: %d bytes\n", (int)m_filelength));
	m_CallbackTable.fseek (m_handle, 0, SEEK_SET, m_CallbackTable.context);

	m_mp3Q.reset ();
	m_eof = 0;

	return MP3_DECODER_ERROR_NO_ERROR;
}

MP3_DECODER_ERROR MP3Decoder::Schedule ()
{
	RMint32 n;
	RMuint32 ok;
	MP3_BUFFER *pqueuedbuffer;

	if (m_State == MP3_STATE_INIT)
	{
		DEBUGMSG (MP3_VERBOSE, ("MP3Decoder::Schedule (MP3_STATE_INIT)\n"));
		return MP3_DECODER_ERROR_NO_ERROR;
	}
	else if (m_State == MP3_STATE_EOF)
	{
		return MP3_DECODER_ERROR_FILE_DONE;
	}

	switch (m_SubState)
	{
	case MP3_SUBSTATE_GET_MP3_BUFFER:
		DEBUGMSG (MP3_VERBOSE, ("MP3Decoder::Schedule (MP3_SUBSTATE_GET_MP3_BUFFER)\n"));
		ASSERT (m_mp3buffer == 0);
		ASSERT (m_CallbackTable.getMP3);

		// shall we get another buffer or is one already queued?
		pqueuedbuffer = m_mp3Q.dequeue ();
		if (pqueuedbuffer)
		{
			m_mp3buffer = pqueuedbuffer->buffer;
			m_mp3length = pqueuedbuffer->length;
			ok = 0;
			ASSERT (m_CallbackTable.release);
			ASSERT (m_mp3buffer);
		}
		else
		{
			ok = m_CallbackTable.getMP3 (&m_mp3buffer, &m_mp3length, m_CallbackTable.context);
			ASSERT (m_CallbackTable.addref);
			m_CallbackTable.addref (m_mp3buffer, m_CallbackTable.context);
			ASSERT (m_CallbackTable.fread);
			ASSERT (m_handle);
			m_mp3length = m_CallbackTable.fread (m_handle, m_mp3buffer, m_mp3length, m_CallbackTable.context);
			DEBUGMSG (1, ("fread length [1]= %d\n", m_mp3length) );	// Powerdown test
		}

		if (ok == 0)
		{
			m_pmp3 = m_mp3buffer;
			m_TotalBytesProcessed += m_mp3length;
			if (m_mp3length == 0)
			{
				mad_synth_finish (&m_Synth);	
				mad_frame_finish (&m_Frame);
				mad_stream_finish (&m_Stream);
				ASSERT (m_handle);
				ASSERT (m_CallbackTable.fclose);
				m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
				m_handle = 0;
				m_State = MP3_STATE_EOF;
				return MP3_DECODER_ERROR_FILE_DONE; 
			}
			else
			{
				m_SubState = MP3_SUBSTATE_GET_PCM_BUFFER;
			}
			mad_stream_buffer (&m_Stream, m_pmp3, m_mp3length);
		}
		break;
	case MP3_SUBSTATE_GET_PCM_BUFFER:	
		DEBUGMSG (MP3_VERBOSE, ("MP3Decoder::Schedule (MP3_SUBSTATE_GET_PCM_BUFFER)\n"));
		ASSERT (m_pcmbuffer == 0);
		ASSERT (m_CallbackTable.getPCM);
		if (m_CallbackTable.getPCM (&m_pcmbuffer, &m_pcmlength, m_CallbackTable.context) == 0)
		{
			ASSERT (m_pcmlength >= (1152 * 4));
			ASSERT (m_CallbackTable.addref);
			m_CallbackTable.addref (m_pcmbuffer, m_CallbackTable.context);
			m_SubState = MP3_SUBSTATE_DECODE_DATA;
		}
		break;
	case MP3_SUBSTATE_DECODE_DATA:
		DEBUGMSG (MP3_VERBOSE, ("MP3Decoder::Schedule (MP3_SUBSTATE_DECODE_DATA)\n"));
		// decode one mp3 frame		
		if (m_MP3DataUsed)
		{
			n = sizeof (m_MP3Data) - m_MP3DataUsed;
			if ((RMint32)m_mp3length < n)
				n = m_mp3length;
			memcpy (m_MP3Data + m_MP3DataUsed, m_pmp3, n);
			m_pBufferStart = m_MP3Data + m_MP3DataUsed;
			m_MP3DataUsed += n;
			ASSERT (m_MP3Data[0] == 0xff);
			ASSERT ((m_MP3Data[1] & 0xf0) == 0xf0);
			mad_stream_buffer (&m_Stream, m_MP3Data, m_MP3DataUsed);
			do
			{
				m_Stream.error = MAD_ERROR_NONE;
				mad_frame_decode (&m_Frame, &m_Stream);
				if (m_Stream.error != MAD_ERROR_NONE)
				{
					if (MAD_RECOVERABLE (m_Stream.error))
					{
						m_mutePCM = MUTE_TIMEOUT;
						DEBUGMSG (1, ("recoverable mp3 frame error: 0x%08lx\n", m_Stream.error));
						m_decodeError = 1;
						continue;
					}
				}
				break;
			} while (1);
			if (m_Stream.error == MAD_ERROR_BUFLEN || m_Stream.buffer == 0)
			{
				ASSERT (n == (RMint32)m_mp3length);
				n = m_Stream.bufend - m_Stream.next_frame;
				memmove (m_MP3Data, m_Stream.next_frame, n);
				m_MP3DataUsed = n;
				if (m_mp3buffer)
				{
					ASSERT (m_CallbackTable.release);
					ASSERT (m_mp3buffer);
					m_CallbackTable.release (m_mp3buffer, m_CallbackTable.context);
					m_mp3buffer = 0;
				}
				ASSERT (m_pcmbuffer);
				ASSERT (m_CallbackTable.release);
				m_CallbackTable.release (m_pcmbuffer, m_CallbackTable.context);
				m_pcmbuffer = 0;
				m_SubState = MP3_SUBSTATE_GET_MP3_BUFFER;
				return MP3_DECODER_ERROR_NO_ERROR;
			}
			ASSERT (m_Stream.error == MAD_ERROR_NONE);
			MP3_DECODE_INFO mp3info;
			mp3info.NumberOfBitsPerSample = 16;
			mp3info.NumberOfChannels = MAD_NCHANNELS(&m_Frame.header);
			mp3info.SamplesPerSecond = m_Frame.header.samplerate;
			mp3info.Bitrate = m_Frame.header.bitrate;
			DEBUGMSG (0, ("mp3info: %lu bits/sample\n", mp3info.NumberOfBitsPerSample));
			DEBUGMSG (0, ("mp3info: %lu channels\n", mp3info.NumberOfChannels));
			DEBUGMSG (0, ("mp3info: %lu samples/second\n", mp3info.SamplesPerSecond));
			if (m_CallbackTable.info)
				m_CallbackTable.info (MP3_MSG_MP3_DECODE_INFO, &mp3info, m_CallbackTable.context);
			mad_synth_frame (&m_Synth, &m_Frame, (short*)m_pcmbuffer);
			m_SubState = MP3_SUBSTATE_PUT_PCM_2;
			return MP3_DECODER_ERROR_NO_ERROR;
		}		
		do
		{
			m_Stream.error = MAD_ERROR_NONE;
			mad_frame_decode (&m_Frame, &m_Stream);
			if (m_Stream.error != MAD_ERROR_NONE)
			{
				if (MAD_RECOVERABLE (m_Stream.error))
				{
					m_mutePCM = MUTE_TIMEOUT;
					DEBUGMSG (1, ("<2>recoverable mp3 frame error: 0x%08lx\n", m_Stream.error));
					m_decodeError = 1;
					continue;
				}
				else
				{
					break;
				}
			}
			break;
		} while (1);
		if (m_Stream.error == MAD_ERROR_NONE)
		{
			ASSERT (m_Stream.error == MAD_ERROR_NONE);
			MP3_DECODE_INFO mp3info;
			mp3info.NumberOfBitsPerSample = 16;
			mp3info.NumberOfChannels = MAD_NCHANNELS(&m_Frame.header);
			mp3info.SamplesPerSecond = m_Frame.header.samplerate;
			mp3info.Bitrate = m_Frame.header.bitrate;
			DEBUGMSG (0, ("mp3info: %lu bits/sample\n", mp3info.NumberOfBitsPerSample));
			DEBUGMSG (0, ("mp3info: %lu channels\n", mp3info.NumberOfChannels));
			DEBUGMSG (0, ("mp3info: %lu samples/second\n", mp3info.SamplesPerSecond));
			if (m_CallbackTable.info)
				m_CallbackTable.info (MP3_MSG_MP3_DECODE_INFO, &mp3info, m_CallbackTable.context);
			mad_synth_frame (&m_Synth, &m_Frame, (short*)m_pcmbuffer);
			m_SubState = MP3_SUBSTATE_PUT_PCM_1;
		}
		else if (m_Stream.error == MAD_ERROR_BUFLEN || m_Stream.buffer == NULL)
		{
			n = m_Stream.bufend - m_Stream.next_frame;
			memmove (m_MP3Data, m_Stream.next_frame, n);
			m_MP3DataUsed = n;
			ASSERT (m_CallbackTable.release);
			ASSERT (m_mp3buffer);
			m_CallbackTable.release (m_mp3buffer, m_CallbackTable.context);
			m_mp3buffer = 0;
			ASSERT (m_pcmbuffer);
			ASSERT (m_CallbackTable.release);
			m_CallbackTable.release (m_pcmbuffer, m_CallbackTable.context);
			m_pcmbuffer = 0;
			m_SubState = MP3_SUBSTATE_GET_MP3_BUFFER;
		}
		break;
	case MP3_SUBSTATE_PUT_PCM_1:
		DEBUGMSG (MP3_VERBOSE, ("MP3Decoder::Schedule (MP3_SUBSTATE_PUT_PCM_1)\n"));
		n = (MAD_NCHANNELS(&m_Frame.header)) * (MAD_NSBSAMPLES(&m_Frame.header)) * 32 * sizeof(RMuint16);
		ASSERT (m_CallbackTable.putPCM);
		if (m_mutePCM)
		{
			memset (m_pcmbuffer, 0, 1152*4);
			m_mutePCM--;
		}
		if (m_CallbackTable.putPCM (m_pcmbuffer, n, m_CallbackTable.context) == 0)
		{
			ASSERT (m_CallbackTable.release);
			ASSERT (m_pcmbuffer);
			m_CallbackTable.release (m_pcmbuffer, m_CallbackTable.context);
			m_pcmbuffer = 0;
			m_SubState = MP3_SUBSTATE_GET_PCM_BUFFER;
		}
		break;
	case MP3_SUBSTATE_PUT_PCM_2:
		DEBUGMSG (MP3_VERBOSE, ("MP3Decoder::Schedule (MP3_SUBSTATE_PUT_PCM_2)\n"));
		n = (MAD_NCHANNELS(&m_Frame.header)) * (MAD_NSBSAMPLES(&m_Frame.header)) * 32 * sizeof(RMuint16);
		ASSERT (m_CallbackTable.putPCM);
		if (m_mutePCM)
		{
			memset (m_pcmbuffer, 0, 1152*4);
			m_mutePCM--;
		}
		if (m_CallbackTable.putPCM (m_pcmbuffer, n, m_CallbackTable.context) == 0)
		{
			ASSERT (m_CallbackTable.release);
			ASSERT (m_pcmbuffer);
			m_CallbackTable.release (m_pcmbuffer, m_CallbackTable.context);
			m_pcmbuffer = 0;
			m_SubState = MP3_SUBSTATE_GET_PCM_BUFFER;
			n = m_Stream.next_frame - m_pBufferStart;
			if (n >= 0)
			{
				m_pmp3 += n;
				m_mp3length -= n;
				m_MP3DataUsed = 0;
				ASSERT (m_pmp3[0] == 0xff);
				ASSERT ((m_pmp3[1] & 0xf0) == 0xf0);
				mad_stream_buffer (&m_Stream, m_pmp3, m_mp3length);
			}
			else
			{
				m_MP3DataUsed = -n;
				memmove (m_MP3Data, m_Stream.next_frame, m_MP3DataUsed);
				m_SubState = MP3_SUBSTATE_GET_PCM_BUFFER;
			}
		}
		break;
	default:
		ASSERT (0);
		break;
	}

	// get an mp3 buffer and read data into it
	RMuint8 *pmp3buffer;
	RMuint32 mp3bufferlength;
	if ((m_enableMP3Q) && (m_eof == 0) && m_mp3Q.canqueue () && m_Qcount++==0)
	{
		m_Qcount%=12;					// power save

		if (m_CallbackTable.getMP3 (&pmp3buffer, &mp3bufferlength, m_CallbackTable.context) == 0)
		{
			mp3bufferlength = m_CallbackTable.fread (m_handle, pmp3buffer, mp3bufferlength, m_CallbackTable.context);
			DEBUGMSG (1, ("fread length [2]= %d\n", mp3bufferlength) );	// Power save
			if (mp3bufferlength)
			{
				m_mp3Q.queue (pmp3buffer, mp3bufferlength);
				ASSERT (m_CallbackTable.addref);
				m_CallbackTable.addref (pmp3buffer, m_CallbackTable.context);
			}
			else
			{
				m_eof = 1;
				ASSERT (m_CallbackTable.reachEof);		// power save
				if (m_CallbackTable.reachEof )
					m_CallbackTable.reachEof();			// power save
			}
		}
	}

	return MP3_DECODER_ERROR_NO_ERROR;
}

MP3_DECODER_ERROR MP3Decoder::Seek (RMint32 seconds)
{
	if (m_Frame.header.bitrate)
	{
		RMint32 byte_rate;
		RMuint32 seek_position;
		byte_rate = (RMint32)m_Frame.header.bitrate / 8;
		seek_position = seconds * byte_rate;
		if ((RMint32)seek_position > (RMint32)(m_filelength - 4096))
		{
			DEBUGMSG (1, ("can not seek to %d, max is %d\n", (int)seek_position, (int)(m_filelength - 4096)));
			return MP3_DECODER_ERROR_INVALID_PARAMETER;	
		}
		DEBUGMSG (1, ("seeking to position byte %d\n", (int)seek_position));
		m_CallbackTable.fseek (m_handle, seek_position, SEEK_SET, m_CallbackTable.context);
		if (m_mp3buffer)
		{
			ASSERT (m_CallbackTable.release);
			m_CallbackTable.release (m_mp3buffer, m_CallbackTable.context);
			m_mp3buffer = 0;
		}
		if (m_pcmbuffer)
		{
			ASSERT (m_CallbackTable.release);
			m_CallbackTable.release (m_pcmbuffer, m_CallbackTable.context);
			m_pcmbuffer = 0;
		}
		// reset internal variables
		mad_stream_init (&m_Stream);
		mad_frame_init (&m_Frame);
		mad_synth_init (&m_Synth);
		mad_timer_reset (&m_mp3Timer);
		m_MP3DataUsed = 0;
		m_State = MP3_STATE_DECODING_FILE;
		m_SubState = MP3_SUBSTATE_GET_MP3_BUFFER;

		m_mp3Q.reset();	// power down
		m_eof = 0;		// power dow
		
		return MP3_DECODER_ERROR_NO_ERROR;
	}
	return MP3_DECODER_ERROR_NOT_IMPLEMENTED;
}

MP3_DECODER_ERROR MP3Decoder::DecodeBufferStart (void)
{
	DEBUGMSG (1, ("MP3Decoder::DecodeBufferStart ()\n"));

	ASSERT (m_State == MP3_STATE_INIT);
	mad_stream_init (&m_Stream);
	mad_frame_init (&m_Frame);
	mad_synth_init (&m_Synth);
	mad_timer_reset (&m_mp3Timer);

	ASSERT (m_MP3DataUsed == 0);
	m_MP3DataUsed = 0;
	m_State = MP3_STATE_DECODING_BUFFER;
	m_SubState = MP3_SUBSTATE_GET_MP3_BUFFER;
	m_mutePCM = 0;

	return MP3_DECODER_ERROR_NO_ERROR;
}

MP3_DECODER_ERROR MP3Decoder::DecodeBuffer (RMuint8 *pBuffer, RMuint32 length)
{
	DEBUGMSG (0, ("MP3Decoder::DecodeBuffer ()\n"));
	MP3_DECODER_ERROR err;
	ASSERT (m_State == MP3_STATE_DECODING_BUFFER);
	ASSERT (m_SubState == MP3_SUBSTATE_GET_MP3_BUFFER);
	ASSERT (pBuffer);
	ASSERT (length);

	m_mp3buffer = pBuffer;
	m_pmp3 = pBuffer;
	m_mp3length = length;
	mad_stream_buffer (&m_Stream, m_pmp3, m_mp3length);
	m_SubState = MP3_SUBSTATE_GET_PCM_BUFFER;
	ASSERT (m_CallbackTable.addref);
	m_CallbackTable.addref (m_mp3buffer, m_CallbackTable.context);

	m_decodeError = 0;
	// XXX yuck - this can potentially hang if
	// the getting a pcm buffer never returns a valid
	// buffer _or_ if writing the pcm never succeeds
	while (m_SubState != MP3_SUBSTATE_GET_MP3_BUFFER)
	{
		err = Schedule ();
		ASSERT (err == MP3_DECODER_ERROR_NO_ERROR);
	}
	ASSERT (m_SubState == MP3_SUBSTATE_GET_MP3_BUFFER);
	if (m_decodeError)
	{
		ASSERT (0);
		return MP3_DECODER_ERROR_BAD_MP3_BUFFER;
	}
	return MP3_DECODER_ERROR_NO_ERROR;
}

MP3_DECODER_ERROR MP3Decoder::DecodeBufferEnd (void)
{
	DEBUGMSG (1, ("MP3Decoder::DecodeBufferEnd ()\n"));

	ASSERT (m_State = MP3_STATE_DECODING_BUFFER);
	mad_synth_finish (&m_Synth);	
	mad_frame_finish (&m_Frame);
	mad_stream_finish (&m_Stream);
	m_State = MP3_STATE_INIT;

	return MP3_DECODER_ERROR_NO_ERROR;
}

void MP3Decoder::SetEqualizer (RMint32 Band, RMint32 Gain)
{	
//	setEqualizer (Band, Gain, &m_Equalizer);
}

//////////////////////////////////////////////////////////////////////////
mp3Q::mp3Q ()
{
}

mp3Q::~mp3Q ()
{
}

void mp3Q::reset ()
{
	RMint32 i;
	m_headBusy = 0;
	m_headFree = &m_Elements[0];
	for (i=0; i<MAX_QUEUED_MP3_BUFFERS-1; i++)
	{
		m_Elements[i].buffer = 0;
		m_Elements[i].length = 0;
		m_Elements[i].next = &m_Elements[i+1];
	}
	m_Elements[i].buffer = 0;
	m_Elements[i].length = 0;
	m_Elements[i].next = 0;
}

RMint32 mp3Q::queue (RMuint8 *pbuffer, RMuint32 length)
{
	// dequeue something from the freeQ
	MP3_BUFFER *tmp, *p;
	tmp = m_headFree;
	if (m_headFree)
		m_headFree = m_headFree->next;
	if (tmp == 0)
	{
		// no free elements left
		ASSERT (0);
		return 1;
	}
	tmp->buffer = pbuffer;
	tmp->length = length;
	tmp->next = 0;
	// queue to tail of busyQ
	p = m_headBusy;
	if (p)
	{
		while (p->next)
			p = p->next;
		p->next = tmp;
	}
	else
	{
		m_headBusy = tmp;
	}
	return 0;
}

MP3_BUFFER *mp3Q::dequeue (void)
{
	MP3_BUFFER *tmp;
	// dequeue from busyQ
	tmp = m_headBusy;
	if (m_headBusy)
		m_headBusy = m_headBusy->next;
	// requeue back to freeQ
	if (tmp)
	{
		tmp->next = m_headFree;
		m_headFree = tmp;
	}
	return tmp;
}

MP3_BUFFER *mp3Q::canqueue ()
{
	return m_headFree;
}
