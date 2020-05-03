
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "warps.h"

int main(int argc, char *argv[]){
    unsigned int seed = 0;
    if(argc > 1){
        int seed = atoi(argv[1]);
        printf("Seed: 0x%08x\n", seed);
        printf("Test Include: 0x%02x\n", tree_available_init[0].me.map);
        wm_generate_mapping(seed);
    }
    else{
        printf("No Seed Provided\n");
        return 0;
    }
    return 0;
}
