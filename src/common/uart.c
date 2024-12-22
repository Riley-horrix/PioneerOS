/**
 * @file uart.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief UART driver implementation.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#include "common/uart.h"
#include "common/gpio.h"
#include "common/mmio.h"
#include "common/types.h"

void uart_init() {
    // TODO : Comment and understand these functions!
    __write_barrier();
    write_mmion(UART0_CR, 0x00000000);

    __write_barrier();
    write_mmion(GPPUD, 0x00000000);

    __write_barrier();
    write_mmion(GPPUDCLK0, (1 << 14) | (1 << 15));

    __write_barrier();
    write_mmion(GPPUDCLK0, 0x00000000);

    __write_barrier();
    write_mmion(UART0_ICR, 0x7FF);
    write_mmion(UART0_IBRD, 1);
    write_mmion(UART0_FBRD, 40);
    write_mmion(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
    write_mmion(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
                                (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
    write_mmion(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_putch(unsigned char c) {
    while (read_mmion(UART0_FR) & (1 << 5)) {
    }
    __read_barrier();
    __write_barrier();
    write_mmion(UART0_DR, c);
}

unsigned char uart_getch() {
    while (read_mmion(UART0_FR) & (1 << 4)) {
    }
    return read_mmion(UART0_DR);
}

void uart_puts(const char *str) {
    while (*str != '\0') {
        uart_putch(*str++);
    }
}