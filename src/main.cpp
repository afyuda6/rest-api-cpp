#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <winsock2.h>
#include <sqlite3.h>
#include "database.h"
#include "sqlite_wrapper.h"

#pragma comment(lib, "ws2_32.lib")

void initializeDatabase();

void parseFormData(const std::string& body, std::unordered_map<std::string, std::string>& params) {
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

void handleRequest(const std::string& request, const std::unordered_map<std::string, std::string>& params, SOCKET clientSocket) {
    sqlite3* db = openDatabase("rest_api_cpp.db");
    if (!db) {
        std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nDatabase connection failed!";
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    if (request.find("GET") == 0) {
        if (request.find("/users") != std::string::npos) {
            std::string query = "SELECT id, name FROM users;";
            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

            std::ostringstream responseStream;
            responseStream << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n[";
            bool first = true;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                if (!first) {
                    responseStream << ",";
                }
                first = false;
                responseStream << "{\"id\": " << sqlite3_column_int(stmt, 0)
                               << ", \"name\": \"" << sqlite3_column_text(stmt, 1) << "\"}";
            }
            responseStream << "]";
            sqlite3_finalize(stmt);

            std::string response = responseStream.str();
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (request.find("/roles") != std::string::npos) {
            std::string query = "SELECT id, name FROM roles;";
            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

            std::ostringstream responseStream;
            responseStream << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n[";
            bool first = true;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                if (!first) {
                    responseStream << ",";
                }
                first = false;
                responseStream << "{\"id\": " << sqlite3_column_int(stmt, 0)
                               << ", \"name\": \"" << sqlite3_column_text(stmt, 1) << "\"}";
            }
            responseStream << "]";
            sqlite3_finalize(stmt);

            std::string response = responseStream.str();
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (request.find("/permissions") != std::string::npos) {
            std::string query = "SELECT id, name FROM permissions;";
            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

            std::ostringstream responseStream;
            responseStream << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n[";
            bool first = true;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                if (!first) {
                    responseStream << ",";
                }
                first = false;
                responseStream << "{\"id\": " << sqlite3_column_int(stmt, 0)
                               << ", \"name\": \"" << sqlite3_column_text(stmt, 1) << "\"}";
            }
            responseStream << "]";
            sqlite3_finalize(stmt);

            std::string response = responseStream.str();
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nInvalid endpoint!";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("POST") == 0) {
        if (request.find("/users") != std::string::npos) {
            std::string body = request.substr(request.find("\r\n\r\n") + 4);  // Extract the POST body
            std::unordered_map<std::string, std::string> params;
            parseFormData(body, params);  // Parse the form data from the POST body

            if (params.find("name") != params.end()) {
                std::string name = params.at("name");
                std::string sql = "INSERT INTO users (name) VALUES ('" + name + "');";
                sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

                std::string response = "HTTP/1.1 201 Created\r\nContent-Type: text/plain\r\n\r\nUser created!";
                send(clientSocket, response.c_str(), response.length(), 0);
            } else {
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing 'name' parameter!";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        } else if (request.find("/roles") != std::string::npos) {
            // Similar logic for roles
            std::string body = request.substr(request.find("\r\n\r\n") + 4);
            std::unordered_map<std::string, std::string> params;
            parseFormData(body, params);
            if (params.find("name") != params.end()) {
                std::string name = params.at("name");
                std::string sql = "INSERT INTO roles (name) VALUES ('" + name + "');";
                sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
                std::string response = "HTTP/1.1 201 Created\r\nContent-Type: text/plain\r\n\r\nRole created!";
                send(clientSocket, response.c_str(), response.length(), 0);
            } else {
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing 'name' parameter!";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        } else if (request.find("/permissions") != std::string::npos) {
            // Similar logic for permissions
            std::string body = request.substr(request.find("\r\n\r\n") + 4);
            std::unordered_map<std::string, std::string> params;
            parseFormData(body, params);
            if (params.find("name") != params.end()) {
                std::string name = params.at("name");
                std::string sql = "INSERT INTO permissions (name) VALUES ('" + name + "');";
                sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
                std::string response = "HTTP/1.1 201 Created\r\nContent-Type: text/plain\r\n\r\nPermission created!";
                send(clientSocket, response.c_str(), response.length(), 0);
            } else {
                std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing 'name' parameter!";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
    }
    else if (request.find("PUT") == 0) {
        // Handle PUT request - update users, roles, or permissions
        if (request.find("/users") != std::string::npos) {
            // Update user logic (updating the database)
            std::string id = params.at("id");
            std::string name = params.at("name");
            std::string sql = "UPDATE users SET name = '" + name + "' WHERE id = " + id + ";";
            sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nUser updated!";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("DELETE") == 0) {
        // Handle DELETE request - delete users, roles, or permissions
        if (request.find("/users") != std::string::npos) {
            // Delete user logic (deleting from the database)
            std::string id = params.at("id");
            std::string sql = "DELETE FROM users WHERE id = " + id + ";";
            sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nUser deleted!";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else {
        std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid HTTP Method!";
        send(clientSocket, response.c_str(), response.length(), 0);
    }

    closeDatabase(db);
}

void startHttpServer(int port) {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int c, read_size;
    char buffer[1024];
    std::unordered_map<std::string, std::string> params;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        return;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Bind failed!" << std::endl;
        return;
    }

    listen(serverSocket, 3);
    std::cout << "Waiting for incoming connections..." << std::endl;

    c = sizeof(struct sockaddr_in);
    while ((clientSocket = accept(serverSocket, (struct sockaddr*)&client, &c)) != INVALID_SOCKET) {
        std::cout << "Connection accepted!" << std::endl;

        read_size = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (read_size > 0) {
            buffer[read_size] = '\0';
            std::string request(buffer);
            handleRequest(request, params, clientSocket);
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    initializeDatabase();
    startHttpServer(8080);
    return 0;
}
