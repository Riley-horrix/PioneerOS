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
    (void)r0;
    (void)r1;
    (void)atags;

    uart_init();
    uart_puts("Hello, World!\r\n");

    uart_puts("\e[0m");

    while (1) {
        unsigned char in = uart_getch();
        if (in == (unsigned char)'\r') {
            uart_puts("\r\n");
        } else {
            uart_putch(in);
        }
    }
}