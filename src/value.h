/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

 #ifndef H__SCLSH__INTERNAL_VALUE
#define H__SCLSH__INTERNAL_VALUE

#include <sclsh/value.h>
#include <sclsh/ast.h>

struct s_SclshValue {
    long ref_count;  // Reference count for memory management
    
    char* string;  // Pointer to the string data
    size_t length;  // Length of the string

    SclshValueList* as_list;
    SclshValueList* as_proc;
    SclshNodeList* as_command_line;
    SclshNodeList* as_interpolation;
};

struct s_SclshValueList {
    size_t count;  // Number of items in the list
    SclshValue* items[];  // Array of pointers to SclshValue
};

#endif