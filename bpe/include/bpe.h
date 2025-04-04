#ifndef BPE_H_
#define BPE_H_

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <random>

#include "thirdparty/json.hpp"
using json = nlohmann::json;

#define DEFAULT_VOCAB_SIZE 999999999999

using namespace std;

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



// TODO
// Error handling for vocab size
//

struct BytePairEncoded {
public:
    unordered_set<string> dict; // track used chars
    unordered_map<Pair, string, PairHash> vocab; // track used pairs and their replacements
    size_t vocab_size; // goal size of vocab
    json j_data;

    BytePairEncoded(){}

    std::vector<std::string> splitIntoUTF8Chars(const std::string& str);
    BytePairEncoded(size_t v_size) : vocab_size(v_size) {}
    void insertPairToVocab(const std::string& first, const std::string& second, const string& replacement);
    void encode(std::string& content, Pair& most_used_pair);
    json dumpVocab();
    void dumpToFile(const char* file_path);
    void printPairChain(const Pair& load_pair, bool is_dot = false);
    std::string getUnusedChar();
    void populateDict(std::string& content);
    bool stopEncoding(int most_used_freq);
    Pair getRandomPair();
};

BytePairEncoded* genBytePairEncoding(const char* file_path, size_t v_size);

#endif // BPE_H_
