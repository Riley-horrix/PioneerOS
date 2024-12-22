/**
 * @file gpio.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Low level gpio driver.
 * @version 0.1
 * @date 2024-12-21
 *
 * Copyright (c) Riley Horrix 2024
 */
#ifndef COMMON_GPIO_H
#define COMMON_GPIO_H

enum {
    // The GPIO registers base address.
    GPIO_BASE = 0x20200000, // 0x3F200000 for raspi2 & 3, 0x20200000 for raspi1

    GPPUD = (GPIO_BASE + 0x94),
    GPPUDCLK0 = (GPIO_BASE + 0x98),
};

#endif // gpio.h