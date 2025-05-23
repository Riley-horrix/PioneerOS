#include "kernel/mm.h"
#include "common/common.h"
#include "common/types.h"
#include "drivers/mbox.h"
#include "drivers/dt.h"

enum MemoryOwnership {
    ARM      = 0,
    VC       = 1,
    RESERVED = 2,
    MMIO     = 3
};

struct MemorySegment {
    u32_t base;
    u32_t size;
    enum MemoryOwnership owner;
};

/**
 * @brief Array of available memory segments.
 * 
 * The size of this array means that this OS only supports a maximum of 32 different memory segments.
 */
struct MemorySegment available_memory[32];

/**
 * @brief Initialise the kernel memory map.
 *
 * This function reads the dtb to discover all free memory segments.
 * 
 * Requires the dtb driver to be initialised.
 *
 * @param dtb Pointer to the flattened device tree.
 * @return int Memory Map return
 */
enum MemoryMapReturn mm_init() { 
    struct MailboxMemoryIterator iter;

    // Get the arm memory base and size
    mailbox_mem_iter_init(&iter, true);

    if (mailbox_mem_iter_next(&iter) != MBOX_GOOD) {
        return MM_FAILED_INIT;
    }

    u32_t arm_base = iter.base;
    u32_t arm_size = iter.size;

    // Get the vc memory base and size
    mailbox_mem_iter_init(&iter, false);
    if (mailbox_mem_iter_next(&iter) != MBOX_GOOD) {
        return MM_FAILED_INIT;
    }

    u32_t vc_base = iter.base;
    u32_t vc_size = iter.size;

    // Get reserved memory from the dtb
    struct DeviceTreeNode* node = dt_get_node_by_path("/reserved-memory");
    if (node == NULL) {
        return MM_FAILED_INIT;
    }

    // node.

    return MM_GOOD;
}