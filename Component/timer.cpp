#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include "timer.h"

#include "../Task/Msg.h"
#include "object.h"

#define TIMER_LOCK		( lock = 1 )
#define TIMER_UNLOCK	( lock = 0 )

static CList< CTimer > TimerQueue;
static int RegisteredTimerIDList[ MAX_TIMERS ];
static CTimer* RegisteredTimerList[ MAX_TIMERS ];
static int lock;
extern CMiscCtrl				*g_MiscCtrl;

static void MainTimerHandler( int signo )
{
	CNode< CTimer >* pNode;
	int i;
	int count;
	CTimer* pTimer;
	struct timeval tv;
	
	gettimeofday( &tv, NULL );
	count = TimerQueue.Count();

	for( i=0; i<count; i++ ) {
		TIMER_LOCK;
		pTimer = TimerQueue[0];
		if( !pTimer ) {
			printf( "no timer: %d \n", TimerQueue.Count() );
			TIMER_UNLOCK;
			return;
		}
		if( tv.tv_sec < pTimer->m_lExpireSec ) {
			TIMER_UNLOCK;
			return;
		}
		if( tv.tv_usec/1000 < pTimer->m_lExpireMSec ) {
			TIMER_UNLOCK;
			return;
		}
		printf( "timer handle.... count: %d, id: %d, window id: 0x%04lx, task id: %d, type: %d\n", 
			TimerQueue.Count(), pTimer->m_nTimerID, pTimer->m_nWindowID, pTimer->m_nTaskID, pTimer->m_nType );
		// expire
		pNode = TimerQueue.DetachHead();
		// Send Timer message
		TMsgEntry msg;
		msg.ssToTaskId = pTimer->m_nTaskID;
		msg.ulMsgId = MSG_TIMER;
		msg.usParam1 = pTimer->m_nWindowID;
		msg.usParam2 = pTimer->m_nTimerID;
		MSG_Send( msg.ssToTaskId, &msg );

		if( pTimer->m_nType != CTimer::TIMER_ONESHOT ) {
			pTimer->m_lExpireMSec = tv.tv_usec/1000 + pTimer->m_nExpireTime%1000;
			pTimer->m_lExpireSec = tv.tv_sec + pTimer->m_nExpireTime/1000;
			TimerQueue.AddNodeToTail( pNode );
		}
		else { 
			RegisteredTimerIDList[pTimer->m_nTimerID] = 0;
			RegisteredTimerList[pTimer->m_nTimerID] = NULL;
			delete pNode;
		}
		TIMER_UNLOCK;
	}
 }

int InitTimer( void )
{
	struct sigaction sa;
	struct itimerval tv;
	int i;

	// Clear registered timer id list
	for( i=0; i<MAX_TIMERS; i++ ) {
		RegisteredTimerIDList[i] = 0;
		RegisteredTimerList[i] = NULL;
	}

	sa.sa_handler = MainTimerHandler;
	sa.sa_flags = 0;
	sigemptyset( &sa.sa_mask );

	if( sigaction( SIGALRM, &sa, NULL ) < 0 ) return -1;

	
	tv.it_interval.tv_sec = 0;
	tv.it_interval.tv_usec = 100000;	// 100 msec
	tv.it_value.tv_sec = 1;
	tv.it_value.tv_usec = 0;
	setitimer( ITIMER_REAL, &tv, NULL );

	lock = 0;

	return 0;
}

int RegisterTimer( CTimer* t )
{
	int i;
	struct timeval tv;

	while( lock );
	
	if( TimerQueue.Count() == MAX_TIMERS ) {
		printf( "failed to register timer: RegisteredCount: %d, Maximum: %d\n", TimerQueue.Count(), MAX_TIMERS );
		return -1;
	}
	
	for( i=0; i<MAX_TIMERS; i++ ) {
		if( RegisteredTimerIDList[i] == 0 ) {
			RegisteredTimerIDList[i] = 1;
			RegisteredTimerList[i] = t;
			break;
		}
	}
	// Ok register
	if( i < MAX_TIMERS ) {
		gettimeofday( &tv, NULL );
		t->m_nTimerID = i;
		t->m_lExpireMSec = tv.tv_usec/1000 + t->m_nExpireTime%1000;
		t->m_lExpireSec = tv.tv_sec + t->m_nExpireTime/1000;
		TimerQueue.AddTail( t );
	}

	//printf( "ok total: %d, register timer %d, current time: %d:%d, expired time: %d:%d\n", 
	//	TimerQueue.Count(), t->m_nTimerID, tv.tv_sec, tv.tv_usec/1000, t->m_lExpireSec, t->m_lExpireMSec );
	
	return i;
}

int DeregisterTimer( int& nID )
{
	int ret=0;
	int dID = nID;

	while( lock );
	nID = -1;
	//printf( "deregisterTimer...\n" );
	if( dID < 0 ) {
		printf( "Invalid ID, too small\n" );
		return -1;
	}
	if( dID >= MAX_TIMERS ) {
		printf( "Invalid ID, too large\n" );
		return -1;
	}

	if( RegisteredTimerIDList[dID] == 0 ) {
		printf( "( nID =%d )is deleted item\n", dID );
		return -1; // delete id;
	}
	CTimer* t = RegisteredTimerList[dID];
	CNode< CTimer >* pNode = TimerQueue.Search( t );
	if( !pNode ) {
		printf( "No such ID\n" );
		return -1;
	}
	TimerQueue.DeleteNode( pNode );
	RegisteredTimerIDList[dID] = 0;
	RegisteredTimerList[dID] = NULL;
	printf( "OK Deleted.................. ID: %d,   count: %d\n", dID, TimerQueue.Count() );

	return 0;
}
