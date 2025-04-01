/**
 * @file boot.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief First stage bootloader for PioneerOS.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#include "common/common.h"
#include "common/types.h"
#include "drivers/dtb.h"
#include "drivers/mbox.h"
#include "drivers/uart.h"

/**
 * @brief Do all of the low level driver initialisation and setup.
 *
 * @param r0
 * @param r1
 * @param atags
 */
void boot_main(u32_t r0, u32_t r1, u32_t atags) {
    (void)r0;
    (void)r1;

    common_init();

    uart_init();
    uart_puts("\nInitialising PinoeerOS!\r\n");

    struct fdt_t deviceTree;
    int res = fdt_parse_blob((void*)atags, &deviceTree);

    if (!res) {
        uart_puts("Dtb parse failed with code : ");
        uart_puti(res);
        uart_puts("\n");
    }

    while (1) {
        unsigned char in = uart_getch();
        if (in == (unsigned char)'\r') {
            uart_puts("\r\n");
        } else if ((i32_t)in == 127) {
            uart_puts("\e[1D\e[0J");
        } else {
            uart_putch(in);
        }
    }
}