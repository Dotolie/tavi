//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Extra Task
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/Extra.cpp,v $
// $Revision: 1.3 $
// $Date: 2006/03/06 01:58:08 $
// $Author: akaiaphanu $
//
//*****************************************************************************
// $Log: Extra.cpp,v $
// Revision 1.3  2006/03/06 01:58:08  akaiaphanu
// working mouse event
//
// Revision 1.2  2006/02/27 02:40:17  akaiaphanu
// working
//
// Revision 1.36  2006/02/20 03:04:30  akaiaphanu
// add function for modekey
//
// Revision 1.35  2006/02/11 09:16:28  akaiaphanu
// working text resource
//
// Revision 1.34  2006/02/11 07:19:11  akaiaphanu
// work for dictionary
//
// Revision 1.33  2005/12/14 03:30:14  akaiaphanu
// cleaning
//
// Revision 1.32  2005/12/14 02:35:19  akaiaphanu
// cleaning functions
//
// Revision 1.31  2005/12/13 07:44:21  akaiaphanu
// cleaning
//
// Revision 1.30  2005/12/12 06:48:45  akaiaphanu
// Updated to HDD Sleep , Background and Switch mode
//
// Revision 1.29  2005/12/08 08:57:03  etnlwind
// Update for Alarm Task
//
// Revision 1.28  2005/12/05 03:24:04  akaiaphanu
// add to bottompanel function
//
// Revision 1.27  2005/12/01 02:50:11  akaiaphanu
// cleaning functions
//
// Revision 1.26  2005/11/25 02:08:54  akaiaphanu
// cleaning functions
//
// Revision 1.25  2005/11/19 05:00:13  akaiaphanu
// updated to 100ms sleep function in clockmode & cleaning
//
// Revision 1.24  2005/11/18 06:22:49  akaiaphanu
// add to function jogkey2
//
// Revision 1.23  2005/11/17 09:04:33  akaiaphanu
// cleaning functions
//
// Revision 1.22  2005/11/15 02:28:12  akaiaphanu
// *** empty log message ***
//
// Revision 1.21  2005/11/15 02:20:12  akaiaphanu
// Updated to process alarmworking
//
// Revision 1.20  2005/11/10 08:53:25  zstein
// *** empty log message ***
//
// Revision 1.19  2005/11/10 07:35:21  akaiaphanu
// merge Extra clock & alarm
//
// Revision 1.18  2005/11/09 06:38:50  akaiaphanu
// updated to timeformat & cleaning
//
// Revision 1.17  2005/11/08 08:10:54  akaiaphanu
// updated to long exit key & draw date func
//
// Revision 1.16  2005/10/28 06:49:13  akaiaphanu
// updated to alarm
//
// Revision 1.15  2005/10/27 08:30:43  akaiaphanu
// updated & cleaning
//
// Revision 1.14  2005/10/27 02:49:11  akaiaphanu
// modify sleep mode
//
// Revision 1.13  2005/10/25 09:48:26  akaiaphanu
// add to function HDD Sleep
//
// Revision 1.12  2005/10/24 02:31:28  akaiaphanu
// add to adjust volume on the background play
//
// Revision 1.11  2005/10/21 01:44:12  akaiaphanu
// add to icon in Extra MainScreen
//
// Revision 1.10  2005/10/19 09:48:12  etnlwind
// Update for Startup Progress Display
//
// Revision 1.9  2005/10/19 06:15:07  akaiaphanu
// Cleanning
//
// Revision 1.8  2005/10/19 05:30:14  etnlwind
// DEBUG Removed
//
// Revision 1.7  2005/10/18 12:40:51  akaiaphanu
// Add Image icons & working
//
// Revision 1.6  2005/10/18 07:32:02  akaiaphanu
// working Draw function
//
// Revision 1.5  2005/10/17 07:32:33  akaiaphanu
// update top-panel
//
// Revision 1.4  2005/10/17 06:24:38  akaiaphanu
// Add to update clock time & date
//
// Revision 1.3  2005/10/12 12:27:08  akaiaphanu
// Cleaning and ClockAction
//
// Revision 1.2  2005/10/12 05:40:54  akaiaphanu
// cleaning
//
// Revision 1.1  2005/10/12 02:22:59  akaiaphanu
// Add to Extra.cpp/h
//
//*****************************************************************************

