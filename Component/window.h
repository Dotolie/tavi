/*
 * $Id: window.h,v 1.2 2006/06/07 08:27:08 zstein Exp $ - CWindow.h
 * 
 * Copyright(C) 2004 New Media Life Inc.
 * 
 * CWindow: main object for GUI
 *
 * Author: Lee Seok<zstein@newmeidalife.com>
 * Date: 12/11/2004
 * Update:
 * History:
 */

#ifndef __CWINDOW_H__
#define __CWINDOW_H__

#include <stdio.h>
#include "object.h"
#include "event.h"
#include "fbdev.h"
#include "timer.h"
#include "String.h"

// Macros
#define ON_MESSAGE( _x )		( WINDOW_USER_MESSAGE+_x )

extern int g_CurrentTaskID;
typedef enum {
	WINDOW_TYPE_NORMAL=0xAA500000,
	WINDOW_TYPE_LISTVIEW,
	WINDOW_TYPE_LABEL,
	WINDOW_TYPE_SCROLL,
	WINDOW_TYPE_VSCROLL,
	WINDOW_TYPE_PROGRESSBAR,
	WINDOW_TYPE_RADIOBUTTON,
	WINDOW_TYPE_SPINBAR,
	WINDOW_TYPE_SLIDEBAR,
	WINDOW_TYPE_CLOCKVIEW,
	WINDOW_TYPE_USER
} WindowType;

class CWindow;
extern CList< CWindow > g_WndList;
extern CWindow* g_pFocusedWnd;
extern CWindow* g_pRootWnd;

class CWindow : public CObject {
private:
	void Init( CWindow* parent, const CRect& rc );
protected:
	int m_nTaskID;
	int m_nWindowID;
	int m_nTimerID;
	WindowType m_nWindowType;
	CString m_szTitle;
public:
	int m_nBorderWidth;
	int m_nBorderColor;;
	int m_nBackgroundColor;
	int m_nFocusedColor;
	int m_mask;
	bool m_fFocus;
	bool m_fVisible;
  bool m_disable;
	CWindow* m_parent;
	CRect rcABS;
	CRect rcClient;
	CList< CWindow > m_uzChildList;
	CList< CWindow >* m_pWndList;
public:
	CWindow( CWindow* parent, const CRect& rc, int borderw, int borderc, int background, int mask );
	CWindow( CWindow* parent );
	CWindow();
	virtual ~CWindow( void );
	// Access Methods
	CPoint GetABSLeftTop() { return CPoint( rcABS.x1, rcABS.y1 ); }
	CPoint GetABSRightBottom() { return CPoint( rcABS.x2, rcABS.y2 ); }
	void SetCoordinate( int x, int y, int w, int h );
	void AddChild( CWindow* child ) { 
		child->m_parent = this;
		m_uzChildList.Add( child ); 
	}
	void SetBackground( int color ) { m_nBackgroundColor = color; }
	bool IsShow( void ) { return m_fVisible; }
	int GetWindowID( void ) { return m_nWindowID; }
	int GetTaskID( void ) { return m_nTaskID; }
	int  SetTimer( int msec, CTimer::TimerType type=CTimer::TIMER_REPEAT );
	void KillTimer( int nID );
	bool GetVisible( void );
	void SetVisible( bool fVisible );
  void SetDisable( bool disable );
	void SetBorderColor( int nBorderColor ) { m_nBorderColor = nBorderColor; }
	void SetBorderWidth( int nBorderWidth ) { m_nBorderWidth = nBorderWidth; }
	void SetFocusedColor( int nFocusedColor ) { m_nFocusedColor = nFocusedColor; }
	CRect GetClientRect( void ) { return rcClient; }
	void SetFocus( bool fFocus ) { OnFocus( fFocus); }
	virtual void SetFocus( void );
	void SetWindowList( CList<CWindow>* pList ) { m_pWndList = pList; }
	static CWindow* GetFocusedWindow( void ) { return g_pFocusedWnd; }
	void Flush( void ) { op_Flush(); }
	WindowType GetWindowType( void ) const { return m_nWindowType; }
	CWindow* GetParent( void ) const { return m_parent; }
	static CWindow* GetRootWindow( void ) { return g_pRootWnd; }
	void RegisterWndList( void );
	void DeregisterWndList( void );
	const CString& GetWindowTitle( void ) const { return m_szTitle; }
	void SetWindowTitle( const CString& szTitle ) { m_szTitle = szTitle; }
	void SendMessage( int id, int to=-1 );
		
	virtual void SendEvent( const CEvent& evt, CWindow* wnd=NULL );
	virtual void Show( void );
	virtual void Hide( void );
	virtual void Update( void ) { OnPaint( rcClient); }
	virtual void Update( int TASK_ID ) { OnPaint( rcClient); }
	virtual void Resize( const CRect& rc, bool fUpdate );
	// Draw methods
protected:
	virtual void OnPaint( const CRect& rc );
	virtual void OnKeypressed( const KeyEvent& key ) {}
	virtual void OnKeyReleased( const KeyEvent& key ) {}
	virtual void OnMouseEvent( const MouseEvent& evt ) {}
	virtual void OnWindowEvent( const WindowEvent& evt ) {}
	virtual void OnTimerEvent( const TimerEvent& evt );
	virtual void OnEvent( const Event& event ) {}
	virtual void OnFocus( bool fFocus ) { 
		m_fFocus = fFocus;
		OnPaint( rcClient); 
	}
};


#endif /* __CWINDOW_H__ */
