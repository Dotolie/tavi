//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : MPEG1, MPEG2 Demux Code
// 
// $Source: /home/cvsroot/TAVI020/tavi/mpgparse/mpgparse.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/04/05 02:11:40 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: mpgparse.cpp,v $
// Revision 1.2  2006/04/05 02:11:40  zstein
// - added ReInitCallbackTable(...).
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.14  2005/12/22 05:22:09  etnlwind
// Update for New Version
//
// Revision 1.13  2005/11/11 04:26:05  etnlwind
// Update for VideoPlayer Screen Clean Method
//
// Revision 1.12  2005/10/13 01:25:32  etnlwind
// Update for Header Info
//
//*****************************************************************************

#include "mpgparse.h"
#include <tavi_structures.h>
#include <memory.h>
#include <stdio.h>

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

extern TAVI_TIME totaltime;
extern VIDEOFILE_PLAYINFO gVideofilePlayinfo;
extern RMuint32 BytesPerSecond;

#define MPG_MIN(X,Y) (((X)<=(Y))?(X):(Y))	// to find the min between two number.

static void MPEG1DemuxCallback (RMuint8 *pData, RMuint32 Length, RMuint8 PacketId, 
	RMint64 Scr, RMint64 Pts, RMint64 Dts, RMuint8 Flags, RMint64 offset, void *Context)
{
	MPEGDemux *pDemux = (MPEGDemux *)Context;
	ASSERT (pDemux);
	pDemux->MPEG1DemuxCallback (pData, Length, PacketId, Scr, Pts, Dts, Flags, offset);
}

static void PESDemuxCallback (RMuint8 *pData, RMuint32 Length, RMuint8 StreamId, RMuint8 SubStreamId, 
	RMint64 Scr, RMint64 Pts, RMint64 Dts, RMuint8 Flags, RMint64 offset, void *Context)
{
	MPEGDemux *pDemux = (MPEGDemux *)Context;
	ASSERT (pDemux);
	pDemux->PESDemuxCallback (pData, Length, StreamId, SubStreamId, Scr, Pts, Dts, Flags, offset);
}

static void AC3DTSInfoCallback (RMuint8 numberOfFrameHeaders, 
	RMuint16 firstAccessUnitPointer, void *context)
{
	MPEGDemux *pDemux = (MPEGDemux *)context;
	ASSERT (pDemux);
	pDemux->AC3DTSInfoCallback (numberOfFrameHeaders, firstAccessUnitPointer);
}

static void LPCMInfoCallback (RMuint8 numberOfFrameHeaders,
	RMuint16 firstAccessUnitPointer, RMuint32 frequency, 
	RMuint8 numberOfChannels, RMuint8 quantizationWordLength, 
	void *context)
{
	MPEGDemux *pDemux = (MPEGDemux *)context;
	ASSERT (pDemux);
	pDemux->LPCMInfoCallback (numberOfFrameHeaders, firstAccessUnitPointer,
		frequency, numberOfChannels, quantizationWordLength);
}

MPEGDemux::MPEGDemux ()
{
	m_file = 0;
}

MPEGDemux::~MPEGDemux ()
{
	if (m_file)
		m_CallbackTable.fclose (m_file, m_CallbackTable.context);
}

// call this after creation
MPG_DEMUX_ERROR MPEGDemux::Init ()
{
	// initialize internal mpeg demux callbacks
	// initialize any internal variables
	m_MPEG1Demux.m_Callback = ::MPEG1DemuxCallback;
	m_MPEG1Demux.m_Context = this;
	m_ProgramDemux.m_Callback = ::PESDemuxCallback;
	m_ProgramDemux.m_ac3dtsCallback = ::AC3DTSInfoCallback;
	m_ProgramDemux.m_lpcmCallback = ::LPCMInfoCallback;
	m_ProgramDemux.m_Context = this;
	m_ProgramDemux.m_IsDVDStream = 0;
	m_MPEG1Demux.Reset ();
	m_MPEG1Demux.ResetState ();
	m_ProgramDemux.Reset ();
	m_ProgramDemux.ResetState ();
	m_DemuxType = 0;
	if (m_file)
		m_CallbackTable.fclose (m_file, m_CallbackTable.context);
	m_file = 0;
	m_CurrentPosition = 0;
	m_IFrameState = 0;
	m_BytesToSkip = 0;
	m_IFrameSubState = 0;
	m_StartPosition = 0;
	m_ReverseErrorLoop = 0;
	return MPG_DEMUX_ERROR_NOT_IMPLEMENTED;
}

// call this to initalize the required callbacks
MPG_DEMUX_ERROR MPEGDemux::InitCallbackTable (MPG_CALLBACK_TABLE *pCallbackTable)
{
	memcpy (&m_CallbackTable, pCallbackTable, sizeof (MPG_CALLBACK_TABLE));
	return MPG_DEMUX_ERROR_NO_ERROR;
}

MPG_DEMUX_ERROR MPEGDemux::ReInitCallbackTable( MPG_CALLBACK_TABLE* pCallbackTable )
{
    int pos;

    printf( "ReInitCallbackTable\n" );
    if( m_file && m_CallbackTable.ftell ) {
        pos = m_CallbackTable.ftell( m_file, NULL );
    }
    InitCallbackTable( pCallbackTable );
    if( m_file && m_CallbackTable.fseek ) {
        m_CallbackTable.fseek( m_file, (RMuint32)pos, SEEK_SET, NULL );
    }

    return MPG_DEMUX_ERROR_NO_ERROR;
}


