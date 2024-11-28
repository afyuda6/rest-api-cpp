#include <iostream>
#include <sqlite3.h>

void initializeDatabase() {
    sqlite3* db;
    if (sqlite3_open("rest_api_cpp.db", &db)) {
        std::cerr << "SQL error (open database): " << sqlite3_errmsg(db) << std::endl;
    }

    if (db) {
        char *errMessage = nullptr;

        const char *dropUsersTableSQL = "DROP TABLE IF EXISTS users;";
        if (sqlite3_exec(db, dropUsersTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (drop users table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        const char *createUsersTableSQL = "CREATE TABLE IF NOT EXISTS users ("
                "id INTEGER PRIMARY KEY, "
                "name TEXT NOT NULL);";
        if (sqlite3_exec(db, createUsersTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (create users table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        sqlite3_close(db);
    }
}
