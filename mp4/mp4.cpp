//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : MPEG4 Demux Code
// 
// $Source: /home/cvsroot/TAVI020/tavi/mp4/mp4.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: mp4.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2005/11/01 11:22:14  etnlwind
// Update for MP4 Debug free
//
// Revision 1.6  2005/10/13 01:25:28  etnlwind
// Update for Header Info
//
//*****************************************************************************

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "mp4.h"
#include <tavi_structures.h> 

int gettime ();

#if 0
#define MP4_ENABLE_DEBUG	1
static FILE *f = 0;
#ifdef _WIN32
#include <windows.h>
static void debug_outs (char *szMsg, ...)
{
	char szBuf[256];
	wvsprintf (szBuf, szMsg, (LPSTR)(&szMsg + 1));
	OutputDebugString (szBuf);
}
static void debug_break (void) 
{
	_asm int 3;
}
#else
#include <stdio.h>
#define debug_outs	printf
static void debug_break (void) 
{
}
#endif
#define ASSERT(exp)					((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

#define MP4_SWAP_ENDIANESS32(x)		(((x & 0x000000ff) << 24) | ((x & 0x0000ff00) << 8) | ((x & 0x00ff0000) >> 8) | ((x & 0xff000000) >> 24))
#define MP4_SWAP_ENDIANESS16(x)		(((x & 0x00ff) << 8) | ((x & 0xff00) >> 8))

#define MP4_DEMUX_STATE_GET_DATA					0
#define MP4_DEMUX_STATE_READ_DATA					1
#define MP4_DEMUX_STATE_PARSE_DATA					2
#define MP4_DEMUX_STATE_PARSE_CHUNK					3
#define MP4_DEMUX_STATE_SEND_DATA					4
#define MP4_RANDOM_ACCESS_ONLY						5
#define MP4_DEMUX_STATE_END_OF_FILE					6

#define MP4_MIN(X,Y) (((X)<=(Y))?(X):(Y))	// to find the min between two number.
#define MP4_MAX(X,Y) (((X)>=(Y))?(X):(Y))	// to find the min between two number.

extern TAVI_TIME totaltime;
extern VIDEOFILE_PLAYINFO gVideofilePlayinfo; 

MP4Demux::MP4Demux ()
{
}

MP4Demux::~MP4Demux ()
{
	if (m_handle)
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
}

MP4_ERROR MP4Demux::Init ()
{
	RMint32 i;
	m_valid = 0;
	m_tmpidx = MP4_MIN_READ_LENGTH;
	m_handle = 0;
	m_videoDSILength = 0;
	for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
		m_audioDSILength[i] = 0;
	for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
		m_spDSILength[i] = 0;
	m_currentHandlerType = 0;
	m_mdatpos = 0;
	m_pData = 0;
	m_total_chunk_size = 0;
	m_samples_per_chunk = 0;	
	m_chunkTime = 0;
	m_video_samples_per_chunk = 0;
	m_video_sample_count = 0;
	m_videoTime = 0;
	m_video_stts_sample_delta_count = 0;
	m_video_ctts_sample_delta_count = 0;
	m_video_ctts_sample_delta = 0;
	for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
	{
		m_audio_samples_per_chunk[i] = 0;
		m_audio_sample_count[i] = 0;
		m_audioTime[i] = 0;
		m_audio_stts_sample_delta_count[i] = 0;
		m_audio_ctts_sample_delta_count[i] = 0;
		m_audio_ctts_sample_delta[i] = 0;
	}
	for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
	{
		m_subpi_samples_per_chunk[i] = 0;
		m_subpi_sample_count[i] = 0;
		m_subpiTime[i] = 0;
		m_subpi_stts_sample_delta_count[i] = 0;
		m_subpi_ctts_sample_delta_count[i] = 0;
		m_subpi_ctts_sample_delta[i] = 0;
	}	
	m_trackindex = -1;
	for (i=0; i<MP4_MAX_TRACKS; i++)
	{
		m_trackids[i] = 0;
		m_tracktimescales[i] = 0;
		m_spWidth[i] = 720;
		m_spHeight[i] = 480;
	}
	m_videoTrackIndex = -1;
	for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
		m_audioTrackIndex[i] = -1;
	for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
		m_subpiTrackIndex[i] = -1;
	m_visualHeight = 480;
	m_visualWidth = 720;
	m_firstVideoChunk = 1;
	m_chpl_present = 0;
	m_naudioStreams = 0;
	m_nspStreams = 0;
	m_RandomAccessPointsOnly = 0;
	m_next_next_chunk = 0;
	m_currentAudioTrack = 0;
	m_currentSubpiTrack = 0;
	return MP4_ERROR_NO_ERROR;
}

MP4_ERROR MP4Demux::InitCallbackTable (MP4_CALLBACK_TABLE *pTable)
{
	m_CallbackTable.context = pTable->context;
	m_CallbackTable.fopen = pTable->fopen;
	m_CallbackTable.fseek = pTable->fseek;
	m_CallbackTable.ftell = pTable->ftell;
	m_CallbackTable.fread = pTable->fread;
	m_CallbackTable.fclose = pTable->fclose;
	m_CallbackTable.addref = pTable->addref;
	m_CallbackTable.release = pTable->release;
	m_CallbackTable.info = pTable->info;
	m_CallbackTable.getData = pTable->getData;
	m_CallbackTable.putDSI = pTable->putDSI;
	m_CallbackTable.putChunk = pTable->putChunk;
	m_CallbackTable.loading = pTable->loading;

	return MP4_ERROR_NO_ERROR;
}

MP4_ERROR MP4Demux::Demux (RMuint16 *filename)
{
	RMint32 i;
	ASSERT (m_handle == 0);

	DEBUGMSG (1, ("MP4Demux::Demux (%s)\n", filename));

	m_handle = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	ASSERT (m_handle);
	if (m_handle == 0)
		return MP4_ERROR_FILE_NOT_FOUND;

	while (NextBox (0) == 0);


	gVideofilePlayinfo.OriginalWidth	= (RMint32) m_visualWidth;
	gVideofilePlayinfo.OriginalHeight	= (RMint32) m_visualHeight;
	totaltime.SecOnly 	= (RMint64) (m_mvhdDuration / m_mvhdTimeScale);
	totaltime.hours 		= (int) (totaltime.SecOnly / 3600);
	totaltime.minutes 	= (int) ((totaltime.SecOnly - totaltime.hours * 3600) / 60);
	totaltime.seconds 	= (int) (totaltime.SecOnly - totaltime.hours*3600 - totaltime.minutes*60);
	
	ASSERT (m_video_stco.IsInitialized ());
	ASSERT (m_video_stsz.IsInitialized ());
	ASSERT (m_video_stsc.IsInitialized ());
	ASSERT (m_audio_stco[0].IsInitialized ());
	ASSERT (m_audio_stsz[0].IsInitialized ());
	ASSERT (m_audio_stsc[0].IsInitialized ());

	RMuint32 video_present, audio_present;

	video_present = 
		m_video_stco.IsInitialized () &&
		m_video_stsz.IsInitialized () &&
		m_video_stsc.IsInitialized ();
   
	audio_present = 
		m_audio_stco[0].IsInitialized () &&
		m_audio_stsz[0].IsInitialized () &&
		m_audio_stsc[0].IsInitialized ();

	if ((video_present == 0) && (audio_present == 0))
	{
		return MP4_ERROR_FILE_NOT_SUPPORTED;
	}

	m_currentOffset = m_mdatpos;
	ASSERT (m_currentOffset);
	m_State = MP4_DEMUX_STATE_GET_DATA;
	m_CallbackTable.fseek (m_handle, m_mdatpos, SEEK_SET, m_CallbackTable.context);

	m_video_chunk_index = 1;

	TRACK_TIMESCALE ts;
	for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
	{
		m_audio_chunk_index[i] = 1;
		if (m_audioTrackIndex[i] != -1)
			ts.audioTimeScale[i] = m_tracktimescales[m_audioTrackIndex[i]];
		else
			ts.audioTimeScale[i] = 0;
	}
	for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
	{
		m_subpi_chunk_index[i] = 1;
		if (m_subpiTrackIndex[i] != -1)
			ts.subpiTimeScale[i] = m_tracktimescales[m_subpiTrackIndex[i]];
		else
			ts.subpiTimeScale[i] = 0;
	}
	if (m_videoTrackIndex != -1)
		ts.videoTimeScale = m_tracktimescales[m_videoTrackIndex];
	else
		ts.videoTimeScale = 0;
	m_CallbackTable.info (MP4_MSG_TRACK_TIMESCALE, &ts, m_CallbackTable.context);

	if (m_videoDSILength)
		m_CallbackTable.putDSI (0, 0, m_videoDSI, m_videoDSILength, ts.videoTimeScale/2, m_CallbackTable.context);
	for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
	{
		if (m_audioDSILength[i])
			m_CallbackTable.putDSI (1 | (i << 8), 0, m_audioDSI[i], m_audioDSILength[i], 0, m_CallbackTable.context);
	}
	for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
	{
		if (m_spDSILength[i])
		{
			SPU_DIMENSIONS spDimensions;
			ASSERT (m_subpiTrackIndex[i] != -1);
			ASSERT (m_subpiTrackIndex[i] < MP4_MAX_TRACKS);
			if (m_subpiTrackIndex[i] != -1)
			{
				spDimensions.spWidth = m_spWidth[m_subpiTrackIndex[i]];
				spDimensions.spHeight = m_spHeight[m_subpiTrackIndex[i]];
				m_CallbackTable.info (MP4_MSG_SPU_DIMENSIONS, &spDimensions, m_CallbackTable.context);
				m_CallbackTable.putDSI (2 | (i << 8), 0, m_spDSI[i], m_spDSILength[i], 0, m_CallbackTable.context);
			}
		}
	}

	if (m_videoDSILength)
	{
		VIDEO_DIMENSIONS visualDimensions;
		visualDimensions.visualWidth = m_visualWidth;
		visualDimensions.visualHeight = m_visualHeight;
		m_CallbackTable.info (MP4_MSG_VIDEO_DIMENSIONS, &visualDimensions, m_CallbackTable.context);
	}
	m_firstVideoChunk = 1;

	return MP4_ERROR_NO_ERROR;
}

