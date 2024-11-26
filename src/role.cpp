#include "role.h"
#include <iostream>

void createRole(sqlite3* db, const std::string& name) {
    std::string sql = "INSERT INTO roles (name) VALUES (?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to insert role: " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }
}

std::vector<std::unordered_map<std::string, std::string>> getRoles(sqlite3* db) {
    std::vector<std::unordered_map<std::string, std::string>> roles;
    std::string sql = "SELECT * FROM roles;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::unordered_map<std::string, std::string> role;
            role["id"] = std::to_string(sqlite3_column_int(stmt, 0));
            role["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            roles.push_back(role);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to fetch roles: " << sqlite3_errmsg(db) << std::endl;
    }

    return roles;
}