// call this to identify the file type
//MPG_DEMUX_ERROR MPEGDemux::DecodeFile (RMint8 *filename)
MPG_DEMUX_ERROR MPEGDemux::DecodeFile (RMuint16*filename)
{
	DEBUGMSG(1,("\n[!] MPEGDemux::DecodeFile()\n"));

	mpg_info( filename );

	// try to detect if the file is a valid mpeg file
	if (IsTransportStream (filename))
	{
		// no support for transport streams
		DEBUGMSG (1, ("error: no support for transport streams\n"));
		return MPG_DEMUX_ERROR_NOT_AN_MPEG_FILE;
	}
	if (IsDVDStream (filename))
	{
		DEBUGMSG (1, ("DVD stream\n"));
		m_DemuxType = DVD_DEMUX;
		m_ProgramDemux.m_IsDVDStream = 1;
	}
	else if (IsProgramStream (filename))
	{
		DEBUGMSG (1, ("MPEG 2 program stream\n"));
		m_DemuxType = PROGRAM_DEMUX;
	}
	else if (IsMPEG1SystemStream (filename))
	{
//		gVideofilePlayinfo.OptimizedWidth = 352;
//		gVideofilePlayinfo.OptimizedHeight = 240;

		DEBUGMSG (1, ("MPEG 1 system stream\n"));
		m_DemuxType = SYSTEM_DEMUX;
		if (IsVCDStream (filename))
		{
			DEBUGMSG (1, ("DAT file\n"));
			m_DemuxType = DAT_DEMUX;
		}
	}
	else if (IsMPEG124ElementaryStream (filename, &m_mpeg12))
	{
		m_DemuxType = NO_DEMUX; 
	}
	else
	{
		DEBUGMSG (1, ("error: MPG_DEMUX_ERROR_NOT_AN_MPEG_FILE\n"));
		return MPG_DEMUX_ERROR_NOT_AN_MPEG_FILE;
	}
	ASSERT (m_file == 0);
	m_file = m_CallbackTable.fopen (filename, m_CallbackTable.context);


/*
				gVideofilePlayinfo.OriginalWidth	= m_avih.Width;
				gVideofilePlayinfo.OriginalHeight	= m_avih.Height;
				totaltime.SecOnly 	= ((U64) m_avih.TotalFrames * (U64)m_avih.MicroSecPerFrame) /1000000;
				totaltime.hours 		= (int) (totaltime.SecOnly / 3600);
				totaltime.minutes 	= (int) ((totaltime.SecOnly - totaltime.hours * 3600) / 60);
				totaltime.seconds 	= (int) (totaltime.SecOnly - totaltime.hours*3600 - totaltime.minutes*60);
*/	
	
	ASSERT (m_file);

	return MPG_DEMUX_ERROR_NO_ERROR;
}




RMuint32 MPEGDemux::GetDemuxType (void)
{
	return m_DemuxType;
}

RMuint32 MPEGDemux::AssumeDVDPacketStructure (RMint32 followdvd)
{
	if (m_DemuxType == PROGRAM_DEMUX)
	{
		m_DemuxType = DVD_DEMUX;
		m_ProgramDemux.m_IsDVDStream = 1;
	}
	else if (m_DemuxType == DVD_DEMUX)
	{
		m_DemuxType = PROGRAM_DEMUX;
		m_ProgramDemux.m_IsDVDStream = 0;
	}
	return 0;
}

// call this repeatly to do work
MPG_DEMUX_ERROR MPEGDemux::Schedule ()
{
	RMuint8 *p, *ptemp;
	RMint32 l, temp, readlength;
	// get a buffer an demux it
	if (m_CallbackTable.getMPG (&p, (RMuint32 *)&l, m_CallbackTable.context) == MPG_DEMUX_ERROR_NO_ERROR)
	{
		ASSERT (m_file);
		if (m_DemuxType == DAT_DEMUX)
			l -= (l % 2352);
		ASSERT (l);
		
		l = m_CallbackTable.fread (m_file, p, l, m_CallbackTable.context);
		readlength = l;
		if (l)
		{
			// demux it			
			m_CallbackTable.addref (p, m_CallbackTable.context);
			ptemp = p;
			while (l)
			{
				ASSERT (l > 0);
				temp = MPG_MIN (l, 32768);
				DEBUGMSG (0, ("demux (%d)\n", temp));
				demux (ptemp, temp);
				ptemp += temp;
				l -= temp;
				m_CurrentPosition += temp;
				if (m_BytesToSkip)
				{
					if (m_IFrameSubState == MPG_FOUND_2ND_PICTURE_HEADER)
					{
						m_MPEG1Demux.Reset ();
						m_ProgramDemux.Reset ();
						if (m_BytesToSkip > 0)
						{
							if (l < m_BytesToSkip)
							{
								m_BytesToSkip -= l;
								m_CurrentPosition += m_BytesToSkip;
								DEBUGMSG (1, ("skipping %d bytes to %d\n", (RMint32)m_BytesToSkip, (RMint32)m_CurrentPosition));
								m_CallbackTable.fseek (m_file, (RMint32)m_CurrentPosition, SEEK_SET, m_CallbackTable.context);							
							}
							else
							{
								l -= m_BytesToSkip;
								ptemp += m_BytesToSkip;
							}
						}
						else
						{
							if (m_IFramePosition != -1)
							{
								m_CurrentPosition = m_IFramePosition + m_BytesToSkip;
								if (m_CurrentPosition <= 0)
								{
									DEBUGMSG (1, ("(1) play backward hit start of file.\n"));
									m_CallbackTable.fseek (m_file, 0, SEEK_SET, m_CallbackTable.context);
									m_CallbackTable.release (p, m_CallbackTable.context);
									return MPG_DEMUX_ERROR_FILE_DONE;
								}
							}
							else
							{
								ASSERT (0);
								m_StartPosition -= 1024 * 128;
								if (m_StartPosition < 0)
								{
									DEBUGMSG (1, ("(2) play backward hit start of file.\n"));
									m_CallbackTable.fseek (m_file, 0, SEEK_SET, m_CallbackTable.context);
									m_CallbackTable.release (p, m_CallbackTable.context);
									return MPG_DEMUX_ERROR_FILE_DONE;
								}
								m_CurrentPosition = m_StartPosition;
							}
						}
						l = 0;
						m_IFrameState = 0;
						m_IFrameSubState = MPG_LOOKING_FOR_SEQUENCE_HEADER; 
						DEBUGMSG (1, ("seeking to %d\n", (RMint32)m_CurrentPosition));
						m_CallbackTable.fseek (m_file, (RMint32)m_CurrentPosition, SEEK_SET, m_CallbackTable.context);
					}
					else if (m_IFrameSubState == MPG_ERROR_SAME_IFRAME_DURING_REVERSE)
					{
						ASSERT (m_IFramePosition != -1);
						m_CurrentPosition = m_IFramePosition + m_BytesToSkip - 1024*32*m_ReverseErrorLoop;
						if (m_CurrentPosition < 0)
						{
							DEBUGMSG (1, ("(3) play backward hit start of file.\n"));
							m_CallbackTable.fseek (m_file, 0, SEEK_SET, m_CallbackTable.context);
							m_CallbackTable.release (p, m_CallbackTable.context);
							return MPG_DEMUX_ERROR_FILE_DONE;
						}
						l = 0;
						m_ReverseErrorLoop++;
						DEBUGMSG (1, ("m_ReverseErrorLoop = %d\n", (RMint32)m_ReverseErrorLoop));
						m_IFrameState = 0;
						m_IFrameSubState = MPG_LOOKING_FOR_SEQUENCE_HEADER; 
						DEBUGMSG (1, ("seeking to %d\n", (RMint32)m_CurrentPosition));
						m_CallbackTable.fseek (m_file, (RMint32)m_CurrentPosition, SEEK_SET, m_CallbackTable.context);
					}
				}				
			}
			m_CallbackTable.release (p, m_CallbackTable.context);
		}
		else
			return MPG_DEMUX_ERROR_FILE_DONE;
	}
	return MPG_DEMUX_ERROR_NOT_IMPLEMENTED;
}

