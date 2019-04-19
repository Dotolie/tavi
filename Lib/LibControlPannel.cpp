//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : VideoPlayer Control Pannel Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibControlPannel.cpp,v $
// $Revision: 1.5 $
// $Date: 2006/06/07 08:07:40 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibControlPannel.cpp,v $
// Revision 1.5  2006/06/07 08:07:40  zstein
// - removed unused codes.
//
// Revision 1.4  2006/05/10 02:36:34  zstein
// - deleted unused function:SetPlayTimeOnly().
//
// Revision 1.3  2006/04/07 04:25:55  zstein
// - cleaned up the source.
//
// Revision 1.2  2006/03/09 02:14:28  zstein
// - changed volume interface.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*****************************************************************************

#include "LibControlPannel.h"

#include <string.h>
#include <Component/object.h>
#include <Component/image.h>
#include <Component/timer.h>

#include <Lib/LibImage.h>
#include <Lib/LibScrollBar.h>
#include <Lib/LibDialogBox.h>
#include <Lib/LibBookmarkDialogBox.h>
#include <Lib/LibSetupDialogBox.h>

#include <Lib/LibListBox.h>
#include <Lib/LibFileBrowser.h>
#include <Lib/LibText.h>

#include <Task/tavi_global.h>

#include "LibDefines.h"



//////////////////////////////////////////////////////////////////////////////////////////////////
//
// CControlPannel Class Figure
//
//
extern CTextEngine_BDF* pT;
//extern CSetupDialogBox* pSetupDbx;
extern SysParameter_t	TaviSysParam;
extern CTopPanel*	g_TopPanel;
extern CSubtitle* pSubtitle;


CControlPannel::CControlPannel(  MSG_PROCESSOR pfpMsgProc  )
{
	DEBUGMSG(1,("CControlPannel()\n"));

	memset( CurrentFilename, 0, 256 );
	memset( &pt, 0, sizeof(TAVI_TIME) );
	memset( &tt, 0, sizeof(TAVI_TIME) );

	width					= _SCREEN_WIDTH_;
	height					= _CP_HEIGHT;		//42;
	sx						= 0;
	sy 						= _SCREEN_HEIGHT_ - _CP_HEIGHT - 1;
	fpControlPannelMsgProc 	= pfpMsgProc;
	LeftIconStatus				= 0;
	RightIconStatus			= _CONTROL_PANNEL_SETUP_ICON;
	PlayStatusIconStatus		= _VCTRL_PAUSE;
	WideNormalScreenStatus	= _VSR_OPTIMIZED;
	CaptionStatus				= CAPTION_DISABLED;
	PlaylistRepeatStatus		= (eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS)TaviSysParam.video.playlist_repeat;
	SectionRepeatStatus		= SECTION_REPEAT_DISABLE;
	EqualizerStatus			= (SOUNDEFFECT_t)TaviSysParam.sound.eq;
	isVisible					= FALSE;
	pTAVI->isControlShown	= FALSE;
	ProgressPercentage		= 0;
	isSeekingState			= FALSE;
	
}
void CControlPannel::BuildControlPannelBackground()
{
	//DEBUGMSG(1, (" - BuildControlPannelBackground()\n"));
	DrawBoxWithoutRefresh(
		_CP_BASE_COLOR_PALLETTE_INDEX,	// RMuint8 color
		sx,							// RMuint16 sx
		sy, 							// RMuint16 sy
		width, 						//RMuint16 width
//		height						// RMuint16 height)
		height+1						// RMuint16 height)
		);

	DrawItemWithoutRefresh( &TAVI_IMAGES[CONTROLPANNEL_EDGE], 0, sx, sy, 4,4 );
	DrawItemWithoutRefresh( &TAVI_IMAGES[CONTROLPANNEL_EDGE], 1, sx+width-4, sy, 4,4 );
}
void CControlPannel::BuildPlayStatusIconStatus()
{
	U16 PlayStatusRes;
	//DrawItemWithoutRefresh(	&TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS], PlayStatusIconStatus, sx+33, sy+5, 12, 11 );

	if( (int)PlayStatusIconStatus > (int)_VCTRL_REW )
		PlayStatusIconStatus = _VCTRL_PAUSE;

	DrawItemWithoutRefresh(	
		&TAVI_IMAGES[VIDEO_ICON_PLAY_STATUS], 
		PlayStatusIconStatus, 
		33, 
		ICONYPOS+2, 
		12, 
		11 );
	
	switch((int)PlayStatusIconStatus)
		{
		case _VCTRL_PLAY:
			PlayStatusRes = ML_PLAY;
			break;
			
		case _VCTRL_FF:
			PlayStatusRes = ML_FF;
			break;
			
		case _VCTRL_REW:
			PlayStatusRes = ML_REWIND;
			break;

		case _VCTRL_PAUSE:
		default:
			PlayStatusRes = ML_PAUSE;
			break;
		}
	
	DrawBoxWithoutRefresh(_CP_BASE_COLOR_PALLETTE_INDEX,  sx+52, ICONYPOS+2, 100, 12);
	pT->DrawResText( PlayStatusRes, sx+52, ICONYPOS+2, _WHITE, 12, DT_TRANSPARENCY|DT_CONTROLPANNEL );	

}

void CControlPannel::BuildCaptionStatus()
{
	RMuint16 output;

	if( CaptionStatus == CAPTION_DISABLED )
		output = _VSI_SUBTITLE_DISABLED;
	else if( CaptionStatus == CAPTION_ENABLED )
		output = _VSI_SUBTITLE_ENABLED;
	else
		output = _VSI_SUBTITLE_DISABLED;

		
//	DrawItemWithoutRefresh(	&TAVI_IMAGES[VIDEO_ICON_CAPTIONEXISTS], CaptionStatus, sx+width-(20+36+8+16+8+12+8+12), sy+2+2, 12, 10 );
	DrawItemWithoutRefresh(	
		&TAVI_IMAGES[ICON_REPEAT], 
		output, 
		ICONXPOS_CAPTION,	//sx+width-(20+36+8+16+8+12+8+12), 
		ICONYPOS,			//sy+2+2, 
		ICONWIDTH_CAPTION, 
		ICONHEIGHT 
		);
}