MP4_ERROR MP4Demux::Schedule ()
{
	RMuint32 error, error0, error1, error2, error3, error4;

	switch (m_State)
	{
	case MP4_DEMUX_STATE_GET_DATA:
		ASSERT (m_pData == 0);
		if (m_CallbackTable.getData (&m_pData, &m_DataLength, m_CallbackTable.context) == 0)
		{
			m_CallbackTable.addref (m_pData, m_CallbackTable.context);
			m_State = MP4_DEMUX_STATE_READ_DATA;
		}
		break;
	case MP4_DEMUX_STATE_READ_DATA:
		ASSERT (m_pData);
		ASSERT (m_DataLength);
		ASSERT (m_currentOffset == m_CallbackTable.ftell (m_handle, m_CallbackTable.context));
		m_mdatLeft = m_mdatpos + m_mdatlength - m_currentOffset;
		ASSERT ((RMint32)m_mdatLeft >= 0);
		m_DataLength = MP4_MIN (m_DataLength, m_mdatLeft);
		m_DataLength = m_CallbackTable.fread (m_handle, m_pData, m_DataLength, m_CallbackTable.context);
		DEBUGMSG (0, ("m_DataLength = %d\n", (RMint32)m_DataLength));
		DEBUGMSG (0, ("m_mdatLeft = %d\n", (RMint32)m_mdatLeft));
		if (m_DataLength)
		{
			m_pPutData = m_pData;
			m_DataLeft = m_DataLength;
			if (m_total_chunk_size)
				m_State = MP4_DEMUX_STATE_SEND_DATA;
			else if (m_samples_per_chunk > 0)
				m_State = MP4_DEMUX_STATE_PARSE_CHUNK;
			else
				m_State = MP4_DEMUX_STATE_PARSE_DATA;
		}
		else
		{
			ASSERT (m_pData);
			m_CallbackTable.release (m_pData, m_CallbackTable.context);
			m_pData = 0;
			m_State = MP4_DEMUX_STATE_END_OF_FILE;
			return MP4_ERROR_END_OF_FILE;
		}
		break;
	case MP4_DEMUX_STATE_PARSE_DATA:
		{
			RMuint32 voffset = m_video_stco.get32 (&error0);
			// XXX need to change if you want to support more than
			// 2 audio or sp streams
			RMuint32 aoffset0 = m_audio_stco[0].get32 (&error1);
			RMuint32 soffset0 = m_subpi_stco[0].get32 (&error2);
			RMuint32 aoffset1 = m_audio_stco[1].get32 (&error3);
			RMuint32 soffset1 = m_subpi_stco[1].get32 (&error4);
			if (error0 && error1 && error2 && error3 && error4)
				m_State = MP4_DEMUX_STATE_END_OF_FILE;
			else if (m_currentOffset == voffset)
			{
				DEBUGMSG (0, ("voffset: %d\n", (RMint32)voffset));
				RMuint32 first_entry;				
				m_video_stco.advance32 (&error);
				m_State = MP4_DEMUX_STATE_PARSE_CHUNK;
				first_entry = m_video_stsc.get32 (&error);
				if (first_entry == m_video_chunk_index)
				{
					m_video_stsc.advance32 (&error);
					ASSERT (error == 0);
					m_video_samples_per_chunk = m_video_stsc.get32 (&error);
					ASSERT (error == 0);
					DEBUGMSG (0, ("video samples/chunk: %d\n", (RMint32)m_video_samples_per_chunk));
					m_video_stsc.advance32 (&error);
					ASSERT (error == 0);
					m_video_stsc.get32 (&error);
					ASSERT (error == 0);
					m_video_stsc.advance32 (&error);
					ASSERT (error == 0);
				}				
				m_video_chunk_index++;
				m_currentID = 0;
				m_samples_per_chunk = m_video_samples_per_chunk;
				DEBUGMSG (0, ("m_video_samples_per_chunk: %d\n", (RMint32)m_video_samples_per_chunk));
				ASSERT (m_samples_per_chunk > 0);
			}
			else if ((m_currentOffset == aoffset0) || (m_currentOffset == aoffset1))
			{
				RMuint32 aoffset;
				RMint32 ai;
				if (m_currentOffset == aoffset0)
				{
					aoffset = aoffset0;
					ai = 0;
					m_current_au_index = 0;
				}
				else
				{
					aoffset = aoffset1;
					ai = 1;
					m_current_au_index = 1;
				}
				DEBUGMSG (0, ("aoffset: %d\n", (RMint32)aoffset));
				RMuint32 first_entry;
				m_audio_stco[ai].advance32 (&error);
				m_State = MP4_DEMUX_STATE_PARSE_CHUNK;
				first_entry = m_audio_stsc[ai].get32 (&error);
				if (first_entry == m_audio_chunk_index[ai])
				{
					m_audio_stsc[ai].advance32 (&error);
					ASSERT (error == 0);
					m_audio_samples_per_chunk[ai] = m_audio_stsc[ai].get32 (&error);
					ASSERT (error == 0);
					m_audio_stsc[ai].advance32 (&error);
					ASSERT (error == 0);
					m_audio_stsc[ai].get32 (&error);
					ASSERT (error == 0);
					m_audio_stsc[ai].advance32 (&error);
					ASSERT (error == 0);
				}
				m_audio_chunk_index[ai]++;
				m_currentID = 1 | (ai << 8);
				m_samples_per_chunk = m_audio_samples_per_chunk[ai];
				DEBUGMSG (0, ("m_audio_samples_per_chunk: %d\n", (RMint32)m_audio_samples_per_chunk));
				ASSERT (m_samples_per_chunk > 0);
			}
			else if ((m_currentOffset == soffset0) || (m_currentOffset == soffset1))
			{
				RMuint32 soffset;
				RMint32 si;
				if (m_currentOffset == soffset0)
				{
					soffset = soffset0;
					si = 0;
					m_current_sp_index = 0;
				}
				else
				{
					soffset = soffset1;
					si = 1;
					m_current_sp_index = 1;
				}
				DEBUGMSG (0, ("soffset: %d\n", (RMint32)soffset));
				RMuint32 first_entry;
				m_subpi_stco[si].advance32 (&error);
				m_State = MP4_DEMUX_STATE_PARSE_CHUNK;
				first_entry = m_subpi_stsc[si].get32 (&error);
				if (first_entry == m_subpi_chunk_index[si])
				{
					m_subpi_stsc[si].advance32 (&error);
					m_subpi_samples_per_chunk[si] = m_subpi_stsc[si].get32 (&error);
					m_subpi_stsc[si].advance32 (&error);
					m_subpi_stsc[si].get32 (&error);
					m_subpi_stsc[si].advance32 (&error);					
				}
				m_subpi_chunk_index[si]++;
				m_currentID = 2 | (si << 8);
				m_samples_per_chunk = m_subpi_samples_per_chunk[si];
				ASSERT (m_samples_per_chunk);
				DEBUGMSG (0, ("m_subpi_samples_per_chunk: %d\n", (RMint32)m_subpi_samples_per_chunk));
				ASSERT (m_samples_per_chunk > 0);
			}
			else
			{
				// resync to next audio/video/subpi chunk
				RMuint32 next_offset, aoffset, soffset;
				ASSERT (voffset || aoffset0 || soffset0 || aoffset1 || soffset1);
				if (soffset0 && soffset1)
					soffset = MP4_MIN (soffset0, soffset1);
				else
					soffset = soffset0;
				if (aoffset0 && aoffset1)
					aoffset = MP4_MIN (aoffset0, aoffset1);
				else
					aoffset = aoffset0;

				if (voffset && aoffset && soffset)
				{
					next_offset = MP4_MIN (soffset, aoffset);
					next_offset = MP4_MIN (voffset, next_offset);
				}
				else if (voffset && aoffset)
					next_offset = MP4_MIN (voffset, aoffset);
				else if (voffset == 0)
					next_offset = aoffset;
				else
					next_offset = voffset;
				DEBUGMSG (1, ("*** resync to: %d\n", (RMint32)next_offset));
				// need to seek?
				if ((next_offset >= m_currentOffset) &&
					(next_offset < (m_currentOffset + m_DataLeft)))
				{
					ASSERT ((RMint32)(next_offset - m_currentOffset) > 0);
					ASSERT ((RMint32)(next_offset - m_currentOffset) < (RMint32)m_DataLeft);
					m_pPutData += (RMint32)(next_offset - m_currentOffset);
					m_DataLeft -= (RMint32)(next_offset - m_currentOffset);
					ASSERT ((RMint32)m_DataLeft > 0);
					m_currentOffset = next_offset;
					m_DataLength = m_DataLeft;
					ASSERT ((RMint32)m_DataLength > 0);
				}
				else
				{
					m_currentOffset = next_offset;
					ASSERT (m_currentOffset);
					m_CallbackTable.fseek (m_handle, next_offset, SEEK_SET, m_CallbackTable.context);
					ASSERT (m_pData);
					m_CallbackTable.release (m_pData, m_CallbackTable.context);
					m_pData = 0;
					m_State = MP4_DEMUX_STATE_GET_DATA;					
				}
			}
		}
		break;
	case MP4_DEMUX_STATE_PARSE_CHUNK:
		if ((m_currentID & 0xff) == 0)
		{			
			ASSERT (m_samples_per_chunk >= 0);
			DEBUGMSG (0, ("(v) m_samples_per_chunk: %d\n", m_samples_per_chunk));
			m_samples_per_chunk--;
			if (m_samples_per_chunk < 0)
			{
				m_State = MP4_DEMUX_STATE_PARSE_DATA;
				break;
			}
			m_total_chunk_size = 0;
			m_chunkFlags = CHUNK_FLAGS_TIME_VALID;
			if (m_video_sample_size == 0)
			{
				m_total_chunk_size = m_video_stsz.get32 (&error);
				DEBUGMSG (0, (" video sample size: %d\n", (RMint32)m_total_chunk_size));
				m_video_stsz.advance32 (&error);
			}
			else
			{
				m_total_chunk_size = m_video_sample_size;
			}
			m_firstVideoChunk = 0;
			m_video_sample_count++;
			if (m_video_stts_sample_delta_count == 0)
			{
				m_video_stts_sample_delta_count = m_video_stts.get32 (&error);
				ASSERT (error == 0);
				if (error == 0)
				{
					m_video_stts.advance32 (&error);
					m_video_stts_sample_delta = m_video_stts.get32 (&error);
					m_video_stts.advance32 (&error);
				}
			}
			m_video_stts_sample_delta_count--;
			if (m_video_ctts.IsInitialized ())
			{
				DEBUGMSG (0, ("m_video_ctts_sample_delta_count = %d\n", (int)m_video_ctts_sample_delta_count));
				if (m_video_ctts_sample_delta_count == 0)
				{
					m_video_ctts_sample_delta_count = m_video_ctts.get32 (&error);
					ASSERT (error == 0);
					if (error == 0)
					{
						m_video_ctts.advance32 (&error);
						m_video_ctts_sample_delta = m_video_ctts.get32 (&error);
						DEBUGMSG (0, ("m_video_ctts_sample_delta = %d\n", (int)m_video_ctts_sample_delta));
						m_video_ctts.advance32 (&error);
					}
				}
				m_video_ctts_sample_delta_count--;				
				m_chunkTime = m_videoTime + m_video_ctts_sample_delta;
				DEBUGMSG (0, ("m_chunkTime = %d (%d + %d)\n",
					(int)m_chunkTime, (int)m_videoTime, (int)m_video_ctts_sample_delta));
				m_videoTime += m_video_stts_sample_delta;
			}	
			else
			{
				m_chunkTime = m_videoTime;
				DEBUGMSG (0, ("m_chunkTime = %d (%d)\n", (int)m_chunkTime, (int)m_videoTime));
				m_videoTime += m_video_stts_sample_delta;
			}
			m_chunkFlags |=	CHUNK_FLAGS_SAMPLE_START;
			m_State = MP4_DEMUX_STATE_SEND_DATA;
		}
		else if ((m_currentID & 0xff) == 1)
		{
			RMint32 ai;
			ai = m_current_au_index;
			ASSERT (m_samples_per_chunk >= 0);
			DEBUGMSG (0, ("(a) m_samples_per_chunk: %d\n", m_samples_per_chunk));
			m_samples_per_chunk--;
			if (m_samples_per_chunk < 0)
			{
				m_State = MP4_DEMUX_STATE_PARSE_DATA;
				break;
			}
			m_total_chunk_size = 0;
			m_chunkFlags = CHUNK_FLAGS_TIME_VALID;
			if (m_audio_sample_size[ai] == 0)
			{
				m_total_chunk_size = m_audio_stsz[ai].get32 (&error);
				ASSERT (m_total_chunk_size <= 4096);
				ASSERT (m_total_chunk_size);
				ASSERT (error == 0);
				m_audio_stsz[ai].advance32 (&error);
				ASSERT (error == 0);
			}
			else
			{
				m_total_chunk_size = m_audio_sample_size[ai];
				ASSERT (m_total_chunk_size);
			}
			ASSERT (m_total_chunk_size <= 4096);
			DEBUGMSG ((m_total_chunk_size > 4096), ("audio error: m_audio_sample_size = %d, m_total_chunk_size = %08lx\n",
				m_audio_sample_size, m_total_chunk_size));
			m_audio_sample_count[ai]++;
			if (m_audio_stts_sample_delta_count[ai] == 0)
			{
				m_audio_stts_sample_delta_count[ai] = m_audio_stts[ai].get32 (&error);
				ASSERT (error == 0);
				if (error == 0)
				{
					m_audio_stts[ai].advance32 (&error);
					m_audio_stts_sample_delta[ai] = m_audio_stts[ai].get32 (&error);
					m_audio_stts[ai].advance32 (&error);
				}						
			}
			m_audio_stts_sample_delta_count[ai]--;
			m_chunkTime = m_audioTime[ai];
			m_audioTime[ai] += m_audio_stts_sample_delta[ai];
			m_chunkFlags |=	CHUNK_FLAGS_SAMPLE_START;
			m_State = MP4_DEMUX_STATE_SEND_DATA;
		}
		else if ((m_currentID & 0xff) == 2)
		{
			RMint32 si;
			si = m_current_sp_index;
			ASSERT (m_samples_per_chunk >= 0);
			DEBUGMSG (0, ("(s) m_samples_per_chunk: %d\n", m_samples_per_chunk));
			m_samples_per_chunk--;
			if (m_samples_per_chunk < 0)
			{
				m_State = MP4_DEMUX_STATE_PARSE_DATA;
				break;
			}
			m_total_chunk_size = 0;
			m_chunkFlags = CHUNK_FLAGS_TIME_VALID;
			if (m_subpi_sample_size[si] == 0)
			{
				m_total_chunk_size = m_subpi_stsz[si].get32 (&error);
				ASSERT (m_total_chunk_size);
				m_subpi_stsz[si].advance32 (&error);
			}
			else
			{
				m_total_chunk_size = m_subpi_sample_size[si];
				ASSERT (m_total_chunk_size);
			}
			m_subpi_sample_count[si]++;
			if (m_subpi_stts_sample_delta_count[si] == 0)
			{
				m_subpi_stts_sample_delta_count[si] = m_subpi_stts[si].get32 (&error);
				ASSERT (error == 0);
				if (error == 0)
				{
					m_subpi_stts[si].advance32 (&error);
					m_subpi_stts_sample_delta[si] = m_subpi_stts[si].get32 (&error);
					m_subpi_stts[si].advance32 (&error);
				}						
			}
			m_subpi_stts_sample_delta_count[si]--;
			m_chunkTime = m_subpiTime[si];
			m_subpiTime[si] += m_subpi_stts_sample_delta[si];
			m_chunkFlags |=	CHUNK_FLAGS_SAMPLE_START;
			m_State = MP4_DEMUX_STATE_SEND_DATA;
		}
		else
			ASSERT (0);
		break;
	case MP4_DEMUX_STATE_SEND_DATA:		
		{
			RMuint32 l;
			// write m_total_chunk_size data
			l = MP4_MIN (m_total_chunk_size, m_DataLeft);
			if (l == m_total_chunk_size)
				m_chunkFlags |= CHUNK_FLAGS_SAMPLE_END;
			if (m_CallbackTable.putChunk (m_currentID, m_chunkFlags, m_chunkTime, m_pPutData, l, m_CallbackTable.context) != 0)
			{
				ASSERT (0);
				break;
			}
			m_chunkFlags &= ~(CHUNK_FLAGS_TIME_VALID | CHUNK_FLAGS_SAMPLE_START | CHUNK_FLAGS_SAMPLE_END);
			m_total_chunk_size -= l;
			m_currentOffset += l;
			ASSERT (m_currentOffset);
			m_pPutData += l;
			m_DataLeft -= l;
			ASSERT ((RMint32)m_DataLeft >= 0);
			if (m_DataLeft == 0)
			{
				ASSERT (m_pData);
				m_CallbackTable.release (m_pData, m_CallbackTable.context);
				m_pData = 0;
				m_State = MP4_DEMUX_STATE_GET_DATA;	
			}
			else
			{
				ASSERT (m_total_chunk_size == 0);
				m_State = MP4_DEMUX_STATE_PARSE_CHUNK;
			}
		}
		break;
	case MP4_RANDOM_ACCESS_ONLY:
		RandomAccessPointsOnly ();
		break;
	case MP4_DEMUX_STATE_END_OF_FILE:
		return MP4_ERROR_END_OF_FILE;
		break;
	default:
		break;
	}
	return MP4_ERROR_NO_ERROR;
}

#define PROCESS_CURRENT_RANDOM_ACCESS_POINT		0
#define GET_RANDOM_ACCESS_DATA					1
#define READ_CURRENT_RANDOM_ACCESS_POINT		2
#define UPDATE_NEXT_RANDOM_ACCESS_POINT			3
#define UPDATE_PREVIOUS_RANDOM_ACCESS_POINT		4

RMuint32 MP4Demux::RandomAccessPointsOnly ()
{
	RMuint32 error, sample_size, chunk_sample_count;
	RMuint32 first_chunk, next_chunk, sample_description_index, chunk_offset, frame_no, chunk_index;
	RMint32 sample_offset, i, frame_incr, rewind;
	switch (m_RandomAccessState)
	{
	case PROCESS_CURRENT_RANDOM_ACCESS_POINT:
		// before coming here, these variables should be initalized:
		// m_currentRandomAccessPoint
		// m_video_chunk_index
		// m_video_samples_per_chunk
		// m_samples_per_chunk
		// m_video_sample_count

		// find the chunk that this sample is in
		chunk_index = m_video_chunk_index - 1;
		chunk_index += (m_currentRandomAccessPoint - m_video_sample_count) / m_samples_per_chunk;
		chunk_sample_count = m_video_sample_count + 
			((m_currentRandomAccessPoint - m_video_sample_count)/m_samples_per_chunk)*m_samples_per_chunk;
		m_video_stco.seek (chunk_index * 4);
		chunk_offset = m_video_stco.get32 (&error);
		ASSERT (error == 0);
		// find the offset to this sample within this chunk
		sample_offset = m_currentRandomAccessPoint - chunk_sample_count;
		// calculate the file position of this random access point
		if (m_video_sample_size)
		{
			chunk_offset += (m_video_sample_size * sample_offset);
			sample_size = m_video_sample_size;
		}
		else
		{
			m_video_stsz.seek (chunk_sample_count * 4);
			for (i=0; i<sample_offset; i++)
			{
				chunk_offset += m_video_stsz.get32 (&error);
				m_video_stsz.advance32 (&error);
				ASSERT (error == 0);
			}
			// calculate the size of this random access point
			sample_size = m_video_stsz.get32 (&error);
			ASSERT (error == 0);
			m_video_stsz.seek (m_currentRandomAccessPoint * 4);
			sample_size = m_video_stsz.get32 (&error);
			ASSERT (error == 0);
		}
		seek (chunk_offset);
		m_RandomAccessSampleSize = sample_size;
		ASSERT (m_RandomAccessSampleSize);
		m_RandomAccessState = GET_RANDOM_ACCESS_DATA;
		m_chunkFlags = CHUNK_FLAGS_TIME_VALID | CHUNK_FLAGS_SAMPLE_START;
		m_chunkTime = m_currentRandomAccessPoint * m_video_stts_sample_delta;
#ifdef MP4_ENABLE_DEBUG
		{
			static int IFrameCounter = 0;
			RMint32 h, m, s, t;
			t = (RMint32)(m_chunkTime / m_tracktimescales[m_videoTrackIndex]);
			h = (t / 3600);
			m = (t - h * 3600) / 60;
			s = t - h*3600 - m*60;
			DEBUGMSG (1, ("(%d) send frame %d @%d %d bytes (%02d:%02d:%02d)\n",
				IFrameCounter++,
				(int)m_currentRandomAccessPoint, (int)chunk_offset, (int)sample_size,
				h, m, s));
		}
		if (f == 0)
			f = fopen ("d:\\iframe.m4v", "wb");		
#endif
		break;

	case GET_RANDOM_ACCESS_DATA:
		ASSERT (m_pRandomAccessData == 0);
		if (m_CallbackTable.getData (&m_pRandomAccessData, &m_RandomLength, m_CallbackTable.context) == 0)
		{
			ASSERT (m_RandomLength);
			m_CallbackTable.addref (m_pRandomAccessData, m_CallbackTable.context);
			m_RandomAccessState = READ_CURRENT_RANDOM_ACCESS_POINT;
		}		
		break;
	case READ_CURRENT_RANDOM_ACCESS_POINT:
		sample_size = MP4_MIN (m_RandomLength, m_RandomAccessSampleSize);
		sample_size = m_CallbackTable.fread (m_handle, m_pRandomAccessData, sample_size, m_CallbackTable.context);
#ifdef MP4_ENABLE_DEBUG		
		if (f)
		{
			fwrite (m_pRandomAccessData, 1, sample_size, f);
		}		
#endif
		if (sample_size == 0)
		{
			ASSERT (0);
			m_State = MP4_DEMUX_STATE_END_OF_FILE;
			return MP4_ERROR_NO_ERROR;
		}
		ASSERT (m_pRandomAccessData);
		DEBUGMSG (0, (" putChunk (%08lx, %08lx%08lx, %08lx, %d)\n",	
			(RMuint32)(m_chunkFlags),
			(RMuint32)(m_chunkTime >> 32), (RMuint32)(m_chunkTime), 
			(RMuint32)m_pRandomAccessData, (int)sample_size));
		if (m_CallbackTable.putChunk (0, m_chunkFlags, m_chunkTime, m_pRandomAccessData, sample_size, m_CallbackTable.context) != 0)
		{
			ASSERT (0);
			m_State = MP4_DEMUX_STATE_END_OF_FILE;
			return MP4_ERROR_NO_ERROR;
		}
		m_CallbackTable.release (m_pRandomAccessData, m_CallbackTable.context);
		m_chunkTime = 0;
		m_chunkFlags = 0;
		m_pRandomAccessData = 0;
		m_RandomAccessSampleSize -= sample_size;
		if (m_RandomAccessSampleSize == 0)
		{
			if (m_RandomAccessPointsOnly > 0)
				m_RandomAccessState = UPDATE_NEXT_RANDOM_ACCESS_POINT;
			else
				m_RandomAccessState = UPDATE_PREVIOUS_RANDOM_ACCESS_POINT;
		}
		else
			m_RandomAccessState = GET_RANDOM_ACCESS_DATA;
		break;
	case UPDATE_NEXT_RANDOM_ACCESS_POINT:
		// re-initialize:
		// m_currentRandomAccessPoint
		// m_video_chunk_index
		// m_video_samples_per_chunk
		// m_samples_per_chunk
		// m_video_sample_count
		ASSERT (m_RandomAccessPointsOnly > 0);
		m_currentRandomAccessPoint = m_video_stss.get32 (&error) - 1;		
		m_video_stss.advance32 (&error);
		if (error)
		{
			m_State = MP4_DEMUX_STATE_END_OF_FILE;
			return MP4_ERROR_NO_ERROR;
		}
		RMuint32 prev_first_chunk, prev_samples_per_chunk, samples_per_chunk;  
		frame_no = m_video_sample_count;
		prev_first_chunk = m_video_chunk_index;
		prev_samples_per_chunk = m_video_samples_per_chunk;
		while (1)
		{
			first_chunk = m_video_stsc.get32 (&error);
			frame_incr = prev_samples_per_chunk * (first_chunk - prev_first_chunk);
			if ((frame_no + frame_incr) >= m_currentRandomAccessPoint)
				break;
			frame_no += frame_incr;
			m_video_stsc.advance32 (&error);
			ASSERT (error == 0);
			if (error)
			{
				m_State = MP4_DEMUX_STATE_END_OF_FILE;
				return MP4_ERROR_NO_ERROR;
			}
			samples_per_chunk = m_video_stsc.get32 (&error);			
			m_video_stsc.advance32 (&error);
			ASSERT (error == 0);
			if (error)
			{
				m_State = MP4_DEMUX_STATE_END_OF_FILE;
				return MP4_ERROR_NO_ERROR;
			}
			sample_description_index = m_video_stsc.get32 (&error);
			m_video_stsc.advance32 (&error);
			ASSERT (error == 0);		
			if (error)
			{
				m_State = MP4_DEMUX_STATE_END_OF_FILE;
				return MP4_ERROR_NO_ERROR;
			}			
			prev_first_chunk = first_chunk;
			prev_samples_per_chunk = samples_per_chunk;
		}
		m_video_sample_count = frame_no;
		m_video_chunk_index = prev_first_chunk;
		m_video_samples_per_chunk = prev_samples_per_chunk;
		m_samples_per_chunk = prev_samples_per_chunk;
		
		m_RandomAccessState = PROCESS_CURRENT_RANDOM_ACCESS_POINT;
		break;

	case UPDATE_PREVIOUS_RANDOM_ACCESS_POINT:
		// re-initialize:
		// m_currentRandomAccessPoint
		// m_video_chunk_index
		// m_video_samples_per_chunk
		// m_samples_per_chunk
		// m_video_sample_count
		ASSERT (m_RandomAccessPointsOnly < 0);
		m_video_stss.rewind32 (&error);
		ASSERT (error == 0);
		m_currentRandomAccessPoint = m_video_stss.get32 (&error) - 1;
		if (error)
		{
			m_State = MP4_DEMUX_STATE_END_OF_FILE;
			return MP4_ERROR_NO_ERROR;
		}
		frame_no = m_video_sample_count;
		next_chunk = m_video_chunk_index;
		while (1)
		{
			rewind = 1;
			if (m_next_next_chunk)
			{
				ASSERT (m_next_next_chunk != next_chunk);
				frame_incr = m_samples_per_chunk * (m_next_next_chunk - next_chunk);
				if ((m_next_next_frame_no - frame_incr) < m_currentRandomAccessPoint)
				{
					first_chunk = next_chunk;
					samples_per_chunk = m_samples_per_chunk;
					frame_no = m_next_next_frame_no;
					rewind = 0;
				}
			}
			if (rewind)
			{
				m_video_stsc.rewind32 (&error);
				m_video_stsc.rewind32 (&error);
				m_video_stsc.rewind32 (&error);
				m_video_stsc.rewind32 (&error);
				m_video_stsc.rewind32 (&error);
				m_video_stsc.rewind32 (&error);
				ASSERT (error == 0);
				if (error)
				{
					m_State = MP4_DEMUX_STATE_END_OF_FILE;
					return MP4_ERROR_NO_ERROR;
				}
				first_chunk = m_video_stsc.get32 (&error);
				m_video_stsc.advance32 (&error);
				samples_per_chunk = m_video_stsc.get32 (&error);
				m_video_stsc.advance32 (&error);
				m_video_stsc.get32 (&error);
				m_video_stsc.advance32 (&error);		
				frame_incr = samples_per_chunk * (next_chunk - first_chunk);
			}
			ASSERT (frame_incr > 0);
			ASSERT (frame_no >= (RMuint32)frame_incr);			
			if ((frame_no - frame_incr) < m_currentRandomAccessPoint)
			{
				if (rewind)
				{
					m_next_next_chunk = next_chunk;
					m_next_next_frame_no = frame_no;
				}
				m_video_chunk_index = first_chunk;
				m_video_samples_per_chunk = samples_per_chunk;
				m_samples_per_chunk = samples_per_chunk;
				m_video_sample_count = frame_no - frame_incr;
				m_RandomAccessState = PROCESS_CURRENT_RANDOM_ACCESS_POINT;
				break;
			}
			m_next_next_chunk = 0;
			next_chunk = first_chunk;
			frame_no -= frame_incr;
		}
		break;
	}
	return MP4_ERROR_NO_ERROR;
}

