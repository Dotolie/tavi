//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Recorder MAIN Window Class
// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RECMainWnd.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: RECMainWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/07/16 03:09:37  zstein
// - start
//
//

//*******************************************************************************

#ifndef __RECORD_MAIN_WINDOW_H__
#define __RECORD_MAIN_WINDOW_H__

#include <Component/ListView.h>
#include "RECMainListView.h"

#define ON_VOICERECORD		ON_MESSAGE( 0x1000<<16 )
#define ON_LINEINRECORD	ON_MESSAGE( 0x2000<<16 )
#define ON_FOLDERVIEW		ON_MESSAGE( 0x3000<<16 )

class CRECMainWnd : public CWindow
{
private:
protected:
	CRECMainListView* m_pListView;
	CWindow* m_pRecWnd;
	CString m_szRootPath;

	void OnWindowEvent( const WindowEvent& evt );

	void OnVoiceRecord( void );
	void OnLineinRecord( void );
	void OnFolderView( void );
	void Show( void );
public:
	CRECMainWnd( CWindow* parent, const CString& szRoot );
	~CRECMainWnd( void );

	void SetFocus( void );
};

#endif /* __RECORD_MAIN_WINDOW_H__ */