#include "Extra.h"

EXTRA_CONFIG	_ExtraProperty;
extern CTextEngine_BDF* pT;

 void 
 Extra_MainScreen(void)
{
	pMpegDecoder->TAVI_InitSystemPalette();
	TAVI_LoadPaletteTransparencyData(TRUE);
	/* Extra Image */
	showbackground(9);
	op_ClearAreaOSD( 0, 0, 320, 240 );

	/* Bottom Panel */
	DrawBottomGuidePannel( SKIP_OSD_REFRESH, BGPID_HOME );
	DEBUGMSG(1,("BottomPanel Pass\n"));
	
	/* Top Panel */
	g_TopPanel->SetTitle( ML_EXTRA );
	g_TopPanel->SetLogo( CTopPanel::LOGO_ID_ETC);
	g_TopPanel->SetBatteryLevel( (CTopPanel::BatteryLevel)CalcBatteryLevel() );
	g_TopPanel->SetVisible( true );
	g_TopPanel->Show();
	DEBUGMSG(1,("TopPanel Pass\n"));

	return;

}

void
Extra_FinishScreen(void)
{
	ClearOsd();
	return;
}


unsigned char* 
Extra_GetImageElement( BITMAPFILEINFO256 bmp, int id, int count )
{
	unsigned char* p;
	int w = bmp.bitmapinfoheader.biWidth;
	int h = bmp.bitmapinfoheader.biHeight;
	int dh = h/count;

	p = bmp.image_buffer+w*dh*id;

	return p;
}

int 
Extra_CurrentFocusedCheck(int whatkey)
{
	/* Case Up Key */
	if(whatkey == -1)
	{
		if(_ExtraProperty.m_CurrentItem == EXTRA_ITEM_CLOCK)
		{
			_ExtraProperty.m_CurrentItem = EXTRA_ITEM_DIC;
		}
		else if(_ExtraProperty.m_CurrentItem == EXTRA_ITEM_ALARM)
		{
			_ExtraProperty.m_CurrentItem = EXTRA_ITEM_CLOCK;
		}	
		else
		{
			_ExtraProperty.m_CurrentItem = EXTRA_ITEM_ALARM;
		}
			
	}
	/* Case Down Key */
	 if(whatkey == 1)
	{
		if(_ExtraProperty.m_CurrentItem == EXTRA_ITEM_CLOCK)
		{
			_ExtraProperty.m_CurrentItem = EXTRA_ITEM_ALARM;
		}
		else if(_ExtraProperty.m_CurrentItem == EXTRA_ITEM_ALARM)
		{
			_ExtraProperty.m_CurrentItem = EXTRA_ITEM_DIC;
		}	
		else
		{
			_ExtraProperty.m_CurrentItem = EXTRA_ITEM_CLOCK;
		}
	}

	return 0;
}

