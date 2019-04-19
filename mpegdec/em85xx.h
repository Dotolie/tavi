//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : em85xx header file  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/mpegdec/em85xx.h,v $
// $Revision: 1.3 $
// $Date: 2006/04/05 02:10:41 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: em85xx.h,v $
// Revision 1.3  2006/04/05 02:10:41  zstein
// - rgb2y/u/v are declared external functions.
//
// Revision 1.2  2006/03/27 11:43:31  zstein
// - rgb2y, rgb2u, rgb2v are defined external.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.32  2006/01/21 04:44:48  ywkim
// Change type of m_pESA
//
// Revision 1.31  2006/01/09 08:32:44  etnlwind
// Update for Video Saturation Control
//
// Revision 1.30  2006/01/04 08:17:42  etnlwind
// Update for Osd Block Copy & Paste
//
// Revision 1.29  2005/12/30 04:26:13  etnlwind
// Update for New Font Adaption
//
// Revision 1.28  2005/12/22 05:14:41  etnlwind
// Update for New Version
//
// Revision 1.27  2005/12/20 11:19:11  ywkim
// Change type of *m_pESA to long from RMuin32 for negative value
//
// Revision 1.26  2005/12/13 08:59:49  etnlwind
// Update for Brightness & Contrast
//
// Revision 1.25  2005/11/21 02:51:51  etnlwind
// Update for EQ Select UI
//
// Revision 1.24  2005/11/08 01:26:45  zstein
// - 레벨리터 관련 함수 변경
//
// Revision 1.23  2005/10/31 11:45:07  ywkim
// Add function of XEN_ClearSpectrumData()
//
// Revision 1.22  2005/10/25 06:20:42  ywkim
// Add function of SetAlpha() for shutdown
//
// Revision 1.21  2005/10/19 09:48:17  etnlwind
// Update for Startup Progress Display
//
// Revision 1.20  2005/10/19 02:01:20  ywkim
// Add new function for 3D sound
//
// Revision 1.19  2005/08/22 01:01:38  ywkim
// change SetupDisplay() for HDcomponent
//
// Revision 1.18  2005/08/17 03:39:45  ywkim
// change parameter type of RenderLevelMeter
//
// Revision 1.17  2005/08/08 11:38:03  ywkim
// Change SetupDisplay()
//
// Revision 1.16  2005/07/26 01:31:04  etnlwind
// Update for
//
// 1. BDF font changed;
//    16pix font -> 16pix bold font, 12pix font -> English Alphabet Size Increased
//
// 2. AVI playback routine has changed by external source
//
// Revision 1.15  2005/07/21 06:53:31  ywkim
// add RenderLevelMeter() for 3D spectrum
//
// Revision 1.14  2005/07/13 06:02:31  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.13  2005/05/12 09:40:22  ywkim
// Add function of SetOutDisplayOption
//
// Revision 1.12  2005/05/11 08:03:47  etnlwind
// Update for MPEG4 and Divx3 low profile Codec Related
//
// Revision 1.11  2005/05/03 09:00:04  etnlwind
// Update for ES4
//
// Revision 1.10  2005/04/28 12:19:15  etnlwind
// Update for FATFS/PBI Selection
// If you want to change File System Driver, Change tavi/include/define_fs.h
//
// Revision 1.9  2005/04/18 10:20:21  etnlwind
// update for ES4
//
// Revision 1.8  2005/03/04 19:38:28  etnlwind
// VP repeat option
//
// Revision 1.7  2005/02/25 13:39:01  etnlwind
// FileManager Add
//
// Revision 1.6  2005/02/16 16:58:06  zstein
// Added GetOSDFrameBuffer() by Lee Seok
//
// Revision 1.5  2005/02/16 16:51:40  zstein
// change MpegDecoder->Init() to MpegDecoder->Init( bool fInit=ture )
//
// Revision 1.4  2005/02/16 05:51:35  ywkim
// Insert function of setosddestination
//
//*****************************************************************************

#ifndef _EM85XX_H
#define _EM85XX_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <memory.h>
#include "rmbasic.h"
#include "mpegdec.h"
#include "em85xx.h"
#include "realmagichwl.h"
#include "jasperlib.h"
#include "../include/tavi_palette.h"
#include "../3Dsound/3Dsound.h"
#include "../Task/tavi_global.h"
////////////////////////////////////////////////////////////////////////////
// Additional NML-TAVI Methods by Sang-jin Ryu
// System Color Definition
typedef struct {
	int	x;
	int	y;
	int	w;
	int	h;
} Output_t;

