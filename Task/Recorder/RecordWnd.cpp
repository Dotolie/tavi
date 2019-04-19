//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Recorder Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecordWnd.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: RecordWnd.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.40  2006/02/15 05:38:22  zstein
// *** empty log message ***
//
// Revision 1.39  2006/02/09 05:19:32  zstein
// - removed pop-noise.
//
// Revision 1.38  2006/02/08 09:22:14  zstein
// - fixed a bug
//
// Revision 1.37  2006/02/03 10:25:38  zstein
// - remove time limit.
//
// Revision 1.36  2006/01/16 11:11:28  zstein
// *** empty log message ***
//
// Revision 1.35  2006/01/16 02:36:40  zstein
// *** empty log message ***
//
// Revision 1.34  2006/01/06 11:39:45  zstein
// *** empty log message ***
//
// Revision 1.33  2006/01/02 01:53:19  zstein
// - fixed formatting filename bug.
//
// Revision 1.32  2005/12/27 02:21:06  zstein
// *** empty log message ***
//
// Revision 1.31  2005/12/16 10:13:23  zstein
// *** empty log message ***
//
// Revision 1.30  2005/12/10 06:39:53  zstein
// *** empty log message ***
//
// Revision 1.29  2005/12/10 06:37:27  ywkim
// Move TAVI_CannotSleep() to front in CRecorderWnd()
//
// Revision 1.28  2005/12/07 04:36:27  zstein
// *** empty log message ***
//
// Revision 1.27  2005/12/01 09:28:02  zstein
// *** empty log message ***
//
// Revision 1.26  2005/11/24 03:03:56  zstein
// *** empty log message ***
//
// Revision 1.25  2005/11/22 09:07:10  zstein
// *** empty log message ***
//
// Revision 1.24  2005/11/18 09:14:34  zstein
// *** empty log message ***
//
// Revision 1.23  2005/11/17 11:20:22  zstein
// *** empty log message ***
//
// Revision 1.22  2005/11/16 07:13:07  zstein
// *** empty log message ***
//
// Revision 1.21  2005/11/11 12:29:20  zstein
// *** empty log message ***
//
// Revision 1.20  2005/11/10 01:09:16  zstein
// *** empty log message ***
//
//*******************************************************************************

#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <Lib/LibImage.h>
#include <Component/FileUtil.h>
#include <Component/Bitmap.h>
#include <Task/tavi_global.h>
#include "RecorderMain.h"
#include "RecordWnd.h"
#include <Task/MP3Rec.h>
#include <Task/ExtLCD.h>
#include <Task/tavi_mq.h>
#include <Task/TopPanel.h>
#include <Task/AudioPlayer/AudioCmd.h>


// Macros
#define FRAME_SIZE		576		// 144000*128/32000; version 1, layer 3, bitrate 128kbps, samplingrate 32000
//#define MIN_SPACE		4		// 4MB

// Extern variables
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern SysParameter_t TaviSysParam;
extern CExtLCD* g_ExtLCD;
extern CTopPanel* g_TopPanel;

// Local variables
static int nBitrates[] = { 8, 16, 20, 24, 32 };
static int nSamplerates[] = { 32000, 44100 };
static int nHideCount;

static int OnAutoGainOn( RMuint8* arg )
{
	CRecorderWnd* pRecWnd = ( CRecorderWnd* )arg;
	pRecWnd->AGCOn();
}

static int OnAutoGainOff( RMuint8* arg )
{
	CRecorderWnd* pRecWnd = ( CRecorderWnd* )arg;
	pRecWnd->AGCOff();
}

static int OnBitrate64Kbps( RMuint8* arg )
{
	CRecorderWnd* pRecWnd = ( CRecorderWnd* )arg;
	pRecWnd->SetBitrate( BITRATE_64K );
}

static int OnBitrate128Kbps( RMuint8* arg )
{
	CRecorderWnd* pRecWnd = ( CRecorderWnd* )arg;
	pRecWnd->SetBitrate( BITRATE_128K );
}


