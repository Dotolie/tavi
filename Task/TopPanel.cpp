//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Top Panel Class 
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/TopPanel.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/06/23 08:20:04 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: TopPanel.cpp,v $
// Revision 1.3  2006/06/23 08:20:04  zstein
// - fixed null-pointer string error.
//
// Revision 1.2  2006/06/22 01:35:43  zstein
// - fixed title bug.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.54  2006/01/03 09:32:44  etnlwind
// Update for Text Engine Modification
//
// Revision 1.53  2006/01/02 06:55:43  etnlwind
// Update for New Font & MsgPeek_Ex
//
// Revision 1.52  2005/12/30 04:25:43  etnlwind
// Update for New Font Adaption
//
// Revision 1.51  2005/12/09 03:38:48  zstein
// *** empty log message ***
//
// Revision 1.50  2005/12/03 06:46:49  zstein
// *** empty log message ***
//
//*******************************************************************************

/* TOP PANEL CLASS
 * This class control state of battery level, hold, Headphone/speaker and alarm.
 */
#include <time.h>

#include <Lib/LibImage.h>
#include <Component/CharSet.h>
#include <Component/Bitmap.h>
 
#include "TopPanel.h"
#include "tavi_global.h"

// Macros
#define BATTERY_LEVEL_COUNT		7
#define TITLE_TEXT_X				32
#define TITLE_TEXT_Y				6
#define LOGO_OFFSET_X				5
#define LOGO_OFFSET_Y				4
#define TITLE_WIDTH					140
#define BATTERY_POS_X				292
#define ICON_BASE_Y					5
#define ICON_SPACE_X				2

// Extern variables
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern CTextEngine_BDF* pT;

// Definitions
CTopPanel::CTopPanel( CWindow* parent, BatteryLevel level, bool fAlarm, bool fHold ) :
	CWindow( parent ),
	m_nBatteryLevel( level ),
	m_fAlarm( fAlarm ),
	m_fHold( fHold )
{
	int iw;
	int ih;
	char* pImage;
	int i;

	iw = TAVI_IMAGES[ICON_TOP_BATTERY].bitmapinfoheader.biWidth;
	ih = TAVI_IMAGES[ICON_TOP_BATTERY].bitmapinfoheader.biHeight/BATTERY_LEVEL_COUNT;
	pImage = ( char* )TAVI_IMAGES[ICON_TOP_BATTERY].image_buffer;

	// Create battery pixmaps
	for( i=0; i<BATTERY_LEVEL_COUNT; i++ ) {
		m_pmBattery[i] = new CPixmap;
		m_pmBattery[i]->CreatePixmap( iw, ih, 8, pImage+i*iw*ih, false );
	}

	// create hold pixmap;
	iw = TAVI_IMAGES[TYPE_ICON_LIST].bitmapinfoheader.biWidth;
	ih = TYPE_ICON_HEIGHT;
	pImage = ( char* )TAVI_IMAGES[TYPE_ICON_LIST].image_buffer+LII_KEYLOCK_U*iw*ih;
	m_pmHold = new CPixmap;
	m_pmHold->CreatePixmap( iw, ih, 8, pImage, false );

	// create playmode pixmap
	pImage = ( char* )TAVI_IMAGES[TYPE_ICON_LIST].image_buffer+LII_PLAY_U*iw*ih;
	m_pmPlay = new CPixmap;
	m_pmPlay->CreatePixmap( iw, ih, 8, pImage, false );

	pImage = ( char* )TAVI_IMAGES[TYPE_ICON_LIST].image_buffer + LII_PAUSE_U*iw*ih;
	m_pmPause = new CPixmap;
	m_pmPause->CreatePixmap( iw, ih, 8, pImage, false );
	
	// create speaker pixmap;
	pImage = ( char* )TAVI_IMAGES[TYPE_ICON_LIST].image_buffer+LII_SPEAKER_ON_U*iw*ih;
	m_pmSpeakerOn = new CPixmap;
	m_pmSpeakerOn->CreatePixmap( iw, ih, 8, pImage, false );

	pImage = ( char* )TAVI_IMAGES[TYPE_ICON_LIST].image_buffer+LII_SPEAKER_OFF_U*iw*ih;
	m_pmSpeakerOff = new CPixmap;
	m_pmSpeakerOff->CreatePixmap( iw, ih, 8, pImage, false );

	pImage = ( char* )TAVI_IMAGES[TYPE_ICON_LIST].image_buffer+LII_EARPHONE_ON*iw*ih;
	m_pmepon = new CPixmap;
	m_pmepon->CreatePixmap( iw, ih, 8, pImage, false );

	pImage = ( char* )TAVI_IMAGES[TYPE_ICON_LIST].image_buffer+LII_EARPHONE_OFF*iw*ih;
	m_pmepoff = new CPixmap;
	m_pmepoff->CreatePixmap( iw, ih, 8, pImage, false );

	// create alarm pixmap;
	pImage = ( char* )TAVI_IMAGES[TYPE_ICON_LIST].image_buffer+LII_ALARM_U*iw*ih;
	m_pmAlarm = new CPixmap;
	m_pmAlarm->CreatePixmap( iw, ih, 8, pImage, false );
	
	m_tsw = 0;
	m_fSpeaker = false;
	m_nPlayMode = PLAYMODE_STOP;
	m_mute = false;

  memset( m_szTitle, 0, sizeof(m_szTitle) );
	m_szlen = 0;
}

