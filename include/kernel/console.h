#ifndef KERNEL_STDIO_H
#define KERNEL_STDIO_H

#include "fs.h"
#include <stdarg.h>

int printf(const char* format, ...);
int vfprintf(FILE* file, const char* format, va_list ap);

#endif
