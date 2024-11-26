#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <functional>
#include <string>
#include <unordered_map>

void startServer(int port, std::function<std::string(const std::string&, const std::unordered_map<std::string, std::string>&)> handler);

#endif