RMuint64 MP4Demux::Duration ()
{
	ASSERT (m_mvhdTimeScale);
	if (m_mvhdTimeScale == 0)
		m_mvhdTimeScale = 1;
	return (RMuint64)((RMuint64)m_mvhdDuration * 1000 / (RMuint64)m_mvhdTimeScale);
}

MP4_ERROR MP4Demux::Seek (RMint64 position_in_ms)
{
	RMuint32 video_present, audio_present, subpi_present;

	video_present = 
		m_video_stco.IsInitialized () &&
		m_video_stsz.IsInitialized () &&
		m_video_stsc.IsInitialized ();
   
	audio_present = 
		m_audio_stco[0].IsInitialized () &&
		m_audio_stsz[0].IsInitialized () &&
		m_audio_stsc[0].IsInitialized ();

	subpi_present = 
		m_subpi_stco[0].IsInitialized () &&
		m_subpi_stsz[0].IsInitialized () &&
		m_subpi_stsc[0].IsInitialized ();

	if ((video_present == 0) && (audio_present == 0))
	{
		ASSERT (0);
		return MP4_ERROR_NOT_ALLOWED;
	}

	m_CallbackTable.info (MP4_MSG_FLUSH_START, 0, m_CallbackTable.context);

	while (m_State != MP4_DEMUX_STATE_GET_DATA)
	{
		if (Schedule () == MP4_ERROR_END_OF_FILE)
			return MP4_ERROR_NOT_ALLOWED;
	}

	m_CallbackTable.info (MP4_MSG_FLUSH_END, 0, m_CallbackTable.context);

	if (position_in_ms == 0)
	{
		RMint32 i;

		m_CallbackTable.info (MP4_MSG_SEEK_START, 0, m_CallbackTable.context);

		m_video_stco.reset ();
		m_video_stts.reset ();
		m_video_ctts.reset ();
		m_video_stsz.reset ();
		m_video_stsc.reset ();
		m_video_stss.reset ();
		m_video_chunk_index = 1;
		m_video_samples_per_chunk = 1;
		m_video_sample_count = 0;
		m_video_stts_sample_delta_count = 0;
		m_video_ctts_sample_delta_count = 0;
		m_video_ctts_sample_delta = 0;
		m_videoTime = 0;

		for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
		{
			m_audio_stco[i].reset ();
			m_audio_stts[i].reset ();
			m_audio_ctts[i].reset ();
			m_audio_stsz[i].reset ();
			m_audio_stsc[i].reset ();
			m_audio_chunk_index[i] = 1;
			m_audio_samples_per_chunk[i] = 1;
			m_audio_sample_count[i] = 0;
			m_audio_stts_sample_delta_count[i] = 0;
			m_audio_ctts_sample_delta_count[i] = 0;
			m_audio_ctts_sample_delta[i] = 0;
			m_audioTime[i] = 0;
		}

		for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
		{
			m_subpi_stco[i].reset ();
			m_subpi_stts[i].reset ();
			m_subpi_stsz[i].reset ();
			m_subpi_stsc[i].reset ();
			m_subpi_chunk_index[i] = 1;
			m_subpi_samples_per_chunk[i] = 1;
			m_subpi_sample_count[i] = 0;
			m_subpi_stts_sample_delta_count[i] = 0;
			m_subpi_ctts_sample_delta_count[i] = 0;
			m_subpi_ctts_sample_delta[i] = 0;
			m_subpiTime[i] = 0;
		}

		m_samples_per_chunk = 0;
		m_chunkFlags = 0;
		m_total_chunk_size = 0;

		m_currentOffset = m_mdatpos;
		seek (m_currentOffset);	

		if (m_videoDSILength)
			m_CallbackTable.putDSI (0, 0, m_videoDSI, m_videoDSILength, (RMint64)m_tracktimescales[m_videoTrackIndex]/2, m_CallbackTable.context);
		for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
		{
			if (m_audioDSILength[i])
				m_CallbackTable.putDSI (1 | (i << 8), 0, m_audioDSI[i], m_audioDSILength[i], 0, m_CallbackTable.context);
		}
		for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
		{
			if (m_spDSILength)
			{
				SPU_DIMENSIONS spDimensions;
				ASSERT (m_subpiTrackIndex[i] != -1);
				ASSERT (m_subpiTrackIndex[i] < MP4_MAX_TRACKS);
				if (m_subpiTrackIndex[i] != -1)
				{
					spDimensions.spWidth = m_spWidth[m_subpiTrackIndex[i]];
					spDimensions.spHeight = m_spHeight[m_subpiTrackIndex[i]];
					m_CallbackTable.info (MP4_MSG_SPU_DIMENSIONS, &spDimensions, m_CallbackTable.context);
					m_CallbackTable.putDSI (2 | (i << 8), 0, m_spDSI[i], m_spDSILength[i], 0, m_CallbackTable.context);
				}
			}
		}

		m_firstVideoChunk = 1;
		return MP4_ERROR_NO_ERROR;
	}

	RMuint32 videoOffset, audioOffset, subpiOffset;
	MP4_ERROR mp4_error;

	if ((video_present) && (audio_present))
	{
		RMint64 maxtime;
		maxtime = (RMint64)m_trackdurations[m_videoTrackIndex] * (RMint64)1000 / (RMint64)m_tracktimescales[m_videoTrackIndex];
		if (maxtime > 1000)
			maxtime -= 1000;
		DEBUGMSG (1, ("duration of video track: %d ms\n", (RMint32)maxtime));
		DEBUGMSG (1, ("seek to: %d ms\n", (RMint32)position_in_ms));
		if (position_in_ms >= maxtime)
		{
			DEBUGMSG (1, ("seek to time exceeds duration\n"));
			return MP4_ERROR_NOT_ALLOWED;
		}
		ASSERT (m_CallbackTable.info);
		m_CallbackTable.info (MP4_MSG_SEEK_START, 0, m_CallbackTable.context);
//		int t0, t1;
//		t0 = gettime ();
		mp4_error = videoSeek (position_in_ms, &videoOffset);
//		t1 = gettime ();
//		printf ("videoSeek took %d ms\n", t1-t0);
		DEBUGMSG (1, ("videoOffset: %d\n", (RMint32)videoOffset));
		ASSERT (mp4_error == MP4_ERROR_NO_ERROR);
//		t0 = gettime ();
		mp4_error = audioSeek (videoOffset, &audioOffset, 1);
//		t1 = gettime ();
//		printf ("audioSeek took %d ms\n", t1-t0);
		DEBUGMSG (1, ("audioOffset: %d\n", (RMint32)audioOffset));
		ASSERT (mp4_error == MP4_ERROR_NO_ERROR);
		m_currentOffset = MP4_MIN (videoOffset, audioOffset); 		
		DEBUGMSG (1, ("m_currentOffset: %d\n", (RMint32)m_currentOffset));
		if (subpi_present)
		{
//			t0 = gettime ();
			mp4_error = subpiSeek (m_currentOffset, &subpiOffset, 1);
//			t1 = gettime ();
//			printf ("subpiSeek took %d ms\n", t1-t0);
			ASSERT (mp4_error == MP4_ERROR_NO_ERROR);
			ASSERT (subpiOffset >= m_currentOffset);
		}
	}
	else if (video_present)
	{
		RMint64 maxtime;
		maxtime = (RMint64)m_trackdurations[m_videoTrackIndex] * (RMint64)1000 / (RMint64)m_tracktimescales[m_videoTrackIndex];
		if (maxtime > 1000)
			maxtime -= 1000;
		DEBUGMSG (1, ("duration of video track: %d ms\n", (RMint32)maxtime));
		DEBUGMSG (1, ("seek to: %d ms\n", (RMint32)position_in_ms));
		if (position_in_ms >= maxtime)
		{
			DEBUGMSG (1, ("seek to time exceeds duration\n"));
			return MP4_ERROR_NOT_ALLOWED;
		}
		ASSERT (m_CallbackTable.info);
		m_CallbackTable.info (MP4_MSG_SEEK_START, 0, m_CallbackTable.context);
		mp4_error = videoSeek (position_in_ms, &videoOffset);
		ASSERT (mp4_error == MP4_ERROR_NO_ERROR);
		m_currentOffset = videoOffset;
		if (subpi_present)
		{
			mp4_error = subpiSeek (m_currentOffset, &subpiOffset, 1);
			ASSERT (mp4_error == MP4_ERROR_NO_ERROR);
			ASSERT (subpiOffset >= m_currentOffset);
		}
	}
	else if (audio_present)
	{
		RMint64 maxtime;
		maxtime = (RMint64)m_trackdurations[m_audioTrackIndex[m_currentAudioTrack]] * (RMint64)1000 / (RMint64)m_tracktimescales[m_audioTrackIndex[m_currentAudioTrack]];
		if (maxtime > 1000)
			maxtime -= 1000;
		DEBUGMSG (1, ("duration of audio track: %d ms\n", (RMint32)maxtime));
		DEBUGMSG (1, ("seek to: %d ms\n", (RMint32)position_in_ms));
		if (position_in_ms >= maxtime)
		{
			DEBUGMSG (1, ("seek to time exceeds duration\n"));
			return MP4_ERROR_NOT_ALLOWED;
		}
		ASSERT (m_CallbackTable.info);
		m_CallbackTable.info (MP4_MSG_SEEK_START, 0, m_CallbackTable.context);
		mp4_error = audioSeek (position_in_ms, &audioOffset, 0);
		ASSERT (mp4_error == MP4_ERROR_NO_ERROR);
		m_currentOffset = audioOffset;
		if (subpi_present)
		{
			mp4_error = subpiSeek (m_currentOffset, &subpiOffset, 1);
			ASSERT (mp4_error == MP4_ERROR_NO_ERROR);
			ASSERT (subpiOffset >= m_currentOffset);
		}
	}
	else
	{
		ASSERT (0);
		return MP4_ERROR_NOT_ALLOWED;
	}
	seek (m_currentOffset);	

	if (m_videoDSILength)
		m_CallbackTable.putDSI (0, 0, m_videoDSI, m_videoDSILength, (RMint64)m_tracktimescales[m_videoTrackIndex]/2, m_CallbackTable.context);
	RMuint32 i;
	for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
	{
		if (m_audioDSILength[i])
			m_CallbackTable.putDSI (1 | (i << 8), 0, m_audioDSI[i], m_audioDSILength[i], 0, m_CallbackTable.context);
	}
	for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
	{
		if (m_spDSILength[i])
		{
			SPU_DIMENSIONS spDimensions;
			ASSERT (m_subpiTrackIndex[i] != -1);
			ASSERT (m_subpiTrackIndex[i] < MP4_MAX_TRACKS);
			if (m_subpiTrackIndex[i] != -1)
			{
				spDimensions.spWidth = m_spWidth[m_subpiTrackIndex[i]];
				spDimensions.spHeight = m_spHeight[m_subpiTrackIndex[i]];
				m_CallbackTable.info (MP4_MSG_SPU_DIMENSIONS, &spDimensions, m_CallbackTable.context);
				m_CallbackTable.putDSI (2 | (i << 8), 0, m_spDSI[i], m_spDSILength[i], 0, m_CallbackTable.context);
			}
		}
	}
	m_firstVideoChunk = 1;

	return MP4_ERROR_NO_ERROR;
}

MP4_ERROR MP4Demux::Next (RMint64 currentTime)
{
	if (!m_chpl_present)
		return MP4_ERROR_NOT_ALLOWED;

	DEBUGMSG (1, ("MP4Demux::Next, currentTime = %d\n", (RMint32)currentTime));
	// find the current chapter we are on
	RMint32 i, currentChapter = 0, nextChapter;	
	for (i=0; i<(RMint32)m_nchapters; i++)
	{
		DEBUGMSG (1, ("MP4Demux::Next, chapter = %d, time = %d\n", 
			i, (RMint32)m_mp4Chapters[i].time));
		if (m_mp4Chapters[i].time > currentTime)
		{
			if (i)
			{
				currentChapter = i - 1;
				break;
			}
			else
				currentChapter = 0;
		}
	}
	if (i == (RMint32)m_nchapters)
	{
		DEBUGMSG (1, ("MP4Demux::Next, last chapter\n"));
		nextChapter = 0;
	}
	else
	{
		DEBUGMSG (1, ("MP4Demux::Next, current chapter = %d\n", (RMint32)currentChapter));
		nextChapter = currentChapter + 1;
	}
	DEBUGMSG (1, ("MP4Demux::Next, seeking to %d, %s\n", 
		(RMint32)m_mp4Chapters[nextChapter].time,
		m_mp4Chapters[nextChapter].name));
	return Seek (m_mp4Chapters[nextChapter].time);
}

MP4_ERROR MP4Demux::Previous (RMint64 currentTime)
{
	if (!m_chpl_present)
		return MP4_ERROR_NOT_ALLOWED;

	DEBUGMSG (1, ("MP4Demux::Previous, currentTime = %d\n", (RMint32)currentTime));
	// find the current chapter we are on
	RMint32 i, currentChapter = 0, prevChapter;	
	for (i=0; i<(RMint32)m_nchapters; i++)
	{
		DEBUGMSG (1, ("MP4Demux::Previous, chapter = %d, time = %d\n", 
			i, (RMint32)m_mp4Chapters[i].time));
		if (m_mp4Chapters[i].time > currentTime)
		{
			if (i)
			{
				currentChapter = i - 1;
				break;
			}
			else
				currentChapter = 0;
		}
	}
	if (i == 0)
	{
		DEBUGMSG (1, ("MP4Demux::Previous, first chapter\n"));
		return MP4_ERROR_NOT_ALLOWED;
	}
	else
	{
		DEBUGMSG (1, ("MP4Demux::Previous, current chapter = %d\n", (RMint32)currentChapter));
		if (currentChapter == 0)
		{
			prevChapter = 0;
		}
		else
		{
			prevChapter = currentChapter - 1;
		}
	}
	DEBUGMSG (1, ("MP4Demux::Previous, seeking to %d, %s\n", 
		(RMint32)m_mp4Chapters[prevChapter].time,
		m_mp4Chapters[prevChapter].name));
	return Seek (m_mp4Chapters[prevChapter].time);
}

