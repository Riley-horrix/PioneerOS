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
#include "common/defines.h"
#include "common/string.h"
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
 * @brief The header structure for a flattened device tree.
 *
 * - `u32_t magic`
 * This field shall contain the value 0xd00dfeed (big-endian).
 *
 * - `u32_t totalsize`
 * This field shall contain the total size in bytes of the devicetree data structure. This size
 * shall encompass all sections of the structure: the header, the memory reservation block,
 * structure block and strings block, as well as any free space gaps between the blocks or after
 * the final block.
 *
 * - `u32_t off_dt_struct`
 * This field shall contain the offset in bytes of the structure block (see section 5.4) from
 * the beginning of the header.
 *
 * - `u32_t off_dt_strings`
 * This field shall contain the offset in bytes of the strings block (see section 5.5) from the
 * beginning of the header.
 *
 * - `u32_t off_mem_rsvmap`
 * This field shall contain the offset in bytes of the memory reservation block (see
 * section 5.3) from the beginning of the header.
 *
 * - `u32_t version`
 * This field shall contain the version of the devicetree data structure. The version is 17 if
 * using the structure as defined in this document. An DTSpec boot program may provide the
 * devicetree of a later version, in which case this field shall contain the version number
 * defined in whichever later document gives the details of that version.
 *
 * - `u32_t last_comp_version`
 * This field shall contain the lowest version of the devicetree data structure with which the
 * version used is backwards compatible. So, for the structure as defined in this document
 * (version 17), this field shall contain 16 because version 17 is backwards compatible with
 * version 16, but not earlier versions. As per section 5.1, a DTSpec boot program should
 * provide a devicetree in a format which is backwards compatible with version 16, and thus this
 * field shall always contain 16.
 *
 * - `u32_t boot_cpuid_phys`
 * This field shall contain the physical ID of the system’s boot CPU. It shall be identical to
 * the physical ID given in the reg property of that CPU node within the devicetree.
 *
 * - `u32_t size_dt_strings`
 * This field shall contain the length in bytes of the strings block section of the devicetree
 * blob.
 *
 * - `u32_t size_dt_struct`
 * This field shall contain the length in bytes of the structure block section of the devicetree
 * blob.
 */
struct dt_header_t {
    u32_t magic;             // Should be 0xd00dfeed
    u32_t totalsize;         // Total size of all blocks
    u32_t off_dt_struct;     // Offset of the structure block
    u32_t off_dt_strings;    // Offset of the strings dictionary.
    u32_t off_mem_rsvmap;    // Offset of the reserved memory map.
    u32_t version;           // Dtb version
    u32_t last_comp_version; // Last compatible version
    u32_t boot_cpuid_phys;   // Physical cpu id
    u32_t size_dt_strings;   // Size of the strings dictionary
    u32_t size_dt_struct;    // Size of the structure block
};

/**
 * @brief A single reserved memory entry.
 *
 * These structures are 8 byte aligned with the start of the dtb.
 *
 */
struct dt_reserve_entry_t {
    uint64_t address;
    uint64_t size;
} MEMORY_STRUCT;

/**
 * @brief A structure for storing information about a property within the structure block.
 *
 */
struct dt_prop_t {
    u32_t len;     // Length of the data block (a 0 value represents an empty property).
    u32_t nameoff; // Offset into the strings block.
    u8_t data[];   // Property data.
} MEMORY_STRUCT;

/**
 * @brief Structure representing the entire flattened device tree.
 *
 */
struct dt_t {
    struct dt_header_t header;               // Info containing header.
    struct dt_reserve_entry_t* reserved_mem; // A list of reserved memory entries.
    void* structure_block;                   // Device tree structure.
    const char* strings;                     // The strings block.
};

/**
 * @brief Structure to store information about a node iterator.
 *
 */
struct dt_node_iter_t {
    const struct dt_t* device_tree; // Device tree of the node.
    const u32_t* node_ptr;          // Pointer to the FDT_BEGIN_NODE tag of this node.
};

// Forward decls

static enum dt_return_value_t dt_parse_blob(void* fdt, struct dt_t* result);

/// @brief Global instance of the system fdt.
struct dt_t system_dt;

/**
 * @brief Initialise the global `dt_t` instance with a given fdt.
 *
 * @param fdt Pointer to the flattened device tree blob.
 * @return enum dt_return_value_t The return value
 */
enum dt_return_value_t dt_init(void* fdt) { return dt_parse_blob(fdt, &system_dt); }

const char* rootName = "/";

static bool verify_node_iter(struct dt_node_iter_t* iter) {
    return beth(*iter->node_ptr) == FDT_BEGIN_NODE;
}

/**
 * @brief Initialise given node iterator to point at the root node, '/'.
 *
 * @param iter The node iterator.
 * @return enum dt_return_value_t Return value.
 */
enum dt_return_value_t dt_iter_init_node(struct dt_node_iter_t* iter) {
    iter->device_tree           = &system_dt;
    const u32_t* structureStart = (const u32_t*)system_dt.structure_block;

    // Skip possible NOPS
    while (beth(*structureStart) == FDT_NOP) {
        structureStart++;
    }

    if (beth(*structureStart) != FDT_BEGIN_NODE) {
        return DT_INVALID_TOKEN;
    }

    iter->node_ptr = structureStart;
    return DT_GOOD;
}

/**
 * @brief Advance the iterator to the next sibling of the node pointed to by the current iterator.
 *
 * @param iter The iterator to move.
 * @return enum dt_return_value_t Return value.
 */
enum dt_return_value_t dt_iter_next_sibling(struct dt_node_iter_t* iter) {
    const u32_t* nodePtr = iter->node_ptr;

