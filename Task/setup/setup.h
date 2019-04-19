/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * setup.h
 * Lee Seok<zstein@newmedialife.com>
 *
 */

#ifndef __SETUP_H__
#define __SETUP_H__

#include <tavi_lib.h>

typedef enum {
	SETUP_GENERAL_WND = 0,
	SETUP_SOUND_WND,
	SETUP_DISPLAY_WND,
	SETUP_POWER_WND,
	SETUP_CLOCK_WND,
	SETUP_NETWORK_WND,

	SETUP_ROOT = 0xff
}	SETUP_SUBMENU_t;

typedef struct _tag_SETUP_CONFIG {
	CRect m_rcList;
	CRect m_rcBar;
	CListView* m_puzListView;
	CWindow* m_FocusedWindow;
	CWindow* m_ChildSetupWnd;
	CList< CWindow > m_WndList;
	SETUP_SUBMENU_t m_tCurrentMenu;
	bool m_fExit;
	int 	m_ExitState;
	int	m_OnConfirm;
	CMsgBox* m_pMsgBox;
	CWindow *m_pTvAdjust;
	CWindow *m_pUserEffect;
	CWindow *m_pUserEQ;
	CWindow *m_pAdjustBright;
	int	m_nTaskID;
} _ATTRIBUTE_PACKED_ TSETUP_CONFIG;

#define DELAYTIME_HDD_SLEEP	10			// delay time for hdd poser save


int 	Setup_MsgProc( int ssTaskId, U32 ssMsgId, int usParam1 , int usParam2 );
int 	Setup_Task( TTaskInitParam* tParam );
int 	Setup_Kill(void);
#endif /* __SETUP_H__ */


