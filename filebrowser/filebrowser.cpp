#include "filebrowser.h"


#include <errno.h>
#include <dirent.h>

#if 1
#ifdef _WIN32
#include <windows.h>
static void debug_outs (char *szMsg, ...)
{
	char szBuf[256];
	wvsprintf (szBuf, szMsg, (LPSTR)(&szMsg + 1));
	OutputDebugString (szBuf);
}
#else
#include <stdio.h>
#define debug_outs	printf
#endif
static void debug_break (void)
{
}
#define ASSERT(exp)				((void)((exp)?1:(debug_outs ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(debug_outs printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

#define FILE_BROWSER_STATE_INIT			0
#define FILE_BROWSER_STATE_BROWSING		1
#define FILE_BROWSER_STATE_WAITING		2
#define FILE_BROWSER_IDLE				3

//extern TTAVI_GLOBAL_CONFIG* pTAVI;
extern BITMAPFILEINFO256 		TAVI_IMAGES[];

FileBrowser::FileBrowser ()
{
	m_handle = 0;
	m_ScreenWidth = 0;
	m_ScreenHeight = 0;
	m_TextHeight = 0;
	m_IconWidth = 32;
	m_IconHeight = 24;
	m_HorizontalOverscan = 0;
	m_VerticalOverscan = 0;
	m_RenderPending = 0;
	m_CurrentIndex = 0;
	m_PlayDirectory = 0;
	m_filenameoffset = 0;
	m_State = FILE_BROWSER_STATE_INIT;
}

FileBrowser::~FileBrowser ()
{
	if (m_handle)
	{
		ASSERT (m_CallbackTable.closedir);
		m_CallbackTable.closedir (m_handle, m_CallbackTable.context);
		m_handle = 0;
	}
}

void FileBrowser::ReuseFileBrowser ()
{
	m_handle = 0;
	m_ScreenWidth = 0;
	m_ScreenHeight = 0;
	m_TextHeight = 0;
	m_IconWidth = 32;
	m_IconHeight = 24;
	m_HorizontalOverscan = 0;
	m_VerticalOverscan = 0;
	m_RenderPending = 0;
	m_CurrentIndex = 0;
	m_PlayDirectory = 0;
	m_filenameoffset = 0;
	m_State = FILE_BROWSER_STATE_INIT;
}


void FileBrowser::ReCloseFileBrowser ()
{
	if (m_handle)
	{
		ASSERT (m_CallbackTable.closedir);
		m_CallbackTable.closedir (m_handle, m_CallbackTable.context);
		m_handle = 0;
	}
}


int FileBrowser::GetParentDir_InCurrentFullDirString( char source[] )
{
	int 	szSrcDir;
	int	NewDirStrLength;
	char 	target[1024];

	szSrcDir = strlen( source );

	DEBUGMSG(1, ("FileBrowser::GetParentDir_InCurrentFullDirString()  (1)...  m_rootdir:[%s] source:[%s]\n", m_rootdir, source ));

	for( NewDirStrLength=szSrcDir;NewDirStrLength>=0; NewDirStrLength--)
	{
		if( source[NewDirStrLength] == '/' )
		{
			strncpy( target, source, NewDirStrLength );
			target[NewDirStrLength] = '\0';
			break;
		}
	}

	DEBUGMSG(1, ("FileBrowser::GetParentDir_InCurrentFullDirString()  (2)...  m_rootdir:[%s] source:[%s] target:[%s] NewDirStrLength:[%d]\n", m_rootdir, source, target, NewDirStrLength ));

	if( strncmp( target, m_rootdir, strlen(m_rootdir) ) < 0 )
		{
		return (-1);
		}
	else
		{
		// CD success
		strcpy( source, target );

		DEBUGMSG(1, ("FileBrowser::GetParentDir_InCurrentFullDirString()  (3)...  m_rootdir:[%s] source:[%s] target:[%s] NewDirStrLength:[%d]\n", m_rootdir, source, target, NewDirStrLength ));

		return NewDirStrLength;
		}
}

/*
int FileBrowser::GetParentDir_InCurrentFullDirString( char target[],  char source[] )
{
    int szSrcDir;
    target[0] = '\0';

    szSrcDir = strlen( source );

    for( int i=szSrcDir; i>=0; i--)
    {
        if( source[i] == '/' )
        {
		strncpy( target, source, i );
		strcpy( source, target );	
            return(i);
            break;
        }
    }

    return 0;
}
*/

int FileBrowser::ChangeToParentDir()
{

	DEBUGMSG(1,("FileBrowser::ChangeToParentDir()\n"));
	
	ASSERT (m_handle);
	ASSERT (m_CallbackTable.closedir);

	DEBUGMSG(1,(" -close current directory\n"));

	m_CallbackTable.closedir (m_handle, m_CallbackTable.context);


	DEBUGMSG(1,(" -GetParentDir_InCurrentFullDirString(m_currentdir=[%s])\n", m_currentdir));


	if( GetParentDir_InCurrentFullDirString(  m_currentdir ) > 0 )
	{
		m_handle = m_CallbackTable.opendir (m_currentdir, m_CallbackTable.context);


		DEBUGMSG(1,(" -open new directory: [%s]\n", m_currentdir));


		ASSERT (m_handle);
		ASSERT (m_CallbackTable.info);
		m_CallbackTable.info (FILEBROWSER_INFO_MSG_CURRENT_DIRECTORY, m_currentdir, m_CallbackTable.context);

		m_AbsoluteCursor	= 0;
		m_TotalItemCount	= 0;
		m_RelativeCursor	= 0;
		m_DisplayStartItem	= 0;

		RefreshList (0);
		RepaintListV2 (0);
		return 0;
	}
	else
	{
		m_CallbackTable.closedir (m_handle, m_CallbackTable.context);
		return -1;
	}

}


/*
	if (m_DirectoryEntries[m_AbsoluteCursor].flags & DIR_ENTRY_INFO_IS_DIRECTORY)
	{
		char sdir[1024];

DEBUGMSG(1,("FileBrowser::Select () : Directory [%s] selected\n", m_DirectoryEntries[m_AbsoluteCursor].name) );

		ASSERT (m_handle);
		ASSERT (m_CallbackTable.closedir);
		printf("close current directory\n");
		m_CallbackTable.closedir (m_handle, m_CallbackTable.context);

		sprintf(m_currentdir, "%s/%s", m_currentdir, m_DirectoryEntries[m_AbsoluteCursor].name );
		printf("open new directory: [%s]\n", m_currentdir);
		m_handle = m_CallbackTable.opendir (m_currentdir, m_CallbackTable.context);
		ASSERT (m_handle);
		ASSERT (m_CallbackTable.info);
		m_CallbackTable.info (FILEBROWSER_INFO_MSG_CURRENT_DIRECTORY, m_currentdir, m_CallbackTable.context);

		m_AbsoluteCursor	= 0;
		m_TotalItemCount	= 0;
		m_RelativeCursor	= 0;
		m_DisplayStartItem	= 0;
	
		RefreshList (0);
		RepaintList (0);
	}
	else if( m_DirectoryEntries[m_AbsoluteCursor].name[0] == '\0' )
	{
		RMint8 currentfile[FILEBROWSER_MAX_FILENAME_LENGTH];
		//sprintf (currentfile, "%s/%s", m_currentdir, m_DirectoryEntries[m_AbsoluteCursor].name);

DEBUGMSG(1,("FileBrowser::Select () : Empty Directory\n"));

		return FILEBROWSER_ERROR_NO_MORE_ENTRIES;

	}
	else
	{
		RMint8 currentfile[FILEBROWSER_MAX_FILENAME_LENGTH];
		sprintf (currentfile, "%s/%s", m_currentdir, m_DirectoryEntries[m_AbsoluteCursor].name);

DEBUGMSG(1,("FileBrowser::Select () : File [%s] selected\n", m_DirectoryEntries[m_AbsoluteCursor].name) );
	
		m_CallbackTable.renderfile (currentfile, m_CallbackTable.context);
		m_RenderPending = 1;
		m_State = FILE_BROWSER_STATE_WAITING;
	}

*/

