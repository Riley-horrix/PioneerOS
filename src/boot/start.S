.section ".text.boot"

.global _start

_start:
    @ Point the stack to 0x8000 since OS starts at 0x8000
    @ (and stack grows downwards)
    mov sp, #0x10000

    @ Set bss region to 0
@     ldr r3, =__bss_start
@     ldr r4, =__bss_end
@     mov r5, #0x0
@     mov r7, #0x4
@     b is_end

@ store_zero:
@     str r5, [r3, +r7]

@ is_end:
@     cmp r4, r4
@     blo store_zero

    @ Jump to C code in kernel.c
    ldr r3, =boot_main
    blx r3

    @ Halt if kernel exits
    b halt

halt:
    wfi
    b halt
