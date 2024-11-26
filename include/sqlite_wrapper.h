#ifndef SQLITE_WRAPPER_H
#define SQLITE_WRAPPER_H

#include <sqlite3.h>
#include <string>

sqlite3* openDatabase(const std::string& dbFile);
void closeDatabase(sqlite3* db);

#endif
