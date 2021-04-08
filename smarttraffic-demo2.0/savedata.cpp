#include "sqliteTest.h"
#include <iostream>
#include <sstream>  
#include <ctime>

using namespace sqlite;
using namespace std;

Sqlite::Sqlite()
{
    //open or create default db
    try {
        sqlite3_open(this->defdb.c_str(), &pdb);
        cout << "already open the defdb" << endl;
        cout << this->tableName << endl;
        createTable();
    } catch(exception &ex) {
        cout << "error:" << ex.what() << endl;
    }
    catch(...) { }
}

Sqlite::Sqlite(string dbPath)
{
    try {
        this->defdb = dbPath;
        sqlite3_open(dbPath.c_str(), &pdb);
    } catch(exception &ex) {
        cout << "error:" << ex.what() << endl;
    } catch(...) { } 
}

//callback function will be excute once or more;
static int tableIsExist_cb(void *data,int argc, char **argv, char **azColName)
{
    *(int *)data = atoi(argv[0]);
    cout << "rungsdfdsfdsfdsf : argc = " << argc << argv[0] << azColName[0] << endl;
    return 0;
}

bool Sqlite::tableIsExist(string str)
{
    char *zErrMsg = 0;
    std::ostringstream check;
    cout << "string = " << str << endl;
    check << "SELECT * FROM SQLITE_MASTER WHERE TYPE = \"TABLE\" AND NAME = \""  \
                << str << "\";";
    static int existCount = 0;

    if((sqlite3_exec(this->pdb, check.str().c_str(), tableIsExist_cb, &existCount, &zErrMsg)) != SQLITE_OK) {
#ifdef DEBUG
        fprintf(stderr, "SQL table don`t exist: %s\n", zErrMsg);
#endif // DEBUG
        existCount = 0;
        sqlite3_free(zErrMsg);
        goto deal;
    } 
#ifdef DEBUG
    fprintf(stderr, "SQL table exist\n");
#endif // DEBUG
deal:
    cout << "existCount = " << existCount << endl;
    if(existCount != 0) {
        cout << "the table " << str << "aleady exists";
        return true;
    } 
    return false;  
}

//create a table 
bool Sqlite::createTable()
{
    // per month create another table,first table is named "_202103" 
    std::ostringstream createStr,timeStr;
    char *zErrMsg = 0;
    if(this->tableName.empty())
    {
        cout << "don`t allow that table name is NULL" << endl;
        return false;
    }

    time_t now = time(0); 
    tm *ltm = localtime(&now);
    //table name was named by the start of "_";
    timeStr <<"_" <<  1900 + ltm->tm_year << 1 + ltm->tm_mon;
    cout << timeStr.str() << endl;
    if(this->tableName.compare(timeStr.str()) != 0) {
        cout << "table name don`t diff" << endl;
        this->tableName  = timeStr.str();
        cout << this->tableName << endl;
        goto create;
    } else {
        return false;
    }

create :
    if(!tableIsExist(this->tableName)) {
        createStr << "CREATE TABLE " << this->tableName.c_str() 
            << " (ID INTEGER  PRIMARY KEY AUTOINCREMENT NOT NULL,\
            PATH TEXT NOT NULL,\
            NAME TEXT NOT NULL,\
            PRETIME INT,\
            LASTTIME INT,\
            LICENSE TEXT NOT NULL);";

        if ((sqlite3_exec(pdb, createStr.str().c_str(), NULL, NULL, &zErrMsg)) != SQLITE_OK /* condition */)
        {
            /* code */
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return false;
        } else {
            cout << "Table created successfully" << endl;
        }
    }  else {
        cout << "table " << this->tableName << "already exit" << endl;
        return true;
    }
}

