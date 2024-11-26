#ifndef PERMISSION_H
#define PERMISSION_H

#include <string>
#include <sqlite3.h>
#include <vector>
#include <unordered_map>

void createPermission(sqlite3* db, const std::string& name, int roleId);
std::vector<std::unordered_map<std::string, std::string>> getPermissions(sqlite3* db);

#endif