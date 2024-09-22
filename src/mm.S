.globl memzero
memzero:
  mov r2, #0
  strb r2, [r0]
  subs r1, r1, #8
  bgt memzero
  bx lr