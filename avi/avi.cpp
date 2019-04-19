//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : AVI(DivX, XviD) Demux Code
// 
// $Source: /home/cvsroot/TAVI020/tavi/avi/avi.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/04/07 04:29:37 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: avi.cpp,v $
// Revision 1.3  2006/04/07 04:29:37  zstein
// - added GetVideoCompression( void ) and
//   released over 30fps error.
//
// Revision 1.2  2006/04/05 02:07:36  zstein
// - added ReInitCallback(...)
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.26  2006/01/06 09:08:59  etnlwind
// Update for Broken Video File
//
// Revision 1.25  2005/11/03 03:14:44  etnlwind
// AVI 비디오 재생시 프레임레이트 정보가 잘못되어 있는지 검사해보고
// 잘못되어 있을경우 대화상자로 사용자에게 현상황을 알리고 재생여부를 확인
// 사용자가 재생을 시도하면, Seek기능을 막고 재생하도록 수정
// (프레임레이트 정보가 깨진 비디오 파일은 주로 Seek기능에는
// 문제가 생기고, 가끔씩 재생중 멈추거나 끊어지는 경우도 발생함)
//
// 비디오 재생중 TASK버튼 누를경우 배경을 지우지 않고 메인으로 가는경우 수정
//
// 비디오 디렉토리 반복 수정
// 비디오 재생시간 잔상제거
// 비디오 전체시간 위치 수정
// 비디오 재생중 볼륨조절시 비디오재생이 좀더 매끄럽게 수정
//
// Revision 1.24  2005/11/01 04:25:00  etnlwind
// Update for DX50 Invalid Header
//
// Revision 1.23  2005/10/13 01:25:23  etnlwind
// Update for Header Info
//
//*****************************************************************************

#include <stdio.h>

#include "avi.h"
#include <tavi_structures.h> 
typedef unsigned long long U64; 

#if 0
#ifdef _WIN32
#include <windows.h>
static void debug_outs (char *szMsg, ...)
{
	char szBuf[256];
	wvsprintf (szBuf, szMsg, (LPSTR)(&szMsg + 1));
	OutputDebugString (szBuf);
}
#else
#include <stdio.h>
#define debug_outs	printf
#endif
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

#define AVI_DEMUX_STATE_CHUNK_NAME_BYTE0		0
#define AVI_DEMUX_STATE_CHUNK_NAME_BYTE1		1
#define AVI_DEMUX_STATE_CHUNK_NAME_BYTE2		2
#define AVI_DEMUX_STATE_CHUNK_NAME_BYTE3		3
#define AVI_DEMUX_STATE_CHUNK_LENGTH_BYTE0		4
#define AVI_DEMUX_STATE_CHUNK_LENGTH_BYTE1		5
#define AVI_DEMUX_STATE_CHUNK_LENGTH_BYTE2		6
#define AVI_DEMUX_STATE_CHUNK_LENGTH_BYTE3		7
#define AVI_DEMUX_STATE_WRITE_CHUNK				8
#define AVI_DEMUX_STATE_RESTORE_STATE			9
#define AVI_DEMUX_STATE_INVALID					10

#define AVI_MIN(X,Y) (((X)<=(Y))?(X):(Y))	// to find the min between two number.

extern TAVI_TIME totaltime;
extern VIDEOFILE_PLAYINFO gVideofilePlayinfo; 

AVIDemux::AVIDemux ()
{
	DEBUGMSG (1, ("AVIDemux::AVIDemux\n"));
	m_handle = 0;
	m_savedbuf = 0;
	m_savedbuflen = 0;
	m_savedp = 0;
	Init ();
}

AVIDemux::~AVIDemux ()
{
	DEBUGMSG (1, ("AVIDemux::~AVIDemux\n"));
	if (m_handle)
	{
		ASSERT (m_CallbackTable.fclose);
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
	}
}

AVI_DEMUX_ERROR AVIDemux::Init ()
{
	DEBUGMSG (1, ("AVIDemux::Init\n"));
	if (m_handle)
	{
		if (m_CallbackTable.fclose)
			m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
	}
	m_CallbackTable.fopen = 0;
	m_CallbackTable.fread = 0;
	m_CallbackTable.fseek = 0;
	m_CallbackTable.fclose = 0;
	m_CallbackTable.addref = 0;
	m_CallbackTable.release = 0;
	m_CallbackTable.info = 0;
	m_CallbackTable.putChunk = 0;
	m_handle = 0;
	m_nStreamHeader = 0;
	m_nStreamFormat = 0;
	m_moviOffset = 0;
	m_sizeOfFile = 0;
	m_chunkstart = 0;
	m_bytecounter = 0;
	RMint32 i;
	for (i=0; i<AVI_MAX_STREAMS_SUPPORTED; i++)
		m_vbrmp3[i] = 0;
	for (i=0; i<AVI_MAX_INDEX_HELPER; i++)
	{
		m_IndexHelper[i].chunkOffset = 0;
		m_IndexHelper[i].indexOffset = 0;
		m_IndexHelper[i].videoFrameCount = 0;
		for (RMint32 j=0; j<AVI_MAX_STREAMS_SUPPORTED; j++)
			m_IndexHelper[i].audioByteOrFrameCount[j] = 0;
	}
	m_avih.TotalFrames = 0;
	m_demuxstate = AVI_DEMUX_STATE_INVALID;
	m_idx1Offset = 0;
	m_currentFramePosition = 0;
	m_KeyFramesOnly = 0;
	m_IndexCacheValid = 0;
	m_IndexCacheOffset = 0;
	m_KeyFrameDelayInMS = 500;
	m_IndexIsRelativeToStartOfFile = 0;
	return AVI_DEMUX_ERROR_NO_ERROR;
}

AVI_DEMUX_ERROR AVIDemux::InitCallbackTable (AVI_CALLBACK_TABLE *pCallbackTable)
{
	DEBUGMSG (1, ("AVIDemux::InitCallbackTable\n"));

	m_CallbackTable.context = pCallbackTable->context;
	m_CallbackTable.fopen = pCallbackTable->fopen;
	m_CallbackTable.fread = pCallbackTable->fread;
	m_CallbackTable.ftell = pCallbackTable->ftell;
	m_CallbackTable.fseek = pCallbackTable->fseek;
	m_CallbackTable.fclose = pCallbackTable->fclose;
	m_CallbackTable.addref = pCallbackTable->addref;
	m_CallbackTable.release = pCallbackTable->release;
	m_CallbackTable.info = pCallbackTable->info;
	m_CallbackTable.getbuffer = pCallbackTable->getbuffer;
	m_CallbackTable.putChunk = pCallbackTable->putChunk;
	m_CallbackTable.loading = pCallbackTable->loading;
	m_CallbackTable.gettimems = pCallbackTable->gettimems;

	return AVI_DEMUX_ERROR_NO_ERROR;
}

AVI_DEMUX_ERROR AVIDemux::ReInitCallbackTable( AVI_CALLBACK_TABLE *pCallback ) 
{
    int pos;
    
    if( m_handle ) {
        if( m_CallbackTable.ftell ) {
            pos = ( int )m_CallbackTable.ftell( m_handle, NULL );
        }
    }
    InitCallbackTable( pCallback );
    if( m_handle ) {
        if( m_CallbackTable.fseek ) {
            m_CallbackTable.fseek( m_handle, (RMuint32)pos, NULL );
        }
    }

    return AVI_DEMUX_ERROR_NO_ERROR;
}

static RMint32 isChar (RMint8 character)
{
	if(((character >= 'a') &&
	    (character <= 'z')) ||
	   ((character >= 'A') &&
	    (character <= 'Z')) ||
	   ((character >= '0') &&
	    (character <= '9')) ||
	   (character == ' '))
		return 1;
	return 0;
}

