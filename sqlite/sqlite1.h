#include <sqlite3.h>

class CSqlite  
{
public:
        CSqlite();
        virtual ~CSqlite();

        int Open(const char *zFilename, int mode, char **pzErrMsg);
        int Exec(const char *zSql, sqlite_callback xCallback, 
                void *pArg, char **pzErrMsg);
        void Close();

        sqlite *m_pdb;

};

