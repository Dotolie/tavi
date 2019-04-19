#include "ac3.h"

#if 0
#include <stdio.h>
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

// ac3 frame size
static RMuint32 frmsizecod_table[4][38] = 
{
	// 48
	{64, 64, 80, 80, 96, 96, 112, 112, 128, 128, 160, 160, 192, 192, 224, 224, 256, 256, 320, 320, 
	384, 384, 448, 448, 512, 512, 640, 640, 768, 768, 896, 896, 1024, 1024, 1152, 1152, 1280, 1280},
	// 44.1
	{69, 70, 87, 88, 104, 105, 121, 122, 139, 140, 174, 175, 208, 209, 243, 244, 278, 279, 348, 349,
	417, 418, 487, 488, 557, 558, 696, 697, 835, 836, 975, 976, 1114, 1115, 1253, 1254, 1393, 1394},
	// 32
	{96, 96, 120, 120, 144, 144, 168, 168, 192, 192, 240, 240, 288, 288, 336, 336, 384, 384, 480, 480,
	576, 576, 672, 672, 768, 768, 960, 960, 1152, 1152, 1344, 1344, 1536, 1536, 1728, 1728, 1920, 1920},
	// reserved - default to 48
	{64, 64, 80, 80, 96, 96, 112, 112, 128, 128, 160, 160, 192, 192, 224, 224, 256, 256, 320, 320, 
	384, 384, 448, 448, 512, 512, 640, 640, 768, 768, 896, 896, 1024, 1024, 1152, 1152, 1280, 1280},
};
static RMuint32 fscod_table[4] = 
{
	48000, 44100, 32000, 48000
};

#define AC3_DEMUX_STATE_INIT		0
#define AC3_DEMUX_STATE_GET_AC3		1
#define AC3_DEMUX_STATE_PUT_AC3		2
#define AC3_DEMUX_STATE_EOF			3

AC3Demux::AC3Demux ()
{
}

AC3Demux::~AC3Demux ()
{
	if (m_Handle)
	{
		ASSERT (m_CallbackTable.fclose);
		m_CallbackTable.fclose (m_Handle, m_CallbackTable.context);
		m_Handle = 0;
	}
}

AC3_DECODER_ERROR AC3Demux::Init ()
{
	m_State = AC3_DEMUX_STATE_INIT;
	m_SamplesPerSecond = 0;
	m_Handle = 0;
	m_pAC3buffer = 0;
	return AC3_DECODER_ERROR_NO_ERROR;
}

AC3_DECODER_ERROR AC3Demux::InitCallbackTable (AC3_CALLBACK_TABLE *pCallbackTable)
{
	ASSERT (m_State == AC3_DEMUX_STATE_INIT);
	ASSERT (m_SamplesPerSecond == 0);
	ASSERT (m_Handle == 0);
	ASSERT (m_pAC3buffer == 0);

	m_CallbackTable.context = pCallbackTable->context;
	m_CallbackTable.fopen = pCallbackTable->fopen;
	m_CallbackTable.fread = pCallbackTable->fread;
	m_CallbackTable.fseek = pCallbackTable->fseek;
	m_CallbackTable.fclose = pCallbackTable->fclose;
	m_CallbackTable.addref = pCallbackTable->addref;
	m_CallbackTable.release = pCallbackTable->release;
	m_CallbackTable.info = pCallbackTable->info;
	m_CallbackTable.getAC3 = pCallbackTable->getAC3;
	m_CallbackTable.putAC3 = pCallbackTable->putAC3;

	return AC3_DECODER_ERROR_NO_ERROR;
}