AVI_DEMUX_ERROR AVIDemux::DemuxFile_V19(RMuint16 *filename)
{
	ASSERT (m_handle == 0);
	ASSERT (m_CallbackTable.fopen);

	m_handle = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	ASSERT (m_handle);
	if (m_handle == 0)
	{
		DEBUGMSG (1, ("fopen (%s) failed\n", filename));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}
	AVI_CHUNK chunk;
	RMuint32 name, saveposition;
	RMuint32 n;
	ASSERT (sizeof(chunk) == 8);
	// verify that this is a AVI RIFF file
	n = m_CallbackTable.fread (m_handle, &chunk, sizeof(chunk), m_CallbackTable.context);
	ASSERT (n == sizeof(chunk));
	if (n != sizeof(chunk))
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		DEBUGMSG (1, ("not an AVI RIFF file: too short\n"));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}	
	if (chunk.Name != AVI_FOURCC ('R','I','F','F'))
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		DEBUGMSG (1, ("not an AVI RIFF file: no RIFF header\n"));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}
	m_RIFFlength = chunk.Length;
	RMuint32 rifflength = m_RIFFlength;
	ASSERT (sizeof(name) == 4);
	// next 4 bytes must be 'AVI '
	n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
	ASSERT (n == sizeof(name));
	if (name != AVI_FOURCC('A','V','I',' '))
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		DEBUGMSG (1, ("not an AVI RIFF file: name is not 'AVI '\n"));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}
	rifflength -= sizeof(name);
	ASSERT ((RMint32)rifflength > 0);
	RMuint32 listOffset;
	RMuint8	sizebuf[4];
	RMuint32 subListLength, subChunkLength, position, mainchunkLength;
	// actually not the length of the file, but the length of the riff 
	// chunk
	m_sizeOfFile = rifflength;

	ASSERT (m_CallbackTable.loading);
	m_CallbackTable.loading (0, m_CallbackTable.context);
	// parse all the chunks
	while (rifflength > 0)
	{
		// read the next chunk
		rifflength -= sizeof (chunk);
		n = m_CallbackTable.fread (m_handle, &chunk, sizeof (chunk), m_CallbackTable.context);
		if(n != sizeof(chunk))
		{
//			printf("n = %d, sizeof(chunk) = %d\n", n, sizeof(chunk));
			DEBUGMSG(1, ("n = %d, sizeof(chunk) = %d\n", n, sizeof(chunk)));
			break;
		}
		ASSERT (n == sizeof (chunk));	
		ASSERT (chunk.Length <= rifflength);

		if (n == 0)
		{
			DEBUGMSG (1, ("read error?\n"));
			if (m_moviOffset)
			{
				// ok a movi offset was found, let's do our best to play 
				// this file - in anycase, the file is corrupted.
				// if an idx1 chunk has not been found, then you won't be
				// able to seek or do I frames
				rifflength = 0;
				continue;
			}
			// not even a m_moviOffset has been found - abort
			return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
		}

		if (((isChar ((RMint8)((chunk.Name >> 24) & 0xff))) == 0) ||
			((isChar ((RMint8)((chunk.Name >> 16) & 0xff))) == 0) ||
			((isChar ((RMint8)((chunk.Name >>  8) & 0xff))) == 0) ||
			((isChar ((RMint8)((chunk.Name >>  0) & 0xff))) == 0))
		{
			ASSERT (0);
			// invalid chunk name
			n = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
			m_CallbackTable.fseek (m_handle, n - (sizeof (chunk) - 1), m_CallbackTable.context);
			rifflength += (sizeof (chunk) - 1);
			return AVI_DEMUX_ERROR_INVALID_CHUNK_NAME;
			continue;
		}

		switch (chunk.Name)
		{
		case AVI_FOURCC ('L','I','S','T'):
			// this is the main LIST chunk
			mainchunkLength = chunk.Length;
			DEBUGMSG (1, ("LIST chunk\n"));
			listOffset = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
			// we recognize 3 kinds of LIST chunks: 'hdrl', 'strl', amd 'movi'
			n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
			ASSERT (n == sizeof(name));
			switch (name)
			{
			case AVI_FOURCC ('h','d','r','l'):
				n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
				ASSERT (n == sizeof(name));
				ASSERT (name == AVI_FOURCC ('a','v','i','h'));
				n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
				ASSERT (n == sizeof(subChunkLength));
				position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
				ASSERT (subChunkLength >= sizeof(m_avih));
				n = m_CallbackTable.fread (m_handle, &m_avih, sizeof(m_avih), m_CallbackTable.context);
				ASSERT (n == sizeof (m_avih));
				DEBUGMSG (1, ("aviHeader:\n"));
				DEBUGMSG (1, (" MicroSecPerFrame: %lu\n", m_avih.MicroSecPerFrame));
				DEBUGMSG (1, (" MaxBytesPerSec: %lu\n", m_avih.MaxBytesPerSec));
				DEBUGMSG (1, (" Flags: %lu\n", m_avih.Flags));
				DEBUGMSG (1, (" TotalFrames: %lu\n", m_avih.TotalFrames));
				DEBUGMSG (1, (" InitialFrames: %lu\n", m_avih.InitialFrames));
				DEBUGMSG (1, (" Streams: %lu\n", m_avih.Streams));
				DEBUGMSG (1, (" SuggestedBufferSize: %lu\n", m_avih.SuggestedBufferSize));
				DEBUGMSG (1, (" Width: %lu\n", m_avih.Width));
				DEBUGMSG (1, (" Height: %lu\n", m_avih.Height));
				DEBUGMSG (1, (" Scale: %lu\n", m_avih.Scale));
				DEBUGMSG (1, (" Rate: %lu\n", m_avih.Rate));
				DEBUGMSG (1, (" Start: %lu\n", m_avih.Start));
				DEBUGMSG (1, (" Length: %lu\n", m_avih.Length));

				if( m_avih.Rate <= 64000 && m_avih.Rate > 1000 )
					m_IsValidScaleAndRate_OnAviMainHeader = 1;
				else
					m_IsValidScaleAndRate_OnAviMainHeader = 0;

				gVideofilePlayinfo.OriginalWidth	= m_avih.Width;
				gVideofilePlayinfo.OriginalHeight	= m_avih.Height;

				totaltime.SecOnly 	= ((U64) m_avih.TotalFrames * (U64)m_avih.MicroSecPerFrame) /1000000;
				totaltime.hours 		= (int) (totaltime.SecOnly / 3600);
				totaltime.minutes 	= (int) ((totaltime.SecOnly - totaltime.hours * 3600) / 60);
				totaltime.seconds 	= (int) (totaltime.SecOnly - totaltime.hours*3600 - totaltime.minutes*60);

				ASSERT (m_CallbackTable.info);
				m_CallbackTable.info (AVI_DEMUX_MSG_AVIHDR, &m_avih, m_CallbackTable.context);
				m_CallbackTable.fseek (m_handle, position + subChunkLength, m_CallbackTable.context);
				mainchunkLength -= 4;	// for 'hdrl'			
				mainchunkLength -= (subChunkLength + 8);	// for 'avih'
				
				// XXX to do: handle more than one sublist
				while ((RMint32)mainchunkLength > 0)
				{
					n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
					ASSERT (n == sizeof(name));					
					n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subListLength), m_CallbackTable.context);
					ASSERT (n == sizeof(subChunkLength));
					mainchunkLength -= 8;	// for the header of this subchunk
					mainchunkLength -= subChunkLength;	// for this subchunk
					if (name == AVI_FOURCC ('L','I','S','T'))
					{					
						subListLength = subChunkLength;
						n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
						ASSERT (n == sizeof(name));
						ASSERT (subListLength >= 4);
						subListLength -= 4;
						if (name == AVI_FOURCC ('s','t','r','l'))
						{
							while ((RMint32)subListLength > 3)
							{							
								n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
								subListLength -= 4;
								ASSERT (n == sizeof(name));
								switch (name)
								{
								case AVI_FOURCC ('s','t','r','h'):
									n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
									ASSERT (n == sizeof(subChunkLength));
									subListLength -= 4;
									position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
									ASSERT (subChunkLength >= sizeof(AVI_STREAM_HEADER));
									n = m_CallbackTable.fread (m_handle, &m_StreamHeaders[m_nStreamHeader], sizeof(AVI_STREAM_HEADER), m_CallbackTable.context);
									ASSERT (n == sizeof(AVI_STREAM_HEADER));
									ASSERT (m_CallbackTable.info);			
									DEBUGMSG (1, ("strh:\n"));
									DEBUGMSG (1, (" rate: %lu (0x%08lx)\n", 
										AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Rate),
										AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Rate)));
									DEBUGMSG (1, (" scale: %lu (0x%08lx)\n", 
										AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Scale),
										AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Scale)));

									m_CallbackTable.info (AVI_DEMUX_MSG_STREAMHDR, &m_StreamHeaders[m_nStreamHeader], m_CallbackTable.context);
                                    #if 0
									if( AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Rate) < 64000 &&
										AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Rate) > 1000 )
										{
										DEBUGMSG(1,("-strh Rate : [valid]\n"));
										DEBUGMSG(1, ("-m_IsValidScaleAndRate_OnAviMainHeader : [%d]\n", m_IsValidScaleAndRate_OnAviMainHeader));

										SuggestedRate = AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Rate);
										SuggestedScale = AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Scale);

										if(!m_IsValidScaleAndRate_OnAviMainHeader)
											{
											m_avih.Rate = SuggestedRate;
											m_avih.Scale = SuggestedScale;
											}
										}
									
									else
										{
										DEBUGMSG(1,("-strh Rate : [invalid]\n"));
										DEBUGMSG(1, ("-m_IsValidScaleAndRate_OnAviMainHeader : [%d]\n", m_IsValidScaleAndRate_OnAviMainHeader));
										
										if(m_IsValidScaleAndRate_OnAviMainHeader)
											{
											(&(m_StreamHeaders[m_nStreamHeader]))->Rate  = m_avih.Rate;
											(&(m_StreamHeaders[m_nStreamHeader]))->Scale = m_avih.Scale;
											SuggestedRate = m_avih.Rate;
											SuggestedScale = m_avih.Scale;
											}
										else if( !m_IsInvalidFPSVideoEnabled )
											{
											m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
											m_handle = 0;
											DEBUGMSG (1, ("invalid frame rate info.\n"));
											return AVI_DEMUX_ERROR_FPS_VALUE_INVALID;
											}
										
										}
									#endif

									DEBUGMSG(1, ("-m_avih.Rate(%lu), m_avih.Scale(%lu)\n", m_avih.Rate, m_avih.Scale));
									DEBUGMSG(1, ("-SuggestedRate(%lu), SuggestedScale(%lu)\n", SuggestedRate, SuggestedScale));
									DEBUGMSG(1, ("-m_StreamHeaders[m_nStreamHeader].Rate: %lu\n", (&(m_StreamHeaders[m_nStreamHeader]))->Rate));
									DEBUGMSG(1, ("-m_StreamHeaders[m_nStreamHeader].Scale: %lu\n", (&(m_StreamHeaders[m_nStreamHeader]))->Scale));
									DEBUGMSG(1, ("-------------------------------------------------------------------------------\n"));

									if (AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType))
									{
										DEBUGMSG (1, (" fccType: %c%c%c%c\n", 
												(RMint8)(AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType) >>  0),
												(RMint8)(AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType) >>  8), 
												(RMint8)(AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType) >> 16), 
												(RMint8)(AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType) >> 24)));
									}
									else
									{
										DEBUGMSG (1, (" fccType: 0000\n"));
									}
									subListLength -= subChunkLength;
									m_CallbackTable.fseek (m_handle, position+subChunkLength, m_CallbackTable.context);
									m_nStreamHeader++;
									break;
								case AVI_FOURCC ('s','t','r','f'):
									if (AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) == AVI_FOURCC('v','i','d','s'))
									{
										n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
										ASSERT (n == sizeof(subChunkLength));
										subListLength -= 4;
										position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
										ASSERT (subChunkLength >= sizeof(AVI_BITMAPINFO));
										n = m_CallbackTable.fread (m_handle, &m_StreamFormats[m_nStreamFormat], sizeof(AVI_BITMAPINFO), m_CallbackTable.context);
										ASSERT (n == sizeof(AVI_BITMAPINFO));
										ASSERT (m_CallbackTable.info);
										m_CallbackTable.info (AVI_DEMUX_MSG_BITMAPINFO, &m_StreamFormats[m_nStreamFormat], m_CallbackTable.context);
										DEBUGMSG (1, ("vids:\n"));
										DEBUGMSG (1, (" m_nStreamFormat: %d\n",m_nStreamFormat));
										DEBUGMSG (1, (" width: %u\n",m_StreamFormats[m_nStreamFormat].bmi.Width));
										DEBUGMSG (1, (" height: %u\n",m_StreamFormats[m_nStreamFormat].bmi.Height));
										subListLength -= subChunkLength;
										m_CallbackTable.fseek (m_handle, position+subChunkLength, m_CallbackTable.context);
									}
									else if (AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) == AVI_FOURCC('a','u','d','s'))
									{
										n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
										ASSERT (n == sizeof(subChunkLength));
										subListLength -= 4;
										position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
										ASSERT (subChunkLength >= sizeof(AVI_WAVEFORMATEX));
										n = m_CallbackTable.fread (m_handle, &m_StreamFormats[m_nStreamFormat], sizeof(AVI_WAVEFORMATEX), m_CallbackTable.context);
										ASSERT (n == sizeof(AVI_WAVEFORMATEX));
										ASSERT (m_CallbackTable.info);
										m_CallbackTable.info (AVI_DEMUX_MSG_WAVEFORMATEX, &m_StreamFormats[m_nStreamFormat], m_CallbackTable.context);
										if ((AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], Scale) == 1152) ||
											((AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], Rate) == m_StreamFormats[m_nStreamFormat].wfx.nSamplesPerSec) &&
											 (AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], SampleSize) == 0)))
										{
											DEBUGMSG (1, ("assuming variable bitrate mp3\n"));
											m_vbrmp3[m_nStreamFormat] = 1;
										}
										DEBUGMSG (1, ("auds:\n"));
										DEBUGMSG (1, (" formattag: %u\n", m_StreamFormats[m_nStreamFormat].wfx.wFormatTag));
										DEBUGMSG (1, (" channels: %u\n", m_StreamFormats[m_nStreamFormat].wfx.nChannels));
										DEBUGMSG (1, (" samplespersec: %u\n", m_StreamFormats[m_nStreamFormat].wfx.nSamplesPerSec));
										DEBUGMSG (1, (" bitspersample: %u\n", m_StreamFormats[m_nStreamFormat].wfx.wBitsPerSample));
										subListLength -= subChunkLength;
										m_CallbackTable.fseek (m_handle, position+subChunkLength, m_CallbackTable.context);
									}
									else
									{
										ASSERT (0);
										DEBUGMSG (1, ("ignoring unknown fcc type = %c%c%c%c\n",
											AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) >>  0, 
											AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) >>  8, 
											AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) >> 16, 
											AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) >> 24));
										n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
										subListLength -= subChunkLength;
										m_CallbackTable.fseek (m_handle, m_CallbackTable.ftell (m_handle, m_CallbackTable.context)+subChunkLength, m_CallbackTable.context);
									}
									m_nStreamFormat++;
									break;
								case AVI_FOURCC ('i','n','d','x'):
									saveposition = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
									position = saveposition;

									n = m_CallbackTable.fread (m_handle, sizebuf, 4, m_CallbackTable.context);
									n = (long)sizebuf[0] +
										((long)sizebuf[1] << 8) +
										((long)sizebuf[2] << 16) +
										((long)sizebuf[3] << 24) + 4;
									saveposition += n;

									//12Byte를 더한 이유는 다음에 인덱스가 비디오 인지 오디오 인지 구분하기 위해서다.
									m_CallbackTable.fseek (m_handle, position + 12, m_CallbackTable.context);
									n = m_CallbackTable.fread (m_handle, sizebuf, 4, m_CallbackTable.context);
									
									//Video Index 정보인지 검사
									if(sizebuf[2] == 'd' && (sizebuf[3] == 'c' || sizebuf[3] == 'b')) 
									{
										//videoidxf = 1;
										position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
										m_CallbackTable.fseek (m_handle, position + 12, m_CallbackTable.context);
										//인덱스 파일 절대 위치를 가져온다.
										n = m_CallbackTable.fread (m_handle, sizebuf, 4, m_CallbackTable.context);
//										videoidxstart = (unsigned long)sizebuf[0] +
//											((unsigned long)sizebuf[1] << 8) +
//											((unsigned long)sizebuf[2] << 16) +
//											((unsigned long)sizebuf[3] << 24);
									}
									//Audio Index 정보인지 검사
									else if(sizebuf[2] == 'w' && (sizebuf[3] == 'c' || sizebuf[3] == 'b')) 
									{
										//audioidxf = 1;
										position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
										m_CallbackTable.fseek (m_handle, position + 12, m_CallbackTable.context);
										//인덱스 파일 절대 위치를 가져온다.
										n = m_CallbackTable.fread (m_handle, sizebuf, 4, m_CallbackTable.context);
//										audioidxstart = (unsigned long)sizebuf[0] +
//											((unsigned long)sizebuf[1] << 8) +
//											((unsigned long)sizebuf[2] << 16) +
//											((unsigned long)sizebuf[3] << 24);
									}
									
									m_CallbackTable.fseek (m_handle, saveposition, m_CallbackTable.context);
									
									//indxflag = 1;
									subListLength = 0; //subChunkLength;
									break;
								case AVI_FOURCC ('s','t','r','n'):
									n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
									subListLength -= 4;
									position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
									subListLength -= subChunkLength;
									m_CallbackTable.fseek (m_handle, position+subChunkLength, m_CallbackTable.context);
									break; 										
								default:
									DEBUGMSG (1, ("ignoring unknown list type = %c%c%c%c\n",
										name >>  0, name >>  8, name >> 16, name >> 24));
									n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
									ASSERT (n == sizeof(subChunkLength));
									subListLength -= 4;
									m_CallbackTable.fseek (m_handle, m_CallbackTable.ftell (m_handle, m_CallbackTable.context)+subChunkLength, m_CallbackTable.context);
									subListLength -= subChunkLength;
									break;
								}
								if (m_nStreamFormat >= AVI_MAX_STREAMS_SUPPORTED)
									subListLength = 0;
							}
							ASSERT ((RMint32)subListLength >= 0);
							ASSERT ((RMint32)subListLength <= 3);
							if (((RMint32)subListLength > 0) && ((RMint32)subListLength < 4))
							{
								RMuint32 dummy;
								m_CallbackTable.fread (m_handle, &dummy, subListLength, m_CallbackTable.context);
							}
						}
					}
				}
				break;
			case AVI_FOURCC ('m','o','v','i'):
				DEBUGMSG (1, ("movi chunk\n"));
				m_moviOffset = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
				break;
			default:
				DEBUGMSG (1, ("unknown list type = %c%c%c%c\n",
					name >>  0, name >>  8, name >> 16, name >> 24));
				break;
			}
			m_CallbackTable.fseek (m_handle, listOffset + chunk.Length, m_CallbackTable.context);
			rifflength -= chunk.Length;
			break;
		case AVI_FOURCC ('i','d','x','1'):
			DEBUGMSG (1, ("idx1 chunk\n"));
			n = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
			m_idx1Offset = n;
			m_idx1Length = chunk.Length;
			{
				// process idx1 chunk
				AVI_INDEXENTRY tmp[512];
				RMint32 i, j, x, n, nindexentries, indexoffset, tindexentries;
				tindexentries = chunk.Length / sizeof (AVI_INDEXENTRY);
				nindexentries = tindexentries;
				DEBUGMSG (1, ("%d index entries found\n", nindexentries));
				RMuint32 videoFrameCount = 0;
				RMuint32 audioByteOrFrameCount[AVI_MAX_STREAMS_SUPPORTED];
				for (i=0; i<AVI_MAX_STREAMS_SUPPORTED; i++)
					audioByteOrFrameCount[i] = 0;
				m_nIndexHelper = 0;
				x = 1;
				indexoffset = 0;
				while (nindexentries)
				{
					n = AVI_MIN (512, nindexentries);
					m_CallbackTable.fread (m_handle, tmp, sizeof(AVI_INDEXENTRY)*n, m_CallbackTable.context);
					for (i=0; i<n; i++)
					{
						if (((tmp[i].ckid & 0xffff0000) == 0x62640000) ||
							((tmp[i].ckid & 0xffff0000) == 0x63640000))
						{
							if (videoFrameCount == 0)
							{
								DEBUGMSG (1, ("checking validity of chunk index ...\n"));
								// try to check if the index given is 
								// relative to the start of the file or
								// if it is relative to the start of the movi chunk
								// the spec. says that it should be relative to the
								// start of the movi chunk.
								RMuint32 saved_position, tmpckid;
								saved_position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
								m_CallbackTable.fseek (m_handle, tmp[i].dwChunkOffset, m_CallbackTable.context);
								m_CallbackTable.fread (m_handle, &tmpckid, sizeof(tmpckid), m_CallbackTable.context);
								DEBUGMSG (1, ("tmpckid = %08lx\n", tmpckid));
								if (((tmpckid & 0xffff0000) == 0x62640000) ||
									((tmpckid & 0xffff0000) == 0x63640000))
								{
									if (m_CallbackTable.info (AVI_DEMUX_MSG_INDEX_CHECK, 0, m_CallbackTable.context))
									{
										DEBUGMSG (1, ("Assuming chunk indices are relative to the start of the file.\n"));
										m_IndexIsRelativeToStartOfFile = 1;
									}
								}
								m_CallbackTable.fseek (m_handle, saved_position, m_CallbackTable.context);
							}
							videoFrameCount++;
							if ((x == 1) && ((tmp[i].dwFlags & AVI_FLAGS_KEYFRAME) == 0));
							else
								x--;
						}
						else 
						if ((tmp[i].ckid & 0xffff0000) == 0x62770000)
						{
							RMuint32 idx = (((tmp[i].ckid & 0xff)-0x30) << 8) | (((tmp[i].ckid & 0xff00) >> 8)-0x30);
							if (idx < AVI_MAX_STREAMS_SUPPORTED)
							{
								if (m_vbrmp3[idx])
									audioByteOrFrameCount[idx]++;
								else
									audioByteOrFrameCount[idx] += tmp[i].dwChunkLength;
							}
						}
						if (x == 0)
						{
							m_IndexHelper[m_nIndexHelper].indexOffset = indexoffset;
							m_IndexHelper[m_nIndexHelper].chunkOffset = tmp[i].dwChunkOffset;
							ASSERT ((RMint32)(videoFrameCount - 1) >= 0);
							m_IndexHelper[m_nIndexHelper].videoFrameCount = videoFrameCount - 1;
							for (j=0; j<AVI_MAX_STREAMS_SUPPORTED; j++)
								m_IndexHelper[m_nIndexHelper].audioByteOrFrameCount[j] = audioByteOrFrameCount[j];
							x = ((m_avih.TotalFrames / AVI_MAX_INDEX_HELPER) + 1);
							m_nIndexHelper++;
						}
						indexoffset++;
					}
					nindexentries -= n; 
					DEBUGMSG (1, ("%d index entries left\n", nindexentries));
					ASSERT (m_CallbackTable.loading);
					m_CallbackTable.loading ((tindexentries - nindexentries) * 100 / tindexentries, m_CallbackTable.context);					
				}
			}
			m_CallbackTable.fseek (m_handle, n + chunk.Length, m_CallbackTable.context);
			rifflength -= chunk.Length;
			break;
		default:
			DEBUGMSG (1, ("dump chunk = %c%c%c%c\n", 
				chunk.Name >>  0,
				chunk.Name >>  8,
				chunk.Name >> 16,
				chunk.Name >> 24));
			n = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
			m_CallbackTable.fseek (m_handle, n + chunk.Length, m_CallbackTable.context);
			rifflength -= chunk.Length;
			break;
		}
		ASSERT (m_CallbackTable.loading);
	}
	ASSERT (m_moviOffset < m_sizeOfFile);
	if (m_moviOffset == 0)
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		DEBUGMSG (1, ("no 'movi' chunk found\n"));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}
	m_CallbackTable.fseek (m_handle, m_moviOffset, m_CallbackTable.context);
	m_demuxstate = AVI_DEMUX_STATE_CHUNK_NAME_BYTE0;
	m_savedbuf = 0;
	m_savedbuflen = 0;
	m_savedp = 0;
	return AVI_DEMUX_ERROR_NO_ERROR;
}

