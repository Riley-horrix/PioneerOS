/**
 * @file dtb.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief Device tree blob implementation file.
 * @version 0.1
 * @date 2025-04-01
 *
 * Copyright (c) Riley Horrix 2025
 */

#include "drivers/dtb.h"
#include "common/common.h"
#include "common/types.h"
#include "drivers/uart.h"

/* ----- Tokens ----- */
/* Each token is 32 bit aligned, which means padding the previous token with 0x0. */
/* The tree structure is 4 byte aligned. */

/**
 * @brief The FDT_BEGIN_NODE token marks the beginning of a node’s representation. It shall be
 * followed by the node’s unit name as extra data. The name is stored as a null-terminated string,
 * and shall include the unit address, if any. The node name is followed by zeroed padding bytes, if
 * necessary for alignment, and then the next token, which may be any token except FDT_END.
 */
#define FDT_BEGIN_NODE 0x00000001

/**
 * @brief The FDT_END_NODE token marks the end of a node’s representation. This token has no extra
 * data; so it is followed immediately by the next token, which may be any token except FDT_PROP.
 *
 */
#define FDT_END_NODE 0x00000002

/**
 * @brief The FDT_PROP token marks the beginning of the representation of one property in the
 * devicetree. It shall be followed by extra data describing the property. This data consists first
 * of the property’s length and name represented as the following C structure, `struct fdt_prop`.
 *
 * Both the fields in this structure are 32-bit big-endian integers.
 *
 * - `len` gives the length of the property’s value in bytes (which may be zero, indicating an empty
 * property).
 *
 * - `nameoff` gives an offset into the strings block at which the property’s name is stored as a
 * null-terminated string. After this structure, the property’s value is given as a byte string of
 * length `len`. This value is followed by zeroed padding bytes (if necessary) to align to the next
 * 32-bit boundary and then the next token, which may be any token except FDT_END.
 */
#define FDT_PROP 0x00000003

/**
 * @brief The FDT_NOP token will be ignored by any program parsing the device tree. This token has
 * no extra data; so it is followed immediately by the next token, which can be any valid token. A
 * property or node definition in the tree can be overwritten with FDT_NOP tokens to remove it from
 * the tree without needing to move other sections of the tree’s representation in the devicetree
 * blob.
 *
 */
#define FDT_NOP 0x00000004

/**
 * @brief The FDT_END token marks the end of the structure block. There shall be only one FDT_END
 * token, and it shall be the last token in the structure block. It has no extra data; so the byte
 * immediately after the FDT_END token has offset from the beginning of the structure block equal to
 * the value of the size_dt_struct field in the device tree blob header.
 *
 */
#define FDT_END 0x00000009

/**
 * @brief Parse a device tree located at `addr`, and write the results into `result`.
 *
 * Returns an enum fdt_return_value_t.
 */
enum fdt_return_value_t fdt_parse_blob(void* addr, struct fdt_t* result) {
    // The header structure should be located at the start of the blob.
    struct fdt_header_t* fdtHeader = (struct fdt_header_t*)addr;
    result->header                 = *fdtHeader;

    // The values are stored as little endian so translate all of the values.
    result->header.magic             = beth(result->header.magic);
    result->header.totalsize         = beth(result->header.totalsize);
    result->header.off_dt_struct     = beth(result->header.off_dt_struct);
    result->header.off_dt_strings    = beth(result->header.off_dt_strings);
    result->header.off_mem_rsvmap    = beth(result->header.off_mem_rsvmap);
    result->header.version           = beth(result->header.version);
    result->header.last_comp_version = beth(result->header.last_comp_version);
    result->header.boot_cpuid_phys   = beth(result->header.boot_cpuid_phys);
    result->header.size_dt_strings   = beth(result->header.size_dt_strings);
    result->header.size_dt_struct    = beth(result->header.size_dt_struct);

    // Ensure that the magic is valid
    if (result->header.magic != FDT_MAGIC) {
        return FDT_NO_MAGIC;
    }

    // Ensure DTB version is parsable.
    if (result->header.version != FDT_VERSION) {
        return FDT_WRONG_VERSION;
    }

    // The reserved memory section is located after the header block, and aligned to an 8 byte boundary.
    result->reserved_mem =
        (struct fdt_reserve_entry_t*)(((ptr_t)addr + sizeof(struct fdt_header_t) + 0x7) & ~0x7);

    // Set the device tree structure pointer.
    result->structure_block = (void*)((u8_t*)addr + result->header.off_dt_strings);

    // Set the string dictionary pointer.
    result->strings = (const char*)addr + result->header.off_dt_strings;

    return FDT_GOOD;
}