#ifndef __ATOH_H__
#define __ATOH_H__

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#define ATOH_MAX_LENGTH 16

uint64_t atoh(const char* str);

#endif
