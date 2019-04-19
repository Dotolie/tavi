//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CSoundPaletteDbx Class Source File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSoundPaletteDbx.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:16:23 $
// $Revision: 1.2 $
// $Date: 2006/06/07 08:16:23 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSoundPaletteDbx.cpp,v $
// Revision 1.2  2006/06/07 08:16:23  zstein
// - removed unused codes.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.32  2005/12/26 09:48:49  yong
// Sorry, typo in JOG2_*
//
// Revision 1.31  2005/12/26 09:46:53  yong
// add JOG2_UP/DOWN/ENTER
//
// Revision 1.30  2005/12/26 09:32:59  etnlwind
// Update for exit via TAVI_KEY_JOG1_ENTER
//
// Revision 1.29  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.28  2005/12/02 11:06:13  etnlwind
// Update for SoundPalette UI
//
// Revision 1.27  2005/12/01 09:32:55  etnlwind
// Update for LibSoundPalette
//
// Revision 1.26  2005/12/01 08:40:18  etnlwind
// Update for EQ UI
//
// Revision 1.25  2005/12/01 05:06:17  etnlwind
// *** empty log message ***
//
// Revision 1.24  2005/12/01 02:47:11  etnlwind
// Update for SoundPalette User-Effect Ordering Changes
//
// Revision 1.23  2005/11/30 10:03:41  etnlwind
// Update for UserEQ and User-Effect UI
//
// Revision 1.22  2005/11/25 13:52:47  etnlwind
// Update for New EQ UI
//
// Revision 1.21  2005/11/25 08:40:40  etnlwind
// Update for New EQ
//
// Revision 1.20  2005/11/25 05:10:50  etnlwind
// Update for New EQ
//
// Revision 1.19  2005/11/24 10:56:46  etnlwind
// Update for New EQ
//
// Revision 1.18  2005/11/23 09:33:53  etnlwind
// Update for TAVI-EQ
//
// Revision 1.17  2005/11/22 08:11:23  etnlwind
// Update for New EQ UI
//
// Revision 1.16  2005/11/22 04:55:07  etnlwind
// Update for SoundPalette New UI
//
// Revision 1.15  2005/11/21 12:00:29  etnlwind
// Update for AlarmClock & Sound Palette UI
//
// Revision 1.14  2005/11/21 02:51:32  etnlwind
// Update for EQ Select UI
//
// Revision 1.13  2005/11/11 04:25:58  etnlwind
// Update for VideoPlayer Screen Clean Method
//
// Revision 1.12  2005/11/08 07:16:12  etnlwind
// 전반
// -EXIT키 용도 재정의
// -서브메뉴에서 Close, Back삭제
//
// Revision 1.11  2005/09/08 02:39:11  ywkim
// Add preset of sound palettes with acoustic,latin,lecture,electronic,hiphop
//
// Revision 1.10  2005/08/19 05:45:41  etnlwind
// Update for VideoPlayer EQ BugFix
//
// Revision 1.9  2005/08/18 13:08:23  etnlwind
// Update for Sound Palette Dbx
//
// Revision 1.8  2005/08/18 12:35:28  etnlwind
// Update for VideoPlayer EQ Dbx
//
// Revision 1.7  2005/08/18 10:38:18  etnlwind
// Update for Video Player Sound Palette Force Closing
//
// Revision 1.6  2005/08/17 08:16:49  etnlwind
// Update for Sound Palette
//
// Revision 1.5  2005/08/17 03:48:02  etnlwind
// Update for Sound Palette Related Func.
//
// Revision 1.4  2005/08/16 05:28:02  etnlwind
// Update for VideoPlayer
//
// Revision 1.3  2005/08/12 11:17:24  etnlwind
// Update for Sound Palette
//
// Revision 1.2  2005/08/10 09:54:00  etnlwind
// Update for Video Player Sound Palette Dbx
//
// Revision 1.1  2005/08/09 10:52:09  etnlwind
// Update for
// 1. CSoundPaletteDbx Added
// 2. Video Control Pannel UI changed
//
//
//*****************************************************************************



#include "LibSoundPaletteDbx.h"
#include <3Dsound/3Dsound.h>
#include "LibDefines.h"
#include <Task/tavi_global.h>
/*
typedef enum {
	SOUND_NORMAL = 0,
	SOUND_eXTX_PRESET,
	SOUND_MEX_BASS_PRESET,
	SOUND_MEX_LITE_PRESET,
	SOUND_MEX_PRO_PRESET,
	SOUND_MEX_USER,
	SOUND_REX_MILD_PRESET,
	SOUND_REX_WIDE_PRESET,
	SOUND_REX_LIVE_PRESET,
	SOUND_REX_WIDE_USER,
	SOUND_REX_LIVE_USER,	
	SOUND_SEQ_FLAT,
	SOUND_SEQ_VOICE,
	SOUND_SEQ_POWERBASS,
	SOUND_SEQ_BRIGHT,
	SOUND_SEQ_DARKWARM,
	SOUND_SEQ_CLASSIC,
	SOUND_SEQ_HARDROCK,
	SOUND_SEQ_SOFTROCK,
	SOUND_SEQ_JAZZ,
	SOUND_SEQ_DANCE,
	SOUND_SEQ_SLOW,
	SOUND_SEQ_POP,
	SOUND_SEQ_BLUES,
	SOUND_SEQ_INSTRUMENTAL,
	SOUND_SEQ_FUNK,
	SOUND_SEQ_USER,
	SOUND_SEQ_10BAND,
	SOUND_SEQ_MAXCOUNT
} SOUNDEFFECT_t;
*/

