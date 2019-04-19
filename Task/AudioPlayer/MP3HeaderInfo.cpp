//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : MP3 Header Information class definition
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/MP3HeaderInfo.cpp,v $
// $Revision: 1.5 $
// $Date: 2006/04/19 09:31:58 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: MP3HeaderInfo.cpp,v $
// Revision 1.5  2006/04/19 09:31:58  zstein
// - support MPEG version 2 Layer 2.
//
// Revision 1.4  2006/04/15 01:24:06  zstein
// - MPEG version 2.5
//
// Revision 1.3  2006/04/05 01:48:12  zstein
// - updated a calculating duration routine.
//
// Revision 1.2  2006/03/07 07:22:55  zstein
// - corrected time info.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.26  2006/02/21 05:24:26  zstein
// - fixed duration time bug.
//
// Revision 1.25  2006/02/11 08:56:14  zstein
// - fixed duration time bug.
//
// Revision 1.24  2006/01/06 11:20:40  zstein
// *** empty log message ***
//
// Revision 1.23  2005/12/26 08:55:07  zstein
// - deleted debug messages.
//
// Revision 1.22  2005/12/26 07:31:48  zstein
// - cleared debug messages.
//
// Revision 1.21  2005/12/19 08:08:39  zstein
// *** empty log message ***
//
// Revision 1.20  2005/12/09 03:39:00  zstein
// *** empty log message ***
//
// Revision 1.19  2005/11/08 01:20:34  zstein
// *** empty log message ***
//
// Revision 1.18  2005/09/14 06:52:34  zstein
// - change DecodeFile( char* ) to DecodeFile( char*, int )
//******************************************************************************
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "MP3HeaderInfo.h"

#include "rmbasic.h"
#include "fatfs.h"
#include "Component/FileUtil.h"
#define DEBUG	0
#if DEBUG
#define MP3DEC_DEBUG		printf
#else
#define MP3DEC_DEBUG
#endif

static short BitrateIndex_V1L1[] = {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 284, 416, 448, -1};
static short BitrateIndex_V1L2[] = {0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, -1};
static short BitrateIndex_V1L3[] = {0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, -1};
static short BitrateIndex_V2L1[] = {0, 32, 48, 58,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, -1};
static short BitrateIndex_V2L2L3[] = {0,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, -1};
static int SRIndex_MPEG1[] = { 44100, 48000, 32000, 0 };
static int SRIndex_MPEG2[] = { 22050, 24000, 16000 };
static int SRIndex_MPEG25[] = { 11025, 12000, 8000 };

MP3HdrInfo::MP3HdrInfo( void )
{
}

MP3HdrInfo::~MP3HdrInfo( void )
{
}

int MP3HdrInfo::DecodeHeader( const char* header )
{
	int index;
	short* pIndex;
	int* pSRIndex;
	
	// decode MPEG Audio Version
	m_nVersion = ( header[1]&0x18) >> 3;
	// decode Layer description
	m_nLayer = (header[1]&0x06) >> 1;
	// decode protected bit
	m_fProtected = ( bool )( header[1]&0x01 );

	// decode bitrate
	index = ( header[2]&0xf0 ) >> 4;
	if( m_nVersion == MPEG_VERSION_1 && m_nLayer == MPEG_LAYER_1 ) pIndex = BitrateIndex_V1L1;
	else if( m_nVersion == MPEG_VERSION_1 && m_nLayer == MPEG_LAYER_2 ) pIndex = BitrateIndex_V1L2;
	else if( m_nVersion == MPEG_VERSION_1 && m_nLayer == MPEG_LAYER_3 ) pIndex = BitrateIndex_V1L3;
	else if( m_nVersion == MPEG_VERSION_2 && m_nLayer == MPEG_LAYER_1 ) pIndex = BitrateIndex_V2L1;
	else pIndex = BitrateIndex_V2L2L3;
	m_nBitrate = pIndex[index];
	// decode sampling rate
	index = ( header[2]&0x0c ) >> 2;
	if( m_nVersion == MPEG_VERSION_1 ) pSRIndex = SRIndex_MPEG1;
	else if( m_nVersion == MPEG_VERSION_2 ) pSRIndex = SRIndex_MPEG2;
	else pSRIndex = SRIndex_MPEG25;
	m_nSamplingrate = pSRIndex[index];
	// decode padding bit
	m_nPadding = ( header[2]&0x02 ) >> 1;
	// decode private bit
	m_fPrivate = ( bool )( header[2]&0x01 );

	 // get next byte
	 //ch = FATFS_fgetc( sdFd );
	 m_nChannel = ( header[3]&0xc0 ) >> 6;
	 // decode mode extension
	 m_nModeExt = ( header[3]&0x30 ) >> 4;
	 // decode copyright
	 m_fCopyright = ( bool )( header[3]&0x08 ) >> 3;
	 // decode original 
	 m_fOriginal = ( bool )( header[3]&0x04 ) >> 2;
	 // decode emphasis
	 m_nEmphasis = ( header[3]&0x03 );

	 return index;
}

