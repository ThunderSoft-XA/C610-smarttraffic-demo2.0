#ifndef __SQLITE_TEST_H__
#define __SQLITE_TEST_H__


#include <ctime>
#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;
namespace sqlite
{
typedef struct _DataProerty
{
        int id;
        string path;
        string name;
        long preTime;
        long lastTime;
        string license;
        /* data */
}DataProerty;

typedef struct _ListData 
{
    int id;
    const unsigned char *name;
    _ListData *next;
}ListData;

class Sqlite {
private:
    string defdb = string("./traffic.db");
    string tableName = string("_20212");
    sqlite3 *pdb;
    bool tableIsExist(string str);
    bool createTable();

public :
    Sqlite();
    Sqlite(string dbPath);
    ~Sqlite();
    //enum colEnum {ID = 0,PATH,NAME,PRETIME,LASTTIME,LICENSE};
    bool insertData(DataProerty *user_data);
    bool updateData(DataProerty *user_data);
    int selectData(DataProerty *user_data);
    ListData *getColData(/*colEnum col */);

};
    
}; // namespace sqlite




#endif // !__SQLITE_TEST_H__