/**
 * @file common.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief Common function implementation.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#include "common/common.h"
#include "common/types.h"

/**
 * @brief Spin the cpu COUNT cycles.
 *
 * @param count Number of cycles to spin.
 */
inline void spin_delay(u32_t count) {
    asm volatile("__spin_delay_%=: subs %[count], %[count], #1; bne __spin_delay_%=\n"
                 : "=r"(count)
                 : [count] "0"(count)
                 : "cc");
}