AC3_DECODER_ERROR AC3Demux::DemuxFile (RMint8 *filename)
{	
	ASSERT (m_CallbackTable.getAC3);
	ASSERT (m_pAC3buffer == 0);
	if (m_CallbackTable.getAC3 (&m_pAC3buffer, &m_AC3length, m_CallbackTable.context))
	{
		DEBUGMSG (1, ("AC3Demux::DemuxFile - unable to get detection buffer\n"));
		return AC3_DECODER_ERROR_NOT_AN_AC3_FILE;
	}
	if (m_AC3length < (1024*16))
	{
		DEBUGMSG (1, ("AC3Demux::DemuxFile - detection buffer too small (<16k)\n"));
		return AC3_DECODER_ERROR_NOT_AN_AC3_FILE;
	}
	m_Handle = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	ASSERT (m_Handle);
	if (m_Handle == 0)
	{
		DEBUGMSG (1, ("AC3Demux::DemuxFile - failed to open %s\n", filename));
		return AC3_DECODER_ERROR_NOT_AN_AC3_FILE;
	}
	m_AC3length = 1024 * 16;
	RMuint32 n = m_CallbackTable.fread (m_Handle, m_pAC3buffer, m_AC3length, m_CallbackTable.context);
	if (n == 0)
	{
		DEBUGMSG (1, ("AC3Demux::DemuxFile - zero size file\n"));
		return AC3_DECODER_ERROR_NOT_AN_AC3_FILE;
	}
	DEBUGMSG (1, ("ac3 (%d): %02x %02x\n", (RMint32)n, m_pAC3buffer[0], m_pAC3buffer[1]));
	// check that this really is an ac3 file
	RMuint32 i;
	m_SamplesPerSecond = 0;
	for (i=0; i<(n-5); i++)
	{
		if ((m_pAC3buffer[i] == 0x0b) && (m_pAC3buffer[i+1] == 0x77))
		{
			DEBUGMSG (1, ("found hdr ...\n"));
			// found a header - check the fscod and the frmsizecod
			RMint32 fscod = (m_pAC3buffer[i+4] >> 6) & 0x3;
			RMint32 frmsizecod = (m_pAC3buffer[i+4] >> 0) & 0x3f;
			if (fscod == 3)
			{
				// fake header
				DEBUGMSG (1, ("fake hdr (1)\n"));
				continue;
			}
			if (frmsizecod > 0x25)
			{
				// fake header
				DEBUGMSG (1, ("fake hdr (2)\n"));
				continue;
			}
			RMuint32 sampling_rate = fscod_table[fscod];
			RMuint32 word_per_syncframe = frmsizecod_table[fscod][frmsizecod];
			// check that the next header is present
			if ((i+word_per_syncframe*2+1) < (1024*16))
			{
				DEBUGMSG (1, ("word_per_syncframe = %d\n", (RMint32)word_per_syncframe));
				if ((m_pAC3buffer[i+word_per_syncframe*2] == 0x0b) &&
					(m_pAC3buffer[i+word_per_syncframe*2+1] == 0x77))
				{
					// ok - accept this as an ac3 file
					AC3_DECODE_INFO info;
					info.SamplesPerSecond = sampling_rate;
					ASSERT (m_CallbackTable.info);
					m_CallbackTable.info (AC3_MSG_AC3_DECODE_INFO, &info, m_CallbackTable.context);
					m_SamplesPerSecond = sampling_rate;
					DEBUGMSG (1, ("AC3 sampling rate: %d\n", (int)m_SamplesPerSecond));
					break;
				}
			}
			else
			{
				DEBUGMSG (1, ("buffer too small\n"));
				DEBUGMSG (1, ("i = %d\n", (RMint32)i));
				DEBUGMSG (1, ("word_per_syncframe = %d\n", (RMint32)word_per_syncframe));
			}
		}
	}
	m_pAC3buffer = 0;
	if (m_SamplesPerSecond)
	{
		m_State = AC3_DEMUX_STATE_GET_AC3;
		return AC3_DECODER_ERROR_NO_ERROR;
	}
	else
	{
		DEBUGMSG (1, ("could not find any valid ac3 headers\n"));
		return AC3_DECODER_ERROR_NOT_AN_AC3_FILE;
	}
}

AC3_DECODER_ERROR AC3Demux::Schedule ()
{
	switch (m_State)
	{
	case AC3_DEMUX_STATE_GET_AC3:
		DEBUGMSG (1, ("AC3_DEMUX_STATE_GET_AC3\n"));
		ASSERT (m_pAC3buffer == 0);
		ASSERT (m_CallbackTable.getAC3);
		if (m_CallbackTable.getAC3 (&m_pAC3buffer, &m_AC3length, m_CallbackTable.context) == 0)
		{
			m_AC3length = m_CallbackTable.fread (m_Handle, m_pAC3buffer, m_AC3length, m_CallbackTable.context);
			if (m_AC3length)
			{
				m_CallbackTable.addref (m_pAC3buffer, m_CallbackTable.context); 
				m_State = AC3_DEMUX_STATE_PUT_AC3;
			}
			else
			{
				m_State = AC3_DEMUX_STATE_EOF;
			}
		}
		break;
	case AC3_DEMUX_STATE_PUT_AC3:
		DEBUGMSG (1, ("AC3_DEMUX_STATE_PUT_AC3\n"));
		if (m_CallbackTable.putAC3 (m_pAC3buffer, m_AC3length, m_CallbackTable.context) == 0)
		{
			m_CallbackTable.release (m_pAC3buffer, m_CallbackTable.context); 
			m_pAC3buffer = 0;
			m_State = AC3_DEMUX_STATE_GET_AC3;
		}
		break;
	case AC3_DEMUX_STATE_EOF:
		DEBUGMSG (1, ("AC3_DEMUX_STATE_EOF\n"));
		return AC3_DECODER_ERROR_FILE_DONE;
		break;
	}
	return AC3_DECODER_ERROR_NO_ERROR;
}
