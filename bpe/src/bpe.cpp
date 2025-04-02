#include "bpe.h"


BytePairEncoded* genBytePairEncoding(const char* file_path, size_t v_size = DEFAULT_VOCAB_SIZE){
    std::ifstream file(file_path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    BytePairEncoded* data = new BytePairEncoded(v_size);

    data->populateDict(content);
    bool stop_flag = false;

    // need to use while to prevent recursion depth from being reached
    while(!stop_flag){
        Pair most_used_pair = {' ', ' ', 0}; // the pair that has been used the most
        std::cout << content << "\n";

        // encode -- set these two vars
        data->encode(content, most_used_pair);
        std::cout << most_used_pair.first;
        std::cout << most_used_pair.second << "\n";
        std::cout << most_used_pair.freq << "\n";
        stop_flag = data->stopEncoding(most_used_pair.freq);
    }

    return data;
}

void BytePairEncoded::populateDict(string& content){
    for(auto& c: content){
        dict.insert(c);
    }
}

void BytePairEncoded::encode(std::string& content, Pair& most_used_pair){
    unordered_map<Pair, std::string, PairHash> pair_freq;
    std::string new_data;

    for(size_t i = 0; i < content.size() - 1; i++){
        Pair tmp = {content[i], content[i + 1], 1};
        auto find = pair_freq.find(tmp);

        std::string replacement;

        // update frequency within local table
        if(find != pair_freq.end()){
            tmp.freq = find->first.freq + 1;
            replacement = find->second;
            pair_freq.erase(find);
            pair_freq[tmp] = replacement;
        }
        else {
            // get an unused char
            replacement = getUnusedChar();
            pair_freq[tmp] = replacement;

            // add to the global vocab
            auto find_vocab = vocab.find(tmp);
            if(find == vocab.end()){
                vocab[tmp] = replacement;
            }
        }

        // update most used
        if(tmp.freq > most_used_pair.freq){
            most_used_pair = tmp;
        }
    }

    content = new_data;
}



bool BytePairEncoded::stopEncoding(int most_used_freq){
    if(most_used_freq == 1){
        return true;
    }

    return true;
}

void BytePairEncoded::insertPairToVocab(char first, char second, const string& replacement){
    Pair p = {first, second, 0};
    vocab[p] = replacement;
}

char BytePairEncoded::getUnusedChar() {
    for (unsigned char c = 1; c < 255; c++) {
        if (dict.find(c) == dict.end()) {
            return c;
        }
    }
    throw runtime_error("No unused characters available in the ASCII range");
}

