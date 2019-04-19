#ifndef __IDDB_OP_H__
#define __IDDB_OP_H__

#include "IDDB.h"

typedef void (*ProgressFN)( bool state, const char* path, int count );

extern int IDDB_Update( IDDB_Desc* hDB, const uchar_t* pathname, ProgressFN fn );

#endif /* __IDDB_OP_H__ */
