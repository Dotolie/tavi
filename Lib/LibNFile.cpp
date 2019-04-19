//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : NFile Abstraction (Python-style File Object)
//
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibNFile.cpp,v $
// $Revision: 1.2 $
// $Date: 2006/04/17 04:26:09 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: LibNFile.cpp,v $
// Revision 1.2  2006/04/17 04:26:09  zstein
// - added Exist() methods.
//
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.1  2005/10/29 06:18:17  yong
// NFile class: simple abstraction for file in Python-style
//
//*****************************************************************************

/******************************************************************************

	Usage:

	0) Basic
		NFile aFile = NFile("/hd1/xxx.db", "r");
		aFile.Close();

	1) To read from/write to a file:

		unsigned char	buf[1024];
		int				nRead, nWritten;

		NFile aFile = NFile("/hd1/xxx.db", "r");
		nRead = aFile.read(buf, sizeof(buf));
		nWritten = aFile.write(buf, nRead);


******************************************************************************/

#include "LibNFile.h"
#include "string.h"

typedef struct {
	char *cFlags;
	RMuint32	nFlags;
} _FILE_MODE_MAP_;

RMuint32
NFile::parseFlags(const char *mode)
{
	/*
		#define _O_RDONLY               0x0000  // open for reading only
		#define _O_WRONLY               0x0001  // open for writing only
		#define _O_RDWR                 0x0002  // NOT IMPLEMENTED !!!
		#define _O_APPEND               0x0008  // writes done at eof
		#define _O_CREAT                0x0100  // create and open file
		#define _O_TRUNC                0x0200  // open and truncate
		#define _O_EXCL                 0x0400  // open only if file doesn't already exist
		#define _O_CORRUPTED            0x1000  // open corrupted file
		#define _O_FIX_CORRUPTED        0x2000  // open and fix corrupted file

		Flag mapping:

		"r"		==> _O_RDONLY
		"w"		==> _O_WRONLY
		"W"		==> _O_WRONLY|_O_TRUNC
		"c"		==> _O_WRONLY | _O_CREAT
		"C"		==> _O_WRONLY|_O_CREAT|_O_TRUNC
		"a"		==> _O_WRONLY | _O_APPEND
	*/
	static _FILE_MODE_MAP_ modeMap[] = {
		{"r", _O_RDONLY},
		{"w", _O_WRONLY},
		{"W", _O_WRONLY|_O_TRUNC},
		{"c", _O_WRONLY | _O_CREAT},
		{"C", _O_WRONLY|_O_CREAT|_O_TRUNC},
		{"a", _O_WRONLY | _O_APPEND},
		{"", 0},	// sentinel
	};

	RMuint32	flags = _O_RDONLY;	// "r"

	for (int i=0; ;i++) {
		if (strcmp(mode, modeMap[i].cFlags) == 0) {	// found mode match
			flags = modeMap[i].nFlags;
			break;
		}
		if (strcmp("", modeMap[i].cFlags) == 0) {	// look for sentinel
			break;
		}
	}

	return flags;
}

NFile::NFile(char *pathname, const char *mode="r")
{
	this->lastError	= FATFS_ERROR_NO_ERROR;
	this->open(pathname, mode);
}

NFile::NFile(RMuint16 *pathname, const char *mode="r")
{
	this->lastError	= FATFS_ERROR_NO_ERROR;
	this->open(pathname, mode);
}

NFile::NFile(CString &filename, const char *mode="r")
{
	printf("NFile(CString &)\n");
	this->lastError	= FATFS_ERROR_NO_ERROR;
	if (filename.IsUnicode() ) {
		this->open((RMuint16 *)filename.GetString(), mode);
	} else {
		this->open((char *)filename.GetString(), mode);
	}
}

NFile::NFile(CString *filename, const char *mode="r")
{
	printf("NFile(CString *)\n");
	this->lastError	= FATFS_ERROR_NO_ERROR;
	if (filename->IsUnicode() ) {
		this->open((RMuint16 *)filename->GetString(), mode);
	} else {
		this->open((char *)filename->GetString(), mode);
	}
}

int
NFile::open(char *pathname, const char *mode="r")
{
	RMuint32	retHandle;
	RMuint32	flags = this->parseFlags(mode);

	this->lastError = fatfs_fopen((RMuint8 *)pathname, (RMuint32)flags, &retHandle);

	if (this->lastError == 0) {
		this->handle = retHandle;
	} else {
		this->handle = 0;
	}

	printf("NFile(ascii) %d, %d\n", flags, this->lastError);
	return this->handle;
}

int
NFile::open(RMuint16 *pathname, const char *mode="r")
{
	/*
	FATFS_ERROR fatfs_wfopen(RMuint16 *pDrvPathFileNameUorW, RMuint32 dwOpenFlags, RMuint32 *pnRetHandle);
	*/
	RMuint32	retHandle;
	RMuint32	flags = this->parseFlags(mode);

	this->lastError = fatfs_wfopen(pathname, (RMuint32)flags, &retHandle);

	if (this->lastError == 0) {
		this->handle = retHandle;
	} else {
		this->handle = 0;
	}

	printf("NFile(utf-16) %d, %d\n", flags, this->lastError);
	return this->handle;
}