int FileBrowser::GetPrevFilename( int current, char filename[] )
{

	int index = current;
	
	// If Infinite Loop
	for(;;)
	{
		// step 1
		if( index == 0 || index > m_TotalItemCount  )
			index = m_TotalItemCount;
		else
			index--;

		// step 2
		printf("[%x]___[%c]___[%x]\n", m_DirectoryEntries[index].flags, m_DirectoryEntries[index].name[0],m_DirectoryEntries[index].name[0] );
		
		
		if ( !(m_DirectoryEntries[index].flags & DIR_ENTRY_INFO_IS_FILE)) 
//			||m_DirectoryEntries[index].name[0] == '\0' ||m_DirectoryEntries[index].name[0] == '\?' )
			index--;
		else
			{
			strcpy( filename, m_DirectoryEntries[index].name );
			return index;
			break;
			}
	}
	return -1;

/*
	// 1 Loop
	for(int i=0; i<m_TotalItemCount; i++)
	{
		// step 1
		if( current == 0 )
			index = m_TotalItemCount;
		else if( current > m_TotalItemCount )
			index = m_TotalItemCount -1;
		else
			index = current -1;

		
		// step 2
		if (m_DirectoryEntries[index].flags & DIR_ENTRY_INFO_IS_DIRECTORY)
			current--;
		else if( m_DirectoryEntries[index].name[0] == '\0' )
			current--;
		else
			strcpy( filename, m_DirectoryEntries[index].name );
			return index;
	}

	return -1;
*/
}

int FileBrowser::GetNextFilename( int current, char filename[] )
{	
	int index = current;
	
	// If Infinite Loop
	for(;;)
	{
		// step 1
		if( index >= m_TotalItemCount ||  index < 0 )
			index = 0;
		else 
			index++;

		// step 2
		if ( (m_DirectoryEntries[index].flags & DIR_ENTRY_INFO_IS_DIRECTORY) 
			||m_DirectoryEntries[index].name[0] == '\0' ||m_DirectoryEntries[index].name[0] == '\?' )
			index++;
		else
			{
			strcpy( filename, m_DirectoryEntries[index].name );
			return index;
			break;
			}
	}
	return -1;
}


FILEBROWSER_ERROR FileBrowser::SetDisplayDimensions (RMint32 w, RMint32 h)
{
	m_ScreenWidth = w;
	m_ScreenHeight = h;
	return FILEBROWSER_ERROR_NO_ERROR;
}

FILEBROWSER_ERROR FileBrowser::SetTextHeight (RMint32 h)
{
	m_TextHeight = h;
	return FILEBROWSER_ERROR_NOT_IMPLEMENTED;
}

FILEBROWSER_ERROR FileBrowser::InitCallbackTable (FILEBROWSER_CALLBACK_TABLE *pCallbackTable)
{
	m_CallbackTable.context = pCallbackTable->context;
	m_CallbackTable.getrootdir = pCallbackTable->getrootdir;
	m_CallbackTable.opendir = pCallbackTable->opendir;
	m_CallbackTable.readdir = pCallbackTable->readdir;
	m_CallbackTable.rendererase = pCallbackTable->rendererase;
	m_CallbackTable.closedir = pCallbackTable->closedir;
	m_CallbackTable.rendericon = pCallbackTable->rendericon;
	m_CallbackTable.rendertext = pCallbackTable->rendertext;
	m_CallbackTable.rendertextwithoutrefresh = pCallbackTable->rendertextwithoutrefresh;
	m_CallbackTable.osdrefresh = pCallbackTable->osdrefresh;
	m_CallbackTable.renderfile = pCallbackTable->renderfile;
	m_CallbackTable.isrenderdone = pCallbackTable->isrenderdone;
	m_CallbackTable.renderstop = pCallbackTable->renderstop;
	m_CallbackTable.info = pCallbackTable->info;
	m_CallbackTable.exitroot = pCallbackTable->exitroot;
	m_CallbackTable.clearosdwithoutrefresh = pCallbackTable->clearosdwithoutrefresh;
	m_CallbackTable.DrawASCIITextWithoutRefresh	= pCallbackTable->DrawASCIITextWithoutRefresh;
	m_CallbackTable.DrawBoxWithoutRefresh		= pCallbackTable->DrawBoxWithoutRefresh;
	

	return FILEBROWSER_ERROR_NO_ERROR;
}

FILEBROWSER_ERROR FileBrowser::Schedule ()
{
	switch (m_State)
	{
	case FILE_BROWSER_STATE_INIT:
		return Init ();
		break;
	case FILE_BROWSER_STATE_BROWSING:
		return Browse ();
		break;
	case FILE_BROWSER_STATE_WAITING:
		return WaitForRender ();
		break;
	case FILE_BROWSER_IDLE:
		break;
	}
	return FILEBROWSER_ERROR_NOT_IMPLEMENTED;
}

FILEBROWSER_ERROR FileBrowser::Right ()
{
	printf("FileBrowser::Right ()\n");
	if (m_RenderPending == 0)
		m_PlayDirectory = 0;

	if (m_State != FILE_BROWSER_STATE_BROWSING)
		return FILEBROWSER_ERROR_COMMAND_NOT_ALLOWED;

	return FILEBROWSER_ERROR_NOT_IMPLEMENTED;
}

FILEBROWSER_ERROR FileBrowser::Left ()
{
	printf("FileBrowser::Left ()\n");
	if (m_RenderPending == 0)
		m_PlayDirectory = 0;

	if (m_State != FILE_BROWSER_STATE_BROWSING)
		return FILEBROWSER_ERROR_COMMAND_NOT_ALLOWED;

	return FILEBROWSER_ERROR_NOT_IMPLEMENTED;
}

