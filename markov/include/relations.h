#ifndef RELATIONS_H_
#define RELATIONS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_set>


#include "thirdparty/json.hpp"
using json = nlohmann::json;

typedef struct {
    std::unordered_map<char, std::unordered_map<char, int>> table;
    std::unordered_map<char, int> occurance;
} RelationTable;

RelationTable* load_table_from_file(const char* file_path);


#endif // RELATIONS_H_
