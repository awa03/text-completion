#include "tokenize.h"


TextFile* load_tokenized_file(const char* file_name, const char delim){
    std::ifstream file(file_name);
    if(!file.is_open()){
        std::cerr << "Failed to open file";
        return nullptr;
    }

    TextFile* data = new TextFile;

    std::string buff = "";
    std::vector<std::string> tokens;
    while(std::getline(file, buff, delim)){
        tokens.push_back(buff);
        data->occurance[buff]++;
    }

    std::unordered_map<std::string, std::unordered_map<std::string, int>> frequency;
    for(int i = 0; i < tokens.size() - 1; i++){
        std::string& first = tokens[i];
        std::string& second = tokens[i+1];
        frequency[first][second]++;
    }


    for(auto& [lead, table] : frequency){
        for(auto& [entry, probability] : table){
            data->probability[lead][entry] = static_cast<double>(static_cast<double>(frequency[lead][entry]) / static_cast<double>(data->occurance[lead]));
        }
    }

    return data;
}


void write_tokenized_file(TextFile* loaded_file, const char* file_path){
    json j_data;

    int i =0;
    for(auto& [first, table] : loaded_file->probability){
        for(auto& [second, probability] : table){
            json sub_entry;

            sub_entry["key"] = first;
            sub_entry["value"] = second;
            sub_entry["probability"] = std::to_string(probability);

            j_data.push_back(sub_entry);
        }
    }

    std::ofstream outFile(file_path);
    if(!outFile.is_open()){
        std::cerr << "Error Opening File\n";
    }

    outFile << j_data.dump();
    outFile.close();
}
