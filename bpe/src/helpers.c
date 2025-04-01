#include "helpers.h"

long GetFileSize(FILE *fptr){
    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    return file_size;
}
