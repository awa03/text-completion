#ifndef RELATIONS_H_
#define RELATIONS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <random>
#include <stack>
#include <unordered_set>


#include "thirdparty/json.hpp"
using json = nlohmann::json;


struct Pair {
    std::string first, second;
    int freq;

    bool operator==(const Pair& other) const{
        return first == other.first && second == other.second;
    }

    Pair& operator=(const Pair& other){
        if (this != &other) {
            first = other.first;
            second = other.second;
            freq = other.freq;
        }
        return *this;

    }

 std::string toString() const {
        return first + second;
    }
};

struct PairHash {
  size_t operator()(const Pair& p) const {
        return std::hash<std::string>()(p.first) ^ std::hash<std::string>()(p.second);
    }
};

typedef struct {
    std::unordered_map<std::string, std::unordered_map<std::string, int>> table;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> probability;
    std::unordered_map<std::string, int> occurance;
    std::unordered_map<Pair, std::string, PairHash> vocab;
    std::unordered_map<std::string, Pair> reversed_vocab;
} RelationTable;



RelationTable* load_table_from_file(const char* file_path);
void load_vocab(RelationTable* relation, json builder);
void reverse_vocab(RelationTable* relation);
Pair getRandomPair(RelationTable* relation);

std::string getHighestProbability(RelationTable* relation, std::string token);
std::string decodeToken(RelationTable* relation, std::string token);
std::string generateSentence(RelationTable* relation, int max_length =100);
std::string getRandomFromProbability(RelationTable* relation, std::string token);
#endif // RELATIONS_H_
