#include <sqlite3.h>
#include <iostream>

sqlite3* openDatabase(const std::string& dbFile) {
    sqlite3* db;
    if (sqlite3_open(dbFile.c_str(), &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

void closeDatabase(sqlite3* db) {
    sqlite3_close(db);
}