static char* GenerateFilename( int source )
{
	char* p = new char[64];
	int count=0;
	time_t t;
	struct tm* tm;
	char* type;

	switch( source ) {
	case CRecorderWnd::RECORD_VOICE: 
		type = "Voice";
		break;
	case CRecorderWnd::RECORD_LINEIN:
		type = "LineIN";
		break;
	case CRecorderWnd::RECORD_FM:
		type = "Radio";
		break;
	}
	
__re_generate:
	time( &t );
	tm = localtime( &t );
	if( count == 0 ) {
		sprintf( p, "%04d%02d%02d_%02d%02d_%s.mp3",
			tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, 
			tm->tm_hour, tm->tm_min, type );
	}
	else {
		sprintf( p, "%04d%02d%02d_%02d%02d_%s_#%d.mp3",
			tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, 
			tm->tm_hour, tm->tm_min, type, count );
	}
	// Check filename exists
	RMuint32 dir;
	FATFS_ERROR error;
	FATFS_DIR_ENTRY_INFO dinfo;
	error = fatfs_dopen( (RMuint8*)_RECORD_DIR, &dir );
	if( error == FATFS_ERROR_NO_ERROR ) {
		char name[1024];
		while( (error=fatfs_dread(dir, &dinfo)) == FATFS_ERROR_NO_ERROR ) {
			ustr2str( name, (uchar_t*)dinfo.name );
			if( strcmp(name, p) == 0 ) {
				fatfs_dclose( dir );
				count++;
				goto __re_generate;
			}
		}
		fatfs_dclose( dir );
	}
	else {
		printf( "Generatefilename error: 0x%x\n", error );
	}

	return p;
}

CRecorderWnd::CRecorderWnd( CWindow* parent, const CString& szSavePath, RecordType type, int Hz ) :
	CWindow( parent ),
	m_szSavePath( szSavePath ),
	m_Type( type ),
	m_Hz( Hz )
{
	int iw, ih;
	char* p, buf[50];

	m_fValid = true;
	m_fRecord = false;

    g_MiscCtrl->Mute( true );
    
	TAVI_CannotSleep( TASK_ID_RECORD );

	int w = TAVI_IMAGES[RECORD_BIG_ICON].bitmapinfoheader.biWidth;
	int h = TAVI_IMAGES[RECORD_BIG_ICON].bitmapinfoheader.biHeight;

	m_pmRecord = new CPixmap;
	m_pmRecord->CreatePixmap( w, h, 8, (char*)TAVI_IMAGES[RECORD_BIG_ICON].image_buffer, false );

	iw = m_pmRecord->m_nWidth;
	ih = m_pmRecord->m_nHeight;

	m_plbSavingPath = new CLabel( this, COLOR_WHITE, 0, "" );
	m_plbSavingPath->Resize( CRect(30+iw+8, 70, 319, 84), false );
	m_plbSavingPath->m_nFontHeight = 12;
	
	int nFreeSpace = GetFreeSpace( "/hd1" );
	//nFreeSpace -= MIN_SPACE; // minimum hdd space : 4mb
	if( nFreeSpace < 0 ) nFreeSpace = 0;
	sprintf( buf, "Free Space : %dMB", nFreeSpace );
	m_plbFree = new CLabel( this, COLOR_WHITE, 0, buf );
	m_plbFree->Resize( CRect(90, 151, 260, 168), false );
	m_plbFree->m_nFontHeight = 12;

	float f = (float)(1024*1024)/(float)FRAME_SIZE;
	int nFrameCount = (int)((float)nFreeSpace*f); 
	int nTotalTime = (int)(((float)nFrameCount/27.8f ) + 0.5); // 32KHz fixed

	m_pBottomWnd = new CRecordBottomWnd( this, 0, nTotalTime );
	m_pBottomWnd->Resize( CRect(0, 169, 319, 209), false );

	m_pVolCtrl = new CRecVolCtrl( this );
	m_pVolCtrl->Resize( CRect(0, 169, 319, 209), false );
	m_pVolCtrl->SetVisible( false );

	// Initalize mp3 record
	if( (signed)TaviSysParam.record.bitrate < 0 || (signed)TaviSysParam.record.bitrate > 3 ) 
		TaviSysParam.record.bitrate = 0;
	if( (signed)TaviSysParam.record.samplingrate < 0 || (signed)TaviSysParam.record.samplingrate > 2 )
		TaviSysParam.record.samplingrate = 0;
	
	CString tmp = "Source: ";
	int major, minor;
	switch( m_Type ) {
	case RECORD_VOICE:
		g_SndCtrl->RecordMode( CSndCtrl::RECORD_MIC );
		tmp += "Voice";
		break;
	case RECORD_LINEIN:
		g_SndCtrl->RecordMode( CSndCtrl::RECORD_LINEIN );
		tmp += "Line-In";
		g_SndCtrl->DACMute( true );
            g_SndCtrl->LineInBypass( true, m_Type );
		break;
	case RECORD_FM:
		g_SndCtrl->RecordMode( CSndCtrl::RECORD_FM );
		major = m_Hz/1000;
		minor = (m_Hz-major*1000)/10;
		sprintf( buf, "Source: Radio %3d.%02dMHz", major, minor );
		tmp = buf;
		break;
	}
	m_plbInfo = new CLabel( this, COLOR_WHITE, 0, (char*)tmp );
	m_plbInfo->Resize( CRect(30+iw+8, 86, 310, 106 ), false );
	m_plbInfo->m_nFontHeight = 12;

	g_SndCtrl->SetLineInMute( false );
	g_SndCtrl->SetLineInVolume( TaviSysParam.sound.input_vol );
	m_fRecording = false;

	// Create Context Menu
	m_pCtxMenuWnd = new CContextMenu( this );
	m_pCtxMenuWnd->SetVisible( false );

	m_pMsgBox = NULL;

	// ext. lcd
	if( g_MiscCtrl->FolderOpen() == false ) 
		g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_PLAY );

	g_SysPixmap.Fill( 0, 30, 320, 210, 0 );

	m_nCurrentTime = 0;

    mep_eat_event();

    g_MiscCtrl->Mute( false );
}

