//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Recorder MAIN List View Class
// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RECMainListView.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: RECMainListView.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/11/10 01:09:16  zstein
// *** empty log message ***
//
// Revision 1.1  2005/07/16 03:09:37  zstein
// - start
//
//

//*******************************************************************************

#ifndef __RECORD_MAIN_LIST_VIEW_H__
#define __RECORD_MAIN_LIST_VIEW_H__

#include <Component/ListView.h>

class CRECMainListView : public CListView
{
private:
protected:
	void OnWindowEvent( const WindowEvent& evt );
	void OnKeypressed( const KeyEvent& evt );

	void MakeList( void );
public:
	CRECMainListView( CWindow* parent );
	~CRECMainListView( void );

	void Show( void );
};

#endif /* __RECORD_MAIN_LIST_VIEW_H__ */