#if 1
AVI_DEMUX_ERROR AVIDemux::DemuxFile (RMuint16 *filename)
{
	ASSERT (m_handle == 0);
	ASSERT (m_CallbackTable.fopen);

	m_handle = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	ASSERT (m_handle);
	if (m_handle == 0)
	{
		DEBUGMSG (1, ("fopen (%s) failed\n", filename));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}
	AVI_CHUNK chunk;
	RMuint32 name, saveposition;
	RMuint32 n;
	ASSERT (sizeof(chunk) == 8);
	// verify that this is a AVI RIFF file
	n = m_CallbackTable.fread (m_handle, &chunk, sizeof(chunk), m_CallbackTable.context);
	ASSERT (n == sizeof(chunk));
	if (n != sizeof(chunk))
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		DEBUGMSG (1, ("not an AVI RIFF file: too short\n"));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}	
	if (chunk.Name != AVI_FOURCC ('R','I','F','F'))
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		DEBUGMSG (1, ("not an AVI RIFF file: no RIFF header\n"));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}
	m_RIFFlength = chunk.Length;
	RMuint32 rifflength = m_RIFFlength;
	ASSERT (sizeof(name) == 4);
	// next 4 bytes must be 'AVI '
	n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
	ASSERT (n == sizeof(name));
	if (name != AVI_FOURCC('A','V','I',' '))
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		DEBUGMSG (1, ("not an AVI RIFF file: name is not 'AVI '\n"));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}
	rifflength -= sizeof(name);
	ASSERT ((RMint32)rifflength > 0);
	RMuint32 listOffset;
	RMuint8	sizebuf[4];
	RMuint32 subListLength, subChunkLength, position, mainchunkLength;
	// actually not the length of the file, but the length of the riff 
	// chunk
	m_sizeOfFile = rifflength;
	

	ASSERT (m_CallbackTable.loading);
	m_CallbackTable.loading (0, m_CallbackTable.context);
	// parse all the chunks
	while (rifflength > 0)
	{
		// read the next chunk
		rifflength -= sizeof (chunk);
		n = m_CallbackTable.fread (m_handle, &chunk, sizeof (chunk), m_CallbackTable.context);
		ASSERT (n == sizeof (chunk));	
		ASSERT (chunk.Length <= rifflength);

		if (n == 0)
		{
			DEBUGMSG (1, ("read error?\n"));
			if (m_moviOffset)
			{
				// ok a movi offset was found, let's do our best to play 
				// this file - in anycase, the file is corrupted.
				// if an idx1 chunk has not been found, then you won't be
				// able to seek or do I frames
				rifflength = 0;
				continue;
			}
			// not even a m_moviOffset has been found - abort
			return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
		}

		if (((isChar ((RMint8)((chunk.Name >> 24) & 0xff))) == 0) ||
			((isChar ((RMint8)((chunk.Name >> 16) & 0xff))) == 0) ||
			((isChar ((RMint8)((chunk.Name >>  8) & 0xff))) == 0) ||
			((isChar ((RMint8)((chunk.Name >>  0) & 0xff))) == 0))
		{
			ASSERT (0);
			// invalid chunk name
			n = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
			m_CallbackTable.fseek (m_handle, n - (sizeof (chunk) - 1), m_CallbackTable.context);
			rifflength += (sizeof (chunk) - 1);
			continue;
		}

		switch (chunk.Name)
		{
		case AVI_FOURCC ('L','I','S','T'):
			// this is the main LIST chunk
			mainchunkLength = chunk.Length;
			DEBUGMSG (1, ("LIST chunk\n"));
			listOffset = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
			// we recognize 3 kinds of LIST chunks: 'hdrl', 'strl', amd 'movi'
			n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
			ASSERT (n == sizeof(name));
			switch (name)
			{
			case AVI_FOURCC ('h','d','r','l'):
				n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
				ASSERT (n == sizeof(name));
				ASSERT (name == AVI_FOURCC ('a','v','i','h'));
				n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
				ASSERT (n == sizeof(subChunkLength));
				position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
				ASSERT (subChunkLength >= sizeof(m_avih));
				n = m_CallbackTable.fread (m_handle, &m_avih, sizeof(m_avih), m_CallbackTable.context);
				ASSERT (n == sizeof (m_avih));
				DEBUGMSG (1, ("aviHeader:\n"));
				DEBUGMSG (1, (" MicroSecPerFrame: %lu\n", m_avih.MicroSecPerFrame));
				DEBUGMSG (1, (" MaxBytesPerSec: %lu\n", m_avih.MaxBytesPerSec));
				DEBUGMSG (1, (" Flags: %lu\n", m_avih.Flags));
				DEBUGMSG (1, (" TotalFrames: %lu\n", m_avih.TotalFrames));
				DEBUGMSG (1, (" InitialFrames: %lu\n", m_avih.InitialFrames));
				DEBUGMSG (1, (" Streams: %lu\n", m_avih.Streams));
				DEBUGMSG (1, (" SuggestedBufferSize: %lu\n", m_avih.SuggestedBufferSize));
				DEBUGMSG (1, (" Width: %lu\n", m_avih.Width));
				DEBUGMSG (1, (" Height: %lu\n", m_avih.Height));
				DEBUGMSG (1, (" Scale: %lu\n", m_avih.Scale));
				DEBUGMSG (1, (" Rate: %lu\n", m_avih.Rate));
				DEBUGMSG (1, (" Start: %lu\n", m_avih.Start));
				DEBUGMSG (1, (" Length: %lu\n", m_avih.Length));

				gVideofilePlayinfo.OriginalWidth	= m_avih.Width;
				gVideofilePlayinfo.OriginalHeight	= m_avih.Height;
				totaltime.SecOnly 	= ((U64) m_avih.TotalFrames * (U64)m_avih.MicroSecPerFrame) /1000000;
				totaltime.hours 		= (int) (totaltime.SecOnly / 3600);
				totaltime.minutes 	= (int) ((totaltime.SecOnly - totaltime.hours * 3600) / 60);
				totaltime.seconds 	= (int) (totaltime.SecOnly - totaltime.hours*3600 - totaltime.minutes*60);

				ASSERT (m_CallbackTable.info);
				m_CallbackTable.info (AVI_DEMUX_MSG_AVIHDR, &m_avih, m_CallbackTable.context);
				m_CallbackTable.fseek (m_handle, position + subChunkLength, m_CallbackTable.context);
				mainchunkLength -= 4;	// for 'hdrl'			
				mainchunkLength -= (subChunkLength + 8);	// for 'avih'
				// XXX to do: handle more than one sublist
				while ((RMint32)mainchunkLength > 0)
				{
					n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
					ASSERT (n == sizeof(name));					
					n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subListLength), m_CallbackTable.context);
					ASSERT (n == sizeof(subChunkLength));
					mainchunkLength -= 8;	// for the header of this subchunk
					mainchunkLength -= subChunkLength;	// for this subchunk
					if (name == AVI_FOURCC ('L','I','S','T'))
					{					
						subListLength = subChunkLength;
						n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
						ASSERT (n == sizeof(name));
						ASSERT (subListLength >= 4);
						subListLength -= 4;
						if (name == AVI_FOURCC ('s','t','r','l'))
						{
							while ((RMint32)subListLength > 3)
							{							
								n = m_CallbackTable.fread (m_handle, &name, sizeof(name), m_CallbackTable.context);
								subListLength -= 4;
								ASSERT (n == sizeof(name));
								switch (name)
								{
								case AVI_FOURCC ('s','t','r','h'):
									n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
									ASSERT (n == sizeof(subChunkLength));
									subListLength -= 4;
									position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
									ASSERT (subChunkLength >= sizeof(AVI_STREAM_HEADER));
									n = m_CallbackTable.fread (m_handle, &m_StreamHeaders[m_nStreamHeader], sizeof(AVI_STREAM_HEADER), m_CallbackTable.context);
									ASSERT (n == sizeof(AVI_STREAM_HEADER));
									ASSERT (m_CallbackTable.info);			
									DEBUGMSG (1, ("strh:\n"));
									DEBUGMSG (1, (" rate: %lu (0x%08lx)\n", 
										AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Rate),
										AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Rate)));
									DEBUGMSG (1, (" scale: %lu (0x%08lx)\n", 
										AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Scale),
										AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], Scale)));
									m_CallbackTable.info (AVI_DEMUX_MSG_STREAMHDR, &m_StreamHeaders[m_nStreamHeader], m_CallbackTable.context);
									if (AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType))
									{
										DEBUGMSG (1, (" fccType: %c%c%c%c\n", 
												(RMint8)(AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType) >>  0),
												(RMint8)(AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType) >>  8), 
												(RMint8)(AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType) >> 16), 
												(RMint8)(AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamHeader], fccType) >> 24)));
									}
									else
									{
										DEBUGMSG (1, (" fccType: 0000\n"));
									}
									subListLength -= subChunkLength;
									m_CallbackTable.fseek (m_handle, position+subChunkLength, m_CallbackTable.context);
									m_nStreamHeader++;
									break;
								case AVI_FOURCC ('s','t','r','f'):
									if (AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) == AVI_FOURCC('v','i','d','s'))
									{
										n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
										ASSERT (n == sizeof(subChunkLength));
										subListLength -= 4;
										position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
										ASSERT (subChunkLength >= sizeof(AVI_BITMAPINFO));
										n = m_CallbackTable.fread (m_handle, &m_StreamFormats[m_nStreamFormat], sizeof(AVI_BITMAPINFO), m_CallbackTable.context);
										ASSERT (n == sizeof(AVI_BITMAPINFO));
										ASSERT (m_CallbackTable.info);
										m_CallbackTable.info (AVI_DEMUX_MSG_BITMAPINFO, &m_StreamFormats[m_nStreamFormat], m_CallbackTable.context);
										DEBUGMSG (1, ("vids:\n"));
										DEBUGMSG (1, (" width: %u\n",m_StreamFormats[m_nStreamFormat].bmi.Width));
										DEBUGMSG (1, (" height: %u\n",m_StreamFormats[m_nStreamFormat].bmi.Width));
										subListLength -= subChunkLength;
										m_CallbackTable.fseek (m_handle, position+subChunkLength, m_CallbackTable.context);
									}
									else if (AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) == AVI_FOURCC('a','u','d','s'))
									{
										n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
										ASSERT (n == sizeof(subChunkLength));
										subListLength -= 4;
										position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
										ASSERT (subChunkLength >= sizeof(AVI_WAVEFORMATEX));
										n = m_CallbackTable.fread (m_handle, &m_StreamFormats[m_nStreamFormat], sizeof(AVI_WAVEFORMATEX), m_CallbackTable.context);
										ASSERT (n == sizeof(AVI_WAVEFORMATEX));
										ASSERT (m_CallbackTable.info);
										m_CallbackTable.info (AVI_DEMUX_MSG_WAVEFORMATEX, &m_StreamFormats[m_nStreamFormat], m_CallbackTable.context);
										if ((AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], Scale) == 1152) ||
											((AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], Rate) == m_StreamFormats[m_nStreamFormat].wfx.nSamplesPerSec) &&
											 (AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], SampleSize) == 0)))
										{
											DEBUGMSG (1, ("assuming variable bitrate mp3\n"));
											m_vbrmp3[m_nStreamFormat] = 1;
										}
										DEBUGMSG (1, ("auds:\n"));
										DEBUGMSG (1, (" formattag: %u\n", m_StreamFormats[m_nStreamFormat].wfx.wFormatTag));
										DEBUGMSG (1, (" channels: %u\n", m_StreamFormats[m_nStreamFormat].wfx.nChannels));
										DEBUGMSG (1, (" samplespersec: %u\n", m_StreamFormats[m_nStreamFormat].wfx.nSamplesPerSec));
										DEBUGMSG (1, (" bitspersample: %u\n", m_StreamFormats[m_nStreamFormat].wfx.wBitsPerSample));
										subListLength -= subChunkLength;
										m_CallbackTable.fseek (m_handle, position+subChunkLength, m_CallbackTable.context);
									}
									else
									{
										ASSERT (0);
										DEBUGMSG (1, ("ignoring unknown fcc type = %c%c%c%c\n",
											AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) >>  0, 
											AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) >>  8, 
											AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) >> 16, 
											AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[m_nStreamFormat], fccType) >> 24));
										n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
										subListLength -= subChunkLength;
										m_CallbackTable.fseek (m_handle, m_CallbackTable.ftell (m_handle, m_CallbackTable.context)+subChunkLength, m_CallbackTable.context);
									}
									m_nStreamFormat++;
									break;
