#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void initializeDatabase();

void parseFormData(const std::string &body, std::unordered_map<std::string, std::string> &params);

void userHandler(const std::string &request, const std::unordered_map<std::string, std::string> &params,
                 int clientSocket);

int main() {
    initializeDatabase();
    int serverSocket, clientSocket;
    sockaddr_in server{}, client{};
    socklen_t clientLen;
    char buffer[8192];
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        return 0;
    }
    int port = 6000;
    const char *portEnv = std::getenv("PORT");
    if (portEnv != nullptr) {
        port = std::atoi(portEnv);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&server), sizeof(server)) == -1) {
        return 0;
    }
    listen(serverSocket, 3);
    clientLen = sizeof(client);
    while ((clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&client), &clientLen)) != -1) {
        ssize_t read_size = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (read_size > 0) {
            std::unordered_map<std::string, std::string> params;
            buffer[read_size] = '\0';
            std::string request(buffer);
            userHandler(request, params, clientSocket);
        }
        close(clientSocket);
    }
    close(serverSocket);
    return 0;
}
