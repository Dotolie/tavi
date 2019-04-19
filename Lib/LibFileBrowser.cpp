//************************ Copyright(C) New Media Life Inc. ************************
//
// Description : LibFileBrowser Source Code File  for TAVI 1600
// 
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibFileBrowser.cpp,v $
// $Revision: 1.11 $
// $Date: 2006/06/22 01:49:34 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: LibFileBrowser.cpp,v $
// Revision 1.11  2006/06/22 01:49:34  zstein
// - added a function checking enjoyed movies.
//
// Revision 1.10  2006/04/18 09:17:39  zstein
// - fixed buffer overflow bug.
//
// Revision 1.9  2006/04/17 01:55:38  ywkim
// Change & Create AddFileItem_UC16_OnTop for TAVICAST
//
// Revision 1.8  2006/04/15 03:01:16  ywkim
// Change some procedure for stopofftimer
//
// Revision 1.7  2006/04/15 02:57:13  zstein
// - removed TAVI_KEY_HOLDOFF routine.
//
// Revision 1.6  2006/04/14 12:34:52  ywkim
// Change & Add some instruction for 'smil', subtitile extention
//
// Revision 1.5  2006/04/13 02:34:11  ywkim
// Change delimiter '\' to '/'
//
// Revision 1.4  2006/04/13 01:45:21  ywkim
// Add function for TAVICAST
//
// Revision 1.3  2006/04/12 04:28:47  ywkim
// Add some function of df_linux(), df_fatfs() & GetFreeSpace()
//
// Revision 1.2  2006/04/07 08:15:56  ywkim
// Change many function for Host
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//*****************************************************************************

#include "LibFileBrowser.h"
#include "Lib/LibDefines.h"
#include <sys/vfs.h>
#include <errno.h>

#define szFATFS_DEI (4+4+4+(2 * FATFS_MAX_PATHNAME_LENGTH)+(2 * FATFS_MAX_PATHNAME_LENGTH)+(1*16)+4+1+1+1+1+4+2+2+2+2+2+2)

CPopUpMenu* pmFileBrowser;
CPopUpSubMenuItem* psSort;

extern TTAVI_GLOBAL_CONFIG* pTAVI;
extern BITMAPFILEINFO256 TAVI_IMAGES[];
extern MpegDecoder *pMpegDecoder;
extern CTextEngine_BDF* pT;
extern CTopPanel*	g_TopPanel;
extern CControlPannel* 	pCp;
extern int errno;

#define MIN(X,Y) (((X)<=(Y))?(X):(Y))	// to find the min between two number.

#define _FB_ROW_HEIGHT								22
#define _FB_SELECTBAR_START_X						19
#define _FB_SELECTBAR_START_Y						34
#define _FB_SELECTBAR_HEIGHT						22
#define _FB_MAX_DISPLAY_SLOT						7 
#define _FB_MAX_DISPLAY_INDEX						8
#define SCROLLBAR_SPACE							15
#define _FB_SELECTED_ITEM_BAR_COLOR				17
#define _FB_SELECTED_ITEM_BAR_TRANSPARENCY_COLOR	27
#define _FB_SELECTED_ITEM_BAR_SHADOW_COLOR		31

static unsigned int htoi(const char *str)
{
	const char *cp;
	unsigned int data, bdata;

	for (cp = str, data = 0; *cp != 0; ++cp) {
		if (*cp >= '0' && *cp <= '9')
			bdata = *cp - '0';
		else if (*cp >= 'A' && *cp <= 'F')
			bdata = *cp - 'A' + 10;
		else if (*cp >= 'a' && *cp <= 'f')
			bdata = *cp - 'a' + 10;
		else
			break;
		data = (data << 4) | bdata;
	}

	return data;
}

static int* load_acc_info( uchar_t* path, int* nfiles )
{
  RMuint32 fd;
  FATFS_ERROR err;
  uchar_t dbpath[MAX_PATH];
  uchar_t tmp[20];
  int* crcs;

  ustrcpy( dbpath, path );
  ustrcat( dbpath, (uchar_t*)"/\0\0\0" );
  str2ustr( tmp, _VIDEOPLAYER_BOOKMARK_DATA );
  ustrcat( dbpath, tmp );

  err = fatfs_wdopen( dbpath, &fd );
  if( err ) {
    printf( "err: 0x%x\n", err );
    return NULL;
  }

  FATFS_DIR_ENTRY_INFO dinfo;
  int crc;
  int i=0;
  char name[9];
  fatfs_dget_numentries( fd, NULL, NULL, (RMuint32*)nfiles, NULL, NULL );
  *nfiles += 2; // add '.' and '..'
  crcs = new int[*nfiles];
  while( 1 ) {
    err = fatfs_dread(fd, &dinfo);
    if( err ) {
      break;
    }
    if( dinfo.attributes&ATTR_DIRECTORY ) {
      continue;
    }
    strncpy( name, dinfo.short_name, 8 );
    name[8] = 0;
    crcs[i++] = htoi(name);
  }
  fatfs_dclose( fd );

  return crcs;
}


bool fpNameAscentSort( FILE_NODE* pNewItem, FILE_NODE* pOldItem )
{
	uchar_t* NewName = (uchar_t*)&(pNewItem->D.name);
	uchar_t* OldName = (uchar_t*)&(pOldItem->D.name);
	int szNew = ustrlen( NewName );
	int szOld = ustrlen( OldName );

	TaviSysParam.sort = Sort_Type_Name_Ascent;

	for(int i=0; i<MIN(szNew, szOld); i++ )
		{
		if( NewName[i] > OldName[i] ) 
			return true;
		else if( NewName[i] < OldName[i] ) 
			return false;
		}
	return false;
}
bool fpNameDescentSort( FILE_NODE* pNewItem, FILE_NODE* pOldItem )
{
	uchar_t* NewName = (uchar_t*)&(pNewItem->D.name);
	uchar_t* OldName = (uchar_t*)&(pOldItem->D.name);
	int szNew = ustrlen( NewName );
	int szOld = ustrlen( OldName );

	TaviSysParam.sort = Sort_Type_Name_Descent;

	for(int i=0; i<MIN(szNew, szOld); i++ )
		{
		if( NewName[i] < OldName[i] ) 
			return true;
		else if( NewName[i] > OldName[i] ) 
			return false;
		}
	return false;
}
bool fpTimeAscentSort( FILE_NODE* pNewItem, FILE_NODE* pOldItem )
{
	RMuint16	NewTime, NewDate, OldTime, OldDate;

	NewTime = pNewItem->D.file_write_time;
	NewDate = pNewItem->D.file_write_date;
	OldTime = pOldItem->D.file_write_time;
	OldDate = pOldItem->D.file_write_date;

	TaviSysParam.sort = Sort_Type_Date_Ascent;

	if( NewDate == OldDate )
		{
		if( NewTime < OldTime ) 
			return TRUE;
		else 
			return FALSE;
		}
	else if( NewDate < OldDate )
		return TRUE;
	else
		return FALSE;
}
bool fpTimeDescentSort( FILE_NODE* pNewItem, FILE_NODE* pOldItem )
{
	RMuint16	NewTime, NewDate, OldTime, OldDate;

	NewTime = pNewItem->D.file_write_time;
	NewDate = pNewItem->D.file_write_date;
	OldTime = pOldItem->D.file_write_time;
	OldDate = pOldItem->D.file_write_date;

	TaviSysParam.sort = Sort_Type_Date_Descent;

	if( NewDate == OldDate )
		{
		if( NewTime > OldTime ) 
			return TRUE;
		else 
			return FALSE;
		}
	else if( NewDate > OldDate )
		return TRUE;
	else
		return FALSE;
}

CFileBrowser::CFileBrowser( char sDbxTitle[], int x, int y, int w, int h, MSG_PROCESSOR pfpMsgProc, U32 option  )
{
	DEBUGMSG(1,("CFileBrowser( [%s], %d, %d, %d, %d, ... , %d)\n", sDbxTitle, x, y, w, h, option ));

	Stop_VideoControlPannelTimer();
	
	SelectedAbsoluteIndex	= 0;	// Cursor
	AbsoluteIndex			= 0;	// 0 ~ LASTINDEX
	FirstShownIndex		= 0;	// Absolute Index			
	LastIndex			= 0;	// Absolute Index
	LastSlot 				= 0;	
	sx 					= 18;
	sy 					= 60;
	width 				= 320-(18*2);
	height 				= h;		// not needed
	iVisibleStatus			= FALSE;
	fpDialogBoxMsgProc 	= pfpMsgProc;
	eActiveMode 			= (eFILEBROWSER_ACTIVE_MODE)option;
	DirectoryLevel			= 0;
	pDlg					= NULL;
	StaticTitle			= (uchar_t*)malloc(sizeof(uchar_t)*260);
	DynamicTitle			= (uchar_t*)malloc(sizeof(uchar_t)*260);
	CurrentFullPathDir		= (uchar_t*)malloc(sizeof(uchar_t)*260);
	SrcPathname			= (uchar_t*)malloc(sizeof(uchar_t)*260);
	RootDir				= (uchar_t*)malloc(sizeof(uchar_t)*260);
	ParentDir				= (uchar_t*)malloc(sizeof(uchar_t)*260);
	Filename				= (uchar_t*)malloc(sizeof(uchar_t)*260);
	TaviCastDir			= (uchar_t*)malloc(sizeof(uchar_t)*260);
	CurrentFileExt			= (char*)malloc(sizeof(char)*32);
	CurrentFileSystem		= FATFS;
	DirRepeatStartPoint	= 0;

	sprintf( CurrentFileExt, "%s", "_db" );
	str2ustr(RootDir, CurrentFileExt );
	crcDbDir				= crc32( (char*)RootDir, ustrlen(RootDir)*2 );

	cTemp = (char*)malloc(sizeof(char)*120);

	sprintf( cTemp, "%s", "System Volume Information" );
	str2ustr( RootDir, cTemp );
	crcSystemVolumeInformationDir = crc32((char*)RootDir, ustrlen((uchar_t*)RootDir)*2 );

	sprintf( cTemp, "%s", "BATTERY.DEBUG" );
	str2ustr( RootDir, cTemp );
	crcBatteryDebugEnableDir = crc32((char*)RootDir, ustrlen((uchar_t*)RootDir)*2 );

	free( cTemp );
	memset( CurrentFileExt, 0, 32 );
	memset( RootDir, 0, sizeof(uchar_t)*260 );
	
	str2ustr( TaviCastDir, _TAVICAST_DIR, strlen(_TAVICAST_DIR) );

	SetSortingMethod( fpNameAscentSort );

	pMbx				= new CMsgBox();

	pSb1 				= new CScrollBar2( 
							_FB_SELECTBAR_START_X + 320-(18*2) + ((18/2)-(8/2)) -5, 
							_FB_SELECTBAR_START_Y, 
							_FB_SELECTBAR_HEIGHT * _FB_MAX_DISPLAY_INDEX, 
							LastIndex + 1,
							FirstShownIndex,
							MIN( (LastIndex - FirstShownIndex + 1), _FB_MAX_DISPLAY_INDEX )
							);

	nfiles = 0;
	enjoyed = NULL;
}

CFileBrowser::~CFileBrowser()
{
	delete pSb1;	pSb1 = NULL;
	delete pMbx; pMbx = NULL;
	
	free(CurrentFileExt); CurrentFileExt=NULL;
	free(Filename); Filename=NULL;
	free(ParentDir); ParentDir=NULL;
	free(RootDir); RootDir=NULL;
	free(SrcPathname); SrcPathname = NULL;
	free(CurrentFullPathDir); CurrentFullPathDir=NULL;
	free(DynamicTitle); DynamicTitle=NULL;
	free(StaticTitle); StaticTitle=NULL;
	free(StaticTitle); TaviCastDir=NULL;

	SubtitleCrcList.FreeAll();
	ItemList.FreeAll();
	ItemList_Files.FreeAll();
}


int CFileBrowser::AddFileItem_UC16( int idx, FILE_NODE* pN,  eCURRENT_DIR_FS_TYPE filesystem )
{
	FILE_NODE* pNode = new FILE_NODE;
	memcpy( pNode, pN, sizeof(FILE_NODE) );
	pNode->isSelected = FALSE;
	pNode->filesystem = filesystem;
	ItemList.Add( pNode );
	LastSlot++;
	return TRUE;
}