extern CTextEngine_BDF* pT;
extern SysParameter_t TaviSysParam;
extern CTopPanel* g_TopPanel;
/*
extern SOUNDEFFECT_t VP_GetSFXValue( int input );
extern int VP_GetSFXMacroValue( SOUNDEFFECT_t input );
extern int VP_GetSFXMacroIconIndexValue( int input );
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// CSoundPaletteDbx Class Figure
//
//

#if 1
#define U EQBOX_UNSELECTED_BASE_COLOR
#define S EQBOX_SELECTED_BASE_COLOR
#define B _CP_BASE_COLOR_PALLETTE_INDEX

static RMuint8 U_LeftArrow[ARROW_SIZE] = 
{
B,B,B,B,B,B,U,U,
B,B,B,B,B,U,U,U,
B,B,B,B,U,U,U,U,
B,B,B,U,U,U,U,U,
B,B,U,U,U,U,U,U,
B,B,B,U,U,U,U,U,
B,B,B,B,U,U,U,U,
B,B,B,B,B,U,U,U,
B,B,B,B,B,B,U,U
};

static RMuint8 U_RightArrow[ARROW_SIZE] = 
{
U,U,B,B, B,B,B,B,
U,U,U,B, B,B,B,B,
U,U,U,U, B,B,B,B,
U,U,U,U, U,B,B,B,
U,U,U,U, U,U,B,B,
U,U,U,U, U,B,B,B,
U,U,U,U, B,B,B,B,
U,U,U,B, B,B,B,B,
U,U,B,B, B,B,B,B 
};

static RMuint8 S_LeftArrow[ARROW_SIZE] = 
{
B,B,B,B,B,B,S,S,
B,B,B,B,B,S,S,S,
B,B,B,B,S,S,S,S,
B,B,B,S,S,S,S,S,
B,B,S,S,S,S,S,S,
B,B,B,S,S,S,S,S,
B,B,B,B,S,S,S,S,
B,B,B,B,B,S,S,S,
B,B,B,B,B,B,S,S
};

static RMuint8 S_RightArrow[ARROW_SIZE] = 
{
S,S,B,B, B,B,B,B,
S,S,S,B, B,B,B,B,
S,S,S,S, B,B,B,B,
S,S,S,S, S,B,B,B,
S,S,S,S, S,S,B,B,
S,S,S,S, S,B,B,B,
S,S,S,S, B,B,B,B,
S,S,S,B, B,B,B,B,
S,S,B,B, B,B,B,B 
};


#endif


CSoundPaletteDbx::CSoundPaletteDbx( U32 DbxTitleRes, int x, int y, int w, int h, SETUP_PROCESSOR fpSetupProc, U32 option )
{
	DEBUGMSG(1,("CFileDbx::CFileDbx( U32 DbxTitleRes, int x, int y, int w, int h, U32 option )\n"));
	DEBUGMSG(1,("-DbxTitleRes: %d\n-x: %d\n-y: %d\n-w: %d\n-h: %d\n-option: 0x%x\n", DbxTitleRes, x, y, w, h, option ));

	sx = x;
	sy = y;
	width = w;
	height = h;
//	BtnOption = option;
//	TitleRes = DbxTitleRes;
	PrevSoundEQ = TaviSysParam.sound.eq;
	m_fpSetupProc = fpSetupProc;
//	pSetup2BEQDbx = NULL;
//	pSetup5BEQDbx = NULL;

#if 1
	CreateEQList();
#else

	AddItem( ML_NORMAL, SOUND_NORMAL );

	for( int Effect=ML_SOUND_PALETTE_TRUE_3D_MILD; Effect<=ML_SOUND_PALETTE_MY_EQ; Effect++ )
		{
			AddItem( Effect, GetSFX_ValueByMacro(Effect) );
/*
		if( Effect == ML_SOUND_PALETTE_ACOUSTIC ||
			Effect == ML_SOUND_PALETTE_ELECTRONIC ||
			Effect == ML_SOUND_PALETTE_HIPHOP ||
			Effect == ML_SOUND_PALETTE_LATIN ||
			Effect == ML_SOUND_PALETTE_LECTURE )
			{
				;	// Not Ready
			}
		else
			AddItem( Effect, GetSFX_ValueByMacro(Effect) );
*/
		}

	SetTitle( TitleRes );
#endif

}

CSoundPaletteDbx::~CSoundPaletteDbx()
{
	if( pSetup2BEQDbx !=NULL )
		delete pSetup2BEQDbx;

	if( pSetup5BEQDbx !=NULL )
		delete pSetup5BEQDbx;	

	if( pSb1 != NULL );
		delete pSb1;

	//ItemList.FreeAll();
	EQList.FreeAll();
}


void CSoundPaletteDbx::BuildBowCursor( RMuint32 option )
{
	RMuint8* LeftArrow;
	RMuint8* RightArrow;

	if( (option&LEFT_NORMAL) == LEFT_NORMAL )
		LeftArrow = U_LeftArrow;
	else if( (option&LEFT_SELECT) == LEFT_SELECT )
		LeftArrow = S_LeftArrow;
		
	if( (option&RIGHT_NORMAL) == RIGHT_NORMAL )
		RightArrow = U_RightArrow;
	else if( (option&RIGHT_SELECT) == RIGHT_SELECT )
		RightArrow = S_RightArrow;

	pMpegDecoder->RenderOsdBits_WithoutRefresh( LeftArrow, 0, 216, 8, 9 );
	pMpegDecoder->RenderOsdBits_WithoutRefresh( RightArrow, (319-9), 216, 8, 9 );

}


void CSoundPaletteDbx::BuildRoundBox(RMuint8 color, RMuint16 sx, RMuint16 sy, RMuint16 width, RMuint16 height)
{
	DrawBoxWithoutRefresh( color, sx+1, sy, width-2, 1);
	DrawBoxWithoutRefresh( color, sx, sy+1, width, height-2);
	DrawBoxWithoutRefresh( color, sx+1, sy+height-1, width-2, 1);
}