    if (!verify_node_iter(iter)) {
        return DT_INVALID_ITER;
    }

    u32_t token;
    int level = 0;

    while ((token = *nodePtr++) != FDT_END) {
        if (token == FDT_BEGIN_NODE) {
            level++;
        }
        if (token == FDT_END_NODE) {
            level--;
            if (level == 0) {
                break;
            }
        }
        if (token == FDT_PROP) {
            // Skip over the property
            struct dt_prop_t* prop = (struct dt_prop_t*)nodePtr;
            int bytesToSkip        = (sizeof(struct dt_prop_t) + beth(prop->len) + 3) & ~0x3;
            nodePtr                = (const u32_t*)((u8_t*)nodePtr + bytesToSkip);
        }
    }

    if (token == FDT_END) {
        return DT_NO_MORE_SIBLINGS;
    }

    // Skip over nop
    while (beth(*nodePtr) == FDT_NOP) {
        nodePtr++;
    }

    if (beth(*nodePtr) != FDT_BEGIN_NODE) {
        return DT_INVALID_TOKEN;
    }

    iter->node_ptr = nodePtr;
    return DT_GOOD;
}

/**
 * @brief Advance the iterator into the node, and onto the first child of the node.
 *
 * @param iter The iterator to move.
 * @return enum dt_return_value_t Return value. TODO (whole file)
 */
enum dt_return_value_t dt_iter_first_child(struct dt_node_iter_t* iter) {
    const u32_t* nodePtr = iter->node_ptr;

    if (!verify_node_iter(iter)) {
        return DT_INVALID_ITER;
    }

    u32_t token;

    // Skip over possible properties
    while ((token = *nodePtr++) != FDT_END) {
        if (token == FDT_END_NODE) {
            return DT_NO_CHILDREN;
        }

        if (token == FDT_PROP) {
            // Skip over the property
            struct dt_prop_t* prop = (struct dt_prop_t*)nodePtr;
            int bytesToSkip        = (sizeof(struct dt_prop_t) + beth(prop->len) + 3) & ~0x3;
            nodePtr                = (const u32_t*)((u8_t*)nodePtr + bytesToSkip);
        }

        if (token == FDT_BEGIN_NODE) {
            nodePtr--; // Move nodePtr back onto tag
            break;
        }
    }

    if (token == FDT_END) {
        return DT_NO_CHILDREN;
    }

    iter->node_ptr = nodePtr;

    return DT_GOOD;
}

/**
 * @brief Read the value of the name of the current node that `iter` is pointing to, and writes `n`
 * bytes of it into `out`.
 *
 * @param iter The node iterator.
 * @param out Where to write the name to.
 * @param n Maximal number of characters to write.
 * @return enum dt_return_value_t Return value;
 */
enum dt_return_value_t dt_iter_namen(struct dt_node_iter_t* iter, char* out, size_t n) {
    if (!verify_node_iter(iter)) {
        return DT_INVALID_ITER;
    }

    strncpy(out, (const char*)(++iter->node_ptr), n);

    return DT_GOOD;
}

/**
 * @brief Parse a device tree located at `fdt`, and write the results into `result`.
 *
 * This function only parsed the header of the fdt and does not validate any properties or nodes.
 *
 * Returns an enum dt_return_value_t.
 */
static enum dt_return_value_t dt_parse_blob(void* fdt, struct dt_t* result) {
    // The header structure should be located at the start of the blob.
    struct dt_header_t* dtHeader = (struct dt_header_t*)fdt;
    result->header               = *dtHeader;

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
    result->reserved_mem = (struct dt_reserve_entry_t*)((u8_t*)fdt + result->header.off_mem_rsvmap);

    // Set the device tree structure pointer.
    result->structure_block = (void*)((u8_t*)fdt + result->header.off_dt_struct);

    // Set the string dictionary pointer.
    result->strings = (const char*)fdt + result->header.off_dt_strings;

    return DT_GOOD;
}

/**
 * @brief Get a node by path.
 *
 * @param path The null terminated node name.
 * @return enum dt_return_value_t Return value.
 */
enum dt_return_value_t dt_get_node_by_path(const char* path, struct dt_node_iter_t* node) {
    struct dt_node_iter_t iter;
    enum dt_return_value_t status = dt_iter_init_node(&iter);
    if (status != DT_GOOD) {
        return status;
    }

    // Node name buffer (255 chars + 1 null terminator)
    char name[256];

    int pathInd = 0;

    // Write name of root node into the buffer
    status = dt_iter_namen(&iter, name, sizeof(name));
    if (status != DT_GOOD) {
        return status;
    }

    while (pathInd < strlen(path)) {
        // Compare the name to the path
        if (strncmp(name, path, strlen(name)) == 0) {
            // If the name and the path is the same, then either go to children, or return.
            if (pathInd + strlen(name) == strlen(path)) {
                // If the path is the same, then return the node.
                *node = iter;
                return DT_GOOD;
            }
            return DT_GOOD;
        }

        // Move to the next sibling
        status = dt_iter_next_sibling(&iter);
        if (status != DT_GOOD) {
            return status;
        }
    }

    return DT_PATH_NOT_FOUND;
}




#define print_tabs(n)                                                                              \
    if (true) {                                                                                    \
        unsigned int i = n;                                                                        \
        while (i-- > 0) {                                                                          \
            uart_putch(' ');                                                                       \
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
UNUSED static enum dt_return_value_t dt_print(struct dt_t* dt) {
    const u32_t* structurePtrStart = (const u32_t*)dt->structure_block;
    const u32_t* nodePtr           = structurePtrStart;

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