#if 1
								case AVI_FOURCC ('i','n','d','x'):
									saveposition = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
									position = saveposition;

									n = m_CallbackTable.fread (m_handle, sizebuf, 4, m_CallbackTable.context);
									n = (long)sizebuf[0] +
										((long)sizebuf[1] << 8) +
										((long)sizebuf[2] << 16) +
										((long)sizebuf[3] << 24) + 4;
									saveposition += n;

									//12Byte를 더한 이유는 다음에 인덱스가 비디오 인지 오디오 인지 구분하기 위해서다.
									m_CallbackTable.fseek (m_handle, position + 12, m_CallbackTable.context);
									n = m_CallbackTable.fread (m_handle, sizebuf, 4, m_CallbackTable.context);
									
									//Video Index 정보인지 검사
									if(sizebuf[2] == 'd' && (sizebuf[3] == 'c' || sizebuf[3] == 'b')) 
									{
										//videoidxf = 1;
										position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
										m_CallbackTable.fseek (m_handle, position + 12, m_CallbackTable.context);
										//인덱스 파일 절대 위치를 가져온다.
										n = m_CallbackTable.fread (m_handle, sizebuf, 4, m_CallbackTable.context);
//										videoidxstart = (unsigned long)sizebuf[0] +
//											((unsigned long)sizebuf[1] << 8) +
//											((unsigned long)sizebuf[2] << 16) +
//											((unsigned long)sizebuf[3] << 24);
									}
									//Audio Index 정보인지 검사
									else if(sizebuf[2] == 'w' && (sizebuf[3] == 'c' || sizebuf[3] == 'b')) 
									{
										//audioidxf = 1;
										position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
										m_CallbackTable.fseek (m_handle, position + 12, m_CallbackTable.context);
										//인덱스 파일 절대 위치를 가져온다.
										n = m_CallbackTable.fread (m_handle, sizebuf, 4, m_CallbackTable.context);
//										audioidxstart = (unsigned long)sizebuf[0] +
//											((unsigned long)sizebuf[1] << 8) +
//											((unsigned long)sizebuf[2] << 16) +
//											((unsigned long)sizebuf[3] << 24);
									}
									
									m_CallbackTable.fseek (m_handle, saveposition, m_CallbackTable.context);
									
									//indxflag = 1;
									subListLength = 0; //subChunkLength;
									break;
								case AVI_FOURCC ('s','t','r','n'):
									n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
									subListLength -= 4;
									position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
									subListLength -= subChunkLength;
									m_CallbackTable.fseek (m_handle, position+subChunkLength, m_CallbackTable.context);
									break; 