/*

void CControlPannel::BuildWideNormalScreenStatus()
{
	DrawItemWithoutRefresh(
		&TAVI_IMAGES[VIDEO_ICON_SCREENTYPE], 
		WideNormalScreenStatus, 
		ICONXPOS_SCR_STATUS,	//sx+width-(20+36+8+16+8+12), 
		ICONYPOS,			//sy+2+2, 
		ICONWIDTH_SCR_STATUS, 
		ICONHEIGHT );
}
*/

void CControlPannel::BuildPlaylistRepeatOptionStatus()
{
	RMuint16 output;

	if( TaviSysParam.video.playlist_repeat == 0 )
		{
		PlaylistRepeatStatus= PLAYLIST_REPEAT_NOREPEAT;
		output = _VSI_PLAYLIST_REPEAT_NOREPEAT;
		}
	else 
		{
		PlaylistRepeatStatus = PLAYLIST_REPEAT_FOLDER;
		output = _VSI_PLAYLIST_REPEAT_FOLDER;		
		}



	DrawItemWithoutRefresh(
		&TAVI_IMAGES[ICON_REPEAT], 
		output, 
		ICONXPOS_PLAYLIST_REPEAT,	//sx+width-(20+36+8+16), 
		ICONYPOS,			//sy+2, 
		ICONWIDTH_REPEAT, 
		ICONHEIGHT );

}

void CControlPannel::BuildSectionRepeatOptionStatus()
{
	RMuint16 output;

	if( SectionRepeatStatus == SECTION_REPEAT_MARK_A )
		output = _VSI_SECTION_REPEAT_MARK_A;
	else if( SectionRepeatStatus == SECTION_REPEAT_MARK_AB )
		output = _VSI_SECTION_REPEAT_MARK_AB;
	else
		output = _VSI_SECTION_REPEAT_DISABLE;

	DrawItemWithoutRefresh(
		&TAVI_IMAGES[ICON_REPEAT], 
		output, 
		ICONXPOS_SECTION_REPEAT,	//sx+width-(20+36+8+16), 
		ICONYPOS,			//sy+2, 
		ICONWIDTH_REPEAT, 
		ICONHEIGHT );
}

void CControlPannel::BuildSectionRepeatOptionStatus( eVIDEOPLAYER_SECTION_REPEAT_STATUS input )
{
	RMuint16 output;
	
	if( input == SECTION_REPEAT_DISABLE )
		output = _VSI_SECTION_REPEAT_DISABLE;
	else if( input == SECTION_REPEAT_MARK_A )
		output = _VSI_SECTION_REPEAT_MARK_A;
	else if( input == SECTION_REPEAT_MARK_AB )
		output = _VSI_SECTION_REPEAT_MARK_AB;
	else
		return;

	DrawItemWithoutRefresh(
		&TAVI_IMAGES[ICON_REPEAT], 
		output, 
		ICONXPOS_SECTION_REPEAT,	//sx+width-(20+36+8+16), 
		ICONYPOS,			//sy+2, 
		ICONWIDTH_REPEAT, 
		ICONHEIGHT );
}

void CControlPannel::BuildPlaylistRepeatOptionStatus( eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS input )
{
	RMuint16 output;
	
	if( input == PLAYLIST_REPEAT_NOREPEAT )
		output = _VSI_PLAYLIST_REPEAT_NOREPEAT;
/*
	else if( input == PLAYLIST_REPEAT_FOLDER )
		output = _VSI_PLAYLIST_REPEAT_FOLDER;
	else
		return;
*/	
	else
		output = _VSI_PLAYLIST_REPEAT_FOLDER;

	DrawItemWithoutRefresh(
		&TAVI_IMAGES[ICON_REPEAT], 
		output, 
		ICONXPOS_PLAYLIST_REPEAT,	//sx+width-(20+36+8+16), 
		ICONYPOS,			//sy+2, 
		ICONWIDTH_REPEAT, 
		ICONHEIGHT );
}


void CControlPannel::BuildEqualizerStatus()
{
	int EQ_Icon;
	
	EQ_Icon = GetSFX_IconIndexBySFX( EqualizerStatus );

	if( EQ_Icon == 28 || EQ_Icon == 29 ) {
		EQ_Icon == 30;	// UserEffect
	}

	DrawItemWithoutRefresh(	
		&TAVI_IMAGES[ICON_EQ], 
		EQ_Icon,
		ICONXPOS_EQ, 	//sx+width-(20+36), 
		ICONYPOS,		//sy+2+2, 
		ICONWIDTH_EQ, 
		ICONHEIGHT );
	
}

#define PROGRESSBAR_SX 33
#define PROGRESSBAR_EX 287
#define PROGRESSBAR_ESX (PROGRESSBAR_EX-2)
#define PROGRESSBAR_SY 220