MP4_ERROR MP4Demux::RandomAccessPointsOnly (RMint64 currentTime, RMint32 direction)
{	
	if (m_video_stss.IsInitialized () == 0)
		return MP4_ERROR_NOT_IMPLEMENTED;
	if (direction == 0)
	{
		if (m_RandomAccessPointsOnly)
		{
			m_next_next_chunk = 0;
			m_RandomAccessPointsOnly = 0;
			m_State = MP4_DEMUX_STATE_GET_DATA;
			return MP4_ERROR_NO_ERROR;
		}						
		return MP4_ERROR_NOT_ALLOWED;
	}
	if (m_RandomAccessPointsOnly)
	{
		DEBUGMSG (1, ("RandomAccessPointsOnly: already in random access points only mode\n"));
		if (m_videoDSILength)
			m_CallbackTable.putDSI (0, 0, m_videoDSI, m_videoDSILength, (RMint64)m_tracktimescales[m_videoTrackIndex]/2, m_CallbackTable.context);
		return MP4_ERROR_NO_ERROR;
	}

	m_CallbackTable.info (MP4_MSG_FLUSH_START, 0, m_CallbackTable.context);

	while (m_State != MP4_DEMUX_STATE_GET_DATA)
	{
		if (Schedule () == MP4_ERROR_END_OF_FILE)
			return MP4_ERROR_NOT_ALLOWED;
	}

	m_CallbackTable.info (MP4_MSG_FLUSH_END, 0, m_CallbackTable.context);

	// a very cheap and easy way to convert time to sample ...
	// it may not work in all cases though ...
	RMuint32 samplecnt;
	currentTime = currentTime * m_tracktimescales[m_videoTrackIndex] / 1000;
	samplecnt = (RMuint32)(currentTime / m_video_stts_sample_delta);
	DEBUGMSG (1, ("start random access at sample %lu\n", samplecnt));

	// do a binary search looking for sample closest to samplecnt
	RMuint32 istart, imiddle, iend, error;
	RMuint32 random_access_point_sample_no;
	istart = 0;
	iend = m_video_stss.entrycount ();
	imiddle = (istart + iend) / 2;
	random_access_point_sample_no = 0;
	while (1)
	{			
		m_video_stss.seek (imiddle * 4);
		random_access_point_sample_no = m_video_stss.get32 (&error) - 1;
		m_video_stss.advance32 (&error);
		if ((imiddle == istart) || (imiddle == iend) || (random_access_point_sample_no == samplecnt))
		{				
			if (random_access_point_sample_no != samplecnt)
			{
				if (direction > 0)
					imiddle = iend;
				else
					imiddle = istart;
				m_video_stss.seek (imiddle * 4);
				random_access_point_sample_no = m_video_stss.get32 (&error) - 1;
				m_video_stss.advance32 (&error);
			}
			break;
		}
		if (samplecnt > random_access_point_sample_no)
			istart = imiddle;
		else 
			iend = imiddle;
		imiddle = (istart + iend) / 2;
	}
	ASSERT (random_access_point_sample_no >= 0);
	m_currentRandomAccessPoint = random_access_point_sample_no;
	m_RandomAccessPointsOnly = direction;
	if (direction < 0)
	{
		m_video_stss.rewind32 (&error);
		ASSERT (error == 0);
	}

	RMuint32 frame_no;
	RMuint32 i;
	RMuint32 first_chunk, samples_per_chunk, sample_description_index;
	RMuint32 prev_samples_per_chunk, prev_first_chunk, frame_incr;

	frame_no = 0;
	m_video_sample_count = frame_no;
	m_video_stsc.reset ();
	prev_first_chunk = m_video_stsc.get32 (&error);
	m_video_chunk_index = prev_first_chunk;
	m_video_stsc.advance32 (&error);
	ASSERT (error == 0);
	prev_samples_per_chunk = m_video_stsc.get32 (&error);
	m_video_samples_per_chunk = prev_samples_per_chunk;
	m_samples_per_chunk = m_video_samples_per_chunk;
	m_video_stsc.advance32 (&error);
	ASSERT (error == 0);
	sample_description_index = m_video_stsc.get32 (&error);
	m_video_stsc.advance32 (&error);
	ASSERT (error == 0);

	for (i=0; i<m_video_stsc.entrycount (); i++)
	{		 
		first_chunk = m_video_stsc.get32 (&error);
		frame_incr = prev_samples_per_chunk * (first_chunk - prev_first_chunk);
		if ((frame_no + frame_incr) >= m_currentRandomAccessPoint)
			break;
		frame_no += frame_incr;
		m_video_stsc.advance32 (&error);
		ASSERT (error == 0);
		samples_per_chunk = m_video_stsc.get32 (&error);
		m_video_stsc.advance32 (&error);
		ASSERT (error == 0);
		sample_description_index = m_video_stsc.get32 (&error);
		m_video_stsc.advance32 (&error);
		ASSERT (error == 0);		
		prev_first_chunk = first_chunk;
		prev_samples_per_chunk = samples_per_chunk;
	}
	m_video_sample_count = frame_no;
	m_video_chunk_index = prev_first_chunk;
	m_video_samples_per_chunk = prev_samples_per_chunk;
	m_samples_per_chunk = prev_samples_per_chunk;

	if (m_videoDSILength)
		m_CallbackTable.putDSI (0, 0, m_videoDSI, m_videoDSILength, (RMint64)m_tracktimescales[m_videoTrackIndex]/2, m_CallbackTable.context);

	m_next_next_chunk = 0;
	m_pRandomAccessData = 0;
	m_State = MP4_RANDOM_ACCESS_ONLY;
	m_RandomAccessState = PROCESS_CURRENT_RANDOM_ACCESS_POINT;
	return MP4_ERROR_NO_ERROR;
}

RMint32 MP4Demux::GetNumberOfAudioStreams ()
{
	return m_naudioStreams;
}

RMint32 MP4Demux::GetNumberOfSubpictureStreams ()
{
	return m_nspStreams;
}

MP4_ERROR MP4Demux::videoSeek (RMuint64 position_in_ms, RMuint32 *fileoffset)
{
	m_video_stco.reset ();
	m_video_stts.reset ();
	m_video_ctts.reset ();
	m_video_stsz.reset ();
	m_video_stsc.reset ();
	m_video_stss.reset ();
	m_video_chunk_index = 1;
	m_video_samples_per_chunk = 1;
	m_video_sample_count = 0;
	m_video_stts_sample_delta_count = 0;
	m_video_ctts_sample_delta_count = 0;
	m_video_ctts_sample_delta = 0;
	m_videoTime = 0;

	m_samples_per_chunk = 0;
	m_chunkFlags = 0;
	m_total_chunk_size = 0;

	RMuint32 frame_no, chunk_offset;
	RMuint32 error;
	RMuint32 i, j;
	RMuint32 first_chunk, samples_per_chunk, sample_description_index;
	RMuint32 prev_samples_per_chunk, prev_first_chunk, frame_incr;
	RMuint32 sample_delta, sync_sample;
	RMuint64 time;

	// seek to the correct stsc and stts table position
	time = position_in_ms * m_tracktimescales[m_videoTrackIndex] / 1000;
	m_video_stts_sample_delta_count = 0;
	frame_no = 0;
	prev_first_chunk = 1;
	sample_delta = 0;
	sync_sample = 0;
	ASSERT (m_video_stsc.IsInitialized ());
	for (i=0; i<m_video_stsc.entrycount (); i++)
	{		 
		first_chunk = m_video_stsc.get32 (&error);
		m_video_stsc.advance32 (&error);
		ASSERT (error == 0);
		samples_per_chunk = m_video_stsc.get32 (&error);
		m_video_stsc.advance32 (&error);
		ASSERT (error == 0);
		sample_description_index = m_video_stsc.get32 (&error);
		m_video_stsc.advance32 (&error);
		ASSERT (error == 0);
		frame_incr = prev_samples_per_chunk * (first_chunk - prev_first_chunk);
		frame_no += frame_incr;
		// calculate the time of this frame to compare it to the requested time
		if (m_video_stts.entrycount () == 0)
		{
			if (m_video_stts_sample_delta_count == 0)
			{
				m_video_stts_sample_delta_count = m_video_stts.get32 (&error);
				m_video_stts.advance32 (&error);
				ASSERT (error == 0);
				sample_delta = m_video_stts.get32 (&error);
				m_video_stts.advance32 (&error);
				ASSERT (error == 0);
				ASSERT (sample_delta);
			}
			ASSERT (sample_delta);
			m_videoTime += (frame_incr * sample_delta);
		}
		else
		{
			for (j=0; j<frame_incr; j++)
			{
				if (m_video_stts_sample_delta_count == 0)
				{
					m_video_stts_sample_delta_count = m_video_stts.get32 (&error);
					m_video_stts.advance32 (&error);
					ASSERT (error == 0);
					sample_delta = m_video_stts.get32 (&error);
					m_video_stts.advance32 (&error);
					ASSERT (error == 0);
					ASSERT (sample_delta);
				}
				m_videoTime += sample_delta;
				m_video_stts_sample_delta_count--;
			}
		}
		if (m_videoTime >= time)
		{
			m_video_stsc.seek (i * 12);
			m_video_samples_per_chunk = samples_per_chunk;
			m_video_chunk_index = first_chunk;
			m_video_sample_count = frame_no;
			break;
		}
		prev_first_chunk = first_chunk;
		prev_samples_per_chunk = samples_per_chunk;
	}
	ASSERT (i < m_video_stsc.entrycount ());
	// seek to the correct ctts table position
	if (m_video_ctts.IsInitialized ())
	{
		RMuint32 i, j, framecnt, n;
		framecnt = 0;
		error = 0;
		for (i=0; i<m_video_ctts.entrycount (); i++)
		{
			n = m_video_ctts_sample_delta_count = m_video_ctts.get32 (&error);
			m_video_ctts.advance32 (&error);
			ASSERT (error == 0);
			m_video_ctts_sample_delta = m_video_ctts.get32 (&error);
			m_video_ctts.advance32 (&error);
			ASSERT (error == 0);			
			for (j=0; j<n; j++)
			{					
				m_video_ctts_sample_delta_count--;
				framecnt++;
				if (framecnt == frame_no)
				{
					goto video_ctts_table_finished;
				}					
			}				
		}
		ASSERT (0);
	}
video_ctts_table_finished:
	// seek to the correct stsz table position
	ASSERT (m_video_stsz.IsInitialized ());
	if (m_video_sample_size == 0)
		m_video_stsz.seek (frame_no * 4);
	// seek to the correct stco table offset
	ASSERT (m_video_stco.IsInitialized ());
	ASSERT (m_video_chunk_index);
	m_video_stco.seek ((m_video_chunk_index - 1) * 4);
	chunk_offset = m_video_stco.get32 (&error);

	DEBUGMSG (1, ("-------------------------\n"));
	DEBUGMSG (1, ("VIDEO SEEK (fast method):\n"));
	DEBUGMSG (1, ("m_video_chunk_index: %lu\n", m_video_chunk_index));
	DEBUGMSG (1, ("m_video_samples_per_chunk: %lu\n", m_video_samples_per_chunk));
	DEBUGMSG (1, ("m_video_sample_count: %lu\n", m_video_sample_count));
	DEBUGMSG (1, ("m_video_stts_sample_delta_count: %lu\n", m_video_stts_sample_delta_count));
	DEBUGMSG (1, ("m_video_ctts_sample_delta_count: %lu\n", m_video_ctts_sample_delta_count));
	DEBUGMSG (1, ("m_video_ctts_sample_delta: %lu\n", m_video_ctts_sample_delta));
	DEBUGMSG (1, ("m_videoTime: %lu\n", m_videoTime));
	DEBUGMSG (1, ("stco: %lu, %lu\n", m_video_stco.tell (), m_video_stco.get32 (&error)));
	DEBUGMSG (1, ("stts: %lu, %lu\n", m_video_stts.tell (), m_video_stts.get32 (&error)));
	DEBUGMSG (1, ("ctts: %lu, %lu\n", m_video_ctts.tell (), m_video_ctts.get32 (&error)));
	DEBUGMSG (1, ("stsz: %lu, %lu\n", m_video_stsz.tell (), m_video_stsz.get32 (&error)));
	DEBUGMSG (1, ("stsc: %lu, %lu\n", m_video_stsc.tell (), m_video_stsc.get32 (&error)));
	DEBUGMSG (1, ("-------------------------\n"));

	*fileoffset = chunk_offset;
	return MP4_ERROR_NO_ERROR;

	// the code below is a brute force methos to do seek,
	// it is really slow, but good for debugging if you want to check the results of
	// the above code

	m_video_stco.reset ();
	m_video_stts.reset ();
	m_video_ctts.reset ();
	m_video_stsz.reset ();
	m_video_stsc.reset ();
	m_video_chunk_index = 1;
	m_video_samples_per_chunk = 1;
	m_video_sample_count = 0;
	m_video_stts_sample_delta_count = 0;
	m_video_ctts_sample_delta_count = 0;
	m_video_ctts_sample_delta = 0;
	m_videoTime = 0;

	m_samples_per_chunk = 0;
	m_chunkFlags = 0;
	m_total_chunk_size = 0;

	RMuint32 chunk_length, first_entry;
	RMuint32 percent0, percent1;
	percent0 = (RMuint32)-1;

	while (1)
	{
		chunk_offset = m_video_stco.get32 (&error);
		if (error)
		{
			return MP4_ERROR_NOT_ALLOWED;
		}
		RMuint64 time = m_videoTime * 1000 / m_tracktimescales[m_videoTrackIndex];
		if (time >= position_in_ms)
		{
			DEBUGMSG (1, ("-------------------------\n"));
			DEBUGMSG (1, ("VIDEO SEEK (slow method):\n"));
			DEBUGMSG (1, ("m_video_chunk_index: %lu\n", m_video_chunk_index));
			DEBUGMSG (1, ("m_video_samples_per_chunk: %lu\n", m_video_samples_per_chunk));
			DEBUGMSG (1, ("m_video_sample_count: %lu\n", m_video_sample_count));
			DEBUGMSG (1, ("m_video_stts_sample_delta_count: %lu\n", m_video_stts_sample_delta_count));
			DEBUGMSG (1, ("m_video_ctts_sample_delta_count: %lu\n", m_video_ctts_sample_delta_count));
			DEBUGMSG (1, ("m_video_ctts_sample_delta: %lu\n", m_video_ctts_sample_delta));
			DEBUGMSG (1, ("m_videoTime: %lu\n", m_videoTime));
			DEBUGMSG (1, ("stco: %lu, %lu\n", m_video_stco.tell (), m_video_stco.get32 (&error)));
			DEBUGMSG (1, ("stts: %lu, %lu\n", m_video_stts.tell (), m_video_stts.get32 (&error)));
			DEBUGMSG (1, ("ctts: %lu, %lu\n", m_video_ctts.tell (), m_video_ctts.get32 (&error)));
			DEBUGMSG (1, ("stsz: %lu, %lu\n", m_video_stsz.tell (), m_video_stsz.get32 (&error)));
			DEBUGMSG (1, ("stsc: %lu, %lu\n", m_video_stsc.tell (), m_video_stsc.get32 (&error)));
			*fileoffset = chunk_offset;
			return MP4_ERROR_NO_ERROR;				
		}
		m_video_stco.advance32 (&error);
		first_entry = m_video_stsc.get32 (&error); 
		if (m_video_chunk_index == first_entry)
		{
			m_video_stsc.advance32 (&error);
			m_video_samples_per_chunk = m_video_stsc.get32 (&error);
			m_video_stsc.advance32 (&error);
			m_video_stsc.get32 (&error);
			m_video_stsc.advance32 (&error);
		}
		m_video_chunk_index++;
		for (i=0; ((RMint32)i)<m_video_samples_per_chunk; i++)
		{
			if (m_video_sample_size == 0)
			{
				chunk_length = m_video_stsz.get32 (&error);
				ASSERT (error == 0);
				m_video_stsz.advance32 (&error);
			}
			m_video_sample_count++;
			if (m_video_stts_sample_delta_count == 0)
			{
				m_video_stts_sample_delta_count = m_video_stts.get32 (&error);
				ASSERT (error == 0);
				m_video_stts.advance32 (&error);
				m_video_stts_sample_delta = m_video_stts.get32 (&error);
				ASSERT (error == 0);
				m_video_stts.advance32 (&error);
			}
			m_video_stts_sample_delta_count--;
			m_videoTime += m_video_stts_sample_delta;
			DEBUGMSG (0, ("videoSeek: m_videoTime = %d\n", (int)m_videoTime));
			if (m_video_ctts.IsInitialized ())
			{
				if (m_video_ctts_sample_delta_count == 0)
				{
					m_video_ctts_sample_delta_count = m_video_ctts.get32 (&error);
					ASSERT (error == 0);
					m_video_ctts.advance32 (&error);
					m_video_ctts_sample_delta = m_video_ctts.get32 (&error);
					ASSERT (error == 0);
					m_video_ctts.advance32 (&error);
				}
				m_video_ctts_sample_delta_count--;
			}
		}		
		percent1 = (RMuint32)(time * 50 / position_in_ms);
		if (percent1 != percent0)
			m_CallbackTable.info (MP4_MSG_SEEK_PERCENTAGE, (void *)percent1, m_CallbackTable.context);
		percent0 = percent1;
	}
	ASSERT (0);
	return MP4_ERROR_NO_ERROR;
}