#endif											
										
								default:
									DEBUGMSG (1, ("ignoring unknown list type = %c%c%c%c\n",
										name >>  0, name >>  8, name >> 16, name >> 24));
									n = m_CallbackTable.fread (m_handle, &subChunkLength, sizeof(subChunkLength), m_CallbackTable.context);
									ASSERT (n == sizeof(subChunkLength));
									subListLength -= 4;
									m_CallbackTable.fseek (m_handle, m_CallbackTable.ftell (m_handle, m_CallbackTable.context)+subChunkLength, m_CallbackTable.context);
									subListLength -= subChunkLength;
									break;
								}
								if (m_nStreamFormat >= AVI_MAX_STREAMS_SUPPORTED)
									subListLength = 0;
							}
							ASSERT ((RMint32)subListLength >= 0);
							ASSERT ((RMint32)subListLength <= 3);
							if (((RMint32)subListLength > 0) && ((RMint32)subListLength < 4))
							{
								RMuint32 dummy;
								m_CallbackTable.fread (m_handle, &dummy, subListLength, m_CallbackTable.context);
							}
						}
					}
				}
				break;
			case AVI_FOURCC ('m','o','v','i'):
				DEBUGMSG (1, ("movi chunk\n"));
				m_moviOffset = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
				break;
			default:
				DEBUGMSG (1, ("unknown list type = %c%c%c%c\n",
					name >>  0, name >>  8, name >> 16, name >> 24));
				break;
			}
			m_CallbackTable.fseek (m_handle, listOffset + chunk.Length, m_CallbackTable.context);
			rifflength -= chunk.Length;
			break;
		case AVI_FOURCC ('i','d','x','1'):
			DEBUGMSG (1, ("idx1 chunk\n"));
			n = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
			m_idx1Offset = n;
			m_idx1Length = chunk.Length;
			{
				// process idx1 chunk
				AVI_INDEXENTRY tmp[512];
				RMint32 i, j, x, n, nindexentries, indexoffset, tindexentries, div_tn;
				tindexentries = chunk.Length / sizeof (AVI_INDEXENTRY);
				nindexentries = tindexentries;
				DEBUGMSG (1, ("%d index entries found\n", nindexentries));
				RMuint32 videoFrameCount = 0;
				RMuint32 audioByteOrFrameCount[AVI_MAX_STREAMS_SUPPORTED];
				for (i=0; i<AVI_MAX_STREAMS_SUPPORTED; i++)
					audioByteOrFrameCount[i] = 0;
				m_nIndexHelper = 0;
				x = 1;
				indexoffset = 0;
				while (nindexentries)
				{
					n = AVI_MIN (512, nindexentries);
					m_CallbackTable.fread (m_handle, tmp, sizeof(AVI_INDEXENTRY)*n, m_CallbackTable.context);
					for (i=0; i<n; i++)
					{
						if (((tmp[i].ckid & 0xffff0000) == 0x62640000) ||
							((tmp[i].ckid & 0xffff0000) == 0x63640000))
						{
							if (videoFrameCount == 0)
							{
								DEBUGMSG (1, ("checking validity of chunk index ...\n"));
								// try to check if the index given is 
								// relative to the start of the file or
								// if it is relative to the start of the movi chunk
								// the spec. says that it should be relative to the
								// start of the movi chunk.
								RMuint32 saved_position, tmpckid;
								saved_position = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
								m_CallbackTable.fseek (m_handle, tmp[i].dwChunkOffset, m_CallbackTable.context);
								m_CallbackTable.fread (m_handle, &tmpckid, sizeof(tmpckid), m_CallbackTable.context);
								DEBUGMSG (1, ("tmpckid = %08lx\n", tmpckid));
								if (((tmpckid & 0xffff0000) == 0x62640000) ||
									((tmpckid & 0xffff0000) == 0x63640000))
								{
									if (m_CallbackTable.info (AVI_DEMUX_MSG_INDEX_CHECK, 0, m_CallbackTable.context))
									{
										DEBUGMSG (1, ("Assuming chunk indices are relative to the start of the file.\n"));
										m_IndexIsRelativeToStartOfFile = 1;
									}
								}
								m_CallbackTable.fseek (m_handle, saved_position, m_CallbackTable.context);
							}
							videoFrameCount++;
							if ((x == 1) && ((tmp[i].dwFlags & AVI_FLAGS_KEYFRAME) == 0));
							else
								x--;
						}
						else 
						if ((tmp[i].ckid & 0xffff0000) == 0x62770000)
						{
							RMuint32 idx = (((tmp[i].ckid & 0xff)-0x30) << 8) | (((tmp[i].ckid & 0xff00) >> 8)-0x30);
							if (idx < AVI_MAX_STREAMS_SUPPORTED)
							{
								if (m_vbrmp3[idx])
									audioByteOrFrameCount[idx]++;
								else
									audioByteOrFrameCount[idx] += tmp[i].dwChunkLength;
							}
						}
						if (x == 0)
						{
							m_IndexHelper[m_nIndexHelper].indexOffset = indexoffset;
							m_IndexHelper[m_nIndexHelper].chunkOffset = tmp[i].dwChunkOffset;
							ASSERT ((RMint32)(videoFrameCount - 1) >= 0);
							m_IndexHelper[m_nIndexHelper].videoFrameCount = videoFrameCount - 1;
							for (j=0; j<AVI_MAX_STREAMS_SUPPORTED; j++)
								m_IndexHelper[m_nIndexHelper].audioByteOrFrameCount[j] = audioByteOrFrameCount[j];
							x = ((m_avih.TotalFrames / AVI_MAX_INDEX_HELPER) + 1);
							m_nIndexHelper++;
						}
						indexoffset++;
					}
					nindexentries -= n; 
					DEBUGMSG (1, ("%d index entries left\n", nindexentries));
					ASSERT (m_CallbackTable.loading);
					if( (div_tn=tindexentries-nindexentries) > 0 )
						m_CallbackTable.loading ( div_tn * 100 / tindexentries, m_CallbackTable.context );
				}
			}
			m_CallbackTable.fseek (m_handle, n + chunk.Length, m_CallbackTable.context);
			rifflength -= chunk.Length;
			break;
		default:
			DEBUGMSG (1, ("dump chunk = %c%c%c%c\n", 
				chunk.Name >>  0,
				chunk.Name >>  8,
				chunk.Name >> 16,
				chunk.Name >> 24));
			n = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
			m_CallbackTable.fseek (m_handle, n + chunk.Length, m_CallbackTable.context);
			rifflength -= chunk.Length;
			break;
		}
		ASSERT (m_CallbackTable.loading);
	}
	ASSERT (m_moviOffset < m_sizeOfFile);
	if (m_moviOffset == 0)
	{
		m_CallbackTable.fclose (m_handle, m_CallbackTable.context);
		m_handle = 0;
		DEBUGMSG (1, ("no 'movi' chunk found\n"));
		return AVI_DEMUX_ERROR_NOT_AN_AVI_FILE;
	}
	m_CallbackTable.fseek (m_handle, m_moviOffset, m_CallbackTable.context);
	m_demuxstate = AVI_DEMUX_STATE_CHUNK_NAME_BYTE0;
	m_savedbuf = 0;
	m_savedbuflen = 0;
	m_savedp = 0;
	return AVI_DEMUX_ERROR_NO_ERROR;
}
#endif

