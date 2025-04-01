/**
 * @file types.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Types.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdint.h>

// Unsigned Integer Types
typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef uint64_t u64_t;

// Signed Integer Types
typedef int8_t i8_t;
typedef int16_t i16_t;
typedef int32_t i32_t;
typedef int64_t i64_t;

// Register Integer Types
typedef volatile u8_t reg8_t;
typedef volatile u16_t reg16_t;
typedef volatile u32_t reg32_t;
typedef volatile u64_t reg64_t;

// Boolean
typedef int bool;
#define true 1
#define false 0

#endif