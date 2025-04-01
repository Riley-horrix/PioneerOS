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
#include "common/common.h"
#include "drivers/mbox.h"
#include "drivers/uart.h"
#include "drivers/dtb.h"

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
    int res = fdt_parse_blob((void*) atags, &deviceTree);

    if (res == FDT_NO_MAGIC) {
        uart_puts("ERROR : NO FDT MAGIC!\n");
        uart_puth(deviceTree.header.magic);
        uart_puts("\n");
        uart_puth(FDT_MAGIC);
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