FILEBROWSER_ERROR FileBrowser::Stop ()
{
	printf("FileBrowser::Stop ()\n");
	if (m_RenderPending)
	{
		DEBUGMSG (1, ("FileBrowser::Stop\n"));
		ASSERT (m_CallbackTable.renderstop);
		m_CallbackTable.renderstop (m_CallbackTable.context);
		ASSERT (m_CallbackTable.rendererase);
//		m_CallbackTable.rendererase (m_CallbackTable.context);		
		m_RenderPending = 0;
		RepaintListV2 (0);
		m_State = FILE_BROWSER_STATE_BROWSING;
	}
	m_PlayDirectory = 0;
	return FILEBROWSER_ERROR_NO_ERROR;
}

FILEBROWSER_ERROR FileBrowser::Idle ()
{
	printf("FileBrowser::Idle ()\n");

	ASSERT (m_CallbackTable.info);
	m_CallbackTable.info (FILEBROWSER_INFO_MSG_CURRENT_DIRECTORY, (void *)"/", m_CallbackTable.context);
//	ASSERT (m_CallbackTable.rendererase);
//	m_CallbackTable.rendererase (m_CallbackTable.context);
	ASSERT (m_CallbackTable.exitroot);
	m_CallbackTable.exitroot (m_CallbackTable.context);
	m_State = FILE_BROWSER_IDLE;

	return FILEBROWSER_ERROR_NO_ERROR;
}

FILEBROWSER_ERROR FileBrowser::Init ()
{
	RMint32 height;

	DEBUGMSG(1,("----------------------------------------------------------\n[!] FileBrowser::Init ()\n----------------------------------------------------------\n\n"));


	ASSERT (m_ScreenHeight);
	ASSERT (m_TextHeight);

	// calculate how many text items we can fit on the screen
	height = m_ScreenHeight - (m_VerticalOverscan * 2);
	ASSERT (height > m_TextHeight);
	// go for 4 pixels between each text element
	m_LinesOnScreen = height / (m_TextHeight + 4);
	ASSERT (m_LinesOnScreen < FILEBROWSER_MAX_LINES_ON_SCREEN);
	if (m_LinesOnScreen > FILEBROWSER_MAX_LINES_ON_SCREEN)
		m_LinesOnScreen = FILEBROWSER_MAX_LINES_ON_SCREEN;
	DEBUGMSG (1, ("%d lines available on the screen\n", m_LinesOnScreen));
	ASSERT (m_CallbackTable.getrootdir);
	m_CallbackTable.getrootdir (m_rootdir, FILEBROWSER_MAX_FILENAME_LENGTH, m_CallbackTable.context);

	if( m_currentdir[0] == '\0' )
		strcpy (m_currentdir, m_rootdir);

	ASSERT (m_CallbackTable.info);
	m_CallbackTable.info (FILEBROWSER_INFO_MSG_CURRENT_DIRECTORY, m_currentdir, m_CallbackTable.context);
	DEBUGMSG (1, ("opening root directory: %s\n", m_rootdir));
	if (m_handle)
	{
		ASSERT (m_CallbackTable.closedir);
		m_CallbackTable.closedir (m_handle, m_CallbackTable.context);
		m_handle = 0;
	}
	ASSERT (m_handle == 0);
	m_handle = m_CallbackTable.opendir (m_rootdir, m_CallbackTable.context);
	ASSERT (m_handle);
	
//		m_AbsoluteCursor	= 0;
//		m_TotalItemCount	= 0;
//		m_RelativeCursor	= 0;
//		m_DisplayStartItem	= 0;
//		m_TotalSlotNumber	= 9;

	RefreshList (0);
	RepaintListV2 (0);
	m_State = FILE_BROWSER_STATE_BROWSING;
	return FILEBROWSER_ERROR_NO_ERROR;
}

FILEBROWSER_ERROR FileBrowser::Browse ()
{
	if (m_PlayDirectory)
	{
		// do automatic browsing ...
		Down ();
		Select ();
	}
	return FILEBROWSER_ERROR_NO_ERROR;
}

FILEBROWSER_ERROR FileBrowser::WaitForRender ()
{
	ASSERT (m_CallbackTable.isrenderdone);
	ASSERT (m_RenderPending);
	if (m_CallbackTable.isrenderdone (m_CallbackTable.context))
	{
		m_CallbackTable.rendererase (m_CallbackTable.context);
		DEBUGMSG (1, ("current file is finished rendering\n"));
		m_RenderPending = 0;
		RepaintListV2 (1);
		m_State = FILE_BROWSER_STATE_BROWSING;
//		Select ();
	}
	return FILEBROWSER_ERROR_NO_ERROR;
}

/*		   
FILEBROWSER_ERROR FileBrowser::RefreshList (RMint32 n)
{
	RMint32 i;
	ASSERT (m_handle);

	DEBUGMSG (1, ("FileBrowser::RefreshList (%d)\n", n));

	m_ValidItems = 0;
	if (n != m_CurrentIndex)
	{
		if (n < m_CurrentIndex)
		{
			ASSERT (n >= 0);
			// close the current directory handle and re-open it again, and seek to the proper position
			ASSERT (m_handle);
			m_CallbackTable.closedir (m_handle, m_CallbackTable.context);
			m_handle = m_CallbackTable.opendir (m_currentdir, m_CallbackTable.context);
			m_CurrentIndex = 0;
			ASSERT (m_handle);
		}
		else
		{
			n -= m_CurrentIndex;
			ASSERT (n > 0);
		}
		while (n)
		{
			strcpy (m_DirectoryEntries[0].parentdir, m_currentdir);
			if (m_CallbackTable.readdir (m_handle, &m_DirectoryEntries[0], m_CallbackTable.context) == 0)
			{
				return FILEBROWSER_ERROR_NO_REPAINT_REQUIRED;			
			}
			m_CurrentIndex++;
			n--;
		}
		m_CurrentIndex-=2;
		m_DirectoryEntries[0] = m_DirectoryEntries[2];
		
	}
	for (i=0; i<m_LinesOnScreen; i++)
	{
		strcpy (m_DirectoryEntries[i].parentdir, m_currentdir);
		if (m_CallbackTable.readdir (m_handle, &m_DirectoryEntries[i], m_CallbackTable.context) == 0)
		{
			DEBUGMSG (1, ("no more entries\n"));
			break;
		}
		m_ValidItems++;
		m_CurrentIndex++;
	}
	if (m_ValidItems)
		m_CallbackTable.rendererase (m_CallbackTable.context);
	return FILEBROWSER_ERROR_NO_ERROR;
}
*/