int CControlPannel::BuildProgressBar( int eDrawMethod )
{
	static int tppg = -1;

	if( GetCurrentStatus()!=_CONTROL_PANNEL_PROGRESS_MODE ) {
		return FALSE;
	}

	switch ( eDrawMethod ) {
	case _NORMAL:
		tppg = (int)((int)(playtime.SecOnly*100)/(int)totaltime.SecOnly);
		break;

	case _REDRAW:
		tppg = (int)((int)(playtime.SecOnly*100)/(int)totaltime.SecOnly);
		break;

	default:
		ASSERT(0);
	}

	if( playtime.SecOnly != 0 ) {
		SetVisualPlaytime( playtime.SecOnly);
	}


	tt.SecOnly = totaltime.SecOnly;
	ppg = tppg;

	if( ppg == 0 ) {
		DrawBoxWithoutRefresh( 24, PROGRESSBAR_SX-4, PROGRESSBAR_SY-1, 264, 8 );
		DrawBoxWithoutRefresh( 27, PROGRESSBAR_SX, PROGRESSBAR_SY, 256, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 1, PROGRESSBAR_SX, PROGRESSBAR_SY, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, PROGRESSBAR_SX+255-2, PROGRESSBAR_SY, 4, 6 );
	}
	else if( ppg >= 100 ) {
		DrawBoxWithoutRefresh( 24, PROGRESSBAR_SX-4, PROGRESSBAR_SY-1, 264, 8 );
		DrawBoxWithoutRefresh( _WHITE, PROGRESSBAR_SX, PROGRESSBAR_SY, 256, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 3, PROGRESSBAR_SX, PROGRESSBAR_SY, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 2, PROGRESSBAR_SX+255-2, PROGRESSBAR_SY, 4, 6 );
	}
	else {
		DrawBoxWithoutRefresh( 24, PROGRESSBAR_SX-4, PROGRESSBAR_SY-1, 264, 8 );
		DrawBoxWithoutRefresh( 27, PROGRESSBAR_SX, PROGRESSBAR_SY, 256, 6 );
		DrawBoxWithoutRefresh( _WHITE, PROGRESSBAR_SX, PROGRESSBAR_SY, (int)(ppg*2.55), 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 3, PROGRESSBAR_SX, PROGRESSBAR_SY, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, PROGRESSBAR_SX+255-2, PROGRESSBAR_SY, 4, 6 );
	}


	if( isSeekable ) {
		DrawItemWithoutRefresh( 
			&TAVI_IMAGES[TOUCHPAD_VOL_N_SEEK_EDGE], 
			4, 
			PROGRESSBAR_SX + (int)(ppg*2.55) -4,
			PROGRESSBAR_SY-1,
			8, 
			8 
			);
	}

 	return TRUE;
}	


// 16 * 18 * 8 icon
#define COLOR_TOUCHPAD_BASE				24
#define COLOR_TOUCHCONTROL_WHITE			17
#define VOLUMECONTROLBAR_HEIGHT			16
#define VOLUMECONTROLBAR_LEFT_ICON_X		29
#define VOLUMECONTROLBAR_LEFT_X				(VOLUMECONTROLBAR_LEFT_ICON_X + 16)
#define VOLUMECONTROLBAR_RIGHT_ICON_X		( 319 - VOLUMECONTROLBAR_LEFT_ICON_X -16 )
#define VOLUMECONTROLBAR_RIGHT_X			VOLUMECONTROLBAR_RIGHT_ICON_X
#define VOLUMECONTROLBAR_TOP_Y				239 - 4 - VOLUMECONTROLBAR_HEIGHT
#define VOLUMECONTROLBAR_CONTENTS_WIDTH	(VOLUMECONTROLBAR_RIGHT_X-VOLUMECONTROLBAR_LEFT_X)


void CControlPannel::BuildVolumeText( int iVolume )
{
	char sTemp[16];
	sprintf( sTemp, "%d", iVolume );
	DrawBoxWithoutRefresh(24, tx+tw+8, sy+5, 3*16, 16);
	pT->DrawEUC8Text( sTemp, tx+tw+8, sy+5, _GREEN, 12, DT_TRANSPARENCY );
}


static int tpv;
int CControlPannel::BuildVolumeControlBar( eDRAW_METHOD eDrawMethod, int displayVolume )
{
	if( displayVolume == 0 )
		{
		tpv = 0;
		DrawBoxWithoutRefresh( 24, PROGRESSBAR_SX-4, PROGRESSBAR_SY-1, 264, 8 );		
		DrawBoxWithoutRefresh( 27, PROGRESSBAR_SX, PROGRESSBAR_SY, 256, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 1, PROGRESSBAR_SX, PROGRESSBAR_SY, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, PROGRESSBAR_SX+255-2, PROGRESSBAR_SY, 4, 6 );
		}

	else if( displayVolume >= 40 ) {
		tpv = 255;
		DrawBoxWithoutRefresh( 24, PROGRESSBAR_SX-4, PROGRESSBAR_SY-1, 264, 8 );
		DrawBoxWithoutRefresh( _WHITE, PROGRESSBAR_SX, PROGRESSBAR_SY, 256, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 3, PROGRESSBAR_SX, PROGRESSBAR_SY, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 2, PROGRESSBAR_SX+255-2, PROGRESSBAR_SY, 4, 6 );
		}

	else {
		tpv = (int)( (displayVolume+1)*2*1.25*2.55 );
		DrawBoxWithoutRefresh( 24, PROGRESSBAR_SX-4, PROGRESSBAR_SY-1, 264, 8 );		
		DrawBoxWithoutRefresh( 27, PROGRESSBAR_SX, PROGRESSBAR_SY, 256, 6 );
		DrawBoxWithoutRefresh( _WHITE, PROGRESSBAR_SX, PROGRESSBAR_SY, tpv, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 3, PROGRESSBAR_SX, PROGRESSBAR_SY, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, PROGRESSBAR_SX+255-2, PROGRESSBAR_SY, 4, 6 );
		}

	DrawItemWithoutRefresh(  &TAVI_IMAGES[TOUCHPAD_VOL_N_SEEK_EDGE],  4, PROGRESSBAR_SX + tpv -4, PROGRESSBAR_SY-1, 8, 8 );
	return TRUE;
}


