//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : MPEG1, MPEG2 Demux Header
// 
// $Source: /home/cvsroot/TAVI020/tavi/mpegdec/mpegdec.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: mpegdec.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/10/13 01:25:29  etnlwind
// Update for Header Info
//
//*****************************************************************************
#ifndef _MPEGDEC_H
#define _MPEGDEC_H

#include "rmbasic.h"

typedef enum 
{
	MPEG_DECODER_ERROR_NO_ERROR,
	MPEG_DECODER_ERROR_NO_DECODER,
	MPEG_DECODER_ERROR_NO_ROOM,
	MPEG_DECODER_ERROR_EOF,
	MPEG_DECODER_ERROR_UNSPECIFIED_ERROR
} MPEG_DECODER_ERROR;

typedef struct tagPCM_PARAMETERS
{
	RMuint32 SamplesPerSecond;
	RMuint32 NumberOfChannels;
	RMuint32 NumberOfBitsPerSample;

} PCM_PARAMETERS;

typedef struct tagAC3_PARAMETERS
{
	RMuint32 SamplesPerSecond;
	RMuint32 rawOutput;

} AC3_PARAMETERS;

typedef struct tagDTS_PARAMETERS
{
	RMuint32 SamplesPerSecond;
	RMuint32 rawOutput;

} DTS_PARAMETERS;

typedef struct tagMPEGAUDIO_PARAMETERS
{
	RMuint32 SamplesPerSecond;
	RMuint32 rawOutput;
	RMuint32 layer;

} MPEGAUDIO_PARAMETERS;

typedef struct tagMPEGVIDEO_PARAMETERS
{
	RMuint32 isMPEG4;				// 0=mpeg1/2, 1=mpeg4
	RMuint32 vopTimeIncrRes;		// vop_time_increment_resolution in the mpeg-4 VOL header
	RMuint32 videoTimeScale;		// video cts timescale
	RMuint32 audioTimeScale;		// audio cts timescale	

	RMuint32 fixedVOPRate;			// 1 = force fixed vop rate (avi files)
	RMuint32 vopTimeIncr;			// fixed vop increment

} MPEGVIDEO_PARAMETERS;

class IMpegDecoder
{
public:
	virtual MPEG_DECODER_ERROR Init ( bool fInit=true ) = 0;
	virtual MPEG_DECODER_ERROR GetMaxDisplayModes (RMint32 *pMode) = 0;
	virtual MPEG_DECODER_ERROR SetupDisplay (RMint32 Mode, RMint32 *Output=0) = 0;
	virtual MPEG_DECODER_ERROR GetScreenDimensions (RMint32 *w, RMint32 *h, RMint32 *n, RMint32 *d) = 0;
	virtual MPEG_DECODER_ERROR DisplayYUV420 (RMuint8 *pY, RMuint8 *pUV, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMint32 w_screen, RMint32 h_screen) = 0;
	virtual MPEG_DECODER_ERROR SetVideoDestination (RMint32 x, RMint32 y, RMint32 w, RMint32 h) = 0;
	virtual MPEG_DECODER_ERROR RenderText (RMint8 *text, RMuint32 colour, RMint32 x, RMint32 y, RMint32 w, RMint32 h) = 0;
	virtual MPEG_DECODER_ERROR RenderOsdPalette (RMuint8 *palette) = 0;
	virtual MPEG_DECODER_ERROR RenderOsdBits (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h) = 0;
	virtual MPEG_DECODER_ERROR SaveOsdBits (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h) = 0;
	virtual MPEG_DECODER_ERROR Play () = 0;
	virtual MPEG_DECODER_ERROR PlayIFrame () = 0;
	virtual MPEG_DECODER_ERROR PlayIFrame (RMint32 speed) = 0;
	virtual MPEG_DECODER_ERROR Pause () = 0;
	virtual MPEG_DECODER_ERROR Stop () = 0;
	virtual MPEG_DECODER_ERROR GetSTC (RMint64 *pSTC, RMuint32 Resolution) = 0;
	virtual MPEG_DECODER_ERROR EndOfStream (RMint64 time, RMuint32 Resolution) = 0;
	virtual MPEG_DECODER_ERROR SetPCMParameters (PCM_PARAMETERS *pPcmParameters) = 0;
	virtual MPEG_DECODER_ERROR SetAC3Parameters (AC3_PARAMETERS *pAc3Parameters) = 0;
	virtual MPEG_DECODER_ERROR SetDTSParameters (DTS_PARAMETERS *pDtsParameters) = 0;
	virtual MPEG_DECODER_ERROR SetMpegAudioParameters (MPEGAUDIO_PARAMETERS *pMpegAudioParameters) = 0;
	virtual MPEG_DECODER_ERROR SetMpegVideoParameters (MPEGVIDEO_PARAMETERS *pMpegVideoParameters) = 0;
	virtual MPEG_DECODER_ERROR WriteAC3 (RMuint8 *pAc3, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts, RMuint32 firstaccessunitpointer) = 0;
	virtual MPEG_DECODER_ERROR WriteDTS (RMuint8 *pDts, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts, RMint32 nFrameHeaders, RMint32 FirstAccessUnitPointer) = 0;
	virtual MPEG_DECODER_ERROR WriteMpegAudio (RMuint8 *pAudio, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts) = 0;
	virtual MPEG_DECODER_ERROR WriteMpegVideo (RMuint8 *pVideo, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts) = 0;
	virtual MPEG_DECODER_ERROR WritePCM (RMuint8 *pPCM, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts) = 0;
	virtual MPEG_DECODER_ERROR Brightness (RMint32 direction, RMint32 *pvalue) = 0;
	virtual MPEG_DECODER_ERROR Contrast (RMint32 direction, RMint32 *pvalue) = 0;
	virtual MPEG_DECODER_ERROR Saturation (RMint32 direction, RMint32 *pvalue) = 0;
	virtual MPEG_DECODER_ERROR ClearScreen () = 0;
	virtual MPEG_DECODER_ERROR Exit () = 0;
};

#endif