int CSoundPaletteDbx::ApplyUserSet()
{
	int UserFx_ResId;
	SOUNDEFFECT_t UserFx_Sfx;
	char UserFx_Title[16];


	/////////////////////////////////////////////////////////////////
	// Apply User-Effect Changes
	switch(TaviSysParam.sound.user_effect)
		{
		case 1:	
			UserFx_ResId = ML_SOUND_PALETTE_MY_3D_WIDE;
			UserFx_Sfx = SOUND_REX_WIDE_USER;
			sprintf( UserFx_Title, "3D.WIDE" ); //var
			break;
			
		case 2:	
			UserFx_ResId = ML_SOUND_PALETTE_MY_3D_LIVE;		
			UserFx_Sfx = SOUND_REX_LIVE_USER; 
			sprintf( UserFx_Title, "3D.LIVE" ); //var
			break;

		default:
		case 0:	
			UserFx_ResId= ML_SOUND_PALETTE_MY_REAL_SOUND;	
			UserFx_Sfx = SOUND_MEX_USER; 
			sprintf( UserFx_Title, "REAL.SOUND" ); //var
			break;
		}

	EQList[EQList.Count()-2]->ResId = UserFx_ResId;
	EQList[EQList.Count()-2]->Sfx = UserFx_Sfx;
	strcpy( EQList[EQList.Count()-2]->Tittle, UserFx_Title );


	/////////////////////////////////////////////////////////////////
	// Apply User-EQ Changes
	EQList[EQList.Count()-1]->p125 = TaviSysParam.sound.band0-9;
	EQList[EQList.Count()-1]->p250 = TaviSysParam.sound.band1-9;
	EQList[EQList.Count()-1]->p1000 = TaviSysParam.sound.band2-9;
	EQList[EQList.Count()-1]->p4000 = TaviSysParam.sound.band3-9;
	EQList[EQList.Count()-1]->p8000 = TaviSysParam.sound.band4-9;

	return TRUE;
}

