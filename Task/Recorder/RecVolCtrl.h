//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Volume ProgressBar class definition

// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecVolCtrl.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: RecVolCtrl.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/07/16 03:09:37  zstein
// - start
//
//

//******************************************************************************
#ifndef __VOLUME_PROGRESS_BAR_H__
#define __VOLUME_PROGRESS_BAR_H__

#include <sys/time.h>
#include <Component/BottomWnd.h>
#include <Component/ProgressBar.h>
#include <Component/Label.h>

class CRecVolCtrl : public CBottomWnd
{
private:
protected:
	CProgressBar* m_pProgressBar;
	CLabel* m_plbVolume;
	CLabel* m_plbLevel;
	int m_nVolRange;
	int m_nVolume;
	struct timeval m_tv;

	void OnWindowEvent( const WindowEvent& evt );
	void OnMouseEvent( const MouseEvent& evt );
	void OnKeypressed( const KeyEvent& evt );
public:
	CRecVolCtrl( CWindow* parent );
	~CRecVolCtrl( void );

	void Show( void );
};

#endif /* __VOLUME_PROGRESS_BAR_H__ */
