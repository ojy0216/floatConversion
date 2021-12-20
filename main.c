#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "f2b.h"
#include "atoh.h"
#include "warning.h"

#define TRUE 1
#define FALSE 0

#define H_LEN 4
#define B_LEN 4
#define S_LEN 8
#define D_LEN 16

void printUsage();

int main(int argc, char* argv[]){
    if(argc < 2){
		ERROR
        printf("Not enough arg\n");
        return EXIT_FAILURE;
    }
	else if(argc > 3){
		ERROR
		printf("Too many arg\n");
		return EXIT_FAILURE;
	}

    int is_bf = FALSE;
    int is_hp = FALSE;
    int is_sp = FALSE;
    int is_dp = FALSE;
    int invalid_flag = FALSE;
    int param_opt;

	if(argc == 3){
    	while(-1 != (param_opt = getopt(argc, argv, "bhsd"))){
        	switch(param_opt){
            	case 'b':
                	is_bf = TRUE;
                	break;
            	case 'h':
                	is_hp = TRUE;
                	break;
            	case 's':
                	is_sp = TRUE;
                	break;
            	case 'd':
                	is_dp = TRUE;
                	break;
            	case '?':
                	invalid_flag = TRUE;
                	break;
        	}
    	}
	}

	if(is_bf + is_hp + is_sp + is_dp > 1){
        // Multiple options are not allowed
		ERROR
		printf("only 1 option is allowed\n");

		return EXIT_FAILURE;
	}

	if(invalid_flag){
        // Unknown option
		printUsage();
	}
    else if(is_bf){
        // BF16
        if(strlen(argv[argc - 1]) > B_LEN){
            WARNING
		    printf("overflow encounters, max input length : %d\n", B_LEN);
        }
        convert(hexBF2Double((uint16_t)atoh(argv[argc - 1])));
    }
    else if(is_hp){
        // FP16
        if(strlen(argv[argc - 1]) > H_LEN){
            WARNING
		    printf("overflow encounters, max input length : %d\n", H_LEN);
        }
        convert(hexHP2Double((uint16_t)atoh(argv[argc - 1])));
    }
    else if(is_sp){
        // FP32
        if(strlen(argv[argc - 1]) > S_LEN){
            WARNING
		    printf("overflow encounters, max input length : %d\n", S_LEN);
        }
        convert(hexSP2Double((uint32_t)atoh(argv[argc - 1])));
    }
    else if(is_dp){
        // FP64
        if(strlen(argv[argc - 1]) > D_LEN){
            WARNING
		    printf("overflow encounters, max input length : %d\n", D_LEN);
        }
        convert(hexDP2Double(atoh(argv[argc - 1])));
    }
    else{
        convert(atof(argv[argc - 1]));
    }

	return EXIT_SUCCESS;
}

void printUsage(){
    printf("Usage : ./main STR [OPTION]\n");
    printf("Convert STR into various floating-point formats\n\n");
    printf(" STR -b : STR is BF16 format\n");
    printf(" STR -h : STR is FP16 format\n");
    printf(" STR -s : STR is FP32 format\n");
    printf(" STR -d : STR is FP64 format\n");
}
