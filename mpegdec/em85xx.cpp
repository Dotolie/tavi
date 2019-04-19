//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : em85xx code file  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/mpegdec/em85xx.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/03/27 11:43:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: em85xx.cpp,v $
// Revision 1.3  2006/03/27 11:43:31  zstein
// - rgb2y, rgb2u, rgb2v are defined external.
//
// Revision 1.2  2006/03/07 02:08:47  ywkim
// Disable orange lcd & Enable samsung lcd
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.63  2006/02/18 05:47:38  ywkim
// Fix mis-assigned parameter of  evSaturation in SetSaturation()
//
// Revision 1.62  2006/01/26 12:00:32  ywkim
// Change paramter of LCD for orange LCD
//
// Revision 1.61  2006/01/21 04:44:48  ywkim
// Change type of m_pESA
//
// Revision 1.60  2006/01/09 08:32:44  etnlwind
// Update for Video Saturation Control
//
// Revision 1.59  2006/01/04 08:17:42  etnlwind
// Update for Osd Block Copy & Paste
//
// Revision 1.58  2005/12/30 04:26:13  etnlwind
// Update for New Font Adaption
//
// Revision 1.57  2005/12/22 05:14:41  etnlwind
// Update for New Version
//
// Revision 1.56  2005/12/20 11:19:11  ywkim
// Change type of *m_pESA to long from RMuin32 for negative value
//
// Revision 1.55  2005/12/14 02:15:09  etnlwind
// Update for Resume_ColorTemperature()
//
// Revision 1.54  2005/12/13 08:59:49  etnlwind
// Update for Brightness & Contrast
//
// Revision 1.53  2005/11/21 02:51:51  etnlwind
// Update for EQ Select UI
//
// Revision 1.52  2005/11/17 11:22:22  zstein
// *** empty log message ***
//
// Revision 1.51  2005/11/17 04:24:04  zstein
// - level meter
//
// Revision 1.50  2005/11/08 01:26:14  zstein
// 레벨미터 모양 변경
//
// Revision 1.49  2005/10/31 11:45:07  ywkim
// Add function of XEN_ClearSpectrumData()
//
// Revision 1.48  2005/10/25 06:20:42  ywkim
// Add function of SetAlpha() for shutdown
//
// Revision 1.47  2005/10/21 10:29:59  etnlwind
// Update for TextViwer Background Process
//
// Revision 1.46  2005/10/21 09:43:56  etnlwind
// Update for TextViwer Scroll, Booting Progress
//
// Revision 1.45  2005/10/19 09:48:17  etnlwind
// Update for Startup Progress Display
//
// Revision 1.44  2005/10/19 02:01:20  ywkim
// Add new function for 3D sound
//
// Revision 1.43  2005/08/30 04:34:12  ywkim
// change mesg output for debug
//
// Revision 1.42  2005/08/22 01:01:38  ywkim
// change SetupDisplay() for HDcomponent
//
// Revision 1.41  2005/08/17 03:39:45  ywkim
// change parameter type of RenderLevelMeter
//
// Revision 1.40  2005/08/09 10:52:09  etnlwind
// Update for
// 1. CSoundPaletteDbx Added
// 2. Video Control Pannel UI changed
//
// Revision 1.39  2005/08/08 11:38:03  ywkim
// Change SetupDisplay()
//
// Revision 1.38  2005/07/26 01:31:04  etnlwind
// Update for
//
// 1. BDF font changed;
//    16pix font -> 16pix bold font, 12pix font -> English Alphabet Size Increased
//
// 2. AVI playback routine has changed by external source
//
// Revision 1.37  2005/07/21 06:53:31  ywkim
// add RenderLevelMeter() for 3D spectrum
//
// Revision 1.36  2005/07/13 13:41:35  etnlwind
// Update for Brightness And Contrast DialogBox
//
// Revision 1.35  2005/07/13 06:02:31  etnlwind
// Update for LibSubMenu Bug Fix And VideoPlayer Brightness-Contrast Dlg Added
//
// Revision 1.34  2005/06/24 07:04:32  ywkim
// Remark SetToggleDisplay
//
// Revision 1.33  2005/06/20 12:03:56  ywkim
// Change DVI parameter VB for samsung lcd
//
// Revision 1.32  2005/06/16 02:07:23  ywkim
// Remove unwanted routine
//
// Revision 1.31  2005/06/14 07:26:37  ywkim
// change valid window coordinate
//
// Revision 1.30  2005/05/17 19:12:50  ywkim
// Change dviparameter
//
// Revision 1.29  2005/05/17 13:53:57  ywkim
// modify dviparameter
//
// Revision 1.28  2005/05/17 10:44:50  ywkim
// Change DVIParameters for samsung lcd
//
// Revision 1.27  2005/05/15 12:17:52  ywkim
// Change lcd destination 240 to 244
//
// Revision 1.26  2005/05/12 09:40:22  ywkim
// Add function of SetOutDisplayOption
//
// Revision 1.25  2005/05/12 04:09:50  etnlwind
// Update for ES4
//
// Revision 1.24  2005/05/11 08:03:47  etnlwind
// Update for MPEG4 and Divx3 low profile Codec Related
//
// Revision 1.23  2005/05/04 12:52:51  etnlwind
// Update for MessageBox Related
//
// Revision 1.22  2005/05/03 09:00:04  etnlwind
// Update for ES4
//
// Revision 1.21  2005/04/28 12:19:15  etnlwind
// Update for FATFS/PBI Selection
// If you want to change File System Driver, Change tavi/include/define_fs.h
//
// Revision 1.20  2005/04/28 05:59:59  zstein
// demo merge.
//
// Revision 1.19  2005/04/18 10:20:21  etnlwind
// update for ES4
//
// Revision 1.18  2005/03/06 13:25:24  ywkim
// Modify parameter of osd for TV out
//
// Revision 1.17  2005/03/06 12:36:38  etnlwind
// *** empty log message ***
//
// Revision 1.16  2005/03/06 08:39:22  etnlwind
// Volume, ControlPannel
//
// Revision 1.15  2005/03/06 00:07:20  etnlwind
// *** empty log message ***
//
// Revision 1.14  2005/03/05 19:29:20  etnlwind
// touchpad 관련 수정
//
// Revision 1.13  2005/03/05 12:40:01  ywkim
// change SetupDisplay for only samsung lcd
//
// Revision 1.12  2005/03/04 20:00:20  etnlwind
// error fix
//
// Revision 1.11  2005/03/04 19:38:28  etnlwind
// VP repeat option
//
// Revision 1.10  2005/03/03 02:47:25  ywkim
// modify function of SetupDisplay for TVout(PAL,HD...)
//
// Revision 1.9  2005/03/02 13:46:58  zstein
// *** empty log message ***
//
// Revision 1.8  2005/02/25 13:39:01  etnlwind
// FileManager Add
//
// Revision 1.7  2005/02/16 16:58:06  zstein
// Added GetOSDFrameBuffer() by Lee Seok
//
// Revision 1.6  2005/02/16 16:51:40  zstein
// change MpegDecoder->Init() to MpegDecoder->Init( bool fInit=ture )
//
// Revision 1.5  2005/02/16 05:55:35  ywkim
// Insert function of setosddestination
//
//*****************************************************************************

#include "em85xx.h"
#include "realmagichwl_userland_api.h"
#include "stdlib.h"

#ifndef U8
	typedef unsigned char U8;
#endif

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

#define MAX_FEEDPACKETS	8192
static feedpacket FeedPacketsArray[MAX_FEEDPACKETS];

//////////////////////
typedef RMuint16	MWIMAGEBITS;

// builtin C-based proportional/fixed font structure
typedef struct 
{
	RMint8		*name;		// font name
	RMint32		maxwidth;	// max width in pixels
	RMint32		height;		// height in pixels
	RMint32		ascent;		// ascent (baseline) height
	RMint32		firstchar;	// first character in bitmap
	RMint32		size;		// font size in characters
	RMuint16	*bits;		// 16-bit right-padded bitmap data
	RMuint16	*offset;	// 256 offsets into bitmap data
	RMuint8		*width;		// 256 character widths or 0 if fixed
} MWCFONT, *PMWCFONT;

typedef struct _tagsdPOINT
{
	RMuint32 x;
	RMuint32 y;
	RMuint32 x2;
	RMuint32 y2;
} sdPOINT;

#include "myfont.c"
static RMuint8 pixel_value = 1;

static void SetPixel (RMuint32 x, RMuint32 y, RMuint32 colour, RMuint8 *pOsd, RMuint32 w, RMuint32 h)
{	// Original
	/*
	if (pixel_value == 3)
		pOsd[(y * w) + x] = 1;
	else
		pOsd[(y * w) + x] = pixel_value;
	*/

	pOsd[(y * w) + x] = colour;
	
}

static void ClearPixel (RMuint32 x, RMuint32 y, RMuint8 *pOsd, RMuint32 w, RMuint32 h)
{
	// Original
	/*
	if (pixel_value == 3)
		pOsd[(y * w) + x] = 3;
	else
		pOsd[(y * w) + x] = 0;
	*/

	pOsd[(y * w) + x] = _TRANSPARENT;
}

static void DrawCharacter (PMWCFONT pFont, RMint8 ch, RMuint32 colour, RMuint32 xOff, RMuint32 yOff, RMuint8 *pOsd, RMuint32 w, RMuint32 h)
{
	RMuint16 *bits;
	bits = pFont->bits + pFont->offset[ch - pFont->firstchar];
	RMuint32 width, height, y, x;

	height = pFont->height;
	width = (pFont->width) ?
		pFont->width[ch - pFont->firstchar] :
		pFont->maxwidth;
	for (y = 0; y < height; y++)
	{
		RMuint16 row = bits[y], mask;
		for (mask = 0x8000, x = 0; x < width; (mask >>= 1), x++)
		{
			if (row & mask)
				SetPixel (x + xOff, y + yOff, colour, pOsd, w, h);
			#if 0
			else
				ClearPixel (x + xOff, y + yOff, pOsd, w, h);
			#endif
		}
	}
}

static RMuint32 mystrlen (PMWCFONT pFont, RMint8 *text)
{
	RMuint32 length = 0;
	if (text)
	{
		while ((*text != 0) && (*text != ' '))
		{
			length += (pFont->width) ?
				pFont->width[*text - pFont->firstchar] :
				pFont->maxwidth;
			text++;
		}
	}
	return length;
}

static void DrawText (PMWCFONT pFont, RMint8 *text, sdPOINT *pLoc, RMuint32 color, RMuint8 *pOsd, RMuint32 w, RMuint32 h)
{
	RMuint32 xOffset = pLoc->x, width, wordlen, spacelen;
	RMint8 *str = text;

	spacelen = (pFont->width) ?
		pFont->width[' ' - pFont->firstchar] :
		pFont->maxwidth;

	while (*str != 0)
	{
		if (*str == ' ')
		{
			wordlen = mystrlen (pFont, str+1) + spacelen;
			if (wordlen <= (pLoc->x2 - pLoc->x))
			{
				if ((xOffset + wordlen) > pLoc->x2)
				{
					str++;
					xOffset = pLoc->x;
					pLoc->y += pFont->height;
					if ((pLoc->y + pFont->height) > pLoc->y2)
						break;
				}
			}
		}

		if ((*str < pFont->firstchar) ||
		   (*str >= (pFont->firstchar + pFont->size)))
			*str = 0x3f; // '?'
		DrawCharacter (pFont, *str, color, xOffset, pLoc->y, pOsd, w, h);
		xOffset += (pFont->width) ?
		           pFont->width[*str - pFont->firstchar] :
		           pFont->maxwidth;
		str++;
		if (xOffset > pLoc->x2)
		{
			xOffset = pLoc->x;
			pLoc->y += pFont->height;
			if ((pLoc->y + pFont->height) > pLoc->y2)
				break;
		}
	}
}
//////////////////////

