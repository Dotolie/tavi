//************************ Copyright(C) New Media Life Inc. ************************

//

// Description : Complex Label Class

// 

// $Source: /home/cvsroot/TAVI020/tavi/Component/ComplexLabel.h,v $

// $Revision: 1.3 $

// $Date: 2006/06/12 04:47:46 $

// $Author: ywkim $ Lee Seok<zstein@newmedialife.com>

//

//******************************************************************************

// $Log: ComplexLabel.h,v $
// Revision 1.3  2006/06/12 04:47:46  ywkim
// Change variable scope of m_pChild
//
// Revision 1.2  2006/06/07 08:19:09  zstein
// - added GetLabel() method.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/02/25 13:57:17  zstein
// Added WindowType
//
// Revision 1.2  2005/02/25 06:11:15  zstein
// Fixed UI bug
//
// Revision 1.1  2005/02/25 01:12:18  zstein
// Complex Label & View
//
//

//******************************************************************************

#ifndef __COMPLEX_LABEL_CLASS_H__
#define __COMPLEX_LABEL_CLASS_H__

#include "window.h"
#include "Label.h"

class CComplexLabel : public CWindow
{
private:
protected:
	CLabel* m_plbTitle;
	
	void OnFocus( bool fFocus );
	void OnKeypressed( const KeyEvent& evt );
	void OnPaint( const CRect& rc ) {}

public:	
	CWindow* m_pChild;
public:
	CComplexLabel( CWindow* parent );
	~CComplexLabel( void );

	void SetLabel( CLabel* pLabel );
	void AddComponent( CWindow* pComponent );
	CWindow* GetComponent( void ) { return m_pChild; }
  CLabel* GetLabel( void ) { return m_plbTitle; }
};

#endif /* __COMPLEX_LABEL_CLASS_H__ */