#define _SEEK_STEP_TIME 	10
#define _MAXIMU_BARSZIE	207

void CControlPannel::SetVisualPlaytime( RMint64 playtime )
{
	DEBUGMSG(1,("SetVisualPlaytime(%d)\n",playtime));
	pt.SecOnly	= playtime;
	pt.hours 		= pt.SecOnly / 3600;
	pt.minutes 	= (pt.SecOnly - pt.hours * 3600) / 60;
	pt.seconds 	= pt.SecOnly - pt.hours*3600 - pt.minutes*60;
	DEBUGMSG(1,("-pt.SecOnly:%lld\n", pt.SecOnly));	
	DEBUGMSG(1,("-pt.hours:%d\n", pt.hours));
	DEBUGMSG(1,("-pt.minutes:%d\n", pt.minutes));
	DEBUGMSG(1,("-pt.seconds:%d\n", pt.seconds));
}
void CControlPannel::ClearVisualPlaytime( )
{
	DEBUGMSG(1,("ClearVisualPlaytime()\n"));
	pt.SecOnly	= 0;
	pt.hours 		= 0;
	pt.minutes 	= 0;
	pt.seconds 	= 0;
}

int CControlPannel::BuildSeekTimeText( int seektime )
{
		SetVisualPlaytime( seektime );
		DrawPlayTime( &pt, OSD_REFRESH );

	return TRUE;
}

