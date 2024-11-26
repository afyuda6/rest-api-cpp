#ifndef ROLE_H
#define ROLE_H

#include <string>
#include <sqlite3.h>
#include <vector>
#include <unordered_map>

void createRole(sqlite3* db, const std::string& name);
std::vector<std::unordered_map<std::string, std::string>> getRoles(sqlite3* db);

#endif