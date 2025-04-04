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
    bool *dot      = flag_bool("dot", false, "Outputs dot file text to console");
    uint64_t *random      = flag_uint64("random-token", 0, "Outputs dot file text to console");
    uint64_t *limit = flag_uint64("limit", DEFAULT_VOCAB_SIZE, "Vocab size");

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
        data->dumpToFile(*output_path);

        if(*dot){
            cout << "digraph Pairs {\n";
            for(auto p: data->vocab){
                data->printPairChain(p.first, true);
            }
            cout << "}";
        }


        srand(time(0));
        for(int i = 0; i < *random; i++){
            data->printPairChain(data->getRandomPair());
        }

    }
    else {
       auto data = loadBytePairEncodedFile(*bpe_path);
        srand(time(0));
        for(int i = 0; i < *random; i++){
            data->printPairChain(data->getRandomPair());
        }
    }

    return 0;
}
