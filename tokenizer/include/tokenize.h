#ifndef TOKENIZE_H_
#define TOKENIZE_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

#include "thirdparty/json.hpp"
using json = nlohmann::json;

struct TextFile {
    json data;
    std::string delim;
    std::unordered_map<std::string, int> occurance;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> probability;
};

TextFile* load_tokenized_file(const char* file_name, const char delim);
void write_tokenized_file(TextFile* loaded_file, const char* file_path);

#endif // TOKENIZE_H_