NFile::~NFile()
{
	this->close();
}

int
NFile::getHandle(void)
{
	return (int)this->handle;
}

int
NFile::getLastError(void)
{
	return (int)this->lastError;
}

int
NFile::read(unsigned char *buf, int howmany)
{
	/*
	FATFS_ERROR fatfs_fread(
		RMuint32 nFileHandle,
		RMuint8 *pbBufUser,
		RMuint32 nBytesToRead,
		RMuint32 *pnBytesRead
	);
	*/
	RMuint32 nRead;
	if (this->handle <= 0) {
		return -2;
	}
	this->lastError = fatfs_fread(this->handle, (RMuint8 *)buf, (RMuint32)howmany, &nRead);
	if (this->lastError == 0) {
		return (int)nRead;
	} else {
		return -1;
	}
}

int
NFile::write(unsigned char *buf, int howmany)
{
	/*
	FATFS_ERROR fatfs_fwrite(
		RMuint32 nFileHandle,
		RMuint8 *pbBufUser,
		RMuint32 nBytesToWrite,
		RMuint32 *pnBytesWritten
	);
	*/
	RMuint32 nWritten;
	if (this->handle <= 0) {
		return -2;
	}
	this->lastError = fatfs_fwrite(this->handle, (RMuint8 *)buf, (RMuint32)howmany, &nWritten);
	if (this->lastError == 0) {
		return (int)nWritten;
	} else {
		return -1;
	}
}

int
NFile::close(void)
{
	if (this->handle > 0) {
		this->lastError = fatfs_fclose(this->handle);
		this->handle = 0;
		return (int)this->lastError;
	} else {
		return -2;
	}
}

int
NFile::eof(void)
{
	/*
	FATFS_ERROR fatfs_feof   ( RMuint32 nFileHandle, RMuint32 *pnFEOF );
	*/
	RMuint32	eof;

	if (this->handle > 0) {
		this->lastError = fatfs_feof(this->handle, &eof);
		if (this->lastError == 0) {
			return (eof ? 1:0);
		} else {
			return -1;
		}
	} else {
		return -2;
	}
}


int
NFile::size(void)
{
	/*
	FATFS_ERROR fatfs_fsize  ( RMuint32 nFileHandle, RMuint32 *pnFileSize );
	*/
	RMuint32	size;

	if (this->handle > 0) {
		this->lastError = fatfs_fsize(this->handle, &size);
		if (this->lastError == 0) {
			return (int)size;
		} else {
			return -1;
		}
	} else {
		return -2;
	}
}

long long
NFile::size64(void)
{
	/*
	FATFS_ERROR fatfs_fsize64( RMuint32 nFileHandle, RMint64 *pnFileSize );
	*/
	RMint64	size;

	if (this->handle > 0) {
		this->lastError = fatfs_fsize64(this->handle, &size);
		if (this->lastError == 0) {
			return (long long)size;
		} else {
			return -1;
		}
	} else {
		return -2;
	}
}

int
NFile::tell(void)
{
	/*
	FATFS_ERROR fatfs_ftell  ( RMuint32 nFileHandle, RMuint32 *pnInternalCurPos );
	*/
	RMuint32	curPos;

	if (this->handle > 0) {
		this->lastError = fatfs_ftell(this->handle, &curPos);
		if (this->lastError == 0) {
			return (int)curPos;
		} else {
			return -1;
		}
	} else {
		return -2;
	}
}

int
NFile::seek(int pos, int flag)
{
	/*
	FATFS_ERROR fatfs_fseek  ( RMuint32 nFileHandle, RMint32 nPosRel, RMint32 nFlagRel );
	*/

	if (this->handle > 0) {
		this->lastError = fatfs_fseek(this->handle, (RMint32)pos, (RMint32)flag);
		return (this->lastError == 0 ? 1: 0);
	} else {
		return -2;
	}
}

int
NFile::seek64(long long pos, int flag)
{
	/*
	FATFS_ERROR fatfs_fseek64( RMuint32 nFileHandle, RMint64 nPosRel, RMint32 nFlagRel );
	*/

	if (this->handle > 0) {
		this->lastError = fatfs_fseek64(this->handle, (RMint64)pos, (RMint32)flag);
		return (this->lastError == 0 ? 1: 0);
	} else {
		return -2;
	}
}


/* ++
    Exist( const char* dir, const char* findname, bool bdir, FATFS_DIR_ENTRY_INFO& info )

Description:
    Find a filename or directory name in given dir.

Arguments:

Return:
    It returns true if find a name, false on failure.

-- */
bool
NFile::Exist( const char* dir, const char* findname, FATFS_DIR_ENTRY_INFO& info )
{
    bool bsuccess = false;

	RMuint32 dirfd;
	FATFS_ERROR err;
	if( (err = fatfs_dopen( (RMuint8*)dir, &dirfd )) == FATFS_ERROR_NO_ERROR ) {
	    while( (err=fatfs_dread(dirfd, &info)) == FATFS_ERROR_NO_ERROR ) {
	        /*
	            Directory!! Check its name
	        */
	        if( !strcasecmp(findname, info.short_name) ) {
	            bsuccess = true;
	            break;
	        }
	    }
	}
	fatfs_dclose( dirfd ); 

    return bsuccess;
}

// END