int CFileBrowser::AddFileItem_UC16( int idx, uchar_t name[], int filesize, eFILE_TYPE filetype, char ext[] )
{
	FILE_NODE* pNode = new FILE_NODE;
	ustrcpy( (uchar_t*)pNode->D.name, name );
	pNode->D.file_size			= filesize;
	pNode->filetype			= filetype;
	pNode->isSelected 		= FALSE;
	ItemList.AddHead( pNode );
	LastSlot++;
	return TRUE;
}
int CFileBrowser::AddFileItem_UC16_OnTop( int idx, uchar_t name[], int filesize, eFILE_TYPE filetype, char ext[], eCURRENT_DIR_FS_TYPE filesystem )
{
	FILE_NODE* pNode = new FILE_NODE;
	pNode->isSelected = FALSE;
	ustrcpy( (uchar_t*)pNode->D.name, name );
	pNode->D.file_size			= filesize;
	pNode->filetype			= filetype;
	pNode->filesystem			= filesystem;
	sprintf( pNode->ext, "%s", ext );
	ItemList.AddHead( pNode );
	LastSlot++;
	return TRUE;
}
int CFileBrowser::AddFileItem_UTF8( int idx, char name[], int filesize, eFILE_TYPE filetype, char ext[], eCURRENT_DIR_FS_TYPE filesystem )
{
	FILE_NODE* pNode = new FILE_NODE;
	pNode->isSelected = FALSE;
	strcpy( (char *)pNode->D.name, name );
	pNode->D.file_size			= filesize;
	pNode->filetype			= filetype;
	pNode->filesystem			= filesystem;
	sprintf( pNode->ext, "%s", ext );
	ItemList.AddHead( pNode );
	LastSlot++;
	return TRUE;
}

int CFileBrowser::AddFileItem_UC16( int idx, FATFS_DIR_ENTRY_INFO* pD, eFILE_TYPE filetype, char ext[] )
{
	FILE_NODE* pNode = new FILE_NODE;
	pNode->isSelected = FALSE;
	memcpy( &(pNode->D), pD, sizeof(FATFS_DIR_ENTRY_INFO) );
	pNode->filetype			= filetype;
	sprintf( pNode->ext, "%s", ext );
	ItemList.Add( pNode );
	LastSlot++;
	return TRUE;
}

int 
CFileBrowser::AddFileItem_UC16_WithSorting( 
    int idx, 
    FATFS_DIR_ENTRY_INFO* pD, 
    eFILE_TYPE filetype, 
    char ext[], 
    eCURRENT_DIR_FS_TYPE filesystem )
{
	FILE_NODE* pNode = new FILE_NODE;
	int crc;
	int find;

	memcpy( &(pNode->D), pD, sizeof(FATFS_DIR_ENTRY_INFO) );

	pNode->filetype			= filetype;
	pNode->isSelected 		= FALSE;
	sprintf( pNode->ext, "%s", ext );
	pNode->filesystem			= filesystem;

	if( filetype == _DIRECTORY || filetype == _DIRECTORY_LINK   )
		{
		return ( ItemList.AddWithSort( pNode, CurrentSortingMethod) );
		}
	else
		{
		return ( ItemList_Files.AddWithSort( pNode, CurrentSortingMethod) );
		}

	
}
int CFileBrowser::AddFileItem_UC16_OnTop( int idx, FATFS_DIR_ENTRY_INFO* pD, eFILE_TYPE filetype, char ext[], eCURRENT_DIR_FS_TYPE filesystem )
{
	FILE_NODE* pNode = new FILE_NODE;

	pNode->isSelected 		= FALSE;
	memcpy( &(pNode->D), pD, sizeof(FATFS_DIR_ENTRY_INFO) );
	pNode->filetype			= filetype;
	pNode->filesystem			= filesystem;
	sprintf( pNode->ext, "%s", ext );
	ItemList.AddHead( pNode );
	LastSlot++;
	return TRUE;	
}

int CFileBrowser::AddSubtitleCRC( int CRC )
{
	CRC_NODE* pNode = new CRC_NODE;
	pNode->CRC = CRC;
	return ( SubtitleCrcList.Add( pNode ) );
}

int CFileBrowser::IsThisLinuxFs()
{
	uchar_t u16Tmp[10];
	str2ustr(u16Tmp, "/usb0");
	
	if( ustrncmp( CurrentFullPathDir, u16Tmp, 5 ) ==  0 )
		return TRUE;
	else
		return FALSE;
}
int CFileBrowser::IsThisLinuxFs_Root()
{
	if( IsThisLinuxFs() && ustrlen( CurrentFullPathDir )==5 )
		return TRUE;
	else
		return FALSE;
}

int CFileBrowser::IsThisLinuxFs(uchar_t* u16String )
{
	if( u16String[0] == (uchar_t) '/'  &&
		u16String[1] == (uchar_t) 'u'  &&
		u16String[2] == (uchar_t) 's'  &&
		u16String[3] == (uchar_t) 'b' &&
		u16String[4] == (uchar_t) '0'
		)
		return TRUE;
	else
		return FALSE;
}
int CFileBrowser::IsThisFatFs()
{
	if( CurrentFullPathDir[0] == (uchar_t) '/'  &&
		CurrentFullPathDir[1] == (uchar_t) 'h'  &&
		CurrentFullPathDir[2] == (uchar_t) 'd'  &&
		CurrentFullPathDir[3] == (uchar_t) '1'  )
		return TRUE;
	else
		return FALSE;
}

int CFileBrowser::IsThisFatFs_Root()
{
	if( IsThisFatFs() && ustrlen(CurrentFullPathDir)==4 )
		return TRUE;
	else
		return FALSE;
}



void CFileBrowser::SetSortingMethod( SORTING_METHOD fpNewMethod )
{
	CurrentSortingMethod = fpNewMethod;
}

SORTING_METHOD CFileBrowser::GetCurrentSortingMethod( )
{
	return CurrentSortingMethod;
}



void CFileBrowser::RefreshForSortingMethodChange(SORTING_METHOD fpNewMethod, eOSD_REFRESH_OPTION option)
{
	if( fpNewMethod != NULL )
		SetSortingMethod( fpNewMethod );

	ClearFileItem( _SKIP_CURSOR );

	if(IsThisFatFs() )
		ScanDir( DynamicTitle, -1 );
	else
		ScanDir_linuxfs( CurrentFullPathDir );

	if( option == SKIP_OSD_REFRESH )
		ReDraw(SKIP_OSD_REFRESH);
	else
		ReDraw(OSD_REFRESH);
}

int CFileBrowser::ClearFileItem( U32 option )
{
	// Indexes
	Stop_FileBrowserTimer();
	ItemList.FreeAll();
	ItemList_Files.FreeAll();
	choice				= 0;
	LastSlot 				= 0;	
	SelectedAbsoluteIndex	= 0;	// Cursor
	AbsoluteIndex			= 0;	// 0 ~ LASTINDEX
	RelativeIndex			= 0;
	FirstShownIndex		= 0;	// Absolute Index			
	LastIndex			= 0;	// Absolute Index

	return TRUE;
}

RMuint8 CFileBrowser::Convert1digitBCDCharToHexCode( RMuint8 BCDChar)
{
	// ex) "A" -> 0x0A

	if( BCDChar >= 0x30 && BCDChar < 0x40 ) return (BCDChar - 0x30);
	else if( BCDChar >= 0x41 && BCDChar < 0x47 ) return (BCDChar - 0x37);
	else if( BCDChar >= 0x61 && BCDChar < 0x67 ) return (BCDChar - 0x57);
	return 0;
}

RMuint16 CFileBrowser::Convert4digitBCDStringToHexCode( RMuint8 * BCDString  )
{
	// ex) "AC00" -> 0xAC00
	return ( (Convert1digitBCDCharToHexCode(BCDString[0])<<12) |
			(Convert1digitBCDCharToHexCode(BCDString[1])<<8) |
			(Convert1digitBCDCharToHexCode(BCDString[2])<<4) |
			 Convert1digitBCDCharToHexCode(BCDString[3])  );
}

RMuint16 CFileBrowser::ConvertVFATTextToUnicode16Str( uchar_t* uc16String, char* euc8String )
{
	int p_uc;
	int p_euc;
	int ssLengthOfSourceText = strlen( euc8String );
	U16 aus2byteTextArray[256];

	for( p_euc=0, p_uc=0; p_euc< ssLengthOfSourceText;  )
		{

		if( euc8String[p_euc] == ':' )
			{
			p_euc++;
			uc16String[p_uc] = (uchar_t)Convert4digitBCDStringToHexCode( (RMuint8*)(euc8String+p_euc) );
			p_euc = p_euc+4;
			p_uc++;
			}

		else
			{
			uc16String[p_uc] = (uchar_t)euc8String[p_euc];
			p_euc++;
			p_uc++;
			}

		}
	
	uc16String[p_uc] = 0x0000;
	return 0x0020;
}

RMuint16 CFileBrowser::ConvertUnicodeToUniXlate( char* euc8String, uchar_t* u16String )
{
	uchar_t ch;
	int 	i=0;
	int	j=0;
	
	if( !u16String || !euc8String ) return 0;	
	while( (ch=u16String[i++]) ) {
		if( ch == 0 ) {
			break;
			}
		else if ( ch < 0x0100 ) {
			euc8String[j++]= ch;
			}
		else {
			sprintf( (euc8String+j), ":%04x", ch );
			j+=5;
			}
		}
	euc8String[j]=0;
}

/* Find out if the last character of a string matches the one given Don't
 * underrun the buffer if the string length is 0.  Also avoids a possible
 * space-hogging inline of strlen() per usage.
 */
char * last_char_is(const char *s, int c)
{  
    char *sret;
    if (!s)
        return NULL;
    sret  = (char *)s+strlen(s)-1;
    if (sret>=s && *sret == c) {
        return sret;
    } else {
        return NULL;
    }
}
char *concat_path_file(const char *path, const char *filename)
{
    char *outbuf;
//	char outbuf[260*2];
    char *lc;
    
    if (!path) 
        path="";
    lc = last_char_is(path, '/');
    while (*filename == '/')
        filename++;
    outbuf = (char *)malloc(strlen(path)+strlen(filename)+1+(lc==NULL));
    sprintf(outbuf, "%s%s%s", path, (lc==NULL)? "/" : "", filename);
   
    return outbuf;
}

int CFileBrowser::ExistTaviCastOnParentDir( uchar_t *puDirName, FATFS_DIR_ENTRY_INFO *pDirInfo4TaviCast )
{
	uchar_t uParentDir[260];
	uchar_t uTaviCast[10];
	RMuint32	fd;
	FATFS_ERROR err;
	int nReturn = -1;

//	print_ustr( puDirName, "++++ IsExistTaviCast :: puDirName = ");
//	print_ustr( RootDir, "++++ IsExistTaviCast :: RootDir = ");
//	print_ustr( CurrentFullPathDir, "++++ IsExistTaviCast :: CurrentFullPathDir = ");

	GetParentDir_In_UC16FullDirString(uParentDir, TaviCastDir );
	GetCurrentDir_InFullDirString_UC16(uTaviCast, TaviCastDir );
 
	if( eActiveMode == _FBMODE_FILEMANAGER) {
		return -1;
		}
	
	if ( ustrcmp( puDirName, RootDir) ==0 ) {
 		if (( err= fatfs_wdopen ((RMuint16 *)uParentDir, &fd) ) > FATFS_ERROR_NO_ERROR) {
//			printf("\n**** IsExistTaviCast :: fatfs_wdopen err=0x%x\n", err);	// for debug
			nReturn =-2;
			}

		while( err==FATFS_ERROR_NO_ERROR ) {
			err = fatfs_dread (fd, pDirInfo4TaviCast);
			if (err != FATFS_ERROR_NO_ERROR)	{
//				printf("\n**** IsExistTaviCast :: fatfs_dread err=0x%x\n", err);	// for debug
				nReturn =-3;
				}
			if( (pDirInfo4TaviCast->attributes & ATTR_DIRECTORY) &&
				(ustrcmp((uchar_t*)pDirInfo4TaviCast->name, uTaviCast )==0) ) {
				// find 'TAVICAST' folder in root /hd1
				ustrcpy ( (uchar_t *)pDirInfo4TaviCast->parentdir, uParentDir );
//				printf("\n**** IsExistTaviCast :: find TAVICAST folder\n");		// for debug
				nReturn = 0;
				break;
				}
//			print_ustr( (uchar_t *)pDirInfo4TaviCast->name, "*** IsExistTaviCast :: name =");
			}
		fatfs_dclose(fd);
		}
	else {
		nReturn = -1;
		}
	return nReturn;

}