MPG_DEMUX_ERROR MPEGDemux::SendIFramesOnly (RMint32 BytesToSkip)
{
	DEBUGMSG (1, ("MPEGDemux::SendIFramesOnly (%d)\n", BytesToSkip));
	if (m_BytesToSkip)
	{
		if (m_IFramePosition != -1)
		{
			DEBUGMSG (1, ("seeking to %d\n", (RMint32)m_IFramePosition));
			m_CallbackTable.fseek (m_file, (RMint32)m_IFramePosition, SEEK_SET, m_CallbackTable.context);
		}
	}
	// reset i frame state
	m_IFrameState = 0;
	m_IFrameSubState = MPG_LOOKING_FOR_SEQUENCE_HEADER;
	m_IFramePosition = -1;
	m_BytesToSkip = BytesToSkip;
	m_MPEG1Demux.Reset ();
	m_ProgramDemux.Reset ();
	if (m_DemuxType == DVD_DEMUX)
		m_ProgramDemux.m_IsDVDStream = 1;
	m_StartPosition = m_CurrentPosition;
	return MPG_DEMUX_ERROR_NO_ERROR;
}

//////////////////////////////////////////////////////////////////
#define TRANSPORT_BLOCK_LENGTH	(1024*16)
RMint32 MPEGDemux::IsTransportStream (RMuint16 *filename)
{
	DEBUGMSG (1, ("IsTransportStream (%s)\n", filename));
	// we will consider this a transport stream if we find at leat 5 sync words (0x47)
	// in the first TRANSPORT_BLOCK_LENGTH bytes of the file
	RMint32 IsTransport = 0, n, i;
	RMuint32 file = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	if (file == 0)
		return 0;
	RMuint8 *pBuffer = 0;
	RMuint32 length = TRANSPORT_BLOCK_LENGTH;
	m_CallbackTable.getDetectionBuffer (&pBuffer, &length, m_CallbackTable.context);	
	ASSERT (pBuffer);
	if (pBuffer)
	{
		n = m_CallbackTable.fread (file, pBuffer, TRANSPORT_BLOCK_LENGTH, m_CallbackTable.context);
		RMuint8 *p = pBuffer; 
		RMint32 sync_counter = 0;
		RMint32 sync_distance = 0;
		for (i=0; i<(n - 188); i++)
		{
			if (*p == 0x47)
			{
				if ((sync_distance == 0) || (sync_distance == 187))
					sync_counter++;
				sync_distance = 0;
			}
			else
				sync_distance++;
			p++;		
		}
		if (sync_counter >= 5)
		{
			IsTransport = 1;
		}
	}
	m_CallbackTable.fclose (file, m_CallbackTable.context);
	return IsTransport;
}

#define PROGRAM_BLOCK_LENGTH	(1024*4)
RMint32 MPEGDemux::IsProgramStream (RMuint16 *filename)
{
	DEBUGMSG (1, ("IsProgramStream (%s)\n", filename));

	// we will consider this a program stream if we find 0x000001BA in the first
	// PROGRAM_BLOCK_LENGTH bytes of the file.
	// Then we will try to detect the pack as a MPEG-2 Pack
	RMint32 IsProgram = 0, n, i;
	RMuint32 file = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	if (file == 0)
		return 0;
	RMuint8 *pBuffer = 0;
	RMuint32 length = PROGRAM_BLOCK_LENGTH;
	m_CallbackTable.getDetectionBuffer (&pBuffer, &length, m_CallbackTable.context);
	ASSERT (pBuffer);
	if (pBuffer)
	{
		RMint32 j;

		j = 0;
		while (j++ < 64)
		{
			n = m_CallbackTable.fread (file, pBuffer, PROGRAM_BLOCK_LENGTH, m_CallbackTable.context);
			RMuint8 *p = pBuffer; 
			for (i=0; i<(n - 5); i++)
			{
				if ((p[0] == 0x00) &&
					(p[1] == 0x00) &&
					(p[2] == 0x01) &&
					(p[3] == 0xBA))
				{
					// for program streams, after the pack start code,
					// 01b must be present - this is how we can tell the
					// difference between mpeg-2 program streams and mpeg-1 system streams
					if ((p[4] & 0xC0) == 0x40)
					{
						IsProgram = 1;
						j = 64;
					}
					break;
				}
				p++;		
			}
		}
	}
	m_CallbackTable.fclose (file, m_CallbackTable.context);
	return IsProgram;
}

