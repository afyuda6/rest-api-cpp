#include "user.h"
#include <iostream>

void createUser(sqlite3* db, const std::string& name, int roleId) {
    std::string sql = "INSERT INTO users (name, role_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, roleId);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to insert user: " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }
}

std::vector<std::unordered_map<std::string, std::string>> getUsers(sqlite3* db) {
    std::vector<std::unordered_map<std::string, std::string>> users;
    std::string sql = "SELECT * FROM users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::unordered_map<std::string, std::string> user;
            user["id"] = std::to_string(sqlite3_column_int(stmt, 0));
            user["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            user["role_id"] = std::to_string(sqlite3_column_int(stmt, 2));
            users.push_back(user);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to fetch users: " << sqlite3_errmsg(db) << std::endl;
    }

    return users;
}