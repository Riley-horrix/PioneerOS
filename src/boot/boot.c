/**
 * @file boot.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief First stage bootloader for PioneerOS.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#include "common/types.h"
#include "common/uart.h"

/**
 * @brief Do all of the low level driver initialisation and setup.
 *
 * @param r0
 * @param r1
 * @param atags
 */
void boot_main(uint32_t r0, uint32_t r1, uint32_t atags) {
    // (void)r0;
    // (void)r1;
    // (void)atags;

    uart_init();
    uart_puts("Hello, World!\r\n");

    uart_puts("\e[0m");

    if (atags == 0) {
        uart_puts("r0 == 0\n");
    }

    uart_puts("r0 : ");
    uart_puti((i32_t) r0);
    uart_puts(" r1 : ");
    uart_puti((i32_t) r1);
    uart_puts(" tags : ");
    uart_puti((i32_t) atags);

    uart_putch('\n');
    uart_puth((u32_t) *(u32_t*)atags);
    uart_putch('\n');


    while (1) {
        unsigned char in = uart_getch();
        if (in == (unsigned char)'\r') {
            uart_puts("\r\n");
        } else if ((i32_t) in == 127) {
            uart_puts("\e[1D\e[0J");
        } else {
            uart_putch(in);
        }
    }
}