int 
Extra_OnKeyPress( int key )
{
	CEvent evt;
	CListItem* puzItem;
	CCMListItem* pCMItem;
	CListItem* pListItem;
	int vkey;
	CNode< CWindow >* pNode;
	CComplexListView* pCMView;
	CWindow *pWnd;
	CWindow *pComponentWnd;

	vkey = MapToVirtualKey( key );
	evt.type = EVENT_KEY;
	evt.u.key.value = vkey;
	evt.u.key.pressed = KEY_PRESSED;
	_ExtraProperty.m_FocusedWindow->SendEvent( evt );

	switch( vkey )
	{
		case VK_LEFT:	DEBUGMSG(1,("Left KEY\n")); break;
		case VK_RIGHT:	DEBUGMSG(1,("Right KEY\n")); break;
		
		case VK_UP:		DEBUGMSG(1,("Up KEY pressed\n")); 
						Extra_CurrentFocusedCheck(-1);
						break;
						
		case VK_DOWN:	DEBUGMSG(1,("Down KEY pressed\n")); 
						Extra_CurrentFocusedCheck(1);
						break;

		case VK_ENTER:	DEBUGMSG(1,("Ok KEY pressed\n"));
						if(_ExtraProperty.m_CurrentItem == EXTRA_ITEM_CLOCK)
						{
							TAVI_CannotSleep(7);
							Extra_Clock_Task();
							TAVI_CanSleep(7);
						}		
						else if(_ExtraProperty.m_CurrentItem == EXTRA_ITEM_ALARM)
						{				
							TAVI_CannotSleep(7);
							AlarmSet_Task();
							TAVI_CanSleep(7);
						}
						else
						{
							TAVI_CannotSleep(7);
							ExDic_Task();
							TAVI_CanSleep(7);
						}
						break;

		// EQ
		case VK_F1:		DEBUGMSG(1,("EQ KEY\n")); break;

		// Menu
		case VK_F2:		DEBUGMSG(1,("Menu KEY\n")); break;

		// Mode Short
		case VK_F3:		DEBUGMSG(1,("Mode KEY pressed\n"));
						_ExtraProperty.m_ExitState = TaskExit_Background;
						_ExtraProperty.m_fExit = true;
						DEBUGMSG(1,("Out Extra_OnKeyPress() as Background\n"));
						return -1;
						break;
		// Exit Long & Short	
		case VK_EXT_ESC : DEBUGMSG(1,("Exit Long KEY pressed\n"));
		case VK_ESC:	DEBUGMSG(1,("Exit KEY pressed\n"));
						_ExtraProperty.m_ExitState = TaskExit_Normal;
						_ExtraProperty.m_fExit = true;
						DEBUGMSG(1,("Out Extra_OnKeyPress() as Normal\n"));
						return -1;
						break;
		// Mode Long				
		case TAVI_KEY_LMODE: DEBUGMSG(1,("Mode Long KEY pressed\n"));
						_ExtraProperty.m_ExitState = TaskExit_Switch;
						_ExtraProperty.m_fExit = true;
						DEBUGMSG(1,("Out Extra_OnKeyPress() as Switch\n"));
						return -1;
						break;
		// Exit Long		
	/*	case VK_EXT_ESC:	DEBUGMSG(1,("Exit Long KEY pressed\n"));
						_ExtraProperty.m_ExitState = TaskExit_ExitAndSwitch;
						_ExtraProperty.m_fExit = true;
						DEBUGMSG(1,("Out Extra_OnKeyPress() as ExitAndSwitch\n"));
						return -1;
						break;
	*/
		case TAVI_KEY_JOG1_UP:
						TAVI_BG_Volume( 1 );
						break;

		case TAVI_KEY_JOG1_DOWN:
						TAVI_BG_Volume( 0 );
						break;
						
		case TAVI_KEY_JOG2_UP:
						TAVI_BG_Next(  );
						break;
						
		case TAVI_KEY_JOG2_DOWN:
						TAVI_BG_Prev(  );
						break;

		case TAVI_KEY_JOG2_ENTER:
						TAVI_BG_PlayPause(  );
						break;

		default: DEBUGMSG(1,("Other KEY pressed\n"));
				break;
		
	}

	return 0;
			
}

void 
Extra_OnMouseEvent( const MouseEvent& evt )
{
	CEvent event;
	CCMListItem* pCMItem;
	CComplexListView* pCMView;
	CWindow* pWnd;
	CWindow *pComponentWnd;
	CNode< CWindow >* pNode;
	
	event.type = EVENT_MOUSE;
	event.u.mouse = evt;
	_ExtraProperty.m_FocusedWindow->SendEvent( event );
	
	if( evt.click == 1 )
	{
		CListItem* puzItem;
		puzItem = _ExtraProperty.m_puzListView->GetSelectedItem();
		switch( puzItem->m_nIdx )
		{
			case 0 : _ExtraProperty.m_CurrentItem = EXTRA_ITEM_CLOCK;
					TAVI_CannotSleep(7);
					Extra_Clock_Task();
					break;

			case 1 : _ExtraProperty.m_CurrentItem = EXTRA_ITEM_ALARM;
					TAVI_CannotSleep(7);
					AlarmSet_Task();
					break;

			case 2 : _ExtraProperty.m_CurrentItem = EXTRA_ITEM_DIC;
					TAVI_CannotSleep(7);
					ExDic_Task();
					break;

			default : break;
		}
	}
	else if(evt.x == -1)
	{
		Extra_CurrentFocusedCheck(-1);
	}
	else if(evt.x == 1)
	{
		Extra_CurrentFocusedCheck(1);
	}
	
	

}

