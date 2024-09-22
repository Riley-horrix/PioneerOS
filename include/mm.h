/**
 * @file mm.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Header providing memory management declarations.
 * @version 0.1
 * @date 2024-09-22
 * 
 * Copyright (c) Riley Horrix 2024
 * 
 */
#pragma once


#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)

#ifndef __ASSEMBLER__

#include "common.h"

/** mm.S */

/**
 * @brief Set n bytes of memory pointed to by src to zero. 
 * 
 * @param src Pointer to start of memory.
 * @param n Number of bytes to set to zero.
 */
extern void memzero(u8* src, u32 n);

#endif