/**
 * @file common.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Common peripheral utilities
 * @version 0.1
 * @date 2024-09-15
 * 
 * Copyright (c) Riley Horrix 2024
 * 
 */
#pragma once

#if PI_VERSION == 1
#define PERI_BASE 0x20000000
#else 
#error "PI_VERSION not supported"
#endif