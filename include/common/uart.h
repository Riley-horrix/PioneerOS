/**
 * @file uart.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief UART driver.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#ifndef COMMON_UART_H
#define COMMON_UART_H

#include "common/types.h"

/**
 * @brief Addresses for UART peripheral memory mapped IO.
 *
 * Peripheral bus addresses start at 0x7E000000
 * Mapped physical addresses start at 0x20000000
 */
enum UartPhysicalAddress {
    UART0_BASE   = 0x20201000,          // Base address
    UART0_DR     = (UART0_BASE + 0x00), // Data register
    UART0_RSRECR = (UART0_BASE + 0x04), // Receive status register / error clear register
    UART0_FR     = (UART0_BASE + 0x18), // Flag register
    UART0_ILPR   = (UART0_BASE + 0x20), // Integer baud rate divisor
    UART0_IBRD   = (UART0_BASE + 0x24), // Integer baud rate divisor
    UART0_FBRD   = (UART0_BASE + 0x28), // Fractional baud rate divisor
    UART0_LCRH   = (UART0_BASE + 0x2C), // Line control register
    UART0_CR     = (UART0_BASE + 0x30), // Control register
    UART0_IFLS   = (UART0_BASE + 0x34), // Interrupt FIFO level select register
    UART0_IMSC   = (UART0_BASE + 0x38), // Interrupt mask set clear register
    UART0_RIS    = (UART0_BASE + 0x3C), // Raw interrupt status register
    UART0_MIS    = (UART0_BASE + 0x40), // Masked interrupt status register
    UART0_ICR    = (UART0_BASE + 0x44), // Interrupt clear register
    UART0_DMACR  = (UART0_BASE + 0x48), // DMA control register
    UART0_ITCR   = (UART0_BASE + 0x80), // Test control register
    UART0_ITIP   = (UART0_BASE + 0x84), // Integration test input register
    UART0_ITOP   = (UART0_BASE + 0x88), // Integration test output register
    UART0_TDR    = (UART0_BASE + 0x8C), // Test data register
};

void uart_init();

void uart_putch(unsigned char character);
void uart_puts(const char* string);
void uart_puti(i32_t number);
void uart_puth(u32_t number);

unsigned char uart_getch();

#endif // uart.h