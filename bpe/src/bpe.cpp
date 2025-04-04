#include "bpe.h"

BytePairEncoded* loadBytePairEncodedFile(const char* file_path){
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

    BytePairEncoded* data = new BytePairEncoded(DEFAULT_VOCAB_SIZE);
    data->loadVocab(j);
    data->dumpVocabPrint();

    return data;
}

void BytePairEncoded::loadVocab(json& builder){
    auto content = builder["encoded"];
    auto c = builder["vocab"];
    for(auto& e : c){
        Pair new_pair;
        new_pair.first = e["pair"]["left"];
        new_pair.second = e["pair"]["right"];
        new_pair.freq = e["freq"];

        std::string replacement = e["string"];
        vocab[new_pair] = replacement;
    }
}


void BytePairEncoded::dumpVocabPrint(){
    for(auto& e : vocab){
        cout << "Replacement: " << e.second << " :" << e.first.first << "," << e.first.second << "\n";
    }
}

BytePairEncoded* genBytePairEncoding(const char* file_path, size_t v_size = DEFAULT_VOCAB_SIZE){
    std::ifstream file(file_path);

    if(!file.is_open()){
        std::cerr << "Error opening file path" << "\n";
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string content = buffer.str();

    if(content.size() <= 1){
        std::cerr << "Filesize error" << "\n";
        return nullptr;
    }

    BytePairEncoded* data = new BytePairEncoded(v_size);

    data->populateDict(content);
    bool stop_flag = false;

    while(!stop_flag){
        Pair most_used_pair = {"", "", 0};
        data->encode(content, most_used_pair);
        if(most_used_pair.freq <= 1){
            break;
        }
    }

    data->j_data["encoded"] = content;
    data->j_data["vocab"] = data->dumpVocab();

    return data;
}

void BytePairEncoded::dumpToFile(const char* file_path){
    std::ofstream outFile(file_path);
    outFile << j_data.dump();
    outFile.close();
}

json BytePairEncoded::dumpVocab() {
    json j_array = json::array();
    for(auto& term : vocab) {
        json entry;
        json pair;
        pair["left"] = term.first.first;
        pair["right"] = term.first.second;
        entry["pair"] = pair;
        entry["string"] = term.second;
        entry["freq"] = term.first.freq;
        j_array.push_back(entry);
    }
    return j_array;
}

std::vector<std::string> BytePairEncoded::splitIntoUTF8Chars(const std::string& str) {
    std::vector<std::string> result;

    for (size_t i = 0; i < str.length();) {
        int charLen = 1;

        if ((str[i] & 0x80) == 0) {
            charLen = 1;
        } else if ((str[i] & 0xE0) == 0xC0) {
            charLen = 2;
        } else if ((str[i] & 0xF0) == 0xE0) {
            charLen = 3;
        } else if ((str[i] & 0xF8) == 0xF0) {
            charLen = 4;
        }

        if (i + charLen <= str.length()) {
            result.push_back(str.substr(i, charLen));
        } else {
            result.push_back(str.substr(i, 1));
        }

        i += charLen;
    }

    return result;
}

void BytePairEncoded::populateDict(string& content){
    auto chars = splitIntoUTF8Chars(content);
    for(auto& c: content){
        dict.insert(std::string(1, c));
    }
}

void BytePairEncoded::encode(std::string& content, Pair& most_used_pair) {
     std::vector<std::string> chars = splitIntoUTF8Chars(content);

    std::unordered_map<Pair, int, PairHash> pair_freq;

    for(size_t i = 0; i < chars.size() - 1; i++) {
        Pair tmp = {chars[i], chars[i + 1], 1};
        auto find = pair_freq.find(tmp);

        if(find != pair_freq.end()) {
            pair_freq[tmp]++;
            auto find_vocab = vocab.find(tmp);
            if(find_vocab != vocab.end()){
                std::string re = find_vocab->second;
                vocab.erase(find_vocab);
                vocab[tmp] = re;
            }
        } else {
            pair_freq[tmp] = 1;
        }
    }

    for(const auto& pair : pair_freq) {
        if(pair.second > most_used_pair.freq) {
            most_used_pair = pair.first;
            most_used_pair.freq = pair.second;
        }
    }

    if(most_used_pair.freq > 1) {
        std::string replacement;
        auto find_vocab = vocab.find(most_used_pair);

        if(find_vocab != vocab.end()) {
            replacement = find_vocab->second;
        } else {
            replacement = getUnusedChar();
            dict.insert(replacement);
            vocab[most_used_pair] = replacement;
        }

        std::string pair_str = most_used_pair.first + most_used_pair.second;
        size_t pos = 0;
        while((pos = content.find(pair_str, pos)) != std::string::npos) {
            content.replace(pos, pair_str.length(), replacement);
            pos += replacement.length();
        }
    }
}

bool BytePairEncoded::stopEncoding(int most_used_freq){
    if(most_used_freq == 1){
        return true;
    }

    return true;
}

void BytePairEncoded::insertPairToVocab(const std::string& first, const std::string& second, const string& replacement) {
    Pair p = {first, second, 0};
    vocab[p] = replacement;
}

std::string BytePairEncoded::getUnusedChar() {
    for (unsigned char c = 33; c < 127; c++) {
        if (c != '"' && c != '\'' && c != '\\' && c != '/' &&
            c != '{' && c != '}' && c != '[' && c != ']' &&
            dict.find(std::string(1, c)) == dict.end()) {
            return std::string(1, c);
        }
    }

    static int unicodeCounter = 0xE000;

    char utf8[5] = {0, 0, 0, 0, 0};
    int codepoint = unicodeCounter++;

    if (codepoint <= 0x7F) {
        utf8[0] = codepoint;
    } else if (codepoint <= 0x7FF) {
        utf8[0] = 0xC0 | (codepoint >> 6);
        utf8[1] = 0x80 | (codepoint & 0x3F);
    } else if (codepoint <= 0xFFFF) {
        utf8[0] = 0xE0 | (codepoint >> 12);
        utf8[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        utf8[2] = 0x80 | (codepoint & 0x3F);
    } else {
        utf8[0] = 0xF0 | (codepoint >> 18);
        utf8[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        utf8[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        utf8[3] = 0x80 | (codepoint & 0x3F);
    }

    return std::string(utf8);
}

/*
 * Since this will resolve in essentially a tree like data structure, we should first swap the list
 * */

void BytePairEncoded::printPairChain(const Pair& load_pair, bool is_dot) {
    unordered_map<std::string, Pair> reversed_vocab;
    for(auto& token : vocab) {
        reversed_vocab[token.second] = token.first;
    }

    std::stack<std::string> replacements;
    std::unordered_set<std::string> visited;

    replacements.push(load_pair.second);
    replacements.push(load_pair.first);


    while(!replacements.empty()) {
        std::string key = replacements.top();
        replacements.pop();

        if(visited.find(key) != visited.end()) {
            continue;
        }

        visited.insert(key);


        auto curr = reversed_vocab.find(key);
        if(curr != reversed_vocab.end()) {
            replacements.push(curr->second.second);
            replacements.push(curr->second.first);
            if(is_dot) std::cout << '"' << key << '"' << " -> "
                                 << '"' << curr->second.first << '"' << "\n";
            if(is_dot) std::cout << '"' << key << '"' << " -> "
                                 << '"' << curr->second.second << '"' << "\n";

        } else {
            if(!is_dot) cout << key;
        }
    }

}



Pair BytePairEncoded::getRandomPair(){
     static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, vocab.size() - 1);

    auto item = vocab.begin();
    std::advance(item, distrib(gen));

    return item->first;
}