int MP3HdrInfo::CalcFrameSize()
{
	int fsize=0;

	if( m_nLayer == MPEG_LAYER_1 ) {
		if( m_nVersion == MPEG_VERSION_1 ) 
			fsize = (48000*m_nBitrate/m_nSamplingrate);
		else
			fsize = (24000*m_nBitrate/m_nSamplingrate);
		if( m_nPadding ) {
			fsize = (m_nFrameSize+0x03)&~0x03;
		}
	}
	else if( m_nLayer == MPEG_LAYER_2 || m_nLayer == MPEG_LAYER_3 ) {
		if( m_nVersion == MPEG_VERSION_1 || m_nVersion == MPEG_VERSION_2 )
			fsize = (144000*m_nBitrate/m_nSamplingrate);
		else
			fsize = (72000*m_nBitrate/m_nSamplingrate);
	}

	if( m_nPadding ) {
		//if( m_nLayer == MPEG_LAYER_1 ) m_nFrameSize++;
		fsize++;
	}


	return fsize;
}

bool MP3HdrInfo::Decode( const char* szPath, int unicode )
{
	if( !szPath ) return false;

	char header[4];
	int nFirstFrmOffset = 0;
	m_fVBR = false;
	FATFS_ERROR err;
	RMuint32 sdFd;
	RMuint32 sdEOF;
	RMuint32 nBytes, i;
	char ch;
	RMint64 nCount = 0;
	int seek_pos = 0;
	int* pHeader = ( int* )header;

	if( unicode ) {
		err = fatfs_wfopen( (RMuint16*)szPath, _O_RDONLY, &sdFd );
	}
	else {
		err = fatfs_fopen( (RMuint8*)szPath, _O_RDONLY, &sdFd );
	}
	if( err != FATFS_ERROR_NO_ERROR ) {
		printf( "failed to load file(%s, 0x%x)\n", szPath, err );
		return false;
	}

	bool fIsMP3 = false;
	int old=0, offset;
	char version, layer;

    while( 1 ) {
    	FATFS_fResetIndex();
    	while( 1 ) {
    		fatfs_feof( sdFd, &sdEOF );
    		if( sdEOF ) break;
    		ch = FATFS_fgetc( sdFd );
    		nCount++;
    		if( ch == 0xff ) {
    			header[0] = ch;
    			ch = FATFS_fgetc( sdFd );
    			nCount++;
                /*
                    MPEG Version 1 or 2, Layer III
                */
                if( (ch&0xe0) != 0xe0 ) {
                    continue;
                }
                
                version = (ch&0x18)>>3;
                if( version == 0x01 ) {
                    continue;
                }
                layer = (ch&0x06) >> 1;
                if( layer != 0x01 /* Layer III */ && layer != 0x2 ) {
                    continue;
                }
                
    			header[1] = ch;
    			ch = FATFS_fgetc( sdFd );
    			nCount++;
    			if( (ch&0xf0) != 0xf0 ) {
    				header[2] = ch;
    				header[3]  = FATFS_fgetc( sdFd );
    				nCount++;
    				fIsMP3 = true;
    				break;
    			}
    		}
    	}

    	if( fIsMP3 == false ) {
    		MP3DEC_DEBUG( "is not mp3..\n" );
    		fatfs_fclose( sdFd );
    		return false;
    	}

    	nFirstFrmOffset = nCount-4;
    	MP3DEC_DEBUG( "ok found.. %d 0x%02x 0x%02x 0x%02x 0x%02x\n", 
    	        nCount, header[0], header[1], header[2], header[3] );

    	DecodeHeader( header );
    	m_nFrameSize = CalcFrameSize();
    	if( m_nFrameSize == 0 ) {
    	    continue;
    	}

    	// Check Next frame
    	nCount = nFirstFrmOffset + m_nFrameSize;
    	fatfs_fseek( sdFd, (RMuint32)nCount, SEEK_SET );
    	fatfs_fread( sdFd, (RMuint8*)header, sizeof(header), &nBytes );
    	if( header[0] != 0xff ) {
    	    fatfs_fseek( sdFd, (RMuint32)nCount, SEEK_SET );
    	}
    	else {
    	    break;
    	}
    }
    

	MP3DEC_DEBUG( "HEADER -> 0x%08lx, old(%d)\n", *pHeader, old  );

	 // caculate framesize and frame count. 
	 // Support VBR
	 m_nFrameCount = 1;
	 
	 int *p = (int*)header;
	 int nOldBitrate = m_nBitrate;
	 int sf_idx;
	 while( 1 ) {
	 	// VBR sample frame count is 10.
	 	if( !m_fVBR && m_nFrameCount > 10 ) break; // Constant Bitrate.

		MP3DEC_DEBUG( "SEEK: %d\n", nCount );
		fatfs_fseek64( sdFd, (RMuint32)nCount, SEEK_SET );
		err = fatfs_fread( sdFd, (RMuint8*)header, sizeof(header), &nBytes );

		if( nBytes == 0 || err != FATFS_ERROR_NO_ERROR ) {
			MP3DEC_DEBUG( "end??: 0x%x, count: %d\n", err, nCount );
			break;
		}
		
		if( (header[0]&0xff) != 0xff ) {
			MP3DEC_DEBUG( "mp3 frame end...0x%x, nCount:%d\n", *pHeader, nCount );
			break;
		}

		sf_idx = DecodeHeader( header );
		m_nFrameSize = CalcFrameSize();
		nCount += m_nFrameSize;
		m_nFrameCount++;
		MP3DEC_DEBUG( "frame count: %d, frame size: %d, seek pos: %d\n", 
			m_nFrameCount, m_nFrameSize, nCount );
		if( m_nSamplingrate == 0 || m_nBitrate == 0 ) {
			m_fVBR = false;
			break;
		}
		if( nOldBitrate != m_nBitrate ) m_fVBR = true;
		nOldBitrate = m_nBitrate;
	 }

	 MP3DEC_DEBUG( "size: %d, count: %d, vbr: %d\n", m_nFrameSize, m_nFrameCount, m_fVBR );

	 // calcurate number of frames
	 if( !m_fVBR ) {
		 RMint64 nSizeInByte;
		 fatfs_fsize64( sdFd, (RMint64*)&nSizeInByte );
		 MP3DEC_DEBUG( "Filesize: %ld\n", nSizeInByte );
		 int size = nSizeInByte - nFirstFrmOffset;
		 m_nFrameCount = size / m_nFrameSize;
		 MP3DEC_DEBUG( "m_nFrameSize: %d\n", m_nFrameSize );
		 MP3DEC_DEBUG( "m_nFrameCount: %ld\n", m_nFrameCount );
		  float div;
		 switch( sf_idx ) {
		 case 0: div = 38.5f; break; // 44.1KHz
		 case 1: div = 32.5f; break; // 48KHz
		 case 2: div = 27.8f; break; // 32KHz
		 default:
		 	div = 27.8f; break;
		 }
		 MP3DEC_DEBUG( "sf_idx: %d, m_nBitrate: %d, size: %d\n", 
		                sf_idx, m_nBitrate, nSizeInByte );
		 //m_nTotalTime = (((float)m_nFrameCount/div));
		 m_nTotalTime = (nSizeInByte-nFirstFrmOffset)/(m_nBitrate/8);
		 m_nTotalTime /= 1000;
		 MP3DEC_DEBUG( "total time: %d\n", m_nTotalTime );
	 }
	 else {
		 // calcurate total time
		 MP3DEC_DEBUG( "m_nFrameSize: %d\n", m_nFrameSize );
		 MP3DEC_DEBUG( "m_nFrameCount: %d\n", m_nFrameCount );
		m_nTotalTime = (((float)m_nFrameCount*26/1000 ));
	 }

	fatfs_fclose( sdFd );

	 return true;
}