RMuint32 AVIDemux::CheckChunkId ()
{
	if ((m_chunkid[2] == 'd') && (m_chunkid[3] == 'c'))
	{
		if ((m_chunkid[0] >= '0' && m_chunkid[0] <= '9') &&
			(m_chunkid[1] >= '0' && m_chunkid[1] <= '9'))
			return 0;	
	}
	if ((m_chunkid[2] == 'd') && (m_chunkid[3] == 'b'))
	{
		if ((m_chunkid[0] >= '0' && m_chunkid[0] <= '9') &&
			(m_chunkid[1] >= '0' && m_chunkid[1] <= '9'))
			return 0;	
	}
	if ((m_chunkid[2] == 'w') && (m_chunkid[3] == 'b'))
	{
		if ((m_chunkid[0] >= '0' && m_chunkid[0] <= '9') &&
			(m_chunkid[1] >= '0' && m_chunkid[1] <= '9'))
			return 0;	
	}	
	// Divx Networks DRM info chunk
	if ((m_chunkid[2] == 'd') && (m_chunkid[3] == 'd'))
	{
		if ((m_chunkid[0] >= '0' && m_chunkid[0] <= '9') &&
			(m_chunkid[1] >= '0' && m_chunkid[1] <= '9'))
			return 0;	
	}	
	// XXX I have never seen a 'JUNK' chunk
	if ((m_chunkid[0] == 'J') && (m_chunkid[1] == 'U') &&
		(m_chunkid[2] == 'N') && (m_chunkid[3] == 'K'))
		return 0;
	// XXX ix## chunks are defined for open avi and are present
	// in the movi chunk
	if ((m_chunkid[0] == 'i') && (m_chunkid[1] == 'x'))
	{
		if ((m_chunkid[2] >= '0' && m_chunkid[2] <= '9') &&
			(m_chunkid[3] >= '0' && m_chunkid[3] <= '9'))
			return 0;
	}
	// XXX idx1 chunks are sometimes at the end of the file
	if ((m_chunkid[0] == 'i') && (m_chunkid[1] == 'd') &&
		(m_chunkid[2] == 'x') && (m_chunkid[3] == '1'))
			return 0;		
	// XXX we do not support 'rec' chunks 
	return 1;
}

AVI_DEMUX_ERROR AVIDemux::Schedule ()
{
	ASSERT (m_moviOffset);
	RMuint8 *p, *buf;
	RMuint32 buflen, n;
	DEBUGMSG (0, ("AVIDemux::Schedule\n"));

	if (m_KeyFramesOnly)
	{
		return ScheduleKeyFramesOnly ();
	}

	if (m_demuxstate == AVI_DEMUX_STATE_RESTORE_STATE)
	{
		ASSERT (m_savedbuf);
		ASSERT (m_savedbuflen);
		ASSERT (m_savedp);
		p = m_savedp;
		buf = m_savedbuf;
		buflen = n = m_savedbuflen;
		m_demuxstate = m_savedstate;
	}
	else
	{
		if (m_CallbackTable.getbuffer (&p, &buflen, m_CallbackTable.context))
			{
			return AVI_DEMUX_ERROR_NO_BUFFER_AVAILABLE;
			}
		n = m_CallbackTable.fread (m_handle, p, buflen, m_CallbackTable.context);
		if (n == 0)
		{
			return AVI_DEMUX_ERROR_FILE_DONE;
		}
		buflen = n; 
		ASSERT (buflen < 0x80000000);
		ASSERT (m_demuxstate <= AVI_DEMUX_STATE_WRITE_CHUNK);
		ASSERT (m_CallbackTable.addref);
		buf = p;
		m_CallbackTable.addref (buf, m_CallbackTable.context);
	}
	
	while ((RMint32)buflen > 0)
	{
		switch (m_demuxstate)
		{
		case AVI_DEMUX_STATE_CHUNK_NAME_BYTE0:	// chunk id byte 0
			m_chunkid[0] = *p++;
			if (m_chunkid[0] != 0)
				m_demuxstate++;
			else
				m_demuxstate = AVI_DEMUX_STATE_CHUNK_NAME_BYTE0;
			buflen--;
			break;
		case AVI_DEMUX_STATE_CHUNK_NAME_BYTE1:	// chunk id byte 1
			m_chunkid[1] = *p++;
			m_demuxstate++;
			buflen--;
			break;
		case AVI_DEMUX_STATE_CHUNK_NAME_BYTE2:	// chunk id byte 2
			m_chunkid[2] = *p++;
			m_demuxstate++;
			buflen--;
			break;
		case AVI_DEMUX_STATE_CHUNK_NAME_BYTE3: // chunk id byte 3
			m_chunkid[3] = *p++;
			m_demuxstate++;
			buflen--;
			// we should have the complete chunkid right now
			// try to verify that the chunk id is correct
			if (CheckChunkId ())
			{
				// something is wrong
				// go back to state AVI_DEMUX_STATE_CHUNK_NAME_BYTE3			       
				m_chunkid[0] = m_chunkid[1];
				m_chunkid[1] = m_chunkid[2];
				m_chunkid[2] = m_chunkid[3];
				m_demuxstate = AVI_DEMUX_STATE_CHUNK_NAME_BYTE3;
			}
			else if ((m_chunkid[2] == 'd') && ((m_chunkid[3] == 'c') || (m_chunkid[3] == 'b')))
			{
				m_currentFramePosition++;
			}
			break;
		case AVI_DEMUX_STATE_CHUNK_LENGTH_BYTE0:	// chunk length byte 0
			m_chunklength = (RMuint32)*p++;
			m_demuxstate++;
			buflen--;
			break;
		case AVI_DEMUX_STATE_CHUNK_LENGTH_BYTE1:	// chunk length byte 1
			m_chunklength |= (RMuint32)*p++ << 8;
			m_demuxstate++;
			buflen--;
			break;
		case AVI_DEMUX_STATE_CHUNK_LENGTH_BYTE2:	// chunk length byte 2
			m_chunklength |= (RMuint32)*p++ << 16;
			m_demuxstate++;
			buflen--;
			break;
		case AVI_DEMUX_STATE_CHUNK_LENGTH_BYTE3:	// chunk length byte 3
			m_chunklength |= (RMuint32)*p++ << 24;
			m_bytecounter = 0;
			m_demuxstate++;
			buflen--;
			m_chunkstart = AVI_FLAG_CHUNK_START;
			// cheesy error detection
			if (m_chunklength >= m_sizeOfFile) 
			{
				m_demuxstate = AVI_DEMUX_STATE_CHUNK_NAME_BYTE0;
			}
			if (m_chunklength == 0 || m_chunklength >= m_sizeOfFile)
			{
				m_CallbackTable.putChunk (m_chunkid, 0, 0, AVI_FLAG_CHUNK_END | m_chunkstart, m_CallbackTable.context);
				m_chunkstart = 0;
			}
			break;
		case AVI_DEMUX_STATE_WRITE_CHUNK: // send data
			if (m_bytecounter == m_chunklength)
			{		
				m_savedbuf = buf;
				m_savedbuflen = buflen;				
				m_savedp = p;
				m_savedstate = AVI_DEMUX_STATE_CHUNK_NAME_BYTE0;
				m_demuxstate = AVI_DEMUX_STATE_RESTORE_STATE;
				return AVI_DEMUX_ERROR_NO_ERROR;
			}
			if ((m_chunklength - m_bytecounter) < buflen)
			{
				RMuint32 chunklength = m_chunklength - m_bytecounter;
				ASSERT ((RMint32)chunklength >= 0);
				if (m_CallbackTable.putChunk (m_chunkid, p, chunklength, AVI_FLAG_CHUNK_END | m_chunkstart, m_CallbackTable.context) == 0)
				{
					m_chunkstart = 0;
					buflen -= chunklength;
					p += chunklength;
					m_bytecounter = m_chunklength;
				}
				else
				{
					// not tested
					ASSERT (0);
					m_savedbuf = buf;
					m_savedbuflen = buflen;				
					m_savedp = p;
					m_savedstate = AVI_DEMUX_STATE_WRITE_CHUNK;
					m_demuxstate = AVI_DEMUX_STATE_RESTORE_STATE;
					return AVI_DEMUX_ERROR_NO_ERROR;
				}
			}
			else
			{
				if (m_CallbackTable.putChunk (m_chunkid, p, buflen, m_chunkstart, m_CallbackTable.context) == 0)
				{
					m_chunkstart = 0;
					m_bytecounter += buflen;
					p += buflen;
					buflen = 0;
				}
				else
				{
					// not tested
					ASSERT (0);
					m_savedbuf = buf;
					m_savedbuflen = buflen;				
					m_savedp = p;
					m_savedstate = AVI_DEMUX_STATE_WRITE_CHUNK;
					m_demuxstate = AVI_DEMUX_STATE_RESTORE_STATE;
					return AVI_DEMUX_ERROR_NO_ERROR;
				}
			}
			break;
		}				
	}
	ASSERT (m_CallbackTable.release);
	m_CallbackTable.release (buf, m_CallbackTable.context);
	return AVI_DEMUX_ERROR_NO_ERROR;
}

