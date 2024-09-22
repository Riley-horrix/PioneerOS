#include "mm.h"

.section ".text.boot"

.globl _start
_start:
  @ mrs r0, mpidr_el1
  and r0, r0, #0xFF
  cmp r0, #0
  beq master
  b proc_hang

master:
  ldr r0, =__bss_start
  ldr r1, =__bss_end
  sub r1, r1, r0
  bl memzero

  mov sp, #LOW_MEMORY
  bl kernel_main
  b proc_hang

proc_hang:
  nop
  b proc_hang
