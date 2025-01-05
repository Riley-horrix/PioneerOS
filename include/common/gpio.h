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

/**
 * @brief Addresses for GPIO memory mapped peripheral IO.
 */
enum GpioPhysicalAddress {
    GPIO_BASE    = 0x20200000,         // GPIO Base address
    GPIO_GPFSEL0 = (GPIO_BASE + 0x00), // Function select registers
    GPIO_GPFSEL1 = (GPIO_BASE + 0x04),
    GPIO_GPFSEL2 = (GPIO_BASE + 0x08),
    GPIO_GPFSEL3 = (GPIO_BASE + 0x0c),
    GPIO_GPFSEL4 = (GPIO_BASE + 0x10),
    GPIO_GPFSEL5 = (GPIO_BASE + 0x14),

    GPIO_GPSET0 = (GPIO_BASE + 0x1c), // Pin output set
    GPIO_GPSET1 = (GPIO_BASE + 0x20),

    GPIO_GPCLR0 = (GPIO_BASE + 0x28), // Pin output clear
    GPIO_GPCLR1 = (GPIO_BASE + 0x2c),

    GPIO_GPLEV0 = (GPIO_BASE + 0x34), // Pin level
    GPIO_GPLEV1 = (GPIO_BASE + 0x38),

    GPIO_GPEDS0 = (GPIO_BASE + 0x40), // Pin event detect status
    GPIO_GPEDS1 = (GPIO_BASE + 0x44),

    GPIO_GPREN0 = (GPIO_BASE + 0x4C), // Pin rising edge detect enable
    GPIO_GPREN1 = (GPIO_BASE + 0x50),

    GPIO_GPFEN0 = (GPIO_BASE + 0x58), // Pin falling edge detect enable
    GPIO_GPFEN1 = (GPIO_BASE + 0x5c),

    GPIO_GPHEN0 = (GPIO_BASE + 0x64), // Pin high detect enable
    GPIO_GPHEN1 = (GPIO_BASE + 0x68),

    GPIO_GPLEN0 = (GPIO_BASE + 0x70), // Pin low detect enable
    GPIO_GPLEN1 = (GPIO_BASE + 0x74),

    GPIO_GPAREN0 = (GPIO_BASE + 0x7c), // Pin async rising edge detect
    GPIO_GPAREN1 = (GPIO_BASE + 0x80),

    GPIO_GPAFEN0 = (GPIO_BASE + 0x88), // Pin async falling edge detect
    GPIO_GPAFEN1 = (GPIO_BASE + 0x8C),

    GPIO_GPPUD     = (GPIO_BASE + 0x94), // Pin pull-up / down enable
    GPIO_GPPUDCLK0 = (GPIO_BASE + 0x98), // Pin pull-up / down enable clock
    GPIO_GPPUDCLK1 = (GPIO_BASE + 0x9C), // Pin pull-up / down enable clock
};

#endif // gpio.h