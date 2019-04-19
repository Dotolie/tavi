//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Recorder Setup Window Class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/RecSetupWnd.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: RecSetupWnd.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/07/16 03:09:37  zstein
// - start
//
//

//******************************************************************************

#ifndef __RECORDER_SETUP_WINDOW_H__
#define __RECORDER_SETUP_WINDOW_H__ 

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"

class CRecSetupWnd : public CModalWnd
{
private:
protected:
	CComplexListView* m_pCMListView;
	
	void OnKeypressed( const KeyEvent& evt );
	void OnMouseEvent( const MouseEvent& evt );
public:
	CRecSetupWnd( CWindow* parent );
	~CRecSetupWnd( void );

	void CreateSetup( void );
	int GetBitrate( void );
	int GetInputVol( void );
	int GetSamplingrate( void );
	bool GetAutoGain( void );
	void Resize( const CRect& rc, bool fUpdate );
};

#endif /* __RECORDER_SETUP_WINDOW_H__ */
