#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

struct Word{
    int32_t order;
    int32_t mask;
    char word[16];
};
size_t xor_inputs(unsigned char *output, FILE *inA, FILE *inB){

    unsigned char blockA[sizeof(struct Word)];
    unsigned char blockB[sizeof(struct Word)];

    int readA;
    int readB;
    size_t count = 0;
    while ((readA = fread(blockA, 1, sizeof(struct Word), inA))&&
            (readB = fread(blockB, 1, sizeof(struct Word), inB))) {
        if(readA != readB){
            return EXIT_FAILURE;
        }
        for(int i = 0; i < readA; i++){
            output[i] = blockA[i] ^ blockB[i];
        }
        count ++;
    }
    return count;
}

int compare(const void *a,const void *b){
    return ((struct Word*)a)->order - ((struct Word*)b)->order;
}
void sort_struct(void *output, size_t count){
    qsort(output, count, sizeof(struct Word), (int)compare);
}

void mask_decoding(struct Word *output, size_t count){

    for(size_t i =0; i< count; i++){
        unsigned char letter;
        if(i+1 == count){
            letter = output[0].mask;
        }
        else{
            letter = output[i+1].mask;
        }
        for(int u =0; u<16;u++){
            letter = letter ^ output[i+1].word[u];
        }
        printf("%c",letter);
    }
}
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "%s", argv[0]);
        return EXIT_FAILURE;
    }


    FILE *inA = fopen(argv[1], "rb");

    if(!inA){
        return EXIT_FAILURE;
    }
    FILE *inB = fopen(argv[2], "rb");

    if(!inB){
        fclose(inA);
        return EXIT_FAILURE;
    }

    if(ftell(inA) != ftell(inB)){
        return EXIT_FAILURE;
    }
    unsigned char *output = malloc(ftell(inA));

    size_t count = xor_inputs(output, inA, inB);
    sort_struct((void*)output, count);
    mask_decoding((struct Word *)output, count);
    free(output);
    fclose(inA);
    fclose(inB);
    return 0;
}