MP4_ERROR MP4Demux::audioSeek (RMuint64 position_in_ms, RMuint32 *fileoffset, RMuint32 isByteOffset)
{
	RMuint32 chunk_offset, chunk_length, first_entry;
	RMuint32 error;
	RMuint32 i, ai;
	RMuint32 audiooffset[2];

	m_samples_per_chunk = 0;
	m_total_chunk_size = 0;
	m_chunkFlags = 0;

///////////////////////////
	for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
	{
		m_audio_stco[i].reset ();
		m_audio_stts[i].reset ();
		m_audio_ctts[i].reset ();
		m_audio_stsz[i].reset ();
		m_audio_stsc[i].reset ();
		m_audio_chunk_index[i] = 1;
		m_audio_samples_per_chunk[i] = 1;
		m_audio_sample_count[i] = 0;
		m_audio_stts_sample_delta_count[i] = 0;
		m_audio_ctts_sample_delta_count[i] = 0;
		m_audio_ctts_sample_delta[i] = 0;
		m_audioTime[i] = 0;
		audiooffset[i] = 0;
	}

	RMuint32 frame_no;
	RMuint32 j;
	RMuint32 first_chunk, samples_per_chunk, sample_description_index;
	RMuint32 prev_samples_per_chunk, prev_first_chunk, frame_incr;
	RMuint32 sample_delta;

	if (isByteOffset == 0)
		goto audio_seek_slow_way;

	for (ai=0; ai<(RMuint32)m_naudioStreams; ai++)
	{
		// seek to the correct offset in the stsc, stts, stco tables
		RMuint32 istart, imiddle, iend;
		RMuint32 current_offset;
		istart = 0;
		iend = m_audio_stco[ai].entrycount ();
		imiddle = (istart + iend) / 2;
		while (1)
		{			
			m_audio_stco[ai].seek (imiddle * 4);
			current_offset = m_audio_stco[ai].get32 (&error);
			if ((imiddle == istart) || (imiddle == iend) || (current_offset == position_in_ms))
			{				
				if (current_offset == position_in_ms)
				{
					m_audio_chunk_index[ai] = imiddle + 1;
				}
				else
				{
					m_audio_stco[ai].seek (istart * 4);
					current_offset = m_audio_stco[ai].get32 (&error);
					if (current_offset > position_in_ms)
					{
						m_audio_chunk_index[ai] = istart + 1;
					}
					else
					{
						m_audio_stco[ai].seek (iend * 4);
						m_audio_chunk_index[ai] = iend + 1;
					}
				}
				break;
			}
			if (position_in_ms > current_offset)
				istart = imiddle;
			else 
				iend = imiddle;
			imiddle = (istart + iend) / 2;
		}

		m_audio_stts_sample_delta_count[ai] = 0;
		frame_no = 0;
		prev_first_chunk = 1;
		sample_delta = 0;
		m_audioTime[ai] = 0;
		ASSERT (m_audio_stsc[ai].IsInitialized ());
		for (i=0; i<m_audio_stsc[ai].entrycount (); i++)
		{		 
			first_chunk = m_audio_stsc[ai].get32 (&error);
			m_audio_stsc[ai].advance32 (&error);
			ASSERT (error == 0);
			samples_per_chunk = m_audio_stsc[ai].get32 (&error);
			m_audio_stsc[ai].advance32 (&error);
			ASSERT (error == 0);
			sample_description_index = m_audio_stsc[ai].get32 (&error);
			m_audio_stsc[ai].advance32 (&error);
			ASSERT (error == 0);
			frame_incr = prev_samples_per_chunk * (first_chunk - prev_first_chunk);
			frame_no += frame_incr;
			// calculate the time of this frame to compare it to the requested time
			if (m_audio_stts[ai].entrycount () == 0)
			{
				if (m_audio_stts_sample_delta_count[ai] == 0)
				{
					m_audio_stts_sample_delta_count[ai] = m_audio_stts[ai].get32 (&error);
					m_audio_stts[ai].advance32 (&error);
					ASSERT (error == 0);
					sample_delta = m_audio_stts[ai].get32 (&error);
					m_audio_stts[ai].advance32 (&error);
					ASSERT (error == 0);
					ASSERT (sample_delta);
				}
				ASSERT (sample_delta);
				m_audioTime[ai] += (frame_incr * sample_delta);
			}
			else
			{
				for (j=0; j<frame_incr; j++)
				{
					if (m_audio_stts_sample_delta_count[ai] == 0)
					{
						m_audio_stts_sample_delta_count[ai] = m_audio_stts[ai].get32 (&error);
						m_audio_stts[ai].advance32 (&error);
						ASSERT (error == 0);
						sample_delta = m_audio_stts[ai].get32 (&error);
						m_audio_stts[ai].advance32 (&error);
						ASSERT (error == 0);
						ASSERT (sample_delta);
					}
					m_audioTime[ai] += sample_delta;
					m_audio_stts_sample_delta_count[ai]--;
				}
			}
			if (first_chunk >= m_audio_chunk_index[ai])
			{
				m_audio_stsc[ai].seek (i * 12);
				m_audio_samples_per_chunk[ai] = samples_per_chunk;
				m_audio_chunk_index[ai] = first_chunk;
				m_audio_sample_count[ai] = frame_no;						
				break;
			}
			prev_first_chunk = first_chunk;
			prev_samples_per_chunk = samples_per_chunk;
		}
		// seek to the correct ctts table position
		if (m_audio_ctts[ai].IsInitialized ())
		{
			RMuint32 i, j, framecnt, n;
			framecnt = 0;
			error = 0;
			for (i=0; i<m_audio_ctts[ai].entrycount (); i++)
			{
				n = m_audio_ctts_sample_delta_count[ai] = m_audio_ctts[ai].get32 (&error);
				m_audio_ctts[ai].advance32 (&error);
				ASSERT (error == 0);
				m_audio_ctts_sample_delta[ai] = m_audio_ctts[ai].get32 (&error);
				m_audio_ctts[ai].advance32 (&error);
				ASSERT (error == 0);
				for (j=0; j<n; j++)
				{					
					m_audio_ctts_sample_delta_count[ai]--;
					framecnt++;
					if (framecnt == m_audio_sample_count[ai])
					{
						goto audio_ctts_table_finished;
					}					
				}				
			}
		}
audio_ctts_table_finished:
		// seek to the correct stsz table position
		ASSERT (m_audio_stsz[ai].IsInitialized ());
		if (m_audio_sample_size[ai] == 0)
			m_audio_stsz[ai].seek (m_audio_sample_count[ai] * 4);
		m_audio_stco[ai].seek ((m_audio_chunk_index[ai] - 1) * 4);
		audiooffset[ai] = m_audio_stco[ai].get32 (&error);

		DEBUGMSG (1, ("-------------------------\n"));
		DEBUGMSG (1, ("AUDIO SEEK (fast method): (%d)\n", (int)ai));
		DEBUGMSG (1, ("m_audio_chunk_index: %lu\n", m_audio_chunk_index[ai]));
		DEBUGMSG (1, ("m_audio_samples_per_chunk: %lu\n", m_audio_samples_per_chunk[ai]));
		DEBUGMSG (1, ("m_audio_sample_count: %lu\n", m_audio_sample_count[ai]));
		DEBUGMSG (1, ("m_audio_stts_sample_delta_count: %lu\n", m_audio_stts_sample_delta_count[ai]));
		DEBUGMSG (1, ("m_audio_ctts_sample_delta_count: %lu\n", m_audio_ctts_sample_delta_count[ai]));
		DEBUGMSG (1, ("m_audio_ctts_sample_delta: %lu\n", m_audio_ctts_sample_delta[ai]));
		DEBUGMSG (1, ("m_audioTime: %lu\n", m_audioTime[ai]));
		DEBUGMSG (1, ("stco: %lu, %lu\n", m_audio_stco[ai].tell (), m_audio_stco[ai].get32 (&error)));
		DEBUGMSG (1, ("stts: %lu, %lu\n", m_audio_stts[ai].tell (), m_audio_stts[ai].get32 (&error)));
		DEBUGMSG (1, ("ctts: %lu, %lu\n", m_audio_ctts[ai].tell (), m_audio_ctts[ai].get32 (&error)));
		DEBUGMSG (1, ("stsz: %lu, %lu\n", m_audio_stsz[ai].tell (), m_audio_stsz[ai].get32 (&error)));
		DEBUGMSG (1, ("stsc: %lu, %lu\n", m_audio_stsc[ai].tell (), m_audio_stsc[ai].get32 (&error)));
		DEBUGMSG (1, ("-------------------------\n"));
	}

	if (audiooffset[0] && audiooffset[1])
		*fileoffset = MP4_MIN (audiooffset[0], audiooffset[1]);
	else if (audiooffset[0])
		*fileoffset = audiooffset[0];
	else
		*fileoffset = audiooffset[1];
	return MP4_ERROR_NO_ERROR;

	for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
	{
		m_audio_stco[i].reset ();
		m_audio_stts[i].reset ();
		m_audio_ctts[i].reset ();
		m_audio_stsz[i].reset ();
		m_audio_stsc[i].reset ();
		m_audio_chunk_index[i] = 1;
		m_audio_samples_per_chunk[i] = 1;
		m_audio_sample_count[i] = 0;
		m_audio_stts_sample_delta_count[i] = 0;
		m_audio_ctts_sample_delta_count[i] = 0;
		m_audio_ctts_sample_delta[i] = 0;
		m_audioTime[i] = 0;
		audiooffset[i] = 0;
	}

	// the code below is a brute force methos to do seek,
	// it is really slow, but good for debugging if you want to check the results of
	// the above code
audio_seek_slow_way:
	RMuint32 percent0, percent1;
	percent0 = (RMuint32)-1;
	RMint32 percent_complete = 50;
	audiooffset[0] = audiooffset[1] = 0;
	for (ai=0; ai<(RMuint32)m_naudioStreams; ai++)
	{
		RMint32 searching;
		searching = 1;
		while (searching)
		{
			chunk_offset = m_audio_stco[ai].get32 (&error);
			if (error)
			{
				return MP4_ERROR_NOT_ALLOWED;
			}
			if (isByteOffset)
			{
				if (chunk_offset >= (RMuint32)position_in_ms)
				{
					audiooffset[ai] = chunk_offset;
					searching = 0;
					percent_complete += 25;
					DEBUGMSG (1, ("-------------------------\n"));
					DEBUGMSG (1, ("AUDIO SEEK (slow method, byte): (%d)\n", (int)ai));
					DEBUGMSG (1, ("m_audio_chunk_index: %lu\n", m_audio_chunk_index[ai]));
					DEBUGMSG (1, ("m_audio_samples_per_chunk: %lu\n", m_audio_samples_per_chunk[ai]));
					DEBUGMSG (1, ("m_audio_sample_count: %lu\n", m_audio_sample_count[ai]));
					DEBUGMSG (1, ("m_audio_stts_sample_delta_count: %lu\n", m_audio_stts_sample_delta_count[ai]));
					DEBUGMSG (1, ("m_audio_ctts_sample_delta_count: %lu\n", m_audio_ctts_sample_delta_count[ai]));
					DEBUGMSG (1, ("m_audio_ctts_sample_delta: %lu\n", m_audio_ctts_sample_delta[ai]));
					DEBUGMSG (1, ("m_audioTime: %lu\n", m_audioTime[ai]));
					DEBUGMSG (1, ("stco: %lu, %lu\n", m_audio_stco[ai].tell (), m_audio_stco[ai].get32 (&error)));
					DEBUGMSG (1, ("stts: %lu, %lu\n", m_audio_stts[ai].tell (), m_audio_stts[ai].get32 (&error)));
					DEBUGMSG (1, ("ctts: %lu, %lu\n", m_audio_ctts[ai].tell (), m_audio_ctts[ai].get32 (&error)));
					DEBUGMSG (1, ("stsz: %lu, %lu\n", m_audio_stsz[ai].tell (), m_audio_stsz[ai].get32 (&error)));
					DEBUGMSG (1, ("stsc: %lu, %lu\n", m_audio_stsc[ai].tell (), m_audio_stsc[ai].get32 (&error)));
					DEBUGMSG (1, ("------------------------------\n", (int)ai));
					continue;
				}
				percent1 = (RMuint32)((RMint64)chunk_offset * 25 / (RMint64)position_in_ms) + percent_complete;
				if (percent1 != percent0)
					m_CallbackTable.info (MP4_MSG_SEEK_PERCENTAGE, (void *)percent1, m_CallbackTable.context);
				percent0 = percent1;
			}
			else
			{
				RMuint64 time = m_audioTime[ai] * 1000 / m_tracktimescales[m_audioTrackIndex[m_currentAudioTrack]];
				if (time >= position_in_ms)
				{
					audiooffset[ai] = chunk_offset;
					searching = 0;
					percent_complete += 25;
					DEBUGMSG (1, ("------------------------------\n"));
					DEBUGMSG (1, ("AUDIO SEEK (slow method, time): (%d)\n", (int)ai));
					DEBUGMSG (1, ("m_audio_chunk_index: %lu\n", m_audio_chunk_index[ai]));
					DEBUGMSG (1, ("m_audio_samples_per_chunk: %lu\n", m_audio_samples_per_chunk[ai]));
					DEBUGMSG (1, ("m_audio_sample_count: %lu\n", m_audio_sample_count[ai]));
					DEBUGMSG (1, ("m_audio_stts_sample_delta_count: %lu\n", m_audio_stts_sample_delta_count[ai]));
					DEBUGMSG (1, ("m_audio_ctts_sample_delta_count: %lu\n", m_audio_ctts_sample_delta_count[ai]));
					DEBUGMSG (1, ("m_audio_ctts_sample_delta: %lu\n", m_audio_ctts_sample_delta[ai]));
					DEBUGMSG (1, ("m_audioTime: %lu\n", m_audioTime[ai]));
					DEBUGMSG (1, ("stco: %lu, %lu\n", m_audio_stco[ai].tell (), m_audio_stco[ai].get32 (&error)));
					DEBUGMSG (1, ("stts: %lu, %lu\n", m_audio_stts[ai].tell (), m_audio_stts[ai].get32 (&error)));
					DEBUGMSG (1, ("ctts: %lu, %lu\n", m_audio_ctts[ai].tell (), m_audio_ctts[ai].get32 (&error)));
					DEBUGMSG (1, ("stsz: %lu, %lu\n", m_audio_stsz[ai].tell (), m_audio_stsz[ai].get32 (&error)));
					DEBUGMSG (1, ("stsc: %lu, %lu\n", m_audio_stsc[ai].tell (), m_audio_stsc[ai].get32 (&error)));
					DEBUGMSG (1, ("------------------------------\n"));
					continue;
				}
				percent1 = (RMuint32)((RMint64)time * 25 / (RMint64)position_in_ms) + percent_complete;
				if (percent1 != percent0)
					m_CallbackTable.info (MP4_MSG_SEEK_PERCENTAGE, (void *)percent1, m_CallbackTable.context);
				percent0 = percent1;
			}
			m_audio_stco[ai].advance32 (&error);
			first_entry = m_audio_stsc[ai].get32 (&error); 
			if (m_audio_chunk_index[ai] == first_entry)
			{
				m_audio_stsc[ai].advance32 (&error);
				m_audio_samples_per_chunk[ai] = m_audio_stsc[ai].get32 (&error);
				m_audio_stsc[ai].advance32 (&error);
				m_audio_stsc[ai].get32 (&error);
				m_audio_stsc[ai].advance32 (&error);
			}
			m_audio_chunk_index[ai]++;
			for (i=0; i<(RMuint32)m_audio_samples_per_chunk[ai]; i++)
			{
				if (m_audio_sample_size[ai] == 0)
				{
					chunk_length = m_audio_stsz[ai].get32 (&error);
					ASSERT (error == 0);
					m_audio_stsz[ai].advance32 (&error);
				}
				m_audio_sample_count[ai]++;
				if (m_audio_stts_sample_delta_count[ai] == 0)
				{
					m_audio_stts_sample_delta_count[ai] = m_audio_stts[ai].get32 (&error);
					ASSERT (error == 0);
					m_audio_stts[ai].advance32 (&error);
					m_audio_stts_sample_delta[ai] = m_audio_stts[ai].get32 (&error);
					ASSERT (error == 0);
					m_audio_stts[ai].advance32 (&error);
				}
				m_audio_stts_sample_delta_count[ai]--;
				m_audioTime[ai] += m_audio_stts_sample_delta[ai];
				if (m_audio_ctts[ai].IsInitialized ())
				{
					if (m_audio_ctts_sample_delta_count[ai] == 0)
					{
						m_audio_ctts_sample_delta_count[ai] = m_audio_ctts[ai].get32 (&error);
						ASSERT (error == 0);
						m_audio_ctts[ai].advance32 (&error);
						m_audio_ctts_sample_delta[ai] = m_audio_ctts[ai].get32 (&error);
						ASSERT (error == 0);
						m_audio_ctts[ai].advance32 (&error);
					}
					m_audio_ctts_sample_delta_count[ai]--;
				}
			}
		}
	}
	if (audiooffset[0] && audiooffset[1])
		*fileoffset = MP4_MIN (audiooffset[0], audiooffset[1]);
	else if (audiooffset[0])
		*fileoffset = audiooffset[0];
	else
		*fileoffset = audiooffset[1];
	return MP4_ERROR_NO_ERROR;
}

