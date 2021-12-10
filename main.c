#include <stdio.h>
#include <stdlib.h>
#include "f2d.h"

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Not enough arg\n");
        return EXIT_FAILURE;
    }

    double dInput = atof(argv[1]);

    convert(dInput);

	return EXIT_SUCCESS;
}
