#include "main.h"
#include <stdio.h>
#include "tokenizer.h"

int main(){
    Tokenizer* token_builder;
    if(InitTokenBuilder(&token_builder) == FAILURE) {
        fprintf(stderr, "Failed to initialize tokenizer\n");
        return 1;
    }

    FreeTokens(token_builder);
    free(token_builder);
    return 0;
}