int 
Extra_MsgProc( int ssTaskId, U32 ssMsgId, int usParam1, int usParam2 )
{
	MouseEvent mevt;
	KeyEvent key;
	
	DEBUGMSG(1,("5=In Extra_MsgProc()\n"));
	DEBUGMSG(1,("TaskId[%x], MsgId[%x], Param1[%x] \n", ssTaskId, ssMsgId, usParam1));
	
	switch( ssMsgId )
	{
		case MSG_CREATE:		break;	// 1
		case MSG_DRAW:			break;	// 4
		case MSG_TIMER:			break;
		case MSG_SETFOCUS : 	break;	// 10000
		case MSG_KILLFOCUS: 	break;	// 20000
		case MSG_DESTROY:		break;

		
		case MSG_BUTTON_DN: 			// 2000	
			DEBUGMSG(1,("button Down\n"));
			break;

								
		case MSG_BUTTON_UP: 			// 1000	
			DEBUGMSG(1,("button up\n")); 
			if (usParam1 == TAVI_KEY_SHUTDOWN)
			{
				DEBUGMSG(1,("Extra_MsgProc() : TAVI_KEY_SHUTDOWN\n" ));
				_ExtraProperty.m_ExitState = TaskExit_Normal;
				_ExtraProperty.m_fExit = true;
				return -1;
			}
			return Extra_OnKeyPress(usParam1);
			break;
						
		case MSG_BATTERY:		break;

		
		case MSG_TOUCHPAD:		DEBUGMSG(1,("touchpad\n"));	// 100000
								mevt.click = usParam2;
								mevt.x = usParam1;
								Extra_OnMouseEvent(mevt);
								return 0;
								break;
			
		default:
			return false;
			break;
	}

	DEBUGMSG(1,("6=Out Extra_MsgProc()\n"));
	
	pMpegDecoder->RefreshOsd ();					// refresh screen							

	return 0;
}


int 
Extra_Kill(void)
{
	if (_ExtraProperty.m_pMsgBox) 
		delete _ExtraProperty.m_pMsgBox;
	
	_ExtraProperty.m_puzListView->ClearList();
	_ExtraProperty.m_WndList.FreeAll();
	_ExtraProperty.m_fExit = true;
	_ExtraProperty.m_ExitState = TaskExit_Normal;
	_ExtraProperty.m_CurrentItem = EXTRA_ITEM_CLOCK;
	_ExtraProperty.m_SubClockState = 0;
	_ExtraProperty.m_SubAlarmState = 0;
	_ExtraProperty.m_SubDicState = 0;
	
	DEBUGMSG(1,("Extra_Task_Kill\n"));
	return 0;
}

static void 
Extra_CreateItems(void)
{
	CPixmap* puzPixmapIcon;
	CPixmap *pBarPixmap;
	CListItem* puzItem;
	
	/* Create ListView Control */
	_ExtraProperty.m_puzListView = new CListView( NULL );
	_ExtraProperty.m_puzListView->Resize( CRect(18, 33, 301, 206), false );

	/*Add Clock Item */
	puzItem = new CListItem( 0, 62, 63, ML_EXTRA_CLOCK);
	_ExtraProperty.m_puzListView->AddItem( puzItem );

	/* Add Alarm Item */
	puzItem = new CListItem( 1, 64, 65, ML_EXTRA_ALARM );
//	puzItem->m_nEnable = false;
	_ExtraProperty.m_puzListView->AddItem( puzItem );

	/* Add Dic Item */
	puzItem = new CListItem( 2, 24, 25, ML_DIC);
	_ExtraProperty.m_puzListView->AddItem( puzItem );
	
#if 0	
	/* Add Icons */
	for( int i=0; i<66; i++ ) 
	{
		puzPixmapIcon = new CPixmap;
		puzPixmapIcon->CreatePixmap( 12, 12, 8, 
			(char*)Extra_GetImageElement(TAVI_IMAGES[TYPE_ICON_LIST], i, 66),  false );
		_ExtraProperty.m_puzListView->AddPixmapIcon( puzPixmapIcon );
	}
	
	/* Set selectbar image */
	pBarPixmap = new CPixmap;
	pBarPixmap->CreatePixmap( 
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biWidth, 
		TAVI_IMAGES[LIST_SELECTBAR].bitmapinfoheader.biHeight,
		8,
		(char*)TAVI_IMAGES[LIST_SELECTBAR].image_buffer, false
		);
		
	_ExtraProperty.m_puzListView->SetSelectBarPixmap( pBarPixmap );
#endif

	_ExtraProperty.m_FocusedWindow = _ExtraProperty.m_puzListView;
	_ExtraProperty.m_WndList.AddHead( _ExtraProperty.m_FocusedWindow );
	_ExtraProperty.m_fExit = false;
	_ExtraProperty.m_OnConfirm = 0;

	/* Show */
	_ExtraProperty.m_puzListView->Show();
	
	return;
}