/*
int CControlPannel::BuildSeekControlBar(eDRAW_METHOD eDrawMethod, int iDirection, int CurrentTargetSeekPosition )
{

	int seekposition;
	int barposition;
	DEBUGMSG(1,("CControlPannel::BuildSeekControlBar( %d, %d, %d)\n", eDrawMethod, iDirection, CurrentTargetSeekPosition ));


	DrawBoxWithoutRefresh( 
		COLOR_TOUCHCONTROL_WHITE,
		VOLUMECONTROLBAR_LEFT_X,
		VOLUMECONTROLBAR_TOP_Y,
		VOLUMECONTROLBAR_CONTENTS_WIDTH,
		16
		);


	DrawBoxWithoutRefresh( 21, VOLUMECONTROLBAR_LEFT_X + 6, 
		VOLUMECONTROLBAR_TOP_Y + 7,  VOLUMECONTROLBAR_CONTENTS_WIDTH-12,	1 );

	DrawBoxWithoutRefresh( 21, VOLUMECONTROLBAR_LEFT_X + 6, 
		VOLUMECONTROLBAR_TOP_Y + 8,  VOLUMECONTROLBAR_CONTENTS_WIDTH-12,	1 );

	DrawBoxWithoutRefresh( 21, VOLUMECONTROLBAR_LEFT_X + 6, 
		VOLUMECONTROLBAR_TOP_Y + 9,  VOLUMECONTROLBAR_CONTENTS_WIDTH-12,	1 );
	
	DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCHPAD_VOL_N_SEEK_EDGE], 2, 
		VOLUMECONTROLBAR_LEFT_ICON_X, VOLUMECONTROLBAR_TOP_Y, 16, 16 );
	
	DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCHPAD_VOL_N_SEEK_EDGE], 3, 
		VOLUMECONTROLBAR_RIGHT_ICON_X, VOLUMECONTROLBAR_TOP_Y, 16, 16 );


	if( eDrawMethod == _NORMAL )
		{

		DEBUGMSG(1,(" -eDrawMethod=NORMAL\n"));
		DEBUGMSG(1,(" -playtime.SecOnly=[%lld]\n", playtime.SecOnly));
		DEBUGMSG(1,(" -totaltime.SecOnly=[%lld]\n", totaltime.SecOnly));
		DEBUGMSG(1,(" -((playtime.SecOnly*100) / totaltime.SecOnly) = [%lld]\n", ((playtime.SecOnly*100) / totaltime.SecOnly) ));
		DEBUGMSG(1,(" - (int)(((playtime.SecOnly*100) / totaltime.SecOnly) / 225) = [%d]\n", (int)(((playtime.SecOnly*100) / totaltime.SecOnly) *2.07) ));
		barposition = (int)(((playtime.SecOnly*100) / totaltime.SecOnly) *2.07 );
		seekposition = CurrentTargetSeekPosition;
		}
	else		// == _REDRAW
		{

		if( iDirection == 0 )
			seekposition = CurrentTargetSeekPosition;
		else
			seekposition = CurrentTargetSeekPosition + (iDirection * _SEEK_STEP_TIME);

		barposition = (int)(((seekposition*100) / totaltime.SecOnly) *2.07 );

		DEBUGMSG(1,(" - pre seekposition : %d\n", seekposition ));
		DEBUGMSG(1,(" - pre barposition : %d\n", barposition ));
		
		
		if( seekposition < 9 )
			{
			barposition 	= 0;
			seekposition 	= 0;
			}
		else if( seekposition > totaltime.SecOnly -9 )
			{
			DEBUGMSG(1,(" -  totaltime.SecOnly  : %d\n",  totaltime.SecOnly  ));
			barposition	= _MAXIMU_BARSZIE;
			seekposition 	= totaltime.SecOnly -_SEEK_STEP_TIME ;		// MAX VALUE
			}

		DEBUGMSG(1,(" - seekposition : %d\n", seekposition ));
		DEBUGMSG(1,(" - barposition : %d\n", barposition ));

		}


	DrawItemWithoutRefresh( 
		&TAVI_IMAGES[TOUCHPAD_BALL], 
		0, 
		VOLUMECONTROLBAR_LEFT_X + 3 + barposition,   
		VOLUMECONTROLBAR_TOP_Y+2, 
		12, 
		12 
		);

	return seekposition;
}
*/
static int tps;
int CControlPannel::BuildSeekControlBar(eDRAW_METHOD eDrawMethod, int iDirection, int CurrentTargetSeekPosition )
{
	static int seekposition;
	static int barposition;
//	static int tppg = -1;

	DEBUGMSG(1,("\nBuildSeekControlBar(%d, %d, %d)\n", eDrawMethod, iDirection, CurrentTargetSeekPosition ));



	if( eDrawMethod == _NORMAL )
		{

		DEBUGMSG(1,(" -eDrawMethod=NORMAL\n"));
		DEBUGMSG(1,(" -playtime.SecOnly=[%lld]\n", playtime.SecOnly));
		DEBUGMSG(1,(" -totaltime.SecOnly=[%lld]\n", totaltime.SecOnly));
		DEBUGMSG(1,(" -((playtime.SecOnly*100) / totaltime.SecOnly) = [%lld]\n", ((playtime.SecOnly*100) / totaltime.SecOnly) ));
		DEBUGMSG(1,(" - (int)(((playtime.SecOnly*100) / totaltime.SecOnly) / 225) = [%d]\n", (int)(((playtime.SecOnly*100) / totaltime.SecOnly) *2.55) ));
		barposition = (int)(((playtime.SecOnly*100) / totaltime.SecOnly) *2.55 );
		seekposition = CurrentTargetSeekPosition;
		}

	else		// == _REDRAW
		{

		if( iDirection == 0 )
			seekposition = CurrentTargetSeekPosition;
		else
			seekposition = CurrentTargetSeekPosition + (iDirection * _SEEK_STEP_TIME);

		barposition = (int)((seekposition*100)/totaltime.SecOnly);

		DEBUGMSG(1,(" - pre seekposition : %d\n", seekposition ));
		DEBUGMSG(1,(" - pre barposition : %d\n", barposition ));
		
		
		if( seekposition < 9 )
			{
			barposition 	= 0;
			seekposition 	= 0;
			}
		else if( seekposition > totaltime.SecOnly -9 )
			{
			DEBUGMSG(1,(" -  totaltime.SecOnly  : %d\n",  totaltime.SecOnly  ));
			barposition	= _MAXIMU_BARSZIE;
			seekposition 	= totaltime.SecOnly -_SEEK_STEP_TIME ;		// MAX VALUE
			}

		DEBUGMSG(1,(" - seekposition : %d\n", seekposition ));
		DEBUGMSG(1,(" - barposition : %d\n", barposition ));

		}



	if( barposition == 0 )
		{
		tps = 0;
		DrawBoxWithoutRefresh( 24, PROGRESSBAR_SX-4, PROGRESSBAR_SY-1, 264, 8 );
		DrawBoxWithoutRefresh( 27, PROGRESSBAR_SX, PROGRESSBAR_SY, 256, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 1, PROGRESSBAR_SX, PROGRESSBAR_SY, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, PROGRESSBAR_SX+255-2, PROGRESSBAR_SY, 4, 6 );
		}

	else if( barposition >= 100 ) {
		tps = 255;
		DrawBoxWithoutRefresh( 24, PROGRESSBAR_SX-4, PROGRESSBAR_SY-1, 264, 8 );
		DrawBoxWithoutRefresh( _WHITE, PROGRESSBAR_SX, PROGRESSBAR_SY, 256, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 3, PROGRESSBAR_SX, PROGRESSBAR_SY, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 2, PROGRESSBAR_SX+255-2, PROGRESSBAR_SY, 4, 6 );
		}

	else {
		tps = (int)(barposition*2.55);
		DrawBoxWithoutRefresh( 24, PROGRESSBAR_SX-4, PROGRESSBAR_SY-1, 264, 8 );
		DrawBoxWithoutRefresh( 27, PROGRESSBAR_SX, PROGRESSBAR_SY, 256, 6 );
		DrawBoxWithoutRefresh( _WHITE, PROGRESSBAR_SX, PROGRESSBAR_SY, tps, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 3, PROGRESSBAR_SX, PROGRESSBAR_SY, 4, 6 );
		DrawItemWithoutRefresh( &TAVI_IMAGES[PROGRESSBAR_EDGE], 0, PROGRESSBAR_SX+255-2, PROGRESSBAR_SY, 4, 6 );
		}

	if(isSeekable)
		{
		DrawItemWithoutRefresh( 
			&TAVI_IMAGES[TOUCHPAD_VOL_N_SEEK_EDGE], 
			4, 
			PROGRESSBAR_SX + tps -4,
			PROGRESSBAR_SY-1,
			8, 
			8 
			);
		}

	return seekposition;

}



#define COLOR_MENUBAR_DIV					24
#define MENUBAR_LEFT_ICON_X				29
#define MENUBAR_RIGHT_ICON_X				( 319 - MENUBAR_LEFT_ICON_X )
#define MENUBAR_CONTENTS_WIDTH			( MENUBAR_RIGHT_ICON_X - MENUBAR_LEFT_ICON_X )

#define MENUBAR_SX							29						
#define MENUBAR_EX							( 319  - 29 )
#define MENUBAR_WIDTH						( MENUBAR_EX - MENUBAR_SX )
#define MENUBAR_WIDTH_HALFSIZE			( ( MENUBAR_WIDTH /3 ) /2 )
#define MENUBAR_CENTER_SX					( MENUBAR_SX + ( MENUBAR_WIDTH / 3 ) )
#define MENUBAR_RIGHT_SX					( MENUBAR_SX + ( ( MENUBAR_WIDTH / 3 ) * 2 ) )