MP4_ERROR MP4Demux::subpiSeek (RMuint64 position_in_ms, RMuint32 *fileoffset, RMuint32 isByteOffset)
{
	RMuint32 chunk_offset, chunk_length, first_entry;
	RMuint32 error;
	RMint32 i, si;
	RMuint32 spoffset[2];

	for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
	{
		m_subpi_stco[i].reset ();
		m_subpi_stts[i].reset ();
		m_subpi_stsz[i].reset ();
		m_subpi_stsc[i].reset ();
		m_subpi_chunk_index[i] = 1;
		m_subpi_samples_per_chunk[i] = 1;
		m_subpi_sample_count[i] = 0;
		m_subpi_stts_sample_delta_count[i] = 0;
		m_subpi_ctts_sample_delta_count[i] = 0;
		m_subpi_ctts_sample_delta[i] = 0;
		m_subpiTime[i] = 0;
		spoffset[i] = 0;
	}
	m_chunkFlags = 0;
	m_total_chunk_size = 0;
	m_samples_per_chunk = 0;

	spoffset[0] = spoffset[1] = 0;
	for (si=0; si<m_nspStreams; si++)
	{
		RMint32 searching;
		searching = 1;
		while (searching)
		{
			chunk_offset = m_subpi_stco[si].get32 (&error);
			if (error)
			{
				return MP4_ERROR_NOT_ALLOWED;
			}
			if (isByteOffset)
			{
				if (chunk_offset >= (RMuint32)position_in_ms)
				{
					spoffset[si] = chunk_offset;
					searching = 0;
					DEBUGMSG (1, ("------------------------------\n"));
					DEBUGMSG (1, ("SUBPIC SEEK (slow method, byte): (%d)\n", (int)si));
					DEBUGMSG (1, ("m_subpi_chunk_index: %lu\n", m_subpi_chunk_index[si]));
					DEBUGMSG (1, ("m_subpi_samples_per_chunk: %lu\n", m_subpi_samples_per_chunk[si]));
					DEBUGMSG (1, ("m_subpi_sample_count: %lu\n", m_subpi_sample_count[si]));
					DEBUGMSG (1, ("m_subpi_stts_sample_delta_count: %lu\n", m_subpi_stts_sample_delta_count[si]));
					DEBUGMSG (1, ("m_subpi_ctts_sample_delta_count: %lu\n", m_subpi_ctts_sample_delta_count[si]));
					DEBUGMSG (1, ("m_subpi_ctts_sample_delta: %lu\n", m_subpi_ctts_sample_delta[si]));
					DEBUGMSG (1, ("m_subpiTime: %lu\n", m_subpiTime[si]));
					DEBUGMSG (1, ("stco: %lu, %lu\n", m_subpi_stco[si].tell (), m_subpi_stco[si].get32 (&error)));
					DEBUGMSG (1, ("stts: %lu, %lu\n", m_subpi_stts[si].tell (), m_subpi_stts[si].get32 (&error)));
					DEBUGMSG (1, ("ctts: %lu, %lu\n", m_subpi_ctts[si].tell (), m_subpi_ctts[si].get32 (&error)));
					DEBUGMSG (1, ("stsz: %lu, %lu\n", m_subpi_stsz[si].tell (), m_subpi_stsz[si].get32 (&error)));
					DEBUGMSG (1, ("stsc: %lu, %lu\n", m_subpi_stsc[si].tell (), m_subpi_stsc[si].get32 (&error)));
					DEBUGMSG (1, ("------------------------------\n"));
					continue;
				}
			}
			else
			{
				RMuint64 time = m_subpiTime[si] * 1000 / m_tracktimescales[m_subpiTrackIndex[m_currentSubpiTrack]];
				if (time >= position_in_ms)
				{
					spoffset[si] = chunk_offset;
					searching = 0;
					DEBUGMSG (1, ("------------------------------\n"));
					DEBUGMSG (1, ("SUBPIC SEEK (slow method, time): (%d)\n", (int)si));
					DEBUGMSG (1, ("m_subpi_chunk_index: %lu\n", m_subpi_chunk_index[si]));
					DEBUGMSG (1, ("m_subpi_samples_per_chunk: %lu\n", m_subpi_samples_per_chunk[si]));
					DEBUGMSG (1, ("m_subpi_sample_count: %lu\n", m_subpi_sample_count[si]));
					DEBUGMSG (1, ("m_subpi_stts_sample_delta_count: %lu\n", m_subpi_stts_sample_delta_count[si]));
					DEBUGMSG (1, ("m_subpi_ctts_sample_delta_count: %lu\n", m_subpi_ctts_sample_delta_count[si]));
					DEBUGMSG (1, ("m_subpi_ctts_sample_delta: %lu\n", m_subpi_ctts_sample_delta[si]));
					DEBUGMSG (1, ("m_subpiTime: %lu\n", m_audioTime[si]));
					DEBUGMSG (1, ("stco: %lu, %lu\n", m_subpi_stco[si].tell (), m_subpi_stco[si].get32 (&error)));
					DEBUGMSG (1, ("stts: %lu, %lu\n", m_subpi_stts[si].tell (), m_subpi_stts[si].get32 (&error)));
					DEBUGMSG (1, ("ctts: %lu, %lu\n", m_subpi_ctts[si].tell (), m_subpi_ctts[si].get32 (&error)));
					DEBUGMSG (1, ("stsz: %lu, %lu\n", m_subpi_stsz[si].tell (), m_subpi_stsz[si].get32 (&error)));
					DEBUGMSG (1, ("stsc: %lu, %lu\n", m_subpi_stsc[si].tell (), m_subpi_stsc[si].get32 (&error)));
					DEBUGMSG (1, ("------------------------------\n"));
					continue;
				}
			}
			m_subpi_stco[si].advance32 (&error);
			first_entry = m_subpi_stsc[si].get32 (&error); 
			if (m_subpi_chunk_index[si] == first_entry)
			{
				m_subpi_stsc[si].advance32 (&error);
				m_subpi_samples_per_chunk[si] = m_subpi_stsc[si].get32 (&error);
				m_subpi_stsc[si].advance32 (&error);
				m_subpi_stsc[si].get32 (&error);
				m_subpi_stsc[si].advance32 (&error);
			}
			m_subpi_chunk_index[si]++;
			for (i=0; i<m_subpi_samples_per_chunk[si]; i++)
			{
				if (m_subpi_sample_size[si] == 0)
				{
					chunk_length = m_subpi_stsz[si].get32 (&error);
					ASSERT (error == 0);
					m_subpi_stsz[si].advance32 (&error);
				}
				m_subpi_sample_count[si]++;
				if (m_subpi_stts_sample_delta_count[si] == 0)
				{
					m_subpi_stts_sample_delta_count[si] = m_subpi_stts[si].get32 (&error);
					ASSERT (error == 0);
					m_subpi_stts[si].advance32 (&error);
					m_subpi_stts_sample_delta[si] = m_subpi_stts[si].get32 (&error);
					ASSERT (error == 0);
					m_subpi_stts[si].advance32 (&error);
				}
				m_subpi_stts_sample_delta_count[si]--;
				m_subpiTime[si] += m_subpi_stts_sample_delta[si];
				if (m_subpi_ctts[si].IsInitialized ())
				{
					if (m_subpi_ctts_sample_delta_count[si] == 0)
					{
						m_subpi_ctts_sample_delta_count[si] = m_subpi_ctts[si].get32 (&error);
						ASSERT (error == 0);
						m_subpi_ctts[si].advance32 (&error);
						m_subpi_ctts_sample_delta[si] = m_subpi_ctts[si].get32 (&error);
						ASSERT (error == 0);
						m_subpi_ctts[si].advance32 (&error);
					}
					m_subpi_ctts_sample_delta_count[si]--;
				}
			}
		}
	}
	if (spoffset[0] && spoffset[1])
		*fileoffset = MP4_MIN (spoffset[0], spoffset[1]);
	else if (spoffset[0])
		*fileoffset = spoffset[0];
	else
		*fileoffset = spoffset[1];
	return MP4_ERROR_NO_ERROR;
}

////////////////////////////////////////////////////////////////////////
static RMint8 verify_character (RMint8 c)
{
	if ((c >= 0x20) && (c <= 0x7f))
		return c;
	else
		return '?';
}

RMuint32 MP4Demux::NextBox (RMuint64 stop_position)
{
	RMuint32 error;

	if (stop_position)
	{
		RMuint32 p = tell ();
		if (p >= stop_position)
			return 1;
	}

	m_error = MP4_ERROR_NO_ERROR;

	m_boxlength = get32 (&error);
	if (error)
		return 1;
	ASSERT (m_boxlength);
	m_boxtype = get32 (&error);
	if (m_boxlength == 1)
		m_boxlength = get64 (&error);		
	if (error)
		return 1;	
	switch (m_boxtype)
	{
	case MP4_FTYP:
		DEBUGMSG (1, ("MP4_FTYP\n"));
		return ftyp_box ();
		break;
	case MP4_MOOV:
		DEBUGMSG (1, ("MP4_MOOV\n"));
		return moov_box ();
		break;
	case MP4_MOOF:
		DEBUGMSG (1, ("MP4_MOOF\n"));
		return moof_box ();
		break;
	case MP4_MDAT:
		DEBUGMSG (1, ("MP4_MDAT\n"));
		return mdat_box ();
		break;
	case MP4_FREE:
		DEBUGMSG (1, ("MP4_FREE\n"));
		return free_box ();
		break;
	case MP4_SKIP:
		DEBUGMSG (1, ("MP4_SKIP\n"));
		return skip_box ();

	case MP4_MVHD:
		DEBUGMSG (1, ("  MP4_MVHD\n"));
		return mvhd_box ();
		break;
	case MP4_TRAK:
		DEBUGMSG (1, ("  MP4_TRAK\n"));
		return trak_box ();
		break;
	case MP4_TKHD:
		DEBUGMSG (1, ("    MP4_TKHD\n"));
		return tkhd_box ();
		break;
	case MP4_TREF:
		DEBUGMSG (1, ("    MP4_TREF\n"));
		return tref_box ();
		break;
	case MP4_EDTS:
		DEBUGMSG (1, ("    MP4_EDTS\n"));
		return edts_box ();
		break;
	case MP4_ELST:
		DEBUGMSG (1, ("      MP4_ELST\n"));
		return elst_box ();
		break;
	case MP4_MDIA:
		DEBUGMSG (1, ("    MP4_MDIA\n"));
		return mdia_box ();
		break;
	case MP4_MDHD:
		DEBUGMSG (1, ("      MP4_MDHD\n"));
		return mdhd_box ();
		break;
	case MP4_HDLR:
		DEBUGMSG (1, ("      MP4_HDLR\n"));
		return hdlr_box ();
		break;
	case MP4_MINF:
		DEBUGMSG (1, ("      MP4_MINF\n"));
		return minf_box ();
		break;
	case MP4_NMHD:
		DEBUGMSG (1, ("        MP4_NMHD\n"));
		return nmhd_box ();
		break;
	case MP4_VMHD:
		DEBUGMSG (1, ("        MP4_VMHD\n"));
		return vmhd_box ();
		break;
	case MP4_SMHD:
		DEBUGMSG (1, ("        MP4_SMHD\n"));
		return smhd_box ();
		break;
	case MP4_HMHD:
		DEBUGMSG (1, ("        MP4_HMHD\n"));
		return hmhd_box ();
		break;
	case MP4_DINF:
		DEBUGMSG (1, ("        MP4_DINF\n"));
		return dinf_box ();
		break;
	case MP4_DREF:
		DEBUGMSG (1, ("          MP4_DREF\n"));
		return dref_box ();
		break;
	case MP4_STBL:
		DEBUGMSG (1, ("        MP4_STBL\n"));
		return stbl_box ();
		break;
	case MP4_STSD:
		DEBUGMSG (1, ("          MP4_STSD\n"));
		return stsd_box ();
		break;
	case MP4_STTS:
		DEBUGMSG (1, ("          MP4_STTS\n"));
		return stts_box ();
		break;
	case MP4_CTTS:
		DEBUGMSG (1, ("          MP4_CTTS\n"));
		return ctts_box ();
		break;
	case MP4_STSC:
		DEBUGMSG (1, ("          MP4_STSC\n"));
		return stsc_box ();
		break;
	case MP4_STSZ:
		DEBUGMSG (1, ("          MP4_STSZ\n"));
		return stsz_box ();
		break;
	case MP4_STZ2:
		DEBUGMSG (1, ("          MP4_STZ2\n"));
		return stz2_box ();
		break;
	case MP4_STC0:
		DEBUGMSG (1, ("          MP4_STCO\n"));
		return stco_box ();
		break;
	case MP4_STSS:
		DEBUGMSG (1, ("          MP4_STSS\n"));
		return stss_box ();
		break;
	case MP4_PADB:
		DEBUGMSG (1, ("          MP4_PADB\n"));
		return padb_box ();
		break;
	case MP4_STDP:
		DEBUGMSG (1, ("          MP4_STDP\n"));
		return stdp_box ();
		break;
	case MP4_MVEX:
		DEBUGMSG (1, ("    MP4_MVEX\n"));
		return mvex_box ();
		break;
	case MP4_TREX:	
		DEBUGMSG (1, ("      MP4_TREX\n"));
		return trex_box ();
		break;
	case MP4_IODS:
		DEBUGMSG (1, ("  MP4_IODS\n"));
		return iods_box ();
		break;

	case MP4_UDTA:
		DEBUGMSG (1, ("  MP4_UDTA\n"));
		return udta_box ();
		break;
	case MP4_CHPL:
		DEBUGMSG (1, ("    MP4_CHPL\n"));
		return chpl_box ();
		break;

	default:
		DEBUGMSG (1, ("MP4_UNKNOWN: %c%c%c%c\n", 
			verify_character ((RMint8)(m_boxtype >> 24)),
			verify_character ((RMint8)(m_boxtype >> 16)),
			verify_character ((RMint8)(m_boxtype >>  8)),
			verify_character ((RMint8)(m_boxtype >>  0))));
		return skip_box ();
		break;
	}
	return 1;
}

