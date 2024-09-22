/**
 * @file gpio.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief GPIO function file
 * @version 0.1
 * @date 2024-09-15
 * 
 * Copyright (c) Riley Horrix 2024
 * 
 */
#pragma once

#include "../common.h"
#include "peripheral/common.h"

#define GPIO_NUMBER_PINS 54

/**
 * @brief Functions for the Pin Function Select array.
 */
typedef enum {
  INPUT = 0b000,
  OUTPUT = 0b001,
  ALT0 = 0b100,
  ALT1 = 0b101,
  ALT2 = 0b110,
  ALT3 = 0b111,
  ALT4 = 0b011,
  ALT5 = 0b010
} FunctionSelectType;

typedef u8 pin_t;

/**
 * @brief Pin level, used to set or clear pins.
 */
typedef enum {
  HIGH = 1,
  LOW = 0
} PinLevel;

/**
 * @brief The function select registers are used to define the function of the
 * general purpose I/O pins.
 * 
 * All unused alternative function lines are tied to ground.
 * 
 * @param pin The pin number
 * @param func Function to set
 */
extern inline void gpio_set_pin_func(pin_t pin, FunctionSelectType func);

/**
 * @brief Writes a zero to a pin. Pin must be set to OUTPUT prior to calling this function to see an output.
 */
extern inline void gpio_set_pin(pin_t pin);

/**
 * @brief Writes a zero to a pin. Pin must be set to OUTPUT prior to calling this function to see an output.
 */
extern inline void gpio_clear_pin(pin_t pin);