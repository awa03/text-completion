#include <iostream>
#include <bpe.h>

#define FLAG_IMPLEMENTATION
#include "thirdparty/flag.h"

void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n", flag_program_name());
    fprintf(stderr, "OPTIONS:\n");
    flag_print_options(stderr);
}

int main(int argc, char *argv[]){
    char **txt_path = flag_str("txt", NULL, "Path to txt file (MANDATORY if encoding)");
    char **bpe_path = flag_str("bpe", NULL, "Path to BPE file (MANADATORY if decoding)");
    char **output_path = flag_str("o", "gen.bpe", "Path to Output file");
    bool *help      = flag_bool("help", false, "Print this help message");
    uint64_t *limit = flag_uint64("limit", 100, "Max amount of tokens to generate");

    if (!flag_parse(argc, argv)) {
        usage();
        flag_print_error(stderr);
        return 1;
    }

    if (*bpe_path == NULL && *txt_path == NULL) {
        usage();
        fprintf(stderr, "ERROR: no -%s is provided\n", flag_name(txt_path));
        fprintf(stderr, "ERROR: no -%s is provided\n", flag_name(bpe_path));
        return 1;
    }
    else if(*bpe_path == NULL && *txt_path != NULL){
        auto data = genBytePairEncoding(*txt_path, *limit);
        // data.dumpToFile(*output_path);
    }
    else {
        // decodeBytePairEncoding(*bpe_path, *output_path);
    }

    return 0;
}