RMuint32 MP4Demux::ftyp_box ()
{	
	RMuint32 boxpos, filetype, error;
	RMint32 boxlength, fileok;
	ASSERT (m_boxlength >= 8);
	boxpos = tell ();

	fileok = 0;
	boxlength = (RMint32)m_boxlength;
	while (boxlength > 0)
	{
		filetype = get32 (&error);
		if (error)
		{
			m_error = MP4_ERROR_FILE_TOO_SHORT;
			return 1;
		}
		if (filetype == MP4_ISOM)
		{
			DEBUGMSG (1, ("  MP4_ISOM\n"));
			fileok = 1;
			break;
		}
		boxlength -= 4;
	}
	if (!fileok)
	{
		DEBUGMSG (1, ("  MP4_ISOM not found\n"));
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	seek (boxpos + m_boxlength - 8);
	return 0;
}

RMuint32 MP4Demux::moov_box ()
{
	RMuint64 moov_boxpos, moov_boxlength;

	ASSERT (m_boxlength >= 8);
	moov_boxlength = m_boxlength;
	moov_boxpos = tell ();

	// process the 'moov' box
	while (NextBox (moov_boxpos + moov_boxlength - 8) == 0);

	seek (moov_boxpos + moov_boxlength - 8);
	return 0;
}

RMuint32 MP4Demux::moof_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::mdat_box ()
{
	ASSERT (m_boxlength >= 8);
	m_mdatpos = tell ();
	m_mdatlength = (RMuint32)(m_boxlength-8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::free_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::skip_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::udta_box ()
{
	RMuint32 boxpos, boxlength;

	ASSERT (m_boxlength >= 8);
	boxpos = tell ();
	boxlength = (RMuint32)m_boxlength;

	// process the udta box
	while (NextBox (boxpos + boxlength - 8) == 0);

	seek (boxpos + boxlength - 8);
	return 0;
}

RMuint32 MP4Demux::chpl_box ()
{
	RMuint32 boxpos, boxlength, error;
	RMuint8 version;

	ASSERT (m_boxlength >= 8);
	boxpos = tell ();
	boxlength = (RMuint32)m_boxlength;

	RMint32 entry_count, i, j;

	version = get8 (&error);
	if (error)
		goto chpl_box_exit;
	get32 (&error);
	entry_count = get32 (&error);
	if (error)
		goto chpl_box_exit;

	RMint64 media_time;
	RMuint8 namelen, name[64];
/*
	for (i=0; i<entry_count; i++)
	{
		if (version)
			media_time = get64 (&error);
		else
			media_time = get32 (&error);
		ASSERT (error == 0);
		namelen = get8 (&error);
		ASSERT (error == 0);
		memset (name, 0, sizeof(name));
		for (j=0; j<namelen; j++)
		{
			if (j < (sizeof(name)-1))
				name[j] = get8 (&error);
			else
				get8 (&error);
			ASSERT (error == 0);
		}		
		DEBUGMSG (1, ("chapter name = %s, time = %d seconds\n", name, (RMint32)(media_time / 10000000)));
	}
*/
	m_chpl_present = boxpos;
	ASSERT (m_chpl_box.IsInitialized () == 0);
	if (m_chpl_box.IsInitialized () == 0)
	{
		m_chpl_box.Init (m_handle, boxpos, boxlength, this, entry_count);
		m_chpl_box.InitCallbackTable (&m_CallbackTable);
		version = m_chpl_box.get8 (&error);
		m_chpl_box.advance8 (&error);
		m_chpl_box.advance32 (&error);
		m_nchapters = m_chpl_box.get32 (&error);
		m_chpl_box.advance32 (&error);
		RMint64 time0;
		for (i=0; i<entry_count; i++)
		{
			if (version)
			{
				media_time = m_chpl_box.get64 (&error);
				m_chpl_box.advance64 (&error);
			}
			else
			{
				media_time = m_chpl_box.get32 (&error);
				m_chpl_box.advance32 (&error);
			}
			ASSERT (error == 0);
			namelen = m_chpl_box.get8 (&error);
			m_chpl_box.advance8 (&error);
			ASSERT (error == 0);
			memset (name, 0, sizeof(name));
			for (j=0; j<namelen; j++)
			{
				if (j < (sizeof(name)-2))
					name[j] = m_chpl_box.get8 (&error);
				else
					m_chpl_box.get8 (&error);
				m_chpl_box.advance8 (&error);
				ASSERT (error == 0);
			}		
			DEBUGMSG (1, ("       chapter name = %s, time = %d seconds\n", name, (RMint32)(media_time / 10000000)));
			if (i == 0)
				time0 = media_time;
			media_time -= time0;
			if (i < MP4_MAX_CHAPTERS)
			{
				m_mp4Chapters[i].time = media_time / 10000;
				strcpy (m_mp4Chapters[i].name, (RMint8 *)name); 
			}
		}
	}
chpl_box_exit:
	seek (boxpos + boxlength - 8);
	return 0;
}

RMuint32 MP4Demux::mvhd_box ()
{
	RMuint32 version_flags, error;

	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	RMuint32 creation_time, modification_time;

	creation_time = get32 (&error);
	modification_time = get32 (&error);
	m_mvhdTimeScale = get32 (&error);
	DEBUGMSG (1, ("    timescale: %d\n", (RMint32)m_mvhdTimeScale));
	m_mvhdDuration = get32 (&error);
	DEBUGMSG (1, ("    duration: %d\n", (RMint32)m_mvhdDuration));

	RMuint32 rate, i;
	RMuint16 volume;

	rate = get32 (&error);
	volume = get16 (&error);
	get16 (&error);
	get32 (&error);
	get32 (&error);
	for (i=0; i<9; i++)
		get32 (&error);
	for (i=0; i<6; i++)
		get32 (&error);
	get32 (&error);
	return error;
}

RMuint32 MP4Demux::trak_box ()
{
	m_trackindex++;
	m_currentHandlerType = 0;
	return 0;
}

RMuint32 MP4Demux::tkhd_box ()
{
	RMuint32 version_flags, error, i;
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}

	RMuint32 creation_time = get32 (&error);
	RMuint32 modication_time = get32 (&error);
	RMuint32 track_id = get32 (&error);
	m_trackids[m_trackindex] = track_id;
	DEBUGMSG (1, ("      track_id: %d\n", (RMint32)track_id));
	ASSERT (track_id);
	get32 (&error);
	RMuint32 duration = get32 (&error);
	get32 (&error);
	get32 (&error);
	RMuint16 layer = get16 (&error);
	RMuint16 predefined = get16 (&error);
	RMuint16 volume = get16 (&error);
	get16 (&error);
	for (i=0; i<9; i++)
		get32 (&error);
	RMuint32 width = get32 (&error);
	DEBUGMSG (1, ("      width: %d\n", (RMint32)width >> 16));
	RMuint32 height = get32 (&error);
	DEBUGMSG (1, ("      height: %d\n", (RMint32)height >> 16));
	if (m_currentHandlerType == MP4_FOURCC('s','u','b','p'))
	{
		m_spWidth[m_trackindex] = (RMint32)width >> 16;
		m_spHeight[m_trackindex] = (RMint32)height >> 16;
	}
	return error;
}

RMuint32 MP4Demux::tref_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::edts_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::elst_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::mdia_box ()
{
	return 0;
}

RMuint32 MP4Demux::mdhd_box ()
{
	RMuint32 version_flags, error;
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	RMuint32 creation_time = get32 (&error);
	RMuint32 modification_time = get32 (&error);
	RMuint32 timescale = get32 (&error);
	RMuint32 duration = get32 (&error);
	RMuint16 language = get16 (&error);
	RMuint16 pre_defined = get16 (&error);
	m_tracktimescales[m_trackindex] = timescale;
	m_trackdurations[m_trackindex] = duration;
	DEBUGMSG (1, ("        track timescale: %d\n", timescale));
	DEBUGMSG (1, ("        track duration: %d\n", duration));
	return error;
}

RMuint32 MP4Demux::hdlr_box ()
{
	ASSERT (m_boxlength >= 8);

	RMuint32 version_flags, error;
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	RMuint32 pre_defined = get32 (&error);
	RMuint32 handler_type = get32 (&error);
	DEBUGMSG (1, ("        handler_type: %c%c%c%c\n", 
		(RMint8)(handler_type >> 24),
		(RMint8)(handler_type >> 16),
		(RMint8)(handler_type >>  8),
		(RMint8)(handler_type >>  0)));
	if (handler_type == MP4_FOURCC ('s','u','b','p'))
		m_nspStreams++;
	if (handler_type == MP4_FOURCC ('s','o','u','n'))
		m_naudioStreams++;
	m_currentHandlerType = handler_type;
	get32 (&error);
	get32 (&error);
	get32 (&error);
	seekcur (m_boxlength-8-24);
	return 0;
}

RMuint32 MP4Demux::minf_box ()
{
	return 0;
}

RMuint32 MP4Demux::vmhd_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::smhd_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::hmhd_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::dinf_box ()
{
	return 0;
}

RMuint32 MP4Demux::dref_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::stbl_box ()
{
	return 0;
}

RMuint32 MP4Demux::stsd_box ()
{
	ASSERT (m_boxlength >= 8);

	RMuint64 stsd_boxlength = m_boxlength;
	RMuint32 stsd_boxpos = tell ();

	RMuint32 version_flags, error, i, j;
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	RMuint32 entry_count = get32 (&error);
	RMuint32 boxlength, boxtype;
	RMuint8 ES_DescrTag;
	for (i=0; i<entry_count; i++)
	{
		boxlength = get32 (&error);
		boxtype = get32 (&error);
		get8 (&error);
		get8 (&error);
		get8 (&error);
		get8 (&error);
		get8 (&error);
		get8 (&error);
		RMuint16 data_refernce_index = get16 (&error);
		RMuint16 width, height, depth, channelcount, samplesize, sampleratehi, sampleratelo;
		RMuint32 hres, vres;
		RMuint16 ES_ID;
		RMuint8 ES_DescriptorFlags, DecoderConfigDescrTag, DecoderSpecificInfoTag, tmpByte;
		RMuint32 ES_DescrLength, DecoderConfigDescrLength, DecoderSpecificInfoLength;
		RMuint8 objectTypeIndication, streamType;
		switch (boxtype)
		{
		case MP4_FOURCC('m','p','4','v'):
			get16 (&error);
			get16 (&error);
			get32 (&error);
			get32 (&error);
			get32 (&error);
			width = get16 (&error);
			height = get16 (&error);
			if (width)
				m_visualWidth = width;
			if (height)
				m_visualHeight = height;

//			gVideofilePlayinfo.OriginalWidth	= m_visualWidth;
//			gVideofilePlayinfo.OriginalHeight	= m_visualHeight;

			hres = get32 (&error);
			vres = get32 (&error);
			get32 (&error);
			get16 (&error);
			for (j=0; j<32; j++)
				get8 (&error);
			depth = get16 (&error);
			get16 (&error);
			boxlength = get32 (&error);
			boxtype = get32 (&error);
			ASSERT (boxtype == MP4_FOURCC('e','s','d','s'));
			version_flags = get32 (&error);
			// ES_Descriptor
			ES_DescrTag = get8 (&error);
			ASSERT (ES_DescrTag == 0x03);
			ES_DescrLength = 0;
			do
			{
				tmpByte = get8 (&error);
				ES_DescrLength = (ES_DescrLength << 7) | (tmpByte & 0x7f);
			} while (tmpByte & 0x80);			
			ES_ID = get16 (&error);
			ES_DescriptorFlags = get8 (&error);
			if (ES_DescriptorFlags & 0x80)
				get16 (&error);
			if (ES_DescriptorFlags & 0x40)
			{
				RMuint8 url_length;
				url_length = get8 (&error);
				for (j=0; j<url_length; j++)
					get8 (&error);
			}
			if (ES_DescriptorFlags & 0x20)
				get16 (&error);
			// DecoderConfigDescriptor
			DecoderConfigDescrTag = get8 (&error);
			ASSERT (DecoderConfigDescrTag == 0x04);			
			DecoderConfigDescrLength = 0;
			do
			{
				tmpByte = get8 (&error);
				DecoderConfigDescrLength = (DecoderConfigDescrLength << 7) | (tmpByte & 0x7f);
			} while (tmpByte & 0x80);
			objectTypeIndication = get8 (&error);
			streamType = get8 (&error);
			get8 (&error);
			get16 (&error);
			get32 (&error);
			get32 (&error);
			// DecoderSpecificInfo
			DecoderSpecificInfoTag = get8 (&error);
			ASSERT (DecoderSpecificInfoTag == 0x5);
			DecoderSpecificInfoLength = 0;
			do
			{
				tmpByte = get8 (&error);
				DecoderSpecificInfoLength = (DecoderSpecificInfoLength << 7) | (tmpByte & 0x7f);
			} while (tmpByte & 0x80);
			ASSERT (DecoderSpecificInfoLength <= MP4_MAX_VIDEO_DSI);
			for (j=0; j<DecoderSpecificInfoLength; j++)
			{
				if (j >= MP4_MAX_VIDEO_DSI)
					break;
				m_videoDSI[j] = get8 (&error);
			}
			m_videoDSILength = j;
			break;
		case MP4_FOURCC('m','p','4','a'):
			get32 (&error);
			get32 (&error);
			channelcount = get16 (&error);
			samplesize = get16 (&error);
			get16 (&error);
			get16 (&error);
			sampleratehi = get16 (&error);
			sampleratelo = get16 (&error);			
			boxlength = get32 (&error);
			boxtype = get32 (&error);
			ASSERT (boxtype == MP4_FOURCC('e','s','d','s'));
			version_flags = get32 (&error);
			ASSERT (version_flags == 0);
			// ES_Descriptor
			ES_DescrTag = get8 (&error);
			ASSERT (ES_DescrTag == 0x03);
			ES_DescrLength = 0;
			do
			{
				tmpByte = get8 (&error);
				ES_DescrLength = (ES_DescrLength << 7) | (tmpByte & 0x7f);
			} while (tmpByte & 0x80);			
			ES_ID = get16 (&error);
			ES_DescriptorFlags = get8 (&error);
			if (ES_DescriptorFlags & 0x80)
				get16 (&error);
			if (ES_DescriptorFlags & 0x40)
			{
				RMuint8 url_length;
				url_length = get8 (&error);
				for (j=0; j<url_length; j++)
					get8 (&error);
			}
			if (ES_DescriptorFlags & 0x20)
				get16 (&error);
			// DecoderConfigDescriptor
			DecoderConfigDescrTag = get8 (&error);
			ASSERT (DecoderConfigDescrTag == 0x04);			
			DecoderConfigDescrLength = 0;
			do
			{
				tmpByte = get8 (&error);
				DecoderConfigDescrLength = (DecoderConfigDescrLength << 7) | (tmpByte & 0x7f);
			} while (tmpByte & 0x80);
			objectTypeIndication = get8 (&error);
			streamType = get8 (&error);
			get8 (&error);
			get16 (&error);
			get32 (&error);
			get32 (&error);
			// DecoderSpecificInfo
			DecoderSpecificInfoTag = get8 (&error);
			ASSERT (DecoderSpecificInfoTag == 0x5);
			DecoderSpecificInfoLength = 0;
			do
			{
				tmpByte = get8 (&error);
				DecoderSpecificInfoLength = (DecoderSpecificInfoLength << 7) | (tmpByte & 0x7f);
			} while (tmpByte & 0x80);
			ASSERT (DecoderSpecificInfoLength <= MP4_MAX_AUDIO_DSI);
			ASSERT (m_naudioStreams > 0);
			for (j=0; j<DecoderSpecificInfoLength; j++)
			{
				if (j >= MP4_MAX_AUDIO_DSI)
					break;
				m_audioDSI[m_naudioStreams-1][j] = get8 (&error);
			}
			m_audioDSILength[m_naudioStreams-1] = j;
			break;
		case MP4_FOURCC('h','i','n','t'):
			ASSERT (0);
		case MP4_FOURCC('m','p','4','s'):
		default:
			boxlength = get32 (&error);
			boxtype = get32 (&error);
			ASSERT (boxtype == MP4_FOURCC('e','s','d','s'));

			version_flags = get32 (&error);
			ASSERT (version_flags == 0);
			// ES_Descriptor
			ES_DescrTag = get8 (&error);
			ASSERT (ES_DescrTag == 0x03);
			ES_DescrLength = 0;
			do
			{
				tmpByte = get8 (&error);
				ES_DescrLength = (ES_DescrLength << 7) | (tmpByte & 0x7f);
			} while (tmpByte & 0x80);			
			ES_ID = get16 (&error);
			ES_DescriptorFlags = get8 (&error);
			if (ES_DescriptorFlags & 0x80)
				get16 (&error);
			if (ES_DescriptorFlags & 0x40)
			{
				RMuint8 url_length;
				url_length = get8 (&error);
				for (j=0; j<url_length; j++)
					get8 (&error);
			}
			if (ES_DescriptorFlags & 0x20)
				get16 (&error);
			// DecoderConfigDescriptor
			DecoderConfigDescrTag = get8 (&error);
			ASSERT (DecoderConfigDescrTag == 0x04);			
			DecoderConfigDescrLength = 0;
			do
			{
				tmpByte = get8 (&error);
				DecoderConfigDescrLength = (DecoderConfigDescrLength << 7) | (tmpByte & 0x7f);
			} while (tmpByte & 0x80);
			objectTypeIndication = get8 (&error);
			streamType = get8 (&error);
			if ((objectTypeIndication == 0xe0) &&
				(streamType == 0xe1))
			{
				// subpicture palette
				get8 (&error);
				get16 (&error);
				get32 (&error);
				get32 (&error);
				// DecoderSpecificInfo
				DecoderSpecificInfoTag = get8 (&error);
				ASSERT (DecoderSpecificInfoTag == 0x5);
				DecoderSpecificInfoLength = 0;
				do
				{
					tmpByte = get8 (&error);
					DecoderSpecificInfoLength = (DecoderSpecificInfoLength << 7) | (tmpByte & 0x7f);
				} while (tmpByte & 0x80);
				ASSERT (DecoderSpecificInfoLength <= MP4_MAX_SP_DSI);
				ASSERT (m_nspStreams > 0);
				for (j=0; j<DecoderSpecificInfoLength; j++)
				{
					if (j >= MP4_MAX_SP_DSI)
						break;
					m_spDSI[m_nspStreams-1][j] = get8 (&error);
				}
				m_spDSILength[m_nspStreams-1] = j;
			}
			break;
		}
	}

	seek (stsd_boxpos + stsd_boxlength - 8);

	return 0;
}

RMuint32 MP4Demux::stts_box ()
{
	ASSERT (m_boxlength >= 8);

	RMuint64 stts_boxlength = m_boxlength;
	RMuint32 stts_boxpos = tell ();

	RMuint32 version_flags, error;
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	RMuint32 entry_count = get32 (&error);
	if (error)
		return error;
	if (m_currentHandlerType == MP4_FOURCC ('v','i','d','e'))
	{
		if (m_video_stts.IsInitialized () == 0)
		{
			m_video_stts.Init (m_handle, tell (), entry_count*8, this, entry_count);
			m_video_stts.InitCallbackTable (&m_CallbackTable);
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','o','u','n'))
	{
		RMint32 i;
		for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
		{
			if (m_audio_stts[i].IsInitialized () == 0)
			{
				m_audio_stts[i].Init (m_handle, tell (), entry_count*8, this, entry_count);
				m_audio_stts[i].InitCallbackTable (&m_CallbackTable);
				break;
			}
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','u','b','p'))
	{
		RMint32 i;
		for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
		{
			if (m_subpi_stts[i].IsInitialized () == 0)
			{
				m_subpi_stts[i].Init (m_handle, tell (), entry_count*8, this, entry_count);
				m_subpi_stts[i].InitCallbackTable (&m_CallbackTable);
				break;
			}
		}
	}

	seek (stts_boxpos + stts_boxlength - 8);
	return 0;
}

RMuint32 MP4Demux::ctts_box ()
{
	ASSERT (m_boxlength >= 8);

	RMuint64 ctts_boxlength = m_boxlength;
	RMuint32 ctts_boxpos = tell ();

	RMuint32 version_flags, error;
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	RMuint32 entry_count = get32 (&error);
	if (error)
		return error;

	if (m_currentHandlerType == MP4_FOURCC ('v','i','d','e'))
	{
		if (m_video_ctts.IsInitialized () == 0)
		{
			m_video_ctts.Init (m_handle, tell (), entry_count*8, this, entry_count);
			m_video_ctts.InitCallbackTable (&m_CallbackTable);
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','o','u','n'))
	{
		RMint32 i;
		for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
		{
			if (m_audio_ctts[i].IsInitialized () == 0)
			{
				m_audio_ctts[i].Init (m_handle, tell (), entry_count*8, this, entry_count);
				m_audio_ctts[i].InitCallbackTable (&m_CallbackTable);
				break;
			}
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','u','b','p'))
	{
		RMint32 i;
		for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
		{
			if (m_subpi_ctts[i].IsInitialized () == 0)
			{
				m_subpi_ctts[i].Init (m_handle, tell (), entry_count*8, this, entry_count);
				m_subpi_ctts[i].InitCallbackTable (&m_CallbackTable);
				break;
			}
		}
	}
/*
	RMuint32 sample_count, i;
	RMint32 sample_offset;
	for (i=0; i<entry_count; i++)
	{
		sample_count = get32 (&error);		
		sample_offset = get32 (&error);		
	}
*/
	seek (ctts_boxpos + ctts_boxlength - 8);
	return 0;
}

// samples per chunk
RMuint32 MP4Demux::stsc_box ()
{
	ASSERT (m_boxlength >= 8);

	RMuint64 stsc_boxlength = m_boxlength;
	RMuint32 stsc_boxpos = tell ();

	RMuint32 version_flags, error;	
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	RMuint32 entry_count = get32 (&error);
	ASSERT (entry_count);	
	if (m_currentHandlerType == MP4_FOURCC ('v','i','d','e'))
	{
		if (m_video_stsc.IsInitialized () == 0)
		{
			m_video_stsc.Init (m_handle, tell (), entry_count*12, this, entry_count);
			m_video_stsc.InitCallbackTable (&m_CallbackTable);
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','o','u','n'))
	{
		RMint32 i;
		for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
		{
			if (m_audio_stsc[i].IsInitialized () == 0)
			{
				m_audio_stsc[i].Init (m_handle, tell (), entry_count*12, this, entry_count);
				m_audio_stsc[i].InitCallbackTable (&m_CallbackTable);
				break;
			}
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','u','b','p'))
	{
		RMint32 i;
		for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
		{
			if (m_subpi_stsc[i].IsInitialized () == 0)
			{
				m_subpi_stsc[i].Init (m_handle, tell (), entry_count*12, this, entry_count);
				m_subpi_stsc[i].InitCallbackTable (&m_CallbackTable);
				break;
			}
		}
	}
	/*
	RMuint32 first_chunk, i;
	RMuint32 samples_per_chunk, sample_description_index;
	for (i=0; i<entry_count; i++)
	{
		first_chunk = get32 (&error);
		samples_per_chunk = get32 (&error);
		sample_description_index = get32 (&error);
	}
	*/
	seek (stsc_boxpos + stsc_boxlength - 8);

	return 0;
}

// sample size box
RMuint32 MP4Demux::stsz_box ()
{
	ASSERT (m_boxlength >= 8);
	RMuint64 stsz_boxlength = m_boxlength;
	RMuint32 stsz_boxpos = tell ();	

	RMuint32 version_flags, error;
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	RMuint32 sample_size = get32 (&error);
	if (error)
		return error;
	RMuint32 sample_count = get32 (&error);
	if (m_currentHandlerType == MP4_FOURCC ('v','i','d','e'))
	{
		m_videoTrackIndex = m_trackindex;
		if (m_video_stsz.IsInitialized () == 0)
		{
			m_video_stsz.Init (m_handle, tell (), sample_count*4, this, sample_count);
			m_video_stsz.InitCallbackTable (&m_CallbackTable);
			m_video_sample_size = sample_size;
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','o','u','n'))
	{		
		RMint32 i;
		for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
		{
			if (m_audio_stsz[i].IsInitialized () == 0)
			{
				m_audio_stsz[i].Init (m_handle, tell (), sample_count*4, this, sample_count);
				m_audio_stsz[i].InitCallbackTable (&m_CallbackTable);
				m_audio_sample_size[i] = sample_size;
				m_audioTrackIndex[i] = m_trackindex;
				break;
			}
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','u','b','p'))
	{		
		RMint32 i;
		for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
		{
			if (m_subpi_stsz[i].IsInitialized () == 0)
			{
				m_subpi_stsz[i].Init (m_handle, tell (), sample_count*4, this, sample_count);
				m_subpi_stsz[i].InitCallbackTable (&m_CallbackTable);
				m_subpi_sample_size[i] = sample_size;
				m_subpiTrackIndex[i] = m_trackindex;
				break;
			}
		}
	}
	/*
	if (sample_size == 0)
	{
		RMuint32 entry_size, t = 0, j = 0, x, i;
		x = sample_count + 1;
		for (i=0; i<sample_count; i++)
		{
			entry_size = get32 (&error);						
			if (j++ == x)
			{
				t = 0;
				j = 1;
			}	
			t += entry_size;
		}
	}
	*/
	seek (stsz_boxpos + stsz_boxlength - 8);
	return error;
}

RMuint32 MP4Demux::stz2_box ()
{
	ASSERT (0);
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

// chunk offset box 
RMuint32 MP4Demux::stco_box ()
{
	ASSERT (m_boxlength >= 8);
	RMuint64 stco_boxlength = m_boxlength;
	RMuint32 stco_boxpos = tell ();	
	DEBUGMSG (0, ("stco_boxpos = %d\n", (RMint32)stco_boxpos));
	DEBUGMSG (0, ("stco_boxlength = %d\n", (RMint32)stco_boxlength));

	RMuint32 version_flags, error;
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}
	RMuint32 entry_count = get32 (&error);
	if (error)
		return error;
	if (m_currentHandlerType == MP4_FOURCC ('v','i','d','e'))
	{
		if (m_video_stco.IsInitialized () == 0)
		{
			m_video_stco.Init (m_handle, tell (), entry_count*4, this, entry_count);
			m_video_stco.InitCallbackTable (&m_CallbackTable);
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','o','u','n'))
	{
		RMint32 i;
		for (i=0; i<MP4_MAX_AUDIOTRACKS; i++)
		{
			if (m_audio_stco[i].IsInitialized () == 0)
			{
				m_audio_stco[i].Init (m_handle, tell (), entry_count*4, this, entry_count);
				m_audio_stco[i].InitCallbackTable (&m_CallbackTable);
				break;
			}
		}
	}
	else if (m_currentHandlerType == MP4_FOURCC ('s','u','b','p'))
	{
		RMint32 i;
		for (i=0; i<MP4_MAX_SUBPICTRACKS; i++)
		{
			if (m_subpi_stco[i].IsInitialized () == 0)
			{
				m_subpi_stco[i].Init (m_handle, tell (), entry_count*4, this, entry_count);
				m_subpi_stco[i].InitCallbackTable (&m_CallbackTable);
				break;
			}
		}
	}
	/*	
	RMuint32 chunk_offset, i;
	for (i=0; i<entry_count; i++)
	{
		chunk_offset = get32 (&error);	
	}
	*/
	DEBUGMSG (0, ("seek (%d + %d - 8)\n", (RMint32)stco_boxpos, (RMint32)stco_boxlength));
	seek (stco_boxpos + stco_boxlength - 8);

	return error;
}

RMuint32 MP4Demux::stss_box ()
{
	ASSERT (m_boxlength >= 8);
	RMuint64 stss_boxlength = m_boxlength;
	RMuint32 stss_boxpos = tell ();	

	RMuint32 version_flags, error;
	version_flags = get32 (&error);
	if (error)
		return error;
	ASSERT ((version_flags & 0xff000000) == 0);
	if (version_flags & 0xff000000)
	{
		m_error = MP4_ERROR_FILE_NOT_SUPPORTED;
		return 1;
	}

	RMuint32 entry_count = get32 (&error);
	if (error)
		return error;
	if (m_currentHandlerType == MP4_FOURCC ('v','i','d','e'))
	{
		if (m_video_stss.IsInitialized () == 0)
		{
			m_video_stss.Init (m_handle, tell (), entry_count*4, this, entry_count);
			m_video_stss.InitCallbackTable (&m_CallbackTable);
		}
	}

	seek (stss_boxpos + stss_boxlength - 8);
	return 0;
}

RMuint32 MP4Demux::stsh_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::padb_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::stdp_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::mvex_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::trex_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::iods_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}

RMuint32 MP4Demux::nmhd_box ()
{
	ASSERT (m_boxlength >= 8);
	seekcur (m_boxlength-8);
	return 0;
}
////////////////////////////////////////////////////////////////////////

RMuint32 MP4Demux::tell ()
{
	RMuint32 p;
	p = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
	if (m_valid)
		p -= (m_ReadLength - m_tmpidx);
	return p;
}

RMuint32 MP4Demux::seek (RMuint64 offset)
{			
	RMuint32 current_position;
	if (m_valid)
	{
		current_position = tell ();
		if ((current_position + m_valid) > offset)
		{
			ASSERT (offset >= current_position);
			m_valid -= (RMint32)(offset - current_position);
			m_tmpidx += (RMint32)(offset - current_position);
			return (RMuint32)offset;
		}
	}	
	m_valid = 0;
	m_tmpidx = MP4_MIN_READ_LENGTH;
	return m_CallbackTable.fseek (m_handle, (RMuint32)offset, SEEK_SET, m_CallbackTable.context);
}

RMuint32 MP4Demux::seekcur (RMuint64 offset)
{
	RMuint32 tmp;
	ASSERT ((RMint32)m_ReadLength >= (RMint32)m_tmpidx);
	tmp = tell ();
	tmp += (RMint32)offset;
	tmp = m_CallbackTable.fseek (m_handle, tmp, SEEK_SET, m_CallbackTable.context);
	m_valid = 0;
	m_tmpidx = MP4_MIN_READ_LENGTH;
	return tmp; 
}

RMuint32 MP4Demux::reload ()
{
	RMuint32 n;

	n = MP4_MIN_READ_LENGTH - m_tmpidx;
	memcpy (m_tmp, m_tmp+m_tmpidx, n);
	m_tmpidx = n;
	n = MP4_MIN_READ_LENGTH - m_tmpidx;
	ASSERT ((RMint32)n > 0);
	n = m_CallbackTable.fread (m_handle, m_tmp+m_tmpidx, n, m_CallbackTable.context);
	ASSERT (n);
	DEBUGMSG ((n == 0), ("fread fails @ %d\n", (int)m_CallbackTable.ftell (m_handle, m_CallbackTable.context)));
	m_ReadLength = n;
	n += m_tmpidx;
	m_tmpidx = 0;
	return n;
}

RMuint8 MP4Demux::get8 (RMuint32 *error)
{
	*error = 0;
	if (m_valid < 1)
	{
		m_valid = reload ();
	}
	ASSERT (m_valid);
	if (m_valid == 0)
	{
		*error = 1;
		return 0;
	}
	m_valid--;
	return m_tmp[m_tmpidx++];
}

RMuint16 MP4Demux::get16 (RMuint32 *error)
{
	RMuint16 tmp;
	*error = 0;
	if (m_valid < 2)
	{
		m_valid = reload ();
	}
	ASSERT (m_valid);
	if (m_valid == 0)
	{
		*error = 1;
		return 0;
	}
	tmp = (
		((RMuint16)(m_tmp[m_tmpidx+0]) << 8) | 
		((RMuint16)(m_tmp[m_tmpidx+1]) << 0));
	m_tmpidx += 2;
	m_valid -= 2;
	return tmp;
}

RMuint32 MP4Demux::get32 (RMuint32 *error)
{
	RMuint32 tmp;
	*error = 0;
	if (m_valid < 4)
	{
		m_valid = reload ();
	}
	ASSERT (m_valid);
	if (m_valid == 0)
	{
		*error = 1;
		return 0;
	}
	tmp = (
		((RMuint32)(m_tmp[m_tmpidx+0]) << 24) | 
		((RMuint32)(m_tmp[m_tmpidx+1]) << 16) |
		((RMuint32)(m_tmp[m_tmpidx+2]) <<  8) | 
		((RMuint32)(m_tmp[m_tmpidx+3]) <<  0));
	m_valid -= 4;
	m_tmpidx += 4;
	return tmp;
}

RMuint64 MP4Demux::get64 (RMuint32 *error)
{
	RMuint64 tmp;
	*error = 0;
	if (m_valid < 8)
	{
		m_valid = reload ();
	}
	ASSERT (m_valid);
	if (m_valid == 0)
	{
		*error = 1;
		return 0;
	}
	tmp = (
		((RMuint64)(m_tmp[m_tmpidx+0]) << 56) | 
		((RMuint64)(m_tmp[m_tmpidx+1]) << 48) |
		((RMuint64)(m_tmp[m_tmpidx+2]) << 40) | 
		((RMuint64)(m_tmp[m_tmpidx+3]) << 32) |
		((RMuint64)(m_tmp[m_tmpidx+4]) << 24) | 
		((RMuint64)(m_tmp[m_tmpidx+5]) << 16) |
		((RMuint64)(m_tmp[m_tmpidx+6]) <<  8) | 
		((RMuint64)(m_tmp[m_tmpidx+7]) <<  0));
	m_valid -= 8;
	m_tmpidx += 8;
	return tmp;
}

RMint32	MP4Demux::GetAudioDSI (RMint32 streamno, RMuint8 **ppdsi)
{
	ASSERT (streamno < MP4_MAX_AUDIOTRACKS);
	*ppdsi = m_audioDSI[streamno];
	return m_audioDSILength[streamno];
}

RMint32	MP4Demux::GetSubpictureDSI (RMint32 streamno, RMuint8 **ppdsi)
{
	ASSERT (streamno < MP4_MAX_SUBPICTRACKS);
	*ppdsi = m_spDSI[streamno];
	return m_spDSILength[streamno];
}

MP4_ERROR MP4Demux::SetCurrentAudioStream (RMint32 streamno)
{
	ASSERT (streamno < MP4_MAX_AUDIOTRACKS);
	m_currentAudioTrack = streamno;
	return MP4_ERROR_NO_ERROR;
}

MP4_ERROR MP4Demux::SetCurrentSubpictureStream (RMint32 streamno)
{
	ASSERT (streamno < MP4_MAX_SUBPICTRACKS);
	m_currentSubpiTrack = streamno;
	return MP4_ERROR_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////
MP4TableHelper::MP4TableHelper ()
{
	m_IsInitialized = 0;
}

MP4TableHelper::~MP4TableHelper ()
{
}

RMuint32 MP4TableHelper::IsInitialized ()
{
	return m_IsInitialized;
}

RMuint32 MP4TableHelper::Init (RMuint32 filehandle, RMuint64 offset, RMuint32 length, MP4Demux *pMP4Demux, RMuint32 entrycount)
{
	m_pMP4Demux = pMP4Demux;
	m_handle = filehandle;
	m_StartingOffset = (RMuint32)offset;
	m_CurrentOffset = m_StartingOffset;
	m_TableLeft = length;
	m_TableLength = length;
	m_valid = 0;
	m_tmpidx = MP4_MIN_READ_LENGTH;;
	ASSERT (m_IsInitialized == 0);
	m_IsInitialized = 1;
	m_entrycount = entrycount;
	return 0;
}

RMuint32 MP4TableHelper::reset ()
{
	m_CurrentOffset = m_StartingOffset;
	m_TableLeft = m_TableLength;
	m_valid = 0;
	m_tmpidx = MP4_MIN_READ_LENGTH;

	return 0;
}

RMuint32 MP4TableHelper::seek (RMuint32 pos)
{
	if (m_IsInitialized == 0)
		return (RMuint32)-1;
	ASSERT (m_StartingOffset);
	
	if (m_valid)
	{
		RMuint32 current_offset = m_CurrentOffset - m_StartingOffset;
		ASSERT ((RMint32)current_offset >= 0);
		RMuint32 starting_offset = current_offset - (m_tmpidx + m_valid);
		ASSERT (starting_offset <= current_offset);
		ASSERT ((RMint32)starting_offset >= 0);
		if ((pos >= starting_offset) && (pos < current_offset))
		{
			m_tmpidx = pos - starting_offset;
			m_valid = current_offset - pos;
			return pos;			
		}
	}
	
	m_CurrentOffset = m_StartingOffset + pos;
	m_TableLeft = m_TableLength;
	m_valid = 0;
	m_tmpidx = MP4_MIN_READ_LENGTH;
	return pos;
}

RMuint32 MP4TableHelper::tell ()
{
	if (m_valid == 0)
		return m_CurrentOffset;
	else
		return (m_CurrentOffset + m_tmpidx);
}

RMuint32 MP4TableHelper::rewind32 (RMuint32 *error)
{
	ASSERT (m_valid);
	if (m_IsInitialized == 0)
		return 0;
	*error = 0;
	if (m_tmpidx < 4)
	{
		ASSERT (m_tmpidx == 0);
		RMuint32 current_position = (m_CurrentOffset - m_StartingOffset) - (m_valid + m_tmpidx);
		if (current_position == 0)
		{
			*error = 1;
			return 0;
		}
		if (current_position > (MP4_MIN_READ_LENGTH - 64))
		{
			seek (current_position - (MP4_MIN_READ_LENGTH - 64));
			get32 (error);
		}
		else
		{
			seek (0);
			get32 (error);
		}
		seek (current_position - 4);
		ASSERT (m_valid);
		return 0;
	}
	m_valid += 4;
	m_tmpidx -= 4;
	return 0;
}

RMuint32 MP4TableHelper::entrycount ()
{
	if (m_IsInitialized == 0)
		return 0;
	return m_entrycount;
}

RMuint32 MP4TableHelper::InitCallbackTable (MP4_CALLBACK_TABLE *pTable)
{
	m_CallbackTable.context = pTable->context;
	m_CallbackTable.fopen = pTable->fopen;
	m_CallbackTable.fseek = pTable->fseek;
	m_CallbackTable.ftell = pTable->ftell;
	m_CallbackTable.fread = pTable->fread;
	m_CallbackTable.fclose = pTable->fclose;
	m_CallbackTable.addref = pTable->addref;
	m_CallbackTable.release = pTable->release;
	m_CallbackTable.info = pTable->info;
	m_CallbackTable.getData = pTable->getData;
	m_CallbackTable.putDSI = pTable->putDSI;
	m_CallbackTable.putChunk = pTable->putChunk;
	return 0;
}

RMuint32 MP4TableHelper::reload ()
{
	RMuint32 n, t, s;

	if (m_IsInitialized == 0)
		return 0;

	if (m_TableLeft == 0)
		return 0;

	t = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
	m_CallbackTable.fseek (m_handle, m_CurrentOffset, SEEK_SET, m_CallbackTable.context);
	n = MP4_MIN_READ_LENGTH - m_tmpidx;
	ASSERT ((RMint32)n <= MP4_MIN_READ_LENGTH);
	memmove (m_tmp, m_tmp+m_tmpidx, n);
	ASSERT (n < 8);
	m_tmpidx = n;
	n = MP4_MIN_READ_LENGTH - m_tmpidx;
	ASSERT ((RMint32)n > 0);
	n = MP4_MIN (n, (m_TableLength - (m_CurrentOffset - m_StartingOffset)));
	ASSERT ((RMint32)n > 0);
	DEBUGMSG (0, ("reload: %d @ %d (m_tmpidx = %d)\n", (int)n, (int)m_CurrentOffset, (int)m_tmpidx));

	s = n;
	n = m_CallbackTable.fread (m_handle, m_tmp+m_tmpidx, n, m_CallbackTable.context);
	ASSERT (n);
	if (n == 0)
	{
		DEBUGMSG (1, ("read error of %d bytes @ offset %d (%d)\n", (int)s, (int)m_CurrentOffset, (int)m_CallbackTable.ftell (m_handle, m_CallbackTable.context)));
		m_CallbackTable.info (MP4_MSG_READ_ERROR, 0, m_CallbackTable.context);
	}
	
	m_TableLeft -= n;
	m_CurrentOffset += n;
	n += m_tmpidx;
	m_tmpidx = 0;

	m_CallbackTable.fseek (m_handle, t, SEEK_SET, m_CallbackTable.context);
	return n;
}

RMuint8 MP4TableHelper::get8 (RMuint32 *error)
{
	if (m_IsInitialized == 0)
		return 0;

	*error = 0;
	if (m_valid < 1)
	{
		m_valid = reload ();
	}
	ASSERT (m_valid);
	if (m_valid == 0)
	{
		*error = 1;
		return 0;
	}
	return m_tmp[m_tmpidx];
}

RMuint32 MP4TableHelper::get32 (RMuint32 *error)
{
	if (m_IsInitialized == 0)
	{
		*error = 1;
		return 0;
	}

	RMuint32 tmp;
	*error = 0;
	if (m_valid < 4)
	{
		m_valid = reload ();
	}
	if (m_valid == 0)
	{
		*error = 1;
		return 0;
	}
	tmp = (
		((RMuint32)(m_tmp[m_tmpidx+0]) << 24) | 
		((RMuint32)(m_tmp[m_tmpidx+1]) << 16) |
		((RMuint32)(m_tmp[m_tmpidx+2]) <<  8) | 
		((RMuint32)(m_tmp[m_tmpidx+3]) <<  0));
	return tmp;
}

RMuint64 MP4TableHelper::get64 (RMuint32 *error)
{
	RMuint64 tmp;
	*error = 0;
	if (m_valid < 8)
	{
		m_valid = reload ();
	}
	ASSERT (m_valid);
	if (m_valid == 0)
	{
		*error = 1;
		return 0;
	}
	tmp = (
		((RMuint64)m_tmp[m_tmpidx+0] << 56) | 
		((RMuint64)m_tmp[m_tmpidx+1] << 48) |
		((RMuint64)m_tmp[m_tmpidx+2] << 40) | 
		((RMuint64)m_tmp[m_tmpidx+3] << 32) |
		((RMuint64)m_tmp[m_tmpidx+4] << 24) | 
		((RMuint64)m_tmp[m_tmpidx+5] << 16) |
		((RMuint64)m_tmp[m_tmpidx+6] <<  8) | 
		((RMuint64)m_tmp[m_tmpidx+7] <<  0));
	return tmp;
}

RMuint32 MP4TableHelper::advance8 (RMuint32 *error)
{
	if (m_IsInitialized == 0)
		return 0;

	*error = 0;
	if (m_valid < 1)
	{
		ASSERT (0);
		*error = 1;
		return 0;
	}
	m_valid -= 1;
	m_tmpidx += 1;
	return 0;
}

RMuint32 MP4TableHelper::advance32 (RMuint32 *error)
{
	if (m_IsInitialized == 0)
		return 0;

	*error = 0;
	if (m_valid < 4)
	{
		ASSERT (0);
		*error = 1;
		return 0;
	}
	m_valid -= 4;
	m_tmpidx += 4;
	return 0;
}

RMuint32 MP4TableHelper::advance64 (RMuint32 *error)
{
	if (m_IsInitialized == 0)
		return 0;

	*error = 0;
	if (m_valid < 8)
	{
		ASSERT (0);
		*error = 1;
		return 0;
	}
	m_valid -= 8;
	m_tmpidx += 8;
	return 0;
}

