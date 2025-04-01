#include "tokenizer.h"

int InitTokens(Tokenizer** builder){
    if ((*builder)->tokens == NULL) {
        (*builder)->tokens = (Token**)malloc(sizeof(Token*) * DEFAULT_VOCAB_SIZE);
        if (!(*builder)->tokens) {
            fprintf(stderr, "Failed to allocate token array\n");
            free(buffer);
            return FAILURE;
        }
        (*builder)->vocab_size = DEFAULT_VOCAB_SIZE;
    }
    return SUCCESS;
}

int InitTokenBuilder(Tokenizer** builder){
    *builder = (Tokenizer*)malloc(sizeof(Tokenizer));
    if (!*builder) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return FAILURE;
    }
    (*builder)->tokens = NULL;
    (*builder)->count = 0;

    return InitTokens(builder);
}


int FreeTokens(Tokenizer* builder){
    if (!builder || !builder->tokens) return SUCCESS;

    for(size_t i = 0; i < builder->count; i++){
        if(builder->tokens[i]) {
            if(builder->tokens[i]->text) {
                free(builder->tokens[i]->text);
            }
            free(builder->tokens[i]);
        }
    }
    free(builder->tokens);
    builder->tokens = NULL;
    builder->count = 0;
    return SUCCESS;
}

int GetTokensFromFile(Tokenizer** builder, const char* file_path){
    // error handling for null cases
    if(!builder){
        fprintf(stderr, "Builder is NULL\n");
        return FAILURE;
    }
    else if(!file_path) {
        fprintf(stderr, "File path is NULL\n");
        return FAILURE;
    }

    FILE *fptr;
    fptr = fopen(file_path, "r");
    if(!fptr){
        fprintf(stderr, "Failed to Open Filepath\n");
        return FAILURE;
    }

    // get and validate the file size
    file_size = GetFileSize(fptr);
    if(file_size <= 0){
        fprintf(stderr, "Invalid File Size\n");
        return FAILURE;
    }

    // load file into a buffer
    file_buffer = (char*)malloc(file_size + 1);
    if (!file_buffer) {
        fprintf(stderr, "Failed to allocate memory for file content\n");
        fclose(fptr);
        return FAILURE;
    }

    size_t bytes_read = fread(file_buffer, 1, file_size, fptr);
    file_buffer[bytes_read] = '\0';
    fclose(fptr);

    InitTokens(builder);

    return SUCCESS;
}

int WriteTokenToFile(Tokenizer** builder, const char* file_path){
    // error handling for null cases
    if(!builder){
        fprintf(stderr, "Builder is NULL\n");
        return FAILURE;
    }
    else if(!file_path) {
        fprintf(stderr, "File path is NULL\n");
        return FAILURE;
    }

    FILE *fptr;
    fptr = fopen(file_path, "w");
    if(!fptr){
        fprintf(stderr, "Failed to Open Filepath\n");
        return FAILURE;
    }

    fclose(fptr);

    return SUCCESS;
}

int TokenExists(Tokenizer* builder, const char* check_str){
    for(size_t i= 0; i < builder->count; i++){
        if(strcmp(builder->tokens[i]->text, check_str) == 0){
            builder->tokens[i]->count++;
            return TRUE;
        }
    }
    return FALSE;
}


Token* CreateToken(char* str, size_t id){
    Token* new_token = (Token*)malloc(sizeof(Token));
    if(!new_token){
        fprintf(stderr, "Failed to Allocate Memory for Token\n");
        return 0;
    }

    // init token
    new_token->text = str;
    new_token->count = 1;
    new_token->id = id;

    return new_token;
}

int DeepCopyTokens(Token** src, size_t src_size, Token** dest){
    for(size_t i = 0; i < src_size; i++){
        dest[i] = malloc(sizeof(Token));
        if(!dest[i]) {
            fprintf(stderr, "Failed to allocate memory for token copy\n");
            return FAILURE;
        }

        // Copy basic fields
        dest[i]->count = src[i]->count;
        dest[i]->id = src[i]->id;

        // Duplicate the string
        size_t len = strlen(src[i]->text) + 1;
        dest[i]->text = malloc(len);
        if(!dest[i]->text) {
            fprintf(stderr, "Failed to allocate memory for token text\n");
            free(dest[i]);
            return FAILURE;
        }
        strcpy(dest[i]->text, src[i]->text);
    }
    return SUCCESS;
}

int GrowTokenVocabSize(Tokenizer* builder){
    // Allocate memory for pointers to Token structs, not Token structs
    Token** new_token_list = malloc(sizeof(Token*) * (builder->count * 2));
    if(!new_token_list) {
        fprintf(stderr, "Failed to allocate memory for new token list\n");
        return FAILURE;
    }

    // copy list
    if(DeepCopyTokens(builder->tokens, builder->count, new_token_list) == FAILURE){
        fprintf(stderr, "Error Copying Tokens to new list\n");
        free(new_token_list);
        return FAILURE;
    }

    // free old list
    if(FreeTokens(builder) == FAILURE){
        fprintf(stderr, "Error Freeing Tokens while growing list\n");
        return FAILURE;
    }

    // assign new list
    builder->tokens = new_token_list;
    builder->vocab_size = builder->count * 2;  // Update vocab_size

    return SUCCESS;
}

int AddToken(Tokenizer* builder, Token* add_tok){
    // if list needs to grow
    if(builder->count - 1 <= builder->vocab_size){
        if(GrowTokenVocabSize(builder) == FAILURE){
            return FAILURE;
        }
    }

    // handle adding
    size_t tok_index = builder->count++;
    builder->tokens[tok_index] = add_tok;
    return SUCCESS;
}


int MergeTokens(Tokenizer* builder, const char* first, const char* second){
    // null error
    if(!builder || !first || !second){
        fprintf(stderr, "Error NULL Merge Attempted\n");
        return FAILURE;
    }

    // create merge buffer
    size_t merge_length = strlen(first) + strlen(second) + 1;
    char* merge_str = (char*)malloc(merge_length);
    if(!merge_str){
        fprintf(stderr, "Error Allocating Memory for merge string\n");
        return FAILURE;
    }

    strcpy(merge_str, first);
    strcat(merge_str, second);

    // check if token exists
    if(TokenExists(builder, merge_str)){
        free(merge_str);
        return SUCCESS;
    }

    Token* new_tok = CreateToken(merge_str, builder->count);
    AddToken(builder, new_tok);

    free(merge_str);
    return SUCCESS;
}
