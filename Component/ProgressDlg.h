//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Progress dialogue for showing the status of db update or etc.
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ProgressDlg.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ProgressDlg.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/12/26 04:44:25  zstein
// *** empty log message ***
//
// Revision 1.2  2005/09/14 02:30:46  zstein
// - update
//
// Revision 1.1  2005/05/15 11:10:25  zstein
// - [START] Progress dialogue
//
//*******************************************************************************

#ifndef __PROGRESS_DIALOG_H__
#define __PROGRESS_DIALOG_H__

#include "window.h"
#include "Label.h"
#include "ModalWnd.h"

class CProgressDlg : public CModalWnd
{
protected:
	CLabel* m_plbText;
	CLabel* m_plbUpdate;
	CLabel* m_plbCount;
public:
	CProgressDlg( CWindow* parent, const char* szTitle );
	~CProgressDlg( void );

	void Update( CString& title, int count );
};

#endif /* __PROGRESS_DIALOG_H__ */