int CSoundPaletteDbx::CreateEQList()
{
	NORMAL = new EQSet;
	WIDE = new EQSet;
	MILD = new EQSet;
	LIVE = new EQSet;
	SPEAKER = new EQSet;
	BASS = new EQSet;
	LITE = new EQSet;
	PRO = new EQSet;
	ACOUSTIC = new EQSet;
	BRIGHT = new EQSet;
	CLASSICAL = new EQSet;
	DANCE = new EQSet;
	DEEPWARM = new EQSet;
	ELECTRONIC = new EQSet;
	FLAT = new EQSet;
	FUNK = new EQSet;
	HARDROCK = new EQSet;
	HIPHOP = new EQSet;
	INSTRUMENTAL = new EQSet;
	JAZZ = new EQSet;
	LATIN = new EQSet;
	LECTURE = new EQSet;
	POP = new EQSet;
	RNB = new EQSet;
	SLOW = new EQSet;
	SOFTROCK = new EQSet;
	VOCAL = new EQSet;
	XBASS = new EQSet;
	USERFX = new EQSet;
	USEREQ = new EQSet;

	MILD->ResId = ML_SOUND_PALETTE_TRUE_3D_MILD;
	MILD->Sfx = SOUND_REX_MILD_PRESET;
	sprintf( MILD->Tittle, "TRUE.3D" );
	sprintf( MILD->Name, "MILD" );
	MILD->p125 = 0;
	MILD->p250 = 0;
	MILD->p1000 = 0;
	MILD->p4000 = 0;
	MILD->p8000 = 0;
	EQList.Add(MILD);

	WIDE->ResId = ML_SOUND_PALETTE_TRUE_3D_WIDE;
	WIDE->Sfx = SOUND_REX_WIDE_PRESET;
	sprintf( WIDE->Tittle, "TRUE.3D" );
	sprintf( WIDE->Name, "WIDE" );
	WIDE->p125 = 0;
	WIDE->p250 = 0;
	WIDE->p1000 = 0;
	WIDE->p4000 = 0;
	WIDE->p8000 = 0;
	EQList.Add(WIDE);

	LIVE->ResId = ML_SOUND_PALETTE_TRUE_3D_LIVE;
	LIVE->Sfx = SOUND_REX_LIVE_PRESET;
	sprintf( LIVE->Tittle, "TRUE.3D" );
	sprintf( LIVE->Name, "LIVE" );
	LIVE->p125 = 0;
	LIVE->p250 = 0;
	LIVE->p1000 = 0;
	LIVE->p4000 = 0;
	LIVE->p8000 = 0;
	EQList.Add(LIVE);

	SPEAKER->ResId = ML_SOUND_PALETTE_TRUE_3D_SPKR;
	SPEAKER->Sfx = SOUND_eXTX_PRESET;
	sprintf( SPEAKER->Tittle, "TRUE.3D" );
	sprintf( SPEAKER->Name, "SPEAKER" );
	SPEAKER->p125 = 0;
	SPEAKER->p250 = 0;
	SPEAKER->p1000 = 0;
	SPEAKER->p4000 = 0;
	SPEAKER->p8000 = 0;
	EQList.Add(SPEAKER);
	

	NORMAL->ResId = ML_NORMAL;
	NORMAL->Sfx = SOUND_NORMAL;
	sprintf( NORMAL->Tittle, "..." );
	sprintf( NORMAL->Name, "NORMAL" );
	NORMAL->p125 = 0;
	NORMAL->p250 = 0;
	NORMAL->p1000 = 0;
	NORMAL->p4000 = 0;
	NORMAL->p8000 = 0;
	EQList.Add(NORMAL);	


	PRO->ResId = ML_SOUND_PALETTE_REAL_PRO;
	PRO->Sfx = SOUND_MEX_PRO_PRESET;	
	sprintf( PRO->Tittle, "REAL.SOUND" );
	sprintf( PRO->Name, "PRO" );
	PRO->p125 = 0;
	PRO->p250 = 0;
	PRO->p1000 = 0;
	PRO->p4000 = 0;
	PRO->p8000 = 0;
	EQList.Add(PRO);
	
	BASS->ResId = ML_SOUND_PALETTE_REAL_BASS;
	BASS->Sfx = SOUND_MEX_BASS_PRESET;	
	sprintf( BASS->Tittle, "REAL.SOUND" );
	sprintf( BASS->Name, "BASS" );
	BASS->p125 = 0;
	BASS->p250 = 0;
	BASS->p1000 = 0;
	BASS->p4000 = 0;
	BASS->p8000 = 0;
	EQList.Add(BASS);

	LITE->ResId = ML_SOUND_PALETTE_REAL_LITE;
	LITE->Sfx = SOUND_MEX_LITE_PRESET;	
	sprintf( LITE->Tittle, "REAL.SOUND" );
	sprintf( LITE->Name, "LITE" );
	LITE->p125 = 0;
	LITE->p250 = 0;
	LITE->p1000 = 0;
	LITE->p4000 = 0;
	LITE->p8000 = 0;
	EQList.Add(LITE);


	ACOUSTIC->ResId = ML_SOUND_PALETTE_ACOUSTIC;
	ACOUSTIC->Sfx = SOUND_SEQ_ACOUSTIC;
	sprintf( ACOUSTIC->Tittle, "PRESET" );
	sprintf( ACOUSTIC->Name, "ACOUSTIC" );
	ACOUSTIC->p125 = 1;
	ACOUSTIC->p250 = 6;
	ACOUSTIC->p1000 = 5;
	ACOUSTIC->p4000 = 2;
	ACOUSTIC->p8000 = 0;
	EQList.Add(ACOUSTIC);

	BRIGHT->ResId = ML_SOUND_PALETTE_BRIGHT;
	BRIGHT->Sfx = SOUND_SEQ_BRIGHT;
	sprintf( BRIGHT->Tittle, "PRESET" );
	sprintf( BRIGHT->Name, "BRIGHT" );
	BRIGHT->p125 = -2;
	BRIGHT->p250 = -1;
	BRIGHT->p1000 = 0;
	BRIGHT->p4000 = 4;
	BRIGHT->p8000 = 5;
	EQList.Add(BRIGHT);

	CLASSICAL->ResId = ML_SOUND_PALETTE_CLASSICAL;
	CLASSICAL->Sfx = SOUND_SEQ_CLASSIC;
	sprintf( CLASSICAL->Tittle, "PRESET" );
	sprintf( CLASSICAL->Name, "CLASSICAL" );
	CLASSICAL->p125 = 4;
	CLASSICAL->p250 = 8;
	CLASSICAL->p1000 = 0;
	CLASSICAL->p4000 = -2;
	CLASSICAL->p8000 = -3;
	EQList.Add(CLASSICAL);

	DANCE->ResId = ML_SOUND_PALETTE_DANCE;
	DANCE->Sfx = SOUND_SEQ_CLASSIC;
	sprintf( DANCE->Tittle, "PRESET" );
	sprintf( DANCE->Name, "DANCE" );
	DANCE->p125 = 8;
	DANCE->p250 = -3;
	DANCE->p1000 = 4;
	DANCE->p4000 = -2;
	DANCE->p8000 = 2;
	EQList.Add(DANCE);

	DEEPWARM->ResId = ML_SOUND_PALETTE_DEEPWARM;
	DEEPWARM->Sfx = SOUND_SEQ_DARKWARM;
	sprintf( DEEPWARM->Tittle, "PRESET" );
	sprintf( DEEPWARM->Name, "DEEPWARM" );
	DEEPWARM->p125 = 1;
	DEEPWARM->p250 = 1;
	DEEPWARM->p1000 = 0;
	DEEPWARM->p4000 = -4;
	DEEPWARM->p8000 = -6;
	EQList.Add(DEEPWARM);

	ELECTRONIC->ResId = ML_SOUND_PALETTE_ELECTRONIC;
	ELECTRONIC->Sfx = SOUND_SEQ_ELECTRONIC;
	sprintf( ELECTRONIC->Tittle, "PRESET" );
	sprintf( ELECTRONIC->Name, "ELECTRONIC" );
	ELECTRONIC->p125 = -1;
	ELECTRONIC->p250 = 6;
	ELECTRONIC->p1000 = 4;
	ELECTRONIC->p4000 = -2;
	ELECTRONIC->p8000 = 2;
	EQList.Add(ELECTRONIC);

	FLAT->ResId = ML_SOUND_PALETTE_FLAT;
	FLAT->Sfx = SOUND_SEQ_FLAT;
	sprintf( FLAT->Tittle, "PRESET" );
	sprintf( FLAT->Name, "FLAT" );
	FLAT->p125 = 0;
	FLAT->p250 = 0;
	FLAT->p1000 = 0;
	FLAT->p4000 = 0;
	FLAT->p8000 = 0;
	EQList.Add(FLAT);

	FUNK->ResId = ML_SOUND_PALETTE_FUNK;
	FUNK->Sfx = SOUND_SEQ_FUNK;
	sprintf( FUNK->Tittle, "PRESET" );
	sprintf( FUNK->Name, "FUNK" );
	FUNK->p125 = 5;
	FUNK->p250 = -2;
	FUNK->p1000 = -3;
	FUNK->p4000 = 4;
	FUNK->p8000 = 0;
	EQList.Add(FUNK);

	HARDROCK->ResId = ML_SOUND_PALETTE_HARDROCK;
	HARDROCK->Sfx = SOUND_SEQ_HARDROCK;
	sprintf( HARDROCK->Tittle, "PRESET" );
	sprintf( HARDROCK->Name, "HARDROCK" );
	HARDROCK->p125 = 6;
	HARDROCK->p250 = 7;
	HARDROCK->p1000 = -5;
	HARDROCK->p4000 = 5;
	HARDROCK->p8000 = 2;
	EQList.Add(HARDROCK);

	HIPHOP->ResId = ML_SOUND_PALETTE_HIPHOP;
	HIPHOP->Sfx = SOUND_SEQ_HIPHOP;
	sprintf( HIPHOP->Tittle, "PRESET" );
	sprintf( HIPHOP->Name, "HIPHOP" );
	HIPHOP->p125 = 4;
	HIPHOP->p250 = 4;
	HIPHOP->p1000 = -1;
	HIPHOP->p4000 = 0;
	HIPHOP->p8000 = 3;
	EQList.Add(HIPHOP);

	INSTRUMENTAL->ResId = ML_SOUND_PALETTE_INSTRUMENTAL;
	INSTRUMENTAL->Sfx = SOUND_SEQ_INSTRUMENTAL;
	sprintf( INSTRUMENTAL->Tittle, "PRESET" );
	sprintf( INSTRUMENTAL->Name, "INSTRUMENTAL" );
	INSTRUMENTAL->p125 = 7;
	INSTRUMENTAL->p250 = 2;
	INSTRUMENTAL->p1000 = 0;
	INSTRUMENTAL->p4000 = 1;
	INSTRUMENTAL->p8000 = 5;
	EQList.Add(INSTRUMENTAL);

	JAZZ->ResId = ML_SOUND_PALETTE_JAZZ;
	JAZZ->Sfx = SOUND_SEQ_JAZZ;
	sprintf( JAZZ->Tittle, "PRESET" );
	sprintf( JAZZ->Name, "JAZZ" );
	JAZZ->p125 = 1;
	JAZZ->p250 = 2;
	JAZZ->p1000 = 4;
	JAZZ->p4000 = 3;
	JAZZ->p8000 = 0;
	EQList.Add(JAZZ);

	LATIN->ResId = ML_SOUND_PALETTE_LATIN;
	LATIN->Sfx = SOUND_SEQ_LATIN;
	sprintf( LATIN->Tittle, "PRESET" );
	sprintf( LATIN->Name, "LATIN" );
	LATIN->p125 = 2;
	LATIN->p250 = 2;
	LATIN->p1000 = 4;
	LATIN->p4000 = 0;
	LATIN->p8000 = 0;
	EQList.Add(LATIN);

	LECTURE->ResId = ML_SOUND_PALETTE_LECTURE;
	LECTURE->Sfx = SOUND_SEQ_LECTURE;
	sprintf( LECTURE->Tittle, "PRESET" );
	sprintf( LECTURE->Name, "LECTURE" );
	LECTURE->p125 =-9;
	LECTURE->p250 = -5;
	LECTURE->p1000 = 3;
	LECTURE->p4000 = 3;
	LECTURE->p8000 = -5;
	EQList.Add(LECTURE);

	POP->ResId = ML_SOUND_PALETTE_POP;
	POP->Sfx = SOUND_SEQ_POP;
	sprintf( POP->Tittle, "PRESET" );
	sprintf( POP->Name, "POP" );
	POP->p125 = 2;
	POP->p250 = -2;
	POP->p1000 = 2;
	POP->p4000 = 4;
	POP->p8000 = 6;
	EQList.Add(POP);

	RNB->ResId = ML_SOUND_PALETTE_RNB;
	RNB->Sfx = SOUND_SEQ_BLUES;
	sprintf( RNB->Tittle, "PRESET" );
	sprintf( RNB->Name, "R&B" );
	RNB->p125 = 0;
	RNB->p250 = 5;
	RNB->p1000 = 1;
	RNB->p4000 = 2;
	RNB->p8000 = 7;
	EQList.Add(RNB);

	SLOW->ResId = ML_SOUND_PALETTE_SLOW;
	SLOW->Sfx = SOUND_SEQ_SLOW;
	sprintf( SLOW->Tittle, "PRESET" );
	sprintf( SLOW->Name, "SLOW" );
	SLOW->p125 = 4;
	SLOW->p250 = 0;
	SLOW->p1000 = -3;
	SLOW->p4000 = -1;
	SLOW->p8000 = 3;
	EQList.Add(SLOW);

	SOFTROCK->ResId = ML_SOUND_PALETTE_SOFTROCK;
	SOFTROCK->Sfx = SOUND_SEQ_SOFTROCK;
	sprintf( SOFTROCK->Tittle, "PRESET" );
	sprintf( SOFTROCK->Name, "SOFTROCK" );
	SOFTROCK->p125 = 2;
	SOFTROCK->p250 = 3;
	SOFTROCK->p1000 = -2;
	SOFTROCK->p4000 = 2;
	SOFTROCK->p8000 = 2;
	EQList.Add(SOFTROCK);

	VOCAL->ResId = ML_SOUND_PALETTE_VOCAL;
	VOCAL->Sfx = SOUND_SEQ_VOICE;
	sprintf( VOCAL->Tittle, "PRESET" );
	sprintf( VOCAL->Name, "VOCAL" );
	VOCAL->p125 = -9;
	VOCAL->p250 = -1;
	VOCAL->p1000 = 4;
	VOCAL->p4000 = 4;
	VOCAL->p8000 = -2;
	EQList.Add(VOCAL);

	XBASS->ResId = ML_SOUND_PALETTE_XBASS;
	XBASS->Sfx = SOUND_SEQ_POWERBASS;
	sprintf( XBASS->Tittle, "PRESET" );
	sprintf( XBASS->Name, "XBASS" );
	XBASS->p125 = 9;
	XBASS->p250 = 9;
	XBASS->p1000 = 1;
	XBASS->p4000 = 0;
	XBASS->p8000 = 2;
	EQList.Add(XBASS);

/*
	ML_NORMAL, 							SOUND_NORMAL,				// 0

	ML_SOUND_PALETTE_TRUE_3D_MILD, 	SOUND_REX_MILD_PRESET,		// 1
	ML_SOUND_PALETTE_TRUE_3D_WIDE, 	SOUND_REX_WIDE_PRESET,		// 2
	ML_SOUND_PALETTE_TRUE_3D_LIVE, 		SOUND_REX_LIVE_PRESET,		// 3
	ML_SOUND_PALETTE_TRUE_3D_SPKR, 	SOUND_eXTX_PRESET,			// 4

	ML_SOUND_PALETTE_REAL_BASS,		SOUND_MEX_BASS_PRESET,		// 5
	ML_SOUND_PALETTE_REAL_LITE, 		SOUND_MEX_LITE_PRESET,		// 6
	ML_SOUND_PALETTE_REAL_PRO, 			SOUND_MEX_PRO_PRESET,		// 7

	ML_SOUND_PALETTE_ACOUSTIC, 		SOUND_SEQ_ACOUSTIC,		// 8
	ML_SOUND_PALETTE_BRIGHT, 			SOUND_SEQ_BRIGHT,			// 9
	ML_SOUND_PALETTE_CLASSICAL, 		SOUND_SEQ_CLASSIC,			// 10
	ML_SOUND_PALETTE_DANCE, 			SOUND_SEQ_DANCE,			// 11
	ML_SOUND_PALETTE_DEEPWARM, 		SOUND_SEQ_DARKWARM,		// 12
	ML_SOUND_PALETTE_ELECTRONIC, 		SOUND_SEQ_ELECTRONIC,		// 13
	ML_SOUND_PALETTE_FLAT, 				SOUND_SEQ_FLAT,				// 14
	ML_SOUND_PALETTE_FUNK, 				SOUND_SEQ_FUNK,			// 15
	ML_SOUND_PALETTE_HARDROCK, 		SOUND_SEQ_HARDROCK,		// 16
	ML_SOUND_PALETTE_HIPHOP, 			SOUND_SEQ_HIPHOP,			// 17
	ML_SOUND_PALETTE_INSTRUMENTAL, 	SOUND_SEQ_INSTRUMENTAL,	// 18
	ML_SOUND_PALETTE_JAZZ, 				SOUND_SEQ_JAZZ,			// 19
	ML_SOUND_PALETTE_LATIN, 			SOUND_SEQ_LATIN,			// 20
	ML_SOUND_PALETTE_LECTURE, 			SOUND_SEQ_LECTURE,			// 21
	ML_SOUND_PALETTE_POP, 				SOUND_SEQ_POP,				// 22
	ML_SOUND_PALETTE_RNB, 				SOUND_SEQ_BLUES,			// 23
	ML_SOUND_PALETTE_SLOW, 			SOUND_SEQ_SLOW,			// 24
	ML_SOUND_PALETTE_SOFTROCK, 		SOUND_SEQ_SOFTROCK,		// 25
	ML_SOUND_PALETTE_VOCAL, 			SOUND_SEQ_VOICE,			// 26
	ML_SOUND_PALETTE_XBASS,			SOUND_SEQ_POWERBASS,		// 27

	ML_SOUND_PALETTE_MY_3D_WIDE, 		SOUND_REX_WIDE_USER,		// 28
	ML_SOUND_PALETTE_MY_3D_LIVE, 		SOUND_REX_LIVE_USER,		// 29
	ML_SOUND_PALETTE_MY_REAL_SOUND, 	SOUND_MEX_USER,			// 30
	ML_SOUND_PALETTE_MY_EQ,			SOUND_SEQ_USER		
*/	
	switch(TaviSysParam.sound.user_effect)
		{
		case 1:	
			USERFX->ResId = ML_SOUND_PALETTE_MY_3D_WIDE;
			USERFX->Sfx = SOUND_REX_WIDE_USER;
			sprintf( USERFX->Tittle, "3D.WIDE" ); //var
			sprintf( USERFX->Name, "USER.EFFECT" ); 
			break;
			
		case 2:	
			USERFX->ResId = ML_SOUND_PALETTE_MY_3D_LIVE;		
			USERFX->Sfx = SOUND_REX_LIVE_USER; 
			sprintf( USERFX->Tittle, "3D.LIVE" ); //var
			sprintf( USERFX->Name, "USER.EFFECT" ); 
			break;

		default:
		case 0:	
			USERFX->ResId = ML_SOUND_PALETTE_MY_REAL_SOUND;	
			USERFX->Sfx = SOUND_MEX_USER; 
			sprintf( USERFX->Tittle, "REAL.SOUND" ); //var
			sprintf( USERFX->Name, "USER.EFFECT" ); 
			break;
		}

	
	USERFX->p125 = 0;
	USERFX->p250 = 0;
	USERFX->p1000 = 0;
	USERFX->p4000 = 0;
	USERFX->p8000 = 0;
	EQList.Add(USERFX);



	USEREQ->ResId = ML_SOUND_PALETTE_MY_EQ;
	USEREQ->Sfx = SOUND_SEQ_USER;
	sprintf( USEREQ->Tittle, "USER.EQ" );
	sprintf( USEREQ->Name, "USER.EQ" );
	USEREQ->p125 = TaviSysParam.sound.band0-9;
	USEREQ->p250 = TaviSysParam.sound.band1-9;
	USEREQ->p1000 = TaviSysParam.sound.band2-9;
	USEREQ->p4000 = TaviSysParam.sound.band3-9;
	USEREQ->p8000 = TaviSysParam.sound.band4-9;
	EQList.Add(USEREQ);

	TotalEQCount = EQList.Count();
	LastEQIdx = TotalEQCount-1;

	return TRUE;

}

