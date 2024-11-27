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
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int c, read_size;
    char buffer[1024];
    std::unordered_map<std::string, std::string> params;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 0;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(6000);

    if (bind(serverSocket, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Bind failed!" << std::endl;
        return 0;
    }

    listen(serverSocket, 3);

    c = sizeof(struct sockaddr_in);
    while ((clientSocket = accept(serverSocket, (struct sockaddr *) &client, &c)) != INVALID_SOCKET) {
        read_size = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (read_size > 0) {
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
