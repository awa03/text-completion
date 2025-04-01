#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"

#define SUCCESS 1
#define TRUE 1
#define FAILURE 0
#define FALSE 0

#define DEFAULT_VOCAB_SIZE 512 // will double when dynamically growing

#define THREAD_COUNT 1 // number of threads


// global variable
char* file_buffer;
long file_size;

typedef struct{
    char* text;      // token string representation
    size_t count;    // token count
    size_t id;       // token identified
}Token;

typedef struct {
    Token** tokens;
    size_t count;
    size_t vocab_size;
} Tokenizer;


int InitTokenBuilder(Tokenizer** builder);
int InitTokens(Tokenizer** builder);
int FreeTokens(Tokenizer* builder);
int GetTokensFromFile(Tokenizer** builder, const char* file_path);
int WriteTokenToFile(Tokenizer** builder, const char* file_path);
int TokenExists(Tokenizer* builder, const char* check_str);
Token* CreateToken(char* str, size_t id);
int DeepCopyTokens(Token** src, size_t src_size, Token** dest);
int GrowTokenVocabSize(Tokenizer* builder);
int AddToken(Tokenizer* builder, Token* add_tok);
int MergeTokens(Tokenizer* builder, const char* first, const char* second);

#endif
