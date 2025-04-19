#ifndef COMMON_STRING_H
#define COMMON_STRING_H

#include "common/types.h"

extern size_t strlen(const char* str);
extern const char* strchr(const char* str, char c);
extern int strncmp(const char* fst, const char* snd, unsigned int n);
extern char* strncpy(char* destination, const char* source, size_t num);

#endif