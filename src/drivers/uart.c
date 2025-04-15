/**
 * @file uart.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief UART driver implementation.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#include "drivers/uart.h"
#include "common/common.h"
#include "common/mmio.h"
#include "common/types.h"
#include "drivers/gpio.h"

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

/**
 * @brief Initialise the UART peripheral on GPIO pins 14 & 15.
 */
void uart_init(void) {
    // Disable UART
    __write_barrier();
    write_mmion(UART0_CR, 0x0);

    // Disable pull-up / pull-down & wait 150 clock cycles for the control signal setup.
    write_mmion(GPIO_GPPUD, 0x0);
    spin_delay(150);

    // Propagate pulldown signal to pins 14 & 15.
    write_mmion(GPIO_GPPUDCLK0, (1 << 14) | (1 << 15));
    spin_delay(150);

    // Clear control and clock signal.
    write_mmion(GPIO_GPPUD, 0x0);
    write_mmion(GPIO_GPPUDCLK0, 0x0);

    // Clear pending interrupts.
    write_mmion(UART0_ICR, 0x7ff);

    // Baud rate divisor = UART_CLK / (16 * BAUD_RATE)
    // BAUD_RATE = 115200
    // UART_CLK = 3MHz
    // Baud rate divisor ≈ 1.6276
    // Integer  = 0x1
    // Fraction = 0.625 = 101000 (0.5 + 0.125)
    write_mmion(UART0_IBRD, 0x1);
    write_mmion(UART0_FBRD, 0b101000);

    // Set word length to 8 bits and enable FIFO
    write_mmion(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // Configure UART interrupts
    // 1        : Clear to Send interrupt
    // 4 & 5    : TX / RX ready to send / receive
    // 6        : Receive timeout mask
    // 7 - 10   : Error masks
    write_mmion(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) |
                                (1 << 9) | (1 << 10));

    // Set TX and RX enable
    write_mmion(UART0_CR, (1 << 8) | (1 << 9));
    // Enable UART
    write_mmion(UART0_CR, (1 << 0));
    __read_barrier();
}

/**
 * @brief Write a character to the UART connection.
 *
 * @param c Character to write.
 */
void uart_putch(const char c) {
    // Wait for UART transmit FIFO full to be not full.
    while (read_mmion(UART0_FR) & (1 << 5)) {
    }
    __read_barrier();
    __write_barrier();
    // Write character to data register.
    write_mmion(UART0_DR, (u32_t)c);
}

/**
 * @brief Receive a character from the UART connection.
 *
 * Note: This is a blocking call and will halt the kernel until a character has been received.
 *
 * @return unsigned char The character received.
 */
unsigned char uart_getch() {
    __read_barrier();
    // Wait for UART receive FIFO to not be empty.
    while (read_mmion(UART0_FR) & (1 << 4)) {
    }
    // Read from data register.
    return read_mmion(UART0_DR);
}

/**
 * @brief Write a string to the UART connection.
 *
 * @param str The null terminated string to send.
 */
void uart_puts(const char* str) {
    while (*str != '\0') {
        uart_putch(*str++);
    }
}

#define abs(x) (x >= 0 ? x : -x)

/**
 * @brief Write a signed integer string to the UART connection.
 *
 * @param number The integer to send.
 */
void uart_puti(i32_t number) {

    if (number == 0) {
        uart_putch('0');
        return;
    }

    char buffer[12]; // 10 decimal digits, 1 '-' and 1 null terminator.
    char* head = buffer + 11;
    i32_t save = number;
    *head--    = '\0';
    while (number != 0) {
        *head-- = '0' + abs(number % 10);
        number /= 10;
    }

    if (save < 0) {
        *head = '-';
    } else {
        head++;
    }

    uart_puts(head);
}

/**
 * @brief Write an unsigned integer string to the UART connection.
 *
 * @param number The unsigned number to send.
 */
void uart_putu(u32_t number) {

    if (number == 0) {
        uart_putch('0');
        return;
    }

    char buffer[11]; // 10 decimal digits and 1 null terminator.
    char* head = buffer + 10;

    *head-- = '\0';
    while (number != 0) {
        *head-- = '0' + (number % 10);
        number /= 10;
    }

    head++;

    uart_puts(head);
}

/**
 * @brief Write a long integer string to the UART connection.
 *
 * @param number The long signed number to send.
 */
// void uart_putil(i64_t number) {

//     if (number == 0) {
//         uart_putch('0');
//         return;
//     }

//     char buffer[21]; // 19 decimal digits, 1 '-' and 1 null terminator.
//     char* head = buffer + 20;
//     i64_t save = number;

//     *head-- = '\0';
//     while (number != 0) {
//         *head-- = '0' + abs(number % 10);
//         number /= 10;
//     }

//     if (save < 0) {
//         *head = '-';
//     } else {
//         head++;
//     }

//     uart_puts(head);
// }

/**
 * @brief Write a long unsigned integer to the UART connection.
 *
 * @param number The long unsigned number to send.
 */
// void uart_putul(u64_t number) {

//     if (number == 0) {
//         uart_putch('0');
//         return;
//     }

//     char buffer[20]; // 19 decimal digits and 1 null terminator.
//     char* head = buffer + 19;

//     *head-- = '\0';
//     while (number != 0) {
//         *head-- = '0' + (number % 10);
//         number /= 10;
//     }

//     head++;

//     uart_puts(head);
// }

#define format_hex(x) ((x < 10) ? ('0' + x) : ('A' + (x - 10)))

/**
 * @brief Write a hex formatted unsigned integer to the UART connection.
 *
 * @param number The unsigned integer to send.
 */
void uart_puth(u32_t number) {
    if (!number) {
        uart_puts("0x0");
        return;
    }

    char buffer[11]; // 0, x, 8 hex digits and a null terminator.
    char* head = buffer + 10;

    *head-- = '\0';
    while (number != 0) {
        *head-- = format_hex((number % 16));
        number >>= 4;
    }

    *head-- = 'x';
    *head   = '0';

    uart_puts(head);
}

void uart_puthl(u64_t number) {
    if (!number) {
        uart_puts("0x0");
        return;
    }

    char buffer[19]; // 1 '0', 1 'x', 16 hex digits and a null terminator.
    char* head = buffer + 18;

    *head-- = '\0';
    while (number != 0) {
        *head-- = format_hex((number & 0xf));
        number >>= 4;
    }

    *head-- = 'x';
    *head   = '0';

    uart_puts(head);
}
