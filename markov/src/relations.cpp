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

    const std::string content = j["encoded"];

    RelationTable *t = new RelationTable;


    // think about the vocab... lets focus on the content for now.

    // tracking the following occurance counts
    // so for string abab
    //    [a b] -> index of occurance
    // [a] 0 1
    // [b] 2 0  -> index of occurance[x]

    for(int i = 0; i < content.size() - 1; i++){
        char left = content[i];
        char right = content[i+1];
        t->table[right][left]++;
        t->occurance[left]++;
    }

    if (!content.empty()) {
        t->occurance[content.back()]++;
    }

    for (const auto& [from, transitions] : t->table) {
        for (const auto& [to, count] : transitions) {
            std::cout << from << " " << to
                      << "\tfollow count: " << static_cast<double>(count) / static_cast<double>(t->occurance[to]) << "\n";
        }
    }

    return t;
}
