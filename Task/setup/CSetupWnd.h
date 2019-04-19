//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Setup Base Window Class
// 
// $Source: /home/cvsroot/TAVI020/tavi/Task/setup/CSetupWnd.h,v $
// $Revision: 1.2 $
// $Date: 2006/06/12 04:25:08 $
// $Author: ywkim $
//
//*****************************************************************************
// $Log: CSetupWnd.h,v $
// Revision 1.2  2006/06/12 04:25:08  ywkim
// Add instruction of ClearList() in ~CSetupWnd()
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.5  2005/11/24 09:12:59  ywkim
// Change some items of setup related sound EQ
// Add new classes of CSetupUserEQ & CSetupUserEffect
//
// Revision 1.4  2005/05/03 11:53:20  ywkim
// Some change with m-language
//
// Revision 1.3  2005/04/28 06:53:17  ywkim
// Change clocksetup
//
// Revision 1.2  2005/04/27 15:41:34  ywkim
// Updated for m-language
//
// Revision 1.1  2005/04/27 05:30:28  ywkim
// Modify show or onPaint for m-language
//
//
//*****************************************************************************

#ifndef __SETUP_BASE_WINDOW_H__
#define __SETUP_BASE_WINDOW_H__

#include <Component/ComplexListView.h>
#include <Component/ModalWnd.h>
#include "../tavi_global.h"

typedef void (*fDoActionProc)( int nItem, int nComponent  );

class CSetupWnd : public CModalWnd
{
private:
protected:
	CComplexListView* m_pCMListView;
	fDoActionProc m_fAction;

	void OnMouseEvent( const MouseEvent& evt );
	
public:
	CSetupWnd( CWindow* parent ) : CModalWnd( parent ) {};
	~CSetupWnd( void ) {ClearList();};
	void ClearList( void );

	virtual void CreateSetup( void ) = 0;
	virtual void SetActionProc( fDoActionProc fProc);
	virtual void DoActionProc(int nItem, int nComponent );
	virtual CCMListItem* GetSelectedItem( void );
	int GetCount(void) { return m_pCMListView->GetCount(); }
	CListItem* GetItem( int nIdx ) { return m_pCMListView->GetItem( nIdx); }
	virtual void Resize( const CRect& rc, bool fUpdate );

};

#endif /* __SETUP_BASE_WINDOW_H__ */