bool Sqlite::insertData(DataProerty *user_data)
{
    // judgement the col is exist or not by the llicense;not exist,
    //exit,judgement per time and lasttime is NULL or not  defore insert data when select data exist ,
    //sure that update data in NULL position;
    std::ostringstream add;
    char *zErrMsg;
    cout << "asdsssssssssssssssssd" << endl;
    cout << user_data->id << user_data->path << user_data->name << user_data->preTime << user_data->lastTime << user_data->license << endl;
    cout << this->tableName << endl;
    add << "INSERT INTO "<<  this->tableName \
            << " (PATH,NAME,PRETIME,LICENSE) "\
            << "VALUES (\""\
            << user_data->path << "\",\""\
            << user_data->name << "\", "\
            << user_data->preTime << ", \"" \
            << user_data->license << "\");";
    cout << "add statements:" << add.str() << endl;
        
   if((sqlite3_exec(this->pdb, add.str().c_str(), 0, 0, &zErrMsg)) != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      cout << "add data failed" << endl;
      return false;
   } else {
        cout << "Records created successfully" << endl;
        return true;
   }
}

bool Sqlite::updateData(DataProerty *user_data)
{
    std::ostringstream update;
    char *zErrMsg = 0;
    int updateID;

    /* Create merged SQL statement */
    updateID = this->selectData(user_data);
    update << "UPDATE " << this->tableName << " set LASTTIME = " << user_data->lastTime \
        << " where LICENSE = \"" << user_data->license << "\" AND ID = " << user_data->id << " ; ";

    /* Execute SQL statement */
    cout << __LINE__ << ____FILE_defined << endl;
    if( (sqlite3_exec(this->pdb, update.str().c_str(), 0, 0, &zErrMsg)) != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    fprintf(stdout, "update data done\n");
    return true;

}

static int selsectData_cb(void *data, int argc, char **argv, char **colName)
{
    *(int *)data = atoi(argv[0]);
    return 0;
}

int Sqlite::selectData(DataProerty *user_data)
{
    std::ostringstream query;
    char *zErrMsg;
    static int retID;

    query << "SELECT * FROM "<< this->tableName << " WHERE LASTTIME IS NULL AND LICENSE = \"" << user_data->license << "\";";
    if( (sqlite3_exec(pdb, query.str().c_str(), selsectData_cb, &retID, &zErrMsg)) != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        cout << "select data failed" << endl;
        return -1;
   }
   cout << "retID = " << retID << endl;
    return retID;

}

ListData *Sqlite::getColData(/* colEnum col */)
{
    std::ostringstream query;
    sqlite3_stmt *pstmt;
    ListData *user_data,*ret_data;
    char *zErrMsg;
    char static *coldata;
    int ret = 0;

    user_data = (ListData *)malloc(sizeof(ListData));
    ret_data = user_data;
    user_data->id = 0;
    user_data->name = NULL;
    user_data->next =  (ListData *)malloc(sizeof(ListData));
    query << "SELECT ID,NAME FROM " << this->tableName;
    ret = sqlite3_prepare(this->pdb,query.str().c_str(),query.str().size(),&pstmt,NULL);
    if (ret != SQLITE_OK) {
		perror("sqlite3_prepare_v2 error:");
	}
    ret = sqlite3_step(pstmt);
    do{
        int i;
        int cNum =0 ;
        cNum = sqlite3_column_count(pstmt);

        int iType; 
        for (i = 0; i < cNum; i++) {
            iType = sqlite3_column_type(pstmt, i);
            if (iType == SQLITE_INTEGER) {
				int iValue;
				iValue = sqlite3_column_int(pstmt, i);
                user_data->next->id = int(iValue);
				printf("%d\t", iValue);
			} else if (iType == SQLITE_TEXT) {
                const unsigned char *iValue;
                iValue = sqlite3_column_text(pstmt, i);
                user_data->next->name = iValue;
                printf("%s\n", iValue);
                //iValue++;
             }
         }
         user_data->next->next = (ListData *)malloc(sizeof(ListData));
         user_data = user_data->next;
         ret = sqlite3_step(pstmt);
    } while(ret != SQLITE_DONE);
    user_data->id = -1;
    user_data->name = NULL;
    user_data->next = NULL;

    return ret_data;
}

Sqlite::~Sqlite()
{
    sqlite3_close(this->pdb);  
}