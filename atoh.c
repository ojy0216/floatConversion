#include "atoh.h"
#include "warning.h"

uint64_t atoh(const char* str){
	uint64_t hex = 0;

	size_t i = 0;
	unsigned int str_len = 0;

	while(isspace(str[i]))
		i++;

	while(isxdigit(str[i])){
		if(str[i] >= '0' && str[i] <= '9')
			hex = (hex << 4) + (str[i] - '0');
		else if(str[i] >= 'a' && str[i] <= 'f')
			hex = (hex << 4) + (str[i] - 'a' + 10);
 		else if(str[i] >= 'A' && str[i] <= 'F')
			hex = (hex << 4) + (str[i] - 'A' + 10);
		i++;
		str_len++;
	}

	if(str_len > ATOH_MAX_LENGTH){
		WARNING
		printf("overflow encounters, max input length : %d\n", ATOH_MAX_LENGTH);
	}

	return hex;
}
