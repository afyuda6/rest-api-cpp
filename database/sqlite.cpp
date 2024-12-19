#include <sqlite3.h>

void initializeDatabase() {
    sqlite3 *db;
    sqlite3_open("rest_api_cpp.db", &db);
    char *errMessage = nullptr;
    const char *dropUsersTableSQL = "DROP TABLE IF EXISTS users;";
    sqlite3_exec(db, dropUsersTableSQL, nullptr, nullptr, &errMessage);
    sqlite3_free(errMessage);
    const char *createUsersTableSQL = "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT NOT NULL);";
    sqlite3_exec(db, createUsersTableSQL, nullptr, nullptr, &errMessage);
    sqlite3_free(errMessage);
    sqlite3_close(db);
}
