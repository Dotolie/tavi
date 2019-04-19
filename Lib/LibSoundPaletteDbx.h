//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CSoundPaletteDbx Class Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibSoundPaletteDbx.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibSoundPaletteDbx.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.21  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.20  2005/12/02 11:06:13  etnlwind
// Update for SoundPalette UI
//
// Revision 1.19  2005/12/01 09:32:55  etnlwind
// Update for LibSoundPalette
//
// Revision 1.18  2005/12/01 02:48:45  etnlwind
// *** empty log message ***
//
// Revision 1.17  2005/11/30 10:03:41  etnlwind
// Update for UserEQ and User-Effect UI
//
// Revision 1.16  2005/11/25 13:52:47  etnlwind
// Update for New EQ UI
//
// Revision 1.15  2005/11/25 08:40:40  etnlwind
// Update for New EQ
//
// Revision 1.14  2005/11/24 10:56:46  etnlwind
// Update for New EQ
//
// Revision 1.13  2005/11/23 09:33:53  etnlwind
// Update for TAVI-EQ
//
// Revision 1.12  2005/11/22 08:11:23  etnlwind
// Update for New EQ UI
//
// Revision 1.11  2005/11/21 12:00:29  etnlwind
// Update for AlarmClock & Sound Palette UI
//
// Revision 1.10  2005/11/21 02:51:32  etnlwind
// Update for EQ Select UI
//
// Revision 1.9  2005/11/11 04:25:58  etnlwind
// Update for VideoPlayer Screen Clean Method
//
// Revision 1.8  2005/09/12 00:52:14  etnlwind
// Update for FileManager
//
// Revision 1.7  2005/09/08 02:39:11  ywkim
// Add preset of sound palettes with acoustic,latin,lecture,electronic,hiphop
//
// Revision 1.6  2005/08/19 05:45:41  etnlwind
// Update for VideoPlayer EQ BugFix
//
// Revision 1.5  2005/08/18 10:38:18  etnlwind
// Update for Video Player Sound Palette Force Closing
//
// Revision 1.4  2005/08/18 09:34:32  etnlwind
// Update for VideoPlayer UserEQ Setup Related
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


 #ifndef __LIB_SOUND_PALETTE_DBX_H__
 #define __LIB_SOUND_PALETTE_DBX_H__

#include <stdio.h>
#include <stdlib.h>
#include <rmbasic.h>
#include <linuxfs.h>
#include <jpeg/jpeg.h>

#include <tavi_lib.h>
#include <tavi_structures.h>
#include <bitmap.h>
#include <ImageFont.h>
#include <ImageResource.h>
#include <meplib.h>

#include "../Component/object.h"
#include "../Component/Stack.h"
#include "../Component/event.h"
#include "../Component/fbdev.h"
#include "../Component/image.h"
#include "../Component/timer.h"

#include <Lib/LibImage.h>

#include <string.h>
#include <Component/object.h>
#include <Component/image.h>

#include <Lib/LibImage.h>
#include <Lib/LibScrollBar.h>
#include <Lib/LibDialogBox.h>
#include <Lib/LibBookmarkDialogBox.h>
#include <Lib/LibSetupDialogBox.h>
#include <Lib/LibControlPannel.h>
#include <Lib/LibListBox.h>
#include "../include/tavi_text_resources.h"

//#include <fatfs.h>


#define __ATTRIBUTE_PACKED__	__attribute__((packed))

typedef struct _tag_SOUND_PALETTE_NODE
{
	int ItemIndex;
	int MacroIndex;
	SOUNDEFFECT_t PaletteValue;

} __ATTRIBUTE_PACKED__ SOUND_PALETTE_NODE;

