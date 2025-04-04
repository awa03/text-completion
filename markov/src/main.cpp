#include <iostream>

/**
*
*
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
*/

int main(int argc, char *argv[]){
    return 0;
}