#define AVI_DEMUX_KEYFRAMEONLY_STATE_NEXT_KEYFRAME	0
#define AVI_DEMUX_KEYFRAMEONLY_STATE_PREV_KEYFRAME	1
#define AVI_DEMUX_KEYFRAMEONLY_STATE_SEND_KEYFRAME	2
#define AVI_DEMUX_KEYFRAMEONLY_STATE_DELAY			3

#include <stdio.h>

AVI_DEMUX_ERROR	AVIDemux::ScheduleKeyFramesOnly ()
{
	ASSERT (m_moviOffset);
	RMuint8 *p;
	RMuint32 buflen, n;
	RMint32 i;
	RMuint32 err;

	switch (m_KeyFrameOnlyState)
	{
	case AVI_DEMUX_KEYFRAMEONLY_STATE_NEXT_KEYFRAME:
		// get the next key frame
		for (i=m_CurrentIndex; i<m_IndexCacheValid; i++)
		{
			if (((m_IndexCache[i].ckid & 0xffff0000) == 0x62640000) ||
				((m_IndexCache[i].ckid & 0xffff0000) == 0x63640000))
			{
				m_currentFramePosition++;
				if (m_IndexCache[i].dwFlags & AVI_FLAGS_KEYFRAME)
				{
					m_CurrentIndex = i + 1;
					if (m_IndexIsRelativeToStartOfFile)
						m_KeyFrameOffset = m_IndexCache[i].dwChunkOffset + 4;
					else
						m_KeyFrameOffset = m_moviOffset + m_IndexCache[i].dwChunkOffset + 4;
					m_KeyFrameLength = m_IndexCache[i].dwChunkLength;
					ASSERT ((RMint32)m_KeyFrameLength > 0);
					m_KeyFrameOnlyState = AVI_DEMUX_KEYFRAMEONLY_STATE_DELAY;
					m_chunkid[0] = '0';
					m_chunkid[1] = '0';
					m_chunkid[2] = 'd';
					m_chunkid[3] = 'c';
					m_KeyFrameFlags = AVI_FLAG_CHUNK_START;
					m_CallbackTable.info (AVI_DEMUX_KEYFRAME_POSITION, (void *)(m_currentFramePosition - 1), m_CallbackTable.context);
					m_CallbackTable.fseek (m_handle, m_KeyFrameOffset, m_CallbackTable.context);
					break;
				}
			}
		}
		if (i == m_IndexCacheValid)
		{
			// reload index and try again
			m_IndexCacheOffset += (m_IndexCacheValid * sizeof (AVI_INDEXENTRY));
			m_CallbackTable.fseek (m_handle, m_IndexCacheOffset, m_CallbackTable.context);
			n = m_CallbackTable.fread (m_handle, &m_IndexCache, sizeof (AVI_INDEXENTRY) * AVI_INDEX_CACHE_SIZE, m_CallbackTable.context);
			m_IndexCacheValid = n / sizeof (AVI_INDEXENTRY);
			m_CurrentIndex = 0;
			if ((m_IndexCacheValid == 0) || (m_IndexCacheOffset > (m_idx1Offset + m_idx1Length)))
				return AVI_DEMUX_ERROR_FILE_DONE;
		}
		break;
	case AVI_DEMUX_KEYFRAMEONLY_STATE_PREV_KEYFRAME:
		// get the previous key frame
		for (i=m_CurrentIndex; i>=0; i--)
		{
			if (((m_IndexCache[i].ckid & 0xffff0000) == 0x62640000) ||
				((m_IndexCache[i].ckid & 0xffff0000) == 0x63640000))
			{
				m_currentFramePosition--;
				if (m_IndexCache[i].dwFlags & AVI_FLAGS_KEYFRAME)
				{
					m_CurrentIndex = i - 1;
					if (m_IndexIsRelativeToStartOfFile)
						m_KeyFrameOffset = m_IndexCache[i].dwChunkOffset + 4;
					else
						m_KeyFrameOffset = m_moviOffset + m_IndexCache[i].dwChunkOffset + 4;
					m_KeyFrameLength = m_IndexCache[i].dwChunkLength;
					ASSERT ((RMint32)m_KeyFrameLength > 0);
					m_KeyFrameOnlyState = AVI_DEMUX_KEYFRAMEONLY_STATE_DELAY;
					m_chunkid[0] = '0';
					m_chunkid[1] = '0';
					m_chunkid[2] = 'd';
					m_chunkid[3] = 'c';
					m_KeyFrameFlags = AVI_FLAG_CHUNK_START;
					m_CallbackTable.info (AVI_DEMUX_KEYFRAME_POSITION, (void *)(m_currentFramePosition + 1), m_CallbackTable.context);
					m_CallbackTable.fseek (m_handle, m_KeyFrameOffset, m_CallbackTable.context);
					break;
				}
			}
		}
		if (i < 0)
		{
			// reload index cache and try again
			if (m_IndexCacheOffset == m_idx1Offset)
				return AVI_DEMUX_ERROR_FILE_DONE;
			m_IndexCacheOffset -= (sizeof (AVI_INDEXENTRY) * AVI_INDEX_CACHE_SIZE);
			if (m_IndexCacheOffset < m_idx1Offset)
			{
				m_CurrentIndex = AVI_INDEX_CACHE_SIZE - ((m_idx1Offset - m_IndexCacheOffset) / sizeof (AVI_INDEXENTRY)) - 1;
				ASSERT ((RMint32)m_CurrentIndex > 0);
				m_IndexCacheOffset = m_idx1Offset;
				ASSERT ((RMint32)m_IndexCacheOffset > 0);
			}
			else
			{
				ASSERT ((RMint32)m_IndexCacheOffset > 0);
				m_CurrentIndex = AVI_INDEX_CACHE_SIZE - 1;
				ASSERT ((RMint32)m_CurrentIndex > 0);
			}
			m_CallbackTable.fseek (m_handle, m_IndexCacheOffset, m_CallbackTable.context);
			n = m_CallbackTable.fread (m_handle, &m_IndexCache, sizeof (AVI_INDEXENTRY) * AVI_INDEX_CACHE_SIZE, m_CallbackTable.context);
			m_IndexCacheValid = n / sizeof (AVI_INDEXENTRY);
		}
		break;
	case AVI_DEMUX_KEYFRAMEONLY_STATE_SEND_KEYFRAME:
		if (m_CallbackTable.getbuffer (&p, &buflen, m_CallbackTable.context))
			return AVI_DEMUX_ERROR_NO_BUFFER_AVAILABLE;
		n = AVI_MIN (buflen, m_KeyFrameLength); 
		n = m_CallbackTable.fread (m_handle, p, n, m_CallbackTable.context);
		m_KeyFrameLength -= n;
		if (m_KeyFrameLength == 0)
		{
			DEBUGMSG (1, ("AVI_DEMUX_KEYFRAMEONLY_STATE_SEND_KEYFRAME: %d\n", (RMint32)m_currentFramePosition));
			m_KeyFrameFlags |= AVI_FLAG_CHUNK_END;
			if (m_KeyFramesOnly > 0)
				m_KeyFrameOnlyState = AVI_DEMUX_KEYFRAMEONLY_STATE_NEXT_KEYFRAME;
			else
				m_KeyFrameOnlyState = AVI_DEMUX_KEYFRAMEONLY_STATE_PREV_KEYFRAME;
			ASSERT (m_CallbackTable.gettimems);
			m_KeyFrameDelay_t0 = m_CallbackTable.gettimems (m_CallbackTable.context);
		}		
		err = m_CallbackTable.putChunk (m_chunkid, p, n, m_KeyFrameFlags, m_CallbackTable.context);
		// not allowed to pend only key frames
		ASSERT (err == 0);
		m_KeyFrameFlags = 0;
		break;
	case AVI_DEMUX_KEYFRAMEONLY_STATE_DELAY:		
		i = m_CallbackTable.gettimems (m_CallbackTable.context);
		if ((RMint32)(i - m_KeyFrameDelay_t0) > m_KeyFrameDelayInMS)
		{
			m_KeyFrameOnlyState = AVI_DEMUX_KEYFRAMEONLY_STATE_SEND_KEYFRAME;
		}
		break;
	}
	return AVI_DEMUX_ERROR_NO_ERROR;
}