/* tiny recursive du */
int du_depth = 0;
int one_file_system = 0;
int count_hardlinks = 0;
dev_t dir_dev = 0;

RMuint64 CFileBrowser::du_linuxfs(char *filename, FOLDER_INFO *pTFI )
{
	struct stat statbuf;
	RMuint64 sum;


	if ((lstat(filename, &statbuf)) != 0) {
		printf("err. can't open file =%s", filename);
		return 0;
	}
	if (du_depth == 0)
		dir_dev = statbuf.st_dev;
	else if (one_file_system && dir_dev != statbuf.st_dev)
		return 0;

	du_depth++;
	sum = (statbuf.st_blocks >> 1);

	/* Don't add in stuff pointed to by symbolic links */
	if (S_ISLNK(statbuf.st_mode)) {
		sum = 0L;
		if (du_depth == 1) {
			}
		}
	if (S_ISDIR(statbuf.st_mode)) {
		DIR *dir;
		struct dirent *entry;
		char *newfile;

		dir = opendir(filename);
		if (!dir) {
			du_depth--;
			return 0;
			}
		pTFI->FolderItemCount ++;
		sum = 0L;

		printf("--- 10th :: du : open [%s], sum=%d\n", filename, sum ); // for debug

		newfile = last_char_is(filename, '/');
		if (newfile) *newfile = '\0'; 

		while ((entry = readdir(dir))) {
			char *name = entry->d_name;

			if ((strcmp(name, "..") == 0) || (strcmp(name, ".") == 0)) {
				continue;
				}
			newfile = concat_path_file(filename, name);
			sum += du_linuxfs(newfile, pTFI);
			free(newfile);
			}

		closedir(dir);
//		printf("--- 13th :: du : close[%s]\n\n", filename );
		}

	if( S_ISREG(statbuf.st_mode) ) {
		pTFI->FolderItemCount ++;
		printf("--- 14th :: du : file [%s], size=%d, FolderItemCount=%d\n", filename, sum, pTFI->FolderItemCount );	// for debug
		}

	du_depth--;

	return sum;
}
RMuint64 CFileBrowser::df_linux(char *filename)
{
	struct statfs stbuf;

	if (statfs(filename, &stbuf) == -1) {
		printf("Unable to get disk space of %s: %d\n", filename, errno);
		return 0;
	}
	return  (RMuint64)stbuf.f_bsize * (RMuint64)stbuf.f_bfree;
}
int CFileBrowser::ScanDir_linuxfs( uchar_t ufullpathDir[] )
{
	DIR* dp = NULL;
	struct dirent* items;
	struct stat fstat;
	char fullpath[260*2];
	char parent[260*2];
	uchar_t u16name[260];
	static FATFS_DIR_ENTRY_INFO D;
	int flags, pickup, idx;

	DEBUGMSG(1,("ScanDir_linuxfs(...)\n" ));
	DEBUGMSG(1,("-fullpathDir : [%s]\n", ufullpathDir ));

	D.handle=0;
	D.flags=0;
	D.first_cluster=0;
	ustrcpy( (uchar_t *)D.parentdir, ufullpathDir );
//	sprintf((char*)D.parentdir, "%s", fullpathDir );
	D.name[0] = '\0';								// file or directory name
	D.short_name[0] = '\0';
	D.namelength=0;
	D.attributes;
	D.attrib_ln;
	D.file_creation_time_tenth;
	D.b_reserved_0;
	D.file_size=0;
	D.file_creation_time;
	D.file_creation_date;
	D.file_write_time;
	D.file_write_date;
	D.file_last_access_date;
	D.w_reserved_0 = 0;

	pickup = 0;
	idx = 1;

	//sprintf( (char*)CurrentFullPathDir, "%s", fullpathDir );
	ustrcpy( CurrentFullPathDir, ufullpathDir );
	ConvertUnicodeToUniXlate(parent, ufullpathDir);	
	
	printf("-- 35th :: scan_linux : CurrentFullPathDir [%s]\n", parent );	// for debug

	dp  = opendir( parent );
	if ( dp == NULL ) return FALSE;

	DEBUGMSG(1,("-fullpathDir : [%s]\n", ufullpathDir ));
	DEBUGMSG(1,("-(char*)CurrentFullPathDir : [%s]\n", (char*)CurrentFullPathDir ));

	while( (items=readdir(dp)) != NULL )
		{
		if( items->d_name[0] == '.' ) continue;
		sprintf( fullpath, "%s/%s", parent,  items->d_name );

		DEBUGMSG(1,("-fullpath : [%s]\n", fullpath ));
		printf("-- 35th :: scan_linux : readdir, fullpath[%s]\n", fullpath ); 	// for debug
		
		if( lstat( fullpath, &fstat ) < 0 ) 
			{
			DEBUGMSG(1,( "failed to lstat( %s )\n", fullpath));
			continue;
			}

		if( S_ISREG(fstat.st_mode) ) 
			{
//			printf("(FILE)%s\n", fullpath);
//			sprintf( (char*)D.name, "%s", (char*)items->d_name );
			ConvertVFATTextToUnicode16Str( (uchar_t *)D.name, (char*)items->d_name);
			D.namelength = ustrlen((uchar_t *)D.name);
			D.attributes = ATTR_FILE;
			D.file_size = (RMuint32)fstat.st_size;

			eFILE_TYPE filetype = (eFILE_TYPE)CheckExtension(  (char*)items->d_name  );
			flags = 0;

			if( eActiveMode == _FBMODE_AUDIOPLAYER && filetype == _AUDIO_FILE ) flags = 1;
			else if( eActiveMode == _FBMODE_VIDEOPLAYER && filetype == _VIDEO_FILE ) flags = 1;
			else if( eActiveMode == _FBMODE_VIDEOPLAYER && filetype == _SUBTITLE_FILE) flags = 1;
			else if( eActiveMode == _FBMODE_IMAGEVIEWER && filetype == _IMAGE_FILE ) flags = 1;
			else if( eActiveMode == _FBMODE_TEXTVIEWER && filetype == _TEXT_FILE ) flags = 1;
			else if( eActiveMode == _FBMODE_FILEMANAGER) flags = 1;

			if( flags == 1 )
				{
				char ext[32];
				
				switch( CheckVideoExtension( (char*)D.name ) )
					{
					case 3:
						sprintf( ext, "%s", "avi" );
		          			AddFileItem_UC16_WithSorting( pickup,  &D, filetype, ext, LINUXFS );
						pickup++;
						break;

					case 4:
						sprintf( ext, "%s", "mp4" );
		          			AddFileItem_UC16_WithSorting( pickup,  &D, filetype, ext, LINUXFS );
						pickup++;
						break;

					case 5:
						sprintf( ext, "%s", "mpg" );
			      			AddFileItem_UC16_WithSorting( pickup,  &D, filetype, ext, LINUXFS );
						pickup++;
						break;

					case 31:
						sprintf( ext, "%s", "smi" );
						AddSubtitleCRC(crc32((char*)D.name, (ustrlen((uchar_t*)D.name)-4)*2) );
						if( eActiveMode == _FBMODE_FILEMANAGER)
							{
				      			AddFileItem_UC16_WithSorting( pickup,  &D, filetype, ext, LINUXFS );
							pickup++;
							}
						break;
						
					case 32:
						sprintf( ext, "%s", "smil" );
						AddSubtitleCRC(crc32((char*)D.name, (ustrlen((uchar_t*)D.name)-5)*2) );
						if( eActiveMode == _FBMODE_FILEMANAGER)
							{
								AddFileItem_UC16_WithSorting( pickup,  &D, filetype, ext, LINUXFS );
							pickup++;
							}
						break;
						
					default:
						if( eActiveMode == _FBMODE_FILEMANAGER)
							{
				      			AddFileItem_UC16_WithSorting( pickup,  &D, filetype, ext, LINUXFS );
							pickup++;
							}
						break;

					}
						
				}


			}

		else if( S_ISDIR(fstat.st_mode) ) 
			{
//			printf("(DIR)%s\n", fullpath);
//			sprintf( (char*)D.name, "%s", (char*)items->d_name );
			ConvertVFATTextToUnicode16Str( (uchar_t *)D.name, (char*)items->d_name);
			D.namelength = ustrlen((uchar_t *)D.name);
			D.attributes = ATTR_DIRECTORY;
			AddFileItem_UC16_WithSorting( 0 , &D, _DIRECTORY, "dir", LINUXFS );
			idx++;
			}
		else 
			continue;		
		

		}

	for(int af=0; af<pickup; af++ )
		{
		AddFileItem_UC16( idx, ItemList_Files[af], LINUXFS );
		idx++;
		}

	closedir(dp);

	return TRUE;

}
RMuint64 CFileBrowser::df_fatfs( RMint32 NoVol)
{
	FATFS_ERROR err;
	RMuint64	u64DiskFreeSizeInBytes;

	err = fatfs_get_vol_free_size(NoVol, &u64DiskFreeSizeInBytes );
	if ( err > FATFS_ERROR_NO_ERROR ) {
		printf("=== df_fatfs :: err=0x%x\n", err);
		}
	return u64DiskFreeSizeInBytes;
}
int CFileBrowser::ScanDir( uchar_t targetdir[], int option )
{
	FATFS_ERROR err;
	FATFS_DIR_ENTRY_INFO dirinfo;
	FATFS_DIR_ENTRY_INFO DirInfo4TaviCast;

	int pickup = 0;
	int flags;
	int i=0;
	char cTemp[260];
	uchar_t ucTemp[260];
	char ext[32];	

	RMuint32 tempCRC;
	int nExistTaviCast=-1;

  if( enjoyed ) {
    delete[] enjoyed;
  }
  enjoyed = NULL;
  nfiles = 0;
  
	if( option == 0 ) { // Root Directory Open (targetdir is FullPath)
	  enjoyed = load_acc_info( RootDir, &nfiles );
		err = fatfs_wdopen ((RMuint16*)RootDir, &currdir_fd);
		ASSERT (err == FATFS_ERROR_NO_ERROR);
		if (err != FATFS_ERROR_NO_ERROR)
			printf("- fatfs error: [%x] : [%s]\n", (int)err, GetErrMsg(cTemp, (int)err) );
		
		ustrcpy (CurrentFullPathDir, RootDir);
	}
	else if( option == 1 ) {// Non-Root Directory (targetdir is Directory Name Only )
		ustrcpy( ucTemp, CurrentFullPathDir );
		ustrcat( ucTemp, (uchar_t*)"/" );
		ustrcat( ucTemp, targetdir );

    enjoyed = load_acc_info( ucTemp, &nfiles );
		if (fatfs_wdopen ((RMuint16 *)ucTemp, &currdir_fd) == FATFS_ERROR_NO_ERROR) {
			ustrcpy (CurrentFullPathDir, ucTemp);			
		}
		else {
		  enjoyed = load_acc_info( CurrentFullPathDir, &nfiles );
			err = fatfs_wdopen ((RMuint16 *)CurrentFullPathDir, &currdir_fd);
		} 
	}
	else if( option == 2 ) { // Fullpath Open
	  enjoyed = load_acc_info( targetdir, &nfiles );
		if (fatfs_wdopen((RMuint16 *)targetdir, &currdir_fd) == FATFS_ERROR_NO_ERROR) {
			ustrcpy( CurrentFullPathDir, targetdir );
			GetCurrentDir_InFullDirString_UC16(DynamicTitle, CurrentFullPathDir);
		}
		else {
		  enjoyed = load_acc_info( RootDir, &nfiles );
			err = fatfs_wdopen ((RMuint16*)RootDir, &currdir_fd);
			if (err != FATFS_ERROR_NO_ERROR)
				printf("- fatfs error: [%x] : [%s]\n", (int)err, GetErrMsg(cTemp, (int)err) );
			
			ustrcpy (CurrentFullPathDir, RootDir);
		}
	}
	else if( option == -1 )	{ // Non-Root Directory cd ..
	  enjoyed = load_acc_info( CurrentFullPathDir, &nfiles );
		if (fatfs_wdopen (CurrentFullPathDir, &currdir_fd) == FATFS_ERROR_NO_ERROR)
			{
			GetLastItem_In_UC16FullDirString_WithUserDefineSplitter(DynamicTitle, CurrentFullPathDir, 0x002f );
		}
		else {
			err = fatfs_wdopen(CurrentFullPathDir, &currdir_fd);
		}
	}


	pickup = 0;
	i = 1;

	while (pickup<1000)
		{

		RELOAD_WATCHDOGTIMER;
		err = fatfs_dread (currdir_fd, &dirinfo);
		if (err != FATFS_ERROR_NO_ERROR)
			{
			break;
			}

		if( dirinfo.name[0] == 0x002e ) continue;
		if (dirinfo.attributes & ATTR_VOLUME_ID) continue;
		if (dirinfo.attributes & 0x10)
			{

			if( eActiveMode == _FBMODE_FILEMANAGER )
				{
				if( (tempCRC=crc32((char*)dirinfo.name, ustrlen((uchar_t*)dirinfo.name)*2 )) != crcSystemVolumeInformationDir )
					{
					AddFileItem_UC16_WithSorting( i , &dirinfo, _DIRECTORY, "dir", FATFS );
					i++;
					}
				if( tempCRC == crcBatteryDebugEnableDir )
					{
					pTAVI->IsBatteryDebugMode = TRUE;
					}		
				}
			else
				{
				if( (tempCRC=crc32((char*)dirinfo.name, ustrlen((uchar_t*)dirinfo.name)*2 )) != crcDbDir )
					{
					AddFileItem_UC16_WithSorting( i , &dirinfo, _DIRECTORY, "dir", FATFS );
					i++;
					}
		
				}
			}
		else
			{
			eFILE_TYPE filetype = (eFILE_TYPE)CheckExtension_UC16(  (uchar_t*)dirinfo.name );
			flags = 0;

			if( eActiveMode == _FBMODE_AUDIOPLAYER && filetype == _AUDIO_FILE ) flags = 1;
			else if( eActiveMode == _FBMODE_ALARM_SETTTING && filetype == _AUDIO_FILE ) flags = 3;
			else if( eActiveMode == _FBMODE_VIDEOPLAYER && filetype == _VIDEO_FILE ) flags = 1;
			else if( eActiveMode == _FBMODE_VIDEOPLAYER && filetype == _SUBTITLE_FILE) flags = 1;
			else if( eActiveMode == _FBMODE_IMAGEVIEWER && filetype == _IMAGE_FILE ) flags = 1;
			else if( eActiveMode == _FBMODE_TEXTVIEWER ) flags = 2;
			else if( eActiveMode == _FBMODE_FILEMANAGER) flags = 1;
/*
			_FBMODE_FILEMANAGER : Show All
			_FBMODE_VIDEOPLAYER : Video Files
			_FBMODE_TEXTVIEWER :  Except Known Extensions 
*/

      tempCRC = crc32((char*)dirinfo.name, ustrlen((uchar_t*)dirinfo.name)*2 );
      
			// FileManager, VideoPlayer
			if( flags == 1 )
				{
				switch( CheckVideoExtension_UC16( (uchar_t*)dirinfo.name ) )
					{
					case 3:
						sprintf( ext, "%s", "avi" );
      			AddFileItem_UC16_WithSorting( pickup,  &dirinfo, filetype, ext, FATFS );
						pickup++;
						break;

					case 4:
						sprintf( ext, "%s", "mp4" );
      			AddFileItem_UC16_WithSorting( pickup,  &dirinfo, filetype, ext, FATFS );
						pickup++;
						break;

					case 5:
						sprintf( ext, "%s", "mpg" );
      			AddFileItem_UC16_WithSorting( pickup,  &dirinfo, filetype, ext, FATFS );
						pickup++;
						break;

					case 31:
						sprintf( ext, "%s", "smi" );
						AddSubtitleCRC(crc32((char*)dirinfo.name, (ustrlen((uchar_t*)dirinfo.name)-4)*2) );
						if( eActiveMode == _FBMODE_FILEMANAGER )
						{
	      			AddFileItem_UC16_WithSorting( pickup,  &dirinfo, filetype, ext, FATFS );
  						pickup++;
						}
						break;
						
					case 32:
						sprintf( ext, "%s", "smil" );
						AddSubtitleCRC(crc32((char*)dirinfo.name, (ustrlen((uchar_t*)dirinfo.name)-5)*2) );
						if( eActiveMode == _FBMODE_FILEMANAGER ) {
							AddFileItem_UC16_WithSorting( pickup,  &dirinfo, filetype, ext, FATFS );
							pickup++;
						}
						break;
						
					default:
						if( eActiveMode == _FBMODE_FILEMANAGER )
							{
				      			AddFileItem_UC16_WithSorting( pickup,  &dirinfo, filetype, ext, FATFS );
							pickup++;
							}
						break;

					}
						
				}

			// TextViewer
			else if( flags == 2 &&  ( filetype!=_AUDIO_FILE && filetype!=_VIDEO_FILE && filetype!=_IMAGE_FILE ) )
				{
	      			AddFileItem_UC16_WithSorting( pickup,  &dirinfo, filetype, ext, FATFS );
				pickup++;
				}

			// ALARM_SET, AUDIO_PLAYER
			else if( flags == 3 && filetype == _AUDIO_FILE ) {
  			AddFileItem_UC16_WithSorting( pickup,  &dirinfo, filetype, ext, FATFS );
				pickup++;
			}

			}

		}

	fatfs_dseek( currdir_fd, 0 );
	fatfs_dclose( currdir_fd );

	// check TAVICAST folder & Add
	if ( ExistTaviCastOnParentDir((RMuint16*)CurrentFullPathDir, &DirInfo4TaviCast) == 0 ) {
		AddFileItem_UC16_OnTop( 0 , &DirInfo4TaviCast, _DIRECTORY_LINK, "dir", FATFS );
		i++;
		}

	for(int af=0; af<pickup; af++ )
		{
		RELOAD_WATCHDOGTIMER;
		AddFileItem_UC16( i, ItemList_Files[af], FATFS );
		i++;
		}

	// USB Client Detection
	if( pTAVI->isTaviWorkingAsUsbHost && IsThisFatFs_Root() ) {
		DEBUGMSG(1,("-pTAVI->isTaviWorkingAsUsbHost : TRUE\n"));
		str2ustr( ucTemp, "usb0", strlen("usb0") );
		AddFileItem_UC16_OnTop( 0, ucTemp, 0, _DIRECTORY, "dir", LINUXFS );
		i++;
	}


	return TRUE;

ERROR__FATFS_ERROR:

	fatfs_dclose( currdir_fd );
	return FALSE;
}

