#ifndef COMMON_STRING_H
#define COMMON_STRING_H

#include "common/types.h"

const char* strchr(const char* str, char c);
int strncmp(const char* fst, const char* snd, unsigned int n);
char* strncpy(char* destination, const char* source, size_t num);
#endif