void CSoundPaletteDbx::BuildAnEqLevelMeter( int level, int sx, int sy, eCursorState CurState )
{
	int ImageLevel;
	int Phase;
	int BandHeight;
	RMuint8 LevelColor;

	if( level == 0 )
		{

		if(CurState)
			LevelColor = EQBOX_SELECTED_BASE_COLOR;
		else
			LevelColor = EQBOX_UNSELECTED_BASE_COLOR;

		if(CurState == _CURSOR_SELECTED)
			LevelColor = EQBOX_SELECTED_BASE_COLOR;
		else if(CurState == _CURSOR_UNSELECTED)
			LevelColor = EQBOX_UNSELECTED_BASE_COLOR;
		else if(CurState == _CURSOR_ENABLED)
			LevelColor = EQBOX_UNSELECTED_BASE_COLOR;

		Phase = 4;
		BandHeight = 1;
		DrawBoxWithoutRefresh( LevelColor, sx, sy+Phase, 6, BandHeight );
		}
	else
		{

		if(CurState == _CURSOR_SELECTED)
			LevelColor = EQBOX_SELECTED_TITLE_TEXT_COLOR;
		else if(CurState == _CURSOR_UNSELECTED)
			LevelColor = EQBOX_UNSELECTED_TITLE_TEXT_COLOR;
		else if(CurState == _CURSOR_ENABLED)
			LevelColor = EQBOX_CURRENT_USING_EQ_TITLE_TEXT_COLOR;

		if( level >0)
			{	// 1 ~ 9
			if(level<3) { Phase=4; BandHeight=1; }
			else if(level<5) { Phase=3; BandHeight=2; }
			else if(level<7) { Phase=2; BandHeight=3; }
			else if(level<9) { Phase=1; BandHeight=4; }
			else { Phase=0; BandHeight=5; }
			DrawBoxWithoutRefresh( LevelColor, sx, sy+Phase, 6, BandHeight );
			}
		else		// -1 ~ -9
			{
			if(level>-3) { Phase=5; BandHeight=1; }
			else if(level>-5) { Phase=6; BandHeight=2; }
			else if(level>-7) { Phase=7; BandHeight=3; }
			else if(level>-9) { Phase=8; BandHeight=4; }
			else { Phase=9; BandHeight=5; }
			DrawBoxWithoutRefresh( LevelColor, sx, sy+5, 6, BandHeight );
			}
		}
}

