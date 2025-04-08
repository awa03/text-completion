#include "markov.h"

RelationNoBPE* load_file_no_bpe(const char* file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << "\n";
        return nullptr;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));

    json j;
    try {
        j = json::parse(content);
    } catch (json::parse_error& e) {
        std::cerr << "Parse error at byte " << e.byte << ": " << e.what() << std::endl;
        return nullptr;
    }

    RelationNoBPE* relation = new RelationNoBPE();

    try {
        for (auto& e : j) {
            const std::string key = e["key"];
            const std::string value = e["value"];
            const std::string p_str = e["probability"];
            const double percision = std::stod(p_str);

            relation->probability[key][value] = percision;
        }
    } catch (json::exception& e) {
        std::cerr << "JSON processing error: " << e.what() << std::endl;
        delete relation;
        return nullptr;
    }

    return relation;
}


std::string generate_sentence_no_bpe(RelationNoBPE* relation, const std::string& start_word, int max_length = 50) {
    if (!relation) {
        return "Error: Relation is null";
    }

    if (relation->probability.find(start_word) == relation->probability.end()) {
        return "Error: Start word not found in model";
    }

    std::string sentence = start_word;
    std::string current_word = start_word;

    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (int i = 0; i < max_length; i++) {
        const auto& next_options = relation->probability[current_word];

        if (next_options.empty()) {
            break;
        }

        std::vector<std::string> options;
        std::vector<double> cumulative_probs;
        double sum = 0.0;

        for (const auto& pair : next_options) {
            options.push_back(pair.first);
            sum += pair.second;
            cumulative_probs.push_back(sum);
        }

        // normalize
        if (std::abs(sum - 1.0) > 0.01) {
            for (auto& prob : cumulative_probs) {
                prob /= sum;
            }
        }

        std::uniform_real_distribution<> dist(0.0, 1.0);
        double random_val = dist(gen);

        std::string next_word;
        for (size_t j = 0; j < cumulative_probs.size(); j++) {
            if (random_val <= cumulative_probs[j]) {
                next_word = options[j];
                break;
            }
        }

        sentence += " " + next_word;
        current_word = next_word;

        if (next_word.find('.') != std::string::npos) {
            break;
        }
    }

    return sentence;
}