RMuint64 CFileBrowser::GetFreeSpace( void )
{
	if ( IsThisLinuxFs() )
		return df_linux("/usb0");
	else
		return df_fatfs(1);
}

void CFileBrowser::CreateTitle( uchar_t text[] )
{
	char szTitle[MAX_PATH*2];
	
	DEBUGMSG(1,("CreateTitle()\n"));
	if( ustrcmp( RootDir, CurrentFullPathDir ) )
		{
		DEBUG_U16MSG( StaticTitle, "-DynamicTitle");
		str2ustr( (uchar_t *)szTitle, "../");
		ustrcat( (uchar_t *)szTitle, DynamicTitle );
		
		g_TopPanel->SetTitle(szTitle, true);
		g_TopPanel->SetVisible( true );
		g_TopPanel->Update();
		}
	else
		{
		DEBUG_U16MSG( StaticTitle, "-StaticTitle");
		g_TopPanel->SetTitle(TitleResIdx);
		g_TopPanel->SetVisible( true );
		g_TopPanel->Update();
		}
	
}

void CFileBrowser::CreateSelectBar()
{
	RelativeIndex = SelectedAbsoluteIndex - FirstShownIndex;

	if( RelativeIndex < 0)
		RelativeIndex = 0;
	else if( RelativeIndex >= _FB_MAX_DISPLAY_INDEX )
		RelativeIndex  = _FB_MAX_DISPLAY_INDEX-1;
 

	int sby = _FB_SELECTBAR_START_Y + ( RelativeIndex * _FB_SELECTBAR_HEIGHT );
	
	DrawBoxWithoutRefresh( 
		_FB_SELECTED_ITEM_BAR_TRANSPARENCY_COLOR, 
		_FB_SELECTBAR_START_X, 
		sby, 
		320-(18*2)-SCROLLBAR_SPACE,
		_FB_ROW_HEIGHT-1
		);

	DrawBoxWithoutRefresh( 
		_FB_SELECTED_ITEM_BAR_COLOR, 
		_FB_SELECTBAR_START_X, 
		sby, 
		22,
		_FB_ROW_HEIGHT-1
		);

	DrawBoxWithoutRefresh( 
		_FB_SELECTED_ITEM_BAR_SHADOW_COLOR, 
		_FB_SELECTBAR_START_X, 
		sby+_FB_ROW_HEIGHT-1, 
		320-(18*2)-SCROLLBAR_SPACE + 3,
		1
		);

	DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE], 0, _FB_SELECTBAR_START_X, sby, 4,22 );
	DrawItemWithoutRefresh(	&TAVI_IMAGES[SELECTBAR_EDGE], 7, _FB_SELECTBAR_START_X+320-(18*2)-SCROLLBAR_SPACE, sby, 4,22 );
}



void CFileBrowser::CreateTextData()
{
	int textcolor, icontype;
	U32 option;
	int crc32value;
	int filecrc;

	char timestring[10];

	LastIndex = ItemList.Count()-1;
	for( RelativeIndex=0; RelativeIndex <= MIN(_FB_MAX_DISPLAY_INDEX-1, LastIndex); RelativeIndex++)
		{

		AbsoluteIndex = FirstShownIndex + RelativeIndex;

		switch( ItemList[AbsoluteIndex]->filetype )
			{
			case _DIRECTORY:
				if ( ItemList[AbsoluteIndex]->filesystem == LINUXFS )
					icontype = 14;
				else		// FATFS
					icontype = 4;
				break;

			case _DIRECTORY_LINK:
				icontype = 26;
				break;

			case _NETWORK_DIRECTORY:
				icontype = 14;
				break;

			case _AUDIO_FILE:
				icontype = 10;
				break;

			case _VIDEO_FILE:
				icontype = 20;
				break;

			case _IMAGE_FILE:
				icontype = 22;
				break;

			case _TEXT_FILE:
				icontype = 24;
				break;

			default:
				icontype = 30;
			}


		crc32value = crc32((char*)ItemList[AbsoluteIndex]->D.name, (ustrlen((uchar_t*)ItemList[AbsoluteIndex]->D.name)-4)*2);
		filecrc = crc32((char*)ItemList[AbsoluteIndex]->D.name, (ustrlen((uchar_t*)ItemList[AbsoluteIndex]->D.name))*2);
		for(int i=0; i< SubtitleCrcList.Count(); i++ )
			{
			if( icontype == 20 && SubtitleCrcList[i]->CRC == crc32value )
				{
				icontype = ICON_VIDEOwSUBTITLE;
				break;
				}
			}

 		if( AbsoluteIndex == SelectedAbsoluteIndex )
 			{
			textcolor 		= _BLACK;
			option		= (DT_TRANSPARENCY|DT_SCROLLTEXT|DT_OUTLINE);
			icontype++;
			}
		else
			{
			textcolor 		= _WHITE;
			option		= (DT_TRANSPARENCY|DT_FILEBROWSER|DT_OUTLINE);
			}


		if( ItemList[AbsoluteIndex]->isSelected ) {
			DrawItemWithoutRefresh(
				&TAVI_IMAGES[ICON_SELECTED],
				0,
				7,
				_FB_SELECTBAR_START_Y + ( RelativeIndex *_FB_SELECTBAR_HEIGHT ) + 4,
				8,
				14
				);
		}

		DrawItemWithoutRefresh(
			&TAVI_IMAGES[TYPE_ICON_LIST],
			icontype,
			_FB_SELECTBAR_START_X+ 5,
			_FB_SELECTBAR_START_Y + ( RelativeIndex *_FB_SELECTBAR_HEIGHT ) + 5,			
			12,
			12
			);

    if( eActiveMode == _FBMODE_VIDEOPLAYER ) {
      for( int i=0; i<nfiles; i++ ) {
        //printf( "enjoyed: 0x%x, crc32value: 0x%x\n", enjoyed[i], crc32value );
        if( filecrc == enjoyed[i] ) {
          textcolor = _GRAY80;
        }
      }
    }
    
		pT->DrawUC16Text(
			(uchar_t*)ItemList[AbsoluteIndex]->D.name,
			_FB_SELECTBAR_START_X+ 22 + 8, 
			_FB_SELECTBAR_START_Y + ( RelativeIndex *_FB_SELECTBAR_HEIGHT ) + 5,
			textcolor,
			12,
			option
			);
		}
}

