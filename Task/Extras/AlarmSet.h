//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Alarm Setting
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/AlarmSet.h,v $
// $Revision: 1.2 $
// $Date: 2006/04/15 01:37:40 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: AlarmSet.h,v $
// Revision 1.2  2006/04/15 01:37:40  zstein
// - added alaram volume control.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.17  2005/12/08 08:15:58  etnlwind
// Update for Task Switching
//
// Revision 1.16  2005/11/30 11:30:11  etnlwind
// Update for AlarmSet Multi-Langul Process
//
// Revision 1.15  2005/11/23 09:33:59  etnlwind
// Update for TAVI-EQ
//
// Revision 1.14  2005/11/21 12:00:32  etnlwind
// Update for AlarmClock & Sound Palette UI
//
// Revision 1.13  2005/11/21 03:06:08  etnlwind
// Update for AlarmPopup Remove
//
// Revision 1.12  2005/11/17 08:34:17  etnlwind
// Update for AlarmSet
//
// Revision 1.11  2005/11/16 08:48:52  etnlwind
// Update for AlarmSet
//
// Revision 1.10  2005/11/10 07:35:21  akaiaphanu
// merge Extra clock & alarm
//
// Revision 1.9  2005/11/06 11:08:10  yong
// Not so many
//
// Revision 1.8  2005/10/17 11:24:25  yong
// read/write TaviSysParam
// USB connect/disconnect bug in message processing
// initial alarm set
//
// Revision 1.7  2005/10/13 05:56:54  yong
// Send touch event to ComplexListView
// work in progress
//
// Revision 1.6  2005/10/12 06:07:06  ywkim
// Add function of Resize for CComplexListView
//
// Revision 1.5  2005/10/12 05:02:42  yong
// working
//
// Revision 1.4  2005/10/12 01:56:52  akaiaphanu
// Add to log Information
//
//*****************************************************************************

/*

	Alarm Setting

	mAN-yOnG lEE <yong@NewMediaLife.com>

	2005-10-11

*/

#ifndef __ALARM_SET_H__
#define __ALARM_SET_H__

#include <time.h>

#include <tavi_lib.h>
#include "Driver/rtc/tavi_rtc.h"
#include "Driver/misc/misc_ctrl.h"
#include "Task/tavi_global.h"

#include <Component/Label.h>
#include <Component/SpinSelectBar.h>
#include <Component/RadioButton.h>
#include <Component/ComplexLabel.h>
#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>

//#include "FileSelect.h"

extern SysParameter_t	TaviSysParam;

#define _ALARMSET_NORMAL_EXIT (-1)
#define _ALARMSET_TASK_EXIT (-2)

typedef enum
{
_DEFAULT_START,
_REFRESH_BY_FILEBROWSER_SELECT,
_REFRESH_BY_FILEBROWSER_EXIT
}eUserMusicAlarmMethod;

typedef enum
{
_SPIN_SELECT_BAR=0,
_RADIO_BUTTON,
_SLIDE_BAR
} eCtrlType;

typedef enum
{
_MACROS=0,
_UNICODE_STRINGS
} eDataType;


#define TP_RIGHT		+1
#define TP_LEFT			-1
#define TP_ENTER		 1
// 0. Default Start			TaviSysParam.alarm.UserMusicFullpath:keep		csTemp: refresh
// 1. FileBrowser Select		TaviSysParam.alarm.UserMusicFullpath:refresh		csTemp: refresh
// 2. FileBrowser Exit		TaviSysParam.alarm.UserMusicFullpath:keep		csTemp: refresh


int _FitInTextSlot( uchar_t* SlotText );
int _SetUserMusicFullpath( eUserMusicAlarmMethod Method );
int _ShowTopPannel();

class CAlarmSet : public CModalWnd
{
private:
	int useAlarm;
	int hour;
	int min;
	int repeat;
	int interval;
	int AlarmType;
	int AlarmType_OriginalValue;
	int onHolidays;
    int m_AlarmVol;
	uchar_t* AlarmFullpath[260];
	char AlarmFilename[64];
	uchar_t* hoursText[24];
	uchar_t* minutesText[60];

	CString* pStr;


protected:

	int _AddItem( eCtrlType Ctrl, void* vC, eDataType Data, int vD[], uchar_t* vDD[], int nData );
	int _addSelect(char *label, int selectType, char **items, int itemCount, int selected);
	int _addSelect(int LabelTxtRes, int selectType, char **items, int itemCount, int selected);
	int addHorizSelect(char *label, char **items, int itemCount, int selected=0);
	int addRadioSelect(char *label, char **items, int itemCount, int selected=0);

public:
	CAlarmSet( );
	~CAlarmSet( void );

	CComplexListView *cListView;
	CMsgBox* pMbx;

	int Add( int LabelTxtRes, eCtrlType Ctrl, eDataType Data, int vD[], uchar_t* vDD[], int nData, int selected );
	void OnPaint( const CRect& rc );
	void Resize( const CRect& rc, bool fUpdate );
	void Init( void );

	void Show( void );
	void Hide( void );

	int OnKeyPressed(int key);
	int OnTouchPad(int p1, int p2);

	int SendEvent( CEvent &evt );
	int CurrentPos( void );

	int _SetConfig( int idx, const CString& value, int value  );
	int SetConfig( int idx, const CString& value );
	int SetConfig( int idx, int value );

	int LoadFromTaviSysParam();
	int Cancel();
	int SaveToTaviSysParam();
};

int 	AlarmSet_MsgProc(int ssTaskId, U32 ssMsgId, int usParam1 , int usParam2);
extern int 	AlarmSet_Task(void);
int	AlarmSet_Kill(void);

#endif
