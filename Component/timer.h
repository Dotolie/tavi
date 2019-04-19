/*
 * $Id: timer.h,v 1.1.1.1 2006/02/22 12:33:31 zstein Exp $ - timer.h
 * 
 * Copyright(C) 2004 New Media Life Inc.
 * 
 * Timer functions
 *
 * Author: Lee Seok<zstein@newmedialife.com>
 * Date: 12/19/2004
 * Update:
 * History:
 */

#ifndef __TIMER_H__
#define __TIMER_H__

// New Timer functions & class
#include <stdio.h>

#define MAX_TIMERS		10

class CTimer;
extern int InitTimer( void );
extern int RegisterTimer( CTimer* t );
extern int DeregisterTimer( int& nID );

class CTimer
{
public:
	int m_nTimerID;
	int m_nTaskID;
	int m_nWindowID;
	typedef enum {
		TIMER_ONESHOT,
		TIMER_REPEAT
	} TimerType;
	TimerType m_nType;
	int m_nExpireTime;	// msec resolution
	long m_lExpireSec;
	long m_lExpireMSec;
public:
	CTimer( void ) {}
	~CTimer( void ) { }
};

#endif /* __TIMER_H__ */
