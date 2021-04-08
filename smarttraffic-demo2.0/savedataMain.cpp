#include "savedata.h"
#include <stdio.h>
#include <mcheck.h>

using namespace sqlite;
using namespace std;

int main(int argc, char **argv)
{
    setenv("MALLOC_TRACE", "mtrace.log", 1);
    mtrace();
    Sqlite *mySqlite = new Sqlite();
    DataProerty *data = new DataProerty({15, "~/workpace/","yue12345.png",1233,0,"yue12345"});

    mySqlite->insertData(data);
    mySqlite->updateData(data);
    mySqlite->getColData();

    delete(mySqlite);
    delete(data);

    return 0;
}