#if 0
static const int arraySFXtoMAC[] = 
	{
	ML_NORMAL, //	SOUND_NORMAL = 0,
	ML_SOUND_PALETTE_TRUE_3D_SPKR, //	SOUND_eXTX_PRESET,
	ML_SOUND_PALETTE_REAL_BASS, //	SOUND_MEX_BASS_PRESET,
	ML_SOUND_PALETTE_REAL_LITE, //	SOUND_MEX_LITE_PRESET,
	ML_SOUND_PALETTE_REAL_PRO, //	SOUND_MEX_PRO_PRESET,
	ML_SOUND_PALETTE_MY_REAL_SOUND, //	SOUND_MEX_USER,
	ML_SOUND_PALETTE_TRUE_3D_MILD, //	SOUND_REX_MILD_PRESET,
	ML_SOUND_PALETTE_TRUE_3D_WIDE, //	SOUND_REX_WIDE_PRESET,
	ML_SOUND_PALETTE_TRUE_3D_LIVE, //	SOUND_REX_LIVE_PRESET,
	ML_SOUND_PALETTE_MY_3D_WIDE, //	SOUND_REX_WIDE_USER,
	ML_SOUND_PALETTE_MY_3D_LIVE, //	SOUND_REX_LIVE_USER,	
	ML_SOUND_PALETTE_FLAT, //	SOUND_SEQ_FLAT,
	ML_SOUND_PALETTE_VOCAL, //	SOUND_SEQ_VOICE,
	ML_SOUND_PALETTE_XBASS, //	SOUND_SEQ_POWERBASS,
	ML_SOUND_PALETTE_BRIGHT, //	SOUND_SEQ_BRIGHT,
	ML_SOUND_PALETTE_DEEPWARM, //	SOUND_SEQ_DARKWARM,
	ML_SOUND_PALETTE_CLASSICAL, //	SOUND_SEQ_CLASSIC,
	ML_SOUND_PALETTE_HARDROCK, //	SOUND_SEQ_HARDROCK,
	ML_SOUND_PALETTE_SOFTROCK, //	SOUND_SEQ_SOFTROCK,
	ML_SOUND_PALETTE_JAZZ, //	SOUND_SEQ_JAZZ,
	ML_SOUND_PALETTE_DANCE, //	SOUND_SEQ_DANCE,
	ML_SOUND_PALETTE_SLOW, //	SOUND_SEQ_SLOW,
	ML_SOUND_PALETTE_POP, //	SOUND_SEQ_POP,
	ML_SOUND_PALETTE_RNB, //	SOUND_SEQ_BLUES,
	ML_SOUND_PALETTE_INSTRUMENTAL, //	SOUND_SEQ_INSTRUMENTAL,
	ML_SOUND_PALETTE_FUNK, //	SOUND_SEQ_FUNK,
	ML_SOUND_PALETTE_MY_EQ //	SOUND_SEQ_USER,
	/*
#define	ML_SOUND_PALETTE_ACOUSTIC	310
#define	ML_SOUND_PALETTE_ELECTRONIC	315
#define	ML_SOUND_PALETTE_HIPHOP	319
#define	ML_SOUND_PALETTE_LATIN	322
#define	ML_SOUND_PALETTE_LECTURE	323
	*/
	};
#endif

#define TableEQMaxRow 31	// 0~31
static const int TableEQ[][2] = 
	{
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
	ML_SOUND_PALETTE_MY_EQ,			SOUND_SEQ_USER				// 31
	};


#define EQBOX_SELECTED_BASE_COLOR 124
#define EQBOX_SELECTED_TITLE_COLOR 194
#define EQBOX_SELECTED_TITLE_TEXT_COLOR _WHITE
#define EQBOX_SELECTED_ITEM_TEXT_COLOR 197

#define EQBOX_UNSELECTED_BASE_COLOR 196
#define EQBOX_UNSELECTED_TITLE_COLOR 195
#define EQBOX_UNSELECTED_TITLE_TEXT_COLOR 192
#define EQBOX_UNSELECTED_ITEM_TEXT_COLOR 193

#define EQBOX_CURRENT_USING_EQ_BASE_COLOR 191
#define EQBOX_CURRENT_USING_EQ_TITLE_COLOR EQBOX_UNSELECTED_TITLE_COLOR
#define EQBOX_CURRENT_USING_EQ_TITLE_TEXT_COLOR _WHITE
#define EQBOX_CURRENT_USING_EQ_ITEM_TEXT_COLOR EQBOX_UNSELECTED_ITEM_TEXT_COLOR
		
#define EQBOX0_SX 13
#define EQBOX0_SY 205
#define EQBOX_SPACE 6

