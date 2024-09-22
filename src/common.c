/**
 * @file common.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief Implementation of common functions
 * @version 0.1
 * @date 2024-09-15
 * 
 * Copyright (c) Riley Horrix 2024
 * 
 */
#include "common.h"

u8 read_byte(void* _addr) {
  u8* addr = (u8*) _addr;
  return *addr;
}

void write_byte(void* _addr, u8 data) {
  u8* addr = (u8*) _addr;
  *addr = data;
}

u16 read_short(void* _addr) {
  u16* addr = (u16*) _addr;
  return *addr;
}

void write_short(void* _addr, u16 data) {
  u16* addr = (u16*) _addr;
  *addr = data;
}

u32 read_word(void* _addr) {
  u32* addr = (u32*) _addr;
  return *addr;
}

void write_word(void* _addr, u32 data) {
  u32* addr = (u32*) _addr;
  *addr = data;
}