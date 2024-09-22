/**
 * @file gpio.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief Implementation of GPIO functionality
 * @version 0.1
 * @date 2024-09-17
 * 
 * Copyright (c) Riley Horrix 2024
 * 
 */
#include "peripheral/gpio.h"

#define GPIO_BASE 0x00200000
#define GPIO_REGS ((GPIORegisters*) (PERI_BASE + GPIO_BASE))

//! @brief Pins per function select register
#define PINS_FUNCTION_REGISTER 10

//! @brief Pins per output set / clear registers
#define PINS_OUTPUT_REGISTER 32

/// @brief Struct for referencing the GPIO Pin functions
typedef struct GPIOPinFunction {
  reg32 __reserved;
  reg32 func[2];
} GPIOPinFunction;

/// @brief Struct for referencing the GPIO peripheral registers
typedef struct GPIORegisters {
  /// @brief Function select array.
  reg32 functionSelect[6];
  /// @brief Pin output set.
  GPIOPinFunction outputSet;
  /// @brief Pin output clear.
  GPIOPinFunction outputClear;
  /// @brief Pin level.
  GPIOPinFunction level;
  /// @brief Event detect status.
  GPIOPinFunction eds;
  /// @brief Rising edge detect enable.
  GPIOPinFunction rede;
  /// @brief Falling edge detect enable.
  GPIOPinFunction fede;
  /// @brief High detect enable.
  GPIOPinFunction hde;
  /// @brief Low detect enable.
  GPIOPinFunction lde;
  /// @brief Asynchronous rising edge detect.
  GPIOPinFunction ared;
  /// @brief Asynchronous falling edge detect.
  GPIOPinFunction afed;
  reg32 __reserved;
  /// @brief Pull up / pull down enable
  reg32 pupde;
  /// @brief Pull up / pull down enable clock 0
  reg32 pupdeClock0;
  /// @brief Pull up / pull down enable clock 1
  reg32 pupdeClock1;
  u8 __buffer[16];
  reg8 test;
} GPIORegisters;


inline void gpio_set_pin_func(pin_t pin, FunctionSelectType func) {
  debug_assert(pin < GPIO_NUMBER_PINS);
  // Function select registers contain 10, 3 bit fields per register.
  u32 reg = GPIO_REGS->functionSelect[pin / PINS_FUNCTION_REGISTER];
  reg |= ((u32) func) << (pin % PINS_FUNCTION_REGISTER);
  GPIO_REGS->functionSelect[pin / PINS_FUNCTION_REGISTER] = reg;
}

inline void gpio_set_pin(pin_t pin) {
  debug_assert(pin < GPIO_NUMBER_PINS);
  // Output set / clear registers contain 32, 1 bit fields per register.
  // This can be made branchless if output set and clear are as an array.
  volatile u32 reg = GPIO_REGS->outputSet.func[pin / PINS_OUTPUT_REGISTER];
  reg |= 0x1 << (pin % PINS_OUTPUT_REGISTER);
  GPIO_REGS->outputSet.func[pin / PINS_OUTPUT_REGISTER] = reg;
}

inline void gpio_clear_pin(pin_t pin) {
  debug_assert(pin < GPIO_NUMBER_PINS);
  // Output set / clear registers contain 32, 1 bit fields per register.
  // This can be made branchless if output set and clear are as an array.
  volatile u32 reg = GPIO_REGS->outputClear.func[pin / PINS_OUTPUT_REGISTER];
  reg |= 0x1 << (pin % PINS_OUTPUT_REGISTER);
  GPIO_REGS->outputSet.func[pin / PINS_OUTPUT_REGISTER] = reg;
}