#define MPEG1SYSTEM_BLOCK_LENGTH	(1024*4)
RMint32 MPEGDemux::IsMPEG1SystemStream (RMuint16 *filename)
{
	DEBUGMSG (1, ("IsMPEG1SystemStream (%s)\n", filename));

	// we will consider this a program stream if we find 0x000001BA in the first
	// PROGRAM_BLOCK_LENGTH bytes of the file.
	// Then we will try to detect the pack as a MPEG-1 Pack
	RMint32 IsSystem = 0, n, i;
	RMuint32 file = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	if (file == 0)
		return 0;
	RMuint8 *pBuffer = 0;
	RMuint32 length = PROGRAM_BLOCK_LENGTH;
	m_CallbackTable.getDetectionBuffer (&pBuffer, &length, m_CallbackTable.context);
	ASSERT (pBuffer);
	if (pBuffer)
	{
		n = m_CallbackTable.fread (file, pBuffer, MPEG1SYSTEM_BLOCK_LENGTH, m_CallbackTable.context);
		RMuint8 *p = pBuffer; 
		for (i=0; i<(n - 5); i++)
		{
			if ((p[0] == 0x00) &&
				(p[1] == 0x00) &&
				(p[2] == 0x01) &&
				(p[3] == 0xBA))
			{
				// for system streams, after the pack start code,
				// 0010b must be present - this is how we can tell the
				// difference between mpeg-2 program streams and mpeg-1 system streams
				if ((p[4] & 0xf0) == 0x20)
					IsSystem = 1;
				break;
			}
			p++;		
		}
	}
	m_CallbackTable.fclose (file, m_CallbackTable.context);
	return IsSystem;
}

#define DVD_BLOCK_LENGTH	(2048*5)
RMint32 MPEGDemux::IsDVDStream (RMuint16 *filename)
{
	DEBUGMSG (1, ("IsDVDStream (%s)\n", filename));

	// we consider this a DVD stream if there are PACK start codes 2048 bytes
	// apart AND the first pack is a Navigation pack
	RMint32 IsDVD = 0, n;
	RMuint32 file = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	if (file == 0)
		return 0;
	RMuint8 *pBuffer = 0;
	RMuint32 length = DVD_BLOCK_LENGTH;
	m_CallbackTable.getDetectionBuffer (&pBuffer, &length, m_CallbackTable.context);
	ASSERT (pBuffer);
	if (pBuffer)
	{
		DEBUGMSG (1, ("IsDVDStream: m_CallbackTable.fread\n"));
		n = m_CallbackTable.fread (file, pBuffer, DVD_BLOCK_LENGTH, m_CallbackTable.context);
		RMuint8 *p1 = pBuffer + 0;
		RMuint8 *p2 = pBuffer + 2048;
		RMuint8 *p3 = pBuffer + 4096;
		RMuint8 *p4 = pBuffer + 6144;
		RMuint8 *p5 = pBuffer + 8192;
		IsDVD = 1;
		// must be 5 packs seperated by 2k each
		if  (!(
			(p1[0] == 0x00) &&
			(p1[1] == 0x00) &&
			(p1[2] == 0x01) &&
			(p1[3] == 0xBA)))
		{		
			IsDVD = 0;
		}
		if  (!(
			(p2[0] == 0x00) &&
			(p2[1] == 0x00) &&
			(p2[2] == 0x01) &&
			(p2[3] == 0xBA)))
		{		
			IsDVD = 0;
		}
		if  (!(
			(p3[0] == 0x00) &&
			(p3[1] == 0x00) &&
			(p3[2] == 0x01) &&
			(p3[3] == 0xBA)))
		{		
			IsDVD = 0;
		}
		if  (!(
			(p4[0] == 0x00) &&
			(p4[1] == 0x00) &&
			(p4[2] == 0x01) &&
			(p4[3] == 0xBA)))
		{		
			IsDVD = 0;
		}
		if  (!(
			(p5[0] == 0x00) &&
			(p5[1] == 0x00) &&
			(p5[2] == 0x01) &&
			(p5[3] == 0xBA)))
		{		
			IsDVD = 0;
		}
		DEBUGMSG (1, ("IsDVDStream: IsDVD = %d\n", (int)IsDVD));
		if (IsDVD)
		{
			RMint32 i;
			m_CallbackTable.fseek (file, 0, SEEK_SET, m_CallbackTable.context);

			for (i=0; i<512; i++)
			{
				n = m_CallbackTable.fread (file, pBuffer, 2048, m_CallbackTable.context);
				if (n == 2048)
				{
					RMuint8 *pSystemHeader;
					pSystemHeader = pBuffer + 14;
					
					// try to find a navigation pack
					if  (!(
						(pSystemHeader[0] == 0x00) &&
						(pSystemHeader[1] == 0x00) &&
						(pSystemHeader[2] == 0x01) &&
						(pSystemHeader[3] == 0xBB)))
					{
						IsDVD = 0;
						continue;
					}
					RMuint8 *pPCIPacketHeader = pSystemHeader + 24;
					if  (!(
						(pPCIPacketHeader[0] == 0x00) &&
						(pPCIPacketHeader[1] == 0x00) &&
						(pPCIPacketHeader[2] == 0x01) &&
						(pPCIPacketHeader[3] == 0xBF) &&
						(pPCIPacketHeader[6] == 0x00)))
					{
						IsDVD = 0;
						continue;
					}
					RMuint8 *pDSIPacketHeader = pPCIPacketHeader + 6 + 1 + 979;
					if  (!(
						(pDSIPacketHeader[0] == 0x00) &&
						(pDSIPacketHeader[1] == 0x00) &&
						(pDSIPacketHeader[2] == 0x01) &&
						(pDSIPacketHeader[3] == 0xBF) &&
						(pDSIPacketHeader[6] == 0x01)))
					{
						IsDVD = 0;
						continue;
					}
					IsDVD = 1;
					break;
				}
			}
		}
	}
	m_CallbackTable.fclose (file, m_CallbackTable.context);
	return IsDVD;
}