#define EQBOX_WIDTH 54
#define EQBOX_HEIGHT 30
#define EQTITLEBOX_WIDTH 50
#define EQTITLEBOX_HEIGHT 13

#define EQTITLE_SY 5
#define EQNAME_SY 18

#define ARROW_SIZE (8*9)

#define LEFT_NORMAL 	0x00000001
#define LEFT_SELECT		0x00000002
#define RIGHT_NORMAL	0x00000004
#define RIGHT_SELECT	0x00000008


typedef struct _EQSet
{
	RMuint32 ResId;
	SOUNDEFFECT_t Sfx;
	char Tittle[12];
	char Name[12];
	int p125;
	int p250;
	int p1000;
	int p4000;
	int p8000;
} __ATTRIBUTE_PACKED__ EQSet;

typedef enum
{
	_CURSOR_UNSELECTED,
	_CURSOR_SELECTED,
	_CURSOR_ENABLED	
} eCursorState;




///////////////////////////////////////////////////////////////////////
//
// CSoundPaletteDbx Class
// --------------------------

class CSoundPaletteDbx : public CDialogBox
{
	//CList<SOUND_PALETTE_NODE> 	ItemList;
	int isVisible;
	SETUP_PROCESSOR m_fpSetupProc;

	EQSet* NORMAL;
	EQSet* WIDE;
	EQSet* MILD;
	EQSet* LIVE;;
	EQSet* SPEAKER;
	EQSet* BASS;
	EQSet* LITE;
	EQSet* PRO;
	EQSet* ACOUSTIC;
	EQSet* BRIGHT;
	EQSet* CLASSICAL;
	EQSet* DANCE;
	EQSet* DEEPWARM;
	EQSet* ELECTRONIC;
	EQSet* FLAT;
	EQSet* FUNK;
	EQSet* HARDROCK;
	EQSet* HIPHOP;
	EQSet* INSTRUMENTAL;
	EQSet* JAZZ;
	EQSet* LATIN;
	EQSet* LECTURE;
	EQSet* POP;
	EQSet* RNB;
	EQSet* SLOW;
	EQSet* SOFTROCK;
	EQSet* VOCAL;
	EQSet* XBASS;
	EQSet* USERFX;
	EQSet* USEREQ;	
	
	CList<EQSet> EQList;
	RMuint8 TempLArrow[ARROW_SIZE];
	RMuint8 TempRArrow[ARROW_SIZE];

	int TotalEQCount;
	int LastEQIdx;
	int CurrentMacro;
	char PrevSoundEQ;
	int CurSoundEffect;

	int CreateEQList();

	void BuildBowCursor( RMuint32 option );
	void BuildRoundBox(RMuint8 color, RMuint16 sx, RMuint16 sy, RMuint16 width, RMuint16 height );
	void BuildAnEqLevelMeter( int level, int sx, int sy, eCursorState CurState );
	void BuildEqLevelMeters( EQSet* pEQSet, int sx, int sy, eCursorState CurState );
	void BuildEqText( char* text, RMuint8 color, RMuint16 sx, RMuint16 sy);
	void BuildEqBox( EQSet* pEQSet, int sx, int sy, eCursorState CurState );
	void BuildEqBoxes( int nCurEq );
	
public:

	CSoundPaletteDbx( U32 DbxTitleRes, int x, int y, int w, int h, SETUP_PROCESSOR fpSetupProc, U32 option );
	virtual ~CSoundPaletteDbx();

	CSetupDialogBox* pSetup2BEQDbx;
	CSetupDialogBox* pSetup5BEQDbx;	

	int ApplyUserSet();
	void Cancel();
	void ForceClose();

	int CommonKey_LEFT(int isTouchInput);
	int CommonKey_RIGHT(int isTouchInput);
	int CommonKey_OK(int isTouchInput);
	int CommonKey_EXIT(int isTouchInput);

	void Hide(eOSD_REFRESH_OPTION refresh);
	void Show();
	void Redraw(eOSD_REFRESH_OPTION refresh);

	int IsVisible() { return isVisible; }
	
	int MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );	

};










#endif