CRecorderWnd::~CRecorderWnd( void )
{
    if( m_Type == RECORD_VOICE ) {
        g_MiscCtrl->Mute( true );
    }
    
	if( m_fRecording ) Pause();
	if( m_pmRecord ) delete m_pmRecord;
	if( m_pMsgBox ) {
		delete m_pMsgBox;
		m_pMsgBox = NULL;
	}

	g_SndCtrl->SetLineInMute( true );
	g_SndCtrl->PlaybackMode();

	// Re-mount filesystem
	if( m_fRecord ) {
		m_pMsgBox = new CMsgBox( ML_INFORMATION, _ORANGE, ML_MSG_PLZ_WAIT, 244, 123, 0, DL_BG_ICON );
		m_pMsgBox->Show();
		
		QID_t qid = SysMSGInit( AUDIO_MSG_KEY );
		AudioCmd* pCmd = new AudioCmd;
		QBUF_t* pBuf = ( QBUF_t* )pCmd;
		pCmd->lMsgID = MSG_SEND_ID; // don't set 0
		pCmd->lCmd = AUDIO_CMD_REMOUNT;
		pCmd->lLength = sizeof(AudioCmd) - 4;
		pCmd->szArg[0] = TaviSysParam.sound.eq;
		SysMSGSend( qid, pBuf, pCmd->lLength, 0 );
		delete pCmd;
		// until mounting hdd done.
		int t;
		struct timeval tv;
		gettimeofday( &tv, NULL );
		t = tv.tv_sec;
		while( abs(tv.tv_sec-t) < 3 ) {
			sleep( 1 );
			gettimeofday( &tv, NULL );
		}
		if( m_pMsgBox ) {
			delete m_pMsgBox;
			m_pMsgBox = NULL;
		}
	}
	

	TAVI_CanSleep( TASK_ID_RECORD );

    if( m_Type == RECORD_VOICE ) {
        g_MiscCtrl->Mute( false );
    }
}

void CRecorderWnd::OnKeypressed( const KeyEvent& evt )
{
	CEvent event;

	event.type = EVENT_KEY;
	event.u.key = evt;

	if( m_fRecording ) {
		switch( evt.value ) {
		case TAVI_KEY_JOG2_ENTER:
		case VK_ENTER:
			Pause();
			break;
		case TAVI_KEY_JOG1_ENTER:
		case VK_ESC:
			SendMessage( WINDOW_KILL );
			break;
		default:
			if( !m_pMsgBox ) {
				m_pMsgBox = new CMsgBox( ML_INFORMATION, _ORANGE, ML_MSG_RECORDING, 244, 123, 0, DL_BG_ICON );
				m_pMsgBox->Show();
				nHideCount = 0;
			}
		}
	}
	else {
		switch( evt.value ) {
		case VK_UP: // line-in volume up
		case VK_DOWN: // line-in volume down
			if( !m_fRecording ) m_pVolCtrl->SetFocus();
			break;
		case TAVI_KEY_JOG1_ENTER:
			if( g_MiscCtrl->FolderOpen() ) break;
		case VK_F3:
		case VK_ESC:
		case TAVI_KEY_LMODE:
			// exit
			SendMessage( WINDOW_KILL );
			break;
		case TAVI_KEY_JOG2_ENTER:
			if( g_MiscCtrl->FolderOpen() ) break;
		case VK_ENTER:
			Record();
			break;
		case VK_F2:
			CreateSubMenu();
			m_pCtxMenuWnd->SetFocus();
			break;
		}
	}
}

