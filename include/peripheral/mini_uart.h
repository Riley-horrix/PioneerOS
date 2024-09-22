/**
 * @file mini_uart.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief UART header file
 * @version 0.1
 * @date 2024-09-22
 * 
 * Copyright (c) Riley Horrix 2024
 * 
 */
#pragma once

/**
 * @brief Initialise the mini UART auxiliary peripheral.
 */
extern void uart_init();

/**
 * @brief Receive a char value from the UART.
 * 
 * @return char The value read.
 */
extern char uart_recv();

/**
 * @brief Send a single character through the UART auxiliary peripheral.
 * 
 * @param c The character to send.
 */
extern void uart_send(const char c);

/**
 * @brief Send a string through the UART auxiliary peripheral.
 * 
 * @param str A pointer to the string to send. 
 * Note that the string must be null terminated.
 */
extern void uart_send_string(const char* str);