//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Spin SelectBar class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/SpinSelectBar.h,v $

// $Revision: 1.1.1.1 $

// $Date: 2006/02/22 12:33:31 $

// $Author: zstein $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: SpinSelectBar.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2005/11/16 08:48:47  etnlwind
// Update for AlarmSet
//
// Revision 1.5  2005/10/17 11:26:05  yong
// nothing special :-(
//
// Revision 1.4  2005/05/18 16:02:39  ywkim
// add function of GetCount()
//
// Revision 1.3  2005/04/27 04:53:21  zstein
// Support Multi-languages.
// Support resource text.
//
// Revision 1.2  2005/03/01 02:11:05  zstein
// *** empty log message ***
//
// Revision 1.1  2005/02/25 01:13:09  zstein
// *** empty log message ***
//
//

//*******************************************************************************

#ifndef __SPIN_SELECTBAR_CLASS_H__
#define __SPIN_SELECTBAR_CLASS_H__

#include "window.h"
#include <Component/String.h>

typedef void( *PFN_SPINBAR )( int index, void* );
class CSpinSelectBar : public CWindow
{
private:
protected:
	CList< CString > m_uzList;
	CPixmap* m_puzLArrow;
	CPixmap* m_puzRArrow;
	int m_nSelectedIdx;
	int* m_pResIDs;
	int m_nCount;
	int m_nItemCount;
	bool m_fResID;
	PFN_SPINBAR m_pfn;
	void* m_pfnarg;
	uchar_t* m_TxtBuf;
	
	void OnFocus( bool fFocus );
	void OnKeypressed( const KeyEvent& key );
	void OnPaint( const CRect& rc );
public:
	CSpinSelectBar( CWindow* parent, bool fResID=0, int nCount=0 );
	~CSpinSelectBar( void );

	void Show( void );
	void Hide( void );
	void Resize( const CRect& rc, bool fUpdate );
	void Add( const CString& item );
	void SetSelectIndex( int idx ) { m_nSelectedIdx = idx; }
	int GetSelectedIndex( void ) { return m_nSelectedIdx; }
	void Add( int id );
	int GetCount(void);
	void Clear( void );
	void RegisterPFN( PFN_SPINBAR pfn, void* arg );
	void ReplaceUniqueText( const CString& item );
	void ReplaceUniqueText( int id );
};

#endif /* __SPIN_SELECTBAR_CLASS_H__ */
