//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Ext. LCD UI class header
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/ExtLCD.h,v $
// $Revision: 1.2 $
// $Date: 2006/03/09 01:57:13 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ExtLCD.h,v $
// Revision 1.2  2006/03/09 01:57:13  zstein
// - changed the value of VOLUME_RANGE to 40.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.20  2006/01/27 11:55:29  yong
// implement DrawSeek() as graphical one like DrawVolume()
//
// Revision 1.19  2006/01/27 06:46:42  zstein
// *** empty log message ***
//
// Revision 1.18  2006/01/26 06:02:13  zstein
// *** empty log message ***
//
// Revision 1.17  2006/01/19 08:42:31  zstein
// *** empty log message ***
//
// Revision 1.16  2005/12/28 12:20:03  yong
// add message for loading music db & scaning folder
//
// Revision 1.15  2005/12/26 04:15:50  zstein
// - changed ::UpdateSeek( const String&, int len ) to
// 	::UpdateSeek( int total, int current )
//
// Revision 1.14  2005/12/20 03:14:32  yong
// add m_LastMode
//
// Revision 1.13  2005/12/19 10:44:02  yong
// USB icon added
//
// Revision 1.12  2005/12/14 12:55:41  yong
// radio related icon fix
//
// Revision 1.11  2005/12/14 11:27:06  yong
// changes in external LCD icons
//
// Revision 1.10  2005/12/12 10:43:16  yong
// add new LCD icons
//
// Revision 1.9  2005/12/11 18:15:31  yong
// DrawVolume() now use graphics
//
// Revision 1.8  2005/11/25 10:22:57  zstein
// *** empty log message ***
//
// Revision 1.7  2005/10/07 06:39:44  zstein
// - updated volume UI.
//
// Revision 1.6  2005/09/28 02:52:47  zstein
// - added key lock message
//
// Revision 1.5  2005/09/09 05:48:40  zstein
// - added usb connect/disconnect message
//
// Revision 1.4  2005/08/29 07:45:21  zstein
// - added volume&seek
//
// Revision 1.3  2005/08/25 07:56:48  zstein
// - added ResetScroll()
//
// Revision 1.2  2005/07/16 04:28:42  zstein
// *** empty log message ***
//
// Revision 1.1  2005/06/21 06:03:43  zstein
// - [NEW] external lcd ui interface
//
//*******************************************************************************

#ifndef __EXTERNAL_LCD_CLASS__
#define __EXTERNAL_LCD_CLASS__

#include <Component/String.h>
#include <Component/Pixmap.h>
#include <Component/ListView.h>
#include "SubLCD.h"

#define EXT_LCD_SCROLL_INTERVAL	150000	// 100ms
#define VOLUME_RANGE			40
class CExtLCD {
public:
	typedef enum {
		EXT_STAGE_READY,
		EXT_STAGE_MENU,
		EXT_STAGE_LIST,
		EXT_STAGE_PLAY,
		EXT_STAGE_VOLUME,
		EXT_STAGE_SEEK,
		EXT_STAGE_USB,
		EXT_STAGE_MESSAGE
	} ExtLCD_Mode;

	typedef enum {
		EXT_MENU_MUSIC,
		EXT_MENU_RADIO,
	} ExtLCD_Menu;

	typedef enum {
		EXT_LCD_BATTERY_NONE,
		EXT_LCD_BATTERY_0,
		EXT_LCD_BATTERY_1,
		EXT_LCD_BATTERY_2,
		EXT_LCD_BATTERY_3,
		EXT_LCD_BATTERY_4,
		EXT_LCD_BATTERY_AC
	} ExtLCD_Battery;

	enum {
		EXT_LCD_ICON_PLAY,
		EXT_LCD_ICON_PAUSE,
		EXT_LCD_ICON_FF,
		EXT_LCD_ICON_REW,
		EXT_LCD_ICON_RADIO,
		EXT_LCD_ICON_MUSIC,
		EXT_LCD_ICON_ANTENNA,
		EXT_LCD_ICON_RECORD,
		EXT_LCD_ICON_FOLDER,

