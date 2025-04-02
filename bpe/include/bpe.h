#ifndef BPE_H_
#define BPE_H_

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>

#define DEFAULT_VOCAB_SIZE 32

using namespace std;

struct Pair {
    char first, second;
    size_t freq;

    bool operator==(const Pair& other) const {
        return first == other.first && second == other.second;
    }
};

struct PairHash {
    size_t operator()(const Pair& p) const {
        return std::hash<char>()(p.first) ^ std::hash<char>()(p.second);
    }
};



// TODO
// Error handling for vocab size
//

struct BytePairEncoded {
public:
    unordered_set<char> dict; // track used chars
    unordered_map<Pair, string, PairHash> vocab; // track used pairs and their replacements
    size_t vocab_size; // goal size of vocab

    BytePairEncoded(){}
    BytePairEncoded(size_t v_size) : vocab_size(v_size) {}
    void insertPairToVocab(char first, char second, const string& replacement);
    void encode(std::string& content, Pair& most_used_pair);
    char getUnusedChar();
    void populateDict(std::string& content);
    bool stopEncoding(int most_used_freq);
};

BytePairEncoded* genBytePairEncoding(const char* file_path, size_t v_size);

#endif // BPE_H_
