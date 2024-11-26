#include <iostream>
#include "sqlite_wrapper.h"

void initializeDatabase() {
    sqlite3 *db = openDatabase("rest_api_cpp.db");
    if (db) {
        // Drop tables if they exist and recreate them
        const char *dropRolesTableSQL = "DROP TABLE IF EXISTS roles;";
        char *errMessage = nullptr;
        if (sqlite3_exec(db, dropRolesTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (drop roles table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        const char *createRolesTableSQL = "CREATE TABLE IF NOT EXISTS roles (id INTEGER PRIMARY KEY, name TEXT);";
        if (sqlite3_exec(db, createRolesTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (create roles table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        // Drop and create users table
        const char *dropUsersTableSQL = "DROP TABLE IF EXISTS users;";
        if (sqlite3_exec(db, dropUsersTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (drop users table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        const char *createUsersTableSQL = "CREATE TABLE IF NOT EXISTS users ("
                "id INTEGER PRIMARY KEY, "
                "name TEXT NOT NULL, "
                "role_id INTEGER, "
                "FOREIGN KEY(role_id) REFERENCES roles(id));";
        if (sqlite3_exec(db, createUsersTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (create users table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        // Drop and create permissions table
        const char *dropPermissionsTableSQL = "DROP TABLE IF EXISTS permissions;";
        if (sqlite3_exec(db, dropPermissionsTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (drop permissions table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        const char *createPermissionsTableSQL = "CREATE TABLE IF NOT EXISTS permissions ("
                "id INTEGER PRIMARY KEY, "
                "name TEXT NOT NULL, "
                "role_id INTEGER, "
                "FOREIGN KEY(role_id) REFERENCES roles(id));";
        if (sqlite3_exec(db, createPermissionsTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (create permissions table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        // Drop and create user_permissions junction table
        const char *dropUserPermissionsTableSQL = "DROP TABLE IF EXISTS user_permissions;";
        if (sqlite3_exec(db, dropUserPermissionsTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (drop user_permissions table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        const char *createUserPermissionsTableSQL = "CREATE TABLE IF NOT EXISTS user_permissions ("
                "user_id INTEGER, "
                "permission_id INTEGER, "
                "FOREIGN KEY(user_id) REFERENCES users(id), "
                "FOREIGN KEY(permission_id) REFERENCES permissions(id), "
                "PRIMARY KEY(user_id, permission_id));";
        if (sqlite3_exec(db, createUserPermissionsTableSQL, nullptr, nullptr, &errMessage) != SQLITE_OK) {
            std::cerr << "SQL error (create user_permissions table): " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }

        closeDatabase(db);
    }
}
