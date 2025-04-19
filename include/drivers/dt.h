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

#ifndef DRIVERS_DTB_H
#define DRIVERS_DTB_H

#include "common/defines.h"
#include "common/types.h"

#define FDT_MAGIC   0xd00dfeed
#define FDT_VERSION 17

enum dt_return_value_t {
    DT_GOOD             = 0,
    DT_NO_MAGIC         = -1,
    DT_WRONG_VERSION    = -2,
    DT_INVALID_TOKEN    = -3,
    DT_INVALID_PATH     = -4,
    DT_NO_MORE_SIBLINGS = -5,
    DT_NO_MORE_PROPS    = -6,
    DT_NO_CHILDREN      = -7,
    DT_INVALID_ITER     = -8,
    DT_PATH_NOT_FOUND   = -9,
};

// Forward decls

struct dt_node_iter_t;

// General

extern enum dt_return_value_t dt_init(void* fdt);

// Searching

extern enum dt_return_value_t dt_get_node_by_path(const char* path);

// Iterators

extern enum dt_return_value_t dt_iter_init_node(struct dt_node_iter_t* iter);
extern enum dt_return_value_t dt_iter_next_sibling(struct dt_node_iter_t* iter);
extern enum dt_return_value_t dt_iter_first_child(struct dt_node_iter_t* iter);
extern enum dt_return_value_t dt_iter_namen(struct dt_node_iter_t* iter, char* out, size_t n);
#endif