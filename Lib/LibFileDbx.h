//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : CLibFileDbx Class Header File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibFileDbx.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibFileDbx.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.4  2005/12/22 05:12:21  etnlwind
// Update for New Version
//
// Revision 1.3  2005/07/14 07:38:47  etnlwind
// Update for Video Player
//
// Revision 1.2  2005/07/13 12:57:32  etnlwind
// Update for DEBUGMSG Related Functions
//
// Revision 1.1  2005/07/12 12:46:27  etnlwind
// Update for LibFileDbx
//
//
//*****************************************************************************


 #ifndef __LIB_FILE_DIALOG_BOX_H__
 #define __LIB_FILE_DIALOG_BOX_H__

#include <stdio.h>
#include <stdlib.h>
#include <rmbasic.h>
#include <linuxfs.h>
#include <jpeg/jpeg.h>

#include <tavi_lib.h>
#include <tavi_structures.h>
#include <bitmap.h>
#include <ImageFont.h>
#include <ImageResource.h>
#include <meplib.h>


#include "../Component/object.h"
#include "../Component/Stack.h"
#include "../Component/event.h"
#include "../Component/fbdev.h"
#include "../Component/image.h"
#include "../Component/timer.h"

#include <Lib/LibImage.h>

#include <string.h>
#include <Component/object.h>
#include <Component/image.h>

#include <Lib/LibImage.h>
#include <Lib/LibScrollBar.h>
#include <Lib/LibDialogBox.h>
#include <Lib/LibBookmarkDialogBox.h>
#include <Lib/LibSetupDialogBox.h>
#include <Lib/LibControlPannel.h>
#include <Lib/LibListBox.h>

#include <fatfs.h>


#define __ATTRIBUTE_PACKED__	__attribute__((packed))

typedef struct _tag_FILEDBX_NODE
{
	int	iCurrentItemIndex;
	int 	iMaxItemIndex;
	FATFS_DIR_ENTRY_INFO D;
	eFILE_TYPE 	filetype;
	char			ext[32];

} __ATTRIBUTE_PACKED__ FILEDBX_NODE;


///////////////////////////////////////////////////////////////////////
//
// CLibFileDbx Class
// --------------------------

class CFileDbx : public CDialogBox
{
	CList<FILEDBX_NODE> 	ItemList;

	char* m_ext;
//	U32 m_option;

	int ScanDir( uchar_t targetdir[] );
	void CreateSelectBar();
	void CreateTextData();
	int isVisible;
	
public:

	CFileDbx( U32 DbxTitleRes, int x, int y, int w, int h, uchar_t* TargetDir, char* ext, U32 option );
	virtual ~CFileDbx();

	int AddFileItem_UC16_WithSorting( int idx, uchar_t name[], int filesize );

	void Show();
	void Redraw();

	int IsVisible() { return isVisible; }
	uchar_t* GetFilenamePtr();
	
	
	int MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );	

};










#endif