// rgb 2 yuv:
// Y  =  0.257r + 0.504g + 0.098b + 16
// Cb = -0.148r - 0.291g + 0.439b + 128
// Cr =  0.439r + 0.368g + 0.071b + 128
RMuint8 rgb2y (RMuint8 r, RMuint8 g, RMuint8 b)
{
	RMint32 f = 257*(RMint32)r + 504*(RMint32)g + 98*(RMint32)b + 16000;
	if (f > 255000)
		f = 255000;
	f = f / 1000;
	return (RMuint8)f;
}
RMuint8 rgb2u (RMuint8 r, RMuint8 g, RMuint8 b)
{
	RMint32 f = -148*(RMint32)r - 291*(RMint32)g + 439*(RMint32)b + 128000;
	if (f > 255000)
		f = 255000;
	if (f < 0)
		f = 0;
	f = f / 1000;
	return (RMuint8)f;
}
RMuint8 rgb2v (RMuint8 r, RMuint8 g, RMuint8 b)
{
	RMint32 f = 439*(RMint32)r - 368*(RMint32)g - 71*(RMint32)b + 128000;
	if (f > 255000)
		f = 255000;
	if (f < 0)
		f = 0;
	f = f / 1000;
	return (RMuint8)f;
}

MpegDecoder::MpegDecoder ()
{
}

MpegDecoder::~MpegDecoder ()
{
}