RMint32 MPEGDemux::IsVCDStream (RMuint16 *filename)
{
	DEBUGMSG (1, ("IsVCDStream (%s)\n", filename));

	RMuint32 file = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	if (file == 0)
		return 0;

	RMint32 n;
	RMuint8 riffhdr[64];

	n = m_CallbackTable.fread (file, riffhdr, sizeof(riffhdr), m_CallbackTable.context);

	// header must be at least 44 bytes long
	if (n < 44)
	{
		m_CallbackTable.fclose (file, m_CallbackTable.context);
		return 0;
	}

	if ((riffhdr[0] != 'R') ||
		(riffhdr[1] != 'I') ||
		(riffhdr[2] != 'F') ||
		(riffhdr[3] != 'F'))
	{
		m_CallbackTable.fclose (file, m_CallbackTable.context);
		return 0;		
	}

	if ((riffhdr[ 8] != 'C') ||
		(riffhdr[ 9] != 'D') ||
		(riffhdr[10] != 'X') ||
		(riffhdr[11] != 'A') ||
		(riffhdr[12] != 'f') ||
		(riffhdr[13] != 'm') ||
		(riffhdr[14] != 't') ||
		(riffhdr[15] != ' '))
	{
		m_CallbackTable.fclose (file, m_CallbackTable.context);
		return 0;		
	}

	m_CallbackTable.fclose (file, m_CallbackTable.context);
	// 44 should be the file offset to the vcd data sectors
	return 44;
}

RMint32 MPEGDemux::IsMPEG124ElementaryStream (RMuint16 *filename, RMuint32 *pmpeg12)
{
	RMint32 IsMPEG12Elementary, i;
	DEBUGMSG (1, ("IsMPEG12ElementaryStream (%s)\n", filename));

	// look for start codes and check that there is no system start
	// start code
	IsMPEG12Elementary = 0;
	RMuint32 file = m_CallbackTable.fopen (filename, m_CallbackTable.context);
	if (file == 0)
		return IsMPEG12Elementary;
	RMuint8 *pBuffer = 0;
	RMuint8 *pDetectionBuffer = 0;
	RMuint32 length = 1024*8;
	m_CallbackTable.getDetectionBuffer (&pDetectionBuffer, &length, m_CallbackTable.context);
	ASSERT (pDetectionBuffer);
	pBuffer = pDetectionBuffer;
	ASSERT (pBuffer);
	if (pBuffer)
	{
		RMuint32 n;
		DEBUGMSG (1, ("IsMPEG12ElementaryStream: m_CallbackTable.fread\n"));
		n = m_CallbackTable.fread (file, pBuffer, 1024*8, m_CallbackTable.context);
		for (i=0; i<n-6; i++)
		{
			if ((pBuffer[0] == 0x00) &&
				(pBuffer[1] == 0x00) &&
				(pBuffer[2] == 0x01))
			{
				IsMPEG12Elementary++;
			}
			pBuffer++;
		}
	}
	m_CallbackTable.fclose (file, m_CallbackTable.context);
	DEBUGMSG (1, ("IsMPEG12ElementaryStream: IsMPEG12Elementary = %d\n", (int)IsMPEG12Elementary));
	if (IsMPEG12Elementary > 20)
	{
		// now take a closer look at the buffer to see if this is an
		// mpeg-1/mpeg-2 or a mpeg-4 stream
		
		// do it a very cheesy way ...
		pBuffer = pDetectionBuffer;
		if ((pBuffer[0] == 0x00) &&
			(pBuffer[1] == 0x00) &&
			(pBuffer[2] == 0x01) &&
			(pBuffer[3] <= 0x2f))
		{
			// mpeg-4
			*pmpeg12 = 0;
			m_CallbackTable.info (MSG_MSG_MPEG4_ELEMENTARY, 0, m_CallbackTable.context);
		}
		else
		if ((pBuffer[0] == 0x00) &&
			(pBuffer[1] == 0x00) &&
			(pBuffer[2] == 0x01) &&
			(pBuffer[3] == 0xb0))
		{
			// mpeg-4
			*pmpeg12 = 0;
			m_CallbackTable.info (MSG_MSG_MPEG4_ELEMENTARY, 0, m_CallbackTable.context);
		}
		else
		{
			// mpeg-1/2
			*pmpeg12 = 1;
			m_CallbackTable.info (MSG_MSG_MPEG12_ELEMENTARY, 0, m_CallbackTable.context);
		}
		return 1;
	}
	return 0;
}

RMint32 MPEGDemux::reformat_vcdsectors (RMuint8 *p, RMint32 l)
{
	while (l >= 2352)
	{
		memset (p, 0xff, 24);
		memset (p+24+2324, 0xff, 4);
		p += 2352;
		l -= 2352;		
	}
	return l;
}

RMint32	MPEGDemux::demux (RMuint8 *p, RMuint32 l)
{
	switch (m_DemuxType)
	{
	case PROGRAM_DEMUX:
	case DVD_DEMUX:
		m_ProgramDemux.Demux (p, l);
		break;
	case DAT_DEMUX:
		reformat_vcdsectors (p, l/2352*2352);
	case SYSTEM_DEMUX:
		m_MPEG1Demux.Demux (p, l);
		break;
	case NO_DEMUX:
		m_CallbackTable.addref (p, m_CallbackTable.context);
		m_CallbackTable.putpayload (p, l, 0xe0, 0,
			0, 0, 0, 0, m_CallbackTable.context);
		m_CallbackTable.release (p, m_CallbackTable.context);
		break;
	default:
		ASSERT (0);
		break;
	}
	return 0;
}

void MPEGDemux::MPEG1DemuxCallback (RMuint8 *pData, RMuint32 Length, RMuint8 PacketId, 
	RMint64 Scr, RMint64 Pts, RMint64 Dts, RMuint8 Flags, RMint64 offset)
{
	ASSERT (m_CallbackTable.putpayload);
	if (m_BytesToSkip == 0)
	{
		m_CallbackTable.putpayload (pData, Length, PacketId, 0,
			Scr, Pts, Dts, Flags, m_CallbackTable.context);
	}
	else
	{
		// video only
		if ((PacketId & 0xf0) == 0xe0)
		{
			if ((m_IFrameSubState == MPG_LOOKING_FOR_SEQUENCE_HEADER) ||
				(m_IFrameSubState == MPG_LOOKING_FOR_1ST_PICTURE_HEADER))
			{

/*
				if(m_IFrameSubState == MPG_LOOKING_FOR_SEQUENCE_HEADER )
					{
					printf("SeqHdr[ ");
					printf("%x ", pData+0 );
					printf("%x ", pData+1 );
					printf("%x ", pData+2 );
					printf("%x ", pData+3 );
					printf("%x ", pData+4 );					
					printf("%x ", pData+5 );
					printf("%x ]\n", pData+6 );

					}
*/			
				if (Flags == 0)
					return;
			}

			RMint32 l;
			RMuint32 sendit;
			ASSERT (m_BytesToSkip);
			sendit = 0;
			l = iframe_parser (pData, Length, &sendit);
			if (m_IFrameSubState == MPG_FOUND_2ND_PICTURE_HEADER)
				Flags = 0;
			ASSERT (l <= Length);
			if (sendit)
			{
				DEBUGMSG (((Flags & PTS_VALID_FLAG) == 0), ("(IFrame) m_CallbackTable.putpayload (%d)\n", (RMint32)(Length-l)));
				DEBUGMSG ((Flags & PTS_VALID_FLAG), ("(IFrame) m_CallbackTable.putpayload (%d) pts=%d\n", (RMint32)(Length-l), (RMint32)Pts));
				m_CallbackTable.putpayload (pData, Length - l, PacketId, 0,
					Scr, Pts, Dts, Flags, m_CallbackTable.context);
			}
		}
	}
}

