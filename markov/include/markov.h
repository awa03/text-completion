#ifndef MARKOV_H_
#define MARKOV_H_

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <random>

#include "thirdparty/json.hpp"
using json = nlohmann::json;

typedef struct {
    std::unordered_map<std::string, std::unordered_map<std::string, double>> probability;
} RelationNoBPE;

RelationNoBPE* load_file_no_bpe(const char* file_path);
std::string generate_sentence_no_bpe(RelationNoBPE* relation, const std::string& start_word, int max_length);

#endif // MARKOV_H_
