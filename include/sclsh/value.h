/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

 #ifndef H__SCLSH__VALUE_H
#define H__SCLSH__VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

typedef struct s_SclshValue SclshValue;

typedef enum {
    SCLSH_VALUE_TYPE_BYTES,
    SCLSH_VALUE_TYPE_LIST,
    SCLSH_VALUE_TYPE_AST_NODE,
} SclshValueType;

struct s_SclshValue {
    long ref_count;  // Reference count for memory management
    SclshValueType type;  // Type of the value (bytes or list)
    
    char* string;  // Pointer to the string data
    size_t length;  // Length of the string

    void* data;  // Pointer to additional data
};

SclshValue* sclsh_value_new(SclshValueType type, 
                            const char* string, 
                            size_t length,
                            void* data);
SclshValue* sclsh_value_ref(SclshValue* value);
void sclsh_value_unref(SclshValue* value);

typedef void (*SclshOpaqueValueDestructor)(void* data);

typedef struct s_SclshListData {
    size_t count;  // Number of items in the list
    SclshValue* items[];  // Array of pointers to SclshValue
} SclshListData;



#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__VALUE_H
