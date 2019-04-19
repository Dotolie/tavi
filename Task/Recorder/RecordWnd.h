//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Recorder Window Class
// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecordWnd.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: RecordWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.7  2005/12/16 10:13:35  zstein
// *** empty log message ***
//
// Revision 1.6  2005/11/22 09:07:10  zstein
// *** empty log message ***
//
// Revision 1.5  2005/11/17 11:20:22  zstein
// *** empty log message ***
//
// Revision 1.4  2005/10/17 01:27:24  zstein
// - Updated UI.
//
// Revision 1.3  2005/10/07 07:10:03  zstein
// - fixed bugs and update
//
// Revision 1.2  2005/09/21 09:21:04  zstein
// - updated ui.
//
// Revision 1.1  2005/07/16 03:09:37  zstein
// - start
//
//

//*******************************************************************************

#ifndef __RECORDER_WINDOW_H__
#define __RECORDER_WINDOW_H__

#include <sys/time.h>
#include <unistd.h>
#include <Component/window.h>
#include <Component/Pixmap.h>
#include <Component/String.h>
#include <Component/Label.h>
#include <Component/ModalWnd.h>
#include <Component/ContextMenu.h>
#include "RecSetupWnd.h"
#include "RecordBottomWnd.h"
#include "RecVolCtrl.h"

class CRecorderWnd : public CWindow
{
public:
	typedef enum {
		RECORD_FM,
		RECORD_LINEIN,
		RECORD_VOICE
	} RecordType;
private:
protected:
	CString m_szSavePath;
	CString m_szSaveFilename;
	CPixmap* m_pmRecord;
	CLabel* m_plbSavingPath;
	CLabel* m_plbInfo;
	CLabel* m_plbFree;
	RecordType m_Type;
	CMsgBox* m_pMsgBox;
	bool m_fValid;
	int m_nCurrentTime;
	int m_nTotalHDSize;
	struct timeval m_tv;
	int m_Hz;
	bool m_fRecord;

	CContextMenu* m_pCtxMenuWnd;
	
	
	void OnKeypressed( const KeyEvent& evt );
	void OnMouseEvent( const MouseEvent& evt );
	void OnWindowEvent( const WindowEvent& evt );
	void OnPaint( const CRect& rc );
	void OnSetup( void );
	void Show( void );

	void Record( void );
	void Pause( void );

	void CreateSubMenu( void );
	void DrawBigFontTime( void );
public:
	CRecordBottomWnd* m_pBottomWnd;
	CRecVolCtrl* m_pVolCtrl;
	bool m_fRecording;
	
	CRecorderWnd( CWindow* parent, const CString& szSavePath, RecordType type, int hz=0 );
	~CRecorderWnd( void );

	bool IsValid( void ) { return m_fValid; }

	void AGCOn( void );
	void AGCOff( void );

	void SwitchBottomPanel( CWindow* pWnd );
	void SetBitrate( int bitrate );
};

#endif /* __RECORDER_WINDOW_H__ */
