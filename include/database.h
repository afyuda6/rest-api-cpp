#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

sqlite3* openDatabase(const std::string& dbFile);
void closeDatabase(sqlite3* db);

#endif