//int CControlPannel::BuildMenuBar( char sLeftMenubar[], char sCenterMenubar[], char sRightMenubar[] )
int CControlPannel::BuildMenuBar( int LeftTextRes, int CenterTextRes, int RightTextRes )
{
	int iLeftStringSX 	= 0;
	int iCenterStringSX 	= 0;
	int iRightStringSX 	= 0;


	DrawBoxWithoutRefresh( 
		COLOR_TOUCHCONTROL_WHITE,
		MENUBAR_SX,
		VOLUMECONTROLBAR_TOP_Y,
		MENUBAR_WIDTH,
		16
		);

	DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCHPAD_MENU_EDGE], 2, 
		MENUBAR_SX, VOLUMECONTROLBAR_TOP_Y-1, 8, 18 );
	
	DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCHPAD_MENU_EDGE], 3, 
		MENUBAR_EX-8, VOLUMECONTROLBAR_TOP_Y-1, 8, 18 );

	DrawBoxWithoutRefresh( 
		COLOR_MENUBAR_DIV, 
		MENUBAR_CENTER_SX,				//MENUBAR_SX + (int)(MENUBAR_WIDTH/3),
		VOLUMECONTROLBAR_TOP_Y + 3,
		1,
		10
		);
		
	DrawBoxWithoutRefresh( 
		COLOR_MENUBAR_DIV, 
		MENUBAR_RIGHT_SX,				//MENUBAR_SX + (int)((int)(MENUBAR_WIDTH/3)*2),
		VOLUMECONTROLBAR_TOP_Y + 3,
		1,
		10
		);

	if( LeftTextRes > -1 )
		{
		if((iLeftStringSX = pT->GetStringWidth( LeftTextRes, 12, DT_TRANSPARENCY )) > 0 )
			{
			iLeftStringSX = MENUBAR_SX + (MENUBAR_WIDTH_HALFSIZE - (iLeftStringSX/2));
			pT->DrawResText( LeftTextRes, iLeftStringSX, VOLUMECONTROLBAR_TOP_Y+2 , _BLACK, 12, DT_TRANSPARENCY );
			}
		}


	if( CenterTextRes > -1 )
		{	
		if((iCenterStringSX = pT->GetStringWidth( CenterTextRes, 12, DT_TRANSPARENCY )) > 0 )
			{
			iCenterStringSX = MENUBAR_CENTER_SX + (MENUBAR_WIDTH_HALFSIZE - (iCenterStringSX/2));
			pT->DrawResText( CenterTextRes, iCenterStringSX, VOLUMECONTROLBAR_TOP_Y+2 , _BLACK, 12, DT_TRANSPARENCY );
			}
		}

	if( RightTextRes > -1 )
		{	
		if((iRightStringSX = pT->GetStringWidth( RightTextRes, 12, DT_TRANSPARENCY )) > 0 )
			{
			iRightStringSX = MENUBAR_RIGHT_SX + (MENUBAR_WIDTH_HALFSIZE - (iRightStringSX/2));
			pT->DrawResText( RightTextRes, iRightStringSX, VOLUMECONTROLBAR_TOP_Y+2 , _BLACK, 12, DT_TRANSPARENCY );
			}
		}
	return TRUE;
}



int CControlPannel::SetScreenRateType( eVIDEOPLAYER_SCREEN_RATE_STATUS input )
{

	WideNormalScreenStatus = input;
	TaviSysParam.video.screen = input;

	return TRUE;
}

eVIDEOPLAYER_SCREEN_RATE_STATUS CControlPannel::GetScreenRateType()
{
	return WideNormalScreenStatus;
}



int CControlPannel::SetTime( int object, TAVI_TIME* time )
{
	return FALSE;
}

int CControlPannel::SetFilename( uchar_t filename[] )
{
	ustrcpy( CurrentFilename, filename );
	return FALSE;
}

void CControlPannel::ClearFilename()
{
	memset( CurrentFilename, 0, sizeof(FATFS_MAX_PATHNAME_LENGTH*sizeof(uchar_t)) );
}

int CControlPannel::SetPlayStatus(int status)
{
	PlayStatusIconStatus = (eVIDEOPLAYER_CONTROL_ICONS) status;
	return TRUE;
}

eVIDEOPLAYER_CONTROL_ICONS CControlPannel::GetPlayStatus()
{
	return PlayStatusIconStatus;
}


int CControlPannel::SetCurrenSubtitleOptionStatus(eVIDEOPLAYER_CAPTION_STATUS input)
{ CaptionStatus= input; return TRUE;}
eVIDEOPLAYER_CAPTION_STATUS CControlPannel::GetCurrenSubtitleOptionStatus()
{ return CaptionStatus;}

int CControlPannel::SetCurrentSectionRepeatOptionStatus(eVIDEOPLAYER_SECTION_REPEAT_STATUS input)
{ SectionRepeatStatus = input; return TRUE;}
eVIDEOPLAYER_SECTION_REPEAT_STATUS CControlPannel::GetCurrentSectionRepeatOptionStatus()
{ return SectionRepeatStatus;}

int CControlPannel::SetCurrentPlaylistRepeatOptionStatus(eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS input)
{ PlaylistRepeatStatus = input; return TRUE;}
eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS CControlPannel::GetCurrentPlaylistRepeatOptionStatus()
{return PlaylistRepeatStatus;}


int CControlPannel::SetEqualizerStatus( SOUNDEFFECT_t input )
{
	EqualizerStatus = input;
//	TaviSysParam.sound.eq= input;
	return TRUE;
}

SOUNDEFFECT_t CControlPannel::GetEqualizerStatus()
{
	return EqualizerStatus;
}



int CControlPannel::SetMenuBar( tMENUBAR_OPTION* ptOptionMenuBar)
{
	/*
	메뉴바가... 개수에 상관없이 등록되어야 할 것임...
	메뉴바 Class로 구성해 보도록 하자..77
	*/
	

	return TRUE;
}