/*
FILEBROWSER_ERROR FileBrowser::RepaintList ()
{
	RMint32 x, y, maxwidth, i, vertical_center_offset;

	x = m_HorizontalOverscan;
	vertical_center_offset = 
		((m_ScreenHeight - (m_VerticalOverscan * 2)) - 
		((m_TextHeight + 4) * m_LinesOnScreen)) / 2;
	y = m_VerticalOverscan + vertical_center_offset + 2;
	maxwidth = m_ScreenWidth - (m_HorizontalOverscan * 2) - (m_IconWidth + 4);

	//for (i=2; i<m_ValidItems; i++) 
	for (i=0; i<m_ValidItems; i++)  // skip '.' , '..'
	{
		FILEBROWSER_ICON_TYPE type;
		RMint32 len = strlen (m_DirectoryEntries[i].name);

		if (m_DirectoryEntries[i].flags & DIR_ENTRY_INFO_IS_DIRECTORY)
		{
			type = FILEBROWSER_ICON_DIRECTORY;
		}
		else if ((m_DirectoryEntries[i].name[len-4] == '.') &&
			((m_DirectoryEntries[i].name[len-3] == 'M') || (m_DirectoryEntries[i].name[len-3] == 'm')) &&
			((m_DirectoryEntries[i].name[len-2] == 'P') || (m_DirectoryEntries[i].name[len-2] == 'p')) &&
			((m_DirectoryEntries[i].name[len-1] == '3')))
		{
			type = FILEBROWSER_ICON_AUDIO;
		}
		else if ((m_DirectoryEntries[i].name[len-4] == '.') &&
			((m_DirectoryEntries[i].name[len-3] == 'W') || (m_DirectoryEntries[i].name[len-3] == 'w')) &&
			((m_DirectoryEntries[i].name[len-2] == 'M') || (m_DirectoryEntries[i].name[len-2] == 'm')) &&
			((m_DirectoryEntries[i].name[len-1] == 'A') || (m_DirectoryEntries[i].name[len-1] == 'a')))
		{
			type = FILEBROWSER_ICON_AUDIO;
		}
		else if ((m_DirectoryEntries[i].name[len-4] == '.') &&
			((m_DirectoryEntries[i].name[len-3] == 'O') || (m_DirectoryEntries[i].name[len-3] == 'o')) &&
			((m_DirectoryEntries[i].name[len-2] == 'G') || (m_DirectoryEntries[i].name[len-2] == 'g')) &&
			((m_DirectoryEntries[i].name[len-1] == 'G') || (m_DirectoryEntries[i].name[len-1] == 'g')))
		{
			type = FILEBROWSER_ICON_AUDIO;
		}
		else if ((m_DirectoryEntries[i].name[len-4] == '.') &&
			((m_DirectoryEntries[i].name[len-3] == 'A') || (m_DirectoryEntries[i].name[len-3] == 'a')) &&
			((m_DirectoryEntries[i].name[len-2] == 'C') || (m_DirectoryEntries[i].name[len-2] == 'c')) &&
			((m_DirectoryEntries[i].name[len-1] == '3') || (m_DirectoryEntries[i].name[len-1] == '3')))
		{
			type = FILEBROWSER_ICON_AUDIO;
		}
		else if ((m_DirectoryEntries[i].name[len-4] == '.') &&
			((m_DirectoryEntries[i].name[len-3] == 'A') || (m_DirectoryEntries[i].name[len-3] == 'a')) &&
			((m_DirectoryEntries[i].name[len-2] == 'V') || (m_DirectoryEntries[i].name[len-2] == 'v')) &&
			((m_DirectoryEntries[i].name[len-1] == 'I') || (m_DirectoryEntries[i].name[len-1] == 'i')))
		{
			type = FILEBROWSER_ICON_VIDEO;
		}
		else if ((m_DirectoryEntries[i].name[len-4] == '.') &&
			((m_DirectoryEntries[i].name[len-3] == 'M') || (m_DirectoryEntries[i].name[len-3] == 'm')) &&
			((m_DirectoryEntries[i].name[len-2] == 'P') || (m_DirectoryEntries[i].name[len-2] == 'p')) &&
			((m_DirectoryEntries[i].name[len-1] == 'G') || (m_DirectoryEntries[i].name[len-1] == 'g')))
		{
			type = FILEBROWSER_ICON_VIDEO;
		}
		else if ((m_DirectoryEntries[i].name[len-5] == '.') &&
			((m_DirectoryEntries[i].name[len-4] == 'M') || (m_DirectoryEntries[i].name[len-3] == 'm')) &&
			((m_DirectoryEntries[i].name[len-3] == 'P') || (m_DirectoryEntries[i].name[len-2] == 'p')) &&
			((m_DirectoryEntries[i].name[len-2] == 'E') || (m_DirectoryEntries[i].name[len-2] == 'e')) &&
			((m_DirectoryEntries[i].name[len-1] == 'G') || (m_DirectoryEntries[i].name[len-1] == 'g')))
		{
			type = FILEBROWSER_ICON_VIDEO;
		}
		else if ((m_DirectoryEntries[i].name[len-4] == '.') &&
			((m_DirectoryEntries[i].name[len-3] == 'V') || (m_DirectoryEntries[i].name[len-3] == 'v')) &&
			((m_DirectoryEntries[i].name[len-2] == 'O') || (m_DirectoryEntries[i].name[len-2] == 'o')) &&
			((m_DirectoryEntries[i].name[len-1] == 'B') || (m_DirectoryEntries[i].name[len-1] == 'b')))
		{
			type = FILEBROWSER_ICON_VIDEO;
		}
		else if ((m_DirectoryEntries[i].name[len-4] == '.') &&
			((m_DirectoryEntries[i].name[len-3] == 'J') || (m_DirectoryEntries[i].name[len-3] == 'j')) &&
			((m_DirectoryEntries[i].name[len-2] == 'P') || (m_DirectoryEntries[i].name[len-2] == 'p'))&&
			((m_DirectoryEntries[i].name[len-1] == 'G') || (m_DirectoryEntries[i].name[len-1] == 'g')))
		{
			type = FILEBROWSER_ICON_IMAGE;
		}
		else if ((m_DirectoryEntries[i].name[len-4] == '.') &&
			((m_DirectoryEntries[i].name[len-3] == 'M') || (m_DirectoryEntries[i].name[len-3] == 'm')) &&
			((m_DirectoryEntries[i].name[len-2] == 'P') || (m_DirectoryEntries[i].name[len-2] == 'p')) &&
			((m_DirectoryEntries[i].name[len-1] == '4')))
		{
			type = FILEBROWSER_ICON_VIDEO;
		}
		else
		{
			type = FILEBROWSER_ICON_UNKNOWN;
		}
		if ((m_DirectoryEntries[i].name[0] == '.') &&
			(m_DirectoryEntries[i].name[1] == 0x0))
		{
			m_CallbackTable.rendericon (FILEBROWSER_ICON_DIRECTORY, x, y, m_IconWidth, m_IconHeight, (i == m_SelectedItem), m_CallbackTable.context);
			m_CallbackTable.rendertext (
				"Auto-Play Directory", 
				x + m_IconWidth + 4, 
				y, 
				maxwidth, 
				m_TextHeight, 
				(i == m_SelectedItem), 
				m_CallbackTable.context);
		}
		else if ((m_DirectoryEntries[i].name[0+m_filenameoffset] == '.') &&
			(m_DirectoryEntries[i].name[1+m_filenameoffset] == '.') &&
			(m_DirectoryEntries[i].name[2+m_filenameoffset] == 0x0) &&
			(strcmp (m_rootdir, m_currentdir) == 0))
		{
			m_CallbackTable.rendericon (FILEBROWSER_ICON_DIRECTORY, x, y, m_IconWidth, m_IconHeight, (i == m_SelectedItem), m_CallbackTable.context);
			m_CallbackTable.rendertext ("..", 
				x + m_IconWidth + 4, y, maxwidth, m_TextHeight, 
				(i == m_SelectedItem), m_CallbackTable.context);
		}
		else
		{
			m_CallbackTable.rendericon (type, x, y, m_IconWidth, m_IconHeight, (i == m_SelectedItem), m_CallbackTable.context);
			m_CallbackTable.rendertext (
				m_DirectoryEntries[i].name+m_filenameoffset,
				x + m_IconWidth + 4, 
				y, 
				maxwidth, 
				m_TextHeight, 
				(i == m_SelectedItem), 	// Color Index
				m_CallbackTable.context
				);
		}
		y += m_TextHeight + 4;
	}
	return FILEBROWSER_ERROR_NO_ERROR;
}
*/





