#ifndef USER_H
#define USER_H

#include <string>
#include <sqlite3.h>
#include <vector>
#include <unordered_map>

void createUser(sqlite3* db, const std::string& name, int roleId);
std::vector<std::unordered_map<std::string, std::string>> getUsers(sqlite3* db);

#endif