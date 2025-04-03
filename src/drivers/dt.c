/**
 * @file dtb.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief Device tree blob implementation file.
 * @version 0.1
 * @date 2025-04-01
 *
 * Copyright (c) Riley Horrix 2025
 */

#include "drivers/dt.h"
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

/// @brief Global instance of the system fdt.
struct dt_t system_fdt;

/**
 * @brief Initialise the global `dt_t` instance with a given fdt.
 * 
 * @param fdt The flattened device tree blob.
 * @return enum dt_return_value_t The return value
 */
enum dt_return_value_t dt_init(void* fdt) {
    return dt_parse_blob(fdt, &system_fdt);
}

/**
 * @brief Parse a device tree located at `fdt`, and write the results into `result`.
 * 
 * This function only parsed the header of the fdt and does not validate any properties or nodes.
 *
 * Returns an enum dt_return_value_t.
 */
enum dt_return_value_t fdt_parse_blob(void* fdt, struct dt_t* result) {
    // The header structure should be located at the start of the blob.
    struct dt_header_t* dtHeader = (struct dt_header_t*)fdt;
    result->header                 = *dtHeader;

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
        return DT_NO_MAGIC;
    }

    // Ensure DTB version is parsable.
    if (result->header.version != FDT_VERSION) {
        return DT_WRONG_VERSION;
    }

    // The reserved memory section is located after the header block.
    result->reserved_mem =
        (struct dt_reserve_entry_t*)((u8_t*)fdt + result->header.off_mem_rsvmap);

    // Set the device tree structure pointer.
    result->structure_block = (void*)((u8_t*)fdt + result->header.off_dt_struct);

    // Set the string dictionary pointer.
    result->strings = (const char*)fdt + result->header.off_dt_strings;

    return DT_GOOD;
}

#define print_tabs(n)                                                                              \
    if (true) {                                                                                    \
        unsigned int i = n;                                                                        \
        while (i-- > 0) {                                                                          \
            uart_putch(' ');                                                                      \
        }                                                                                          \
    }

/**
 * @brief Print a node property.
 *
 * @param prop A pointer to the property data.
 * @return int Number of bytes parsed.
 */
static int dt_print_property(struct dt_t* fdt, struct dt_prop_t* prop, unsigned int level) {
    print_tabs(level);

    uart_puts(fdt->strings + beth(prop->nameoff));
    uart_putch(':');
    u32_t ind = 0;
    while (ind < beth(prop->len)) {
        uart_puth(beth(*(u32_t*)(prop->data + ind)));
        ind += 4;
    }
    uart_putch('\n');

    return (sizeof(struct dt_prop_t) + beth(prop->len) + 3) & ~0x3;
}

#define TAB_INDENT 2

/**
 * @brief Print a node and all of it's children nodes.
 * 
 * @param fdt The fdt in which this node is contained.
 * @param nodePtr A pointer to the node.
 * 
 * @return The status of the print.
 */
static enum dt_return_value_t dt_print_node(struct dt_t* fdt, const u32_t* nodePtr) {
    // Assumes that nodeptr is currently pointing at first FDT_BEGIN_NODE
    // Parse internal tokens
    int level = 0;
    u32_t token;
    while ((token = beth(*nodePtr++)) != FDT_END) {
        switch (token) {
        case FDT_NOP:
            break;
        case FDT_PROP:
            // Print the property
            int len = dt_print_property(fdt, (struct dt_prop_t*)nodePtr, level);
            nodePtr = (const u32_t*)((u8_t*)nodePtr + len);
            break;
        case FDT_BEGIN_NODE:
            // Print the node name
            const char* namePtr = (const char*)nodePtr;

            print_tabs(level);
            uart_puts(namePtr);
            uart_puts(" {\n");

            // Skip over name
            while (*namePtr++)
                ;

            // Move ptr to 4 byte boundary
            nodePtr = (const u32_t*)(((ptr_t)namePtr + 3) & ~0x3);
            level += TAB_INDENT;
            break;
        case FDT_END_NODE:
            level -= TAB_INDENT;
            print_tabs(level);
            uart_puts("}\n");
            if (level == 0) {
                return DT_GOOD;
            }
            break;
        default:
            return DT_INVALID_TOKEN;
        }
    }
}

/**
 * @brief Print a simplified representation of a flattened device tree.
 * 
 * @param fdt The flattened device tree.
 * @return enum dt_return_value_t Return status.
 */
enum dt_return_value_t dt_print(struct dt_t* dt) {
    const u32_t* structurePtrStart = (const u32_t*)dt->structure_block;
    const u32_t* nodePtr = structurePtrStart;

    // Skip over FDT_NOP
    while (beth(*nodePtr) == FDT_NOP) {
        nodePtr++;
    }

    // Check if valid node
    if (beth(*nodePtr) != FDT_BEGIN_NODE) {
        return DT_INVALID_TOKEN;
    }

    return dt_print_node(dt, nodePtr);
}
