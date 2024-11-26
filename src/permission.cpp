#include "permission.h"
#include <iostream>

void createPermission(sqlite3* db, const std::string& name, int roleId) {
    std::string sql = "INSERT INTO permissions (name, role_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, roleId);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to insert permission: " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }
}

std::vector<std::unordered_map<std::string, std::string>> getPermissions(sqlite3* db) {
    std::vector<std::unordered_map<std::string, std::string>> permissions;
    std::string sql = "SELECT * FROM permissions;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::unordered_map<std::string, std::string> permission;
            permission["id"] = std::to_string(sqlite3_column_int(stmt, 0));
            permission["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            permission["role_id"] = std::to_string(sqlite3_column_int(stmt, 2));
            permissions.push_back(permission);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to fetch permissions: " << sqlite3_errmsg(db) << std::endl;
    }

    return permissions;
}