void CFileBrowser::CreateVScrollBar()
{

}


int CFileBrowser::MsgBtnUp( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	if( fpDialogBoxMsgProc == 0 ) {
		return ( MsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
	}
	else {
		return ( fpDialogBoxMsgProc( ssTaskId, ulMsgId, ssParam1, ssParam2) );
	}
}



int CFileBrowser::CursorUp()
{
	DEBUGMSG(1,("[CFileBrowser::CursorUp()]\n"));


#if 0
	if( SelectedAbsoluteIndex > 0 )
		SelectedAbsoluteIndex--;						// SelectedAbsoluteIndex


	if ( SelectedAbsoluteIndex <  FirstShownIndex  
		&& FirstShownIndex > 0 )
		FirstShownIndex--;
#else
	if( SelectedAbsoluteIndex > 0 )
		{
		SelectedAbsoluteIndex--;
		if ( SelectedAbsoluteIndex <  FirstShownIndex  
			&& FirstShownIndex > 0 )
			FirstShownIndex--;
		}
	else
		{
		SelectedAbsoluteIndex	= LastIndex;				// SelectedAbsoluteIndex
		FirstShownIndex 		= LastIndex - MIN(_FB_MAX_DISPLAY_SLOT, LastIndex);
		}
#endif

	choice = SelectedAbsoluteIndex;
	return choice;

}


int CFileBrowser::CursorDown()
{
	DEBUGMSG(1,("[CFileBrowser::CursorDown()]\n"));
	
#if 0
	if( SelectedAbsoluteIndex < LastIndex )
		SelectedAbsoluteIndex++;

	if ( SelectedAbsoluteIndex >  (FirstShownIndex +  _FB_MAX_DISPLAY_INDEX -1) 
		&& (FirstShownIndex +  _FB_MAX_DISPLAY_INDEX -1) < LastIndex )
		FirstShownIndex++;

#else

	if( SelectedAbsoluteIndex < LastIndex )
		{
		SelectedAbsoluteIndex++;

		if ( SelectedAbsoluteIndex >  (FirstShownIndex +  _FB_MAX_DISPLAY_INDEX -1) 
			&& (FirstShownIndex +  _FB_MAX_DISPLAY_INDEX -1) < LastIndex )
			FirstShownIndex++;
		}
	else
		{
		SelectedAbsoluteIndex	= 0;				// SelectedAbsoluteIndex
		FirstShownIndex		= 0;
		}
#endif

	choice = SelectedAbsoluteIndex ;
	return choice;
}

int CFileBrowser::CursorLeft()
{
	DEBUGMSG(1,("[CFileBrowser::CursorLeft()]\n"));

//	if( DirectoryLevel == 0 )
//		return -1;


	 if( ChangeToParentDirectory( _REDO_CURSOR ) )
		{
		ReDraw(SKIP_OSD_REFRESH);
		return 0;
		}

	else 
		{
//		ReDraw(SKIP_OSD_REFRESH);
		return -1; // exit?
		}
}

int CFileBrowser::CursorRight()
{
	DEBUGMSG(1,("[CFileBrowser::CursorRight()]\n"));
	return (Select(SKIP_OSD_REFRESH));
}



void CFileBrowser::InitCursorLoop()
{
	DEBUGMSG(1,("[CFileBrowser::InitCursorLoop()]\n"));
	TouchPreParam1	= 0;
	TouchPreParam2	= 0;
	TouchSum		= 0;
	TouchCounter		= 0;
	TouchMeanValue	= 0;
}

void CFileBrowser::CursorLoop( int Direction, int RawLoopData )
{
	int LoopTarget;
	DEBUGMSG(1,("[CFileBrowser::CursorLoop()]\n"));
	DEBUGMSG(1,("-Direction : %d\n", Direction ));
	DEBUGMSG(1,("-RawLoopData : %d\n", RawLoopData ));

	if( RawLoopData > 300 )		LoopTarget = 4;
	else if( RawLoopData > 250 )	LoopTarget = 3;
	else if( RawLoopData > 200 )	LoopTarget = 2;	
	else 						LoopTarget = 1;

	DEBUGMSG(1,("-LoopTarget : %d\n", LoopTarget ));

	if( Direction > 0 )
		{
		for(int loop=0; loop<LoopTarget; loop++)
			{
			CursorDown();
			ReDraw(OSD_REFRESH);
			}
		}
	else
		{
		for(int loop=0; loop<LoopTarget; loop++)
			{
			CursorUp();
			ReDraw(OSD_REFRESH);
			}
		}
}

int CFileBrowser::ChangeToParentDirectory( U32 option )
{
	int szSrcLen=0;
	RMuint32 DTcrc;
	DEBUGMSG(1,("[CFileBrowser::ChangeToParentDirectory( %d )\n - DirectoryLevel( %d)]\n", option, DirectoryLevel ));

	if( IsThisLinuxFs() )
		{
			if( IsThisLinuxFs_Root() )
				{
				ClearFileItem( option );
				str2ustr( CurrentFullPathDir, "/hd1" );
				
				print_ustr(CurrentFullPathDir, "--- 10th :: is root on linuxfs : CurrentFullPathDir" );	// for debug

				ScanDir( CurrentFullPathDir, 0 );
				CurrentFileSystem = FATFS;
				return TRUE;
				}
			else {
				uchar_t u16String[260];
				
				DTcrc = crc32( (char*)DynamicTitle, ustrlen(DynamicTitle)*2 );		
				CurrentFileSystem = LINUXFS;
				GetParentDir_In_UC16FullDirString( u16String, CurrentFullPathDir);

				print_ustr(CurrentFullPathDir, "--- 11th :: is not root on linuxfs : CurrentFullPathDir" );	// for debug

				ClearFileItem( option );
				ScanDir_linuxfs(u16String);			
				SetSelectbarValues(DTcrc);
				GetLastItem_In_UC16FullDirString_WithUserDefineSplitter( DynamicTitle, u16String, (uchar_t)'/');
				return TRUE;
				}
		}

	else
		{
		if( ustrcmp( CurrentFullPathDir, RootDir )==0 )
			{
			return FALSE;		
			}
		else
			{
			DTcrc = crc32( (char*)DynamicTitle, ustrlen(DynamicTitle)*2 );
			CurrentFileSystem = FATFS;
			szSrcLen = ustrlen( CurrentFullPathDir );
			DEBUG_U16MSG( CurrentFullPathDir, "-(B)CurrentFullPathDir" );
			DEBUGMSG(1,("-DTcrc : 0x%08x\n", DTcrc ));
			if ( ustrcmp( CurrentFullPathDir, TaviCastDir )==0 ) {
				ustrcpy(CurrentFullPathDir, RootDir);
				}
			else {
				for(int i=szSrcLen;i>0;i--)
					{
					if( CurrentFullPathDir[i] == 0x002f )
						{
						CurrentFullPathDir[i] = 0x0000;
						break;
						}
					}
				}
			DEBUG_U16MSG( CurrentFullPathDir, "-(A)CurrentFullPathDir" );
			DEBUG_U16MSG( DynamicTitle, "-DynamicTitle" );
			ClearFileItem(_SKIP_CURSOR);
			ScanDir( CurrentFullPathDir, -1 );
			SetSelectbarValues(DTcrc);
			return TRUE;
			}
		}

	printf("[ChangeToParentDirectory()] return FALSE\n");
	return FALSE;
}


int MsgProc_WaitButtonPress( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	DEBUGMSG(1,("[pfpWaitBtnPress] : ssParam1[0x%x]\n",  ssParam1));

	if( ulMsgId != MSG_BUTTON_UP )
		return -1;

	switch( ssParam1 )
	{
		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_UP:
		case TAVI_KEY_DOWN:
		case TAVI_KEY_LEFT:
		case TAVI_KEY_RIGHT:
		case TAVI_KEY_ENTER:
			extern CFileBrowser* pLFBrowser;
			pLFBrowser->ChangeToParentDirectory( 0);
			return -1;
			break;

		//////////////////////////////////////////////////////////////////////
		case TAVI_KEY_SHUTDOWN:
			//VP_CommonKeyProcess_EXIT(ssTaskId, 0);
			return -1;
			break;
	}

	return 0;
}


int CFileBrowser::Reset()
{
	SelectedAbsoluteIndex	= 0;	// Cursor
	AbsoluteIndex			= 0;	// 0 ~ LASTINDEX
	FirstShownIndex		= 0;	// Absolute Index			
	LastIndex			= 0;	// Absolute Index
	LastSlot 				= 0;	
	choice 				= 0;
	DirectoryLevel			= 0;

	DynamicTitle[0] = (uchar_t)0;
	CurrentFullPathDir[0] = (uchar_t)0;
	ParentDir[0] = (uchar_t)0;
	Filename[0] = (uchar_t)0;

	ItemList.FreeAll();
	ItemList_Files.FreeAll();
	SubtitleCrcList.FreeAll();

	return TRUE;
}


extern int FM_MsgProc_FileBrowser( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 );

int CFileBrowser::SetActiveMode( U32 ActiveMode, MSG_PROCESSOR pfpMsgProc, U32 StartAtLastDir )
{
	DEBUGMSG(1,("[CFileBrowser::SetActiveMode(ActiveMode=%d,...,StartAtLastDir=%d)] ... ", ActiveMode, StartAtLastDir ) );

	Reset();	
	eActiveMode = (eFILEBROWSER_ACTIVE_MODE)ActiveMode;

	if( pfpMsgProc != 0 ) fpDialogBoxMsgProc = pfpMsgProc;
	else fpDialogBoxMsgProc = FM_MsgProc_FileBrowser;
//	else { fpDialogBoxMsgProc = FM_MsgProc_FileBrowser; return TRUE; }

	switch( (int)eActiveMode )
		{
		case _FBMODE_FILEMANAGER: 
			TitleIcon = ICON_FILEMANAGER;
			TitleResIdx = ML_MANAGEMENT;
			str2ustr( RootDir, _HDD_ROOT, strlen(_HDD_ROOT) );
			break;

		case _FBMODE_VIDEOPLAYER: 
			TitleIcon = ICON_VIDEOPLAYER;
			TitleResIdx = ML_MOVIE;
			str2ustr( RootDir, _VIDEO_DIR, strlen(_VIDEO_DIR) );
			break;

		case _FBMODE_AUDIOPLAYER: 
			TitleIcon = TITLE_MUSIC;
			TitleResIdx = ML_MUSIC;
			str2ustr( RootDir, _AUDIO_DIR, strlen(_AUDIO_DIR) );
			break;

		case _FBMODE_IMAGEVIEWER: 
			TitleIcon = PHOTO_TITLE;
			TitleResIdx = ML_PHOTO;
			str2ustr( RootDir, _PICTURE_DIR, strlen(_PICTURE_DIR) );
			break;			

		case _FBMODE_TEXTVIEWER: 
			TitleIcon = ICON_TEXTVIEWER;
			TitleResIdx = ML_TEXT;
			str2ustr( RootDir, _eBOOK_DIR, strlen(_eBOOK_DIR) );
			break;

		case _FBMODE_ALARM_SETTTING: 
			TitleIcon = ICON_EXTRA;
			TitleResIdx = ML_EXTRA;
			str2ustr( RootDir, _AUDIO_DIR, strlen(_AUDIO_DIR) );
			break;


		default:
			ASSERT(0);
		}

	ustrcpy( StaticTitle, pT->GetTextRes(TitleResIdx) );

	if(StartAtLastDir) 
		{ 
		if(LoadDirInfo())
			{
			ScanDir( CurrentFullPathDir, 2 ); 
			crcRootDir = crc32( (char*)RootDir, ustrlen(RootDir)*2 );
			return TRUE;
			}
		}

	ustrcpy(CurrentFullPathDir, RootDir ); 
	ScanDir( RootDir, 0 );
	crcRootDir = crc32( (char*)RootDir, ustrlen(RootDir)*2 );
	ustrcpy(DynamicTitle, StaticTitle );

	return TRUE;
}
int CFileBrowser::SetSelectbarValues( RMuint32 CRC )
{
	RMuint32 nv1 = CRC;
	RMuint32 nv2;
	int ret = FALSE;


	DEBUGMSG(1,("SetSelectbarValues()\n"));

	LastIndex = ItemList.Count()-1;	// if LastIndex== 36,  range = [0~36]
	DEBUGMSG(1,("-CRC : [%08x]\n", CRC));
	DEBUGMSG(1,("-LastIndex : %d\n", LastIndex ));
	
	for(int i=0; i<=LastIndex; i++ )
		{
		nv2=crc32( (char*)ItemList[i]->D.name, ustrlen((uchar_t*)ItemList[i]->D.name)*2);

		if( nv1 == nv2 )	{
			choice = SelectedAbsoluteIndex = i;
			ret = TRUE;
			break;
			}
		else	{
			DEBUGMSG(1,("-nv2(crc32):%08x\n",nv2));
			}

		}
	if(ret)
		{
		if( SelectedAbsoluteIndex <= _FB_MAX_DISPLAY_SLOT )
			FirstShownIndex = 0;
		else if( LastIndex - SelectedAbsoluteIndex  <  _FB_MAX_DISPLAY_SLOT )
			FirstShownIndex = SelectedAbsoluteIndex - _FB_MAX_DISPLAY_SLOT;
		else
			FirstShownIndex = SelectedAbsoluteIndex;
		}

	DEBUGMSG(1,("-FirstShownIndex : %d\n", FirstShownIndex));
	DEBUGMSG(1,("-SelectedAbsoluteIndex : %d\n", SelectedAbsoluteIndex));
	DEBUGMSG(1,("-choice : %d\n", choice));

	return ret;
}
int CFileBrowser::SetCurrentChoice(int NewChoice)
{
	SelectedAbsoluteIndex = NewChoice;
	choice = NewChoice;
	return choice;
}
int CFileBrowser::SetCurrentChoiceToNext()
{

	if( SelectedAbsoluteIndex < LastIndex )
		SelectedAbsoluteIndex++;
	else if( SelectedAbsoluteIndex >= LastIndex )
		{
		SelectedAbsoluteIndex = 0;
		FirstShownIndex = 0;
		choice = SelectedAbsoluteIndex ;
		return choice;
		}

	if ( SelectedAbsoluteIndex >  (FirstShownIndex +  _FB_MAX_DISPLAY_INDEX -1) 
		&& (FirstShownIndex +  _FB_MAX_DISPLAY_INDEX -1) < LastIndex )
		FirstShownIndex++;

	choice = SelectedAbsoluteIndex ;
	
	return choice;
}
int CFileBrowser::RememberDirRepeatStartPoint()
{
	DirRepeatStartPoint = choice;
	return choice;
}
int CFileBrowser::GetActiveMode()
{
	return ((int)eActiveMode);
}
int CFileBrowser::GetCurrentChoice()
{
	return choice;
}
eFILE_TYPE CFileBrowser::GetListItemType( int index )
{
	return (ItemList[index]->filetype);
}
eFILE_TYPE CFileBrowser::GetCurrentListItemType( )
{
	return (ItemList[choice]->filetype);
}
RMuint32 CFileBrowser::GetCurrentListItemFilesize( )
{
	return (ItemList[choice]->D.file_size );
}
int CFileBrowser::GetCurrentListItemNameLength( )
{
	return ustrlen( (uchar_t*)ItemList[choice]->D.name );
}
int CFileBrowser::GetListItemSize( int index )
{
	return (ItemList[index]->D.file_size);
}
int CFileBrowser::GetListItemName( int index, char StringBuffer[] )
{
	sprintf( StringBuffer, "%s", ItemList[index]->D.name );
	return TRUE;
}
int CFileBrowser::GetListItemName_UC16( int index, uchar_t StringBuffer[] )
{
	ustrcpy( StringBuffer, (uchar_t*)ItemList[index]->D.name );
	return TRUE;
}
int CFileBrowser::GetListItemCount()
{
	return ( ItemList.Count() );
}
int CFileBrowser::Get_IL_SelectedCount()
{
	return CurrentFI.FolderItemCount;
}
int CFileBrowser::RecursiveScan( const uchar_t* pathSource, FOLDER_INFO* pFI, eRECURSIVE_SCAN_OPTION option )
{
	char szCurPathSou[260*2];
	CStack< CString > dirStackSou( 1024 );
	CString szDirPathSou;
	uchar_t* wszPath;
	FATFS_ERROR sdFatfsErr;
	FATFS_DIR_ENTRY_INFO sdDirEntrySou;
	RMuint32 hDirSou;
	char delimeter[4] = { '/', 0, 0, 0 };
	CString szPathSou;
	CString szTmp;

	pFI->FolderItemCount = 0;
	pFI->ItemTotalSize = 0;
	//TotalSizeOfSelectedFiles = 0;

	DEBUGMSG(1,( "+RecursiveScan\n" ));

//	TAVI_WakeUp();

	szPathSou.SetString( (char*)pathSource, true );
	dirStackSou.Push( szPathSou );
	while( dirStackSou.IsEmpty() == false ) {
		szDirPathSou = dirStackSou.Pop();
		ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
		wszPath = ( uchar_t* )szDirPathSou;
		sdFatfsErr = fatfs_wdopen( (RMuint16*)wszPath, (RMuint32*)&hDirSou );
		while( sdFatfsErr == FATFS_ERROR_NO_ERROR ) {
			sdFatfsErr = fatfs_dread( hDirSou, &sdDirEntrySou );
			if( sdFatfsErr != FATFS_ERROR_NO_ERROR ) break;
			{
					print_ustr((uchar_t*)sdDirEntrySou.name, 0);	// for debug
			}

			if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 && 
			     sdDirEntrySou.name[2] == 0 && sdDirEntrySou.name[3] == 0 ) continue;
			if( sdDirEntrySou.name[0] == '.' && sdDirEntrySou.name[1] == 0 &&
				sdDirEntrySou.name[2] == '.' && sdDirEntrySou.name[3] == 0 ) continue;

			ustrcpy( (uchar_t*)szCurPathSou, (uchar_t*)szDirPathSou );
			ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)delimeter );
			ustrcat( (uchar_t*)szCurPathSou, (uchar_t*)sdDirEntrySou.name );
	
			if( sdDirEntrySou.attributes&ATTR_DIRECTORY )
				{
				szPathSou.SetString( szCurPathSou, true );
				dirStackSou.Push( szPathSou );
				pFI->FolderItemCount++;
				}
			else
				{

				if( option==GET_ONLY_TFI )
					{
					pFI->ItemTotalSize += sdDirEntrySou.file_size;
					pFI->FolderItemCount++;
					}
				else
					{
					ASSERT(0);
					}

			}
		}
		fatfs_dclose( hDirSou );
	}


	return pFI->FolderItemCount;
}

