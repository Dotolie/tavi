// Sqlite1.cpp: implementation of the CSqlite class.
//
//////////////////////////////////////////////////////////////////////

#include "sqlite1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSqlite::CSqlite()
{

}

CSqlite::~CSqlite()
{
        if (m_pdb)
        {
                Close();
        }
}

int CSqlite::Open(const char *zFilename, int mode, char **pzErrMsg)
{
        m_pdb = sqlite_open(zFilename, 0, pzErrMsg);

        // when failure
        if (m_pdb == 0)
                return 0;
        // when success
        else
                return 1;
}

int CSqlite::Exec(const char *zSql, sqlite_callback xCallback, 
                                  void *pArg, char **pzErrMsg)
{
        return sqlite_exec(m_pdb, zSql, xCallback, pArg, pzErrMsg);
}

void CSqlite::Close()
{
        sqlite_close(m_pdb);
}