/*
	RMint32			m_SelectedItem;	// Range : 0 ~ m_TotalItemCount
	RMint32			m_CursorPos;		// Range : 0 ~ m_MaxCurPos

	RMint32			m_DisplayEndItem;	// Range : 0~  m_MaxCurPos
	RMint32			m_TotalItemCount;	// Range : 0 ~ m_TotalItemCount   Fixed Value


	RMint32			m_DisplayStartItem;	// Range : 0~  m_MaxCurPos

	RMint32			m_AbsoluteCursor;	// Range : 0 ~ m_TotalItemCount
	RMint32			m_TotalItemCount;

	RMint32			m_RelativeCursor;	// Range : 0 ~ m_TotalSlotNumber;
	RMint32 			m_TotalSlotNumber;

*/

FILEBROWSER_ERROR FileBrowser::Up ()
{
	if (m_State != FILE_BROWSER_STATE_BROWSING)
		return FILEBROWSER_ERROR_COMMAND_NOT_ALLOWED;

	printf("Before Up()\n");
	printf("==============================\n");
	printf("m_AbsoluteCursor\t: [%d/%d]\n", m_AbsoluteCursor, m_TotalItemCount);
	printf("m_RelativeCursor\t: [%d/%d]\n", m_RelativeCursor, m_TotalSlotNumber);
	printf("m_DisplayStartItemt: [%d]\n", m_DisplayStartItem);
	printf("==============================\n\n");

	if( m_AbsoluteCursor == 0 )
		return FILEBROWSER_ERROR_NO_MORE_ENTRIES;

	if( m_RelativeCursor == 0 )
		{
		m_AbsoluteCursor--;
		m_DisplayStartItem--;
		}
	else if( m_RelativeCursor > 0 )
		{
		m_AbsoluteCursor--;
		m_RelativeCursor--;
		}

	RepaintListV2(1);

	printf("After Up()\n");
	printf("==============================\n");
	printf("m_AbsoluteCursor\t: [%d/%d]\n", m_AbsoluteCursor, m_TotalItemCount);
	printf("m_RelativeCursor\t: [%d/%d]\n", m_RelativeCursor, m_TotalSlotNumber);
	printf("m_DisplayStartItemt: [%d]\n", m_DisplayStartItem);
	printf("==============================\n\n");
	
	return FILEBROWSER_ERROR_NO_ERROR;
}

FILEBROWSER_ERROR FileBrowser::Down ()
{

	if (m_State != FILE_BROWSER_STATE_BROWSING)
		return FILEBROWSER_ERROR_COMMAND_NOT_ALLOWED;

	printf("After Down()\n");
	printf("==============================\n");
	printf("m_AbsoluteCursor\t: [%d/%d]\n", m_AbsoluteCursor, m_TotalItemCount);
	printf("m_RelativeCursor\t: [%d/%d]\n", m_RelativeCursor, m_TotalSlotNumber);
	printf("m_DisplayStartItemt: [%d]\n", m_DisplayStartItem);
	printf("==============================\n\n");


	if( m_AbsoluteCursor == m_TotalItemCount-1 )
		return FILEBROWSER_ERROR_NO_MORE_ENTRIES;

	if( m_RelativeCursor == m_TotalSlotNumber-1 )
		{
		m_AbsoluteCursor++;
		m_DisplayStartItem++;
		}
	else if( m_RelativeCursor < m_TotalSlotNumber-1 )
		{
		m_AbsoluteCursor++;
		m_RelativeCursor++;
		}

	RepaintListV2(1);

	printf("Before Down()\n");
	printf("==============================\n");
	printf("m_AbsoluteCursor\t: [%d/%d]\n", m_AbsoluteCursor, m_TotalItemCount);
	printf("m_RelativeCursor\t: [%d/%d]\n", m_RelativeCursor, m_TotalSlotNumber);
	printf("m_DisplayStartItemt: [%d]\n", m_DisplayStartItem);
	printf("==============================\n\n");
	
	return FILEBROWSER_ERROR_NO_ERROR;
}


/*
void RefreshList (unsigned long n)
{
	DIR*				dp;
	FILE*	 			fp;
//	struct dirent**		aNameList;
//	struct stat 			statbuf;

	struct dirent *dentry;
	static char filename[1024];
	struct stat stbuf;

	char	sdir[1024];
	char m_currentdir[256];

	int					number=0;
	int					filetype=0;
	int					i=0, j=0;

	sprintf( m_currentdir, "%s", "/hd1/video" );
	
	if( (dp=opendir( m_currentdir )) < 0)
		perror("[!]Failed to opendir() in RefreshList ");
	if(chdir(m_currentdir) < 0)
		perror("[!]Failed to fchdir() in RefreshList ");
//	if ((number=scandir(  "/hd1/video" , &aNameList, 0, alphasort ))< 0)
//		perror("[!]Failed to scandir() in RefreshList ");

	while((dentry = readdir(dp)) != NULL)
	{
		if(dentry->d_ino == 0)
			continue;
		memcpy(filename, dentry->d_name, 1023);
		if(stat(filename, &stbuf) == -1)
		{
			perror(filename);
			break;	
		}

		if((stbuf.st_mode & S_IFMT) == S_IFREG)
			printf("%-14s %d\n", filename, stbuf.st_size);
		else
		printf("%-14s\n",filename);
	}

	closedir( dp );

}
*/

#define TITLE_X 		14
#define TITLE_Y			36
#define BAR_WIDTH		292

