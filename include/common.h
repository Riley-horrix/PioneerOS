/**
 * @file common.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Implementation of a common utilities class
 * @version 0.1
 * @date 2024-09-15
 * 
 * Copyright (c) Riley Horrix 2024
 * 
 */
#pragma once

#include <stdint.h>

#ifdef DEBUG
  #include <assert.h>
  #define debug_assert(bool) assert(bool)
#else
  #define debug_assert(bool) do {} while(0)
#endif

/// Unsigned byte value.
typedef uint8_t u8;
/// Unsigned short value.
typedef uint16_t u16;
/// Unsigned word value.
typedef uint32_t u32;

/// Signed byte value.
typedef int8_t i8;
/// Signed short value.
typedef int16_t i16;
/// Signed word value.
typedef int32_t i32;

/// Register byte value.
typedef volatile u8 reg8;

/// Register word value.
typedef volatile u32 reg32;

/**
 * @brief Reads a byte value at addr.
 * 
 * @param addr The address
 * @return u8 
 */
extern u8 read_byte(void* addr);

/**
 * @brief Writes a data byte at addr.
 * 
 * @param addr The address.
 * @param data The data to write.
 */
extern void write_byte(void* addr, u8 data);

/**
 * @brief Reads a short value at addr.
 * 
 * @param addr The address
 * @return u8 
 */
extern u16 read_short(void* addr);

/**
 * @brief Writes a data short at addr.
 * 
 * @param addr The address.
 * @param data The data to write.
 */
extern void write_short(void* addr, u16 data);

/**
 * @brief Reads a word value at addr.
 * 
 * @param addr The address
 * @return u8 
 */
extern u32 read_word(void* addr);

/**
 * @brief Writes a data word at addr.
 * 
 * @param addr The address.
 * @param data The data to write.
 */
extern void write_word(void* addr, u32 data);