void MPEGDemux::PESDemuxCallback (RMuint8 *pData, RMuint32 Length, RMuint8 StreamId, RMuint8 SubStreamId, 
	RMint64 Scr, RMint64 Pts, RMint64 Dts, RMuint8 Flags, RMint64 offset)
{
	ASSERT (m_CallbackTable.putpayload);
	if (m_BytesToSkip == 0)
	{
		m_CallbackTable.putpayload (pData, Length, StreamId, SubStreamId,
			Scr, Pts, Dts, Flags, m_CallbackTable.context);
	}
	else
	{
		if ((StreamId & 0xf0) == 0xe0)
		{
			////////////////////////////////////////////////////////////////////////
			// this is a shortcut to avoid parsing everything
			// XXX WARNING:
			// for this shortcut to work, every I frame MUST have a PTS.
			// In addition, every frame follwing the I frame (in decode
			// order) must also have a PTS.
			if ((m_IFrameSubState == MPG_LOOKING_FOR_SEQUENCE_HEADER) ||
				(m_IFrameSubState == MPG_LOOKING_FOR_1ST_PICTURE_HEADER))
			{				
				if (Flags == 0)
				{
					return;
				}
			}			
			else if (m_IFrameSubState == MPG_LOOKING_FOR_2ND_PICTURE_HEADER)
			{
				if (Flags == 0)
				{					
					// just send it
					m_CallbackTable.putpayload (pData, Length, StreamId, SubStreamId,
						Scr, Pts, Dts, Flags, m_CallbackTable.context);
					return;
				}
			}			
			// end shortcut
			///////////////////////////////////////////////////////////////////////////

			RMint32 l;
			RMuint32 sendit;
			ASSERT (m_BytesToSkip);
			sendit = 0;
			l = iframe_parser (pData, Length, &sendit);
			if (m_IFrameSubState == MPG_FOUND_2ND_PICTURE_HEADER)
				Flags = 0;
			ASSERT (l <= Length);
			if (sendit)
			{
				DEBUGMSG (((Flags & PTS_VALID_FLAG) == 0), ("(IFrame) m_CallbackTable.putpayload (%d)\n", Length-l));
				DEBUGMSG ((Flags & PTS_VALID_FLAG), ("(IFrame) m_CallbackTable.putpayload (%d) pts=%d\n", Length-l, (RMint32)Pts));
				m_CallbackTable.putpayload (pData, Length-l, StreamId, SubStreamId,
					Scr, Pts, Dts, Flags, m_CallbackTable.context);
			}
		}
	}
}

void MPEGDemux::AC3DTSInfoCallback (RMuint8 numberOfFrameHeaders, RMuint16 firstAccessUnitPointer)
{
	AC3DTS_INFO info;
	info.nFrameHeaders = numberOfFrameHeaders;
	info.FirstAccessUnitPointer = firstAccessUnitPointer;
	m_CallbackTable.info (MSG_MSG_AC3DTS_INFO, &info, m_CallbackTable.context);
}

void MPEGDemux::LPCMInfoCallback (RMuint8 numberOfFrameHeaders, RMuint16 firstAccessUnitPointer, RMuint32 frequency, 
		RMuint8 numberOfChannels, RMuint8 quantizationWordLength)
{
	LPCM_INFO info;
	info.nFrameHeaders = numberOfFrameHeaders;
	info.FirstAccessUnitPointer = firstAccessUnitPointer;
	info.Frequency = frequency;
	info.NumberOfChannels = numberOfChannels + 1;
	info.QuantizationWordLength = quantizationWordLength;
	m_CallbackTable.info (MSG_MSG_LPCM_INFO, &info, m_CallbackTable.context);
}