FILEBROWSER_ERROR FileBrowser::RefreshList (RMint32 n)
{
	DIR*				dp;
//	FILE*	 			fp;
	struct dirent*			aNameList;
	struct stat 			statbuf;

	char	sdir[1024];

	int					number=0;
	int					filetype=0;
	int					i=0, j=0;
	m_CurrentIndex 		= 0;

	m_AbsoluteCursor		= 0;
	m_TotalItemCount		= 0;
	m_RelativeCursor		= 0;
	m_DisplayStartItem	= 0;
	m_TotalSlotNumber	= 9;

	for(int x=0; x<FILEBROWSER_MAX_PLAYLIST_LENGTH; x++)
		{
		if( m_DirectoryEntries[x].name[0] == '\0' ) 
			break;
		m_DirectoryEntries[x].flags			= 0;
		m_DirectoryEntries[x].handle		= 0;
		m_DirectoryEntries[x].name[0] 		= '\0';
		m_DirectoryEntries[x].namelength	= 0;
		m_DirectoryEntries[x].parentdir[0]	= '\0';
		}

	
	if( (dp=opendir( m_currentdir )) < 0)
		perror("[!]Failed to opendir() in RefreshList ");

	else if( chdir(m_currentdir) < 0)
		perror("[!]Failed to chdir() in RefreshList ");
	
	else
	{

		while((aNameList = readdir(dp)) != NULL)
		{
			if(aNameList->d_ino == 0)
				continue;
			
			memcpy(sdir, aNameList->d_name, 1023);

			if( sdir[0] == '.' )
				continue;

			if(stat(sdir, &statbuf) == -1) 	{
				perror(sdir);
				break;	
			}

			if( S_ISREG(statbuf.st_mode) ) 
				filetype = DIR_ENTRY_INFO_IS_FILE;
			else if( S_ISDIR(statbuf.st_mode) ) 
				filetype = DIR_ENTRY_INFO_IS_DIRECTORY;
			else 
				continue;

			m_DirectoryEntries[i].flags				= filetype;

//			m_DirectoryEntries[i].handle	=

			sprintf( m_DirectoryEntries[i].name, "%s", sdir );

			m_DirectoryEntries[i].namelength 		= strlen(sdir);

			sprintf( m_DirectoryEntries[i].parentdir, "%s", m_currentdir );
 
			i++;
			
		}

		free( aNameList );
		m_TotalItemCount 	= i;
		printf("\n------------------------------------------\n");
		printf("m_TotalItemCount=[%d]  i=[%d]\n", m_TotalItemCount, i );
		printf("------------------------------------------\n");
		
	}

	closedir( dp );
			
	return FILEBROWSER_ERROR_NO_ERROR;
}

/*
FILEBROWSER_ERROR FileBrowser::RefreshList (RMint32 n)
{

	FILE*	 			fp;
	struct dirent**		aNameList;
	struct stat 			statbuf;

	char	sdir[1024];

	int					number=0;
	int					filetype=0;
	int					i=0, j=0;
	m_CurrentIndex 		= 0;

	m_AbsoluteCursor		= 0;
	m_TotalItemCount		= 0;
	m_RelativeCursor		= 0;
	m_DisplayStartItem	= 0;
	m_TotalSlotNumber	= 9;


//	FILEBROWSER_MAX_PLAYLIST_LENGTH

	for(int x=0; x<FILEBROWSER_MAX_PLAYLIST_LENGTH; x++)
		{
		if( m_DirectoryEntries[x].name[0] == '\0' ) 
			break;
		m_DirectoryEntries[x].flags			= 0;
		m_DirectoryEntries[x].handle		= 0;
		m_DirectoryEntries[x].name[0] 		= '\0';
		m_DirectoryEntries[x].namelength	= 0;
		m_DirectoryEntries[x].parentdir[0]	= '\0';
		}
	
	DEBUGMSG(1, ("(1)RefreshList():m_currentdir=[%s] chdir() result=[%d]\n", m_currentdir, chdir((char*)m_currentdir)) );

//	printchdir((char*)m_currentdir);
	m_handle 			= m_CallbackTable.opendir (m_currentdir, m_CallbackTable.context);
	DEBUGMSG(1, ("(2)RefreshList():m_currentdir=[%s]  number=[%d] m_handle=[%d]\n", m_currentdir, number, m_handle) );
//	number 				= scandir( (char*)m_currentdir, &aNameList, 0, alphasort );
	usleep(100000);
	number 				= scandir( ".", &aNameList, 0, alphasort );
//	number				= m_CallbackTable.scandir( ".", &aNameList, 0, alphasort );

//	m_CallbackTable.rendertext( "Music Library", TITLE_X, TITLE_Y, BAR_WIDTH, m_TextHeight, _WHITE, m_CallbackTable.context );


	
	if (number < 0)
	{
		perror("Failed to scandir() in RefreshList ");
		//DEBUGMSG(1,("[!%d] Failed to RefreshList(): scandir result number=[%d]\n", number));
	}
	else
	{
		for( i=0, j=0; j<number; j++ )
		{	

			if( aNameList[j]->d_name[0] == '.' )
				continue;

			sprintf( sdir, "%s/%s", m_currentdir, aNameList[j]->d_name );
			
			DEBUGMSG(1,( "sdir : [%s]\n", sdir  ));

//			if( lstat( aNameList[j]->d_name, &statbuf ) < 0 ) 
			if( lstat( sdir, &statbuf ) < 0 ) 
			{
				DEBUGMSG(1,( "failed to lstat( %s )\n", aNameList[i]->d_name));
				continue;
			}

			if( S_ISREG(statbuf.st_mode) ) 
				filetype = DIR_ENTRY_INFO_IS_FILE;
			else if( S_ISDIR(statbuf.st_mode) ) 
				filetype = DIR_ENTRY_INFO_IS_DIRECTORY;
			else 
				continue;

			m_DirectoryEntries[i].flags		= filetype;
//			m_DirectoryEntries[i].handle	=
//			m_DirectoryEntries[i].name 	= 	aNameList[j]->d_name;
			sprintf( m_DirectoryEntries[i].name, "%s", aNameList[j]->d_name );
			free(aNameList[j]);
//			m_DirectoryEntries[i].namelength
			m_DirectoryEntries[i].parentdir	= m_currentdir;

			i++;

		}
		free( aNameList );
		m_TotalItemCount 	= i;
		printf("\n------------------------------------------\n");
		printf("m_TotalItemCount=[%d]  i=[%d]\n", m_TotalItemCount, i );
		printf("------------------------------------------\n");
		
	}

			
	return FILEBROWSER_ERROR_NO_ERROR;
}
*/