extern RMuint8 rgb2y (RMuint8 r, RMuint8 g, RMuint8 b);
extern RMuint8 rgb2u (RMuint8 r, RMuint8 g, RMuint8 b);
extern RMuint8 rgb2v (RMuint8 r, RMuint8 g, RMuint8 b);


extern RMuint8 rgb2y (RMuint8 r, RMuint8 g, RMuint8 b);
extern RMuint8 rgb2u (RMuint8 r, RMuint8 g, RMuint8 b);
extern RMuint8 rgb2v (RMuint8 r, RMuint8 g, RMuint8 b);


class MpegDecoder : public IMpegDecoder
{
	OSDBuffer	m_OsdBuffer;
	RMint32		m_screenWidth;
	RMint32		m_screenHeight;
	RMint32		m_numeratorPixelAspectRatio;
	RMint32		m_denominatorPixelAspectRatio;
	RMint32		m_yuvWidth;
	RMint32		m_yuvHeight;
	RMint32		m_osdWidth;
	RMint32		m_osdHeight;
	RMint32		m_isMPEG4;

public:
	MpegDecoder ();
	virtual ~MpegDecoder ();

	RMuint32	m_handle;
	RMuint32 *m_pTaviSysParam;
	MPEG_DECODER_ERROR Init ( bool fInit=true );
	MPEG_DECODER_ERROR GetMaxDisplayModes (RMint32 *pMode);
	MPEG_DECODER_ERROR SetupDisplay (RMint32 Mode, RMint32 *Output=NULL);
	MPEG_DECODER_ERROR GetScreenDimensions (RMint32 *w, RMint32 *h, RMint32 *n, RMint32 *d);
	MPEG_DECODER_ERROR DisplayYUV420 (RMuint8 *pY, RMuint8 *pUV, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMint32 w_screen, RMint32 h_screen);
	MPEG_DECODER_ERROR SetVideoDestination (RMint32 x, RMint32 y, RMint32 w, RMint32 h);
	MPEG_DECODER_ERROR RenderText (RMint8 *text, RMuint32 colour, RMint32 x, RMint32 y, RMint32 w, RMint32 h);
	MPEG_DECODER_ERROR RenderOsdBits (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h);
	MPEG_DECODER_ERROR SaveOsdBits (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h);
	MPEG_DECODER_ERROR Play ();
	MPEG_DECODER_ERROR PlayIFrame ();
	MPEG_DECODER_ERROR PlayIFrame (RMint32 speed);
	MPEG_DECODER_ERROR Pause ();
	MPEG_DECODER_ERROR Stop ();
	MPEG_DECODER_ERROR GetSTC (RMint64 *pSTC, RMuint32 Resolution);
	MPEG_DECODER_ERROR EndOfStream (RMint64 STC, RMuint32 Resolution);
	MPEG_DECODER_ERROR SetPCMParameters (PCM_PARAMETERS *pPcmParameters);
	MPEG_DECODER_ERROR SetAC3Parameters (AC3_PARAMETERS *pAc3Parameters);
	MPEG_DECODER_ERROR SetDTSParameters (DTS_PARAMETERS *pDtsParameters);
	MPEG_DECODER_ERROR SetMpegAudioParameters (MPEGAUDIO_PARAMETERS *pMpegAudioParameters);
	MPEG_DECODER_ERROR SetMpegVideoParameters (MPEGVIDEO_PARAMETERS *pMpegVideoParameters);
	MPEG_DECODER_ERROR WritePCM (RMuint8 *pPCM, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts);
//	MPEG_DECODER_ERROR WriteAC3 (RMuint8 *pAc3, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts);
	MPEG_DECODER_ERROR WriteAC3 (RMuint8 *pAc3, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts, RMuint32 firstaccessunitpointer = 0);
	MPEG_DECODER_ERROR WriteDTS (RMuint8 *pDts, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts, RMint32 nFrameHeaders, RMint32 FirstAccessUnitPointer);
	MPEG_DECODER_ERROR WriteMpegAudio (RMuint8 *pAudio, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts);
	MPEG_DECODER_ERROR WriteMpegVideo (RMuint8 *pVideo, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts);
	MPEG_DECODER_ERROR Brightness (RMint32 direction, RMint32 *pvalue);
	MPEG_DECODER_ERROR Contrast (RMint32 direction, RMint32 *pvalue);
	MPEG_DECODER_ERROR Saturation (RMint32 direction, RMint32 *pvalue);
	MPEG_DECODER_ERROR ClearScreen();
	MPEG_DECODER_ERROR Exit ();