CTopPanel::~CTopPanel( void )
{
	int i;
	// Delete battery pixmap;
	for( i=0; i<BATTERY_LEVEL_COUNT; i++ ) 
		delete m_pmBattery[i];

	// Delete hold pixmap;
	if( m_pmHold ) delete m_pmHold;
	// Delete headphone pixmap;
	// Delete speaker pixmap;
	if( m_pmSpeakerOn ) delete m_pmSpeakerOn;
	if( m_pmSpeakerOff ) delete m_pmSpeakerOff;
	if( m_pmepon ) delete m_pmepon;
	if( m_pmepoff ) delete m_pmepoff;
	// Delete alarm pixmap;
	// Delete NML Logo pixmap;
	if( m_pmLogo ) delete m_pmLogo;
}

void CTopPanel::SetBatteryLevel( BatteryLevel level )
{
	if( level > BATTERY_LEVEL_AC_OFF ) return;
	if( level < BATTERY_NONE ) return;

	m_nBatteryLevel = level;
}

void CTopPanel::SetHold( bool fHold )
{
	m_fHold = fHold;
}

void CTopPanel::SetAlarm( bool fAlaram )
{
	m_fAlarm = fAlaram;
}

void CTopPanel::SetLogo( LogoType id )
{
	if( id < LOGO_ID_NML ) return;
	if( id >= LOGO_ID_END ) return;

	char* szPath = NULL;
	m_current = (int)id;
	switch( id ) {
	case LOGO_ID_NML:
		szPath = _IMAGE_MAIN".rotate/nml.bmp";
		break;
	case LOGO_ID_PHOTO: 
		szPath = _IMAGE_CONTROL"/photo/title-photo.bmp";
		break;
	case LOGO_ID_MUSIC: 
		szPath = _IMAGE_CONTROL"/audio/title-music.bmp";
		break;
	case LOGO_ID_VIDEO: 
		szPath = _IMAGE_CONTROL"/video/title-video.bmp";
		break;
	case LOGO_ID_RADIO: 
		szPath = _IMAGE_CONTROL"/radio/title-radio.bmp";
		break;
	case LOGO_ID_SETUP: 
		szPath = _IMAGE_CONTROL"/setup/title-setup.bmp";
		break;
	case LOGO_ID_RECORD: 
		szPath = _IMAGE_CONTROL"/record/title-record.bmp";
		break;
	case LOGO_ID_FILEMANAGER:
		szPath = _IMAGE_CONTROL"/browse/title-browse.bmp";
		break;
	case LOGO_ID_ETC:
		szPath = _IMAGE_CONTROL"/extra/title-extra.bmp";
		break;
	case LOGO_ID_TEXT: 
		szPath = _IMAGE_CONTROL"/text/title-text.bmp";
		break;
	}

	if( m_pmLogo ) delete m_pmLogo;
	m_pmLogo = NULL;

	if( m_current == LOGO_ID_NML ) {
		// change background color;
		m_nBackgroundColor = 67;
		m_nBorderColor = 67;
	}
	else {
		m_nBackgroundColor = 0;
		m_nBorderColor = 0;
	}
	
	CBitmap bmp;
	if( bmp.DecodeBMP( szPath, false ) ) return;
	
	BMPINFOHEADER header = bmp.GetInfoHeader();
	int w = header.width;
	int h = header.height;
	char* buf = new char[w*h];
	bmp.GetImageData( buf );
	
	m_pmLogo = new CPixmap;
	m_pmLogo->CreatePixmap( w, h, 8, buf, true );
}

