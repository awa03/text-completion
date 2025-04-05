#include "relations.h"

RelationTable* load_table_from_file(const char* file_path){
    std::ifstream file(file_path);
    if(!file.is_open()){
        std::cerr << "Error opening file path" << "\n";
        return nullptr;
    }

    json j;
    try {
        file >> j;
    } catch(json::parse_error& e){
        std::cerr << "Parse error: " << e.what() << std::endl;
        return nullptr;
    }

    RelationTable *t = new RelationTable;
    const std::string content = j["encoded"];

    load_vocab(t, j);
    reverse_vocab(t);

    // tracking the following occurance counts
    // so for string abab
    //    [a b] -> index of occurance
    // [a] 0 1
    // [b] 2 0  -> index of occurance[x]

    for(int i = 0; i < content.size() - 1; i++){
        std::string left = std::string(1, content[i]);
        std::string right = std::string(1, content[i+1]);
        t->table[right][left]++;
        t->occurance[left]++;
    }

    for (const auto& [from, transitions] : t->table) {
        for (const auto& [to, count] : transitions) {
            t->probability[from][to] =  static_cast<double>(count) / static_cast<double>(t->occurance[to]);
        }
    }

    return t;
}

void load_vocab(RelationTable* relation, json builder){
    auto c = builder["vocab"];
    for(auto& e : c){
        Pair new_pair;
        new_pair.first = e["pair"]["left"];
        new_pair.second = e["pair"]["right"];
        new_pair.freq = e["freq"];

        std::string replacement = e["string"];
        relation->vocab[new_pair] = replacement;
    }
}

void reverse_vocab(RelationTable* relation){
    for(auto& [first, second] : relation->vocab) {
        relation->reversed_vocab[second] = first;
    }
}

Pair getRandomPair(RelationTable* relation){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, relation->vocab.size() - 1);

    auto item = relation->vocab.begin();
    std::advance(item, distrib(gen));

    return item->first;
}


std::string decodeToken(RelationTable* relation, std::string token) {
    if (!relation) {
        std::cerr << "Error: Null relation table\n";
        return "";
    }

    std::vector<std::string> toProcess;
    toProcess.push_back(token);
    std::string result = "";

    while (!toProcess.empty()) {
        std::string current = toProcess.back();
        toProcess.pop_back();

        auto it = relation->reversed_vocab.find(current);
        if (it == relation->reversed_vocab.end()) {
            result += current;
        } else {
            toProcess.push_back(it->second.second);
            toProcess.push_back(it->second.first);
        }
    }

    return result;
}

std::string getNextTokenWithTemperature(RelationTable* relation, std::string token, double temperature = 1.0) {
    if (!relation) {
        return "";
    }

    auto iter = relation->probability.find(token);
    if (iter == relation->probability.end()) {
        return "";
    }

    // Apply temperature to probabilities
    std::vector<std::pair<std::string, double>> choices;
    double sum = 0.0;

    for (auto& [key, probability] : iter->second) {
        // Apply temperature (lower = more deterministic, higher = more random)
        double adjusted_prob = std::pow(probability, 1.0/temperature);
        choices.push_back({key, adjusted_prob});
        sum += adjusted_prob;
    }

    // Normalize probabilities
    for (auto& choice : choices) {
        choice.second /= sum;
    }

    // Select based on weighted probability
    double r = static_cast<double>(rand()) / RAND_MAX;
    double cumulative = 0.0;

    for (auto& [key, probability] : choices) {
        cumulative += probability;
        if (r <= cumulative) {
            return key;
        }
    }

    // Fallback
    return choices.empty() ? "" : choices[0].first;
}

// finds highest pair probability
std::string getHighestProbability(RelationTable* relation, std::string token){
    if (!relation) {
        std::cerr << "Error: Null relation table\n";
        return "";
    }

    double highest = 0.0;
    std::string highest_str = "";

    auto iter = relation->probability.find(token);
    if (iter != relation->probability.end()) {
        for(auto& [key, probability] : iter->second) {
            if(probability > highest) {
                highest = probability;
                highest_str = key;
            }
        }
    }

    return highest_str;
}

std::string generateSentence(RelationTable* relation, int max_length) {
    if (!relation || relation->probability.empty()) {
        std::cerr << "Error: Invalid relation table or empty probability map\n";
        return "";
    }

    auto random_pair = getRandomPair(relation);
    std::string current_token = random_pair.first;
    std::string decoded = decodeToken(relation, current_token);
    std::string result = decoded;

    for (int i = 0; i < max_length; i++) {
        std::string next_token = getHighestProbability(relation, current_token);

        if (next_token.empty()) {
            auto random_pair = getRandomPair(relation);
            next_token = random_pair.first;
        }

        std::string next_decoded = decodeToken(relation, next_token);
        result += next_decoded;

        current_token = next_token;

        if (next_decoded.find('.') != std::string::npos ||
            next_decoded.find('!') != std::string::npos ||
            next_decoded.find('?') != std::string::npos) {
            if (rand() % 100 < 30) {
                break;
            }
        }
    }

    return result;
}
