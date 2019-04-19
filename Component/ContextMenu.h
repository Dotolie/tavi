//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : Context Menu
// 
// $Source: /home/cvsroot/TAVI020/tavi/Component/ContextMenu.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $ Lee Seok<zstein@newmedialife.com>
//
//******************************************************************************
// $Log: ContextMenu.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.2  2005/08/23 13:47:00  zstein
// - update & fixed bugs
//
// Revision 1.1  2005/07/16 04:43:30  zstein
// - created.
//
//******************************************************************************

#ifndef __CONTEXT_MENU_H__
#define __CONTEXT_MENU_H__

#include <Lib/LibSubMenu.h>
#include "window.h"

class CContextMenu : public CWindow
{
protected:
	void OnKeypressed( const KeyEvent& evt );
	void OnMouseEvent( const MouseEvent& evt );
	void OnWindowEvent( const WindowEvent& evt );
public:
	CPopUpMenu* m_pCtxMenu;
	
	CContextMenu( CWindow* parent );
	~CContextMenu( void );

	void Show( void );
	void Hide( void );
	void Reset( void );
};

#endif /* __CONTEXT_MENU_H__ */