		EXT_LCD_ICON_NEW_PLAY=100,
		EXT_LCD_ICON_NEW_PAUSE,
		EXT_LCD_ICON_NEW_PLAYLIST,
		EXT_LCD_ICON_NEW_FOLDER,
		EXT_LCD_ICON_NEW_ARTIST,
		EXT_LCD_ICON_NEW_ALBUM,
		EXT_LCD_ICON_NEW_GENRE,
		EXT_LCD_ICON_NEW_ALLTRACKS,
		EXT_LCD_ICON_NEW_MUSIC,
		EXT_LCD_ICON_NEW_RADIO,
		EXT_LCD_ICON_NEW_USB,
		EXT_LCD_ICON_NEW_HELP,
	};

	typedef struct {
		int m_nIcon;
		int m_nIndex;
		int m_nTotal;
		CString m_szText;
	} ExtLCDListItem;

	enum {
		EXT_LCD_PLAY,
		EXT_LCD_PAUSE,
		EXT_LCD_FF,
		EXT_LCD_REW,
		EXT_LCD_RECORD
	};

	enum {
		EXT_MSG_UNKNOWN,
		EXT_MSG_USB_CONNECT,
		EXT_MSG_USB_DISCONNECT,
		EXT_MSG_KEY_LOCKED,
		EXT_MSG_KEY_UNLOCKED,
		EXT_MSG_LOAD_MUSIC_DB,
		EXT_MSG_SCAN_MUSIC_FOLDER,
		EXT_MSG_END
	};

protected:
	int m_nBatteryLevel;
	ExtLCDListItem m_ListItem;
	ExtLCD_Menu m_CurrentMenu;
	ExtLCD_Mode m_LastMode;
	ExtLCD_Mode m_CurrentMode;

	CPixmap* m_pmFullScreen;
	CPixmap* m_pmMenu[4];
	CPixmap* m_pmMainFrm;
	CPixmap* m_pmAM;
	CPixmap* m_pmPM;
	CPixmap* m_pmBattery[6];
	CPixmap* m_pmIcons[10];
	CPixmap* m_pmSpin[2];
	CPixmap* m_pmBuffer;
	CPixmap* m_pmIconsNew[14];

	CPixmap* m_pmBarThumb;
	CPixmap* m_pmSpeaker;
	CPixmap* m_pmSearch;

	int m_nListTextWidth;
	int m_nScrollCount;
	int m_fStartScroll;
	int m_nPlayState;
	int m_nStartScrollTime;
	int m_nLastus;
	int m_nIdleCount;
	int m_nVolume;
	int m_nMsgIcon;
	CString m_szMessage;
	CString m_szTitle;

	int m_nModeTime;

	void DrawReady( void );
	void DrawMenu( void );
	void DrawList( void );
	void DrawPlay( void );
	void DrawBattery( void );
	void DrawString( CString& szText, int x, int y, int w, int h );
	void DrawPlayState( void );
	void _DrawProgressBar( CPixmap *ico, int cur, int total, CString s, int slen);
	void DrawVolume( void );
	void DrawSeek( int cur, int total );
	void DrawMessage( void );
	void Paint( void );

public:
	
    CExtLCD( void );
    ~CExtLCD( void );

    void ChangeMode( ExtLCD_Mode mode );
    void ChangeMenu( ExtLCD_Menu menu );
    void SetBatteryLevel( ExtLCD_Battery level );
    void Update( void );
    void Flush( void );
    inline ExtLCD_Mode GetCurrentMode( void ) { return m_CurrentMode; }
    inline ExtLCD_Menu GetCurrentMenu( void ) { return m_CurrentMenu; }
    void NextMenu( void );
    void PrevMenu( void );
    void SetListItem( const ExtLCDListItem& item );
    void SetListItem( const CListItem* pitem, int total );
    void SetPlayState( int nPlayState );
    void SetPlayTitle( const CString& szTitle );
    void SetVolume( int vol );
    void SetMessage( uchar_t* msg );
    void UpdateBattery( ExtLCD_Battery level );
    void UpdatePlayState( int nPlayState );
    void UpdateVolume( int vol );
    void UpdateSeek( int totaltime, int currenttime );
    void ScrollText( void );
    void ResetScroll( void );
    void Clear( void );
    void ReservedMessage( int message );

    CPixmap *getPixmap(int);
    void IdleAction(void);
};

extern void SetListItem( const CListItem* pitem, int total );

#endif /* __EXTERNAL_LCD_CLASS__ */
