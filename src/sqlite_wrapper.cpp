#include "sqlite_wrapper.h"
#include <sqlite3.h>
#include <iostream>

sqlite3* openDatabase(const std::string& dbName) {
    sqlite3* db = nullptr;
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

void closeDatabase(sqlite3* db) {
    if (db) {
        sqlite3_close(db);
    }
}