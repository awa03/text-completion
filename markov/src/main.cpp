#include <iostream>
#include "relations.h"

#define FLAG_IMPLEMENTATION
#include "thirdparty/flag.h"

/**
*
*
*/

void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n", flag_program_name());
    fprintf(stderr, "OPTIONS:\n");
    flag_print_options(stderr);
}

int main(int argc, char *argv[]){
    bool *help      = flag_bool("help", false, "Print this help message");
    char **bpe_path = flag_str("bpe", NULL, "Path to BPE file (MANADATORY)");
    uint64_t *limit = flag_uint64("limit", 100, "Text size");

    if (!flag_parse(argc, argv)) {
        usage();
        flag_print_error(stderr);
        return 1;
    }

    if(*bpe_path == nullptr) return -1;
    RelationTable* table = load_table_from_file(*bpe_path);

    srand(time(0));
    std::cout << generateSentence(table, *limit);
}