/*
void AP_ChageAndScanDir(char aNewDir[])
{
	FILE*	 			fp;
	struct dirent**		aNameList;
	struct stat 			statbuf;
	tAUDIOFILE_NODE*	node;
//	char					aComponentDir[256];
	char					aFilename[256];
	int					number;
	int					filetype;

	chdir( aNewDir );
	sprintf( AudioPlayer.CurrentDir, "%s%c", aNewDir, '\0' );

	number = scandir( AudioPlayer.CurrentDir, &aNameList, 0, alphasort );
	if (number < 0)
		{
		perror("scandir");
		//ShowMessage( INFO_FATAL, ERROR_MSG_OPEN_DIRECTORY );
		// return to main menu
		return;	
		}
	else
	{
		for(int  i=0; i<number; i++ )
		{
			// skip current dir symbol
			if( strcmp( CURRENT_DIR, aNameList[i]->d_name ) == 0 ) continue; 
			if( strcmp( PARENT_DIR, aNameList[i]->d_name ) == 0 ) continue;
			if( lstat( aNameList[i]->d_name, &statbuf ) < 0 ) 
				{
				printf( "failed to lstat( %s )\n", aNameList[i]->d_name);
				continue;
				}

			if( S_ISREG(statbuf.st_mode) ) 
				filetype = FILE_TYPE_REGULAR;
			else if( S_ISDIR(statbuf.st_mode) ) 
				filetype = FILE_TYPE_DIRECTORY;
			else 
				continue;
		
			sprintf( aFilename, "%s/%s%c", _MUSIC_DIR, aNameList[i]->d_name, '\0' );
			//printf("[%s][%s]\n", aNameList[i]->d_name, number);
			
			if( (fp=fopen( aFilename, "rb" )) != NULL )
			{
				node 			= new tAUDIOFILE_NODE;
				node->Pathname 	= new char[ strlen( AudioPlayer.CurrentDir ) ];
				node->Filename	= new char[ aNameList[i]->d_reclen ];
				node->Tag		= new ID3v1;

				strcpy( node->Pathname, AudioPlayer.CurrentDir);
				strcpy( node->Filename	, aNameList[i]->d_name );

				// this function returns TRUE/FALSE by ID3v1 tag's exist
				// if it FALSE, 'node->ID3v1' has 'NULL' value.
				// so, when we display the information in ID3 tags, 
				// we must check the ptag has 'NULL' value.
				AP_CheckID3v1( fp, node->Tag );

				AudioPlayer.list.Add( node );

				fclose(fp);
			}
			free(aNameList[i]);
		}
		free(aNameList);
	}	
}
*/

#define LIST_TEXT_SX	45
#define LIST_TEXT_SY	56
#define LIST_TEXT_WIDTH	262

#define LIST_CTRL_AREA_SX		TITLE_X
#define LIST_CTRL_AREA_SY		LIST_TEXT_SY
#define LIST_CTRL_AREA_WIDTH	292
#define LIST_CTRL_AREA_HEIGHT	150

#define MAX(X,Y) (((X)>=(Y))?(X):(Y))	// to find the max between two number.
#define MIN(X,Y) (((X)<=(Y))?(X):(Y))	// to find the min between two number.


FILEBROWSER_ERROR FileBrowser::RepaintList (int option)
{
	int i,j,color;
	// Clear List Control

	if(option == 1 ) 
		{
		m_CallbackTable.rendertextwithoutrefresh(
			0,
			LIST_CTRL_AREA_SX, 		LIST_CTRL_AREA_SY, 
			LIST_CTRL_AREA_WIDTH, 	LIST_CTRL_AREA_HEIGHT,
			0,
			m_CallbackTable.context
			);
		}
	else
		{
		char title[256];				// "/hd1/music/playlist"
		int sp = strlen(m_currentdir);	// 19
		
		for( int i=sp;i>=0; i--)
		{
			if( m_currentdir[i] == '/' )	// m_currentdir[19] ='\0', m_currentdir[18] ='t' 
			{					// i=11
				strcpy( title, m_currentdir + i +1);
				break;
			}
		}
		DEBUGMSG(1,("FileBrowser::RepaintList - title=[%s]\n", title) );
		m_CallbackTable.rendertextwithoutrefresh(0,0,30,320,210,0,m_CallbackTable.context);
		m_CallbackTable.rendertextwithoutrefresh( title, TITLE_X, TITLE_Y, BAR_WIDTH, m_TextHeight, _WHITE, m_CallbackTable.context );		
		}
/*

	m_CallbackTable.rendertext(
		0,
		LIST_CTRL_AREA_SX, 
		LIST_CTRL_AREA_SY, 
		LIST_CTRL_AREA_WIDTH, 
		LIST_CTRL_AREA_HEIGHT,
		0,
		m_CallbackTable.context
		);
		
	m_CallbackTable.clearosdwithoutrefresh(
		LIST_CTRL_AREA_SX, 
		LIST_CTRL_AREA_SY, 
		LIST_CTRL_AREA_WIDTH, 
		LIST_CTRL_AREA_HEIGHT
		);
	
	m_CallbackTable.clearosd(
		LIST_CTRL_AREA_SX, 
		LIST_CTRL_AREA_SY, 
		LIST_CTRL_AREA_WIDTH, 
		LIST_CTRL_AREA_HEIGHT
		);	
*/


		m_CallbackTable.DrawBoxWithoutRefresh( 
			_WHITE, 
			LIST_CTRL_AREA_SX-3,		//LIST_CTRL_AREA_SX, 
			LIST_TEXT_SY,				//LIST_TEXT_SY + ( m_TextHeight * j ), 
			320-LIST_CTRL_AREA_SX*2, 
			1						//	m_TextHeight 
			);



	for(  i=m_DisplayStartItem,j=0; i<MIN( (m_DisplayStartItem + m_TotalSlotNumber) , (m_TotalItemCount)  ); i++, j++ )
	{	
		int font;
		if( i == m_AbsoluteCursor)
			{
			color = _BLACK;
			font = GULIM_13_BLACK_BOLD;
			//font = FONT_DOTUM12_YELLOW;
			}
		else
			{
			color = _WHITE;
			font = GULIM_13_WHITE_NORMAL_BORDER;
//			font = FONT_DOTUM12;
			}



		if( (m_DirectoryEntries[i].flags & DIR_ENTRY_INFO_IS_FILE) )
			//m_CallbackTable.rendericon( 15, 24, LIST_TEXT_SY + ( m_TextHeight * j )+4, 12, 10 );
			m_CallbackTable.rendericon( 20, 24, LIST_TEXT_SY + ( m_TextHeight * j )+4, 12, 12 );
		else 
			m_CallbackTable.rendericon( 4, 24, LIST_TEXT_SY + ( m_TextHeight * j )+4, 12, 12 );
/*	
		m_CallbackTable.rendertextwithoutrefresh( 
			m_DirectoryEntries[i].name, 
			LIST_TEXT_SX, 
			LIST_TEXT_SY + ( m_TextHeight * j ), 
			LIST_TEXT_WIDTH, 
			m_TextHeight, 
			color, 
			m_CallbackTable.context 
			);
*/
		m_CallbackTable.DrawASCIITextWithoutRefresh( 
			m_DirectoryEntries[i].name, 
			16, 
			15, 
			&TAVI_IMAGES[font], 
			LIST_TEXT_SX-8, //55, 
			LIST_TEXT_SY+3 + ( m_TextHeight * j ), //120-1, 
			1 );	
		
		DEBUGMSG(1,( "((%s))\n", m_DirectoryEntries[i].name ));
		
		
	}

	m_CallbackTable.osdrefresh();

	DEBUGMSG(1,("eof RepaintList()\n"));
	
	return FILEBROWSER_ERROR_NO_ERROR;
}

