#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void parseFormData(const std::string &body, std::unordered_map<std::string, std::string> &params) {
    std::istringstream bodyStream(body);
    std::string key_value;
    while (std::getline(bodyStream, key_value, '&')) {
        size_t equalPos = key_value.find('=');
        if (equalPos != std::string::npos) {
            std::string key = key_value.substr(0, equalPos);
            std::string value = key_value.substr(equalPos + 1);
            params[key] = value;
        }
    }
}

std::string trim(const std::string &str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        ++start;
    }
    auto end = str.end();
    while (end != start && std::isspace(*(end - 1))) {
        --end;
    }
    return std::string(start, end);
}

void handleReadUsers(sqlite3 *db, const int clientSocket) {
    std::string query = "SELECT id, name FROM users;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    std::ostringstream responseStream;
    responseStream <<
            "HTTP/1.1 200 OK\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"status\": \"OK\", \"code\": 200, \"data\": [";
    bool first = true;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (!first) {
            responseStream << ",";
        }
        first = false;
        responseStream << "{\"id\": " << sqlite3_column_int(stmt, 0) << ", \"name\": \"" << sqlite3_column_text(stmt, 1)
                << "\"}";
    }
    responseStream << "]}";
    sqlite3_finalize(stmt);
    std::string response = responseStream.str();
    send(clientSocket, response.c_str(), response.length(), 0);
}

void handleCreateUser(const std::string &request, sqlite3 *db, const int clientSocket) {
    std::string body = request.substr(request.find("\r\n\r\n") + 4);
    std::unordered_map<std::string, std::string> params;
    parseFormData(body, params);
    if (params.find("name") != params.end()) {
        std::string name = params.at("name");
        if (trim(name).empty() || name == "+") {
            std::string response =
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                    "Access-Control-Allow-Headers: Content-Type\r\n"
                    "Content-Type: application/json\r\n\r\n"
                    "{\"status\": \"Bad Request\", \"code\": 400, \"errors\": \"Missing 'name' parameter\"}";
            send(clientSocket, response.c_str(), response.length(), 0);
        } else {
            std::string sql = "INSERT INTO users (name) VALUES ('" + name + "');";
            sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
            std::string response =
                    "HTTP/1.1 201 Created\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                    "Access-Control-Allow-Headers: Content-Type\r\n"
                    "Content-Type: application/json\r\n\r\n"
                    "{\"status\": \"Created\", \"code\": 201}";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    } else {
        std::string response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\": \"Bad Request\", \"code\": 400, \"errors\": \"Missing 'name' parameter\"}";
        send(clientSocket, response.c_str(), response.length(), 0);
    }
}

void handleUpdateUser(const std::string &request, sqlite3 *db, const int clientSocket) {
    std::string body = request.substr(request.find("\r\n\r\n") + 4);
    std::unordered_map<std::string, std::string> params;
    parseFormData(body, params);
    if (params.find("id") != params.end() && params.find("name") != params.end()) {
        std::string id = params.at("id");
        std::string name = params.at("name");
        if (trim(name).empty() || trim(id).empty() || name == "+" || id == "+") {
            std::string response =
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                    "Access-Control-Allow-Headers: Content-Type\r\n"
                    "Content-Type: application/json\r\n\r\n"
                    "{\"status\": \"Bad Request\", \"code\": 400, \"errors\": \"Missing 'id' or 'name' parameter\"}";
            send(clientSocket, response.c_str(), response.length(), 0);
        } else {
            std::string sql = "UPDATE users SET name = '" + name + "' WHERE id = " + id + ";";
            sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
            std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                    "Access-Control-Allow-Headers: Content-Type\r\n"
                    "Content-Type: application/json\r\n\r\n"
                    "{\"status\": \"OK\", \"code\": 200}";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    } else {
        std::string response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\": \"Bad Request\", \"code\": 400, \"errors\": \"Missing 'id' or 'name' parameter\"}";
        send(clientSocket, response.c_str(), response.length(), 0);
    }
}

void handleDeleteUser(const std::string &request, sqlite3 *db, const int clientSocket) {
    std::string body = request.substr(request.find("\r\n\r\n") + 4);
    std::unordered_map<std::string, std::string> params;
    parseFormData(body, params);
    if (params.find("id") != params.end()) {
        std::string id = params.at("id");
        if (trim(id).empty() || id == "+") {
            std::string response =
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                    "Access-Control-Allow-Headers: Content-Type\r\n"
                    "Content-Type: application/json\r\n\r\n"
                    "{\"status\": \"Bad Request\", \"code\": 400, \"errors\": \"Missing 'id' parameter\"}";
            send(clientSocket, response.c_str(), response.length(), 0);
        } else {
            std::string sql = "DELETE FROM users WHERE id = " + id + ";";
            sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
            std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                    "Access-Control-Allow-Headers: Content-Type\r\n"
                    "Content-Type: application/json\r\n\r\n"
                    "{\"status\": \"OK\", \"code\": 200}";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    } else {
        std::string response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\": \"Bad Request\", \"code\": 400, \"errors\": \"Missing 'id' parameter\"}";
        send(clientSocket, response.c_str(), response.length(), 0);
    }
}

void userHandler(const std::string &request, const std::unordered_map<std::string, std::string> &params,
                 int clientSocket) {
    sqlite3 *db;
    sqlite3_open("rest_api_cpp.db", &db);

    if (request.find("OPTIONS") == 0) {
        std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
    } else {
        size_t usersPos = request.find("/users");
        if (usersPos != std::string::npos &&
            (usersPos + 6 == request.size() ||
             request[usersPos + 6] == ' ' ||
             request[usersPos + 6] == '?' ||
             (request[usersPos + 6] == '/' &&
              (usersPos + 7 == request.size() || request[usersPos + 7] == ' ')))) {
            if (request.find("GET") == 0) {
                handleReadUsers(db, clientSocket);
            } else if (request.find("POST") == 0) {
                handleCreateUser(request, db, clientSocket);
            } else if (request.find("PUT") == 0) {
                handleUpdateUser(request, db, clientSocket);
            } else if (request.find("DELETE") == 0) {
                handleDeleteUser(request, db, clientSocket);
            } else {
                std::string response =
                        "HTTP/1.1 405 Method Not Allowed\r\n"
                        "Access-Control-Allow-Origin: *\r\n"
                        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                        "Access-Control-Allow-Headers: Content-Type\r\n"
                        "Content-Type: application/json\r\n\r\n"
                        "{\"status\": \"Method Not Allowed\", \"code\": 405}";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        } else {
            std::string response =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                    "Access-Control-Allow-Headers: Content-Type\r\n"
                    "Content-Type: application/json\r\n\r\n"
                    "{\"status\": \"Not Found\", \"code\": 404}";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    sqlite3_close(db);
}