void CSoundPaletteDbx::BuildEqLevelMeters( EQSet* pEQSet, int sx, int sy, eCursorState CurState )
{
	BuildAnEqLevelMeter( pEQSet->p125, sx, sy, CurState );
	BuildAnEqLevelMeter( pEQSet->p250, sx+(8), sy, CurState );
	BuildAnEqLevelMeter( pEQSet->p1000, sx+(2*8), sy, CurState );
	BuildAnEqLevelMeter( pEQSet->p4000, sx+(3*8), sy, CurState );
	BuildAnEqLevelMeter( pEQSet->p8000, sx+(4*8), sy, CurState );
}

void CSoundPaletteDbx::BuildEqText( char* text, RMuint8 color, RMuint16 sx, RMuint16 sy)
{
	_DrawASCIITextWithoutRefresh( text, 4, 8, &TAVI_IMAGES[EQ_DIGIT], sx, sy, EQ_DIGIT, color );
}


void CSoundPaletteDbx::BuildEqBox( EQSet* pEQSet, int sx, int sy, eCursorState CurState )
{
	RMuint8 BaseColor;
	RMuint8 TitleColor;
	RMuint8 TitleTextColor;
	RMuint8 ItemTextColor;
	
	if(CurState == _CURSOR_SELECTED)
		{
		BaseColor = EQBOX_SELECTED_BASE_COLOR;
		TitleColor = EQBOX_SELECTED_TITLE_COLOR;
		TitleTextColor = EQBOX_SELECTED_TITLE_TEXT_COLOR;
		ItemTextColor = EQBOX_SELECTED_ITEM_TEXT_COLOR;
		}

	else if(CurState == _CURSOR_UNSELECTED )
		{
		BaseColor = EQBOX_UNSELECTED_BASE_COLOR;
		TitleColor = EQBOX_UNSELECTED_TITLE_COLOR;
		TitleTextColor = EQBOX_UNSELECTED_TITLE_TEXT_COLOR;
		ItemTextColor = EQBOX_UNSELECTED_ITEM_TEXT_COLOR;
		}

	else if(CurState == _CURSOR_ENABLED)
		{
		BaseColor = EQBOX_CURRENT_USING_EQ_BASE_COLOR;
		TitleColor = EQBOX_CURRENT_USING_EQ_TITLE_COLOR;
		TitleTextColor = EQBOX_CURRENT_USING_EQ_TITLE_TEXT_COLOR;
		ItemTextColor = EQBOX_CURRENT_USING_EQ_ITEM_TEXT_COLOR;
		}
//pT->DrawEUC8Text(temp, 270, 20+((sx-EQBOX0_SX)/(EQBOX_WIDTH+EQBOX_SPACE)*20), _ORANGE, BaseColor,12,0);

	BuildRoundBox( BaseColor, sx, sy, EQBOX_WIDTH, EQBOX_HEIGHT );
	BuildRoundBox( TitleColor, sx+2, sy+2, EQTITLEBOX_WIDTH, EQTITLEBOX_HEIGHT );

	if( (pEQSet->ResId >= ML_SOUND_PALETTE_ACOUSTIC && pEQSet->ResId<ML_SOUND_PALETTE_XBASS+1) || pEQSet->ResId == ML_SOUND_PALETTE_MY_EQ )
		BuildEqLevelMeters(pEQSet, sx+8, sy+3, CurState );
	else
		BuildEqText( pEQSet->Tittle, TitleTextColor, sx + ((EQBOX_WIDTH - (strlen(pEQSet->Tittle)*4))/2)+1, sy+EQTITLE_SY);

	BuildEqText( pEQSet->Name, ItemTextColor, sx + ((EQBOX_WIDTH - (strlen(pEQSet->Name)*4))/2)+2, sy+EQNAME_SY);	

}