int CFileBrowser::IsLastItem()
{
	if( choice+1 == ItemList.Count() )
		return TRUE;
	else
		return FALSE;
}

int CFileBrowser::IsIL()
{
	return (ItemList[choice]->isSelected);
}
int CFileBrowser::IsSIL()
{
	return IsSIL(choice);
}
int CFileBrowser::IsSIL(int index)
{

	DEBUGMSG(1,("\nIsSIL(%d)\n", index ));
	DEBUGMSG(1,("-SelectedItemList.Count() : %d\n", SelectedItemList.Count() ));
	for( int i=0; i<SelectedItemList.Count(); i++ )
		{
		DEBUGMSG(1,("=[index:%d, i:%d]==============================================\n",index,i));
		//printu("-ItemList[index]->D.name", (uchar_t*)ItemList[index]->D.name );
		//printu("-SelectedItemList[i]->name", (uchar_t*)SelectedItemList[i]->name );
		if( ustrcmp( (uchar_t*)ItemList[index]->D.name, (uchar_t*)SelectedItemList[i]->name )==0 )
			{
			DEBUGMSG(1,("... Same Dir\n"));
			DEBUGMSG(1,("---------------------------------------------------------------------------\n"));
			printu("-ItemList[index]->D.parentdir", (uchar_t*)ItemList[index]->D.parentdir );
			//printu("-SelectedItemList[i]->parentdir", (uchar_t*)SelectedItemList[i]->parentdir );			
			if( ustrcmp( (uchar_t*)ItemList[index]->D.parentdir, (uchar_t*)SelectedItemList[i]->parentdir )==0 )
				{
				DEBUGMSG(1,("... Same Parent\n"));
				return TRUE;
				}
			}

		DEBUGMSG(1,("... Different Dir\n"));
		DEBUGMSG(1,("===========================================================\n\n"));

		}
	return FALSE;
}

int CFileBrowser::Add_IL_CheckTag( eFILEITEM_ENCLOSE_OPTION option )
{
	uchar_t PathName[260];
	char szPathName[260*2];
	FOLDER_INFO TFI;

	DEBUGMSG(1,("Add_IL_CheckTag()\n"));

	TFI.FolderItemCount = 0;
	TFI.ItemTotalSize = 0;

	if( option == ENCLOSE_ALL )
		{
		for( int index=0; index<ItemList.Count(); index++ )
			{		
			ItemList[index]->isSelected = TRUE;
			// FOLDER
			if( (ItemList[index]->D.attributes&0x10) ||
				ItemList[index]->filetype == _DIRECTORY  ||
				ItemList[index]->filetype == _NETWORK_DIRECTORY )
				{
				
				if ( ItemList[choice]->filesystem == LINUXFS ) {
					ustrcpy( PathName, (uchar_t*)ItemList[choice]->D.parentdir );
					addchar( PathName, '/');
					ustrcat( PathName, (uchar_t*)ItemList[choice]->D.name );

					ConvertUnicodeToUniXlate( szPathName, PathName );

					TFI.ItemTotalSize = du_linuxfs( szPathName, &TFI )*1024;
					printf("--- 6th :: szPathName=%s, FolderItemCount =%d\n", szPathName, TFI.FolderItemCount);	// for debug
					}
				else {
					ustrcpy( PathName, CurrentFullPathDir );
					addchar( PathName, '/' );
					ustrcat( PathName, (uchar_t*)ItemList[index]->D.name );

					RecursiveScan( PathName, &TFI, GET_ONLY_TFI );
					TFI.FolderItemCount++;
					}
				TotalSizeOfSelectedFiles += TFI.FolderItemCount;
				CurrentFI.FolderItemCount += TFI.FolderItemCount;
				CurrentFI.ItemTotalSize += TFI.ItemTotalSize;
				}

			// FILE
			else
				{
				TotalSizeOfSelectedFiles += 1;
				CurrentFI.FolderItemCount += 1;
				CurrentFI.ItemTotalSize += ItemList[index]->D.file_size;
				}
				
			}

			return TRUE;

		}
	
	else if( option == ENCLOSE_UNDER_CURSOR || option == ENCLOSE_SELECTED )
		{
		ItemList[choice]->isSelected = TRUE;
		
		// FOLDER
		if( (ItemList[choice]->D.attributes&0x10) ||
			ItemList[choice]->filetype == _DIRECTORY  ||
			ItemList[choice]->filetype == _NETWORK_DIRECTORY )
			{
			if ( ItemList[choice]->filesystem == LINUXFS ) {
				ustrcpy( PathName, (uchar_t*)ItemList[choice]->D.parentdir );
				addchar( PathName, '/');
				ustrcat( PathName, (uchar_t*)ItemList[choice]->D.name );
		
				ConvertUnicodeToUniXlate( szPathName, PathName );

				TFI.ItemTotalSize = du_linuxfs( szPathName, &TFI )*1024;
				printf("--- 7th :: szPathName=%s, FolderItemCount =%d, size=%d\n", (char*)szPathName, TFI.FolderItemCount, TFI.ItemTotalSize);	// for debug
				}
			else {
				print_ustr(CurrentFullPathDir, "--- 8th :: CurrentFullPathDir");	// for debug

				ustrcpy( PathName, CurrentFullPathDir );
				addchar( PathName, '/' );
				ustrcat( PathName, (uchar_t*)ItemList[choice]->D.name );
				
				print_ustr(PathName, "--- 9th :: PathName");	// for debug

				RecursiveScan( PathName, &TFI, GET_ONLY_TFI );
				TFI.FolderItemCount++;
				}
			TotalSizeOfSelectedFiles += TFI.FolderItemCount;
			CurrentFI.FolderItemCount += TFI.FolderItemCount;
			CurrentFI.ItemTotalSize += TFI.ItemTotalSize;
			}		
		// FILE
		else
			{
			TotalSizeOfSelectedFiles += 1;
			CurrentFI.FolderItemCount += 1;
			CurrentFI.ItemTotalSize += ItemList[choice]->D.file_size;
			}

		return TRUE;
		}

	return FALSE;
}

