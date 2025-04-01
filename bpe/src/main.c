#include "main.h"
#include <stdio.h>
#include "tokenizer.h"

// for implementing arguments
#define FLAG_IMPLEMENTATION
#include "thirdparty/flag.h"

void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n", flag_program_name());
    fprintf(stderr, "OPTIONS:\n");
    flag_print_options(stderr);
}

int main(int argc, char **argv){
    srand(time(0));

    char **bpe_path = flag_str("bpe", NULL, "Path to BPE file (MANDATORY)");
    bool *help      = flag_bool("help", false, "Print this help message");
    uint64_t *limit = flag_uint64("limit", 100, "Max amount of tokens to generate");
    char **delim    = flag_str("delim", "", "Token delimiter");
    uint64_t *seed  = flag_uint64("seed", 0, "Random seed (0 to use current time)");

    if (!flag_parse(argc, argv)) {
        usage();
        flag_print_error(stderr);
        return 1;
    }

     if (*bpe_path == NULL) {
        usage();
        fprintf(stderr, "ERROR: no -%s is provided\n", flag_name(bpe_path));
        return 1;
    }

    if (*help) {
        usage();
        return 0;
    }

    if (*seed != 0) {
        srand(*seed);
    }

    Tokenizer* token_builder;
    if(InitTokenBuilder(&token_builder) == FAILURE) {
        fprintf(stderr, "Failed to initialize tokenizer\n");
        return 1;
    }

    FreeTokens(token_builder);
    free(token_builder);
    return 0;
}