RMint32 MPEGDemux::iframe_parser (RMuint8 *p, RMint32 len, RMuint32 *sendit)
{
	if (m_IFrameSubState == MPG_LOOKING_FOR_2ND_PICTURE_HEADER)
		*sendit |= 1;
	else if (m_IFrameSubState == MPG_FOUND_2ND_PICTURE_HEADER)
	{
		m_MPEG1Demux.Reset ();
		m_MPEG1Demux.ResetState ();
		return len;
	}
	else if (m_IFrameSubState == MPG_ERROR_SAME_IFRAME_DURING_REVERSE)
	{
		m_MPEG1Demux.Reset ();
		m_MPEG1Demux.ResetState ();
		return len;
	}
	while (len)
	{
		switch (m_IFrameState)
		{		
			case 0:	// found 0xxxxxxxxx looking for 0x00xxxxxx
				if (*p == 0)
					m_IFrameState++;
				break;
			case 1: // found 0x00xxxxxx looking for 0x0000xxxx
				if (*p == 0)
					m_IFrameState++;
				else
					m_IFrameState = 0;
				break;
			case 2:	// found 0x0000xxxx looking for 0x000001xx
				if (*p == 1)
					m_IFrameState++;
				else if (*p == 0);					
				else
					m_IFrameState = 0;
				break;
			case 3: // found 0x000001xx 
				if (m_IFrameSubState == MPG_LOOKING_FOR_SEQUENCE_HEADER)
				{
					// sequence header OR group of pictures header
					if ((*p == 0xb8) || (*p == 0xb3))
					{
						DEBUGMSG ((*p == 0xb8), ("FOUND GOP HEADER\n"));
						DEBUGMSG ((*p == 0xb3), ("FOUND SEQUENCE HEADER\n"));
						m_IFrameSubState = MPG_LOOKING_FOR_1ST_PICTURE_HEADER;
						*sendit |= 1;
					}

				}
				else
				if (m_IFrameSubState == MPG_LOOKING_FOR_1ST_PICTURE_HEADER)
				{
					if (*p == 0x00)
					{
						DEBUGMSG (1, ("FOUND START PICTURE HEADER (%d)\n", (RMint32)m_CurrentPosition));
						m_IFrameSubState = MPG_LOOKING_FOR_2ND_PICTURE_HEADER;
						*sendit |= 1;
						if (m_BytesToSkip < 0)
						{
							if (m_IFramePosition == -1)
								m_IFramePosition = m_CurrentPosition;
							else if (m_CurrentPosition >= m_IFramePosition)
							{
								*sendit = 0;
								DEBUGMSG (1, ("MPG_ERROR_SAME_IFRAME_DURING_REVERSE\n"));
								m_IFrameSubState = MPG_ERROR_SAME_IFRAME_DURING_REVERSE;
							}
							else
								m_IFramePosition = m_CurrentPosition;
						}
						else
						{
							m_IFramePosition = m_CurrentPosition;
						}
						DEBUGMSG (1, ("m_IFramePosition = %d\n", (RMint32)m_IFramePosition));
					}				
				}
				else 
				if (m_IFrameSubState == MPG_LOOKING_FOR_2ND_PICTURE_HEADER)
				{
					if (*p == 0x00)
					{
						m_ReverseErrorLoop = 0;
						DEBUGMSG (1, ("FOUND END PICTURE HEADER\n"));
						m_IFrameSubState = MPG_FOUND_2ND_PICTURE_HEADER;
						return len;
					}				
				}				
				m_IFrameState = 0;
				break;
			default:
				ASSERT (0);
				break;
		}
		len--;
		p++;
	}
	return len;
}


#if 1
#define THS			0xFFF00000L
#define TVS			0xFFF00L
#define TPAR			0xF0L
#define TAR			0xFL
#define TBR			0xFFFFF000L

#define TCH			0x7C000000L
#define TCM			0x3F00000L
#define TCS			0x7E000L