void CSoundPaletteDbx::BuildEqBoxes( int nCurEq )
{
	RMuint8 BaseColor;
	RMuint8 TitleColor;
	RMuint8 TitleTextColor;
	RMuint8 ItemTextColor;
	int Cur;
	int StartEq = nCurEq-2;
	eCursorState CursorState;

	char temp[50];

	if(StartEq<0) 
		StartEq = LastEQIdx - abs(StartEq) + 1;

	Cur = StartEq;

	for( int loop=0; loop<5; loop++, Cur++ )
		{
		
		if(Cur>LastEQIdx)	Cur=0;
		
		if(loop==2) 
			{
			CursorState = _CURSOR_SELECTED;
			}
		else 
			{
			if( EQList[Cur]->ResId == CurrentMacro ) CursorState = _CURSOR_ENABLED;
			else CursorState = _CURSOR_UNSELECTED;
			}

		//sprintf(temp,"Cur:%d|RId:%d|CEft:%d|CS:%d", Cur, EQList[Cur]->ResId, CurSoundEffect, CursorState );
		//pT->DrawEUC8Text(temp, 1, 20+(loop*20), _RED, BaseColor,12,0);
		BuildEqBox( EQList[Cur], (EQBOX_WIDTH+EQBOX_SPACE)*loop + EQBOX0_SX, EQBOX0_SY, CursorState);
		}

}