MPEG_DECODER_ERROR MpegDecoder::Init ( bool fInit )
{
	DEBUGMSG (1, ("MpegDecoder::Init\n"));
	m_handle = open ("/dev/realmagichwl0", O_RDONLY);
	ASSERT (m_handle != -1);
	if (m_handle == -1)
		return MPEG_DECODER_ERROR_NO_DECODER;

	m_yuvWidth = 0;
	m_yuvHeight = 0;
	m_osdWidth = 0;
	m_osdHeight = 0;
	m_isMPEG4 = 0;

	FeedPacketQueuesInit (FeedPacketsArray, MAX_FEEDPACKETS);

	if( fInit ) {
		// make the osd buffer is 320x240
		RMuint32 i;
		RMuint8 *pOSDHeader, *pPalette, *pBits;
		m_osdWidth = 320;
		m_osdHeight = 240;
		asm ("mcr p15, 0, r0, c7, c10, 0\n");	
		ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, &m_OsdBuffer);
		ASSERT (m_OsdBuffer.framebuffer);
		pOSDHeader = (RMuint8 *)(m_OsdBuffer.framebuffer);
		pPalette = pOSDHeader + 8;
		pBits = pPalette + 1024;
		pOSDHeader[0] = 0x3e;
		pOSDHeader[1] = ((m_osdWidth*m_osdHeight+1024+8) >> 16) & 0xff;
		pOSDHeader[2] = ((m_osdWidth*m_osdHeight+1024+8) >>  8) & 0xff;
		pOSDHeader[3] = ((m_osdWidth*m_osdHeight+1024+8) >>  0) & 0xff;
		pOSDHeader[4] = (m_osdWidth >> 8) & 0xff;
		pOSDHeader[5] = (m_osdWidth >> 0) & 0xff;
		pOSDHeader[6] = (m_osdHeight >> 8) & 0xff;
		pOSDHeader[7] = (m_osdHeight >> 0) & 0xff;
		// init palette
		for (i=0; i<256; i++)
		{
			pPalette[0] = 0x00;				// alpha (0x00 = transparent, 0xff = opaque)
			if (i)
				pPalette[0] = 0xff;
			pPalette[1] = rgb2y (0, 0, 0);	// y
			pPalette[2] = rgb2u (0, 0, 0);	// u
			pPalette[3] = rgb2v (0, 0, 0);	// v
			pPalette += 4;
		}
		// some pre-defined colours
		pPalette = pOSDHeader + 8;
		pPalette += 4;
		pPalette[0] = 0xff;
		pPalette[1] = rgb2y (0, 0, 0);			// y
		pPalette[2] = rgb2u (0, 0, 0);			// u
		pPalette[3] = rgb2v (0, 0, 0);			// v
		pPalette += 4;
		pPalette[0] = 0xff;
		pPalette[1] = rgb2y (0xff, 0xff, 0);	// y
		pPalette[2] = rgb2u (0xff, 0xff, 0);	// u
		pPalette[3] = rgb2v (0xff, 0xff, 0);	// v
		pPalette += 4;
		pPalette[0] = 0x40;
		pPalette[1] = rgb2y (0xff, 0xff, 0xff);		// y
		pPalette[2] = rgb2u (0xff, 0xff, 0xff);		// u
		pPalette[3] = rgb2v (0xff, 0xff, 0xff);		// v

		for (i=0; i<m_osdWidth*m_osdHeight;i++)
			pBits[i] = 0;
#if XEN_LIB		
		XEN_GetSpectrumData();		// XEN_LIB
#endif // XEN_LIB
		/* 
		// Enable this if you do NOT want to wait for an I frame before
		// the decoding AFTER a STOP
		RMuint32 BroadcastVideo;
		BroadcastVideo = 1;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evBroadcastedVideo, sizeof(BroadcastVideo), &BroadcastVideo);
		*/	
	}
	
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::GetMaxDisplayModes (RMint32 *pMode)
{
	ASSERT (pMode);
	*pMode = 2;
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetupDisplay (RMint32 Mode , RMint32 *Output )
{
	ASSERT (m_handle != -1);
	Wnd_type Wnd;
	evDigOvOnlyParams_type	DVIParameters;

	evOutputDevice_type		OutputDevice;
	evTvStandard_type		TvStandard;
	evTvOutputFormat_type		TvOutputFormat;
	evDigOvOnlyParams_type	DigitalVideoParams;
	Output_t	*pOutput;
	
	switch (Mode)
	{
	case 0:
		// go to digital output	
		Wnd.x = 0;
		Wnd.y = 0;
		Wnd.w = 320;
		Wnd.h = 240;
		m_numeratorPixelAspectRatio = 675;
		m_denominatorPixelAspectRatio = 639;
		TvStandard = evTvStandard_NTSC;
		TvOutputFormat = evTvOutputFormat_OUTPUT_OFF;
		OutputDevice = evOutputDevice_DigOvOnly;

		//  HSyncTotal = PreHSync + HSyncActive + PostHSync + VideoWidth;
		//  VSyncTotal = PreVSync + VSyncActive + PostVSync + VideoHeight;
		//  HFreq = VFreq * VSyncTotal / (Interlaced ? 2:1);
		//  PixelFreq = HFreq * HSyncTotal * ((nbits==8) ? 2:1);
#if 1
//		DVIParameters.HFreq = 18328; //27859;
		DVIParameters.HFreq = 18775; //27859;
		DVIParameters.VFreq = 7480; //11371;

		DVIParameters.VideoWidth = 320;
		DVIParameters.VideoHeight = 240;	

		DVIParameters.PreHSync = 1;
		DVIParameters.HSyncActive = 19;
		DVIParameters.PostHSync = 1;

		DVIParameters.PreVSync = 4;
		DVIParameters.VSyncActive = 5;
		DVIParameters.PostVSync =  2;
#else	// for orange lcd
		DVIParameters.HFreq = 19822; //27859;
		DVIParameters.VFreq = 7480; //11371;

		DVIParameters.VideoWidth = 320;
		DVIParameters.VideoHeight = 240;	

		DVIParameters.PreHSync = 10;
		DVIParameters.HSyncActive = 39;
		DVIParameters.PostHSync = 29;

		DVIParameters.PreVSync = 6;
		DVIParameters.VSyncActive = 10;
		DVIParameters.PostVSync =  9;
#endif
		DVIParameters.HSyncTotal = 
			DVIParameters.VideoWidth +
			DVIParameters.PreHSync +
			DVIParameters.HSyncActive +
			DVIParameters.PostHSync;
		DVIParameters.VSyncTotal =
			DVIParameters.VideoHeight +
			DVIParameters.PreVSync +
			DVIParameters.VSyncActive +
			DVIParameters.PostVSync;

		DVIParameters.PixelFreq = 0;
		DVIParameters.Interlaced = 0;
		DVIParameters.HSyncPolarity = 0;
		DVIParameters.VSyncPolarity = 0;

		DVIParameters.BitsPerClock = 24;
		DVIParameters.Ccir = CCIR_601;
		DVIParameters.InvertField = CCIR_NON_INVERT_FIELD;
		DVIParameters.SyncEnable = VSyncEn_HSyncEn_VrdyEn;
		DVIParameters.Vip20 = 1;
		DVIParameters.SyncGen = evSyncGen_em8xxx_Master;
		DVIParameters.TvHdtvStandard = evTvHdtvStandard_NTSC;	// actually doesn't matter

		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evValidWindow, 
			sizeof(Wnd), &Wnd);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDigOvOnlyParams, 
			sizeof(DVIParameters), &DVIParameters);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvOutputFormat, 
			sizeof(TvOutputFormat), &TvOutputFormat);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvStandard, 
			sizeof(TvStandard), &TvStandard);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evOutputDevice,
			sizeof(OutputDevice), &OutputDevice);
		RUA_DECODER_SET_PROPERTY (m_handle, OSD_SET, eOsdDestinationWindow,
			sizeof(Wnd), &Wnd);
		m_screenWidth = Wnd.w;
		m_screenHeight = Wnd.h;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDestinationWindow, 
			sizeof(Wnd), &Wnd);
		break;	
	case 1:
		// ntsc - composite/svideo
		TvStandard = evTvStandard_NTSC;
		TvOutputFormat = evTvOutputFormat_COMPOSITE;
		OutputDevice = evOutputDevice_TV;

		Wnd.x = 0;
		Wnd.y = 0;
		Wnd.w = 720;
		Wnd.h = 480;
		m_numeratorPixelAspectRatio = 8;
		m_denominatorPixelAspectRatio = 9;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evValidWindow,
			sizeof (Wnd), &Wnd);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvOutputFormat,
			sizeof(TvOutputFormat), &TvOutputFormat);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvStandard, 
			sizeof(TvStandard), &TvStandard);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evOutputDevice,
			sizeof(OutputDevice), &OutputDevice);

		if ( Output )
		{
			pOutput=	(Output_t *)Output;
			Wnd.x = pOutput[Mode].x;
			Wnd.y = pOutput[Mode].y;
			Wnd.w = pOutput[Mode].w;
			Wnd.h = pOutput[Mode].h;
		}		

		RUA_DECODER_SET_PROPERTY (m_handle, OSD_SET, eOsdDestinationWindow,
			sizeof(Wnd), &Wnd);
		m_screenWidth = Wnd.w;
		m_screenHeight = Wnd.h;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDestinationWindow,
			sizeof(Wnd), &Wnd);
		break;
	case 2:
		// pal - composite/svideo
		Wnd.x = 0;
		Wnd.y = 0;
		Wnd.w = 720;
		Wnd.h = 576;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evValidWindow,
			sizeof (Wnd), &Wnd);
		TvStandard = evTvStandard_PAL;
		TvOutputFormat = evTvOutputFormat_COMPOSITE;
		OutputDevice = evOutputDevice_TV;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvStandard,
			sizeof (TvStandard), &TvStandard);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvOutputFormat,
			sizeof (TvOutputFormat), &TvOutputFormat);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evOutputDevice,
			sizeof (OutputDevice), &OutputDevice);

		if ( Output )
		{
			pOutput=	(Output_t *)Output;
			Wnd.x = pOutput[Mode].x;
			Wnd.y = pOutput[Mode].y;
			Wnd.w = pOutput[Mode].w;
			Wnd.h = pOutput[Mode].h;
		}		
		RUA_DECODER_SET_PROPERTY (m_handle, OSD_SET, eOsdDestinationWindow,
			sizeof(Wnd), &Wnd);
		m_screenWidth = Wnd.w;
		m_screenHeight = Wnd.h;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDestinationWindow,
			sizeof(Wnd), &Wnd);
		
		break;
	case 3:
		// ntsc - component
		Wnd.x = 0;
		Wnd.y = 0;
		Wnd.w = 720;
		Wnd.h = 480;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evValidWindow,
			sizeof (Wnd), &Wnd);
		TvStandard = evTvStandard_NTSC;
		TvOutputFormat = evTvOutputFormat_COMPONENT_YUV;
		OutputDevice = evOutputDevice_TV;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvStandard,
			sizeof (TvStandard), &TvStandard);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvOutputFormat,
			sizeof (TvOutputFormat), &TvOutputFormat);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evOutputDevice,
			sizeof (OutputDevice), &OutputDevice);

		if ( Output )
		{
			pOutput=	(Output_t *)Output;
			Wnd.x = pOutput[Mode].x;
			Wnd.y = pOutput[Mode].y;
			Wnd.w = pOutput[Mode].w;
			Wnd.h = pOutput[Mode].h;
		}		
		RUA_DECODER_SET_PROPERTY (m_handle, OSD_SET, eOsdDestinationWindow,
			sizeof(Wnd), &Wnd);
		m_screenWidth = Wnd.w;
		m_screenHeight = Wnd.h;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDestinationWindow,
			sizeof(Wnd), &Wnd);
		
		break;
	case 4:
		// pal - component
		Wnd.x = 0;
		Wnd.y = 0;
		Wnd.w = 720;
		Wnd.h = 576;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evValidWindow,
			sizeof (Wnd), &Wnd);
		TvStandard = evTvStandard_PAL;
		TvOutputFormat = evTvOutputFormat_COMPONENT_YUV;
		OutputDevice = evOutputDevice_TV;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvStandard,
			sizeof (TvStandard), &TvStandard);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvOutputFormat,
			sizeof (TvOutputFormat), &TvOutputFormat);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evOutputDevice,
			sizeof (OutputDevice), &OutputDevice);

		if ( Output )
		{
			pOutput=	(Output_t *)Output;
			Wnd.x = pOutput[Mode].x;
			Wnd.y = pOutput[Mode].y;
			Wnd.w = pOutput[Mode].w;
			Wnd.h = pOutput[Mode].h;
		}		
	
		RUA_DECODER_SET_PROPERTY (m_handle, OSD_SET, eOsdDestinationWindow,
			sizeof(Wnd), &Wnd);
		m_screenWidth = Wnd.w;
		m_screenHeight = Wnd.h;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDestinationWindow,
			sizeof(Wnd), &Wnd);
		break;
	case 5:
		// 480p
		Wnd.x = 0;
		Wnd.y = 0;
		Wnd.w = 720;
		Wnd.h = 480;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evValidWindow,
			sizeof (Wnd), &Wnd);

		TvStandard = evTvStandard_NTSC;
		TvOutputFormat = evTvOutputFormat_COMPONENT_YUV;
		OutputDevice = evOutputDevice_DigOvOnly;		
		DigitalVideoParams.HFreq = 31469;
		DigitalVideoParams.VFreq = 5994;
		DigitalVideoParams.VideoWidth = 720;
		DigitalVideoParams.VideoHeight = 480;
		DigitalVideoParams.HSyncTotal = 858;	
		DigitalVideoParams.PreHSync = 16;
		DigitalVideoParams.HSyncActive = 62;
		DigitalVideoParams.PostHSync = 60;
		DigitalVideoParams.VSyncTotal = 525;	
		DigitalVideoParams.PreVSync = 9;
		DigitalVideoParams.VSyncActive = 6;
		DigitalVideoParams.PostVSync = 30;
		DigitalVideoParams.PixelFreq = 0;
		DigitalVideoParams.Interlaced = 0;
		DigitalVideoParams.HSyncPolarity = 0;
		DigitalVideoParams.VSyncPolarity = 0;
		DigitalVideoParams.BitsPerClock = 8;
		DigitalVideoParams.Ccir = CCIR_601;
		DigitalVideoParams.InvertField = CCIR_NON_INVERT_FIELD;
		DigitalVideoParams.SyncEnable = VSyncEn_HSyncEn_VrdyEn;
		DigitalVideoParams.Vip20 = 1;
		DigitalVideoParams.SyncGen = evSyncGen_em8xxx_Master;
		DigitalVideoParams.TvHdtvStandard = evTvHdtvStandard_480P;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDigOvOnlyParams,
			sizeof (DigitalVideoParams), &DigitalVideoParams);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvOutputFormat,
			sizeof (TvOutputFormat), &TvOutputFormat);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evOutputDevice,
			sizeof (OutputDevice), &OutputDevice);

		if ( Output )
		{
			pOutput=	(Output_t *)Output;
			Wnd.x = pOutput[Mode].x;
			Wnd.y = pOutput[Mode].y;
			Wnd.w = pOutput[Mode].w;
			Wnd.h = pOutput[Mode].h;
		}		
		RUA_DECODER_SET_PROPERTY (m_handle, OSD_SET, eOsdDestinationWindow,
			sizeof(Wnd), &Wnd);
		m_screenWidth = Wnd.w;
		m_screenHeight = Wnd.h;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDestinationWindow,
			sizeof(Wnd), &Wnd);
		
		break;
	case 6:
		// 720p
		Wnd.x = 0;
		Wnd.y = 0;
		Wnd.w = 1280;
		Wnd.h = 720;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evValidWindow,
			sizeof (Wnd), &Wnd);

		TvStandard = evTvStandard_NTSC;
		TvOutputFormat = evTvOutputFormat_COMPONENT_YUV;
		OutputDevice = evOutputDevice_DigOvOnly;		
		DigitalVideoParams.HFreq = 45000;                         
		DigitalVideoParams.VFreq = 6000;                          
		DigitalVideoParams.VideoWidth = 1280;                     
		DigitalVideoParams.VideoHeight = 720;                     
		DigitalVideoParams.HSyncTotal = 1650;	                 
		DigitalVideoParams.PreHSync = 110;                        
		DigitalVideoParams.HSyncActive = 40;                      
		DigitalVideoParams.PostHSync = 220;                       
		DigitalVideoParams.VSyncTotal = 750;	                     
		DigitalVideoParams.PreVSync = 5;                          
		DigitalVideoParams.VSyncActive = 5;                       
		DigitalVideoParams.PostVSync = 20;                        
		DigitalVideoParams.PixelFreq = 0;                         
		DigitalVideoParams.Interlaced = 0;                        
		DigitalVideoParams.HSyncPolarity = 1;                     
		DigitalVideoParams.VSyncPolarity = 1;                     
		DigitalVideoParams.BitsPerClock = 24;                     
		DigitalVideoParams.Ccir = CCIR_601;                       
		DigitalVideoParams.InvertField = CCIR_NON_INVERT_FIELD;   
		DigitalVideoParams.SyncEnable = VSyncEn_HSyncEn_VrdyEn;   
		DigitalVideoParams.Vip20 = 1;                             
		DigitalVideoParams.SyncGen = evSyncGen_em8xxx_Master;     
		DigitalVideoParams.TvHdtvStandard = evTvHdtvStandard_720P;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDigOvOnlyParams,
			sizeof (DigitalVideoParams), &DigitalVideoParams);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvOutputFormat,
			sizeof (TvOutputFormat), &TvOutputFormat);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evOutputDevice,
			sizeof (OutputDevice), &OutputDevice);

		if ( Output )
		{
			pOutput=	(Output_t *)Output;
			Wnd.x = pOutput[Mode].x;
			Wnd.y = pOutput[Mode].y;
			Wnd.w = pOutput[Mode].w;
			Wnd.h = pOutput[Mode].h;
		}		
		RUA_DECODER_SET_PROPERTY (m_handle, OSD_SET, eOsdDestinationWindow,
			sizeof(Wnd), &Wnd);
		m_screenWidth = Wnd.w;
		m_screenHeight = Wnd.h;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDestinationWindow,
			sizeof(Wnd), &Wnd);
		break;

	case 7:
		// 1080i
		Wnd.x = 0;
		Wnd.y = 0;
		Wnd.w = 1920;
		Wnd.h = 1080;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evValidWindow,
			sizeof (Wnd), &Wnd);

		TvStandard = evTvStandard_NTSC;
		TvOutputFormat = evTvOutputFormat_COMPONENT_YUV;
		OutputDevice = evOutputDevice_DigOvOnly;		
		DigitalVideoParams.HFreq = 33716;                        
		DigitalVideoParams.VFreq = 5994;                         
		DigitalVideoParams.VideoWidth = 1920;                    
		DigitalVideoParams.VideoHeight = 1080;                   
		DigitalVideoParams.HSyncTotal = 2199;	                 
		DigitalVideoParams.PreHSync = 88;                        
		DigitalVideoParams.HSyncActive = 44;                     
		DigitalVideoParams.PostHSync = 148;                      
		DigitalVideoParams.VSyncTotal = 1125;	                     
		DigitalVideoParams.PreVSync = 5;                         
		DigitalVideoParams.VSyncActive = 10;                     
		DigitalVideoParams.PostVSync = 30;                       
		DigitalVideoParams.PixelFreq = 0;                        
		DigitalVideoParams.Interlaced = 1;                       
		DigitalVideoParams.HSyncPolarity = 0;                     
		DigitalVideoParams.VSyncPolarity = 0;                     
		DigitalVideoParams.BitsPerClock = 24;                     
		DigitalVideoParams.Ccir = CCIR_601;                       
		DigitalVideoParams.InvertField = CCIR_NON_INVERT_FIELD;   
		DigitalVideoParams.SyncEnable = VSyncEn_HSyncEn_VrdyEn;   
		DigitalVideoParams.Vip20 = 1;                             
		DigitalVideoParams.SyncGen = evSyncGen_em8xxx_Master;     
		DigitalVideoParams.TvHdtvStandard = evTvHdtvStandard_1080I;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDigOvOnlyParams,
			sizeof (DigitalVideoParams), &DigitalVideoParams);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evTvOutputFormat,
			sizeof (TvOutputFormat), &TvOutputFormat);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evOutputDevice,
			sizeof (OutputDevice), &OutputDevice);

		if ( Output )
		{
			pOutput=	(Output_t *)Output;
			Wnd.x = pOutput[Mode].x;
			Wnd.y = pOutput[Mode].y;
			Wnd.w = pOutput[Mode].w;
			Wnd.h = pOutput[Mode].h;
		}		
		RUA_DECODER_SET_PROPERTY (m_handle, OSD_SET, eOsdDestinationWindow,
			sizeof(Wnd), &Wnd);
		m_screenWidth = Wnd.w;
		m_screenHeight = Wnd.h;
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evDestinationWindow,
			sizeof(Wnd), &Wnd);
		break;
	default:
		ASSERT (0);
		break;
	}

	printf("SetupDisplay ; x=%d, y=%d, w=%d, h=%d\n",Wnd.x,Wnd.y,Wnd.w,Wnd.h);

	return MPEG_DECODER_ERROR_NO_ERROR;

}

MPEG_DECODER_ERROR MpegDecoder::GetScreenDimensions (RMint32 *w, RMint32 *h, RMint32 *n, RMint32 *d)
{
	ASSERT (w);
	ASSERT (h);
	*w = m_screenWidth;
	*h = m_screenHeight;
	*n = m_numeratorPixelAspectRatio;
	*d = m_denominatorPixelAspectRatio;
	return MPEG_DECODER_ERROR_NO_ERROR;	
}