	////////////////////////////////////////////////////////////////////////////
	// Additional TAVI Methods by Sang-jin Ryu
	MPEG_DECODER_ERROR TAVI_InitSystemPalette();
	MPEG_DECODER_ERROR TAVI_ShowPaletteInfo();
	MPEG_DECODER_ERROR RenderOsdPalette (RMuint8 *palette);
	MPEG_DECODER_ERROR RenderOsdBits_WithoutRefresh (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h);
	MPEG_DECODER_ERROR RenderOsdBits_WithoutRefresh_WithFixedBackgroundColor (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMuint8 FixedBackgroundColor);
	MPEG_DECODER_ERROR RenderOsdBits_WithoutRefresh_SkipTransparency (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h);
	MPEG_DECODER_ERROR RenderOsdBits_WithoutRefresh_SkipTransparency_WithVariableFontColor (RMuint8 *osd, RMuint8 color, RMint32 x, RMint32 y, RMint32 w, RMint32 h );
	MPEG_DECODER_ERROR RefreshOsd ();
	MPEG_DECODER_ERROR RenderOsdBox(RMuint8 palette, RMint32 x, RMint32 y, RMint32 w, RMint32 h );
	MPEG_DECODER_ERROR RenderOsdBox_WithoutRefresh(RMuint8 palette, RMint32 x, RMint32 y, RMint32 w, RMint32 h );
	MPEG_DECODER_ERROR ClearAreaOSD_WithoutRefresh( int x, int y, int w, int h );
	MPEG_DECODER_ERROR RenderText_WithoutRefresh(RMint8 *text, RMuint32 colour, RMint32 x, RMint32 y, RMint32 w, RMint32 h);
	MPEG_DECODER_ERROR SeekStop ();
	MPEG_DECODER_ERROR Reset ();
	RMuint8 GetPixelTransparent( int x, int y );
	MPEG_DECODER_ERROR OsdFadeIn();
	MPEG_DECODER_ERROR SetAlpha ( RMuint8 color, RMuint8 alpha );
	MPEG_DECODER_ERROR OsdBlockFadeOut( int sx, int sy, int width, int height, int BASE_PALLETTE_INCLUDED_IMAGE_INDEX );
	MPEG_DECODER_ERROR  OsdBlockSlideDown( int x, int y, int w, int h  );
	MPEG_DECODER_ERROR SetToggleDisplay( int option );
	RMuint8 GetPixelColor( int x, int y );
	void SetPixel_SkipTransparency( int color, int x, int y );
	MPEG_DECODER_ERROR ClearBG();
	MPEG_DECODER_ERROR SetBrightness(RMint32 direction, RMint32 *pvalue);
	MPEG_DECODER_ERROR SetContrast(RMint32 direction, RMint32 *pvalue);
	MPEG_DECODER_ERROR SetSaturation(RMint32 direction, RMint32 *pvalue);
	MPEG_DECODER_ERROR SetSTC (RMint64 Stc, RMuint32 timeResolution);
	MPEG_DECODER_ERROR Resume_ColorTemperature( char brightness, char contrast, char saturation=32);
	MPEG_DECODER_ERROR OsdBlockCopy( RMuint8* pB, int sx, int sy, int w, int h );
	MPEG_DECODER_ERROR OsdBlockPaste( int sx, int sy, int w, int h, RMuint8* pB );
	
		
	////////////////////////////////////////////////////////////////////////////
	// Additional TAVI Methods by Lee Seok 
	MPEG_DECODER_ERROR ClearAreaOSD( int x, int y, int w, int h );
	void SetPixel( int color, int x, int y );
	void Refresh();
	OSDBuffer GetOSDFrameBuffer() { return m_OsdBuffer; };
	////////////////////////////////////////////////////////////////////////////
	// Additional TAVI Methods by who
	MPEG_DECODER_ERROR SetOSDDestination (RMint32 x, RMint32 y, RMint32 w, RMint32 h);
	MPEG_DECODER_ERROR SetOutDisplayOption( int option );
	////////////////////////////////////////////////////////////////////////////
	// Additional TAVI Methods for XEN_LIB
	long **m_pESA;
	MPEG_DECODER_ERROR XEN_RenderLevelMeter(RMuint8 palette, RMuint8 bg, RMint32 x, RMint32 y );
	MPEG_DECODER_ERROR XEN_GetSpectrumData(void);
	MPEG_DECODER_ERROR XEN_ClearSpectrumData(void);
	MPEG_DECODER_ERROR XEN_SetContentsProp( int SampleRate );
	int XEN_Get_UserEQ_BandValue( int BandName );
	MPEG_DECODER_ERROR XEN_Preset( SOUNDEFFECT_t Mode );
	MPEG_DECODER_ERROR XEN_SetEffect( CMD_t CMD );
	MPEG_DECODER_ERROR XEN_SetPower( int nValue );

};

#endif
