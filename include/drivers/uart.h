/**
 * @file uart.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief UART driver.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#ifndef DRIVERS_UART_H
#define DRIVERS_UART_H

#include "common/types.h"

void uart_init();

void uart_putch(unsigned char character);
void uart_puts(const char* string);
void uart_puti(i32_t number);
void uart_puth(u32_t number);

unsigned char uart_getch();

#endif // uart.h