int 
Extra_Task( TTaskInitParam* tParam )
{
	TMsgEntry tMsg;
	int ret = 0;
	
	DEBUGMSG(1,("1=In Extra_Task()\n"));

	/* Key & TouchPad */
	SetTaviKeyMode(TK_REPEAT);
	mep_report( MEP_REPORT_RELATIVE);

	/* Set current task id */
	g_CurrentTaskID = tParam->ssTaskId;

	/* Set flag Exit State */
	_ExtraProperty.m_fExit = false;
	
 	/* Check Task resume for background */
	if( tParam->resume ) 
	{	
		/* case clock */
		if((_ExtraProperty.m_SubClockState == CLOCK_EXIT_BACKGROUND) 
			||(_ExtraProperty.m_SubClockState == CLOCK_EXIT_SWITCH) )
		{	
			Extra_Clock_Task();
			if(_ExtraProperty.m_SubClockState == CLOCK_EXIT_NORMAL 
				|| _ExtraProperty.m_SubClockState == 0 )
			{	
				_ExtraProperty.m_SubClockState = 0; 
				DEBUGMSG(1,("Clock Normal Exit\n"));
			}
			else
			{
				_ExtraProperty.m_ExitState = _ExtraProperty.m_SubClockState;
				goto __CheckState;
			}
		}
		/* case alarm */
		if(_ExtraProperty.m_SubAlarmState == ALARM_EXIT_BACKGROUND ||
			_ExtraProperty.m_SubAlarmState == ALARM_EXIT_SWITCH )
		{	
			AlarmSet_Task();
			if(_ExtraProperty.m_SubAlarmState == ALARM_EXIT_NORMAL )
			{	
				DEBUGMSG(1,("Clock Normal Exit\n"));
			}
			else
			{
				_ExtraProperty.m_ExitState = _ExtraProperty.m_SubAlarmState;
				goto __CheckState;
			}
		}
		/* case dic */
		if(_ExtraProperty.m_SubDicState == DIC_EXIT_BACKGROUND ||
			_ExtraProperty.m_SubDicState == DIC_EXIT_SWITCH )
		{	
			ExDic_Task();
			if(_ExtraProperty.m_SubDicState == DIC_EXIT_NORMAL )
			{	
				DEBUGMSG(1,("Dic Normal Exit\n"));
			}
			else
			{
				_ExtraProperty.m_ExitState = _ExtraProperty.m_SubDicState;
				goto __CheckState;
			}
		}
		
		Extra_MainScreen();
		_ExtraProperty.m_puzListView->Show();
		tParam->resume = 0;
		/* Start HDD Sleep on the extra main */
		TAVI_CanSleep(7);
	}
	else
	{ 
		Extra_MainScreen();
		Extra_CreateItems();
		/* Start HDD Sleep on the extra main */
		TAVI_CanSleep(7);
	}

	
	/* MSG loop */
	ClearKeyBuffer();
	while( _ExtraProperty.m_fExit == false ) 
	{
		if( MSG_Peek( tParam->ssTaskId, &tMsg ) == 0)
		{
			/* No Runnig Clock*/
			if(_ExtraProperty.m_SubClockState == 0) 
			{
				if(Extra_MsgProc( tMsg.ssToTaskId, tMsg.ulMsgId, tMsg.usParam1, tMsg.usParam2 ) == -1)
				{
					DEBUGMSG(1,("Break While loop\n"));
					DEBUGMSG(1,("_ExtraProperty.m_fExit = %d\n", _ExtraProperty.m_fExit));
					/* Extra Task Exit by normal mode */
					if( _ExtraProperty.m_ExitState == EXTRA_EXIT_NORMAL ) 
					{
						ret = TaskExit_Normal;
						tParam->resume = 0;
						Extra_Kill();
					}
					/* Extra Task Exit by background mode */
					if( _ExtraProperty.m_ExitState == EXTRA_EXIT_BACKGROUND )
					{
						ret = TaskExit_Background;
						tParam->resume = 1;
					}
					/* Extra Task Exit by switch mode */
					if(_ExtraProperty.m_ExitState == EXTRA_EXIT_SWITCH )
					{
						ret = TaskExit_Switch;
						tParam->resume = 1;
					}
					/* Extra Task Exit by ExitAndSwitch mode */
					if(_ExtraProperty.m_ExitState == EXTRA_EXIT_EXITSWITCH )
					{
						ret = TaskExit_ExitAndSwitch;
						tParam->resume = 0;
						Extra_Kill();
					}
					
					break;
				}
			}
			__CheckState :
			/* Clock & AlarmSet Exit by alarm mode */
			if(_ExtraProperty.m_SubClockState == CLOCK_EXIT_ALARM ||
				_ExtraProperty.m_SubAlarmState == ALARM_EXIT_ALARM ||
				_ExtraProperty.m_SubDicState == DIC_EXIT_ALARM )
			{
				Extra_Kill();
				ret = 1;
				break;
			}
			/* Clock & AlarmSet Exit by normal mode */
			if(_ExtraProperty.m_SubClockState == CLOCK_EXIT_NORMAL ||
				_ExtraProperty.m_SubAlarmState == ALARM_EXIT_NORMAL ||
				_ExtraProperty.m_SubDicState == DIC_EXIT_NORMAL)
			{
				_ExtraProperty.m_SubClockState = 0;
				_ExtraProperty.m_SubAlarmState = 0;
				_ExtraProperty.m_SubDicState = 0;
				
				if(1)
				{
					Extra_MainScreen();
					_ExtraProperty.m_puzListView->Show();
				}
			}
			/* Clock , AlarmSet and Dic Exit by background mode */
			if(_ExtraProperty.m_SubClockState == CLOCK_EXIT_BACKGROUND || 
				_ExtraProperty.m_SubAlarmState == ALARM_EXIT_BACKGROUND ||
				_ExtraProperty.m_SubDicState == DIC_EXIT_BACKGROUND )
			{
				_ExtraProperty.m_ExitState = TaskExit_Background;
				tParam->resume = 1;
				ret = 2;
				break;
			}
			/* Clock , AlarmSet and Dic Exit by switch mode */
			if(_ExtraProperty.m_SubClockState == CLOCK_EXIT_SWITCH || 
				_ExtraProperty.m_SubAlarmState == ALARM_EXIT_SWITCH ||
				_ExtraProperty.m_SubDicState == DIC_EXIT_SWITCH )
			{
				_ExtraProperty.m_ExitState = TaskExit_Switch;
				tParam->resume = 1;
				ret = 3;
				break;
			}
			/* Clock , AlarmSet and Dic Exit by ExitAndSwitch mode */
			if(_ExtraProperty.m_SubClockState == CLOCK_EXIT_EXITSWITCH ||
				_ExtraProperty.m_SubAlarmState == ALARM_EXIT_EXITSWITCH ||
				_ExtraProperty.m_SubDicState == DIC_EXIT_EXITSWITCH )
			{
				_ExtraProperty.m_ExitState = TaskExit_ExitAndSwitch;
				Extra_Kill();
				ret = 4;
				break;
			}
			
		}
		else
		{
			
			DEBUGMSG(1,("================================\n"));
			DEBUGMSG(1,("TaskName = [%s]\n",tParam->pTaskName));
			DEBUGMSG(1,("TaskID = [%d]\n",tParam->ssTaskId));
			DEBUGMSG(1,("Resume = [%d]\n",tParam->resume));
			DEBUGMSG(1,("ExitState = [%d]\n",_ExtraProperty.m_ExitState));
			DEBUGMSG(1,("fExit = [%d]\n",_ExtraProperty.m_fExit));
			DEBUGMSG(1,("================================\n"));
			
		}
		usleep(100000);	// 1,000,000 usec = 1sec
	}
	
	TAVI_CannotSleep(7);
	
	Extra_FinishScreen();
	
	return ret;

}

