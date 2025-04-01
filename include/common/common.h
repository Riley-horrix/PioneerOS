/**
 * @file common.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Common functions
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#include "common/types.h"

extern bool beTest;

void common_init(void);

void spin_delay(u32_t count);

// Endianess functions

u16_t beths(u16_t bg32);
u32_t beth(u32_t bg32);
u64_t bethl(u64_t bg32);

#endif // common.h