void CSoundPaletteDbx::Hide(eOSD_REFRESH_OPTION refresh)
{
	Stop_SoundPaletteCtrlHideTimer();
	Stop_SoundPaletteTouchApplyTimer();
	Stop_VideoControlPannelTimer();
	ClearControlPannelBackground();
	g_TopPanel->SetVisible( false );
	g_TopPanel->Update();
	DrawBoxWithoutRefresh(0,0,0,320, g_TopPanel->GetClientRect().GetHeight() );
	isVisible = FALSE;
	if( refresh == OSD_REFRESH )
		pMpegDecoder->RefreshOsd();
}

void CSoundPaletteDbx::Show()
{
	isVisible = TRUE;
	//DEBUGMSG(1, ("CSoundPaletteDbx::Show()\n"));
	printf("CSoundPaletteDbx::Show()\n");
	
	CurrentMacro = GetSFX_MacroBySFX( (SOUNDEFFECT_t)TaviSysParam.sound.eq );
	CurSoundEffect = GetSFX_ValueByMacro( CurrentMacro );

	SelectedAbsoluteIndex = -1;

	for( int i=0; i<=LastEQIdx; i++ )
		{
		if( EQList[i]->ResId == CurrentMacro )
			{
			SelectedAbsoluteIndex = i;
			break;
			}
		}

	if( SelectedAbsoluteIndex == -1 )
		SelectedAbsoluteIndex = LastEQIdx-1;	// UserEffect

	BuildControlPannelBackground();
	BuildBowCursor(LEFT_NORMAL|RIGHT_NORMAL);
	Redraw(OSD_REFRESH);
}



void CSoundPaletteDbx::Redraw(eOSD_REFRESH_OPTION refresh)
{
	g_TopPanel->SetVisible( true );
	g_TopPanel->Update();
//	BuildControlPannelBackground();
	BuildEqBoxes(SelectedAbsoluteIndex);
	isVisible = TRUE;
	if(refresh== OSD_REFRESH);
		pMpegDecoder->RefreshOsd();
}
void CSoundPaletteDbx::ForceClose()
{
	Cancel();
	MsgProc( 2, MSG_BUTTON_UP, TAVI_KEY_LEXIT, 0 );
}
void CSoundPaletteDbx::Cancel()
{
	TaviSysParam.sound.eq = 	PrevSoundEQ;
	if(m_fpSetupProc) m_fpSetupProc(NULL);
}
int CSoundPaletteDbx::CommonKey_LEFT(int isTouchInput)
{
	int SendOption = isTouchInput;

	BuildBowCursor(LEFT_SELECT|RIGHT_NORMAL);
	pMpegDecoder->RefreshOsd();

	SelectedAbsoluteIndex--;
	if(SelectedAbsoluteIndex<0)
		SelectedAbsoluteIndex = LastEQIdx;

	TaviSysParam.sound.eq = GetSFX_ValueByMacro( EQList[SelectedAbsoluteIndex]->ResId );
	Redraw(OSD_REFRESH);

	if(m_fpSetupProc) m_fpSetupProc(&SendOption);

	BuildBowCursor(LEFT_NORMAL|RIGHT_NORMAL);
	pMpegDecoder->RefreshOsd();				

	return TRUE;
}
int CSoundPaletteDbx::CommonKey_RIGHT(int isTouchInput)
{
	int SendOption = isTouchInput;

	BuildBowCursor(LEFT_NORMAL|RIGHT_SELECT);
	pMpegDecoder->RefreshOsd();
	
	SelectedAbsoluteIndex++;
	if(SelectedAbsoluteIndex>LastEQIdx)
		SelectedAbsoluteIndex = 0;

	TaviSysParam.sound.eq = GetSFX_ValueByMacro( EQList[SelectedAbsoluteIndex]->ResId );
	Redraw(OSD_REFRESH);

	pMpegDecoder->RefreshOsd();
	if(m_fpSetupProc) m_fpSetupProc(&SendOption);

	BuildBowCursor(LEFT_NORMAL|RIGHT_NORMAL);
	pMpegDecoder->RefreshOsd();				

	
	return TRUE;
}
int CSoundPaletteDbx::CommonKey_OK(int isTouchInput)
{
	PrevSoundEQ = TaviSysParam.sound.eq;
	Hide(OSD_REFRESH);
	return -1;
}
int CSoundPaletteDbx::CommonKey_EXIT(int isTouchInput)
{
	Cancel();					
	Hide(OSD_REFRESH);
	return -1;
}

int CSoundPaletteDbx::MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	DEBUGMSG(1,("CSoundPaletteDbx::MsgProc( %d, %d, %d, %d )\n", ssTaskId, ulMsgId, ssParam1, ssParam2 ));

	//////////////////////////////////////////////////////////////////////
	//	MsgBtnUp
	//	Key Message Process

	if( ulMsgId == MSG_BUTTON_UP )
		{
		switch( ssParam1 )
			{
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LEFT:
			case TAVI_KEY_JOG2_DOWN:
				CommonKey_LEFT(0);
				break;
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_RIGHT:
			case TAVI_KEY_JOG2_UP:
				CommonKey_RIGHT(0);
				break;
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_ENTER:
			case TAVI_KEY_JOG2_ENTER:
				CommonKey_OK(0);
				return -1;
				break;
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_EQ:
				Hide(OSD_REFRESH);
				return -1;
				break;
			case TAVI_KEY_MENU:
				break;
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_EXIT:
			case TAVI_KEY_LEXIT:
			case TAVI_KEY_MODE:
			case TAVI_KEY_LMODE:
			case TAVI_KEY_JOG1_ENTER:
			case TAVI_KEY_SHUTDOWN:
				CommonKey_EXIT(0);
				return -1;
				break;
			}
		}


	//////////////////////////////////////////////////////////////////////
	//	MsgBtnUp
	//	Touch Pad Message Process

	else if( ulMsgId == MSG_TOUCHPAD )
		{

		if( ssParam1 == -1 )
			CommonKey_LEFT(1);
		else if( ssParam1 == 1 )
			CommonKey_RIGHT(1);
		else if( ssParam2 == 1 )
			{
			CommonKey_OK(0);
			return -1;
			}
		}

	return 0;
}