void CTopPanel::SetTitle( const CString& szTitle )
{
  SetTitle( szTitle.GetString(), szTitle.IsUnicode() );
}

void CTopPanel::SetTitle( const char* szTitle, bool fUnicode )
{
	m_t_unicode = fUnicode;
	if( szTitle ) {
    if( m_t_unicode ) {
      uchar_t* pwsz = (uchar_t*)m_szTitle;
      ustrcpy( pwsz, (const uchar_t*)szTitle );
      m_szlen = ustrlen(pwsz);
    }
    else {
      strcpy( m_szTitle, (const char*)szTitle );
      m_szlen = strlen( m_szTitle );
    }
  }
  else {
    memset( m_szTitle, 0, sizeof(m_szTitle) );
  }
	//m_szTitle.ConvertWideString();
	m_nTitleResID = -1;
}

void CTopPanel::SetTitle( int id ) 
{
	m_nTitleResID = id;
}

void CTopPanel::SetSpeaker( bool onoff )
{
	m_fSpeaker = onoff;
}

void CTopPanel::SetPlayMode( int mode ) 
{
	if( mode < PLAYMODE_STOP ) return;
	if( mode > PLAYMODE_PLAY ) return;

	m_nPlayMode = mode;
}

void CTopPanel::OnPaint( const CRect& rc )
{
	int fontsize = 12;
	int opt = DT_TRANSPARENCY|DT_BOLD|DT_OUTLINE|DT_TOP_PANNEL;
	int PalIdx;
	if( m_fVisible == false ) return;

	if( pTAVI->ssTaskId > 0 ) {
		PalIdx = 0;
	}
	else {
		PalIdx = BG_FILTER_COLOR_PALETTE;
	}
	CWindow::OnPaint( rc );
	// Draw Logo
	if(m_fLogo == false)
	{
		if( m_pmLogo ) {
			op_BitBlt( &g_SysPixmap, rcABS.x1+LOGO_OFFSET_X, rcABS.y1+LOGO_OFFSET_Y, 
				m_pmLogo, 0, 0, m_pmLogo->m_nWidth, m_pmLogo->m_nHeight );
		}
	}

	// Draw battery image
	op_BitBlt_SkipTr( &g_SysPixmap, SX_BATTERY, rcABS.y1+ICON_BASE_Y,  m_pmBattery[m_nBatteryLevel], 0, 0,  m_pmBattery[m_nBatteryLevel]->m_nWidth, m_pmBattery[m_nBatteryLevel]->m_nHeight, PalIdx );

	// Draw title
	if(m_fTitle == false)
	{
		if( m_current != LOGO_ID_NML ) {
			if( m_nTitleResID == -1 ) {
				uchar_t* pwsz;
				uchar_t wsz[256];
				int len, dotdot=0, i;
				if( m_t_unicode ) {
					pwsz = ( uchar_t* )m_szTitle;
					len = m_szlen;
				}
				else {
					len = ConvertUnicode( (char*)m_szTitle, m_szlen, wsz, sizeof(wsz), TaviSysParam.etc.language );
					pwsz = wsz;
				}
				int nBoundaryLen = pT->GetBoundaryLength( pwsz, len, 12, opt, TITLE_WIDTH );
				if( len > nBoundaryLen ) {
					dotdot = 3;
					nBoundaryLen -= dotdot;
				}
				int x = TITLE_TEXT_X;
				for( i=0; i<nBoundaryLen; i++ ) {
					x += pT->DrawCharacter( pwsz[i], x, TITLE_TEXT_Y, _WHITE, fontsize, opt );
				}
				wsz[0] = '.';
				for( i=0; i<dotdot; i++ ) {
					x += pT->DrawCharacter( wsz[0], x, TITLE_TEXT_Y, _WHITE, fontsize, opt );
				}
			}
			else { // Resource Text
				pT->DrawResText( m_nTitleResID, TITLE_TEXT_X, TITLE_TEXT_Y, _WHITE, fontsize, opt );
			}
		}
	}
	
	// Draw time
	if(m_fTimer == false)
		{
		int w;
		time_t t = time( NULL );
		struct tm* tm = localtime( &t );
		char buf[10];
		uchar_t wbuf[10];
		if( TaviSysParam.etc.timeformat ) 
			{ // 24 hour format
			sprintf( buf, "%02d:%02d", tm->tm_hour, tm->tm_min );
			}
		else 
			{ // 12 hour format
			int h = tm->tm_hour > 12 ? tm->tm_hour-12 : tm->tm_hour;
			if( !h ) h = 12; 
			sprintf( buf, "%02d:%02d", 	h, tm->tm_min );
			}
		
		str2ustr( wbuf, buf, strlen(buf) );
		w = pT->GetStringWidth( wbuf, 12, (DT_TRANSPARENCY|DT_OUTLINE|DT_STATIC_6WIDTH) );
		op_DrawOsdBox( m_nBackgroundColor, SX_TIME, rcABS.y1+LOGO_OFFSET_Y+2, w, 14 );

		if( pTAVI->ssTaskId ) 
			pT->DrawUC16Text( wbuf, SX_TIME, rcABS.y1+LOGO_OFFSET_Y+2, _WHITE, 12, (DT_TRANSPARENCY|DT_OUTLINE|DT_STATIC_6WIDTH) );
		else
			pT->DrawUC16Text( wbuf, SX_TIME, rcABS.y1+LOGO_OFFSET_Y+2, _WHITE, _BLACK, 12, (DT_TRANSPARENCY|DT_OUTLINE|DT_STATIC_6WIDTH) );

		}

	// Draw Speaker On/Off
	CPixmap* pm;
	if( m_mute )
		{
		if( m_fSpeaker ) pm = m_pmSpeakerOff;
		else pm = m_pmepoff;
		}
	else 
		{
		if( m_fSpeaker ) pm = m_pmSpeakerOn;
		else pm = m_pmepon;
		}
	op_BitBlt_SkipTr( &g_SysPixmap, SX_SPEAKER, rcABS.y1+ICON_BASE_Y, pm, 0, 0, pm->m_nWidth, pm->m_nHeight, PalIdx );

	// Draw play status
	if( m_nPlayMode != PLAYMODE_STOP )
		{
		if( m_nPlayMode == PLAYMODE_PLAY ) pm = m_pmPlay;
		else pm = m_pmPause;
		op_BitBlt_SkipTr( &g_SysPixmap,  SX_PLAYSTATUS, rcABS.y1+ICON_BASE_Y, pm, 0, 0, pm->m_nWidth, pm->m_nHeight, PalIdx );
		}

	// Draw Hold
	if( m_fHold )
 		op_BitBlt_SkipTr( &g_SysPixmap, SX_HOLD, rcABS.y1+ICON_BASE_Y, m_pmHold, 0, 0, m_pmHold->m_nWidth, m_pmHold->m_nHeight, PalIdx);
	else
		g_SysPixmap.Fill( SX_HOLD, rcABS.y1+ICON_BASE_Y, m_pmHold->m_nWidth, m_pmHold->m_nHeight, PalIdx );

	UpdateAlarmStatus();
	
	// Draw Alarm
	if( m_fAlarm )
		op_BitBlt_SkipTr( &g_SysPixmap, SX_ALARM, rcABS.y1+ICON_BASE_Y, m_pmAlarm, 0, 0, m_pmAlarm->m_nWidth, m_pmAlarm->m_nHeight, PalIdx);
	else
		g_SysPixmap.Fill( SX_ALARM, rcABS.y1+ICON_BASE_Y, m_pmAlarm->m_nWidth, m_pmAlarm->m_nHeight, PalIdx );
}


