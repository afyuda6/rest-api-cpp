#include "http_server.h"
#include "role.h"
#include "user.h"
#include "permission.h"
#include "sqlite_wrapper.h"
#include <sstream>
#include "database.h"

void initializeDatabase();

std::string apiHandler(const std::string& path, const std::unordered_map<std::string, std::string>& params) {
    sqlite3* db = openDatabase("rest_api_cpp.db");
    if (!db) {
        return R"({"error": "Database connection failed"})";
    }

    if (path == "/roles") {
        if (params.find("name") != params.end()) {
            // Create role
            createRole(db, params.at("name"));
            closeDatabase(db);
            return R"({"status": "Role created"})";
        }
        // Get roles
        auto roles = getRoles(db);
        std::ostringstream jsonResponse;
        jsonResponse << "[";
        for (size_t i = 0; i < roles.size(); ++i) {
            const auto& role = roles[i];
            jsonResponse << "{"
                    << "\"id\": \"" << role.at("id") << "\", "
                    << "\"name\": \"" << role.at("name") << "\""
                    << "}";
            if (i < roles.size() - 1) {
                jsonResponse << ", ";
            }
        }
        jsonResponse << "]";
        closeDatabase(db);
        return jsonResponse.str();
    }
    if (path == "/users") {
        if (params.find("name") != params.end() && params.find("role_id") != params.end()) {
            // Create user
            int roleId = std::stoi(params.at("role_id")); // Convert string to int
            createUser(db, params.at("name"), roleId);
            closeDatabase(db);
            return R"({"status": "User created"})";
        }
        // Get users
        auto users = getUsers(db);
        std::ostringstream jsonResponse;
        jsonResponse << "[";
        for (size_t i = 0; i < users.size(); ++i) {
            const auto& user = users[i];
            jsonResponse << "{"
                    << "\"id\": \"" << user.at("id") << "\", "
                    << "\"name\": \"" << user.at("name") << "\", "
                    << "\"role_id\": \"" << user.at("role_id") << "\""
                    << "}";
            if (i < users.size() - 1) {
                jsonResponse << ", ";
            }
        }
        jsonResponse << "]";
        closeDatabase(db);
        return jsonResponse.str();
    }
    if (path == "/permissions") {
        if (params.find("name") != params.end() && params.find("role_id") != params.end()) {
            try {
                int roleId = std::stoi(params.at("role_id"));  // Convert string to int
                createPermission(db, params.at("name"), roleId);
                closeDatabase(db);
                return R"({"status": "Permission created"})";
            } catch (const std::exception& e) {
                closeDatabase(db);
                return R"({"error": "Invalid role_id"})";
            }
        }
        // Get permissions
        auto permissions = getPermissions(db);
        std::ostringstream jsonResponse;
        jsonResponse << "[";
        for (size_t i = 0; i < permissions.size(); ++i) {
            const auto& permission = permissions[i];
            jsonResponse << "{"
                    << "\"id\": \"" << permission.at("id") << "\", "
                    << "\"name\": \"" << permission.at("name") << "\""
                    << "}";
            if (i < permissions.size() - 1) {
                jsonResponse << ", ";
            }
        }
        jsonResponse << "]";
        closeDatabase(db);
        return jsonResponse.str();
    }

    closeDatabase(db);
    return R"({"error": "Invalid endpoint"})";
}

int main() {
    initializeDatabase();

    // Start the server
    startServer(8080, apiHandler);

    return 0;
}