AVI_DEMUX_ERROR AVIDemux::Seek (RMuint32 seconds, RMuint32 audiostreamno, RMuint32 *pvideoposition, RMuint32 *paudioposition, RMuint32 *paudiobyte)
{
	RMuint32 i;
	RMuint32 s0, s1, Rate=24000, Scale=1000;

	if (m_idx1Offset == 0)
	{
		ASSERT (0);
		return AVI_DEMUX_ERROR_INVALID_PARAMETER;
	}

	ASSERT (audiostreamno <	AVI_MAX_STREAMS_SUPPORTED);

	if (audiostreamno >= AVI_MAX_STREAMS_SUPPORTED)
		return AVI_DEMUX_ERROR_INVALID_PARAMETER;

	DEBUGMSG (1, ("AVI Seek (%d)\n", (RMint32)seconds));
	for (i=0; i<AVI_MAX_STREAMS_SUPPORTED; i++)
	{		
		if (AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[i], fccType) == AVI_FOURCC ('v','i','d','s'))
		{
		    #if 0
			if( AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[i], Rate) < 64000 &&
				AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[i], Rate) > 1000 )
				{
				Rate = AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[i], Rate);
				Scale = AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[i], Scale);
				}
			else
				{
				Rate = SuggestedRate;
				Scale = SuggestedScale;
				}
			#endif
			Rate = AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[i], Rate);
			Scale = AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[i], Scale);
		
			DEBUGMSG (1, (" Rate: %lu\n", AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[i], Rate)));
			DEBUGMSG (1, (" Scale: %lu\n", AVI_PACKED_STRUCTURE_ACCESS (m_StreamHeaders[i], Scale)));
			break;
		}
	}
	for (i=0; i<m_nIndexHelper-1; i++)
	{
        #if 0
		if( Rate > 64000 || Rate < 1000 )
			{
			Rate = SuggestedRate;
			Scale = SuggestedScale;
			}
		#endif

		s0 = m_IndexHelper[i].videoFrameCount * Scale / Rate;
		s1 = m_IndexHelper[i+1].videoFrameCount * Scale / Rate;
		if ((seconds >= s0) && (seconds <s1))
		{			
			DEBUGMSG (1, (" videoFrameCount: %lu\n", m_IndexHelper[i].videoFrameCount));
			DEBUGMSG (1, (" audioByteOrFrameCount: %lu\n", m_IndexHelper[i].audioByteOrFrameCount[audiostreamno]));
			DEBUGMSG (1, (" vbrmp3: %lu\n", m_vbrmp3[audiostreamno]));
			RMuint32 offset;
			if (m_IndexIsRelativeToStartOfFile)
				offset = m_IndexHelper[i].chunkOffset;
			else
				offset = m_moviOffset + m_IndexHelper[i].chunkOffset - 4;
			DEBUGMSG (1, (" seek to fileposition %lu.\n", offset));
			m_CallbackTable.fseek (m_handle, offset, m_CallbackTable.context);
			m_demuxstate = AVI_DEMUX_STATE_CHUNK_NAME_BYTE0;
			*pvideoposition = m_IndexHelper[i].videoFrameCount;
			*paudioposition = m_IndexHelper[i].audioByteOrFrameCount[audiostreamno];
			if (m_vbrmp3[audiostreamno])
				*paudiobyte = 0;
			else
				*paudiobyte = 1;
			m_currentFramePosition = m_IndexHelper[i].videoFrameCount;
			DEBUGMSG (1, (" seek ok.\n"));
			return AVI_DEMUX_ERROR_NO_ERROR;
		}
	}
	DEBUGMSG (1, (" seek failed.\n"));
	return AVI_DEMUX_ERROR_INVALID_PARAMETER;
}

AVI_DEMUX_ERROR AVIDemux::KeyFramesOnly (RMint32 direction)
{
	RMuint32 i, n;
	RMuint32 p0, p1;
	DEBUGMSG (1, ("AVIDemux::KeyFramesOnly (%d)\n", direction));

	if (m_idx1Offset == 0)
	{
		ASSERT (0);
		return AVI_DEMUX_ERROR_INVALID_PARAMETER;
	}

	if (direction == 0)
	{
		m_KeyFramesOnly = 0;
		// continue normal demux from the current position
		m_CallbackTable.fseek (m_handle, m_KeyFrameOffset - 4, m_CallbackTable.context);
		return AVI_DEMUX_ERROR_NO_ERROR;
	}
	m_KeyFrameDelay_t0 = m_CallbackTable.gettimems (m_CallbackTable.context);
	RMuint32 currentpos = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);
	m_KeyFrameOffset = currentpos + 4;
	DEBUGMSG (1, ("m_KeyFrameOffset = %d (%d + 4)\n", 
		(RMint32)m_KeyFrameOffset, (RMint32)currentpos));
	DEBUGMSG (1, ("m_nIndexHelper = %d\n", (RMint32)m_nIndexHelper));
	DEBUGMSG (1, ("m_currentFramePosition = %d\n", (RMint32)m_currentFramePosition));
	// find the nearest key frame
	for (i=0; i<m_nIndexHelper-1; i++)
	{
		if (m_IndexHelper[i].videoFrameCount)
			p0 = m_IndexHelper[i].videoFrameCount - 1;
		else
			p0 = 0;
		if (m_IndexHelper[i+1].videoFrameCount)
			p1 = m_IndexHelper[i+1].videoFrameCount - 1;
		else
			p1 = 0;
		DEBUGMSG (1, ("p0 = %d, p1 = %d\n", (RMint32)p0, (RMint32)p1));
		if ((m_currentFramePosition >= p0) && (m_currentFramePosition < p1))
		{
			// initialize the index cache
			m_KeyFramesOnly = direction;
			if (direction > 0)
			{
				m_IndexCacheOffset = m_idx1Offset + (m_IndexHelper[i].indexOffset * sizeof (AVI_INDEXENTRY));
				m_CallbackTable.fseek (m_handle, m_IndexCacheOffset, m_CallbackTable.context);
				n = m_CallbackTable.fread (m_handle, &m_IndexCache, sizeof (AVI_INDEXENTRY) * AVI_INDEX_CACHE_SIZE, m_CallbackTable.context);
				m_IndexCacheValid = n / sizeof (AVI_INDEXENTRY);
				m_KeyFrameOnlyState = AVI_DEMUX_KEYFRAMEONLY_STATE_NEXT_KEYFRAME;
				m_CurrentIndex = 0;
			}
			else
			{
				if (m_IndexHelper[i].indexOffset < AVI_INDEX_CACHE_SIZE)
				{
					m_IndexCacheOffset = m_idx1Offset;
					ASSERT ((RMint32)m_IndexCacheOffset >= 0);
					m_CurrentIndex = m_IndexHelper[i].indexOffset;
					ASSERT ((RMint32)m_CurrentIndex >= 0);
				}
				else
				{
					m_IndexCacheOffset = m_idx1Offset + (m_IndexHelper[i].indexOffset - AVI_INDEX_CACHE_SIZE) * sizeof (AVI_INDEXENTRY);
					ASSERT ((RMint32)m_IndexCacheOffset >= 0);
					m_CurrentIndex = AVI_INDEX_CACHE_SIZE - 1;
					ASSERT ((RMint32)m_CurrentIndex >= 0);
				}
				m_CallbackTable.fseek (m_handle, m_IndexCacheOffset, m_CallbackTable.context);
				n = m_CallbackTable.fread (m_handle, &m_IndexCache, sizeof (AVI_INDEXENTRY) * AVI_INDEX_CACHE_SIZE, m_CallbackTable.context);
				m_IndexCacheValid = n / sizeof (AVI_INDEXENTRY);
				m_KeyFrameOnlyState = AVI_DEMUX_KEYFRAMEONLY_STATE_PREV_KEYFRAME;
			}			
			m_CallbackTable.fseek (m_handle, currentpos, m_CallbackTable.context);
			return AVI_DEMUX_ERROR_NO_ERROR;
		}
	}
	m_CallbackTable.fseek (m_handle, currentpos, m_CallbackTable.context);
	return AVI_DEMUX_ERROR_NO_KEYFRAME;
}

AVI_DEMUX_ERROR AVIDemux::GetFrame (RMuint32 frameno, RMuint8 *buffer, RMuint32 *length)
{
	if (frameno)
	{
		ASSERT (0);
		return AVI_DEMUX_ERROR_NOT_IMPLEMENTED;
	}
	if (m_idx1Offset == 0)
	{
		ASSERT (0);
		return AVI_DEMUX_ERROR_INVALID_PARAMETER;
	}
	RMuint32 currentpos = m_CallbackTable.ftell (m_handle, m_CallbackTable.context);

	AVI_INDEXENTRY idx;
	m_CallbackTable.fseek (m_handle, m_idx1Offset, m_CallbackTable.context);
	while (1)
	{
		if (m_CallbackTable.fread (m_handle, &idx, sizeof(idx), m_CallbackTable.context) != sizeof(idx))
		{
			ASSERT (0);
			m_CallbackTable.fseek (m_handle, currentpos, m_CallbackTable.context);
			return AVI_DEMUX_ERROR_INVALID_PARAMETER;
		}
		if (((idx.ckid & 0xffff0000) == 0x62640000) ||
			((idx.ckid & 0xffff0000) == 0x63640000))
		{
			ASSERT (idx.dwFlags & AVI_FLAGS_KEYFRAME);
			if (m_IndexIsRelativeToStartOfFile)
				m_CallbackTable.fseek (m_handle, idx.dwChunkOffset, m_CallbackTable.context);
			else
			{
				ASSERT ((RMint32)(m_moviOffset + idx.dwChunkOffset - 4) > 0);
				m_CallbackTable.fseek (m_handle, m_moviOffset + idx.dwChunkOffset - 4, m_CallbackTable.context);
			}
			*length = AVI_MIN (*length, idx.dwChunkLength);
			*length = m_CallbackTable.fread (m_handle, buffer, *length, m_CallbackTable.context);
			break;
		}
	}
	m_CallbackTable.fseek (m_handle, currentpos, m_CallbackTable.context);
	return AVI_DEMUX_ERROR_NO_ERROR;
}

AVI_DEMUX_ERROR AVIDemux::SetKeyFrameDelay (RMuint32 delay_ms)
{
	m_KeyFrameDelayInMS = delay_ms;
	return AVI_DEMUX_ERROR_NO_ERROR;
}

AVI_DEMUX_ERROR AVIDemux::IsStreamNumberVBR (RMuint32 audiostreamno, RMuint32 *is_vbr)
{
	*is_vbr = m_vbrmp3[audiostreamno];
	return AVI_DEMUX_ERROR_NO_ERROR;
}

int AVIDemux::GetVideoCompression( void ) const
{
    return m_StreamFormats[0].bmi.Compression;
}