// Added by Sangjin Ryu
void CTopPanel::DrawTime( int isAppStatus )
{
	int posx, w;
	time_t t = time( NULL );
	struct tm* tm = localtime( &t );
	char buf[10];
	uchar_t wbuf[10];

	if( TaviSysParam.etc.timeformat ) 
		{ // 24 hour format
		sprintf( buf, "%02d:%02d", tm->tm_hour, tm->tm_min );
		}
	else 
		{ // 12 hour format
		int h = tm->tm_hour > 12 ? tm->tm_hour-12 : tm->tm_hour;
		if( !h ) h = 12; 
		sprintf( buf, "%02d:%02d", 	h, tm->tm_min );
		}
	
	str2ustr( wbuf, buf, strlen(buf) );
	w = pT->GetStringWidth( wbuf, 12, (DT_TRANSPARENCY|DT_OUTLINE|DT_STATIC_6WIDTH) );
	op_DrawOsdBox( m_nBackgroundColor, SX_TIME, rcABS.y1+LOGO_OFFSET_Y+2, w, 14 );
	pT->DrawUC16Text( wbuf, SX_TIME, rcABS.y1+LOGO_OFFSET_Y+2, _WHITE, 12, (DT_TRANSPARENCY|DT_OUTLINE|DT_STATIC_6WIDTH) );
}

