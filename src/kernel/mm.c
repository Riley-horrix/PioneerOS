#include "kernel/mm.h"
#include "drivers/dtb.h"

/**
 * @brief Initialise the kernel memory map.
 *
 * This function reads the dtb to discover all free memory segments.
 *
 * @param dtb Pointer to the flattened device tree.
 * @return int Memory Map return
 */
enum MemoryMapReturn mm_init(void* dtb) {
    struct fdt_t deviceTree;
    int res = fdt_parse_blob((void*)dtb, &deviceTree);

    if (res != FDT_GOOD) {
        return MM_BAD_DEVICE_TREE;
    }

    fdt_print(&deviceTree);

    return MM_GOOD;
}