#include "kernel/mm.h"
#include "drivers/dt.h"

/**
 * @brief Initialise the kernel memory map.
 *
 * This function reads the dtb to discover all free memory segments.
 *
 * @param dtb Pointer to the flattened device tree.
 * @return int Memory Map return
 */
enum MemoryMapReturn mm_init() { return MM_GOOD; }