void CTopPanel::DrawHold(int isHold, int isAppStatus)
{
#if 0
	int posx;
	
	m_fHold = isHold;

	if( m_tsw == 0 )
		DrawTime(isAppStatus);

	posx = rcABS.x1 + BATTERY_POS_X;
	posx -= (ICON_SPACE_X + m_tsw);
	posx -= (ICON_SPACE_X + m_pmHold->m_nWidth);

	if( m_fHold ) 
		{
		DrawItemWithoutRefresh(&TAVI_IMAGES[TYPE_ICON_LIST], 47, posx, rcABS.y1+ICON_BASE_Y, 12, 12 );
		}
	else 
		{
		if( isAppStatus )
			g_SysPixmap.Fill( posx, rcABS.y1+ICON_BASE_Y, m_pmHold->m_nWidth, m_pmHold->m_nHeight, 0 );
		else
			g_SysPixmap.Fill( posx, rcABS.y1+ICON_BASE_Y, m_pmHold->m_nWidth, m_pmHold->m_nHeight, 67 );
		}

	m_tsw = 0;
#endif	
}

void CTopPanel::DrawExtSpk(int isExtSpkActivated, int isAppStatus)
{
	// Draw Speaker On/Off
	int posx;
	RMuint16 PictureID;

	m_fSpeaker = isExtSpkActivated;

	if( m_fSpeaker ) PictureID = 59;	// SPK ON
	else PictureID = 61;			// SPK OFF
/*
	posx = rcABS.x1 + BATTERY_POS_X;
	posx -= (ICON_SPACE_X + m_tsw);
	posx -= ((ICON_SPACE_X+m_pmHold->m_nWidth)*2);
*/
	DrawItemWithoutRefresh(&TAVI_IMAGES[TYPE_ICON_LIST], PictureID, SX_SPEAKER, rcABS.y1+ICON_BASE_Y, 12, 12 );
}

void CTopPanel::DrawBgPlayStat(int isExtBgPlayActivated, int isAppStatus)
{
	/*
	int posx;
	CPixmap* pm;	
	
	// Draw play status

	m_nPlayMode = isExtBgPlayActivated;

	if( m_nPlayMode != PLAYMODE_STOP ) {
		if( m_nPlayMode == PLAYMODE_PLAY ) pm = m_pmPlay;
		else pm = m_pmPause;

		posx = rcABS.x1 + BATTERY_POS_X;
		posx -= (ICON_SPACE_X + m_tsw);
		posx -= ((ICON_SPACE_X+m_pmHold->m_nWidth)*3);

		op_BitBlt( &g_SysPixmap, 
			posx, rcABS.y1+ICON_BASE_Y,
			pm, 0, 0,
			pm->m_nWidth, pm->m_nHeight );
	}
	*/
}
void  CTopPanel::SetHideItems(unsigned int flags)
{
	switch(flags) /* 1 = logo, 3 = logo & title, 7 = logo & title & timer */
	{
		case 1 :
			m_fLogo = true;
			break;
		case 3 :
			m_fLogo = true;
			m_fTitle = true;
			break;
		case 7 :
			m_fLogo = true;
			m_fTitle = true;
			m_fTimer = true;
			break;
			
		default : break;

	}

}
void  CTopPanel::SetShowItems(unsigned int flags)
{
	switch(flags) /* 1 = logo, 3 = logo & title, 7 = logo & title & timer */
	{
		case 1 :
			m_fLogo = false;
			break;
		case 3 :
			m_fLogo = false;
			m_fTitle = false;
			break;
		case 7 :
			m_fLogo = false;
			m_fTitle = false;
			m_fTimer = false;
			break;
			
		default : break;

	}

}

void CTopPanel::ToggleMute( void )
{
	m_mute = !m_mute;
}
