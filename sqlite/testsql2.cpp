#include <stdio.h>
#include "sqlite3.h"
static int select_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    char* szName = argv[0];
 
    printf("%s\n", szName);
    return 0;
 
}
 
    
 
int main(int argc, char* argv[])
{
    sqlite3 *db;
 
    sqlite3_open(":memory:", &db); 
 
    sqlite3_exec(db, "create table test (name varchar(100))", NULL, NULL, NULL);
 
    char caBuffer[5000];
    
    sqlite3_stmt *pStat;
    char* pTail;
    sqlite3_prepare(db, "insert into test values(?)", strlen("insert into test values(,?,)"), &pStat, (const char**)&pTail);
    
    for(int i=0;i<10000;i++)
    {       
        sprintf(caBuffer, "±æ°Ô~~~ %d", i);
        
        sqlite3_bind_text(pStat, 1, caBuffer, strlen(caBuffer)+2, SQLITE_STATIC);
        sqlite3_step(pStat);
 
        sqlite3_reset(pStat);
    }
 
    sqlite3_finalize(pStat);
 
    sqlite3_exec(db, "select * from test", select_callback, NULL, NULL);
    sqlite3_close(db);
    
    return 0;
}

