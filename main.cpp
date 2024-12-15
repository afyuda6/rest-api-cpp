#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <winsock2.h>
#include <sqlite3.h>

#pragma comment(lib, "ws2_32.lib")

void initializeDatabase();

void parseFormData(const std::string &body, std::unordered_map<std::string, std::string> &params);

void userHandler(const std::string &request, const std::unordered_map<std::string, std::string> &params,
                   SOCKET clientSocket);

int main() {
    initializeDatabase();
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in server{}, client{};
    int c;
    char buffer[1024];
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    const SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        return 0;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(6000);
    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&server), sizeof(server)) == SOCKET_ERROR) {
        return 0;
    }
    listen(serverSocket, 3);
    c = sizeof(sockaddr_in);
    while ((clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&client), &c)) != INVALID_SOCKET) {
        if (int read_size = recv(clientSocket, buffer, sizeof(buffer), 0); read_size > 0) {
            std::unordered_map<std::string, std::string> params;
            buffer[read_size] = '\0';
            std::string request(buffer);
            userHandler(request, params, clientSocket);
        }
        closesocket(clientSocket);
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