int CFileBrowser::Add_IL_To_SIL( eFILEITEM_ENCLOSE_OPTION option )
{
	FATFS_DIR_ENTRY_INFO* pNode;
	uchar_t PathName[1024];
	FOLDER_INFO TFI;

	DEBUGMSG(1,("AddTo_SIL()\n"));
	DEBUGMSG(1,("-option: %d\n", option ));

	if( option == ENCLOSE_SELECTED )
		{
		Clear_SIL_Only();
		for( int index=0; index<ItemList.Count(); index++ )
			{
			if(ItemList[index]->isSelected == TRUE )
				{
				pNode = new FATFS_DIR_ENTRY_INFO;
				ustrcpy( (uchar_t*)ItemList[index]->D.parentdir, CurrentFullPathDir );
				memcpy( pNode, &(ItemList[index]->D) , szFATFS_DEI );
				SelectedItemList.Add( pNode );
				}
			}
		return TRUE;
		}

	else if( option == ENCLOSE_UNDER_CURSOR )
		{
		Add_IL_CheckTag( ENCLOSE_UNDER_CURSOR );
		pNode = new FATFS_DIR_ENTRY_INFO;
		ustrcpy( (uchar_t*)ItemList[choice]->D.parentdir, CurrentFullPathDir );
		memcpy( pNode, &(ItemList[choice]->D) , szFATFS_DEI );
		SelectedItemList.Add( pNode );
		return TRUE;
		}

	ASSERT(0);
	return FALSE;
}

int CFileBrowser::Remove_IL_CheckTag( eFILEITEM_ENCLOSE_OPTION option )
{
	uchar_t PathName[260];
	char szPathName[260*2];
	FOLDER_INFO TFI;

	TFI.FolderItemCount = 0;
	TFI.ItemTotalSize = 0;

	DEBUGMSG(1,("Remove_IL_CheckTag()\n"));

	if( option == ENCLOSE_UNDER_CURSOR )
		{
		
		if( ItemList[choice]->filetype == _DIRECTORY 
			||ItemList[choice]->filetype == _NETWORK_DIRECTORY )
			{
			if ( ItemList[choice]->filesystem == LINUXFS ) {
				ustrcpy( PathName, (uchar_t*)ItemList[choice]->D.parentdir );
				addchar( PathName, '/');
				ustrcat( PathName, (uchar_t*)ItemList[choice]->D.name );

				ConvertUnicodeToUniXlate( szPathName, PathName );
				TFI.ItemTotalSize = du_linuxfs( szPathName, &TFI )*1024;
				printf("--- 8th :: szPathName=%s, FolderItemCount =%d\n", szPathName, TFI.FolderItemCount);	// for debug				
				}
			else {
				ustrcpy( PathName, CurrentFullPathDir );
				addchar( PathName, '/' );
				ustrcat( PathName, (uchar_t*)ItemList[choice]->D.name );
				
				RecursiveScan( PathName, &TFI, GET_ONLY_TFI );
				TFI.FolderItemCount++;
				}
			
			DEBUGMSG(1,("-TFI.FolderItemCount:[%d]\n", TFI.FolderItemCount));
			DEBUGMSG(1,("-TFI.ItemTotalSize:[%d]\n", TFI.ItemTotalSize));
			CurrentFI.FolderItemCount -= TFI.FolderItemCount;
			CurrentFI.ItemTotalSize -= TFI.ItemTotalSize;
			DEBUGMSG(1,("-CurrentFI.ItemTotalSize:[%d]\n", CurrentFI.ItemTotalSize));
			ItemList[choice]->isSelected = FALSE;
			}

		else
			{
			CurrentFI.FolderItemCount--;
			CurrentFI.ItemTotalSize -= ItemList[choice]->D.file_size;
			ItemList[choice]->isSelected = FALSE;
			}
		return TRUE;
		}
	
	else if( option == ENCLOSE_ALL )
		{
		Clear_IL_Only();
		return TRUE;
		}
	
	else 
		ASSERT(0);

	return FALSE;
}


int CFileBrowser::RemoveOn_SIL( eFILEITEM_ENCLOSE_OPTION option )
{
	uchar_t PathName[1024];
	FOLDER_INFO TFI;
	CNode<FATFS_DIR_ENTRY_INFO>* pNode;

	DEBUGMSG(1,("RemoveOn_SIL()\n"));

	for( int i=0; i<SelectedItemList.Count(); i++ )
		{
		if( ustrcmp( (uchar_t*)ItemList[choice]->D.name, (uchar_t*)SelectedItemList[i]->name )==0 )
			{
			if( ustrcmp( (uchar_t*)ItemList[choice]->D.parentdir, (uchar_t*)SelectedItemList[i]->parentdir )==0 )
				{
				pNode = SelectedItemList.GetNode( choice );
				SelectedItemList.DeleteNode( pNode, 1);
				Remove_IL_CheckTag( option );
				return TRUE;
				break;
				}
			}
		}

	return FALSE;
}

int CFileBrowser::Clear_IL_Only()
{
	DEBUGMSG(1,("\nClear_IL_Only\n"));

	CurrentFI.FolderItemCount = 0;
	CurrentFI.ItemTotalSize = 0;

	for( int i=0; i<ItemList.Count(); i++ )
		ItemList[i]->isSelected = FALSE;

	return TRUE;
}
int CFileBrowser::Clear_SIL_Only()
{
	DEBUGMSG(1,("\nClear_SIL_Only()\n"));
	TotalSizeOfSelectedFiles = 0;
	SelectedItemList.FreeAll();
	return TRUE;
}
int CFileBrowser::Clear_IL_SIL_All()
{
	DEBUGMSG(1,("\nClear_IL_SIL_All()\n"));
	Clear_SIL_Only();
	Clear_IL_Only();
	return TRUE;
}

int CFileBrowser::Get_CurrentFI_Count()
{
	return CurrentFI.FolderItemCount;
}

RMuint64 CFileBrowser::Get_CurrentFI_Size()
{
	return CurrentFI.ItemTotalSize;
}


void CFileBrowser::SetSrcPathname()
{
	memset( SrcPathname, 0, sizeof(SrcPathname) );
	ustrcpy( SrcPathname, CurrentFullPathDir );
}

int CFileBrowser::Compare_SD_Folder()
{
	if( ustrcmp( SrcPathname, CurrentFullPathDir ) == 0 )
		return TRUE;
	else
		return FALSE;
}

int CFileBrowser::LoadDirInfo()
{
	RMuint32 fd;
	RMuint32 ret;
	RMuint32 dd;
	RMuint32 nw;
	uchar_t Name[260];
	char temp[260];
	FATFS_ERROR err = FATFS_ERROR_NO_ERROR;

	if( eActiveMode == _FBMODE_VIDEOPLAYER )
		ustrcpy( CurrentFullPathDir, (uchar_t*)TaviSysParam.video.lastaccess );
	else if( eActiveMode == _FBMODE_TEXTVIEWER )
		ustrcpy( CurrentFullPathDir, (uchar_t*)TaviSysParam.text.lastaccess );

	return TRUE;

}

int CFileBrowser::SaveDirCheck()
{

	RMuint32 ret;
	RMuint32 dd;
	uchar_t Name[260];
	char temp[260];
	FATFS_ERROR err = FATFS_ERROR_NO_ERROR;

	DEBUGMSG(1,("\nSaveDirCheck()\n"));

//	TAVI_WakeUp();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// _SAVEDATA_DIR check
	sprintf( temp, "%s", _SAVEDATA_FULLPATH );str2ustr( Name, temp, strlen(temp) );
	
	if( (err = fatfs_wdopen( Name, &dd )) > FATFS_ERROR_NO_ERROR )		
		{//_SAVEDATA_DIR not found
		DEBUGMSG(1,("-_SAVEDATA_FULLPATH(%s) not found\n", temp ));
		
		sprintf( temp, "%s", _SYSTEMDIR );str2ustr( Name, temp ,strlen(temp) );

		if( (err = fatfs_wdopen( Name, &dd )) > FATFS_ERROR_NO_ERROR ) 	
			{
			DEBUGMSG(1,("-_SYSTEMDIR(%s) not found\n : 0x%x\n", temp, err ));
			return FALSE; 	// SYSTEMDIR NOT FOUND Systerm Error
			}

		DEBUGMSG(1,("-_SYSTEMDIR(%s) found\n", temp ));
		sprintf( temp, "%s", _SAVEDATA_DIR );str2ustr( Name, temp ,strlen(temp) );

		if( (err = fatfs_wdcreate_dir( dd, Name )) > FATFS_ERROR_NO_ERROR ) 	
			{
			DEBUGMSG(1,("-Unable to create _SAVEDATA_DIR(%s) : 0x%x\n", temp, err ));
			return FALSE;	// Unable to create _SAVEDATA_DIR
			}

		DEBUGMSG(1,("-_SAVEDATA_DIR(%s) created\n", temp ));
		fatfs_dclose(dd);

		return TRUE;

		}
	else
		{
		fatfs_dclose(dd);
		DEBUGMSG(1,("-_SAVEDATA_FULLPATH(%s) found\n", temp ));
		return TRUE;
		}

	return FALSE;
}

int CFileBrowser::SaveSdiFile( FOLDER_RESUME_INFO* pR, RMuint32 option )
{

	RMuint32 fd;
	RMuint32 ret;
	RMuint32 nw;
	uchar_t Name[260];
	char temp[260];
	FATFS_ERROR err = FATFS_ERROR_NO_ERROR;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// File Save
	DEBUGMSG(1,("-SaveSdiFile() ... "));
	if( eActiveMode == _FBMODE_FILEMANAGER )
		sprintf( temp, "%s", _FILEMANAGER_SAVEFILE );
	
	else if( eActiveMode == _FBMODE_VIDEOPLAYER )
		sprintf( temp, "%s", _VIDEO_FILEBROWSER_SAVEFILE );
	
	else if( eActiveMode == _FBMODE_TEXTVIEWER)
		sprintf( temp, "%s", _TEXT_FILEBROWSER_SAVEFILE );

//	TAVI_WakeUp();
	str2ustr( Name, temp, strlen(temp) );
/*
	if( option == 1 )
		{
		fatfs_wdelete_file(Name);
		return TRUE;
		}
*/	

#if 0
	err = fatfs_wfopen( (RMuint16*)Name, _O_WRONLY|_O_TRUNC, &fd );
	if( err != FATFS_ERROR_NO_ERROR || fd == 0) 
		{
		err = FATFS_ERROR_NO_ERROR;
		err = fatfs_wfopen( (RMuint16*)Name, _O_WRONLY|_O_CREAT, &fd );
		if( err != FATFS_ERROR_NO_ERROR || fd == 0) { DEBUGMSG(1,("failed (err=0x%Xh | fd=%d)\n", err, fd)); return FALSE;}
		else { DEBUGMSG(1,("pass\n")); }
		}
#else
	if( (err=fatfs_wfopen( (RMuint16*)Name, _O_WRONLY|_O_CREAT|_O_TRUNC, &fd )) > FATFS_ERROR_NO_ERROR )
		{
		DEBUGMSG(1,("failed (err=0x%Xh | fd=%d)\n", err, fd)); 
		if(fd)fatfs_fclose(fd);
		return FALSE;
		}
#endif

	if( (err=fatfs_fwrite( fd, (RMuint8*)pR, sizeof(FOLDER_RESUME_INFO), &nw )) > FATFS_ERROR_NO_ERROR )
		{
		DEBUGMSG(1,("failed (err=0x%Xh | nw=%d)\n", err, nw)); 
		if(fd)fatfs_fclose(fd);
		return FALSE;
		}

	DEBUGMSG(1,("done (%d bytes)\n", nw));
	
	if(fd)fatfs_fclose(fd);
	return TRUE;

}