void CControlPannel::SetSeekValue( int newvalue ) { SeekValue = newvalue; }
int CControlPannel::GetSeekValue() { printf("(((((((SeekValue:%d)))))))\n", SeekValue ); return SeekValue; }

// Fixed Value On File Format
void CControlPannel::SetSeekable( int input ) { isSeekable = input; }
int CControlPannel::GetSeekable() { /*printf("(((((((isSeekable:%d)))))))\n", isSeekable ); */ return isSeekable; }

// Variable Value On Video Play Status
void CControlPannel::SetSeekingState( int input ) { isSeekingState = input; }
int CControlPannel::IsSeekingState() { return isSeekingState; }

int CControlPannel::RedrawPlayStatus( eVIDEOPLAYER_CONTROL_ICONS status, eOSD_REFRESH_OPTION refresh )
{
	PlayStatusIconStatus = status;
	BuildPlayStatusIconStatus();
	if( refresh )
		pMpegDecoder->RefreshOsd();
	return TRUE;
}

/*
typedef enum
{
	_CONTROL_PANNEL_PROGRESS_MODE,
	_CONTROL_PANNEL_VOLUME_MODE,
	_CONTROL_PANNEL_SUBMENU_MODE,
	_CONTROL_PANNEL_SUBMENU_CAPTION_MODE,
	_CONTROL_PANNEL_SEEK_MODE
} eCONTROL_PANNEL_STATUS;
*/

#if 0
int CControlPannel::Show()
{
	//RegisterTimerHandler
	if( isVisible )
		{

		eCONTROL_PANNEL_STATUS eNewState;

		switch( CurrentStatus )
			{

			case _CONTROL_PANNEL_SUBMENU_MODE:
				// we must go _CONTROL_PANNEL_SEEK_MODE
				eNewState = _CONTROL_PANNEL_VOLUME_MODE;
				break;

			case _CONTROL_PANNEL_VOLUME_MODE:
				// we must go _CONTROL_PANNEL_PROGRESS_MODE
				//eNewState = _CONTROL_PANNEL_SEEK_MODE;
				eNewState = _CONTROL_PANNEL_PROGRESS_MODE;
				break;
				
			case _CONTROL_PANNEL_PROGRESS_MODE:
				// we must go _CONTROL_PANNEL_SUBMENU_MODE
				eNewState = _CONTROL_PANNEL_SUBMENU_MODE;
				break;


			case _CONTROL_PANNEL_SEEK_MODE:
				// we must go _CONTROL_PANNEL_VOLUME_MODE
				eNewState = _CONTROL_PANNEL_SUBMENU_MODE;
				break;
				
			default:
				eNewState = _CONTROL_PANNEL_SUBMENU_MODE;
				ASSERT(0);
			}


		return( Show( eNewState ) );

		}

	else
		{
		return( Show( _CONTROL_PANNEL_PROGRESS_MODE ) );
		}
	
}
#else
int CControlPannel::Show()
{
	//RegisterTimerHandler

	if( isVisible )
		{

		eCONTROL_PANNEL_STATUS eNewState;

		switch( CurrentStatus )
			{
/*
			case _CONTROL_PANNEL_SUBMENU_MODE:
				// we must go _CONTROL_PANNEL_SEEK_MODE
				eNewState = _CONTROL_PANNEL_VOLUME_MODE;
				break;
*/
			case _CONTROL_PANNEL_VOLUME_MODE:
				// we must go _CONTROL_PANNEL_PROGRESS_MODE
				//eNewState = _CONTROL_PANNEL_SEEK_MODE;
				eNewState = _CONTROL_PANNEL_PROGRESS_MODE;
				break;
				
			case _CONTROL_PANNEL_PROGRESS_MODE:
				// we must go _CONTROL_PANNEL_SUBMENU_MODE
				eNewState = _CONTROL_PANNEL_VOLUME_MODE;
				break;
/*

			case _CONTROL_PANNEL_SEEK_MODE:
				// we must go _CONTROL_PANNEL_VOLUME_MODE
				eNewState = _CONTROL_PANNEL_SUBMENU_MODE;
				break;

			default:
				eNewState = _CONTROL_PANNEL_SUBMENU_MODE;
				ASSERT(0);
			}

*/				
			default:
				eNewState = _CONTROL_PANNEL_PROGRESS_MODE;
				ASSERT(0);
			}


		return( Show( eNewState ) );

		}

	else
		{
		return( Show( CurrentStatus ) );
		}
}
#endif


int CControlPannel::Reset()
{
	memset( CurrentFilename, 0, sizeof(CurrentFilename) );
	memset( &pt, 0, sizeof(TAVI_TIME) );
	memset( &tt, 0, sizeof(TAVI_TIME) );

	ClearVisualPlaytime();

	width					= _SCREEN_WIDTH_;
	height					= 42;
	sx						= 0;
	sy 						= _SCREEN_HEIGHT_ - height -1;
	LeftIconStatus				= 0;
	RightIconStatus			= _CONTROL_PANNEL_SETUP_ICON;
	PlayStatusIconStatus		= _VCTRL_PAUSE;
	WideNormalScreenStatus	= _VSR_OPTIMIZED;
	CaptionStatus				= CAPTION_DISABLED;
	PlaylistRepeatStatus		= (eVIDEOPLAYER_PLAYLIST_REPEAT_STATUS)TaviSysParam.video.playlist_repeat;
	SectionRepeatStatus		= SECTION_REPEAT_DISABLE;
	EqualizerStatus			= (SOUNDEFFECT_t)TaviSysParam.sound.eq;
	isVisible					= FALSE;
	ProgressPercentage		= 0;
	isSeekingState			= FALSE;	

	return TRUE;
}


int CControlPannel::Show( eCONTROL_PANNEL_STATUS eNewState )
{
	Show( eNewState, 0 );
}