MPEG_DECODER_ERROR MpegDecoder::DisplayYUV420 (RMuint8 *pY, RMuint8 *pUV, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMint32 w_screen, RMint32 h_screen)
{
	RMint32 i, Yaddr, UVaddr, isMPEG4;
	YUVframe F;
	RMuint8 *Y = (RMuint8 *)pY;
	RMuint8 *UV = (RMuint8 *)pUV;

	if ((pY == 0) && (pUV == 0))
	{			
		int retval;
		decoderproperty Dp;
		evYUVWriteParams_type yuv; 

		ASSERT (m_yuvWidth);
		ASSERT (m_yuvHeight);

		isMPEG4 = 0;
		RUA_DECODER_SET_PROPERTY (m_handle, DECODER_SET, edecVideoStd, sizeof (isMPEG4), &isMPEG4);

		yuv.wWidth = m_yuvWidth;
		yuv.wHeight = m_yuvHeight;
		yuv.YUVFormat = YUV_420_UNPACKED;
		Dp.PropSet = VIDEO_SET;
		Dp.PropId = evYUVWriteParams;
		Dp.PropTypeLength = sizeof (evYUVWriteParams_type);
		Dp.pValue = &yuv;
		retval = ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);
		if (retval != 0)
		{
			DEBUGMSG (1, ("ERROR: could not set yuv display parameters\n"));
			return (MPEG_DECODER_ERROR)retval;
		}
		isMPEG4 = m_isMPEG4;
		RUA_DECODER_SET_PROPERTY (m_handle, DECODER_SET, edecVideoStd, sizeof (isMPEG4), &isMPEG4);
		return MPEG_DECODER_ERROR_NO_ERROR;
	}

	if ((w_screen != m_yuvWidth) || (h_screen != m_yuvHeight))
	{
		int retval;
		decoderproperty Dp;	
		evYUVWriteParams_type yuv; 

		isMPEG4 = m_isMPEG4 = 0;
		RUA_DECODER_SET_PROPERTY (m_handle, DECODER_SET, edecVideoStd, sizeof (isMPEG4), &isMPEG4);

		yuv.wWidth = w_screen;
		yuv.wHeight = h_screen;
		yuv.YUVFormat = YUV_420_UNPACKED;
		Dp.PropSet = VIDEO_SET;
		Dp.PropId = evYUVWriteParams;
		Dp.PropTypeLength = sizeof (evYUVWriteParams_type);
		Dp.pValue = &yuv;
		retval = ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);
		if (retval == 0)
		{
			m_yuvWidth = w_screen;
			m_yuvHeight = h_screen;	
		}
		else
		{
			DEBUGMSG (1, ("ERROR: could not set yuv display parameters\n"));
			return (MPEG_DECODER_ERROR)retval;
		}
	}

	DEBUGMSG (1, ("MpegDecoder::DisplayYUV420 (0x%08lx,0x%08lx,%3d,%3d,%3d,%3d)\n", (RMuint32)pY, (RMuint32)pUV, x, y, w, h));
	ASSERT (m_yuvHeight >= h);
	ASSERT (m_yuvWidth >= w);

	Yaddr = (y * m_yuvWidth) + x;
	UVaddr = (y * m_yuvWidth / 2) + x;
	for (i=0; i<h; i++)
	{
		ASSERT (Yaddr < (m_yuvHeight*m_yuvWidth));
		F.pY = (RMuint32 *)Y;		
		F.pUV = (RMuint32 *)UV;
		F.nYbytes = w;
		F.Yaddr = Yaddr;
		F.UVaddr = UVaddr;
		if (i & 1)
			F.nUVbytes = 0;
		else
			F.nUVbytes = w;
		ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_DISPLAY_YUV, &F);
		Y += w;
		Yaddr += m_yuvWidth;
		if ((i & 1) == 0)
		{
			UV += w;
			UVaddr += m_yuvWidth;
		}
	}
	return MPEG_DECODER_ERROR_NO_ERROR;	
} 

MPEG_DECODER_ERROR MpegDecoder::RenderText (RMint8 *text, RMuint32 colour, RMint32 x, RMint32 y, RMint32 w, RMint32 h)
{
	RMuint8 *p = (RMuint8 *)m_OsdBuffer.framebuffer;

	if (text == 0)
	{
		RMint32 i;
		RMuint8 *pbits = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;
		pbits += (y * m_osdWidth);
		pbits += x;
		for (i=0; i<h; i++)
		{
			memset (pbits, 0, w);
			pbits += m_osdWidth;
		}
		asm ("mcr p15, 0, r0, c7, c10, 0\n");	
		ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);
		return MPEG_DECODER_ERROR_NO_ERROR;
	}
/*
	switch (colour)
	{
	case 0xffff00:
		pixel_value = 2;
		break;
	case 0x0000ff:
		pixel_value = 3;
		break;
	default:
		pixel_value = 1;
		break;
	}
*/
	pixel_value = colour;

	sdPOINT pt;
	pt.x = x;
	pt.y = y;
	pt.x2 = x+w;
	pt.y2 = y+h;
	//DrawText (&font_myfont, text, &pt, 1, p+8+1024, m_osdWidth, m_osdHeight);
	DrawText (&font_myfont, text, &pt, colour, p+8+1024, m_osdWidth, m_osdHeight);

	asm ("mcr p15, 0, r0, c7, c10, 0\n");	
	ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::RenderText_WithoutRefresh(RMint8 *text, RMuint32 colour, RMint32 x, RMint32 y, RMint32 w, RMint32 h)
{
	RMuint8 *p = (RMuint8 *)m_OsdBuffer.framebuffer;

	if (text == 0)
	{
		RMint32 i;
		RMuint8 *pbits = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;
		pbits += (y * m_osdWidth);
		pbits += x;
		for (i=0; i<h; i++)
		{
			memset (pbits, 0, w);
			pbits += m_osdWidth;
		}
//		asm ("mcr p15, 0, r0, c7, c10, 0\n");	
//		ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);
		return MPEG_DECODER_ERROR_NO_ERROR;
	}

	pixel_value = colour;
	
	sdPOINT pt;
	pt.x = x;
	pt.y = y;
	pt.x2 = x+w;
	pt.y2 = y+h;
	DrawText (&font_myfont, text, &pt, colour, p+8+1024, m_osdWidth, m_osdHeight);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::RenderOsdBits (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h)
{	
	RMint32 i;
	RMuint8 *pbits = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;
	pbits += (y * m_osdWidth);
	pbits += x;
	for (i=0; i<h; i++)
	{
		if (osd)
			memcpy (pbits, osd, w);
		else
			memset (pbits, 1, w);
		pbits += m_osdWidth;
		if (osd)
			osd += w;
	}
	asm ("mcr p15, 0, r0, c7, c10, 0\n");	
	ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);
	return MPEG_DECODER_ERROR_NO_ERROR;
}


MPEG_DECODER_ERROR MpegDecoder::SaveOsdBits (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h)
{
	RMint32 i;
	RMuint8 *pbits = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;
	pbits += (y * m_osdWidth);
	pbits += x;
	for (i=0; i<h; i++)
	{
		memcpy (osd, pbits, w);
		pbits += m_osdWidth;
		osd += w;
	}
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetVideoDestination (RMint32 x, RMint32 y, RMint32 w, RMint32 h)
{
	decoderproperty Dp;
	Wnd_type Wnd; 
	if ((w == 0) && (h == 0))
	{
		w = m_screenWidth;
		h = m_screenHeight;
	}
	Wnd.x = x;
	Wnd.y = y;
	Wnd.w = w;
	Wnd.h = h;
	Dp.PropSet = VIDEO_SET;
	Dp.PropId = evDestinationWindow;
	Dp.PropTypeLength = sizeof (Wnd_type);
	Dp.pValue = &Wnd;
//	ASSERT (Wnd.x >= 0);
	ASSERT (Wnd.y >= 0);
	ASSERT (Wnd.w >= 0);
	ASSERT (Wnd.w <= 1920);
	ASSERT (Wnd.h <= 1080);
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::Play ()
{
	RMuint32 mode = VideoHwPlayNormal;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_PLAY, &mode);
	DEBUGMSG(1,("(play)\n"));	
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::PlayIFrame ()
{
	RMuint32 mode = VideoHwPlayIFrame;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_PLAY, &mode);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::PlayIFrame (RMint32 speed)
{
	if (speed < 0)
	{
		speed = -speed;
		RUA_DECODER_STOP (m_handle);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evVOBUReverseSpeed, sizeof(speed), &speed);
		RUA_DECODER_PLAY (m_handle, VideoHwPlayIFrameReverse);
	}
	else if (speed > 0)
	{
		RUA_DECODER_STOP (m_handle);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evSpeed, sizeof(speed), &speed);
		RUA_DECODER_PLAY (m_handle, VideoHwPlayIFrame);	
	}
	else
	{
		speed = 256;
		RUA_DECODER_STOP (m_handle);
		RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evSpeed, sizeof(speed), &speed);
		RUA_DECODER_PLAY (m_handle, VideoHwPlayNormal);
	}
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::Pause ()
{
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_PAUSE, 0);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::Reset ()
{
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_RESET, 0);
	return MPEG_DECODER_ERROR_NO_ERROR;
}


MPEG_DECODER_ERROR MpegDecoder::SeekStop ()
{
	RMuint32 n = 0;

//	RUA_DECODER_STOP (m_handle);
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_DISCARD_REMAINING, 0);
//	RUA_DECODER_PLAY (m_handle, VideoHwPlayNormal);
	FeedPacketQueuesInit (FeedPacketsArray, MAX_FEEDPACKETS);

	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evSkippedFramesCounter, sizeof(n), &n);
	printf ("%d frames skipped.\n", n);
	fflush (stdout);


	return MPEG_DECODER_ERROR_NO_ERROR;
}


