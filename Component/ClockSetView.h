//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : ClockSetView class header
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ClockSetView.h,v $
// $Revision: 1.3 $
// $Date: 2006/06/12 04:46:48 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: ClockSetView.h,v $
// Revision 1.3  2006/06/12 04:46:48  ywkim
// Add member variable of m_changed for melon
//
// Revision 1.2  2006/03/14 11:16:42  ywkim
// Add argument of MAX_COUNT_TIMEZONES for arTimeZone[]
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/08/17 09:23:47  ywkim
// change routine for timezone
//
// Revision 1.4  2005/07/29 03:17:18  ywkim
// Change routine for timezone
//
// Revision 1.3  2005/05/04 06:15:32  ywkim
// Add verification fuc. of day
//
// Revision 1.2  2005/04/28 06:54:23  ywkim
// Add function of Readtime & Savetime
//
// Revision 1.1  2005/04/27 15:43:05  ywkim
// Initially build with m-language
//
//*****************************************************************************
#ifndef __ClockSetView_H__
#define __ClockSetView_H__

#include "window.h"
#include <Component/String.h>
#include <sys/time.h>
#include <time.h>

#define MAX_COUNT_TIMEZONES	39

typedef enum
{
	DAY_VIEW = 0,
	MONTH_VIEW,
	YEAR_VIEW,
	HOUR_VIEW,
	MINUTE_VIEW,
	SECOND_VIEW
} TYPE_OF_CLOCKVIEW_t;


class CClockSetView : public CWindow
{
private:
protected:
	CPixmap* m_puzLArrow;
	CPixmap* m_puzRArrow;
	
	TYPE_OF_CLOCKVIEW_t	m_TypeOfView;
	static struct tm m_time;
	static int m_is24hformat;
	static int m_dm;
	static int m_timezone;
	
	void OnFocus( bool fFocus );
	void OnKeypressed( const KeyEvent& key );
	void OnPaint( const CRect& rc );
public:
	static char m_szTimeZone[30];
	static int m_changed;

public:

	CClockSetView( CWindow* parent );
	~CClockSetView( void );

	void Show( void );
	void Hide( void );
	void Resize( const CRect& rc, bool fUpdate );
	void SetViewType( const TYPE_OF_CLOCKVIEW_t& ViewType );
	void SetSelectedView( int nValue );
	int GetSelectedView( void ); 
	char *SaveTime(void);
	void ReadTime(void);
	void SetTimeFormat(int is24hformat);
	void SetTimeZone(int nZone);
};
#endif /* __ClockSetView_H__ */
