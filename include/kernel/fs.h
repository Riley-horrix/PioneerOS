#ifndef KERNEL_FS_H
#define KERNEL_FS_H

// File location index (limits file sizes to 2^32 bytes).
typedef unsigned int loc_t;

typedef struct file_ptr {
    int fd;         // File descriptor.
    loc_t location; // File ptr location.
} FILE;

FILE* fopen(int fd);

#endif