MPEG_DECODER_ERROR MpegDecoder::Stop ()
{
	// just for testing
/*
	RMuint32 n = 0;
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evSkippedFramesCounter, sizeof(n), &n);
	printf ("%d frames skipped.\n", n);
	fflush (stdout);
*/
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_STOP, 0);
	FeedPacketQueuesInit (FeedPacketsArray, MAX_FEEDPACKETS);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::GetSTC (RMint64 *pSTC, RMuint32 Resolution)
{
    etimSystemTimeClock_type foo;

    foo.dwTimeResolution_Hz = Resolution;
    RUA_DECODER_GET_PROPERTY (m_handle, TIME_SET, etimSystemTimeClock,
		sizeof(etimSystemTimeClock_type),&foo);
	*pSTC = foo.dwlTime;
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetSTC (RMint64 Stc, RMuint32 timeResolution)
{
    etimSystemTimeClock_type foo;

    foo.dwlTime = Stc;
    foo.dwTimeResolution_Hz = timeResolution;
    RUA_DECODER_SET_PROPERTY (m_handle, TIME_SET, etimSystemTimeClock,
		sizeof(etimSystemTimeClock_type),&foo);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::EndOfStream (RMint64 time, RMuint32 Resolution)
{
	if (time == -1)
	{
		edecDataFifo_type datafifo;
		datafifo.Type = AUDIO;
		RUA_DECODER_GET_PROPERTY (m_handle, DECODER_SET, edecDataFifo,
			sizeof(datafifo), &datafifo);
		DEBUGMSG (1, ("audio datafifo fullness: %d\n", (RMint32)datafifo.Info.Fullness));
		if (datafifo.Info.Fullness < 1024)
		{
			datafifo.Type = VIDEO;
			RUA_DECODER_GET_PROPERTY (m_handle, DECODER_SET, edecDataFifo,
				sizeof(datafifo), &datafifo);
			DEBUGMSG (1, ("video datafifo fullness: %d\n", (RMint32)datafifo.Info.Fullness));
			if (datafifo.Info.Fullness < 8192)
			{
				// data fifos are empty
				return MPEG_DECODER_ERROR_EOF;
			}
		}
		return MPEG_DECODER_ERROR_NO_ERROR;
	}
    etimSystemTimeClock_type foo;
    foo.dwTimeResolution_Hz = Resolution;
    RUA_DECODER_GET_PROPERTY (m_handle, TIME_SET, etimSystemTimeClock,
		sizeof(etimSystemTimeClock_type),&foo);
	DEBUGMSG (1, ("STC = %d, time to stop = %d\n", (RMint32)foo.dwlTime, (RMint32)time));
	if (foo.dwlTime >= time)
	{
		// STC is greater than or equal to the specified time
		return MPEG_DECODER_ERROR_EOF;
	}
	return MPEG_DECODER_ERROR_NO_ERROR;	
}

MPEG_DECODER_ERROR MpegDecoder::SetPCMParameters (PCM_PARAMETERS *pPcmParameters)
{
	decoderproperty Dp;

	RMint32 activate = 0;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);

	RMuint32 sr = pPcmParameters->SamplesPerSecond;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioSampleRate;
	Dp.PropTypeLength = sizeof (RMuint32);
	Dp.pValue = &sr;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	eAudioDigitalOutput_type outputType = eAudioDigitalOutput_Pcm;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioDigitalOutput;
	Dp.PropTypeLength = sizeof (outputType);
	Dp.pValue = &outputType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	RMuint32 bps = pPcmParameters->NumberOfBitsPerSample;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioNumberOfBitsPerSample;
	Dp.PropTypeLength = sizeof (bps);
	Dp.pValue = &bps;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	RMuint32 nac = pPcmParameters->NumberOfChannels;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioNumberOfChannels;
	Dp.PropTypeLength = sizeof (nac);
	Dp.pValue = &nac;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	eAudioFormat_type formatType = eAudioFormat_PCM;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioFormat;
	Dp.PropTypeLength = sizeof (formatType);
	Dp.pValue = &formatType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	activate = 1;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);

	activate = 0;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);

	activate = 1;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);
		
	sr = pPcmParameters->SamplesPerSecond;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioSampleRate;
	Dp.PropTypeLength = sizeof (RMuint32);
	Dp.pValue = &sr;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	outputType = eAudioDigitalOutput_Pcm;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioDigitalOutput;
	Dp.PropTypeLength = sizeof (outputType);
	Dp.pValue = &outputType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	bps = pPcmParameters->NumberOfBitsPerSample;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioNumberOfBitsPerSample;
	Dp.PropTypeLength = sizeof (bps);
	Dp.pValue = &bps;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	nac = pPcmParameters->NumberOfChannels;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioNumberOfChannels;
	Dp.PropTypeLength = sizeof (nac);
	Dp.pValue = &nac;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	formatType = eAudioFormat_PCM;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioFormat;
	Dp.PropTypeLength = sizeof (formatType);
	Dp.pValue = &formatType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);
	
#if XEN_LIB
	sr = pPcmParameters->SamplesPerSecond;
	Dp.PropId = eXENsetContentsProp;
	Dp.pValue = &sr;
	ioctl (m_handle, REALMAGICHWL_IOCTL_XEN_SET_PROPERTY, &Dp);
