#include <iostream>
#include "tokenize.h"

#define FLAG_IMPLEMENTATION
#include "thirdparty/flag.h"

void usage(void) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n", flag_program_name());
    fprintf(stderr, "OPTIONS:\n");
    flag_print_options(stderr);
}

int main(int argc, char *argv[]){
    bool *help      = flag_bool("help", false, "Print this help message");
    char **txt_path = flag_str("txt", NULL, "Path to Text file (MANADATORY)");
    char **out_path = flag_str("o", "gen.json", "Path to Out file");
    char **delim = flag_str("-delim", " ", "tokenizer delimiter");

    if (!flag_parse(argc, argv)) {
        usage();
        flag_print_error(stderr);
        return 1;
    }

    if(*txt_path != nullptr){
        TextFile* data = load_tokenized_file(*txt_path, **delim);
        write_tokenized_file(data, *out_path);
    }
}