void CRecorderWnd::OnMouseEvent( const MouseEvent& evt )
{
	if( m_fRecording ) return;
	if( m_fVisible == false ) return;
	
	CEvent event;
	event.type = EVENT_MOUSE;
	event.u.mouse = evt;

	SendEvent( event, m_pVolCtrl );
}

#include <sys/time.h>
void CRecorderWnd::OnWindowEvent( const WindowEvent& evt )
{
	CEvent event;
	struct timeval tv;

	switch( evt.nMessage ) {
	case WINDOW_IDLE:
		if( m_fRecording ) {
			MP3Rec_Recording();
			MP3Rec_Encoding();
			gettimeofday( &tv, NULL );
			if( abs(tv.tv_sec - m_tv.tv_sec) >= 1 ) {
				m_tv = tv;
				m_nCurrentTime++;
				if( m_pMsgBox ) nHideCount++;
				else {
					#if 0
					int nFreeSpace;
					if( m_nCurrentTime%10 == 0 ) {
						nFreeSpace = GetFreeSpace( "/hd1" );
						if( nFreeSpace < 0 ) nFreeSpace = 0;
						if( nFreeSpace == 0 ) {
							Pause();
							return;
						}

						if( g_TopPanel->GetBatteryLevel() == CTopPanel::BATTERY_LEVEL_0 ) {
							Pause();
							return;
						}
					}
					#else
					if( m_nCurrentTime >= m_pBottomWnd->GetTotalTime() ) {
						Pause();
						return;
					}
					#endif
					DrawBigFontTime();
					m_pBottomWnd->UpdateCurrentTime( m_nCurrentTime, true );
					if( m_pMsgBox ) nHideCount++;
				}
			}

			if( nHideCount > 3 ) { // after 3 seconds, hide message box
				delete m_pMsgBox;
				m_pMsgBox = 0;
				nHideCount = 0;
				Show();
			}
			MP3Rec_Encoding();
		}
		else {
			if( g_MiscCtrl->FolderOpen() == false ) g_ExtLCD->ScrollText();
		}
		break;
	case ON_FOLDER:
		if( evt.nParam ) g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_READY );
		else g_ExtLCD->ChangeMode( CExtLCD::EXT_STAGE_PLAY );
		g_ExtLCD->Update();
		break;
	case WINDOW_NOTIFY:
		if( m_Type == RECORD_LINEIN ) {
			SendMessage( WINDOW_KILL );
		}
		break;
	}
}

void CRecorderWnd::OnPaint( const CRect& rc )
{
	DrawBigFontTime();
	
	// Draw Record image
	op_BitBlt( &g_SysPixmap, rcABS.x1+30, rcABS.y1+20, 
		m_pmRecord, 0, 0, m_pmRecord->m_nWidth, m_pmRecord->m_nHeight );

	g_ExtLCD->SetPlayTitle( m_szSaveFilename );
	if( m_fRecording ) g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_RECORD );
	else g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_PAUSE );
	if( g_MiscCtrl->FolderOpen() == false ) g_ExtLCD->Update();
}

