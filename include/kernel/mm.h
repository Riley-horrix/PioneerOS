/**
 * @file mm.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Memory map header functions.
 * @version 0.1
 * @date 2025-04-02
 *
 * Copyright (c) Riley Horrix 2025
 */
#ifndef KERNEL_MM_H
#define KERNEL_MM_H

enum MemoryMapReturn {
    MM_GOOD            = 0,
    MM_BAD_DEVICE_TREE = -1,
};

enum MemoryMapReturn mm_init();

#endif