int CFileBrowser::SaveDirInfo( )
{
	int ret = FALSE;

	if( eActiveMode == _FBMODE_VIDEOPLAYER )
		ustrcpy( (uchar_t*)TaviSysParam.video.lastaccess, CurrentFullPathDir );
	else if( eActiveMode == _FBMODE_TEXTVIEWER )
		ustrcpy( (uchar_t*)TaviSysParam.text.lastaccess, CurrentFullPathDir );

	return ret;
}


int CFileBrowser::Select( eOSD_REFRESH_OPTION option )
{
	uchar_t u16String[260];

	if( ItemList[choice]->filesystem == LINUXFS ) {
		if( (ItemList[choice]->D.attributes & 0x10) ||
			ItemList[choice]->filetype == _DIRECTORY ||
			ItemList[choice]->filetype == _NETWORK_DIRECTORY )
			{// directory

			DEBUGMSG(1,("-This is Directory\n"));	
			CurrentFileSystem = LINUXFS;
			ustrcpy(DynamicTitle, (uchar_t *)ItemList[choice]->D.name);
			ustrcpy(u16String, (uchar_t*)ItemList[choice]->D.parentdir);
			adduchar(u16String, (uchar_t)'/' );
			ustrcat(u16String, (uchar_t*)ItemList[choice]->D.name );

			print_ustr(u16String, "--20th :: Select : dirPath" );	// for debug

			ClearFileItem(_KEEP_CURSOR);
			ScanDir_linuxfs( u16String );
			
			if( option == SKIP_OSD_REFRESH )
				ReDraw(SKIP_OSD_REFRESH);
			else 
				ReDraw(OSD_REFRESH);

			return 0;
			}

		else		
			{// normal file
			DEBUGMSG(1,("-This is File\n"));
			str2ustr( Filename, (char*)ItemList[choice]->D.name, strlen((char*)ItemList[choice]->D.name) );
			strcpy( CurrentFileExt, ItemList[choice]->ext );
			DEBUGMSG(1,("- Play File : [%s]\n", Filename ));
			return 1;
			}
		
	}
	else {
		if( (ItemList[choice]->D.attributes & 0x10) ||
			ItemList[choice]->filetype == _DIRECTORY ||
			ItemList[choice]->filetype == _DIRECTORY_LINK ||
			ItemList[choice]->filetype == _NETWORK_DIRECTORY  )
			{
			CurrentFileSystem = FATFS;
			ustrcpy( DynamicTitle, (uchar_t*)ItemList[choice]->D.name );

			if ( ItemList[choice]->filetype == _DIRECTORY_LINK ) {
				ustrcpy( CurrentFullPathDir, (uchar_t*)ItemList[choice]->D.parentdir );
			}

			ClearFileItem(_KEEP_CURSOR);
			ScanDir( DynamicTitle, 1 );
			if( option == SKIP_OSD_REFRESH )
				ReDraw(SKIP_OSD_REFRESH);
			else 
				ReDraw(OSD_REFRESH);
			
			return 0;
		}
		else {	// normal file
			ustrcpy( Filename, (uchar_t*)ItemList[choice]->D.name );
			strcpy( CurrentFileExt, ItemList[choice]->ext );
			
			return 1;
		}
	}
}


int CFileBrowser::ProcFBAutoScrollTimer( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	int ret;

    if( !GetListItemCount() ) return 1;
    
	Stop_FileBrowserTimer();					

	if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) )
		{

		checkpoint = pT->GetEndXPosOfTxtBuf() + 20;

		for( int sx=0; ; sx++ )
			{
			RELOAD_WATCHDOGTIMER;
			g_TopPanel->DrawTime(TRUE);

			if( sx >= checkpoint )
				{
				sx=0;
				if( (ret=pT->DrawScrollText(0)) < 0 )  
					{
					Stop_FileBrowserTimer();
					// key input!!	: stop scroll, exit
					return ret;
					}
					
				else
					{
					Reset_FileBrowserTimer();
					// eof!!  		: stop scroll, wait 3sec, rescroll
					}
				
				break;
				}

			if( (ret=pT->DrawScrollText(sx)) < 0 )
				{
				Stop_FileBrowserTimer();
				if ( ret == -2 )
					return ret;
				pT->DrawScrollText(0);
				break;
				}

			}

		}			

	return TRUE;
}

int CFileBrowser::isVisible()
{
	return iVisibleStatus;
}

int CFileBrowser::Show()
{
  if( enjoyed ) {
    delete[] enjoyed;
    nfiles = 0;
  }
  enjoyed = load_acc_info( CurrentFullPathDir, &nfiles );
	ReDraw( SKIP_OSD_REFRESH );

	DrawBottomGuidePannel(OSD_REFRESH, (BGPID_HOME|BGPID_MENU));
	
	iVisibleStatus = TRUE;
	SetTaviTouchpadMode( MEP_REPORT_RELATIVE );
	SetTaviKeyMode(TK_REPEAT|TK_VARIOUS);
	Reset_FileBrowserTimer();


  return TRUE;
}

int CFileBrowser::Hide()
{
	Stop_FileBrowserTimer();
	ClearOsd();
	g_TopPanel->SetVisible( false );
	iVisibleStatus = FALSE;
	return TRUE;
}

void CFileBrowser::ReDraw( eOSD_REFRESH_OPTION option )
{
	ClearOsd();

	CreateTitle(0);

	if( ItemList.Count()  == 0 )
		{
		pT->DrawResText( ML_NO_FILES, 320/2 - (pT->GetStringWidth(ML_NO_FILES, 12, DT_TRANSPARENCY|DT_OUTLINE)/2), 110, _WHITE, 12, DT_TRANSPARENCY|DT_OUTLINE );
		}
	else
		{
		CreateSelectBar();
		CreateTextData();
		if( ItemList.Count() > _FB_MAX_DISPLAY_INDEX )
			{
			pSb1->ReDraw( LastIndex + 1, FirstShownIndex, MIN( (LastIndex - FirstShownIndex + 1), _FB_MAX_DISPLAY_INDEX ) );
			}
		}

	if( eActiveMode == _FBMODE_FILEMANAGER )
		{
		//DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU|BGPID_SELECT));	
		}
	
	else
		{
		DrawBottomGuidePannel( SKIP_OSD_REFRESH, (BGPID_HOME|BGPID_MENU));
		}

	

	if(option == OSD_REFRESH )
		pMpegDecoder->RefreshOsd();
	
}
void CFileBrowser::ReDraw()
{ 
	ReDraw(OSD_REFRESH); 
}

void CFileBrowser::Refresh(eOSD_REFRESH_OPTION option)
{
	DEBUGMSG(1,("CFileBrowser::Refresh()\n"));
	ClearFileItem(_SKIP_CURSOR);
//	ScanDir( DynamicTitle, 1 );
	if(IsThisFatFs() )
		ScanDir( DynamicTitle, -1 );
	else
		ScanDir_linuxfs( CurrentFullPathDir );
	
	if( option == SKIP_OSD_REFRESH )
		ReDraw(SKIP_OSD_REFRESH);
	else if( option == OSD_REFRESH )
		ReDraw(OSD_REFRESH);
}
void CFileBrowser::Refresh()
{
	Refresh( OSD_REFRESH ); 
}

int CFileBrowser::MsgProc( int ssTaskId, U32 ulMsgId, int ssParam1, int ssParam2 )
{
	TMsgEntry tMsgTemp;
	int checkpoint;
	int FullpathNameLength;

	DEBUGMSG(1,("[CFileBrowser::MsgProc] : ssParam1[0x%x]\n",  ssParam1));

	//////////////////////////////////////////////////////////////////////
	//
	//	CFileBrowser::MsgProc
	//	Touch Pad Message Process
	//

	if( ulMsgId == MSG_TOUCHPAD ) {
		if( ssParam1 == 1 ) {
			CursorDown();
			ReDraw(OSD_REFRESH);
			Reset_FileBrowserTimer();
		}
		else if( ssParam1 == -1 ) {
			CursorUp();
			ReDraw(OSD_REFRESH);
			Reset_FileBrowserTimer();
		}
		else if( ssParam2 == 1 ) {
			if( GetListItemCount() > 0 ) {
				Stop_FileBrowserTimer();
				FullpathNameLength =  ustrlen(CurrentFullPathDir) + GetCurrentListItemNameLength() + 1;
				DEBUGMSG(1,("-FullpathNameLength : %d\n", FullpathNameLength ));
				
				if( Select(OSD_REFRESH) )
					{
					pCp->SetFilename( Filename );
					return FILEBROWSER_RETURN_FILE_SELECTED;
					}			
				else
					{
					if(  pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf())  )
						Reset_FileBrowserTimer();
					return FILEBROWSER_RETURN_NORMAL;
					}
            }
        }
    }

	//////////////////////////////////////////////////////////////////////
	//
	//	CFileBrowser::MsgProc
	//	Key Message Process
	//
	else if( ulMsgId == MSG_BUTTON_UP )
		{

		switch( ssParam1 )
			{
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_UP:
				if( GetListItemCount() > 0 )
					{
					CursorUp();
					ReDraw(OSD_REFRESH);
					if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) )
						Reset_FileBrowserTimer();
					}
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_DOWN:
				if( GetListItemCount() > 0 )
					{
					CursorDown();
					ReDraw(OSD_REFRESH);
					if( pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf()) )
						Reset_FileBrowserTimer();
					}
				break;
				
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_LEFT:
				if( CursorLeft()==0) 
					{
					Reset_FileBrowserTimer();
					ReDraw(OSD_REFRESH);
					}
				break;
			case TAVI_KEY_EXIT:
				if( CursorLeft()==0) 
					{
					Reset_FileBrowserTimer();
					ReDraw(OSD_REFRESH);
					}
				else
					{
					SaveDirInfo();
					return FILEBROWSER_RETURN_EXIT;
					}
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_RIGHT:
			case TAVI_KEY_ENTER:
				if( GetListItemCount() > 0 )
					{
					Stop_FileBrowserTimer();
					FullpathNameLength =  ustrlen(CurrentFullPathDir) + GetCurrentListItemNameLength() + 1;
					DEBUGMSG(1,("-FullpathNameLength : %d\n", FullpathNameLength ));
					
					if( Select(OSD_REFRESH) )
						{
						pCp->SetFilename( Filename );
						return FILEBROWSER_RETURN_FILE_SELECTED;
						}			
					else
						{
						if(  pT->GetEndXPosOfTxtBuf() > (320-38-pT->GetStartXPosOfTxtBuf())  )
							Reset_FileBrowserTimer();
						return FILEBROWSER_RETURN_NORMAL;
						}
					}

				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_EQ:
				break;
						
			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_MENU:
				if( pmFileBrowser != NULL )
					{
					if( !pmFileBrowser->IsVisible() )
						{
						Stop_FileBrowserTimer();
						pmFileBrowser->Show();
						}
					}
				break;

			//////////////////////////////////////////////////////////////////////
			case TAVI_KEY_MODE:
			case TAVI_KEY_LMODE:
			case TAVI_KEY_SHUTDOWN:
			case TAVI_KEY_LEXIT:
				SaveDirInfo();
 				return FILEBROWSER_RETURN_EXIT;

				break;
			}
		}

	return FILEBROWSER_RETURN_NORMAL;
}

 
