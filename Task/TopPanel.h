//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Top Pane Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/TopPanel.h,v $
// $Revision: 1.3 $
// $Date: 2006/06/23 08:20:04 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: TopPanel.h,v $
// Revision 1.3  2006/06/23 08:20:04  zstein
// - fixed null-pointer string error.
//
// Revision 1.2  2006/06/22 01:35:43  zstein
// - fixed title bug.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.25  2006/01/03 09:32:44  etnlwind
// Update for Text Engine Modification
//
// Revision 1.24  2005/11/25 13:46:06  zstein
// *** empty log message ***
//
// Revision 1.23  2005/11/25 12:47:50  zstein
// *** empty log message ***
//
// Revision 1.22  2005/11/22 09:08:07  zstein
// *** empty log message ***
//
//*******************************************************************************

/* TOP PANEL CLASS
 * This class control state of battery level, hold, Headphone/speaker and alarm.
 */

#ifndef __TOP_PANEL_CLASS_H__
#define __TOP_PANEL_CLASS_H__

#include <Component/window.h>
#include <Component/Pixmap.h>
#include <Component/String.h>

#define SX_ALARM 201
#define SX_HOLD 215
#define SX_PLAYSTATUS 229
#define SX_SPEAKER 243
#define SX_TIME 259
#define SX_BATTERY 292

class CTopPanel : public CWindow 
{
protected:
	CPixmap* m_pmBattery[7];
	CPixmap* m_pmHold;
	CPixmap* m_pmepon;
	CPixmap* m_pmepoff;
	CPixmap* m_pmSpeakerOn;
	CPixmap* m_pmSpeakerOff;
	CPixmap* m_pmAlarm;
	CPixmap* m_pmLogo;
	CPixmap* m_pmPlay;
	CPixmap* m_pmPause;

	bool m_fHold;
	int m_nBatteryLevel;
	bool m_fAlarm;
	bool m_fSpeaker;
	//CString m_szTitle;
	bool m_t_unicode;
  int  m_szlen;
  char m_szTitle[512];
	int m_nTitleResID;
	int m_tsw;			// TimeStringWidth
	int m_current;
	int m_nPlayMode;
	bool m_fTimer;
	bool m_fLogo;
	bool m_fTitle;
	bool m_mute;

	void OnPaint( const CRect& rc );
public:
	typedef enum {
		BATTERY_NONE,
		BATTERY_LEVEL_0,
		BATTERY_LEVEL_1,
		BATTERY_LEVEL_2,
		BATTERY_LEVEL_3,
		BATTERY_LEVEL_4, 
		BATTERY_LEVEL_AC_ON, 
		BATTERY_LEVEL_AC_OFF
	} BatteryLevel;
	typedef enum {
		LOGO_ID_NML,
		LOGO_ID_MUSIC,
		LOGO_ID_VIDEO,
		LOGO_ID_RADIO,
		LOGO_ID_PHOTO,
		LOGO_ID_TEXT,
		LOGO_ID_RECORD,
		LOGO_ID_ETC,
		LOGO_ID_FILEMANAGER,
		LOGO_ID_SETUP,
		LOGO_ID_END
	} LogoType;
	enum {
		PLAYMODE_STOP,
		PLAYMODE_PAUSE,
		PLAYMODE_PLAY
	};
	
	CTopPanel( CWindow* parent, BatteryLevel level, bool fAlarm, bool fHold );
	~CTopPanel( void );

	void SetBatteryLevel( BatteryLevel level );
	void SetHold( bool fHold );
	void SetAlarm( bool fAlarm );
	void SetTitle( const CString& szTitle );
	void SetTitle( const char* szTitle, bool fUnicode );
	void SetTitle( int nResID );
	void SetLogo( LogoType id );
	void SetSpeaker( bool onoff );
	void SetPlayMode( int mode );
	void SetHideItems(unsigned int flags);
	void SetShowItems(unsigned int flags);
	void DrawTime(int isAppStatus);
	void DrawHold(int isHold, int isAppStatus=1);
	void DrawExtSpk(int isExtSpkActivated, int isAppStatus=1);
	void DrawBgPlayStat(int isExtBgPlayActivated, int isAppStatus=1);	
	BatteryLevel GetBatteryLevel( void ) { return (BatteryLevel)m_nBatteryLevel; }
	int GetPlayMode( void ) { return m_nPlayMode; }
	bool GetSpeakerOnOff( void ) { return m_fSpeaker; }
	

	void ToggleMute( void );
	bool IsMute( void ) { return m_mute; }
};

 #endif /* __TOP_PANEL_CLASS_H__ */
