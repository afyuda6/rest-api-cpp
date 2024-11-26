#include "http_server.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

void startServer(int port, std::function<std::string(const std::string&, const std::unordered_map<std::string, std::string>&)> handler) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    std::cout << "Server started on port " << port << "...\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            continue;
        }

        std::thread([clientSocket, handler]() {
            char buffer[1024] = {0};
            recv(clientSocket, buffer, sizeof(buffer), 0);

            // Parse the HTTP request
            std::istringstream requestStream(buffer);
            std::string method, path;
            requestStream >> method >> path;

            std::unordered_map<std::string, std::string> queryParams;

            // Parse query parameters
            size_t queryPos = path.find('?');
            if (queryPos != std::string::npos) {
                std::string query = path.substr(queryPos + 1);
                path = path.substr(0, queryPos);

                std::istringstream queryStream(query);
                std::string param;
                while (std::getline(queryStream, param, '&')) {
                    size_t eqPos = param.find('=');
                    if (eqPos != std::string::npos) {
                        queryParams[param.substr(0, eqPos)] = param.substr(eqPos + 1);
                    }
                }
            }

            // Generate the response
            std::string response = handler(path, queryParams);

            // Send the response
            std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(response.size()) + "\r\n\r\n" + response;
            send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);

            closesocket(clientSocket);
        }).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
}