RMint64 MPEGDemux::mpg_info( RMuint16* filename )
{
	int TotalSec, StartTimeSec, EndTimeSec;
	RMuint32 file;
	RMuint32 StartFD, EndFD;
	unsigned char pBuffer[PROGRAM_BLOCK_LENGTH];
	int idx, iGOP, rc;
	long bsp, tcode;
	long last=0, last_b=0;
	int sh, sm, ss;
	int eh, em, es;

	RMuint32 LoadingProgress = 0;

	DEBUGMSG(1,("\nMPEGDemux::mpg_info()\n"));

//	fp = fopen( "./sea.mpg", "r" );
	if( (	file = m_CallbackTable.fopen ( filename, m_CallbackTable.context )) == 0 )
		{
		printf("faild to fopen\n");
		return 0;
		}


	////////////////////////////////////////////////////////////
	// Sequence Layer Process
	memset( pBuffer, 0, PROGRAM_BLOCK_LENGTH );
	rc = m_CallbackTable.fread ( file, pBuffer, PROGRAM_BLOCK_LENGTH, m_CallbackTable.context);
	for( idx=0; idx<rc; idx++ )
		{
		if( 	(pBuffer[idx] == 0x00) && 
			(pBuffer[idx+1] == 0x00) && 
			(pBuffer[idx+2] == 0x01) && 
			(pBuffer[idx+3] == 0xb3) )
			{
			
			tcode = 0;
			tcode |= pBuffer[idx+7];
			tcode |= pBuffer[idx+6]<<8;
			tcode |= pBuffer[idx+5]<<16;				
			tcode |= pBuffer[idx+4]<<24;

			/*
			typedef struct _t_SEQ_LAYER
			{
				int HorizontalSize;
				int VerticalSize;
				int PelAspectRatio;
				int PictureRate;
				int BitRate;
				int VBVBufferSize;
			} _ATTRIBUTE_PACKED_ SEQ_LAYER;
			*/

			//   0000 0000 0000 0000   0000 0000 0000 0000
                     //   1111 1111 1111 0000   0000 0000 0000 0000  		HorizontalSize	
			//   0000 0000 0000 1111   1111 1111	0000 0000		VerticalSize
			//	0000 0000 0000 0000   0000 0000 1111 0000		PelAspectRatio
			//	0000 0000 0000 0000   0000 0000 0000 1111		PictureRate

			m_SEQ.HorizontalSize 	= (tcode&THS)>>20;
			m_SEQ.VerticalSize	= (tcode&TVS)>>8;
			m_SEQ.PelAspectRatio	= (tcode&TPAR)>>4;
			m_SEQ.PictureRate	= (tcode&TAR);

	gVideofilePlayinfo.OriginalWidth	= m_SEQ.HorizontalSize;
	gVideofilePlayinfo.OriginalHeight	= m_SEQ.VerticalSize;

			DEBUGMSG(1,("-m_SEQ.HorizontalSize:%d\n",m_SEQ.HorizontalSize));
			DEBUGMSG(1,("-m_SEQ.VerticalSize:%d\n",m_SEQ.VerticalSize));
			DEBUGMSG(1,("-m_SEQ.PelAspectRatio:%d\n",m_SEQ.PelAspectRatio));
			DEBUGMSG(1,("-m_SEQ.PictureRate:%d\n",m_SEQ.PictureRate));

 			tcode = 0;
			tcode |= pBuffer[idx+11];
			tcode |= pBuffer[idx+10]<<8;
			tcode |= pBuffer[idx+9]<<16;				
			tcode |= pBuffer[idx+8]<<24;

			m_SEQ.BitRate		= (tcode&TBR)>>12;
			DEBUGMSG(1,("-m_SEQ.BitRate:%d\n",m_SEQ.BitRate));

			//   0000 0000 0000 0000   0000 0000 0000 0000
			//   1111 1111 1111 1111   1111						BitRate
			//	

			}			
		}
			

	////////////////////////////////////////////////////////////
	// GOP Layer Process 1	
	memset( pBuffer, 0, PROGRAM_BLOCK_LENGTH );
	rc = m_CallbackTable.fread ( file, pBuffer, PROGRAM_BLOCK_LENGTH, m_CallbackTable.context);
	for( idx=0; idx<rc; idx++ )
		{
		if( 	(pBuffer[idx] == 0x00) && 
			(pBuffer[idx+1] == 0x00) && 
			(pBuffer[idx+2] == 0x01) && 
			(pBuffer[idx+3] == 0xb8) )
			{
			tcode = 0;
			tcode |= pBuffer[idx+7];
			tcode |= pBuffer[idx+6]<<8;
			tcode |= pBuffer[idx+5]<<16;				
			tcode |= pBuffer[idx+4]<<24;

			sh = (tcode&TCH)>>26;
			sm = (tcode&TCM)>>20;
			ss =  (tcode&TCS)>>13;
			StartTimeSec = (sh*60*60) + (sm*60) + ss;
			StartFD = m_CallbackTable.ftell( file, m_CallbackTable.context ) + idx;

			}
		}


	////////////////////////////////////////////////////////////
	// GOP Layer Process 2
//	if(fseek( fp, -100000L, SEEK_END )>0)
	m_CallbackTable.fseek( file, 0, SEEK_END, m_CallbackTable.context );
	m_FileSize = m_CallbackTable.ftell( file, m_CallbackTable.context );
	
	for( int loop=1; loop<400; loop++ )
		{


		/*
		memset( buf, 0, 8192 );
		rd = fread(buf, sizeof(unsigned char), 8192, fp );
		*/

		memset( pBuffer, 0, PROGRAM_BLOCK_LENGTH );
		m_CallbackTable.fseek ( file, m_FileSize-PROGRAM_BLOCK_LENGTH*loop, SEEK_SET, m_CallbackTable.context);
		rc = m_CallbackTable.fread ( file, pBuffer, PROGRAM_BLOCK_LENGTH, m_CallbackTable.context);

		for( idx=0; idx<rc; idx++ )
			{
			if( 	(pBuffer[idx] == 0x00) && 
				(pBuffer[idx+1] == 0x00) && 
				(pBuffer[idx+2] == 0x01) && 
				(pBuffer[idx+3] == 0xb8) )
				{
				tcode = 0;
				tcode |= pBuffer[idx+7];
				tcode |= pBuffer[idx+6]<<8;
				tcode |= pBuffer[idx+5]<<16;				
				tcode |= pBuffer[idx+4]<<24;

				printf("%d-%d tcode[%x]\n", loop, idx, tcode );

				// 0000 0000 0101 1010 0000 0000 0000 0000
				//   hhh hh
				//             11 1111 
				//                           111 111
				//                                      1 1111 1

				eh = (int)((tcode&TCH)>>26);
				em = (int)((tcode&TCM)>>20);
				es =  (int)((tcode&TCS)>>13);
				
				EndTimeSec = (eh*60*60) + (em*60) + es;

				EndFD = m_CallbackTable.ftell( file, m_CallbackTable.context ) + idx;

				//EndTimeSec = ((tcode&TCH)>>26)*60*60 + ((tcode&TCM)>>20)*60 + (tcode&TCS)>>13;

				loop = 400;

				}
			}



			if( LoadingProgress > 95 )
				{
				LoadingProgress = 95;
				}
			else
				{
				m_CallbackTable.loading( LoadingProgress, m_CallbackTable.context );
				LoadingProgress+=5;				
				}

		}

	TotalSec = EndTimeSec - StartTimeSec;
	if( TotalSec < 0 )
		TotalSec = EndTimeSec;

	DEBUGMSG(1,("StartTimeSec:%lld\nEndTimeSec:%lld\n", StartTimeSec, EndTimeSec ));

	if(	gVideofilePlayinfo.OriginalWidth == 0 ||
		gVideofilePlayinfo.OriginalHeight == 0 )
		{
		gVideofilePlayinfo.OriginalWidth	= 352;
		gVideofilePlayinfo.OriginalHeight	= 240;
		}		
	
	totaltime.SecOnly 	= TotalSec;
	totaltime.hours 		= (int) (totaltime.SecOnly / 3600);
	totaltime.minutes 	= (int) ((totaltime.SecOnly - totaltime.hours * 3600) / 60);
	totaltime.seconds 	= (int) (totaltime.SecOnly - totaltime.hours*3600 - totaltime.minutes*60);
	BytesPerSecond 		= (m_FileSize-StartFD) /TotalSec;

	if(!BytesPerSecond)
		{
		BytesPerSecond=(m_SEQ.BitRate*400)/4;
		totaltime.SecOnly = (m_FileSize-StartFD) /BytesPerSecond *2;
		}

	DEBUGMSG(1,("mpg_info()\n-totaltime.SecOnly:%lld\n-totaltime.hours:%d\n-totaltime.minutes:%d\n-totaltime.seconds:%d\n", 
		totaltime.SecOnly, totaltime.hours, totaltime.minutes, totaltime.seconds ));

	m_CallbackTable.fclose(file, m_CallbackTable.context);
	LoadingProgress = 100;
	m_CallbackTable.loading( LoadingProgress, m_CallbackTable.context );

	return true;
}
#endif

MPG_DEMUX_ERROR MPEGDemux::Seek( RMuint32 t )
{
	if( (int)t < 0 ) 
		return MPG_DEMUX_ERROR_SEEK_FAILED;
	
	m_CurrentPosition = m_CallbackTable.fseek (
		m_file, 
		(RMint32)(t * BytesPerSecond), 
		SEEK_SET, 
		m_CallbackTable.context
		);
	
	m_MPEG1Demux.Reset ();
	m_MPEG1Demux.ResetState ();
	m_ProgramDemux.Reset ();
	
	return MPG_DEMUX_ERROR_NO_ERROR;
}