int CControlPannel::Show( eCONTROL_PANNEL_STATUS eNewState, RMuint32 ClearOption)
{
	if( ClearOption == CO_SKIP_CAPTION ) {
		ClearOsd();
	}
	else {
		if( pSubtitle && !isVisible ) {
			RMuint8* pOsdTemp = (RMuint8*)malloc( 320*240*sizeof(RMuint8));
			pMpegDecoder->OsdBlockCopy( pOsdTemp, 0,height,width,240-height );
			ClearOsd();
			pMpegDecoder->OsdBlockPaste( 0, 0,width,240-height, pOsdTemp );
			free(pOsdTemp);
			pOsdTemp = NULL;
		}
		else if( !isVisible ) {
			ClearOsd();
		}
  }	
	
	CurrentStatus = eNewState;

	Reset_VideoControlPannelTimer();

    // Draw top-panel.
	g_TopPanel->SetTitle( (char*)CurrentFilename, true );
	g_TopPanel->SetVisible( true );
	g_TopPanel->Update();
	// Clear Control Pannel Background
	BuildControlPannelBackground();

	switch( CurrentStatus ) {
	case _CONTROL_PANNEL_PROGRESS_MODE:
		SetTaviKeyMode( TK_LONG|TK_SINGLE );
		BuildPlayStatusIconStatus();
		BuildEqualizerStatus();
		BuildPlaylistRepeatOptionStatus();
		BuildSectionRepeatOptionStatus();
		BuildCaptionStatus();

		if(pt.SecOnly == 0 ) {
			SetVisualPlaytime( playtime.SecOnly );
		}

		BuildSeekControlBar(_REDRAW, 0, pt.SecOnly );
		DrawPlayTime( &pt, SKIP_OSD_REFRESH );
		DrawTotalTime( &totaltime, SKIP_OSD_REFRESH );
		break;

	case _CONTROL_PANNEL_SEEK_MODE:
		SetTaviKeyMode( TK_LONG|TK_VARIOUS);
		BuildPlayStatusIconStatus();
		BuildEqualizerStatus();
		BuildPlaylistRepeatOptionStatus();
		BuildSectionRepeatOptionStatus();
		BuildCaptionStatus();

		DEBUGMSG(1,("((s4))"));
		SetVisualPlaytime( playtime.SecOnly );
		BuildSeekControlBar(_REDRAW, 0, pt.SecOnly );
		DrawTotalTime( &totaltime, SKIP_OSD_REFRESH );
		DEBUGMSG(1,("s4"));
		DrawPlayTime(&pt, SKIP_OSD_REFRESH );
		break;

	case _CONTROL_PANNEL_VOLUME_MODE:
		SetTaviKeyMode( TK_LONG|TK_VARIOUS );

		tw 				= pT->GetStringWidth( ML_VOLUME, 12, 0 );
		tx 				= 160-( (tw+(9*2))/2 );

			pT->DrawResText( ML_VOLUME, tx, sy+5, _WHITE, 12, DT_TRANSPARENCY );
		BuildVolumeText( g_SndCtrl->GetVolume() );
		BuildVolumeControlBar(_NORMAL, g_SndCtrl->GetVolume());	

		DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCH_MINI_ICON], 0, PROGRESSBAR_SX, 229, 12, 7 ); 
		DrawItemWithoutRefresh( &TAVI_IMAGES[TOUCH_MINI_ICON], 1, PROGRESSBAR_SX+255-12, 229, 12, 7 ); 
		break;


	default:
		ASSERT(0);
	}

	pMpegDecoder->RefreshOsd();
	isVisible = TRUE;
	pTAVI->isControlShown = TRUE;
	DEBUGMSG(1,("END of [pCp->Show()]\n"));
	
	return isVisible;
}

int CControlPannel::Hide()
{
	g_TopPanel->SetVisible( false );
	pMpegDecoder->RenderText(0, 0, sx, sy, width, height );
	isVisible = FALSE;
	pTAVI->isControlShown = FALSE;
	return isVisible;
}

int CControlPannel::Hide( int* VP_HideIndicator, int ClearOption )
{
	g_TopPanel->SetVisible( false );

	if( pSubtitle && isVisible )
		{
		RMuint8* pOsdTemp = (RMuint8*)malloc( 320*240*sizeof(RMuint8));
		pMpegDecoder->OsdBlockCopy( pOsdTemp, 0,30,width,239-30-height );
		ClearOsd();
		pMpegDecoder->OsdBlockPaste( 0, 30+height,width,240-30-height, pOsdTemp );
		free(pOsdTemp);
		pOsdTemp = NULL;
		pMpegDecoder->RefreshOsd();
		}
	else if( ClearOption )
		{
		ClearOsd();
		pMpegDecoder->RefreshOsd();
		}
	
	else
		pMpegDecoder->RenderText(0, 0, sx, sy, width, height );

	SetTaviTouchpadMode( MEP_REPORT_RELATIVE );
	
	VP_HideIndicator = FALSE;
	isVisible = FALSE;
	pTAVI->isControlShown = FALSE;
	return isVisible;
}

int CControlPannel::SlidingHide()
{
	isVisible = FALSE;
	pTAVI->isControlShown = FALSE;
	pMpegDecoder->RenderText(0,0,0,0,320,50);
	pMpegDecoder->OsdBlockSlideDown( 0, (240-42-1), 320, 42);

	return isVisible;
}

int CControlPannel::MsgBtnUp( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	return ( fpControlPannelMsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
}

eCONTROL_PANNEL_STATUS CControlPannel::GetCurrentStatus()
{
	return CurrentStatus;
}

RMint64 CControlPannel::GetPlaySeconds()
{
	return pt.SecOnly;
}

