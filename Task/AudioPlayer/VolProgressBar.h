//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Volume ProgressBar class definition

// 

// $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/VolProgressBar.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: VolProgressBar.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.11  2005/11/01 10:32:59  zstein
// *** empty log message ***
//
// Revision 1.10  2005/08/29 08:05:32  zstein
// - updated
//
// Revision 1.9  2005/08/17 02:05:36  zstein
// - updated UI
//
// Revision 1.8  2005/06/16 23:51:43  zstein
// - bug fixed.
//
// Revision 1.7  2005/06/15 01:25:25  zstein
// - changed touch I/F.
//
// Revision 1.6  2005/06/02 05:51:58  zstein
// - bug fixed
//
// Revision 1.5  2005/05/12 04:48:26  zstein
// *** empty log message ***
//
// Revision 1.4  2005/03/07 00:50:37  zstein
// Cebit Version
//
// Revision 1.3  2005/03/01 01:53:46  zstein
// *** empty log message ***
//
// Revision 1.2  2005/02/21 02:11:39  zstein
// BUG fixed & update
//
// Revision 1.1  2005/02/16 16:17:37  zstein
// *** empty log message ***
//

//******************************************************************************
#ifndef __VOLUME_PROGRESS_BAR_H__
#define __VOLUME_PROGRESS_BAR_H__

#include <Component/ProgressBar.h>
#include <Component/Label.h>

class CVolProgressBar : public CWindow
{
private:
protected:
	CPixmap* m_pVolMinus;
	CPixmap* m_pVolPlus;
	int m_nVolLevel;
	int m_nVolRange;
 	CProgressBar* m_pProgressBar;
	CLabel* m_plbVolume;
	CLabel* m_plbLevel;
	bool m_fVolumeStart;
	int m_nDir;
	
	void OnPaint( const CRect& rc );
	void OnMouseEvent( const MouseEvent& evt );
	void OnKeypressed( const KeyEvent& evt );

	void DrawExtLCDVol( int vol );
public:
	CVolProgressBar( CWindow* parent );
	~CVolProgressBar( void );

	void Show( void );
	void Hide( void );
	void StartVolCtrl( int dir ) { 
		m_fVolumeStart = true; 
		m_nDir = dir;
	}
	void SetVolume( int vol );
	int GetVolume( void ) { return m_nVolLevel; }
	int GetVolumeRange( void ) { return m_nVolRange; }
};

#endif /* __VOLUME_PROGRESS_BAR_H__ */