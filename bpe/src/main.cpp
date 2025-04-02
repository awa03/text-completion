#include <iostream>
#include <bpe.h>

int main(int argc, char *argv[]){
    genBytePairEncoding("tests/test_small_1.txt", 20);
    return 0;
}
