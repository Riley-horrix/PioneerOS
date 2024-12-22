/**
 * @file mmio.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Memory Mapped I/O helpers for peripherals.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#ifndef COMMON_MMIO_H
#define COMMON_MMIO_H

#include "common/types.h"

//! @brief Prevents read reordering over different peripheral devices.
#define __read_barrier() asm volatile("" ::: "memory")

//! @brief Prevents write reordering over different peripheral devices.
#define __write_barrier() asm volatile("" ::: "memory")

void write_mmio(reg32_t *reg, u32_t value);
u32_t read_mmio(reg32_t *reg);

void write_mmion(u32_t reg, u32_t value);
u32_t read_mmion(u32_t reg);

#endif