void CRecorderWnd::Record( void )
{
	int nFreeSpace = GetFreeSpace( "/hd1" );

	// CHECK BATTERY LEVEL
	if(  g_TopPanel->GetBatteryLevel() == CTopPanel::BATTERY_LEVEL_0 ) {
		m_pMsgBox = new CMsgBox( ML_INFORMATION, _ORANGE, ML_LOW_BETTRY, 244, 123, 0, DL_BG_ICON );
		m_pMsgBox->Show();
		for( int i=0; i<30; i++ ) sleep( 1 );
		SendMessage( WINDOW_KILL );
		return;
	}
	
	if( nFreeSpace <= 0 ) {
		m_pMsgBox = new CMsgBox( ML_INFORMATION, _ORANGE, ML_DISK_FULL, 244, 123, 0, DL_BG_ICON );
		m_pMsgBox->Show();
		for( int i=0; i<30; i++ ) sleep( 1 );
		delete m_pMsgBox;
		Show();
		//OnPaint( rcClient );
		return;
	}

	// KEY SAMPLING SPEED DOWN
	extern int handle_key;
	int speed = 5;
	if( ioctl( handle_key, TAVI_KEY_IOCS_SPEED, &speed ) < 0 ) {
		perror( "ioctl(TAVI_KEY_IOCS_SPEED)\n" );
	}

	int bitrate;
	switch( TaviSysParam.record.bitrate ) {
	case BITRATE_64K: 
		bitrate = 64;
		break;
	case BITRATE_128K:
		bitrate = 128;
		break;
	default:
		bitrate = 128;
		break;
	}
	
	if( MP3Rec_Init( bitrate, 32000, 2 ) < 0 ) 
		m_fValid = false;
	else m_fValid = true;

	if( m_fValid == false ) {
		DrawMessageBox( "Record", "MP3 encoding module has some problem.", 10, 79, 300, 100 );
		sleep( 30 );
		m_fRecording = false;
		OnPaint( rcClient );
		return;
	}

	char* p = GenerateFilename( m_Type );
	m_szSaveFilename = p;
	delete[] p;
	m_plbSavingPath->SetText( m_szSaveFilename );
	g_ExtLCD->SetPlayTitle( m_szSaveFilename );
	if( g_MiscCtrl->FolderOpen() == false ) g_ExtLCD->Update();
	
	CString szRecordingPath = m_szSavePath + "/";
	szRecordingPath += m_szSaveFilename;
	if( MP3Rec_SetEncodingPath( (char*)szRecordingPath ) == false ) {
		DrawMessageBox( "Record", "Can't create file.", 10, 79, 300, 100 );
		sleep( 30 );
		m_fRecording = false;
		MP3Rec_Exit();
		OnPaint( rcClient );
		return;
	}
	g_pFocusedWnd = this;
	m_pVolCtrl->SetVisible( false );
	m_pBottomWnd->Show();
	
	MP3Rec_Start();

	m_fRecording = true;

	m_pBottomWnd->SetStatus( 1 );

	m_nCurrentTime = 0;
	m_fRecord = true;
	nHideCount = 0;
	
	gettimeofday( &m_tv, NULL );

	ClearKeyBuffer();

	// Ext. LCD
	g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_RECORD );
	if( g_MiscCtrl->FolderOpen() == false ) g_ExtLCD->Update();
	REC_SetProperty( REC_PROP_M_RECORDING, true );
}

void CRecorderWnd::Pause( void )
{
	MP3Rec_Stop();
	MP3Rec_Exit();
	if( m_pMsgBox ) delete m_pMsgBox;
	m_pMsgBox = NULL;

    if( m_nCurrentTime < 5 ) {
        char* path;
        CString szRecordingPath = m_szSavePath + "/";
        szRecordingPath += m_szSaveFilename;
        path = ( char* )szRecordingPath;
        fatfs_delete_file( (RMuint8*)path );
    }
	
	m_fRecording = false;

	// Ext. LCD
	g_ExtLCD->SetPlayTitle( m_szSaveFilename );
	g_ExtLCD->SetPlayState( CExtLCD::EXT_LCD_PAUSE );
	if( g_MiscCtrl->FolderOpen() == false ) g_ExtLCD->Update();
	REC_SetProperty( REC_PROP_M_RECORDING, false );

	int nFreeSpace = GetFreeSpace( "/hd1" );
	char buf[60];
	sprintf( buf, "Free Space : %dMB", nFreeSpace );
	m_plbFree->SetText( buf, false );

	m_pBottomWnd->SetStatus( 0 );
	m_pBottomWnd->UpdateCurrentTime( 0, false );
	m_nCurrentTime = 0;
	Show();

	ClearKeyBuffer();

	// RESOTRE KEY SAMPLING SPEED(MAXIMUM SPEED)
	extern int handle_key;
	int speed = 10;
	if( ioctl( handle_key, TAVI_KEY_IOCS_SPEED, &speed ) < 0 ) {
		perror( "ioctl(TAVI_KEY_IOCS_SPEED)\n" );
	}
}

