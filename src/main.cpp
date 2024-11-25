#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "sqlite3.h"

#pragma comment(lib, "Ws2_32.lib")

const char* DB_FILE = "test.db";

// Initialize the SQLite database
void initDatabase() {
    sqlite3* db;
    char* errMsg = nullptr;

    // Open database
    int rc = sqlite3_open(DB_FILE, &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(1);
    }

    // Create a simple table if it doesn't exist
    const char* sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT);";
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

// Function to handle HTTP responses
void sendResponse(SOCKET clientSocket, const std::string& response, const std::string& contentType = "application/json") {
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: " << contentType << "\r\n"
        << "Content-Length: " << response.length() << "\r\n\r\n"
        << response;
    send(clientSocket, oss.str().c_str(), oss.str().length(), 0);
}

// Handle GET requests
void handleGet(SOCKET clientSocket) {
    std::string response = R"({"message": "Hello, World!"})";
    sendResponse(clientSocket, response);
}

// Handle POST requests
void handlePost(SOCKET clientSocket, const std::string& body) {
    size_t namePos = body.find("name=");
    if (namePos == std::string::npos) {
        sendResponse(clientSocket, R"({"error": "Invalid request"})", "application/json");
        return;
    }

    std::string name = body.substr(namePos + 5);

    // Insert into SQLite database
    sqlite3* db;
    sqlite3_open(DB_FILE, &db);

    std::string sql = "INSERT INTO users (name) VALUES ('" + name + "');";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);

    sqlite3_close(db);

    std::string response = R"({"message": "User added successfully!"})";
    sendResponse(clientSocket, response);
}

// Function to handle HTTP requests
void handleRequest(SOCKET clientSocket) {
    char buffer[1024] = {0};
    recv(clientSocket, buffer, 1024, 0);

    std::istringstream request(buffer);
    std::string method, path;
    request >> method >> path;

    if (method == "GET" && path == "/get") {
        handleGet(clientSocket);
    } else if (method == "POST" && path == "/post") {
        std::string body(buffer + request.tellg());
        handlePost(clientSocket, body);
    } else {
        sendResponse(clientSocket, R"({"error": "Not Found"})", "application/json");
    }

    closesocket(clientSocket);
}

// Basic HTTP server function
void startServer() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(serverSocket, (sockaddr*)&address, sizeof(address));
    listen(serverSocket, 5);

    std::cout << "Server started at http://localhost:8080" << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        std::thread(handleRequest, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    initDatabase();
    startServer();
    return 0;
}
