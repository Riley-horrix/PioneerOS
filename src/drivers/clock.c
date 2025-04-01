#include "drivers/clock.h"
#include "common/mmio.h"

enum ClockPhysicalAddress {
    CLOCK_BASE                = 0x20003000,
    CLOCK_CONTROL_REG         = (CLOCK_BASE + 0x00),
    CLOCK_SYS_TIMER_LOW       = (CLOCK_BASE + 0x04),
    CLOCK_SYS_TIMER_HIGH      = (CLOCK_BASE + 0x08),
    CLOCK_SYS_TIMER_COMPARE_0 = (CLOCK_BASE + 0x0c),
    CLOCK_SYS_TIMER_COMPARE_1 = (CLOCK_BASE + 0x10),
    CLOCK_SYS_TIMER_COMPARE_2 = (CLOCK_BASE + 0x14),
    CLOCK_SYS_TIMER_COMPARE_3 = (CLOCK_BASE + 0x18),
};

/**
 * @brief Get the current system clock in microseconds.
 *
 * @return u64_t Microseconds since power on.
 */
u64_t clock_micros(void) {
    return (u64_t)read_mmion(CLOCK_SYS_TIMER_LOW) | ((u64_t)read_mmion(CLOCK_SYS_TIMER_HIGH) << 32);
}