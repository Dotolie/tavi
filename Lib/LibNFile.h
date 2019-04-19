//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : NFile Abstraction (Python-style File Object)
//
// $Source: /home/cvsroot/TAVI020/tavi/Lib/LibNFile.h,v $
// $Revision: 1.2 $
// $Date: 2006/04/17 04:26:09 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: LibNFile.h,v $
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

#ifndef _NML_FILE_H_
#define _NML_FILE_H_

#include "rmbasic.h"
#include "fatfs.h"
#include "Component/String.h"

class NFile {
private:
	RMuint32	handle;
	FATFS_ERROR	lastError;
	RMuint32	parseFlags(const char *mode);

public:
	NFile(char *filename, const char *mode);
	NFile(RMuint16 *filename, const char *mode);
	NFile(CString &filename, const char *mode);
	NFile(CString *filename, const char *mode);
	~NFile();
	int open(char *filename, const char *mode);
	int open(RMuint16 *filename, const char *mode);
	int getHandle(void);
	int getLastError(void);
	int read(unsigned char *buf, int howmany);
	int close(void);
	int write(unsigned char *buf, int howmany);
	int eof(void);
	int size(void);
	long long size64(void);
	int tell(void);
	int seek(int pos, int flag);
	int seek64(long long pos, int flag);

    static bool Exist( const char* dir, const char* findname, FATFS_DIR_ENTRY_INFO& info );
};

#endif