#endif // XEN_LIB
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetAC3Parameters (AC3_PARAMETERS *pAc3Parameters)
{
	decoderproperty Dp;

	RMint32 activate = 0;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);

	RMuint32 sr = pAc3Parameters->SamplesPerSecond;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioSampleRate;
	Dp.PropTypeLength = sizeof (RMuint32);
	Dp.pValue = &sr;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	eAudioDigitalOutput_type outputType = eAudioDigitalOutput_Pcm;
	if (pAc3Parameters->rawOutput)
		outputType = eAudioDigitalOutput_Compressed;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioDigitalOutput;
	Dp.PropTypeLength = sizeof (outputType);
	Dp.pValue = &outputType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	eAudioFormat_type formatType = eAudioFormat_AC3;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioFormat;
	Dp.PropTypeLength = sizeof (formatType);
	Dp.pValue = &formatType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	activate = 1;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetDTSParameters (DTS_PARAMETERS *pDtsParameters)
{
	decoderproperty Dp;

	RMint32 activate = 0;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);

	RMuint32 sr = pDtsParameters->SamplesPerSecond;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioSampleRate;
	Dp.PropTypeLength = sizeof (RMuint32);
	Dp.pValue = &sr;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	// only pass though allowed
	ASSERT (pDtsParameters->rawOutput);
	eAudioDigitalOutput_type outputType = eAudioDigitalOutput_Compressed;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioDigitalOutput;
	Dp.PropTypeLength = sizeof (outputType);
	Dp.pValue = &outputType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	eAudioFormat_type formatType = eAudioFormat_DTS;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioFormat;
	Dp.PropTypeLength = sizeof (formatType);
	Dp.pValue = &formatType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	activate = 1;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetMpegAudioParameters (MPEGAUDIO_PARAMETERS *pMpegAudioParameters)
{
	decoderproperty Dp;

	RMint32 activate = 0;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);

	RMuint32 sr = pMpegAudioParameters->SamplesPerSecond;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioSampleRate;
	Dp.PropTypeLength = sizeof (RMuint32);
	Dp.pValue = &sr;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	eAudioDigitalOutput_type outputType = eAudioDigitalOutput_Pcm;
	if (pMpegAudioParameters->rawOutput)
		outputType = eAudioDigitalOutput_Compressed;
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioDigitalOutput;
	Dp.PropTypeLength = sizeof (outputType);
	Dp.pValue = &outputType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	eAudioFormat_type formatType = eAudioFormat_MPEG1;
	if (pMpegAudioParameters->layer == 3)
	{
		ASSERT (0);
		formatType = eAudioFormat_MPEG1_LAYER3;
	}
	Dp.PropSet = AUDIO_SET;
	Dp.PropId = eAudioFormat;
	Dp.PropTypeLength = sizeof (formatType);
	Dp.pValue = &formatType;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_SET_PROPERTY, &Dp);

	activate = 1;
	ioctl (m_handle, REALMAGICHWL_IOCTL_DECODER_AUDIO_SWITCH, &activate);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetMpegVideoParameters (MPEGVIDEO_PARAMETERS *pMpegVideoParameters)
{
	DEBUGMSG (1, ("MpegDecoder::SetMpegVideoParameters:\n"));
	DEBUGMSG (1, (" isMPEG4: %lu\n", pMpegVideoParameters->isMPEG4));
	DEBUGMSG (1, (" vopTimeIncrRes: %lu\n", pMpegVideoParameters->vopTimeIncrRes));
	DEBUGMSG (1, (" videoTimeScale: %lu\n", pMpegVideoParameters->videoTimeScale));
	DEBUGMSG (1, (" audioTimeScale: %lu\n", pMpegVideoParameters->audioTimeScale));
	DEBUGMSG (1, (" fixedVOPRate: %lu\n", pMpegVideoParameters->fixedVOPRate));
	DEBUGMSG (1, (" vopTimeIncr: %lu\n", pMpegVideoParameters->vopTimeIncr));

	RUA_DECODER_STOP (m_handle);
	RUA_DECODER_SET_PROPERTY (m_handle, DECODER_SET, edecVideoStd, sizeof (pMpegVideoParameters->isMPEG4), &pMpegVideoParameters->isMPEG4);
	RUA_DECODER_PLAY (m_handle, VideoHwPlayNormal);
	if (pMpegVideoParameters->isMPEG4)
	{	
		RUA_DECODER_SET_PROPERTY (m_handle, TIME_SET, 
			etimVOPTimeIncrRes,
			sizeof (pMpegVideoParameters->vopTimeIncrRes),
			&pMpegVideoParameters->vopTimeIncrRes);		
		RUA_DECODER_SET_PROPERTY (m_handle, TIME_SET,
			etimVideoCTSTimeScale,
			sizeof (pMpegVideoParameters->videoTimeScale),
			&pMpegVideoParameters->videoTimeScale);
		RUA_DECODER_SET_PROPERTY (m_handle, TIME_SET,
			etimAudioCTSTimeScale,
			sizeof (pMpegVideoParameters->audioTimeScale),
			&pMpegVideoParameters->audioTimeScale);
		if (pMpegVideoParameters->fixedVOPRate)
		{
			edecForceFixedVOPRate_type FixedVOPRate;
			FixedVOPRate.dwFixedTimeIncrRes = pMpegVideoParameters->videoTimeScale;
			FixedVOPRate.dwFixedVopTimeIncr = pMpegVideoParameters->vopTimeIncr;
			FixedVOPRate.dwForceFixedVOPRate = 1;
			RUA_DECODER_SET_PROPERTY (m_handle, DECODER_SET,
				edecForceFixedVOPRate,
				sizeof (FixedVOPRate),
				&FixedVOPRate);
		}
		else
		{
			edecForceFixedVOPRate_type FixedVOPRate;
			FixedVOPRate.dwFixedTimeIncrRes = 30000;
			FixedVOPRate.dwFixedVopTimeIncr = 1000;
			FixedVOPRate.dwForceFixedVOPRate = 0;
			RUA_DECODER_SET_PROPERTY (m_handle, DECODER_SET,
				edecForceFixedVOPRate,
				sizeof (FixedVOPRate),
				&FixedVOPRate);
		}
	}
	m_isMPEG4 = pMpegVideoParameters->isMPEG4;
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::WritePCM (RMuint8 *pPCM, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts)
{
	feedpacket *pDataPacket;
	pDataPacket = getFreeFeedPacket ();
	if (pDataPacket == 0)
	{
		return MPEG_DECODER_ERROR_NO_ROOM;
	}

	DEBUGMSG (0, ("-> PCM (%10lu, %10lu)\n", (RMuint32)scr, (RMuint32)pts));
	pDataPacket->vobu_sptm = 0;
	pDataPacket->scr = scr;
	pDataPacket->flags = flags;
	pDataPacket->packet.Pts = pts;
	pDataPacket->packet.pData = pPCM;
	pDataPacket->packet.EncryptedInfo = 0;
	pDataPacket->packet.unused1 = 0;	
	pDataPacket->packet.pOverlapped = prefcount;
	pDataPacket->packet.DataLeft = length;
	ASSERT (length <= 0x2000);
	pDataPacket->streamID = AUDIO;
	pDataPacket->packet.nFrameHeaders = 0xffff;
	pDataPacket->packet.FirstAccessUnitPointer = 0x0;
	asm ("mcr p15, 0, r0, c7, c10, 0\n");
	nextPacket (pAudioQ);
//	printf("WritePCM : nb=%ld, in=%ld, out=%ld\n", pAudioQ->nb, pAudioQ->nb_in, pAudioQ->nb_out );
	return MPEG_DECODER_ERROR_NO_ERROR;
}

#if 0
MPEG_DECODER_ERROR MpegDecoder::WriteAC3 (RMuint8 *pAc3, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts)
{
	feedpacket *pDataPacket;
	pDataPacket = getFreeFeedPacket ();
	if (pDataPacket == 0)
	{
		return MPEG_DECODER_ERROR_NO_ROOM;
	}

	DEBUGMSG (0, ("-> AC3 (%10lu, %10lu)\n", (RMuint32)scr, (RMuint32)pts));

	pDataPacket->vobu_sptm = 0;
	pDataPacket->scr = scr;
	pDataPacket->flags = flags;
	pDataPacket->packet.Pts = pts;
	pDataPacket->packet.pData = pAc3;
	pDataPacket->packet.EncryptedInfo = 0;
	pDataPacket->packet.unused1 = 0;	
	pDataPacket->packet.pOverlapped = prefcount;
	pDataPacket->packet.DataLeft = length;
	ASSERT (length <= 0xffff);
	pDataPacket->streamID = AUDIO;
	pDataPacket->packet.nFrameHeaders = 0xffff;
	pDataPacket->packet.FirstAccessUnitPointer = 0x0;
	asm ("mcr p15, 0, r0, c7, c10, 0\n");
	nextPacket (pAudioQ);
	return MPEG_DECODER_ERROR_NO_ERROR;
}
#else
MPEG_DECODER_ERROR MpegDecoder::WriteAC3 (RMuint8 *pAc3, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts, RMuint32 firstaccessunitpointer)
{
	feedpacket *pDataPacket;
	pDataPacket = getFreeFeedPacket ();
	if (pDataPacket == 0)
	{
		return MPEG_DECODER_ERROR_NO_ROOM;
	}

	DEBUGMSG (0, ("-> AC3 (%10lu, %10lu)\n", (RMuint32)scr, (RMuint32)pts));

	pDataPacket->vobu_sptm = 0;
	pDataPacket->scr = scr;
	pDataPacket->flags = flags;
	pDataPacket->packet.Pts = pts;
	pDataPacket->packet.pData = pAc3;
	pDataPacket->packet.EncryptedInfo = 0;
	pDataPacket->packet.unused1 = 0;	
	pDataPacket->packet.pOverlapped = prefcount;
	pDataPacket->packet.DataLeft = length;
	ASSERT (length <= 0xffff);
	pDataPacket->streamID = AUDIO;
	pDataPacket->packet.nFrameHeaders = 0xffff;
	pDataPacket->packet.FirstAccessUnitPointer = firstaccessunitpointer;
	asm ("mcr p15, 0, r0, c7, c10, 0\n");
	nextPacket (pAudioQ);
	return MPEG_DECODER_ERROR_NO_ERROR;
}
#endif


MPEG_DECODER_ERROR MpegDecoder::WriteDTS (RMuint8 *pDts, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts, RMint32 nFrameHeaders, RMint32 FirstAccessUnitPointer)
{
	feedpacket *pDataPacket;
	pDataPacket = getFreeFeedPacket ();
	if (pDataPacket == 0)
	{
		return MPEG_DECODER_ERROR_NO_ROOM;
	}

	DEBUGMSG (0, ("-> DTS (%10lu, %10lu)\n", (RMuint32)scr, (RMuint32)pts));

	pDataPacket->vobu_sptm = 0;
	pDataPacket->scr = scr;
	pDataPacket->flags = flags;
	pDataPacket->packet.Pts = pts;
	pDataPacket->packet.pData = pDts;
	pDataPacket->packet.EncryptedInfo = 0;
	pDataPacket->packet.unused1 = 0;	
	pDataPacket->packet.pOverlapped = prefcount;
	pDataPacket->packet.DataLeft = length;
	ASSERT (length <= 0xffff);
	pDataPacket->streamID = AUDIO;
	pDataPacket->packet.nFrameHeaders = 1;
	pDataPacket->packet.FirstAccessUnitPointer = 0;
	asm ("mcr p15, 0, r0, c7, c10, 0\n");
	nextPacket (pAudioQ);
	return MPEG_DECODER_ERROR_NO_ERROR;
}


MPEG_DECODER_ERROR MpegDecoder::WriteMpegAudio (RMuint8 *pAudio, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts)
{
	feedpacket *pDataPacket;
	pDataPacket = getFreeFeedPacket ();
	if (pDataPacket == 0)
	{
		return MPEG_DECODER_ERROR_NO_ROOM;
	}

	DEBUGMSG (0, ("-> MPEGA (%10lu, %10lu)\n", (RMuint32)scr, (RMuint32)pts));

	pDataPacket->vobu_sptm = 0;
	pDataPacket->scr = scr;
	pDataPacket->flags = flags;
	pDataPacket->packet.Pts = pts;
	pDataPacket->packet.pData = pAudio;
	pDataPacket->packet.EncryptedInfo = 0;
	pDataPacket->packet.unused1 = 0;	
	pDataPacket->packet.pOverlapped = prefcount;
	pDataPacket->packet.DataLeft = length;
	ASSERT (length <= 0xffff);
	pDataPacket->streamID = AUDIO;
	pDataPacket->packet.nFrameHeaders = 0xffff;
	pDataPacket->packet.FirstAccessUnitPointer = 0x0;
	asm ("mcr p15, 0, r0, c7, c10, 0\n");
	nextPacket (pAudioQ);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::WriteMpegVideo (RMuint8 *pVideo, RMuint32 length, RMuint8 *prefcount, RMuint32 flags, RMint64 scr, RMint64 pts)
{
	feedpacket *pDataPacket;
	pDataPacket = getFreeFeedPacket ();
	if (pDataPacket == 0)
	{
		ASSERT (0);		
		return MPEG_DECODER_ERROR_NO_ROOM;
	}
	DEBUGMSG (0, ("-> VID (%10lu, %10lu) : %10lu (%ld)\n", (RMuint32)scr, (RMuint32)pts));
	pDataPacket->vobu_sptm = 0;
	pDataPacket->scr = scr;
	pDataPacket->flags = flags;
	pDataPacket->packet.Pts = pts;
	pDataPacket->packet.pData = pVideo;
	pDataPacket->packet.EncryptedInfo = 0;
	pDataPacket->packet.unused1 = 0;	
	pDataPacket->packet.pOverlapped = prefcount;
	ASSERT (length <= 0xffff);
	pDataPacket->packet.DataLeft = length;
	pDataPacket->streamID = VIDEO;
	asm ("mcr p15, 0, r0, c7, c10, 0\n");
	nextPacket (pVideoQ);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

#define MULTIPLIER 16
#define MAX_BRIGHTNESS 60
#define MAX_CONTRAST 60
#define MAX_SATURATION 60
#define DEFAULT_BRIGHTNESS 32
#define DEFAULT_CONTRAST 32
#define DEFAULT_SATURATION 32
MPEG_DECODER_ERROR MpegDecoder::Resume_ColorTemperature( char brightness, char contrast, char saturation )
{
	RMint32 value;

	if( brightness > MAX_BRIGHTNESS )
		brightness = DEFAULT_BRIGHTNESS;

	if( contrast > MAX_CONTRAST )
		contrast= DEFAULT_CONTRAST;

	if( saturation > MAX_SATURATION )
		saturation= DEFAULT_SATURATION;

	value = (RMint32)brightness;
	value *= MULTIPLIER;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evBrightness, sizeof(value), &value);
	
	value = (RMint32)contrast;
	value *= MULTIPLIER;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evContrast, sizeof(value), &value);

	value = (RMint32)saturation;
	value *= MULTIPLIER;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evSaturation, sizeof(value), &value);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::Brightness (RMint32 direction, RMint32 *pvalue)
{
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evBrightness, sizeof(*pvalue), pvalue);
	if (direction == 0)
	{
		*pvalue /= MULTIPLIER; //32;
		return MPEG_DECODER_ERROR_NO_ERROR;
	}
	if (direction > 0)
		*pvalue += MULTIPLIER;
	else
		*pvalue -= MULTIPLIER;
	if (*pvalue > 1000)
		*pvalue = 1000;
	if (*pvalue < 0)
		*pvalue = 0;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evBrightness, sizeof(*pvalue), pvalue);
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evBrightness, sizeof(*pvalue), pvalue);
	*pvalue /= MULTIPLIER;
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::Contrast (RMint32 direction, RMint32 *pvalue)
{
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evContrast, sizeof(*pvalue), pvalue);
	if (direction == 0)
	{
		*pvalue /= MULTIPLIER;
		return MPEG_DECODER_ERROR_NO_ERROR;
	}
	if (direction > 0)
		*pvalue += MULTIPLIER;
	else
		*pvalue -= MULTIPLIER;
	if (*pvalue > 1000)
		*pvalue = 1000;
	if (*pvalue < 0)
		*pvalue = 0;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evContrast, sizeof(*pvalue), pvalue);
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evContrast, sizeof(*pvalue), pvalue);
	*pvalue /= MULTIPLIER;
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::Saturation (RMint32 direction, RMint32 *pvalue)
{
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evSaturation, sizeof(*pvalue), pvalue);
	if (direction == 0)
	{
		*pvalue /= MULTIPLIER;
		return MPEG_DECODER_ERROR_NO_ERROR;
	}
	if (direction > 0)
		*pvalue += MULTIPLIER;
	else
		*pvalue -= MULTIPLIER;
	if (*pvalue > 1000)
		*pvalue = 1000;
	if (*pvalue < 0)
		*pvalue = 0;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evSaturation, sizeof(*pvalue), pvalue);
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evSaturation, sizeof(*pvalue), pvalue);
	*pvalue /= MULTIPLIER;
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::ClearScreen()
{
	ebiCommand_type command;
	command = ebiCommand_VideoHwBlackFrame;
	RUA_DECODER_SET_PROPERTY (m_handle, BOARDINFO_SET, ebiCommand, sizeof(command), &command);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::Exit ()
{
	ASSERT (m_handle);
	close (m_handle);
	m_handle = 0;

	return MPEG_DECODER_ERROR_NO_ERROR;
}





////////////////////////////////////////////////////////////////////////////
// Additional NML-TAVI Methods by Sang-jin Ryu
MPEG_DECODER_ERROR MpegDecoder::TAVI_InitSystemPalette()
{
	/*
	NML-TAVI Pallette Setting

	0		transparent	//
	1		black		// 1 ~ 15 (16 palette ) system color
	2		white
	3		yellow
	4		blue
	5		red
	6~16	reserved

	17~255 : 			// programmable pallete
	*/

	int i;

	RMuint8 *ppalette = (RMuint8 *) m_OsdBuffer.framebuffer + 8;

	// system palette 0 :  _TRANSPARENT
	ppalette[0] = 0;
	ppalette[1] = rgb2y(0,0,0); 
	ppalette[2] = rgb2u(0,0,0); 
	ppalette[3] = rgb2v(0,0,0); 
	ppalette += 4;

	// system palette 1 : _BLACK
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(0,0,0); 
	ppalette[2] = rgb2u(0,0,0); 
	ppalette[3] = rgb2v(0,0,0); 
	ppalette += 4;

	// system palette 2 : _WHITE
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(0xff, 0xff, 0xff); 
	ppalette[2] = rgb2u(0xff, 0xff, 0xff); 
	ppalette[3] = rgb2v(0xff, 0xff, 0xff); 
	ppalette += 4;

	// system palette 3 : _YELLOW
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(253, 240, 33); 
	ppalette[2] = rgb2u(253, 240, 33); 
	ppalette[3] = rgb2v(253, 240, 33); 
	ppalette += 4;

	// system palette 4 : _BLUE
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(33, 253, 33); 
	ppalette[2] = rgb2u(33, 253, 33); 
	ppalette[3] = rgb2v(33, 253, 33); 
	ppalette += 4;

	// system palette 5 : _RED
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(253, 0, 0); 
	ppalette[2] = rgb2u(253, 0, 0); 
	ppalette[3] = rgb2v(253, 0, 0); 
	ppalette += 4;

	// system palette 6 : _GREEN
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(136, 195, 19); 
	ppalette[2] = rgb2u(136, 195, 19); 
	ppalette[3] = rgb2v(136, 195, 19); 
	ppalette += 4;

	// system palette 7 : _ORANGE
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(255, 144, 0); 
	ppalette[2] = rgb2u(255, 144, 0); 
	ppalette[3] = rgb2v(255, 144, 0); 
	ppalette += 4;

	// system palette 8 : _GRAY
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(0x99, 0x99, 0x99); 
	ppalette[2] = rgb2u(0x99, 0x99, 0x99); 
	ppalette[3] = rgb2v(0x99, 0x99, 0x99); 
	ppalette += 4;	

	// system palette 9 : _LIGHTGREEN
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(152, 207, 30); 
	ppalette[2] = rgb2u(152, 207, 30); 
	ppalette[3] = rgb2v(152, 207, 30);
	ppalette += 4;

	// system palette 10 : _DARKGREEN
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(116,201,3);
	ppalette[2] = rgb2u(116,201,3);
	ppalette[3] = rgb2v(116,201,3);
	ppalette += 4;	

	for (i=11; i<16; i++)
	{
	ppalette[0] = 0xff;
	ppalette[1] = rgb2y(0xff, 0, 0xff); 
	ppalette[2] = rgb2u(0xff, 0, 0xff); 
	ppalette[3] = rgb2v(0xff, 0, 0xff); 
	ppalette += 4;
	}

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::TAVI_ShowPaletteInfo()
{
	RMint32 i;
	RMuint8 *ppalette = (RMuint8 *)m_OsdBuffer.framebuffer + 8 ;

	for(i=0;i<256;i++)
		{
		printf("%d[%x][%x][%x][%x]\n", i, (U8)ppalette[0], (U8)ppalette[1], (U8)ppalette[2], (U8)ppalette[3] );
		ppalette += 4;
		}

	return MPEG_DECODER_ERROR_NO_ERROR;	
}
	
MPEG_DECODER_ERROR MpegDecoder::RenderOsdPalette (RMuint8 *palette)
{
	RMint32 i;
	RMuint8 *ppalette = (RMuint8 *)m_OsdBuffer.framebuffer + 8 ;


	for(i=0; i<16;i++)
		{
		ppalette += 4;
		palette += 4;
		}

	for (i=16; i<256; i++)
	{
		ppalette[0] = palette[0];
		ppalette[1] = palette[1];
		ppalette[2] = palette[2];
		ppalette[3] = palette[3];

		ppalette += 4;
		palette += 4;
	}

	return MPEG_DECODER_ERROR_NO_ERROR;	
}




MPEG_DECODER_ERROR MpegDecoder::RenderOsdBits_WithoutRefresh (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h)
{	
	RMint32 i;
	RMuint8 *pbits = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;
	pbits += (y * m_osdWidth);
	pbits += x;
	for (i=0; i<h; i++)
	{
		if (osd)
			memcpy (pbits, osd, w);
		else
			memset (pbits, 1, w);
		pbits += m_osdWidth;
		if (osd)
			osd += w;
	}
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::RenderOsdBits_WithoutRefresh_SkipTransparency (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h)
{	
	RMint32 i,j;
	RMint32 skipbytes 	= m_osdWidth - w;
	RMuint8 *pbits 		= (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;

	pbits 			+= (y * m_osdWidth);
	pbits 			+= x;
	
	for (i=0; i<h; i++)
	{
		for( j=0; j<w; j++, pbits++, osd++ )
		{
			if(*osd)
				*pbits = *osd;
		}

		pbits += skipbytes;
		
	}
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::RenderOsdBits_WithoutRefresh_SkipTransparency_WithVariableFontColor (RMuint8 *osd, RMuint8 color, RMint32 x, RMint32 y, RMint32 w, RMint32 h )
{	
	RMint32 i,j;
	RMint32 skipbytes 	= m_osdWidth - w;
	RMuint8 *pbits 		= (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;

	pbits 			+= (y * m_osdWidth);
	pbits 			+= x;
	
	for (i=0; i<h; i++)
	{
		for( j=0; j<w; j++, pbits++, osd++ )
		{
			if(*osd)
				*pbits = color;
		}

		pbits += skipbytes;
		
	}
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::RenderOsdBits_WithoutRefresh_WithFixedBackgroundColor (RMuint8 *osd, RMint32 x, RMint32 y, RMint32 w, RMint32 h, RMuint8 FixedBackgroundColor)
{	
	RMint32 i,j;
	RMint32 skipbytes 	= m_osdWidth - w;
	RMuint8 *pbits 		= (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;

	pbits 			+= (y * m_osdWidth);
	pbits 			+= x;
	
	for (i=0; i<h; i++)
	{
		for( j=0; j<w; j++, pbits++, osd++ )
		{
			if(*osd)
				*pbits = *osd;
			else
				*pbits = FixedBackgroundColor;
		}

		pbits += skipbytes;
		
	}
	return MPEG_DECODER_ERROR_NO_ERROR;
}


MPEG_DECODER_ERROR MpegDecoder::RefreshOsd()
{
	asm ("mcr p15, 0, r0, c7, c10, 0\n");	
	ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::RenderOsdBox_WithoutRefresh(RMuint8 palette, RMint32 x, RMint32 y, RMint32 w, RMint32 h )
{	
	RMint32 i,j;
	RMuint8 *pbits = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;
	pbits += (y * m_osdWidth);
	pbits += x;
	for (i=0; i<h; i++)
	{
		memset (pbits, palette, w);
		pbits += m_osdWidth;
	}
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::RenderOsdBox(RMuint8 palette, RMint32 x, RMint32 y, RMint32 w, RMint32 h )
{	
	RMint32 i,j;
	RMuint8 *pbits = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;
	pbits += (y * m_osdWidth);
	pbits += x;
	for (i=0; i<h; i++)
	{
		memset (pbits, palette, w);
		pbits += m_osdWidth;
	}
	asm ("mcr p15, 0, r0, c7, c10, 0\n");	
	ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::ClearAreaOSD( int x, int y, int w, int h )
{
	RMuint8* p = ( RMuint8* )m_OsdBuffer.framebuffer + 8 + 1024;
	int i;
	
	p += y*m_osdWidth + x;
	for( i=0; i<h; i++ ) {
		memset( p, 0, w );
		p+=m_osdWidth;
	}
	asm( "mcr p15, 0, r0, c7, c10, 0\n" );
	ioctl( m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0 );
	
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::ClearAreaOSD_WithoutRefresh( int x, int y, int w, int h )
{
	printf("this is ClearAreaOSD_WithoutRefresh\n");

	RMuint8* p = ( RMuint8* )m_OsdBuffer.framebuffer + 8 + 1024;
	int i;
	
	p += y*m_osdWidth + x;
	for( i=0; i<h; i++ ) {
		memset( p, 0, w );
		p+=m_osdWidth;
	}
	
	return MPEG_DECODER_ERROR_NO_ERROR;
}

void MpegDecoder::SetPixel( int color, int x, int y )
{
	RMuint8* pOsd = ( RMuint8* )m_OsdBuffer.framebuffer + 8 + 1024;
	int w = m_osdWidth;

	pOsd[(y * w) + x] = (RMuint8)color;
}

void MpegDecoder::SetPixel_SkipTransparency( int color, int x, int y )
{
	if( color )
		{
		RMuint8* pOsd = ( RMuint8* )m_OsdBuffer.framebuffer + 8 + 1024;
		int w = m_osdWidth;

		pOsd[(y * w) + x] = (RMuint8)color;
		}
}


void MpegDecoder::Refresh()
{
	asm ("mcr p15, 0, r0, c7, c10, 0\n");	
	ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);
}

RMuint8 MpegDecoder::GetPixelColor( int x, int y )
{
	RMuint8* pOsd = ( RMuint8* )m_OsdBuffer.framebuffer + 8 + 1024;
	int w = m_osdWidth;

	return ( pOsd[(y * w) + x] );
}

RMuint8 MpegDecoder::GetPixelTransparent( int x, int y )
{
	RMuint8* pOsd = ( RMuint8* )m_OsdBuffer.framebuffer + 8 + 1024;
	RMuint8 ret = pOsd[(y * m_osdWidth) + x] ;

//	DEBUGMSG(1,(" GetPixelTransparent( %d, %d ) == 0x%x\n", x, y, ret) );
	
	return ret;
}

MPEG_DECODER_ERROR MpegDecoder::OsdFadeIn()
{
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetAlpha ( RMuint8 color, RMuint8 alpha )
{
	RMuint8 *pOSDHeader, *pPalette;

	ASSERT (m_OsdBuffer.framebuffer);

	pOSDHeader = (RMuint8 *)(m_OsdBuffer.framebuffer);

	pPalette = pOSDHeader + 8;
	pPalette += color *4;
	pPalette[0] = alpha;				// alpha (0x00 = transparent, 0xff = opaque)

	asm ("mcr p15, 0, r0, c7, c10, 0\n");	
	ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::OsdBlockCopy( RMuint8* pB, int sx, int sy, int w, int h )
{
	RMuint8* pO = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;

	if( pB == NULL )
		return MPEG_DECODER_ERROR_NO_ERROR;

	pO += (sy * m_osdWidth);
	pO += sx;

	for(int i=0; i<h; i++)
		{
		memcpy( pB, pO, w );
		pO += m_osdWidth;
		pB += m_osdWidth;
		}
	return MPEG_DECODER_ERROR_NO_ERROR;
}
MPEG_DECODER_ERROR MpegDecoder::OsdBlockPaste( int sx, int sy, int w, int h, RMuint8* pB )
{
	RMuint8* pO = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;

	if( pB == NULL )
		return MPEG_DECODER_ERROR_NO_ERROR;

	pO += (sy * m_osdWidth);
	pO += sx;

	for(int i=0; i<h; i++)
		{
		memcpy( pO, pB, w );
		pO += m_osdWidth;
		pB += m_osdWidth;
		}
	return MPEG_DECODER_ERROR_NO_ERROR;
}


#define FadeStepValue	0x30;
MPEG_DECODER_ERROR MpegDecoder::OsdBlockFadeOut( int sx, int sy, int width, int height, int BASE_PALLETTE_INCLUDED_IMAGE_INDEX )
{
	/*
	1. save original palette
	2. 
	*/

/*
	int		PalletteMap[2][256];
	RMuint8 	VarPallette[256];

	RMuint8

	// check target palette
	RMint32 i,j;
	RMint32 skipbytes 	= m_osdWidth - w;
	RMuint8 temp;
	RMuint8* pPbuf;
	RMuint8* pOsd ;

	pOsd 				= (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;
	pOsd 				+= (sy * m_osdWidth);
	pOsd 				+= sx;
	pPbuf 				= VarPallette;
	
	for (i=0; i<height; i++)
	{
		for( j=0; j<width; j++, pOsd++, pPbuf++ )
		{
			if(*pOsd > 0x30)
				{
				temp = *pOsd;
				temp -= 0x30
				
				
				*pOsd = *pOsd;
				}
		}

		pOsd += skipbytes;
		
	}




	
	


	RMuint8* VarPalette;
	RMint32 i;
	RMuint8 *ppalette = (RMuint8 *)m_OsdBuffer.framebuffer + 8 ;

	palette = (RMuint8*) malloc(4*256);

	for(idx=0; idx < TAVI_IMAGES[BASE_PALLETTE_INCLUDED_IMAGE_INDEX].bitmapinfoheader.biCirImportant ; idx++)
	{
		RMuint8 r=TAVI_IMAGES[BASE_PALLETTE_INCLUDED_IMAGE_INDEX].palette[idx].R;
		RMuint8 g=TAVI_IMAGES[BASE_PALLETTE_INCLUDED_IMAGE_INDEX].palette[idx].G;
		RMuint8 b=TAVI_IMAGES[BASE_PALLETTE_INCLUDED_IMAGE_INDEX].palette[idx].B; 

		palette[(idx<<2)+0]= (RMuint8) aubVideoPlayerPalletteTransparent[idx];
		palette[(idx<<2)+1]=rgb_Y(r,g,b); 
		palette[(idx<<2)+2]=rgb_U(r,g,b); 
		palette[(idx<<2)+3]=rgb_V(r,g,b);
	} 

	pMpegDecoder->RenderOsdPalette(palette);
*/
	return MPEG_DECODER_ERROR_NO_ERROR;

}

MPEG_DECODER_ERROR MpegDecoder::SetBrightness (RMint32 direction, RMint32 *pvalue)
{
	DEBUGMSG(1,("SetBrightness : direction=%d\n", direction ));

	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evBrightness, sizeof(*pvalue), pvalue);
	if (direction == 0)
	{
		*pvalue /= MULTIPLIER;
		return MPEG_DECODER_ERROR_NO_ERROR;
	}

	*pvalue += (MULTIPLIER*direction);

	if (*pvalue > 1000)
		*pvalue = 1000;
	if (*pvalue < 0)
		*pvalue = 0;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evBrightness, sizeof(*pvalue), pvalue);
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evBrightness, sizeof(*pvalue), pvalue);
	*pvalue /= MULTIPLIER;
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetContrast (RMint32 direction, RMint32 *pvalue)
{
	DEBUGMSG(1,("SetContrast : direction=%d\n", direction ));

	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evContrast, sizeof(*pvalue), pvalue);
	if (direction == 0)
	{
		*pvalue /= MULTIPLIER;
		return MPEG_DECODER_ERROR_NO_ERROR;
	}

	*pvalue += (MULTIPLIER*direction);

	if (*pvalue > 1000)
		*pvalue = 1000;
	if (*pvalue < 0)
		*pvalue = 0;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evContrast, sizeof(*pvalue), pvalue);
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evContrast, sizeof(*pvalue), pvalue);
	*pvalue /= MULTIPLIER;
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetSaturation (RMint32 direction, RMint32 *pvalue)
{
	DEBUGMSG(1,("SetSaturation : direction=%d\n", direction ));

	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evSaturation, sizeof(*pvalue), pvalue);
	if (direction == 0)
	{
		*pvalue /= MULTIPLIER;
		return MPEG_DECODER_ERROR_NO_ERROR;
	}

	*pvalue += (MULTIPLIER*direction);

	if (*pvalue > 1000)
		*pvalue = 1000;
	if (*pvalue < 0)
		*pvalue = 0;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evSaturation, sizeof(*pvalue), pvalue);
	RUA_DECODER_GET_PROPERTY (m_handle, VIDEO_SET, evSaturation, sizeof(*pvalue), pvalue);
	*pvalue /= MULTIPLIER;
	return MPEG_DECODER_ERROR_NO_ERROR;
}


MPEG_DECODER_ERROR MpegDecoder::OsdBlockSlideDown( int x, int y, int w, int h  )
{

	RenderText(0,0,0,0,320,240);

#if 0
	RMint32 i,j,p;
	RMuint8* pOsd;
	RMuint8* pBuffer;
	RMuint8* pBlockStartPoint;
	int DrawingLinesAtOnce;

	// Init
	pBuffer			= (RMuint8*)malloc( sizeof(RMuint8)*w*h*2 );
	memset( pBuffer, 0, sizeof(RMuint8)*w*h*2 );
	
	// copy osd to buff
	// pBlockStartPoint	= (RMuint8 *)pBuffer + (w*h);	
	pBlockStartPoint	= pBuffer;
	pOsd			= (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024 + (y * m_osdWidth) + x;	
	for (i=0; i<h; i++)
		{
		memcpy( pBlockStartPoint, pOsd, w );

		pBlockStartPoint += w;
		pOsd += m_osdWidth;
		}

	DrawingLinesAtOnce = 1;

	for( int hap = 0; hap<h; hap++ )
		{
//		RenderText(0,0,x,y+hap,w,DrawingLinesAtOnce);
		RenderText(0,0,x,y,w,(hap*DrawingLinesAtOnce));
		RenderOsdBits(pBuffer, x,y+(hap*DrawingLinesAtOnce),w,h-(hap*DrawingLinesAtOnce));
		DrawingLinesAtOnce = DrawingLinesAtOnce *2;
		}
	
/*
	// Move
	pOsd			= (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024 + (y * m_osdWidth) + x;	
	pBlockStartPoint	= pBuffer -(2*w);
	for (i=0; i<h; i++)
		{
		memcpy (pOsd, pBlockStartPoint, w);
		pBlockStartPoint -= w;
		pOsd += m_osdWidth;
		}
	RefreshOsd();
*/
	
	// Move
/*
	p = ( h ) - 1;
	for( j=0 ; j<p; j++ ) 
		{
		pOsd			= (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024 + (y * m_osdWidth) + x;	
		pBlockStartPoint 	-= ( w );

		for (i=0; i<h; i++)
			{
			memcpy (pOsd, pBlockStartPoint, w);
			pOsd += m_osdWidth;
			}

		RefreshOsd();
		}
*/
#endif

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetOSDDestination (RMint32 x, RMint32 y, RMint32 w, RMint32 h)
{
	Wnd_type Wnd;
	Wnd.x = x;
	Wnd.y = y;
	Wnd.w = w;
	Wnd.h = h;
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evValidWindow, 
		sizeof(Wnd), &Wnd);

	RUA_DECODER_SET_PROPERTY (m_handle, OSD_SET, eOsdDestinationWindow, sizeof (Wnd), &Wnd);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetToggleDisplay( int option )
{
//	SetVideoDestination (0, 0, 320, 240);
//	SetOSDDestination (0, 0, 320, 240);
       SetupDisplay(option);
	return MPEG_DECODER_ERROR_NO_ERROR;

/*
	if ( option )              					// for TV	
		{
//		pMpegDecoder->SetVideoDestination (0, 0, 720, 480);
//		SetVideoDestination(40, 10, 640, 460);
//		SetOSDDestination (40, 10, 640, 460);
//		SetVideoDestination (0, 0, 720, 480);
//		SetOSDDestination (0, 0, 720, 480);
		SetupDisplay (option);

		}
	else									// for lcd
		{
		SetVideoDestination (0, 0, 320, 240);
		SetOSDDestination (0, 0, 320, 250);
		SetupDisplay (option);
		}
*/

}

MPEG_DECODER_ERROR MpegDecoder::ClearBG()
{
    ebiCommand_type command;
    command = ebiCommand_VideoHwBlackFrame;
    RUA_DECODER_SET_PROPERTY (m_handle, BOARDINFO_SET, ebiCommand, sizeof(command), &command);
    return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::SetOutDisplayOption( int option )
{
	evOutDisplayOption_type	OutDisplayOption;
	evInAspectRatio_type		InAspectRatio;
	switch ( option )
	{
		case 0:
			OutDisplayOption = evOutDisplayOption_Normal;
			InAspectRatio = evInAspectRatio_4x3;
			break;
		case 1:
			OutDisplayOption = evOutDisplayOption_16x9to4x3_LetterBox;
			InAspectRatio = evInAspectRatio_16x9;
			break;
		case 2:
			OutDisplayOption = evOutDisplayOption_16x9to4x3_PanScan;
			InAspectRatio = evInAspectRatio_16x9;
			break;
		case 3:
			OutDisplayOption = evOutDisplayOption_4x3to16x9_HorzCenter;
			InAspectRatio = evInAspectRatio_4x3;
			break;
		case 4:
			OutDisplayOption = evOutDisplayOption_4x3to16x9_VertCenter;
			InAspectRatio = evInAspectRatio_4x3;
			break;
	}
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evInAspectRatio,
		sizeof(InAspectRatio), &InAspectRatio);
	RUA_DECODER_SET_PROPERTY (m_handle, VIDEO_SET, evOutDisplayOption, 
		sizeof(OutDisplayOption), &OutDisplayOption);
	return MPEG_DECODER_ERROR_NO_ERROR;

}

#define LEVEL_METER_BAR_WIDTH		20
#define LEVEL_METER_BAR_HEIGHT	1
#define LEVEL_METER_BARS			10
#define LEVEL_METER_UNITS			16
#define GAP							2
MPEG_DECODER_ERROR MpegDecoder::XEN_RenderLevelMeter(RMuint8 palette, RMuint8 bg, RMint32 x, RMint32 y )
{	
	RMint32 i,j;
	RMuint8 *pbits = (RMuint8 *)m_OsdBuffer.framebuffer + 8 + 1024;
	int level;
	int sx;

	pbits += (y * m_osdWidth);
	pbits += x;

	for ( j=LEVEL_METER_UNITS;j>0;j--)
	{
		for( int k=0; k<LEVEL_METER_BAR_HEIGHT; k++ ) {
			for (i=0;i<LEVEL_METER_BARS;i++)
			{
				sx = i*(LEVEL_METER_BAR_WIDTH+GAP);
				if (j <= (*m_pESA)[i] ) {
					memset (pbits+sx, palette-j, LEVEL_METER_BAR_WIDTH );
					//memset (pbits+sx, palette, LEVEL_METER_BAR_WIDTH );
				}
				else
					memset (pbits+sx, bg, LEVEL_METER_BAR_WIDTH);
			}
			pbits += m_osdWidth;
		}
		pbits += m_osdWidth;
	}

	asm ("mcr p15, 0, r0, c7, c10, 0\n");	
	ioctl (m_handle, REALMAGICHWL_IOCTL_OSDFB_REFRESH, 0);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::XEN_GetSpectrumData(void)
{
	decoderproperty Dp;

	Dp.PropId = eXENgetSpectrum;

	ioctl (m_handle, REALMAGICHWL_IOCTL_XEN_GET_PROPERTY, &Dp);

	m_pESA = (long **)Dp.pValue;
	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::XEN_ClearSpectrumData( void )
{
	decoderproperty Dp;

	Dp.PropId = eXENclearESA;
	
	ioctl (m_handle, REALMAGICHWL_IOCTL_XEN_SET_PROPERTY, &Dp);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::XEN_SetContentsProp( int SampleRate )
{
	decoderproperty Dp;
	int	sr;

	sr = SampleRate;
	
	Dp.PropId = eXENsetContentsProp;
	Dp.pValue = &sr;

	ioctl (m_handle, REALMAGICHWL_IOCTL_XEN_SET_PROPERTY, &Dp);

	return MPEG_DECODER_ERROR_NO_ERROR;
}


int MpegDecoder::XEN_Get_UserEQ_BandValue( int BandName )
{
	decoderproperty Dp;
	CMD_t	CMD;
	
	CMD.nMode =  GET_EQPARAMETER;
	CMD.Param.PARAMETER.TypeNo = (ePARAM_TYPE)BandName;

	Dp.PropId = eXENgetEffect;
	Dp.pValue = &CMD;
	
	ioctl (m_handle, REALMAGICHWL_IOCTL_XEN_GET_PROPERTY, &Dp);

	return CMD.Param.PARAMETER.Value;
}

MPEG_DECODER_ERROR MpegDecoder::XEN_Preset( SOUNDEFFECT_t Mode )
{
	decoderproperty Dp;
	SOUNDEFFECT_t tMode;
	
	tMode = Mode;

	Dp.PropId = eXENsetPreset;
	Dp.pValue = &tMode;

	DEBUGMSG (1, (" XEN_Preset =%d\n", Mode ) );

	ioctl (m_handle, REALMAGICHWL_IOCTL_XEN_SET_PROPERTY, &Dp);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::XEN_SetEffect( CMD_t CMD )
{
	decoderproperty Dp;
	CMD_t tCMD;

	tCMD = CMD;
	Dp.PropId = eXENsetEffect;
	Dp.pValue = &tCMD;

	DEBUGMSG (1, (" XEN_SetEffect : Mode=%d, Para1=%d, Para2=%d\n", 
		CMD.nMode, CMD.Param.PARAMETER.TypeNo, CMD.Param.PARAMETER.Value ) );

	ioctl (m_handle, REALMAGICHWL_IOCTL_XEN_SET_PROPERTY, &Dp);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

MPEG_DECODER_ERROR MpegDecoder::XEN_SetPower( int nValue )
{
	decoderproperty Dp;
	int	tValue;

	tValue = nValue;
	
	Dp.PropId = eXENsetPower;
	Dp.pValue = &tValue;

	DEBUGMSG (1, (" XEN_SetPower =%d\n", nValue ) );

	ioctl (m_handle, REALMAGICHWL_IOCTL_XEN_SET_PROPERTY, &Dp);

	return MPEG_DECODER_ERROR_NO_ERROR;
}

