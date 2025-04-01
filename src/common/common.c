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

static u32_t test = 0x1234;

/**
 * @brief Boolean representing system endianess, configured after `common_init()`. If `beTest` then
 * the system is big endian.
 */
bool beTest = false;

/**
 * @brief Initialise common functions.
 *
 */
void common_init(void) { beTest = *(u8_t*)&test == (u8_t)0x12; }

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

/**
 * @brief Big endian to host short.
 *
 * @param bg32 The value to translate.
 * @return u32_t The translated value.
 */
u16_t beths(u16_t bg16) {
    if (beTest) {
        return bg16;
    } else {
        return ((bg16 & 0xff00) >> 8) | ((bg16 & 0xff) << 8);
    }
}

/**
 * @brief Big endian to host.
 *
 * @param bg32 The value to translate.
 * @return u32_t The translated value.
 */
u32_t beth(u32_t bg32) {
    if (beTest) {
        return bg32;
    } else {
        return ((bg32 & 0xff) << 24) | ((bg32 & 0xff00) << 8) | ((bg32 & 0xff0000) >> 8) |
               ((bg32 & 0xff000000) >> 24);
    }
}

/**
 * @brief Big endian to host long.
 *
 * @param bg32 The value to translate.
 * @return u64_t The translated value.
 */
u64_t bethl(u64_t bg64) {
    if (beTest) {
        return bg64;
    } else {
        return ((bg64 & 0xff) << 56) | ((bg64 & 0xff00) << 40) | ((bg64 & 0xff0000) << 24) |
               ((bg64 & 0xff000000) << 8) | ((bg64 & 0xff00000000) >> 8) |
               ((bg64 & 0xff0000000000) >> 24) | ((bg64 & 0xff000000000000) >> 40) |
               ((bg64 & 0xff00000000000000) >> 56);
    }
}