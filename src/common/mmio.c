/**
 * @file mmio.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief Memory Mapped IO helper implementation.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#include "common/mmio.h"

/**
 * @brief Write a value to a peripheral MMIO register.
 *
 * Note that writes (although unlikely) may be reordered by the compiler or
 * data be sent out of synch, so you should have a memory read barrier
 * __write_barrier(); before the first write to a peripheral.
 *
 * @param reg Address of the register.
 * @param value Value to write.
 */
inline void write_mmio(reg32_t* reg, u32_t value) { *reg = value; }

/**
 * @brief Read a register from a peripheral MMIO register.
 *
 * Note that reads may be reordered by the compiler or data will arrive
 * out of synch, so you should have a memory read barrier __read_barrier();
 * after the last read from the same peripheral.
 *
 * @param reg Address of the register.
 * @return u32_t Value at that address.
 */
inline u32_t read_mmio(reg32_t* reg) { return *reg; }

/**
 * @brief Write a value to a peripheral MMIO register.
 *
 * Like write_mmio() but casts the unsigned int to a volatile int pointer.
 *
 * Note that writes (although unlikely) may be reordered by the compiler or
 * data be sent out of synch, so you should have a memory read barrier
 * __write_barrier(); before the first write to a peripheral.
 *
 * @param reg Address of the register.
 * @param value Value to write.
 */
inline void write_mmion(u32_t reg, u32_t value) { write_mmio((reg32_t*)reg, value); }

/**
 * @brief Read a register from a peripheral MMIO register.
 *
 * Like read_mmio() but casts the unsigned int to a volatile int pointer.
 *
 * Note that reads may be reordered by the compiler or data will arrive
 * out of synch, so you should have a memory read barrier __read_barrier();
 * after the last read from the same peripheral.
 *
 * @param reg Address of the register.
 * @return u32_t Value at that address.
 */
inline u32_t read_mmion(u32_t reg) { return read_mmio((reg32_t*)reg); }
