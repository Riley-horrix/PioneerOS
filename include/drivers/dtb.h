/**
 * @file dtb.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Driver for reading the device tree blob.
 * @version 0.1
 * @date 2025-04-01
 *
 * https://devicetree-specification.readthedocs.io/en/stable/flattened-format.html
 * https://www.devicetree.org/specifications
 *
 * The GPU bootcode will fill the dtb in with hardware information about the current running device.
 *
 * Copyright (c) Riley Horrix 2025
 */

#include "common/common.h"
#include "common/types.h"

#define FDT_MAGIC 0xd00dfeed


enum fdt_return_value_t {
    FDT_GOOD = 0,
    FDT_NO_MAGIC = 1,
};

/**
 * @brief The header structure for a flattened device tree.
 *
 */
struct fdt_header_t {
    /**
     * This field shall contain the value 0xd00dfeed (big-endian).
     */
    u32_t magic;

    /**
     * This field shall contain the total size in bytes of the devicetree data structure. This size
     * shall encompass all sections of the structure: the header, the memory reservation block,
     * structure block and strings block, as well as any free space gaps between the blocks or after
     * the final block.
     */
    u32_t totalsize;

    /**
     * This field shall contain the offset in bytes of the structure block (see section 5.4) from
     * the beginning of the header.
     */
    u32_t off_dt_struct;

    /**
     * This field shall contain the offset in bytes of the strings block (see section 5.5) from the
     * beginning of the header.
     */
    u32_t off_dt_strings;

    /**
     * This field shall contain the offset in bytes of the memory reservation block (see
     * section 5.3) from the beginning of the header.
     */
    u32_t off_mem_rsvmap;

    /**
     * This field shall contain the version of the devicetree data structure. The version is 17 if
     * using the structure as defined in this document. An DTSpec boot program may provide the
     * devicetree of a later version, in which case this field shall contain the version number
     * defined in whichever later document gives the details of that version.
     */
    u32_t version;

    /**
     * This field shall contain the lowest version of the devicetree data structure with which the
     * version used is backwards compatible. So, for the structure as defined in this document
     * (version 17), this field shall contain 16 because version 17 is backwards compatible with
     * version 16, but not earlier versions. As per section 5.1, a DTSpec boot program should
     * provide a devicetree in a format which is backwards compatible with version 16, and thus this
     * field shall always contain 16.
     */
    u32_t last_comp_version;

    /**
     * This field shall contain the physical ID of the system’s boot CPU. It shall be identical to
     * the physical ID given in the reg property of that CPU node within the devicetree.
     */
    u32_t boot_cpuid_phys;

    /**
     * This field shall contain the length in bytes of the strings block section of the devicetree
     * blob.
     */
    u32_t size_dt_strings;

    /**
     * This field shall contain the length in bytes of the structure block section of the devicetree
     * blob.
     */
    u32_t size_dt_struct;
};

/**
 * @brief A single reserved memory entry.
 *
 * These structures are 8 byte aligned with the start of the dtb.
 *
 */
struct fdt_reserve_entry_t {
    uint64_t address;
    uint64_t size;
} MEMORY_STRUCT;

/**
 * @brief A structure for storing information about a property within the structure block.
 *
 */
struct fdt_prop_t {
    u32_t len;     // Length of the data block (a 0 value represents an empty property).
    u32_t nameoff; // Offset into the strings block.
    u8_t data[];   // Property data.
} MEMORY_STRUCT;

/**
 * @brief Structure representing the entire flattened device tree.
 *
 */
struct fdt_t {
    struct fdt_header_t header;               // Info containing header.
    struct fdt_reserve_entry_t* reserved_mem; // A list of reserved memory entries.
    char* strings;                          // The strings block.
};

enum fdt_return_value_t fdt_parse_blob(void* addr, struct fdt_t* result);