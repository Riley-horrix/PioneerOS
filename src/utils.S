.globl delay
delay:
  subs r0, r0, #1
  bne delay
  bx lr