FILEBROWSER_ERROR FileBrowser::RepaintListV2 (int option)
{
	int i,j,color;
	// Clear List Control

	if(option == 1 ) 
		{
		m_CallbackTable.rendertextwithoutrefresh(
			0,
			LIST_CTRL_AREA_SX, 		LIST_CTRL_AREA_SY, 
			LIST_CTRL_AREA_WIDTH, 	LIST_CTRL_AREA_HEIGHT,
			0,
			m_CallbackTable.context
			);
		}
	else
		{
		char title[256];				// "/hd1/music/playlist"
		int sp = strlen(m_currentdir);	// 19
		
		for( int i=sp;i>=0; i--)
		{
			if( m_currentdir[i] == '/' )	// m_currentdir[19] ='\0', m_currentdir[18] ='t' 
			{					// i=11
				strcpy( title, m_currentdir + i +1);
				break;
			}
		}

		// Top Left Video Item
//		pMpegDecoder->RenderOsdBits_WithoutRefresh( (RMuint8*)(TAVI_IMAGES[ICON_VIDEOPLAYER].image_buffer), 2, 1, 24,18 );	
//		DrawASCIITextWithoutRefresh( "Video", 16, 16, &TAVI_IMAGES[GULIM_16_WHITE_BOLD_BORDER], 26, 2, __TEXT);



		DEBUGMSG(1,("FileBrowser::RepaintListV2 - title=[%s]\n", title) );
		m_CallbackTable.rendertextwithoutrefresh(0,0,30,320,210,0,m_CallbackTable.context);
		m_CallbackTable.DrawItemWithoutRefresh(&TAVI_IMAGES[ICON_VIDEOPLAYER], 0, 2, 1, 24, 18 );
		m_CallbackTable.DrawASCIITextWithoutRefresh( title, 16, 16, &TAVI_IMAGES[GULIM_16_WHITE_BOLD_BORDER], 26, 2, 1);
		// Top Right Battery Level
		m_CallbackTable.DrawItemWithoutRefresh( &TAVI_IMAGES[ICON_TOP_BATTERY], 5, BATTERY_X, BATTERY_Y, 24, 13 ); 
		m_CallbackTable.osdrefresh();
		

		}

		m_CallbackTable.DrawBoxWithoutRefresh( 
			_WHITE, 
			LIST_CTRL_AREA_SX-3,		//LIST_CTRL_AREA_SX, 
			LIST_TEXT_SY,				//LIST_TEXT_SY + ( m_TextHeight * j ), 
			320-LIST_CTRL_AREA_SX*2, 
			1						//	m_TextHeight 
			);



	for(  i=m_DisplayStartItem,j=0; i<MIN( (m_DisplayStartItem + m_TotalSlotNumber) , (m_TotalItemCount)  ); i++, j++ )
	{	
		int font;
		if( i == m_AbsoluteCursor)
			{
			color = _BLACK;
			font = GULIM_13_BLACK_BOLD;
			}
		else
			{
			color = _WHITE;
			font = GULIM_13_WHITE_NORMAL_BORDER;
			}



		if( (m_DirectoryEntries[i].flags & DIR_ENTRY_INFO_IS_FILE) )
			//m_CallbackTable.rendericon( 15, 24, LIST_TEXT_SY + ( m_TextHeight * j )+4, 12, 10 );
			m_CallbackTable.rendericon( 20, 24, LIST_TEXT_SY + ( m_TextHeight * j )+4, 12, 12 );
		else 
			m_CallbackTable.rendericon( 4, 24, LIST_TEXT_SY + ( m_TextHeight * j )+4, 12, 12 );
/*	
		m_CallbackTable.rendertextwithoutrefresh( 
			m_DirectoryEntries[i].name, 
			LIST_TEXT_SX, 
			LIST_TEXT_SY + ( m_TextHeight * j ), 
			LIST_TEXT_WIDTH, 
			m_TextHeight, 
			color, 
			m_CallbackTable.context 
			);
*/
		m_CallbackTable.DrawASCIITextWithoutRefresh( 
			m_DirectoryEntries[i].name, 
			16, 
			15, 
			&TAVI_IMAGES[font], 
			LIST_TEXT_SX-8, //55, 
			LIST_TEXT_SY+3 + ( m_TextHeight * j ), //120-1, 
			1 );	
		
		DEBUGMSG(1,( "((%s))\n", m_DirectoryEntries[i].name ));
		
		
	}

	m_CallbackTable.osdrefresh();

	DEBUGMSG(1,("eof RepaintListV2()\n"));
	
	return FILEBROWSER_ERROR_NO_ERROR;
}

FILEBROWSER_ERROR FileBrowser::Select ()
{
	if (m_State != FILE_BROWSER_STATE_BROWSING)
		return FILEBROWSER_ERROR_COMMAND_NOT_ALLOWED;

	if (m_DirectoryEntries[m_AbsoluteCursor].flags & DIR_ENTRY_INFO_IS_DIRECTORY)
	{
		char sdir[1024];

		DEBUGMSG(1,("FileBrowser::Select () : Directory [%s] selected\n", m_DirectoryEntries[m_AbsoluteCursor].name) );

/*
		else
		{
			strcat (m_currentdir, "/");
			strcat (m_currentdir, m_DirectoryEntries[m_SelectedItem].name);
			DEBUGMSG (1, ("new directory: %s\n", m_currentdir));
		}
*/		
		ASSERT (m_handle);
		ASSERT (m_CallbackTable.closedir);
		printf("close current directory\n");
		m_CallbackTable.closedir (m_handle, m_CallbackTable.context);

		sprintf(m_currentdir, "%s/%s", m_currentdir, m_DirectoryEntries[m_AbsoluteCursor].name );
		printf("open new directory: [%s]\n", m_currentdir);
		m_handle = m_CallbackTable.opendir (m_currentdir, m_CallbackTable.context);
		ASSERT (m_handle);
		ASSERT (m_CallbackTable.info);
		m_CallbackTable.info (FILEBROWSER_INFO_MSG_CURRENT_DIRECTORY, m_currentdir, m_CallbackTable.context);
/*
		m_AbsoluteCursor	= 0;
		m_TotalItemCount	= 0;
		m_RelativeCursor	= 0;
		m_DisplayStartItem	= 0;
*/	
		RefreshList (0);
		RepaintListV2 (0);
	}
	else if( m_DirectoryEntries[m_AbsoluteCursor].name[0] == '\0' )
	{
		RMint8 currentfile[FILEBROWSER_MAX_FILENAME_LENGTH];
		//sprintf (currentfile, "%s/%s", m_currentdir, m_DirectoryEntries[m_AbsoluteCursor].name);

		DEBUGMSG(1,("FileBrowser::Select () : Empty Directory\n"));

		return FILEBROWSER_ERROR_NO_MORE_ENTRIES;

	}
	else
	{
		RMint8 currentfile[FILEBROWSER_MAX_FILENAME_LENGTH];
		sprintf (currentfile, "%s/%s", m_currentdir, m_DirectoryEntries[m_AbsoluteCursor].name);

		DEBUGMSG(1,("FileBrowser::Select () : File [%s] selected\n", m_DirectoryEntries[m_AbsoluteCursor].name) );
	
		m_CallbackTable.renderfile (currentfile, m_CallbackTable.context);
		m_RenderPending = 1;
		m_State = FILE_BROWSER_STATE_WAITING;
	}

	return FILEBROWSER_ERROR_NO_ERROR;
}