void CRecorderWnd::AGCOn( void )
{
	m_pCtxMenuWnd->Hide();
	TaviSysParam.record.AGC = 1;
	if( m_Type == RECORD_VOICE )
		g_SndCtrl->ALCOn( true, 0 );
	else 
		g_SndCtrl->ALCOn( true, 1 );
	m_pBottomWnd->Show();
	mep_eat_event();
}

void CRecorderWnd::AGCOff( void )
{
	m_pCtxMenuWnd->Hide();
	TaviSysParam.record.AGC = 0;
	g_SndCtrl->ALCOn( false, 0 );
	m_pBottomWnd->Show();
	mep_eat_event();
}



void CRecorderWnd::CreateSubMenu( void )
{
	// Auto gain menu
	bool fAutoGain = TaviSysParam.record.AGC;
	int nSelect;

	m_pCtxMenuWnd->Reset();
	CPopUpSubMenu* pSubMenu;
	#if 0
	CPopUpSubMenu* pSubMenu = new CPopUpSubMenu;
	bool bitrate_64k=false, bitrate_128k=false;
	if( TaviSysParam.record.bitrate == BITRATE_64K ) bitrate_64k = true;
	else if( TaviSysParam.record.bitrate == BITRATE_128K ) bitrate_128k = true;
	pSubMenu->AddSubMenuItem( ML_REC_BITRATE_64K, bitrate_64k, OnBitrate64Kbps, NULL );
	pSubMenu->AddSubMenuItem( ML_REC_BITRATE_128K, bitrate_128k, OnBitrate128Kbps, NULL );
	nSelect = bitrate_64k ? 0 : 1;
	pSubMenu->PointedMenuItemIdx = nSelect;
	m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_REC_BITRATE, pSubMenu, nSelect, NULL, NULL );
	#endif
	pSubMenu = new CPopUpSubMenu;
	pSubMenu->AddSubMenuItem( ML_AUTO_GAIN_ON, fAutoGain, OnAutoGainOn, (RMuint8*)this );
	pSubMenu->AddSubMenuItem( ML_AUTO_GAIN_OFF, !fAutoGain, OnAutoGainOff, (RMuint8*)this );
	nSelect = fAutoGain ? 0 : 1;
	pSubMenu->PointedMenuItemIdx = nSelect;
	m_pCtxMenuWnd->m_pCtxMenu->AddSubMenu( ML_AUTO_GAIN, pSubMenu, nSelect, NULL, NULL );
}

void CRecorderWnd::DrawBigFontTime( void )
{
	int hr = m_nCurrentTime/3600;
	int m = (m_nCurrentTime-hr*3600)/60;
	int s = m_nCurrentTime%60;
	int tp; // ten's place
	int op; // one's place
	int w = TAVI_IMAGES[LARGE_DIGIT].bitmapinfoheader.biWidth;
	int h = 37;
	int y = rcABS.y1 + 20;
	int x;
	unsigned char* base = (unsigned char*)TAVI_IMAGES[LARGE_DIGIT].image_buffer;
	unsigned char* p;

	//if( !p ) return;

	tp = hr/10;
	op = hr%10;
	// hour
	p = base + (tp+3)*w*h;
	x = 100;
	pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, x, y, w, h );
	p = base + (op+3)*w*h;
	x += w-8;
	pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, x, y, w, h );
	// colon
	p = base + 13*w*h;
	x += w-7;
	pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, x, y, w, h );
	tp = m/10;
	op = m%10;
	// minute
	p = base + (tp+3)*w*h;
	x += 11;
	pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, x, y, w, h );
	p = base + (op+3)*w*h;
	x += w-8;
	pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, x, y, w, h );
	// colon
	p = base + 13*w*h;
	x += w-7;
	pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, x, y, w, h );
	tp = s/10;
	op = s%10;
	// second
	p = base + (tp+3)*w*h;
	x += 11;
	pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, x, y, w, h );
	p = base + (op+3)*w*h;
	x += w-8;
	pMpegDecoder->RenderOsdBits_WithoutRefresh ( p, x, y, w, h );
}

void CRecorderWnd::Show( void )
{
	CWindow::Show();
	mep_eat_event();
}

void CRecorderWnd::SetBitrate( int bitrate )
{
	TaviSysParam.record.bitrate = bitrate;
	if( bitrate == BITRATE_64K ) m_pBottomWnd->UpdateBitrate( 64 );
	else m_pBottomWnd->UpdateBitrate( 128 );
	Flush();
	mep_eat_event();
}
